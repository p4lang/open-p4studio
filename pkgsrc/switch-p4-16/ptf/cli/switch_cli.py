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
from __future__ import print_function

import binascii
import time
import os
import sys
import logging
import unittest
import random
import argparse
import re

from ptf.base_tests import BaseTest

import ptf.mask

try:
    import pexpect
except ImportError:
    sys.stderr.write("You do not have pexpect installed\n")

class Cli():

    child = None
    logfile = ""
    COMMAND_PROMPT = '[$#] '

    def __init__(self, echo="False", filename=""):
        self.child = pexpect.spawn('bfshell')
        if filename == "":
            filename = "logfile.txt"
            self.logfile = open(filename, "wb")
            self.child.logfile = self.logfile
            self.child.logfile_read = sys.stdout.buffer
        else:
            self.logfile = file(filename, "wb")
            self.child.logfile = self.logfile
            self.child.logfile_read = sys.stdout.buffer

        self.child.setecho(echo)

    def send(self, cmd, timeout=8):
        print(cmd)
        self.child.send(cmd)
        self.child.send("\r\n")

    def expect(self, prompt, timeout=8):
        print(prompt)
        self.child.expect(prompt, timeout)
        return self.child.before

    def before(self):
        return self.child.before

    def after(self):
        return self.child.after

    def sendonly(self, cmd):
        print(cmd)
        self.child.sendline(cmd)

    def sendExpect(self, cmd, prompt, timeout=8):
        print(cmd)
        print(prompt)
        self.child.send(cmd)
        self.child.send("\r\n")
        self.child.expect(prompt, timeout=timeout)
        if re.search("FAILED", self.child.before):
            return 1
        return 0

    def sendlineExpect(self, cmd, prompt, timeout=8):
        print(cmd)
        print(prompt)
        self.child.sendline(cmd)
        self.child.expect(self.COMMAND_PROMPT, timeout=timeout)
        if re.search("FAILED", self.child.before):
            return 1
        return 0

    def closeConnection(self):
        if self.child != None:
            self.child.close()
            self.logfile.close()

###############################################################################

cli_list = [
    ("add port type NORMAL lane_list 0 speed 25000", "success"),
    ("add vlan vlan_id 10", "success"),
    ("add vlan_member member_handle port 6 vlan_handle vlan 2", "success"),
    ("set port handle 6 attribute port_vlan_id 10", "success"),
    ("add mac_entry vlan_handle vlan 2 mac_address 00:22:22:22:22:22 destination_handle port 6 type STATIC", "success"),
    ("show mac_entry mac_address 00:22:22:22:22:22 vlan_handle vlan 2", "object_id"),
    ("show mac_entry mac_address 00:22:22:22:22:23 vlan_handle vlan 2", "failed"),
    ("show port all", "oid"),
    ("show port handle 6", "Attributes"),
    ("show vlan all", "oid"),
    ("show counter port all", "Count"),
    ("show counter port handle 6", "Count"),
    ("show counter vlan  all", "Count"),
    ("add port type NORMAL lane_list 2 speed 10000", "success"),
    ("add rif vrf_handle vrf 1 type PORT src_mac 00:11:11:11:11:11 port_handle port 7", "success"),
    ("add nexthop type IP handle rif 2 dest_ip 10.1.1.1", "success"),
    ("add route vrf_handle vrf 1 ip_prefix 4444::1 nexthop_handle nexthop 3", "success"),
    ("add route vrf_handle vrf 1 ip_prefix 2001:db8::1111:2222:3333:4 nexthop_handle nexthop 3", "success"),
    ("add route vrf_handle vrf 1 ip_prefix 20.1.1.0/24 nexthop_handle nexthop 3", "success"),
    ("show route ip_prefix 4444::1 is_nbr_sourced false vrf_handle vrf 1", "object_id"),
    ("show route ip_prefix 20.1.1.0/24 is_nbr_sourced false vrf_handle vrf 1", "object_id"),
    ("show route ip_prefix 20.1.1.1/24 is_nbr_sourced false vrf_handle vrf 1", "failed"),
    ("add acl_table type ipv6 direction INGRESS bind_point_attach true", "success"),
    ("add acl_entry table_handle acl_table 1 src_ip ::ffff:127.0.0.0 src_ip_mask ffff:ffff:ffff:ffff:ffff:ffff:ff00:0 packet_action drop", "success"),
    ("show acl_entry all", "::ffff:127.0.0.0"),
    ("del acl_entry handle 1", "success"),
    ("add acl_entry table_handle acl_table 1 src_ip :: src_ip_mask ffff:ffff:ffff:ffff:ffff:ffff:ff00:0 packet_action drop", "success"),
    ("show acl_entry all", "::"),
    ("del acl_entry handle 1", "success"),
    ("add acl_entry table_handle acl_table 1 src_ip 1:: src_ip_mask ffff:ffff:ffff:ffff:ffff:ffff:ff00:0 packet_action drop", "success"),
    ("show acl_entry all", "1::"),
    ("del acl_entry handle 1", "success"),
    ("add acl_entry table_handle acl_table 1 src_ip ::127.0.0.1 src_ip_mask ffff:ffff:ffff:ffff:ffff:ffff:ff00:0 packet_action drop", "success"),
    ("show acl_entry all", "::127.0.0.1"),
    ("del acl_entry handle 1", "success"),
    ("add acl_entry table_handle acl_table 1 src_ip 1::127.0.0.1 src_ip_mask ffff:ffff:ffff:ffff:ffff:ffff:ff00:0 packet_action drop", "success"),
    ("show acl_entry all", "1::7f00:1"),
    ("del acl_entry handle 1", "success"),
    ("del acl_table handle 1", "success"),
    ("del route handle 3", "success"),
    ("del route handle 2", "success"),
    ("del route handle 1", "success"),
    ("del nexthop handle 3", "success"),
    ("del rif handle 2", "success"),
    ("del port handle 7", "success"),
    ("del mac_entry handle 1", "success"),
    ("del vlan_member handle 1", "success"),
    ("set port handle 6 attribute port_vlan_id 1", "success"),
    ("del vlan handle 2", "success"),
    ("del port handle 6", "success"),
    ("add qos_map dscp 1 tc 1", "success"),
    ("add qos_map dscp 2 tc 2", "success"),
    ("add qos_map_ingress type DSCP_TO_TC qos_map_list qos_map 1,2", "success"),
    ("add qos_map_ingress type DSCP_TO_TC qos_map_list qos_map 1", "success"),
    ("add qos_map_ingress type DSCP_TO_TC qos_map_list qos_map 2", "success"),
    ("show qos_map_ingress all", "oid"),
    ("show qos_map_ingress handle 1", "Attributes"),
    ("show qos_map_ingress handle 2", "Attributes"),
    ("show qos_map_ingress handle 3", "Attributes"),
    ("del qos_map_ingress handle 2", "success"),
    ("del qos_map_ingress handle 2", "failed"),
    ("show qos_map_ingress all", "oid"),
    ("add qos_map dscp 3 tc 3", "success"),
    ("set qos_map_ingress handle 3 attribute qos_map_list qos_map 3,2,1", "success"),
    ("show qos_map_ingress handle 3", "Attributes"),
    ("set qos_map_ingress handle 3 attribute qos_map_list qos_map 1", "success"),
    ("show qos_map_ingress handle 3", "Attributes"),
    ("show qos_map_ingress handle 3 dependencies", "object_type"),
    ("del qos_map_ingress handle 3", "success"),
    ("del qos_map_ingress handle 1", "success"),
    ("del qos_map handle 3", "success"),
    ("del qos_map handle 2", "success"),
    ("del qos_map handle 1", "success"),
]

'''
Before running this test, make sure sde_install is in the path
'''
class CliTest(BaseTest):
    def setUp(self):
        BaseTest.setUp(self)
        self.cli = Cli()
        time.sleep(1)
        self.cli.send("bf_switch")
        output = self.cli.expect('bf_switch:0>\s+\r')

    def runTest(self):
        for each in cli_list:
            self.cli.send(each[0])
            output = self.cli.expect(each[1])

    def tearDown(self):
        self.cli.send("exit")
        self.cli.closeConnection()
