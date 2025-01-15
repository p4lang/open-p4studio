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

#from ipaddress import *


#@group('etrap')
@disabled
class EtrapTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        v4_enabled = 1
        v6_enabled = 1
        mac_valid = 0
        mac = ''

        self.tc = 12
        self.qid = 2

        vr_id = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        rif_id1 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port1, 0, v4_enabled, v6_enabled, mac)
        rif_id2 = sai_thrift_create_router_interface(self.client, vr_id, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4_enabled, v6_enabled, mac)

        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_addr1 = '10.10.10.1'
        ip_addr1_subnet = '10.10.10.0'
        ip_mask1 = '255.255.255.0'
        ipv6_addr1 = '1234::99aa'
        ipv6_addr1_subnet = '1234::9900'
        ipv6_mask1 = 'FFFF:FFF:FFFF:FFFF:FFFF:FFFF:FFFF:FF00'
        dmac1 = '00:11:22:33:44:55'

        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, rif_id1, ip_addr1, dmac1)
        nhop1 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, ip_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, SAI_IP_ADDR_FAMILY_IPV4, ip_addr1_subnet, ip_mask1, rif_id1)

        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV6, rif_id1, ipv6_addr1, dmac1)
        nhop2 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV6, ipv6_addr1, rif_id1)
        sai_thrift_create_route(self.client, vr_id, SAI_IP_ADDR_FAMILY_IPV6, ipv6_addr1_subnet, ipv6_mask1, rif_id1)

        ingress_tc_list = [11, 12, 13, 14]
        ingress_queue_list = [1, 2, 3, 4]
        tc_qos_map_id = sai_thrift_create_qos_map(self.client, SAI_QOS_MAP_TYPE_TC_TO_QUEUE, ingress_tc_list, ingress_queue_list)

        sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)
        sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, tc_qos_map_id)

        sai_policer_id = sai_thrift_create_policer(self.client,
                                                   meter_type=SAI_METER_TYPE_BYTES,
                                                   mode=SAI_POLICER_MODE_SR_TCM,
                                                   cir=80,
                                                   cbs=0,
                                                   pir=0,
                                                   pbs=0,
                                                   red_action=SAI_PACKET_ACTION_FORWARD)


        entry_priority = 1
        ip_v4_src_prefix = ip_network('20.20.20.0/28')
        ip_v6_src_prefix = ip_network('2000::2000/124')

        self.acl_v4_table_id = sai_thrift_create_acl_table(self.client,
            SAI_ACL_STAGE_INGRESS,
            [SAI_ACL_BIND_POINT_TYPE_SWITCH],
            SAI_IP_ADDR_FAMILY_IPV4,
            ip_src = "20.20.20.1")

        self.acl_v4_entry_id = sai_thrift_create_acl_entry(self.client,
            self.acl_v4_table_id,
            entry_priority,
            addr_family=SAI_IP_ADDR_FAMILY_IPV4,
            ip_src=str(ip_v4_src_prefix.network_address),
            ip_src_mask=str(ip_v4_src_prefix.netmask),
            policer=sai_policer_id,
            tc=self.tc)

        self.acl_v6_table_id = sai_thrift_create_acl_table(self.client,
            SAI_ACL_STAGE_INGRESS,
            [SAI_ACL_BIND_POINT_TYPE_SWITCH],
            SAI_IP_ADDR_FAMILY_IPV6,
            ip_src = "2000::2001")

        self.acl_v6_entry_id = sai_thrift_create_acl_entry(self.client,
            self.acl_v6_table_id,
            entry_priority,
            addr_family=SAI_IP_ADDR_FAMILY_IPV6,
            ip_src=str(ip_v6_src_prefix.network_address),
            ip_src_mask=str(ip_v6_src_prefix.netmask),
            policer=sai_policer_id,
            tc=self.tc)

        try:
            assert(self.acl_v4_table_id != 0)
            assert(self.acl_v4_entry_id != 0)
            assert(self.acl_v6_table_id != 0)
            assert(self.acl_v6_entry_id != 0)

        finally:
            sai_thrift_set_port_attribute(self.client, port1, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_set_port_attribute(self.client, port2, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, SAI_NULL_OBJECT_ID)
            sai_thrift_remove_qos_map(self.client, tc_qos_map_id)
            self.client.sai_thrift_remove_acl_entry(self.acl_v4_entry_id)
            self.client.sai_thrift_remove_acl_entry(self.acl_v6_entry_id)
            self.client.sai_thrift_remove_acl_table(self.acl_v4_entry_id)
            self.client.sai_thrift_remove_acl_table(self.acl_v6_entry_id)
            self.client.sai_thrift_remove_policer(sai_policer_id)
            sai_thrift_remove_route(self.client, vr_id, SAI_IP_ADDR_FAMILY_IPV4, ip_addr1_subnet, ip_mask1, nhop1)
            sai_thrift_remove_route(self.client, vr_id, SAI_IP_ADDR_FAMILY_IPV6, ipv6_addr1_subnet, ipv6_mask1, nhop2)
            self.client.sai_thrift_remove_next_hop(nhop1)
            self.client.sai_thrift_remove_next_hop(nhop2)
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, rif_id1, ip_addr1, dmac1)
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV6, rif_id1, ipv6_addr1, dmac1)

            self.client.sai_thrift_remove_router_interface(rif_id1)
            self.client.sai_thrift_remove_router_interface(rif_id2)

            self.client.sai_thrift_remove_virtual_router(vr_id)
