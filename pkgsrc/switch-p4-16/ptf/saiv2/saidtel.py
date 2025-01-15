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
Thrift SAI interface DTEL tests
"""

from bf_switcht_api_thrift.model_headers import *

from sai_base_test import *

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))

from common.utils import *  # noqa pylint: disable=wrong-import-position
from common.dtel_utils import *  # noqa pylint: disable=wrong-import-position

SID = 0x11111111
MAC_SELF = ROUTER_MAC
COLLECTOR_MAC = '00:11:22:33:44:57'
REPORT_SRC = '4.4.4.1'
REPORT_DST = ['4.4.4.3']
IPADDR_INF = ['2.2.0.1', '1.1.0.1', '172.16.0.4']
IPADDR_NBR = ['2.2.0.200', '1.1.0.100', '172.16.0.1']


def exp_mod_packet(pkt, int_v2, fields):
    '''
    Creates expected MoD packet based on given fields

    Args:
        pkt (packet) : inner frame packet
        int_v2 (bool) : if feature INT_V2 is enabled
        fields (dict) : dictionary with fields (ingress_port, hw_id,
                        drop_reason and flow)

    Return:
        packet: exp_mod_pkt with expected MoD packet to be received
    '''
    ingress_port = fields['ingress_port']
    hw_id = fields['hw_id']
    drop_reason = fields['drop_reason']
    if 'flow' in fields:
        flow = fields['flow']
    else:
        flow = 0

    if int_v2:
        exp_mod_pkt = ipv4_dtel_v2_pkt(
            eth_dst=COLLECTOR_MAC,
            eth_src=MAC_SELF,
            ip_src=REPORT_SRC,
            ip_dst=REPORT_DST[0],
            ip_id=0,
            ip_ttl=63,
            dropped=1,
            congested_queue=0,
            path_tracking_flow=flow,
            hw_id=hw_id,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=INVALID_PORT_ID,
            ingress_tstamp=0,
            queue_id=0,
            drop_reason=drop_reason,
            inner_frame=pkt)

    else:
        exp_mod_inner = mod_report(
            packet=pkt,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=INVALID_PORT_ID,
            queue_id=0,
            drop_reason=drop_reason)

        exp_mod_pkt = ipv4_dtel_pkt(
            eth_dst=COLLECTOR_MAC,
            eth_src=MAC_SELF,
            ip_src=REPORT_SRC,
            ip_dst=REPORT_DST[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_MOD,
            dropped=1,
            congested_queue=0,
            path_tracking_flow=flow,
            hw_id=hw_id,
            inner_frame=exp_mod_inner)

    return exp_mod_pkt


def exp_egress_mod_packet(pkt, int_v2, fields):
    '''
    Creates expected egress MoD packet based on given fields

    Args:
        pkt (packet) : inner frame packet
        int_v2 (bool) : if feature INT_V2 is enabled
        fields (dict) : dictionary with fields (ingress_port, egress_port,
                        hw_id, drop_reason, drop, queue and flow)

    Return:
        packet: exp_mod_pkt with expected MoD packet to be received
    '''
    ingress_port = fields['ingress_port']
    egress_port = fields['egress_port']
    hw_id = fields['hw_id']
    drop_reason = fields['drop_reason']
    if 'drop' in fields:
        drop = fields['drop']
    else:
        drop = 1

    if 'queue' in fields:
        queue = fields['queue']
    else:
        queue = 0

    if 'flow' in fields:
        flow = fields['flow']
    else:
        flow = 0

    if int_v2:
        exp_mod_pkt = ipv4_dtel_v2_pkt(
            eth_dst=COLLECTOR_MAC,
            eth_src=MAC_SELF,
            ip_src=REPORT_SRC,
            ip_dst=REPORT_DST[0],
            ip_id=0,
            ip_ttl=63,
            dropped=drop,
            congested_queue=queue,
            path_tracking_flow=flow,
            hw_id=hw_id,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=egress_port,
            ingress_tstamp=0,
            queue_id=0,
            drop_reason=drop_reason,
            inner_frame=pkt)

    else:
        exp_mod_inner = mod_report(
            packet=pkt,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=egress_port,
            queue_id=0,
            drop_reason=drop_reason)

        exp_mod_pkt = ipv4_dtel_pkt(
            eth_dst=COLLECTOR_MAC,
            eth_src=MAC_SELF,
            ip_src=REPORT_SRC,
            ip_dst=REPORT_DST[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_MOD,
            dropped=drop,
            congested_queue=queue,
            path_tracking_flow=flow,
            hw_id=hw_id,
            inner_frame=exp_mod_inner)

    return exp_mod_pkt


def exp_postcard_packet(pkt, int_v2, fields):
    '''
    Creates postcard packet based on given fields

    Args:
        pkt (packet) : inner frame packet
        int_v2 (bool) : if feature INT_V2 is enabled
        fields (dict) : dictionary with fields (ingress_port, egress_port,
                        hw_id, drop, queue and flow)

    Return:
        packet: exp_e2e_pkt with expected MoD packet to be received
    '''
    ingress_port = fields['ingress_port']
    egress_port = fields['egress_port']
    hw_id = fields['hw_id']
    if 'drop' in fields:
        drop = fields['drop']
    else:
        drop = 0

    if 'queue' in fields:
        queue = fields['queue']
    else:
        queue = 0

    if 'flow' in fields:
        flow = fields['flow']
    else:
        flow = 0

    if int_v2:
        exp_e2e_pkt = ipv4_dtel_v2_pkt(
            eth_dst=COLLECTOR_MAC,
            eth_src=MAC_SELF,
            ip_src=REPORT_SRC,
            ip_dst=REPORT_DST[0],
            ip_id=0,
            ip_ttl=63,
            dropped=drop,
            congested_queue=queue,
            path_tracking_flow=flow,
            hw_id=hw_id,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=egress_port,
            queue_id=0,
            queue_depth=0,
            ingress_tstamp=0,
            egress_tstamp=0,
            inner_frame=pkt)

    else:

        exp_pc_inner = postcard_report(
            packet=pkt,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=egress_port,
            queue_id=0,
            queue_depth=0,
            egress_tstamp=0)

        exp_e2e_pkt = ipv4_dtel_pkt(
            eth_dst=COLLECTOR_MAC,
            eth_src=MAC_SELF,
            ip_src=REPORT_SRC,
            ip_dst=REPORT_DST[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL,
            dropped=drop,
            congested_queue=queue,
            path_tracking_flow=flow,
            hw_id=hw_id,
            inner_frame=exp_pc_inner)

    return exp_e2e_pkt


def exp_dod_packet(pkt, int_v2, fields):
    '''
    Creates expected DoD packet based on given fields

    Args:
        pkt (packet) : inner frame packet
        int_v2 (bool) : if feature INT_V2 is enabled
        fields (dict) : dictionary with fields (ingress_port, hw_id,
                        queue_report, drop_reason and flow)

    Return:
        packet: exp_dod_pkt with expected DoD packet to be received
    '''
    ingress_port = fields['ingress_port']
    egress_port = fields['egress_port']
    hw_id = fields['hw_id']
    if 'drop' in fields:
        drop = fields['drop']
    else:
        drop = 1

    if 'queue' in fields:
        queue = fields['queue']
    else:
        queue = 0

    if 'flow' in fields:
        flow = fields['flow']
    else:
        flow = 0

    # SWITCH_DROP_REASON_TRAFFIC_MANAGER 71
    if int_v2:
        exp_dod_pkt = ipv4_dtel_v2_pkt(
            eth_dst=COLLECTOR_MAC,
            eth_src=MAC_SELF,
            ip_src=REPORT_SRC,
            ip_dst=REPORT_DST[0],
            ip_id=0,
            ip_ttl=63,
            dropped=drop,
            congested_queue=queue,
            path_tracking_flow=flow,
            hw_id=hw_id,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=egress_port,
            ingress_tstamp=0,
            queue_id=0,
            drop_reason=71,
            inner_frame=pkt)
    else:
        exp_dod_inner = mod_report(
            packet=pkt,
            switch_id=SID,
            ingress_port=ingress_port,
            egress_port=egress_port,
            queue_id=0,
            drop_reason=71)

        exp_dod_pkt = ipv4_dtel_pkt(
            eth_dst=COLLECTOR_MAC,
            eth_src=MAC_SELF,
            ip_src=REPORT_SRC,
            ip_dst=REPORT_DST[0],
            ip_id=0,
            ip_ttl=63,
            next_proto=DTEL_REPORT_NEXT_PROTO_MOD,
            dropped=drop,
            congested_queue=queue,
            path_tracking_flow=flow,
            hw_id=hw_id,
            inner_frame=exp_dod_inner)

    return exp_dod_pkt


class DtelAttrTest(SaiHelper):
    """
    Test DST ip list set get
    """

    def setUp(self):
        SaiHelper.setUp(self)

        # test dtel report session creation
        dtel_dst_addrs = [
            sai_ipaddress('10.10.10.1'),
            sai_ipaddress('20.20.20.1'),
            sai_ipaddress('30.30.30.1')
        ]
        self.dtel_dst_addrs_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_src_ip = sai_ipaddress('1.1.1.1')
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            dst_ip_list=self.dtel_dst_addrs_list,
            src_ip=self.dtel_src_ip,
            udp_dst_port=9000,
            virtual_router_id=self.default_vrf)

        attr = sai_thrift_get_switch_attribute(
            self.client, max_acl_action_count=True)
        max_acl_action_count = attr['max_acl_action_count']
        s32 = sai_thrift_s32_list_t(int32list=[], count=max_acl_action_count)
        cap = sai_thrift_acl_capability_t(action_list=s32)
        attr = sai_thrift_get_switch_attribute(
            self.client, acl_stage_ingress=cap)
        self.acl_stage_ingress = \
            attr['acl_stage_ingress'].action_list.int32list
        self.assertTrue(len(self.acl_stage_ingress) != 0)
        action_type = SAI_ACL_ACTION_TYPE_DTEL_REPORT_ALL_PACKETS
        self.assertTrue(action_type in self.acl_stage_ingress)
        action_type = SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP
        self.assertTrue(action_type in self.acl_stage_ingress)
        action_type = SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE
        self.assertTrue(action_type in self.acl_stage_ingress)
        action_type = SAI_ACL_ACTION_TYPE_DTEL_TAIL_DROP_REPORT_ENABLE
        self.assertTrue(action_type in self.acl_stage_ingress)

    def runTest(self):
        try:
            attr = sai_thrift_get_dtel_report_session_attribute(
                self.client,
                self.dtel_report_session,
                dst_ip_list=sai_thrift_ip_address_list_t(
                    addresslist=[], count=5))
            self.assertEqual(attr['dst_ip_list'].count,
                             self.dtel_dst_addrs_list.count)
            for i in range(0, self.dtel_dst_addrs_list.count):
                self.assertEqual(
                    attr['dst_ip_list'].addresslist[i].addr_family,
                    self.dtel_dst_addrs_list.addresslist[i].addr_family)
                self.assertEqual(
                    attr['dst_ip_list'].addresslist[i].addr.ip4,
                    self.dtel_dst_addrs_list.addresslist[i].addr.ip4)
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        SaiHelper.tearDown(self)


class DtelBaseTest(SaiHelper):
    """ Basic Dtel test class """

    def setUp(self):
        super(DtelBaseTest, self).setUp()

        if test_param_get('int_v2') == 'enabled':
            self.int_v2 = True
            print("Feature int_v2 enabled")
        else:
            self.int_v2 = False
            print("Feature int_v2 disabled")

        if test_param_get('arch') == 'tofino':
            # ingress port mapping
            self.port24_fp = 28
            self.port25_fp = 29
            self.port26_fp = 30
            self.port28_fp = 32
            self.port30_fp = 34
            self.port31_fp = 35
            self.cpu_port0_fp = 502
            self.architecture = 'tofino'
        elif test_param_get('arch') == 'tofino2':
            # ingress port mapping
            self.port24_fp = 32
            self.port25_fp = 33
            self.port26_fp = 34
            self.port28_fp = 36
            self.port30_fp = 38
            self.port31_fp = 39
            self.cpu_port0_fp = 502
            self.architecture = 'tofino2'

        self.ipv6_1 = '1234:5678:9abc:def0:4422:1133:5577:99aa'
        self.ipv6_2 = '1234:5678:9abc:def0:4422:1133:5577:99ab'
        self.mac_nbr = ['00:11:22:33:44:54', '00:11:22:33:44:55',
                        '00:11:22:33:44:56']
        self.report_udp_port = UDP_PORT_DTEL_REPORT
        self.report_truncate_size = 256

        self.vrf = sai_thrift_create_virtual_router(
            self.client, admin_v4_state=True, admin_v6_state=True)

        self.port24_rif = sai_thrift_create_router_interface(
            self.client, type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.vrf, port_id=self.port24,
            admin_v4_state=True, admin_v6_state=True)
        self.port25_rif = sai_thrift_create_router_interface(
            self.client, type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.vrf, port_id=self.port25,
            admin_v4_state=True, admin_v6_state=True)
        self.port26_rif = sai_thrift_create_router_interface(
            self.client, type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.vrf, port_id=self.port26,
            admin_v4_state=True, admin_v6_state=True)
        self.port27_rif = sai_thrift_create_router_interface(
            self.client, type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.vrf, port_id=self.port27,
            admin_v4_state=True, admin_v6_state=True)

        self.nbr10 = sai_thrift_neighbor_entry_t(
            rif_id=self.port24_rif,
            ip_address=sai_ipaddress(IPADDR_INF[0]))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr10, dst_mac_address=self.mac_nbr[0])
        self.nhop10 = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(IPADDR_INF[0]),
            router_interface_id=self.port24_rif, type=SAI_NEXT_HOP_TYPE_IP)
        self.route10 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix(IPADDR_NBR[0] + '/32'))
        sai_thrift_create_route_entry(
            self.client, self.route10, next_hop_id=self.nhop10)
        self.route10_ip6 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('2000::1/128'))
        sai_thrift_create_route_entry(
            self.client, self.route10_ip6, next_hop_id=self.nhop10)

        self.nbr11 = sai_thrift_neighbor_entry_t(
            rif_id=self.port25_rif,
            ip_address=sai_ipaddress(IPADDR_INF[1]))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr11, dst_mac_address=self.mac_nbr[1])
        self.nhop11 = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(IPADDR_INF[1]),
            router_interface_id=self.port25_rif, type=SAI_NEXT_HOP_TYPE_IP)
        self.route11 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix(IPADDR_NBR[1] + '/32'))
        sai_thrift_create_route_entry(
            self.client, self.route11, next_hop_id=self.nhop11)
        self.route11_ip6 = sai_thrift_route_entry_t(
            vr_id=self.vrf,
            destination=sai_ipprefix(self.ipv6_1 + "/128"))
        sai_thrift_create_route_entry(
            self.client, self.route11_ip6, next_hop_id=self.nhop11)

        self.nbr12 = sai_thrift_neighbor_entry_t(
            rif_id=self.port26_rif,
            ip_address=sai_ipaddress(IPADDR_INF[2]))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr12, dst_mac_address=self.mac_nbr[2])
        self.nhop12 = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(IPADDR_INF[2]),
            router_interface_id=self.port26_rif, type=SAI_NEXT_HOP_TYPE_IP)
        self.route12 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix(IPADDR_NBR[2] + '/32'))
        sai_thrift_create_route_entry(
            self.client, self.route12, next_hop_id=self.nhop12)
        self.route12_ip6 = sai_thrift_route_entry_t(
            vr_id=self.vrf,
            destination=sai_ipprefix(self.ipv6_2 + "/128"))
        sai_thrift_create_route_entry(
            self.client, self.route12_ip6, next_hop_id=self.nhop12)

        # configure dtel routes
        self.nbr13 = sai_thrift_neighbor_entry_t(
            rif_id=self.port27_rif, ip_address=sai_ipaddress('11.11.11.1'))
        sai_thrift_create_neighbor_entry(
            self.client, self.nbr13, dst_mac_address=COLLECTOR_MAC)
        self.nhop13 = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress('11.11.11.1'),
            router_interface_id=self.port27_rif, type=SAI_NEXT_HOP_TYPE_IP)
        self.route13 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix(REPORT_DST[0] + '/32'))
        sai_thrift_create_route_entry(
            self.client, self.route13, next_hop_id=self.nhop13)

    def tearDown(self):
        sai_thrift_remove_route_entry(self.client, self.route13)
        sai_thrift_remove_next_hop(self.client, self.nhop13)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr13)

        sai_thrift_remove_route_entry(self.client, self.route12_ip6)
        sai_thrift_remove_route_entry(self.client, self.route11_ip6)
        sai_thrift_remove_route_entry(self.client, self.route10_ip6)

        sai_thrift_remove_route_entry(self.client, self.route12)
        sai_thrift_remove_next_hop(self.client, self.nhop12)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr12)

        sai_thrift_remove_route_entry(self.client, self.route11)
        sai_thrift_remove_next_hop(self.client, self.nhop11)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr11)

        sai_thrift_remove_route_entry(self.client, self.route10)
        sai_thrift_remove_next_hop(self.client, self.nhop10)
        sai_thrift_remove_neighbor_entry(self.client, self.nbr10)

        sai_thrift_remove_router_interface(self.client, self.port24_rif)
        sai_thrift_remove_router_interface(self.client, self.port25_rif)
        sai_thrift_remove_router_interface(self.client, self.port26_rif)
        sai_thrift_remove_router_interface(self.client, self.port27_rif)

        sai_thrift_remove_virtual_router(self.client, self.vrf)
        super(DtelBaseTest, self).tearDown()


class MoDReportTest(DtelBaseTest):
    """ MoD Report test class """

    def setUp(self):
        super(MoDReportTest, self).setUp()

        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            drop_report_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

    def runTest(self):
        try:
            self.modNoDropTest()
            self.modHostifReasonCodeDropTest()
            self.ingressAclDropReportTest()
            self.egressAclDropReportTest()
            self.dtelWatchlistTest()
            self.modPortBindingWatchlistTest()
            self.modIPv4MalformedPacketsTest()
            self.modIPv6MalformedPacketsTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(MoDReportTest, self).tearDown()

    def modNoDropTest(self):
        '''
        Test verifies no report is generated when the packet is not dropped
        '''
        print("modNoDropTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        try:
            # create DTEL watchlist
            bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
            bind_point_type_list = sai_thrift_s32_list_t(
                count=len(bind_points), int32list=bind_points)
            action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
            action_type_list = sai_thrift_s32_list_t(
                count=len(action_types), int32list=action_types)
            dtel_acl_table_id = sai_thrift_create_acl_table(
                self.client,
                acl_stage=SAI_ACL_STAGE_INGRESS,
                acl_bind_point_type_list=bind_point_type_list,
                field_src_ip=True,
                field_dst_ip=True,
                acl_action_type_list=action_type_list)
            sai_thrift_set_switch_attribute(self.client,
                                            ingress_acl=dtel_acl_table_id)

            # create DTEL watchlist entry
            src_ip = IPADDR_NBR[0]
            src_ip_mask = '255.255.255.0'
            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
            dst_ip = IPADDR_NBR[1]
            dst_ip_mask = '255.255.255.0'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))

            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_src_ip=src_ip_t,
                field_dst_ip=dst_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)

            payload = '@!#?'
            pkt = simple_udp_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=108,
                ip_ttl=64,
                with_udp_chksum=True,
                udp_payload=payload)

            exp_pkt = simple_udp_packet(
                eth_dst=self.mac_nbr[1],
                eth_src=MAC_SELF,
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                with_udp_chksum=True,
                ip_id=108,
                ip_ttl=63,
                udp_payload=payload)

            send_packet(self, self.dev_port24, pkt)
            verify_packet(self, exp_pkt, self.dev_port25)
            verify_no_other_packets(self, timeout=1)
            print("pass packet forwarded")

        finally:
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)

    def modHostifReasonCodeDropTest(self):
        '''
        Test verifies report is generated for trapped packet.
        Packet is trapped using drop hostif trap action.
        '''
        print("modHostifReasonCodeDropTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)

        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

        trap_group = sai_thrift_create_hostif_trap_group(
            self.client, admin_state=True, queue=4)
        trap = sai_thrift_create_hostif_trap(
            self.client,
            trap_group=trap_group,
            trap_type=SAI_HOSTIF_TRAP_TYPE_PIM,
            packet_action=SAI_PACKET_ACTION_DROP)

        pim_pkt = simple_ip_packet(
            ip_proto=103,  # PIM
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            pktlen=256)

        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': SWITCH_HOSTIF_TRAP_ATTR_TYPE_PIM}
        exp_mod_pkt = exp_mod_packet(pim_pkt, self.int_v2, fields)

        try:
            send_packet(self, self.dev_port24, pim_pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass - drop report generated")

        finally:
            sai_thrift_remove_hostif_trap(self.client, trap)
            sai_thrift_remove_hostif_trap_group(self.client, trap_group)
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)

    def ingressAclDropReportTest(self):
        ''' Test verifies report is generated for ingress ACL '''
        print("ingressAclDropReportTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=False)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)

        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)
        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

        # create ACL table
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        pktlen = 256
        payload = '@!#?'
        pkt = simple_udp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=108,
            ip_ttl=64,
            with_udp_chksum=True,
            udp_payload=payload,
            pktlen=pktlen)

        exp_pkt = simple_udp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            with_udp_chksum=True,
            ip_id=108,
            ip_ttl=63,
            udp_payload=payload,
            pktlen=pktlen)

        # SWITCH_DROP_REASON_ACL_DROP 80
        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': 80}
        exp_mod_pkt = exp_mod_packet(pkt, self.int_v2, fields)

        try:
            send_packet(self, self.dev_port24, pkt)
            verify_packet(self, exp_pkt, self.dev_port25)
            verify_no_other_packets(self, timeout=1)
            print("pass 1st packet w/o ACL drop")

            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=acl_table_id)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          ingress_acl=acl_table_id)
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            print("pass 2nd packet w/ ACL drop")

            sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                          drop_report_enable=True)

            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass 3rd packet w/ drop report")

            truncate_size = 128
            sai_thrift_set_dtel_report_session_attribute(
                self.client, self.dtel_report_session,
                truncate_size=truncate_size)

            if self.architecture == 'tofino':
                if self.int_v2:
                    drop_truncate_adjust = 8
                else:
                    drop_truncate_adjust = 6
            else:
                if self.int_v2:
                    drop_truncate_adjust = 8
                else:
                    drop_truncate_adjust = 4

            truncated_amount = pktlen - (truncate_size + drop_truncate_adjust)
            exp_mod_pkt_full = exp_mod_packet(pkt, self.int_v2, fields)
            exp_mod_pkt = Ether(bytes(exp_mod_pkt_full)[:-truncated_amount])
            exp_mod_pkt[IP].len -= truncated_amount  # noqa pylint: disable=no-member
            exp_mod_pkt[UDP].len -= truncated_amount  # noqa pylint: disable=no-member

            if self.int_v2:
                decrease_by = (truncated_amount + 3) // 4
                exp_mod_pkt[DTEL_REPORT_V2_HDR].report_length -= decrease_by

            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass 4th packet w/ drop report, truncated")

        finally:
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          ingress_acl=0)

            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)
            sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                          drop_report_enable=True)
            sai_thrift_set_dtel_report_session_attribute(
                self.client, self.dtel_report_session,
                truncate_size=self.report_truncate_size)

    def egressAclDropReportTest(self):
        ''' Test verifies report is generated for egress ACL '''
        print("egressAclDropReportTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

        # create ACL table
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_EGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        pktlen = 256
        payload = '@!#?'
        pkt = simple_udp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=108,
            ip_ttl=64,
            with_udp_chksum=True,
            udp_payload=payload,
            pktlen=pktlen)

        exp_pkt = simple_udp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            with_udp_chksum=True,
            ip_id=108,
            ip_ttl=63,
            udp_payload=payload,
            pktlen=pktlen)

        # SWITCH_DROP_REASON_EGRESS_ACL_DROP 92
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4, 'drop_reason': 92}
        exp_mod_pkt = exp_egress_mod_packet(exp_pkt, self.int_v2, fields)

        try:
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          egress_acl=acl_table_id)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          egress_acl=acl_table_id)

            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass 1st packet w/ drop report")

        finally:
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          egress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          egress_acl=0)
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)

    def dtelWatchlistTest(self):
        ''' Test verifies drop reasons for IPv4 and IPv6 watchlist. '''
        print("dtelWatchlistTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)

        # create ACL tables
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)
        acl_table_id2 = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ipv6=True,
            field_dst_ipv6=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_packet_action=packet_action)
        src_ip = IPADDR_NBR[1]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[2]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id2 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_packet_action=packet_action)

        src_ip = '2000::1'
        src_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip6=src_ip_mask))
        dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=self.ipv6_1),
            mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id3 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id2,
            priority=10,
            field_src_ipv6=src_ip_t,
            field_dst_ipv6=dst_ip_t,
            action_packet_action=packet_action)

        src_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=self.ipv6_1),
            mask=sai_thrift_acl_field_data_mask_t(ip6=src_ip_mask))
        dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=self.ipv6_2),
            mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id4 = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id2,
            priority=10,
            field_src_ipv6=src_ip_t,
            field_dst_ipv6=dst_ip_t,
            action_packet_action=packet_action)

        sai_thrift_set_port_attribute(self.client, self.port24,
                                      ingress_acl=acl_table_id)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      ingress_acl=acl_table_id)
        sai_thrift_set_port_attribute(self.client, self.port26,
                                      ingress_acl=acl_table_id)

        pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=64)

        non_dtel_pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[2],
            ip_src=IPADDR_NBR[1],
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=32)

        pkt_v6 = simple_tcpv6_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ipv6_dst=self.ipv6_1,
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=64)

        non_dtel_pkt_v6 = simple_tcpv6_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ipv6_dst=self.ipv6_2,
            ipv6_src=self.ipv6_1,
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=33)

        # SWITCH_DROP_REASON_ACL_DROP 80
        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': 80}
        exp_mod_pkt = exp_mod_packet(pkt, self.int_v2, fields)
        non_dtel_exp_mod_pkt = exp_mod_packet(non_dtel_pkt, self.int_v2,
                                              fields)
        exp_mod_pkt_v6 = exp_mod_packet(pkt_v6, self.int_v2, fields)

        try:
            print("Verifying IPv4 packets")
            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on dst_ip field")
            dst_ip = IPADDR_NBR[1]
            dst_ip_mask = '255.255.255.0'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_dst_ip=dst_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on src_ip field")
            src_ip = IPADDR_NBR[0]
            src_ip_mask = '255.255.255.0'
            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_src_ip=src_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on ip_proto field")
            ip_proto = 6
            ip_proto_mask = 127
            ip_proto_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
                mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_ip_protocol=ip_proto_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Report the 2nd packet also since IP_PROTO=6 matches")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_dtel_packet(self, non_dtel_exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on ip_tos field")
            dscp = 2
            dscp_mask = 127
            dscp_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=dscp),
                mask=sai_thrift_acl_field_data_mask_t(u8=dscp_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_dscp=dscp_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on l4_src_port field")
            l4_src_port = 3333
            l4_src_port_mask = 32759
            l4_src_port_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u16=l4_src_port),
                mask=sai_thrift_acl_field_data_mask_t(u16=l4_src_port_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_l4_src_port=l4_src_port_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on l4_dst_port field")
            l4_dst_port = 5555
            l4_dst_port_mask = 32759
            l4_dst_port_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u16=l4_dst_port),
                mask=sai_thrift_acl_field_data_mask_t(u16=l4_dst_port_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_l4_dst_port=l4_dst_port_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on ip_ttl field")
            ttl = 64
            ttl_mask = 127
            ttl_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=ttl),
                mask=sai_thrift_acl_field_data_mask_t(u8=ttl_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_ttl=ttl_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on tcp_flags field")
            tcp_flags = 4
            tcp_flags_mask = 127
            tcp_flags_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=tcp_flags),
                mask=sai_thrift_acl_field_data_mask_t(u8=tcp_flags_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_tcp_flags=tcp_flags_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on ether_type field")
            ether_type = 0x0800
            ether_type_mask = 0x7FFF
            ether_type_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u16=ether_type),
                mask=sai_thrift_acl_field_data_mask_t(u16=ether_type_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_ether_type=ether_type_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Report the 2nd packet also since eth type 0x800 matches")
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_dtel_packet(self, non_dtel_exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Verifying IPv6 packets")
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=acl_table_id2)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          ingress_acl=acl_table_id2)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=acl_table_id2)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt_v6)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt_v6)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on src_ip field")
            src_ip = '2000::1'
            src_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=src_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip6=src_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_src_ipv6=src_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt_v6)
            verify_dtel_packet(self, exp_mod_pkt_v6, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt_v6)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt_v6)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt_v6)
            verify_no_other_packets(self, timeout=1)

            print("Add ACL entry to match and report on dst_ip field")
            dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip6=self.ipv6_1),
                mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_dst_ipv6=dst_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pkt_v6)
            verify_dtel_packet(self, exp_mod_pkt_v6, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pkt_v6)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Send packet with no dtel ACL rule")
            send_packet(self, self.dev_port24, pkt_v6)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port24, non_dtel_pkt_v6)
            verify_no_other_packets(self, timeout=1)

        finally:
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port26,
                                          ingress_acl=0)
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id4)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id3)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id2)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id2)
            sai_thrift_remove_acl_table(self.client, acl_table_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)

    def modPortBindingWatchlistTest(self):
        '''
        Test verifies report is generated for trapped packet.
        Packet is trapped using drop hostif trap action.
        '''
        print("modPortBindingWatchlistTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

        sai_thrift_set_port_attribute(self.client, self.port24,
                                      ingress_acl=dtel_acl_table_id)

        trap_group = sai_thrift_create_hostif_trap_group(
            self.client, admin_state=True, queue=4)
        trap = sai_thrift_create_hostif_trap(
            self.client,
            trap_group=trap_group,
            trap_type=SAI_HOSTIF_TRAP_TYPE_PIM,
            packet_action=SAI_PACKET_ACTION_DROP)
        pim_pkt = simple_ip_packet(
            ip_proto=103,  # PIM
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_tos=8,
            ip_id=105,
            ip_ttl=64,
            pktlen=256)
        non_dtel_pim_pkt = simple_ip_packet(
            ip_proto=103,  # PIM
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[2],
            ip_src=IPADDR_NBR[1],
            ip_tos=4,
            ip_id=106,
            ip_ttl=32,
            pktlen=256)

        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': SWITCH_HOSTIF_TRAP_ATTR_TYPE_PIM}
        exp_mod_pkt = exp_mod_packet(pim_pkt, self.int_v2, fields)
        non_dtel_exp_mod_pkt = exp_mod_packet(non_dtel_pim_pkt, self.int_v2,
                                              fields)

        try:
            send_packet(self, self.dev_port24, pim_pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass - drop report generated")

            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            print("Add ACL entry to match and report on dst_ip field")
            dst_ip = IPADDR_NBR[1]
            dst_ip_mask = '255.255.255.0'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_dst_ip=dst_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pim_pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pim_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Add ACL entry to match and report on src_ip field")
            src_ip = IPADDR_NBR[0]
            src_ip_mask = '255.255.255.0'
            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_src_ip=src_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pim_pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pim_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Add ACL entry to match and report on ip_proto field")
            ip_proto = 103
            ip_proto_mask = 127
            ip_proto_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
                mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_ip_protocol=ip_proto_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pim_pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Report the 2nd packet also since IP_PROTO=103 matches")
            send_packet(self, self.dev_port24, non_dtel_pim_pkt)
            verify_dtel_packet(self, non_dtel_exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Add ACL entry to match and report on ip_tos field")
            dscp = 2
            dscp_mask = 127
            dscp_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=dscp),
                mask=sai_thrift_acl_field_data_mask_t(u8=dscp_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_dscp=dscp_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pim_pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pim_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Add ACL entry to match and report on ip_ttl field")
            ttl = 64
            ttl_mask = 127
            ttl_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u8=ttl),
                mask=sai_thrift_acl_field_data_mask_t(u8=ttl_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_ttl=ttl_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pim_pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Do not report unmatched packet")
            send_packet(self, self.dev_port24, non_dtel_pim_pkt)
            verify_no_other_packets(self, timeout=1)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            print("Add ACL entry to match and report on ether_type field")
            ether_type = 0x0800
            ether_type_mask = 0x7FFF
            ether_type_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u16=ether_type),
                mask=sai_thrift_acl_field_data_mask_t(u16=ether_type_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_ether_type=ether_type_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)
            send_packet(self, self.dev_port24, pim_pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("Report the 2nd packet also since eth type 0x800 matches")
            send_packet(self, self.dev_port24, non_dtel_pim_pkt)
            verify_dtel_packet(self, non_dtel_exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)

        finally:
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_remove_hostif_trap(self.client, trap)
            sai_thrift_remove_hostif_trap_group(self.client, trap_group)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)

    def modIPv4MalformedPacketsTest(self):
        ''' Test verifies drop reports for different drop reasonsf for IPv4 '''
        print("modIPv4MalformedPacketsTest")
        dropped_num = 0
        bind_mirror_on_drop_pkt(self.int_v2)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

        try:
            stats = sai_thrift_get_port_stats(self.client, self.port24)
            initial_stats = stats['SAI_PORT_STAT_IF_IN_DISCARDS']
            print("Valid packet from port %d to port %d" %
                  (self.dev_port24, self.dev_port25))
            payload = '@!#?'
            pkt = simple_udp_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=108,
                ip_ttl=64,
                with_udp_chksum=True,
                udp_payload=payload)

            exp_pkt = simple_udp_packet(
                eth_dst=self.mac_nbr[1],
                eth_src=MAC_SELF,
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                with_udp_chksum=True,
                ip_id=108,
                ip_ttl=63,
                udp_payload=payload)

            send_packet(self, self.dev_port24, pkt)
            verify_packets(self, exp_pkt, [self.dev_port25])

            print("Valid invalid checksum, drop")
            pkt[IP].chksum = 0
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_INVALID_CHECKSUM 31
            fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                      'drop_reason': 31}
            verify_dtel_packet(self,
                               exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            dropped_num += 1

            print("IPv4 IHL 0, drop")
            pkt = simple_tcp_packet(
                eth_src=self.mac_nbr[1],
                eth_dst=MAC_SELF,
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=108,
                ip_ihl=0)
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_IHL INVALID 30
            self.assertTrue(receive_packet(self, self.dev_port27))
            dropped_num += 1

            print("IPv4 IHL 1, drop")
            pkt[IP].ihl = 1
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_IHL INVALID 30
            self.assertTrue(receive_packet(self, self.dev_port27))
            dropped_num += 1

            print("IPv4 IHL 2, drop")
            pkt[IP].ihl = 2
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_IHL INVALID 30
            self.assertTrue(receive_packet(self, self.dev_port27))
            dropped_num += 1

            print("IPv4 IHL 3, drop")
            pkt[IP].ihl = 3
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_IHL INVALID 30
            self.assertTrue(receive_packet(self, self.dev_port27))
            dropped_num += 1

            print("IPv4 IHL 4, drop")
            pkt[IP].ihl = 4
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_IHL INVALID 30
            self.assertTrue(receive_packet(self, self.dev_port27))
            dropped_num += 1

            print("IPv4 TTL 0, drop")
            pkt = simple_tcp_packet(
                eth_src=self.mac_nbr[1],
                eth_dst=MAC_SELF,
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=108,
                ip_ttl=0)
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_TTL_ZERO 26
            fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                      'drop_reason': 26}
            verify_dtel_packet(self,
                               exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            dropped_num += 1

            print("IPv4 invalid version, drop")
            pkt = simple_tcp_packet(
                eth_src=self.mac_nbr[1],
                eth_dst=MAC_SELF,
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=108,
                ip_ttl=64)
            pkt[IP].version = 6
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_VERSION_INVALID 25
            fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                      'drop_reason': 25}
            verify_dtel_packet(self,
                               exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            dropped_num += 1

            sai_thrift_set_route_entry_attribute(
                self.client, self.route11,
                packet_action=SAI_PACKET_ACTION_DROP)

            print("Route drop")
            payload = '@!#?'
            pkt = simple_udp_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=108,
                ip_ttl=64,
                with_udp_chksum=True,
                udp_payload=payload)

            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_BLACKHOLE_ROUTE 53
            fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                      'drop_reason': 53}
            verify_dtel_packet(self,
                               exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            dropped_num += 1

            # This is not working for now - bug opened
            sai_thrift_set_route_entry_attribute(
                self.client, self.route11,
                packet_action=SAI_PACKET_ACTION_FORWARD)
            # Workaround
            sai_thrift_remove_route_entry(self.client, self.route11)
            self.route11 = sai_thrift_route_entry_t(
                vr_id=self.vrf,
                destination=sai_ipprefix(IPADDR_NBR[1] + '/32'))
            sai_thrift_create_route_entry(self.client, self.route11,
                                          next_hop_id=self.nhop11)

            # Create DTEL acl entry matching loopback IP
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            src_ip = '127.10.10.0'
            src_ip_mask = '255.255.255.0'
            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
            dst_ip = IPADDR_NBR[1]
            dst_ip_mask = '255.255.255.0'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))

            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_src_ip=src_ip_t,
                field_dst_ip=dst_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)

            print("IPv4 src is loopback, drop")
            payload = '@!#?'
            pkt = simple_udp_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ip_dst=IPADDR_NBR[1],
                ip_src='127.10.10.1',
                ip_id=108,
                ip_ttl=0,
                with_udp_chksum=True,
                udp_payload=payload)

            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_SRC_LOOPBACK 28
            fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                      'drop_reason': 28}
            verify_dtel_packet(self,
                               exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            dropped_num += 1

            stats = sai_thrift_get_port_stats(self.client, self.port24)
            final_stats = stats['SAI_PORT_STAT_IF_IN_DISCARDS']
            self.assertTrue(final_stats - dropped_num == initial_stats)
        finally:
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)

    def modIPv6MalformedPacketsTest(self):
        ''' Test verifies drop reports for different drop reasons from IPv6 '''
        print("modIPv6MalformedPacketsTest")
        dropped_num = 0
        bind_mirror_on_drop_pkt(self.int_v2)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = '2000::1'
        src_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip6=src_ip_mask))
        dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=self.ipv6_1),
            mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_src_ipv6=src_ip_t,
            field_dst_ipv6=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

        try:
            stats = sai_thrift_get_port_stats(self.client, self.port24)
            initial_stats = stats['SAI_PORT_STAT_IF_IN_DISCARDS']
            print("Valid packet from port %d to port %d" %
                  (self.dev_port24, self.dev_port25))
            pkt = simple_tcpv6_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ipv6_dst=self.ipv6_1,
                ipv6_src='2000::1',
                ipv6_hlim=64)

            exp_pkt = simple_tcpv6_packet(
                eth_dst=self.mac_nbr[1],
                eth_src=MAC_SELF,
                ipv6_dst=self.ipv6_1,
                ipv6_src='2000::1',
                ipv6_hlim=63)

            send_packet(self, self.dev_port24, pkt)
            verify_packets(self, exp_pkt, [self.dev_port25])

            print("MAC SA IPv6 multicast, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=MAC_SELF,
                eth_src='33:33:5e:00:00:01',
                ipv6_dst=self.ipv6_1,
                ipv6_src='2000::1',
                ipv6_hlim=0)

            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_SRC_MAC_MULTICAST 11
            fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                      'drop_reason': 11}
            verify_dtel_packet(self,
                               exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            dropped_num += 1

            print("IPv6 TTL 0, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ipv6_dst=self.ipv6_1,
                ipv6_src='2000::1',
                ipv6_hlim=0)

            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_TTL_ZERO 26
            fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                      'drop_reason': 26}
            verify_dtel_packet(self,
                               exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            dropped_num += 1

            print("IPv6 invalid version, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ipv6_dst=self.ipv6_1,
                ipv6_src='2000::1',
                ipv6_hlim=64)

            pkt[IPv6].version = 4
            send_packet(self, self.dev_port24, pkt)
            # SWITCH_DROP_REASON_OUTER_IP_INVALID_VERSION 25
            fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                      'drop_reason': 25}
            verify_dtel_packet(self,
                               exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            dropped_num += 1

            stats = sai_thrift_get_port_stats(self.client, self.port24)
            final_stats = stats['SAI_PORT_STAT_IF_IN_DISCARDS']
            self.assertTrue(final_stats - dropped_num == initial_stats)
        finally:
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)


# Test is disabled by default due to inability to disabled suppression
# for L2 malformed packets. Please remember to restart the model
# if this test is run
@group('dtel-restart')
class MoDL2MalformedPacketsTest(DtelBaseTest):
    """ MoD L2 Malformed Packets test class """

    def runTest(self):
        ''' Test verifies drop reasons L2 '''
        print("modL2MalformedPacketsTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            drop_report_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)

        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)
        # create DTEL watchlist entry
        dst_ip = '11.11.11.0'
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

        port28_bp = sai_thrift_create_bridge_port(
            self.client, bridge_id=self.default_1q_bridge,
            port_id=self.port28, type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        port29_bp = sai_thrift_create_bridge_port(
            self.client, bridge_id=self.default_1q_bridge,
            port_id=self.port29, type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)

        lag10 = sai_thrift_create_lag(self.client)
        lag10_bp = sai_thrift_create_bridge_port(
            self.client, bridge_id=self.default_1q_bridge,
            port_id=lag10, type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True)
        lag10_member30 = sai_thrift_create_lag_member(
            self.client, lag_id=lag10, port_id=self.port30)
        lag10_member31 = sai_thrift_create_lag_member(
            self.client, lag_id=lag10, port_id=self.port31)

        vlan100 = sai_thrift_create_vlan(self.client, vlan_id=100)
        vlan_member101 = sai_thrift_create_vlan_member(
            self.client, vlan_id=vlan100, bridge_port_id=port28_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member102 = sai_thrift_create_vlan_member(
            self.client, vlan_id=vlan100, bridge_port_id=port29_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        vlan_member103 = sai_thrift_create_vlan_member(
            self.client, vlan_id=vlan100, bridge_port_id=lag10_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

        sai_thrift_set_port_attribute(self.client, self.port28,
                                      port_vlan_id=100)
        sai_thrift_set_port_attribute(self.client, self.port29,
                                      port_vlan_id=100)

        mac_action = SAI_PACKET_ACTION_FORWARD
        fdb_entry1 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address='00:01:00:00:00:12',
            bv_id=vlan100)
        sai_thrift_create_fdb_entry(
            self.client, fdb_entry1, type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=port28_bp, packet_action=mac_action)
        fdb_entry2 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address='00:01:00:00:00:22',
            bv_id=vlan100)
        sai_thrift_create_fdb_entry(
            self.client, fdb_entry2, type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=port29_bp, packet_action=mac_action)
        fdb_entry3 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address='00:01:00:00:00:13',
            bv_id=vlan100)
        sai_thrift_create_fdb_entry(
            self.client, fdb_entry3, type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=port28_bp, packet_action=mac_action)
        fdb_entry4 = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id, mac_address='00:01:00:00:00:32',
            bv_id=vlan100)
        sai_thrift_create_fdb_entry(
            self.client, fdb_entry4, type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=lag10_bp, packet_action=mac_action)

        try:
            print("Valid packet from port %d to %d" %
                  (self.dev_port28, self.dev_port29))
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            verify_packets(self, pkt, [self.dev_port29])

            print("Valid packet from lag 10 to port %d" % (self.dev_port29))
            tag_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:32',
                ip_dst='11.11.11.1',
                dl_vlan_enable=True,
                vlan_vid=100,
                ip_ttl=64)
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:32',
                ip_dst='11.11.11.1',
                ip_ttl=64,
                pktlen=96)
            send_packet(self, self.dev_port30, tag_pkt)
            verify_packets(self, pkt, [self.dev_port29])

            print("Same if check fail, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:13',
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_SAME_IFINDEX 58
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 58}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("MAC DA zeros, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:00:00:00:00:00',
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_OUTER_DST_MAC_ZERO 12
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 12}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("MAC SA zeros, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:00:00:00:00:00',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_OUTER_SRC_MAC_ZERO 10
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 10}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("MAC SA broadcast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='ff:ff:ff:ff:ff:ff',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_OUTER_SRC_MAC_MULTICAST 11
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 11}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("MAC SA multicast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='01:00:5e:00:00:01',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_OUTER_SRC_MAC_MULTICAST 11
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 11}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("MAC_SA==MAC_DA, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:22',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_OUTER_SAME_MAC_CHECK 11
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 17}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("Port vlan mapping miss, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_PORT_VLAN_MAPPING_MISS 55
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 55}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("Port vlan mapping miss lag, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port30, pkt)
            # SWITCH_DROP_REASON_PORT_VLAN_MAPPING_MISS 55
            fields = {'ingress_port': self.port30_fp, 'hw_id': 4,
                      'drop_reason': 55}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.dev_port31, pkt)
            # SWITCH_DROP_REASON_PORT_VLAN_MAPPING_MISS 55
            fields = {'ingress_port': self.port31_fp, 'hw_id': 4,
                      'drop_reason': 55}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("L2 unicast miss, drop")
            sai_thrift_set_switch_attribute(
                self.client,
                fdb_unicast_miss_packet_action=SAI_PACKET_ACTION_DROP)
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:44',
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_L2_MISS_UNICAST 89
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 89}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("L2 multicast miss, drop")
            sai_thrift_set_switch_attribute(
                self.client,
                fdb_multicast_miss_packet_action=SAI_PACKET_ACTION_DROP)
            pkt = simple_tcp_packet(
                eth_dst='11:11:11:22:22:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_L2_MISS_MULTICAST 90
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 90}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

            print("L2 broadcast miss, drop")
            sai_thrift_set_switch_attribute(
                self.client,
                fdb_broadcast_miss_packet_action=SAI_PACKET_ACTION_DROP)
            pkt = simple_tcp_packet(
                eth_dst='ff:ff:ff:ff:ff:ff',
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.dev_port28, pkt)
            # SWITCH_DROP_REASON_L2_MISS_BROADCAST 91
            fields = {'ingress_port': self.port28_fp, 'hw_id': 4,
                      'drop_reason': 91}
            verify_dtel_packet(self, exp_mod_packet(pkt, self.int_v2, fields),
                               self.dev_port27)
            verify_no_other_packets(self, timeout=1)

        finally:
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_set_switch_attribute(
                self.client,
                fdb_unicast_miss_packet_action=SAI_PACKET_ACTION_FORWARD)
            sai_thrift_set_switch_attribute(
                self.client,
                fdb_multicast_miss_packet_action=SAI_PACKET_ACTION_FORWARD)
            sai_thrift_set_switch_attribute(
                self.client,
                fdb_broadcast_miss_packet_action=SAI_PACKET_ACTION_FORWARD)

            sai_thrift_set_port_attribute(self.client, self.port28,
                                          port_vlan_id=0)
            sai_thrift_set_port_attribute(self.client, self.port29,
                                          port_vlan_id=0)

            sai_thrift_remove_fdb_entry(self.client, fdb_entry1)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry2)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry3)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry4)

            sai_thrift_remove_vlan_member(self.client, vlan_member101)
            sai_thrift_remove_vlan_member(self.client, vlan_member102)
            sai_thrift_remove_vlan_member(self.client, vlan_member103)
            sai_thrift_remove_vlan(self.client, vlan100)

            sai_thrift_remove_lag_member(self.client, lag10_member30)
            sai_thrift_remove_lag_member(self.client, lag10_member31)
            sai_thrift_remove_bridge_port(self.client, lag10_bp)
            sai_thrift_remove_lag(self.client, lag10)

            sai_thrift_remove_bridge_port(self.client, port28_bp)
            sai_thrift_remove_bridge_port(self.client, port29_bp)

            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)
            sai_thrift_remove_dtel_report_session(self.client,
                                                  dtel_report_session)
            sai_thrift_remove_dtel(self.client, dtel)


class MoDAndIngressPortMirrorTest(DtelBaseTest):
    """ MoD and ingress port mirror test class """

    def setUp(self):
        super(MoDAndIngressPortMirrorTest, self).setUp()
        bind_mirror_on_drop_pkt(self.int_v2)

        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)

        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable,
            action_acl_dtel_flow_op=acl_dtel_flow_op)

        self.mirror_session = sai_thrift_create_mirror_session(
            self.client,
            monitor_port=self.port27,
            type=SAI_MIRROR_SESSION_TYPE_LOCAL)

        payload = '@!#?'
        self.pkt = simple_udp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=108,
            ip_ttl=64,
            with_udp_chksum=True,
            udp_payload=payload)
        self.pkt[IP].chksum = 0

        self.exp_pkt = simple_udp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            with_udp_chksum=True,
            ip_id=108,
            ip_ttl=63,
            udp_payload=payload)

    def runTest(self):
        self.dropReportEnabledIngressMirrorDisabledTest()
        self.dropReportEnabledIngressMirrorEnabledTest()
        self.dropReportDisabledIngressMirrorEnabledTest()

    def tearDown(self):
        obj_list = sai_thrift_object_list_t(count=0, idlist=[])
        sai_thrift_set_port_attribute(
            self.client, self.port24,
            ingress_mirror_session=obj_list)
        sai_thrift_remove_mirror_session(self.client, self.mirror_session)
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)
        super(MoDAndIngressPortMirrorTest, self).tearDown()

    def dropReportEnabledIngressMirrorDisabledTest(self):
        '''
        Test verifies correct flow with
        drop report enabled and ingress mirror disabled.
        '''
        print("dropReportEnabledIngressMirrorDisabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        obj_list = sai_thrift_object_list_t(count=0, idlist=[])
        sai_thrift_set_port_attribute(
            self.client, self.port24,
            ingress_mirror_session=obj_list)

        # SWITCH_DROP_REASON_OUTER_IP_INVALID_CHECKSUM 31
        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': 31}
        exp_mod_pkt = exp_mod_packet(self.pkt, self.int_v2, fields)

        print("Sending packet port %d" % self.dev_port24,
              " -> port %d" % self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def dropReportEnabledIngressMirrorEnabledTest(self):
        '''
        Test verifies correct flow with
        drop report enabled and ingress mirror enabled.
        '''
        print("dropReportEnabledIngressMirrorEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        obj_list = sai_thrift_object_list_t(count=1,
                                            idlist=[self.mirror_session])
        sai_thrift_set_port_attribute(
            self.client, self.port24,
            ingress_mirror_session=obj_list)

        print("Sending packet port %d" % self.dev_port24,
              " -> port %d" % self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packets(self, self.pkt, [self.dev_port27])

    def dropReportDisabledIngressMirrorEnabledTest(self):
        '''
        Test verifies correct flow with
        drop report disabled and ingress mirror enabled.
        '''
        print("dropReportDisabledIngressMirrorEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=False)
        obj_list = sai_thrift_object_list_t(count=1,
                                            idlist=[self.mirror_session])
        sai_thrift_set_port_attribute(
            self.client, self.port24,
            ingress_mirror_session=obj_list)

        print("Sending packet port %d" % self.dev_port24,
              " -> port %d" % self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packets(self, self.pkt, [self.dev_port27])


class FlowReportTest(DtelBaseTest):
    """ Basic Flow Report test class """

    def setUp(self):
        IPADDR_INF[2] = '1.1.0.2'
        IPADDR_NBR[2] = '1.1.0.101'
        super(FlowReportTest, self).setUp()
        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            postcard_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        # create high priority DTEL watchlist entry
        # with protocol==udp and l4_dst_port=self.report_udp_port
        # to prevent loop reports
        l4_dst_port = self.report_udp_port
        l4_dst_port_mask = 32759
        l4_dst_port_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=l4_dst_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_dst_port_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_NOP))
        ip_proto = 17
        ip_proto_mask = 127
        ip_proto_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
            mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))

        self.dtel_acl_high_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1000,
            field_l4_dst_port=l4_dst_port_t,
            field_ip_protocol=ip_proto_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

    def runTest(self):
        try:
            self.noFlowReportTest()
            self.validFlowReportTest()
            self.ipFlowReportTest()
            self.flowReportSeqNumTest()
            self.ipv4FlowDtelAclTableFieldTest()
            self.ipv6FlowDtelAclTableFieldTest()
            self.flowDtelAclPriorityTest()
            self.flowReportSessionTest()
            self.multiMirrorTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_high_entry_id)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(FlowReportTest, self).tearDown()
        IPADDR_INF[2] = '172.16.0.4'
        IPADDR_NBR[2] = '172.16.0.1'

    def noFlowReportTest(self):
        '''
        Test verifies no flow report is generated
        when flow reports are disabled.
        '''
        print("noFlowReportTest")
        pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=False)
        # no flow report
        try:
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

            src_ip = IPADDR_NBR[0]
            src_ip_mask = '255.255.255.0'
            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
            dst_ip = '10.10.10.10'
            dst_ip_mask = '255.255.255.0'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            acl_dtel_flow_op = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

            self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.dtel_acl_table_id,
                priority=10,
                field_src_ip=src_ip_t,
                field_dst_ip=dst_ip_t,
                action_acl_dtel_flow_op=acl_dtel_flow_op,
                action_dtel_report_all_packets=enable)

            print("No report should be generated since flow report " +
                  "are disabled")
            print("Sending packet port %d" % self.dev_port24, " -> port %d" %
                  self.dev_port27)
            send_packet(self, self.dev_port24, pkt_in)
            verify_packet(self, exp_pkt_out, self.dev_port25)
            verify_no_other_packets(self, timeout=1)
            print("Flow report enabled")
            print("No report should be generated " +
                  "since flow is not configured in dtel acl")
            sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                          postcard_enable=True)
            send_packet(self, self.dev_port24, pkt_in)
            verify_packet(self, exp_pkt_out, self.dev_port25)
            verify_no_other_packets(self, timeout=1)
        finally:
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

            src_ip = IPADDR_NBR[0]
            src_ip_mask = '255.255.255.0'
            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
            dst_ip = IPADDR_NBR[1]
            dst_ip_mask = '255.255.255.0'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            acl_dtel_flow_op = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

            self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.dtel_acl_table_id,
                priority=10,
                field_src_ip=src_ip_t,
                field_dst_ip=dst_ip_t,
                action_acl_dtel_flow_op=acl_dtel_flow_op,
                action_dtel_report_all_packets=enable)

    def validFlowReportTest(self):
        ''' Test verifies valid flow report generation '''
        print("validFlowReportTest")
        pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                               fields)

        print("Reports for flow that matches dtel_acl")
        print("Valid report should be generated for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should be generated again for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[2],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[2],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[2],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port26_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt2 = exp_postcard_packet(exp_pkt, self.int_v2, fields)

        print("Valid report should be generated for %s" % IPADDR_NBR[2])
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port26)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def ipFlowReportTest(self):
        '''
        Test verifies flow report for packets other than TCP, UDP or ICMP
        '''
        print("ipFlowReportTest")

        print("Report should be generated for IP flow " +
              "that is not TCP, UDP or ICMP")
        pkt = simple_ip_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_proto=103,  # PIM
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_ip_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_proto=103,  # PIM
            ip_id=105,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt3 = exp_postcard_packet(exp_pkt, self.int_v2, fields)

        print("Valid report should be generated for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt3, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def flowReportSeqNumTest(self):
        ''' Test verifies sequence number incrementation in flow reports '''
        print("flowReportSeqNumTest")
        pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                               fields)

        print("Valid report should be generated for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        postcard_pkt = receive_postcard_packet(self, exp_postcard_pkt,
                                               self.dev_port27)
        if self.int_v2:
            index = DTEL_REPORT_V2_HDR
        else:
            index = DTEL_REPORT_HDR
        current_seq_num = postcard_pkt[index].sequence_number

        print("Initial sequence number = %d" % current_seq_num)
        for _ in range(5):
            current_seq_num += 1
            exp_postcard_pkt[index].sequence_number = current_seq_num
            send_packet(self, self.dev_port24, pkt_in)
            verify_packet(self, exp_pkt_out, self.dev_port25)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27,
                                   ignore_seq_num=False)

    def ipv4FlowDtelAclTableFieldTest(self):
        ''' Test verifies watchlist fields for IPv4 '''
        print("ipv4FlowDtelAclTableFieldTest")
        pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src='00:22:22:22:22:22',
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=63)
        no_dtel_pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[2],
            ip_src=IPADDR_NBR[1],
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=32)
        exp_no_dtel_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[2],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[2],
            ip_src=IPADDR_NBR[1],
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=31)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2, fields)
        fields = {'ingress_port': self.port25_fp,
                  'egress_port': self.port26_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt2 = exp_postcard_packet(exp_no_dtel_pkt, self.int_v2,
                                                fields)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on dst_ip field")
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.255'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on src_ip field")
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.255'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on ip_proto field")
        ip_proto = 6
        ip_proto_mask = 127
        ip_proto_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
            mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_ip_protocol=ip_proto_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report the 2nd packet also since IP_PROTO=6 matches")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on ip_tos field")
        dscp = 2
        dscp_mask = 127
        dscp_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=dscp),
            mask=sai_thrift_acl_field_data_mask_t(u8=dscp_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_dscp=dscp_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on l4_src_port field")
        l4_src_port = 3333
        l4_src_port_mask = 32759
        l4_src_port_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=l4_src_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_src_port_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True, parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_l4_src_port=l4_src_port_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on l4_dst_port field")
        l4_dst_port = 5555
        l4_dst_port_mask = 32759
        l4_dst_port_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=l4_dst_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_dst_port_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_l4_dst_port=l4_dst_port_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on ip_ttl field")
        pkt2 = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src='00:22:22:22:22:22',
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=62)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt3 = exp_postcard_packet(exp_pkt2, self.int_v2, fields)

        ttl = 63
        ttl_mask = 127
        ttl_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=ttl),
            mask=sai_thrift_acl_field_data_mask_t(u8=ttl_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))
        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_ttl=ttl_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt2)
        verify_packet(self, exp_pkt2, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt3, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on tcp_flags field")
        tcp_flags = 4
        tcp_flags_mask = 127
        tcp_flags_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=tcp_flags),
            mask=sai_thrift_acl_field_data_mask_t(u8=tcp_flags_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_tcp_flags=tcp_flags_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on ether_type field")
        ether_type = 0x0800
        ether_type_mask = 0x7FFF
        ether_type_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=ether_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=ether_type_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_ether_type=ether_type_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report the 2nd packet also since eth type 0x800 matches")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def ipv6FlowDtelAclTableFieldTest(self):
        ''' Test verifies watchlist fields for IPv6 '''
        print("ipv6FlowDtelAclTableFieldTest")
        pkt = simple_tcpv6_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ipv6_dst=self.ipv6_1,
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ipv6_dst=self.ipv6_1,
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=63)
        no_dtel_pkt = simple_tcpv6_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ipv6_dst=self.ipv6_2,
            ipv6_src='2000::2',
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=33)
        exp_no_dtel_pkt = simple_tcpv6_packet(
            eth_dst=self.mac_nbr[2],
            eth_src=MAC_SELF,
            ipv6_dst=self.ipv6_2,
            ipv6_src='2000::2',
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=32)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2, fields)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on dst_ip field")
        dst_ip_mask = 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=self.ipv6_1),
            mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_dst_ipv6=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Add ACL entry to match and report on src_ip field")
        src_ip = '2000::1'
        src_ip_mask = 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip6=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip6=src_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_src_ipv6=src_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Do not report unmatched packet")
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

    def flowDtelAclPriorityTest(self):
        ''' Test verifies acl priority flow reports '''
        print("flowDtelAclPriorityTest")

        pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src='00:22:22:22:22:22',
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=63)
        no_dtel_pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[2],
            ip_src=IPADDR_NBR[1],
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=32)
        exp_no_dtel_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[2],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[2],
            ip_src=IPADDR_NBR[1],
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=31)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2, fields)
        fields = {'ingress_port': self.port25_fp,
                  'egress_port': self.port26_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt2 = exp_postcard_packet(exp_no_dtel_pkt, self.int_v2,
                                                fields)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("Send packet with no dtel ACL rule")
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)

        print("Send packet matching dtel_acl entry for dst_ip with /24")
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Flow reports generated")

        print("Send packet matching dtel_acl entry for dst_ip with /28" +
              " with NOP action - no postcard should be generated")
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.240'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_NOP))

        acl_entry2 = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=100,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)
        print("No flow reports generated")

        print("Send packet matching dtel_acl entry for dst_ip with /32")
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.255'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        acl_entry3 = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1000,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)
        print("Report generated for packet matching /32")
        print("No report generated for packet matching /28 " +
              "but not matching /32")

        sai_thrift_remove_acl_entry(self.client, acl_entry3)
        print("Delete dtel_acl entry for dst_ip with /32")

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_no_other_packets(self, timeout=1)
        print("No flow reports generated")

        sai_thrift_set_acl_entry_attribute(
            self.client, acl_entry2,
            action_acl_dtel_flow_op=acl_dtel_flow_op)
        print("Change report_type of /28 entry to FLOW")

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Flow reports generated")

        sai_thrift_remove_acl_entry(self.client, acl_entry2)
        print("Delete dtel_acl entry for dst_ip with /28")

        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        send_packet(self, self.dev_port25, no_dtel_pkt)
        verify_packet(self, exp_no_dtel_pkt, self.dev_port26)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Flow reports generated")

    def flowReportSessionTest(self):
        ''' Test verifies flow report truncate length'''
        print("flowReportSessionTest")

        print("Reports for flow that matches dtel_acl")
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            pktlen=256)
        exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=63,
            pktlen=256)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        # udp_dst_port
        udp_port_dtel_report = UDP_PORT_DTEL_REPORT ^ 0x111
        print("Change DTEL report UDP port from %x to %x" %
              (UDP_PORT_DTEL_REPORT, udp_port_dtel_report))
        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            udp_dst_port=udp_port_dtel_report)

        if self.int_v2:
            split_layers(UDP, DTEL_REPORT_V2_HDR, dport=UDP_PORT_DTEL_REPORT)
            bind_layers(UDP, DTEL_REPORT_V2_HDR, dport=udp_port_dtel_report)
        else:
            split_layers(UDP, DTEL_REPORT_HDR, dport=UDP_PORT_DTEL_REPORT)
            bind_layers(UDP, DTEL_REPORT_HDR, dport=udp_port_dtel_report)

        exp_postcard_pkt[UDP].dport = udp_port_dtel_report
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            udp_dst_port=self.report_udp_port)
        if self.int_v2:
            split_layers(UDP, DTEL_REPORT_V2_HDR, dport=udp_port_dtel_report)
            bind_layers(UDP, DTEL_REPORT_V2_HDR, dport=UDP_PORT_DTEL_REPORT)
        else:
            split_layers(UDP, DTEL_REPORT_HDR, dport=udp_port_dtel_report)
            bind_layers(UDP, DTEL_REPORT_HDR, dport=UDP_PORT_DTEL_REPORT)

        # src_ip
        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            src_ip=sai_ipaddress('4.4.4.100'))

        exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2, fields)
        exp_postcard_pkt[IP].src = '4.4.4.100'
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            src_ip=sai_ipaddress(REPORT_SRC))

        # set dtel_dst_ip_list
        dtel_dst_addrs = [sai_ipaddress('5.5.5.1')]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            dst_ip_list=dtel_dst_ip_list)

        exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2, fields)
        exp_postcard_pkt[IP].dst = '5.5.5.1'
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)

        route = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('5.5.5.1/32'))
        sai_thrift_create_route_entry(self.client, route,
                                      next_hop_id=self.nhop13)

        print(sai_thrift_get_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            dst_ip_list=dtel_dst_ip_list))

        dtel_dst_addrs = [sai_ipaddress('5.5.5.1')]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            dst_ip_list=dtel_dst_ip_list)

        print(sai_thrift_get_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            dst_ip_list=dtel_dst_ip_list))

        exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2, fields)
        exp_postcard_pkt[IP].dst = '5.5.5.1'
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_remove_route_entry(self.client, route)

        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # truncate size
        truncate_size = 128
        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            truncate_size=truncate_size)
        print("Set report session truncated_size to %d" % truncate_size)

        truncated_amount = 256 - truncate_size

        exp_postcard_pkt_full = exp_postcard_packet(exp_pkt, self.int_v2,
                                                    fields)
        exp_postcard_pkt = Ether(
            bytes(exp_postcard_pkt_full)[:-truncated_amount])
        exp_postcard_pkt[IP].len -= truncated_amount  # noqa pylint: disable=no-member
        exp_postcard_pkt[UDP].len -= truncated_amount  # noqa pylint: disable=no-member

        if self.int_v2:
            decreased_by = (truncated_amount + 3) // 4
            exp_postcard_pkt[DTEL_REPORT_V2_HDR].report_length -= decreased_by  # noqa pylint: disable=no-member
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            truncate_size=512)

    def multiMirrorTest(self):
        '''
        Test verifies flow reports distributed among multiple destination IP
        adresses in report session.
        '''
        print("multiMirrorTest")

        print("Test queue report to multiple mirror destinations")
        report_dst = ['4.4.4.3', '4.4.4.4', '4.4.4.5', '4.4.4.6',
                      '4.4.4.7']

        route2 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('4.4.4.4/32'))
        sai_thrift_create_route_entry(self.client, route2,
                                      next_hop_id=self.nhop13)
        route3 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('4.4.4.5/32'))
        sai_thrift_create_route_entry(self.client, route3,
                                      next_hop_id=self.nhop13)
        route4 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('4.4.4.6/32'))
        sai_thrift_create_route_entry(self.client, route4,
                                      next_hop_id=self.nhop13)
        route5 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('4.4.4.7/32'))
        sai_thrift_create_route_entry(self.client, route5,
                                      next_hop_id=self.nhop13)

        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(report_dst[0]),
                          sai_ipaddress(report_dst[1]),
                          sai_ipaddress(report_dst[2]),
                          sai_ipaddress(report_dst[3]),
                          sai_ipaddress(report_dst[4])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress('4.4.4.1'),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        payload = 'flow report'
        try:
            max_itrs = 100
            random.seed(314159)
            mirror_sessions_num = len(report_dst)
            count = [0] * mirror_sessions_num
            mirror_ports = [0] * mirror_sessions_num
            exp_postcard_pkt = [None] * mirror_sessions_num
            ignore_seq_num = True
            ignore_seq_num_sessions_count = 0
            exp_seq_num = [0] * mirror_sessions_num
            mirror_ports = [self.dev_port27] * mirror_sessions_num
            for i in range(0, max_itrs):
                src_port = i + 10000
                dst_port = i + 10001

                pkt = simple_udp_packet(
                    eth_dst=MAC_SELF,
                    eth_src=self.mac_nbr[1],
                    ip_dst=IPADDR_NBR[1],
                    ip_src=IPADDR_NBR[0],
                    ip_id=105,
                    ip_ttl=64,
                    udp_sport=src_port,
                    udp_dport=dst_port,
                    udp_payload=payload)
                exp_pkt = simple_udp_packet(
                    eth_dst=self.mac_nbr[1],
                    eth_src=MAC_SELF,
                    ip_dst=IPADDR_NBR[1],
                    ip_src=IPADDR_NBR[0],
                    ip_id=105,
                    ip_ttl=63,
                    udp_sport=src_port,
                    udp_dport=dst_port,
                    udp_payload=payload)
                for j in range(0, len(report_dst)):
                    fields = {'ingress_port': self.port24_fp,
                              'egress_port': self.port25_fp,
                              'hw_id': 0, 'flow': 1}
                    exp_postcard_pkt[j] = exp_postcard_packet(
                        exp_pkt, self.int_v2, fields)
                    exp_postcard_pkt[j][IP].dst = report_dst[j]
                    if self.int_v2:
                        hdr = DTEL_REPORT_V2_HDR
                    else:
                        hdr = DTEL_REPORT_HDR
                    exp_postcard_pkt[j][hdr].sequence_number = exp_seq_num[j]
                send_packet(self, self.dev_port24, pkt)
                verify_packet(self, exp_pkt, self.dev_port25)
                (rcv_index, seq_num) = verify_any_dtel_packet_any_port(
                    self, exp_postcard_pkt, mirror_ports,
                    ignore_seq_num=ignore_seq_num)
                # print(("%d %d   %d" % (i, rcv_index, seq_num)))
                count[rcv_index] += 1
                if exp_seq_num[rcv_index] == 0:
                    ignore_seq_num_sessions_count += 1
                if ignore_seq_num_sessions_count == mirror_sessions_num:
                    ignore_seq_num = False
                exp_seq_num[rcv_index] = seq_num + 1

            for i in range(0, mirror_sessions_num):
                avg_level = (max_itrs / float(mirror_sessions_num)) * 0.5
                self.assertTrue(
                    (count[i] >= avg_level),
                    "Not all mirror sessions are equally balanced"
                    " (%d < %f (%d%%) for %d" % (count[i], avg_level, 50, i))
                print("mirror session %d count %d" % (i, count[i]))

            print("passed balancing the load among telemetry mirror sessions")

        finally:
            sai_thrift_remove_route_entry(self.client, route2)
            sai_thrift_remove_route_entry(self.client, route3)
            sai_thrift_remove_route_entry(self.client, route4)
            sai_thrift_remove_route_entry(self.client, route5)


class FlowAndEgressPortMirrorTest(DtelBaseTest):
    """ Flow and egress port mirror test class """

    def setUp(self):
        super(FlowAndEgressPortMirrorTest, self).setUp()
        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP,
                        SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

        self.mirror_session = sai_thrift_create_mirror_session(
            self.client,
            monitor_port=self.port27,
            type=SAI_MIRROR_SESSION_TYPE_LOCAL)

        self.pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=63)

    def runTest(self):
        try:
            self.flowReportEnabledEgressMirrorDisabledTest()
            self.flowReportEnabledEgressMirrorEnabledTest()
            self.flowReportDisabledEgressMirrorEnabledTest()
        finally:
            pass

    def tearDown(self):
        obj_list = sai_thrift_object_list_t(count=0, idlist=[])
        sai_thrift_set_port_attribute(
            self.client, self.port25,
            egress_mirror_session=obj_list)
        sai_thrift_remove_mirror_session(self.client, self.mirror_session)
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)

        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(FlowAndEgressPortMirrorTest, self).tearDown()

    def flowReportEnabledEgressMirrorDisabledTest(self):
        '''
        Test verifies correct flow with
        flow report enabled and egress mirror disabled.
        '''
        print("flowReportEnabledEgressMirrorDisabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        obj_list = sai_thrift_object_list_t(count=0, idlist=[])
        sai_thrift_set_port_attribute(
            self.client, self.port25,
            egress_mirror_session=obj_list)

        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)

        print("Sending packet port %d" % self.dev_port24,
              " -> port %d" % self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packet(self, self.exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def flowReportEnabledEgressMirrorEnabledTest(self):
        '''
        Test verifies correct flow with
        flow report enabled and egress mirror enabled.
        '''
        print("flowReportEnabledEgressMirrorEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        obj_list = sai_thrift_object_list_t(count=1,
                                            idlist=[self.mirror_session])
        sai_thrift_set_port_attribute(
            self.client, self.port25,
            egress_mirror_session=obj_list)

        print("Sending packet port %d" % self.dev_port24,
              " -> port %d" % self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packets(self, self.exp_pkt, [self.dev_port25, self.dev_port27])
        verify_no_other_packets(self, timeout=1)

    def flowReportDisabledEgressMirrorEnabledTest(self):
        '''
        Test verifies correct flow with
        flow report disabled and egress mirror enabled.
        '''
        print("flowReportDisabledEgressMirrorEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=False)
        obj_list = sai_thrift_object_list_t(count=1,
                                            idlist=[self.mirror_session])
        sai_thrift_set_port_attribute(
            self.client, self.port25,
            egress_mirror_session=obj_list)

        print("Sending packet port %d" % self.dev_port24,
              " -> port %d" % self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packets(self, self.exp_pkt, [self.dev_port25, self.dev_port27])
        verify_no_other_packets(self, timeout=1)


class QueueReportTest(DtelBaseTest):
    """ Basic Queue Report test class """

    def setUp(self):
        IPADDR_INF[2] = '1.1.0.2'
        IPADDR_NBR[2] = '1.1.0.101'
        super(QueueReportTest, self).setUp()
        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        attr = sai_thrift_get_port_attribute(
            self.client, self.port25, qos_number_of_queues=True)
        qos_number_of_queues = attr['qos_number_of_queues']

        queue_list = sai_thrift_object_list_t(
            count=qos_number_of_queues, idlist=[])
        attr = sai_thrift_get_port_attribute(
            self.client, self.port25, qos_queue_list=queue_list)
        print(attr['SAI_PORT_ATTR_QOS_QUEUE_LIST'])
        queue_idx = attr['SAI_PORT_ATTR_QOS_QUEUE_LIST'].idlist[0]

        self.queue_report = sai_thrift_create_dtel_queue_report(
            self.client,
            queue_id=queue_idx,
            depth_threshold=1000,
            latency_threshold=500000000,
            breach_quota=10,
            tail_drop=True)

        self.pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2, 'queue': 1}
        self.exp_postcard_pkt = exp_postcard_packet(
            self.exp_pkt, self.int_v2, fields)
        sai_thrift_set_dtel_attribute(
            self.client, self.dtel, queue_report_enable=True)

    def runTest(self):
        try:
            self.lowLatencyNoReportTest(self.pkt, self.exp_pkt)
            print("Latency threshold decreased to 10 nanoseconds")
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, self.queue_report, latency_threshold=10)
            self.validQueueReportTest(self.pkt, self.exp_pkt,
                                      self.exp_postcard_pkt)

            print("Latency threshold decreased to 500000000 nanoseconds")
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, self.queue_report, latency_threshold=500000000)
            print("Depth threshold decreased to 0")
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, self.queue_report, depth_threshold=0)
            self.validQueueReportTest(self.pkt, self.exp_pkt,
                                      self.exp_postcard_pkt)

            print("Latency threshold decreased to 10 nanoseconds")
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, self.queue_report, latency_threshold=10)
            print("Depth threshold decreased to 0")
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, self.queue_report, depth_threshold=0)
            self.validQueueReportTest(self.pkt, self.exp_pkt,
                                      self.exp_postcard_pkt)

            print("Queue report enabled but quota is 0")
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, self.queue_report, breach_quota=0)
            self.noQuotaNoReportTest(self.pkt, self.exp_pkt,
                                     self.exp_postcard_pkt)

            print("Quota reset to 2")
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, self.queue_report, breach_quota=2)
            self.queueReportChangeTest(self.pkt, self.exp_pkt,
                                       self.exp_postcard_pkt)

            print("Quota reset to 10")
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, self.queue_report, breach_quota=10)
            self.egressDropQueueReportTest(self.pkt,
                                           self.exp_postcard_pkt)

            sai_thrift_remove_dtel_queue_report(self.client, self.queue_report)

            self.multiMirrorTest()

        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)
        super(QueueReportTest, self).tearDown()
        IPADDR_INF[2] = '172.16.0.4'
        IPADDR_NBR[2] = '172.16.0.1'

    def lowLatencyNoReportTest(self, pkt, exp_pkt):
        '''
        Test verifies no queue report when latency is under threshold

        Args:
            pkt (packet) : packet to be send
            exp_pkt (packet) : packet expected to be received
        '''
        print("LowLatencyNoReportTest")
        print("No report is generated since latency is below configured value")
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)

    def validQueueReportTest(self, pkt, exp_pkt, exp_postcard_pkt):
        '''
        Test verifies valid queue report

        Args:
            pkt (packet) : packet to be send
            exp_pkt (packet) : packet expected to be received
            exp_postcard_pkt (packet) : postcard packet expected to be received
        '''
        print("ValidQueueReportTest")
        print("Queue report is now generated")
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)

    def noQuotaNoReportTest(self, pkt, exp_pkt, exp_postcard_pkt):
        '''
        Test verifies no report when quota is breached

        Args:
            pkt (packet) : packet to be send
            exp_pkt (packet) : packet expected to be received
            exp_postcard_pkt (packet) : postcard packet expected to be received
        '''
        print("NoQuotaNoReportTest")
        print("Queue report is generated first time after resetting quota")
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)

        print("No report is generated since breach quota is zero")
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)

        print("No report is generated since breach quota is zero")
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)

    def queueReportChangeTest(self, pkt, exp_pkt, exp_postcard_pkt):
        '''
        Test verifies queue report when latency changes

        Args:
            pkt (packet) : packet to be send
            exp_pkt (packet) : packet expected to be received
            exp_postcard_pkt (packet) : postcard packet expected to be received
        '''
        print("QueueReportChangeTest")
        sai_thrift_set_dtel_attribute(
            self.client, self.dtel, latency_sensitivity=0)

        print("Queue report is generated when quota is above 0")
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)

        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, pkt)
        verify_packet(self, exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)

        print("Queue reports due to latency change when quota = 0")
        num_packets = 20
        num_postcards_rcvd = 0
        for _ in range(num_packets):
            send_packet(self, self.dev_port24, pkt)
            verify_packet(self, exp_pkt, self.dev_port25)
            # verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            postcard_pkt = receive_postcard_packet(self, exp_postcard_pkt,
                                                   self.dev_port27)
            if postcard_pkt is not None:
                num_postcards_rcvd += 1
        self.assertTrue(num_postcards_rcvd >= num_packets * 0.3,
                        "Not enough postcards received due to latency change: "
                        " %d" % num_postcards_rcvd)

        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      latency_sensitivity=30)

    def egressDropQueueReportTest(self, pkt, exp_postcard_pkt):
        '''
        Test verifies queue report without drop reason when qdepth is above
        threshold and packet is dropped in the egress pipeline

        Args:
            pkt (packet) : packet to be send
            exp_postcard_pkt (packet) : postcard packet expected to be received
        '''
        print("EggressDropQueueReportTest")
        # create ACL table
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_EGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        sai_thrift_set_port_attribute(self.client, self.port24,
                                      egress_acl=acl_table_id)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      egress_acl=acl_table_id)

        try:
            print("Sending packet port %d" % self.dev_port24, " dropped")
            send_packet(self, self.dev_port24, pkt)
            verify_postcard_packet(self, exp_postcard_pkt,
                                   self.dev_port27)
            verify_no_other_packets(self, timeout=1)

        finally:
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          egress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          egress_acl=0)
            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)

    def multiMirrorTest(self):
        '''
        Test verifies queue reports distributed among multiple
        destination IP adresses in report session.
        '''
        print("multiMirrorTest")
        attr = sai_thrift_get_port_attribute(
            self.client, self.port25, qos_number_of_queues=True)
        qos_number_of_queues = attr['qos_number_of_queues']

        queue_list = sai_thrift_object_list_t(
            count=qos_number_of_queues, idlist=[])
        attr = sai_thrift_get_port_attribute(
            self.client, self.port25, qos_queue_list=queue_list)
        print(attr['SAI_PORT_ATTR_QOS_QUEUE_LIST'])
        queue_idx = attr['SAI_PORT_ATTR_QOS_QUEUE_LIST'].idlist[0]

        queue_report = sai_thrift_create_dtel_queue_report(
            self.client,
            queue_id=queue_idx,
            depth_threshold=0,
            latency_threshold=0,
            breach_quota=1024,
            tail_drop=True)

        print("Test queue report to multiple mirror destinations")
        report_dst = ['4.4.4.3', '4.4.4.4', '4.4.4.5', '4.4.4.6',
                      '4.4.4.7']

        route2 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('4.4.4.4/32'))
        sai_thrift_create_route_entry(self.client, route2,
                                      next_hop_id=self.nhop13)
        route3 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('4.4.4.5/32'))
        sai_thrift_create_route_entry(self.client, route3,
                                      next_hop_id=self.nhop13)
        route4 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('4.4.4.6/32'))
        sai_thrift_create_route_entry(self.client, route4,
                                      next_hop_id=self.nhop13)
        route5 = sai_thrift_route_entry_t(
            vr_id=self.vrf, destination=sai_ipprefix('4.4.4.7/32'))
        sai_thrift_create_route_entry(self.client, route5,
                                      next_hop_id=self.nhop13)

        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(report_dst[0]),
                          sai_ipaddress(report_dst[1]),
                          sai_ipaddress(report_dst[2]),
                          sai_ipaddress(report_dst[3]),
                          sai_ipaddress(report_dst[4])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress('4.4.4.1'),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        payload = 'q report'
        try:
            max_itrs = 100
            random.seed(314159)
            mirror_sessions_num = len(report_dst)
            count = [0] * mirror_sessions_num
            mirror_ports = [0] * mirror_sessions_num
            exp_postcard_pkt = [None] * mirror_sessions_num
            ignore_seq_num = True
            ignore_seq_num_sessions_count = 0
            exp_seq_num = [0] * mirror_sessions_num
            mirror_ports = [self.dev_port27] * mirror_sessions_num
            for i in range(0, max_itrs):
                src_port = i + 10000
                dst_port = i + 10001

                pkt = simple_udp_packet(
                    eth_dst=MAC_SELF,
                    eth_src=self.mac_nbr[1],
                    ip_dst=IPADDR_NBR[1],
                    ip_src=IPADDR_NBR[0],
                    ip_id=105,
                    ip_ttl=64,
                    udp_sport=src_port,
                    udp_dport=dst_port,
                    udp_payload=payload)
                exp_pkt = simple_udp_packet(
                    eth_dst=self.mac_nbr[1],
                    eth_src=MAC_SELF,
                    ip_dst=IPADDR_NBR[1],
                    ip_src=IPADDR_NBR[0],
                    ip_id=105,
                    ip_ttl=63,
                    udp_sport=src_port,
                    udp_dport=dst_port,
                    udp_payload=payload)
                for j in range(0, len(report_dst)):
                    fields = {'ingress_port': self.port24_fp,
                              'egress_port': self.port25_fp,
                              'hw_id': 0, 'queue': 1}
                    exp_postcard_pkt[j] = exp_postcard_packet(
                        exp_pkt, self.int_v2, fields)
                    exp_postcard_pkt[j][IP].dst = report_dst[j]
                    if self.int_v2:
                        hdr = DTEL_REPORT_V2_HDR
                    else:
                        hdr = DTEL_REPORT_HDR
                    exp_postcard_pkt[j][hdr].sequence_number = exp_seq_num[j]
                send_packet(self, self.dev_port24, pkt)
                verify_packet(self, exp_pkt, self.dev_port25)
                (rcv_index, seq_num) = verify_any_dtel_packet_any_port(
                    self, exp_postcard_pkt, mirror_ports,
                    ignore_seq_num=ignore_seq_num)
                # print(("%d %d   %d" % (i, rcv_index, seq_num)))
                count[rcv_index] += 1
                if exp_seq_num[rcv_index] == 0:
                    ignore_seq_num_sessions_count += 1
                if ignore_seq_num_sessions_count == mirror_sessions_num:
                    ignore_seq_num = False
                exp_seq_num[rcv_index] = seq_num + 1

            for i in range(0, mirror_sessions_num):
                avg_level = (max_itrs / float(mirror_sessions_num)) * 0.5
                self.assertTrue(
                    (count[i] >= avg_level),
                    "Not all mirror sessions are equally balanced"
                    " (%d < %f (%d%%) for %d" % (count[i], avg_level, 50, i))
                print("mirror session %d count %d" % (i, count[i]))

            print("passed balancing the load among telemetry mirror sessions")

        finally:
            sai_thrift_remove_dtel_queue_report(self.client, queue_report)
            sai_thrift_remove_route_entry(self.client, route2)
            sai_thrift_remove_route_entry(self.client, route3)
            sai_thrift_remove_route_entry(self.client, route4)
            sai_thrift_remove_route_entry(self.client, route5)


# Please remember to restart the model if this test is run
@group('dtel-restart')
class DropSuppressionTest(DtelBaseTest):
    """ Drop Suppression test class """

    def runTest(self):
        print("DropSuppressionTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            drop_report_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable)

        # create ACL table
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        payload = '@!#?'
        pkt = simple_udp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=108,
            ip_ttl=64,
            with_udp_chksum=True,
            udp_payload=payload)

        # SWITCH_DROP_REASON_ACL_DROP 80
        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': 80}
        exp_mod_pkt = exp_mod_packet(pkt, self.int_v2, fields)
        sai_thrift_set_port_attribute(self.client, self.port24,
                                      ingress_acl=acl_table_id)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      ingress_acl=acl_table_id)

        try:
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass - drop report generated")

            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            print("pass - no drop report generated")

            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_src_ip=src_ip_t,
                field_dst_ip=dst_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)

            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass - drop report generated")

        finally:
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          ingress_acl=0)

            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)
            sai_thrift_remove_dtel_report_session(self.client,
                                                  dtel_report_session)
            sai_thrift_remove_dtel(self.client, dtel)


# Please remember to restart the model if this test is run
@group('dtel-restart')
class EgressDropSuppressionTest(DtelBaseTest):
    """ Egres Drop Suppression test class """

    def runTest(self):
        print("EgressDropSuppressionTest")

        bind_mirror_on_drop_pkt(self.int_v2)

        dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            drop_report_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable)

        # create ACL table
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_EGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        payload = '@!#?'
        pkt = simple_udp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=108,
            ip_ttl=64,
            with_udp_chksum=True,
            udp_payload=payload)

        exp_pkt = simple_udp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            with_udp_chksum=True,
            ip_id=108,
            ip_ttl=63,
            udp_payload=payload)

        # SWITCH_DROP_REASON_EGRESS_ACL_DROP 92
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4,
                  'drop_reason': 92, 'queue': 0}
        exp_mod_pkt = exp_egress_mod_packet(exp_pkt, self.int_v2, fields)
        sai_thrift_set_port_attribute(self.client, self.port24,
                                      egress_acl=acl_table_id)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      egress_acl=acl_table_id)

        try:
            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass - drop report generated")

            send_packet(self, self.dev_port24, pkt)
            verify_no_other_packets(self, timeout=1)
            print("pass - no drop report generated")

            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)

            dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=dtel_acl_table_id,
                priority=10,
                field_src_ip=src_ip_t,
                field_dst_ip=dst_ip_t,
                action_dtel_drop_report_enable=enable,
                action_dtel_report_all_packets=enable)

            send_packet(self, self.dev_port24, pkt)
            verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
            verify_no_other_packets(self, timeout=1)
            print("pass - drop report generated")

        finally:
            sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port24,
                                          egress_acl=0)
            sai_thrift_set_port_attribute(self.client, self.port25,
                                          egress_acl=0)

            sai_thrift_remove_acl_entry(self.client, acl_entry_id)
            sai_thrift_remove_acl_table(self.client, acl_table_id)
            sai_thrift_remove_acl_entry(self.client, dtel_acl_entry_id)
            sai_thrift_remove_acl_table(self.client, dtel_acl_table_id)
            sai_thrift_remove_dtel_report_session(self.client,
                                                  dtel_report_session)
            sai_thrift_remove_dtel(self.client, dtel)


# Please remember to restart the model if this test is run
@group('dtel-restart')
class FlowSuppressionTest(DtelBaseTest):
    """ Flow Report Suppression test class """

    def setUp(self):
        super(FlowSuppressionTest, self).setUp()
        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            postcard_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1000,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op)

        self.pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        self.exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        self.exp_postcard_pkt = exp_postcard_packet(self.exp_pkt_out,
                                                    self.int_v2, fields)

    def runTest(self):
        try:
            self.simpleSuppressionTest()
            self.suppressionPathChangeTest()
            self.suppressionLatencyChangeTest()
            # Redo SimpleSuppressionTest to catch last latency change
            self.simpleSuppressionTest()
            self.suppressionTCPTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)
        super(FlowSuppressionTest, self).tearDown()

    def simpleSuppressionTest(self):
        ''' Test verifies simple suppression '''
        print("simpleSuppressionTest")

        print("Valid report is generated for %s" % IPADDR_NBR[1])
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, self.exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should not be generated again for %s" %
              IPADDR_NBR[1])
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_no_other_packets(self, timeout=1)

    def suppressionPathChangeTest(self):
        ''' Test verifies suppression after path change '''
        print("suppressionPathChangeTest")

        print("Send identical packet from a different port")
        fields = {'ingress_port': self.port26_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt2 = exp_postcard_packet(self.exp_pkt_out, self.int_v2,
                                                fields)

        print("Sending packet port %d" % self.dev_port26, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port26, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should not be generated again for %s" %
              IPADDR_NBR[1])
        print("Sending packet port %d" % self.dev_port26, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port26, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)

        print("Send identical packet from original port")
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, self.exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should not be generated again for %s" %
              IPADDR_NBR[1])
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)

    def suppressionLatencyChangeTest(self):
        ''' Test verifies supression after latency change '''
        print("suppressionLatencyChangeTest")

        print("Send identical packet from original port")
        print("Report should not be generated again for %s" %
              IPADDR_NBR[1])
        print("Sending packet port %d" % self.dev_port24, " -> port %d" %
              self.dev_port25)
        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)

        print("Change latency sensitivity to 0")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      latency_sensitivity=0)

        print("Flow reports due to latency change when quota = 0")
        num_packets = 20
        num_postcards_rcvd = 0
        for _ in range(num_packets):
            send_packet(self, self.dev_port24, self.pkt_in)
            verify_packet(self, self.exp_pkt_out, self.dev_port25)
            postcard_pkt = receive_postcard_packet(self, self.exp_postcard_pkt,
                                                   self.dev_port27)
            if postcard_pkt is not None:
                num_postcards_rcvd += 1
        self.assertTrue(num_postcards_rcvd >= num_packets * 0.30,
                        "Not enough postcards received due to latency change:"
                        " %d" % num_postcards_rcvd)

        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      latency_sensitivity=30)

    def suppressionTCPTest(self):
        ''' Test verifies supression for different TCP flags '''
        print("suppressionTCPTest")

        print("Send SYN packet, should generate a report")
        self.pkt_in[TCP].flags = "S"
        self.exp_pkt_out[TCP].flags = "S"
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt2 = exp_postcard_packet(self.exp_pkt_out, self.int_v2,
                                                fields)

        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("Send packet with tcp_flags=0, should not generate any report")
        self.pkt_in[TCP].flags = 0
        self.exp_pkt_out[TCP].flags = 0
        exp_postcard_pkt2 = exp_postcard_packet(self.exp_pkt_out, self.int_v2,
                                                fields)

        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_no_other_packets(self, timeout=1)

        print("Send RST packet, should generate a report")

        self.pkt_in[TCP].flags = "R"
        self.exp_pkt_out[TCP].flags = "R"
        exp_postcard_pkt2 = exp_postcard_packet(self.exp_pkt_out, self.int_v2,
                                                fields)

        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("Send packet with tcp_flags=P, should not generate any report")
        self.pkt_in[TCP].flags = 0
        self.exp_pkt_out[TCP].flags = 0
        exp_postcard_pkt2 = exp_postcard_packet(self.exp_pkt_out, self.int_v2,
                                                fields)

        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_no_other_packets(self, timeout=1)

        print("Send FIN packet, should generate a report")

        self.pkt_in[TCP].flags = "F"
        self.exp_pkt_out[TCP].flags = "F"
        exp_postcard_pkt2 = exp_postcard_packet(self.exp_pkt_out, self.int_v2,
                                                fields)

        send_packet(self, self.dev_port24, self.pkt_in)
        verify_packet(self, self.exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt2, self.dev_port27)
        verify_no_other_packets(self, timeout=1)


class FlowAndMoDReportTest(DtelBaseTest):
    """ Test class for Flow Report and MoD Report tests """

    def setUp(self):
        super(FlowAndMoDReportTest, self).setUp()
        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP,
                        SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        self.src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        self.dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        self.enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        self.dtel_acl_entry_id = None
        self.acl_table_id = 0
        self.acl_entry_id = 0
        self.pkt = 0
        self.exp_pkt = 0

    def _configureIngressAcl(self):
        ''' Helper function to configure ingress ACL. '''
        # create ACL table
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        self.acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        self.acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        sai_thrift_set_port_attribute(self.client, self.port24,
                                      ingress_acl=self.acl_table_id)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      ingress_acl=self.acl_table_id)

        self.pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=63)

    def _cleanUpIngressAcl(self):
        ''' Helper function to cleanUp ingress ACL. '''
        sai_thrift_set_port_attribute(self.client, self.port24,
                                      ingress_acl=0)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      ingress_acl=0)

        sai_thrift_remove_acl_entry(self.client, self.acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.acl_table_id)

    def _configureEgressAcl(self):
        ''' Helper function to configure egress ACL. '''
        # create ACL table
        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        self.acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_EGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        self.acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)

        sai_thrift_set_port_attribute(self.client, self.port24,
                                      egress_acl=self.acl_table_id)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      egress_acl=self.acl_table_id)

        pktlen = 256
        payload = '@!#?'
        self.pkt = simple_udp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=108,
            ip_ttl=64,
            with_udp_chksum=True,
            udp_payload=payload,
            pktlen=pktlen)

        self.exp_pkt = simple_udp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            with_udp_chksum=True,
            ip_id=108,
            ip_ttl=63,
            udp_payload=payload,
            pktlen=pktlen)

    def _cleanUpEgressAcl(self):
        ''' Helper function to cleanUp egress ACL. '''
        sai_thrift_set_port_attribute(self.client, self.port24,
                                      egress_acl=0)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      egress_acl=0)

        sai_thrift_remove_acl_entry(self.client, self.acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.acl_table_id)

    def _create_action_acl_dtel_flow_op(self, flow_op=None):
        acl_dtel_flow_op = None
        if flow_op:
            acl_dtel_flow_op = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=flow_op))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_src_ip=self.src_ip_t,
            field_dst_ip=self.dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=self.enable,
            action_dtel_drop_report_enable=self.enable)

    def _remove_action_acl_dtel_flow_op(self):
        if not (sai_thrift_remove_acl_entry(self.client,
                self.dtel_acl_entry_id)):
            self.dtel_acl_entry_id = None

    def _ingressReportFlowOpPostcardTests(self):
        self._create_action_acl_dtel_flow_op(SAI_ACL_DTEL_FLOW_OP_POSTCARD)
        self.ingressDropReportEnabledFlowReportDisabledTest()
        self.ingressDropReportEnabledFlowReportEnabledTest()
        self.ingressDropReportDisabledFlowReportEnabledTest()
        self._remove_action_acl_dtel_flow_op()

    def _ingressReportFlowOpNopTests(self):
        self._create_action_acl_dtel_flow_op(SAI_ACL_DTEL_FLOW_OP_NOP)
        self.ingressDropReportEnabledFlowReportEnabledFlowOpNopTest()
        self._remove_action_acl_dtel_flow_op()

    def _ingressReportTests(self):
        self._create_action_acl_dtel_flow_op()
        self.ingressDropReportEnabledFlowReportEnabledFlowOpNoneTest()
        self._remove_action_acl_dtel_flow_op()

    def _egressReportFlowOpPostcardTests(self):
        self._create_action_acl_dtel_flow_op(SAI_ACL_DTEL_FLOW_OP_POSTCARD)
        self.egressDropReportEnabledFlowReportDisabledTest()
        self.egressDropReportEnabledFlowReportEnabledTest()
        self.egressDropReportDisabledFlowReportEnabledTest()
        self._remove_action_acl_dtel_flow_op()

    def _egressReportFlowOpNopTests(self):
        self._create_action_acl_dtel_flow_op(SAI_ACL_DTEL_FLOW_OP_NOP)
        self.egressDropReportEnabledFlowReportEnabledFlowOpNopTest()
        self._remove_action_acl_dtel_flow_op()

    def _egressReportTests(self):
        self._create_action_acl_dtel_flow_op()
        self.egressDropReportEnabledFlowReportEnabledFlowOpNoneTest()
        self._remove_action_acl_dtel_flow_op()

    def _validFlowReportFlowOpPostcardTests(self):
        self._create_action_acl_dtel_flow_op(SAI_ACL_DTEL_FLOW_OP_POSTCARD)
        self.validFlowReportDropReportDisableFlowReportEnabledTest()
        self.validFlowReportDropReportEnabledFlowReportEnabledTest()
        self.validFlowReportFlowReportEnabledTest()
        self._remove_action_acl_dtel_flow_op()

    def _validFlowReportFlowOpNopTests(self):
        self._create_action_acl_dtel_flow_op(SAI_ACL_DTEL_FLOW_OP_NOP)
        self.validFlowReportDropReportEnabledFlowReportEnabledFlowopNopTest()
        self._remove_action_acl_dtel_flow_op()

    def _validFlowReportTests(self):
        self._create_action_acl_dtel_flow_op()
        self.validFlowReportDropReportEnabledFlowReportEnabledFlowopNoneTest()
        self._remove_action_acl_dtel_flow_op()

    def runTest(self):
        try:
            self._configureIngressAcl()
            # Ingress ACL tests

            self._ingressReportFlowOpPostcardTests()
            self._ingressReportFlowOpNopTests()
            self._ingressReportTests()

            self._cleanUpIngressAcl()
            self._configureEgressAcl()
            # Egress ACL tests

            self._egressReportFlowOpPostcardTests()
            self._egressReportFlowOpNopTests()
            self._egressReportTests()

            self._cleanUpEgressAcl()

            # Verifies valid flow report generation tests
            self._validFlowReportFlowOpPostcardTests()
            self._validFlowReportFlowOpNopTests()
            self._validFlowReportTests()

        finally:
            pass

    def tearDown(self):
        if self.dtel_acl_entry_id:
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(FlowAndMoDReportTest, self).tearDown()

    def validFlowReportFlowReportEnabledTest(self):
        ''' Test verifies valid flow report generation when flow
            report is enabled and flow operations is postcard
        '''
        print("validFlowReportFlowReportEnabledTest")
        pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                               fields)

        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        print("Reports for flow that matches dtel_acl")
        print("Valid report should be generated for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should be generated again for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def validFlowReportDropReportDisableFlowReportEnabledTest(self):
        ''' Test verifies valid flow report generation when flow
            report is enabled and drop report is disabled and
            flow operations is postcard
        '''
        print("validFlowReportDropReportDisableFlowReportEnabledTest")
        pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                               fields)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=False)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        print("Reports for flow that matches dtel_acl")
        print("Valid report should be generated for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should be generated again for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def validFlowReportDropReportEnabledFlowReportEnabledTest(self):
        ''' Test verifies valid flow report generation when flow
            report is enabled and drop report is enabled and
            flow operations is postcard
        '''
        print("validFlowReportDropReportEnabledFlowReportEnabledTest")
        pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                               fields)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        print("Reports for flow that matches dtel_acl")
        print("Valid report should be generated for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should be generated again for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def validFlowReportDropReportEnabledFlowReportEnabledFlowopNopTest(self):
        ''' Test verifies valid flow report generation when flow
            report is enabled and drop report is enabled and
            flow operations is nop
        '''
        print("validFlowReportDropReportEnabledFlowReportEnabledFlowopNopTest")
        pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)

        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                               fields)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        print("Reports for flow that matches dtel_acl")
        print("Valid report should be generated for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        print("verify_packet")
        verify_no_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should be generated again for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_no_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def validFlowReportDropReportEnabledFlowReportEnabledFlowopNoneTest(self):
        ''' Test verifies valid flow report generation when flow
            report is enabled and drop report is enabled
        '''
        print("validFlowReportDropReportEnabledFlowReportEnabledFlowopNone\
              Test")
        pkt_in = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[0],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            tcp_flags=None)
        exp_pkt_out = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            tcp_flags=None,
            ip_ttl=63)

        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 1}
        exp_postcard_pkt = exp_postcard_packet(exp_pkt_out, self.int_v2,
                                               fields)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        print("Reports for flow that matches dtel_acl")
        print("Valid report should be generated for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        print("verify_packet")
        verify_no_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)
        print("Report should be generated again for %s" % IPADDR_NBR[1])
        send_packet(self, self.dev_port24, pkt_in)
        verify_packet(self, exp_pkt_out, self.dev_port25)
        verify_no_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def ingressDropReportEnabledFlowReportDisabledTest(self):
        '''
        Test verifies ingress reports generation when drop report is enabled
        and flow report is disabled
        '''
        print("IngressDropReportEnabledFlowReportDisabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        # SWITCH_DROP_REASON_ACL_DROP 80
        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': 80}
        exp_mod_pkt = exp_mod_packet(self.pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def ingressDropReportEnabledFlowReportEnabledTest(self):
        '''
        Test verifies inress reports generation when drop report is enabled
        and flow report is enabled
        '''
        print("IngressDropReportEnabledFlowReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        # SWITCH_DROP_REASON_ACL_DROP 80
        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': 80, 'flow': 1}
        exp_mod_pkt = exp_mod_packet(self.pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def ingressDropReportEnabledFlowReportEnabledFlowOpNopTest(self):
        '''
        Test verifies inress reports generation when drop report is enabled
        and flow report is enabled and flow operations is nop
        '''
        print("ingressDropReportEnabledFlowReportEnabledFlowOpNopTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': 80}
        exp_mod_pkt = exp_mod_packet(self.pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def ingressDropReportEnabledFlowReportEnabledFlowOpNoneTest(self):
        '''
        Test verifies inress reports generation when drop report is enabled
        and flow report is enabled
        '''
        print("ingressDropReportEnabledFlowReportEnabledFlowOpNoneTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        fields = {'ingress_port': self.port24_fp, 'hw_id': 4,
                  'drop_reason': 80}
        exp_mod_pkt = exp_mod_packet(self.pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def ingressDropReportDisabledFlowReportEnabledTest(self):
        '''
        Test verifies ingress reports generation when drop report is disabled
        and flow report is enabled
        '''
        print("IngressDropReportDisabledFlowReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=False)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        send_packet(self, self.dev_port24, self.pkt)
        verify_no_other_packets(self, timeout=1)

    def egressDropReportEnabledFlowReportDisabledTest(self):
        '''
        Test verifies ingress reports generation when drop report is enabled
        and flow report is disabled
        '''
        print("EgressDropReportEnabledFlowReportDisabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=False)
        # SWITCH_DROP_REASON_EGRESS_ACL_DROP 92
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4, 'drop_reason': 92}
        exp_mod_pkt = exp_egress_mod_packet(self.exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def egressDropReportEnabledFlowReportEnabledTest(self):
        '''
        Test verifies egress reports generation when drop report is enabled
        and flow report is enabled
        '''
        print("EgressDropReportEnabledFlowReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        # SWITCH_DROP_REASON_EGRESS_ACL_DROP 92
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4,
                  'drop_reason': 92, 'flow': 1}
        exp_mod_pkt = exp_egress_mod_packet(self.exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def egressDropReportEnabledFlowReportEnabledFlowOpNopTest(self):
        '''
        Test verifies egress reports generation when drop report is enabled
        and flow operations is nop
        '''
        print("egressDropReportEnabledFlowReportEnabledFlowOpNopTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4,
                  'drop_reason': 92}
        exp_mod_pkt = exp_egress_mod_packet(self.exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def egressDropReportEnabledFlowReportEnabledFlowOpNoneTest(self):
        '''
        Test verifies egress reports generation when drop report is enabled
        and flow report is enabled and flow operations is nop
        '''
        print("egressDropReportEnabledFlowReportEnabledFlowOpNoneTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)

        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4,
                  'drop_reason': 92}
        exp_mod_pkt = exp_egress_mod_packet(self.exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def egressDropReportDisabledFlowReportEnabledTest(self):
        '''
        Test verifies egress reports generation when drop report is disabled
        and flow report is enabled
        '''
        print("EgressDropReportDisabledFlowReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=False)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        send_packet(self, self.dev_port24, self.pkt)
        verify_no_other_packets(self, timeout=1)


class FlowAndQueueReportTest(DtelBaseTest):
    """ Test class for Flow Report and Queue Report tests """

    def setUp(self):
        super(FlowAndQueueReportTest, self).setUp()
        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            latency_sensitivity=30)

        attr = sai_thrift_get_port_attribute(
            self.client, self.port25, qos_number_of_queues=True)
        qos_number_of_queues = attr['qos_number_of_queues']

        queue_list = sai_thrift_object_list_t(
            count=qos_number_of_queues, idlist=[])
        attr = sai_thrift_get_port_attribute(
            self.client, self.port25, qos_queue_list=queue_list)
        queue_idx = attr['SAI_PORT_ATTR_QOS_QUEUE_LIST'].idlist[0]

        self.queue_report = sai_thrift_create_dtel_queue_report(
            self.client,
            queue_id=queue_idx,
            depth_threshold=0,
            latency_threshold=10,
            breach_quota=10,
            tail_drop=True)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        self.pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=63)

    def runTest(self):
        try:
            self.queueReportEnabledFlowReportDisabledTest()
            self.queueReportEnabledFlowReportEnabledTest()
            self.queueReportDisabledFlowReportEnabledTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel_queue_report(self.client, self.queue_report)
        sai_thrift_remove_dtel(self.client, self.dtel)
        super(FlowAndQueueReportTest, self).tearDown()

    def queueReportEnabledFlowReportDisabledTest(self):
        '''
        Verify flow with enabled Queue Report and
        disabled Flow Report.
        '''
        print("queueReportEnabledFlowReportDisabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=False)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      queue_report_enable=True)
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, breach_quota=1)

        print("Queue report is generated when quota is 1")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2, 'queue': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packet(self, self.exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("No report when quota is 0")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2, 'queue': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packet(self, self.exp_pkt, self.dev_port25)
        verify_no_other_packets(self, timeout=1)

    def queueReportEnabledFlowReportEnabledTest(self):
        '''
        Verify flow with enabled Queue Report and
        enabled Flow Report.
        '''
        print("queueReportEnabledFlowReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      queue_report_enable=True)
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, breach_quota=1)

        print("Queue report is generated when quota is 1")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2,
                  'queue': 1, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packet(self, self.exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("No report when quota is 0")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packet(self, self.exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("Set high queue report thresholds")
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, breach_quota=10)
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, depth_threshold=1000)
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, latency_threshold=500000000)
        print("Report with cleared q bit is generated")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packet(self, self.exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def queueReportDisabledFlowReportEnabledTest(self):
        '''
        Verify flow with disabled Queue Report and
        enabled Flow Report.
        '''
        print("queueReportDisabledFlowReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      queue_report_enable=False)
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, breach_quota=1)

        print("Queue report is generated when quota is 1")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2,
                  'queue': 0, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packet(self, self.exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("No report when quota is 0")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2,
                  'queue': 0, 'flow': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_packet(self, self.exp_pkt, self.dev_port25)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)


class QueueAndMoDReportTest(DtelBaseTest):
    """ Test class for Queue Report and MoD Report tests """

    def setUp(self):
        super(QueueAndMoDReportTest, self).setUp()
        bind_postcard_pkt(self.int_v2)
        bind_mirror_on_drop_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            latency_sensitivity=30)

        attr = sai_thrift_get_port_attribute(
            self.client, self.port25, qos_number_of_queues=True)
        qos_number_of_queues = attr['qos_number_of_queues']

        queue_list = sai_thrift_object_list_t(
            count=qos_number_of_queues, idlist=[])
        attr = sai_thrift_get_port_attribute(
            self.client, self.port25, qos_queue_list=queue_list)
        queue_idx = attr['SAI_PORT_ATTR_QOS_QUEUE_LIST'].idlist[0]

        self.queue_report = sai_thrift_create_dtel_queue_report(
            self.client,
            queue_id=queue_idx,
            depth_threshold=0,
            latency_threshold=10,
            breach_quota=10,
            tail_drop=True)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_report_all_packets=enable,
            action_dtel_drop_report_enable=enable)

        self.pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=63)

        bind_points = [SAI_ACL_BIND_POINT_TYPE_PORT]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        self.acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_EGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True)

        # create ACL entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        packet_action = sai_thrift_acl_action_data_t(
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_PACKET_ACTION_DROP))
        self.acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            action_packet_action=packet_action)
        sai_thrift_set_port_attribute(self.client, self.port24,
                                      egress_acl=self.acl_table_id)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      egress_acl=self.acl_table_id)

    def runTest(self):
        try:
            self.queueReportEnabledDropReportDisabledTest()
            self.queueReportEnabledDropReportEnabledTest()
            self.queueReportDisabledDropReportEnabledTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_set_port_attribute(self.client, self.port24,
                                      egress_acl=0)
        sai_thrift_set_port_attribute(self.client, self.port25,
                                      egress_acl=0)

        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_entry(self.client, self.acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.acl_table_id)
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel_queue_report(self.client, self.queue_report)
        sai_thrift_remove_dtel(self.client, self.dtel)
        super(QueueAndMoDReportTest, self).tearDown()

    def queueReportEnabledDropReportDisabledTest(self):
        '''
        Verify flow with enabled Queue Report and
        disabled Drop Report.
        '''
        print("queueReportEnabledDropReportDisabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=False)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      queue_report_enable=True)
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, breach_quota=1)

        print("Queue report is generated when quota is 1")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2, 'queue': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("No report when quota is 0")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 2, 'queue': 1}
        exp_postcard_pkt = exp_postcard_packet(self.exp_pkt, self.int_v2,
                                               fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_no_other_packets(self, timeout=1)

    def queueReportEnabledDropReportEnabledTest(self):
        '''
        Verify flow with enabled Queue Report and
        enabled Drop Report.
        '''
        print("queueReportEnabledDropReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      queue_report_enable=True)
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, breach_quota=1)

        print("Queue report is generated when quota is 1")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        # SWITCH_DROP_REASON_EGRESS_ACL_DROP 92
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4,
                  'drop_reason': 92, 'queue': 1}
        exp_mod_pkt = exp_egress_mod_packet(self.exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("No report when quota is 0")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        # SWITCH_DROP_REASON_EGRESS_ACL_DROP 92
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4,
                  'drop_reason': 92, 'queue': 0}
        exp_mod_pkt = exp_egress_mod_packet(self.exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

    def queueReportDisabledDropReportEnabledTest(self):
        '''
        Verify flow with disabled Queue Report and
        enabled Drop Report.
        '''
        print("queueReportDisbledDropReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      queue_report_enable=False)
        sai_thrift_set_dtel_queue_report_attribute(
            self.client, self.queue_report, breach_quota=1)

        print("Queue report is generated when quota is 1")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        # SWITCH_DROP_REASON_EGRESS_ACL_DROP 92
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4,
                  'drop_reason': 92, 'queue': 0}
        exp_mod_pkt = exp_egress_mod_packet(self.exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)

        print("No report when quota is 0")
        print("Sending packet port %d " % self.dev_port24, " -> port %d" %
              self.dev_port25)
        # SWITCH_DROP_REASON_EGRESS_ACL_DROP 92
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 4,
                  'drop_reason': 92, 'queue': 0}
        exp_mod_pkt = exp_egress_mod_packet(self.exp_pkt, self.int_v2, fields)
        send_packet(self, self.dev_port24, self.pkt)
        verify_dtel_packet(self, exp_mod_pkt, self.dev_port27)
        verify_no_other_packets(self, timeout=1)


# The below test is only intended to run on the model.
# Remember to run with --dod-test-mode.
@group('dtel-restart')
class QueueReportDoDTest(DtelBaseTest):
    """ Basic Queue Report DoD test class """

    def setUp(self):
        super(QueueReportDoDTest, self).setUp()

        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            latency_sensitivity=30,
            queue_report_enable=True)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

    def runTest(self):
        try:
            attr = sai_thrift_get_port_attribute(
                self.client, self.port25, qos_number_of_queues=True)
            qos_number_of_queues = attr['qos_number_of_queues']

            queue_list = sai_thrift_object_list_t(
                count=qos_number_of_queues, idlist=[])
            attr = sai_thrift_get_port_attribute(
                self.client, self.port25, qos_queue_list=queue_list)
            print(attr['SAI_PORT_ATTR_QOS_QUEUE_LIST'])
            queue_idx = attr['SAI_PORT_ATTR_QOS_QUEUE_LIST'].idlist[0]

            queue_report = sai_thrift_create_dtel_queue_report(
                self.client,
                queue_id=queue_idx,
                depth_threshold=1000,
                latency_threshold=500000000,
                breach_quota=10,
                tail_drop=False)

            pktlen = 256
            pkt = simple_tcp_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[1],
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=105,
                ip_ttl=64,
                pktlen=pktlen)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.mac_nbr[1],
                eth_src=MAC_SELF,
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=105,
                ip_ttl=63,
                pktlen=pktlen)

            dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                          self.dev_port27, pkt, exp_pkt, False)
            print("No queue report when tail drop is not enabled")

            fields_dod = {'ingress_port': self.port24_fp,
                          'egress_port': self.port25_fp, 'hw_id': 0,
                          'queue': 1, 'drop': 0}
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, queue_report, tail_drop=True)
            dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                          self.dev_port27, pkt, exp_pkt, True,
                          exp_dod_packet(pkt, self.int_v2, fields_dod))
            print("Queue report is now generated for tail dropped packet")

            print("Change DTEL report truncate size to 128")
            truncate_size = 128
            sai_thrift_set_dtel_report_session_attribute(
                self.client, self.dtel_report_session,
                truncate_size=truncate_size)

            if self.architecture == 'tofino':
                drop_truncate_adjust = 0
            else:
                if self.int_v2:
                    # 8 is due to telem report header size diff from sw local
                    drop_truncate_adjust = 8
                else:
                    # 4 is due to telem report header size diff from sw local
                    drop_truncate_adjust = 4

            truncated_amount = pktlen - (truncate_size + drop_truncate_adjust)
            exp_dod_pkt_full = exp_dod_packet(pkt, self.int_v2, fields_dod)
            exp_dod_pkt = Ether(bytes(exp_dod_pkt_full)[:-truncated_amount])
            exp_dod_pkt[IP].len -= truncated_amount  # noqa pylint: disable=no-member
            exp_dod_pkt[UDP].len -= truncated_amount  # noqa pylint: disable=no-member
            dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                          self.dev_port27, pkt, exp_pkt, True, exp_dod_pkt)
            print("Truncated queue report is generated " +
                  "for tail dropped packet")

            sai_thrift_set_dtel_queue_report_attribute(
                self.client, queue_report, breach_quota=0)
            dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                          self.dev_port27, pkt, exp_pkt, True)
            print("No queue report when quota is zero")

            sai_thrift_set_dtel_report_session_attribute(
                self.client, self.dtel_report_session,
                truncate_size=512)

            # Now run tests with interleaved tail drop reports amongst
            # normal queue reports
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, queue_report, breach_quota=19)
            sai_thrift_set_dtel_queue_report_attribute(
                self.client, queue_report, depth_threshold=0)
            fields = {'ingress_port': self.port24_fp,
                      'egress_port': self.port25_fp, 'hw_id': 1, 'queue': 1}
            dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                          self.dev_port27, pkt, exp_pkt, True,
                          exp_dod_packet(pkt, self.int_v2, fields_dod),
                          exp_e2e_pkt=exp_postcard_packet(exp_pkt, self.int_v2,
                                                          fields),
                          exp_report_port=self.dev_port27)
            print("Queue report is generated when quota is not zero")

            dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                          self.dev_port27, pkt, exp_pkt, True,
                          exp_e2e_pkt=exp_postcard_packet(exp_pkt, self.int_v2,
                                                          fields),
                          exp_report_port=self.dev_port27)
            print("Queue report is not generated when quota is zero")

        finally:
            sai_thrift_remove_dtel_queue_report(self.client, queue_report)

    def tearDown(self):
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(QueueReportDoDTest, self).tearDown()


# The below test is only intended to run on the model.
# Remember to run with --dod-test-mode.
@group('dtel-restart')
class MoDDoDTest(DtelBaseTest):
    """ Basic MoD DoD Test class """

    def setUp(self):
        super(MoDDoDTest, self).setUp()

        bind_mirror_on_drop_pkt(self.int_v2)

        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            drop_report_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)

        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)
        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=10,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_dtel_drop_report_enable=enable,
            action_dtel_report_all_packets=enable)

    def runTest(self):
        pktlen = 256
        pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64,
            pktlen=pktlen)
        exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=63,
            pktlen=pktlen)

        fields_dod = {'ingress_port': self.port24_fp,
                      'egress_port': self.port25_fp, 'hw_id': 0}
        dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                      self.dev_port27, pkt, exp_pkt, True,
                      exp_dod_packet(pkt, self.int_v2, fields_dod))
        print("Drop report recv'd with SWITCH_DROP_REASON_TRAFFIC_MANAGER")

        print("Sending packets with ttl=1, redirected to CPU")
        pkt[IP].ttl = 1
        cpu_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_port=self.dev_port24,
            ingress_ifindex=0,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR,
            ingress_bd=0x0,
            inner_pkt=pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd_and_ifindex(cpu_pkt)
        # dtel_checkDoD(self, self.dev_port24, self.cpu_port0,
        #              self.dev_port27, pkt, cpu_pkt, False)
        print("No drop of ttl=1 packet redirected to CPU since dod is not set")

        pkt[IP].ttl = 64
        print("Change DTEL report truncate size to 128")
        truncate_size = 128
        sai_thrift_set_dtel_report_session_attribute(
            self.client, self.dtel_report_session,
            truncate_size=truncate_size)

        fields_dod = {'ingress_port': self.port24_fp,
                      'egress_port': self.port25_fp, 'hw_id': 4}

        if self.architecture == 'tofino':
            drop_truncate_adjust = 0
        else:
            if self.int_v2:
                # 8 is due to telem report header size diff from sw local
                drop_truncate_adjust = 8
            else:
                # 4 is due to telem report header size diff from sw local
                drop_truncate_adjust = 4

        truncated_amount = pktlen - (truncate_size + drop_truncate_adjust)
        exp_dod_pkt_full = exp_dod_packet(pkt, self.int_v2, fields_dod)
        exp_dod_pkt = Ether(bytes(exp_dod_pkt_full)[:-truncated_amount])
        exp_dod_pkt[IP].len -= truncated_amount  # noqa pylint: disable=no-member
        exp_dod_pkt[UDP].len -= truncated_amount  # noqa pylint: disable=no-member
        dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                      self.dev_port27, pkt, exp_pkt, True, exp_dod_pkt)
        print("Drop report recv'd containing packet truncated to 128")

    def tearDown(self):
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(MoDDoDTest, self).tearDown()


# The below test is only intended to run on the model.
# Remember to run with --dod-test-mode.
@group('dtel-restart')
class MoDAndFlowDoDTest(DtelBaseTest):
    """ Basic MoD DoD Test class """

    def setUp(self):
        super(MoDAndFlowDoDTest, self).setUp()

        bind_mirror_on_drop_pkt(self.int_v2)
        bind_postcard_pkt(self.int_v2)

        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            drop_report_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP,
                        SAI_ACL_ACTION_TYPE_DTEL_DROP_REPORT_ENABLE]

        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)

        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)
        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable,
            action_dtel_drop_report_enable=enable)

        self.pkt = simple_tcp_packet(
            eth_dst=MAC_SELF,
            eth_src=self.mac_nbr[1],
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst=self.mac_nbr[1],
            eth_src=MAC_SELF,
            ip_dst=IPADDR_NBR[1],
            ip_src=IPADDR_NBR[0],
            ip_id=105,
            ip_ttl=63)

    def runTest(self):
        try:
            self.flowReportEnabledDropReportEnabledTest()
            self.flowReportDisabledDropReportEnabledTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(MoDAndFlowDoDTest, self).tearDown()

    def flowReportEnabledDropReportEnabledTest(self):
        '''
        Test verifies DoD reports with
        flow report enabled and drop report enabled.
        '''
        print("flowReportEnabledDropReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=True)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)

        fields_dod = {'ingress_port': self.port24_fp,
                      'egress_port': self.port25_fp, 'hw_id': 0, 'flow': 1}
        fields = {'ingress_port': self.port24_fp,
                  'egress_port': self.port25_fp, 'hw_id': 0, 'flow': 1}
        dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                      self.dev_port27, self.pkt, self.exp_pkt, True,
                      exp_dod_packet(self.pkt, self.int_v2, fields_dod),
                      exp_e2e_pkt=exp_postcard_packet(self.exp_pkt,
                                                      self.int_v2,
                                                      fields),
                      exp_report_port=self.dev_port27)
        print("Drop report is generated with F bit set to 1")

    def flowReportDisabledDropReportEnabledTest(self):
        '''
        Test verifies DoD reports with
        flow report disabled and drop report enabled.
        '''
        print("flowReportDisabledDropReportEnabledTest")
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      postcard_enable=False)
        sai_thrift_set_dtel_attribute(self.client, self.dtel,
                                      drop_report_enable=True)

        fields_dod = {'ingress_port': self.port24_fp,
                      'egress_port': self.port25_fp, 'hw_id': 0, 'flow': 0}
        dtel_checkDoD(self, self.dev_port24, self.dev_port25,
                      self.dev_port27, self.pkt, self.exp_pkt, True,
                      exp_dod_packet(self.pkt, self.int_v2, fields_dod))
        print("Drop report is generated with F bit set to 0")


class DtelAclEthertypeTest(DtelBaseTest):
    """ Basic Flow Report test class """

    def setUp(self):
        super(DtelAclEthertypeTest, self).setUp()
        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            postcard_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        self.trap_group = sai_thrift_create_hostif_trap_group(
            self.client, admin_state=True, queue=0)

        self.hostif = sai_thrift_create_hostif(self.client,
                                               name="hostif",
                                               obj_id=self.port24,
                                               type=SAI_HOSTIF_TYPE_NETDEV)
        time.sleep(5)

        self.trap_list = [
            [SAI_HOSTIF_TRAP_TYPE_ARP_REQUEST, SAI_PACKET_ACTION_COPY],
            [SAI_HOSTIF_TRAP_TYPE_ARP_RESPONSE, SAI_PACKET_ACTION_TRAP]
        ]

        self.traps = []
        for trap in self.trap_list:
            self.traps.append(sai_thrift_create_hostif_trap(
                self.client, packet_action=trap[1],
                trap_type=trap[0], trap_group=self.trap_group))

        # create DTEL watchlist entry
        eth_type = 0x806
        eth_type_mask = 0x7FFF
        eth_type_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=eth_type),
            mask=sai_thrift_acl_field_data_mask_t(u16=eth_type_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_ether_type=eth_type_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

    def runTest(self):
        try:
            sock = open_packet_socket("hostif")
            pkt = simple_arp_packet(arp_op=1, eth_dst=MAC_SELF, pktlen=100)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.dev_port24,
                ingress_ifindex=(self.port24 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                ingress_bd=0x100e,
                inner_pkt=pkt)
            fields = {'ingress_port': self.port24_fp,
                      'egress_port': self.cpu_port0_fp, 'hw_id': 1, 'flow': 1}
            exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2,
                                                   fields)

            print("Sending Unicast ARP request to router MAC")
            send_packet(self, self.dev_port24, pkt)
            self.assertTrue(socket_verify_packet(pkt, sock))
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)

            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
            # create DTEL watchlist entry
            src_ip = IPADDR_NBR[0]
            src_ip_mask = '255.255.255.0'
            src_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
            dst_ip = IPADDR_NBR[1]
            dst_ip_mask = '255.255.255.0'
            dst_ip_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
                mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
            eth_type = 0x806
            eth_type_mask = 0x7FFF
            eth_type_t = sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_field_data_data_t(u16=eth_type),
                mask=sai_thrift_acl_field_data_mask_t(u16=eth_type_mask))
            enable = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(booldata=True))
            acl_dtel_flow_op = sai_thrift_acl_action_data_t(
                enable=True,
                parameter=sai_thrift_acl_action_parameter_t(
                    s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

            self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
                self.client,
                table_id=self.dtel_acl_table_id,
                priority=1,
                field_dst_ip=dst_ip_t,
                field_src_ip=src_ip_t,
                field_ether_type=eth_type_t,
                action_acl_dtel_flow_op=acl_dtel_flow_op,
                action_dtel_report_all_packets=enable)

            pkt = simple_tcp_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=105,
                ip_tos=8,
                tcp_sport=3333,
                tcp_dport=5555,
                tcp_flags="R",
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.mac_nbr[1],
                eth_src=MAC_SELF,
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=105,
                ip_tos=8,
                tcp_sport=3333,
                tcp_dport=5555,
                tcp_flags="R",
                ip_ttl=63)
            fields = {'ingress_port': self.port24_fp,
                      'egress_port': self.port25_fp, 'hw_id': 1, 'flow': 1}
            exp_postcard_pkt = exp_postcard_packet(exp_pkt, self.int_v2,
                                                   fields)

            print("Try to match on ether type not set (0x800) and " +
                  "no report should be generated")
            send_packet(self, self.dev_port24, pkt)
            verify_packet(self, exp_pkt, self.dev_port25)
            verify_no_packet(self, exp_postcard_pkt, self.dev_port27,
                             timeout=2)

        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        for trap in self.traps:
            sai_thrift_remove_hostif_trap(self.client, trap)
        sai_thrift_remove_hostif(self.client, self.hostif)
        sai_thrift_remove_hostif_trap_group(self.client, self.trap_group)

        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(DtelAclEthertypeTest, self).tearDown()


@group('dtel-inner')
class VxLanReportTest(DtelBaseTest):
    '''
    This class contains basic configuration for VxLAN tunnel tests
    '''

    def __init__(self, ipv6=False):
        super(VxLanReportTest, self).__init__()
        self.ipv6 = ipv6

        if ipv6 is True:
            self.tun_ip = "2001:0db8::10:1"
            self.lpb_ip = "2001:0db8::10:10"
            self.tun_ip_mask = "/128"
        else:
            self.tun_ip = "10.10.10.1"
            self.lpb_ip = "10.10.10.2"
            self.tun_ip_mask = "/32"

    def setUp(self):
        super(VxLanReportTest, self).setUp()

        IPADDR_INF[2] = '1.1.0.2'
        IPADDR_NBR[2] = '1.1.0.101'
        bind_postcard_pkt(self.int_v2)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            postcard_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.vrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        self.oport = self.port30
        self.oport_dev = self.dev_port30
        self.uport = self.port31
        self.uport_dev = self.dev_port31

        self.vni = 1000
        self.vm_ip = "100.100.1.1"
        self.vm_ipv6 = "2001:0db8::1:1"
        self.customer_ip = "100.100.2.1"
        self.customer_ipv6 = "2001:0db8::2:1"
        self.inner_dmac = "00:11:11:11:11:11"
        self.customer_mac = "00:22:22:22:22:22"
        self.unbor_mac = "00:33:33:33:33:33"

        # underlay configuration
        self.uvrf = sai_thrift_create_virtual_router(self.client)

        # overlay configuraion
        self.ovrf = sai_thrift_create_virtual_router(self.client)
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP
        ttl_mode = SAI_TUNNEL_TTL_MODE_PIPE_MODEL

        # underlay loopback RIF for tunnel
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.uvrf)

        self.urif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.uvrf,
            port_id=self.uport)

        self.orif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.ovrf,
            port_id=self.oport)

        # encap/decap mappers
        self.encap_tunnel_map = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI)

        self.decap_tunnel_map = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID)

        # encap/decap mapper entries
        self.encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map=self.encap_tunnel_map,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI,
            virtual_router_id_key=self.ovrf,
            vni_id_value=self.vni)

        self.decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map=self.encap_tunnel_map,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID,
            virtual_router_id_value=self.ovrf,
            vni_id_key=self.vni)

        encap_maps = sai_thrift_object_list_t(count=1,
                                              idlist=[self.encap_tunnel_map])
        decap_maps = sai_thrift_object_list_t(count=1,
                                              idlist=[self.decap_tunnel_map])

        # tunnel
        self.tunnel = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            encap_mappers=encap_maps,
            decap_mappers=decap_maps,
            encap_ttl_mode=ttl_mode,
            decap_ttl_mode=ttl_mode,
            underlay_interface=self.urif_lpb)

        # tunnel termination entry
        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            tunnel_type=tunnel_type,
            vr_id=self.uvrf,
            action_tunnel_id=self.tunnel,
            type=term_type,
            dst_ip=sai_ipaddress(self.lpb_ip))

        # route to customer from VM
        self.onhop = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress(self.customer_ip),
            router_interface_id=self.orif, type=SAI_NEXT_HOP_TYPE_IP)

        self.onbor = sai_thrift_neighbor_entry_t(
            rif_id=self.orif, ip_address=sai_ipaddress(self.customer_ip))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.onbor,
                                         dst_mac_address=self.customer_mac,
                                         no_host_route=True)

        self.customer_route = sai_thrift_route_entry_t(
            vr_id=self.ovrf,
            destination=sai_ipprefix(self.customer_ip + '/32'))
        sai_thrift_create_route_entry(self.client,
                                      self.customer_route,
                                      next_hop_id=self.onhop)

        self.onhop_v6 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress(self.customer_ipv6),
            router_interface_id=self.orif,
            type=SAI_NEXT_HOP_TYPE_IP)

        self.onbor_v6 = sai_thrift_neighbor_entry_t(
            rif_id=self.orif, ip_address=sai_ipaddress(self.customer_ipv6))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.onbor_v6,
                                         dst_mac_address=self.customer_mac,
                                         no_host_route=True)

        self.customer_v6_route = sai_thrift_route_entry_t(
            vr_id=self.ovrf,
            destination=sai_ipprefix(self.customer_ipv6 + '/128'))
        sai_thrift_create_route_entry(self.client,
                                      self.customer_v6_route,
                                      next_hop_id=self.onhop_v6)

        # tunnel nexthop for VM
        self.tunnel_nhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_TUNNEL_ENCAP,
            tunnel_id=self.tunnel,
            ip=sai_ipaddress(self.tun_ip),
            tunnel_mac=self.inner_dmac,
            tunnel_vni=self.vni)

        # routes to VM via tunnel nexthop
        self.vm_route = sai_thrift_route_entry_t(
            vr_id=self.ovrf, destination=sai_ipprefix(self.vm_ip + '/32'))
        sai_thrift_create_route_entry(self.client,
                                      self.vm_route,
                                      next_hop_id=self.tunnel_nhop)

        self.vm_v6_route = sai_thrift_route_entry_t(
            vr_id=self.ovrf, destination=sai_ipprefix(self.vm_ipv6 + '/128'))
        sai_thrift_create_route_entry(self.client,
                                      self.vm_v6_route,
                                      next_hop_id=self.tunnel_nhop)

        # route to tunnel
        self.unhop = sai_thrift_create_next_hop(self.client,
                                                ip=sai_ipaddress(self.tun_ip),
                                                router_interface_id=self.urif,
                                                type=SAI_NEXT_HOP_TYPE_IP)

        self.unbor = sai_thrift_neighbor_entry_t(
            rif_id=self.urif, ip_address=sai_ipaddress(self.tun_ip))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor_mac,
                                         no_host_route=True)

        self.tunnel_route = sai_thrift_route_entry_t(
            vr_id=self.uvrf,
            destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
        sai_thrift_create_route_entry(self.client,
                                      self.tunnel_route,
                                      next_hop_id=self.unhop)

        # More Egress rifs
        self.rif2 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.uvrf,
            src_mac_address=ROUTER_MAC,
            port_id=self.port28)

        self.onhop2 = sai_thrift_create_next_hop(
            self.client, ip=sai_ipaddress('20.20.0.1'),
            router_interface_id=self.rif2, type=SAI_NEXT_HOP_TYPE_IP)

        self.onbor2 = sai_thrift_neighbor_entry_t(
            rif_id=self.rif2, ip_address=sai_ipaddress('20.20.0.1'))
        sai_thrift_create_neighbor_entry(self.client,
                                         self.onbor2,
                                         dst_mac_address='00:22:22:22:22:33',
                                         no_host_route=True)

        self.customer_route2 = sai_thrift_route_entry_t(
            vr_id=self.uvrf,
            destination=sai_ipprefix('20.20.0.1' + '/32'))
        sai_thrift_create_route_entry(self.client,
                                      self.customer_route2,
                                      next_hop_id=self.onhop2)

    def runTest(self):
        try:
            self.basicTunnelReportTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_route_entry(self.client, self.tunnel_route)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_route_entry(self.client, self.vm_v6_route)
        sai_thrift_remove_route_entry(self.client, self.vm_route)
        sai_thrift_remove_next_hop(self.client, self.tunnel_nhop)
        sai_thrift_remove_route_entry(self.client, self.customer_v6_route)
        sai_thrift_remove_neighbor_entry(self.client, self.onbor_v6)
        sai_thrift_remove_next_hop(self.client, self.onhop_v6)
        sai_thrift_remove_route_entry(self.client, self.customer_route)
        sai_thrift_remove_neighbor_entry(self.client, self.onbor)
        sai_thrift_remove_next_hop(self.client, self.onhop)
        sai_thrift_remove_route_entry(self.client, self.customer_route2)
        sai_thrift_remove_neighbor_entry(self.client, self.onbor2)
        sai_thrift_remove_next_hop(self.client, self.onhop2)
        sai_thrift_remove_tunnel_term_table_entry(self.client,
                                                  self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.tunnel)
        sai_thrift_remove_tunnel_map_entry(self.client,
                                           self.decap_tunnel_map_entry)
        sai_thrift_remove_tunnel_map_entry(self.client,
                                           self.encap_tunnel_map_entry)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map)
        sai_thrift_remove_router_interface(self.client, self.rif2)
        sai_thrift_remove_router_interface(self.client, self.orif)
        sai_thrift_remove_router_interface(self.client, self.urif)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)
        sai_thrift_remove_virtual_router(self.client, self.ovrf)
        sai_thrift_remove_virtual_router(self.client, self.uvrf)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        super(VxLanReportTest, self).tearDown()

    def basicTunnelReportTest(self):
        print("\nbasicTunnelReportTest()")

        pkt_udp = simple_udp_packet(eth_dst=self.customer_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.customer_ip,
                                    ip_src=self.vm_ip,
                                    ip_id=108,
                                    ip_ttl=63,
                                    udp_sport=3333)
        pkt_tcp = simple_tcp_packet(eth_dst=self.customer_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.customer_ip,
                                    ip_src=self.vm_ip,
                                    ip_id=108,
                                    ip_ttl=63,
                                    tcp_sport=3333)
        pkt_udp_v6 = simple_udpv6_packet(eth_dst=self.customer_mac,
                                         eth_src=ROUTER_MAC,
                                         ipv6_dst=self.customer_ipv6,
                                         ipv6_src=self.vm_ipv6,
                                         ipv6_hlim=63,
                                         udp_sport=3333)
        pkt_tcp_v6 = simple_tcpv6_packet(eth_dst=self.customer_mac,
                                         eth_src=ROUTER_MAC,
                                         ipv6_dst=self.customer_ipv6,
                                         ipv6_src=self.vm_ipv6,
                                         ipv6_hlim=63,
                                         tcp_sport=3333)
        inner_pkt_udp = simple_udp_packet(eth_dst=ROUTER_MAC,
                                          eth_src=self.inner_dmac,
                                          ip_dst=self.customer_ip,
                                          ip_src=self.vm_ip,
                                          ip_id=108,
                                          ip_ttl=64,
                                          udp_sport=3333)
        inner_pkt_tcp = simple_tcp_packet(eth_dst=ROUTER_MAC,
                                          eth_src=self.inner_dmac,
                                          ip_dst=self.customer_ip,
                                          ip_src=self.vm_ip,
                                          ip_id=108,
                                          ip_ttl=64,
                                          tcp_sport=3333)
        inner_pkt_udp_v6 = simple_udpv6_packet(eth_dst=ROUTER_MAC,
                                               eth_src=self.inner_dmac,
                                               ipv6_dst=self.customer_ipv6,
                                               ipv6_src=self.vm_ipv6,
                                               ipv6_hlim=64,
                                               udp_sport=3333)
        inner_pkt_tcp_v6 = simple_tcpv6_packet(eth_dst=ROUTER_MAC,
                                               eth_src=self.inner_dmac,
                                               ipv6_dst=self.customer_ipv6,
                                               ipv6_src=self.vm_ipv6,
                                               ipv6_hlim=64,
                                               tcp_sport=3333)
        vxlan_pkt_udp = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            udp_sport=11638,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni,
                                            inner_frame=inner_pkt_udp)
        vxlan_pkt_tcp = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            udp_sport=11638,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni,
                                            inner_frame=inner_pkt_tcp)
        vxlan_pkt_udp_v6 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                               eth_src=self.unbor_mac,
                                               ip_dst=self.lpb_ip,
                                               ip_src=self.tun_ip,
                                               ip_id=0,
                                               ip_ttl=64,
                                               ip_flags=0x2,
                                               udp_sport=11638,
                                               with_udp_chksum=False,
                                               vxlan_vni=self.vni,
                                               inner_frame=inner_pkt_udp_v6)
        vxlan_pkt_tcp_v6 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                               eth_src=self.unbor_mac,
                                               ip_dst=self.lpb_ip,
                                               ip_src=self.tun_ip,
                                               ip_id=0,
                                               ip_ttl=64,
                                               ip_flags=0x2,
                                               udp_sport=11638,
                                               with_udp_chksum=False,
                                               vxlan_vni=self.vni,
                                               inner_frame=inner_pkt_tcp_v6)
        vxlan_pkt_transit = simple_vxlan_packet(
                eth_src=self.unbor_mac,
                eth_dst=ROUTER_MAC,
                ip_id=0,
                ip_dst='20.20.0.1',
                ip_src=self.tun_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt_udp)
        vxlan_pkt_transit_v6 = simple_vxlan_packet(
                eth_src=self.unbor_mac,
                eth_dst=ROUTER_MAC,
                ip_id=0,
                ip_dst='20.20.0.1',
                ip_src=self.tun_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt_udp_v6)

        # Transit packets
        vxlan_exp_pkt_transit = simple_vxlan_packet(
                eth_dst='00:22:22:22:22:33',
                eth_src=ROUTER_MAC,
                ip_id=0,
                ip_dst='20.20.0.1',
                ip_src=self.tun_ip,
                ip_ttl=63,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt_udp)
        vxlan_exp_pkt_transit_v6 = simple_vxlan_packet(
                eth_dst='00:22:22:22:22:33',
                eth_src=ROUTER_MAC,
                ip_id=0,
                ip_dst='20.20.0.1',
                ip_src=self.tun_ip,
                ip_ttl=63,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt_udp_v6)

        # create DTEL watchlist entry for decap
        dst_ip = self.customer_ip
        dst_ip_mask = '255.255.255.255'
        dst_ip_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Valid v4 packet, outer ip miss dst_vtep, routed")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port28_fp,
                      'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_transit)
            verify_packet(self, vxlan_exp_pkt_transit, self.dev_port28)
            exp_postcard_pkt = exp_postcard_packet(vxlan_exp_pkt_transit,
                                                   self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 UDP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_udp)
            verify_packet(self, pkt_udp, self.oport_dev)
            exp_postcard_pkt = exp_postcard_packet(pkt_udp, self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 TCP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_tcp)
            verify_packet(self, pkt_tcp, self.oport_dev)
            exp_postcard_pkt = exp_postcard_packet(pkt_tcp, self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 3)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create DTEL watchlist entry for decap, no match
        dst_ip = '192.0.2.1'
        dst_ip_mask = '255.255.255.255'
        dst_ip_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 UDP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_udp)
            verify_packet(self, pkt_udp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 TCP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_tcp)
            verify_packet(self, pkt_tcp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create IPv6 DTEL watchlist entry for decap
        dst_ip = self.customer_ipv6
        dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        dst_ip_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_dst_ipv6=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Valid v6 packet, outer ip miss dst_vtep, routed")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port28_fp,
                      'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_transit_v6)
            verify_packet(self, vxlan_exp_pkt_transit_v6, self.dev_port28)
            exp_postcard_pkt = exp_postcard_packet(vxlan_exp_pkt_transit_v6,
                                                   self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 UDP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_udp_v6)
            verify_packet(self, pkt_udp_v6, self.oport_dev)
            exp_postcard_pkt = exp_postcard_packet(pkt_udp_v6, self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 TCP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_tcp_v6)
            verify_packet(self, pkt_tcp_v6, self.oport_dev)
            exp_postcard_pkt = exp_postcard_packet(pkt_tcp_v6, self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 3)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create IPv6 DTEL watchlist entry for decap, no match
        dst_ip = '2001:db8::1'
        dst_ip_mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        dst_ip_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(ip6=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip6=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_dst_ipv6=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 UDP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_udp_v6)
            verify_packet(self, pkt_udp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 TCP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_tcp_v6)
            verify_packet(self, pkt_tcp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create TCP IPv4/IPv6 DTEL watchlist entry for decap
        l4_src_port = 3333
        l4_src_port_mask = 32759
        l4_src_port_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=l4_src_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_src_port_mask))
        ip_proto = 6  # IP_PROTO_TCP
        ip_proto_mask = 127
        ip_proto_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
            mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))
        tunnel_vni = self.vni
        tunnel_vni_mask = 0xFFFFFF
        tunnel_vni_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u32=tunnel_vni),
            mask=sai_thrift_acl_field_data_mask_t(u32=tunnel_vni_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_l4_src_port=l4_src_port_t,
            field_ip_protocol=ip_proto_t,
            field_tunnel_vni=tunnel_vni_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 UDP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_udp)
            verify_packet(self, pkt_udp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 TCP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_tcp)
            verify_packet(self, pkt_tcp, self.oport_dev)
            exp_postcard_pkt = exp_postcard_packet(pkt_tcp, self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 UDP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_udp_v6)
            verify_packet(self, pkt_udp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 TCP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_tcp_v6)
            verify_packet(self, pkt_tcp_v6, self.oport_dev)
            exp_postcard_pkt = exp_postcard_packet(pkt_tcp_v6, self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create TCP IPv4/IPv6 DTEL watchlist entry for decap, no match
        l4_src_port = 3334
        l4_src_port_mask = 32759
        l4_src_port_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=l4_src_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_src_port_mask))
        ip_proto = 6  # IP_PROTO_TCP
        ip_proto_mask = 127
        ip_proto_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
            mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_l4_src_port=l4_src_port_t,
            field_ip_protocol=ip_proto_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 UDP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_udp)
            verify_packet(self, pkt_udp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 TCP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_tcp)
            verify_packet(self, pkt_tcp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 UDP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_udp_v6)
            verify_packet(self, pkt_udp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 TCP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_tcp_v6)
            verify_packet(self, pkt_tcp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create UDP IPv4/IPv6 DTEL watchlist entry for decap
        l4_src_port = 3333
        l4_src_port_mask = 32759
        l4_src_port_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=l4_src_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_src_port_mask))
        ip_proto = 17  # IP_PROTO_UDP
        ip_proto_mask = 127
        ip_proto_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
            mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))
        tunnel_vni = self.vni
        tunnel_vni_mask = 0xFFFFFF
        tunnel_vni_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u32=tunnel_vni),
            mask=sai_thrift_acl_field_data_mask_t(u32=tunnel_vni_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_l4_src_port=l4_src_port_t,
            field_ip_protocol=ip_proto_t,
            field_tunnel_vni=tunnel_vni_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 UDP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_udp)
            verify_packet(self, pkt_udp, self.oport_dev)
            exp_postcard_pkt = exp_postcard_packet(pkt_udp, self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 TCP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_tcp)
            verify_packet(self, pkt_tcp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 UDP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_udp_v6)
            verify_packet(self, pkt_udp_v6, self.oport_dev)
            exp_postcard_pkt = exp_postcard_packet(pkt_udp_v6, self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 TCP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_tcp_v6)
            verify_packet(self, pkt_tcp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 2)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create UDP IPv4/IPv6 DTEL watchlist entry for decap, no match
        l4_src_port = 3334
        l4_src_port_mask = 32759
        l4_src_port_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u16=l4_src_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_src_port_mask))
        ip_proto = 17  # IP_PROTO_UDP
        ip_proto_mask = 127
        ip_proto_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
            mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_l4_src_port=l4_src_port_t,
            field_ip_protocol=ip_proto_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 UDP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_udp)
            verify_packet(self, pkt_udp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 TCP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_tcp)
            verify_packet(self, pkt_tcp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 UDP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_udp_v6)
            verify_packet(self, pkt_udp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 TCP packet")
            send_packet(self, self.uport_dev, vxlan_pkt_tcp_v6)
            verify_packet(self, pkt_tcp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create IPv4/IPv6 DTEL watchlist entry for decap, no match
        tunnel_vni = self.vni + 1
        tunnel_vni_mask = 0xFFFFFF
        tunnel_vni_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(u32=tunnel_vni),
            mask=sai_thrift_acl_field_data_mask_t(u32=tunnel_vni_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_tunnel_vni=tunnel_vni_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        try:
            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 UDP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_udp)
            verify_packet(self, pkt_udp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v4 TCP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_tcp)
            verify_packet(self, pkt_tcp, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 UDP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_udp_v6)
            verify_packet(self, pkt_udp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            print("Vxlan Decap Dtel Report : "
                  "Sending valid VxLAN v6 TCP packet")
            fields = {'ingress_port': self.port31_fp,
                      'egress_port': self.port30_fp, 'hw_id': 1, 'flow': 1}
            send_packet(self, self.uport_dev, vxlan_pkt_tcp_v6)
            verify_packet(self, pkt_tcp_v6, self.oport_dev)
            verify_no_other_packets(self, timeout=1)

            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 0)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)

        # create DTEL watchlist entry for Encap
        dst_ip = self.vm_ip
        dst_ip_mask = '255.255.255.255'
        dst_ip_t = sai_thrift_acl_field_data_t(
            enable=True,
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create ACL counter
        acl_counter = sai_thrift_create_acl_counter(
            self.client, table_id=self.dtel_acl_table_id)

        # attach ACL counter to ACL entry
        action_counter_t = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(oid=acl_counter))
        sai_thrift_set_acl_entry_attribute(
            self.client, self.dtel_acl_entry_id,
            action_counter=action_counter_t)

        print("Vxlan Encap Dtel Report : Sending valid VxLAN v4 packet")
        pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                eth_src=self.customer_mac,
                                ip_dst=self.vm_ip,
                                ip_src=self.customer_ip,
                                ip_id=108,
                                ip_ttl=64)
        inner_pkt = simple_udp_packet(eth_dst=self.inner_dmac,
                                      eth_src=ROUTER_MAC,
                                      ip_dst=self.vm_ip,
                                      ip_src=self.customer_ip,
                                      ip_id=108,
                                      ip_ttl=63)
        vxlan_pkt = Mask(
            simple_vxlan_packet(eth_dst=self.unbor_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni,
                                inner_frame=inner_pkt))
        vxlan_pkt_report = simple_vxlan_packet(
                                eth_dst=self.unbor_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni,
                                inner_frame=inner_pkt)

        try:
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            fields = {'ingress_port': self.port30_fp,
                      'egress_port': self.port31_fp, 'hw_id': 1, 'flow': 1}

            send_packet(self, self.oport_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport_dev)
            vxlan_pkt_report["UDP"].sport = 0
            exp_postcard_pkt = exp_postcard_packet(vxlan_pkt_report,
                                                   self.int_v2,
                                                   fields)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port27)
            packets = sai_thrift_get_acl_counter_attribute(
                    self.client, acl_counter, packets=True)
            self.assertEqual(packets['packets'], 1)
        finally:
            action_counter_t = sai_thrift_acl_action_data_t(
                    parameter=sai_thrift_acl_action_parameter_t(
                        oid=0),
                    enable=True)
            sai_thrift_set_acl_entry_attribute(
                    self.client, self.dtel_acl_entry_id,
                    action_counter=action_counter_t)
            sai_thrift_remove_acl_counter(self.client, acl_counter)
            sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        print("\tOK")


@group('dtel-inner')
class FlowReportOverAsymVxLanTest(DtelBaseTest):
    """
    Flow Report where the report uses asymmetric L3 forwarding into
    a VxLAN tunnel
    """

    def setUp(self):
        super(FlowReportOverAsymVxLanTest, self).setUp()
        bind_postcard_pkt(self.int_v2)

        self.oport1 = self.port0
        self.oport1_dev = self.dev_port0
        self.oport1_bp = self.port0_bp
        self.uport1 = self.port10
        self.uport1_dev = self.dev_port10
        self.uport1_rif = self.port10_rif
        self.tun1_ip = "10.0.0.65"
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"
        self.uport1_myip = "10.0.0.64"
        self.uport_mask = "/31"

        self.vlan10_id = 10
        self.vni1 = 1000
        self.vlan10_myip = "192.168.100.254"
        self.vlan10_dir_br = "192.168.100.255"
        self.customer1_ip = "192.168.100.1"
        self.inner_dmac = "00:11:11:11:11:11"
        self.customer1_mac = "00:22:22:22:22:11"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.bcast_mac = "ff:ff:ff:ff:ff:ff"

        REPORT_SRC = self.vlan10_myip
        REPORT_DST = [self.customer1_ip]

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # underlay my ip address + prefix
        self.uport1_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.uport1_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_prefix_route,
                                      next_hop_id=self.uport1_rif)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport1_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport1_rif)

        # create overlay vrf
        self.ovrf = sai_thrift_create_virtual_router(self.client)

        # create vlan10 rif, ip address, prefix
        self.vlan10_rif = sai_thrift_create_router_interface(
            self.client,
            virtual_router_id=self.ovrf,
            src_mac_address=ROUTER_MAC,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            vlan_id=self.vlan10,
            mtu=9100,
            nat_zone_id=0)

        self.vlan10_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan10_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan10_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.vlan10_nbor_dir_br = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.vlan10_dir_br),
            rif_id=self.vlan10_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.vlan10_nbor_dir_br,
                                         dst_mac_address=self.bcast_mac)

        self.vlan10_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan10_myip + '/24'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan10_prefix_route,
                                      next_hop_id=self.vlan10_rif)

        # create dtel
        self.dtel = sai_thrift_create_dtel(
            self.client,
            switch_id=SID,
            postcard_enable=True,
            latency_sensitivity=30)

        # create report session according to params
        dtel_dst_addrs = [sai_ipaddress(REPORT_DST[0])]
        dtel_dst_ip_list = sai_thrift_ip_address_list_t(
            addresslist=dtel_dst_addrs, count=len(dtel_dst_addrs))
        self.dtel_report_session = sai_thrift_create_dtel_report_session(
            self.client,
            src_ip=sai_ipaddress(REPORT_SRC),
            dst_ip_list=dtel_dst_ip_list,
            virtual_router_id=self.ovrf,
            truncate_size=self.report_truncate_size,
            udp_dst_port=self.report_udp_port)

        # create DTEL watchlist
        bind_points = [SAI_ACL_BIND_POINT_TYPE_SWITCH]
        bind_point_type_list = sai_thrift_s32_list_t(
            count=len(bind_points), int32list=bind_points)
        action_types = [SAI_ACL_ACTION_TYPE_ACL_DTEL_FLOW_OP]
        action_type_list = sai_thrift_s32_list_t(
            count=len(action_types), int32list=action_types)
        self.dtel_acl_table_id = sai_thrift_create_acl_table(
            self.client,
            acl_stage=SAI_ACL_STAGE_INGRESS,
            acl_bind_point_type_list=bind_point_type_list,
            field_src_ip=True,
            field_dst_ip=True,
            acl_action_type_list=action_type_list)
        sai_thrift_set_switch_attribute(self.client,
                                        ingress_acl=self.dtel_acl_table_id)

        # create high priority DTEL watchlist entry
        # with protocol==udp and l4_dst_port=self.report_udp_port
        # to prevent loop reports
        l4_dst_port = self.report_udp_port
        l4_dst_port_mask = 32759
        l4_dst_port_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u16=l4_dst_port),
            mask=sai_thrift_acl_field_data_mask_t(u16=l4_dst_port_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_NOP))
        ip_proto = 17
        ip_proto_mask = 127
        ip_proto_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(u8=ip_proto),
            mask=sai_thrift_acl_field_data_mask_t(u8=ip_proto_mask))

        self.dtel_acl_high_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1000,
            field_l4_dst_port=l4_dst_port_t,
            field_ip_protocol=ip_proto_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

        # create DTEL watchlist entry
        src_ip = IPADDR_NBR[0]
        src_ip_mask = '255.255.255.0'
        src_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=src_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=src_ip_mask))
        dst_ip = IPADDR_NBR[1]
        dst_ip_mask = '255.255.255.0'
        dst_ip_t = sai_thrift_acl_field_data_t(
            data=sai_thrift_acl_field_data_data_t(ip4=dst_ip),
            mask=sai_thrift_acl_field_data_mask_t(ip4=dst_ip_mask))
        enable = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(booldata=True))
        acl_dtel_flow_op = sai_thrift_acl_action_data_t(
            enable=True,
            parameter=sai_thrift_acl_action_parameter_t(
                s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))

        self.dtel_acl_entry_id = sai_thrift_create_acl_entry(
            self.client,
            table_id=self.dtel_acl_table_id,
            priority=1,
            field_src_ip=src_ip_t,
            field_dst_ip=dst_ip_t,
            action_acl_dtel_flow_op=acl_dtel_flow_op,
            action_dtel_report_all_packets=enable)

    def runTest(self):
        try:
            self.FlowReportOverVxLanEncapTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_set_switch_attribute(self.client, ingress_acl=0)
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_entry_id)
        sai_thrift_remove_acl_entry(self.client, self.dtel_acl_high_entry_id)
        sai_thrift_remove_acl_table(self.client, self.dtel_acl_table_id)
        sai_thrift_remove_dtel_report_session(self.client,
                                              self.dtel_report_session)
        sai_thrift_remove_dtel(self.client, self.dtel)

        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_route_entry(self.client, self.vlan10_prefix_route)
        sai_thrift_remove_neighbor_entry(self.client, self.vlan10_nbor_dir_br)
        sai_thrift_remove_route_entry(self.client, self.vlan10_my_route)
        sai_thrift_remove_router_interface(self.client, self.vlan10_rif)
        sai_thrift_remove_virtual_router(self.client, self.ovrf)
        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_route_entry(self.client, self.uport1_prefix_route)
        sai_thrift_remove_route_entry(self.client, self.uport1_my_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        REPORT_SRC = '4.4.4.1'
        REPORT_DST = ['4.4.4.3']
        super(FlowReportOverAsymVxLanTest, self).tearDown()

    def FlowReportOverVxLanEncapTest(self):
        ''' Test verifies valid flow report forwarding into a VxLan tunnel '''
        print("FlowReportOverVxLanEncapTest")

        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        REPORT_SRC = self.vlan10_myip
        REPORT_DST = [self.customer1_ip]
        COLLECTOR_MAC = self.customer1_mac

        try:
            # create tunnel maps
            decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

            encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

            # create tunnel
            decap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[decap_tunnel_map_vlan])
            encap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[encap_tunnel_map_vlan])
            tunnel0 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=peer_mode)

            tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=term_type,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(self.lpb_ip),
                tunnel_type=tunnel_type,
                action_tunnel_id=tunnel0)

            # create tunnel map entries for vlans
            decap_tunnel_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan10_id,
                vni_id_key=self.vni1)

            # when first route comes down for a remote vtep,
            # p2p tunnel comes down first
            tunnel1 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                encap_dst_ip=sai_ipaddress(self.tun1_ip))

            # bridge port is created on p2p tunnel
            tun1_bp = sai_thrift_create_bridge_port(
                self.client,
                type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                tunnel_id=tunnel1,
                bridge_id=self.default_1q_bridge,
                admin_state=True,
                fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

            # vlan_member is created using tunnel bridge_port
            tun1_vlan_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=tun1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            # customer_1 neighbor, nexthop, fdb_entry
            nbor = sai_thrift_neighbor_entry_t(
                rif_id=self.vlan10_rif,
                ip_address=sai_ipaddress(self.customer1_ip))
            sai_thrift_create_neighbor_entry(
                self.client, nbor, dst_mac_address=self.customer1_mac)

            tunnel_nhop_asym = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.customer1_ip),
                router_interface_id=self.vlan10_rif)

            fdb_entry = sai_thrift_fdb_entry_t(
                bv_id=self.vlan10,
                mac_address=self.customer1_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=tun1_bp,
                endpoint_ip=sai_ipaddress(self.tun1_ip))

            pkt_in = simple_tcp_packet(
                eth_dst=MAC_SELF,
                eth_src=self.mac_nbr[0],
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=105,
                ip_ttl=64,
                tcp_flags=None)
            exp_pkt_out = simple_tcp_packet(
                eth_dst=self.mac_nbr[1],
                eth_src=MAC_SELF,
                ip_dst=IPADDR_NBR[1],
                ip_src=IPADDR_NBR[0],
                ip_id=105,
                tcp_flags=None,
                ip_ttl=63)

            if self.int_v2:
                exp_postcard_pkt_inner = ipv4_dtel_v2_pkt(
                    eth_dst=COLLECTOR_MAC,
                    eth_src=MAC_SELF,
                    ip_src=REPORT_SRC,
                    ip_dst=REPORT_DST[0],
                    ip_id=0,
                    ip_ttl=63,
                    dropped=0,
                    congested_queue=0,
                    path_tracking_flow=1,
                    hw_id=1,
                    switch_id=SID,
                    ingress_port=self.port24_fp,
                    egress_port=self.port25_fp,
                    queue_id=0,
                    queue_depth=0,
                    ingress_tstamp=0,
                    egress_tstamp=0,
                    inner_frame=exp_pkt_out)

            else:

                exp_pc_inner = postcard_report(
                    packet=exp_pkt_out,
                    switch_id=SID,
                    ingress_port=self.port24_fp,
                    egress_port=self.port25_fp,
                    queue_id=0,
                    queue_depth=0,
                    egress_tstamp=0)

                exp_postcard_pkt_inner = ipv4_dtel_pkt(
                    eth_dst=COLLECTOR_MAC,
                    eth_src=MAC_SELF,
                    ip_src=REPORT_SRC,
                    ip_dst=REPORT_DST[0],
                    ip_id=0,
                    ip_ttl=63,
                    next_proto=DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL,
                    dropped=0,
                    congested_queue=0,
                    path_tracking_flow=1,
                    hw_id=1,
                    inner_frame=exp_pc_inner)

            exp_postcard_pkt = simple_vxlan_packet(
                eth_dst=self.unbor1_mac,
                eth_src=ROUTER_MAC,
                ip_dst=self.tun1_ip,
                ip_src=self.lpb_ip,
                ip_id=0,
                ip_ttl=63,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=exp_postcard_pkt_inner)

            print("Reports for flow that matches dtel_acl")
            print("Valid report should be generated for %s" % IPADDR_NBR[1])
            send_packet(self, self.dev_port24, pkt_in)
            verify_packet(self, exp_pkt_out, self.dev_port25)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port10)
            verify_no_other_packets(self, timeout=1)
            print("Report should be generated again for %s" % IPADDR_NBR[1])
            send_packet(self, self.dev_port24, pkt_in)
            verify_packet(self, exp_pkt_out, self.dev_port25)
            verify_postcard_packet(self, exp_postcard_pkt, self.dev_port10)
            verify_no_other_packets(self, timeout=1)

        finally:
            REPORT_SRC = '4.4.4.1'
            REPORT_DST = ['4.4.4.3']
            COLLECTOR_MAC = '00:11:22:33:44:57'

            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            sai_thrift_remove_neighbor_entry(self.client, nbor)
            sai_thrift_remove_next_hop(self.client, tunnel_nhop_asym)
            if 'tun1_vlan_member' in locals() and tun1_vlan_member:
                sai_thrift_remove_vlan_member(self.client, tun1_vlan_member)
            if 'tun1_bp' in locals() and tun1_bp:
                sai_thrift_remove_bridge_port(self.client, tun1_bp)
            if 'tunnel1' in locals() and tunnel1:
                sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan10)
            sai_thrift_remove_tunnel_term_table_entry(self.client, tunnel_term)
            sai_thrift_remove_tunnel(self.client, tunnel0)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)
