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
Thrift SAI interface host interface extensions tests
"""

# pylint: disable=wrong-import-position

import os
import sys

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))
from sai_base_test import *
from saihostif import HostifTrapTypeBaseClass


class HostifTrapTypesExtensionsTest(HostifTrapTypeBaseClass):
    '''
    This verifies creation of different types of hostif traps
    If action is TRAP - only iport is in use, if action is COPY
    packet is send on iport and should be forwarded to eport
    '''

    def runTest(self):
        try:
            self.icmpTrapTest()
            self.icmpv6TrapTest()
            self.iccpTrapTest()
            self.arpSuppTrapTest(
                SAI_HOSTIF_TRAP_TYPE_END + 0)
            self.arpSuppTrapTest(
                SAI_HOSTIF_TRAP_TYPE_ROUTER_CUSTOM_RANGE_BASE + 1)
            self.ndSuppTrapTest(
                SAI_HOSTIF_TRAP_TYPE_END + 4)
            self.ndSuppTrapTest(
                SAI_HOSTIF_TRAP_TYPE_ROUTER_CUSTOM_RANGE_BASE + 2)
        finally:
            pass

    def icmpTrapTest(self):
        '''
        This verifies trap of type ICMP
        '''
        print("\nicmpTrapTest()")

        try:
            icmp_trap = sai_thrift_create_hostif_trap(
                self.client,
                packet_action=SAI_PACKET_ACTION_TRAP,
                trap_type=SAI_HOSTIF_TRAP_TYPE_END + 1)
            self.assertTrue(icmp_trap != 0)

            icmp_pkt = simple_icmp_packet(eth_dst=ROUTER_MAC,
                                          ip_dst=self.my_ip,
                                          icmp_type=8,
                                          icmp_data='000102030405')

            print("Sending ICMP packet")
            send_packet(self, self.dev_port24, icmp_pkt)
            self.assertTrue(socket_verify_packet(icmp_pkt, self.hif_socket))
            send_packet(self, self.dev_port10, icmp_pkt)
            self.assertTrue(socket_verify_packet(icmp_pkt, self.hif_socket2))
            print("\tOK")

            print("Verifying CPU port queue stats")
            time.sleep(4)
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue0)
            self.assertEqual(
                post_stats["SAI_QUEUE_STAT_PACKETS"],
                self.cpu_queue_state + 2)
            print("\tOK")

            self.cpu_queue_state = post_stats["SAI_QUEUE_STAT_PACKETS"]

        finally:
            sai_thrift_remove_hostif_trap(self.client, icmp_trap)

    def icmpv6TrapTest(self):
        '''
        This verifies trap of type ICMPv6
        '''
        print("\nicmpv6TrapTest()")

        try:
            icmpv6_trap = sai_thrift_create_hostif_trap(
                self.client,
                packet_action=SAI_PACKET_ACTION_TRAP,
                trap_type=SAI_HOSTIF_TRAP_TYPE_END + 2)
            self.assertTrue(icmpv6_trap != 0)

            icmpv6_pkt = simple_icmpv6_packet(eth_dst=ROUTER_MAC,
                                              ipv6_dst=self.my_ipv6,
                                              icmp_type=128)

            print("Sending ICMPv6 packet")
            send_packet(self, self.dev_port24, icmpv6_pkt)
            self.assertTrue(socket_verify_packet(icmpv6_pkt, self.hif_socket))
            send_packet(self, self.dev_port10, icmpv6_pkt)
            self.assertTrue(socket_verify_packet(icmpv6_pkt, self.hif_socket2))
            print("\tOK")

            print("Verifying CPU port queue stats")
            time.sleep(4)
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue0)
            self.assertEqual(
                post_stats["SAI_QUEUE_STAT_PACKETS"],
                self.cpu_queue_state + 2)
            print("\tOK")

            self.cpu_queue_state = post_stats["SAI_QUEUE_STAT_PACKETS"]

        finally:
            sai_thrift_remove_hostif_trap(self.client, icmpv6_trap)

    def iccpTrapTest(self):
        '''
        This verifies trap of type ICCP
        '''
        print("\niccpTrapTest()")

        try:
            iccp_trap = sai_thrift_create_hostif_trap(
                self.client,
                packet_action=SAI_PACKET_ACTION_TRAP,
                trap_type=SAI_HOSTIF_TRAP_TYPE_END + 3)
            self.assertTrue(iccp_trap != 0)

            iccp_pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                         ip_dst=self.my_ip,
                                         tcp_dport=8888)

            print("Sending ICCP packet")
            send_packet(self, self.dev_port10, iccp_pkt)
            self.assertTrue(socket_verify_packet(iccp_pkt, self.hif_socket2))
            print("\tOK")

            print("Verifying CPU port queue stats")
            time.sleep(4)
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue0)
            self.assertEqual(
                post_stats["SAI_QUEUE_STAT_PACKETS"],
                self.cpu_queue_state + 1)
            print("\tOK")

            self.cpu_queue_state = post_stats["SAI_QUEUE_STAT_PACKETS"]

        finally:
            sai_thrift_remove_hostif_trap(self.client, iccp_trap)

    def arpSuppTrapTest(self, trap_type):
        '''
        This verifies trap of type ARP suppression
        '''
        print("\narpTrapTest()")

        try:
            arp_req_trap = sai_thrift_create_hostif_trap(
                self.client,
                packet_action=SAI_PACKET_ACTION_TRAP,
                trap_type=SAI_HOSTIF_TRAP_TYPE_ARP_REQUEST,
                trap_priority=10)
            self.assertTrue(arp_req_trap != 0)
            arp_sup_trap = sai_thrift_create_hostif_trap(
                self.client,
                packet_action=SAI_PACKET_ACTION_DROP,
                trap_type=trap_type,
                trap_priority=20)
            self.assertTrue(arp_sup_trap != 0)

            # Broadcast ARP Request
            arp_pkt = simple_arp_packet(arp_op=1, pktlen=100)

            print("Sending broadcast ARP request")
            send_packet(self, self.dev_port24, arp_pkt)  # CPU queue++
            self.assertTrue(socket_verify_packet(arp_pkt, self.hif_socket))
            print("\tOK")

            print("Verifying CPU port queue stats")
            time.sleep(4)
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue0)
            self.assertEqual(
                post_stats["SAI_QUEUE_STAT_PACKETS"],
                self.cpu_queue_state + 1)
            print("\tOK")

            self.cpu_queue_state = post_stats["SAI_QUEUE_STAT_PACKETS"]

            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(id=(SAI_VLAN_ATTR_END + 0),
                                          value=attr_value)
            sai_thrift_set_vlan_attribute(
                self.client,
                self.vlan100,
                custom_attribute=attr)

            print("Sending broadcast ARP request, suppressed")
            send_packet(self, self.dev_port24, arp_pkt)

            print("Verifying CPU port queue stats")
            time.sleep(4)
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue0)
            self.assertEqual(
                post_stats["SAI_QUEUE_STAT_PACKETS"],
                self.cpu_queue_state)
            print("\tOK")

        finally:
            attr_value = sai_thrift_attribute_value_t(booldata=False)
            attr = sai_thrift_attribute_t(id=(SAI_VLAN_ATTR_END + 0),
                                          value=attr_value)
            sai_thrift_set_vlan_attribute(
                self.client,
                self.vlan100,
                custom_attribute=attr)
            sai_thrift_remove_hostif_trap(self.client, arp_sup_trap)
            sai_thrift_remove_hostif_trap(self.client, arp_req_trap)

    def ndSuppTrapTest(self, trap_type):
        '''
        This verifies trap of type ICMPv6
        '''
        print("\nicmpv6TrapTest()")

        try:
            nd_trap = sai_thrift_create_hostif_trap(
                self.client,
                packet_action=SAI_PACKET_ACTION_TRAP,
                trap_type=SAI_HOSTIF_TRAP_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                trap_priority=10)
            self.assertTrue(nd_trap != 0)
            nd_sup_trap = sai_thrift_create_hostif_trap(
                self.client,
                packet_action=SAI_PACKET_ACTION_DROP,
                trap_type=trap_type,
                trap_priority=20)
            self.assertTrue(nd_sup_trap != 0)

            icmpv6_pkt = simple_icmpv6_packet(eth_dst=ROUTER_MAC,
                                              ipv6_dst=self.my_ipv6,
                                              icmp_type=134)


            print("Sending broadcast ARP request")
            send_packet(self, self.dev_port24, icmpv6_pkt)  # CPU queue++
            self.assertTrue(socket_verify_packet(icmpv6_pkt, self.hif_socket))
            print("\tOK")

            print("Verifying CPU port queue stats")
            time.sleep(4)
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue0)
            self.assertEqual(
                post_stats["SAI_QUEUE_STAT_PACKETS"],
                self.cpu_queue_state + 1)
            print("\tOK")

            self.cpu_queue_state = post_stats["SAI_QUEUE_STAT_PACKETS"]

            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(id=(SAI_VLAN_ATTR_END + 0),
                                          value=attr_value)
            sai_thrift_set_vlan_attribute(
                self.client,
                self.vlan100,
                custom_attribute=attr)

            print("Sending broadcast ND, suppressed")
            send_packet(self, self.dev_port24, icmpv6_pkt)

            print("Verifying CPU port queue stats")
            time.sleep(4)
            post_stats = sai_thrift_get_queue_stats(
                self.client, self.cpu_queue0)
            self.assertEqual(
                post_stats["SAI_QUEUE_STAT_PACKETS"],
                self.cpu_queue_state)
            print("\tOK")

        finally:
            attr_value = sai_thrift_attribute_value_t(booldata=False)
            attr = sai_thrift_attribute_t(id=(SAI_VLAN_ATTR_END + 0),
                                          value=attr_value)
            sai_thrift_set_vlan_attribute(
                self.client,
                self.vlan100,
                custom_attribute=attr)
            sai_thrift_remove_hostif_trap(self.client, nd_trap)
            sai_thrift_remove_hostif_trap(self.client, nd_sup_trap)

