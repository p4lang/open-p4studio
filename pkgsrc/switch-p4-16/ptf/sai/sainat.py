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
Thrift SAI interface NAT tests
"""

import socket
import sys
from struct import pack, unpack

from switch_utils import *

import sai_base_test
from ptf.mask import Mask
from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *
from switchsai_thrift.sai_headers import  *
from switch_utils import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from bf_switcht_api_thrift.model_headers import *

@group('nat')
class NatTest(sai_base_test.ThriftInterfaceDataPlane):
  def runTest(self):
    try:
      if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_NAT) == 0):
        print "NAT feature not enabled, skipping"
        return
      switch_init(self.client)
      self.RouteConfigure()
      self.TrapConfigure()
      self.NatConfigure()
      self.SourceNatTest()
      self.DestNatTest()
    finally:
      self.RouteConfigureClean()
      self.NatCleanup()

  def TrapConfigure(self):
    self.trap_group = sai_thrift_create_hostif_trap_group(self.client, queue_id=4)

    self.dnat_trap = sai_thrift_create_hostif_trap(client=self.client,
                                         trap_type=SAI_HOSTIF_TRAP_TYPE_DNAT_MISS,
                                         packet_action=SAI_PACKET_ACTION_TRAP,
                                         trap_group=self.trap_group)
    self.snat_trap = sai_thrift_create_hostif_trap(client=self.client,
                                         trap_type=SAI_HOSTIF_TRAP_TYPE_SNAT_MISS,
                                         packet_action=SAI_PACKET_ACTION_TRAP,
                                         trap_group=self.trap_group)
    pkt = simple_tcp_packet(eth_dst=router_mac,
                            eth_src='00:22:22:22:22:22',
                            ip_dst='10.10.10.1',
                            ip_src='192.168.0.1',
                            ip_id=105,
                            ip_ttl=64)
    pkt1 = pkt.copy()
    cpu_nat_pkt = simple_cpu_packet(
        ingress_port=switch_ports[0],
        ingress_ifindex=0,
        ingress_bd=0,
        packet_type=0,
        reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_SNAT_MISS,
        inner_pkt=pkt1)
    self.cpu_port = 64
    send_packet(self, switch_ports[0], str(pkt))
    cpu_nat_pkt = cpu_packet_mask_ingress_bd_and_ifindex(cpu_nat_pkt)
    verify_packets(self, cpu_nat_pkt, [self.cpu_port])

  def RouteConfigure(self):
      self.server_port = port_list[0]
      self.wan_port = port_list[1]
      v4_enabled = 1
      mac = ''
      v6_enabled = 1
      self.vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
      self.server_rif = sai_thrift_create_router_interface(self.client, self.vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, self.server_port, 0, v4_enabled, v6_enabled, mac,nat_zone=0)
      self.wan_rif = sai_thrift_create_router_interface(self.client, self.vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, self.wan_port, 0, v4_enabled, v6_enabled, mac,nat_zone=1)
      self.addr_family = SAI_IP_ADDR_FAMILY_IPV4
      self.ip_addr1 = '10.10.10.0'
      self.ip_mask1 = '255.255.255.0'
      self.dmac1 = '00:11:22:33:44:55'
      self.nhop_ip1 = '20.20.20.1'
      self.nhop_ip1_subnet = '20.20.20.0'
      self.ip_mask2 = '255.255.255.0'

      sai_thrift_create_neighbor(self.client, self.addr_family, self.wan_rif, self.nhop_ip1, self.dmac1)
      self.nhop1 = sai_thrift_create_nhop(self.client, self.addr_family, self.nhop_ip1, self.wan_rif)
      #10.10.10.0/24 --> NHOP1(WAN)
      sai_thrift_create_route(self.client, self.vr_id, self.addr_family, self.ip_addr1, self.ip_mask1, self.nhop1)
      sai_thrift_create_route(self.client, self.vr_id, self.addr_family, self.nhop_ip1_subnet, self.ip_mask2, self.wan_rif)

      #192.168.0.1/24 --> NHOP(SERVER)
      self.server_ip = '192.168.0.1'
      self.server_ip_mask = '255.255.255.0'
      self.server_dmac = '00:33:44:55:66:77'
      sai_thrift_create_neighbor(self.client, self.addr_family, self.server_rif, self.server_ip, self.server_dmac)
      self.server_nhop = sai_thrift_create_nhop(self.client, self.addr_family, self.server_ip, self.server_rif)
      sai_thrift_create_route(self.client, self.vr_id, self.addr_family, self.server_ip, self.server_ip_mask, self.server_nhop)

      # send the test packet(s)
      pkt = simple_tcp_packet(eth_dst=router_mac,
                              eth_src='00:22:22:22:22:22',
                              ip_dst='10.10.10.1',
                              ip_src='192.168.0.1',
                              ip_id=105,
                              ip_ttl=64)
      exp_pkt = simple_tcp_packet(
                              eth_dst='00:11:22:33:44:55',
                              eth_src=router_mac,
                              ip_dst='10.10.10.1',
                              ip_src='192.168.0.1',
                              ip_id=105,
                              ip_ttl=63)
      send_packet(self, switch_ports[0], str(pkt))
      verify_packets(self, exp_pkt, [switch_ports[1]])
      pkt = simple_tcp_packet(eth_dst=router_mac,
                              eth_src='00:22:22:22:22:22',
                              ip_dst='20.20.20.1',
                              ip_src='192.168.0.1',
                              ip_id=105,
                              ip_ttl=64)
      exp_pkt = simple_tcp_packet(
                              eth_dst='00:11:22:33:44:55',
                              eth_src=router_mac,
                              ip_dst='20.20.20.1',
                              ip_src='192.168.0.1',
                              ip_id=105,
                              ip_ttl=63)
      send_packet(self, switch_ports[0], str(pkt))
      verify_packets(self, exp_pkt, [switch_ports[1]])

      pkt = simple_tcp_packet(eth_dst=router_mac,
                              eth_src='00:22:22:22:22:22',
                              ip_dst='192.168.0.1',
                              ip_src='20.20.20.1',
                              ip_id=105,
                              ip_ttl=64)
      exp_pkt = simple_tcp_packet(
                              eth_dst='00:33:44:55:66:77',
                              eth_src=router_mac,
                              ip_src='20.20.20.1',
                              ip_dst='192.168.0.1',
                              ip_id=105,
                              ip_ttl=63)
      send_packet(self, switch_ports[1], str(pkt))
      verify_packets(self, exp_pkt, [switch_ports[0]])

  def RouteConfigureClean(self):
    sai_thrift_remove_route(self.client, self.vr_id, self.addr_family, self.ip_addr1, self.ip_mask1, self.nhop1)
    sai_thrift_remove_route(self.client, self.vr_id, self.addr_family, self.nhop_ip1_subnet, self.ip_mask2, self.wan_rif)
    self.client.sai_thrift_remove_next_hop(self.nhop1)
    sai_thrift_remove_neighbor(self.client, self.addr_family, self.wan_rif, self.nhop_ip1, self.dmac1)

    sai_thrift_remove_route(self.client, self.vr_id, self.addr_family, self.server_ip, self.server_ip_mask, self.server_rif)
    sai_thrift_remove_neighbor(self.client, self.addr_family, self.server_rif, self.server_ip, self.server_dmac)
    self.client.sai_thrift_remove_next_hop(self.server_nhop)

    self.client.sai_thrift_remove_router_interface(self.wan_rif)
    self.client.sai_thrift_remove_router_interface(self.server_rif)

    self.client.sai_thrift_remove_virtual_router(self.vr_id)

  def NatConfigure(self):
    self.null_ip_key = '0.0.0.0'
    self.null_port = 32765
    #Server IP - 192.168.0.1
    #Translated server IP - 20.20.20.1
    #WAN IP - 10.10.10.1
    #self.server_ip = '192.168.0.1'
    self.translate_server_ip = '200.200.200.1'

    self.l4_src_port = 100
    self.l4_dst_port = 1234
    self.proto = 6

    self.translate_sport = 500
    self.translate_dport = 2000

    self.nat_type = SAI_NAT_TYPE_DESTINATION_NAT
    sai_thrift_create_nat_entry(self.client, self.nat_type, self.null_ip_key, self.translate_server_ip, self.proto, 0, self.l4_dst_port,
                                0, self.server_ip, 0, self.translate_dport)

    self.nat_type = SAI_NAT_TYPE_DESTINATION_NAT
    sai_thrift_create_nat_entry(self.client, self.nat_type, self.null_ip_key, self.translate_server_ip, 0, 0, 0,
                                0, self.server_ip, 0, 0)

    self.nat_type = SAI_NAT_TYPE_DESTINATION_NAT_POOL
    sai_thrift_create_nat_entry(self.client, self.nat_type, self.null_ip_key, self.translate_server_ip, 0, 0, 0,
                                0, self.server_ip, 0, 0)

    self.nat_type = SAI_NAT_TYPE_SOURCE_NAT
    sai_thrift_create_nat_entry(self.client, self.nat_type, self.server_ip, self.null_ip_key, self.proto, self.l4_src_port, 0,
                                self.translate_server_ip, 0, self.translate_sport, 0)

    self.nat_type = SAI_NAT_TYPE_SOURCE_NAT
    sai_thrift_create_nat_entry(self.client, self.nat_type, self.server_ip, self.null_ip_key, 0, 0, 0,
                                self.translate_server_ip, 0, 0, 0)

  def SourceNatTest(self):
    try:
        #Validate server to WAN
        #Translate server-ip to public ip
        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_id=105,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(
                                eth_dst='00:11:22:33:44:55',
                                eth_src=router_mac,
                                ip_dst='10.10.10.1',
                                ip_src='200.200.200.1',
                                ip_id=105,
                                ip_ttl=63)
        send_packet(self, switch_ports[0], str(pkt))
        verify_packets(self, exp_pkt, [switch_ports[1]])

        #Translate server-ip + port to public ip + port
        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ip_dst='10.10.10.1',
                                ip_src='192.168.0.1',
                                ip_id=105,
                                ip_ttl=64,tcp_sport=100)
        exp_pkt = simple_tcp_packet(
                                eth_dst='00:11:22:33:44:55',
                                eth_src=router_mac,
                                ip_dst='10.10.10.1',
                                ip_src='200.200.200.1',
                                ip_id=105,
                                ip_ttl=63,tcp_sport=500)
        send_packet(self, switch_ports[0], str(pkt))
        verify_packets(self, exp_pkt, [switch_ports[1]])
        ret_attr = sai_thrift_get_nat_entry(self.client, SAI_NAT_TYPE_SOURCE_NAT, self.server_ip, self.null_ip_key, self.proto, self.l4_src_port, 0, SAI_NAT_ENTRY_ATTR_PACKET_COUNT)
        self.assertEqual(ret_attr.value.u64, 1)
        print "Stats %d" %(ret_attr.value.u64)
        ret_attr = sai_thrift_get_nat_entry(self.client, SAI_NAT_TYPE_SOURCE_NAT, self.server_ip, self.null_ip_key, self.proto, self.l4_src_port, 0, SAI_NAT_ENTRY_ATTR_HIT_BIT)
        print "1st query - Hit bit %d" %(ret_attr.value.booldata)
        ret_attr = sai_thrift_get_nat_entry(self.client, SAI_NAT_TYPE_SOURCE_NAT, self.server_ip, self.null_ip_key, self.proto, self.l4_src_port, 0, SAI_NAT_ENTRY_ATTR_HIT_BIT)
        print "2nd query - Hit bit %d" %(ret_attr.value.booldata)
        ret_attr = sai_thrift_set_nat_entry(self.client, SAI_NAT_TYPE_SOURCE_NAT, self.server_ip, self.null_ip_key, self.proto, self.l4_src_port, 0, SAI_NAT_ENTRY_ATTR_PACKET_COUNT)
        ret_attr = sai_thrift_get_nat_entry(self.client, SAI_NAT_TYPE_SOURCE_NAT, self.server_ip, self.null_ip_key, self.proto, self.l4_src_port, 0, SAI_NAT_ENTRY_ATTR_PACKET_COUNT)
        self.assertEqual(ret_attr.value.u64, 0)
    except:
        print "DNAT Test Failed"

  def DestNatTest(self):
    try:
        #Translate public-ip to server-ip
        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ip_dst='200.200.200.1',
                                ip_src='10.10.10.1',
                                ip_id=105,
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(
                                eth_dst='00:33:44:55:66:77',
                                eth_src=router_mac,
                                ip_dst='192.168.0.1',
                                ip_src='10.10.10.1',
                                ip_id=105,
                                ip_ttl=63)
        send_packet(self, switch_ports[1], str(pkt))
        verify_packets(self, exp_pkt, [switch_ports[0]])

        #Translate public-ip + port to server-ip + port
        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ip_dst='200.200.200.1',
                                ip_src='10.10.10.1',
                                ip_id=105,
                                ip_ttl=64,tcp_dport=1234)
        exp_pkt = simple_tcp_packet(
                                eth_dst='00:33:44:55:66:77',
                                eth_src=router_mac,
                                ip_dst='192.168.0.1',
                                ip_src='10.10.10.1',
                                ip_id=105,
                                ip_ttl=63,tcp_dport=2000)
        send_packet(self, switch_ports[1], str(pkt))
        verify_packets(self, exp_pkt, [switch_ports[0]])
        ret_attr = sai_thrift_get_nat_entry(self.client, SAI_NAT_TYPE_DESTINATION_NAT, self.null_ip_key, self.translate_server_ip, self.proto, 0, self.l4_dst_port, SAI_NAT_ENTRY_ATTR_PACKET_COUNT)
        self.assertEqual(ret_attr.value.u64, 1)
    except:
        print "SNAT Test Failed"

  def NatCleanup(self):
    sai_thrift_delete_nat_entry(self.client, SAI_NAT_TYPE_DESTINATION_NAT, self.null_ip_key, self.translate_server_ip, self.proto, 0, self.l4_dst_port)
    sai_thrift_delete_nat_entry(self.client, SAI_NAT_TYPE_DESTINATION_NAT, self.null_ip_key, self.translate_server_ip, 0, 0, 0)
    sai_thrift_delete_nat_entry(self.client, SAI_NAT_TYPE_DESTINATION_NAT_POOL, self.null_ip_key, self.translate_server_ip, 0, 0, 0)
    sai_thrift_delete_nat_entry(self.client, SAI_NAT_TYPE_SOURCE_NAT, self.server_ip, self.null_ip_key, self.proto, self.l4_src_port, 0)
    sai_thrift_delete_nat_entry(self.client, SAI_NAT_TYPE_SOURCE_NAT, self.server_ip, self.null_ip_key, 0, 0, 0)
    sai_thrift_remove_hostif_trap(self.client, self.dnat_trap)
    sai_thrift_remove_hostif_trap(self.client, self.snat_trap)
    sai_thrift_remove_hostif_trap_group(self.client, self.trap_group)

