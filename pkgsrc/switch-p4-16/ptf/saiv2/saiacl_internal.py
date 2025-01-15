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
Thrift SAI interface ACL tests
"""

import copy

from sai_base_test import *


class AclPreIngressTest(SaiHelper):
    '''
    Test pre-ingress ACL
    '''

    dmac1 = '00:11:22:33:44:55'
    dmac2 = '00:11:22:33:44:56'
    src_mac = '00:26:dd:14:c4:ee'
    src_mac_mask = 'ff:ff:ff:ff:ff:ff'
    ip_addr1 = '10.0.0.1'
    ip_addr2 = '10.10.10.2'
    ipv6_addr = '1234:5678:9abc:def0:4422:1133:5577:99aa'

    pkt = simple_ip_packet(
        eth_src=src_mac,
        eth_dst=ROUTER_MAC,
        ip_dst=ip_addr1,
        ip_ttl=64)

    exp_pkt_default_vrf = simple_ip_packet(
        eth_src=ROUTER_MAC,
        eth_dst=dmac1,
        ip_dst=ip_addr1,
        ip_ttl=63)

    exp_pkt_vrf1 = simple_ip_packet(
        eth_src=ROUTER_MAC,
        eth_dst=dmac2,
        ip_dst=ip_addr1,
        ip_ttl=63)

    pktv6 = simple_tcpv6_packet(
        eth_dst=ROUTER_MAC,
        eth_src=src_mac,
        ipv6_dst=ipv6_addr,
        ipv6_hlim=64)

    exp_pktv6_vrf1 = simple_tcpv6_packet(
        eth_dst=dmac2,
        eth_src=ROUTER_MAC,
        ipv6_dst=ipv6_addr,
        ipv6_hlim=63)

    def setUp(self):
        super(AclPreIngressTest, self).setUp()

        self.port24_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)

        self.port25_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port25)

        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr2),
            router_interface_id=self.port25_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port25_rif, ip_address=sai_ipaddress(self.ip_addr2))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry1, dst_mac_address=self.dmac1)

        self.route_entry0 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.0.0.1/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry0, next_hop_id=self.nhop1)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:99aa/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry1, next_hop_id=self.nhop1)

        self.vrf1 = sai_thrift_create_virtual_router(self.client)
        self.vrf1_port26_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.vrf1,
            port_id=self.port26)
        self.vrf1_nhop0 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr2),
            router_interface_id=self.vrf1_port26_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.vrf1_neighbor_entry0 = sai_thrift_neighbor_entry_t(
            rif_id=self.vrf1_port26_rif,
            ip_address=sai_ipaddress(self.ip_addr2))
        sai_thrift_create_neighbor_entry(
            self.client, self.vrf1_neighbor_entry0, dst_mac_address=self.dmac2)
        self.vrf1_route_entry0 = sai_thrift_route_entry_t(
            vr_id=self.vrf1, destination=sai_ipprefix('10.0.0.1/32'))
        sai_thrift_create_route_entry(
            self.client, self.vrf1_route_entry0, next_hop_id=self.vrf1_nhop0)
        self.vrf1_route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.vrf1,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:99aa/128'))
        sai_thrift_create_route_entry(
            self.client, self.vrf1_route_entry1, next_hop_id=self.vrf1_nhop0)

    def runTest(self):
        try:
            self.testPreIngressAcl()
            self.testPreIngressAclFields()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.vrf1_route_entry1)
        sai_thrift_remove_route_entry(self.client, self.vrf1_route_entry0)
        sai_thrift_remove_next_hop(self.client, self.vrf1_nhop0)
        sai_thrift_remove_neighbor_entry(
            self.client, self.vrf1_neighbor_entry0)
        sai_thrift_remove_router_interface(self.client, self.vrf1_port26_rif)
        sai_thrift_remove_virtual_router(self.client, self.vrf1)

        sai_thrift_remove_route_entry(self.client, self.route_entry0)
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_router_interface(self.client, self.port25_rif)
        sai_thrift_remove_router_interface(self.client, self.port24_rif)

        super(AclPreIngressTest, self).tearDown()

    def testPreIngressAcl(self):
        '''
        Test pre-ingress matching and VRF assignment
        '''
        print("testPreIngressAcl")
        acl_table_oid = None
        acl_entry_oid = None
        try:
            table_stage = SAI_ACL_STAGE_PRE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            acl_table_oid = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_acl_ip_type=True,
                field_src_mac=True,
                field_ether_type=True,
                field_dst_ip=True,
                field_dst_ipv6=True,
                field_dscp=True,
                field_in_port=True)
            self.assertTrue(acl_table_oid != 0)

            src_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.src_mac),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.src_mac_mask))

            action_set_vrf = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=self.vrf1))

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_FORWARD))

            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table_oid,
                priority=10,
                field_src_mac=src_mac_t,
                action_packet_action=packet_action,
                action_set_vrf=action_set_vrf)
            self.assertTrue(acl_entry_oid != 0)

            # Send to port in default vrf, expect in default vrf -
            # pre ingress is not enabled on switch
            send_packet(self, self.dev_port24, self.pkt)
            verify_packets(self, self.exp_pkt_default_vrf, [self.dev_port25])

            # bind pre-ingress table to switch
            sai_thrift_set_switch_attribute(self.client,
                                            pre_ingress_acl=acl_table_oid)

            # Send to port in default vrf, expect in vrf1
            send_packet(self, self.dev_port24, self.pkt)
            verify_packets(self, self.exp_pkt_vrf1, [self.dev_port26])

            sai_thrift_set_switch_attribute(self.client,
                                            pre_ingress_acl=0)

            send_packet(self, self.dev_port24, self.pkt)
            verify_packets(self, self.exp_pkt_default_vrf, [self.dev_port25])

        finally:
            sai_thrift_set_switch_attribute(self.client, pre_ingress_acl=0)
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            if acl_table_oid:
                sai_thrift_remove_acl_table(self.client, acl_table_oid)

    def testPreIngressAclFields(self):
        '''
        Testing every field hit in this table
        mac_src_addr : ternary;
        ip_dst_addr : ternary;
        ipv6_dst_addr : ternary;
        dscp : ternary;
        ingress_port : ternary
        '''
        print("testPreIngressAclFields")
        acl_table_oid = None
        acl_entry_oid = None
        try:
            table_stage = SAI_ACL_STAGE_PRE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            acl_table_oid = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_acl_ip_type=True,
                field_src_mac=True,
                field_dst_ip=True,
                field_dst_ipv6=True,
                field_dscp=True,
                field_in_port=True)
            self.assertTrue(acl_table_oid != 0)
            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table_oid)
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter),
                enable=True)

            # bind pre-ingress table to switch
            sai_thrift_set_switch_attribute(self.client,
                                            pre_ingress_acl=acl_table_oid)

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_FORWARD))

            action_set_vrf = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=self.vrf1))

            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt_default_vrf, self.dev_port25)

            ipv4_type = SAI_ACL_IP_TYPE_IPV4ANY
            ipv6_type = SAI_ACL_IP_TYPE_IPV6ANY
            for ip_type, pkt in zip([ipv4_type, ipv6_type],
                                    [self.pkt, self.pktv6]):
                ip_type_t = sai_thrift_acl_field_data_t(
                    data=sai_thrift_acl_field_data_data_t(s32=ip_type))

                print("Add acl_entry with match on ip_type", ip_type)
                acl_entry_oid = sai_thrift_create_acl_entry(
                    self.client,
                    table_id=acl_table_oid,
                    priority=10,
                    field_acl_ip_type=ip_type_t,
                    action_packet_action=packet_action,
                    action_counter=action_counter_t,
                    action_set_vrf=action_set_vrf)
                self.assertTrue(acl_entry_oid != 0)

                # Send to port in default vrf, expect in vrf1
                if ip_type == SAI_ACL_IP_TYPE_IPV4ANY:
                    send_packet(self, self.dev_port24, pkt)
                    verify_packet(self, self.exp_pkt_vrf1, self.dev_port26)
                else:
                    send_packet(self, self.dev_port24, pkt)
                    verify_packet(self, self.exp_pktv6_vrf1, self.dev_port26)

                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                self.assertEqual(packets['packets'], 1)
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

            src_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.src_mac),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.src_mac_mask))

            print("Add acl_entry with match on src_mac")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table_oid,
                priority=10,
                field_src_mac=src_mac_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t,
                action_set_vrf=action_set_vrf)
            self.assertTrue(acl_entry_oid != 0)

            # Send to port in default vrf, expect in vrf1
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt_vrf1, self.dev_port26)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

            dst_ip = self.ip_addr1
            dst_ip_mask = '255.255.255.255'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))

            print("Add acl_entry with match on dst_ip")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table_oid,
                priority=10,
                field_dst_ip=dst_ip_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t,
                action_set_vrf=action_set_vrf)
            self.assertTrue(acl_entry_oid != 0)

            # Send to port in default vrf, expect in vrf1
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt_vrf1, self.dev_port26)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

            dst_ipv6 = self.ipv6_addr
            dst_ipv6_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ipv6_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ipv6),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ipv6_mask))

            print("Add acl_entry with match on dst_ipv6")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table_oid,
                priority=10,
                field_dst_ipv6=dst_ipv6_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t,
                action_set_vrf=action_set_vrf)
            self.assertTrue(acl_entry_oid != 0)

            # Send to port in default vrf, expect in vrf1
            send_packet(self, self.dev_port24, self.pktv6)
            verify_packet(self, self.exp_pktv6_vrf1, self.dev_port26)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

            pkt_dscp = copy.deepcopy(self.pkt)
            pkt_dscp['IP'].tos = 8
            exp_pkt_dscp = copy.deepcopy(self.exp_pkt_vrf1)
            exp_pkt_dscp['IP'].tos = 8
            dscp_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=2),
                mask=sai_thrift_acl_field_data_mask_t(u8=127))

            print("Add acl_entry with match on dscp")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table_oid,
                priority=10,
                field_dscp=dscp_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t,
                action_set_vrf=action_set_vrf)
            self.assertTrue(acl_entry_oid != 0)

            # Send to port in default vrf, expect in vrf1
            send_packet(self, self.dev_port24, pkt_dscp)
            verify_packet(self, exp_pkt_dscp, self.dev_port26)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

            for port, dev_port in zip([self.port24, self.port25],
                                      [self.dev_port24, self.dev_port25]):
                in_port_t = sai_thrift_acl_field_data_t(
                    data=sai_thrift_acl_field_data_data_t(oid=port))

                print("Add acl_entry with match on in_port", dev_port)
                acl_entry_oid = sai_thrift_create_acl_entry(
                    self.client,
                    table_id=acl_table_oid,
                    priority=10,
                    field_in_port=in_port_t,
                    action_packet_action=packet_action,
                    action_counter=action_counter_t,
                    action_set_vrf=action_set_vrf)
                self.assertTrue(acl_entry_oid != 0)

                # Send to port in default vrf, expect in vrf1
                send_packet(self, dev_port, self.pkt)
                verify_packet(self, self.exp_pkt_vrf1, self.dev_port26)

                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                self.assertEqual(packets['packets'], 1)
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

        finally:
            sai_thrift_set_switch_attribute(self.client, pre_ingress_acl=0)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            if acl_table_oid:
                sai_thrift_remove_acl_table(self.client, acl_table_oid)


class AclIngressTest(SaiHelper):
    '''
    Test ingress ACL
    '''

    dmac1 = '00:11:22:33:44:55'
    dmac2 = '00:11:22:33:44:56'
    mac_mask = 'ff:ff:ff:ff:ff:ff'
    ctrl_dst_ip_addr = '11.1.1.1'
    dst_ip_addr = '10.1.1.1'
    src_ip_addr = '20.1.1.1'
    ip_mask = '255.255.255.255'
    dst_ipv6_addr = '1234:5678:9abc:def0:4422:1133:5577:99aa'
    src_ipv6_addr = '2468:5678:9abc:def0:4422:1133:5577:99aa'
    ipv6_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
    l4_dst_port = 1111

    pkt = simple_tcp_packet(
        eth_dst=ROUTER_MAC,
        ip_dst=dst_ip_addr,
        ip_src=src_ip_addr,
        tcp_dport=l4_dst_port,
        ip_ttl=64)

    exp_pkt = simple_tcp_packet(
        eth_src=ROUTER_MAC,
        eth_dst=dmac1,
        ip_dst=dst_ip_addr,
        ip_src=src_ip_addr,
        tcp_dport=l4_dst_port,
        ip_ttl=63)

    ctrl_pkt = simple_tcp_packet(
        eth_dst=ROUTER_MAC,
        ip_dst=ctrl_dst_ip_addr,
        tcp_dport=2222,
        ip_ttl=54)

    ctrl_exp_pkt = simple_tcp_packet(
        eth_src=ROUTER_MAC,
        eth_dst=dmac1,
        ip_dst=ctrl_dst_ip_addr,
        tcp_dport=2222,
        ip_ttl=53)

    pktv6 = simple_tcpv6_packet(
        eth_dst=ROUTER_MAC,
        ipv6_dst=dst_ipv6_addr,
        ipv6_src=src_ipv6_addr,
        tcp_dport=l4_dst_port,
        ipv6_hlim=64)

    exp_pktv6 = simple_tcpv6_packet(
        eth_dst=dmac1,
        eth_src=ROUTER_MAC,
        ipv6_dst=dst_ipv6_addr,
        ipv6_src=src_ipv6_addr,
        tcp_dport=l4_dst_port,
        ipv6_hlim=63)

    icmp6_pkt = simple_icmpv6_packet(
        eth_dst=ROUTER_MAC,
        ipv6_dst='ff02::2',
        ipv6_src='2000::1',
        icmp_type=133,
        ipv6_hlim=64)

    icmp4_pkt = simple_icmp_packet(
        eth_dst=ROUTER_MAC,
        ip_src=src_ip_addr,
        ip_dst=dst_ip_addr,
        icmp_type=8,
        icmp_data='000102030405')

    arp_pkt = simple_arp_packet(
        arp_op=1,
        eth_dst=ROUTER_MAC,
        hw_tgt=ROUTER_MAC,
        ip_snd=src_ip_addr,
        ip_tgt=dst_ip_addr)

    def setUp(self):
        super(AclIngressTest, self).setUp()

        self.port24_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)

        self.port25_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port25)

        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port25_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port25_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry1, dst_mac_address=self.dmac1)

        self.route_entry0 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.1.1.0/24'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry0, next_hop_id=self.nhop1,
            meta_data=10)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:99aa/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry1, next_hop_id=self.nhop1,
            meta_data=10)

        self.ctrl_route_entry0 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('11.1.1.1/32'))
        sai_thrift_create_route_entry(
            self.client, self.ctrl_route_entry0, next_hop_id=self.nhop1)

        # create hostifs
        # these hostif will be used for receiving trapped packets
        # hostif1 will receive packets from user trap 1
        hostif1_name = "hostif1"
        self.hostif1 = sai_thrift_create_hostif(self.client,
                                                name=hostif1_name,
                                                obj_id=self.port24,
                                                type=SAI_HOSTIF_TYPE_NETDEV)
        self.assertTrue(self.hostif1 != 0)
        time.sleep(6)
        self.hostif1_socket = open_packet_socket(hostif1_name)

        self.hostif_trap_group = sai_thrift_create_hostif_trap_group(
            self.client, admin_state=True, queue=2)

        self.udt1 = sai_thrift_create_hostif_user_defined_trap(
            self.client,
            trap_group=self.hostif_trap_group,
            type=SAI_HOSTIF_USER_DEFINED_TRAP_TYPE_ACL)
        self.assertTrue(self.udt1 != 0)

        # associate user defined trap 1 with hostif 1
        channel = SAI_HOSTIF_TABLE_ENTRY_CHANNEL_TYPE_NETDEV_PHYSICAL_PORT
        self.hostif_table_entry_udt1_hif1 = \
            sai_thrift_create_hostif_table_entry(
                self.client,
                channel_type=channel,
                host_if=self.hostif1,
                trap_id=self.udt1,
                type=SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID)
        self.assertTrue(self.hostif_table_entry_udt1_hif1 != 0)

        self.local_mirror_session = sai_thrift_create_mirror_session(
            self.client,
            monitor_port=self.port26,
            type=SAI_MIRROR_SESSION_TYPE_LOCAL)

        src_ipv4_egr = sai_thrift_ip_addr_t(ip4="222.1.1.1")
        dst_ipv4_egr = sai_thrift_ip_addr_t(ip4="111.1.1.1")
        self.src_ip_addr_egr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=src_ipv4_egr)
        self.dst_ip_addr_egr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=dst_ipv4_egr)
        encap_type = SAI_ERSPAN_ENCAPSULATION_TYPE_MIRROR_L3_GRE_TUNNEL
        self.erspan_mirror_session = sai_thrift_create_mirror_session(
            self.client,
            monitor_port=self.port27,
            type=SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE,
            erspan_encapsulation_type=encap_type,
            iphdr_version=0x4,
            src_ip_address=self.src_ip_addr_egr,
            dst_ip_address=self.dst_ip_addr_egr,
            src_mac_address="00:00:00:00:11:22",
            dst_mac_address="00:00:00:00:11:33",
            gre_protocol_type=0x22eb,
            ttl=64)

        self.acl_table_oid = None
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)
        self.acl_table_oid = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_acl_ip_type=True,
            field_ether_type=True,
            field_dst_mac=True,
            field_src_ip=True,
            field_dst_ip=True,
            field_src_ipv6=True,
            field_dst_ipv6=True,
            field_ttl=True,
            field_dscp=True,
            field_ecn=True,
            field_ip_protocol=True,
            field_icmp_type=True,
            field_icmpv6_type=True,
            field_l4_dst_port=True,
            field_in_port=True,
            field_route_dst_user_meta=True,
            user_defined_field_group_min=True)
        self.assertTrue(self.acl_table_oid != 0)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.acl_table_oid)

    def runTest(self):
        try:
            self.testIngressAclActions()
            self.testIngressAclActionMeterPermit()
            self.testIngressAclActionMeterTrap()
            self.testIngressAclActionMeterCopy()
            self.testIngressAclActionMeterUpdate()
            self.testIngressAclMirrorAction()
            self.testIngressAclFields()
        finally:
            pass

    def tearDown(self):
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_table(self.client, self.acl_table_oid)
        sai_thrift_remove_mirror_session(
            self.client, self.erspan_mirror_session)
        sai_thrift_remove_mirror_session(
            self.client, self.local_mirror_session)
        sai_thrift_remove_hostif_table_entry(
            self.client, self.hostif_table_entry_udt1_hif1)
        sai_thrift_remove_hostif_user_defined_trap(
            self.client, self.udt1)
        sai_thrift_remove_hostif_trap_group(
            self.client, self.hostif_trap_group)
        sai_thrift_remove_hostif(self.client, self.hostif1)
        sai_thrift_remove_route_entry(self.client, self.ctrl_route_entry0)
        sai_thrift_remove_route_entry(self.client, self.route_entry0)
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_router_interface(self.client, self.port25_rif)
        sai_thrift_remove_router_interface(self.client, self.port24_rif)

        super(AclIngressTest, self).tearDown()

    def testIngressAclMirrorAction(self):
        '''
        Testing mirror action in this table
        '''
        print("testIngressAclMirrorAction")
        acl_entry_oid = None
        try:
            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_FORWARD))

            # dst_mac
            dst_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.dmac1),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.mac_mask))

            obj_list = sai_thrift_object_list_t(
                count=1, idlist=[self.erspan_mirror_session])
            mirror_session_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(objlist=obj_list))

            print("Add acl_entry with match on dst_mac and mirror action")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_mac=dst_mac_t,
                action_packet_action=packet_action,
                action_mirror_ingress=mirror_session_id)
            self.assertTrue(acl_entry_oid != 0)

            self.testMirrorPacket()

            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None
        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

    def testIngressAclActions(self):
        '''
        Testing every action in this table
        '''
        print("testIngressAclActions")
        acl_entry_oid = None
        try:
            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=self.acl_table_oid)
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter),
                enable=True)

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_FORWARD))

            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)

            # dst_mac
            dst_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.dmac1),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.mac_mask))

            print("Add acl_entry with match on dst_mac")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_mac=dst_mac_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # forward
            print("Verify packet forward action")
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)

            # drop
            print("Verify packet drop action")
            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_DROP))
            sai_thrift_set_acl_entry_attribute(
                self.client,
                acl_entry_oid,
                action_packet_action=packet_action)
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self, timeout=1)

            # PINS uses only permit action with meters
            # disable these action tests
            # pre_stats = sai_thrift_get_queue_stats(
            #     self.client, self.cpu_queue0)
            # # trap
            # print("Verify packet trap action")
            # packet_action = sai_thrift_acl_action_data_t(
            #     parameter=sai_thrift_acl_action_parameter_t(
            #         s32=SAI_PACKET_ACTION_TRAP))
            # set_user_trap_id = sai_thrift_acl_action_data_t(
            #     parameter=sai_thrift_acl_action_parameter_t(oid=self.udt1))
            # sai_thrift_set_acl_entry_attribute(
            #     self.client,
            #     acl_entry_oid,
            #     action_packet_action=packet_action,
            #     action_set_user_trap_id=set_user_trap_id)
            # send_packet(self, self.dev_port24, self.pkt)
            # time.sleep(2)

            # #copy
            # print("Verify packet copy action")
            # packet_action = sai_thrift_acl_action_data_t(
            #     parameter=sai_thrift_acl_action_parameter_t(
            #         s32=SAI_PACKET_ACTION_COPY))
            # set_user_trap_id = sai_thrift_acl_action_data_t(
            #     parameter=sai_thrift_acl_action_parameter_t(oid=self.udt1))
            # sai_thrift_set_acl_entry_attribute(
            #     self.client,
            #     acl_entry_oid,
            #     action_packet_action=packet_action,
            #     action_set_user_trap_id=set_user_trap_id)
            # send_packet(self, self.dev_port24, self.pkt)
            # verify_packet(self, self.exp_pkt, self.dev_port25)

            # print("Verifying CPU port queue stats")
            # time.sleep(4)
            # post_stats = sai_thrift_get_queue_stats(
            #     self.client, self.cpu_queue0)
            # self.assertEqual(
            #     post_stats["SAI_QUEUE_STAT_PACKETS"],
            #     pre_stats["SAI_QUEUE_STAT_PACKETS"] + 2)
            # self.dataplane.flush()

            # # clear trap id
            # set_user_trap_id = sai_thrift_acl_action_data_t(
            #     parameter=sai_thrift_acl_action_parameter_t(oid=0))
            # sai_thrift_set_acl_entry_attribute(
            #     self.client,
            #     acl_entry_oid,
            #     action_packet_action=packet_action,
            #     action_set_user_trap_id=set_user_trap_id)

            # local mirror
            print("Verify packet local mirror action")
            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_FORWARD))
            obj_list = sai_thrift_object_list_t(
                count=1, idlist=[self.local_mirror_session])
            mirror_session_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(objlist=obj_list))
            sai_thrift_set_acl_entry_attribute(
                self.client,
                acl_entry_oid,
                action_packet_action=packet_action,
                action_mirror_ingress=mirror_session_id)
            send_packet(self, self.dev_port24, self.pkt)
            verify_each_packet_on_each_port(
                self, [self.exp_pkt, self.pkt],
                [self.dev_port25, self.dev_port26])

            # erspan mirror
            print("Verify packet erspan mirror action")
            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_FORWARD))
            obj_list = sai_thrift_object_list_t(
                count=1, idlist=[self.erspan_mirror_session])
            mirror_session_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(objlist=obj_list))
            sai_thrift_set_acl_entry_attribute(
                self.client,
                acl_entry_oid,
                action_packet_action=packet_action,
                action_mirror_ingress=mirror_session_id)

            self.testMirrorPacket()
            # for some reason model clear stats for every action update
            # disabling for now
            # packets = sai_thrift_get_acl_counter_attribute(
            #     self.client, acl_counter, packets=True)
            # self.assertEqual(packets['packets'], 3)

            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            sai_thrift_remove_acl_counter(self.client, acl_counter)

    def testIngressAclActionMeterPermit(self):
        '''
        Testing every action in this table
        '''
        print("testIngressAclActionMeterPermit")
        acl_entry_oid = None
        try:
            permit_policer = sai_thrift_create_policer(
                self.client,
                mode=SAI_POLICER_MODE_TR_TCM,
                meter_type=SAI_METER_TYPE_BYTES,
                color_source=SAI_POLICER_COLOR_SOURCE_BLIND,
                cir=10000,
                pir=10000,
                cbs=10000,
                pbs=10000,
                green_packet_action=SAI_PACKET_ACTION_FORWARD,
                yellow_packet_action=SAI_PACKET_ACTION_DROP,
                red_packet_action=SAI_PACKET_ACTION_DROP)

            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=self.acl_table_oid)
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter),
                enable=True)

            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)

            # dst_mac
            dst_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.dmac1),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.mac_mask))

            set_policer_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=permit_policer))

            set_user_trap_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(oid=self.udt1))

            print("Add acl_entry with match on dst_mac")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_mac=dst_mac_t,
                action_set_user_trap_id=set_user_trap_id,
                action_set_policer=set_policer_id,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # forward
            print("Verify packet forward action")
            for _ in range(0, 20):
                send_packet(self, self.dev_port24, self.pkt)
                verify_packet(self, self.exp_pkt, self.dev_port25)

            time.sleep(5)
            stats = sai_thrift_get_policer_stats(self.client, permit_policer)
            self.assertEqual(stats['SAI_POLICER_STAT_PACKETS'], 20)

            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_policer(self.client, permit_policer)

    def testIngressAclActionMeterTrap(self):
        '''
        Testing every action in this table
        '''
        print("testIngressAclActionMeterTrap")
        acl_entry_oid = None
        try:
            trap_policer = sai_thrift_create_policer(
                self.client,
                mode=SAI_POLICER_MODE_TR_TCM,
                meter_type=SAI_METER_TYPE_BYTES,
                color_source=SAI_POLICER_COLOR_SOURCE_BLIND,
                cir=10,
                pir=10,
                cbs=10,
                pbs=10,
                green_packet_action=SAI_PACKET_ACTION_TRAP,
                yellow_packet_action=SAI_PACKET_ACTION_DROP,
                red_packet_action=SAI_PACKET_ACTION_DROP)

            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=self.acl_table_oid)
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter),
                enable=True)

            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)

            # dst_mac
            dst_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.dmac1),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.mac_mask))

            set_policer_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(oid=trap_policer))

            set_user_trap_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(oid=self.udt1))

            print("Add acl_entry with match on dst_mac")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_mac=dst_mac_t,
                action_set_user_trap_id=set_user_trap_id,
                action_set_policer=set_policer_id,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # trap
            pre_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue2)
            print("Verify packet trap action")
            for _ in range(0, 20):
                send_packet(self, self.dev_port24, self.pkt)

            time.sleep(4)
            stats = sai_thrift_get_policer_stats(self.client, trap_policer)
            self.assertEqual(stats['SAI_POLICER_STAT_PACKETS'], 20)

            print("Verifying CPU port queue stats")
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue2)
            queue2_count = post_stats["SAI_QUEUE_STAT_PACKETS"] \
                - pre_stats["SAI_QUEUE_STAT_PACKETS"]
            self.assertEqual(
                queue2_count, stats['SAI_POLICER_STAT_GREEN_PACKETS'])

            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_policer(self.client, trap_policer)

    def testIngressAclActionMeterCopy(self):
        '''
        Testing every action in this table
        '''
        print("testIngressAclActionMeterCopy")
        acl_entry_oid = None
        try:
            copy_policer = sai_thrift_create_policer(
                self.client,
                mode=SAI_POLICER_MODE_TR_TCM,
                meter_type=SAI_METER_TYPE_BYTES,
                color_source=SAI_POLICER_COLOR_SOURCE_BLIND,
                cir=10,
                pir=10,
                cbs=10,
                pbs=10,
                green_packet_action=SAI_PACKET_ACTION_COPY,
                yellow_packet_action=SAI_PACKET_ACTION_FORWARD,
                red_packet_action=SAI_PACKET_ACTION_FORWARD)

            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=self.acl_table_oid)
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter),
                enable=True)

            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)

            # dst_mac
            dst_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.dmac1),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.mac_mask))

            set_policer_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(oid=copy_policer))

            set_user_trap_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(oid=self.udt1))

            print("Add acl_entry with match on dst_mac")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_mac=dst_mac_t,
                action_set_user_trap_id=set_user_trap_id,
                action_set_policer=set_policer_id,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # copy
            pre_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue2)
            print("Verify packet copy action")
            for _ in range(0, 20):
                send_packet(self, self.dev_port24, self.pkt)
                verify_packet(self, self.exp_pkt, self.dev_port25)

            time.sleep(4)
            stats = sai_thrift_get_policer_stats(self.client, copy_policer)
            self.assertEqual(stats['SAI_POLICER_STAT_PACKETS'], 20)

            print("Verifying CPU port queue stats")
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue2)
            queue2_count = post_stats["SAI_QUEUE_STAT_PACKETS"] \
                - pre_stats["SAI_QUEUE_STAT_PACKETS"]
            self.assertEqual(
                queue2_count, stats['SAI_POLICER_STAT_GREEN_PACKETS'])

            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_policer(self.client, copy_policer)

    def testMirrorPacket(self):
        '''
        Test mirror packet
        '''
        exp_mirrored_pkt = ipv4_erspan_pkt(eth_dst="00:00:00:00:11:33",
                                           eth_src="00:00:00:00:11:22",
                                           ip_src="222.1.1.1",
                                           ip_dst="111.1.1.1",
                                           ip_id=0,
                                           ip_ttl=64,
                                           ip_flags=0x2,
                                           version=2,
                                           mirror_id=2,
                                           inner_frame=self.pkt)
        # IEEE 1588
        exp_mirrored_pkt["ERSPAN_III"].gra = 2

        exp_mask_mirrored_pkt = Mask(exp_mirrored_pkt)
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, "tos")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, "frag")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, "ihl")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, "len")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, "ttl")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, "flags")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.IP, "chksum")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.GRE, "proto")

        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III,
                                                    "session_id")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III,
                                                    "vlan")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III,
                                                    "timestamp")
        exp_mask_mirrored_pkt.set_do_not_care_scapy(ptf.packet.ERSPAN_III,
                                                    "sgt_other")
        send_packet(self, self.dev_port24, self.pkt)
        verify_each_packet_on_each_port(
            self, [self.exp_pkt, exp_mask_mirrored_pkt],
            [self.dev_port25, self.dev_port27])

    def testIngressAclActionMeterUpdate(self):
        '''
        Testing meter action update
        '''
        print("testIngressAclActionMeterUpdate")
        acl_entry_oid = None
        try:
            permit_policer = sai_thrift_create_policer(
                self.client,
                mode=SAI_POLICER_MODE_TR_TCM,
                meter_type=SAI_METER_TYPE_BYTES,
                color_source=SAI_POLICER_COLOR_SOURCE_BLIND,
                cir=10000,
                pir=10000,
                cbs=10000,
                pbs=10000,
                green_packet_action=SAI_PACKET_ACTION_FORWARD,
                yellow_packet_action=SAI_PACKET_ACTION_DROP,
                red_packet_action=SAI_PACKET_ACTION_DROP)

            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=self.acl_table_oid)
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter),
                enable=True)

            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)

            # dst_mac
            dst_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.dmac1),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.mac_mask))

            set_policer_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=permit_policer))

            set_user_trap_id = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(oid=self.udt1))

            print("Add acl_entry with match on dst_mac without attaching "
                  "policer id")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_mac=dst_mac_t,
                action_set_user_trap_id=set_user_trap_id,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # forward
            print("Verify packet forward action without meter update")
            for _ in range(0, 20):
                send_packet(self, self.dev_port24, self.pkt)
                verify_packet(self, self.exp_pkt, self.dev_port25)

            time.sleep(5)
            stats = sai_thrift_get_policer_stats(self.client, permit_policer)
            self.assertEqual(stats['SAI_POLICER_STAT_PACKETS'], 0)

            print("Update policer ID to acl entry")
            sai_thrift_set_acl_entry_attribute(
                self.client,
                acl_entry_oid,
                action_set_policer=set_policer_id)

            print("Verify packet forward action with meter update")
            for _ in range(0, 20):
                send_packet(self, self.dev_port24, self.pkt)
                verify_packet(self, self.exp_pkt, self.dev_port25)

            time.sleep(5)
            stats = sai_thrift_get_policer_stats(self.client, permit_policer)
            self.assertEqual(stats['SAI_POLICER_STAT_PACKETS'], 20)

            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            if acl_counter:
                sai_thrift_remove_acl_counter(self.client, acl_counter)
            if permit_policer:
                sai_thrift_remove_policer(self.client, permit_policer)

    def testIngressAclFields(self):
        '''
        Testing every field hit in this table
        '''
        print("testIngressAclFields")
        acl_entry_oid = None
        try:
            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=self.acl_table_oid)
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter),
                enable=True)

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_DROP))

            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)
            send_packet(self, self.dev_port24, self.pktv6)
            verify_packet(self, self.exp_pktv6, self.dev_port25)

            # ip_type
            ipv4_type = SAI_ACL_IP_TYPE_IPV4ANY
            ipv6_type = SAI_ACL_IP_TYPE_IPV6ANY
            for ip_type, pkt in zip([ipv4_type, ipv6_type],
                                    [self.pkt, self.pktv6]):
                ip_type_t = sai_thrift_acl_field_data_t(
                    data=sai_thrift_acl_field_data_data_t(s32=ip_type))

                print("Add acl_entry with match on ip_type", ip_type)
                acl_entry_oid = sai_thrift_create_acl_entry(
                    self.client,
                    table_id=self.acl_table_oid,
                    priority=10,
                    field_acl_ip_type=ip_type_t,
                    action_packet_action=packet_action,
                    action_counter=action_counter_t)
                self.assertTrue(acl_entry_oid != 0)

                send_packet(self, self.dev_port24, pkt)
                verify_no_other_packets(self, timeout=1)

                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                self.assertEqual(packets['packets'], 1)
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
                acl_entry_oid = None

            # dst_mac
            dst_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.dmac1),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.mac_mask))

            print("Add acl_entry with match on dst_mac")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_mac=dst_mac_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self, timeout=1)

            # # never drop ctrl packet
            # send_packet(self, self.dev_port24, self.ctrl_pkt)
            # verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=self.dst_ip_addr),
                mask=sai_thrift_acl_field_data_mask_t(ip4=self.ip_mask))

            print("Add acl_entry with match on dst_ip")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_dst_ip=dst_ip_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            dst_ipv6_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=self.dst_ipv6_addr),
                mask=sai_thrift_acl_field_data_mask_t(ip6=self.ipv6_mask))

            print("Add acl_entry with match on dst_ipv6")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_dst_ipv6=dst_ipv6_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pktv6)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=self.src_ip_addr),
                mask=sai_thrift_acl_field_data_mask_t(ip4=self.ip_mask))

            print("Add acl_entry with match on src_ip")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_ip=src_ip_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            src_ipv6_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=self.src_ipv6_addr),
                mask=sai_thrift_acl_field_data_mask_t(ip6=self.ipv6_mask))

            print("Add acl_entry with match on src_ipv6")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_src_ipv6=src_ipv6_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pktv6)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            ip = 0x0800
            ipv6 = 0x6DD  # thift 2s complement
            arp = 0x0806
            for etype, pkt in zip([arp, ip, ipv6, ip, ipv6],
                                  [self.arp_pkt, self.pkt, self.pktv6,
                                   self.icmp4_pkt, self.icmp6_pkt]):
                etype_t = sai_thrift_acl_field_data_t(
                    data=sai_thrift_acl_field_data_data_t(u16=etype),
                    mask=sai_thrift_acl_field_data_mask_t(u16=0x7FFF))

                print("Add acl_entry with match on ether type", etype)
                acl_entry_oid = sai_thrift_create_acl_entry(
                    self.client,
                    table_id=self.acl_table_oid,
                    priority=10,
                    field_ether_type=etype_t,
                    action_packet_action=packet_action,
                    action_counter=action_counter_t)
                self.assertTrue(acl_entry_oid != 0)

                # send packet to drop
                send_packet(self, self.dev_port24, pkt)
                verify_no_other_packets(self, timeout=1)

                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                self.assertEqual(packets['packets'], 1)
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
                acl_entry_oid = None

            pkt_dscp = copy.deepcopy(self.pkt)
            pkt_dscp['IP'].tos = 8
            exp_pkt_dscp = copy.deepcopy(self.exp_pkt)
            exp_pkt_dscp['IP'].tos = 8
            dscp_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=2),
                mask=sai_thrift_acl_field_data_mask_t(u8=127))

            print("Add acl_entry with match on dscp")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_dscp=dscp_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, pkt_dscp)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            pkt_ecn = copy.deepcopy(self.pkt)
            pkt_ecn['IP'].tos = 2
            exp_pkt_ecn = copy.deepcopy(self.exp_pkt)
            exp_pkt_ecn['IP'].tos = 2
            ecn_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=2),
                mask=sai_thrift_acl_field_data_mask_t(u8=127))

            print("Add acl_entry with match on ecn")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_ecn=ecn_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, pkt_ecn)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            ttl_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=0x40),
                mask=sai_thrift_acl_field_data_mask_t(u8=127))

            print("Add acl_entry with match on ttl")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_ttl=ttl_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            l4_dst_port_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u16=1111),
                mask=sai_thrift_acl_field_data_mask_t(u16=0x7FFF))

            print("Add acl_entry with match on l4 dst port")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_l4_dst_port=l4_dst_port_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            tcp = 0x6
            icmp = 0x1
            icmpv6 = 0x3A
            for proto, pkt in zip([tcp, icmp, icmpv6],
                                  [self.pktv6, self.icmp4_pkt,
                                   self.icmp6_pkt]):
                ip_protocol_t = sai_thrift_acl_field_data_t(
                    data=sai_thrift_acl_field_data_data_t(u8=proto),
                    mask=sai_thrift_acl_field_data_mask_t(u8=127))

                print("Add acl_entry with match on ip protocol", proto)
                acl_entry_oid = sai_thrift_create_acl_entry(
                    self.client,
                    table_id=self.acl_table_oid,
                    priority=10,
                    field_ip_protocol=ip_protocol_t,
                    action_packet_action=packet_action,
                    action_counter=action_counter_t)
                self.assertTrue(acl_entry_oid != 0)

                # send packet to drop
                send_packet(self, self.dev_port24, pkt)
                verify_no_other_packets(self, timeout=1)

                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                self.assertEqual(packets['packets'], 1)
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
                acl_entry_oid = None

            icmp_type_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=8),
                mask=sai_thrift_acl_field_data_mask_t(u8=127))

            print("Add acl_entry with match on icmp type")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_icmp_type=icmp_type_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.icmp4_pkt)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            # router solicitation is 133 (0x85). Thrift can't do 2s complement
            icmpv6_type_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=5),
                mask=sai_thrift_acl_field_data_mask_t(u8=127))

            print("Add acl_entry with match on icmpv6 type")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_icmpv6_type=icmpv6_type_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.icmp6_pkt)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            # udf_t = sai_thrift_acl_field_data_t(
            #     data=sai_thrift_acl_field_data_data_t(u8=8),
            #     mask=sai_thrift_acl_field_data_mask_t(u8=127))

            # print("Add acl_entry with match on udf")
            # acl_entry_oid = sai_thrift_create_acl_entry(
            #     self.client,
            #     table_id=self.acl_table_oid,
            #     priority=10,
            #     field_icmp_type=icmp_type_t,
            #     action_packet_action=packet_action,
            #     action_counter=action_counter_t)
            # self.assertTrue(acl_entry_oid != 0)

            # # send packet to drop
            # send_packet(self, self.dev_port24, self.icmp4_pkt)
            # verify_no_other_packets(self, timeout=1)

            # packets = sai_thrift_get_acl_counter_attribute(
            #     self.client, acl_counter, packets=True)
            # self.assertEqual(packets['packets'], 1)
            # sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            # acl_entry_oid = None

            for port, dev_port in zip([self.port24, self.port25],
                                      [self.dev_port24, self.dev_port25]):
                in_port_t = sai_thrift_acl_field_data_t(
                    data=sai_thrift_acl_field_data_data_t(oid=port))

                print("Add acl_entry with match on in_port", dev_port)
                acl_entry_oid = sai_thrift_create_acl_entry(
                    self.client,
                    table_id=self.acl_table_oid,
                    priority=10,
                    field_in_port=in_port_t,
                    action_packet_action=packet_action,
                    action_counter=action_counter_t)
                self.assertTrue(acl_entry_oid != 0)

                # send packet to drop
                send_packet(self, dev_port, self.pkt)
                verify_no_other_packets(self, timeout=1)

                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                self.assertEqual(packets['packets'], 1)
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
                acl_entry_oid = None

            # grouped fields tests
            ip_type_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    s32=SAI_ACL_IP_TYPE_IPV4ANY))
            ttl_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=0x40),
                mask=sai_thrift_acl_field_data_mask_t(u8=127))

            print("Drop all ipv4 packets with ttl 64")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_acl_ip_type=ip_type_t,
                field_ttl=ttl_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self, timeout=1)
            # do not drop ipv6 packet
            send_packet(self, self.dev_port24, self.pktv6)
            verify_packet(self, self.exp_pktv6, self.dev_port25)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            l4_dst_port_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u16=1111),
                mask=sai_thrift_acl_field_data_mask_t(u16=0x7FFF))
            ip_type_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    s32=SAI_ACL_IP_TYPE_IPV6ANY))

            print("Drop all ipv6 packets with l4 dst port 1111")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_acl_ip_type=ip_type_t,
                field_l4_dst_port=l4_dst_port_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pktv6)
            verify_no_other_packets(self, timeout=1)
            # do not drop ipv4 packet
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port25)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

            dst_user_meta_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u32=10),
                mask=sai_thrift_acl_field_data_mask_t(u32=0x7f))

            print("Add acl_entry with match on route_dst_user_meta 10")
            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_route_dst_user_meta=dst_user_meta_t,
                action_packet_action=packet_action,
                action_counter=action_counter_t)
            self.assertTrue(acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self, timeout=1)

            # never drop ctrl packet
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            acl_entry_oid = None

        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            sai_thrift_remove_acl_counter(self.client, acl_counter)


class AclIngressIntegrationTest(SaiHelper):
    '''
    Test ingress ACL
    '''

    dmac1 = '00:11:22:33:44:55'
    dmac2 = '00:11:22:33:44:56'
    smac1 = '00:22:22:33:44:55'
    mac_mask = 'ff:ff:ff:ff:ff:ff'
    ctrl_dst_ip_addr = '11.1.1.1'
    dst_ip_addr = '10.1.1.1'
    src_ip_addr = '20.1.1.1'
    ip_mask = '255.255.255.255'

    pkt = simple_tcp_packet(
        eth_dst=ROUTER_MAC,
        eth_src=smac1,
        ip_dst=dst_ip_addr,
        ip_ttl=64)

    exp_pkt = simple_tcp_packet(
        eth_src=ROUTER_MAC,
        eth_dst=dmac2,
        ip_dst=dst_ip_addr,
        ip_ttl=63)

    ctrl_pkt = simple_tcp_packet(
        eth_dst=ROUTER_MAC,
        ip_dst=dst_ip_addr,
        ip_ttl=64)

    ctrl_exp_pkt = simple_tcp_packet(
        eth_src=ROUTER_MAC,
        eth_dst=dmac1,
        ip_dst=dst_ip_addr,
        ip_ttl=63)

    icmp_pkt = simple_icmp_packet(
        eth_dst=ROUTER_MAC,
        ip_src=src_ip_addr,
        ip_dst=dst_ip_addr,
        icmp_type=8,
        icmp_data='000102030405')

    def setUp(self):
        super(AclIngressIntegrationTest, self).setUp()

        self.vrf1 = sai_thrift_create_virtual_router(self.client)

        self.port24_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)

        self.port25_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port25)

        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port25_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port25_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry1, dst_mac_address=self.dmac1)

        self.port26_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port26)

        self.nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.20.2'),
            router_interface_id=self.port26_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.port26_rif, ip_address=sai_ipaddress('20.20.20.2'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry2, dst_mac_address=self.dmac2)

        self.route_entry0 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.1.1.0/24'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry0, next_hop_id=self.nhop1)

        self.route_entry0_vrf1 = sai_thrift_route_entry_t(
            vr_id=self.vrf1, destination=sai_ipprefix('10.1.1.0/24'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry0_vrf1, next_hop_id=self.nhop2)

    def runTest(self):
        print()
        pre_ingress_acl_table_oid = None
        pre_ingress_acl_entry_oid = None
        ingress_acl_table_oid = None
        icmp_acl_entry_oid = None
        arp_acl_entry_oid = None
        try:
            table_stage = SAI_ACL_STAGE_PRE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            pre_ingress_acl_table_oid = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_acl_ip_type=True,
                field_src_mac=True,
                field_ether_type=True,
                field_dst_ip=True,
                field_dst_ipv6=True,
                field_dscp=True,
                field_in_port=True)
            self.assertTrue(pre_ingress_acl_table_oid != 0)

            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            ingress_acl_table_oid = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_acl_ip_type=True,
                field_ether_type=True,
                field_dst_mac=True,
                field_src_ip=True,
                field_dst_ip=True,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_ttl=True,
                field_dscp=True,
                field_ecn=True,
                field_ip_protocol=True,
                field_icmp_type=True,
                field_icmpv6_type=True,
                field_l4_dst_port=True,
                field_in_port=True,
                user_defined_field_group_min=True)
            self.assertTrue(ingress_acl_table_oid != 0)
            sai_thrift_set_switch_attribute(self.client,
                                            ingress_acl=ingress_acl_table_oid)

            # bind pre-ingress table to switch
            sai_thrift_set_switch_attribute(
                self.client,
                pre_ingress_acl=pre_ingress_acl_table_oid)

            src_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.smac1),
                mask=sai_thrift_acl_field_data_mask_t(mac=self.mac_mask))

            action_set_vrf = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=self.vrf1))

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_FORWARD))

            pre_ingress_acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=pre_ingress_acl_table_oid,
                priority=10,
                field_src_mac=src_mac_t,
                action_packet_action=packet_action,
                action_set_vrf=action_set_vrf)
            self.assertTrue(pre_ingress_acl_entry_oid != 0)

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_DROP))

            icmp_type_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=8),
                mask=sai_thrift_acl_field_data_mask_t(u8=127))

            print("Add acl_entry with match on icmp type")
            icmp_acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=ingress_acl_table_oid,
                priority=10,
                field_icmp_type=icmp_type_t,
                action_packet_action=packet_action)
            self.assertTrue(icmp_acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, self.icmp_pkt)
            verify_no_other_packets(self, timeout=1)

            etype_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u16=0x0806),
                mask=sai_thrift_acl_field_data_mask_t(u16=0x7FFF))

            print("Add acl_entry with match on arp packets")
            arp_acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=ingress_acl_table_oid,
                priority=10,
                field_ether_type=etype_t,
                action_packet_action=packet_action)
            self.assertTrue(arp_acl_entry_oid != 0)

            # send packet to drop
            send_packet(self, self.dev_port24, simple_arp_packet())
            verify_no_other_packets(self, timeout=1)

            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.exp_pkt, self.dev_port26)
            send_packet(self, self.dev_port24, self.ctrl_pkt)
            verify_packet(self, self.ctrl_exp_pkt, self.dev_port25)

        finally:
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_set_switch_attribute(self.client, pre_ingress_acl=0)
            if icmp_acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, icmp_acl_entry_oid)
            if arp_acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, arp_acl_entry_oid)
            if pre_ingress_acl_entry_oid:
                sai_thrift_remove_acl_entry(
                    self.client, pre_ingress_acl_entry_oid)
            if pre_ingress_acl_table_oid:
                sai_thrift_remove_acl_table(
                    self.client, pre_ingress_acl_table_oid)
            if ingress_acl_table_oid:
                sai_thrift_remove_acl_table(self.client, ingress_acl_table_oid)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry0_vrf1)
        sai_thrift_remove_route_entry(self.client, self.route_entry0)
        sai_thrift_remove_next_hop(self.client, self.nhop2)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry2)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_router_interface(self.client, self.port26_rif)
        sai_thrift_remove_router_interface(self.client, self.port25_rif)
        sai_thrift_remove_router_interface(self.client, self.port24_rif)
        sai_thrift_remove_virtual_router(self.client, self.vrf1)

        super(AclIngressIntegrationTest, self).tearDown()
