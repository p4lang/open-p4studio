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
from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os
import ptf.mask

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
sys.path.append(os.path.join(this_dir, '../api'))
from common.utils import *
from api.api_base_tests import *
from api.switch_helpers import *
import api.model_utils as u

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

###############################################################################
class QoSTest(ApiHelper):
    '''
    This test maps DSCP-->TC-->Queue in the ingress and map TC-->DSCP in the egress.
    DSCP(10,15,20,25) ---> TC(1,2,3,4) --->Queue(1,2,3,4)
                               |
                               |
                               +---------->DSCP(30,35,40,45)
    This test also configures queue(4) as strict priority queue with shaper. 
    Queue shaper functionality works only in hardware.
    '''
    def createDscpToTcQosMap(self, dscp_list, tc_list):
        print("Configuring DscpToTc maps")
        dscp_tc_maps = []
        for i in range(len(dscp_list)):
            qos_map = self.add_qos_map(self.device, dscp=dscp_list[i], tc=tc_list[i])
            dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))

        dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
              type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)
        return dscp_tc_map_ingress

    def createTcToQueueMap(self, tc_list, qid_list):
        print("Configuring TcToQueue maps")
        tc_queue_maps = []
        for i in range(len(tc_list)):
            qos_map = self.add_qos_map(self.device, tc=tc_list[i], qid=qid_list[i])
            tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        tc_queue_maps_ingress = self.add_qos_map_ingress(self.device,
              type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)
        return tc_queue_maps_ingress

    def createTcToDscpQosMap(self, tc_list, dscp_list):
        print("Configuring TcToDscp maps")
        tc_dscp_maps = []
        for i in range(len(dscp_list)):
            qos_map = self.add_qos_map(self.device, tc=tc_list[i], dscp=dscp_list[i])
            tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        tc_dscp_map_egress = self.add_qos_map_egress(self.device,
              type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)
        return tc_dscp_map_egress

    def attachQueueSchProfile(self, port_handle, queue_id, sch_profile_handle):
        sch_group_list = self.attribute_get(port_handle,
                           SWITCH_PORT_ATTR_PORT_QUEUE_SCHEDULER_GROUP_HANDLES)
        for sch_group_handle in sch_group_list:
            # print queue and port handle
            group_type = self.attribute_get(sch_group_handle.oid,
                                               SWITCH_SCHEDULER_GROUP_ATTR_TYPE)
            if group_type == SWITCH_SCHEDULER_GROUP_ATTR_TYPE_QUEUE:
                queue_handle = self.attribute_get(sch_group_handle.oid,
                                                 SWITCH_SCHEDULER_GROUP_ATTR_QUEUE_HANDLE)
                qid = self.attribute_get(queue_handle,
                                         SWITCH_QUEUE_ATTR_QUEUE_ID)
                if qid == queue_id:
                    self.attribute_set(sch_group_handle.oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                                       sch_profile_handle)
                    return

        # failed to match qid to sch group.
        self.assertTrue(False, "Failed to match qid: %d to sch group associated with port_handle: 0x%lx "%\
                        (queue_id, port_handle))

    def createStrictPriorityProfile(self, max_rate, max_burst, pps):
        shaper_type = SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS
        if pps == True:
            shaper_type = SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_PPS

        strict_scheduler_handle = self.add_scheduler(self.device, type=SWITCH_SCHEDULER_ATTR_TYPE_STRICT,
                                shaper_type=shaper_type,
                                max_rate=max_rate,
                                max_burst_size=max_burst)

        return strict_scheduler_handle

    def runTest(self):
        self.configure()
        dscp_list = [10, 15, 20, 25]
        tc_list = [1, 2, 3, 4]
        queue_list = [1,2, 3, 4]
        rewrite_dscp_list = [30, 35, 40, 45]

        # Map DSCP 25 to strict priority queue(4)
        strict_priority_queue = 4

        #Add port0 and port1 to vlan10
        self.add_vlan_member(self.device,
                             vlan_handle=self.vlan10,
                             member_handle=self.port0)
        self.add_vlan_member(self.device,
                             vlan_handle=self.vlan10,
                             member_handle=self.port1)
        # set default vlan for port 0 on ingress
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        #   MAC entry to port1
        self.mac_entry = self.add_mac_entry(self.device, mac_address='00:11:11:11:11:11', vlan_handle=self.vlan10, destination_handle=self.port1)
        #  dscp to tc QoS map
        self.dscp_tc_handle = self.createDscpToTcQosMap(dscp_list, tc_list)

        #  TC to Queue QoS map
        self.tc_queue_handle = self.createTcToQueueMap(tc_list, queue_list)

        #  TC to DSCP rewrite QoS map
        self.tc_dscp_handle = self.createTcToDscpQosMap(tc_list, rewrite_dscp_list)

        # strict priority scheduler profile
        # Strict priority+shaper(100000 PPS)
        # pps - packets
        self.shaper_max_pps = 100000
        self.shaper_max_burst = 1000
        self.strict_sch_handle = self.createStrictPriorityProfile(self.shaper_max_pps, self.shaper_max_burst, True)

        # Attach DSCP to TC and TC to Queue on self.port0
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_DSCP_TOS_QOS_GROUP, self.dscp_tc_handle)

        # Attach TC to DSCP rewrite QoS map on self.port1
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, self.tc_dscp_handle)

        #Query all queue handles of port1
        self.queue_list = self.attribute_get(self.port1,
                     SWITCH_PORT_ATTR_QUEUE_HANDLES)

        #Attach strict priority queue scheduler to priority queue(4) on port1
        self.attachQueueSchProfile(self.port1, strict_priority_queue, self.strict_sch_handle)
        try:
            self.client.object_counters_clear_all(self.queue_list[1].oid)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11', ip_tos=40)
            exp_pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11', ip_tos=120)
            send_packet(self, self.devports[0], str(pkt))
            #Verify packet egress out of port 1 after mac lookup and DSCP rewrite in egress.
            verify_packets(self, exp_pkt, [self.devports[1]])

            #Verify DSCP 10 packet goes to queeue 1, matching DSCP-->TC-->Queue table
            queue_counter = self.object_counters_get(self.queue_list[1].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 1)

            #Verify DSCP 2 packet goes to queeue 0, matching DSCP-->TC-->Queue table
            self.client.object_counters_clear_all(self.queue_list[0].oid)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11', ip_tos=8)
            send_packet(self, self.devports[0], str(pkt))
            verify_packets(self, pkt, [self.devports[1]])
            #Verify DSCP 2 packet goes to queeue 0
            queue_counter = self.object_counters_get(self.queue_list[0].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 1)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, 0)
            self.attachQueueSchProfile(self.port1, strict_priority_queue, 0)
            self.cleanup()
