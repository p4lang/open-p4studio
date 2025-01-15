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
Thrift SAI buffer tests
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

@group('qos')
@group('bfn')
class PPGBufferProfileSetTest(sai_base_test.ThriftInterfaceDataPlane):
  def runTest(self):
    switch_init(self.client)
    port1 = port_list[0]
    port2 = port_list[1]

    try:
      ppg_list = []
      for i in range(0,3):
        print i
        handle = self.client.sai_thrift_create_ppg(port1, i)
        ppg_list.append(handle)

      print ppg_list

      ingress_pfc_cos_list = [1, 2, 3]
      ingress_pg_list = [0, 1, 2]

      ingress_qos_map_id = sai_thrift_create_qos_map(
          self.client, SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP, ingress_pfc_cos_list,
          ingress_pg_list)

      ingress_pool_size = 1024

      ig_buffer_pool_id = sai_thrift_create_pool_profile(self.client, SAI_BUFFER_POOL_TYPE_INGRESS, ingress_pool_size, SAI_BUFFER_POOL_THRESHOLD_MODE_DYNAMIC)

      ig_buffer_profile = sai_thrift_create_buffer_profile(self.client, ig_buffer_pool_id, 1024, 0, 30, 10)

      thrift_attr_value = sai_thrift_attribute_value_t(oid = ig_buffer_profile)
      thrift_attr = sai_thrift_attribute_t(id = SAI_INGRESS_PRIORITY_GROUP_ATTR_BUFFER_PROFILE, value = thrift_attr_value)
      self.client.sai_thrift_set_priority_group_attribute(ppg_list[0], thrift_attr)
      profile_handle = self.client.sai_thrift_get_ppg_profile(ppg_list[0])
      sai_thrift_set_port_attribute(self.client, port1,
                                    SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP,
                                    ingress_qos_map_id)
      assert(profile_handle == ig_buffer_profile)


      print "Qos map handle 0x%lx"%(ingress_qos_map_id)
    finally:
      thrift_attr_value = sai_thrift_attribute_value_t(oid = SAI_NULL_OBJECT_ID)
      thrift_attr = sai_thrift_attribute_t(id = SAI_INGRESS_PRIORITY_GROUP_ATTR_BUFFER_PROFILE, value = thrift_attr_value)
      self.client.sai_thrift_set_priority_group_attribute(ppg_list[0], thrift_attr)

      sai_thrift_set_port_attribute(self.client, port1,
                                    SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP,
                                    SAI_NULL_OBJECT_ID)
      sai_thrift_remove_qos_map(self.client, ingress_qos_map_id)
      for ppg_handle in ppg_list:
        self.client.sai_thrift_remove_ppg(ppg_handle)
      sai_thrift_remove_buffer_profile(self.client, ig_buffer_profile)
      sai_thrift_remove_buffer_pool(self.client, ig_buffer_pool_id)
