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
from ptf.packet import *
from ptf.thriftutils import *

import os
import ptf.mask as mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *

from ipaddress import *

acl_table_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_SWITCH)

#@group('etrap')
class EtrapConfigTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        # Etrap ACL tables and entries
        self.etrap_v4_tbl = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_ETRAP,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        self.etrap_v6_tbl = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6_ETRAP,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

    def runTest(self):
        try:
            self.EtrapAclEntryCreateRemoveTest()
            self.EtrapAclEntryCreateRemoveTest(host_ip=True)
            self.EtrapAclEntryCreateRemoveTest(v6=True)
            self.EtrapAclEntryCreateRemoveTest(v6=True, host_ip=True)
            self.EtrapAclEntryAttrUpdateTest()
            self.EtrapAclEntryCreateNegativeTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def checkEtrapAttributes(self, etrap_entries, meter, tc):
        for etrap_entry in etrap_entries:
            etrap_tc = self.attribute_get(etrap_entry, SWITCH_ETRAP_ACL_ENTRY_ATTR_TC)
            self.assertTrue((etrap_tc == tc), "TC {} in etrap_entry not equal to {}".format(etrap_tc, tc))

            etrap_meter = self.attribute_get(etrap_entry, SWITCH_ETRAP_ACL_ENTRY_ATTR_METER_HANDLE)
            self.assertTrue((etrap_meter != 0), "Meter handle not set in etrap_entry")

            cbs = self.attribute_get(meter, SWITCH_METER_ATTR_CBS)
            etrap_cbs = self.attribute_get(etrap_meter, SWITCH_ETRAP_METER_ATTR_CBS)
            self.assertTrue((etrap_cbs == cbs), "CBS {} in etrap_meter not equal to {}".format(etrap_cbs, cbs))

            pbs = self.attribute_get(meter, SWITCH_METER_ATTR_PBS)
            etrap_pbs = self.attribute_get(etrap_meter, SWITCH_ETRAP_METER_ATTR_PBS)
            self.assertTrue((etrap_pbs == pbs), "PBS {} in etrap_meter not equal to {}".format(etrap_pbs, pbs))

            cir = self.attribute_get(meter, SWITCH_METER_ATTR_CIR)
            etrap_cir = self.attribute_get(etrap_meter, SWITCH_ETRAP_METER_ATTR_CIR)
            self.assertTrue((etrap_cir == cir), "CIR {} in etrap_meter not equal to {}".format(etrap_cir, cir))

            pir = self.attribute_get(meter, SWITCH_METER_ATTR_PIR)
            etrap_pir = self.attribute_get(etrap_meter, SWITCH_ETRAP_METER_ATTR_PIR)
            self.assertTrue((etrap_pir == pir), "PIR {} in etrap_meter not equal to {}".format(etrap_pir, pir))

    def checkEtrapsExist(self, src_prefix=None, dst_prefix=None, should_exist=True):
        etraps_list = []
        if (self.client.is_feature_enable(SWITCH_FEATURE_ETRAP) != 0):
            src_addrs = []
            dst_addrs = []
            mask = '255.255.255.255'
            zero_ip = '0.0.0.0'
            etrap_type = SWITCH_ETRAP_ACL_ENTRY_ATTR_TYPE_IPV4

            if (src_prefix and src_prefix.version == 6) or (dst_prefix and dst_prefix.version == 6):
                mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
                zero_ip = '::'
                etrap_type = SWITCH_ETRAP_ACL_ENTRY_ATTR_TYPE_IPV6

            if src_prefix:
                for addr in src_prefix:
                    src_addrs.append(addr)
            else:
                src_addrs.append(ip_address(zero_ip))

            if dst_prefix:
                for addr in dst_prefix:
                    dst_addrs.append(addr)
            else:
                dst_addrs.append(ip_address(zero_ip))

            for src_addr in src_addrs:
                for dst_addr in dst_addrs:
                    attrs = list()
                    value = switcht_value_t(type=switcht_value_type.ENUM, ENUM=etrap_type)
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_TYPE, value=value)
                    attrs.append(attr)

                    value = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=str(src_addr))
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_SRC_IP, value=value)
                    attrs.append(attr)

                    mask_val = mask
                    # We don't expand zero IPs but pass as is
                    if str(src_addr) == zero_ip:
                        mask_val = zero_ip
                    value = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=mask_val)
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_SRC_IP_MASK, value=value)
                    attrs.append(attr)


                    value = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=str(dst_addr))
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_DST_IP, value=value)
                    attrs.append(attr)

                    mask_val = mask
                    if str(dst_addr) == zero_ip:
                        mask_val = zero_ip
                    value = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=mask_val)
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_DST_IP_MASK, value=value)
                    attrs.append(attr)

                    ret = self.client.object_get(SWITCH_OBJECT_TYPE_ETRAP_ACL_ENTRY, attrs)
                    if should_exist:
                        assert ret.object_id != 0, 'Etrap ACL entry object not found for src {} and dst {} IPs'.format(src_addr, dst_addr)
                        etraps_list.append(ret.object_id)
                    else:
                        assert ret.object_id == 0, 'Etrap ACL entry object {} should not exist for src {} and dst {} IPs'.format(ret.object_id, src_addr, dst_addr)
        return etraps_list

    def EtrapAclEntryCreateRemoveTest(self, v6=False, host_ip=False):
        '''
        This test tries to verify etrap objects are created/removed
        when ACL entry is created/removed
        '''
        version_str = "V4"
        host_ips_str = ""

        if v6:
            version_str = "V6"

        if host_ip:
            host_ips_str = "Host IPs"
        print("EtrapAclEntryCreateRemoveTest() {} {}".format(version_str, host_ips_str))

        meter1 = self.add_meter(self.device,
            pir=10,cir=20,cbs=30,pbs=40,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        meter2 = self.add_meter(self.device,
            pir=50,cir=60,cbs=70,pbs=80,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        meter3 = self.add_meter(self.device,
            pir=90,cir=100,cbs=110,pbs=120,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        try:
            table_handle=self.etrap_v4_tbl
            src_prefix = ip_network(u'10.10.10.0/27')
            dst_prefix = ip_network(u'20.20.20.0/28')
            if host_ip and not v6:
                src_prefix = ip_network(u'10.10.10.1/32')
                dst_prefix = ip_network(u'20.20.20.2/32')

            if v6:
                table_handle=self.etrap_v6_tbl
                src_prefix = ip_network(u'1000::1000/125')
                dst_prefix = ip_network(u'2000::2000/124')
                if host_ip:
                    src_prefix = ip_network(u'1000::1001/128')
                    dst_prefix = ip_network(u'2000::2002/128')

            # Create etrap ACL entry with source subnet only
            acl_entry1 = self.add_acl_entry(self.device,
                src_ip=str(src_prefix.network_address),
                src_ip_mask=str(src_prefix.netmask),
                action_meter_handle=meter1,
                action_set_tc=20,
                table_handle=table_handle)

            etrap_entries = self.checkEtrapsExist(src_prefix=src_prefix)
            self.checkEtrapAttributes(etrap_entries, meter1, 20)

            # Create etrap ACL entry with dst subnet only
            acl_entry2 = self.add_acl_entry(self.device,
                dst_ip=str(dst_prefix.network_address),
                dst_ip_mask=str(dst_prefix.netmask),
                action_meter_handle=meter2,
                action_set_tc=24,
                table_handle=table_handle)
            etrap_entries = self.checkEtrapsExist(dst_prefix=dst_prefix)
            self.checkEtrapAttributes(etrap_entries, meter2, 24)
            # Verify that etrap entries for the first entry are still there
            etrap_entries = self.checkEtrapsExist(src_prefix=src_prefix)
            self.checkEtrapAttributes(etrap_entries, meter1, 20)

            acl_entry3 = self.add_acl_entry(self.device,
                src_ip=str(src_prefix.network_address),
                src_ip_mask=str(src_prefix.netmask),
                dst_ip=str(dst_prefix.network_address),
                dst_ip_mask=str(dst_prefix.netmask),
                action_meter_handle=meter3,
                action_set_tc=28,
                table_handle=table_handle)
            etrap_entries = self.checkEtrapsExist(src_prefix=src_prefix, dst_prefix=dst_prefix)
            self.checkEtrapAttributes(etrap_entries, meter3, 28)

            # Remove the acl_entry3 and validate that etrap entries for this one are removed
            # and for the other two still exist
            self.cleanlast()
            self.checkEtrapsExist(src_prefix=src_prefix, dst_prefix=dst_prefix, should_exist=False)

            # Verify that etrap entries for the first two entries are still there
            etrap_entries = self.checkEtrapsExist(src_prefix=src_prefix)
            self.checkEtrapAttributes(etrap_entries, meter1, 20)
            etrap_entries = self.checkEtrapsExist(dst_prefix=dst_prefix)
            self.checkEtrapAttributes(etrap_entries, meter2, 24)

            # Remove the acl_entry2 and validate that etrap entries for this one are removed
            # and for the first one still exist
            self.cleanlast()
            self.checkEtrapsExist(dst_prefix=dst_prefix, should_exist=False)

            # Verify that etrap entries for the first entry are still there
            etrap_entries = self.checkEtrapsExist(src_prefix=src_prefix)
            self.checkEtrapAttributes(etrap_entries, meter1, 20)

            # Remove the acl_entry1 and validate that etrap entries for this one are removed
            self.cleanlast()
            self.checkEtrapsExist(src_prefix=src_prefix, should_exist=False)

        finally:
            self.clean_to_object(meter1)

    def EtrapAclEntryAttrUpdateTest(self):
        '''
        This test tries to verify etrap objects attributes are 
        updated when the corresponding ACL entry attribute gets updated
        '''
        print("EtrapAclEntryAttrUpdateTest()")

        meter1 = self.add_meter(self.device,
            pir=10,cir=20,cbs=30,pbs=40,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        meter2 = self.add_meter(self.device,
            pir=50,cir=60,cbs=70,pbs=80,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        try:
            # Create etrap ACL entry with source subnet only
            src_prefix = ip_network(u'10.10.10.0/27')
            acl_entry1 = self.add_acl_entry(self.device,
                src_ip=str(src_prefix.network_address),
                src_ip_mask=str(src_prefix.netmask),
                action_meter_handle=meter1,
                action_set_tc=20,
                table_handle=self.etrap_v4_tbl)
            etrap_entries = self.checkEtrapsExist(src_prefix=src_prefix)
            self.checkEtrapAttributes(etrap_entries, meter1, 20)

            # Update meter handle in ACL entry and validate it's updated in all the etrap objects
            self.attribute_set(acl_entry1, SWITCH_ACL_ENTRY_ATTR_ACTION_METER_HANDLE, meter2)
            self.checkEtrapAttributes(etrap_entries, meter2, 20)

            # Update TC in ACL entry and validate it's updated in all the etrap objects
            self.attribute_set(acl_entry1, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            self.checkEtrapAttributes(etrap_entries, meter2, 24)

        finally:
            self.clean_to_object(meter1)

    def EtrapAclEntryCreateNegativeTest(self):
        '''
        This test tries to verify etrap objects are not created 
        when src/dst IPs and masks are zero
        '''
        print("EtrapAclEntryCreateNegativeTest()")

        meter1 = self.add_meter(self.device,
            pir=10,cir=20,cbs=30,pbs=40,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        try:
            prefix = ip_network(u'0.0.0.0/0')
            acl_entry1 = self.add_acl_entry(self.device,
                src_ip=str(prefix.network_address),
                src_ip_mask=str(prefix.netmask),
                dst_ip=str(prefix.network_address),
                dst_ip_mask=str(prefix.netmask),
                action_meter_handle=meter1,
                action_set_tc=20,
                table_handle=self.etrap_v4_tbl)

            self.assertTrue(self.status != 0, "The etrap creation should fail for both zero IPs")

            prefix = ip_network(u'::/0')
            acl_entry1 = self.add_acl_entry(self.device,
                src_ip=str(prefix.network_address),
                src_ip_mask=str(prefix.netmask),
                dst_ip=str(prefix.network_address),
                dst_ip_mask=str(prefix.netmask),
                action_meter_handle=meter1,
                action_set_tc=20,
                table_handle=self.etrap_v6_tbl)

            self.assertTrue(self.status != 0, "The etrap creation should fail for both zero v6 IPs")

        finally:
            self.clean_to_object(meter1)


#@group('etrap-hw')
class EtrapTrafficTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        self.tc_val = 20
        self.qid = 5

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        # tc to qid qos_map
        self.qos_map = self.add_qos_map(self.device, tc=self.tc_val, qid=self.qid)
        tc_queue_maps = []
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.qos_map))
        self.tc_qid_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)

        self.queue_handle = 0
        self.default_queue_handle = 0

        queue_list = self.attribute_get(self.port0, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        for queue in queue_list:
            queue_id = self.attribute_get(queue.oid, SWITCH_QUEUE_ATTR_QUEUE_ID)
            if self.qid == queue_id:
                self.queue_handle = queue.oid
            if 0 == queue_id:
                self.default_queue_handle = queue.oid

        self.meter1 = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            pir=80,cir=80,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        # Etrap ACL tables and entries
        self.etrap_v4_tbl = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_ETRAP,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        self.etrap_v6_tbl = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6_ETRAP,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

    def runTest(self):
        try:
            self.TrafficTest()
            self.TrafficTest(v6=True)

        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def CheckCounter(self, oid, cntr_id, count, obj_descr="", positive_check=True):
        counters = self.client.object_counters_get(oid)
        for cntr in counters:
            if cntr.counter_id == cntr_id:
                if positive_check:
                    self.assertTrue(cntr.count == count,
                        '{}: number of packets/bytes is {}, expected {}' \
                            .format(obj_descr, cntr.count, count))
                else:
                    self.assertTrue(cntr.count != count,
                        '{}: number of packets/bytes should not be equal to {}' \
                            .format(obj_descr, count))
                break

    def checkEtrapsExist(self, src_prefix=None, dst_prefix=None, should_exist=True):
        etraps_list = []
        if (self.client.is_feature_enable(SWITCH_FEATURE_ETRAP) != 0):
            src_addrs = []
            dst_addrs = []
            mask = '255.255.255.255'
            zero_ip = '0.0.0.0'
            etrap_type = SWITCH_ETRAP_ACL_ENTRY_ATTR_TYPE_IPV4

            if (src_prefix and src_prefix.version == 6) or (dst_prefix and dst_prefix.version == 6):
                mask = 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
                zero_ip = '::'
                etrap_type = SWITCH_ETRAP_ACL_ENTRY_ATTR_TYPE_IPV6

            if src_prefix:
                for addr in src_prefix:
                    src_addrs.append(addr)
            else:
                src_addrs.append(ip_address(zero_ip))

            if dst_prefix:
                for addr in dst_prefix:
                    dst_addrs.append(addr)
            else:
                dst_addrs.append(ip_address(zero_ip))

            for src_addr in src_addrs:
                for dst_addr in dst_addrs:
                    attrs = list()
                    value = switcht_value_t(type=switcht_value_type.ENUM, ENUM=etrap_type)
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_TYPE, value=value)
                    attrs.append(attr)

                    value = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=str(src_addr))
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_SRC_IP, value=value)
                    attrs.append(attr)

                    mask_val = mask
                    # We don't expand zero IPs but pass as is
                    if str(src_addr) == zero_ip:
                        mask_val = zero_ip
                    value = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=mask_val)
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_SRC_IP_MASK, value=value)
                    attrs.append(attr)


                    value = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=str(dst_addr))
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_DST_IP, value=value)
                    attrs.append(attr)

                    mask_val = mask
                    if str(dst_addr) == zero_ip:
                        mask_val = zero_ip
                    value = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=mask_val)
                    attr = switcht_attribute_t(id=SWITCH_ETRAP_ACL_ENTRY_ATTR_DST_IP_MASK, value=value)
                    attrs.append(attr)

                    ret = self.client.object_get(SWITCH_OBJECT_TYPE_ETRAP_ACL_ENTRY, attrs)
                    if should_exist:
                        assert ret.object_id != 0, 'Etrap ACL entry object not found for src {} and dst {} IPs'.format(src_addr, dst_addr)
                        etraps_list.append(ret.object_id)
                    else:
                        assert ret.object_id == 0, 'Etrap ACL entry object {} should not exist for src {} and dst {} IPs'.format(ret.object_id, src_addr, dst_addr)
        return etraps_list

    def checkTraffic(self, src_prefix):
        if (self.client.is_feature_enable(SWITCH_FEATURE_ETRAP) != 0):
            for addr in src_prefix:
                pkt_count = 3
                self.client.object_counters_clear_all(self.queue_handle)
                self.client.object_counters_clear_all(self.default_queue_handle)

                pkt = simple_tcp_packet(
                    eth_dst='00:11:11:11:11:11',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='172.20.10.1',
                    ip_src=str(addr),
                    ip_ttl=64)

                if src_prefix.version == 6:
                    pkt = simple_tcpv6_packet(
                        eth_dst='00:11:11:11:11:11',
                        eth_src='00:22:22:22:22:22',
                        ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                        ipv6_src=str(addr),
                        ipv6_hlim=64)

                for i in range (0, pkt_count):
                    send_packet(self, self.devports[0], pkt)

                self.CheckCounter(self.queue_handle, SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS, 2, "queue fwd")
                self.CheckCounter(self.default_queue_handle, SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS, 1, "queue fwd")

    def TrafficTest(self, v6=False):
        '''
        This test tries to verify etrap objects are created/removed
        when ACL entry is created/removed
        '''
        version_str = "V4"

        if v6:
            version_str = "V6"

        print("TrafficTest() {}".format(version_str))

        try:
            table_handle=self.etrap_v4_tbl
            src_prefix = ip_network(u'10.10.10.0/27')

            if v6:
                table_handle=self.etrap_v6_tbl
                src_prefix = ip_network(u'2000::2000/124')

            # Create etrap ACL entry with source subnet only
            self.acl_entry1 = self.add_acl_entry(self.device,
                src_ip=str(src_prefix.network_address),
                src_ip_mask=str(src_prefix.netmask),
                action_meter_handle=self.meter1,
                action_set_tc=20,
                table_handle=table_handle)

            self.checkEtrapsExist(src_prefix=src_prefix)
            self.checkTraffic(src_prefix)

        finally:
            self.clean_to_object(self.acl_entry1)
