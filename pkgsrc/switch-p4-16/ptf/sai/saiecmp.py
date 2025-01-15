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
Thrift SAI ECMP test cases
"""
import socket
import sai_base_test
from ptf import config
from ptf.testutils import *
from ptf.thriftutils import *
from switchsai_thrift.sai_headers import  *
from switch_utils import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *

class EcmpFGBasicTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print('')
        print ('ECMP Fine Grain Basic Test for configuration')
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

        #creating the ECMP FINE GRAIN TYPE
        nhop_group1 = sai_thrift_create_next_hop_group(self.client,
                        type=SAI_NEXT_HOP_GROUP_TYPE_FINE_GRAIN_ECMP,
                        config_size=120)

        #Getting the Real Size
        nhop_grp_real_size = sai_thrift_get_next_hop_group_attr(self.client, 
                          nhop_group1,
                          SAI_NEXT_HOP_GROUP_ATTR_REAL_SIZE)
        print ('Number of members in FG ECMP group --> Real Size: %u' %nhop_grp_real_size)


        #Adding the next-hop-group members
        nhop_gmember1 = sai_thrift_create_next_hop_group_member(self.client,
                           nhop_group=nhop_group1, nhop=nhop1, index=0)
        nhop_gmember2 = sai_thrift_create_next_hop_group_member(self.client,
                           nhop_group=nhop_group1, nhop=nhop2, index=1)
        nhop_gmember3 = sai_thrift_create_next_hop_group_member(self.client,
                           nhop_group=nhop_group1, nhop=nhop3, index=2)

        #creating the route
        sai_thrift_create_route(self.client, vr1, addr_family, ip_addr1,
                                                   ip_mask1, nhop_group1)

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
            print ('Traffic distribution [port1, port2, port3] --> %s' % count)
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.8)),
                                'Not all paths are equally balanced, %s' % count)

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

class EcmpFGGrpMemAttrTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print('')
        print ('ECMP Fine Grain Attribute Membership Test')
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

        #creating the ECMP FINE GRAIN TYPE
        nhop_group1 = sai_thrift_create_next_hop_group(self.client,
                        type=SAI_NEXT_HOP_GROUP_TYPE_FINE_GRAIN_ECMP,
                        config_size=120)

        #Getting the Real Size
        nhop_grp_real_size = sai_thrift_get_next_hop_group_attr(self.client, 
                          nhop_group1,
                          SAI_NEXT_HOP_GROUP_ATTR_REAL_SIZE)
        print("Number of members in FG ECMP group --> Real Size: %u" %nhop_grp_real_size)


        #Adding the next-hop-group members
        print('Insert Ecmp Grp Member --> with Nexthop1')
        nhop_gmember1 = sai_thrift_create_next_hop_group_member(self.client,
                           nhop_group=nhop_group1, nhop=nhop1, index=0)

        print('Get the nexthop from the Ecmp Grp Member.. ')
        get_ecmp_gmem = sai_thrift_get_next_hop_group_member_attribute(self.client,
                        nhop_gmem_id=nhop_gmember1, attr_name=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_ID)

        if(get_ecmp_gmem.value.oid == nhop1):
            print('Get nexthop from Ecmp Grp Member success --> Nexthop1')
        else:
            print('Get nexthop from Ecmp Grp Member fail')

        #creating the route
        sai_thrift_create_route(self.client, vr1, addr_family, ip_addr1,
                                                   ip_mask1, nhop_group1)

        # send the test packet(s)
        try:
            max_itrs = 10

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
            print ('Traffic distribution [port1, port2, port3] --> %s' % count)
            self.assertTrue(count[0] == max_itrs, "Traffic is not following the exact path")

            # Update nexthop2 to the ecmp member
            print('Assigning Nexthop2 to an existing Ecmp Grp Member')
            sai_thrift_set_next_hop_group_member_attribute(self.client, 
                        nhop_gmem_id=nhop_gmember1, nexthop_handle=nhop2)

            print('Get nexthop from the Ecmp Grp Member.. ')
            get_ecmp_gmem = sai_thrift_get_next_hop_group_member_attribute(self.client,
                        nhop_gmem_id=nhop_gmember1, attr_name=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_ID)

            if(get_ecmp_gmem.value.oid == nhop2):
                print('Get nexthop from Ecmp Grp Member success --> Nexthop2')
            else:
                print('Get nexthop from Ecmp Grp Member fail')

            count = PacketTest()
            print ('Traffic distribution [port1, port2, port3] --> %s' % count)
            self.assertTrue(count[1] == max_itrs, 'Traffic is not following the exact path')

            # Update nexthop1 back again to the same ecmp member
            print('Assigning Ecmp Grp Member with orginal nexthop i.e. --> Nexthop1 ')
            sai_thrift_set_next_hop_group_member_attribute(self.client, 
                   nhop_gmem_id=nhop_gmember1, nexthop_handle=nhop1)

            get_ecmp_gmem = sai_thrift_get_next_hop_group_member_attribute(self.client,
                        nhop_gmem_id=nhop_gmember1, attr_name=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_ID)

            if(get_ecmp_gmem.value.oid == nhop1):
                print('Get nexthop from Ecmp Grp Member success --> Nexthop1')
            else:
                print('Get nexthop from Ecmp Grp Member fail')
            
            count = PacketTest()
            print ('Traffic distribution [port1, port2, port3] --> %s' % count)
            self.assertTrue(count[0] == max_itrs, 'Traffic is not following the exact path')


        finally:
            sai_thrift_remove_route(self.client, vr1, addr_family, ip_addr1,
                                                       ip_mask1, nhop_group1)

            self.client.sai_thrift_remove_next_hop_group_member(nhop_gmember1)

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
          
class EcmpFGAddDelMemTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print('')
        print ('ECMP Fine Grain Insert/Remove Member Test')
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

        #creating the ECMP FINE GRAIN TYPE
        nhop_group1 = sai_thrift_create_next_hop_group(self.client,
                        type=SAI_NEXT_HOP_GROUP_TYPE_FINE_GRAIN_ECMP,
                        config_size=120)

        #Getting the Real Size
        nhop_grp_real_size = sai_thrift_get_next_hop_group_attr(self.client, 
                          nhop_group1,
                          SAI_NEXT_HOP_GROUP_ATTR_REAL_SIZE)
        print("Number of members in FG ECMP group --> Real Size: %u" %nhop_grp_real_size)


        #Adding the next-hop-group members
        nhop_gmember1 = sai_thrift_create_next_hop_group_member(self.client,
                           nhop_group=nhop_group1, nhop=nhop1, index=0)
        nhop_gmember2 = sai_thrift_create_next_hop_group_member(self.client,
                           nhop_group=nhop_group1, nhop=nhop2, index=1)
        nhop_gmember3 = sai_thrift_create_next_hop_group_member(self.client,
                           nhop_group=nhop_group1, nhop=nhop3, index=2)

        get_ecmp_gmem = sai_thrift_get_next_hop_group_member_attribute(self.client,
                        nhop_gmem_id=nhop_gmember1, attr_name=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_ID)

        #creating the route
        sai_thrift_create_route(self.client, vr1, addr_family, ip_addr1,
                                                   ip_mask1, nhop_group1)

        # send the test packet(s)
        try:
            max_itrs = 1

            def PacketTest():
                count = [0, 0, 0]
                dst_ip_addr = '10.10.10.1'
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
                return count

            count = PacketTest()
            print ('Traffic distribution [port1, port2, port3] --> %s' % count)

            print('Removing Ecmp Grp Member with nexthop2 at Member idx 1')
            self.client.sai_thrift_remove_next_hop_group_member(nhop_gmember2)
            count = PacketTest()
            print ('Change in hash calc moves traffic to another port [port1, port2, port3] --> %s' % count)

            print('Re-Inserting Ecmp Grp Member with nexthop2 back at Member idx 1')
            nhop_gmember2 = sai_thrift_create_next_hop_group_member(self.client,
                           nhop_group=nhop_group1, nhop=nhop2, index=1)
            count = PacketTest()
            print ('Traffic now moves back to original port [port1, port2, port3] --> %s' % count)

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

