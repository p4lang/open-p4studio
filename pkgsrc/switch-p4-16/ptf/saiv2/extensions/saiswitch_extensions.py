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
import binascii

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))

from sai_base_test import *
from saihash import SAIHashTest
from saihash import L3_MAX_ITRS
from saihash import verify_lb_active_ports
from saihash import verify_equaly_balanced


class SymmetricHashExtensionsTest(SAIHashTest):
    '''
    This test verifies symmetric hashing on ECMP
    '''

    max_itrs = L3_MAX_ITRS

    def setUp(self):

        super(SymmetricHashExtensionsTest, self).setUp()

        # setup IPv4 hash fields for all fields
        self.setupECMPIPv4Hash()

        # add reverse route
        self.route4 = sai_thrift_route_entry_t(
            switch_id=self.switch_id,
            destination=sai_ipprefix('192.168.8.1/8'),
            vr_id=self.default_vrf)
        status = sai_thrift_create_route_entry(
            self.client,
            self.route4,
            next_hop_id=self.nhop_group1)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route4)

        super(SymmetricHashExtensionsTest, self).tearDown()

    def runTest(self):
        try:
            self.symAttrTest()
            self.ecmpIPv4SymHashTest()
        finally:
            pass

    def symAttrTest(self):
        try:
            self.setupECMPIPv4Hash()
            self.setupECMPIPv6Hash()

            # v4 symmetric
            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(id=(SAI_SWITCH_ATTR_END + 5),
                                          value=attr_value)
            sai_thrift_set_switch_attribute(self.client, custom_attribute=attr)

            attrs = sai_thrift_get_switch_attribute(
                self.client, custom_attribute=attr)
            self.assertEqual(attrs['custom_attribute'].value.booldata, True)

            # v6 symmetric
            attr_value = sai_thrift_attribute_value_t(booldata=False)
            attr = sai_thrift_attribute_t(id=(SAI_SWITCH_ATTR_END + 6),
                                          value=attr_value)
            sai_thrift_set_switch_attribute(self.client, custom_attribute=attr)

            attrs = sai_thrift_get_switch_attribute(
                self.client, custom_attribute=attr)
            self.assertEqual(attrs['custom_attribute'].value.booldata, False)
        finally:
            attr_value = sai_thrift_attribute_value_t(booldata=False)
            attr = sai_thrift_attribute_t(id=(SAI_SWITCH_ATTR_END + 5),
                                          value=attr_value)
            sai_thrift_set_switch_attribute(self.client, custom_attribute=attr)
            attr_value = sai_thrift_attribute_value_t(booldata=False)
            attr = sai_thrift_attribute_t(id=(SAI_SWITCH_ATTR_END + 6),
                                          value=attr_value)
            sai_thrift_set_switch_attribute(self.client, custom_attribute=attr)

    def symPacketTest(
            self,
            dip='10.10.10.1',
            sip='192.168.8.1',
            dport=1234,
            sport=4321,
            l3=False,
            l4=False,
            flip=False):
        """
        Function that performs the IPv4 ECMP test with L3 hashed traffic

        Args:
            dip (string): starting dst IP address
            sip (string): starting src IP address
            dport (int): starting dst port
            sport (int): starting src port
            l3 (bool): Verify sip/dip algo
            l4 (bool): Verify sport/dport algo
            flip (bool): revere traffic

        Returns:
            list: list of numbers of packet egressed on specific test port
        """

        count = [0, 0, 0, 0]
        dst_ip = int(binascii.hexlify(socket.inet_aton(dip)), 16)
        src_ip = int(binascii.hexlify(socket.inet_aton(sip)), 16)
        tcp_sport = sport
        tcp_dport = dport
        for i in range(0, self.max_itrs):
            dst_ip_addr = socket.inet_ntoa(
                binascii.unhexlify(hex(dst_ip)[2:].zfill(8)))
            src_ip_addr = socket.inet_ntoa(
                binascii.unhexlify(hex(src_ip)[2:].zfill(8)))

            if False:
                print("dport", tcp_dport, "dst_ip:", dst_ip_addr,
                      "src_ip:", src_ip_addr, "sport", tcp_sport)
            pkt = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                    ip_dst=dst_ip_addr,
                                    ip_src=src_ip_addr,
                                    tcp_sport=tcp_sport,
                                    tcp_dport=tcp_dport,
                                    ip_ttl=64)

            exp_pkt1 = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                         eth_src=ROUTER_MAC,
                                         ip_dst=dst_ip_addr,
                                         ip_src=src_ip_addr,
                                         tcp_sport=tcp_sport,
                                         tcp_dport=tcp_dport,
                                         ip_ttl=63)

            exp_pkt2 = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                         eth_src=ROUTER_MAC,
                                         ip_dst=dst_ip_addr,
                                         ip_src=src_ip_addr,
                                         tcp_sport=tcp_sport,
                                         tcp_dport=tcp_dport,
                                         ip_ttl=63)

            exp_pkt3 = simple_tcp_packet(eth_dst='00:33:33:33:33:33',
                                         eth_src=ROUTER_MAC,
                                         ip_dst=dst_ip_addr,
                                         ip_src=src_ip_addr,
                                         tcp_sport=tcp_sport,
                                         tcp_dport=tcp_dport,
                                         ip_ttl=63)

            exp_pkt4 = simple_tcp_packet(eth_dst='00:44:44:44:44:44',
                                         eth_src=ROUTER_MAC,
                                         ip_dst=dst_ip_addr,
                                         ip_src=src_ip_addr,
                                         tcp_sport=tcp_sport,
                                         tcp_dport=tcp_dport,
                                         ip_ttl=63)
            send_packet(self, self.dev_port15, pkt)
            ports_to_verify = [
                self.dev_port10,
                self.dev_port11,
                self.dev_port12,
                self.dev_port13]
            rcv_idx = verify_any_packet_any_port(self,
                                                 [exp_pkt1,
                                                  exp_pkt2,
                                                  exp_pkt3,
                                                  exp_pkt4],
                                                 ports_to_verify,
                                                 timeout=2)
            count[rcv_idx] += 1
            if l3:
                if flip:
                    dst_ip += i * 7
                else:
                    src_ip += i * 7
            if l4:
                tcp_sport += 13
                tcp_dport += 13

        nbr_active_ports = verify_lb_active_ports(count)
        self.assertTrue(nbr_active_ports == 4)
        equaly_balanced = verify_equaly_balanced(count)
        self.assertTrue(equaly_balanced,
                        "Ecmp paths are not equally balanced")
        return count

    def ecmpIPv4SymHashTest(self):
        """
        Verifies symmetric hashing for ECMP IPv4 Hash
        """

        attr_value = sai_thrift_attribute_value_t(booldata=True)
        attr = sai_thrift_attribute_t(id=(SAI_SWITCH_ATTR_END + 5),
                                      value=attr_value)
        sai_thrift_set_switch_attribute(self.client, custom_attribute=attr)

        try:
            print('\n')

            count1 = self.symPacketTest(l3=True, l4=True)
            print("Forward L3=True, L4=True:", count1)

            dip = '192.168.8.1'
            sip = '10.10.10.1'
            dport = 4321
            sport = 1234
            count2 = self.symPacketTest(
                dip, sip, dport, sport, l3=True, l4=True, flip=True)
            print("Reverse L3=True, L4=True:", count2)
            self.assertEqual(count1, count2)

            count1 = self.symPacketTest(l3=True)
            print("Forward L3=True, L4=False:", count1)

            dip = '192.168.8.1'
            sip = '10.10.10.1'
            count2 = self.symPacketTest(dip, sip, l3=True, flip=True)
            print("Reverse L3=True, L4=False:", count2)
            self.assertEqual(count1, count2)
        finally:
            attr_value = sai_thrift_attribute_value_t(booldata=False)
            attr = sai_thrift_attribute_t(id=(SAI_SWITCH_ATTR_END + 5),
                                          value=attr_value)
            sai_thrift_set_switch_attribute(self.client, custom_attribute=attr)
