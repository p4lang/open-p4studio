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
Thrift SAI interface Multicast tests
"""

from sai_base_test import *


# # # # # # # # # # # # L2MC tests follow # # # # # # # # # # # #

class L2MCTestBase(SaiHelperBase):
    '''
    Base VLANs and configuration for L2MC tests
      VLAN 10:
        - access ports: port0, port1, port2, lag0 (port4, port5), port8
        - trunk ports: port3, port6
      VLAN 20:
        - access port: port7
        - trunk ports: port3, port6
    '''
    def setUp(self):
        super(L2MCTestBase, self).setUp()

        # create VLAN 10
        self.vlan10 = sai_thrift_create_vlan(self.client, vlan_id=10)
        # port0 VLAN 10 member
        self.port0_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port0,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_mbr0 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port0_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client, self.port0, port_vlan_id=10)
        # port1 VLAN 10 member
        self.port1_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port1,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_mbr1 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client, self.port1, port_vlan_id=10)
        # port2 VLAN 10 member
        self.port2_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port2,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_mbr2 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client, self.port2, port_vlan_id=10)
        # port3 VLAN 10 trunk member
        self.port3_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port3,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_mbr3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port3_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)
        # lag0 (port4, port5) VLAN 10 member
        self.lag0 = sai_thrift_create_lag(self.client)
        self.lag0_mbr4 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag0, port_id=self.port4)
        self.lag0_mbr5 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag0, port_id=self.port5)
        self.lag0_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.lag0,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_lag_mbr = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.lag0_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        # port6 VLAN 10 trunk member
        self.port6_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port6,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_mbr6 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port6_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)
        # port8 VLAN 10 member
        self.port8_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port8,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_mbr8 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port8_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client, self.port8, port_vlan_id=10)

        # create VLAN 20
        self.vlan20 = sai_thrift_create_vlan(self.client, vlan_id=20)
        # port3 VLAN 20 trunk member
        self.vlan20_mbr3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.port3_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)
        # port6 VLAN 20 trunk member
        self.vlan20_mbr6 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.port6_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)
        # port7 VLAN 20 member
        self.port7_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port7,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan20_mbr7 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.port7_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client, self.port7, port_vlan_id=20)

    def tearDown(self):
        sai_thrift_set_port_attribute(self.client, self.port7, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan20_mbr7)
        sai_thrift_remove_bridge_port(self.client, self.port7_bp)
        sai_thrift_remove_vlan_member(self.client, self.vlan20_mbr6)
        sai_thrift_remove_vlan_member(self.client, self.vlan20_mbr3)
        sai_thrift_remove_vlan(self.client, self.vlan20)

        sai_thrift_set_port_attribute(self.client, self.port8, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_mbr8)
        sai_thrift_remove_bridge_port(self.client, self.port8_bp)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_mbr6)
        sai_thrift_remove_bridge_port(self.client, self.port6_bp)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_lag_mbr)
        sai_thrift_remove_bridge_port(self.client, self.lag0_bp)
        sai_thrift_remove_lag_member(self.client, self.lag0_mbr5)
        sai_thrift_remove_lag_member(self.client, self.lag0_mbr4)
        sai_thrift_remove_lag(self.client, self.lag0)
        sai_thrift_set_port_attribute(self.client, self.port3, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_mbr3)
        sai_thrift_remove_bridge_port(self.client, self.port3_bp)
        sai_thrift_set_port_attribute(self.client, self.port2, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_mbr2)
        sai_thrift_remove_bridge_port(self.client, self.port2_bp)
        sai_thrift_set_port_attribute(self.client, self.port1, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_mbr1)
        sai_thrift_remove_bridge_port(self.client, self.port1_bp)
        sai_thrift_set_port_attribute(self.client, self.port0, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_mbr0)
        sai_thrift_remove_bridge_port(self.client, self.port0_bp)
        sai_thrift_remove_vlan(self.client, self.vlan10)

        super(L2MCTestBase, self).tearDown()


class L2MCBaseTopo(L2MCTestBase):
    '''
    Base L2MC topology configuration
      L2MC group 1: port0, port1, port2, port3
      L2MC group 2: port0, port2, lag0
      L2MC group 3: port3, port6, port7
      non-multicast port: port8
    '''
    def setUp(self):
        super(L2MCBaseTopo, self).setUp()

        # create L2MC group1: port0, port1, port2, port3
        self.l2mc_grp1 = sai_thrift_create_l2mc_group(self.client)
        self.grp1_mbr0 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp1,
            l2mc_output_id=self.port0_bp)
        self.grp1_mbr1 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp1,
            l2mc_output_id=self.port1_bp)
        self.grp1_mbr2 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp1,
            l2mc_output_id=self.port2_bp)
        self.grp1_mbr3 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp1,
            l2mc_output_id=self.port3_bp)

        # create L2MC group2: port0, port2, lag0 (port4, port5)
        self.l2mc_grp2 = sai_thrift_create_l2mc_group(self.client)
        self.grp2_mbr0 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp2,
            l2mc_output_id=self.port0_bp)
        self.grp2_mbr2 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp2,
            l2mc_output_id=self.port2_bp)
        self.grp2_lag_mbr = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp2,
            l2mc_output_id=self.lag0_bp)

        # create L2MC group3: port3, port6, port7
        self.l2mc_grp3 = sai_thrift_create_l2mc_group(self.client)
        self.grp3_mbr3 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp3,
            l2mc_output_id=self.port3_bp)
        self.grp3_mbr6 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp3,
            l2mc_output_id=self.port6_bp)
        self.grp3_mbr7 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp3,
            l2mc_output_id=self.port7_bp)

    def runTest(self):
        try:
            self.l2mcGroupAttrTest()
            self.l2mcGroupMemberAttrTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_l2mc_group_member(self.client, self.grp3_mbr7)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp3_mbr6)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp3_mbr3)
        sai_thrift_remove_l2mc_group(self.client, self.l2mc_grp3)

        sai_thrift_remove_l2mc_group_member(self.client, self.grp2_lag_mbr)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp2_mbr2)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp2_mbr0)
        sai_thrift_remove_l2mc_group(self.client, self.l2mc_grp2)

        sai_thrift_remove_l2mc_group_member(self.client, self.grp1_mbr3)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp1_mbr2)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp1_mbr1)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp1_mbr0)
        sai_thrift_remove_l2mc_group(self.client, self.l2mc_grp1)

        super(L2MCBaseTopo, self).tearDown()

    def l2mcGroupAttrTest(self):
        '''
        Test L2MC group L2MC_MEMBER_LIST read-only attribute
        '''
        print("\nL2MCGroupAttrTest()")

        mbr_list = sai_thrift_object_list_t(
            count=10, idlist=[])
        attr = sai_thrift_get_l2mc_group_attribute(
            self.client, self.l2mc_grp1, l2mc_member_list=mbr_list)

        self.assertEqual(attr["l2mc_member_list"].count, 4,
                         "Incorrect number of L2MC group 1 members")

        mbr_list = attr["l2mc_member_list"].idlist
        mbr_list.sort()
        init_mbr_list = [
            self.grp1_mbr0, self.grp1_mbr1, self.grp1_mbr2, self.grp1_mbr3]
        init_mbr_list.sort()
        self.assertTrue(mbr_list == init_mbr_list,
                        "Incorrect L2MC member list")

    def l2mcGroupMemberAttrTest(self):
        '''
        Test L2MC group member create-only attributes
        '''
        print("\nL2MCGroupMemberAttrTest()")

        attr = sai_thrift_get_l2mc_group_member_attribute(self.client,
                                                          self.grp1_mbr0,
                                                          l2mc_group_id=True,
                                                          l2mc_output_id=True)
        self.assertTrue(attr["l2mc_group_id"] == self.l2mc_grp1,
                        "Incorrect L2MC group id")
        self.assertTrue(attr["l2mc_output_id"] == self.port0_bp,
                        "Incorrect L2MC output id")


class L2MCIpv4Test(L2MCBaseTopo):
    '''
    Base tests for L2 multicast functionalities with IPv4 packets
    '''
    def setUp(self):
        super(L2MCIpv4Test, self).setUp()

        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"
        self.grp_ip1 = "230.1.1.5"  # used for (*, G) and (S, G) paths
        self.grp_ip2 = "230.1.1.10"  # used for (*, G) only
        self.src_ip1 = "10.0.10.5"  # used for (*, G) only
        self.src_ip2 = "10.0.10.10"  # used for (S, G) only

        # configure L2MC forwarding paths
        # (*, G) path to group 1
        self.l2mc_bridge1 = sai_thrift_l2mc_entry_t(
            bv_id=self.vlan10,
            type=SAI_L2MC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip1))
        sai_thrift_create_l2mc_entry(
            self.client, self.l2mc_bridge1, output_group_id=self.l2mc_grp1)
        # (*, G) path to group 2
        self.l2mc_bridge2 = sai_thrift_l2mc_entry_t(
            bv_id=self.vlan10,
            type=SAI_L2MC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip2))
        sai_thrift_create_l2mc_entry(
            self.client, self.l2mc_bridge2, output_group_id=self.l2mc_grp2)
        # (S, G) path to group 3
        self.l2mc_bridge3 = sai_thrift_l2mc_entry_t(
            bv_id=self.vlan20,
            type=SAI_L2MC_ENTRY_TYPE_SG,
            destination=sai_ipaddress(self.grp_ip1),
            source=sai_ipaddress(self.src_ip2))
        sai_thrift_create_l2mc_entry(
            self.client, self.l2mc_bridge3, output_group_id=self.l2mc_grp3)

    def runTest(self):
        try:
            self.l2mcTaggingTest()
            self.l2mcLagTest()
            self.l2mcSGTest()
            self.l2mcMultipleBridgesTest()
            self.l2mcGroupUpdateTest()
            self.l2mcAttrTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_l2mc_entry(self.client, self.l2mc_bridge3)
        sai_thrift_remove_l2mc_entry(self.client, self.l2mc_bridge2)
        sai_thrift_remove_l2mc_entry(self.client, self.l2mc_bridge1)

        super(L2MCIpv4Test, self).tearDown()

    def l2mcTaggingTest(self):
        '''
        Exercise ipv4_multicast_bridge_star_g with a group containing tagged
        port. Tagged packet is sent on a port belonging to two groups:
        Group 1 pointed by (*, G) bridge and Group 3 pointed by (S, G) bridge
        '''
        print("\nL2MCTaggingTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            pktlen=100)
        tag_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)

        print("Sending untagged packet to multicast group 1")
        send_packet(self, self.dev_port0, pkt)
        verify_each_packet_on_each_port(
            self, [pkt, pkt, tag_pkt],
            [self.dev_port1, self.dev_port2, self.dev_port3])
        print("\tOK")

        print("Sending tagged packet to multicast group 1")
        send_packet(self, self.dev_port3, tag_pkt)
        verify_packets(
            self, pkt, [self.dev_port0, self.dev_port1, self.dev_port2])
        print("\tOK")

        print("Sending packet to port not belonging to the group - "
              "flood within group 1")
        send_packet(self, self.dev_port8, pkt)
        verify_each_packet_on_each_port(
            self, [pkt, pkt, pkt, tag_pkt],
            [self.dev_port0, self.dev_port1, self.dev_port2, self.dev_port3])
        print("\tOK")

    def l2mcLagTest(self):
        '''
        Exercise ipv4_multicast_bridge_star_g with a group containing LAG
        '''
        print("\nL2MCLagTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip2,
            pktlen=100)
        tag_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip2,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        print("Sending packet to multicast group 2")
        send_packet(self, self.dev_port0, pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [pkt, pkt],
            [[self.dev_port2], [self.dev_port4, self.dev_port5]])
        print("\tOK")

        print("Sending packet to port not belonging to the VLAN "
              "- flood within group 3")
        send_packet(self, self.dev_port7, pkt)
        verify_packets(self, tag_pkt, [self.dev_port3, self.dev_port6])
        print("\tOK")

    def l2mcSGTest(self):
        '''
        Exercise ipv4_multicast_bridge_s_g with a group containing tagged ports
        grp_ip is the same as for path directing to l2mc_grp1 but src_ip should
        point to l2mc_grp3.
        Packet is sent on a port belonging to two groups:
        Group 1 pointed by (*, G) bridge and Group 3 pointed by (S, G) bridge -
        it should be received only on ports in Group 3
        '''
        print("\nL2MCSGTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip2,
            pktlen=100)
        tag_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip2,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        print("Sending packet to multicast group 3")
        send_packet(self, self.dev_port3, tag_pkt)
        verify_each_packet_on_each_port(
            self, [tag_pkt, pkt], [self.dev_port6, self.dev_port7])
        print("\tOK")

    def l2mcMultipleBridgesTest(self):
        '''
        Verify if a multicast group (without members of type tunnel) may be
        used by multiple L2MC bridges
        '''
        print("\nl2mcMultipleBridgesTest()")

        grp_ip3 = "230.1.1.15"

        try:
            l2mc_bridge4 = sai_thrift_l2mc_entry_t(
                bv_id=self.vlan10,
                type=SAI_L2MC_ENTRY_TYPE_XG,
                destination=sai_ipaddress(grp_ip3))
            sai_thrift_create_l2mc_entry(
                self.client, l2mc_bridge4, output_group_id=self.l2mc_grp1)

            pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.src_mac,
                ip_src=self.src_ip1,
                ip_dst=grp_ip3,
                pktlen=100)
            tag_pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.src_mac,
                ip_src=self.src_ip1,
                ip_dst=grp_ip3,
                dl_vlan_enable=True,
                vlan_vid=10,
                pktlen=104)

            print("Sending packet to multicast group 1 via new bridge")
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_each_port(
                self, [pkt, pkt, tag_pkt],
                [self.dev_port1, self.dev_port2, self.dev_port3])
            print("\tOK")

            print("\nVerifying if the old bridge still works:")
            self.l2mcTaggingTest()

        finally:
            sai_thrift_remove_l2mc_entry(self.client, l2mc_bridge4)

    def l2mcGroupUpdateTest(self):
        '''
        Test L2MC group update to check whether group member may be created
        after L2MC entry
        '''
        print("\nl2mcGroupUpdateTest()")

        try:
            print("Add port8 to multicast group 2")
            grp2_mbr8 = sai_thrift_create_l2mc_group_member(
                self.client,
                l2mc_group_id=self.l2mc_grp2,
                l2mc_output_id=self.port8_bp)

            pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.src_mac,
                ip_src=self.src_ip1,
                ip_dst=self.grp_ip2,
                pktlen=100)

            print("Sending packet to multicast group 2")
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, pkt, pkt],
                [[self.dev_port2], [self.dev_port4, self.dev_port5],
                 [self.dev_port8]])
            print("\tOK")

            print("Remove port8 from multicast group 2")
            sai_thrift_remove_l2mc_group_member(self.client, grp2_mbr8)

            print("Sending packet to multicast group 2")
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, pkt],
                [[self.dev_port2], [self.dev_port4, self.dev_port5]])
            print("\tOK")

        finally:
            pass

    def l2mcAttrTest(self):
        '''
        Test L2MC entry attributes
        Note that this test modifies initial L2MC bridges configuration
        '''
        print("\nl2mcAttrTest()")

        attr = sai_thrift_get_l2mc_entry_attribute(
            self.client, self.l2mc_bridge1, output_group_id=True)
        self.assertTrue(attr["output_group_id"] == self.l2mc_grp1,
                        "Incorrect L2MC group id")

        status = sai_thrift_set_l2mc_entry_attribute(
            self.client, self.l2mc_bridge1, output_group_id=self.l2mc_grp2)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        # verify with traffic
        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            pktlen=100)

        print("Sending packet to multicast group 2")
        send_packet(self, self.dev_port0, pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [pkt, pkt],
            [[self.dev_port2], [self.dev_port4, self.dev_port5]])
        print("\tOK")


class L2MCIpv6Test(L2MCBaseTopo):
    '''
    Base tests for L2 multicast functionalities with IPv6 packets
    '''
    def setUp(self):
        super(L2MCIpv6Test, self).setUp()

        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"
        self.grp_ip1 = "ffbf::5"  # used for (*, G) and (S, G) paths
        self.grp_ip2 = "ffbf::10"  # used for (*, G) only
        self.src_ip1 = "2001:0db8::5"  # used for (*, G) only
        self.src_ip2 = "2001:0db8::10"  # used for (S, G) only

        # configure L2MC forwarding paths
        # (*, G) path to group 1
        self.l2mc_bridge1 = sai_thrift_l2mc_entry_t(
            bv_id=self.vlan10,
            type=SAI_L2MC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip1))
        sai_thrift_create_l2mc_entry(
            self.client, self.l2mc_bridge1, output_group_id=self.l2mc_grp1)
        # (*, G) path to group 2
        self.l2mc_bridge2 = sai_thrift_l2mc_entry_t(
            bv_id=self.vlan10,
            type=SAI_L2MC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip2))
        sai_thrift_create_l2mc_entry(
            self.client, self.l2mc_bridge2, output_group_id=self.l2mc_grp2)
        # (S, G) path to group 3
        self.l2mc_bridge3 = sai_thrift_l2mc_entry_t(
            bv_id=self.vlan20,
            type=SAI_L2MC_ENTRY_TYPE_SG,
            destination=sai_ipaddress(self.grp_ip1),
            source=sai_ipaddress(self.src_ip2))
        sai_thrift_create_l2mc_entry(
            self.client, self.l2mc_bridge3, output_group_id=self.l2mc_grp3)

    def runTest(self):
        try:
            self.l2mcTaggingTest()
            self.l2mcLagTest()
            self.l2mcSGTest()
            self.l2mcMultipleBridgesTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_l2mc_entry(self.client, self.l2mc_bridge3)
        sai_thrift_remove_l2mc_entry(self.client, self.l2mc_bridge2)
        sai_thrift_remove_l2mc_entry(self.client, self.l2mc_bridge1)

        super(L2MCIpv6Test, self).tearDown()

    def l2mcTaggingTest(self):
        '''
        Exercise ipv4_multicast_bridge_star_g with a group containing tagged
        port. Tagged packet is sent on a port belonging to two groups:
        Group 1 pointed by (*, G) bridge and Group 3 pointed by (S, G) bridge
        '''
        print("\nL2MCTaggingTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            pktlen=100)
        tag_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)

        print("Sending untagged packet to multicast group 1")
        send_packet(self, self.dev_port0, pkt)
        verify_each_packet_on_each_port(
            self, [pkt, pkt, tag_pkt],
            [self.dev_port1, self.dev_port2, self.dev_port3])
        print("\tOK")

        print("Sending tagged packet to multicast group 1")
        send_packet(self, self.dev_port3, tag_pkt)
        verify_packets(
            self, pkt, [self.dev_port0, self.dev_port1, self.dev_port2])
        print("\tOK")

        print("Sending packet to port not belonging to the group - "
              "flood within group 1")
        send_packet(self, self.dev_port8, pkt)
        verify_each_packet_on_each_port(
            self, [pkt, pkt, pkt, tag_pkt],
            [self.dev_port0, self.dev_port1, self.dev_port2, self.dev_port3])
        print("\tOK")

    def l2mcLagTest(self):
        '''
        Exercise ipv4_multicast_bridge_star_g with a group containing LAG
        '''
        print("\nL2MCLagTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip2,
            pktlen=100)
        tag_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip2,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        print("Sending packet to multicast group 2")
        send_packet(self, self.dev_port0, pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [pkt, pkt],
            [[self.dev_port2], [self.dev_port4, self.dev_port5]])
        print("\tOK")

        print("Sending packet to port not belonging to the VLAN "
              "- flood within group 3")
        send_packet(self, self.dev_port7, pkt)
        verify_packets(self, tag_pkt, [self.dev_port3, self.dev_port6])
        print("\tOK")

    def l2mcSGTest(self):
        '''
        Exercise ipv4_multicast_bridge_s_g with a group containing tagged ports
        grp_ip is the same as for path directing to l2mc_grp1 but src_ip should
        point to l2mc_grp3.
        Packet is sent on a port belonging to two groups:
        Group 1 pointed by (*, G) bridge and Group 3 pointed by (S, G) bridge -
        it should be received only on ports in Group 3
        '''
        print("\nL2MCSGTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip2,
            pktlen=100)
        tag_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip2,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        print("Sending packet to multicast group 3")
        send_packet(self, self.dev_port3, tag_pkt)
        verify_each_packet_on_each_port(
            self, [tag_pkt, pkt], [self.dev_port6, self.dev_port7])
        print("\tOK")

    def l2mcMultipleBridgesTest(self):
        '''
        Verify if a multicast group (without members of type tunnel) may be
        used by multiple L2MC bridges
        '''
        print("\nl2mcMultipleBridgesTest()")

        grp_ip3 = "ffbf::15"

        try:
            l2mc_bridge4 = sai_thrift_l2mc_entry_t(
                bv_id=self.vlan10,
                type=SAI_L2MC_ENTRY_TYPE_XG,
                destination=sai_ipaddress(grp_ip3))
            sai_thrift_create_l2mc_entry(
                self.client, l2mc_bridge4, output_group_id=self.l2mc_grp1)

            pkt = simple_udpv6_packet(
                eth_dst=self.dst_mac,
                eth_src=self.src_mac,
                ipv6_src=self.src_ip1,
                ipv6_dst=grp_ip3,
                pktlen=100)
            tag_pkt = simple_udpv6_packet(
                eth_dst=self.dst_mac,
                eth_src=self.src_mac,
                ipv6_src=self.src_ip1,
                ipv6_dst=grp_ip3,
                dl_vlan_enable=True,
                vlan_vid=10,
                pktlen=104)

            print("Sending packet to multicast group 1 via new bridge")
            send_packet(self, self.dev_port0, pkt)
            verify_each_packet_on_each_port(
                self, [pkt, pkt, tag_pkt],
                [self.dev_port1, self.dev_port2, self.dev_port3])
            print("\tOK")

            print("\nVerifying if the old bridge still works:")
            self.l2mcTaggingTest()

        finally:
            sai_thrift_remove_l2mc_entry(self.client, l2mc_bridge4)


@group("tunnel")
class L2MCTunnelTest(L2MCTestBase):
    '''
    Simple L2 multicast tests with member of type tunnel
    '''
    def setUp(self):
        super(L2MCTunnelTest, self).setUp()

        # tunnels configuration
        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport_dev = self.dev_port10
        self.uport_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10)

        self.vni = 1000
        self.tun_ip = "10.0.0.65"
        self.lpb_ip = "10.1.0.32"
        self.src_ip = "192.168.100.1"
        self.vxlan_dst_ip = "192.168.100.2"
        self.src_mac = "00:22:22:22:22:11"
        self.vxlan_dst_mac = "00:22:22:22:22:22"
        self.unbor_mac = "00:11:11:11:11:11"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun_ip),
            rif_id=self.uport_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(
            self.client, self.unbor, dst_mac_address=self.unbor_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun_ip),
            router_interface_id=self.uport_rif)

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

        # create tunnel map entries for vlan
        self.decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni)

        self.p2p_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun_ip))

        # bridge port created on p2p tunnel
        self.p2p_tunnel_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member created using tunnel bridge_port
        self.p2p_tunnel_vlan_mbr = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.vxlan_dst_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.p2p_tunnel_bp)

        # multicast configuration
        self.grp_ip = "230.1.1.5"
        self.dst_mac = "01:00:5e:01:01:05"

        # create L2MC group1: port0, port1, p2p_tunnel
        self.l2mc_grp = sai_thrift_create_l2mc_group(self.client)
        self.grp_mbr0 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp,
            l2mc_output_id=self.port0_bp)
        self.grp_mbr1 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp,
            l2mc_output_id=self.port1_bp)
        self.grp_mbr2 = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp,
            l2mc_output_id=self.port2_bp)
        self.grp_tun_mbr = sai_thrift_create_l2mc_group_member(
            self.client,
            l2mc_group_id=self.l2mc_grp,
            l2mc_output_id=self.p2p_tunnel_bp)

        self.l2mc_bridge = sai_thrift_l2mc_entry_t(
            bv_id=self.vlan10,
            type=SAI_L2MC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip))
        sai_thrift_create_l2mc_entry(
            self.client, self.l2mc_bridge, output_group_id=self.l2mc_grp)

    def runTest(self):
        try:
            self.l2mcTunnelTest()
            self.tunnelMultipleBridgesTest()
            self.tunnelMultipleBridgesMultipleTunnelsTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_l2mc_entry(self.client, self.l2mc_bridge)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp_tun_mbr)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp_mbr2)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp_mbr1)
        sai_thrift_remove_l2mc_group_member(self.client, self.grp_mbr0)
        sai_thrift_remove_l2mc_group(self.client, self.l2mc_grp)

        sai_thrift_remove_vlan_member(self.client, self.p2p_tunnel_vlan_mbr)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)

        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)
        sai_thrift_remove_router_interface(self.client, self.uport_rif)

        super(L2MCTunnelTest, self).tearDown()

    def l2mcTunnelTest(self):
        '''
        Simple L2 multicast forwarding test with one of the multicast group
        member being a tunnel
        '''
        print("\nl2mcTunnelTest()")

        # L2 forwarding into VXLAN tunnel
        pkt = simple_udp_packet(eth_dst=self.vxlan_dst_mac,
                                eth_src=self.src_mac,
                                ip_dst=self.vxlan_dst_ip,
                                ip_src=self.src_ip,
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
                                vxlan_vni=self.vni,
                                inner_frame=pkt))
        vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet on port %d" % self.oport_dev)
        send_packet(self, self.oport_dev, pkt)
        verify_packet(self, vxlan_pkt, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")

        # L2 multicast forwarding
        pkt = simple_udp_packet(eth_dst=self.dst_mac,
                                eth_src=self.src_mac,
                                ip_dst=self.grp_ip,
                                ip_src=self.src_ip,
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
                                vxlan_vni=self.vni,
                                inner_frame=pkt))
        vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

        print("Sending multicast packet on port %d" % self.oport_dev)
        send_packet(self, self.oport_dev, pkt)
        verify_each_packet_on_each_port(
            self, [pkt, pkt, vxlan_pkt],
            [self.dev_port1, self.dev_port2, self.uport_dev])
        print("Packet was forwarded to ports and tunnel in multicast group")

    def tunnelMultipleBridgesTest(self):
        '''
        Verify if a multicast group with a member of type tunnel may be used by
        multiple L2MC bridges
        '''
        print("\ntunnelMultipleBridgesTest()")

        grp_ip2 = "230.1.1.10"

        try:
            l2mc_bridge2 = sai_thrift_l2mc_entry_t(
                bv_id=self.vlan10,
                type=SAI_L2MC_ENTRY_TYPE_XG,
                destination=sai_ipaddress(grp_ip2))
            sai_thrift_create_l2mc_entry(
                self.client, l2mc_bridge2, output_group_id=self.l2mc_grp)

            pkt = simple_udp_packet(eth_dst=self.dst_mac,
                                    eth_src=self.src_mac,
                                    ip_dst=grp_ip2,
                                    ip_src=self.src_ip,
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
                                    vxlan_vni=self.vni,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on port %d" % self.oport_dev)
            send_packet(self, self.oport_dev, pkt)
            verify_each_packet_on_each_port(
                self, [pkt, pkt, vxlan_pkt],
                [self.dev_port1, self.dev_port2, self.uport_dev])
            print("Packet was forwarded via new bridge")

        finally:
            sai_thrift_remove_l2mc_entry(self.client, l2mc_bridge2)

    def tunnelMultipleBridgesMultipleTunnelsTest(self):
        '''
        Verify if a multicast group with several members of type tunnel may be
        used by multiple L2MC bridges and test adding and deletion of such
        members
        '''
        print("\ntunnelMultipleBridgesMultipleTunnelsTest()")

        grp_ip2 = "230.1.1.10"
        tun2_ip = "10.0.0.33"
        unbor2_mac = "00:11:11:11:11:22"

        try:
            l2mc_bridge2 = sai_thrift_l2mc_entry_t(
                bv_id=self.vlan10,
                type=SAI_L2MC_ENTRY_TYPE_XG,
                destination=sai_ipaddress(grp_ip2))
            sai_thrift_create_l2mc_entry(
                self.client, l2mc_bridge2, output_group_id=self.l2mc_grp)

            # add a member of type tunnel after bridges are created
            uport_dev2 = self.dev_port11
            uport2_rif = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port11)

            unbor2 = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(tun2_ip),
                rif_id=uport2_rif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor2,
                                             dst_mac_address=unbor2_mac)

            unhop2 = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(tun2_ip),
                router_interface_id=uport2_rif)

            p2p_tunnel2 = sai_thrift_create_tunnel(
                self.client,
                type=self.tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=self.decap_maps,
                encap_mappers=self.encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                encap_dst_ip=sai_ipaddress(tun2_ip))

            p2p_tunnel2_bp = sai_thrift_create_bridge_port(
                self.client,
                type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                tunnel_id=p2p_tunnel2,
                bridge_id=self.default_1q_bridge,
                admin_state=True,
                fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

            p2p_tunnel2_vlan_mbr = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=p2p_tunnel2_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            print("Adding another L2MC group member of type tunnel")
            grp_tun_mbr = sai_thrift_create_l2mc_group_member(
                self.client,
                l2mc_group_id=self.l2mc_grp,
                l2mc_output_id=p2p_tunnel2_bp)

            pkt = simple_udp_packet(eth_dst=self.dst_mac,
                                    eth_src=self.src_mac,
                                    ip_dst=grp_ip2,
                                    ip_src=self.src_ip,
                                    pktlen=100)
            vxlan_pkt1 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni,
                                    inner_frame=pkt))
            vxlan_pkt1.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet with new group IP")
            send_packet(self, self.oport_dev, pkt)
            verify_each_packet_on_each_port(
                self, [pkt, pkt, vxlan_pkt1, vxlan_pkt2],
                [self.dev_port1, self.dev_port2, self.uport_dev, uport_dev2])
            print("Packet was forwarded via new bridge "
                  "(to a group with new tunnel)")

            print("Removing additional L2MC group member of type tunnel")
            sai_thrift_remove_l2mc_group_member(self.client, grp_tun_mbr)

            print("Sending packet with new group IP")
            send_packet(self, self.oport_dev, pkt)
            verify_each_packet_on_each_port(
                self, [pkt, pkt, vxlan_pkt1],
                [self.dev_port1, self.dev_port2, self.uport_dev])
            print("Packet was forwarded via new bridge"
                  "(new tunnel was not used)")

            print("\n\tVerify forwarding via co-existed old bridge")
            self.l2mcTunnelTest()

        finally:
            sai_thrift_remove_vlan_member(self.client, p2p_tunnel2_vlan_mbr)
            sai_thrift_remove_bridge_port(self.client, p2p_tunnel2_bp)
            sai_thrift_remove_tunnel(self.client, p2p_tunnel2)
            sai_thrift_remove_next_hop(self.client, unhop2)
            sai_thrift_remove_neighbor_entry(self.client, unbor2)
            sai_thrift_remove_router_interface(self.client, uport2_rif)
            sai_thrift_remove_l2mc_entry(self.client, l2mc_bridge2)


# # # # # # # # # # # # IPMC tests follow # # # # # # # # # # # #

class IPMCBaseTopo(SaiHelperBase):
    '''
    Base configuration and topology for IPMC tests
        Port        MC grp
        port0        1, 2
        port1         1
        port2        1, 2
        port3        1, 3
        lag0          2
          port4
          port5
        port6         3
        port7         3
        VLAN 10       3
          port 8
          port 9

    Args:
        ipv6 (bool): ipv6 configuraton indicator
    '''
    def __init__(self, ipv6=False):
        super(IPMCBaseTopo, self).__init__()
        self.ipv6 = ipv6

        self.default_rmac = "00:77:66:55:44:00"
        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"

        if ipv6 is True:
            self.v6_mc_enable = True
            self.v4_mc_enable = False
            self.grp_ip1 = "ffbf::5"  # used for (*, G) and (S, G) paths
            self.grp_ip2 = "ffbf::10"  # used for (*, G) only
            self.grp_ip3 = "ffbf::15"  # used for (*, G) path for SVI RPF
            self.src_ip1 = "2001:0db8::5"  # used for (*, G) only
            self.src_ip2 = "2001:0db8::10"  # used for (S, G) only
        else:
            self.v6_mc_enable = False
            self.v4_mc_enable = True
            self.grp_ip1 = "230.1.1.5"  # used for (*, G) and (S, G) paths
            self.grp_ip2 = "230.1.1.10"  # used for (*, G) only
            self.grp_ip3 = "230.1.1.15"  # used for (*, G) path for SVI RPF
            self.src_ip1 = "10.0.10.5"  # used for (*, G) only
            self.src_ip2 = "10.0.10.10"  # used for (S, G) only

    def setUp(self):
        super(IPMCBaseTopo, self).setUp()

        # create L3 router interfaces
        self.rif0 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=self.v4_mc_enable,
            v6_mcast_enable=self.v6_mc_enable,
            port_id=self.port0)

        self.rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=self.v4_mc_enable,
            v6_mcast_enable=self.v6_mc_enable,
            port_id=self.port1)

        self.rif2 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=self.v4_mc_enable,
            v6_mcast_enable=self.v6_mc_enable,
            port_id=self.port2)

        self.rif3 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=self.v4_mc_enable,
            v6_mcast_enable=self.v6_mc_enable,
            port_id=self.port3)

        self.lag0 = sai_thrift_create_lag(self.client)
        self.lag_mbr4 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag0, port_id=self.port4)
        self.lag_mbr5 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag0, port_id=self.port5)
        self.lag_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=self.v4_mc_enable,
            v6_mcast_enable=self.v6_mc_enable,
            port_id=self.lag0)

        self.rif6 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=self.v4_mc_enable,
            v6_mcast_enable=self.v6_mc_enable,
            port_id=self.port6)

        self.rif7 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=self.v4_mc_enable,
            v6_mcast_enable=self.v6_mc_enable,
            port_id=self.port7)

        self.vlan10 = sai_thrift_create_vlan(self.client, vlan_id=10)
        self.port8_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port8,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_mbr8 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port8_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(
            self.client, self.port8, port_vlan_id=10)
        self.port9_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port9,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan10_mbr9 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port9_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(
            self.client, self.port9, port_vlan_id=10)

        self.vlan10_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            virtual_router_id=self.default_vrf,
            v4_mcast_enable=self.v4_mc_enable,
            v6_mcast_enable=self.v6_mc_enable,
            vlan_id=self.vlan10)

        # create ipmc groups
        # group 1: port0, port1, port2, port3)
        self.ipmc_grp1 = sai_thrift_create_ipmc_group(self.client)

        self.ipmc1_mbr0 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp1, ipmc_output_id=self.rif0)
        self.ipmc1_mbr1 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp1, ipmc_output_id=self.rif1)
        self.ipmc1_mbr2 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp1, ipmc_output_id=self.rif2)
        self.ipmc1_mbr3 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp1, ipmc_output_id=self.rif3)

        # group 2: port0, port2, lag0 (port4, port5)
        self.ipmc_grp2 = sai_thrift_create_ipmc_group(self.client)

        self.ipmc2_mbr0 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp2, ipmc_output_id=self.rif0)
        self.ipmc2_mbr2 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp2, ipmc_output_id=self.rif2)
        self.ipmc2_lag_mbr = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp2, ipmc_output_id=self.lag_rif)

        # group 3: port3, port6, port7
        self.ipmc_grp3 = sai_thrift_create_ipmc_group(self.client)

        self.ipmc3_mbr3 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp3, ipmc_output_id=self.rif3)
        self.ipmc3_mbr6 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp3, ipmc_output_id=self.rif6)
        self.ipmc3_mbr7 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp3, ipmc_output_id=self.rif7)
        self.ipmc3_mbr_svi = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp3, ipmc_output_id=self.vlan10_rif)

        # configure IPMC forwarding paths and rpf groups -
        # in case of SM PIM mode there may be only one ingress rif
        # (*, G) route to group 1
        self.rpf_grp1 = sai_thrift_create_rpf_group(self.client)
        self.rpf1_mbr = sai_thrift_create_rpf_group_member(
            self.client,
            rpf_group_id=self.rpf_grp1, rpf_interface_id=self.rif0)

        self.ipmc_route1 = sai_thrift_ipmc_entry_t(
            vr_id=self.default_vrf,
            type=SAI_IPMC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip1))
        sai_thrift_create_ipmc_entry(
            self.client,
            self.ipmc_route1,
            rpf_group_id=self.rpf_grp1,
            output_group_id=self.ipmc_grp1)

        # (*, G) route to group 2
        self.ipmc_route2 = sai_thrift_ipmc_entry_t(
            vr_id=self.default_vrf,
            type=SAI_IPMC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip2))
        sai_thrift_create_ipmc_entry(
            self.client,
            self.ipmc_route2,
            rpf_group_id=self.rpf_grp1,
            output_group_id=self.ipmc_grp2)

        # (S, G) route to group 3
        self.rpf_grp2 = sai_thrift_create_rpf_group(self.client)
        self.rpf2_mbr = sai_thrift_create_rpf_group_member(
            self.client,
            rpf_group_id=self.rpf_grp2, rpf_interface_id=self.rif6)

        self.ipmc_route3 = sai_thrift_ipmc_entry_t(
            vr_id=self.default_vrf,
            type=SAI_IPMC_ENTRY_TYPE_SG,
            destination=sai_ipaddress(self.grp_ip1),
            source=sai_ipaddress(self.src_ip2))
        sai_thrift_create_ipmc_entry(
            self.client,
            self.ipmc_route3,
            rpf_group_id=self.rpf_grp2,
            output_group_id=self.ipmc_grp3)

        # (*, G) route to group 3
        self.rpf_grp_vlan = sai_thrift_create_rpf_group(self.client)
        self.rpf_mbr_vlan = sai_thrift_create_rpf_group_member(
            self.client,
            rpf_group_id=self.rpf_grp_vlan, rpf_interface_id=self.vlan10_rif)

        self.ipmc_route4 = sai_thrift_ipmc_entry_t(
            vr_id=self.default_vrf,
            type=SAI_IPMC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip3))
        sai_thrift_create_ipmc_entry(
            self.client,
            self.ipmc_route4,
            rpf_group_id=self.rpf_grp_vlan,
            output_group_id=self.ipmc_grp3)

    def tearDown(self):
        sai_thrift_remove_ipmc_entry(self.client, self.ipmc_route4)
        sai_thrift_remove_rpf_group_member(self.client, self.rpf_mbr_vlan)
        sai_thrift_remove_rpf_group(self.client, self.rpf_grp_vlan)
        sai_thrift_remove_ipmc_entry(self.client, self.ipmc_route3)
        sai_thrift_remove_rpf_group_member(self.client, self.rpf2_mbr)
        sai_thrift_remove_rpf_group(self.client, self.rpf_grp2)
        sai_thrift_remove_ipmc_entry(self.client, self.ipmc_route2)
        sai_thrift_remove_ipmc_entry(self.client, self.ipmc_route1)
        sai_thrift_remove_rpf_group_member(self.client, self.rpf1_mbr)
        sai_thrift_remove_rpf_group(self.client, self.rpf_grp1)

        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc3_mbr_svi)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc3_mbr7)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc3_mbr6)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc3_mbr3)
        sai_thrift_remove_ipmc_group(self.client, self.ipmc_grp3)

        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc2_lag_mbr)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc2_mbr2)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc2_mbr0)
        sai_thrift_remove_ipmc_group(self.client, self.ipmc_grp2)

        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc1_mbr3)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc1_mbr2)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc1_mbr1)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc1_mbr0)
        sai_thrift_remove_ipmc_group(self.client, self.ipmc_grp1)

        sai_thrift_remove_router_interface(self.client, self.vlan10_rif)
        sai_thrift_set_port_attribute(
            self.client, self.port9, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_mbr9)
        sai_thrift_remove_bridge_port(self.client, self.port9_bp)
        sai_thrift_set_port_attribute(
            self.client, self.port8, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_mbr8)
        sai_thrift_remove_bridge_port(self.client, self.port8_bp)
        sai_thrift_remove_vlan(self.client, self.vlan10)

        sai_thrift_remove_router_interface(self.client, self.rif7)
        sai_thrift_remove_router_interface(self.client, self.rif6)

        sai_thrift_remove_router_interface(self.client, self.lag_rif)
        sai_thrift_remove_lag_member(self.client, self.lag_mbr5)
        sai_thrift_remove_lag_member(self.client, self.lag_mbr4)
        sai_thrift_remove_lag(self.client, self.lag0)

        sai_thrift_remove_router_interface(self.client, self.rif3)
        sai_thrift_remove_router_interface(self.client, self.rif2)
        sai_thrift_remove_router_interface(self.client, self.rif1)
        sai_thrift_remove_router_interface(self.client, self.rif0)

        super(IPMCBaseTopo, self).tearDown()


class IPMCIpv4Test(IPMCBaseTopo):
    '''
    Tests for IPv4 multicast functionalities
    '''
    def runTest(self):
        try:
            self.ipmcXGTest()
            self.ipmcXGLagTest()
            self.ipmcSGTest()
            self.ipv4McastAdminStateTest()
            self.vlanRpfMbrTest()
        finally:
            pass

    def ipmcXGTest(self):
        '''
        Exercice ipv4_multicast_route_star_g
        Packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) route and Group 3 pointed by (S, G) route -
        should be received only on ports in Group 1
        '''
        print("\nipmcXGTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            ip_ttl=63)

        print("Sending packet to multicast group 1 \n"
              "IPv4 multicast hit, RPF pass, action route")
        send_packet(self, self.dev_port0, pkt)
        verify_packets(
            self, exp_pkt, [self.dev_port1, self.dev_port2, self.dev_port3])
        print("\tOK")

        print("Sending packet to multicast group 1 \n"
              "IPv4 multicast hit, RPF fail, action drop")
        send_packet(self, self.dev_port1, pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def ipmcXGLagTest(self):
        '''
        Exercice ipv4_multicast_route_star_g with a group containing LAG
        '''
        print("\nipmcXGLagTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip2,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip2,
            ip_ttl=63)

        print("Sending packet to multicast group 2 \n"
              "IPv4 multicast hit, RPF pass, action route")
        send_packet(self, self.dev_port0, pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [exp_pkt, exp_pkt],
            [[self.dev_port2], [self.dev_port4, self.dev_port5]])
        print("\tOK")

        print("Sending packet to multicast group 2 \n"
              "IPv4 multicast hit, RPF fail, action drop")
        send_packet(self, self.dev_port4, pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def ipmcSGTest(self):
        '''
        Exercice ipv4_multicast_route_s_g
        Packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) route and Group 3 pointed by (S, G) route -
        should be received only on ports in Group 3
        '''
        print("\nipmcSGTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            ip_ttl=63)

        print("Sending packet to multicast group 3 \n"
              "IPv4 multicast hit, RPF pass, action route")
        send_packet(self, self.dev_port6, pkt)
        verify_packets(
            self, exp_pkt,
            [self.dev_port3, self.dev_port7, self.dev_port8, self.dev_port9])
        print("\tOK")

        print("Sending packet to multicast group 3 \n"
              "IPv4 multicast hit, RPF fail, action drop")
        send_packet(self, self.dev_port7, pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def ipv4McastAdminStateTest(self):
        '''
        Verify multicast is disabled when IPv4 admin state on an ingress RIF
        is False
        '''
        print("\nipv4McastAdminStateTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            ip_ttl=63)

        print("Ingress RIF multicast disabled")
        status = sai_thrift_set_router_interface_attribute(
            self.client, self.rif0, v4_mcast_enable=False)
        self.assertEqual(status, SAI_STATUS_SUCCESS)
        print("Sending packet to multicast group 1 - drop")
        send_packet(self, self.dev_port0, pkt)
        verify_no_other_packets(self)
        print("\tOK")

        print("Ingress RIF multicast enabled")
        status = sai_thrift_set_router_interface_attribute(
            self.client, self.rif0, v4_mcast_enable=True)
        self.assertEqual(status, SAI_STATUS_SUCCESS)
        print("Sending packet to multicast group 1 - route")
        send_packet(self, self.dev_port0, pkt)
        verify_packets(
            self, exp_pkt, [self.dev_port1, self.dev_port2, self.dev_port3])
        print("\tOK")

    def vlanRpfMbrTest(self):
        '''
        Verify IPMC forwarding when RPF member is a RIF of type VLAN
        Additional VLAN is created with ports no. 10 and 11 acting as untagged
        '''
        print("\nvlanRpfMbrTest")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip3,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip3,
            ip_ttl=63)

        print("Sending packet to multicast group 3 (*, G) path "
              "using SVI RPF member")
        send_packet(self, self.dev_port8, pkt)
        verify_packets(
            self, exp_pkt, [self.dev_port3, self.dev_port6, self.dev_port7])
        send_packet(self, self.dev_port9, pkt)
        verify_packets(
            self, exp_pkt, [self.dev_port3, self.dev_port6, self.dev_port7])
        print("\tOK")


class IPMCIpv6Test(IPMCBaseTopo):
    '''
    Tests for IPv6 multicast functionalities
    '''
    def __init__(self):
        super(IPMCIpv6Test, self).__init__(ipv6=True)

    def runTest(self):
        try:
            self.ipmcXGTest()
            self.ipmcXGLagTest()
            self.ipmcSGTest()
            self.ipv6McastAdminStateTest()
            self.vlanRpfMbrTest()
        finally:
            pass

    def ipmcXGTest(self):
        '''
        Exercice ipv6_multicast_route_star_g
        Packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) route and Group 3 pointed by (S, G) route -
        should be received only on ports in Group 1
        '''
        print("\nipmcXGTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=63)

        print("Sending packet to multicast group 1 \n"
              "IPv4 multicast hit, RPF pass, action route")
        send_packet(self, self.dev_port0, pkt)
        verify_packets(
            self, exp_pkt, [self.dev_port1, self.dev_port2, self.dev_port3])
        print("\tOK")

        print("Sending packet to multicast group 1 \n"
              "IPv6 multicast hit, RPF fail, action drop")
        send_packet(self, self.dev_port1, pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def ipmcXGLagTest(self):
        '''
        Exercice ipv6_multicast_route_star_g with a group containing LAG
        '''
        print("\nipmcXGLagTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip2,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip2,
            ipv6_hlim=63)

        print("Sending packet to multicast group 2 \n"
              "IPv6 multicast hit, RPF pass, action route")
        send_packet(self, self.dev_port0, pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [exp_pkt, exp_pkt],
            [[self.dev_port2], [self.dev_port4, self.dev_port5]])
        print("\tOK")

        print("Sending packet to multicast group 2 \n"
              "IPv6 multicast hit, RPF fail, action drop")
        send_packet(self, self.dev_port4, pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def ipmcSGTest(self):
        '''
        Exercice ipv6_multicast_route_s_g
        Packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) route and Group 3 pointed by (S, G) route -
        should be received only on ports in Group 3
        '''
        print("\nipmcSGTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=63)

        print("Sending packet to multicast group 3 \n"
              "IPv6 multicast hit, RPF pass, action route")
        send_packet(self, self.dev_port6, pkt)
        verify_packets(
            self, exp_pkt,
            [self.dev_port3, self.dev_port7, self.dev_port8, self.dev_port9])
        print("\tOK")

        print("Sending packet to multicast group 3 \n"
              "IPv6 multicast hit, RPF fail, action drop")
        send_packet(self, self.dev_port7, pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def ipv6McastAdminStateTest(self):
        '''
        Verify multicast is disabled when IPv6 admin state on an ingress RIF
        is False
        '''
        print("\nipv6McastAdminStateTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=63)

        print("Ingress RIF multicast disabled")
        status = sai_thrift_set_router_interface_attribute(
            self.client, self.rif0, v6_mcast_enable=False)
        self.assertEqual(status, SAI_STATUS_SUCCESS)
        print("Sending packet to multicast group 1 - drop")
        send_packet(self, self.dev_port0, pkt)
        verify_no_other_packets(self)
        print("\tOK")

        print("Ingress RIF multicast enabled")
        status = sai_thrift_set_router_interface_attribute(
            self.client, self.rif0, v6_mcast_enable=True)
        print("Sending packet to multicast group 1 - route")
        send_packet(self, self.dev_port0, pkt)
        verify_packets(
            self, exp_pkt, [self.dev_port1, self.dev_port2, self.dev_port3])
        print("\tOK")

    def vlanRpfMbrTest(self):
        '''
        Verify IPMC forwarding when RPF member is a RIF of type VLAN
        Additional VLAN is created with ports no. 10 and 11 acting as untagged
        '''
        print("\nvlanRpfMbrTest")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip3,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip3,
            ipv6_hlim=63)

        print("Sending packet to multicast group 3 (*, G) path "
              "using SVI RPF member")
        send_packet(self, self.dev_port8, pkt)
        verify_packets(
            self, exp_pkt, [self.dev_port3, self.dev_port6, self.dev_port7])
        send_packet(self, self.dev_port9, pkt)
        verify_packets(
            self, exp_pkt, [self.dev_port3, self.dev_port6, self.dev_port7])
        print("\tOK")


class IPMCSviTestBase(IPMCBaseTopo):
    '''
    Helper class that adds IPMC member of type RIF of type VLAN to the
    configuration from IPMCBaseTopo: VLAN 20 is created with ports 10, 11 and
    LAG on ports 12 and 13; then RIF is ceated on the VLAN and the RIF is added
    as a member of ipmc_grp3.

    Args:
        ipv6 (bool): ipv6 configuraton indicator
    '''
    def __init__(self, ipv6=False):
        super(IPMCSviTestBase, self).__init__(ipv6=ipv6)

    def setUp(self):
        super(IPMCSviTestBase, self).setUp()

        self.vlan20 = sai_thrift_create_vlan(self.client, vlan_id=20)
        # port10 VLAN 10 member
        self.port10_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port10,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan20_mbr10 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.port10_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(
            self.client, self.port10, port_vlan_id=10)
        # port11 VLAN 10 member
        self.port11_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port11,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan20_mbr11 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.port11_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(
            self.client, self.port11, port_vlan_id=10)
        # LAG (port12, port13) VLAN 10 member
        self.lag = sai_thrift_create_lag(self.client)
        self.lag_mbr12 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag, port_id=self.port12)
        self.lag_mbr13 = sai_thrift_create_lag_member(
            self.client, lag_id=self.lag, port_id=self.port13)
        self.lag_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.lag,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.vlan20_lag_mbr = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.lag_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_lag_attribute(self.client, self.lag, port_vlan_id=10)

        self.vlan20_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            virtual_router_id=self.default_vrf,
            vlan_id=self.vlan20)

        # add SVI as a member of IPMC group 3
        self.ipmc3_svi_member = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp3, ipmc_output_id=self.vlan20_rif)

    def tearDown(self):
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc3_svi_member)
        sai_thrift_remove_router_interface(self.client, self.vlan20_rif)
        sai_thrift_set_lag_attribute(self.client, self.lag, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan20_lag_mbr)
        sai_thrift_remove_bridge_port(self.client, self.lag_bp)
        sai_thrift_remove_lag_member(self.client, self.lag_mbr13)
        sai_thrift_remove_lag_member(self.client, self.lag_mbr12)
        sai_thrift_remove_lag(self.client, self.lag)
        sai_thrift_set_port_attribute(self.client, self.port11, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan20_mbr11)
        sai_thrift_remove_bridge_port(self.client, self.port11_bp)
        sai_thrift_set_port_attribute(self.client, self.port10, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan20_mbr10)
        sai_thrift_remove_bridge_port(self.client, self.port10_bp)
        sai_thrift_remove_vlan(self.client, self.vlan20)

        super(IPMCSviTestBase, self).tearDown()


class IPMCSviIpv4Test(IPMCSviTestBase):
    '''
    Base test for IPv4 multicast functionalities when one of the IPMC group
    members is a RIF of type VLAN
    '''
    def runTest(self):
        try:
            self.ipmcSviTest()
        finally:
            pass

    def ipmcSviTest(self):
        '''
        Test IPMC forwarding when one of the IPMC group member is a RIF of type
        VLAN
        '''
        print("\nipmcSviTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            ip_ttl=63)

        print("Sending packet to multicast group 3 (with SVI)")
        send_packet(self, self.dev_port6, pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [exp_pkt]*7,
            [[self.dev_port3], [self.dev_port7], [self.dev_port8],
             [self.dev_port9], [self.dev_port10],
             [self.dev_port11], [self.dev_port12, self.dev_port13]])
        print("\tOK")


class IPMCSviIpv6Test(IPMCSviTestBase):
    '''
    Base test for IPv6 multicast functionalities when one of the IPMC group
    members is a RIF of type VLAN
    '''
    def __init__(self):
        super(IPMCSviIpv6Test, self).__init__(ipv6=True)

    def runTest(self):
        try:
            self.ipmcSviTest()
        finally:
            pass

    def ipmcSviTest(self):
        '''
        Test IPMC forwarding when one of the IPMC group member is a RIF of type
        VLAN
        '''
        print("\nipmcSviTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=63)

        print("Sending packet to multicast group 3 (with SVI)")
        send_packet(self, self.dev_port6, pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [exp_pkt]*7,
            [[self.dev_port3], [self.dev_port7], [self.dev_port8],
             [self.dev_port9], [self.dev_port10],
             [self.dev_port11], [self.dev_port12, self.dev_port13]])
        print("\tOK")


@group("tunnel")
class IPMCSviIpv4TunnelTest(IPMCSviTestBase):
    '''
    Test for IPv4 multicast functionalities when some of the IPMC group
    members are RIFs of type VLAN and the VLANs contain a member(s) of type
    tunnel
    '''
    def setUp(self):
        super(IPMCSviIpv4TunnelTest, self).setUp()

        # tunnel configuration
        self.uport_dev = self.dev_port14
        self.uport_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port14)

        self.vni1 = 1000
        self.vni2 = 2000
        self.tun_ip = "10.0.0.65"
        self.lpb_ip = "10.1.0.32"
        self.src_ip = "192.168.100.1"
        self.src_mac = "00:22:22:22:22:11"
        self.unbor_mac = "00:11:11:11:11:11"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun_ip),
            rif_id=self.uport_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(
            self.client, self.unbor, dst_mac_address=self.unbor_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun_ip),
            router_interface_id=self.uport_rif)

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
            decap_ttl_mode=SAI_TUNNEL_TTL_MODE_PIPE_MODEL,
            encap_ttl_mode=SAI_TUNNEL_TTL_MODE_PIPE_MODEL,
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
        self.decap_tunnel_map_entry_vlan10 = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=10,
                vni_id_key=self.vni1)

        self.encap_tunnel_map_entry_vlan10 = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=10,
                vni_id_value=self.vni1)

        self.decap_tunnel_map_entry_vlan20 = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=20,
                vni_id_key=self.vni2)

        self.encap_tunnel_map_entry_vlan20 = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=20,
                vni_id_value=self.vni2)

        self.p2p_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun_ip))

        # bridge port created on p2p tunnel
        self.p2p_tunnel_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member created using tunnel bridge_port
        self.p2p_tunnel_vlan20_mbr = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.p2p_tunnel_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

    def runTest(self):
        try:
            self.ipmcSviTunnelTest()
            self.ipmcMultipleSviTunnelTest()
            self.ipmcSviMultipleTunnelTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.p2p_tunnel_vlan20_mbr)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel)
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

        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)
        sai_thrift_remove_router_interface(self.client, self.uport_rif)

        super(IPMCSviIpv4TunnelTest, self).tearDown()

    def ipmcSviTunnelTest(self):
        '''
        Test IPMC forwarding when one of the IPMC group member is a RIF of type
        VLAN and one of the VLAN members is a tunnel
        '''
        print("\nipmcSviTunnelTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            ip_ttl=63)
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
                                inner_frame=exp_pkt))
        vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet to multicast group 3 (with SVI with tunnel)")
        send_packet(self, self.dev_port6, pkt)
        exp_pkts = [exp_pkt] * 7
        exp_pkts.append(vxlan_pkt)
        verify_each_packet_on_multiple_port_lists(
            self, exp_pkts,
            [[self.dev_port3], [self.dev_port7], [self.dev_port8],
             [self.dev_port9], [self.dev_port10], [self.dev_port11],
             [self.dev_port12, self.dev_port13], [self.uport_dev]])
        print("\tOK")

    def ipmcMultipleSviTunnelTest(self):
        '''
        Test IPMC forwarding when multiple IPMC group members are RIFs of type
        VLAN and these VLANs contain the same tunnel as their member
        '''
        print("\nipmcMultipleSviTunnelTest()")

        try:
            # add tunnel to vlan10 which is an SVI belonging to ipmc_grp3
            p2p_tunnel_vlan10_mbr = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=self.p2p_tunnel_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.src_mac,
                ip_src=self.src_ip2,
                ip_dst=self.grp_ip1,
                ip_ttl=64)
            exp_pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.default_rmac,
                ip_src=self.src_ip2,
                ip_dst=self.grp_ip1,
                ip_ttl=63)
            vxlan_pkt10 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=exp_pkt))
            vxlan_pkt10.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt20 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=exp_pkt))
            vxlan_pkt20.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet to multicast group 3 "
                  "(with 2 SVIs with the same tunnel)")
            send_packet(self, self.dev_port6, pkt)
            exp_pkts = [exp_pkt] * 2  # regular RIFs
            exp_pkts.extend([exp_pkt] * 3)  # VLAN 20
            exp_pkts.append(vxlan_pkt20)  # VLAN 20 tunnel
            exp_pkts.extend([exp_pkt] * 2)  # VLAN 10
            exp_pkts.append(vxlan_pkt10)  # VLAN 10 tunnel
            verify_each_packet_on_multiple_port_lists(
                self, exp_pkts,
                # regular ports from IPMCBaseTopo
                [[self.dev_port3], [self.dev_port7],
                 # VLAN 20 ports from IPMCSviTestBase + tunnel added above
                 [self.dev_port10], [self.dev_port11],
                 [self.dev_port12, self.dev_port13], [self.uport_dev],
                 # VLAN 10 ports from IPMCBaseTopo + tunnel from setUp()
                 [self.dev_port8], [self.dev_port9], [self.uport_dev]])
            print("\tOK")

        finally:
            sai_thrift_remove_vlan_member(self.client, p2p_tunnel_vlan10_mbr)

    def ipmcSviMultipleTunnelTest(self):
        '''
        Test IPMC forwarding when one of the IPMC group member is a RIF of type
        VLAN and the VLAN contains several members of type tunnel; also test
        addition and deletion of a VLAN member that is a tunnel
        '''
        print("\nipmcSviMultipleTunnelTest")

        tun2_ip = "10.0.0.33"
        unbor2_mac = "00:11:11:11:11:22"

        try:
            uport2_dev = self.dev_port15
            uport2_rif = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port15)

            unbor2 = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(tun2_ip),
                rif_id=uport2_rif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(
                self.client, unbor2, dst_mac_address=unbor2_mac)

            unhop2 = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(tun2_ip),
                router_interface_id=uport2_rif)

            p2p_tunnel2 = sai_thrift_create_tunnel(
                self.client,
                type=self.tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=self.decap_maps,
                encap_mappers=self.encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                encap_dst_ip=sai_ipaddress(tun2_ip))

            p2p_tunnel2_bp = sai_thrift_create_bridge_port(
                self.client,
                type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                tunnel_id=p2p_tunnel2,
                bridge_id=self.default_1q_bridge,
                admin_state=True,
                fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

            pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.src_mac,
                ip_src=self.src_ip2,
                ip_dst=self.grp_ip1,
                ip_ttl=64)
            exp_pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.default_rmac,
                ip_src=self.src_ip2,
                ip_dst=self.grp_ip1,
                ip_ttl=63)
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
                                    inner_frame=exp_pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=exp_pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')

            print("Adding tunnel to VLAN")
            p2p_tunnel2_vlan20_mbr = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan20,
                bridge_port_id=p2p_tunnel2_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            print("Sending packet to multicast group 3 "
                  "(with SVI with 2 tunnels)")
            send_packet(self, self.dev_port6, pkt)
            exp_pkts = [exp_pkt] * 7
            exp_pkts.append(vxlan_pkt)
            exp_pkts.append(vxlan_pkt2)
            verify_each_packet_on_multiple_port_lists(
                self, exp_pkts,
                [[self.dev_port3], [self.dev_port7], [self.dev_port8],
                 [self.dev_port9], [self.dev_port10], [self.dev_port11],
                 [self.dev_port12, self.dev_port13],
                 [self.uport_dev], [uport2_dev]])
            print("\tOK")

            print("Removing tunnel from VLAN ")
            sai_thrift_remove_vlan_member(self.client, p2p_tunnel2_vlan20_mbr)
            print("Sending packet to multicast group 3 "
                  "(with SVI with 1 remaining tunnel)")
            send_packet(self, self.dev_port6, pkt)
            exp_pkts = [exp_pkt] * 7
            exp_pkts.append(vxlan_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, exp_pkts,
                [[self.dev_port3], [self.dev_port7], [self.dev_port8],
                 [self.dev_port9], [self.dev_port10], [self.dev_port11],
                 [self.dev_port12, self.dev_port13],
                 [self.uport_dev]])
            print("\tOK")

        finally:
            sai_thrift_remove_bridge_port(self.client, p2p_tunnel2_bp)
            sai_thrift_remove_tunnel(self.client, p2p_tunnel2)
            sai_thrift_remove_next_hop(self.client, unhop2)
            sai_thrift_remove_neighbor_entry(self.client, unbor2)
            sai_thrift_remove_router_interface(self.client, uport2_rif)
