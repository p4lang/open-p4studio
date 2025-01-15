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
Thrift SAI interface Mirror tests
"""

import socket

#from switch import *
from ptf.mask import Mask
import sai_base_test
from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *
from switchsai_thrift.sai_headers import  *
from switch_utils import *
from erspan3 import *


this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.pktpy_utils import pktpy_skip # noqa pylint: disable=wrong-import-position


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class spanmonitor(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This performs Local mirroring
    We set port2 traffic to be monitored(both ingress and egress) on port1
    We send a packet from port 2 to port 3
    We expect the same packet on port 1 which is a mirror packet
    '''
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        monitor_port=port1
        source_port=port2
        mac3='00:00:00:00:00:33'
        mac2='00:00:00:00:00:22'
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        mac_action = SAI_PACKET_ACTION_FORWARD
        vlan_remote_id = 2

        # Put ports under test in VLAN 2
        vlan_remote_oid = sai_thrift_create_vlan(self.client, vlan_remote_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port3, SAI_VLAN_TAGGING_MODE_TAGGED)

        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac3, port3, mac_action)
        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac2, port2, mac_action)

        # Set PVID
        attr_value = sai_thrift_attribute_value_t(u16=vlan_remote_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=None,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        # SWI-2058 workaround
        spanid_egress=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=None,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)

        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attrb_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid_egress]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attrb_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='10.0.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=2,
                                ip_id=101,
                                ip_ttl=64)

        exp_pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='10.0.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=2,
                                ip_id=101,
                                ip_ttl=64)

        pkt2 = simple_tcp_packet(eth_dst='00:00:00:00:00:22',
                                eth_src='00:00:00:00:00:33',
                                ip_dst='10.0.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=2,
                                ip_id=101,
                                ip_ttl=64,
                                pktlen=104)

        exp_pkt2 = simple_tcp_packet(eth_dst='00:00:00:00:00:22',
                                eth_src='00:00:00:00:00:33',
                                ip_dst='10.0.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=2,#use vlan_vid field if packets are expected to be monitored on client side otherwise not needed 
                                ip_id=101,
                                ip_ttl=64,
                                pktlen=104)

        m=Mask(exp_pkt2)
        m.set_do_not_care_scapy(ptf.packet.IP,'id')
        m.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        try:
            # in tuple: 0 is device number, 2 is port number
            # this tuple uniquely identifies a port
            # for ingress mirroring
            print "Checking INGRESS Local Mirroring"
            print "Sending packet port 2 -> port 3 (00:00:00:00:00:22 -> 00:00:00:00:00:33)"
            send_packet(self,switch_ports[1], pkt)
            verify_packets(self, exp_pkt, ports=[switch_ports[0],switch_ports[2]])
            # for egress mirroring
            print "Checking EGRESS Local Mirroring"
            print "Sending packet port 3 -> port 2 (00:00:00:00:00:33 -> 00:00:00:00:00:22)"
            send_packet(self, switch_ports[2], pkt2)
            verify_each_packet_on_each_port(self, [m,pkt2], ports=[switch_ports[0],switch_ports[1]])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac3, port3)
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac2, port2)

            # Remove ports from mirror destination
            attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=0,object_id_list=[spanid]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attrb_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attrb_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            # Now you can remove destination
            self.client.sai_thrift_remove_mirror_session(spanid)
            self.client.sai_thrift_remove_mirror_session(spanid_egress)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            # Remove ports from VLAN 2
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan(vlan_remote_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class spanaclmonitor(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This performs Local mirroring
    We set port2 traffic to be monitored(both ingress and egress) on port1
    We send a packet from port 2 to port 3
    We expect the same packet on port 1 which is a mirror packet
    '''
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        monitor_port=port1
        source_port=port2
        mac3='00:00:00:00:00:33'
        mac2='00:00:00:00:00:22'
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        mac_action = SAI_PACKET_ACTION_FORWARD
        vlan_id = 1
        vlan_remote_id = 2
        vlan_oid = switch.default_vlan.oid
        vlan_remote_oid = sai_thrift_create_vlan(self.client, vlan_remote_id)
        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac3, port3, mac_action)
        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac2, port2, mac_action)

        # Put ports under test in VLAN 2
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port3, SAI_VLAN_TAGGING_MODE_TAGGED)

        # Set PVID
        attr_value = sai_thrift_attribute_value_t(u16=vlan_remote_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=vlan_oid,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print spanid
        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1 -[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP
        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = None
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = "172.16.0.1"
        ip_dst_mask = "255.255.255.0"
        ip_proto = 6
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = 4000
        dst_l4_port = 5000
        ingress_mirror_id = spanid
        egress_mirror_id = None
        range_list = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to port2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='172.16.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=2,
                                ip_id=101,
                                ip_ttl=64,
                                tcp_sport=4000,
                                tcp_dport=5000)

        exp_pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='172.16.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=2,
                                ip_id=101,
                                ip_ttl=64,
                                tcp_sport=4000,
                                tcp_dport=5000)

        pkt2 = simple_tcp_packet(eth_dst='00:00:00:00:00:22',
                                eth_src='00:33:33:33:33:33',
                                ip_dst='172.16.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=2,
                                ip_id=101,
                                ip_ttl=64,
                                pktlen=104,
                                tcp_sport=4000,
                                tcp_dport=5000)

        exp_pkt2 = simple_tcp_packet(eth_dst='00:00:00:00:00:22',
                                eth_src='00:33:33:33:33:33',
                                ip_dst='172.16.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=2,#use vlan_vid field if packets are expected to be monitored on client side otherwise not needed
                                ip_id=101,
                                ip_ttl=64,
                                pktlen=104,
                                tcp_sport=4000,
                                tcp_dport=5000)

        m=Mask(exp_pkt2)
        m.set_do_not_care_scapy(ptf.packet.IP,'id')
        m.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        try:
            # in tuple: 0 is device number, 2 is port number
            # this tuple uniquely identifies a port
            # for ingress mirroring
            print "Checking INGRESS Local Mirroring"
            print "Sending packet port 2 -> port 3 (00:00:00:00:00:22 -> 00:00:00:00:00:33)"
            send_packet(self,switch_ports[1], pkt)
            verify_packets(self, exp_pkt, [switch_ports[0],switch_ports[2]])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac3, port3)
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac2, port2)

            # unbind this ACL table from port2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)

            # Now you can remove destination
            self.client.sai_thrift_remove_mirror_session(spanid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            # Remove ports from VLAN 2
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan(vlan_remote_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror-acl')
class erspanAclLagMonitor(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This test performs erspan monitoring on Lag/Port
    We send traffic from port1 to port2. And update the mirror session
    monitor port. We test the following cases
    *Mirror Monitor Port: LAG1 (Port3, Port4)
    *Mirror Monitor Port: LAG1 (Port3)
    *Mirror Monitor Port: LAG1 (NULL) [No mirroring]
    *Mirror Monitor Port: LAG1 (Port3)
    *Mirror Monitor Port: LAG1 (Port3, Port4)
    *Mirror Monitor Port: LAG1 -> Port5
    *Mirror Monitor Port: Port5 -> LAG1 (Port3, Port4)
    *Mirror Monitor Port: LAG1 -> LAG2 (Port6, Port7)
    '''
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        port5 = port_list[4]
        port6 = port_list[5]
        port7 = port_list[6]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        dst_ip_addr = '172.16.1.1'
        dst_ip_addr_subnet = '172.16.1.0'
        dst_ip_mask = '255.255.255.0'
        neigh_dmac = '00:11:22:33:44:55'

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        lag_id = self.client.sai_thrift_create_lag([])
        lag_id2 = self.client.sai_thrift_create_lag([])
        lag_member_id21 = sai_thrift_create_lag_member(self.client, lag_id2, port6)
        lag_member_id22 = sai_thrift_create_lag_member(self.client, lag_id2, port7)

        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)
        rif_id3 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, lag_id, 0, v4_enabled, v6_enabled, mac)
        rif_id4 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port5, 0, v4_enabled, v6_enabled, mac)
        rif_id5 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, lag_id2, 0, v4_enabled, v6_enabled, mac)

        sai_thrift_create_neighbor(self.client, addr_family, rif_id2, dst_ip_addr, neigh_dmac)
        nhop = sai_thrift_create_nhop(self.client, addr_family, dst_ip_addr, rif_id2)
        sai_thrift_create_route(self.client, vr_id, addr_family, dst_ip_addr_subnet, dst_ip_mask, nhop)

        monitor_port=lag_id
        source_port=port1
        mirror_type=SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE
        src_mac='00:00:00:00:11:22'
        dst_mac='00:00:00:00:11:33'
        encap_type=SAI_ERSPAN_ENCAPSULATION_TYPE_MIRROR_L3_GRE_TUNNEL
        ip_version=0x4
        tos=0
        ttl=64
        gre_type=0x22EB
        src_ip='17.18.19.0'
        dst_ip='33.19.20.0'
        addr_family=0
        vlan=0x1
        vlan_pri=0x6
        vlan_tpid=0x8100

        erspanid=sai_thrift_create_mirror_session(self.client,
                          mirror_type=mirror_type,
                          port=monitor_port,
                          vlan=vlan,
                          vlan_priority=vlan_pri,
                          vlan_tpid=vlan_tpid,
                          vlan_header_valid=False,
                          src_mac=src_mac,
                          dst_mac=dst_mac,
                          src_ip=src_ip,
                          dst_ip=dst_ip,
                          encap_type=encap_type,
                          iphdr_version=ip_version,
                          ttl=ttl,
                          tos=tos,
                          gre_type=gre_type)

        #Add lag member to monitor port
        lag_member_id1 = sai_thrift_create_lag_member(self.client, lag_id, port3)
        lag_member_id2 = sai_thrift_create_lag_member(self.client, lag_id, port4)

        # setup ACL to Mirror on Source IP
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            SAI_IP_ADDR_FAMILY_IPV4,
            ip_src=ip_src)

        acl_mirror_entry = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action=None, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
            ip_src=ip_src, ip_src_mask=ip_src_mask, ingress_mirror = erspanid)

        # bind this ACL table to port1
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:00:00:00:00:22',
                                ip_src='192.168.0.1',
                                ip_dst='172.16.1.2',
                                ip_id=101,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst=neigh_dmac,
                                eth_src=router_mac,
                                ip_src='192.168.0.1',
                                ip_dst='172.16.1.2',
                                ip_id=101,
                                ip_ttl=63)
        exp_inner_pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:00:00:00:00:22',
                                ip_src='192.168.0.1',
                                ip_dst='172.16.1.2',
                                ip_id=101,
                                ip_ttl=64)
        exp_mirrored_pkt = ipv4_erspan_pkt(
            eth_src='00:00:00:00:11:22',
            eth_dst='00:00:00:00:11:33',
            ip_src='17.18.19.0',
            ip_dst='33.19.20.0',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x2,
            version=2,
            mirror_id=1,
            inner_frame=exp_inner_pkt)

        exp_mask_mirrored_pkt=Mask(exp_mirrored_pkt)
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'tos')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'ihl')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'len')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'frag')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'ttl')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'flags')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.GRE,'proto')

        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'span_id')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'direction')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'timestamp')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'sgt_other')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'version')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'vlan')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'priority')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'truncated')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'unknown2')

        try:
            print "Sending packet port 1 -> port 2. Mirror Pkt expected on Lag (Port3/Port4)"
            send_packet(self, switch_ports[0], pkt)
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet_any_port(self, exp_mask_mirrored_pkt, [switch_ports[2], switch_ports[3]])
            print "PASS"
            print "Removing the lag member (port3) from lag"
            sai_thrift_remove_lag_member(self.client, lag_member_id1)
            send_packet(self, switch_ports[0], pkt)
            print "Sending packet port 1 -> port 2. Mirror Pkt expected on Lag (Port4)"
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet_any_port(self, exp_mask_mirrored_pkt, [switch_ports[3]])
            print "PASS"
            print "Removing lag member ((port4) from lag"
            sai_thrift_remove_lag_member(self.client, lag_member_id2)
            print "Sending packet port 1 -> port 2. Verify no mirrored packet"
            send_packet(self, switch_ports[0], pkt)
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_no_other_packets(self)
            print "PASS"
            print "Adding back lag members"
            print "Adding port 3 to lag"
            lag_member_id1 = sai_thrift_create_lag_member(self.client, lag_id, port3)
            print "Sending packet port 1 -> port 2. Mirror Pkt expected on Lag (Port3)"
            send_packet(self, switch_ports[0], pkt)
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet_any_port(self, exp_mask_mirrored_pkt, [switch_ports[2]])
            print "PASS"
            print "Adding port 4 to lag"
            lag_member_id2 = sai_thrift_create_lag_member(self.client, lag_id, port4)
            print "Sending packet port 1 -> port 2. Mirror Pkt expected on Lag (Port3/Port4)"
            send_packet(self, switch_ports[0], pkt)
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet_any_port(self, exp_mask_mirrored_pkt, [switch_ports[2], switch_ports[3]])
            print "PASS"

            print "Set Monitor port as Port5 (Monitor Port: LAG -> Regular Port Case)"
            attr_value = sai_thrift_attribute_value_t(oid=port5)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_MONITOR_PORT, value=attr_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)

            print "Sending packet port 1 -> port 2. Mirror Pkt expected on Port5"
            send_packet(self, switch_ports[0], pkt)
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet_any_port(self, exp_mask_mirrored_pkt, [switch_ports[4]])
            print "PASS"

            print "Set Monitor port as Lag (Monitor Port: Regular Port -> Lag Case)"
            attr_value = sai_thrift_attribute_value_t(oid=lag_id)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_MONITOR_PORT, value=attr_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)

            print "Sending packet port 1 -> port 2. Mirror Pkt expected on Lag(Port3/Port4)"
            send_packet(self, switch_ports[0], pkt)
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet_any_port(self, exp_mask_mirrored_pkt, [switch_ports[2], switch_ports[3]])
            print "PASS"

            print "Set Monitor port as Lag (Monitor Port: Lag -> Lag' Case)"
            attr_value = sai_thrift_attribute_value_t(oid=lag_id2)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_MONITOR_PORT, value=attr_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)

            print "Sending packet port 1 -> port 2. Mirror Pkt expected on Lag(Port6/Port7)"
            send_packet(self, switch_ports[0], pkt)
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet_any_port(self, exp_mask_mirrored_pkt, [switch_ports[5], switch_ports[6]])
            print "PASS"

            print "Change DST MAC Addr for erspan ecnap"
            attr_value = sai_thrift_attribute_value_t(mac='00:00:00:00:11:77')
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_DST_MAC_ADDRESS, value=attr_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)

            new_exp_mirrored_pkt = ipv4_erspan_pkt(
                eth_src='00:00:00:00:11:22',
                eth_dst='00:00:00:00:11:77',
                ip_src='17.18.19.0',
                ip_dst='33.19.20.0',
                ip_id=0,
                ip_ttl=64,
                ip_flags=0x2,
                version=2,
                mirror_id=1,
                inner_frame=exp_inner_pkt)

            exp_mask_mirrored_pkt=Mask(new_exp_mirrored_pkt)
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'tos')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'frag')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'ttl')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'flags')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'chksum')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.GRE,'proto')

            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'span_id')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'direction')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'timestamp')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'sgt_other')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'version')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'vlan')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'priority')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'truncated')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'unknown2')

            print "Sending packet port 1 -> port 2. Mirror Pkt expected on Lag(Port6/Port7) with Dest Mac:00:00:00:00:11:77"
            send_packet(self, switch_ports[0], pkt)
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet_any_port(self, exp_mask_mirrored_pkt, [switch_ports[5], switch_ports[6]])
            print "PASS"

        finally:
            sai_thrift_remove_route(self.client, vr_id, addr_family, dst_ip_addr_subnet, dst_ip_mask, nhop)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id2, dst_ip_addr, neigh_dmac)
            self.client.sai_thrift_remove_next_hop(nhop)

            # unbind this ACL table from port1s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_mirror_entry)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            # Remove mirror session
            self.client.sai_thrift_remove_mirror_session(erspanid)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_router_interface(rif_id3)
            self.client.sai_thrift_remove_router_interface(rif_id4)
            self.client.sai_thrift_remove_router_interface(rif_id5)

            sai_thrift_remove_lag_member(self.client, lag_member_id1)
            sai_thrift_remove_lag_member(self.client, lag_member_id2)
            sai_thrift_remove_lag_member(self.client, lag_member_id21)
            sai_thrift_remove_lag_member(self.client, lag_member_id22)
            self.client.sai_thrift_remove_lag(lag_id)
            self.client.sai_thrift_remove_lag(lag_id2)

            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class erspanmonitor(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This test performs erspan monitoring
    From port2(source port) we send traffic to port 3
    erspan mirror packets are expected on port 1(monitor port)
    '''
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        mac3='00:00:00:00:00:33'
        mac2='00:00:00:00:00:22'
        monitor_port=port1
        source_port=port2
        mirror_type=SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE
        vlan=0x2
        vlan_tpid=0x8100
        vlan_pri=0x6
        src_mac='00:00:00:00:11:22'
        dst_mac='00:00:00:00:11:33'
        encap_type=SAI_ERSPAN_ENCAPSULATION_TYPE_MIRROR_L3_GRE_TUNNEL
        ip_version=0x4
        tos=0x3c
        ttl=0xf0
        gre_type=0x22EB
        src_ip='17.18.19.0'
        dst_ip='33.19.20.0'
        addr_family=0
        vlan_remote_id = 3
        mac_action = SAI_PACKET_ACTION_FORWARD

        # Put ports under test in VLAN 3
        vlan_remote_oid = sai_thrift_create_vlan(self.client, vlan_remote_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port3, SAI_VLAN_TAGGING_MODE_TAGGED)

        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac3, port3, mac_action)
        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac2, port2, mac_action)

        # Set PVID
        attr_value = sai_thrift_attribute_value_t(u16=vlan_remote_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        erspanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=vlan,vlan_priority=vlan_pri,vlan_tpid=vlan_tpid,vlan_header_valid=True,src_mac=src_mac,dst_mac=dst_mac,src_ip=src_ip,dst_ip=dst_ip,encap_type=encap_type,iphdr_version=ip_version,ttl=ttl,tos=tos,gre_type=gre_type)
        # SWI-2058 workaround
        erspanid_egress=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=vlan,vlan_priority=vlan_pri,vlan_tpid=vlan_tpid,vlan_header_valid=True,src_mac=src_mac,dst_mac=dst_mac,src_ip=src_ip,dst_ip=dst_ip,encap_type=encap_type,iphdr_version=ip_version,ttl=ttl,tos=tos,gre_type=gre_type)

        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[erspanid]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attrb_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[erspanid_egress]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attrb_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='10.0.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=3,
                                ip_id=101,
                                ip_ttl=64)

        pkt2 = simple_tcp_packet(eth_dst='00:00:00:00:00:22',
                                eth_src='00:00:00:00:00:33',
                                dl_vlan_enable=True,
                                vlan_vid=3,
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                ip_ttl=64)

        pkt3 = simple_tcp_packet(eth_dst='00:00:00:00:00:22',
                                eth_src='00:00:00:00:00:33',
                                dl_vlan_enable=True,
                                vlan_vid=3,
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                ip_ttl=64)

        exp_pkt1= ipv4_erspan_pkt(pktlen=142,
                                    eth_dst='00:00:00:00:11:33',
                                    eth_src='00:00:00:00:11:22',
                                    dl_vlan_enable=True,
                                    vlan_vid=2,
                                    ip_id=0,
                                    ip_ttl=240,
                                    ip_tos=0x3c,
                                    ip_ihl=5,
                                    ip_src='17.18.19.0',
                                    ip_dst='33.19.20.0',
                                    version=2,
                                    mirror_id=(erspanid & 0x3FFFFFFF),
                                    inner_frame=pkt
                                    )

        exp_pkt2= ipv4_erspan_pkt(pktlen=142,
                                    eth_dst='00:00:00:00:11:33',
                                    eth_src='00:00:00:00:11:22',
                                    dl_vlan_enable=True,
                                    vlan_vid=2,
                                    ip_id=0,
                                    ip_ttl=240,
                                    ip_tos=0x3c,
                                    ip_ihl=5,
                                    ip_src='17.18.19.0',
                                    ip_dst='33.19.20.0',
                                    version=2,
                                    mirror_id=(erspanid_egress & 0x3FFFFFFF),
                                    inner_frame=pkt3
                                    )
        m1=Mask(exp_pkt1)
        m1.set_do_not_care_scapy(ptf.packet.IP,'tos')
        m1.set_do_not_care_scapy(ptf.packet.IP,'frag')
        m1.set_do_not_care_scapy(ptf.packet.IP,'flags')
        m1.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        m1.set_do_not_care_scapy(ptf.packet.GRE,'proto')

        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'span_id')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'timestamp')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'sgt_other')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'direction')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'version')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'vlan')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'priority')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'truncated')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'unknown2')

        m2=Mask(exp_pkt2)
        m2.set_do_not_care_scapy(ptf.packet.IP,'tos')
        m2.set_do_not_care_scapy(ptf.packet.IP,'frag')
        m2.set_do_not_care_scapy(ptf.packet.IP,'flags')
        m2.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        m2.set_do_not_care_scapy(ptf.packet.GRE,'proto')

        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'span_id')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'timestamp')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'sgt_other')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'direction')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'version')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'vlan')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'priority')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'truncated')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'unknown2')

        n=Mask(pkt2)
        n.set_do_not_care_scapy(ptf.packet.IP,'len')
        n.set_do_not_care_scapy(ptf.packet.IP,'chksum')

        try:
            # in tuple: 0 is device number, 2 is port number
            # this tuple uniquely identifies a port
            # for ingress mirroring
            print "Checking INGRESS ERSPAN Mirroring"
            print "Sending packet port 2 -> port 3 (00:00:00:00:00:22 -> 00:00:00:00:00:33)"
            send_packet(self,switch_ports[1], pkt)
            verify_each_packet_on_each_port(self, [m1,pkt], ports=[switch_ports[0],switch_ports[2]])#FIXME need to properly implement
            # for egress mirroring
            print "Checking EGRESS ERSPAN Mirroring"
            print "Sending packet port 3 -> port 2 (00:00:00:00:00:33 -> 00:00:00:00:00:22)"
            send_packet(self, switch_ports[2], pkt2)
            verify_each_packet_on_each_port(self, [pkt2,m2], ports=[switch_ports[1],switch_ports[0]])#FIXME need to properly implement
        finally:
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac2, port2)
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac3, port3)

            # Remove ports from mirror destination
            attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=0,object_id_list=[erspanid]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attrb_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attrb_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            # Now you can remove destination
            self.client.sai_thrift_remove_mirror_session(erspanid)
            self.client.sai_thrift_remove_mirror_session(erspanid_egress)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            # Remove ports from VLAN 3
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan(vlan_remote_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class erspan_novlan_monitor(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This test performs erspan monitoring
    From port2(source port) we send traffic to port 3
    erspan mirror packets are expected on port 1(monitor port)
    '''
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        mac3='00:00:00:00:00:33'
        mac2='00:00:00:00:00:22'
        monitor_port=port1
        source_port=port2
        mirror_type=SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE
        src_mac='00:00:00:00:11:22'
        dst_mac='00:00:00:00:11:33'
        encap_type=SAI_ERSPAN_ENCAPSULATION_TYPE_MIRROR_L3_GRE_TUNNEL
        ip_version=0x4
        tos=0x3c
        ttl=0xf0
        gre_type=0x88be
        src_ip='17.18.19.0'
        dst_ip='33.19.20.0'
        addr_family=0
        vlan_remote_id = 3
        mac_action = SAI_PACKET_ACTION_FORWARD

        # Put ports under test in VLAN 3
        vlan_remote_oid = sai_thrift_create_vlan(self.client, vlan_remote_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port3, SAI_VLAN_TAGGING_MODE_TAGGED)

        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac3, port3, mac_action)
        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac2, port2, mac_action)

        # Set PVID
        attr_value = sai_thrift_attribute_value_t(u16=vlan_remote_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        erspanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=None,vlan_priority=None,vlan_tpid=None,vlan_header_valid=False,src_mac=src_mac,dst_mac=dst_mac,src_ip=src_ip,dst_ip=dst_ip,encap_type=encap_type,iphdr_version=ip_version,ttl=ttl,tos=tos,gre_type=gre_type)
        # SWI-2058 workaround
        erspanid_egress=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=None,vlan_priority=None,vlan_tpid=None,vlan_header_valid=False,src_mac=src_mac,dst_mac=dst_mac,src_ip=src_ip,dst_ip=dst_ip,encap_type=encap_type,iphdr_version=ip_version,ttl=ttl,tos=tos,gre_type=gre_type)

        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[erspanid]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attrb_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[erspanid_egress]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attrb_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='10.0.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=3,
                                ip_id=101,
                                ip_ttl=64)

        pkt2 = simple_tcp_packet(eth_dst='00:00:00:00:00:22',
                                eth_src='00:00:00:00:00:33',
                                dl_vlan_enable=True,
                                vlan_vid=3,
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                ip_ttl=64)

        pkt3 = simple_tcp_packet(eth_dst='00:00:00:00:00:22',
                                eth_src='00:00:00:00:00:33',
                                dl_vlan_enable=True,
                                vlan_vid=3,
                                ip_dst='10.0.0.1',
                                ip_id=101,
                                ip_ttl=64)

        exp_pkt1= ipv4_erspan_pkt(pktlen=142,
                                    eth_dst='00:00:00:00:11:33',
                                    eth_src='00:00:00:00:11:22',
                                    ip_id=0,
                                    ip_ttl=240,
                                    ip_tos=0x3c,
                                    ip_ihl=5,
                                    ip_src='17.18.19.0',
                                    ip_dst='33.19.20.0',
                                    version=1,
                                    sgt_other=0,
                                    mirror_id=(erspanid & 0x3FFFFFFF),
                                    inner_frame=pkt
                                    )

        exp_pkt2= ipv4_erspan_pkt(pktlen=142,
                                    eth_dst='00:00:00:00:11:33',
                                    eth_src='00:00:00:00:11:22',
                                    ip_id=0,
                                    ip_ttl=240,
                                    ip_tos=0x3c,
                                    ip_ihl=5,
                                    ip_src='17.18.19.0',
                                    ip_dst='33.19.20.0',
                                    version=1,
                                    sgt_other=0,
                                    mirror_id=(erspanid_egress & 0x3FFFFFFF),
                                    inner_frame=pkt3
                                    )

        m1=Mask(exp_pkt1)
        m1.set_do_not_care_scapy(ptf.packet.IP,'tos')
        m1.set_do_not_care_scapy(ptf.packet.IP,'frag')
        m1.set_do_not_care_scapy(ptf.packet.IP,'ttl')
        m1.set_do_not_care_scapy(ptf.packet.IP,'flags')
        m1.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        m1.set_do_not_care_scapy(ptf.packet.GRE,'proto')

        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'span_id')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'direction')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'version')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'vlan')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'priority')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'truncated')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'unknown2')

        m2=Mask(exp_pkt2)
        m2.set_do_not_care_scapy(ptf.packet.IP,'tos')
        m2.set_do_not_care_scapy(ptf.packet.IP,'frag')
        m2.set_do_not_care_scapy(ptf.packet.IP,'ttl')
        m2.set_do_not_care_scapy(ptf.packet.IP,'flags')
        m2.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        m2.set_do_not_care_scapy(ptf.packet.GRE,'proto')

        m2.set_do_not_care_scapy(ptf.packet.ERSPAN, 'span_id')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN, 'direction')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN, 'version')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN, 'vlan')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN, 'priority')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN, 'truncated')
        m2.set_do_not_care_scapy(ptf.packet.ERSPAN, 'unknown2')

        n=Mask(pkt2)
        n.set_do_not_care_scapy(ptf.packet.IP,'len')
        n.set_do_not_care_scapy(ptf.packet.IP,'chksum')

        try:
            # in tuple: 0 is device number, 2 is port number
            # this tuple uniquely identifies a port
            # for ingress mirroring
            print "Checking INGRESS ERSPAN Mirroring"
            print "Sending packet port 2 -> port 3 (00:00:00:00:00:22 -> 00:00:00:00:00:33)"
            send_packet(self, switch_ports[1], pkt)
            verify_each_packet_on_each_port(self, [m1,pkt], ports=[switch_ports[0],switch_ports[2]])#FIXME need to properly implement
            # for egress mirroring
            print "Checking EGRESS ERSPAN Mirroring"
            print "Sending packet port 3 -> port 2 (00:00:00:00:00:33 -> 00:00:00:00:00:22)"
            send_packet(self, switch_ports[2], pkt2)
            verify_each_packet_on_each_port(self, [pkt2,m2], ports=[switch_ports[1],switch_ports[0]])#FIXME need to properly implement
        finally:
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac2, port2)
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac3, port3)

            # Remove ports from mirror destination
            attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=0,object_id_list=[erspanid]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attrb_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attrb_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            # Now you can remove destination
            self.client.sai_thrift_remove_mirror_session(erspanid)
            self.client.sai_thrift_remove_mirror_session(erspanid_egress)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            # Remove ports from VLAN 3
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan(vlan_remote_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class erspanvlanmonitor(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This test performs erspan monitoring
    From port2(source port) we send traffic to port 3
    erspan mirror packets are expected on port 1(monitor port)
    '''
    def verify_erspan_packet(self, exp_pkt, port):
        m1=Mask(exp_pkt)
        m1.set_do_not_care_scapy(ptf.packet.IP,'tos')
        m1.set_do_not_care_scapy(ptf.packet.IP,'frag')
        m1.set_do_not_care_scapy(ptf.packet.IP,'ttl')
        m1.set_do_not_care_scapy(ptf.packet.IP,'flags')
        m1.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        m1.set_do_not_care_scapy(ptf.packet.GRE,'proto')

        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'span_id')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'direction')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'version')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'vlan')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'priority')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'truncated')
        m1.set_do_not_care_scapy(ptf.packet.ERSPAN, 'unknown2')
        verify_packet(self, m1, port)

    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        mac3='00:00:00:00:00:33'
        mac2='00:00:00:00:00:22'
        monitor_port=port1
        source_port=port2
        mirror_type=SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE
        vlan=0x1
        vlan_tpid=0x08100
        vlan_pri=0x6
        src_mac='00:00:00:00:11:22'
        dst_mac='00:00:00:00:11:33'
        encap_type=SAI_ERSPAN_ENCAPSULATION_TYPE_MIRROR_L3_GRE_TUNNEL
        ip_version=0x4
        tos=0x3c
        ttl=64
        gre_type=0x88be
        src_ip='17.18.19.0'
        dst_ip='33.19.20.0'
        src_ip1='41.42.43.44'
        dst_ip1='45.46.47.48'
        src_mac1='00:00:00:00:22:33'
        dst_mac1='00:00:00:00:22:44'
        addr_family=0
        vlan_remote_id = 3
        vlan_oid = switch.default_vlan.oid
        vlan_remote_oid = sai_thrift_create_vlan(self.client, vlan_remote_id)
        mac_action = SAI_PACKET_ACTION_FORWARD

        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac3, port3, mac_action)
        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac2, port2, mac_action)

        # Put ports under test in VLAN 3
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port3, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)

        # Set PVID
        attr_value = sai_thrift_attribute_value_t(u16=vlan_remote_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        erspanid=sai_thrift_create_mirror_session(
                          self.client,
                          mirror_type=mirror_type,
                          port=monitor_port,
                          vlan=10,
                          vlan_priority=vlan_pri,
                          vlan_tpid=vlan_tpid,
                          vlan_header_valid=True,
                          src_mac=src_mac,
                          dst_mac=dst_mac,
                          src_ip=src_ip,
                          dst_ip=dst_ip,
                          encap_type=encap_type,
                          iphdr_version=ip_version,
                          ttl=ttl,
                          tos=tos,
                          gre_type=gre_type)

        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[erspanid]))

        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attrb_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='172.16.0.1',
                                ip_id=101,
                                dl_vlan_enable=True,
                                vlan_vid=3,
                                ip_ttl=64)

        exp_mirrored_pkt = ipv4_erspan_pkt(
            eth_src='00:00:00:00:11:22',
            eth_dst='00:00:00:00:11:33',
            ip_src='17.18.19.0',
            ip_dst='33.19.20.0',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x2,
            version=1,
            dl_vlan_enable=True,
            vlan_vid=10,
            mirror_id=1,
            ip_tos=0x3c,
            inner_frame=pkt)

        try:
            # in tuple: 0 is device number, 2 is port number
            # this tuple uniquely identifies a port
            # for ingress mirroring
            print "Checking INGRESS ERSPAN Mirroring"
            print "Sending packet port 2 -> port 3 (00:00:00:00:00:22 -> 00:00:00:00:00:33). Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            #verify_packet(self, m1, switch_ports[0])
            self.verify_erspan_packet(exp_mirrored_pkt, switch_ports[0])
            print "PASS"

            print "Setting mirror session src ip address: {}".format(src_ip1)
            addr = sai_thrift_ip_t(ip4=src_ip1)
            src_ip_addr = sai_thrift_ip_address_t(addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
            attrb_value = sai_thrift_attribute_value_t(ipaddr=src_ip_addr)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_SRC_IP_ADDRESS, value=attrb_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)
            exp_mirrored_pkt[IP].src=src_ip1
            print "Sending packet port 2 -> port 3. Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            self.verify_erspan_packet(exp_mirrored_pkt, switch_ports[0])
            print "PASS"

            print "Setting mirror session dst ip address: {}".format(dst_ip1)
            addr = sai_thrift_ip_t(ip4=dst_ip1)
            dst_ip_addr = sai_thrift_ip_address_t(addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
            attrb_value = sai_thrift_attribute_value_t(ipaddr=dst_ip_addr)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_DST_IP_ADDRESS, value=attrb_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)
            exp_mirrored_pkt[IP].dst=dst_ip1
            print "Sending packet port 2 -> port 3. Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            self.verify_erspan_packet(exp_mirrored_pkt, switch_ports[0])
            print "PASS"

            print "Setting mirror session src mac address: {}".format(src_mac1)
            attrb_value = sai_thrift_attribute_value_t(mac=src_mac1)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_SRC_MAC_ADDRESS, value=attrb_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)
            exp_mirrored_pkt[Ether].src=src_mac1
            print "Sending packet port 2 -> port 3. Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            self.verify_erspan_packet(exp_mirrored_pkt, switch_ports[0])
            print "PASS"

            print "Setting mirror session dst mac address: {}".format(dst_mac1)
            attrb_value = sai_thrift_attribute_value_t(mac=dst_mac1)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_DST_MAC_ADDRESS, value=attrb_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)
            exp_mirrored_pkt[Ether].dst=dst_mac1
            print "Sending packet port 2 -> port 3. Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            self.verify_erspan_packet(exp_mirrored_pkt, switch_ports[0])
            print "PASS"

            vlan_id1 = 20
            print "Setting mirror session VLAN ID: {}".format(vlan_id1)
            attrb_value = sai_thrift_attribute_value_t(u16=vlan_id1)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_ID, value=attrb_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)
            exp_mirrored_pkt[Dot1Q].vlan=vlan_id1
            print "Sending packet port 2 -> port 3. Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            self.verify_erspan_packet(exp_mirrored_pkt, switch_ports[0])
            print "PASS"

            vlan_prio1 = 2
            print "Setting mirror session VLAN PRI: {}".format(vlan_prio1)
            attrb_value = sai_thrift_attribute_value_t(u8=vlan_prio1)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_PRI, value=attrb_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)
            exp_mirrored_pkt[Dot1Q].prio=vlan_prio1
            print "Sending packet port 2 -> port 3. Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            self.verify_erspan_packet(exp_mirrored_pkt, switch_ports[0])
            print "PASS"

            vlan_header_valid=False
            exp_mirrored_pkt2 = ipv4_erspan_pkt(
                eth_src=src_mac1,
                eth_dst=dst_mac1,
                ip_src=src_ip1,
                ip_dst=dst_ip1,
                ip_id=0,
                ip_ttl=64,
                ip_flags=0x2,
                version=1,
                dl_vlan_enable=False,
                mirror_id=1,
                ip_tos=0x3c,
                inner_frame=pkt)
            print "Setting Vlan header valid for mirror session to : {}".format(vlan_header_valid)
            attrb_value = sai_thrift_attribute_value_t(booldata=vlan_header_valid)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_HEADER_VALID, value=attrb_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)
            print "Sending packet port 2 -> port 3. Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            self.verify_erspan_packet(exp_mirrored_pkt2, switch_ports[0])
            print "PASS"

            vlan_header_valid=True
            print "Setting Vlan header valid for mirror session to : {}".format(vlan_header_valid)
            attrb_value = sai_thrift_attribute_value_t(booldata=vlan_header_valid)
            attr = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_HEADER_VALID, value=attrb_value)
            self.client.sai_thrift_set_mirror_attribute(erspanid, attr)
            print "Sending packet port 2 -> port 3. Mirrored pkt expected on port 1"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            self.verify_erspan_packet(exp_mirrored_pkt, switch_ports[0])
            print "PASS"

        finally:
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac2, port2)
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac3, port3)

            # Remove ports from mirror destination
            attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[SAI_NULL_OBJECT_ID]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attrb_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            # Now you can remove destination
            self.client.sai_thrift_remove_mirror_session(erspanid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            # Remove ports from VLAN 3
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_remote_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class IngressLocalMirrorTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 1 -> ptf_intf 2, ptf_intf 3 (local mirror)"
        print "Sending packet ptf_intf 2 -> ptf_intf 1, ptf_intf 3 (local mirror)"

        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        # setup local mirror session
        mirror_type = SAI_MIRROR_SESSION_TYPE_LOCAL
        monitor_port = port3
        print "Create mirror session: mirror_type = SAI_MIRROR_TYPE_LOCAL, monitor_port = ptf_intf 3 "
        ingress_mirror_id = sai_thrift_create_mirror_session(self.client,
            mirror_type,
            monitor_port,
            None, None, None,
            None, None, None,
            None, None, None,
            None, None, None,
            None)
        print "ingress_mirror_id = %d" %ingress_mirror_id

        attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[ingress_mirror_id]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        try:
            #assert ingress_mirror_id > 0, 'ingress_mirror_id is <= 0'

            pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                ip_id=102,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_id=102,
                ip_ttl=64,
                pktlen=104)

            print '#### Sending 00:22:22:22:22:22 | 00:11:11:11:11:11 | 10.0.0.1 | 192.168.0.1 | @ ptf_intf 1 ####'
            send_packet(self,switch_ports[1], str(pkt))
            verify_each_packet_on_each_port(self, [exp_pkt, pkt],  [switch_ports[2], switch_ports[3]] )

            time.sleep(1)

            pkt = simple_tcp_packet(eth_dst=mac1,
                eth_src=mac2,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                vlan_vid=10,
                dl_vlan_enable=True,
                ip_id=102,
                ip_ttl=64,
                pktlen=104)
            exp_pkt = simple_tcp_packet(eth_dst=mac1,
                eth_src=mac2,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                ip_id=102,
                ip_ttl=64,
                pktlen=100)

            print '#### Sending 00:11:11:11:11:11 | 00:22:22:22:22:22 | 10.0.0.1 | 192.168.0.1 | @ ptf_intf 2 ####'
            send_packet(self, switch_ports[2], str(pkt))
            verify_each_packet_on_each_port(self, [exp_pkt, pkt], [switch_ports[1],switch_ports[3]])

        finally:
            attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=0,object_id_list=[]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            self.client.sai_thrift_remove_mirror_session(ingress_mirror_id)

            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)


@pktpy_skip  # TODO bf-pktpy
@disabled
class IngressRSpanMirrorTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 1 -> ptf_intf 2, ptf_intf 3 (rspan mirror)"

        switch_init(self.client)
        vlan_id = 10
        vlan_remote_id = 110
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_remote_oid = sai_thrift_create_vlan(self.client, vlan_remote_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        # setup remote mirror session
        mirror_type = SAI_MIRROR_SESSION_TYPE_REMOTE
        monitor_port = port3
        vlan = vlan_remote_id
        print "Create mirror session: mirror_type = SAI_MIRROR_SESSION_TYPE_REMOTE, monitor_port = ptf_intf 3 "
        ingress_mirror_id = sai_thrift_create_mirror_session(self.client,
            mirror_type,
            monitor_port,
            vlan, None, None,
            None, None, None,
            None, None, None,
            None, None, None,
            None)
        print "ingress_mirror_id ' %x" %ingress_mirror_id

        attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[ingress_mirror_id]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        try:
            #assert ingress_mirror_id > 0, 'ingress_mirror_id is <= 0'

            pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                ip_id=102,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_id=102,
                ip_ttl=64,
                pktlen=104)
            exp_mirrored_pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=110,
                ip_id=102,
                ip_ttl=64,
                pktlen=104)
            print '#### Sending 00:22:22:22:22:22 | 00:11:11:11:11:11 | 10.0.0.1 | 192.168.0.1 | @ ptf_intf 1 ####'
            send_packet(self,switch_ports[1], str(pkt))
            verify_each_packet_on_each_port(self, [exp_pkt, exp_mirrored_pkt], [switch_ports[2],switch_ports[3]])

        finally:
            attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=0,object_id_list=[]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)

            self.client.sai_thrift_remove_mirror_session(ingress_mirror_id)

            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)
            self.client.sai_thrift_remove_vlan(vlan_remote_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class IngressERSpanMirrorTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 1 -> ptf_intf 2, ptf_intf 3 (erspan mirror)"

        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        # setup enhanced remote mirror session
        mirror_type = SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE
        monitor_port = port3
        vlan = vlan_id
        vlan_header_valid = True
        iphdr_version = 4
        tunnel_src_ip = "1.1.1.1"
        tunnel_dst_ip = "1.1.1.2"
        tunnel_src_mac = router_mac
        tunnel_dst_mac = "00:33:33:33:33:33"
        gre_protocol = 0x22EB
        ttl = 64

        print "Create mirror session: mirror_type = SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE, monitor_port = ptf_intf 3 "
        ingress_mirror_id = sai_thrift_create_mirror_session(self.client,
            mirror_type,
            monitor_port,
            vlan, None, None, vlan_header_valid,
            tunnel_src_mac, tunnel_dst_mac,
            tunnel_src_ip, tunnel_dst_ip,
            None, iphdr_version, ttl, None, gre_protocol)
        print "ingress_mirror_id = %d" %ingress_mirror_id

        attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[ingress_mirror_id]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        try:
            #assert ingress_mirror_id > 0, 'ingress_mirror_id is <= 0'

            pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                ip_id=102,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_id=102,
                ip_ttl=64,
                pktlen=104)
            exp_mirrored_pkt = ipv4_erspan_pkt(eth_dst=tunnel_dst_mac,
                eth_src=tunnel_src_mac,
                ip_src=tunnel_src_ip,
                ip_dst=tunnel_dst_ip,
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_id=0,
                ip_ttl=64,
                version=2,
                mirror_id=(ingress_mirror_id & 0x3FFFFFFF),
                inner_frame=pkt)

            exp_mask_mirrored_pkt = Mask(exp_mirrored_pkt)
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, 'flags')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, 'chksum')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.GRE, 'proto')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'span_id')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'timestamp')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'sgt_other')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'direction')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'version')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'vlan')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'priority')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'truncated')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'unknown2')

            print '#### Sending 00:22:22:22:22:22 | 00:11:11:11:11:11 | 10.0.0.1 | 192.168.0.1 | @ ptf_intf 1 ####'
            send_packet(self,switch_ports[1], str(pkt))
            verify_packet(self, exp_pkt, switch_ports[2])
            verify_packet(self, exp_mask_mirrored_pkt, switch_ports[3])

        finally:
            attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=0,object_id_list=[]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)

            self.client.sai_thrift_remove_mirror_session(ingress_mirror_id)

            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class EgressLocalMirrorTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 1 -> ptf_intf 2, ptf_intf 3 (local mirror)"

        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        # setup local mirror session
        mirror_type = SAI_MIRROR_SESSION_TYPE_LOCAL
        monitor_port = port3
        print "Create mirror session: mirror_type = SAI_MIRROR_TYPE_LOCAL, monitor_port = ptf_intf 3 "
        egress_mirror_id = sai_thrift_create_mirror_session(self.client,
            mirror_type,
            monitor_port,
            None, None, None,
            None, None, None,
            None, None, None,
            None, None, None,
            None)
        print "egress_mirror_id = %d" %egress_mirror_id

        attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[egress_mirror_id]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        try:
            #assert egress_mirror_id > 0, 'egress_mirror_id is <= 0'

            pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                ip_id=102,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_id=102,
                ip_ttl=64,
                pktlen=104)

            print '#### Sending 00:22:22:22:22:22 | 00:11:11:11:11:11 | 10.0.0.1 | 192.168.0.1 | @ ptf_intf 1 ####'
            send_packet(self,switch_ports[1], str(pkt))
            verify_each_packet_on_each_port(self, [exp_pkt, exp_pkt], [switch_ports[2],switch_ports[3]])

        finally:
            attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=0,object_id_list=[]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            self.client.sai_thrift_remove_mirror_session(egress_mirror_id)

            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class EgressERSpanMirrorTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 1 -> ptf_intf 2, ptf_intf 3 (erspan mirror)"

        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        # setup enhanced remote mirror session
        mirror_type = SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE
        monitor_port = port3
        vlan = vlan_id
        vlan_header_valid = True
        iphdr_version = 4
        tunnel_src_ip = "1.1.1.1"
        tunnel_dst_ip = "1.1.1.2"

        tunnel_src_mac = router_mac
        tunnel_dst_mac = "00:33:33:33:33:33"
        gre_protocol = 0x22EB
        ttl = 64

        print "Create mirror session: mirror_type = SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE, monitor_port = ptf_intf 3 "
        egress_mirror_id = sai_thrift_create_mirror_session(self.client,
            mirror_type,
            monitor_port,
            vlan, None, None, vlan_header_valid,
            tunnel_src_mac, tunnel_dst_mac,
            tunnel_src_ip, tunnel_dst_ip,
            None, iphdr_version, ttl, None, gre_protocol)
        print "egress_mirror_id = %d" %egress_mirror_id

        attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[egress_mirror_id]))
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        try:
            #assert egress_mirror_id > 0, 'egress_mirror_id is <= 0'

            pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                ip_id=102,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst=mac2,
                eth_src=mac1,
                ip_dst='10.0.0.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_id=102,
                ip_ttl=64,
                pktlen=104)
            exp_mirrored_pkt = ipv4_erspan_pkt(eth_dst=tunnel_dst_mac,
                eth_src=tunnel_src_mac,
                ip_src=tunnel_src_ip,
                ip_dst=tunnel_dst_ip,
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_id=0,
                ip_ttl=64,
                version=2,
                mirror_id=(egress_mirror_id & 0x3FFFFFFF),
                inner_frame=exp_pkt)

            exp_mask_mirrored_pkt = Mask(exp_mirrored_pkt)
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, 'flags')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, 'chksum')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.GRE, 'proto')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'span_id')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'timestamp')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'sgt_other')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'direction')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'version')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'vlan')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'priority')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'truncated')
            exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III, 'unknown2')

            print '#### Sending 00:22:22:22:22:22 | 00:11:11:11:11:11 | 10.0.0.1 | 192.168.0.1 | @ ptf_intf 1 ####'
            send_packet(self,switch_ports[1], str(pkt))
            verify_packet(self, exp_pkt, switch_ports[2])
            verify_packet(self, exp_mask_mirrored_pkt, switch_ports[3])

        finally:
            attr_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=0,object_id_list=[]))
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            self.client.sai_thrift_remove_mirror_session(egress_mirror_id)

            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class ERSpanIngressACLIngressMirrorTest(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This test performs erspan monitoring
    From port2(source port) we send traffic to port 3
    erspan mirror packets are expected on port 1(monitor port)
    '''
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        mac3='00:00:00:00:00:33'
        mac2='00:00:00:00:00:22'
        monitor_port=port1
        source_port=port2
        mirror_type=SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE
        vlan=0x1
        vlan_tpid=0x8100
        vlan_pri=0x6
        src_mac='00:00:00:00:11:22'
        dst_mac='00:00:00:00:11:33'
        encap_type=SAI_ERSPAN_ENCAPSULATION_TYPE_MIRROR_L3_GRE_TUNNEL
        ip_version=0x4
        tos=0
        ttl=64
        gre_type=0x88be
        src_ip='17.18.19.0'
        dst_ip='33.19.20.0'
        src_ip1='41.42.43.44'
        dst_ip1='45.46.47.48'
        src_mac1='00:00:00:00:22:33'
        dst_mac1='00:00:00:00:22:44'
        addr_family=0
        vlan_remote_id = 3
        vlan_oid = switch.default_vlan.oid
        vlan_remote_oid = sai_thrift_create_vlan(self.client, vlan_remote_id)
        mac_action = SAI_PACKET_ACTION_FORWARD

        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac3, port3, mac_action)
        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac2, port2, mac_action)

        # Put ports under test in VLAN 3
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # Set PVID
        attr_value = sai_thrift_attribute_value_t(u16=vlan_remote_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        erspanid=sai_thrift_create_mirror_session(
                          self.client,
                          mirror_type=mirror_type,
                          port=monitor_port,
                          vlan=vlan,
                          vlan_priority=vlan_pri,
                          vlan_tpid=vlan_tpid,
                          vlan_header_valid=False,
                          src_mac=src_mac,
                          dst_mac=dst_mac,
                          src_ip=src_ip,
                          dst_ip=dst_ip,
                          encap_type=encap_type,
                          iphdr_version=ip_version,
                          ttl=ttl,
                          tos=tos,
                          gre_type=gre_type)

        # setup ACL to block based on Source IP
        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = None
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = "172.16.0.1"
        ip_dst_mask = "255.255.255.0"
        ip_proto = 6
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = erspanid
        egress_mirror_id = None
        range_list = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to port2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='172.16.0.1',
                                ip_id=101,
                                ip_ttl=64,
                                tcp_sport=4000,
                                tcp_dport=5000)

        eth_pkt = simple_eth_packet(pktlen=100, eth_dst='00:00:00:00:00:33', eth_src='00:00:00:00:00:22')

        exp_mirrored_pkt = ipv4_erspan_pkt(
            eth_src='00:00:00:00:11:22',
            eth_dst='00:00:00:00:11:33',
            ip_src='17.18.19.0',
            ip_dst='33.19.20.0',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x2,
            version=1,
            mirror_id=1,
            inner_frame=pkt)

        exp_mask_mirrored_pkt=Mask(exp_mirrored_pkt)
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'tos')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'frag')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'ttl')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'flags')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.GRE,'proto')

        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'span_id')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'direction')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'version')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'vlan')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'priority')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'truncated')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'unknown2')

        exp_eth_mirrored_pkt = ipv4_erspan_pkt(
            eth_src='00:00:00:00:11:22',
            eth_dst='00:00:00:00:11:33',
            ip_src='17.18.19.0',
            ip_dst='33.19.20.0',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x2,
            version=1,
            mirror_id=1,
            inner_frame=eth_pkt)

        try:
            # in tuple: 0 is device number, 2 is port number
            # this tuple uniquely identifies a port
            # for ingress mirroring
            print "Checking INGRESS ERSPAN Mirroring"
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt,switch_ports[2])
            verify_packet(self, exp_mask_mirrored_pkt, switch_ports[0])


        finally:
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac2, port2)
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac3, port3)

            # unbind this ACL table from port2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)

            # Now you can remove destination
            self.client.sai_thrift_remove_mirror_session(erspanid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            # Remove ports from VLAN 3
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_remote_oid)


@pktpy_skip  # TODO bf-pktpy
@group('mirror')
class ERSpanEgressACLEgressMirrorTest(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This test performs erspan monitoring
    From port2(source port) we send traffic to port 3
    erspan mirror packets are expected on port 1(monitor port)
    '''
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        mac3='00:00:00:00:00:33'
        mac2='00:00:00:00:00:22'
        monitor_port=port1
        source_port=port2
        mirror_type=SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE
        vlan=0x1
        vlan_tpid=0x8100
        vlan_pri=0x6
        src_mac='00:00:00:00:11:22'
        dst_mac='00:00:00:00:11:33'
        encap_type=SAI_ERSPAN_ENCAPSULATION_TYPE_MIRROR_L3_GRE_TUNNEL
        ip_version=0x4
        tos=0
        ttl=64
        gre_type=0x88be
        src_ip='17.18.19.0'
        dst_ip='33.19.20.0'
        src_ip1='41.42.43.44'
        dst_ip1='45.46.47.48'
        src_mac1='00:00:00:00:22:33'
        dst_mac1='00:00:00:00:22:44'
        addr_family=0
        vlan_remote_id = 3
        vlan_oid = switch.default_vlan.oid
        vlan_remote_oid = sai_thrift_create_vlan(self.client, vlan_remote_id)
        mac_action = SAI_PACKET_ACTION_FORWARD

        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac3, port3, mac_action)
        sai_thrift_create_fdb(self.client, vlan_remote_oid, mac2, port2, mac_action)

        # Put ports under test in VLAN 3
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_remote_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # Set PVID
        attr_value = sai_thrift_attribute_value_t(u16=vlan_remote_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        erspanid=sai_thrift_create_mirror_session(
                          self.client,
                          mirror_type=mirror_type,
                          port=monitor_port,
                          vlan=vlan,
                          vlan_priority=vlan_pri,
                          vlan_tpid=vlan_tpid,
                          vlan_header_valid=False,
                          src_mac=src_mac,
                          dst_mac=dst_mac,
                          src_ip=src_ip,
                          dst_ip=dst_ip,
                          encap_type=encap_type,
                          iphdr_version=ip_version,
                          ttl=ttl,
                          tos=tos,
                          gre_type=gre_type)

        # setup ACL to block based on Source IP
        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        table_stage = SAI_ACL_STAGE_EGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = None
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = "172.16.0.1"
        ip_dst_mask = "255.255.255.0"
        ip_proto = 6
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = erspanid
        range_list = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to port2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        pkt = simple_tcp_packet(eth_dst='00:00:00:00:00:33',
                                eth_src='00:00:00:00:00:22',
                                ip_dst='172.16.0.1',
                                ip_id=101,
                                ip_ttl=64,
                                tcp_sport=4000,
                                tcp_dport=5000)

        eth_pkt = simple_eth_packet(pktlen=100, eth_dst='00:00:00:00:00:33', eth_src='00:00:00:00:00:22')

        exp_mirrored_pkt = ipv4_erspan_pkt(
            eth_src='00:00:00:00:11:22',
            eth_dst='00:00:00:00:11:33',
            ip_src='17.18.19.0',
            ip_dst='33.19.20.0',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x2,
            version=1,
            mirror_id=1,
            inner_frame=pkt)

        exp_mask_mirrored_pkt=Mask(exp_mirrored_pkt)
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'tos')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'frag')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'ttl')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'flags')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP,'chksum')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.GRE,'proto')

        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'span_id')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'direction')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'version')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'vlan')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'priority')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'truncated')
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN, 'unknown2')

        exp_eth_mirrored_pkt = ipv4_erspan_pkt(
            eth_src='00:00:00:00:11:22',
            eth_dst='00:00:00:00:11:33',
            ip_src='17.18.19.0',
            ip_dst='33.19.20.0',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x2,
            version=1,
            mirror_id=1,
            inner_frame=eth_pkt)

        try:
            # in tuple: 0 is device number, 2 is port number
            # this tuple uniquely identifies a port
            # for ingress mirroring
            print "Checking EGRESS ERSPAN Mirroring"
            print "Sending packet port 2 -> port 3 (00:00:00:00:00:22 -> 00:00:00:00:00:33)"
            send_packet(self,switch_ports[1], pkt)
            verify_packet(self, pkt, switch_ports[2])
            verify_packet(self, exp_mask_mirrored_pkt, switch_ports[0])


        finally:
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac2, port2)
            sai_thrift_delete_fdb(self.client, vlan_remote_oid, mac3, port3)

            # unbind this ACL table from port2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port3, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)

            # Now you can remove destination
            self.client.sai_thrift_remove_mirror_session(erspanid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            # Remove ports from VLAN 3
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_remote_oid)
