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

import time
import sys
import datetime

import random

import pd_base_tests

from ptf.testutils import *
from ptf.thriftutils import *
from p4testutils.misc_utils import *

from mirror_test.p4_pd_rpc.ttypes import *
from conn_mgr_pd_rpc.ttypes import *
from mirror_pd_rpc.ttypes import *
from mc_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from pal_rpc.ttypes import *
from ptf_port import *
import p4testutils.pal_utils as pal_utils

try:
    from pltfm_pm_rpc.ttypes import *
except ImportError as e:
    pass

g_arch         = test_param_get("arch").lower()
g_is_tofino    = ( g_arch == "tofino" )
g_is_tofino2   = ( g_arch == "tofino2" )
g_is_tofino3   = ( g_arch == "tofino3" )
dev_id         = 0

if g_is_tofino:
    MAX_SID_NORM = 1015
    MAX_SID_COAL = 1023
    BASE_SID_NORM = 1
    BASE_SID_COAL = 1016
    EXP_LEN1 = 127
    EXP_LEN2 = 63
elif g_is_tofino2:
    MAX_SID_NORM = 255
    MAX_SID_COAL = 255
    BASE_SID_NORM = 1
    BASE_SID_COAL = 1
    EXP_LEN1 = 127
    EXP_LEN2 = 59
elif g_is_tofino3:
    MAX_SID_NORM = 255
    # MAX_SID_COAL shall be also set to 255 for TF3
    # Only 16 coalesced sessions are supported, but session IDs can
    # be from the same range as normal sessions: 0..255
    MAX_SID_COAL = 255
    BASE_SID_NORM = 1
    BASE_SID_COAL = 1
    EXP_LEN1 = 127
    EXP_LEN2 = 59
else:
    assert False, "Unsupported arch %s".format(g_arch)

MCID1=1
MCID2=2
HASH1=1
HASH2=2

def portToBitIdx(port):
    pipe = port_to_pipe(port)
    index = port_to_pipe_local_port(port)
    return 72 * pipe + index

def set_port_map(indicies):
    bit_map = [0] * ((288+7)//8)
    if g_is_tofino3:
        bit_map = [0] * ((576+7)//8)
    for i in indicies:
        index = portToBitIdx(i)
        bit_map[index//8] = (bit_map[index//8] | (1 << (index%8))) & 0xFF
    return bytes_to_string(bit_map)

def set_lag_map(indicies):
    bit_map = [0] * ((256+7)//8)
    if g_is_tofino3:
        bit_map = [0] * ((512+7)//8)
    for i in indicies:
        bit_map[i//8] = (bit_map[i//8] | (1 << (i%8))) & 0xFF
    return bytes_to_string(bit_map)

logger = get_logger()
swports = get_sw_ports()
swports_by_pipe = {}
for port in swports:
    pipe = port_to_pipe(port)
    if pipe not in swports_by_pipe:
        swports_by_pipe[pipe] = []
    swports_by_pipe[pipe].append(port)
    swports_by_pipe[pipe].sort()

logger.info("All ports: {}".format(swports))

def mirror_session(mir_type, mir_dir, sid, egr_port=0, egr_port_v=False,
                   egr_port_queue=0, packet_color=0, mcast_grp_a=0,
                   mcast_grp_a_v=False, mcast_grp_b=0, mcast_grp_b_v=False,
                   max_pkt_len=0, level1_mcast_hash=0, level2_mcast_hash=0,
                   mcast_l1_xid=0, mcast_l2_xid=0, mcast_rid=0, cos=0, c2c=False, extract_len=0, timeout=0,
                   int_hdr=[], hdr_len=0):
    return MirrorSessionInfo_t(mir_type,
                               mir_dir,
                               sid,
                               egr_port,
                               egr_port_v,
                               egr_port_queue,
                               packet_color,
                               mcast_grp_a,
                               mcast_grp_a_v,
                               mcast_grp_b,
                               mcast_grp_b_v,
                               max_pkt_len,
                               level1_mcast_hash,
                               level2_mcast_hash,
                               mcast_l1_xid,
                               mcast_l2_xid,
                               mcast_rid,
                               cos,
                               c2c,
                               extract_len,
                               timeout,
                               int_hdr,
                               hdr_len)


def p0_add(test, shdl, dt, port, egr_port=None, ing_mir=0, egr_mir=0, ing_sid=0, egr_sid=0):
    if egr_port is None: egr_port = port
    mspec = mirror_test_p0_match_spec_t(port)
    aspec = mirror_test_set_md_action_spec_t(egr_port, ing_mir, ing_sid, egr_mir, egr_sid)
    h = test.client.p0_table_add_with_set_md(shdl, dt, mspec, aspec)
    return h

class TestBasicForwarding(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        setup_random()
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        shdl = self.conn_mgr.client_init()
        try:
            # Set up the forwarding table to send the packet back out the same port
            # it came in on.
            for port in swports:
                p0_add(self, shdl, dt, port)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=83)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Set up the forwarding table to send the packet to an invalid port which
            # drops the packet.
            for port in swports:
                p0_add(self, shdl, dt, port, egr_port=511)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=83)
            for port in swports:
                send_packet(self, port, pkt)
            time.sleep(3)
            verify_no_other_packets(self)

        finally:
            entry_count = self.client.p0_get_entry_count(shdl, dt)
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                self.client.p0_table_delete(shdl, dev_id, hdl)
            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.client_cleanup(shdl)

class TestTruncMir(pd_base_tests.ThriftInterfaceDataPlane):
    """
    Check that we can ingress mirror a packet out the port it came in on.
    """
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        setup_random()
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        shdl = self.conn_mgr.client_init()
        self.sids = []
        try:
            # Setup a session to mirror the packets out the ingress ports.
            # The original packets will be sent to a dummy port that drops them.
            sids = random.sample(range(BASE_SID_NORM, MAX_SID_NORM), len(swports))
            for port,sid in zip(swports, sids):
                p0_add(self, shdl, dt, port, egr_port=511, ing_mir=1, ing_sid=sid)
                if port % 2 == 0:
                    max_len = 128
                else:
                    max_len = 64
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                      Direction_e.PD_DIR_INGRESS,
                                      sid,
                                      port,
                                      True, max_pkt_len = max_len)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.sids.append(sid)
                logger.info("Using session {} for port {}".format(sid, port))
                sys.stdout.flush()
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=200)
            rec_pkt1 = simple_eth_packet(pktlen=EXP_LEN1)
            rec_pkt2 = simple_eth_packet(pktlen=EXP_LEN2)
            for port in swports:
                send_packet(self, port, pkt)
                if port % 2 == 0:
                    verify_packet(self, rec_pkt1, port)
                else:
                    verify_packet(self, rec_pkt2, port)
            verify_no_other_packets(self)

            # Disable the sessions and check again
            for sid in self.sids:
                self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=200)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

        finally:
            entry_count = self.client.p0_get_entry_count(shdl, dt)
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                self.client.p0_table_delete(shdl, dev_id, hdl)
            for sid in self.sids:
                self.mirror.mirror_session_delete(shdl, dt, sid)
            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.client_cleanup(shdl)

class TestBasicIngMir(pd_base_tests.ThriftInterfaceDataPlane):
    """
    Check that we can ingress mirror a packet out the port it came in on.
    """
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        setup_random()
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        shdl = self.conn_mgr.client_init()
        self.sids = []
        num_pipes = int(test_param_get('num_pipes'))
        try:
            # Setup a session to mirror the packets out the ingress ports.
            # The original packets will be sent to a dummy port that drops them.
            sids = random.sample(range(BASE_SID_NORM, MAX_SID_NORM), len(swports))
            for port,sid in zip(swports, sids):
                p0_add(self, shdl, dt, port, egr_port=511, ing_mir=1, ing_sid=sid)
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                      Direction_e.PD_DIR_INGRESS,
                                      sid,
                                      port,
                                      True)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.sids.append(sid)
                logger.info("Using session {} for port {}".format(sid, port))
                sys.stdout.flush()
            self.conn_mgr.complete_operations(shdl)

            for l in range(79, 110):
                pkt = simple_eth_packet(pktlen=l)
                for port in swports:
                    send_packet(self, port, pkt)
                for port in swports:
                    verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Disable the sessions and check again
            for sid in self.sids:
                self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=80)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

            # Enable the sessions and check again
            for sid in self.sids:
                self.mirror.mirror_session_enable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=81)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Disable the sessions in both directions and check again
            for sid in self.sids:
                self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_BOTH, dt, sid)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=82)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

            # Enable the sessions and check again
            for sid in self.sids:
                self.mirror.mirror_session_enable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=83)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Delete the sessions, add them again in a disabled state.
            while len(self.sids):
                sid = self.sids.pop(0)
                self.mirror.mirror_session_delete(shdl, dt, sid)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=89)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

            # Recreate the session per pipe.
            for port,sid in zip(swports, sids):
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                      Direction_e.PD_DIR_INGRESS,
                                      sid,
                                      port,
                                      True)
                for p in range( num_pipes ):
                    self.mirror.mirror_session_create(shdl, DevTarget_t(dev_id, p), info)
                self.sids.append(sid)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=92)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Disable in one pipe, leave enabled in other pipes.
            for sid in self.sids:
                self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_BOTH, DevTarget_t(dev_id, 0), sid)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=92)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                pipe = port_to_pipe(port)
                if pipe != 0:
                    verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Disable in other pipes, re-enable in first pipe.
            for sid in self.sids:
                self.mirror.mirror_session_enable(shdl, Direction_e.PD_DIR_INGRESS, DevTarget_t(dev_id, 0), sid)
                for p in range(1, num_pipes):
                    self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_BOTH, DevTarget_t(dev_id, p), sid)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=93)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                pipe = port_to_pipe(port)
                if pipe == 0:
                    verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Get all sessions and then get sessions by pipe and make sure everything
            # is found.
            all_sessions = [self.mirror.mirror_session_get_first(shdl, dt)]
            while True:
                try:
                    x = self.mirror.mirror_session_get_next(shdl, dt, all_sessions[-1].info.mir_id, all_sessions[-1].pipe_id)
                    all_sessions.append(x)
                except InvalidPipeMgrOperation as e:
                    break
            per_pipe_sessions = []
            for p in range(num_pipes):
                sess = [self.mirror.mirror_session_get_first(shdl, DevTarget_t(dev_id, p))]
                while True:
                    try:
                        x = self.mirror.mirror_session_get_next(shdl, DevTarget_t(dev_id, p), sess[-1].info.mir_id, sess[-1].pipe_id)
                        sess.append(x)
                    except InvalidPipeMgrOperation as e:
                        break
                per_pipe_sessions.append( list(sess) )
            self.assertEqual(num_pipes, len(per_pipe_sessions))
            for p in range(num_pipes):
                # All pipes should have the same number of sessions configured
                self.assertEqual( len(per_pipe_sessions[p]), len(per_pipe_sessions[0]) )
            # The total number of sessions queried per-pipe should equal the total
            # number of sessions queried using all-pipes.
            self.assertEqual(len(all_sessions), num_pipes * len(per_pipe_sessions[0]))
            all_session_sids = [x.info.mir_id for x in all_sessions]
            for s in all_session_sids:
                self.assertIn(s, self.sids)
            for p in range(num_pipes):
                pipe_sids = [x.info.mir_id for x in per_pipe_sessions[p]]
                self.assertEqual( sorted(self.sids), sorted(pipe_sids) )



        finally:
            entry_count = self.client.p0_get_entry_count(shdl, dt)
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                self.client.p0_table_delete(shdl, dev_id, hdl)
            for sid in self.sids:
                try:
                    self.mirror.mirror_session_delete(shdl, dt, sid)
                except InvalidPipeMgrOperation as e:
                    pass
                for p in range(num_pipes):
                    try:
                        self.mirror.mirror_session_delete(shdl, DevTarget_t(dev_id, p), sid)
                    except InvalidPipeMgrOperation as e:
                        pass
            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.client_cleanup(shdl)

class TestBasicEgrMir(pd_base_tests.ThriftInterfaceDataPlane):
    """
    Check that we can ingress mirror a packet out the port it came in on.
    """
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        setup_random()
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        shdl = self.conn_mgr.client_init()
        self.sids = []
        try:
            # Setup a session to mirror the packets out the ingress ports.
            # The original packets will be sent back out the ingress ports.
            sids = random.sample(range(BASE_SID_NORM, MAX_SID_NORM), len(swports))
            for port,sid in zip(swports, sids):
                p0_add(self, shdl, dt, port, egr_port=port, egr_mir=1, egr_sid=sid)
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                      Direction_e.PD_DIR_EGRESS,
                                      sid,
                                      port,
                                      True)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.sids.append(sid)
                logger.info("Using session {} for port {}".format(sid, port))
                sys.stdout.flush()
            self.conn_mgr.complete_operations(shdl)

            for l in range(79, 110):
                pkt = simple_eth_packet(pktlen=l)
                for port in swports:
                    send_packet(self, port, pkt)
                for port in swports:
                    verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Disable the sessions and check again
            for sid in self.sids:
                self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_EGRESS, dt, sid)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=80)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

            # Enable the sessions and check again
            for sid in self.sids:
                self.mirror.mirror_session_enable(shdl, Direction_e.PD_DIR_EGRESS, dt, sid)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=81)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Disable the sessions in both directions and check again
            for sid in self.sids:
                self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_BOTH, dt, sid)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=82)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

            # Enable the sessions and check again
            for sid in self.sids:
                self.mirror.mirror_session_enable(shdl, Direction_e.PD_DIR_EGRESS, dt, sid)
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=83)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Delete the sessions, add them again in a disabled state.
            while len(self.sids):
                sid = self.sids.pop(0)
                self.mirror.mirror_session_delete(shdl, dt, sid)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=89)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

        finally:
            entry_count = self.client.p0_get_entry_count(shdl, dt)
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                self.client.p0_table_delete(shdl, dev_id, hdl)
            for sid in self.sids:
                self.mirror.mirror_session_delete(shdl, dt, sid)
            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.client_cleanup(shdl)

class TestBatching(pd_base_tests.ThriftInterfaceDataPlane):
    """
    Check that we can ingress mirror a packet out the port it came in on.
    """
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        setup_random()
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        shdl = self.conn_mgr.client_init()
        shdl2 = self.conn_mgr.client_init()
        self.sids = []
        in_batch = None
        try:
            # Setup a session to mirror the packets out the ingress ports.
            # The original packets will be sent to a dummy port that drops them.
            sids = random.sample(range(BASE_SID_NORM, MAX_SID_NORM), len(swports))
            self.conn_mgr.begin_batch(shdl)
            in_batch = shdl
            for port,sid in zip(swports, sids):
                p0_add(self, shdl, dt, port, egr_port=511, ing_mir=1, ing_sid=sid)
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                      Direction_e.PD_DIR_INGRESS,
                                      sid,
                                      port,
                                      True)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.sids.append(sid)
                logger.info("Using session {} for port {}".format(sid, port))
                sys.stdout.flush()

            # Before ending the batch ensure another session cannot use the same
            # mirroring session ids.
            for sid in self.sids:
                try:
                    self.mirror.mirror_session_disable(shdl2, Direction_e.PD_DIR_INGRESS, dt, sid)
                    self.assertTrue(False)
                except InvalidPipeMgrOperation as e:
                    pass
            self.conn_mgr.end_batch(shdl, True)
            in_batch = None

            # Verify that the configuration is in place.
            pkt = simple_eth_packet(pktlen=100)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Disable the sessions with another session and check again
            for sid in self.sids:
                self.mirror.mirror_session_disable(shdl2, Direction_e.PD_DIR_INGRESS, dt, sid)
            self.conn_mgr.complete_operations(shdl2)

            pkt = simple_eth_packet(pktlen=110)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

            # Enable the sessions in a batch check again
            self.conn_mgr.begin_batch(shdl2)
            in_batch = shdl2
            for sid in self.sids:
                self.mirror.mirror_session_enable(shdl2, Direction_e.PD_DIR_INGRESS, dt, sid)
                try:
                    self.mirror.mirror_session_enable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
                    self.assertTrue(False)
                except InvalidPipeMgrOperation as e:
                    pass
            self.conn_mgr.end_batch(shdl2, True)
            in_batch = None

            pkt = simple_eth_packet(pktlen=120)
            for port in swports:
                send_packet(self, port, pkt)
            for port in swports:
                verify_packet(self, pkt, port)
            verify_no_other_packets(self)

            # Disable the sessions in a batch but don't push it, config will not be
            # applied.
            self.conn_mgr.begin_batch(shdl)
            in_batch = shdl
            for sid in self.sids:
                self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
            time.sleep(3)

            # End the batch and verify that the config is applied.
            self.conn_mgr.end_batch(shdl, True)
            in_batch = None
            pkt = simple_eth_packet(pktlen=140)
            for port in swports:
                send_packet(self, port, pkt)
            verify_no_other_packets(self)

        finally:
            if in_batch:
                self.conn_mgr.end_batch(in_batch, False)
            entry_count = self.client.p0_get_entry_count(shdl, dt)
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                self.client.p0_table_delete(shdl, dev_id, hdl)
            for sid in self.sids:
                self.mirror.mirror_session_delete(shdl, dt, sid)
            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.complete_operations(shdl2)
            self.conn_mgr.client_cleanup(shdl)
            self.conn_mgr.client_cleanup(shdl2)

class TestMCast(pd_base_tests.ThriftInterfaceDataPlane):
    """
    Check that we can ingress mirror a packet to a multicast group.
    """
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        setup_random()
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        shdl = self.conn_mgr.client_init()
        mc_shdl = self.mc.mc_create_session()
        sids = random.sample(range(BASE_SID_NORM,MAX_SID_NORM), 4)
        sid_to_ports = {}
        try:
            # All multicast
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[0],
                                  mcast_grp_a=hex_to_i16(0xAAAA),
                                  mcast_grp_a_v=True,
                                  mcast_grp_b=hex_to_i16(0x5555),
                                  mcast_grp_b_v=True,
                                  c2c=True)
            self.mirror.mirror_session_create(shdl, dt, info)
            # Only group 1
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[1],
                                  mcast_grp_a=hex_to_i16(0xAAAA),
                                  mcast_grp_a_v=True)
            self.mirror.mirror_session_create(shdl, dt, info)
            # Only group 2
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[2],
                                  mcast_grp_b=hex_to_i16(0x5555),
                                  mcast_grp_b_v=True)
            self.mirror.mirror_session_create(shdl, dt, info)
            # Only Copy-to-CPU
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[3],
                                  c2c=True)
            self.mirror.mirror_session_create(shdl, dt, info)

            mgrp_hdl_a = self.mc.mc_mgrp_create(mc_shdl, dev_id, hex_to_i16(0xAAAA))
            mgrp_hdl_b = self.mc.mc_mgrp_create(mc_shdl, dev_id, hex_to_i16(0x5555))
            # Group a will ues a port in each pipe
            ports_a = [swports_by_pipe[p][0] for p in swports_by_pipe]
            # Group b will use a random set of ports
            ports_b = random.sample(swports, 4)
            rid = 1
            logger.info("Multicast group 0xAAAA, ports {}".format(ports_a))
            logger.info("Multicast group 0x5555, ports {}".format(ports_b))
            node_hdl_a = self.mc.mc_node_create(mc_shdl, dev_id, rid, set_port_map(ports_a), set_lag_map([]))
            node_hdl_b = self.mc.mc_node_create(mc_shdl, dev_id, rid, set_port_map(ports_b), set_lag_map([]))
            self.mc.mc_associate_node(mc_shdl, dev_id, mgrp_hdl_a, node_hdl_a, hex_to_i16(0x8321), 1)
            self.mc.mc_associate_node(mc_shdl, dev_id, mgrp_hdl_b, node_hdl_b, 0, 0)
            self.tm.tm_set_cpuport(dev_id, swports[0])

            self.conn_mgr.complete_operations(shdl)
            self.mc.mc_complete_operations(mc_shdl)

            # Get the pipe vectors for each group
            v1 = self.mc.mc_get_pipe_vec(mc_shdl, dev_id, hex_to_i16(0xAAAA))
            v2 = self.mc.mc_get_pipe_vec(mc_shdl, dev_id, hex_to_i16(0x5555))
            v1_expected = 0
            for p in ports_a:
                v1_expected = v1_expected | (1 << port_to_pipe(p))
            v2_expected = 0
            for p in ports_b:
                v2_expected = v2_expected | (1 << port_to_pipe(p))
            self.assertEqual(v1, v1_expected)
            self.assertEqual(v2, v2_expected)

            # The first session will generate packets for each multicast group and
            # the CPU port
            sid_to_ports[sids[0]] = list(ports_a + ports_b + [swports[0]])
            # The second and third sessions generate copies to one multicast group.
            sid_to_ports[sids[1]] = list(ports_a)
            sid_to_ports[sids[2]] = list(ports_b)
            # The last session generates a copy to the CPU port
            sid_to_ports[sids[3]] = list([swports[0]])

            # Expect all pipe-masks to be the default
            if not g_is_tofino3:
                # For Tofino 3, there is no per-session pipe vector
                for sid in sids:
                    msk = self.mirror.mirror_session_pipe_vec_get(shdl, dt, sid)
                    self.assertEqual(msk, 0xF)

            # Send a packet to each session and verify copies are made.
            for sid in sids:
                p0_add(self, shdl, dt, swports[0], egr_port=511, ing_mir=1, ing_sid=sid)
                self.conn_mgr.complete_operations(shdl)
                pkt = simple_eth_packet(pktlen=1000)
                logger.info("Sending packet to session SID {}... ".format(sid))

                send_packet(self, swports[0], pkt)
                for p in sid_to_ports[sid]:
                    verify_packet(self, pkt, p)
                verify_no_other_packets(self)
                logger.info("Ok")

            # Change the pipe mask to one pipe at a time and make sure that only
            # packets for that pipe are sent.
            if not g_is_tofino3:
                logger.info("Change the pipe mask to one pipe at a time. Sending packets... ")
                for pipe in range(int(test_param_get('num_pipes'))):
                    msk = 1 << pipe
                    for sid in sids:
                        self.mirror.mirror_session_pipe_vec_set(shdl, dt, sid, msk)
                        m = self.mirror.mirror_session_pipe_vec_get(shdl, dt, sid)
                        self.assertEqual(msk, m)
                    # Send a packet to each session and verify copies are made only to the
                    # configured pipe.
                    for sid in sids:
                        p0_add(self, shdl, dt, swports[0], egr_port=511, ing_mir=1, ing_sid=sid)
                        self.conn_mgr.complete_operations(shdl)
                        pkt = simple_eth_packet(pktlen=1000)
                        send_packet(self, swports[0], pkt)
                        for p in sid_to_ports[sid]:
                            if pipe == port_to_pipe(p):
                                verify_packet(self, pkt, p)
                        verify_no_other_packets(self)
                logger.info("Ok")

                # Change the pipe mask back to all pipes.
                msk = 0
                for pipe in range(int(test_param_get('num_pipes'))):
                    msk = msk | (1 << pipe)
                for sid in sids:
                    self.mirror.mirror_session_pipe_vec_set(shdl, dt, sid, msk)
                    m = self.mirror.mirror_session_pipe_vec_get(shdl, dt, sid)
                    self.assertEqual(msk, m)

            # Modify the first session to use the first group.
            logger.info("Modifying session SID {} to use only multicast group 0xAAAA. Sending packets... ".format(sids[0]))
            self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sids[0])
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[0],
                                  mcast_grp_a=hex_to_i16(0xAAAA),
                                  mcast_grp_a_v=True)
            self.mirror.mirror_session_update(shdl, dt, info, True)

            # Send a packet and make sure it comes to all ports in the group.
            p0_add(self, shdl, dt, swports[0], egr_port=511, ing_mir=1, ing_sid=sids[0])
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=1000)
            send_packet(self, swports[0], pkt)
            for p in ports_a:
                verify_packet(self, pkt, p)
            verify_no_other_packets(self)
            logger.info("Ok")

            # Modify the session to use an XID that will prune the node, now no
            # copies should be made.
            logger.info("Modifying session SID {} to use XID that will prune the node. Sending packets... ".format(sids[0]))
            self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sids[0])
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[0],
                                  mcast_grp_a=hex_to_i16(0xAAAA),
                                  mcast_grp_a_v=True,
                                  mcast_l1_xid = hex_to_i16(0x8321))
            self.mirror.mirror_session_update(shdl, dt, info, True)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=1000)
            send_packet(self, swports[0], pkt)
            time.sleep(1)
            verify_no_other_packets(self)
            logger.info("Ok")

            # Modify the session to use an XID that will not prune the node, now
            # copies should be made again.
            logger.info("Modifying session SID {} to use XID that will not prune the node. Sending packets... ".format(sids[0]))
            self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sids[0])
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[0],
                                  mcast_grp_a=hex_to_i16(0xAAAA),
                                  mcast_grp_a_v=True,
                                  mcast_l1_xid = hex_to_i16(0xFFFF))
            self.mirror.mirror_session_update(shdl, dt, info, True)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=1000)
            send_packet(self, swports[0], pkt)
            for p in ports_a:
                verify_packet(self, pkt, p)
            verify_no_other_packets(self)
            logger.info("Ok")

            # Modify the session to use an RID that will cause L2 pruning.
            logger.info("Modifying session SID {} to use RID that will cause L2 pruning. Sending packets... ".format(sids[0]))
            self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sids[0])
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[0],
                                  mcast_grp_a=hex_to_i16(0xAAAA),
                                  mcast_grp_a_v=True,
                                  mcast_l1_xid = hex_to_i16(0xFFFF),
                                  mcast_l2_xid = 257,
                                  mcast_rid = hex_to_i16(rid))
            self.mc.mc_update_port_prune_table(mc_shdl, dev_id, 257, set_port_map(ports_a[:1]))
            self.mc.mc_complete_operations(mc_shdl)
            self.mirror.mirror_session_update(shdl, dt, info, True)
            self.conn_mgr.complete_operations(shdl)
            pkt = simple_eth_packet(pktlen=1000)
            send_packet(self, swports[0], pkt)
            for p in ports_a[1:]:
                verify_packet(self, pkt, p)
            verify_no_other_packets(self)
            logger.info("Ok")

            # Modify the session with a bad L2 Exclusion Id
            logger.info("Modifying session SID {} with a bad L2 Exclusion Id... ".format(sids[0]))
            self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sids[0])
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[0],
                                  mcast_grp_a=hex_to_i16(0xAAAA),
                                  mcast_grp_a_v=True,
                                  mcast_l1_xid = hex_to_i16(0xFFFF),
                                  mcast_l2_xid = hex_to_i16(0xFFFF),
                                  mcast_rid = hex_to_i16(rid))
            try:
                self.mirror.mirror_session_update(shdl, dt, info, True)
                self.assertTrue(False)
            except InvalidPipeMgrOperation as e:
                pass
            logger.info("Ok")
            # Modify the session with a bad port
            logger.info("Modifying session SID {} with a bad port... ".format(sids[0]))
            self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sids[0])
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                  Direction_e.PD_DIR_INGRESS,
                                  sids[0],
                                  egr_port=hex_to_i16(0xFFFF),
                                  egr_port_v=True)
            try:
                self.mirror.mirror_session_update(shdl, dt, info, True)
                self.assertTrue(False)
            except InvalidPipeMgrOperation as e:
                pass
            logger.info("Ok")


        finally:
            while self.mc.mc_mgrp_get_count(mc_shdl, dev_id):
                h = self.mc.mc_mgrp_get_first(mc_shdl, dev_id)
                self.mc.mc_mgrp_destroy(mc_shdl, dev_id, h)
            while self.mc.mc_node_get_count(mc_shdl, dev_id):
                h = self.mc.mc_node_get_first(mc_shdl, dev_id)
                self.mc.mc_node_destroy(mc_shdl, dev_id, h)
            for sid in sids:
                self.mirror.mirror_session_delete(shdl, dt, sid)
            self.mc.mc_update_port_prune_table(mc_shdl, dev_id, 257, set_port_map([]))
            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.client_cleanup(shdl)
            self.mc.mc_destroy_session(mc_shdl)

class TestIngMirMulticast(pd_base_tests.ThriftInterfaceDataPlane):
    """
    Check that we can change hash value to ingress mirror a packet out a different multicast port by lag/ecmp.
    """
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        setup_random()
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        shdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        sids = random.sample(range(BASE_SID_NORM, MAX_SID_NORM), len(swports))
        lag_id = 1
        node_rid = 1
        pkt_l1_xid = 0
        pkt_l2_xid = 0
        pkt_rid = 0
        self.sids = []
        try:
            #configure mc_tbl of mcid with two mc_nodes with 1/2 hash value
            mgrp_hdl = self.mc.mc_mgrp_create(mc_sess_hdl, dev_id, hex_to_i16(MCID1))
            members = [swports[2], swports[3]]
            port_map = set_port_map(members)
            self.mc.mc_set_lag_membership(mc_sess_hdl, dev_id, lag_id, port_map)
            lag_map = set_lag_map([lag_id])
            port_map = set_port_map([])
            l1_hdl = self.mc.mc_node_create(mc_sess_hdl, dev_id, node_rid, port_map, lag_map)
            self.mc.mc_associate_node(mc_sess_hdl, dev_id, mgrp_hdl, l1_hdl, 0, 0)
            self.mc.mc_complete_operations(mc_sess_hdl)
            #configure mirror session with multicast, mcid and hash value from session table(default)
            for port,sid in zip(swports[0:1], sids[0:1]):
                p0_add(self, shdl, dt, port, egr_port=511, ing_mir=1, ing_sid=sid)
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                    Direction_e.PD_DIR_INGRESS,
                                    sid,
                                    mcast_grp_a=MCID1,
                                    mcast_grp_a_v=True,
                                    level2_mcast_hash=HASH1)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.sids.append(sid)
                logger.info("Using session {} for port {}".format(sid, port))
                sys.stdout.flush()
            self.conn_mgr.complete_operations(shdl)

            pkt = simple_eth_packet(pktlen=80)
            lag_mbr = self.mc.mc_get_lag_member_from_hash(mc_sess_hdl, dev_id, l1_hdl, lag_id, HASH1, pkt_l1_xid, pkt_l2_xid, pkt_rid)
            for port in swports[0:1]:
                send_packet(self, port, pkt)
                time.sleep(1)
                verify_packet(self, pkt, lag_mbr.port)
                verify_no_other_packets(self)

            if not g_is_tofino:
                for sid in self.sids:
                    self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
                    # Configure hash from p4, which set to HASH2
                    self.mirror.mirror_session_meta_flag_update(shdl, dt, sid, MetaFlag_e.PD_HASH_CFG, 1)
                    self.mirror.mirror_session_enable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
                self.conn_mgr.complete_operations(shdl)
                pkt = simple_eth_packet(pktlen=80)
                lag_mbr = self.mc.mc_get_lag_member_from_hash(mc_sess_hdl, dev_id, l1_hdl, lag_id, HASH2, pkt_l1_xid, pkt_l2_xid, pkt_rid)
                for port in swports[0:1]:
                    send_packet(self, port, pkt)
                    time.sleep(1)
                    verify_packet(self, pkt, lag_mbr.port)
                    verify_no_other_packets(self)
                for sid in self.sids:
                    self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
                    # Configure mc_cfg_f to 1, use mc_ctrl to control multicast work or not.
                    self.mirror.mirror_session_meta_flag_update(shdl, dt, sid, MetaFlag_e.PD_MC_CFG, 1)
                    self.mirror.mirror_session_enable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
                self.conn_mgr.complete_operations(shdl)
                pkt = simple_eth_packet(pktlen=80)
                for port in swports[0:1]:
                    send_packet(self, port, pkt)
                    time.sleep(1)
                    verify_no_other_packets(self)
                for sid in self.sids:
                    self.mirror.mirror_session_disable(shdl, Direction_e.PD_DIR_INGRESS, dt, sid)
                self.conn_mgr.complete_operations(shdl)

        finally:
            while self.mc.mc_mgrp_get_count(mc_sess_hdl, dev_id):
                h = self.mc.mc_mgrp_get_first(mc_sess_hdl, dev_id)
                self.mc.mc_mgrp_destroy(mc_sess_hdl, dev_id, h)
            while self.mc.mc_node_get_count(mc_sess_hdl, dev_id):
                h = self.mc.mc_node_get_first(mc_sess_hdl, dev_id)
                self.mc.mc_node_destroy(mc_sess_hdl, dev_id, h)
            self.mc.mc_destroy_session(mc_sess_hdl)
            entry_count = self.client.p0_get_entry_count(shdl, dt)
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                self.client.p0_table_delete(shdl, dev_id, hdl)

            for sid in self.sids:
                self.mirror.mirror_session_delete(shdl, dt, sid)
            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.client_cleanup(shdl)


class TestMirInfoGet(pd_base_tests.ThriftInterfaceDataPlane):
    """
    Check Mirror info
    """
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        setup_random()
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        shdl = self.conn_mgr.client_init()
        self.sids = []
        try:
            sids = random.sample(range(BASE_SID_NORM, MAX_SID_NORM), len(swports))
            logger.info("Normal SIDs: {}".format(sids))
            sys.stdout.flush()
            mirror_infos = []
            for port,sid in zip(swports, sids):
                p0_add(self, shdl, dt, port, egr_port=511, ing_mir=1, ing_sid=sid)
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                      Direction_e.PD_DIR_INGRESS,
                                      sid,
                                      port,
                                      True)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.sids.append(sid)
                mirror_infos.append(info)
                logger.info("Using session {} for port {}".format(sid, port))
                sys.stdout.flush()
            self.conn_mgr.complete_operations(shdl)

            # Get first
            queried_mirror_infos = []
            first_info = self.mirror.mirror_session_get_first(shdl, dt)
            try:
                self.assertIn(first_info.info, mirror_infos)
            except AssertionError as e:
                logger.info("Queried session does not match created sessions.")
                logger.info("Queried:")
                logger.info(first_info)
                logger.info("Expected one of:")
                for i in mirror_infos:
                    logger.info(i)
                raise e
            queried_mirror_infos.append(first_info)

            # Get all next
            prev = first_info
            while True:
                try:
                    cur = self.mirror.mirror_session_get_next(shdl, dt, prev.info.mir_id, prev.pipe_id)
                except InvalidPipeMgrOperation as e:
                    break
                self.assertNotIn(cur, queried_mirror_infos)
                self.assertIn(cur.info, mirror_infos)
                queried_mirror_infos.append(cur)
                prev = cur

            # Got all of them, make sure they are the correct ones.
            self.assertEqual( len(queried_mirror_infos), len(mirror_infos) )
            for i in queried_mirror_infos:
                self.assertIn(i.info, mirror_infos)

            # Get the first, then delete it, then use it to get next.
            first = self.mirror.mirror_session_get_first(shdl, dt)
            self.assertEqual(first, queried_mirror_infos[0])
            self.mirror.mirror_session_delete(shdl, dt, first.info.mir_id)
            second = self.mirror.mirror_session_get_next(shdl, dt, first.info.mir_id, first.pipe_id)
            self.assertNotEqual(first, second)
            self.assertIn(second.info, mirror_infos)

            # Get first, get next, delete the one get-next returned, then use it to
            # get next again.
            first = self.mirror.mirror_session_get_first(shdl, dt)
            self.assertIn(first.info, mirror_infos)
            second = self.mirror.mirror_session_get_next(shdl, dt, first.info.mir_id, first.pipe_id)
            self.assertIn(second.info, mirror_infos)
            self.mirror.mirror_session_delete(shdl, dt, second.info.mir_id)
            third = self.mirror.mirror_session_get_next(shdl, dt, second.info.mir_id, second.pipe_id)
            self.assertIn(third.info, mirror_infos)
            self.assertNotEqual(second, third)

            # Clean up all sessions.
            while True:
                try:
                    x = self.mirror.mirror_session_get_first(shdl, dt)
                except InvalidPipeMgrOperation as e:
                    break
                self.assertIn(x.info, mirror_infos)
                self.mirror.mirror_session_delete(shdl, dt, x.info.mir_id)

            logger.info("Test max session_id get")
            sys.stdout.flush()
            max_sid = self.mirror.mirror_session_get_max_session_id(shdl, dev_id, MirrorType_e.PD_MIRROR_TYPE_NORM)
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                    Direction_e.PD_DIR_INGRESS,
                                    max_sid,
                                    swports[0],
                                    True)
            self.mirror.mirror_session_create(shdl, dt, info)
            self.conn_mgr.complete_operations(shdl)
            first = self.mirror.mirror_session_get_first(shdl, dt)
            self.assertEqual(first.info, info)
            self.mirror.mirror_session_delete(shdl, dt, first.info.mir_id)

            if not g_is_tofino:
                max_sid = self.mirror.mirror_session_get_max_session_id(shdl, dev_id, MirrorType_e.PD_MIRROR_TYPE_COAL)
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_COAL,
                                      Direction_e.PD_DIR_EGRESS,
                                      max_sid,
                                      swports[0],
                                      True)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.conn_mgr.complete_operations(shdl)
                next_s = self.mirror.mirror_session_get_first(shdl, dt)
                next_s_info = next_s.info
                self.assertTrue(next_s_info.mir_id == MAX_SID_COAL, "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.assertTrue(next_s_info.mir_type == MirrorType_e.PD_MIRROR_TYPE_COAL, "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.assertTrue(next_s_info.direction == Direction_e.PD_DIR_EGRESS, "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.assertTrue(next_s_info.egr_port == swports[0], "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.assertTrue(next_s_info.egr_port_v == True, "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.mirror.mirror_session_delete(shdl, dt, max_sid)

            logger.info("Test base session_id get")
            sys.stdout.flush()
            base_sid = self.mirror.mirror_session_get_base_session_id(shdl, dev_id, MirrorType_e.PD_MIRROR_TYPE_NORM)
            info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                    Direction_e.PD_DIR_INGRESS,
                                    base_sid,
                                    swports[0],
                                    True)
            self.mirror.mirror_session_create(shdl, dt, info)
            self.conn_mgr.complete_operations(shdl)
            first = self.mirror.mirror_session_get_first(shdl, dt)
            self.assertEqual(first.info, info)
            self.mirror.mirror_session_delete(shdl, dt, first.info.mir_id)

            if not g_is_tofino:
                base_sid = self.mirror.mirror_session_get_base_session_id(shdl, dev_id, MirrorType_e.PD_MIRROR_TYPE_COAL)
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_COAL,
                                      Direction_e.PD_DIR_INGRESS,
                                      base_sid,
                                      swports[0],
                                      True)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.conn_mgr.complete_operations(shdl)
                next_s = self.mirror.mirror_session_get_first(shdl, dt)
                next_s_info = next_s.info
                self.assertTrue(next_s_info.mir_id == BASE_SID_COAL, "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.assertTrue(next_s_info.mir_type == MirrorType_e.PD_MIRROR_TYPE_COAL, "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.assertTrue(next_s_info.direction == Direction_e.PD_DIR_INGRESS, "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.assertTrue(next_s_info.egr_port == swports[0], "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.assertTrue(next_s_info.egr_port_v == True, "Get first Mirror Session Info error: sid %d, mir_type %d, dir %d" % (next_s_info.mir_id, next_s_info.mir_type, next_s_info.direction))
                self.mirror.mirror_session_delete(shdl, dt, base_sid)

            logger.info("Test exception cases")
            sys.stdout.flush()
            try:
                next_s_info = self.mirror.mirror_session_get_first(shdl, dt)
                self.assertTrue(False)
            except InvalidPipeMgrOperation as e:
                pass

            if not g_is_tofino:
                possible_coal_sids = set( range(BASE_SID_NORM, MAX_SID_NORM) )
                used_normal_sids = set(sids)
                sids_coal = random.sample(possible_coal_sids - used_normal_sids, len(swports))
                logger.info("Coalescing SIDs: {}".format(sids_coal))
                sys.stdout.flush()
                logger.info("Creating coalescing session {}".format(sids_coal[0]))
                sys.stdout.flush()
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_COAL,
                                      Direction_e.PD_DIR_EGRESS,
                                      sids_coal[0],
                                      swports[0],
                                      True)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.conn_mgr.complete_operations(shdl)
                first_s = self.mirror.mirror_session_get_first(shdl, dt)
                first_s_info = first_s.info
                self.assertTrue(first_s_info.mir_id == sids_coal[0], "Get first Mirror Session Info error: sid %d, expected sid %d" % (first_s_info.mir_id, sids_coal[0]))
                try:
                    next_s_info = self.mirror.mirror_session_get_next(shdl, dt, first_s_info.mir_id, first_s.pipe_id)
                    self.assertTrue(False)
                except InvalidPipeMgrOperation as e:
                    pass

                self.mirror.mirror_session_delete(shdl, dt, sids_coal[0])

        finally:
            entry_count = self.client.p0_get_entry_count(shdl, dt)
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                self.client.p0_table_delete(shdl, dev_id, hdl)

            while True:
                try:
                    cur = self.mirror.mirror_session_get_first(shdl, dt)
                except InvalidPipeMgrOperation as e:
                    break
                self.mirror.mirror_session_delete(shdl, dt, cur.info.mir_id)

            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.client_cleanup(shdl)



class TestDMA(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def reg_verify(self):
        if g_is_tofino:
            base = 0x001000d0
            vals = [self.conn_mgr.reg_rd(dev_id, base + 4*a) for a in range(4)]
            pipes = [(x >> 5) & 3 for x in vals]
            stages = [(x >> 1) & 0xF for x in vals]
            for s in stages:
                if s >= 12 or s < 0:
                    log = ""
                    for v in vals:
                        log += "{},".format(hex(v))
                    logger.info(log)
                self.assertLess(s, 12)
                self.assertGreaterEqual(s, 0)
        elif g_is_tofino2:
            base = 0x00200130
            vals = [self.conn_mgr.reg_rd(dev_id, base + 4*a) for a in range(4)]
            pipes = [(x >> 7) & 3 for x in vals]
            stages = [(x >> 2) & 0x1F for x in vals]
            for s in stages:
                if s >= 20 or s < 0:
                    log = ""
                    for v in vals:
                        log += "{},".format(hex(v))
                    logger.info(log)
                self.assertLess(s, 20)
                self.assertGreaterEqual(s, 0)


    def runTest(self):
        dts = [ DevTarget_t(dev_id, hex_to_i16(0xFFFF)) ]
        for p in range(int(test_param_get('num_pipes'))):
            dts.append( DevTarget_t(dev_id, hex_to_i16(p)) )
        shdl = self.conn_mgr.client_init()
        try:
            self.conn_mgr.complete_operations(shdl)
            self.reg_verify()
            for port in swports:
                p0_add(self, shdl, dts[0], port)
            self.conn_mgr.complete_operations(shdl)
            self.reg_verify()
            for dt in dts:
                sid = 128
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                      Direction_e.PD_DIR_INGRESS,
                                      sid,
                                      swports[-1],
                                      True)
                self.mirror.mirror_session_create(shdl, dt, info)
                self.conn_mgr.complete_operations(shdl)
                try:
                    self.reg_verify()
                except AssertionError:
                    logger.info("Failed session create with dt: {}".format(dt))
                    raise
                self.mirror.mirror_session_delete(shdl, dt, sid)
                self.conn_mgr.complete_operations(shdl)
                try:
                    self.reg_verify()
                except AssertionError:
                    logger.info("Failed session delete with dt: {}".format(dt))
                    raise

            self.conn_mgr.complete_operations(shdl)
            self.reg_verify()
            entry_count = self.client.p0_get_entry_count(shdl, dts[0])
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dts[0])
                self.client.p0_table_delete(shdl, dev_id, hdl)
            self.conn_mgr.complete_operations(shdl)
            self.reg_verify()

        finally:
            entry_count = self.client.p0_get_entry_count(shdl, dts[0])
            for _ in range(entry_count):
                hdl = self.client.p0_get_first_entry_handle(shdl, dts[0])
                self.client.p0_table_delete(shdl, dev_id, hdl)

            while True:
                try:
                    cur = self.mirror.mirror_session_get_first(shdl, dts[0])
                except InvalidPipeMgrOperation as e:
                    break
                self.mirror.mirror_session_delete(shdl, dts[0], cur.info.mir_id)

            self.conn_mgr.complete_operations(shdl)
            self.conn_mgr.client_cleanup(shdl)



@group("thorough")
class TestFastReconfig(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["mirror_test"])

    def runTest(self):
        if not g_is_tofino3:
            setup_random()
            shdl = self.conn_mgr.client_init()
            self.sids = []
            num_pipes = int(test_param_get('num_pipes'))
            dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            dts = [DevTarget_t(dev_id, p) for p in range(num_pipes)]
            count = len(dts) + 1
            sids = random.sample(range(BASE_SID_NORM, MAX_SID_NORM), 2*count)
            sids_pre_warm_init = sids[:len(sids)//2]
            sids_post_warm_init = sids[len(sids)//2:]
            self.assertTrue( len(swports) >= len(sids_pre_warm_init) )
            warm_init_in_progress = False
            try:
                for i in [1,2,3]:
                    logger.info("{} Starting pass {}".format(datetime.datetime.now(), i))
                    for p in swports:
                        p0_add(self, shdl, dt, p, egr_port=511)

                    sid_to_ports = {}
                    all_ports = set(swports)
                    for sid in sids_pre_warm_init:
                        ig_port = all_ports.pop()
                        eg_port = random.choice(swports)
                        ig_pipe = port_to_pipe( ig_port )
                        p0_add(self, shdl, dt, ig_port, egr_port=511, ing_mir=1, ing_sid=sid)
                        info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                            Direction_e.PD_DIR_INGRESS,
                                            sid,
                                            eg_port,
                                            True)
                        if 0 == random.choice([0,1]):
                            logger.info("Session {} mirrors port {} to port {} on all pipes".format(sid, ig_port, eg_port))
                            self.mirror.mirror_session_create(shdl, dt, info)
                        else:
                            logger.info("Session {} mirrors port {} to port {} on pipe {}".format(sid, ig_port, eg_port, ig_pipe))
                            self.mirror.mirror_session_create(shdl, dts[ig_pipe], info)
                        sys.stdout.flush()
                        sid_to_ports[sid] = (ig_port, eg_port)
                    self.conn_mgr.complete_operations(shdl)

                    # Make sure mirroring works
                    logger.info("{} Verifying config".format(datetime.datetime.now()))
                    for sid in sids_pre_warm_init:
                        pkt = simple_eth_packet(pktlen=100)
                        ig_port,eg_port = sid_to_ports[sid]
                        send_packet(self, ig_port, pkt)
                        verify_packet(self, pkt, eg_port)

                    logger.info("{} Starting warm init".format(datetime.datetime.now()))
                    self.devport_mgr.devport_mgr_warm_init_begin(dev_id, dev_init_mode.DEV_WARM_INIT_FAST_RECFG, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, True)
                    warm_init_in_progress = True

                    pal_utils.add_ports(self)

                    # Make sure mirroring works
                    logger.info("{} Verifying config".format(datetime.datetime.now()))
                    for sid in sids_pre_warm_init:
                        pkt = simple_eth_packet(pktlen=100)
                        ig_port,eg_port = sid_to_ports[sid]
                        send_packet(self, ig_port, pkt)
                        verify_packet(self, pkt, eg_port)

                    # Replay new configuration
                    logger.info("{} Replaying new config".format(datetime.datetime.now()))
                    for p in swports:
                        p0_add(self, shdl, dt, p, egr_port=511)
                        all_ports = set(swports)
                    for sid in sids_post_warm_init:
                        ig_port = all_ports.pop()
                        eg_port = random.choice(swports)
                        ig_pipe = port_to_pipe(ig_port)
                        p0_add(self, shdl, dt, ig_port, egr_port=511, ing_mir=1, ing_sid=sid)
                        info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                            Direction_e.PD_DIR_INGRESS,
                                            sid,
                                            eg_port,
                                            True)
                        if 0 == random.choice([0,1]):
                            logger.info("Session {} mirrors port {} to port {} on all pipes".format(sid, ig_port, eg_port))
                            self.mirror.mirror_session_create(shdl, dt, info)
                        else:
                            logger.info("Session {} mirrors port {} to port {} on pipe {}".format(sid, ig_port, eg_port, ig_pipe))
                            self.mirror.mirror_session_create(shdl, dts[ig_pipe], info)
                        sys.stdout.flush()
                        sid_to_ports[sid] = (ig_port, eg_port)

                    # Pick another session and configure it many times, we should not run out
                    # of DMA memory.
                    logger.info("{} Reconfiguring one session".format(datetime.datetime.now()))
                    all_sids = set( range(BASE_SID_NORM, MAX_SID_NORM) )
                    free_sids = all_sids - set(sids)
                    free_sid = free_sids.pop()
                    info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                        Direction_e.PD_DIR_INGRESS,
                                        free_sid,
                                        0,
                                        True)
                    for _ in range(1000):
                        for d in [dt]+dts:
                            self.mirror.mirror_session_create(shdl, d, info)
                            self.mirror.mirror_session_delete(shdl, d, free_sid)

                    # Make sure the original config still works
                    logger.info("{} Verifying original config again".format(datetime.datetime.now()))
                    for sid in sids_pre_warm_init:
                        pkt = simple_eth_packet(pktlen=100)
                        ig_port,eg_port = sid_to_ports[sid]
                        send_packet(self, ig_port, pkt)
                        verify_packet(self, pkt, eg_port)

                    logger.info("{} Calling warm init end".format(datetime.datetime.now()))
                    self.devport_mgr.devport_mgr_warm_init_end(dev_id)
                    warm_init_in_progress = False
                    if test_param_get('target') == "hw":
                        pal_utils.check_port_status(self, swports)

                    # Now the new configuration should be in effect
                    logger.info("{} Verifying new config".format(datetime.datetime.now()))
                    for sid in sids_post_warm_init:
                        pkt = simple_eth_packet(pktlen=100)
                        ig_port,eg_port = sid_to_ports[sid]
                        send_packet(self, ig_port, pkt)
                        verify_packet(self, pkt, eg_port)

                    # Clean up
                    logger.info("{} Cleaning up".format(datetime.datetime.now()))
                    entry_count = self.client.p0_get_entry_count(shdl, dt)
                    for _ in range(entry_count):
                        hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                        self.client.p0_table_delete(shdl, dev_id, hdl)
                    while True:
                        try:
                            x = self.mirror.mirror_session_get_first(shdl, dt)
                        except InvalidPipeMgrOperation as e:
                            break
                        self.mirror.mirror_session_delete(shdl, DevTarget_t(dev_id,x.pipe_id), x.info.mir_id)

            finally:
                if warm_init_in_progress:
                    self.devport_mgr.devport_mgr_warm_init_end(dev_id)
                entry_count = self.client.p0_get_entry_count(shdl, dt)
                for _ in range(entry_count):
                    hdl = self.client.p0_get_first_entry_handle(shdl, dt)
                    self.client.p0_table_delete(shdl, dev_id, hdl)
                while True:
                    try:
                        x = self.mirror.mirror_session_get_first(shdl, dt)
                    except InvalidPipeMgrOperation as e:
                        break
                    self.mirror.mirror_session_delete(shdl, DevTarget_t(dev_id,x.pipe_id), x.info.mir_id)
        else:
            logger.info("")
            logger.info("Test not yet supported on Tofino 3")
