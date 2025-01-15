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
Thrift SAI interface SAMPLEPACKET tests
"""

import time

from bf_switcht_api_thrift.model_headers import *

from sai_base_test import *


def sock_read_pkts(sock, pkt_num=1, timeout=1, mask=None):
    '''
    reads packets from socket

    Args:
        sock (socket) : socket to read from
        pkt_num (int) : number of packets to read
        timeout (float) : timeout for reading a packet
        mask (Mask) : mask to match packets

    Return:
        list: list of read packets
    '''
    max_pkt_size = 9100
    pkts = []
    while len(pkts) < pkt_num:
        t = time.time() + timeout
        while time.time() < t:
            try:
                pkt = sock.recv(max_pkt_size)
                break
            except OSError:
                pkt = None
        if not pkt:
            break

        if isinstance(mask, Mask) and not mask.pkt_match(pkt):
            continue

        pkts.append(Ether(pkt))

    return pkts


def adj_diff(lst: list):
    '''
    Compute differences of adjacent elements if list

    Args:
        lst (list) : list of subtractable elements

    Return:
        list: list of adjacent elements subtraction result
    '''
    return [lst[i + 1] - lst[i] for i in range(len(lst) - 1)]


class SamplePacketTestBase(SaiHelper):
    '''
    SAI SAMPLEPACKET test base
    '''

    def setUp(self):
        super(SamplePacketTestBase, self).setUp()

        self.port24_bp = None
        self.port25_bp = None
        self.port26_bp = None
        self.vlan100 = None
        self.vlan100_member24 = None
        self.vlan100_member25 = None
        self.vlan100_member26 = None
        self.fdb_entry_p24 = None
        self.fdb_entry_p25 = None
        self.fdb_entry_p26 = None
        self.pkt_p24dst = None
        self.pkt_p25dst = None
        self.pkt_p26dst = None
        self.p24_hostif = None
        self.p25_hostif = None
        self.p26_hostif = None
        self.p24_hostif_socket = None
        self.p25_hostif_socket = None
        self.p26_hostif_socket = None

        self.port24_bp = sai_thrift_create_bridge_port(
            self.client, bridge_id=self.default_1q_bridge,
            port_id=self.port24, type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port24_bp != 0)
        self.port25_bp = sai_thrift_create_bridge_port(
            self.client, bridge_id=self.default_1q_bridge,
            port_id=self.port25, type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port25_bp != 0)
        self.port26_bp = sai_thrift_create_bridge_port(
            self.client, bridge_id=self.default_1q_bridge,
            port_id=self.port26, type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        self.assertTrue(self.port26_bp != 0)

        self.vlan100 = sai_thrift_create_vlan(self.client, vlan_id=100)
        self.assertTrue(self.vlan100 != 0)
        self.vlan100_member24 = sai_thrift_create_vlan_member(
            self.client, vlan_id=self.vlan100, bridge_port_id=self.port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.assertTrue(self.vlan100_member24 != 0)
        self.vlan100_member25 = sai_thrift_create_vlan_member(
            self.client, vlan_id=self.vlan100, bridge_port_id=self.port25_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.assertTrue(self.vlan100_member25 != 0)
        self.vlan100_member26 = sai_thrift_create_vlan_member(
            self.client, vlan_id=self.vlan100, bridge_port_id=self.port26_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.assertTrue(self.vlan100_member26 != 0)

        sai_thrift_set_port_attribute(self.client, self.port24,
                                      port_vlan_id=100)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      port_vlan_id=100)
        sai_thrift_set_port_attribute(self.client, self.port26,
                                      port_vlan_id=100)

        self.fdb_entry_p24 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address="00:89:24:44:89:24",
            bv_id=self.vlan100)
        status = sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry_p24,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port24_bp,
            packet_action=SAI_PACKET_ACTION_FORWARD)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.fdb_entry_p25 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address="00:89:24:44:89:25",
            bv_id=self.vlan100)
        status = sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry_p25,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port25_bp,
            packet_action=SAI_PACKET_ACTION_FORWARD)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.fdb_entry_p26 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address="00:89:24:44:89:26",
            bv_id=self.vlan100)
        status = sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry_p26,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.port26_bp,
            packet_action=SAI_PACKET_ACTION_FORWARD)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        self.pkt_p24dst = simple_udp_packet(
            eth_src='00:77:22:33:88:11',
            eth_dst='00:89:24:44:89:24',
            ip_src='192.168.67.3',
            ip_dst='192.168.69.24')
        self.pkt_p24dst[UDP].payload = Raw()
        self.pkt_p25dst = simple_udp_packet(
            eth_src='00:77:22:33:88:11',
            eth_dst='00:89:24:44:89:25',
            ip_src='192.168.67.3',
            ip_dst='192.168.69.25')
        self.pkt_p25dst[UDP].payload = Raw()
        self.pkt_p26dst = simple_udp_packet(
            eth_src='00:77:22:33:88:11',
            eth_dst='00:89:24:44:89:26',
            ip_src='192.168.67.3',
            ip_dst='192.168.69.26')
        self.pkt_p26dst[UDP].payload = Raw()

        self.p24_hostif = sai_thrift_create_hostif(self.client,
                                                   name="test_port24",
                                                   obj_id=self.port24,
                                                   type=SAI_HOSTIF_TYPE_NETDEV)
        self.assertTrue(self.p24_hostif != 0)
        self.p25_hostif = sai_thrift_create_hostif(self.client,
                                                   name="test_port25",
                                                   obj_id=self.port25,
                                                   type=SAI_HOSTIF_TYPE_NETDEV)
        self.assertTrue(self.p25_hostif != 0)
        self.p26_hostif = sai_thrift_create_hostif(self.client,
                                                   name="test_port26",
                                                   obj_id=self.port26,
                                                   type=SAI_HOSTIF_TYPE_NETDEV)
        self.assertTrue(self.p26_hostif != 0)

        self.p24_hostif_socket = open_packet_socket("test_port24")
        self.p25_hostif_socket = open_packet_socket("test_port25")
        self.p26_hostif_socket = open_packet_socket("test_port26")

    def tearDown(self):
        sai_thrift_remove_hostif(self.client, self.p26_hostif)
        sai_thrift_remove_hostif(self.client, self.p25_hostif)
        sai_thrift_remove_hostif(self.client, self.p24_hostif)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry_p26)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry_p25)
        sai_thrift_remove_fdb_entry(self.client, self.fdb_entry_p24)
        sai_thrift_set_port_attribute(self.client, self.port26, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port25, port_vlan_id=0)
        sai_thrift_set_port_attribute(self.client, self.port24, port_vlan_id=0)
        sai_thrift_remove_vlan_member(self.client, self.vlan100_member26)
        sai_thrift_remove_vlan_member(self.client, self.vlan100_member25)
        sai_thrift_remove_vlan_member(self.client, self.vlan100_member24)
        sai_thrift_remove_vlan(self.client, self.vlan100)
        sai_thrift_remove_bridge_port(self.client, self.port26_bp)
        sai_thrift_remove_bridge_port(self.client, self.port25_bp)
        sai_thrift_remove_bridge_port(self.client, self.port24_bp)

        super(SamplePacketTestBase, self).tearDown()


class IngressSamplePacketSlowPathTest(SamplePacketTestBase):
    '''
    SAI SAMPLEPACKET ingress slow path test
    '''

    def setUp(self):
        super(IngressSamplePacketSlowPathTest, self).setUp()

        self.samplepkt_trap = None
        self.samplepkt_slow_exclusive = None
        self.samplepkt_slow_shared = None

        self.samplepkt_trap = sai_thrift_create_hostif_trap(
            self.client,
            trap_type=SAI_HOSTIF_TRAP_TYPE_SAMPLEPACKET,
            packet_action=SAI_PACKET_ACTION_COPY
        )
        self.assertTrue(self.samplepkt_trap != 0)
        self.samplepkt_slow_exclusive = sai_thrift_create_samplepacket(
            self.client,
            sample_rate=10,
            type=SAI_SAMPLEPACKET_TYPE_SLOW_PATH,
            mode=SAI_SAMPLEPACKET_MODE_EXCLUSIVE)
        self.assertTrue(self.samplepkt_slow_exclusive != 0)
        self.samplepkt_slow_shared = sai_thrift_create_samplepacket(
            self.client,
            sample_rate=10,
            type=SAI_SAMPLEPACKET_TYPE_SLOW_PATH,
            mode=SAI_SAMPLEPACKET_MODE_SHARED)
        self.assertTrue(self.samplepkt_slow_shared != 0)

    def tearDown(self):
        sai_thrift_set_port_attribute(self.client, self.port26,
                                      ingress_samplepacket_enable=0)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      ingress_samplepacket_enable=0)
        sai_thrift_set_port_attribute(self.client, self.port24,
                                      ingress_samplepacket_enable=0)
        sai_thrift_remove_samplepacket(self.client,
                                       self.samplepkt_slow_exclusive)
        sai_thrift_remove_samplepacket(self.client, self.samplepkt_slow_shared)
        sai_thrift_remove_hostif_trap(self.client, self.samplepkt_trap)
        super(IngressSamplePacketSlowPathTest, self).tearDown()

    def testIngressSampling(self):
        '''
        This verifies ingress slow path sampling
        '''
        print("\ntestIngressSampling()")
        pkt_num = 100

        mask_p26dst = self.pkt_p26dst.copy()
        mask_p26dst[Ether].remove_payload()
        mask_p26dst[Ether].type = 0x0800
        mask_p26dst = Mask(mask_p26dst, ignore_extra_bytes=True)

        def getidx(pkt):
            '''
            extracts index from a packet

            Args:
                pkt (packet) : packet from where the index should be taken

            Return:
                int: index
            '''
            return int(bytes(pkt[UDP].payload).strip(b'\x00'))

        print("1: verify shared sample rate 10 on port 24 and 25 after")
        print("   sending 100 packets to each port")
        status = sai_thrift_set_port_attribute(
            self.client,
            self.port24,
            ingress_samplepacket_enable=self.samplepkt_slow_shared
        )
        self.assertEqual(status, SAI_STATUS_SUCCESS)
        status = sai_thrift_set_port_attribute(
            self.client, self.port25,
            ingress_samplepacket_enable=self.samplepkt_slow_shared)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        for idx in range(pkt_num):
            p = self.pkt_p26dst / str(idx * 2)
            send_packet(self, self.dev_port24, p)
            verify_packet(self, p, self.dev_port26)
            p = self.pkt_p26dst / str(idx * 2 + 1)
            send_packet(self, self.dev_port25, p)
            verify_packet(self, p, self.dev_port26)

        start_idx, end_idx = [0], [pkt_num * 2 - 1]
        p24pkts = sock_read_pkts(self.p24_hostif_socket, pkt_num,
                                 mask=mask_p26dst, timeout=5)
        p24idxs = [getidx(p) for p in p24pkts]
        p25pkts = sock_read_pkts(self.p25_hostif_socket, pkt_num,
                                 mask=mask_p26dst, timeout=5)
        p25idxs = [getidx(p) for p in p25pkts]

        # get gaps in sampling larger then expected rate
        shrd_idxs = sorted(p24idxs + p25idxs)
        shrd_diff = adj_diff(start_idx + shrd_idxs + end_idx)
        shrd_gaps = [x for x in shrd_diff if x - 1 > 10]
        self.assertTrue(
            len(shrd_gaps) == 0,
            f"the shared sample gaps from ports 25 and 24 are larger " +
            f"than expected at rate 10: {shrd_gaps};\n" +
            f"received packet indexes: {shrd_idxs}")

        print("2: verify exclusive sample rate 10 on port 24 and 25 after")
        print("   sending 100 packets to each port")
        status = sai_thrift_set_port_attribute(
            self.client, self.port25,
            ingress_samplepacket_enable=self.samplepkt_slow_exclusive)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        for idx in range(pkt_num):
            p = self.pkt_p26dst / str(idx)
            send_packet(self, self.dev_port24, p)
            verify_packet(self, p, self.dev_port26)
            send_packet(self, self.dev_port25, p)
            verify_packet(self, p, self.dev_port26)

        start_idx, end_idx = [0], [pkt_num - 1]
        p24pkts = sock_read_pkts(self.p24_hostif_socket, pkt_num,
                                 mask=mask_p26dst, timeout=5)
        p24idxs = [getidx(p) for p in p24pkts]
        p24diff = adj_diff(start_idx + p24idxs + end_idx)
        p25pkts = sock_read_pkts(self.p25_hostif_socket, pkt_num,
                                 mask=mask_p26dst, timeout=5)
        p25idxs = [getidx(p) for p in p25pkts]
        p25diff = adj_diff(start_idx + p25idxs + end_idx)

        # get gaps in sampling larger then expected rate
        p24gaps = [x for x in p24diff if x - 1 > 10]
        p25gaps = [x for x in p25diff if x - 1 > 10]

        self.assertTrue(len(p24gaps) == 0,
                        f"the sample gaps from port 24 are larger " +
                        f"than expected at rate 10: {p24gaps}\n" +
                        f"received packet indexes: {p24idxs}")
        self.assertTrue(len(p25gaps) == 0,
                        f"the sample gaps from port 25 are larger " +
                        f"than expected at rate 10: {p25gaps}\n" +
                        f"received packet indexes: {p25idxs}")

        print("3: verify sample rate 5 on port 24 and sample rate 10 on 25")
        print("   after sending 100 packets to each port")
        status = sai_thrift_set_samplepacket_attribute(
            self.client, self.samplepkt_slow_shared, sample_rate=5)
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        for idx in range(pkt_num):
            p = self.pkt_p26dst / str(idx)
            send_packet(self, self.dev_port24, p)
            verify_packet(self, p, self.dev_port26)
            send_packet(self, self.dev_port25, p)
            verify_packet(self, p, self.dev_port26)

        start_idx, end_idx = [0], [pkt_num - 1]
        p24pkts = sock_read_pkts(self.p24_hostif_socket, pkt_num,
                                 mask=mask_p26dst, timeout=5)
        p24idxs = [getidx(p) for p in p24pkts]
        p24diff = adj_diff(start_idx + p24idxs + end_idx)
        p25pkts = sock_read_pkts(self.p25_hostif_socket, pkt_num,
                                 mask=mask_p26dst, timeout=5)
        p25idxs = [getidx(p) for p in p25pkts]
        p25diff = adj_diff(start_idx + p25idxs + end_idx)

        # get gaps in sampling larger then expected rate
        p24gaps = [x for x in p24diff if x - 1 > 5]
        p25gaps = [x for x in p25diff if x - 1 > 10]

        self.assertTrue(len(p24gaps) == 0,
                        f"the sample gaps from port 24 are larger " +
                        f"than expected at rate 5: {p24gaps}\n" +
                        f"received packet indexes: {p24idxs}")
        self.assertTrue(len(p25gaps) == 0,
                        f"the sample gaps from port 25 are larger " +
                        f"than expected at rate 10: {p25gaps}\n" +
                        f"received packet indexes: {p25idxs}")

    def runTest(self):
        try:
            self.testIngressSampling()
        except Exception as e:
            raise e
        finally:
            pass
