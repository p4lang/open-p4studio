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
Thrift SAI interface Spanning Tree tests
"""

from sai_base_test import *


# pylint: disable=too-many-public-methods
class L2StpTest(SaiHelperBase):
    """
    The class runs STP test cases
    """

    def setUp(self):
        super(L2StpTest, self).setUp()
        self.debug_cnt = None

        self.mac0 = '00:11:11:11:11:11'
        self.mac1 = '00:22:22:22:22:22'
        self.mac2 = '00:33:33:33:33:33'
        self.mac3 = '00:44:44:44:44:44'
        self.mac4 = '00:55:55:55:55:55'
        mac_action = SAI_PACKET_ACTION_FORWARD

        self.vlan10 = sai_thrift_create_vlan(self.client, vlan_id=10)
        self.vlan20 = sai_thrift_create_vlan(self.client, vlan_id=20)

        drop_reason = sai_thrift_s32_list_t(
            count=1, int32list=[SAI_IN_DROP_REASON_INGRESS_STP_FILTER])
        self.debug_cnt = sai_thrift_create_debug_counter(
            self.client,
            type=SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS,
            in_drop_reason_list=drop_reason)
        self.assertNotEqual(self.debug_cnt, 0)
        attr = sai_thrift_get_debug_counter_attribute(
            self.client, self.debug_cnt, index=True)
        self.dc_stp_drop_idx = attr['index'] \
            + SAI_PORT_STAT_IN_DROP_REASON_RANGE_BASE

        self.port24_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.port25_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port25,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.port26_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port26,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.port27_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port27,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.port28_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port28,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        # vlan10
        self.vlan10_member4 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client,
                                      self.port24,
                                      port_vlan_id=10)
        self.vlan10_member5 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port25_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client,
                                      self.port25,
                                      port_vlan_id=10)
        self.vlan10_member8 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.port28_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        # vlan20
        self.vlan20_member6 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.port26_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client,
                                      self.port26,
                                      port_vlan_id=20)
        self.vlan20_member7 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.port27_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        sai_thrift_set_port_attribute(self.client,
                                      self.port27,
                                      port_vlan_id=20)
        self.vlan20_member8 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.port28_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        self.fdb_entry24 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                  mac_address=self.mac0,
                                                  bv_id=self.vlan10)
        sai_thrift_create_fdb_entry(self.client,
                                    self.fdb_entry24,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.port24_bp,
                                    packet_action=mac_action)
        self.fdb_entry25 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                  mac_address=self.mac1,
                                                  bv_id=self.vlan10)
        sai_thrift_create_fdb_entry(self.client,
                                    self.fdb_entry25,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.port25_bp,
                                    packet_action=mac_action)
        self.fdb_entry26 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                  mac_address=self.mac2,
                                                  bv_id=self.vlan20)
        sai_thrift_create_fdb_entry(self.client,
                                    self.fdb_entry26,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.port26_bp,
                                    packet_action=mac_action)
        self.fdb_entry27 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                  mac_address=self.mac3,
                                                  bv_id=self.vlan20)
        sai_thrift_create_fdb_entry(self.client,
                                    self.fdb_entry27,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.port27_bp,
                                    packet_action=mac_action)

        self.stp = sai_thrift_create_stp(self.client)
        self.stp_port24 = sai_thrift_create_stp_port(
            self.client,
            stp=self.stp,
            bridge_port=self.port24_bp,
            state=SAI_STP_PORT_STATE_FORWARDING)
        self.stp_port25 = sai_thrift_create_stp_port(
            self.client,
            stp=self.stp,
            bridge_port=self.port25_bp,
            state=SAI_STP_PORT_STATE_FORWARDING)
        self.stp_port26 = sai_thrift_create_stp_port(
            self.client,
            stp=self.stp,
            bridge_port=self.port26_bp,
            state=SAI_STP_PORT_STATE_FORWARDING)
        self.stp_port27 = sai_thrift_create_stp_port(
            self.client,
            stp=self.stp,
            bridge_port=self.port27_bp,
            state=SAI_STP_PORT_STATE_FORWARDING)
        self.stp_port28 = sai_thrift_create_stp_port(
            self.client,
            stp=self.stp,
            bridge_port=self.port28_bp,
            state=SAI_STP_PORT_STATE_FORWARDING)

        sai_thrift_set_vlan_attribute(self.client,
                                      self.vlan10,
                                      stp_instance=self.stp)
        sai_thrift_set_vlan_attribute(self.client,
                                      self.vlan20,
                                      stp_instance=self.stp)

    def runTest(self):
        try:
            self.stpAttributesTest()
            self.stpBasicTest()
            self.stpLearningTest()
            self.stpEgressBlockingTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_set_vlan_attribute(self.client, self.vlan10, stp_instance=0)
        sai_thrift_set_vlan_attribute(self.client, self.vlan20, stp_instance=0)

        sai_thrift_remove_stp_port(self.client, self.stp_port28)
        sai_thrift_remove_stp_port(self.client, self.stp_port27)
        sai_thrift_remove_stp_port(self.client, self.stp_port26)
        sai_thrift_remove_stp_port(self.client, self.stp_port25)
        sai_thrift_remove_stp_port(self.client, self.stp_port24)

        sai_thrift_remove_stp(self.client, self.stp)

        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry27)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry26)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry25)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry24)
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_set_port_attribute(self.client, self.port27, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port26, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port25, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port24, port_vlan_id=0)

        sai_thrift_remove_vlan_member(self.client, self.vlan20_member8)
        sai_thrift_remove_vlan_member(self.client, self.vlan20_member7)
        sai_thrift_remove_vlan_member(self.client, self.vlan20_member6)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_member8)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_member5)
        sai_thrift_remove_vlan_member(self.client, self.vlan10_member4)

        sai_thrift_remove_bridge_port(self.client, self.port24_bp)
        sai_thrift_remove_bridge_port(self.client, self.port25_bp)
        sai_thrift_remove_bridge_port(self.client, self.port26_bp)
        sai_thrift_remove_bridge_port(self.client, self.port27_bp)
        sai_thrift_remove_bridge_port(self.client, self.port28_bp)

        sai_thrift_remove_vlan(self.client, self.vlan10)
        sai_thrift_remove_vlan(self.client, self.vlan20)
        sai_thrift_remove_debug_counter(self.client, self.debug_cnt)
        super(L2StpTest, self).tearDown()

    def stpAttributesTest(self):
        """
        Validate attribute get set
        """
        print("\nstpAttributesTest()")
        vlan30 = sai_thrift_create_vlan(self.client, vlan_id=30)
        try:
            # stp
            # get port_list
            attr = sai_thrift_get_stp_attribute(
                self.client,
                self.stp,
                port_list=sai_thrift_object_list_t(idlist=[], count=5))
            self.assertEqual(attr["port_list"].count, 5)
            self.assertTrue(self.stp_port24 in attr["port_list"].idlist)
            self.assertTrue(self.stp_port28 in attr["port_list"].idlist)

            # get vlan_list
            attr = sai_thrift_get_stp_attribute(
                self.client,
                self.stp,
                vlan_list=sai_thrift_vlan_list_t(idlist=[], count=2))
            self.assertEqual(attr["vlan_list"].count, 2)
            self.assertTrue(10 in attr["vlan_list"].idlist)
            self.assertTrue(20 in attr["vlan_list"].idlist)
            self.assertFalse(30 in attr["vlan_list"].idlist)

            # verify vlan list change
            attr = sai_thrift_get_stp_attribute(
                self.client,
                self.default_stp,
                vlan_list=sai_thrift_object_list_t(idlist=[], count=5))
            self.assertEqual(attr["vlan_list"].count, 2)
            self.assertTrue(1 in attr["vlan_list"].idlist)
            self.assertTrue(30 in attr["vlan_list"].idlist)

            # vlan 30 moves from default stp to new stp
            sai_thrift_set_vlan_attribute(self.client,
                                          vlan30,
                                          stp_instance=self.stp)

            # vlan 30 is absent in default stp
            attr = sai_thrift_get_stp_attribute(
                self.client,
                self.default_stp,
                vlan_list=sai_thrift_object_list_t(idlist=[], count=5))
            self.assertEqual(attr["vlan_list"].count, 1)
            self.assertFalse(30 in attr["vlan_list"].idlist)
            # vlan 30 is present in new stp
            attr = sai_thrift_get_stp_attribute(
                self.client,
                self.stp,
                vlan_list=sai_thrift_vlan_list_t(idlist=[], count=5))
            self.assertEqual(attr["vlan_list"].count, 3)
            self.assertTrue(30 in attr["vlan_list"].idlist)

            # stp_port
            attr = sai_thrift_get_stp_port_attribute(
                self.client,
                self.stp_port24,
                stp=True,
                bridge_port=True,
                state=True)
            self.assertEqual(self.stp, attr["stp"])
            self.assertEqual(self.port24_bp, attr["bridge_port"])
            self.assertEqual(SAI_STP_PORT_STATE_FORWARDING, attr["state"])
        finally:
            sai_thrift_remove_vlan(self.client, vlan30)

    def stpBasicTest(self):
        """
        Basic stp port state test
        """
        print("\nstpBasicTest()")
        pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                eth_src='00:22:22:22:22:22')

        pkt2 = simple_tcp_packet(eth_dst='00:33:33:33:33:33',
                                 eth_src='00:44:44:44:44:44')
        try:
            print("forward packet from port 25 to 24 in forwarding state")
            send_packet(self, self.dev_port25, pkt)
            verify_packet(self, pkt, self.dev_port24)

            print("drop packet from port 25 in blocking state")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port25,
                state=SAI_STP_PORT_STATE_BLOCKING)
            pre_stats = sai_thrift_get_debug_counter_port_stats(
                self.client, self.port25, [self.dc_stp_drop_idx])
            send_packet(self, self.dev_port25, pkt)
            verify_no_other_packets(self, timeout=1)
            time.sleep(5)
            post_stats = sai_thrift_get_debug_counter_port_stats(
                self.client, self.port25, [self.dc_stp_drop_idx])
            self.assertEqual(
                post_stats[self.dc_stp_drop_idx],
                pre_stats[self.dc_stp_drop_idx] + 1)

            print("forward packet from port 27 to 26 in forwarding state")
            send_packet(self, self.dev_port27, pkt2)
            verify_packet(self, pkt2, self.dev_port26)

            print("drop packet from port 27 in blocking state")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port27,
                state=SAI_STP_PORT_STATE_BLOCKING)
            send_packet(self, self.dev_port27, pkt2)
            verify_no_other_packets(self, timeout=1)

            print("forward packet again from port 25 to 24")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port25,
                state=SAI_STP_PORT_STATE_FORWARDING)
            pre_stats = sai_thrift_get_debug_counter_port_stats(
                self.client, self.port25, [self.dc_stp_drop_idx])
            send_packet(self, self.dev_port25, pkt)
            verify_packet(self, pkt, self.dev_port24)
            time.sleep(5)
            post_stats = sai_thrift_get_debug_counter_port_stats(
                self.client, self.port25, [self.dc_stp_drop_idx])
            self.assertEqual(
                post_stats[self.dc_stp_drop_idx],
                pre_stats[self.dc_stp_drop_idx])

            print("forward packet again from port 27 to 26")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port27,
                state=SAI_STP_PORT_STATE_FORWARDING)
            send_packet(self, self.dev_port27, pkt2)
            verify_packet(self, pkt2, self.dev_port26)
        finally:
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port25,
                state=SAI_STP_PORT_STATE_FORWARDING)
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port27,
                state=SAI_STP_PORT_STATE_FORWARDING)

    def stpLearningTest(self):
        """
        Basic stp port learning state test
        """
        print("\nstpLearningTest()")
        pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                eth_src='00:22:22:22:22:22')

        unknown_pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:77:77:77:77:77')

        pkt2 = simple_tcp_packet(eth_dst='00:77:77:77:77:77',
                                 eth_src='00:11:11:11:11:11')
        try:
            print("forward packet from port 25 to 24 in forwarding state")
            send_packet(self, self.dev_port25, pkt)
            verify_packet(self, pkt, self.dev_port24)

            print("drop packet from port 25 in learning state")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port25,
                state=SAI_STP_PORT_STATE_LEARNING)
            send_packet(self, self.dev_port25, pkt)
            verify_no_other_packets(self, timeout=1)

            print("Port 25 in learning, learn and drop")
            send_packet(self, self.dev_port25, unknown_pkt)
            verify_no_other_packets(self, timeout=1)
            time.sleep(5)

            print("Pkt from port 24 to port 25 should be dropped, learning")
            send_packet(self, self.dev_port24, pkt2)
            verify_no_other_packets(self, timeout=1)

            print("Reset port 25 to state forwarding")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port25,
                state=SAI_STP_PORT_STATE_FORWARDING)
            print("Pkt from port 24 to port 25 should now be forwarded")
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, pkt2, self.dev_port25)
        finally:
            time.sleep(15)
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port25,
                state=SAI_STP_PORT_STATE_FORWARDING)

    def stpEgressBlockingTest(self):
        """
        Basic egress stp port forwarding test
        """
        print("\nstpEgressBlockingTest()")
        unknown_pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:12',
                                        eth_src='00:11:11:11:11:11')
        unknown_exp_vlan_pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:12',
                                                 eth_src='00:11:11:11:11:11',
                                                 dl_vlan_enable=True,
                                                 vlan_vid=10,
                                                 pktlen=104)
        unknown_pkt2 = simple_tcp_packet(eth_dst='00:11:11:11:11:12',
                                         eth_src='00:33:33:33:33:33')
        unknown_exp_vlan_pkt2 = simple_tcp_packet(eth_dst='00:11:11:11:11:12',
                                                  eth_src='00:33:33:33:33:33',
                                                  dl_vlan_enable=True,
                                                  vlan_vid=20,
                                                  pktlen=104)

        try:
            print("Send unknown UC packet from 24, flood on 25 and 28")
            send_packet(self, self.dev_port24, unknown_pkt)
            verify_each_packet_on_each_port(
                self, [unknown_pkt, unknown_exp_vlan_pkt],
                [self.dev_port25, self.dev_port28])

            print("Port 25 in stp blocking state")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port25,
                state=SAI_STP_PORT_STATE_BLOCKING)

            print("Send unknown UC pkt from 24, 25 blocked, flood only to 28")
            send_packet(self, self.dev_port24, unknown_pkt)
            verify_packets(self, unknown_exp_vlan_pkt, [self.dev_port28])

            print("Port 28 in stp blocking state")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port28,
                state=SAI_STP_PORT_STATE_BLOCKING)

            print("Send unknown UC packet from 24, ports 25, 28 blocked")
            send_packet(self, self.dev_port24, unknown_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Send unknown UC packet from 26, flood on 27, 28 blocked")
            send_packet(self, self.dev_port26, unknown_pkt2)
            verify_packets(self, unknown_pkt2, [self.dev_port27])

            print("Port 28 in stp forwarding state")
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port28,
                state=SAI_STP_PORT_STATE_FORWARDING)

            print("Send unknown UC packet from 26, flood on 27 and 28")
            send_packet(self, self.dev_port26, unknown_pkt2)
            verify_each_packet_on_each_port(
                self, [unknown_pkt2, unknown_exp_vlan_pkt2],
                [self.dev_port27, self.dev_port28])

        finally:
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port25,
                state=SAI_STP_PORT_STATE_FORWARDING)
            sai_thrift_set_stp_port_attribute(
                self.client,
                self.stp_port28,
                state=SAI_STP_PORT_STATE_FORWARDING)
