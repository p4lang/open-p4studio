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
Thrift SAI interface MYMAC tests
"""

from sai_base_test import *


class L3MymacTest(SaiHelper):
    '''
    Mymac test class
    '''
    def setUp(self):
        super(L3MymacTest, self).setUp()
        print()

        self.mymac = '00:77:66:55:55:55'
        self.mymac_mask = 'ff:ff:ff:ff:ff:ff'
        dmac = '00:11:22:33:44:55'

        self.nhop1 = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(self.client, self.neighbor_entry,
                                         dst_mac_address=dmac)

        self.route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(self.client, self.route_entry1,
                                      next_hop_id=self.nhop1)

        self.pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.pkt2 = simple_tcp_packet(
            eth_dst='00:77:66:55:55:55',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.exp_pkt2 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

    def runTest(self):
        try:
            self.mymacUpadateTest()
            self.mymacAnyRifTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route_entry1)
        sai_thrift_remove_neighbor_entry(self.client, self.neighbor_entry)
        sai_thrift_remove_next_hop(self.client, self.nhop1)
        super(L3MymacTest, self).tearDown()

    def mymacUpadateTest(self):
        '''
        mymac update test
        '''
        print("mymacUpadateTest")

        try:
            print("Sending packet with dmac as rmac, forward")
            send_packet(self, self.dev_port11, self.pkt1)
            verify_packet(self, self.exp_pkt, self.dev_port10)

            print("Sending packet with dmac as mymac, drop")
            send_packet(self, self.dev_port11, self.pkt2)
            verify_no_other_packets(self, timeout=3)

            print("update mymac")
            my_mac_oid = sai_thrift_create_my_mac(
                self.client, port_id=self.port11,
                mac_address=self.mymac,
                mac_address_mask=self.mymac_mask)

            print("Sending packet with dmac as mymac, forward")
            send_packet(self, self.dev_port11, self.pkt2)
            verify_packet(self, self.exp_pkt, self.dev_port10)

            print("Sending packet with dmac as rmac, forward")
            send_packet(self, self.dev_port11, self.pkt1)
            verify_packet(self, self.exp_pkt, self.dev_port10)

        finally:
            sai_thrift_remove_my_mac(self.client, my_mac_oid)

    def mymacAnyRifTest(self):
        '''
        mymac multiple RIF test
        '''
        print("mymacAnyRifTest")

        try:
            print("update mymac")
            my_mac_oid = sai_thrift_create_my_mac(
                self.client, mac_address=self.mymac,
                mac_address_mask=self.mymac_mask)

            print("Sending packet with dmac as mymac on rif1, forward")
            send_packet(self, self.dev_port11, self.pkt2)
            verify_packet(self, self.exp_pkt, self.dev_port10)

            print("Sending packet with dmac as mymac on rif2, forward")
            send_packet(self, self.dev_port12, self.pkt2)
            verify_packet(self, self.exp_pkt, self.dev_port10)

            print("Sending packet with dmac as mymac on rif3, forward")
            send_packet(self, self.dev_port13, self.pkt2)
            verify_packet(self, self.exp_pkt, self.dev_port10)

        finally:
            sai_thrift_remove_my_mac(self.client, my_mac_oid)
