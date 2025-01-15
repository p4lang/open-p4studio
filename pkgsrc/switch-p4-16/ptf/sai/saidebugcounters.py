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
Thrift SAI interface Debug Counters tests
"""
import switchsai_thrift

import socket
#from switch import *
import sai_base_test
from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

from switchsai_thrift.sai_headers import  *
from switchsai_thrift.ttypes import *
from switch_utils import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.pktpy_utils import pktpy_skip # noqa pylint: disable=wrong-import-position


class BaseDebugCounterTest(sai_base_test.ThriftInterfaceDataPlane):

    default_mac_1 = '00:11:11:11:11:11'
    default_mac_2 = '00:22:22:22:22:22'
    neighbor_mac  = '00:11:22:33:44:55'
    reserved_mac  = '01:80:C2:00:00:01'
    mc_mac        = '01:00:5E:AA:AA:AA'

    neighbor_ip   = '10.10.10.1'
    neighbor_ip_2 = '10.10.10.2'
    loopback_ipv4 = '127.0.0.1'
    loopback_ipv6 = ['::1', '0:0:0:0:0:ffff:7f00:1998']
    mc_ip         = '224.0.0.1'
    class_e_ip    = '240.0.0.1'
    unspec_ip     = '0.0.0.0'
    bc_ip         = '255.255.255.255'
    link_local_ip = '169.254.0.1'

    neighbor_ipv6   = '1234:5678:9abc:def0:4422:1133:5577:99aa'
    mc_scope_0_ipv6 = 'FF00:0:0:0:0:0:0:1'
    mc_scope_1_ipv6 = 'FF01:0:0:0:0:0:0:1'

    ingress_port  = 0
    egress_port   = 1

    addr_family        = SAI_IP_ADDR_FAMILY_IPV4
    ip_addr_subnet     = '10.10.10.0'
    ip_mask            = '255.255.255.0'
    addr_family_v6     = SAI_IP_ADDR_FAMILY_IPV6
    ip_addr_subnet_v6  = '1234:5678:9abc:def0:4422:1133:5577:0'
    ip_mask_v6         = 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:0'
    v4_enabled = 1
    v6_enabled = 1

    dc_oid=None
    second_dc_oid=None
    vr_ids=[]
    rif_ids=[]
    neighbors=[]
    routes=[]
    fdbs=[]

    def getPortStatsExt(self, port, counter_ids):
        ''' function gets the regular port counter stats
            using sai_thrift_get_port_stats_ext() '''
        if not isinstance(counter_ids, list):
            counter_ids = [counter_ids]

        stats = self.client.sai_thrift_get_port_stats_ext(port, counter_ids, len(counter_ids))
        return  stats

    def getSwitchStatsExt(self, counter_ids):
        ''' function gets the regular switch counter stats
            using sai_thrift_get_switch_stats_ext() '''
        if not isinstance(counter_ids, list):
            counter_ids = [counter_ids]
        stats = self.client.sai_thrift_get_switch_stats_ext(0, counter_ids, len(counter_ids))
        return  stats

    def set_counter_reasons(self, dc_oid, in_drop_reasons):
        if not isinstance(in_drop_reasons, list):
            in_drop_reasons = [in_drop_reasons]

        dc_reasons_list = sai_thrift_s32_list_t(count=len(in_drop_reasons), s32list=in_drop_reasons)
        dc_reasons_value = sai_thrift_attribute_value_t(s32list=dc_reasons_list)
        dc_reasons = sai_thrift_attribute_t(id=SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, value=dc_reasons_value)

        self.client.sai_thrift_set_debug_counter_attribute(dc_oid, dc_reasons)

        return

    def isInDropReasonSupported(self, drop_reason_list):

        # get the supported IN drop reason capabilities
        in_dc_caps_list = self.client.sai_thrift_query_attribute_enum_values_capability(
            SAI_OBJECT_TYPE_DEBUG_COUNTER,
            SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST,
            20)
        # print("Supported drop_reasons=%s" %(self.mapDropReasonToString(in_dc_caps_list)))
        for drop_reason in (drop_reason_list):
            found = False
            for in_drop_reason in (in_dc_caps_list):
                if (drop_reason == in_drop_reason):
                    found = True
            if (found == False):
                # print("Unable to find drop reason: %s (%d)"
                #       %(self.mapDropReasonToString([drop_reason]), drop_reason))
                return False

        return True

    def create_debug_counter(self, dc_type, in_drop_reasons):
        if not isinstance(in_drop_reasons, list):
             in_drop_reasons = [in_drop_reasons]
        #if self.dc_oid is not None:
        #    self.set_counter_reasons(in_drop_reasons)
        #    return
        dc_type_value = sai_thrift_attribute_value_t(s32=dc_type)
        dc_type_attr = sai_thrift_attribute_t(id=SAI_DEBUG_COUNTER_ATTR_TYPE, value=dc_type_value)
        dc_reasons_list = sai_thrift_s32_list_t(count=len(in_drop_reasons), s32list=in_drop_reasons)
        dc_reasons_value = sai_thrift_attribute_value_t(s32list=dc_reasons_list)

        dc_reasons = sai_thrift_attribute_t(id=SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, value=dc_reasons_value)
        self.dc_oid = self.client.sai_thrift_create_debug_counter([dc_type_attr, dc_reasons])

        return

    def create_counter(self, in_drop_reasons):
        if not isinstance(in_drop_reasons, list):
            in_drop_reasons = [in_drop_reasons]

        #if self.dc_oid is not None:
        #    self.set_counter_reasons(in_drop_reasons)
        #    return

        dc_type_value = sai_thrift_attribute_value_t(s32=SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS)
        dc_type = sai_thrift_attribute_t(id=SAI_DEBUG_COUNTER_ATTR_TYPE, value=dc_type_value)

        dc_reasons_list = sai_thrift_s32_list_t(count=len(in_drop_reasons), s32list=in_drop_reasons)
        dc_reasons_value = sai_thrift_attribute_value_t(s32list=dc_reasons_list)
        dc_reasons = sai_thrift_attribute_t(id=SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, value=dc_reasons_value)

        self.dc_oid = self.client.sai_thrift_create_debug_counter([dc_type, dc_reasons])

        return

    def mapDropReasonToString(self, drop_reason_list):
        names = []
        for drop_reason in drop_reason_list:
            if (drop_reason == SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER):
                names.append("VLAN_FILTER")
            elif (drop_reason == SAI_IN_DROP_REASON_L2_ANY):
                names.append("L2_ANY")
            elif (drop_reason == SAI_IN_DROP_REASON_SMAC_MULTICAST):
                names.append("SMAC_MC")
            elif (drop_reason == SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC):
                names.append("SMAC_EQUALS_DMAS")
            elif (drop_reason == SAI_IN_DROP_REASON_TTL):
                names.append("TTL")
            elif (drop_reason == SAI_IN_DROP_REASON_IP_HEADER_ERROR):
                names.append("IP_HEADER_ERROR")
            elif (drop_reason == SAI_IN_DROP_REASON_SIP_MC):
                names.append("SIP_MC")
            elif (drop_reason == SAI_IN_DROP_REASON_LPM6_MISS):
                names.append("LPM6_MISS")
            elif (drop_reason == SAI_IN_DROP_REASON_SIP_CLASS_E):
                names.append("SIP_CLASS_E")
            elif (drop_reason == SAI_IN_DROP_REASON_L3_ANY):
                names.append("L3_ANY")
            elif (drop_reason == SAI_IN_DROP_REASON_IRIF_DISABLED):
                names.append("IRIF_DISABLED")
            elif (drop_reason == SAI_IN_DROP_REASON_ACL_ANY):
                names.append("ACL_ANY")
            elif (drop_reason == SAI_IN_DROP_REASON_DIP_LOOPBACK):
                names.append("DIP_LOOPBACK")
            elif (drop_reason == SAI_IN_DROP_REASON_SIP_LOOPBACK):
                names.append("SIP_LOOPBACK")
            else:
                names.append("<undefined>")
        names = ",".join(names)

        return names

    def verifyDebugCounterDropPackets(self, dc_oid, drop_reason_list):

        port = port_list[0]

        pkt_ihl = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src=self.default_mac_1,
                                    ip_ihl=1)
        pkt_filter_vlan = simple_tcp_packet(
            eth_dst=self.default_mac_2,
            eth_src=self.default_mac_1,
            vlan_vid=42, dl_vlan_enable=True)
        pkt_src_ip_mc = simple_tcp_packet(eth_dst=router_mac,
                                          eth_src=self.default_mac_1,
                                          ip_src=self.mc_ip)
        src_mac_mc_pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                           eth_src=self.mc_mac)
        smas_equals_dmsc_pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                           eth_src=self.default_mac_1)
        zero_smac_pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                          eth_src='00:00:00:00:00:00')
        ttl_zero_pkt = simple_tcp_packet(eth_dst=router_mac,
                                         eth_src=self.default_mac_1,
                                         ip_dst=self.neighbor_ip,
                                         ip_ttl=0)
        pkt_src_ip_class_e = simple_tcp_packet(eth_dst=router_mac,
                                               eth_src=self.default_mac_1,
                                               ip_src=self.class_e_ip)
        dst_ipv4_loopback_pkt = simple_tcp_packet(eth_dst=router_mac,
                                                  eth_src=self.default_mac_1,
                                                  ip_dst=self.loopback_ipv4)
        src_ipv4_loopback_pkt = simple_tcp_packet(eth_dst=router_mac,
                                                  eth_src=self.default_mac_1,
                                                  ip_src=self.loopback_ipv4)

        map_drop_reasons = []
        map_drop_reasons.append([SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER, [pkt_filter_vlan]])
        map_drop_reasons.append([SAI_IN_DROP_REASON_IP_HEADER_ERROR, [pkt_ihl]])
        map_drop_reasons.append([SAI_IN_DROP_REASON_SIP_MC, [pkt_src_ip_mc]])
        map_drop_reasons.append([SAI_IN_DROP_REASON_SMAC_MULTICAST, [src_mac_mc_pkt]])
        map_drop_reasons.append([SAI_IN_DROP_REASON_TTL, [ttl_zero_pkt]])
        map_drop_reasons.append([SAI_IN_DROP_REASON_SIP_CLASS_E, [pkt_src_ip_class_e]])
        map_drop_reasons.append([SAI_IN_DROP_REASON_DIP_LOOPBACK, [dst_ipv4_loopback_pkt]])
        map_drop_reasons.append([SAI_IN_DROP_REASON_SIP_LOOPBACK, [src_ipv4_loopback_pkt]])
        if (self.isInDropReasonSupported([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC])):
            map_drop_reasons.append([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC, [smas_equals_dmsc_pkt]])
            map_drop_reasons.append([SAI_IN_DROP_REASON_L2_ANY, [src_mac_mc_pkt, zero_smac_pkt, pkt_filter_vlan]])
        else:
            map_drop_reasons.append([SAI_IN_DROP_REASON_L2_ANY, [src_mac_mc_pkt, pkt_filter_vlan]])


        dc_pkt_cnt = 0
        found_pkt = False
        print("Verify drop reasons = %s" %(self.mapDropReasonToString(drop_reason_list)))
        port = port_list[self.ingress_port]
        # sai_thrift_clear_all_counters(self.client)
        counter = self.client.sai_thrift_get_debug_counter_port_stats_by_oid(port, dc_oid)
        sw_counter_before = self.getPortStatsExt(port, SAI_PORT_STAT_IF_IN_DISCARDS)[0]
        for dc_drop_reason in drop_reason_list:
            for map_drop_reason, pkts in map_drop_reasons:
                if (map_drop_reason == dc_drop_reason):
                    # found corresponding test packet(s)
                    found_pkt = True
                    break
            if (found_pkt):
                dc_pkt_cnt += len(pkts)
                for pkt in pkts:
                    send_packet(self, self.ingress_port, str(pkt))
                    verify_no_packet(self, pkt, self.egress_port)
            else:
                # skip check as there is no packet sent
                continue
        dc_counter_after = self.client.sai_thrift_get_debug_counter_port_stats_by_oid(port, dc_oid)
        sw_counter_after = self.getPortStatsExt(port, SAI_PORT_STAT_IF_IN_DISCARDS)[0]
        if ((counter + dc_pkt_cnt) != dc_counter_after):
            # debug counter has invalid value
            return False
        if (sw_counter_after != sw_counter_before + dc_pkt_cnt):
            # port counter has invalid value
            return False
        return True

    def verifyDebugCounterDropList(self, dc_oid, drop_reasons):

        attr = sai_thrift_get_debug_counter_attribute(
            self.client, dc_oid,
            SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, 10)
        if (len(drop_reasons) != attr.value.s32list.count):
            print("SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST incorrect count=",
                   attr.value.s32list.count)
            return False

        # verify if IN list is correct
        found = False
        for drop_reason in (drop_reasons):
            found = False
            for i in range(0, attr.value.s32list.count):
                if (attr.value.s32list.s32list[i] == drop_reason):
                    found = True
            if (found is not True):
                return False
        return True


    def create_router(self):
        self.vr_ids.append(sai_thrift_create_virtual_router(self.client, self.v4_enabled, self.v6_enabled))

        self.rif_ids.append(sai_thrift_create_router_interface(self.client, self.vr_ids[-1], SAI_ROUTER_INTERFACE_TYPE_PORT, port_list[self.ingress_port], 0, self.v4_enabled, self.v6_enabled, router_mac))
        self.rif_ids.append(sai_thrift_create_router_interface(self.client, self.vr_ids[-1], SAI_ROUTER_INTERFACE_TYPE_PORT, port_list[self.egress_port], 0, self.v4_enabled, self.v6_enabled, router_mac))

        return

    def set_rif_attribute(self, rif_id, attr_id, attr_value):
        rif_attribute_value=None
        if attr_id == SAI_ROUTER_INTERFACE_ATTR_MTU:
            rif_attribute_value = sai_thrift_attribute_value_t(u32=attr_value)
        elif attr_id == SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE:
            rif_attribute_value = sai_thrift_attribute_value_t(booldata=attr_value)
        elif attr_id == SAI_ROUTER_INTERFACE_ATTR_LOOPBACK_PACKET_ACTION:
            rif_attribute_value = sai_thrift_attribute_value_t(s32=attr_value)
        else:
            return

        rif_attribute = sai_thrift_attribute_t(id    = attr_id,
                                               value = rif_attribute_value)
        self.client.sai_thrift_set_router_interface_attribute(rif_id, rif_attribute)
        return

    def create_route(self, packet_action=None):
        self.routes.append([self.vr_ids[-1], self.addr_family, self.ip_addr_subnet, self.ip_mask, self.rif_ids[-1], packet_action])
        sai_thrift_create_route(self.client, *self.routes[-1])

        return

    def create_route_v6(self):
        self.routes.append([self.vr_ids[-1], self.addr_family_v6, self.ip_addr_subnet_v6, self.ip_mask_v6, self.rif_ids[-1]])
        sai_thrift_create_route(self.client, *self.routes[-1])

        return

    def create_neighbor(self):
        self.neighbors.append([self.addr_family, self.rif_ids[-1], self.neighbor_ip, self.neighbor_mac])
        sai_thrift_create_neighbor(self.client, *self.neighbors[-1])

        return

    def create_neighbor_v6(self):
        self.neighbors.append([self.addr_family_v6, self.rif_ids[-1], self.neighbor_ip_v6, self.neighbor_mac])
        sai_thrift_create_neighbor(self.client, *self.neighbors[-1])

        return

    def create_fdb(self, port, mac, action):
        self.fdbs.append([switch.default_vlan.oid, mac, port, action])
        sai_thrift_create_fdb(self.client, *self.fdbs[-1])

        return

    def test_port_debug_counter(self, in_drop_reasons, pkts, result=1):
        if not isinstance(pkts, list):
            pkts = [pkts]

        print("saidebugcounters.py.test_port_debug_counter(PORT_IN_DROP_REASONS)")
        self.create_debug_counter(SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS, in_drop_reasons)
        port = port_list[0]
        thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, self.dc_oid, SAI_DEBUG_COUNTER_ATTR_INDEX, 1)
        index = thrift_attr.value.u32 + SAI_PORT_STAT_IN_DROP_REASON_RANGE_BASE
        stats = self.getPortStatsExt(port, [index, SAI_PORT_STAT_IF_IN_DISCARDS])
        dc_counter_before = stats[0]
        sw_counter_before = stats[1]

        try:
            for pkt in pkts:
                send_packet(self, self.ingress_port, str(pkt))
                if result != 0:
                    verify_no_packet(self, pkt, self.egress_port)

            stats = self.getPortStatsExt(port, [index, SAI_PORT_STAT_IF_IN_DISCARDS])
            dc_counter_after = stats[0]
            sw_counter_after = stats[1]
            print("DebugCounter before test=", dc_counter_before, "DebugCounter after test=", dc_counter_after," result=", result)
            print("Port counter before test=", sw_counter_before, "Port counter after test=", sw_counter_after," result=", result)

            self.assertTrue(0 != result)
            expexted_drop_packets = dc_counter_before + len(pkts)
            self.assertTrue(dc_counter_after == expexted_drop_packets)
            self.assertTrue(sw_counter_after == sw_counter_before + len(pkts))
        finally:
            pass

        return

    def test_switch_counter(self, in_drop_reasons, pkts, result=1):
        if not isinstance(pkts, list):
            pkts = [pkts]

        try:
            self.create_debug_counter(SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS, in_drop_reasons)
            dc_counter_before = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)
            sw_counter_before = self.getSwitchStatsExt(SAI_PORT_STAT_IF_IN_DISCARDS)[0]
            print("Switch counter before=", dc_counter_before)

            for pkt in pkts:
                send_packet(self, self.ingress_port, str(pkt))
                if result != 0:
                    verify_no_packet(self, pkt, self.egress_port)

            dc_counter_after = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)
            sw_counter_after = self.getSwitchStatsExt(SAI_PORT_STAT_IF_IN_DISCARDS)[0]
            print("DebugCounter before=", dc_counter_before, "DebugCounter after=", dc_counter_after," result=", result)
            print("Switch counter before=", sw_counter_before, "Switch counter after=", sw_counter_after," result=", result)
            if result != 0:
                # verify both DebugCounter counter and switch IN_DISCARDS counter
                self.assertTrue(dc_counter_after == dc_counter_before + 1)
                self.assertTrue(sw_counter_after == sw_counter_before + 1)
            else:
                # no drop expected
                self.assertTrue(dc_counter_after == dc_counter_before)
                self.assertTrue(sw_counter_after == sw_counter_before)
        finally:
            pass

        return

    def cleanup(self):
        self.ingress_port  = 0
        self.egress_port   = 1
        del self.fdbs[:]
        del self.vr_ids[:]
        del self.rif_ids[:]
        del self.neighbors[:]
        del self.routes[:]
        self.dc_oid=None
        self.second_dc_oid=None

        return

    def setUp(self):
        ThriftInterfaceDataPlane.setUp(self)
        sai_thrift_clear_all_counters(self.client)
        self.cleanup()

        return

    def tearDown(self):
        if self.dc_oid is not None:
            self.client.sai_thrift_remove_debug_counter(self.dc_oid)
        if self.second_dc_oid is not None:
            self.client.sai_thrift_remove_debug_counter(self.second_dc_oid)

        for route_data in self.routes:
            sai_thrift_remove_route(self.client, *route_data[:-1])

        for neighbor_data in self.neighbors:
            sai_thrift_remove_neighbor(self.client, *neighbor_data)

        for fdb in self.fdbs:
            sai_thrift_delete_fdb(self.client, *fdb[:-1])

        for rif in self.rif_ids:
            self.client.sai_thrift_remove_router_interface(rif)

        for vr in self.vr_ids:
            self.client.sai_thrift_remove_virtual_router(vr)

        self.cleanup()

        ThriftInterfaceDataPlane.tearDown(self)

        return

""" Debug counters API tests """

""" L2 reasons """

@group('debug_counters') # tested
class PortDropMCSMAC(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                eth_src=self.mc_mac)

        self.test_port_debug_counter([SAI_IN_DROP_REASON_SMAC_MULTICAST], [pkt])

        return

@group('debug_counters') # drop reason supported Counter does not work
class PortDropL2Any(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        test_pkts = []
        mc_smac_pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                        eth_src=self.mc_mac)
        zero_smac_pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                          eth_src='00:00:00:00:00:00')
        vlan_discard_pkt = simple_tcp_packet(eth_dst=self.default_mac_2,
                                             eth_src=self.default_mac_1,
                                             vlan_vid=42, dl_vlan_enable=True)

        self.test_port_debug_counter([SAI_IN_DROP_REASON_L2_ANY],
                                     [zero_smac_pkt, mc_smac_pkt,
                                      vlan_discard_pkt])

        return


@pktpy_skip  # TODO bf-pktpy
@group('debug_counters') # drop reason supported Counter does not work
class PortDropL2AnyMultiReasons(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        test_pkts = []
        mc_smac_pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                        eth_src=self.mc_mac)
        zero_smac_pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                          eth_src='00:00:00:00:00:00')
        vlan_discard_pkt = simple_tcp_packet(eth_dst=self.default_mac_2,
                                             eth_src=self.default_mac_1,
                                             vlan_vid=42, dl_vlan_enable=True)
        # test demonstrates that each counter is counted only once
        # even SAI_IN_DROP_REASON_L2_ANY reason cumulates the following counters too
        # - SAI_IN_DROP_REASON_SMAC_MULTICAST
        # - SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER
        self.test_port_debug_counter([SAI_IN_DROP_REASON_L2_ANY,
                                      SAI_IN_DROP_REASON_SMAC_MULTICAST,
                                      SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER],
                                     [zero_smac_pkt, mc_smac_pkt,
                                      vlan_discard_pkt])

        return

@group('debug_counters') # Conditional test per profile
class PortDropSMACequalsDMAC(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        if (self.isInDropReasonSupported([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC])):
            pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                    eth_src=self.default_mac_1)

            self.test_port_debug_counter([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC], pkt)

        return

@group('debug_counters')
@disabled
class PortDropDMACReserved(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        pkt = simple_tcp_packet(eth_dst=self.reserved_mac,
                                eth_src=self.default_mac_1)

        self.test_port_debug_counter([SAI_IN_DROP_REASON_DMAC_RESERVED], pkt)

        return

@group('debug_counters') # tested
class PortDropIngressVLANFilter(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        pkt = simple_tcp_packet(eth_dst=self.default_mac_2,
                                eth_src=self.default_mac_1,
                                vlan_vid=42, dl_vlan_enable=True)

        self.test_port_debug_counter([SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER], pkt)

        return

@group('debug_counters') # tested
class SwitchDropIngressVLANFilter(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        pkt = simple_tcp_packet(eth_dst=self.default_mac_2,
                                eth_src=self.default_mac_1,
                                vlan_vid=42, dl_vlan_enable=True)

        self.test_switch_counter([SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER], pkt)

        return

@group('debug_counters') # tested
class SwitchDropMCSMAC(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        pkt = simple_tcp_packet(eth_dst=self.default_mac_1,
                                eth_src=self.mc_mac)

        self.test_switch_counter([SAI_IN_DROP_REASON_SMAC_MULTICAST], pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropL2LoopbackFilter(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_fdb(port_list[0], self.default_mac_2, SAI_PACKET_ACTION_FORWARD)

        pkt = simple_tcp_packet(eth_dst=self.default_mac_2,
                                eth_src=self.default_mac_1)

        self.test_switch_counter(SAI_IN_DROP_REASON_L2_LOOPBACK_FILTER, pkt)

        return

""" L3 reasons """

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropL3LoopbackFilter(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()
        self.set_rif_attribute(self.rif_ids[1], SAI_ROUTER_INTERFACE_ATTR_LOOPBACK_PACKET_ACTION, SAI_PACKET_ACTION_DROP)
        self.create_neighbor()
        self.create_route()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.neighbor_ip,
                                ip_src=self.neighbor_ip_2)
        self.ingress_port = 1
        self.egress_port = 0
        self.test_switch_counter(SAI_IN_DROP_REASON_L3_LOOPBACK_FILTER, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@pktpy_skip
@disabled
class DropNonRoutable(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_igmp_packet(eth_dst=router_mac,
                                 eth_src=self.default_mac_1)

        self.test_switch_counter(SAI_IN_DROP_REASON_NON_ROUTABLE, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropNoL3Header(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_arp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1)

        self.test_switch_counter(SAI_IN_DROP_REASON_NO_L3_HEADER, pkt)

        return

@group('debug_counters') # Tested
class DropIPHeaderError(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_ihl=1)

        self.test_switch_counter(SAI_IN_DROP_REASON_IP_HEADER_ERROR, pkt)

        return

@group('debug_counters') # tested
class DropUCDIPMCDMAC(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        print("IPv4 packet...")
        pkt_ipv4 = simple_tcp_packet(eth_dst=self.mc_mac,
                                eth_src=self.default_mac_1)
        self.test_switch_counter(SAI_IN_DROP_REASON_UC_DIP_MC_DMAC, pkt_ipv4)

        print("IPv6 packet...")
        pkt_ipv6 = simple_tcpv6_packet(eth_dst=self.mc_mac,
                                eth_src=self.default_mac_1)
        self.test_switch_counter(SAI_IN_DROP_REASON_UC_DIP_MC_DMAC, pkt_ipv6)

        return


@pktpy_skip  # TODO bf-pktpy
@group('debug_counters') # tested
class DropDIPLoopback(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        print("Send loopback_ipv4 packet")
        pkt_ipv4 = simple_tcp_packet(eth_dst=router_mac,
                                     eth_src=self.default_mac_1,
                                     ip_dst=self.loopback_ipv4)
        self.test_switch_counter(SAI_IN_DROP_REASON_DIP_LOOPBACK, pkt_ipv4)

        print("Send loopback_ipv6 packet")
        pkt_ipv6 = simple_tcpv6_packet(eth_dst=router_mac,
                                       eth_src=self.default_mac_1,
                                       ipv6_dst=self.loopback_ipv6[0])
        self.test_switch_counter(SAI_IN_DROP_REASON_DIP_LOOPBACK, pkt_ipv6)

        pkt_ipv6 = simple_tcpv6_packet(eth_dst=router_mac,
                               eth_src=self.default_mac_1,
                               ipv6_dst=self.loopback_ipv6[1])
        self.test_switch_counter(SAI_IN_DROP_REASON_DIP_LOOPBACK, pkt_ipv6)

        return

@group('debug_counters') # tested
class DropSIPLoopback(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_src=self.loopback_ipv4)

        self.test_switch_counter(SAI_IN_DROP_REASON_SIP_LOOPBACK, pkt)

        return

@group('debug_counters') # tested
class DropMulticastSIP(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_src=self.mc_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_SIP_MC, pkt)

        return

@group('debug_counters') # tested
class DropSIPClassE(BaseDebugCounterTest):
    def runTest(self):
        #TODO: Add validation for bcast ip
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_src=self.class_e_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_SIP_CLASS_E, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropSIPUnspecified(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_src=self.unspec_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_SIP_UNSPECIFIED, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropMCDMACMismatch(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=self.mc_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.mc_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_MC_DMAC_MISMATCH, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropSIPEqualsDIP(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.neighbor_ip,
                                ip_src=self.neighbor_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_SIP_EQUALS_DIP, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropSIPBC(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_src=self.bc_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_SIP_BC, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropDIPLocal(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.unspec_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_DIP_LOCAL, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropDIPLinkLocal(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.link_local_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_DIP_LINK_LOCAL, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropSIPLinkLocal(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_src=self.link_local_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_SIP_LINK_LOCAL, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropIPv6MCScope0(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcpv6_packet(eth_dst=router_mac,
                                  eth_src=self.default_mac_1,
                                  ipv6_dst=self.mc_scope_0_ipv6)

        self.test_switch_counter(SAI_IN_DROP_REASON_IPV6_MC_SCOPE0, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropIPv6MCScope1(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcpv6_packet(eth_dst=router_mac,
                                  eth_src=self.default_mac_1,
                                  ipv6_dst=self.mc_scope_1_ipv6)

        self.test_switch_counter(SAI_IN_DROP_REASON_IPV6_MC_SCOPE1, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropIRIFDisabled(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()
        self.set_rif_attribute(self.rif_ids[0], SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE, False)
        self.create_neighbor()
        self.create_route()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.neighbor_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_IRIF_DISABLED, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropERIFDisabled(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()
        self.set_rif_attribute(self.rif_ids[1], SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE, False)
        self.create_neighbor()
        self.create_route()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.neighbor_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_ERIF_DISABLED, pkt)

        return

@group('debug_counters') # unsupported Drop Reason
@disabled
class L3IPv4PortDropTTLCheck(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()
        self.set_rif_attribute(self.rif_ids[1], SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE, False)
        self.create_neighbor()
        self.create_route()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.neighbor_ip,
                                ip_ttl=0)

        self.test_port_debug_counter([SAI_IN_DROP_REASON_TTL], pkt)

        return

@group('debug_counters') # unsupported Drop Reason
@disabled
class L3IPv4SwitchDropTTLCheck(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()
        self.set_rif_attribute(self.rif_ids[1], SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE, False)
        self.create_neighbor()
        self.create_route()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.neighbor_ip,
                                ip_ttl=0)

        self.test_switch_counter(SAI_IN_DROP_REASON_TTL, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropLPM4Miss(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1)

        self.test_switch_counter(SAI_IN_DROP_REASON_LPM4_MISS, pkt)

        return

#@group('debug_counters') # unsupported Switch Drop Reason
@disabled
class DropLPM6Miss(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt = simple_tcpv6_packet(eth_dst=router_mac,
                                  eth_src=self.default_mac_1)

        self.test_switch_counter(SAI_IN_DROP_REASON_LPM6_MISS, pkt)

        return

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropBlackholeRoute(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()
        self.create_route(SAI_PACKET_ACTION_DROP)

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.neighbor_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_BLACKHOLE_ROUTE, pkt)

        return

""" ACL reasons """

#@group('debug_counters') # unsupported Drop Reason
@disabled
class DropACLAny(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()
        self.create_neighbor()
        self.create_route()

        table_stage           = SAI_ACL_STAGE_INGRESS
        table_bind_point_list = [SAI_ACL_BIND_POINT_TYPE_PORT]
        entry_priority        = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY
        action                = SAI_PACKET_ACTION_DROP
        in_ports              = port_list[0], port_list[1]
        mac_src               = None
        mac_dst               = None
        mac_src_mask          = None
        mac_dst_mask          = "ff:ff:ff:ff:ff:ff"
        ip_src                = "192.168.0.1"
        ip_src_mask           = "255.255.255.0"
        ip_dst                = None
        ip_dst_mask           = None
        ip_proto              = None
        in_port               = None
        out_port              = None
        out_ports             = None
        src_l4_port           = None
        dst_l4_port           = None
        ingress_mirror_id     = None
        egress_mirror_id      = None

        acl_table_id = sai_thrift_create_acl_table(self.client,
            table_stage,
            table_bind_point_list,
            self.addr_family,
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
            dst_l4_port)
        acl_entry_id = sai_thrift_create_acl_entry(self.client,
            acl_table_id,
            entry_priority,
            action, self.addr_family,
            mac_src, mac_src_mask,
            mac_dst, mac_dst_mask,
            ip_src, ip_src_mask,
            ip_dst, ip_dst_mask,
            ip_proto,
            in_ports, out_ports,
            in_port, out_port,
            src_l4_port, dst_l4_port,
            ingress_mirror_id,
            egress_mirror_id)

        attr_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port_list[0], attr)

        pkt = simple_tcp_packet(eth_dst=router_mac,
                                eth_src=self.default_mac_1,
                                ip_dst=self.neighbor_ip)

        self.test_switch_counter(SAI_IN_DROP_REASON_ACL_ANY, pkt)

        attr_value = sai_thrift_attribute_value_t(oid=SAI_NULL_OBJECT_ID)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_INGRESS_ACL, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port_list[0], attr)

        return


""" Negative cases """


@pktpy_skip  # TODO bf-pktpy
@group('debug_counters') # tested
class NoDropIngressVLANFilter(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        pkt = simple_tcp_packet(eth_dst=self.default_mac_2,
                                eth_src=self.default_mac_1)

        self.test_switch_counter(SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER, pkt, 0)

        return

""" Multiple reasons """

#@group('debug_counters') # does not support SAI_IN_DROP_REASON_LPM6_MISS drop reason
@disabled
class DropMultipleReasons(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt_ipv6_miss = simple_tcpv6_packet(eth_dst=router_mac,
                                            eth_src=self.default_mac_1)

        pkt_vlan = simple_tcp_packet(eth_dst=self.default_mac_2,
                                     eth_src=self.default_mac_1,
                                     vlan_vid=42, dl_vlan_enable=True)

        self.test_switch_counter([SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER, SAI_IN_DROP_REASON_LPM6_MISS],
                [pkt_vlan, pkt_ipv6_miss], 2)

        return


""" Editing drop reasons """


@pktpy_skip  # TODO bf-pktpy
@group('debug_counters') # tested modified second drop reason.
class EditingDropReasons(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        #pkt_ipv6_miss = simple_tcpv6_packet(eth_dst=router_mac,
        #                                    eth_src=self.default_mac_1)
        pkt_ihl = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src=self.default_mac_1,
                                    ip_ihl=1)

        pkt_vlan = simple_tcp_packet(eth_dst=self.default_mac_2,
                                     eth_src=self.default_mac_1,
                                     vlan_vid=42, dl_vlan_enable=True)

        self.create_debug_counter(
            SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS,
            [SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER])
        self.assertTrue(self.verifyDebugCounterDropList(self.dc_oid,
                                                   [SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER]))
        counter = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)

        try:
            send_packet(self, self.ingress_port, str(pkt_vlan))
            verify_no_packet(self, pkt_vlan, self.egress_port)
        finally:
            dc_counter_after = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)
            self.assertTrue(dc_counter_after == counter + 1)

        self.set_counter_reasons(self.dc_oid, [SAI_IN_DROP_REASON_IP_HEADER_ERROR])
        counter = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)
        try:
            send_packet(self, self.ingress_port, str(pkt_ihl))
            verify_no_packet(self, pkt_ihl, self.egress_port)
        finally:
            dc_counter_after = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)

            print("counter before test=", counter, " counter after test=", dc_counter_after)
            self.assertTrue(dc_counter_after == counter + 1)

        self.set_counter_reasons(self.dc_oid,
                                 [SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER,
                                  SAI_IN_DROP_REASON_IP_HEADER_ERROR])
        counter = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)
        try:
            send_packet(self, self.ingress_port, str(pkt_vlan))
            verify_no_packet(self, pkt_vlan, self.egress_port)
            send_packet(self, self.ingress_port, str(pkt_ihl))
            verify_no_packet(self, pkt_ihl, self.egress_port)
        finally:
            # counter read does not clear the counters
            dc_counter_after = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)
            print("counter before test=", counter, " counter after test=", dc_counter_after)
            self.assertTrue(dc_counter_after == counter + 2)

        self.set_counter_reasons(self.dc_oid,
                                 [SAI_IN_DROP_REASON_IP_HEADER_ERROR])
        counter = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)
        self.assertTrue(self.verifyDebugCounterDropList(self.dc_oid,
                                                   [SAI_IN_DROP_REASON_IP_HEADER_ERROR]))
        try:
            send_packet(self, self.ingress_port, str(pkt_ihl))
            send_packet(self, self.ingress_port, str(pkt_vlan))
            verify_no_packet(self, pkt_ihl, self.egress_port)
        finally:
            dc_counter_after = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(self.dc_oid)
            print("counter before test=", counter, " counter after test=", dc_counter_after)
            self.assertTrue(dc_counter_after == counter + 1)

        return


@pktpy_skip  # TODO bf-pktpy
@group('debug_counters')
class MultipleDebugCounters(BaseDebugCounterTest):

    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt_ihl = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src=self.default_mac_1,
                                    ip_ihl=1)

        pkt_vlan = simple_tcp_packet(eth_dst=self.default_mac_2,
                                     eth_src=self.default_mac_1,
                                     vlan_vid=42, dl_vlan_enable=True)

        self.create_debug_counter(SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS,
                                  [SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER,
                                   SAI_IN_DROP_REASON_IP_HEADER_ERROR])
        first_dc_oid = self.dc_oid
        counter = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(first_dc_oid)

        sw_counter_before = self.getSwitchStatsExt(SAI_PORT_STAT_IF_IN_DISCARDS)[0]
        print("Begin test counter = ", counter)

        try:
            send_packet(self, self.ingress_port, str(pkt_vlan))
            verify_no_packet(self, pkt_vlan, self.egress_port)
            send_packet(self, self.ingress_port, str(pkt_ihl))
            verify_no_packet(self, pkt_vlan, self.egress_port)
        finally:
            sw_counter_after = self.getSwitchStatsExt(SAI_PORT_STAT_IF_IN_DISCARDS)[0]
            dc_counter_after = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(first_dc_oid)
            print("counter = ", counter, ", dc_counter_after=", dc_counter_after)
            self.assertTrue(dc_counter_after == counter + 2)
            # verify regular switch counter
            self.assertTrue(sw_counter_after == sw_counter_before + 2)

        # create second debug counter
        self.create_debug_counter(SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS,
                                  [SAI_IN_DROP_REASON_IP_HEADER_ERROR])
        second_dc_oid = self.dc_oid
        self.second_dc_oid = self.dc_oid
        self.dc_oid = first_dc_oid
        second_counter = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(second_dc_oid)
        first_counter = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(first_dc_oid)
        sw_counter_before = self.getSwitchStatsExt(SAI_PORT_STAT_IF_IN_DISCARDS)[0]
        print("First DC counter = ", first_counter)
        print("Second DC counter = ", second_counter)
        # self.assertTrue(1 == second_counter)
        try:
            send_packet(self, self.ingress_port, str(pkt_vlan))
            send_packet(self, self.ingress_port, str(pkt_ihl))
            verify_no_packet(self, pkt_vlan, self.egress_port)
        finally:
            first_dc_counter_after = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(first_dc_oid)
            second_dc_counter_after = self.client.sai_thrift_get_debug_counter_switch_stats_by_oid(second_dc_oid)
            sw_counter_after = self.getSwitchStatsExt(SAI_PORT_STAT_IF_IN_DISCARDS)[0]
            print("First DC counter after = ", first_dc_counter_after)
            print("Second DC counter after = ", second_dc_counter_after)
            self.assertTrue(first_dc_counter_after == first_counter + 2)
            self.assertTrue(second_dc_counter_after == second_counter + 1)
            # verify regular switch counter
            self.assertTrue(sw_counter_after == sw_counter_before + 2)
        return

@group('debug_counters')
class GetDebugCounter(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt_ihl = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src=self.default_mac_1,
                                    ip_ihl=1)

        pkt_vlan = simple_tcp_packet(eth_dst=self.default_mac_2,
                                     eth_src=self.default_mac_1,
                                     vlan_vid=42, dl_vlan_enable=True)

        self.create_debug_counter(
            SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS,
            [SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER,
             SAI_IN_DROP_REASON_IP_HEADER_ERROR])
        first_dc_oid = self.dc_oid
        try:
            send_packet(self, self.ingress_port, str(pkt_vlan))
            send_packet(self, self.ingress_port, str(pkt_ihl))
            verify_no_packet(self, pkt_vlan, self.egress_port)
        finally:
            # check the first counter
            thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, first_dc_oid, SAI_DEBUG_COUNTER_ATTR_TYPE, 10)
            if (SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS == thrift_attr.value.s32):
                print "type SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS is correct"
            thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, first_dc_oid, SAI_DEBUG_COUNTER_ATTR_INDEX, 10)
            if ((first_dc_oid & 0xFFFFFF) == thrift_attr.value.u32):
                print "type SAI_DEBUG_COUNTER_ATTR_INDEX is correct"
            self.assertTrue((first_dc_oid & 0xFFFFFF) == thrift_attr.value.u32)
            thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, first_dc_oid, SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, 10)
            if (2 == thrift_attr.value.s32list.count):
                print "type SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST is correct count=", thrift_attr.value.s32list.count
            self.assertTrue(2 == thrift_attr.value.s32list.count)

            # verify if IN list is correct
            for drop_reason in ([SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER, SAI_IN_DROP_REASON_IP_HEADER_ERROR]):
                found = 0
                for i in range(0, thrift_attr.value.s32list.count):
                    if (thrift_attr.value.s32list.s32list[i] == drop_reason):
                        found = 1
                self.assertTrue(found == 1)

            # verify there is no OUT list
            thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, first_dc_oid, SAI_DEBUG_COUNTER_ATTR_OUT_DROP_REASON_LIST, 10)
            self.assertTrue(0 == thrift_attr.value.s32list.count)

        # check the second debug counter
        self.create_debug_counter(SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS, [SAI_IN_DROP_REASON_IP_HEADER_ERROR])
        second_dc_oid = self.dc_oid
        self.second_dc_oid = second_dc_oid
        self.dc_oid = first_dc_oid

        thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, second_dc_oid, SAI_DEBUG_COUNTER_ATTR_TYPE, 10)
        if (SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS == thrift_attr.value.s32):
            print "type SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS is correct"
        thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, second_dc_oid, SAI_DEBUG_COUNTER_ATTR_INDEX, 10)
        if ((second_dc_oid & 0xFFFFFF) == thrift_attr.value.u32):
            print "type SAI_DEBUG_COUNTER_ATTR_INDEX is correct"
        self.assertTrue((second_dc_oid & 0xFFFFFF) == thrift_attr.value.u32)
        thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, second_dc_oid, SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, 10)
        if (1 == thrift_attr.value.s32list.count):
            print "type SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST is correct"
        if (SAI_IN_DROP_REASON_IP_HEADER_ERROR == thrift_attr.value.s32list.s32list[0]):
            print "type SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST is correct"
        self.assertTrue(SAI_IN_DROP_REASON_IP_HEADER_ERROR == thrift_attr.value.s32list.s32list[0])
        self.assertTrue(1 == thrift_attr.value.s32list.count)

        # for second debug counter should be 0 out drop reasons
        thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, second_dc_oid, SAI_DEBUG_COUNTER_ATTR_OUT_DROP_REASON_LIST, 10)
        print("thrift_attr.value.s32list.count=", thrift_attr.value.s32list.count)
        self.assertTrue(0 == thrift_attr.value.s32list.count)

        # extends the second debug counter
        drop_reason_list = [SAI_IN_DROP_REASON_IP_HEADER_ERROR,
                            SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER]
        if (self.isInDropReasonSupported([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC])):
            drop_reason_list.append(SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC)

        self.set_counter_reasons(second_dc_oid, drop_reason_list)

        # verify the second debug counter for in drop reasons
        thrift_attr = sai_thrift_get_debug_counter_attribute(self.client, second_dc_oid, SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, 10)
        # print("thrift_attr.value.s32list.count=", thrift_attr.value.s32list.count)
        print("thrift_attr.value.s32list=", thrift_attr.value.s32list)
        self.assertTrue(len(drop_reason_list) == thrift_attr.value.s32list.count)

        self.assertTrue(self.verifyDebugCounterDropList(
            second_dc_oid, drop_reason_list))


@pktpy_skip  # TODO bf-pktpy
@group('debug_counters')
class DebugCounterRemoveDropReason(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        try:
            drop_reason_cap = [
                SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER,\
                SAI_IN_DROP_REASON_L2_ANY,\
                SAI_IN_DROP_REASON_SMAC_MULTICAST,\
                SAI_IN_DROP_REASON_TTL,\
                SAI_IN_DROP_REASON_IP_HEADER_ERROR,\
                SAI_IN_DROP_REASON_SIP_MC,
                SAI_IN_DROP_REASON_SIP_CLASS_E,
                SAI_IN_DROP_REASON_DIP_LOOPBACK,
                SAI_IN_DROP_REASON_SIP_LOOPBACK
                ]

            if (self.isInDropReasonSupported([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC])):
                drop_reason_cap.append(SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC)

            drop_reason_list = [SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER]
            self.create_debug_counter(
                SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS,
                drop_reason_list)

            self.assertTrue(self.verifyDebugCounterDropList(
                    self.dc_oid, drop_reason_list))

            self.set_counter_reasons(self.dc_oid, [])
            self.assertTrue(self.verifyDebugCounterDropList(
                    self.dc_oid, []))

            drop_reason_list = drop_reason_cap
            self.set_counter_reasons(self.dc_oid, drop_reason_list)
            self.assertTrue(self.verifyDebugCounterDropList(
                    self.dc_oid, drop_reason_list))

            for i in range(0, len(drop_reason_cap)):
                if (i == 0):
                    print("Verify initial DebugCounter drop_reason_list with traffic:", drop_reason_list)
                    self.assertTrue(self.verifyDebugCounterDropPackets(
                        self.dc_oid, drop_reason_list))

                # remove drop reason
                drop_reason_list.pop(0)
                print("Setting DebugCounter drop_reason_list to:", drop_reason_list)
                self.set_counter_reasons(self.dc_oid, drop_reason_list)

                print("Verify updated DebugCounter drop_reason_list")
                attr = sai_thrift_get_debug_counter_attribute(
                    self.client,
                    self.dc_oid,
                    SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, 20)
                self.assertTrue(len(drop_reason_list) == attr.value.s32list.count)
                self.assertTrue(self.verifyDebugCounterDropList(
                    self.dc_oid, drop_reason_list))
                print("\tok")

                print("Verify updated DebugCounter drop_reason_list with traffic")
                # verify packets after removing the drop_reason
                self.assertTrue(self.verifyDebugCounterDropPackets(
                    self.dc_oid, drop_reason_list))
                print("\tok")
        finally:
            pass

@group('debug_counters')
class DebugCounterAddDropReasonDuplicate(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        try:
            self.create_debug_counter(
                SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS,
                [SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER])
            drop_reason_cap = [
                SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER,\
                SAI_IN_DROP_REASON_SMAC_MULTICAST, \
                SAI_IN_DROP_REASON_L2_ANY,\
                SAI_IN_DROP_REASON_TTL,\
                SAI_IN_DROP_REASON_IP_HEADER_ERROR,\
                SAI_IN_DROP_REASON_SIP_MC,
                SAI_IN_DROP_REASON_SIP_CLASS_E,
                SAI_IN_DROP_REASON_DIP_LOOPBACK,
                SAI_IN_DROP_REASON_SIP_LOOPBACK
                ]

            if (self.isInDropReasonSupported([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC])):
                drop_reason_cap.append(SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC)

            self.set_counter_reasons(self.dc_oid, drop_reason_cap)
            drop_reason_list = []

            for drop_reason in drop_reason_cap:
                for dr in drop_reason_cap:
                    drop_reason_list.append(dr)

                drop_reason_list.append(drop_reason)
                print("Adding duplicate drop_reason : %s" %(self.mapDropReasonToString([drop_reason])))
                # expected error
                self.set_counter_reasons(self.dc_oid, drop_reason_list)
                attr = sai_thrift_get_debug_counter_attribute(
                    self.client,
                    self.dc_oid,
                    SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, 20)
                self.assertTrue(len(drop_reason_cap) == attr.value.s32list.count)
                print("Verify the DebugCounter drop_reason_list has not been updated")
                self.assertTrue(self.verifyDebugCounterDropList(
                    self.dc_oid, drop_reason_cap))
                # print("Verify updated DebugCounter drop_reason_list with traffic")
                # self.assertTrue(self.verifyDebugCounterDropPackets(self.dc_oid, drop_reason_list))
                print("\tok")
        finally:
            pass


@pktpy_skip  # TODO bf-pktpy
@group('debug_counters')
class DebugCounterAddDropReason(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        self.create_router()

        pkt_ihl = simple_tcp_packet(eth_dst=router_mac,
                                    eth_src=self.default_mac_1,
                                    ip_ihl=1)

        pkt_vlan = simple_tcp_packet(eth_dst=self.default_mac_2,
                                     eth_src=self.default_mac_1,
                                     vlan_vid=42, dl_vlan_enable=True)
        try:
            self.create_debug_counter(
                SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS,
                [SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER])
            drop_reason_cap = [
                SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER,\
                SAI_IN_DROP_REASON_L2_ANY,\
                SAI_IN_DROP_REASON_SMAC_MULTICAST,\
                SAI_IN_DROP_REASON_TTL,\
                SAI_IN_DROP_REASON_IP_HEADER_ERROR,\
                SAI_IN_DROP_REASON_SIP_MC,
                SAI_IN_DROP_REASON_SIP_CLASS_E,
                SAI_IN_DROP_REASON_DIP_LOOPBACK,
                SAI_IN_DROP_REASON_SIP_LOOPBACK]

            if (self.isInDropReasonSupported([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC])):
                drop_reason_cap.append(SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC)

            drop_reason_list = []
            for drop_reason in drop_reason_cap:
                # drop_reason = drop_reason_cap[i]
                drop_reason_list.append(drop_reason)
                print("Setting DebugCounter drop_reason_list to: %s" %(self.mapDropReasonToString(drop_reason_list)))
                self.set_counter_reasons(self.dc_oid, drop_reason_list)

                attr = sai_thrift_get_debug_counter_attribute(
                    self.client,
                    self.dc_oid,
                    SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, 20)
                self.assertTrue(len(drop_reason_list) == attr.value.s32list.count)
                self.assertTrue(self.verifyDebugCounterDropList(
                    self.dc_oid, drop_reason_list))
                print("Verify updated DebugCounter drop_reason_list with traffic")
                self.assertTrue(self.verifyDebugCounterDropPackets(self.dc_oid, drop_reason_list))
                print("\tok")
        finally:
            pass


@group('debug_counters')
class GetDebugCounterEnumValuesCapabilities(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        # supported SAI IN drop reason list
        in_caps_list = [
            SAI_IN_DROP_REASON_INGRESS_VLAN_FILTER,
            SAI_IN_DROP_REASON_L2_ANY,
            SAI_IN_DROP_REASON_L3_ANY,
            SAI_IN_DROP_REASON_SMAC_MULTICAST,
            SAI_IN_DROP_REASON_TTL,
            SAI_IN_DROP_REASON_IP_HEADER_ERROR,
            SAI_IN_DROP_REASON_SIP_MC,
            SAI_IN_DROP_REASON_IRIF_DISABLED,
            SAI_IN_DROP_REASON_ACL_ANY,
            SAI_IN_DROP_REASON_DIP_LOOPBACK,
            SAI_IN_DROP_REASON_SIP_LOOPBACK,
            SAI_IN_DROP_REASON_SIP_CLASS_E,
            SAI_IN_DROP_REASON_DIP_LINK_LOCAL,
            SAI_IN_DROP_REASON_SIP_LINK_LOCAL,
            SAI_IN_DROP_REASON_SIP_UNSPECIFIED,
            SAI_IN_DROP_REASON_UC_DIP_MC_DMAC]

        # make sure SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC is supported by current profile
        if (self.isInDropReasonSupported([SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC])):
            in_caps_list.append(SAI_IN_DROP_REASON_SMAC_EQUALS_DMAC)

        # supported SAI OUT drop reason list
        out_caps_list = []

        in_dc_caps_list = self.client.sai_thrift_query_attribute_enum_values_capability(
            SAI_OBJECT_TYPE_DEBUG_COUNTER,
            SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST,
            20)
        print("Supported drop_reasons: %s len=%d" %(self.mapDropReasonToString(in_dc_caps_list), len(in_dc_caps_list)))
        for drop_reason in (in_caps_list):
            found = False
            for in_drop_reason in (in_dc_caps_list):
                if (drop_reason == in_drop_reason):
                    found = True
            if (found == False):
                print("Unable to find drop reason: %s (%d)"
                      %(self.mapDropReasonToString([drop_reason]), drop_reason))
            self.assertTrue(found == True)
        self.assertTrue(len(in_dc_caps_list) == len(in_caps_list))

        out_dc_cap_list = self.client.sai_thrift_query_attribute_enum_values_capability(
            SAI_OBJECT_TYPE_DEBUG_COUNTER,
            SAI_DEBUG_COUNTER_ATTR_OUT_DROP_REASON_LIST, 20)
        # currently out drop reasons not supported
        self.assertTrue(len(out_dc_cap_list) == 0)

        in_dc_caps_list = self.client.sai_thrift_query_attribute_enum_values_capability(
            SAI_OBJECT_TYPE_DEBUG_COUNTER,
            SAI_DEBUG_COUNTER_ATTR_IN_DROP_REASON_LIST, 2)
        self.assertTrue(len(in_dc_caps_list) == 0)

group('debug_counters')
class GetDebugCounterAvailability(BaseDebugCounterTest):
    def runTest(self):
        switch_init(self.client)

        # supported debug counter types
        dc_types = [SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS,\
                    SAI_DEBUG_COUNTER_TYPE_SWITCH_IN_DROP_REASONS,\
                    SAI_DEBUG_COUNTER_TYPE_PORT_OUT_DROP_REASONS,\
                    SAI_DEBUG_COUNTER_TYPE_SWITCH_OUT_DROP_REASONS]
        #verify supported debug counter types
        for dc_type in (dc_types):
            # Get Debug Counter availability
            dc_avail = self.client.sai_thrift_sai_get_debug_counter_type_availability(dc_type)
            print("verify DC Type = %s, dc_availability=%d" %(dc_type, dc_avail))
            self.assertTrue(dc_avail == 0x400)
            print("\tok")

        # verify unknown(random) debug counter types, should return value of 0
        for dc_type in [123, 777, 0x400]:
            dc_avail = self.client.sai_thrift_sai_get_debug_counter_type_availability(dc_type)
            print("verify unsupported DC Type = %s, dc_availability=%d" %(dc_type, dc_avail))
            self.assertTrue(dc_avail == 0)
            print("\tok")
