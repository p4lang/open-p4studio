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
Thrift PD interface basic tests
"""
from collections import OrderedDict

import time
import datetime
import sys
import logging
import math

import unittest

import pd_base_tests

from ptf import config
from ptf.testutils import *
from ptf.thriftutils import *

import os

from perf_test.p4_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *

import random

this_dir = os.path.dirname(os.path.abspath(__file__))


seed = random.randint(0, 65536)
random.seed(seed)
print('Seed used %d' % seed)
sys.stdout.flush()

class TestPipelineTable(pd_base_tests.EmptyTableTest):
    program = "perf_test"
    table = "perf_exm_immediate_action"


@group("thorough")
class TestExmPerfImmediateAction(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["perf_test"])

    """ Exact match perf test with immediate action """
    def runTest(self):
        print()
        min_rate = 88000
        sess_hdl = self.conn_mgr.client_init()

        dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))

        print(datetime.datetime.now(), "  Creating test data")
        num_entries = 500000

        macaddrs = []
        ipdstAddrdict = {}
        ipdstAddrs = []
        for i in range(num_entries):
            ipaddr = "%d.%d.%d.%d" % (random.randint(1,255), random.randint(0,255), random.randint(0,255), random.randint(0,255) )
            while ipaddr in ipdstAddrdict:
                ipaddr = "%d.%d.%d.%d" %(random.randint(1,255), random.randint(0,255), random.randint(0,255), random.randint(0,255))
            ipdstAddrs += [ipaddr]
            ipdstAddrdict[ipaddr] = True
        vrfs = [(random.randint(1, 2000)) for x in range(num_entries)]
        nhop_indices = [(random.randint(1, 5000)) for x in range(num_entries)]

        ipsrcAddrs = ["123.%d.%d.%d" % (random.randint(0,255), random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]
        tcpSrcPorts = ["%d" % (random.randint(0,255)) for x in range(num_entries)]
        mask = "255.255.255.255"
        priority = [(random.randint(1, 255)) for x in range(num_entries)]

        ipdstResult = ["250.%d.%d.%d" % (random.randint(0,255), random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]

        macaddrs += ["00:33:%02x:%02x:%02x:%02x" % (random.randint(0,255), random.randint(0,255), random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]

        self.client.perf_exm_immediate_action_table_bulk_init(sess_hdl, dev_tgt, num_entries)

        print(datetime.datetime.now(), "  Setting up entries")
        for i in range(0, num_entries):
            match_spec = perf_test_perf_exm_immediate_action_match_spec_t(
                    vrfs[i],
                    ipv4Addr_to_i32(ipdstAddrs[i]))

            action_spec = perf_test_fib_hit_nexthop_action_spec_t(nhop_indices[i])

            self.client.perf_exm_immediate_action_table_add_with_fib_hit_nexthop_bulk_setup(sess_hdl,
                                                                                            dev_tgt,
                                                                                            match_spec,
                                                                                            action_spec)

        print(datetime.datetime.now(), "  Starting test")
        rate = self.client.perf_exm_immediate_action_table_perf_test_execute()
        print(datetime.datetime.now(), "  Completed test")
        self.conn_mgr.client_cleanup(sess_hdl)
        print("Rate (Operations/second) : %d" % (rate))
        if rate < min_rate:
            sys.stdout.flush()
            assert(0)
