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

pkt_len = int(test_param_get('pkt_size'))
acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
acl_group_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_PORT)

@group('acl')
class PFCWDTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        self.qid = 4
        self.qid2 = 5
        self.dscp_val = 4
        self.dscp_val2 = 5
        self.tc_val = 32
        self.tc_val2 = 34

        # dscp to tc qos_map
        qos_map = self.add_qos_map(self.device, dscp=self.dscp_val, tc=self.tc_val)
        qos_map2 = self.add_qos_map(self.device, dscp=self.dscp_val2, tc=self.tc_val2)
        dscp_tc_maps = []
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)

        # tc to icos qos_map
        tc_qos_map1 = self.add_qos_map(self.device, tc=self.tc_val, icos=4)
        tc_qos_map2 = self.add_qos_map(self.device, tc=self.tc_val2, icos=5)
        tc_icos_maps = []
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map1))
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map2))
        self.tc_icos_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=tc_icos_maps)

        # tc to qid qos_map
        qos_map3 = self.add_qos_map(self.device, tc=self.tc_val, qid=self.qid)
        qos_map4 = self.add_qos_map(self.device, tc=self.tc_val2, qid=self.qid2)
        tc_queue_maps = []
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map3))
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map4))
        self.tc_qid_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)

        # pfc priority to qid qos_map
        qos_map5 = self.add_qos_map(self.device, pfc_priority=3, qid=3)
        qos_map6 = self.add_qos_map(self.device, pfc_priority=4, qid=4)
        qos_map7 = self.add_qos_map(self.device, pfc_priority=5, qid=5)
        pfc_priority_queue_maps = []
        pfc_priority_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
        pfc_priority_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map6))
        pfc_priority_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map7))
        self.pfc_priority_queue_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_PFC_PRIORITY_TO_QUEUE, qos_map_list=pfc_priority_queue_maps)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,
                           self.pfc_priority_queue_map_egress)

        ingress_pool_size = 1024

        # create ingress buffer pool profile.
        ingres_buffer_pool_handle  = self.add_buffer_pool(self.device,
                        direction=SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS,
                        threshold_mode=SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE_DYNAMIC,
                        pool_size=ingress_pool_size)

        # create device level ingress buffer profile.
        ingress_buffer_size = 2048
        threshold = 256
        device_ingress_buffer_profile = self.add_buffer_profile(self.device,
                           threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
                           threshold=threshold,
                           buffer_size=ingress_buffer_size,
                           buffer_pool_handle=ingres_buffer_pool_handle)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, device_ingress_buffer_profile)
        print("Ingress buffer profile handle (Device default) 0x%lx"%(device_ingress_buffer_profile))

        # create PPG.
        ppg_index1 = 2
        ppg_index2 = 3
        ppg_attr_list = {SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE:
                            ["Buffer profile handle", 0],
                            SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX:
                            ["Priority group index", ppg_index1],
                            SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE:
                            ["Ingress port handle", self.port0],
                            SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW:
                            ["created_in_hw", False],
                            SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE:
                            ["buffer_profile_in_use", 0]
                            }
        self.ingress_ppg_handle = self.add_port_priority_group(self.device,
                          ppg_index =  ppg_index1,
                          port_handle = self.port0)
        self.ingress_ppg_handle2 = self.add_port_priority_group(self.device,
                          ppg_index =  ppg_index2,
                          port_handle = self.port0)
        print("Created Ingress ppg, handle 0x%lx"%(self.ingress_ppg_handle))
        self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg_handle, ppg_attr_list),
             "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg_handle))

        # icos to ppg qos_map
        qos_map8 = self.add_qos_map(self.device, icos=4, ppg=ppg_index1)
        qos_map9 = self.add_qos_map(self.device, icos=5, ppg=ppg_index2)
        icos_ppg_maps = []
        icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map8))
        icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map9))
        self.icos_to_ppg_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)
        print("Attaching ingress_icos_ppg_map  0x%lx to port 0x%lx"%(self.icos_to_ppg_map_ingress, self.port0))
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        # First Ingress ACL table and entries
        self.ipfc_wd_tbl1 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_PFCWD,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        self.itbl1_entry1 = self.add_acl_entry(self.device,
            pfc_wd_qid=3,
            table_handle=self.ipfc_wd_tbl1)

        self.itbl1_entry2 = self.add_acl_entry(self.device,
            pfc_wd_qid=4,
            table_handle=self.ipfc_wd_tbl1)

        # Second Ingress ACL table and entries
        self.ipfc_wd_tbl2 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_PFCWD,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        self.itbl2_entry1 = self.add_acl_entry(self.device,
            pfc_wd_qid=1,
            table_handle=self.ipfc_wd_tbl2)

        # Egress ACL table and entries
        self.epfc_wd_tbl = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_PFCWD,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        self.etbl_entry1 = self.add_acl_entry(self.device,
            pfc_wd_qid=3,
            table_handle=self.epfc_wd_tbl)

        # ACL groups
        self.iacl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        self.eacl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])

    def runTest(self):
        try:
            self.TableBindUnbindTest()
            self.TableMultiplePortsBindUnbindTest()
            self.GroupBindUnbindTest()
            self.GroupMemberAddRemoveTest()
            self.EntryCreateRemoveTest()
            self.TrafficDropTest()
            self.TrafficDropTest(True)
            self.TrafficDropPortInDiscardsCountersTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, 0)
        self.cleanup()

    def check_pfc_wd_exists(self, port_list, entry_list, direction, should_exist=True):
        pfc_wd_list = []
        if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD) != 0):
            for port in port_list:
                for entry in entry_list:
                    attrs = list()
                    value = switcht_value_t(type=switcht_value_type.OBJECT_ID, OBJECT_ID=self.device)
                    attr = switcht_attribute_t(id=SWITCH_PFC_WD_ATTR_DEVICE, value=value)
                    attrs.append(attr)
                    value = switcht_value_t(type=switcht_value_type.ENUM, ENUM=direction)
                    attr = switcht_attribute_t(id=SWITCH_PFC_WD_ATTR_DIRECTION, value=value)
                    attrs.append(attr)
                    qid = self.attribute_get(entry, SWITCH_ACL_ENTRY_ATTR_PFC_WD_QID)
                    value = switcht_value_t(type=switcht_value_type.UINT8, UINT8=qid)
                    attr = switcht_attribute_t(id=SWITCH_PFC_WD_ATTR_QID, value=value)
                    attrs.append(attr)
                    value = switcht_value_t(type=switcht_value_type.OBJECT_ID, OBJECT_ID=port)
                    attr = switcht_attribute_t(id=SWITCH_PFC_WD_ATTR_PORT_HANDLE, value=value)
                    attrs.append(attr)
                    ret = self.client.object_get(SWITCH_OBJECT_TYPE_PFC_WD, attrs)
                    if should_exist:
                        assert ret.object_id != 0, 'pfc_wd object not found for port {} and entry {}'.format(port, entry)
                        pfc_wd_list.append(ret.object_id)
                    else:
                        assert ret.object_id == 0, 'pfc_wd object {} should not exist for port {} and entry {}'.format(ret.object_id, port, entry)
        return pfc_wd_list

    def CheckCounter(self, oid, cntr_id, count, obj_descr="", positive_check=True):
        if self.test_params['target'] == 'hw':
            time.sleep(2)
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

    def TableBindUnbindTest(self):
        '''
        This test tries to verify PFC WD objects are created/removed
        when PFCWD ACL table is bound/unbound to/from a port
        '''
        print("TableBindUnbindTest()")
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD)==0):
                print("PFC feature not enabled, skipping")
                return

            # Bind Ingress PFC WD table
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.ipfc_wd_tbl1)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            # Bind Egress PFC WD table
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE,
                self.epfc_wd_tbl)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0],
                [self.etbl_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_EGRESS)
            # Bind a different Ingress PFC WD table
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.ipfc_wd_tbl2)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl2_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)
            # Unbind Ingress PFC WD table
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.check_pfc_wd_exists([self.port0], [self.itbl2_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)
            self.check_pfc_wd_exists([self.port0], [self.etbl_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_EGRESS)
            # Unbind Engress PFC WD table
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.check_pfc_wd_exists([self.port0], [self.etbl_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_EGRESS, False)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)

    def TableMultiplePortsBindUnbindTest(self):
        '''
        This test tries to verify PFC WD objects are created/removed
        when PFCWD ACL table is bound/unbound to/from several ports
        '''
        print("TableMultiplePortsBindUnbindTest()")
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD)==0):
                print("PFC feature not enabled, skipping")
                return

            # Bind Ingress PFC WD table
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.ipfc_wd_tbl1)
            self.attribute_set(self.port1,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.ipfc_wd_tbl1)
            self.check_pfc_wd_exists([self.port0, self.port1],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            # Unbind Ingress PFC WD table for port0
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)
            self.check_pfc_wd_exists([self.port1],
               [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            # Unbind Ingress PFC WD table for port1
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.check_pfc_wd_exists([self.port1],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_EGRESS, False)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)

    def GroupBindUnbindTest(self):
        '''
        This test tries to verify PFC WD objects are created/removed
        when PFCWD ACL group is bound/unbound to/from a port
        '''
        print("GroupBindUnbindTest()")

        if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD)==0):
            print("PFC feature not enabled, skipping")
            return

        # Create ACL tables and entries
        acl_table1 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry11 = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:22',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table1)

        # Create ACL group members
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table1,
            acl_group_handle=self.iacl_group)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=self.ipfc_wd_tbl1,
            acl_group_handle=self.iacl_group)
        acl_grp_member3 = self.add_acl_group_member(self.device,
            acl_table_handle=self.ipfc_wd_tbl2,
            acl_group_handle=self.iacl_group)
        acl_grp_member4 = self.add_acl_group_member(self.device,
            acl_table_handle=self.epfc_wd_tbl,
            acl_group_handle=self.eacl_group)

        try:
            # Bind Ingress ACL Group
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.iacl_group)
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE,
                self.eacl_group)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2, self.itbl2_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0], [self.etbl_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_EGRESS)
            # No PFC WD object should be created for MAC ACL table
            self.check_pfc_wd_exists([self.port0], [acl_entry11],
             SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)
            # Unbind Ingress ACL Group
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2, self.itbl2_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)
            self.check_pfc_wd_exists([self.port0],
                [self.etbl_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_EGRESS)
            # Unbind Egress ACL Group
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.check_pfc_wd_exists([self.port0], [self.etbl_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_EGRESS, False)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.clean_to_object(acl_table1)

    def GroupMemberAddRemoveTest(self):
        '''
        This test tries to verify PFC WD objects are created/removed
        when PFCWD ACL group is bound/unbound to/from a port
        '''
        print("GroupMemberAddRemoveTest()")

        if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD)==0):
            print("PFC feature not enabled, skipping")
            return

        # Create ACL tables and entries
        acl_table1 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry11 = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:22',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table1)

        try:
            # Bind ingress PFC WD table to port 1
            self.attribute_set(self.port1,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.ipfc_wd_tbl1)
            self.check_pfc_wd_exists([self.port1],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            # Bind Ingress ACL Group to port 0
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.iacl_group)

            # Create ACL group members
            acl_grp_member1 = self.add_acl_group_member(self.device,
                acl_table_handle=self.ipfc_wd_tbl1,
                acl_group_handle=self.iacl_group)
            self.check_pfc_wd_exists([self.port0, self.port1],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            acl_grp_member2 = self.add_acl_group_member(self.device,
                acl_table_handle=self.ipfc_wd_tbl2,
                acl_group_handle=self.iacl_group)
            self.check_pfc_wd_exists([self.port1],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2, self.itbl2_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            acl_grp_member3 = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table1,
                acl_group_handle=self.iacl_group)
            # No PFC WD object should be created for MAC ACL table
            self.check_pfc_wd_exists([self.port0], [acl_entry11],
                 SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)

            self.cleanlast()
            # PFC WD objects shouldn't be affected after non PFC WD group member removal
            self.check_pfc_wd_exists([self.port1],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2, self.itbl2_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            self.cleanlast()
            self.check_pfc_wd_exists([self.port1],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0], [self.itbl2_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)

            self.cleanlast()
            self.check_pfc_wd_exists([self.port1],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.clean_to_object(acl_table1)

    def EntryCreateRemoveTest(self):
        '''
        This test tries to verify PFC WD objects are created/removed
        when PFCWD ACL entry is created/removed
        '''
        print("EntryCreateRemoveTest()")

        if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD)==0):
            print("PFC feature not enabled, skipping")
            return

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=self.ipfc_wd_tbl1,
            acl_group_handle=self.iacl_group)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table,
            acl_group_handle=self.iacl_group)

        try:
            # Create new PFC WD ACL entry
            port_list = []
            port_list.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.port0))
            port_list.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.port1))

            itbl2_entry2 = self.add_acl_entry(self.device,
                pfc_wd_qid=5,
                in_ports=port_list,
                table_handle=self.ipfc_wd_tbl2)
            self.check_pfc_wd_exists([self.port0, self.port1],
                [itbl2_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            # Set other IN_PORT_LIST and check PFC WD object should be created
            port_list1 = []
            port_list1.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.port2))
            port_list1.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.port3))

            self.attribute_set(itbl2_entry2,
                SWITCH_ACL_ENTRY_ATTR_IN_PORTS, port_list1)
            self.check_pfc_wd_exists([self.port0, self.port1], [itbl2_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)
            self.check_pfc_wd_exists([self.port2, self.port3],
                [itbl2_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            tmp_entry = itbl2_entry2
            self.cleanlast()
            self.check_pfc_wd_exists([self.port0, self.port1], [tmp_entry],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)

            # Bind ACL group instead of PFC WD table
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.iacl_group)
            self.check_pfc_wd_exists([self.port0], [self.itbl2_entry1],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)
            self.check_pfc_wd_exists([self.port0],
               [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            # Add entry to a PFC WD table which is ACL group member
            itbl1_entry3 = self.add_acl_entry(self.device,
                in_ports=port_list,
                pfc_wd_qid=5,
                table_handle=self.ipfc_wd_tbl1)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2, itbl1_entry3],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            # Set other QID and check PFC WD object should be created
            self.attribute_set(itbl1_entry3,
                SWITCH_ACL_ENTRY_ATTR_PFC_WD_QID, 6)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2, itbl1_entry3],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            tmp_entry = itbl1_entry3
            self.cleanlast()
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0], [tmp_entry],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)

            # Add entry to a non PFC WD table which is ACL group member
            acl_entry = self.add_acl_entry(self.device,
                src_mac='00:22:22:22:22:22',
                src_mac_mask='ff:ff:ff:ff:ff:ff',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2], SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)
            self.check_pfc_wd_exists([self.port0], [acl_entry],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS, False)

            self.cleanlast()
            self.check_pfc_wd_exists([self.port0],
                [self.itbl1_entry1, self.itbl1_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.clean_to_object(acl_table)

    def TrafficDropTest(self, use_qos_acl=False):
        '''
        This test tries to verify that traffic is dropped by PFC WD objects and
        counters are working correctly
        '''
        # Use QoS Maps
        if (not use_qos_acl and (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) == 0)):
            return
        # Use QoS ACL
        if (use_qos_acl and (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0)):
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD)==0):
            print("PFC feature not enabled, skipping")
            return


        print("TrafficDropTest()")
        # Add entry to a PFC WD table which is ACL group member
        etbl_entry2 = self.add_acl_entry(self.device,
            pfc_wd_qid=self.qid,
            table_handle=self.epfc_wd_tbl)

        itbl2_entry2 = self.add_acl_entry(self.device,
            pfc_wd_qid=self.qid,
            table_handle=self.ipfc_wd_tbl2)
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=self.ipfc_wd_tbl2,
            acl_group_handle=self.iacl_group)

        qos_acl_table = 0

        if use_qos_acl == False:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
        else:
            qos_acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            qos_acl_entry1 = self.add_acl_entry(self.device,
                dst_ip='10.0.0.1',
                dst_ip_mask='255.255.255.255',
                action_set_tc=self.tc_val,
                table_handle=qos_acl_table)
            qos_acl_entry2 = self.add_acl_entry(self.device,
                dst_ip='20.0.0.1',
                dst_ip_mask='255.255.255.255',
                action_set_tc=self.tc_val,
                table_handle=qos_acl_table)
            acl_grp_member2 = self.add_acl_group_member(self.device,
                acl_table_handle=qos_acl_table,
                acl_group_handle=self.iacl_group)

        pkt_count = 3

        dscp = self.dscp_val << 2
        # Simulate ECN bit is set
        tos = dscp | 1
        pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            ip_tos=tos)

        exp_pkt = pkt.copy()

        pkt2 = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64,
            ip_tos=tos)

        exp_pkt2 = pkt2.copy()
        try:
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                qos_acl_table)
            self.attribute_set(self.port1,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                qos_acl_table)

            queue_handle = 0
            queue_list = self.attribute_get(self.port0, SWITCH_PORT_ATTR_QUEUE_HANDLES)
            for queue in queue_list:
                queue_id = self.attribute_get(queue.oid, SWITCH_QUEUE_ATTR_QUEUE_ID)
                if self.qid == queue_id:
                    queue_handle = queue.oid
                    break
            self.assertTrue(queue_handle != 0,
                'could not get queue handle port {} and qid {}'.format(self.port0, 0))

            self.client.object_counters_clear_all(queue_handle)
            self.client.object_counters_clear_all(self.ingress_ppg_handle)

            print("Sending L2 packet from {} -> {}".format(self.devports[1], self.devports[0]))
            send_packet(self, self.devports[1], pkt)
            verify_packets(self, exp_pkt, [self.devports[0]])

            print("Sending L2 packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt2)
            verify_packets(self, exp_pkt2, [self.devports[1]])

            self.CheckCounter(queue_handle, SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS, 1, "queue fwd")
            self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1, "PPG fwd")

            # Bind Egress and Ingress PFC WD table
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE,
                self.epfc_wd_tbl)
            pfc_wd = self.check_pfc_wd_exists([self.port0],
                [etbl_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_EGRESS)
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.iacl_group)
            pfc_wd_ingress = self.check_pfc_wd_exists([self.port0],
                [itbl2_entry2],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD) != 0):
                print("Sending {} L2 packets from {} -> {} : should be dropped" \
                    .format(pkt_count, self.devports[1], self.devports[0]))
                for i in range (0, pkt_count):
                    send_packet(self, self.devports[1], pkt)
                    send_packet(self, self.devports[0], pkt2)
                verify_no_other_packets(self, timeout=2)

                self.CheckCounter(pfc_wd[0], SWITCH_PFC_WD_COUNTER_ID_PACKETS, pkt_count, "egress PFC WD drop")
                self.CheckCounter(pfc_wd[0], SWITCH_PFC_WD_COUNTER_ID_BYTES, 0, "egress PFC WD drop", False)
                if self.arch == 'tofino':
                    # For non tofino queue and ppg drop counters clear is not working now
                    # so skip the validation
                    # Egress PFC WD dropped packest are included into a queue dropped packets
                    self.CheckCounter(queue_handle, SWITCH_QUEUE_COUNTER_ID_DROPPED_PACKETS, pkt_count, "queue drop")
                    self.CheckCounter(queue_handle, SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS, 1, "queue fwd")

                self.CheckCounter(pfc_wd_ingress[0], SWITCH_PFC_WD_COUNTER_ID_PACKETS, pkt_count, "ingress PFC WD drop")
                self.CheckCounter(pfc_wd_ingress[0], SWITCH_PFC_WD_COUNTER_ID_BYTES, 0, "ingress PFC WD drop", False)
                if self.arch == 'tofino':
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1, "PPG fwd")
                    # Ingress PFC WD dropped packest are included into a PPG dropped packets
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_DROPPED_PACKETS, pkt_count, "PPG drop")

                # Clear queue packets counter and validate it's zeroed and the other remain untouched
                cntr_ids = [SWITCH_QUEUE_COUNTER_ID_DROPPED_PACKETS]
                self.client.object_counters_clear(queue_handle, cntr_ids)
                self.CheckCounter(pfc_wd[0], SWITCH_PFC_WD_COUNTER_ID_PACKETS, 0, "egress PFC WD drop")
                self.CheckCounter(pfc_wd[0], SWITCH_PFC_WD_COUNTER_ID_BYTES, 0, "egress PFC WD drop", False)
                if self.arch == 'tofino':
                    # Egress PFC WD dropped packest are included into a queue dropped packets
                    self.CheckCounter(queue_handle, SWITCH_QUEUE_COUNTER_ID_DROPPED_PACKETS, 0, "queue drop")
                    # Queue stat packets when PFC WD exists looks as (queue fwd packets number - PFC WD dropped
                    # packets). So when PFC WD is cleared the queue stats gets increased with the previously
                    # dropped by PFC WD packets.
                    self.CheckCounter(queue_handle, SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS, 1 + pkt_count, "queue fwd")

                # Clear PPG packets counter and validate it's zeroed and the other remain untouched
                cntr_ids = [SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_DROPPED_PACKETS]
                self.client.object_counters_clear(self.ingress_ppg_handle, cntr_ids)
                self.CheckCounter(pfc_wd_ingress[0], SWITCH_PFC_WD_COUNTER_ID_PACKETS, 0, "ingress PFC WD drop")
                self.CheckCounter(pfc_wd_ingress[0], SWITCH_PFC_WD_COUNTER_ID_BYTES, 0, "ingress PFC WD drop", False)
                if self.arch == 'tofino':
                    # Ingress PFC WD dropped packest are included into a PPG dropped packets
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_DROPPED_PACKETS, 0, "PPG drop")
                    # PPG stat packets when PFC WD exists looks as (PPG fwd packets number - PFC WD dropped
                    # packets). So when PFC WD is cleared the PPG stats gets increased with the previously
                    # dropped by PFC WD packets.
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1 + pkt_count, "PPG fwd")

                # Clear all counetrs and validate all are zeroed
                self.client.object_counters_clear_all(queue_handle)
                self.client.object_counters_clear_all(self.ingress_ppg_handle)

                self.CheckCounter(pfc_wd[0], SWITCH_PFC_WD_COUNTER_ID_PACKETS, 0, "egress PFC WD drop")
                self.CheckCounter(pfc_wd[0], SWITCH_PFC_WD_COUNTER_ID_BYTES, 0, "egress PFC WD drop")
                if self.arch == 'tofino':
                    self.CheckCounter(queue_handle, SWITCH_QUEUE_COUNTER_ID_DROPPED_PACKETS, 0, "queue drop")

                self.CheckCounter(pfc_wd_ingress[0], SWITCH_PFC_WD_COUNTER_ID_PACKETS, 0, "ingress PFC WD drop")
                self.CheckCounter(pfc_wd_ingress[0], SWITCH_PFC_WD_COUNTER_ID_BYTES, 0, "ingress PFC WD drop")
                if self.arch == 'tofino':
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_DROPPED_PACKETS, 0, "PPG drop")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.clean_to_object(etbl_entry2)

    def TrafficDropPortInDiscardsCountersTest(self):
        '''
        This test tries to verify that traffic is dropped by PFC WD ingress objects and
        PPG drop counter is added to port IN_DISCARDS
        '''
        # Use QoS Maps
        if self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) == 0:
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD)==0):
            print("PFC feature not enabled, skipping")
            return

        print("TrafficDropPortInDiscardsCountersTest()")
        # Add entry to a PFC WD table which is ACL group member
        itbl2_entry2 = self.add_acl_entry(self.device,
            pfc_wd_qid=self.qid,
            table_handle=self.ipfc_wd_tbl2)
        itbl2_entry3 = self.add_acl_entry(self.device,
            pfc_wd_qid=self.qid2,
            table_handle=self.ipfc_wd_tbl2)
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=self.ipfc_wd_tbl2,
            acl_group_handle=self.iacl_group)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)

        pkt_count = 3

        dscp = self.dscp_val << 2
        # Simulate ECN bit is set
        tos = dscp | 1
        pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            ip_tos=tos)

        exp_pkt = pkt.copy()

        dscp2 = self.dscp_val2 << 2
        # Simulate ECN bit is set
        tos2 = dscp2 | 1
        pkt2 = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            ip_tos=tos2)

        exp_pkt2 = pkt2.copy()

        try:
            self.client.object_counters_clear_all(self.ingress_ppg_handle)
            self.client.object_counters_clear_all(self.ingress_ppg_handle2)
            self.client.object_counters_clear_all(self.port0)

            print("Sending L2 packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])
            send_packet(self, self.devports[0], pkt2)
            verify_packets(self, exp_pkt2, [self.devports[1]])

            self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1, "PPG fwd")
            self.CheckCounter(self.ingress_ppg_handle2, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1, "PPG fwd")

            # Bind Ingress PFC WD table
            self.attribute_set(self.port0,
                SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE,
                self.iacl_group)
            pfc_wd_ingress = self.check_pfc_wd_exists([self.port0],
                [itbl2_entry2, itbl2_entry3],
                SWITCH_PFC_WD_ATTR_DIRECTION_INGRESS)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PFC_WD) != 0):
                print("Sending {} L2 packets from {} -> {} : should be dropped" \
                    .format(pkt_count, self.devports[0], self.devports[1]))
                for i in range (0, pkt_count):
                    send_packet(self, self.devports[0], pkt)
                    send_packet(self, self.devports[0], pkt2)
                verify_no_other_packets(self, timeout=2)

                self.CheckCounter(pfc_wd_ingress[0], SWITCH_PFC_WD_COUNTER_ID_PACKETS, pkt_count, "ingress PFC WD drop")
                self.CheckCounter(pfc_wd_ingress[1], SWITCH_PFC_WD_COUNTER_ID_PACKETS, pkt_count, "ingress PFC WD drop")
                if self.arch == 'tofino':
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1, "PPG fwd")
                    self.CheckCounter(self.ingress_ppg_handle2, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1, "PPG fwd")
                    # Ingress PFC WD dropped packest are included into a PPG dropped packets
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_DROPPED_PACKETS, pkt_count, "PPG drop")
                    self.CheckCounter(self.ingress_ppg_handle2, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_DROPPED_PACKETS, pkt_count, "PPG drop")
                    # PPG dropped packest are included into a port IN_DISCARDS packets
                    self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS, pkt_count + pkt_count, "Port IN_DISCARDS")

                # Clear PPG packets counter and validate it's zeroed and the other remain untouched
                cntr_ids = [SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS]
                self.client.object_counters_clear(self.port0, cntr_ids)
                self.CheckCounter(pfc_wd_ingress[0], SWITCH_PFC_WD_COUNTER_ID_PACKETS, 0, "ingress PFC WD drop")
                self.CheckCounter(pfc_wd_ingress[1], SWITCH_PFC_WD_COUNTER_ID_PACKETS, 0, "ingress PFC WD drop")
                if self.arch == 'tofino':
                    # PPG dropped packest are included into a port IN_DISCARDS packets
                    self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS, 0, "Port IN DISCARDS")
                    # Ingress PFC WD dropped packest are included into a PPG dropped packets
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_DROPPED_PACKETS, 0, "PPG drop")
                    self.CheckCounter(self.ingress_ppg_handle2, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_DROPPED_PACKETS, 0, "PPG drop")
                    # PPG stat packets when PFC WD exists looks as (PPG fwd packets number - PFC WD dropped
                    # packets). So when PFC WD is cleared the PPG stats gets increased with the previously
                    # dropped by PFC WD packets.
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1 + pkt_count, "PPG fwd")
                    self.CheckCounter(self.ingress_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, 1 + pkt_count, "PPG fwd")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.clean_to_object(itbl2_entry2)
