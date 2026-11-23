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
import ptf.testutils as testutils
from p4testutils.misc_utils import *
from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.client as gc

dev_id = 0
p4_program_name = "tna_action_selector"

logger = get_logger()
swports = get_sw_ports()


class ActionProfileSelectorGetTest(BfRuntimeTest):
    """@brief Populate the selector table and the action profile, read
    the entries and verify that they are equal.
    """

    def setUp(self):
        client_id = 0
        BfRuntimeTest.setUp(self, client_id, p4_program_name)
        setup_random()

    def runTest(self):
        ig_port = swports[1]
        eg_ports = swports[2:]

        target = gc.Target(device_id=dev_id, pipe_id=0xffff)

        # Add the new member to the selection table.
        pkt = testutils.simple_tcp_packet()
        exp_pkt = pkt
        max_grp_size = 200

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get(p4_program_name)

        forward_table = bfrt_info.table_get("SwitchIngress.forward")
        sel_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector")
        action_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector_ap")

        num_entries = random.randint(5, 15)

        # Construct input for selector table
        # This list contains dictionaries for each entry
        # dict(grp_id -> dict(act_member -> mem_status) )
        mem_dict_dict = {}
        for j in range(num_entries):
            member_list_size = 5
            # temp list from the eg_ports tuple and shuffling it
            temp_eg_ports = list(eg_ports)
            random.shuffle(temp_eg_ports)
            # making members and member_statuses
            members = [x for x in temp_eg_ports[:member_list_size]]
            member_status = [True] * member_list_size
            # Making 2 of them as false
            member_status[0] = member_status[1] = False
            random.shuffle(member_status)
            mem_dict = {members[i]: member_status[i]
                        for i in range(0, len(members))}
            mem_dict_dict[j + 1] = mem_dict

        for port in eg_ports:
            # Create a new member for each port with the port number as the id.
            action_table.entry_add(
                target,
                [action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID', port)])],
                [action_table.make_data([gc.DataTuple('port', port)],
                                        'SwitchIngress.hit')])

        # Add the new member to the selection table.
        # Adding all members at the same time along with the max group size
        for grp_id, mem_dict in mem_dict_dict.items():
            sel_table.entry_add(
                target,
                [sel_table.make_key([
                    gc.KeyTuple('$SELECTOR_GROUP_ID', grp_id)])],
                [sel_table.make_data([
                    gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size),
                    gc.DataTuple('$ACTION_MEMBER_ID',
                                 int_arr_val=list(mem_dict.keys())),
                    gc.DataTuple('$ACTION_MEMBER_STATUS',
                                 bool_arr_val=list(mem_dict.values()))])])

        # Get test on selector table
        try:
            for grp_id, mem_dict in mem_dict_dict.items():
                logger.info("Checking Get for grp_id %d and member_status_list %s",
                            grp_id, str(mem_dict))
                resp = sel_table.entry_get(
                    target,
                    [sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                                     grp_id)])],
                    {"from_hw": False})
                data_dict = next(resp)[0].to_dict()

                # Create a dict out of the response recvd
                mem_dict_recv = {
                    data_dict["$ACTION_MEMBER_ID"][i]:
                        data_dict["$ACTION_MEMBER_STATUS"][i]
                    for i in range(0, len(data_dict["$ACTION_MEMBER_ID"]))}
                # Both the dictionaries should be equal
                assert mem_dict == mem_dict_recv
        finally:
            # Clearing Selector Table
            logger.info("Clearing selector table")
            sel_table.entry_del(target)

            assert next(sel_table.usage_get(target, flags={'from_hw':False})) == 0, \
                "usage = %s expected = 0" % (sel_table.usage_get)
            for data, key in sel_table.entry_get(target):
                assert 0, "Not expecting any entries here"

            # Clearing Action profile members
            logger.info("Clearing action table")
            action_table.entry_del(target)

            assert next(action_table.usage_get(target, flags={'from_hw':False})) == 0, \
                "usage = %s expected = 0" % (action_table.usage_get)
            for data, key in action_table.entry_get(target):
                assert 0, "Not expecting any entries here"


class ActionProfileSelectorTest(BfRuntimeTest):
    """@brief Basic test: populate the forward table, the selector table, and
    the action profile. Send a packet and verify it is received at one of the
    ports selected by the selector table.
    """

    def setUp(self):
        client_id = 0
        BfRuntimeTest.setUp(self, client_id, p4_program_name)
        setup_random()

    def runTest(self):
        ig_port = swports[1]
        eg_ports = swports[2:6]
        group_id = 1

        target = gc.Target(device_id=dev_id, pipe_id=0xffff)

        # Add the new member to the selection table.
        pkt = testutils.simple_tcp_packet()
        exp_pkt = pkt
        max_grp_size = 200

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get(p4_program_name)

        forward_table = bfrt_info.table_get("SwitchIngress.forward")
        action_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector_ap")
        sel_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector")

        members = []
        member_status = [False] * len(eg_ports)
        exp_ports = []
        # Disable 2 of the ports
        disable = random.sample(list(range(len(eg_ports))), 2)
        for i, port in enumerate(eg_ports):
            # Create a new member for each port with the port number as the id.
            action_table.entry_add(
                target,
                [action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID',
                                                    port)])],
                [action_table.make_data([gc.DataTuple('port', port)],
                                        'SwitchIngress.hit')])

            members.append(port)
            if i not in disable:
                member_status[i] = True
                exp_ports.append(port)

        # Create a dictionary with the member_status and members
        mem_dict = {members[i]: member_status[i] for i in range(0, len(members))}

        # Add the new member to the selection table.
        # Adding all members at the same time along with the max group size
        sel_table.entry_add(
            target,
            [sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', group_id)])],
            [sel_table.make_data([gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size),
                                  gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=members),
                                  gc.DataTuple('$ACTION_MEMBER_STATUS',
                                               bool_arr_val=member_status)])])

        forward_table.entry_add(
            target,
            [forward_table.make_key([gc.KeyTuple('ig_intr_md.ingress_port',
                                                 ig_port)])],
            [forward_table.make_data([gc.DataTuple('$SELECTOR_GROUP_ID',
                                                   group_id)])])

        try:
            logger.info("Sending packet on port %d", ig_port)
            testutils.send_packet(self, ig_port, pkt)

            logger.info("Expecting packet on one of enabled ports %s", exp_ports)
            testutils.verify_any_packet_any_port(self, [exp_pkt], exp_ports)
        finally:
            forward_table.entry_del(
                target,
                [forward_table.make_key([gc.KeyTuple('ig_intr_md.ingress_port',
                                                     ig_port)])])

            # Delete Selector Table entry
            logger.info("Deleting Selector group id %d", group_id)
            sel_table.entry_del(
                target,
                [sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', group_id)])])

            # Delete Action profile members
            for port in eg_ports:
                logger.info("Deleting Action profile member id %d", port)
                action_table.entry_del(
                    target,
                    [action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID',
                                                        port)])])

        logger.info("Sending packet on port %d", ig_port)
        testutils.send_packet(self, ig_port, pkt)

        logger.info("Packet is expected to get dropped.")
        testutils.verify_no_other_packets(self)


class ActionProfileSelectorIteratorTest(BfRuntimeTest):
    """@brief Populate the action profile and the selector table, delete random
    entries from them and verify the remaining entries are the ones that we
    expect to be still there.
    """

    def setUp(self):
        client_id = 0
        BfRuntimeTest.setUp(self, client_id, p4_program_name)
        setup_random()

    def runTest(self):
        '''
        This test does the following:
        1. Adds certain number of action profile members
        2. Adds certain number of groups with random action profile members
           chosen, to the selector table (upto a max grp size)
        3. Deletes a random number of random groups.
        4. Does a get all from the selector table
        5. Verifies that the read data matches for each of the remaining groups
        '''
        target = gc.Target(device_id=dev_id, pipe_id=0xffff)

        # Add the new member to the selection table.
        max_grp_size = 7

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get(p4_program_name)

        forward_table = bfrt_info.table_get("SwitchIngress.forward")
        action_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector_ap")
        sel_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector")

        num_act_prof_entries = 1024
        num_sel_grps = 100

        egress_ports = [random.randint(1, 5) for x in range(num_act_prof_entries)]
        action_mbr_ids = [x for x in range(num_act_prof_entries)]
        sel_grp_ids = [x for x in range(num_sel_grps)]
        status = [True, False]
        num_mbrs_in_grps = [random.randint(1, 7) for x in range(num_sel_grps)]
        mbrs_in_grps = [(random.sample(action_mbr_ids, num_mbrs_in_grps[x]),
                         [status[random.randint(0, 1)]
                          for y in range(num_mbrs_in_grps[x])])
                        for x in range(num_sel_grps)]

        # Construct input for selector table
        # This list contains dictionaries for each entry
        # dict(grp_id -> dict(act_member -> mem_status) )
        mem_dict_dict = {}
        for j in range(num_sel_grps):
            members, member_status = mbrs_in_grps[j]
            mem_dict = {members[i]: member_status[i]
                        for i in range(0, len(members))}
            mem_dict_dict[sel_grp_ids[j]] = mem_dict

        logger.info("Inserting %d entries to action profile table",
                    num_act_prof_entries)
        for j in range(num_act_prof_entries):
            # Create a new member for each port with the port number as the id.
            action_table.entry_add(
                target,
                [action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID',
                                                    action_mbr_ids[j])])],
                [action_table.make_data([gc.DataTuple('port', egress_ports[j])],
                                        'SwitchIngress.hit')])

        logger.info("DONE Inserting %d entries to action profile table",
                    num_act_prof_entries)

        # Add the new member to the selection table.
        # Adding all members at the same time along with the max group size
        logger.info("Inserting %d groups to selector table", num_sel_grps)
        for grp_id, mem_dict in mem_dict_dict.items():
            sel_table.entry_add(
                target,
                [sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                                 grp_id)])],
                [sel_table.make_data([gc.DataTuple('$MAX_GROUP_SIZE',
                                                   max_grp_size),
                                      gc.DataTuple('$ACTION_MEMBER_ID',
                                                   int_arr_val=list(mem_dict.keys())),
                                      gc.DataTuple('$ACTION_MEMBER_STATUS',
                                                   bool_arr_val=list(mem_dict.values()))])])

        logger.info("DONE Inserting %d groups to selector table", num_sel_grps)

        # Get test on selector table
        # remove random number of groups from the existing groups
        grps_removed = random.sample(sel_grp_ids,
                                     random.randint(1, num_sel_grps // 4))

        logger.info("Deleting %d groups from selector table", len(grps_removed))
        for grp_id in grps_removed:
            logger.info("Deleting Selector group id %d", grp_id)
            sel_table.entry_del(
                target,
                [sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', grp_id)])])

        logger.info("DONE Deleting %d groups from selector table",
                    len(grps_removed))

        remaining_grps = [x for x in sel_grp_ids if x not in grps_removed]
        sel_grp_ids = remaining_grps[:]

        logger.info("Getting all %d remaining groups from the selector table",
                    len(remaining_grps))
        # This call will cause a BRI error on switchd, which can be ignored
        resp = sel_table.entry_get(
            target,
            None,
            {"from_hw": False})

        logger.info("DONE Getting all %d remaining groups from the selector table",
                    len(remaining_grps))

        logger.info("Verifying all %d remaining groups from the selector table",
                    len(remaining_grps))
        for data, key in resp:
            data_dict = data.to_dict()
            key_dict = key.to_dict()
            recv_grp_id = key_dict["$SELECTOR_GROUP_ID"]['value']
            assert recv_grp_id in remaining_grps
            remaining_grps.remove(recv_grp_id)

            mem_dict_recv = {
                data_dict["$ACTION_MEMBER_ID"][i]:
                    data_dict["$ACTION_MEMBER_STATUS"][i]
                for i in range(0, len(data_dict["$ACTION_MEMBER_ID"]))}
            # Both the dictionaries should be equal
            assert mem_dict_dict[recv_grp_id] == mem_dict_recv
            logger.info("Selector group id %d matched", recv_grp_id)

        assert len(remaining_grps) == 0
        logger.info("All %d remaining groups matched", len(remaining_grps))

        logger.info("Deleting %d remaining groups", len(remaining_grps))

        # Delete Selector Table entry
        for grp_id in sel_grp_ids:
            logger.info("Deleting Selector group id %d", grp_id)
            sel_table.entry_del(
                target,
                [sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                                 grp_id)])])

        logger.info("DONE Deleting %d remaining groups", len(remaining_grps))

        logger.info("Deleting %d action profile entries", num_act_prof_entries)
        # Delete Action profile members
        for j in range(num_act_prof_entries):
            logger.info("Deleting Action profile member id %d",
                        action_mbr_ids[j])
            action_table.entry_del(
                target,
                [action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID',
                                                    action_mbr_ids[j])])])

        logger.info("DONE Deleting %d action profile entries",
                    num_act_prof_entries)


@testutils.group("thorough")
class ActionProfileSelectorTestMany(BfRuntimeTest):
    """@brief Fill the selector table, randomly shuffle its content and send
    100 randomized IPv4 packets to be processed by the table. Verify the the
    resulting behavior is as expected.
    """

    def setUp(self):
        client_id = 0
        BfRuntimeTest.setUp(self, client_id, p4_program_name)

        setup_random()

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_action_selector")

        self.forward_table = bfrt_info.table_get("SwitchIngress.forward")
        self.action_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector_ap")
        self.sel_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector")
        self.set_dest_table = bfrt_info.table_get("SwitchIngress.set_dest")

        self.target = gc.Target(device_id=0, pipe_id=0xffff)

        self.dev_cfg_table = bfrt_info.table_get("device_configuration")


    def modify_grp(self, sel_table, members, member_status):
        target = gc.Target(device_id=dev_id, pipe_id=0xffff)

        # Modify group
        sel_table.entry_mod(
            target,
            [sel_table.make_key(
                [gc.KeyTuple('$SELECTOR_GROUP_ID', self.group_id)])],
            [sel_table.make_data(
                [gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=members),
                 gc.DataTuple('$ACTION_MEMBER_STATUS',
                              bool_arr_val=member_status)])
             ])

        # Verify
        member_dict = {members[i]: member_status[i]
                       for i in range(len(members))}
        get_resp = sel_table.entry_get(
            target,
            [sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                             self.group_id)])],
            {"from_hw": False})
        data_dict = next(get_resp)[0].to_dict()
        member_dict_recv = {
            data_dict["$ACTION_MEMBER_ID"][i]:
                data_dict["$ACTION_MEMBER_STATUS"][i]
            for i in range(len(data_dict["$ACTION_MEMBER_ID"]))}
        assert member_dict == member_dict_recv

    def runBaseTest(self):
        self.group_id = 1
        ig_port = swports[1]
        eg_ports = swports[2:-1]
        num_ports = len(eg_ports)
        target = gc.Target(device_id=dev_id, pipe_id=0xffff)


        act_prof_size = 2048
        max_grp_size = 200
        # Prepare action profile table by filling with members
        logger.info("Filling action profile table")
        for i in range(1, act_prof_size + 1):
            self.action_table.entry_add(
                target,
                [self.action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID', i)])],
                [self.action_table.make_data([gc.DataTuple('idx', i)],
                                        'SwitchIngress.set_md')])

            self.set_dest_table.entry_add(
                target,
                [self.set_dest_table.make_key([gc.KeyTuple('ig_md.md', i)])],
                [self.set_dest_table.make_data([gc.DataTuple('port',
                                                        eg_ports[i % num_ports])],
                                          'SwitchIngress.hit')])

        # Perform some preliminary member set operations. They should all succeed
        # since there are no match entries pointing to the group.

        # Fill up the selector group
        logger.info("Churning selector group without match ref")
        members = random.sample(range(1, act_prof_size + 1), max_grp_size)
        member_status = [True] * max_grp_size
        self.sel_table.entry_add(
            target,
            [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', self.group_id)])],
            [self.sel_table.make_data([gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size),
                                  gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=members),
                                  gc.DataTuple('$ACTION_MEMBER_STATUS', bool_arr_val=member_status)])])

        # Disable all members
        member_status = [False] * max_grp_size
        self.modify_grp(self.sel_table, members, member_status)

        # Perform a few churns
        for _ in range(10):
            num_mbrs = random.randint(1, max_grp_size)
            members = random.sample(range(1, act_prof_size + 1), num_mbrs)
            member_status = [random.choice([False, True]) for _ in range(num_mbrs)]
            self.modify_grp(self.sel_table, members, member_status)

        # Clear the group
        members = []
        member_status = []
        self.modify_grp(self.sel_table, members, member_status)

        logger.info("Try to add a match entry to refer to empty group. " +
                    "This should fail, i.e. we expect an gRPC error to be displayed:")
        try:
            self.forward_table.entry_add(
                target,
                [self.forward_table.make_key([gc.KeyTuple('ig_intr_md.ingress_port',
                                                     ig_port)])],
                [self.forward_table.make_data([gc.DataTuple('$SELECTOR_GROUP_ID',
                                                       self.group_id)])])
        except:
            pass

        usage = next(self.forward_table.usage_get(target))
        assert usage == 0
        logger.info("    Test successful")

        # Add a member so the match entry can be attached
        members = [1]
        member_status = [True]
        self.modify_grp(self.sel_table, members, member_status)

        logger.info("Adding match entry")
        self.forward_table.entry_add(
            target,
            [self.forward_table.make_key([gc.KeyTuple('ig_intr_md.ingress_port',
                                                 ig_port)])],
            [self.forward_table.make_data([gc.DataTuple('$SELECTOR_GROUP_ID',
                                                   self.group_id)])])

        # Verify
        pkt = testutils.simple_tcp_packet()
        testutils.send_packet(self, ig_port, pkt)
        testutils.verify_any_packet_any_port(self, [pkt], [eg_ports[1]])

        # Perform some membership churn. Packets should come out only on
        # active members.
        for i in range(10):
            logger.info("Preparing group for churn iter %d", i)
            num_mbrs = random.randint(1, max_grp_size)
            members = random.sample(range(1, act_prof_size + 1), num_mbrs)
            member_status = [random.choice([False, True])
                             for _ in range(num_mbrs)]
            # If all members are disabled by chance, ensure the API will fail,
            # then enable the first and move on.
            if True not in member_status:
                api_failed = False
                try:
                    self.sel_table.entry_mod(
                        target,
                        [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                                         self.group_id)])],
                        [self.sel_table.make_data([gc.DataTuple('$ACTION_MEMBER_ID',
                                                           int_arr_val=members),
                                              gc.DataTuple('$ACTION_MEMBER_STATUS',
                                                           bool_arr_val=member_status)])])
                except:
                    api_failed = True
                    pass
                assert api_failed
                member_status[0] = True

            self.modify_grp(self.sel_table, members, member_status)

            logger.info("Sending some packets for churn iter %d", i)
            active_ports = [eg_ports[members[idx] % num_ports]
                            for idx, status in enumerate(member_status) if status]

            for _ in range(100):
                eth_src = '%x:%x:%x:%x:%x:%x' % (
                    random.randint(0, 255), random.randint(0, 255),
                    random.randint(0, 255), random.randint(0, 255),
                    random.randint(0, 255), random.randint(0, 255)
                )
                eth_dst = '%x:%x:%x:%x:%x:%x' % (
                    random.randint(0, 255), random.randint(0, 255),
                    random.randint(0, 255), random.randint(0, 255),
                    random.randint(0, 255), random.randint(0, 255)
                )
                ip_src = '%d.%d.%d.%d' % (random.randint(0, 255),
                                          random.randint(0, 255),
                                          random.randint(0, 255),
                                          random.randint(0, 255))
                ip_dst = '%d.%d.%d.%d' % (random.randint(0, 255),
                                          random.randint(0, 255),
                                          random.randint(0, 255),
                                          random.randint(0, 255))
                pkt = testutils.simple_tcp_packet(eth_src=eth_src,
                                                  eth_dst=eth_dst,
                                                  ip_src=ip_src,
                                                  ip_dst=ip_dst)
                testutils.send_packet(self, ig_port, pkt)
                testutils.verify_any_packet_any_port(self, [pkt], active_ports)

        # Try to empty or delete the group. Should fail since there is a match
        # reference
        logger.info("Try to empty or delete a group with active references. " +
                    "This should fail, i.e. we expect an gRPC error to be displayed:")
        try:
            members = []
            member_status = []
            self.sel_table.entry_mod(
                target,
                [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                                 self.group_id)])],
                [self.sel_table.make_data([gc.DataTuple('$ACTION_MEMBER_ID',
                                                   int_arr_val=members),
                                      gc.DataTuple('$ACTION_MEMBER_STATUS',
                                                   bool_arr_val=member_status)])])
        except:
            pass

        get_resp = self.sel_table.entry_get(
            target,
            [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                             self.group_id)])],
            {"from_hw": False})
        data_dict = next(get_resp)[0].to_dict()
        assert len(data_dict["$ACTION_MEMBER_ID"]) > 0

        try:
            self.sel_table.entry_del(
                target,
                [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                                 self.group_id)])])
        except:
            pass
        usage = next(self.sel_table.usage_get(target))
        assert usage == 1

        # Delete match entry
        logger.info("Deleting match entry")
        self.forward_table.entry_del(
            target,
            [self.forward_table.make_key([gc.KeyTuple('ig_intr_md.ingress_port',
                                                 ig_port)])])

        # Delete Selector Table entry
        logger.info("Deleting Selector group id %d", self.group_id)
        self.sel_table.entry_del(
            target,
            [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                             self.group_id)])])

        # Delete Action profile members
        logger.info("Deleting action profile members")
        for i in range(1, act_prof_size + 1):
            self.action_table.entry_del(
                target,
                [self.action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID', i)])])
            self.set_dest_table.entry_del(
                target,
                [self.set_dest_table.make_key([gc.KeyTuple('ig_md.md', i)])])

        logger.info("Sending packet on port %d", ig_port)
        testutils.send_packet(self, ig_port, pkt)

        logger.info("Packet is expected to get dropped.")
        testutils.verify_no_other_packets(self)

    def runTest(self):
        self.runBaseTest()
        logger.info("Rerun enabling sequence order")
        selector_member_order = True
        dev_data = self.dev_cfg_table.make_data([
                      gc.DataTuple('selector_member_order',
                      bool_val=selector_member_order)])
        self.dev_cfg_table.default_entry_set(self.target, dev_data)
        self.runBaseTest()

    def tearDown(self):
        logger.info("Deleting match entry")
        self.forward_table.entry_del(self.target)

        # Delete Selector Table entry
        logger.info("Deleting Selector group")
        self.sel_table.entry_del(self.target)

        # Delete Action profile members
        logger.info("Deleting Action profile members")
        self.action_table.entry_del(self.target)

        self.dev_cfg_table.default_entry_reset(self.target)

        BfRuntimeTest.tearDown(self)

class ActionProfileWeightedSelectorGetTest(BfRuntimeTest):
    """@brief Populate the action profile and the duplicate entries of selector
    table, read the entries and verify that they are equal.
    """

    def setUp(self):
        client_id = 0
        BfRuntimeTest.setUp(self, client_id, p4_program_name)
        setup_random()

        bfrt_info = self.interface.bfrt_info_get(p4_program_name)
        # Get bfrt_info and set it as part of the test

        self.sel_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector")
        self.action_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector_ap")
        self.dev_cfg_table = bfrt_info.table_get("device_configuration")


    def runBaseTest(self, selector_member_order = False):
        ig_port = swports[1]
        eg_ports = swports[2:]

        self.target = gc.Target(device_id=dev_id, pipe_id=0xffff)

        # Add the new member to the selection table.
        pkt = testutils.simple_tcp_packet()
        exp_pkt = pkt
        max_grp_size = 200

        num_entries = random.randint(20, 30)

        # Construct input for selector table
        # This list contains dictionaries for each entry
        # dict(grp_id -> dict(act_member -> mem_status) )
        logger.info("Initial members and member_status")
        mbr_dict = {}
        for j in range(num_entries):
            member_list_size = len(eg_ports)
            # temp list from the eg_ports tuple and shuffling it
            temp_eg_ports = list(eg_ports)
            random.shuffle(temp_eg_ports)
            # making members and member_statuses
            members = [x for x in temp_eg_ports[:member_list_size]]
            member_status = [True] * member_list_size
            # Making 2 of them as false
            member_status[0] = member_status[1] = False
            random.shuffle(member_status)

            # duplicate few member and status
            num_dup = random.randint(6, 10)
            for x in range(num_dup):
              pos = random.randint(0, member_list_size - 1)
              members.append(members[pos])
              member_status.append(member_status[pos])

            logger.info("group ID: %d", j)
            logger.info("    members : %s", str(members))
            logger.info("    member_status : %s", str(member_status))

            mbr_dict.update({j : [members, member_status]})

        for port in eg_ports:
            # Create a new member for each port with the port number as the id.
            self.action_table.entry_add(
                self.target,
                [self.action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID', port)])],
                [self.action_table.make_data([gc.DataTuple('port', port)],
                                        'SwitchIngress.hit')])
        # Add the new member to the selection table.
        # Adding all members at the same time along with the max group size

        for grp_id, mbr_details in mbr_dict.items():
          mbr_id = mbr_details[0]
          mbr_status = mbr_details[1]

          logger.info("Adding table entry : grp_id: %d", grp_id)
          self.sel_table.entry_add(
              self.target,
                [self.sel_table.make_key([
                    gc.KeyTuple('$SELECTOR_GROUP_ID', grp_id)])],
                [self.sel_table.make_data([
                    gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size),
                    gc.DataTuple('$ACTION_MEMBER_ID',
                                 int_arr_val=mbr_id),
                    gc.DataTuple('$ACTION_MEMBER_STATUS',
                                 bool_arr_val=mbr_status)])])

        # Get test on selector table

        for grp_id, mbr_details in mbr_dict.items():
          mbr_id = mbr_details[0]
          mbr_status = mbr_details[1]

          resp = self.sel_table.entry_get(
                self.target,
                [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID',
                                                 grp_id)])],
                {"from_hw": False})
          data_dict = next(resp)[0].to_dict()

          logger.info("Checking Get for grp_id %d and member_status_list %s",
                     grp_id, str(mbr_id))

          # Both the list should be equal
          if selector_member_order:
            assert mbr_id == data_dict["$ACTION_MEMBER_ID"]
          else:
            assert sorted(mbr_id) == sorted(data_dict["$ACTION_MEMBER_ID"])

        # Clearing Selector Table
        logger.info("Clearing selector table")
        self.sel_table.entry_del(self.target)

        # Clearing Action profile members
        logger.info("Clearing action table")
        self.action_table.entry_del(self.target)

    def runTest(self):
        self.runBaseTest()

        logger.info("Rerun enabling sequence order")
        selector_member_order = True
        dev_data = self.dev_cfg_table.make_data([
                      gc.DataTuple('selector_member_order',
                      bool_val=selector_member_order)])
        self.dev_cfg_table.default_entry_set(self.target, dev_data)
        self.runBaseTest(selector_member_order)

    def tearDown(self):

        # Clearing Selector Table
        logger.info("Clearing selector table")
        self.sel_table.entry_del(self.target)

        # Clearing Action profile members
        logger.info("Clearing action table")
        self.action_table.entry_del(self.target)

        self.dev_cfg_table.default_entry_reset(self.target)
        BfRuntimeTest.tearDown(self)


class ActionProfileWeightSelectorTest(BfRuntimeTest):
    """@brief Basic test: populate the forward table, the selector table, and
    the action profile. Send a packet and verify it is received at one of the
    ports selected by the selector table and compare the weight.
    """

    def setUp(self):
        client_id = 0
        BfRuntimeTest.setUp(self, client_id, p4_program_name)
        setup_random()

    def runTest(self):
        ig_port = swports[1]
        eg_ports = swports[2:5]
        group_id = 1

        self.target = gc.Target(device_id=dev_id, pipe_id=0xffff)

        # Add the new member to the selection table.
        pkt = testutils.simple_tcp_packet()
        exp_pkt = pkt
        max_grp_size = 30
        weight = [5, 10, 15]

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get(p4_program_name)

        self.forward_table = bfrt_info.table_get("SwitchIngress.forward")
        self.action_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector_ap")
        self.sel_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector")

        members = []
        member_status = [True] * max_grp_size
        exp_ports = []

        for port in range(0, len(eg_ports)):
            # Create a new member for each port with the port number as the id.
            self.action_table.entry_add(
                self.target,
                [self.action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID',
                                                    eg_ports[port])])],
                [self.action_table.make_data([gc.DataTuple('port', eg_ports[port])],
                                        'SwitchIngress.hit')])

            members += [eg_ports[port]] * weight[port]

        logger.info("members : %s", str(members))
        logger.info("member_status : %s", str(member_status))

        # Create a dictionary with the member_status and members
        mbr_dict = {members[i]: member_status[i] for i in range(0, len(members))}

        # Add the new member to the selection table.
        # Adding all members at the same time along with the max group size
        self.sel_table.entry_add(
            self.target,
            [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', group_id)])],
            [self.sel_table.make_data([gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size),
                                  gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=members),
                                  gc.DataTuple('$ACTION_MEMBER_STATUS',
                                               bool_arr_val=member_status)])])

        self.forward_table.entry_add(
            self.target,
            [self.forward_table.make_key([gc.KeyTuple('ig_intr_md.ingress_port',
                                                 ig_port)])],
            [self.forward_table.make_data([gc.DataTuple('$SELECTOR_GROUP_ID',
                                                   group_id)])])

        count = [0, 0, 0]
        max_itrs =  300
        logger.info("Sending %d packets on port %d", max_itrs, ig_port)
        logger.info("Expecting packets on one of enabled ports %s", eg_ports)
        for i in range(0, max_itrs):
          eth_src = '%x:%x:%x:%x:%x:%x' % (
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255))
          eth_dst = '%x:%x:%x:%x:%x:%x' % (
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255))
          ip_src = '%d.%d.%d.%d' % (random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255))
          ip_dst = '%d.%d.%d.%d' % (random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255))
          pkt = testutils.simple_tcp_packet(eth_src=eth_src,
                                              eth_dst=eth_dst,
                                              ip_src=ip_src,
                                              ip_dst=ip_dst)
          testutils.send_packet(self, ig_port, pkt)
          rcv_idx = testutils.verify_any_packet_any_port(self, [pkt], eg_ports)
          count[rcv_idx] += 1

        logger.info("Packet distribution : %s", str(count))
        logger.info("Ideal distribution : %s", str(weight))

    def tearDown(self):
        self.forward_table.entry_del(self.target)

        # Delete Selector Table entry
        logger.info("Deleting Selector group")
        self.sel_table.entry_del(self.target)

        # Delete Action profile members
        logger.info("Deleting Action profile members")
        self.action_table.entry_del(self.target)
        BfRuntimeTest.tearDown(self)

@testutils.group("thorough")
class SelectorRandomADTOffset(BfRuntimeTest):
    """@brief Basic adt offset test. Adds and removes selector entries at random, valid
    offsets. Sends traffic to check if all members work. Maximum tested size depends on
    the availablity of ports. Test sends 20 packets per member with assumption at least 1
    will egress on each port.
    Valid adt offset must have 0 bits in all LSB that cover values under selector size.
    """

    def setUp(self):
        client_id = 0
        BfRuntimeTest.setUp(self, client_id, p4_program_name)
        setup_random()

    def sendRandomPackets(self, ig_port, eg_ports, max_grp_size):
        pkt_num = max_grp_size * 20
        weight = [1] * max_grp_size
        count = [0] * len(weight)
        logger.info("   Sending %d packets on port %d", pkt_num, ig_port)
        logger.info("   Expecting packets on all of enabled ports %s", eg_ports)
        for i in range(0, pkt_num):
          eth_src = '%x:%x:%x:%x:%x:%x' % (
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255))
          eth_dst = '%x:%x:%x:%x:%x:%x' % (
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255))
          ip_src = '%d.%d.%d.%d' % (random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255))
          ip_dst = '%d.%d.%d.%d' % (random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255))
          pkt = testutils.simple_tcp_packet(eth_src=eth_src,
                                              eth_dst=eth_dst,
                                              ip_src=ip_src,
                                              ip_dst=ip_dst)
          testutils.send_packet(self, ig_port, pkt)
          rcv_idx = testutils.verify_any_packet_any_port(self, [pkt], eg_ports)
          count[rcv_idx] += 1

        logger.info("   Packet distribution : %s", str(count))
        res = []
        for i in range(len(weight)):
            res.append(int(pkt_num/len(weight) * weight[i]))
        logger.info("   Ideal distribution : %s", str(res))
        for c in count:
            assert c != 0, "Port did not send any packets"


    def runTest(self):
        ig_port = swports[0]
        eg_ports = swports[1:]
        group_id = 1

        self.target = gc.Target(device_id=dev_id, pipe_id=0xffff)

        # Add the new member to the selection table.
        pkt = testutils.simple_tcp_packet()
        exp_pkt = pkt

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get(p4_program_name)

        self.forward_table = bfrt_info.table_get("SwitchIngress.forward")
        self.action_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector_ap")
        self.sel_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector")

        if ("$ADT_OFFSET" not in self.sel_table.info.data_field_name_list_get()):
            logger.info("==================================================")
            logger.info("SKIPPING TEST AS ADT_OFFSET FIELD IS NOT AVAILABLE")
            logger.info("==================================================")
            return

        members = []
        member_status = [True] * len(eg_ports)
        exp_ports = []

        for port in range(0, len(eg_ports)):
            # Create a new member for each port with the port number as the id.
            self.action_table.entry_add(
                self.target,
                [self.action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID',
                                                    eg_ports[port])])],
                [self.action_table.make_data([gc.DataTuple('port', eg_ports[port])],
                                        'SwitchIngress.hit')])

            members += [eg_ports[port]]

        logger.info("members : %s", str(members))
        logger.info("member_status : %s", str(member_status))

        # Create a dictionary with the member_status and members
        mbr_dict = {members[i]: member_status[i] for i in range(0, len(members))}

        supported_grp_sizes = [];
        for grp_size in [2, 4, 8, 16, 32, 64]:
            if grp_size <= len(eg_ports):
                supported_grp_sizes.append(grp_size)

        for max_grp_size in supported_grp_sizes:
            logger.info("=============================================================")
            logger.info("Testing max_grp_size : %s", str(max_grp_size))
            for _ in range(5): # Few iterations to test different offsets
                adt_offset = random.randint(0, self.action_table.info.size)
                adt_offset &= ~(max_grp_size - 1)
                logger.info(" random adt_offset : %s", str(adt_offset))

                self.sel_table.entry_add(
                    self.target,
                    [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', group_id)])],
                    [self.sel_table.make_data([gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size),
                                          gc.DataTuple('$ADT_OFFSET', adt_offset),
                                          gc.DataTuple('$ACTION_MEMBER_ID', int_arr_val=members[0:max_grp_size]),
                                          gc.DataTuple('$ACTION_MEMBER_STATUS',
                                              bool_arr_val=member_status[0:max_grp_size])])])

                self.forward_table.entry_add(
                    self.target,
                    [self.forward_table.make_key([gc.KeyTuple('ig_intr_md.ingress_port',
                                                         ig_port)])],
                    [self.forward_table.make_data([gc.DataTuple('$SELECTOR_GROUP_ID',
                                                           group_id)])])

                self.sendRandomPackets(ig_port, eg_ports[:max_grp_size], max_grp_size);
                self.forward_table.entry_del(self.target, [])
                self.sel_table.entry_del(self.target, [])

    def tearDown(self):
        logger.info("Deleting Forwarding entry")
        self.forward_table.entry_del(self.target)

        logger.info("Deleting Selector group")
        self.sel_table.entry_del(self.target)

        logger.info("Deleting Action profile members")
        self.action_table.entry_del(self.target)
        BfRuntimeTest.tearDown(self)


class SelectorADTOffset(BfRuntimeTest):
    """@brief Basic adt offset test. Adds multiple selector entries to fill out the table
    and then updates forward table to check if all selectors work.
    Sends traffic to check if all members work. Maximum tested size depends on
    the availablity of ports. Test sends 20 packets per member with assumption at least 1
    will egress on each port.
    Valid adt offset must have 0 bits in all LSB that cover values under selector size.
    """

    def setUp(self):
        client_id = 0
        BfRuntimeTest.setUp(self, client_id, p4_program_name)
        setup_random()

    def sendRandomPackets(self, ig_port, eg_ports, max_grp_size):
        pkt_num = max_grp_size * 20
        weight = [1] * max_grp_size
        count = [0] * len(weight)
        logger.info("   Sending %d packets on port %d", pkt_num, ig_port)
        logger.info("   Expecting packets on all of enabled ports %s", eg_ports)
        for i in range(0, pkt_num):
          eth_src = '%x:%x:%x:%x:%x:%x' % (
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255))
          eth_dst = '%x:%x:%x:%x:%x:%x' % (
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255),
                random.randint(0, 255), random.randint(0, 255))
          ip_src = '%d.%d.%d.%d' % (random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255))
          ip_dst = '%d.%d.%d.%d' % (random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255),
                                      random.randint(0, 255))
          pkt = testutils.simple_tcp_packet(eth_src=eth_src,
                                              eth_dst=eth_dst,
                                              ip_src=ip_src,
                                              ip_dst=ip_dst)
          testutils.send_packet(self, ig_port, pkt)
          rcv_idx = testutils.verify_any_packet_any_port(self, [pkt], eg_ports)
          count[rcv_idx] += 1

        logger.info("   Packet distribution : %s", str(count))
        res = []
        for i in range(len(weight)):
            res.append(int(pkt_num/len(weight) * weight[i]))
        logger.info("   Ideal distribution : %s", str(res))
        for c in count:
            assert c != 0, "Port did not send any packets"


    def runTest(self):
        ig_port = swports[0]
        eg_ports = swports[1:]
        group_id = 1

        self.target = gc.Target(device_id=dev_id, pipe_id=0xffff)

        # Add the new member to the selection table.
        pkt = testutils.simple_tcp_packet()
        exp_pkt = pkt

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get(p4_program_name)

        self.forward_table = bfrt_info.table_get("SwitchIngress.forward")
        self.action_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector_ap")
        self.sel_table = bfrt_info.table_get(
            "SwitchIngress.example_action_selector")

        if ("$ADT_OFFSET" not in self.sel_table.info.data_field_name_list_get()):
            logger.info("==================================================")
            logger.info("SKIPPING TEST AS ADT_OFFSET FIELD IS NOT AVAILABLE")
            logger.info("==================================================")
            return

        members = []
        member_status = [True] * len(eg_ports)
        exp_ports = []

        for port in range(0, len(eg_ports)):
            # Create a new member for each port with the port number as the id.
            self.action_table.entry_add(
                self.target,
                [self.action_table.make_key([gc.KeyTuple('$ACTION_MEMBER_ID',
                                                    eg_ports[port])])],
                [self.action_table.make_data([gc.DataTuple('port', eg_ports[port])],
                                        'SwitchIngress.hit')])

            members += [eg_ports[port]]

        logger.info("members : %s", str(members))
        logger.info("member_status : %s", str(member_status))

        # Create a dictionary with the member_status and members
        mbr_dict = {members[i]: member_status[i] for i in range(0, len(members))}

        supported_grp_sizes = []
        for grp_size in [2, 4, 8, 16, 32, 64]:
            if grp_size <= len(eg_ports):
                supported_grp_sizes.append(grp_size)

        tested_groups = []
        offsets = []
        adt_offset = 0
        for _ in range(100):
            max_grp_size = random.choice(supported_grp_sizes)
            tested_groups.append(max_grp_size)
            if (adt_offset % max_grp_size):
                rest = adt_offset % max_grp_size
                adt_offset += max_grp_size - rest
            offsets.append(adt_offset)
            adt_offset += max_grp_size

        for grp_id in range(len(tested_groups)):
            max_grp_size = tested_groups[grp_id]
            adt_offset = offsets[grp_id]
            logger.info("Adding group %s offset %s size %s",
                    str(grp_id), str(adt_offset), str(max_grp_size))
            self.sel_table.entry_add(
                self.target,
                [self.sel_table.make_key([gc.KeyTuple('$SELECTOR_GROUP_ID', grp_id)])],
                [self.sel_table.make_data([gc.DataTuple('$MAX_GROUP_SIZE', max_grp_size),
                                      gc.DataTuple('$ADT_OFFSET', adt_offset),
                                      gc.DataTuple('$ACTION_MEMBER_ID',
                                          int_arr_val=members[0:max_grp_size]),
                                      gc.DataTuple('$ACTION_MEMBER_STATUS',
                                          bool_arr_val=member_status[0:max_grp_size])])])

        logger.info("=============================================================")
        for grp_id in range(len(tested_groups)):
            max_grp_size = tested_groups[grp_id]
            adt_offset = offsets[grp_id]
            logger.info("Testing group %s offset %s size %s",
                    str(grp_id), str(hex(adt_offset)), str(hex(max_grp_size)))

            self.forward_table.entry_add_or_mod(
                self.target,
                [self.forward_table.make_key([gc.KeyTuple('ig_intr_md.ingress_port',
                                                     ig_port)])],
                [self.forward_table.make_data([gc.DataTuple('$SELECTOR_GROUP_ID',
                                                       grp_id)])])

            self.sendRandomPackets(ig_port, eg_ports[:max_grp_size], max_grp_size);

        self.forward_table.entry_del(self.target, [])
        self.sel_table.entry_del(self.target, [])

    def tearDown(self):
        logger.info("Deleting Forwarding entry")
        self.forward_table.entry_del(self.target)

        logger.info("Deleting Selector group")
        self.sel_table.entry_del(self.target)

        logger.info("Deleting Action profile members")
        self.action_table.entry_del(self.target)
        BfRuntimeTest.tearDown(self)
