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
import random

from ptf import config
from ptf.thriftutils import *
import ptf.testutils as testutils
from p4testutils.misc_utils import *
from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as gc
import google.rpc.code_pb2 as code_pb2
from functools import partial

logger = logging.getLogger('Test')
swports = get_sw_ports()

if not len(logger.handlers):
    logger.addHandler(logging.StreamHandler())

@testutils.group("thorough")
class EntryGetErrorsTest(BfRuntimeTest):

    '''@brief This test is to verify handling of large number of errors in grpc requests
        1. Add entries and get entries with error_in_response set in metadata. Verify the read entries.
        2. Add few entries in table. Try to get entries with matching and not matching keys.
            Entries present should be fetched and errors should be reported for entries that are not found
            Verify that errors are reported for the missing entries in the correct order.
        3. Verify entry_get with high number of failures
    '''

    def setUp(self):
        client_id = 0
        p4_name = "bri_grpc_error"
        BfRuntimeTest.setUp(self, client_id, p4_name)

        self.bfrt_info = self.interface.bfrt_info_get("bri_grpc_error")
        self.forward_table = self.bfrt_info.table_get("Ingress.ipv4_host")
        self.forward_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.target = gc.Target(device_id=0, pipe_id=0xffff)
    def runTest(self):

        bfrt_info = self.bfrt_info
        target = self.target
        forward_table = self.forward_table
        key_list = []

        #client metadata field to indicate server to send status in response
        client_metadata = [("error_in_resp", "1")]

        ''' TC:1 Verify success case with status reported via response'''
        num_entries =  10
        num_valid_entries = num_entries

        # Populate key and data
        dip = ['%d.%d.%d.%d' % (random.randint(0,255), random.randint(0,255),
                                random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]

        for i in range(num_entries):
            key_list.append(forward_table.make_key([gc.KeyTuple('hdr.ipv4.dst_addr', dip[i])]))

        data_list = [forward_table.make_data([], 'Ingress.drop')]

        logger.info("Adding %d entries", num_entries)
        forward_table.entry_add(target, key_list, data_list*num_valid_entries)

        logger.info("Adding %d entries with errors_in_response in metadata", num_entries)
        resp = forward_table.entry_get(target,
                                       key_list, {"from_hw": False}, metadata=client_metadata)
        entries = []
        for i in range(num_entries):
            entries.append((key_list[i], (data_list*num_valid_entries)[i]))
        x = 0

        # Verify all entries are read
        for data, key in resp:
            try:
                entries.remove((key, data))
            except ValueError:
                assert False, "Invalid entry returned"

        logger.info("PASS: entries read same as entries added(all entries success case)")

        # Delete all the entries
        self.forward_table.entry_del(self.target)

        ''' TC:2 Verify errors reported via response are same as errors reported via grpc status
                 Verify that errors map correctly with the missing entries
        '''

        # Adding few entries in the table. num_valid_entires are randomly selected from num_entries
        num_valid_entries = random.randint(0, num_entries-1)
        entries_added_idx = [random.randint(0, num_entries-1) for i in range(num_valid_entries)]
        # remove duplicates if any
        entries_added_idx = list(set(entries_added_idx))
        num_valid_entries = len(entries_added_idx)

        # Indices of entries that are not added in the table.
        # Errors should be reported for these indices when entry_get is called for all the keys
        entries_missing_idx = [x for x in range(num_entries) if x not in entries_added_idx]
        logger.info("No. of entries added: %d", num_valid_entries)
        logger.info("Indices of entries added:")
        logger.info(entries_added_idx)

        key_list_add = []
        for idx in entries_added_idx:
            key_list_add.append(key_list[idx])

        forward_table.entry_add(target, key_list_add, data_list*num_valid_entries)

        logger.info("Get entries with errors reported via grpc status")
        num_read = 0
        resp = forward_table.entry_get(target,
                                       key_list, {"from_hw": False})

        # parse the response and get errors
        error_grpc_status = []
        self.parse_response(resp, error_grpc_status, num_entries, num_valid_entries)

        logger.info("Get entries with errors reported via response->errors")
        resp = forward_table.entry_get(target,
                                       key_list, {"from_hw": False}, metadata=client_metadata)

        # parse the response and store entries and errors received
        error_response = []
        self.parse_response(resp, error_response, num_entries, num_valid_entries)

        # compare the two results
        assert error_grpc_status == error_response, "errors received are not the same"
        logger.info("PASS: Entries and errors received are same via two methods")

        # verify that num of errors is equal to num of missing entries
        logger.info("Num of errors in response: %d", len(error_response))
        logger.info("Num of missing entries: %d", num_entries-num_valid_entries)
        assert len(error_response) == (num_entries-num_valid_entries), "errors received doesn't match missing entries"
        logger.info("PASS: num of errors matched num of missing entries")

        # verify that errors map with entries
        error_indices = []
        for error in error_response:
            error_indices.append(error[0])

        # Indices of entries that are not added in the table.
        # Errors should be reported for these indices when entry_get is called for all the keys
        entries_missing_idx = [x for x in range(num_entries) if x not in entries_added_idx]

        logger.info("Num of errors received in response: %d", len(error_indices))
        logger.info("Indices of missing entries received in response:")
        logger.info(error_indices)

        logger.info("Num of missing entries: %d", len(entries_missing_idx))
        logger.info("Indices of missing entries:")
        logger.info(entries_missing_idx)

        assert entries_missing_idx == error_indices, "errors received didnt match missing entries"
        logger.info("PASS: Errors mapped correctly with missing entries")

        key_list.clear()
        dip.clear()
        error_response.clear()
        # Delete all the entries
        self.forward_table.entry_del(self.target)
        logger.info("All entries deleted")

        '''TC:3 Scale test'''

        num_entries = 2000000
        num_valid_entries = 0

        logger.info("Making %d keys, this takes time", num_entries)
        dip = ['%d.%d.%d.%d' % (random.randint(0,255), random.randint(0,255),
                                random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]

        for i in range(num_entries):
            key_list.append(forward_table.make_key([gc.KeyTuple('hdr.ipv4.dst_addr', dip[i])]))

        logger.info("Getting %d entries, this takes time", num_entries)
        resp = forward_table.entry_get(target,
                                       key_list, {"from_hw": False}, metadata=client_metadata)

        self.parse_response(resp, error_response, num_entries, num_valid_entries)
        assert len(error_response) == (num_entries-num_valid_entries), "errors received doesn't match missing entries"
        logger.info("PASS: Scale test of %d entries", num_entries)

    def tearDown(self):
        # delete all entries
        self.forward_table.entry_del(self.target)
        usage = next(self.forward_table.usage_get(self.target, [], flags={'from_hw':False}))
        assert usage == 0
        BfRuntimeTest.tearDown(self)

    def parse_response(self, resp, errors, num_entries, num_valid_entries):
        try:
            recieve_num = 0
            key_list_recv = []
            num_read = 0
            for _, key in resp:
                num_read += 1

            logger.info("Receive entries = %d", num_read)
            assert num_read == num_valid_entries

        except gc.BfruntimeRpcException as e:
            error_list = e.sub_errors_get()
            for error in error_list:
                errors.append(error)
            logger.info("Received errors = %d", len(error_list))
            logger.info("Expected error length = %d Received = %d", num_entries-num_valid_entries,
                        len(error_list))
#            assert len(error_list) == num_entries-num_valid_entries

class EntryAddErrorTest(BfRuntimeTest):

    '''@brief This test is to verify the error status during the WriteRequest in add operation
        1. Add entries entries with error_in_response set in metadata. Verify the added entries.
        2. Add the entries to the table. Try to duplicate entries at random indices.
           Verify that errors are reported for these inserted entries.
        3. Verify entry_add with high number of failures.
    '''
    def setUp(self):
        client_id = 0
        p4_name = "bri_grpc_error"
        BfRuntimeTest.setUp(self, client_id, p4_name)

        self.bfrt_info = self.interface.bfrt_info_get("bri_grpc_error")
        self.forward_table = self.bfrt_info.table_get("Ingress.ipv4_host")
        self.forward_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.target = gc.Target(device_id=0, pipe_id=0xffff)

    def runTest(self):
        bfrt_info = self.bfrt_info
        target = self.target
        forward_table = self.forward_table
        key_list = []

        #client metadata field to indicate server to send status in response
        client_metadata = [("error_in_resp", "1")]
        num_entries =  10
        num_valid_entries = num_entries

        # Populate key and data
        dip = ['%d.%d.%d.%d' % (random.randint(0,255), random.randint(0,255),
                                random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]
        for i in range(num_entries):
            key_list.append(forward_table.make_key([gc.KeyTuple('hdr.ipv4.dst_addr', dip[i])]))
        data_list = [forward_table.make_data([], 'Ingress.drop')]

        ''' TC:1 Backward compatibity test: entry_add api should work as before
            Add num_entires with error_in response set in metadata.
            Verify that entries are added correctly.
        '''
        logger.info("TC:1: Testing backward compatibilty")
        logger.info(
            "Adding %d entries with errors_in_response in metadata", num_entries)
        forward_table.entry_add(
            target, key_list, data_list*num_valid_entries, metadata=client_metadata)

        logger.info("Getting %d entries", num_entries)
        resp = forward_table.entry_get(target, key_list,
                                           {"from_hw": False})
        entries = []
        for i in range(num_entries):
            entries.append((key_list[i], (data_list*num_valid_entries)[i]))
        x = 0

        # Verify all entries are read
        for data, key in resp:
            try:
                entries.remove((key, data))
            except ValueError:
                assert False, "Invalid entry returned"

        assert len(entries) == 0, "All the entries are not read"
        logger.info("PASS: entries read same as entries added(all entries success case)")

        # Delete all the entries
        self.forward_table.entry_del(self.target)

        ''' TC:2: add entries with duplicates and verify the errors are reported correctly
            step 1: Add random of entries between 0 to 9
            step 2: Add all the entries from 0 to 9
            Errors should be reported for the entries that were already added in step 1
        '''
        logger.info("TC:2: Testing error entries")
        # Adding few random entries in the table.
        # num_entries_added are randomly selected from num_entries
        num_entries_added = random.randint(0, num_entries-1)
        entries_added_idx = [random.randint(
            0, num_entries-1) for i in range(num_entries_added)]
        # remove duplicates if any
        entries_added_idx = list(set(entries_added_idx))
        entries_added_idx.sort()
        num_entries_added = len(entries_added_idx)

        key_list_add = []
        for idx in entries_added_idx:
            key_list_add.append(key_list[idx])

        logger.info("Adding %d entries", num_entries_added)
        forward_table.entry_add(target, key_list_add, data_list*num_entries_added, metadata=client_metadata)

        # Now adding all the entries from 0 to num_entries.
        # Errors should be reported for the entries that were added above.
        logger.info("Adding %d entries", num_entries)
        error_response = []
        try:
            forward_table.entry_add(target, key_list, data_list*num_valid_entries, metadata=client_metadata)
        except gc.BfruntimeRpcException as e:
            self.parse_status(e, error_response, num_entries_added)

        # verify that errors map with entries
        error_indices = []
        for error in error_response:
            error_indices.append(error[0])

        logger.info("Indices of duplicate entries:")
        logger.info(entries_added_idx)

        logger.info("Entries for which errors are received")
        logger.info(error_indices)

        assert entries_added_idx == error_indices, "errors received didnt match missing entries"
        logger.info("PASS: Errors mapped correctly with duplicate entries")

        # Delete all the entries
        self.forward_table.entry_del(self.target)

        '''TC:3 Duplicate entries in one entry_add'''
        logger.info("TC:3 Duplicate entries in one entry_add")
        key_list_new = key_list
        key_list_new.extend(key_list_new)
        new_num_entries = num_entries*2
        logger.info("Adding %d entries", new_num_entries)
        error_response = []
        try:
            forward_table.entry_add(target, key_list, data_list*new_num_entries, metadata=client_metadata)
        except gc.BfruntimeRpcException as e:
            self.parse_status(e, error_response, num_entries)

        # Delete all the entries
        self.forward_table.entry_del(self.target)
        key_list.clear()

        '''TC:4 Scale test'''
        logger.info("TC:4 Scale test")
        num_entries = 10000
        num_valid_entries = num_entries

        logger.info("Making %d keys, this takes time", num_entries)
        dip = ['%d.%d.%d.%d' % (random.randint(0,255), random.randint(0,255),
                                random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]

        for i in range(num_entries):
            key_list.append(forward_table.make_key([gc.KeyTuple('hdr.ipv4.dst_addr', dip[i])]))

        logger.info("Adding %d entries", num_entries)
        try:
            forward_table.entry_add(target, key_list, data_list*num_valid_entries, metadata=client_metadata)
        except gc.BfruntimeRpcException as e:
            error_response = []
            self.parse_status(e, error_response, num_entries)
        logger.info("Adding same %d entries again", num_entries)
        try:
            add_resp = forward_table.entry_add(target, key_list, data_list*num_valid_entries, metadata=client_metadata)
        except gc.BfruntimeRpcException as e:
            error_response = []
            self.parse_status(e, error_response, num_entries)
        logger.info("PASS: Scale test of %d entries", num_entries)

        # Delete all the entries
        self.forward_table.entry_del(self.target)
        key_list.clear()

    def tearDown(self):
        # delete all entries
        self.forward_table.entry_del(self.target)
        usage = next(self.forward_table.usage_get(self.target, [], flags={'from_hw':False}))
        assert usage == 0
        BfRuntimeTest.tearDown(self)

    def parse_status(self, error_object, errors, num_expected_errors):
        error_list = error_object.sub_errors_get()
        for error in error_list:
            errors.append(error)
        logger.info("Received errors = %d, Expected errors = %d", len(error_list), num_expected_errors)
        assert len(error_list) == num_expected_errors, "Num of errors received num of expected errors"
        logger.info("PASS: Number of errors matched number of expected errors")

class EntryDelErrorTest(BfRuntimeTest):

    '''@brief This test is to verify the error status during WriteRequest
        1. Add entries entries with error_in_response set in metadata.
        1. Delete entries and verify the response.
        2. Delete entries which are not present.
    '''
    def setUp(self):
        client_id = 0
        p4_name = "bri_grpc_error"
        BfRuntimeTest.setUp(self, client_id, p4_name)

        self.bfrt_info = self.interface.bfrt_info_get("bri_grpc_error")
        self.forward_table = self.bfrt_info.table_get("Ingress.ipv4_host")
        self.forward_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.target = gc.Target(device_id=0, pipe_id=0xffff)

    def runTest(self):
        bfrt_info = self.bfrt_info
        target = self.target
        forward_table = self.forward_table
        key_list = []

        #client metadata field to indicate server to send status in response
        client_metadata = [("error_in_resp", "1")]

        num_entries =  10
        num_valid_entries = num_entries
        # Populate key and data
        dip = ['%d.%d.%d.%d' % (random.randint(0,255), random.randint(0,255),
                                random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]
        for i in range(num_entries):
            key_list.append(forward_table.make_key([gc.KeyTuple('hdr.ipv4.dst_addr', dip[i])]))

        data_list = [forward_table.make_data([], 'Ingress.drop')]

        ''' TC:1 Backward compatibity test: entry_del api should work as before
            Add num_entires with error_in response set in metadata.
            Verify that entries are added correctly.
        '''
        logger.info("TC:1: Testing backward compatibilty")
        logger.info(
            "Adding %d entries with errors_in_response in metadata", num_entries)
        forward_table.entry_add(
            target, key_list, data_list*num_valid_entries, metadata=client_metadata)

        logger.info("Deleting %d entries", num_entries)
        forward_table.entry_del(target, key_list, metadata=client_metadata)

        usage = next(forward_table.usage_get(target, [], flags={'from_hw':False}))
        assert usage == 0, "Not all entries are deleted"
        logger.info("PASS: All entries are deleted")

        ''' TC:2: Try to delete entries that are not present and verify the errors are reported correctly
            step 1: Add random of entries between 0 to 10
            step 2: Delete entries with all the keys
            Errors should be reported for the entries that were not added in step 1
        '''
        logger.info("TC:2: Testing error entries")
        # Adding few random entries in the table.
        # num_entries_added are randomly selected from num_entries
        num_entries_added = random.randint(0, num_entries-1)
        entries_added_idx = [random.randint(
            0, num_entries-1) for i in range(num_entries_added)]
        # remove duplicates if any
        entries_added_idx = list(set(entries_added_idx))
        entries_added_idx.sort()
        num_entries_added = len(entries_added_idx)

        key_list_add = []
        for idx in entries_added_idx:
            key_list_add.append(key_list[idx])

        # Indices of entries that are not added in the table.
        # Errors should be reported for these indices when entry_del is called for all the keys
        entries_missing_idx = [x for x in range(num_entries) if x not in entries_added_idx]

        logger.info("Adding %d entries", num_entries_added)
        forward_table.entry_add(target, key_list_add, data_list*num_entries_added)

        # Now adding all the entries from 0 to num_entries.
        # Errors should be reported for the entries that were added above.
        logger.info("Deleting %d entries", num_entries)
        error_response = []
        try:
            forward_table.entry_del(target, key_list, metadata=client_metadata)
        except gc.BfruntimeRpcException as e:
            self.parse_status(e, error_response, num_entries-num_entries_added)
        # verify that errors map with entries
        error_indices = []
        for error in error_response:
            error_indices.append(error[0])

        logger.info("Indices of added entries:")
        logger.info(entries_added_idx)

        logger.info("Indices of missing entries:")
        logger.info(entries_missing_idx)

        logger.info("Entries for which errors are received")
        logger.info(error_indices)

        assert entries_missing_idx == error_indices, "errors received didnt match missing entries"
        logger.info("PASS: Errors mapped correctly with duplicate entries")

        # Delete all the entries
        self.forward_table.entry_del(self.target)

    def tearDown(self):
        # delete all entries
        self.forward_table.entry_del(self.target)
        usage = next(self.forward_table.usage_get(self.target, [], flags={'from_hw':False}))
        assert usage == 0
        BfRuntimeTest.tearDown(self)

    def parse_status(self, error_object, errors, num_expected_errors):
        error_list = error_object.sub_errors_get()
        for error in error_list:
            errors.append(error)
        logger.info("Received errors = %d, Expected errors = %d", len(error_list), num_expected_errors)
        assert len(error_list) == num_expected_errors, "Num of errors received num of expected errors"
        logger.info("PASS: Number of errors matched number of expected errors")

class EntryAddOrModTest(BfRuntimeTest):

    '''@brief This test is to verify the error status during the WriteRequest in add_or_mod operation
        1. Add entries entries with error_in_response set in metadata. Verify the added entries.
        2. Add the entries to the table. Try to duplicate entries at random indices.
           Verify that errors are reported for these inserted entries.
        3. Verify entry_add_or_mod with high number of failures.
    '''
    def setUp(self):
        client_id = 0
        p4_name = "bri_grpc_error"
        BfRuntimeTest.setUp(self, client_id, p4_name)

        self.bfrt_info = self.interface.bfrt_info_get("bri_grpc_error")
        self.forward_table = self.bfrt_info.table_get("Ingress.ipv4_host")
        self.forward_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.target = gc.Target(device_id=0, pipe_id=0xffff)

    def runTest(self):
        bfrt_info = self.bfrt_info
        target = self.target
        forward_table = self.forward_table
        key_list = []

        #client metadata field to indicate server to send status in response
        client_metadata = [("error_in_resp", "1"), ("ignore_not_found", "1")]
        num_entries =  10
        num_valid_entries = num_entries

        # Populate key and data
        dip = ['%d.%d.%d.%d' % (random.randint(0,255), random.randint(0,255),
                                random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]
        for i in range(num_entries):
            key_list.append(forward_table.make_key([gc.KeyTuple('hdr.ipv4.dst_addr', dip[i])]))
        data_list = [forward_table.make_data([], 'Ingress.drop')]

        ''' TC:1 Backward compatibity test: entry_add_or_mod api should work as before
            Add num_entires with error_in response and ignore_not_found set in metadata.
            Verify that entries are added and modified correctly.
        '''
        logger.info("TC:1: Testing backward compatibilty")
        logger.info("Adding %d entries with entry_add_or_mod", num_entries)
        forward_table.entry_add_or_mod(
            target, key_list, data_list*num_valid_entries, metadata=client_metadata)

        logger.info("Getting %d entries", num_entries)
        resp = forward_table.entry_get(target, key_list, {"from_hw": False})
        entries = []
        for i in range(num_entries):
            entries.append((key_list[i], (data_list*num_valid_entries)[i]))

        # Verify all entries are read
        for data, key in resp:
            try:
                entries.remove((key, data))
            except ValueError:
                assert False, "Invalid entry returned"

        assert len(entries) == 0, "All the entries are not read"
        logger.info("PASS: entries read same as entries added(all entries success case)")

        logger.info("Modifying %d entries with entry_add_or_mod", num_entries)
        new_data_list = [forward_table.make_data([], 'Ingress.send')]
        forward_table.entry_add_or_mod(
            target, key_list, new_data_list*num_valid_entries, metadata=client_metadata)

        logger.info("Getting %d entries", num_entries)
        resp = forward_table.entry_get(target, key_list, {"from_hw": False})
        exp_action = new_data_list[0].action_name
        for data, key in resp:
            assert data.action_name == exp_action, "Modifying entries with entry_add_or_mod failed"
        logger.info("PASS: entries were modified(all entries success case)")

        # Delete all the entries
        self.forward_table.entry_del(self.target)

        ''' TC:2: add entries with duplicates and verify the modify performed correctly
            step 1: Add random of entries between 0 to 9
            step 2: Add all the entries from 0 to 9
            Modify should be performed for the entries that were already added in step 1
        '''
        logger.info("TC:2: Testing random entries")
        # Adding few random entries in the table.
        # num_entries_added are randomly selected from num_entries
        num_entries_added = random.randint(0, num_entries-1)
        entries_added_idx = [random.randint(
            0, num_entries-1) for i in range(num_entries_added)]
        # remove duplicates if any
        entries_added_idx = list(set(entries_added_idx))
        entries_added_idx.sort()
        num_entries_added = len(entries_added_idx)

        key_list_add = []
        for idx in entries_added_idx:
            key_list_add.append(key_list[idx])

        logger.info("Adding %d entries", num_entries_added)
        forward_table.entry_add_or_mod(target, key_list_add, new_data_list*num_entries_added, metadata=client_metadata)

        # Now adding all the entries from 0 to num_entries.
        # Modify should be performed for the entries that were added above.
        logger.info("Adding %d entries", num_entries)
        error_response = []
        try:
            resp = forward_table.entry_add_or_mod(target, key_list, data_list*num_valid_entries, metadata=client_metadata)
        except gc.BfruntimeRpcException as e:
            self.parse_status(e, error_response, num_entries_added)

        # verify that errors map with entries
        error_indices = []
        idx = 0
        for sts in resp:
            if  sts.code == code_pb2.ALREADY_EXISTS:
                error_indices.append(idx)
            idx += 1

        resp = forward_table.entry_get(target, key_list, {"from_hw": False})
        exp_action = data_list[0].action_name
        for data, key in resp:
            assert data.action_name == exp_action, "Modifying entries with entry_add_or_mod failed"

        logger.info("Indices of duplicate entries:")
        logger.info(entries_added_idx)

        logger.info("Entries for which were modifieded")
        logger.info(error_indices)

        assert entries_added_idx == error_indices, "entries modified didnt match missing entries"
        logger.info("PASS: Modify performed correctly with duplicate entries")

        # Delete all the entries
        self.forward_table.entry_del(self.target)

        '''TC:3 Duplicate entries in one entry_add_or_mod'''
        logger.info("TC:3 Duplicate entries in one entry_add_or_mod")
        key_list_new = key_list
        key_list_new.extend(key_list_new)
        new_num_entries = num_entries*2
        logger.info("Adding %d entries", new_num_entries)
        resp = forward_table.entry_add_or_mod(target, key_list, data_list*new_num_entries, metadata=client_metadata)
        num_modified = 0
        for sts in resp:
            if  sts.code == code_pb2.ALREADY_EXISTS:
                num_modified += 1
        assert num_entries == num_modified

        # Delete all the entries
        self.forward_table.entry_del(self.target)
        key_list.clear()

        '''TC:4 Scale test'''
        logger.info("TC:4 Scale test")
        num_entries = 10000
        num_valid_entries = num_entries

        logger.info("Making %d keys, this takes time", num_entries)
        dip = ['%d.%d.%d.%d' % (random.randint(0,255), random.randint(0,255),
                                random.randint(0,255), random.randint(0,255)) for x in range(num_entries)]

        for i in range(num_entries):
            key_list.append(forward_table.make_key([gc.KeyTuple('hdr.ipv4.dst_addr', dip[i])]))

        logger.info("Adding %d entries", num_entries)
        try:
            forward_table.entry_add(target, key_list, data_list*num_valid_entries, metadata=client_metadata)
        except gc.BfruntimeRpcException as e:
            error_response = []
            self.parse_status(e, error_response, num_entries)
        logger.info("Adding same %d entries with AddOrMod", num_entries)
        try:
            add_resp = forward_table.entry_add_or_mod(target, key_list, data_list*num_valid_entries, metadata=client_metadata)
            num_modified = 0
            for sts in add_resp:
                if  sts.code == code_pb2.ALREADY_EXISTS:
                    num_modified += 1
            assert num_entries == num_modified
        except gc.BfruntimeRpcException as e:
            error_response = []
            self.parse_status(e, error_response, num_entries)
        logger.info("PASS: Scale test of %d entries", num_entries)

        # Delete all the entries
        self.forward_table.entry_del(self.target)
        key_list.clear()

    def tearDown(self):
        # delete all entries
        self.forward_table.entry_del(self.target)
        usage = next(self.forward_table.usage_get(self.target, [], flags={'from_hw':False}))
        assert usage == 0
        BfRuntimeTest.tearDown(self)

    def parse_status(self, error_object, errors, num_expected_errors):
        error_list = error_object.sub_errors_get()
        for error in error_list:
            errors.append(error)
        logger.info("Received errors = %d, Expected errors = %d", len(error_list), num_expected_errors)
        assert len(error_list) == num_expected_errors, "Num of errors received num of expected errors"
        logger.info("PASS: Number of errors matched number of expected errors")



class EntryDelIgnoreErrorTest(BfRuntimeTest):

    '''@brief Simple test to verify ignore_not_found flag working in transaction logic on
    entry delete along with checking errors in response.
    TC:1 checks errors in response when removing all existing and some non existing entries
         Every odd entry in the call uses non-existent key and should error out.
    TC:2 Remove few entries and some non-existing entries with TXN logic enabled.
         TXN should be applied and valid entries should get removed.
         Every odd entry in the call uses non-existent key and should error out.
    '''
    def setUp(self):
        client_id = 0
        p4_name = "bri_grpc_error"
        BfRuntimeTest.setUp(self, client_id, p4_name)

        self.target = gc.Target(device_id=0, pipe_id=0xffff)
        self.bfrt_info = self.interface.bfrt_info_get("bri_grpc_error")

        self.forward_table = self.bfrt_info.table_get("Ingress.ipv4_host")
        self.forward_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.port_table = self.bfrt_info.table_get("$PORT")
        self.sel_table = self.bfrt_info.table_get("Ingress.sel")
        self.ap_table = self.bfrt_info.table_get("Ingress.ap")

    def tearDown(self):
        # Clear all test tables
        self.forward_table.entry_del(self.target)
        self.port_table.entry_del(self.target)
        self.sel_table.entry_del(self.target)
        self.ap_table.entry_del(self.target)

        BfRuntimeTest.tearDown(self)


    def runTest(self):
        # client metadata field to indicate server to send status in response
        # Every odd key should error out
        client_metadata = [("error_in_resp", "1"), ("ignore_not_found", "1")]
        num_entries =  10
        num_valid_entries = num_entries

        logger.info("Testing Match-Action table with delete scenario and TXN")
        # Populate key and data
        dip = ['%d.%d.%d.%d' % (random.randint(0,255), random.randint(0,255),
                                random.randint(0,255), random.randint(0,255)) for x in range(num_entries * 2)]

        key_list = []
        del_key_list = []

        for i in range(num_entries):
            key_list.append(
                    self.forward_table.make_key([gc.KeyTuple('hdr.ipv4.dst_addr', dip[i])]))
        data_list = [self.forward_table.make_data([], 'Ingress.drop')]
        for i in range(num_entries):
            del_key_list.append(
                    self.forward_table.make_key(
                        [gc.KeyTuple('hdr.ipv4.dst_addr', dip[i + num_entries])]))

        self.tc1_table(self.forward_table, key_list, data_list, del_key_list)
        self.tc2_table(self.forward_table, key_list, data_list, del_key_list)

        logger.info("Testing Port-Config table with delete scenario and TXN")
        self.port_table.entry_del(self.target, [])
        key_list = []
        del_key_list = []
        for i in range(num_entries):
            key_list.append(self.port_table.make_key([gc.KeyTuple('$DEV_PORT', i)]))
        data_list = [self.port_table.make_data([gc.DataTuple('$SPEED', str_val="BF_SPEED_10G"),
                                    gc.DataTuple('$FEC', str_val="BF_FEC_TYP_NONE"),
                                    gc.DataTuple('$PORT_ENABLE', bool_val=True)])]
        for i in range(num_entries):
            del_key_list.append(
                self.port_table.make_key(
                    [gc.KeyTuple('$DEV_PORT', i + num_entries)]))

        self.tc1_table(self.port_table, key_list, data_list, del_key_list, err_code=0)
        self.tc2_table(self.port_table, key_list, data_list, del_key_list, err_code=0)


        logger.info("Testing Action table with delete scenario and TXN")
        key_list = []
        del_key_list = []
        for i in range(num_entries):
            key_list.append(self.ap_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID', i)]))
        data_list = [self.ap_table.make_data([], 'Ingress.drop')]
        for i in range(num_entries):
            del_key_list.append(
                self.ap_table.make_key(
                    [gc.KeyTuple('$ACTION_MEMBER_ID', i + num_entries)]))


        self.tc1_table(self.ap_table, key_list, data_list, del_key_list)
        self.tc2_table(self.ap_table, key_list, data_list, del_key_list)

        # Dont clear the table since it is needed for selector

        logger.info("Testing Selector table with delete scenario and TXN")
        key_list = []
        del_key_list = []
        for i in range(num_entries):
            key_list.append(self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', i)]))
        data_list = [self.sel_table.make_data([
            gc.DataTuple('$MAX_GROUP_SIZE', 8),
            # Action entries 0 and 1 will be non existing, hence start from 2
            gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=[2,3,4,5,6,7,8,9]),
            gc.DataTuple('$ACTION_MEMBER_STATUS', bool_arr_val=[True] * 8)])]

        for i in range(num_entries):
            del_key_list.append(
                self.sel_table.make_key(
                    [gc.KeyTuple('$SELECTOR_GROUP_ID', i + num_entries)]))

        self.tc1_table(self.sel_table, key_list, data_list, del_key_list)
        self.tc2_table(self.sel_table, key_list, data_list, del_key_list)

    def tc1_table(self, table, key_list, data_list, del_key_list, err_code=5):
        logger.info(" TC:1: Check if errors per entry are reported while GRPC_OK")

        client_metadata = [("error_in_resp", "1"), ("ignore_not_found", "1")]

        logger.info("  Adding %d entries", len(key_list))
        dlist = [data_list[0]] * len(key_list) if (len(data_list) != len(key_list)) else data_list
            
        table.entry_add(
            self.target, key_list, dlist, metadata=client_metadata)

        entries = key_list[:]
        self.check_entries(table, entries)

        # Create flat list of existing and non-existing keys
        key_list_to_del = []
        for x in zip(key_list, del_key_list):
            key_list_to_del += list(x)

        logger.info("  Removing %d entries with ignore_not_found = 1", len(key_list_to_del))
        resp = table.entry_del(
            self.target, key_list_to_del, metadata=client_metadata)
        self.check_errors(resp, err_code, [1,3,5,7,9])
        self.check_entries(table, [])


    def tc2_table(self, table, key_list, data_list, del_key_list, err_code=5):
        logger.info(" TC:2: Check if TXN is rolled back")

        client_metadata = [("error_in_resp", "1"), ("ignore_not_found", "1")]

        logger.info("  Adding entries")
        table.entry_add(
            self.target, key_list, data_list*len(key_list), metadata=client_metadata)

        entries = key_list[:]
        self.check_entries(table, entries)

        # Create flat list of existing and non-existing keys
        key_list_to_del = []
        for x in zip(key_list, del_key_list):
            key_list_to_del += list(x)

        logger.info("  Remove 2 entries and 2 fake entries with rollback and ignore_not_found")
        resp = table.entry_del(
                self.target, key_list_to_del[:4], bfruntime_pb2.WriteRequest.ROLLBACK_ON_ERROR,
                metadata=client_metadata)
        # Port table don't error out on non-existent entries
        self.check_errors(resp, err_code, [1, 3])

        entries = key_list[2:]
        self.check_entries(table, entries)


    def check_entries(self, table, entries):
        logger.info("  Getting %d entries", len(entries))
        resp = table.entry_get(self.target, [], {"from_hw": False})

        # Verify all entries are read
        for _, key in resp:
            try:
                entries.remove(key)
            except ValueError:
                assert False, "Invalid entry returned"
        assert len(entries) == 0, "All the entries are not read"

    def check_errors(self, resp, err_code, expected_errors):
        i = 0;
        error = next(resp, None)
        while error != None:
            if (i in expected_errors):
                assert error.code == err_code, "Error code doesn't match for key {} exp {} got {}".format(i, err_code, error.code)
            error = next(resp, None)
            i += 1
        logger.info("  PASS: Reported errors matched expected errors")
