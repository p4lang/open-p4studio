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
Thrift SAI interface basic tests
"""

import switchsai_thrift

import time
import sys
import logging

import unittest
import random

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os

import sai_base_test
from switchsai_thrift.ttypes import *
from switchsai_thrift.sai_headers import *
from switch_utils import *
from bf_switcht_api_thrift.model_headers import *

@group('qos')
@group('bfn')
class L3IPv4QosDscpRewriteTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        v4_enabled = 1
        v6_enabled = 1
        mac_valid = 0
        mac = ''

        lag1 = sai_thrift_create_lag(self.client, [])
        lag_member1 = sai_thrift_create_lag_member(self.client, lag1, port3)

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)
        rif_id3 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, lag1, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '10.10.10.1'
        ip_addr1_subnet = '10.10.10.0'
        ip_mask1 = '255.255.255.0'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        ingress_dscp_list = [1, 2, 3, 4]
        ingress_tc_list = [11, 12, 13, 14]
        ingress_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_DSCP_TO_TC, ingress_dscp_list, ingress_tc_list)

        ingress_tc_list = [11, 12, 13, 14]
        ingress_queue_list = [1, 2, 3, 4]
        #tc_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_TC_TO_QUEUE, ingress_tc_list, ingress_queue_list)

        egress_tc_and_color_list = [[11, 0], [12, 0], [13, 0,], [14, 0]]
        egress_dscp_list = [10, 20, 30, 40]
        egress_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP, egress_tc_and_color_list, egress_dscp_list)

        sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, ingress_qos_map_id)
        ##sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP, egress_qos_map_id)

        sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, ingress_qos_map_id)
        ##sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP, egress_qos_map_id)

        sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, ingress_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP, egress_qos_map_id)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_tos=4,  # dscp=1
                                ip_id=105,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(
                                eth_dst='00:11:22:33:44:55',
                                eth_src=router_mac,
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_tos=40, # dscp=10
                                ip_id=105,
                                ip_ttl=63)
        try:
            print
            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])
            #Update the existing map entry
            update_dscp_list = [1]
            update_tc_list = [10]
            update_tc_color_list = [[10,0]]
            egress_dscp_list = [60]
            sai_thrift_set_qos_map_attribute(self.client, ingress_qos_map_id, SAI_QOS_MAP_TYPE_DSCP_TO_TC, update_dscp_list, update_tc_list);
            sai_thrift_set_qos_map_attribute(self.client, egress_qos_map_id, SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP, update_tc_color_list, egress_dscp_list);
            exp_pkt = simple_tcp_packet(
                                    eth_dst='00:11:22:33:44:55',
                                    eth_src=router_mac,
                                    ip_dst='10.10.10.1',
                                    ip_src='192.168.0.1',
                                    ip_tos=240, # dscp=60
                                    ip_id=105,
                                    ip_ttl=63)
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            #Update the existing map with a new entry
            update_dscp_list = [5]
            update_tc_list = [17]
            update_tc_color_list = [[17,0]]
            egress_dscp_list = [50]
            sai_thrift_set_qos_map_attribute(self.client, ingress_qos_map_id, SAI_QOS_MAP_TYPE_DSCP_TO_TC, update_dscp_list, update_tc_list);
            sai_thrift_set_qos_map_attribute(self.client, egress_qos_map_id, SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP, update_tc_color_list, egress_dscp_list);
            #Send packet with DSCP 5 and egress with updated DSCP 50
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_dst='10.10.10.1',
                                    ip_src='192.168.0.1',
                                    ip_tos=20,  # dscp=5
                                    ip_id=105,
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                                    eth_dst='00:11:22:33:44:55',
                                    eth_src=router_mac,
                                    ip_dst='10.10.10.1',
                                    ip_src='192.168.0.1',
                                    ip_tos=200, # dscp=50
                                    ip_id=105,
                                    ip_ttl=63)

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])
        finally:
            sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            #sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            #sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_remove_qos_map(self.client, ingress_qos_map_id)
            #sai_thrift_remove_qos_map(self.client, tc_qos_map_id)
            sai_thrift_remove_qos_map(self.client, egress_qos_map_id)
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, nhop1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_router_interface(rif_id3)

            self.client.sai_thrift_remove_virtual_router(vr_id)

            sai_thrift_remove_lag_member(self.client, lag_member1)
            sai_thrift_remove_lag(self.client, lag1)

@group('qos')
@group('bfn')
class L3IPv4QueueQosMapUpdateDeleteTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        v4_enabled = 1
        v6_enabled = 1
        mac_valid = 0
        mac = ''

        lag1 = sai_thrift_create_lag(self.client, [])
        lag_member1 = sai_thrift_create_lag_member(self.client, lag1, port3)

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)
        rif_id3 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, lag1, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '10.10.10.1'
        ip_addr1_subnet = '10.10.10.0'
        ip_mask1 = '255.255.255.0'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        ingress_dscp_list = [1, 2, 3, 4]
        ingress_tc_list = [11, 12, 13, 14]
        ingress_queue_list = [1, 2, 3, 4]
        ingress_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_DSCP_TO_TC, ingress_dscp_list, ingress_tc_list)
        tc_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_TC_TO_QUEUE, ingress_tc_list, ingress_queue_list)

        sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, ingress_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, ingress_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, ingress_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_tos= 1 << 2,  # dscp=1
                                ip_id=105,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(
                                eth_dst='00:11:22:33:44:55',
                                eth_src=router_mac,
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_tos= 1 << 2, # dscp=1
                                ip_id=105,
                                ip_ttl=63)
        try:
            attr = sai_thrift_get_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_QUEUE_LIST)
            assert(attr != None), 'attr == None'
            queue_list = attr.value.objlist.object_id_list
            assert(len(queue_list) >= 8), 'Incorrect queue list size: {}'.format(len(queue_list))

            q0_packets = sai_thrift_read_queue_counters(self.client, queue_list[0])[0]
            q1_packets = sai_thrift_read_queue_counters(self.client, queue_list[1])[0]
            q5_packets = sai_thrift_read_queue_counters(self.client, queue_list[5])[0]
            q6_packets = sai_thrift_read_queue_counters(self.client, queue_list[6])[0]

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            # Traffic should go to queue 1
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[0]
            assert(pkt_count == q1_packets + 2), 'queue 1 packets counter {} != {}'.format(pkt_count, q1_packets + 2)

            # Now add new element to each mapping
            update_ingress_dscp_list = [1, 2, 3, 4, 5]
            update_ingress_tc_list = [11, 12, 13, 14, 15]
            # TC 11 will point to queue 5
            update_ingress_queue_list = [5, 2, 3, 4, 6]

            # Update the maps
            sai_thrift_set_qos_map_attribute(self.client, ingress_qos_map_id, SAI_QOS_MAP_TYPE_DSCP_TO_TC, update_ingress_dscp_list, update_ingress_tc_list)
            sai_thrift_set_qos_map_attribute(self.client, tc_qos_map_id, SAI_QOS_MAP_TYPE_TC_TO_QUEUE, update_ingress_tc_list, update_ingress_queue_list)

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            # Test new mapping added
            pkt2 = pkt.copy()
            pkt2[IP].tos = 5 << 2 #dscp 5
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IP].tos = 5 << 2 #dscp 5

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[0]])

            # Now traffic with dscp 1 should go to queue 5
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[5])[0]
            assert(pkt_count == q5_packets + 2), 'queue 5 packets counter {} != {}'.format(pkt_count, q5_packets + 2)

            # Traffic with dscp 5 should go to queue 6
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[6])[0]
            assert(pkt_count == q6_packets + 2), 'queue 6 packets counter {} != {}'.format(pkt_count, q6_packets + 2)

            # Queue 1 counter should still be the same
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[0]
            assert(pkt_count == q1_packets + 2), 'queue 1 packets counter {} != {}'.format(pkt_count, q1_packets + 2)

            # Now remove one element from each mapping
            update_ingress_dscp_list = [1, 2, 3, 4]
            update_ingress_tc_list = [11, 12, 13, 14]
            # TC 11 will point to queue 5
            update_ingress_queue_list = [5, 2, 3, 4]

            # Update the maps
            sai_thrift_set_qos_map_attribute(self.client, ingress_qos_map_id, SAI_QOS_MAP_TYPE_DSCP_TO_TC, update_ingress_dscp_list, update_ingress_tc_list)
            sai_thrift_set_qos_map_attribute(self.client, tc_qos_map_id, SAI_QOS_MAP_TYPE_TC_TO_QUEUE, update_ingress_tc_list, update_ingress_queue_list)

            # Test the update mapping
            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[0]])

            # Traffic with dscp 1 should still go to queue 5
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[5])[0]
            assert(pkt_count == q5_packets + 4), 'queue 5 packets counter {} != {}'.format(pkt_count, q5_packets + 4)

            # Traffic with dscp 5 should go to queue 0
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[0])[0]
            assert(pkt_count == q0_packets + 2), 'queue 6 packets counter {} != {}'.format(pkt_count, q0_packets + 2)

            # Unassign TC to Queue map from the ports and remove it
            sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_remove_qos_map(self.client, tc_qos_map_id)
            tc_qos_map_id = 0

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[0]])
            print "Sending packet lag 1 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[2], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[0]])

            # Now traffic should go to queue 0
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[0])[0]
            assert(pkt_count == q0_packets + 6), 'queue 0 packets counter {} != {}'.format(pkt_count, q0_packets + 6)

            # queue 1, 5 and 6 counters should still be the same
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[5])[0]
            assert(pkt_count == q5_packets + 4), 'queue 5 packets counter {} != {}'.format(pkt_count, q5_packets + 4)
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[6])[0]
            assert(pkt_count == q6_packets + 2), 'queue 6 packets counter {} != {}'.format(pkt_count, q6_packets + 2)
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[0]
            assert(pkt_count == q1_packets + 2), 'queue 1 packets counter {} != {}'.format(pkt_count, q1_packets + 2)

        finally:
            sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port3, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_remove_qos_map(self.client, ingress_qos_map_id)
            if tc_qos_map_id != 0:
                sai_thrift_remove_qos_map(self.client, tc_qos_map_id)
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, nhop1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_router_interface(rif_id3)

            self.client.sai_thrift_remove_virtual_router(vr_id)

            sai_thrift_remove_lag_member(self.client, lag_member1)
            sai_thrift_remove_lag(self.client, lag1)

@group('qos')
class L3IPv4PpgCountersClearTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        v4_enabled = 1
        v6_enabled = 1
        mac_valid = 0
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '10.10.10.1'
        ip_addr1_subnet = '10.10.10.0'
        ip_mask1 = '255.255.255.0'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_tos= 1 << 2,  # dscp=1
                                ip_id=105,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(
                                eth_dst='00:11:22:33:44:55',
                                eth_src=router_mac,
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_tos= 1 << 2, # dscp=1
                                ip_id=105,
                                ip_ttl=63)
        try:

            ppg_list_attr = sai_thrift_attribute_t(id = SAI_PORT_ATTR_INGRESS_PRIORITY_GROUP_LIST)
            attr_list = self.client.sai_thrift_get_port_handles_attribute(port2, ppg_list_attr)
            num_ppgs = attr_list.value.objlist.count
            ppg_list = attr_list.value.objlist.object_id_list

            assert (len(ppg_list) > 0), 'Empty ppg list'

            ppg_packets_pre = sai_thrift_read_ppg_counters(self.client, ppg_list[0])[0]
            ppg_bytes_pre = sai_thrift_read_ppg_counters(self.client, ppg_list[0])[1]

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1)"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            # Check counters on ppg
            pkt_count = sai_thrift_read_ppg_counters(self.client, ppg_list[0])[0]
            bytes_count = sai_thrift_read_ppg_counters(self.client, ppg_list[0])[1]

            assert(pkt_count == ppg_packets_pre + 1), 'ppg packets counter {} != {}'.format(pkt_count, ppg_packets_pre + 1)
            assert((bytes_count - ppg_bytes_pre) != 0), 'ppg bytes counter is 0'

            # Clear bytes counter
            cnt_ids = [SAI_INGRESS_PRIORITY_GROUP_STAT_BYTES]
            self.client.sai_thrift_clear_pg_stats(ppg_list[0], cnt_ids, len(cnt_ids))

            # Check counters on ppg
            pkt_count = sai_thrift_read_ppg_counters(self.client, ppg_list[0])[0]
            bytes_count = sai_thrift_read_ppg_counters(self.client, ppg_list[0])[1]
            assert(pkt_count == ppg_packets_pre + 1), 'ppg packets counter {} != {}'.format(pkt_count, ppg_packets_pre + 1)
            assert(bytes_count == 0), 'ppg bytes counter is not 0'

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1)"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            # Clear bytes and packets counter
            cnt_ids = [SAI_INGRESS_PRIORITY_GROUP_STAT_BYTES, SAI_INGRESS_PRIORITY_GROUP_STAT_PACKETS]
            self.client.sai_thrift_clear_pg_stats(ppg_list[0], cnt_ids, len(cnt_ids))

            # Check counters on queue 1
            pkt_count = sai_thrift_read_ppg_counters(self.client, ppg_list[0])[0]
            bytes_count = sai_thrift_read_ppg_counters(self.client, ppg_list[0])[1]
            assert(pkt_count == 0), 'ppg packets counter is not 0'
            assert(bytes_count == 0), 'ppg bytes counter is not 0'

        finally:
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, nhop1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)

            self.client.sai_thrift_remove_virtual_router(vr_id)

@group('qos')
@group('bfn')
class L3IPv4QueueCountersClearTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        v4_enabled = 1
        v6_enabled = 1
        mac_valid = 0
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '10.10.10.1'
        ip_addr1_subnet = '10.10.10.0'
        ip_mask1 = '255.255.255.0'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        ingress_dscp_list = [1, 2, 3, 4]
        ingress_tc_list = [11, 12, 13, 14]
        ingress_queue_list = [1, 2, 3, 4]
        ingress_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_DSCP_TO_TC, ingress_dscp_list, ingress_tc_list)
        tc_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_TC_TO_QUEUE, ingress_tc_list, ingress_queue_list)

        sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, ingress_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, ingress_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_tos= 1 << 2,  # dscp=1
                                ip_id=105,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(
                                eth_dst='00:11:22:33:44:55',
                                eth_src=router_mac,
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_tos= 1 << 2, # dscp=1
                                ip_id=105,
                                ip_ttl=63)
        try:
            attr = sai_thrift_get_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_QUEUE_LIST)
            assert(attr != None), 'attr == None'
            queue_list = attr.value.objlist.object_id_list
            assert(len(queue_list) >= 8), 'Incorrect queue list size: {}'.format(len(queue_list))

            q1_packets_pre = sai_thrift_read_queue_counters(self.client, queue_list[1])[0]
            q1_bytes_pre = sai_thrift_read_queue_counters(self.client, queue_list[1])[1]

            print "Sending packet port 2 -> port 1 (192.168.0.1 -> 172.16.10.1 [id = 101])"
            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            # Check counters on queue 1
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[0]
            bytes_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[1]
            assert(pkt_count == q1_packets_pre + 1), 'queue 1 packets counter {} != {}'.format(pkt_count, q1_packets_pre + 1)
            assert((bytes_count - q1_bytes_pre) != 0), 'queue 1 bytes counter is 0'

            # Clear bytes counter
            cnt_ids = [SAI_QUEUE_STAT_BYTES]
            self.client.sai_thrift_clear_queue_stats(queue_list[1], cnt_ids, len(cnt_ids))

            # Check counters on queue 1
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[0]
            bytes_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[1]
            assert(pkt_count == q1_packets_pre + 1), 'queue 1 packets counter {} != {}'.format(pkt_count, q1_packets_pre + 1)
            assert(bytes_count == 0), 'queue 1 bytes counter is not 0'

            # Clear bytes and packets counter
            cnt_ids = [SAI_QUEUE_STAT_BYTES, SAI_QUEUE_STAT_PACKETS]
            self.client.sai_thrift_clear_queue_stats(queue_list[1], cnt_ids, len(cnt_ids))

            # Check counters on queue 1
            pkt_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[0]
            bytes_count = sai_thrift_read_queue_counters(self.client, queue_list[1])[1]
            assert(pkt_count == 0), 'queue 1 packets counter is not 0'
            assert(bytes_count == 0), 'queue 1 bytes counter is not 0'

        finally:
            sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_remove_qos_map(self.client, ingress_qos_map_id)
            sai_thrift_remove_qos_map(self.client, tc_qos_map_id)
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, nhop1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)

            self.client.sai_thrift_remove_virtual_router(vr_id)

class StormControlTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) == 0):
          print("Storm Control feature not enable, skipping")
          return

        switch_init(self.client)
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        port4 = port_list[4]
        mac1 = '00:22:22:22:22:22'
        mac2 = '00:11:11:11:11:11'
        mac_action = SAI_PACKET_ACTION_FORWARD

        self.vlan10 = sai_thrift_create_vlan(self.client, 10)
        vlan_member11 = sai_thrift_create_vlan_member(self.client, self.vlan10, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member12 = sai_thrift_create_vlan_member(self.client, self.vlan10, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        vlan_member13 = sai_thrift_create_vlan_member(self.client, self.vlan10, port3, SAI_VLAN_TAGGING_MODE_TAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=10)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        sai_thrift_create_fdb(self.client, self.vlan10, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, self.vlan10, mac2, port2, mac_action)

        uc_sc = sai_thrift_create_policer(self.client,
                                            meter_type=SAI_METER_TYPE_PACKETS,
                                            mode=SAI_POLICER_MODE_STORM_CONTROL,
                                            cir=2,
                                            cbs=4,
                                            pir=2,
                                            pbs=4,
                                            red_action=SAI_PACKET_ACTION_DROP)
        mc_sc = sai_thrift_create_policer(self.client,
                                            meter_type=SAI_METER_TYPE_PACKETS,
                                            mode=SAI_POLICER_MODE_STORM_CONTROL,
                                            cir=2,
                                            cbs=4,
                                            pir=2,
                                            pbs=4,
                                            red_action=SAI_PACKET_ACTION_DROP)
        bc_sc = sai_thrift_create_policer(self.client,
                                            meter_type=SAI_METER_TYPE_PACKETS,
                                            mode=SAI_POLICER_MODE_STORM_CONTROL,
                                            cir=1,
                                            cbs=2,
                                            pir=1,
                                            pbs=2,
                                            red_action=SAI_PACKET_ACTION_DROP)

        a_val = sai_thrift_attribute_value_t(oid=uc_sc)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_FLOOD_STORM_CONTROL_POLICER_ID, value=a_val)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        tattr = sai_thrift_get_port_attribute(self.client, port1, SAI_PORT_ATTR_FLOOD_STORM_CONTROL_POLICER_ID)
        assert(uc_sc == tattr.value.oid)

        a_val = sai_thrift_attribute_value_t(oid=mc_sc)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_MULTICAST_STORM_CONTROL_POLICER_ID, value=a_val)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        tattr = sai_thrift_get_port_attribute(self.client, port2, SAI_PORT_ATTR_MULTICAST_STORM_CONTROL_POLICER_ID)
        assert(mc_sc == tattr.value.oid)

        a_val = sai_thrift_attribute_value_t(oid=bc_sc)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_BROADCAST_STORM_CONTROL_POLICER_ID, value=a_val)
        self.client.sai_thrift_set_port_attribute(port3, attr)
        tattr = sai_thrift_get_port_attribute(self.client, port3, SAI_PORT_ATTR_BROADCAST_STORM_CONTROL_POLICER_ID)
        assert(bc_sc == tattr.value.oid)

        def verifyPortScStats(policer, pkt_sent, meter_cnt):
            green, yellow, red = self.client.sai_thrift_get_policer_stats(policer,
                [SAI_POLICER_STAT_GREEN_PACKETS, SAI_POLICER_STAT_YELLOW_PACKETS, SAI_POLICER_STAT_RED_PACKETS])

            pkt_count = green + red + yellow
            print(" Storm control meter stats")
            print(" Packets sent = %d" %pkt_sent)
            print(" Packets to hit meter  = %d" %meter_cnt)
            print(" Packets green = %d" %green)
            print(" Packets yellow = %d" %yellow)
            print(" Packets red = %d" %red)
            print
            return (pkt_count == meter_cnt)

        def StormControlKUcastTest():
            print("StormControlKUcastTest")
            print(" Test known ucast traffic")
            pkt_cnt = 5
            try:
                pkt = simple_tcp_packet(
                    eth_dst='00:11:11:11:11:11',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='172.16.0.1',
                    ip_id=104,
                    ip_ttl=64)
                v_pkt = simple_tcp_packet(
                    eth_dst='00:11:11:11:11:11',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='172.16.0.1',
                    dl_vlan_enable=True,
                    vlan_vid=10,
                    ip_id=104,
                    ip_ttl=64,
                    pktlen=104)
                rx_cnt = 0
                for i in range(0, pkt_cnt):
                    send_packet(self, switch_ports[1], str(pkt))
                    verify_packets(self, v_pkt, [switch_ports[2]])
                    rx_cnt += 1
                self.assertTrue(rx_cnt == pkt_cnt)
                print(" Known ucast traffic pkt count match")

                # SAI sc port stats not available
                #print("Waiting 2 secs before checking meter stats")
                #time.sleep(2)
                #self.assertTrue(verifyPortScStats(uc_sc, pkt_cnt, 0))
            finally:
                pass

        def StormControlUkUcastTest():
            print("StormControlUkUcastTest")
            print(" Test unknown ucast traffic")
            pkt_cnt = 5
            try:
                pkt = simple_tcp_packet(
                    eth_dst='00:10:10:10:10:10',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='172.16.0.1',
                    ip_id=104,
                    ip_ttl=64)
                v_pkt = simple_tcp_packet(
                    eth_dst='00:10:10:10:10:10',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='172.16.0.1',
                    dl_vlan_enable=True,
                    vlan_vid=10,
                    ip_id=104,
                    ip_ttl=64,
                    pktlen=104)
                for i in range(0, pkt_cnt):
                    send_packet(self, switch_ports[1], str(pkt))

                # SAI sc port stats not available
                #print("Waiting 2 secs before checking meter stats")
                #time.sleep(2)
                #self.assertTrue(verifyPortScStats(uc_sc, pkt_cnt, pkt_cnt))

            finally:
                pass

        def StormControlMcastTest():
            print("StormControlMulticastTest")
            print(" Test unknown mcast traffic")
            pkt_cnt = 5
            try:
                pkt = simple_tcp_packet(
                    eth_dst='01:00:5e:00:01:05',
                    eth_src='00:11:11:11:11:11',
                    ip_dst='225.0.1.5',
                    dl_vlan_enable=True,
                    vlan_vid=10,
                    ip_ttl=64)
                for i in range(0, pkt_cnt):
                    send_packet(self, switch_ports[2], str(pkt))

                # SAI sc port stats not available
                #print("Waiting 2 secs before checking meter stats")
                #time.sleep(2)
                #self.assertTrue(verifyPortScStats(mc_sc, pkt_cnt, pkt_cnt))

            finally:
                pass

        def StormControlBcastTest():
            print("StormControlBcastTest")
            print(" Test bcast traffic")
            pkt_cnt = 5
            try:
                pkt = simple_tcp_packet(
                    eth_dst='ff:ff:ff:ff:ff:ff',
                    eth_src='00:11:11:11:11:11',
                    ip_dst='172.16.0.1',
                    dl_vlan_enable=True,
                    vlan_vid=10,
                    ip_ttl=64)
                for i in range(0, pkt_cnt):
                    send_packet(self, switch_ports[3], str(pkt))

                # SAI sc port stats not available
                #print("Waiting 2 secs before checking meter stats")
                #time.sleep(2)
                #self.assertTrue(verifyPortScStats(bc_sc, pkt_cnt, pkt_cnt))

            finally:
                pass

        try:
            # sc meter stats are not supp from sai
            if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) != 0):
                StormControlKUcastTest()
                #StormControlUkUcastTest()
                #StormControlMcastTest()
                #StormControlBcastTest()

        finally:
            if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) != 0):
                a_val = sai_thrift_attribute_value_t(oid=0)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_FLOOD_STORM_CONTROL_POLICER_ID, value=a_val)
                self.client.sai_thrift_set_port_attribute(port1, attr)

                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_MULTICAST_STORM_CONTROL_POLICER_ID, value=a_val)
                self.client.sai_thrift_set_port_attribute(port2, attr)

                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_BROADCAST_STORM_CONTROL_POLICER_ID, value=a_val)
                self.client.sai_thrift_set_port_attribute(port3, attr)

                self.client.sai_thrift_remove_policer(thrift_policer_id=uc_sc)
                self.client.sai_thrift_remove_policer(thrift_policer_id=mc_sc)
                self.client.sai_thrift_remove_policer(thrift_policer_id=bc_sc)

                sai_thrift_delete_fdb(self.client, self.vlan10, mac1, port1)
                sai_thrift_delete_fdb(self.client, self.vlan10, mac2, port2)

                self.client.sai_thrift_remove_vlan_member(vlan_member11)
                attr_value = sai_thrift_attribute_value_t(u16=1)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port1, attr)

                self.client.sai_thrift_remove_vlan_member(vlan_member12)
                self.client.sai_thrift_remove_vlan_member(vlan_member13)

                self.client.sai_thrift_remove_vlan(self.vlan10)

@group('qos')
@group('bfn')
class QosMapPfcPriorityTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        switch_init(self.client)
        ingress_pfc = [2,3]
        ingress_pg = [2,3]
        ingress_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP, ingress_pfc, ingress_pg)

        try:
            qos_list =  sai_thrift_get_qos_map_attribute(self.client, ingress_qos_map_id, SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST, len(ingress_pfc))
            i = 0
            for qosmap in qos_list.value.qosmap.data:
                assert(qosmap.pg == ingress_pg[i])
                i += 1
            ingress_pfc = [1,4]
            updated_ingress_pg = [1,4]
            sai_thrift_set_qos_map_attribute(self.client, ingress_qos_map_id, SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP, ingress_pfc, updated_ingress_pg)
            qos_list =  sai_thrift_get_qos_map_attribute(self.client, ingress_qos_map_id, SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST, len(ingress_pfc))
            i = 0
            for qosmap in qos_list.value.qosmap.data:
                assert(qosmap.pg == updated_ingress_pg[i])
                i += 1
        finally:
            sai_thrift_remove_qos_map(self.client, ingress_qos_map_id)

class IngressQosMapUpdatetest(sai_base_test.ThriftInterfaceDataPlane):
    baseConfigQosMappings = [
       (0, 0), (1, 1), (2, 2), (3, 3),
       (4, 4), (5, 5), (6, 6), (7, 7)
       ]
    baseTestConfigMappings = [
       (0, 0), (1, 1), (2, 2), (3, 3)
       ]
    testConfigMappings = [
       [(4, 4), (5, 5), (6, 6), (7, 7)],  #No Overlap
       [(0, 4), (1, 5), (2, 6), (3, 7)],  #All keys Exist, but key values different i.e. Full Update
       [(0, 0), (1, 1)],                  #Some keys Exist, Map to Same values i.e. Partial Delete
       [(0, 0), (1, 4)],                  #Some keys Exist, Some keys same value some keys different values i.e Partial Delete + Partial Update
       [(0, 0), (1, 1), (4, 3), (5, 6)],  #Some old keys & some new keys, old keys same value, new keys new values
       [(0, 0), (1, 1), (4, 0), (5, 1)],  #Some old and some new keys, old keys same value. new keys map to samevalues
       [(0, 3), (1, 2), (2, 1), (3, 0)],  #Key Value Pairs reversed
       [(0, 0), (1, 0), (2, 0), (3, 0)],  #All keys remapped to same values
       [(0, 0), (1, 0), (2, 1), (3, 1), (4, 2), (5, 5)],
       [(0, 1), (1, 2), (2, 1), (3, 2)]
       ]

    # Dictionary for qos ingress map types to port qos attrs
    qos_helper_dict = {
        SAI_QOS_MAP_TYPE_DOT1P_TO_TC: { 'port_attr': SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP, 'port_attr_name': 'qos_dot1p_to_tc_map', 'qos_map_name':'dot1p_to_tc'},
        SAI_QOS_MAP_TYPE_DSCP_TO_TC : { 'port_attr': SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, 'port_attr_name': 'qos_dscp_to_tc_map', 'qos_map_name': 'dscp_to_tc'},
        SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP: { 'port_attr':SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP, 'port_attr_name': 'qos_tc_to_priority_group_map', 'qos_map_name': 'tc_to_priority_group'},
        SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP: { 'port_attr': SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP, 'port_attr_name': 'qos_pfc_priority_to_priority_group_map', 'qos_map_name': 'pfc_priority_to_priority_group'}
    }

    def create_qos_test_config(self, mappings):
        return sai_thrift_create_qos_map(self.client, self.test_qos_map_type, [key for key, value in
            mappings], [value for key, value in mappings])

    def setUp(self, test_qos_map_type):
        super(IngressQosMapUpdatetest, self).setUp()
        switch_init(self.client)
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)==0):
            print("Ingress QoS Map feature not enabled, skipping test")
            return
        self.port_test_attr = IngressQosMapUpdatetest.qos_helper_dict[test_qos_map_type]['port_attr']
        self.port_test_attr_name = IngressQosMapUpdatetest.qos_helper_dict[test_qos_map_type]['port_attr_name']
	self.test_qos_map_type = test_qos_map_type
	self.test_qos_map_type_name = IngressQosMapUpdatetest.qos_helper_dict[test_qos_map_type]['qos_map_name']
        self.ppg_handles = []
        self.expected_ppg_stats = [0]*8
        test_l3_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='40.40.1.20',
            ip_id=105,
            ip_dscp=0,
            ip_ttl=64)
        test_l2_pkt = simple_arp_packet(pktlen=60, vlan_vid=100, vlan_pcp=0)
        if self.test_qos_map_type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC:
            self.test_pkt = test_l2_pkt
        else:
            self.test_pkt = test_l3_pkt
        self.test_port = port_list[0]
        # Create ppgs for test port
        for ppg_index in range(8):
            self.ppg_handles.append(self.client.sai_thrift_create_ppg(self.test_port, ppg_index))

        #Setup base forwarding Config
        self.vlan100 = sai_thrift_create_vlan(self.client, 100)
        self.vlan_members = []
        self.vlan_members.append(sai_thrift_create_vlan_member(self.client, self.vlan100, port_list[0],
            SAI_VLAN_TAGGING_MODE_UNTAGGED))
        self.vlan_members.append(sai_thrift_create_vlan_member(self.client, self.vlan100, port_list[1],
            SAI_VLAN_TAGGING_MODE_UNTAGGED
            ))
        attr_value = sai_thrift_attribute_value_t(u16=100)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port_list[0], attr)
        self.client.sai_thrift_set_port_attribute(port_list[1], attr)

        #Setup Base QoS Config
        self.base_config_qos_maps = []
        if self.port_test_attr != SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP and self.port_test_attr != SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP:
            self.base_config_qos_maps.append(sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_DSCP_TO_TC, [dscp for dscp, tc in IngressQosMapUpdatetest.baseConfigQosMappings], [tc for dscp, tc in IngressQosMapUpdatetest.baseConfigQosMappings]))
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP,
                    self.base_config_qos_maps[-1])
        if self.port_test_attr != SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP:
            self.base_config_qos_maps.append(sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP, [tc for tc, pfc_prio in IngressQosMapUpdatetest.baseConfigQosMappings], [pfc_prio for tc, pfc_prio in IngressQosMapUpdatetest.baseConfigQosMappings]))
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP,
                    self.base_config_qos_maps[-1])
        if self.port_test_attr != SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP:
           self.base_config_qos_maps.append(sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP, [pfc_prio for pfc_prio, ppg in IngressQosMapUpdatetest.baseConfigQosMappings], [ppg for pfc_prio, ppg in IngressQosMapUpdatetest.baseConfigQosMappings]))
           sai_thrift_set_port_attribute(self.client, self.test_port,
                    SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP,
                    self.base_config_qos_maps[-1])

        #Setup Base Test Config
        self.test_ingress_qos_map_handle = self.create_qos_test_config([])
        sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr, self.test_ingress_qos_map_handle)

    def tearDown(self):
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)!=0):
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP, SAI_NULL_OBJECT_ID)
            self.verifyQosMapping()
            sai_thrift_remove_qos_map(self.client, self.test_ingress_qos_map_handle)
            for qos_map in self.base_config_qos_maps:
                sai_thrift_remove_qos_map(self.client, qos_map)
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan100)
            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port_list[0], attr)
            self.client.sai_thrift_set_port_attribute(port_list[1], attr)
            for vlan_member in self.vlan_members:
                self.client.sai_thrift_remove_vlan_member(vlan_member)
            self.client.sai_thrift_remove_vlan(self.vlan100)
            for ppg in self.ppg_handles:
                self.client.sai_thrift_remove_ppg(ppg)
        super(IngressQosMapUpdatetest, self).tearDown()

    def verifyQosMapping(self, mapping=[]):
        print "QoS Mapping Verification unimplemented"

    def trafficQosMappingTest(self, mapping=[]):
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_PPG_STATS) == 0):
            print("PPG stats is not enabled, return")
            return

        if mapping:
            pkt_count=1
            exp_stats = [0]*len(self.ppg_handles)
            post_test_stats = [0]*len(self.ppg_handles)
            num_pkts = []
            for key, v in mapping:
                if self.test_qos_map_type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC:
                    self.test_pkt[Ether][Dot1Q].prio = key
                else:
                    self.test_pkt[IP].tos = key << 2
                for count in range(pkt_count):
                    send_packet(self, switch_ports[0], str(self.test_pkt))
                    self.expected_ppg_stats[v]+=1
                num_pkts.append(pkt_count)
                pkt_count+=1
            print("Waiting for 8 sec... before collecting stats")
            time.sleep(8)
            for ppg_index in range(len(self.ppg_handles)):
                counters = sai_thrift_read_ppg_counters(self.client, self.ppg_handles[ppg_index])
                post_test_stats[ppg_index] = counters[0]
            print("Verifying PPG stats for qos mapping:{}, packet per mapping:{}".format(mapping, num_pkts))
            self.assertTrue(post_test_stats == self.expected_ppg_stats, "PPG stats {} do not match expected PPG stats {} for qos"
            "mapping {}, packet per mapping list {}".format(post_test_stats, self.expected_ppg_stats, mapping, num_pkts))
            print("OK")

    def QosMapAttrSetTest(self):
        print("\\nQosMapAttrSetTest()")
        print("SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST Set Test")
        try:
            print("Setting SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST to {}:{}".format(
                self.test_qos_map_type_name, IngressQosMapUpdatetest.baseTestConfigMappings))
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [k for k, v in self.baseTestConfigMappings], [v for k, v in self.baseConfigQosMappings])
            self.verifyQosMapping(IngressQosMapUpdatetest.baseTestConfigMappings)
            self.trafficQosMappingTest(IngressQosMapUpdatetest.baseTestConfigMappings)

            print("Setting SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST to {}:[]".format(
                self.test_qos_map_type_name))
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [], [])
            self.verifyQosMapping()
            self.trafficQosMappingTest()
        finally:
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [], [])
            self.verifyQosMapping()

    def QosMapAttrUpdateTest1(self):
        print ("\nQosMapAttrUpdateTest1()")
        print("SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST Update Test")
        print("Update from {0}:{1} => {0}:{1}".format(self.test_qos_map_type_name, IngressQosMapUpdatetest.baseTestConfigMappings))
        try:
            print("Setting SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST to {}:{}".format(
                self.test_qos_map_type_name, IngressQosMapUpdatetest.baseTestConfigMappings))
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [k for k, v in self.baseTestConfigMappings], [v for k, v in self.baseConfigQosMappings])
            self.verifyQosMapping(IngressQosMapUpdatetest.baseTestConfigMappings)
            self.trafficQosMappingTest(IngressQosMapUpdatetest.baseTestConfigMappings)

            print("Setting SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST to {}:{}".format(
                self.test_qos_map_type_name, IngressQosMapUpdatetest.baseTestConfigMappings))
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [k for k, v in self.baseTestConfigMappings], [v for k, v in self.baseConfigQosMappings])
            self.verifyQosMapping(IngressQosMapUpdatetest.baseTestConfigMappings)
            self.trafficQosMappingTest(IngressQosMapUpdatetest.baseTestConfigMappings)
        finally:
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [], [])
            self.verifyQosMapping()

    def QosMapAttrUpdateTest(self,mapping):
        print("\nUpdate from {0}:{1} to {0}:{2}".format(self.test_qos_map_type_name, IngressQosMapUpdatetest.baseTestConfigMappings,mapping))
        try:
            print("Setting SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST to {}:{}".format(
                self.test_qos_map_type_name, IngressQosMapUpdatetest.baseTestConfigMappings))
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [k for k, v in self.baseTestConfigMappings], [v for k, v in self.baseConfigQosMappings])
            self.verifyQosMapping(IngressQosMapUpdatetest.baseTestConfigMappings)
            #Commented to speed up test, actual stats verification anyways needs to happen after the update below
            #self.trafficQosMappingTest(IngressQosMapUpdatetest.baseTestConfigMappings)
            print("OK")

            print("Setting SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST to {}:{}".format(self.test_qos_map_type_name, mapping))
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [k for k, v in mapping], [v for k, v in mapping])
            self.verifyQosMapping(mapping)
            self.trafficQosMappingTest(mapping)
        finally:
            sai_thrift_set_qos_map_attribute(self.client, self.test_ingress_qos_map_handle,
                    self.test_qos_map_type, [], [])
            self.verifyQosMapping()

    def QosMapAttrUpdateTest2(self):
        print ("\nQosMapAttrUpdateTest2()")
        print("SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST Update Test")
        for mapping in IngressQosMapUpdatetest.testConfigMappings:
            self.QosMapAttrUpdateTest(mapping)

    def IngressQosMapUpdateTests(self):
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)==0):
            return
        try:
            self.QosMapAttrSetTest()
            self.QosMapAttrUpdateTest1()
            self.QosMapAttrUpdateTest2()
        finally:
            pass

@group('qos')
@group('bfn')
class IcosPpgQosMapIngressUpdateTest(IngressQosMapUpdatetest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP)

    def verifyQosMapping(self, mapping=[]):
        try:
            print ("Verifying ppg mapping {}".format(mapping))
            # There is no method in SAI to verify that ppg exists in hardware, so we skip that check here and just reset the stats for ppgs that no longer exist
            mapped_ppgs = set()
            for icos, ppg in mapping:
                mapped_ppgs.add(ppg)
            #Reset ppg stats for the ppgs that no longer exist
            for ppg_index in range(len(self.expected_ppg_stats)):
                if ppg_index in mapped_ppgs:
                    continue
                else:
                    self.expected_ppg_stats[ppg_index] = 0
        finally:
            pass

    def runTest(self):
        super(self.__class__, self).IngressQosMapUpdateTests()

@group('qos')
@group('bfn')
class DscpTcQosMapIngressUpdateTest(IngressQosMapUpdatetest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SAI_QOS_MAP_TYPE_DSCP_TO_TC)

    def verifyQosMapping(self, mapping=[]):
        pass

    def runTest(self):
        super(self.__class__, self).IngressQosMapUpdateTests()

@group('qos')
@group('bfn')
class PcpTcQosMapIngressUpdateTest(IngressQosMapUpdatetest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SAI_QOS_MAP_TYPE_DOT1P_TO_TC)

    def verifyQosMapping(self, mapping=[]):
        pass

    def runTest(self):
        super(self.__class__, self).IngressQosMapUpdateTests()

class PortQosMapUpdatetest(sai_base_test.ThriftInterfaceDataPlane):
    baseConfigQosMappings = [
       (0, 0), (1, 1), (2, 2), (3, 3),
       (4, 4), (5, 5), (6, 6), (7, 7)
       ]
    baseTestConfigMappings = [
       (0, 0), (1, 1), (2, 2), (3, 3)
       ]
    testConfigMappings = [
       [(4, 4), (5, 5), (6, 6), (7, 7)],  #No Overlap
       [(0, 4), (1, 5), (2, 6), (3, 7)],  #All keys Exist, but key values different i.e. Full Update
       [(0, 0), (1, 1)],                  #Some keys Exist, Map to Same values i.e. Partial Delete
       [(0, 0), (1, 4)],                  #Some keys Exist, Some keys same value some keys different values i.e Partial Delete + Partial Update
       [(0, 0), (1, 1), (4, 3), (5, 6)],  #Some old keys & some new keys, old keys same value, new keys new values
       [(0, 0), (1, 1), (4, 0), (5, 1)],  #Some old and some new keys, old keys same value. new keys map to samevalues
       [(0, 3), (1, 2), (2, 1), (3, 0)],  #Key Value Pairs reversed
       [(0, 0), (1, 0), (2, 0), (3, 0)],  #All keys remapped to same values
       [(0, 0), (1, 0), (2, 1), (3, 1), (4, 2), (5, 5)],
       [(0, 1), (1, 2), (2, 1), (3, 2)]
       ]

    # Dictionary for qos ingress map types to port qos attrs
    qos_helper_dict = {
        SAI_QOS_MAP_TYPE_DOT1P_TO_TC: { 'port_attr': SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP, 'port_attr_name': 'qos_dot1p_to_tc_map', 'qos_map_name':'dot1p_to_tc'},
        SAI_QOS_MAP_TYPE_DSCP_TO_TC : { 'port_attr': SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, 'port_attr_name': 'qos_dscp_to_tc_map', 'qos_map_name': 'dscp_to_tc'},
        SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP: { 'port_attr':SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP, 'port_attr_name': 'qos_tc_to_priority_group_map', 'qos_map_name': 'tc_to_priority_group'},
        SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP: { 'port_attr': SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP, 'port_attr_name': 'qos_pfc_priority_to_priority_group_map', 'qos_map_name': 'pfc_priority_to_priority_group'}
    }

    def create_qos_test_config(self, mappings):
        return sai_thrift_create_qos_map(self.client, self.test_qos_map_type, [key for key, value in
            mappings], [value for key, value in mappings])

    def setUp(self, test_qos_map_type):
        super(PortQosMapUpdatetest, self).setUp()
        switch_init(self.client)
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)==0):
            print("Ingress QoS Map feature not enabled, skipping test")
            return
        self.port_test_attr = PortQosMapUpdatetest.qos_helper_dict[test_qos_map_type]['port_attr']
        self.port_test_attr_name = PortQosMapUpdatetest.qos_helper_dict[test_qos_map_type]['port_attr_name']
	self.test_qos_map_type = test_qos_map_type
	self.test_qos_map_type_name = PortQosMapUpdatetest.qos_helper_dict[test_qos_map_type]['qos_map_name']
        self.ppg_handles = []
        self.expected_ppg_stats = [0]*8
        test_l3_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='40.40.1.20',
            ip_id=105,
            ip_dscp=0,
            ip_ttl=64)
        test_l2_pkt = simple_arp_packet(pktlen=60, vlan_vid=100, vlan_pcp=0)
        if self.test_qos_map_type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC:
            self.test_pkt = test_l2_pkt
        else:
            self.test_pkt = test_l3_pkt
        self.test_port = port_list[0]
        # Create ppgs for test port
        for ppg_index in range(8):
            self.ppg_handles.append(self.client.sai_thrift_create_ppg(self.test_port, ppg_index))

        #Setup base forwarding Config
        self.vlan100 = sai_thrift_create_vlan(self.client, 100)
        self.vlan_members = []
        self.vlan_members.append(sai_thrift_create_vlan_member(self.client, self.vlan100, port_list[0],
            SAI_VLAN_TAGGING_MODE_UNTAGGED))
        self.vlan_members.append(sai_thrift_create_vlan_member(self.client, self.vlan100, port_list[1],
            SAI_VLAN_TAGGING_MODE_UNTAGGED
            ))
        attr_value = sai_thrift_attribute_value_t(u16=100)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port_list[0], attr)
        self.client.sai_thrift_set_port_attribute(port_list[1], attr)

        #Setup Base QoS Config
        self.base_config_qos_maps = []
        if self.port_test_attr != SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP and self.port_test_attr != SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP:
            self.base_config_qos_maps.append(sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_DSCP_TO_TC, [dscp for dscp, tc in IngressQosMapUpdatetest.baseConfigQosMappings], [tc for dscp, tc in IngressQosMapUpdatetest.baseConfigQosMappings]))
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP,
                    self.base_config_qos_maps[-1])
        if self.port_test_attr != SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP:
            self.base_config_qos_maps.append(sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP, [tc for tc, pfc_prio in IngressQosMapUpdatetest.baseConfigQosMappings], [pfc_prio for tc, pfc_prio in IngressQosMapUpdatetest.baseConfigQosMappings]))
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP,
                    self.base_config_qos_maps[-1])
        if self.port_test_attr != SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP:
           self.base_config_qos_maps.append(sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP, [pfc_prio for pfc_prio, ppg in IngressQosMapUpdatetest.baseConfigQosMappings], [ppg for pfc_prio, ppg in IngressQosMapUpdatetest.baseConfigQosMappings]))
           sai_thrift_set_port_attribute(self.client, self.test_port,
                    SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP,
                    self.base_config_qos_maps[-1])

        #Setup Base Test Config
        self.test_ingress_qos_map_handle = self.create_qos_test_config(PortQosMapUpdatetest.baseTestConfigMappings)

    def tearDown(self):
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)!=0):
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, self.test_port, SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP, SAI_NULL_OBJECT_ID)
            self.verifyQosMapping()
            sai_thrift_remove_qos_map(self.client, self.test_ingress_qos_map_handle)
            for qos_map in self.base_config_qos_maps:
                sai_thrift_remove_qos_map(self.client, qos_map)
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan100)
            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port_list[0], attr)
            self.client.sai_thrift_set_port_attribute(port_list[1], attr)
            for vlan_member in self.vlan_members:
                self.client.sai_thrift_remove_vlan_member(vlan_member)
            self.client.sai_thrift_remove_vlan(self.vlan100)
            for ppg in self.ppg_handles:
                self.client.sai_thrift_remove_ppg(ppg)
        super(PortQosMapUpdatetest, self).tearDown()

    def verifyQosMapping(self, mapping=[]):
        print "QoS Mapping Verification unimplemented"

    def trafficQosMappingTest(self, mapping=[]):
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_PPG_STATS) == 0):
            print("PPG stats is not enabled, return")
            return

        if mapping:
            pkt_count=1
            exp_stats = [0]*len(self.ppg_handles)
            post_test_stats = [0]*len(self.ppg_handles)
            num_pkts = []
            for key, v in mapping:
                if self.test_qos_map_type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC:
                    self.test_pkt[Ether][Dot1Q].prio = key
                else:
                    self.test_pkt[IP].tos = key << 2
                for count in range(pkt_count):
                    send_packet(self, switch_ports[0], str(self.test_pkt))
                    self.expected_ppg_stats[v]+=1
                num_pkts.append(pkt_count)
                pkt_count+=1
            print("Waiting for 4 sec... before collecting stats")
            time.sleep(4)
            for ppg_index in range(len(self.ppg_handles)):
                counters = sai_thrift_read_ppg_counters(self.client, self.ppg_handles[ppg_index])
                post_test_stats[ppg_index] = counters[0]
            print("Verifying PPG stats for qos mapping:{}, packet per mapping:{}".format(mapping, num_pkts))
            self.assertTrue(post_test_stats == self.expected_ppg_stats, "PPG stats {} do not match expected PPG stats {} for qos"
            "mapping {}, packet per mapping list {}".format(post_test_stats, self.expected_ppg_stats, mapping, num_pkts))
            print("OK")

    def QosMapAttrSetTest(self):
        print("\\nQosMapAttrSetTest()")
        print("Port Attr {} Set Test".format(self.port_test_attr_name))
        try:
            print("Setting Port Attr {} to {}:{}".format(self.port_test_attr_name,
                self.port_test_attr_name, PortQosMapUpdatetest.baseTestConfigMappings))
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    self.test_ingress_qos_map_handle)
            self.verifyQosMapping(IngressQosMapUpdatetest.baseTestConfigMappings)
            self.trafficQosMappingTest(IngressQosMapUpdatetest.baseTestConfigMappings)

            print("Setting Port Attr {} to {}:[]".format(self.port_test_attr_name,
                self.test_qos_map_type_name))
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    SAI_NULL_OBJECT_ID)
            self.verifyQosMapping()
            self.trafficQosMappingTest()
        finally:
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    SAI_NULL_OBJECT_ID)
            self.verifyQosMapping()

    def QosMapAttrUpdateTest1(self):
        print ("\nQosMapAttrUpdateTest1()")
        print("Port Attr {} Update Test".format(self.port_test_attr_name))
        print("Update from {0}:{1} => {0}:{1}".format(self.test_qos_map_type_name, PortQosMapUpdatetest.baseTestConfigMappings))
        try:
            print("Setting Port Attr {} to {}:{}".format(self.port_test_attr_name,
                self.test_qos_map_type_name, PortQosMapUpdatetest.baseTestConfigMappings))
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    self.test_ingress_qos_map_handle)
            self.verifyQosMapping(IngressQosMapUpdatetest.baseTestConfigMappings)
            self.trafficQosMappingTest(IngressQosMapUpdatetest.baseTestConfigMappings)

            print("Setting Port Attr {} to {}:{}".format(self.port_test_attr_name,
                self.test_qos_map_type_name, PortQosMapUpdatetest.baseTestConfigMappings))
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    self.test_ingress_qos_map_handle)
            self.verifyQosMapping(PortQosMapUpdatetest.baseTestConfigMappings)
            self.trafficQosMappingTest(PortQosMapUpdatetest.baseTestConfigMappings)
        finally:
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    SAI_NULL_OBJECT_ID)
            self.verifyQosMapping()

    def QosMapAttrUpdateTest(self,mapping):
        print("\nUpdate from {0}:{1} to {0}:{2}".format(self.test_qos_map_type_name, PortQosMapUpdatetest.baseTestConfigMappings,mapping))
        try:
            update_qos_config = sai_thrift_create_qos_map(self.client, self.test_qos_map_type, [key for key, value in
                mapping], [value for key, value in mapping])
            print("Setting Port Attr {} to {}:{}".format(self.port_test_attr_name,
                self.test_qos_map_type_name, PortQosMapUpdatetest.baseTestConfigMappings))
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    self.test_ingress_qos_map_handle)
            self.verifyQosMapping(IngressQosMapUpdatetest.baseTestConfigMappings)
            #Commented to speed up test, actual stats verification anyways needs to happen after the update below
            #self.trafficQosMappingTest(IngressQosMapUpdatetest.baseTestConfigMappings)
            print("OK")

            print("Setting Port Attr {} to {}:{}".format(self.port_test_attr_name, self.test_qos_map_type_name, mapping))
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    update_qos_config)
            self.verifyQosMapping(mapping)
            self.trafficQosMappingTest(mapping)
        finally:
            sai_thrift_set_port_attribute(self.client, self.test_port, self.port_test_attr,
                    SAI_NULL_OBJECT_ID)
            sai_thrift_remove_qos_map(self.client, update_qos_config)
            self.verifyQosMapping()

    def QosMapAttrUpdateTest2(self):
        print ("\nQosMapAttrUpdateTest2()")
        print("Port Attr {} Update Test".format(self.port_test_attr_name))
        for mapping in PortQosMapUpdatetest.testConfigMappings:
            self.QosMapAttrUpdateTest(mapping)
    def runPortQosMapUpdateTests(self):
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)==0):
            return
        try:
            self.QosMapAttrSetTest()
            self.QosMapAttrUpdateTest1()
            self.QosMapAttrUpdateTest2()
        finally:
            pass

@group('qos')
@group('bfn')
class PortIcosPpgQosMapUpdateTest(PortQosMapUpdatetest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP)

    def verifyQosMapping(self, mapping=[]):
        try:
            print ("Verifying ppg mapping {}".format(mapping))
            # There is no method in SAI to verify that ppg exists in hardware, so we skip that check here and just reset the stats for ppgs that no longer exist
            mapped_ppgs = set()
            for icos, ppg in mapping:
                mapped_ppgs.add(ppg)
            #Reset ppg stats for the ppgs that no longer exist
            for ppg_index in range(len(self.expected_ppg_stats)):
                if ppg_index in mapped_ppgs:
                    continue
                else:
                    self.expected_ppg_stats[ppg_index] = 0
        finally:
            pass

    def runTest(self):
        super(self.__class__, self).runPortQosMapUpdateTests()

@group('qos')
@group('bfn')
class PortDscpTcQosMapUpdateTest(PortQosMapUpdatetest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SAI_QOS_MAP_TYPE_DSCP_TO_TC)

    def verifyQosMapping(self, mapping=[]):
        pass

    def runTest(self):
        super(self.__class__, self).runPortQosMapUpdateTests()

@group('qos')
@group('bfn')
class PortAttrPcpTcQosMapUpdateTest(PortQosMapUpdatetest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SAI_QOS_MAP_TYPE_DOT1P_TO_TC)

    def verifyQosMapping(self, mapping=[]):
        pass

    def runTest(self):
        super(self.__class__, self).runPortQosMapUpdateTests()
