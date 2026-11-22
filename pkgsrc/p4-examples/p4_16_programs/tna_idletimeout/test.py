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
import time
import datetime
import random
from collections import namedtuple

import pdb

from ptf import config
import ptf.testutils as testutils
from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as client
from p4testutils.misc_utils import *
from p4testutils.bfrt_utils import *

p4_name     = "tna_idletimeout"
client_id   = 0
dev_id      = 0

logger = get_logger()
swports = get_sw_ports()

swports_0 = []
swports_1 = []
swports_2 = []
swports_3 = []

# the following method categorizes the ports in ports.json file as belonging to either of the pipes (0, 1, 2, 3)
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


class IdleTimeoutTest(BfRuntimeTest):
    """@brief Simple test of the basic idle timeout configuration parameters and
    their usage.
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runCustomTest(self):
        ig_port = swports[1]
        eg_port = swports[2]

        dmac = client.bytes_to_mac(client.to_bytes(random.randint(0, 2 ** 48 - 1), 6))
        smac = client.bytes_to_mac(client.to_bytes(random.randint(0, 2 ** 48 - 1), 6))
        smac_mask = 0xffffffffffff
        dip = client.bytes_to_ipv4(client.to_bytes(random.randint(0, 2 ** 32 - 1), 4))
        dip_mask = ((0xffffffff) << (32 - random.randint(0, 32))) & (0xffffffff)

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_table = bfrt_info.table_get("SwitchIngress.dmac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.src_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.port_table = bfrt_info.table_get("$PORT")
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))

        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target

        dmac_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                             predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
        # Set Idle Table attributes
        ttl_query_length = 1000
        dmac_table.attribute_idle_time_set(target,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)

        resp = dmac_table.attribute_get(target, "IdleTimeout")
        for d in resp:
            assert d["ttl_query_interval"] == ttl_query_length
            assert d["idle_table_mode"] == bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE
            assert d["enable"] == True

        pkt = testutils.simple_tcp_packet(eth_dst=dmac, ip_dst=dip, eth_src=smac)
        exp_pkt = pkt

        dmac_table.entry_add(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask),  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],  # priority req for ternary
            [dmac_table.make_data([client.DataTuple('port', eg_port),
                                   client.DataTuple('$ENTRY_TTL', 1000)],
                                  'SwitchIngress.hit')]
        )

        logger.info("Sending packet on port %d", ig_port)
        testutils.send_packet(self, ig_port, pkt)

        logger.info("Expecting packet on port %d", eg_port)
        testutils.verify_packet(self, exp_pkt, eg_port)

        time.sleep(2)

        # Check for timeout notification.
        idle_time = self.interface.idletime_notification_get()
        recv_key = bfrt_info.key_from_idletime_notification(idle_time)
        key_dict = recv_key.to_dict()
        recv_dmac = key_dict["hdr.ethernet.dst_addr"]["value"]
        recv_smac = key_dict["hdr.ethernet.src_addr"]["value"]
        recv_smac_mask = key_dict["hdr.ethernet.src_addr"]["mask"]
        recv_dip = key_dict["hdr.ipv4.dst_addr"]["value"]
        recv_dip_mask = key_dict["hdr.ipv4.dst_addr"]["mask"]

        if (dmac != recv_dmac):
            logger.error("Error! dmac = %s received dmac = %s", str(dmac), str(recv_dmac))
            assert 0

        if (smac != recv_smac):
            logger.error("Error! smac = %s received smac = %s", str(smac), str(recv_smac))
            assert 0

        dip_mask_hex = client.to_bytes(dip_mask, 4)
        dip_hex = client.ipv4_to_bytes(dip)
        recv_dip_hex = client.ipv4_to_bytes(recv_dip)
        for i in range(4):
            dip_hex[i] = dip_hex[i] & dip_mask_hex[i]
        if (dip_hex != recv_dip_hex):
            logger.error("Error! dip = %s received dip = %s", str(dip), str(recv_dip))
            assert 0

        logger.info("Deleting entry from table")
        dmac_table.entry_del(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask),  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])])  # priority req for ternary

        logger.info("Sending packet on port %d", ig_port)
        testutils.send_packet(self, ig_port, pkt)

        logger.info("Packet is expected to get dropped.")
        testutils.verify_no_other_packets(self)

        logger.info("Disable idle timeout on the table")
        dmac_table.attribute_idle_time_set(target,
                                           False,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)
        default_interval = 5000
        resp = dmac_table.attribute_get(target, "IdleTimeout")
        for d in resp:
            assert d["ttl_query_interval"] == default_interval
            assert d["idle_table_mode"] == bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE
            assert d["enable"] == False

    def runTest(self):
        self.runCustomTest()

class Idle2WayNotifications(BfRuntimeTest):
    """@brief Simple test that showcases 2 way notification, when client gets notified
        on both entry state changes from active to idle and vice-versa.
    """

    def setUp(self):
        # Only enable idle table notifications, enable_entry_active notification is disabled
        # by default
        notifications = client.Notifications(enable_idletimeout=True, enable_entry_active=True, enable_port_status_change=False, enable_learn=False)
        BfRuntimeTest.setUp(self, client_id, p4_name, notifications=notifications)
        setup_random()

    def verify_idle_notification(self, dmac, smac, smac_mask, dip, dip_mask, is_active):
        logger.info("Verifying entry %s notification", "active" if is_active else "idle")
        idle_time = self.interface.idletime_notification_get()

        # Check notification type
        if is_active:
            assert(idle_time.type == bfruntime_pb2.IdleTimeoutNotification.ENTRY_ACTIVE)
        else:
            assert(idle_time.type == bfruntime_pb2.IdleTimeoutNotification.ENTRY_IDLE)

        # Verify notification entry fields
        recv_key = self.bfrt_info.key_from_idletime_notification(idle_time)
        key_dict = recv_key.to_dict()
        recv_dmac = key_dict["hdr.ethernet.dst_addr"]["value"]
        recv_smac = key_dict["hdr.ethernet.src_addr"]["value"]
        recv_smac_mask = key_dict["hdr.ethernet.src_addr"]["mask"]
        recv_dip = key_dict["hdr.ipv4.dst_addr"]["value"]
        recv_dip_mask = key_dict["hdr.ipv4.dst_addr"]["mask"]
        if (dmac != recv_dmac):
            logger.error("Error! dmac = %s received dmac = %s", str(dmac), str(recv_dmac))
            assert 0

        if (smac != recv_smac):
            logger.error("Error! smac = %s received smac = %s", str(smac), str(recv_smac))
            assert 0

        dip_mask_hex = client.to_bytes(dip_mask, 4)
        dip_hex = client.ipv4_to_bytes(dip)
        recv_dip_hex = client.ipv4_to_bytes(recv_dip)
        for i in range(4):
            dip_hex[i] = dip_hex[i] & dip_mask_hex[i]
        if (dip_hex != recv_dip_hex):
            logger.error("Error! dip = %s received dip = %s", str(dip), str(recv_dip))
            assert 0

    def tearDown(self):
        logger.info("Deleting all entries from table")
        self.dmac_table.entry_del(self.target, [])

        logger.info("Disable idle timeout on the table")
        self.dmac_table.attribute_idle_time_set(self.target,
                                           False,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)
        BfRuntimeTest.tearDown(self)

    def runCustomTest(self):
        ig_port = swports[1]
        eg_port = swports[2]

        dmac = client.bytes_to_mac(client.to_bytes(random.randint(0, 2 ** 48 - 1), 6))
        smac = client.bytes_to_mac(client.to_bytes(random.randint(0, 2 ** 48 - 1), 6))
        smac_mask = 0xffffffffffff
        dip = client.bytes_to_ipv4(client.to_bytes(random.randint(0, 2 ** 32 - 1), 4))
        dip_mask = ((0xffffffff) << (32 - random.randint(0, 32))) & (0xffffffff)

        # Get bfrt_info and set it as part of the test
        self.bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        self.dmac_table = self.bfrt_info.table_get("SwitchIngress.dmac")
        self.dmac_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        self.dmac_table.info.key_field_annotation_add("hdr.ethernet.src_addr", "mac")
        self.dmac_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.port_table = self.bfrt_info.table_get("$PORT")

        self.target = client.Target(device_id=0, pipe_id=0xffff)

        self.dmac_table.attribute_entry_scope_set(self.target, predefined_pipe_scope=True,
                                             predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
        # Set Idle Table to notify mode
        ttl_query_length = 1000
        self.dmac_table.attribute_idle_time_set(self.target,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)

        # Add entry and wait for idle notification
        entry_ttl = 2000
        self.dmac_table.entry_add(
            self.target,
            [self.dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask),
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],
            [self.dmac_table.make_data([client.DataTuple('port', eg_port),
                                   client.DataTuple('$ENTRY_TTL', entry_ttl)],
                                  'SwitchIngress.hit')]
        )
        time.sleep(entry_ttl * 2 / 1000)
        self.verify_idle_notification(dmac, smac, smac_mask, dip, dip_mask, False)

        # Send pkt and verify active notification
        pkt = testutils.simple_tcp_packet(eth_dst=dmac, ip_dst=dip, eth_src=smac)
        exp_pkt = pkt

        logger.info("Sending packet on port %d", ig_port)
        testutils.send_packet(self, ig_port, pkt)
        logger.info("Expecting packet on port %d", eg_port)
        testutils.verify_packet(self, exp_pkt, eg_port)
        # Wait a small amount of time for the model to generate the active
        # notification and for the driver to process that notification.
        time.sleep((entry_ttl / 1000) / 2)

        self.verify_idle_notification(dmac, smac, smac_mask, dip, dip_mask, True)

        # Wait again to get idle notification
        time.sleep(entry_ttl * 2 / 1000)
        self.verify_idle_notification(dmac, smac, smac_mask, dip, dip_mask, False)

    def runTest(self):
        self.runCustomTest()

class IdleTimeoutGetTTLTest(BfRuntimeTest):
    """@brief Show how to set the TTL of an entry and verify it is descreasing
    as expected.
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runCustomTest(self):
        eg_port = swports[2]
        smac = '12:34:45:67:89:ab'
        smac_mask =  0xffffffffffff
        dmac = '11:22:33:44:55:77'
        dip = '10.11.12.1'
        ttl_set = 10000

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_table = bfrt_info.table_get("SwitchIngress.dmac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.src_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.port_table = bfrt_info.table_get("$PORT")
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))

        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target
        # Set Idle Table attributes
        ttl_query_length = 1000
        dmac_table.attribute_idle_time_set(target,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)

        dmac_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                             predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)

        pkt = testutils.simple_tcp_packet(eth_dst=dmac, ip_dst=dip, eth_src=smac)
        exp_pkt = pkt

        dmac_table.entry_add(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, '255.255.0.0'),  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],  # priority req for ternary
            [dmac_table.make_data([client.DataTuple('port', eg_port),
                                   client.DataTuple('$ENTRY_TTL', ttl_set)],
                                  'SwitchIngress.hit')]
        )

        # sleep for sometime so we know that the ttl of the entry is definitely
        # less than 10000 when we query it later
        time.sleep(7)

        # check ttl get
        resp = dmac_table.entry_get(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, '255.255.0.0'),
                                  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],
            {"from_hw": True})

        data_dict = next(resp)[0].to_dict()
        recv_port = data_dict["port"]
        recv_ttl = data_dict["$ENTRY_TTL"]
        logger.info("Received TTL is %s", str(recv_ttl))
        if (recv_port != eg_port):
            logger.error("Error! port sent = %s received port = %s", str(eg_port), str(recv_port))
            assert 0
        if (recv_ttl >= ttl_set):
            logger.error("Error! ttl set = %s received ttl = %s",
                         str(ttl_set),
                         str(recv_ttl))
            assert 0

        logger.info("Deleting entry from table")
        dmac_table.entry_del(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, '255.255.0.0'),
                                  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])])  # priority req for ternary

        logger.info("Disable idle timeout on the table")
        dmac_table.attribute_idle_time_set(target, False,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)

    def runTest(self):
        self.runCustomTest()

class IdlePollTest(BfRuntimeTest):
    """@brief Demonstrate how to use the hit state attribute and verify it is
    working as expected.
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def verifyHitStateValue(self, resp, hit_state):
        data_dict = next(resp)[0].to_dict()
        assert data_dict["$ENTRY_HIT_STATE"] == hit_state

    def runCustomTest(self):
        ig_port = swports[1]
        eg_port = swports[2]
        smac = '12:34:45:67:89:ab'
        smac_mask = 0xffffffffffff
        dmac = '11:22:33:44:55:77'
        dip = '10.11.12.1'

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_table = bfrt_info.table_get("SwitchIngress.dmac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.src_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.port_table = bfrt_info.table_get("$PORT")
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))

        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target
        dmac_table.attribute_idle_time_set(target,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE)
        resp = dmac_table.attribute_get(target, "IdleTimeout")
        # Server sets attributes only on notify mode.
        for d in resp:
            assert d["ttl_query_interval"] == 0
            assert d["idle_table_mode"] == bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE
            assert d["enable"] == True

        dmac_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                             predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)

        pkt = testutils.simple_tcp_packet(eth_dst=dmac, ip_dst=dip, eth_src=smac)
        exp_pkt = pkt

        dmac_table.entry_add(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, '255.255.0.0'),  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],  # priority req for ternary
            [dmac_table.make_data([client.DataTuple('port', eg_port)],
                                  'SwitchIngress.hit')]
        )

        # Get hit state update from sw. It should be ENTRY_IDLE
        resp = dmac_table.entry_get(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  # LPM
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip,
                                                  '255.255.0.0'),  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],
            # priority req for ternary
            {"from_hw": False})
        self.verifyHitStateValue(resp, "ENTRY_IDLE")

        logger.info("Sending packet on port %d", ig_port)
        testutils.send_packet(self, ig_port, pkt)

        logger.info("Expecting packet on port %d", eg_port)
        testutils.verify_packet(self, exp_pkt, eg_port)

        # Apply table operations to sync the hit status bits
        dmac_table.operations_execute(target, 'UpdateHitState')

        # Get hit state update from sw. It should be ENTRY_ACTIVE
        resp = dmac_table.entry_get(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  # LPM
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip,
                                                  '255.255.0.0'),  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],
            # priority req for ternary
            {"from_hw": False})
        self.verifyHitStateValue(resp, "ENTRY_ACTIVE")

        # Apply table operations to sync the hit status bits
        dmac_table.operations_execute(target, 'UpdateHitState')

        # Get hit state update from sw again. It should be ENTRY_IDLE since we have read it already
        # and it's clear on read
        resp = dmac_table.entry_get(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  # LPM
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip,
                                                  '255.255.0.0'),  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],
            # priority req for ternary
            {"from_hw": False})
        self.verifyHitStateValue(resp, "ENTRY_IDLE")

        # Set hit state to ACTIVE and verify it works for both SW and HW read.
        dmac_table.entry_mod(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, '255.255.0.0'),
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],
            [dmac_table.make_data([client.DataTuple('$ENTRY_HIT_STATE', str_val="ENTRY_ACTIVE")])])

        for hw_read in (False, True):
            resp = dmac_table.entry_get(
                target,
                [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),
                                      client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                      client.KeyTuple('hdr.ipv4.dst_addr', dip, '255.255.0.0'),
                                      client.KeyTuple('$MATCH_PRIORITY', 1)])],
                # priority req for ternary
                {"from_hw": hw_read})
            self.verifyHitStateValue(resp, "ENTRY_ACTIVE")

        # Apply table operations to sync the hit status bits
        dmac_table.operations_execute(target, 'UpdateHitState')

        # Get hit state update from sw again. It should be ENTRY_IDLE since we have read it already
        # and it's clear on read
        resp = dmac_table.entry_get(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  # LPM
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip,
                                                  '255.255.0.0'),  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])],
            # priority req for ternary
            {"from_hw": False})
        self.verifyHitStateValue(resp, "ENTRY_IDLE")

        logger.info("Deleting entry from the table")
        dmac_table.entry_del(
            target,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                  client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                  client.KeyTuple('hdr.ipv4.dst_addr', dip, '255.255.0.0'),
                                  # ternary
                                  client.KeyTuple('$MATCH_PRIORITY', 1)])])  # priority req for ternary

        logger.info("Disable idle timeout on the table")
        # Server sets attributes only on notify mode.
        dmac_table.attribute_idle_time_set(target, False,
                bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE)
        resp = dmac_table.attribute_get(target, "IdleTimeout")
        for d in resp:
            # Server sets values only for notify mode.
            assert d["ttl_query_interval"] == 0
            assert d["idle_table_mode"] == bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE
            assert d["enable"] == False

    def runTest(self):
        self.runCustomTest()

class IdlePollIndirectTest(BfRuntimeTest):
    """@brief This test does the following
    1. Puts the idle table associated with the match indirect table in poll mode.
    2. Adds match entry.
    3. Send a packet to make the entry ACTIVE
    4. Read HIT STATE to verify that its ACTIVE
    5. Read HIT STATE again and verify that its IDLE
    6. Repeat the same set of steps with table in asymmetric mode with entries in each pipe
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def verifyHitStateValue(self, resp, hit_state):
        data_dict = next(resp)[0].to_dict()
        assert data_dict["$ENTRY_HIT_STATE"] == hit_state

    def runCustomTest(self):
        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_indirect_table = bfrt_info.table_get("SwitchIngress.dmac_indirect")
        dmac_indirect_table.info.key_field_annotation_add("hdr.ipv4.src_addr", "ipv4")
        action_profile_table = bfrt_info.table_get("SwitchIngress.action_profile")
        self.port_table = bfrt_info.table_get("$PORT")
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))
        num_pipes = self.num_pipes

        ig_eg_port_list = {}
        for p in range(num_pipes):
            ig_eg_port_list[p] = list()
        if len(swports_0):
            ig_eg_port_list[0] = random.sample(swports_0, min(2, len(swports_0)))
        if len(swports_1):
            ig_eg_port_list[1] = random.sample(swports_1, min(2, len(swports_1)))
        if len(swports_2):
            ig_eg_port_list[2] = random.sample(swports_2, min(2, len(swports_2)))
        if len(swports_3):
            ig_eg_port_list[3] = random.sample(swports_3, min(2, len(swports_3)))

        # First, test symmetric mode
        logger.info("Testing Poll Mode for Match indirect table in symmetric mode")
        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target

        dmac_indirect_table.attribute_idle_time_set(target, True,
                                                    bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE)
        resp = dmac_indirect_table.attribute_get(target, "IdleTimeout")
        for d in resp:
            assert d["idle_table_mode"] == bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE
            assert d["enable"] == True

        dmac_indirect_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                                      predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)

        logger.info("Adding match entry to indirect table in symmetric mode")
        sips = ["%d.%d.%d.%d" % tuple([random.randint(1, 255) for x in range(4)]) for x in range(num_pipes)]
        action_mbr_ids = random.sample(list(range(1, 30000)), num_pipes)

        ig_port = swports[0]
        eg_port = swports[1]

        action_profile_table.entry_add(
            target,
            [action_profile_table.make_key([client.KeyTuple('$ACTION_MEMBER_ID', action_mbr_ids[0])])],
            [action_profile_table.make_data([client.DataTuple('port', eg_port)],
                                            'SwitchIngress.hit')]
        )

        dmac_indirect_table.entry_add(
            target,
            [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr', sips[0])])],
            [dmac_indirect_table.make_data([client.DataTuple('$ACTION_MEMBER_ID', action_mbr_ids[0])])])

        pkt = testutils.simple_tcp_packet(ip_src=sips[0])
        exp_pkt = pkt

        logger.info("Sending packet on port %d", ig_port)
        testutils.send_packet(self, ig_port, pkt)

        logger.info("Expecting packet on port %d", eg_port)
        testutils.verify_packet(self, exp_pkt, eg_port)

        logger.info("Reading hit state of the entry and expecting it to be HIT")
        # Apply table operations to sync the hit status bits
        dmac_indirect_table.operations_execute(target, 'UpdateHitState')

        resp = dmac_indirect_table.entry_get(
            target,
            [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr',
                                                           sips[0])])],
            {"from_hw": False})
        self.verifyHitStateValue(resp, "ENTRY_ACTIVE")

        # Now read the HIT STATE again, and we should expect IDLE
        # Apply table operations to sync the hit status bits
        dmac_indirect_table.operations_execute(target, 'UpdateHitState')

        resp = dmac_indirect_table.entry_get(
            target,
            [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr',
                                                           sips[0])])],
            {"from_hw": True})
        self.verifyHitStateValue(resp, "ENTRY_IDLE")

        logger.info("Deleting entry from the Match table and action profile table")

        dmac_indirect_table.entry_del(
            target,
            [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr', sips[0])])])

        action_profile_table.entry_del(
            target,
            [action_profile_table.make_key([client.KeyTuple('$ACTION_MEMBER_ID', action_mbr_ids[0])])])

        logger.info("Disable idle timeout on the table")
        dmac_indirect_table.attribute_idle_time_set(target, False,
                            bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE)

        logger.info("Testing Poll mode for Match indirect table in asymmetric mode")

        dmac_indirect_table.attribute_idle_time_set(target, True,
                                                    bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE)

        dmac_indirect_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                                      predefined_pipe_scope_val=bfruntime_pb2.Mode.SINGLE)

        logger.info("Installing match entries in all the pipes")
        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            target = client.Target(device_id=0, pipe_id=i)
            eg_port = ig_eg_port_list[i][-1]
            sip = sips[i]

            action_profile_table.entry_add(
                target,
                [action_profile_table.make_key([client.KeyTuple('$ACTION_MEMBER_ID', action_mbr_ids[i])])],
                [action_profile_table.make_data([client.DataTuple('port', eg_port)],
                                                'SwitchIngress.hit')]
            )

            dmac_indirect_table.entry_add(
                target,
                [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr', sip)])],
                [dmac_indirect_table.make_data([client.DataTuple('$ACTION_MEMBER_ID', action_mbr_ids[i])])])

        logger.info("Sending packets for each pipe")
        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            ig_port = ig_eg_port_list[i][0]
            eg_port = ig_eg_port_list[i][-1]

            sip = sips[i]
            pkt = testutils.simple_tcp_packet(ip_src=sip)
            exp_pkt = pkt

            logger.info("Sending packet on port %d", ig_port)
            testutils.send_packet(self, ig_port, pkt)

            logger.info("Expecting packet on port %d", eg_port)
            testutils.verify_packet(self, exp_pkt, eg_port)

        # Apply table operations to sync the hit status bits
        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target

        dmac_indirect_table.operations_execute(target, 'UpdateHitState')

        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            target = client.Target(device_id=0, pipe_id=i)
            resp = dmac_indirect_table.entry_get(
                target,
                [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr',
                                                               sips[i])])],
                {"from_hw": False})
            self.verifyHitStateValue(resp, "ENTRY_ACTIVE")

        dmac_indirect_table.operations_execute(target, 'UpdateHitState')

        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            target = client.Target(device_id=0, pipe_id=i)
            resp = dmac_indirect_table.entry_get(
                target,
                [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr',
                                                               sips[i])])],
                {"from_hw": False})
            self.verifyHitStateValue(resp, "ENTRY_IDLE")

        logger.info("Deleting entries from table")
        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            sip = sips[i]
            target = client.Target(device_id=0, pipe_id=i)

            dmac_indirect_table.entry_del(
                target,
                [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr', sip)])])

            action_profile_table.entry_del(
                target,
                [action_profile_table.make_key([client.KeyTuple('$ACTION_MEMBER_ID', action_mbr_ids[i])])])

        logger.info("Disable idle timeout on the table")
        dmac_indirect_table.attribute_idle_time_set(target, False,
                        bfruntime_pb2.IdleTable.IDLE_TABLE_POLL_MODE)

    def runTest(self):
        self.runCustomTest()

class IdleTableIndirectTableTest(BfRuntimeTest):
    """@brief Show how to set the TTL of an entry in an indirect table and verify
    it is descreasing as expected as well as removed after the timeout interval.
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runCustomTest(self):
        eg_port = swports[2]
        smac = '12:34:45:67:89:ab'
        dmac = '11:22:33:44:55:77'
        dip = '10.11.12.1'
        sip = '1.2.3.4'
        ttl_set = 10000

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_indirect_table = bfrt_info.table_get("SwitchIngress.dmac_indirect")
        dmac_indirect_table.info.key_field_annotation_add("hdr.ipv4.src_addr", "ipv4")
        action_profile_table = bfrt_info.table_get("SwitchIngress.action_profile")
        self.port_table = bfrt_info.table_get("$PORT")
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))

        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target

        # Set Idle Table attributes
        ttl_query_length = 1000

        dmac_indirect_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                                      predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)

        dmac_indirect_table.attribute_idle_time_set(target,
                                                    True,
                                                    bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                                    ttl_query_length)

        pkt = testutils.simple_tcp_packet(eth_dst=dmac, ip_dst=dip, eth_src=smac)

        action_profile_table.entry_add(
            target,
            [action_profile_table.make_key([client.KeyTuple('$ACTION_MEMBER_ID', 1)])],
            [action_profile_table.make_data([client.DataTuple('port', eg_port)],
                                            'SwitchIngress.hit')]
        )

        dmac_indirect_table.entry_add(
            target,
            [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr', sip)])],
            [dmac_indirect_table.make_data([client.DataTuple('$ACTION_MEMBER_ID', 1),
                                            client.DataTuple('$ENTRY_TTL', ttl_set)])])

        # sleep for sometime so we know that the ttl of the entry is definitely less than 10000 when we query it later
        time.sleep(7)

        # check ttl get
        resp = dmac_indirect_table.entry_get(
            target,
            [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr',
                                                           sip)])],
            {"from_hw": True},
            dmac_indirect_table.make_data([client.DataTuple("$ENTRY_TTL")]))

        data_dict = next(resp)[0].to_dict()
        print(data_dict)
        recv_ttl = data_dict["$ENTRY_TTL"]
        logger.info("Received TTL is %s", str(recv_ttl))
        if (recv_ttl >= ttl_set):
            logger.error("Error! ttl set = %s received ttl = %s",
                         str(ttl_set),
                         str(recv_ttl))
            assert 0

        # Now wait enough time for idle_timeout to expire
        logger.info("Waiting for idle timeout")
        time.sleep(7)

        idle_time = self.interface.idletime_notification_get()
        key_dict = bfrt_info.key_from_idletime_notification(idle_time).to_dict()
        recv_sip = key_dict["hdr.ipv4.src_addr"]["value"]
        assert sip == recv_sip, "sip = %s recv sip = %s" % (sip, recv_sip)

        logger.info("Deleting entry from table")
        dmac_indirect_table.entry_del(
            target,
            [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr', sip)])])
        action_profile_table.entry_del(
            target,
            [action_profile_table.make_key([client.KeyTuple('$ACTION_MEMBER_ID', 1)])])

        logger.info("Disable idle timeout on the table")
        dmac_indirect_table.attribute_idle_time_set(target, False,
                                                    bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)

    def runTest(self):
        self.runCustomTest()

class IdleTimeoutAsymmetricTest(BfRuntimeTest):
    """@brief This test adds entries to a match table with ENTRY_SCOPE as
    single-pipeline and verifies that the aging notification is received for
    entries from all the pipes.
    Here are the steps that this test follows:

    1. Set entry scope of the match table to single-pipeline
    2. Add match entry to each of the pipe-line
    3. Send packets to each of the match entry, so that the entry goes active
    4. Wait for idle time notification from each of the pipe
    5. Delete the match entries
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runCustomTest(self):
        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_table = bfrt_info.table_get("SwitchIngress.dmac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.src_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")
        self.port_table = bfrt_info.table_get("$PORT")
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))
        num_pipes = self.num_pipes

        ig_eg_port_list = {}
        for p in range(num_pipes):
            ig_eg_port_list[p] = list()
        if len(swports_0):
            ig_eg_port_list[0] = random.sample(swports_0, min(2, len(swports_0)))
        if len(swports_1):
            ig_eg_port_list[1] = random.sample(swports_1, min(2, len(swports_1)))
        if len(swports_2):
            ig_eg_port_list[2] = random.sample(swports_2, min(2, len(swports_2)))
        if len(swports_3):
            ig_eg_port_list[3] = random.sample(swports_3, min(2, len(swports_3)))

        dmacs = ['%s:%s:%s:%s:%s:%s' % tuple([hex(random.randint(1, 255))[2:].zfill(2) for x in range(6)]) for i in
                 range(num_pipes)]
        smacs = ['%s:%s:%s:%s:%s:%s' % tuple([hex(random.randint(1, 255))[2:].zfill(2) for x in range(6)]) for i in
                 range(num_pipes)]
        dips = ["%d.%d.%d.%d" % tuple([random.randint(1, 255) for x in range(4)]) for x in range(num_pipes)]
        dip_masks = [(((0xffffffff) << (32 - random.randint(0, 32))) & (0xffffffff)) for x in range(num_pipes)]
        smac_mask = 0xffffffffffff

        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target

        # Set Idle Table attributes
        ttl_query_length = 1000
        dmac_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                             predefined_pipe_scope_val=bfruntime_pb2.Mode.SINGLE)

        dmac_table.attribute_idle_time_set(target, True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)

        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            target = client.Target(device_id=0, pipe_id=i)
            eg_port = ig_eg_port_list[i][-1]
            smac = smacs[i]
            dmac = dmacs[i]
            dip = dips[i]
            dip_mask = dip_masks[i]
            dmac_table.entry_add(
                target,
                [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                      client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                      client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask),  # ternary
                                      client.KeyTuple('$MATCH_PRIORITY', 1)])],  # priority req for ternary
                [dmac_table.make_data([client.DataTuple('port', eg_port),
                                       client.DataTuple('$ENTRY_TTL', 1000)],
                                      'SwitchIngress.hit')]
            )

        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            ig_port = ig_eg_port_list[i][0]
            eg_port = ig_eg_port_list[i][-1]

            smac = smacs[i]
            dmac = dmacs[i]
            dip = dips[i]
            pkt = testutils.simple_tcp_packet(eth_dst=dmac, ip_dst=dip, eth_src=smac)
            exp_pkt = pkt

            logger.info("Sending packet on port %d", ig_port)
            testutils.send_packet(self, ig_port, pkt)

            logger.info("Expecting packet on port %d", eg_port)
            testutils.verify_packet(self, exp_pkt, eg_port)

        testutils.verify_no_other_packets(self, timeout=2)

        logger.info("Waiting for entries to age out")
        time.sleep(5)

        # find the field_ids of all the key_fields
        pipes_seen = set()
        # Check for timeout notification.
        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            idle_time = self.interface.idletime_notification_get()
            recv_dmac = recv_smac = recv_dip = ""
            assert idle_time.target.device_id == 0
            pipe_id = idle_time.target.pipe_id
            assert pipe_id >= 0 and pipe_id < num_pipes

            smac = smacs[pipe_id]
            dmac = dmacs[pipe_id]
            dip = dips[pipe_id]
            dip_mask = dip_masks[pipe_id]

            logger.info("Received idle timeout notification from pipe %d" % pipe_id)
            assert pipe_id not in pipes_seen

            pipes_seen.add(pipe_id)
            recv_key = bfrt_info.key_from_idletime_notification(idle_time).to_dict()
            recv_dmac = recv_key["hdr.ethernet.dst_addr"]["value"]
            recv_smac = recv_key["hdr.ethernet.src_addr"]["value"]
            recv_dip = recv_key["hdr.ipv4.dst_addr"]["value"]

            if (dmac != recv_dmac):
                logger.error("Error! dmac = %s received dmac = %s", str(dmac), str(recv_dmac))
                assert 0

            if (smac != recv_smac):
                logger.error("Error! smac = %s received smac = %s", str(smac), str(recv_smac))
                assert 0

            dip_mask_hex = client.to_bytes(dip_mask, 4)
            dip_hex = client.ipv4_to_bytes(dip)
            recv_dip_hex = client.ipv4_to_bytes(recv_dip)
            for i in range(4):
                dip_hex[i] = dip_hex[i] & dip_mask_hex[i]
            if (dip_hex != recv_dip_hex):
                logger.error("Error! dip = %s received dip = %s", str(dip), str(recv_dip))
                assert 0

        logger.info("Deleting entries from table")
        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            smac = smacs[i]
            dmac = dmacs[i]
            dip = dips[i]
            dip_mask = dip_masks[i]

            target = client.Target(device_id=0, pipe_id=i)
            self.target = target

            dmac_table.entry_del(
                target,
                [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                      client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                      client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask),  # ternary
                                      client.KeyTuple('$MATCH_PRIORITY', 1)])])  # priority req for ternary

            ig_port = ig_eg_port_list[i][0]

            logger.info("Sending packet on port %d", ig_port)
            testutils.send_packet(self, ig_port, pkt)

            logger.info("Packet is expected to get dropped.")
            testutils.verify_no_other_packets(self)

        logger.info("Disable idle timeout on the table")
        dmac_table.attribute_idle_time_set(target, False,
                bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)

    def runTest(self):
        self.runCustomTest()

class IdleTimeoutIndirectAsymmetricTest(BfRuntimeTest):
    """@brief This test adds entries to a match table with ENTRY_SCOPE as
    single-pipeline and verifies that the aging notification is received for
    entries from all the pipes.
    Here are the steps that this test follows:

    1. Set entry scope of the match table to single-pipeline
    2. Add match entry to each of the pipe-line
    3. Send packets to each of the match entry, so that the entry goes active
    4. Wait for idle time notification from each of the pipe
    5. Delete the match entries
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runCustomTest(self):
        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_indirect_table = bfrt_info.table_get("SwitchIngress.dmac_indirect")
        dmac_indirect_table.info.key_field_annotation_add("hdr.ipv4.src_addr", "ipv4")
        action_profile_table = bfrt_info.table_get("SwitchIngress.action_profile")
        self.port_table = bfrt_info.table_get("$PORT")
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))
        num_pipes = self.num_pipes

        ig_eg_port_list = {}
        for p in range(num_pipes):
            ig_eg_port_list[p] = list()
        if len(swports_0):
            ig_eg_port_list[0] = random.sample(swports_0, min(2, len(swports_0)))
        if len(swports_1):
            ig_eg_port_list[1] = random.sample(swports_1, min(2, len(swports_1)))
        if len(swports_2):
            ig_eg_port_list[2] = random.sample(swports_2, min(2, len(swports_2)))
        if len(swports_3):
            ig_eg_port_list[3] = random.sample(swports_3, min(2, len(swports_3)))

        sips = ["%d.%d.%d.%d" % tuple([random.randint(1, 255) for x in range(4)]) for x in range(num_pipes)]
        action_mbr_ids = random.sample(list(range(1, 30000)), num_pipes)

        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target

        # Set Idle Table attributes
        ttl_query_length = 1000
        dmac_indirect_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                                      predefined_pipe_scope_val=bfruntime_pb2.Mode.SINGLE)

        dmac_indirect_table.attribute_idle_time_set(target, True,
                                                    bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                                    ttl_query_length)

        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            target = client.Target(device_id=0, pipe_id=i)
            eg_port = ig_eg_port_list[i][-1]
            sip = sips[i]

            action_profile_table.entry_add(
                target,
                [action_profile_table.make_key([client.KeyTuple('$ACTION_MEMBER_ID', action_mbr_ids[i])])],
                [action_profile_table.make_data([client.DataTuple('port', eg_port)],
                                                'SwitchIngress.hit')]
            )

            dmac_indirect_table.entry_add(
                target,
                [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr', sip)])],
                [dmac_indirect_table.make_data([client.DataTuple('$ACTION_MEMBER_ID', action_mbr_ids[i]),
                                                client.DataTuple('$ENTRY_TTL', 1000)])])

        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            ig_port = ig_eg_port_list[i][0]
            eg_port = ig_eg_port_list[i][-1]

            sip = sips[i]
            pkt = testutils.simple_tcp_packet(ip_src=sip)
            exp_pkt = pkt

            logger.info("Sending packet on port %d", ig_port)
            testutils.send_packet(self, ig_port, pkt)

            logger.info("Expecting packet on port %d", eg_port)
            testutils.verify_packet(self, exp_pkt, eg_port)

        testutils.verify_no_other_packets(self, timeout=2)

        logger.info("Waiting for entries to age out")
        time.sleep(5)

        # find the field_ids of all the key_fields
        pipes_seen = set()
        # Check for timeout notification.
        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            idle_time = self.interface.idletime_notification_get()
            recv_sip = ""
            assert idle_time.target.device_id == 0
            pipe_id = idle_time.target.pipe_id
            assert pipe_id >= 0 and pipe_id < num_pipes

            sip = sips[pipe_id]

            logger.info("Received idle timeout notification from pipe %d" % pipe_id)
            assert pipe_id not in pipes_seen

            pipes_seen.add(pipe_id)
            key_dict = bfrt_info.key_from_idletime_notification(idle_time).to_dict()
            recv_sip = key_dict["hdr.ipv4.src_addr"]["value"]

            if (sip != recv_sip):
                logger.error("Error! sip = %s received sip = %s for pipe id %d", str(sip), str(recv_sip), i)
                assert 0

        logger.info("Deleting entries from table")
        for i in range(num_pipes):
            if len(ig_eg_port_list[i]) == 0:
                continue
            sip = sips[i]
            target = client.Target(device_id=0, pipe_id=i)

            dmac_indirect_table.entry_del(
                target,
                [dmac_indirect_table.make_key([client.KeyTuple('hdr.ipv4.src_addr', sip)])])

            action_profile_table.entry_del(
                target,
                [action_profile_table.make_key([client.KeyTuple('$ACTION_MEMBER_ID', action_mbr_ids[i])])])

            ig_port = ig_eg_port_list[i][0]

            pkt = testutils.simple_tcp_packet(ip_src=sip)
            exp_pkt = pkt

            logger.info("Sending packet on port %d", ig_port)
            testutils.send_packet(self, ig_port, pkt)

            logger.info("Packet is expected to get dropped.")
            testutils.verify_no_other_packets(self)

        logger.info("Disable idle timeout on the table")
        dmac_indirect_table.attribute_idle_time_set(target, False,
                bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)

    def runTest(self):
        self.runCustomTest()

@testutils.group("thorough")
class IdleTimeoutEnableDisable(BfRuntimeTest):
    """@brief Test table with added entries before table is enabled.
    1. Set table to notify mode.
    2. Add entries with different TTLs and verify aging is disabled.
    3. Enable table and verify TTL is decreasing as it should.
    4. Change ttl query length to much higher value and verify TTL doesn't
       get updated after similar wait time as before (change was applied).
    5. Disable table after one entry ages out.
    6. Enable table and verify that aged entry stayed aged and
       active entries continue with last seen TTL value.
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runCustomTest(self):
        eg_port = swports[2]
        dmac = '11:22:33:44:55:'

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_table = bfrt_info.table_get("SwitchIngress.dmac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        self.port_table = bfrt_info.table_get("$PORT")
        self.num_pipes = get_num_pipes(bfrt_info.table_get("device_configuration"))

        target = client.Target(device_id=0, pipe_id=0xffff)
        self.target = target

        # Set Idle Table attributes
        ttl_query_length = 1000
        logger.info("Set idle timeout table mode to notify")
        dmac_table.attribute_idle_time_set(target,
                                           False,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)

        dmac_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                             predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)

        # x used as both last DMAC byte and ttl value
        ttl_list = [10000, 20000, 30000]
        logger.info("Add {} entries".format(len(ttl_list)))
        for x in (ttl_list):
            dmac_table.entry_add(
                target,
                [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr',
                                                      dmac + str(x//1000))])],
                [dmac_table.make_data([client.DataTuple('port', eg_port),
                                       client.DataTuple('$ENTRY_TTL', x)],
                                      'SwitchIngress.hit')]
            )

        # Sleep for 5000ms (5 times query length), then get the entries.
        # The entries current TTL should not have decreased because idletimeouts
        # are not enabled on the table.
        time.sleep(5 * ttl_query_length / 1000)

        # check ttl get on all entries
        resp = dmac_table.entry_get(target, None, {"from_hw": True})

        for data, _ in resp:
            dd = data.to_dict()
            recv_port = dd["port"]
            recv_ttl = dd["$ENTRY_TTL"]
            logger.info("Received TTL is %s", str(recv_ttl))
            if (recv_port != eg_port):
                logger.error("Error! port sent = %s received port = %s", str(eg_port), str(recv_port))
                assert 0
            if recv_ttl not in ttl_list:
                logger.error("Error! ttl set = %s received ttl = %s",
                             str(ttl_list)[1:-1],
                             str(recv_ttl))
                assert 0

        logger.info("Enable with ttl_query_length to {}".format(ttl_query_length))
        dmac_table.attribute_idle_time_set(target,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)
        logger.info(datetime.datetime.now())

        # Enable idle timeout and sleep for another 5000ms, then get the entries.
        # We expect the entries TTL to have decreased from their initial values
        # since timeouts are enabled now and the entries are aging. Add extra
        # 0.1s in order to avoid any race conditions.
        time.sleep((5 * ttl_query_length / 1000) + 0.1)

        resp = dmac_table.entry_get(target, None, {"from_hw": True})

        exp_ttl = 5000
        err_margin = ttl_query_length * 2
        for data, _ in resp:
            dd = data.to_dict()
            recv_ttl = dd["$ENTRY_TTL"]
            logger.info("Received TTL is %s", str(recv_ttl))
            if (recv_ttl > exp_ttl + err_margin):
                logger.error("Error! ttl exp = %s received ttl = %s",
                             str(exp_ttl + err_margin),
                             str(recv_ttl))
                assert 0
            if (recv_ttl < exp_ttl - err_margin):
                logger.error("Error! ttl exp = %s received ttl = %s",
                             str(exp_ttl - err_margin),
                             str(recv_ttl))
                assert 0
            exp_ttl += 10000
        logger.info(datetime.datetime.now())

        ttl_query_length = 10000
        logger.info("Change ttl_query_length to {}".format(ttl_query_length))
        dmac_table.attribute_idle_time_set(target,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)
        logger.info(datetime.datetime.now())

        # In this test ttl_query_length is much longer than before.
        # Get entries two times with sleep in between equal to half of
        # ttl_query_length. Expectation is that if new configuration was properly
        # applied, then ttl will not change.
        resp = dmac_table.entry_get(target, None, {"from_hw": True})

        recv_ttl_list = []
        exp_ttl = 5000
        err_margin = ttl_query_length * 2
        for data, _ in resp:
            dd = data.to_dict()
            recv_ttl = dd["$ENTRY_TTL"]
            recv_ttl_list.append(recv_ttl)
            logger.info("Received TTL is %s", str(recv_ttl))
            if (recv_ttl > exp_ttl + err_margin):
                logger.error("Error! ttl exp = %s received ttl = %s",
                             str(exp_ttl + err_margin),
                             str(recv_ttl))
                assert 0
            if (recv_ttl < exp_ttl - err_margin):
                logger.error("Error! ttl exp = %s received ttl = %s",
                             str(exp_ttl - err_margin),
                             str(recv_ttl))
                assert 0
            exp_ttl += 10000
        logger.info(datetime.datetime.now())

        time.sleep((ttl_query_length / 2) / 1000)

        resp = dmac_table.entry_get(target, None, {"from_hw": True})
        exp_ttl = 5000
        err_margin = ttl_query_length * 2
        for data, _ in resp:
            dd = data.to_dict()
            recv_ttl = dd["$ENTRY_TTL"]
            logger.info("Received TTL is %s", str(recv_ttl))
            if (recv_ttl > exp_ttl + err_margin):
                logger.error("Error! ttl exp = %s received ttl = %s",
                             str(exp_ttl + err_margin),
                             str(recv_ttl))
                assert 0
            if (recv_ttl < exp_ttl - err_margin):
                logger.error("Error! ttl exp = %s received ttl = %s",
                             str(exp_ttl - err_margin),
                             str(recv_ttl))
                assert 0
            exp_ttl += 10000
        logger.info(datetime.datetime.now())

        ttl_query_length = 1000
        logger.info("Change ttl_query_length to {}".format(ttl_query_length))
        dmac_table.attribute_idle_time_set(target,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)
        logger.info(datetime.datetime.now())

        # Changing query length back to 1000 will reset the sweep period.
        # Sleep for another 7 intervals to make sure entry ages out.
        time.sleep((7 * ttl_query_length / 1000) + 0.1)
        # Check if first entry aged out as expected.
        exp_dmac = dmac+"10"
        idle_time = self.interface.idletime_notification_get()
        recv_key = bfrt_info.key_from_idletime_notification(idle_time)
        key_dict = recv_key.to_dict()
        recv_dmac = key_dict["hdr.ethernet.dst_addr"]["value"]
        if (exp_dmac != recv_dmac):
            logger.error("Error! dmac = %s received dmac = %s", str(exp_dmac), str(recv_dmac))
            assert 0

        # Disable idle timeout and verify ttl values on all entries.
        logger.info("Disable idle table")
        dmac_table.attribute_idle_time_set(target,
                                           False,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)
        logger.info(datetime.datetime.now())

        resp = dmac_table.entry_get(target, None, {"from_hw": True})

        num_aged_entries = 0
        recv_ttl_list = []
        # There should be 12 intervals total that passed
        exp_ttl = 20000 - 12 * ttl_query_length
        err_margin = ttl_query_length * 2
        for data, _ in resp:
            dd = data.to_dict()
            recv_ttl = dd["$ENTRY_TTL"]
            recv_ttl_list.append(recv_ttl)
            logger.info("Received TTL is %s", str(recv_ttl))
            if (recv_ttl == 0):
                num_aged_entries += 1
            elif (recv_ttl > exp_ttl + err_margin):
                logger.error("Error! ttl exp = %s received ttl = %s",
                             str(exp_ttl + err_margin),
                             str(recv_ttl))
                assert 0
            elif (recv_ttl < exp_ttl - err_margin):
                logger.error("Error! ttl exp = %s received ttl = %s",
                             str(exp_ttl - err_margin),
                             str(recv_ttl))
                assert 0
            if recv_ttl != 0 :
                exp_ttl += 10000

        assert num_aged_entries == 1
        logger.info(datetime.datetime.now())

        logger.info("Enable with ttl_query_length to {}".format(ttl_query_length))
        dmac_table.attribute_idle_time_set(target,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)

        resp = dmac_table.entry_get(target, None, {"from_hw": True})

        # Verify that after re-enabling table ttl values remain as before
        for data, _ in resp:
            dd = data.to_dict()
            recv_ttl = dd["$ENTRY_TTL"]
            logger.info("Received TTL is %s", str(recv_ttl))
            if (recv_ttl not in recv_ttl_list):
                logger.error("Error! received ttl = %s not as expected %s",
                             str(recv_ttl),
                             str(recv_ttl_list)[1:-1])
                assert 0
        logger.info(datetime.datetime.now())

        # Wait for highest entry TTL + query length
        time.sleep(((max(recv_ttl_list)+ttl_query_length)/1000) + 0.1)

        # Check if remaining entries aged out as expected.
        for x in (["20","30"]):
            exp_dmac = dmac+x
            idle_time = self.interface.idletime_notification_get()
            recv_key = bfrt_info.key_from_idletime_notification(idle_time)
            key_dict = recv_key.to_dict()
            recv_dmac = key_dict["hdr.ethernet.dst_addr"]["value"]
            if (exp_dmac != recv_dmac):
                logger.error("Error! dmac = %s received dmac = %s", str(exp_dmac), str(recv_dmac))
                assert 0

        # All entries should have TTL = 0
        resp = dmac_table.entry_get(target, None, {"from_hw": True})

        for data, _ in resp:
            dd = data.to_dict()
            recv_ttl = dd["$ENTRY_TTL"]
            logger.info("Received TTL is %s", str(recv_ttl))
            if (recv_ttl != 0):
                logger.error("Error! received ttl = %s != 0",
                             str(recv_ttl))
                assert 0
        logger.info(datetime.datetime.now())

        logger.info("Deleting all entries from the table")
        dmac_table.entry_del(target, [])
        logger.info("Disable idle timeout on the table")
        dmac_table.attribute_idle_time_set(target, False,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)

    def runTest(self):
        self.runCustomTest()

@testutils.group("thorough")
class IdleTimeoutBoundaryTest(BfRuntimeTest):
    """@Test of the boundary idle timeout configuration parameters and
    their usage.
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runTest(self):
        ig_port = swports[1]
        eg_port = swports[2]

        dmac = client.bytes_to_mac(client.to_bytes(random.randint(0, 2 ** 48 - 1), 6))
        smac = client.bytes_to_mac(client.to_bytes(random.randint(0, 2 ** 48 - 1), 6))
        smac_mask = 0xffffffffffff
        dip = client.bytes_to_ipv4(client.to_bytes(random.randint(0, 2 ** 32 - 1), 4))
        dip_mask = ((0xffffffff) << (32 - random.randint(0, 32))) & (0xffffffff)
        print("dmac is {}".format(dmac))

        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        dmac_table = bfrt_info.table_get("SwitchIngress.dmac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ethernet.src_addr", "mac")
        dmac_table.info.key_field_annotation_add("hdr.ipv4.dst_addr", "ipv4")

        target = client.Target(device_id=0, pipe_id=0xffff)

        dmac_table.attribute_entry_scope_set(target, predefined_pipe_scope=True,
                                             predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)
        # Set Idle Table attributes
        ttl_query_length_list = {100, 333, 999, 1111, 2222, 3333}
        for ttl_query_length in ttl_query_length_list:
            ttl_set = ttl_query_length
            logger.info("ttl_query_length = %d", ttl_query_length)
            logger.info("ttl_set = %d", ttl_set)
            dmac_table.attribute_idle_time_set(target,
                                               True,
                                               bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                               ttl_query_length)

            resp = dmac_table.attribute_get(target, "IdleTimeout")
            for d in resp:
                assert d["ttl_query_interval"] == ttl_query_length
                assert d["idle_table_mode"] == bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE
                assert d["enable"] == True

            pkt = testutils.simple_tcp_packet(eth_dst=dmac, ip_dst=dip, eth_src=smac)
            exp_pkt = pkt

            dmac_table.entry_add(
                target,
                [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                      client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                      client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask),  # ternary
                                      client.KeyTuple('$MATCH_PRIORITY', 1)])],  # priority req for ternary
                [dmac_table.make_data([client.DataTuple('port', eg_port),
                                       client.DataTuple('$ENTRY_TTL', ttl_set)],
                                      'SwitchIngress.hit')]
            )


            # sleep for sometime so we know that the ttl of the entry is definitely
            # less than configured when we query it later
            time.sleep(7)

            # check ttl get
            resp = dmac_table.entry_get(
                target,
                [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                      client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                      client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask), # ternary
                                      client.KeyTuple('$MATCH_PRIORITY', 1)])],
                {"from_hw": True})

            # check recv_ttl is 0
            data_dict = next(resp)[0].to_dict()
            recv_port = data_dict["port"]
            recv_ttl = data_dict["$ENTRY_TTL"]
            logger.info("Received TTL is %s", str(recv_ttl))
            if (recv_ttl !=0):
                logger.error("Error! recv_ttl = %s", str(recv_ttl))
                assert 0

            # Check for timeout notification before sending traffic
            idle_time = self.interface.idletime_notification_get()
            recv_key = bfrt_info.key_from_idletime_notification(idle_time)
            key_dict = recv_key.to_dict()
            recv_dmac = key_dict["hdr.ethernet.dst_addr"]["value"]
            recv_smac = key_dict["hdr.ethernet.src_addr"]["value"]
            recv_dip = key_dict["hdr.ipv4.dst_addr"]["value"]
            recv_dip_mask = key_dict["hdr.ipv4.dst_addr"]["mask"]

            if (dmac != recv_dmac):
                logger.error("Error! dmac = %s received dmac = %s", str(dmac), str(recv_dmac))
                assert 0

            # sending test packets
            logger.info("Sending packet on port %d", ig_port)
            testutils.send_packet(self, ig_port, pkt)

            logger.info("Expecting packet on port %d", eg_port)
            testutils.verify_packet(self, exp_pkt, eg_port)

            time.sleep(8)
            # check ttl get
            resp = dmac_table.entry_get(
                target,
                [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                      client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),
                                      client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask), # ternary
                                      client.KeyTuple('$MATCH_PRIORITY', 1)])],
                {"from_hw": True})
            data_dict = next(resp)[0].to_dict()
            recv_port = data_dict["port"]
            recv_ttl = data_dict["$ENTRY_TTL"]
            logger.info("Received TTL is %s", str(recv_ttl))


            # Check for timeout notification.
            idle_time = self.interface.idletime_notification_get()
            recv_key = bfrt_info.key_from_idletime_notification(idle_time)
            key_dict = recv_key.to_dict()
            recv_dmac = key_dict["hdr.ethernet.dst_addr"]["value"]
            recv_smac = key_dict["hdr.ethernet.src_addr"]["value"]
            recv_dip = key_dict["hdr.ipv4.dst_addr"]["value"]

            if (dmac != recv_dmac):
                logger.error("Error! dmac = %s received dmac = %s", str(dmac), str(recv_dmac))
                assert 0

            if (smac != recv_smac):
                logger.error("Error! smac = %s received smac = %s", str(smac), str(recv_smac))
                assert 0

            dip_mask_hex = client.to_bytes(dip_mask, 4)
            dip_hex = client.ipv4_to_bytes(dip)
            recv_dip_hex = client.ipv4_to_bytes(recv_dip)
            for i in range(4):
                dip_hex[i] = dip_hex[i] & dip_mask_hex[i]
            if (dip_hex != recv_dip_hex):
                logger.error("Error! dip = %s received dip = %s", str(dip), str(recv_dip))
                assert 0

            logger.info("Deleting entry from table")
            dmac_table.entry_del(
                target,
                [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac),  # exact
                                      client.KeyTuple('hdr.ethernet.src_addr', smac, smac_mask),  # LPM
                                      client.KeyTuple('hdr.ipv4.dst_addr', dip, dip_mask),  # ternary
                                      client.KeyTuple('$MATCH_PRIORITY', 1)])])  # priority req for ternary

            logger.info("Sending packet on port %d", ig_port)
            testutils.send_packet(self, ig_port, pkt)

            logger.info("Packet is expected to get dropped.")
            testutils.verify_no_other_packets(self)

            logger.info("Disable idle timeout on the table")
            dmac_table.attribute_idle_time_set(target,
                                               False,
                                               bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)
            default_interval = 5000
            resp = dmac_table.attribute_get(target, "IdleTimeout")
            for d in resp:
                assert d["ttl_query_interval"] == default_interval
                assert d["idle_table_mode"] == bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE
                assert d["enable"] == False


@testutils.group("thorough")
class IdleTimeoutMultiEntryTest(BfRuntimeTest):
    """@brief Simple test of the basic idle timeout configuration parameters and
    their usage.
    """

    def setUp(self):
        BfRuntimeTest.setUp(self, client_id, p4_name)
        self.bfrt_info = self.interface.bfrt_info_get("tna_idletimeout")
        setup_random()

    def tearDown(self):
        trgt = client.Target(device_id=dev_id)
        dmac_table = self.bfrt_info.table_get("SwitchIngress.dmac")
        # Cleanup entries.
        dmac_table.entry_del(trgt, [])

        # Disable idletime on the table.
        dmac_table.attribute_idle_time_set(trgt,
                                           False,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)
        resp = dmac_table.attribute_get(trgt, "IdleTimeout")
        once = 0
        for d in resp:
            self.assertEqual(d["idle_table_mode"], bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE)
            self.assertEqual(d["enable"], False)
            once += 1
        self.assertEqual(once, 1)
        BfRuntimeTest.tearDown(self)

    def runTest(self):
        ig_port = swports[1]
        eg_port = swports[2]
        trgt = client.Target(device_id=dev_id)

        # Create "max_entry" number of unique keys to be used in the test.
        # We will first add entries using these keys.  Then test forwarding for
        # the entries.  Then wait for them to age out and verify the idle time
        # notifications are for these entries.
        entry_count = 5
        EntryVals = namedtuple("EntryVals", "dmac smac smac_msk dip dip_msk pri")
        entries = set()
        while len(entries) < entry_count:
            dmac = random.getrandbits(48)
            smac = random.getrandbits(48)
            smac_msk = 0xffffffffffff
            dip_msk = random.getrandbits(32)
            dip = random.getrandbits(32) & dip_msk
            pri = 48
            entries.add(EntryVals(dmac, smac, smac_msk, dip, dip_msk, pri))

        dmac_table = self.bfrt_info.table_get("SwitchIngress.dmac")

        # Make sure the table is in symmetric mode.
        dmac_table.attribute_entry_scope_set(trgt, predefined_pipe_scope=True,
                                             predefined_pipe_scope_val=bfruntime_pb2.Mode.ALL)

        # Enable idletime on the table, set query interval to 1 second (1000ms).
        ttl_query_length = 1000
        dmac_table.attribute_idle_time_set(trgt,
                                           True,
                                           bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE,
                                           ttl_query_length)
        # Read the configuration back to demonstrate how to query the configured mode.
        resp = dmac_table.attribute_get(trgt, "IdleTimeout")
        once = 0
        for d in resp:
            assert d["ttl_query_interval"] == ttl_query_length
            assert d["idle_table_mode"] == bfruntime_pb2.IdleTable.IDLE_TABLE_NOTIFY_MODE
            assert d["enable"] == True
            once += 1
        self.assertEqual(once, 1)

        # Add the entries to the table.
        logger.info("Adding %d entries", entry_count)
        dmac_table.entry_add(
            trgt,
            [dmac_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', e.dmac),
                                  client.KeyTuple('hdr.ethernet.src_addr', e.smac, e.smac_msk),
                                  client.KeyTuple('hdr.ipv4.dst_addr', e.dip, e.dip_msk),
                                  client.KeyTuple('$MATCH_PRIORITY', e.pri)]) for e in entries],
            [dmac_table.make_data([client.DataTuple('port', eg_port),
                                   client.DataTuple('$ENTRY_TTL', ttl_query_length)],
                                  'SwitchIngress.hit')] * len(entries))
        for e in entries:
            logger.info("%s", e)

        # Send a packet for each entry.
        logger.info("Send traffic for each entry")
        for e in entries:
            dmac = client.bytes_to_mac(client.to_bytes(e.dmac, 6))
            smac = client.bytes_to_mac(client.to_bytes(e.smac, 6))
            dip  = client.bytes_to_ipv4(client.to_bytes(e.dip, 4))
            pkt = testutils.simple_tcp_packet(eth_dst=dmac, ip_dst=dip, eth_src=smac)
            testutils.send_packet(self, ig_port, pkt)
            testutils.verify_packet(self, pkt, eg_port)

        # Wait for them to age out.
        wait_time = 1.5 * ttl_query_length
        logger.info("Waiting %f seconds for them to age out", wait_time / 1000 )
        time.sleep(wait_time / 1000)

        # Get the age out notifications and make sure they are for the correct set of keys.
        for _ in range(entry_count):
            idle_time = self.interface.idletime_notification_get()
            recv_key = self.bfrt_info.key_from_idletime_notification(idle_time)
            key_dict = recv_key.to_dict()
            dmac = key_dict["hdr.ethernet.dst_addr"]["value"]
            smac = key_dict["hdr.ethernet.src_addr"]["value"]
            smac_msk = key_dict["hdr.ethernet.src_addr"]["mask"]
            dip = key_dict["hdr.ipv4.dst_addr"]["value"]
            dip_msk = key_dict["hdr.ipv4.dst_addr"]["mask"]
            pri = key_dict["$MATCH_PRIORITY"]["value"]
            ent = EntryVals(dmac, smac, smac_msk, dip, dip_msk, pri)
            logger.info("Received notification for entry:")
            logger.info("%s", ent)
            if ent not in entries:
                logger.info("Recieved notification for unexpected entry!")
                logger.info("Expected set:")
                for e in entries:
                    logger.info("%s", e)
            self.assertIn(ent, entries)
            entries.remove(ent)

        # We should have seen all entries age out
        self.assertEqual(len(entries), 0)

        # There should be no more notifications.
        try:
            idle_time = self.interface.idletime_notification_get()
            logger.info("Extra idle notification received!!!")
            self.assertTrue(False)
        except RuntimeError:
            pass
