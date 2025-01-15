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
Thrift SAI interface INT transit tests
"""

import switchsai_thrift
import pdb
import time
import sys
import logging
import os
import unittest
import random

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *
from switchsai_thrift.ttypes import *
from switchsai_thrift.sai_headers import *
from switch_utils import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '../'))
from common.utils import *
from common.dtel_utils import *

SID = 10000
swports = switch_ports[0:5]
devports = range(4)

mac_self = '00:77:66:55:44:00'
nports = 3
ipaddr_inf = ['2.2.0.1',  '1.1.0.1', '172.16.0.4']
ipaddr_nbr = ['2.2.0.200', '1.1.0.100', '172.16.0.1']
mac_nbr = ['00:11:22:33:44:54', '00:11:22:33:44:55', '00:11:22:33:44:56']
report_ports = [3]
report_src = '4.4.4.1'
report_dst = ['4.4.4.3']
report_udp_port = UDP_PORT_DTEL_REPORT
report_truncate_size = 256
configure_routes = True
collector_mac = '00:11:22:33:44:57'

if test_param_get('target') == "asic-model":
    reset_cycle = 6
    min_sleeptime = 75
else:
    reset_cycle = 1
    min_sleeptime = 1

def exp_mod_packet(packet, int_v2, ingress_port, hw_id, drop_reason, f=0):

    if int_v2:
        exp_mod_pkt = ipv4_dtel_v2_pkt(
            eth_dst=collector_mac,
            eth_src=mac_self,
            ip_src=report_src,
            ip_dst=report_dst[0],
            ip_id=0,
            ip_ttl=63,
            dropped=1,
            congested_queue=0,
            path_tracking_flow=f,
            hw_id=hw_id,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=INVALID_PORT_ID,
            ingress_tstamp=0,
            queue_id=0,
            drop_reason=drop_reason,
            inner_frame=packet)

    else:
        exp_mod_inner = mod_report(
            packet=packet,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=INVALID_PORT_ID,
            queue_id=0,
            drop_reason=drop_reason)

        exp_mod_pkt = ipv4_dtel_pkt(
            eth_dst=collector_mac,
            eth_src=mac_self,
            ip_src=report_src,
            ip_dst=report_dst[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_MOD,
            dropped=1,
            congested_queue=0,
            path_tracking_flow=f,
            hw_id=hw_id,
            inner_frame=exp_mod_inner)

    return exp_mod_pkt

def exp_postcard_packet(packet, int_v2, ing_port, eg_port, hw_id,
                        d=0, q=0, f=0):
    if int_v2:
        exp_e2e_pkt = ipv4_dtel_v2_pkt(
            eth_dst=collector_mac,
            eth_src=mac_self,
            ip_src=report_src,
            ip_dst=report_dst[0],
            ip_id=0,
            ip_ttl=63,
            dropped=d,
            congested_queue=q,
            path_tracking_flow=f,
            hw_id=hw_id,
            switch_id=SID,
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
            switch_id=SID,
            ingress_port=ing_port,
            egress_port=eg_port,
            queue_id=0,
            queue_depth=0,
            egress_tstamp=0)

        exp_e2e_pkt = ipv4_dtel_pkt(
            eth_dst=collector_mac,
            eth_src=mac_self,
            ip_src=report_src,
            ip_dst=report_dst[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL,
            dropped=d,
            congested_queue=q,
            path_tracking_flow=f,
            hw_id=hw_id,
            inner_frame=exp_pc_inner)

    return exp_e2e_pkt

class DataPath():
    rif = []
    nhop = []
    drif = 0
    dnhop = []
    vr_id = 0
    ip_family = SAI_IP_ADDR_FAMILY_IPV4
    mask = '255.255.255.255'

def configure_datapath(api, dp):
    client = api.client

    switch_init(client)

    dp.vr_id = sai_thrift_create_virtual_router(client, 1, 1)
    for i in range(len(mac_nbr)):
        rif = sai_thrift_create_router_interface(client, dp.vr_id,
            SAI_ROUTER_INTERFACE_TYPE_PORT, port_list[i], 0, 1, 1, '')
        dp.rif.append(rif)
        sai_thrift_create_neighbor(client, dp.ip_family, rif, ipaddr_inf[i], mac_nbr[i])
        nhop = sai_thrift_create_nhop(client, dp.ip_family, ipaddr_inf[i], rif)
        dp.nhop.append(nhop)
        sai_thrift_create_route(client, dp.vr_id, dp.ip_family, ipaddr_nbr[i], dp.mask, nhop)

    # configure dtel routes
    dp.drif = sai_thrift_create_router_interface(client, dp.vr_id,
        SAI_ROUTER_INTERFACE_TYPE_PORT, port_list[3], 0, 1, 1, '')
    sai_thrift_create_neighbor(client, dp.ip_family, dp.drif, '11.11.11.1', collector_mac)
    dp.dnhop = sai_thrift_create_nhop(client, dp.ip_family, '11.11.11.1', dp.drif)
    sai_thrift_create_route(client, dp.vr_id, dp.ip_family, report_dst[0], dp.mask, dp.dnhop)

    if (client.sai_thrift_is_feature_enable(SWITCH_FEATURE_INT_V2) != 0):
        api.int_v2 = True
    else:
        api.int_v2 = False

def remove_datapath(client, dp):
    sai_thrift_remove_route(client, dp.vr_id, dp.ip_family, report_dst[0], dp.mask, dp.dnhop)
    client.sai_thrift_remove_next_hop(dp.dnhop)
    sai_thrift_remove_neighbor(client, dp.ip_family, dp.drif, '11.11.11.1', collector_mac)
    client.sai_thrift_remove_router_interface(dp.drif)

    for i in range(len(mac_nbr)):
        sai_thrift_remove_route(client, dp.vr_id, dp.ip_family, ipaddr_nbr[i], dp.mask, dp.nhop[i])
        client.sai_thrift_remove_next_hop(dp.nhop[i])
        sai_thrift_remove_neighbor(client, dp.ip_family, dp.rif[i], ipaddr_inf[i], mac_nbr[i])
        client.sai_thrift_remove_router_interface(dp.rif[i])
    client.sai_thrift_remove_virtual_router(dp.vr_id)

@group('dtel')
class INGRESS_DROP_REPORT_Test(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print "Test DTel reports due to drops in the ingress pipeline"

        dp = DataPath()
        configure_datapath(self, dp)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(self.client, switch_id=SID)

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        # create DTEL watchlist
        dtel_acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=[SAI_ACL_BIND_POINT_TYPE_SWITCH],
            ip_src=ipaddr_nbr[0],
            ip_dst=ipaddr_nbr[1],
            action=SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE)

        # create DTEL watchlist entry
        dtel_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=dtel_acl_table_id,
            entry_priority=10,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            ip_dst=ipaddr_nbr[1],
            ip_dst_mask='255.255.255.0',
            dtel_drop_report_enable=True,
            dtel_report_all_packets=True)

        # Create ACL table
        bp_point = [SAI_ACL_BIND_POINT_TYPE_PORT]
        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=bp_point, ip_src=True, ip_dst=True)

        # Create ACL entry
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=acl_table_id,
            entry_priority=10,
            ip_src=ipaddr_nbr[0], ip_src_mask='255.255.255.0',
            action=SAI_PACKET_ACTION_DROP)

        payload = '@!#?'
        pkt = simple_udp_packet(
            eth_dst=mac_self,
            eth_src=mac_nbr[0],
            ip_dst=ipaddr_nbr[1],
            ip_src=ipaddr_nbr[0],
            ip_id=108,
            ip_ttl=64,
            with_udp_chksum=False,
            udp_payload=payload)

        exp_pkt = simple_udp_packet(
            eth_dst=mac_nbr[1],
            eth_src=mac_self,
            ip_dst=ipaddr_nbr[1],
            ip_src=ipaddr_nbr[0],
            with_udp_chksum=False,
            ip_id=108,
            ip_ttl=63,
            udp_payload=payload)

        exp_mod_pkt = exp_mod_packet(pkt, self.int_v2, 4, 4, 80)

        try:
            send_packet(self, swports[0], str(pkt))
            verify_packet(self, exp_pkt, swports[1])
            verify_no_other_packets(self)
            print "pass 1st packet w/o ACL drop"

            for port in [port_list[0], port_list[1]]:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=acl_table_id))
                self.client.sai_thrift_set_port_attribute(port, attr)

            send_packet(self, swports[0], str(pkt))
            verify_no_other_packets(self, timeout=1)
            print "pass 2nd packet w/ ACL drop"

            sai_thrift_set_dtel_attribute(self.client, dtel, SAI_DTEL_ATTR_DROP_REPORT_ENABLE, True)

            send_packet(self, swports[0], str(pkt))
            verify_dtel_packet(
                self, exp_mod_pkt, swports[report_ports[0]])
            verify_no_other_packets(self, timeout=1)
            print "pass 3rd packet w/ drop report"

            '''
            event = sai_mgr.create_dtel_event(
                SAI_DTEL_EVENT_TYPE_DROP_REPORT, 5);
            exp_mod_pkt[IP].tos = 5<<2
            send_packet(self, swports[0], str(pkt))
            verify_dtel_packet(
                self, exp_mod_pkt, swports[report_ports[0]])
            verify_no_other_packets(self, timeout=1)
            print "pass 4th packet w/ drop report DSCP 5"

            event.dscp_value = 9
            exp_mod_pkt[IP].tos = 9<<2
            send_packet(self, swports[0], str(pkt))
            verify_dtel_packet(
                self, exp_mod_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "pass 5th packet w/ drop report DSCP 9"
            '''

        finally:
            ### Cleanup
            for port in [port_list[0], port_list[1]]:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID))
                self.client.sai_thrift_set_port_attribute(port, attr)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            self.client.sai_thrift_remove_acl_entry(dtel_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(dtel_acl_table_id)
            self.client.sai_thrift_remove_dtel_report_session(dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)

@group('dtel')
class MalformedMoDTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print "Test DTel reports due to malformed packet drops in the ingress pipeline"

        dp = DataPath()
        configure_datapath(self, dp)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(self.client, switch_id=SID, drop_report_enable=True)

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        # create DTEL watchlist
        dtel_acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=[SAI_ACL_BIND_POINT_TYPE_SWITCH],
            ip_src=ipaddr_nbr[0],
            ip_dst=ipaddr_nbr[1],
            action=SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE)

        # create DTEL watchlist entry
        dtel_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=dtel_acl_table_id,
            entry_priority=10,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            ip_dst=ipaddr_nbr[1],
            ip_dst_mask='255.255.255.0',
            dtel_drop_report_enable=True,
            dtel_report_all_packets=True)

        # Bad checksum
        payload = '@!#?'
        pkt = simple_udp_packet(
            eth_dst=mac_self,
            eth_src=mac_nbr[0],
            ip_dst=ipaddr_nbr[1],
            ip_src=ipaddr_nbr[0],
            ip_id=108,
            ip_ttl=63,
            with_udp_chksum=False,
            udp_payload=payload)
        pkt['IP'].chksum = 0

        exp_mod_pkt = exp_mod_packet(pkt, self.int_v2, 4, 4, 31)

        try:
            send_packet(self, swports[0], str(pkt))
            verify_dtel_packet(
                self, exp_mod_pkt, swports[report_ports[0]])
            verify_no_other_packets(self, timeout=1)
            print "pass TTL 0 packet with drop report"

        finally:
            ### Cleanup
            self.client.sai_thrift_remove_acl_entry(dtel_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(dtel_acl_table_id)
            self.client.sai_thrift_remove_dtel_report_session(dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)

@disabled
class INGRESS_DROP_REPORT_Stful_Test(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print "Test stateful suppression of DTel drop reports"

        # create SAI manager
        sai_mgr = SAIManager(self, params)

        if (self.client.sai_thrift_is_feature_enable(SWITCH_FEATURE_INT_V2) != 0):
            self.int_v2 = True
        else:
            self.int_v2 = False
        bind_mirror_on_drop_pkt(self.int_v2)

        # create report session according to params
        sai_mgr.create_dtel_report_session()

        drop_watchlist = sai_mgr.create_dtel_watchlist('Drop')

        drop_watchlist_entry = sai_mgr.create_dtel_watchlist_entry(
            watchlist=drop_watchlist,
            priority=10,
            ip_src=params.ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            ip_dst=params.ipaddr_nbr[1],
            ip_dst_mask='255.255.255.0',
            dtel_drop_report_enable=True,
            dtel_report_all=True)

        payload = '@!#?'
        pkt = simple_udp_packet(
            eth_dst=params.mac_self,
            eth_src=params.mac_nbr[0],
            ip_dst=params.ipaddr_nbr[1],
            ip_src=params.ipaddr_nbr[0],
            ip_id=108,
            ip_ttl=64,
            with_udp_chksum=False,
            udp_payload=payload)

        exp_pkt = simple_udp_packet(
            eth_dst=params.mac_nbr[1],
            eth_src=params.mac_self,
            ip_dst=params.ipaddr_nbr[1],
            ip_src=params.ipaddr_nbr[0],
            with_udp_chksum=False,
            ip_id=108,
            ip_ttl=63,
            udp_payload=payload)

        exp_mod_pkt = exp_mod_packet(pkt, self.int_v2, 4, 4, 80)

        try:
            send_packet(self, swports[0], str(pkt))
            verify_packet(self, exp_pkt, swports[1])
            verify_no_other_packets(self)
            print "pass 1st packet w/o ACL drop"

            action_list = [SAI_ACL_ENTRY_ATTR_ACTION_PACKET_ACTION]
            packet_action = SAI_PACKET_ACTION_DROP
            in_ports = [sai_mgr.ports[0], sai_mgr.ports[1]]
            ip_src = params.ipaddr_nbr[0]
            ip_src_mask = "255.255.255.0"

            'Create ACL table'
            attr_list = []
            attr_value = sai_thrift_attribute_value_t(s32=SAI_ACL_STAGE_INGRESS)
            attr = sai_thrift_attribute_t(
                id=SAI_ACL_TABLE_ATTR_ACL_STAGE, value=attr_value)
            attr_list.append(attr)

            bp_point = [SAI_ACL_BIND_POINT_TYPE_PORT]
            attr_value = sai_thrift_attribute_value_t(
                s32list=sai_thrift_s32_list_t(s32list=bp_point, count=1))
            attr = sai_thrift_attribute_t(
                id=SAI_ACL_TABLE_GROUP_ATTR_ACL_BIND_POINT_TYPE_LIST,
                value=attr_value)
            attr_list.append(attr)

            attr_value = sai_thrift_attribute_value_t(booldata=1)
            attr = sai_thrift_attribute_t(
                id=SAI_ACL_TABLE_ATTR_FIELD_SRC_IP, value=attr_value)
            attr_list.append(attr)

            acl_table_id = self.client.sai_thrift_create_acl_table(attr_list)

            'Create ACL entry'
            attr_list = []
            attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
            attr = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_TABLE_ID, value=attr_value)
            attr_list.append(attr)

            attr_value = sai_thrift_attribute_value_t(u32=10)
            attr = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_PRIORITY, value=attr_value)
            attr_list.append(attr)

            attr_value = sai_thrift_attribute_value_t(
                aclfield=sai_thrift_acl_field_data_t(
                    data=sai_thrift_acl_data_t(ip4=ip_src),
                    mask=sai_thrift_acl_mask_t(ip4=ip_src_mask)))
            attr = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_FIELD_SRC_IP, value=attr_value)
            attr_list.append(attr)

            attr_value = sai_thrift_attribute_value_t(
                aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                                                     s32=packet_action)))
            attr = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_ACTION_PACKET_ACTION,
                value=attr_value)
            attr_list.append(attr)

            acl_entry_id = self.client.sai_thrift_create_acl_entry(attr_list)

            for port in in_ports:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=acl_table_id))
                self.client.sai_thrift_set_port_attribute(port, attr)

            send_packet(self, swports[0], str(pkt))
            verify_no_other_packets(self, timeout=1)

            print "pass 2nd packet w/ ACL drop"

            sai_mgr.switch.dtel_drop_report_enable = True

            send_packet(self, swports[0], str(pkt))
            #receive_print_packet(
            #    self, swports[params.report_ports[0]], exp_mod_pkt, True)
            verify_dtel_packet(
                self, exp_mod_pkt, swports[params.report_ports[0]])
            verify_no_other_packets(self, timeout=1)
            print "pass 3rd packet w/ drop report"

            send_packet(self, swports[0], str(pkt))
            verify_dtel_packet(
                self, exp_mod_pkt, swports[params.report_ports[0]])
            verify_no_other_packets(self, timeout=1)
            print "pass 4th packet w/ drop report, no drop suppression"

            # Enable drop report suppression
            drop_watchlist_entry.dtel_report_all = False

            print "Clear bloom filters. Can take up to %d secs." % min_sleeptime
            sai_mgr.switch.dtel_flow_state_clear_cycle = reset_cycle
            time.sleep(min_sleeptime)

            print "Disable bloom filter clearing."
            sai_mgr.switch.dtel_flow_state_clear_cycle = 0
            time.sleep(2*reset_cycle)

            send_packet(self, swports[0], str(pkt))
            verify_dtel_packet(
                self, exp_mod_pkt, swports[params.report_ports[0]])
            verify_no_other_packets(self)
            print "pass 5th packet after enabling drop report suppression"

            send_packet(self, swports[0], str(pkt))
            verify_no_other_packets(self, timeout=2)
            print "pass 6th packet w/o drop report since it is suppressed"

        finally:
            ### Cleanup
            for port in in_ports:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID))
                self.client.sai_thrift_set_port_attribute(port, attr)
            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            sai_mgr.switch.dtel_drop_report_enable = False
            sai_mgr.cleanup()

@group('dtel')
class SAI_PostcardTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Test postcard"

        dp = DataPath()
        configure_datapath(self, dp)
        bind_postcard_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(self.client, switch_id=SID, postcard_enable=True)

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        # create DTEL watchlist
        dtel_acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=[SAI_ACL_BIND_POINT_TYPE_SWITCH],
            ip_src=ipaddr_nbr[0],
            ip_dst=ipaddr_nbr[1],
            action=SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP)

        u32range = sai_thrift_range_t(min=900, max=1100)
        acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, u32range)
        range_list = [acl_range_id]

        # create DTEL watchlist entry
        dtel_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=dtel_acl_table_id,
            entry_priority=10,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            ip_dst=ipaddr_nbr[1],
            ip_dst_mask='255.255.255.0',
            range_list=range_list,
            dtel_postcard_enable=True,
            dtel_report_all_packets=True)

        # run test
        try:
            pkt_in = simple_tcp_packet(
                eth_dst=mac_self,
                eth_src=mac_nbr[0],
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=1001,
                ip_id=105,
                ip_ttl=64,
                tcp_flags=None)

            exp_pkt_out = simple_tcp_packet(
                eth_dst=mac_nbr[1],
                eth_src=mac_self,
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=1001,
                ip_id=105,
                tcp_flags=None,
                ip_ttl=63)

            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   4, 5, 1, f=1)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            # verify potcard packet
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for the 1st pkt with sport 1001."

            pkt_in = simple_tcp_packet(
                eth_dst=mac_self,
                eth_src=mac_nbr[0],
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=5005,
                ip_id=105,
                ip_ttl=64,
                tcp_flags=None)

            exp_pkt_out = simple_tcp_packet(
                eth_dst=mac_nbr[1],
                eth_src=mac_self,
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=5005,
                ip_id=105,
                tcp_flags=None,
                ip_ttl=63)

            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   4, 5, 1, f=1)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            verify_no_other_packets(self)
            print "Passed for the 2nd pkt with sport 5005."

            self.client.sai_thrift_remove_acl_entry(dtel_acl_entry_id)
            # send the same test packet through different port
            send_packet(self, swports[2], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            verify_no_other_packets(self)
            print "Passed for watchlist_delete api"

            # create DTEL watchlist entry
            dtel_acl_entry_id = sai_thrift_create_acl_entry(self.client,
                acl_table_id=dtel_acl_table_id,
                entry_priority=10,
                ip_src=ipaddr_nbr[0],
                ip_src_mask='255.255.255.0',
                ip_dst=ipaddr_nbr[1],
                ip_dst_mask='255.255.255.0',
                src_l4_port=5005,
                dtel_postcard_enable=True,
                dtel_report_all_packets=True)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            # verify potcard packet
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for the 3rd pkt with sport 5005."

            # TODO(BFN)
            #new_SID = SID + 1
            #sai_thrift_set_dtel_attribute(self.client, dtel, SAI_DTEL_ATTR_SWITCH_ID, new_SID)

            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   4, 5, 1, f=1)
            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            # verify potcard packet
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for the 4th pkt with sport 5005 and modified switch id."

        # cleanup
        finally:
            split_postcard_pkt(self.int_v2)
            ### Cleanup
            for port in [port_list[0], port_list[1]]:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID))
                self.client.sai_thrift_set_port_attribute(port, attr)

            self.client.sai_thrift_remove_acl_entry(dtel_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(dtel_acl_table_id)
            self.client.sai_thrift_delete_acl_range(acl_range_id)
            self.client.sai_thrift_remove_dtel_report_session(dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)

@group('dtel')
class SAI_DropAndFLowOpTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Test drop report and postcard actions set within the same ACL entry"

        dp = DataPath()
        configure_datapath(self, dp)
        bind_postcard_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(self.client, switch_id=SID, postcard_enable=True, drop_report_enable=True)

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        # create DTEL watchlist
        dtel_acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=[SAI_ACL_BIND_POINT_TYPE_SWITCH],
            ip_src=ipaddr_nbr[0],
            ip_dst=ipaddr_nbr[1],
            action=SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP)

        # create DTEL watchlist entry
        dtel_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=dtel_acl_table_id,
            entry_priority=10,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            ip_dst=ipaddr_nbr[1],
            ip_dst_mask='255.255.255.0',
            dtel_drop_report_enable=True,
            dtel_postcard_enable=True,
            dtel_tail_drop_report_enable=True,
            dtel_report_all_packets=True)

        # Create ACL table
        bp_point = [SAI_ACL_BIND_POINT_TYPE_PORT]
        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=bp_point, ip_src=True, ip_dst=True)

        # Create ACL entry
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=acl_table_id,
            entry_priority=10,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            action=SAI_PACKET_ACTION_DROP)

        # run test
        try:
            pkt_in = simple_tcp_packet(
                eth_dst=mac_self,
                eth_src=mac_nbr[0],
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                ip_id=105,
                ip_ttl=64,
                tcp_flags=None)

            exp_pkt_out = simple_tcp_packet(
                eth_dst=mac_nbr[1],
                eth_src=mac_self,
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                ip_id=105,
                tcp_flags=None,
                ip_ttl=63)

            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   4, 5, 1, f=1)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            # verify potcard packet
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for the flow report packet"

            bind_mirror_on_drop_pkt(self.int_v2)

            for port in [port_list[0], port_list[1]]:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=acl_table_id))
                self.client.sai_thrift_set_port_attribute(port, attr)

            exp_mod_pkt = exp_mod_packet(pkt_in, self.int_v2, 4, 4, 80, f=1)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify potcard packet
            verify_dtel_packet(
                self, exp_mod_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for the mirror on drop test."

            self.client.sai_thrift_set_acl_entry_attribute(
                dtel_acl_entry_id,
                sai_thrift_attribute_t(
                    id=SAI_ACL_ENTRY_ATTR_ACTION_ACL_DTEL_FLOW_OP,
                    value=sai_thrift_attribute_value_t(
                        aclaction=sai_thrift_acl_action_data_t(
                            enable=True,
                            parameter=sai_thrift_acl_parameter_t(
                                s32=SAI_ACL_DTEL_FLOW_OP_NOP)))))

            exp_mod_pkt = exp_mod_packet(pkt_in, self.int_v2, 4, 4, 80, f=0)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify potcard packet
            verify_dtel_packet(
                self, exp_mod_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for the mirror on drop only test."

            self.client.sai_thrift_set_acl_entry_attribute(
                dtel_acl_entry_id,
                sai_thrift_attribute_t(
                    id=SAI_ACL_ENTRY_ATTR_ACTION_DTEL_DROP_REPORT_ENABLE,
                    value=sai_thrift_attribute_value_t(
                        aclaction=sai_thrift_acl_action_data_t(
                            enable=True,
                            parameter=sai_thrift_acl_parameter_t(
                                booldata=False)))))

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            verify_no_other_packets(self, timeout=1)
            print "Passed for the mirror on drop disabled test."

        # cleanup
        finally:
            split_mirror_on_drop_pkt(self.int_v2)
            split_postcard_pkt(self.int_v2)
            ### Cleanup
            for port in [port_list[0], port_list[1]]:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID))
                self.client.sai_thrift_set_port_attribute(port, attr)

            self.client.sai_thrift_remove_acl_entry(acl_entry_id)
            self.client.sai_thrift_remove_acl_table(acl_table_id)
            self.client.sai_thrift_remove_acl_entry(dtel_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(dtel_acl_table_id)
            self.client.sai_thrift_remove_dtel_report_session(dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)

@group('dtel')
class SAI_DropAndFLowOpDiffTablesTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print "Test DTel MoD report when the similar entries MoD and Flow reports are installed in different tables"

        dp = DataPath()
        configure_datapath(self, dp)
        bind_mirror_on_drop_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(self.client, switch_id=SID, drop_report_enable=True)

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        # create drop DTEL watchlist
        dtel_drop_acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=[SAI_ACL_BIND_POINT_TYPE_SWITCH],
            ip_src=ipaddr_nbr[0],
            ip_dst=ipaddr_nbr[1],
            action=SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE)

        # create DTEL watchlist entry
        dtel_drop_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=dtel_drop_acl_table_id,
            entry_priority=16564,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.0.0',
            ip_dst=ipaddr_nbr[1],
            ip_dst_mask='255.255.0.0',
            dtel_drop_report_enable=True,
            dtel_report_all_packets=True)

        # create flow DTEL watchlist
        dtel_flow_acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=[SAI_ACL_BIND_POINT_TYPE_SWITCH],
            ip_src=ipaddr_nbr[0],
            ip_dst=ipaddr_nbr[1],
            action=SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP)

        # create DTEL watchlist entry
        dtel_flow_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=dtel_flow_acl_table_id,
            entry_priority=16574,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            ip_dst=ipaddr_nbr[1],
            ip_dst_mask='255.255.255.0',
            dtel_drop_report_enable=True,
            dtel_report_all_packets=True)

        # TTL 0 packet
        payload = '@!#?'
        pkt = simple_udp_packet(
            eth_dst=mac_self,
            eth_src=mac_nbr[0],
            ip_dst=ipaddr_nbr[1],
            ip_src=ipaddr_nbr[0],
            ip_id=108,
            ip_ttl=0,
            with_udp_chksum=False,
            udp_payload=payload)

        exp_mod_pkt = exp_mod_packet(pkt, self.int_v2, 4, 4, 26)

        try:
            # Drop entry has higher priority so so MoD report has to be sent
            send_packet(self, swports[0], str(pkt))
            verify_dtel_packet(
                self, exp_mod_pkt, swports[report_ports[0]])
            verify_no_other_packets(self, timeout=1)
            print "pass TTL 0 packet with drop report"

            pkt2 = pkt.copy()
            pkt2[IP].ttl=64

            exp_pkt2 = simple_udp_packet(
                eth_dst=mac_nbr[1],
                eth_src=mac_self,
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                with_udp_chksum=False,
                ip_id=108,
                ip_ttl=63,
                udp_payload=payload)

            # Now packet is not dropped as TTL is valid but drop ACL entry still
            # has to be hit due to higher priority. No report is expected.
            send_packet(self, swports[0], str(pkt2))
            verify_packet(self, exp_pkt2, swports[1])
            verify_no_other_packets(self)
            print "pass packet with good TTL. No MoD or Flow report"

        finally:
            ### Cleanup
            self.client.sai_thrift_remove_acl_entry(dtel_drop_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(dtel_drop_acl_table_id)
            self.client.sai_thrift_remove_acl_entry(dtel_flow_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(dtel_flow_acl_table_id)
            self.client.sai_thrift_remove_dtel_report_session(dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)

class SAI_QueueReportConfigTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Test queue report"

        dp = DataPath()
        configure_datapath(self, dp)

        port = port_list[16]
        depth_thresh = 10
        latency_thres = 100000
        tail_drop = True
        breach_quota = 1000

        dtel = sai_thrift_create_dtel(self.client, switch_id=SID, queue_report_enable=True)

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        port_attr = sai_thrift_attribute_t(id = SAI_PORT_ATTR_QOS_QUEUE_LIST)
        attr_list = self.client.sai_thrift_get_port_handles_attribute(port,  port_attr);

        queue_handles = attr_list.value.objlist.object_id_list

        try:
            queue_report = sai_thrift_create_dtel_queue_report(self.client, queue_handles[0], depth_thresh, latency_thres, breach_quota, tail_drop)

            assert queue_report != 0, 'queue_report == 0'

        finally:
            ### Cleanup
            self.client.sai_thrift_remove_dtel_queue_report(queue_report)
            self.client.sai_thrift_remove_dtel_report_session(dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)

@group('dtel-ifa')
class SAI_IfaEdgeTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Test DTEL reports and discard when receiving IFA clone packets"

        dp = DataPath()
        configure_datapath(self, dp)
        bind_postcard_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(
            self.client, switch_id=SID, int_endpoint_enable=True,
            int_dscp_value=7, int_dscp_mask=0x3F, int_edge_ports=[port_list[1]])

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        # run test
        try:
            pkt_in = simple_tcp_packet(
                eth_dst=mac_self,
                eth_src=mac_nbr[0],
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=1001,
                ip_id=105,
                ip_ttl=64,
                tcp_flags=None)

            exp_pkt_out = simple_tcp_packet(
                eth_dst=mac_nbr[1],
                eth_src=mac_self,
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=1001,
                ip_id=105,
                tcp_flags=None,
                ip_ttl=63)

            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   4, 5, 1, f=1)

            # send a test packet that does not match IFA DSCP
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            verify_no_other_packets(self)
            print "Passed for 1st pkt not matching IFA DSCP, not dropped or reported"

            # change test packet to match IFA DSCP
            pkt_in[IP].tos = 28  # dscp=7
            exp_pkt_out[IP].tos = 28  # dscp=7
            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   4, 5, 1, f=1)

            # send a test packet that matches IFA DSCP
            send_packet(self, swports[0], str(pkt_in))
            # verify test packet is dropped and postcard packet is received
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for 2nd pkt matching IFA DSCP, dropped and reported"

            # change port 1 to no longer be an edge port
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_SINK_PORT_LIST, [])

            # send a test packet out port that is not an edge port
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            verify_no_other_packets(self)
            print "Passed for 3rd pkt that does not go out an edge port"

            # change ports 0, 1, 3 to be edge ports
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_SINK_PORT_LIST,
                [port_list[0], port_list[1], port_list[3]])

            # disable switch wide IFA reports
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_INT_ENDPOINT_ENABLE, False)

            # send a test packet when IFA reports are disabled
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            verify_no_other_packets(self)
            print "Passed for 4th pkt when IFA reports are disabled"

            # enable switch wide IFA reports
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_INT_ENDPOINT_ENABLE, True)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify postcard packet
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for 5th pkt when edge port and IFA reports re-enabled"

            # change IFA DSCP to single bit match
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_INT_L4_DSCP, [0x20, 0x20])

            # send a test packet that does not match IFA DSCP
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            verify_no_other_packets(self)
            print "Passed for 6th pkt not matching reconfigured IFA DSCP"

            # change test packet to match IFA DSCP
            pkt_in[IP].tos = 152  # dscp=38
            exp_pkt_out[IP].tos = 152  # dscp=38
            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   4, 5, 1, f=1)

            # send a test packet that matches IFA DSCP
            send_packet(self, swports[0], str(pkt_in))
            # verify postcard packet
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for 7th pkt matching reconfigured IFA DSCP"

        # cleanup
        finally:
            split_postcard_pkt(self.int_v2)
            ### Cleanup
            self.client.sai_thrift_remove_dtel_report_session(
                dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)

@group('dtel-ifa')
class SAI_IfaCloneTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Test IFA clone functionality"

        dp = DataPath()
        configure_datapath(self, dp)
        bind_postcard_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(
            self.client, switch_id=SID, int_endpoint_enable=True,
            int_dscp_value=7, int_dscp_mask=0x3F, int_edge_ports=[])

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        # create sample session
        samplepacket_session = sai_thrift_create_samplepacket(self.client,
            rate=1)

        # create DTEL watchlist
        dtel_acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=[SAI_ACL_BIND_POINT_TYPE_PORT],
            ip_src=ipaddr_nbr[0],
            ip_dst=ipaddr_nbr[1],
            action=SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP)

        u32range = sai_thrift_range_t(min=900, max=1100)
        acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, u32range)
        range_list = [acl_range_id]

        # create DTEL watchlist entry
        dtel_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=dtel_acl_table_id,
            entry_priority=10,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            ip_dst=ipaddr_nbr[1],
            ip_dst_mask='255.255.255.0',
            range_list=range_list,
            ingress_samplepacket=samplepacket_session)

        for port in [port_list[0]]:
            attr = sai_thrift_attribute_t(
                id=SAI_PORT_ATTR_INGRESS_ACL,
                value=sai_thrift_attribute_value_t(oid=dtel_acl_table_id))
            self.client.sai_thrift_set_port_attribute(port, attr)

        # run test
        try:
            pkt_in = simple_tcp_packet(
                eth_dst=mac_self,
                eth_src=mac_nbr[0],
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=1001,
                ip_id=105,
                ip_ttl=64,
                tcp_flags=None)

            exp_pkt_out = simple_tcp_packet(
                eth_dst=mac_nbr[1],
                eth_src=mac_self,
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=1001,
                ip_id=105,
                tcp_flags=None,
                ip_ttl=63)

            exp_cloned_pkt_out = exp_pkt_out.copy()
            exp_cloned_pkt_out[IP].tos = 28  # dscp=7

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 1st pkt, not cloned"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_each_packet_on_each_port(self,
                                            [exp_pkt_out, exp_cloned_pkt_out],
                                            [swports[1], swports[1]])
            print "Passed for the 2nd pkt, cloned with full DSCP rewrite"

            # change IFA DSCP to single bit match
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_INT_L4_DSCP, [0x20, 0x20])
            exp_cloned_pkt_out[IP].tos = 128  # dscp=32

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 3rd pkt, not cloned"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_each_packet_on_each_port(self,
                                            [exp_pkt_out, exp_cloned_pkt_out],
                                            [swports[1], swports[1]])
            print "Passed for the 4th pkt, cloned with single bit DSCP rewrite"

            # change sample rate
            sai_thrift_set_samplepacket_attribute(
                self.client,
                samplepacket_session,
                SAI_SAMPLEPACKET_ATTR_SAMPLE_RATE,
                2)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 5th pkt, not cloned after rate change"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 6th pkt, not cloned after rate change"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_each_packet_on_each_port(self,
                                            [exp_pkt_out, exp_cloned_pkt_out],
                                            [swports[1], swports[1]])
            print "Passed for the 7th pkt, cloned after rate change"

            # change sample rate back to 1
            sai_thrift_set_samplepacket_attribute(
                self.client,
                samplepacket_session,
                SAI_SAMPLEPACKET_ATTR_SAMPLE_RATE,
                1)

            # change port 1 to be an edge port
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_SINK_PORT_LIST,
                [port_list[1]])

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 8th pkt, not cloned"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 9th pkt, not cloned due to edge port"

            # change port 1 to no longer be an edge port
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_SINK_PORT_LIST,
                [])

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 10th pkt, not cloned"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_each_packet_on_each_port(self,
                                            [exp_pkt_out, exp_cloned_pkt_out],
                                            [swports[1], swports[1]])
            print "Passed for the 11th pkt, cloned after no longer an edge port"

            # send a test packet on port 2
            send_packet(self, swports[2], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 12th pkt, not cloned"

            # send a test packet on port 2
            send_packet(self, swports[2], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 13th pkt, not cloned due to non-matching port"

            # disable switch wide IFA reports
            sai_thrift_set_dtel_attribute(
                self.client, dtel, SAI_DTEL_ATTR_INT_ENDPOINT_ENABLE, False)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 14th pkt, not cloned"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 15th pkt, not cloned due to IFA disable"

        # cleanup
        finally:
            split_postcard_pkt(self.int_v2)
            ### Cleanup
            for port in [port_list[0], port_list[1]]:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID))
                self.client.sai_thrift_set_port_attribute(port, attr)

            self.client.sai_thrift_remove_acl_entry(dtel_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(dtel_acl_table_id)
            self.client.sai_thrift_delete_acl_range(acl_range_id)
            self.client.sai_thrift_remove_dtel_report_session(dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)

# On the model this test leaves bfilters unclean, but since the last packet
# is received on a different ingress port, the test can be rerun and still pass
@group('dtel-ifa')
class SAI_FlowAndIfaCloneTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "Test IFA clone functionality with DTEL flow report generation"

        dp = DataPath()
        configure_datapath(self, dp)
        bind_postcard_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(
            self.client, switch_id=SID, int_endpoint_enable=True,
            postcard_enable=True, int_dscp_value=1, int_dscp_mask=0x1,
            int_edge_ports=[])

        # create report session according to params
        dtel_report_session = sai_thrift_create_dtel_report_session(self.client,
            report_src, report_dst, dp.vr_id,
            report_truncate_size, report_udp_port)

        # create sample session
        samplepacket_session = sai_thrift_create_samplepacket(self.client,
            rate=1)

        # create DTEL watchlist
        dtel_acl_table_id = sai_thrift_create_acl_table(self.client,
            table_bind_point_list=[SAI_ACL_BIND_POINT_TYPE_SWITCH],
            ip_src=ipaddr_nbr[0],
            ip_dst=ipaddr_nbr[1],
            action=SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP)

        u32range = sai_thrift_range_t(min=900, max=1100)
        acl_range_id = sai_thrift_create_acl_range(
            self.client, SAI_ACL_RANGE_TYPE_L4_SRC_PORT_RANGE, u32range)
        range_list = [acl_range_id]

        # create DTEL watchlist entry
        dtel_acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id=dtel_acl_table_id,
            entry_priority=10,
            ip_src=ipaddr_nbr[0],
            ip_src_mask='255.255.255.0',
            ip_dst=ipaddr_nbr[1],
            ip_dst_mask='255.255.255.0',
            range_list=range_list,
            dtel_postcard_enable=True,
            dtel_report_all_packets=False,
            ingress_samplepacket=samplepacket_session)

        # run test
        try:
            pkt_in = simple_tcp_packet(
                eth_dst=mac_self,
                eth_src=mac_nbr[0],
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=1099,
                ip_id=105,
                ip_ttl=64,
                tcp_flags=None)

            exp_pkt_out = simple_tcp_packet(
                eth_dst=mac_nbr[1],
                eth_src=mac_self,
                ip_dst=ipaddr_nbr[1],
                ip_src=ipaddr_nbr[0],
                tcp_sport=1099,
                ip_id=105,
                tcp_flags=None,
                ip_ttl=63)

            exp_cloned_pkt_out = exp_pkt_out.copy()
            exp_cloned_pkt_out[IP].tos = 4  # dscp=1

            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   4, 5, 1, f=1)

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            # verify postcard packet
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for the 1st pkt, not cloned"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_each_packet_on_each_port(self,
                                            [exp_pkt_out, exp_cloned_pkt_out],
                                            [swports[1], swports[1]])
            print "Passed for the 2nd pkt, cloned"

            # send a test packet
            send_packet(self, swports[0], str(pkt_in))
            # verify packet out
            verify_packets(self, exp_pkt_out, [swports[1]])
            print "Passed for the 3rd pkt, not cloned"

            # send a test packet on different ingress port
            exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                                   5, 5, 1, f=1)
            send_packet(self, swports[1], str(pkt_in))
            # verify packet out
            verify_packet(self, exp_pkt_out, swports[1])
            # verify postcard packet
            verify_postcard_packet(
                self, exp_postcard_pkt, swports[report_ports[0]])
            verify_no_other_packets(self)
            print "Passed for the 4th pkt, DTEL report overrides clone"

        # cleanup
        finally:
            split_postcard_pkt(self.int_v2)
            ### Cleanup
            for port in [port_list[0], port_list[1]]:
                attr = sai_thrift_attribute_t(
                    id=SAI_PORT_ATTR_INGRESS_ACL,
                    value=sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID))
                self.client.sai_thrift_set_port_attribute(port, attr)

            self.client.sai_thrift_remove_acl_entry(dtel_acl_entry_id)
            self.client.sai_thrift_remove_acl_table(dtel_acl_table_id)
            self.client.sai_thrift_delete_acl_range(acl_range_id)
            self.client.sai_thrift_remove_dtel_report_session(dtel_report_session)
            remove_datapath(self.client, dp)
            self.client.sai_thrift_remove_dtel(dtel)
