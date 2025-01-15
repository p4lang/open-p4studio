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
Thrift SAI interface ACL tests
"""

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
import time
from bf_switcht_api_thrift.model_headers import *


this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.pktpy_utils import pktpy_skip # noqa pylint: disable=wrong-import-position

'''
The intention is to simulate the ACL setup used in ACL community tests
Create an ACL group per port
Create 3 ACL tables for ipv4 and mirror types
Create an ACL group member per table and per group
Add entries in each of the tables and test ACLs work as expected
'''


@pktpy_skip  # TODO bf-pktpy
class SonicACLTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        self.acl_groups = []
        self.acl_group_members = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        for index, port in port_list.items():
            # create ACL table group
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            print index, port
            self.acl_groups.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_ipv4_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, 50)
        acl_table_ipv4_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        # create ACL table group members
        for acl_group in self.acl_groups:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id2, 1)
            self.acl_group_members.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_mirror_id, 1)
            self.acl_group_members.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id, 1)
            self.acl_group_members.append(acl_table_group_member_id)

        # create acl entries in secondary ipv4 acl
        '''
        "ACL_RULE|DATAACL|DEFAULT_RULE": {
            "type": "hash",
            "value": {
                "ETHER_TYPE": "2048",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "1"
            }
        },
        '''
        acl_entry_id_default = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id2, 1, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp, ether_type=2048)

        '''
        "ACL_RULE|DATAACL|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id2, 9999, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        # remove the entries just created in a secondary table
        # to simulate behaviour of SONiC CT
        self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
        self.client.sai_thrift_remove_acl_entry(acl_entry_id_default)

        # create acl entries in ipv4 acl
        # catch all drop rule
        '''
        "ACL_RULE|DATAINGRESS|DEFAULT_RULE": {
            "type": "hash",
            "value": {
                "ETHER_TYPE": "2048",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "1"
            }
        },
        '''
        acl_entry_id_default = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 1, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp, ether_type=2048)

        '''
        "ACL_RULE|DATAINGRESS|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9999, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_10": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT_RANGE": "4656-4671",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9990"
            }
        },
        '''
        u32range = sai_thrift_range_t(min=4656, max=4671)
        acl_range_id1 = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, u32range)
        range_list1 = [acl_range_id1]
        acl_entry_id10 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9990, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list1, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_11": {
            "type": "hash",
            "value": {
                "L4_DST_PORT_RANGE": "4640-4687",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9989"
            }
        },
        '''
        u32range = sai_thrift_range_t(min=4640, max=4687)
        acl_range_id2 = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, u32range)
        range_list2 = [acl_range_id2]
        acl_entry_id11 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9989, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list2, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_12": {
            "type": "hash",
            "value": {
                "IP_PROTOCOL": "1",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9988",
                "SRC_IP": "20.0.0.4/32"
            }
        },
        '''
        acl_entry_id12 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9988, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.4", "255.255.255.255", ip_dst, ip_dst_mask,
            1, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_13": {
            "type": "hash",
            "value": {
                "IP_PROTOCOL": "17",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9987",
                "SRC_IP": "20.0.0.4/32"
            }
        }
        '''
        acl_entry_id13 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9987, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.4", "255.255.255.255", ip_dst, ip_dst_mask,
            17, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_14": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9986",
                "SRC_IP": "20.0.0.6/32"
            }
        },
        '''
        acl_entry_id14 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9986, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.6", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_15": {
            "type": "hash",
            "value": {
                "DST_IP": "192.168.0.17/32",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9985"
            }
        },
        '''
        acl_entry_id15 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9985, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, "192.168.0.17", "255.255.255.255",
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_16": {
            "type": "hash",
            "value": {
                "DST_IP": "172.16.3.0/32",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9984"
            }
        },
        '''
        acl_entry_id16 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9984, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, "172.16.3.0", "255.255.255.255",
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_17": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT": "4721",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9983"
            }
        },
        '''
        acl_entry_id17 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9983, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            4721, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_18": {
            "type": "hash",
            "value": {
                "IP_PROTOCOL": "127",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9982"
            }
        },
        '''
        acl_entry_id18 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9982, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            127, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_19": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9981",
                "TCP_FLAGS": "0x24/0x24"
            }
        },
        '''
        acl_entry_id19 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9981, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp, tcp_flags=0x24)

        '''
        "ACL_RULE|DATAINGRESS|RULE_2": {
            "type": "hash",
            "value": {
                "DST_IP": "192.168.0.16/32",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9998"
            }
        },
        '''
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9998, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, "192.168.0.16", "255.255.255.255",
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_20": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9980",
                "SRC_IP": "20.0.0.7/32"
            }
        },
        '''
        acl_entry_id20 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9980, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.7", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_21": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9979",
                "SRC_IP": "20.0.0.7/32"
            }
        },
        '''
        acl_entry_id21 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9979, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.7", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_22": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "4731",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9978"
            }
        },
        '''
        acl_entry_id22 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9978, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 4731,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_23": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT_RANGE": "4756-4771",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9977"
            }
        },
        '''
        u32range = sai_thrift_range_t(min=4756, max=4771)
        acl_range_id3 = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, u32range)
        range_list3 = [acl_range_id3]
        acl_entry_id23 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9977, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list3, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_24": {
            "type": "hash",
            "value": {
                "L4_DST_PORT_RANGE": "4740-4787",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9976"
            }
        },
        '''
        u32range = sai_thrift_range_t(min=4740, max=4787)
        acl_range_id4 = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, u32range)
        range_list4 = [acl_range_id4]
        acl_entry_id24 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9976, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list4, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_25": {
            "type": "hash",
            "value": {
                "IP_PROTOCOL": "1",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9975",
                "SRC_IP": "20.0.0.8/32"
            }
        },
        '''
        acl_entry_id25 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9975, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.8", "255.255.255.255", ip_dst, ip_dst_mask,
            1, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_26": {
            "type": "hash",
            "value": {
                "IP_PROTOCOL": "17",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9974",
                "SRC_IP": "20.0.0.8/32"
            }
        },
        '''
        acl_entry_id26 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9974, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.8", "255.255.255.255", ip_dst, ip_dst_mask,
            17, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_27": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT": "179",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9973"
            }
        },
        '''
        acl_entry_id27 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9973, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            179, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_28": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "179",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9972"
            }
        },
        '''
        acl_entry_id28 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9972, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 179,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_3": {
            "type": "hash",
            "value": {
                "DST_IP": "172.16.2.0/32",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9997"
            }
        },
        '''
        acl_entry_id3 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9997, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, "172.16.2.0", "255.255.255.255",
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_4": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT": "4621",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9996"
            }
        },
        '''
        acl_entry_id4 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9996, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            4621, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_5": {
            "type": "hash",
            "value": {
                "IP_PROTOCOL": "126",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9995"
            }
        },
        '''
        acl_entry_id5 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9996, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            126, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_6": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9994",
                "TCP_FLAGS": "0x1b/0x1b"
            }
        },
        '''
        acl_entry_id6 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9994, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp, tcp_flags=0x1b)
        '''
        "ACL_RULE|DATAINGRESS|RULE_7": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9993",
                "SRC_IP": "20.0.0.3/32"
            }
        },
        '''
        acl_entry_id7 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9993, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.3", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_8": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9992",
                "SRC_IP": "20.0.0.3/32"
            }
        },
        '''
        acl_entry_id8 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9992, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.3", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|DATAINGRESS|RULE_9": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "4631",
                "PACKET_ACTION": "FORWARD",
                "PRIORITY": "9991"
            }
        },
        '''
        acl_entry_id9 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9991, SAI_PACKET_ACTION_FORWARD, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 4631,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        try:
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.1",
              ip_dst = "192.168.0.1",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self,switch_ports[1], str(pkt))
            verify_no_other_packets(self, timeout=2)

            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.1",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            exp_pkt = simple_tcp_packet(
              eth_dst = dmac1,
              eth_src = router_mac,
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.1",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 63)
            send_packet(self,switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt,[switch_ports[0]])

        finally:
            # unbind this ACL groups from ports object id
            for index, port in port_list.items():
                attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id28)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id27)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id26)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id25)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id24)
            self.client.sai_thrift_delete_acl_range(acl_range_id4)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id23)
            self.client.sai_thrift_delete_acl_range(acl_range_id3)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id22)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id21)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id20)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id19)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id18)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id17)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id16)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id15)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id14)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id13)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id12)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id11)
            self.client.sai_thrift_delete_acl_range(acl_range_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id10)
            self.client.sai_thrift_delete_acl_range(acl_range_id1)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id9)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id8)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id7)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id6)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id5)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id4)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id3)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id_default)
            for acl_group_member in self.acl_group_members:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id)
            self.client.sai_thrift_remove_acl_table(acl_table_mirror_id)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id2)
            for acl_group in self.acl_groups:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

# currently there is no support to have multiple tables from the same port label space
# There can be just one table using a specific port label space, so not enabling the below ptf


@pktpy_skip  # TODO bf-pktpy
@group('acl-m1')
class ACLGroupSeveralMembersTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        self.acl_group_members = []
        self.acl_tables = []
        self.acl_entries = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        monitor_port = port3
        source_port = port2
        mirror_type = SAI_MIRROR_SESSION_TYPE_LOCAL

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        ipv4_family = SAI_IP_ADDR_FAMILY_IPV4
        ipv4_addr1 = '192.168.0.1'
        ipv6_family = SAI_IP_ADDR_FAMILY_IPV6
        ipv6_addr1 = '4000::1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, ipv4_family, rif_id1, ipv4_addr1, dmac1)
        sai_thrift_create_neighbor(self.client, ipv6_family, rif_id1, ipv6_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, ipv4_family, ipv4_addr1, rif_id1)
        nhop2 = sai_thrift_create_nhop(self.client, ipv6_family, ipv6_addr1, rif_id1)

        spanid = sai_thrift_create_mirror_session(self.client, mirror_type=mirror_type, port=monitor_port)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        self.acl_group = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        ip_src = None

        # IPv4 ACL table
        acl_table_ipv4_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            ipv4_family,
            ip_src="192.168.0.1")
        self.acl_tables.append(acl_table_ipv4_id)

        # IPv6 ACL table
        acl_table_ipv6_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            SAI_IP_ADDR_FAMILY_IPV6,
            ip_src='2000::1')
        self.acl_tables.append(acl_table_ipv6_id)

        # Mirror ACL table
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            ipv4_family,
            ip_src="192.168.0.1")
        self.acl_tables.append(acl_table_mirror_id)

        # create ACL table group members
        v4_acl_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
            self.acl_group, acl_table_ipv4_id, 1)
        self.acl_group_members.append(v4_acl_group_member_id)

        v6_acl_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
            self.acl_group, acl_table_ipv6_id, 1)
        self.acl_group_members.append(v6_acl_group_member_id)

        mirror_acl_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
            self.acl_group, acl_table_mirror_id, 1)
        self.acl_group_members.append(mirror_acl_group_member_id)

        ipv4_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9999,
            action=SAI_PACKET_ACTION_DROP,
            addr_family=ipv4_family,
            ip_src="20.0.0.1", ip_src_mask="255.255.255.255")
        self.acl_entries.append(ipv4_acl_entry_id)

        ipv6_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv6_id, 9999,
            action=SAI_PACKET_ACTION_DROP,
            addr_family=ipv6_family,
            ip_src='2000::1', ip_src_mask="ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
        self.acl_entries.append(ipv6_acl_entry_id)

        mirror_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_mirror_id, 9999,
            ingress_mirror=spanid,
            addr_family=ipv4_family,
            ip_src="20.0.0.3", ip_src_mask="255.255.255.255")
        self.acl_entries.append(mirror_acl_entry_id)

        try:
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.1",
              ip_dst = "192.168.0.1",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            exp_pkt = simple_tcp_packet(
              eth_dst = dmac1,
              eth_src = router_mac,
              ip_src = "20.0.0.1",
              ip_dst = "192.168.0.1",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 63)

            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt, [switch_ports[0]])

            pktv6 = simple_tcpv6_packet(
                eth_dst=router_mac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::1',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            exp_pktv6 = simple_tcpv6_packet(
                eth_dst= dmac1,
                eth_src= router_mac,
                ipv6_dst='4000::1',
                ipv6_src='2000::1',
                ipv6_hlim=63)

            send_packet(self, switch_ports[1], str(pktv6))
            verify_packets(self, exp_pktv6, [switch_ports[0]])

            pkt2 = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.1",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            exp_pkt2 = simple_tcp_packet(
              eth_dst = dmac1,
              eth_src = router_mac,
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.1",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 63)

            exp_mirror_pkt = pkt2.copy()
            m = Mask(exp_mirror_pkt)
            m.set_do_not_care_scapy(ptf.packet.IP,'id')
            m.set_do_not_care_scapy(ptf.packet.IP,'chksum')

            send_packet(self, switch_ports[1], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[0]])

            # Bind ACL group to port and verify ACLs work
            attr_value = sai_thrift_attribute_value_t(oid=self.acl_group)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            send_packet(self, switch_ports[1], str(pkt))
            verify_no_other_packets(self, timeout=2)

            send_packet(self, switch_ports[1], str(pktv6))
            verify_no_other_packets(self, timeout=2)

            send_packet(self, switch_ports[1], str(pkt2))
            verify_each_packet_on_each_port(self, [exp_pkt2,m], ports=[0,2])

            # Unbind ACL group from port - ACLs sholdn't have any effect
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)

            send_packet(self, switch_ports[1], str(pkt))
            verify_packets(self, exp_pkt,[switch_ports[0]])

            send_packet(self, switch_ports[1], str(pktv6))
            verify_packets(self, exp_pktv6, [switch_ports[0]])

            send_packet(self, switch_ports[1], str(pkt2))
            verify_packets(self, exp_pkt2, [switch_ports[0]])

        finally:
            # unbind ACL group from port
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            # cleanup ACL
            for acl_group_member in self.acl_group_members:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table_group(self.acl_group)
            for acl_entry in self.acl_entries:
                self.client.sai_thrift_remove_acl_entry(acl_entry)
            for acl_table in self.acl_tables:
                self.client.sai_thrift_remove_acl_table(acl_table)
            # cleanup
            self.client.sai_thrift_remove_mirror_session(spanid)
            self.client.sai_thrift_remove_next_hop(nhop1)
            self.client.sai_thrift_remove_next_hop(nhop2)
            sai_thrift_remove_neighbor(self.client, ipv4_family, rif_id1, ipv4_addr1, dmac1)
            sai_thrift_remove_neighbor(self.client, ipv6_family, rif_id1, ipv6_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('acl')
@group('acl-ocp')
class IPAclTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.0.1 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_tos=0xc8,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_tos=0xc8,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self, switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_packets(self, exp_pkt, [switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list,
            ip_dscp)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list,
            ip_dscp)

        # bind this ACL table to port2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL \'DROP, src 192.168.0.1/255.255.255.0, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            # send the same packet
            send_packet(self, switch_ports[1], str(pkt))
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_no_other_packets(self, timeout=2)
        finally:
            # unbind this ACL table from port2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('acl')
@group('acl-ocp')
class IPv6NextHdrTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (2000::1 ---> 1234:5678:9abc:def0:4422:1133:5577:99aa)"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)

        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV6
        ip_addr1 = '1234:5678:9abc:def0:4422:1133:5577:99aa'
        ip_addr1_subnet = '1234:5678:9abc:def0:4422:1133:5577:0'
        ip_mask1 = 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:0'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        tcpv6 = simple_tcpv6_packet(
                                eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ipv6_dst=ip_addr1,
                                ipv6_src='2000::1',
                                ipv6_hlim=64)
        exp_tcpv6 = simple_tcpv6_packet(
                                eth_dst=dmac1,
                                eth_src=router_mac,
                                ipv6_dst=ip_addr1,
                                ipv6_src='2000::1',
                                ipv6_hlim=63)

        udpv6 = simple_udpv6_packet(
                                eth_dst=router_mac,
                                eth_src='00:22:22:22:22:22',
                                ipv6_dst=ip_addr1,
                                ipv6_src='2000::1',
                                ipv6_hlim=64)
        exp_udpv6 = simple_udpv6_packet(
                                eth_dst=dmac1,
                                eth_src=router_mac,
                                ipv6_dst=ip_addr1,
                                ipv6_src='2000::1',
                                ipv6_hlim=63)

        try:
            print '#### NO ACL Applied: sending TCP packets ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 1234:5678:9abc:def0:4422:1133:5577:99aa | 2000::1 | @ ptf_intf 2'
            send_packet(self, switch_ports[1], str(tcpv6))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 1234:5678:9abc:def0:4422:1133:5577:99aa | 2000::1 | @ ptf_intf 1'
            verify_packets(self, exp_tcpv6, [switch_ports[0]])

            print '#### NO ACL Applied: sending UDP packets ####'
            send_packet(self, switch_ports[1], str(udpv6))
            verify_packets(self, exp_udpv6, [switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2-[acl]-> ptf_intf 1 (2000::1-[acl]-> 1234:5678:9abc:def0:4422:1133:5577:99aa)"
        # setup ACL to block based on Source IP

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        addr_family=SAI_IP_ADDR_FAMILY_IPV6
        # next level protocol is TCP
        ipv6_next_header = 0x06

        acl_table_ipv6_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            ip_src='2000::1')

        # Add drop ACL entry to IPv6 ACL Table
        ipv6_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv6_id, 9999,
            action=SAI_PACKET_ACTION_DROP,
            addr_family=addr_family,
            ipv6_next_header=ipv6_next_header,
            ip_src='2000::1',
            ip_src_mask="ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")

        # bind this ACL table to port2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_ipv6_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        try:
            assert acl_table_ipv6_id != 0, 'acl_table_ipv6_id is == 0'
            assert ipv6_acl_entry_id != 0, 'ipv6_acl_entry_id is == 0'

            print '#### Sending   TCP', router_mac, '| 00:22:22:22:22:22 | 1234:5678:9abc:def0:4422:1133:5577:99aa | 2000::1 | @ ptf_intf 2'
            send_packet(self, switch_ports[1], str(tcpv6))
            # ensure the TCP packet is dropped and check for absence of packet here
            print '#### NOT Expecting TCP 00:11:22:33:44:55 |', router_mac, '| 1234:5678:9abc:def0:4422:1133:5577:99aa | 2000::1 | @ ptf_intf 1'
            verify_no_other_packets(self, timeout=2)

            print '#### Sending   UDP', router_mac, '| 00:22:22:22:22:22 | 1234:5678:9abc:def0:4422:1133:5577:99aa | 2000::1 | @ ptf_intf 2'
            send_packet(self, switch_ports[1], str(udpv6))
            # ensure the UDP packet is forwarded
            print '#### Expecting UDP 00:11:22:33:44:55 |', router_mac, '| 1234:5678:9abc:def0:4422:1133:5577:99aa | 2000::1 | @ ptf_intf 1'
            verify_packets(self, exp_udpv6, [switch_ports[0]])

            # change action_type of ACL entry from ACL_DROP to ACL_PERMIT
            aclaction_data = sai_thrift_acl_action_data_t(parameter=sai_thrift_acl_data_t(s32=SAI_PACKET_ACTION_FORWARD), enable = True)
            attribute_value = sai_thrift_attribute_value_t(aclaction=aclaction_data)
            attr = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_PACKET_ACTION, value=attribute_value)
            self.client.sai_thrift_set_acl_entry_attribute(ipv6_acl_entry_id, attr)

            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 1234:5678:9abc:def0:4422:1133:5577:99aa | 2000::1 | @ ptf_intf 2'
            # send the same packet
            send_packet(self, switch_ports[1], str(tcpv6))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 1234:5678:9abc:def0:4422:1133:5577:99aa | 2000::1 | @ ptf_intf 1'
            # check that TCP packet is forwarded
            verify_packets(self, exp_tcpv6, [switch_ports[0]])

        finally:
            #unbind this ACL table from port2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            #cleanup ACL
            self.client.sai_thrift_remove_acl_entry(ipv6_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv6_id)
            #cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

@group('acl')
@group('acl-ocp')
class IPAclFragmentTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.0.1 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_tos=0xc8,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_tos=0xc8,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self, switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_packets(self, exp_pkt, [switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        # setup ACL to block based on Source IP
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None
        ip_frag = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list,
            ip_dscp,
            ip_frag=True)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list,
            ip_dscp,
            ip_frag=SAI_ACL_IP_FRAG_ANY)

        # bind this ACL table to port2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL Applied, but non frag ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self, switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
            print '#### ACL no Drop, DF=1, offset = 0, Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            # send the same packet
            pkt['IP'].flags = 2
            exp_pkt['IP'].flags = 2
            pkt['IP'].frag = 0
            send_packet(self, switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
            exp_pkt['IP'].flags = 0
            print '#### ACL Drop, MF=1, offset = 0, first fragment, Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            # send the same packet
            pkt['IP'].flags = 1
            pkt['IP'].frag = 0
            send_packet(self, switch_ports[1], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_no_other_packets(self, timeout=2)
            print '#### ACL Drop, MF=1, offset = 20, non head fragment, Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            # send the same packet
            pkt['IP'].flags = 1
            pkt['IP'].frag = 20
            send_packet(self, switch_ports[1], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_no_other_packets(self, timeout=2)
            print '#### ACL Drop, MF=0, offset = 20, last fragment, Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            # send the same packet
            pkt['IP'].flags = 0
            pkt['IP'].frag = 20
            send_packet(self, switch_ports[1], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_no_other_packets(self, timeout=2)
        finally:
            # unbind this ACL table from port2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

@group('acl')
@group('acl-ocp')
class MACSrcAclTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "This test is not supported. MAC lookup cannot be done for IP Packets in switch.p4"
        return

        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.0.1 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source MAC
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = '00:22:22:22:22:22'
        mac_dst = None
        mac_src_mask = 'ff:ff:ff:ff:ff:ff'
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to port2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL \'DROP, src mac 00:22:22:22:22:22, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            # send the same packet
            send_packet(self,switch_ports[1], str(pkt))
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_no_other_packets(self, timeout=2)
        finally:
            # unbind this ACL table from port2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('acl')
@group('acl-ocp')
class L3AclTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print "Testing L3AclTest"

        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 --->172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        L4_SRC_PORT = 1000
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_sport = L4_SRC_PORT,
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_sport = L4_SRC_PORT,
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP and SPORT
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None

        u32range = sai_thrift_range_t(min=1000, max=1000)
        acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, u32range)
        range_list = [acl_range_id]

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to rif_id2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_router_interface_attribute(rif_id2, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            # send the same packet
            send_packet(self,switch_ports[1], str(pkt))
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from rif_id2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_router_interface_attribute(rif_id2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            self.client.sai_thrift_delete_acl_range(acl_range_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('acl')
class L3AclCounterTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print "Testing L3AclCounterTest"

        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 --->172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        L4_SRC_PORT = 1000
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_sport = L4_SRC_PORT,
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_sport = L4_SRC_PORT,
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP and SPORT
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None

        u32range = sai_thrift_range_t(min=1000, max=1000)
        acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, u32range)
        range_list = [acl_range_id]

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to rif_id2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_router_interface_attribute(rif_id2, attr)

        # create ACL counter and bind it to the ACL entry
        acl_counter_id = sai_thrift_create_acl_counter(self.client, acl_table_id)
        acl_action_data = sai_thrift_acl_action_data_t(parameter=sai_thrift_acl_parameter_t(oid=acl_counter_id),
                                                        enable=True)
        attr_value = sai_thrift_attribute_value_t(aclaction=acl_action_data)
        attr = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_COUNTER, value=attr_value)
        self.client.sai_thrift_set_acl_entry_attribute(acl_entry_id, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'
            assert acl_counter_id != 0, 'acl_counter_id is == 0'
            pkt_cnt = 5

            attr_list = []
            attr_list.append(SAI_ACL_COUNTER_ATTR_PACKETS)
            attr_list.append(SAI_ACL_COUNTER_ATTR_BYTES)
            attr_values = self.client.sai_thrift_get_acl_counter_attribute(acl_counter_id, attr_list)

            initial_pkts_cnt = attr_values[0].u64
            initial_bytes_cnt = attr_values[1].u64

            print '#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            # send the same packet
            for i in range(0, pkt_cnt):
                send_packet(self,switch_ports[1], str(pkt))
            # ensure packets are dropped
            # check for absence of packets here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0'
            verify_no_other_packets(self, timeout=1)

            time.sleep(2)

            attr_values = self.client.sai_thrift_get_acl_counter_attribute(acl_counter_id, attr_list)

            actual_pkts_cnt = (attr_values[0].u64 - initial_pkts_cnt)
            actual_bytes_cnt = (attr_values[1].u64 - initial_bytes_cnt)

            assert actual_pkts_cnt == pkt_cnt, 'packets counter value: {} is not {}'.format(actual_pkts_cnt, pkt_cnt)
            #assert actual_bytes_cnt == len(pkt) * pkt_cnt, 'bytes counter value: {} is not {}'.format(actual_bytes_cnt, len(pkt) * pkt_cnt)

        finally:
            # unbind this ACL table from rif_id2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_router_interface_attribute(rif_id2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            self.client.sai_thrift_remove_acl_counter(acl_counter_id)
            self.client.sai_thrift_delete_acl_range(acl_range_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

@group('acl')
@group('acl-ocp')
@disabled
# No support to have two ACL tables of same type [Ex - V4 type] or two table types sharing the same Label space on a port
class SeqAclTableGroupTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.0.1 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        # create ACL table group
        acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
            group_stage,
            group_bind_point_list,
            group_type)

        # setup ACL tables to block based on Source MAC
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None

        # create ACL table #1
        acl_table_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_id1,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # create ACL table #2
        acl_table_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_id2,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # setup ACL table group members
        group_member_priority1 = 1
        group_member_priority2 = 100

        # create ACL table group members
        acl_table_group_member_id1 = sai_thrift_create_acl_table_group_member(self.client,
            acl_table_group_id,
            acl_table_id1,
            group_member_priority1)
        acl_table_group_member_id2 = sai_thrift_create_acl_table_group_member(self.client,
            acl_table_group_id,
            acl_table_id2,
            group_member_priority2)

        # bind this ACL table group to port2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        try:
            assert acl_table_group_id != 0, 'acl_table_group_id is == 0'
            assert acl_table_id1 != 0, 'acl_entry_id1 is == 0'
            assert acl_entry_id1 != 0, 'acl_entry_id1 is == 0'
            assert acl_table_id2 != 0, 'acl_entry_id2 is == 0'
            assert acl_entry_id2 != 0, 'acl_entry_id2 is == 0'
            assert acl_table_group_member_id1 != 0, 'acl_table_group_member_id1 is == 0'
            assert acl_table_group_member_id2 != 0, 'acl_table_group_member_id2 is == 0'

            print '#### ACL \'DROP, src mac 00:22:22:22:22:22, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            # send the same packet
            send_packet(self,switch_ports[1], str(pkt))
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from port2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_table_group_member(acl_table_group_member_id1)
            self.client.sai_thrift_remove_acl_table_group_member(acl_table_group_member_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            self.client.sai_thrift_remove_acl_table(acl_table_id1)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_table(acl_table_id2)
            self.client.sai_thrift_remove_acl_table_group(acl_table_group_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

@group('acl')
@group('acl-ocp')
@disabled
# No support to have two ACL tables of same type [Ex - V4 type] or two table types sharing the same Label space on a port
class MultBindAclTableGroupTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 4 -> [ptf_intf 1, ptf_intf 2, ptf_intf 3] (192.168.0.1 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)
        rif_id3 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port3, 0, v4_enabled, v6_enabled, mac)
        rif_id4 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port4, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id4, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id4)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id4)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            send_packet(self, switch_ports[0], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_packet(self, exp_pkt, switch_ports[3])
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_packet(self, exp_pkt, switch_ports[3])
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 3'
            send_packet(self, switch_ports[2], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_packet(self, exp_pkt, switch_ports[3])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet [ptf_intf 1, ptf_intf 2, ptf_intf 3] - [acl]->ptf_intf 4 (192.168.0.1 -[acl]-> 172.16.10.1 [id = 105])"

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        # create ACL table group
        acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
            group_stage,
            group_bind_point_list,
            group_type)

        # setup ACL tables to block based on Source MAC
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2, port3, port4]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None

        # create ACL table #1
        acl_table_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_id1,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # create ACL table #2
        acl_table_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_id2,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # setup ACL table group members
        group_member_priority1 = 1
        group_member_priority2 = 100

        # create ACL table group members
        acl_table_group_member_id1 = sai_thrift_create_acl_table_group_member(self.client,
            acl_table_group_id,
            acl_table_id1,
            group_member_priority1)
        acl_table_group_member_id2 = sai_thrift_create_acl_table_group_member(self.client,
            acl_table_group_id,
            acl_table_id2,
            group_member_priority2)

        # bind this ACL table group to port1, port2, port3 object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        try:
            assert acl_table_group_id != 0, 'acl_table_group_id is == 0'
            assert acl_table_id1 != 0, 'acl_entry_id1 is == 0'
            assert acl_entry_id1 != 0, 'acl_entry_id1 is == 0'
            assert acl_table_id2 != 0, 'acl_entry_id2 is == 0'
            assert acl_entry_id2 != 0, 'acl_entry_id2 is == 0'
            assert acl_table_group_member_id1 != 0, 'acl_table_group_member_id1 is == 0'
            assert acl_table_group_member_id2 != 0, 'acl_table_group_member_id2 is == 0'

            print '#### ACL \'DROP, src mac 00:22:22:22:22:22, in_ports[ptf_intf_1,2,3,4]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            send_packet(self, switch_ports[0], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_no_other_packets(self, timeout=1)
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_no_other_packets(self, timeout=1)
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 3'
            send_packet(self, switch_ports[2], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from port1, port2, port3 object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_table_group_member(acl_table_group_member_id1)
            self.client.sai_thrift_remove_acl_table_group_member(acl_table_group_member_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            self.client.sai_thrift_remove_acl_table(acl_table_id1)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_table(acl_table_id2)
            self.client.sai_thrift_remove_acl_table_group(acl_table_group_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id4)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id4, ip_addr1, dmac1)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_router_interface(rif_id3)
            self.client.sai_thrift_remove_router_interface(rif_id4)
            self.client.sai_thrift_remove_virtual_router(vr_id)

@group('acl')
@group('acl-ocp')
@disabled
# No support to have two ACL tables of same type [Ex - V4 type] or two table types sharing the same Label space on a port
class BindAclTableInGroupTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet [ptf_intf 1, ptf_intf 2, ptf_intf 3, ptf_intf 4]-> ptf_intf 5 (192.168.0.1 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        port4 = port_list[4]
        port5 = port_list[5]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)
        rif_id3 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port3, 0, v4_enabled, v6_enabled, mac)
        rif_id4 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port4, 0, v4_enabled, v6_enabled, mac)
        rif_id5 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port5, 0, v4_enabled, v6_enabled, mac)


        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id5, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id5)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id5)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 5'
            verify_packet(self, exp_pkt, switch_ports[5])
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self, switch_ports[2], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 5'
            verify_packet(self, exp_pkt, switch_ports[5])
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 3'
            send_packet(self, switch_ports[3], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 5'
            verify_packet(self, exp_pkt, switch_ports[5])
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            send_packet(self, switch_ports[4], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 5'
            verify_packet(self, exp_pkt, switch_ports[5])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet [ptf_intf 1, ptf_intf 2, ptf_intf 3, ptf_intf 4] -> ptf_intf 5 (192.168.0.1 ---> 172.16.10.1 [id = 105])"

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        # create ACL table group
        acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
            group_stage,
            group_bind_point_list,
            group_type)

        # setup ACL tables to block based on Source MAC
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2, port3, port4, port5]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        ingress_mirror_id = None
        egress_mirror_id = None
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None

        # create ACL table #1
        acl_table_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_id1,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # create ACL table #2
        acl_table_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_id2,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # setup ACL table group members
        group_member_priority1 = 1
        group_member_priority2 = 100

        # create ACL table group members
        acl_table_group_member_id1 = sai_thrift_create_acl_table_group_member(self.client,
            acl_table_group_id,
            acl_table_id1,
            group_member_priority1)
        acl_table_group_member_id2 = sai_thrift_create_acl_table_group_member(self.client,
            acl_table_group_id,
            acl_table_id2,
            group_member_priority2)

        # bind this ACL table group to port1, port2, port3, port4 object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        attr_value1 = sai_thrift_attribute_value_t(oid=acl_table_id2)
        attr1 = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)
        self.client.sai_thrift_set_port_attribute(port4, attr1)

        try:
            assert acl_table_group_id != 0, 'acl_table_group_id is == 0'
            assert acl_table_id1 != 0, 'acl_entry_id1 is == 0'
            assert acl_entry_id1 != 0, 'acl_entry_id1 is == 0'
            assert acl_table_id2 != 0, 'acl_entry_id2 is == 0'
            assert acl_entry_id2 != 0, 'acl_entry_id2 is == 0'
            assert acl_table_group_member_id1 != 0, 'acl_table_group_member_id1 is == 0'
            assert acl_table_group_member_id2 != 0, 'acl_table_group_member_id2 is == 0'

            print '#### ACL \'DROP, src mac 00:22:22:22:22:22, in_ports[ptf_intf_1,2,3,4]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 5'
            verify_no_other_packets(self, timeout=1)
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self, switch_ports[2], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 5'
            verify_no_other_packets(self, timeout=1)
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 3'
            send_packet(self, switch_ports[3], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 5'
            verify_no_other_packets(self, timeout=1)
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            send_packet(self, switch_ports[4], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 5'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from port1, port2, port3, port4 object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_set_port_attribute(port2, attr)
            self.client.sai_thrift_set_port_attribute(port3, attr)
            self.client.sai_thrift_set_port_attribute(port4, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_table_group_member(acl_table_group_member_id1)
            self.client.sai_thrift_remove_acl_table_group_member(acl_table_group_member_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            self.client.sai_thrift_remove_acl_table(acl_table_id1)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_table(acl_table_id2)
            self.client.sai_thrift_remove_acl_table_group(acl_table_group_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id5)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id5, ip_addr1, dmac1)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_router_interface(rif_id3)
            self.client.sai_thrift_remove_router_interface(rif_id4)
            self.client.sai_thrift_remove_router_interface(rif_id5)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('acl')
@group('acl-ocp')
class L3AclRangeTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        L4_DST_PORT = 1000
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP and SPORT
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None

        u32range = sai_thrift_range_t(min=1000, max=1000)
        acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, u32range)
        range_list = [acl_range_id]
        print "ACL range created 0x%lx"%(acl_range_id)

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            None,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        print "ACL Table created 0x%lx"%(acl_table_id)

        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to rif_id2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_router_interface_attribute(rif_id2, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            # send the same packet
            send_packet(self,switch_ports[1], str(pkt))
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from rif_id2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_router_interface_attribute(rif_id2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            self.client.sai_thrift_delete_acl_range(acl_range_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('acl')
@group('acl-ocp')
class L3L4PortTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Testing L4 src/dest port acl filter"
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        L4_DST_PORT = 1000
        L4_SRC_PORT = 500
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_sport = L4_SRC_PORT,
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_sport = L4_SRC_PORT,
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP and SPORT
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = L4_SRC_PORT
        dst_l4_port = L4_DST_PORT
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            None,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        print "ACL Table created 0x%lx"%(acl_table_id)

        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to rif_id2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_router_interface_attribute(rif_id2, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            # send the same packet
            send_packet(self,switch_ports[1], str(pkt))
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from rif_id2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_router_interface_attribute(rif_id2, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('egress-acl')
class EgressL3L4PortTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
          print "Egress ACL feature not enabled, skipping"
          return

        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        L4_DST_PORT = 1000
        L4_SRC_PORT = 500
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_mask1 = '255.255.255.0'
        ip_addr1 = '172.16.10.1'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_sport = L4_SRC_PORT,
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_sport = L4_SRC_PORT,
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP and SPORT
        table_stage = SAI_ACL_STAGE_EGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = L4_SRC_PORT
        dst_l4_port = L4_DST_PORT
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            None,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        print "ACL Table created 0x%lx"%(acl_table_id)

        acl_counter_handle = sai_thrift_create_acl_counter(
            client=self.client, acl_table_id=acl_table_id)
        print acl_counter_handle

        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list,acl_counter_id = acl_counter_handle)

        # bind this ACL table to rif_id2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_EGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_router_interface_attribute(rif_id1, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            # send the same packet
            num_packet = 10
            send_packet(self,switch_ports[1], str(pkt),count=num_packet)
            time.sleep(10)
            counter_values1 = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=acl_counter_handle)
            print counter_values1[0].u64
            if counter_values1[0].u64 == num_packet:
              print "ACL counter matches with num packets sent"
            else:
              print "ACL counter mismatch"
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from rif_id2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_EGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_router_interface_attribute(rif_id1, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)
            self.client.sai_thrift_remove_acl_counter(acl_counter_handle)


@pktpy_skip  # TODO bf-pktpy
@group('egress-acl')
class EgressL3AclRangeTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
          print "Egress ACL feature not enabled, skipping"
          return
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        L4_DST_PORT = 1000
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_addr1 = '172.16.10.1'
        ip_mask1 = '255.255.255.0'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=63)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP and SPORT
        table_stage = SAI_ACL_STAGE_EGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None

        u32range = sai_thrift_range_t(min=1000, max=1000)
        acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, u32range)
        range_list = [acl_range_id]
        print "ACL range created 0x%lx"%(acl_range_id)

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            None,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        print "ACL Table created 0x%lx"%(acl_table_id)

        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        # bind this ACL table to rif_id2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_EGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_router_interface_attribute(rif_id1, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            # send the same packet
            send_packet(self,switch_ports[1], str(pkt))
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from rif_id2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_EGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_router_interface_attribute(rif_id1, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            self.client.sai_thrift_delete_acl_range(acl_range_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
@group('mirror-acl')
class MultAclTableGroupBindTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print ''
        print '----------------------------------------------------------------------------------------------'
        print 'Testing both IPV4, MIRROR ACL table within a acl table group on same set of ports'
        print "Sending packet ptf_intf 4 -> [ptf_intf 1, ptf_intf 2, ptf_intf 3] (192.168.0.1 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)
        rif_id3 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port3, 0, v4_enabled, v6_enabled, mac)
        rif_id4 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port4, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '172.16.10.0'
        ip_addr1 = '172.16.10.1'
        ip_mask1 = '255.255.255.0'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id4, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id4)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id4)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        print '#### NO ACL Applied ####'
        print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
        send_packet(self, switch_ports[0], str(pkt))
        print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
        verify_packet(self, exp_pkt, switch_ports[3])
        print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
        send_packet(self,switch_ports[1], str(pkt))
        print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
        verify_packet(self, exp_pkt, switch_ports[3])
        print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 3'
        send_packet(self, switch_ports[2], str(pkt))
        print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
        verify_packet(self, exp_pkt, switch_ports[3])

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        # setup ACL tables to block on matching IP
        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = 1
        in_ports = [port1, port2, port3, port4]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None

        #  ACL table 1
        acl_table = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)

        # Setup Mirror ACL table
        monitor_port = port1
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        span_session=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print span_session

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (20.20.20.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL table to block on below matching param
        ip_src = "192.168.0.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = "172.16.10.1"
        ip_dst_mask = "255.255.255.0"
        ip_proto = 6
        action = None

        mirror_acl_table = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            in_ports,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)

        # setup ACL table group members
        group_member_priority1 = 1
        group_member_priority2 = 100

        acl_group_list = []
        acl_group_member_list = []
        for port in in_ports:
          # ACL table group
          acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
              group_stage,
              group_bind_point_list,
              group_type)

          # ACL table group member 1 - v4 table
          acl_table_group_member_id1 = sai_thrift_create_acl_table_group_member(self.client,
              acl_table_group_id,
              acl_table,
              group_member_priority1)

          # ACL table group member 2 - mirror table
          acl_table_group_member_id2 = sai_thrift_create_acl_table_group_member(self.client,
              acl_table_group_id,
              mirror_acl_table,
              group_member_priority1)

          acl_group_list.append(acl_table_group_id)
          acl_group_member_list.append(acl_table_group_member_id1)
          acl_group_member_list.append(acl_table_group_member_id2)


        for i in range(0,len(in_ports)):
          # attach this ACL table group to port1, port2, port3, port4
          print "Bind aclgroup 0x%lx to port 0x%lx"%(acl_group_list[i],in_ports[i])
          attr_value = sai_thrift_attribute_value_t(oid=acl_group_list[i])
          attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
          self.client.sai_thrift_set_port_attribute(in_ports[i], attr)

        action = SAI_PACKET_ACTION_DROP
        acl_entry = sai_thrift_create_acl_entry(self.client,
            acl_table,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            None, None,
            None,
            None,
            range_list)

        action = None
        src_l4_port = 4000
        dst_l4_port = 5000
        ingress_mirror_id = span_session

        mirror_acl_entry = sai_thrift_create_acl_entry(self.client,
            mirror_acl_table,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        try:
            print '#### ACL \'DROP, src mac 00:22:22:22:22:22, in_ports[ptf_intf_1,2,3,4]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 1'
            time.sleep(5)
            send_packet(self, switch_ports[0], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_no_other_packets(self, timeout=1)
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_no_other_packets(self, timeout=1)
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.0.1 | @ ptf_intf 3'
            send_packet(self, switch_ports[2], str(pkt))
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.0.1 | @ ptf_intf 4'
            verify_no_other_packets(self, timeout=1)
            print "Verify Mirror ACL"
            time.sleep(5)

            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='192.168.0.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=105,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)

            print "TX packet port 2 -> port 3, ipv4 acl blocks route pkt but mirror acl mirrors pkt to port 0"
            send_packet(self, switch_ports[2], pkt)
            verify_packets(self, pkt, ports=[switch_ports[0]])


        finally:
            # cleanup acl, remove acl group member
            for mbr in acl_group_member_list:
                self.client.sai_thrift_remove_acl_table_group_member(mbr)

            #  unlink this acl table from port1, port2, port3 object
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            for i in range(0,len(in_ports)):
                self.client.sai_thrift_set_port_attribute(in_ports[i], attr)

            # cleanup acl group, entries, tables
            for grp in acl_group_list:
                self.client.sai_thrift_remove_acl_table_group(grp)
            self.client.sai_thrift_remove_acl_entry(acl_entry)
            self.client.sai_thrift_remove_acl_table(acl_table)
            self.client.sai_thrift_remove_acl_entry(mirror_acl_entry)
            self.client.sai_thrift_remove_acl_table(mirror_acl_table)

            # cleanup mirror sess
            self.client.sai_thrift_remove_mirror_session(span_session)
            # l3 part
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1, rif_id4)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id4, ip_addr1, dmac1)
            self.client.sai_thrift_remove_next_hop(nhop1)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_router_interface(rif_id3)
            self.client.sai_thrift_remove_router_interface(rif_id4)
            self.client.sai_thrift_remove_virtual_router(vr_id)

@group('egress-acl')
@disabled
class EgressL3AclDscp(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
          print "Egress ACL feature not enabled, skipping"
          return
        print '----------------------------------------------------------------------------------------------'
        print "Sending packet ptf_intf 2 -> ptf_intf 1 (192.168.100.100 ---> 172.16.10.1 [id = 105])"

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        L4_DST_PORT = 1000
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '172.16.10.1'
        ip_mask1 = '255.255.255.255'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)

        # send the test packet(s)
        pkt = simple_tcp_packet(eth_dst=router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=64,ip_tos = 200)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=router_mac,
            ip_dst='172.16.10.1',
            ip_src='192.168.100.100',
            tcp_dport = L4_DST_PORT,
            ip_id=105,
            ip_ttl=63,ip_tos = 200)
        try:
            print '#### NO ACL Applied ####'
            print '#### Sending  ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 2'
            send_packet(self,switch_ports[1], str(pkt))
            print '#### Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            verify_packets(self, exp_pkt,[switch_ports[0]])
        finally:
            print '----------------------------------------------------------------------------------------------'

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (192.168.0.1-[acl]-> 172.16.10.1 [id = 105])"
        # setup ACL to block based on Source IP and SPORT
        table_stage = SAI_ACL_STAGE_EGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_ROUTER_INTF]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        dscp=50

        u32range = sai_thrift_range_t(min=1000, max=1200)
        acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, u32range)
        range_list = [acl_range_id]
        print "ACL range created 0x%lx"%(acl_range_id)

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            None,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list, dscp)
        print "ACL Table created 0x%lx"%(acl_table_id)

        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list, dscp)

        # bind this ACL table to rif_id2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_EGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_router_interface_attribute(rif_id1, attr)

        try:
            assert acl_table_id != 0, 'acl_table_id is == 0'
            assert acl_entry_id != 0, 'acl_entry_id is == 0'

            print '#### ACL \'DROP, src ip 192.168.100.1/255.255.255.0, SPORT 1000, in_ports[ptf_intf_1,2]\' Applied ####'
            print '#### Sending      ', router_mac, '| 00:22:22:22:22:22 | 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 1'
            # send the same packet
            send_packet(self,switch_ports[1], str(pkt))
            # ensure packet is dropped
            # check for absence of packet here!
            print '#### NOT Expecting 00:11:22:33:44:55 |', router_mac, '| 172.16.10.1 | 192.168.100.100 | SPORT 1000 | @ ptf_intf 0'
            verify_no_other_packets(self, timeout=1)
        finally:
            # unbind this ACL table from rif_id2s object id
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_EGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_router_interface_attribute(rif_id1, attr)
            # cleanup ACL
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            self.client.sai_thrift_delete_acl_range(acl_range_id)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1, ip_mask1, rif_id1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

@group('acl')
@group('acl-ocp')
class VlanAclTest(sai_base_test.ThriftInterfaceDataPlane):
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
                                ip_src='192.168.100.1',
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

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
        entry_priority = 1
        action = SAI_PACKET_ACTION_DROP
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = "192.168.100.1"
        ip_src_mask = "255.255.255.0"
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None

        acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
            SAI_ACL_STAGE_INGRESS,
            [SAI_ACL_BIND_POINT_TYPE_VLAN],
            SAI_ACL_TABLE_GROUP_TYPE_PARALLEL)

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            SAI_IP_ADDR_FAMILY_IPV4,
            mac_src,
            mac_dst,
            ip_src,
            ip_dst,
            ip_proto,
            None,
            out_ports,
            in_port,
            out_port,
            src_l4_port,
            dst_l4_port,
            range_list)
        print "ACL Table created 0x%lx"%(acl_table_id)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, SAI_IP_ADDR_FAMILY_IPV4,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            None, None,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
            acl_table_group_id,
            acl_table_id,
            100)

        # bind this ACL table to rif_id2s object id
        attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
        attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_vlan_attribute(vlan_oid, attr)

        try:
            send_packet(self, switch_ports[0], str(pkt))
            verify_no_other_packets(self, timeout=1)
        finally:
            sai_thrift_delete_fdb(self.client, vlan_oid, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_oid, mac2, port2)
            attr_value = sai_thrift_attribute_value_t(oid=0)
            attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_vlan_attribute(vlan_oid, attr)
            self.client.sai_thrift_remove_acl_table_group_member(acl_table_group_member_id)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            self.client.sai_thrift_remove_acl_table_group(acl_table_group_id)

            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan_member(vlan_member2)
            self.client.sai_thrift_remove_vlan(vlan_oid)

#####################################################################################################################

'''
This test verifies SAI ACL User Meta behavior. ACL user meta field is set by IP/IPv6 ACL in the ingress stage and
used as key in Egress stage by IP/IPv6 ACL tables.The allowed range of user meta field is defined by the switch attr
SAI_SWITCH_ATTR_ACL_USER_META_DATA_RANGE (Read only attr).
'''
@disabled
@group('egress-acl')
@group('acl-user-meta')
class IPAclUserMetaTest(sai_base_test.ThriftInterfaceDataPlane):
    acl_grp_members= []
    acl_grps= []
    acl_rules = []
    acl_tables = []
    acl_counters = []
    vlan_members = []
    vlans = []
    vlan_ports = []
    bp_list =[]
    vrs = []
    rifs = []
    nhops = []
    routes = {}
    nbrs = {}

    def acl_cleanup(self):
        for bp in list(self.bp_list):
           if bp in self.vlans:
              print "Unbinding ACL Group from Vlan 0x%lx"%(bp)
              attr_value = sai_thrift_attribute_value_t(oid=0)
              attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_INGRESS_ACL, value=attr_value)
              self.client.sai_thrift_set_vlan_attribute(bp, attr)
              attr2 = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_EGRESS_ACL, value=attr_value)
              self.client.sai_thrift_set_vlan_attribute(bp, attr2)
           else:
              print "Unbinding ACL grp from Port 0x%lx"%(bp)
              attr_value = sai_thrift_attribute_value_t(oid=0)
              attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
              self.client.sai_thrift_set_port_attribute(bp, attr)
              attr2 = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_ACL, value=attr_value)
              self.client.sai_thrift_set_port_attribute(bp, attr2)

        for acl_grp_member in list(self.acl_grp_members):
           self.client.sai_thrift_remove_acl_table_group_member(acl_grp_member)
           self.acl_grp_members.remove(acl_grp_member)
        for acl_rule in list(self.acl_rules):
           self.client.sai_thrift_remove_acl_entry(acl_rule)
           self.acl_rules.remove(acl_rule)
        for acl_table in list(self.acl_tables):
           self.client.sai_thrift_remove_acl_table(acl_table)
           self.acl_tables.remove(acl_table)
        for acl_grp in list(self.acl_grps):
           self.client.sai_thrift_remove_acl_table_group(acl_grp)
           self.acl_grps.remove(acl_grp)

    def counter_cleanup(self):
        for acl_counter in list(self.acl_counters):
           self.client.sai_thrift_remove_acl_counter(acl_counter)
           self.acl_counters.remove(acl_counter)

    def cleanup(self):
        for port in list(self.vlan_ports):
            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)
        for vlan_member in list(self.vlan_members):
           self.client.sai_thrift_remove_vlan_member(vlan_member)
           self.vlan_members.remove(vlan_member)
        for vlan in list(self.vlans):
           self.client.sai_thrift_remove_vlan(vlan)
           self.vlans.remove(vlan)
        for (vr, addr_family, ip_addr, ip_mask), nhop in self.routes.items():
           sai_thrift_remove_route(self.client, vr, addr_family, ip_addr, ip_mask, nhop)
           del self.routes[(vr, addr_family, ip_addr, ip_mask)]
        for (addr_family, ip_addr) , (rif_id, dmac) in self.nbrs.items():
           sai_thrift_remove_neighbor(self.client, addr_family, rif_id, ip_addr, dmac)
           del self.nbrs[(addr_family, ip_addr)]
        for nhop in list(self.nhops):
           self.client.sai_thrift_remove_next_hop(nhop)
           self.nhops.remove(nhop)
        for rif in list(self.rifs):
           self.client.sai_thrift_remove_router_interface(rif)
           self.rifs.remove(rif)
        for vr in list(self.vrs):
           self.client.sai_thrift_remove_virtual_router(vr)
           self.vrs.remove(vr)


    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        # L3 ports
        port3 = port_list[2]
        port4 = port_list[3]
        port5 = port_list[4]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        user_meta_range = self.client.sai_thrift_get_switch_attribute_by_id(SAI_SWITCH_ATTR_ACL_USER_META_DATA_RANGE)
        self.assertTrue(user_meta_range.value.u32range.max >= 1023 and user_meta_range.value.u32range.min <= 15,"Test acl user meta range values are outside switch acl meta range [Min:%d, Max%d]"%(user_meta_range.value.u32range.min, user_meta_range.value.u32range.max))

        #Device L2 config
        #Add port 1,2 to Vlan10
        vlan_id=10
        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        self.vlans.append(vlan_oid)
        vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member)
        vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member)
        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.vlan_ports.append(port1)
        self.vlan_ports.append(port2)

        #Device L3 config
        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.vrs.append(vr_id)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port3, 0, v4_enabled, v6_enabled, mac)
        self.rifs.append(rif_id1)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port4, 0, v4_enabled, v6_enabled, mac)
        self.rifs.append(rif_id2)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '172.24.10.1'
        ip_mask1 = '255.255.255.255'
        dmac1 = '00:11:22:33:44:55'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        self.nbrs[(addr_family, ip_addr1)] = (rif_id1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        self.nhops.append(nhop1)
        route_mask = '255.255.255.0'
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1, route_mask, nhop1)
        self.routes[(vr_id, addr_family, ip_addr1, route_mask)] = nhop1

        try:
           # send & verify test packet(s) : IPv4 & IPv6 without any ACL entries
           print "Sending IPv4 Test packets - port 1 -> port 2 [access vlan=10])"

           #Test packet for: Ingress Set Meta 512 -> Egress Match 512, Drop packet
           ipv4_pkt1 = simple_ip_packet(pktlen=100,
                         eth_dst='00:1B:19:00:00:00', ip_src='172.16.10.1')
           exp_ipv4_pkt1 = ipv4_pkt1
           send_packet(self, switch_ports[0], str(ipv4_pkt1))
           verify_packets(self, exp_ipv4_pkt1, [switch_ports[1]])

           #Test packet for: Ingress Set Meta 1023 -> Egress Match 1023, Permit packet
           ipv4_pkt2 = simple_ip_packet(pktlen=100,
                         eth_dst='00:1B:19:00:00:00', ip_src='172.18.10.1')
           exp_ipv4_pkt2 = ipv4_pkt2
           send_packet(self, switch_ports[0], str(ipv4_pkt2))
           verify_packets(self, exp_ipv4_pkt2, [switch_ports[1]])

           #Test packet not matching any IPv4 ACL rules
           ipv4_pkt3 = simple_ip_packet(pktlen=100,
                         eth_dst='00:1B:19:00:00:00', ip_src='172.16.10.2')
           exp_ipv4_pkt3 = ipv4_pkt3
           send_packet(self, switch_ports[0], str(ipv4_pkt3))
           verify_packets(self, exp_ipv4_pkt3, [switch_ports[1]])

           print "Sending IPv6/TCP Test packets - port 1 -> port 2 [access vlan=10])"
           #Test packet for: Ingress Set Meta 511 -> Egress Match 511, Drop packet
           tcpv6_pkt1 = simple_tcpv6_packet(pktlen=100,
                         ipv6_src='2000::1')
           exp_tcpv6_pkt1 = tcpv6_pkt1
           send_packet(self, switch_ports[0], str(tcpv6_pkt1))
           verify_packets(self, exp_tcpv6_pkt1, [switch_ports[1]])

           #Test packet for: Ingress Set Meta 15 -> Egress Match 15, Permit packet
           tcpv6_pkt2 = simple_tcpv6_packet(pktlen=100,
                         ipv6_src='4000::1')
           exp_tcpv6_pkt2 = tcpv6_pkt2
           send_packet(self, switch_ports[0], str(tcpv6_pkt2))
           verify_packets(self, exp_tcpv6_pkt2, [switch_ports[1]])

           #Test packet not matching any IPv4 ACL rules
           tcpv6_pkt3= simple_tcpv6_packet(pktlen=100,
                         ipv6_src='6000::1')
           exp_tcpv6_pkt3 = tcpv6_pkt3
           send_packet(self, switch_ports[0], str(tcpv6_pkt3))
           verify_packets(self, exp_tcpv6_pkt3, [switch_ports[1]])

           '''
           #Not yet supported
           print "Sending IPv4/TCP Test packet (for compound actions) - port 4 -> port 3 [L3 routed])"
           #Test packet for: Ingress redirect to port and Set Meta 64  -> Egress Match 64, Permit packet
           tcpv4_pkt1 = simple_tcp_packet(eth_dst=router_mac,
              eth_src='00:22:22:22:22:22',
              ip_dst='172.24.10.8',
              ip_src='192.168.100.100',
              ip_id=105,
              ip_ttl=64)
           exp_tcpv4_pkt1 = simple_tcp_packet(eth_dst=dmac1,
              eth_src=router_mac,
              ip_dst='172.24.10.8',
              ip_src='192.168.100.100',
              ip_id=105,
              ip_ttl=63)
           send_packet(self, 3, str(tcpv4_pkt1))
           verify_packets(self, exp_tcpv4_pkt1, [2])

           #Test packet for: Ingress redirect to port and Set Meta 63  -> Egress Match 63, Drop packet
           tcpv4_pkt2 = simple_tcp_packet(eth_dst=router_mac,
              eth_src='00:22:22:22:22:22',
              ip_dst='172.24.10.8',
              ip_src='192.168.100.99',
              ip_id=105,
              ip_ttl=64)
           exp_tcpv4_pkt2 = simple_tcp_packet(eth_dst=dmac1,
              eth_src=router_mac,
              ip_dst='172.24.10.8',
              ip_src='192.168.100.99',
              ip_id=105,
              ip_ttl=63)
           send_packet(self, 3, str(tcpv4_pkt2))
           verify_packets(self, exp_tcpv4_pkt2, [2])
           '''
###########################################Start of Engress ACLs################################################

           # Create ACL Table, Group and Rules - Egress (Bind point Vlan)
           table_stage = SAI_ACL_STAGE_EGRESS
           table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
           group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL
           entry_priority = 1000
           drop_action = SAI_PACKET_ACTION_DROP
           fwd_action = SAI_PACKET_ACTION_FORWARD

           # Create Egress ACL GRP
           eg_acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
               table_stage, table_bind_point_list, group_type)
           self.acl_grps.append(eg_acl_table_group_id)

           # Create Egress IPv4 ACL table
           eg_aclv4_table_id = sai_thrift_create_acl_table(self.client,
               table_stage,
               table_bind_point_list,
               SAI_IP_ADDR_FAMILY_IPV4,
               ip_src="192.168.0.1")
           self.acl_tables.append(eg_aclv4_table_id)
           print "Egress IPv4 ACL Table created 0x%lx"%(eg_aclv4_table_id)

           # Add drop ACL entry with user meta 512 to IPv4 ACL Table
           eg_ipv4_512_drop_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=eg_aclv4_table_id)
           self.acl_counters.append(eg_ipv4_512_drop_acl_counter)
           metadata = 512
           if metadata > 0x7fff:
               metadata -=0x10000
           metadata_mask = user_meta_range.value.u32range.max
           if metadata_mask > 0x7fff:
               metadata_mask -=0x10000

           eg_ipv4_512_drop_entry = sai_thrift_create_acl_entry(self.client,
               eg_aclv4_table_id,
               entry_priority,
               action=drop_action, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
               acl_counter_id=eg_ipv4_512_drop_acl_counter, user_metadata=metadata, user_metadata_mask=metadata_mask)
           self.acl_rules.append(eg_ipv4_512_drop_entry)
           print "Egress IPv4 ACL DROP (User Meta: %d) entry created 0x%lx, counter: 0x%lx"%(metadata, eg_ipv4_512_drop_entry, eg_ipv4_512_drop_acl_counter)

           if entry_priority >= 1:
              entry_priority -=1

           # Add permit ACL entry with user meta 1023 to IPv4 ACL Table
           eg_ipv4_1023_permit_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=eg_aclv4_table_id)
           self.acl_counters.append(eg_ipv4_1023_permit_acl_counter)
           metadata = 1023
           if metadata > 0x7fff:
               metadata -=0x10000
           eg_ipv4_1023_permit_entry = sai_thrift_create_acl_entry(self.client,
               eg_aclv4_table_id,
               entry_priority,
               action=fwd_action, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
               acl_counter_id=eg_ipv4_1023_permit_acl_counter, user_metadata=metadata, user_metadata_mask=metadata_mask)
           self.acl_rules.append(eg_ipv4_1023_permit_entry)
           print "Egress IPv4 ACL PERMIT (User Meta: %d) entry created 0x%lx, counter: 0x%lx"%(metadata, eg_ipv4_1023_permit_entry, eg_ipv4_1023_permit_acl_counter)

           if entry_priority >= 1:
              entry_priority -=1

           # Create IPv6 ACL table
           eg_aclv6_table_id = sai_thrift_create_acl_table(self.client,
               table_stage,
               table_bind_point_list,
               SAI_IP_ADDR_FAMILY_IPV6,
               ip_src='2000::1')
           self.acl_tables.append(eg_aclv6_table_id)
           print "Egress IPv6 ACL Table created 0x%lx"%(eg_aclv6_table_id)

           # Add drop ACL entry with user meta 511 to IPv6 ACL Table
           eg_ipv6_511_drop_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=eg_aclv6_table_id)
           self.acl_counters.append(eg_ipv6_511_drop_acl_counter)
           metadata=511
           if metadata > 0x7fff:
               metadata -=0x10000
           eg_ipv6_511_drop_entry = sai_thrift_create_acl_entry(self.client,
               eg_aclv6_table_id,
               entry_priority,
               action=drop_action, addr_family=SAI_IP_ADDR_FAMILY_IPV6,
               acl_counter_id=eg_ipv6_511_drop_acl_counter, user_metadata=metadata, user_metadata_mask=metadata_mask)
           self.acl_rules.append(eg_ipv6_511_drop_entry)
           print "Egress IPv6 ACL DROP (User meta: %d) entry created 0x%lx, counter: 0x%lx"%(metadata, eg_ipv6_511_drop_entry, eg_ipv6_511_drop_acl_counter)

           if entry_priority >= 1:
              entry_priority -=1

           # Add permit ACL entry with user meta 15 to IPv6 ACL Table
           eg_ipv6_15_permit_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=eg_aclv6_table_id)
           self.acl_counters.append(eg_ipv6_15_permit_acl_counter)
           metadata=15
           if metadata > 0x7fff:
               metadata -=0x10000
           eg_ipv6_15_permit_entry = sai_thrift_create_acl_entry(self.client,
               eg_aclv6_table_id,
               entry_priority,
               action=fwd_action, addr_family=SAI_IP_ADDR_FAMILY_IPV6,
               acl_counter_id=eg_ipv6_15_permit_acl_counter, user_metadata=metadata, user_metadata_mask=metadata_mask)
           self.acl_rules.append(eg_ipv6_15_permit_entry)
           print "Egress IPv6 ACL PERMIT (User meta: %d) entry created 0x%lx, counter: 0x%lx"%(metadata, eg_ipv6_15_permit_entry, eg_ipv6_15_permit_acl_counter)

           if entry_priority >= 1:
              entry_priority -=1

           eg_aclv4_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
               eg_acl_table_group_id,
               eg_aclv4_table_id,
               100)
           self.acl_grp_members.append(eg_aclv4_table_group_member_id)

           eg_aclv6_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
               eg_acl_table_group_id,
               eg_aclv6_table_id,
               200)
           self.acl_grp_members.append(eg_aclv6_table_group_member_id)

           '''
           # Unsupported: Compound ACL actions Redirect + Set meta (For corresponding ACL rules in Ingress)
           # Create ACL Table, Group and Rules - Egress (Bind point Port)
           table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]

           # Create Egress ACL GRP for Bind point Port
           eg_acl_table_group_id2 = sai_thrift_create_acl_table_group(self.client,
               table_stage, table_bind_point_list, group_type)
           self.acl_grps.append(eg_acl_table_group_id2)

           # Create Egress IPv4 ACL table
           eg_aclv4_table_id2 = sai_thrift_create_acl_table(self.client,
               table_stage,
               table_bind_point_list,
               SAI_IP_ADDR_FAMILY_IPV4,
               ip_src="192.168.0.1")
           self.acl_tables.append(eg_aclv4_table_id2)
           print "Egress IPv4 ACL Table created 0x%lx"%(eg_aclv4_table_id2)

           # Add permit ACL entry with user meta 64 to IPv4 ACL Table
           eg_ipv4_64_permit_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=eg_aclv4_table_id2)
           self.acl_counters.append(eg_ipv4_64_permit_acl_counter)
           metadata = 64
           if metadata > 0x7fff:
               metadata -=0x10000
           eg_ipv4_64_permit_entry = sai_thrift_create_acl_entry(self.client,
               eg_aclv4_table_id2,
               entry_priority,
               action=fwd_action, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
               acl_counter_id=eg_ipv4_64_permit_acl_counter, user_metadata=metadata, user_metadata_mask=metadata_mask)
           self.acl_rules.append(eg_ipv4_64_permit_entry)
           print "Egress IPv4 ACL Permit (User Meta: %d) entry created 0x%lx, counter: 0x%lx"%(metadata, eg_ipv4_64_permit_entry, eg_ipv4_64_permit_acl_counter)

           if entry_priority >= 1:
              entry_priority -=1

           # Add deny ACL entry with user meta 63 to IPv4 ACL Table
           eg_ipv4_63_drop_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=eg_aclv4_table_id2)
           self.acl_counters.append(eg_ipv4_63_drop_acl_counter)
           metadata = 63
           if metadata > 0x7fff:
               metadata -=0x10000
           eg_ipv4_63_drop_entry = sai_thrift_create_acl_entry(self.client,
               eg_aclv4_table_id2,
               entry_priority,
               action=drop_action, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
               acl_counter_id=eg_ipv4_63_drop_acl_counter, user_metadata=metadata, user_metadata_mask=metadata_mask)
           self.acl_rules.append(eg_ipv4_63_drop_entry)
           print "Egress IPv4 ACL Permit (User Meta: %d) entry created 0x%lx, counter: 0x%lx"%(metadata, eg_ipv4_63_drop_entry, eg_ipv4_63_drop_acl_counter)

           if entry_priority >= 1:
              entry_priority -=1

           eg_aclv4_table_group_member_id2 = sai_thrift_create_acl_table_group_member(self.client,
               eg_acl_table_group_id2,
               eg_aclv4_table_id2,
               300)
           self.acl_grp_members.append(eg_aclv4_table_group_member_id2)
           '''

###########################################End of Engress ACLs################################################

###########################################Start of Ingress ACLs################################################

           # Create ACL Table, Group and Rules - Ingress
           ig_table_stage = SAI_ACL_STAGE_INGRESS
           ig_table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
           ig_entry_priority = 1000
           ig_action = SAI_ACL_ACTION_TYPE_SET_ACL_META_DATA
           ig_group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

           #Forward Action for ACL
           ig_fwd_action = SAI_PACKET_ACTION_FORWARD

           # Create Ingress ACL GRP
           ig_acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
               ig_table_stage,
               ig_table_bind_point_list, ig_group_type)
           self.acl_grps.append(ig_acl_table_group_id)


           # Create Ingress IPv4 ACL table
           ig_aclv4_table_id = sai_thrift_create_acl_table(self.client,
               ig_table_stage,
               ig_table_bind_point_list,
               SAI_IP_ADDR_FAMILY_IPV4,
               ip_src="192.168.0.1")
           self.acl_tables.append(ig_aclv4_table_id)
           print "Ingress IPv4 ACL Table created 0x%lx"%(ig_aclv4_table_id)

           # Add ACL entry (Match - IP SRC:172.16.10.1 Action - Set User Meta 512) to IPv4 ACL Table
           ig_ipv4_512_meta_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=ig_aclv4_table_id)
           self.acl_counters.append(ig_ipv4_512_meta_acl_counter)
           ig_ip_src = '172.16.10.1'
           ig_ip_src_mask = '255.255.255.255'
           ig_metadata = 512
           if ig_metadata > 0x7fff:
               ig_metadata -=0x10000
           ig_ipv4_512_meta_entry = sai_thrift_create_acl_entry(self.client,
               ig_aclv4_table_id,
               ig_entry_priority,
               action=None, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
               ip_src=ig_ip_src, ip_src_mask=ig_ip_src_mask,
               acl_counter_id=ig_ipv4_512_meta_acl_counter, set_user_metadata=ig_metadata)
           self.acl_rules.append(ig_ipv4_512_meta_entry)
           print "Ingress IPv4 ACL (Match - IP SRC:%s Action - Set User Meta: %d) entry created 0x%lx, counter: 0x%lx"%(ig_ip_src, ig_metadata, ig_ipv4_512_meta_entry, ig_ipv4_512_meta_acl_counter)

           if ig_entry_priority >= 1:
              ig_entry_priority -=1


           # Add ACL entry (Match - IP SRC:172.18.10.1 Action - Set User Meta 1023) to IPv4 ACL Table
           ig_ipv4_1023_meta_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=ig_aclv4_table_id)
           self.acl_counters.append(ig_ipv4_1023_meta_acl_counter)
           ig_ip_src = '172.18.10.1'
           ig_ip_src_mask = '255.255.255.255'
           ig_metadata = 1023
           if ig_metadata > 0x7fff:
               ig_metadata -=0x10000
           ig_ipv4_1023_meta_entry = sai_thrift_create_acl_entry(self.client,
               ig_aclv4_table_id,
               ig_entry_priority,
               action=fwd_action, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
               ip_src=ig_ip_src, ip_src_mask=ig_ip_src_mask,
               acl_counter_id=ig_ipv4_1023_meta_acl_counter, set_user_metadata=ig_metadata)
           self.acl_rules.append(ig_ipv4_1023_meta_entry)
           print "Ingress IPv4 ACL (Match - IP SRC:%s Action - Set User Meta: %d) entry created 0x%lx, counter: 0x%lx"%(ig_ip_src, ig_metadata, ig_ipv4_1023_meta_entry, ig_ipv4_1023_meta_acl_counter)

           if ig_entry_priority >= 1:
              ig_entry_priority -=1

           # Create IPv6 ACL table
           ig_aclv6_table_id = sai_thrift_create_acl_table(self.client,
               ig_table_stage,
               ig_table_bind_point_list,
               SAI_IP_ADDR_FAMILY_IPV6,
               ip_src="2000::1")
           self.acl_tables.append(ig_aclv6_table_id)
           print "Ingress IPv6 ACL Table created 0x%lx"%(ig_aclv6_table_id)


           # Add ACL entry (Match - IP SRC:2000::1 Action - Set User Meta 511) to IPv6 ACL Table
           ig_ipv6_511_meta_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=ig_aclv6_table_id)
           self.acl_counters.append(ig_ipv6_511_meta_acl_counter)
           ig_ip_src='2000::1'
           ig_ip_src_mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:0"
           ig_metadata=511
           if ig_metadata > 0x7fff:
               ig_metadata -=0x10000
           ig_ipv6_511_meta_entry = sai_thrift_create_acl_entry(self.client,
               ig_aclv6_table_id,
               ig_entry_priority,
               action=None, addr_family=SAI_IP_ADDR_FAMILY_IPV6,
               ip_src=ig_ip_src, ip_src_mask=ig_ip_src_mask,
               acl_counter_id=ig_ipv6_511_meta_acl_counter, set_user_metadata=ig_metadata)
           self.acl_rules.append(ig_ipv6_511_meta_entry)
           print "Ingress IPv6 ACL (Match - IP SRC:%s Action - Set User Meta: %d) entry created 0x%lx, counter: 0x%lx"%(ig_ip_src, ig_metadata, ig_ipv6_511_meta_entry, ig_ipv6_511_meta_acl_counter)

           if ig_entry_priority >= 1:
              ig_entry_priority -=1

           # Add ACL entry (Match - IP SRC:4000::1 Action - Set User Meta 15) to IPv6 ACL Table
           ig_ipv6_15_meta_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=ig_aclv6_table_id)
           self.acl_counters.append(ig_ipv6_15_meta_acl_counter)
           ig_ip_src='4000::1'
           ig_ip_src_mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:0"
           ig_metadata=15
           if ig_metadata > 0x7fff:
               ig_metadata -=0x10000
           ig_ipv6_15_meta_entry = sai_thrift_create_acl_entry(self.client,
               ig_aclv6_table_id,
               ig_entry_priority,
               action=fwd_action, addr_family=SAI_IP_ADDR_FAMILY_IPV6,
               ip_src=ig_ip_src, ip_src_mask=ig_ip_src_mask,
               acl_counter_id=ig_ipv6_15_meta_acl_counter, set_user_metadata=ig_metadata)
           self.acl_rules.append(ig_ipv6_15_meta_entry)
           print "Ingress IPv6 ACL (Match - IP SRC:%s Action - Set User Meta: %d) entry created 0x%lx, counter: 0x%lx"%(ig_ip_src, ig_metadata, ig_ipv6_15_meta_entry, ig_ipv6_15_meta_acl_counter)

           ig_aclv4_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
               ig_acl_table_group_id,
               ig_aclv4_table_id,
               100)
           self.acl_grp_members.append(ig_aclv4_table_group_member_id)

           ig_aclv6_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
               ig_acl_table_group_id,
               ig_aclv6_table_id,
               200)
           self.acl_grp_members.append(ig_aclv6_table_group_member_id)

           # Create ACL Table, Group and Rules - Ingress (Bind point Port)
           ig_table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
           # Create Ingress ACL GRP
           ig_acl_table_group_id2 = sai_thrift_create_acl_table_group(self.client,
               ig_table_stage,
               ig_table_bind_point_list, ig_group_type)
           self.acl_grps.append(ig_acl_table_group_id2)

           '''
           # Unsupported: Compound ACL actions Redirect + Set meta
           # Create Ingress IPv4 ACL table
           ig_aclv4_table_id2 = sai_thrift_create_acl_table(self.client,
               ig_table_stage,
               ig_table_bind_point_list,
               SAI_IP_ADDR_FAMILY_IPV4,
               ip_src="192.168.0.1")
           self.acl_tables.append(ig_aclv4_table_id2)
           print "Ingress IPv4 ACL Table created 0x%lx"%(ig_aclv4_table_id2)

           # Add ACL entry (Match - IP SRC:192.168.100.100 Action - Set User Meta 64) to IPv4 ACL Table
           ig_ipv4_64_meta_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=ig_aclv4_table_id2)
           self.acl_counters.append(ig_ipv4_64_meta_acl_counter)
           ig_ip_src = '192.168.100.100'
           ig_ip_src_mask = '255.255.255.255'
           ig_metadata = 64
           ig_redirect_action = SAI_ACL_ENTRY_ATTR_ACTION_REDIRECT
           if ig_metadata > 0x7fff:
               ig_metadata -=0x10000
           ig_ipv4_64_meta_entry = sai_thrift_create_acl_entry(self.client,
               ig_aclv4_table_id2,
               ig_entry_priority,
               action=ig_redirect_action, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
               ip_src=ig_ip_src, ip_src_mask=ig_ip_src_mask,
               acl_counter_id=ig_ipv4_64_meta_acl_counter, redirect_oid=port5, set_user_metadata=ig_metadata)
           self.acl_rules.append(ig_ipv4_64_meta_entry)
           print "Ingress IPv4 ACL (Match - IP SRC:%s Action - Set User Meta: %d, redirect to Port 0x%lx) entry created 0x%lx, counter: 0x%lx"%(ig_ip_src, ig_metadata, port5, ig_ipv4_64_meta_entry, ig_ipv4_64_meta_acl_counter)

           if ig_entry_priority >= 1:
              ig_entry_priority -=1


           # Add ACL entry (Match - IP SRC:192.168.100.99 Action - Set User Meta 63) to IPv4 ACL Table
           ig_ipv4_63_meta_acl_counter = sai_thrift_create_acl_counter(
               client=self.client, acl_table_id=ig_aclv4_table_id2)
           self.acl_counters.append(ig_ipv4_63_meta_acl_counter)
           ig_ip_src = '192.168.100.99'
           ig_ip_src_mask = '255.255.255.255'
           ig_metadata = 63
           if ig_metadata > 0x7fff:
               ig_metadata -=0x10000
           ig_ipv4_63_meta_entry = sai_thrift_create_acl_entry(self.client,
               ig_aclv4_table_id2,
               ig_entry_priority,
               action=ig_redirect_action, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
               ip_src=ig_ip_src, ig_ip_src_mask,
               ig_ip_dst, ip_src_mask=ig_ip_dst_mask,
               acl_counter_id=ig_ipv4_63_meta_acl_counter, redirect_oid=port5, set_user_metadata=ig_metadata)
           self.acl_rules.append(ig_ipv4_63_meta_entry)
           print "Ingress IPv4 ACL (Match - IP SRC:%s Action - Set User Meta: %d, redirect to Port 0x%lx) entry created 0x%lx, counter: 0x%lx"%(ig_ip_src, ig_metadata, port5, ig_ipv4_63_meta_entry, ig_ipv4_63_meta_acl_counter)

           if ig_entry_priority >= 1:
              ig_entry_priority -=1

           ig_aclv4_table_group_member_id2 = sai_thrift_create_acl_table_group_member(self.client,
               ig_acl_table_group_id2,
               ig_aclv4_table_id2,
               400)
           self.acl_grp_members.append(ig_aclv4_table_group_member_id2)
           '''
###########################################End of ingress ACLs################################################

           print "Binding ACL grp 0x%lx to Vlan10"%(eg_acl_table_group_id)
           attr_value = sai_thrift_attribute_value_t(oid=eg_acl_table_group_id)
           attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_EGRESS_ACL, value=attr_value)
           self.client.sai_thrift_set_vlan_attribute(self.vlans[0], attr)

           print "Binding ACL grp 0x%lx to Vlan10"%(ig_acl_table_group_id)
           attr_value = sai_thrift_attribute_value_t(oid=ig_acl_table_group_id)
           attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_INGRESS_ACL, value=attr_value)
           self.client.sai_thrift_set_vlan_attribute(self.vlans[0], attr)
           self.bp_list.append(self.vlans[0])

           '''
           # Unsupported: Uncomment when support for Compound ACL actions Redirect + Set meta is added
           print "Binding ACL grp 0x%lx to Port4 (0x%lx) & Port5 (0x%lx)"%(ig_acl_table_group_id2, port4, port5)
           attr_value = sai_thrift_attribute_value_t(oid=ig_acl_table_group_id2)
           attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
           self.client.sai_thrift_set_port_attribute(port4, attr)
           self.client.sai_thrift_set_port_attribute(port5, attr)

           print "Binding ACL grp 0x%lx to Port5 (0x%lx) and Port4(0x%lx)"%(eg_acl_table_group_id2, port5, port4)
           attr_value = sai_thrift_attribute_value_t(oid=eg_acl_table_group_id2)
           attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_EGRESS_ACL, value=attr_value)
           self.client.sai_thrift_set_port_attribute(port4, attr)
           self.client.sai_thrift_set_port_attribute(port5, attr)
           self.bp_list.append(port4)
           self.bp_list.append(port5)
           '''

           # Unsupported: Uncomment when support for Compound ACL actions Redirect + Set meta is added
           #exp_counters = {"eg_ipv4_512":0, "eg_ipv4_1023":0, "eg_ipv6_511":0, "eg_ipv6_15":0, "ig_ipv4_512":0, "ig_ipv4_1023":0, "ig_ipv6_511":0, "ig_ipv6_15":0, "eg_ipv4_64":0, "ig_ipv4_64":0, "eg_ipv4_63":0, "ig_ipv4_63":0}
           exp_counters = {"eg_ipv4_512":0, "eg_ipv4_1023":0, "eg_ipv6_511":0, "eg_ipv6_15":0, "ig_ipv4_512":0, "ig_ipv4_1023":0, "ig_ipv6_511":0, "ig_ipv6_15":0}

           #Initialize counters
           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv4_512_drop_acl_counter)
           exp_counters["eg_ipv4_512"]=counter[0].u64

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv4_1023_permit_acl_counter)
           exp_counters["eg_ipv4_1023"]=counter[0].u64

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv6_511_drop_acl_counter)
           exp_counters["eg_ipv6_511"]=counter[0].u64

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv6_15_meta_acl_counter)
           exp_counters["ig_ipv6_15"]=counter[0].u64

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv4_512_meta_acl_counter)
           exp_counters["ig_ipv4_512"]=counter[0].u64

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv4_1023_meta_acl_counter)
           exp_counters["ig_ipv4_1023"]=counter[0].u64

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv6_511_meta_acl_counter)
           exp_counters["ig_ipv6_511"]=counter[0].u64

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv6_15_permit_acl_counter)
           exp_counters["eg_ipv6_15"]=counter[0].u64



           print "Sending Packets matching ACL rules on VLAN10."

           # send & verify test packet(s) : IPv4 & IPv6 with ACL entries installed
           print "Sending IPv4 Test packet - port 1 -> port 2 [access vlan=10])"
           print "[Test] IPv4 ACL: Ingress Set Meta 512 -> Egress Match 512, Drop packet"
           send_packet(self, switch_ports[0], str(ipv4_pkt1))
           verify_no_other_packets(self, timeout=1)
           exp_counters["eg_ipv4_512"]+=1
           exp_counters["ig_ipv4_512"]+=1
           print "[Test] passed"

           #Test packet for: Ingress Set Meta 1023 -> Egress Match 1023, Permit packet
           print "Sending IPv4 Test packet - port 1 -> port 2 [access vlan=10])"
           print "[Test] IPv4 ACL: Ingress Set Meta 1023 -> Egress Match 1023, Permit packet"
           send_packet(self, switch_ports[0], str(ipv4_pkt2))
           verify_packets(self, exp_ipv4_pkt2, [switch_ports[1]])
           exp_counters["eg_ipv4_1023"]+=1
           exp_counters["ig_ipv4_1023"]+=1
           print "[Test] passed"

           #Test packet not matching any IPv4 ACL rules
           print "Sending IPv4 Test packet - port 1 -> port 2 [access vlan=10])"
           print "[Test] IPv4 ACL: Ingress Miss -> Egress Miss, No ACL Action"
           send_packet(self, switch_ports[0], str(ipv4_pkt3))
           verify_packets(self, exp_ipv4_pkt3, [switch_ports[1]])
           print "[Test] passed"

           #Test packet for: Ingress Set Meta 511 -> Egress Match 511, Drop packet
           print "Sending IPv6/TCP Test packet - port 1 -> port 2 [access vlan=10])"
           print "[Test] IPv6 ACL: Ingress Set Meta 511 -> Egress Match 511, Drop packet"
           send_packet(self, switch_ports[0], str(tcpv6_pkt1))
           verify_no_other_packets(self, timeout=1)
           exp_counters["eg_ipv6_511"]+=1
           exp_counters["ig_ipv6_511"]+=1
           print "[Test] passed"

           #Test packet for: Ingress Set Meta 15 -> Egress Match 15, Permit packet
           print "Sending IPv6/TCP Test packet - port 1 -> port 2 [access vlan=10])"
           print "[Test] IPv6 ACL: Ingress Set Meta 15 -> Egress Match 15, Permit packet"
           send_packet(self, switch_ports[0], str(tcpv6_pkt2))
           verify_packets(self, exp_tcpv6_pkt2, [switch_ports[1]])
           exp_counters["eg_ipv6_15"]+=1
           exp_counters["ig_ipv6_15"]+=1
           print "[Test] passed"

           #Test packet not matching any IPv6 ACL rules
           print "Sending IPv6/TCP Test packet - port 1 -> port 2 [access vlan=10])"
           print "[Test] IPv6 ACL: Ingress Miss -> Egress Miss, No ACL Action"
           send_packet(self, switch_ports[0], str(tcpv6_pkt3))
           verify_packets(self, exp_tcpv6_pkt3, [switch_ports[1]])
           print "[Test] passed"

           '''
           # Unsupported: Uncomment when support for Compound ACL actions Redirect + Set meta is added
           #Test for Compound Actions - Redirect and set user meta data
           print "Sending IPv4 Test packet - port 4 -> port 3"
           print "[Test] IPv4 ACL: Ingress Set Meta 64, Redirect to port5 -> Egress Match 64, Permit packet"
           send_packet(self, 3, str(tcpv4_pkt1))
           verify_packets(self, tcpv4_pkt1, [4])
           exp_counters["eg_ipv4_64"]+=1
           exp_counters["ig_ipv4_64"]+=1
           print "[Test] passed"
           #Test for Compound Actions - Redirect and set user meta data

           print "Sending IPv4 Test packet - port 4 -> port 3"
           print "[Test] IPv4 ACL: Ingress Set Meta 63, Redirect to port5 -> Egress Match 63, Drop packet"
           send_packet(self, 3, str(tcpv4_pkt2))
           verify_no_other_packets(self, timeout=1)
           exp_counters["eg_ipv4_63"]+=1
           exp_counters["ig_ipv4_63"]+=1
           print "[Test] passed"
           '''
           #Verifying ACL counters
           print "Verifying ACL counters"
           time.sleep(5)
           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv4_512_drop_acl_counter)
           self.assertTrue(exp_counters["eg_ipv4_512"]==counter[0].u64,"Egress IPv4 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(eg_ipv4_512_drop_acl_counter, counter[0].u64, exp_counters["eg_ipv4_512"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv4_1023_permit_acl_counter)
           self.assertTrue(exp_counters["eg_ipv4_1023"]==counter[0].u64,"Egress IPv4 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(eg_ipv4_1023_permit_acl_counter, counter[0].u64, exp_counters["eg_ipv4_1023"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv6_511_drop_acl_counter)
           self.assertTrue(exp_counters["eg_ipv6_511"]==counter[0].u64,"Egress IPv6 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(eg_ipv6_511_drop_acl_counter, counter[0].u64, exp_counters["eg_ipv6_511"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv6_15_meta_acl_counter)
           self.assertTrue(exp_counters["ig_ipv6_15"]==counter[0].u64,"Ingress IPv6 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(ig_ipv6_15_meta_acl_counter, counter[0].u64, exp_counters["ig_ipv6_15"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv4_512_meta_acl_counter)
           self.assertTrue(exp_counters["ig_ipv4_512"]==counter[0].u64,"Ingress IPv4 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(ig_ipv4_512_meta_acl_counter, counter[0].u64, exp_counters["ig_ipv4_512"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv4_1023_meta_acl_counter)
           self.assertTrue(exp_counters["ig_ipv4_1023"]==counter[0].u64,"Ingress IPv4 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(ig_ipv4_1023_meta_acl_counter, counter[0].u64, exp_counters["ig_ipv4_1023"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv6_511_meta_acl_counter)
           self.assertTrue(exp_counters["ig_ipv6_511"]==counter[0].u64,"Ingress IPv6 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(ig_ipv6_511_meta_acl_counter, counter[0].u64, exp_counters["ig_ipv6_511"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv6_15_permit_acl_counter)
           self.assertTrue(exp_counters["eg_ipv6_15"]==counter[0].u64,"Egress IPv6 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(eg_ipv6_15_permit_acl_counter, counter[0].u64, exp_counters["eg_ipv6_15"]))

           '''
           # Unsupported: Uncomment when support for Compound ACL actions Redirect + Set meta is added
           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv4_64_permit_acl_counter)
           self.assertTrue(exp_counters["eg_ipv4_64"]==counter[0].u64,"Egress IPv4 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(eg_ipv4_64_permit_acl_counter, counter[0].u64, exp_counters["eg_ipv4_64"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv4_64_meta_acl_counter)
           self.assertTrue(exp_counters["ig_ipv4_64"]==counter[0].u64,"Ingress IPv4 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(ig_ipv4_64_meta_acl_counter, counter[0].u64, exp_counters["ig_ipv4_64"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=eg_ipv4_63_drop_acl_counter)
           self.assertTrue(exp_counters["eg_ipv4_63"]==counter[0].u64,"Egress IPv4 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(eg_ipv4_63_drop_acl_counter, counter[0].u64, exp_counters["eg_ipv4_63"]))

           counter = sai_thrift_get_acl_counter_attribute(
               client=self.client, acl_counter_id=ig_ipv4_63_meta_acl_counter)
           self.assertTrue(exp_counters["ig_ipv4_63"]==counter[0].u64,"Ingress IPv4 ACL Counter Mismatch for counter 0x%lx - Actual:%d Expected:%d"%(ig_ipv4_63_meta_acl_counter, counter[0].u64, exp_counters["ig_ipv4_63"]))
           '''

        finally:
           print '----------------------------------------------------------------------------------------------'
           self.acl_cleanup()
           self.counter_cleanup()
           self.cleanup()


'''
This test mimics SONiC/SAI ACL behavior. SONiC does not have notion of MAC ACL.
It programs entries with non ip eth type match entries into IP ACL. This is different
than the SDE behavior which expects this entries to be programmed into MAC ACL.
This test verifies the SDE behavior to be able to identify such acl rules and
program it correctly in the corresponding H/W ACL tables,
'''
@disabled
@group('sonic-acl')
class IPEthAclTest(sai_base_test.ThriftInterfaceDataPlane):
    traps = []
    acl_grp_members= []
    acl_grps= []
    acl_rules = []
    acl_tables = []
    acl_counters = []
    vlan_members = []
    vlans = []
    default_bridge_ports = []
    rifs = []
    vrs = []
    vlan_ports = []
    mirror_sessions = []
    bp_list = []

    def trap_acl_cleanup(self):
        #Unbind ACL from Vlan10
        for bp in list(self.bp_list):
            if bp in self.vlans:
                print "Unbinding ACL grp from Vlan"
                attr_value = sai_thrift_attribute_value_t(oid=0)
                attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_vlan_attribute(bp, attr)
            else:
                print "Unbinding ACL grp from Port"
                attr_value = sai_thrift_attribute_value_t(oid=0)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(bp, attr)
            self.bp_list.remove(bp)
        for trap in list(self.traps):
            sai_thrift_remove_hostif_trap(self.client, trap)
            self.traps.remove(trap)
        for acl_grp_member in list(self.acl_grp_members):
            self.client.sai_thrift_remove_acl_table_group_member(acl_grp_member)
            self.acl_grp_members.remove(acl_grp_member)
        for acl_rule in list(self.acl_rules):
            self.client.sai_thrift_remove_acl_entry(acl_rule)
            self.acl_rules.remove(acl_rule)
        for acl_table in list(self.acl_tables):
            self.client.sai_thrift_remove_acl_table(acl_table)
            self.acl_tables.remove(acl_table)
        for acl_grp in list(self.acl_grps):
            self.client.sai_thrift_remove_acl_table_group(acl_grp)
            self.acl_grps.remove(acl_grp)

    def counter_cleanup(self):
        for acl_counter in list(self.acl_counters):
            self.client.sai_thrift_remove_acl_counter(acl_counter)

    def cleanup(self):
        for mirror_session in list(self.mirror_sessions):
            self.client.sai_thrift_remove_mirror_session(mirror_session)
        vlan_attr_value = sai_thrift_attribute_value_t(u16=1)
        vlan_attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=vlan_attr_value)
        for vlan_port in list(self.vlan_ports):
            self.client.sai_thrift_set_port_attribute(vlan_port, vlan_attr)
        for vlan_member in list(self.vlan_members):
            self.client.sai_thrift_remove_vlan_member(vlan_member)
            self.vlan_members.remove(vlan_member)
        for vlan in list(self.vlans):
            self.client.sai_thrift_remove_vlan(vlan)
            self.vlans.remove(vlan)
        for rif in list(self.rifs):
            self.client.sai_thrift_remove_router_interface(rif)
            self.rifs.remove(rif)
        for vr in list(self.vrs):
            self.client.sai_thrift_remove_virtual_router(vr)
            self.vrs.remove(vr)

    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        port5 = port_list[4]

        v4_enabled = 1
        v6_enabled = 1
        mac = ''

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.vrs.append(vr_id)

        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id,  SAI_ROUTER_INTERFACE_TYPE_PORT, port5, 0, v4_enabled, v6_enabled, mac)
        self.rifs.append(rif_id1)

        #Add port 1,2 to Vlan10
        vlan_oid = sai_thrift_create_vlan(self.client, 10)
        self.vlans.append(vlan_oid)
        vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member)
        self.vlan_ports.append(port1)
        vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, port2, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member)
        self.vlan_ports.append(port2)

        attr_value = sai_thrift_attribute_value_t(u16=10)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)

        #Add port 3,4 to Vlan20
        vlan_oid = sai_thrift_create_vlan(self.client, 20)
        self.vlans.append(vlan_oid)
        vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, port3, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member)
        self.vlan_ports.append(port3)
        vlan_member = sai_thrift_create_vlan_member(self.client, vlan_oid, port4, SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member)
        self.vlan_ports.append(port4)

        attr_value = sai_thrift_attribute_value_t(u16=20)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port3, attr)
        self.client.sai_thrift_set_port_attribute(port4, attr)

        try:
            print "Sending ARP packet - port 1 -> port 2 [access vlan=10])"
            print "Sending ARP packet - port 3 -> port 4 [access vlan=20])"
            # send & verify test packet(s) : ARP, IPX, IPv4, LLDP, IPv6 without any ACL entries
            arp_pkt = simple_arp_packet(arp_op=1,pktlen=100)
            exp_arp_pkt = arp_pkt
            send_packet(self, 0, str(arp_pkt))
            verify_packets(self, exp_arp_pkt, [1])
            send_packet(self, 2, str(arp_pkt))
            verify_packets(self, exp_arp_pkt, [3])

            print "Sending IPX packet - port 1 -> port 2 [access vlan=10])"
            print "Sending IPX packet - port 3 -> port 4 [access vlan=20])"
            ipx_pkt = simple_eth_packet(pktlen=100,
                  eth_dst='00:01:02:03:04:05',
                  eth_src='00:06:07:08:09:0a',
                  eth_type=0x8137)
            exp_ipx_pkt = ipx_pkt
            send_packet(self, 0, str(ipx_pkt))
            verify_packets(self, exp_ipx_pkt, [1])
            send_packet(self, 2, str(ipx_pkt))
            verify_packets(self, exp_ipx_pkt, [3])


            print "Sending IPv4 packet - port 1 -> port 2 [access vlan=10])"
            print "Sending IPv4 packet - port 3 -> port 4 [access vlan=20])"
            ipv4_pkt = simple_ip_packet(pktlen=100,
                  eth_dst='00:1B:19:00:00:00')
            exp_ipv4_pkt = ipv4_pkt
            send_packet(self, 0, str(ipv4_pkt))
            verify_packets(self, exp_ipv4_pkt, [1])
            send_packet(self, 2, str(ipv4_pkt))
            verify_packets(self, exp_ipv4_pkt, [3])

            print "Sending LLDP packet - port 1 -> port 2 [access vlan=10])"
            print "Sending LLDP packet - port 3 -> port 4 [access vlan=20])"
            lldp_pkt = simple_eth_packet(pktlen=100,
                  eth_dst='01:80:C2:00:00:0E',
                  eth_type=0x88cc)
            exp_lldp_pkt = lldp_pkt
            send_packet(self, 0, str(lldp_pkt))
            verify_packets(self, exp_lldp_pkt, [1])
            send_packet(self, 2, str(lldp_pkt))
            verify_packets(self, exp_lldp_pkt, [3])

            print "Sending IPv6/TCP(SRC_IP 2000::4) packet - port 1 -> port 2 [access vlan=10])"
            print "Sending IPv6/TCP(SRC_IP 2000::4) packet - port 3 -> port 4 [access vlan=20])"
            tcpv6_pkt = simple_tcpv6_packet(pktlen=100,
                  ipv6_src='2000::4')
            exp_tcpv6_pkt = tcpv6_pkt
            send_packet(self, 0, str(tcpv6_pkt))
            verify_packets(self, exp_tcpv6_pkt, [1])
            send_packet(self, 2, str(tcpv6_pkt))
            verify_packets(self, exp_tcpv6_pkt, [3])

            print "Sending IPv4/TCP packet(SRC_IP 172.0.0.17) - port 1 -> port 2 [access vlan=10])"
            print "Sending IPv4/TCP packet(SRC_IP 172.0.0.17) - port 3 -> port 4 [access vlan=20])"
            tcpv4_pkt = simple_tcp_packet(pktlen=100,
                  ip_src='172.0.0.17')
            exp_tcpv4_pkt = tcpv4_pkt
            send_packet(self, 0, str(tcpv4_pkt))
            verify_packets(self, exp_tcpv4_pkt, [1])
            send_packet(self, 2, str(tcpv4_pkt))
            verify_packets(self, exp_tcpv4_pkt, [3])

            print "Sending IPv6/TCP packet(SRC_IP 3000::1) - port 1 -> port 2 [access vlan=10])"
            print "Sending IPv6/TCP packet(SRC_IP 3000::1) - port 3 -> port 4 [access vlan=20])"
            tcpv6_pkt2 = simple_tcpv6_packet(pktlen=100,
                  ipv6_src='3000::1')
            exp_tcpv6_pkt2 = tcpv6_pkt2
            send_packet(self, 0, str(tcpv6_pkt2))
            verify_packets(self, exp_tcpv6_pkt2, [1])
            send_packet(self, 2, str(tcpv6_pkt2))
            verify_packets(self, exp_tcpv6_pkt2, [3])

            print "Sending IPv6/UDP packet - port 1 -> port 2 [access vlan=10])"
            print "Sending IPv6/UDP packet - port 3 -> port 4 [access vlan=20])"
            udpv6_pkt = simple_udpv6_packet(pktlen=100, ipv6_src='5000::1')
            exp_udpv6_pkt = udpv6_pkt
            send_packet(self, 0, str(udpv6_pkt))
            verify_packets(self, exp_udpv6_pkt, [1])
            send_packet(self, 2, str(udpv6_pkt))
            verify_packets(self, exp_udpv6_pkt, [3])

            print "Sending IPv4/UDP packet - port 1 -> port 2 [access vlan=10])"
            print "Sending IPv4/UDP packet - port 3 -> port 4 [access vlan=20])"
            udpv4_pkt = simple_udp_packet(pktlen=100, ip_src='10.0.0.1')
            exp_udpv4_pkt = udpv4_pkt
            send_packet(self, 0, str(udpv4_pkt))
            verify_packets(self, exp_udpv4_pkt, [1])
            send_packet(self, 2, str(udpv4_pkt))
            verify_packets(self, exp_udpv4_pkt, [3])

            print "Sending EthType(0x1234) packet - port 1 -> port 2 [access vlan=10])"
            print "Sending EthType(0x1234) packet - port 3 -> port 4 [access vlan=20])"
            mac_pkt = simple_eth_packet(pktlen=100,
                  eth_dst='00:01:02:03:04:05',
                  eth_src='00:06:07:08:09:0a',
                  eth_type=0x1234)
            exp_mac_pkt = mac_pkt
            send_packet(self, 0, str(mac_pkt))
            verify_packets(self, exp_mac_pkt, [1])
            send_packet(self, 2, str(mac_pkt))
            verify_packets(self, exp_mac_pkt, [3])

            # Used for negative test cases verification
            print "Sending EthType(0x4321) packet - port 1 -> port 2 [access vlan=10])"
            print "Sending EthType(0x4321) packet - port 3 -> port 4 [access vlan=20])"
            mac_pkt2 = simple_eth_packet(pktlen=100,
                  eth_dst='00:01:02:03:04:05',
                  eth_src='00:06:07:08:09:0a',
                  eth_type=0x4321)
            exp_mac_pkt2 = mac_pkt2
            send_packet(self, 0, str(mac_pkt2))
            verify_packets(self, exp_mac_pkt2, [1])
            send_packet(self, 2, str(mac_pkt2))
            verify_packets(self, exp_mac_pkt2, [3])

            # Create ACL Table, Group and Rules
            table_stage = SAI_ACL_STAGE_INGRESS
            table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_VLAN]
            entry_priority = 1000
            action = SAI_PACKET_ACTION_DROP
            eth_type = 0x0806
            addr_family = SAI_IP_ADDR_FAMILY_IPV4

            vlan_acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                table_stage,
                table_bind_point_list,
                SAI_ACL_TABLE_GROUP_TYPE_PARALLEL)
            self.acl_grps.append(vlan_acl_table_group_id)
            print "VLAN ACL Table Group created 0x%lx"%(vlan_acl_table_group_id)

            port_acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                table_stage,
                [SAI_ACL_BIND_POINT_TYPE_PORT],
                SAI_ACL_TABLE_GROUP_TYPE_PARALLEL)
            self.acl_grps.append(port_acl_table_group_id)
            print "VLAN ACL Table Group created 0x%lx"%(port_acl_table_group_id)

            aclv4_table_id = sai_thrift_create_acl_table(self.client,
                table_stage,
                table_bind_point_list,
                addr_family,
                ip_src = '10.10.10.1')
            self.acl_tables.append(aclv4_table_id)

            print "IPv4 ACL Table created 0x%lx"%(aclv4_table_id)

            # Add ARP drop ACL entry to IPv4 ACL Table
            arp_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=aclv4_table_id)
            self.acl_counters.append(arp_acl_counter)
            arp_drop_entry = sai_thrift_create_acl_entry(self.client,
                aclv4_table_id,
                entry_priority,
                action, addr_family,
                ether_type = eth_type, acl_counter_id=arp_acl_counter)
            self.acl_rules.append(arp_drop_entry)
            print "ARP ACL DROP entry created 0x%lx"%(arp_drop_entry)

            if entry_priority >= 1:
               entry_priority -= 1

            # Add IPX drop ACL entry to IPv4 ACL Table
            ipx_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=aclv4_table_id)
            self.acl_counters.append(ipx_acl_counter)
            eth_type = 0x8137
            if eth_type > 0x7fff:
                eth_type -=0x10000
            ipx_drop_entry = sai_thrift_create_acl_entry(self.client,
                aclv4_table_id,
                entry_priority,
                action, addr_family,
                ether_type = eth_type, acl_counter_id=ipx_acl_counter)
            self.acl_rules.append(ipx_drop_entry)
            print "IPX ACL DROP entry created 0x%lx"%(ipx_drop_entry)

            if entry_priority >= 1:
                entry_priority -= 1

            # Add drop all IPv4 ACL entry to IPv4 ACL Table
            ipv4_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=aclv4_table_id)
            self.acl_counters.append(ipv4_acl_counter)
            eth_type = 0x0800
            if eth_type > 0x7fff:
                eth_type -=0x10000
            ipv4_drop_entry = sai_thrift_create_acl_entry(self.client,
                aclv4_table_id,
                entry_priority,
                action, addr_family,
                ether_type = eth_type, acl_counter_id=ipv4_acl_counter)
            self.acl_rules.append(ipv4_drop_entry)
            print "IPv4 ACL DROP (catch all) entry created 0x%lx"%(ipv4_drop_entry)

            if entry_priority >= 1:
               entry_priority -= 1

            ip_src='2000::1'
            # During ACL table create match key fiedls are just used to select the H/W table this SAI table
            # needs to be mapped to. Adding dummy ip src entry below, to allow helper sai api to correclty
            # pass down the list of attrs to sai C api
            # Create IPv6 ACL table
            aclv6_table_id = sai_thrift_create_acl_table(self.client,
                table_stage,
                table_bind_point_list,
                SAI_IP_ADDR_FAMILY_IPV6,
                ip_src = ip_src)
            self.acl_tables.append(aclv6_table_id)
            print "IPv6 ACL Table created 0x%lx"%(aclv6_table_id)

            # Add LLDP drop ACL entry to IPv6 ACL Table
            lldp_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=aclv6_table_id)
            self.acl_counters.append(lldp_acl_counter)
            eth_type = 0x88cc
            ip_src=None
            if eth_type > 0x7fff:
                eth_type -=0x10000
            lldp_drop_entry = sai_thrift_create_acl_entry(self.client,
                aclv6_table_id,
                entry_priority,
                action, SAI_IP_ADDR_FAMILY_IPV6,
                ether_type = eth_type, acl_counter_id=lldp_acl_counter)
            self.acl_rules.append(lldp_drop_entry)
            print "Installing user-acl-rule to drop LLDP 0x%lx"%(lldp_drop_entry)

            if entry_priority >= 1:
                entry_priority -= 1

            # Add IPv6 drop ACL entry based on SRC_IPv6 Addr to IPv6 ACL Table
            ipv6_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=aclv6_table_id)
            self.acl_counters.append(ipv6_acl_counter)
            ip_src = "2000::1"
            ip_src_mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:0"
            eth_type=0x86dd
            if eth_type > 0x7fff:
                eth_type -=0x10000
            ipv6_drop_entry = sai_thrift_create_acl_entry(self.client,
                aclv6_table_id,
                entry_priority,
                action, SAI_IP_ADDR_FAMILY_IPV6,
                ether_type = eth_type, acl_counter_id=ipv6_acl_counter)
            self.acl_rules.append(ipv6_drop_entry)
            print "IPv6 ACL DROP entry created for SRC_IP:2000::0:x 0x%lx"%(ipv6_drop_entry)

            if entry_priority >= 1:
                entry_priority -= 1

            #Setup Mirror ACL
            monitor_port=port5
            mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
            ingress_mirror_id=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
            self.mirror_sessions.append(ingress_mirror_id)
            print 'Mirror session created 0x%lx'%ingress_mirror_id

            # Create Mirror ACL table
            mac_src=None
            mac_dst=None
            ip_src="172.0.0.17"
            ip_dst=None
            ip_proto=None
            in_ports=None
            out_ports=None
            in_port=None
            out_port=None
            src_l4_port=None
            dst_l4_port=None
            range_list=None
            dscp=1
            eth_type=None
            table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
            mirror_table_id = sai_thrift_create_acl_table(self.client,
                table_stage,
                table_bind_point_list,
                SAI_IP_ADDR_FAMILY_IPV4,
                dscp = dscp)
            self.acl_tables.append(mirror_table_id)

            # Add IPV4 SRC IP Mirror ACL entry to Mirror ACL Table
            v4src_mirror_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=mirror_table_id)
            self.acl_counters.append(v4src_mirror_acl_counter)
            ip_src="172.0.0.17"
            ip_src_mask="255.255.255.255"
            dscp=None
            action=None
            v4src_mirror_entry = sai_thrift_create_acl_entry(self.client,
                mirror_table_id,
                entry_priority,
                action, SAI_IP_ADDR_FAMILY_IPV4,
                ip_src=ip_src, ip_src_mask=ip_src_mask,
                ingress_mirror=ingress_mirror_id,
                acl_counter_id=v4src_mirror_acl_counter)
            self.acl_rules.append(v4src_mirror_entry)
            print "v4src ACL Mirror entry created 0x%lx"%(v4src_mirror_entry)

            if entry_priority >= 1:
                entry_priority -= 1

            # Add IPV6 SRC IP Mirror ACL entry to Mirror ACL Table
            v6src_mirror_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=mirror_table_id)
            ip_src = "3000::1"
            ip_src_mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:0"
            dscp=None
            self.acl_counters.append(v6src_mirror_acl_counter)

            v6src_mirror_entry = sai_thrift_create_acl_entry(self.client,
                mirror_table_id,
                entry_priority,
                action, SAI_IP_ADDR_FAMILY_IPV6,
                ip_src=ip_src, ip_src_mask = ip_src_mask,
                ingress_mirror = ingress_mirror_id,
                acl_counter_id=v6src_mirror_acl_counter)
            self.acl_rules.append(v6src_mirror_entry)
            print "v6src ACL Mirror entry created 0x%lx"%(v6src_mirror_entry)

            # Add UDP (IPv4/IPv6 common) ACL entry to Mirror ACL Table
            udp_mirror_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=mirror_table_id)
            dscp=None
            ip_src = None
            ip_src_mask = None
            ip_proto = 0x11
            self.acl_counters.append(udp_mirror_acl_counter)
            udp_mirror_entry = sai_thrift_create_acl_entry(self.client,
                mirror_table_id,
                entry_priority,
                action, SAI_IP_ADDR_FAMILY_IPV4,
                ip_proto=ip_proto,
                ingress_mirror=ingress_mirror_id,
                acl_counter_id=udp_mirror_acl_counter)
            self.acl_rules.append(udp_mirror_entry)
            print "TCP ACL Mirror entry created 0x%lx"%(udp_mirror_entry)

            if entry_priority >= 1:
               entry_priority -=1


            ## Add Eth type MAC ACL entry to Mirror ACL Table
            mac_mirror_acl_counter = sai_thrift_create_acl_counter(
                client=self.client, acl_table_id=mirror_table_id)
            dscp=None
            ip_src = None
            ip_src_mask = None
            ip_proto = None
            eth_type=0x1234
            if eth_type > 0x7fff:
                eth_type -=0x10000
            self.acl_counters.append(mac_mirror_acl_counter)
            mac_mirror_entry = sai_thrift_create_acl_entry(self.client,
                mirror_table_id,
                entry_priority,
                action, SAI_IP_ADDR_FAMILY_IPV4,
                ingress_mirror=ingress_mirror_id,
                ether_type=eth_type, acl_counter_id=mac_mirror_acl_counter)
            self.acl_rules.append(mac_mirror_entry)
            print "MAC ACL Mirror entry created 0x%lx"%(mac_mirror_entry)

            if entry_priority >= 1:
               entry_priority -=1

            aclv4_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
            vlan_acl_table_group_id,
            aclv4_table_id,
            100)
            self.acl_grp_members.append(aclv4_table_group_member_id)

            aclv6_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                vlan_acl_table_group_id,
                aclv6_table_id,
                200)
            self.acl_grp_members.append(aclv6_table_group_member_id)

            mirror_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                port_acl_table_group_id,
                mirror_table_id,
                300)
            self.acl_grp_members.append(mirror_table_group_member_id)

            print "Binding ACL grp 0x%lx to Vlan10"%(vlan_acl_table_group_id)
            ## bind ACL GRP to VLAN 10
            attr_value = sai_thrift_attribute_value_t(oid=vlan_acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_vlan_attribute(self.vlans[0], attr)
            self.bp_list.append(self.vlans[0])

            print "Binding ACL grp 0x%lx to Port3"%(port_acl_table_group_id)
            ## bind ACL GRP to VLAN 10
            attr_value = sai_thrift_attribute_value_t(oid=port_acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port3, attr)
            self.bp_list.append(port3)

            # Create LLDP trap entry
            lldp_trap = sai_thrift_create_hostif_trap(client=self.client,
                             trap_type=SAI_HOSTIF_TRAP_TYPE_LLDP,
                             packet_action=SAI_PACKET_ACTION_TRAP)
            self.traps.append(lldp_trap)
            print "Installing LLDP hostif trap rule 0x%lx"%(lldp_trap)

            exp_counters = {"arp":0, "ipx":0, "ipv4":0, "lldp":0, "ipv6":0, "mirror_ip4":0, "mirror_ip6":0, "mirror_udp":0, "mirror_mac":0}

            # Initialize Counters
            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=arp_acl_counter)
            exp_counters["arp"] = counter[0].u64

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=ipx_acl_counter)
            exp_counters["ipx"] = counter[0].u64

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=ipv4_acl_counter)
            exp_counters["ipv4"] = counter[0].u64

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=lldp_acl_counter)
            exp_counters["lldp"] = counter[0].u64

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=ipv6_acl_counter)
            exp_counters["ipv6"] = counter[0].u64

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=v4src_mirror_acl_counter)
            exp_counters["mirror_ip4"] = counter[0].u64

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=v6src_mirror_acl_counter)
            exp_counters["mirror_ip6"] = counter[0].u64

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=udp_mirror_acl_counter)
            exp_counters["mirror_udp"] = counter[0].u64

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=mac_mirror_acl_counter)
            exp_counters["mirror_mac"] = counter[0].u64

            print "Sending Packets matching ACL rules on VLAN10. Packets should be dropped"
            print "Sending ARP packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(arp_pkt))
            verify_no_other_packets(self, timeout=1)
            exp_counters["arp"]+=1

            print "Sending IPX packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(ipx_pkt))
            verify_no_other_packets(self, timeout=1)
            exp_counters["ipx"]+=1

            print "Sending IPv4 packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(ipv4_pkt))
            verify_no_other_packets(self, timeout=1)
            exp_counters["ipv4"]+=1

            #  User ACL entries will have higher priority than hostif trap entries
            print "Have Hostif entry to trap LLDP, have User-acl to drop LLDP ingressing on port 1"
            print "User-acl entry must take priority over hostif entry"
            print "TX LLDP packet - port 1 -> drop"
            send_packet(self, 0, str(lldp_pkt))
            verify_no_other_packets(self, timeout=1)
            exp_counters["lldp"]+=1

            print "Sending TCPv6 packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(tcpv6_pkt))
            verify_no_other_packets(self, timeout=1)
            exp_counters["ipv6"]+=1

            print "Sending EthType(0x4321) packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(mac_pkt2))
            verify_packets(self, exp_mac_pkt2, [1])

            print "Sending Packets matching ACL rules on VLAN20. Packets should not be dropped"
            print "Sending ARP packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(arp_pkt))
            verify_packets(self, exp_arp_pkt, [3])

            print "Sending IPX packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(ipx_pkt))
            verify_packets(self, exp_ipx_pkt, [3])

            print "Sending IPv4 packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(ipv4_pkt))
            verify_packets(self, exp_ipv4_pkt, [3])

            #This packet will be trapped to the cpu
            print "Sending LLDP packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(lldp_pkt))
            try:
               verify_packets(self, exp_lldp_pkt, [64], timeout=1)
            except:
               pass

            print "Sending TCPv6 packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(tcpv6_pkt))
            verify_packets(self, exp_tcpv6_pkt, [3])

            print "Sending Packets matching Mirror ACL rules on Port3. Packets should be mirrored"
            print "Sending TCPv4 (SRC IP 172.0.0.17) packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(tcpv4_pkt))
            verify_packets(self, exp_tcpv4_pkt, [3, 4])
            exp_counters["mirror_ip4"]+=1

            print "Sending TCPv6 (SRC IP 3000::1) packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(tcpv6_pkt2))
            verify_packets(self, exp_tcpv6_pkt2, [3, 4])
            exp_counters["mirror_ip6"]+=1

            print "Sending UDPv6  packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(udpv6_pkt))
            verify_packets(self, exp_udpv6_pkt, [3, 4])
            exp_counters["mirror_udp"]+=1

            print "Sending UDPv4  packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(udpv4_pkt))
            verify_packets(self, exp_udpv4_pkt, [3, 4])
            exp_counters["mirror_udp"]+=1

            print "Sending EthType(0x1234) packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(mac_pkt))
            verify_packets(self, exp_mac_pkt, [3, 4])
            exp_counters["mirror_mac"]+=1

            print "Sending EthType(0x4321) packet - port 2 -> port 3 [access vlan=20])"
            send_packet(self, 2, str(mac_pkt2))
            verify_packets(self, exp_mac_pkt2, [3])

            #Verifying ACL counters
            print "Verifying ACL counters"
            time.sleep(10)
            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=arp_acl_counter)
            self.assertTrue(exp_counters["arp"]==counter[0].u64,"ARP ACL DROP Counter Mismatch")
            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=ipx_acl_counter)
            self.assertTrue(exp_counters["ipx"]==counter[0].u64,"IPX ACL DROP Counter Mismatch")

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=ipv4_acl_counter)
            self.assertTrue(exp_counters["ipv4"]==counter[0].u64,"IPv4 ACL DROP Counter Mismatch")

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=lldp_acl_counter)
            self.assertTrue(exp_counters["lldp"]==counter[0].u64,"LLDP ACL DROP Counter Mismatch")

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=ipv6_acl_counter)
            self.assertTrue(exp_counters["ipv6"]==counter[0].u64,"IPv6 SRC_IP ACL DROP Counter Mismatch")

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=v4src_mirror_acl_counter)
            self.assertTrue(exp_counters["mirror_ip4"]==counter[0].u64,"IPv4 SRC_IP ACL Mirror Counter Mismatch")

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=v6src_mirror_acl_counter)
            self.assertTrue(exp_counters["mirror_ip6"]==counter[0].u64,"IPv6 SRC_IP ACL Mirror Counter Mismatch")

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=udp_mirror_acl_counter)
            self.assertTrue(exp_counters["mirror_udp"]==counter[0].u64,"UDP ACL Mirror Counter Mismatch")

            counter = sai_thrift_get_acl_counter_attribute(
                client=self.client, acl_counter_id=mac_mirror_acl_counter)
            self.assertTrue(exp_counters["mirror_mac"]==counter[0].u64,"MAC ACL Mirror Counter Mismatch")
            self.trap_acl_cleanup()

            print "Verifying ACL rules were correctly removed"
            print "Sending Packets matching ACL rules on VLAN10. Packets should not be dropped"
            print "Sending ARP packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(arp_pkt))
            verify_packets(self, exp_arp_pkt, [1])

            print "Sending IPX packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(ipx_pkt))
            verify_packets(self, exp_ipx_pkt, [1])

            print "Sending IPv4 packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(ipv4_pkt))
            verify_packets(self, exp_ipv4_pkt, [1])

            print "Sending LLDP packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(lldp_pkt))
            verify_packets(self, exp_lldp_pkt, [1])

            print "Sending TCPv6 packet - port 1 -> port 2 [access vlan=10])"
            send_packet(self, 0, str(tcpv6_pkt))
            verify_packets(self, exp_tcpv6_pkt, [1])

            print "Sending Packets matching ACL rules on VLAN20. Packets should not be dropped"
            print "Sending ARP packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(arp_pkt))
            verify_packets(self, exp_arp_pkt, [3])

            print "Sending IPX packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(ipx_pkt))
            verify_packets(self, exp_ipx_pkt, [3])

            print "Sending IPv4 packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(ipv4_pkt))
            verify_packets(self, exp_ipv4_pkt, [3])

            print "Sending LLDP packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(lldp_pkt))
            verify_packets(self, exp_lldp_pkt, [3])

            print "Sending TCPv6 packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(tcpv6_pkt))
            verify_packets(self, exp_tcpv6_pkt, [3])

            print "Sending Packets matching Mirror ACL rules on Port3. Packets should not be mirrored"
            print "Sending TCPv4 (SRC IP 172.0.0.17) packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(tcpv4_pkt))
            verify_packets(self, exp_tcpv4_pkt, [3])

            print "Sending TCPv6 (SRC IP 3000::1) packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(tcpv6_pkt2))
            verify_packets(self, exp_tcpv6_pkt2, [3])

            print "Sending UDPv6  packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(udpv6_pkt))
            verify_packets(self, exp_udpv6_pkt, [3])

            print "Sending UDPv4  packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(udpv4_pkt))
            verify_packets(self, exp_udpv4_pkt, [3])

            print "Sending EthType(0x1234) packet - port 3 -> port 4 [access vlan=20])"
            send_packet(self, 2, str(mac_pkt))
            verify_packets(self, exp_mac_pkt, [3])
        finally:
            print '----------------------------------------------------------------------------------------------'
            self.trap_acl_cleanup()
            self.counter_cleanup()
            self.cleanup()

@disabled
@group('acl-redirect')
@group('sonic-acl')
class AclRedirectTest(sai_base_test.ThriftInterfaceDataPlane):
    acl_grp_members= []
    acl_grps= []
    acl_rules = []
    acl_tables = []
    vlan_members = []
    vlans = []
    vlan_ports = []
    bp_list = []
    fdb = {}
    lags = []
    lag_members = []
    vrs = []
    rifs = []

    def acl_cleanup(self):
       for bp in list(self.bp_list):
           attr_value = sai_thrift_attribute_value_t(oid=0)
           attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
           self.client.sai_thrift_set_port_attribute(bp, attr)
           self.bp_list.remove(bp)
       for acl_grp_member in list(self.acl_grp_members):
          self.client.sai_thrift_remove_acl_table_group_member(acl_grp_member)
          self.acl_grp_members.remove(acl_grp_member)
       for acl_rule in list(self.acl_rules):
          self.client.sai_thrift_remove_acl_entry(acl_rule)
          self.acl_rules.remove(acl_rule)
       for acl_table in list(self.acl_tables):
          self.client.sai_thrift_remove_acl_table(acl_table)
          self.acl_tables.remove(acl_table)
       for acl_grp in list(self.acl_grps):
          self.client.sai_thrift_remove_acl_table_group(acl_grp)
          self.acl_grps.remove(acl_grp)

    def cleanup(self):
       for mac in self.fdb.keys():
           vlan, port = self.fdb.get(mac)
           sai_thrift_delete_fdb(self.client, vlan, mac, port)
           del self.fdb[mac]
       for vlan_member in list(self.vlan_members):
           self.client.sai_thrift_remove_vlan_member(vlan_member)
           self.vlan_members.remove(vlan_member)
       vlan_attr_value = sai_thrift_attribute_value_t(u16=1)
       vlan_attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=vlan_attr_value)
       for vlan_port in list(self.vlan_ports):
           self.client.sai_thrift_set_port_attribute(vlan_port, vlan_attr)
       for lag_member in list(self.lag_members):
           sai_thrift_remove_lag_member(self.client, lag_member)
           self.lag_members.remove(lag_member)
       for rif in list(self.rifs):
           self.client.sai_thrift_remove_router_interface(rif)
           self.rifs.remove(rif)
       for lag in list(self.lags):
           sai_thrift_remove_lag(self.client, lag)
           self.lags.remove(lag)
       for vlan in list(self.vlans):
           self.client.sai_thrift_remove_vlan(vlan)
           self.vlans.remove(vlan)
       for vr in list(self.vrs):
           self.client.sai_thrift_remove_virtual_router(vr)
           self.vrs.remove(vr)

    def runTest(self):
        print "Testing AclRedirectTest"

        print '----------------------------------------------------------------------------------------------'

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        port5 = port_list[4]
        #port6 = port_list[5]
        #port7 = port_list[6]
        #port8 = port_list[7]
        #port9 = port_list[8]
        #port10 = port_list[9]
        mac1 = '00:11:11:11:11:11'
        mac_action = SAI_PACKET_ACTION_FORWARD

        #Add port 1,2,3 to Vlan10
        vlan_id = 10
        vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
        self.vlans.append(vlan_oid)
        vlan_member1 = sai_thrift_create_vlan_member(self.client, vlan_oid, port1,
            SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member1)
        self.vlan_ports.append(port1)
        vlan_member2 = sai_thrift_create_vlan_member(self.client, vlan_oid, port2,
            SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member2)
        self.vlan_ports.append(port2)
        vlan_member3 = sai_thrift_create_vlan_member(self.client, vlan_oid, port3,
            SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member3)
        self.vlan_ports.append(port3)
        attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)
        self.client.sai_thrift_set_port_attribute(port2, attr)
        self.client.sai_thrift_set_port_attribute(port3, attr)

        #Create Lag (port 4,5) and add it to Vlan10
        lag_id1 = sai_thrift_create_lag(self.client, [])
        self.lags.append(lag_id1)
        lag_member_id1 = sai_thrift_create_lag_member(self.client, lag_id1, port4)
        self.lag_members.append(lag_member_id1)
        lag_member_id2 = sai_thrift_create_lag_member(self.client, lag_id1, port5)
        self.lag_members.append(lag_member_id2)
        vlan_member4 = sai_thrift_create_vlan_member(self.client, vlan_oid, lag_id1,
            SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.vlan_members.append(vlan_member4)
        self.vlan_ports.append(port4)
        self.vlan_ports.append(port5)

        sai_thrift_create_fdb(self.client, vlan_oid, mac1, port1, mac_action)
        self.fdb[mac1] = (vlan_oid, port1)
        eth_pkt1 = simple_eth_packet(pktlen=100,
                 eth_dst='00:11:11:11:11:11',
                 eth_src='00:06:07:08:09:0a',
                 eth_type=0x8137)
        eth_pkt2 = simple_eth_packet(pktlen=100,
                 eth_dst='00:11:11:11:11:11',
                 eth_src='00:06:07:08:09:0a',
                 eth_type=0x8136)
        eth_pkt3 = simple_eth_packet(pktlen=100,
                 eth_dst='00:11:11:11:11:11',
                 eth_src='00:06:07:08:09:0a',
                 eth_type=0x8135)
        eth_pkt4 = simple_eth_packet(pktlen=100,
                 eth_dst='00:11:11:11:11:11',
                 eth_src='00:06:07:08:09:0a',
                 eth_type=0x8134)
        neg_test_pkt = simple_eth_packet(pktlen=100,
                 eth_dst='00:11:11:11:11:11',
                 eth_src='00:06:07:08:09:0a',
                 eth_type=0x8133)

        #v4_enabled = 1
        #v6_enabled = 1
        #mac = ''
        #vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        #self.vrs.append(vr_id)
        #rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port6, 0, v4_enabled, v6_enabled, mac)
        #self.rifs.append(rif_id1)

        #rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port7, 0, v4_enabled, v6_enabled, mac)
        #self.rifs.append(rif_id2)

        #rif_id3 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port8, 0, v4_enabled, v6_enabled, mac)
        #self.rifs.append(rif_id3)

        ##Create L3 Lag(port 8,9)
        #lag_id2 = sai_thrift_create_lag(self.client, [])
        #self.lags.append(lag_id2)
        #lag_member_id3 = sai_thrift_create_lag_member(self.client, lag_id2, port9)
        #self.lag_members.append(lag_member_id3)
        #lag_member_id4 = sai_thrift_create_lag_member(self.client, lag_id2, port10)
        #self.lag_members.append(lag_member_id4)
        #rif_id4 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, lag_id2, 0, v4_enabled, v6_enabled, mac)
        #self.rifs.append(rif_id4)

        try:
             print '#### NO ACL Applied ####'
             # send the test packet(s)
             print "Sending Test packet EthType:0x%lx port 1 -> port 0"%(eth_pkt1[Ether].type)
             send_packet(self, switch_ports[1], str(eth_pkt1))
             verify_packets(self, eth_pkt1, [switch_ports[0]])

             print "Sending Test packet EthType:0x%lx port 1 -> port 0"%(eth_pkt2[Ether].type)
             send_packet(self, switch_ports[1], str(eth_pkt2))
             verify_packets(self, eth_pkt2, [switch_ports[0]])

             print "Sending Test packet EthType:0x%lx port 1 -> port 0"%(eth_pkt3[Ether].type)
             send_packet(self, switch_ports[1], str(eth_pkt3))
             verify_packets(self, eth_pkt3, [switch_ports[0]])

             print "Sending Test packet EthType:0x%lx port 1 -> port 0"%(eth_pkt4[Ether].type)
             send_packet(self, switch_ports[1], str(eth_pkt4))
             verify_packets(self, eth_pkt4, [switch_ports[0]])

             print "Sending Test(negative test) packet EthType:0x%lx port 1 -> port 0"%(neg_test_pkt[Ether].type)
             send_packet(self, switch_ports[1], str(neg_test_pkt))
             verify_packets(self, neg_test_pkt, [switch_ports[0]])
             print "Sending Test(negative test) packet EthType:0x%lx port 1 -> port 0"%(neg_test_pkt[Ether].type)

             # setup ACL to redirect based on Ether type
             table_stage = SAI_ACL_STAGE_INGRESS
             table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
             entry_priority = 1
             action = SAI_ACL_ENTRY_ATTR_ACTION_REDIRECT
             addr_family = SAI_IP_ADDR_FAMILY_IPV4
             eth_type = 0x8137

             acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                 SAI_ACL_STAGE_INGRESS,
                 [SAI_ACL_BIND_POINT_TYPE_PORT],
                 SAI_ACL_TABLE_GROUP_TYPE_PARALLEL)
             self.acl_grps.append(acl_table_group_id)

             acl_table_id = sai_thrift_create_acl_table(self.client,
                 table_stage,
                 table_bind_point_list,
                 addr_family,
                 ip_src='10.10.10.1')
             self.acl_tables.append(acl_table_id)
             self.assertTrue((acl_table_id != 0), "ACL table create failed")
             print "IPV4 ACL Table created 0x%lx"%(acl_table_id)

             table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                     acl_table_group_id,
                     acl_table_id,
                     100)
             self.assertTrue(table_group_member_id != 0, "ACL group member add failed for acl table 0x%lx, acl group 0x%lx" %(acl_table_id, acl_table_group_id))
             self.acl_grp_members.append(table_group_member_id)

             eth_type = 0x8137
             if eth_type > 0x7fff:
                 eth_type -=0x10000
             acl_entry_id = sai_thrift_create_acl_entry(self.client,
                 acl_table_id,
                 entry_priority,
                 action, SAI_IP_ADDR_FAMILY_IPV4,
                 ether_type=eth_type, redirect_oid=port3)
             self.acl_rules.append(acl_entry_id)
             self.assertTrue((acl_entry_id != 0), 'ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx, create failed for acl table 0x%lx'%(eth_type, port3, acl_table_id))
             print "ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx created 0x%lx"%(eth_pkt1[Ether].type, port3, acl_entry_id)

             entry_priority += 1
             eth_type = 0x8136
             if eth_type > 0x7fff:
                 eth_type -=0x10000
             acl_entry_id = sai_thrift_create_acl_entry(self.client,
                 acl_table_id,
                 entry_priority,
                 action, SAI_IP_ADDR_FAMILY_IPV4,
                 ether_type=eth_type, redirect_oid=lag_id1)
             self.acl_rules.append(acl_entry_id)
             self.assertTrue((acl_entry_id != 0), 'ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx, create failed for acl table 0x%lx'%(eth_type, lag_id1, acl_table_id))
             print "ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx created 0x%lx"%(eth_pkt2[Ether].type, lag_id1, acl_entry_id)

             acl_table_id = sai_thrift_create_acl_table(self.client,
                 table_stage,
                 table_bind_point_list,
                 addr_family=SAI_IP_ADDR_FAMILY_IPV6,
                 ip_src='2000::1')
             self.acl_tables.append(acl_table_id)
             self.assertTrue((acl_table_id != 0), "ACL table create failed")
             print "IPV6 ACL Table created 0x%lx"%(acl_table_id)

             table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                     acl_table_group_id,
                     acl_table_id,
                     200)
             self.assertTrue(table_group_member_id != 0, "ACL group member add failed for acl table 0x%lx, acl group 0x%lx" %(acl_table_id, acl_table_group_id))
             self.acl_grp_members.append(table_group_member_id)

             eth_type = 0x8135
             if eth_type > 0x7fff:
                 eth_type -=0x10000
             acl_entry_id = sai_thrift_create_acl_entry(self.client,
                 acl_table_id,
                 entry_priority,
                 action, SAI_IP_ADDR_FAMILY_IPV4,
                 ether_type=eth_type, redirect_oid=port3)
             self.acl_rules.append(acl_entry_id)
             self.assertTrue((acl_entry_id != 0), 'ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx, create failed for acl table 0x%lx'%(eth_type, port3, acl_table_id))
             print "ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx created 0x%lx"%(eth_pkt3[Ether].type, port3, acl_entry_id)

             eth_type = 0x8134
             if eth_type > 0x7fff:
                 eth_type -=0x10000
             acl_entry_id = sai_thrift_create_acl_entry(self.client,
                 acl_table_id,
                 entry_priority,
                 action, SAI_IP_ADDR_FAMILY_IPV4,
                 ether_type=eth_type, redirect_oid=lag_id1)
             self.acl_rules.append(acl_entry_id)
             self.assertTrue((acl_entry_id != 0), 'ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx, create failed for acl table 0x%lx'%(eth_type, lag_id1, acl_table_id))
             print "ACL entry Match: EthType-0x%lx Action: Redirect-0x%lx created 0x%lx"%(eth_pkt4[Ether].type, lag_id1, acl_entry_id)

             print "Binding ACL grp 0x%lx to Port2"%(acl_table_group_id)
             ## bind ACL GRP to Port2
             attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
             attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
             self.client.sai_thrift_set_port_attribute(port2, attr)
             self.bp_list.append(port2)

             print "Sending Test packet EthType:0x%lx port 1 -> [ACL REDIRECT] -> port 2"%(eth_pkt1[Ether].type)
             # ensure packet is redirected!
             send_packet(self, switch_ports[1], str(eth_pkt1))
             verify_packets(self, eth_pkt1, [switch_ports[2]])

             # ensure packet is redirected!
             print "Sending Test packet EthType:0x%lx port 3 -> [ACL REDIRECT] -> Lag1 (Port 3/Port 4)"%(eth_pkt2[Ether].type)
             send_packet(self, switch_ports[1], str(eth_pkt2))
             verify_packets_any(self, eth_pkt2, [switch_ports[3], switch_ports[4]])

             # ensure packet is redirected!
             print "Sending Test packet EthType:0x%lx port 1 -> [ACL REDIRECT] -> port 2"%(eth_pkt3[Ether].type)
             send_packet(self, switch_ports[1], str(eth_pkt3))
             verify_packets(self, eth_pkt3, [switch_ports[2]])

             # ensure packet is redirected!
             print "Sending Test packet EthType:0x%lx port 1 -> [ACL REDIRECT] -> Lag1 (Port 3/Port 4)"%(eth_pkt4[Ether].type)
             send_packet(self, switch_ports[1], str(eth_pkt4))
             verify_packets_any(self, eth_pkt4, [switch_ports[3], switch_ports[4]])

             # ensure packet is not redirected!
             print "Sending Test(negative test) packet EthType:0x%lx port 1 -> port 0"%(neg_test_pkt[Ether].type)
             send_packet(self, switch_ports[1], str(neg_test_pkt))
             verify_packets(self, neg_test_pkt, [0])
        finally:
             self.acl_cleanup()
             self.cleanup()

class AclPortGroupMirrorTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
          print "ACL port_group not supported in the profile"
          return
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        total_ports = len(port_list)
        self.acl_groups = []
        self.acl_group_members = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        port_list1 = []
        port_list2 = []
        print "Total ports"
        print total_ports
        for i in range(0, total_ports/2):
          port_list1.append(port_list[i])
        print port_list1
        for i in range(total_ports/2, total_ports):
          port_list2.append(port_list[i])

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        self.acl_groups1 = []
        self.acl_groups2 = []
        for port in port_list1:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups1.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)

        for port in port_list2:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups2.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)


        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_ipv4_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        acl_table_ipv4_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, 50)

        acl_table_ipv4_id3 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        # create ACL table group members
        #Attach ACL table acl_table_ipv4_id3 to sencond set of port list.
        for acl_group in self.acl_groups2:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id3, 1)
            self.acl_group_members.append(acl_table_group_member_id)

        #Attach ACL table acl_table_ipv4_id1,acl_table_ipv4_id2 and acl_table_mirror_id to first set of port list.
        for acl_group in self.acl_groups1:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id2, 1)
            self.acl_group_members.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_mirror_id, 1)
            self.acl_group_members.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id1, 1)
            self.acl_group_members.append(acl_table_group_member_id)

        monitor_port = port1
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print spanid
        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))

        print "Sending packet ptf_intf 2 -[acl]-> ptf_intf 1 (20.20.20.1-[acl]-> 172.16.10.1 [id = 105])"
        ip_src = "20.20.20.1"
        ip_src_mask = "255.255.255.0"
        ingress_mirror_id = spanid
        action = None
        in_ports = None

        try:
            mirror_acl_entry = sai_thrift_create_acl_entry(self.client,
                acl_table_mirror_id,
                100,
                action, addr_family,
                mac_src, mac_src_mask,
                mac_dst, mac_dst_mask,
                ip_src, ip_src_mask,
                ip_dst, ip_dst_mask,
                ip_proto,
                in_ports, out_ports,
                in_port, out_port,
                src_l4_port, dst_l4_port,
                ingress_mirror_id,
                egress_mirror_id,
                range_list)


            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst="192.168.0.0",
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 3, pkt)
            verify_packets(self, pkt, ports=[0])

        finally:
            self.client.sai_thrift_remove_acl_entry(mirror_acl_entry)
            for acl_group_member in self.acl_group_members:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            for index, port in port_list.items():
                attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port, attr)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id1)
            self.client.sai_thrift_remove_acl_table(acl_table_mirror_id)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id2)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id3)
            for acl_group in self.acl_groups1:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            for acl_group in self.acl_groups2:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

class AclPortGroupMirrorDenyTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
          print "ACL port_group not supported in the profile"
          return
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        total_ports = len(port_list)
        self.acl_groups = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        port_list1 = []
        port_list2 = []
        print "Total ports"
        print total_ports
        for i in range(0, total_ports/2):
          port_list1.append(port_list[i])
        print port_list1
        for i in range(total_ports/2, total_ports):
          port_list2.append(port_list[i])

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        self.acl_groups1 = []
        self.acl_groups2 = []
        for port in port_list1:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups1.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)

        for port in port_list2:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups2.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)


        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_ipv4_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        acl_table_ipv4_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, 50)

        acl_table_ipv4_id3 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        # create ACL table group members
        #Attach ACL table acl_table_ipv4_id3 to sencond set of port list.
        self.acl_group_members_id3 = []
        for acl_group in self.acl_groups2:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id3, 1)
            self.acl_group_members_id3.append(acl_table_group_member_id)

        #Attach ACL table acl_table_ipv4_id1,acl_table_ipv4_id2 and acl_table_mirror_id to first set of port list.
        self.acl_group_members_id1 = []
        self.acl_group_members_id2 = []
        self.acl_group_members_mirror = []
        for acl_group in self.acl_groups1:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id2, 1)
            self.acl_group_members_id2.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_mirror_id, 1)
            self.acl_group_members_mirror.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id1, 1)
            self.acl_group_members_id1.append(acl_table_group_member_id)

        in_ports = None
        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID3|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "10.0.0.2/32"
            }
        },
        '''
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id3, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "10.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID2|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id2, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_2": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9979",
                "SRC_IP": "20.0.0.7/32"
            }
        },
        '''
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9979, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.7", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|DATAINGRESS|RULE_3": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "4731",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9978"
            }
        },
        '''
        acl_entry_id3 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9978, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 4731,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_4": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT": "6000 - 7000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9977"
            }
        },
        '''
        src_range = sai_thrift_range_t(min=6000, max=7000)
        src_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, src_range)
        src_range_list = [src_acl_range_id]

        acl_entry_id4 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9977, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            src_range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_5": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "8000 - 9000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9976"
            }
        },
        '''
        dst_range = sai_thrift_range_t(min=8000, max=9000)
        dst_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, dst_range)
        dst_range_list = [dst_acl_range_id]

        acl_entry_id5 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9976, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            dst_range_list, ip_dscp)

        action = None
        monitor_port = port1
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print spanid
        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))

        ip_src = "20.20.20.1"
        ip_src_mask = "255.255.255.0"
        ingress_mirror_id = spanid

        mirror_acl_entry = sai_thrift_create_acl_entry(self.client,
            acl_table_mirror_id,
            100,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        try:
            #send packet matching rule in table2 - acl_table_ipv4_id2
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x127B,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 6123,
              tcp_dport = 5555,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 5555,
              tcp_dport = 8123,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - mirror_acl_entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 2, pkt)
            verify_packets(self, pkt, ports=[0])

        finally:
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id3)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id4)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id5)
            for acl_group_member in self.acl_group_members_id1:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id1)
            self.client.sai_thrift_delete_acl_range(src_acl_range_id)
            self.client.sai_thrift_delete_acl_range(dst_acl_range_id)

            #ACL tableid2 is still attached to the port, verify deny ACL.
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            for acl_group_member in self.acl_group_members_id2:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id2)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            for acl_group_member in self.acl_group_members_id3:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id3)

            #Mirror ACL table is still attached to the port, verify mirror entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 2, pkt)
            verify_packets(self, pkt, ports=[0])

            self.client.sai_thrift_remove_acl_entry(mirror_acl_entry)
            for acl_group_member in self.acl_group_members_mirror:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_mirror_id)

            for index, port in port_list.items():
                attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port, attr)
            for acl_group in self.acl_groups1:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            for acl_group in self.acl_groups2:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

class AclMultiplePortGroupMirrorDenyTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
          print "ACL port_group not supported in the profile"
          return
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        total_ports = len(port_list)
        self.acl_groups = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        port_list1 = []
        port_list2 = []
        for i in range(0, total_ports/2):
          port_list1.append(port_list[i])
        for i in range(total_ports/2, total_ports):
          port_list2.append(port_list[i])

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        self.acl_groups1 = []
        self.acl_groups2 = []
        for port in port_list1:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups1.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)

        for port in port_list2:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups2.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)


        self.port_group1 = []
        self.port_group2 = []

        #port_group1 = ACL groups of port_list1
        #port_group2 = ACL groups of port_list1+port_list2
        for acl_group in self.acl_groups1:
          self.port_group1.append(acl_group)
          self.port_group2.append(acl_group)

        for acl_group in self.acl_groups2:
          self.port_group2.append(acl_group)

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_ipv4_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        acl_table_ipv4_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, 50)

        acl_table_ipv4_id3 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        # create ACL table group members
        #Attach ACL table acl_table_ipv4_id3 to sencond set of port list.
        self.acl_group_members_id3 = []
        for acl_group in self.port_group1:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id3, 1)
            self.acl_group_members_id3.append(acl_table_group_member_id)

        #Attach ACL table acl_table_ipv4_id1,acl_table_ipv4_id2 and acl_table_mirror_id to first set of port list.
        self.acl_group_members_id1 = []
        self.acl_group_members_id2 = []
        self.acl_group_members_mirror = []
        for acl_group in self.port_group2:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id2, 1)
            self.acl_group_members_id2.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_mirror_id, 1)
            self.acl_group_members_mirror.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id1, 1)
            self.acl_group_members_id1.append(acl_table_group_member_id)

        in_ports = None
        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID3|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "10.0.0.2/32"
            }
        },
        '''
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id3, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "10.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID2|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id2, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_2": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9979",
                "SRC_IP": "20.0.0.7/32"
            }
        },
        '''
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9979, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.7", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_3": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "4731",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9978"
            }
        },
        '''
        acl_entry_id3 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9978, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 4731,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_4": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT": "6000 - 7000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9977"
            }
        },
        '''
        src_range = sai_thrift_range_t(min=6000, max=7000)
        src_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, src_range)
        src_range_list = [src_acl_range_id]

        acl_entry_id4 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9977, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            src_range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_5": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "8000 - 9000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9976"
            }
        },
        '''
        dst_range = sai_thrift_range_t(min=8000, max=9000)
        dst_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, dst_range)
        dst_range_list = [dst_acl_range_id]

        acl_entry_id5 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9976, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            dst_range_list, ip_dscp)

        action = None
        monitor_port = port1
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print spanid
        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))

        ip_src = "20.20.20.1"
        ip_src_mask = "255.255.255.0"
        ingress_mirror_id = spanid

        '''
        "ACL_RULE|ACL_TABLE_MIRROR_ID|RULE_1": {
            "type": "hash",
            "value": {
                "IP_SRC": "20.20.20.1",
                "PACKET_ACTION": "MIRROR",
                "PRIORITY": "100"
            }
        },
        '''
        mirror_acl_entry = sai_thrift_create_acl_entry(self.client,
            acl_table_mirror_id,
            100,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        try:
            #send packet matching rule in table2 - acl_table_ipv4_id2
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x127B,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 6123,
              tcp_dport = 5555,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 5555,
              tcp_dport = 8123,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - mirror_acl_entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 2, pkt)
            verify_packets(self, pkt, ports=[0])

        finally:
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id3)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id4)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id5)
            for acl_group_member in self.acl_group_members_id1:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id1)
            self.client.sai_thrift_delete_acl_range(src_acl_range_id)
            self.client.sai_thrift_delete_acl_range(dst_acl_range_id)

            #ACL tableid2 is still attached to the port, verify deny ACL.
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            for acl_group_member in self.acl_group_members_id2:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id2)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            for acl_group_member in self.acl_group_members_id3:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id3)

            #Mirror ACL table is still attached to the port, verify mirror entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 2, pkt)
            verify_packets(self, pkt, ports=[0])

            self.client.sai_thrift_remove_acl_entry(mirror_acl_entry)
            for acl_group_member in self.acl_group_members_mirror:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_mirror_id)

            for index, port in port_list.items():
                attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port, attr)
            for acl_group in self.acl_groups1:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            for acl_group in self.acl_groups2:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

class AclSinglePortLagMirrorDenyTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
          print "ACL port_group not supported in the profile"
          return
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        total_ports = len(port_list)
        self.acl_groups = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        port_list1 = [port1]
        port_list2 = [port2]

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        self.acl_groups1 = []
        self.acl_groups2 = []
        for port in port_list1:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups1.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)

        for port in port_list2:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups2.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)


        self.port_group1 = []
        self.port_group2 = []

        #port_group1 = ACL groups of port_list1
        #port_group2 = ACL groups of port_list1+port_list2
        for acl_group in self.acl_groups1:
          self.port_group1.append(acl_group)

        for acl_group in self.acl_groups2:
          self.port_group2.append(acl_group)

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port1, port2]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_ipv4_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        acl_table_ipv4_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, 50)

        acl_table_ipv4_id3 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        # create ACL table group members
        #Attach ACL table acl_table_ipv4_id3 to sencond set of port list.
        self.acl_group_members_id3 = []
        for acl_group in self.port_group1:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id3, 1)
            self.acl_group_members_id3.append(acl_table_group_member_id)

        #Attach ACL table acl_table_ipv4_id1,acl_table_ipv4_id2 and acl_table_mirror_id to first set of port list.
        self.acl_group_members_id1 = []
        self.acl_group_members_id2 = []
        self.acl_group_members_mirror = []
        for acl_group in self.port_group2:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id2, 1)
            self.acl_group_members_id2.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_mirror_id, 1)
            self.acl_group_members_mirror.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id1, 1)
            self.acl_group_members_id1.append(acl_table_group_member_id)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID3|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "10.0.0.2/32"
            }
        },
        '''
        in_ports = None
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id3, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "10.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID2|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id2, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_2": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9979",
                "SRC_IP": "20.0.0.7/32"
            }
        },
        '''
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9979, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.7", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_3": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "4731",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9978"
            }
        },
        '''
        acl_entry_id3 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9978, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 4731,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_4": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT": "6000 - 7000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9977"
            }
        },
        '''
        src_range = sai_thrift_range_t(min=6000, max=7000)
        src_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, src_range)
        src_range_list = [src_acl_range_id]

        acl_entry_id4 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9977, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            src_range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_5": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "8000 - 9000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9976"
            }
        },
        '''
        dst_range = sai_thrift_range_t(min=8000, max=9000)
        dst_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, dst_range)
        dst_range_list = [dst_acl_range_id]

        acl_entry_id5 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9976, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            dst_range_list, ip_dscp)

        action = None
        monitor_port = port3
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print spanid
        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))

        ip_src = "20.20.20.1"
        ip_src_mask = "255.255.255.0"
        ingress_mirror_id = spanid

        '''
        "ACL_RULE|ACL_TABLE_MIRROR_ID|RULE_1": {
            "type": "hash",
            "value": {
                "IP_SRC": "20.20.20.1",
                "PACKET_ACTION": "MIRROR",
                "PRIORITY": "100"
            }
        },
        '''
        mirror_acl_entry = sai_thrift_create_acl_entry(self.client,
            acl_table_mirror_id,
            100,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        try:
            #send packet matching rule in table2 - acl_table_ipv4_id2
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x127B,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 6123,
              tcp_dport = 5555,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 5555,
              tcp_dport = 8123,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - mirror_acl_entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 1, pkt)
            verify_packets(self, pkt, ports=[2])

        finally:
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id3)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id4)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id5)
            for acl_group_member in self.acl_group_members_id1:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id1)
            self.client.sai_thrift_delete_acl_range(src_acl_range_id)
            self.client.sai_thrift_delete_acl_range(dst_acl_range_id)

            #ACL tableid2 is still attached to the port, verify deny ACL.
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            for acl_group_member in self.acl_group_members_id2:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id2)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            for acl_group_member in self.acl_group_members_id3:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id3)

            #Mirror ACL table is still attached to the port, verify mirror entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 1, pkt)
            verify_packets(self, pkt, ports=[2])

            self.client.sai_thrift_remove_acl_entry(mirror_acl_entry)
            for acl_group_member in self.acl_group_members_mirror:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_mirror_id)

            for index, port in port_list.items():
                attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port, attr)
            for acl_group in self.acl_groups1:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            for acl_group in self.acl_groups2:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

class AclSingleInPortAclEntryMirrorDenyTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
          print "ACL port_group not supported in the profile"
          return
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        port5 = port_list[4]
        total_ports = len(port_list)
        self.acl_groups = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        port_list1 = [port4]
        port_list2 = [port5]
        print "Total ports"
        print total_ports

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        self.acl_groups1 = []
        self.acl_groups2 = []
        for port in port_list1:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups1.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)

        for port in port_list2:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups2.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)


        self.port_group1 = []
        self.port_group2 = []

        #port_group1 = ACL groups of port_list1
        #port_group2 = ACL groups of port_list1+port_list2
        for acl_group in self.acl_groups1:
          self.port_group1.append(acl_group)

        for acl_group in self.acl_groups2:
          self.port_group2.append(acl_group)

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port4, port5]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_ipv4_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        acl_table_ipv4_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, 50)

        acl_table_ipv4_id3 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        # create ACL table group members
        #Attach ACL table acl_table_ipv4_id3 to sencond set of port list.
        self.acl_group_members_id3 = []
        for acl_group in self.port_group1:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id3, 1)
            self.acl_group_members_id3.append(acl_table_group_member_id)

        #Attach ACL table acl_table_ipv4_id1,acl_table_ipv4_id2 and acl_table_mirror_id to first set of port list.
        self.acl_group_members_id1 = []
        self.acl_group_members_id2 = []
        self.acl_group_members_mirror = []
        for acl_group in self.port_group2:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id2, 1)
            self.acl_group_members_id2.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_mirror_id, 1)
            self.acl_group_members_mirror.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id1, 1)
            self.acl_group_members_id1.append(acl_table_group_member_id)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID3|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "10.0.0.2/32"
            }
        },
        '''
        #All ACL tables are bound to port4 and port5. But the ACL entry in_ports = port1
        #Packets are sent on Port1 to match mirror or deny rules.
        in_ports = [port2]
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id3, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "10.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID2|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        in_ports = [port2]
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id2, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_2": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9979",
                "SRC_IP": "20.0.0.7/32"
            }
        },
        '''
        in_ports = [port2]
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9979, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.7", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_3": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "4731",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9978"
            }
        },
        '''
        in_ports = [port2]
        acl_entry_id3 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9978, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 4731,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_4": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT": "6000 - 7000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9977"
            }
        },
        '''
        src_range = sai_thrift_range_t(min=6000, max=7000)
        src_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, src_range)
        src_range_list = [src_acl_range_id]

        in_ports = [port2]
        acl_entry_id4 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9977, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            src_range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_5": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "8000 - 9000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9976"
            }
        },
        '''
        dst_range = sai_thrift_range_t(min=8000, max=9000)
        dst_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, dst_range)
        dst_range_list = [dst_acl_range_id]

        in_ports = [port2]
        acl_entry_id5 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9976, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            dst_range_list, ip_dscp)

        action = None
        monitor_port = port3
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print spanid
        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))

        ip_src = "20.20.20.1"
        ip_src_mask = "255.255.255.0"
        ingress_mirror_id = spanid

        '''
        "ACL_RULE|ACL_TABLE_MIRROR_ID|RULE_1": {
            "type": "hash",
            "value": {
                "IP_SRC": "20.20.20.1",
                "PACKET_ACTION": "MIRROR",
                "PRIORITY": "100"
            }
        },
        '''
        in_ports = [port2]
        mirror_acl_entry = sai_thrift_create_acl_entry(self.client,
            acl_table_mirror_id,
            100,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        try:
            #send packet matching rule in table2 - acl_table_ipv4_id2
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x127B,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 6123,
              tcp_dport = 5555,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 5555,
              tcp_dport = 8123,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - mirror_acl_entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 1, pkt)
            verify_packets(self, pkt, ports=[2])

        finally:
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id3)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id4)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id5)
            for acl_group_member in self.acl_group_members_id1:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id1)
            self.client.sai_thrift_delete_acl_range(src_acl_range_id)
            self.client.sai_thrift_delete_acl_range(dst_acl_range_id)

            #ACL tableid2 is still attached to the port, verify deny ACL.
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            for acl_group_member in self.acl_group_members_id2:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id2)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            for acl_group_member in self.acl_group_members_id3:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id3)

            #Mirror ACL table is still attached to the port, verify mirror entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 1, pkt)
            verify_packets(self, pkt, ports=[2])

            self.client.sai_thrift_remove_acl_entry(mirror_acl_entry)
            for acl_group_member in self.acl_group_members_mirror:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_mirror_id)

            for index, port in port_list.items():
                attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port, attr)
            for acl_group in self.acl_groups1:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            for acl_group in self.acl_groups2:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            self.client.sai_thrift_remove_virtual_router(vr_id)

class AclMultiplePortLagGroupMirrorDenyTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
          print "ACL port_group not supported in the profile"
          return
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        total_ports = len(port_list)
        self.acl_groups = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        port_list1 = []
        port_list2 = []
        print "Total ports"
        print total_ports
        for i in range(0, total_ports/2):
          port_list1.append(port_list[i])
        print port_list1
        for i in range(total_ports/2, total_ports):
          port_list2.append(port_list[i])

        lag1 = self.client.sai_thrift_create_lag([])

        port4 = port_list[4]
        lag_member_id2 = sai_thrift_create_lag_member(self.client, lag1, port1)
        lag_member_id1 = sai_thrift_create_lag_member(self.client, lag1, port4)
        #Remove port1 and port4 from port_list1 and add lag1 to the port_list
        port_list1.remove(port1)
        port_list1.remove(port4)
        port_list1.append(lag1)

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, lag1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        self.acl_groups1 = []
        self.acl_groups2 = []
        for port in port_list1:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups1.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            if port == lag1:
              print "Setting LAG ACL attribute"
              attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_INGRESS_ACL, value=attr_value)
              self.client.sai_thrift_set_lag_attribute(port, attr)
            else:
              attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
              self.client.sai_thrift_set_port_attribute(port, attr)

        for port in port_list2:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups2.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            if port == lag1:
              print "Setting LAG ACL attribute"
              attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_INGRESS_ACL, value=attr_value)
              self.client.sai_thrift_set_lag_attribute(port, attr)
            else:
              attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
              self.client.sai_thrift_set_port_attribute(port, attr)


        self.port_group1 = []
        self.port_group2 = []

        #port_group1 = ACL groups of port_list1
        #port_group2 = ACL groups of port_list1+port_list2
        for acl_group in self.acl_groups1:
          self.port_group1.append(acl_group)
          self.port_group2.append(acl_group)

        for acl_group in self.acl_groups2:
          self.port_group2.append(acl_group)

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = None
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_ipv4_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        acl_table_ipv4_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, 50)

        acl_table_ipv4_id3 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        # create ACL table group members
        #Attach ACL table acl_table_ipv4_id3 to sencond set of port list.
        self.acl_group_members_id3 = []
        for acl_group in self.port_group1:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id3, 1)
            self.acl_group_members_id3.append(acl_table_group_member_id)

        #Attach ACL table acl_table_ipv4_id1,acl_table_ipv4_id2 and acl_table_mirror_id to first set of port list.
        self.acl_group_members_id1 = []
        self.acl_group_members_id2 = []
        self.acl_group_members_mirror = []
        for acl_group in self.port_group2:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id2, 1)
            self.acl_group_members_id2.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_mirror_id, 1)
            self.acl_group_members_mirror.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id1, 1)
            self.acl_group_members_id1.append(acl_table_group_member_id)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID3|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "10.0.0.2/32"
            }
        },
        '''
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id3, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "10.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID2|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id2, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_2": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9979",
                "SRC_IP": "20.0.0.7/32"
            }
        },
        '''
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9979, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.7", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_3": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "4731",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9978"
            }
        },
        '''
        acl_entry_id3 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9978, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 4731,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        action = None
        monitor_port = port1
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print spanid
        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))

        ip_src = "20.20.20.1"
        ip_src_mask = "255.255.255.0"
        ingress_mirror_id = spanid

        '''
        "ACL_RULE|ACL_TABLE_MIRROR_ID|RULE_1": {
            "type": "hash",
            "value": {
                "IP_SRC": "20.20.20.1",
                "PACKET_ACTION": "MIRROR",
                "PRIORITY": "100"
            }
        },
        '''
        mirror_acl_entry = sai_thrift_create_acl_entry(self.client,
            acl_table_mirror_id,
            100,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        try:
            #send packet matching rule in table2 - acl_table_ipv4_id2
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            #Send packet to two LAG members and check the ingress ACL is applied
            send_packet(self, 0, str(pkt))
            verify_no_other_packets(self, timeout=1)

            send_packet(self, 3, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x127B,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - mirror_acl_entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 2, pkt)
            verify_packets(self, pkt, ports=[0])

        finally:
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id3)
            for acl_group_member in self.acl_group_members_id1:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id1)

            #ACL tableid2 is still attached to the port, verify deny ACL.
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            for acl_group_member in self.acl_group_members_id2:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id2)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            for acl_group_member in self.acl_group_members_id3:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id3)

            #Mirror ACL table is still attached to the port, verify mirror entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 2, pkt)
            verify_packets(self, pkt, ports=[0])

            self.client.sai_thrift_remove_acl_entry(mirror_acl_entry)
            for acl_group_member in self.acl_group_members_mirror:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_mirror_id)

            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_lag_attribute(lag1, attr)

            for index, port in port_list.items():
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port, attr)

            for acl_group in self.acl_groups1:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            for acl_group in self.acl_groups2:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            sai_thrift_remove_lag_member(self.client, lag_member_id1)
            sai_thrift_remove_lag_member(self.client, lag_member_id2)
            self.client.sai_thrift_remove_lag(lag1)
            self.client.sai_thrift_remove_virtual_router(vr_id)

class AclSingleInLagAclEntryMirrorDenyTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
          print "ACL port_group not supported in the profile"
          return
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        port4 = port_list[3]
        port5 = port_list[4]
        total_ports = len(port_list)
        self.acl_groups = []
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        port_list1 = [port4]
        port_list2 = [port5]
        print "Total ports"
        print total_ports

        lag1 = self.client.sai_thrift_create_lag([])

        lag_member_id1 = sai_thrift_create_lag_member(self.client, lag1, port1)
        lag_member_id2 = sai_thrift_create_lag_member(self.client, lag1, port2)

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, lag1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)

        # setup ACL table group
        group_stage = SAI_ACL_STAGE_INGRESS
        group_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        group_type = SAI_ACL_TABLE_GROUP_TYPE_PARALLEL

        self.acl_groups1 = []
        self.acl_groups2 = []
        for port in port_list1:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups1.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)

        for port in port_list2:
            acl_table_group_id = sai_thrift_create_acl_table_group(self.client,
                group_stage,
                group_bind_point_list,
                group_type)
            self.acl_groups2.append(acl_table_group_id)
            # bind this ACL group to ports object id
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port, attr)


        self.port_group1 = []
        self.port_group2 = []

        #port_group1 = ACL groups of port_list1
        #port_group2 = ACL groups of port_list1+port_list2
        for acl_group in self.acl_groups1:
          self.port_group1.append(acl_group)

        for acl_group in self.acl_groups2:
          self.port_group2.append(acl_group)

        table_stage = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP
        in_ports = [port4, port5]
        mac_src = None
        mac_dst = None
        mac_src_mask = None
        mac_dst_mask = None
        ip_src = None
        ip_src_mask = None
        ip_dst = None
        ip_dst_mask = None
        ip_proto = None
        in_port = None
        out_port = None
        out_ports = None
        src_l4_port = None
        dst_l4_port = None
        ingress_mirror_id = None
        egress_mirror_id = None
        range_list = None
        ip_dscp = None

        acl_table_ipv4_id1 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        acl_table_ipv4_id2 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)
        acl_table_mirror_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, 50)

        acl_table_ipv4_id3 = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            mac_src, mac_dst,
            "192.168.0.1", ip_dst, ip_proto,
            in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            range_list, ip_dscp)

        # create ACL table group members
        #Attach ACL table acl_table_ipv4_id3 to sencond set of port list.
        self.acl_group_members_id3 = []
        for acl_group in self.port_group1:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id3, 1)
            self.acl_group_members_id3.append(acl_table_group_member_id)

        #Attach ACL table acl_table_ipv4_id1,acl_table_ipv4_id2 and acl_table_mirror_id to first set of port list.
        self.acl_group_members_id1 = []
        self.acl_group_members_id2 = []
        self.acl_group_members_mirror = []
        for acl_group in self.port_group2:
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id2, 1)
            self.acl_group_members_id2.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_mirror_id, 1)
            self.acl_group_members_mirror.append(acl_table_group_member_id)
            acl_table_group_member_id = sai_thrift_create_acl_table_group_member(self.client,
                acl_group, acl_table_ipv4_id1, 1)
            self.acl_group_members_id1.append(acl_table_group_member_id)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID3|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "10.0.0.2/32"
            }
        },
        '''
        #All ACL tables are bound to port4 and port5. But the ACL entry in_ports = lag1
        #Packets are sent on Port1 to match mirror or deny rules.
        in_ports = [lag1]
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id3, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "10.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)
        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID2|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''
        in_ports = [lag1]
        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id2, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.2", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_2": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9979",
                "SRC_IP": "20.0.0.7/32"
            }
        },
        '''
        in_ports = [lag1]
        acl_entry_id2 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9979, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            "20.0.0.7", "255.255.255.255", ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_3": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "4731",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9978"
            }
        },
        '''
        in_ports = [lag1]
        acl_entry_id3 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9978, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, 4731,
            ingress_mirror_id, egress_mirror_id,
            range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_4": {
            "type": "hash",
            "value": {
                "L4_SRC_PORT": "6000 - 7000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9977"
            }
        },
        '''
        src_range = sai_thrift_range_t(min=6000, max=7000)
        src_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, src_range)
        src_range_list = [src_acl_range_id]

        in_ports = [lag1]
        acl_entry_id4 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9977, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            src_range_list, ip_dscp)

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID1|RULE_5": {
            "type": "hash",
            "value": {
                "L4_DST_PORT": "8000 - 9000",
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9976"
            }
        },
        '''
        dst_range = sai_thrift_range_t(min=8000, max=9000)
        dst_acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_DST_PORT_RANGE, dst_range)
        dst_range_list = [dst_acl_range_id]

        in_ports = [lag1]
        acl_entry_id5 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id1, 9976, SAI_PACKET_ACTION_DROP, addr_family,
            mac_src, mac_src_mask, mac_dst, mac_dst_mask,
            ip_src, ip_src_mask, ip_dst, ip_dst_mask,
            ip_proto, in_ports, out_ports, in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id, egress_mirror_id,
            dst_range_list, ip_dscp)

        action = None
        monitor_port = port3
        mirror_type=SAI_MIRROR_SESSION_TYPE_LOCAL
        spanid=sai_thrift_create_mirror_session(self.client,mirror_type=mirror_type,port=monitor_port,vlan=0,vlan_priority=0,vlan_tpid=0,vlan_header_valid=False,src_mac=None,dst_mac=None,src_ip=None,dst_ip=None,encap_type=0,iphdr_version=0,ttl=0,tos=0,gre_type=0)
        print spanid
        attrb_value = sai_thrift_attribute_value_t(objlist=sai_thrift_object_list_t(count=1,object_id_list=[spanid]))

        ip_src = "20.20.20.1"
        ip_src_mask = "255.255.255.0"
        ingress_mirror_id = spanid

        '''
        "ACL_RULE|ACL_TABLE_MIRROR_ID|RULE_1": {
            "type": "hash",
            "value": {
                "IP_SRC": "20.20.20.1",
                "PACKET_ACTION": "MIRROR",
                "PRIORITY": "100"
            }
        },
        '''
        in_ports = [lag1]
        mirror_acl_entry = sai_thrift_create_acl_entry(self.client,
            acl_table_mirror_id,
            100,
            action, addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id,
            range_list)

        try:
            #send packet matching rule in table2 - acl_table_ipv4_id2
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.7",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x127B,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 6123,
              tcp_dport = 5555,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - acl_table_ipv4_id1
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.3",
              ip_dst = "192.168.0.0",
              tcp_sport = 5555,
              tcp_dport = 8123,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            #send packet matching rule in table1 - mirror_acl_entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 1, pkt)
            verify_packets(self, pkt, ports=[2])

        finally:
            self.client.sai_thrift_remove_acl_entry(acl_entry_id2)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id3)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id4)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id5)
            for acl_group_member in self.acl_group_members_id1:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id1)
            self.client.sai_thrift_delete_acl_range(src_acl_range_id)
            self.client.sai_thrift_delete_acl_range(dst_acl_range_id)

            #ACL tableid2 is still attached to the port, verify deny ACL.
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 1, str(pkt))
            verify_no_other_packets(self, timeout=1)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            for acl_group_member in self.acl_group_members_id2:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id2)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            for acl_group_member in self.acl_group_members_id3:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id3)

            #Mirror ACL table is still attached to the port, verify mirror entry
            pkt = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='20.20.20.1',
                                    ip_dst='172.16.10.1',
                                    ip_id=101,
                                    ip_ttl=64,
                                    tcp_sport=4000,
                                    tcp_dport=5000)
            print "Sending packet port 2 -> port 3 (00:22:22:22:22:22 -> 00:00:00:00:00:33)"
            send_packet(self, 1, pkt)
            verify_packets(self, pkt, ports=[2])

            self.client.sai_thrift_remove_acl_entry(mirror_acl_entry)
            for acl_group_member in self.acl_group_members_mirror:
                self.client.sai_thrift_remove_acl_table_group_member(acl_group_member)
            self.client.sai_thrift_remove_acl_table(acl_table_mirror_id)

            for index, port in port_list.items():
                attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
                attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
                self.client.sai_thrift_set_port_attribute(port, attr)
            for acl_group in self.acl_groups1:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            for acl_group in self.acl_groups2:
                self.client.sai_thrift_remove_acl_table_group(acl_group)
            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            sai_thrift_remove_lag_member(self.client, lag_member_id1)
            sai_thrift_remove_lag_member(self.client, lag_member_id2)
            self.client.sai_thrift_remove_lag(lag1)
            self.client.sai_thrift_remove_virtual_router(vr_id)


@pktpy_skip  # TODO bf-pktpy
class AclLagEgressTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print

        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        v4_enabled = 1
        v6_enabled = 1
        mac = ''
        lag_member_id2 = None

        lag1 = self.client.sai_thrift_create_lag([])

        lag_member_id1 = sai_thrift_create_lag_member(self.client, lag1, port1)

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, lag1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port3, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1_subnet = '192.168.0.0'
        ip_mask1_subnet = '255.255.0.0'
        ip_addr1 = '192.168.0.1'
        dmac1 = '00:22:22:22:22:22'
        sai_thrift_create_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, addr_family, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, nhop1)

        table_stage = SAI_ACL_STAGE_EGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action = SAI_PACKET_ACTION_DROP

        acl_table_ipv4_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            addr_family,
            ip_src="192.168.0.1")

        '''
        "ACL_RULE|ACL_TABLE_IPV4_ID|RULE_1": {
            "type": "hash",
            "value": {
                "PACKET_ACTION": "DROP",
                "PRIORITY": "9999",
                "SRC_IP": "20.0.0.2/32"
            }
        },
        '''

        acl_entry_id1 = sai_thrift_create_acl_entry(self.client,
            acl_table_ipv4_id, 9999, SAI_PACKET_ACTION_DROP, addr_family,
            ip_src = "20.0.0.2", ip_src_mask = "255.255.255.255")

        try:
            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            exp_pkt = simple_tcp_packet(
              eth_dst = "00:22:22:22:22:22",
              eth_src = router_mac,
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 63)

            send_packet(self, 2, str(pkt))
            verify_packets(self, exp_pkt, ports=[0])

            # Now bind the ACL table - the packet should be dropped
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_ipv4_id)
            attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_EGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_lag_attribute(lag1, attr)

            pkt = simple_tcp_packet(
              eth_dst = router_mac,
              eth_src = "00:22:22:22:22:22",
              ip_src = "20.0.0.2",
              ip_dst = "192.168.0.0",
              tcp_sport = 0x4321,
              tcp_dport = 0x51,
              ip_ttl = 64)
            send_packet(self, 2, str(pkt))
            verify_no_other_packets(self, timeout=1)

            # Add one more LAG member and verify the packet is not forwarded to it
            lag_member_id2 = sai_thrift_create_lag_member(self.client, lag1, port2)
            send_packet(self, 2, str(pkt))
            verify_no_other_packets(self, timeout=1)

            # Now bind the ACL table - the packet should be dropped
            attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
            attr = sai_thrift_attribute_t(id=SAI_LAG_ATTR_EGRESS_ACL, value=attr_value)
            self.client.sai_thrift_set_lag_attribute(lag1, attr)

            send_packet(self, 2, str(pkt))
            verify_any_packet_any_port(self, [exp_pkt], [0, 1], timeout=2)

        finally:
            self.client.sai_thrift_remove_acl_entry(acl_entry_id1)
            self.client.sai_thrift_remove_acl_table(acl_table_ipv4_id)

            # cleanup
            sai_thrift_remove_route(self.client, vr_id, addr_family, ip_addr1_subnet, ip_mask1_subnet, rif_id1)
            self.client.sai_thrift_remove_next_hop(nhop1)
            sai_thrift_remove_neighbor(self.client, addr_family, rif_id1, ip_addr1, dmac1)
            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)
            sai_thrift_remove_lag_member(self.client, lag_member_id1)
            if lag_member_id2:
                sai_thrift_remove_lag_member(self.client, lag_member_id2)
            self.client.sai_thrift_remove_lag(lag1)
            self.client.sai_thrift_remove_virtual_router(vr_id)
