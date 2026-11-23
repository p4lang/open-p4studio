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

import ptf.testutils as testutils
from bfruntime_client_base_tests import BfRuntimeTest
from p4testutils.misc_utils import *
from p4testutils.bfrt_utils import *
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as gc
import random

logger = get_logger()
swports = get_sw_ports()

client_id = 0
p4_name = "tna_register"
arch = testutils.test_param_get("arch")


def VerifyReadRegisters(self, register_name_lo, register_name_hi,
                        register_value_lo, register_value_hi, data_dict):
    # After update to register table handling, number of register data depends
    # on how many pipes were present in the request.
    for i in range(len(register_value_lo)):
        value_lo = data_dict[register_name_lo][i]
        value_hi = data_dict[register_name_hi][i]
        assert value_lo == register_value_lo[i], \
                                        "Register field lo didn't match with the read value." \
                                        " Lo[%d] Expected Value (%d) : Read value (%d)" % (
                                        i, register_value_lo[i], value_lo)
        assert value_hi == register_value_hi[i], \
                                        "Register field hi didn't match with the read value." \
                                        " Hi[%d] Expected Value (%d) : Read value (%d)" % (
                                        i, register_value_hi[i], value_hi)


class RegisterTest(BfRuntimeTest):
    """@brief This test adds a value in an indirect register table and then
        reads the value back using the single-entry sync-from-hw functionality.
    """

    def setUp(self):
        client_id = 0
        p4_name = "tna_register"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runTest(self):
        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_register")

        register_idx = random.randint(0, 500)
        register_value_hi = random.randint(0, 1000)
        register_value_lo = random.randint(0, 1000)
        logger.info("Register Index %d", register_idx)
        logger.info("Register value hi %s", str(register_value_hi))
        logger.info("Register value lo %s", str(register_value_lo))
        register_value_hi_arr = {}
        register_value_lo_arr = {}
        num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))

        for i in range(num_pipes):
            register_value_hi_arr[i] = register_value_hi
            register_value_lo_arr[i] = register_value_lo

        target = gc.Target(device_id=0, pipe_id=0xffff)
        register_table = bfrt_info.table_get("SwitchIngress.test_reg")

        # Get the current value of the register.
        resp = register_table.entry_get(
            target,
            [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', register_idx)])],
            {"from_hw": True})

        data, _ = next(resp)
        data_dict = data.to_dict()
        init_hi = data_dict["SwitchIngress.test_reg.second"][0]
        init_lo = data_dict["SwitchIngress.test_reg.first"][0]

        # Note: Short names for data fields work too as long as they
        # are sufficiently unique
        register_table.entry_add(
            target,
            [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', register_idx)])],
            [register_table.make_data(
                [gc.DataTuple('first', register_value_lo),
                 gc.DataTuple('test_reg.second', register_value_hi)])])

        resp = register_table.entry_get(
            target,
            [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', register_idx)])],
            {"from_hw": True})

        data, _ = next(resp)
        data_dict = data.to_dict()
        VerifyReadRegisters(self, "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                            register_value_lo_arr, register_value_hi_arr, data_dict)

        # Restore the original register value
        register_table.entry_add(
            target,
            [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', register_idx)])],
            [register_table.make_data(
                [gc.DataTuple('first', init_lo),
                 gc.DataTuple('test_reg.second', init_hi)])])


class IndirectRegisterSyncTest(BfRuntimeTest):
    """@brief This test adds a value in a register table and then checks the sync from_hw functionality.
        It also clears the table and checks whether the entries added were removed or not.
    """

    def setUp(self):
        client_id = 0
        p4_name = "tna_register"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def tearDown(self):
        logger.info("Deleting entries from the match action and the indirect stful tables")
        self.reg_match_table.entry_del(self.target)
        self.register_table.entry_del(self.target)
        self.test_cdn.default_entry_reset(self.target)
        self.test_asn.default_entry_reset(self.target)

        logger.info("Reading back the indirect register table and ensuring it got deleted")
        resp = self.register_table.entry_get(
            self.target,
            [self.register_table.make_key(
                [gc.KeyTuple('$REGISTER_INDEX', self.reg_index)])],
            {"from_hw": True})
        for data, key in resp:
            data_dict = data.to_dict()
            assert data_dict["SwitchIngress.test_reg.first"] == [5]*self.num_pipes
            assert data_dict["SwitchIngress.test_reg.second"] == [7]*self.num_pipes
        BfRuntimeTest.tearDown(self)


    def insertEntry(self, target, register_idx, register_val):
        self.register_bool_table.entry_add(
            target,
            [self.register_bool_table.make_key([gc.KeyTuple('$REGISTER_INDEX', register_idx)])],
            [self.register_bool_table.make_data(
                [gc.DataTuple('SwitchIngress.bool_register_table.f1', register_val)])])

    def clearTable(self):
        target = gc.Target(device_id=0, pipe_id=0xffff)
        resp = self.register_bool_table.entry_del(target)
        return resp

    def runTest(self):
        ig_port = swports[1]
        reg_index = random.randint(0, 1023)
        self.reg_index = reg_index
        dmac = '00:11:22:33:44:55'

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_register")
        self.register_bool_table = bfrt_info.table_get('SwitchIngress.bool_register_table')

        register_table = bfrt_info.table_get("SwitchIngress.test_reg")
        reg_match_table = bfrt_info.table_get("SwitchIngress.reg_match")
        test_cdn = bfrt_info.table_get("SwitchIngress.test_cdn")
        test_asn = bfrt_info.table_get("SwitchIngress.test_asn")
        reg_match_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        self.reg_match_table = reg_match_table
        self.register_table = register_table
        self.test_cdn = test_cdn
        self.test_asn = test_asn

        register_value_hi = random.randint(0, 12345678)
        register_value_lo = random.randint(0, 23456789)
        register_value_hi_arr = {}
        register_value_lo_arr = {}
        num_pipes = int(testutils.test_param_get('num_pipes'))
        self.num_pipes = num_pipes
        for i in range(num_pipes):
            register_value_hi_arr[i] = 7
            register_value_lo_arr[i] = 5

        # Prepare bool_register_table - fill with zeroes.
        target = gc.Target(device_id=0, pipe_id=0xffff)
        self.register_bool_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                                           predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
        logger.info("Set registers in all pipes to 0")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        register_idx = 0
        num_entries = 1024
        for register_idx in range(num_entries):
            self.insertEntry(target, register_idx, 0)

        pkt = testutils.simple_tcp_packet(eth_dst=dmac)
        exp_pkt = pkt

        # Verify initial register values of hi = 7 and lo = 5
        logger.info("Verify initial values of hi = 7 and lo = 5 in test_reg stful table")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        self.target = target
        resp = register_table.entry_get(
                target,
                [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', reg_index)])],
                {"from_hw": False})
        data, _ = next(resp)
        data_dict = data.to_dict()
        VerifyReadRegisters(self, "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                            register_value_lo_arr, register_value_hi_arr, data_dict)
        logger.info("Indirect Register initial values matched these defined in P4 program")


        for i in range(num_pipes):
            register_value_hi_arr[i] = register_value_hi
            register_value_lo_arr[i] = register_value_lo

        # Program the same register values in all the 4 pipes
        logger.info("Inserting entry in test_reg stful table with hi = %s lo = %s", str(register_value_hi),
                str(register_value_lo))

        # insert entry in MAT and the indirect register table both
        register_table.entry_add(
            target,
            [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', reg_index)])],
            [register_table.make_data(
                [gc.DataTuple('SwitchIngress.test_reg.first', register_value_lo),
                    gc.DataTuple('SwitchIngress.test_reg.second', register_value_hi)])])
        reg_match_table.entry_add(
            target,
            [reg_match_table.make_key([gc.KeyTuple('hdr.ethernet.dst_addr', dmac)])],
            [reg_match_table.make_data(
                [gc.DataTuple('idx', reg_index)],
                'SwitchIngress.register_action')])


        # Apply table operations to sync the registers on the indirect register table
        logger.info("Syncing indirect stful registers")
        register_table.operations_execute(target, 'Sync')

        # Get from sw and check its value.
        logger.info("Reading back the indirect stful registers for the programmed match entry")
        resp = register_table.entry_get(
            target,
            [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', reg_index)])],
            {"from_hw": False})
        # The values read back should match the initialized values
        logger.info("Verifying read back indirect register values")
        data, _ = next(resp)
        data_dict = data.to_dict()
        VerifyReadRegisters(self, "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                            register_value_lo_arr, register_value_hi_arr, data_dict)
        logger.info("Indirect Register values matched before seding packets")

        num_pkts = random.randint(1, 10)
        logger.info("Sending %d packets on port %d", num_pkts, ig_port)
        for i in range(num_pkts):
            testutils.send_packet(self, ig_port, pkt)
            testutils.verify_packet(self, pkt, ig_port)

        # Apply table operations to sync the registers on the indirect register table
        logger.info("Syncing indirect stful registers")
        register_table.operations_execute(target, 'Sync')

        # Get from sw and check its value. They should be the correct updated values now
        logger.info("Reading back the indirect stful registers for the programmed match entry")
        resp = register_table.entry_get(
            target,
            [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', reg_index)])],
            {"from_hw": False})
        # Since the packet is sent on a single pipe only the register entry
        # from that pipe will be updated.
        ingress_pipe = ig_port >> 7
        new_reg_val_lo = register_value_lo + (1 * num_pkts)
        new_reg_val_hi = register_value_hi + (100 * num_pkts)
        register_value_lo_arr[ingress_pipe] = new_reg_val_lo
        register_value_hi_arr[ingress_pipe] = new_reg_val_hi
        logger.info("Verifying read back indirect register values")
        data, _ = next(resp)
        data_dict = data.to_dict()
        VerifyReadRegisters(self, "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                            register_value_lo_arr, register_value_hi_arr, data_dict)
        logger.info("Indirect Register values matched after seding packets")

        # Change register param on conditions to verify it works with frames.
        # test_cdn < 100 will cause register_value_hi to not increment.
        asn_val = 10
        test_cdn.default_entry_set(target, test_cdn.make_data([gc.DataTuple("value", 100)]))
        test_asn.default_entry_set(target, test_asn.make_data([gc.DataTuple("value", asn_val)]))

        num_pkts = random.randint(1, 10)
        logger.info("Sending %d packets on port %d", num_pkts, ig_port)
        for i in range(num_pkts):
            testutils.send_packet(self, ig_port, pkt)
            testutils.verify_packet(self, pkt, ig_port)

        # Apply table operations to sync the registers on the indirect register table
        logger.info("Syncing indirect stful registers")
        register_table.operations_execute(target, 'Sync')

        # Get from sw and check its value. They should be the correct updated values now
        logger.info("Reading back the indirect stful registers for the programmed match entry")
        resp = register_table.entry_get(
            target,
            [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', reg_index)])],
            {"from_hw": False})
        # Since the packet is sent on a single pipe only the register entry
        # from that pipe will be updated.
        ingress_pipe = ig_port >> 7
        register_value_lo_arr[ingress_pipe] = new_reg_val_lo + (asn_val * num_pkts)
        register_value_hi_arr[ingress_pipe] = new_reg_val_hi
        logger.info("Verifying read back indirect register values")
        data, _ = next(resp)
        data_dict = data.to_dict()
        VerifyReadRegisters(self, "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                            register_value_lo_arr, register_value_hi_arr, data_dict)
        logger.info("Indirect Register values matched after seding packets")

        logger.info("Clear the bool_register_table")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        self.register_bool_table.attribute_entry_scope_set(
            target, predefined_pipe_scope=True,
            predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
        self.clearTable()



class DirectRegisterSyncTest(BfRuntimeTest):
    def setUp(self):
        client_id = 0
        p4_name = "tna_register"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def insertEntry(self, target, register_idx, register_val):
        self.register_bool_table.entry_add(
            target,
            [self.register_bool_table.make_key([gc.KeyTuple('$REGISTER_INDEX', register_idx)])],
            [self.register_bool_table.make_data(
                [gc.DataTuple('SwitchIngress.bool_register_table.f1', register_val)])])

    def clearTable(self):
        target = gc.Target(device_id=0, pipe_id=0xffff)
        resp = self.register_bool_table.entry_del(target)
        return resp

    def runTest(self):
        ig_port = swports[1]
        smac = '00:11:22:33:44:55'

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_register")
        self.register_bool_table = bfrt_info.table_get('SwitchIngress.bool_register_table')

        register_dir_table = bfrt_info.table_get("SwitchIngress.reg_match_dir")
        register_dir_table.info.key_field_annotation_add("hdr.ethernet.src_addr", "mac")

        register_value_hi = random.randint(0, 12345678)
        register_value_lo = random.randint(0, 23456789)
        register_value_hi_arr = {}
        register_value_lo_arr = {}

        # Prepare bool_register_table - fill with zeroes.
        logger.info("Zeroing bool_register_table")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        self.register_bool_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                                           predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
        logger.info("Set registers in all pipes to 0")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        register_idx = 0
        num_entries = 1024
        for register_idx in range(num_entries):
            self.insertEntry(target, register_idx, 0)

        num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))
        for i in range(num_pipes):
            register_value_hi_arr[i] = register_value_hi
            register_value_lo_arr[i] = register_value_lo

        pkt = testutils.simple_tcp_packet(eth_src=smac)
        exp_pkt = pkt

        # Program the same register values in all the 4 pipes
        logger.info("Inserting entry in reg_match_dir table and programming stful reg with hi = %s lo = %s",
                    str(register_value_hi), str(register_value_lo))
        target = gc.Target(device_id=0, pipe_id=0xffff)
        try:
            # Note: Short names for data fields work too as long as they
            # are sufficiently unique
            # Insert entry in Match table
            register_dir_table.entry_add(
                target,
                [register_dir_table.make_key([gc.KeyTuple('hdr.ethernet.src_addr', smac)])],
                [register_dir_table.make_data(
                    [gc.DataTuple('SwitchIngress.test_reg_dir.first', register_value_lo),
                     gc.DataTuple('second', register_value_hi)],
                    'SwitchIngress.register_action_dir')])

            # Apply table operations to sync the direct registers
            logger.info("Syncing stful registers")
            register_dir_table.operations_execute(target, 'SyncRegisters')

            # Get from sw and check its value
            logger.info("Reading back the stful registers for the programmed match entry")
            resp = register_dir_table.entry_get(
                target,
                [register_dir_table.make_key([gc.KeyTuple('hdr.ethernet.src_addr', smac)])],
                {"from_hw": False})
            # The values read back should match the initialized values
            logger.info("Verifying read back register values")
            data_dict = next(resp)[0].to_dict()
            VerifyReadRegisters(self, "SwitchIngress.test_reg_dir.first", "SwitchIngress.test_reg_dir.second",
                                register_value_lo_arr, register_value_hi_arr, data_dict)
            logger.info("Register values matched before sending packets")

            num_pkts = random.randint(1, 10)
            logger.info("Sending %d packets on port %d", num_pkts, ig_port)
            for i in range(num_pkts):
                testutils.send_packet(self, ig_port, pkt)
                testutils.verify_packet(self, pkt, ig_port)

            # Apply table operations to sync the direct registers
            logger.info("Syncing stful registers")
            register_dir_table.operations_execute(target, 'SyncRegisters')

            # Get from sw and check its value. They should be updated
            logger.info("Reading back the stful registers for the programmed match entry")
            resp = register_dir_table.entry_get(
                target,
                [register_dir_table.make_key([gc.KeyTuple('hdr.ethernet.src_addr', smac)])],
                {"from_hw": False})

            # Since the packet is sent on a single pipe only the register entry
            # from that pipe will be updated.
            ingress_pipe = ig_port >> 7
            register_value_lo_arr[ingress_pipe] = register_value_lo + (1 * num_pkts)
            register_value_hi_arr[ingress_pipe] = register_value_hi + (100 * num_pkts)
            logger.info("Verifying read back register values")
            data_dict = next(resp)[0].to_dict()
            VerifyReadRegisters(self, "SwitchIngress.test_reg_dir.first", "SwitchIngress.test_reg_dir.second",
                                register_value_lo_arr, register_value_hi_arr, data_dict)
            logger.info("Register values matched after seding packets")

        finally:
            logger.info("Deleting entry from the table")
            register_dir_table.entry_del(
                target,
                [register_dir_table.make_key([gc.KeyTuple('hdr.ethernet.src_addr', smac)])])

            logger.info("Clear the bool_register_table")
            target = gc.Target(device_id=0, pipe_id=0xffff)
            self.register_bool_table.attribute_entry_scope_set(
                target, predefined_pipe_scope=True,
                predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
            self.clearTable()


class RegisterAttributesTest(BfRuntimeTest):
    def setUp(self):
        client_id = 0
        p4_name = "tna_register"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def verifyRegisterValue(self, resp, register_value):
        # Since the table exists in all pipes and 1 stage per pipe, we expect
        # to get back a number of register values equal to the number of pipes
        # (one each per pipe and stage).
        num_register_values = self.num_pipes

        data_dict = next(resp)[0].to_dict()

        for i in range(num_register_values):
            read_value = data_dict["SwitchIngress.bool_register_table.f1"][i]
            logger.info("Register Expected Value (%s) : Read value (%s)", str(register_value[i]), str(read_value))
            if read_value != register_value[i]:
                logger.error("Register field didn't match with the read value")
                assert (0)

    def insertEntry(self, target, register_idx, register_val):
        self.register_bool_table.entry_add(
            target,
            [self.register_bool_table.make_key([gc.KeyTuple('$REGISTER_INDEX', register_idx)])],
            [self.register_bool_table.make_data(
                [gc.DataTuple('SwitchIngress.bool_register_table.f1', register_val)])])

    def getEntry(self, register_idx):
        # Please note that grpc server is always going to return all instances of the register
        # i.e. one per pipe and stage the table exists in. The asymmetric suport for indirect
        # register tables is limited only to the insertion of the entries. Thus even if we
        # made the indirect register table asymmetric, we need to pass in the device target
        # as consisting of all the pipes while reading the entry
        target = gc.Target(device_id=0, pipe_id=0xffff)
        resp = self.register_bool_table.entry_get(
            target,
            [self.register_bool_table.make_key(
                [gc.KeyTuple('$REGISTER_INDEX', register_idx)])],
            {"from_hw": True})
        return resp

    def clearTable(self):
        target = gc.Target(device_id=0, pipe_id=0xffff)
        resp = self.register_bool_table.entry_del(target)
        return resp

    def runTest(self):
        register_idx = random.randint(0, 500)
        register_value = {}

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_register")
        self.register_bool_table = bfrt_info.table_get('SwitchIngress.bool_register_table')
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))
        pipes = list(range(self.num_pipes))

        # Run this test only for 4 pipe devices
        if self.num_pipes < 4:
            logger.info("Skipping Entry scope test for a non 4 pipe device")
        else:
            # The default values of all the registers is 1 (see p4)
            target = gc.Target(device_id=0, pipe_id=0xffff)
            self.register_bool_table.attribute_entry_scope_set(
                target, predefined_pipe_scope=True,
                predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
            target = gc.Target(device_id=0, pipe_id=0xffff)
            #Test initial value specified in P4 - shall be 1 for all pipes
            register_value = {0: 1, 1: 1, 2: 1, 3: 1}
            resp = self.getEntry(register_idx)
            self.verifyRegisterValue(resp, register_value)

            # Set pipes 0 and 1 in scope 1 and pipes 2 and 3 in scope 2
            logger.info("")
            logger.info("=============== Tesing User Defined Scope:"
                        "Scope 1 (pipe 0 and 1), Scope 2 (pipe 2 and 3) ===============")
            logger.info("Scope 1 (pipe 0 and 1), Scope 2 (pipe 2 and 3)")
            target = gc.Target(device_id=0, pipe_id=0xffff)
            self.register_bool_table.attribute_entry_scope_set(
                target, predefined_pipe_scope=False,
                predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL, user_defined_pipe_scope_val=0xc03)

            # Set the register in pipes 0 and 1 to 0
            logger.info("Set the registers in pipes 0 and 1 to 0")
            target = gc.Target(device_id=0, pipe_id=0x0)
            self.insertEntry(target, register_idx, 0)
            resp = self.getEntry(register_idx)
            # pipes 2 and 3 will have the default value of 1
            register_value = {0: 0, 1: 0, 2: 1, 3: 1}
            self.verifyRegisterValue(resp, register_value)

            # Now set the registers in pipes 2 and 3 to 0 as well
            logger.info("Now set the registers in pipes 2 and 3 to 0 as well")
            target = gc.Target(device_id=0, pipe_id=0x2)
            self.insertEntry(target, register_idx, 0)
            resp = self.getEntry(register_idx)
            # pipes 2 and 3 will also be equal to 0
            register_value = {0: 0, 1: 0, 2: 0, 3: 0}
            self.verifyRegisterValue(resp, register_value)

            # Now clear the table and verify if the entries restored the initial value of 1
            logger.info("Clear the table and verify initial values of 1")
            target = gc.Target(device_id=0, pipe_id=0xffff)
            self.register_bool_table.attribute_entry_scope_set(
                target, predefined_pipe_scope=True,
                predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
            target = gc.Target(device_id=0, pipe_id=0xffff)
            register_value = {0: 1, 1: 1, 2: 1, 3: 1}
            self.clearTable()
            resp = self.getEntry(register_idx)
            self.verifyRegisterValue(resp, register_value)


        # program 0 in all the pipes
        # Set the scope of the table to ALL_PIPES
        logger.info("")
        logger.info("=============== Testing All Pipe Scope ===============")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        self.register_bool_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                                           predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
        logger.info("Set registers in all pipes to 0")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        self.insertEntry(target, register_idx, 0)

        # Read back values should be all 0s
        resp = self.getEntry(register_idx)
        register_value = {0: 0, 1: 0, 2: 0, 3: 0}
        self.verifyRegisterValue(resp, register_value)

        # Set the scope of the table to SINGLE_PIPE
        logger.info("=============== Testing Single Pipe Scope ===============")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        self.register_bool_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                                           predefined_pipe_scope_val=bfruntime_pb2.Mode.SINGLE)
        # Program a register in pipe 1 as 1
        if 1 in pipes:
            logger.info("Set register in pipe 1 to 1")
            target = gc.Target(device_id=0, pipe_id=0x1)
            self.insertEntry(target, register_idx, 1)
            resp = self.getEntry(register_idx)
            register_value = {pipe:0 for pipe in pipes}
            register_value[1] = 1
            self.verifyRegisterValue(resp, register_value)

        if 3 in pipes:
            logger.info("Adding entry in pipe 3")
            # Program a register in pipe 3 as 1
            logger.info("Set register in pipe 3 to 1")
            target = gc.Target(device_id=0, pipe_id=0x3)
            self.insertEntry(target, register_idx, 1)
            resp = self.getEntry(register_idx)
            register_value = {pipe:0 for pipe in pipes}
            register_value[1] = 1
            register_value[3] = 1
            self.verifyRegisterValue(resp, register_value)


class IndirectRegisterIteratorTest(BfRuntimeTest):
    def setUp(self):
        client_id = 0
        p4_name = "tna_register"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runTest(self):
        '''
        This test does the following
        1. Adds 1k register table entries with random data
        2. Does a register sync table operation
        3. Gets all the entries from the register table
        4. Verifies the read values match what has been programmed
        '''

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_register")
        num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))
        register_table = bfrt_info.table_get("SwitchIngress.test_reg")

        num_entries = 1024
        # Program the same register values in all the 4 pipes
        logger.info("Inserting %d entries into the register table", num_entries)
        target = gc.Target(device_id=0, pipe_id=0xffff)

        register_value_hi = [random.randint(1, 10000) for x in range(num_entries)]
        register_value_lo = [random.randint(1, 10000) for x in range(num_entries)]

        for x in range(num_entries):
            register_table.entry_add(
                target,
                [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', x)])],
                [register_table.make_data(
                    [gc.DataTuple('SwitchIngress.test_reg.first', register_value_lo[x]),
                     gc.DataTuple('SwitchIngress.test_reg.second', register_value_hi[x])])])
        # Apply table operations to sync the registers on the indirect register table
        logger.info("Syncing indirect stful registers")
        register_table.operations_execute(target, 'Sync')

        # Get from sw and check its value.
        logger.info("Reading back the register table entries")
        resp = register_table.entry_get(
            target,
            flags={"from_hw": False})

        # The values read back should match the initialized values
        logger.info("Verifying read back indirect register values")
        i = 0
        for data, key in resp:
            data_dict = data.to_dict()
            key_dict = key.to_dict()
            VerifyReadRegisters(self, "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                                [register_value_lo[i]] * num_pipes, [register_value_hi[i]] * num_pipes, data_dict)
            assert key_dict["$REGISTER_INDEX"]['value'] == i
            i += 1
        # Clear the table to set default values
        logger.info("Clearing the register table")
        register_table.entry_del(target)

class RegisterParamTest(BfRuntimeTest):
    """@brief Executes register param APIs and verifies values.
    """

    def setUp(self):
        client_id = 0
        p4_name = "tna_register"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def tearDown(self):
        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_register")
        target = gc.Target(device_id=0, pipe_id=0xffff)

        test_asn = bfrt_info.table_get("SwitchIngress.test_asn")
        test_cdn = bfrt_info.table_get("SwitchIngress.test_cdn")

        test_asn.default_entry_reset(target)
        test_cdn.default_entry_reset(target)
        BfRuntimeTest.tearDown(self)

    def runTest(self):
        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_register")

        test_asn = bfrt_info.table_get("SwitchIngress.test_asn")
        test_cdn = bfrt_info.table_get("SwitchIngress.test_cdn")

        # Get param values
        asn_val = 0
        asn_exp = 1
        cdn_val = 0
        cdn_exp = 100000000
        target = gc.Target(device_id=0, pipe_id=0xffff)

        resp = test_asn.default_entry_get(target)
        for data, _ in resp:
            dd = data.to_dict()
            asn_val = dd["value"]

        resp = test_cdn.default_entry_get(target)
        for data, _ in resp:
            dd = data.to_dict()
            cdn_val = dd["value"]

        logger.info("Reg param init values: cdn %s asn %s", str(cdn_val), str(asn_val))
        assert asn_val == asn_exp
        assert cdn_val == cdn_exp

        for asn_exp, cdn_exp in [
                [random.randint(0, 2 ** 31), random.randint(0, 2 ** 31)],
                [0, 0],
                [-1, -1],
                [2**32 - 1, 2**32 -1]]:
            logger.info("Reg param values set: cdn %s asn %s",
                    str(cdn_exp), str(asn_exp))
            # Based on bytes it is not possible to tell if provided value was
            # negative or not on get, hence convert negative values to unsigned
            if asn_exp < 0:
                asn_exp &= 0xffffffff
            if cdn_exp < 0:
                cdn_exp &= 0xffffffff

            test_asn.default_entry_set(
                target,
                test_asn.make_data(
                    [gc.DataTuple('value', asn_exp)]))

            test_cdn.default_entry_set(
                target,
                test_cdn.make_data(
                    [gc.DataTuple('value', cdn_exp)]));

            resp = test_asn.default_entry_get(target)
            for data, _ in resp:
                dd = data.to_dict()
                asn_val = dd["value"]

            resp = test_cdn.default_entry_get(target)
            for data, _ in resp:
                dd = data.to_dict()
                cdn_val = dd["value"]

            logger.info("Reg param values get: cdn %s asn %s",
                    str(cdn_val), str(asn_val))
            assert asn_val == asn_exp
            assert cdn_val == cdn_exp

        asn_exp = 1
        cdn_exp = 100000000
        logger.info("Reg param values reset")

        test_asn.default_entry_reset(target)
        test_cdn.default_entry_reset(target)

        resp = test_asn.default_entry_get(target)
        for data, _ in resp:
            dd = data.to_dict()
            asn_val = dd["value"]

        resp = test_cdn.default_entry_get(target)
        for data, _ in resp:
            dd = data.to_dict()
            cdn_val = dd["value"]

        logger.info("Reg param values get: cdn %s asn %s",
                str(cdn_val), str(asn_val))
        assert asn_val == asn_exp
        assert cdn_val == cdn_exp

class IndirectRegisterAsymTest(BfRuntimeTest):
    """@brief Tests new approach to asymmrtic registers. It is possible now to
     access registers per pipe regardless of symmetricity of the table making
     register table always asymmetric.
    """

    def setUp(self):
        client_id = 0
        p4_name = "tna_register"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def tearDown(self):
        logger.info("Deleting entries from the match action and the indirect stful tables")
        target = gc.Target(device_id=0, pipe_id=0xffff)
        self.reg_match_table.entry_del(target)
        self.register_table.entry_del(target)

        BfRuntimeTest.tearDown(self)

    def clearTable(self):
        target = gc.Target(device_id=0, pipe_id=0xffff)
        resp = self.register_bool_table.entry_del(target)
        return resp

    def verifyAllPipes(self):
        # Get for all pipes after all iterations and verify different values on each pipe
        logger.info("\nVerify all pipes again reading all at once")
        logger.info("Expected values per pipe:")
        logger.info("test_reg.first %s", str(self.reg_val_lo))
        logger.info("test_reg.second %s", str(self.reg_val_hi))
        pipe = 0xffff
        target = gc.Target(device_id=0, pipe_id=pipe)
        logger.info("Testing pipe {}:".format(pipe))
        for from_hw in [False, True]:
            resp = self.register_table.entry_get(
                    target,
                    [self.register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', self.reg_index)])],
                    {"from_hw": from_hw})
            data, _ = next(resp)
            data_dict = data.to_dict()
            VerifyReadRegisters(self,
                    "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                    self.reg_val_lo, self.reg_val_hi, data_dict)
            logger.info("Indirect Register initial values matched expected from_hw=%s.",
                    from_hw)

    def runTest(self, replay=False):
        reg_index = 0
        if replay == False:
            reg_index = random.randint(0, 1023)
            self.reg_index = reg_index
        else:
            reg_index = self.reg_index

        dmac = '00:11:22:33:44:55'

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_register")

        register_table = bfrt_info.table_get("SwitchIngress.test_reg")
        reg_match_table = bfrt_info.table_get("SwitchIngress.reg_match")
        reg_match_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        self.reg_match_table = reg_match_table
        self.register_table = register_table

        if replay == False:
            register_value_hi = random.randint(0, 12345678)
            register_value_lo = random.randint(0, 23456789)
        exp_reg_value_hi_arr = {}
        exp_reg_value_lo_arr = {}
        num_pipes = int(testutils.test_param_get('num_pipes'))
        self.num_pipes = num_pipes

        # Initial values from P4 program
        if replay == False:
            for i in range(num_pipes):
                exp_reg_value_hi_arr[i] = 7
                exp_reg_value_lo_arr[i] = 5
        else:
            exp_reg_value_hi_arr = self.reg_val_hi
            exp_reg_value_lo_arr = self.reg_val_lo

        # Iterate over all pipes individualy and all_pipes
        pipe_arr = [0xffff] + list(range(num_pipes))
        for pipe in pipe_arr:
            target = gc.Target(device_id=0, pipe_id=pipe)
            logger.info("\nTesting pipe {}:".format(pipe))
            # Verify initial register values of hi = 7 and lo = 5
            if replay == False:
                logger.info("Verify initial values of hi = %s and lo = %s in test_reg stful table",
                        exp_reg_value_hi_arr[0] if pipe == 0xffff else exp_reg_value_hi_arr[pipe],
                        exp_reg_value_lo_arr[0] if pipe == 0xffff else exp_reg_value_lo_arr[pipe],
                        )
                resp = register_table.entry_get(
                        target,
                        [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', reg_index)])],
                        {"from_hw": False})
                data, _ = next(resp)
                data_dict = data.to_dict()
                VerifyReadRegisters(self,
                        "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                        exp_reg_value_lo_arr if pipe == 0xffff else [exp_reg_value_lo_arr[pipe]],
                        exp_reg_value_hi_arr if pipe == 0xffff else [exp_reg_value_hi_arr[pipe]],
                        data_dict)
                logger.info("Indirect Register initial values matched expected.")

                register_value_hi += pipe
                register_value_lo += pipe
                if pipe == 0xffff:
                    for i in range(num_pipes):
                        exp_reg_value_hi_arr[i] = register_value_hi
                        exp_reg_value_lo_arr[i] = register_value_lo
                else:
                    exp_reg_value_hi_arr[pipe] = register_value_hi
                    exp_reg_value_lo_arr[pipe] = register_value_lo

                # Program the same register values in all the 4 pipes
                logger.info("Inserting entry in test_reg stful table with hi = %s lo = %s in pipe %d",
                        register_value_hi, str(register_value_lo), pipe)

            # Insert/modify register entry to new value
            register_table.entry_add(
                target,
                [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', reg_index)])],
                [register_table.make_data(
                    [gc.DataTuple('SwitchIngress.test_reg.first',
                        exp_reg_value_lo_arr[0] if pipe == 0xffff else exp_reg_value_lo_arr[pipe]),
                     gc.DataTuple('SwitchIngress.test_reg.second',
                        exp_reg_value_hi_arr[0] if pipe == 0xffff else exp_reg_value_hi_arr[pipe])
                     ])])

            if replay == False:
                # Get from sw and check its value.
                logger.info("Reading back the indirect stful registers for the programmed match entry")
                resp = register_table.entry_get(
                    target,
                    [register_table.make_key([gc.KeyTuple('$REGISTER_INDEX', reg_index)])],
                    {"from_hw": False})
                # The values read back should match the initialized values
                logger.info("Verifying read back indirect register values")
                data, _ = next(resp)
                data_dict = data.to_dict()
                VerifyReadRegisters(self,
                        "SwitchIngress.test_reg.first", "SwitchIngress.test_reg.second",
                        exp_reg_value_lo_arr if pipe == 0xffff else [exp_reg_value_lo_arr[pipe]],
                        exp_reg_value_hi_arr if pipe == 0xffff else [exp_reg_value_hi_arr[pipe]],
                        data_dict)

            logger.info("Indirect Register values matched")
            self.reg_val_hi = exp_reg_value_hi_arr
            self.reg_val_lo = exp_reg_value_lo_arr

        # Verify values for all pipe at once
        if replay == False:
            self.verifyAllPipes()

@testutils.group("thorough")
class IndirectRegisterAsymTestWarmInit(BfRuntimeTest):
    """@brief Tests values and behavior of register table against hitless and
     fast reconfig warm init modes. Test uses IndirectRegisterAsymTest as a base.
    """

    def setUp(self):
        client_id = 0
        self.test = IndirectRegisterAsymTest()
        self.test.setUp()

    def tearDown(self):
        self.test.tearDown()

    def runTest(self):
        self.test.runTest(False)

        # Not supported on tofino3 yet
        if testutils.test_param_get('arch') == 'tofino3':
            return
        bfrt_info = self.test.interface.bfrt_info_get("tna_register")
        num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))
        self.test.port_table = bfrt_info.table_get("$PORT")
        self.test.target = gc.Target(device_id=0, pipe_id=0xFFFF, direction=0)

        # Start a hitless HA.
        start_warm_init(self.test, p4_name, num_pipes, hitless=True)
        self.test.runTest(replay=True)
        end_warm_init(self.test)

        self.test.verifyAllPipes()

        # Start a hitless HA.
        start_warm_init(self.test, p4_name, num_pipes, hitless=False)
        self.test.runTest(replay=True)
        end_warm_init(self.test)
        self.test.verifyAllPipes()
