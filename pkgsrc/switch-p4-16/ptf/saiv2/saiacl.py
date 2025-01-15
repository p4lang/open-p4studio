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

from sai_base_test import *

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))

from common.utils import *  # noqa pylint: disable=wrong-import-position


class AclGroupTest(SaiHelper):
    '''
    ACL group test class
    '''

    def setUp(self):
        super(AclGroupTest, self).setUp()

        self.port_mac = '00:11:22:33:44:55'
        self.lag_mac = '00:11:22:33:44:56'
        self.port_mac2 = '00:11:22:33:44:57'
        self.lag_mac2 = '00:11:22:33:44:58'

        self.port_fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.port_mac,
            bv_id=self.vlan10)
        sai_thrift_create_fdb_entry(
            self.client,
            self.port_fdb_entry,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port0_bp)

        self.lag_fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.lag_mac,
            bv_id=self.vlan10)
        sai_thrift_create_fdb_entry(
            self.client,
            self.lag_fdb_entry,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.lag1_bp)

        # create bridge ports
        self.port24_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port24_bp != 0)

        self.port25_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port25,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port25_bp != 0)

        # create LAGs
        self.lag6 = sai_thrift_create_lag(self.client)
        self.assertTrue(self.lag6 != 0)
        self.lag6_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.lag6,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.lag6_bp != 0)
        self.lag6_member26 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag6, port_id=self.port26)
        self.lag6_member27 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag6, port_id=self.port27)
        self.lag6_member28 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag6, port_id=self.port28)

        # create vlan 40 with port24, port25 and lag6
        self.vlan40 = sai_thrift_create_vlan(self.client, vlan_id=40)
        self.assertTrue(self.vlan40 != 0)
        self.vlan40_member24 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan40,
            bridge_port_id=self.port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan40_member25 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan40,
            bridge_port_id=self.port25_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)
        self.vlan40_member_lag6 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan40,
            bridge_port_id=self.lag6_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # setup untagged ports
        sai_thrift_set_port_attribute(
            self.client, self.port24, port_vlan_id=40)
        sai_thrift_set_lag_attribute(self.client, self.lag6, port_vlan_id=40)

        self.port_fdb_entry2 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.port_mac2,
            bv_id=self.vlan40)
        sai_thrift_create_fdb_entry(
            self.client,
            self.port_fdb_entry2,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port24_bp)

        self.lag_fdb_entry2 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.lag_mac2,
            bv_id=self.vlan40)
        sai_thrift_create_fdb_entry(
            self.client,
            self.lag_fdb_entry2,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.lag6_bp)

    def runTest(self):
        self.portLagIngressAclTableGroupTest()
        self.portLagEgressAclTableGroupTest()

    def tearDown(self):
        sai_thrift_remove_fdb_entry(self.client, self.port_fdb_entry)
        sai_thrift_remove_fdb_entry(self.client, self.lag_fdb_entry)
        sai_thrift_remove_fdb_entry(self.client, self.port_fdb_entry2)
        sai_thrift_remove_fdb_entry(self.client, self.lag_fdb_entry2)

        sai_thrift_set_port_attribute(self.client, self.port24, port_vlan_id=0)
        sai_thrift_set_lag_attribute(self.client, self.lag6, port_vlan_id=0)

        # remove vlan config
        sai_thrift_remove_vlan_member(self.client, self.vlan40_member_lag6)
        sai_thrift_remove_vlan_member(self.client, self.vlan40_member25)
        sai_thrift_remove_vlan_member(self.client, self.vlan40_member24)
        sai_thrift_remove_vlan(self.client, self.vlan40)

        # remove lag config
        sai_thrift_remove_lag_member(self.client, self.lag6_member28)
        sai_thrift_remove_lag_member(self.client, self.lag6_member27)
        sai_thrift_remove_lag_member(self.client, self.lag6_member26)
        sai_thrift_remove_bridge_port(self.client, self.lag6_bp)
        sai_thrift_remove_lag(self.client, self.lag6)

        # remove bridge ports
        sai_thrift_remove_bridge_port(self.client, self.port25_bp)
        sai_thrift_remove_bridge_port(self.client, self.port24_bp)

        super(AclGroupTest, self).tearDown()

    def portLagIngressAclTableGroupTest(self):
        '''
        Verify combination of port and LAG as
        bind points to ingress ACL table group.
        '''
        print("portLagIngressAclTableGroupTest")

        # create ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                             SAI_ACL_BIND_POINT_TYPE_LAG]
        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_points), int32list=group_bind_points)
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        acl_group = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=group_stage,
            acl_bind_point_type_list=group_bind_point_type_list,
            type=group_type)

        # create ACL table
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                             SAI_ACL_BIND_POINT_TYPE_LAG]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)
        acl_table = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        # create ACL table entry
        src_ip = '10.0.0.1'
        src_ip2 = '10.0.0.2'
        src_ip_mask = '255.255.255.255'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        # add ACL table group member
        member1 = sai_thrift_create_acl_table_group_member(
            self.client,
            acl_table_group_id=acl_group,
            acl_table_id=acl_table)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry,
            action_counter=action_counter_t)

        try:
            pkt1 = simple_udp_packet(
                eth_dst=self.port_mac,
                eth_src=self.lag_mac,
                ip_src=src_ip,
                pktlen=100)
            pkt2 = simple_udp_packet(
                eth_dst=self.lag_mac,
                eth_src=self.port_mac,
                ip_src=src_ip,
                pktlen=100)

            pkt3 = simple_udp_packet(
                eth_dst=self.port_mac2,
                eth_src=self.lag_mac2,
                ip_src=src_ip2,
                pktlen=100)
            pkt4 = simple_udp_packet(
                eth_dst=self.lag_mac2,
                eth_src=self.port_mac2,
                ip_src=src_ip2,
                pktlen=100)

            print("Sending packet without ACL table group")
            print("Sending packet from lag to port")
            send_packet(self, self.dev_port4, pkt1)
            verify_packet(self, pkt1, self.dev_port0)

            print("Sending packet from port to lag")
            send_packet(self, self.dev_port0, pkt2)
            verify_packet_any_port(
                self, pkt2,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            print("Sending packet from lag2 to port2")
            send_packet(self, self.dev_port26, pkt3)
            verify_packet(self, pkt3, self.dev_port24)

            print("Sending packet from port2 to lag2")
            send_packet(self, self.dev_port24, pkt4)
            verify_packet_any_port(
                self, pkt4,
                [self.dev_port26, self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            print("Attach ACL table group to port")
            sai_thrift_set_port_attribute(self.client, self.port0,
                                          ingress_acl=acl_group)
            print("Sending packet from port to lag, drop")
            send_packet(self, self.dev_port0, pkt2)
            verify_no_other_packets(self)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Sending packet from port2 to lag2, do not drop")
            send_packet(self, self.dev_port24, pkt4)
            verify_packet_any_port(
                self, pkt4,
                [self.dev_port26, self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Attach ACL table group to lag")
            sai_thrift_set_lag_attribute(self.client, self.lag1,
                                         ingress_acl=acl_group)
            print("Sending packet from lag to port, drop")
            send_packet(self, self.dev_port4, pkt1)
            verify_no_other_packets(self)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)

            print("Sending packet from lag to port, drop")
            send_packet(self, self.dev_port26, pkt3)
            verify_packet(self, pkt3, self.dev_port24)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_t)

            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter, packets=None)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            sai_thrift_remove_acl_counter(self.client, acl_counter)

            sai_thrift_set_port_attribute(self.client, self.port0,
                                          ingress_acl=0)
            sai_thrift_set_lag_attribute(self.client, self.lag1,
                                         ingress_acl=0)

            sai_thrift_remove_acl_table_group_member(self.client, member1)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)
            sai_thrift_remove_acl_table_group(self.client, acl_group)

    def portLagEgressAclTableGroupTest(self):
        '''
        Verify combination of port and LAG as
        bind points to egress ACL table group.
        '''
        print("portLagEgressAclTableGroupTest")
        # create ACL table group
        group_stage = SAI_ACL_STAGE_EGRESS
        group_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                             SAI_ACL_BIND_POINT_TYPE_LAG]
        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_points), int32list=group_bind_points)
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        acl_group = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=group_stage,
            acl_bind_point_type_list=group_bind_point_type_list,
            type=group_type)

        # create ACL table
        table_stage = SAI_ACL_STAGE_EGRESS
        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                             SAI_ACL_BIND_POINT_TYPE_LAG]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)
        acl_table = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        # create ACL table entry
        src_ip = '10.0.0.1'
        src_ip2 = '10.0.0.2'
        src_ip_mask = '255.255.255.255'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        # add ACL table group member
        member1 = sai_thrift_create_acl_table_group_member(
            self.client,
            acl_table_group_id=acl_group,
            acl_table_id=acl_table)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry,
            action_counter=action_counter_t)

        try:
            pkt1 = simple_udp_packet(
                eth_dst=self.port_mac,
                eth_src=self.lag_mac,
                ip_src=src_ip,
                pktlen=100)
            pkt2 = simple_udp_packet(
                eth_dst=self.lag_mac,
                eth_src=self.port_mac,
                ip_src=src_ip,
                pktlen=100)

            pkt3 = simple_udp_packet(
                eth_dst=self.port_mac2,
                eth_src=self.lag_mac2,
                ip_src=src_ip2,
                pktlen=100)
            pkt4 = simple_udp_packet(
                eth_dst=self.lag_mac2,
                eth_src=self.port_mac2,
                ip_src=src_ip2,
                pktlen=100)

            print("Sending packet without ACL table group")
            print("Sending packet from lag to port")
            send_packet(self, self.dev_port4, pkt1)
            verify_packet(self, pkt1, self.dev_port0)

            print("Sending packet from port to lag")
            send_packet(self, self.dev_port0, pkt2)
            verify_packet_any_port(
                self, pkt2,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            print("Sending packet from lag2 to port2")
            send_packet(self, self.dev_port26, pkt3)
            verify_packet(self, pkt3, self.dev_port24)

            print("Sending packet from port2 to lag2")
            send_packet(self, self.dev_port24, pkt4)
            verify_packet_any_port(
                self, pkt4,
                [self.dev_port26, self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            print("Attach ACL table group to port")
            sai_thrift_set_port_attribute(self.client, self.port0,
                                          egress_acl=acl_group)
            print("Sending packet from lag to port, drop")
            send_packet(self, self.dev_port4, pkt1)
            verify_no_other_packets(self)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Sending packet from lag to port, drop")
            send_packet(self, self.dev_port26, pkt3)
            verify_packet(self, pkt3, self.dev_port24)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Attach ACL table group to lag")
            sai_thrift_set_lag_attribute(self.client, self.lag1,
                                         egress_acl=acl_group)
            print("Sending packet from port to lag, drop")
            send_packet(self, self.dev_port0, pkt2)
            verify_no_other_packets(self)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)

            print("Sending packet from port2 to lag2, do not drop")
            send_packet(self, self.dev_port24, pkt4)
            verify_packet_any_port(
                self, pkt4,
                [self.dev_port26, self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_t)

            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter, packets=None)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            sai_thrift_remove_acl_counter(self.client, acl_counter)

            sai_thrift_set_port_attribute(self.client, self.port0,
                                          egress_acl=0)
            sai_thrift_set_lag_attribute(self.client, self.lag1,
                                         egress_acl=0)

            sai_thrift_remove_acl_table_group_member(self.client, member1)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)
            sai_thrift_remove_acl_table_group(self.client, acl_group)


@group('acl')
@group('acl-ocp')
class SrcIpAclTest(SaiHelper):
    """
    Verify matching on src ip address field
    """

    def setUp(self):
        super(SrcIpAclTest, self).setUp()

        l4_src_port = 1000

        rif_id1 = self.port10_rif
        self.rif_id2 = self.port11_rif

        ip_addr_subnet = '172.16.10.0'
        ip_addr = '172.16.10.1'
        dmac = '00:11:22:33:44:55'
        mac_src = '00:22:22:22:22:22'
        ip_addr_src = '192.168.100.100'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  ip_addr, rif_id1, dmac))
        self.nbr_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry, dst_mac_address=dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (ip_addr, rif_id1))
        self.nhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(ip_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create route with %s ip prefix and %d router"
              " interface id" % (ip_addr_subnet, rif_id1))
        self.route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                ip_addr_subnet))
        sai_thrift_create_route_entry(self.client, self.route_entry,
                                      next_hop_id=rif_id1)

        self.pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     eth_src=mac_src,
                                     ip_dst=ip_addr,
                                     ip_src=ip_addr_src,
                                     tcp_sport=l4_src_port,
                                     ip_id=105,
                                     ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                         eth_src=ROUTER_MAC,
                                         ip_dst=ip_addr,
                                         ip_src=ip_addr_src,
                                         tcp_sport=l4_src_port,
                                         ip_id=105,
                                         ip_ttl=63)

    def runTest(self):
        print("Testing SrcIpAclTest")

        print('--------------------------------------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 "
              "--->172.16.10.1 [id = 105])")

        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            verify_packets(self, self.exp_pkt, [self.dev_port10])
        finally:
            print('----------------------------------------------------------'
                  '------------------------------------')

        print("Sending packet ptf_intf 2 -[ACL]-> ptf_intf 1 (192.168.0.1"
              "-[ACL]-> 172.16.10.1 [id = 105])")

        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        table_stage_egress = SAI_ACL_STAGE_EGRESS
        entry_priority = 1
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"

        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)

        acl_ingress_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        self.assertNotEqual(acl_ingress_table_id, 0)

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_src_mask))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_ingress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_table_id,
            priority=entry_priority,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        self.assertNotEqual(acl_ingress_entry_id, 0)

        # create ACL counter
        acl_counter_ingress = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id,
            action_counter=action_counter_ingress)

        # bind this ACL table to rif_id2s object id
        sai_thrift_set_router_interface_attribute(
            self.client, self.rif_id2, ingress_acl=acl_ingress_table_id)

        try:
            print('#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT'
                  ' 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)

            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, ingress_acl=int(SAI_NULL_OBJECT_ID))

            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_ingress_entry_id,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)

            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_ingress_table_id)

            acl_egress_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage_egress,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_ip=True)

            self.assertNotEqual(acl_egress_table_id, 0)

            acl_egress_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_egress_table_id,
                priority=entry_priority,
                field_src_ip=src_ip_t,
                action_packet_action=packet_action)

            self.assertNotEqual(acl_egress_entry_id, 0)

            # create ACL counter
            acl_counter_egress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_egress_table_id)

            # attach ACL counter to ACL entry
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_entry_id,
                action_counter=action_counter_egress)

            # bind this ACL table to rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=acl_egress_table_id)

            print('#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT'
                  ' 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=int(SAI_NULL_OBJECT_ID))

            # cleanup ACL\
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_entry_id,
                action_counter=action_counter_egress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress)

            sai_thrift_remove_acl_entry(self.client, acl_egress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_egress_table_id)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry)
        super(SrcIpAclTest, self).tearDown()


@group('acl')
@group('acl-ocp')
class AdminStateAclTest(SaiHelper):
    """
    Verify ACL entry admin state
    """

    def setUp(self):
        super(AdminStateAclTest, self).setUp()

        l4_src_port = 1000

        rif_id1 = self.port10_rif
        self.rif_id2 = self.port11_rif

        ip_addr_subnet = '172.16.10.0'
        ip_addr = '172.16.10.1'
        dmac = '00:11:22:33:44:55'
        mac_src = '00:22:22:22:22:22'
        ip_addr_src = '192.168.100.100'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  ip_addr, rif_id1, dmac))
        self.nbr_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry, dst_mac_address=dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (ip_addr, rif_id1))
        self.nhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(ip_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create route with %s ip prefix and %d router"
              " interface id" % (ip_addr_subnet, rif_id1))
        self.route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                ip_addr_subnet))
        sai_thrift_create_route_entry(self.client, self.route_entry,
                                      next_hop_id=rif_id1)

        self.pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     eth_src=mac_src,
                                     ip_dst=ip_addr,
                                     ip_src=ip_addr_src,
                                     tcp_sport=l4_src_port,
                                     ip_id=105,
                                     ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                         eth_src=ROUTER_MAC,
                                         ip_dst=ip_addr,
                                         ip_src=ip_addr_src,
                                         tcp_sport=l4_src_port,
                                         ip_id=105,
                                         ip_ttl=63)

    def runTest(self):
        print("Testing AdminStateAclTest")

        print('--------------------------------------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 "
              "--->172.16.10.1 [id = 105])")

        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            verify_packets(self, self.exp_pkt, [self.dev_port10])
        finally:
            print('----------------------------------------------------------'
                  '------------------------------------')

        print("Sending packet ptf_intf 2 -[ACL]-> ptf_intf 1 (192.168.0.1"
              "-[ACL]-> 172.16.10.1 [id = 105])")

        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        entry_priority = 1
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"

        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)

        acl_ingress_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        self.assertNotEqual(acl_ingress_table_id, 0)

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_src_mask))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_ingress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_table_id,
            priority=entry_priority,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action,
            admin_state=True)

        self.assertNotEqual(acl_ingress_entry_id, 0)

        attr = sai_thrift_get_acl_entry_attribute(
            self.client,
            acl_ingress_entry_id,
            admin_state=True)
        self.assertTrue(attr['admin_state'] is True)

        attr = sai_thrift_set_acl_entry_attribute(
            self.client,
            acl_ingress_entry_id,
            admin_state=False)
        attr = sai_thrift_get_acl_entry_attribute(
            self.client,
            acl_ingress_entry_id,
            admin_state=True)
        self.assertTrue(attr['admin_state'] is False)

        # create ACL counter
        acl_counter_ingress = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id,
            action_counter=action_counter_ingress)

        # bind this ACL table to rif_id2s object id
        sai_thrift_set_router_interface_attribute(
            self.client, self.rif_id2, ingress_acl=acl_ingress_table_id)

        try:
            print('#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT'
                  ' 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, ingress_acl=int(SAI_NULL_OBJECT_ID))

            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_ingress_entry_id,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)

            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_ingress_table_id)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry)
        super(AdminStateAclTest, self).tearDown()


@group('acl')
@group('acl-ocp')
class DstIpAclTest(SaiHelper):
    """
    Verify matching on dst ip address field
    """

    def setUp(self):
        super(DstIpAclTest, self).setUp()

        l4_dst_port = 1000

        rif_id1 = self.port10_rif
        self.rif_id2 = self.port11_rif

        ip_addr_subnet = '172.16.10.0'
        ip_addr = '172.16.10.1'
        dmac = '00:11:22:33:44:55'
        mac_src = '00:22:22:22:22:22'
        ip_addr_src = '192.168.100.100'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  ip_addr, rif_id1, dmac))
        self.nbr_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry, dst_mac_address=dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (ip_addr, rif_id1))
        self.nhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(ip_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create route with %s ip prefix and %d router"
              " interface id" % (ip_addr_subnet, rif_id1))
        self.route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                ip_addr_subnet))
        sai_thrift_create_route_entry(self.client, self.route_entry,
                                      next_hop_id=rif_id1)

        self.pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     eth_src=mac_src,
                                     ip_dst=ip_addr,
                                     ip_src=ip_addr_src,
                                     tcp_sport=l4_dst_port,
                                     ip_id=105,
                                     ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                         eth_src=ROUTER_MAC,
                                         ip_dst=ip_addr,
                                         ip_src=ip_addr_src,
                                         tcp_sport=l4_dst_port,
                                         ip_id=105,
                                         ip_ttl=63)

    def runTest(self):
        print("Testing DstIpAclTest")

        print('--------------------------------------------------------------'
              '--------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 "
              "---> 172.16.10.1 [id = 105])")

        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            verify_packets(self, self.exp_pkt, [self.dev_port10])
        finally:
            print('----------------------------------------------------------')

        print("Sending packet ptf_intf 2 -[ACL]-> ptf_intf 1 (192.168.0.1"
              "-[ACL]-> 172.16.10.1 [id = 105])")

        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        table_stage_egress = SAI_ACL_STAGE_EGRESS
        entry_priority = 1
        ip_dst = "172.16.10.1"
        ip_dst_mask = "255.255.255.0"

        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)

        acl_ingress_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        self.assertNotEqual(acl_ingress_table_id, 0)

        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_dst),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_dst_mask))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_ingress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_table_id,
            priority=entry_priority,
            field_dst_ip=dst_ip_t,
            action_packet_action=packet_action)

        self.assertNotEqual(acl_ingress_entry_id, 0)

        # create ACL counter
        acl_counter_ingress = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id,
            action_counter=action_counter_ingress)

        # bind this ACL table to rif_id2s object id
        sai_thrift_set_router_interface_attribute(
            self.client, self.rif_id2, ingress_acl=acl_ingress_table_id)

        try:
            print('#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT'
                  ' 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_ingress_entry_id,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)

            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, ingress_acl=int(SAI_NULL_OBJECT_ID))

            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_ingress_table_id)

            acl_egress_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage_egress,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_dst_ip=True)

            self.assertNotEqual(acl_egress_table_id, 0)

            acl_egress_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_egress_table_id,
                priority=entry_priority,
                field_dst_ip=dst_ip_t,
                action_packet_action=packet_action)

            self.assertNotEqual(acl_egress_entry_id, 0)

            # create ACL counter
            acl_counter_egress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_egress_table_id)

            # attach ACL counter to ACL entry
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_entry_id,
                action_counter=action_counter_egress)

            # bind this ACL table to rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=acl_egress_table_id)

            print('#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT'
                  ' 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=int(SAI_NULL_OBJECT_ID))

            # cleanup ACL
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_entry_id,
                action_counter=action_counter_egress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress)

            # cleanup ACL
            sai_thrift_remove_acl_entry(self.client, acl_egress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_egress_table_id)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry)
        super(DstIpAclTest, self).tearDown()


@group('acl')
@group('acl-ocp')
class MACSrcAclTest(SaiHelper):
    """
    Verify matching on src mac address field
    """

    def setUp(self):
        super(MACSrcAclTest, self).setUp()

        rif_id1 = self.port10_rif
        self.rif_id2 = self.port11_rif

        ip_addr_subnet = '172.16.10.0'
        ip_addr = '172.16.10.1'
        dmac = '00:11:22:33:44:55'
        self.mac_src = '00:22:22:22:22:22'
        ip_addr_src = '192.168.0.1'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  ip_addr, rif_id1, dmac))
        self.nbr_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry, dst_mac_address=dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (ip_addr, rif_id1))
        self.nhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(ip_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create route with %s ip prefix and %d router"
              " interface id" % (ip_addr_subnet, rif_id1))
        self.route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                ip_addr_subnet))
        sai_thrift_create_route_entry(self.client, self.route_entry,
                                      next_hop_id=rif_id1)

        self.pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     eth_src=self.mac_src,
                                     ip_dst=ip_addr,
                                     ip_src=ip_addr_src,
                                     ip_id=105,
                                     ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                         eth_src=ROUTER_MAC,
                                         ip_dst=ip_addr,
                                         ip_src=ip_addr_src,
                                         ip_id=105,
                                         ip_ttl=63)

    def runTest(self):
        print("This test is not supported. MAC lookup cannot be done for IP "
              "Packets in switch.p4")
        # noqa pylint: disable=unreachable
        return
        print('--------------------------------------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.0.1 --->"
              " 172.16.10.1 [id = 105])")

        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.1 | @ ptf_intf 1')
            verify_packets(self, self.exp_pkt, [self.dev_port10])
        finally:
            print('----------------------------------------------------------')

            print("Sending packet ptf_intf 2 -[ACL]-> ptf_intf 1 (192.168.0.1-"
                  "[ACL]-> 172.16.10.1 [id = 105])")
            # setup ACL to block based on Source MAC

            table_stage_ingress = SAI_ACL_STAGE_INGRESS
            table_stage_egress = SAI_ACL_STAGE_EGRESS
            entry_priority = 1
            mac_src_mask = 'ff:ff:ff:ff:ff:ff'

            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)

            acl_ingress_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage_ingress,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_mac=True)

            self.assertNotEqual(acl_ingress_table_id, 0)

            src_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=self.mac_src),
                mask=sai_thrift_acl_field_data_mask_t(mac=mac_src_mask))

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_DROP))

            acl_ingress_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_ingress_table_id,
                priority=entry_priority,
                field_src_mac=src_mac_t,
                action_packet_action=packet_action)

            self.assertNotEqual(acl_ingress_entry_id, 0)

            # create ACL counter
            acl_counter_ingress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_ingress_table_id)

            # attach ACL counter to ACL entry
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_ingress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_ingress_entry_id,
                action_counter=action_counter_ingress)

            # bind this ACL table to rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, ingress_acl=acl_ingress_table_id)

        try:
            print('#### ACL \'DROP, src mac 00:22:22:22:22:22, '
                  'in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 2')
            # send the same packet
            send_packet(self, self.dev_port11, self.pkt)
            # ensure packet is dropped
            # check for absence of packet here!
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC,
                  '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_ingress_entry_id,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)

            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, ingress_acl=int(SAI_NULL_OBJECT_ID))

            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_ingress_table_id)

            acl_egress_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage_egress,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_mac=True)

            self.assertNotEqual(acl_egress_table_id, 0)

            acl_egress_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_egress_table_id,
                priority=entry_priority,
                field_src_mac=src_mac_t,
                action_packet_action=packet_action)

            self.assertNotEqual(acl_egress_entry_id, 0)

            # create ACL counter
            acl_counter_egress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_egress_table_id)

            # attach ACL counter to ACL entry
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_entry_id,
                action_counter=action_counter_egress)

            # bind this ACL table to rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=acl_egress_table_id)

            print('#### ACL \'DROP, src mac 00:22:22:22:22:22, '
                  'in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 2')
            # send the same packet
            send_packet(self, self.dev_port11, pkt)
            # ensure packet is dropped
            # check for absence of packet here!
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC,
                  '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=int(SAI_NULL_OBJECT_ID))

            # cleanup ACL
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_entry_id,
                action_counter=action_counter_egress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress)

            # cleanup ACL
            sai_thrift_remove_acl_entry(self.client, acl_egress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_egress_table_id)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry)
        super(MACSrcAclTest, self).tearDown()


@group('acl')
@group('acl-ocp')
class L3L4PortTest(SaiHelper):
    """
    Verify matching on l4_dst_port and l4_src_port fields
    """

    def setUp(self):
        super(L3L4PortTest, self).setUp()
        self.l4_dst_port = 1000
        self.l4_src_port = 500

        rif_id1 = self.port10_rif
        self.rif_id2 = self.port11_rif

        ip_addr_subnet = '172.16.10.0'
        ip_addr = '172.16.10.1'
        dmac = '00:11:22:33:44:55'
        mac_src = '00:22:22:22:22:22'
        ip_addr_src = '192.168.100.100'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  ip_addr, rif_id1, dmac))
        self.nbr_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry, dst_mac_address=dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (ip_addr, rif_id1))
        self.nhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(ip_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create route with %s ip prefix and %d router"
              " interface id" % (ip_addr_subnet, rif_id1))
        self.route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                ip_addr_subnet))
        sai_thrift_create_route_entry(self.client, self.route_entry,
                                      next_hop_id=rif_id1)

        self.pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     eth_src=mac_src,
                                     ip_dst=ip_addr,
                                     ip_src=ip_addr_src,
                                     tcp_sport=self.l4_src_port,
                                     tcp_dport=self.l4_dst_port,
                                     ip_id=105,
                                     ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                         eth_src=ROUTER_MAC,
                                         ip_dst=ip_addr,
                                         ip_src=ip_addr_src,
                                         tcp_sport=self.l4_src_port,
                                         tcp_dport=self.l4_dst_port,
                                         ip_id=105,
                                         ip_ttl=63)

    def runTest(self):
        print("Testing L4 src/dest port ACL filter")
        print('--------------------------------------------------------------'
              '--------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 --->"
              " 172.16.10.1 [id = 105])")

        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            verify_packets(self, self.exp_pkt, [self.dev_port10])
        finally:
            print('----------------------------------------------------------'
                  '------------------------------------')

        print("Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1"
              "-[a]cl]-> 172.16.10.1 [id = 105])")
        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        table_stage_egress = SAI_ACL_STAGE_EGRESS
        entry_priority = 1

        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"

        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)

        acl_ingress_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        self.assertNotEqual(acl_ingress_table_id, 0)

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_src_mask))

        l4_dst_port_mask = 32759
        l4_dst_port_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=self.l4_dst_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_dst_port_mask))

        l4_src_port_mask = 32759
        l4_src_port_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=self.l4_src_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_src_port_mask))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_ingress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_table_id,
            priority=entry_priority,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action,
            field_l4_dst_port=l4_dst_port_t,
            field_l4_src_port=l4_src_port_t)

        # create ACL counter
        acl_counter_ingress = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id,
            action_counter=action_counter_ingress)

        self.assertNotEqual(acl_ingress_entry_id, 0)

        # bind this ACL table to rif_id2s object id
        sai_thrift_set_router_interface_attribute(
            self.client, self.rif_id2, ingress_acl=acl_ingress_table_id)

        try:
            print('#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT'
                  ' 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            # send the same packet
            send_packet(self, self.dev_port11, self.pkt)
            # ensure packet is dropped
            # check for absence of packet here!
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_ingress_entry_id,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)

            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, ingress_acl=int(SAI_NULL_OBJECT_ID))

            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_ingress_table_id)

            acl_egress_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage_egress,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_ip=True)

            self.assertNotEqual(acl_egress_table_id, 0)

            acl_egress_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_egress_table_id,
                priority=entry_priority,
                field_src_ip=src_ip_t,
                action_packet_action=packet_action,
                field_l4_dst_port=l4_dst_port_t,
                field_l4_src_port=l4_src_port_t)

            # create ACL counter
            acl_counter_egress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_egress_table_id)

            # attach ACL counter to ACL entry
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_entry_id,
                action_counter=action_counter_egress)

            self.assertNotEqual(acl_egress_entry_id, 0)

            # bind this ACL table to rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=acl_egress_table_id)

            print('#### ACL \'DROP, src mac 00:22:22:22:22:22, '
                  'in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 2')
            # send the same packet
            send_packet(self, self.dev_port11, self.pkt)
            # ensure packet is dropped
            # check for absence of packet here!
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC,
                  '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=int(SAI_NULL_OBJECT_ID))

            # cleanup ACL
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_entry_id,
                action_counter=action_counter_egress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress)

            # cleanup ACL
            sai_thrift_remove_acl_entry(self.client, acl_egress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_egress_table_id)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry)
        super(L3L4PortTest, self).tearDown()


@group('acl')
@group('acl-ocp')
class L3AclRangeTest(SaiHelper):
    """
    Verify matching on ACL range
    """

    def setUp(self):
        super(L3AclRangeTest, self).setUp()

        l4_dst_port = 1000

        rif_id1 = self.port10_rif
        self.rif_id2 = self.port11_rif

        ip_addr_subnet = '172.16.10.0'
        ip_addr = '172.16.10.1'
        dmac = '00:11:22:33:44:55'
        mac_src = '00:22:22:22:22:22'
        ip_addr_src = '192.168.100.100'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  ip_addr, rif_id1, dmac))
        self.nbr_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry, dst_mac_address=dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (ip_addr, rif_id1))
        self.nhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(ip_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create route with %s ip prefix and %d router "
              "interface id" % (ip_addr_subnet, rif_id1))
        self.route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                ip_addr_subnet))
        sai_thrift_create_route_entry(self.client, self.route_entry,
                                      next_hop_id=rif_id1)

        self.tcp_pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                         eth_src=mac_src,
                                         ip_dst=ip_addr,
                                         ip_src=ip_addr_src,
                                         tcp_dport=l4_dst_port,
                                         ip_id=105,
                                         ip_ttl=64)
        self.tcp_exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                             eth_src=ROUTER_MAC,
                                             ip_dst=ip_addr,
                                             ip_src=ip_addr_src,
                                             tcp_dport=l4_dst_port,
                                             ip_id=105,
                                             ip_ttl=63)

        self.udp_pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                         eth_src=mac_src,
                                         ip_dst=ip_addr,
                                         udp_dport=l4_dst_port,
                                         ip_src=ip_addr_src,
                                         ip_id=105,
                                         ip_ttl=64)
        self.udp_exp_pkt = simple_udp_packet(eth_dst=dmac,
                                             eth_src=ROUTER_MAC,
                                             ip_dst=ip_addr,
                                             ip_src=ip_addr_src,
                                             udp_dport=l4_dst_port,
                                             ip_id=105,
                                             ip_ttl=63)

        self.table_stage_ingress = SAI_ACL_STAGE_INGRESS
        self.table_stage_egress = SAI_ACL_STAGE_EGRESS

        self.tcp_protocol = 0x06
        self.udp_protocol = 0x11

    def runTest(self):
        self.routingTest()
        print("Sending TCP packet ptf_intf 2 -[ingress ACL]-> ptf_intf 1 "
              "(192.168.0.1-[ingress ACL]-> 172.16.10.1 [id = 105])")
        self.aclTest(self.table_stage_ingress, self.tcp_protocol)
        print("Sending UDP packet ptf_intf 2 -[ingress ACL]-> ptf_intf 1 "
              "(192.168.0.1-[ingress ACL]-> 172.16.10.1 [id = 105])")
        self.aclTest(self.table_stage_ingress, self.udp_protocol)
        print("Sending TCP packet ptf_intf 2 -[egress ACL]-> ptf_intf 1 "
              "(192.168.0.1-[egress ACL]-> 172.16.10.1 [id = 105])")
        self.aclTest(self.table_stage_egress, self.tcp_protocol)
        print("Sending UDP packet ptf_intf 2 -[egress ACL]-> ptf_intf 1 "
              "(192.168.0.1-[egress ACL]-> 172.16.10.1 [id = 105])")
        self.aclTest(self.table_stage_egress, self.udp_protocol)

    def routingTest(self):
        """
        Verifies routing for TCP and UDP traffic
        """
        print('--------------------------------------------------------------')
        print("Sending TCP packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 "
              "---> 172.16.10.1 [id = 105])")
        print('#### NO ACL Applied ####')
        print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
              '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2')
        send_packet(self, self.dev_port11, self.tcp_pkt)
        print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
              '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
        verify_packets(self, self.tcp_exp_pkt, [self.dev_port10])
        print('----------------------------------------------------------')
        print("Sending UDP packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 "
              "---> 172.16.10.1 [id = 105])")
        print('#### NO ACL Applied ####')
        print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
              '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2')
        send_packet(self, self.dev_port11, self.udp_pkt)
        print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
              '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
        verify_packets(self, self.udp_exp_pkt, [self.dev_port10])
        print('----------------------------------------------------------')

    def aclTest(self, stage, protocol):
        """
        Verifies ingress or egress ACLs for range and TCP or UDP traffic

        Args:
            stage (int): specifies ingress or egress type of ACL
            protocol (int): specifies protocol field value
        """
        if protocol == 0x06:
            pkt = self.tcp_pkt
        elif protocol == 0x11:
            pkt = self.udp_pkt

        field_protocol = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=protocol),
            mask=sai_thrift_acl_field_data_mask_t(u8=0x0F))

        entry_priority = 1

        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)

        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True,
            field_ip_protocol=True)

        self.assertNotEqual(acl_table_id, 0)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        u32range = sai_thrift_u32_range_t(min=1000, max=1000)
        acl_range_id = sai_thrift_create_acl_range(
            self.client,
            type=SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE,
            limit=u32range)
        range_list = [acl_range_id]
        print("ACL range created 0x%lx" % (acl_range_id))

        range_list_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(
                objlist=sai_thrift_object_list_t(
                    count=len(range_list),
                    idlist=range_list)))

        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=entry_priority,
            action_packet_action=packet_action,
            field_acl_range_type=range_list_t,
            field_ip_protocol=field_protocol)
        print("ACL ingress table created 0x%lx" % (acl_table_id))

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id)

        # attach ACL counter to ACL entry
        action_counter = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter)

        if stage == SAI_ACL_STAGE_INGRESS:
            # bind this ACL table to rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, ingress_acl=acl_table_id)
        elif stage == SAI_ACL_STAGE_EGRESS:
            # bind this ACL table to rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.rif_id2, egress_acl=acl_table_id)

        try:
            print('#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT'
                  ' 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            # send the same packet
            send_packet(self, self.dev_port11, pkt)
            # ensure packet is dropped
            # check for absence of packet here!
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # unbind this ACL table from rif_id2s object id
            if stage == SAI_ACL_STAGE_INGRESS:
                sai_thrift_set_router_interface_attribute(
                    self.client, self.rif_id2, ingress_acl=int(
                        SAI_NULL_OBJECT_ID))
            elif stage == SAI_ACL_STAGE_EGRESS:
                sai_thrift_set_router_interface_attribute(
                    self.client, self.rif_id2, egress_acl=int(
                        SAI_NULL_OBJECT_ID))

            # cleanup ACL
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_id,
                action_counter=action_counter)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter)

            # cleanup ACL
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_range(self.client, acl_range_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry)
        super(L3AclRangeTest, self).tearDown()


@group('acl-m1')
class ACLGroupSeveralMembersTest(SaiHelper):
    """
    Verify matching on ACL groups with the IPv4 and IPv6 groups members
    """
    def setUp(self):
        super(ACLGroupSeveralMembersTest, self).setUp()

        mirror_type = SAI_MIRROR_SESSION_TYPE_LOCAL

        rif_id1 = self.port10_rif

        self.ipv4_addr = '192.168.0.1'
        self.ipv6_addr = '4000::1'
        self.dmac = '00:22:22:22:22:22'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  self.ipv4_addr, rif_id1, self.dmac))
        self.nbr_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(self.ipv4_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry1, dst_mac_address=self.dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (self.ipv4_addr, rif_id1))
        self.nhop1 = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(self.ipv4_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  self.ipv6_addr, rif_id1, self.dmac))
        self.nbr_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(self.ipv6_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry2, dst_mac_address=self.dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (self.ipv6_addr, rif_id1))
        self.nhop2 = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(self.ipv6_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create mirror session")
        self.spanid = sai_thrift_create_mirror_session(
            self.client,
            monitor_port=self.port24,
            type=mirror_type)

    def runTest(self):
        # setup ACL table groups
        ipv4_addr_src1 = "20.0.0.1"
        ipv4_addr_src2 = "20.0.0.3"
        ipv4_mask = "255.255.255.255"
        ipv6_mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"
        ipv6_addr_src = '2000::1'

        group_stage_ingress = SAI_ACL_STAGE_INGRESS
        group_stage_egress = SAI_ACL_STAGE_EGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL
        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        table_stage_egress = SAI_ACL_STAGE_EGRESS

        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_point_list), int32list=group_bind_point_list)

        print("Create ACL tables groups")
        acl_group_ingress = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=group_stage_ingress,
            acl_bind_point_type_list=group_bind_point_type_list,
            type=group_type)

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list), int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        print("Create ACL field data")
        src_ip_t_ipv4 = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ipv4_addr_src1),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ipv4_mask))

        src_ip_t_ipv6 = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=ipv6_addr_src),
            mask=sai_thrift_acl_field_data_mask_t(
                ip6=ipv6_mask))

        src_ip_t_mirror = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ipv4_addr_src2),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ipv4_mask))

        # create ACL tables
        print("Create ACL tables")
        acl_ingress_ipv4_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        acl_ingress_ipv6_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ipv6=True)

        # create ACL table group members
        print("Create ACL group members")
        acl_group_ingress_ipv4_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=acl_group_ingress,
                acl_table_id=acl_ingress_ipv4_table_id)

        acl_group_ingress_ipv6_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=acl_group_ingress,
                acl_table_id=acl_ingress_ipv6_table_id)

        # create ACL entries
        print("Create ACL entries")
        ipv4_acl_ingress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9999,
            field_src_ip=src_ip_t_ipv4,
            action_packet_action=packet_action)

        ipv6_acl_ingress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv6_table_id,
            priority=9998,
            field_src_ipv6=src_ip_t_ipv6,
            action_packet_action=packet_action)

        mirror_acl_ingress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9997,
            field_src_ip=src_ip_t_mirror,
            action_packet_action=packet_action)

        # create ACL counter
        acl_counter_ingress_ipv4 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress_ipv4 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress_ipv4),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, ipv4_acl_ingress_entry_id,
            action_counter=action_counter_ingress_ipv4)

        # create ACL counter
        acl_counter_ingress_ipv6 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv6_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress_ipv6 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress_ipv6),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, ipv6_acl_ingress_entry_id,
            action_counter=action_counter_ingress_ipv6)

        # create ACL counter
        acl_counter_ingress_mirror = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress_mirror = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress_mirror),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, mirror_acl_ingress_entry_id,
            action_counter=action_counter_ingress_mirror)

        try:
            pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                    eth_src=self.dmac,
                                    ip_src=ipv4_addr_src1,
                                    ip_dst=self.ipv4_addr,
                                    tcp_sport=0x4321,
                                    tcp_dport=0x51,
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst=self.dmac,
                                        eth_src=ROUTER_MAC,
                                        ip_src=ipv4_addr_src1,
                                        ip_dst=self.ipv4_addr,
                                        tcp_sport=0x4321,
                                        tcp_dport=0x51,
                                        ip_ttl=63)

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.1 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 20.0.0.1'
                  ' | 192.168.0.1 | @ ptf_intf 1')
            verify_packets(self, exp_pkt, [self.dev_port10])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv6, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            pktv6 = simple_tcpv6_packet(eth_dst=ROUTER_MAC,
                                        eth_src=self.dmac,
                                        ipv6_dst=self.ipv6_addr,
                                        ipv6_src=ipv6_addr_src,
                                        ipv6_hlim=64)
            exp_pktv6 = simple_tcpv6_packet(eth_dst=self.dmac,
                                            eth_src=ROUTER_MAC,
                                            ipv6_dst=self.ipv6_addr,
                                            ipv6_src=ipv6_addr_src,
                                            ipv6_hlim=63)

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 4000::1'
                  ' | 2000::1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pktv6)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 4000::1'
                  ' | 2000::1 | @ ptf_intf 1')
            verify_packets(self, exp_pktv6, [self.dev_port10])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv6, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            pkt2 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     eth_src=self.dmac,
                                     ip_src=ipv4_addr_src2,
                                     ip_dst=self.ipv4_addr,
                                     tcp_sport=0x4321,
                                     tcp_dport=0x51,
                                     ip_ttl=64)
            exp_pkt2 = simple_tcp_packet(eth_dst=self.dmac,
                                         eth_src=ROUTER_MAC,
                                         ip_src=ipv4_addr_src2,
                                         ip_dst=self.ipv4_addr,
                                         tcp_sport=0x4321,
                                         tcp_dport=0x51,
                                         ip_ttl=63)

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.3 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt2)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 20.0.0.3'
                  ' | 192.168.0.1 | @ ptf_intf 1')
            verify_packets(self, exp_pkt2, [self.dev_port10])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv6, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            # bind ACL group to port and verify ACLs work
            sai_thrift_set_port_attribute(
                self.client, self.port11, ingress_acl=acl_group_ingress)

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.1 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt)
            print('#### NOT Expecting ', self.dmac, ' | ', ROUTER_MAC, '| '
                  '20.0.0.1 | 192.168.0.1 | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv6, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 4000::1'
                  ' | 2000::1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pktv6)
            print('#### NOT Expecting ', self.dmac, ' | ', ROUTER_MAC, '| '
                  '4000::1 | 2000::1 | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv6, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            # unbind ACL group from port - ACLs sholdn't have any effect
            sai_thrift_set_port_attribute(
                self.client, self.port11, ingress_acl=int(SAI_NULL_OBJECT_ID))

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.1 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 20.0.0.1'
                  ' | 192.168.0.1 | @ ptf_intf 1')
            verify_packets(self, exp_pkt, [self.dev_port10])

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 4000::1'
                  ' | 2000::1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pktv6)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 4000::1'
                  ' | 2000::1 | @ ptf_intf 1')
            verify_packets(self, exp_pktv6, [self.dev_port10])

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.3 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt2)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 20.0.0.3'
                  ' | 192.168.0.1 | @ ptf_intf 1')
            verify_packets(self, exp_pkt2, [self.dev_port10])

            # cleanup ACL
            action_counter_ingress_ipv4 = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, ipv4_acl_ingress_entry_id,
                action_counter=action_counter_ingress_ipv4)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv4, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv4, packets=True)
            sai_thrift_remove_acl_counter(
                self.client, acl_counter_ingress_ipv4)

            action_counter_ingress_ipv6 = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, ipv6_acl_ingress_entry_id,
                action_counter=action_counter_ingress_ipv6)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv6, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_ipv6, packets=True)
            sai_thrift_remove_acl_counter(
                self.client, acl_counter_ingress_ipv6)

            action_counter_ingress_mirror = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, mirror_acl_ingress_entry_id,
                action_counter=action_counter_ingress_mirror)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress_mirror, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress_mirror, packets=True)
            sai_thrift_remove_acl_counter(
                self.client, acl_counter_ingress_mirror)

            sai_thrift_remove_acl_table_group_member(
                self.client, acl_group_ingress_ipv4_member_id)
            sai_thrift_remove_acl_table_group_member(
                self.client, acl_group_ingress_ipv6_member_id)
            sai_thrift_remove_acl_table_group(self.client, acl_group_ingress)
            sai_thrift_remove_acl_entry(self.client, ipv4_acl_ingress_entry_id)
            sai_thrift_remove_acl_entry(self.client, ipv6_acl_ingress_entry_id)
            sai_thrift_remove_acl_entry(self.client,
                                        mirror_acl_ingress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_ingress_ipv4_table_id)
            sai_thrift_remove_acl_table(self.client, acl_ingress_ipv6_table_id)

            print("Create ACL tables groups")
            acl_group_egress = sai_thrift_create_acl_table_group(
                self.client,
                acl_stage=group_stage_egress,
                acl_bind_point_type_list=group_bind_point_type_list,
                type=group_type)

            # create ACL tables
            print("Create ACL tables")
            acl_egress_ipv4_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage_egress,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_ip=True)

            acl_egress_ipv6_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage_egress,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_ipv6=True)

            # create ACL table group members
            print("Create ACL group members")
            acl_group_egress_ipv4_member_id = \
                sai_thrift_create_acl_table_group_member(
                    self.client,
                    acl_table_group_id=acl_group_egress,
                    acl_table_id=acl_egress_ipv4_table_id)

            acl_group_egress_ipv6_member_id = \
                sai_thrift_create_acl_table_group_member(
                    self.client,
                    acl_table_group_id=acl_group_egress,
                    acl_table_id=acl_egress_ipv6_table_id)

            # create ACL entries
            print("Create ACL entries")
            ipv4_acl_egress_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_egress_ipv4_table_id,
                priority=9999,
                field_src_ip=src_ip_t_ipv4,
                action_packet_action=packet_action)

            ipv6_acl_egress_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_egress_ipv6_table_id,
                priority=9998,
                field_src_ipv6=src_ip_t_ipv6,
                action_packet_action=packet_action)

            mirror_acl_egress_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_egress_ipv4_table_id,
                priority=9997,
                field_src_ip=src_ip_t_mirror,
                action_packet_action=packet_action)

            # create ACL counter
            acl_counter_egress_ipv4 = sai_thrift_create_acl_counter(
                self.client, table_id=acl_egress_ipv4_table_id)

            # attach ACL counter to ACL entry
            action_counter_egress_ipv4 = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress_ipv4),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, ipv4_acl_egress_entry_id,
                action_counter=action_counter_egress_ipv4)

            # create ACL counter
            acl_counter_egress_ipv6 = sai_thrift_create_acl_counter(
                self.client, table_id=acl_egress_ipv6_table_id)

            # attach ACL counter to ACL entry
            action_counter_egress_ipv6 = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress_ipv6),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, ipv6_acl_egress_entry_id,
                action_counter=action_counter_egress_ipv6)

            # create ACL counter
            acl_counter_egress_mirror = sai_thrift_create_acl_counter(
                self.client, table_id=acl_egress_ipv4_table_id)

            # attach ACL counter to ACL entry
            action_counter_egress_mirror = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress_mirror),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, mirror_acl_egress_entry_id,
                action_counter=action_counter_egress_mirror)

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.1 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 20.0.0.1'
                  ' | 192.168.0.1 | @ ptf_intf 1')
            verify_packets(self, exp_pkt, [self.dev_port10])

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 4000::1'
                  ' | 2000::1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pktv6)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 4000::1'
                  ' | 2000::1 | @ ptf_intf 1')
            verify_packets(self, exp_pktv6, [self.dev_port10])

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.3 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt2)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 20.0.0.3'
                  ' | 192.168.0.1 | @ ptf_intf 1')
            verify_packets(self, exp_pkt2, [self.dev_port10])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv6, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            # bind ACL group to port and verify ACLs work
            sai_thrift_set_port_attribute(
                self.client, self.port10, egress_acl=acl_group_egress)

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.1 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt)
            print('#### NOT Expecting ', self.dmac, ' | ', ROUTER_MAC, '| '
                  '20.0.0.1 | 192.168.0.1 | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv6, packets=True)
            self.assertEqual(packets['packets'], 0)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 4000::1'
                  ' | 2000::1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pktv6)
            print('#### NOT Expecting ', self.dmac, ' | ', ROUTER_MAC, '| '
                  '4000::1 | 2000::1 | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv6, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            # unbind ACL group from port - ACLs sholdn't have any effect
            sai_thrift_set_port_attribute(
                self.client, self.port10, egress_acl=int(SAI_NULL_OBJECT_ID))

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.1 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 20.0.0.1'
                  ' | 192.168.0.1 | @ ptf_intf 1')
            verify_packets(self, exp_pkt, [self.dev_port10])

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 4000::1'
                  ' | 2000::1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pktv6)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 4000::1'
                  ' | 2000::1 | @ ptf_intf 1')
            verify_packets(self, exp_pktv6, [self.dev_port10])

            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '20.0.0.3 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt2)
            print('#### Expecting ', self.dmac, ' | ', ROUTER_MAC, '| 20.0.0.3'
                  ' | 192.168.0.1 | @ ptf_intf 1')
            verify_packets(self, exp_pkt2, [self.dev_port10])

        finally:
            # cleanup ACL
            action_counter_egress_ipv4 = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, ipv4_acl_egress_entry_id,
                action_counter=action_counter_egress_ipv4)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=True)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress_ipv4)

            action_counter_egress_ipv6 = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, ipv6_acl_egress_entry_id,
                action_counter=action_counter_egress_ipv6)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress_ipv6, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv6, packets=True)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress_ipv6)

            action_counter_egress_mirror = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, mirror_acl_egress_entry_id,
                action_counter=action_counter_egress_mirror)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress_mirror, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_mirror, packets=True)
            sai_thrift_remove_acl_counter(
                self.client, acl_counter_egress_mirror)

            sai_thrift_remove_acl_table_group_member(
                self.client, acl_group_egress_ipv4_member_id)
            sai_thrift_remove_acl_table_group_member(
                self.client, acl_group_egress_ipv6_member_id)
            sai_thrift_remove_acl_table_group(self.client, acl_group_egress)
            sai_thrift_remove_acl_entry(self.client, ipv4_acl_egress_entry_id)
            sai_thrift_remove_acl_entry(self.client, ipv6_acl_egress_entry_id)
            sai_thrift_remove_acl_entry(self.client,
                                        mirror_acl_egress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_egress_ipv4_table_id)
            sai_thrift_remove_acl_table(self.client, acl_egress_ipv6_table_id)

    def tearDown(self):
        sai_thrift_remove_mirror_session(self.client, self.spanid)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_next_hop(self.client, self.nhop2)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry1)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry2)
        super(ACLGroupSeveralMembersTest, self).tearDown()


@group('mirror-acl')
class MultAclTableGroupBindTest(SaiHelper):
    """
    Verify matching on ACL table groups
    """
    def setUp(self):
        super(MultAclTableGroupBindTest, self).setUp()
        rif_id = self.port13_rif

        ip_addr_subnet = '172.16.10.0'
        self.ip_addr = '172.16.10.1'
        self.dmac = '00:11:22:33:44:55'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  self.ip_addr, rif_id, self.dmac))
        self.nbr_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif_id,
            ip_address=sai_ipaddress(self.ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry, dst_mac_address=self.dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (self.ip_addr, rif_id))
        self.nhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(self.ip_addr),
            router_interface_id=rif_id,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create route with %s ip prefix and %d router"
              " interface id" % (ip_addr_subnet, rif_id))
        self.route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                ip_addr_subnet))
        sai_thrift_create_route_entry(self.client, self.route_entry,
                                      next_hop_id=rif_id)

        # setup mirror ACL table
        mirror_type = SAI_MIRROR_SESSION_TYPE_LOCAL
        print("Create mirror session")
        self.span_session = sai_thrift_create_mirror_session(
            self.client,
            monitor_port=self.port10,
            type=mirror_type,
            vlan_header_valid=False)
        print(self.span_session)

    def runTest(self):
        print('--------------------------------------------------------------')
        print('Testing both IPV4, MIRROR ACL table within a ACL table group on'
              ' same set of ports')
        print("Sending packet ptf_intf 4 -> [ptf_intf 1, ptf_intf 2, ptf_intf "
              "3] (192.168.0.1 ---> 172.16.10.1 [id = 105])")

        mac_src = '00:22:22:22:22:22'
        ip_mask = '255.255.255.0'
        ipv4_addr = '192.168.0.1'

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                eth_src=mac_src,
                                ip_dst=self.ip_addr,
                                ip_src=ipv4_addr,
                                ip_id=105,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst=self.dmac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.ip_addr,
                                    ip_src=ipv4_addr,
                                    ip_id=105,
                                    ip_ttl=63)

        print('#### NO ACL Applied ####')
        print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 172.16.10.1'
              ' | 192.168.0.1 | @ ptf_intf 1')
        send_packet(self, self.dev_port10, pkt)
        print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| 172.16.10.1'
              ' | 192.168.0.1 | @ ptf_intf 4')
        verify_packet(self, exp_pkt, self.dev_port13)
        print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 172.16.10.1'
              ' | 192.168.0.1 | @ ptf_intf 2')
        send_packet(self, self.dev_port11, pkt)
        print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| 172.16.10.1'
              ' | 192.168.0.1 | @ ptf_intf 4')
        verify_packet(self, exp_pkt, self.dev_port13)
        print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | 172.16.10.1'
              ' | 192.168.0.1 | @ ptf_intf 3')
        send_packet(self, self.dev_port12, pkt)
        print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| 172.16.10.1'
              ' | 192.168.0.1 | @ ptf_intf 4')
        verify_packet(self, exp_pkt, self.dev_port13)

        # setup ACL table group
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL
        group_stage_ingress = SAI_ACL_STAGE_INGRESS
        group_stage_egress = SAI_ACL_STAGE_EGRESS

        # setup ACL table 1
        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        table_stage_egress = SAI_ACL_STAGE_EGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]

        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_point_list), int32list=group_bind_point_list)

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list), int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        print("Create ACL field data")
        src_ip_t_ipv4 = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ipv4_addr),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_mask))

        # create ACL tables
        print("Create ACL tables")
        acl_ingress_ip_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        print("Sending packet ptf_intf 2 -[ACL]-> ptf_intf 1 (20.20.20.1-[ACL]"
              "-> 172.16.10.1 [id = 105])")
        # setup ACL table to block on below matching param
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = "172.16.10.1"
        ip_dst_mask = "255.255.255.0"
        ip_proto = 6

        src_ip_t_mirror = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_src_mask))

        dst_ip_t_mirror = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_dst),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_dst_mask))

        # create ACL tables
        print("Create ACL tables")
        acl_ingress_mirror_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            field_ip_protocol=ip_proto)

        # setup ACL table group members
        group_member_priority = 1

        acl_group_ingress_list = []
        acl_group_member_ingress_list = []
        in_ports = [self.port10, self.port11, self.port12]

        for port in in_ports:
            # ACL table group
            print("Create ACL tables groups for", port, " port")
            acl_table_group_ingress = sai_thrift_create_acl_table_group(
                self.client,
                acl_stage=group_stage_ingress,
                acl_bind_point_type_list=group_bind_point_type_list,
                type=group_type)

            # create ACL table group member 1 - v4 tables
            acl_group_ingress_ip_member_id = \
                sai_thrift_create_acl_table_group_member(
                    self.client,
                    acl_table_group_id=acl_table_group_ingress,
                    acl_table_id=acl_ingress_ip_table_id,
                    priority=group_member_priority)

            # create ACL table group members 2 - mirror tables
            print("Create ACL group members")
            acl_group_ingress_mirror_member_id = \
                sai_thrift_create_acl_table_group_member(
                    self.client,
                    acl_table_group_id=acl_table_group_ingress,
                    acl_table_id=acl_ingress_mirror_table_id,
                    priority=group_member_priority)

            acl_group_ingress_list.append(acl_table_group_ingress)
            acl_group_member_ingress_list.append(
                acl_group_ingress_ip_member_id)
            acl_group_member_ingress_list.append(
                acl_group_ingress_mirror_member_id)

        for i, ports in enumerate(in_ports):
            # attach this ACL table group to port10, port11, port12
            print("Bind ACL ingress group 0x % lx to port 0x % lx" % (
                acl_group_ingress_list[i], ports))
            sai_thrift_set_port_attribute(
                self.client, ports,
                ingress_acl=acl_group_ingress_list[i])

        action = SAI_PACKET_ACTION_DROP
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        # create ACL entries
        print("Create ACL entries")
        acl_ingress_ip_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ip_table_id,
            priority=1,
            field_src_ip=src_ip_t_ipv4,
            action_packet_action=packet_action)

        # create ACL counter
        acl_counter_ingress = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ip_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_ip_entry_id,
            action_counter=action_counter_ingress)

        src_l4_port = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=4000),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        dst_l4_port = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=5000),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        mirror_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                objlist=sai_thrift_object_list_t(
                    count=len([self.span_session]),
                    idlist=[self.span_session])))

        # create ACL entries
        print("Create ACL entries")
        mirror_acl_ingress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_mirror_table_id,
            priority=1,
            field_src_ip=src_ip_t_mirror,
            field_dst_ip=dst_ip_t_mirror,
            field_l4_src_port=src_l4_port,
            field_l4_dst_port=dst_l4_port,
            action_mirror_ingress=mirror_action)

        # create ACL counter
        acl_counter_mirror = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_mirror_table_id)

        # attach ACL counter to ACL entry
        action_counter_mirror = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_mirror),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, mirror_acl_ingress_entry_id,
            action_counter=action_counter_mirror)

        try:
            print('#### ACL \'DROP, src mac 00:22:22:22:22:22, '
                  'in_ports[ptf_intf_1,2,3,4]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 1')
            time.sleep(5)
            send_packet(self, self.dev_port10, pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 4')
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 4')
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 2)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 3')
            send_packet(self, self.dev_port12, pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 4')
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 3)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)

            print("Verify Mirror ACL")
            time.sleep(5)
            pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                    eth_src=mac_src,
                                    ip_src=ipv4_addr,
                                    ip_dst=self.ip_addr,
                                    ip_id=105,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)

            print("TX packet port 12 -> port 13, ipv4 ACL blocks route pkt but"
                  " mirror ACL mirrors pkt to port 10")
            send_packet(self, self.dev_port12, pkt)
            verify_packets(self, pkt, ports=[self.dev_port10])
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 4)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_mirror, packets=True)
            self.assertEqual(packets['packets'], 1)

            # cleanup ACL, remove ACL group member
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_ingress_ip_entry_id,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)

            action_counter_mirror = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, mirror_acl_ingress_entry_id,
                action_counter=action_counter_mirror)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_mirror, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_mirror, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_mirror)

            for mbr in acl_group_member_ingress_list:
                sai_thrift_remove_acl_table_group_member(self.client, mbr)

            # unlink this ACL table from port10, port12, port13 object

            for i, ports in enumerate(in_ports):
                sai_thrift_set_port_attribute(
                    self.client, ports,
                    ingress_acl=int(SAI_NULL_OBJECT_ID))

            # cleanup ACL group, entries, tables
            for grp in acl_group_ingress_list:
                sai_thrift_remove_acl_table_group(self.client, grp)

            sai_thrift_remove_acl_entry(
                self.client, acl_ingress_ip_entry_id)
            sai_thrift_remove_acl_table(
                self.client, acl_ingress_ip_table_id)
            sai_thrift_remove_acl_entry(
                self.client, mirror_acl_ingress_entry_id)
            sai_thrift_remove_acl_table(
                self.client, acl_ingress_mirror_table_id)

            # create ACL tables
            print("Create ACL tables")
            acl_egress_ip_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage_egress,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_ip=True)

            # ACL table group
            print("Create ACL egress table groups")
            acl_table_group_egress = sai_thrift_create_acl_table_group(
                self.client,
                acl_stage=group_stage_egress,
                acl_bind_point_type_list=group_bind_point_type_list,
                type=group_type)

            # create ACL table group member 1 - v4 tables
            acl_group_egress_ip_member_id = \
                sai_thrift_create_acl_table_group_member(
                    self.client,
                    acl_table_group_id=acl_table_group_egress,
                    acl_table_id=acl_egress_ip_table_id,
                    priority=group_member_priority)

            # attach this ACL table group to port4
            print("Bind ACL egress group to port4")
            sai_thrift_set_port_attribute(
                self.client, self.port13, egress_acl=acl_table_group_egress)

            # create ACL entries
            print("Create ACL entries")
            acl_egress_ip_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_egress_ip_table_id,
                priority=1,
                field_src_ip=src_ip_t_ipv4,
                action_packet_action=packet_action)

            # create ACL counter
            acl_counter_egress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_egress_ip_table_id)

            # attach ACL counter to ACL entry
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_ingress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_ip_entry_id,
                action_counter=action_counter_egress)

            # send the test packet(s)
            pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                    eth_src='00:22:22:22:22:22',
                                    ip_dst='172.16.10.1',
                                    ip_src='192.168.0.1',
                                    ip_id=105,
                                    ip_ttl=64)

            print('#### ACL \'DROP, src mac 00:22:22:22:22:22, '
                  'in_ports[ptf_intf_1,2,3,4]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 1')
            time.sleep(5)
            send_packet(self, self.dev_port10, pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 4')
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 1)

            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 4')
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 2)

            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 3')
            send_packet(self, self.dev_port12, pkt)
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.0.1 | @ ptf_intf 4')
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 3)
            time.sleep(5)

        finally:
            # cleanup ACL, remove ACL group member
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_egress_ip_entry_id,
                action_counter=action_counter_egress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress)

            sai_thrift_remove_acl_table_group_member(
                self.client, acl_group_egress_ip_member_id)

            # unlink this ACL table from port4 object
            sai_thrift_set_port_attribute(self.client, self.port13,
                                          egress_acl=int(SAI_NULL_OBJECT_ID))

            # cleanup ACL group, entries, tables
            sai_thrift_remove_acl_table_group(self.client,
                                              acl_table_group_egress)
            sai_thrift_remove_acl_entry(
                self.client, acl_egress_ip_entry_id)
            sai_thrift_remove_acl_table(
                self.client, acl_egress_ip_table_id)

    def tearDown(self):
        # cleanup mirror session
        sai_thrift_remove_mirror_session(self.client, self.span_session)
        # l3 part
        sai_thrift_remove_route_entry(self.client, self.route_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry)
        super(MultAclTableGroupBindTest, self).tearDown()


@disabled
# It is not possible to fully transfer this test form the old framework due to
# the fact that we don't support two IPV4 permit/deny ACLs to the same port.
# The test in its current form causes instability in other ACL tests.
# SWI-4847
class SonicACLTest(SaiHelper):
    """
    Create an ACL group per port
    Create an ACL group member per table and per group
    Add entries in each of the tables and test ACLs work as expected
    """

    def setUp(self):
        super(SonicACLTest, self).setUp()

        rif_id1 = self.port10_rif

        ip_addr_subnet = '192.168.0.0'
        ip_addr = '192.168.0.1'
        self.dmac = '00:22:22:22:22:22'

        print("Create neighbor with %s ip address, %d router interface"
              " id and %s destination mac" % (
                  ip_addr, rif_id1, self.dmac))
        self.nbr_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif_id1,
            ip_address=sai_ipaddress(ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr_entry, dst_mac_address=self.dmac)

        print("Create nhop with %s ip address and %d router"
              " interface id" % (ip_addr, rif_id1))
        self.nhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(ip_addr),
            router_interface_id=rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        print("Create route with %s ip prefix and %d router"
              " interface id" % (ip_addr_subnet, rif_id1))
        self.route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                ip_addr_subnet))
        sai_thrift_create_route_entry(self.client, self.route_entry,
                                      next_hop_id=rif_id1)

    def runTest(self):
        # setup ACL table group
        acl_ingress_groups_list = []
        acl_ingress_group_members_list = []
        counters = []
        action_counters = []
        acl_entries = []
        acl_rules = []
        ip_mask = '255.255.255.0'
        ipv4_addr = '192.168.0.1'

        group_stage_ingress = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_point_list), int32list=group_bind_point_list)

        ports_list = [self.port0, self.port1, self.port2, self.port3,
                      self.port4, self.port5, self.port6, self.port7,
                      self.port8, self.port9, self.port10, self.port11,
                      self.port12, self.port13, self.port14, self.port15,
                      self.port16, self.port17, self.port18, self.port19,
                      self.port20, self.port21, self.port22, self.port23,
                      self.port24, self.port25, self.port26, self.port27,
                      self.port28, self.port29, self.port30, self.port31]

        for index, port in enumerate(ports_list):
            # create ACL table group
            acl_table_group_ingress = sai_thrift_create_acl_table_group(
                self.client,
                acl_stage=group_stage_ingress,
                acl_bind_point_type_list=group_bind_point_type_list,
                type=group_type)

            print(index, port)
            acl_ingress_groups_list.append(acl_table_group_ingress)
            # bind this ACL group to ports object id
            print("Bind ACL ingress group 0x % lx to port 0x % lx" % (
                acl_ingress_groups_list[index], port))
            sai_thrift_set_port_attribute(
                self.client, port,
                ingress_acl=acl_ingress_groups_list[index])

        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list), int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        print("Create ACL field data")
        src_ip_t_ipv4 = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ipv4_addr),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_mask))

        # create ACL tables
        print("Create ACL tables")
        acl_ingress_ipv4_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        # create ACL ingress table group members
        for acl_group in acl_ingress_groups_list:
            # create ACL table group member 1 - v4 tables
            acl_group_ingress_ip_member_id = \
                sai_thrift_create_acl_table_group_member(
                    self.client,
                    acl_table_group_id=acl_group,
                    acl_table_id=acl_ingress_ipv4_table_id,
                    priority=1)

            acl_ingress_group_members_list.append(
                acl_group_ingress_ip_member_id)

        # create ACL entries in secondary ipv4 ACL

        # "ACL_RULE|DATAACL|DEFAULT_RULE": {
        #     "type": "hash",
        #     "value": {
        #         "ETHER_TYPE": "2048",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "1"
        #     }
        # },
        ether_type = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=2048),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        acl_ingress_entry_id_default = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=1,
            field_ether_type=ether_type,
            action_packet_action=packet_action)

        # "ACL_RULE|DATAACL|RULE_1": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9999",
        #         "SRC_IP": "20.0.0.2/32"
        #     }
        # },
        print("Create ACL field data")
        src_ip_t_ipv4 = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="20.0.0.2"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_FORWARD))\

        acl_ingress_entry_id1 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9999,
            field_src_ip=src_ip_t_ipv4,
            action_packet_action=packet_action)

        # remove the entries just created in a secondary table
        # to simulate behaviour of SONiC CT
        sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id1)
        sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id_default)

        # create acl entries in ipv4 acl
        # catch all drop rule
        # "ACL_RULE|DATAINGRESS|DEFAULT_RULE": {
        #     "type": "hash",
        #     "value": {
        #         "ETHER_TYPE": "2048",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "1"
        #     }
        # },
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_ingress_entry_id_default = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=1,
            field_ether_type=ether_type,
            action_packet_action=packet_action)

        # create ACL counter
        acl_counter_default = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_default = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_default),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id_default,
            action_counter=action_counter_default)

        counters.append(acl_counter_default)
        action_counters.append(action_counter_default)
        acl_entries.append(acl_ingress_entry_id_default)
        acl_rules.append("DEFAULT_RULE")

        # "ACL_RULE|DATAINGRESS|RULE_1": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9999",
        #         "SRC_IP": "20.0.0.2/32"
        #     }
        # },
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_FORWARD))

        acl_ingress_entry_id1 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9999,
            field_src_ip=src_ip_t_ipv4,
            action_packet_action=packet_action)

        # create ACL counter
        acl_counter_id1 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id1 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id1),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id1,
            action_counter=action_counter_id1)

        counters.append(acl_counter_id1)
        action_counters.append(action_counter_id1)
        acl_entries.append(acl_ingress_entry_id1)
        acl_rules.append("RULE_1")

        # "ACL_RULE|DATAINGRESS|RULE_10": {
        #     "type": "hash",
        #     "value": {
        #         "L4_SRC_PORT_RANGE": "4656-4671",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9990"
        #     }
        # },
        u32range = sai_thrift_u32_range_t(min=4656, max=4671)
        acl_range_id1 = sai_thrift_create_acl_range(
            self.client,
            type=SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE,
            limit=u32range)
        range_list = [acl_range_id1]
        print("ACL range created 0x%lx" % (acl_range_id1))

        range_list_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(
                objlist=sai_thrift_object_list_t(
                    count=len(range_list),
                    idlist=range_list)))

        acl_ingress_entry_id10 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9990,
            action_packet_action=packet_action,
            field_acl_range_type=range_list_t)

        # create ACL counter
        acl_counter_id10 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id10 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id10),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id10,
            action_counter=action_counter_id10)

        counters.append(acl_counter_id10)
        action_counters.append(action_counter_id10)
        acl_entries.append(acl_ingress_entry_id10)
        acl_rules.append("RULE_10")

        # "ACL_RULE|DATAINGRESS|RULE_11": {
        #     "type": "hash",
        #     "value": {
        #         "L4_DST_PORT_RANGE": "4640-4687",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9989"
        #     }
        # },
        u32range = sai_thrift_u32_range_t(min=4640, max=4687)
        acl_range_id2 = sai_thrift_create_acl_range(
            self.client,
            type=SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE,
            limit=u32range)
        range_list = [acl_range_id2]
        print("ACL range created 0x%lx" % (acl_range_id2))

        range_list_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(
                objlist=sai_thrift_object_list_t(
                    count=len(range_list),
                    idlist=range_list)))

        acl_ingress_entry_id11 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9989,
            action_packet_action=packet_action,
            field_acl_range_type=range_list_t)

        # create ACL counter
        acl_counter_id11 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id11 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id11),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id11,
            action_counter=action_counter_id11)

        counters.append(acl_counter_id11)
        action_counters.append(action_counter_id11)
        acl_entries.append(acl_ingress_entry_id11)
        acl_rules.append("RULE_11")

        # "ACL_RULE|DATAINGRESS|RULE_12": {
        #     "type": "hash",
        #     "value": {
        #         "IP_PROTOCOL": "1",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9988",
        #         "SRC_IP": "20.0.0.4/32"
        #     }
        # },
        src_ip_t_ipv4 = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="20.0.0.4"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        ip_protocol = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=1),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        acl_ingress_entry_id12 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9988,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t_ipv4,
            field_ip_protocol=ip_protocol)

        # create ACL counter
        acl_counter_id12 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id12 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id12),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id12,
            action_counter=action_counter_id12)

        counters.append(acl_counter_id12)
        action_counters.append(action_counter_id12)
        acl_entries.append(acl_ingress_entry_id12)
        acl_rules.append("RULE_12")

        # "ACL_RULE|DATAINGRESS|RULE_13": {
        #     "type": "hash",
        #     "value": {
        #         "IP_PROTOCOL": "17",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9987",
        #         "SRC_IP": "20.0.0.4/32"
        #     }
        # }
        ip_protocol = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=17),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        acl_ingress_entry_id13 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9987,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t_ipv4,
            field_ip_protocol=ip_protocol)

        # create ACL counter
        acl_counter_id13 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id13 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id13),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id13,
            action_counter=action_counter_id13)

        counters.append(acl_counter_id13)
        action_counters.append(action_counter_id13)
        acl_entries.append(acl_ingress_entry_id13)
        acl_rules.append("RULE_13")

        # "ACL_RULE|DATAINGRESS|RULE_14": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9986",
        #         "SRC_IP": "20.0.0.6/32"
        #     }
        # },
        src_ip_t_ipv4 = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="20.0.0.6"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_ingress_entry_id14 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9986,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t_ipv4)

        # create ACL counter
        acl_counter_id14 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id14 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id14),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id14,
            action_counter=action_counter_id14)

        counters.append(acl_counter_id14)
        action_counters.append(action_counter_id14)
        acl_entries.append(acl_ingress_entry_id14)
        acl_rules.append("RULE_14")

        # "ACL_RULE|DATAINGRESS|RULE_15": {
        #     "type": "hash",
        #     "value": {
        #         "DST_IP": "192.168.0.17/32",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9985"
        #     }
        # },
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="192.168.0.17"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        acl_ingress_entry_id15 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9985,
            action_packet_action=packet_action,
            field_dst_ip=dst_ip_t)

        # create ACL counter
        acl_counter_id15 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id15 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id15),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id15,
            action_counter=action_counter_id15)

        counters.append(acl_counter_id15)
        action_counters.append(action_counter_id15)
        acl_entries.append(acl_ingress_entry_id15)
        acl_rules.append("RULE_15")

        # "ACL_RULE|DATAINGRESS|RULE_16": {
        #     "type": "hash",
        #     "value": {
        #         "DST_IP": "172.16.3.0/32",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9984"
        #     }
        # },
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="172.16.3.0"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        acl_ingress_entry_id16 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9984,
            action_packet_action=packet_action,
            field_dst_ip=dst_ip_t)

        # create ACL counter
        acl_counter_id16 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id16 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id16),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id16,
            action_counter=action_counter_id16)

        counters.append(acl_counter_id16)
        action_counters.append(action_counter_id16)
        acl_entries.append(acl_ingress_entry_id16)
        acl_rules.append("RULE_16")

        # "ACL_RULE|DATAINGRESS|RULE_17": {
        #     "type": "hash",
        #     "value": {
        #         "L4_SRC_PORT": "4721",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9983"
        #     }
        # },
        src_l4_port = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=4721),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        acl_ingress_entry_id17 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9983,
            action_packet_action=packet_action,
            field_l4_src_port=src_l4_port)

        # create ACL counter
        acl_counter_id17 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id17 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id17),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id17,
            action_counter=action_counter_id17)

        counters.append(acl_counter_id17)
        action_counters.append(action_counter_id17)
        acl_entries.append(acl_ingress_entry_id17)
        acl_rules.append("RULE_17")

        # "ACL_RULE|DATAINGRESS|RULE_18": {
        #     "type": "hash",
        #     "value": {
        #         "IP_PROTOCOL": "127",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9982"
        #     }
        # },
        ip_protocol = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=127),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        acl_ingress_entry_id18 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9982,
            action_packet_action=packet_action,
            field_ip_protocol=ip_protocol)

        # create ACL counter
        acl_counter_id18 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id18 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id18),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id18,
            action_counter=action_counter_id18)

        counters.append(acl_counter_id18)
        action_counters.append(action_counter_id18)
        acl_entries.append(acl_ingress_entry_id18)
        acl_rules.append("RULE_18")

        # "ACL_RULE|DATAINGRESS|RULE_19": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9981",
        #         "TCP_FLAGS": "0x24/0x24"
        #     }
        # },
        tcp_flags = sai_thrift_acl_field_data_t(
            enable=True,
            mask=sai_thrift_acl_field_data_mask_t(u8=127),
            data=sai_thrift_acl_field_data_data_t(u8=36))

        acl_ingress_entry_id19 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9981,
            action_packet_action=packet_action,
            field_tcp_flags=tcp_flags)

        # create ACL counter
        acl_counter_id19 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id19 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id19),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id19,
            action_counter=action_counter_id19)

        counters.append(acl_counter_id19)
        action_counters.append(action_counter_id19)
        acl_entries.append(acl_ingress_entry_id19)
        acl_rules.append("RULE_19")

        # "ACL_RULE|DATAINGRESS|RULE_2": {
        #     "type": "hash",
        #     "value": {
        #         "DST_IP": "192.168.0.16/32",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9998"
        #     }
        # },
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_FORWARD))

        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="192.168.0.16"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        acl_ingress_entry_id2 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9998,
            action_packet_action=packet_action,
            field_dst_ip=dst_ip_t)

        # create ACL counter
        acl_counter_id2 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id2 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id2),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id2,
            action_counter=action_counter_id2)

        counters.append(acl_counter_id2)
        action_counters.append(action_counter_id2)
        acl_entries.append(acl_ingress_entry_id2)
        acl_rules.append("RULE_2")

        # "ACL_RULE|DATAINGRESS|RULE_20": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9980",
        #         "SRC_IP": "20.0.0.7/32"
        #     }
        # },
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="20.0.0.7"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        acl_ingress_entry_id20 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9980,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t)

        # create ACL counter
        acl_counter_id20 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id20 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id20),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id20,
            action_counter=action_counter_id20)

        counters.append(acl_counter_id20)
        action_counters.append(action_counter_id20)
        acl_entries.append(acl_ingress_entry_id20)
        acl_rules.append("RULE_20")

        # "ACL_RULE|DATAINGRESS|RULE_21": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9979",
        #         "SRC_IP": "20.0.0.7/32"
        #     }
        # },
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_ingress_entry_id21 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9979,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t)

        # create ACL counter
        acl_counter_id21 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id21 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id21),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id21,
            action_counter=action_counter_id21)

        counters.append(acl_counter_id21)
        action_counters.append(action_counter_id21)
        acl_entries.append(acl_ingress_entry_id21)
        acl_rules.append("RULE_21")

        # "ACL_RULE|DATAINGRESS|RULE_22": {
        #     "type": "hash",
        #     "value": {
        #         "L4_DST_PORT": "4731",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9978"
        #     }
        # },
        dst_l4_port = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=4731),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        acl_ingress_entry_id22 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9978,
            action_packet_action=packet_action,
            field_l4_dst_port=dst_l4_port)

        # create ACL counter
        acl_counter_id22 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id22 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id22),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id22,
            action_counter=action_counter_id22)

        counters.append(acl_counter_id22)
        action_counters.append(action_counter_id22)
        acl_entries.append(acl_ingress_entry_id22)
        acl_rules.append("RULE_22")

        # "ACL_RULE|DATAINGRESS|RULE_23": {
        #     "type": "hash",
        #     "value": {
        #         "L4_SRC_PORT_RANGE": "4756-4771",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9977"
        #     }
        # },
        u32range = sai_thrift_u32_range_t(min=4756, max=4771)
        acl_range_id3 = sai_thrift_create_acl_range(
            self.client,
            type=SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE,
            limit=u32range)
        range_list = [acl_range_id3]
        print("ACL range created 0x%lx" % (acl_range_id3))

        range_list_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(
                objlist=sai_thrift_object_list_t(
                    count=len(range_list),
                    idlist=range_list)))

        acl_ingress_entry_id23 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9977,
            action_packet_action=packet_action,
            field_acl_range_type=range_list_t)

        # create ACL counter
        acl_counter_id23 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id23 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id23),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id23,
            action_counter=action_counter_id23)

        counters.append(acl_counter_id23)
        action_counters.append(action_counter_id23)
        acl_entries.append(acl_ingress_entry_id23)
        acl_rules.append("RULE_23")

        # "ACL_RULE|DATAINGRESS|RULE_24": {
        #     "type": "hash",
        #     "value": {
        #         "L4_DST_PORT_RANGE": "4740-4787",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9976"
        #     }
        # },
        u32range = sai_thrift_u32_range_t(min=4740, max=4787)
        acl_range_id4 = sai_thrift_create_acl_range(
            self.client,
            type=SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE,
            limit=u32range)
        range_list = [acl_range_id4]
        print("ACL range created 0x%lx" % (acl_range_id4))

        range_list_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(
                objlist=sai_thrift_object_list_t(
                    count=len(range_list),
                    idlist=range_list)))

        acl_ingress_entry_id24 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9976,
            action_packet_action=packet_action,
            field_acl_range_type=range_list_t)

        # create ACL counter
        acl_counter_id24 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id24 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id24),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id24,
            action_counter=action_counter_id24)

        counters.append(acl_counter_id24)
        action_counters.append(action_counter_id24)
        acl_entries.append(acl_ingress_entry_id24)
        acl_rules.append("RULE_24")

        # "ACL_RULE|DATAINGRESS|RULE_25": {
        #     "type": "hash",
        #     "value": {
        #         "IP_PROTOCOL": "1",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9975",
        #         "SRC_IP": "20.0.0.8/32"
        #     }
        # },
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="20.0.0.8"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        ip_protocol = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=1),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        acl_ingress_entry_id25 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9975,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t,
            field_ip_protocol=ip_protocol)

        # create ACL counter
        acl_counter_id25 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id25 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id25),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id25,
            action_counter=action_counter_id25)

        counters.append(acl_counter_id25)
        action_counters.append(action_counter_id25)
        acl_entries.append(acl_ingress_entry_id25)
        acl_rules.append("RULE_25")

        # "ACL_RULE|DATAINGRESS|RULE_26": {
        #     "type": "hash",
        #     "value": {
        #         "IP_PROTOCOL": "17",
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9974",
        #         "SRC_IP": "20.0.0.8/32"
        #     }
        # },
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="20.0.0.8"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        ip_protocol = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=17),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        acl_ingress_entry_id26 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9974,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t,
            field_ip_protocol=ip_protocol)

        # create ACL counter
        acl_counter_id26 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id26 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id26),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id26,
            action_counter=action_counter_id26)

        counters.append(acl_counter_id26)
        action_counters.append(action_counter_id26)
        acl_entries.append(acl_ingress_entry_id26)
        acl_rules.append("RULE_26")

        # "ACL_RULE|DATAINGRESS|RULE_27": {
        #     "type": "hash",
        #     "value": {
        #         "L4_SRC_PORT": "179",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9973"
        #     }
        # },
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_FORWARD))

        src_l4_port = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=179),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        acl_ingress_entry_id27 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9973,
            action_packet_action=packet_action,
            field_l4_src_port=src_l4_port)

        # create ACL counter
        acl_counter_id27 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id27 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id27),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id27,
            action_counter=action_counter_id27)

        counters.append(acl_counter_id27)
        action_counters.append(action_counter_id27)
        acl_entries.append(acl_ingress_entry_id27)
        acl_rules.append("RULE_27")

        # "ACL_RULE|DATAINGRESS|RULE_28": {
        #     "type": "hash",
        #     "value": {
        #         "L4_DST_PORT": "179",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9972"
        #     }
        # },
        dst_l4_port = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=179),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        acl_ingress_entry_id28 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9972,
            action_packet_action=packet_action,
            field_l4_dst_port=dst_l4_port)

        # create ACL counter
        acl_counter_id28 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id28 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id28),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id28,
            action_counter=action_counter_id28)

        counters.append(acl_counter_id28)
        action_counters.append(action_counter_id28)
        acl_entries.append(acl_ingress_entry_id28)
        acl_rules.append("RULE_28")

        # "ACL_RULE|DATAINGRESS|RULE_3": {
        #     "type": "hash",
        #     "value": {
        #         "DST_IP": "172.16.2.0/32",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9997"
        #     }
        # },
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="172.16.2.0"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        acl_ingress_entry_id3 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9997,
            action_packet_action=packet_action,
            field_dst_ip=dst_ip_t)

        # create ACL counter
        acl_counter_id3 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id3 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id3),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id3,
            action_counter=action_counter_id3)

        counters.append(acl_counter_id3)
        action_counters.append(action_counter_id3)
        acl_entries.append(acl_ingress_entry_id3)
        acl_rules.append("RULE_3")

        # "ACL_RULE|DATAINGRESS|RULE_4": {
        #     "type": "hash",
        #     "value": {
        #         "L4_SRC_PORT": "4621",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9996"
        #     }
        # },
        src_l4_port = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=4621),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        acl_ingress_entry_id4 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9996,
            action_packet_action=packet_action,
            field_l4_src_port=src_l4_port)

        # create ACL counter
        acl_counter_id4 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id4 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id4),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id4,
            action_counter=action_counter_id4)

        counters.append(acl_counter_id4)
        action_counters.append(action_counter_id4)
        acl_entries.append(acl_ingress_entry_id4)
        acl_rules.append("RULE_4")

        # "ACL_RULE|DATAINGRESS|RULE_5": {
        #     "type": "hash",
        #     "value": {
        #         "IP_PROTOCOL": "126",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9995"
        #     }
        # },
        ip_protocol = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=126),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        acl_ingress_entry_id5 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9995,
            action_packet_action=packet_action,
            field_ip_protocol=ip_protocol)

        # create ACL counter
        acl_counter_id5 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id5 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id5),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id5,
            action_counter=action_counter_id5)

        counters.append(acl_counter_id5)
        action_counters.append(action_counter_id5)
        acl_entries.append(acl_ingress_entry_id5)
        acl_rules.append("RULE_5")

        # "ACL_RULE|DATAINGRESS|RULE_6": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9994",
        #         "TCP_FLAGS": "0x1b/0x1b"
        #     }
        # },
        tcp_flags = sai_thrift_acl_field_data_t(
            enable=True,
            mask=sai_thrift_acl_field_data_mask_t(u8=127),
            data=sai_thrift_acl_field_data_data_t(u8=27))

        acl_ingress_entry_id6 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9994,
            action_packet_action=packet_action,
            field_tcp_flags=tcp_flags)

        # create ACL counter
        acl_counter_id6 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id6 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id6),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id6,
            action_counter=action_counter_id6)

        counters.append(acl_counter_id6)
        action_counters.append(action_counter_id6)
        acl_entries.append(acl_ingress_entry_id6)
        acl_rules.append("RULE_6")

        # "ACL_RULE|DATAINGRESS|RULE_7": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "DROP",
        #         "PRIORITY": "9993",
        #         "SRC_IP": "20.0.0.3/32"
        #     }
        # },
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4="20.0.0.3"),
            mask=sai_thrift_acl_field_data_mask_t(ip4="255.255.255.255"))

        acl_ingress_entry_id7 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9993,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t)

        # create ACL counter
        acl_counter_id7 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id7 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id7),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id7,
            action_counter=action_counter_id7)

        counters.append(acl_counter_id7)
        action_counters.append(action_counter_id7)
        acl_entries.append(acl_ingress_entry_id7)
        acl_rules.append("RULE_7")

        # "ACL_RULE|DATAINGRESS|RULE_8": {
        #     "type": "hash",
        #     "value": {
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9992",
        #         "SRC_IP": "20.0.0.3/32"
        #     }
        # },
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_FORWARD))

        acl_ingress_entry_id8 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9992,
            action_packet_action=packet_action,
            field_src_ip=src_ip_t)

        # create ACL counter
        acl_counter_id8 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id8 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id8),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id8,
            action_counter=action_counter_id8)

        counters.append(acl_counter_id8)
        action_counters.append(action_counter_id8)
        acl_entries.append(acl_ingress_entry_id8)
        acl_rules.append("RULE_8")

        # "ACL_RULE|DATAINGRESS|RULE_9": {
        #     "type": "hash",
        #     "value": {
        #         "L4_DST_PORT": "4631",
        #         "PACKET_ACTION": "FORWARD",
        #         "PRIORITY": "9991"
        #     }
        # },
        dst_l4_port = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=4631),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        acl_ingress_entry_id9 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_ingress_ipv4_table_id,
            priority=9991,
            action_packet_action=packet_action,
            field_l4_dst_port=dst_l4_port)

        # create ACL counter
        acl_counter_id9 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_ingress_ipv4_table_id)

        # attach ACL counter to ACL entry
        action_counter_id9 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_id9),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ingress_entry_id9,
            action_counter=action_counter_id9)

        counters.append(acl_counter_id9)
        action_counters.append(action_counter_id9)
        acl_entries.append(acl_ingress_entry_id9)
        acl_rules.append("RULE_9")

        try:
            pkt = simple_tcp_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.dmac,
                ip_src="20.0.0.1",
                ip_dst=ipv4_addr,
                tcp_sport=0x4321,
                tcp_dport=0x51,
                ip_ttl=64)
            send_packet(self, self.dev_port11, pkt)
            verify_no_other_packets(self, timeout=2)

            print("Entries counters after sending the first packet:")
            for i, acl_counter in enumerate(counters):
                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                print(acl_rules[i], " : ", packets['packets'])

            pkt = simple_tcp_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.dmac,
                ip_src="20.0.0.2",
                ip_dst=ipv4_addr,
                tcp_sport=0x4321,
                tcp_dport=0x51,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.dmac,
                eth_src=ROUTER_MAC,
                ip_src="20.0.0.2",
                ip_dst=ipv4_addr,
                tcp_sport=0x4321,
                tcp_dport=0x51,
                ip_ttl=63)
            send_packet(self, self.dev_port11, pkt)
            verify_packets(self, exp_pkt, [self.dev_port10])

            print("Entries counters after sending the second packet:")
            for i, acl_counter in enumerate(counters):
                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                print(acl_rules[i], " : ", packets['packets'])

        finally:
            print("Unbind ACL groups from ports object id:")
            for i, ports in enumerate(ports_list):
                print(i, ports)
                sai_thrift_set_port_attribute(
                    self.client, ports,
                    ingress_acl=int(SAI_NULL_OBJECT_ID))

            for acl_group_member in acl_ingress_group_members_list:
                sai_thrift_remove_acl_table_group_member(self.client,
                                                         acl_group_member)

            for acl_group in acl_ingress_groups_list:
                sai_thrift_remove_acl_table_group(self.client, acl_group)

            for i, acl_action_counter in enumerate(action_counters):
                acl_action_counter = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
                sai_thrift_set_acl_entry_attribute(
                    self.client, acl_entries[i],
                    action_counter=acl_action_counter)

            for acl_counter in counters:
                sai_thrift_set_acl_counter_attribute(
                    self.client, acl_counter, packets=None)
                packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
                self.assertEqual(packets['packets'], 0)
                sai_thrift_remove_acl_counter(self.client, acl_counter)

            # cleanup ACL
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id28)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id27)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id26)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id25)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id24)
            sai_thrift_remove_acl_range(self.client, acl_range_id4)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id23)
            sai_thrift_remove_acl_range(self.client, acl_range_id3)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id22)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id21)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id20)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id19)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id18)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id17)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id16)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id15)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id14)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id13)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id12)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id11)
            sai_thrift_remove_acl_range(self.client, acl_range_id2)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id10)
            sai_thrift_remove_acl_range(self.client, acl_range_id1)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id9)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id8)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id7)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id6)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id5)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id4)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id3)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id2)
            sai_thrift_remove_acl_entry(self.client, acl_ingress_entry_id1)
            sai_thrift_remove_acl_entry(self.client,
                                        acl_ingress_entry_id_default)

            sai_thrift_remove_acl_table(self.client,
                                        acl_ingress_ipv4_table_id)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr_entry)
        super(SonicACLTest, self).tearDown()


class AclTableTypeTest(SaiHelper):
    '''
    ACL Type class. This test creates tables with various match fields
    '''
    acl_range_type = sai_thrift_s32_list_t(count=0, int32list=[])

    def setUp(self):
        super(AclTableTypeTest, self).setUp()

        self.dmac = '00:11:22:33:44:55'
        self.src_mac = '00:22:22:22:22:22'
        self.ip_addr1 = '10.0.0.1'
        self.ip_addr2 = '10.10.10.2'

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
            self.client, self.neighbor_entry1, dst_mac_address=self.dmac)

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

        self.vlan_oid = sai_thrift_create_vlan(self.client, 100)
        mac_action = SAI_PACKET_ACTION_FORWARD

        self.port26_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port26,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port26_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.port27_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port27,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port27_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.fdb_entry1 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.src_mac,
            bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry1,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port26_bp,
            packet_action=mac_action)

        self.fdb_entry2 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.dmac,
            bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry2,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port27_bp,
            packet_action=mac_action)

    def runTest(self):
        self.testIPv4Acl()
        self.testIPv6Acl()
        self.testIngressIPAclMirrorIn()

    def tearDown(self):
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry1)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry2)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member1)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member2)
        sai_thrift_remove_bridge_port(self.client, self.port26_bp)
        sai_thrift_remove_bridge_port(self.client, self.port27_bp)
        sai_thrift_remove_vlan(self.client, self.vlan_oid)

        sai_thrift_remove_route_entry(self.client, self.route_entry0)
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_router_interface(self.client, self.port25_rif)
        sai_thrift_remove_router_interface(self.client, self.port24_rif)
        super(AclTableTypeTest, self).tearDown()

    def testIPv4Acl(self):
        '''
        Verify various IPv4 field combinations for table creation
        '''
        print("testIPv4Acl")
        pkt1 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                 eth_src=self.src_mac,
                                 ip_dst=self.ip_addr1,
                                 ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(eth_dst=self.dmac,
                                     eth_src=ROUTER_MAC,
                                     ip_dst=self.ip_addr1,
                                     ip_ttl=63)

        pkt2 = simple_tcp_packet(eth_dst=self.dmac,
                                 eth_src=self.src_mac,
                                 dl_vlan_enable=True,
                                 vlan_vid=100,
                                 ip_src=self.ip_addr2,
                                 ip_dst=self.ip_addr1,
                                 ip_id=102,
                                 ip_ttl=64)
        exp_pkt2 = simple_tcp_packet(eth_dst=self.dmac,
                                     eth_src=self.src_mac,
                                     ip_dst=self.ip_addr1,
                                     ip_src=self.ip_addr2,
                                     ip_id=102,
                                     dl_vlan_enable=True,
                                     vlan_vid=100,
                                     ip_ttl=64)

        try:
            # verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port25)

            send_packet(self, self.dev_port26, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port27)

            # create ACL table
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                                 SAI_ACL_BIND_POINT_TYPE_LAG]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_ip=True,
                field_dst_ip=True,
                field_ip_protocol=True,
                field_dscp=True,
                field_l4_src_port=True,
                field_l4_dst_port=True,
                field_ttl=True,
                field_tcp_flags=True,
                field_ether_type=True,
                field_acl_range_type=self.acl_range_type,
                field_icmp_code=True,
                field_icmp_type=True,
                field_acl_ip_frag=True,
                field_acl_ip_type=True,
                field_outer_vlan_id=True)

            # create ACL table entry
            dst_ip_mask = '255.255.255.255'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=self.ip_addr1),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_DROP))
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                action_packet_action=packet_action)

            # create ACL counter
            acl_counter_ingress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            # attach ACL counter to ACL entry
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_ingress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)

            # bind ACL table to ingress port 24
            sai_thrift_set_port_attribute(
                self.client, self.port24, ingress_acl=acl_table)

            sai_thrift_set_port_attribute(
                self.client, self.port26, ingress_acl=acl_table)

            # verify packet dropped after ACL apply
            send_packet(self, self.dev_port24, pkt1)
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            send_packet(self, self.dev_port26, pkt2)
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)

    def testIPv6Acl(self):
        '''
        Verify various IPv6 field combinations for table creation
        '''
        print("testIPv6Acl")
        pkt1 = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=63)

        pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)

        try:
            # verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port25)

            send_packet(self, self.dev_port26, pkt2)
            # verify_no_other_packets(self)
            verify_packet(self, exp_pkt2, self.dev_port27)

            # create ACL table
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                                 SAI_ACL_BIND_POINT_TYPE_LAG]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_ip_protocol=True,
                field_ipv6_next_header=True,
                field_dscp=True,
                field_l4_src_port=True,
                field_l4_dst_port=True,
                field_ttl=True,
                field_tcp_flags=True,
                field_ether_type=True,
                field_ipv6_flow_label=True,
                field_acl_range_type=self.acl_range_type,
                field_icmpv6_code=True,
                field_icmpv6_type=True,
                field_acl_ip_frag=True,
                field_acl_ip_type=True,
                field_outer_vlan_id=True)

            # create ACL table entry
            dst_ip = '1234:5678:9abc:def0:4422:1133:5577:99aa'
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))

            packet_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_DROP))
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ipv6=dst_ip_t,
                action_packet_action=packet_action)

            # create ACL counter
            acl_counter_ingress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            # attach ACL counter to ACL entry
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_ingress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)

            # bind ACL table to ingress port 24
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=acl_table)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=acl_table)

            # verify packet dropped after ACL apply
            send_packet(self, self.dev_port24, pkt1)
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            send_packet(self, self.dev_port26, pkt2)
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)

    def testIngressIPAclMirrorIn(self):
        '''
        Test Ingress IP mirror Acl Mirror In functionality
        '''
        print("testIngressIPAclMirrorIn")
        pkt1 = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=64)
        e_pkt1 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=63)

        pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        e_pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)

        try:
            #  verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, e_pkt1, self.dev_port25)

            send_packet(self, self.dev_port26, pkt2)
            verify_packet(self, e_pkt2, self.dev_port27)

            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port28,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            #  ACL table
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                                 SAI_ACL_BIND_POINT_TYPE_LAG]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_INGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_ip_protocol=True,
                field_ipv6_next_header=True,
                field_dscp=True,
                field_l4_src_port=True,
                field_l4_dst_port=True,
                field_ttl=True,
                field_tcp_flags=True,
                field_ether_type=True,
                field_acl_range_type=self.acl_range_type,
                field_icmpv6_code=True,
                field_icmpv6_type=True,
                field_acl_ip_frag=True,
                field_acl_ip_type=True,
                field_outer_vlan_id=True)

            #  ACL table entry
            dst_ip = '1234:5678:9abc:def0:4422:1133:5577:99aa'
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ipv6=dst_ip_t,
                action_mirror_ingress=mirror_action)

            #  ACL counter
            acl_counter_ingress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            #  attach ACL counter to ACL entry
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_ingress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)

            #  attach ACL table to ingress port 24, 26
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=acl_table)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=acl_table)

            #  verify route-pkt egress out of port 25
            #  verify if the mirror packet [out of port 28] is incoming pkt
            send_packet(self, self.dev_port24, pkt1)
            verify_each_packet_on_each_port(self, [e_pkt1, pkt1],
                                            [self.dev_port25, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            #  verify switch-pkt egress out of port 27
            #  verify if the mirror packet [out of port 28] is incoming pkt
            send_packet(self, self.dev_port26, pkt2)
            verify_each_packet_on_each_port(self, [e_pkt2, pkt2],
                                            [self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            #  cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)
            sai_thrift_remove_mirror_session(self.client, mirror_session)


@group('acl-mirror-inout')
class AclTableMirrorInOutTest(SaiHelper):
    '''
    Ingress/Egress ACL Mirror In/out action combinations
    '''
    acl_range_type = sai_thrift_s32_list_t(count=0, int32list=[])

    def setUp(self):
        super(AclTableMirrorInOutTest, self).setUp()

        self.dmac = '00:11:22:33:44:55'
        self.src_mac = '00:22:22:22:22:22'
        self.ip_addr1 = '10.0.0.1'
        self.ip_addr2 = '10.10.10.2'

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
            self.client, self.neighbor_entry1, dst_mac_address=self.dmac)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:99aa/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry1, next_hop_id=self.nhop1)

        self.vlan_oid = sai_thrift_create_vlan(self.client, 100)
        mac_action = SAI_PACKET_ACTION_FORWARD

        self.port26_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port26,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port26_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.port27_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port27,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port27_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.fdb_entry1 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.src_mac,
            bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry1,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port26_bp,
            packet_action=mac_action)

        self.fdb_entry2 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.dmac,
            bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry2,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port27_bp,
            packet_action=mac_action)

    def runTest(self):
        self.testIngressIPAclMirrorIn()
        self.testIngressIPAclMirrorOut()
        self.testEgressIPAclMirrorIn()
        self.testEgressIPAclMirrorOut()

    def tearDown(self):
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry1)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry2)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member1)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member2)
        sai_thrift_remove_bridge_port(self.client, self.port26_bp)
        sai_thrift_remove_bridge_port(self.client, self.port27_bp)
        sai_thrift_remove_vlan(self.client, self.vlan_oid)

        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_router_interface(self.client, self.port25_rif)
        sai_thrift_remove_router_interface(self.client, self.port24_rif)
        super(AclTableMirrorInOutTest, self).tearDown()

    def testIngressIPAclMirrorIn(self):
        '''
        Test Ingress IP mirror Acl Mirror In functionality
        '''
        print("testIngressIPAclMirrorIn")
        pkt1 = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=64)
        e_pkt1 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=63)

        pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        e_pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)

        try:
            #  verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, e_pkt1, self.dev_port25)

            send_packet(self, self.dev_port26, pkt2)
            verify_packet(self, e_pkt2, self.dev_port27)

            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port28,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            #  ACL table
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                                 SAI_ACL_BIND_POINT_TYPE_LAG]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_INGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_ip_protocol=True,
                field_ipv6_next_header=True,
                field_dscp=True,
                field_l4_src_port=True,
                field_l4_dst_port=True,
                field_ttl=True,
                field_tcp_flags=True,
                field_ether_type=True,
                field_acl_range_type=self.acl_range_type,
                field_icmpv6_code=True,
                field_icmpv6_type=True,
                field_acl_ip_frag=True,
                field_acl_ip_type=True,
                field_outer_vlan_id=True)

            #  ACL table entry
            dst_ip = '1234:5678:9abc:def0:4422:1133:5577:99aa'
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ipv6=dst_ip_t,
                action_mirror_ingress=mirror_action)

            #  ACL counter
            acl_counter_ingress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            #  attach ACL counter to ACL entry
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_ingress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)

            #  attach ACL table to ingress port 24, 26
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=acl_table)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=acl_table)

            #  verify route-pkt egress out of port 25
            #  verify if the mirror packet [out of port 28] is incoming pkt
            send_packet(self, self.dev_port24, pkt1)
            verify_each_packet_on_each_port(self, [e_pkt1, pkt1],
                                            [self.dev_port25, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            #  verify switch-pkt egress out of port 27
            #  verify if the mirror packet [out of port 28] is incoming pkt
            send_packet(self, self.dev_port26, pkt2)
            verify_each_packet_on_each_port(self, [e_pkt2, pkt2],
                                            [self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            #  cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)
            sai_thrift_remove_mirror_session(self.client, mirror_session)

    def testIngressIPAclMirrorOut(self):
        '''
        Test Ingress IP mirror Acl Mirror Out functionality
        '''
        print("testIngressIPAclMirrorOut")
        pkt1 = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=64)
        e_pkt1 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=63)

        pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        e_pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)

        try:
            #  verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, e_pkt1, self.dev_port25)

            send_packet(self, self.dev_port26, pkt2)
            verify_packet(self, e_pkt2, self.dev_port27)

            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port28,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            #  ACL table
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                                 SAI_ACL_BIND_POINT_TYPE_LAG]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_EGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_ip_protocol=True,
                field_ipv6_next_header=True,
                field_dscp=True,
                field_l4_src_port=True,
                field_l4_dst_port=True,
                field_ttl=True,
                field_tcp_flags=True,
                field_ether_type=True,
                field_acl_range_type=self.acl_range_type,
                field_icmpv6_code=True,
                field_icmpv6_type=True,
                field_acl_ip_frag=True,
                field_acl_ip_type=True,
                field_outer_vlan_id=True)

            #  ACL table entry
            dst_ip = '1234:5678:9abc:def0:4422:1133:5577:99aa'
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ipv6=dst_ip_t,
                action_mirror_egress=mirror_action)

            #  ACL counter
            acl_counter_ingress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            #  attach ACL counter to ACL entry
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_ingress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)

            #  attach ACL table to ingress port 24, 26
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=acl_table)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=acl_table)

            #  verify route-pkt egress out of port 25
            #  verify if the mirror packet [out of port 28] is route-pkt
            send_packet(self, self.dev_port24, pkt1)
            verify_each_packet_on_each_port(self, [e_pkt1, e_pkt1],
                                            [self.dev_port25, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            #  verify switch-pkt egress out of port 27
            #  verify if the mirror packet [out of port 28] is switch-pkt
            send_packet(self, self.dev_port26, pkt2)
            verify_each_packet_on_each_port(self, [e_pkt2, e_pkt2],
                                            [self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            #  cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)
            sai_thrift_remove_mirror_session(self.client, mirror_session)

    def testEgressIPAclMirrorIn(self):
        '''
        Test Egress IP mirror Acl Mirror In functionality
        '''
        print("testEgressIPAclMirrorIn")
        pkt1 = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=64)
        e_pkt1 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=63)

        pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        e_pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)

        try:
            #  verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, e_pkt1, self.dev_port25)

            send_packet(self, self.dev_port26, pkt2)
            verify_packet(self, e_pkt2, self.dev_port27)

            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port28,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            #  ACL table
            table_stage = SAI_ACL_STAGE_EGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                                 SAI_ACL_BIND_POINT_TYPE_LAG]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_INGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_ip_protocol=True,
                field_ipv6_next_header=True,
                field_dscp=True,
                field_l4_src_port=True,
                field_l4_dst_port=True,
                field_ttl=True,
                field_tcp_flags=True,
                field_ether_type=True,
                field_acl_range_type=self.acl_range_type,
                field_icmpv6_code=True,
                field_icmpv6_type=True,
                field_acl_ip_frag=True,
                field_acl_ip_type=True,
                field_outer_vlan_id=True)

            #  ACL table entry
            dst_ip = '1234:5678:9abc:def0:4422:1133:5577:99aa'
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ipv6=dst_ip_t,
                action_mirror_ingress=mirror_action)

            #  ACL counter
            acl_counter_egress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            #  attach ACL counter to ACL entry
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_egress)

            #  attach ACL table to egress port 25, 27
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          egress_acl=acl_table)
            sai_thrift_set_port_attribute(self.client, self.port27,
                                          egress_acl=acl_table)

            #  verify route-pkt egress out of port 25
            #  verify if the mirror packet [out of port 28] is incoming pkt
            send_packet(self, self.dev_port24, pkt1)
            verify_each_packet_on_each_port(self, [e_pkt1, pkt1],
                                            [self.dev_port25, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 1)

            #  verify switch-pkt egress out of port 27
            #  verify if the mirror packet [out of port 28] is incoming pkt
            send_packet(self, self.dev_port26, pkt2)
            verify_each_packet_on_each_port(self, [e_pkt2, pkt2],
                                            [self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            #  cleanup ACL
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_egress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          egress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port27,
                                          egress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)
            sai_thrift_remove_mirror_session(self.client, mirror_session)

    def testEgressIPAclMirrorOut(self):
        '''
        Test Egress IP mirror Acl Mirror Out functionality
        '''
        print("testEgressIPAclMirrorOut")
        pkt1 = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=64)
        e_pkt1 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=63)

        pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        e_pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac,
            eth_src=self.src_mac,
            dl_vlan_enable=True,
            vlan_vid=100,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)

        try:
            #  verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, e_pkt1, self.dev_port25)

            send_packet(self, self.dev_port26, pkt2)
            verify_packet(self, e_pkt2, self.dev_port27)

            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port28,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            #  ACL table
            table_stage = SAI_ACL_STAGE_EGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT,
                                 SAI_ACL_BIND_POINT_TYPE_LAG]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_EGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_ip_protocol=True,
                field_ipv6_next_header=True,
                field_dscp=True,
                field_l4_src_port=True,
                field_l4_dst_port=True,
                field_ttl=True,
                field_tcp_flags=True,
                field_ether_type=True,
                field_acl_range_type=self.acl_range_type,
                field_icmpv6_code=True,
                field_icmpv6_type=True,
                field_acl_ip_frag=True,
                field_acl_ip_type=True,
                field_outer_vlan_id=True)

            #  ACL table entry
            dst_ip = '1234:5678:9abc:def0:4422:1133:5577:99aa'
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ipv6=dst_ip_t,
                action_mirror_egress=mirror_action)

            #  ACL counter
            acl_counter_egress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            #  attach ACL counter to ACL entry
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_egress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_egress)

            #  attach ACL table to egress port 25, 27
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          egress_acl=acl_table)
            sai_thrift_set_port_attribute(self.client, self.port27,
                                          egress_acl=acl_table)

            #  verify route-pkt egress out of port 25
            #  verify if the mirror packet [out of port 28] is route-pkt
            send_packet(self, self.dev_port24, pkt1)
            verify_each_packet_on_each_port(self, [e_pkt1, e_pkt1],
                                            [self.dev_port25, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 1)

            #  verify switch-pkt egress out of port 27
            #  verify if the mirror packet [out of port 28] is switch-pkt
            send_packet(self, self.dev_port26, pkt2)
            verify_each_packet_on_each_port(self, [e_pkt2, e_pkt2],
                                            [self.dev_port27, self.dev_port28])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            #  cleanup ACL
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter_egress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          egress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port27,
                                          egress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_table(self.client, acl_table)
            sai_thrift_remove_mirror_session(self.client, mirror_session)


@group('acl-redirect')
class AclRedirectPortAndLagTest(SaiHelper):
    """
    Verify ACL redirection for ports and lags test cases
    """
    def setUp(self):
        super(AclRedirectPortAndLagTest, self).setUp()
        self.acl_grp_members = []
        self.acl_grps = []
        self.acl_rules = []
        self.acl_tables = []
        self.vlan_members = []
        self.vlan_ports = []
        self.bridge_ports = []
        self.fdbs = []
        self.lags = []
        self.lag_members = []
        self.action_counters = []
        self.counters = []

        self.mac = '00:11:11:11:11:11'
        mac_action = SAI_PACKET_ACTION_FORWARD

        # Add port 24, 25, 26 to Vlan100
        vlan_id = 100
        self.vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)

        port24_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.bridge_ports.append(port24_bp)

        vlan_member1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member1)
        self.vlan_ports.append(self.port24)

        port25_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port25,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.bridge_ports.append(port25_bp)

        vlan_member2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=port25_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member2)
        self.vlan_ports.append(self.port25)

        port26_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port26,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.bridge_ports.append(port26_bp)

        vlan_member3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=port26_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member3)
        self.vlan_ports.append(self.port26)

        sai_thrift_set_port_attribute(
            self.client, self.port24, port_vlan_id=vlan_id)
        sai_thrift_set_port_attribute(
            self.client, self.port25, port_vlan_id=vlan_id)
        sai_thrift_set_port_attribute(
            self.client, self.port26, port_vlan_id=vlan_id)

        # Create Lag (port 27, 28) and add it to Vlan100
        self.lag_id = sai_thrift_create_lag(self.client)
        self.lags.append(self.lag_id)
        lag1_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.lag_id,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.bridge_ports.append(lag1_bp)
        lag_member_id1 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag_id, port_id=self.port27)
        self.lag_members.append(lag_member_id1)

        lag_member_id2 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag_id, port_id=self.port28)
        self.lag_members.append(lag_member_id2)

        vlan_member4 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=lag1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.vlan_members.append(vlan_member4)
        self.vlan_ports.append(self.port27)
        self.vlan_ports.append(self.port28)

        fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.mac,
            bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            fdb_entry,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=port24_bp,
            packet_action=mac_action)
        self.fdbs.append(fdb_entry)

    def runTest(self):
        print("Testing AclRedirectPortAndLagTest")
        print('-------------------------------------------------------------')

        eth_pkt1 = simple_eth_packet(pktlen=100,
                                     eth_dst=self.mac,
                                     eth_src='00:06:07:08:09:0a',
                                     eth_type=0x8137)
        eth_pkt2 = simple_eth_packet(pktlen=100,
                                     eth_dst=self.mac,
                                     eth_src='00:06:07:08:09:0a',
                                     eth_type=0x8136)
        eth_pkt3 = simple_eth_packet(pktlen=100,
                                     eth_dst=self.mac,
                                     eth_src='00:06:07:08:09:0a',
                                     eth_type=0x8135)
        eth_pkt4 = simple_eth_packet(pktlen=100,
                                     eth_dst=self.mac,
                                     eth_src='00:06:07:08:09:0a',
                                     eth_type=0x8134)
        neg_test_pkt = simple_eth_packet(pktlen=100,
                                         eth_dst=self.mac,
                                         eth_src='00:06:07:08:09:0a',
                                         eth_type=0x1111)

        print('#### NO ACL Applied ####')
        # send the test packet(s)
        print("Sending Test packet EthType:0x%lx port 25 -> port 24" % (
            eth_pkt1[Ether].type))
        send_packet(self, self.dev_port25, eth_pkt1)
        verify_packets(self, eth_pkt1, [self.dev_port24])

        print("Sending Test packet EthType:0x%lx port 25 -> port 24" % (
            eth_pkt2[Ether].type))
        send_packet(self, self.dev_port25, eth_pkt2)
        verify_packets(self, eth_pkt2, [self.dev_port24])

        print("Sending Test packet EthType:0x%lx port 25 -> port 24" % (
            eth_pkt3[Ether].type))
        send_packet(self, self.dev_port25, eth_pkt3)
        verify_packets(self, eth_pkt3, [self.dev_port24])

        print("Sending Test packet EthType:0x%lx port 25 -> port 24" % (
            eth_pkt4[Ether].type))
        send_packet(self, self.dev_port25, eth_pkt4)
        verify_packets(self, eth_pkt4, [self.dev_port24])

        print("Sending Test(negative test) packet EthType:0x%lx port 25 -> "
              "port 24" % (neg_test_pkt[Ether].type))
        send_packet(self, self.dev_port25, neg_test_pkt)
        verify_packets(self, neg_test_pkt, [self.dev_port24])
        print("Sending Test(negative test) packet EthType:0x%lx port 25 -> "
              "port 24" % (neg_test_pkt[Ether].type))

        # setup ACL to redirect based on Ether type
        entry_priority = 1
        acl_action = SAI_ACL_ENTRY_ATTR_ACTION_REDIRECT

        # setup ACL table group
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL
        group_stage_ingress = SAI_ACL_STAGE_INGRESS
        group_member_priority = 100

        # setup ACL table
        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]

        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_point_list),
            int32list=group_bind_point_list)

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        # create ACL table group
        acl_table_group_ingress = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=group_stage_ingress,
            acl_bind_point_type_list=group_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(acl_table_group_ingress)

        # create ACL tables
        print("Create ACL tables")
        acl_table_id_ingress = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list)

        self.acl_tables.append(acl_table_id_ingress)
        self.assertTrue((acl_table_id_ingress != 0),
                        "ACL table create failed")
        print("IPV4 ACL Table created 0x%lx" % (acl_table_id_ingress))

        # create ACL table group members
        acl_group_member_id_ingress = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=acl_table_group_ingress,
                acl_table_id=acl_table_id_ingress,
                priority=group_member_priority)

        self.assertTrue(acl_group_member_id_ingress != 0,
                        "ACL group member add failed for ACL table 0x%lx, "
                        "acl group 0x%lx" % (
                            acl_table_id_ingress, acl_table_group_ingress))
        self.acl_grp_members.append(acl_group_member_id_ingress)

        eth_type = 0x8137 - 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        redirect_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=acl_action,
                oid=self.port26))

        # create ACL entries
        print("Create ACL entries")
        acl_ip_entry_id_ingress1 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id_ingress,
            priority=entry_priority,
            field_ether_type=ether_type,
            action_redirect=redirect_action)

        # create ACL counter
        acl_counter_ingress1 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id_ingress)

        # attach ACL counter to ACL entry
        action_counter_ingress1 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress1),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ip_entry_id_ingress1,
            action_counter=action_counter_ingress1)

        self.counters.append(acl_counter_ingress1)
        self.action_counters.append(action_counter_ingress1)

        self.acl_rules.append(acl_ip_entry_id_ingress1)
        self.assertTrue((acl_ip_entry_id_ingress1 != 0), 'ACL entry Match: '
                        'EthType-0x%lx Action: Redirect-0x%lx, create '
                        'failed for ACL table 0x%lx' % (
                            eth_type, self.port26, acl_table_id_ingress))
        print("ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx "
              "created 0x%lx" % (eth_pkt1[Ether].type, self.port26,
                                 acl_ip_entry_id_ingress1))

        entry_priority += 1
        eth_type = 0x8136 - 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        redirect_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=acl_action,
                oid=self.lag_id))

        acl_ip_entry_id_ingress2 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id_ingress,
            priority=entry_priority,
            field_ether_type=ether_type,
            action_redirect=redirect_action)

        # create ACL counter
        acl_counter_ingress2 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id_ingress)

        # attach ACL counter to ACL entry
        action_counter_ingress2 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress2),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ip_entry_id_ingress2,
            action_counter=action_counter_ingress2)

        self.counters.append(acl_counter_ingress2)
        self.action_counters.append(action_counter_ingress2)

        self.acl_rules.append(acl_ip_entry_id_ingress2)
        self.assertTrue((acl_ip_entry_id_ingress2 != 0), 'ACL entry Match: '
                        'EthType-0x%lx Action: Redirect-0x%lx, create '
                        'failed for ACL table 0x%lx' % (
                            eth_type, self.lag_id, acl_table_id_ingress))
        print("ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx "
              "created 0x%lx" % (eth_pkt2[Ether].type, self.lag_id,
                                 acl_ip_entry_id_ingress2))

        # create ACL table group members
        acl_group_member_id_ingress = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=acl_table_group_ingress,
                acl_table_id=acl_table_id_ingress,
                priority=200)

        self.assertTrue(acl_group_member_id_ingress != 0,
                        "ACL group member add failed for ACL table 0x%lx, "
                        "ACL group 0x%lx" % (
                            acl_table_id_ingress, acl_table_group_ingress))
        self.acl_grp_members.append(acl_group_member_id_ingress)

        eth_type = 0x8135 - 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        redirect_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=acl_action,
                oid=self.port26))

        print("Create ACL entries")
        acl_ip_entry_id_ingress3 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id_ingress,
            priority=entry_priority,
            field_ether_type=ether_type,
            action_redirect=redirect_action)

        # create ACL counter
        acl_counter_ingress3 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id_ingress)

        # attach ACL counter to ACL entry
        action_counter_ingress3 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress3),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ip_entry_id_ingress3,
            action_counter=action_counter_ingress3)

        self.counters.append(acl_counter_ingress3)
        self.action_counters.append(action_counter_ingress3)

        self.acl_rules.append(acl_ip_entry_id_ingress3)
        self.assertTrue((acl_ip_entry_id_ingress3 != 0), 'ACL entry Match: '
                        'EthType-0x%lx Action: Redirect-0x%lx, create '
                        'failed for acl table 0x%lx' % (
                            eth_type, self.port26, acl_table_id_ingress))
        print("ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx "
              "created 0x%lx" % (eth_pkt3[Ether].type, self.port26,
                                 acl_ip_entry_id_ingress3))

        eth_type = 0x8134 - 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        redirect_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=acl_action,
                oid=self.lag_id))

        print("Create ACL entries")
        acl_ip_entry_id_ingress4 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id_ingress,
            priority=entry_priority,
            field_ether_type=ether_type,
            action_redirect=redirect_action)

        # create ACL counter
        acl_counter_ingress4 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id_ingress)

        # attach ACL counter to ACL entry
        action_counter_ingress4 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress4),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_ip_entry_id_ingress4,
            action_counter=action_counter_ingress4)

        self.counters.append(acl_counter_ingress4)
        self.action_counters.append(action_counter_ingress4)

        self.acl_rules.append(acl_ip_entry_id_ingress4)
        self.assertTrue((acl_ip_entry_id_ingress4 != 0), 'ACL entry Match: '
                        'EthType-0x%lx Action: Redirect-0x%lx, create '
                        'failed for ACL table 0x%lx' % (
                            eth_type, self.lag_id, acl_table_id_ingress))
        print("ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx "
              "created 0x%lx" % (eth_pkt3[Ether].type, self.lag_id,
                                 acl_ip_entry_id_ingress4))

        print("Binding ACL grp 0x%lx to Port25" % (acl_table_group_ingress))
        # bind ACL GRP to Port25
        sai_thrift_set_port_attribute(
            self.client, self.port25, ingress_acl=acl_table_group_ingress)

        print("Sending Test packet EthType:0x%lx port 25 -> [ACL REDIRECT] "
              "-> port 26" % (eth_pkt1[Ether].type))
        # ensure packet is redirected!
        send_packet(self, self.dev_port25, eth_pkt1)
        verify_packets(self, eth_pkt1, [self.dev_port26])

        packets1 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress1, packets=True)
        self.assertEqual(packets1['packets'], 1)
        packets2 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress2, packets=True)
        self.assertEqual(packets2['packets'], 0)
        packets3 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress3, packets=True)
        self.assertEqual(packets3['packets'], 0)
        packets4 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress4, packets=True)
        self.assertEqual(packets4['packets'], 0)

        # ensure packet is redirected!
        print("Sending Test packet EthType:0x%lx port 25 -> [ACL REDIRECT] "
              "-> Lag1 (Port 26/Port 27)" % (eth_pkt2[Ether].type))
        send_packet(self, self.dev_port25, eth_pkt2)
        verify_packets_any(self, eth_pkt2, [self.dev_port27,
                                            self.dev_port28])

        packets1 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress1, packets=True)
        self.assertEqual(packets1['packets'], 1)
        packets2 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress2, packets=True)
        self.assertEqual(packets2['packets'], 1)
        packets3 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress3, packets=True)
        self.assertEqual(packets3['packets'], 0)
        packets4 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress4, packets=True)
        self.assertEqual(packets4['packets'], 0)

        # ensure packet is redirected!
        print("Sending Test packet EthType:0x%lx port 25 -> [ACL REDIRECT] "
              "-> port 26" % (eth_pkt3[Ether].type))
        send_packet(self, self.dev_port25, eth_pkt3)
        verify_packets(self, eth_pkt3, [self.dev_port26])

        packets1 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress1, packets=True)
        self.assertEqual(packets1['packets'], 1)
        packets2 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress2, packets=True)
        self.assertEqual(packets2['packets'], 1)
        packets3 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress3, packets=True)
        self.assertEqual(packets3['packets'], 1)
        packets4 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress4, packets=True)
        self.assertEqual(packets4['packets'], 0)

        # ensure packet is redirected!
        print("Sending Test packet EthType:0x%lx port 25 -> [ACL REDIRECT] "
              "-> Lag1 (Port 27/Port 28)" % (eth_pkt4[Ether].type))
        send_packet(self, self.dev_port25, eth_pkt4)
        verify_packets_any(self, eth_pkt4, [self.dev_port27,
                                            self.dev_port28])

        packets1 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress1, packets=True)
        self.assertEqual(packets1['packets'], 1)
        packets2 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress2, packets=True)
        self.assertEqual(packets2['packets'], 1)
        packets3 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress3, packets=True)
        self.assertEqual(packets3['packets'], 1)
        packets4 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress4, packets=True)
        self.assertEqual(packets4['packets'], 1)

        # ensure packet is not redirected!
        print("Sending Test(negative test) packet EthType:0x%lx port 25 -> "
              "port 24" % (neg_test_pkt[Ether].type))
        send_packet(self, self.dev_port25, neg_test_pkt)
        verify_packets(self, neg_test_pkt, [self.dev_port24])

        packets1 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress1, packets=True)
        self.assertEqual(packets1['packets'], 1)
        packets2 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress2, packets=True)
        self.assertEqual(packets2['packets'], 1)
        packets3 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress3, packets=True)
        self.assertEqual(packets3['packets'], 1)
        packets4 = sai_thrift_get_acl_counter_attribute(
            self.client, acl_counter_ingress4, packets=True)
        self.assertEqual(packets4['packets'], 1)

    def tearDown(self):
        # Clean up ACL configuration
        sai_thrift_set_port_attribute(
            self.client, self.port25, ingress_acl=int(SAI_NULL_OBJECT_ID))
        for acl_grp_member in list(self.acl_grp_members):
            sai_thrift_remove_acl_table_group_member(self.client,
                                                     acl_grp_member)
            self.acl_grp_members.remove(acl_grp_member)
        for acl_grp in list(self.acl_grps):
            sai_thrift_remove_acl_table_group(self.client, acl_grp)
            self.acl_grps.remove(acl_grp)

        for i, acl_action_counter in enumerate(self.action_counters):
            acl_action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, self.acl_rules[i],
                action_counter=acl_action_counter)

        for acl_counter in self.counters:
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter)

        for acl_rule in list(self.acl_rules):
            sai_thrift_remove_acl_entry(self.client, acl_rule)
            self.acl_rules.remove(acl_rule)

        for acl_table in list(self.acl_tables):
            sai_thrift_remove_acl_table(self.client, acl_table)
            self.acl_tables.remove(acl_table)

        for fdb in list(self.fdbs):
            sai_thrift_remove_fdb_entry(self.client, fdb)
            self.fdbs.remove(fdb)

        # Clean up network configuration
        sai_thrift_set_port_attribute(
            self.client, self.port24, port_vlan_id=int(SAI_NULL_OBJECT_ID))
        sai_thrift_set_port_attribute(
            self.client, self.port25, port_vlan_id=int(SAI_NULL_OBJECT_ID))
        sai_thrift_set_port_attribute(
            self.client, self.port26, port_vlan_id=int(SAI_NULL_OBJECT_ID))

        for vlan_member in list(self.vlan_members):
            sai_thrift_remove_vlan_member(self.client, vlan_member)
            self.vlan_members.remove(vlan_member)
        sai_thrift_remove_vlan(self.client, self.vlan_oid)
        for lag_member in list(self.lag_members):
            sai_thrift_remove_lag_member(self.client, lag_member)
            self.lag_members.remove(lag_member)
        for port in list(self.bridge_ports):
            sai_thrift_remove_bridge_port(self.client, port)
            self.bridge_ports.remove(port)
        for lag in list(self.lags):
            sai_thrift_remove_lag(self.client, lag)
            self.lags.remove(lag)
        for vlan_port in list(self.vlan_ports):
            self.vlan_ports.remove(vlan_port)

        super(AclRedirectPortAndLagTest, self).tearDown()


@disabled
@group("acl-ip-type")
class AclIpTypeTrapTest(SaiHelper):
    '''
    Test ACL IP type field
    '''

    def setUp(self):
        super(AclIpTypeTrapTest, self).setUp()

        hostif_name = "hostif1"
        self.port = self.port24
        self.dev_port = self.dev_port24

        self.hostif_oid = sai_thrift_create_hostif(self.client,
                                                   name=hostif_name,
                                                   obj_id=self.port,
                                                   type=SAI_HOSTIF_TYPE_NETDEV)

        self.assertNotEqual(self.hostif_oid, 0)

        self.hostif_socket = open_packet_socket(hostif_name)

        self.udt_oid = sai_thrift_create_hostif_user_defined_trap(
            self.client,
            type=SAI_HOSTIF_USER_DEFINED_TRAP_TYPE_ACL)
        self.assertNotEqual(self.udt_oid, 0)

        channel = SAI_HOSTIF_TABLE_ENTRY_CHANNEL_TYPE_NETDEV_PHYSICAL_PORT
        self.hostif_table_entry_oid = \
            sai_thrift_create_hostif_table_entry(
                self.client,
                channel_type=channel,
                host_if=self.hostif_oid,
                trap_id=self.udt_oid,
                type=SAI_HOSTIF_TABLE_ENTRY_TYPE_TRAP_ID)
        self.assertNotEqual(self.hostif_table_entry_oid, 0)

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)
        self.acl_table_oid = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_acl_ip_type=True)
        self.assertNotEqual(self.acl_table_oid, 0)

        self.set_user_trap_id = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(oid=self.udt_oid))

        self.packet_action_trap = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_TRAP))

        self.ip4_pkt = simple_ip_packet()
        self.ip6_pkt = simple_ipv6ip_packet()
        self.lldp_pkt = simple_eth_packet(eth_type=0x88cc)

    def tearDown(self):
        sai_thrift_remove_acl_table(self.client, self.acl_table_oid)
        sai_thrift_remove_hostif_table_entry(
            self.client, self.hostif_table_entry_oid)
        sai_thrift_remove_hostif_user_defined_trap(
            self.client, self.udt_oid)
        sai_thrift_remove_hostif(self.client, self.hostif_oid)
        super(AclIpTypeTrapTest, self).tearDown()

    def runTest(self):
        try:
            self.testAclTypeAny()
            self.testAclTypeIpv4Any()
            # self.testAclTypeIpv6Any()
        finally:
            pass

    def testAclTypeAny(self):
        '''
        Test SAI_ACL_IP_TYPE_ANY
        '''
        print("testAclTypeAny")
        acl_entry_oid = None
        try:
            acl_ip_type = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(s32=SAI_ACL_IP_TYPE_ANY))

            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_acl_ip_type=acl_ip_type,
                action_set_user_trap_id=self.set_user_trap_id,
                action_packet_action=self.packet_action_trap)
            self.assertNotEqual(acl_entry_oid, 0)

            print("Sending IPv4 packet")
            send_packet(self, self.dev_port, self.ip4_pkt)
            print("Verifying IPv4 packet on port host interface")
            self.assertTrue(
                socket_verify_packet(
                    self.ip4_pkt,
                    self.hostif_socket))

            print("Sending IPv6 packet")
            send_packet(self, self.dev_port, self.ip6_pkt)
            print("Verifying IPv6 packet on port host interface")
            self.assertTrue(socket_verify_packet(self.ip6_pkt,
                                                 self.hostif_socket))

            print("Sending LLDP packet")
            send_packet(self, self.dev_port, self.lldp_pkt)
            print("Verifying LLDP packet on port host interface")
            self.assertTrue(
                socket_verify_packet(
                    self.lldp_pkt,
                    self.hostif_socket))

            print("\tOK")
        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

    def testAclTypeIpv4Any(self):
        '''
        Test SAI_ACL_IP_TYPE_IPV4ANY
        '''
        print("testAclTypeIpv4Any")
        acl_entry_oid = None
        try:
            acl_ip_type = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    s32=SAI_ACL_IP_TYPE_IPV4ANY))

            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_acl_ip_type=acl_ip_type,
                action_set_user_trap_id=self.set_user_trap_id,
                action_packet_action=self.packet_action_trap)
            self.assertNotEqual(acl_entry_oid, 0)

            print("Sending IPv4 packet")
            send_packet(self, self.dev_port, self.ip4_pkt)
            print("Verifying IPv4 packet on port host interface")
            self.assertTrue(
                socket_verify_packet(
                    self.ip4_pkt,
                    self.hostif_socket))

            print("Sending IPv6 packet")
            send_packet(self, self.dev_port, self.ip6_pkt)
            print("Verifying no IPv6 packet on port host interface")
            self.assertTrue(not socket_verify_packet(self.ip6_pkt,
                                                     self.hostif_socket))

            print("Sending LLDP packet")
            send_packet(self, self.dev_port, self.lldp_pkt)
            print("Verifying no LLDP packet on port host interface")
            self.assertTrue(
                not socket_verify_packet(
                    self.lldp_pkt,
                    self.hostif_socket))

            print("\tOK")
        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)

    def testAclTypeIpv6Any(self):
        '''
        Test SAI_ACL_IP_TYPE_IPV6ANY
        '''
        print("testAclTypeIpv6Any")
        acl_entry_oid = None
        try:
            acl_ip_type = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    s32=SAI_ACL_IP_TYPE_IPV6ANY))

            acl_entry_oid = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table_oid,
                priority=10,
                field_acl_ip_type=acl_ip_type,
                action_set_user_trap_id=self.set_user_trap_id,
                action_packet_action=self.packet_action_trap)
            self.assertNotEqual(acl_entry_oid, 0)

            print("Sending IPv4 packet")
            send_packet(self, self.dev_port, self.ip4_pkt)
            print("Verifying no IPv4 packet on port host interface")
            self.assertTrue(
                socket_verify_packet(
                    self.ip4_pkt,
                    self.hostif_socket))

            print("Sending IPv6 packet")
            send_packet(self, self.dev_port, self.ip6_pkt)
            print("Verifying IPv6 packet on port host interface")
            self.assertTrue(
                not socket_verify_packet(
                    self.ip6_pkt,
                    self.hostif_socket))

            print("Sending LLDP packet")
            send_packet(self, self.dev_port, self.lldp_pkt)
            print("Verifying no LLDP packet on port host interface")
            self.assertTrue(
                not socket_verify_packet(
                    self.lldp_pkt,
                    self.hostif_socket))

            print("\tOK")
        finally:
            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)


@group('acl-pre-ingress')
class AclPreIngressTest(AclTableTypeTest):
    '''
    Verify pre-ingress ACL
    '''

    def setUp(self):
        super(AclPreIngressTest, self).setUp()

        self.dmac1 = '00:11:22:33:44:55'
        self.dmac2 = '00:11:22:33:44:56'
        self.ip_addr1 = '10.0.0.1'
        self.ip_addr2 = '10.10.10.2'
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

    def runTest(self):
        self.testPreIngressAcl()

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.vrf1_route_entry0)
        sai_thrift_remove_next_hop(self.client, self.vrf1_nhop0)
        sai_thrift_remove_neighbor_entry(
            self.client, self.vrf1_neighbor_entry0)
        sai_thrift_remove_router_interface(self.client, self.vrf1_port26_rif)
        sai_thrift_remove_virtual_router(self.client, self.vrf1)
        super(AclPreIngressTest, self).tearDown()

    def testPreIngressAcl(self):
        '''
        Verify pre-ingress matching and VRF assignment
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
                field_src_mac=True,
                field_dst_mac=True,
                field_ether_type=True,
                field_src_ip=True,
                field_dst_ip=True,
                field_tos=True)
            self.assertNotEqual(acl_table_oid, 0)

            src_mac = '00:26:dd:14:c4:ee'
            src_mac_mask = 'ff:ff:ff:ff:ff:ff'
            src_mac_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(mac=src_mac),
                mask=sai_thrift_acl_field_data_mask_t(mac=src_mac_mask))

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
            self.assertNotEqual(acl_entry_oid, 0)

            # create ACL counter
            acl_counter_ingress = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table_oid)

            # attach ACL counter to ACL entry
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter_ingress),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_oid,
                action_counter=action_counter_ingress)

            pkt = simple_ip_packet(
                eth_src=src_mac,
                eth_dst=ROUTER_MAC,
                ip_dst=self.ip_addr1,
                ip_ttl=64)

            exp_pkt_default_vrf = simple_ip_packet(
                eth_src=ROUTER_MAC,
                eth_dst=self.dmac1,
                ip_dst=self.ip_addr1,
                ip_ttl=63)

            exp_pkt_vrf1 = simple_ip_packet(
                eth_src=ROUTER_MAC,
                eth_dst=self.dmac2,
                ip_dst=self.ip_addr1,
                ip_ttl=63)

            # send to port in default vrf, expect in default vrf
            # pre ingress is not enabled on switch
            send_packet(self, self.dev_port24, pkt)
            verify_any_packet_on_ports_list(self, pkts=[exp_pkt_default_vrf],
                                            ports=[[self.dev_port25]])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)

            # bind pre-ingress table to switch
            sai_thrift_set_switch_attribute(self.client,
                                            pre_ingress_acl=acl_table_oid)

            # send to port in default vrf, expect in vrf1
            send_packet(self, self.dev_port24, pkt)
            verify_any_packet_on_ports_list(self, pkts=[exp_pkt_vrf1],
                                            ports=[[self.dev_port26]])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_oid,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)
            sai_thrift_set_switch_attribute(self.client, pre_ingress_acl=0)

            if acl_entry_oid:
                sai_thrift_remove_acl_entry(self.client, acl_entry_oid)
            if acl_table_oid:
                sai_thrift_remove_acl_table(self.client, acl_table_oid)


class IPv6NextHdrTest(SaiHelper):
    """
    Verify ACL blocking TCP traffic
    """
    def setUp(self):
        super(IPv6NextHdrTest, self).setUp()
        self.ip_addr1 = '1234:5678:9abc:def0:4422:1133:5577:99aa'
        self.ip_addr2 = '2000::1'
        self.mac1 = '00:11:22:33:44:55'
        self.mac2 = '00:22:22:22:22:22'
        mask = '/112'
        self.table_stage_ingress = SAI_ACL_STAGE_INGRESS
        self.table_stage_egress = SAI_ACL_STAGE_EGRESS

        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress(self.ip_addr1))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry1,
            dst_mac_address=self.mac1)

        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr1),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                self.ip_addr1 + mask))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry1,
            next_hop_id=self.nhop1)

        self.neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.port11_rif, ip_address=sai_ipaddress(self.ip_addr2))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry2,
            dst_mac_address=self.mac2)

        self.nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr2),
            router_interface_id=self.port11_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix(
                self.ip_addr2 + mask))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry2,
            next_hop_id=self.nhop2)

        self.tcpv6_1 = simple_tcpv6_packet(eth_dst=ROUTER_MAC,
                                           eth_src=self.mac2,
                                           ipv6_dst=self.ip_addr1,
                                           ipv6_src=self.ip_addr2,
                                           ipv6_hlim=64)
        self.exp_tcpv6_1 = simple_tcpv6_packet(eth_dst=self.mac1,
                                               eth_src=ROUTER_MAC,
                                               ipv6_dst=self.ip_addr1,
                                               ipv6_src=self.ip_addr2,
                                               ipv6_hlim=63)

        self.udpv6_1 = simple_udpv6_packet(eth_dst=ROUTER_MAC,
                                           eth_src=self.mac2,
                                           ipv6_dst=self.ip_addr1,
                                           ipv6_src=self.ip_addr2,
                                           ipv6_hlim=64)
        self.exp_udpv6_1 = simple_udpv6_packet(eth_dst=self.mac1,
                                               eth_src=ROUTER_MAC,
                                               ipv6_dst=self.ip_addr1,
                                               ipv6_src=self.ip_addr2,
                                               ipv6_hlim=63)

        self.tcpv6_2 = simple_tcpv6_packet(eth_dst=ROUTER_MAC,
                                           eth_src=self.mac1,
                                           ipv6_dst=self.ip_addr2,
                                           ipv6_src=self.ip_addr1,
                                           ipv6_hlim=64)
        self.exp_tcpv6_2 = simple_tcpv6_packet(eth_dst=self.mac2,
                                               eth_src=ROUTER_MAC,
                                               ipv6_dst=self.ip_addr2,
                                               ipv6_src=self.ip_addr1,
                                               ipv6_hlim=63)

        self.udpv6_2 = simple_udpv6_packet(eth_dst=ROUTER_MAC,
                                           eth_src=self.mac1,
                                           ipv6_dst=self.ip_addr2,
                                           ipv6_src=self.ip_addr1,
                                           ipv6_hlim=64)
        self.exp_udpv6_2 = simple_udpv6_packet(eth_dst=self.mac2,
                                               eth_src=ROUTER_MAC,
                                               ipv6_dst=self.ip_addr2,
                                               ipv6_src=self.ip_addr1,
                                               ipv6_hlim=63)

    def runTest(self):
        self.aclRoutingTest()
        self.aclIPv6NextHdrTest(self.table_stage_ingress)
        self.aclIPv6NextHdrTest(self.table_stage_egress)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_route_entry(self.client, self.route_entry2)
        sai_thrift_remove_next_hop(self.client, self.nhop2)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry2)
        super(IPv6NextHdrTest, self).tearDown()

    def aclRoutingTest(self):
        """
        Verify routing without ACL
        """
        try:
            print('----------------------------------------------------------')
            print("Sending packet ptf_intf 2 -> ptf_intf 1 (", self.ip_addr2,
                  " ---> ", self.ip_addr1, ")")
            print('#### NO ACL Applied: sending TCP packets ####')
            print('#### Sending  ', ROUTER_MAC, ' | ', self.mac2, ' | ',
                  self.ip_addr2, ' | ', self.ip_addr1, ' | @ ptf_intf 2')
            send_packet(self, self.dev_port11, self.tcpv6_1)
            print('#### Expecting ', self.mac1, ' | ', ROUTER_MAC, ' | ',
                  self.ip_addr2, ' | ', self.ip_addr1, ' | @ ptf_intf 1')
            verify_packets(self, self.exp_tcpv6_1, [self.dev_port10])
            print('#### NO ACL Applied: sending UDP packets ####')
            send_packet(self, self.dev_port11, self.udpv6_1)
            verify_packets(self, self.exp_udpv6_1, [self.dev_port10])

            print('----------------------------------------------------------')
            print("Sending packet ptf_intf 2 -> ptf_intf 1 (", self.ip_addr1,
                  " ---> ", self.ip_addr2, ")")
            print('#### NO ACL Applied: sending TCP packets ####')
            print('#### Sending  ', ROUTER_MAC, ' | ', self.mac1, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2, ' | @ ptf_intf 2')
            send_packet(self, self.dev_port10, self.tcpv6_2)
            print('#### Expecting ', self.mac2, ' | ', ROUTER_MAC, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2, ' | @ ptf_intf 1')
            verify_packets(self, self.exp_tcpv6_2, [self.dev_port11])
            print('#### NO ACL Applied: sending UDP packets ####')
            send_packet(self, self.dev_port10, self.udpv6_2)
            verify_packets(self, self.exp_udpv6_2, [self.dev_port11])
        finally:
            print('----------------------------------------------------------')

    def aclIPv6NextHdrTest(self, table_stage):
        """
        Verify ACL with next header field
        Args:
            table_stage (int): specifies ingress or egress type of ACL
        """
        # setup ACL to block based on Source IP
        acl_mask = 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        # next level protocol is TCP
        ipv6_next_header = 0x06

        if table_stage == SAI_ACL_STAGE_INGRESS:
            src_ip = self.ip_addr2
            dst_ip = self.ip_addr1
        elif table_stage == SAI_ACL_STAGE_EGRESS:
            src_ip = self.ip_addr1
            dst_ip = self.ip_addr2

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list), int32list=table_bind_point_list)

        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ipv6=True,
            field_ipv6_next_header=True)

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(
                ip6=acl_mask))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        field_ipv6_next_header = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=ipv6_next_header),
            mask=sai_thrift_acl_field_data_mask_t(u8=0x0F))

        # Add drop ACL entry to IPv6 ACL Table
        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            priority=9999,
            table_id=acl_table_id,
            field_src_ipv6=src_ip_t,
            action_packet_action=packet_action,
            field_ipv6_next_header=field_ipv6_next_header)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id)

        # attach ACL counter to ACL entry
        action_counter = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter)

        if table_stage == SAI_ACL_STAGE_INGRESS:
            # bind this ACL table to ports object id
            sai_thrift_set_port_attribute(
                self.client, self.port11, ingress_acl=acl_table_id)
            sport = self.dev_port11
            dport = self.dev_port10
            pkt_udp = self.udpv6_1
            exp_pkt_udp = self.exp_udpv6_1
            pkt_tcp = self.tcpv6_1
            exp_pkt_tcp = self.exp_tcpv6_1
            dmac = self.mac1
            smac = self.mac2
        elif table_stage == SAI_ACL_STAGE_EGRESS:
            # bind this ACL table to ports object id
            sai_thrift_set_port_attribute(
                self.client, self.port11, egress_acl=acl_table_id)
            sport = self.dev_port10
            dport = self.dev_port11
            pkt_udp = self.udpv6_2
            exp_pkt_udp = self.exp_udpv6_2
            pkt_tcp = self.tcpv6_2
            exp_pkt_tcp = self.exp_tcpv6_2
            dmac = self.mac2
            smac = self.mac1

        try:
            self.assertNotEqual(acl_table_id, 0)
            self.assertNotEqual(acl_entry_id, 0)

            print("Sending packet ptf_intf 2-[ACL]-> ptf_intf 1 (", src_ip,
                  " -[ACL]-> ", dst_ip, ")")
            print('#### Sending   TCP', ROUTER_MAC, ' | ', smac, ' | ',
                  src_ip, ' | ', dst_ip, ' | @ ptf_intf 2')
            send_packet(self, sport, pkt_tcp)
            # ensure the TCP packet is dropped and check for absence
            # of packet here
            print('#### NOT Expecting TCP ', dmac, ' | ', ROUTER_MAC, ' | ',
                  src_ip, ' | ', dst_ip, ' | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print('#### Sending   UDP', ROUTER_MAC, ' | ', smac, ' | ',
                  src_ip, ' | ', dst_ip, ' | @ ptf_intf 2')
            send_packet(self, sport, pkt_udp)
            # ensure the UDP packet is forwarded
            print('#### Expecting UDP ', dmac, ' | ', ROUTER_MAC, ' | ',
                  src_ip, ' | ', dst_ip, ' | @ ptf_intf 1')
            verify_packets(self, exp_pkt_udp, [dport])

            # change action_type of ACL entry from ACL_DROP to ACL_PERMIT
            aclaction_data = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_PACKET_ACTION_FORWARD), enable=True)
            sai_thrift_set_acl_entry_attribute(
                client=self.client,
                acl_entry_oid=acl_entry_id,
                action_packet_action=aclaction_data)

            print('#### Sending      ', ROUTER_MAC, ' | ', smac, ' | ', src_ip,
                  ' | ', dst_ip, ' | @ ptf_intf 2')
            # send the same packet
            send_packet(self, sport, pkt_tcp)
            print('#### Expecting ', dmac, ' | ', ROUTER_MAC, ' | ', src_ip,
                  ' | ', dst_ip, ' | @ ptf_intf 1')
            # check that TCP packet is forwarded
            verify_packets(self, exp_pkt_tcp, [dport])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # cleanup ACL
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_id,
                action_counter=action_counter)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter)

            # unbind this ACL table from ports object id
            if table_stage == SAI_ACL_STAGE_INGRESS:
                sai_thrift_set_port_attribute(
                    self.client, self.port11, ingress_acl=0)
            elif table_stage == SAI_ACL_STAGE_EGRESS:
                sai_thrift_set_port_attribute(
                    self.client, self.port11, egress_acl=0)

            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)


@group('acl')
@group('acl-ocp')
class IPAclFragmentTest(SaiHelper):
    """
    Verify ACL with IP fragmentation
    """
    def setUp(self):
        super(IPAclFragmentTest, self).setUp()
        print('--------------------------------------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.0.1 ---> "
              "172.16.10.1 [id = 105])")

        self.ip_addr1 = '172.16.10.1'
        self.ip_addr2 = '192.168.0.1'
        self.dmac1 = '00:11:22:33:44:55'
        self.dmac2 = '00:22:22:22:22:22'
        mask = '/24'
        self.table_stage_ingress = SAI_ACL_STAGE_INGRESS
        self.table_stage_egress = SAI_ACL_STAGE_EGRESS

        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress(self.ip_addr1))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry1,
            dst_mac_address=self.dmac1)

        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr1),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.ip_addr1 + mask))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry1,
            next_hop_id=self.nhop1)

        self.neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.port11_rif, ip_address=sai_ipaddress(self.ip_addr2))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry2,
            dst_mac_address=self.dmac2)

        self.nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr2),
            router_interface_id=self.port11_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.ip_addr2 + mask))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry2,
            next_hop_id=self.nhop2)

        self.pkt1 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                      eth_src=self.dmac2,
                                      ip_dst=self.ip_addr1,
                                      ip_src=self.ip_addr2,
                                      ip_id=105,
                                      ip_tos=0xc8,
                                      ip_ttl=64)
        self.exp_pkt1 = simple_tcp_packet(eth_dst=self.dmac1,
                                          eth_src=ROUTER_MAC,
                                          ip_dst=self.ip_addr1,
                                          ip_src=self.ip_addr2,
                                          ip_id=105,
                                          ip_tos=0xc8,
                                          ip_ttl=63)

        self.pkt2 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                      eth_src=self.dmac1,
                                      ip_dst=self.ip_addr2,
                                      ip_src=self.ip_addr1,
                                      ip_id=105,
                                      ip_tos=0xc8,
                                      ip_ttl=64)
        self.exp_pkt2 = simple_tcp_packet(eth_dst=self.dmac2,
                                          eth_src=ROUTER_MAC,
                                          ip_dst=self.ip_addr2,
                                          ip_src=self.ip_addr1,
                                          ip_id=105,
                                          ip_tos=0xc8,
                                          ip_ttl=63)

    def runTest(self):
        self.aclRoutingTest()
        self.aclIPFragmentTest(self.table_stage_ingress)
        # self.aclIPFragmentTest(self.table_stage_egress)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_route_entry(self.client, self.route_entry2)
        sai_thrift_remove_next_hop(self.client, self.nhop2)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry2)
        super(IPAclFragmentTest, self).tearDown()

    def aclRoutingTest(self):
        """
        Verify routing
        """
        # send the test packet(s)
        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac2, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2, ' | @ ptf_intf 2')
            send_packet(self, self.dev_port11, self.pkt1)
            print('#### Expecting ', self.dmac1, ' |', ROUTER_MAC, '| ',
                  self.ip_addr1, ' | ', self.ip_addr2, ' | @ ptf_intf 1')
            verify_packets(self, self.exp_pkt1, [self.dev_port10])

            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac1, ' | ',
                  self.ip_addr2, ' | ', self.ip_addr1, ' | @ ptf_intf 2')
            send_packet(self, self.dev_port10, self.pkt2)
            print('#### Expecting ', self.dmac2, ' |', ROUTER_MAC, '| ',
                  self.ip_addr2, ' | ', self.ip_addr1, ' | @ ptf_intf 1')
            verify_packets(self, self.exp_pkt2, [self.dev_port11])
        finally:
            print('----------------------------------------------------------')

    def aclIPFragmentTest(self, table_stage):
        """
        Verify ACL with IP frgamentation
        Args:
            table_stage (int): specifies ingress or egress type of ACL
        """
        # setup ACL to block based on Source IP
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list), int32list=table_bind_point_list)

        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_acl_ip_frag=True)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_ip_frag = sai_thrift_acl_field_data_t(
            sai_thrift_acl_field_data_data_t(
                s32=SAI_ACL_IP_FRAG_ANY))

        # Add drop ACL entry to IPv6 ACL Table
        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            priority=entry_priority,
            table_id=acl_table_id,
            action_packet_action=packet_action,
            field_acl_ip_frag=acl_ip_frag)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id)

        # attach ACL counter to ACL entry
        action_counter = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter)

        try:
            if table_stage == SAI_ACL_STAGE_INGRESS:
                sai_thrift_set_port_attribute(
                    self.client, self.port11, ingress_acl=acl_table_id)
                sport = self.dev_port11
                dport = self.dev_port10
                pkt = self.pkt1
                exp_pkt = self.exp_pkt1
                ip_addr1 = self.ip_addr1
                ip_addr2 = self.ip_addr2
                dmac = self.dmac1
                smac = self.dmac2
            elif table_stage == SAI_ACL_STAGE_EGRESS:
                sai_thrift_set_port_attribute(
                    self.client, self.port11, egress_acl=acl_table_id)
                sport = self.dev_port10
                dport = self.dev_port11
                pkt = self.pkt2
                exp_pkt = self.exp_pkt2
                ip_addr1 = self.ip_addr2
                ip_addr2 = self.ip_addr1
                dmac = self.dmac2
                smac = self.dmac1

            self.assertNotEqual(acl_table_id, 0)
            self.assertNotEqual(acl_entry_id, 0)

            print('#### ACL Applied, but non frag ####')
            print('#### Sending  ', ROUTER_MAC, ' | ', smac, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 2')
            send_packet(self, sport, pkt)
            print('#### Expecting ', dmac, ' | ', ROUTER_MAC, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 1')
            verify_packets(self, exp_pkt, [dport])
            print('#### ACL no Drop, DF=1, offset = 0, Applied ####')
            print('#### Sending      ', ROUTER_MAC, ' | ', smac, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 2')
            # send the same packet
            pkt['IP'].flags = 2
            exp_pkt['IP'].flags = 2
            pkt['IP'].frag = 0
            send_packet(self, sport, pkt)
            print('#### Expecting ', dmac, ' | ', ROUTER_MAC, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 1')
            verify_packets(self, exp_pkt, [dport])
            exp_pkt['IP'].flags = 0
            print('#### ACL Drop, MF=1, offset = 0, '
                  'first fragment, Applied ####')
            print('#### Sending      ', ROUTER_MAC, ' | ', smac, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 2')
            # send the same packet
            pkt['IP'].flags = 1
            pkt['IP'].frag = 0
            send_packet(self, sport, pkt)
            print('#### NOT Expecting ', dmac, ' |', ROUTER_MAC, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
            print('#### ACL Drop, MF=1, offset = 20, '
                  'non head fragment, Applied ####')
            print('#### Sending      ', ROUTER_MAC, ' | ', smac, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 2')
            # send the same packet
            pkt['IP'].flags = 1
            pkt['IP'].frag = 20
            send_packet(self, sport, pkt)
            print('#### NOT Expecting ', dmac, ' | ', ROUTER_MAC, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)
            print('#### ACL Drop, MF=0, offset = 20, last fragment,'
                  ' Applied ####')
            print('#### Sending      ', ROUTER_MAC, ' | ', smac, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 2')
            # send the same packet
            pkt['IP'].flags = 0
            pkt['IP'].frag = 20
            send_packet(self, sport, pkt)
            print('#### NOT Expecting ', dmac, ' | ', ROUTER_MAC, ' | ',
                  ip_addr1, ' | ', ip_addr2, ' | @ ptf_intf 1')
            verify_no_other_packets(self, timeout=2)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 3)

        finally:
            # cleanup ACL
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_id,
                action_counter=action_counter)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter)

            # unbind this ACL table from ports object id
            if table_stage == SAI_ACL_STAGE_INGRESS:
                sai_thrift_set_port_attribute(
                    self.client, self.port11, ingress_acl=0)
            elif table_stage == SAI_ACL_STAGE_EGRESS:
                sai_thrift_set_port_attribute(
                    self.client, self.port11, egress_acl=0)

            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)


@group('acl')
class L3AclCounterTest(SaiHelper):
    """
    Verify ACL counter test case
    """
    def setUp(self):
        super(L3AclCounterTest, self).setUp()

        l4_src_port = 1000
        mask = '/24'
        self.ip_addr1 = '172.16.10.1'
        self.dmac1 = '00:11:22:33:44:55'
        self.ip_addr2 = '192.168.100.100'
        self.dmac2 = '00:22:22:22:22:22'

        self.table_stage_ingress = SAI_ACL_STAGE_INGRESS
        self.table_stage_egress = SAI_ACL_STAGE_EGRESS

        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress(self.ip_addr1))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry1,
            dst_mac_address=self.dmac1)

        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr1),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.ip_addr1 + mask))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry1,
            next_hop_id=self.nhop1)

        self.neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.port11_rif, ip_address=sai_ipaddress(self.ip_addr2))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry2,
            dst_mac_address=self.dmac2)

        self.nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr2),
            router_interface_id=self.port11_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.ip_addr2 + mask))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry2,
            next_hop_id=self.nhop2)

        self.pkt1 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                      eth_src=self.dmac2,
                                      ip_dst=self.ip_addr1,
                                      ip_src=self.ip_addr2,
                                      tcp_sport=l4_src_port,
                                      ip_id=105,
                                      ip_ttl=64)
        self.exp_pkt1 = simple_tcp_packet(eth_dst=self.dmac1,
                                          eth_src=ROUTER_MAC,
                                          ip_dst=self.ip_addr1,
                                          ip_src=self.ip_addr2,
                                          tcp_sport=l4_src_port,
                                          ip_id=105,
                                          ip_ttl=63)

        self.pkt2 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                      eth_src=self.dmac1,
                                      ip_dst=self.ip_addr2,
                                      ip_src=self.ip_addr1,
                                      tcp_sport=l4_src_port,
                                      ip_id=105,
                                      ip_ttl=64)
        self.exp_pkt2 = simple_tcp_packet(eth_dst=self.dmac2,
                                          eth_src=ROUTER_MAC,
                                          ip_dst=self.ip_addr2,
                                          ip_src=self.ip_addr1,
                                          tcp_sport=l4_src_port,
                                          ip_id=105,
                                          ip_ttl=63)

    def runTest(self):
        self.aclRoutingTest()
        self.l3AclCounterTest(self.table_stage_ingress)
        self.l3AclCounterTest(self.table_stage_egress)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_route_entry(self.client, self.route_entry2)
        sai_thrift_remove_next_hop(self.client, self.nhop2)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry2)
        super(L3AclCounterTest, self).tearDown()

    def aclRoutingTest(self):
        """
        Verify ACL configuration
        """
        print('--------------------------------------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (", self.ip_addr2,
              " ---> ", self.ip_addr1, " [id = 105])")
        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, ' | ', self.dmac2, ' | ',
                  self.ip_addr2, ' | ', self.ip_addr1, ' | SPORT 1000 | '
                  '@ ptf_intf 2')
            send_packet(self, self.dev_port11, self.pkt1)
            print('#### Expecting ', self.dmac1, ' | ', ROUTER_MAC, ' | ',
                  self.ip_addr2, ' | ', self.ip_addr1, ' | SPORT 1000 | '
                  '@ ptf_intf 1')
            verify_packets(self, self.exp_pkt1, [self.dev_port10])

            print('----------------------------------------------------------')
            print("Sending packet ptf_intf 2 -> ptf_intf 1 (", self.ip_addr1,
                  " ---> ", self.ip_addr2, " [id = 105])")
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac1, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2, ' | SPORT 1000 | '
                  '@ ptf_intf 2')
            send_packet(self, self.dev_port10, self.pkt2)
            print('#### Expecting ', self.dmac2, ' |', ROUTER_MAC, '| ',
                  self.ip_addr1, ' | ', self.ip_addr2, ' | SPORT 1000 | '
                  '@ ptf_intf 1')
            verify_packets(self, self.exp_pkt2, [self.dev_port11])
        finally:
            print('----------------------------------------------------------')

    def l3AclCounterTest(self, table_stage):
        """
        Verify ACL with action counter
        Args:
            table_stage (int): specifies the type of ACL table stage
        """
        print("Testing L3AclCounterTest")

        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        ip_src_mask = "255.255.255.0"

        if table_stage == SAI_ACL_STAGE_INGRESS:
            ip_src = "192.168.100.1"
            ip_src_addr = "192.168.100.100"
            ip_dst = self.ip_addr1
            ip_dst_addr = "172.16.10.1"
            dmac = self.dmac1
            smac = self.dmac2
        elif table_stage == SAI_ACL_STAGE_EGRESS:
            ip_src = "172.16.10.1"
            ip_src_addr = "172.16.10.1"
            ip_dst = "192.168.100.1"
            ip_dst_addr = "192.168.100.100"
            dmac = self.dmac2
            smac = self.dmac1

        print("Sending packet ptf_intf 2 -[ACL]-> ptf_intf 1 "
              "(", ip_src, "-[ACL]-> ", ip_dst, " [id = 105])")

        table_bind_points = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_points), int32list=table_bind_points)

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_src_mask))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=entry_priority,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        # bind this ACL table to rif_id2s object id
        if table_stage == SAI_ACL_STAGE_INGRESS:
            sai_thrift_set_router_interface_attribute(
                self.client, self.port11_rif, ingress_acl=acl_table_id)
            pkt = self.pkt1
            sport = self.dev_port11
        elif table_stage == SAI_ACL_STAGE_EGRESS:
            sai_thrift_set_router_interface_attribute(
                self.client, self.port11_rif, egress_acl=acl_table_id)
            pkt = self.pkt2
            sport = self.dev_port10

        # create ACL counter and bind it to the ACL entry
        acl_counter_id = sai_thrift_create_acl_counter(
            client=self.client, table_id=acl_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter_id),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter_t)

        try:
            self.assertNotEqual(acl_table_id, 0)
            self.assertNotEqual(acl_entry_id, 0)
            self.assertNotEqual(acl_counter_id, 0)
            pkt_cnt = 5

            attr_values = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_id, packets=True, bytes=True)

            initial_pkts_cnt = attr_values["packets"]
            initial_bytes_cnt = attr_values["bytes"]

            print('#### ACL \'DROP, src ip ', ip_src, '/', ip_src_mask, ', '
                  'SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| ', smac, ' | ', ip_src_addr,
                  ' | ', ip_dst_addr, ' | SPORT 1000 | @ ptf_intf 1')
            # send the same packet
            for i in range(0, pkt_cnt):
                print(i, pkt_cnt)
                send_packet(self, sport, pkt)
            # ensure packets are dropped
            # check for absence of packets here!
            print('#### NOT Expecting ', dmac, ' | ', ROUTER_MAC, ' | ',
                  ip_src_addr, ' | ', ip_dst_addr, ' | SPORT 1000 | '
                  '@ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)

            time.sleep(2)

            attr_values = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_id, packets=True, bytes=True)

            actual_pkts_cnt = (attr_values["packets"] - initial_pkts_cnt)
            print(actual_pkts_cnt)
            actual_bytes_cnt = (attr_values["bytes"] - initial_bytes_cnt)
            print(actual_bytes_cnt)

            self.assertEqual(actual_pkts_cnt, pkt_cnt, "packets counter value "
                             "actual_pkts_cnt is not pkt_cnt")

        finally:
            # unbind this ACL table from rif_id object id
            if table_stage == SAI_ACL_STAGE_INGRESS:
                sai_thrift_set_router_interface_attribute(
                    self.client, self.port11_rif, ingress_acl=0)
            elif table_stage == SAI_ACL_STAGE_EGRESS:
                sai_thrift_set_router_interface_attribute(
                    self.client, self.port11_rif, egress_acl=0)
            # cleanup ACL
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)
            sai_thrift_remove_acl_counter(self.client, acl_counter_id)


@group('acl-outer-vlan')
class OuterVlanIdAclTest(SaiHelper):
    """
    Verify ACL outer vlan id test case
    """

    def setUp(self):
        super(OuterVlanIdAclTest, self).setUp()
        print("Sending L2 packet - port 24 <-> port 25 [trunk vlan=101])")
        vlan_id = 101
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac3 = '00:33:33:33:33:33'
        mac_action = SAI_PACKET_ACTION_FORWARD
        self.table_stage_ingress = SAI_ACL_STAGE_INGRESS
        self.group_stage_ingress = SAI_ACL_STAGE_INGRESS

        self.vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)

        self.port24_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.port25_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port25,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port25_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.port26_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port26,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port26_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.fdb_entry1 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address=mac1, bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry1,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port24_bp,
            packet_action=mac_action)

        self.fdb_entry2 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address=mac2, bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry2,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port25_bp,
            packet_action=mac_action)

        self.fdb_entry3 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address=mac3, bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry3,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port26_bp,
            packet_action=mac_action)

        self.pkt1 = simple_tcp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            dl_vlan_enable=True,
            vlan_vid=101,
            ip_src='192.168.100.1',
            ip_dst='172.16.0.1',
            ip_id=102,
            ip_ttl=64)

        self.pkt2 = simple_tcp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:33:33:33:33:33',
            dl_vlan_enable=True,
            vlan_vid=101,
            ip_src='192.168.100.1',
            ip_dst='172.16.0.1',
            ip_id=102,
            ip_ttl=64)

    def runTest(self):
        self.aclOuterVlanTest(
            self.table_stage_ingress, self.group_stage_ingress)

    def tearDown(self):
        sai_thrift_remove_fdb_entry(
            self.client, self.fdb_entry1)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry2)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry3)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member1)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member2)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member3)
        sai_thrift_remove_bridge_port(self.client, self.port24_bp)
        sai_thrift_remove_bridge_port(self.client, self.port25_bp)
        sai_thrift_remove_bridge_port(self.client, self.port26_bp)
        sai_thrift_remove_vlan(self.client, self.vlan_oid)
        super(OuterVlanIdAclTest, self).tearDown()

    def aclOuterVlanTest(self, table_stage, group_stage):
        """
        Verify Outer Vlan ID Test
        Args:
            table_stage (int): specifies the type of ACL table stage
            group_stage (int): specifies the type of ACL group stage
        """
        table_bind_point_list = [
            SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = 1

        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL
        group_member_priority = 100

        acl_action = SAI_PACKET_ACTION_DROP
        outer_vlan_id = 101

        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_point_list),
            int32list=group_bind_point_list)

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=acl_action))

        outer_vlan_id = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=101),
            mask=sai_thrift_acl_field_data_mask_t(u16=4095))

        acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=group_stage,
            acl_bind_point_type_list=group_bind_point_type_list,
            type=group_type)

        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_outer_vlan_id=101)

        acl_table_group_member_id = sai_thrift_create_acl_table_group_member(
            self.client,
            acl_table_group_id=acl_table_group_id,
            acl_table_id=acl_table_id,
            priority=group_member_priority)

        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=entry_priority,
            action_packet_action=packet_action,
            field_outer_vlan_id=outer_vlan_id)

        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id)

        # attach ACL counter to ACL entry
        action_counter = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter)

        # bind this ACL table group to ports
        sai_thrift_set_port_attribute(
            self.client, self.port24, ingress_acl=acl_table_group_id)
        sai_thrift_set_port_attribute(
            self.client, self.port25, ingress_acl=acl_table_group_id)
        sai_thrift_set_port_attribute(
            self.client, self.port26, ingress_acl=acl_table_group_id)

        try:
            # Send VLAN tagged packet
            send_packet(
                self, self.dev_port24, self.pkt1)
            verify_no_other_packets(self, timeout=1)
            # Send VLAN untagged packet
            send_packet(self, self.dev_port25, self.pkt2)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            # cleanup ACL
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_id,
                action_counter=action_counter)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(
                self.client, acl_counter)

            sai_thrift_set_port_attribute(
                self.client, self.port24, ingress_acl=0)
            sai_thrift_set_port_attribute(
                self.client, self.port25, ingress_acl=0)
            sai_thrift_set_port_attribute(
                self.client, self.port26, ingress_acl=0)

            sai_thrift_remove_acl_table_group_member(
                self.client, acl_table_group_member_id)
            sai_thrift_remove_acl_entry(
                self.client, acl_entry_id)
            sai_thrift_remove_acl_table(
                self.client, acl_table_id)
            sai_thrift_remove_acl_table_group(
                self.client, acl_table_group_id)


@group('acl-outer-vlan')
class OuterVlanIdAclTestEgress(SaiHelper):
    """
    Verify matching on outer vlan Id egress L3 routing case
    """
    def setUp(self):
        super(OuterVlanIdAclTestEgress, self).setUp()

        self.vlan100_rif_counter_in = 0
        self.vlan200_rif_counter_out = 0

        self.port24_bridgeport = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.port25_bridgeport = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port25,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.port26_bridgeport = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port26,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        # vlan100 with port members 24, 25 and 26
        self.vlan100 = sai_thrift_create_vlan(self.client, vlan_id=100)
        self.vlan_member1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan100,
            bridge_port_id=self.port24_bridgeport,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_member2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan100,
            bridge_port_id=self.port25_bridgeport,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_member3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan100,
            bridge_port_id=self.port26_bridgeport,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(
            self.client, self.port24, port_vlan_id=100)
        sai_thrift_set_port_attribute(
            self.client, self.port25, port_vlan_id=100)
        sai_thrift_set_port_attribute(
            self.client, self.port26, port_vlan_id=100)

        # create vlan100_rif
        self.vlan100_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            virtual_router_id=self.default_vrf,
            vlan_id=self.vlan100)

        self.dmac1 = '00:11:22:33:44:55'  # 10.10.10.1
        self.dmac2 = '00:22:22:33:44:55'  # 10.10.10.2
        self.dmac3 = '00:33:22:33:44:55'  # 10.10.10.3
        self.dmac4 = '00:44:22:33:44:55'  # 11.11.11.1
        self.dmac5 = '00:11:33:33:44:55'  # 20.10.10.1
        self.dmac6 = '00:22:33:33:44:55'  # 20.10.10.2
        self.dmac7 = '00:44:33:33:44:55'  # 20.11.11.1

        # create nexthop1, nexthop2 & nexthop3 on SVI
        self.nexthop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.0.1'),
            router_interface_id=self.vlan100_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan100_rif, ip_address=sai_ipaddress('10.10.0.1'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry1, dst_mac_address=self.dmac1)
        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry1, next_hop_id=self.nexthop1)
        self.route_entry1_v6 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:0000/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry1_v6, next_hop_id=self.nexthop1)

        self.neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan100_rif, ip_address=sai_ipaddress('10.10.0.2'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry2, dst_mac_address=self.dmac2)
        self.nexthop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.0.2'),
            router_interface_id=self.vlan100_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.2/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry2, next_hop_id=self.nexthop2)
        self.route_entry2_v6 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:2222/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry2_v6, next_hop_id=self.nexthop2)

        self.nexthop3 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.0.3'),
            router_interface_id=self.vlan100_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry3 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan100_rif, ip_address=sai_ipaddress('10.10.0.3'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry3, dst_mac_address=self.dmac3)
        self.route_entry3 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.3/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry3, next_hop_id=self.nexthop3)

        # create nexthop4 and route to L2 intf
        self.nexthop4 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('11.11.0.2'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry4 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('11.11.0.2'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry4, dst_mac_address=self.dmac4)
        self.route_entry4 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('11.11.11.1/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry4, next_hop_id=self.nexthop4)
        self.route_entry4_v6 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:1111/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry4_v6, next_hop_id=self.nexthop4)

        # create LAG10 and LAG11
        self.lag10 = sai_thrift_create_lag(self.client)
        self.lag10_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.lag10,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.lag10_member1 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag10, port_id=self.port30)
        self.lag10_member2 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag10, port_id=self.port31)

        self.lag11 = sai_thrift_create_lag(self.client)
        self.lag11_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.lag11,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.lag11_member1 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag11, port_id=self.port28)
        self.lag11_member2 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag11, port_id=self.port29)

        # vlan200 with members LAG10(UNTAGGED) and LAG11(TAGGED)
        self.vlan200 = sai_thrift_create_vlan(self.client, vlan_id=200)
        self.vlan_member4 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan200,
            bridge_port_id=self.lag10_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_member5 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan200,
            bridge_port_id=self.lag11_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)
        sai_thrift_set_lag_attribute(self.client, self.lag10, port_vlan_id=200)
        sai_thrift_set_lag_attribute(self.client, self.lag11, port_vlan_id=200)

        self.vlan200_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            virtual_router_id=self.default_vrf,
            vlan_id=self.vlan200)

        # Create nexthop5 and nexthop6 on SVI
        self.nexthop5 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.10.0.1'),
            router_interface_id=self.vlan200_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry5 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan200_rif, ip_address=sai_ipaddress('20.10.0.1'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry5, dst_mac_address=self.dmac5)
        self.route_entry5 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('20.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry5, next_hop_id=self.nexthop5)
        self.route_entry5_v6 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:99aa/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry5_v6, next_hop_id=self.nexthop5)

        self.nexthop6 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.10.0.2'),
            router_interface_id=self.vlan200_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry6 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan200_rif, ip_address=sai_ipaddress('20.10.0.2'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry6, dst_mac_address=self.dmac6)
        self.route_entry6 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('20.10.10.2/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry6, next_hop_id=self.nexthop6)
        self.route_entry6_v6 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:1122:3344:5566:7788/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry6_v6, next_hop_id=self.nexthop6)

        # create nexthop7 and route to L2 intf
        self.nexthop7 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('21.11.0.2'),
            router_interface_id=self.port11_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry7 = sai_thrift_neighbor_entry_t(
            rif_id=self.port11_rif, ip_address=sai_ipaddress('21.11.0.2'))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry7, dst_mac_address=self.dmac7)
        self.route_entry7 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('21.11.11.1/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry7, next_hop_id=self.nexthop7)
        self.route_entry7_v6 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:1122:3344:5566:6677/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry7_v6, next_hop_id=self.nexthop7)

    def runTest(self):
        group_stage_egress = SAI_ACL_STAGE_EGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL
        table_stage_egress = SAI_ACL_STAGE_EGRESS

        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_point_list), int32list=group_bind_point_list)

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list), int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        outer_vlan_id_200 = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=200),
            mask=sai_thrift_acl_field_data_mask_t(u16=4095))

        # create Egress ACL Group
        print("Create Egress ACL tables group")
        acl_group_egress = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=group_stage_egress,
            acl_bind_point_type_list=group_bind_point_type_list,
            type=group_type)

        # create Egress IPv4 ACL table
        print("Create Egress IPv4 ACL table")
        acl_egress_ipv4_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_egress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        # create Egress ACL table group member
        print("Create Egress ACL group member")
        acl_group_egress_ipv4_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=acl_group_egress,
                acl_table_id=acl_egress_ipv4_table_id)

        # create Egress IPv4 ACL entry
        print("Create Egress IPv4 ACL entry")
        ipv4_acl_egress_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_egress_ipv4_table_id,
            action_packet_action=packet_action,
            field_outer_vlan_id=outer_vlan_id_200)
        # create IPv4 ACL counter
        acl_counter_egress_ipv4 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_egress_ipv4_table_id)
        # attach IPv4 ACL counter to ACL entry
        action_counter_egress_ipv4 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_egress_ipv4), enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, ipv4_acl_egress_entry_id,
            action_counter=action_counter_egress_ipv4)

        mac_action = SAI_PACKET_ACTION_FORWARD
        port27_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port27,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        vlan_member6 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan100,
            bridge_port_id=port27_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        fdb_entry27 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                             mac_address=self.dmac2,
                                             bv_id=self.vlan100)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry27,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=port27_bp,
                                    packet_action=mac_action)

        # Create FDB entries for LAG10
        fdb_entry_lag10 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                 mac_address=self.dmac5,
                                                 bv_id=self.vlan200)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry_lag10,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.lag10_bp,
                                    packet_action=mac_action)

        # Create FDB entries for LAG11
        fdb_entry_lag11 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                 mac_address=self.dmac6,
                                                 bv_id=self.vlan200)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry_lag11,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.lag11_bp,
                                    packet_action=mac_action)

        tagged_pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                       eth_src=self.dmac2,
                                       ip_dst='20.10.10.1',
                                       ip_src='192.168.0.1',
                                       dl_vlan_enable=True,
                                       vlan_vid=100,
                                       ip_ttl=64,
                                       pktlen=104)
        tagged_pkt_tolag11 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                               eth_src=self.dmac2,
                                               ip_dst='20.10.10.2',
                                               ip_src='192.168.0.1',
                                               dl_vlan_enable=True,
                                               vlan_vid=100,
                                               ip_ttl=64,
                                               pktlen=104)
        exp_pkt = simple_tcp_packet(eth_dst=self.dmac5,
                                    eth_src=ROUTER_MAC,
                                    ip_dst='20.10.10.1',
                                    ip_src='192.168.0.1',
                                    ip_ttl=63)
        tagged_exp_pkt_tolag11 = simple_tcp_packet(eth_dst=self.dmac6,
                                                   eth_src=ROUTER_MAC,
                                                   ip_dst='20.10.10.2',
                                                   ip_src='192.168.0.1',
                                                   dl_vlan_enable=True,
                                                   vlan_vid=200,
                                                   ip_ttl=63,
                                                   pktlen=104)

        try:

            print("Sending packet port %d to LAG, (192.168.0.1 -> 20.10.10.1)"
                  % (self.dev_port27))
            send_packet(self, self.dev_port27, tagged_pkt)
            verify_packet_any_port(self, exp_pkt,
                                   [self.dev_port30, self.dev_port31])
            self.vlan100_rif_counter_in += 1
            self.vlan200_rif_counter_out += 1

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 0)

            # bind ACL group to LAG10 and verify ACL work
            sai_thrift_set_lag_attribute(
                self.client, self.lag10, egress_acl=acl_group_egress)
            send_packet(self, self.dev_port27, tagged_pkt)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Sending packet port %d to LAG, (192.168.0.1 -> 20.10.10.2)"
                  % (self.dev_port27))
            send_packet(self, self.dev_port27, tagged_pkt_tolag11)
            verify_packet_any_port(self, tagged_exp_pkt_tolag11,
                                   [self.dev_port28, self.dev_port29])

            # bind ACL group to LAG11 and verify ACL work
            sai_thrift_set_lag_attribute(
                self.client, self.lag11, egress_acl=acl_group_egress)
            send_packet(self, self.dev_port27, tagged_pkt_tolag11)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 2)

        finally:
            action_counter_egress_ipv4 = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, ipv4_acl_egress_entry_id,
                action_counter=action_counter_egress_ipv4)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress_ipv4, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress_ipv4)

            sai_thrift_remove_acl_table_group_member(
                self.client, acl_group_egress_ipv4_member_id)
            sai_thrift_remove_acl_table_group(self.client, acl_group_egress)
            sai_thrift_remove_acl_entry(self.client, ipv4_acl_egress_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_egress_ipv4_table_id)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry_lag10)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry_lag11)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry27)
            sai_thrift_remove_vlan_member(self.client, vlan_member6)
            sai_thrift_remove_bridge_port(self.client, port27_bp)
            sai_thrift_flush_fdb_entries(self.client)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry5)
        sai_thrift_remove_route_entry(self.client, self.route_entry5_v6)
        sai_thrift_remove_route_entry(self.client, self.route_entry6)
        sai_thrift_remove_route_entry(self.client, self.route_entry6_v6)
        sai_thrift_remove_route_entry(self.client, self.route_entry7)
        sai_thrift_remove_route_entry(self.client, self.route_entry7_v6)

        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry5)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry6)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry7)

        sai_thrift_remove_next_hop(self.client, self.nexthop5)
        sai_thrift_remove_next_hop(self.client, self.nexthop6)
        sai_thrift_remove_next_hop(self.client, self.nexthop7)

        sai_thrift_set_lag_attribute(self.client, self.lag10, port_vlan_id=0)
        sai_thrift_set_lag_attribute(self.client, self.lag11, port_vlan_id=0)

        sai_thrift_remove_router_interface(self.client, self.vlan200_rif)

        sai_thrift_remove_vlan_member(self.client, self.vlan_member4)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member5)

        sai_thrift_remove_vlan(self.client, self.vlan200)

        sai_thrift_remove_lag_member(self.client, self.lag10_member1)
        sai_thrift_remove_lag_member(self.client, self.lag10_member2)
        sai_thrift_remove_lag_member(self.client, self.lag11_member1)
        sai_thrift_remove_lag_member(self.client, self.lag11_member2)
        sai_thrift_remove_bridge_port(self.client, self.lag10_bp)
        sai_thrift_remove_bridge_port(self.client, self.lag11_bp)
        sai_thrift_remove_lag(self.client, self.lag10)
        sai_thrift_remove_lag(self.client, self.lag11)

        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_route_entry(self.client, self.route_entry1_v6)
        sai_thrift_remove_route_entry(self.client, self.route_entry2)
        sai_thrift_remove_route_entry(self.client, self.route_entry2_v6)
        sai_thrift_remove_route_entry(self.client, self.route_entry3)
        sai_thrift_remove_route_entry(self.client, self.route_entry4)
        sai_thrift_remove_route_entry(self.client, self.route_entry4_v6)

        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry2)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry3)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry4)

        sai_thrift_remove_next_hop(self.client, self.nexthop1)
        sai_thrift_remove_next_hop(self.client, self.nexthop2)
        sai_thrift_remove_next_hop(self.client, self.nexthop3)
        sai_thrift_remove_next_hop(self.client, self.nexthop4)

        sai_thrift_remove_router_interface(self.client, self.vlan100_rif)

        sai_thrift_set_port_attribute(self.client, self.port24, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port25, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port26, port_vlan_id=0)

        sai_thrift_remove_vlan_member(self.client, self.vlan_member1)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member2)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member3)

        sai_thrift_remove_vlan(self.client, self.vlan100)

        sai_thrift_remove_bridge_port(self.client, self.port24_bridgeport)
        sai_thrift_remove_bridge_port(self.client, self.port25_bridgeport)
        sai_thrift_remove_bridge_port(self.client, self.port26_bridgeport)
        super(OuterVlanIdAclTestEgress, self).tearDown()


@group('acl-dscp-mirror')
class DscpMirrorAclTest(SaiHelper):
    """This test tries to verify ACL groups with IP Mirror ACLs and
       DSCP mirror ACL.
       Acl-group/table of bp_type PORT
       Have 2 acl tables of type [mirror, tos_mirror]
       from seperate label space"""

    def setUp(self):
        super(DscpMirrorAclTest, self).setUp()
        print('--------------------------------------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.0.1 ---> "
              "172.16.10.1)")

        self.ip_addr1 = '172.16.10.1'
        self.ip_addr2 = '192.168.0.1'
        self.ipv6_addr1 = '1234:5678:9abc:def0:4422:1133:5577:88aa'
        self.ipv6_addr2 = '2000::1'
        self.dmac1 = '00:11:22:33:44:55'
        self.dmac2 = '00:22:22:22:22:22'
        mask_v4 = '/24'
        mask_v6 = '/112'
        self.table_stage_ingress = SAI_ACL_STAGE_INGRESS
        self.table_stage_egress = SAI_ACL_STAGE_EGRESS

        self.neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress(self.ip_addr1))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry,
            dst_mac_address=self.dmac1)

        self.nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr1),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry_v4 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.ip_addr1 + mask_v4))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry_v4,
            next_hop_id=self.nhop)

        self.route_entry_v6 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.ipv6_addr1 + mask_v6))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry_v6,
            next_hop_id=self.nhop)

        self.pol_id = sai_thrift_create_policer(
            self.client,
            mode=SAI_POLICER_MODE_SR_TCM,
            meter_type=SAI_METER_TYPE_PACKETS,
            color_source=SAI_POLICER_COLOR_SOURCE_BLIND,
            cir=10,
            cbs=10)
        self.span_id = sai_thrift_create_mirror_session(
            self.client,
            monitor_port=self.port20,
            type=SAI_MIRROR_SESSION_TYPE_LOCAL,
            policer=self.pol_id)

        self.pkt_v4 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                        eth_src=self.dmac2,
                                        ip_dst=self.ip_addr1,
                                        ip_src=self.ip_addr2,
                                        ip_dscp=6,
                                        ip_ttl=64)
        self.exp_pkt_v4 = simple_tcp_packet(eth_dst=self.dmac1,
                                            eth_src=ROUTER_MAC,
                                            ip_dst=self.ip_addr1,
                                            ip_src=self.ip_addr2,
                                            ip_dscp=6,
                                            ip_ttl=63)

        self.pkt_v6 = simple_tcpv6_packet(eth_dst=ROUTER_MAC,
                                          eth_src=self.dmac2,
                                          ipv6_dst=self.ipv6_addr1,
                                          ipv6_src=self.ipv6_addr2,
                                          ipv6_dscp=6,
                                          ipv6_hlim=64)
        self.exp_pkt_v6 = simple_tcpv6_packet(eth_dst=self.dmac1,
                                              eth_src=ROUTER_MAC,
                                              ipv6_dst=self.ipv6_addr1,
                                              ipv6_src=self.ipv6_addr2,
                                              ipv6_dscp=6,
                                              ipv6_hlim=63)

    def runTest(self):
        try:
            self.aclRoutingTest()
            self.dscpMirrorTest(
                self.table_stage_ingress, self.pkt_v4, self.exp_pkt_v4)
            self.dscpMirrorTest(
                self.table_stage_ingress, self.pkt_v6, self.exp_pkt_v6)
            self.dscpMirrorTest(
                self.table_stage_egress, self.pkt_v4, self.exp_pkt_v4)
            self.dscpMirrorTest(
                self.table_stage_egress, self.pkt_v6, self.exp_pkt_v6)

        finally:
            pass

    def aclRoutingTest(self):
        """Tests routing"""
        # send the test packet(s)
        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac2, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            send_packet(self, self.dev_port11, self.pkt_v4)
            print('#### Expecting ', self.dmac1, ' |', ROUTER_MAC, '| ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            verify_packets(self, self.exp_pkt_v4, [self.dev_port10])

            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac2, ' | ',
                  self.ipv6_addr1, ' | ', self.ipv6_addr2)
            send_packet(self, self.dev_port11, self.pkt_v6)
            print('#### Expecting ', self.dmac1, ' |', ROUTER_MAC, '| ',
                  self.ipv6_addr1, ' | ', self.ipv6_addr2)
            verify_packets(self, self.exp_pkt_v6, [self.dev_port10])

        finally:
            print('----------------------------------------------------------')

    def dscpMirrorTest(self, table_stage, pkt, exp_pkt):
        """
        Tests  IP Mirror ACLs and DSCP mirror ACL.
        Firstly, send a packet with dscp_field=6.
        This packet will be matched on ACL rule from IP Mirror table.
        Secondly, send packet with dscp_field=10.
        This packet will be matched on ACL rule from TOS Mirror table.
        Args:
            table_stage (int): specifies ingress or egress type of ACL
            pkt (packet): IPv4 or IPv6 packet for sending
            exp_pkt (packet): IPv4 or IPv6 packet for receiving
        """

        pkt = copy.deepcopy(pkt)
        exp_pkt = copy.deepcopy(exp_pkt)

        # setup ACL block
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        ipv4_src_mask = "255.255.255.0"
        ipv6_src_mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:0"

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list), int32list=table_bind_point_list)

        mirror_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                objlist=sai_thrift_object_list_t(
                    count=len([self.span_id]),
                    idlist=[self.span_id])))

        if table_stage == SAI_ACL_STAGE_INGRESS:
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_INGRESS]
            action_ingress = mirror_action
            action_egress = None
        elif table_stage == SAI_ACL_STAGE_EGRESS:
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_EGRESS]
            action_ingress = None
            action_egress = mirror_action

        action_type_list = sai_thrift_s32_list_t(
            count=len(actions), int32list=actions)

        acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=SAI_ACL_TABLE_GROUP_TYPE_PARALLEL)

        # setup ACL tos_mirror table
        acl_table_id1 = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            acl_action_type_list=action_type_list,
            field_dscp=True)

        acl_counter1 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id1)

        action_counter1 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter1),
            enable=True)

        acl_ip_dscp = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=6),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        acl_entry_id1 = sai_thrift_create_acl_entry(
            self.client,
            priority=entry_priority,
            table_id=acl_table_id1,
            action_counter=action_counter1,
            action_mirror_ingress=action_ingress,
            action_mirror_egress=action_egress,
            field_dscp=acl_ip_dscp)

        # setup ACL ip_mirror table
        acl_table_id2 = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            acl_action_type_list=action_type_list,
            field_src_ip=(pkt == self.pkt_v4),
            field_src_ipv6=(pkt == self.pkt_v6),
            field_dscp=True)

        acl_counter2 = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id2)

        action_counter2 = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter2),
            enable=True)

        acl_ip_dscp = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=10),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        # set src_ip acl field relate to packet type
        if pkt == self.pkt_v4:
            src_ipv4_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=self.ip_addr2),
                mask=sai_thrift_acl_field_data_mask_t(ip4=ipv4_src_mask))
            src_ipv6_t = None
        elif pkt == self.pkt_v6:
            src_ipv4_t = None
            src_ipv6_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=self.ipv6_addr2),
                mask=sai_thrift_acl_field_data_mask_t(ip6=ipv6_src_mask))

        acl_entry_id2 = sai_thrift_create_acl_entry(
            self.client,
            priority=entry_priority,
            table_id=acl_table_id2,
            action_counter=action_counter2,
            action_mirror_ingress=action_ingress,
            action_mirror_egress=action_egress,
            field_src_ip=src_ipv4_t,
            field_src_ipv6=src_ipv6_t,
            field_dscp=acl_ip_dscp)

        acl_table_group_member_id1 = sai_thrift_create_acl_table_group_member(
            self.client,
            acl_table_group_id=acl_table_group_id,
            acl_table_id=acl_table_id1,
            priority=100)

        acl_table_group_member_id2 = sai_thrift_create_acl_table_group_member(
            self.client,
            acl_table_group_id=acl_table_group_id,
            acl_table_id=acl_table_id2,
            priority=100)

        if table_stage == SAI_ACL_STAGE_INGRESS:
            receive_ports = [self.dev_port10]
            sai_thrift_set_port_attribute(
                self.client, self.port11, ingress_acl=acl_table_group_id)
        elif table_stage == SAI_ACL_STAGE_EGRESS:
            receive_ports = [self.dev_port10, self.dev_port20]
            sai_thrift_set_port_attribute(
                self.client, self.port10, egress_acl=acl_table_group_id)

        try:
            print('#### ACL Applied, match DSCP_MIRROR acl table ####')
            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac2, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            send_packet(self, self.dev_port11, pkt)

            print('#### Expecting ', self.dmac1, ' |', ROUTER_MAC, '| ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            verify_packets(self, exp_pkt, receive_ports)

            # dscp field 6, will be match tos_marror table
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter1, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter2, packets=True)
            self.assertEqual(packets['packets'], 0)

            print('#### ACL Applied, match IP_MIRROR acl table ####')
            # change ip_dscp fild in pkt (6 -> 10)
            new_tos = ip_make_tos(0, 0, 10)
            if pkt == self.pkt_v4:
                pkt['IP'].tos = new_tos
                exp_pkt['IP'].tos = new_tos
            elif pkt == self.pkt_v6:
                pkt['IPv6'].tc = new_tos
                exp_pkt['IPv6'].tc = new_tos

            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac2, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            send_packet(self, self.dev_port11, pkt)

            print('#### Expecting ', self.dmac1, ' |', ROUTER_MAC, '| ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            verify_packets(self, exp_pkt, receive_ports)

            # dscp field 10, match ip_mirror table
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter1, packets=True)
            self.assertEqual(packets['packets'], 1)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter2, packets=True)
            self.assertEqual(packets['packets'], 1)

            stats = sai_thrift_get_policer_stats(self.client, self.pol_id)
            print("Policer statistics")
            print('SAI_POLICER_STAT_PACKETS:',
                  stats['SAI_POLICER_STAT_PACKETS'])
            print('SAI_POLICER_STAT_YELLOW_PACKETS:',
                  stats['SAI_POLICER_STAT_YELLOW_PACKETS'])
            print('SAI_POLICER_STAT_GREEN_PACKETS:',
                  stats['SAI_POLICER_STAT_GREEN_PACKETS'])
            print('SAI_POLICER_STAT_RED_PACKETS:',
                  stats['SAI_POLICER_STAT_RED_PACKETS'])

        finally:
            sai_thrift_clear_policer_stats(self.client, self.pol_id)
            stats = sai_thrift_get_policer_stats(self.client, self.pol_id)
            print("Policer statistics after clear")
            print('SAI_POLICER_STAT_PACKETS:',
                  stats['SAI_POLICER_STAT_PACKETS'])
            print('SAI_POLICER_STAT_YELLOW_PACKETS:',
                  stats['SAI_POLICER_STAT_YELLOW_PACKETS'])
            print('SAI_POLICER_STAT_GREEN_PACKETS:',
                  stats['SAI_POLICER_STAT_GREEN_PACKETS'])
            print('SAI_POLICER_STAT_RED_PACKETS:',
                  stats['SAI_POLICER_STAT_RED_PACKETS'])

            self.assertEqual(stats['SAI_POLICER_STAT_PACKETS'], 0)
            self.assertEqual(stats['SAI_POLICER_STAT_YELLOW_PACKETS'], 0)
            self.assertEqual(stats['SAI_POLICER_STAT_GREEN_PACKETS'], 0)
            self.assertEqual(stats['SAI_POLICER_STAT_RED_PACKETS'], 0)

            # unbind this ACL table from ports object id
            if table_stage == SAI_ACL_STAGE_INGRESS:
                sai_thrift_set_port_attribute(
                    self.client, self.port11, ingress_acl=0)
            elif table_stage == SAI_ACL_STAGE_EGRESS:
                sai_thrift_set_port_attribute(
                    self.client, self.port10, egress_acl=0)

            # cleanup ACL
            sai_thrift_remove_acl_table_group_member(
                self.client, acl_table_group_member_id1)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id1)
            sai_thrift_remove_acl_table(self.client, acl_table_id1)
            sai_thrift_remove_acl_counter(self.client, acl_counter1)

            sai_thrift_remove_acl_table_group_member(
                self.client, acl_table_group_member_id2)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id2)
            sai_thrift_remove_acl_table(self.client, acl_table_id2)
            sai_thrift_remove_acl_counter(self.client, acl_counter2)

            sai_thrift_remove_acl_table_group(self.client, acl_table_group_id)

            print('----------------------------------------------------------')

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry_v4)
        sai_thrift_remove_route_entry(self.client, self.route_entry_v6)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry)

        sai_thrift_remove_mirror_session(self.client, self.span_id)
        sai_thrift_remove_policer(self.client, self.pol_id)

        super(DscpMirrorAclTest, self).tearDown()


@group('acl')
class PFCWDAclTest(SaiHelper):
    """This test tries to verify ACL for PFCWD feature.
       Firstly configure qos_maps on port 11.
       After that create ACL table with type PFCWD
       and add entry to this table.
    """

    def setUp(self):
        super(PFCWDAclTest, self).setUp()
        print('--------------------------------------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.0.1 ---> "
              "172.16.10.1)")

        self.ip_addr1 = '172.16.10.1'
        self.ip_addr2 = '192.168.0.1'
        self.dmac1 = '00:11:22:33:44:55'
        self.dmac2 = '00:22:22:22:22:22'
        mask_v4 = '/24'

        self.neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress(self.ip_addr1))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry,
            dst_mac_address=self.dmac1)

        self.nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr1),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry_v4 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.ip_addr1 + mask_v4))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry_v4,
            next_hop_id=self.nhop)

        # dcsp to tc qos_map (6 -> 6)
        dscp_to_tc = []
        dscp_to_tc.append(
            sai_thrift_qos_map_t(
                key=sai_thrift_qos_map_params_t(dscp=6),
                value=sai_thrift_qos_map_params_t(tc=6)))

        qos_map_list = sai_thrift_qos_map_list_t(
            maplist=dscp_to_tc, count=len(dscp_to_tc))

        self.dscp_to_tc_map = sai_thrift_create_qos_map(
            self.client, type=SAI_QOS_MAP_TYPE_DSCP_TO_TC,
            map_to_value_list=qos_map_list)

        sai_thrift_set_port_attribute(
            self.client, self.port11, qos_dscp_to_tc_map=self.dscp_to_tc_map)

        # tc to qid qos_map (6 -> 6)
        tc_to_q = []
        tc_to_q.append(
            sai_thrift_qos_map_t(
                key=sai_thrift_qos_map_params_t(tc=6),
                value=sai_thrift_qos_map_params_t(queue_index=6)))

        qos_map_list = sai_thrift_qos_map_list_t(
            maplist=tc_to_q, count=len(tc_to_q))

        self.tc_to_q_map = sai_thrift_create_qos_map(
            self.client, type=SAI_QOS_MAP_TYPE_TC_TO_QUEUE,
            map_to_value_list=qos_map_list)

        sai_thrift_set_port_attribute(
            self.client, self.port11, qos_tc_to_queue_map=self.tc_to_q_map)

        self.pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     eth_src=self.dmac2,
                                     ip_dst=self.ip_addr1,
                                     ip_src=self.ip_addr2,
                                     ip_dscp=6,
                                     ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(eth_dst=self.dmac1,
                                         eth_src=ROUTER_MAC,
                                         ip_dst=self.ip_addr1,
                                         ip_src=self.ip_addr2,
                                         ip_dscp=6,
                                         ip_ttl=63)

    def runTest(self):
        try:
            self.aclRoutingTest()
            self.pfcwdMirrorTest()

        finally:
            pass

    def aclRoutingTest(self):
        """Tests routing"""
        # send the test packet(s)
        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac2, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            send_packet(self, self.dev_port11, self.pkt)
            print('#### Expecting ', self.dmac1, ' |', ROUTER_MAC, '| ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            verify_packets(self, self.exp_pkt, [self.dev_port10])

        finally:
            print('----------------------------------------------------------')

    def pfcwdMirrorTest(self):
        """
        Tests  PFCWD ACL.
        Firstly, create ACL table. PFCWD table identify by TC and IN_PORTS.
        Secondly, add ACL rule for drop traffic with TC 6 on port 11.
        Due to QOS configuration "dscp 6 -> tc 6 -> qid 6",
        packets with DSCP field 6 will be matched on ACL rule and dropped.
        """
        # setup ACL block
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list), int32list=table_bind_point_list)

        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_in_ports=True,
            field_tc=True)

        traffic_class_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=6),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        in_ports = [self.port11]
        in_ports_list_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(
                objlist=sai_thrift_object_list_t(
                    count=len(in_ports),
                    idlist=in_ports)))

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))

        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            priority=entry_priority,
            table_id=acl_table_id,
            field_in_ports=in_ports_list_t,
            action_packet_action=packet_action,
            field_tc=traffic_class_t)

        try:
            print('#### ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| ', self.dmac2, ' | ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            send_packet(self, self.dev_port11, self.pkt)

            print('#### Expecting ', self.dmac1, ' |', ROUTER_MAC, '| ',
                  self.ip_addr1, ' | ', self.ip_addr2)
            verify_no_other_packets(self, timeout=1)

        finally:
            # cleanup ACL
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)

            print('----------------------------------------------------------')

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry_v4)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry)

        sai_thrift_set_port_attribute(
            self.client, self.port11, qos_tc_to_queue_map=0)
        sai_thrift_set_port_attribute(
            self.client, self.port11, qos_dscp_to_tc_map=0)

        # remove qos maps
        sai_thrift_remove_qos_map(self.client, self.tc_to_q_map)
        sai_thrift_remove_qos_map(self.client, self.dscp_to_tc_map)

        super(PFCWDAclTest, self).tearDown()


@group('acl')
@group('acl-ocp')
class VlanAclTest(SaiHelper):
    """
    Verify ACL vlan test case
    """
    def setUp(self):
        super(VlanAclTest, self).setUp()
        print("Sending L2 packet - port 24 -> port 25 [trunk vlan=100])")
        vlan_id = 100
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        self.ip_addr1 = '192.168.100.1'
        self.ip_addr2 = '172.16.0.1'
        mac_action = SAI_PACKET_ACTION_FORWARD
        self.table_stage_ingress = SAI_ACL_STAGE_INGRESS
        self.table_stage_egress = SAI_ACL_STAGE_EGRESS
        self.group_stage_ingress = SAI_ACL_STAGE_INGRESS
        self.group_stage_egress = SAI_ACL_STAGE_EGRESS

        self.vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)

        self.port24_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.port25_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port25,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        self.vlan_member2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan_oid,
            bridge_port_id=self.port25_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.fdb_entry1 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address=mac1, bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry1,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port24_bp,
            packet_action=mac_action)

        self.fdb_entry2 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address=mac2, bv_id=self.vlan_oid)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry2,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port25_bp,
            packet_action=mac_action)

        self.pkt1 = simple_tcp_packet(eth_dst=mac2,
                                      eth_src=mac1,
                                      dl_vlan_enable=True,
                                      vlan_vid=100,
                                      ip_src=self.ip_addr1,
                                      ip_dst=self.ip_addr2,
                                      ip_id=102,
                                      ip_ttl=64)
        self.exp_pkt1 = simple_tcp_packet(eth_dst=mac2,
                                          eth_src=mac1,
                                          ip_dst=self.ip_addr2,
                                          ip_src=self.ip_addr1,
                                          ip_id=102,
                                          dl_vlan_enable=True,
                                          vlan_vid=100,
                                          ip_ttl=64)

        self.pkt2 = simple_tcp_packet(eth_dst=mac1,
                                      eth_src=mac2,
                                      dl_vlan_enable=True,
                                      vlan_vid=100,
                                      ip_src=self.ip_addr2,
                                      ip_dst=self.ip_addr1,
                                      ip_id=102,
                                      ip_ttl=64)
        self.exp_pkt2 = simple_tcp_packet(eth_dst=mac1,
                                          eth_src=mac2,
                                          ip_dst=self.ip_addr1,
                                          ip_src=self.ip_addr2,
                                          ip_id=102,
                                          dl_vlan_enable=True,
                                          vlan_vid=100,
                                          ip_ttl=64)

    def runTest(self):
        self.noAclTest()
        self.aclVlanTest(self.table_stage_ingress, self.group_stage_ingress)
        self.aclVlanTest(self.table_stage_egress, self.group_stage_egress)

    def tearDown(self):
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry1)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry2)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member1)
        sai_thrift_remove_vlan_member(self.client, self.vlan_member2)
        sai_thrift_remove_bridge_port(self.client, self.port24_bp)
        sai_thrift_remove_bridge_port(self.client, self.port25_bp)
        sai_thrift_remove_vlan(self.client, self.vlan_oid)
        super(VlanAclTest, self).tearDown()

    def noAclTest(self):
        """
        Verify forwarding without ACL
        """
        print('#### NO ACL Applied ####')
        # send the test packet(s)
        print("Sending TCP type test packet port 24 -> port 25")
        send_packet(self, self.dev_port24, self.pkt1)
        verify_packets(self, self.exp_pkt1, [self.dev_port25])
        print("Sending TCP type test packet port 25 -> port 24")
        send_packet(self, self.dev_port25, self.pkt2)
        verify_packets(self, self.exp_pkt2, [self.dev_port24])

    def aclVlanTest(self, table_stage, group_stage):
        """
        Verify ACL with vlan
        Args:
            table_stage (int): specifies the type of ACL table stage
            group_stage (int): specifies the type of ACL group stage
        """
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
        entry_priority = 1

        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL
        group_member_priority = 100

        acl_action = SAI_PACKET_ACTION_DROP
        ip_src_mask = "255.255.255.0"

        if table_stage == SAI_ACL_STAGE_INGRESS:
            ip_src = self.ip_addr1
            sport = self.dev_port24
            pkt = self.pkt1
        elif table_stage == SAI_ACL_STAGE_EGRESS:
            ip_src = self.ip_addr2
            sport = self.dev_port25
            pkt = self.pkt2

        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_point_list),
            int32list=group_bind_point_list)

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=acl_action))

        acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=group_stage,
            acl_bind_point_type_list=group_bind_point_type_list,
            type=group_type)

        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_src_mask))

        acl_table_group_member_id = sai_thrift_create_acl_table_group_member(
            self.client,
            acl_table_group_id=acl_table_group_id,
            acl_table_id=acl_table_id,
            priority=group_member_priority)

        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=entry_priority,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id)

        # attach ACL counter to ACL entry
        action_counter = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter)

        if table_stage == SAI_ACL_STAGE_INGRESS:
            sai_thrift_set_vlan_attribute(self.client,
                                          vlan_oid=self.vlan_oid,
                                          ingress_acl=acl_table_group_id)
        elif table_stage == SAI_ACL_STAGE_EGRESS:
            sai_thrift_set_vlan_attribute(self.client,
                                          vlan_oid=self.vlan_oid,
                                          egress_acl=acl_table_group_id)

        try:
            send_packet(self, sport, pkt)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # cleanup ACL
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_id,
                action_counter=action_counter)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter)

            if table_stage == SAI_ACL_STAGE_INGRESS:
                sai_thrift_set_vlan_attribute(self.client,
                                              vlan_oid=self.vlan_oid,
                                              ingress_acl=0)
            elif table_stage == SAI_ACL_STAGE_EGRESS:
                sai_thrift_set_vlan_attribute(self.client,
                                              vlan_oid=self.vlan_oid,
                                              egress_acl=0)
            sai_thrift_remove_acl_table_group_member(
                self.client, acl_table_group_member_id)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)
            sai_thrift_remove_acl_table_group(self.client, acl_table_group_id)


class AclLagTest(SaiHelper):
    """
    Verify ACL with lag test case
    """
    def setUp(self):
        super(AclLagTest, self).setUp()

        self.lag_id = sai_thrift_create_lag(self.client)

        self.lag_member_id1 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag_id, port_id=self.port24)

        self.vrf = sai_thrift_create_virtual_router(self.client)
        self.rif_id1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.vrf,
            port_id=self.lag_id)

        self.rif_id2 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.vrf,
            port_id=self.port26)

        self.ip_addr1 = "20.0.0.2"
        self.ip_addr_subnet1 = '20.0.0.0'
        self.dmac1 = '00:22:22:22:22:22'
        self.ip_addr_subnet2 = '192.168.0.0'
        self.ip_addr2 = '192.168.0.1'
        self.dmac2 = '00:11:22:33:44:55'
        self.ip_src_mask = "255.255.255.255"
        mask = '/16'

        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.rif_id1, ip_address=sai_ipaddress(self.ip_addr2))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry1,
            dst_mac_address=self.dmac1)

        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr2),
            router_interface_id=self.rif_id1,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.vrf,
            destination=sai_ipprefix(self.ip_addr_subnet2 + mask))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry1,
            next_hop_id=self.nhop1)

        self.neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.rif_id2, ip_address=sai_ipaddress(self.ip_addr1))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry2,
            dst_mac_address=self.dmac2)

        self.nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr1),
            router_interface_id=self.rif_id2,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.vrf,
            destination=sai_ipprefix(self.ip_addr_subnet1 + mask))
        sai_thrift_create_route_entry(
            self.client,
            self.route_entry2,
            next_hop_id=self.nhop2)

    def runTest(self):
        self.lagAclEgressTest()
        self.lagAclIngressTest()

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry2)
        sai_thrift_remove_next_hop(self.client, self.nhop2)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry2)
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_router_interface(self.client, self.rif_id2)
        sai_thrift_remove_router_interface(self.client, self.rif_id1)
        sai_thrift_remove_lag_member(self.client, self.lag_member_id1)
        sai_thrift_remove_lag(self.client, self.lag_id)
        sai_thrift_remove_virtual_router(self.client, self.vrf)
        super(AclLagTest, self).tearDown()

    def lagAclEgressTest(self):
        '''
        Verify egress ACL with lag
        "ACL_RULE|ACL_TABLE_IPV4_ID|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        table_stage_egress = SAI_ACL_STAGE_EGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        action = SAI_PACKET_ACTION_DROP

        ip_src = self.ip_addr1
        ip_dst = self.ip_addr_subnet2
        sport = self.dev_port26
        dport1 = self.dev_port24
        dport2 = self.dev_port25

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=self.ip_src_mask))

        acl_table_ipv4_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_egress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_ipv4_id,
            priority=9999,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        # create ACL counter
        acl_counter_egress = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_ipv4_id)

        # attach ACL counter to ACL entry
        action_counter_egress = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_egress),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter_egress)

        try:
            pkt = simple_tcp_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.dmac1,
                ip_src=ip_src,
                ip_dst=ip_dst,
                tcp_sport=0x4321,
                tcp_dport=0x51,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.dmac1,
                eth_src=ROUTER_MAC,
                ip_src=ip_src,
                ip_dst=ip_dst,
                tcp_sport=0x4321,
                tcp_dport=0x51,
                ip_ttl=63)

            send_packet(self, sport, pkt)
            verify_packets(self, exp_pkt, ports=[dport1])

            # Now bind the ACL table - the packet should be dropped
            sai_thrift_set_lag_attribute(self.client,
                                         lag_oid=self.lag_id,
                                         egress_acl=acl_table_ipv4_id)

            send_packet(self, sport, pkt)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 1)

            # Add one more LAG member and verify the packet
            # is not forwarded to it
            lag_member_id2 = sai_thrift_create_lag_member(
                self.client, lag_id=self.lag_id, port_id=self.port25)
            send_packet(self, sport, pkt)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 2)

            # Now unbind the ACL table
            sai_thrift_set_lag_attribute(self.client,
                                         lag_oid=self.lag_id,
                                         egress_acl=0)

            send_packet(self, sport, pkt)
            verify_packet_any_port(
                self, exp_pkt, [dport1, dport2], timeout=2)

        finally:
            # cleanup ACL
            action_counter_egress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_id,
                action_counter=action_counter_egress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_egress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_egress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_egress)

            if lag_member_id2:
                sai_thrift_remove_lag_member(self.client, lag_member_id2)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_ipv4_id)

    def lagAclIngressTest(self):
        '''
        Verify ingress ACL with lag
        "ACL_RULE|ACL_TABLE_IPV4_ID|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "192.168.0.1/32"
            }
        },
        '''
        table_stage_ingress = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        action = SAI_PACKET_ACTION_DROP

        ip_src = self.ip_addr2
        ip_dst = self.ip_addr_subnet1
        dport = self.dev_port26
        sport1 = self.dev_port24
        sport2 = self.dev_port27

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=self.ip_src_mask))

        acl_table_ipv4_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage_ingress,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True)

        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_ipv4_id,
            priority=9999,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        # create ACL counter
        acl_counter_ingress = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_ipv4_id)

        # attach ACL counter to ACL entry
        action_counter_ingress = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter_ingress)

        try:
            pkt = simple_tcp_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.dmac2,
                ip_src=ip_src,
                ip_dst=ip_dst,
                tcp_sport=0x4321,
                tcp_dport=0x51,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.dmac2,
                eth_src=ROUTER_MAC,
                ip_src=ip_src,
                ip_dst=ip_dst,
                tcp_sport=0x4321,
                tcp_dport=0x51,
                ip_ttl=63)

            # Add one more LAG member
            lag_member_id2 = sai_thrift_create_lag_member(
                self.client, lag_id=self.lag_id, port_id=self.port27)

            send_packet(self, sport1, pkt)
            verify_packets(self, exp_pkt, ports=[dport])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)

            send_packet(self, sport2, pkt)
            verify_packets(self, exp_pkt, ports=[dport])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)

            # Now bind the ACL table - the packet should be dropped
            sai_thrift_set_lag_attribute(self.client,
                                         lag_oid=self.lag_id,
                                         ingress_acl=acl_table_ipv4_id)

            send_packet(self, sport1, pkt)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

            send_packet(self, sport2, pkt)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 2)

            # Now unbind the ACL table
            sai_thrift_set_lag_attribute(self.client,
                                         lag_oid=self.lag_id,
                                         ingress_acl=0)

        finally:
            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_id,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)

            if lag_member_id2:
                sai_thrift_remove_lag_member(self.client, lag_member_id2)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_ipv4_id)


class IngressL3AclDscp(SaiHelper):
    """
    Verify ACL test case with the dscp field
    """
    def setUp(self):
        super(IngressL3AclDscp, self).setUp()

        l4_dst_port = 1000
        ip_addr = '172.16.10.1'
        dmac = '00:11:22:33:44:55'

        self.neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress(ip_addr))
        sai_thrift_create_neighbor_entry(
            self.client,
            self.neighbor_entry,
            dst_mac_address=dmac)

        self.nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(ip_addr),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.pkt = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_dport=l4_dst_port,
            ip_id=105,
            ip_ttl=64,
            ip_tos=200)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=ROUTER_MAC,
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_dport=l4_dst_port,
            ip_id=105,
            ip_ttl=63,
            ip_tos=200)

    def runTest(self):
        self.routingTest()
        self.ingressL3AclDscpTest()

    def tearDown(self):
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry)
        super(IngressL3AclDscp, self).tearDown()

    def routingTest(self):
        """
        Verify basic routing
        """
        print('--------------------------------------------------------------')
        print("Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 ---> "
              "172.16.10.1 [id = 105])")
        try:
            print('#### NO ACL Applied ####')
            print('#### Sending  ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2')
            send_packet(self, self.dev_port11, self.pkt)
            print('#### Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            verify_packets(self, self.exp_pkt, [self.dev_port10])
        finally:
            print('----------------------------------------------------------')

    def ingressL3AclDscpTest(self):
        """
        Verify ACL with the dscp field
        """
        print("Sending packet ptf_intf 2 -[ACL]-> ptf_intf 1 "
              "(192.168.0.1-[ACL]-> 172.16.10.1 [id = 105])")
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"

        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_src_mask))

        field_dscp = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=50))

        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=table_stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ip=True,
            field_dscp=True)
        print("ACL Table created 0x%lx" % (acl_table_id))

        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=entry_priority,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action,
            field_dscp=field_dscp)

        # create ACL counter
        acl_counter_ingress = sai_thrift_create_acl_counter(
            self.client, table_id=acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_ingress = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=acl_counter_ingress),
            enable=True)
        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry_id,
            action_counter=action_counter_ingress)

        # bind this ACL table to rif_id2s object id
        sai_thrift_set_router_interface_attribute(
            self.client, self.port11_rif, ingress_acl=acl_table_id)

        try:
            self.assertNotEqual(acl_table_id, 0)
            self.assertNotEqual(acl_entry_id, 0)

            print('#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, '
                  'SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####')
            print('#### Sending      ', ROUTER_MAC, '| 00:22:22:22:22:22 | '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1')
            # send the same packet
            send_packet(self, self.dev_port11, self.pkt)
            # ensure packet is dropped
            # check for absence of packet here!
            print('#### NOT Expecting 00:11:22:33:44:55 |', ROUTER_MAC, '| '
                  '172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0')
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 1)

        finally:
            # unbind this ACL table from rif_id2s object id
            sai_thrift_set_router_interface_attribute(
                self.client, self.port11_rif, ingress_acl=0)

            # cleanup ACL
            action_counter_ingress = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_id,
                action_counter=action_counter_ingress)
            sai_thrift_set_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=None)
            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter_ingress, packets=True)
            self.assertEqual(packets['packets'], 0)
            sai_thrift_remove_acl_counter(self.client, acl_counter_ingress)

            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)


# WIP
@disabled
class IPEthAclTest(SaiHelper):
    """
    This test mimics SONiC/SAI ACL behavior. SONiC does not have notion
    of MAC ACL. It programs entries with non ip eth type match entries
    into IP ACL. This is different than the SDE behavior which expects
    this entries to be programmed into MAC ACL. This test verifies
    the SDE behavior to be able to identify such acl rules and program
    it correctly in the corresponding H/W ACL tables.
    """
    def setUp(self):
        super(IPEthAclTest, self).setUp()
        self.traps = []
        self.acl_grp_members = []
        self.acl_grps = []
        self.acl_rules = []
        self.acl_tables = []
        self.acl_counters = []
        self.vlans = []
        self.vlan_members = []
        self.bridge_ports = []
        self.vlan_ports = []
        self.bp_list = []

        self.rif_id = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port28)

        # Add port 24, 25 to Vlan100
        vlan_id1 = 100
        vlan_oid1 = sai_thrift_create_vlan(self.client, vlan_id1)
        self.vlans.append(vlan_oid1)

        port1_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.bridge_ports.append(port1_bp)
        vlan_member1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=vlan_oid1,
            bridge_port_id=port1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member1)
        self.vlan_ports.append(self.port24)

        port2_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port25,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.bridge_ports.append(port2_bp)
        vlan_member2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=vlan_oid1,
            bridge_port_id=port2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member2)
        self.vlan_ports.append(self.port25)

        sai_thrift_set_port_attribute(
            self.client, self.port24, port_vlan_id=vlan_id1)
        sai_thrift_set_port_attribute(
            self.client, self.port25, port_vlan_id=vlan_id1)

        # Add port 26, 27 to Vlan200
        vlan_id2 = 200
        vlan_oid2 = sai_thrift_create_vlan(self.client, vlan_id2)
        self.vlans.append(vlan_oid2)

        port3_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port26,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.bridge_ports.append(port3_bp)
        vlan_member3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=vlan_oid2,
            bridge_port_id=port3_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member3)
        self.vlan_ports.append(self.port26)

        port4_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port27,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.bridge_ports.append(port4_bp)
        vlan_member4 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=vlan_oid2,
            bridge_port_id=port4_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member4)
        self.vlan_ports.append(self.port27)

        sai_thrift_set_port_attribute(
            self.client, self.port26, port_vlan_id=vlan_id2)
        sai_thrift_set_port_attribute(
            self.client, self.port27, port_vlan_id=vlan_id2)

        self.arp_pkt = simple_arp_packet(arp_op=1, pktlen=100)
        self.exp_arp_pkt = self.arp_pkt

        # Setup Mirror ACL
        monitor_port = self.port28
        mirror_type = SAI_MIRROR_SESSION_TYPE_LOCAL
        self.spanid = sai_thrift_create_mirror_session(
            self.client,
            type=mirror_type,
            monitor_port=monitor_port)
        print(self.spanid)
        print('Mirror session created 0x%lx' % self.spanid)
        mirror_session_list = sai_thrift_object_list_t(
            count=1, idlist=[self.spanid])

        self.mirror_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                objlist=mirror_session_list))

        self.ipx_pkt = simple_eth_packet(pktlen=100,
                                         eth_dst='00:01:02:03:04:05',
                                         eth_src='00:06:07:08:09:0a',
                                         eth_type=0x8137)
        self.exp_ipx_pkt = self.ipx_pkt

        self.ipv4_pkt = simple_ip_packet(pktlen=100,
                                         eth_dst='00:1B:19:00:00:00')
        self.exp_ipv4_pkt = self.ipv4_pkt

        self.lldp_pkt = simple_eth_packet(pktlen=100,
                                          eth_dst='01:80:C2:00:00:0E',
                                          eth_type=0x88cc)
        self.exp_lldp_pkt = self.lldp_pkt

        self.tcpv6_pkt = simple_tcpv6_packet(pktlen=100, ipv6_src='2000::4')
        self.exp_tcpv6_pkt = self.tcpv6_pkt

        self.tcpv4_pkt = simple_tcp_packet(pktlen=100, ip_src='172.0.0.17')
        self.exp_tcpv4_pkt = self.tcpv4_pkt

        self.tcpv6_pkt2 = simple_tcpv6_packet(pktlen=100, ipv6_src='3000::1')
        self.exp_tcpv6_pkt2 = self.tcpv6_pkt2

        self.udpv6_pkt = simple_udpv6_packet(pktlen=100, ipv6_src='5000::1')
        self.exp_udpv6_pkt = self.udpv6_pkt

        self.udpv4_pkt = simple_udp_packet(pktlen=100, ip_src='10.0.0.1')
        self.exp_udpv4_pkt = self.udpv4_pkt

        self.mac_pkt = simple_eth_packet(pktlen=100,
                                         eth_dst='00:01:02:03:04:05',
                                         eth_src='00:06:07:08:09:0a',
                                         eth_type=0x1234)
        self.exp_mac_pkt = self.mac_pkt

        self.mac_pkt2 = simple_eth_packet(pktlen=100,
                                          eth_dst='00:01:02:03:04:05',
                                          eth_src='00:06:07:08:09:0a',
                                          eth_type=0x4321)
        self.exp_mac_pkt2 = self.mac_pkt2

        self.exp_counters = {"arp": 0, "ipx": 0, "ipv4": 0, "lldp": 0,
                             "ipv6": 0, "mirror_ip4": 0, "mirror_ip6": 0,
                             "mirror_udp": 0, "mirror_mac": 0}

    def runTest(self):
        self.noAclTest()
        self.arpAclTest()
        self.ipxAclTest()
        self.ipv4AclTest()
        self.lldpAclTest()
        self.ipv4MirrorAclTest()
        self.ipv6MirrorAclTest()
        self.udpMirrorAclTest()
        self.macMirrorAclTest()

    def tearDown(self):
        sai_thrift_remove_mirror_session(self.client, self.spanid)
        for vlan_port in list(self.vlan_ports):
            sai_thrift_set_port_attribute(
                self.client, vlan_port, port_vlan_id=int(SAI_NULL_OBJECT_ID))
            self.vlan_ports.remove(vlan_port)
        for vlan_member in list(self.vlan_members):
            sai_thrift_remove_vlan_member(self.client, vlan_member)
            self.vlan_members.remove(vlan_member)
        for vlan in list(self.vlans):
            sai_thrift_remove_vlan(self.client, vlan)
            self.vlans.remove(vlan)
        for bridge_port in self.bridge_ports:
            sai_thrift_remove_bridge_port(self.client, bridge_port)
            self.bridge_ports.remove(bridge_port)
        sai_thrift_remove_router_interface(self.client, self.rif_id)
        super(IPEthAclTest, self).tearDown()

    def noAclTest(self):
        """Tests routing without ACL"""
        print("Sending ARP packet - port 24 -> port 25 [access vlan=100])")
        print("Sending ARP packet - port 26 -> port 27 [access vlan=200])")
        # send & verify test packet(s) : ARP, IPX, IPv4, LLDP,
        # IPv6 without any ACL entries
        send_packet(self, self.dev_port24, self.arp_pkt)
        verify_packets(self, self.exp_arp_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.arp_pkt)
        verify_packets(self, self.exp_arp_pkt, [self.dev_port27])

        print("Sending IPX packet - port 24 -> port 25 [access vlan=100])")
        print("Sending IPX packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.ipx_pkt)
        verify_packets(self, self.exp_ipx_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.ipx_pkt)
        verify_packets(self, self.exp_ipx_pkt, [self.dev_port27])

        print("Sending IPv4 packet - port 24 -> port 25 [access vlan=100])")
        print("Sending IPv4 packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.ipv4_pkt)
        verify_packets(self, self.exp_ipv4_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.ipv4_pkt)
        verify_packets(self, self.exp_ipv4_pkt, [self.dev_port27])

        print("Sending LLDP packet - port 24 -> port 25 [access vlan=100])")
        print("Sending LLDP packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.lldp_pkt)
        verify_packets(self, self.exp_lldp_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.lldp_pkt)
        verify_packets(self, self.exp_lldp_pkt, [self.dev_port27])

        print("Sending IPv6/TCP(SRC_IP 2000::4) packet - port 24 -> "
              "port 25 [access vlan=100])")
        print("Sending IPv6/TCP(SRC_IP 2000::4) packet - port 26 -> "
              "port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.tcpv6_pkt)
        verify_packets(self, self.exp_tcpv6_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.tcpv6_pkt)
        verify_packets(self, self.exp_tcpv6_pkt, [self.dev_port27])

        print("Sending IPv4/TCP packet(SRC_IP 172.0.0.17) - port 24 -> "
              "port 25 [access vlan=100])")
        print("Sending IPv4/TCP packet(SRC_IP 172.0.0.17) - port 26 -> "
              "port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.tcpv4_pkt)
        verify_packets(self, self.exp_tcpv4_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.tcpv4_pkt)
        verify_packets(self, self.exp_tcpv4_pkt, [self.dev_port27])

        print("Sending IPv6/TCP packet(SRC_IP 3000::1) - port 24 -> "
              "port 25 [access vlan=100])")
        print("Sending IPv6/TCP packet(SRC_IP 3000::1) - port 26 -> "
              "port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.tcpv6_pkt2)
        verify_packets(self, self.exp_tcpv6_pkt2, [self.dev_port25])
        send_packet(self, self.dev_port26, self.tcpv6_pkt2)
        verify_packets(self, self.exp_tcpv6_pkt2, [self.dev_port27])

        print("Sending IPv6/UDP packet - port 24 -> "
              "port 25 [access vlan=100])")
        print("Sending IPv6/UDP packet - port 26 -> "
              "port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.udpv6_pkt)
        verify_packets(self, self.exp_udpv6_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.udpv6_pkt)
        verify_packets(self, self.exp_udpv6_pkt, [self.dev_port27])

        print("Sending IPv4/UDP packet - port 24 -> "
              "port 25 [access vlan=100])")
        print("Sending IPv4/UDP packet - port 26 -> "
              "port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.udpv4_pkt)
        verify_packets(self, self.exp_udpv4_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.udpv4_pkt)
        verify_packets(self, self.exp_udpv4_pkt, [self.dev_port27])

        print("Sending EthType(0x1234) packet - port 24 -> "
              "port 25 [access vlan=100])")
        print("Sending EthType(0x1234) packet - port 26 -> "
              "port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.mac_pkt)
        verify_packets(self, self.exp_mac_pkt, [self.dev_port25])
        send_packet(self, self.dev_port26, self.mac_pkt)
        verify_packets(self, self.exp_mac_pkt, [self.dev_port27])

        # Used for negative test cases verification
        print("Sending EthType(0x4321) packet - port 24 -> "
              "port 25 [access vlan=100])")
        print("Sending EthType(0x4321) packet - port 26 -> "
              "port 27 [access vlan=200])")
        send_packet(self, self.dev_port24, self.mac_pkt2)
        verify_packets(self, self.exp_mac_pkt2, [self.dev_port25])
        send_packet(self, self.dev_port26, self.mac_pkt2)
        verify_packets(self, self.exp_mac_pkt2, [self.dev_port27])

    def arpAclTest(self):
        """Tests ACL dropping ARP packets"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        action = SAI_PACKET_ACTION_DROP
        eth_type = 0x0806
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        vlan_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(vlan_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            vlan_acl_table_group_id))

        aclv4_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_ether_type=True)
        self.acl_tables.append(aclv4_table_id)
        print("IPv4 ACL Table created 0x%lx" % (aclv4_table_id))

        aclv4_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=vlan_acl_table_group_id,
                acl_table_id=aclv4_table_id,
                priority=100)
        self.acl_grp_members.append(aclv4_table_group_member_id)

        # Add ARP drop ACL entry to IPv4 ACL Table
        arp_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=aclv4_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=arp_acl_counter),
            enable=True)
        self.acl_counters.append(arp_acl_counter)

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        arp_drop_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=aclv4_table_id,
            priority=entry_priority,
            action_packet_action=packet_action,
            field_ether_type=ether_type,
            action_counter=action_counter_t)

        sai_thrift_set_acl_entry_attribute(
            self.client, arp_drop_entry,
            action_counter=action_counter_t)

        self.acl_rules.append(arp_drop_entry)
        print("ARP ACL DROP entry created 0x%lx" % (arp_drop_entry))

        print("Binding ACL grp 0x%lx to Vlan100" % (vlan_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_vlan_attribute(self.client,
                                      vlan_oid=self.vlans[0],
                                      ingress_acl=vlan_acl_table_group_id)
        self.bp_list.append(self.vlans[0])

        # Initialize Counters
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=arp_acl_counter,
            packets=True)
        self.exp_counters["arp"] = counter["packets"]

        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should be dropped")
        print("Sending ARP packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.arp_pkt)
        verify_no_other_packets(self, timeout=1)
        self.exp_counters["arp"] += 1

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=arp_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["arp"] == counter["packets"],
                        "ARP ACL DROP Counter Mismatch")

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending ARP packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.arp_pkt)
        verify_packets(self, self.exp_arp_pkt, [self.dev_port27])

        self.trapAclCleanup()

        print("Verifying ACL rules were correctly removed")
        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should not be dropped")
        print("Sending ARP packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.arp_pkt)
        verify_packets(self, self.exp_arp_pkt, [self.dev_port25])

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending ARP packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.arp_pkt)
        verify_packets(self, self.exp_arp_pkt, [self.dev_port27])

    def ipxAclTest(self):
        """Tests ACL dropping IPX packets"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        action = SAI_PACKET_ACTION_DROP
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        vlan_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(vlan_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            vlan_acl_table_group_id))

        aclv4_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_ether_type=True)
        self.acl_tables.append(aclv4_table_id)
        print("IPv4 ACL Table created 0x%lx" % (aclv4_table_id))

        aclv4_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=vlan_acl_table_group_id,
                acl_table_id=aclv4_table_id,
                priority=100)
        self.acl_grp_members.append(aclv4_table_group_member_id)

        if entry_priority >= 1:
            entry_priority -= 1

        # Add IPX drop ACL entry to IPv4 ACL Table
        ipx_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=aclv4_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=ipx_acl_counter),
            enable=True)
        self.acl_counters.append(ipx_acl_counter)

        eth_type = 0x8137
        if eth_type > 0x7fff:
            eth_type -= 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        ipx_drop_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=aclv4_table_id,
            priority=entry_priority,
            action_packet_action=packet_action,
            field_ether_type=ether_type,
            action_counter=action_counter_t)

        sai_thrift_set_acl_entry_attribute(
            self.client, ipx_drop_entry,
            action_counter=action_counter_t)

        self.acl_rules.append(ipx_drop_entry)
        print("IPX ACL DROP entry created 0x%lx" % (ipx_drop_entry))

        print("Binding ACL grp 0x%lx to Vlan100" % (vlan_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_vlan_attribute(self.client,
                                      vlan_oid=self.vlans[0],
                                      ingress_acl=vlan_acl_table_group_id)
        self.bp_list.append(self.vlans[0])

        # Initialize Counters
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=ipx_acl_counter,
            packets=True)
        self.exp_counters["ipx"] = counter["packets"]

        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should be dropped")
        print("Sending IPX packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.ipx_pkt)
        verify_no_other_packets(self, timeout=1)
        self.exp_counters["ipx"] += 1

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=ipx_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["ipx"] == counter["packets"],
                        "IPX ACL DROP Counter Mismatch")

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending IPX packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.ipx_pkt)
        verify_packets(self, self.exp_ipx_pkt, [self.dev_port27])

        self.trapAclCleanup()

        print("Verifying ACL rules were correctly removed")
        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should not be dropped")
        print("Sending IPX packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.ipx_pkt)
        verify_packets(self, self.exp_ipx_pkt, [self.dev_port25])

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending IPX packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.ipx_pkt)
        verify_packets(self, self.exp_ipx_pkt, [self.dev_port27])

    def ipv4AclTest(self):
        """Tests ACL dropping all packets with IPv4 address"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        action = SAI_PACKET_ACTION_DROP
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        vlan_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(vlan_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            vlan_acl_table_group_id))

        aclv4_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_ether_type=True)
        self.acl_tables.append(aclv4_table_id)
        print("IPv4 ACL Table created 0x%lx" % (aclv4_table_id))

        aclv4_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=vlan_acl_table_group_id,
                acl_table_id=aclv4_table_id,
                priority=100)
        self.acl_grp_members.append(aclv4_table_group_member_id)

        if entry_priority >= 1:
            entry_priority -= 1

        ipv4_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=aclv4_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=ipv4_acl_counter),
            enable=True)
        self.acl_counters.append(ipv4_acl_counter)

        eth_type = 0x0800
        if eth_type > 0x7fff:
            eth_type -= 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        ipv4_drop_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=aclv4_table_id,
            priority=entry_priority,
            action_packet_action=packet_action,
            field_ether_type=ether_type,
            action_counter=action_counter_t)

        sai_thrift_set_acl_entry_attribute(
            self.client, ipv4_drop_entry,
            action_counter=action_counter_t)

        self.acl_rules.append(ipv4_drop_entry)
        print("IPv4 ACL DROP (catch all) entry created 0x%lx" % (
            ipv4_drop_entry))

        print("Binding ACL grp 0x%lx to Vlan100" % (vlan_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_vlan_attribute(self.client,
                                      vlan_oid=self.vlans[0],
                                      ingress_acl=vlan_acl_table_group_id)
        self.bp_list.append(self.vlans[0])

        # Initialize Counters
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=ipv4_acl_counter,
            packets=True)
        self.exp_counters["ipv4"] = counter["packets"]

        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should be dropped")
        print("Sending IPv4 packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.ipv4_pkt)
        verify_no_other_packets(self, timeout=1)
        self.exp_counters["ipv4"] += 1

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=ipv4_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["ipv4"] == counter["packets"],
                        "IPv4 ACL DROP Counter Mismatch")

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending IPv4 packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.ipv4_pkt)
        verify_packets(self, self.exp_ipv4_pkt, [self.dev_port27])

        self.trapAclCleanup()

        print("Verifying ACL rules were correctly removed")
        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should not be dropped")
        print("Sending IPv4 packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.ipv4_pkt)
        verify_packets(self, self.exp_ipv4_pkt, [self.dev_port25])

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending IPv4 packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.ipv4_pkt)
        verify_packets(self, self.exp_ipv4_pkt, [self.dev_port27])

    def lldpAclTest(self):
        """Tests ACL dropping LLDP packets"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        action = SAI_PACKET_ACTION_DROP
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        vlan_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(vlan_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            vlan_acl_table_group_id))

        aclv6_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_ether_type=True,)
        self.acl_tables.append(aclv6_table_id)
        print("IPv6 ACL Table created 0x%lx" % (aclv6_table_id))

        aclv6_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=vlan_acl_table_group_id,
                acl_table_id=aclv6_table_id,
                priority=200)
        self.acl_grp_members.append(aclv6_table_group_member_id)

        if entry_priority >= 1:
            entry_priority -= 1

        # Add LLDP drop ACL entry to IPv6 ACL Table
        lldp_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=aclv6_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=lldp_acl_counter),
            enable=True)
        self.acl_counters.append(lldp_acl_counter)

        eth_type = 0x88cc
        if eth_type > 0x7fff:
            eth_type -= 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        lldp_drop_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=aclv6_table_id,
            priority=entry_priority,
            action_packet_action=packet_action,
            field_ether_type=ether_type,
            action_counter=action_counter_t)

        sai_thrift_set_acl_entry_attribute(
            self.client, lldp_drop_entry,
            action_counter=action_counter_t)

        self.acl_rules.append(lldp_drop_entry)
        print("Installing user-acl-rule to drop LLDP 0x%lx" % (
            lldp_drop_entry))

        print("Binding ACL grp 0x%lx to Vlan100" % (vlan_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_vlan_attribute(self.client,
                                      vlan_oid=self.vlans[0],
                                      ingress_acl=vlan_acl_table_group_id)
        self.bp_list.append(self.vlans[0])

        # Create LLDP trap entry
        lldp_trap = sai_thrift_create_hostif_trap(
            client=self.client,
            trap_type=SAI_HOSTIF_TRAP_TYPE_LLDP,
            packet_action=SAI_PACKET_ACTION_TRAP)
        self.traps.append(lldp_trap)
        print("Installing LLDP hostif trap rule 0x%lx" % (lldp_trap))

        # Initialize Counters
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=lldp_acl_counter,
            packets=True)
        self.exp_counters["lldp"] = counter["packets"]

        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should be dropped")
        # User ACL entries will have higher priority
        # than hostif trap entries
        print("Have Hostif entry to trap LLDP, have User-acl to drop "
              "LLDP ingressing on port 1")
        print("User-acl entry must take priority over hostif entry")
        print("TX LLDP packet - port 24 -> drop")
        send_packet(self, self.dev_port24, self.lldp_pkt)
        verify_no_other_packets(self, timeout=1)
        self.exp_counters["lldp"] += 1

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=lldp_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["lldp"] == counter["packets"],
                        "LLDP ACL DROP Counter Mismatch")

        # This packet will be trapped to the cpu
        pre_stats = sai_thrift_get_queue_stats(
            self.client, self.cpu_queue0)
        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending LLDP packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.lldp_pkt)
        time.sleep(10)
        post_stats = sai_thrift_get_queue_stats(
            self.client, self.cpu_queue0)
        self.assertEqual(
            post_stats["SAI_QUEUE_STAT_PACKETS"],
            pre_stats["SAI_QUEUE_STAT_PACKETS"] + 1)

        self.trapAclCleanup()

        print("Verifying ACL rules were correctly removed")
        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should not be dropped")
        print("Sending LLDP packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.lldp_pkt)
        verify_packets(self, self.exp_lldp_pkt, [self.dev_port25])

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending LLDP packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.lldp_pkt)
        verify_packets(self, self.exp_lldp_pkt, [self.dev_port27])

    def ipv6AclTest(self):
        """Tests ACL dropping all packets with IPv6 address"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        action = SAI_PACKET_ACTION_DROP
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=action))

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        vlan_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(vlan_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            vlan_acl_table_group_id))

        aclv6_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_src_ipv6=True,
            field_ether_type=True)
        self.acl_tables.append(aclv6_table_id)
        print("IPv6 ACL Table created 0x%lx" % (aclv6_table_id))

        aclv6_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=vlan_acl_table_group_id,
                acl_table_id=aclv6_table_id,
                priority=200)
        self.acl_grp_members.append(aclv6_table_group_member_id)

        if entry_priority >= 1:
            entry_priority -= 1

        # Add IPv6 drop ACL entry based on SRC_IPv6 Addr to IPv6 ACL Table
        ipv6_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=aclv6_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=ipv6_acl_counter),
            enable=True)
        self.acl_counters.append(ipv6_acl_counter)

        ip_src = "2000::1"
        ip_src_mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:0"
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip6=ip_src_mask))

        eth_type = 0x86dd
        if eth_type > 0x7fff:
            eth_type -= 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        ipv6_drop_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=aclv6_table_id,
            priority=entry_priority,
            field_src_ipv6=src_ip_t,
            action_packet_action=packet_action,
            field_ether_type=ether_type,
            action_counter=action_counter_t)

        sai_thrift_set_acl_entry_attribute(
            self.client, ipv6_drop_entry,
            action_counter=action_counter_t)
        self.acl_rules.append(ipv6_drop_entry)
        print("IPv6 ACL DROP entry created for SRC_IP:2000::0:x 0x%lx" % (
            ipv6_drop_entry))

        print("Binding ACL grp 0x%lx to Vlan100" % (vlan_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_vlan_attribute(self.client,
                                      vlan_oid=self.vlans[0],
                                      ingress_acl=vlan_acl_table_group_id)
        self.bp_list.append(self.vlans[0])

        # Initialize Counters
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=ipv6_acl_counter,
            packets=True)
        self.exp_counters["ipv6"] = counter["packets"]

        print("Sending Packets matching ACL rules on VLAN100. "
              "Packets should be dropped")
        print("Sending TCPv6 packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.tcpv6_pkt)
        verify_no_other_packets(self, timeout=1)
        self.exp_counters["ipv6"] += 1

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=ipv6_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["ipv6"] == counter["packets"],
                        "IPv6 SRC_IP ACL DROP Counter Mismatch")

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending TCPv6 packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.tcpv6_pkt)
        verify_packets(self, self.exp_tcpv6_pkt, [self.dev_port27])

        self.trapAclCleanup()

        print("Verifying ACL rules were correctly removed")
        print("Sending TCPv6 packet - port 24 -> port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.tcpv6_pkt)
        verify_packets(self, self.exp_tcpv6_pkt, [self.dev_port25])

        print("Sending Packets matching ACL rules on VLAN200. "
              "Packets should not be dropped")
        print("Sending TCPv6 packet - port 26 -> port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.tcpv6_pkt)
        verify_packets(self, self.exp_tcpv6_pkt, [self.dev_port27])

    def ipv4MirrorAclTest(self):
        """Tests ACL mirroring based on IPv4 address"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        port_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(port_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            port_acl_table_group_id))

        if entry_priority >= 1:
            entry_priority -= 1

        # Create Mirror ACL table
        ip_src = "172.0.0.17"
        ip_src_mask = "255.255.255.255"
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip4=ip_src_mask))

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        field_dscp = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=1))

        mirror_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_dscp=True,
            field_src_ip=True)
        self.acl_tables.append(mirror_table_id)

        mirror_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=port_acl_table_group_id,
                acl_table_id=mirror_table_id,
                priority=300)
        self.acl_grp_members.append(mirror_table_group_member_id)

        # Add IPV4 SRC IP Mirror ACL entry to Mirror ACL Table
        v4src_mirror_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=mirror_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=v4src_mirror_acl_counter),
            enable=True)
        self.acl_counters.append(v4src_mirror_acl_counter)

        v4src_mirror_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=mirror_table_id,
            priority=entry_priority,
            field_src_ip=src_ip_t,
            action_mirror_ingress=self.mirror_action,
            action_counter=action_counter_t,
            field_dscp=field_dscp)
        self.acl_rules.append(v4src_mirror_entry)
        print("v4src ACL Mirror entry created 0x%lx" % (v4src_mirror_entry))

        print("Binding ACL grp 0x%lx to Port3" % (port_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_port_attribute(
            self.client, self.port26, ingress_acl=port_acl_table_group_id)
        self.bp_list.append(self.port26)

        # Initialize Counters
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=v4src_mirror_acl_counter,
            packets=True)
        self.exp_counters["mirror_ip4"] = counter["packets"]

        print("Sending Packets matching Mirror ACL rules on Port26. "
              "Packets should be mirrored")
        print("Sending TCPv4 (SRC IP 172.0.0.17) packet - port 26 -> "
              "port 27 [access vlan=200])")
        send_packet(self, self.dev_port26, self.tcpv4_pkt)
        verify_packets(self, self.exp_tcpv4_pkt, [self.dev_port27,
                                                  self.dev_port28])
        self.exp_counters["mirror_ip4"] += 1

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=v4src_mirror_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["mirror_ip4"] == counter["packets"],
                        "IPv4 SRC_IP ACL Mirror Counter Mismatch")

        self.trapAclCleanup()

        print("Sending Packets matching Mirror ACL rules on Port26. "
              "Packets should not be mirrored")
        print("Sending TCPv4 (SRC IP 172.0.0.17) packet - port 26 -> "
              "port 4 [access vlan=200])")
        send_packet(self, self.dev_port26, self.tcpv4_pkt)
        verify_packets(self, self.exp_tcpv4_pkt, [self.dev_port27])

    def ipv6MirrorAclTest(self):
        """Tests ACL mirroring based on IPv6 address"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        port_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(port_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            port_acl_table_group_id))

        # Create Mirror ACL table
        field_dscp = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=1))

        mirror_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_dscp=True,
            field_src_ipv6=True)
        self.acl_tables.append(mirror_table_id)

        mirror_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=port_acl_table_group_id,
                acl_table_id=mirror_table_id,
                priority=300)
        self.acl_grp_members.append(mirror_table_group_member_id)

        if entry_priority >= 1:
            entry_priority -= 1

        # Add IPV6 SRC IP Mirror ACL entry to Mirror ACL Table
        v6src_mirror_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=mirror_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=v6src_mirror_acl_counter),
            enable=True)
        self.acl_counters.append(v6src_mirror_acl_counter)

        ip_src = "3000::1"
        ip_src_mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:0"
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=ip_src),
            mask=sai_thrift_acl_field_data_mask_t(ip6=ip_src_mask))

        v6src_mirror_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=mirror_table_id,
            priority=entry_priority,
            field_src_ipv6=src_ip_t,
            action_mirror_ingress=self.mirror_action,
            action_counter=action_counter_t,
            field_dscp=field_dscp)
        self.acl_rules.append(v6src_mirror_entry)
        print("v6src ACL Mirror entry created 0x%lx" % (v6src_mirror_entry))

        print("Binding ACL grp 0x%lx to Port26" % (port_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_port_attribute(
            self.client, self.port26, ingress_acl=port_acl_table_group_id)
        self.bp_list.append(self.port26)

        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=v6src_mirror_acl_counter,
            packets=True)
        self.exp_counters["mirror_ip6"] = counter["packets"]

        print("Sending Packets matching Mirror ACL rules on Port26. "
              "Packets should be mirrored")
        print("Sending TCPv6 (SRC IP 3000::1) packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.tcpv6_pkt2)
        verify_packets(self, self.exp_tcpv6_pkt2, [self.dev_port27,
                                                   self.dev_port28])
        self.exp_counters["mirror_ip6"] += 1

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=v6src_mirror_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["mirror_ip6"] == counter["packets"],
                        "IPv6 SRC_IP ACL Mirror Counter Mismatch")

        self.trapAclCleanup()

        print("Sending Packets matching Mirror ACL rules on Port26. "
              "Packets should not be mirrored")
        print("Sending TCPv6 (SRC IP 3000::1) packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.tcpv6_pkt2)
        verify_packets(self, self.exp_tcpv6_pkt2, [self.dev_port27])

    def udpMirrorAclTest(self):
        """Tests mirroring UDP packets"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        port_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(port_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            port_acl_table_group_id))

        # Create Mirror ACL table
        field_dscp = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=1))

        mirror_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_dscp=True)
        self.acl_tables.append(mirror_table_id)

        mirror_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=port_acl_table_group_id,
                acl_table_id=mirror_table_id,
                priority=300)
        self.acl_grp_members.append(mirror_table_group_member_id)

        if entry_priority >= 1:
            entry_priority -= 1

        # Add UDP (IPv4/IPv6 common) ACL entry to Mirror ACL Table
        udp_mirror_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=mirror_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=udp_mirror_acl_counter),
            enable=True)
        self.acl_counters.append(udp_mirror_acl_counter)

        ip_protocol = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=17),
            mask=sai_thrift_acl_field_data_mask_t(u8=127))

        udp_mirror_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=mirror_table_id,
            priority=entry_priority,
            field_ip_protocol=ip_protocol,
            action_mirror_ingress=self.mirror_action,
            action_counter=action_counter_t,
            field_dscp=field_dscp)
        self.acl_rules.append(udp_mirror_entry)
        print("TCP ACL Mirror entry created 0x%lx" % (udp_mirror_entry))

        print("Binding ACL grp 0x%lx to Port26" % (port_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_port_attribute(
            self.client, self.port26, ingress_acl=port_acl_table_group_id)
        self.bp_list.append(self.port26)

        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=udp_mirror_acl_counter,
            packets=True)
        self.exp_counters["mirror_udp"] = counter["packets"]

        print("Sending Packets matching Mirror ACL rules on Port3. "
              "Packets should be mirrored")
        print("Sending UDPv6  packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.udpv6_pkt)
        verify_packets(self, self.exp_udpv6_pkt, [self.dev_port27,
                                                  self.dev_port28])
        self.exp_counters["mirror_udp"] += 1

        print("Sending UDPv4  packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.udpv4_pkt)
        verify_packets(self, self.exp_udpv4_pkt, [self.dev_port27,
                                                  self.dev_port28])
        self.exp_counters["mirror_udp"] += 1

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=udp_mirror_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["mirror_udp"] == counter["packets"],
                        "UDP ACL Mirror Counter Mismatch")

        self.trapAclCleanup()

        print("Sending Packets matching Mirror ACL rules on Port26. "
              "Packets should not be mirrored")
        print("Sending UDPv6  packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.udpv6_pkt)
        verify_packets(self, self.exp_udpv6_pkt, [self.dev_port27])

        print("Sending UDPv4  packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.udpv4_pkt)
        verify_packets(self, self.exp_udpv4_pkt, [self.dev_port27])

    def macMirrorAclTest(self):
        """Tests mirroring MAC packets"""
        # Create ACL Table, Group and Rules
        stage = SAI_ACL_STAGE_INGRESS
        entry_priority = 1000
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        table_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(table_bind_point_list),
            int32list=table_bind_point_list)

        port_acl_table_group_id = sai_thrift_create_acl_table_group(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            type=group_type)
        self.acl_grps.append(port_acl_table_group_id)
        print("VLAN ACL Table Group created 0x%lx" % (
            port_acl_table_group_id))

        # Create Mirror ACL table
        field_dscp = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=1))

        mirror_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=stage,
            acl_bind_point_type_list=table_bind_point_type_list,
            field_ether_type=True,
            field_dscp=True)
        self.acl_tables.append(mirror_table_id)

        mirror_table_group_member_id = \
            sai_thrift_create_acl_table_group_member(
                self.client,
                acl_table_group_id=port_acl_table_group_id,
                acl_table_id=mirror_table_id,
                priority=300)
        self.acl_grp_members.append(mirror_table_group_member_id)

        if entry_priority >= 1:
            entry_priority -= 1

        # Add Eth type MAC ACL entry to Mirror ACL Table
        mac_mirror_acl_counter = sai_thrift_create_acl_counter(
            client=self.client, table_id=mirror_table_id)
        action_counter_t = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                oid=mac_mirror_acl_counter),
            enable=True)
        self.acl_counters.append(mac_mirror_acl_counter)

        eth_type = 0x1234
        if eth_type > 0x7fff:
            eth_type -= 0x10000

        ether_type = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=32767))

        mac_mirror_entry = sai_thrift_create_acl_entry(
            self.client,
            table_id=mirror_table_id,
            priority=entry_priority,
            action_mirror_ingress=self.mirror_action,
            action_counter=action_counter_t,
            field_ether_type=ether_type,
            field_dscp=field_dscp)
        self.acl_rules.append(mac_mirror_entry)
        print("MAC ACL Mirror entry created 0x%lx" % (mac_mirror_entry))

        print("Binding ACL grp 0x%lx to Port26" % (port_acl_table_group_id))
        # bind ACL GRP to VLAN 100
        sai_thrift_set_port_attribute(
            self.client, self.port26, ingress_acl=port_acl_table_group_id)
        self.bp_list.append(self.port26)

        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=mac_mirror_acl_counter,
            packets=True)
        self.exp_counters["mirror_mac"] = counter["packets"]

        print("Sending Packets matching Mirror ACL rules on Port26. "
              "Packets should be mirrored")
        print("Sending EthType(0x1234) packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.mac_pkt)
        verify_packets(self, self.exp_mac_pkt, [self.dev_port27,
                                                self.dev_port28])
        self.exp_counters["mirror_mac"] += 1
        print("Sending EthType(0x4321) packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.mac_pkt2)
        verify_packets(self, self.exp_mac_pkt2, [self.dev_port27])

        print("Sending EthType(0x4321) packet - port 24 -> "
              "port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.mac_pkt2)
        verify_packets(self, self.exp_mac_pkt2, [self.dev_port25])

        # Verifying ACL counters
        print("Verifying ACL counters")
        time.sleep(10)
        counter = sai_thrift_get_acl_counter_attribute(
            client=self.client,
            acl_counter_oid=mac_mirror_acl_counter,
            packets=True)
        self.assertTrue(self.exp_counters["mirror_mac"] == counter["packets"],
                        "MAC ACL Mirror Counter Mismatch")

        self.trapAclCleanup()

        print("Sending Packets matching Mirror ACL rules on Port26. "
              "Packets should not be mirrored")
        print("Sending EthType(0x1234) packet - port 26 -> port 27 "
              "[access vlan=200])")
        send_packet(self, self.dev_port26, self.mac_pkt)
        verify_packets(self, self.exp_mac_pkt, [self.dev_port27])

        print("Sending EthType(0x4321) packet - port 24 -> "
              "port 25 [access vlan=100])")
        send_packet(self, self.dev_port24, self.mac_pkt2)
        verify_packets(self, self.exp_mac_pkt2, [self.dev_port25])

    def trapAclCleanup(self):
        """ Cleans up ACL"""
        print("Cleans up")
        for bp in self.bp_list:
            if bp in self.vlans:
                print("Unbinding ACL grp from Vlan")
                sai_thrift_set_vlan_attribute(
                    self.client, vlan_oid=bp, ingress_acl=0)
            else:
                sai_thrift_set_port_attribute(
                    self.client, bp, ingress_acl=0)
                print("Unbinding ACL grp from Port")
            self.bp_list.remove(bp)
        for trap in self.traps:
            sai_thrift_remove_hostif_trap(self.client, trap)
            self.traps.remove(trap)
        for acl_rule in self.acl_rules:
            action_counter_t = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0),
                enable=True)
            sai_thrift_set_acl_entry_attribute(
                self.client, acl_rule,
                action_counter=action_counter_t)
        for acl_counter in self.acl_counters:
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            self.acl_counters.remove(acl_counter)
        for acl_grp_member in self.acl_grp_members:
            sai_thrift_remove_acl_table_group_member(self.client,
                                                     acl_grp_member)
            self.acl_grp_members.remove(acl_grp_member)
        for acl_rule in self.acl_rules:
            sai_thrift_remove_acl_entry(self.client, acl_rule)
            self.acl_rules.remove(acl_rule)
        for acl_table in self.acl_tables:
            sai_thrift_remove_acl_table(self.client, acl_table)
            self.acl_tables.remove(acl_table)
        for acl_grp in self.acl_grps:
            sai_thrift_remove_acl_table_group(self.client, acl_grp)
            self.acl_grps.remove(acl_grp)


@disabled
class AclInOutTest(SaiHelper):
    """
    Simulates the ACL setup used in ACL community tests
    Creates an ACL group per port
    Creates 1 ACL tables for mirror types
    Creates an ACL group member per table and per group
    Adds entries in each of the tables and test ACLs work as expected

    Ingress port: port 0 to port 9
    Egress port : port10, port11, port13, port24 and port25
    Mirror port : port12
    """

    def setUp(self):
        super(AclInOutTest, self).setUp()

        self.dmac1 = '00:11:22:33:44:11'
        self.dmac2 = '00:11:22:33:44:22'
        self.src_mac = '00:22:22:22:22:22'
        self.ip_addr1 = '10.0.0.1'
        self.ip_addr2 = '10.0.0.2'
        self.ip_addr3 = '10.10.10.1'
        self.ip_addr4 = '10.10.10.2'

        # creating group list and group member list
        self.acl_ingress_groups_list = []
        self.acl_ingress_group_members_list = []
        self.acl_egress_groups_list = []
        self.acl_egress_group_members_list = []
        self.route_interface_list = []
        self.acl_table = None

        # port list for sending traffic (ingress port)
        self.ports_list = [self.port24, self.port25, self.port26, self.port27,
                           self.port28, self.port29, self.port30, self.port31]

        # creating nhop
        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr3),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.ip_addr4),
            router_interface_id=self.port11_rif,
            type=SAI_NEXT_HOP_TYPE_IP)

        # creating neighbor entry
        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress(self.ip_addr3))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry1, dst_mac_address=self.dmac1)

        self.neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.port11_rif, ip_address=sai_ipaddress(self.ip_addr4))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry2, dst_mac_address=self.dmac2)

        # creating route entry
        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.0.0.1/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry1, next_hop_id=self.nhop1)

        self.route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.0.0.2/32'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry2, next_hop_id=self.nhop2)

        self.route_entry3 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:99aa/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry3, next_hop_id=self.nhop2)

        self.route_entry4 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(
                '1234:5678:9abc:def0:4422:1133:5577:88aa/128'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry4, next_hop_id=self.nhop1)

        # ACL group config value
        group_stage_ingress = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        group_bind_point_type_list = sai_thrift_s32_list_t(
            count=len(group_bind_point_list), int32list=group_bind_point_list)

        # handling all ingress port
        for index, port in enumerate(self.ports_list):
            print("index: {}, port_handle: {}".format(index, port))

            # createing rif for all the port
            route_interface_id = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=port)

            print('route_interface_id: ', route_interface_id)
            self.route_interface_list.append(route_interface_id)

            # create ACL table group
            acl_table_group_ingress = sai_thrift_create_acl_table_group(
                self.client,
                acl_stage=group_stage_ingress,
                acl_bind_point_type_list=group_bind_point_type_list,
                type=group_type)

            self.acl_ingress_groups_list.append(acl_table_group_ingress)

            # bind this ACL group to ports object id
            print("Bind ACL ingress group 0x % lx to port 0x % lx" % (
                self.acl_ingress_groups_list[index], port))
            sai_thrift_set_port_attribute(
                self.client, port,
                ingress_acl=self.acl_ingress_groups_list[index])

    def runTest(self):
        try:
            self.testIPv4OutPortsMirrorAcl()
            self.testIPv6OutPortsMirrorAcl()
            self.testIPv4InPortsMirrorAcl()
            self.testIPv6InPortsMirrorAcl()
            self.testMaxInPortsEntry()
        finally:
            pass

    def testIPv6InPortsMirrorAcl(self):
        '''
        Test various IP mirror functionality
        '''
        print("\ntestIPv6InPortsMirrorAcl")
        pkt = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst=self.dmac2,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=63)

        try:
            acl_ipv6_ingress_group_members_list = []
            # ACL table config value
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_INGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)

            print("Create ACL tables for IPv6 traffic")
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_in_ports=True)

            # create ACL ingress table group members
            print("Creates ACL group members")
            for acl_group in self.acl_ingress_groups_list:
                acl_group_ingress_member_id = \
                    sai_thrift_create_acl_table_group_member(
                        self.client,
                        acl_table_group_id=acl_group,
                        acl_table_id=acl_table,
                        priority=1)

                acl_ipv6_ingress_group_members_list.append(
                    acl_group_ingress_member_id)

            # verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt)
            verify_packet(self, exp_pkt, self.dev_port11)
            verify_no_packet(self, pkt, self.dev_port12, timeout=2)

            # create dest_ip_t o match ACL entry
            dst_ip = '1234:5678:9abc:def0:4422:1133:5577:99aa'
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))

            # creating a local mirror session
            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port12,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))

            # creating in_ports
            in_ports = [self.port24, self.port25, self.port26]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            acl_entry_ipv6_mirror = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ipv6=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)

            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            # attach ACL counter to ACL entry
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter), enable=True)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_ipv6_mirror,
                action_counter=action_counter)

            # sending traffic to port with in-ports list
            print("\nTraffic using Port24")
            send_packet(self, self.dev_port24, pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, pkt],
                                            [self.dev_port11, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 1)

            print("\nTraffic using Port25")
            send_packet(self, self.dev_port25, pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, pkt],
                                            [self.dev_port11, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 2)

            print("\nTraffic using Port26")
            send_packet(self, self.dev_port26, pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, pkt],
                                            [self.dev_port11, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 3)

            print("\nTraffic though non in-ports port: port27")
            send_packet(self, self.dev_port27, pkt)
            verify_packet(self, exp_pkt, self.dev_port11)
            verify_no_packet(self, pkt, self.dev_port12, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 3)

        finally:
            print("removing acl entry counter")
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0), enable=True)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry_ipv6_mirror,
                action_counter=action_counter)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            sai_thrift_remove_acl_counter(self.client, acl_counter)

            print("removing acl etntry for ingress ipv6 mirror acl")
            sai_thrift_remove_acl_entry(self.client, acl_entry_ipv6_mirror)

            print("Removing ACL group members")
            for acl_group_member in acl_ipv6_ingress_group_members_list:
                sai_thrift_remove_acl_table_group_member(self.client,
                                                         acl_group_member)

            print("Removing ACL table")
            sai_thrift_remove_acl_table(self.client, acl_table)

    def testIPv6OutPortsMirrorAcl(self):
        '''
        Test various Out Ports mirror ACL functionality
        '''
        print("\ntestIPv6OutPortsMirrorAcl")
        pkt = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst=self.dmac2,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_hlim=63)

        pkt2 = simple_tcpv6_packet(
            eth_dst=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
            ipv6_hlim=64)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst=self.dmac1,
            eth_src=ROUTER_MAC,
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
            ipv6_hlim=63)
        acl_ipv6_egress_groups_list = []
        acl_ipv6_egress_group_members_list = []
        try:
            # ACL group config value
            group_stage_egress = SAI_ACL_STAGE_EGRESS
            group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
            group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

            group_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(group_bind_point_list),
                int32list=group_bind_point_list)

            # Egress port list
            e_ports_list = [self.port10, self.port11, self.port13]

            # handling all egress port
            for index, port in enumerate(e_ports_list):
                print("index: {}, port_handle: {}".format(index, port))

                # create ACL table group
                acl_table_group_egress = sai_thrift_create_acl_table_group(
                    self.client,
                    acl_stage=group_stage_egress,
                    acl_bind_point_type_list=group_bind_point_type_list,
                    type=group_type)

                acl_ipv6_egress_groups_list.append(acl_table_group_egress)

                # bind this ACL group to ports object id
                print("Bind ACL egress group 0x % lx to port 0x % lx" % (
                    acl_ipv6_egress_groups_list[index], port))
                sai_thrift_set_port_attribute(
                    self.client, port,
                    egress_acl=acl_ipv6_egress_groups_list[index])

            # ACL table config value
            table_stage = SAI_ACL_STAGE_EGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_EGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)

            print("Create Egress Mirror ACL tables")
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ipv6=True,
                field_dst_ipv6=True,
                field_out_ports=True)

            # create ACL egress table group members
            print("Creates ACL group members")
            for acl_group in acl_ipv6_egress_groups_list:
                acl_group_egress_member_id = \
                    sai_thrift_create_acl_table_group_member(
                        self.client,
                        acl_table_group_id=acl_group,
                        acl_table_id=acl_table,
                        priority=1)

                acl_ipv6_egress_group_members_list.append(
                    acl_group_egress_member_id)

            # verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt)
            verify_packet(self, exp_pkt, self.dev_port11)
            verify_no_packet(self, pkt, self.dev_port12, timeout=2)

            # create dest_ip_t o match ACL entry
            dst_ip = '1234:5678:9abc:def0:4422:1133:5577:99aa'
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))

            dst_ip2 = '1234:5678:9abc:def0:4422:1133:5577:88aa'
            dst_ip2_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip2_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=dst_ip2),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip2_mask))

            # creating a local mirror session
            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port12,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))

            # creating out_ports
            out_ports = [self.port11, self.port10]
            out_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(out_ports),
                        idlist=out_ports)))

            # creating the ACL entry to mirror the traffic
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=1,
                field_dst_ipv6=dst_ip_t,
                field_out_ports=out_ports_list_t,
                action_mirror_egress=mirror_action)

            acl_entry2 = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=1,
                field_dst_ipv6=dst_ip2_t,
                field_out_ports=out_ports_list_t,
                action_mirror_egress=mirror_action)

            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            # attach ACL counter to ACL entry
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter), enable=True)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry2,
                action_counter=action_counter)

            # sending traffic to port with in-ports list
            print("\nchecking 1st port in out-port list for mirror ACL")
            send_packet(self, self.dev_port24, pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, exp_pkt],
                                            [self.dev_port11, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 1)

            send_packet(self, self.dev_port24, pkt2)
            verify_each_packet_on_each_port(self, [exp_pkt2, exp_pkt2],
                                            [self.dev_port10, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 2)

        finally:
            print("removing acl entry counter")
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0), enable=True)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry2,
                action_counter=action_counter)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            sai_thrift_remove_acl_counter(self.client, acl_counter)

            print("removing acl etntry for egress mirror acl")
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_entry(self.client, acl_entry2)

            print("Unbind Egress ACL groups from ports object id")
            for ports in e_ports_list:
                sai_thrift_set_port_attribute(
                    self.client, ports,
                    egress_acl=int(SAI_NULL_OBJECT_ID))

            print("Removing Egress ACL group members")
            for acl_group_member in acl_ipv6_egress_group_members_list:
                sai_thrift_remove_acl_table_group_member(self.client,
                                                         acl_group_member)
            print("Removing Egress ACL table")
            sai_thrift_remove_acl_table(self.client, acl_table)

            print("Removing EgressACL groups")
            for acl_group in acl_ipv6_egress_groups_list:
                sai_thrift_remove_acl_table_group(self.client, acl_group)

    def testIPv4OutPortsMirrorAcl(self):
        '''
        Test various Out Ports mirror ACL functionality
        '''
        print("\ntestIPv4OutPortsMirrorAcl")
        pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                eth_src=self.src_mac,
                                ip_dst=self.ip_addr2,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst=self.dmac2,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.ip_addr2,
                                    ip_ttl=63)
        pkt_2 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                  eth_src=self.src_mac,
                                  ip_dst=self.ip_addr1,
                                  ip_ttl=64)
        exp_pkt_2 = simple_tcp_packet(eth_dst=self.dmac1,
                                      eth_src=ROUTER_MAC,
                                      ip_dst=self.ip_addr1,
                                      ip_ttl=63)
        try:
            # ACL group config value
            group_stage_egress = SAI_ACL_STAGE_EGRESS
            group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
            group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

            group_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(group_bind_point_list),
                int32list=group_bind_point_list)

            # Egress port list
            e_ports_list = [self.port10, self.port11, self.port13]

            # handling all egress port
            for index, port in enumerate(e_ports_list):
                print("index: {}, port_handle: {}".format(index, port))

                # create ACL table group
                acl_table_group_egress = sai_thrift_create_acl_table_group(
                    self.client,
                    acl_stage=group_stage_egress,
                    acl_bind_point_type_list=group_bind_point_type_list,
                    type=group_type)

                self.acl_egress_groups_list.append(acl_table_group_egress)

                # bind this ACL group to ports object id
                print("Bind ACL egress group 0x % lx to port 0x % lx" % (
                    self.acl_egress_groups_list[index], port))
                sai_thrift_set_port_attribute(
                    self.client, port,
                    egress_acl=self.acl_egress_groups_list[index])

            # ACL table config value
            table_stage = SAI_ACL_STAGE_EGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_EGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)

            print("Create Egress Mirror ACL tables")
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ip=True,
                field_dst_ip=True,
                field_out_ports=True)

            # create ACL egress table group members
            print("Creates ACL group members")
            for acl_group in self.acl_egress_groups_list:
                acl_group_egress_member_id = \
                    sai_thrift_create_acl_table_group_member(
                        self.client,
                        acl_table_group_id=acl_group,
                        acl_table_id=acl_table,
                        priority=1)

                self.acl_egress_group_members_list.append(
                    acl_group_egress_member_id)

            # verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt)
            verify_packet(self, exp_pkt, self.dev_port11)
            verify_no_packet(self, pkt, self.dev_port12, timeout=2)

            # create dest_ip_t o match ACL entry
            dst_ip_mask = '255.255.255.255'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=self.ip_addr2),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))

            dst_ip2_mask = '255.255.255.255'
            dst_ip2_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=self.ip_addr1),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip2_mask))

            # creating a local mirror session
            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port12,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))

            # creating out_ports
            out_ports = [self.port11, self.port10]
            out_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(out_ports),
                        idlist=out_ports)))

            print("Going to create the acl entry")
            # creating the ACL entry to mirror the traffic
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=1,
                field_dst_ip=dst_ip_t,
                field_out_ports=out_ports_list_t,
                action_mirror_egress=mirror_action)

            acl_entry2 = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=1,
                field_dst_ip=dst_ip2_t,
                field_out_ports=out_ports_list_t,
                action_mirror_egress=mirror_action)

            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=acl_table)

            # attach ACL counter to ACL entry
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter), enable=True)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry2,
                action_counter=action_counter)

            # sending traffic to port with in-ports list
            print("\nchecking 1st port in out-port list for mirror ACL")
            send_packet(self, self.dev_port24, pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, exp_pkt],
                                            [self.dev_port11, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 1)

            send_packet(self, self.dev_port24, pkt_2)
            verify_each_packet_on_each_port(self, [exp_pkt_2, exp_pkt_2],
                                            [self.dev_port10, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 2)

        finally:
            print("removing acl entry counter")
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0), enable=True)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry2,
                action_counter=action_counter)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            sai_thrift_remove_acl_counter(self.client, acl_counter)

            print("removing acl etntry for egress mirror acl")
            sai_thrift_remove_acl_entry(self.client, acl_entry)
            sai_thrift_remove_acl_entry(self.client, acl_entry2)

            print("Unbind Egress ACL groups from ports object id")
            for ports in e_ports_list:
                sai_thrift_set_port_attribute(
                    self.client, ports,
                    egress_acl=int(SAI_NULL_OBJECT_ID))

            print("Removing Egress ACL group members")
            for acl_group_member in self.acl_egress_group_members_list:
                sai_thrift_remove_acl_table_group_member(self.client,
                                                         acl_group_member)
            print("Removing Egress ACL table")
            sai_thrift_remove_acl_table(self.client, acl_table)

            print("Removing EgressACL groups")
            for acl_group in self.acl_egress_groups_list:
                sai_thrift_remove_acl_table_group(self.client, acl_group)

    def testMaxInPortsEntry(self):
        '''
        Test Maximum in-ports ACL entry create
        '''
        print("\ntestMaxInPortsEntry\n")
        try:
            # ACL table config value
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_INGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)

            print("Create ACL tables")
            acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ip=True,
                field_dst_ip=True,
                field_ttl=True,
                field_in_ports=True)

            # storing the acl entry list
            acl_table_entry_list = []

            # create dest_ip_t o match ACL entry
            dst_ip_mask = '255.255.255.255'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=self.ip_addr2),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))

            # creating a local mirror session
            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port12,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))

            # creating in_ports
            in_ports = [self.port24, self.port25, self.port26]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 1
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)
            acl_table_entry_list.append(acl_entry)

            # creating in_ports
            in_ports = [self.port24, self.port28, self.port26]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 2
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)
            acl_table_entry_list.append(acl_entry)

            # creating in_ports
            in_ports = [self.port24]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 3
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)
            acl_table_entry_list.append(acl_entry)

            # creating in_ports
            in_ports = [self.port28]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 4
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)
            acl_table_entry_list.append(acl_entry)

            # creating in_ports
            in_ports = [self.port26]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 5
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)
            acl_table_entry_list.append(acl_entry)

            # creating in_ports
            in_ports = [self.port24, self.port28]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 6
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)
            acl_table_entry_list.append(acl_entry)

            # creating in_ports
            in_ports = [self.port26, self.port28]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 7
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)
            acl_table_entry_list.append(acl_entry)

            # creating in_ports
            in_ports = [self.port26, self.port24]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 8
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)
            acl_table_entry_list.append(acl_entry)

            # creating in_ports
            in_ports = [self.port25, self.port28]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            # 9
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)

            print("acl_entry: ", acl_entry)
            self.assertEqual(acl_entry, 0.0)

        finally:
            print("removing acl etntry for ingress ipv4 mirror acl")
            for acl_entry_p in acl_table_entry_list:
                print("Cleanup, acl_entry_p", acl_entry_p)
                sai_thrift_remove_acl_entry(self.client, acl_entry_p)

            print("Removing ACL table")
            sai_thrift_remove_acl_table(self.client, acl_table)

    def testIPv4InPortsMirrorAcl(self):
        '''
        Test various IP mirror functionality
        '''
        print("\ntestIPv4InPortsMirrorAcl")
        pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                eth_src=self.src_mac,
                                ip_dst=self.ip_addr2,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst=self.dmac2,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.ip_addr2,
                                    ip_ttl=63)
        try:
            # ACL table config value
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
            table_bind_point_type_list = sai_thrift_s32_list_t(
                count=len(table_bind_points), int32list=table_bind_points)
            actions = [SAI_ACL_ACTION_TYPE_MIRROR_INGRESS]
            action_type_list = sai_thrift_s32_list_t(
                count=len(actions), int32list=actions)

            print("Create ACL tables")
            self.acl_table = sai_thrift_create_acl_table(
                self.client,
                acl_stage=table_stage,
                acl_bind_point_type_list=table_bind_point_type_list,
                acl_action_type_list=action_type_list,
                field_src_ip=True,
                field_dst_ip=True,
                field_ttl=True,
                field_in_ports=True)

            # create ACL ingress table group members
            print("Creates ACL group members")
            for acl_group in self.acl_ingress_groups_list:
                acl_group_ingress_member_id = \
                    sai_thrift_create_acl_table_group_member(
                        self.client,
                        acl_table_group_id=acl_group,
                        acl_table_id=self.acl_table,
                        priority=1)

                self.acl_ingress_group_members_list.append(
                    acl_group_ingress_member_id)

            # verify packet forwarding without ACL
            send_packet(self, self.dev_port24, pkt)
            verify_packet(self, exp_pkt, self.dev_port11)
            verify_no_packet(self, pkt, self.dev_port12, timeout=2)

            # create dest_ip_t o match ACL entry
            dst_ip_mask = '255.255.255.255'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=self.ip_addr2),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))

            # creating a local mirror session
            mirror_session = sai_thrift_create_mirror_session(
                self.client,
                monitor_port=self.port12,
                type=SAI_MIRROR_SESSION_TYPE_LOCAL)

            mirror_session_list = sai_thrift_object_list_t(
                count=1, idlist=[mirror_session])
            mirror_action = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    objlist=mirror_session_list))

            # creating in_ports
            in_ports = [self.port24, self.port25, self.port26]
            in_ports_list_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(
                    objlist=sai_thrift_object_list_t(
                        count=len(in_ports),
                        idlist=in_ports)))

            # creating the ACL entry to mirror the traffic
            acl_entry = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.acl_table,
                priority=10,
                field_dst_ip=dst_ip_t,
                field_in_ports=in_ports_list_t,
                action_mirror_ingress=mirror_action)

            # create ACL counter
            acl_counter = sai_thrift_create_acl_counter(
                self.client, table_id=self.acl_table)

            # attach ACL counter to ACL entry
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=acl_counter), enable=True)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter)

            # sending traffic to port with in-ports list
            print("\nTraffic using Port24")
            send_packet(self, self.dev_port24, pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, pkt],
                                            [self.dev_port11, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 1)

            print("\nTraffic using Port25")
            send_packet(self, self.dev_port25, pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, pkt],
                                            [self.dev_port11, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 2)

            print("\nTraffic using Port26")
            send_packet(self, self.dev_port26, pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, pkt],
                                            [self.dev_port11, self.dev_port12])

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 3)

            print("\nTraffic though non in-ports port: port27")
            send_packet(self, self.dev_port27, pkt)
            verify_packet(self, exp_pkt, self.dev_port11)
            verify_no_packet(self, pkt, self.dev_port12, timeout=2)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)

            print("packets['packets']: ", packets['packets'])
            self.assertEqual(packets['packets'], 3)

        finally:
            print("removing acl entry counter")
            action_counter = sai_thrift_acl_action_data_t(
                parameter=sai_thrift_acl_action_parameter_t(
                    oid=0), enable=True)

            sai_thrift_set_acl_entry_attribute(
                self.client, acl_entry,
                action_counter=action_counter)

            packets = sai_thrift_get_acl_counter_attribute(
                self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            sai_thrift_remove_acl_counter(self.client, acl_counter)

            print("removing acl etntry for ingress ipv4 mirror acl")
            sai_thrift_remove_acl_entry(self.client, acl_entry)

            print("cleanup mirror session")
            sai_thrift_remove_mirror_session(self.client, mirror_session)

            print("Removing ACL group members")
            for acl_group_member in self.acl_ingress_group_members_list:
                sai_thrift_remove_acl_table_group_member(self.client,
                                                         acl_group_member)

            print("Removing ACL table")
            sai_thrift_remove_acl_table(self.client, self.acl_table)

    def tearDown(self):

        # cleaning route entries
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_route_entry(self.client, self.route_entry2)
        sai_thrift_remove_route_entry(self.client, self.route_entry3)
        sai_thrift_remove_route_entry(self.client, self.route_entry4)

        # cleaning next hop entries
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_next_hop(self.client, self.nhop2)

        # cleaning neighbor entries
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry2)

        # cleaning rif
        for port_rif in self.route_interface_list:
            sai_thrift_remove_router_interface(self.client, port_rif)

        print("Unbind ACL groups from ports object id")
        for ports in self.ports_list:
            sai_thrift_set_port_attribute(
                self.client, ports,
                ingress_acl=int(SAI_NULL_OBJECT_ID))

        print("Removing ACL groups")
        for acl_group in self.acl_ingress_groups_list:
            sai_thrift_remove_acl_table_group(self.client, acl_group)

        # cleanup ACL
        super(AclInOutTest, self).tearDown()
