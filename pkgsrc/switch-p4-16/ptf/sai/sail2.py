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
Thrift SAI interface L2 tests
"""
import socket
import sai_base_test
from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *
from switchsai_thrift.sai_headers import  *
from switch_utils import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from common.dtel_utils import *
from common.pktpy_utils import pktpy_skip, pktpy_skip_test  # noqa pylint: disable=wrong-import-position


class VlanData():
    vlan_id = 0
    ports = []
    untagged = 0
    large_port = 0
    def __init__(self, vlan_id=0, ports=[], untagged=0, large_port=0):
        self.vlan_id = vlan_id
        self.ports = ports
        self.untagged = untagged
        self.large_port = large_port

'''
+-----------+-----------------+----------------+
|   VLAN ID | Ports           | Port Tagging   |
+===========+=================+================+
|       100 | Port1           | untagged       |
|           | Port2           | tagged         |
|           | Lag1            | tagged         |
|           | Lag2            | tagged         |-<
+-----------+-----------------+----------------+
|       200 | Port1           | tagged         |
|           | Port2           | untagged       |-<
|           | Lag1            | tagged         |
|           | Lag2            | tagged         |
+-----------+-----------------+----------------+
|       300 | Port1           | tagged         |
|           | Port2           | tagged         |
|           | Lag1            | untagged       |-<
|           | Lag2            | tagged         |
+-----------+-----------------+----------------+
|       400 | Port1           | tagged         |-<
|           | Port2           | tagged         |
|           | Lag1            | tagged         |
|           | Lag2            | untagged       |
+-----------+-----------------+----------------+
'''
@group('l2')
class L2VlanMembershipTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print 'Flood test on ports 1, 2, lag 1 and 2'
        switch_init(self.client)
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        port4 = port_list[4]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'

        self.vlan100 = sai_thrift_create_vlan(self.client, 100)
        self.vlan200 = sai_thrift_create_vlan(self.client, 200)
        self.vlan300 = sai_thrift_create_vlan(self.client, 300)
        self.vlan400 = sai_thrift_create_vlan(self.client, 400)

        lag_id1 = sai_thrift_create_lag(self.client, [])
        lag_member_id11 = sai_thrift_create_lag_member(self.client, lag_id1, port3)
        lag_id2 = sai_thrift_create_lag(self.client, [])
        lag_member_id21 = sai_thrift_create_lag_member(self.client, lag_id2, port4)

        # vlan100
        vlan_member101 = sai_thrift_create_vlan_member(self.client, self.vlan100, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member102 = sai_thrift_create_vlan_member(self.client, self.vlan100, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member103 = sai_thrift_create_vlan_member(self.client, self.vlan100, lag_id1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member104 = sai_thrift_create_vlan_member(self.client, self.vlan100, lag_id2, SAI_VLAN_TAGGING_MODE_TAGGED)

        # vlan200
        vlan_member201 = sai_thrift_create_vlan_member(self.client, self.vlan200, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member202 = sai_thrift_create_vlan_member(self.client, self.vlan200, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member203 = sai_thrift_create_vlan_member(self.client, self.vlan200, lag_id1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member204 = sai_thrift_create_vlan_member(self.client, self.vlan200, lag_id2, SAI_VLAN_TAGGING_MODE_TAGGED)

        # vlan300
        vlan_member301 = sai_thrift_create_vlan_member(self.client, self.vlan300, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member302 = sai_thrift_create_vlan_member(self.client, self.vlan300, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member303 = sai_thrift_create_vlan_member(self.client, self.vlan300, lag_id1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member304 = sai_thrift_create_vlan_member(self.client, self.vlan300, lag_id2, SAI_VLAN_TAGGING_MODE_TAGGED)

        # vlan400
        vlan_member401 = sai_thrift_create_vlan_member(self.client, self.vlan400, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member402 = sai_thrift_create_vlan_member(self.client, self.vlan400, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member403 = sai_thrift_create_vlan_member(self.client, self.vlan400, lag_id1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member404 = sai_thrift_create_vlan_member(self.client, self.vlan400, lag_id2, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=100)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        attr_value = sai_thrift_attribute_value_t(u16=200)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        attr_value = sai_thrift_attribute_value_t(u16=300)
        attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_lag_attribute(lag_id1, attr)
        attr_value = sai_thrift_attribute_value_t(u16=400)
        attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_lag_attribute(lag_id2, attr)

        self.pkt = simple_arp_packet(
            eth_src='00:22:22:33:44:55',
            arp_op=1, #ARP request
            hw_snd='00:22:22:33:44:55',
            pktlen=100)
        self.tagged_pkt = simple_arp_packet(
            eth_src='00:22:22:33:44:55',
            arp_op=1, #ARP request
            hw_snd='00:22:22:33:44:55',
            vlan_vid=100,
            pktlen=104)
        self.arp_resp = simple_arp_packet(
            eth_dst='00:22:22:33:44:55',
            arp_op=2, #ARP request
            hw_tgt='00:22:22:33:44:55',
            pktlen=100)
        self.tagged_arp_resp = simple_arp_packet(
            eth_dst='00:22:22:33:44:55',
            arp_op=2, #ARP request
            hw_tgt='00:22:22:33:44:55',
            vlan_vid=100,
            pktlen=104)

        try:
            vlan_ports = switch_ports[1:5]
            vlan_data = {self.vlan100: VlanData(100, vlan_ports, switch_ports[1], switch_ports[4]),
                         self.vlan200: VlanData(200, vlan_ports, switch_ports[2], switch_ports[4]),
                         self.vlan300: VlanData(300, vlan_ports, switch_ports[3], switch_ports[4]),
                         self.vlan400: VlanData(400, vlan_ports, switch_ports[4], switch_ports[4])}
            self.BasicTest(vlan_data)
            self.client.sai_thrift_remove_vlan_member(vlan_member104)
            self.client.sai_thrift_remove_vlan_member(vlan_member202)
            self.client.sai_thrift_remove_vlan_member(vlan_member303)
            self.client.sai_thrift_remove_vlan_member(vlan_member401)
            vlan_data = {self.vlan100: VlanData(100, [switch_ports[1],switch_ports[2],switch_ports[3]], switch_ports[1], switch_ports[3]),
                         self.vlan200: VlanData(200, [switch_ports[1],switch_ports[3],switch_ports[4]], switch_ports[0], switch_ports[4]),
                         self.vlan300: VlanData(300, [switch_ports[1],switch_ports[2],switch_ports[4]], switch_ports[0], switch_ports[4]),
                         self.vlan400: VlanData(400, [switch_ports[2],switch_ports[3],switch_ports[4]], switch_ports[4], switch_ports[4])}
            self.BasicTest(vlan_data)
            vlan_member104 = sai_thrift_create_vlan_member(self.client, self.vlan100, lag_id2, SAI_VLAN_TAGGING_MODE_TAGGED)
            vlan_member202 = sai_thrift_create_vlan_member(self.client, self.vlan200, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
            vlan_member303 = sai_thrift_create_vlan_member(self.client, self.vlan300, lag_id1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
            vlan_member401 = sai_thrift_create_vlan_member(self.client, self.vlan400, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
            vlan_data = {self.vlan100: VlanData(100, vlan_ports, switch_ports[1], switch_ports[4]),
                         self.vlan200: VlanData(200, vlan_ports, switch_ports[2], switch_ports[4]),
                         self.vlan300: VlanData(300, vlan_ports, switch_ports[3], switch_ports[4]),
                         self.vlan400: VlanData(400, vlan_ports, switch_ports[4], switch_ports[4])}
            self.BasicTest(vlan_data)
        finally:
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan100)
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan200)
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan300)
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan400)
            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)
            self.client.sai_thrift_set_port_attribute(port4, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member101)
            self.client.sai_thrift_remove_vlan_member(vlan_member102)
            self.client.sai_thrift_remove_vlan_member(vlan_member103)
            self.client.sai_thrift_remove_vlan_member(vlan_member104)
            self.client.sai_thrift_remove_vlan_member(vlan_member201)
            self.client.sai_thrift_remove_vlan_member(vlan_member202)
            self.client.sai_thrift_remove_vlan_member(vlan_member203)
            self.client.sai_thrift_remove_vlan_member(vlan_member204)
            self.client.sai_thrift_remove_vlan_member(vlan_member301)
            self.client.sai_thrift_remove_vlan_member(vlan_member302)
            self.client.sai_thrift_remove_vlan_member(vlan_member303)
            self.client.sai_thrift_remove_vlan_member(vlan_member304)
            self.client.sai_thrift_remove_vlan_member(vlan_member401)
            self.client.sai_thrift_remove_vlan_member(vlan_member402)
            self.client.sai_thrift_remove_vlan_member(vlan_member403)
            self.client.sai_thrift_remove_vlan_member(vlan_member404)

            sai_thrift_remove_lag_member(self.client, lag_member_id21)
            sai_thrift_remove_lag_member(self.client, lag_member_id11)
            sai_thrift_remove_lag(self.client, lag_id2)
            sai_thrift_remove_lag(self.client, lag_id1)

            self.client.sai_thrift_remove_vlan(self.vlan100)
            self.client.sai_thrift_remove_vlan(self.vlan200)
            self.client.sai_thrift_remove_vlan(self.vlan300)
            self.client.sai_thrift_remove_vlan(self.vlan400)

    def BasicTest(self, vlan_data):
        try:
            for vlan_key in vlan_data.keys():
                vlan = vlan_data[vlan_key]
                print "#### Flooding on %s ####" % (str(vlan.vlan_id))
                self.tagged_pkt[Dot1Q].vlan = vlan.vlan_id
                self.tagged_arp_resp[Dot1Q].vlan = vlan.vlan_id
                pkt_list = [None]*vlan.large_port
                arp_pkt_list = [None]*vlan.large_port
                for port in vlan.ports:
                    if port != vlan.untagged:
                       pkt_list[port-1] = self.tagged_pkt
                       arp_pkt_list[port-1] = self.tagged_arp_resp
                    else:
                        pkt_list[port-1] = self.pkt
                        arp_pkt_list[port-1] = self.arp_resp
                for port in vlan.ports:
                    print "Testing flooding and learning on port%d" % (port)
                    other_ports = [p for p in vlan.ports if p != port]
                    verify_pkt_list = [pkt_list[pl-1] for pl in other_ports]
                    print "Sending arp request from port ", port, " flooding on ", other_ports
                    send_packet(self, port, str(pkt_list[port-1]))
                    verify_each_packet_on_each_port(self, verify_pkt_list, other_ports)
                    time.sleep(2)
                    for send_port in other_ports:
                        print "Sending arp response from ", send_port, "->", port
                        send_packet(self, send_port, str(arp_pkt_list[send_port-1]))
                        verify_packets(self, arp_pkt_list[port-1], [port])
                    sai_thrift_flush_fdb_by_vlan(self.client, vlan_key)
        finally:
            for vlan_key in vlan_data.keys():
                sai_thrift_flush_fdb_by_vlan(self.client, vlan_key)

'''
+-----------+-----------------+----------------+
|   VLAN ID | Ports           | Port Tagging   |
+===========+=================+================+
|       100 | Port1           | tagged         |
|           | Port2           | tagged         |
|           | Lag1            | tagged         |
|           | Lag2            | tagged         |
|           | Port7           | tagged         |
+-----------+-----------------+----------------+
|       200 | Port1           | untagged       |
|           | Port2           | untagged       |
|           | Lag1            | untagged       |
|           | Lag2            | untagged       |
+-----------+-----------------+----------------+
Port 7 moves to 200 midway thru the test
'''
@group('l2')
class L2FloodEnhancedTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print 'Flood test on ports 1, 2, lag 1 and 2'
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        port5 = port_list[4]
        port6 = port_list[5]
        port7 = port_list[6]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'

        vlan100 = sai_thrift_create_vlan(self.client, 100)
        vlan200 = sai_thrift_create_vlan(self.client, 200)
        attr_value = sai_thrift_attribute_value_t(booldata=True)
        attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_LEARN_DISABLE, value=attr_value)
        self.client.sai_thrift_set_vlan_attribute(vlan100, attr)
        self.client.sai_thrift_set_vlan_attribute(vlan200, attr)

        lag_id1 = sai_thrift_create_lag(self.client, [])
        lag_member_id11 = sai_thrift_create_lag_member(self.client, lag_id1, port3)
        lag_member_id12 = sai_thrift_create_lag_member(self.client, lag_id1, port4)
        lag_id2 = sai_thrift_create_lag(self.client, [])
        lag_member_id21 = sai_thrift_create_lag_member(self.client, lag_id2, port5)
        lag_member_id22 = sai_thrift_create_lag_member(self.client, lag_id2, port6)

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan100, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan100, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan100, lag_id1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member4 = sai_thrift_create_vlan_member(self.client, vlan100, lag_id2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member5 = sai_thrift_create_vlan_member(self.client, vlan200, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member6 = sai_thrift_create_vlan_member(self.client, vlan200, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member7 = sai_thrift_create_vlan_member(self.client, vlan200, lag_id1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member8 = sai_thrift_create_vlan_member(self.client, vlan200, lag_id2, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        vlan_member9 = sai_thrift_create_vlan_member(self.client, vlan100, port7, SAI_VLAN_TAGGING_MODE_TAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=200)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_lag_attribute(lag_id1, attr)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_lag_attribute(lag_id2, attr)

        pkt200 = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:22:33:44:55',
            arp_op=1, #ARP request
            ip_tgt='10.10.10.1',
            ip_snd='10.10.10.2',
            hw_snd='00:22:22:33:44:55',
            pktlen=100)
        pkt100 = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:22:33:44:55',
            arp_op=1, #ARP request
            ip_tgt='10.10.10.1',
            ip_snd='10.10.10.2',
            hw_snd='00:22:22:33:44:55',
            vlan_vid=100,
            pktlen=100)

        try:
            l1 = [switch_ports[2],switch_ports[3]]
            l2 = [switch_ports[4],switch_ports[5]]
            send_packet(self,switch_ports[0], str(pkt100))
            verify_any_packet_on_ports_list(self, [pkt100], [l1, l2, [switch_ports[1]], [switch_ports[6]]])
            send_packet(self, switch_ports[2], str(pkt100))
            verify_any_packet_on_ports_list(self, [pkt100], [[switch_ports[0]], l2, [switch_ports[1]], [switch_ports[6]]])
            send_packet(self, switch_ports[4], str(pkt100))
            verify_any_packet_on_ports_list(self, [pkt100], [[switch_ports[0]], l1, [switch_ports[1]], [switch_ports[6]]])
            send_packet(self, switch_ports[6], str(pkt100))
            verify_any_packet_on_ports_list(self, [pkt100], [l1, l2, [switch_ports[0]], [switch_ports[1]]])
            print "Remove port 6 from vlan 100"
            self.client.sai_thrift_remove_vlan_member(vlan_member9)
            send_packet(self,switch_ports[0], str(pkt100))
            verify_any_packet_on_ports_list(self, [pkt100], [l1, l2, [switch_ports[1]]])
            send_packet(self, switch_ports[2], str(pkt100))
            verify_any_packet_on_ports_list(self, [pkt100], [[switch_ports[0]], l2, [switch_ports[1]]])
            send_packet(self, switch_ports[4], str(pkt100))
            verify_any_packet_on_ports_list(self, [pkt100], [[switch_ports[0]], l1, [switch_ports[1]]])

            send_packet(self,switch_ports[1], str(pkt200))
            verify_any_packet_on_ports_list(self, [pkt200], [l1, l2, [switch_ports[0]]])
            send_packet(self, switch_ports[3], str(pkt200))
            verify_any_packet_on_ports_list(self, [pkt200], [[switch_ports[0]], l2, [switch_ports[1]]])
            send_packet(self, switch_ports[5], str(pkt200))
            verify_any_packet_on_ports_list(self, [pkt200], [[switch_ports[0]], l1, [switch_ports[1]]])
            print "Add port 6 to vlan 200"
            vlan_member9 = sai_thrift_create_vlan_member(self.client, vlan200, port7, SAI_VLAN_TAGGING_MODE_UNTAGGED)
            attr_value = sai_thrift_attribute_value_t(u16=200)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port7, attr)
            send_packet(self,switch_ports[1], str(pkt200))
            verify_any_packet_on_ports_list(self, [pkt200], [l1, l2, [switch_ports[0]], [switch_ports[6]]])
            send_packet(self, switch_ports[3], str(pkt200))
            verify_any_packet_on_ports_list(self, [pkt200], [[switch_ports[0]], l2, [switch_ports[1]], [switch_ports[6]]])
            send_packet(self, switch_ports[5], str(pkt200))
            verify_any_packet_on_ports_list(self, [pkt200], [[switch_ports[0]], l1, [switch_ports[1]], [switch_ports[6]]])
            send_packet(self, switch_ports[6], str(pkt200))
            verify_any_packet_on_ports_list(self, [pkt200], [l1, l2, [switch_ports[0]], [switch_ports[1]]])
        finally:
            sai_thrift_flush_fdb_by_vlan(self.client, vlan100)
            sai_thrift_flush_fdb_by_vlan(self.client, vlan200)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)
            self.client.sai_thrift_set_port_attribute(port4, attr)
            self.client.sai_thrift_set_port_attribute(port5, attr)
            self.client.sai_thrift_set_port_attribute(port6, attr)
            self.client.sai_thrift_set_port_attribute(port7, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan_member(vlan_member4)
            self.client.sai_thrift_remove_vlan_member(vlan_member5)
            self.client.sai_thrift_remove_vlan_member(vlan_member6)
            self.client.sai_thrift_remove_vlan_member(vlan_member7)
            self.client.sai_thrift_remove_vlan_member(vlan_member8)
            self.client.sai_thrift_remove_vlan_member(vlan_member9)

            sai_thrift_remove_lag_member(self.client, lag_member_id21)
            sai_thrift_remove_lag_member(self.client, lag_member_id22)
            sai_thrift_remove_lag_member(self.client, lag_member_id11)
            sai_thrift_remove_lag_member(self.client, lag_member_id12)
            sai_thrift_remove_lag(self.client, lag_id2)
            sai_thrift_remove_lag(self.client, lag_id1)

            self.client.sai_thrift_remove_vlan(vlan100)
            self.client.sai_thrift_remove_vlan(vlan200)


@group('l2')
class L2VlanCountersClearTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=101,
                                ip_ttl=64)

        try:
            in_bytes_pre = sai_thrift_read_vlan_counters(self.client, vlan_oid)[0]
            out_bytes_pre = sai_thrift_read_vlan_counters(self.client, vlan_oid)[1]
            in_packets_pre = sai_thrift_read_vlan_counters(self.client, vlan_oid)[2]
            in_ucast_packets_pre = sai_thrift_read_vlan_counters(self.client, vlan_oid)[3]
            out_packets_pre = sai_thrift_read_vlan_counters(self.client, vlan_oid)[4]
            out_ucast_packets_pre = sai_thrift_read_vlan_counters(self.client, vlan_oid)[5]

            print "Sending L2 packet port 1 -> port 2 [access vlan=10])"
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, pkt, [switch_ports[1]])

            # Check counters
            in_bytes = sai_thrift_read_vlan_counters(self.client, vlan_oid)[0]
            out_bytes = sai_thrift_read_vlan_counters(self.client, vlan_oid)[1]
            in_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[2]
            in_ucast_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[3]
            out_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[4]
            out_ucast_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[5]
            assert(in_packets == in_packets_pre + 1), 'vlan IN packets counter {} != {}'.format(in_packets, in_packets_pre + 1)
            assert(in_ucast_packets == in_ucast_packets_pre + 1), 'vlan IN unicats packets counter {} != {}'.format(in_ucast_packets, in_packets_ucast_pre + 1)
            assert((in_bytes - in_bytes_pre) != 0), 'vlan IN bytes counter is 0'
            assert(out_packets == out_packets_pre + 1), 'vlan OUT packets counter {} != {}'.format(out_packets, out_packets_pre + 1)
            assert(out_ucast_packets == out_ucast_packets_pre + 1), 'vlan OUT unicats packets counter {} != {}'.format(out_ucast_packets, out_packets_ucast_pre + 1)
            assert((out_bytes - out_bytes_pre) != 0), 'vlan OUT bytes counter is 0'

            # Clear octets counters
            cnt_ids = [SAI_VLAN_STAT_IN_OCTETS, SAI_VLAN_STAT_OUT_OCTETS]
            self.client.sai_thrift_clear_vlan_stats(vlan_oid, cnt_ids, len(cnt_ids))

            # Check counters
            in_bytes = sai_thrift_read_vlan_counters(self.client, vlan_oid)[0]
            out_bytes = sai_thrift_read_vlan_counters(self.client, vlan_oid)[1]
            in_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[2]
            in_ucast_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[3]
            out_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[4]
            out_ucast_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[5]
            assert(in_packets == in_packets_pre + 1), 'vlan IN packets counter {} != {}'.format(in_packets, in_packets_pre + 1)
            assert(in_ucast_packets == in_ucast_packets_pre + 1), 'vlan IN unicats packets counter {} != {}'.format(in_ucast_packets, in_packets_ucast_pre + 1)
            assert(in_bytes == 0), 'vlan IN bytes counter is not 0'
            assert(out_packets == out_packets_pre + 1), 'vlan OUT packets counter {} != {}'.format(out_packets, out_packets_pre + 1)
            assert(out_ucast_packets == out_ucast_packets_pre + 1), 'vlan OUT unicats packets counter {} != {}'.format(out_ucast_packets, out_packets_ucast_pre + 1)
            assert(out_bytes == 0), 'vlan OUT bytes counter is not 0'

            print "Sending L2 packet port 1 -> port 2 [access vlan=10])"
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, pkt, [switch_ports[1]])

            # Clear bytes and packets counter
            cnt_ids = [SAI_VLAN_STAT_IN_OCTETS,
                       SAI_VLAN_STAT_OUT_OCTETS,
                       SAI_VLAN_STAT_IN_PACKETS,
                       SAI_VLAN_STAT_IN_UCAST_PKTS,
                       SAI_VLAN_STAT_OUT_PACKETS,
                       SAI_VLAN_STAT_OUT_UCAST_PKTS]
            self.client.sai_thrift_clear_vlan_stats(vlan_oid, cnt_ids, len(cnt_ids))

            # Check counters
            in_bytes = sai_thrift_read_vlan_counters(self.client, vlan_oid)[0]
            out_bytes = sai_thrift_read_vlan_counters(self.client, vlan_oid)[1]
            in_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[2]
            in_ucast_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[3]
            out_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[4]
            out_ucast_packets = sai_thrift_read_vlan_counters(self.client, vlan_oid)[5]
            assert(in_packets == 0), 'vlan IN packets counter is not 0'
            assert(in_ucast_packets == 0), 'vlan IN unicast packets counter is not 0'
            assert(in_bytes == 0), 'vlan IN bytes counter is not 0'
            assert(out_packets == 0), 'vlan OUT packets counter is not 0'
            assert(out_ucast_packets == 0), 'vlan OUT unicast packets counter is not 0'
            assert(out_bytes == 0), 'vlan OUT bytes counter is not 0'

        finally:
            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
class L2AccessToAccessVlanTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending L2 packet port 1 -> port 2 [access vlan=10])"
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=101,
                                ip_ttl=64)

        try:
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, pkt, [switch_ports[1]])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
class L2TrunkToTrunkVlanTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending L2 packet - port 1 -> port 2 [trunk vlan=10])"
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                dl_vlan_enable=True,
                                vlan_vid=10,
                                ip_dst='172.16.0.1',
                                ip_id=102,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=102,
                                dl_vlan_enable=True,
                                vlan_vid=10,
                                ip_ttl=64)

        try:
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[1]])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
class L2AccessToTrunkVlanTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending L2 packet - port 1 -> port 2 [trunk vlan=10])"
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_TAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=102,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                dl_vlan_enable=True,
                                vlan_vid=10,
                                ip_id=102,
                                ip_ttl=64,
                                pktlen=104)

        try:
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[1]])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
class L2TrunkToAccessVlanTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending L2 packet - port 1 -> port 2 [trunk vlan=10])"
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port2, mac_action)

        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                dl_vlan_enable=True,
                                vlan_vid=10,
                                ip_dst='172.16.0.1',
                                ip_id=102,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=102,
                                ip_ttl=64,
                                pktlen=96)
        try:
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[1]])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)

            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
class L2FloodTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print 'Flood test on ports 1, 2 and 3'
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=107,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=107,
                                ip_ttl=64)

        try:
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, exp_pkt,[switch_ports[1], switch_ports[2]])
            send_packet(self,switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0], switch_ports[2]])
            send_packet(self, switch_ports[2], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0], switch_ports[1]])
        finally:
            sai_thrift_flush_fdb_by_vlan(self.client, vlan_oid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
@group('lag')
class L2LagTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)

        lag_id1 = sai_thrift_create_lag(self.client, [])

        lag_member_id1 = sai_thrift_create_lag_member(self.client, lag_id1, port1)
        lag_member_id2 = sai_thrift_create_lag_member(self.client, lag_id1, port2)
        lag_member_id3 = sai_thrift_create_lag_member(self.client, lag_id1, port3)

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, lag_id1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port4, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_lag_attribute(lag_id1, attr)

        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port4, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, lag_id1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port4, mac_action)

        try:
            count = [0, 0, 0]
            dst_ip = int(socket.inet_aton('10.10.10.1').encode('hex'),16)
            max_itrs = 200
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(hex(dst_ip)[2:].zfill(8).decode('hex'))
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_dst=dst_ip_addr,
                                        ip_src='192.168.8.1',
                                        ip_id=109,
                                        ip_ttl=64)

                exp_pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_dst=dst_ip_addr,
                                            ip_src='192.168.8.1',
                                            ip_id=109,
                                            ip_ttl=64)

                send_packet(self, switch_ports[3], str(pkt))
                ports_verify = switch_ports[0:3]
                rcv_idx = verify_any_packet_any_port(self, [exp_pkt], ports_verify, timeout = 5)
                count[rcv_idx] += 1
                dst_ip += 1

            print count
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.8)),
                        "Not all paths are equally balanced")

            pkt = simple_tcp_packet(eth_src='00:11:11:11:11:11',
                                    eth_dst='00:22:22:22:22:22',
                                    ip_dst='10.0.0.1',
                                    ip_id=109,
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_src='00:11:11:11:11:11',
                                    eth_dst='00:22:22:22:22:22',
                                    ip_dst='10.0.0.1',
                                    ip_id=109,
                                    ip_ttl=64)
            print "Sending packet port 1 (lag member) -> port 4"
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[3]])
            print "Sending packet port 2 (lag member) -> port 4"
            send_packet(self,switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[3]])
            print "Sending packet port 3 (lag member) -> port 4"
            send_packet(self, switch_ports[2], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[3]])
        finally:

            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, lag_id1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port4)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)

            sai_thrift_remove_lag_member(self.client, lag_member_id1)
            sai_thrift_remove_lag_member(self.client, lag_member_id2)
            sai_thrift_remove_lag_member(self.client, lag_member_id3)
            sai_thrift_remove_lag(self.client, lag_id1)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            for port in sai_port_list:
                self.client.sai_thrift_set_port_attribute(port, attr)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
@group('sonic')
class L2VlanBcastUcastTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        """
        For SONiC
        Vlan broadcast and known unicast test. Verify the broacast packet reaches all ports in the vlan and unicast packet reach specific port.
        Steps:
        1. remove all ports from default vlan
        2. create vlan 10
        3. add n-1 ports to vlan 10
        4. add mac for each port
        5. send untagged broadcast packet from port 1, verify all n-1 ports receive the packet except the last port
        6. send untagged unicast packets from port 1 to the rest of the vlan members ports. Verify only one port at a time receives the packet and port n does not.
        7. clean up.
        """

        switch_init(self.client)
        vlan_id = 10
        mac_list = []
        vlan_member_list = []
        ingress_port = switch_ports[0]

        for i in range (1, len(port_list)):
            mac_list.append("00:00:00:00:00:%02x" %(i+1))
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        for i in range (0, len(port_list)-1):
            vlan_member_list.append(sai_thrift_create_vlan_member(self.client, vlan_oid, port_list[i], SAI_VLAN_TAGGING_MODE_UNTAGGED))

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        for i in range (0, len(port_list)-1):
            self.client.sai_thrift_set_port_attribute(port_list[i], attr)
            sai_thrift_create_fdb(self.client, vlan_oid, mac_list[i], port_list[i], mac_action)

        bcast_pkt = simple_tcp_packet(eth_dst='ff:ff:ff:ff:ff:ff',
                                eth_src='00:00:00:00:00:01',
                                ip_dst='172.16.0.1',
                                ip_id=101,
                                ip_ttl=64)

        try:
            expected_ports = []
            for i in range (1, len(port_list)-1):
                expected_ports.append(switch_ports[i])

            send_packet(self, ingress_port, str(bcast_pkt))
            verify_packets(self, bcast_pkt, expected_ports)

            for i in range (1, len(port_list)-1):
                ucast_pkt = simple_tcp_packet(eth_dst=mac_list[i],
                                    eth_src='00:00:00:00:00:01',
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
                send_packet(self, ingress_port, str(ucast_pkt))
                verify_packets(self, ucast_pkt, [switch_ports[i]])

        finally:
            attr_value = sai_thrift_attribute_value_t(u16=switch.default_vlan.vid)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)

            for i in range (0, len(port_list)-1):
                sai_thrift_delete_fdb(self.client, vlan_oid, mac_list[i], port_list[i])
                self.client.sai_thrift_set_port_attribute(port_list[i], attr)
            sai_thrift_flush_fdb_by_vlan(self.client, vlan_oid)

            for vlan_member in vlan_member_list:
                self.client.sai_thrift_remove_vlan_member(vlan_member)

            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
class L2BridgePortRemoveTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "PTF L2 bridge port delete test ..."
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        fdb_aging_time = 60

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        attr_value = sai_thrift_attribute_value_t(booldata=False)
        attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_LEARN_DISABLE, value=attr_value)
        self.client.sai_thrift_set_vlan_attribute(vlan_oid, attr)

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member4 = sai_thrift_create_vlan_member(self.client, vlan_oid, port4, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)
        self.client.sai_thrift_set_port_attribute(port4, attr)

        attr_value = sai_thrift_attribute_value_t(u32=fdb_aging_time)
        attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_AGING_TIME, value=attr_value)
        self.client.sai_thrift_set_switch_attribute(attr)

        pkt = simple_tcp_packet(eth_dst=mac2,
                                eth_src=mac1,
                                ip_dst='172.16.0.1',
                                ip_id=101,
                                ip_ttl=64)

        pkt1 = simple_tcp_packet(eth_dst=mac1,
                                 eth_src=mac2,
                                 ip_dst='172.16.0.1',
                                 ip_id=101,
                                 ip_ttl=64)

        try:
            print "Send packet from port1 to port2 and verify on each of ports"
            print '#### Sending 00:11:11:11:11:11| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1 ####'
            send_packet(self,switch_ports[0], str(pkt))
            verify_each_packet_on_each_port(self, [pkt, pkt, pkt],[switch_ports[1], switch_ports[2], switch_ports[3]])
            time.sleep(5)
            print "Send packet from port2 to port1 and verify only on port1"
            print '#### Sending 00:22:22:22:22:22| 00:11:11:11:11:11 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2 ####'
            send_packet(self, switch_ports[1], str(pkt1))
            verify_packets(self, pkt1, [switch_ports[0]])

            print "Remove the bridge port for port0, the learnt MAC must be flushed..."
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            bport_oid = sai_thrift_get_bridge_port_by_port(self.client, port1)
            attr_value = sai_thrift_attribute_value_t(booldata=0)
            attr = sai_thrift_attribute_t(id=SAI_BRIDGE_PORT_ATTR_ADMIN_STATE, value=attr_value)
            self.client.sai_thrift_set_bridge_port_attribute(bport_oid, attr)
            self.client.sai_thrift_remove_bridge_port(bport_oid)
            print "Send packet from port2 and verify on each of ports and not on port1"
            print '#### Sending 00:22:22:22:22:22| 00:11:11:11:11:11 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2 ####'
            send_packet(self,switch_ports[1], str(pkt1))
            verify_each_packet_on_each_port(self, [pkt1, pkt1], [switch_ports[2], switch_ports[3]])
        finally:
            time.sleep(10)
            sai_thrift_flush_fdb_by_vlan(self.client, vlan_oid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)
            self.client.sai_thrift_set_port_attribute(port4, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan_member(vlan_member4)
            self.client.sai_thrift_remove_vlan(vlan_oid)
            sai_thrift_create_bridge_port(self.client, SAI_BRIDGE_PORT_TYPE_PORT, port1)

@group('l2')
class L2FdbAgingTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "PTF L2 FDB aging test ..."
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        fdb_aging_time = 18

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        attr_value = sai_thrift_attribute_value_t(booldata=False)
        attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_LEARN_DISABLE, value=attr_value)
        self.client.sai_thrift_set_vlan_attribute(vlan_oid, attr)

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        attr_value = sai_thrift_attribute_value_t(u32=fdb_aging_time)
        attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_AGING_TIME, value=attr_value)
        self.client.sai_thrift_set_switch_attribute(attr)

        pkt = simple_tcp_packet(eth_dst=mac2,
                                eth_src=mac1,
                                ip_dst='172.16.0.1',
                                ip_id=101,
                                ip_ttl=64)

        pkt1 = simple_tcp_packet(eth_dst=mac1,
                                 eth_src=mac2,
                                 ip_dst='172.16.0.1',
                                 ip_id=101,
                                 ip_ttl=64)

        try:
            print "Send packet from port1 to port2 and verify on each of ports"
            print '#### Sending 00:11:11:11:11:11| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1 ####'
            send_packet(self,switch_ports[0], str(pkt))
            verify_each_packet_on_each_port(self, [pkt, pkt],[switch_ports[1], switch_ports[2]])
            time.sleep(5)
            print "Send packet from port2 to port1 and verify only on port1"
            print '#### Sending 00:22:22:22:22:22| 00:11:11:11:11:11 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2 ####'
            send_packet(self, switch_ports[1], str(pkt1))
            verify_packets(self, pkt1, [switch_ports[0]])
            print "Wait when the aging time for FDB entries in the FDB table expires, and the entries are removed ..."
            time.sleep(25)
            print "Send packet from port2 to port1 and verify on each of ports"
            print '#### Sending 00:22:22:22:22:22| 00:11:11:11:11:11 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2 ####'
            send_packet(self,switch_ports[1], str(pkt1))
            verify_each_packet_on_each_port(self, [pkt1, pkt1], [switch_ports[0], switch_ports[2] ])
        finally:
            time.sleep(10)
            sai_thrift_flush_fdb_by_vlan(self.client, vlan_oid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
class L2ARPRequestReplyFDBLearningTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        v4_enabled = 1
        v6_enabled = 1
        vlan_id = 10

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        attr_value = sai_thrift_attribute_value_t(booldata=False)
        attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_LEARN_DISABLE, value=attr_value)
        self.client.sai_thrift_set_vlan_attribute(vlan_oid, attr)

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        try:
            # send the test packet(s)
            print "Send ARP request packet from port1 ..."
            arp_req_pkt = simple_arp_packet(eth_dst='ff:ff:ff:ff:ff:ff',
                                            eth_src='00:11:11:11:11:11',
                                            arp_op=1, #ARP request
                                            ip_snd='172.16.10.1',
                                            ip_tgt='172.16.10.2',
                                            hw_snd='00:11:11:11:11:11')

            send_packet(self,switch_ports[0], str(arp_req_pkt))

            time.sleep(2)
            print "Send ARP reply packet from port2 ..."
            arp_rpl_pkt = simple_arp_packet(eth_dst=router_mac,
                                            eth_src='00:11:22:33:44:55',
                                            arp_op=2, #ARP reply
                                            ip_snd='172.16.10.2',
                                            ip_tgt='172.16.10.1',
                                            hw_snd=router_mac,
                                            hw_tgt='00:11:22:33:44:55')

            send_packet(self,switch_ports[1], str(arp_rpl_pkt))

            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:11:22:33:44:55',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64)

            time.sleep(2)
            print "Send packet from port2 to port1 and verify only on port1 (src_mac and dst_mac addresses are learned)"
            print '#### Sending 00:11:11:11:11:11 | 00:11:22:33:44:55 | 172.16.10.1 | 172.16.10.2 | @ ptf_intf 2 ####'
            send_packet(self,switch_ports[1], str(pkt))
            verify_packets(self, pkt, [switch_ports[0]])

        finally:
            time.sleep(10)
            sai_thrift_flush_fdb_by_vlan(self.client, vlan_oid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@group('l2')
class L2FloodFdbMissTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print 'Flood test on ports 1, 2 and 3'
        switch_init(self.client)
        self.test_params = testutils.test_params_get()
        cpu_port = 64
        if self.test_params['arch'] == "tofino2":
            cpu_port = 2
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'

        self.trap_group = sai_thrift_create_hostif_trap_group(self.client, queue_id=4, policer_id=None)
        self.arp_req_trap = sai_thrift_create_hostif_trap(client=self.client,
                                             trap_type=SAI_HOSTIF_TRAP_TYPE_ARP_REQUEST,
                                             packet_action=SAI_PACKET_ACTION_TRAP,
                                             trap_group=self.trap_group)
        self.lldp_trap = sai_thrift_create_hostif_trap(client=self.client,
                                             trap_type=SAI_HOSTIF_TRAP_TYPE_LLDP,
                                             packet_action=SAI_PACKET_ACTION_TRAP,
                                             trap_group=self.trap_group)

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        try:
            self.UnicastActionTest(cpu_port)
            self.BroadcastActionTest(cpu_port)
            self.MulticastActionTest(cpu_port)
        finally:
            time.sleep(5)
            sai_thrift_flush_fdb_by_vlan(self.client, vlan_oid)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan(vlan_oid)

            sai_thrift_remove_hostif_trap(self.client, self.arp_req_trap)
            sai_thrift_remove_hostif_trap(self.client, self.lldp_trap)
            sai_thrift_remove_hostif_trap_group(self.client, self.trap_group)

            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_FORWARD)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_UNICAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_BROADCAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_MULTICAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)

    def UnicastActionTest(self, cpu_port):
        try:
            pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=107,
                                ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=107,
                                ip_ttl=64)
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, exp_pkt,[switch_ports[1], switch_ports[2]])

            #L2 Miss drop action
            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_DROP)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_UNICAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)

            print "L2 MISS drop action"
            send_packet(self,switch_ports[0], str(pkt))
            verify_no_other_packets(self, timeout=2)

            print "L2 MISS Copy to Cpu action"
            #L2 Miss copy to cpu action
            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_COPY)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_UNICAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            send_packet(self,switch_ports[0], str(pkt))
            verify_packet(self, exp_pkt, switch_ports[1])
            verify_packet(self, exp_pkt, switch_ports[2])

            print "L2 MISS Trap action"
            #L2 Miss trap action
            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_TRAP)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_UNICAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            send_packet(self,switch_ports[0], str(pkt))
            result = ptf.testutils.dp_poll(self, port_number=cpu_port)
            if isinstance(result, self.dataplane.PollFailure):
                self.fail("Failed to receive packet on CPU")
            else:
                print "Packet redirect to CPU"
            time.sleep(1)

            print "Regular flood action"
            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_FORWARD)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_UNICAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[1], switch_ports[2]])
        finally:
            pass

    def BroadcastActionTest(self, cpu_port):
        try:
            bcast_pkt = simple_tcp_packet(eth_dst='ff:ff:ff:ff:ff:ff',
                                eth_src='00:11:11:11:11:11',
                                ip_dst='172.16.0.1',
                                ip_id=107,
                                ip_ttl=64)
            pkt = simple_arp_packet(arp_op=1, pktlen=100)

            print "ARP request packet copy to CPU"
            send_packet(self,switch_ports[0], str(pkt))
            result = ptf.testutils.dp_poll(self, port_number=cpu_port)
            if isinstance(result, self.dataplane.PollFailure):
                self.fail("Failed to receive packet on CPU")
            else:
                print "ARP Packet redirect to CPU"
            time.sleep(1)

            print "Broadcast packet should be flooded"
            send_packet(self,switch_ports[0], str(bcast_pkt))
            verify_packets(self, bcast_pkt,[switch_ports[1], switch_ports[2]])

            #L2 Miss drop action
            print "Set bcast action to drop"
            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_DROP)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_BROADCAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)

            print "ARP request should still be copied to CPU"
            send_packet(self,switch_ports[0], str(pkt))
            result = ptf.testutils.dp_poll(self, port_number=cpu_port)
            if isinstance(result, self.dataplane.PollFailure):
                self.fail("Failed to receive packet on CPU")
            else:
                print "Packet redirect to CPU"
            time.sleep(1)

            print "Broadcast packet should be dropped"
            send_packet(self,switch_ports[0], str(bcast_pkt))
            verify_no_other_packets(self, timeout=2)

            print "Set bcast action back to forward"
            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_FORWARD)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_BROADCAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)

            print "Broadcast packet should be flooded again"
            send_packet(self,switch_ports[0], str(bcast_pkt))
            verify_packets(self, bcast_pkt,[switch_ports[1], switch_ports[2]])
        finally:
            pass

    def MulticastActionTest(self, cpu_port):
        try:
            lldp_pkt = simple_eth_packet(eth_dst='01:80:c2:00:00:0e',
                                eth_src='00:11:11:11:11:11',
                                pktlen=48,
                                eth_type=0x88cc)

            print "LLDP packet copy to CPU"
            send_packet(self,switch_ports[0], str(lldp_pkt))
            result = ptf.testutils.dp_poll(self, port_number=cpu_port)
            if isinstance(result, self.dataplane.PollFailure):
                self.fail("Failed to receive packet on CPU")
            else:
                print "Packet redirect to CPU"
            time.sleep(1)

            #L2 Miss drop action
            print "Set mcast action to drop"
            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_DROP)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_MULTICAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)

            print "LLDP should still be copied to CPU"
            send_packet(self,switch_ports[0], str(lldp_pkt))
            result = ptf.testutils.dp_poll(self, port_number=cpu_port)
            if isinstance(result, self.dataplane.PollFailure):
                self.fail("Failed to receive packet on CPU")
            else:
                print "Packet redirect to CPU"
            time.sleep(1)

            print "Set mcast action back to forward"
            attr_value = sai_thrift_attribute_value_t(s32=SAI_PACKET_ACTION_FORWARD)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_MULTICAST_MISS_PACKET_ACTION, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
        finally:
            pass

@group('l2')
@group('lag')
class L2LagDynLearningTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]

        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)

        lag_id1 = sai_thrift_create_lag(self.client, [])

        lag_member_id1 = sai_thrift_create_lag_member(self.client, lag_id1, port1)
        lag_member_id2 = sai_thrift_create_lag_member(self.client, lag_id1, port2)

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, lag_id1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_oid, port4, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_lag_attribute(lag_id1, attr)

        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port3, attr) 

        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port4, attr)

        try:
            pkt1 = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_dst='1.1.1.1',
                                    ip_src='10.10.10.1',
                                    ip_id=109,
                                    ip_ttl=64)

            exp_pkt1 = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_dst='1.1.1.1',
                                        ip_src='10.10.10.1',
                                        ip_id=109,
                                        ip_ttl=64)

            print "Sending packet port 4 -> lag 1 (port 0, 1), port 3"
            send_packet(self, switch_ports[3], str(pkt1))
            verify_packet(self, exp_pkt1, switch_ports[2])
            verify_packet_any_port(self, exp_pkt1, [switch_ports[0], switch_ports[1]])

            time.sleep(1)

            pkt2 = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='2.2.2.1',
                                    ip_src='20.20.20.1',
                                    ip_id=109,
                                    ip_ttl=64)
            exp_pkt2 = simple_tcp_packet(eth_dst='00:22:22:22:22:22',
                                        eth_src='00:11:11:11:11:11',
                                        ip_dst='2.2.2.1',
                                        ip_src='20.20.20.1',
                                        ip_id=109,
                                        ip_ttl=64)
            print "Sending packet port 1 (lag member) -> port 4"
            send_packet(self,switch_ports[0], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[3]])
            print "Sending packet port 2 (lag member) -> port 4"
            send_packet(self,switch_ports[1], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[3]])

            time.sleep(1)

            pkt3 = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_dst='3.3.3.1',
                                    ip_src='30.30.30.1',
                                    ip_id=109,
                                    ip_ttl=64)

            exp_pkt3 = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_dst='3.3.3.1',
                                        ip_src='30.30.30.1',
                                        ip_id=109,
                                        ip_ttl=64)

            print "Sending packet port 4 -> lag 1 (port 0, 1)"
            send_packet(self, switch_ports[3], str(pkt3))
            verify_any_packet_any_port(self, [exp_pkt3], [switch_ports[0], switch_ports[1]])

            pkt4 = simple_tcp_packet(eth_dst='00:33:33:33:33:33',
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='4.4.4.1',
                                    ip_src='40.40.40.1',
                                    ip_id=109,
                                    ip_ttl=64)
            exp_pkt4 = simple_tcp_packet(eth_dst='00:33:33:33:33:33',
                                        eth_src='00:11:11:11:11:11',
                                        ip_dst='4.4.4.1',
                                        ip_src='40.40.40.1',
                                        ip_id=109,
                                        ip_ttl=64)

            print "Sending packet port 1 (lag member) -> port 3, 4"
            send_packet(self,switch_ports[0], str(pkt4))
            verify_packets(self, exp_pkt4, [switch_ports[2],switch_ports[3]])
            print "Sending packet port 2 (lag member) -> port 3, 4"
            send_packet(self,switch_ports[1], str(pkt4))
            verify_packets(self, exp_pkt4, [switch_ports[2],switch_ports[3]])

        finally:
            sai_thrift_flush_fdb_by_vlan(self.client, vlan_oid)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)

            sai_thrift_remove_lag_member(self.client, lag_member_id1)
            sai_thrift_remove_lag_member(self.client, lag_member_id2)
            sai_thrift_remove_lag(self.client, lag_id1)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            for port in sai_port_list:
                self.client.sai_thrift_set_port_attribute(port, attr)
            self.client.sai_thrift_remove_vlan(vlan_oid)

@pktpy_skip  # TODO bf-pktpy
@group('l2')
class LagFdbFlushTest(sai_base_test.ThriftInterfaceDataPlane):
    vlan_members = []
    vlans = []
    default_bridge_ports = []
    vlan_ports = []
    lags = []
    lag_members = []
    default_aging_time = 10

    def cleanup(self):
       attr_value = sai_thrift_attribute_value_t(u32=self.default_aging_time.value.u32)
       attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_AGING_TIME, value=attr_value)
       self.client.sai_thrift_set_switch_attribute(attr)
       for vlan in list(self.vlans):
           sai_thrift_flush_fdb_by_vlan(self.client, vlan)
           vlan_attr_value = sai_thrift_attribute_value_t(u16=1)
           vlan_attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=vlan_attr_value)
       for port in list(self.vlan_ports):
           self.client.sai_thrift_set_port_attribute(port, vlan_attr)
           self.vlan_ports.remove(port)
       for vlan_member in list(self.vlan_members):
           self.client.sai_thrift_remove_vlan_member(vlan_member)
           self.vlan_members.remove(vlan_member)
       for lag_member in list(self.lag_members):
           sai_thrift_remove_lag_member(self.client, lag_member)
           self.lag_members.remove(lag_member)
       for lag in list(self.lags):
           sai_thrift_remove_lag(self.client, lag)
           self.lags.remove(lag)
       for vlan in list(self.vlans):
           self.client.sai_thrift_remove_vlan(vlan)
           self.vlans.remove(vlan)

    def runTest(self):
       switch_init(self.client)

       port1 = port_list[0]
       port2 = port_list[1]
       port3 = port_list[2]
       port4 = port_list[3]

       sai_thrift_vlan_remove_all_ports(self.client, switch.default_vlan.oid)

       lag_id1 = sai_thrift_create_lag(self.client, [])
       self.lags.append(lag_id1)
       lag_member_id1 = sai_thrift_create_lag_member(self.client, lag_id1, port3)
       self.lag_members.append(lag_member_id1)
       lag_member_id2 = sai_thrift_create_lag_member(self.client, lag_id1, port4)
       self.lag_members.append(lag_member_id2)

       vlan_oid = sai_thrift_create_vlan(self.client, 10)
       self.vlans.append(vlan_oid)

       vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
       self.vlan_members.append(vlan_member)
       self.vlan_ports.append(port1)
       vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
       self.vlan_members.append(vlan_member)
       self.vlan_ports.append(port2)
       vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, lag_id1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
       self.vlan_members.append(vlan_member)
       self.vlan_ports.extend([port3, port4])

       #Vlan10
       attr_value = sai_thrift_attribute_value_t(u16=10)
       attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
       self.client.sai_thrift_set_lag_attribute(lag_id1, attr)

       attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
       self.client.sai_thrift_set_port_attribute(port1, attr)
       self.client.sai_thrift_set_port_attribute(port2, attr)

       pkt1 = simple_eth_packet(pktlen=100,
         eth_dst='00:01:02:03:04:05',
         eth_src='00:06:07:08:09:0a')

       exp_pkt1 = pkt1

       pkt2 = simple_eth_packet(pktlen=100,
         eth_dst='00:06:07:08:09:0a',
         eth_src='00:05:04:03:02:01')
       exp_pkt2 = pkt2

       pkt3 = simple_eth_packet(pktlen=100,
         eth_dst='00:06:07:08:09:0b',
         eth_src='00:05:04:03:02:02')
       exp_pkt3 = pkt3

       pkt4 = simple_eth_packet(pktlen=100,
         eth_dst='00:05:04:03:02:02',
         eth_src='00:06:07:08:09:0c')
       exp_pkt4 = pkt4

       self.default_aging_time = self.client.sai_thrift_get_switch_attribute_by_id(SAI_SWITCH_ATTR_FDB_AGING_TIME)

       fdb_aging_time = 25
       attr_value = sai_thrift_attribute_value_t(u32=fdb_aging_time)
       attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_FDB_AGING_TIME, value=attr_value)
       self.client.sai_thrift_set_switch_attribute(attr)

       try:
         #(Flood) Lag -> port1, port2
         send_packet(self, switch_ports[2], str(pkt1))
         verify_packets(self, exp_pkt1, [switch_ports[0], switch_ports[1]])

         #port1 -> Lag Port (port3/port4)
         send_packet(self, switch_ports[0], str(pkt2))
         verify_any_packet_on_ports_list(self, exp_pkt2, [[switch_ports[2], switch_ports[3]]])

         #(Flood) port2 -> Lag Port (port3/port4) & port1
         send_packet(self, switch_ports[1], str(pkt3))
         verify_any_packet_on_ports_list(self, exp_pkt3, [[switch_ports[0]], [switch_ports[2], switch_ports[3]]])

         #Lag -> port2
         send_packet(self, switch_ports[2], str(pkt4))
         verify_any_packet_on_ports_list(self, exp_pkt4, [[switch_ports[1]]])

         #Flush Fdb
         sai_thrift_flush_fdb_by_lag_port(self.client, lag_id1)

         #port1 -> Lag Port (port3/port4) & port 2
         send_packet(self, switch_ports[0], str(pkt2))
         verify_any_packet_on_ports_list(self, [exp_pkt2], [[switch_ports[1]], [switch_ports[2], switch_ports[3]]])

         sai_thrift_flush_fdb_by_lag_port(self.client, port2)
         #Lag -> port2 & port 1
         send_packet(self, switch_ports[2], str(pkt4))
         verify_any_packet_on_ports_list(self, exp_pkt4, [[switch_ports[0]], [switch_ports[1]]])
         print "PASS"
       finally:
         self.cleanup()

@group('l2')
class L2FdbFlushTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        switch_init(self.client)
        vlan_id_10 = 10
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        port5 = port_list[4]
        port6 = port_list[5]
        mac1 = '00:11:11:11:11:11'
        mac2 = '00:22:22:22:22:22'
        mac3 = '00:33:33:33:33:33'
        mac4 = '00:44:44:44:44:44'
        mac5 = '00:55:55:55:55:55'
        mac6 = '00:66:66:66:66:66'
        mac_action = SAI_PACKET_ACTION_FORWARD

        vlan_oid_10 = sai_thrift_create_vlan(self.client, vlan_id_10)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid_10, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid_10, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_oid_10, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member4 = sai_thrift_create_vlan_member(self.client, vlan_oid_10, port4, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id_10)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)
        self.client.sai_thrift_set_port_attribute(port4, attr)

        sai_thrift_create_fdb(self.client, vlan_oid_10, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid_10, mac2, port2, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid_10, mac3, port3, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid_10, mac4, port4, mac_action)

        try:
            print "verify forwarding with static MACs"
            for mac, port in zip([mac2, mac3, mac4], [1,2,3]):
                pkt = simple_tcp_packet(eth_dst=mac,
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
                send_packet(self,switch_ports[0], str(pkt))
                verify_packets(self, pkt, [switch_ports[port]])

            print "Flush by vlan on vlan 10"
            sai_thrift_flush_fdb(self.client, vlan_oid=vlan_oid_10, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_STATIC)
            sai_thrift_create_fdb(self.client, vlan_oid_10, mac1, port1, mac_action)
            for mac in [mac2, mac3, mac4]:
                pkt = simple_tcp_packet(eth_dst=mac,
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
                send_packet(self,switch_ports[0], str(pkt))
                verify_packets(self, pkt, [switch_ports[1], switch_ports[2], switch_ports[3]])

            print "Add static macs on port 2"
            sai_thrift_create_fdb(self.client, vlan_oid_10, mac2, port2, mac_action)
            sai_thrift_create_fdb(self.client, vlan_oid_10, mac3, port2, mac_action)
            sai_thrift_create_fdb(self.client, vlan_oid_10, mac4, port2, mac_action)
            for mac in [mac2, mac3, mac4]:
                pkt = simple_tcp_packet(eth_dst=mac,
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='172.16.0.1',
                                    ip_id=100,
                                    ip_ttl=64)
                send_packet(self,switch_ports[0], str(pkt))
                verify_packets(self, pkt, [switch_ports[1]])

            print "Flush by port on port 2 and verify mac is not flushed on port 1"
            sai_thrift_flush_fdb(self.client, lag_port_oid=port2, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_STATIC)
            sai_thrift_create_fdb(self.client, vlan_oid_10, mac5, port3, mac_action)
            pkt = simple_tcp_packet(eth_dst=mac1,
                                    eth_src=mac5,
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self,switch_ports[2], str(pkt))
            verify_packets(self, pkt, [switch_ports[0]])

            print "Verify packets are flooded after port 2 macs are flushed"
            for mac in [mac2, mac3, mac4]:
                pkt = simple_tcp_packet(eth_dst=mac,
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
                send_packet(self,switch_ports[0], str(pkt))
                verify_packets(self, pkt, [switch_ports[1], switch_ports[2], switch_ports[3]])

            print "Flush all MACs on all ports and vlans"
            sai_thrift_flush_fdb(self.client, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            print "Learn dynamic macs on all ports"
            for mac, port in zip([mac1, mac2, mac3, mac4], [0,1,2,3]):
                pkt = simple_tcp_packet(eth_dst='00:22:33:44:55:66',
                                    eth_src=mac,
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
                ports = [switch_ports[0], switch_ports[1], switch_ports[2], switch_ports[3]]
                ports.remove(switch_ports[port])
                send_packet(self,switch_ports[port], str(pkt))
                verify_packets(self, pkt, ports)

            time.sleep(3)
            print "No flooding anymore after learning"
            for mac, port in zip([mac2, mac3, mac4], [1,2,3]):
                pkt = simple_tcp_packet(eth_dst=mac,
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
                send_packet(self,switch_ports[0], str(pkt))
                verify_packets(self, pkt, [switch_ports[port]])

            print "Flush by vlan all dynamic macs, flood again"
            sai_thrift_flush_fdb(self.client, vlan_oid=vlan_oid_10, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
            for mac, port in zip([mac1, mac2, mac3, mac4], [0,1,2,3]):
                pkt = simple_tcp_packet(eth_dst='00:22:33:44:55:66',
                                    eth_src=mac,
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
                ports = [switch_ports[0], switch_ports[1], switch_ports[2], switch_ports[3]]
                ports.remove(switch_ports[port])
                send_packet(self,switch_ports[port], str(pkt))
                verify_packets(self, pkt, ports)

            time.sleep(3)
            print "Flush by port on port 2"
            sai_thrift_flush_fdb(self.client, lag_port_oid=port2, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
            pkt = simple_tcp_packet(eth_dst=mac2,
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, pkt, [switch_ports[1], switch_ports[2], switch_ports[3]])
            print "Flush by port on port 3"
            sai_thrift_flush_fdb(self.client, lag_port_oid=port3, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
            pkt = simple_tcp_packet(eth_dst=mac3,
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, pkt, [switch_ports[1], switch_ports[2], switch_ports[3]])
            print "Flush by port on port 4"
            sai_thrift_flush_fdb(self.client, lag_port_oid=port4, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
            pkt = simple_tcp_packet(eth_dst=mac4,
                                    eth_src='00:11:11:11:11:11',
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self,switch_ports[0], str(pkt))
            verify_packets(self, pkt, [switch_ports[1], switch_ports[2], switch_ports[3]])

            print "Flush by vlan and port on port 1"
            sai_thrift_flush_fdb(self.client, vlan_oid=vlan_oid_10, lag_port_oid=port1, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
            pkt = simple_tcp_packet(eth_dst=mac1,
                                    eth_src=mac2,
                                    ip_dst='172.16.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self,switch_ports[1], str(pkt))
            verify_packets(self, pkt, [switch_ports[0], switch_ports[2], switch_ports[3]])
        finally:
            sai_thrift_flush_fdb(self.client, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)
            self.client.sai_thrift_set_port_attribute(port4, attr)
            self.client.sai_thrift_set_port_attribute(port5, attr)
            self.client.sai_thrift_set_port_attribute(port6, attr)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan_member(vlan_member3)
            self.client.sai_thrift_remove_vlan_member(vlan_member4)
            self.client.sai_thrift_remove_vlan(vlan_oid_10)
