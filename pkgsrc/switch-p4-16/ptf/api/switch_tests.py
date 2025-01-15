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
Thrift API interface basic tests
"""
import bf_switcht_api_thrift

import binascii

import time
import sys
import logging

import unittest
import random

import ptf.dataplane as dataplane
import api_base_tests
import pd_base_tests
from switch_helpers import ApiHelper
import model_utils as u
from p4testutils.misc_utils import mask_set_do_not_care_packet

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os
import ptf.mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from common.dtel_utils import *

import switch_dtel as dtel

acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
acl_table_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_SWITCH)
acl_group_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_PORT)

###############################################################################

@disabled
class RemoveSwitch(ApiHelper):
    def runTest(self):
        print('WARNING: Please restart driver and model to run any further tests after this')
        self.configure()
        for port in self.port_list:
            self.client.object_delete(port)
        self.client.object_delete(self.vlan10)
        self.client.object_delete(self.vlan20)
        self.client.object_delete(self.vlan30)
        self.client.object_delete(self.vlan40)
        self.client.object_delete(self.vrf10)
        self.client.bf_switcht_clean(False, "")

###############################################################################

@disabled
class OneTestToRuleThemAll(ApiHelper):
    '''
    This is not a feature test. Treat it like an integration test.
    Extend it so it will exercise all the ports/lags configured.
    Extend it so it will plays well with ACLs and other configurations.
    If a feature is per profile, enclose setup and test with feature_get.
    Store all data in the dicts at the beginning of the test.
    DO NOT USE hard-coded entries in the packet definitions.
    Limit attribute_set in the runTest part. This will test warm_init better.

    Data ports:
      L3 Port  : p1, p10
      L3 Lag   : l1, l4
      SVI Port : p4, p9, p13
      SVI Lag  : l2, l3, l5
    Monitor ports: p0

    Vlan10: p4, l2, p13
            u   t   u
    Vlan20: l3, p9, l5
            u   t   t

     1111::2      1212::2                1313::2                   1414::2
   ------------ ------------           ------------              ------------
    11.11.11.2   12.12.12.2             13.13.13.2                14.14.14.2
     1.1.1.2      2.2.2.2                3.3.3.2                   4.4.4.2
         |           |                       |                          |
    -------------------------------------------------------------------------
    |    p1     p2     p3          p4     p5    p6        p7     p8     p9  |
    |           |  l1   |                 |  l2  |         |  l3  |         |
    |           ---------                 --------         --------         |
    |                                        t                          t   |
    |                                                                       |   222.222.222.2
    |                                                                   p0  | -----------------
    |                                                                       |      2222:2
    |                                                             t         |
    |              ---------                                  ---------     |
    |              |  l4   |                                  |  l5   |     |
    |    p10       p11     p12           p13                  p14     p15   |
    -------------------------------------------------------------------------
         |              |                |                        |
      5.5.5.2       6.6.6.2          3.3.3.3                   4.4.4.3
     15.15.15.2    16.16.16.2       13.13.13.3               14.14.14.3
    ------------  ------------     ------------             ------------
       1515::2       1616::2          1313::3                  1414::3
    ACL:
      allow only host to host
      allow myip connections
      drop all non-host connections in the same network
      drop all connections from data to monitor port
      drop all connections from monitor to data port
      drop all ssh(22) connections from data ports
      allow ssh connection from monitor port
      drop all ipv6 nd packets
      drop all 3000-4000 src port v4 connections from data ports
      drop all 4000-5000 src port v6 connections from data ports
    '''

    # data dicts
    p0 =      {'myip': '222.222.222.1', 'nhop': '222.222.222.2', 'nhop2': '22.22.22.2', 'nhopv6': '2222::2', 'mac': '00:11:11:11:11:11', 'dp': [0]}
    p1 =      {'myip': '11.11.11.1', 'nhop': '11.11.11.2', 'nhop2': '1.1.1.2', 'nhopv6': '1111::2', 'mac': '00:22:22:22:22:01', 'dp': [1]}
    l1 =      {'myip': '12.12.12.1', 'nhop': '12.12.12.2', 'nhop2': '2.2.2.2', 'nhopv6': '1212::2', 'mac': '00:22:22:22:02:03', 'dp': [2,3]}
    p10 =     {'myip': '15.15.15.1', 'nhop': '15.15.15.2', 'nhop2': '5.5.5.2', 'nhopv6': '1515::2', 'mac': '00:22:22:22:22:10', 'dp': [10]}
    l4 =      {'myip': '16.16.16.1', 'nhop': '16.16.16.2', 'nhop2': '6.6.6.2', 'nhopv6': '1616::2', 'mac': '00:22:22:22:11:12', 'dp': [11,12]}
    v10_p4 =  {'mac': '00:22:22:22:22:04', 'dp': [4]}
    v10_l2 =  {'myip': '13.13.13.1', 'nhop': '13.13.13.2', 'nhop2': '3.3.3.2', 'nhopv6': '1313::2', 'mac': '00:22:22:22:05:06', 'dp': [5,6], 'tagged': True, 'vlan': 10}
    v10_p13 = {'myip': '13.13.13.1', 'nhop': '13.13.13.3', 'nhop2': '3.3.3.3', 'nhopv6': '1313::3', 'mac': '00:22:22:22:22:13', 'dp': [13]}
    v10 =     {'myip': '13.13.13.1', 'members': [v10_l2, v10_p4, v10_p13], 'vlan': 10}
    v20_l3 =  {'mac': '00:22:22:22:07:08', 'dp': [7,8]}
    v20_p9 =  {'myip': '14.14.14.1', 'nhop': '14.14.14.2', 'nhop2': '4.4.4.2', 'nhopv6': '1414::2', 'mac': '00:22:22:22:22:09', 'dp': [9], 'tagged': True, 'vlan': 20}
    v20_l5 =  {'myip': '14.14.14.1', 'nhop': '14.14.14.3', 'nhop2': '4.4.4.3', 'nhopv6': '1414::3', 'mac': '00:22:22:22:14:15', 'dp': [14,15], 'tagged': True, 'vlan': 20}
    v20 =     {'myip': '14.14.14.1', 'members': [v20_l3, v20_p9, v20_l5], 'vlan': 20}
    l3_intf = [p1, l1, p10, l4, v10_l2, v10_p13, v20_p9, v20_l5]

    def add_nhop(self, rif, intf_data):
        print(('Add %s nexthop with neighbor %s') % (intf_data['nhop'], intf_data['mac']))
        nhop = self.add_nexthop(self.device, handle=rif, dest_ip=intf_data['nhop'])
        neighbor = self.add_neighbor(self.device, mac_address=intf_data['mac'], handle=rif, dest_ip=intf_data['nhop'])
        self.add_route(self.device, ip_prefix=intf_data['nhop'], vrf_handle=self.vrf10, nexthop_handle=nhop)
        self.add_route(self.device, ip_prefix=intf_data['nhop2'], vrf_handle=self.vrf10, nexthop_handle=nhop)
        self.add_route(self.device, ip_prefix=intf_data['nhopv6'], vrf_handle=self.vrf10, nexthop_handle=nhop)
        return nhop

    def add_l3(self, intf, intf_data):
        print(('RIF with myip %s') % (intf_data['myip']))
        rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=intf,
            vrf_handle=self.vrf10, src_mac=self.rmac, mtu=1000)
        self.add_route(self.device, ip_prefix=intf_data['myip'], vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        return rif

    def add_svi(self, intf, intf_type, intf_data):
        print(('SVI with myip %s') % (intf_data['myip']))
        rif = self.add_rif(self.device, type=intf_type, vlan_handle=intf, vrf_handle=self.vrf10, src_mac=self.rmac, mtu=1000)
        self.add_route(self.device, ip_prefix=intf_data['myip'], vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        return rif

    def configure_lags(self):
        # add lag l1
        self.lag1 = self.add_lag(self.device)
        self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port2)
        self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port3)
        # add lag l2
        self.lag2 = self.add_lag(self.device)
        self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port5)
        self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port6)
        # add lag l3
        self.lag3 = self.add_lag(self.device)
        self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port7)
        self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port8)
        # add lag l4
        self.lag4 = self.add_lag(self.device)
        self.add_lag_member(self.device, lag_handle=self.lag4, port_handle=self.port11)
        self.add_lag_member(self.device, lag_handle=self.lag4, port_handle=self.port12)
        # add lag l5
        self.lag5 = self.add_lag(self.device)
        self.add_lag_member(self.device, lag_handle=self.lag5, port_handle=self.port14)
        self.add_lag_member(self.device, lag_handle=self.lag5, port_handle=self.port15)

    def setUp(self):
        print()
        self.configure()
        self.configure_lags()

        self.nhop_glean = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)
        self.nhop_drop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_DROP)

        # port 0 will act as mirror/dtel port
        self.rif_port0 = self.add_l3(self.port0, self.p0)
        self.nh_port0 = self.add_nhop(self.rif_port0, self.p0)

        print('#### Begin L3 intf configuration ####')
        self.rif_port1 = self.add_l3(self.port1, self.p1)
        self.nh_port1 = self.add_nhop(self.rif_port1, self.p1)
        self.rif_port10 = self.add_l3(self.port10, self.p10)
        self.nh_port10 = self.add_nhop(self.rif_port10, self.p10)
        self.rif_lag1 = self.add_l3(self.lag1, self.l1)
        self.nh_lag1 = self.add_nhop(self.rif_lag1, self.l1)
        self.rif_lag4 = self.add_l3(self.lag4, self.l4)
        self.nh_lag4 = self.add_nhop(self.rif_lag4, self.l4)

        print('#### Begin Vlan 10 configuration ####')
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port13)
        self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.rif_vlan10 = self.add_svi(self.vlan10, SWITCH_RIF_ATTR_TYPE_VLAN, self.v10)
        self.nh_lag2 = self.add_nhop(self.rif_vlan10, self.v10_l2)
        self.nh_port13 = self.add_nhop(self.rif_vlan10, self.v10_p13)
        # add static MACs for vlan 10 members
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address=self.v10_l2['mac'], destination_handle=self.lag2)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address=self.v10_p13['mac'], destination_handle=self.port13)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address=self.v10_p4['mac'], destination_handle=self.port4)

        print('#### Begin Vlan 20 configuration ####')
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag3)
        self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 20)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port9, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag5, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.rif_vlan20 = self.add_svi(self.vlan20, SWITCH_RIF_ATTR_TYPE_VLAN, self.v20)
        self.nh_port9 = self.add_nhop(self.rif_vlan20, self.v20_p9)
        self.nh_lag5 = self.add_nhop(self.rif_vlan20, self.v20_l5)
        #add static MACs for vlan 20 members
        self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address=self.v20_p9['mac'], destination_handle=self.port9)
        self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address=self.v20_l5['mac'], destination_handle=self.lag5)
        self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address=self.v20_l3['mac'], destination_handle=self.lag3)

        print('#### Begin hostif configuration ####')
        self.hostif_p1 = self.add_hostif(self.device, name="port1", handle=self.port1, oper_status=True, ip_addr=self.p1['myip']+'/24', mac=self.rmac)
        self.hostif_p10 = self.add_hostif(self.device, name="port10", handle=self.port10, oper_status=True, ip_addr=self.p10['myip']+'/24', mac=self.rmac)
        self.hostif_l1 = self.add_hostif(self.device, name="lag1", handle=self.lag1, oper_status=True, ip_addr=self.l1['myip']+'/24', mac=self.rmac)
        self.hostif_l4 = self.add_hostif(self.device, name="lag4", handle=self.lag4, oper_status=True, ip_addr=self.l4['myip']+'/24', mac=self.rmac)

        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.meter0 = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            pbs=16,cbs=8,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
        self.meter1 = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            pbs=8,cbs=4,cir=500,pir=500,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1, policer_handle=self.meter0)
        self.hostif_trap_group1 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1, policer_handle=self.meter1)
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) != 0):
            trap_list = [
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, self.hostif_trap_group0],
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, self.hostif_trap_group0],
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, self.hostif_trap_group1],
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_SSH, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, self.hostif_trap_group1],
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, self.hostif_trap_group1]
            ]
        else:
            trap_list = [
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, self.hostif_trap_group0],
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, self.hostif_trap_group0],
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_SSH, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, self.hostif_trap_group1],
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, self.hostif_trap_group1]
            ]
        for trap in trap_list:
            trap.append(self.add_hostif_trap(self.device, type=trap[0], hostif_trap_group_handle=trap[2], packet_action=trap[1]))

        print('#### Begin ECMP configuration ####')
        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nh_port1, ecmp_handle=ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.nh_lag1, ecmp_handle=ecmp0)
        ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=self.nh_lag2, ecmp_handle=ecmp0)
        ecmp_member04 = self.add_ecmp_member(self.device, nexthop_handle=self.nh_port9, ecmp_handle=ecmp0)
        self.add_route(self.device, ip_prefix='111.111.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmp0)

        print('#### Begin mirror configuration ####')
        self.ing_mirrors = []
        self.eg_mirrors = []
        for port in self.port_list[1:]:
            ing_mirror = self.add_mirror(self.device, type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS, egress_port_handle=self.port0)
            self.ing_mirrors.append(ing_mirror)
            eg_mirror = self.add_mirror(self.device, type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS, egress_port_handle=self.port0)
            self.eg_mirrors.append(eg_mirror)

        print('#### Begin ACL table/group configuration ####')
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_v6_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_mirror_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        for port in [self.port0, self.port1, self.port4, self.port9, self.port10, self.port13]:
            acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
            self.add_acl_group_member(self.device,
                acl_table_handle=acl_table, acl_group_handle=acl_group)
            self.add_acl_group_member(self.device,
                acl_table_handle=acl_v6_table, acl_group_handle=acl_group)
            self.add_acl_group_member(self.device,
                acl_table_handle=acl_mirror_table, acl_group_handle=acl_group)
            self.attribute_set(port, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group)
        for lag in [self.lag1, self.lag2, self.lag3, self.lag4, self.lag5]:
            acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
            self.add_acl_group_member(self.device,
                acl_table_handle=acl_table, acl_group_handle=acl_group)
            self.add_acl_group_member(self.device,
                acl_table_handle=acl_v6_table, acl_group_handle=acl_group)
            self.add_acl_group_member(self.device,
                acl_table_handle=acl_mirror_table, acl_group_handle=acl_group)
            self.attribute_set(lag, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, acl_group)

        print('#### Begin ACL entry configuration ####')
        # allow host entries
        for intf in self.l3_intf:
            self.add_acl_entry(self.device,
                dst_ip=intf['nhop'],
                dst_ip_mask='255.255.255.255',
                priority=0xf,
                table_handle=acl_table)
            self.add_acl_entry(self.device,
                dst_ip=intf['nhopv6'],
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                priority=0xf,
                table_handle=acl_v6_table)
        # allow myip packets
        for intf in [self.p1, self.l1, self.p10, self.l4, self.v10_l2, self.v20_l5]:
            self.add_acl_entry(self.device,
                dst_ip=intf['myip'],
                dst_ip_mask='255.255.255.255',
                priority=0xf,
                table_handle=acl_table)
        for intf in [self.p1, self.l1, self.p10, self.l4, self.v10_l2, self.v20_l5]:
            # deny all other in same network
            self.add_acl_entry(self.device,
                dst_ip=intf['nhop'][:-1] + '0',
                dst_ip_mask='255.255.255.0',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                priority=0xffff,
                table_handle=acl_table)
            self.add_acl_entry(self.device,
                dst_ip=intf['nhopv6'][:-1] + '0',
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0000',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                priority=0xffff,
                table_handle=acl_v6_table)
            # deny all connections from monitor to data ports
            self.add_acl_entry(self.device,
                src_ip='222.222.222.0',
                src_ip_mask='255.255.255.0',
                dst_ip=intf['nhop'][:-1] + '0',
                dst_ip_mask='255.255.255.0',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                priority=0,
                table_handle=acl_table)
            self.add_acl_entry(self.device,
                src_ip='2222::0',
                src_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0000',
                dst_ip=intf['nhopv6'][:-1] + '0',
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0000',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                priority=0,
                table_handle=acl_v6_table)
        # deny all packets destined to the mirror port from data ports
        self.add_acl_entry(self.device,
            dst_ip='222.222.222.0',
            dst_ip_mask='255.255.255.0',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.add_acl_entry(self.device,
            dst_ip='2222::0',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0000',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_v6_table)
        # drop packets in src port range 3000-4000
        src = switcht_range_t(min=3000, max=4000)
        src_range = self.add_acl_range(self.device,
            type=SWITCH_ACL_RANGE_ATTR_TYPE_SRC_PORT, range=src)
        self.add_acl_entry(self.device,
            src_port_range_id=src_range,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        src = switcht_range_t(min=4000, max=5000)
        src_range_v6 = self.add_acl_range(self.device,
            type=SWITCH_ACL_RANGE_ATTR_TYPE_SRC_PORT, range=src)
        self.add_acl_entry(self.device,
            src_port_range_id=src_range_v6,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_v6_table)
        # drop ipv6 ND packets
        src = switcht_range_t(min=133, max=137)
        src_range = self.add_acl_range(self.device,
            type=SWITCH_ACL_RANGE_ATTR_TYPE_SRC_PORT, range=src)
        self.add_acl_entry(self.device,
            src_port_range_id=src_range,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_v6_table)
        # drop ssh packets
        self.add_acl_entry(self.device,
            l4_dst_port=22, l4_dst_port_mask=0xff,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.add_acl_entry(self.device,
            l4_dst_port=22, l4_dst_port_mask=0xff,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_v6_table)

        # TODO
        print('#### Begin ECMP/LAG hash configuration ####')
        print('#### Begin QoS configuration ####')

        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
          print("DTEL not tested")
          return

        print('#### Begin DTEL configuration ####')
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_SWITCH_ID, dtel.switch_id)
        self.dtel = self.add_dtel(self.device,
            drop_report=True, queue_report=True, flow_report=True, tail_drop_report=True)
        report_dst_ip_list = []
        report_dst_ip = switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr='22.22.22.2')
        report_dst_ip_list.append(report_dst_ip)
        self.report_session = self.add_report_session(
            self.device, udp_dst_port=UDP_PORT_DTEL_REPORT,
            src_ip='22.22.22.1', vrf_handle=self.vrf10,
            dst_ip_list=report_dst_ip_list, truncate_size=512, ttl=64)
        mult = 4
        if self.arch == 'tofino2':
            mult = 8
        self.fp = []
        for port in self.port_list:
            fport = self.attribute_get(port, SWITCH_PORT_ATTR_CONNECTOR_ID) * mult
            fport = fport + self.attribute_get(port, SWITCH_PORT_ATTR_CHANNEL_ID)
            self.fp.append(fport)
        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, dtel_acl_table)
        for intf in [self.p1, self.l1, self.p10, self.l4, self.v10_l2, self.v20_l5]:
            self.add_acl_entry(self.device,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                dst_ip=intf['nhop2'][:-1] + '0',
                dst_ip_mask='255.255.255.0',
                l4_dst_port=4040, l4_dst_port_mask=0x7fff,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=dtel_acl_table,
                priority=10)
            self.add_acl_entry(self.device,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                dst_ip=intf['nhop2'][:-1] + '0',
                dst_ip_mask='255.255.255.0',
                l4_dst_port=4041, l4_dst_port_mask=0x7fff,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=dtel_acl_table,
                priority=10)
            dtel.params.report_ports = [0]
            dtel.params.mac_nbr = [self.p0['mac']]
            dtel.params.ipaddr_report_src = ['22.22.22.1']
            dtel.params.ipaddr_report_dst = ['22.22.22.2']

    def runTest(self):
        print('#### Begin tests ####')
        try:
            self.L2Test()
            self.FloodTest()
            self.L2PVMissTest()
            self.L3Test()
            self.IngMirrorTest()
            self.EgMirrorTest()
            self.MtuTest()
            self.IngressACLTest()
            self.LBTest()
            if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) != 0):
                self.DtelTest()
            # run hostif last cuz of the garps
            self.HostifTest()
            pass
        finally:
            pass

    def tearDown(self):
        for port in [self.port0, self.port1, self.port4, self.port9, self.port10, self.port13]:
            self.attribute_set(port, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        for lag in [self.lag1, self.lag2, self.lag3, self.lag4, self.lag5]:
            self.attribute_set(lag, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.hostif_trap_group1, SWITCH_HOSTIF_TRAP_GROUP_ATTR_POLICER_HANDLE, 0)
        self.attribute_set(self.hostif_trap_group0, SWITCH_HOSTIF_TRAP_GROUP_ATTR_POLICER_HANDLE, 0)
        self.cleanup()

    def pkt_params(self, intf, plen=100):
        tagged = False
        pkt_len = plen
        vlan_id = 0
        if 'tagged' in list(intf.keys()):
            tagged = True
            pkt_len += 4
            vlan_id = intf['vlan']
        return tagged, pkt_len, vlan_id

    def make_l3_pkt(self, src_intf, dst_intf, plen=100, nhop2=False):
        src_tagged, src_pkt_len, src_vlan_id = self.pkt_params(src_intf, plen)
        dst_tagged, dst_pkt_len, dst_vlan_id = self.pkt_params(dst_intf, plen)
        nhop = 'nhop2' if nhop2 else 'nhop2'
        pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            eth_src=src_intf['mac'],
            ip_dst=dst_intf[nhop],
            ip_src=src_intf[nhop],
            ip_ttl=64,
            dl_vlan_enable=src_tagged,
            vlan_vid=src_vlan_id,
            pktlen=src_pkt_len)
        exp_pkt = simple_tcp_packet(
            eth_dst=dst_intf['mac'],
            eth_src=self.rmac,
            ip_dst=dst_intf[nhop],
            ip_src=src_intf[nhop],
            ip_ttl=63,
            dl_vlan_enable=dst_tagged,
            vlan_vid=dst_vlan_id,
            pktlen=dst_pkt_len)
        cpu_pkt = simple_cpu_packet(
            ingress_port=0,
            packet_type=0,
            ingress_ifindex=0,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
            ingress_bd=0x02,
            inner_pkt=exp_pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
        return pkt, exp_pkt, cpu_pkt

    def make_l3_v6_pkt(self, src_intf, dst_intf, plen=100):
        src_tagged, src_pkt_len, src_vlan_id = self.pkt_params(src_intf, plen)
        dst_tagged, dst_pkt_len, dst_vlan_id = self.pkt_params(dst_intf, plen)
        pkt = simple_tcpv6_packet(
            eth_dst=self.rmac,
            eth_src=src_intf['mac'],
            ipv6_dst=dst_intf['nhopv6'],
            ipv6_src=src_intf['nhopv6'],
            ipv6_hlim=64,
            dl_vlan_enable=src_tagged,
            vlan_vid=src_vlan_id,
            pktlen=src_pkt_len)
        exp_pkt = simple_tcpv6_packet(
            eth_dst=dst_intf['mac'],
            eth_src=self.rmac,
            ipv6_dst=dst_intf['nhopv6'],
            ipv6_src=src_intf['nhopv6'],
            ipv6_hlim=63,
            dl_vlan_enable=dst_tagged,
            vlan_vid=dst_vlan_id,
            pktlen=dst_pkt_len)
        cpu_pkt = simple_cpu_packet(
            ingress_port=0,
            packet_type=0,
            ingress_ifindex=0,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
            ingress_bd=0x02,
            inner_pkt=exp_pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
        return pkt, exp_pkt, cpu_pkt

    def L3Test(self):
        print('L3: V4 Unicast')
        for src_intf in self.l3_intf:
            for dst_intf in self.l3_intf:
                if src_intf == dst_intf:
                    continue
                pkt, exp_pkt, _ = self.make_l3_pkt(src_intf, dst_intf)
                for src_port in src_intf['dp']:
                    logging.debug('port %d [%s] -> port %s [%s]',
                        src_port, src_intf['nhop'], str(dst_intf['dp'])[1:-1], dst_intf['nhop'])
                    send_packet(self, self.devports[src_port], pkt)
                    verify_packet_any_port(self, exp_pkt, [self.devports[x] for x in dst_intf['dp']])
        print('L3: V6 Unicast')
        for src_intf in self.l3_intf:
            for dst_intf in self.l3_intf:
                if src_intf == dst_intf:
                    continue
                pkt, exp_pkt, _ = self.make_l3_v6_pkt(src_intf, dst_intf)
                for src_port in src_intf['dp']:
                    logging.debug('port %d [%s] -> port %s [%s]',
                        src_port, src_intf['nhopv6'], str(dst_intf['dp'])[1:-1], dst_intf['nhopv6'])
                    send_packet(self, self.devports[src_port], pkt)
                    verify_packet_any_port(self, exp_pkt, [self.devports[x] for x in dst_intf['dp']])

    def IngMirrorTest(self):
        print('Mirror: Ingress')
        try:
            for port, mirror in zip(self.port_list[1:], self.ing_mirrors):
                self.attribute_set(port, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)
            test_intfs = [self.p1, self.p10, self.v10_p13, self.v20_p9]
            for src_intf in test_intfs:
                for dst_intf in test_intfs:
                    if src_intf == dst_intf:
                        continue
                    pkt, exp_pkt, _ = self.make_l3_pkt(src_intf, dst_intf)
                    for src_port in src_intf['dp']:
                        logging.debug('port %d [%s] -> port [%s] [%s], ing mirror port %d',
                            src_port, src_intf['nhop'], str(dst_intf['dp'])[1:-1], dst_intf['nhop'], self.devports[0])
                        send_packet(self, self.devports[src_port], pkt)
                        verify_each_packet_on_each_port(self, [exp_pkt, pkt],
                            [self.devports[x] for x in dst_intf['dp']] + [self.devports[0]])
                    pkt, exp_pkt, _ = self.make_l3_v6_pkt(src_intf, dst_intf)
                    for src_port in src_intf['dp']:
                        logging.debug('port %d [%s] -> port %s [%s], ing mirror port %d',
                            src_port, src_intf['nhopv6'], str(dst_intf['dp'])[1:-1], dst_intf['nhopv6'], self.devports[0])
                        send_packet(self, self.devports[src_port], pkt)
                        verify_each_packet_on_each_port(self, [exp_pkt, pkt],
                            [self.devports[x] for x in dst_intf['dp']] + [self.devports[0]])
        finally:
            for port in self.port_list[1:]:
                self.attribute_set(port, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)

    def EgMirrorTest(self):
        print('Mirror: Egress')
        try:
            for port, mirror in zip(self.port_list[1:], self.eg_mirrors):
                self.attribute_set(port, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, mirror)
            test_intfs = [self.p1, self.p10, self.v10_p13, self.v20_p9]
            for src_intf in test_intfs:
                for dst_intf in test_intfs:
                    if src_intf == dst_intf:
                        continue
                    pkt, exp_pkt, _ = self.make_l3_pkt(src_intf, dst_intf)
                    for src_port in src_intf['dp']:
                        logging.debug('port %d [%s] -> port %s [%s], eg mirror port %d',
                            src_port, src_intf['nhop'], str(dst_intf['dp'])[1:-1], dst_intf['nhop'], self.devports[0])
                        send_packet(self, self.devports[src_port], pkt)
                        verify_each_packet_on_each_port(self, [exp_pkt, exp_pkt],
                            [self.devports[x] for x in dst_intf['dp']] + [self.devports[0]])
                    pkt, exp_pkt, _ = self.make_l3_v6_pkt(src_intf, dst_intf)
                    for src_port in src_intf['dp']:
                        logging.debug('port %d [%s] -> port %s [%s], eg mirror port %d',
                            src_port, src_intf['nhopv6'], str(dst_intf['dp'])[1:-1], dst_intf['nhopv6'], self.devports[0])
                        send_packet(self, self.devports[src_port], pkt)
                        verify_each_packet_on_each_port(self, [exp_pkt, exp_pkt],
                            [self.devports[x] for x in dst_intf['dp']] + [self.devports[0]])
        finally:
            for port in self.port_list[1:]:
                self.attribute_set(port, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)

    def MtuTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) != 0):
            print('L3: V4 MTU check fail')
            # Test egress copy to CPU mirror
            for src_intf in self.l3_intf:
                for dst_intf in self.l3_intf:
                    if src_intf == dst_intf:
                        continue
                    pkt, _, cpu_pkt = self.make_l3_pkt(src_intf, dst_intf, 1200)
                    for src_port in src_intf['dp']:
                        logging.debug('port %d [%s] -> cpu_port MTU error', src_port, src_intf['nhop'])
                        send_packet(self, self.devports[src_port], pkt)
                        verify_packet(self, cpu_pkt, self.cpu_port)
            print('L3: V6 MTU check fail')
            for src_intf in self.l3_intf:
                for dst_intf in self.l3_intf:
                    if src_intf == dst_intf:
                        continue
                    pkt, _, cpu_pkt = self.make_l3_v6_pkt(src_intf, dst_intf, 1200)
                    for src_port in src_intf['dp']:
                        logging.debug('port %d [%s] -> cpu_port MTU error', src_port, src_intf['nhopv6'])
                        send_packet(self, self.devports[src_port], pkt)
                        verify_packet(self, cpu_pkt, self.cpu_port)

    def IngressACLTest(self):
        try:
            print('ACL: v4 Ingress')
            for intf in self.l3_intf:
                tagged, pkt_len, vlan_id = self.pkt_params(intf)
                pkt = simple_tcp_packet(
                        eth_dst=self.rmac,
                        eth_src=intf['mac'],
                        ip_dst=self.p0['nhop'],
                        ip_src=intf['nhop'],
                        dl_vlan_enable=tagged,
                        vlan_vid=vlan_id,
                        pktlen=pkt_len)
                for src_port in intf['dp']:
                    logging.debug('port %d [%s] -> port %d [%s], drop',
                        src_port, intf['nhop'], self.devports[0], self.p0['nhop'])
                    send_packet(self, self.devports[src_port], pkt)
                    verify_no_other_packets(self, timeout=0.1)
                pkt = simple_tcp_packet(
                        eth_dst=self.rmac,
                        eth_src=intf['mac'],
                        ip_dst=self.p1['nhop'],
                        ip_src=intf['nhop'],
                        tcp_dport=22,
                        dl_vlan_enable=tagged,
                        vlan_vid=vlan_id,
                        pktlen=pkt_len)
                for src_port in intf['dp']:
                    logging.debug('port %d [%s] -> [%s] dport 22, drop',
                        src_port, intf['nhop'], self.p1['nhop'])
                    send_packet(self, self.devports[src_port], pkt)
                    verify_no_other_packets(self, timeout=0.1)
                for i in range(0,4):
                    sport = random.randrange(3001, 4000)
                    pkt = simple_tcp_packet(
                          eth_dst=self.rmac,
                          eth_src=intf['mac'],
                          ip_dst=self.p1['nhop'],
                          ip_src=intf['nhop'],
                          tcp_sport=sport,
                          dl_vlan_enable=tagged,
                          vlan_vid=vlan_id,
                          pktlen=pkt_len)
                    for src_port in intf['dp']:
                        logging.debug('port %d [%s] -> [%s] sport %d, drop',
                          src_port, intf['nhop'], self.p1['nhop'], sport)
                        send_packet(self, self.devports[src_port], pkt)
                        verify_no_other_packets(self, timeout=0.1)
                pkt = simple_tcp_packet(
                        eth_dst=self.rmac,
                        eth_src=self.p0['mac'],
                        ip_dst=intf['nhop'],
                        ip_src=self.p0['nhop'],
                        pktlen=pkt_len)
                logging.debug('port %s [%s] -> [%s], drop',
                    self.p0['dp'], self.p0['nhop'], intf['nhop'])
                send_packet(self, self.devports[0], pkt)
                verify_no_other_packets(self, timeout=0.1)
            print('ACL: v6 Ingress')
            for intf in self.l3_intf:
                tagged, pkt_len, vlan_id = self.pkt_params(intf)
                pkt_v6 = simple_tcpv6_packet(
                        eth_dst=self.rmac,
                        eth_src=intf['mac'],
                        ipv6_dst=self.p0['nhopv6'],
                        ipv6_src=intf['nhopv6'],
                        dl_vlan_enable=tagged,
                        vlan_vid=vlan_id,
                        pktlen=pkt_len)
                for src_port in intf['dp']:
                    logging.debug('port %d [%s] -> port %d [%s], drop',
                        src_port, intf['nhopv6'], self.devports[0], self.p0['nhopv6'])
                    send_packet(self, self.devports[src_port], pkt_v6)
                    verify_no_other_packets(self, timeout=0.1)
                pkt_v6 = simple_tcpv6_packet(
                        eth_dst=self.rmac,
                        eth_src=intf['mac'],
                        ipv6_dst=self.p1['nhopv6'],
                        ipv6_src=intf['nhopv6'],
                        tcp_dport=22,
                        dl_vlan_enable=tagged,
                        vlan_vid=vlan_id,
                        pktlen=pkt_len)
                for src_port in intf['dp']:
                    logging.debug('port %d [%s] -> [%s] dport 22, drop',
                        src_port, intf['nhopv6'], self.p1['nhopv6'])
                    send_packet(self, self.devports[src_port], pkt_v6)
                    verify_no_other_packets(self, timeout=0.1)
                for i in range(0,4):
                    sportv6 = random.randrange(4001, 5000)
                    pkt_v6 = simple_tcpv6_packet(
                          eth_dst=self.rmac,
                          eth_src=intf['mac'],
                          ipv6_dst=self.p1['nhopv6'],
                          ipv6_src=intf['nhopv6'],
                          tcp_sport=sportv6,
                          dl_vlan_enable=tagged,
                          vlan_vid=vlan_id,
                          pktlen=pkt_len)
                    for src_port in intf['dp']:
                        logging.debug('port %d [%s] -> [%s] sport %d, drop',
                          src_port, intf['nhopv6'], self.p1['nhopv6'], sportv6)
                        send_packet(self, self.devports[src_port], pkt_v6)
                        verify_no_other_packets(self, timeout=0.1)
        finally:
            pass

    def make_l2_pkt(self, src_intf, dst_intf):
        pkts = []
        src_tagged, src_pkt_len, src_vlan_id = self.pkt_params(src_intf)
        dst_tagged, dst_pkt_len, dst_vlan_id = self.pkt_params(dst_intf)
        pkt = simple_tcp_packet(
                eth_dst=dst_intf['mac'],
                eth_src=src_intf['mac'],
                ip_dst='192.168.1.1',
                ip_src='192.168.1.2',
                dl_vlan_enable=src_tagged,
                vlan_vid=src_vlan_id,
                pktlen=src_pkt_len)
        exp_pkt = simple_tcp_packet(
                eth_dst=dst_intf['mac'],
                eth_src=src_intf['mac'],
                ip_dst='192.168.1.1',
                ip_src='192.168.1.2',
                dl_vlan_enable=dst_tagged,
                vlan_vid=dst_vlan_id,
                pktlen=dst_pkt_len)
        return pkt, exp_pkt

    def make_l2_v6_pkt(self, src_intf, dst_intf):
        pkts = []
        src_tagged, src_pkt_len, src_vlan_id = self.pkt_params(src_intf)
        dst_tagged, dst_pkt_len, dst_vlan_id = self.pkt_params(dst_intf)
        pkt = simple_tcpv6_packet(
                eth_dst=dst_intf['mac'],
                eth_src=src_intf['mac'],
                ipv6_dst='3333::2',
                ipv6_src='4444::2',
                dl_vlan_enable=src_tagged,
                vlan_vid=src_vlan_id,
                pktlen=src_pkt_len)
        exp_pkt = simple_tcpv6_packet(
                eth_dst=dst_intf['mac'],
                eth_src=src_intf['mac'],
                ipv6_dst='3333::2',
                ipv6_src='4444::2',
                dl_vlan_enable=dst_tagged,
                vlan_vid=dst_vlan_id,
                pktlen=dst_pkt_len)
        return pkt, exp_pkt

    def L2Test(self):
        print('L2: V4 Unicast')
        for vlan in [self.v10, self.v20]:
            for src_intf in vlan['members']:
                for dst_intf in vlan['members']:
                    if src_intf == dst_intf:
                        continue
                    pkt, exp_pkt = self.make_l2_pkt(src_intf, dst_intf)
                    for src_port in src_intf['dp']:
                        logging.debug('v4 vlan %d: port %d %s -> port %s %s',
                            vlan['vlan'], src_port, src_intf['mac'], dst_intf['dp'], dst_intf['mac'])
                        send_packet(self, self.devports[src_port], pkt)
                        verify_packet_any_port(self, exp_pkt, [self.devports[x] for x in dst_intf['dp']])
                    pkt, exp_pkt = self.make_l2_v6_pkt(src_intf, dst_intf)
                    for src_port in src_intf['dp']:
                        logging.debug('v6 vlan %d: port %d %s -> port %s %s',
                            vlan['vlan'], src_port, src_intf['mac'], dst_intf['dp'], dst_intf['mac'])
                        send_packet(self, self.devports[src_port], pkt)
                        verify_packet_any_port(self, exp_pkt, [self.devports[x] for x in dst_intf['dp']])

    def FloodTest(self):
        print('L2: Flooding')
        # Test PRE and L1/L2 pruning
        for vlan in [self.v10, self.v20]:
            for src_intf in vlan['members']:
                dst_intf_list = [item for item in vlan['members'] if item != src_intf]
                pkts = []
                for intf in [src_intf] + dst_intf_list:
                    tagged, pkt_len, vlan_id = self.pkt_params(intf)
                    pkt = simple_tcp_packet(
                          eth_dst='00:22:22:22:22:22',
                          eth_src=src_intf['mac'],
                          ip_dst='192.168.1.1',
                          ip_src='192.168.1.2',
                          dl_vlan_enable=tagged,
                          vlan_vid=vlan_id,
                          pktlen=pkt_len)
                    pkts.append(pkt)
                dst_ports = [[self.devports[x] for x in item['dp']] for item in dst_intf_list]
                for src_port in src_intf['dp']:
                    logging.debug('vlan %d: port %d -> port %s', vlan['vlan'], src_port, dst_ports)
                    send_packet(self, self.devports[src_port], pkts[0])
                    verify_any_packet_on_ports_list(self, pkts[1:], dst_ports)

    def L2PVMissTest(self):
        print('L2: PV miss')
        # Test registers via vlan_membership
        for vlan in [self.v10, self.v20]:
            for src_intf in vlan['members']:
                for dst_intf in vlan['members']:
                    if src_intf == dst_intf:
                        continue
                    tagged, pkt_len, vlan_id = self.pkt_params(src_intf)
                    pkt = simple_tcp_packet(
                            eth_dst='00:22:22:22:22:22',
                            eth_src=src_intf['mac'],
                            ip_dst='192.168.1.1',
                            ip_src='192.168.1.2',
                            dl_vlan_enable=True,
                            vlan_vid=30,
                            pktlen=pkt_len)
                    for src_port in src_intf['dp']:
                        logging.debug('vlan %d: tagged %s in_port %d -> drop', vlan['vlan'], tagged, src_port)
                        send_packet(self, self.devports[src_port], pkt)
                        verify_no_other_packets(self, timeout=0.1)

    def PingPortTest(self, sender_ip, intf_ip, host_mac, ingress_port, egress_port_list):
        try:
            # test arp resoultion.
            logging.debug("port %d: ARP to %s: recv on %s", ingress_port, intf_ip, egress_port_list)
            pkt = simple_arp_packet(
                        arp_op=1,
                        pktlen=100,
                        eth_src=host_mac,
                        hw_snd=host_mac,
                        ip_snd=sender_ip,
                        ip_tgt=intf_ip)
            exp_pkt = simple_arp_packet(
                        pktlen=42,
                        arp_op=2,
                        eth_src=self.rmac,
                        eth_dst=host_mac,
                        hw_snd=self.rmac,
                        hw_tgt=host_mac,
                        ip_snd=intf_ip,
                        ip_tgt=sender_ip)
            send_packet(self, ingress_port, pkt)
            verify_packet_any_port(self, exp_pkt, egress_port_list)
            time.sleep(1)

            # test ping
            logging.debug("port %d: ICMP to %s: recv on %s", ingress_port, intf_ip, egress_port_list)
            pkt = simple_icmp_packet(
                        eth_src=host_mac,
                        eth_dst=self.rmac,
                        ip_src=sender_ip,
                        ip_dst=intf_ip,
                        icmp_type=8,
                        icmp_data='000102030405')
            exp_pkt= simple_icmp_packet(
                        eth_src=self.rmac,
                        eth_dst=host_mac,
                        ip_src=intf_ip,
                        ip_dst=sender_ip,
                        icmp_type=0,
                        icmp_data='000102030405')
            m = ptf.mask.Mask(exp_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")

            send_packet(self, ingress_port, pkt)
            verify_packet_any_port(self, m, egress_port_list)
            time.sleep(1)
        finally:
            pass

    def HostifTest(self):
        print('HostifTest: Ping')
        # Test ingress TM copy to CPU
        for intf in [self.p1, self.p10]:
            for src_port in intf['dp']:
                self.PingPortTest(intf['nhop'], intf['myip'], intf['mac'],
                  self.devports[src_port], [self.devports[x] for x in intf['dp']])

    def DtelTest(self):
        print('DtelTest: Flow/Drop')
        time.sleep(4)
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_INT_V2) == 0):
                self.int_v2 = False
            else:
                self.int_v2 = True
            bind_postcard_pkt(self.int_v2)
            for intf in [self.p10, self.v10_p13, self.v20_p9]:
                logging.debug("Flow: port %s -> %s/4041 recv on %s", self.p1['nhop2'], intf['nhop2'], intf['dp'])
                pkt, exp_pkt, _ = self.make_l3_pkt(self.p1, intf, nhop2=True)
                pkt[TCP].dport = 4041
                exp_pkt[TCP].dport = 4041
                send_packet(self, self.devports[1], pkt)
                verify_packet(self, exp_pkt, self.devports[intf['dp'][0]])
                verify_postcard_packet(self,
                    dtel.exp_postcard_packet(exp_pkt, self.int_v2, self.fp[1], self.fp[intf['dp'][0]], 1, f=1),
                    self.devports[0])
            split_postcard_pkt(self.int_v2)
            bind_mirror_on_drop_pkt(self.int_v2)
            for intf in self.l3_intf[1:]:
                logging.debug("Drop: port %s -> %s/4040", self.p1['nhop2'], intf['nhop2'])
                pkt, exp_pkt, _ = self.make_l3_pkt(self.p1, intf, nhop2=True)
                pkt[IP].version = 6
                pkt[TCP].dport = 4040
                send_packet(self, self.devports[1], pkt)
                verify_dtel_packet(self,
                    dtel.exp_mod_packet(pkt, self.int_v2, self.fp[1], 4, 25),
                    self.devports[0])
            split_mirror_on_drop_pkt(self.int_v2)
        finally:
            pass

    def LBTest(self):
        print("LBTest()")
        try:
            count = [0, 0, 0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('111.111.0.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('100.100.0.2')), 16)
            max_itrs = 300
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt = simple_tcp_packet(
                    eth_dst=self.rmac,
                    eth_src=self.p10['mac'],
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_id=106,
                    ip_ttl=64,
                    pktlen=100)
                exp_pkts = []
                for intf in [self.p1, self.l1, self.v10_l2, self.v20_p9]:
                    tagged, pkt_len, vlan_id = self.pkt_params(intf)
                    exp_pkts.append(simple_tcp_packet(
                        eth_dst=intf['mac'],
                        eth_src=self.rmac,
                        ip_dst=dst_ip_addr,
                        ip_src=src_ip_addr,
                        dl_vlan_enable=tagged,
                        vlan_vid=vlan_id,
                        ip_id=106,
                        ip_ttl=63,
                        pktlen=pkt_len))
                send_packet(self, self.devports[10], pkt)
                rcv_idx = verify_any_packet_any_port(self, exp_pkts, [
                        self.devports[1],
                        self.devports[2], self.devports[3],
                        self.devports[5], self.devports[6],
                        self.devports[9]
                    ], timeout=0.5)
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1
            ecmp_count = [
                (count[0]), (count[1] + count[2]),
                (count[3] + count[4]), (count[5])]
            print('count:', count)
            print('ecmp-count:', ecmp_count)
            for i in range(0, 4):
                self.assertTrue((ecmp_count[i] >= ((max_itrs / 5) * 0.7)),
                                "Ecmp paths are not equally balanced")
        finally:
            pass

