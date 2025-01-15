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
Thrift SAI interface RIF tests
"""

from sai_base_test import *

# pylint: disable=import-error,wrong-import-order
from bfd_utils import bfd_ipv4_packet

# TODO
#  A system MUST NOT periodically transmit BFD Control packets if
#  bfd.RemoteMinRxInterval is zero.


class BFDSessionTest(SaiHelper):
    '''
    Test BFD session basic behaviour
    '''

    def runTest(self):
        print()
        try:
            self.sessionActiveVerifyCreateAttributes()
            self.sessionActiveDownInitUpLocalNegotiatedTest()
            self.sessionActiveDownInitUpTest()
            self.sessionActiveDownUpTest()
            self.sessionPassiveDownUpTest()
            self.sessionInitLocalExpireTest()
            self.sessionUpLocalExpireTest()
            self.sessionUpRemoteExpireTest()
            self.sessionNotConfiguredTest()
            self.sessionUpRemoteParamUpdateTest()
        finally:
            pass

    def setUp(self):
        super(BFDSessionTest, self).setUp()

        self.local_ip = '192.168.68.1'
        self.remote_mac = '00:22:33:44:55:66'
        self.remote_ip = '192.168.67.1'

        self.trap = None
        self.ip2me_route = None
        self.nbor = None
        self.nhop = None
        self.route = None

        self.trap = sai_thrift_create_hostif_trap(
            self.client,
            trap_group=self.default_trap_group,
            trap_type=SAI_HOSTIF_TRAP_TYPE_BFD,
            packet_action=SAI_PACKET_ACTION_TRAP)
        self.assertTrue(self.trap != 0)
        self.ip2me_route = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.local_ip + '/32'))
        status = sai_thrift_create_route_entry(
            self.client, self.ip2me_route, next_hop_id=self.cpu_port_hdl)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.nbor = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress(self.remote_ip))
        status = sai_thrift_create_neighbor_entry(
            self.client,
            self.nbor,
            dst_mac_address=self.remote_mac)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.remote_ip),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        self.assertTrue(self.nhop != 0)

        self.route = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix(self.remote_ip + '/32'))
        status = sai_thrift_create_route_entry(
            self.client,
            self.route,
            next_hop_id=self.nhop)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route)
        sai_thrift_remove_next_hop(self.client, self.nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.nbor)
        sai_thrift_remove_route_entry(self.client, self.ip2me_route)
        sai_thrift_remove_hostif_trap(self.client, self.trap)

        super(BFDSessionTest, self).tearDown()

    def sessionAttrGet(self, session: sai_object_id_t) -> dict:
        '''Returns attributes for the session
        :param session: BFD session oid
        :return: dict of attributes
        :rtype: dict
        '''
        return sai_thrift_get_bfd_session_attribute(
            self.client,
            session,
            type=True,
            hw_lookup_valid=True,
            virtual_router=True,
            local_discriminator=True,
            remote_discriminator=True,
            udp_src_port=True,
            bfd_encapsulation_type=True,
            iphdr_version=True,
            src_ip_address=True,
            dst_ip_address=True,
            min_tx=True,
            min_rx=True,
            multiplier=True,
            remote_min_tx=True,
            remote_min_rx=True,
            state=True,
            offload_type=True,
            negotiated_tx=True,
            negotiated_rx=True,
            local_diag=True,
            remote_diag=True,
            remote_multiplier=True)

    def sessionActiveVerifyCreateAttributes(self):
        '''
        Verify active session attributes after creation
        '''

        print('sessionActiveVerifyCreateAttributes()')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=250000,
            min_rx=250000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["min_tx"], 250000)
        self.assertEqual(attr["min_rx"], 250000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 0)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 0)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 0)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr['negotiated_tx'], 0)
        self.assertEqual(attr['negotiated_rx'], 0)

    def sessionActiveDownInitUpTest(self):
        '''
        Verify active session's down-init-up transition
        '''

        print('sessionActiveDownInitUpTest()')

        exp_pkt_down = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=1,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=250000,
            echo_rx_interval=0)
        exp_pkt_down = Mask(exp_pkt_down)
        exp_pkt_down.set_do_not_care_scapy(IP, 'id')
        exp_pkt_down.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_down.set_do_not_care_scapy(UDP, 'chksum')

        pkt_down = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=1,
            detect_mult=4,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=2000000,
            min_rx_interval=2000000,
            echo_rx_interval=0)

        exp_pkt_init = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=2,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=1000000,
            min_rx_interval=250000,
            echo_rx_interval=0)
        exp_pkt_init = Mask(exp_pkt_init)
        exp_pkt_init.set_do_not_care_scapy(IP, 'id')
        exp_pkt_init.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_init.set_do_not_care_scapy(UDP, 'chksum')

        pkt_up = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=3,
            detect_mult=4,
            my_discriminator=321,
            your_discriminator=123,
            min_tx_interval=2000000,
            min_rx_interval=2000000,
            echo_rx_interval=0)

        exp_pkt_up = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=3,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=250000,
            min_rx_interval=250000,
            echo_rx_interval=0)
        exp_pkt_up = Mask(exp_pkt_up)
        exp_pkt_up.set_do_not_care_scapy(IP, 'id')
        exp_pkt_up.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_up.set_do_not_care_scapy(UDP, 'chksum')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=250000,
            min_rx=250000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            verify_packet(self, exp_pkt_down, self.dev_port10, timeout=1)
            send_packet(self, self.dev_port10, pkt_down)
            verify_packet(self, exp_pkt_init, self.dev_port10, timeout=1)
            send_packet(self, self.dev_port10, pkt_up)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=8)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["remote_discriminator"], 321)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 250000)
        self.assertEqual(attr["min_rx"], 250000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_UP)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 0)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_min_tx"], 2000000)
        self.assertEqual(attr["remote_min_rx"], 2000000)
        self.assertEqual(attr["remote_multiplier"], 4)
        self.assertEqual(attr['negotiated_tx'], 2000000)
        self.assertEqual(attr['negotiated_rx'], 8000000)

    def sessionActiveDownUpTest(self):
        '''
        Verify active session's down-up transition
        '''
        pkt_init = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=2,
            detect_mult=4,
            my_discriminator=321,
            your_discriminator=123,
            min_tx_interval=2000000,
            min_rx_interval=2000000,
            echo_rx_interval=0)

        exp_pkt_up = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=3,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=250000,
            min_rx_interval=250000,
            echo_rx_interval=0)
        exp_pkt_up = Mask(exp_pkt_up)
        exp_pkt_up.set_do_not_care_scapy(IP, 'id')
        exp_pkt_up.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_up.set_do_not_care_scapy(UDP, 'chksum')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=250000,
            min_rx=250000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_init)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=8)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["remote_discriminator"], 321)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 250000)
        self.assertEqual(attr["min_rx"], 250000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_UP)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 0)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_min_tx"], 2000000)
        self.assertEqual(attr["remote_min_rx"], 2000000)
        self.assertEqual(attr["remote_multiplier"], 4)
        self.assertEqual(attr['negotiated_tx'], 2000000)
        self.assertEqual(attr['negotiated_rx'], 8000000)

    def sessionPassiveDownUpTest(self):
        '''
        Verify passive session's down-up transition
        '''

        print('sessionPassiveDownUpTest()')

        exp_pkt_down = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=1,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=250000,
            echo_rx_interval=0)
        exp_pkt_down = Mask(exp_pkt_down)
        exp_pkt_down.set_do_not_care_scapy(IP, 'id')
        exp_pkt_down.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_down.set_do_not_care_scapy(UDP, 'chksum')

        pkt_down = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=1,
            detect_mult=4,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=2000000,
            min_rx_interval=2000000,
            echo_rx_interval=0)

        exp_pkt_init = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=2,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=1000000,
            min_rx_interval=250000,
            echo_rx_interval=0)
        exp_pkt_init = Mask(exp_pkt_init)
        exp_pkt_init.set_do_not_care_scapy(IP, 'id')
        exp_pkt_init.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_init.set_do_not_care_scapy(UDP, 'chksum')

        pkt_up = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=3,
            detect_mult=4,
            my_discriminator=321,
            your_discriminator=123,
            min_tx_interval=2000000,
            min_rx_interval=2000000,
            echo_rx_interval=0)

        exp_pkt_up = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=3,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=250000,
            min_rx_interval=250000,
            echo_rx_interval=0)
        exp_pkt_up = Mask(exp_pkt_up)
        exp_pkt_up.set_do_not_care_scapy(IP, 'id')
        exp_pkt_up.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_up.set_do_not_care_scapy(UDP, 'chksum')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=250000,
            min_rx=250000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            verify_no_packet(self, exp_pkt_down, self.dev_port10, timeout=3)
            send_packet(self, self.dev_port10, pkt_down)
            verify_packet(self, exp_pkt_init, self.dev_port10, timeout=1)
            send_packet(self, self.dev_port10, pkt_up)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=8)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["remote_discriminator"], 321)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 250000)
        self.assertEqual(attr["min_rx"], 250000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_UP)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 0)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_min_tx"], 2000000)
        self.assertEqual(attr["remote_min_rx"], 2000000)
        self.assertEqual(attr["remote_multiplier"], 4)
        self.assertEqual(attr['negotiated_tx'], 2000000)
        self.assertEqual(attr['negotiated_rx'], 8000000)

    def sessionActiveDownInitUpLocalNegotiatedTest(self):
        '''
        Verify active session's down-init-up transition
        '''
        exp_pkt_down = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=1,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=2000000,
            echo_rx_interval=0)
        exp_pkt_down = Mask(exp_pkt_down)
        exp_pkt_down.set_do_not_care_scapy(IP, 'id')
        exp_pkt_down.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_down.set_do_not_care_scapy(UDP, 'chksum')

        pkt_down = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=1,
            detect_mult=5,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=250000,
            echo_rx_interval=0)

        exp_pkt_init = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=2,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=1000000,
            min_rx_interval=2000000,
            echo_rx_interval=0)
        exp_pkt_init = Mask(exp_pkt_init)
        exp_pkt_init.set_do_not_care_scapy(IP, 'id')
        exp_pkt_init.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_init.set_do_not_care_scapy(UDP, 'chksum')

        pkt_up = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=3,
            detect_mult=5,
            my_discriminator=321,
            your_discriminator=123,
            min_tx_interval=1000000,
            min_rx_interval=250000,
            echo_rx_interval=0)

        exp_pkt_up = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=3,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=500000,
            min_rx_interval=2000000,
            echo_rx_interval=0)
        exp_pkt_up = Mask(exp_pkt_up)
        exp_pkt_up.set_do_not_care_scapy(IP, 'id')
        exp_pkt_up.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_up.set_do_not_care_scapy(UDP, 'chksum')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=500000,
            min_rx=2000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            verify_packet(self, exp_pkt_down, self.dev_port10, timeout=1)
            send_packet(self, self.dev_port10, pkt_down)
            verify_packet(self, exp_pkt_init, self.dev_port10, timeout=1)
            attr_init = self.sessionAttrGet(session)
            send_packet(self, self.dev_port10, pkt_up)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=10)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr_init["state"], SAI_BFD_SESSION_STATE_INIT)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["remote_discriminator"], 321)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 500000)
        self.assertEqual(attr["min_rx"], 2000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_UP)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 0)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_min_tx"], 1000000)
        self.assertEqual(attr["remote_min_rx"], 250000)
        self.assertEqual(attr["remote_multiplier"], 5)
        self.assertEqual(attr['negotiated_tx'], 500000)
        self.assertEqual(attr['negotiated_rx'], 10000000)

    def sessionInitLocalExpireTest(self):
        '''
        Verify session init-expire transition
        '''

        print('sessionInitLocalExpireTest()')

        pkt_down = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=1,
            detect_mult=3,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)

        exp_pkt_init = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=2,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_init = Mask(exp_pkt_init)
        exp_pkt_init.set_do_not_care_scapy(IP, 'id')
        exp_pkt_init.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_init.set_do_not_care_scapy(UDP, 'chksum')

        exp_pkt_down_expire = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=1,
            version=1,
            diag=1,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_down_expire = Mask(exp_pkt_down_expire)
        exp_pkt_down_expire.set_do_not_care_scapy(IP, 'id')
        exp_pkt_down_expire.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_down_expire.set_do_not_care_scapy(UDP, 'chksum')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=1000000,
            min_rx=1000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_down)
            for _ in range(3):
                verify_packet(self, exp_pkt_init, self.dev_port10, timeout=2)
            verify_packet(self, exp_pkt_down_expire,
                          self.dev_port10, timeout=3)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 1000000)
        self.assertEqual(attr["min_rx"], 1000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 1)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 1000000)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 3)
        self.assertEqual(attr['negotiated_tx'], 1000000)
        self.assertEqual(attr['negotiated_rx'], 3000000)

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=1000000,
            min_rx=1000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_down)
            for _ in range(3):
                verify_packet(self, exp_pkt_init, self.dev_port10, timeout=2)
            verify_no_packet(self, exp_pkt_down_expire,
                             self.dev_port10, timeout=3)
            verify_no_other_packets(self, timeout=3)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 1000000)
        self.assertEqual(attr["min_rx"], 1000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 1)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 1000000)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 3)
        self.assertEqual(attr['negotiated_tx'], 1000000)
        self.assertEqual(attr['negotiated_rx'], 3000000)

    def sessionUpLocalExpireTest(self):
        '''
        Verify session up-expire transition
        '''

        print('sessionUpLocalExpireTest()')

        pkt_down = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=1,
            detect_mult=3,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)

        exp_pkt_init = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=2,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_init = Mask(exp_pkt_init)
        exp_pkt_init.set_do_not_care_scapy(IP, 'id')
        exp_pkt_init.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_init.set_do_not_care_scapy(UDP, 'chksum')

        pkt_up = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=3,
            detect_mult=3,
            my_discriminator=321,
            your_discriminator=123,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)

        exp_pkt_up = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=3,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=250000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_up = Mask(exp_pkt_up)
        exp_pkt_up.set_do_not_care_scapy(IP, 'id')
        exp_pkt_up.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_up.set_do_not_care_scapy(UDP, 'chksum')

        exp_pkt_down_expire = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=1,
            version=1,
            diag=1,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_down_expire = Mask(exp_pkt_down_expire)
        exp_pkt_down_expire.set_do_not_care_scapy(IP, 'id')
        exp_pkt_down_expire.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_down_expire.set_do_not_care_scapy(UDP, 'chksum')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=250000,
            min_rx=1000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_down)
            verify_packet(self, exp_pkt_init, self.dev_port10, timeout=2)
            send_packet(self, self.dev_port10, pkt_up)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=2)
            verify_packet(self, exp_pkt_down_expire,
                          self.dev_port10, timeout=3)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 250000)
        self.assertEqual(attr["min_rx"], 1000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 1)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 1000000)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 3)
        self.assertEqual(attr['negotiated_tx'], 1000000)
        self.assertEqual(attr['negotiated_rx'], 3000000)

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=250000,
            min_rx=1000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_down)
            verify_packet(self, exp_pkt_init, self.dev_port10, timeout=2)
            send_packet(self, self.dev_port10, pkt_up)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=2)
            verify_no_packet(self, exp_pkt_down_expire,
                             self.dev_port10, timeout=3)
            verify_no_other_packets(self, timeout=3)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 250000)
        self.assertEqual(attr["min_rx"], 1000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 1)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 1000000)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 3)
        self.assertEqual(attr['negotiated_tx'], 1000000)
        self.assertEqual(attr['negotiated_rx'], 3000000)

    def sessionUpRemoteExpireTest(self):
        '''
        Verify the session's behaviour on remote up-expire transition
        '''

        print('sessionUpRemoteExpireTest()')

        pkt_down = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=1,
            detect_mult=3,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)

        exp_pkt_init = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=2,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_init = Mask(exp_pkt_init)
        exp_pkt_init.set_do_not_care_scapy(IP, 'id')
        exp_pkt_init.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_init.set_do_not_care_scapy(UDP, 'chksum')

        pkt_up = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=3,
            detect_mult=3,
            my_discriminator=321,
            your_discriminator=123,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)

        exp_pkt_up = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=3,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=250000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_up = Mask(exp_pkt_up)
        exp_pkt_up.set_do_not_care_scapy(IP, 'id')
        exp_pkt_up.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_up.set_do_not_care_scapy(UDP, 'chksum')

        pkt_down_expire = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=1,
            flags=0,
            sta=1,
            detect_mult=3,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)

        exp_pkt_down = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=1,
            version=1,
            diag=3,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_down = Mask(exp_pkt_down)
        exp_pkt_down.set_do_not_care_scapy(IP, 'id')
        exp_pkt_down.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_down.set_do_not_care_scapy(UDP, 'chksum')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=250000,
            min_rx=1000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_down)
            verify_packet(self, exp_pkt_init, self.dev_port10, timeout=2)
            send_packet(self, self.dev_port10, pkt_up)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=2)
            send_packet(self, self.dev_port10, pkt_down_expire)
            verify_packet(self, exp_pkt_down, self.dev_port10, timeout=3)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 250000)
        self.assertEqual(attr["min_rx"], 1000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 3)
        self.assertEqual(attr["remote_diag"], 1)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 1000000)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 3)
        self.assertEqual(attr['negotiated_tx'], 1000000)
        self.assertEqual(attr['negotiated_rx'], 3000000)

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=250000,
            min_rx=1000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_down)
            verify_packet(self, exp_pkt_init, self.dev_port10, timeout=2)
            send_packet(self, self.dev_port10, pkt_up)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=2)
            send_packet(self, self.dev_port10, pkt_down_expire)
            verify_no_packet(self, exp_pkt_down, self.dev_port10, timeout=3)
            verify_no_other_packets(self, timeout=3)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 250000)
        self.assertEqual(attr["min_rx"], 1000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 3)
        self.assertEqual(attr["remote_diag"], 1)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 1000000)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 3)
        self.assertEqual(attr['negotiated_tx'], 1000000)
        self.assertEqual(attr['negotiated_rx'], 3000000)

    def sessionNotConfiguredTest(self):
        '''
        Verify session is not esablished if not configured
        '''

        print("sessionNotConfiguredTest()")

        pkt_down = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src='192.168.69.1',
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=1,
            detect_mult=3,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)

        pkt_init = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src='192.168.69.1',
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=2,
            detect_mult=3,
            my_discriminator=321,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)

        exp_pkt_down = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=1,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=0,
            min_tx_interval=1000000,
            min_rx_interval=1000000,
            echo_rx_interval=0)
        exp_pkt_down = Mask(exp_pkt_down)
        exp_pkt_down.set_do_not_care_scapy(IP, 'id')
        exp_pkt_down.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_down.set_do_not_care_scapy(UDP, 'chksum')

        send_packet(self, self.dev_port10, pkt_down)
        send_packet(self, self.dev_port10, pkt_init)
        verify_no_other_packets(self, timeout=2)

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=1000000,
            min_rx=1000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_down)
            verify_packet(self, exp_pkt_down, self.dev_port10, timeout=2)
            send_packet(self, self.dev_port10, pkt_init)
            verify_packet(self, exp_pkt_down, self.dev_port10, timeout=2)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 1000000)
        self.assertEqual(attr["min_rx"], 1000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 0)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 0)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 0)
        self.assertEqual(attr['negotiated_tx'], 0)
        self.assertEqual(attr['negotiated_rx'], 0)

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=1000000,
            min_rx=1000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_down)
            send_packet(self, self.dev_port10, pkt_init)
            verify_no_other_packets(self, timeout=2)
            attr = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr["type"], SAI_BFD_SESSION_TYPE_ASYNC_PASSIVE)
        self.assertEqual(attr["hw_lookup_valid"], True)
        self.assertEqual(attr["virtual_router"], self.default_vrf)
        self.assertEqual(attr["local_discriminator"], 123)
        self.assertEqual(attr["udp_src_port"], 51996)
        self.assertEqual(attr["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr["iphdr_version"], 4)
        self.assertEqual(attr["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr["min_tx"], 1000000)
        self.assertEqual(attr["min_rx"], 1000000)
        self.assertEqual(attr["multiplier"], 3)
        self.assertEqual(attr["state"], SAI_BFD_SESSION_STATE_DOWN)
        self.assertEqual(attr["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr["local_diag"], 0)
        self.assertEqual(attr["remote_diag"], 0)
        self.assertEqual(attr["remote_discriminator"], 0)
        self.assertEqual(attr["remote_min_tx"], 0)
        self.assertEqual(attr["remote_min_rx"], 1)
        self.assertEqual(attr["remote_multiplier"], 0)
        self.assertEqual(attr['negotiated_tx'], 0)
        self.assertEqual(attr['negotiated_rx'], 0)

    def sessionUpRemoteParamUpdateTest(self):
        '''
        Verify session handles poll sequence with update of remote params in up
        state
        '''

        print("sessionUpRemoteParamUpdateTest()")

        pkt_init = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=0,
            sta=2,
            detect_mult=5,
            my_discriminator=321,
            your_discriminator=123,
            min_tx_interval=1000000,
            min_rx_interval=250000,
            echo_rx_interval=0)

        exp_pkt_up = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=3,
            version=1,
            diag=0,
            flags=0,
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=500000,
            min_rx_interval=2000000,
            echo_rx_interval=0)
        exp_pkt_up = Mask(exp_pkt_up)
        exp_pkt_up.set_do_not_care_scapy(IP, 'id')
        exp_pkt_up.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_up.set_do_not_care_scapy(UDP, 'chksum')

        pkt_up_poll = bfd_ipv4_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.remote_mac,
            ip_src=self.remote_ip,
            ip_dst=self.local_ip,
            udp_sport=61996,
            udp_dport=3784,
            version=1,
            diag=0,
            flags=(1 << 5),
            sta=3,
            detect_mult=2,
            my_discriminator=321,
            your_discriminator=123,
            min_tx_interval=2000000,
            min_rx_interval=1500000,
            echo_rx_interval=0)

        exp_pkt_up_final = bfd_ipv4_packet(
            eth_dst=self.remote_mac,
            eth_src=ROUTER_MAC,
            ip_src=self.local_ip,
            ip_dst=self.remote_ip,
            udp_sport=51996,
            udp_dport=3784,
            sta=3,
            version=1,
            diag=0,
            flags=(1 << 4),
            detect_mult=3,
            my_discriminator=123,
            your_discriminator=321,
            min_tx_interval=500000,
            min_rx_interval=2000000,
            echo_rx_interval=0)
        exp_pkt_up_final = Mask(exp_pkt_up_final)
        exp_pkt_up_final.set_do_not_care_scapy(IP, 'id')
        exp_pkt_up_final.set_do_not_care_scapy(IP, 'chksum')
        exp_pkt_up_final.set_do_not_care_scapy(UDP, 'chksum')

        session = sai_thrift_create_bfd_session(
            self.client,
            type=SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE,
            bfd_encapsulation_type=SAI_BFD_ENCAPSULATION_TYPE_NONE,
            offload_type=SAI_BFD_SESSION_OFFLOAD_TYPE_FULL,
            hw_lookup_valid=True,
            virtual_router=self.default_vrf,
            iphdr_version=4,
            src_ip_address=sai_ipaddress(self.local_ip),
            dst_ip_address=sai_ipaddress(self.remote_ip),
            udp_src_port=51996,
            min_tx=500000,
            min_rx=2000000,
            multiplier=3,
            local_discriminator=123)
        self.assertTrue(session != 0)

        try:
            send_packet(self, self.dev_port10, pkt_init)
            verify_packet(self, exp_pkt_up, self.dev_port10, timeout=10)
            attr_up = self.sessionAttrGet(session)
            send_packet(self, self.dev_port10, pkt_up_poll)
            verify_packet(self, exp_pkt_up_final, self.dev_port10, timeout=3)
            attr_updated = self.sessionAttrGet(session)
        finally:
            sai_thrift_remove_bfd_session(self.client, session)

        self.assertEqual(attr_up["type"], SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr_up["hw_lookup_valid"], True)
        self.assertEqual(attr_up["virtual_router"], self.default_vrf)
        self.assertEqual(attr_up["local_discriminator"], 123)
        self.assertEqual(attr_up["remote_discriminator"], 321)
        self.assertEqual(attr_up["udp_src_port"], 51996)
        self.assertEqual(attr_up["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr_up["iphdr_version"], 4)
        self.assertEqual(attr_up["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(attr_up["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr_up["min_tx"], 500000)
        self.assertEqual(attr_up["min_rx"], 2000000)
        self.assertEqual(attr_up["multiplier"], 3)
        self.assertEqual(attr_up["state"], SAI_BFD_SESSION_STATE_UP)
        self.assertEqual(attr_up["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr_up["local_diag"], 0)
        self.assertEqual(attr_up["remote_diag"], 0)
        self.assertEqual(attr_up["remote_min_tx"], 1000000)
        self.assertEqual(attr_up["remote_min_rx"], 250000)
        self.assertEqual(attr_up["remote_multiplier"], 5)
        self.assertEqual(attr_up['negotiated_tx'], 500000)
        self.assertEqual(attr_up['negotiated_rx'], 10000000)

        self.assertEqual(attr_updated["type"],
                         SAI_BFD_SESSION_TYPE_ASYNC_ACTIVE)
        self.assertEqual(attr_updated["hw_lookup_valid"], True)
        self.assertEqual(attr_updated["virtual_router"], self.default_vrf)
        self.assertEqual(attr_updated["local_discriminator"], 123)
        self.assertEqual(attr_updated["remote_discriminator"], 321)
        self.assertEqual(attr_updated["udp_src_port"], 51996)
        self.assertEqual(attr_updated["bfd_encapsulation_type"],
                         SAI_BFD_ENCAPSULATION_TYPE_NONE)
        self.assertEqual(attr_updated["iphdr_version"], 4)
        self.assertEqual(
            attr_updated["src_ip_address"].addr.ip4, self.local_ip)
        self.assertEqual(
            attr_updated["dst_ip_address"].addr.ip4, self.remote_ip)
        self.assertEqual(attr_updated["min_tx"], 500000)
        self.assertEqual(attr_updated["min_rx"], 2000000)
        self.assertEqual(attr_updated["multiplier"], 3)
        self.assertEqual(attr_updated["state"], SAI_BFD_SESSION_STATE_UP)
        self.assertEqual(attr_updated["offload_type"],
                         SAI_BFD_SESSION_OFFLOAD_TYPE_FULL)
        self.assertEqual(attr_updated["local_diag"], 0)
        self.assertEqual(attr_updated["remote_diag"], 0)
        self.assertEqual(attr_updated["remote_min_tx"], 2000000)
        self.assertEqual(attr_updated["remote_min_rx"], 1500000)
        self.assertEqual(attr_updated["remote_multiplier"], 2)
        self.assertEqual(attr_updated['negotiated_tx'], 1500000)
        self.assertEqual(attr_updated['negotiated_rx'], 4000000)
