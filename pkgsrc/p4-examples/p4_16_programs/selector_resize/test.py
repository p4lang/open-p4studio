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

import logging
import pdb
import random
import time

from ptf import config

from ptf.testutils import *
from ptf_port import *
from p4testutils.misc_utils import *

from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as gc
import google.rpc.code_pb2 as code_pb2
import copy
logger = get_logger()
swports = get_sw_ports()

g_is_model = test_param_get('target') != "hw"

tern = 0
exm = 1

EPSILON = 1e-6

if test_param_get("arch") is None:
    raise "Missing test parameter 'arch'"

def setup_ports(test):
    test.ports = []
    for data, _ in test.dev_cfg_table.default_entry_get(test.target):
        data_dict = data.to_dict()
        test.num_pipes = data_dict['num_pipes']
    for device, port, ifname in config["interfaces"]:
        pipe = port >> 7
        if pipe in range(test.num_pipes):
            test.ports.append(port)
    test.ports.sort()

def cleanup(test):
    # In case of asymetric tests tableClear will fail for table that was not set
    # to assymetric
    need_one_more = False;
    for tbl in test.ecmp:
        try:
            tbl.default_entry_reset(test.target)
            tbl.entry_del(test.target, [])
        except:
            need_one_more = True;
    for tbl in test.selector:
        try:
            tbl.entry_del(test.target, [])
        except:
            need_one_more = True;
    for tbl in test.act_profile:
        try:
            tbl.entry_del(test.target, [])
        except:
            need_one_more = True;

    test.target = gc.Target(device_id=0, pipe_id=0xffff)
    mode = bfruntime_pb2.Mode.ALL
    for tbl in test.ecmp:
        tbl.attribute_entry_scope_set(test.target, predefined_pipe_scope=True,
                predefined_pipe_scope_val=mode)

    if need_one_more:
        cleanup(test)

def mod_selector_entry(test, sel_tbl, grp_id, size, ap_entries, en_arr = None, txn=False):
    logger.info("Changing group size to %d", size)
    key = sel_tbl.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', grp_id)])
    if en_arr == None:
        en_arr = [True] * len(ap_entries)
    data = sel_tbl.make_data(
        [
            gc.DataTuple('$MAX_GROUP_SIZE', size),
            gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=ap_entries),
            gc.DataTuple('$ACTION_MEMBER_STATUS', bool_arr_val=en_arr)
        ])
    if txn:
        sel_tbl.entry_mod(test.target, [key], [data],{"reset_ttl":True}, bfruntime_pb2.WriteRequest.ROLLBACK_ON_ERROR)
    else:
        sel_tbl.entry_mod(test.target, [key], [data])
def get_selector_entry(test, sel_tbl, grp_id):
    key = sel_tbl.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', grp_id)])

    return  sel_tbl.entry_get(test.target, [key])

def check_traffic(test, ports, dmacs, ttl=None):
    port = test.ports[0]
    for dmac in dmacs:
        pkt = simple_tcp_packet(eth_dst=gc.bytes_to_mac(gc.to_bytes(dmac, 6)))
        if ttl != None:
            pkt['IP'].ttl = ttl
        if (len(dmacs) < 10):
            logger.info("Sending packet on port %d 0x%x", port, dmac)
        send_packet(test, port, pkt)
        if (len(dmacs) < 10):
            logger.info("Expecting packet on any port {}".format(ports))
        verify_packets_any(test, pkt, ports, n_timeout=EPSILON)

def add_mat_entry(test, tbl, tbl_type, dmac, sel_grp_id,txn=False):
    mask = 0xffffffffffff if tbl_type == tern else None
    key = tbl.make_key(
                [gc.KeyTuple('hdr.ethernet.dst_addr', dmac, mask)])
    data = tbl.make_data(
                [gc.DataTuple('$SELECTOR_GROUP_ID', sel_grp_id)])
    if txn:
        tbl.entry_add(test.target, [key], [data], bfruntime_pb2.WriteRequest.ROLLBACK_ON_ERROR)
    else:
        tbl.entry_add(test.target, [key], [data])
def setup_tbls(test):
    p4_name = "selector_resize"
    gc_id = 0
    BfRuntimeTest.setUp(test, gc_id, p4_name)
    test.bfrt_info = test.interface.bfrt_info_get(p4_name)
    test.dev_cfg_table = test.bfrt_info.table_get("device_configuration")
    test.selector = [None] * 2
    test.ecmp = [None] * 2
    test.act_profile = [None] * 2
    for tbl_type in [tern, exm]:
        ctrl = "nexthop_t" if tbl_type == tern else "nexthop_e"
        test.selector[tbl_type] = test.bfrt_info.table_get(ctrl+".ecmp_selector")
        test.ecmp[tbl_type] = test.bfrt_info.table_get(ctrl+".ecmp")
        test.act_profile[tbl_type] = test.bfrt_info.table_get(ctrl+".act_profile")
    test.target = gc.Target(device_id=0, pipe_id=0xffff)

@group("thorough")
class Resize(BfRuntimeTest):
    """ @brief This test populates action profile table and creates selector table.
        Test is divided in 3 cases:
        1. Resizing without members
        2. Resizing with the same members
        3. Resizing with new random members
    """
    def __init__(self):
        BfRuntimeTest.__init__(self)

    def setUp(self):
        setup_tbls(self)
        setup_ports(self)
        setup_random()

    def tearDown(self):
        # Requires self.target to be properly set.
        cleanup(self)
        self.dev_cfg_table.default_entry_reset(self.target)
        BfRuntimeTest.tearDown(self)

    def runBaseTest(self):
        act_profile = self.act_profile[tern]
        selector = self.selector[tern]
        ecmp = self.ecmp[tern]

        max_grp_size = selector.info.size_get()
        num_ap_entries = act_profile.info.size_get()
        num_test_iter = 10
        num_ports = 4

        for symmetric in [True, False, False]:
            if symmetric == True:
                pipe = 0xffff
                mode = bfruntime_pb2.Mode.ALL
            else:
                pipe = random.randint(0, self.num_pipes - 1)
                mode = bfruntime_pb2.Mode.SINGLE

            t = gc.Target(device_id=0, pipe_id=0xffff)
            ecmp.attribute_entry_scope_set(t, predefined_pipe_scope=True, predefined_pipe_scope_val=mode)
            self.target = gc.Target(device_id=0, pipe_id=pipe)
            logger.info("Testing pipe {}...".format(pipe))
            logger.info("Populating action profile table with {} entries..."
                    .format(num_ap_entries))
            for x in range(num_ap_entries):
                key = act_profile.make_key(
                        [gc.KeyTuple('$ACTION_MEMBER_ID', x)])
                data = act_profile.make_data(
                        [gc.DataTuple('dst_port', self.ports[x % num_ports])],
                        'SwitchIngress.nexthop_t.set_eg_port')
                act_profile.entry_add(self.target, [key], [data])

            sel_grp_id = 0
            key = selector.make_key(
                    [gc.KeyTuple('$SELECTOR_GROUP_ID', sel_grp_id)])
            data = selector.make_data(
                    [gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size)])

            logger.info("Adding selector entry...")
            selector.entry_add(self.target, [key], [data])

            # Some edge cases + random values
            max_grp_sizes = [1, max_grp_size, 1, max_grp_size // 2, 1]
            for x in range(num_test_iter):
                max_grp_sizes.append(random.randint(1, max_grp_size))

            ap_entries = []
            logger.info("Modifying selector entry {} times - no members..."
                    .format(len(max_grp_sizes)))
            for size in max_grp_sizes:
                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries)
                for data, _ in get_selector_entry(self, selector, sel_grp_id):
                    dd = data.to_dict()
                    assert dd['$MAX_GROUP_SIZE'] == size

            mod_selector_entry(self, selector, sel_grp_id, 1, [0])
            # Mat entry requires selector to have at least 1 entry in AP.
            dmac = 0x001234987654
            logger.info("Adding ecmp entry dmac 0x{:X}...".format(dmac))
            add_mat_entry(self, ecmp, tern, dmac, sel_grp_id)
            # Will be hashed to port [x]
            if symmetric:
                check_traffic(self, self.ports[:num_ports], [dmac])

            # Random values, next test expect no changes in action profile
            min_grp_size = 512
            max_grp_sizes = [max_grp_size - 1, min_grp_size]
            for x in range(num_test_iter):
                max_grp_sizes.append(random.randint(min_grp_size, max_grp_size))

            logger.info("Modifying selector entry {} times - random same {} members..."
                    .format(len(max_grp_sizes), min_grp_size))
            # Generate data
            ap_entries = []
            for x in range(min_grp_size):
                ap_id = random.randint(0, num_ap_entries - 1)
                while ap_id in ap_entries:
                    ap_id = random.randint(0, num_ap_entries - 1)
                ap_entries.append(ap_id)
            ap_entries.sort()

            for size in max_grp_sizes:
                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries)
                # Arrays need to be sorted for comparison
                for data, _ in get_selector_entry(self, selector, sel_grp_id):
                    dd = data.to_dict()
                    dd['$ACTION_MEMBER_ID'].sort()
                    assert dd['$MAX_GROUP_SIZE'] == size
                    assert dd['$ACTION_MEMBER_ID'] == ap_entries
                    assert dd['$ACTION_MEMBER_STATUS'] == [True] * (min_grp_size)
                if symmetric:
                    check_traffic(self, self.ports[:num_ports], [dmac])

            logger.info("Modifying selector entry - remove all members...")
            ecmp.entry_del(self.target, [])
            ap_entries = []
            mod_selector_entry(self, selector, sel_grp_id, max_grp_size, ap_entries)

            # Some edge cases + random values
            max_grp_sizes = [max_grp_size - 1, max_grp_size // 2]
            for x in range(num_test_iter):
                max_grp_sizes.append(random.randint(2, max_grp_size))

            logger.info("Modifying selector entry {} times - random new members..."
                    .format(len(max_grp_sizes)))

            mod_selector_entry(self, selector, sel_grp_id, 1, [0])
            # Mat entry requires selector to have at least 1 entry in AP.
            logger.info("Adding ecmp entry dmac 0x{:X}...".format(dmac))
            add_mat_entry(self, ecmp, tern, dmac, sel_grp_id)
            if symmetric:
                check_traffic(self, self.ports[:num_ports], [dmac])

            # Same test with action profile entries added to selector
            for size in max_grp_sizes:
                # Generate data
                ap_entries = []
                for x in range(size):
                    ap_id = random.randint(0, num_ap_entries - 1)
                    while ap_id in ap_entries:
                        ap_id = random.randint(0, num_ap_entries - 1)
                    ap_entries.append(ap_id)
                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries)
                # Arrays need to be sorted for comparison
                ap_entries.sort()
                for data, _ in get_selector_entry(self, selector, sel_grp_id):
                    dd = data.to_dict()
                    dd['$ACTION_MEMBER_ID'].sort()
                    assert dd['$MAX_GROUP_SIZE'] == size
                    assert dd['$ACTION_MEMBER_ID'] == ap_entries
                    assert dd['$ACTION_MEMBER_STATUS'] == [True] * (size)
                if symmetric:
                    check_traffic(self, self.ports[:num_ports], [dmac])

            cleanup(self)

    def runTest(self):
        self.runBaseTest()
        logger.info("Rerun enabling sequence order")
        selector_member_order = True
        dev_data = self.dev_cfg_table.make_data([
                      gc.DataTuple('selector_member_order',
                      bool_val=selector_member_order)])
        self.dev_cfg_table.default_entry_set(self.target, dev_data)
        self.runBaseTest()


class Negative(BfRuntimeTest):
    """ @brief Negative test cases for selector resizing scenario.
    """
    def __init__(self):
        BfRuntimeTest.__init__(self)

    def setUp(self):
        setup_tbls(self)
        setup_ports(self)
        setup_random()

    def tearDown(self):
        cleanup(self)
        self.dev_cfg_table.default_entry_reset(self.target)
        BfRuntimeTest.tearDown(self)

    def runBaseTest(self):
        act_profile = self.act_profile[exm]
        selector = self.selector[exm]
        ecmp = self.ecmp[exm]

        self.target = gc.Target(device_id=0, pipe_id=0xffff)

        max_grp_size = selector.info.size_get();
        num_ap_entries = 10
        # 10 entries should be enough to cover corner cases
        logger.info("Populating action profile table with {} entries...".format(num_ap_entries))
        for x in range(num_ap_entries):
            key = act_profile.make_key(
                    [gc.KeyTuple('$ACTION_MEMBER_ID', x)])
            data = act_profile.make_data(
                    [gc.DataTuple('dst_port', self.ports[x % len(self.ports)])],
                   'SwitchIngress.nexthop_e.set_eg_port')
            act_profile.entry_add(self.target, [key], [data])

        sel_grp_id = 0
        key = selector.make_key(
                [gc.KeyTuple('$SELECTOR_GROUP_ID', sel_grp_id)])
        data = selector.make_data(
                [gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size)])

        logger.info("Adding selector entry...")
        selector.entry_add(self.target, [key], [data])

        # Generate data
        logger.info("Adding selector entry members...")
        ap_entries = []
        for x in range(num_ap_entries):
            ap_id = random.randint(0, num_ap_entries - 1)
            while ap_id in ap_entries:
                ap_id = random.randint(0, num_ap_entries - 1)
            ap_entries.append(ap_id)
        mod_selector_entry(self, selector, sel_grp_id, max_grp_size, ap_entries)

        logger.info("Modify selector size above max grp size")
        got_exception = False
        try:
            mod_selector_entry(self, selector, sel_grp_id, max_grp_size + 1, [])
        except:
            got_exception = True

        assert got_exception == True, "Modify did not fail as expected"

        logger.info("Modify selector size for single pipe")
        got_exception = False
        tgt = self.target
        try:
            self.target = gc.Target(device_id=0, pipe_id=0)
            mod_selector_entry(self, selector, sel_grp_id, max_grp_size // 2, [])
        except:
            got_exception = True
        finally:
            self.target = tgt

        assert got_exception == True, "Modify did not fail as expected"
        cleanup(self)

    def runTest(self):
        self.runBaseTest()
        logger.info("Rerun enabling sequence order")
        selector_member_order = True
        dev_data = self.dev_cfg_table.make_data([
                      gc.DataTuple('selector_member_order',
                      bool_val=selector_member_order)])
        self.dev_cfg_table.default_entry_set(self.target, dev_data)
        self.runBaseTest()


@group("thorough")
class Traffic(BfRuntimeTest):
    """ @brief This test populates action profile table and creates selector
        table. Idea is to check if MAT entries are properly refreshed after
        resizing the selector table entry. Should be checked for both ternary
        and exact match table types. Default entry with selector action is being
        tested along rest of MAT entries.
    """
    def __init__(self):
        BfRuntimeTest.__init__(self)

    def setUp(self):
        setup_tbls(self)
        setup_ports(self)
        setup_random()

    def tearDown(self):
        # Requires self.target to be properly set.
        cleanup(self)
        self.dev_cfg_table.default_entry_reset(self.target)
        BfRuntimeTest.tearDown(self)

    def runBaseTest(self):
        for tbl_type in [tern, exm]:
            logger.info("\nTesting {} table".format(
                "TERNARY" if tbl_type == tern else "EXM"))
            selector = self.selector[tbl_type]
            ecmp = self.ecmp[tbl_type]
            act_profile = self.act_profile[tbl_type]

            max_grp_size = selector.info.size_get()
            num_ap_entries = max_grp_size #act_profile.info.size_get()
            p_idx_base = tbl_type
            self.target = gc.Target(device_id=0, pipe_id=0xffff)

            logger.info("Populating action table...")
            for x in range(0, 5):
                key = act_profile.make_key(
                        [gc.KeyTuple('$ACTION_MEMBER_ID', x)])
                data = act_profile.make_data(
                        [gc.DataTuple('dst_port', self.ports[x%5])],
                        'SwitchIngress.nexthop_{}.set_eg_port'.format(
                            't' if tbl_type == tern else 'e'))
                act_profile.entry_add(self.target, [key], [data])
            for x in range(5, num_ap_entries):
                key = act_profile.make_key(
                        [gc.KeyTuple('$ACTION_MEMBER_ID', x)])
                data = act_profile.make_data(
                        [gc.DataTuple('dst_port', self.ports[4])],
                        'SwitchIngress.nexthop_{}.set_eg_port'.format(
                            't' if tbl_type == tern else 'e'))
                act_profile.entry_add(self.target, [key], [data])

            sel_grp_id = 0
            key = selector.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', sel_grp_id)])
            data = selector.make_data([
                gc.DataTuple('$MAX_GROUP_SIZE', 1),
                gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=[p_idx_base]),
                gc.DataTuple('$ACTION_MEMBER_STATUS', bool_arr_val=[True])
                ])

            logger.info("Adding selector entry...")
            selector.entry_add(self.target, [key], [data])

            eg_port = self.ports[p_idx_base]

            logger.info("Setting default entry...")
            miss_dmac = 0xffffffffffff
            data = ecmp.make_data([gc.DataTuple("$SELECTOR_GROUP_ID", sel_grp_id)])
            ecmp.default_entry_set(self.target, data)
            check_traffic(self, [eg_port], [miss_dmac])

            dmac = 0x1122334455
            dmacs = [dmac, miss_dmac]
            logger.info("Adding ecmp entry dmac 0x{:X}...".format(dmac))
            add_mat_entry(self, ecmp, tbl_type, dmac, sel_grp_id)
            check_traffic(self, [eg_port], dmacs)

            logger.info("Modifying selector entry - Change size only "
                "and verify trafic still flows...")
            mod_selector_entry(self, selector, sel_grp_id, 2,
                    [p_idx_base])
            check_traffic(self, [eg_port], dmacs)

            num_mat_entries = 1022
            logger.info("Adding ecmp {} entries...".format(num_mat_entries))
            for dmac in range(num_mat_entries):
                add_mat_entry(self, ecmp, tbl_type, dmac, sel_grp_id)
                dmacs.append(dmac)
            # All trafic will hash to port [p_idx_base]
            check_traffic(self, [eg_port], dmacs)

            logger.info("Modifying selector entry - Change size only "
                "and verify trafic still flows...")
            mod_selector_entry(self, selector, sel_grp_id, 3,
                    [p_idx_base])
            check_traffic(self, [eg_port], dmacs)

            # Default entry supports only single selector word, hence selector
            # size of max 120, anything above will throw an error.
            max_size_for_default_ent = 120
            sizes = [max_size_for_default_ent, max_grp_size // 2]
            for size in sizes:
                logger.info("Modifying selector entry - Change size %d and ports",
                        size)
                if size > max_size_for_default_ent and miss_dmac in dmacs:
                    ecmp.default_entry_reset(self.target)
                    dmacs.remove(miss_dmac)
                ap_entries = []
                for x in range(size):
                    ap_id = random.randint(5, num_ap_entries - 1)
                    while ap_id in ap_entries:
                        ap_id = random.randint(5, num_ap_entries - 1)
                    ap_entries.append(ap_id)

                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries)

                # ALL traffic will hash the same to port [4]
                check_traffic(self, [self.ports[4]], dmacs)

            cleanup(self)

    def runTest(self):
        self.runBaseTest()
        logger.info("Rerun enabling sequence order")
        selector_member_order = True
        dev_data = self.dev_cfg_table.make_data([
                      gc.DataTuple('selector_member_order',
                      bool_val=selector_member_order)])
        self.dev_cfg_table.default_entry_set(self.target, dev_data)
        self.runBaseTest()


@group("thorough")
class Members(BfRuntimeTest):
    """ @brief Resize a group with disabled members
        Verify the members are still disabled after the resize.
        Test creates a group all members disabled except one. Disabled members
        use different port X than enabled ones Y.
        After resize random packets should only use port X.
        Re-enable members and some packets should use port Y.
    """
    def __init__(self):
        BfRuntimeTest.__init__(self)

    def setUp(self):
        setup_tbls(self)
        setup_ports(self)
        setup_random()

    def tearDown(self):
        # Requires self.target to be properly set.
        cleanup(self)
        self.dev_cfg_table.default_entry_reset(self.target)
        BfRuntimeTest.tearDown(self)

    def verify_packet(self, dmac, num_pkts, allow_dis=False):
        dis_port_pkts = 0
        for _ in range(num_pkts):
            pkt = simple_tcp_packet(
                    eth_dst=gc.bytes_to_mac(gc.to_bytes(dmac, 6)))
            pkt["IP"].src = gc.bytes_to_ipv4(
                    gc.to_bytes(random.randint(0, 2**32 -1), 4))
            pkt["IP"].dst = gc.bytes_to_ipv4(
                        gc.to_bytes(random.randint(0, 2**32 -1), 4))
            logger.info("Sending packet on port %d 0x%x", self.ig_port, dmac)
            send_packet(self, self.ig_port, pkt)
            logger.info("Expecting packet on port %d", self.en_port)
            try:
                verify_packet(self, pkt, self.en_port)
            except:
                assert allow_dis == True, "Packet not received on expected port"
                logger.info("No pkt, expecting packet on port %d", self.dis_port)
                verify_packet(self, pkt, self.dis_port)
                dis_port_pkts += 1

        logger.info("Received packets %d %d", num_pkts - dis_port_pkts,
                dis_port_pkts)

        if allow_dis == True:
            assert dis_port_pkts != 0, "Packets were not received on 2nd port"

    def runBaseTest(self):
        for tbl_type in [tern, exm]:
            logger.info("\nTesting {} table".format(
                "TERNARY" if tbl_type == tern else "EXM"))
            selector = self.selector[tbl_type]
            ecmp = self.ecmp[tbl_type]
            act_profile = self.act_profile[tbl_type]

            max_grp_size = selector.info.size_get()
            num_ap_entries = 20
            num_pkts = 10
            self.target = gc.Target(device_id=0, pipe_id=0xffff)
            self.ig_port = self.ports[0]
            self.en_port = self.ports[1]
            self.dis_port = self.ports[2]
            logger.info("Enabled port %d, disabled %d", self.en_port,
                    self.dis_port)

            logger.info("Populating action table...")
            key = act_profile.make_key(
                    [gc.KeyTuple('$ACTION_MEMBER_ID', 0)])
            data = act_profile.make_data(
                    [gc.DataTuple('dst_port', self.en_port)],
                    'SwitchIngress.nexthop_{}.set_eg_port'.format(
                        't' if tbl_type == tern else 'e'))
            ap_entries = [0]
            act_profile.entry_add(self.target, [key], [data])
            for x in range(1, num_ap_entries):
                key = act_profile.make_key(
                        [gc.KeyTuple('$ACTION_MEMBER_ID', x)])
                data = act_profile.make_data(
                        [gc.DataTuple('dst_port', self.dis_port)],
                        'SwitchIngress.nexthop_{}.set_eg_port'.format(
                            't' if tbl_type == tern else 'e'))
                act_profile.entry_add(self.target, [key], [data])
                ap_entries.append(x)

            logger.info("Adding selector entry...")
            sel_grp_id = 0
            key = selector.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', sel_grp_id)])
            en_arr = [False] * len(ap_entries)
            en_arr[0] = True
            data = selector.make_data([
                gc.DataTuple('$MAX_GROUP_SIZE', num_ap_entries),
                gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=ap_entries),
                gc.DataTuple('$ACTION_MEMBER_STATUS', bool_arr_val=en_arr)
                ])
            selector.entry_add(self.target, [key], [data])

            dmac = 0x1122334455
            logger.info("Adding ecmp entry dmac 0x{:X}...".format(dmac))
            add_mat_entry(self, ecmp, tbl_type, dmac, sel_grp_id)
            self.verify_packet(dmac, num_pkts)

            # Tested sizes, last one same as original size.
            sizes = [num_ap_entries + 2, num_ap_entries + 3, 2 * num_ap_entries,
                     5 * num_ap_entries, max_grp_size // 2, max_grp_size,
                     num_ap_entries]
            for size in sizes:
                logger.info("Modifying selector entry - Change size (%d) "
                    "and verify trafic still flows...", size)
                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries,
                        en_arr)
                self.verify_packet(dmac, num_pkts)

            # Enable all members and check traffic
            for size in sizes:
                logger.info("Modifying selector entry - Change size (%d), enable "
                    " and verify trafic still flows...", size)
                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries)
                # Allow both ports
                self.verify_packet(dmac, num_pkts, True)

            cleanup(self)

    def runTest(self):
        self.runBaseTest()
        logger.info("Rerun enabling sequence order")
        selector_member_order = True
        dev_data = self.dev_cfg_table.make_data([
                      gc.DataTuple('selector_member_order',
                      bool_val=selector_member_order)])
        self.dev_cfg_table.default_entry_set(self.target, dev_data)
        self.runBaseTest()

@group("thorough")
class Shared(BfRuntimeTest):
    """ @brief Resize a group with disabled members
        Verify the members are still disabled after the resize.
        Test creates a group all members disabled except one. Disabled members
        use different port X than enabled ones Y.
        After resize random packets should only use port X.
        Re-enable members and some packets should use port Y.
    """
    def __init__(self):
        BfRuntimeTest.__init__(self)

    def setUp(self):
        p4_name = "selector_resize"
        gc_id = 0
        BfRuntimeTest.setUp(self, gc_id, p4_name)
        self.bfrt_info = self.interface.bfrt_info_get(p4_name)
        self.dev_cfg_table = self.bfrt_info.table_get("device_configuration")
        self.ecmp = [None] * 2
        self.selector = [self.bfrt_info.table_get("nexthop_shared.ecmp_selector")]
        self.act_profile = [self.bfrt_info.table_get("nexthop_shared.act_profile")]
        for tbl_type in [tern, exm]:
            self.ecmp[tbl_type] = self.bfrt_info.table_get(
                    "nexthop_shared.ecmp{}".format(
                        "_t" if tbl_type == tern else "_e"))
        self.target = gc.Target(device_id=0, pipe_id=0xffff)

        setup_ports(self)
        setup_random()

    def tearDown(self):
        # Requires self.target to be properly set.
        cleanup(self)
        self.dev_cfg_table.default_entry_reset(self.target)
        BfRuntimeTest.tearDown(self)

    def runBaseTest(self):
        selector = self.selector[0]
        act_profile = self.act_profile[0]

        max_grp_size = 120
        num_ap_entries = 24
        num_mat_entries = max_grp_size + 1
        self.ig_port = self.ports[0]
        self.eg_port = self.ports[1]

        logger.info("Populating action table...")
        ap_entries = []
        for x in range(0, num_ap_entries):
            key = act_profile.make_key(
                    [gc.KeyTuple('$ACTION_MEMBER_ID', x)])
            data = act_profile.make_data(
                    [gc.DataTuple('dst_port', self.eg_port)],
                    'SwitchIngress.nexthop_shared.set_eg_port')
            act_profile.entry_add(self.target, [key], [data])
            ap_entries.append(x)

        logger.info("Adding selector entry...")
        sel_grp_id = 0
        key = selector.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', sel_grp_id)])
        data = selector.make_data([
            gc.DataTuple('$MAX_GROUP_SIZE', num_ap_entries),
            gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=ap_entries),
            gc.DataTuple('$ACTION_MEMBER_STATUS', bool_arr_val=[True] * len(ap_entries))
            ])
        selector.entry_add(self.target, [key], [data])
        dmacs = []
        for tbl_type in [tern, exm]:
            logger.info("Populating %s table...",
                    "TERN" if tbl_type == tern else "EXM")
            for x in range(num_mat_entries): 
                dmac = 0x01122334400 | x
                dmac |= tbl_type << 40
                logger.info("Adding entry dmac 0x{:X}...".format(dmac))
                add_mat_entry(self, self.ecmp[tbl_type], tbl_type, dmac, sel_grp_id)
                check_traffic(self, [self.eg_port], [dmac], tbl_type) 
                dmacs.append(dmac)

        # Tested sizes, last one same as original size.
        sizes = [num_ap_entries + 2, max_grp_size // 2, max_grp_size]
        for x in range(10):
            sizes.append(random.randint(num_ap_entries, max_grp_size))
        sizes.append(num_ap_entries)
        for size in sizes:
            logger.info("Modifying selector entry - Change size (%d) "
                "and verify trafic still flows...", size)
            mod_selector_entry(self, selector, sel_grp_id, size, ap_entries)
            check_traffic(self, [self.eg_port], dmacs[:num_mat_entries]) 
            check_traffic(self, [self.eg_port], dmacs[num_mat_entries:], 1) 

        cleanup(self)

    def runTest(self):
        self.runBaseTest()
        logger.info("Rerun enabling sequence order")
        selector_member_order = True
        dev_data = self.dev_cfg_table.make_data([
                      gc.DataTuple('selector_member_order',
                      bool_val=selector_member_order)])
        self.dev_cfg_table.default_entry_set(self.target, dev_data)
        self.runBaseTest()

@group("thorough")
class ResizeWithTrans(BfRuntimeTest):
    """ @brief This test populates action profile table and creates selector table with Transactions.
        Test is divided in 3 cases:
        1. Resizing without members
        2. Resizing with the same members
        3. Resizing with new random members
        4. Transaction Rollback on error
    """
    def __init__(self):
        BfRuntimeTest.__init__(self)

    def setUp(self):
        setup_tbls(self)
        setup_ports(self)
        setup_random()

    def tearDown(self):
        # Requires self.target to be properly set.
        cleanup(self)
        BfRuntimeTest.tearDown(self)

    def runTest(self):
        act_profile = self.act_profile[tern]
        selector = self.selector[tern]
        ecmp = self.ecmp[tern]

        max_grp_size = selector.info.size_get()
        num_ap_entries = act_profile.info.size_get()
        num_test_iter = 10
        num_ports = 4

        for symmetric in [True, False, False]:
            if symmetric == True:
                pipe = 0xffff
                mode = bfruntime_pb2.Mode.ALL
            else:
                pipe = random.randint(0, self.num_pipes - 1)
                mode = bfruntime_pb2.Mode.SINGLE

            t = gc.Target(device_id=0, pipe_id=0xffff)
            ecmp.attribute_entry_scope_set(t, predefined_pipe_scope=True, predefined_pipe_scope_val=mode)
            self.target = gc.Target(device_id=0, pipe_id=pipe)
            logger.info("Testing pipe {}...".format(pipe))
            logger.info("Populating action profile table with {} entries..."
                    .format(num_ap_entries))
            for x in range(num_ap_entries):
                key = act_profile.make_key(
                        [gc.KeyTuple('$ACTION_MEMBER_ID', x)])
                data = act_profile.make_data(
                        [gc.DataTuple('dst_port', self.ports[x % num_ports])],
                        'SwitchIngress.nexthop_t.set_eg_port')
                act_profile.entry_add(self.target, [key], [data])

            sel_grp_id = 0
            key = selector.make_key(
                    [gc.KeyTuple('$SELECTOR_GROUP_ID', sel_grp_id)])
            data = selector.make_data(
                    [gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size)])

            logger.info("Adding selector entry...")
            selector.entry_add(self.target, [key], [data])

            # Some edge cases + random values
            max_grp_sizes = [1, max_grp_size, 1, max_grp_size // 2, 1]
            for x in range(num_test_iter):
                max_grp_sizes.append(random.randint(1, max_grp_size))

            ap_entries = []
            logger.info("Modifying selector entry {} times - no members..."
                    .format(len(max_grp_sizes)))
            for size in max_grp_sizes:
                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries,None,False)
                for data, _ in get_selector_entry(self, selector, sel_grp_id):
                    dd = data.to_dict()
                    assert dd['$MAX_GROUP_SIZE'] == size
            mod_selector_entry(self, selector, sel_grp_id, 1, [2],None,True)

            for data, _ in get_selector_entry(self, selector, sel_grp_id):
                dd = data.to_dict()
                assert dd['$MAX_GROUP_SIZE'] == 1
                assert dd['$ACTION_MEMBER_ID'] == [2]
                assert dd['$ACTION_MEMBER_STATUS'] == [True] * (1)
            
             # Mat entry requires selector to have at least 1 entry in AP.
            dmac = 0x001234987654
            logger.info("Adding ecmp entry dmac 0x{:X}...".format(dmac))
            add_mat_entry(self, ecmp, tern, dmac, sel_grp_id,True)
            # Will be hashed to port [x]
            if symmetric:
                check_traffic(self, self.ports[:num_ports], [dmac])

            # Random values, next test expect no changes in action profile
            min_grp_size = 512
            max_grp_sizes = [max_grp_size - 1, min_grp_size]
            for x in range(num_test_iter):
                max_grp_sizes.append(random.randint(min_grp_size, max_grp_size))

            logger.info("Modifying selector entry {} times - random same {} members..."
                    .format(len(max_grp_sizes), min_grp_size))
            # Generate data
            ap_entries = []
            for x in range(min_grp_size):
                ap_id = random.randint(0, num_ap_entries - 1)
                while ap_id in ap_entries:
                    ap_id = random.randint(0, num_ap_entries - 1)
                ap_entries.append(ap_id)
            ap_entries.sort()

            for size in max_grp_sizes:
                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries,None,True)
                # Arrays need to be sorted for comparison
                for data, _ in get_selector_entry(self, selector, sel_grp_id):
                    dd = data.to_dict()
                    dd['$ACTION_MEMBER_ID'].sort()
                    assert dd['$MAX_GROUP_SIZE'] == size
                    assert dd['$ACTION_MEMBER_ID'] == ap_entries
                    assert dd['$ACTION_MEMBER_STATUS'] == [True] * (min_grp_size)
                if symmetric:
                    check_traffic(self, self.ports[:num_ports], [dmac])

            logger.info("Modifying selector entry - remove all members...")
            ecmp.entry_del(self.target, [])
            ap_entries = []
            mod_selector_entry(self, selector, sel_grp_id, max_grp_size, ap_entries,None,True)

            # Some edge cases + random values
            max_grp_sizes = [max_grp_size - 1, max_grp_size // 2]
            for x in range(num_test_iter):
                max_grp_sizes.append(random.randint(2, max_grp_size))

            logger.info("Modifying selector entry {} times - random new members..."
                    .format(len(max_grp_sizes)))

            mod_selector_entry(self, selector, sel_grp_id, 1, [0],None,True)
            # Mat entry requires selector to have at least 1 entry in AP.
            logger.info("Adding ecmp entry dmac 0x{:X}...".format(dmac))
            add_mat_entry(self, ecmp, tern, dmac, sel_grp_id,True)
            if symmetric:
                check_traffic(self, self.ports[:num_ports], [dmac])

            # Same test with action profile entries added to selector
            for size in max_grp_sizes:
                # Generate data
                ap_entries = []
                for x in range(size):
                    ap_id = random.randint(0, num_ap_entries - 1)
                    while ap_id in ap_entries:
                        ap_id = random.randint(0, num_ap_entries - 1)
                    ap_entries.append(ap_id)
                mod_selector_entry(self, selector, sel_grp_id, size, ap_entries,None,True)
                # Arrays need to be sorted for comparison
                ap_entries.sort()
                for data, _ in get_selector_entry(self, selector, sel_grp_id):
                    dd = data.to_dict()
                    dd['$ACTION_MEMBER_ID'].sort()
                    assert dd['$MAX_GROUP_SIZE'] == size
                    assert dd['$ACTION_MEMBER_ID'] == ap_entries
                    assert dd['$ACTION_MEMBER_STATUS'] == [True] * (size)
                if symmetric:
                    check_traffic(self, self.ports[:num_ports], [dmac])

            cleanup(self)

            # Transaction Rollback test will be validate by adding a invalid action id to selector group
            for x in range(num_ap_entries):
                key = act_profile.make_key(
                        [gc.KeyTuple('$ACTION_MEMBER_ID', x)])
                data = act_profile.make_data(
                        [gc.DataTuple('dst_port', self.ports[x % num_ports])],
                        'SwitchIngress.nexthop_t.set_eg_port')
                act_profile.entry_add(self.target, [key], [data],bfruntime_pb2.WriteRequest.ROLLBACK_ON_ERROR)
            sel_grp_size=100
            #create a list of 100 random action meber list 
            action_mbr_list = [random.randrange(1, num_ap_entries, 1) for i in range(sel_grp_size)]
            action_mbr_list.sort()
            sel_grp_id = 0
            key = selector.make_key(
                    [gc.KeyTuple('$SELECTOR_GROUP_ID', sel_grp_id)])
            data = selector.make_data(
                    [gc.DataTuple('$MAX_GROUP_SIZE', sel_grp_size),
                    gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=action_mbr_list),
                    gc.DataTuple('$ACTION_MEMBER_STATUS', bool_arr_val= [True]*sel_grp_size),
                    ])

            logger.info("Adding selector entry...")
            selector.entry_add(self.target, [key], [data], bfruntime_pb2.WriteRequest.ROLLBACK_ON_ERROR)
            for data, _ in get_selector_entry(self, selector, sel_grp_id):
                dd = data.to_dict()
                dd['$ACTION_MEMBER_ID'].sort()
                assert dd['$MAX_GROUP_SIZE'] == sel_grp_size
                assert dd['$ACTION_MEMBER_ID'] == action_mbr_list
                assert dd['$ACTION_MEMBER_STATUS'] == [True] * (sel_grp_size)
            logger.info("Simulating Rollback Transaction with invalid selector entry...")
            #create invalid transaction by adding an action profile id which is not valid
            action_mbr_list1 = copy.deepcopy(action_mbr_list)
            action_mbr_list1.append(num_ap_entries+1)
            data1 = selector.make_data(
                    [gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=action_mbr_list1),
                    gc.DataTuple('$ACTION_MEMBER_STATUS',  bool_arr_val= [True]*len(action_mbr_list1)),
                    ])
            try:
                selector.entry_mod(self.target, [key], [data1],{"reset_ttl":True}, bfruntime_pb2.WriteRequest.ROLLBACK_ON_ERROR)
                logger.info("Expected to fail.. But entry mod worked - Failed!")
                assert False
            except:
                logger.info("Expected fail and validating rollback..")
                for data, _ in get_selector_entry(self, selector, sel_grp_id):
                    dd = data.to_dict()
                    dd['$ACTION_MEMBER_ID'].sort()
                    assert dd['$MAX_GROUP_SIZE'] == sel_grp_size
                    assert dd['$ACTION_MEMBER_ID'] == action_mbr_list
                    assert dd['$ACTION_MEMBER_STATUS'] == [True] * (sel_grp_size)
            cleanup(self)
