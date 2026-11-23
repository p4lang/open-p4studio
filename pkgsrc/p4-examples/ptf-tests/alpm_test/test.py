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
import re
import math

import unittest
import random

import pd_base_tests

from ptf import config
from ptf.testutils import *
from p4testutils.misc_utils import *
from ptf.thriftutils import *

import os

from alpm_test.p4_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *

import random
import pdb

this_dir = os.path.dirname(os.path.abspath(__file__))

dev_id = 0
MAX_PORT_COUNT = 456

swports = get_sw_ports()

def port_to_pipe(port):
    local_port = port & 0x7F
    assert(local_port < 72)
    pipe = (port >> 7) & 0x3
    assert(port == ((pipe << 7) | local_port))
    return pipe

swports_0 = []
swports_1 = []
swports_2 = []
swports_3 = []
for port in swports:
    pipe = port_to_pipe(port)
    if pipe == 0:
        swports_0.append(port)
    elif pipe == 1:
        swports_1.append(port)
    elif pipe == 2:
        swports_2.append(port)
    elif pipe == 3:
        swports_3.append(port)

seed = random.randint(0, 65536)
random.seed(seed)
print(('Seed used for the file %d' %(seed)))
sys.stdout.flush()

def tofLocalPortToOfPort(port, dev_id):
    assert port < MAX_PORT_COUNT
    return (dev_id * MAX_PORT_COUNT) + port

def InstallAllDefaultEntries(self):
    global dev_id

    sess_hdl = self.conn_mgr.client_init()
    # Get the dev_id from the config
    dev_id = 0
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

    nop_default_fns = [
                        self.client.ipv4_alpm_set_default_action_nop,
                        self.client.ipv4_alpm_large_set_default_action_nop
                      ]

    print("Installing default entries for all tables")

    for nop_fn in nop_default_fns:
        nop_fn(sess_hdl,
               dev_tgt)

    print("closing session")
    status = self.conn_mgr.client_cleanup(sess_hdl)

def RemoveAllDefaultEntries(self):
    global dev_id

    sess_hdl = self.conn_mgr.client_init()
    # Get the dev_id from the config
    dev_id = 0
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

    reset_default_fns = [
                          self.client.ipv4_alpm_table_reset_default_entry,
                          self.client.ipv4_alpm_large_table_reset_default_entry
                        ]

    print("Removing default entries for all tables")

    for reset_fn in reset_default_fns:
        reset_fn(sess_hdl, dev_tgt)

    print("closing session")
    status = self.conn_mgr.client_cleanup(sess_hdl)

def buildMask(match_len):
    return hex_to_i32(((1 << 32) - 1) - ((1 << (32 - match_len)) - 1))

def lpm(test_matrix, delete_indices, dst):
    best_match_len = -1
    best_match_port = -1
    dst = ipv4Addr_to_i32(dst)
    for i in range(len(test_matrix)):
        if i not in delete_indices:
            (ip_dst, match_len, port) = test_matrix[i]
            match_addr_int = ipv4Addr_to_i32(ip_dst)
            mask = buildMask(match_len)
            if (dst & mask) == (match_addr_int & mask) and match_len > best_match_len:
                best_match_port = port
                best_match_len = match_len
    return best_match_port

def readData(total_entries):
    dir_path = os.path.dirname(os.path.realpath(__file__))
    data_file = os.path.join(dir_path, 'data.txt')
    f = open(data_file, 'r')
    lines = f.readlines()
    f.close()

    test_matrix = []
    ip_addrs = set([])
    while len(test_matrix) < total_entries:
        idx = random.randint(0, len(lines) - 1)
        line = lines[idx]
        line = re.split('/|\t', line.strip())
        if line[0] in ip_addrs:
            continue
        test_matrix.append((line[0], int(line[1]), swports[int(line[2]) % (len(swports)-1)]))
        ip_addrs.add(line[0])

    return test_matrix


class TestAddRoute(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        match_spec = alpm_test_ipv4_alpm_match_spec_t(
            0,
            ipv4Addr_to_i32("10.0.0.1"),
            0
        )
        port = swports[3]
        action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
        print("adding entry")
        entry_hdl = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
            sess_hdl,
            dev_tgt,
            match_spec,
            action_spec
        )

        print(("PIPE_MGR gave me that entry handle:", entry_hdl))

        try:
            self.conn_mgr.complete_operations(sess_hdl)
            print()
            print(("Sending packet port %s -> port %s (192.168.0.1 -> 10.0.0.1 [id = 101])" % (swports[1], swports[3])))
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

        finally:
            print("deleting entry")
            status = self.client.ipv4_alpm_table_delete( sess_hdl,
                dev_id,
                entry_hdl
            )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))

class TestDrop(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        port = swports[2]

        #Add a * entry to send it out of port 2
        match_spec = alpm_test_ipv4_alpm_match_spec_t(
            0,
            ipv4Addr_to_i32("10.0.0.1"),
            0
        )
        action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
        print("adding * entry")
        entry_hdl = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
            sess_hdl,
            dev_tgt,
            match_spec,
            action_spec
        )

        # Add the drop entry
        match_spec = alpm_test_ipv4_alpm_match_spec_t(
            0,
            ipv4Addr_to_i32("10.0.0.1"),
            32
        )
        print("adding DROP entry")
        drop_entry_hdl = self.client.ipv4_alpm_table_add_with_lpm_miss(
            sess_hdl,
            dev_tgt,
            match_spec
        )
        print(("PIPE_MGR gave me that entry handle:", drop_entry_hdl))

        try:
            self.conn_mgr.complete_operations(sess_hdl)
            print()
            print(("Sending packet port %s -> port %s (192.168.0.1 -> 10.0.0.1 [id = 101])" % (swports[1], swports[2])))
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

            verify_no_other_packets(self, timeout=3)

        finally:
            print("deleting Drop entry")
            status = self.client.ipv4_alpm_table_delete( sess_hdl,
                dev_id,
                drop_entry_hdl
            )

            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

            print("deleting * entry")
            status = self.client.ipv4_alpm_table_delete( sess_hdl,
                dev_id,
                entry_hdl
            )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))

class TestModify(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        port1 = swports[2]
        port2 = swports[3]

        #Add a * entry to send it out of port 2
        match_spec = alpm_test_ipv4_alpm_match_spec_t(
            0,
            ipv4Addr_to_i32("10.0.0.1"),
            0
        )
        action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port1)
        print("adding * entry")
        entry_hdl = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
            sess_hdl,
            dev_tgt,
            match_spec,
            action_spec
        )

        try:
            self.conn_mgr.complete_operations(sess_hdl)
            print()
            print(("Sending packet port %s -> port %s (192.168.0.1 -> 10.0.0.1 [id = 101])" % (swports[1], swports[2])))
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(port1, dev_id)])

        finally:
            print("Modifying entry")
            action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port2)
            status = self.client.ipv4_alpm_table_modify_with_ipv4_lpm_hit(
                sess_hdl,
                dev_id,
                entry_hdl,
                action_spec
            )

            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(port2, dev_id)])

            print("deleting * entry")
            status = self.client.ipv4_alpm_table_delete( sess_hdl,
                dev_id,
                entry_hdl
            )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))

class TestTcamMove(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Test TCAM moves """
    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        test_matrix = [
                       ("10.0.0.1", 0, swports[4], "20.20.20.20"),
                       ("10.0.0.1", 8, swports[2], "10.20.20.20"),
                       ("10.0.0.1", 16, swports[3], "10.0.20.20"),
                       ("10.0.0.1", 24, swports[4], "10.0.0.20"),
                       ("10.0.0.1", 32, swports[2], "10.0.0.1"),
                       ]

        entry_hdls = []

        try:
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                match_spec = alpm_test_ipv4_alpm_match_spec_t(
                    0,
                    ipv4Addr_to_i32(match_addr),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                print("adding entries")
                entry_hdls.append(self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))

            self.conn_mgr.complete_operations(sess_hdl)
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s' % ip_dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

            test_matrix_move = [
                           ("86.0.0.1", 16, swports[2], "86.0.20.20"),
                           ("86.0.0.1", 8, swports[3], "86.20.20.20"),
                           ]

            for (match_addr, match_len, port, ip_dst) in test_matrix_move:
                match_spec = alpm_test_ipv4_alpm_match_spec_t(
                    0,
                    ipv4Addr_to_i32(match_addr),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                print("adding entries")
                entry_hdls.append(self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))

            test_matrix += test_matrix_move

            self.conn_mgr.complete_operations(sess_hdl)
            # Verify the original entries (which got moved) and the current entries
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s' % ip_dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

        finally:
            print(("Deleting %d entries" % len(entry_hdls)))
            for entry_hdl in entry_hdls:
                print(("Deleting 0x%08x" % entry_hdl))
                status = self.client.ipv4_alpm_table_delete( sess_hdl,
                    dev_id,
                    entry_hdl
                )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))
        return

@group("thorough")
class TestManyEntries(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        total_entries = 50
        population = list(range(256*256))
        entries = random.sample(population, total_entries)
        match_addrs = [ "10.0.%d.%d" % ( x//256, x % 256) for x in entries]
        match_lens  = [ 32 ] * total_entries
        ports       = [ swports[(x % 4) + 1] for x in entries]

        test_matrix = list(zip(match_addrs, match_lens, ports, match_addrs))

        entry_hdls = []
        print(("Adding %d entries" % total_entries))
        try:
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                match_spec = alpm_test_ipv4_alpm_match_spec_t(
                    0,
                    ipv4Addr_to_i32(match_addr),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))

            self.conn_mgr.complete_operations(sess_hdl)
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s' % ip_dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

        finally:
            print(("Deleting %d entries" % len(entry_hdls)))
            for entry_hdl in entry_hdls:
                status = self.client.ipv4_alpm_table_delete(
                    sess_hdl,
                    dev_id,
                    entry_hdl
                )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))
        return

class TestNegativeOverflowAtomicTransaction(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    def singleRunAtomicTransaction(self, sess_hdl, dev_tgt, test_matrix, total_entries):
        entry_hdls = []
        print(("Adding %d entries" % total_entries))
        self.conn_mgr.begin_txn(sess_hdl, True)
        capacity = 0
        try:
            for (match_addr, match_len, port, ip_dst, vrf) in test_matrix:
                match_spec = alpm_test_ipv4_alpm_match_spec_t(
                    vrf,
                    ipv4Addr_to_i32(match_addr),
                    match_len
                )

                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))
        except InvalidTableOperation:
            self.conn_mgr.complete_operations(sess_hdl)
            print(("Capacity at %d entries!" % len(entry_hdls)))
            capacity = len(entry_hdls)
            self.conn_mgr.complete_operations(sess_hdl)

        assert capacity > 6000
        return capacity

    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        total_entries = 256 * 32
        entries = list(range(total_entries))

        match_addrs = [ "10.0.%d.%d" % ( x//256, x % 256) for x in entries]
        match_lens  = [ 31 + (x % 2)  for x in entries]
        ports       = [ swports[(x % 4) + 1] for x in entries]
        vrfs        = [ x//256 for x in entries]

        test_matrix = list(zip(match_addrs, match_lens, ports, match_addrs, vrfs))
        try:
            initialCapacity = self.singleRunAtomicTransaction(sess_hdl, dev_tgt, test_matrix, total_entries)
            # Repeat the test a few times and verify if the capacity has not changed.
            for i in range(5):
                capacity = self.singleRunAtomicTransaction(sess_hdl, dev_tgt, test_matrix, total_entries)
                assert(capacity == initialCapacity)
        finally:
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))

        return

class TestCoveringPrefix(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        total_entries = 10
        match_addrs = [ "10.0.%d.%d" % ( x//256, x % 256) for x in range(total_entries)]
        match_lens  = [ 32 ] * total_entries
        ports       = [ swports[(x % 4) + 1] for x in range(total_entries)]

        test_matrix = list(zip(match_addrs, match_lens, ports, match_addrs))

        entry_hdls = []
        print(("Adding %d entries" % total_entries))
        try:
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                match_spec = alpm_test_ipv4_alpm_match_spec_t(
                    0,
                    ipv4Addr_to_i32(match_addr),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))

            self.conn_mgr.complete_operations(sess_hdl)
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s' % ip_dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

            print("Deleting some entries")
            status = self.client.ipv4_alpm_table_delete(
                sess_hdl,
                dev_id,
                entry_hdls.pop(5)
            )
            status = self.client.ipv4_alpm_table_delete(
                sess_hdl,
                dev_id,
                entry_hdls.pop(0)
            )

            print("Adding covering prefix")
            match_spec = alpm_test_ipv4_alpm_match_spec_t(
                0,
                ipv4Addr_to_i32("10.0.0.0"),
                29
            )
            action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(swports[5])
            cp_entry_hdl = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
                sess_hdl,
                dev_tgt,
                match_spec,
                action_spec
            )

            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.0',
                                    ip_id=101,
                                    ip_ttl=64)
            print('Sending pkt with ip-dst 10.0.0.0')
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(swports[5], dev_id)])

            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.5',
                                    ip_id=101,
                                    ip_ttl=64)
            print('Sending pkt with ip-dst 10.0.0.5')
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(swports[5], dev_id)])

            print("Adding more general covering prefix")
            match_spec = alpm_test_ipv4_alpm_match_spec_t(
                0,
                ipv4Addr_to_i32("10.0.0.0"),
                25
            )
            action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(swports[6])
            cp_entry_hdl_2 = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
                sess_hdl,
                dev_tgt,
                match_spec,
                action_spec
            )
            print("Deleting first covering prefix")
            status = self.client.ipv4_alpm_table_delete(
                sess_hdl,
                dev_id,
                cp_entry_hdl
            )

            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.0',
                                    ip_id=101,
                                    ip_ttl=64)
            print('Sending pkt with ip-dst 10.0.0.0')
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(swports[6], dev_id)])

            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.5',
                                    ip_id=101,
                                    ip_ttl=64)
            print('Sending pkt with ip-dst 10.0.0.5')
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(swports[6], dev_id)])



            print("Adding subtree root")
            match_spec = alpm_test_ipv4_alpm_match_spec_t(
                0,
                ipv4Addr_to_i32("10.0.0.4"),
                30
            )
            action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(swports[7])
            root_entry_hdl = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
                sess_hdl,
                dev_tgt,
                match_spec,
                action_spec
            )

            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.0',
                                    ip_id=101,
                                    ip_ttl=64)
            print('Sending pkt with ip-dst 10.0.0.0')
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(swports[6], dev_id)])

            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.4',
                                    ip_id=101,
                                    ip_ttl=64)
            print('Sending pkt with ip-dst 10.0.0.4')
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(swports[1], dev_id)])

            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.5',
                                    ip_id=101,
                                    ip_ttl=64)
            print('Sending pkt with ip-dst 10.0.0.5')
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(swports[7], dev_id)])

        finally:
            print(("Deleting %d entries" % len(entry_hdls)))
            for entry_hdl in entry_hdls:
                print(("Deleting 0x%08x" % entry_hdl))
                status = self.client.ipv4_alpm_table_delete(
                    sess_hdl,
                    dev_id,
                    entry_hdl
                )
            print("Deleting covering prefix")
            status = self.client.ipv4_alpm_table_delete(
                sess_hdl,
                dev_id,
                cp_entry_hdl_2
            )
            print("Deleting root entry")
            status = self.client.ipv4_alpm_table_delete(
                sess_hdl,
                dev_id,
                root_entry_hdl
            )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))
        return

@group("thorough")
class TestCoveringPrefix2(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        print("Switch Ports: ")
        print(swports)
        print("Adding global covering prefix")
        match_spec = alpm_test_ipv4_alpm_small_match_spec_t(
            0,
            ipv4Addr_to_i32("0.0.0.0"),
            1
        )
        action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(swports[5])
        cp_entry_hdl_g = self.client.ipv4_alpm_small_table_add_with_ipv4_lpm_hit(
            sess_hdl,
            dev_tgt,
            match_spec,
            action_spec
        )

        total_entries = 256
        match_addrs = [ "10.10.10.%d" % (((x * 2) + 1) % 256) for x in range(total_entries // 2)]
        match_lens  = [ 32 ] * total_entries
        ports       = [ swports[(x % 3) + 1] for x in range(total_entries)]

        test_matrix = list(zip(match_addrs, match_lens, ports, match_addrs))

        entry_hdls = []
        print(("Adding %d entries" % total_entries))
        try:
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                print(("Adding IP %s" % ip_dst))
                match_spec = alpm_test_ipv4_alpm_small_match_spec_t(
                    0,
                    ipv4Addr_to_i32(match_addr),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_small_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))

            self.conn_mgr.complete_operations(sess_hdl)

            for x in range(total_entries // 2):
                dst = '10.10.10.%d' % (x*2)
                print(dst)
                port = swports[5]
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s for global covering prefix port 15' % dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

            print("Adding better covering prefix")
            match_spec = alpm_test_ipv4_alpm_small_match_spec_t(
                0,
                ipv4Addr_to_i32("10.10.10.0"),
                24
            )
            action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(swports[4])
            cp_entry_hdl = self.client.ipv4_alpm_small_table_add_with_ipv4_lpm_hit(
                sess_hdl,
                dev_tgt,
                match_spec,
                action_spec
            )

            self.conn_mgr.complete_operations(sess_hdl)
            for x in range(total_entries // 2):
                dst = '10.10.10.%d' % (x*2)
                print(dst)
                port = swports[4]
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s for better covering prefix port 14' % dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

        finally:
            print(("Deleting %d entries" % len(entry_hdls)))
            for entry_hdl in entry_hdls:
                print(("Deleting 0x%08x" % entry_hdl))
                status = self.client.ipv4_alpm_small_table_delete(
                    sess_hdl,
                    dev_id,
                    entry_hdl
                )
            print("Deleting covering prefix entries")
            status = self.client.ipv4_alpm_small_table_delete(
                sess_hdl,
                dev_id,
                cp_entry_hdl
            )

            status = self.client.ipv4_alpm_small_table_delete(
                sess_hdl,
                dev_id,
                cp_entry_hdl_g
            )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used for the file %d' %(seed)))
        return


@group("thorough")
class TestCapacity(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        total_entries = 200000
        entries = random.sample(list(range(2**28)), total_entries)
        match_addrs = [ i32_to_ipv4Addr(x) for x in entries]
        match_lens  = [ 32 ] * total_entries
        ports       = [ swports[(x % 4) + 2] for x in range(total_entries)]

        test_matrix = list(zip(match_addrs, match_lens, ports, match_addrs))

        entry_hdls = []
        print(("Attempting to add %d entries" % total_entries))
        try:
            for (match_addr, match_len, port, ip_dst) in test_matrix:
                match_spec = alpm_test_ipv4_alpm_large_match_spec_t(
                    0,
                    ipv4Addr_to_i32(match_addr),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_large_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))
                if len(entry_hdls) % 10000 == 0:
                    print(("%d entries added" % len(entry_hdls)))

        except InvalidTableOperation:
            self.conn_mgr.complete_operations(sess_hdl)
            print(("Capacity at %d entries!" % len(entry_hdls)))
            test_set = random.sample(list(range(len(entry_hdls))), 2000)
            util = len(entry_hdls) * 100.0 / total_entries
            assert util > 95, 'Utilization has not reached optimum level %.2f%%' % (util)
            for idx in test_set:
                (match_addr, match_len, port, ip_dst) = test_matrix[idx]
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s' % ip_dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

        finally:
            print(("Deleting %d entries" % len(entry_hdls)))
            for entry_hdl in entry_hdls:
                status = self.client.ipv4_alpm_large_table_delete(
                    sess_hdl,
                    dev_id,
                    entry_hdl
                )
            util = len(entry_hdls) * 100.0 / total_entries
            print(("Utilization for %d entries with 1024 partitions and 4 subtrees per partition: %.2f%%" % (total_entries, util)))

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))
        return

@group("thorough")
class TestRealData(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    def runTest(self):
        InstallAllDefaultEntries(self)
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        total_entries = 30000
        test_matrix = readData(total_entries)

        entry_hdls = []
        print(("Adding %d entries" % len(test_matrix)))
        try:
            for (ip_dst, match_len, port) in test_matrix:
                match_spec = alpm_test_ipv4_alpm_large_match_spec_t(
                    0,
                    ipv4Addr_to_i32(ip_dst),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_large_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))
            self.conn_mgr.complete_operations(sess_hdl)

            test_matrix_sample = [test_matrix[i] for i in random.sample(list(range(len(test_matrix))), 30)]
            for (ip_dst, match_len, port) in test_matrix_sample:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s port %s' % (ip_dst, port)))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

            print("Deleting half of our entries")
            delete_indices = random.sample(list(range(len(test_matrix))), len(test_matrix) // 2)
            delete_hdls = []
            for idx in delete_indices:
                delete_hdls.append(entry_hdls[idx])
                status = self.client.ipv4_alpm_large_table_delete(
                    sess_hdl,
                    dev_id,
                    entry_hdls[idx]
                )

            self.conn_mgr.complete_operations(sess_hdl)
            test_matrix_sample = [i for i in random.sample(list(range(len(test_matrix))), 20)]
            for i in test_matrix_sample:
                (ip_dst, match_len, port) = test_matrix[i]
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s' % ip_dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                dst_port = lpm(test_matrix, delete_indices, ip_dst)
                if dst_port == -1:
                    verify_no_other_packets(self)
                else:
                    verify_packets(self, pkt, [tofLocalPortToOfPort(dst_port, dev_id)])

        finally:
            print(("Deleting %d entries" % (len(entry_hdls) - len(delete_hdls))))
            for entry_hdl in entry_hdls:
                if entry_hdl not in delete_hdls:
                    status = self.client.ipv4_alpm_large_table_delete(
                        sess_hdl,
                        dev_id,
                        entry_hdl
                    )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))
        return


class TestStateRestore(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        self.client.ipv4_alpm_set_default_action_nop(sess_hdl, dev_tgt)

        match_spec = alpm_test_ipv4_alpm_match_spec_t(
            0,
            ipv4Addr_to_i32("10.0.0.1"),
            0
        )
        port = swports[3]
        action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
        print("adding entry")
        entry_hdl = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit(
            sess_hdl,
            dev_tgt,
            match_spec,
            action_spec
        )

        print(("PIPE_MGR gave me that entry handle:", entry_hdl))

        try:
            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.log_state(dev_id, "test_log_file".encode())
            print()
            print(("Sending packet port %s -> port %s (192.168.0.1 -> 10.0.0.1 [id = 101])" % (swports[1], swports[3])))
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

        finally:
            print("deleting entry")
            status = self.client.ipv4_alpm_table_delete( sess_hdl,
                dev_id,
                entry_hdl
            )
            self.client.ipv4_alpm_table_reset_default_entry(sess_hdl, dev_tgt)
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used for the file %d' %(seed)))

        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))
        print("Restoring state")
        self.conn_mgr.restore_state(dev_id, "test_log_file".encode())
        print("State restored")

        try:
            print()
            print(("Sending packet port %s -> port %s (192.168.0.1 -> 10.0.0.1 [id = 101])" % (swports[1], swports[3])))
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

        finally:
            print("deleting entry")
            status = self.client.ipv4_alpm_table_delete( sess_hdl,
                dev_id,
                entry_hdl
            )
            self.client.ipv4_alpm_table_reset_default_entry(sess_hdl, dev_tgt)
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used for the file %d' %(seed)))


@group("thorough")
class TestStateRestoreLarge(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        self.client.ipv4_alpm_large_set_default_action_nop(sess_hdl, dev_tgt)

        total_entries = 30000
        test_matrix = readData(total_entries)

        entry_hdls = []
        print(("Adding %d entries" % (len(test_matrix) // 2)))
        try:
            for idx in range(len(test_matrix)//2):
                (ip_dst, match_len, port) = test_matrix[idx]
                match_spec = alpm_test_ipv4_alpm_large_match_spec_t(
                    0,
                    ipv4Addr_to_i32(ip_dst),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_large_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))
            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.log_state(dev_id, "test_log_file".encode())

            test_matrix_sample = [test_matrix[i] for i in random.sample(list(range(len(test_matrix)//2)), 15)]
            for (ip_dst, match_len, port) in test_matrix_sample:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s port %s' % (ip_dst, port)))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

        finally:
            print(("Deleting %d entries" % len(entry_hdls)))
            for entry_hdl in entry_hdls:
                status = self.client.ipv4_alpm_large_table_delete(
                    sess_hdl,
                    dev_id,
                    entry_hdl
                )
            self.client.ipv4_alpm_large_table_reset_default_entry(sess_hdl, dev_tgt)
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used for the file %d' %(seed)))

        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))
        print("Restoring state...")
        self.conn_mgr.restore_state(dev_id, "test_log_file".encode())
        print(("%d entries restored" % (len(test_matrix) // 2)))

        try:
            print(("Adding %d more entries" % (len(test_matrix) // 2)))
            for idx in range(len(test_matrix)//2, len(test_matrix)):
                (ip_dst, match_len, port) = test_matrix[idx]
                match_spec = alpm_test_ipv4_alpm_large_match_spec_t(
                    0,
                    ipv4Addr_to_i32(ip_dst),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_large_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))
            self.conn_mgr.complete_operations(sess_hdl)

            test_matrix_sample = [i for i in random.sample(list(range(len(test_matrix))), 15)]
            for i in test_matrix_sample:
                (ip_dst, match_len, port) = test_matrix[i]
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s' % ip_dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                verify_packets(self, pkt, [tofLocalPortToOfPort(port, dev_id)])

            print("Deleting half of our entries")
            delete_indices = random.sample(list(range(len(test_matrix))), len(test_matrix) // 2)
            delete_hdls = []
            for idx in delete_indices:
                delete_hdls.append(entry_hdls[idx])
                status = self.client.ipv4_alpm_large_table_delete(
                    sess_hdl,
                    dev_id,
                    entry_hdls[idx]
                )

            self.conn_mgr.complete_operations(sess_hdl)
            test_matrix_sample = [i for i in random.sample(list(range(len(test_matrix))), 15)]
            for i in test_matrix_sample:
                (ip_dst, match_len, port) = test_matrix[i]
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_id=101,
                                        ip_ttl=64)
                print(('Sending pkt with ip-dst %s' % ip_dst))
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                pkt["IP"].ttl = pkt["IP"].ttl - 1
                dst_port = lpm(test_matrix, delete_indices, ip_dst)
                if dst_port == -1:
                    verify_no_other_packets(self)
                else:
                    verify_packets(self, pkt, [tofLocalPortToOfPort(dst_port, dev_id)])

        finally:
            print(("Deleting %d entries" % (len(entry_hdls) - len(delete_hdls))))
            for entry_hdl in entry_hdls:
                if entry_hdl not in delete_hdls:
                    status = self.client.ipv4_alpm_large_table_delete(
                        sess_hdl,
                        dev_id,
                        entry_hdl
                    )
            self.client.ipv4_alpm_large_table_reset_default_entry(sess_hdl, dev_tgt)

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used for the file %d' %(seed)))
        return


class IdleNotifyHelper():
    '''Helper function for notify tests'''
    def __init__(self, fns, params, idle_params, dataplane, thriftinterface, tbl_params, min_entry_ttl = None):
        self.thriftinterface = thriftinterface
        self.dataplane = dataplane
        self.fns = fns
        self.sess_hdl, self.dev_id, self.port, self.ig_port, self.scopes = params
        self.dev_tgt = DevTarget_t(self.dev_id, hex_to_i16(0xFFFF))
        # If scope test is being done
        if self.scopes == 1:
            print("Setting scopes")
            self.fns["reset_default"](self.sess_hdl, self.dev_tgt)
            self.dev_tgt_0 = DevTarget_t(self.dev_id, 0)
            self.dev_tgt_2 = DevTarget_t(self.dev_id, 2)
            #Set the scopes
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_USER_DEFINED
            # pipes 0,1 in one scope, pipes 2,3 in other scope
            prop_args = 0x0c03
            self.fns["set_property"](self.sess_hdl, self.dev_id, prop, prop_val, prop_args)

        self.cookie = 12

        self.min_ttl, self.query_ttl, self.max_ttl = idle_params
        if min_entry_ttl is None:
            self.min_entry_ttl = self.min_ttl
        else:
            self.min_entry_ttl = min_entry_ttl
        self.bit_width, self.twoway, self.perflowdisable = tbl_params

        assert self.max_ttl >= max(self.query_ttl, self.min_ttl)
        print(('TTL Max %d Min %d query %d' % (self.max_ttl, self.min_ttl, self.query_ttl)))

        self.calc_sweep_period()

        self.max_error = datetime.timedelta.min
        self.min_error = datetime.timedelta.max

        assert self.twoway == True

    def calc_sweep_period(self):
        if test_param_get('arch') == "tofino":
            if test_param_get('target') == "asic-model":
                clock_speed = 1271000000
            else:
                clock_speed = self.thriftinterface.devport_mgr.devport_mgr_get_clock_speed(self.dev_id).bps_clock_speed
        else:
            clock_speed = 1000000000
        notify_time = min(self.query_ttl, self.min_ttl)

        no_states = (1 << self.bit_width)-1

        if self.twoway:
            no_states -= 1
        if self.perflowdisable:
            no_states -= 1

        sweep_count = notify_time * (clock_speed // 1000)
        sweep_count //= no_states
        sweep_count >>= 21

        sweep_interval = min(15, int(math.ceil(math.log(sweep_count, 2))))
        self.hw_sweep_period = (1000 << (sweep_interval + 21))//clock_speed

        self.hw_notify_period = self.hw_sweep_period * no_states

        if self.query_ttl == self.min_ttl and self.min_ttl == self.max_ttl:
            if notify_time > self.hw_notify_period:
                self.sw_sweep_period = notify_time
            else:
                self.sw_sweep_period = 0
        else:
            self.sw_sweep_period = notify_time

        print(('HW notify period %d sw_sweep_period %d' % (self.hw_notify_period, self.sw_sweep_period)))

        if self.twoway:
            self.twowayerr = self.hw_sweep_period + self.sw_sweep_period
        else:
            self.twowayerr = 0

    def calc_err_margin(self, ttl, active_err=None):
        if active_err is None:
            active_err = self.twowayerr
        if self.sw_sweep_period:
            err = self.sw_sweep_period
            err += self.sw_sweep_period - (abs(ttl-self.hw_notify_period) % self.sw_sweep_period)
        else:
            err = abs(ttl-self.hw_notify_period)
        err += active_err
        # Account for some more error due to the communication - 1000ms
        err += 1000
        return err

    def enable_notify_mode(self):
        idle_tbl_params = alpm_test_idle_time_params_t (
                alpm_test_idle_time_mode.NOTIFY_MODE,
                self.query_ttl,
                self.max_ttl,
                self.min_ttl,
                self.cookie
                )
        if test_param_get('target') != "bmv2":
            self.fns["idle_tmo_enable"](self.sess_hdl,
                    self.dev_id,
                    idle_tbl_params)

    def disable_notify_mode(self):
        self.fns["idle_tmo_disable"](self.sess_hdl,
                self.dev_id)

    def add_match_entry(self, match_entries, ip_dst, ttl, match_len = 32):
        match_key_ip_dst = ip_dst

        match_spec = self.fns["match_spec_create"](ipv4Addr_to_i32(match_key_ip_dst), match_len)
        act_spec = alpm_test_ipv4_lpm_hit_action_spec_t(self.port)
        if self.scopes == 1:
            ent_hdl = self.fns["mat_ent_add"](self.sess_hdl, self.dev_tgt_0, match_spec, act_spec, ttl)
        else:
            ent_hdl = self.fns["mat_ent_add"](self.sess_hdl, self.dev_tgt, match_spec, act_spec, ttl)
        self.fns["complete_operations"](self.sess_hdl)
        now = datetime.datetime.now()
        match_entries[ent_hdl] = OrderedDict()
        match_entries[ent_hdl]["ip_dst"] = ip_dst
        match_entries[ent_hdl]["active_time"] = now
        match_entries[ent_hdl]["active_err"] = 0
        if ttl:
            state = "active"
        else:
            state = "inactive"
        match_entries[ent_hdl]["state"] = state
        match_entries[ent_hdl]["ttl"] = ttl
        print(('Entry 0x%x add entry ttl %d state-%s time %s' % (ent_hdl, ttl, state, now)))
        return ent_hdl

    def del_match_entry(self, match_entries, ent_hdl, delete=True):
        self.fns["mat_ent_del"](self.sess_hdl, self.dev_id, ent_hdl)
        now = datetime.datetime.now()
        print(('Entry 0x%x delete time %s' % (ent_hdl, now)))
        if delete:
            del match_entries[ent_hdl]

    def set_ttl(self, match_entries, ent_hdl, ttl):
        self.fns["set_ttl"](self.sess_hdl, self.dev_id, ent_hdl, ttl)
        self.fns["complete_operations"](self.sess_hdl)
        now = datetime.datetime.now()
        old_state = match_entries[ent_hdl]["state"]
        old_ttl = match_entries[ent_hdl]["ttl"]
        old_active_time = match_entries[ent_hdl]["active_time"]

        new_active_time = None
        if (now - old_active_time) >= datetime.timedelta(milliseconds=old_ttl):
            new_active_time = now - datetime.timedelta(milliseconds=old_ttl)

        if old_state == 'inactive':
            new_active_time = now

        if new_active_time:
            match_entries[ent_hdl]["active_time"] = new_active_time

        match_entries[ent_hdl]["active_err"] = self.sw_sweep_period
        if ttl:
            state = "active"
        else:
            state = "inactive"
        match_entries[ent_hdl]["state"] = state
        match_entries[ent_hdl]["ttl"] = ttl
        print(('Entry 0x%x set ttl %d old-ttl %d state-%s time %s' % (ent_hdl, ttl, old_ttl, state, now)))

    def verify_ttl(self, match_entries, aged_entries):

        print('Verifying TTL')
        for ent_hdl in match_entries:
            active_time = match_entries[ent_hdl]["active_time"]
            ttl = match_entries[ent_hdl]["ttl"]
            state = match_entries[ent_hdl]["state"]
            active_err = match_entries[ent_hdl]["active_err"]

            now = datetime.datetime.now()
            cur_ttl = self.fns["get_ttl"](self.sess_hdl, self.dev_id, ent_hdl)

            if ttl == 0:
                print(('Entry 0x%x DISABLED' % (ent_hdl)))
                continue

            err_margin = self.calc_err_margin(ttl, active_err)
            print(('Entry 0x%x Error margin %d active_err %d' % (ent_hdl, err_margin, active_err)))
            max_entry_alive_time = datetime.timedelta(milliseconds=(ttl+err_margin))
            # If a sweep happens right after activate, then the state changes
            # a little ahead of time, so account for this

            abs_entry_alive_time = datetime.timedelta(milliseconds=ttl)
            min_entry_alive_time = datetime.timedelta(milliseconds=(ttl-self.hw_sweep_period))

            diff = now - active_time

            max_ttl_diff = datetime.timedelta(milliseconds=(ttl-cur_ttl+err_margin))
            min_ttl_diff = datetime.timedelta(milliseconds=(ttl-cur_ttl))

            print(('Now %s Diff %s cur_ttl %d ttl %d ' % (now, diff,  cur_ttl, ttl)))
            assert cur_ttl <= ttl, 'Current ttl %d is greater than entry-ttl %d' % (cur_ttl, ttl)
            if state == "idle":
                assert cur_ttl == 0, 'Current ttl %d is not zero for idle entry' % (cur_ttl)
                continue

            if diff < datetime.timedelta(milliseconds=active_err):
                # In this case, the cur_ttl may not be accurate
                pass
            else:
                if diff >= max_entry_alive_time:
                    assert cur_ttl == 0, 'Current ttl %d is not zero' % (cur_ttl)
                else:
                    if diff < min_entry_alive_time :
                        assert cur_ttl != 0, 'Current ttl is zero'
#                    assert diff <= max_ttl_diff, 'The TTL decrement is too less ent 0x%x' % ent_hdl
#                    assert diff >= min_ttl_diff, 'The TTL decrement is too fast ent 0x%x' % ent_hdl

                if cur_ttl == 0:
                    aged_entries.add(ent_hdl)
                    match_entries[ent_hdl]["state"] = "idle"
                    if (diff - abs_entry_alive_time) > self.max_error:
                        self.max_error = diff - abs_entry_alive_time
                    if (diff - abs_entry_alive_time) < self.min_error:
                        self.min_error = diff - abs_entry_alive_time

    def count_active_entries(self, match_entries):
        active_entries = [x for x in match_entries if match_entries[x]["state"] == "active"]
        return len(active_entries)

    def get_expired_entries(self, expired_entries):
        expired = self.fns["get_expired"](self.sess_hdl, self.dev_id)
        cur_expired_entries = set([x.entry for x in expired])

        expired_entries |= cur_expired_entries

    def sweep_check(self, match_entries):
        aged_entries = set()
        expired_entries = set()

        # Give some buffer
        time_period_msecs = min(self.min_ttl, self.query_ttl)+100
        time_period=datetime.timedelta(milliseconds=time_period_msecs)
        total_time_slept = datetime.timedelta()

        total_time_to_sleep = datetime.timedelta(milliseconds=self.max_ttl)
        # Add the maximum error margin
        total_time_to_sleep += datetime.timedelta(milliseconds=self.calc_err_margin(self.hw_notify_period))
        total_time_to_sleep += datetime.timedelta(milliseconds=2000)

        first_called = datetime.datetime.now()
        num_calls=0
        drift=datetime.timedelta()
        while total_time_slept < total_time_to_sleep:
            total_time_slept += time_period
            time.sleep((time_period_msecs/1000.0)-(drift.microseconds/1000000.0))
            current_time = datetime.datetime.now()
            self.verify_ttl(match_entries, aged_entries)
            self.get_expired_entries(expired_entries)
            if self.count_active_entries(match_entries) == 0:
                break
            num_calls += 1
            difference = current_time - first_called
            drift = difference - time_period* num_calls

        self.get_expired_entries(expired_entries)

        print(('Aged entries', aged_entries))
        print(('Expired entries', expired_entries))
        assert expired_entries >= aged_entries

    def send_matching_pkts(self, match_entries, in_pkts):
        self.fns["complete_operations"](self.sess_hdl)
        debug = False

        for (pkt, ent_hdl) in in_pkts:
            print(("Sending packet ent 0x%x ip_dst %s" % (ent_hdl, match_entries[ent_hdl]["ip_dst"])))
            send_packet(self.thriftinterface, tofLocalPortToOfPort(self.ig_port, self.dev_id), pkt)
            (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll(port_number=tofLocalPortToOfPort(self.port, self.dev_id),timeout=5)
            assert rcv_pkt != None, "No packet received"
            now = datetime.datetime.now()
            match_entries[ent_hdl]["active_time"] = now
            match_entries[ent_hdl]["active_err"] = self.twowayerr
            ttl = match_entries[ent_hdl]["ttl"]
            if ttl:
                match_entries[ent_hdl]["state"] = "active"
            state = match_entries[ent_hdl]["state"]
            print(('Entry 0x%x send pkt %d state-%s time %s' % (ent_hdl, ttl, state, now)))

    def test(self):
        NUM_MAT_ENTRIES = 9
        match_entries = OrderedDict()
        try:
            self.enable_notify_mode()

            ip_dsts = ["10.0.%d.%d" % (x//255, x%255) for x in range(NUM_MAT_ENTRIES)]
            prefix_len = 32

            # Add 1/2 entries now
            for ip_dst in ip_dsts[:len(ip_dsts)//2]:
                ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                self.add_match_entry(match_entries, ip_dst, ttl, match_len=prefix_len)

            in_pkts = [(simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=match_entries[ent_hdl]["ip_dst"],
                                        ip_ttl=64,
                                        with_tcp_chksum = False), ent_hdl) for ent_hdl in match_entries]

            self.send_matching_pkts(match_entries, in_pkts)

            for ip_dst in ip_dsts[len(ip_dsts)//2:]:
                ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                self.add_match_entry(match_entries, ip_dst, ttl, match_len=prefix_len)
            # Update the moved atcam entries
            now = datetime.datetime.now()

            self.sweep_check(match_entries)


            in_pkts = [(simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=match_entries[ent_hdl]["ip_dst"],
                                        ip_ttl=64,
                                        with_tcp_chksum = False), ent_hdl) for ent_hdl in match_entries]

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))
            self.sweep_check(match_entries)

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

            time.sleep(((self.hw_notify_period+100)/1000.0))

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))
            self.sweep_check(match_entries)

            #Make entries active again
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))
            time.sleep(((self.hw_notify_period+100)/1000.0))

            # Delete some entries
#            entries_to_delete = random.sample(match_entries.keys(), 1+ ((NUM_MAT_ENTRIES-1)//4))
            entries_to_delete = list(match_entries.keys())
            ip_dsts_deleted = []

            for ent_hdl in entries_to_delete:
                ip_dsts_deleted.append(match_entries[ent_hdl]["ip_dst"])
                self.del_match_entry(match_entries, ent_hdl)

            for ip_dst in ip_dsts_deleted:
                ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                self.add_match_entry(match_entries, ip_dst, ttl)

            self.sweep_check(match_entries)

            in_pkts = [(simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=match_entries[ent_hdl]["ip_dst"],
                                        ip_ttl=64,
                                        with_tcp_chksum = False), ent_hdl) for ent_hdl in match_entries]

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

            time.sleep(((self.hw_notify_period+100)/1000.0))

#            entries_to_set_ttl = random.sample(match_entries.keys(), 1+ ((NUM_MAT_ENTRIES-1)//4))
            entries_to_set_ttl = list(match_entries.keys())
            for ent_hdl in entries_to_set_ttl:
                ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                self.set_ttl(match_entries, ent_hdl, ttl)

            self.sweep_check(match_entries)

        finally:
            for ent_hdl in match_entries:
                self.del_match_entry(match_entries, ent_hdl, delete=False)
            self.fns["complete_operations"](self.sess_hdl)
            self.disable_notify_mode()
            print(('MAX ERROR %s MIN ERROR %s' % (self.max_error, self.min_error)))
            if self.scopes == 1:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                self.fns["set_property"](self.sess_hdl, self.dev_id, prop, prop_val, 0)
            print(('Seed used for the file %d' %(seed)))

class IdlePollHelper():
    '''Helper function for poll tests'''
    def __init__(self, fns, params, dataplane, thriftinterface):
        self.thriftinterface = thriftinterface
        self.dataplane = dataplane
        self.fns = fns
        self.sess_hdl, self.dev_id, self.port, self.ig_port, self.scopes = params
        self.dev_tgt = DevTarget_t(self.dev_id, hex_to_i16(0xFFFF))

        # If scope test is being done
        if self.scopes == 1:
            self.fns["reset_default"](self.sess_hdl, self.dev_tgt)
            print("Setting scopes")
            self.dev_tgt_0 = DevTarget_t(self.dev_id, 0)
            self.dev_tgt_2 = DevTarget_t(self.dev_id, 2)
            #Set the scopes
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_USER_DEFINED
            # pipes 0,1 in one scope, pipes 2,3 in other scope
            prop_args = 0x0c03
            self.fns["set_property"](self.sess_hdl, self.dev_id, prop, prop_val, prop_args)

    def enable_poll_mode(self):
        idle_tbl_params = alpm_test_idle_time_params_t (
                alpm_test_idle_time_mode.POLL_MODE
                )
        if test_param_get('target') != "bmv2":
            self.fns["idle_tmo_enable"](self.sess_hdl,
                    self.dev_id,
                    idle_tbl_params)

    def disable_poll_mode(self):
        self.fns["idle_tmo_disable"](self.sess_hdl,
                self.dev_id)

    def add_match_entry(self, match_entries, ip_dst, match_len = 32):
        match_key_ip_dst = ip_dst

        match_spec = self.fns["match_spec_create"](ipv4Addr_to_i32(match_key_ip_dst), match_len)
        act_spec = alpm_test_ipv4_lpm_hit_action_spec_t(self.port)
        ttl = 0
        if self.scopes == 1:
            ent_hdl = self.fns["mat_ent_add"](self.sess_hdl, self.dev_tgt_0, match_spec, act_spec, ttl)
        else:
            ent_hdl = self.fns["mat_ent_add"](self.sess_hdl, self.dev_tgt, match_spec, act_spec, ttl)
        match_entries[ip_dst] = OrderedDict()
        match_entries[ip_dst]["entry_hdl"] = ent_hdl
        match_entries[ip_dst]["state"] = alpm_test_idle_time_hit_state.ENTRY_IDLE
        return ent_hdl

    def del_match_entry(self, match_entries, ip_dst, delete=True):
        ent_hdl = match_entries[ip_dst]["entry_hdl"]
        self.fns["mat_ent_del"](self.sess_hdl, self.dev_id, ent_hdl)
        if delete:
            del match_entries[ip_dst]

    def verify_hit_state(self, match_entries):
        self.fns["complete_operations"](self.sess_hdl)
        self.fns["update_hit_state"](self.sess_hdl, self.dev_id)

        for ip_dst in match_entries:
            ent_hdl = match_entries[ip_dst]["entry_hdl"]
            state = match_entries[ip_dst]["state"]

            hit_state = self.fns["get_hit_state"](self.sess_hdl, self.dev_id, ent_hdl)
            assert state == hit_state, "State %d hit_state %d ent_hdl 0x%x" % (state, hit_state, ent_hdl)

            # Reset the hit-state
            match_entries[ip_dst]["state"] = alpm_test_idle_time_hit_state.ENTRY_IDLE
        time.sleep(2)

    def send_matching_pkts(self, match_entries, in_pkts):
        self.fns["complete_operations"](self.sess_hdl)
        debug = False

        for (pkt, ip_dst) in in_pkts:
            print(("Sending packet ip_dst %s" % ip_dst))
            send_packet(self.thriftinterface, tofLocalPortToOfPort(self.ig_port, self.dev_id), pkt)
            (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll(port_number=tofLocalPortToOfPort(self.port, self.dev_id),timeout=5)
            assert rcv_pkt != None
            match_entries[ip_dst]["state"] = alpm_test_idle_time_hit_state.ENTRY_ACTIVE

    def test(self):
        NUM_MAT_ENTRIES = 9
        match_entries = OrderedDict()
        try:
            self.enable_poll_mode()
            self.verify_hit_state(match_entries)

            ip_dsts = ["10.0.%d.%d" % (x//255, x%255) for x in range(NUM_MAT_ENTRIES)]
            prefix_len = 32

            in_pkts = [(simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_ttl=64,
                                        with_tcp_chksum = False), ip_dst) for ip_dst in ip_dsts]

            # Add 1/2 entries now
            for ip_dst in ip_dsts[:len(ip_dsts)//2]:
                ent_hdl = self.add_match_entry(match_entries, ip_dst, match_len=prefix_len)
            self.verify_hit_state(match_entries)

            # Send pkts matching the entries
            assert len(ip_dsts) == len(in_pkts)
            self.send_matching_pkts(match_entries, in_pkts[:len(ip_dsts)//2])

            #Add the rest of the entries
            for ip_dst in ip_dsts[len(ip_dsts)//2:]:
                self.add_match_entry(match_entries, ip_dst, match_len=prefix_len)
            self.verify_hit_state(match_entries)

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,1+((NUM_MAT_ENTRIES-1)//2))))

            self.verify_hit_state(match_entries)

            # Send pkts matching each entry
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

            # Delete some entries
            entries_to_delete = random.sample(ip_dsts, 1+ ((NUM_MAT_ENTRIES-1)//4))

            for ip_dst in entries_to_delete:
                self.del_match_entry(match_entries, ip_dst)

            for ip_dst in entries_to_delete:
                self.add_match_entry(match_entries, ip_dst, match_len=prefix_len)
            self.verify_hit_state(match_entries)
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

            self.verify_hit_state(match_entries)
            self.verify_hit_state(match_entries)
        finally:
            for ip_dst in match_entries:
                self.del_match_entry(match_entries, ip_dst, delete=False)
            self.disable_poll_mode()
            if self.scopes == 1:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                self.fns["set_property"](self.sess_hdl, self.dev_id, prop, prop_val, 0)
            print(('Seed used for the file %d' %(seed)))

@group("thorough")
class TestIdleTime(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        InstallAllDefaultEntries(self)
        sys.stdout.flush()
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_id = 0
        port = swports[2]
        ig_port = swports[1]
        scopes = 0

        try:
            fns = OrderedDict()
            fns["mat_ent_add"] = self.client.ipv4_alpm_idle_table_add_with_ipv4_lpm_hit
            fns["mat_ent_del"] = self.client.ipv4_alpm_idle_table_delete
            fns["idle_tmo_enable"] = self.client.ipv4_alpm_idle_idle_tmo_enable
            fns["idle_tmo_disable"] = self.client.ipv4_alpm_idle_idle_tmo_disable
            fns["match_spec_create"] = alpm_test_ipv4_alpm_idle_match_spec_t
            fns["complete_operations"] = self.conn_mgr.complete_operations
            fns["set_ttl"] = self.client.ipv4_alpm_idle_set_ttl
            fns["get_ttl"] = self.client.ipv4_alpm_idle_get_ttl
            fns["get_expired"] = self.client.ipv4_alpm_idle_idle_tmo_get_expired
            fns["update_hit_state"] = self.client.ipv4_alpm_idle_update_hit_state
            fns["get_hit_state"] = self.client.ipv4_alpm_idle_get_hit_state

            params = sess_hdl, dev_id, port, ig_port, scopes

            print('---------Poll Test START---------')
            poll_test_helper = IdlePollHelper(fns, params, self.dataplane, self)
            test_params = OrderedDict()
            poll_test_helper.test()
            print('---------Poll Test COMPLETED---------')
            print('---------Notify Test START---------')
            idle_params = (25000, 5000, 50000)
            tbl_params = (3, True, True)
            notify_test_helper = IdleNotifyHelper(fns, params, idle_params, self.dataplane, self, tbl_params)
            test_params = OrderedDict()
            notify_test_helper.test()
            print('---------Notify Test COMPLETED---------')

        finally:
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            RemoveAllDefaultEntries(self)
            print(('Seed used %d' % seed))
            sys.stdout.flush()
            print(('Seed used for the file %d' %(seed)))

class TestAlpmScopes(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        num_pipes = int(test_param_get('num_pipes'))
        if (num_pipes < 4):
            return

        if (len(swports_0) < 2) or (len(swports_1) < 2) or (len(swports_2) < 2) or (len(swports_3) < 2):
            print("Skipping test as there are less than 2 ports in some pipe")
            return

        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        # Reset the default entry
        dev_tgt_all = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.client.ipv4_alpm_table_reset_default_entry(sess_hdl, dev_tgt_all)

        dev_tgt_0 = DevTarget_t(dev_id, 0)
        dev_tgt_2 = DevTarget_t(dev_id, 2)

        #Set the scopes
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val = tbl_property_value_t.ENTRY_SCOPE_USER_DEFINED
        # pipes 0,1 in one scope, pipes 2,3 in other scope
        prop_args = 0x0c03
        self.client.ipv4_alpm_set_property(sess_hdl, dev_id, prop, prop_val, prop_args)

        print("Adding entry in first scope (pipes 0,1)")
        match_spec = alpm_test_ipv4_alpm_match_spec_t(
            0,
            ipv4Addr_to_i32("15.1.1.1"),
            16
        )
        port_0 = swports_0[1]
        dmac_0 = "00:01:02:03:04:88"
        action_spec = alpm_test_ipv4_lpm_hit_change_dmac_action_spec_t(port_0, macAddr_to_string(dmac_0))
        entry_hdl_0 = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit_change_dmac(
            sess_hdl,
            dev_tgt_0,
            match_spec,
            action_spec
        )
        print(("PIPE_MGR gave me that entry handle:", entry_hdl_0))

        print("Adding entry in first scope (pipes 2,3)")
        match_spec = alpm_test_ipv4_alpm_match_spec_t(
            0,
            ipv4Addr_to_i32("11.1.1.1"),
            16
        )
        port_2 = swports_2[1]
        dmac_2 = "00:01:02:03:04:99"
        action_spec = alpm_test_ipv4_lpm_hit_change_dmac_action_spec_t(port_2, macAddr_to_string(dmac_2))
        entry_hdl_2 = self.client.ipv4_alpm_table_add_with_ipv4_lpm_hit_change_dmac(
            sess_hdl,
            dev_tgt_2,
            match_spec,
            action_spec
        )
        print(("PIPE_MGR gave me that entry handle:", entry_hdl_2))
        self.conn_mgr.complete_operations(sess_hdl)


        try:
            self.conn_mgr.complete_operations(sess_hdl)
            print()
            print("Testing first scope ")
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='15.1.1.1',
                                    ip_id=101,
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst=dmac_0,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='15.1.1.1',
                                    ip_id=101,
                                    ip_ttl=63)
            print(("Sending packet %s -> %s " % (swports_0[0], port_0)))
            send_packet(self, tofLocalPortToOfPort(swports_0[0], dev_id), pkt)
            verify_packets(self, exp_pkt, [tofLocalPortToOfPort(port_0, dev_id)])
            print(("Sending packet %s -> %s " % (swports_1[0], port_0)))
            send_packet(self, tofLocalPortToOfPort(swports_1[0], dev_id), pkt)
            verify_packets(self, exp_pkt, [tofLocalPortToOfPort(port_0, dev_id)])
            print(("Sending packet %s -> %s (negative test)" % (swports_2[0], port_0)))
            send_packet(self, tofLocalPortToOfPort(swports_2[0], dev_id), pkt)
            verify_no_other_packets(self)


            print("Testing second scope ")
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='11.1.1.1',
                                    ip_id=101,
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst=dmac_2,
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='11.1.1.1',
                                    ip_id=101,
                                    ip_ttl=63)
            print(("Sending packet %s -> %s " % (swports_2[0], port_2)))
            send_packet(self, tofLocalPortToOfPort(swports_2[0], dev_id), pkt)
            verify_packets(self, exp_pkt, [tofLocalPortToOfPort(port_2, dev_id)])
            print(("Sending packet %s ->  %s " % (swports_3[0], port_2)))
            send_packet(self, tofLocalPortToOfPort(swports_3[0], dev_id), pkt)
            verify_packets(self, exp_pkt, [tofLocalPortToOfPort(port_2, dev_id)])
            print(("Sending packet %s -> %s (negative test)" % (swports_0[0], port_2)))
            send_packet(self, tofLocalPortToOfPort(swports_0[0], dev_id), pkt)
            verify_no_other_packets(self)


        finally:
            print("deleting entry")
            status = self.client.ipv4_alpm_table_delete( sess_hdl,
                dev_id,
                entry_hdl_0
            )
            status = self.client.ipv4_alpm_table_delete( sess_hdl,
                dev_id,
                entry_hdl_2
            )
            self.conn_mgr.complete_operations(sess_hdl)
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
            self.client.ipv4_alpm_set_property(sess_hdl, dev_id, prop, prop_val, 0)

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used for the file %d' %(seed)))



@group("thorough")
class TestIdleScopes(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        num_pipes = int(test_param_get('num_pipes'))
        if (num_pipes < 4):
            return

        if (len(swports_0) < 2) or (len(swports_1) < 2) or (len(swports_2) < 2) or (len(swports_3) < 2):
            print("Skipping test as there are less than 2 ports in some pipe")
            return
        sys.stdout.flush()
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_id = 0
        port = swports_0[2]
        ig_port = swports_0[1]
        scopes = 1

        try:
            fns = OrderedDict()
            fns["mat_ent_add"] = self.client.ipv4_alpm_idle_table_add_with_ipv4_lpm_hit
            fns["mat_ent_del"] = self.client.ipv4_alpm_idle_table_delete
            fns["idle_tmo_enable"] = self.client.ipv4_alpm_idle_idle_tmo_enable
            fns["idle_tmo_disable"] = self.client.ipv4_alpm_idle_idle_tmo_disable
            fns["match_spec_create"] = alpm_test_ipv4_alpm_idle_match_spec_t
            fns["complete_operations"] = self.conn_mgr.complete_operations
            fns["set_ttl"] = self.client.ipv4_alpm_idle_set_ttl
            fns["get_ttl"] = self.client.ipv4_alpm_idle_get_ttl
            fns["get_expired"] = self.client.ipv4_alpm_idle_idle_tmo_get_expired
            fns["update_hit_state"] = self.client.ipv4_alpm_idle_update_hit_state
            fns["get_hit_state"] = self.client.ipv4_alpm_idle_get_hit_state
            fns["set_property"] = self.client.ipv4_alpm_idle_set_property
            fns["reset_default"] = self.client.ipv4_alpm_idle_table_reset_default_entry

            params = sess_hdl, dev_id, port, ig_port, scopes

            print('---------Poll Test START---------')
            poll_test_helper = IdlePollHelper(fns, params, self.dataplane, self)
            test_params = OrderedDict()
            poll_test_helper.test()
            print('---------Poll Test COMPLETED---------')
            print('---------Notify Test START---------')
            idle_params = (25000, 5000, 50000)
            tbl_params = (3, True, True)
            notify_test_helper = IdleNotifyHelper(fns, params, idle_params, self.dataplane, self, tbl_params)
            test_params = OrderedDict()
            notify_test_helper.test()
            print('---------Notify Test COMPLETED---------')

        finally:
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used %d' % seed))
            sys.stdout.flush()
            print(('Seed used for the file %d' %(seed)))

class TestSnapshot(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        InstallAllDefaultEntries(self)
        print(('Seed used %d' % seed))
        sys.stdout.flush()
        random.seed(seed)
        sess_hdl = self.conn_mgr.client_init()

        dev_id = 0
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        entry_hdl = None
        snap_hdl = 0

        ig_port = swports[0]
        eg_port = swports[-1]
        ig_pipe = ig_port >> 7
        vlan = 0
        ip_dst = "1.2.3.4"
        match_len = 32
        priority = 0
        try:

            match_spec = alpm_test_ipv4_alpm_large_match_spec_t(
                    vlan,
                    ipv4Addr_to_i32(ip_dst),
                    match_len
                    )
            action_spec = alpm_test_ipv4_lpm_hit_action_spec_t (
                    eg_port
                    )

            entry_hdl = self.client.ipv4_alpm_large_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                    )

            snap_hdl = self.client.snapshot_create(dev_tgt, 0, 11, 0)
            print(("SnapHdl %d" % (snap_hdl)))
            sys.stdout.flush()
            self.assertNotEqual(snap_hdl, 0)
            # Set it to only capture for this packet's IPs
            trig_spec1 = alpm_test_snapshot_trig_spec_t("ipv4_dstAddr",
                            ipv4Addr_to_i32(ip_dst), ipv4Addr_to_i32('255.255.255.255'))
            trig_spec2 = alpm_test_snapshot_trig_spec_t("tcp_valid", 1, 1)
            self.client.snapshot_capture_trigger_set(snap_hdl, trig_spec1,
                                                     trig_spec2)
            # Enable the snapshot
            self.client.snapshot_state_set(snap_hdl, 1, 0)
            snap_state = self.client.snapshot_state_get(snap_hdl, ig_pipe)
            self.assertEqual(snap_state, 1)
            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst=ip_dst,
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, ig_port, pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            verify_packets(self, pkt, [eg_port])

            # Verify the snapshot was taken
            snap_state = self.client.snapshot_state_get(snap_hdl, ig_pipe)
            self.assertEqual(snap_state, 0)
            # Verify the entry handle was matched
            t = self.client.snapshot_capture_tbl_data_get(snap_hdl, ig_pipe, "ipv4_alpm_large")
            self.assertTrue(t.hit)
            self.assertTrue(t.executed)
            self.assertFalse(t.inhibited)
            self.assertEqual(t.hit_entry_handle, entry_hdl)


        finally:
            # Clean up the snapshot
            if snap_hdl != 0:
                self.client.snapshot_delete(snap_hdl)

            if entry_hdl:
                self.client.ipv4_alpm_large_table_delete(sess_hdl, dev_id, entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used %d' % seed))
            sys.stdout.flush()
            RemoveAllDefaultEntries(self)
            print(('Seed used for the file %d' %(seed)))

class TestDefaultEntry(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    """ Basic test """
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        def_entry_hdl = self.client.ipv4_alpm_set_default_action_nop(sess_hdl, dev_tgt)
        try:
            self.conn_mgr.complete_operations(sess_hdl)
            self.assertTrue(def_entry_hdl == self.client.ipv4_alpm_table_get_default_entry_handle(
                            sess_hdl, dev_tgt))

        finally:
            print("reset entry")
            self.client.ipv4_alpm_table_reset_default_entry(sess_hdl, dev_tgt)
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used for the file %d' %(seed)))

class TestExcluded(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["alpm_test"])

    def send_and_verify(self, test_matrix, send_indices, delete_indices, use_lpm=False):
        for idx in send_indices:
            (ip_dst, match_len, port) = test_matrix[idx]
            pkt = simple_tcp_packet(ip_dst=ip_dst,
                                    ip_ttl=64)
            print(('Sending pkt with ip-dst %s' % ip_dst))
            send_packet(self, tofLocalPortToOfPort(swports[0], dev_id), pkt)
            pkt["IP"].ttl = pkt["IP"].ttl - 1
            if use_lpm:
                port = lpm(test_matrix, delete_indices, ip_dst)
            if port == -1:
                print(('Expected on default port %d' % self.default_port))
                verify_packet(self, pkt, tofLocalPortToOfPort(self.default_port, dev_id))
            else:
                print(('Expected on port %d' % port))
                verify_packet(self, pkt, tofLocalPortToOfPort(port, dev_id))

    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        print(("PIPE_MGR gave me that session handle:", sess_hdl))

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.default_port = swports[-1]
        action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(self.default_port)
        self.client.ipv4_alpm_excluded_set_default_action_ipv4_lpm_hit(sess_hdl, dev_tgt, action_spec)

        total_entries = 60000
        test_matrix = readData(total_entries)

        # Include a few excluded (/4) entries
        excluded_entries = set([])
        excluded_match_len = 4
        for (ip_dst, _, _) in test_matrix:
            addr = ipv4Addr_to_i32(ip_dst) & buildMask(excluded_match_len)
            if addr in excluded_entries:
                continue
            excluded_entries.add(addr)
            test_matrix.append((i32_to_ipv4Addr(addr), excluded_match_len, random.choice(swports[:-1])))
            if len(excluded_entries) == 10:
                break

        entry_hdls = []
        delete_hdls = []
        print(("Adding %d entries and %d excluded entries" % (total_entries, len(excluded_entries))))
        try:
            for (ip_dst, match_len, port) in test_matrix:
                match_spec = alpm_test_ipv4_alpm_excluded_match_spec_t(
                    6,
                    ipv4Addr_to_i32(ip_dst),
                    match_len
                )
                action_spec = alpm_test_ipv4_lpm_hit_action_spec_t(port)
                entry_hdls.append(self.client.ipv4_alpm_excluded_table_add_with_ipv4_lpm_hit(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    action_spec
                ))
            self.conn_mgr.complete_operations(sess_hdl)

            print("Checking normal entries")
            # Check a subset of our normal entries
            send_indices = random.sample(list(range(total_entries)), 100)
            self.send_and_verify(test_matrix, send_indices, [])

            print("Checking exclude entries")
            # Check excluded entries
            send_indices = list(range(total_entries, len(test_matrix)))
            self.send_and_verify(test_matrix, send_indices, [], True)

            print("Deleting half of our entries")
            delete_indices = random.sample(list(range(total_entries)), total_entries // 2)
            delete_indices += random.sample(list(range(total_entries, len(test_matrix))), len(excluded_entries) // 2)
            for idx in delete_indices:
                self.client.ipv4_alpm_excluded_table_delete(sess_hdl, dev_id, entry_hdls[idx])
                delete_hdls.append(entry_hdls[idx])
            self.conn_mgr.complete_operations(sess_hdl)

            print("Checking normal entries after delete")
            # Check a subset of our normal entries
            send_indices = random.sample(list(range(total_entries)), 20)
            self.send_and_verify(test_matrix, send_indices, delete_indices, True)

            print("Checking exclude entries after delete")
            # Check excluded entries
            send_indices = list(range(total_entries, len(test_matrix)))
            self.send_and_verify(test_matrix, send_indices, delete_indices, True)

        finally:
            print(("Deleting %d entries" % (len(entry_hdls) - len(delete_hdls))))
            for entry_hdl in entry_hdls:
                if entry_hdl not in delete_hdls:
                    status = self.client.ipv4_alpm_excluded_table_delete(
                        sess_hdl,
                        dev_id,
                        entry_hdl
                    )
            self.client.ipv4_alpm_excluded_table_reset_default_entry(sess_hdl, dev_tgt)
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print(('Seed used for the file %d' %(seed)))

        return
