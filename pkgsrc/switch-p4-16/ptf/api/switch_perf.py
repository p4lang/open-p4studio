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
Thrift triggered BMAI & BFRT APIperformance tests
"""

import csv

from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *
from tabulate import tabulate

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

from switch_helpers import ApiHelper


fail_status_code_messages = {
    SWITCH_STATUS_FAILURE: "Unexpected failure: SWITCH_STATUS_FAILURE",
    SWITCH_STATUS_NOT_SUPPORTED: "Unexpected failure: SWITCH_STATUS_NOT_SUPPORTED",
    SWITCH_STATUS_NO_MEMORY: "Unexpected failure: SWITCH_STATUS_NO_MEMORY",
    SWITCH_STATUS_INSUFFICIENT_RESOURCES: "Test failed, could not open the test data file",
    SWITCH_STATUS_INVALID_PARAMETER: "Test failed, test data in incorrect format",
    SWITCH_STATUS_ITEM_ALREADY_EXISTS: "Unexpected failure: SWITCH_STATUS_ITEM_ALREADY_EXISTS",
    SWITCH_STATUS_ITEM_NOT_FOUND: "Test failed, check test data files location",
    SWITCH_STATUS_RESOURCE_IN_USE: "Unexpected failure: SWITCH_STATUS_RESOURCE_IN_USE",
    SWITCH_STATUS_HW_FAILURE: "Unexpected failure: SWITCH_STATUS_HW_FAILURE",
    SWITCH_STATUS_PD_FAILURE: "Unexpected failure: SWITCH_STATUS_PD_FAILURE",
    SWITCH_STATUS_DEPENDENCY_FAILURE: "Unexpected failure: SWITCH_STATUS_DEPENDENCY_FAILURE",
    SWITCH_STATUS_INVALID_KEY_GROUP: "Unexpected failure: SWITCH_STATUS_INVALID_KEY_GROUP",
    SWITCH_STATUS_NOT_IMPLEMENTED: "Test failed. Testcase not implemented in selected configuration",
    SWITCH_STATUS_FEATURE_NOT_SUPPORTED: "Unexpected failure: SWITCH_STATUS_FEATURE_NOT_SUPPORTED",
    SWITCH_STATUS_TABLE_FULL: "Unexpected failure: SWITCH_STATUS_TABLE_FULL"
}
non_failure_return_codes = [SWITCH_STATUS_SUCCESS, SWITCH_STATUS_NOT_IMPLEMENTED]

class UnitsAndHeaders:

        NUM_ENTRIES = "Entries"
        BMAI = "BMAI"
        BMAI_BATCH = "BMAI (batch)"
        BFRT = "BFRT"
        BFRT_BATCH = "BFRT (batch)"

        headers = [NUM_ENTRIES,
                        BMAI,
                        BMAI_BATCH,
                        BFRT,
                        BFRT_BATCH]

        units_list = ["[-]",
                        "[op/s]",
                        "[op/s]",
                        "[op/s]",
                        "[op/s]"]

class PerfTest:
    """@brief API Perf test helper class

    This is a helper class to be used when creating and running performance tests
    for insertion of entries to various switch tables using BFRT and BMAI APIs.

    It can be used to prepare and run performance tests
    that will add and remove entries to specified bf_switch tables.
    An explicit implementetion on the switch side is necessary.
    The test will be run multiple times, for BMAI and BFRT APIs,
    for specified table and for number of entries starting from min
    up to max, increased by step, to produce a set of data describing
    num_entries <-> rate relation
    """
    def __init__(self, inherit_self, function, table, min, max, step):
        """@brief Creates a new PerfTest object.
        @param inherit_self instance of ApiHelper that is needed for
        the configuration functionality (adding vrfs, vlans, nexthops, etc)
        @param function function that will be used on the switch side to run the test
        @param table table that will be filled in the test
        @param min minimum number of entries that will be added during the test
        @param max maximum number of entries that will be added during the test
        @param step number of entries that the min will be increased by during the test
        """

        self.s = inherit_self
        self.table = table
        self.results = {}
        self.test_params = []
        self.device = self.s.get_device_handle(0)
        self.function = function
        # preparing set of parameters that will be passed to the
        # switch_api_perf_test function on the switch side:
        # device handle, API, table, whether or not to use batch mode
        # and number of inserted entries
        for n in range(min, max, step):
            self.test_params.append([self.device, switcht_perf_api.BMAI, table, False, n ])
            self.test_params.append([self.device, switcht_perf_api.BMAI, table, True,  n ])
            self.test_params.append([self.device, switcht_perf_api.BFRT, table, False, n ])
            self.test_params.append([self.device, switcht_perf_api.BFRT, table, True,  n ])
            self.results[n] = {}

    def configure(self):
        # run configure from ApiHelper to have ports in place
        self.s.configure()
        # add vlans
        for num in range(41, 4041):
            self.s.add_vlan(self.s.device, vlan_id=num)

        #add vrfs
        self.s.vrf20 = self.s.add_vrf(self.device)
        self.s.vrf30 = self.s.add_vrf(self.device)
        self.s.vrf40 = self.s.add_vrf(self.device)
        self.s.vrf50 = self.s.add_vrf(self.device)

        # add rifs
        self.rif0 = self.s.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.s.port0,
            vrf_handle=self.s.vrf10, src_mac=self.s.rmac)
        self.rif1 = self.s.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.s.port1,
            vrf_handle=self.s.vrf20, src_mac=self.s.rmac)
        self.rif2 = self.s.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.s.port2,
            vrf_handle=self.s.vrf30, src_mac=self.s.rmac)
        self.rif3 = self.s.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.s.port3,
            vrf_handle=self.s.vrf40, src_mac=self.s.rmac)
        self.rif4 = self.s.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.s.port4,
            vrf_handle=self.s.vrf50, src_mac=self.s.rmac)

        # add nhops
        self.nhop0 = self.s.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_DROP)
        self.nhop1 = self.s.add_nexthop(self.device, handle=self.rif0, dest_ip='40.10.0.2')
        self.nhop2 = self.s.add_nexthop(self.device, handle=self.rif1, dest_ip='3.40.0.2')
        self.nhop3 = self.s.add_nexthop(self.device, handle=self.rif2, dest_ip='20.40.0.2')
        self.nhop4 = self.s.add_nexthop(self.device, handle=self.rif3, dest_ip='40.3.0.2')
        self.nhop5 = self.s.add_nexthop(self.device, handle=self.rif4, dest_ip='6.40.0.2')

    def run(self):
        for x in self.test_params:
            if x[1] == switcht_perf_api.BMAI and x[3] == True:
                self.mode = UnitsAndHeaders.BMAI_BATCH
            elif x[1] == switcht_perf_api.BMAI and x[3] == False:
                self.mode = UnitsAndHeaders.BMAI
            elif x[1] == switcht_perf_api.BFRT and x[3] == False:
                self.mode = UnitsAndHeaders.BFRT
            else:
                self.mode = UnitsAndHeaders.BFRT_BATCH
            print("Testing " + self.mode + ", " + str(x[4]) + " entries")
            result = self.function(*x)
            rate = result.object_id
            return_code = result.status


            self.s.assertIn(return_code, non_failure_return_codes,
                            fail_status_code_messages.get(return_code, "Unexpected return code: %d" % return_code))
            # Results is a dictionary where the key is the number of entries
            # If the tests are implemented only in BMAI, then BFRT results will be shown as 0
            self.results[x[4]][self.mode] = \
                rate if return_code == SWITCH_STATUS_SUCCESS else 0
            if return_code == SWITCH_STATUS_NOT_IMPLEMENTED:
                print("WARNING: %s" % fail_status_code_messages[return_code])

    def cleanup(self):
        return

    def printResults(self):
        headers = [f"{UnitsAndHeaders.headers[i]}\n{UnitsAndHeaders.units_list[i]}"
                   for i in range (0, len(UnitsAndHeaders.headers))]
        flat_results = []
        for k, v in self.results.items():
            flat_results = flat_results + [[k, *v.values()]]
        print(tabulate(flat_results, headers, numalign="right", stralign = "right"))
        return

    def dumpResults(self):
        units_list = UnitsAndHeaders.units_list
        headers = UnitsAndHeaders.headers
        filename = switcht_perf_table._VALUES_TO_NAMES[self.table] + ".csv"
        with open(filename, mode = 'w') as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames = headers)
            header = dict(zip(headers, headers))
            units = dict(zip(headers, units_list))
            writer.writerow(header)
            writer.writerow(units)
            for k, v in sorted(self.results.items()):
                v[UnitsAndHeaders.NUM_ENTRIES] = k
                writer.writerow(v)
        return


@group('perf')
@disabled
class MacPerfTest(ApiHelper):
    """
        This test is intended to be run on ASIC x1_tofino
        as it depends on the table size specific to this profile
    """
    def runTest(self):
        max_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_DMAC).size
        print(max_size)
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.MAC_ENTRY, 1000, max_size, 1000)
        pt.configure()
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()


@group('perf')
@disabled
class LPMRouteIpv4PerfTest(ApiHelper):
    """
        This test is intended to be run on ASIC x1_tofino
        as it depends on the table size specific to this profile
    """
    def runTest(self):
        max_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV4_LPM).size
        print(max_size)
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.LPM_ROUTE_IPV4, 500, int(0.8*max_size), 10000)
        pt.configure()
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()

###############################################################################

@group('perf')
@disabled
class LPMRouteIpv6PerfTest(ApiHelper):
    """
        This test is intended to be run on ASIC x1_tofino
        as it depends on the table size specific to this profile
    """
    def runTest(self):
        max_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM64).size
        print(max_size)
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.LPM_ROUTE_IPV6, 1000, int(0.8*max_size), 4000)
        pt.configure()
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()

###############################################################################

@group('perf')
@disabled
class HostRouteIpv4PerfTest(ApiHelper):
    """
        This test is intended to be run on ASIC x1_tofino
        as it depends on the table size specific to this profile
    """
    def runTest(self):
        max_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV4_HOST).size
        print(max_size)
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.HOST_ROUTE_IPV4, 5000, max_size, 5000)
        pt.configure()
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()

###############################################################################

@group('perf')
@disabled
class HostRouteIpv6PerfTest(ApiHelper):
    """
        This test is intended to be run on ASIC x1_tofino
        as it depends on the table size specific to this profile
    """
    def runTest(self):
        max_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_HOST64).size
        print(max_size)
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.HOST_ROUTE_IPV6, 1000, max_size, 5000)
        pt.configure()
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()

###############################################################################

@group('perf')
@disabled
class NexthopPerfTest(ApiHelper):
    def runTest(self):
        max_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_NEXTHOP).size
        print(max_size)
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.NEXTHOP, 1000, max_size, 5000)
        pt.configure()
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()

###############################################################################

@group('perf')
@disabled
class VlanPerfTest(ApiHelper):
    def runTest(self):
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.VLAN, 1000, 4095, 1000)
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()

###############################################################################

@group('perf')
@disabled
class McastMemberPerfTest(ApiHelper):
    def runTest(self):
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.MCAST_MEMBER, 1000, 151000, 50000)
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()

###############################################################################

@group('perf')
@disabled
class AclIpv4PerfTest(ApiHelper):
    """
        This test is intended to be run on ASIC x1_tofino
        as it depends on the table size specific to this profile
    """
    def runTest(self):
        pt = PerfTest(self, self.client.switch_api_perf_test,
                        switcht_perf_table.ACL_IPV4, 100, 2001, 100)
        pt.configure()
        pt.run()
        pt.printResults()
        pt.dumpResults()
        self.cleanup()
