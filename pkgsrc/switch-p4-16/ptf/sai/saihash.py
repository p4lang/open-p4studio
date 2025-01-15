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
Thrift SAI interface Hash Object tests
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

@group('hash')
class LagSeedTest(sai_base_test.ThriftInterfaceDataPlane):
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

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid,
                                    lag_id1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid,
                                      port4, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_lag_attribute(lag_id1, attr)

        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port4, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, lag_id1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port4, mac_action)

        try:
            max_itrs = 120
            def PacketTest():
                count = [0, 0, 0]
                dst_ip = int(socket.inet_aton('10.10.10.1').encode('hex'),16)
                src_ip = int(socket.inet_aton('11.11.11.1').encode('hex'),16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntoa(hex(dst_ip)[2:].zfill(8).decode('hex'))
                    src_ip_addr = socket.inet_ntoa(hex(dst_ip)[2:].zfill(8).decode('hex'))
                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_dst=dst_ip_addr,
                                            ip_src=src_ip_addr,
                                            ip_id=109,
                                            ip_ttl=64)

                    send_packet(self, switch_ports[3], str(pkt))
                    ports_verify = switch_ports[0:3]
                    rcv_idx = verify_any_packet_any_port(self, [pkt],
                                               ports_verify, timeout = 5)
                    count[rcv_idx] += 1
                    dst_ip += 1

                return count
            attr_value = sai_thrift_attribute_value_t(u32=200)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_LAG_DEFAULT_HASH_SEED,
                                                                   value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            atrr = self.client.sai_thrift_get_switch_attribute_by_id(SAI_SWITCH_ATTR_LAG_DEFAULT_HASH_SEED)
            self.assertEqual(attr.value.u32, 200)
            count = PacketTest()
            print count
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.8)),
                        "Not all paths are equally balanced")
            attr_value = sai_thrift_attribute_value_t(u32=400)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_LAG_DEFAULT_HASH_SEED,
                                                                   value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            atrr = self.client.sai_thrift_get_switch_attribute_by_id(SAI_SWITCH_ATTR_LAG_DEFAULT_HASH_SEED)
            self.assertEqual(attr.value.u32, 400)
            count = PacketTest()
            print count
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.8)),
                        "Not all paths are equally balanced")
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

@group('hash')
class EcmpSeedTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending packet port 3 -> port [0,1,2] (192.168.0.1 -> 10.10.10.1 [id = 101])"
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '10.10.0.0'
        ip_mask1 = '255.255.0.0'
        ip_mask2 = '255.255.255.0'
        nhop_ip1 = '11.11.11.11'
        nhop_ip1_subnet = '11.11.11.0'
        nhop_ip2 = '22.22.22.22'
        nhop_ip2_subnet = '22.22.22.0'
        nhop_ip3 = '33.33.33.33'
        nhop_ip3_subnet = '33.33.33.0'
        dmac1 = '00:11:22:33:44:55'
        dmac2 = '00:11:22:33:44:56'
        dmac3 = '00:11:22:33:44:57'

        vr1 = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)

        rif1 = sai_thrift_create_router_interface(self.client, vr1,
               SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif2 = sai_thrift_create_router_interface(self.client, vr1,
               SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)
        rif3 = sai_thrift_create_router_interface(self.client, vr1,
               SAI_ROUTER_INTERFACE_TYPE_PORT, port3, 0, v4_enabled, v6_enabled, mac)
        rif4 = sai_thrift_create_router_interface(self.client, vr1,
               SAI_ROUTER_INTERFACE_TYPE_PORT, port4, 0, v4_enabled, v6_enabled, mac)

        sai_thrift_create_neighbor(self.client, addr_family, rif1, nhop_ip1, dmac1)
        sai_thrift_create_neighbor(self.client, addr_family, rif2, nhop_ip2, dmac2)
        sai_thrift_create_neighbor(self.client, addr_family, rif3, nhop_ip3, dmac3)

        nhop1 = sai_thrift_create_nhop(self.client, addr_family, nhop_ip1, rif1)
        nhop2 = sai_thrift_create_nhop(self.client, addr_family, nhop_ip2, rif2)
        nhop3 = sai_thrift_create_nhop(self.client, addr_family, nhop_ip3, rif3)

        nhop_group1 = sai_thrift_create_next_hop_group(self.client)

        nhop_gmember1 = sai_thrift_create_next_hop_group_member(self.client, nhop_group1, nhop1)
        nhop_gmember2 = sai_thrift_create_next_hop_group_member(self.client, nhop_group1, nhop2)
        nhop_gmember3 = sai_thrift_create_next_hop_group_member(self.client, nhop_group1, nhop3)

        sai_thrift_create_route(self.client, vr1, addr_family, ip_addr1, ip_mask1, nhop_group1)

        # send the test packet(s)
        try:
            max_itrs = 120
            def PacketTest():
                count = [0, 0, 0]
                dst_ip = int(socket.inet_aton('10.10.10.1').encode('hex'),16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntoa(hex(dst_ip)[2:].zfill(8).decode('hex'))
                    pkt = simple_tcp_packet(eth_dst=router_mac,
                                            eth_src='00:22:22:22:22:22',
                                            ip_dst=dst_ip_addr,
                                            ip_src='192.168.8.1',
                                            ip_id=106,
                                            ip_ttl=64)
                    exp_pkt1 = simple_tcp_packet(eth_dst='00:11:22:33:44:55',
                                             eth_src=router_mac,
                                             ip_dst=dst_ip_addr,
                                             ip_src='192.168.8.1',
                                             ip_id=106,
                                             ip_ttl=63)
                    exp_pkt2 = simple_tcp_packet(eth_dst='00:11:22:33:44:56',
                                             eth_src=router_mac,
                                             ip_dst=dst_ip_addr,
                                             ip_src='192.168.8.1',
                                             ip_id=106,
                                             ip_ttl=63)
                    exp_pkt3 = simple_tcp_packet(eth_dst='00:11:22:33:44:57',
                                             eth_src=router_mac,
                                             ip_dst=dst_ip_addr,
                                             ip_src='192.168.8.1',
                                             ip_id=106,
                                             ip_ttl=63)

                    send_packet(self, switch_ports[3], str(pkt))
                    rcv_idx = verify_any_packet_any_port(self,
                                                     [exp_pkt1, exp_pkt2, exp_pkt3],
                                                     [switch_ports[0], switch_ports[1],switch_ports[2]])
                    count[rcv_idx] += 1
                    dst_ip += 1
                return count

            attr_value = sai_thrift_attribute_value_t(u32=200)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_ECMP_DEFAULT_HASH_SEED,
                                                                    value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            atrr = self.client.sai_thrift_get_switch_attribute_by_id(SAI_SWITCH_ATTR_ECMP_DEFAULT_HASH_SEED)
            self.assertEqual(attr.value.u32, 200)
            count = PacketTest()
            print count
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.8)),
                                "Not all paths are equally balanced, %s" % count)
            attr_value = sai_thrift_attribute_value_t(u32=400)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_ECMP_DEFAULT_HASH_SEED,
                                                                    value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            atrr = self.client.sai_thrift_get_switch_attribute_by_id(SAI_SWITCH_ATTR_ECMP_DEFAULT_HASH_SEED)
            self.assertEqual(attr.value.u32, 400)
            count = PacketTest()
            print count
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.8)),
                                "Not all paths are equally balanced, %s" % count)
        finally:
            sai_thrift_remove_route(self.client, vr1, addr_family,
                                  ip_addr1, ip_mask1, nhop_group1)

            self.client.sai_thrift_remove_next_hop_group_member(nhop_gmember1)
            self.client.sai_thrift_remove_next_hop_group_member(nhop_gmember2)
            self.client.sai_thrift_remove_next_hop_group_member(nhop_gmember3)

            self.client.sai_thrift_remove_next_hop_group(nhop_group1)

            self.client.sai_thrift_remove_next_hop(nhop1)
            self.client.sai_thrift_remove_next_hop(nhop2)
            self.client.sai_thrift_remove_next_hop(nhop3)

            sai_thrift_remove_neighbor(self.client, addr_family,
                                           rif1, nhop_ip1, dmac1)
            sai_thrift_remove_neighbor(self.client, addr_family,
                                           rif2, nhop_ip2, dmac2)
            sai_thrift_remove_neighbor(self.client, addr_family,
                                           rif3, nhop_ip3, dmac3)

            self.client.sai_thrift_remove_router_interface(rif1)
            self.client.sai_thrift_remove_router_interface(rif2)
            self.client.sai_thrift_remove_router_interface(rif3)
            self.client.sai_thrift_remove_router_interface(rif4)
            self.client.sai_thrift_remove_virtual_router(vr1)

class LagHashTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        switch_init(self.client)

        lag_hash_id_atrr = self.client.sai_thrift_get_switch_attribute_by_id(SAI_SWITCH_ATTR_LAG_HASH)
        print ('Reading LAG hash id: ', lag_hash_id_atrr.value.oid)

        #creating the hash_obj
        print ("Creating LAG hash object with all fields..")
        hash_attr_list = [
                          SAI_NATIVE_HASH_FIELD_SRC_MAC,
                          SAI_NATIVE_HASH_FIELD_DST_MAC,
                          SAI_NATIVE_HASH_FIELD_ETHERTYPE,
                          SAI_NATIVE_HASH_FIELD_IN_PORT
                         ]
        hash_id = sai_thrift_create_hash(self.client, hash_attr_list)
        print('Created hash id: ', hash_id)

        #Associating the hash obj to the device
        attr_value = sai_thrift_attribute_value_t(oid=hash_id)
        attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_LAG_HASH, value=attr_value)
        self.client.sai_thrift_set_switch_attribute(attr)

        print('Setting lag hash attribute for the switch')
        lag_hash_id_after_atrr = self.client.sai_thrift_get_switch_attribute_by_id(SAI_SWITCH_ATTR_LAG_HASH)
        print ('Read lag hash id: ', lag_hash_id_after_atrr.value.oid)

        self.assertEqual(lag_hash_id_atrr.value.oid, lag_hash_id_after_atrr.value.oid)
        print('Continueing with initial lag hash id')

        sai_thrift_remove_hash(self.client, hash_id)


@group('hash')
class LagHashIPV4Test(sai_base_test.ThriftInterfaceDataPlane):
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

        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid,
                                    lag_id1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid,
                                       port4, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_lag_attribute(lag_id1, attr)

        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port4, attr)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, lag_id1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_oid, mac2, port4, mac_action)

        #creating the hash_obj
        print ("Setting the LAG hash IPV4 object with all fields..")
        hash_attr_list = [
                          SAI_NATIVE_HASH_FIELD_SRC_IP,
                          SAI_NATIVE_HASH_FIELD_DST_IP,
                          SAI_NATIVE_HASH_FIELD_IP_PROTOCOL,
                          SAI_NATIVE_HASH_FIELD_L4_DST_PORT,
                          SAI_NATIVE_HASH_FIELD_L4_SRC_PORT
                          ]
        print('Config hash_data', hash_attr_list)
        hash_id = sai_thrift_create_hash(self.client, hash_attr_list)

        #Associating the hash obj to the device
        attr_value = sai_thrift_attribute_value_t(oid=hash_id)
        attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_LAG_HASH_IPV4, value=attr_value)
        self.client.sai_thrift_set_switch_attribute(attr)

        print("Getting the attribute list..")
        hash_data = sai_thrift_get_hash_attribute(self.client, hash_id)
        data_val = hash_data.attr_list[0].value.s32list.s32list
        print('Read hash_data: ', data_val)

        try:
            max_itrs = 120
            def PacketTest():
                count = [0, 0, 0]
                dst_ip = int(socket.inet_aton('10.10.10.1').encode('hex'),16)
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
                    rcv_idx = verify_any_packet_any_port(self, [exp_pkt],
                                                 ports_verify, timeout = 5)
                    count[rcv_idx] += 1
                    dst_ip += 1

                return count
            count = PacketTest()
            print ('Traffic distribution [port1, port2, port3]', count)
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.7)),
                        "Not all Lag paths are equally balanced")

            #selecting only source IP for hasing
            print("Selecting source IP for Lag hash calculation..")
            set_attr_list = [SAI_NATIVE_HASH_FIELD_SRC_IP]
            sai_thrift_update_hash_attribute(self.client, hash_id, set_attr_list)

            print("Getting the attribute list..")
            hash_data = sai_thrift_get_hash_attribute(self.client, hash_id)
            data_val = hash_data.attr_list[0].value.s32list.s32list
            print('hash_data: ', data_val)

            count = PacketTest()
            print ('Traffic distribution [port1, port2, port3]', count)
            for i in range(0, 3):
                self.assertTrue((count[i] == max_itrs) or (count[i] == 0),
                        "Not all Lag paths are equally balanced")

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
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID,
                                                        value=attr_value)
            for port in sai_port_list:
                self.client.sai_thrift_set_port_attribute(port, attr)
            self.client.sai_thrift_remove_vlan(vlan_oid)

            #Associating the hash obj to the device
            attr_value = sai_thrift_attribute_value_t(oid=0)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_LAG_HASH_IPV4,
                                                       value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            print("Removing the hash object")
            sai_thrift_remove_hash(self.client, hash_id)

@group('hash')
class EcmpIPV4HashTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Sending packet port 3 -> port [0,1,2] (192.168.0.1 -> 10.10.10.1 [id = 101])"
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '10.10.0.0'
        ip_mask1 = '255.255.0.0'
        ip_mask2 = '255.255.255.0'
        nhop_ip1 = '11.11.11.11'
        nhop_ip1_subnet = '11.11.11.0'
        nhop_ip2 = '22.22.22.22'
        nhop_ip2_subnet = '22.22.22.0'
        nhop_ip3 = '33.33.33.33'
        nhop_ip3_subnet = '33.33.33.0'
        dmac1 = '00:11:22:33:44:55'
        dmac2 = '00:11:22:33:44:56'
        dmac3 = '00:11:22:33:44:57'

        vr1 = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)

        rif1 = sai_thrift_create_router_interface(self.client, vr1, 
               SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif2 = sai_thrift_create_router_interface(self.client, vr1,
               SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)
        rif3 = sai_thrift_create_router_interface(self.client, vr1,
               SAI_ROUTER_INTERFACE_TYPE_PORT, port3, 0, v4_enabled, v6_enabled, mac)
        rif4 = sai_thrift_create_router_interface(self.client, vr1,
               SAI_ROUTER_INTERFACE_TYPE_PORT, port4, 0, v4_enabled, v6_enabled, mac)

        sai_thrift_create_neighbor(self.client, addr_family, rif1, nhop_ip1, dmac1)
        sai_thrift_create_neighbor(self.client, addr_family, rif2, nhop_ip2, dmac2)
        sai_thrift_create_neighbor(self.client, addr_family, rif3, nhop_ip3, dmac3)

        nhop1 = sai_thrift_create_nhop(self.client, addr_family, nhop_ip1, rif1)
        nhop2 = sai_thrift_create_nhop(self.client, addr_family, nhop_ip2, rif2)
        nhop3 = sai_thrift_create_nhop(self.client, addr_family, nhop_ip3, rif3)

        nhop_group1 = sai_thrift_create_next_hop_group(self.client)

        nhop_gmember1 = sai_thrift_create_next_hop_group_member(self.client,
                                                          nhop_group1, nhop1)
        nhop_gmember2 = sai_thrift_create_next_hop_group_member(self.client,
                                                          nhop_group1, nhop2)
        nhop_gmember3 = sai_thrift_create_next_hop_group_member(self.client,
                                                          nhop_group1, nhop3)

        sai_thrift_create_route(self.client, vr1, addr_family, ip_addr1,
                                                   ip_mask1, nhop_group1)

        #creating the hash_obj
        print ("Setting the ECMP IPV4 hash object with all fields..")
        hash_attr_list = [
                          SAI_NATIVE_HASH_FIELD_SRC_IP,
                          SAI_NATIVE_HASH_FIELD_DST_IP,
                          SAI_NATIVE_HASH_FIELD_IP_PROTOCOL,
                          SAI_NATIVE_HASH_FIELD_L4_DST_PORT,
                          SAI_NATIVE_HASH_FIELD_L4_SRC_PORT
                          ]
        hash_id = sai_thrift_create_hash(self.client, hash_attr_list)
        print('hash_data', hash_attr_list)

        #Associating the hash obj to the device
        attr_value = sai_thrift_attribute_value_t(oid=hash_id)
        attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_ECMP_HASH_IPV4,
                                                        value=attr_value)
        self.client.sai_thrift_set_switch_attribute(attr)

        print("Getting the attribute list..")
        hash_data = sai_thrift_get_hash_attribute(self.client, hash_id)
        data_val = hash_data.attr_list[0].value.s32list.s32list
        print('hash_data: ', data_val)

        # send the test packet(s)
        try:
            max_itrs = 120
            def PacketTest():
                count = [0, 0, 0]
                dst_ip = int(socket.inet_aton('10.10.10.1').encode('hex'),16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntoa(hex(dst_ip)[2:].zfill(8).decode('hex'))
                    pkt = simple_tcp_packet(eth_dst=router_mac,
                                            eth_src='00:22:22:22:22:22',
                                            ip_dst=dst_ip_addr,
                                            ip_src='192.168.8.1',
                                            ip_id=106,
                                            ip_ttl=64)
                    exp_pkt1 = simple_tcp_packet(eth_dst='00:11:22:33:44:55',
                                             eth_src=router_mac,
                                             ip_dst=dst_ip_addr,
                                             ip_src='192.168.8.1',
                                             ip_id=106,
                                             ip_ttl=63)
                    exp_pkt2 = simple_tcp_packet(eth_dst='00:11:22:33:44:56',
                                             eth_src=router_mac,
                                             ip_dst=dst_ip_addr,
                                             ip_src='192.168.8.1',
                                             ip_id=106,
                                             ip_ttl=63)
                    exp_pkt3 = simple_tcp_packet(eth_dst='00:11:22:33:44:57',
                                             eth_src=router_mac,
                                             ip_dst=dst_ip_addr,
                                             ip_src='192.168.8.1',
                                             ip_id=106,
                                             ip_ttl=63)

                    send_packet(self, switch_ports[3], str(pkt))
                    rcv_idx = verify_any_packet_any_port(self,
                               [exp_pkt1, exp_pkt2, exp_pkt3],
                               [switch_ports[0], switch_ports[1],switch_ports[2]])
                    count[rcv_idx] += 1
                    dst_ip += 1
                return count

            count = PacketTest()
            print ('Traffic distribution [port1, port2, port3]', count)
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.8)),
                                "Not all paths are equally balanced, %s" % count)

            #selecting only source IP for hasing
            print("Selecting source IP for Lag hash calculation..")
            set_attr_list = [SAI_NATIVE_HASH_FIELD_SRC_IP]
            sai_thrift_update_hash_attribute(self.client, hash_id, set_attr_list)

            print("Getting the attribute list..")
            hash_data = sai_thrift_get_hash_attribute(self.client, hash_id)
            data_val = hash_data.attr_list[0].value.s32list.s32list
            print('hash_data: ', data_val)

            count = PacketTest()
            print ('Traffic distribution [port1, port2, port3]', count)
            for i in range(0, 3):
                self.assertTrue((count[i] == max_itrs) or (count[i] == 0),
                        "Not all Lag paths are equally balanced")

        finally:
            sai_thrift_remove_route(self.client, vr1, addr_family, ip_addr1,
                                                       ip_mask1, nhop_group1)

            self.client.sai_thrift_remove_next_hop_group_member(nhop_gmember1)
            self.client.sai_thrift_remove_next_hop_group_member(nhop_gmember2)
            self.client.sai_thrift_remove_next_hop_group_member(nhop_gmember3)

            self.client.sai_thrift_remove_next_hop_group(nhop_group1)

            self.client.sai_thrift_remove_next_hop(nhop1)
            self.client.sai_thrift_remove_next_hop(nhop2)
            self.client.sai_thrift_remove_next_hop(nhop3)

            sai_thrift_remove_neighbor(self.client, addr_family,
                                           rif1, nhop_ip1, dmac1)
            sai_thrift_remove_neighbor(self.client, addr_family,
                                           rif2, nhop_ip2, dmac2)
            sai_thrift_remove_neighbor(self.client, addr_family,
                                           rif3, nhop_ip3, dmac3)

            self.client.sai_thrift_remove_router_interface(rif1)
            self.client.sai_thrift_remove_router_interface(rif2)
            self.client.sai_thrift_remove_router_interface(rif3)
            self.client.sai_thrift_remove_router_interface(rif4)
            self.client.sai_thrift_remove_virtual_router(vr1)

            #Associating the hash obj to the device
            attr_value = sai_thrift_attribute_value_t(oid=0)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_ECMP_HASH_IPV4,
                                                           value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)
            print("Removing the hash object")
            sai_thrift_remove_hash(self.client, hash_id)

