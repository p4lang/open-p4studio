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
Thrift API interface basic tests for DTEL specific features.
Currently includes MOD
"""
import bf_switcht_api_thrift

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

from ptf import config
from ptf.testutils import *
from ptf.thriftutils import *

import os
import ptf.mask as mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *
this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from common.dtel_utils import *
from common.pktpy_utils import pktpy_skip # noqa pylint: disable=wrong-import-position

device = 0
swports = list(range(4))
switch_id = 0x11111111
SWITCH_DTEL_REPORT_TYPE_FLOW  = 1
SWITCH_DTEL_REPORT_TYPE_QUEUE = 2
SWITCH_DTEL_REPORT_TYPE_DROP  = 4

params = SwitchConfig_Params()
params.switch_id = switch_id
params.mac_self = '00:77:66:55:44:33'
params.nports = 3
params.ipaddr_inf = ['172.16.0.1',  '172.20.0.1',  '172.30.0.1']
params.ipaddr_nbr = ['172.16.0.11', '172.20.0.12', '172.30.0.13']
params.mac_nbr = ['00:11:22:33:44:55', '00:11:22:33:44:56', '00:11:22:33:44:57']
params.report_ports = [1]
params.ipaddr_report_src = ['4.4.4.1']
params.ipaddr_report_dst = ['4.4.4.3']
params.device = device
params.swports = swports

acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
acl_table_bp_lag = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_LAG)
acl_table_bp_vlan = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_VLAN)
acl_table_bp_rif = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_RIF)
acl_table_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_SWITCH)
acl_group_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_PORT)
acl_group_bp_vlan = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_VLAN)
acl_group_bp_rif = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_RIF)
acl_group_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_SWITCH)


def exp_mod_packet(packet, int_v2, ingress_port, hw_id, drop_reason, f=0):

    if int_v2:
        exp_mod_pkt = ipv4_dtel_v2_pkt(
            eth_dst=params.mac_nbr[params.report_ports[0]],
            eth_src=params.mac_self,
            ip_src=params.ipaddr_report_src[0],
            ip_dst=params.ipaddr_report_dst[0],
            ip_id=0,
            ip_ttl=63,
            dropped=1,
            congested_queue=0,
            path_tracking_flow=f,
            hw_id=hw_id,
            switch_id=switch_id,
            ingress_port=ingress_port,
            egress_port=INVALID_PORT_ID,
            ingress_tstamp=0,
            queue_id=0,
            drop_reason=drop_reason,
            inner_frame=packet)

    else:
        exp_mod_inner = mod_report(
            packet=packet,
            switch_id=switch_id,
            ingress_port=ingress_port,
            egress_port=INVALID_PORT_ID,
            queue_id=0,
            drop_reason=drop_reason)

        exp_mod_pkt = ipv4_dtel_pkt(
            eth_dst=params.mac_nbr[params.report_ports[0]],
            eth_src=params.mac_self,
            ip_src=params.ipaddr_report_src[0],
            ip_dst=params.ipaddr_report_dst[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_MOD,
            dropped=1,
            congested_queue=0,
            path_tracking_flow=f,
            hw_id=hw_id,
            inner_frame=exp_mod_inner)

    return exp_mod_pkt


def exp_egress_mod_packet(
        packet, int_v2, ing_port, eg_port, hw_id, drop_reason, d=1, q=0, f=0):

    if int_v2:
        exp_mod_pkt = ipv4_dtel_v2_pkt(
            eth_dst=params.mac_nbr[params.report_ports[0]],
            eth_src=params.mac_self,
            ip_src=params.ipaddr_report_src[0],
            ip_dst=params.ipaddr_report_dst[0],
            ip_id=0,
            ip_ttl=63,
            dropped=d,
            congested_queue=q,
            path_tracking_flow=f,
            hw_id=hw_id,
            switch_id=switch_id,
            ingress_port=ing_port,
            egress_port=eg_port,
            ingress_tstamp=0,
            queue_id=0,
            drop_reason=drop_reason,
            inner_frame=packet)

    else:
        exp_mod_inner = mod_report(
            packet=packet,
            switch_id=switch_id,
            ingress_port=ing_port,
            egress_port=eg_port,
            queue_id=0,
            drop_reason=drop_reason)

        exp_mod_pkt = ipv4_dtel_pkt(
            eth_dst=params.mac_nbr[params.report_ports[0]],
            eth_src=params.mac_self,
            ip_src=params.ipaddr_report_src[0],
            ip_dst=params.ipaddr_report_dst[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_MOD,
            dropped=d,
            congested_queue=q,
            path_tracking_flow=f,
            hw_id=hw_id,
            inner_frame=exp_mod_inner)

    return exp_mod_pkt


def exp_dod_packet(api, packet, ing_port, eg_port, hw_id, d=1, q=0, f=0):
    SWITCH_DROP_REASON_TRAFFIC_MANAGER = 71

    if api.int_v2:
        exp_dod_pkt = ipv4_dtel_v2_pkt(
            eth_dst=params.mac_nbr[params.report_ports[0]],
            eth_src=params.mac_self,
            ip_src=params.ipaddr_report_src[0],
            ip_dst=params.ipaddr_report_dst[0],
            ip_id=0,
            ip_ttl=63,
            dropped=d,
            congested_queue=q,
            path_tracking_flow=f,
            hw_id=hw_id,
            switch_id=switch_id,
            ingress_port=ing_port,
            egress_port=eg_port,
            ingress_tstamp=0,
            queue_id=0,
            drop_reason=SWITCH_DROP_REASON_TRAFFIC_MANAGER,
            inner_frame=packet)
    else:
        exp_dod_inner = mod_report(
            packet=packet,
            switch_id=switch_id,
            ingress_port=ing_port,
            egress_port=eg_port,
            queue_id=0,
            drop_reason=SWITCH_DROP_REASON_TRAFFIC_MANAGER)

        exp_dod_pkt = ipv4_dtel_pkt(
            eth_dst=params.mac_nbr[params.report_ports[0]],
            eth_src=params.mac_self,
            ip_src=params.ipaddr_report_src[0],
            ip_dst=params.ipaddr_report_dst[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_MOD,
            dropped=d,
            congested_queue=q,
            path_tracking_flow=f,
            hw_id=hw_id,
            inner_frame=exp_dod_inner)

    return exp_dod_pkt

def exp_postcard_packet(packet, int_v2, ing_port, eg_port, hw_id, d=0, q=0, f=0):
    if int_v2:
        exp_e2e_pkt = ipv4_dtel_v2_pkt(
            eth_dst=params.mac_nbr[params.report_ports[0]],
            eth_src=params.mac_self,
            ip_src=params.ipaddr_report_src[0],
            ip_dst=params.ipaddr_report_dst[0],
            ip_id=0,
            ip_ttl=63,
            dropped=d,
            congested_queue=q,
            path_tracking_flow=f,
            hw_id=hw_id,
            switch_id=switch_id,
            ingress_port=ing_port,
            egress_port=eg_port,
            queue_id=0,
            queue_depth=0,
            ingress_tstamp=0,
            egress_tstamp=0,
            inner_frame=packet)

    else:

        exp_pc_inner = postcard_report(
            packet=packet,
            switch_id=switch_id,
            ingress_port=ing_port,
            egress_port=eg_port,
            queue_id=0,
            queue_depth=0,
            egress_tstamp=0)

        exp_e2e_pkt = ipv4_dtel_pkt(
            eth_dst=params.mac_nbr[params.report_ports[0]],
            eth_src=params.mac_self,
            ip_src=params.ipaddr_report_src[0],
            ip_dst=params.ipaddr_report_dst[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL,
            dropped=d,
            congested_queue=q,
            path_tracking_flow=f,
            hw_id=hw_id,
            inner_frame=exp_pc_inner)

    return exp_e2e_pkt

def configure_dtel(api, d=False, q=False, f=False, tail=False):
    api.configure()
    api.attribute_set(api.device, SWITCH_DEVICE_ATTR_SWITCH_ID, switch_id)

    # enable d/q/f sessions
    api.dtel = api.add_dtel(api.device, drop_report=d, queue_report=q, flow_report=f, tail_drop_report=tail)

    # add report session information
    report_dst_ip_list = []
    for ip in params.ipaddr_report_dst:
        report_dst_ip_list.append(switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr=ip))
    api.report_session = api.add_report_session(
        api.device, udp_dst_port=UDP_PORT_DTEL_REPORT,
        src_ip=params.ipaddr_report_src[0], vrf_handle=api.vrf10,
        dst_ip_list=report_dst_ip_list, truncate_size=512, ttl=64)

    #workaround for model bug that reports high latency values
    if test_param_get('target') == "asic-model":
        api.attribute_set(
            api.device, SWITCH_DEVICE_ATTR_LATENCY_SENSITIVITY, 30)

    api.fp = []
    mult = 4
    if api.arch == 'tofino2':
        mult = 8
    # configure ingress l3 port
    api.rif0 = api.add_rif(api.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
        port_handle=api.port0, vrf_handle=api.vrf10, src_mac=api.rmac)
    # configure egress l3 port
    rif2 = api.add_rif(api.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
        port_handle=api.port2, vrf_handle=api.vrf10, src_mac=api.rmac)
    nhop2 = api.add_nexthop(api.device, handle=rif2, dest_ip='11.11.11.2')
    neighbor2 = api.add_neighbor(api.device, mac_address='00:11:22:33:44:22', handle=rif2, dest_ip='11.11.11.2')
    route2 = api.add_route(api.device, ip_prefix='11.11.11.0/24', vrf_handle=api.vrf10, nexthop_handle=nhop2)
    route3 = api.add_route(api.device, ip_prefix='1234:5678:9abc:def0:4422:1133:0:0/96', vrf_handle=api.vrf10, nexthop_handle=nhop2)
    for port in api.port_list:
        fport = api.attribute_get(port, SWITCH_PORT_ATTR_CONNECTOR_ID) * mult
        fport = fport + api.attribute_get(port, SWITCH_PORT_ATTR_CHANNEL_ID)
        api.fp.append(fport)

    # configure l2 ports
    vlan_mbr0 = api.add_vlan_member(api.device, vlan_handle=api.vlan10, member_handle=api.port3)
    vlan_mbr1 = api.add_vlan_member(api.device, vlan_handle=api.vlan10, member_handle=api.port4)
    api.attribute_set(api.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
    api.attribute_set(api.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

    # configure collector port
    rif1 = api.add_rif(api.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
        port_handle=api.port1, vrf_handle=api.vrf10, src_mac=api.rmac)
    nhop1 = api.add_nexthop(api.device, handle=rif1, dest_ip='10.10.10.1')
    neighbor1 = api.add_neighbor(api.device, mac_address='00:11:22:33:44:56', handle=rif1, dest_ip='10.10.10.1')
    route1 = api.add_route(api.device, ip_prefix='4.4.4.0/24', vrf_handle=api.vrf10, nexthop_handle=nhop1)

    if (api.client.is_feature_enable(SWITCH_FEATURE_INT_V2) == 1):
        api.int_v2 = True;
    else:
        api.int_v2 = False;


###############################################################################
@group('dtel')
class MoDNoDropTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        # testing by having global table space
        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return

        print("Test not sending a drop report when no packet is dropped")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[2])
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            self.cleanup()
            return
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.cleanup()

###############################################################################
@group('dtel')
class MoDHostifReasonCodeDropTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=1,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device,
            queue_handle=queue_handles[0].oid, admin_state=True)
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_PIM,
            hostif_trap_group_handle=self.hostif_trap_group0,
            packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return

        print("Test drop report due to hostif reason code with drop action")

        try:
            print("Sending packets from port %d" % (self.devports[0]))
            pim_pkt = simple_ip_packet(
                ip_proto=103,                # PIM
                eth_dst=params.mac_self,
                eth_src=params.mac_nbr[0],
                ip_dst=params.ipaddr_nbr[1],
                ip_src=params.ipaddr_nbr[0],
                ip_id=105,
                ip_ttl=64,
                pktlen=256)
            send_packet(self, self.devports[0], pim_pkt)
            verify_dtel_packet(self, exp_mod_packet(pim_pkt, self.int_v2, self.fp[0], 4, SWITCH_HOSTIF_TRAP_ATTR_TYPE_PIM), self.devports[1])
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            self.cleanup()
            return
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.cleanup()

###############################################################################


@group('dtel')
class MoDIngressACLDropTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()

        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='4.4.4.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=dtel_acl_table,
            acl_group_handle=acl_group)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table,
            acl_group_handle=acl_group)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return

        print("Test drop report due to ipv4 acl deny action")

        try:
            print("Set DTEL report truncate size to 512")
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_TRUNCATE_SIZE,
                512)

            print("Sending packet from port %d" % (self.devports[0]))
            pktlen=256
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='4.4.4.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64,
                pktlen=pktlen)
            send_packet(self, self.devports[0], pkt)
            # 80 is SWITCH_DROP_REASON_ACL_DROP
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])

            print("Change DTEL report truncate size to 128")
            truncate_size = 128
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_TRUNCATE_SIZE,
                truncate_size)

            if (self.arch == 'tofino'):
                if self.int_v2:
                    # 8 is due to mirror metadata size diff from sw local
                    drop_truncate_adjust = 8
                else:
                    # 6 is due to mirror metadata size diff from sw local
                    drop_truncate_adjust = 6
            else:
                if self.int_v2:
                    # 8 is due to telem report header size diff from sw local
                    drop_truncate_adjust = 8
                else:
                    # 4 is due to telem report header size diff from sw local
                    drop_truncate_adjust = 4
            truncated_amount = pktlen - (truncate_size + drop_truncate_adjust)

            exp_mod_pkt_full = exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 80)
            exp_mod_pkt = truncate_packet(exp_mod_pkt_full, truncated_amount)
            exp_mod_pkt["IP"].len -= truncated_amount
            exp_mod_pkt["UDP"].len -= truncated_amount
            if self.int_v2:
                exp_mod_pkt[DTEL_REPORT_V2_HDR].report_length -= (
                    truncated_amount + 3) // 4
            send_packet(self, self.devports[0], pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.devports[1])
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            self.cleanup()
            return
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.cleanup()

###############################################################################
@group('dtel')
class MoDEgressACLDropTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()

        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=10,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.0',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("Drop report or egress IP ACL feature not enabled, skipping")
            return

        print("Test drop report due to egress ipv4 acl deny action")

        try:
            print("Sending packets from port %d" % (self.devports[0]))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:22',
                eth_src='00:77:66:55:44:33',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            send_packet(self, self.devports[0], pkt)
            # 92 is SWITCH_DROP_REASON_EGRESS_ACL_DROP
            verify_dtel_packet(self, exp_egress_mod_packet(exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92), self.devports[1])
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            self.cleanup()
            return
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
        self.cleanup()

###############################################################################

# Please remember to restart the model if this test is run
@disabled
class DropSuppressionTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return

        print("Test drop report due to ipv4 acl deny action")
        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)

        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            table_handle=dtel_acl_table)
        try:
            print("Sending packets from port %d" % (self.devports[0]))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            print("Drop report should be generated")
            send_packet(self, self.devports[0], pkt)
            # 80 is SWITCH_DROP_REASON_ACL_DROP
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("No drop report should be generated")
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            print("Updated DTEL watchlist with action_report_all_packets")
            self.cleanlast()
            dtel_acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=dtel_acl_table)
            print("Drop report should be generated")
            send_packet(self, self.devports[0], pkt)
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.cleanup()

###############################################################################


# Please remember to restart the model if this test is run
class EgressDropSuppressionTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return

        print("Test drop report due to egress ipv4 acl deny action")
        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.attribute_set(
            self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            table_handle=dtel_acl_table)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        try:
            print("Sending packets from port %d" % (self.devports[0]))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                tcp_dport=8888,
                ip_id=105,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:22',
                eth_src='00:77:66:55:44:33',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                tcp_dport=8888,
                ip_id=105,
                ip_ttl=63)

            print("Drop report should be generated")
            send_packet(self, self.devports[0], pkt)
            # 92 is SWITCH_DROP_REASON_EGRESS_ACL_DROP
            verify_dtel_packet(self, exp_egress_mod_packet(exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92, q=0), self.devports[1])
            print("No drop report should be generated")
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            print("Updated DTEL watchlist with action_report_all_packets")
            self.cleanlast()
            dtel_acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=dtel_acl_table)
            print("Drop report should be generated")
            send_packet(self, self.devports[0], pkt)
            verify_dtel_packet(self, exp_egress_mod_packet(exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92, q=0), self.devports[1])
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 0)
            self.cleanup()

###############################################################################
@group('dtel')
class DtelWatchListUnitTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        self.dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_table2 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.1',
            dst_ip_mask='255.255.255.255',
            src_ip='20.20.20.1',
            src_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        acl_entry2 = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.255',
            src_ip='20.20.20.2',
            src_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        acl_entry3 = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            src_ip='2000::1',
            src_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table2)
        acl_entry4 = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99ab',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            src_ip='2000::2',
            src_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table2)
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table,
            acl_group_handle=acl_group)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table2,
            acl_group_handle=acl_group)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=self.dtel_acl_table,
            acl_group_handle=acl_group)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group)

        # send the test packet(s)
        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='20.20.20.1',
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=64)
        self.allow_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='20.20.20.2',
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=32)

        self.pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=64)
        self.allow_pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99ab',
            ipv6_src='2000::2',
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=33)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return
        print("Test drop report due to ipv4 acl deny action")
        try:
            self.IPv4AclTableFieldTest()
            self.IPv6AclTableFieldTest()
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            self.cleanup()
            return
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.cleanup()

    def IPv4AclTableFieldTest(self):
        print("IPv4AclTableFieldTest()")
        '''
        #define INGRESS_IP_ACL_KEY
        lkp.ip_src_addr : ternary;
        lkp.ip_dst_addr : ternary;
        lkp.ip_proto : ternary;
        lkp.ip_tos : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        lkp.ip_ttl : ternary;
        lkp.ip_frag : ternary;
        lkp.tcp_flags : ternary;
        lkp.mac_type : ternary;
        '''
        try:
            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on dst_ip
            print("Add ACL entry to match and report on dst_ip field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dst_ip='11.11.11.1',
                dst_ip_mask='255.255.255.255',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on src_ip
            print("Add ACL entry to match and report on src_ip field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                src_ip='20.20.20.1',
                src_ip_mask='255.255.255.255',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on ip_proto
            print("Add ACL entry to match and report on ip_proto field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                ip_proto=6,
                ip_proto_mask=127,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Report the 2nd packet also since IP_PROTO=6 matches")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_dtel_packet(self, exp_mod_packet(self.allow_pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on ip_tos
            print("Add ACL entry to match and report on ip_tos field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                ip_dscp=2,
                ip_dscp_mask=127,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on tcp sport
            print("Add ACL entry to match and report on l4 src port field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                l4_src_port=3333,
                l4_src_port_mask=32759,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on tcp dport
            print("Add ACL entry to match and report on l4 dst port field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                l4_dst_port=5555,
                l4_dst_port_mask=32759,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on ip ttl
            print("Add ACL entry to match and report on ip ttl field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                ttl=64,
                ttl_mask=127,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on tcp flags
            print("Add ACL entry to match and report on tcp flags field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                tcp_flags=4,
                tcp_flags_mask=127,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_no_other_packets(self, timeout=1)
            # match on ethertype
            print("Add ACL entry to match and report on ethertype field")
            acl_entry = self.add_acl_entry(self.device,
                eth_type=0x0800,
                eth_type_mask=0x7FFF,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Report the 2nd packet also since eth type 0x800 matches")
            send_packet(self, self.devports[0], self.allow_pkt)
            verify_dtel_packet(self, exp_mod_packet(self.allow_pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            self.cleanlast()
        finally:
            pass

    def IPv6AclTableFieldTest(self):
        print("IPv6AclTableFieldTest()")

        try:
            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt_v6)
            verify_no_other_packets(self, timeout=1)
            # match on dst_ip
            print("Add ACL entry to match and report on dst_ip field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_dtel_packet(self, exp_mod_packet(self.pkt_v6, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt_v6)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], self.allow_pkt_v6)
            verify_no_other_packets(self, timeout=1)
            # match on src_ip
            print("Add ACL entry to match and report on src_ip field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                src_ip='2000::1',
                src_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_dtel_packet(self, exp_mod_packet(self.pkt_v6, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.allow_pkt_v6)
            verify_no_other_packets(self, timeout=1)
            self.cleanlast()
        finally:
            pass

###############################################################################
@group('dtel')
class MoDL2MalformedPacketsTest(ApiHelper):
    '''
    This test tries to verify packets using validate_ethernet table
    We send a packet from port 0 and forward/drop depending on packet contents
    '''

    def setUp(self):
        super(self.__class__, self).setUp()

        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            dst_ip='10.10.10.0',
            dst_ip_mask='255.255.255.0',
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table,
            priority=10)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:12', destination_handle=self.port3)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:22', destination_handle=self.port4)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:13', destination_handle=self.port3)

        self.lag0 = self.add_lag(self.device)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:32', destination_handle=self.lag0)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return
        print('Configuring devices for L2 malformed packet test cases')
        print('Note: Running this test twice without restarting will fail due')
        print(' to inability to disable suppression for L2 malformed packets')

        try:
            initial_stats = self.client.object_counters_get(self.port3)
            num_drops = 0

            print("Valid packet from port %d to %d" % (self.devports[3], self.devports[4]))
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            verify_packets(self, pkt, [self.devports[4]])

            print("Valid packet from lag 0 to port %d" % self.devports[4])
            tag_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:32',
                ip_dst='10.10.10.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64)
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:32',
                ip_dst='10.10.10.1',
                ip_ttl=64,
                pktlen=96)
            send_packet(self, self.devports[5], tag_pkt)
            verify_packets(self, pkt, [self.devports[4]])

            print("Same if check fail, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:13',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_SAME_IFINDEX 58
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 58), self.devports[1])
            num_drops += 1

            print("MAC DA zeros, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:00:00:00:00:00',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_OUTER_DST_MAC_ZERO 12
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 12), self.devports[1])
            num_drops += 1

            print("MAC SA zeros, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:00:00:00:00:00',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_OUTER_SRC_MAC_ZERO 10
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 10), self.devports[1])
            num_drops += 1

            print("MAC SA broadcast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='ff:ff:ff:ff:ff:ff',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_OUTER_SRC_MAC_MULTICAST 11
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 11), self.devports[1])
            num_drops += 1

            print("MAC SA multicast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='01:00:5e:00:00:01',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_OUTER_SRC_MAC_MULTICAST 11
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 11), self.devports[1])
            num_drops += 1

            if (self.client.is_feature_enable(SWITCH_FEATURE_SAME_MAC_CHECK) == 0):
                print("Same MAC Check feature not enabled, skipping")
            else:
                print("MAC_SA==MAC_DA, drop")
                pkt = simple_tcp_packet(
                    eth_dst='00:01:00:00:00:22',
                    eth_src='00:01:00:00:00:22',
                    ip_dst='10.10.10.1',
                    ip_id=108,
                    ip_ttl=64)
                send_packet(self, self.devports[3], pkt)
                # SWITCH_DROP_REASON_OUTER_SAME_MAC_CHECK 17
                verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 17), self.devports[1])
                num_drops += 1

            print("Port vlan mapping miss, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_PORT_VLAN_MAPPING_MISS 55
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 55), self.devports[1])
            num_drops += 1

            print("Port vlan mapping miss, lag, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[5], pkt)
            # SWITCH_DROP_REASON_PORT_VLAN_MAPPING_MISS 55
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[5], 4, 55), self.devports[1])
            send_packet(self, self.devports[6], pkt)
            # SWITCH_DROP_REASON_PORT_VLAN_MAPPING_MISS 55
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[6], 4, 55), self.devports[1])

            print("L2 unicast miss, drop")
            self.attribute_set(
                self.device,
                SWITCH_DEVICE_ATTR_UNICAST_MISS_PACKET_ACTION,
                SWITCH_MAC_ENTRY_ATTR_ACTION_DROP)
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:44',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_L2_MISS_UNICAST 89
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 89), self.devports[1])
            num_drops += 1

            print("L2 multicast miss, drop")
            pkt = simple_tcp_packet(
                eth_dst='11:11:11:22:22:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            exp_mcast_pkt = pkt.copy()
            print("Multicast hit sending l2 packet from {} -> {}".format(self.devports[3], self.devports[4]))
            send_packet(self, self.devports[3], pkt)
            verify_packets(self, exp_mcast_pkt, [self.devports[4]])

            self.attribute_set(
                self.device,
                SWITCH_DEVICE_ATTR_MULTICAST_MISS_PACKET_ACTION,
                SWITCH_MAC_ENTRY_ATTR_ACTION_DROP)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_L2_MISS_MULTICAST 90
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 90), self.devports[1])
            num_drops += 1

            print("L2 broadcast miss, drop")
            self.attribute_set(
                self.device,
                SWITCH_DEVICE_ATTR_BROADCAST_MISS_PACKET_ACTION,
                SWITCH_MAC_ENTRY_ATTR_ACTION_DROP)
            pkt = simple_tcp_packet(
                eth_dst='ff:ff:ff:ff:ff:ff',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.255',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_L2_MISS_BROADCAST 91
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[3], 4, 91), self.devports[1])
            num_drops += 1

            print("Sleeping for 5 sec before fetching stats")
            time.sleep(5)
            try:
                final_stats = self.client.object_counters_get(self.port3)
                drop_stats = {a.counter_id: (a.count - b.count) for a, b in zip(final_stats, initial_stats)}
                final_count = 0
                print("Drop Stats: ")
                for counter_id in drop_stats:
                    if counter_id >= SWITCH_PORT_COUNTER_ID_SC_UCAST_RED_PACKETS and \
                       counter_id <= SWITCH_PORT_COUNTER_ID_SC_BCAST_GREEN_PACKETS:
                        # SC stats has nothing to do with dtel
                        continue
                    if (counter_id == SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS):
                        print("[%d:%d]" % (counter_id, drop_stats[counter_id]))
                        print()
                        final_count += drop_stats[counter_id]
                    if (counter_id == SWITCH_PORT_COUNTER_ID_IF_IN_L2_MISS_UNICAST):
                        print("[%d:%d]" % (counter_id, drop_stats[counter_id]))
                    if (counter_id == SWITCH_PORT_COUNTER_ID_IF_IN_L2_MISS_MULTICAST):
                        print("[%d:%d]" % (counter_id, drop_stats[counter_id]))
                    if (counter_id == SWITCH_PORT_COUNTER_ID_IF_IN_L2_MISS_BROADCAST):
                        print("[%d:%d]" % (counter_id, drop_stats[counter_id]))
                    if counter_id == SWITCH_PORT_COUNTER_ID_IF_IN_FDB_AND_BLACKHOLE_DISCARDS:
                        print("[%d:%d]" % (counter_id, drop_stats[counter_id]))

                same_mac_drop = drop_stats[SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_SAME_MAC_CHECK_DISCARDS]
                same_if_index_drop = drop_stats[SWITCH_PORT_COUNTER_ID_IF_IN_SAME_IFINDEX_DISCARDS]
                print("Expected drop count: %d" % num_drops)
                print("Final drop count   : %d" % final_count)
                print("Same outer MAC drop : %d" % same_mac_drop)
                print("Same ifindex drop   : %d" % same_if_index_drop)
                self.assertEqual(num_drops, final_count)
                #if (self.client.is_feature_enable(SWITCH_FEATURE_SAME_MAC_CHECK) != 0):
                #    self.assertEqual(1, same_mac_drop)
                #else:
                #    self.assertEqual(0, same_mac_drop)
                #else:
                #    self.assertEqual(0, same_if_index_drop)
            finally:
                pass
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            self.cleanup()
            return
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
        self.attribute_set(
            self.device,
            SWITCH_DEVICE_ATTR_UNICAST_MISS_PACKET_ACTION,
            SWITCH_MAC_ENTRY_ATTR_ACTION_FORWARD)
        self.attribute_set(
            self.device,
            SWITCH_DEVICE_ATTR_MULTICAST_MISS_PACKET_ACTION,
            SWITCH_MAC_ENTRY_ATTR_ACTION_FORWARD)
        self.attribute_set(
            self.device,
            SWITCH_DEVICE_ATTR_BROADCAST_MISS_PACKET_ACTION,
            SWITCH_MAC_ENTRY_ATTR_ACTION_FORWARD)
        self.cleanup()

###############################################################################
@group('dtel')
class MoDIPv4MalformedPacketsTest(ApiHelper):
    '''
    This test tries to verify packets using validate_ipv4 table
    We send a packet from port 0 and forward/drop depending on packet contents
    '''
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return
        print('Configuring devices for ipv4 malformed packet test cases')
        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            dst_ip='11.11.11.0',
            dst_ip_mask='255.255.255.0',
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table,
            priority=10)
        dtel_acl_entry_2 = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            eth_type=-0x7734,  # 0x88cc signed
            eth_type_mask=-1,  # 0xffff signed
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table,
            priority=10)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        try:
            initial_stats = self.client.object_counters_get(self.port0)
            num_drops = 0

            print("Valid packet from port %d to %d" % (self.devports[0], self.devports[2]))
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                ip_dst='11.11.11.2',
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_src=self.rmac,
                eth_dst='00:11:22:33:44:22',
                ip_dst='11.11.11.2',
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[2])

            print("IPv4 invalid checksum, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.2',
                ip_id=108,
                ip_ttl=64)
            pkt["IP"].chksum = 0
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_OUTER_IP_INVALID_CHECKSUM 31
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 31), self.devports[1])
            num_drops += 1

            for i in [0,1,2,3,4]:
                print("IPv4 IHL {}, drop".format(i))
                pkt = simple_tcp_packet(
                    eth_dst=self.rmac,
                    eth_src='00:01:00:00:00:11',
                    ip_dst='11.11.11.2',
                    ip_id=108,
                    ip_ihl=i)
                send_packet(self, self.devports[0], pkt)
                # SWITCH_DROP_REASON_OUTER_IP_IHL_INVALID 30
                self.assertTrue(receive_packet(self, self.devports[1]))
                num_drops += 1

            print("IPv4 TTL 0, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.2',
                ip_id=108,
                ip_ttl=0)
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_OUTER_IP_TTL_ZERO 26
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 26), self.devports[1])
            num_drops += 1

            print("IPv4 invalid version, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.2',
                ip_id=108,
                ip_ttl=64)
            pkt["IP"].version = 6
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_OUTER_IP_VERSION_INVALID 25
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 25), self.devports[1])
            num_drops += 1

            print("IPv4 src is loopback, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:11:11:11:11:11',
                ip_dst='11.11.11.2',
                ip_src='127.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_OUTER_IP_SRC_LOOPBACK 28
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 28), self.devports[1])
            num_drops += 1

            print("Drop nexthop")
            nhop3 = self.add_nexthop(
                self.device,
                type=SWITCH_NEXTHOP_ATTR_TYPE_DROP)
            route0 = self.add_route(
                self.device,
                ip_prefix='11.11.11.1',
                vrf_handle=self.vrf10,
                nexthop_handle=nhop3)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_NEXTHOP 93
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 93), self.devports[1])
            num_drops += 1

            print("Non-IP (LLDP) packet to router MAC")
            pkt = simple_eth_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:01:00:00:00:11',
                pktlen=100)
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_NON_IP_ROUTER_MAC 94
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 94), self.devports[1])
            num_drops += 1

            print("Sleeping for 5 sec before fetching stats")
            time.sleep(5)
            try:
                final_stats = self.client.object_counters_get(self.port0)
                drop_stats = {a.counter_id: (a.count - b.count) for a, b in zip(final_stats, initial_stats)}

                final_count = 0
                final_discards_count = 0
                final_out_discards_count = 0
                print("Drop Stats: ")
                for counter_id in drop_stats:
                    # Ignore counters that are not discards,
                    # and discards that dtel does not track
                    if counter_id < SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS or \
                       counter_id > SWITCH_PORT_COUNTER_ID_WRED_RED_DROPPED_PACKETS or \
                       counter_id == SWITCH_PORT_COUNTER_ID_IN_CURR_OCCUPANCY_BYTES or \
                       counter_id == SWITCH_PORT_COUNTER_ID_OUT_CURR_OCCUPANCY_BYTES:
                        continue
                    # we must ignore SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS counter as each drop
                    # is counted individually. Otherwise they are counted twice.
                    if counter_id == SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS:
                        final_discards_count = drop_stats[SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS]
                        continue
                    if counter_id == SWITCH_PORT_COUNTER_ID_IF_OUT_DISCARDS:
                        final_out_discards_count = drop_stats[SWITCH_PORT_COUNTER_ID_IF_OUT_DISCARDS]
                        continue
                    if (drop_stats[counter_id] != 0):
                        print("[%d:%d]" % (counter_id, drop_stats[counter_id]))
                        print()
                        final_count += drop_stats[counter_id]
                print("Expected drop count: %d" % num_drops)
                print("Final drop count   : %d" % final_count)
                print("Total in discards drop count : %d" % final_discards_count)
                print("Total out discards drop count : %d" % final_out_discards_count)
                self.assertEqual(num_drops, final_count)
                # total discards drops must be the same
                self.assertEqual(final_discards_count, final_count)
            finally:
                pass
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()

###############################################################################
@group('dtel')
class MoDIPv6MalformedPacketsTest(ApiHelper):
    '''
    This test tries to verify packets using validate_ipv6 table
    We send a packet from port 0 and forward/drop depending on packet contents
    '''
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return
        print('Configuring devices for ipv6 malformed packet test cases')
        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        try:
            initial_stats = self.client.object_counters_get(self.port0)
            num_drops = 0

            print("Valid packet from port %d to %d" % (self.devports[0], self.devports[2]))
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:22',
                eth_src=self.rmac,
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_hlim=63)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])

            print("MAC SA IPv6 multicast, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='33:33:5e:00:00:01',
                ipv6_dst='2000::1',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_OUTER_SRC_MAC_MULTICAST 11
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 11), self.devports[1])
            num_drops += 1

            print("IPv6 TTL 0, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:11',
                ipv6_hlim=0)
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_OUTER_IP_TTL_ZERO 26
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 26), self.devports[1])
            num_drops += 1

            print("IPv6 invalid version, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:11')
            pkt["IPv6"].version = 4
            send_packet(self, self.devports[0], pkt)
            # SWITCH_DROP_REASON_OUTER_IP_VERSION_INVALID 25
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 25), self.devports[1])
            num_drops += 1

            print("Sleeping for 5 sec before fetching stats")
            time.sleep(5)
            try:
                final_stats = self.client.object_counters_get(self.port0)
                drop_stats = {a.counter_id: (a.count - b.count) for a, b in zip(final_stats, initial_stats)}

                final_count = 0
                final_discards_count = 0
                print("Drop Stats: ")
                for counter_id in drop_stats:
                    # Ignore counters that are not discards,
                    # and discards that dtel does not track
                    if counter_id < SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS or \
                       counter_id > SWITCH_PORT_COUNTER_ID_WRED_RED_DROPPED_PACKETS or \
                       counter_id == SWITCH_PORT_COUNTER_ID_IN_CURR_OCCUPANCY_BYTES or \
                       counter_id == SWITCH_PORT_COUNTER_ID_OUT_CURR_OCCUPANCY_BYTES or \
                       counter_id == SWITCH_PORT_COUNTER_ID_IF_OUT_DISCARDS:
                        continue
                    # we must ignore SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS counter as each drop
                    # is counted individually. Otherwise they are counted twice.
                    if counter_id == SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS:
                        final_discards_count = drop_stats[SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS]
                        continue
                    if (drop_stats[counter_id] != 0):
                        print("[%d:%d]" % (counter_id, drop_stats[counter_id]))
                        print()
                        final_count += drop_stats[counter_id]
                print("Expected drop count: %d" % num_drops)
                print("Final drop count   : %d" % final_count)
                print("Total Discards drop count : %d" % final_discards_count)
                self.assertEqual(num_drops, final_count)
                # total discards drops must be the same
                self.assertEqual(final_discards_count, final_count)
            finally:
                pass
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()

###############################################################################
@group('dtel')
class MoDandIngressPortMirrorTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_PORT_MIRROR) == 0
            or self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report or ingress port mirror not enabled")
            print("skipping")
            return

        print("Test combination of drop reports and ingress port mirror")
        configure_dtel(self, d=True)

        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW | SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        # Configure ingress mirror session but do not yet attach to a port
        self.mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port1)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.pkt["IP"].chksum = 0

        try:
            self.DropReportEnabledIngressMirrorDisabledTest()
            self.DropReportEnabledIngressMirrorEnabledTest()
            self.DropReportDisabledIngressMirrorEnabledTest()
        finally:
            split_mirror_on_drop_pkt(self.int_v2)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.cleanup()

    def DropReportEnabledIngressMirrorDisabledTest(self):
        try:
            print("Drop report enabled, ingress mirror disabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            # SWITCH_DROP_REASON_OUTER_IP_INVALID_CHECKSUM 31
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 31), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

    def DropReportEnabledIngressMirrorEnabledTest(self):
        try:
            print("Drop report enabled, ingress mirror enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.mirror)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.pkt, [self.devports[1]])
        finally:
            pass

    def DropReportDisabledIngressMirrorEnabledTest(self):
        try:
            print("Drop report disabled, ingress mirror enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, False)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.mirror)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.pkt, [self.devports[1]])
        finally:
            pass

###############################################################################

'''
The below test is only intended to run on the model.
'''


@disabled
class MoDDoDTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return
        if test_param_get('target') != "asic-model":
            print("DoD PTF only works with asic-model & --dod-test-mode, skipping")
            return

        print("Test drop report due to DOD.")
        configure_dtel(self, d=True, tail=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        try:
            print("Sending packets from port %d" % (self.devports[0]))
            pktlen = 256
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64,
                pktlen=pktlen)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:22',
                eth_src='00:77:66:55:44:33',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63,
                pktlen=pktlen)

            DOD_REPORT_PORT = self.devports[1]
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                pkt, exp_pkt, True,
                exp_dod_packet(self, pkt, self.fp[0], self.fp[2], 0))
            print("Drop report recv'd with SWITCH_DROP_REASON_TRAFFIC_MANAGER")

            print("Sending packets with ttl=1, redirected to CPU")
            pkt["IP"].ttl = 1
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR,
                ingress_bd=0x0,
                inner_pkt=pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            dtel_checkDoD(
                self, self.devports[0], self.cpu_port, DOD_REPORT_PORT,
                pkt, cpu_pkt, False)
            print("No drop of ttl=1 packet redirected to CPU since dod is not set")

            pkt["IP"].ttl = 64
            print("Change DTEL report truncate size to 128")
            truncate_size = 128
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_TRUNCATE_SIZE,
                128)

            if (self.arch == 'tofino'):
                drop_truncate_adjust = 0
            else:
                if self.int_v2:
                # 8 is due to telem report header size diff from sw local
                    drop_truncate_adjust = 8
                else:
                    # 4 is due to telem report header size diff from sw local
                    drop_truncate_adjust = 4
            truncated_amount = pktlen - (truncate_size + drop_truncate_adjust)

            exp_dod_pkt_full = exp_dod_packet(
                self, pkt, self.fp[0], self.fp[2], 4)
            exp_dod_pkt = truncate_packet(exp_dod_pkt_full, truncated_amount)
            exp_dod_pkt["IP"].len -= truncated_amount
            exp_dod_pkt["UDP"].len -= truncated_amount
            if self.int_v2:
                exp_dod_pkt[DTEL_REPORT_V2_HDR].report_length -= (
                    truncated_amount + 3) // 4
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                pkt, exp_pkt, True, exp_dod_pkt)
            print("Drop report recv'd containing packet truncated to 128")
        finally:
            time.sleep(2)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
            self.cleanup()

###############################################################################

'''
The below test is only intended to run on the model.
'''


# NOTE(sborkows): This test is skipped due to running it explicitly with flag
#  --dod-test-mode
@disabled
class MoDandFlowDoDTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Drop or flow report feature not enabled, skipping")
            return
        if test_param_get('target') != "asic-model":
            print("DoD PTF only works with asic-model & --dod-test-mode, skipping")
            return

        print("Test drop report due to DOD, setting F bit as well as D bit.")
        configure_dtel(self, d=False, f=False, tail=True)

        bind_postcard_pkt(self.int_v2)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW | SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            self.FlowReportEnabledDropReportEnabledTest()
            self.FlowReportDisabledDropReportEnabledTest()
        finally:
            split_mirror_on_drop_pkt(self.int_v2)
            split_postcard_pkt(self.int_v2)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.cleanup()

    def FlowReportEnabledDropReportEnabledTest(self):
        try:
            print("Flow report enabled, drop report enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            DOD_REPORT_PORT = self.devports[1]
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                self.pkt, self.exp_pkt, True,
                exp_dod_packet(
                    self, self.pkt, self.fp[0], self.fp[2], 0, f=1),
                exp_e2e_pkt=exp_postcard_packet(
                    self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0, f=1),
                exp_report_port=self.devports[1])
            print("Drop report is generated with F bit set to 1")
        finally:
            pass

    def FlowReportDisabledDropReportEnabledTest(self):
        try:
            print("Flow report disabled, drop report enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, False)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            DOD_REPORT_PORT = self.devports[1]
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                self.pkt, self.exp_pkt, True,
                exp_dod_packet(self, self.pkt, self.fp[0], self.fp[2], 0, f=0))
            print("Drop report is generated with F bit set to 0")
        finally:
            pass

###############################################################################
@group('dtel')
class QueueReportTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_QUEUE_REPORT) == 0):
            print("Queue report feature not enabled, skipping")
            return

        print("Test queue report")
        configure_dtel(self, q=False)
        bind_postcard_pkt(self.int_v2)

        queue_handles = self.attribute_get(self.port2, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.qr_id = self.add_queue_report(self.device, queue_handle=queue_handles[0].oid,
            latency_threshold=500000000, depth_threshold=1000, breach_quota=10)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            self.NoQueueReportTest()
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_QUEUE_REPORT, True)
            self.LowLatencyNoReportTest()
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_LATENCY_THRESHOLD, 10)
            print("Latency threshold decreased to 10 nanoseconds")
            self.ValidQueueReportTest()
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_LATENCY_THRESHOLD, 500000000)
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_DEPTH_THRESHOLD, 0)
            print("Depth threshold decreased to 0")
            self.ValidQueueReportTest()
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 0)
            print("Queue report enabled, but quota is 0")
            self.NoQuotaNoReportTest()
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 2)
            print("Quota reset to 2")
            self.QueueReportChangeTest()
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 10)
            print("Quota reset to 10")
            self.EgressDropQueueReportTest()
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def NoQueueReportTest(self):
        try:
            print("No report is generated since queue reports are disabled")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            pass

    def NoQuotaNoReportTest(self):
        try:
            #Workaround for first packet after setting register having
            #different quantized latency and so triggering a queue report
            print("Queue report is generated first time after resetting quota")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1), self.devports[1])
            print("No report is generated since breach quota is zero")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
            print("No report is generated since breach quota is zero")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            pass

    def LowLatencyNoReportTest(self):
        try:
            print("No report is generated since latency is below configured value")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            pass

    def ValidQueueReportTest(self):
        try:
            print("Queue report is now generated")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1), self.devports[1])
        finally:
            pass

    def QueueReportChangeTest(self):
        try:
            print("Change latency sensitivity to 0")
            self.attribute_set(
                self.device, SWITCH_DEVICE_ATTR_LATENCY_SENSITIVITY, 0)
            print("Queue report is generated when quota is above 0")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1), self.devports[1])
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1), self.devports[1])
            print("Queue reports due to latency change when quota = 0")
            num_packets = 20
            num_postcards_rcvd = 0
            for i in range(num_packets):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[2])
                postcard_pkt = receive_postcard_packet(
                    self,
                    exp_postcard_packet(
                        self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1),
                    self.devports[1])
                if postcard_pkt is not None:
                    num_postcards_rcvd += 1
            self.assertTrue(num_postcards_rcvd >= num_packets * 0.30,
                "Not enough postcards received due to latency change:"
                " %d" % num_postcards_rcvd)
        finally:
            print("Reset latency sensitivity to a higher value")
            #workaround for model bug that reports high latency values
            if test_param_get('target') == "asic-model":
                self.attribute_set(
                    self.device, SWITCH_DEVICE_ATTR_LATENCY_SENSITIVITY, 30)
            else:
                self.attribute_set(
                    self.device, SWITCH_DEVICE_ATTR_LATENCY_SENSITIVITY, 18)

    def EgressDropQueueReportTest(self):
        print("Queue report generated when packet is dropped in egress pipeline")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("Drop report or egress IP ACL feature not enabled, skipping")
            return

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.0',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.attribute_set(
            self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1), self.devports[1])
            verify_no_other_packets(self)
        finally:
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)

###############################################################################
@group('dtel')
class MoDandQueueReportTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_QUEUE_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("Queue report or drop report or egress ACL feature not enabled")
            print("skipping")
            return

        print("Test combination of drop and queue reports for egress pipeline drops")
        configure_dtel(self, d=False, q=False)
        bind_postcard_pkt(self.int_v2)
        bind_mirror_on_drop_pkt(self.int_v2)

        queue_handles = self.attribute_get(self.port2, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.qr_id = self.add_queue_report(self.device, queue_handle=queue_handles[0].oid,
            latency_threshold=10, depth_threshold=0, breach_quota=10)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=10,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.0',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)
        self.attribute_set(
            self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 10)
        self.attribute_set(
            self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            self.QueueReportEnabledDropReportDisabledTest()
            self.QueueReportEnabledDropReportEnabledTest()
            self.QueueReportDisabledDropReportEnabledTest()
        finally:
            split_mirror_on_drop_pkt(self.int_v2)
            split_postcard_pkt(self.int_v2)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.cleanup()

    def QueueReportEnabledDropReportDisabledTest(self):
        try:
            print("Queue report enabled, drop report disabled, quota set to 1")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_QUEUE_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, False)
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 1)
            print("Queue report is generated when quota is 1")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1), self.devports[1])
            verify_no_other_packets(self)
            print("No report when quota is 0")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def QueueReportEnabledDropReportEnabledTest(self):
        try:
            print("Queue report enabled, drop report enabled, quota set to 1")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_QUEUE_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 1)
            print("Drop report is generated when quota is 1")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            # 92 is SWITCH_DROP_REASON_EGRESS_ACL_DROP
            verify_dtel_packet(self, exp_egress_mod_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92, q=1), self.devports[1])
            verify_no_other_packets(self)
            print("Drop report with cleared q bit when quota is 0")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_egress_mod_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92, q=0), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

    def QueueReportDisabledDropReportEnabledTest(self):
        try:
            print("Queue report disabled, drop report enabled, quota set to 1")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_QUEUE_REPORT, False)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 1)
            print("Drop report with cleared q bit is generated when quota is 1")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            # 92 is SWITCH_DROP_REASON_EGRESS_ACL_DROP
            verify_dtel_packet(self, exp_egress_mod_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92, q=0), self.devports[1])
            verify_no_other_packets(self)
            print("Drop report with cleared q bit is generated when quota is 0")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_dtel_packet(self, exp_egress_mod_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92, q=0), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

###############################################################################


@group('dtel')
class QueueReportEntropyTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_QUEUE_REPORT) == 0):
            print("Queue report feature not enabled, skipping")
            return

        print("Test UDP source port entropy in queue reports")
        configure_dtel(self, q=True)
        bind_postcard_pkt(self.int_v2)

        queue_handles = self.attribute_get(self.port2, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.qr_id = self.add_queue_report(
            self.device, queue_handle=queue_handles[0].oid,
            latency_threshold=0, depth_threshold=0, breach_quota=1024)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        self.exp_postcard_pkt = exp_postcard_packet(
            self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0, q=1)

        try:
            self.QueueReportUdpSrcPortZeroTest()
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_UDP_SRC_PORT,
                999)
            self.QueueReportUdpSrcPortNonZeroTest()
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_UDP_SRC_PORT_ENTROPY,
                True)
            self.QueueReportUdpSrcPortEntropyTest()
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def QueueReportUdpSrcPortZeroTest(self):
        try:
            print("Queue report with UDP source port = 0 is generated")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            self.exp_postcard_pkt["UDP"].sport = 0
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(
                self, self.exp_postcard_pkt, self.devports[1],
                ignore_udp_sport = False)
        finally:
            pass

    def QueueReportUdpSrcPortNonZeroTest(self):
        try:
            print("Queue report with UDP source port = 999 is generated")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            self.exp_postcard_pkt["UDP"].sport = 999
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(
                self, self.exp_postcard_pkt, self.devports[1],
                ignore_udp_sport = False)
        finally:
            pass

    def QueueReportUdpSrcPortEntropyTest(self):
        try:
            print("Queue reports with UDP source port entropy are generated")
            print("Sending packets port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")

            postcard_pkt_udp_src_port = []

            for i in range(15):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[2])

                postcard_pkt_udp = receive_postcard_packet(
                    self, self.exp_postcard_pkt, self.devports[1])
                self.assertTrue(
                    postcard_pkt_udp is not None,
                    "Did not receive pkt on %r" % self.devports[1])
                verify_no_other_packets(self, timeout=1)

                postcard_pkt_udp_src_port.append(postcard_pkt_udp["UDP"].sport)
                self.pkt["TCP"].dport += 1
                self.exp_pkt["TCP"].dport += 1
                self.exp_postcard_pkt = exp_postcard_packet(
                    self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0, q=1)

            postcard_pkt_udp_src_port_unique_values = len(set(
                postcard_pkt_udp_src_port))

            print("%d unique dtel report UDP source port values out of %d values" \
                % (postcard_pkt_udp_src_port_unique_values,
                  len(postcard_pkt_udp_src_port)))

            self.assertTrue(postcard_pkt_udp_src_port_unique_values >= (
                len(postcard_pkt_udp_src_port) * 0.80),
                "Not enough unique dtel report UDP source port values")

        finally:
            pass

###############################################################################


@group('dtel')
class QueueReportEcmpLagTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_QUEUE_REPORT) == 0):
            print("Queue report feature not enabled, skipping")
            return

        print("Test ECMP of queue reports with UDP source port entropy")
        orig_mac_nbr = params.mac_nbr
        orig_report_ports = params.report_ports
        params.mac_nbr = ['00:11:22:33:44:55', '00:11:22:33:44:56',
                          '00:11:22:33:44:57', '00:11:22:33:44:58',
                          '00:11:22:33:44:59', '00:11:22:33:44:5a',
                          '00:11:22:33:44:5b', '00:11:22:33:44:5c',
                          '00:11:22:33:44:5d']
        configure_dtel(self, q=True)
        bind_postcard_pkt(self.int_v2)

        self.attribute_set(
            self.report_session,
            SWITCH_REPORT_SESSION_ATTR_UDP_SRC_PORT_ENTROPY,
            True)

        queue_handles = self.attribute_get(self.port2,
                                           SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.qr_id = self.add_queue_report(
            self.device, queue_handle=queue_handles[0].oid,
            latency_threshold=0, depth_threshold=0, breach_quota=1024)

        # configure ecmp rifs, nhops, neighbors to reach collector
        rif5 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port5, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop5 = self.add_nexthop(self.device, handle=rif5, dest_ip='10.10.5.1')
        neighbor5 = self.add_neighbor(self.device,
            mac_address='00:11:22:33:44:5a', handle=rif5, dest_ip='10.10.5.1')

        rif6 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port6, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop6 = self.add_nexthop(self.device, handle=rif6, dest_ip='10.10.6.1')
        neighbor6 = self.add_neighbor(self.device,
            mac_address='00:11:22:33:44:5b', handle=rif6, dest_ip='10.10.6.1')

        rif7 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port7, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop7 = self.add_nexthop(self.device, handle=rif7, dest_ip='10.10.7.1')
        neighbor7 = self.add_neighbor(self.device,
            mac_address='00:11:22:33:44:5c', handle=rif7, dest_ip='10.10.7.1')

        self.ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(
            self.device, nexthop_handle=nhop5, ecmp_handle=self.ecmp0)
        ecmp_member02 = self.add_ecmp_member(
            self.device, nexthop_handle=nhop6, ecmp_handle=self.ecmp0)
        ecmp_member03 = self.add_ecmp_member(
            self.device, nexthop_handle=nhop7, ecmp_handle=self.ecmp0)

        # configure lag rif, nhop, neighbor to reach collector
        lag0 = self.add_lag(self.device)
        lag_mbr_00 = self.add_lag_member(self.device,
            lag_handle=lag0, port_handle=self.port8)
        lag_mbr_01 = self.add_lag_member(self.device,
            lag_handle=lag0, port_handle=self.port9)
        lag_mbr_02 = self.add_lag_member(self.device,
            lag_handle=lag0, port_handle=self.port10)
        rif8 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=lag0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop8 = self.add_nexthop(self.device,
            handle=rif8, dest_ip='10.10.8.1')
        neighbor8 = self.add_neighbor(self.device,
            mac_address='00:11:22:33:44:5d', handle=rif8, dest_ip='10.10.8.1')

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            tcp_dport=3000)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            tcp_dport=3000)

        try:
            self.QueueReportEcmpTest()
            self.QueueReportLagTest()
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()
            params.report_ports = orig_report_ports
            params.mac_nbr = orig_mac_nbr

    def QueueReportEcmpTest(self):
        # configure ecmp route to collector
        route0 = self.add_route(
            self.device, ip_prefix='4.4.4.3/32', vrf_handle=self.vrf10,
            nexthop_handle=self.ecmp0)

        try:
            print("Queue reports with UDP source port entropy are generated")
            print("Sending packets port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")

            max_itrs = 30
            ecmp_port_count = 3
            count = [0] * ecmp_port_count
            mirror_ports = [0] * ecmp_port_count
            params.report_ports = [5, 6, 7]
            self.exp_postcard_pkt = [None] * ecmp_port_count
            for j in range(0, ecmp_port_count):
                mirror_ports[j] = self.devports[params.report_ports[j]]
                self.exp_postcard_pkt[j] = exp_postcard_packet(
                    self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0, q=1)

            for i in range(max_itrs):
                for j in range(0, ecmp_port_count):
                    params.report_ports[0] = 5 + j
                    self.exp_postcard_pkt[j] = exp_postcard_packet(self.exp_pkt,
                        self.int_v2, self.fp[0], self.fp[2], 0, q=1)
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[2])
                (rcv_index, seq_num) = verify_any_dtel_packet_any_port(
                    self, self.exp_postcard_pkt, mirror_ports)
                print(("%d %d" % (i, rcv_index)))
                count[rcv_index] += 1

                self.pkt["TCP"].dport += 1
                self.exp_pkt["TCP"].dport += 1

            for i in range(0, ecmp_port_count):
                self.assertTrue(
                    (count[i] >= ((max_itrs / float(ecmp_port_count)) * 0.50)),
                    "Not all queue reports are equally balanced"
                    " (%d < %f (%d%%) for %d" % (count[i],
                       ((max_itrs / float(ecmp_port_count)) * 0.50), 50, i))
                print("mirror session %d count %d" % (i, count[i]))

            print("passed balancing queue reports across ecmp routes")

        finally:
            # remove ecmp route to collector
            self.cleanlast()

    def QueueReportLagTest(self):
        # configure lag route to collector
        route0 = self.add_route(
            self.device, ip_prefix='4.4.4.3/32', vrf_handle=self.vrf10,
            nexthop_handle=self.nhop8)

        try:
            print("Queue reports with UDP source port entropy are generated")
            print("Sending packets port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")

            max_itrs = 30
            lag_port_count = 3
            count = [0] * lag_port_count
            mirror_ports = [0] * lag_port_count
            params.report_ports = [8, 9, 10]
            for j in range(0, lag_port_count):
                mirror_ports[j] = self.devports[params.report_ports[j]]

            for i in range(max_itrs):
                self.exp_postcard_pkt = exp_postcard_packet(
                    self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0, q=1)
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[2])
                (rcv_index, seq_num) = verify_dtel_packet_any_port(
                    self, self.exp_postcard_pkt, mirror_ports)
                print(("%d %d" % (i, rcv_index)))
                count[rcv_index] += 1

                self.pkt["TCP"].dport += 1
                self.exp_pkt["TCP"].dport += 1

            for i in range(0, lag_port_count):
                self.assertTrue(
                    (count[i] >= ((max_itrs / float(lag_port_count)) * 0.50)),
                    "Not all queue reports are equally balanced"
                    " (%d < %f (%d%%) for %d" % (count[i],
                       ((max_itrs / float(lag_port_count)) * 0.50), 50, i))
                print("mirror session %d count %d" % (i, count[i]))

            print("passed balancing queue reports across lag")

        finally:
            # remove lag route to collector
            self.cleanlast()

###############################################################################
@group('dtel')
class QueueReportMultiMirrorTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_QUEUE_REPORT) == 0):
            print("Queue report feature not enabled, skipping")
            return

        print("Test queue reports to multiple mirror destinations")
        orig_report_ports = params.report_ports
        orig_ipaddr_report_src = params.ipaddr_report_src
        orig_ipaddr_report_dst = params.ipaddr_report_dst
        params.report_ports = [1, 1, 1, 1, 1]
        params.ipaddr_report_src = [
            '4.4.4.1', '4.4.4.1', '4.4.4.1', '4.4.4.1', '4.4.4.1']
        params.ipaddr_report_dst = []
        configure_dtel(self, q=True)
        params.ipaddr_report_dst = [
            '4.4.4.3', '4.4.4.4', '4.4.4.5', '4.4.4.6', '4.4.4.7']
        bind_postcard_pkt(self.int_v2)

        queue_handles = self.attribute_get(self.port2, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.qr_id = self.add_queue_report(
            self.device, queue_handle=queue_handles[0].oid,
            latency_threshold=0, depth_threshold=0, breach_quota=1024)
        payload = 'q report'

        try:
            max_itrs = 100
            random.seed(314159)
            for mirror_sessions_num in [5, 2, 3]:
                print("Change DTEL report destination IP address list to "
                      "first %d addresses" % mirror_sessions_num)
                dst_ip_list = []
                for i in range(0, mirror_sessions_num):
                    dst_ip_list.append(switcht_list_val_t(
                        type=switcht_value_type.IP_ADDRESS,
                        ip_addr=params.ipaddr_report_dst[i]))
                self.attribute_set(
                    self.report_session,
                    SWITCH_REPORT_SESSION_ATTR_DST_IP_LIST,
                    dst_ip_list)

                count = [0] * mirror_sessions_num
                mirror_ports = [0] * mirror_sessions_num
                exp_postcard_pkt = [None] * mirror_sessions_num
                ignore_seq_num = True
                ignore_seq_num_session_count = 0
                exp_seq_num = [0] * mirror_sessions_num
                for i in range(0, mirror_sessions_num):
                    mirror_ports[i] = self.devports[params.report_ports[i]]
                for i in range(0, max_itrs):
                    src_port = i + 10000
                    dst_port = i + 10001

                    pkt = simple_udp_packet(
                        eth_dst='00:77:66:55:44:33',
                        eth_src='00:22:22:22:22:22',
                        ip_dst='11.11.11.2',
                        ip_src='192.168.0.1',
                        ip_id=105,
                        ip_ttl=64,
                        udp_sport=src_port,
                        udp_dport=dst_port,
                        udp_payload=payload)
                    exp_pkt = simple_udp_packet(
                        eth_dst='00:11:22:33:44:22',
                        eth_src='00:77:66:55:44:33',
                        ip_dst='11.11.11.2',
                        ip_src='192.168.0.1',
                        ip_id=105,
                        ip_ttl=63,
                        udp_sport=src_port,
                        udp_dport=dst_port,
                        udp_payload=payload)

                    for j in range(0, mirror_sessions_num):
                        exp_postcard_pkt[j] = exp_postcard_packet(
                            exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0,
                            q=1)
                        exp_postcard_pkt[j]["IP"].dst = params.ipaddr_report_dst[j]
                        if self.int_v2:
                            exp_postcard_pkt[j][DTEL_REPORT_V2_HDR].sequence_number = exp_seq_num[j]
                        else:
                            exp_postcard_pkt[j][DTEL_REPORT_HDR].sequence_number = exp_seq_num[j]
                    send_packet(self, self.devports[0], pkt)
                    verify_packet(self, exp_pkt, self.devports[2])
                    (rcv_index, seq_num) = verify_any_dtel_packet_any_port(
                        self, exp_postcard_pkt, mirror_ports,
                        ignore_seq_num=ignore_seq_num)
                    print(("%d %d   %d" % (i, rcv_index, seq_num)))
                    count[rcv_index] += 1
                    if (exp_seq_num[rcv_index] == 0):
                        ignore_seq_num_session_count += 1
                    if (ignore_seq_num_session_count == mirror_sessions_num):
                        ignore_seq_num = False
                    exp_seq_num[rcv_index] = seq_num + 1

                for i in range(0, mirror_sessions_num):
                    self.assertTrue(
                        (count[i] >= ((max_itrs / float(mirror_sessions_num)) * 0.50)),
                        "Not all mirror sessions are equally balanced"
                        " (%d < %f (%d%%) for %d" % (count[i],
                           ((max_itrs / float(mirror_sessions_num)) * 0.50), 50, i))
                    print("mirror session %d count %d" % (i, count[i]))

                print("passed balancing the load among telemetry mirror sessions")

        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()
            params.report_ports = orig_report_ports
            params.ipaddr_report_src = orig_ipaddr_report_src
            params.ipaddr_report_dst = orig_ipaddr_report_dst

###############################################################################

'''
The below test is only intended to run on the model.
'''

@disabled
class QueueReportDoDTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_QUEUE_REPORT) == 0):
            print("Queue report feature not enabled, skipping")
            return
        if test_param_get('target') != "asic-model":
            print("DoD PTF only works with asic-model & --dod-test-mode, skipping")
            return

        print("Test queue reports due to tail drops")
        configure_dtel(self, q=True, tail=True)

        bind_postcard_pkt(self.int_v2)

        queue_handles = self.attribute_get(
            self.port2, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.qr_id = self.add_queue_report(
            self.device, queue_handle=queue_handles[0].oid,
            latency_threshold=500000000, depth_threshold=1000, breach_quota=10,
            tail_drop=0)

        pktlen = 256
        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=pktlen)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=pktlen)

        try:
            # First run tests when no other queue reports are generated
            # since thresholds are not reached, makes debugging easier

            DOD_REPORT_PORT = self.devports[1]
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                self.pkt, self.exp_pkt, False)
            print("No queue report when tail drop is not enabled")

            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_TAIL_DROP, 1)
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                self.pkt, self.exp_pkt, True,
                exp_dod_packet(
                    self, self.pkt, self.fp[0], self.fp[2], 0, d=0, q=1))
            print("Queue report is generated for tail dropped packet")

            print("Change DTEL report truncate size to 128")
            truncate_size = 128
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_TRUNCATE_SIZE,
                128)
            if (self.arch == 'tofino'):
                drop_truncate_adjust = 0
            else:
                if self.int_v2:
                # 8 is due to telem report header size diff from sw local
                    drop_truncate_adjust = 8
                else:
                    # 4 is due to telem report header size diff from sw local
                    drop_truncate_adjust = 4
            truncated_amount = pktlen - (truncate_size + drop_truncate_adjust)

            exp_dod_pkt_full = exp_dod_packet(
                self, self.pkt, self.fp[0], self.fp[2], 0, d=0, q=1)
            exp_dod_pkt = truncate_packet(exp_dod_pkt_full, truncated_amount)
            exp_dod_pkt["IP"].len -= truncated_amount
            exp_dod_pkt["UDP"].len -= truncated_amount
            if self.int_v2:
                exp_dod_pkt[DTEL_REPORT_V2_HDR].report_length -= (
                    truncated_amount + 3) // 4
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                self.pkt, self.exp_pkt, True, exp_dod_pkt)
            print("Truncated queue report is generated for tail dropped packet")
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_TRUNCATE_SIZE,
                512)

            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 0)
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                self.pkt, self.exp_pkt, True)
            print("No queue report is generated when quota is zero")

            # Now run tests with interleaved tail drop reports amongst
            # normal queue reports
            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 19)
            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_DEPTH_THRESHOLD, 0)
            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                self.pkt, self.exp_pkt, True,
                exp_dod_packet(
                    self, self.pkt, self.fp[0], self.fp[2], 0, d=0, q=1),
                exp_e2e_pkt=exp_postcard_packet(
                    self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0, q=1),
                exp_report_port=self.devports[1])
            print("Queue report is generated when quota is not zero")

            dtel_checkDoD(
                self, self.devports[0], self.devports[2], DOD_REPORT_PORT,
                self.pkt, self.exp_pkt, True,
                exp_e2e_pkt=exp_postcard_packet(
                    self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0, q=1),
                exp_report_port=self.devports[1])
            print("Queue report is not generated when quota is zero")

        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

###############################################################################
@group('dtel')
class FlowReportTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Flow report feature not enabled, skipping")
            return

        print("Test flow report")
        configure_dtel(self, f=False)
        bind_postcard_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.240',
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            self.NoFlowReportTest()
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            print("Flow report enabled")
            self.UnconfiguredFlowNoFlowReportTest()
            self.ValidFlowReportTest()
            self.IPFlowReportTest()
            self.FlowReportSeqNumTest()
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()

    def NoFlowReportTest(self):
        try:
            print("No report should be generated since flow reports are disabled")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            pass

    def UnconfiguredFlowNoFlowReportTest(self):
        npkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.224',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        nexp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.224',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.pkt["IP"].dst = '11.11.11.224'
        self.exp_pkt["IP"].dst = '11.11.11.224'
        try:
            print("No report should be generated since flow is not configured in dtel acl")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.224)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            self.pkt["IP"].dst = '11.11.11.2'
            self.exp_pkt["IP"].dst = '11.11.11.2'

    def ValidFlowReportTest(self):
        try:
            print("Reports for flow that matches dtel_acl")
            self.pkt["IP"].dst = '11.11.11.3'
            self.exp_pkt["IP"].dst = '11.11.11.3'
            print("Valid report should be generated for 11.11.11.3")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            print("Report should be generated for 11.11.11.3 again")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            self.pkt["IP"].dst = '11.11.11.4'
            self.exp_pkt["IP"].dst = '11.11.11.4'
            print("Valid report should be generated for 11.11.11.4")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.4)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
        finally:
            self.pkt["IP"].dst = '11.11.11.2'
            self.exp_pkt["IP"].dst = '11.11.11.2'

    def IPFlowReportTest(self):
        try:
            print("Report should be generated for IP flow that is not TCP, UDP, or ICMP")
            pkt = simple_ip_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                ip_proto=103,                # PIM
                ip_id=105,
                ip_ttl=64)
            exp_pkt = simple_ip_packet(
                eth_dst='00:11:22:33:44:22',
                eth_src='00:77:66:55:44:33',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                ip_proto=103,                # PIM
                ip_id=105,
                ip_ttl=63)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
        finally:
            pass

    def FlowReportSeqNumTest(self):
        try:
            exp_postcard_pkt = exp_postcard_packet(
                self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            postcard_pkt = receive_postcard_packet(
                self, exp_postcard_pkt, self.devports[1])
            self.assertTrue(postcard_pkt is not None,
                            "Did not receive postcard packet")
            if self.int_v2:
                current_seq_num = postcard_pkt[DTEL_REPORT_V2_HDR].sequence_number
            else:
                current_seq_num = postcard_pkt[DTEL_REPORT_HDR].sequence_number
            print("Initial sequence number = %d" % current_seq_num)
            for i in range(5):
                current_seq_num += 1
                if self.int_v2:
                    exp_postcard_pkt[DTEL_REPORT_V2_HDR].sequence_number = current_seq_num
                else:
                    exp_postcard_pkt[DTEL_REPORT_HDR].sequence_number = current_seq_num
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[2])
                verify_postcard_packet(self, exp_postcard_pkt, self.devports[1],
                                       ignore_seq_num=False)
            print("Passed sequence number increment test")
        finally:
            pass

###############################################################################
@group('dtel')
class DtelFlowWatchListTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("This DTEL testcase is not supported with ACL port_group")
            return

        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Flow report feature not enabled, skipping")
            return

        print("Test all dtel_acl fields triggering flow reports")
        configure_dtel(self, f=True)
        bind_postcard_pkt(self.int_v2)

        self.dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(
            self.port0,
            SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port0,
            SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.dtel_acl_table)

        # send the test packet(s)
        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='20.20.20.1',
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_src='20.20.20.1',
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=63)
        self.no_dtel_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='20.20.20.2',
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=32)
        self.exp_no_dtel_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='20.20.20.2',
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=31)

        self.pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=64)
        self.exp_pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=63)
        self.no_dtel_pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99ab',
            ipv6_src='2000::2',
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=33)
        self.exp_no_dtel_pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99ab',
            ipv6_src='2000::2',
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=32)

        try:
            self.IPv4FlowDtelAclTableFieldTest()
            self.IPv6FlowDtelAclTableFieldTest()
            self.FlowDtelAclPriorityTest()
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()

    def IPv4FlowDtelAclTableFieldTest(self):
        print("IPv4DtelAclTableFieldTest()")
        '''
        #define INGRESS_IP_ACL_KEY
        lkp.ip_src_addr : ternary;
        lkp.ip_dst_addr : ternary;
        lkp.ip_proto : ternary;
        lkp.ip_tos : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        lkp.ip_ttl : ternary;
        lkp.ip_frag : ternary;
        lkp.tcp_flags : ternary;
        lkp.mac_type : ternary;
        '''
        try:
            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on dst_ip
            print("Add ACL entry to match and report on dst_ip field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dst_ip='11.11.11.1',
                dst_ip_mask='255.255.255.255',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on src_ip
            print("Add ACL entry to match and report on src_ip field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                src_ip='20.20.20.1',
                src_ip_mask='255.255.255.255',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on ip_proto
            print("Add ACL entry to match and report on ip_proto field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                ip_proto=6,
                ip_proto_mask=127,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Report the 2nd packet also since IP_PROTO=6 matches")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_no_dtel_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on ip_tos
            print("Add ACL entry to match and report on ip_tos field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                ip_dscp=2,
                ip_dscp_mask=127,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on tcp sport
            print("Add ACL entry to match and report on l4 src port field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                l4_src_port=3333,
                l4_src_port_mask=32759,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on tcp dport
            print("Add ACL entry to match and report on l4 dst port field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                l4_dst_port=5555,
                l4_dst_port_mask=32759,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on ip ttl
            print("Add ACL entry to match and report on ip ttl field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                ttl=64,
                ttl_mask=127,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on tcp flags
            print("Add ACL entry to match and report on tcp flags field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                tcp_flags=4,
                tcp_flags_mask=127,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            # match on ethertype
            print("Add ACL entry to match and report on ethertype field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                eth_type=0x0800,
                eth_type_mask=0x7FFF,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Report the 2nd packet also since eth type 0x800 matches")
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_no_dtel_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            self.cleanlast()
        finally:
            pass

    def IPv6FlowDtelAclTableFieldTest(self):
        print("IPv6DtelAclTableFieldTest()")

        try:
            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packet(self, self.exp_pkt_v6, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt_v6)
            verify_packet(self, self.exp_no_dtel_pkt_v6, self.devports[2])
            verify_no_other_packets(self)
            # match on dst_ip
            print("Add ACL entry to match and report on dst_ip field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packet(self, self.exp_pkt_v6, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt_v6, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt_v6)
            verify_packet(self, self.exp_no_dtel_pkt_v6, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packet(self, self.exp_pkt_v6, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt_v6)
            verify_packet(self, self.exp_no_dtel_pkt_v6, self.devports[2])
            verify_no_other_packets(self)
            # match on src_ip
            print("Add ACL entry to match and report on src_ip field")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                src_ip='2000::1',
                src_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packet(self, self.exp_pkt_v6, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt_v6, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Do not report unmatched packet")
            send_packet(self, self.devports[0], self.no_dtel_pkt_v6)
            verify_packet(self, self.exp_no_dtel_pkt_v6, self.devports[2])
            verify_no_other_packets(self)
            self.cleanlast()
        finally:
            pass

    def FlowDtelAclPriorityTest(self):
        print("FlowDtelAclPriorityTest()")

        try:
            print("Send packet matching dtel_acl entry for dst_ip with /24")
            acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dst_ip='11.11.11.0',
                dst_ip_mask='255.255.255.0',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                priority=100,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_no_dtel_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Flow reports generated")
            print("Add dtel_acl entry for dst_ip with /28, with action NONE")
            acl_entry_2 = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dst_ip='11.11.11.0',
                dst_ip_mask='255.255.255.240',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_NONE,
                action_report_all_packets=True,
                priority=10,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            print("No flow reports generated")
            print("Add dtel_acl entry for dst_ip with /32, with action FLOW")
            acl_entry_3 = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dst_ip='11.11.11.1',
                dst_ip_mask='255.255.255.255',
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                priority=1,
                table_handle=self.dtel_acl_table)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            print("Report generated for packet matching /32")
            print("No report for packet matching /28 but not matching /32")
            self.cleanlast()
            print("Delete dtel_acl entry for dst_ip with /32")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_no_other_packets(self)
            print("No flow reports generated")
            self.attribute_set(
                acl_entry_2,
                SWITCH_ACL_ENTRY_ATTR_ACTION_REPORT_TYPE,
                SWITCH_DTEL_REPORT_TYPE_FLOW)
            print("Change report_type of /28 entry to FLOW")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_no_dtel_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Flow reports generated")
            self.cleanlast()
            print("Delete dtel_acl entry for dst_ip with /28")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            send_packet(self, self.devports[0], self.no_dtel_pkt)
            verify_packet(self, self.exp_no_dtel_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_no_dtel_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Flow reports generated")
            self.cleanlast()
        finally:
            pass

###############################################################################


@group('dtel')
class FlowReportSessionTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Flow report feature not enabled, skipping")
            return

        print("Test setting report session attributes with flow reports")
        configure_dtel(self, f=True)
        bind_postcard_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.240',
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=256)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=256)
        try:
            self.ReportSessionDstUdpPortTest()
            self.ReportSessionSrcIPAddrTest()
            self.ReportSessionDstIPAddrTest()
            self.ReportSessionIPTosTest()
            self.ReportSessionIPTtlTest()
            self.ReportSessionTruncateSizeTest()
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()

    def ReportSessionDstUdpPortTest(self):
        try:
            udp_port_dtel_report = UDP_PORT_DTEL_REPORT^0x1111
            print("Change DTEL report UDP port from %x to %x" % (UDP_PORT_DTEL_REPORT, udp_port_dtel_report))

            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_UDP_DST_PORT,
                udp_port_dtel_report)
            if self.int_v2:
                split_layers(UDP, DTEL_REPORT_V2_HDR, dport=UDP_PORT_DTEL_REPORT)
                bind_layers(UDP, DTEL_REPORT_V2_HDR, dport=udp_port_dtel_report)
            else:
                split_layers(UDP, DTEL_REPORT_HDR, dport=UDP_PORT_DTEL_REPORT)
                bind_layers(UDP, DTEL_REPORT_HDR, dport=udp_port_dtel_report)

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            exp_postcard_pkt = exp_postcard_packet(
                          self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1)
            exp_postcard_pkt["UDP"].dport = udp_port_dtel_report
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_pkt, self.devports[1])
        finally:
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_UDP_DST_PORT,
                UDP_PORT_DTEL_REPORT)
            if self.int_v2:
                split_layers(UDP, DTEL_REPORT_V2_HDR, dport=udp_port_dtel_report)
                bind_layers(UDP, DTEL_REPORT_V2_HDR, dport=UDP_PORT_DTEL_REPORT)
            else:
                split_layers(UDP, DTEL_REPORT_HDR, dport=udp_port_dtel_report)
                bind_layers(UDP, DTEL_REPORT_HDR, dport=UDP_PORT_DTEL_REPORT)

    def ReportSessionSrcIPAddrTest(self):
        try:
            print("Change DTEL report source IP address to 4.4.4.100")
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_SRC_IP,
                '4.4.4.100')

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            exp_postcard_pkt = exp_postcard_packet(
                          self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1)
            exp_postcard_pkt["IP"].src = '4.4.4.100'
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_pkt, self.devports[1])
        finally:
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_SRC_IP,
                params.ipaddr_report_src[0])

    def ReportSessionDstIPAddrTest(self):
        try:
            print("Change DTEL report destination IP address to 4.4.4.9")
            dst_ip_list = []
            dst_ip_list.append(switcht_list_val_t(
                type=switcht_value_type.IP_ADDRESS, ip_addr='4.4.4.9'))
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_DST_IP_LIST,
                dst_ip_list)

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            exp_postcard_pkt = exp_postcard_packet(
                          self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1)
            exp_postcard_pkt["IP"].dst = '4.4.4.9'
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_pkt, self.devports[1])
        finally:
            dst_ip_list = []
            dst_ip_list.append(switcht_list_val_t(
                type=switcht_value_type.IP_ADDRESS,
                ip_addr=params.ipaddr_report_dst[0]))
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_DST_IP_LIST,
                dst_ip_list)

    def ReportSessionIPTtlTest(self):
        try:
            print("Change DTEL report IP TTL to 16")
            self.attribute_set(
                self.report_session, SWITCH_REPORT_SESSION_ATTR_TTL, 16)

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            exp_postcard_pkt = exp_postcard_packet(
                          self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1)
            exp_postcard_pkt["IP"].ttl = 15
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_pkt, self.devports[1])
        finally:
            self.attribute_set(
                self.report_session, SWITCH_REPORT_SESSION_ATTR_TTL, 64)

    def ReportSessionIPTosTest(self):
        try:
            print("Change DTEL report IP ToS to 32")
            self.attribute_set(
                self.report_session, SWITCH_REPORT_SESSION_ATTR_TOS, 32)

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            exp_postcard_pkt = exp_postcard_packet(
                          self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1)
            exp_postcard_pkt["IP"].tos = 32
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_pkt, self.devports[1])
        finally:
            self.attribute_set(
                self.report_session, SWITCH_REPORT_SESSION_ATTR_TOS, 0)

    def ReportSessionTruncateSizeTest(self):
        try:
            print("Change DTEL report truncate size to 128")
            truncate_size = 128
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_TRUNCATE_SIZE,
                truncate_size)

            truncated_amount = 256 - truncate_size

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            exp_postcard_pkt_full = exp_postcard_packet(
                          self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1)
            exp_postcard_pkt = truncate_packet(exp_postcard_pkt_full, truncated_amount)
            exp_postcard_pkt["IP"].len -= truncated_amount
            exp_postcard_pkt["UDP"].len -= truncated_amount
            if self.int_v2:
                exp_postcard_pkt[DTEL_REPORT_V2_HDR].report_length -= (
                    truncated_amount + 3) // 4
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_pkt, self.devports[1])
        finally:
            self.attribute_set(
                self.report_session,
                SWITCH_REPORT_SESSION_ATTR_TRUNCATE_SIZE,
                512)

###############################################################################

# Please remember to restart the model if this test is run
@disabled
class FlowReportSuppressionTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Flow report feature not enabled, skipping")
            return

        print("Test flow report")
        configure_dtel(self, f=True)
        bind_postcard_pkt(self.int_v2)

        self.dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            dst_ip='11.11.11.0',
            dst_ip_mask='255.255.255.240',
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
            priority=100,
            table_handle=self.dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.dtel_acl_table)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.dtel_acl_table)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.3',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            tcp_flags=0)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.3',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            tcp_flags=0)

        try:
            self.FlowReportSimpleSuppressionTest()
            self.FlowReportNoSuppressionTest()
            self.FlowReportSuppressionPathChangeTest()
            self.FlowReportSuppressionLatencyChangeTest()
            # Redo simple suppression test to catch last latency change
            self.FlowReportSimpleSuppressionTest()
            self.FlowReportSuppressionTCPTest()
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()

    def FlowReportSimpleSuppressionTest(self):
        try:
            print("Valid report is generated for 11.11.11.3")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Report should not be generated for 11.11.11.3 again")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            pass

    def FlowReportNoSuppressionTest(self):
        try:
            print("Add DTEL watchlist entry with action_report_all_packets")
            dtel_acl_entry_2 = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                dst_ip='11.11.11.3',
                dst_ip_mask='255.255.255.255',
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                priority=10,
                table_handle=self.dtel_acl_table)
            print("Flow report should be generated")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Remove last DTEL watchlist entry")
            self.cleanlast()
            print("Report should not be generated for 11.11.11.3 again")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            pass

    def FlowReportSuppressionPathChangeTest(self):
        try:
            print("Send identical packet from a different port")
            print("Sending packet port %d" % self.devports[1], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[1], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[1],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Send identical packet from original port")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

    def FlowReportSuppressionLatencyChangeTest(self):
        try:
            print("Send identical packet from original port => no report")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
            print("Change latency sensitivity to 0")
            self.attribute_set(
                self.device, SWITCH_DEVICE_ATTR_LATENCY_SENSITIVITY, 0)
            print("Flow reports due to latency change when quota = 0")
            num_packets = 20
            num_postcards_rcvd = 0
            for i in range(num_packets):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[2])
                postcard_pkt = receive_postcard_packet(
                    self,
                    exp_postcard_packet(
                        self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2,
                        f=1),
                    self.devports[1])
                if postcard_pkt is not None:
                    num_postcards_rcvd += 1
            self.assertTrue(num_postcards_rcvd >= num_packets * 0.30,
                "Not enough postcards received due to latency change:"
                " %d" % num_postcards_rcvd)
        finally:
            print("Reset latency sensitivity to a higher value")
            #workaround for model bug that reports high latency values
            if test_param_get('target') == "asic-model":
                self.attribute_set(
                    self.device, SWITCH_DEVICE_ATTR_LATENCY_SENSITIVITY, 30)
            else:
                self.attribute_set(
                    self.device, SWITCH_DEVICE_ATTR_LATENCY_SENSITIVITY, 18)

    def FlowReportSuppressionTCPTest(self):
        try:
            print("Send SYN packet, should generate a report")
            self.pkt["TCP"].flags = "S"
            self.exp_pkt["TCP"].flags = "S"
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Send packet with tcp_flags=0, should not generate any report")
            self.pkt["TCP"].flags = 0
            self.exp_pkt["TCP"].flags = 0
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
            print("Send RST packet, should generate a report")
            self.pkt["TCP"].flags = "R"
            self.exp_pkt["TCP"].flags = "R"
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Send packet with tcp_flags=P, should not generate any report")
            self.pkt["TCP"].flags = "P"
            self.exp_pkt["TCP"].flags = "P"
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
            print("Send FIN packet, should generate a report")
            self.pkt["TCP"].flags = "F"
            self.exp_pkt["TCP"].flags = "F"
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.3)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0],self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

###############################################################################
@group('dtel')
class FlowReportMultiMirrorTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Flow report feature not enabled, skipping")
            return

        print("Test flow reports to multiple mirror destinations")
        orig_report_ports = params.report_ports
        orig_ipaddr_report_src = params.ipaddr_report_src
        orig_ipaddr_report_dst = params.ipaddr_report_dst
        params.report_ports = [1, 1, 1, 1]
        params.ipaddr_report_src = ['4.4.4.1', '4.4.4.1', '4.4.4.1', '4.4.4.1']
        params.ipaddr_report_dst = ['4.4.4.3', '4.4.4.4', '4.4.4.5', '4.4.4.6']
        configure_dtel(self, f=True)
        bind_postcard_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.240',
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        payload = 'flow report'

        try:
            max_itrs = 100
            random.seed(314159)
            mirror_sessions_num = len(params.report_ports)
            count = [0] * mirror_sessions_num
            mirror_ports = [0] * mirror_sessions_num
            exp_postcard_pkt = [None] * mirror_sessions_num
            ignore_seq_num = True
            ignore_seq_num_session_count = 0
            exp_seq_num = [0] * mirror_sessions_num
            for i in range(0, mirror_sessions_num):
                mirror_ports[i] = self.devports[params.report_ports[i]]
            for i in range(0, max_itrs):
                src_port = i + 10000
                dst_port = i + 10001

                pkt = simple_udp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='11.11.11.2',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_ttl=64,
                    udp_sport=src_port,
                    udp_dport=dst_port,
                    udp_payload=payload)
                exp_pkt = simple_udp_packet(
                    eth_dst='00:11:22:33:44:22',
                    eth_src='00:77:66:55:44:33',
                    ip_dst='11.11.11.2',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_ttl=63,
                    udp_sport=src_port,
                    udp_dport=dst_port,
                    udp_payload=payload)

                for j in range(0, len(params.report_ports)):
                    exp_postcard_pkt[j] = exp_postcard_packet(
                        exp_pkt, self.int_v2, self.fp[0], self.fp[2], 0, f=1)
                    exp_postcard_pkt[j]["IP"].dst = params.ipaddr_report_dst[j]
                    if self.int_v2:
                        exp_postcard_pkt[j][DTEL_REPORT_V2_HDR].sequence_number = exp_seq_num[j]
                    else:
                        exp_postcard_pkt[j][DTEL_REPORT_HDR].sequence_number = exp_seq_num[j]
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[2])
                (rcv_index, seq_num) = verify_any_dtel_packet_any_port(
                    self, exp_postcard_pkt, mirror_ports,
                    ignore_seq_num=ignore_seq_num)
                print(("%d %d   %d" % (i, rcv_index, seq_num)))
                count[rcv_index] += 1
                if (exp_seq_num[rcv_index] == 0):
                    ignore_seq_num_session_count += 1
                if (ignore_seq_num_session_count == mirror_sessions_num):
                    ignore_seq_num = False
                exp_seq_num[rcv_index] = seq_num + 1

            for i in range(0, mirror_sessions_num):
                self.assertTrue(
                    (count[i] >= ((max_itrs / float(mirror_sessions_num)) * 0.50)),
                    "Not all mirror sessions are equally balanced"
                    " (%d < %f (%d%%) for %d" % (count[i],
                       ((max_itrs / float(mirror_sessions_num)) * 0.50), 50, i))
                print("mirror session %d count %d" % (i, count[i]))

            print("passed balancing the load among telemetry mirror sessions")

        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()
            params.report_ports = orig_report_ports
            params.ipaddr_report_src = orig_ipaddr_report_src
            params.ipaddr_report_dst = orig_ipaddr_report_dst

###############################################################################
@group('dtel')
class FlowandQueueReportTest(ApiHelper):
    def runTest(self):
        print()
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("This DTEL testcase is not supported with ACL portgroup")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_QUEUE_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Queue report or flow report feature not enabled")
            print("skipping")
            return

        print("Test combination of flow and queue reports")
        configure_dtel(self, f=False, q=False)
        bind_postcard_pkt(self.int_v2)

        queue_handles = self.attribute_get(
            self.port2, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.qr_id = self.add_queue_report(
            self.device, queue_handle=queue_handles[0].oid,
            latency_threshold=10, depth_threshold=0, breach_quota=10)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            self.QueueReportEnabledFlowReportDisabledTest()
            self.QueueReportEnabledFlowReportEnabledTest()
            self.QueueReportDisabledFlowReportEnabledTest()
        finally:
            split_postcard_pkt(self.int_v2)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()

    def QueueReportEnabledFlowReportDisabledTest(self):
        try:
            print("Queue report enabled, flow report disabled, quota set to 1")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_QUEUE_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, False)
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 1)
            print("Queue report is generated when quota is 1")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1), self.devports[1])
            verify_no_other_packets(self)
            print("No report when quota is 0")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_no_other_packets(self)
        finally:
            pass

    def QueueReportEnabledFlowReportEnabledTest(self):
        try:
            print("Queue report enabled, flow report enabled, quota set to 1")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_QUEUE_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 1)
            print("Report is generated when quota is 1")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=1, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Report with cleared q bit when quota is 0")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=0, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Set high queue report thresholds")
            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 10)
            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_DEPTH_THRESHOLD, 1000)
            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_LATENCY_THRESHOLD,
                500000000)
            print("Report with cleared q bit is generated")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=0, f=1), self.devports[1])
            verify_no_other_packets(self)
        finally:
            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_DEPTH_THRESHOLD, 0)
            self.attribute_set(
                self.qr_id, SWITCH_QUEUE_REPORT_ATTR_LATENCY_THRESHOLD, 10)

    def QueueReportDisabledFlowReportEnabledTest(self):
        try:
            print("Queue report disabled, flow report enabled, quota set to 1")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_QUEUE_REPORT, False)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            self.attribute_set(self.qr_id, SWITCH_QUEUE_REPORT_ATTR_BREACH_QUOTA, 1)
            print("Flow report with cleared q bit is generated when quota is 1")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=0, f=1), self.devports[1])
            verify_no_other_packets(self)
            print("Flow report with cleared q bit is generated when quota is 0")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 2, q=0, f=1), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

###############################################################################
@group('dtel')
class FlowandIngressMoDReportTest(ApiHelper):
    def runTest(self):
        print()
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("This DTEL testcase is not supported with ACL portgroup")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Drop report or flow report feature not enabled")
            print("skipping")
            return

        print("Test combination of flow and ingress drop reports")
        configure_dtel(self, f=False, d=False)
        bind_mirror_on_drop_pkt(self.int_v2)

        acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW | SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table,
            acl_group_handle=acl_group)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=dtel_acl_table,
            acl_group_handle=acl_group)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            self.DropReportEnabledFlowReportDisabledTest()
            self.DropReportEnabledFlowReportEnabledTest()
            self.DropReportDisabledFlowReportEnabledTest()
        finally:
            split_mirror_on_drop_pkt(self.int_v2)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()

    def DropReportEnabledFlowReportDisabledTest(self):
        try:
            print("Drop report enabled, flow report disabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, False)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            # 80 is SWITCH_DROP_REASON_ACL_DROP
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

    def DropReportEnabledFlowReportEnabledTest(self):
        try:
            print("Drop report enabled, flow report enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            # 80 is SWITCH_DROP_REASON_ACL_DROP
            verify_dtel_packet(self, exp_mod_packet(self.pkt, self.int_v2, self.fp[0], 4, 80, f=1), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

    def DropReportDisabledFlowReportEnabledTest(self):
        try:
            print("Drop report disabled, flow report enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, False)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

###############################################################################
@group('dtel')
class FlowandEgressMoDReportTest(ApiHelper):
    def runTest(self):
        print()
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("This DTEL testcase is not supported with ACL portgroup")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("Drop report or flow report or egress IP ACL not enabled")
            print("skipping")
            return

        print("Test combination of flow and egress drop reports")
        configure_dtel(self, f=False, d=False)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW | SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=10,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.attribute_set(
            self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 10)
        self.attribute_set(
            self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            self.EgressDropReportEnabledFlowReportDisabledTest()
            self.EgressDropReportEnabledFlowReportEnabledTest()
            self.EgressDropReportDisabledFlowReportEnabledTest()
        finally:
            split_mirror_on_drop_pkt(self.int_v2)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.cleanup()

    def EgressDropReportEnabledFlowReportDisabledTest(self):
        try:
            print("Drop report enabled, flow report disabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, False)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            # 92 is SWITCH_DROP_REASON_EGRESS_ACL_DROP
            verify_dtel_packet(self, exp_egress_mod_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

    def EgressDropReportEnabledFlowReportEnabledTest(self):
        try:
            print("Drop report enabled, flow report enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            # 92 is SWITCH_DROP_REASON_EGRESS_ACL_DROP
            verify_dtel_packet(self, exp_egress_mod_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92, f=1), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

    def EgressDropReportDisabledFlowReportEnabledTest(self):
        try:
            print("Drop report disabled, flow report enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, False)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

###############################################################################
@group('dtel')
class FlowandEgressPortMirrorTest(ApiHelper):
    def runTest(self):
        print()
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("This DTEL testcase is not supported with ACL portgroup")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_PORT_MIRROR) == 0
            or self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Flow report or egress port mirror not enabled")
            print("skipping")
            return

        print("Test combination of flow reports and egress port mirror")
        configure_dtel(self, f=True)
        bind_postcard_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW | SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        # Configure egress mirror session but do not yet attach to a port
        self.mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port1)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:22',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            self.FlowReportEnabledEgressMirrorDisabledTest()
            self.FlowReportEnabledEgressMirrorEnabledTest()
            self.FlowReportDisabledEgressMirrorEnabledTest()
        finally:
            split_postcard_pkt(self.int_v2)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            self.cleanup()

    def FlowReportEnabledEgressMirrorDisabledTest(self):
        try:
            print("Flow report enabled, egress mirror disabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[2])
            verify_postcard_packet(self, exp_postcard_packet(self.exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1), self.devports[1])
            verify_no_other_packets(self)
        finally:
            pass

    def FlowReportEnabledEgressMirrorEnabledTest(self):
        try:
            print("Flow report enabled, egress mirror enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, True)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.mirror)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(
                self, self.exp_pkt, [self.devports[1], self.devports[2]])
        finally:
            pass

    def FlowReportDisabledEgressMirrorEnabledTest(self):
        try:
            print("Flow report disabled, egress mirror enabled")
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_FLOW_REPORT, False)
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.mirror)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 11.11.11.2)")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(
                self, self.exp_pkt, [self.devports[1], self.devports[2]])
        finally:
            pass

###################################################################################
@group('dtel')
class MoDIngressDropReasonTest(ApiHelper):
    '''
    This test tries to verify user configurable ingress drop reason values
    '''

    def setUp(self):
        super(self.__class__, self).setUp()

        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            dst_ip='10.10.10.0',
            dst_ip_mask='255.255.255.0',
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table,
            priority=10)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:12', destination_handle=self.port3)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:22', destination_handle=self.port4)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:13', destination_handle=self.port3)

        self.lag0 = self.add_lag(self.device)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:32', destination_handle=self.lag0)
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return

        try:
            initial_stats = self.client.object_counters_get(self.port3)
            num_drops = 0
            print("Drop nexthop")
            nhop3 = self.add_nexthop(
                self.device,
                handle=self.rif0,   # Just here to avoid error messages
                type=SWITCH_NEXTHOP_ATTR_TYPE_DROP)
            route0 = self.add_route(
                self.device,
                ip_prefix='10.10.10.1',
                vrf_handle=self.vrf10,
                nexthop_handle=nhop3)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            dtel_drop_control_handle = self.add_dtel_drop_control(self.device, enable = False, dtel_handle= self.dtel, drop_reason= 93)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            time.sleep(1)
            self.attribute_set(dtel_drop_control_handle, SWITCH_DTEL_DROP_CONTROL_ATTR_ENABLE, True)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            #SWITCH_DROP_REASON_NEXTHOP 93
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, self.fp[0], 4, 93), self.devports[1])
            dtel_drop_control_handle = self.add_dtel_drop_control(self.device, enable = False, dtel_handle = self.dtel, drop_reason = 25)
            print("IPv4 invalid version, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            pkt["IP"].version = 6
            send_packet(self, self.devports[3], pkt)
            verify_packet(self, pkt, self.devports[4])
            print("L2 unicast miss, drop")
            self.attribute_set(
                self.device,
                SWITCH_DEVICE_ATTR_UNICAST_MISS_PACKET_ACTION,
                SWITCH_MAC_ENTRY_ATTR_ACTION_DROP)
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:44',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)

            dtel_drop_control_handle = self.add_dtel_drop_control(self.device, enable = False, dtel_handle = self.dtel, drop_reason = 89)
            send_packet(self, self.devports[3], pkt)
            # SWITCH_DROP_REASON_L2_MISS_UNICAST 89
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            # test drop report dependency
            self.attribute_set(dtel_drop_control_handle, SWITCH_DTEL_DROP_CONTROL_ATTR_ENABLE, True)
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, False)
            send_packet(self, self.devports[3], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            try:
                final_stats = self.client.object_counters_get(self.port3)
                drop_stats = {a.counter_id: (a.count - b.count) for a, b in zip(final_stats, initial_stats)}
                final_count = 0
                print("Drop Stats: ")
                for counter_id in drop_stats:
                    if (counter_id == SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS):
                        print("[%d:%d]" % (counter_id, drop_stats[counter_id]))
                        print()
                        final_count += drop_stats[counter_id]
                print("Expected drop count: %d" % num_drops)
                print("Final drop count   : %d" % final_count)
                self.assertEqual(num_drops, final_count)
            finally:
                pass
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(
                self.device,
                SWITCH_DEVICE_ATTR_UNICAST_MISS_PACKET_ACTION,
                SWITCH_MAC_ENTRY_ATTR_ACTION_FORWARD)
            self.attribute_set(
                self.device,
                SWITCH_DEVICE_ATTR_MULTICAST_MISS_PACKET_ACTION,
                SWITCH_MAC_ENTRY_ATTR_ACTION_FORWARD)
            self.attribute_set(
                self.device,
                SWITCH_DEVICE_ATTR_BROADCAST_MISS_PACKET_ACTION,
                SWITCH_MAC_ENTRY_ATTR_ACTION_FORWARD)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.cleanup()

#################################################################################################################################
@group('dtel')
class MoDEgressDropReasonTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()

        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=True,
            table_handle=dtel_acl_table)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=10,
            port_lag_label_mask=0xFF,
            dst_ip='11.11.11.2',
            dst_ip_mask='255.255.255.0',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, dtel_acl_table)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("Drop report or egress IP ACL feature not enabled, skipping")
            return

        print ("Test drop report due to drop reason : egress ipv4 acl deny action")
        try:
            print("Sending packets from port %d" % (self.devports[0]))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:22',
                eth_src='00:77:66:55:44:33',
                ip_dst='11.11.11.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            dtel_drop_control_handle = self.add_dtel_drop_control(self.device, enable = False, dtel_handle = self.dtel, drop_reason= 92)
            send_packet(self, self.devports[0], pkt)
            # 92 is SWITCH_DROP_REASON_EGRESS_ACL_DROP
            verify_no_other_packets(self, timeout=1)
            self.attribute_set(dtel_drop_control_handle, SWITCH_DTEL_DROP_CONTROL_ATTR_ENABLE, True)
            send_packet(self, self.devports[0], pkt)
            verify_dtel_packet(self, exp_egress_mod_packet(exp_pkt, self.int_v2, self.fp[0], self.fp[2], 4, 92), self.devports[1])
            # test drop report dependency
            self.attribute_set(self.dtel, SWITCH_DTEL_ATTR_DROP_REPORT, False)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            self.cleanup()
            return
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
        self.cleanup()


@group('dtel')
class DtelAclEthertypeTest(ApiHelper):
    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("This DTEL testcase is not supported with ACL port_group")
            return

        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Flow report feature not enabled, skipping")
            return

        configure_dtel(self, f=True)
        bind_postcard_pkt(self.int_v2)

        self.dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)

        # create hostif for port0
        self.hostif_name0 = "test_host_if0"
        self.hostif0 = self.add_hostif(self.device, name=self.hostif_name0, handle=self.port0, oper_status=True)
        self.assertTrue(self.hostif0 != 0)
        print("Sleeping for 5 secs")
        time.sleep(5)

        self.trap_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU],
        ]

        for trap in self.trap_list:
            trap.append(self.add_hostif_trap(self.device, type=trap[0],
                                             hostif_trap_group_handle=self.hostif_trap_group0,
                                             packet_action=trap[1]))

        try :
            if self.test_params['target'] != "hw":
                self.sock = open_packet_socket(self.hostif_name0)
            self.rx_cnt = 0
            self.pre_counter = self.client.object_counters_get(self.hostif0)
            pkt = simple_arp_packet(arp_op=1, eth_dst=self.rmac, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=0x100d,
                  inner_pkt=pkt)
            self.exp_arpq_uc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            # add acl rule to report specific ether type arp (0x806)
            acl_entry = self.add_acl_entry(self.device,
                  eth_type=0x806,
                  eth_type_mask=0x7FFF,
                  dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                  action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                  action_report_all_packets=True,
                  table_handle=self.dtel_acl_table)
            self.attribute_set(
                  self.port0,
                  SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.dtel_acl_table)

            print('Sending Unicast ARP request to router MAC')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != "hw":
                verify_packet(self, self.exp_arpq_uc_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            verify_postcard_packet(self, exp_postcard_packet(exp_pkt, self.int_v2, self.fp[0], 502, 1, f=1), self.devports[1])
            # try to match on a ether type not set(0x800) and no report should
            # be generated
            self.attribute_set(
                  self.port0,
                  SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
            tcp_pkt = simple_tcp_packet(
                  eth_dst='00:77:66:55:44:33',
                  eth_src='00:22:22:22:22:22',
                  ip_dst='11.11.11.1',
                  ip_src='20.20.20.1',
                  ip_id=105,
                  ip_tos=8,
                  tcp_sport=3333,
                  tcp_dport=5555,
                  tcp_flags="R",
                  ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                  eth_dst='00:11:22:33:44:22',
                  eth_src='00:77:66:55:44:33',
                  ip_dst='11.11.11.1',
                  ip_src='20.20.20.1',
                  ip_id=105,
                  ip_tos=8,
                  tcp_sport=3333,
                  tcp_dport=5555,
                  tcp_flags="R",
                  ip_ttl=63)
            send_packet(self, self.devports[0], tcp_pkt)
            verify_packet(self, exp_pkt, self.devports[2])
            verify_no_packet(self, exp_postcard_packet(exp_pkt, self.int_v2, self.fp[0], self.fp[2], 1, f=1), self.devports[1], timeout=2)
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()
            pass

@disabled
class DropReportPerfTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()

        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            return

        configure_dtel(self, d=True)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        dtel_acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            action_report_type=SWITCH_DTEL_REPORT_TYPE_DROP,
            action_report_all_packets=False,
            table_handle=dtel_acl_table)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=5,
            port_lag_label_mask=0xFF,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0):
            print("Drop report feature not enabled, skipping")
            return

        print("Test performance of drop report bloom filter at scale, with many thousands of flows")

        try:
            self.attribute_set(
                self.device,
               SWITCH_DEVICE_ATTR_FLOW_STATE_CLEAR_CYCLE, 2)
            time.sleep(10)
            self.attribute_set(
                self.device,
               SWITCH_DEVICE_ATTR_FLOW_STATE_CLEAR_CYCLE, 0)
            time.sleep(10)

            num_no_report_pkts = 0
            payload = 'perf test'
            init_sport = 10000
            init_dport = 23000
            for max_itrs in [32000]:
                for i in range(0, max_itrs):
                    src_port = init_sport + ((i * 7) % 32768)
                    dst_port = init_dport + ((i * 17) % 32768)
                    temp = (i % 256)
                    dst_ip = '11.11.11.' + repr(temp)
                    pkt = simple_udp_packet(
                        eth_dst='00:77:66:55:44:33',
                        eth_src='00:22:22:22:22:22',
                        ip_dst=dst_ip,
                        ip_src='192.168.0.1',
                        ip_id=105,
                        ip_ttl=64,
                        udp_sport=src_port,
                        udp_dport=dst_port,
                        udp_payload=payload)
                    send_packet(self, self.devports[0], pkt)
                    nrcv = receive_dtel_packet(
                        self,
                        exp_mod_packet(pkt, False, self.fp[0], 4, 80),
                        self.devports[1])
                    if nrcv:
                        sys.stdout.write('-')
                    else:
                        sys.stdout.write('|')
                        num_no_report_pkts += 1
                    verify_no_other_packets(self)
                    sys.stdout.flush()
                    if (i+1) % 1000 == 0:
                        print (i+1)
                        print ("Missed drop reports / total #of dropped flows " \
                              "= %d / %d " % (num_no_report_pkts, (i + 1)))
                        num_no_report_pkts = 0
        finally:
            if self.client.is_feature_enable(SWITCH_FEATURE_DROP_REPORT) == 0:
                self.cleanup()
                return
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.cleanup()
            pass

@disabled
class DtelFlowPerfTest(ApiHelper):
    def runTest(self):
        print()

        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Flow report feature not enabled, skipping")
            return

        print("Test performance of flow report bloom filter at scale, with many thousands of flows")
        configure_dtel(self, f=True)
        bind_postcard_pkt(self.int_v2)

        self.dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(
            self.port0,
            SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 5)
        acl_entry = self.add_acl_entry(self.device,
                port_lag_label=5,
                port_lag_label_mask=0xFF,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=False,
                table_handle=self.dtel_acl_table)

        try:
            for max_iter in [2000, 4000, 8000]:
                fp_indices=[]
                dup_indices = []
                num_no_report_pkts = 0
                num_rcvd_pkts = 0
                payload = 'perf test'
                init_sport = 10000
                init_dport = 23000
                src_port = init_sport
                dst_port = init_dport
                self.attribute_set(self.device,
                     SWITCH_DEVICE_ATTR_FLOW_STATE_CLEAR_CYCLE, 2)
                time.sleep(10)
                self.attribute_set(self.device,
                     SWITCH_DEVICE_ATTR_FLOW_STATE_CLEAR_CYCLE, 0)
                time.sleep(10)

                for k in range(0, 2):
                    for i in range(max_iter):
                        src_port = init_sport + ((i * 7) % 32768)
                        dst_port = init_dport + ((i * 17) % 32768)
                        temp = i % 256;
                        dst_ip = '11.11.11.' + repr(temp)
                        pkt = simple_udp_packet(
                            eth_dst='00:77:66:55:44:33',
                           eth_src='00:22:22:22:22:22',
                           ip_dst=dst_ip,
                           ip_src='192.168.0.1',
                           ip_id=105,
                           ip_ttl=64,
                           udp_sport=src_port,
                           udp_dport=dst_port,
                           udp_payload=payload)
                        exp_pkt = simple_udp_packet(
                            eth_dst='00:11:22:33:44:22',
                           eth_src='00:77:66:55:44:33',
                           ip_dst=dst_ip,
                           ip_src='192.168.0.1',
                           ip_id=105,
                           ip_ttl=63,
                           udp_sport=src_port,
                           udp_dport=dst_port,
                           udp_payload=payload)
                        send_packet(self, self.devports[0], pkt)
                        verify_packet(self, exp_pkt, self.devports[2])
                        nrcv = receive_postcard_packet(
                            self,
                            exp_postcard_packet(exp_pkt, False, self.fp[0],
                                                self.fp[2], 1, f=1),
                            self.devports[1])
                        verify_no_other_packets(self, timeout = 0)
                        if k == 0:
                            if nrcv:
                                sys.stdout.write('-')
                            else:
                                sys.stdout.write('|')
                                num_no_report_pkts += 1
                                fp_indices.append(i)
                        else:
                            if nrcv:
                                sys.stdout.write('|')
                                num_rcvd_pkts += 1
                                dup_indices.append(i)
                            else:
                                sys.stdout.write('-')
                        sys.stdout.flush()
                        if (i+1) % 1000==0:
                            print (i+1)

                    print("total #of flows = %d" % max_iter)
                    print ("Missed flow reports indices during first run = %d " % \
                       (num_no_report_pkts))
                    print ("First flow reports during second run" , (set(fp_indices) & set(dup_indices)))
                    print("Duplicate flow reports = %d" %  (num_rcvd_pkts - len(set(fp_indices) & set(dup_indices))))
        finally:
            if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
                self.cleanup()
                return

            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.cleanup()
            pass

##############################################################################################################################
@group('dtel')
class DtelTunnelFlowWatchListTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()

        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_INNER_DTEL) == 0):
            print("Inner DTEL feature not enabled, skipping")
            return

        configure_dtel(self, f=True)
        bind_postcard_pkt(self.int_v2)

        acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_switch])
        # Dtel inner and outer tables
        self.inner_dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_INNER_DTEL_IPV4,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.dtel_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=self.inner_dtel_acl_table,
            acl_group_handle=acl_group)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=self.dtel_acl_table,
            acl_group_handle=acl_group)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_group)

        #Tunnel configuration
        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip = '100.100.1.1'
        self.vm_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9911'
        self.customer_ip = '100.100.3.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.vni = 0x1234
        self.l2_vni = 0x4321
        self.my_lb_ip = '1.1.1.3'
        self.tunnel_ip = '1.1.1.1'
        self.inner_dmac = "00:11:11:11:11:11"
        self.underlay_neighbor_mac = '00:33:33:33:33:33'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port10, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port9, vrf_handle=self.ovrf, src_mac=self.rmac)


        # Configure MTUs for the RIFs
        tcpv4_pkt = simple_tcp_packet()
        tcpv6_pkt = simple_tcpv6_packet()
        self.v4mtu_offset = len(simple_vxlan_packet(inner_frame=tcpv4_pkt)) - len(tcpv4_pkt)
        self.v6mtu_offset = len(simple_vxlan_packet(inner_frame=tcpv6_pkt)) - len(tcpv6_pkt)
        self.overlay_v4_mtu = 200
        self.overlay_v6_mtu = 500
        self.underlay_v4_mtu = self.overlay_v4_mtu + self.v4mtu_offset - 1
        self.underlay_v6_mtu = self.overlay_v6_mtu + self.v6mtu_offset - 1

        self.attribute_set(self.orif, SWITCH_RIF_ATTR_MTU, self.overlay_v4_mtu)
        self.attribute_set(self.urif, SWITCH_RIF_ATTR_MTU, self.underlay_v4_mtu)

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # Create route to customer from VM
        self.onhop = self.add_nexthop(self.device, handle=self.orif, dest_ip='100.100.3.1')
        self.oneighbor = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer_ip)  # 100.100.3.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)
        self.customev6_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)

        # Create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 1.1.1.1
            mac_address=self.inner_dmac,
            tunnel_vni=self.vni)

        # Add routes for VM (v4 & v6 adress) via tunnel nexthop
        self.customerv4_route = self.add_route(self.device,
            ip_prefix=self.vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customerv6_route = self.add_route(self.device,
            ip_prefix=self.vm_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to tunnel ip 1.1.1.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)
        self.tunnel_route = self.add_route(self.device, ip_prefix=self.tunnel_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # Create more egress RIFs
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port7, vrf_handle=self.uvrf, src_mac=self.rmac)
        nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='20.20.0.1')
        self.add_neighbor(self.device, mac_address='00:22:22:22:22:33', handle=rif2, dest_ip='20.20.0.1')
        self.add_route(self.device, ip_prefix='20.20.1.1', vrf_handle=self.uvrf, nexthop_handle=nhop2)

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
                return
            if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
                return
            if (self.client.is_feature_enable(SWITCH_FEATURE_INNER_DTEL) == 0):
                return
            self.VxlanTunnelTest()
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0 or
            self.client.is_feature_enable(SWITCH_FEATURE_INNER_DTEL) == 0):
            self.cleanup()
            return
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
        self.cleanup()
    def VxlanTunnelTest(self):
        try:
            # packets
            pkt_tcp = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            pkt_udp = simple_udp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            pkt2_tcp = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=63)
            pkt2_udp = simple_udp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=63)
            pkt_encap_tcp = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src = self.customer_ip,
                ip_dst = self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            pkt_encap_udp = simple_udp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src = self.customer_ip,
                ip_dst = self.vm_ip,
                ip_id=108,
                ip_ttl=64)

            # basic vxlan packets with valid outer and inner headers
            vxlan_pkt_tcp = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt_tcp)
            vxlan_pkt_udp = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt_udp)

            #Decap rule
            dtel_acl_entry = self.add_acl_entry(self.device,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                dst_ip=self.customer_ip,
                dst_ip_mask='255.255.255.255',
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.inner_dtel_acl_table)

            # Encap rule
            dtel_acl_entry = self.add_acl_entry(self.device,
                dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
                dst_ip=self.vm_ip,
                dst_ip_mask='255.255.255.255',
                action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
                action_report_all_packets=True,
                table_handle=self.dtel_acl_table)

            print("Vxlan Decap --> Dtel report : Valid v4 packet, vni 0x1234 from L3 intf port 10 (tcp)")
            send_packet(self, self.devports[10], vxlan_pkt_tcp)
            verify_packet(self, pkt2_tcp, self.devports[9])
            verify_postcard_packet(self, exp_postcard_packet(pkt2_tcp, self.int_v2, self.fp[10],self.fp[9], 1, f=1), self.devports[1])

            print("Vxlan Decap --> Dtel report : Valid v4 packet, vni 0x1234 from L3 intf port 10 (udp)")
            send_packet(self, self.devports[10], vxlan_pkt_udp)
            verify_packet(self, pkt2_udp, self.devports[9])
            verify_postcard_packet(self, exp_postcard_packet(pkt2_udp, self.int_v2, self.fp[10],self.fp[9], 1, f=1), self.devports[1])

            vxlan_pkt_tcp = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='20.20.1.1',
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt_tcp)
            vxlan_pkt_udp = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='20.20.1.1',
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt_udp)
            inner_pkt_tcp = simple_tcp_packet(
                eth_dst=self.inner_dmac,
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            inner_pkt_udp = simple_udp_packet(
                eth_dst=self.inner_dmac,
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)

            vxlan_exp_pkt_encap_tcp = mask.Mask(simple_vxlan_packet(
                eth_dst=self.underlay_neighbor_mac,
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.tunnel_ip,
                ip_src=self.my_lb_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt_tcp))
            vxlan_exp_pkt_encap_udp = mask.Mask(simple_vxlan_packet(
                eth_dst=self.underlay_neighbor_mac,
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.tunnel_ip,
                ip_src=self.my_lb_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt_udp))
            vxlan_exp_pkt_encap_dtel_tcp = simple_vxlan_packet(
                eth_dst=self.underlay_neighbor_mac,
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.tunnel_ip,
                ip_src=self.my_lb_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt_tcp)
            vxlan_exp_pkt_encap_dtel_udp = simple_vxlan_packet(
                eth_dst=self.underlay_neighbor_mac,
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.tunnel_ip,
                ip_src=self.my_lb_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt_udp)
            # Transit packet
            vxlan_exp_pkt_tcp = simple_vxlan_packet(
                eth_dst='00:22:22:22:22:33',
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='20.20.1.1',
                ip_src=self.tunnel_ip,
                ip_ttl=63,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt_tcp)
            vxlan_exp_pkt_udp = simple_vxlan_packet(
                eth_dst='00:22:22:22:22:33',
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='20.20.1.1',
                ip_src=self.tunnel_ip,
                ip_ttl=63,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt_udp)

            print("Vxlan Encap --> Dtel report : Valid v4 packet to L3 intf port 9 (tcp)")
            mask_set_do_not_care_packet(vxlan_exp_pkt_encap_tcp, UDP, 'sport')
            send_packet(self, self.devports[9], pkt_encap_tcp)
            verify_packet(self, vxlan_exp_pkt_encap_tcp, self.devports[10])
            vxlan_exp_pkt_encap_dtel_tcp["UDP"].sport = 0
            verify_postcard_packet(self, exp_postcard_packet(vxlan_exp_pkt_encap_dtel_tcp, self.int_v2, self.fp[9],self.fp[10], 1, f=1), self.devports[1])

            print("Vxlan Encap --> Dtel report : Valid v4 packet to L3 intf port 9 (udp)")
            mask_set_do_not_care_packet(vxlan_exp_pkt_encap_udp, UDP, 'sport')
            send_packet(self, self.devports[9], pkt_encap_udp)
            verify_packet(self, vxlan_exp_pkt_encap_udp, self.devports[10])
            vxlan_exp_pkt_encap_dtel_udp["UDP"].sport = 0
            verify_postcard_packet(self, exp_postcard_packet(vxlan_exp_pkt_encap_dtel_udp, self.int_v2, self.fp[9],self.fp[10], 1, f=1), self.devports[1])

            print("Valid v4 packet, vni 0x1234 from L3 intf port 1, outer ip miss dst_vtep, routed (tcp)")
            send_packet(self, self.devports[10], vxlan_pkt_tcp)
            verify_packet(self, vxlan_exp_pkt_tcp, self.devports[7])
            verify_postcard_packet(self, exp_postcard_packet(vxlan_exp_pkt_tcp, self.int_v2, self.fp[10],self.fp[7], 1, f=1), self.devports[1])

            print("Valid v4 packet, vni 0x1234 from L3 intf port 1, outer ip miss dst_vtep, routed (udp)")
            send_packet(self, self.devports[10], vxlan_pkt_udp)
            verify_packet(self, vxlan_exp_pkt_udp, self.devports[7])
            verify_postcard_packet(self, exp_postcard_packet(vxlan_exp_pkt_udp, self.int_v2, self.fp[10],self.fp[7], 1, f=1), self.devports[1])
        finally:
            pass
