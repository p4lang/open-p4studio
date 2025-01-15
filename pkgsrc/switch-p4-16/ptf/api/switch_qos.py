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
Thrift BF_SWITCH API QOS queue, buffer, PPG and Scheduler basic tests
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
from switch_helpers import *
import model_utils as u

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
from common.pktpy_utils import pktpy_skip # noqa pylint: disable=wrong-import-position

from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as client

import copy
import six

acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
acl_group_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_PORT)
###############################################################################

@group('qos')
class schedulerGroupAttribute(ApiHelper):
    '''
    This test verify the schduler and scheduler group APIs.
    '''
    def checkSchedulerAttribute(self, scheduler_id, attr_list):
        for attr_id, attr_value in six.iteritems(attr_list):
            thrift_attr_value = self.attribute_get(scheduler_id, attr_id)
            if ((attr_id == SWITCH_SCHEDULER_ATTR_TYPE) and \
               (attr_value[1] == thrift_attr_value)):
                print("Scheduler type is correct %d"%(thrift_attr_value))
            elif ((attr_id == SWITCH_SCHEDULER_ATTR_SHAPER_TYPE) and \
              (attr_value[1] == thrift_attr_value)):
                print("Scheduler shaper type is correct %d"%(thrift_attr_value))
            elif((attr_id == SWITCH_SCHEDULER_ATTR_WEIGHT) and \
                (attr_value[1] == thrift_attr_value)):
                print("Scheduler weight is correct %d"%(thrift_attr_value))
            elif((attr_id == SWITCH_SCHEDULER_ATTR_MIN_RATE) or \
                 (attr_id == SWITCH_SCHEDULER_ATTR_MIN_BURST_SIZE) or \
                 (attr_id == SWITCH_SCHEDULER_ATTR_MAX_RATE) or \
                 (attr_id == SWITCH_SCHEDULER_ATTR_MAX_BURST_SIZE) and \
                 (attr_value[1] == thrift_attr_value)):
                print("Min and Max rate parameters are correct %lu"%(thrift_attr_value))
            else:
                print("Attr %s is incorrect, expected %d and got %d"%(attr_value[0], attr_value[1], thrift_attr_value))
                self.assertTrue(False, "Check SchedulerAttribute failed for Scheduler handle: 0x%lx"%(scheduler_id))


    def checkSchedulerGroupAttribute(self, scheduler_grp_id, attr_list):
        for attr_id, attr_value in six.iteritems(attr_list):
            thrift_attr_value = self.attribute_get(scheduler_grp_id, attr_id)
            if (attr_value[1] == thrift_attr_value):
                print("{} is correct {}".format(attr_value[0], thrift_attr_value))
            else:
                print("Attr {} is incorrect, expected {}"%(attr_value[0], attr_value[1]))
                self.assertTrue(False, "Check SchedulerAttribute failed for Scheduler Group handle: 0x%lx"%(scheduler_grp_id))

    def configureStrictScheduler(self):
            # Create a Strict Scheduler
        print("Create/Get/Set StrictSchedulerShaperTest")
        # min_rate and max_rate in bits
        min_rate = 80000000
        max_rate = 4000000000
        # max_burst and min_burst in bytes
        min_burst = 1024
        max_burst = 1024

        # shaper type SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS indicates min/max rate is in bits mode.
        self.strict_scheduler_attributes = {SWITCH_SCHEDULER_ATTR_TYPE:
                              ["Scheduler type", SWITCH_SCHEDULER_ATTR_TYPE_STRICT],
                              SWITCH_SCHEDULER_ATTR_SHAPER_TYPE:
                              ["Scheduler shaper type", SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS],
                              SWITCH_SCHEDULER_ATTR_MIN_RATE:
                              ["Scheduler min rate", min_rate],
                              SWITCH_SCHEDULER_ATTR_MIN_BURST_SIZE:
                              ["Scheduler min burst size", min_burst],
                              SWITCH_SCHEDULER_ATTR_MAX_RATE:
                              ["Scheduler max rate", max_rate],
                              SWITCH_SCHEDULER_ATTR_MAX_BURST_SIZE:
                              ["Scheduler max burst", max_burst]}

        # create strict scheduler profile
        self.strict_scheduler_handle = self.add_scheduler(self.device, type=SWITCH_SCHEDULER_ATTR_TYPE_STRICT,
                               shaper_type=SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS,
                               min_rate=min_rate,
                               min_burst_size=min_burst, max_rate=max_rate,
                               max_burst_size=max_burst)

        # configure queue in first scheduler_group as STRICT scheduling
        print("Setting the Queue 0 to Strict scheduler")
        self.attribute_set(self.sch_group_list[1].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                           self.strict_scheduler_handle)
        self.scheduler_group_attrs[self.sch_group_list[1].oid][SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] =\
        ["Scheduler Handle", self.strict_scheduler_handle]

        print("Setting the Queue 4 to Strict scheduler")
        self.attribute_set(self.sch_group_list[5].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                           self.strict_scheduler_handle)
        self.scheduler_group_attrs[self.sch_group_list[5].oid][SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] =\
        ["Scheduler Handle", self.strict_scheduler_handle]

        print("Setting the Queue 7 to Strict scheduler")
        self.attribute_set(self.sch_group_list[8].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                           self.strict_scheduler_handle)
        self.scheduler_group_attrs[self.sch_group_list[8].oid][SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] =\
        ["Scheduler Handle", self.strict_scheduler_handle]

    def configureDWRRSchedulerShaper(self):
        weight = 100
        # min_rate and max_rate in bits
        min_rate = 80000000
        max_rate = 4000000000
        # max_burst and min_burst in bytes
        min_burst = 1024
        max_burst = 1024

        # shaper type SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS indicates min/max rate is in bits mode.
        self.dwrr_scheduler_attributes = {SWITCH_SCHEDULER_ATTR_TYPE:
                              ["Scheduler type", SWITCH_SCHEDULER_ATTR_TYPE_DWRR],
                              SWITCH_SCHEDULER_ATTR_SHAPER_TYPE:
                              ["Scheduler shaper type", SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS],
                              SWITCH_SCHEDULER_ATTR_WEIGHT:
                              ["Scheduler weight", weight],
                              SWITCH_SCHEDULER_ATTR_MIN_RATE:
                              ["Scheduler min rate", min_rate],
                              SWITCH_SCHEDULER_ATTR_MIN_BURST_SIZE:
                              ["Scheduler min burst size", min_burst],
                              SWITCH_SCHEDULER_ATTR_MAX_RATE:
                              ["Scheduler max rate", max_rate],
                              SWITCH_SCHEDULER_ATTR_MAX_BURST_SIZE:
                              ["Scheduler max burst", max_burst]}

        # create DWRR scheduler profile
        self.dwrr_scheduler_handle = self.add_scheduler(self.device, type=SWITCH_SCHEDULER_ATTR_TYPE_DWRR,
                               shaper_type=SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS,
                               weight=weight, min_rate=min_rate,
                               min_burst_size=min_burst, max_rate=max_rate,
                               max_burst_size=max_burst)

        # configure queue in second and third scheduler_group as DWRR scheduling
        print("Setting the Queue 1 to DWRR scheduler")
        self.attribute_set(self.sch_group_list[2].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                            self.dwrr_scheduler_handle)
        print("Setting the Queue 2 to DWRR scheduler")
        self.attribute_set(self.sch_group_list[3].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                            self.dwrr_scheduler_handle)
        self.scheduler_group_attrs[self.sch_group_list[2].oid][SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] =\
        ["Scheduler Handle", self.dwrr_scheduler_handle]
        self.scheduler_group_attrs[self.sch_group_list[3].oid][SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] =\
        ["Scheduler Handle", self.dwrr_scheduler_handle]

    def configurePortShaper(self):
        print("Create/Get/Set PortShaperTest")
        weight = 100
        # min_rate and max_rate in bits
        min_rate = 80000000
        max_rate = 4000000000
        # max_burst and min_burst in bytes
        min_burst = 1024
        max_burst = 1024

        # shaper type SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS indicates min/max rate is in bits mode.
        self.port_scheduler_attributes = {SWITCH_SCHEDULER_ATTR_TYPE:
                              ["Scheduler type", SWITCH_SCHEDULER_ATTR_TYPE_DWRR],
                              SWITCH_SCHEDULER_ATTR_SHAPER_TYPE:
                              ["Scheduler shaper type", SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS],
                              SWITCH_SCHEDULER_ATTR_WEIGHT:
                              ["Scheduler weight", weight],
                              SWITCH_SCHEDULER_ATTR_MIN_RATE:
                              ["Scheduler min rate", min_rate],
                              SWITCH_SCHEDULER_ATTR_MIN_BURST_SIZE:
                              ["Scheduler min burst size", min_burst],
                              SWITCH_SCHEDULER_ATTR_MAX_RATE:
                              ["Scheduler max rate", max_rate],
                              SWITCH_SCHEDULER_ATTR_MAX_BURST_SIZE:
                              ["Scheduler max burst", max_burst]}

        # create DWRR scheduler profile
        self.port_dwrr_scheduler_handle = self.add_scheduler(self.device, type=SWITCH_SCHEDULER_ATTR_TYPE_DWRR,
                               shaper_type=SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS,
                               weight=weight, min_rate=min_rate,
                               min_burst_size=min_burst, max_rate=max_rate,
                               max_burst_size=max_burst)

        # configure shaper on self.port0
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_SCHEDULER_HANDLE,
                            self.port_dwrr_scheduler_handle)

    def print_queue_priority(self):
       print("QueuePriority Values: ")
       print('Queue 0 priority: ', self.queue_prio_list[0].u8data)
       print('Queue 1 priority: ', self.queue_prio_list[1].u8data)
       print('Queue 2 priority: ', self.queue_prio_list[2].u8data)
       print('Queue 3 priority: ', self.queue_prio_list[3].u8data)
       print('Queue 4 priority: ', self.queue_prio_list[4].u8data)
       print('Queue 5 priority: ', self.queue_prio_list[5].u8data)
       print('Queue 6 priority: ', self.queue_prio_list[6].u8data)
       print('Queue 7 priority: ', self.queue_prio_list[7].u8data)

    def QueuePriorityTest(self):
        try:
          print("QueuePriorityTest()")
          self.queue_prio_list = self.attribute_get(self.port0, SWITCH_PORT_ATTR_QUEUE_PRIORITIES)
          self.print_queue_priority()
          if((self.queue_prio_list[0].u8data != 5) or
             (self.queue_prio_list[4].u8data != 6) or
             (self.queue_prio_list[7].u8data != 7)):
            self.assertTrue("QueuePriority is not set properly by scheduler_group")

          print("Setting the Queue 4 to DWRR scheduler")
          self.attribute_set(self.sch_group_list[5].oid,
                             SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                             self.dwrr_scheduler_handle)
          self.scheduler_group_attrs[self.sch_group_list[5].oid][SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] = \
                            ["Scheduler Handle", self.dwrr_scheduler_handle]

          print("Setting the Queue 6 to DWRR scheduler")
          self.attribute_set(self.sch_group_list[7].oid,
                             SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                             self.dwrr_scheduler_handle)
          self.scheduler_group_attrs[self.sch_group_list[7].oid][SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] = \
                            ["Scheduler Handle", self.dwrr_scheduler_handle]

          self.queue_prio_list = self.attribute_get(self.port0, SWITCH_PORT_ATTR_QUEUE_PRIORITIES)
          self.print_queue_priority()
          if((self.queue_prio_list[4].u8data != 0) or
             (self.queue_prio_list[6].u8data != 0)):
            self.assertTrue("QueuePriority is not set properly by scheduler_group")

          print("Setting the Queue 5 to Strict scheduler")
          self.attribute_set(self.sch_group_list[6].oid,
                             SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                             self.strict_scheduler_handle)
          self.scheduler_group_attrs[self.sch_group_list[6].oid][SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] = \
                             ["Scheduler Handle", self.strict_scheduler_handle]

          self.queue_prio_list = self.attribute_get(self.port0, SWITCH_PORT_ATTR_QUEUE_PRIORITIES)
          self.print_queue_priority()
          if(self.queue_prio_list[5].u8data != 6):
            self.assertTrue("QueuePriority is not set properly by scheduler_group")

        finally:
          pass

    def setUp(self):
        print()
        print('Configuring devices for schedulerGroup test cases')
        self.configure()

        self.sch_group_list = self.attribute_get(self.port0,
                         SWITCH_PORT_ATTR_PORT_QUEUE_SCHEDULER_GROUP_HANDLES)
        self.scheduler_group_attrs = {}
        for sch_group_handle in self.sch_group_list:
            scheduler_grp = {}
            print("Scheduler group handle 0x%lx"%(sch_group_handle.oid))
            # print queue and port handle
            group_type = self.attribute_get(sch_group_handle.oid, SWITCH_SCHEDULER_GROUP_ATTR_TYPE)
            scheduler_handle = self.attribute_get(sch_group_handle.oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE)
            scheduler_grp[SWITCH_SCHEDULER_GROUP_ATTR_TYPE] = ["Scheduler Group Type", group_type]
            scheduler_grp[SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE] = ["Scheduler Handle", scheduler_handle]
            if group_type == SWITCH_SCHEDULER_GROUP_ATTR_TYPE_PORT:
                port_handle = self.attribute_get(sch_group_handle.oid, SWITCH_SCHEDULER_GROUP_ATTR_PORT_HANDLE)
                print("Port handle from sch  0x%lx"%(port_handle))
                scheduler_grp[SWITCH_SCHEDULER_GROUP_ATTR_PORT_HANDLE] = ["Port Handle", port_handle]
            else:
                queue_handle = self.attribute_get(sch_group_handle.oid, SWITCH_SCHEDULER_GROUP_ATTR_QUEUE_HANDLE)
                print("Queue handle from sch  0x%lx"%(queue_handle))
                scheduler_grp[SWITCH_SCHEDULER_GROUP_ATTR_QUEUE_HANDLE] = ["Queue Handle", queue_handle]
            self.scheduler_group_attrs[sch_group_handle.oid] = scheduler_grp
        print("")

        self.configureStrictScheduler()
        self.configureDWRRSchedulerShaper()
        self.configurePortShaper()

    def runTest(self):
        try:
            self.QueuePriorityTest()
            self.StrictSchedulerShaperTest()
            self.DWRRSchedulerShaperTest()
            self.PortShaperTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.sch_group_list[1].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE, 0)
        self.attribute_set(self.sch_group_list[2].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE, 0)
        self.attribute_set(self.sch_group_list[3].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE, 0)
        self.attribute_set(self.sch_group_list[5].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE, 0)
        self.attribute_set(self.sch_group_list[6].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE, 0)
        self.attribute_set(self.sch_group_list[7].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE, 0)
        self.attribute_set(self.sch_group_list[8].oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_SCHEDULER_HANDLE, 0)
        self.cleanup()

    def StrictSchedulerShaperTest(self):
        try:
            print("StrictSchedulerShaperTest()")
            # verify configured profile.
            self.checkSchedulerAttribute(self.strict_scheduler_handle,
                                  self.strict_scheduler_attributes)
            # verify configured scheduler group.
            self.checkSchedulerGroupAttribute(self.sch_group_list[1].oid,
                                  self.scheduler_group_attrs[self.sch_group_list[1].oid])
        finally:
            pass

    def DWRRSchedulerShaperTest(self):
        try:
            print("DWRRSchedulerShaperTest()")
            # verify configured profile.
            self.checkSchedulerAttribute(self.dwrr_scheduler_handle,
                                    self.dwrr_scheduler_attributes)

            # change DWRR Scheduler shaper rate(bits)
            max_rate = 2000000000
            self.dwrr_scheduler_attributes[SWITCH_SCHEDULER_ATTR_MAX_RATE][1] = max_rate
            self.attribute_set(self.dwrr_scheduler_handle, SWITCH_SCHEDULER_ATTR_MAX_RATE,
                            max_rate)

            # verify configured profile.
            self.checkSchedulerAttribute(self.dwrr_scheduler_handle,
                                        self.dwrr_scheduler_attributes)

            # verify configured scheduler group.
            self.checkSchedulerGroupAttribute(self.sch_group_list[2].oid,
                                    self.scheduler_group_attrs[self.sch_group_list[2].oid])
            self.checkSchedulerGroupAttribute(self.sch_group_list[3].oid,
                                    self.scheduler_group_attrs[self.sch_group_list[3].oid])
            self.checkSchedulerGroupAttribute(self.sch_group_list[4].oid,
                                    self.scheduler_group_attrs[self.sch_group_list[4].oid])
        finally:
            pass

    def PortShaperTest(self):
        try:
            # verify configured profile.
            self.checkSchedulerAttribute(self.port_dwrr_scheduler_handle,
                                  self.port_scheduler_attributes)

            thrift_attr_value = self.attribute_get(self.port0, SWITCH_PORT_ATTR_SCHEDULER_HANDLE)
            if thrift_attr_value != self.port_dwrr_scheduler_handle:
                self.assertTrue(False, "Port scheduler profile id is incorrect, expected: 0x%lx got: 0x%lx "%\
                         (self.port_dwrr_scheduler_handle, thrift_attr_value))
            else:
                print("Port scheduler profile id is correct")
        finally:
            pass

###############################################################################

@group('qos')
class BufferAttribute(ApiHelper):
    '''
    This test verify the Buffer Pool and Buffer Profile APIs.
    '''

    #Ingress Buffer Pool Params
    ingress_pool_size = 1024
    ingress_pool_dir = SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS
    ingress_pool_threshold_mode = SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE_DYNAMIC
    ingress_buffer_pool_attr_list = {SWITCH_BUFFER_POOL_ATTR_POOL_SIZE:
                        ["Buffer pool size", ingress_pool_size],
                        SWITCH_BUFFER_POOL_ATTR_DIRECTION:
                        ["Buffer pool type", ingress_pool_dir],
                        SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE:
                        ["Buffer threshold mode", ingress_pool_threshold_mode]
                        }
    ingress_buffer_pool_handle = 0

    # Ingress buffer profile Attrs.
    ingress_buffer_size = 1024
    ingress_buffer_profile_threshold = 128
    ingress_buffer_profile_threshold_mode = SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC
    ingress_buffer_profile_attr_list = {SWITCH_BUFFER_PROFILE_ATTR_BUFFER_SIZE:
                        ["Buffer profile size", ingress_buffer_size],
                        SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE:
                        ["Buffer profile threshold mode", ingress_buffer_profile_threshold_mode],
                        SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD:
                        ["Buffer threshold limit", ingress_buffer_profile_threshold],
                        SWITCH_BUFFER_PROFILE_ATTR_BUFFER_POOL_HANDLE:
                        ["Buffer profile pool handle", ingress_buffer_pool_handle]
                        }
    ingress_buffer_profile_handle = 0

    # Egress Buffer Pool Attrs
    egress_pool_size = 1024
    egress_pool_dir = SWITCH_BUFFER_POOL_ATTR_DIRECTION_EGRESS
    egress_pool_threshold_mode = SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE_DYNAMIC
    queue_list = []
    egress_buffer_pool_attr_list = {SWITCH_BUFFER_POOL_ATTR_POOL_SIZE:
                        ["Buffer pool size", egress_pool_size],
                        SWITCH_BUFFER_POOL_ATTR_DIRECTION:
                        ["Buffer pool type", egress_pool_dir],
                        SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE:
                        ["Buffer threshold mode", egress_pool_threshold_mode]
                        }
    egress_buffer_pool_handle = 0

    # create egress buffer profile.
    egress_buffer_size = 1024
    egress_xon_threshold = 1024
    egress_buffer_profile_threshold = 128
    egress_buffer_profile_threshold_mode = SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC
    egress_buffer_profile_attr_list = {SWITCH_BUFFER_PROFILE_ATTR_BUFFER_SIZE:
                        ["Buffer profile size", egress_buffer_size],
                        SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE:
                        ["Buffer profile threshould mode", egress_buffer_profile_threshold_mode],
                        SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD:
                        ["Buffer threshold limit", egress_buffer_profile_threshold],
                        SWITCH_BUFFER_PROFILE_ATTR_BUFFER_POOL_HANDLE:
                        ["Buffer profile pool handle", egress_buffer_pool_handle],
                        SWITCH_BUFFER_PROFILE_ATTR_XON_THRESHOLD:
                        ["Buffer profile XON threshold", egress_xon_threshold]
                        }
    egress_buffer_profile_handle = 0

    def setUp(self):
        print()
        print('Configuring devices for BufferAttribute test cases')
        self.configure()

        # create ingress buffer pool & profile.
        self.ingress_buffer_pool_handle  = self.add_buffer_pool(self.device,
                        direction = self.ingress_pool_dir,
                        threshold_mode = self.ingress_pool_threshold_mode,
                        pool_size=self.ingress_pool_size)
        # Update the profile attr list with pool handle
        self.ingress_buffer_profile_attr_list[SWITCH_BUFFER_PROFILE_ATTR_BUFFER_POOL_HANDLE] = ["Buffer profile pool\
      handle", self.ingress_buffer_pool_handle]
        self.ingress_buffer_profile_handle = self.add_buffer_profile(self.device,
                           threshold_mode = self.ingress_buffer_profile_threshold_mode,
                           threshold = self.ingress_buffer_profile_threshold,
                           buffer_size = self.ingress_buffer_size,
                           buffer_pool_handle = self.ingress_buffer_pool_handle)

        # create egress buffer pool & profile.
        self.egress_buffer_pool_handle  = self.add_buffer_pool(self.device,
                        direction = self.egress_pool_dir,
                        threshold_mode = self.egress_pool_threshold_mode,
                        pool_size = self.egress_pool_size)
        # Update the profile attr list with pool handle
        self.egress_buffer_profile_attr_list[SWITCH_BUFFER_PROFILE_ATTR_BUFFER_POOL_HANDLE] = ["Buffer profile pool handle",
        self.egress_buffer_pool_handle]
        self.egress_buffer_profile_handle = self.add_buffer_profile(self.device,
                        threshold_mode = self.egress_buffer_profile_threshold_mode,
                        threshold = self.egress_buffer_profile_threshold,
                        buffer_size = self.egress_buffer_size,
                        buffer_pool_handle = self.egress_buffer_pool_handle, xon_threshold=self.egress_xon_threshold)

    def runTest(self):
        try:
            self.IngressBufferTest()
            self.EgressBufferTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def IngressBufferTest(self):
        try:
            print("IngressBufferTest()")
            self.assertTrue(self.CheckObjectAttributes(self.ingress_buffer_pool_handle, self.ingress_buffer_pool_attr_list),
                 "Check SchedulerAttribute failed for ingress_buffer_pool_handle: 0x%lx"%(self.ingress_buffer_pool_handle))
            print("Ingress buffer handle 0x%lx"%(self.ingress_buffer_pool_handle))

            self.assertTrue(self.CheckObjectAttributes(self.ingress_buffer_profile_handle, self.ingress_buffer_profile_attr_list),\
                 "Check SchedulerAttribute failed for ingress_buffer_profile_handle: 0x%lx"%(self.ingress_buffer_profile_handle))
            print("Ingress buffer profile handle 0x%lx"%(self.ingress_buffer_profile_handle))
        finally:
            pass

    def EgressBufferTest(self):
        try:
            print("EgressBufferTest()")
            self.assertTrue(self.CheckObjectAttributes(self.egress_buffer_pool_handle, self.egress_buffer_pool_attr_list),
                 "Check SchedulerAttribute failed for egress_buffer_pool_handle: 0x%lx"%(self.egress_buffer_pool_handle))
            print("Egress buffer handle 0x%lx"%(self.egress_buffer_pool_handle))

            self.assertTrue(self.CheckObjectAttributes(self.egress_buffer_profile_handle, self.egress_buffer_profile_attr_list),\
                 "Check SchedulerAttribute failed for egress_buffer_profile_handle: 0x%lx"%(self.egress_buffer_profile_handle))
            print("egress buffer profile handle 0x%lx"%(self.egress_buffer_profile_handle))

            print("Configure Egress buffer profile to queue 5")
            queue_id = 5
            queue_list = self.attribute_get(self.port0,
                             SWITCH_PORT_ATTR_QUEUE_HANDLES)
            for queue_handle in queue_list:
                qid = self.attribute_get(queue_handle.oid,
                                         SWITCH_QUEUE_ATTR_QUEUE_ID)
                if qid == queue_id:
                    print("Attaching buffer_profile  - Queue handle from port 0x%lx, Egress buffer prof 0x%lx"\
                          %(queue_handle.oid, self.egress_buffer_profile_handle))
                    self.queue_handle = queue_handle.oid
                    self.attribute_set(self.queue_handle, SWITCH_QUEUE_ATTR_BUFFER_PROFILE_HANDLE,
                                       self.egress_buffer_profile_handle)

        finally:
            self.attribute_set(self.queue_handle, SWITCH_QUEUE_ATTR_BUFFER_PROFILE_HANDLE, 0)
            pass

###############################################################################

@group('qos')
class PortPPGAttribute(ApiHelper):
    '''
    This test verify the Port PPG and PFC APIs.
    '''

    ingress_buffer_profile_handle = 0
    device_ingress_buffer_profile = 0
    ingress_ppg1_handle = 0
    ingress_ppg2_handle = 0

    ppg_index = 1
    # PPG for Port0
    ppg1_attr_list = {SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE:
                        ["Buffer profile handle", 0],
                        SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX:
                        ["Priority group index", ppg_index],
                        SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW:
                        ["created_in_hw", False],
                        SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE:
                        ["buffer_profile_in_use", 0]
                        }

    # PPG for Port1
    ppg2_attr_list = {SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE:
                        ["Buffer profile handle", 0],
                        SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX:
                        ["Priority group index", ppg_index],
                        SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW:
                        ["created_in_hw", False],
                        SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE:
                        ["buffer_profile_in_use", 0]
                        }

    def configureIcosToPPGMaps(self, ppg_index):
        print("Configuring icos-ppg maps")
        qos_map1 = self.add_qos_map(self.device, icos=1, ppg=ppg_index)
        qos_map2 = self.add_qos_map(self.device, icos=2, ppg=ppg_index)
        qos_map3 = self.add_qos_map(self.device, icos=3, ppg=ppg_index)
        qos_map4 = self.add_qos_map(self.device, icos=4, ppg=ppg_index)
        icos_ppg_list = [qos_map1, qos_map2, qos_map3, qos_map4]
        icos_ppg_maps = []
        for qos_map in icos_ppg_list:
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        self.icos_to_ppg_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)

    def setUp(self):
        print()
        print('Configuring devices for PortPPGAttribute test cases')
        self.configure()

        # create ingress buffer pool.
        ingress_pool_size = 10080
        if self.arch == 'tofino2':
            self.exp_ingress_pool_size = 10208
        else:
            self.exp_ingress_pool_size = ingress_pool_size
        ingres_buffer_pool_handle  = self.add_buffer_pool(self.device,
                        direction=SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS,
                        threshold_mode=SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE_DYNAMIC,
                        pool_size=ingress_pool_size)

        # create ingress buffer profile.
        self.ingress_buffer_gmin = 1024
        if self.arch == 'tofino':
            self.exp_ingress_buffer_gmin = 1040
        elif self.arch == 'tofino2':
            self.exp_ingress_buffer_gmin = 1056
        else:
            self.exp_ingress_buffer_gmin = self.ingress_buffer_gmin
        threshold = 192
        xoff_threshold = 512
        if self.arch == 'tofino':
            self.exp_ingress_xoff_threshold = 560
        elif self.arch == 'tofino2':
            self.exp_ingress_xoff_threshold = 528
        else:
            self.exp_ingress_xoff_threshold = xoff_threshold
        xon_threshold = 1024
        self.ingress_buffer_profile_handle = self.add_buffer_profile(self.device,
                           threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
                           threshold=threshold,
                           buffer_size=self.ingress_buffer_gmin,
                           xon_threshold = xon_threshold,
                           xoff_threshold = xoff_threshold,
                           buffer_pool_handle=ingres_buffer_pool_handle)
        print("Ingress buffer profile handle 0x%lx"%(self.ingress_buffer_profile_handle))

        # create static ingress buffer profile
        self.ingress_buffer_gmin_2 = 0
        self.exp_ingress_buffer_gmin_2 = self.ingress_buffer_gmin_2
        threshold = 2048
        if self.arch == 'tofino':
            self.exp_ingress_threshold = 2080
        elif self.arch == 'tofino2':
            self.exp_ingress_threshold = 2112
        else:
            self.exp_ingress_threshold = threshold
        self.static_ingress_buffer_profile_handle = self.add_buffer_profile(self.device,
                           threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_STATIC,
                           threshold=threshold,
                           buffer_size=self.ingress_buffer_gmin_2,
                           xon_threshold = xon_threshold,
                           xoff_threshold = xoff_threshold,
                           buffer_pool_handle=ingres_buffer_pool_handle)
        print("Static ingress buffer profile handle 0x%lx"%(self.static_ingress_buffer_profile_handle))

        # create device level ingress buffer profile.
        ingress_buffer_gmin_3 = 2048
        if self.arch == 'tofino':
            self.exp_ingress_buffer_gmin_3 = 2080
        elif self.arch == 'tofino2':
            self.exp_ingress_buffer_gmin_3 = 2112
        else:
            self.exp_ingress_buffer_gmin_3 = ingress_buffer_gmin_3
        threshold = 192
        self.device_ingress_buffer_profile = self.add_buffer_profile(self.device,
                           threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
                           threshold=threshold,
                           buffer_size=ingress_buffer_gmin_3,
                           buffer_pool_handle=ingres_buffer_pool_handle)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, self.device_ingress_buffer_profile)
        print("Ingress buffer profile handle (Device default) 0x%lx"%(self.device_ingress_buffer_profile))

        # create PPGs.
        self.ingress_ppg1_handle = self.add_port_priority_group(self.device,
                          ppg_index =  self.ppg_index,
                          port_handle = self.port0)
        print("Created Ingress ppg, handle 0x%lx"%(self.ingress_ppg1_handle))
        self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE] = ["Ingress port handle", self.port0]

        # create PPGs.
        self.ingress_ppg2_handle = self.add_port_priority_group(self.device,
                          ppg_index =  self.ppg_index,
                          port_handle = self.port1)
        print("Created Ingress ppg, handle 0x%lx"%(self.ingress_ppg2_handle))
        self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE] = ["Ingress port handle", self.port1]

        # create icos_to_ppg mapping for ppg_index
        self.configureIcosToPPGMaps(self.ppg_index)

        print("Attaching ingress_icos_ppg_map  0x%lx to port 0x%lx"%(self.icos_to_ppg_map_ingress, self.port1))
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)
        self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW] = ["created_in_hw", True]
        self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", self.device_ingress_buffer_profile]

    def runTest(self):
        try:
            self.PortPPGTest()
            self.PortPPGTest2()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, 0)
        #self.attribute_set(self.ingress_ppg1_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, 0)
        self.cleanup()

    def configurePfcPriorityToQueueMaps(self):
        print("Configuring pfc_priority to queue maps")
        qos_map1 = self.add_qos_map(self.device, pfc_priority=1, qid=0)
        qos_map2 = self.add_qos_map(self.device, pfc_priority=2, qid=0)
        qos_map3 = self.add_qos_map(self.device, pfc_priority=3, qid=0)
        qos_map4 = self.add_qos_map(self.device, pfc_priority=4, qid=0)
        pfc_priority_queue_list = [qos_map1, qos_map2, qos_map3, qos_map4]
        pfc_priority_queue_maps = []
        for qos_map in pfc_priority_queue_list:
            pfc_priority_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        self.pfc_priority_queue_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_PFC_PRIORITY_TO_QUEUE, qos_map_list=pfc_priority_queue_maps)

    def verifyPpgMaxOccupancy(self, ppg_handle, ppg_counter_type, expected_bytes_count=0):
        ppg_counters = self.object_counters_get(ppg_handle)
        for counter in ppg_counters:
            if counter.counter_id == ppg_counter_type:
                if counter.counter_id == SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_GUARANTEED_BYTES:
                    print("Received : %d, expected: %d"%(counter.count, expected_bytes_count))
                    return (counter.count == expected_bytes_count)
                if counter.counter_id == SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SHARED_MAX_OCCUPANCY_BYTES:
                    print("Received : %d, expected: %d"%(counter.count, expected_bytes_count))
                    return (counter.count == expected_bytes_count)
            if counter.counter_id == SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SKID_MAX_OCCUPANCY_BYTES:
                print("Received : %d, expected: %d"%(counter.count, expected_bytes_count))
                return (counter.count == expected_bytes_count)

    def verifyQueueMaxOccupancy(self, queue_handle, queue_cntr_type, expected_pkt_count=0, expected_bytes_count=0):
        queue_cntrs = self.object_counters_get(queue_handle)
        for cntr in queue_cntrs:
            if cntr.counter_id == queue_cntr_type:
                if cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_MAX_OCCUPANCY_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                else:
                    print("Invalid queue counter_id: %d" % cntr.counter_id)
                    return 0

    def PortPPGTest(self):
        try:
            print("PortPPGTest()")
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg1_handle, self.ppg1_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg1_handle))

            # disable pgg lossless pfc frame
            self.attribute_set(self.ingress_ppg1_handle,
                      SWITCH_PORT_PRIORITY_GROUP_ATTR_LOSSLESS_ADMIN_ENABLE, False)

            # enable pgg lossless pfc frame
            self.attribute_set(self.ingress_ppg1_handle,
                      SWITCH_PORT_PRIORITY_GROUP_ATTR_LOSSLESS_ADMIN_ENABLE, True)

            print("Attaching ingress_icos_ppg_map  0x%lx to port 0x%lx"%(self.icos_to_ppg_map_ingress, self.port0))
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW] = ["created_in_hw", True]
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", self.device_ingress_buffer_profile]

            # Verify ppg was created in hardware
            print("Verifying ppg 0x%lx attributes after attaching ingress_icos_ppg_map 0x%lx to port 0x%lx"%(self.ingress_ppg1_handle, self.icos_to_ppg_map_ingress, self.port0))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg1_handle, self.ppg1_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg1_handle))
            print()

            # Attach Buffer Profile to Port Priority Group
            print("Attaching buffer profile 0x%lx to port priority group 0x%lx"%(self.ingress_buffer_profile_handle, self.ingress_ppg1_handle))
            self.attribute_set(self.ingress_ppg1_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, self.ingress_buffer_profile_handle)
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE] = ["Buffer profile handle", self.ingress_buffer_profile_handle]
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", self.ingress_buffer_profile_handle]
            print("Verifying ppg 0x%lx attributes after attaching buffer profile 0x%lx"%(self.ingress_ppg1_handle, self.ingress_buffer_profile_handle))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg1_handle, self.ppg1_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg1_handle))
            print()

            print("Verifying ppg max occupancy values from buffer profile")
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg1_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_GUARANTEED_BYTES,
                expected_bytes_count=self.exp_ingress_buffer_gmin),
                "Verify ppg gmin failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg1_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg1_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SHARED_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=(self.exp_ingress_pool_size/2)), #baf 6 -> 50%
                "Verify ppg shared max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg1_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg1_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SKID_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=self.exp_ingress_xoff_threshold),
                "Verify ppg skid max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg1_handle))
            print()

            # Attach Static Buffer Profile to Port Priority Group
            print("Attaching buffer profile 0x%lx to port priority group 0x%lx"%(self.static_ingress_buffer_profile_handle, self.ingress_ppg1_handle))
            self.attribute_set(self.ingress_ppg1_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, self.static_ingress_buffer_profile_handle)
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE] = ["Buffer profile handle", self.static_ingress_buffer_profile_handle]
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", self.static_ingress_buffer_profile_handle]
            print("Verifying ppg 0x%lx attributes after attaching buffer profile 0x%lx"%(self.ingress_ppg1_handle, self.static_ingress_buffer_profile_handle))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg1_handle, self.ppg1_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg1_handle))

            print("Verifying ppg max occupancy values from buffer profile")
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg1_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_GUARANTEED_BYTES,
                expected_bytes_count=self.exp_ingress_buffer_gmin_2),
                "Verify ppg gmin failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg1_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg1_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SHARED_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=self.exp_ingress_threshold),
                "Verify ppg shared max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg1_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg1_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SKID_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=self.exp_ingress_xoff_threshold),
                "Verify ppg skid max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg1_handle))
            print()

            # Detach PPG Buffer Profile
            self.attribute_set(self.ingress_ppg1_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, 0)
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE] = ["Buffer profile handle", 0]
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", self.device_ingress_buffer_profile]
            print("Verifying ppg 0x%lx attributes after detaching buffer profile 0x%lx"%(self.ingress_ppg1_handle, self.static_ingress_buffer_profile_handle))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg1_handle, self.ppg1_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg1_handle))

            print("Verifying ppg max occupancy values from device buffer profile")
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg1_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_GUARANTEED_BYTES,
                expected_bytes_count=self.exp_ingress_buffer_gmin_3),
                "Verify ppg gmin failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg1_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg1_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SHARED_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=(self.exp_ingress_pool_size/2)), #baf 6 -> 50%
                "Verify ppg shared max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg1_handle))
            print()

            ## Detach Device level Defaul Buffer Profile
            #self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, 0)
            #self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", 0]
            #print("Verifying ppg 0x%lx attributes after detaching buffer profile 0x%lx"%(self.ingress_ppg1_handle, device_ingress_buffer_profile))
            #self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg1_handle, self.ppg1_attr_list),
            #     "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg1_handle))
            #print()

            # Remove icos_to_ppg mapping from port0
            print("Detaching ingress_icos_ppg_map  0x%lx from port 0x%lx"%(self.icos_to_ppg_map_ingress, self.port0))
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.ppg1_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW] = ["created_in_hw", False]
            # Verify ppg was deleted in hardware
            print("Verifying ppg 0x%lx attributes after detaching ingress_icos_ppg_map 0x%lx from port 0x%lx"%(self.ingress_ppg1_handle, self.icos_to_ppg_map_ingress, self.port0))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg1_handle, self.ppg1_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg1_handle))
            print()

            # create pfc_priority_to_queue mapping
            #self.configurePfcPriorityToQueueMaps()
            #self.attribute_set(self.port0, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,
            #                   self.pfc_priority_queue_map_egress)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            #self.attribute_set(self.port0, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.ingress_ppg1_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, 0)
            pass

    def PortPPGTest2(self):
        try:
            print("PortPPGTest2()")
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg2_handle, self.ppg2_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg2_handle))
            print()

            # disable pgg lossless pfc frame
            self.attribute_set(self.ingress_ppg2_handle,
                      SWITCH_PORT_PRIORITY_GROUP_ATTR_LOSSLESS_ADMIN_ENABLE, False)

            # enable pgg lossless pfc frame
            self.attribute_set(self.ingress_ppg2_handle,
                      SWITCH_PORT_PRIORITY_GROUP_ATTR_LOSSLESS_ADMIN_ENABLE, True)


            # Attach Buffer Profile to Port Priority Group
            print("Attaching buffer profile 0x%lx to port priority group 0x%lx"%(self.ingress_buffer_profile_handle,
            self.ingress_ppg2_handle))
            self.attribute_set(self.ingress_ppg2_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, self.ingress_buffer_profile_handle)
            self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE] = ["Buffer profile handle", self.ingress_buffer_profile_handle]
            self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", self.ingress_buffer_profile_handle]
            print("Verifying ppg 0x%lx attributes after attaching buffer profile 0x%lx"%(self.ingress_ppg2_handle, self.ingress_buffer_profile_handle))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg2_handle, self.ppg2_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg2_handle))
            print()

            print("Verifying ppg max occupancy values from buffer profile")
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_GUARANTEED_BYTES,
                expected_bytes_count=self.exp_ingress_buffer_gmin),
                "Verify ppg gmin failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SHARED_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=(self.exp_ingress_pool_size/2)), #baf 6 -> 50%
                "Verify ppg shared max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SKID_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=self.exp_ingress_xoff_threshold),
                "Verify ppg skid max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))
            print()

            # Modify buffer profile buffer size
            print("Verify ppg gmin after attribute set of buffer_size")
            self.attribute_set(self.ingress_buffer_profile_handle,
                               SWITCH_BUFFER_PROFILE_ATTR_BUFFER_SIZE,
                               self.ingress_buffer_gmin_2)
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_GUARANTEED_BYTES,
                expected_bytes_count=self.exp_ingress_buffer_gmin_2),
                "Verify ppg gmin failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))

            self.attribute_set(self.ingress_buffer_profile_handle,
                               SWITCH_BUFFER_PROFILE_ATTR_BUFFER_SIZE,
                               self.ingress_buffer_gmin)

            # Attach Static Buffer Profile to Port Priority Group
            print("Attaching buffer profile 0x%lx to port priority group 0x%lx"%(self.static_ingress_buffer_profile_handle,
                self.ingress_ppg2_handle))
            self.attribute_set(self.ingress_ppg2_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, self.static_ingress_buffer_profile_handle)
            self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE] = ["Buffer profile handle", self.static_ingress_buffer_profile_handle]
            self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", self.static_ingress_buffer_profile_handle]
            print("Verifying ppg 0x%lx attributes after attaching buffer profile 0x%lx"%(self.ingress_ppg2_handle, self.static_ingress_buffer_profile_handle))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg2_handle, self.ppg2_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg2_handle))

            print("Verifying ppg max occupancy values from buffer profile")
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_GUARANTEED_BYTES,
                expected_bytes_count=self.exp_ingress_buffer_gmin_2),
                "Verify ppg gmin failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SHARED_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=self.exp_ingress_threshold),
                "Verify ppg shared max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SKID_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=self.exp_ingress_xoff_threshold),
                "Verify ppg skid max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))
            print()

            # Detach PPG Buffer Profile
            self.attribute_set(self.ingress_ppg2_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, 0)
            self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE] = ["Buffer profile handle", 0]
            self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE] = ["buffer_profile_in_use", self.device_ingress_buffer_profile]
            print("Verifying ppg 0x%lx attributes after detaching buffer profile 0x%lx"%(self.ingress_ppg2_handle, self.static_ingress_buffer_profile_handle))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg2_handle, self.ppg2_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg2_handle))

            print("Verifying ppg max occupancy values from device buffer profile")
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_GUARANTEED_BYTES,
                expected_bytes_count=self.exp_ingress_buffer_gmin_3),
                "Verify ppg gmin failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))
            self.assertTrue(
                self.verifyPpgMaxOccupancy(self.ingress_ppg2_handle,
                SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_SHARED_MAX_OCCUPANCY_BYTES,
                expected_bytes_count=(self.exp_ingress_pool_size/2)), #baf 6 -> 50%
                "Verify ppg shared max occupancy failed for ppg_handle: 0x%lx "%(
                self.ingress_ppg2_handle))
            print()

            # Remove icos_to_ppg mapping from port1
            print("Detaching ingress_icos_ppg_map  0x%lx from port 0x%lx"%(self.icos_to_ppg_map_ingress, self.port1))
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW] = ["created_in_hw", False]
            # Verify ppg was deleted in hardware
            print("Verifying ppg 0x%lx attributes after detaching ingress_icos_ppg_map 0x%lx from port"
            "0x%lx"%(self.ingress_ppg2_handle, self.icos_to_ppg_map_ingress, self.port1))
            self.assertTrue(self.CheckObjectAttributes(self.ingress_ppg2_handle, self.ppg2_attr_list),
                 "Check Port Priority Group Attributes failed for ingress_ppg_handle: 0x%lx"%(self.ingress_ppg2_handle))
            print()

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)
            self.ppg2_attr_list[SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW] = ["created_in_hw", True]
            #self.attribute_set(self.port0, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.ingress_ppg2_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE, 0)
            pass


###############################################################################

@group('qos')
class PpgQueueStatTest(ApiHelper):
    '''
    this is test ppg stats
    '''
    def_ppg_icos_map = [0,1]
    def_ppg_qid_map = [0,1]
    def_ppg_tc_map = [20,24]
    def_ppg_dscp_map = [1,2]

    non_def_ppg_icos_map = [4,5]
    non_def_ppg_qid_map = [4,5]
    non_def_ppg_tc_map = [28,30]
    non_def_ppg_dscp_map = [9,4]

    # non default ppg settings
    ingress_lossless_pool_size = 100000
    lossless_ppg = 2
    ppg_lossless_g_limit = 1024
    ppg_lossless_index = lossless_ppg
    ppg_lossless_buffer_mode = SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC
    ppg_lossless_buffer_threshold = 128
    ppg_skid_limit = 128

    def createDscpToTcQosMap(self, dscp_list, tc_list):
        print("Configuring DscpToTc maps")
        dscp_tc_maps = []
        for i in range(len(dscp_list)):
            qos_map = self.add_qos_map(self.device, dscp=dscp_list[i], tc=tc_list[i])
            dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))

        dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
              type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)
        return dscp_tc_map_ingress

    def createTcToDscpQosMap(self, tc_list, dscp_list):
        print("Configuring TcToDscp maps")
        tc_dscp_maps = []
        for i in range(len(dscp_list)):
            qos_map = self.add_qos_map(self.device, tc=tc_list[i], dscp=dscp_list[i])
            tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        tc_dscp_map_egress = self.add_qos_map_egress(self.device,
              type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)
        return tc_dscp_map_egress

    def createTcToQueueIcosMap(self, tc_list, icos_list, qid_list):
        print("Configuring TcToQueue maps")
        tc_icos_queue_maps = []
        for i in range(len(tc_list)):
            qos_map = self.add_qos_map(self.device, tc=tc_list[i], icos=icos_list[i], qid=qid_list[i])
            tc_icos_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        tc_icos_maps_ingress = self.add_qos_map_ingress(self.device,
              type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS_AND_QUEUE, qos_map_list=tc_icos_queue_maps)
        return tc_icos_maps_ingress

    def configureIcosToPPGMaps(self, icos_map_list, ppg_index):
        print("Configuring icos-ppg maps")
        icos_ppg_maps = []
        for i in range(len(icos_map_list)):
            qos_map = self.add_qos_map(self.device, icos=icos_map_list[i], ppg=ppg_index)
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        icos_to_ppg_map_ingress = self.add_qos_map_ingress(self.device,
              type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)
        return icos_to_ppg_map_ingress

    def attachDscpQosMap(self, port_handle, map_handle):
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, map_handle)

    def attachTcIcosQosmap(self, port_handle, tc_icos_qos_handle):
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, tc_icos_qos_handle)

    def attachTcDscpQosMap(self, port_handle, tc_dscp_qos_map):
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, tc_dscp_qos_map)

    def createPpgBufferPool(self, ingress_pool_size, mode, b_size, th):
        # create ingress buffer pool profile.
        ingres_buffer_pool_handle  = self.add_buffer_pool(self.device,
                          direction=SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS,
                          threshold_mode=mode,
                          pool_size=ingress_pool_size)

        # create ingress buffer profile.
        ingress_buffer_profile_handle = self.add_buffer_profile(self.device,
                             threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
                             threshold=th,
                             buffer_size=b_size,
                             buffer_pool_handle=ingres_buffer_pool_handle)

        return ingress_buffer_profile_handle

    def createPPG(self, port_handle, ppg_index, ingress_buffer_profile_handle):
        ingress_ppg_handle = self.add_port_priority_group(self.device,
                   buffer_profile_handle=ingress_buffer_profile_handle,
                   ppg_index =  ppg_index,
                   port_handle = port_handle)
        return ingress_ppg_handle

    def attachIcosPpgQosmap(self, port_handle, icos_ppg_qos_map_handle):
        print("attachIcosPpgQosmap: port_handle: 0x%lx, icos_ppg_qos_map_handle: 0x%lx"%(port_handle,icos_ppg_qos_map_handle))
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, icos_ppg_qos_map_handle)

    def verifyPpgStats(self, ppg_handle, expected_pkt_count):
        counter = self.object_counters_get(ppg_handle)
        print("Received : %d, expected: %d"%(counter[0].count, expected_pkt_count))
        return (counter[0].count == expected_pkt_count)

    def verifyQueueStats(self, queue_handle, queue_cntr_type, expected_pkt_count=0, expected_bytes_count=0):
        queue_cntrs = self.object_counters_get(queue_handle)
        for cntr in queue_cntrs:
            if cntr.counter_id == queue_cntr_type:
                if cntr.counter_id == SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS:
                    print("Received : %d, expected: %d"%(cntr.count, expected_pkt_count))
                    return (cntr.count == expected_pkt_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_STAT_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_DROPPED_PACKETS:
                    print("Received : %d, expected: %d"%(cntr.count, expected_pkt_count))
                    return (cntr.count == expected_pkt_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_CURR_OCCUPANCY_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_WATERMARK_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_MAX_OCCUPANCY_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                else:
                    print("Invalid queue counter_id: %d" % cntr.counter_id)
                    return 0

    def qid_to_handle(self, port_handle, qid):
        queue_list = self.attribute_get(port_handle, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        for queue_handle in queue_list:
            queue_id = self.attribute_get(queue_handle.oid, SWITCH_QUEUE_ATTR_QUEUE_ID)
            if qid == queue_id:
                return queue_handle.oid

        print("Invalid queue id: %d, queue_handle does not exists" % qid)
        return 0

    def setUp(self):
        self.configure()
        self._feature_check = True
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS) == 0):
            self._feature_check = False
            print("PPG feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) == 0):
            self._feature_check = False
            print("QoS Map feature not enabled, skipping")
            return

        dscp_test_ports = [self.port0, self.port1]

        # configure port0 and port1 for dscp/tos testing
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        # create ingress DSCP to TC map
        dscp_map_list = []
        dscp_map_list.extend(self.def_ppg_dscp_map)
        dscp_map_list.extend(self.non_def_ppg_dscp_map)

        tc_map_list = []
        tc_map_list.extend(self.def_ppg_tc_map)
        tc_map_list.extend(self.non_def_ppg_tc_map)

        icos_map_list = []
        icos_map_list.extend(self.def_ppg_icos_map)
        icos_map_list.extend(self.non_def_ppg_icos_map)

        qid_map_list = []
        qid_map_list.extend(self.def_ppg_qid_map)
        qid_map_list.extend(self.non_def_ppg_qid_map)

        self.ingress_dscp_tc_map_handle = self.createDscpToTcQosMap(dscp_map_list, tc_map_list)

        # create ingress tc to icos/qid map
        self.tc_queue_icos_handle = self.createTcToQueueIcosMap(tc_map_list, icos_map_list, qid_map_list)

        # create egress tc to dscp map
        self.egress_tc_dscp_map_handle = self.createTcToDscpQosMap(tc_map_list, dscp_map_list)

        # create ingress buffer profile.
        self.ingress_ppg_buffer_handle =  \
                     self.createPpgBufferPool(self.ingress_lossless_pool_size,
                                    self.ppg_lossless_buffer_mode,
                                    self.ppg_lossless_g_limit,
                                    self.ppg_lossless_buffer_threshold)
        # create icos to ppg mapping
        self.icos_ppg_map_handle = self.configureIcosToPPGMaps(self.non_def_ppg_icos_map, self.lossless_ppg)

        # Attach DSCP to TC on self.port0
        self.attachDscpQosMap(self.port0, self.ingress_dscp_tc_map_handle)

        # Attach tc to icos map on self.port0
        self.attachTcIcosQosmap(self.port0, self.tc_queue_icos_handle)

        # Attach TC to DsCP on self.port1
        self.attachTcDscpQosMap(self.port1, self.egress_tc_dscp_map_handle)

        # Configure non def PPG
        self.port0_ppg_handle = self.createPPG(self.port0, self.lossless_ppg,
                                      self.ingress_ppg_buffer_handle)

        # Attach icos_to_ppg mapping to port
        self.attachIcosPpgQosmap(self.port0, self.icos_ppg_map_handle)

        # Clear default ppg stats
        self.port0_def_ppg_handle = self.attribute_get(self.port0,
                                       SWITCH_PORT_ATTR_DEFAULT_PPG)
        print("Ingress port0 def_ppg handle 0x%lx"%(self.port0_def_ppg_handle))
        self.client.object_counters_clear_all(self.port0_def_ppg_handle)

    def runTest(self):
        if(self._feature_check):
            try:
                self.PortPPGStatTest(10)
                self.PortQueueStatTest()
            finally:
                self.QosMapCleanup()
                pass

    def QosMapCleanup(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
        self.attribute_set(self.ingress_dscp_tc_map_handle,
                 SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
        self.attribute_set(self.tc_queue_icos_handle,
                 SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
        self.attribute_set(self.icos_ppg_map_handle,
                 SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
        self.attribute_set(self.egress_tc_dscp_map_handle,
               SWITCH_QOS_MAP_EGRESS_ATTR_QOS_MAP_LIST, [])

    def tearDown(self):
        self.cleanup()

    def PortPPGStatTest(self, check_stats_count):
        print("PortPPGStatTest()")
        try:
            pkt_count = 10

            # send packet to default ppg and check stats
            for i in range(pkt_count):
                pkt = simple_tcp_packet(
                      eth_dst='00:77:66:55:44:33',
                      eth_src='00:22:22:22:22:22',
                      ip_dst='172.20.10.1',
                      ip_src='192.168.0.1',
                      ip_id=105,
                      ip_tos=4, # dscp 1
                      ip_ttl=64)
                send_packet(self, self.devports[0], pkt)
            time.sleep(5)
            self.assertTrue(self.verifyPpgStats(self.port0_def_ppg_handle, check_stats_count),
                 "Verify PpgStats failed for def ppg_handle: 0x%lx "%(self.port0_def_ppg_handle))

            print("Ingress port0 non_def_ppg handle 0x%lx"%(self.port0_ppg_handle))

            # send packet to non-default ppg and check stats
            for i in range(pkt_count):
                pkt = simple_tcp_packet(
                      eth_dst='00:77:66:55:44:33',
                      eth_src='00:22:22:22:22:22',
                      ip_dst='172.20.10.1',
                      ip_src='192.168.0.1',
                      ip_id=105,
                      ip_tos=36, # dscp 9
                      ip_ttl=64)
                send_packet(self, self.devports[0], pkt)
            time.sleep(5)
            self.assertTrue(self.verifyPpgStats(self.port0_ppg_handle, check_stats_count),
                 "Verify PpgStats failed for non_def ppg_handle: 0x%lx"%(self.port0_ppg_handle))

        finally:
            pass

    def PortQueueStatTest(self):
        print("PortQueueStatTest()")
        try:
            pkt_count = 10
            queue_0_handle = self.qid_to_handle(self.port1, self.def_ppg_qid_map[0])
            self.assertTrue((queue_0_handle != 0),
                           "Invalid qid: %d , failed to get queue_handle 0x%lx"%(self.def_ppg_qid_map[0], queue_0_handle))

            queue_4_handle = self.qid_to_handle(self.port1, self.non_def_ppg_qid_map[0])
            self.assertTrue((queue_4_handle != 0),
                           "Invalid qid: %d, failed to get queue_handle "%(self.non_def_ppg_qid_map[0]))

            self.client.object_counters_clear_all(queue_0_handle)
            self.client.object_counters_clear_all(queue_4_handle)

            # send packet to default ppg and check stats
            for i in range(pkt_count):
                pkt = simple_tcp_packet(
                      eth_dst='00:77:66:55:44:33',
                      eth_src='00:22:22:22:22:22',
                      ip_dst='172.20.10.1',
                      ip_src='192.168.0.1',
                      ip_id=105,
                      ip_tos=4, # dscp 1
                      ip_ttl=64)
                send_packet(self, self.devports[0], pkt)
            time.sleep(5)
            self.assertTrue(self.verifyQueueStats(queue_0_handle, SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS,
                            expected_pkt_count=pkt_count),
                           "Verify QueueStats failed for qid: %d qid_handle: 0x%lx "%(self.def_ppg_qid_map[0], queue_0_handle))

            # send packet to non-default ppg and check stats
            for i in range(pkt_count):
                pkt = simple_tcp_packet(
                      eth_dst='00:77:66:55:44:33',
                      eth_src='00:22:22:22:22:22',
                      ip_dst='172.20.10.1',
                      ip_src='192.168.0.1',
                      ip_id=105,
                      ip_tos=36, # dscp 9
                      ip_ttl=64)
                send_packet(self, self.devports[0], pkt)
            time.sleep(5)
            self.assertTrue(self.verifyQueueStats(queue_4_handle, SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS,
                            expected_pkt_count=pkt_count),
                           "Verify QueueStats failed for qid: %d queue_handle: 0x%lx "%(self.non_def_ppg_qid_map[0], queue_4_handle))

        finally:
            pass

###############################################################################

@group('qos_hw')
@disabled
class QueueBufferHwProgramTest(ApiHelper):
    '''
    This test verifies queue buffer programming in hardware
    '''
    non_def_qid = 4

    def createQueueBufferPool(self, egress_pool_size, mode):
        # create egress buffer pool profile.
        egress_buffer_pool_handle = self.add_buffer_pool(self.device,
                          direction=SWITCH_BUFFER_POOL_ATTR_DIRECTION_EGRESS,
                          threshold_mode=mode,
                          pool_size=egress_pool_size)
        return egress_buffer_pool_handle

    def createQueueBufferProfile(self, egress_buffer_pool_handle, mode, b_size, th):
        # create egress buffer profile.
        egress_buffer_profile_handle = self.add_buffer_profile(self.device,
                             threshold_mode=mode,
                             threshold=th,
                             buffer_size=b_size,
                             buffer_pool_handle=egress_buffer_pool_handle)

        return egress_buffer_profile_handle

    def verifyQueueStats(self, queue_handle, queue_cntr_type, expected_pkt_count=0, expected_bytes_count=0):
        queue_cntrs = self.object_counters_get(queue_handle)
        for cntr in queue_cntrs:
            if cntr.counter_id == queue_cntr_type:
                if cntr.counter_id == SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS:
                    print("Received : %d, expected: %d"%(cntr.count, expected_pkt_count))
                    return (cntr.count == expected_pkt_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_STAT_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_DROPPED_PACKETS:
                    print("Received : %d, expected: %d"%(cntr.count, expected_pkt_count))
                    return (cntr.count == expected_pkt_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_CURR_OCCUPANCY_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_WATERMARK_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                elif cntr.counter_id ==  SWITCH_QUEUE_COUNTER_ID_MAX_OCCUPANCY_BYTES:
                    print("Received : %d, expected: %d"%(cntr.count, expected_bytes_count))
                    return (cntr.count == expected_bytes_count)
                else:
                    print("Invalid queue counter_id: %d" % cntr.counter_id)
                    return 0

    def qid_to_handle(self, port_handle, qid):
        queue_list = self.attribute_get(port_handle, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        for queue_handle in queue_list:
            queue_id = self.attribute_get(queue_handle.oid, SWITCH_QUEUE_ATTR_QUEUE_ID)
            if qid == queue_id:
                return queue_handle.oid

        print("Invalid queue id: %d, queue_handle does not exists" % qid)
        return 0

    def setUp(self):
        self.configure()
        print()

        # create egress buffer profile and attach to lossless queue 4
        egress_lossless_pool_size = 100300
        if self.arch == 'tofino':
            self.exp_egress_lossless_pool_size = 100320
        elif self.arch == 'tofino2':
            self.exp_egress_lossless_pool_size = 100320
        else:
            self.exp_egress_lossless_pool_size = egress_lossless_pool_size
        self.queue_lossless_g_limit = 1024
        if self.arch == 'tofino':
            self.exp_queue_lossless_g_limit = 1040
        elif self.arch == 'tofino2':
            self.exp_queue_lossless_g_limit = 1056
        else:
            self.exp_queue_lossless_g_limit = self.queue_lossless_g_limit
        queue_lossless_buffer_threshold = 192
        queue_static_buffer_threshold = 2048
        if self.arch == 'tofino':
            self.exp_static_buffer_threshold = 2080
        elif self.arch == 'tofino2':
            self.exp_static_buffer_threshold = 2112
        else:
            self.exp_static_buffer_threshold = queue_static_buffer_threshold

        egress_queue_buffer_pool_handle = self.createQueueBufferPool(
            egress_lossless_pool_size,
            SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE_DYNAMIC)

        self.egress_queue_buffer_profile_handle = self.createQueueBufferProfile(
            egress_queue_buffer_pool_handle,
            SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
            self.queue_lossless_g_limit,
            queue_lossless_buffer_threshold)

        self.static_queue_buffer_profile_handle = self.createQueueBufferProfile(
            egress_queue_buffer_pool_handle,
            SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_STATIC,
            0,
            queue_static_buffer_threshold)

        queue_4_handle = self.qid_to_handle(self.port1,
                                            self.non_def_qid)
        self.attribute_set(queue_4_handle,
                           SWITCH_QUEUE_ATTR_BUFFER_PROFILE_HANDLE,
                           self.egress_queue_buffer_profile_handle)

    def runTest(self):
        try:
            self.PortQueueMaxOccupancyTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def PortQueueMaxOccupancyTest(self):
        print("PortQueueMaxOccupancyTest()")
        try:
            queue_4_handle = self.qid_to_handle(self.port1, self.non_def_qid)
            self.assertTrue((queue_4_handle != 0),
                           "Invalid qid: %d, failed to get queue_handle "%(self.non_def_qid))

            # check queue 4 max occupancy
            # threshold value 192 -> baf 6 -> 50%
            exp_max_occupancy = (self.exp_egress_lossless_pool_size / 2) + self.exp_queue_lossless_g_limit
            self.assertTrue(self.verifyQueueStats(queue_4_handle,
                            SWITCH_QUEUE_COUNTER_ID_MAX_OCCUPANCY_BYTES,
                            expected_bytes_count=exp_max_occupancy),
                           "Verify QueueStats failed for qid: %d queue_handle: 0x%lx "%(self.non_def_qid, queue_4_handle))

            self.attribute_set(queue_4_handle,
                               SWITCH_QUEUE_ATTR_BUFFER_PROFILE_HANDLE,
                               self.static_queue_buffer_profile_handle)

            # check queue 4 max occupancy
            self.assertTrue(self.verifyQueueStats(queue_4_handle,
                            SWITCH_QUEUE_COUNTER_ID_MAX_OCCUPANCY_BYTES,
                            expected_bytes_count=self.exp_static_buffer_threshold),
                           "Verify QueueStats failed for qid: %d queue_handle: 0x%lx "%(self.non_def_qid, queue_4_handle))

            # Modify buffer profile buffer size
            print("Verify q max occupancy after attribute set of buffer_size")
            self.attribute_set(self.static_queue_buffer_profile_handle,
                               SWITCH_BUFFER_PROFILE_ATTR_BUFFER_SIZE,
                               self.queue_lossless_g_limit)
            # check queue 4 max occupancy
            self.assertTrue(self.verifyQueueStats(queue_4_handle,
                            SWITCH_QUEUE_COUNTER_ID_MAX_OCCUPANCY_BYTES,
                            expected_bytes_count=(
                                self.exp_static_buffer_threshold +
                                self.exp_queue_lossless_g_limit)),
                           "Verify QueueStats failed for qid: %d queue_handle: 0x%lx "%(self.non_def_qid, queue_4_handle))

        finally:
            pass

###############################################################################

@group('qos_hw')
@disabled
class QosSystemTest(ApiHelper):
    '''
    This test verify the qos-maps/queueing/scheduling/shaping with traffic.
    this wil  be used to configure the switch and traffic will be sent from ixia
    to verify the functionality.
    '''
    appl1_high_dscp = 34
    appl1_high_tc = 5
    appl1_high_icos = 5
    appl1_high_qid = 5
    appl1_high_ppg = 3
    appl1_high_color = SWITCH_QOS_MAP_ATTR_COLOR_GREEN


    appl2_normal_dscp = 35
    appl2_normal_tc = 6
    appl2_normal_icos = 6
    appl2_normal_qid = 6
    appl2_normal_ppg = 2
    appl2_normal_color = SWITCH_QOS_MAP_ATTR_COLOR_GREEN

    appl3_normal_dscp = 35
    appl3_normal_tc = 6
    appl3_normal_icos = 6
    appl3_normal_qid = 6
    appl3_normal_ppg = 2
    appl3_normal_color = SWITCH_QOS_MAP_ATTR_COLOR_GREEN

    appl4_lossless_dscp = 36
    appl4_lossless_tc = 4
    appl4_lossless_icos = 3
    appl4_lossless_qid = 4
    appl4_lossless_ppg = 2
    appl4_lossless_color = SWITCH_QOS_MAP_ATTR_COLOR_GREEN
    pfc_enable_map = 0x8

    # strict schedule profile
    # pps - packets
    appl1_high_min_rate_pps = 100
    appl1_high_max_rate_pps = 1000
    # bps - bits.
    appl1_high_min_rate_bps = 80000000
    appl1_high_max_rate_bps = 4000000000
    # bytes
    appl1_high_min_burst = 1024
    appl1_high_max_burst = 1024

    # DWRR schedule profile
    appl_normal_min_rate = 80000000
    appl_normal_max_rate = 4000000000
    appl_normal_min_burst = 1024
    appl_normal_max_burst = 1024
    appl_normal_weight = 100

    # egress port-2
    port_2_max_rate = 4000000000
    port_2_max_burst = 1024

    # Set ingress lossless pool as 140,000
    ingress_lossless_pool_size = 100000


    #ppg and buffer settings
    lossless_ppg = 2
    ppg_lossless_g_limit = 1024
    ppg_lossless_index = lossless_ppg
    ppg_lossless_buffer_mode = SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC
    ppg_lossless_buffer_threshold = 128
    ppg_skid_limit = 128
    # xon threshold is TM hysterisys
    ppg_lossless_xon_threshold = 512

    # xoff threshold is TM PPG skid limit
    ppg_lossless_xoff_threshold = 2048


    # Set egress lossless pool as 140,000
    egress_lossless_pool_size = 10000

    #lossless  settings
    queue_lossless_buffer_size = 1024
    queue_lossless_buffer_mode = SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC
    queue_lossless_buffer_threshold = 128

    def createDscpToTcColorQosMap(self):
        print("Configuring DscpToTc maps")
        qos_map1 = self.add_qos_map(self.device, dscp=self.appl1_high_dscp, tc=self.appl1_high_tc)
        qos_map2 = self.add_qos_map(self.device, dscp=self.appl2_normal_dscp, tc=self.appl2_normal_tc)
        qos_map3 = self.add_qos_map(self.device, dscp=self.appl4_lossless_dscp, tc=self.appl4_lossless_tc)
        dscp_tc_list = [qos_map1, qos_map2, qos_map3]
        dscp_tc_maps = []
        for qos_map in dscp_tc_list:
            dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
              type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)
        return dscp_tc_map_ingress

    def createTcToDscpQosMap(self):
        print("Configuring TcToDscp maps")
        qos_map1 = self.add_qos_map(self.device, tc=self.appl1_high_tc, dscp=self.appl1_high_dscp)
        qos_map2 = self.add_qos_map(self.device, tc=self.appl2_normal_tc, dscp=self.appl2_normal_dscp)
        qos_map3 = self.add_qos_map(self.device, tc=self.appl4_lossless_tc, dscp=self.appl4_lossless_dscp)
        tc_dscp_list = [qos_map1, qos_map2, qos_map3]
        tc_dscp_maps = []
        for qos_map in tc_dscp_list:
            tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        tc_dscp_map_egress = self.add_qos_map_egress(self.device,
              type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)
        return tc_dscp_map_egress

    def createTcToQueueIcosMap(self):
        print("Configuring TcToQueue maps")
        qos_map1 = self.add_qos_map(self.device, tc=self.appl1_high_tc, qid=self.appl1_high_qid, icos=self.appl1_high_icos)
        qos_map2 = self.add_qos_map(self.device, tc=self.appl2_normal_tc, qid=self.appl2_normal_qid, icos=self.appl2_normal_icos)
        qos_map3 = self.add_qos_map(self.device, tc=self.appl4_lossless_tc, qid=self.appl4_lossless_qid, icos=self.appl4_lossless_icos)
        tc_queue_icos_list = [qos_map1, qos_map2, qos_map3]
        tc_queue_icos_maps = []
        for qos_map in tc_queue_icos_list:
            tc_queue_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        tc_queue_icos_maps_ingress = self.add_qos_map_ingress(self.device,
              type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS_AND_QUEUE, qos_map_list=tc_queue_icos_maps)
        return tc_queue_icos_maps_ingress

    def configureIcosToPPGMaps(self, icos_map, ppg_index):
        print("Configuring icos-ppg maps")
        icos_ppg_maps = []
        qos_map1 = self.add_qos_map(self.device, icos=icos_map, ppg=ppg_index)
        icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        icos_to_ppg_map_ingress = self.add_qos_map_ingress(self.device,
              type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)
        return icos_to_ppg_map_ingress

    def configurePfcPriorityToQueueMaps(self, pfc_priority_map, queue_id):
        print("Configuring pfc_priority to queue maps")
        qos_map1 = self.add_qos_map(self.device, pfc_priority=pfc_priority_map, qid=queue_id)
        pfc_priority_queue_maps = []
        pfc_priority_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        pfc_priority_queue_map_egress = self.add_qos_map_egress(self.device,
              type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_PFC_PRIORITY_TO_QUEUE, qos_map_list=pfc_priority_queue_maps)
        return pfc_priority_queue_map_egress

    def attachDscpQosMap(self, port_handle, map_handle):
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, map_handle)

    def attachTcQueueQosmap(self, port_handle, tc_handle):
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, tc_handle)

    def attachTcDscpQosMap(self, port_handle, tc_dscp_qos_map):
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, tc_dscp_qos_map)

    def createStrictSchrProfile(self, min_rate, min_burst, max_rate, max_burst, pps):
        shaper_type = SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS
        if pps == True:
            shaper_type = SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_PPS

        strict_scheduler_handle = self.add_scheduler(self.device, type=SWITCH_SCHEDULER_ATTR_TYPE_STRICT,
                                shaper_type=shaper_type,
                                min_rate=min_rate,
                                min_burst_size=min_burst, max_rate=max_rate,
                                max_burst_size=max_burst)

        return strict_scheduler_handle

    def createDwrrSchShaperProfile(self, weight, min_rate,min_burst, max_rate, max_burst):
        # create DWRR scheduler profile
        dwrr_scheduler_handle = self.add_scheduler(self.device, type=SWITCH_SCHEDULER_ATTR_TYPE_DWRR,
                                   shaper_type=SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_BPS,
                                   weight=weight, min_rate=min_rate,
                                   min_burst_size=min_burst, max_rate=max_rate,
                                   max_burst_size=max_burst)
        return dwrr_scheduler_handle

    def createPortShaperProfile(self):
        pass
        return

    def attachPortShaperProfile(self, port_handle, shaper_profile_handle):
        # configure shaper on port
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_SCHEDULER_HANDLE,
                           shaper_profile_handle)

    def attachQueueSchProfile(self, port_handle, queue_id, sch_profile_handle):
        sch_group_list = self.attribute_get(port_handle,
                           SWITCH_PORT_ATTR_PORT_QUEUE_SCHEDULER_GROUP_HANDLES)
        for sch_group_handle in sch_group_list:
            print("Scheduler group handle 0x%lx"%(sch_group_handle.oid))
            # print queue and port handle
            group_type = self.attribute_get(sch_group_handle.oid,
                                               SWITCH_SCHEDULER_GROUP_ATTR_TYPE)
            if group_type == SWITCH_SCHEDULER_GROUP_ATTR_TYPE_QUEUE:
                queue_handle = self.attribute_get(sch_group_handle.oid,
                                                 SWITCH_SCHEDULER_GROUP_ATTR_QUEUE_HANDLE)
                qid = self.attribute_get(queue_handle,
                                         SWITCH_QUEUE_ATTR_QUEUE_ID)
                if qid == queue_id:
                    print("Attach sch profile to sch group handle:  0x%lx"%(sch_group_handle.oid))
                    self.attribute_set(sch_group_handle.oid, SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                                       sch_profile_handle)
                    return

        # failed to match qid to sch group.
        self.assertTrue(False, "Failed to match qid: %d to sch group associated with port_handle: 0x%lx "%\
                        (queue_id, port_handle))

    def createIngressLosslessPool(self, ingress_pool_size, mode, b_size, th):
        # create ingress buffer pool profile.
        ingres_buffer_pool_handle  = self.add_buffer_pool(self.device,
                          direction=SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS,
                          threshold_mode=mode,
                          pool_size=ingress_pool_size)

        # create ingress buffer profile.
        ingress_buffer_profile_handle = self.add_buffer_profile(self.device,
                             threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
                             threshold=th,
                             buffer_size=b_size,
                             xon_threshold = self.ppg_lossless_xon_threshold,
                             xoff_threshold = self.ppg_lossless_xoff_threshold,
                             buffer_pool_handle=ingres_buffer_pool_handle)

        return ingress_buffer_profile_handle

    def createEgressLosslessPool(self, egress_pool_size, mode, b_size, th):
        # create egress buffer pool profile.
        egress_buffer_pool_handle  = self.add_buffer_pool(self.device,
                          direction=SWITCH_BUFFER_POOL_ATTR_DIRECTION_EGRESS,
                          threshold_mode=mode,
                          pool_size=egress_pool_size)

        # create egress buffer profile.
        egress_buffer_profile_handle = self.add_buffer_profile(self.device,
                             threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
                             threshold=th,
                             buffer_size=b_size,
                             buffer_pool_handle=egress_buffer_pool_handle)

        return egress_buffer_profile_handle

    def createLossLessPPG(self, port_handle, ppg_index, ingress_buffer_profile_handle):
        ingress_ppg_handle = self.add_port_priority_group(self.device,
                   buffer_profile_handle=ingress_buffer_profile_handle,
                   ppg_index =  ppg_index,
                   port_handle = port_handle)
        return ingress_ppg_handle

    def attachIcosPpgQosmap(self, port_handle, ingress_qos_map_handle):
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, ingress_qos_map_handle)

    def attachPfcPriorityQueueQosmap(self, port_handle, pfc_priority_qos_map_handle):
        self.attribute_set(port_handle, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,
                             pfc_priority_qos_map_handle)

    def configureEgressLossLessQueue(self, port_handle, queue_id, egress_buffer_profile_handle):
        queue_list = self.attribute_get(self.port0,
                           SWITCH_PORT_ATTR_QUEUE_HANDLES)
        for queue_handle in queue_list:
            qid = self.attribute_get(queue_handle.oid,
                                     SWITCH_QUEUE_ATTR_QUEUE_ID)
            if qid == queue_id:
                print("Attaching buffer_profile  - Queue handle 0x%lx from port 0x%lx, Egress buffer prof 0x%lx"\
                    %(queue_handle.oid, port_handle, egress_buffer_profile_handle))
                self.attribute_set(queue_handle.oid, SWITCH_QUEUE_ATTR_BUFFER_PROFILE_HANDLE,
                                   egress_buffer_profile_handle)
    def switch_config(self):
        # L2 configuration

        port_list = []
        device = 0
        self.devports = []
        self.device = self.get_device_handle(device)
        self.cpu_port_hdl = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_CPU_PORT)
        self.cpu_port = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_DEV_PORT)
        self.vlan10 = self.add_vlan(self.device, vlan_id=10)
        self.vlan20 = self.add_vlan(self.device, vlan_id=20)
        self.vlan30 = self.add_vlan(self.device, vlan_id=30)
        self.vlan40 = self.add_vlan(self.device, vlan_id=40)

        # configure port0 and port4 with 40G speed
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([swports[0]]), speed=40000)
        port_list.append(self.port0)
        dev_port = self.attribute_get(self.port0, SWITCH_PORT_ATTR_DEV_PORT)
        self.devports.append(dev_port)

        self.port4 = self.add_port(self.device, lane_list=u.lane_list_t([swports[4]]), speed=40000)
        port_list.append(self.port4)
        dev_port = self.attribute_get(self.port4, SWITCH_PORT_ATTR_DEV_PORT)
        self.devports.append(dev_port)

        self.port8 = self.add_port(self.device, lane_list=u.lane_list_t([swports[8]]), speed=40000)
        port_list.append(self.port8)
        dev_port = self.attribute_get(self.port8, SWITCH_PORT_ATTR_DEV_PORT)
        self.devports.append(dev_port)

        self.port12 = self.add_port(self.device, lane_list=u.lane_list_t([swports[12]]), speed=40000)
        port_list.append(self.port12)
        dev_port = self.attribute_get(self.port12, SWITCH_PORT_ATTR_DEV_PORT)
        self.devports.append(dev_port)


        self.wait_for_interface_up(port_list)

        #L3 configuration
        self.rmac = '00:77:66:55:44:33'
        self.vrf10 = self.add_vrf(self.device, id=10, src_mac=self.rmac)

    def runTest(self):
        print()
        self.switch_config()

        dscp_test_ports = [self.port0, self.port4]
        #pcp_test_ports = [self.port2, self.port3]

        # configure port0 and port4 for qos dscp testing on layer-3 port
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        # configure port8 and port12 for pcp testing
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port8, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port12, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.port8)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port12)

        # create ingress DSCP to TC and color map
        dscp_map_handle = self.createDscpToTcColorQosMap()

        # create ingress tc to queue and icos map
        tc_queueu_handle = self.createTcToQueueIcosMap()

        # create egress tc to dscp map
        tc_dscp_map_handle = self.createTcToDscpQosMap()

        # create strict scheduler profile mode pps.
        strict_sch_handle = self.createStrictSchrProfile(self.appl1_high_min_rate_pps, self.appl1_high_min_burst,
                                                         self.appl1_high_max_rate_pps, self.appl1_high_max_burst, True)
        # create DWRR scheduler profile.
        dwrr_sch_handle_1 = self.createDwrrSchShaperProfile(self.appl_normal_weight, self.appl_normal_min_rate,
                                                         self.appl_normal_min_burst,
                                                         self.appl_normal_max_rate, self.appl_normal_max_burst)
        dwrr_sch_handle_2 = self.createDwrrSchShaperProfile(self.appl_normal_weight, self.appl_normal_min_rate,
                                                         self.appl_normal_min_burst,
                                                         0, 0)
        # create ingress and egress lossless buffer profile.
        ingress_loss_less_buffer_handle =  \
                     self.createIngressLosslessPool(self.ingress_lossless_pool_size,
                                                    self.ppg_lossless_buffer_mode,
                                                    self.ppg_lossless_g_limit,
                                                    self.ppg_lossless_buffer_threshold)
        egress_loss_less_buffer_handle = \
                     self.createEgressLosslessPool(self.egress_lossless_pool_size,
                                                   self.queue_lossless_buffer_mode, 0,
                                                   self.queue_lossless_buffer_threshold)

        # create icos to ppg mapping
        icos_ppg_map_handle = self.configureIcosToPPGMaps(self.appl4_lossless_icos, self.lossless_ppg)

        # create pfc-priority to queue mapping.
        pfc_prio_queue_map_handle = \
            self.configurePfcPriorityToQueueMaps(self.appl4_lossless_icos, self.appl4_lossless_qid)

        for port in dscp_test_ports:
            # Attach DSCP to TC/Color profile to all the ports
            self.attachDscpQosMap(port,dscp_map_handle)

            # Attach TC to Queue/iCos profile to all the ports.
            self.attachTcDscpQosMap(port, tc_dscp_map_handle)

            # todo: Attach WRED profile.

            # configure queues.
            self.attachQueueSchProfile(port, self.appl1_high_qid, strict_sch_handle)
            self.attachQueueSchProfile(port, self.appl2_normal_qid, dwrr_sch_handle_1)
            self.attachQueueSchProfile(port, self.appl3_normal_qid, dwrr_sch_handle_1)
            self.attachQueueSchProfile(port, self.appl4_lossless_qid, dwrr_sch_handle_2)

            # enable PFC flow control
            self.attribute_set(port, SWITCH_PORT_ATTR_FLOW_CONTROL, SWITCH_PORT_ATTR_FLOW_CONTROL_PFC)
            self.attribute_set(port, SWITCH_PORT_ATTR_PFC_MAP, self.pfc_enable_map)

            # Configure lossless PPG for appl4 traffic.
            lossless_ppg_handle = self.createLossLessPPG(port,
                                                        self.lossless_ppg,
                                                        ingress_loss_less_buffer_handle)

            # Attach icos to ppg mapping to port
            self.attachIcosPpgQosmap(port, icos_ppg_map_handle)

            # Attach pfc-priority to queue mapping
            self.attachPfcPriorityQueueQosmap(port, pfc_prio_queue_map_handle)

            # configure egress lossless queue
            self.configureEgressLossLessQueue(port, self.appl4_lossless_qid,
                                         egress_loss_less_buffer_handle)
        print("configuration successfull")
        return

###############################################################################
@group('qos')
class QoSTest(ApiHelper):
    def runTest(self):
        print()

        self.configure()

        # configure port0 and port1 for dscp/tos testing
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.port3)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port2)

        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port4, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port5, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:55:55:55:55:55', destination_handle=self.port5)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:66:66:66:66:66', destination_handle=self.port4)

        # Create lag0 with port6 and part of vlan 20
        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6)
        lag_vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag0, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:77:77:77:77:77', destination_handle=self.lag0)

        # Create lag1 with port7, L3 interface
        self.lag1 = self.add_lag(self.device)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port7)
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag1, vrf_handle=self.vrf10, src_mac=self.rmac)

        try:
            self.NoQosTest()
            self.DefaultTcColorTest()
            if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) != 0):
                self.configureTrafficClass()
                self.configurePcpMaps()

                self.configureDscpMaps()
                self.enablePcpMapsOnInPortLag(True)
                self.PcpRewriteTest()
                self.DscpRewriteTest(True)
                self.PcpRewriteTest()
                self.enablePcpMapsOnInPortLag(False)
                self.DscpRewriteTest(False)

                #TODO: Need to verify TOS tests (limit to ingres TOS_TO_ cases)
                if (0):
                    self.configureTosMaps()
                    self.enablePcpMapsOnInPortLag(True)
                    self.PcpRewriteTest()
                    self.TosRewriteTest()
                    self.PcpRewriteTest()
                    self.enablePcpMapsOnInPortLag(False)
                    self.TosRewriteTest()

        finally:
            self.cleanup()

    def DefaultTcColorTest(self):
        print("===== Test default Tc Color with traffic =====")
        qos_map1 = self.add_qos_map(self.device, tc=20, dscp=9)
        tc_dscp_maps = []
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        tc_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)
        qos_map2 = self.add_qos_map(self.device, tc=20, color=SWITCH_QOS_MAP_ATTR_COLOR_RED, qid=4)
        tc_color_queue_maps = []
        tc_color_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        tc_color_qid_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_color_queue_maps)
        queue_handles = self.attribute_get(self.port1, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        queue_4_handle = queue_handles[4].oid
        pre_cntrs = self.object_counters_get(queue_4_handle)

        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='172.20.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_tos=4,
            ip_ttl=64)

        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='172.20.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_tos=4,
            ip_ttl=63)

        try:
            print("  pass packet w/ mapped dscp value 1 -> 1, no traffic class configured")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            send_packet(self, self.devports[7], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

            print("  pass packet w/ mapped dscp value 1 -> 9, traffic class configured")
            # traffic class is a miss
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC, 20)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, tc_dscp_map_egress)
            exp_pkt[IP].tos = 36
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

            #TODO: Excluded LAG tests and partial egress mapping config 
            if (0):
                self.attribute_set(self.lag1, SWITCH_LAG_ATTR_TC, 20)
                send_packet(self, self.devports[7], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

                # now hit the traffic class table and update queue id
                self.attribute_set(self.port0, SWITCH_PORT_ATTR_COLOR, SWITCH_QOS_MAP_ATTR_COLOR_RED)
                for i in range(0,10):
                    send_packet(self, self.devports[0], pkt)
                    verify_packet(self, exp_pkt, self.devports[1])
                post_cntrs = self.object_counters_get(queue_4_handle)
                c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
                self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 10)

                self.attribute_set(self.lag1, SWITCH_LAG_ATTR_COLOR, SWITCH_QOS_MAP_ATTR_COLOR_RED)
                for i in range(0,10):
                    send_packet(self, self.devports[7], pkt)
                    time.sleep(2)
                    verify_packet(self, exp_pkt, self.devports[1])
                post_cntrs = self.object_counters_get(queue_4_handle)
                c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
                self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 20)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_COLOR, SWITCH_QOS_MAP_ATTR_COLOR_GREEN)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_COLOR, SWITCH_QOS_MAP_ATTR_COLOR_GREEN)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC, 0)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_TC, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def configureDscpMaps(self):
        print("===== Configuring DSCP/TC ingress/egress maps =====")
        qos_map1 = self.add_qos_map(self.device, dscp=1, tc=20)
        qos_map2 = self.add_qos_map(self.device, dscp=2, tc=24)
        qos_map3 = self.add_qos_map(self.device, dscp=3, tc=28)
        qos_map4 = self.add_qos_map(self.device, dscp=4, tc=30)
        self.dscp_tc_maps = []
        self.dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        self.dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        self.dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map3))
        self.dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map4))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=self.dscp_tc_maps)

        qos_map5 = self.add_qos_map(self.device, tc=20, dscp=9)
        qos_map6 = self.add_qos_map(self.device, tc=24, dscp=10)
        qos_map7 = self.add_qos_map(self.device, tc=28, dscp=11)
        qos_map8 = self.add_qos_map(self.device, tc=30, dscp=12)
        qos_map9 = self.add_qos_map(self.device, tc=25, dscp=14)
        self.tc_dscp_maps = []
        self.tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
        self.tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map6))
        self.tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map7))
        self.tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map8))
        self.tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map9))
        self.tc_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=self.tc_dscp_maps)

    def configureTosMaps(self):
        print("==================================================")
        print("===== Configuring TOS/TC ingress/egress maps =====")
        qos_map1 = self.add_qos_map(self.device, tos=7, tc=20)
        qos_map2 = self.add_qos_map(self.device, tos=11, tc=24)
        tos_tc_maps = []
        tos_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        tos_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        self.tos_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TOS_TO_TC, qos_map_list=tos_tc_maps)

        qos_map5 = self.add_qos_map(self.device, tc=20, tos=39)
        qos_map6 = self.add_qos_map(self.device, tc=24, tos=43)
        tc_tos_maps = []
        tc_tos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
        tc_tos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map6))
        self.tc_tos_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_TOS, qos_map_list=tc_tos_maps)

    def configureGlobalPcpMaps(self):
        print("Configuring Global PCP maps")
        qos_map1 = self.add_qos_map(self.device, pcp=1, tc=1)
        qos_map2 = self.add_qos_map(self.device, pcp=2, tc=2)
        pcp_tc_maps = []
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        self.pcp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC, qos_map_list=pcp_tc_maps, global_enable=True)

        qos_map5 = self.add_qos_map(self.device, tc=1, pcp=4)
        qos_map6 = self.add_qos_map(self.device, tc=2, pcp=5)
        tc_pcp_maps = []
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map6))
        self.tc_pcp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_PCP, qos_map_list=tc_pcp_maps)

    def CleanUpGlobalPcpMaps(self):
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()

    def configurePcpMaps(self):
        print("===== Configuring PCP/TC ingress/egress maps =====")
        qos_map1 = self.add_qos_map(self.device, pcp=1, tc=20)
        qos_map2 = self.add_qos_map(self.device, pcp=2, tc=24)
        qos_map3 = self.add_qos_map(self.device, pcp=5, tc=20)
        qos_map4 = self.add_qos_map(self.device, pcp=7, tc=25)
        pcp_tc_maps = []
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map3))
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map4))
        self.pcp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC, qos_map_list=pcp_tc_maps)

        qos_map5 = self.add_qos_map(self.device, tc=20, pcp=4)
        qos_map6 = self.add_qos_map(self.device, tc=24, pcp=5)
        qos_map7 = self.add_qos_map(self.device, tc=25, pcp=6)
        tc_pcp_maps = []
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map6))
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map7))
        self.tc_pcp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_PCP, qos_map_list=tc_pcp_maps)

    def configureTrafficClass(self):
        print("===== Configuring traffic class =====")
        '''
        Although 4 qos_maps are created, only 3 hardware entries are updated with
        one of them having action icos_and_queue
        '''
        qos_map1 = self.add_qos_map(self.device, tc=3, icos=3)
        qos_map2 = self.add_qos_map(self.device, tc=4, icos=4)
        tc_icos_maps = []
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        self.tc_icos_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=tc_icos_maps)
        self.assertEqual(self.status(), 0)
        qos_map3 = self.add_qos_map(self.device, tc=4, qid=4)
        qos_map4 = self.add_qos_map(self.device, tc=5, qid=5)
        tc_queue_maps = []
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map3))
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map4))
        self.tc_qid_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)
        self.assertEqual(self.status(), 0)

        # There should only be 2 entries after this
        qos_map5 = self.add_qos_map(self.device, tc=4, icos=5)
        tc_icos_maps = []
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
        self.attribute_set(self.tc_icos_map_ingress,
            SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, tc_icos_maps)
        self.attribute_set(self.tc_icos_map_ingress,
            SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])

        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()

        qos_map1 = self.add_qos_map(self.device, tc=3, icos=3, qid=3)
        qos_map2 = self.add_qos_map(self.device, tc=4, icos=4, qid=4)
        tc_icos_q_maps = []
        tc_icos_q_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        tc_icos_q_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        self.tc_icos_q_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS_AND_QUEUE,
            qos_map_list=tc_icos_q_maps)

        self.cleanlast()
        self.cleanlast()
        self.cleanlast()

    def NoQosTest(self):
        print("===== Test traffic with no Qos Maps =====")
        try:
            # send test packet before qos maps are configured)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=24,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=24,
                ip_ttl=63)

            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("  pass packet before dscp qos maps are configured")

            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=2,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=2,
                ip_ttl=64)

            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            print("  pass packet before pcp qos maps are configured")

        finally:
            pass

    def DscpRewriteTest(self, use_pcp=False):
        try:
            print("Turn on DSCP/TC ingress/egress maps on port/lag ")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            #  self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, self.dscp_tc_map_ingress)  # qos-map supp only on port
            #  self.attribute_set(self.lag1, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, self.dscp_tc_map_ingress)  # qos-map supp only on port

            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("===== Test DSCP/TC rewrite maps with traffic =====")
            print("  pass packet w/ mapped dscp value 1 -> 9  ")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=36, # dscp 9
                ip_ttl=63)
            exp_pkt[IP].tos = 36
            #  test packet from port 0
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

            '''
            # send test packet to lag 1
            send_packet(self, self.devports[7], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            '''

            print("  pass packet w/ mapped dscp value 3 -> 11")
            pkt[IP].tos = 12
            exp_pkt[IP].tos = 44
            #  test packet from port 0
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

            #  test packet from lag 1
            '''
            send_packet(self, self.devports[7], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            '''

            print("  pass packet w/ unmapped dscp value 6, no change ")
            pkt[IP].tos = 24
            exp_pkt[IP].tos = 24
            #  test packet from port 0
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

            # test packet to lag 1
            '''
            #send_packet(self, self.devports[7], pkt)
            #verify_packet(self, exp_pkt, self.devports[1])
            '''

            # update the ingress qos map list
            print(" Update ingress qos map with new tc")
            qos_map1 = self.add_qos_map(self.device, dscp=1, tc=30)
            qos_map2 = self.add_qos_map(self.device, dscp=2, tc=28)
            qos_map3 = self.add_qos_map(self.device, dscp=3, tc=24)
            qos_map4 = self.add_qos_map(self.device, dscp=4, tc=20)
            dscp_tc_maps = []
            dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
            dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
            dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map3))
            dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map4))
            self.attribute_set(self.dscp_tc_map_ingress,
                SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, dscp_tc_maps)

            print("  pass packet w/ mapped dscp value 1 -> 12")
            pkt[IP].tos = 4
            exp_pkt[IP].tos = 48
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

            # test packet to lag 1
            '''
            #send_packet(self, self.devports[7], pkt)
            #verify_packet(self, exp_pkt, self.devports[1])
            '''

            # update the egress qos map list
            print(" Update egress qos map with new dscp values")
            qos_map5 = self.add_qos_map(self.device, tc=20, dscp=12)
            qos_map6 = self.add_qos_map(self.device, tc=24, dscp=11)
            qos_map7 = self.add_qos_map(self.device, tc=28, dscp=10)
            qos_map8 = self.add_qos_map(self.device, tc=30, dscp=9)
            qos_map9 = self.add_qos_map(self.device, tc=25, dscp=15)
            tc_dscp_maps = []
            tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
            tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map6))
            tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map7))
            tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map8))
            tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map9))
            self.attribute_set(self.tc_dscp_map_egress,
                SWITCH_QOS_MAP_EGRESS_ATTR_QOS_MAP_LIST, tc_dscp_maps)
            print(" pass packet w/ mapped dscp value 1 -> 9 ")
            pkt[IP].tos = 4
            exp_pkt[IP].tos = 36
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

            # send test packet to lag 1
            '''
            send_packet(self, self.devports[7], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            '''

            if use_pcp == True:
                #TODO: Excluded LAG tasts
                if (0):
                    print("  pass v4 packet w/ unmapped dscp value 5, use pcp value 5 --> dscp value 12 ")
                    pkt = simple_tcp_packet(
                        eth_dst='00:77:77:77:77:77',
                        eth_src='00:66:66:66:66:66',
                        ip_dst='172.20.10.1',
                        ip_src='192.168.0.1',
                        dl_vlan_enable=True,
                        vlan_vid=20,
                        vlan_pcp=5,
                        ip_ttl=64)
                    exp_pkt = copy.deepcopy(pkt)
                    exp_pkt[IP].tos = 48
                    send_packet(self, self.devports[4], pkt)
                    verify_packet(self, exp_pkt, self.devports[6])

                    print("  pass v6 packet w/ unmapped dscp value, use pcp value 7 --> dscp value 15 ")
                    pkt = simple_tcpv6_packet(
                        eth_dst='00:00:00:00:00:11',
                        eth_src='00:22:22:22:22:22',
                        ipv6_dst='4000::2',
                        ipv6_src='2000::1',
                        ipv6_hlim=64,
                        dl_vlan_enable=True,
                        vlan_vid=20,
                        ipv6_tc=20,
                        vlan_pcp=7)
                    exp_pkt = copy.deepcopy(pkt)
                    exp_pkt[IPv6].tc = 60
                    send_packet(self, self.devports[4], pkt)
                    verify_packets(self, exp_pkt, [self.devports[5], self.devports[6]])

            print("  pass packet w/ mapped dscp value 2 -> 10, pcp stays unmodified at 6 ")
            pkt = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=8, # dscp 2
                dl_vlan_enable=True,
                vlan_vid=20,
                vlan_pcp=6,
                ip_ttl=64)
            pkt[IP].tos = 8
            exp_pkt = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=40, # dscp 10
                dl_vlan_enable=True,
                vlan_vid=20,
                vlan_pcp=6,
                ip_ttl=64)
            exp_pkt[IP].tos = 40
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[5])

            '''
            # send test packet to LAG
            send_packet(self, self.devports[6], pkt)
            '''
            # Flood the packet, ingress LAG
            pkt[Ether].dst='00:00:00:00:00:11'
            exp_pkt[Ether].dst='00:00:00:00:00:11'

            '''
            send_packet(self, self.devports[6], pkt)
            verify_packets(self, exp_pkt, [self.devports[5], self.devports[4]])
            '''
            # Flood the packet, ingress port and egress port+lag
            # TODO: Excluded LAG tests
            if (0):
                send_packet(self, self.devports[4], pkt)
                verify_packets(self, exp_pkt, [self.devports[5], self.devports[6]])

                print("  pass v6 packet w/ with mapped dscp value 2 --> 10 ")
                # validate with IPV6 packet
                pkt = simple_tcpv6_packet(
                    eth_dst='00:00:00:00:00:11',
                    eth_src='00:22:22:22:22:22',
                    ipv6_dst='4000::2',
                    ipv6_src='2000::1',
                    ipv6_hlim=64,
                    dl_vlan_enable=True,
                    vlan_vid=20,
                    ipv6_tc=8)
                exp_pkt = copy.deepcopy(pkt)
                exp_pkt[IPv6].tc = 40
                send_packet(self, self.devports[4], pkt)
                verify_packets(self, exp_pkt, [self.devports[5], self.devports[6]])

            if ptf.testutils.test_params_get()["target"] != "hw":
                #  CPU packet with bypass all
                print("  sending packet from CPU port")
                pkt = simple_tcp_packet(
                    eth_dst='00:11:11:11:11:11',
                    eth_src='00:00:00:00:00:01',
                    ip_dst='172.17.10.1',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_ttl=64,dl_vlan_enable=True, vlan_vid=10)

                cpu_pkt = simple_cpu_packet(
                    dst_device=0,
                    ingress_ifindex=0xFFFF & self.port2,
                    ingress_port=0,
                    ingress_bd=0,
                    tx_bypass=True,
                    reason_code=0xFFFF,
                    inner_pkt=pkt)
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packet(self, pkt, self.devports[2])

            print("  Verify mirrored packets are egressed without rewrite")
            mirror = self.add_mirror(self.device,
                type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
                egress_port_handle=self.port8)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)

            pkt = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=8, # dscp 2
                dl_vlan_enable=True,
                vlan_vid=20,
                vlan_pcp=6,
                ip_ttl=64)

            #  test packet to validate ingress port mirror
            exp_pkt = copy.deepcopy(pkt)
            exp_pkt[IP].tos = 40
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, pkt, self.devports[8])

        finally:
            print("Turn off DSCP/TC ingress/egress maps on port/lag ")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)

            #  self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, 0) # qos-map supp only on port
            #  self.attribute_set(self.lag1, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, 0) # qos-map supp only on port

            self.attribute_set(self.dscp_tc_map_ingress,
                SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, self.dscp_tc_maps)
            self.attribute_set(self.tc_dscp_map_egress,
                SWITCH_QOS_MAP_EGRESS_ATTR_QOS_MAP_LIST, self.tc_dscp_maps)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            pass

    def enablePcpMapsOnInPortLag(self, enable):
        if enable == True:
            print("Turn on PCP-->TC ingress qos-maps on port/lag ")
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, self.pcp_tc_map_ingress)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, self.pcp_tc_map_ingress)
            # self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, self.pcp_tc_map_ingress)    # qos-map supp only on port
        else:
            print("Turn off PCP-->TC ingress maps on port/lag ")
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)
            # self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, 0)    # qos-map supp only on port

    def PcpRewriteTest(self):
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, self.tc_pcp_map_egress)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, self.tc_pcp_map_egress)

        try:
            print("===== Test PCP/TC rewrite maps with traffic =====")
            # send test packet before qos maps are configured
            print("  Validate with IPV4 packet")
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=2,
                ip_ttl=64)
            exp_pkt = copy.deepcopy(pkt)
            exp_pkt[Dot1Q].prio = 5
            print("  PCP packet ingress on port -> port")
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])

            pkt = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:77:77:77:77:77',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                vlan_pcp=2,
                ip_ttl=64)
            exp_pkt = copy.deepcopy(pkt)
            exp_pkt[Dot1Q].prio = 5
            '''
            print("  PCP packet ingress on lag -> port")
            send_packet(self, self.devports[6], pkt)
            verify_packet(self, exp_pkt, self.devports[5])
            print("  pass packet w/ mapped pcp value 1 -> 4")
            '''

            print("  Validate with IPV6 packet")
            pkt = simple_tcpv6_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::2',
                ipv6_src='2000::1',
                ipv6_hlim=64,
                dl_vlan_enable=True,
                vlan_vid=10,
                ipv6_tc=8,
                vlan_pcp=2)
            exp_pkt = copy.deepcopy(pkt)
            exp_pkt[Dot1Q].prio = 5
            print("  PCP v6 packet ingress on port -> port")
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])

            pkt = simple_tcpv6_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:77:77:77:77:77',
                ipv6_dst='4000::2',
                ipv6_src='2000::1',
                ipv6_hlim=64,
                dl_vlan_enable=True,
                vlan_vid=20,
                ipv6_tc=8,
                vlan_pcp=2)
            exp_pkt = copy.deepcopy(pkt)
            exp_pkt[Dot1Q].prio = 5
            '''
            print("  PCP v6 packet ingress on lag -> port")
            send_packet(self, self.devports[6], pkt)
            verify_packet(self, exp_pkt, self.devports[5])
            '''
            #TODO: Excluded LAG tests
            if (0):
                print("  Validate with IPV4 packet with pcp-pcp 2 --> 5 ")
                pkt = simple_tcp_packet(
                    eth_dst='00:77:77:77:77:77',
                    eth_src='00:66:66:66:66:66',
                    ip_dst='172.20.10.1',
                    ip_src='192.168.0.1',
                    dl_vlan_enable=True,
                    vlan_vid=20,
                    vlan_pcp=2,
                    ip_ttl=64)
                exp_pkt = copy.deepcopy(pkt)
                exp_pkt[Dot1Q].prio = 5
                print("  PCP packet ingress on port -> lag")
                send_packet(self, self.devports[4], pkt)
                verify_packet(self, exp_pkt, self.devports[6])

                print("  Validate with IPV6 packet with pcp-pcp 2 --> 5 ")
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:77:77:77:77',
                    eth_src='00:66:66:66:66:66',
                    ipv6_dst='4000::2',
                    ipv6_src='2000::1',
                    ipv6_hlim=64,
                    dl_vlan_enable=True,
                    vlan_vid=20,
                    ipv6_tc=8,
                    vlan_pcp=2)
                exp_pkt = copy.deepcopy(pkt)
                exp_pkt[Dot1Q].prio = 5
                print("  PCP v6 packet ingress on port -> lag")
                send_packet(self, self.devports[4], pkt)
                verify_packet(self, exp_pkt, self.devports[6])

                print("  Validate with IPV4 packet with pcp-pcp 5 --> 4 ")
                pkt = simple_tcp_packet(
                    eth_dst='00:77:77:77:77:77',
                    eth_src='00:66:66:66:66:66',
                    ip_dst='172.20.10.1',
                    ip_src='192.168.0.1',
                    dl_vlan_enable=True,
                    vlan_vid=20,
                    vlan_pcp=5,
                    ip_ttl=64)
                exp_pkt = copy.deepcopy(pkt)
                exp_pkt[Dot1Q].prio = 4
                print("  PCP packet ingress on port -> lag")
                send_packet(self, self.devports[4], pkt)
                verify_packet(self, exp_pkt, self.devports[6])

                print("  Validate with IPV6 packet with pcp-pcp 7 --> 6 ")
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:77:77:77:77',
                    eth_src='00:66:66:66:66:66',
                    ipv6_dst='4000::2',
                    ipv6_src='2000::1',
                    ipv6_hlim=64,
                    dl_vlan_enable=True,
                    vlan_vid=20,
                    ipv6_tc=8,
                    vlan_pcp=7)
                exp_pkt = copy.deepcopy(pkt)
                exp_pkt[Dot1Q].prio = 6
                print("  PCP v6 packet ingress on port -> lag")
                send_packet(self, self.devports[4], pkt)
                verify_packet(self, exp_pkt, self.devports[6])

                if ptf.testutils.test_params_get()["target"] != "hw":
                    print("  sending packet from CPU port")
                    pkt = simple_tcp_packet(
                        eth_dst='00:33:33:33:33:33',
                        eth_src='00:00:00:00:00:01',
                        ip_dst='172.17.10.1',
                        ip_src='192.168.0.1',
                        ip_id=105,
                        ip_ttl=64,dl_vlan_enable=True, vlan_vid=10)

                    cpu_pkt = simple_cpu_packet(
                        dst_device=0,
                        ingress_ifindex=0xFFFF & self.port3,
                        ingress_port=0,
                        ingress_bd=0,
                        tx_bypass=True,
                        reason_code=0xFFFF,
                        inner_pkt=pkt)
                    send_packet(self, self.cpu_port, cpu_pkt)
                    verify_packet(self, pkt, self.devports[3])

            print("  Verify mirrored packets are egressed without rewrite")
            mirror = self.add_mirror(self.device,
                type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
                egress_port_handle=self.port8)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)

            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=2,
                ip_ttl=64)

            exp_pkt = copy.deepcopy(pkt)
            exp_pkt[Dot1Q].prio = 5

            send_packet(self, self.devports[2], pkt)
            verify_packet(self, pkt, self.devports[8])

        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            pass

    def GlobalPcpRewriteTest(self):
        print("Global PcpRewriteTest()")
        try:
            self.configureGlobalPcpMaps()
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, self.tc_pcp_map_egress)
           
            # send test packet before qos maps are configured
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=2,
                ip_ttl=64)
            send_packet(self, self.devports[2], pkt)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=5,
                ip_ttl=64)
            verify_packet(self, exp_pkt, self.devports[3])
            print("  pass packet w/ mapped global pcp value 1 -> 4")

        finally:
            #self.cleanlast()
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            self.CleanUpGlobalPcpMaps()
            pass


    def TosRewriteTest(self):
        try:
            print("Turn on TOS/TC ingress/egress maps on port/lag ")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.tos_tc_map_ingress)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.tos_tc_map_ingress)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.tos_tc_map_ingress)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.tos_tc_map_ingress)
            #  self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, self.tos_tc_map_ingress)   # qos-map supp only on port
            #  self.attribute_set(self.lag1, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, self.tos_tc_map_ingress)   # qos-map supp only on port

            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, self.tc_tos_map_egress)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, self.tc_tos_map_egress)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, self.tc_tos_map_egress)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, self.tc_tos_map_egress)
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, self.tc_tos_map_egress)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_EGRESS_QOS_GROUP, self.tc_tos_map_egress)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_QOS_GROUP, self.tc_tos_map_egress)

            print("===== Test TOS/TC rewrite maps with traffic =====")
            #  ToS[7 --> 39]
            #  ToS[11 --> 43]
            # send test packet before qos maps are configured
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=7,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=39,
                ip_ttl=63)
            exp_pkt[IP].tos = 39
            verify_packet(self, exp_pkt, self.devports[1])
            print("  pass packet w/ mapped tos value 7 -> 39")

            # send test packet to LAG
            '''
            send_packet(self, self.devports[7], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            '''

            # send test packet with unmapped dscp value
            print("  pass packet w/ unmapped ToS value 24")
            pkt[IP].tos = 24
            send_packet(self, self.devports[0], pkt)

            exp_pkt[IP].tos = 24
            verify_packet(self, exp_pkt, self.devports[1])

            # send test packet to LAG
            '''
            send_packet(self, self.devports[7], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            '''

            print("  Validate with IPV6 packet")
            # validate with IPV6 packet
            pkt = simple_tcpv6_packet(
                eth_dst='00:00:00:00:00:11',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::2',
                ipv6_src='2000::1',
                ipv6_hlim=64, dl_vlan_enable=True, vlan_vid=20, ipv6_tc=7)
            exp_pkt = copy.deepcopy(pkt)
            exp_pkt[IPv6].tc = 39
            send_packet(self, self.devports[4], pkt)
            verify_packets(self, exp_pkt, [self.devports[5], self.devports[6]])

            if ptf.testutils.test_params_get()["target"] != "hw":
                #CPU packet with bypass all
                print("  sending packet from CPU port")
                pkt = simple_tcp_packet(
                    eth_dst='00:11:11:11:11:11',
                    eth_src='00:00:00:00:00:01',
                    ip_dst='172.17.10.1',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_ttl=64,dl_vlan_enable=True, vlan_vid=10)

                cpu_pkt = simple_cpu_packet(
                    dst_device=0,
                    ingress_ifindex=0xFFFF & self.port2,
                    ingress_port=0,
                    ingress_bd=0,
                    tx_bypass=True,
                    reason_code=0xFFFF,
                    inner_pkt=pkt)
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packet(self, pkt, self.devports[2])

            print("  Verify mirrored packets are egressed without rewrite")
            mirror = self.add_mirror(self.device,
                type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
                egress_port_handle=self.port8)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)

            pkt = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=7,
                dl_vlan_enable=True,
                vlan_vid=20,
                vlan_pcp=6,
                ip_ttl=64)

            exp_pkt = copy.deepcopy(pkt)
            exp_pkt[IP].tos = 39
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[5])
            verify_packet(self, pkt, self.devports[8])

        finally:
            print("Turn off TOS/TC ingress/egress maps on port/lag ")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)

            self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, 0)
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, 0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_EGRESS_QOS_GROUP, 0)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_QOS_GROUP, 0)

            #  self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, 0)  # qos-map supp only on port
            #  self.attribute_set(self.lag1, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, 0)  # qos-map supp only on port
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)

            pass

###############################################################################
@group('meter')
class StormControlTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) == 0):
            print("No Storm Control feature, skipping")
            return

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                                  mac_address='00:22:22:22:22:22',
                                  destination_handle=self.port0)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                                  mac_address='00:11:11:11:11:11',
                                  destination_handle=self.port1)

        self.uc_storm_control = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_STORM_CONTROL,
            pbs=125,cbs=125,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_BYTES,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        self.mc_storm_control = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_STORM_CONTROL,
            pbs=125, cbs=125, cir=1000, pir=1000,
            type=SWITCH_METER_ATTR_TYPE_BYTES,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        self.bc_storm_control = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_STORM_CONTROL,
            pbs=125,cbs=125,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_BYTES,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_UNKNOWN_UNICAST_STORM_CONTROL, self.uc_storm_control)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_MULTICAST_STORM_CONTROL, self.mc_storm_control)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_BROADCAST_STORM_CONTROL, self.bc_storm_control)

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) != 0):
                self.StormControlUcastTest()
                self.StormControlMulticastTest()
                self.StormControlBcastTest()
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) != 0):
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_UNKNOWN_UNICAST_STORM_CONTROL, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_MULTICAST_STORM_CONTROL, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_BROADCAST_STORM_CONTROL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def verifyPortScStats(self, port_handle, port_cntr_type, rx_pkt_count):
        if port_cntr_type < SWITCH_PORT_COUNTER_ID_SC_UCAST_RED_PACKETS or \
            port_cntr_type > SWITCH_PORT_COUNTER_ID_SC_BCAST_GREEN_PACKETS:
            print(" Inval port SC port_cntr_type: %d" % port_cntr_type)
            return 0

        port_cntrs = self.object_counters_get(port_handle)
        for cntr in port_cntrs:
            if cntr.counter_id == port_cntr_type:
                print(" cntr-type %d, stats-cnt %d, rx-pkt-cnt %d"%(port_cntr_type, cntr.count, rx_pkt_count))
                return cntr.count == rx_pkt_count

    def StormControlUcastTest(self):
        print("StormControlUnknownUcastTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            print("L2 & L3 Multicast feature not enabled, skipping")
            return
        pkt_cnt = 8
        try:
            self.client.object_counters_clear_all(self.port0)
            pkt = simple_udp_packet(
                eth_dst='00:10:10:10:10:10',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.1',
                ip_ttl=64, ip_tos=80, pktlen=1000)

            for i in range(0, pkt_cnt):
                send_packet(self, self.devports[0], pkt)

            print(" Waiting 5 secs before checking meter stats")
            time.sleep(5)
            green_pkt_cnt = count_matched_packets(self, pkt, self.devports[1])
            red_pkt_cnt = pkt_cnt - green_pkt_cnt
            self.assertTrue(red_pkt_cnt > 0)
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_UCAST_GREEN_PACKETS, green_pkt_cnt))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_UCAST_RED_PACKETS, red_pkt_cnt))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_MCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_MCAST_RED_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_BCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_BCAST_RED_PACKETS, 0))

            self.client.object_counters_clear_all(self.port0)
            pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.1',
                ip_ttl=64, ip_tos=80,
                pktlen=1000)
            for i in range(0, pkt_cnt):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, pkt, self.devports[1])

            print("StormControlKnownUcastTest()")
            # This time the traffic is know UC, so SC counter must be zero
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_UCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_UCAST_RED_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_MCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_MCAST_RED_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_BCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port0, SWITCH_PORT_COUNTER_ID_SC_BCAST_RED_PACKETS, 0))
        finally:
            pass

    def StormControlMulticastTest(self):
        print("StormControlMulticastTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            print("Multicast feature not enable, skipping")
            return

        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)
        self.attribute_set(self.vlan30, SWITCH_VLAN_ATTR_IGMP_SNOOPING, True)
        self.vlan_mbr201 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port1,
                                           tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.vlan_mbr202 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port2,
                                           tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.vlan_mbr203 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port3,
                                           tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.vlan_mbr204 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port4,
                                           tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        self.rif20 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20,
                                           vrf_handle=self.vrf10, src_mac=self.rmac, ipv4_multicast=True)

        self.mcast_grp = self.add_l2mc_group(self.device)
        self.mc_member11 = self.add_l2mc_member(
            self.device, output_handle=self.port1, l2mc_group_handle=self.mcast_grp)
        self.mc_member12 = self.add_l2mc_member(
            self.device, output_handle=self.port2, l2mc_group_handle=self.mcast_grp)
        self.mc_member13 = self.add_l2mc_member(
            self.device, output_handle=self.port3, l2mc_group_handle=self.mcast_grp)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_MROUTER_MC_HANDLE, self.mcast_grp)

        # Create snooping entry for vlan 30 - execrcise ipv4_multicast_bridge_s_g
        self.mroute = self.add_l2mc_bridge(
            self.device, vlan_handle=self.vlan20, grp_ip='230.1.1.5', src_ip='10.0.10.5',group_handle=self.mcast_grp)

        pkt_cnt = 8
        try:
            self.client.object_counters_clear_all(self.port1)
            pkt = simple_udp_packet(
                eth_dst='01:00:5e:00:01:05',
                eth_src='00:00:00:11:11:11',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_src='10.0.10.5',
                ip_dst='225.0.1.5',
                ip_ttl=64,
                pktlen=1000)
            print("Sending  Unknown Multicast Traffic")
            for i in range(0, pkt_cnt):
                send_packet(self, self.devports[1], pkt)

            print(" Waiting 7 secs before checking meter stats")
            time.sleep(7)
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_UCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_UCAST_RED_PACKETS, 0))
            green_pkt_cnt_2 = count_matched_packets(self, pkt, self.devports[2])
            green_pkt_cnt_3 = count_matched_packets(self, pkt, self.devports[3])
            green_pkt_cnt_4 = count_matched_packets(self, pkt, self.devports[4])
            self.assertTrue(green_pkt_cnt_2, green_pkt_cnt_3)
            self.assertTrue(green_pkt_cnt_3, green_pkt_cnt_4)
            green_pkt_cnt = green_pkt_cnt_2
            red_pkt_cnt = pkt_cnt - green_pkt_cnt
            self.assertTrue(red_pkt_cnt > 0)
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_MCAST_GREEN_PACKETS, green_pkt_cnt))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_MCAST_RED_PACKETS, red_pkt_cnt))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_BCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_BCAST_RED_PACKETS, 0))

            self.client.object_counters_clear_all(self.port1)
            pkt = simple_udp_packet(
                eth_dst='01:00:5e:00:01:05',
                eth_src='00:00:00:11:11:11',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_src='10.0.10.5',
                ip_dst='230.1.1.5',
                ip_ttl=64,
                pktlen=1000)
            print("Sending Known Multicast Traffic")
            for i in range(0, pkt_cnt):
                send_packet(self, self.devports[1], pkt)
            print(" Waiting 7 secs before checking meter stats")
            time.sleep(5)
            # This time the traffic is known MC, so SC counter must be zero
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_UCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_UCAST_RED_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_MCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_MCAST_RED_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_BCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port1, SWITCH_PORT_COUNTER_ID_SC_BCAST_RED_PACKETS, 0))
        finally:
            self.client.object_counters_clear_all(self.port1)
            self.clean_to_object(self.mroute)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_MROUTER_MC_HANDLE, 0)
            self.clean_to_object(self.vlan_mbr201)
            self.attribute_set(self.vlan30, SWITCH_VLAN_ATTR_IGMP_SNOOPING, False)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, True)
            pass

    def StormControlBcastTest(self):
        print("StormControlBcastTest()")
        pkt_cnt = 5
        try:
            self.client.object_counters_clear_all(self.port2)
            pkt = simple_udp_packet(
                eth_dst='ff:ff:ff:ff:ff:ff',
                eth_src='00:33:33:33:33:33',
                ip_dst='172.16.0.1',
                ip_ttl=64, ip_tos=80, pktlen=1000)
            for i in range(0, pkt_cnt):
                send_packet(self, self.devports[2], pkt)

            print(" Waiting 5 secs before checking meter stats")
            time.sleep(5)
            self.assertTrue(self.verifyPortScStats(self.port2, SWITCH_PORT_COUNTER_ID_SC_UCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port2, SWITCH_PORT_COUNTER_ID_SC_UCAST_RED_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port2, SWITCH_PORT_COUNTER_ID_SC_MCAST_GREEN_PACKETS, 0))
            self.assertTrue(self.verifyPortScStats(self.port2, SWITCH_PORT_COUNTER_ID_SC_MCAST_RED_PACKETS, 0))
            green_pkt_cnt = count_matched_packets(self, pkt, self.devports[1])
            red_pkt_cnt = pkt_cnt - green_pkt_cnt
            #self.assertTrue(red_pkt_cnt > 0)
            self.assertTrue(self.verifyPortScStats(self.port2, SWITCH_PORT_COUNTER_ID_SC_BCAST_GREEN_PACKETS, green_pkt_cnt))
            self.assertTrue(self.verifyPortScStats(self.port2, SWITCH_PORT_COUNTER_ID_SC_BCAST_RED_PACKETS, red_pkt_cnt))
        finally:
            pass

@group('meter')
class StormControlScaleTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) == 0):
            print("No Storm Control feature, skipping")
            return

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                                  mac_address='00:22:22:22:22:22',
                                  destination_handle=self.port0)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                                  mac_address='00:11:11:11:11:11',
                                  destination_handle=self.port1)

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) != 0):
                met_i = 0
                while True:
                    self.uc_storm_control = self.add_meter(self.device,
                        mode=SWITCH_METER_ATTR_MODE_STORM_CONTROL,
                        pbs=2,cbs=2,cir=1,pir=1,
                        type=SWITCH_METER_ATTR_TYPE_BYTES,
                        color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
                    self.bc_storm_control = self.add_meter(self.device,
                        mode=SWITCH_METER_ATTR_MODE_STORM_CONTROL,
                        pbs=2,cbs=2,cir=1,pir=1,
                        type=SWITCH_METER_ATTR_TYPE_BYTES,
                        color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
                    met_i += 2

                    if met_i % 100 == 0:
                        self.attribute_set(self.port0, SWITCH_PORT_ATTR_UNKNOWN_UNICAST_STORM_CONTROL, self.uc_storm_control)
                        self.attribute_set(self.port2, SWITCH_PORT_ATTR_BROADCAST_STORM_CONTROL, self.bc_storm_control)
                        print("Testing Meters, ucast meter %d bcast meter %d" %((met_i-2)+1, (met_i-1)+1))
                        self.StormControlUcastTest()
                        self.StormControlBcastTest()

                    if met_i > 1020:
                        break

                    #  For ucast, test meter reset on port then reconfigure
                    #  For bcast, test meter update on port
                    self.attribute_set(self.port0, SWITCH_PORT_ATTR_UNKNOWN_UNICAST_STORM_CONTROL, 0)
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) != 0):
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_UNKNOWN_UNICAST_STORM_CONTROL, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_BROADCAST_STORM_CONTROL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def verifyPortScStatCount(self, port_handle, rx_pkt_count, bcast=False):
        port_cntrs = self.object_counters_get(port_handle)
        total_stat_count = 0
        for cntr in port_cntrs:
            if bcast:
                if cntr.counter_id != SWITCH_PORT_COUNTER_ID_SC_BCAST_RED_PACKETS and \
                   cntr.counter_id != SWITCH_PORT_COUNTER_ID_SC_BCAST_GREEN_PACKETS:
                    continue
            else:
                if cntr.counter_id != SWITCH_PORT_COUNTER_ID_SC_UCAST_RED_PACKETS and \
                   cntr.counter_id != SWITCH_PORT_COUNTER_ID_SC_UCAST_GREEN_PACKETS:
                    continue
            total_stat_count += cntr.count
            print("  ctr %d, ctr-stat-cnt %d" %(cntr.counter_id, cntr.count))
        print(" total-stat-cnt %d, rx-pkt-cnt %d" %(total_stat_count, rx_pkt_count))
        return (total_stat_count == rx_pkt_count)

    def StormControlUcastTest(self):
        print(" StormControlUnknownUcastTest()")
        pkt_cnt = 5
        try:
            self.client.object_counters_clear_all(self.port0)
            pkt = simple_udp_packet(
                eth_dst='00:10:10:10:10:10',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.1',
                ip_ttl=64, ip_tos=80, pktlen=1000)

            for i in range(0, pkt_cnt):
                send_packet(self, self.devports[0], pkt)

            print("  Waiting 5 secs before checking meter stats")
            time.sleep(5)
            self.assertTrue(self.verifyPortScStatCount(self.port0, pkt_cnt, False))

        finally:
            pass

    def StormControlBcastTest(self):
        print(" StormControlBcastTest()")
        pkt_cnt = 5
        try:
            self.client.object_counters_clear_all(self.port2)
            pkt = simple_udp_packet(
                eth_dst='ff:ff:ff:ff:ff:ff',
                eth_src='00:33:33:33:33:33',
                ip_dst='172.16.0.1',
                ip_ttl=64, ip_tos=80, pktlen=1000)
            for i in range(0, pkt_cnt):
                send_packet(self, self.devports[2], pkt)

            print("  Waiting 5 secs before checking meter stats")
            time.sleep(5)
            self.assertTrue(self.verifyPortScStatCount(self.port2, pkt_cnt, True))

        finally:
            pass

@group('wred')
@disabled
class WredTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()
        self._feature_check=True
        if (self.client.is_feature_enable(SWITCH_FEATURE_WRED) == 0):
            print("WRED feature not enabled, skipping")
            self._feature_check=False
            return
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='172.20.10.1')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55',
            handle=self.rif1, dest_ip='172.20.10.1')
        self.route0 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='172.20.10.2')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55',
            handle=self.rif2, dest_ip='172.20.10.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.2', vrf_handle=self.vrf10, nexthop_handle=self.nhop2)

        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop3 = self.add_nexthop(self.device, handle=self.rif3, dest_ip='10.10.0.2')
        self.neighbor3 = self.add_neighbor(self.device, mac_address='00:33:22:33:44:55',
            handle=self.rif3, dest_ip='10.10.0.2')
        self.route2 = self.add_route(self.device, ip_prefix='4000::2', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)

        self.rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop4 = self.add_nexthop(self.device, handle=self.rif4, dest_ip='172.20.10.3')
        self.neighbor4 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55',
            handle=self.rif4, dest_ip='172.20.10.3')
        self.route3 = self.add_route(self.device, ip_prefix='172.20.10.3', vrf_handle=self.vrf10,
        nexthop_handle=self.nhop4)

        self.p1_queue_handles = self.attribute_get(self.port1, SWITCH_PORT_ATTR_QUEUE_HANDLES)

        # Create ecn marking wred profile (WredIPv4Test)
        max_thr = 50 * 80
        self.ecn_wred_handle = self.add_wred(self.device, enable=False, ecn_mark=True,
            min_threshold=0, max_threshold=max_thr, probability=100, time_constant=100)
        self.ecn_wred_profile = self.add_wred_profile(self.device, wred_green_handle=self.ecn_wred_handle)
        self.attribute_set(self.p1_queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, self.ecn_wred_profile)

        #(WredDropIPv4Test)
        self.p3_queue_handles = self.attribute_get(self.port3, SWITCH_PORT_ATTR_QUEUE_HANDLES)

    def runTest(self):
        try:
            if (self._feature_check):
                self.WredAttrCheckTest()
                self.WredIPv4Test()
                self.WredIPv6Test()
                self.WredDropIPv4Test()
        finally:
            pass

    def tearDown(self):
        if (self._feature_check):
            self.attribute_set(self.p1_queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, 0)
        self.cleanup()

    def WredIPv4Test(self):
        print("WredIPv4Test()")
        i_port_stats = self.object_counters_get(self.port1)
        i_queue_stats = self.object_counters_get(self.p1_queue_handles[0].oid)
        try:
            pkt1 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)

            # ECN-capable transport
            pkt2 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_tos=1,
                ip_id=105,
                ip_ttl=64)

            exp_pkt1 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            exp_pkt2 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_tos=3,
                ip_id=105,
                ip_ttl=63)

            exp_pkt3 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_tos=1,
                ip_id=105,
                ip_ttl=63)

            x = SWITCH_QUEUE_COUNTER_ID_WRED_GREEN_ECN_MARKED_PACKETS
            y = SWITCH_PORT_COUNTER_ID_WRED_GREEN_ECN_MARKED_PACKETS

            print("ECT is NOT set.")
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[1])

            print("ECT is set.")
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, exp_pkt2, self.devports[1])

            e_port_stats = self.object_counters_get(self.port1)
            e_queue_stats = self.object_counters_get(self.p1_queue_handles[0].oid)
            self.assertEqual(e_queue_stats[x].count - i_queue_stats[x].count, 1)
            self.assertEqual(e_port_stats[y].count - i_port_stats[y].count, 1)

            print("Detach the profile. Packet should NOT get marked.")
            self.attribute_set(self.p1_queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, 0)

            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, exp_pkt3, self.devports[1])

            e_port_stats = self.object_counters_get(self.port1)
            e_queue_stats = self.object_counters_get(self.p1_queue_handles[0].oid)
            self.assertEqual(e_queue_stats[x].count - i_queue_stats[x].count, 0)
            self.assertEqual(e_port_stats[y].count - i_port_stats[y].count, 0)
        finally:
            # Always end test with WRED profile attached. We do this specifically for testing fast reconfig
            # scenario
            self.attribute_set(self.p1_queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, self.ecn_wred_profile)

    def WredIPv6Test(self):
        print("WredIPv6Test()")
        i_port_stats = self.object_counters_get(self.port3)
        i_queue_stats = self.object_counters_get(self.p3_queue_handles[0].oid)
        self.attribute_set(self.p3_queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, self.ecn_wred_profile)
        try:
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::2',
                ipv6_src='2000::1',
                ipv6_ecn=1,
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:33:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='4000::2',
                ipv6_src='2000::1',
                ipv6_ecn=3,
                ipv6_hlim=63)

            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[3])

            e_port_stats = self.object_counters_get(self.port3)
            e_queue_stats = self.object_counters_get(self.p3_queue_handles[0].oid)
            x = SWITCH_QUEUE_COUNTER_ID_WRED_GREEN_ECN_MARKED_PACKETS
            y = SWITCH_PORT_COUNTER_ID_WRED_GREEN_ECN_MARKED_PACKETS
            self.assertTrue(e_queue_stats[x].count - i_queue_stats[x].count, 1)
            self.assertTrue(e_port_stats[y].count - i_port_stats[y].count, 1)
        finally:
            self.attribute_set(self.p3_queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, 0)

    def WredDropIPv4Test(self):
        print("WredDropIPv4Test()")
        max_thr = 50 * 80
        wred_handle = self.add_wred(self.device, enable=True, ecn_mark=False,
            min_threshold=0, max_threshold=max_thr, probability=100, time_constant=100)
        wred_profile = self.add_wred_profile(self.device, wred_green_handle=wred_handle)
        queue_handles = self.attribute_get(self.port4, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        i_port_stats = self.object_counters_get(self.port4)
        i_queue_stats = self.object_counters_get(queue_handles[0].oid)
        try:
            pkt1 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.3',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)

            pkt2 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.2',
                ip_src='192.168.0.1',
                ip_tos=1,
                ip_id=105,
                ip_ttl=64)

            # ECN-capable transport
            pkt3 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.3',
                ip_src='192.168.0.1',
                ip_tos=1,
                ip_id=105,
                ip_ttl=64)

            # send the test packet(s)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.3',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            print("Attach the wred profile to a wrong queue. Packet should NOT get dropped")
            self.attribute_set(queue_handles[1].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, wred_profile)
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt, self.devports[4])

            print("Attach the wred profile to the correct queue. Packet should get dropped")
            self.attribute_set(queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, wred_profile)
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)

            e_port_stats = self.object_counters_get(self.port4)
            e_queue_stats = self.object_counters_get(queue_handles[0].oid)
            x = SWITCH_QUEUE_COUNTER_ID_WRED_GREEN_DROPPED_PACKETS
            y = SWITCH_PORT_COUNTER_ID_WRED_GREEN_DROPPED_PACKETS
            self.assertEqual(e_queue_stats[x].count - i_queue_stats[x].count, 1)
            self.assertEqual(e_port_stats[y].count - i_port_stats[y].count, 1)

            print("Detach the profile. Packet should NOT get dropped.")
            self.attribute_set(queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, 0)
            self.attribute_set(queue_handles[1].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, 0)

            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt, self.devports[4])

            print("Turn on ECN marking.")
            self.attribute_set(wred_handle, SWITCH_WRED_ATTR_ECN_MARK, True)
            self.attribute_set(queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, wred_profile)

            print("Send the test packet to port 1. Should get marked.")
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.3',
                ip_src='192.168.0.1',
                ip_tos=3,
                ip_id=105,
                ip_ttl=63)
            print("ECT is NOT set.")
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)

            print("ECT is set.")
            send_packet(self, self.devports[0], pkt3)
            verify_packet(self, exp_pkt, self.devports[4])

            print("Send a packet to different port. Should NOT get marked.")
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.2',
                ip_src='192.168.0.1',
                ip_tos=1,
                ip_id=105,
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, exp_pkt, self.devports[2])

            e_port_stats = self.object_counters_get(self.port4)
            e_queue_stats = self.object_counters_get(queue_handles[0].oid)
            x = SWITCH_QUEUE_COUNTER_ID_WRED_GREEN_ECN_MARKED_PACKETS
            y = SWITCH_PORT_COUNTER_ID_WRED_GREEN_ECN_MARKED_PACKETS
            self.assertEqual(e_queue_stats[x].count - i_queue_stats[x].count, 1)
            self.assertEqual(e_port_stats[y].count - i_port_stats[y].count, 1)

            print("Delete the profile")
            self.attribute_set(queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.3',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt, self.devports[4])
        finally:
            self.attribute_set(queue_handles[0].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, 0)
            self.attribute_set(queue_handles[1].oid, SWITCH_QUEUE_ATTR_WRED_PROFILE_HANDLE, 0)
            pass

    def WredAttrCheckTest(self):
        try:
          print("WredAttrCheckTest()")
          wred_handle = self.add_wred(self.device, enable=True, ecn_mark=False,
              min_threshold=0, max_threshold=4000, probability=100, time_constant=100)
          weight = self.attribute_get(wred_handle, SWITCH_WRED_ATTR_WEIGHT)
          self.assertTrue(weight == 0, "Incorrect value of WRED weight attribute")
        finally:
          pass

@group('qos')
@group('acl')
@disabled
class QoSAclMarkingTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()

        # configure port0 and port1 for qos acl testing
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.port0)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port1)

        try:
            self.MacQosAclTest()
            self.Ipv4QosAclTest()
            self.Ipv6QosAclTest()
            self.QosAclBdLabelTest()
        finally:
            self.cleanup()


    def MacQosAclTest(self):
        '''
        This test tries to verify qos mac acl entries
        We send a packet from port 0 and port 1
        The port_lag_label comes from the NOS.
        '''
        print("MacQosAclTest()")
        tc_value = 22
        ingress_pcp_value = 3
        egress_pcp_value = 5
        qos_port_lable = 10

        # create qos_mac acl , action = set tc=22
        # @todo: create acl counter for qos acl after p4 support added.
        qos_mac_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC_QOS,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        qos_mac_acl_entry = self.add_acl_entry(self.device,
            src_mac='00:33:33:33:33:33',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            pcp=ingress_pcp_value,
            pcp_mask = 0x0F,
            port_lag_label = qos_port_lable,
            port_lag_label_mask=255,
            tc=tc_value,
            table_handle=qos_mac_acl_table)

        # configure ingress qos tc to queue maps
        ingress_qos_map1 = self.add_qos_map(self.device, tc=tc_value, qid=1)
        tc_qos_maps1 = []
        tc_qos_maps1.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=ingress_qos_map1))
        tc_qos_maps_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_qos_maps1)

        # configure egress tc to pcp qos maps.
        egress_qos_map1 = self.add_qos_map(self.device, tc=tc_value, pcp=egress_pcp_value)
        tc_qos_maps = []
        tc_qos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=egress_qos_map1))
        tc_qos_maps_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_PCP, qos_map_list=tc_qos_maps)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, tc_qos_maps_egress)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, qos_port_lable)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, qos_mac_acl_table)

        try:
            print("Sending packet port %d" % self.devports[0], "  -> port %d" % \
                  self.devports[1], "  (00:33:33:33:33:33 -> 00:44:44:44:44:44) mac_acl hit")

            pkt = simple_tcp_packet(
                  eth_dst='00:44:44:44:44:44',
                  eth_src='00:33:33:33:33:33',
                  ip_dst='172.20.10.1',
                  ip_src='192.168.0.1',
                  dl_vlan_enable=True,
                  vlan_vid=10,
                  vlan_pcp=ingress_pcp_value,
                  ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            exp_pkt = simple_tcp_packet(
                  eth_dst='00:44:44:44:44:44',
                  eth_src='00:33:33:33:33:33',
                  ip_dst='172.20.10.1',
                  ip_src='192.168.0.1',
                  dl_vlan_enable=True,
                  vlan_vid=10,
                  vlan_pcp=egress_pcp_value,
                  ip_ttl=64)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped pcp value 3 -> 5")

        finally:
            # cleanup
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # cleanup qos-maps
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # cleanup acls
            self.cleanlast()
            self.cleanlast()
            pass


    def QosAclBdLabelTest(self):
        '''
        This test tries to verify qos qos ipv4_acl with bd_label entries
        We send a packet from port 0->port 1
        The bd_label comes from the NOS.
        '''
        print("QosAclBdLabelTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            print("qos-acl feature not enabled, skipping")
            return
        tc_value = 23
        ingress_dscp_value = 1
        egress_dscp_value = 9
        qos_bd_lable = 10

        # create qos_ipv4_acl
        # @todo: create acl counter for qos acl after p4 support added.
        ip_qos_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_QOS,
            bind_point_type=[acl_table_bp_vlan],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        ip_qos_acl_entry = self.add_acl_entry(self.device,
            dst_ip='172.20.10.1',
            dst_ip_mask='255.255.255.255',
            ip_dscp=ingress_dscp_value,
            ip_dscp_mask = 0x0f,
            bd_label = qos_bd_lable,
            bd_label_mask=255,
            tc=tc_value,
            table_handle=ip_qos_acl_table)

        # configure ingress qos tc to queue maps
        ingress_qos_map1 = self.add_qos_map(self.device, tc=tc_value, qid=1)
        tc_qos_maps = [ingress_qos_map1]
        tc_qos_maps_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_qos_maps)

        # configure egress tc to pcp qos maps.
        egress_qos_map1 = self.add_qos_map(self.device, tc=tc_value, dscp=egress_dscp_value)
        tc_qos_maps = [egress_qos_map1]
        tc_qos_maps_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_qos_maps)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, tc_qos_maps_egress)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_INGRESS_VLAN_RIF_LABEL, qos_bd_lable)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, ip_qos_acl_table)

        try:
            print("Sending packet port %d" % self.devports[0], "  -> port %d" % self.devports[
                  1], "  (192.168.0.1 -> 172.20.10.1) qos ip_acl hit")

            pkt = simple_tcp_packet(
                  eth_dst='00:44:44:44:44:44',
                  eth_src='00:33:33:33:33:33',
                  ip_dst='172.20.10.1',
                  ip_src='192.168.0.1',
                  ip_id=105,
                  ip_tos=4, # dscp 1
                  ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            exp_pkt = simple_tcp_packet(
                  eth_dst='00:44:44:44:44:44',
                  eth_src='00:33:33:33:33:33',
                  ip_dst='172.20.10.1',
                  ip_src='192.168.0.1',
                  ip_id=105,
                  ip_tos=36, # dscp 9
                  ip_ttl=63)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 9")

        finally:
            # cleanup
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_INGRESS_VLAN_RIF_LABEL, 0)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, 0)

            # cleanup qos-maps
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # cleanup acls
            self.cleanlast()
            self.cleanlast()
            pass

    def Ipv4QosAclTest(self):
        '''
        This test tries to verify qos ipv4 qos acl entries
        We send a packet from port 0->port 1
        The port_lag_label comes from the NOS.
        '''
        print("Ipv4QosAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            print("qos-acl feature not enabled, skipping")
            return
        tc_value = 23
        ingress_dscp_value = 1
        egress_dscp_value = 9
        qos_port_lable = 10

        # create qos_ip_acl
        # @todo: create acl counter for qos acl after p4 support added.
        ip_qos_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        ip_qos_acl_entry = self.add_acl_entry(self.device,
            dst_ip='172.20.10.1',
            dst_ip_mask='255.255.255.255',
            ip_dscp=ingress_dscp_value,
            ip_dscp_mask = 0x0F,
            port_lag_label = qos_port_lable,
            port_lag_label_mask=255,
            tc=tc_value,
            table_handle=ip_qos_acl_table)

        # configure ingress qos tc to queue maps
        ingress_qos_map1 = self.add_qos_map(self.device, tc=tc_value)
        tc_qos_maps = [ingress_qos_map1]
        tc_qos_maps_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_qos_maps)

        # configure egress tc to pcp qos maps.
        egress_qos_map1 = self.add_qos_map(self.device, tc=tc_value, dscp=egress_dscp_value)
        tc_qos_maps = [egress_qos_map1]
        tc_qos_maps_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_qos_maps)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, tc_qos_maps_egress)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, qos_port_lable)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, ip_qos_acl_table)

        try:
            print("Sending packet port %d" % self.devports[0], "  -> port %d" % self.devports[
                  1], "  (192.168.0.1 -> 172.20.10.1) qos ip_acl hit")

            pkt = simple_tcp_packet(
                  eth_dst='00:44:44:44:44:44',
                  eth_src='00:33:33:33:33:33',
                  ip_dst='172.20.10.1',
                  ip_src='192.168.0.1',
                  ip_id=105,
                  ip_tos=4, # dscp 1
                  ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            exp_pkt = simple_tcp_packet(
                  eth_dst='00:44:44:44:44:44',
                  eth_src='00:33:33:33:33:33',
                  ip_dst='172.20.10.1',
                  ip_src='192.168.0.1',
                  ip_id=105,
                  ip_tos=36, # dscp 9
                  ip_ttl=63)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 9")

        finally:
            # cleanup
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # cleanup qos-maps
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # cleanup acls
            self.cleanlast()
            self.cleanlast()
            pass


    def Ipv6QosAclTest(self):
        '''
        This test tries to verify qos ipv6 qos acl entries
        We send a packet from port 0->port 1
        The port_lag_label comes from the NOS.
        '''
        print("Ipv6QosAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            print("qos-acl feature not enabled, skipping")
            return
        tc_value = 24
        ingress_dscp_value = 1
        egress_dscp_value = 9
        qos_port_lable = 10

        # create qos_ip_acl
        # @todo: create acl counter for qos acl after p4 support added.
        ip_qos_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        ip_qos_acl_entry = self.add_acl_entry(self.device,
            ip_dscp=ingress_dscp_value,
            ip_dscp_mask = 0x0F,
            port_lag_label = qos_port_lable,
            port_lag_label_mask=255,
            tc=tc_value,
            table_handle=ip_qos_acl_table)

        # configure ingress qos tc to queue maps
        ingress_qos_map1 = self.add_qos_map(self.device, tc=tc_value, qid=1)
        tc_qos_maps = [ingress_qos_map1]
        tc_qos_maps_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_qos_maps)

        # configure egress tc to pcp qos maps.
        egress_qos_map1 = self.add_qos_map(self.device, tc=tc_value, dscp=egress_dscp_value)
        tc_qos_maps = [egress_qos_map1]
        tc_qos_maps_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_qos_maps)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, tc_qos_maps_egress)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, qos_port_lable)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, ip_qos_acl_table)

        try:
            print("Sending packet port %d" % self.devports[0], "  -> port %d" % self.devports[
                  1], "  (2000::1 -> 1234:5678:9abc:def0:4422:1133:5577:99aa) qos ip_acl hit")

            pkt = simple_tcpv6_packet(
                eth_dst='00:44:44:44:44:44',
              eth_src='00:33:33:33:33:33',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_dscp=ingress_dscp_value,
              ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:44:44:44:44:44',
              eth_src='00:33:33:33:33:33',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_dscp=egress_dscp_value,
              ipv6_hlim=63)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 9")

        finally:
            # cleanup
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, qos_port_lable)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # cleanup qos-maps
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # cleanup acls
            self.cleanlast()
            self.cleanlast()
            pass

@group('qos')
class TcIcosQueueMarkingStatsTest(ApiHelper):
    '''
    Test to validate TC to iCoS and Queue mapping
    '''
    def configureDscpMaps(self):
        dscp_qos_map1 = self.add_qos_map(self.device, dscp=1, tc=20)
        dscp_qos_map2 = self.add_qos_map(self.device, dscp=2, tc=24)
        dscp_qos_map3 = self.add_qos_map(self.device, dscp=3, tc=28)
        dscp_qos_map4 = self.add_qos_map(self.device, dscp=4, tc=30)
        dscp_tc_maps = []
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=dscp_qos_map1))
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=dscp_qos_map2))
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=dscp_qos_map3))
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=dscp_qos_map4))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)

    def CreatePortPPG(self, port_handle, ppg_index):
        ppg_handle = self.add_port_priority_group(self.device, port_handle=port_handle, ppg_index=ppg_index)
        return ppg_handle

    def ConfigureTc2QueueMap(self):
        self.queue_qos_map1 = self.add_qos_map(self.device, tc=20, qid=3)
        self.queue_qos_map2 = self.add_qos_map(self.device, tc=24, qid=4)
        self.queue_qos_map3 = self.add_qos_map(self.device, tc=28, qid=5)
        self.queue_qos_map4 = self.add_qos_map(self.device, tc=30, qid=6)
        tc_queue_maps = []
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.queue_qos_map1))
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.queue_qos_map2))
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.queue_qos_map3))
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.queue_qos_map4))

        self.tc_queue_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)

    def ConfigureTc2ICosPPGMap(self):
        self.tc_qos_map1 = self.add_qos_map(self.device, tc=20, icos=3)
        self.tc_qos_map2 = self.add_qos_map(self.device, tc=24, icos=4)
        self.tc_qos_map3 = self.add_qos_map(self.device, tc=28, icos=5)
        self.tc_qos_map4 = self.add_qos_map(self.device, tc=30, icos=6)
        tc_icos_maps = []
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_qos_map1))
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_qos_map2))
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_qos_map3))
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_qos_map4))

        self.tc_icos_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=tc_icos_maps)

        #iCos=3, ppg_index=3
        #iCos=4, ppg_index=4
        #iCos=5, ppg_index=5
        #iCos=6, ppg_index=6

        self.icos_pg_map1 = self.add_qos_map(self.device, icos=3, ppg=3)
        self.icos_pg_map2 = self.add_qos_map(self.device, icos=4, ppg=4)
        self.icos_pg_map3 = self.add_qos_map(self.device, icos=5, ppg=5)
        self.icos_pg_map4 = self.add_qos_map(self.device, icos=6, ppg=6)
        icos_pg_list= [self.icos_pg_map1, self.icos_pg_map2, self.icos_pg_map3, self.icos_pg_map4]

        icos_ppg_maps=[]
        for icos_map in icos_pg_list:
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_map))

        self.icos_to_ppg_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)

    def TestDscpIcosQueueUpdateDelete(self, is_lag=False):
        port_lag_str = "port"
        port_lag_oid = self.port0
        port_lag_qos_group_attr_id = SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP
        port_oid = self.port0
        port_index = 0

        if is_lag:
            # qos-map dscp/pcp supp only on port
            print(" Lag not supp, dscp/pcp qos-map can be set only on port ")
            return
            '''
            port_lag_str = "LAG port"
            port_lag_oid = self.lag0
            port_lag_qos_group_attr_id = SWITCH_LAG_ATTR_INGRESS_QOS_DSCP_TOS_GROUP  # qos-map supp only on port
            port_oid = self.port2
            port_index = 2
            '''

        print("Try both DSCP to iCos and Queue {} test with update/delete and stats".format(port_lag_str))

        try:
            self.attribute_set(port_lag_oid, port_lag_qos_group_attr_id, self.dscp_tc_map_ingress)

            #iCos=3, ppg_index=3
            #iCos=4, ppg_index=4
            #iCos=5, ppg_index=5
            self.ingress_port_ppg_3 = self.CreatePortPPG(port_oid, 3)
            self.ingress_port_ppg_4 = self.CreatePortPPG(port_oid, 4)
            self.ingress_port_ppg_5 = self.CreatePortPPG(port_oid, 5)

            icos_pg_map1 = self.add_qos_map(self.device, icos=3, ppg=3)
            icos_pg_map2 = self.add_qos_map(self.device, icos=4, ppg=4)
            icos_pg_map3 = self.add_qos_map(self.device, icos=5, ppg=5)
            icos_ppg_maps=[]
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map1))
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map2))
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map3))

            self.icos_to_ppg_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)

            tc_qos_map1 = self.add_qos_map(self.device, tc=20, icos=3)
            tc_qos_map2 = self.add_qos_map(self.device, tc=24, icos=4)
            # this one will be used for update
            new_icos = 5
            tc_qos_map3 = self.add_qos_map(self.device, tc=20, icos=new_icos)
            tc_icos_maps = []
            tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map1))
            tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map2))

            self.tc_icos_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=tc_icos_maps)

            queue_qos_map1 = self.add_qos_map(self.device, tc=20, qid=3)
            queue_qos_map2 = self.add_qos_map(self.device, tc=24, qid=4)
            # this one will be used for update
            new_qid = 6
            queue_qos_map3 = self.add_qos_map(self.device, tc=20, qid=new_qid)
            tc_queue_maps = []
            tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=queue_qos_map1))
            tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=queue_qos_map2))

            self.tc_queue_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)

            self.attribute_set(port_oid, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)
            self.attribute_set(port_oid, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, self.tc_queue_map_ingress)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=63)

            self.queue_list = self.attribute_get(self.port1,
                             SWITCH_PORT_ATTR_QUEUE_HANDLES)
            self.client.object_counters_clear_all(self.ingress_port_ppg_3)
            self.client.object_counters_clear_all(self.ingress_port_ppg_5)
            self.client.object_counters_clear_all(self.queue_list[0].oid)
            self.client.object_counters_clear_all(self.queue_list[3].oid)
            self.client.object_counters_clear_all(self.queue_list[new_qid].oid)

            print("pass packets w/ DSCP 1 to Queue 3 and iCos 3/PPG 3")
            pkt_count = 2
            for i in range(0, pkt_count):
                send_packet(self, self.devports[port_index], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            time.sleep(2)
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            queue_counter = self.object_counters_get(self.queue_list[3].oid)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)

            # Update tc_to_icos and tc_to_queue maps, check for icos 5 and queue 6 stats
            tc_icos_maps = []
            tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map2))
            tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map3))

            self.attribute_set(self.tc_icos_map_ingress, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, tc_icos_maps)
            icos_maps = self.attribute_get(self.tc_icos_map_ingress, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST)
            self.assertTrue(len(tc_icos_maps) == len(icos_maps))
            self.assertFalse([icos_maps[i] for i in range(len(icos_maps)) if icos_maps[i].oid != tc_icos_maps[i].oid])

            tc_queue_maps = []
            tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=queue_qos_map2))
            tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=queue_qos_map3))

            self.attribute_set(self.tc_queue_map_ingress, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, tc_queue_maps)
            queue_maps = self.attribute_get(self.tc_queue_map_ingress, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST)
            self.assertTrue(len(tc_queue_maps) == len(queue_maps))
            self.assertFalse([queue_maps[i] for i in range(len(queue_maps)) if queue_maps[i].oid != tc_queue_maps[i].oid])

            print("pass packets w/ DSCP 1 to Queue 6 and iCos 5/PPG 5")
            for i in range(0, pkt_count):
                send_packet(self, self.devports[port_index], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            ppg_3_counter = self.object_counters_get(self.ingress_port_ppg_3)
            ppg_5_counter = self.object_counters_get(self.ingress_port_ppg_5)
            queue_3_counter = self.object_counters_get(self.queue_list[3].oid)
            queue_6_counter = self.object_counters_get(self.queue_list[6].oid)


            # ppg & queue 3 counters should remain the same from the previous run
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_3_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            self.assertTrue(queue_3_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)

            # queue 6 and ppg 5 counters should contain packets count from the current run
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_5_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            self.assertTrue(queue_6_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)

            # Remove tc_to_queue maps, check for iCos 5 and Queue 0 stats
            self.attribute_set(port_oid, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.clean_to_object(queue_qos_map1)

            print("pass packets w/ DSCP 1 to Queue 0 and iCos 5/PPG 5")
            for i in range(0, pkt_count):
                send_packet(self, self.devports[port_index], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_5)
            queue_counter = self.object_counters_get(self.queue_list[new_qid].oid)

            # ppg 5 counters should contain packets count from previous and current run
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count + pkt_count)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)

            # Remove tc_to_icos maps, check for Queue 0 stats
            self.clean_to_object(tc_qos_map1)

            print("pass packets w/ DSCP 1 to Queue 0 and default iCos")
            for i in range(0, pkt_count):
                send_packet(self, self.devports[port_index], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_5)
            queue_counter = self.object_counters_get(self.queue_list[0].oid)

            # ppg 5 counter should remain unchanged
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count + pkt_count)
            # queue 0 counter should contain packets count from previous and current run
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count + pkt_count)

            self.client.object_counters_clear_all(self.ingress_port_ppg_3)
            self.client.object_counters_clear_all(self.ingress_port_ppg_5)
            self.client.object_counters_clear_all(self.queue_list[0].oid)
            self.client.object_counters_clear_all(self.queue_list[3].oid)
            self.client.object_counters_clear_all(self.queue_list[new_qid].oid)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_5)
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)

            queue_counter = self.object_counters_get(self.queue_list[0].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)
            queue_counter = self.object_counters_get(self.queue_list[3].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)
            queue_counter = self.object_counters_get(self.queue_list[new_qid].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)

        finally:
            self.attribute_set(port_lag_oid, port_lag_qos_group_attr_id, 0)
            self.attribute_set(port_oid, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.attribute_set(port_oid, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.clean_to_object(self.ingress_port_ppg_3)

    def TestDscpIcosPpgUpdate(self, is_lag=False):
        port_lag_str = "port"
        port_lag_oid = self.port0
        port_lag_qos_group_attr_id = SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP
        port_oid = self.port0
        port_index = 0

        if is_lag:
            # qos-map supp only on port
            print(" Lag not supp, dscp/pcp qos-map can be set only on port ")
            return
            '''
            port_lag_str = "LAG port"
            port_lag_oid = self.lag0
            port_lag_qos_group_attr_id = SWITCH_LAG_ATTR_INGRESS_QOS_DSCP_TOS_GROUP
            port_oid = self.port2
            port_index = 2
            '''

        print("Try DSCP to iCos and iCos to PPG {} test with update and stats".format(port_lag_str))

        try:
            self.attribute_set(port_lag_oid, port_lag_qos_group_attr_id, self.dscp_tc_map_ingress)

            #iCos=3, ppg_index=3
            #iCos=4, ppg_index=4
            #iCos=3, ppg_index=5
            self.ingress_port_ppg_3 = self.CreatePortPPG(port_oid, 3)
            self.ingress_port_ppg_4 = self.CreatePortPPG(port_oid, 4)
            self.ingress_port_ppg_5 = self.CreatePortPPG(port_oid, 5)

            icos_pg_map1 = self.add_qos_map(self.device, icos=3, ppg=3)
            icos_pg_map2 = self.add_qos_map(self.device, icos=4, ppg=4)
            # This one will be used for update
            icos_pg_map3 = self.add_qos_map(self.device, icos=3, ppg=5)
            icos_ppg_maps=[]
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map1))
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map2))

            self.icos_to_ppg_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)
            self.attribute_set(port_oid, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)

            tc_qos_map1 = self.add_qos_map(self.device, tc=20, icos=3)
            tc_qos_map2 = self.add_qos_map(self.device, tc=24, icos=4)
            tc_icos_maps = []
            tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map1))
            tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map2))

            self.tc_icos_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=tc_icos_maps)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=63)

            self.queue_list = self.attribute_get(self.port1,
                             SWITCH_PORT_ATTR_QUEUE_HANDLES)
            self.client.object_counters_clear_all(self.ingress_port_ppg_3)
            self.client.object_counters_clear_all(self.ingress_port_ppg_5)

            print("pass packets w/ DSCP 1 to iCos 3/PPG 3")
            pkt_count = 2
            for i in range(0, pkt_count):
                send_packet(self, self.devports[port_index], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)

            # Update icos_to_ppg maps, check for icos 5 stats
            icos_ppg_maps=[]
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map2))
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map3))

            self.attribute_set(self.icos_to_ppg_map_ingress, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, icos_ppg_maps)
            icos_maps = self.attribute_get(self.icos_to_ppg_map_ingress, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST)
            self.assertTrue(len(icos_ppg_maps) == len(icos_maps))
            self.assertFalse([icos_maps[i] for i in range(len(icos_maps)) if icos_maps[i].oid != icos_ppg_maps[i].oid])

            print("pass packets w/ DSCP 1 to iCos 3/PPG 5")
            for i in range(0, pkt_count):
                send_packet(self, self.devports[port_index], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_5_counter = self.object_counters_get(self.ingress_port_ppg_5)
                self.assertTrue(ppg_5_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            #ppg_5_counter = self.object_counters_get(self.ingress_port_ppg_5)
            ## For ppg packets are counted per port/icos. As we changed mapping from
            ## icos to ppg now ppg 5 counters should contain packets count from the current
            ## and previous run
            #self.assertTrue(ppg_5_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count + pkt_count)

            self.client.object_counters_clear_all(self.ingress_port_ppg_3)
            self.client.object_counters_clear_all(self.ingress_port_ppg_5)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_5)
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)

        finally:
            self.attribute_set(port_lag_oid, port_lag_qos_group_attr_id, 0)
            self.attribute_set(port_oid, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.attribute_set(port_oid, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.clean_to_object(self.ingress_port_ppg_3)

    def DscpQueueTest(self):
        print("DSCP to Queue test with stats")
        self.ConfigureTc2QueueMap();
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, self.tc_queue_map_ingress)

            print("pass packet w/ DSCP 1 to Queue 3")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 9
                ip_ttl=63)
            pkt_count = 10
            #Send 10 packets and check the Queue stats

            #TC to Queue mapping is set, so check for Queue 3 stats.
            self.queue_list = self.attribute_get(self.port1,
                             SWITCH_PORT_ATTR_QUEUE_HANDLES)

            self.client.object_counters_clear_all(self.queue_list[3].oid)
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])
            counter = self.object_counters_get(self.queue_list[3].oid)

            self.assertTrue(counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)

            #Clear and check the queue counters are zero.
            self.client.object_counters_clear_all(self.queue_list[3].oid)
            counter = self.object_counters_get(self.queue_list[3].oid)

            self.assertTrue(counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            # delete tc to queue maps
            self.clean_to_object(self.queue_qos_map1)

    def DscpIcosTest(self):
        print("DSCP to iCos test with stats")
        self.ConfigureTc2ICosPPGMap()
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            #iCos=3, ppg_index=3
            #iCos=4, ppg_index=4
            #iCos=5, ppg_index=5
            #iCos=6, ppg_index=6
            self.ingress_port_ppg_3 = self.CreatePortPPG(self.port0, 3)
            self.ingress_port_ppg_4 = self.CreatePortPPG(self.port0, 4)
            self.ingress_port_ppg_5 = self.CreatePortPPG(self.port0, 5)
            self.ingress_port_ppg_6 = self.CreatePortPPG(self.port0, 6)

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)

            print("pass packet w/ DSCP 1 to iCos/PPG 3")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 9
                ip_ttl=63)
            pkt_count = 10
            #Send 10 packets and check the PPG stats
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS) == 0):
                print("PPG stats is not enabled, return")
                return

            #Only iCos mapping is set, so check for PPG3 stats and Queue0 stats.
            self.queue_list = self.attribute_get(self.port1,
                             SWITCH_PORT_ATTR_QUEUE_HANDLES)
            self.client.object_counters_clear_all(self.ingress_port_ppg_3)
            self.client.object_counters_clear_all(self.queue_list[0].oid)
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            queue_counter = self.object_counters_get(self.queue_list[0].oid)

            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)

            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            queue_counter = self.object_counters_get(self.queue_list[0].oid)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.clean_to_object(self.tc_qos_map1)

    def DscpIcosQueueTest2Configure(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)):
            self.configureDscpMaps()
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            #iCos=3, ppg_index=3
            #iCos=4, ppg_index=4
            #iCos=5, ppg_index=5
            #iCos=6, ppg_index=6
            self.ingress_port_ppg_3 = self.CreatePortPPG(self.port0, 3)
            self.ingress_port_ppg_4 = self.CreatePortPPG(self.port0, 4)
            self.ingress_port_ppg_5 = self.CreatePortPPG(self.port0, 5)
            self.ingress_port_ppg_6 = self.CreatePortPPG(self.port0, 6)
            self.ConfigureTc2ICosPPGMap()
            self.ConfigureTc2QueueMap();
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, self.tc_queue_map_ingress)

    def DscpIcosQueueTest2TrafficTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)):
            print("Warm/Fast Reboot Version: Try both DSCP to iCos and Queue test with stats")
            try:
                print("pass packet w/ DSCP 1 to Queue 3 and iCos/PPG 3")
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='172.20.10.1',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_tos=4, # dscp 1
                    ip_ttl=64)
                exp_pkt = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:55',
                    eth_src='00:77:66:55:44:33',
                    ip_dst='172.20.10.1',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_tos=4, # dscp 9
                    ip_ttl=63)
                qid = 3
                self.queue_list = self.attribute_get(self.port1,
                                 SWITCH_PORT_ATTR_QUEUE_HANDLES)
                self.client.object_counters_clear_all(self.ingress_port_ppg_3)
                self.client.object_counters_clear_all(self.queue_list[qid].oid)
                if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                    ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
                queue_counter = self.object_counters_get(self.queue_list[qid].oid)

                if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                    self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)
                self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)

                pkt_count = 5
                for i in range(0, pkt_count):
                    send_packet(self, self.devports[0], pkt)
                    verify_packet(self, exp_pkt, self.devports[1])

                if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                    ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
                queue_counter = self.object_counters_get(self.queue_list[qid].oid)

                if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                    self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
                self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)
            finally:
                pass

    def DscpIcosQueueTest2Cleanup(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)):
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.clean_to_object(self.ingress_port_ppg_3)

    def DscpIcosQueueTest(self):
        print("Try both DSCP to iCos and Queue test with stats")

        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)

            #iCos=3, ppg_index=3
            #iCos=4, ppg_index=4
            #iCos=5, ppg_index=5
            #iCos=6, ppg_index=6
            self.ingress_port_ppg_3 = self.CreatePortPPG(self.port0, 3)
            self.ingress_port_ppg_4 = self.CreatePortPPG(self.port0, 4)
            self.ingress_port_ppg_5 = self.CreatePortPPG(self.port0, 5)
            self.ingress_port_ppg_6 = self.CreatePortPPG(self.port0, 6)

            self.ConfigureTc2ICosPPGMap()
            self.ConfigureTc2QueueMap();
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, self.tc_queue_map_ingress)

            print("pass packet w/ DSCP 1 to Queue 3 and iCos/PPG 3")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 9
                ip_ttl=63)
            qid = 3
            self.queue_list = self.attribute_get(self.port1,
                             SWITCH_PORT_ATTR_QUEUE_HANDLES)
            self.client.object_counters_clear_all(self.ingress_port_ppg_3)
            self.client.object_counters_clear_all(self.queue_list[qid].oid)
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            queue_counter = self.object_counters_get(self.queue_list[qid].oid)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)

            pkt_count = 5
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            queue_counter = self.object_counters_get(self.queue_list[qid].oid)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)
            #Remove TC to Queue map, check for iCos 3 and Queue 0 stats
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.cleanlast()

            qid = 0
            self.client.object_counters_clear_all(self.ingress_port_ppg_3)
            self.client.object_counters_clear_all(self.queue_list[qid].oid)
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            queue_counter = self.object_counters_get(self.queue_list[qid].oid)

            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)):
                self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.clean_to_object(self.ingress_port_ppg_3)

    def DscpIcosQueueUpdateDeletePortTest(self):
        self.TestDscpIcosQueueUpdateDelete()

    def DscpIcosQueueUpdateDeleteLagPortTest(self):
        self.TestDscpIcosQueueUpdateDelete(is_lag=True)

    def DscpIcosPpgUpdatePortTest(self):
        self.TestDscpIcosPpgUpdate()

    def DscpIcosPpgUpdateLagPortTest(self):
        self.TestDscpIcosPpgUpdate(is_lag=True)

    def DscpTcQueueStatsClearTest(self):
        print("Try TC to Queue mapping with stats clear")

        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)

            self.queue_qos_map1 = self.add_qos_map(self.device, tc=20, qid=3)
            self.queue_qos_map2 = self.add_qos_map(self.device, tc=24, qid=4)
            tc_queue_maps = []
            tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.queue_qos_map1))
            tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.queue_qos_map2))

            self.tc_queue_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=63)

            self.queue_list = self.attribute_get(self.port1,
                             SWITCH_PORT_ATTR_QUEUE_HANDLES)

            queue_counter = self.object_counters_get(self.queue_list[3].oid)
            init_pkts = queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count

            print("pass packets w/ DSCP 1 to Queue 3")
            pkt_count = 2
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            queue_counter = self.object_counters_get(self.queue_list[3].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == init_pkts + pkt_count)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_BYTES].count != 0)

            time.sleep(3)
            cntr_ids = [SWITCH_QUEUE_COUNTER_ID_STAT_BYTES]
            self.client.object_counters_clear(self.queue_list[3].oid, cntr_ids)
            queue_counter = self.object_counters_get(self.queue_list[3].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_BYTES].count == 0)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)

            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            time.sleep(3)
            cntr_ids = [SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS]
            self.client.object_counters_clear(self.queue_list[3].oid, cntr_ids)
            queue_counter = self.object_counters_get(self.queue_list[3].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_BYTES].count != 0)

            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            self.client.object_counters_clear_all(self.queue_list[3].oid)

            queue_counter = self.object_counters_get(self.queue_list[3].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == 0)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_BYTES].count == 0)

            for i in range(SWITCH_QUEUE_COUNTER_ID_DROPPED_PACKETS, SWITCH_QUEUE_COUNTER_ID_MAX):
                cntr_ids = [i]
                self.client.object_counters_clear(self.queue_list[3].oid, cntr_ids)
                self.assertEqual(self.status(), 0, "Clear of counter id {} failed".format(i))

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.clean_to_object(self.queue_qos_map1)

    def DscpIcosPpgStatsClearTest(self):
        print("Try DSCP to iCos and iCos to PPG test with stats clear")

        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)==0):
                return

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)

            #iCos=3, ppg_index=3
            #iCos=4, ppg_index=4
            self.ingress_port_ppg_3 = self.CreatePortPPG(self.port0, 3)
            self.ingress_port_ppg_4 = self.CreatePortPPG(self.port0, 4)

            icos_pg_map1 = self.add_qos_map(self.device, icos=3, ppg=3)
            icos_pg_map2 = self.add_qos_map(self.device, icos=4, ppg=4)
            icos_ppg_maps=[]
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map1))
            icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=icos_pg_map2))

            self.icos_to_ppg_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)

            tc_qos_map1 = self.add_qos_map(self.device, tc=20, icos=3)
            tc_qos_map2 = self.add_qos_map(self.device, tc=24, icos=4)
            tc_icos_maps = []
            tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map1))
            tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=tc_qos_map2))

            self.tc_icos_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=tc_icos_maps)

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_to_ppg_map_ingress)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=63)

            ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            init_pkts = ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count

            pkt_count = 2

            print("pass packets w/ DSCP 1 to iCos 3/PPG 3")
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count + init_pkts)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_BYTES].count != 0)

            cntr_ids = [SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_BYTES]
            self.client.object_counters_clear(self.ingress_port_ppg_3, cntr_ids)

            ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count + init_pkts)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_BYTES].count == 0)

            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            time.sleep(3)
            cntr_ids = [SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS]
            self.client.object_counters_clear(self.ingress_port_ppg_3, cntr_ids)

            ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_BYTES].count != 0)

            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            self.client.object_counters_clear_all(self.ingress_port_ppg_3)

            ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == 0)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_BYTES].count == 0)

            for i in range(SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS, SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_MAX):
                cntr_ids = [i]
                self.client.object_counters_clear(self.ingress_port_ppg_3, cntr_ids)
                self.assertEqual(self.status(), 0, "Clear of counter id {} failed".format(i))

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.clean_to_object(self.ingress_port_ppg_3)

    def setUp(self):
        self.configure()
        print()
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

        self.lag0 = self.add_lag(self.device)
        self.lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='172.20.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_tos=4, # dscp 1
            ip_ttl=64)

        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='172.20.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_tos=4, # dscp 1
            ip_ttl=63)
        #send packet before applying QoS configuration

        send_packet(self, self.devports[0], pkt)
        verify_packet(self, exp_pkt, self.devports[1])

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) != 0):
                self.configureDscpMaps()
                self.DscpIcosTest()
                self.DscpQueueTest()
                self.DscpIcosQueueTest()
                # The below version is specifically for warm/fast testing
                #self.DscpIcosQueueTest2Configure()
                #self.DscpIcosQueueTest2TrafficTest()
                #self.DscpIcosQueueTest2Cleanup()
                self.DscpIcosQueueUpdateDeletePortTest()
                self.DscpIcosQueueUpdateDeleteLagPortTest()
                self.DscpIcosPpgUpdatePortTest()
                self.DscpIcosPpgUpdateLagPortTest()
                self.DscpTcQueueStatsClearTest()
                self.DscpIcosPpgStatsClearTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, 0)
        self.cleanup()

@group('qos')
class QosMapAttrUpdateTest(ApiHelper):
    def runTest(self):
        print()

        self.configure()

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

        # configure port0 and port1 for dscp testing
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.port3)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port2)

        print("Configuring PCP maps")
        self.pcp_tc_map1 = self.add_qos_map(self.device, pcp=1, tc=20)
        self.pcp_tc_map2 = self.add_qos_map(self.device, pcp=2, tc=24)
        pcp_tc_maps = []
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.pcp_tc_map1))
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.pcp_tc_map2))
        self.pcp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC, qos_map_list=pcp_tc_maps)
        self.assertEqual(self.status(), 0)

        self.tc_pcp_map1 = self.add_qos_map(self.device, tc=20, pcp=4)
        self.tc_pcp_map2 = self.add_qos_map(self.device, tc=24, pcp=5)
        tc_pcp_maps = []
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_pcp_map1))
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_pcp_map2))
        self.tc_pcp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_PCP, qos_map_list=tc_pcp_maps)
        self.assertEqual(self.status(), 0)
        print("Configuring DSCP maps")
        self.dscp_tc_map1 = self.add_qos_map(self.device, dscp=1, tc=20)
        self.dscp_tc_map2 = self.add_qos_map(self.device, dscp=2, tc=24)
        dscp_tc_maps = []
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_map1))
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_map2))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)

        self.tc_dscp_map1 = self.add_qos_map(self.device, tc=20, dscp=9)
        self.tc_dscp_map2 = self.add_qos_map(self.device, tc=24, dscp=10)
        tc_dscp_maps = []
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_dscp_map1))
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_dscp_map2))
        self.tc_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)
        self.assertEqual(self.status(), 0)
        print("Configuring Traffic Class")
        self.tc_icos_map1 = self.add_qos_map(self.device, tc=20, icos=3)
        self.tc_icos_map2 = self.add_qos_map(self.device, tc=24, icos=4)
        tc_icos_maps = []
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_icos_map1))
        tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_icos_map2))
        self.tc_icos_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=tc_icos_maps)
        self.assertEqual(self.status(), 0)

        self.tc_queue_map1 = self.add_qos_map(self.device, tc=20, qid=4)
        self.tc_queue_map2 = self.add_qos_map(self.device, tc=24, qid=5)
        tc_queue_maps = []
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_queue_map1))
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_queue_map2))
        self.tc_qid_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)
        self.assertEqual(self.status(), 0)

        print("Configuring icos to ppg maps")
        self.icos_ppg_map1 = self.add_qos_map(self.device, icos=3, ppg=3)
        self.icos_ppg_map2 = self.add_qos_map(self.device, icos=4, ppg=4)
        icos_ppg_maps = []
        icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.icos_ppg_map1))
        icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.icos_ppg_map2))
        self.icos_ppg_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=icos_ppg_maps)
        self.assertEqual(self.status(), 0)

        self.ingress_port_ppg_3 = self.add_port_priority_group(self.device, port_handle=self.port0, ppg_index=3)
        self.ingress_port_ppg_4 = self.add_port_priority_group(self.device, port_handle=self.port0, ppg_index=4)
        self.ingress_port_ppg_5 = self.add_port_priority_group(self.device, port_handle=self.port0, ppg_index=5)

        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) != 0):
                self.TcToIcosQosMapAttrUpdateTest()
                self.TcToQueueQosMapAttrUpdateTest()
                self.DscpQosMapAttrUpdateRewriteTest()
                self.PcpQosMapAttrUpdateRewriteTest()

        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, 0)
            self.cleanup()

    def TcToIcosQosMapAttrUpdateTest(self):
        print("TcToIcosQosMapAttrUpdateTest()")
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS)==0):
                return
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_ppg_map_ingress)

            self.client.object_counters_clear_all(self.ingress_port_ppg_3)
            self.client.object_counters_clear_all(self.ingress_port_ppg_4)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=63)

            pkt_count = 2

            # dscp 1 -> tc 20 -> icos 3 -> ppg 3
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            print("pass packet w/ dscp value 1 -> tc 20 -> icos 3 -> ppg 3")

            # dscp 1 -> tc 20 -> icos 4 -> ppg 4
            self.attribute_set(self.tc_icos_map1, SWITCH_QOS_MAP_ATTR_ICOS, 4)
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            ppg_counter = self.object_counters_get(self.ingress_port_ppg_4)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)
            print("pass packet w/ dscp value 1 -> tc 20 -> icos 4 -> ppg 4")

            # ppg 3 counter still should contain packets count from the previous run
            ppg_counter = self.object_counters_get(self.ingress_port_ppg_3)
            self.assertTrue(ppg_counter[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count == pkt_count)

        finally:
            self.attribute_set(self.tc_icos_map1, SWITCH_QOS_MAP_ATTR_ICOS, 3)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            pass

    def TcToQueueQosMapAttrUpdateTest(self):
        print("TcToQueueQosMapAttrUpdateTest()")
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)

            self.queue_list = self.attribute_get(self.port1, SWITCH_PORT_ATTR_QUEUE_HANDLES)
            num_queues = len(self.queue_list)
            print("Total number of queues/port is: %d"%(num_queues))
            if (self.arch == 'tofino'):
              self.assertTrue(num_queues == 8, "Tofino1 supports upto 8 queues/port")
            else:
              self.assertTrue(num_queues == 16, "Tofino2/3 supports upto 16 queues/port")

            self.client.object_counters_clear_all(self.queue_list[3].oid)
            self.client.object_counters_clear_all(self.queue_list[4].oid)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=63)

            pkt_count = 2

            # dscp 1 -> tc 20 -> queue 4
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

            queue_counter = self.object_counters_get(self.queue_list[4].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)
            print("pass packet w/ mapped dscp value 1 -> tc 20 -> queue 4")

            # dscp 1 -> tc 20 -> queue 3
            self.attribute_set(self.tc_queue_map1, SWITCH_QOS_MAP_ATTR_QID, 3)
            for i in range(0, pkt_count):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> tc 20 -> queue 3")

            queue_counter = self.object_counters_get(self.queue_list[3].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)
            # queue 4 counter still should contain packets count from the previous run
            queue_counter = self.object_counters_get(self.queue_list[4].oid)
            self.assertTrue(queue_counter[SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS].count == pkt_count)

        finally:
            self.attribute_set(self.tc_queue_map1, SWITCH_QOS_MAP_ATTR_QID, 4)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            pass

    def DscpQosMapAttrUpdateRewriteTest(self):
        print("DscpQosMapAttrUpdateRewriteTest()")
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            self.attribute_set(self.dscp_tc_map1, SWITCH_QOS_MAP_ATTR_TC, 20)
            self.attribute_set(self.tc_dscp_map2, SWITCH_QOS_MAP_ATTR_DSCP, 10)

            print("pass packet w/ mapped dscp value 1 -> 9")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=36, # dscp 9
                ip_ttl=63)

            # dscp 1 -> tc 20 -> dscp 9
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 9")

            # dscp 1 -> tc 24 -> dscp 10
            self.attribute_set(self.dscp_tc_map1, SWITCH_QOS_MAP_ATTR_TC, 24)
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IP].tos = 40
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 10")

            # dscp 1 -> tc 24 -> dscp 8
            exp_pkt3 = exp_pkt.copy()
            exp_pkt3[IP].tos = 32
            self.attribute_set(self.tc_dscp_map2, SWITCH_QOS_MAP_ATTR_DSCP, 8)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt3, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 8")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def PcpQosMapAttrUpdateRewriteTest(self):
        print("PcpQosMapAttrUpdateRewriteTest()")
        try:
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, self.pcp_tc_map_ingress)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, self.tc_pcp_map_egress)
            self.attribute_set(self.pcp_tc_map1, SWITCH_QOS_MAP_ATTR_TC, 20)
            self.attribute_set(self.tc_pcp_map2, SWITCH_QOS_MAP_ATTR_PCP, 5)

            print("Validate with IPV4 packet")
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=1,
                ip_ttl=64)
            exp_pkt = pkt.copy()
            exp_pkt[Dot1Q].prio = 4
            # pcp 1 -> tc 20 -> pcp 4
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            print("pass packet w/ mapped pcp value 1 -> 4")

            # pcp 1 -> tc 24 -> pcp 5
            self.attribute_set(self.pcp_tc_map1, SWITCH_QOS_MAP_ATTR_TC, 24)
            exp_pkt2 = pkt.copy()
            exp_pkt2[Dot1Q].prio = 5
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt2, self.devports[3])
            print("pass packet w/ mapped pcp value 1 -> 5")

            # pcp 1 -> tc 24 -> pcp 6
            exp_pkt3 = pkt.copy()
            exp_pkt3[Dot1Q].prio = 6
            self.attribute_set(self.tc_pcp_map2, SWITCH_QOS_MAP_ATTR_PCP, 6)
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt3, self.devports[3])
            print("pass packet w/ mapped pcp value 1 -> 6")

        finally:
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            pass


@group('qos')
class QosAclTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            return

        # configure port0 and port1 for dscp testing
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route2 = self.add_route(self.device, ip_prefix='172.30.30.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route3 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        print("Configuring DSCP maps")
        self.dscp_tc_map1 = self.add_qos_map(self.device, dscp=1, tc=20)
        self.dscp_tc_map2 = self.add_qos_map(self.device, dscp=2, tc=24)
        dscp_tc_maps = []
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_map1))
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_map2))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)

        self.tc_dscp_map1 = self.add_qos_map(self.device, tc=20, dscp=9)
        self.tc_dscp_map2 = self.add_qos_map(self.device, tc=24, dscp=10)
        tc_dscp_maps = []
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_dscp_map1))
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_dscp_map2))
        self.tc_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)
        self.assertEqual(self.status(), 0)

        self.acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.acl_entry = self.add_acl_entry(self.device,
            dst_ip='172.20.10.1',
            dst_ip_mask='255.255.255.255',
            action_set_tc=20,
            table_handle=self.acl_table)
        self.acl_entry1 = self.add_acl_entry(self.device,
            dst_ip='172.30.30.1',
            dst_ip_mask='255.255.255.255',
            action_set_tc=24,
            table_handle=self.acl_table)
        self.ipv6_acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            action_set_tc=20,
            table_handle=self.acl_table)

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            return
        try:
            self.DscpRewriteNoAclTest()
            self.DscpRewriteQosMapAndAclTest()
            self.TcUpdateDscpRewriteTest()
            self.TcUpdateIpv6DscpRewriteTest()
            self.SetColorAndMeterConfigTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def DscpRewriteNoAclTest(self):
        print("DscpRewriteNoAclTest()")
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("pass packet w/ mapped dscp value 1 -> 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=63)

            if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) != 0):
                # ingress qos map is enabled so dscp will be rewritten
                exp_pkt[IP].tos = 9 << 2 # dscp 9

            # qos maps shouldn't have any effect
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 1")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def DscpRewriteQosMapAndAclTest(self):
        print("DscpRewriteQosMapAndAclTest()")
        # Test the case when both ingress QOS map and QOS ACL are enabled
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("pass packet w/ mapped dscp value 1 -> 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> ingress qos_map -> tc 20 -> dscp 9
            # dscp 1 -> QOS ACL -> tc 24 -> dscp 10
            # ACL entry should take precedence over qos-map
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 10")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def TcUpdateDscpRewriteTest(self):
        print("TcUpdateDscpRewriteTest()")
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("pass packet w/ mapped dscp value 0 -> 9")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=0,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=9 << 2, # dscp 9
                ip_ttl=63)

            #dscp 0 -> tc 20 -> dscp 9
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 9")

            # dscp 0 -> tc 24 -> dscp 10
            self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IP].tos = 10 << 2 # dscp 10
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass packet w/ mapped dscp value 0 -> 10")

            exp_pkt3 = exp_pkt.copy()
            exp_pkt3[IP].tos = 0
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt3, self.devports[1])
            print("pass packet w/ mapped dscp value 0 -> 0")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def TcUpdateIpv6DscpRewriteTest(self):
        print("TcUpdateIpv6DscpRewriteTest()")
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            print("pass ipv6 packet w/ mapped dscp value 0 -> 9")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=0,
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=9,
                ipv6_hlim=63)

            # dscp 0 -> tc 20 -> dscp 9
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass ipv6 packet w/ mapped dscp value 1 -> 9")

            # dscp 0 -> tc 24 -> dscp 10
            self.attribute_set(self.ipv6_acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IPv6].tc = 10 << 2 # dscp 10
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass ipv6 packet w/ mapped dscp value 0 -> 10")

            exp_pkt3 = exp_pkt.copy()
            exp_pkt3[IPv6].tc = 0
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt3, self.devports[1])
            print("pass ipv6 packet w/ mapped dscp value 0 -> 0")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def SetColorAndMeterConfigTest(self):
        print("SetColorAndMeterConfigTest()")
        try:
            self.meter = self.add_meter(self.device,
                mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
                pbs=16,cbs=8,cir=1000,pir=1000,
                type=SWITCH_METER_ATTR_TYPE_BYTES,
                color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
            self.assertEqual(self.status(), 0)

            self.meter2 = self.add_meter(self.device,
                mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
                pbs=16,cbs=8,cir=2000,pir=2000,
                type=SWITCH_METER_ATTR_TYPE_BYTES,
                color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
            self.assertEqual(self.status(), 0)

            acl_entry2 = self.add_acl_entry(self.device,
                dst_ip='172.40.40.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
                action_meter_handle=self.meter,
                table_handle=self.acl_table)
            self.assertEqual(self.status(), 0)

            self.attribute_set(acl_entry2, SWITCH_ACL_ENTRY_ATTR_ACTION_METER_HANDLE, self.meter2)
            self.assertEqual(self.status(), 0)

            acl_entry3 = self.add_acl_entry(self.device,
                dst_ip='172.50.50.1',
                dst_ip_mask='255.255.255.255',
                action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                table_handle=self.acl_table)
            self.assertEqual(self.status(), 0)

            self.attribute_set(acl_entry3, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_RED)
            self.assertEqual(self.status(), 0)
        finally:
            pass

class PortQosMapUpdateTest(ApiHelper):
    '''
    DSCP/PCP ---> TC | TC ---> ICOS | ICOS ---> PPG
    '''
    baseConfigQosMappings = [
        (0, 0), (1, 1), (2, 2), (3, 3),
       (4, 4), (5, 5), (6, 6), (7, 7)
        ]
    baseTestConfigMappings = [
        (0, 0), (1, 1), (2, 2), (3, 3)
        ]
    testConfigMappings = [
        [(4, 4), (5, 5), (6, 6), (7, 7)], #No Overlap
       [(0, 4), (1, 5), (2, 6), (3, 7)],  #All keys Exist, but key values different i.e. Full Update
       [(0, 0), (1, 1)], #Some keys Exist, Map to Same values i.e. Partial Delete
       [(0, 0), (1, 4)], #Some keys Exist, Some keys same value some keys different values i.e Partial Delete + Partial Update
       [(0, 0), (1, 1), (4, 3), (5, 6)], #Some old keys & some new keys, old keys same value, new keys new values
       [(0, 0), (1, 1), (4, 0), (5, 1)] #Some old and some new keys, old keys same value. new keys map to samevalues
        ]

    # Dictionary for qos ingress map types to port qos attrs
    qos_helper_dict = {
        SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC: { 'port_attr': SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 'port_attr_name': 'ingress_qos_pcp_group', 'qos_map_name':'pcp_to_tc'},
        SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC: { 'port_attr': SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 'port_attr_name': 'ingress_qos_dscp_tos_group', 'qos_map_name': 'dscp_to_tc'},
        SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS: { 'port_attr':SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 'port_attr_name': 'tc_qos_map_handle', 'qos_map_name': 'tc_to_icos'},
        SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG: { 'port_attr': SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 'port_attr_name': 'icos_ppg_qos_map_handle', 'qos_map_name': 'icos_to_ppg'}
    }

    #  Helper functions for creating qos_maps from qos mappings
    def create_dscp_tc_map(self, dscp_tc_maps):
        for ckey, cvalue in dscp_tc_maps:
            yield self.add_qos_map(self.device, dscp=ckey, tc=cvalue)
    def create_pcp_tc_map(self, pcp_tc_maps):
        for ckey, cvalue in pcp_tc_maps:
            yield self.add_qos_map(self.device, pcp=ckey, tc=cvalue)
    def create_tc_icos_map(self, tc_icos_maps):
        for ckey, cvalue in tc_icos_maps:
            yield self.add_qos_map(self.device, tc=ckey, icos=cvalue)
    def create_icos_ppg_map(self, icos_ppg_maps):
        for ckey, cvalue in icos_ppg_maps:
            yield self.add_qos_map(self.device, icos=ckey, ppg=cvalue)

    def create_qos_test_config(self, mappings):
        test_config_maps=[]
        if self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC:
            for qos_map_object in self.create_dscp_tc_map(mappings):
                test_config_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        elif self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC:
            for qos_map_object in self.create_pcp_tc_map(mappings):
                test_config_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        elif self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS:
            for qos_map_object in self.create_tc_icos_map(mappings):
                test_config_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        elif self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG:
            for qos_map_object in self.create_icos_ppg_map(mappings):
                test_config_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map_object))
        test_config_handle = self.add_qos_map_ingress(self.device,
                type=self.test_qos_map_type,
                qos_map_list=test_config_maps)
        return (test_config_maps, test_config_handle)

    def setUp(self, test_qos_map_type):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)==0):
            print("Ingress QoS Map feature not enabled, skipping test")
            return

        self.port_test_attr = PortQosMapUpdateTest.qos_helper_dict[test_qos_map_type]['port_attr']
        self.port_test_attr_name = PortQosMapUpdateTest.qos_helper_dict[test_qos_map_type]['port_attr_name']
        self.test_qos_map_type = test_qos_map_type
        self.test_qos_map_type_name = PortQosMapUpdateTest.qos_helper_dict[test_qos_map_type]['qos_map_name']
        self.ppg_handles = []
        self.expected_ppg_stats = [0]*8
        self.test_l3_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='40.40.1.20',
            ip_id=105,
            ip_dscp=0,
            ip_ttl=64)
        self.test_l2_pkt = simple_arp_packet(pktlen=60, vlan_vid=10, vlan_pcp=0)
        if self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC:
            self.test_pkt = self.test_l2_pkt
        else:
            self.test_pkt = self.test_l3_pkt
        self.test_port = self.port0
        # create device level ingress buffer profile.
        self.ingress_buffer_pool_handle  = self.add_buffer_pool(self.device,
                         direction=SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS,
                         threshold_mode=SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE_DYNAMIC,
                         pool_size=1024)
        self.device_ingress_buffer_profile = self.add_buffer_profile(self.device,
                           threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
                           threshold=256,
                           buffer_size=2048,
                           buffer_pool_handle=self.ingress_buffer_pool_handle)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, self.device_ingress_buffer_profile)
        self.default_ppg_handle = self.attribute_get(self.port0, SWITCH_PORT_ATTR_DEFAULT_PPG)
        self.expected_default_ppg_stat = 0
        # Create ppgs for test port
        for ppg_index in range(8):
            self.ppg_handles.append(self.add_port_priority_group(self.device, ppg_index = ppg_index, port_handle =
                self.port0))

        #Setup base forwarding config
        self.vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle = self.vlan10, member_handle = self.port0)
        self.vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle = self.vlan10, member_handle = self.port1)

        #Setup Base Qos Config
        self.dscp_tc_maps = []
        self.pcp_tc_maps = []
        self.tc_icos_maps = []
        self.icos_ppg_maps = []
        if self.port_test_attr != SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP:
            for qos_map_object in self.create_dscp_tc_map(PortQosMapUpdateTest.baseConfigQosMappings):
                self.dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        if self.port_test_attr != SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP:
            for qos_map_object in self.create_pcp_tc_map(PortQosMapUpdateTest.baseConfigQosMappings):
                self.pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        if self.port_test_attr != SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE:
            for qos_map_object in self.create_tc_icos_map(PortQosMapUpdateTest.baseConfigQosMappings):
                self.tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        if self.port_test_attr != SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE:
            for qos_map_object in self.create_icos_ppg_map(PortQosMapUpdateTest.baseConfigQosMappings):
                self.icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        if self.dscp_tc_maps:
            self.dscp_tc_maps_handle = self.add_qos_map_ingress(self.device, type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC,
                qos_map_list=self.dscp_tc_maps)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_maps_handle)
        if self.pcp_tc_maps:
            self.pcp_tc_maps_handle = self.add_qos_map_ingress(self.device, type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC,
                qos_map_list=self.pcp_tc_maps)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, self.pcp_tc_maps_handle)
        if self.tc_icos_maps:
            self.tc_icos_maps_handle = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=self.tc_icos_maps)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, self.tc_icos_maps_handle)
        if self.icos_ppg_maps:
            self.icos_ppg_maps_handle = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=self.icos_ppg_maps)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_ppg_maps_handle)

        #Setup Base Test Config
        self.base_test_config_maps, self.base_test_config_handle = self.create_qos_test_config(
                PortQosMapUpdateTest.baseTestConfigMappings)

    def verifyQosMapping(self, mapping=[]):
        print("QoS Mapping Verification unimplemented")

    def trafficQosMappingTest(self, mapping=[]):
        if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS) == 0):
            print("PPG stats is not enabled, return")
            return

        if mapping:
            pkt_count=1
            exp_stats = [0]*len(self.ppg_handles)
            post_test_stats = [0]*len(self.ppg_handles)
            num_pkts = []
            #for ppg_index in range(len(self.ppg_handles)):
            #    counters = self.client.object_counters_get(self.ppg_handles[ppg_index])
            #    exp_stats[ppg_index] = counters[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count
            for key, v in mapping:
                if self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC:
                    self.test_pkt[Ether][Dot1Q].prio = key
                else:
                    self.test_pkt[IP].tos = key << 2
                for count in range(pkt_count):
                    send_packet(self, self.devports[0], self.test_pkt)
                    self.expected_ppg_stats[v]+=1
                num_pkts.append(pkt_count)
                pkt_count+=1
            print("Waiting for 4 sec... before collecting stats")
            time.sleep(4)
            for ppg_index in range(len(self.ppg_handles)):
                counters = self.client.object_counters_get(self.ppg_handles[ppg_index])
                post_test_stats[ppg_index] = counters[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count
            self.assertTrue(post_test_stats == self.expected_ppg_stats, "PPG stats {} do not match expected PPG stats {} for qos"
            "mapping {}, packet per mapping list {}".format(post_test_stats, self.expected_ppg_stats, mapping, num_pkts))

        # commented since ppg stats for default ppg not accounted for in dataplane
        # Verify Unmapped qos traffic gets accounted for in default ppg. We basically have two sets of test priority
        # values, [0, 1, 2, 3] and [4, 5, 6, 7]. We use the first value of the mapping list to identify which test set we
        # are dealing with and use the other set for verifying stats for default ppg.
        #if not mapping:
        #    def_prio = [0, 1, 2, 3, 4, 5, 6, 7]
        #elif(mapping[0] == 0):
        #    def_prio = [4, 5, 6, 7]
        #else:
        #    def_prio = [0, 1, 2, 3]
        #counters = self.client.object_counters_get(self.default_ppg_handle)
        #exp_def_ppg_stats = counters[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count
        #for prio in def_prio:
        #    if self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC:
        #        self.test_pkt[Ether][Dot0Q].prio = prio
        #    else:
        #        self.test_pkt[IP].tos = prio << 2
        #    send_packet(self, self.devports[0], self.test_pkt)
        #    exp_def_ppg_stats += 1
        #print("Waiting for 4 sec... before collecting stats")
        #time.sleep(4)
        #counters = self.client.object_counters_get(self.default_ppg_handle)
        #def_ppg_stats = counters[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count
        #self.assertTrue(def_ppg_stats == exp_def_ppg_stats, "Default PPG stats {} do not match expected stats {} for"
        #         "priorities {}, packet per priority list {}".format(def_ppg_stats, exp_def_ppg_stats, def_prio, num_pkts))

    def portQosMapAttrSetTest(self):
        print("\nportQosMapAttrSetTest()")
        print("Port Attr: {} Set Test".format(self.port_test_attr_name))
        try:
            print("Setting attr to {}:{}".format(self.test_qos_map_type_name, PortQosMapUpdateTest.baseTestConfigMappings))
            self.attribute_set(self.test_port, self.port_test_attr, self.base_test_config_handle)
            self.verifyQosMapping(PortQosMapUpdateTest.baseTestConfigMappings)
            self.trafficQosMappingTest(PortQosMapUpdateTest.baseTestConfigMappings)
            print("OK")

            print("Setting attr to {}:NULL".format(self.test_qos_map_type_name))
            self.attribute_set(self.test_port, self.port_test_attr, 0)
            self.verifyQosMapping()
            self.trafficQosMappingTest()
            print("OK")
        finally:
            self.attribute_set(self.test_port, self.port_test_attr, 0)

    def portQosMapAttrUpdateTest1(self):
        print ("\nportQosMapAttrUpdateTest1()")
        print("Port Attr: {} Update Test".format(self.port_test_attr_name))
        print("Update from {0}:{1} => {0}:{1}".format(self.test_qos_map_type_name, PortQosMapUpdateTest.baseTestConfigMappings))
        try:
            #  alloct his before to keep the cleanup simple
            copy_of_base_test_config_handle = self.add_qos_map_ingress(self.device, type=self.test_qos_map_type,
                qos_map_list=self.base_test_config_maps)

            print("Setting attr to {}:{}".format(self.test_qos_map_type_name, PortQosMapUpdateTest.baseTestConfigMappings))
            self.attribute_set(self.test_port, self.port_test_attr, self.base_test_config_handle)
            self.verifyQosMapping(PortQosMapUpdateTest.baseTestConfigMappings)
            self.trafficQosMappingTest(PortQosMapUpdateTest.baseTestConfigMappings)
            print("OK")

            print("Setting attr to {}:{}".format(self.test_qos_map_type_name, PortQosMapUpdateTest.baseTestConfigMappings))
            self.attribute_set(self.test_port, self.port_test_attr, copy_of_base_test_config_handle)
            self.verifyQosMapping(PortQosMapUpdateTest.baseTestConfigMappings)
            self.trafficQosMappingTest(PortQosMapUpdateTest.baseTestConfigMappings)
            print("OK")
        finally:
            self.attribute_set(self.test_port, self.port_test_attr, 0)
            self.cleanlast()

    def portQosMapAttrUpdateTest(self,mapping):
        print("\nUpdate from {0}:{1} to {0}:{2}".format(self.test_qos_map_type_name, PortQosMapUpdateTest.baseTestConfigMappings,mapping))
        try:
            # Creating this before hand to keep the cleanup simple
            test_config_maps, test_config_handle = self.create_qos_test_config(mapping)

            print("Setting attr to {}:{}".format(self.test_qos_map_type_name, PortQosMapUpdateTest.baseTestConfigMappings))
            self.attribute_set(self.test_port, self.port_test_attr, self.base_test_config_handle)
            self.verifyQosMapping(PortQosMapUpdateTest.baseTestConfigMappings)
            #Commented to speed up test, actual stats verification anyways needs to happen after the update below
            #self.trafficQosMappingTest(PortQosMapUpdateTest.baseTestConfigMappings)
            print("OK")

            print("Setting attr to {}:{}".format(self.test_qos_map_type_name, mapping))
            self.attribute_set(self.test_port, self.port_test_attr, test_config_handle)
            self.verifyQosMapping(mapping)
            self.trafficQosMappingTest(mapping)
            print("OK")
        finally:
            self.attribute_set(self.test_port, self.port_test_attr, 0)
            self.attribute_set(test_config_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            self.verifyQosMapping()
            self.clean_to_object(test_config_maps[0].oid)

    def portQosMapAttrUpdateTest2(self):
        print ("\nportQosMapAttrUpdateTest2()")
        print("Port Attr: {} Update Test".format(self.port_test_attr_name))
        for mapping in PortQosMapUpdateTest.testConfigMappings:
            self.portQosMapAttrUpdateTest(mapping)

    def runPortQosMapUpdateTests(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)==0):
            return
        try:
            self.portQosMapAttrSetTest()
            self.portQosMapAttrUpdateTest1()
            self.portQosMapAttrUpdateTest2()
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)!=0):
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)

            self.attribute_set(self.base_test_config_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])

            if self.dscp_tc_maps:
                self.attribute_set(self.dscp_tc_maps_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            if self.pcp_tc_maps:
                self.attribute_set(self.pcp_tc_maps_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            if self.tc_icos_maps:
                self.attribute_set(self.tc_icos_maps_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            if self.icos_ppg_maps:
                self.attribute_set(self.icos_ppg_maps_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            self.verifyQosMapping()
            self.clean_to_object(self.ppg_handles[0])
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, 0)
        self.cleanup()

@group('qos')
class PortIcosPpgQosMapUpdateTest(PortQosMapUpdateTest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG)

    def verifyQosMapping(self, mapping=[]):
        try:
            print ("Verifying ppg mapping {}".format(mapping))
            # Verify only ppgs having mapping exist in hardware
            mapped_ppgs = set()
            for icos, ppg in mapping:
                mapped_ppgs.add(ppg)
            hw_ppgs = set()
            for index in range(len(self.ppg_handles)):
                if(self.attribute_get(self.ppg_handles[index], SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW)):
                    hw_ppgs.add(index)
            print("Mapped ppgs {}, ppgs created in hw {}".format(mapped_ppgs, hw_ppgs))
            self.assertTrue(mapped_ppgs == hw_ppgs, ("ppgs with mappings {}, ppgs created in hardware {} are not"
                    "same").format(mapped_ppgs, hw_ppgs))
            #Reset ppg stats for the ppgs that were deleted
            for ppg_index in range(len(self.expected_ppg_stats)):
                if ppg_index in hw_ppgs:
                    continue
                else:
                    self.expected_ppg_stats[ppg_index] = 0
        finally:
            pass

    def runTest(self):
        super(self.__class__, self).runPortQosMapUpdateTests()

@group('qos')
class PortDscpTcQosMapUpdateTest(PortQosMapUpdateTest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC)

    def verifyQosMapping(self, mapping=[]):
        pass

    def runTest(self):
        super(self.__class__, self).runPortQosMapUpdateTests()

@group('qos')
class PortPcpTcQosMapUpdateTest(PortQosMapUpdateTest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC)

    def verifyQosMapping(self, mapping=[]):
        pass

    def runTest(self):
        super(self.__class__, self).runPortQosMapUpdateTests()

class QosMapIngressUpdateTest(ApiHelper):
    '''
    This class is the base class for port qos map tests. It provides base qos config for data plane. The tests are
    structured to test one port qos map attr at a time keeping the remaining qos mapping constant throughtout the
    testing. These constant entries are a simple 1:1 mapping for eg. dscp 0 -> tc 0, tc 0 -> icos 0, icos 0 -> ppg0 and
    so on. Base confing involves 8 entries, one for each key value from 0-7. The test qos config is updated as part of
    test, to verify various update scenarios such as partial update, partial delete, full update, full delete etc.

    DSCP/PCP ---> TC | TC ---> ICOS | ICOS ---> PPG
    '''
    baseConfigQosMappings = [
        (0, 0), (1, 1), (2, 2), (3, 3),
       (4, 4), (5, 5), (6, 6), (7, 7)
        ]
    baseTestConfigMappings = [
        (0, 0), (1, 1), (2, 2), (3, 3)
        ]
    testConfigMappings = [
        [(4, 4), (5, 5), (6, 6), (7, 7)], #No Overlap
       [(0, 4), (1, 5), (2, 6), (3, 7)],  #All keys Exist, but key values different i.e. Full Update
       [(0, 0), (1, 1)], #Some keys Exist, Map to Same values i.e. Partial Delete
       [(0, 0), (1, 4)], #Some keys Exist, Some keys same value some keys different values i.e Partial Delete + Partial Update
       [(0, 0), (1, 1), (4, 3), (5, 6)], #Some old keys & some new keys, old keys same value, new keys new values
       [(0, 0), (1, 1), (4, 0), (5, 1)] #Some old and some new keys, old keys same value. new keys map to samevalues
        ]

    # Dictionary for qos ingress map types to port qos attrs
    qos_helper_dict = {
        SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC: { 'port_attr': SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 'map_list': SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, 'port_attr_name': 'ingress_qos_pcp_group', 'qos_map_name':'pcp_to_tc'},
        SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC: { 'port_attr': SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 'map_list': SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, 'port_attr_name': 'ingress_qos_dscp_tos_group', 'qos_map_name': 'dscp_to_tc'},
        SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS: { 'port_attr':SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 'map_list': SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, 'port_attr_name': 'tc_qos_map_handle', 'qos_map_name': 'tc_to_icos'},
        SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG: { 'port_attr': SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 'map_list': SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, 'port_attr_name': 'icos_ppg_qos_map_handle', 'qos_map_name': 'icos_to_ppg'}
    }

    #Helper function for creating qos_maps from qos mappings
    def create_dscp_tc_map(self, dscp_tc_maps):
        for ckey, cvalue in dscp_tc_maps:
            yield self.add_qos_map(self.device, dscp=ckey, tc=cvalue)
    def create_pcp_tc_map(self, pcp_tc_maps):
        for ckey, cvalue in pcp_tc_maps:
            yield self.add_qos_map(self.device, pcp=ckey, tc=cvalue)
    def create_tc_icos_map(self, tc_icos_maps):
        for ckey, cvalue in tc_icos_maps:
            yield self.add_qos_map(self.device, tc=ckey, icos=cvalue)
    def create_icos_ppg_map(self, icos_ppg_maps):
        for ckey, cvalue in icos_ppg_maps:
            yield self.add_qos_map(self.device, icos=ckey, ppg=cvalue)

    def create_qos_test_config(self, mappings):
        test_config_maps=[]
        if self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC:
            for qos_map_object in self.create_dscp_tc_map(mappings):
                test_config_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        elif self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC:
            for qos_map_object in self.create_pcp_tc_map(mappings):
                test_config_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        elif self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS:
            for qos_map_object in self.create_tc_icos_map(mappings):
                test_config_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        elif self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG:
            for qos_map_object in self.create_icos_ppg_map(mappings):
                test_config_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map_object))
        return test_config_maps

    def setUp(self, test_qos_map_type):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)==0):
            print("Ingress QoS Map feature not enabled, skipping test")
            return

        self.port_test_attr =  QosMapIngressUpdateTest.qos_helper_dict[test_qos_map_type]['port_attr']
        self.port_test_attr_name =  QosMapIngressUpdateTest.qos_helper_dict[test_qos_map_type]['port_attr_name']
        self.test_qos_map_type = test_qos_map_type
        self.test_qos_map_type_name =  QosMapIngressUpdateTest.qos_helper_dict[test_qos_map_type]['qos_map_name']
        self.test_qos_map_list =  QosMapIngressUpdateTest.qos_helper_dict[test_qos_map_type]['map_list']
        self.ppg_handles = []
        self.expected_ppg_stats = [0]*8
        self.test_l3_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='40.40.1.20',
            ip_id=105,
            ip_dscp=0,
            ip_ttl=64)
        self.test_l2_pkt = simple_arp_packet(pktlen=60, vlan_vid=10, vlan_pcp=0)
        if self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC:
            self.test_pkt = self.test_l2_pkt
        else:
            self.test_pkt = self.test_l3_pkt
        self.test_port = self.port0
        # create device level ingress buffer profile.
        self.ingress_buffer_pool_handle  = self.add_buffer_pool(self.device,
                         direction=SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS,
                         threshold_mode=SWITCH_BUFFER_POOL_ATTR_THRESHOLD_MODE_DYNAMIC,
                         pool_size=1024)
        self.device_ingress_buffer_profile = self.add_buffer_profile(self.device,
                           threshold_mode=SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC,
                           threshold=256,
                           buffer_size=2048,
                           buffer_pool_handle=self.ingress_buffer_pool_handle)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, self.device_ingress_buffer_profile)
        self.default_ppg_handle = self.attribute_get(self.port0, SWITCH_PORT_ATTR_DEFAULT_PPG)
        self.expected_default_ppg_stat = 0
        # Create ppgs for test port
        for ppg_index in range(8):
            self.ppg_handles.append(self.add_port_priority_group(self.device, ppg_index = ppg_index, port_handle =
                self.port0))

        #Setup base forwarding config
        self.vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle = self.vlan10, member_handle = self.port0)
        self.vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle = self.vlan10, member_handle = self.port1)

        #Setup Base Qos Config
        self.dscp_tc_maps = []
        self.pcp_tc_maps = []
        self.tc_icos_maps = []
        self.icos_ppg_maps = []
        if self.port_test_attr != SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP:
            for qos_map_object in self.create_dscp_tc_map(QosMapIngressUpdateTest.baseConfigQosMappings):
                self.dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        if self.port_test_attr != SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP:
            for qos_map_object in self.create_pcp_tc_map(QosMapIngressUpdateTest.baseConfigQosMappings):
                self.pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        if self.port_test_attr != SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE:
            for qos_map_object in self.create_tc_icos_map(QosMapIngressUpdateTest.baseConfigQosMappings):
                self.tc_icos_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        if self.port_test_attr != SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE:
            for qos_map_object in self.create_icos_ppg_map(QosMapIngressUpdateTest.baseConfigQosMappings):
                self.icos_ppg_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                    oid=qos_map_object))
        if self.dscp_tc_maps:
            self.dscp_tc_maps_handle = self.add_qos_map_ingress(self.device, type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC,
                qos_map_list=self.dscp_tc_maps)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_maps_handle)
        if self.pcp_tc_maps:
            self.pcp_tc_maps_handle = self.add_qos_map_ingress(self.device, type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC,
                qos_map_list=self.pcp_tc_maps)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, self.pcp_tc_maps_handle)
        if self.tc_icos_maps:
            self.tc_icos_maps_handle = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_ICOS, qos_map_list=self.tc_icos_maps)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, self.tc_icos_maps_handle)
        if self.icos_ppg_maps:
            self.icos_ppg_maps_handle = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG, qos_map_list=self.icos_ppg_maps)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, self.icos_ppg_maps_handle)

        #Setup Base Test Config
        self.base_test_config_maps = self.create_qos_test_config(
                QosMapIngressUpdateTest.baseTestConfigMappings)
        self.test_qos_map_ingress_handle = self.add_qos_map_ingress(self.device,
                type=self.test_qos_map_type, qos_map_list=[])
        self.attribute_set(self.test_port, self.port_test_attr, self.test_qos_map_ingress_handle)

    def verifyQosMapping(self, mapping=[]):
        print("QoS Mapping Verification unimplemented")

    def trafficQosMappingTest(self, mapping=[]):
        if (self.client.is_feature_enable(SWITCH_FEATURE_PPG_STATS) == 0):
            print("PPG stats is not enabled, return")
            return

        if mapping:
            pkt_count=1
            exp_stats = [0]*len(self.ppg_handles)
            post_test_stats = [0]*len(self.ppg_handles)
            num_pkts = []
            #for ppg_index in range(len(self.ppg_handles)):
            #    counters = self.client.object_counters_get(self.ppg_handles[ppg_index])
            #    self.expected_ppg_stats[ppg_index] += counters[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count
            for key, v in mapping:
                if self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC:
                    self.test_pkt[Ether][Dot1Q].prio = key
                else:
                    self.test_pkt[IP].tos = key << 2
                for count in range(pkt_count):
                    send_packet(self, self.devports[0], self.test_pkt)
                    self.expected_ppg_stats[v]+=1
                num_pkts.append(pkt_count)
                pkt_count+=1
            print("Waiting for 4 sec... before collecting stats")
            time.sleep(4)
            for ppg_index in range(len(self.ppg_handles)):
                counters = self.client.object_counters_get(self.ppg_handles[ppg_index])
                post_test_stats[ppg_index] = counters[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count
            print("Verifying PPG stats for qos mapping:{}, packet per mapping:{}".format(mapping, num_pkts))
            self.assertTrue(post_test_stats == self.expected_ppg_stats, "PPG stats {} do not match expected PPG stats {} for qos"
            "mapping {}, packet per mapping list {}".format(post_test_stats, self.expected_ppg_stats, mapping, num_pkts))
            print("OK")

        if self.test_qos_map_type != SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG:
            return
        # Verify Unmapped qos traffic gets accounted for in default ppg. We basically have two sets of test priority
        # values, [0, 1, 2, 3] and [4, 5, 6, 7]. We use the first value of the mapping list to identify which test set we
        # are dealing with and use the other set for verifying stats for default ppg.
        def_prio = [0, 1, 2, 3, 4, 5, 6 ,7]
        if mapping:
            for icos_ppg_map in mapping:
                def_prio.remove(icos_ppg_map[0])

        counters = self.client.object_counters_get(self.default_ppg_handle)
        exp_def_ppg_stats = counters[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count
        for prio in def_prio:
            if self.test_qos_map_type == SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC:
                self.test_pkt[Ether][Dot1Q].prio = prio
            else:
                self.test_pkt[IP].tos = prio << 2
            send_packet(self, self.devports[0], self.test_pkt)
            exp_def_ppg_stats += 1
        print("Waiting for 4 sec... before collecting stats")
        time.sleep(4)
        counters = self.client.object_counters_get(self.default_ppg_handle)
        def_ppg_stats = counters[SWITCH_PORT_PRIORITY_GROUP_COUNTER_ID_PACKETS].count
        print("Verifying Default PPG stats for qos mapping:{}".format(def_prio))
        self.assertTrue(def_ppg_stats == exp_def_ppg_stats, "Default PPG stats:{} do not match expected stats:{} for"
                 " priorities {}".format(def_ppg_stats, exp_def_ppg_stats, def_prio))

    def QosMapAttrSetTest(self):
        print("\\nQosMapAttrSetTest()")
        print("Qos Map Ingress Attr:qos_map_list Set Test")
        try:
            print("Setting qos_map_ingress qos_map_list attr to {}:{}".format(
                self.test_qos_map_type_name, QosMapIngressUpdateTest.baseTestConfigMappings))
            self.attribute_set(self.test_qos_map_ingress_handle
                    ,self.test_qos_map_list, self.base_test_config_maps)
            self.verifyQosMapping(QosMapIngressUpdateTest.baseTestConfigMappings)
            self.trafficQosMappingTest(QosMapIngressUpdateTest.baseTestConfigMappings)
            print("OK")

            print("Setting qos_map_ingress qos_map_list attr to {}:[]".format(
                self.test_qos_map_type_name))
            self.attribute_set(self.test_qos_map_ingress_handle
                    ,self.test_qos_map_list, [])
            self.verifyQosMapping()
            self.trafficQosMappingTest()
            print("OK")
        finally:
            self.attribute_set(self.test_qos_map_ingress_handle, self.test_qos_map_list, [])
            self.verifyQosMapping()

    def QosMapAttrUpdateTest1(self):
        print ("\nQosMapAttrUpdateTest1()")
        print("Qos Map Ingress Attr:qos_map_list Update Test")
        print("Update from {0}:{1} => {0}:{1}".format(self.test_qos_map_type_name, QosMapIngressUpdateTest.baseTestConfigMappings))
        try:

            copy_of_base_test_config_maps = self.create_qos_test_config(
                    QosMapIngressUpdateTest.baseTestConfigMappings)
            print("Setting qos_map_ingress qos_map_list attr to {}:{}".format(
                self.test_qos_map_type_name, QosMapIngressUpdateTest.baseTestConfigMappings))
            self.attribute_set(self.test_qos_map_ingress_handle, self.test_qos_map_list, self.base_test_config_maps)
            self.verifyQosMapping(QosMapIngressUpdateTest.baseTestConfigMappings)
            self.trafficQosMappingTest(QosMapIngressUpdateTest.baseTestConfigMappings)
            print("OK")

            print("Setting qon_map_ingress qos_map_list attr to {}:{}".format(
                self.test_qos_map_type_name, QosMapIngressUpdateTest.baseTestConfigMappings))
            self.attribute_set(self.test_qos_map_ingress_handle, self.test_qos_map_list, copy_of_base_test_config_maps)
            self.verifyQosMapping(QosMapIngressUpdateTest.baseTestConfigMappings)
            self.trafficQosMappingTest(QosMapIngressUpdateTest.baseTestConfigMappings)
            print("OK")
        finally:
            self.attribute_set(self.test_qos_map_ingress_handle, self.test_qos_map_list, [])
            self.verifyQosMapping()
            self.clean_to_object(copy_of_base_test_config_maps[0].oid)

    def QosMapAttrUpdateTest(self,mapping):
        print("\nUpdate from {0}:{1} to {0}:{2}".format(self.test_qos_map_type_name, QosMapIngressUpdateTest.baseTestConfigMappings,mapping))
        try:
            # Creating this before hand to keep the cleanup simple
            test_config_maps = self.create_qos_test_config(mapping)

            print("Setting qos_map_ingress qos_map_list attr to {}:{}".format(
                self.test_qos_map_type_name, QosMapIngressUpdateTest.baseTestConfigMappings))
            self.attribute_set(self.test_qos_map_ingress_handle, self.test_qos_map_list,
                    self.base_test_config_maps)
            self.verifyQosMapping(QosMapIngressUpdateTest.baseTestConfigMappings)
            #Commented to speed up test, actual stats verification anyways needs to happen after the update below
            #self.trafficQosMappingTest(QosMapIngressUpdateTest.baseTestConfigMappings)
            print("OK")

            print("Setting qos_map_ingress qos_map_list attr to {}:{}".format(self.test_qos_map_type_name, mapping))
            self.attribute_set(self.test_qos_map_ingress_handle, self.test_qos_map_list,
                    test_config_maps)
            self.verifyQosMapping(mapping)
            self.trafficQosMappingTest(mapping)
            print("OK")
        finally:
            self.attribute_set(self.test_qos_map_ingress_handle, self.test_qos_map_list, [])
            self.verifyQosMapping()
            self.clean_to_object(test_config_maps[0].oid)

    def QosMapAttrUpdateTest2(self):
        print ("\nQosMapAttrUpdateTest2()")
        print("Qos Map Ingress Attr:qos_map_list Update Test")
        for mapping in QosMapIngressUpdateTest.testConfigMappings:
            self.QosMapAttrUpdateTest(mapping)

    def runQosMapIngressUpdateTests(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)==0):
            return
        try:
            self.QosMapAttrSetTest()
            self.QosMapAttrUpdateTest1()
            self.QosMapAttrUpdateTest2()
        finally:
            pass

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP)!=0):
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_TC_QOS_MAP_HANDLE, 0)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)
            self.attribute_set(self.test_qos_map_ingress_handle, self.test_qos_map_list, [])
            if self.dscp_tc_maps:
                self.attribute_set(self.dscp_tc_maps_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            if self.pcp_tc_maps:
                self.attribute_set(self.pcp_tc_maps_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            if self.tc_icos_maps:
                self.attribute_set(self.tc_icos_maps_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            if self.icos_ppg_maps:
                self.attribute_set(self.icos_ppg_maps_handle, SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST, [])
            self.verifyQosMapping()
            self.clean_to_object(self.ppg_handles[0])
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE, 0)
        self.cleanup()

@group('qos')
class IcosPpgQosMapIngressUpdateTest(QosMapIngressUpdateTest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG)

    def verifyQosMapping(self, mapping=[]):
        try:
            print ("Verifying ppg mapping {}".format(mapping))
            # Verify only ppgs having mapping exist in hardware
            mapped_ppgs = set()
            for icos, ppg in mapping:
                mapped_ppgs.add(ppg)
            hw_ppgs = set()
            for index in range(len(self.ppg_handles)):
                if(self.attribute_get(self.ppg_handles[index], SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW)):
                    hw_ppgs.add(index)
            print("Mapped ppgs {}, ppgs created in hw {}".format(mapped_ppgs, hw_ppgs))
            self.assertTrue(mapped_ppgs == hw_ppgs, ("ppgs with mappings {}, ppgs created in hardware {} are not"
                    "same").format(mapped_ppgs, hw_ppgs))
            #Reset ppg stats for the ppgs that were deleted
            for ppg_index in range(len(self.expected_ppg_stats)):
                if ppg_index in hw_ppgs:
                    continue
                else:
                    self.expected_ppg_stats[ppg_index] = 0

        finally:
            pass

    def runTest(self):
        super(self.__class__, self).runQosMapIngressUpdateTests()


@group('qos')
class DscpTcQosMapIngressUpdateTest(QosMapIngressUpdateTest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC)

    def verifyQosMapping(self, mapping=[]):
        pass

    def runTest(self):
        super(self.__class__, self).runQosMapIngressUpdateTests()


@group('qos')
class PcpTcQosMapIngressUpdateTest(QosMapIngressUpdateTest):
    def setUp(self):
        super(self.__class__, self).setUp(test_qos_map_type = SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC)

    def verifyQosMapping(self, mapping=[]):
        pass

    def runTest(self):
        super(self.__class__, self).runQosMapIngressUpdateTests()

@disabled
class TmPortTxEnableTest(ApiHelper):
    def runTest(self):
        self.configure()
        self.device = self.get_device_handle(0)
        print(self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PKT_TX_ENABLE, True)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PKT_TX_ENABLE, False)


class PortPfcConfigTest(ApiHelper, BfRuntimeTest):
    test_configs = [
    {'configuration':{'mode':SWITCH_PORT_ATTR_PFC_MODE_COMBINED, 'pfc_map':12},
    'results': {'rx_pfc_map':12, 'tx_pfc_map':12, 'tm_pfc_queue_map':[(2,2), (3,3)],
    'tail_drop_disable':[]}},
    {'configuration':{'mode':SWITCH_PORT_ATTR_PFC_MODE_SEPARATE, 'pfc_map':12},
    'results': {'rx_pfc_map':0, 'tx_pfc_map':0, 'tm_pfc_queue_map':[],
    'tail_drop_disable':[]}},
    {'configuration':{'mode':SWITCH_PORT_ATTR_PFC_MODE_SEPARATE, 'rx_pfc_map':24},
    'results': {'rx_pfc_map':24, 'tx_pfc_map':0, 'tm_pfc_queue_map':[],
    'tail_drop_disable':[]}},
    {'configuration':{'mode':SWITCH_PORT_ATTR_PFC_MODE_SEPARATE, 'tx_pfc_map':48,
    'pfc_to_queue_map':[(4,4), (5,5)]},
    'results': {'rx_pfc_map':24, 'tx_pfc_map':48, 'tm_pfc_queue_map':[(4,4), (5,5)],
    'tail_drop_disable':[4, 5]}},
    {'configuration':{'mode':SWITCH_PORT_ATTR_PFC_MODE_COMBINED, 'pfc_map':12,
    'pfc_to_queue_map':[(2,4),(3,5)], 'update_pfc_to_queue_map':True},
    'results': {'rx_pfc_map':12, 'tx_pfc_map':12, 'tm_pfc_queue_map':[(2,4), (3,5)],
    'tail_drop_disable':[4,5]}},
    {'configuration':{'mode':SWITCH_PORT_ATTR_PFC_MODE_COMBINED, 'pfc_map':4,
    'pfc_to_queue_map':[(2,4),(3,5)]},
    'results': {'rx_pfc_map':4, 'tx_pfc_map':4, 'tm_pfc_queue_map':[(2,4), (3,5)],
    'tail_drop_disable':[4]}},
    {'configuration':{'mode':SWITCH_PORT_ATTR_PFC_MODE_SEPARATE, 'rx_pfc_map':0, 'tx_pfc_map':8,
    'pfc_to_queue_map':[(2,4),(3,5)]},
    'results': {'rx_pfc_map':0, 'tx_pfc_map':8, 'tm_pfc_queue_map':[(2,4), (3,5)],
    'tail_drop_disable':[5]}}
    ]

    def createPfcPriorityToQueueMap(self, pfc_to_queue_map):
        qos_maps = []
        for prio,queue in pfc_to_queue_map:
            qos_maps.append(self.add_qos_map(self.device, pfc_priority=prio, qid=queue))
        pfc_priority_queue_maps = []
        for qos_map in qos_maps:
            pfc_priority_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        return self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_PFC_PRIORITY_TO_QUEUE, qos_map_list=pfc_priority_queue_maps)

    def updatePfcPriorityToQueueMap(self, pfc_to_queue_map_handle, pfc_to_queue_map):
        qos_maps = []
        for prio,queue in pfc_to_queue_map:
            qos_maps.append(self.add_qos_map(self.device, pfc_priority=prio, qid=queue))
        pfc_priority_queue_maps = []
        for qos_map in qos_maps:
            pfc_priority_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map))
        self.attribute_set(pfc_to_queue_map_handle, SWITCH_QOS_MAP_EGRESS_ATTR_QOS_MAP_LIST, pfc_priority_queue_maps)

    def program_pfc_config(self, test_config):
        print("Configuring PFC Config:{}".format(test_config))
        if 'mode' in test_config:
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_PFC_MODE, test_config['mode'])
        if 'pfc_map' in test_config:
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_PFC_MAP, test_config['pfc_map'])
        if 'rx_pfc_map' in test_config:
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_RX_PFC_MAP, test_config['rx_pfc_map'])
        if 'tx_pfc_map' in test_config:
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_TX_PFC_MAP, test_config['tx_pfc_map'])
        if 'pfc_to_queue_map' in test_config:
            pfc_to_queue_map_handle = self.attribute_get(self.test_port, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE)
            if 'update_pfc_to_queue_map' in test_config and test_config['update_pfc_to_queue_map'] and\
            pfc_to_queue_map_handle != 0:
                self.updatePfcPriorityToQueueMap(pfc_to_queue_map_handle, test_config['pfc_to_queue_map'])
            else:
                self.attribute_set(self.test_port, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,
                                   self.createPfcPriorityToQueueMap(test_config['pfc_to_queue_map']))
        else:
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,0)

    def verify_pfc_config(self, expected_results):
        results={}
        resp = self.port_table.entry_get(
        self.target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', self.test_dev_port)])])
        data = next(resp)[0].to_dict()
        # DRV-5469: MAC Rx PFC MAP configuration not supported on Tofino2 MAC
        if self.arch == 'tofino2':
            expected_results.pop('rx_pfc_map', None)
        else:
            results['rx_pfc_map']=data['$RX_PFC_EN_MAP']
        results['tx_pfc_map']=data['$TX_PFC_EN_MAP']
        results['tm_pfc_queue_map']=[]
        results['tail_drop_disable']=[]
        print("Expected PFC Config:{}".format(expected_results))
        for pfc,queue in expected_results['tm_pfc_queue_map']:
            resp = self.queue_config_table.entry_get(
            self.port_pipe_target,
            [self.queue_config_table.make_key([client.KeyTuple('pg_id', self.test_port_pg_id),
		    								   client.KeyTuple('pg_queue', self.qid_map[queue])])])
            data = next(resp)[0].to_dict()
            results['tm_pfc_queue_map'].append((data['pfc_cos'], queue))

            resp = self.queue_buffer_table.entry_get(
            self.port_pipe_target,
            [self.queue_buffer_table.make_key([client.KeyTuple('pg_id', self.test_port_pg_id),
		    								   client.KeyTuple('pg_queue', self.qid_map[queue])])])
            data = next(resp)[0].to_dict()
            if not data['tail_drop_enable']:
                results['tail_drop_disable'].append(queue)
        self.assertDictEqual(results, expected_results)


    def setUp(self):
        self.configure()
        self.hardware=True
        if (self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1):
            # Skip this test for folded switch pipeline due to bfrt info get failure
            # This test is anyways independent of the profile and ideally can be run
            # on just one profile per arch
            print("Skipping test for folded switch pipeline")
            return
        self.device = self.get_device_handle(0)
        BfRuntimeTest.setUp(self, 0, "switch", notifications=client.Notifications(enable_learn=False))
        self.target = client.Target(device_id=0, pipe_id=0xffff)
        self.port_pipe_target = client.Target(device_id=0, pipe_id=0xffff)
        bfrt_info = self.interface.bfrt_info_get("switch")
        self.port_table = bfrt_info.table_get("$PORT")
        prefix='tf1'
        test_params = ptf.testutils.test_params_get()
        self.arch = test_params['arch']
        if ptf.testutils.test_params_get()["target"] != "hw":
           self.hardware=False
        if self.arch == 'tofino2':
            prefix='tf2'
        if not self.hardware:
            print("Skipping test for Model")
            return
        port_config_table = bfrt_info.table_get(prefix+".tm.port.cfg")
        self.queue_config_table = bfrt_info.table_get(prefix+".tm.queue.cfg")
        self.queue_buffer_table = bfrt_info.table_get(prefix+".tm.queue.buffer")

        self.test_port = self.port1
        self.test_dev_port = self.attribute_get(self.port1, SWITCH_PORT_ATTR_DEV_PORT)
        #Cache default port attrs
        self.pfc_mode = self.attribute_get(self.test_port, SWITCH_PORT_ATTR_PFC_MODE)
        self.pfc_map = self.attribute_get(self.test_port, SWITCH_PORT_ATTR_PFC_MAP)
        self.rx_pfc_map = self.attribute_get(self.test_port, SWITCH_PORT_ATTR_RX_PFC_MAP)
        self.tx_pfc_map = self.attribute_get(self.test_port, SWITCH_PORT_ATTR_TX_PFC_MAP)
        self.pfc_to_queue_map = self.attribute_get(self.test_port, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE)

        self.pipe_id = self.test_dev_port >> 7
        self.port_pipe_target = client.Target(device_id=0, pipe_id=self.pipe_id)
        resp = port_config_table.entry_get(self.target, [port_config_table.make_key([client.KeyTuple('dev_port',
        self.test_dev_port)])])
        data = next(resp)[0].to_dict()
        self.test_port_pg_id = data['pg_id']
        self.qid_map = {}
        for logical_queue, physical_queue in zip(data['ingress_qid_map'], data['egress_qid_queues']):
            self.qid_map[logical_queue] = physical_queue

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1 or not self.hardware):
            return
        try:
            print("Verifying PFC config on Port:{}".format(hex(self.test_port)))
            for config in PortPfcConfigTest.test_configs:
                self.program_pfc_config(config['configuration'])
                try:
                    self.verify_pfc_config(config['results'])
                finally:
                    pfc_to_queue_map_handle = self.attribute_get(self.test_port, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE)
                    if 'update_pfc_to_queue_map' in config['configuration'] and config['configuration']['update_pfc_to_queue_map'] and\
                    pfc_to_queue_map_handle != 0:
                        self.updatePfcPriorityToQueueMap(pfc_to_queue_map_handle, [])
        finally:
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,self.pfc_to_queue_map)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_TX_PFC_MAP, self.tx_pfc_map)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_RX_PFC_MAP, self.rx_pfc_map)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_PFC_MAP, self.pfc_map)
            self.attribute_set(self.test_port, SWITCH_PORT_ATTR_PFC_MODE, self.pfc_mode)

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 0 and self.hardware):
            BfRuntimeTest.tearDown(self)
        self.cleanup()

@group('qos')
class TcAndQueueMapWithAclTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            return

        # configure port0 and port1 
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route2 = self.add_route(self.device, ip_prefix='172.30.30.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route3 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        self.dscp_tc_map1 = self.add_qos_map(self.device, dscp=1, tc=20)
        self.dscp_tc_map2 = self.add_qos_map(self.device, dscp=2, tc=24)
        dscp_tc_maps = []
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_map1))
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_map2))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)

        self.tc_dscp_map1 = self.add_qos_map(self.device, tc=20, dscp=9)
        self.tc_dscp_map2 = self.add_qos_map(self.device, tc=24, dscp=10)
        tc_dscp_maps = []
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_dscp_map1))
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_dscp_map2))
        self.tc_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)
        self.assertEqual(self.status(), 0)

        # Map tc20,tc24 to Q3,Q4
        qos_map1 = self.add_qos_map(self.device, tc=20, qid=3)
        qos_map2 = self.add_qos_map(self.device, tc=24, qid=4)
        tc_queue_maps = [] 
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        self.tc_qid_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)
        self.assertEqual(self.status(), 0)
        queue_handles = self.attribute_get(self.port1, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.queue_3_handle = queue_handles[3].oid
        self.queue_4_handle = queue_handles[4].oid

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            print("SWITCH_FEATURE_INGRESS_IP_QOS_ACL is not set , returing here")
            return
        try:
            self.TcAndQueueMapWithNoAclTest()
            self.TcAndQueueMapWithAclTest()
            self.TcAndQueueMapRewriteTest()
            self.TcAndQueueMapWithIpv6()
            self.TcAndQueueMapWithMultipleACLsOnPort()
            self.TcAndQueueMapWithMultiplePorts()
            self.TcAndQueueMapNegativeTest()

        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def TcAndQueueMapWithNoAclTest(self):
        print("TcAndQueueMapWithNoAclTest()")
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("send packet..")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=0, # dscp 0
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=0, # dscp 0
                ip_ttl=63)

            # qos maps shouldn't have any effect
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped NO ACL dscp  0 -> 0")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def TcAndQueueMapWithAclTest(self):
        print("TcAndQueueMapWithAclTest()")
        # Test the case when both ingress QOS map and QOS ACL are enabled
        try:
            self.acl_table = self.add_acl_table(self.device,
               type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
               bind_point_type=[acl_table_bp_port],
               direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            self.acl_entry = self.add_acl_entry(self.device,
               dst_ip='172.20.10.1',
               dst_ip_mask='255.255.255.255',
               action_set_tc=20,
               table_handle=self.acl_table)
            self.acl_entry1 = self.add_acl_entry(self.device,
               dst_ip='172.30.30.1',
               dst_ip_mask='255.255.255.255',
               action_set_tc=24,
               table_handle=self.acl_table)

            self.attribute_set(self.acl_table, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            pre_cntrs = self.object_counters_get(self.queue_4_handle)

            print("send packet..")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QOS ACL -> tc 24 -> dscp 10
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp 1 -> QOS ACL(set_tc=24) -> TC24")

            # tc24 -> Q4
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass packet w/ queue mapped TC24 -> Q4 ")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass

    def TcAndQueueMapRewriteTest(self):
        print("TcAndQueueMapRewriteTest()")
        try:
            self.acl_table = self.add_acl_table(self.device,
               type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
               bind_point_type=[acl_table_bp_port],
               direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            self.acl_entry = self.add_acl_entry(self.device,
               dst_ip='172.20.10.1',
               dst_ip_mask='255.255.255.255',
               action_set_tc=20,
               table_handle=self.acl_table)
            self.acl_entry1 = self.add_acl_entry(self.device,
               dst_ip='172.30.30.1',
               dst_ip_mask='255.255.255.255',
               action_set_tc=24,
               table_handle=self.acl_table)

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("send packet..")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=0,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=9 << 2, # dscp 9
                ip_ttl=63)

            #dscp 0 -> tc 20 -> dscp 9
            pre_cntrs = self.object_counters_get(self.queue_3_handle)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp 0 -> QOS ACL(set_tc=20) -> TC20 -> dscp 9")

            # Get Q counters tc20 -> Q3
            post_cntrs = self.object_counters_get(self.queue_3_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass packet w/ queue mapped TC20 -> Q3 ")

            # dscp 0 -> tc 24 -> dscp 10
            self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IP].tos = 10 << 2 # dscp 10
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass packet w/ mapped dscp 0 -> QOS ACL(set_tc=24) -> TC24 -> dscp 10")

            # Get Q counters tc24 -> Q4
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass packet w/ queue mapped TC24 -> Q4 ")

            exp_pkt3 = exp_pkt.copy()
            exp_pkt3[IP].tos = 0
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt3, self.devports[1])
            print("pass packet w/ mapped NO ACL dscp value 0 -> 0")

            # Get Q counters tc24 -> Q4 , counter should be 0
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 0)
            print("pass packet w/ queue mapped TC24 -> Q4  , count is 0")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass

    def TcAndQueueMapWithIpv6(self):
        print("TcAndQueueMapWithIpv6()")
        try:
            self.acl_table = self.add_acl_table(self.device,
               type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
               bind_point_type=[acl_table_bp_port],
               direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            self.ipv6_acl_entry = self.add_acl_entry(self.device,
               dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
               dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
               action_set_tc=20,
               table_handle=self.acl_table)


            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            print("send packet..")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=0,
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=9,
                ipv6_hlim=63)

            # dscp 0 -> tc 20 -> dscp 9
            pre_cntrs = self.object_counters_get(self.queue_3_handle)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass ipv6 packet w/ mapped dscp 0 -> QOS ACL(set_tc=20) -> TC20 -> dscp 9")
           
            # Get Q counters tc20 -> Q3
            post_cntrs = self.object_counters_get(self.queue_3_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass ipv6 packet w/ queue mapped TC20 -> Q3 ")

            # dscp 0 -> tc 24 -> dscp 10
            self.attribute_set(self.ipv6_acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IPv6].tc = 10 << 2 # dscp 10
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass ipv6 packet w/ mapped dscp 0 -> QOS ACL(set tc=24) -> TC 24 -> dscp 10")

            # Get Q counters tc20 -> Q4
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass ipv6 packet w/ queue mapped TC20 -> Q4 ")

            exp_pkt3 = exp_pkt.copy()
            exp_pkt3[IPv6].tc = 0
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt3, self.devports[1])
            print("pass ipv6 packet w/ mapped NO ACL dscp 0 -> 0")

            # Get Q counters tc20 -> Q4, count should be 0 
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 0)
            print("pass ipv6 packet w/ queue mapped TC20 -> Q4 , count is 0")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.cleanlast()
            self.cleanlast()
            pass

    def TcAndQueueMapWithMultipleACLsOnPort(self):
        print("TcAndQueueMapWithMultipleACLsOnPort()")
        # Test the case with multiple ACLs(IPV4, IPV6, QOS) on a port 
        try:
            self.acl_table_qos = self.add_acl_table(self.device,
               type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
               bind_point_type=[acl_table_bp_port],
               direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

            self.acl_table_ipv4 = self.add_acl_table(self.device,
               type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
               bind_point_type=[acl_table_bp_port],
               direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

            self.acl_table_ipv6 = self.add_acl_table(self.device,
               type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
               bind_point_type=[acl_table_bp_port],
               direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

            self.acl_entry_qos = self.add_acl_entry(self.device,
               dst_ip='172.20.10.1',
               dst_ip_mask='255.255.255.255',
               packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
               table_handle=self.acl_table_ipv4)

            self.acl_entry_ipv4 = self.add_acl_entry(self.device,
               dst_ip='172.30.30.1',
               dst_ip_mask='255.255.255.255',
               action_set_tc=24,
               table_handle=self.acl_table_qos)

            self.acl_entry_ipv6 = self.add_acl_entry(self.device,
               dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
               dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
               packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
               table_handle=self.acl_table_ipv6)

            self.acl_group = self.add_acl_group(self.device,
               bind_point_type=[acl_group_bp_port])
            self.acl_group_member_qos = self.add_acl_group_member(self.device,
               acl_table_handle=self.acl_table_qos,
               acl_group_handle=self.acl_group)
            self.acl_group_member_ipv4 = self.add_acl_group_member(self.device,
               acl_table_handle=self.acl_table_ipv4,
               acl_group_handle=self.acl_group)
            self.acl_group_member_ipv6 = self.add_acl_group_member(self.device,
               acl_table_handle=self.acl_table_ipv6,
               acl_group_handle=self.acl_group)

            self.attribute_set(self.acl_table_qos, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            #Qos ACL testing
            print("QOS ACL testing")
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            print("send packet..")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QOS ACL -> tc 24 -> dscp 10
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp 1 -> QOS ACL(set_tc=24) -> TC24")

            # tc24 -> Q4
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass packet w/ queue mapped TC24 -> Q4 ")


            #ipv4 ACL testing
            print("ACL ipv4 test")
            print("send packet..")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            #packet has to drop
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            print("pass ipv4 packet dropped")

            #packet has not to drop
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass ipv4 packet not dropped")


            # ACL Ipv6 test
            print("ipv6 ACL test")
            print("send packet..")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_hlim=63)

            # drop
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            print("pass ipv6 packet dropped")

            #packet has not to drop
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass ipv6 packet not dropped")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass

    def TcAndQueueMapWithMultiplePorts(self):
        print("TcAndQueueMapWithMultiplePorts()")
        # Test the case with applying ACL on multiple ports 
        try:
            self.acl_table_qos = self.add_acl_table(self.device,
               type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
               bind_point_type=[acl_table_bp_port],
               direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

            self.acl_entry_ipv4 = self.add_acl_entry(self.device,
               dst_ip='172.30.30.1',
               dst_ip_mask='255.255.255.255',
               action_set_tc=24,
               table_handle=self.acl_table_qos)

            self.acl_group = self.add_acl_group(self.device,
               bind_point_type=[acl_group_bp_port])
            self.acl_group_member_qos = self.add_acl_group_member(self.device,
               acl_table_handle=self.acl_table_qos,
               acl_group_handle=self.acl_group)

            self.attribute_set(self.acl_table_qos, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            #testing on port0
            print("testing on port0")
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            print("send packet on port0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QOS ACL -> tc 24 -> dscp 10
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp 1 -> QOS ACL(set_tc=24) -> TC24")

            # tc24 -> Q4
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass packet w/ queue mapped TC24 -> Q4 ")

            #testing on port2
            print("testing on port2")
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            print("send packet on port2")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QOS ACL -> tc 24 -> dscp 10
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp 1 -> QOS ACL(set_tc=24) -> TC24")

            # tc24 -> Q4
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass packet w/ queue mapped TC24 -> Q4 ")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass

    def TcAndQueueMapNegativeTest(self):
        print("TcAndQueueMapNegativeTest()")
        # Test the case with applying ACL on port and test if ACL should apply for other ports
        try:
            self.acl_table_qos = self.add_acl_table(self.device,
               type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
               bind_point_type=[acl_table_bp_port],
               direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

            self.acl_entry_ipv4 = self.add_acl_entry(self.device,
               dst_ip='172.30.30.1',
               dst_ip_mask='255.255.255.255',
               action_set_tc=24,
               table_handle=self.acl_table_qos)

            self.acl_group = self.add_acl_group(self.device,
               bind_point_type=[acl_group_bp_port])
            self.acl_group_member_qos = self.add_acl_group_member(self.device,
               acl_table_handle=self.acl_table_qos,
               acl_group_handle=self.acl_group)

            self.attribute_set(self.acl_table_qos, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group)
            #self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)


            #testing on port0
            print("testing on port0")
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            print("send packet on port0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QOS ACL -> tc 24 -> dscp 10
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp 1 -> QOS ACL(set_tc=24) -> TC24")

            # tc24 -> Q4
            post_cntrs = self.object_counters_get(self.queue_4_handle)
            c = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_cntrs[c].count - pre_cntrs[c].count, 1)
            print("pass packet w/ queue mapped TC24 -> Q4 ")

            #testing on port2
            print("testing on port2")
            pre_cntrs = self.object_counters_get(self.queue_4_handle)
            print("send packet on port2")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                #ip_tos=10 << 2, # dscp 10
                ip_tos=1 << 2, # dscp 10
                ip_ttl=63)

            # ACL shouldn't have any effect
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped NO ACL dscp  1 -> 1")


        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass

