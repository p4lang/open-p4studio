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
import copy
import random

import pd_base_tests
from pal_rpc.ttypes import *
from ptf.testutils import *
from ptf.thriftutils import *
import ptf.dataplane as dataplane
from fast_reconfig.p4_pd_rpc.ttypes import *
from mc_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *
import pal_utils as pal_utils
import misc_utils as misc_utils

dev_id = 0
sess_hdl = 0
mc_sess_hdl = 0
port_to_ifid = {}
ifid_to_brid = {}
brid_to_mgid = {}
brid_to_l1 = {}
ing_port_entry_hdls = []
ing_src_ifid_entry_hdls = []
ucast_dmac_entry_hdls = []
mcast_dmac_entry_hdls = []
ports_in_tree = {}

logger = misc_utils.get_logger()
swports = misc_utils.get_sw_ports()
logger.info("All ports: {}".format(swports))

def portToBitIdx(port):
    pipe = misc_utils.port_to_pipe(port)
    index = misc_utils.port_to_pipe_local_port(port)
    return 72 * pipe + index

def set_port_map(indicies):
    bit_map = [0] * ((288+7)//8)
    for i in indicies:
        index = portToBitIdx(i)
        bit_map[index//8] = (bit_map[index//8] | (1 << (index%8))) & 0xFF
    return bytes_to_string(bit_map)
def set_lag_map(indicies):
    bit_map = [0] * ((256+7)//8)
    for i in indicies:
        bit_map[i//8] = (bit_map[i//8] | (1 << (i%8))) & 0xFF
    return bytes_to_string(bit_map)

def verify_packet_list(test, port_ll, pkt_ll):
    global dev_id, sess_hdl, mc_sess_hdl, port_to_ifid, ifid_to_brid, brid_to_mgid, brid_to_l1, ing_port_entry_hdls, ing_src_ifid_entry_hdls, ucast_dmac_entry_hdls, mcast_dmac_entry_hdls, ports_in_tree
    more_to_rx = False
    for port_list in port_ll:
        if len(port_list) != 0:
            more_to_rx = True
    while more_to_rx:
        found_port = False
        found_pkt  = False
        (rcv_device, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll( timeout=1.0 )

        if rcv_port is None:
            logger.info("Didn't receive packet!!!")
            logger.info("Expected ports remaining: {}".format(port_ll))
            sys.stdout.flush()
            test.assertTrue(rcv_port is not None)

        # See if the received port+packet pair is in any of the lists passed in.
        for port_list, pkt_list in zip(port_ll, pkt_ll):
            if rcv_port in port_list:
                found_port = True
                for exp_pkt in pkt_list:
                    if dataplane.match_exp_pkt(exp_pkt, rcv_pkt):
                        pkt_list.remove(exp_pkt)
                        found_pkt = True
                        break
                if found_pkt:
                    port_list.remove(rcv_port)
                    break

        if found_port != True or found_pkt != True:
            logger.info("Unexpected Rx: port {}".format(rcv_port))
            logger.info(format_packet(rcv_pkt))
            logger.info("Expected the following:")
            for port_list, pkt_list in zip(port_ll, pkt_ll):
                logger.info("  Ports: {}".format(sorted(port_list)))
                for pkt in pkt_list:
                    logger.info("  Pkt: {}".format(format_packet(pkt)))
            sys.stdout.flush()
            test.assertTrue(found_port == True, "Unexpected port %r" % rcv_port)
            test.assertTrue(found_pkt  == True, "Unexpected pkt on port %r" % rcv_port)

        more_to_rx = False
        for port_list in port_ll:
            if len(port_list) != 0:
                more_to_rx = True

    (rcv_device, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll( timeout=0.1 )
    if rcv_port != None:
        logger.info("Extra Rx: port {}, Packet {}".format(rcv_port, format_packet(rcv_pkt)))
        sys.stdout.flush()
        test.assertTrue(rcv_pkt == None, "Receive extra packet")

def build_rx_packet_list(port_list, pkt):
    pkt_list = []
    tmp_port_list = sorted(port_list)
    last_pipe = -1
    for port in tmp_port_list:
        if last_pipe == misc_utils.port_to_pipe(port):
            pkt["IP"].tos = 0
            pkt_list.append(copy.deepcopy(pkt))
        else:
            pkt["IP"].tos = 1
            last_pipe = misc_utils.port_to_pipe(port)
            pkt_list.append(copy.deepcopy(pkt))

    return pkt_list

def add_port_entry(test, dev_tgt, port, vlan_v, vlan, ifid):
    global dev_id, sess_hdl, mc_sess_hdl, port_to_ifid, ifid_to_brid, brid_to_mgid, brid_to_l1, ing_port_entry_hdls, ing_src_ifid_entry_hdls, ucast_dmac_entry_hdls, mcast_dmac_entry_hdls, ports_in_tree
    m_spec = fast_reconfig_ing_port_match_spec_t(port, vlan_v, vlan)
    a_spec = fast_reconfig_set_ifid_action_spec_t(hex_to_i32(ifid))
    return test.client.ing_port_table_add_with_set_ifid(sess_hdl, dev_tgt, m_spec, a_spec)

def add_ifid_entry(test, dev_tgt, ifid, rid, yid, brid, h1, h2):
    global dev_id, sess_hdl, mc_sess_hdl, port_to_ifid, ifid_to_brid, brid_to_mgid, brid_to_l1, ing_port_entry_hdls, ing_src_ifid_entry_hdls, ucast_dmac_entry_hdls, mcast_dmac_entry_hdls, ports_in_tree
    m_spec = fast_reconfig_ing_src_ifid_match_spec_t(hex_to_i32(ifid))
    a_spec = fast_reconfig_set_src_ifid_md_action_spec_t(
                                                         hex_to_i16(rid),
                                                         yid,
                                                         hex_to_i16(brid),
                                                         hex_to_i16(h1),
                                                         hex_to_i16(h2))
    return test.client.ing_src_ifid_table_add_with_set_src_ifid_md(
                                              sess_hdl, dev_tgt, m_spec, a_spec)


class McastClass():
    def __init__(self):
        sys.stdout.flush()

    def config(self_local, test):
        global dev_id, sess_hdl, mc_sess_hdl, port_to_ifid, ifid_to_brid, brid_to_mgid, brid_to_l1, ing_port_entry_hdls, ing_src_ifid_entry_hdls, ucast_dmac_entry_hdls, mcast_dmac_entry_hdls, ports_in_tree
        sess_hdl = test.conn_mgr.client_init()
        mc_sess_hdl = test.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        ifids = set()
        brids = set()

        for p in swports:
            port_to_ifid[p] = p
            ifids.add( p )

        brids.update( random.sample(range(0, 0x10000), len(ifids)) )
        brid_list = list( brids )
        for ifid,brid in zip(ifids, brids):
            ifid_to_brid[ifid] = brid

        for port_num in swports:
            vlan_val = 0
            vlan_id = 0
            ifid = port_to_ifid[port_num]
            entry_hdl = add_port_entry(test, dev_tgt, port_num, vlan_val, vlan_id, ifid)
            ing_port_entry_hdls.append(entry_hdl)

        for port_num in swports:
            ifid = port_to_ifid[port_num]
            rid = 0
            yid = 0
            brid = ifid_to_brid[ifid]
            hash1 = 1
            hash2 = 1
            entry_hdl = add_ifid_entry(test, dev_tgt, ifid, rid, yid, brid, hash1, hash2)
            ing_src_ifid_entry_hdls.append(entry_hdl)

        for i, port in enumerate(swports):
            ing_port = port
            egr_port = swports[(i+1) % len(swports)]
            ifid = port_to_ifid[ing_port]
            brid = ifid_to_brid[ifid]
            dmac = "00:11:11:11:11:00"
            match_spec = fast_reconfig_ing_dmac_match_spec_t(
                                hex_to_i16(brid),
                                macAddr_to_string(dmac))
            action_spec = fast_reconfig_switch_action_spec_t(
                                hex_to_i16(egr_port))
            entry_hdl = test.client.ing_dmac_table_add_with_switch(
                                sess_hdl,
                                dev_tgt,
                                match_spec,
                                action_spec)
            ucast_dmac_entry_hdls.append(entry_hdl)

        for brid in brids:
            dmac = "FF:FF:FF:FF:FF:FF"
            match_spec = fast_reconfig_ing_dmac_match_spec_t(
                                hex_to_i16(brid),
                                macAddr_to_string(dmac))
            entry_hdl = test.client.ing_dmac_table_add_with_flood(
                                sess_hdl,
                                dev_tgt,
                                match_spec)
            mcast_dmac_entry_hdls.append(entry_hdl)

        # Allocate an MGID per brid
        for brid in brids:
            mgrp_hdl = test.mc.mc_mgrp_create(mc_sess_hdl, dev_id, hex_to_i16(brid))
            brid_to_mgid[brid] = mgrp_hdl
            logger.info("MGID {} has handle {}".format(hex(brid), hex(mgrp_hdl)))

        # Allocate a single L1 node for each brid
        for brid in brids:
            port_map = set_port_map([])
            lag_map = set_lag_map([])
            l1_hdl = test.mc.mc_node_create(mc_sess_hdl, dev_id, hex_to_i16((~brid)&0xFFFF), port_map, lag_map)
            brid_to_l1[brid] = l1_hdl
            logger.info("MGID {} has node handle {}".format(hex(brid), hex(l1_hdl)))

        # Add L2 nodes to the L1s
        # Add all ports to the first
        l2_node_ports = []
        l2_node_lags = []
        for p in swports:
            l2_node_ports.append(int(p))
        ports_in_tree[brid_list[0]] = sorted(l2_node_ports)
        port_map = set_port_map(l2_node_ports)
        lag_map = set_lag_map(l2_node_lags)
        test.mc.mc_node_update(mc_sess_hdl, dev_id, brid_to_l1[ brid_list[0] ], port_map, lag_map)
        test.mc.mc_associate_node(mc_sess_hdl, dev_id,
                                           brid_to_mgid[ brid_list[0] ],
                                           brid_to_l1[ brid_list[0] ],
                                           0,
                                           0)
        logger.info("MGID {} Node {} has ports {}".format(hex(brid_list[0]), hex(brid_to_l1[ brid_list[0] ]), [brid_list[0]]))
        # Add no ports to the second
        l2_node_ports = []
        l2_node_lags = []
        ports_in_tree[brid_list[1]] = sorted(l2_node_ports)
        port_map = set_port_map(l2_node_ports)
        lag_map = set_lag_map(l2_node_lags)
        test.mc.mc_node_update(mc_sess_hdl, dev_id, brid_to_l1[ brid_list[1] ], port_map, lag_map)
        test.mc.mc_associate_node(mc_sess_hdl, dev_id,
                                           brid_to_mgid[ brid_list[1] ],
                                           brid_to_l1[ brid_list[1] ],
                                           0,
                                           0)
        logger.info("MGID {} Node {} has ports {}".format(hex(brid_list[1]), hex(brid_to_l1[ brid_list[1] ]), ports_in_tree[brid_list[1]]))
        # Add a random number of ports to the rest
        for i in range(2, len(brid_list)):
            num_ports = random.randint(0, len(swports)-1)
            l2_node_ports = random.sample(swports, num_ports)
            l2_node_lags = []
            ports_in_tree[brid_list[i]] = sorted(l2_node_ports)
            port_map = set_port_map(l2_node_ports)
            lag_map = set_lag_map(l2_node_lags)
            test.mc.mc_node_update(mc_sess_hdl, dev_id, brid_to_l1[ brid_list[i] ], port_map, lag_map)
            test.mc.mc_associate_node(mc_sess_hdl, dev_id,
                                               brid_to_mgid[ brid_list[i] ],
                                               brid_to_l1[ brid_list[i] ],
                                               0,
                                               0)
            logger.info("MGID {} Node {} has ports {}".format(hex(brid_list[i]), hex(brid_to_l1[ brid_list[i]]), ports_in_tree[brid_list[i]]))
        sys.stdout.flush()

        # Wait for a pipe APIs to complete.
        test.conn_mgr.complete_operations(sess_hdl)

        # Wait for multicast APIs to complete.
        test.mc.mc_complete_operations(mc_sess_hdl)

    def verifypkts(self_local, test):
        global dev_id, sess_hdl, mc_sess_hdl, port_to_ifid, ifid_to_brid, brid_to_mgid, brid_to_l1, ing_port_entry_hdls, ing_src_ifid_entry_hdls, ucast_dmac_entry_hdls, mcast_dmac_entry_hdls, ports_in_tree
        for i in range(0, len(swports)):
            logger.info("Sending unicast on port {} expecting receive on port {}".format(swports[i], swports[(i+1)%len(swports)]))
            sys.stdout.flush()
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:00',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_tos=255,
                                    ip_ttl=64,
                                    ip_id=101)
            send_packet(test,  swports[i], pkt)
            epkt = simple_tcp_packet(eth_dst='00:11:11:11:11:00',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_tos=0,
                                    ip_ttl=64,
                                    ip_id=0)

            verify_packet(test, epkt, swports[(i+1)%len(swports)])
        verify_no_other_packets(test, timeout=2)

        for ing_port in swports:
            ifid = port_to_ifid[ing_port]
            brid = ifid_to_brid[ifid]
            rx_port_list = ports_in_tree[brid]
            # Make a copy of the original port list(rx_port_list) to prevent changes to it.
            rx_port_list_copy = list(rx_port_list)
            logger.info("brid: {} rx_port_list: {}".format(brid, rx_port_list_copy))
            rx_pkt_list = build_rx_packet_list(rx_port_list_copy, simple_tcp_packet(eth_dst='FF:FF:FF:FF:FF:FF',
                                                                               eth_src='00:22:22:22:22:22',
                                                                               ip_src='1.1.1.1',
                                                                               ip_dst='10.0.0.1',
                                                                               ip_ttl=64,
                                                                               ip_id=(~brid & 0xFFFF)))
            logger.info("Sending multicast on port {} expecting receive on ports {}".format(ing_port, rx_port_list))
            sys.stdout.flush()
            pkt = simple_tcp_packet(eth_dst='FF:FF:FF:FF:FF:FF',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_tos=255,
                                    ip_ttl=64,
                                    ip_id=101)
            send_packet(test, ing_port, pkt)

            verify_packet_list(test, [rx_port_list_copy], [rx_pkt_list])
        logger.info("  Multicast traffic okay")

    def pushconfig(self_local, test):
        global dev_id, sess_hdl, mc_sess_hdl, port_to_ifid, ifid_to_brid, brid_to_mgid, brid_to_l1, ing_port_entry_hdls, ing_src_ifid_entry_hdls, ucast_dmac_entry_hdls, mcast_dmac_entry_hdls, ports_in_tree
        # Wait for a pipe APIs to complete.
        test.conn_mgr.complete_operations(sess_hdl)

        # Wait for multicast APIs to complete
        # thread get stuck, commenting this for now
        test.mc.mc_complete_operations(mc_sess_hdl)

    def cleanup(self_local, test):
        global dev_id, sess_hdl, mc_sess_hdl, port_to_ifid, ifid_to_brid, brid_to_mgid, brid_to_l1, ing_port_entry_hdls, ing_src_ifid_entry_hdls, ucast_dmac_entry_hdls, mcast_dmac_entry_hdls, ports_in_tree
        for brid in brid_to_l1:
            test.mc.mc_dissociate_node(mc_sess_hdl, dev_id, brid_to_mgid[brid], brid_to_l1[brid])
            test.mc.mc_node_destroy(mc_sess_hdl, dev_id, brid_to_l1[brid])
        for brid in brid_to_mgid:
            test.mc.mc_mgrp_destroy(mc_sess_hdl, dev_id, brid_to_mgid[brid])
        for entry_hdl in ing_port_entry_hdls:
            test.client.ing_port_table_delete(sess_hdl, dev_id, entry_hdl)
        for entry_hdl in ing_src_ifid_entry_hdls:
            test.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
        for entry_hdl in ucast_dmac_entry_hdls:
            test.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
        for entry_hdl in mcast_dmac_entry_hdls:
            test.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
        test.conn_mgr.client_cleanup(sess_hdl)
        test.mc.mc_destroy_session(mc_sess_hdl)

        # Reset so we can configure again
        sess_hdl = 0
        mc_sess_hdl = 0
        port_to_ifid = {}
        ifid_to_brid = {}
        brid_to_mgid = {}
        brid_to_l1 = {}
        ing_port_entry_hdls = []
        ing_src_ifid_entry_hdls = []
        ucast_dmac_entry_hdls = []
        mcast_dmac_entry_hdls = []
        ports_in_tree = {}

class TestPipelineTable(pd_base_tests.EmptyTableTest):
    program = "fast_reconfig"
    table = "ing_port"


@group("thorough")
class TestProfileChangeMcast(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["fast_reconfig"])

    """ Profile change Mcast """
    def runTest(self):
        global dev_id
        if test_param_get('target') == "bmv2":
            return
        #pdb.set_trace()
        mcObj = McastClass()
        # Apply config and run test
        logger.info("-- Starting profile change test for Mcast --")
        logger.info("Setup config")
        sys.stdout.flush()
        mcObj.config(self)
        logger.info("Push the config")
        sys.stdout.flush()
        mcObj.pushconfig(self)
        logger.info("Send pkts and verify")
        sys.stdout.flush()
        mcObj.verifypkts(self)
        logger.info("Cleaning up config")
        sys.stdout.flush()
        mcObj.cleanup(self)

        # Synchronize counters
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, False)
        self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, True)
        self.conn_mgr.complete_operations(sess_hdl)
        self.conn_mgr.client_cleanup(sess_hdl)

        # Stage 1 lock
        logger.info("Stage 1 lock")
        sys.stdout.flush()
        self.devport_mgr.devport_mgr_warm_init_begin(0, dev_init_mode.DEV_WARM_INIT_FAST_RECFG, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, True)

        if test_param_get('target') == "hw":
            # re-add ports here using platform code
            logger.info("Add ports")
            pal_utils.add_ports(self)

        # Synchronizing counters should fail when device is locked.
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        error_device_locked = False
        try:
            self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, False)
        except InvalidCounterOperation as ico:
            # Expect error code Device Locked (21)
            if (ico.code == 21):
                error_device_locked = True
        self.assertTrue(error_device_locked)
        error_device_locked = False
        try:
            self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, True)
        except InvalidCounterOperation as ico:
            # Expect error code Device Locked (21)
            if (ico.code == 21):
                error_device_locked = True
        self.assertTrue(error_device_locked)
        self.conn_mgr.complete_operations(sess_hdl)
        self.conn_mgr.client_cleanup(sess_hdl)

        # Replay state
        logger.info("Replay config")
        sys.stdout.flush()
        mcObj.config(self)

        # Stage 2 unlock
        logger.info("Stage 2 unlock")
        sys.stdout.flush()
        self.devport_mgr.devport_mgr_warm_init_end(0)

        if test_param_get('target') == "hw":
            # Check port status
            pal_utils.check_port_status(self, swports)

        # Synchronize counters
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, False)
        self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, True)
        self.conn_mgr.complete_operations(sess_hdl)
        self.conn_mgr.client_cleanup(sess_hdl)

        logger.info("Push the config again")
        sys.stdout.flush()
        mcObj.pushconfig(self)

        # Send packet and verify
        logger.info("Send pkts again and verify")
        sys.stdout.flush()
        mcObj.verifypkts(self)
        logger.info("Cleaning up config")
        sys.stdout.flush()
        mcObj.cleanup(self)


@group("thorough")
class TestWarmInitQuick(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["fast_reconfig"])
        # In warm init quick chips goes thorough fast-reconfig. There is no
        # config replay. bf-drivers redownloads the config back to asic.

    """ Warm Init quick """
    def runTest(self):
        global dev_id
        if test_param_get('arch') != "tofino":
            return
        mcObj = McastClass()
        # Apply config and run test
        logger.info("-- Starting warm init quick test for Mcast --")
        logger.info("Setup config")
        sys.stdout.flush()
        mcObj.config(self)
        logger.info("Push the config")
        sys.stdout.flush()
        mcObj.pushconfig(self)
        logger.info("Send pkts and verify")
        sys.stdout.flush()
        mcObj.verifypkts(self)

        # Stage 1 lock
        logger.info("Stage 1 lock")
        sys.stdout.flush()
        self.devport_mgr.devport_mgr_warm_init_begin(0, dev_init_mode.DEV_WARM_INIT_FAST_RECFG_QUICK, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, True)

        if test_param_get('target') == "hw":
            # re-add ports here using platform code
            logger.info("Add ports")
            pal_utils.add_ports(self)

        # Synchronizing counters should fail when device is locked.
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        error_device_locked = False
        try:
            self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, False)
        except InvalidCounterOperation as ico:
            # Expect error code Device Locked (21)
            if (ico.code == 21):
                error_device_locked = True
        self.assertTrue(error_device_locked)
        error_device_locked = False
        try:
            self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, True)
        except InvalidCounterOperation as ico:
            # Expect error code Device Locked (21)
            if (ico.code == 21):
                error_device_locked = True
        self.assertTrue(error_device_locked)
        self.conn_mgr.complete_operations(sess_hdl)
        self.conn_mgr.client_cleanup(sess_hdl)

        # Stage 2 unlock
        logger.info("Stage 2 unlock")
        sys.stdout.flush()
        self.devport_mgr.devport_mgr_warm_init_end(0)

        if test_param_get('target') == "hw":
            # Check port status
            pal_utils.check_port_status(self, swports)

        # Synchronize counters
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, False)
        self.client.counter_hw_sync_count_egr_encode(sess_hdl, dev_tgt, True)
        self.conn_mgr.complete_operations(sess_hdl)
        self.conn_mgr.client_cleanup(sess_hdl)

        # Send packet and verify
        logger.info("Send pkts again and verify again")
        sys.stdout.flush()
        mcObj.verifypkts(self)
        logger.info("Cleaning up config")
        sys.stdout.flush()
        mcObj.cleanup(self)
