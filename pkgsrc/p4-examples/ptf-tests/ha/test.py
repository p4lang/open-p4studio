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
import sys

import pd_base_tests

from ptf import config
from ptf.testutils import *
from ptf.thriftutils import *
import p4testutils.misc_utils as misc_utils
import p4testutils.pal_utils as pal_utils

import os

from ha.p4_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *
from pal_rpc.ttypes import *

import random

dev_id = 0

logger = misc_utils.get_logger()
swports = misc_utils.get_sw_ports()


def clean_up_tables(test, sess_hdl, dev_tgt):
    while test.client.port_tbl_get_entry_count(sess_hdl, dev_tgt):
        h = test.client.port_tbl_get_first_entry_handle(sess_hdl, dev_tgt)
        test.client.port_tbl_table_delete(sess_hdl, dev_id, h)
    test.client.port_tbl_table_reset_default_entry(sess_hdl, dev_tgt)
    while test.client.set_eg_get_entry_count(sess_hdl, dev_tgt):
        h = test.client.set_eg_get_first_entry_handle(sess_hdl, dev_tgt)
        test.client.set_eg_table_delete(sess_hdl, dev_id, h)
    test.client.set_eg_table_reset_default_entry(sess_hdl, dev_tgt)
    while test.client.hash_action_ha_exm_get_entry_count(sess_hdl, dev_tgt):
        h = test.client.hash_action_ha_exm_get_first_entry_handle(sess_hdl, dev_tgt)
        test.client.hash_action_ha_exm_table_delete(sess_hdl, dev_id, h)
    test.client.hash_action_ha_exm_table_reset_default_entry(sess_hdl, dev_tgt)
    while test.client.tcam_range_get_entry_count(sess_hdl, dev_tgt):
        h = test.client.tcam_range_get_first_entry_handle(sess_hdl, dev_tgt)
        test.client.tcam_range_table_delete(sess_hdl, dev_id, h)
    test.client.tcam_range_table_reset_default_entry(sess_hdl, dev_tgt)

class TestPipelineTable(pd_base_tests.EmptyTableTest):
    program = "ha"
    table = "port_tbl"


@group("thorough")
class TestExmHashActionHA(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["ha"])
        self.match_spec_arr = []
        self.action_spec_arr = []
        self.ha_entry_hdl_arr = []
        self.match_spec_dict = {}
        self.action_spec_dict = {}
        self.entry_idx_to_replay_dict = {}
        self.act_vid_dict = {}
        self.act_ttl_dict = {}
        self.act_pcp_dict = {}
        self.new_additional_entry_idx_replay_dict = {}

    def addEntries(self, sess_hdl, dev_tgt, maxEntries, def_vid, def_pcp, def_ttl):

        mat_ttl_dict = {}
        self.act_vid_dict[def_vid] = True
        self.act_pcp_dict[def_pcp] = True
        self.act_ttl_dict[def_ttl] = True
        numEntries = len(self.match_spec_arr)
        for i in range(maxEntries):
            match_spec = ha_hash_action_ha_exm_match_spec_t(3, 3, 1, 1)
            match_spec_str = "{}.{}.1.1".format(match_spec.ipv4_ttl, match_spec.vlan_tag_pri)
            while match_spec_str in self.match_spec_dict:
                mat_ttl = random.randint(1, 127)
                while mat_ttl in mat_ttl_dict:
                    mat_ttl = random.randint(1, 127)
                mat_ttl_dict[mat_ttl] = True

                mat_pcp = random.randint(1, 7)

                match_spec = ha_hash_action_ha_exm_match_spec_t(mat_ttl, mat_pcp, 1, 1)
                match_spec_str = "{}.{}.1.1".format(mat_ttl, mat_pcp)
            self.match_spec_arr.append(match_spec)
            self.match_spec_dict[match_spec_str] = True

            act_vid = random.randint(10, 2047)
            while act_vid in self.act_vid_dict:
                act_vid = random.randint(10, 2047)
            self.act_vid_dict[act_vid] = True

            act_pcp = random.randint(1, 7)
            while act_pcp in self.act_pcp_dict:
                act_pcp = random.randint(1, 7)

            act_ttl = random.randint(1, 127)
            while act_ttl in self.act_ttl_dict:
                act_ttl = random.randint(1, 127)
            self.act_ttl_dict[act_ttl] = True

            action_spec = ha_hash_action_ha_action_spec_t(act_vid, act_pcp, act_ttl)
            action_spec_str = "{}.{}.{}".format(act_vid, act_pcp, act_ttl)
            self.action_spec_arr.append(action_spec)
            self.action_spec_dict[action_spec_str] = True
            ha_entry_hdl = self.client.hash_action_ha_exm_table_add_with_hash_action_ha(sess_hdl,
                                                                                      dev_tgt,
                                                                                      match_spec,
                                                                                      action_spec)
            logger.info("Entry {} : Mat ttl : {} Mat pcp : {} : Act vid : {} : Act pcp : {} : Act ttl : {} "
                        .format(i + numEntries, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2))
            if numEntries != 0: # indicates that the entry is being added during cfg replay
                self.new_additional_entry_idx_replay_dict[i + numEntries] = True
            self.conn_mgr.complete_operations(sess_hdl)

    def replayRandomSetOfSameEntries(self, sess_hdl, dev_tgt, numEntries):
        maxEntries = len(self.match_spec_arr)
        if maxEntries < numEntries:
            assert 0

        for i in range (numEntries):
            entry_idx = random.randint(0, maxEntries-1)
            while entry_idx in self.entry_idx_to_replay_dict:
                entry_idx = random.randint(0, maxEntries-1)
            self.entry_idx_to_replay_dict[entry_idx] = True
            # Replay the same entries
            match_spec = self.match_spec_arr[entry_idx]
            action_spec = self.action_spec_arr[entry_idx]
            ha_entry_hdl = self.client.hash_action_ha_exm_table_add_with_hash_action_ha(sess_hdl,
                                                                                       dev_tgt,
                                                                                       match_spec,
                                                                                       action_spec)
            self.ha_entry_hdl_arr.append(ha_entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)
            logger.info("Same Entry Replayed {} : Mat ttl : {} Mat pcp : {} : Act vid : {} : Act pcp : {} : Act ttl : {} "
                        .format(entry_idx, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2))

    def replayRandomSetOfUpdatedEntries(self, sess_hdl, dev_tgt, numEntries):
        maxEntries = len(self.match_spec_arr)
        if maxEntries < numEntries:
            assert 0

        for i in range (numEntries):
            entry_idx = random.randint(0, maxEntries-1)
            while entry_idx in self.entry_idx_to_replay_dict:
                entry_idx = random.randint(0, maxEntries-1)
            self.entry_idx_to_replay_dict[entry_idx] = True
            # Replay the same match spec with different action specs
            match_spec = self.match_spec_arr[entry_idx]
            action_spec = self.action_spec_arr[entry_idx]
            action_spec_str = "{}.{}.{}".format(action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2)
            while action_spec_str in self.action_spec_dict:
                act_vid = random.randint(10, 2047)
                while act_vid in self.act_vid_dict:
                    act_vid = random.randint(10, 2047)
                self.act_vid_dict[act_vid] = True

                act_pcp = random.randint(1, 7)
                while act_pcp in self.act_pcp_dict:
                    act_pcp = random.randint(1, 7)

                act_ttl = random.randint(1, 127)
                while act_ttl in self.act_ttl_dict:
                    act_ttl = random.randint(1, 127)
                self.act_ttl_dict[act_ttl] = True

                action_spec.action_value_0 = act_vid
                action_spec.action_value_1 = act_pcp
                action_spec.action_value_2 = act_ttl
                action_spec_str = "{}.{}.{}".format(act_vid, act_pcp, act_ttl)
            self.action_spec_dict[action_spec_str] = True
            ha_entry_hdl = self.client.hash_action_ha_exm_table_add_with_hash_action_ha(sess_hdl,
                                                                                       dev_tgt,
                                                                                       match_spec,
                                                                                       action_spec)
            self.ha_entry_hdl_arr.append(ha_entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)
            logger.info("Updt Entry Replayed {} : Mat ttl : {} Mat pcp : {} : Act vid : {} : Act pcp : {} : Act ttl : {} "
                        .format(entry_idx, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2))

    def verifyEntry(self, src_port, dst_port, mat_ttl, mat_pcp, act_vid, act_pcp, act_ttl):
        pkt = simple_tcp_packet(eth_dst='00:24:68:AC:DF:56',
                            eth_src='00:22:22:22:22:22',
                            dl_vlan_enable=True,
                            vlan_vid=10,
                            vlan_pcp=mat_pcp,
                            ip_src='1.2.3.4',
                            ip_dst='5.6.7.8',
                            ip_id=101,
                            ip_ttl=mat_ttl,
                            tcp_sport = 9000,
                            with_tcp_chksum=False)
        exp_pkt = simple_tcp_packet(eth_dst='00:24:68:AC:DF:56',
                                eth_src='00:22:22:22:22:22',
                                dl_vlan_enable=True,
                                vlan_vid=act_vid,
                                vlan_pcp=act_pcp,
                                ip_src='1.2.3.4',
                                ip_dst='5.6.7.8',
                                ip_id=101,
                                ip_ttl=act_ttl,
                                tcp_sport = 9000,
                                with_tcp_chksum=False)

        send_packet(self, src_port, pkt)
        verify_packet(self, exp_pkt, dst_port)

    def verifyAllAddedEntries(self, src_port, dst_port):
        maxEntries = len(self.match_spec_arr)
        logger.info("Total entries are {}".format(maxEntries))
        for i in range(maxEntries):
            match_spec = self.match_spec_arr[i]
            action_spec = self.action_spec_arr[i]
            self.verifyEntry(src_port, dst_port, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2)
            logger.info("Entry Verified {}".format(i))

    def verifyEntriesAfterHA(self, src_port, dst_port, def_vid, def_pcp, def_ttl):
        maxEntries = len(self.match_spec_arr)
        for i in range(maxEntries):
            # Only the entries that were replayed should hit
            match_spec = self.match_spec_arr[i]
            action_spec = self.action_spec_arr[i]
            if i in self.entry_idx_to_replay_dict:
                logger.info("Verifying non-default entry (replayed) {} : Mat ttl : {} Mat pcp : {} : Act vid : {} : Act pcp : {} : Act ttl : {} "
                            .format(i, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2))
                self.verifyEntry(src_port, dst_port, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2)
            elif i in self.new_additional_entry_idx_replay_dict:
                logger.info("Verifying additional entry  (replayed) {} : Mat ttl : {} Mat pcp : {} : Act vid : {} : Act pcp : {} : Act ttl : {} "
                            .format(i, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2))
                self.verifyEntry(src_port, dst_port, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, action_spec.action_value_0, action_spec.action_value_1, action_spec.action_value_2)
            else:
                #Pass the default action values
                logger.info("Verifying default entry (non-replayed) {} : Mat ttl : {} Mat pcp : {} : Act vid : {} : Act pcp : {} : Act ttl : {} "
                            .format(i, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, def_vid, def_pcp, def_ttl))
                self.verifyEntry(src_port, dst_port, match_spec.ipv4_ttl, match_spec.vlan_tag_pri, def_vid, def_pcp, def_ttl)

    def runTest(self):
        misc_utils.setup_random()
        sess_hdl = self.conn_mgr.client_init()

        maxEntries = 25
        replayedSameEntries = 10
        replayedUpdatedEntries = 10
        additionalEntries = 10
        src_port = swports[1]
        dst_port = swports[2]
        dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))
        def_vid = 1947
        def_pcp = 5
        def_ttl = 45

        action_spec = ha_set_egr_port_action_spec_t(hex_to_i16(dst_port))
        entry_hdl = self.client.set_eg_set_default_action_set_egr_port(sess_hdl, dev_tgt, action_spec)
        self.conn_mgr.complete_operations(sess_hdl)
        try:
            # We are matching on vlan_pcp, ip_ttl, ipv4 header validity and ethernet header validity
            self.addEntries(sess_hdl, dev_tgt, maxEntries, def_vid, def_pcp, def_ttl)

            self.verifyAllAddedEntries(src_port, dst_port)

            self.devport_mgr.devport_mgr_warm_init_begin(dev_id, dev_init_mode.DEV_WARM_INIT_HITLESS, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, True)
            pal_utils.add_ports(self)

            # Replay a random set of same entries
            self.replayRandomSetOfSameEntries(sess_hdl, dev_tgt, replayedSameEntries)

            # Replay a random set of updated entries
            self.replayRandomSetOfUpdatedEntries(sess_hdl, dev_tgt, replayedUpdatedEntries)

            # Replay a random set of additional new entries
            self.addEntries(sess_hdl, dev_tgt, additionalEntries, def_vid, def_pcp, def_ttl)

            # Replay default entry
            entry_hdl = self.client.set_eg_set_default_action_set_egr_port(sess_hdl, dev_tgt, action_spec)

            self.devport_mgr.devport_mgr_warm_init_end(dev_id)

            self.conn_mgr.complete_operations(sess_hdl)

            if test_param_get('target') == "hw":
                pal_utils.check_port_status(self, swports)

            self.verifyEntriesAfterHA(src_port, dst_port, def_vid, def_pcp, def_ttl)

            ha_report = self.client.hash_action_ha_exm_get_ha_reconciliation_report(sess_hdl, dev_tgt)
            logger.info("{}".format(ha_report))
            total_entries = 8192
            total_modified = additionalEntries + replayedUpdatedEntries
            total_deleted = total_entries - total_modified - replayedSameEntries
            self.assertEqual(ha_report.num_entries_added, 0)
            self.assertEqual(ha_report.num_entries_deleted, total_deleted)
            self.assertEqual(ha_report.num_entries_modified, total_modified)
            verify_no_other_packets(self)

        finally:
            clean_up_tables(self, sess_hdl, dev_tgt)

            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.client_cleanup(sess_hdl)

@group("thorough")
class Phase0HA(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["ha"])
        self.match_spec_arr = []
        self.action_spec_arr = []
        self.ha_entry_hdl_arr = []
        self.aux_entry_hdl_arr = []
        self.match_spec_dict = {}
        self.action_spec_dict = {}
        self.entry_idx_to_replay_dict = {}
        self.act_exc_id_dict = {}
        self.act_qid_dict = {}
        self.act_rid_dict = {}
        self.act_mcast_hash_dict = {}
        self.new_additional_entry_idx_replay_dict = {}

    def addEntries(self, sess_hdl, dev_tgt, maxEntries):
        numEntries = len(self.match_spec_arr)
        for i in range(maxEntries):
            port_idx = random.randint(0, len(swports)-1)
            match_spec = ha_port_tbl_match_spec_t(swports[port_idx])
            match_spec_str = "{}".format(match_spec.ig_intr_md_ingress_port)
            while match_spec_str in self.match_spec_dict:
                port_idx = random.randint(0, len(swports)-1)
                match_spec = ha_port_tbl_match_spec_t(swports[port_idx])
                match_spec_str = "{}".format(match_spec.ig_intr_md_ingress_port)
            self.match_spec_arr.append(match_spec)
            self.match_spec_dict[match_spec_str] = True

            exc_id = random.randint(0, 511)
            while exc_id in self.act_exc_id_dict:
                exc_id = random.randint(0, 511)
            self.act_exc_id_dict[exc_id] = True

            qid = random.randint(0, 31)
            while qid in self.act_qid_dict:
                qid = random.randint(0, 31)
            self.act_qid_dict[qid] = True

            cos = random.randint(0, 7)

            rid = random.randint(0, 32767)
            while rid in self.act_rid_dict:
                rid = random.randint(0, 32767)
            self.act_rid_dict[rid] = True

            mcast_hash = random.randint(0, 8191)
            while mcast_hash in self.act_mcast_hash_dict:
                mcast_hash = random.randint(0, 8191)
            self.act_mcast_hash_dict[mcast_hash] = True

            action_spec = ha_set_md_action_spec_t(exc_id, qid, cos, rid, mcast_hash)
            action_spec_str = "{}.{}.{}.{}.{}".format(exc_id, qid, cos, rid, mcast_hash)
            self.action_spec_arr.append(action_spec)
            self.action_spec_dict[action_spec_str] = True
            ha_entry_hdl = self.client.port_tbl_table_add_with_set_md(sess_hdl, dev_tgt, match_spec, action_spec)
            logger.info("Entry {} : Mat Port : {} : Act exc_id : {} : Act qid : {} : Act cos : {} : Act rid : {} : Act mcast hash : {}"
                        .format(i + numEntries, match_spec.ig_intr_md_ingress_port, action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid, action_spec.action_level1_mcast_hash))


            if numEntries != 0: # indicates that the entry is being added during cfg replay
                self.new_additional_entry_idx_replay_dict[i + numEntries] = True
                self.ha_entry_hdl_arr.append(ha_entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)

    def addEntriesAuxTable(self, sess_hdl, dev_tgt):
        # clear the aux table entry hdl array
        self.aux_entry_hdl_arr = []
        numEntries = len(self.match_spec_arr)
        for i in range(numEntries):
            match_spec = self.match_spec_arr[i]
            action_spec = self.action_spec_arr[i]
            # Now add the entry in the other table
            match_spec_1 = ha_set_eg_match_spec_t(action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid, action_spec.action_level1_mcast_hash)
            action_spec_1 = ha_set_egr_port_action_spec_t(match_spec.ig_intr_md_ingress_port)
            hdl = self.client.set_eg_table_add_with_set_egr_port(sess_hdl, dev_tgt, match_spec_1, action_spec_1)
            self.aux_entry_hdl_arr.append(hdl)

    def replayRandomSetOfSameEntries(self, sess_hdl, dev_tgt, numEntries):
        maxEntries = len(self.match_spec_arr)
        if maxEntries < numEntries:
            assert 0

        for i in range (numEntries):
            entry_idx = random.randint(0, maxEntries-1)
            while entry_idx in self.entry_idx_to_replay_dict:
                entry_idx = random.randint(0, maxEntries-1)
            self.entry_idx_to_replay_dict[entry_idx] = True
            # Replay the same entries
            match_spec = self.match_spec_arr[entry_idx]
            action_spec = self.action_spec_arr[entry_idx]
            ha_entry_hdl = self.client.port_tbl_table_add_with_set_md(sess_hdl, dev_tgt, match_spec, action_spec)
            self.ha_entry_hdl_arr.append(ha_entry_hdl)
            logger.info("Same Entry Replayed {} : Mat Port : {} : Act exc_id : {} : Act qid : {} : Act cos : {} : Act rid : {} : Act mcast hash : {}"
                        .format(entry_idx, match_spec.ig_intr_md_ingress_port, action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid, action_spec.action_level1_mcast_hash))

    def replayRandomSetOfUpdatedEntries(self, sess_hdl, dev_tgt, numEntries):
        maxEntries = len(self.match_spec_arr)
        if maxEntries < numEntries:
            assert 0

        for i in range (numEntries):
            entry_idx = random.randint(0, maxEntries-1)
            while entry_idx in self.entry_idx_to_replay_dict:
                entry_idx = random.randint(0, maxEntries-1)
            self.entry_idx_to_replay_dict[entry_idx] = True
            # Replay the same match spec with different action spec
            match_spec = self.match_spec_arr[entry_idx]
            action_spec = self.action_spec_arr[entry_idx]
            action_spec_str = "{}.{}.{}.{}.{}".format(action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid, action_spec.action_level1_mcast_hash)

            while action_spec_str in self.action_spec_dict:
                exc_id = random.randint(0, 511)
                while exc_id in self.act_exc_id_dict:
                    exc_id = random.randint(0, 511)
                self.act_exc_id_dict[exc_id] = True

                qid = random.randint(0, 31)
                while qid in self.act_qid_dict:
                    qid = random.randint(0, 31)
                self.act_qid_dict[qid] = True

                cos = random.randint(0, 7)

                rid = random.randint(0, 32767)
                while rid in self.act_rid_dict:
                    rid = random.randint(0, 32767)
                self.act_rid_dict[rid] = True

                mcast_hash = random.randint(0, 8191)
                while mcast_hash in self.act_mcast_hash_dict:
                    mcast_hash = random.randint(0, 8191)
                self.act_mcast_hash_dict[mcast_hash] = True

                action_spec_str = "{}.{}.{}.{}.{}".format(exc_id, qid, cos, rid, mcast_hash)

            action_spec.action_level2_exclusion_id = exc_id
            action_spec.action_qid = qid
            action_spec.action_cos = cos
            action_spec.action_rid = rid
            action_spec.action_level1_mcast_hash = mcast_hash
            self.action_spec_dict[action_spec_str] = True
            ha_entry_hdl = self.client.port_tbl_table_add_with_set_md(sess_hdl, dev_tgt, match_spec, action_spec)
            self.ha_entry_hdl_arr.append(ha_entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)
            logger.info("Updt Entry Replayed {} : Mat Port : {} : Act exc_id : {} : Act qid : {} : Act cos : {} : Act rid : {} : Act mcast hash : {}"
                        .format(entry_idx, match_spec.ig_intr_md_ingress_port, action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid, action_spec.action_level1_mcast_hash))

    def verifyEntry(self, src_port, dst_port):
        pkt = simple_tcp_packet(eth_dst='00:24:68:AC:DF:56',
                            eth_src='00:22:22:22:22:22',
                            dl_vlan_enable=True,
                            vlan_vid=1947,
                            vlan_pcp=5,
                            ip_src='1.2.3.4',
                            ip_dst='5.6.7.8',
                            ip_id=101,
                            ip_ttl=45,
                            tcp_sport = 9000,
                            with_tcp_chksum=False)
        exp_pkt = simple_tcp_packet(eth_dst='00:24:68:AC:DF:56',
                                eth_src='00:22:22:22:22:22',
                                dl_vlan_enable=True,
                                vlan_vid=1947,
                                vlan_pcp=5,
                                ip_src='1.2.3.4',
                                ip_dst='5.6.7.8',
                                ip_id=101,
                                ip_ttl=45,
                                tcp_sport = 9000,
                                with_tcp_chksum=False)

        send_packet(self, src_port, pkt)
        verify_packet(self, exp_pkt, dst_port)

    def verifyAllAddedEntries(self):
        maxEntries = len(self.match_spec_arr)
        logger.info("Total entries are {}".format(maxEntries))
        for i in range (maxEntries):
            match_spec = self.match_spec_arr[i]
            action_spec = self.action_spec_arr[i]
            self.verifyEntry(match_spec.ig_intr_md_ingress_port, match_spec.ig_intr_md_ingress_port)
            logger.info("Entry Verified {}".format(i))

    def verifyEntriesAfterHA(self):
        maxEntries = len(self.match_spec_arr)
        logger.info("Total entries are {}".format(maxEntries))
        test_failed = 0
        for i in range (maxEntries):
            # Only entries that were replayed should hit
            match_spec = self.match_spec_arr[i]
            action_spec = self.action_spec_arr[i]
            if i in self.entry_idx_to_replay_dict:
                # These entries were re-added as it is, hence they should hit
                self.verifyEntry(match_spec.ig_intr_md_ingress_port, match_spec.ig_intr_md_ingress_port)
                logger.info("Entry Replayed Verified {} : Mat Port : {} : Act exc_id : {} : Act qid : {} : Act cos : {} : Act rid : {} : Act mcast hash : {}"
                            .format(i, match_spec.ig_intr_md_ingress_port, action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid, action_spec.action_level1_mcast_hash))
            elif i in self.new_additional_entry_idx_replay_dict:
                # These are the additional entries that were replayed. These should hit too
                self.verifyEntry(match_spec.ig_intr_md_ingress_port, match_spec.ig_intr_md_ingress_port)
                logger.info("Additional Entry Replayed Verified {} : Mat Port : {} : Act exc_id : {} : Act qid : {} : Act cos : {} : Act rid : {} : Act mcast hash : {}"
                            .format(i, match_spec.ig_intr_md_ingress_port, action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid, action_spec.action_level1_mcast_hash))
            else:
                # These entries were not replayed. Hence they should miss and cause a packet drop
                try:
                    self.verifyEntry(match_spec.ig_intr_md_ingress_port, match_spec.ig_intr_md_ingress_port)
                    # If it comes to this point, it means that the entry actually hit which implies that the entry was never deleted. Hence assert
                    logger.info("Entry {} : Was not deleted : Mat Port : {} : Act exc_id : {} : Act qid : {} : Act cos : {} : Act rid : {} : Act mcast hash : {}"
                                .format(i, match_spec.ig_intr_md_ingress_port, action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid,action_spec.action_level1_mcast_hash))
                    test_failed = 1
                    assert(0)
                except:
                    if test_failed == 1:
                        assert(0)
                    else:
                        logger.info("Deleted Entry Verified {} : Mat Port : {} : Act exc_id : {} : Act qid : {} : Act cos : {} : Act rid : {} : Act mcast hash : {}"
                                    .format(i, match_spec.ig_intr_md_ingress_port, action_spec.action_level2_exclusion_id, action_spec.action_qid, action_spec.action_cos, action_spec.action_rid,action_spec.action_level1_mcast_hash))

    def runTest(self):
        misc_utils.setup_random()
        num_pipes = int(test_param_get('num_pipes'))
        logger.info("Num pipes is {}".format(num_pipes))
        sys.stdout.flush()

        dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))

        sess_hdl = self.conn_mgr.client_init()
        entry_hdl = self.client.set_eg_set_default_action_drop_packet(sess_hdl, dev_tgt)

        # Wait for all pipe APIs to complete.
        self.conn_mgr.complete_operations(sess_hdl)

        maxEntries = 10
        replayedSameEntries = 5
        replayedUpdatedEntries = 5
        additionalEntries = 5

        # Set the symmetricity for the tables
        self.client.port_tbl_set_property(sess_hdl, dev_tgt.dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES, 0)

        try:

            # Add entries in the phase0 table
            self.addEntries(sess_hdl, dev_tgt, maxEntries)

            # Add entries in the AUX table
            self.addEntriesAuxTable(sess_hdl, dev_tgt)

            self.verifyAllAddedEntries()

            self.devport_mgr.devport_mgr_warm_init_begin(dev_id, dev_init_mode.DEV_WARM_INIT_HITLESS, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, True)
            pal_utils.add_ports(self)

            # Replay a random set of same entries
            self.replayRandomSetOfSameEntries(sess_hdl, dev_tgt, replayedSameEntries)

            # Replay a random set of updated entries
            self.replayRandomSetOfUpdatedEntries(sess_hdl, dev_tgt, replayedUpdatedEntries)

            # Replay a random set of additional new entries
            self.addEntries(sess_hdl, dev_tgt, additionalEntries)

            # Replay default entry
            entry_hdl = self.client.set_eg_set_default_action_drop_packet(sess_hdl, dev_tgt)

            self.devport_mgr.devport_mgr_warm_init_end(dev_id)

            self.conn_mgr.complete_operations(sess_hdl)

            # Add entries in the AUX table
            self.addEntriesAuxTable(sess_hdl, dev_tgt)

            self.conn_mgr.complete_operations(sess_hdl)

            if test_param_get('target') == "hw":
                pal_utils.check_port_status(self, swports)

            self.verifyEntriesAfterHA()

            ha_report = self.client.port_tbl_get_ha_reconciliation_report(sess_hdl, dev_tgt)
            logger.info("{}".format(ha_report))
            assert(ha_report.num_entries_added == 0)
            if num_pipes == 2:
                assert(ha_report.num_entries_deleted == 129)
            else:
                assert(ha_report.num_entries_deleted == 273)
            assert(ha_report.num_entries_modified == 10)
            verify_no_other_packets(self)

        finally:
            clean_up_tables(self, sess_hdl, dev_tgt)
            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.client_cleanup(sess_hdl)

@group("thorough")
class TestRange(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["ha"])
        self.tcp_dport_start = 0
        self.tcp_dport_end = 0
        self.ttl_start = 0
        self.ttl_end = 0
        self.match_spec_arr = []
        self.action_spec_arr = []
        self.ha_entry_hdl_arr = []
        self.match_spec_dict = {}
        self.action_spec_dict = {}

    def getMatchValues(self, iter_no, tcp_dport_partition_length, ttl_partition_length):
        self.tcp_dport_start = random.randint((tcp_dport_partition_length * iter_no), (tcp_dport_partition_length * (iter_no+1)) - 1)
        self.tcp_dport_end = random.randint(self.tcp_dport_start, (tcp_dport_partition_length * (iter_no+1)) - 1)
        while self.tcp_dport_end <= self.tcp_dport_start:
            self.tcp_dport_start = random.randint((tcp_dport_partition_length * (iter_no)), (tcp_dport_partition_length * (iter_no+1)) - 1)
            self.tcp_dport_end = random.randint(self.tcp_dport_start, (tcp_dport_partition_length * (iter_no+1)) - 1)

        self.ttl_start = random.randint((ttl_partition_length * (iter_no)), (ttl_partition_length * (iter_no+1)) - 1)
        self.ttl_end = random.randint(self.ttl_start, (ttl_partition_length * (iter_no+1)) - 1)
        while self.ttl_end <= self.ttl_start:
            self.ttl_start = random.randint((ttl_partition_length * (iter_no)), (ttl_partition_length * (iter_no+1)) - 1)
            self.ttl_end = random.randint(self.ttl_start, (ttl_partition_length * (iter_no+1)) - 1)

    def addEntries(self, sess_hdl, dev_tgt, maxEntries):

        numEntries = len(self.match_spec_arr)
        tcp_dport_partition_length = 32767 // maxEntries
        ttl_partition_length = 127 // maxEntries
        for i in range(maxEntries):
            self.getMatchValues(i, tcp_dport_partition_length, ttl_partition_length)
            ipv4_daddr = (random.randint(0, 255))
            match_spec = ha_tcam_range_match_spec_t(
                             ipv4Addr_to_i32("10.0.0.{}".format(ipv4_daddr)),
                             ipv4Addr_to_i32("255.255.255.255"),
                             hex_to_i32(self.tcp_dport_start),
                             hex_to_i32(self.tcp_dport_end),
                             hex_to_i32(self.ttl_start),
                             hex_to_i32(self.ttl_end))
            match_spec_str = "{}.{}.{}.{}.{}.{}.{}.{}".format(ipv4_daddr,ipv4_daddr,ipv4_daddr,ipv4_daddr,match_spec.tcp_dstPort_start,match_spec.tcp_dstPort_end,match_spec.ipv4_ttl_start,match_spec.ipv4_ttl_end)
            while match_spec_str in self.match_spec_dict:
                self.getMatchValues(i, tcp_dport_partition_length, ttl_partition_length)
                ipv4_daddr = (random.randint(0, 255))
                match_spec = ha_tcam_range_match_spec_t(
                                 ipv4Addr_to_i32("10.0.0.{}".format(ipv4_daddr)),
                                 ipv4Addr_to_i32("255.255.255.255"),
                                 hex_to_i32(self.tcp_dport_start),
                                 hex_to_i32(self.tcp_dport_end),
                                 hex_to_i32(self.ttl_start),
                                 hex_to_i32(self.ttl_end))
                match_spec_str = "{}.{}.{}.{}.{}.{}.{}.{}".format(ipv4_daddr,ipv4_daddr,ipv4_daddr,ipv4_daddr,match_spec.tcp_dstPort_start,match_spec.tcp_dstPort_end,match_spec.ipv4_ttl_start,match_spec.ipv4_ttl_end)

            self.match_spec_arr.append(match_spec)
            self.match_spec_dict[match_spec_str] = True

            port_idx = random.randint(0, len(swports)-1)
            act_vid = random.randint(10, 2047)
            act_pcp = random.randint(1, 7)
            act_ttl = random.randint(1, 127)
            port = hex_to_i16(swports[port_idx])
            action_spec = ha_tcam_range_action_action_spec_t(hex_to_i16(port), act_vid, act_pcp, act_ttl)
            self.action_spec_arr.append(action_spec)
            ha_entry_hdl = self.client.tcam_range_table_add_with_tcam_range_action(sess_hdl,
                                                                              dev_tgt,
                                                                              match_spec,
                                                                              0,
                                                                              action_spec)
            self.ha_entry_hdl_arr.append(ha_entry_hdl)
            logger.info("Entry {} : Mat ipv4dstAddr : {} : Range Mat tcp_dport : {} - {} : Range Mat ttl : {} - {} : Act egr_port : {}"
                        .format(i + numEntries, i32_to_ipv4Addr(match_spec.ipv4_dstAddr), match_spec.tcp_dstPort_start, match_spec.tcp_dstPort_end, match_spec.ipv4_ttl_start, match_spec.ipv4_ttl_end, action_spec.action_val))

    def verifyEntry(self, src_port, dst_port, match_spec, action_spec):
        packet_tcp_dport = random.randint(match_spec.tcp_dstPort_start, match_spec.tcp_dstPort_end)
        packet_ttl = random.randint(match_spec.ipv4_ttl_start, match_spec.ipv4_ttl_end)
        pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                eth_src='00:22:22:22:22:22',
                                dl_vlan_enable=True,
                                vlan_vid=10,
                                vlan_pcp=2,
                                ip_src='1.1.1.1',
                                ip_dst=i32_to_ipv4Addr(match_spec.ipv4_dstAddr),
                                ip_id=101,
                                ip_ttl=packet_ttl,
                                tcp_dport=packet_tcp_dport)
        exp_pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                eth_src='00:22:22:22:22:22',
                                dl_vlan_enable=True,
                                vlan_vid=action_spec.action_value_0,
                                vlan_pcp=action_spec.action_value_1,
                                ip_src='1.1.1.1',
                                ip_dst=i32_to_ipv4Addr(match_spec.ipv4_dstAddr),
                                ip_id=101,
                                ip_ttl=action_spec.action_value_2,
                                tcp_dport=packet_tcp_dport)
        send_packet(self, src_port, pkt)
        verify_packet(self, exp_pkt, dst_port)

    def verifyAllAddedEntries(self):
        maxEntries = len(self.match_spec_arr)
        logger.info("Total entries are {}".format(maxEntries))
        for i in range (maxEntries):
            match_spec = self.match_spec_arr[i]
            action_spec = self.action_spec_arr[i]
            self.verifyEntry(swports[0], action_spec.action_val, match_spec, action_spec)
            logger.info("Entry Verified {}".format(i))
        verify_no_other_packets(self)

    def replaySameEntries(self, sess_hdl, dev_tgt, numEntries):
        maxEntries = len(self.match_spec_arr)
        if maxEntries < numEntries:
            assert 0

        for i in range (numEntries):
            # Replay all the same entries
            match_spec = self.match_spec_arr[i]
            action_spec = self.action_spec_arr[i]
            ha_entry_hdl = self.client.tcam_range_table_add_with_tcam_range_action(sess_hdl,
                                                                              dev_tgt,
                                                                              match_spec,
                                                                              0,
                                                                              action_spec)
            logger.info("Entry {} : Mat ipv4dstAddr : {} : Range Mat tcp_dport : {} - {} : Range Mat ttl : {} - {} : Act egr_port : {} "
                        .format(i + numEntries, i32_to_ipv4Addr(match_spec.ipv4_dstAddr), match_spec.tcp_dstPort_start, match_spec.tcp_dstPort_end, match_spec.ipv4_ttl_start, match_spec.ipv4_ttl_end, action_spec.action_val))

    def verifyEntriesAfterHA(self):
        maxEntries = len(self.match_spec_arr)
        logger.info("Total entries are {}".format(maxEntries))
        for i in range (maxEntries):
            # All entries that were replayed should hit
            match_spec = self.match_spec_arr[i]
            action_spec = self.action_spec_arr[i]
            self.verifyEntry(swports[0], action_spec.action_val, match_spec, action_spec)
            logger.info("Entry Replayed Verified {} : Mat ipv4dstAddr : {} : Range Mat tcp_dport : {} - {} : Range Mat ttl : {} - {} : Act egr_port : {} "
                        .format(i, i32_to_ipv4Addr(match_spec.ipv4_dstAddr), match_spec.tcp_dstPort_start, match_spec.tcp_dstPort_end, match_spec.ipv4_ttl_start, match_spec.ipv4_ttl_end, action_spec.action_val))
        verify_no_other_packets(self)

    """ Basic test """
    def runTest(self):
        misc_utils.setup_random()
        sess_hdl = self.conn_mgr.client_init()

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        maxEntries = 25

        try:
            # Add entries in the tcam range table
            self.addEntries(sess_hdl, dev_tgt, maxEntries)

            self.conn_mgr.complete_operations(sess_hdl)
            self.verifyAllAddedEntries()

            self.devport_mgr.devport_mgr_warm_init_begin(dev_id, dev_init_mode.DEV_WARM_INIT_HITLESS, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, True)

            pal_utils.add_ports(self)

            # Replay a random set of same entries
            self.replaySameEntries(sess_hdl, dev_tgt, maxEntries)

            self.devport_mgr.devport_mgr_warm_init_end(dev_id)

            self.conn_mgr.complete_operations(sess_hdl)

            if test_param_get('target') == "hw":
                pal_utils.check_port_status(self, swports)

            self.verifyEntriesAfterHA()

            ha_report = self.client.tcam_range_get_ha_reconciliation_report(sess_hdl, dev_tgt)
            logger.info("{}".format(ha_report))
            assert(ha_report.num_entries_added == 0)
            assert(ha_report.num_entries_deleted == 0)
            assert(ha_report.num_entries_modified == 0)

        finally:
            clean_up_tables(self, sess_hdl, dev_tgt)

            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.client_cleanup(sess_hdl)
