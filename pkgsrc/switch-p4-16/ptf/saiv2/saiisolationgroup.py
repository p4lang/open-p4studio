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
Thrift SAI interface Port Isolation tests
"""

from sai_base_test import *


class PortIsolationTest(SaiHelper):
    """
    The class runs VLAN test cases
    """

    def setUp(self):
        super(PortIsolationTest, self).setUp()
        self.mac11 = '00:1b:11:11:11:11'
        self.mac22 = '00:1b:22:22:22:22'
        self.mac33 = '00:1b:33:33:33:33'

        # create ingress RIFs, use port10_rif and port24_rif as ingress
        self.port24_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)

        # create nhop for port11_rif
        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.0.2'),
            router_interface_id=self.port11_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port11_rif, ip_address=sai_ipaddress('10.10.0.2'))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.neighbor_entry1,
                                         dst_mac_address=self.mac11)

        self.nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.2'),
            router_interface_id=self.port12_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.port12_rif, ip_address=sai_ipaddress('20.20.0.2'))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.neighbor_entry2,
                                         dst_mac_address=self.mac22)

        self.nhop3 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('30.30.0.3'),
            router_interface_id=self.port13_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry3 = sai_thrift_neighbor_entry_t(
            rif_id=self.port13_rif, ip_address=sai_ipaddress('30.30.0.3'))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.neighbor_entry3,
                                         dst_mac_address=self.mac33)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(self.client,
                                      self.route_entry1,
                                      next_hop_id=self.nhop1)

        self.route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('20.20.20.1/32'))
        sai_thrift_create_route_entry(self.client,
                                      self.route_entry2,
                                      next_hop_id=self.nhop2)

        self.route_entry3 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('30.30.30.1/32'))
        sai_thrift_create_route_entry(self.client,
                                      self.route_entry3,
                                      next_hop_id=self.nhop3)

        self.isol_group1 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_PORT)
        self.isolation_group_mbr1 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group1,
            isolation_object=self.port11)
        self.isolation_group_mbr2 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group1,
            isolation_object=self.port12)

        self.isol_group2 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_PORT)
        self.isolation_group_mbr3 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group2,
            isolation_object=self.port12)
        self.isolation_group_mbr4 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group2,
            isolation_object=self.port13)

    def runTest(self):
        try:
            self.attributeTest()
            self.forwardingTest()
            self.isolationGroupMemberTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isolation_group_mbr4)
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isolation_group_mbr3)
        sai_thrift_remove_isolation_group(self.client, self.isol_group2)
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isolation_group_mbr2)
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isolation_group_mbr1)
        sai_thrift_remove_isolation_group(self.client, self.isol_group1)

        sai_thrift_remove_route_entry(self.client, self.route_entry3)
        sai_thrift_remove_route_entry(self.client, self.route_entry2)
        sai_thrift_remove_route_entry(self.client, self.route_entry1)

        sai_thrift_remove_next_hop(self.client, self.nhop3)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry3)
        sai_thrift_remove_next_hop(self.client, self.nhop2)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry2)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry1)

        sai_thrift_remove_router_interface(self.client, self.port24_rif)
        super(PortIsolationTest, self).tearDown()

    def attributeTest(self):
        """
        Verify isolation group crud operations
        """
        print("attributeTest")
        attrs = sai_thrift_get_isolation_group_attribute(self.client,
                                                         self.isol_group1,
                                                         type=True)
        self.assertEqual(attrs["type"], SAI_ISOLATION_GROUP_TYPE_PORT)

        attrs = sai_thrift_get_isolation_group_member_attribute(
            self.client,
            self.isolation_group_mbr1,
            isolation_group_id=True,
            isolation_object=True)
        self.assertEqual(attrs["isolation_group_id"], self.isol_group1)
        self.assertEqual(attrs["isolation_object"], self.port11)

    def forwardingTest(self):
        """
        Forwarding between ports with isolation groups attached
        Ingress ports: port10 and port24
        Isolation groups:
          grp1: port 11 and port12
          grp2: port 12 and port13
        """
        print("forwardingTest")
        try:
            pkt1 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     ip_dst='10.10.10.1',
                                     ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(eth_dst=self.mac11,
                                         eth_src=ROUTER_MAC,
                                         ip_dst='10.10.10.1',
                                         ip_ttl=63)
            pkt2 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     ip_dst='20.20.20.1',
                                     ip_ttl=64)
            exp_pkt2 = simple_tcp_packet(eth_dst=self.mac22,
                                         eth_src=ROUTER_MAC,
                                         ip_dst='20.20.20.1',
                                         ip_ttl=63)
            pkt3 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     ip_dst='30.30.30.1',
                                     ip_ttl=64)
            exp_pkt3 = simple_tcp_packet(eth_dst=self.mac33,
                                         eth_src=ROUTER_MAC,
                                         ip_dst='30.30.30.1',
                                         ip_ttl=63)

            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port12)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            print("Sending packet from port %d" % self.dev_port24)
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port12)
            send_packet(self, self.dev_port24, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            # attach isolation groups
            print("Attach isolation groups to ingress ports")
            sai_thrift_set_port_attribute(
                self.client,
                self.port10,
                isolation_group=self.isol_group1)
            sai_thrift_set_port_attribute(
                self.client,
                self.port24,
                isolation_group=self.isol_group2)

            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port10, pkt2)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            print("Sending packet from port %d" % self.dev_port24)
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port24, pkt2)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, pkt3)
            verify_no_other_packets(self, timeout=1)
            # detach isolation groups
            print("Detach isolation groups from ingress ports")
            sai_thrift_set_port_attribute(self.client,
                                          self.port10,
                                          isolation_group=int(
                                              SAI_NULL_OBJECT_ID))
            sai_thrift_set_port_attribute(self.client,
                                          self.port24,
                                          isolation_group=int(
                                              SAI_NULL_OBJECT_ID))
            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port12)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            print("Sending packet from port %d" % self.dev_port24)
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port12)
            send_packet(self, self.dev_port24, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

        finally:
            sai_thrift_set_port_attribute(self.client,
                                          self.port10,
                                          isolation_group=int(
                                              SAI_NULL_OBJECT_ID))
            sai_thrift_set_port_attribute(self.client,
                                          self.port24,
                                          isolation_group=int(
                                              SAI_NULL_OBJECT_ID))

    def isolationGroupMemberTest(self):
        """
        Forwarding between ports with isolation groups attached
        Ingress ports: port10 and port24
        Isolation groups:
          grp3: port 12 and port13
        """
        print("isolationGroupMemberTest")

        isol_group3 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_PORT)

        try:
            pkt1 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     ip_dst='10.10.10.1',
                                     ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(eth_dst=self.mac11,
                                         eth_src=ROUTER_MAC,
                                         ip_dst='10.10.10.1',
                                         ip_ttl=63)
            pkt2 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     ip_dst='20.20.20.1',
                                     ip_ttl=64)
            exp_pkt2 = simple_tcp_packet(eth_dst=self.mac22,
                                         eth_src=ROUTER_MAC,
                                         ip_dst='20.20.20.1',
                                         ip_ttl=63)
            pkt3 = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                     ip_dst='30.30.30.1',
                                     ip_ttl=64)
            exp_pkt3 = simple_tcp_packet(eth_dst=self.mac33,
                                         eth_src=ROUTER_MAC,
                                         ip_dst='30.30.30.1',
                                         ip_ttl=63)

            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port12)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            # attach isolation groups
            print("Attach empty isolation group to ingress port")
            sai_thrift_set_port_attribute(
                self.client,
                self.port10,
                isolation_group=isol_group3)

            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port12)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            print("Add port11 to the isolation group")
            isolation_group3_mbr1 = sai_thrift_create_isolation_group_member(
                self.client,
                isolation_group_id=isol_group3,
                isolation_object=self.port11)

            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port12)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            print("Add port12 to the isolation group")
            isolation_group3_mbr2 = sai_thrift_create_isolation_group_member(
                self.client,
                isolation_group_id=isol_group3,
                isolation_object=self.port12)

            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port10, pkt2)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            print("Remove port11 from the isolation group")
            sai_thrift_remove_isolation_group_member(self.client,
                                                     isolation_group3_mbr1)
            isolation_group3_mbr1 = False

            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port10, pkt2)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

            print("Remove port12 from the isolation group")
            sai_thrift_remove_isolation_group_member(self.client,
                                                     isolation_group3_mbr2)
            isolation_group3_mbr2 = False

            print("Sending packet from port %d" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port11)
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port12)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port13)

        finally:
            if isolation_group3_mbr1:
                sai_thrift_remove_isolation_group_member(self.client,
                                                         isolation_group3_mbr1)
            if isolation_group3_mbr2:
                sai_thrift_remove_isolation_group_member(self.client,
                                                         isolation_group3_mbr2)

            sai_thrift_set_port_attribute(self.client,
                                          self.port10,
                                          isolation_group=int(
                                              SAI_NULL_OBJECT_ID))

            sai_thrift_remove_isolation_group(self.client, isol_group3)


class BridgePortIsolationTest(SaiHelper):
    """
    The class runs bridge port isolation group test
    """

    def setUp(self):
        super(BridgePortIsolationTest, self).setUp()

        self.port24_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port24_bp != 0)

        self.vlan10_member3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.isol_group1 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_BRIDGE_PORT)
        self.isol_group2 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_BRIDGE_PORT)
        self.isol_group3 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_BRIDGE_PORT)

        self.isol_group1_mbr_lag1 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group1,
            isolation_object=self.lag1_bp)

        self.isol_group2_mbr_p24 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group2,
            isolation_object=self.port24_bp)

        self.isol_group3_mbr_lag1 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group3,
            isolation_object=self.lag1_bp)
        self.isol_group3_mbr_p24 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group3,
            isolation_object=self.port24_bp)

    def runTest(self):
        try:
            self.attributeTest()
            self.forwardingTest()
            self.isolationGroupMemberTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isol_group1_mbr_lag1)
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isol_group2_mbr_p24)
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isol_group3_mbr_lag1)
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isol_group3_mbr_p24)

        sai_thrift_remove_isolation_group(self.client, self.isol_group1)
        sai_thrift_remove_isolation_group(self.client, self.isol_group2)
        sai_thrift_remove_isolation_group(self.client, self.isol_group3)

        sai_thrift_remove_vlan_member(self.client, self.vlan10_member3)
        sai_thrift_remove_bridge_port(self.client, self.port24_bp)

        super(BridgePortIsolationTest, self).tearDown()

    def attributeTest(self):
        """
        Verify isolation group crud operations
        """
        print("attributeTest")
        try:
            attrs = sai_thrift_get_isolation_group_attribute(
                self.client,
                self.isol_group1,
                type=True)

            self.assertEqual(attrs["type"],
                             SAI_ISOLATION_GROUP_TYPE_BRIDGE_PORT)

            attrs = sai_thrift_get_isolation_group_member_attribute(
                self.client,
                self.isol_group1_mbr_lag1,
                isolation_group_id=True,
                isolation_object=True)

            self.assertEqual(attrs["isolation_group_id"], self.isol_group1)
            self.assertEqual(attrs["isolation_object"], self.lag1_bp)

            attrs = sai_thrift_get_bridge_port_attribute(self.client,
                                                         self.port1_bp,
                                                         isolation_group=True)
            sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port1_bp,
                isolation_group=self.isol_group1)
            attrs = sai_thrift_get_bridge_port_attribute(self.client,
                                                         self.port1_bp,
                                                         isolation_group=True)
        finally:
            sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port1_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))

    def forwardingTest(self):
        """
        Forwarding between ports with isolation groups attached
        Ingress ports: port0
        Isolation groups:
          grp1: lag6
          grp2: port24
          grp3: lag6 and port24
        """
        print("\nForwardingTest()")

        vlan_id = 10
        src_mac = "00:11:11:11:11:11"
        dst_mac = "00:22:22:22:22:22"

        pkt = simple_udp_packet(eth_dst=dst_mac,
                                eth_src=src_mac,
                                pktlen=100)
        tag_pkt = simple_udp_packet(eth_dst=dst_mac,
                                    eth_src=src_mac,
                                    dl_vlan_enable=True,
                                    vlan_vid=vlan_id,
                                    pktlen=104)

        try:
            print("Sending packet on port %d, %s -> %s - will flood" %
                  (self.dev_port0, src_mac, dst_mac))
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, pkt],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port24]])

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=self.isol_group1)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt],
                [[self.dev_port1],
                 [self.dev_port24]])

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=self.isol_group2)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6]])

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=self.isol_group3)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt],
                [[self.dev_port1]])

            print("Verification complete")

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

    def isolationGroupMemberTest(self):
        """
        Forwarding between ports with isolation groups attached
        Ingress ports: port0
        Isolation groups:
          grp4: lag6 and port24
        """
        print("IsolationGroupMemberTest()")

        vlan_id = 10
        src_mac = "00:11:11:11:11:11"
        dst_mac = "00:22:22:22:22:22"

        pkt = simple_udp_packet(eth_dst=dst_mac,
                                eth_src=src_mac,
                                pktlen=100)
        tag_pkt = simple_udp_packet(eth_dst=dst_mac,
                                    eth_src=src_mac,
                                    dl_vlan_enable=True,
                                    vlan_vid=vlan_id,
                                    pktlen=104)

        isol_group4 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_BRIDGE_PORT)

        try:
            print("Sending packet on port %d, %s -> %s - will flood" %
                  (self.dev_port0, src_mac, dst_mac))
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, pkt],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port24]])

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=isol_group4)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Attach empty isolation group to the port %d" %
                  self.dev_port0)
            print("Sending packet on port %d, %s -> %s - will flood" %
                  (self.dev_port0, src_mac, dst_mac))
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, pkt],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port24]])

            print("Add lag1 to the isolation group")
            isol_group4_mbr_lag1 = sai_thrift_create_isolation_group_member(
                self.client,
                isolation_group_id=isol_group4,
                isolation_object=self.lag1_bp)

            print("Sending packet on port %d, %s -> %s - will flood" %
                  (self.dev_port0, src_mac, dst_mac))
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt],
                [[self.dev_port1],
                 [self.dev_port24]])

            print("Add port %d to the isolation group" % self.dev_port24)
            isol_group4_mbr_p24 = sai_thrift_create_isolation_group_member(
                self.client,
                isolation_group_id=isol_group4,
                isolation_object=self.port24_bp)

            print("Sending packet on port %d, %s -> %s - will flood" %
                  (self.dev_port0, src_mac, dst_mac))
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt],
                [[self.dev_port1]])

            print("Remove lag1 from the isolation group")
            sai_thrift_remove_isolation_group_member(self.client,
                                                     isol_group4_mbr_lag1)
            isol_group4_mbr_lag1 = False

            print("Sending packet on port %d, %s -> %s - will flood" %
                  (self.dev_port0, src_mac, dst_mac))
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6]])

            print("Remove port %d from the isolation group" % self.dev_port24)
            sai_thrift_remove_isolation_group_member(self.client,
                                                     isol_group4_mbr_p24)
            isol_group4_mbr_p24 = False

            print("Sending packet on port %d, %s -> %s - will flood" %
                  (self.dev_port0, src_mac, dst_mac))
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, pkt],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port24]])

            print("Verification complete")

        finally:
            if isol_group4_mbr_lag1:
                sai_thrift_remove_isolation_group_member(self.client,
                                                         isol_group4_mbr_lag1)
            if isol_group4_mbr_p24:
                sai_thrift_remove_isolation_group_member(self.client,
                                                         isol_group4_mbr_p24)

            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sai_thrift_remove_isolation_group(self.client, isol_group4)


class CombinedIsolationTest(SaiHelper):
    """
    The class runs combined isolation group test
    """

    def setUp(self):
        super(CombinedIsolationTest, self).setUp()

        # create LAGs
        self.lag6 = sai_thrift_create_lag(self.client)
        self.assertTrue(self.lag6 != 0)
        self.lag6_member0 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag6, port_id=self.port24)
        self.lag6_member1 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag6, port_id=self.port25)

        self.lag7 = sai_thrift_create_lag(self.client)
        self.assertTrue(self.lag7 != 0)
        self.lag7_member0 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag7, port_id=self.port27)
        self.lag7_member1 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag7, port_id=self.port28)

        self.lag6_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.lag6,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.lag6_bp != 0)

        self.lag7_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.lag7,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.lag7_bp != 0)

        self.port26_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port26,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port26_bp != 0)

        self.port27_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port27,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port27_bp != 0)

        self.port29_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port29,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port29_bp != 0)

        self.vlan100 = sai_thrift_create_vlan(self.client, vlan_id=100)
        self.assertTrue(self.vlan100 != 0)
        self.vlan100_member0 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan100,
            bridge_port_id=self.lag6_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.vlan100_member1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan100,
            bridge_port_id=self.port26_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.vlan100_member2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan100,
            bridge_port_id=self.lag7_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.vlan100_member3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan100,
            bridge_port_id=self.port29_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # setup untagged ports
        sai_thrift_set_lag_attribute(self.client,
                                     self.lag6,
                                     port_vlan_id=100)
        sai_thrift_set_lag_attribute(self.client,
                                     self.lag7,
                                     port_vlan_id=100)
        sai_thrift_set_port_attribute(self.client,
                                      self.port26,
                                      port_vlan_id=100)
        sai_thrift_set_port_attribute(self.client,
                                      self.port29,
                                      port_vlan_id=100)

        self.rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            virtual_router_id=self.default_vrf,
            vlan_id=self.vlan100)

        self.rif_mac = ROUTER_MAC
        self.lag6_mac = '00:11:11:11:11:11'
        self.port26_mac = '00:22:22:22:22:22'
        self.lag7_mac = '00:33:33:33:33:33'
        self.port29_mac = '00:44:44:44:44:44'

        self.rif_ip = '1.1.1.100'
        self.lag6_ip = '1.1.1.10'
        self.port26_ip = '1.1.1.20'
        self.lag7_ip = '1.1.1.30'
        self.port29_ip = '1.1.1.40'

        self.nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.rif_ip),
            router_interface_id=self.rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry_100 = sai_thrift_neighbor_entry_t(
            rif_id=self.rif, ip_address=sai_ipaddress(self.rif_ip))
        sai_thrift_create_neighbor_entry(
            self.client, self.neighbor_entry_100, dst_mac_address=self.rif_mac)

        self.route_entry0 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('1.1.1.0/24'))
        sai_thrift_create_route_entry(
            self.client, self.route_entry0, next_hop_id=self.nhop1)

        self.neighbor_entry_10 = sai_thrift_neighbor_entry_t(
            rif_id=self.rif, ip_address=sai_ipaddress(self.lag6_ip))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.neighbor_entry_10,
                                         dst_mac_address=self.lag6_mac)
        self.neighbor_entry_20 = sai_thrift_neighbor_entry_t(
            rif_id=self.rif, ip_address=sai_ipaddress(self.port26_ip))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.neighbor_entry_20,
                                         dst_mac_address=self.port26_mac)
        self.neighbor_entry_30 = sai_thrift_neighbor_entry_t(
            rif_id=self.rif, ip_address=sai_ipaddress(self.lag7_ip))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.neighbor_entry_30,
                                         dst_mac_address=self.lag7_mac)
        self.neighbor_entry_40 = sai_thrift_neighbor_entry_t(
            rif_id=self.rif, ip_address=sai_ipaddress(self.port29_ip))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.neighbor_entry_40,
                                         dst_mac_address=self.port29_mac)

        # test fdb creation
        self.fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.lag6_mac,
            bv_id=self.vlan100)
        status = sai_thrift_create_fdb_entry(self.client,
                                             self.fdb_entry,
                                             type=SAI_FDB_ENTRY_TYPE_STATIC,
                                             bridge_port_id=self.lag6_bp)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.port26_mac,
            bv_id=self.vlan100)
        status = sai_thrift_create_fdb_entry(self.client,
                                             self.fdb_entry,
                                             type=SAI_FDB_ENTRY_TYPE_STATIC,
                                             bridge_port_id=self.port26_bp)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.lag7_mac,
            bv_id=self.vlan100)
        status = sai_thrift_create_fdb_entry(self.client,
                                             self.fdb_entry,
                                             type=SAI_FDB_ENTRY_TYPE_STATIC,
                                             bridge_port_id=self.lag7_bp)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.port29_mac,
            bv_id=self.vlan100)
        status = sai_thrift_create_fdb_entry(self.client,
                                             self.fdb_entry,
                                             type=SAI_FDB_ENTRY_TYPE_STATIC,
                                             bridge_port_id=self.port29_bp)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.isol_group1 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_BRIDGE_PORT)
        self.isol_group2 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_PORT)

        self.isol_group1_m_lag7_bp = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group1,
            isolation_object=self.lag7_bp)
        self.isol_group2_mbr_p26 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group2,
            isolation_object=self.port26)

    def runTest(self):
        try:
            self.forwardingTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_isolation_group_member(
            self.client,
            self.isol_group1_m_lag7_bp)

        sai_thrift_remove_isolation_group_member(
            self.client,
            self.isol_group2_mbr_p26)

        sai_thrift_remove_isolation_group(self.client, self.isol_group1)
        sai_thrift_remove_isolation_group(self.client, self.isol_group2)

        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry_10)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry_20)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry_30)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry_40)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry_100)

        sai_thrift_remove_route_entry(self.client, self.route_entry0)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        sai_thrift_remove_router_interface(self.client, self.rif)

        sai_thrift_set_lag_attribute(self.client, self.lag6, port_vlan_id=0)
        sai_thrift_set_lag_attribute(self.client, self.lag7, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port26, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port29, port_vlan_id=0)

        sai_thrift_remove_vlan_member(self.client, self.vlan100_member0)
        sai_thrift_remove_vlan_member(self.client, self.vlan100_member1)
        sai_thrift_remove_vlan_member(self.client, self.vlan100_member2)
        sai_thrift_remove_vlan_member(self.client, self.vlan100_member3)
        sai_thrift_remove_vlan(self.client, self.vlan100)

        sai_thrift_remove_lag_member(self.client, self.lag6_member0)
        sai_thrift_remove_lag_member(self.client, self.lag6_member1)
        sai_thrift_remove_lag_member(self.client, self.lag7_member0)
        sai_thrift_remove_lag_member(self.client, self.lag7_member1)

        sai_thrift_remove_bridge_port(self.client, self.lag6_bp)
        sai_thrift_remove_bridge_port(self.client, self.lag7_bp)
        sai_thrift_remove_bridge_port(self.client, self.port26_bp)
        sai_thrift_remove_bridge_port(self.client, self.port27_bp)

        sai_thrift_remove_lag(self.client, self.lag6)
        sai_thrift_remove_lag(self.client, self.lag7)

        super(CombinedIsolationTest, self).tearDown()

    def getDropCount(self):
        """
        Return sum of drop count for all ports

        :returns: Overall drop count.
        :rtype: int
        """
        cnt = 0
        for port in [getattr(self, "port"+str(p)) for p in range(0, 32)]:
            ctrs = sai_thrift_get_port_stats(self.client, port)
            cnt += ctrs['SAI_PORT_STAT_IF_OUT_DISCARDS']
        return cnt

    def forwardingTest(self):
        """
        Forwarding between ports with isolation groups attached
        Ingress ports: port24
        Isolation groups:
          grp1: port26_bp
          grp2: port25
        """
        print("\nForwardingTest()")

        pkt_brcast = simple_udp_packet(eth_dst="ff:ff:ff:ff:ff:ff",
                                       eth_src=self.lag6_mac,
                                       pktlen=100)

        pkt1_b = simple_udp_packet(eth_dst=self.port26_mac,
                                   eth_src=self.lag6_mac,
                                   pktlen=100)
        pkt2_b = simple_udp_packet(eth_dst=self.lag7_mac,
                                   eth_src=self.lag6_mac,
                                   pktlen=100)
        pkt3_b = simple_udp_packet(eth_dst=self.port29_mac,
                                   eth_src=self.lag6_mac,
                                   pktlen=100)

        pkt1_r = simple_udp_packet(eth_dst=self.rif_mac,
                                   eth_src=self.lag6_mac,
                                   ip_dst=self.port26_ip,
                                   ip_src=self.lag6_ip,
                                   ip_ttl=63)
        pkt1_r_exp = simple_udp_packet(eth_src=self.rif_mac,
                                       eth_dst=self.port26_mac,
                                       ip_dst=self.port26_ip,
                                       ip_src=self.lag6_ip,
                                       ip_ttl=62)

        pkt2_r = simple_udp_packet(eth_dst=self.rif_mac,
                                   eth_src=self.lag6_mac,
                                   ip_dst=self.lag7_ip,
                                   ip_src=self.lag6_ip,
                                   ip_ttl=63)
        pkt2_r_exp = simple_udp_packet(eth_src=self.rif_mac,
                                       eth_dst=self.lag7_mac,
                                       ip_dst=self.lag7_ip,
                                       ip_src=self.lag6_ip,
                                       ip_ttl=62)

        pkt3_r = simple_udp_packet(eth_dst=self.rif_mac,
                                   eth_src=self.lag6_mac,
                                   ip_dst=self.port29_ip,
                                   ip_src=self.lag6_ip,
                                   ip_ttl=63)
        pkt3_r_exp = simple_udp_packet(eth_src=self.rif_mac,
                                       eth_dst=self.port29_mac,
                                       ip_dst=self.port29_ip,
                                       ip_src=self.lag6_ip,
                                       ip_ttl=62)

        before_cnt = self.getDropCount()
        try:
            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt_brcast)
            verify_each_packet_on_multiple_port_lists(
                self,
                [pkt_brcast, pkt_brcast, pkt_brcast],
                [[self.dev_port26],
                 [self.dev_port27, self.dev_port28],
                 [self.dev_port29]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_b)
            verify_packets(self, pkt1_b, [self.dev_port26])

            print("Sending bridged packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_b)
            verify_packet_any_port(
                self,
                pkt2_b,
                [self.dev_port27, self.dev_port28])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_b)
            verify_packets(self, pkt3_b, [self.dev_port29])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_r)
            verify_packets(self, pkt1_r_exp, [self.dev_port26])

            print("Sending routed packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_r)
            verify_packet_any_port(
                self,
                pkt2_r_exp,
                [self.dev_port27, self.dev_port28])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.dev_port29])

            #################################################################
            print("Attach bridge port isolation group "
                  "with member lag7 to lag6")
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.lag6_bp,
                isolation_group=self.isol_group1)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt_brcast)
            verify_packets(self,
                           pkt_brcast,
                           [self.dev_port26,
                            self.dev_port29])

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port25))
            send_packet(self, self.dev_port25, pkt_brcast)
            verify_packets(self,
                           pkt_brcast,
                           [self.dev_port26,
                            self.dev_port29])

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port26))
            send_packet(self, self.dev_port26, pkt_brcast)
            verify_each_packet_on_multiple_port_lists(
                self,
                [pkt_brcast, pkt_brcast, pkt_brcast],
                [[self.dev_port24, self.dev_port25],
                 [self.dev_port27, self.dev_port28],
                 [self.dev_port29]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_b)
            verify_packets(self, pkt1_b, [self.dev_port26])

            print("Sending bridged packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_b)
            verify_no_other_packets(self)

            print("Sending bridged packet from port %d -> to lag7" %
                  (self.dev_port25))
            send_packet(self, self.dev_port25, pkt2_b)
            verify_no_other_packets(self)

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_b)
            verify_packets(self, pkt3_b, [self.dev_port29])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_r)
            verify_packets(self, pkt1_r_exp, [self.dev_port26])

            print("Sending routed packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_r)
            verify_packet_any_port(
                self,
                pkt2_r_exp,
                [self.dev_port27, self.dev_port28])

            print("Sending routed packet from port %d -> to lag7" %
                  (self.dev_port25))
            send_packet(self, self.dev_port25, pkt2_r)
            verify_packet_any_port(
                self,
                pkt2_r_exp,
                [self.dev_port27, self.dev_port28])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.dev_port29])

            print("Attach port isolation group with member port%d to port%d" %
                  (self.dev_port26, self.dev_port24))
            #################################################################
            status = sai_thrift_set_port_attribute(
                self.client,
                self.port24,
                isolation_group=self.isol_group2)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port24))

            send_packet(self, self.dev_port24, pkt_brcast)
            verify_packets(self, pkt_brcast, [self.dev_port29])

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port25))
            send_packet(self, self.dev_port25, pkt_brcast)
            verify_packets(self, pkt_brcast, [self.dev_port26,
                                              self.dev_port29])

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port26))
            send_packet(self, self.dev_port26, pkt_brcast)
            verify_each_packet_on_multiple_port_lists(
                self,
                [pkt_brcast, pkt_brcast, pkt_brcast],
                [[self.dev_port24, self.dev_port25],
                 [self.dev_port27, self.dev_port28],
                 [self.dev_port29]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_b)
            verify_no_other_packets(self)

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port25, self.dev_port26))
            send_packet(self, self.dev_port25, pkt1_b)
            verify_packets(self, pkt1_b, [self.dev_port26])

            print("Sending bridged packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_b)
            verify_no_other_packets(self)

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_b)
            verify_packets(self, pkt3_b, [self.dev_port29])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_r)
            verify_no_other_packets(self)

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port25, self.dev_port26))
            send_packet(self, self.dev_port25, pkt1_r)
            verify_packets(self, pkt1_r_exp, [self.dev_port26])

            print("Sending routed packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_r)
            verify_packet_any_port(
                self,
                pkt2_r_exp,
                [self.dev_port27, self.dev_port28])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.dev_port29])

            print("Detach bridge_port isolation group from lag6")
            #################################################################
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.lag6_bp,
                isolation_group=0)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt_brcast)
            verify_each_packet_on_multiple_port_lists(
                self,
                [pkt_brcast, pkt_brcast],
                [[self.dev_port27, self.dev_port28],
                 [self.dev_port29]])

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port25))
            send_packet(self, self.dev_port25, pkt_brcast)
            verify_each_packet_on_multiple_port_lists(
                self,
                [pkt_brcast, pkt_brcast, pkt_brcast],
                [[self.dev_port26],
                 [self.dev_port27, self.dev_port28],
                 [self.dev_port29]])

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port26))
            send_packet(self, self.dev_port26, pkt_brcast)
            verify_each_packet_on_multiple_port_lists(
                self,
                [pkt_brcast, pkt_brcast, pkt_brcast],
                [[self.dev_port24, self.dev_port25],
                 [self.dev_port27, self.dev_port28],
                 [self.dev_port29]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_b)
            verify_no_other_packets(self)

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port25, self.dev_port26))
            send_packet(self, self.dev_port25, pkt1_b)
            verify_packets(self, pkt1_b, [self.dev_port26])

            print("Sending bridged packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_b)
            verify_packet_any_port(
                self,
                pkt2_b,
                [self.dev_port27, self.dev_port28])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_b)
            verify_packets(self, pkt3_b, [self.dev_port29])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_r)
            verify_no_other_packets(self)

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port25, self.dev_port26))
            send_packet(self, self.dev_port25, pkt1_r)
            verify_packets(self, pkt1_r_exp, [self.dev_port26])

            print("Sending routed packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_r)
            # verify_no_other_packets(self)
            verify_packet_any_port(
                self,
                pkt2_r_exp,
                [self.dev_port27, self.dev_port28])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.dev_port29])

            print("Detach isolation group from port24")
            #################################################################
            status = sai_thrift_set_port_attribute(
                self.client,
                self.port24,
                isolation_group=0)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending bridged packet (brcast) from port %d" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt_brcast)
            verify_each_packet_on_multiple_port_lists(
                self,
                [pkt_brcast, pkt_brcast, pkt_brcast],
                [[self.dev_port26],
                 [self.dev_port27, self.dev_port28],
                 [self.dev_port29]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_b)
            verify_packets(self, pkt1_b, [self.dev_port26])

            print("Sending bridged packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_b)
            verify_packet_any_port(
                self,
                pkt2_b,
                [self.dev_port27, self.dev_port28])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_b)
            verify_packets(self, pkt3_b, [self.dev_port29])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port26))
            send_packet(self, self.dev_port24, pkt1_r)
            verify_packets(self, pkt1_r_exp, [self.dev_port26])

            print("Sending routed packet from port %d -> to lag7" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, pkt2_r)
            verify_packet_any_port(
                self,
                pkt2_r_exp,
                [self.dev_port27, self.dev_port28])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.dev_port24, self.dev_port29))
            send_packet(self, self.dev_port24, pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.dev_port29])

            after_cnt = self.getDropCount()
            print("after_cnt == {}; before_cnt == {}", after_cnt, before_cnt)
            assert after_cnt - before_cnt == 13
            print("Verification complete")

        finally:
            sai_thrift_set_bridge_port_attribute(
                self.client,
                self.lag6_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            sai_thrift_set_port_attribute(
                self.client,
                self.port24,
                isolation_group=int(SAI_NULL_OBJECT_ID))
