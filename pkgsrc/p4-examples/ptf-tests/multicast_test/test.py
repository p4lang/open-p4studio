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
import sys
import logging
import copy
import pdb

import unittest
import random

import pd_base_tests

from ptf import config
from ptf.testutils import *
from ptf.thriftutils import *
import ptf.dataplane as dataplane
from p4testutils.misc_utils import *

import os

from multicast_test.p4_pd_rpc.ttypes import *
from conn_mgr_pd_rpc.ttypes import *
from mc_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from ptf_port import *
from pal_rpc.ttypes import *

dev_id = 0


class TestUtil:
    def __init__(self):
        self.pm_shdl = None
        self.mc_shdl = None
        self.test = None
        self.lag_tbl = None
        self.yid_tbl = None
        self.sw_mask = [0 for x in range(2*288)]
        self.hw_mask = [0 for x in range(2*288)]
        self.pbt_on = False
    def setup(self, test, pm_hdl, mc_hdl):
        self.pm_shdl = pm_hdl
        self.mc_shdl = mc_hdl
        self.test = test
        self.lag_tbl = LagTable(self.test, self.mc_shdl, dev_id)
        self.yid_tbl = YidTable(self.test, self.mc_shdl, dev_id, 0)
        self.sw_mask = [0 for x in range(2*288)]
        self.hw_mask = [0 for x in range(2*288)]
        self.backup_ports = [0 for x in range(2*288)]
        for x in range(2*288):
            self.backup_ports[x] = BitIdxToPort(x)
    def cleanUp(self):
        self.lag_tbl.cleanUp()
        self.yid_tbl.cleanUp()
        self.sw_mask = [0 for x in range(2*288)]
        self.hw_mask = [0 for x in range(2*288)]
        backup_port_max = 288
        if test_param_get("arch") == "tofino3":
            backup_port_max = 2*288
        for x in range(backup_port_max):
            self.clr_backup_port( BitIdxToPort(x) )
        self.disable_port_ff()
        self.disable_backup_ports()
        self.pm_shdl = None
        self.mc_shdl = None
        self.lag_tbl = None
        self.yid_tbl = None
        self.test = None
    def get_lag_tbl(self):
        return self.lag_tbl
    def get_yid_tbl(self):
        return self.yid_tbl
    def get_sw_mask(self):
        return self.sw_mask
    def get_hw_mask(self):
        return self.hw_mask
    def sw_port_down(self, port):
        self.sw_mask[ portToBitIdx(port) ] = 1
        self.test.mc.mc_set_port_mc_fwd_state(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id), hex_to_i16(port), hex_to_byte(0))
    def sw_port_up(self, port):
        self.sw_mask[ portToBitIdx(port) ] = 0
        self.test.mc.mc_set_port_mc_fwd_state(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id), hex_to_i16(port), hex_to_byte(1))
    def enable_port_ff(self):
        self.test.mc.mc_enable_port_ff(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id))
    def disable_port_ff(self):
        self.test.mc.mc_disable_port_ff(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id))
    def enable_backup_ports(self):
        self.test.mc.mc_enable_port_protection(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id))
        self.pbt_on = True
    def disable_backup_ports(self):
        self.test.mc.mc_disable_port_protection(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id))
        self.pbt_on = False
    def set_backup_port(self, pport, bport):
        self.backup_ports[ portToBitIdx(pport) ] = bport
        self.test.mc.mc_set_port_protection(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id), hex_to_i16(pport), hex_to_i16(bport))
    def clr_backup_port(self, port):
        self.backup_ports[ portToBitIdx(port) ] = port
        self.test.mc.mc_clear_port_protection(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id), hex_to_i16(port))
    def get_backup_port(self, port):
        if self.pbt_on:
            return self.backup_ports[ portToBitIdx(port) ]
        else:
            return port
    def clr_hw_port_down(self, port):
        self.hw_mask[ portToBitIdx(port) ] = 0
        self.test.mc.mc_clr_port_ff_state(hex_to_i32(self.mc_shdl), hex_to_i32(dev_id), hex_to_i16(port))
    def set_port_down(self, port):
        self.hw_mask[ portToBitIdx(port) ] = 1
        if test_param_get("target") == "hw":
            self.test.pal.pal_port_dis(dev_id, port)
        else:
            take_port_down(port)
    def set_port_up(self, port):
        if test_param_get("target") == "hw":
            self.test.pal.pal_port_enable(dev_id, port)
            for _ in range(100):
                x = self.test.pal.pal_port_oper_status_get(dev_id, port)
                if x == pal_oper_status_t.BF_PORT_DOWN:
                    time.sleep(0.2)
                else:
                    break
            assert x == pal_oper_status_t.BF_PORT_UP
        else:
            bring_port_up(port)

t = TestUtil()


def portToBitIdx(port):
    pipe = port_to_pipe(port)
    index = port_to_pipe_local_port(port)
    return 72 * pipe + index

def BitIdxToPort(index):
    pipe = index // 72
    local_port = index % 72
    return (pipe << 7) | local_port

def portMapToPorts(bitmap):
    devports = []
    for i in range(2*288):
        if bitmap & (1<<i):
            devports.append(BitIdxToPort(i))
    return devports

def set_port_map(indicies):
    # Use extended port map for mcast get prune table comparison.
    bit_map = [0] * ((2*288+7)//8)
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
    more_to_rx = False
    for port_list in port_ll:
        if len(port_list) != 0:
            more_to_rx = True
    while more_to_rx:
        found_port = False
        found_pkt  = False
        (rcv_device, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll( timeout=3.0 )
        #print("Rx on port", rcv_port)
        #print(format_packet( rcv_pkt ))
        #sys.stdout.flush()

        if rcv_port is None:
            print("Didn't receive packet!!!")
            print("Expected ports remaining:", port_ll)
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
            print("Unexpected Rx: port", rcv_port)
            print(format_packet(rcv_pkt))
            print("Expected the following:")
            for port_list, pkt_list in zip(port_ll, pkt_ll):
                print("  Ports:", sorted(port_list))
                for pkt in pkt_list:
                    print(format_packet(pkt))
            sys.stdout.flush()
            test.assertTrue(found_port == True, "Unexpected port %r" % rcv_port)
            test.assertTrue(found_pkt  == True, "Unexpected pkt on port %r" % rcv_port)

        more_to_rx = False
        for port_list in port_ll:
            if len(port_list) != 0:
                more_to_rx = True

    (rcv_device, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll( timeout=0.1 )
    if rcv_port != None:
        print("Extra Rx: port", rcv_port, "Packet", format_packet(rcv_pkt))
        sys.stdout.flush()
        test.assertTrue(rcv_pkt == None, "Receive extra packet")

def build_rx_packet_list(port_list, pkt):
    pkt_list = []
    tmp_port_list = sorted(port_list)
    last_pipe = -1
    for port in tmp_port_list:
        if last_pipe == port_to_pipe(port):
            pkt["IP"].tos = 0
            pkt_list.append(copy.deepcopy(pkt))
        else:
            pkt["IP"].tos = 1
            last_pipe = port_to_pipe(port)
            pkt_list.append(copy.deepcopy(pkt))

    return pkt_list

class L1Node:
    def __init__(self, test, shdl, rid, dev):
        self.shdl      = shdl
        self.rid       = rid
        self.dev       = dev
        self.mgid_hdl  = None
        self.xid       = None
        self.l2_hdl    = None
        self.mbr_ports = []
        self.mbr_lags  = []
        ports = set_port_map(self.mbr_ports)
        lags  = set_lag_map(self.mbr_lags)
        self.l1_hdl = test.mc.mc_node_create(hex_to_i32(shdl), hex_to_i32(dev), hex_to_i16(rid), ports, lags)
        assert(0 != self.l1_hdl)
    def __repr__(self):
        return "L1Node_" + str(hex(self.l1_hdl))
    def __str__(self):
        return str(hex(self.l1_hdl))
    def l1_hdl(self):
        return self.l1_hdl
    def get_rid(self):
        return self.rid
    def get_mbr_ports(self):
        return list(self.mbr_ports)
    def associate(self, test, mgrp_hdl, xid):
        self.mgid_hdl = mgrp_hdl
        self.xid = xid
        if xid is None:
            xid = 0
            use_xid = 0
        else:
            use_xid = 1
        test.mc.mc_associate_node(hex_to_i32(self.shdl),
                                  hex_to_i32(self.dev),
                                  hex_to_i32(self.mgid_hdl),
                                  hex_to_i32(self.l1_hdl),
                                  hex_to_i16(xid), use_xid)
    def dissociate(self, test):
        test.mc.mc_dissociate_node(hex_to_i32(self.shdl),
                                   hex_to_i32(self.dev),
                                   hex_to_i32(self.mgid_hdl),
                                   hex_to_i32(self.l1_hdl))
    def is_associated(self, test):
        return test.mc.mc_node_get_association(hex_to_i32(self.shdl),
                                               hex_to_i32(self.dev),
                                               hex_to_i32(self.l1_hdl))
    def addMbrs(self, test, port_list, lag_list):
        if port_list is None and lag_list is None:
            return 0
        if port_list is not None:
            self.mbr_ports += port_list
            self.mbr_ports.sort()
        if lag_list is not None:
            for i in lag_list:
                assert i >= 0 and i <= 255
            self.mbr_lags += lag_list
            self.mbr_lags.sort()
        ports = set_port_map(self.mbr_ports)
        lags  = set_lag_map(self.mbr_lags)
        test.mc.mc_node_update(hex_to_i32(self.shdl),
                               hex_to_i32(self.dev),
                               hex_to_i32(self.l1_hdl),
                               ports, lags)
    def updateMbrs(self, test, port_list, lag_list):
        if port_list is None and lag_list is None:
            return 0
        if port_list is not None:
            self.mbr_ports = port_list
            self.mbr_ports.sort()
        if lag_list is not None:
            for i in lag_list:
                assert i >= 0 and i <= 255
            self.mbr_lags = lag_list
            self.mbr_lags.sort()
        ports = set_port_map(self.mbr_ports)
        lags  = set_lag_map(self.mbr_lags)
        test.mc.mc_node_update(hex_to_i32(self.shdl),
                               hex_to_i32(self.dev),
                               hex_to_i32(self.l1_hdl),
                               ports, lags)
    def getPorts(self, test, rid, xid, yid, h2):
        global t
        # Start with the individual ports on the L1 and then apply pruning
        port_list = self.get_mbr_ports()
        if self.rid == rid or rid == t.get_yid_tbl().global_rid():
            t.get_yid_tbl().prune_ports(yid, port_list)
        # If any ports are down, replace them with their backup
        # Since the backup table is initialized such that each port backups up
        # itself we can blindly take the backup table contents if the port is
        # down.
        if port_list:
            for x in range(len(port_list)):
                pport = port_list[x]
                pport_idx = portToBitIdx(pport)
                if t.sw_mask[pport_idx] == 1 or t.hw_mask[pport_idx] == 1:
                    port_list[x] = t.get_backup_port(pport)
        # For each LAG on the L1, pick the correct member port
        for lag_id in self.mbr_lags:
            lag_mbr = test.mc.mc_get_lag_member_from_hash(hex_to_i32(self.shdl),
                                                          hex_to_i32(self.dev),
                                                          hex_to_i32(self.l1_hdl),
                                                          hex_to_byte(lag_id),
                                                          hex_to_i16(h2),
                                                          hex_to_i16(xid),
                                                          hex_to_i16(yid),
                                                          hex_to_i16(rid))
            if not lag_mbr.is_pruned and (lag_mbr.port != 0xFFFF):
                port_list.append(lag_mbr.port)
        return port_list

    def cleanUp(self, test):
        if self.mgid_hdl is not None:
            test.mc.mc_dissociate_node(hex_to_i32(self.shdl),
                                       hex_to_i32(self.dev),
                                       hex_to_i32(self.mgid_hdl),
                                       hex_to_i32(self.l1_hdl))
            self.mgid_hdl = None
        test.mc.mc_node_destroy(hex_to_i32(self.shdl), hex_to_i32(self.dev), hex_to_i32(self.l1_hdl))
        self.l1_hdl = 0

class LagGrp:
    def __init__(self, test, shdl, dev, lag_id):
        assert lag_id >= 0 and lag_id <= 255
        self.shdl = shdl
        self.dev = dev
        self.test = test
        self.lag_id = lag_id
        self.left_cnt = 0
        self.right_cnt = 0
        self.mbrs = []
    def setRmtCnt(self, left, right):
        self.left_cnt = left
        self.right_cnt = right
        self.test.mc.mc_set_remote_lag_member_count(hex_to_i32(self.shdl), hex_to_i32(self.dev),
                                                    hex_to_byte(self.lag_id), hex_to_i32(left), hex_to_i32(right))
    def addMbr(self, port_list):
        new_membership = list(set(self.mbrs + port_list))
        new_membership.sort()
        bit_map = set_port_map(new_membership)
        self.test.mc.mc_set_lag_membership(hex_to_i32(self.shdl), hex_to_i32(self.dev), hex_to_byte(self.lag_id), bit_map)
        self.mbrs = new_membership
    def rmvMbr(self, port_list):
        l = [x for x in self.mbrs if x not in port_list]
        self.mbrs = list(set(l))
        bit_map = set_port_map(self.mbrs)
        self.test.mc.mc_set_lag_membership(hex_to_i32(self.shdl), hex_to_i32(self.dev), hex_to_byte(self.lag_id), bit_map)
    def getMbrByHash(self, h, pkt_rid, node_rid, yid):
        global t
        if test_param_get("arch") != "tofino":
            h = (h ^ node_rid) & 0x1FFF
        if len(self.mbrs) == 0:
            return None # No members
        len_pack = (self.left_cnt + len(self.mbrs) + self.right_cnt) & 0x1FFF
        if len_pack:
            index_pack = h % len_pack
        else:
            index_pack = 0
        vec_pack = sorted(self.mbrs)

        vec_pack_mask = []
        for m in vec_pack:
            if t.get_sw_mask()[portToBitIdx(m)] == 1:
                continue
            if t.get_hw_mask()[portToBitIdx(m)] == 1:
                continue
            vec_pack_mask.append(m)
        len_pack_mask = len(vec_pack_mask)
        if len_pack_mask == 0:
            index_pack_mask = 0
        else:
            index_pack_mask = h % len_pack_mask

        if index_pack < self.right_cnt: # Hashed to remote right member
            return None
        if index_pack >= (len(self.mbrs) + self.right_cnt): # Hashed to remote left member
            return None

        if len_pack_mask == 0: # No live ports
            port = vec_pack[index_pack-self.right_cnt]
        elif vec_pack[index_pack-self.right_cnt] in vec_pack_mask:
            port = vec_pack[index_pack-self.right_cnt]
        else:
            port = vec_pack_mask[index_pack_mask]

        # Apply pruning to the selected port.
        if pkt_rid == node_rid or pkt_rid == t.get_yid_tbl().global_rid():
            if t.get_yid_tbl().is_port_pruned(yid, port):
                return None
        if port in vec_pack_mask:
            # Port is up
            return port
        else:
            # Port is down, use the backup instead.
            return t.get_backup_port( port )
    def cleanUp(self):
        self.setRmtCnt(0,0)
        self.rmvMbr(self.mbrs)
class LagTable:
    def __init__(self, test, shdl, dev):
        self.shdl = shdl
        self.dev = dev
        self.test = test
        self.lags = []
        for i in range(255):
            lag = LagGrp(self.test, self.shdl, self.dev, i)
            self.lags.append(lag)
    def getLag(self, lag_id):
        assert lag_id >= 0 and lag_id <= 255
        return self.lags[lag_id]
    def cleanUp(self):
        for lag in self.lags:
            lag.cleanUp()

class EcmpGrp:
    def __init__(self, test, shdl, dev):
        self.shdl = shdl
        self.dev = dev
        self.test = test
        self.mbrs = [None for _ in range(32)]
        self.mgrp_hdls = []
        self.hdl = test.mc.mc_ecmp_create(hex_to_i32(self.shdl), self.dev)
        assert(self.hdl != 0)
    def addMbr(self, rid, port_list, lag_list):
        l1 = L1Node(self.test, self.shdl, rid, self.dev)
        l1.addMbrs(self.test, port_list, lag_list)
        self.test.mc.mc_ecmp_mbr_add(hex_to_i32(self.shdl), hex_to_i32(self.dev), hex_to_i32(self.hdl), hex_to_i32(l1.l1_hdl))
        self.mbrs[ self.mbrs.index(None) ] = l1
    def rmvMbr(self, index):
        l1 = self.mbrs[index]
        self.test.mc.mc_ecmp_mbr_rem(hex_to_i32(self.shdl), hex_to_i32(self.dev), hex_to_i32(self.hdl), hex_to_i32(l1.l1_hdl))
        self.mbrs[index] = None
        l1.cleanUp(self.test)
    def getMbrs(self):
        count = self.test.mc.mc_ecmp_get_mbr_count(self.shdl, self.dev, self.hdl)
        mbrs = []
        if count == 0:
            return mbrs
        mbrs.append(self.test.mc.mc_ecmp_get_first_mbr(self.shdl, self.dev, self.hdl))
        if count > 1:
            mbrs += self.test.mc.mc_ecmp_get_next_i_mbr(self.shdl, self.dev, self.hdl, mbrs[0], count - 1)
        return mbrs
    def associate(self, mgrp_hdl, xid):
        if xid is None:
            xid = 0
            use_xid = 0
        else:
            use_xid = 1
        self.test.mc.mc_associate_ecmp(hex_to_i32(self.shdl),
                                       hex_to_i32(self.dev),
                                       hex_to_i32(mgrp_hdl),
                                       hex_to_i32(self.hdl),
                                       hex_to_i16(xid), use_xid)
        self.mgrp_hdls.append(mgrp_hdl)
    def dissociate(self, mgrp_hdl):
        self.mgrp_hdls.remove(mgrp_hdl)
        self.test.mc.mc_dissociate_ecmp(hex_to_i32(self.shdl),
                                        hex_to_i32(self.dev),
                                        hex_to_i32(mgrp_hdl),
                                        hex_to_i32(self.hdl))
    def getMbrByHash(self, val):
        #print("Group", hex(self.hdl), "getting member for hash", val)
        #print("  MBRS", self.mbrs)
        live_cnt = 32 - self.mbrs.count(None)
        #print("  Live:", live_cnt)
        idx1 = val % 32
        #print("  Select1:", idx1)
        idx2 = idx1
        if live_cnt != 0:
            idx2 = val % live_cnt
        #print("  Select2:", idx2)
        if self.mbrs[idx1] is not None:
            #print("  Taking member at", idx1)
            return self.mbrs[idx1]
        idx = 0
        for node in self.mbrs:
            if node is not None and idx == idx2:
                #print("  Taking member at", idx2)
                return node
            if node is not None:
                idx = idx + 1
        #print("  NO MEMBERS!")
        return None
    def getRid(self, h):
        node = self.getMbrByHash(h)
        if node is not None:
            return node.get_rid()
        return 0xDEAD
    def cleanUp(self):
        # Remove members
        for i in range(len(self.mbrs)):
            if self.mbrs[i] is not None:
                self.rmvMbr(i)
        # Dissociate mgids
        for mgrp_hdl in  self.mgrp_hdls:
            self.test.mc.mc_dissociate_ecmp(hex_to_i32(self.shdl),
                                            hex_to_i32(self.dev),
                                            hex_to_i32(mgrp_hdl),
                                            hex_to_i32(self.hdl))
        self.mgrp_hdls = []
        # Clean up ECMP group
        self.test.mc.mc_ecmp_destroy(hex_to_i32(self.shdl), hex_to_i32(self.dev), hex_to_i32(self.hdl))
        self.hdl = 0


class YidTable:
    def __init__(self, test, shdl, dev, global_rid):
        self.shdl = shdl
        self.dev = dev
        self.prune_list = []
        self.test = test
        for x in range(288):
            self.prune_list.append([])
        self.set_global_rid(global_rid)
    def set_global_rid(self, grid):
        self.grid = grid
        self.test.mc.mc_set_global_rid(hex_to_i32(self.shdl), self.dev, hex_to_i16(grid))
    def global_rid(self):
        return self.grid
    def set_pruned_ports(self, yid, new_prune_list):
        self.prune_list[yid] = list(new_prune_list)
        prune_map = set_port_map(self.prune_list[yid])
        self.test.mc.mc_update_port_prune_table(hex_to_i32(self.shdl), self.dev, yid, prune_map)
    def get_pruned_ports(self, yid):
        return list(self.prune_list[yid])
    def prune_ports(self, yid, port_list):
        l = list(port_list)
        for p in l:
            if p in self.prune_list[yid]:
                port_list.remove(p)
    def is_port_pruned(self, yid, port):
        if port in self.prune_list[yid]:
            return True
        return False
    def cleanUp(self):
        for yid in range(288):
            self.set_pruned_ports(yid, [])
        self.set_global_rid(0)

class MCTree:
    def __init__(self, test, shdl, dev, mgid):
        self.shdl = shdl
        self.dev  = dev
        self.mgid = mgid
        self.test = test
        self.mgid_hdl = self.test.mc.mc_mgrp_create(hex_to_i32(self.shdl), self.dev, hex_to_i16(mgid))
        self.nodes = []
        self.ecmps = []
    def add_node(self, rid, xid, mbr_ports, mbr_lags):
        l1 = L1Node(self.test, self.shdl, rid, self.dev)
        l1.addMbrs(self.test, mbr_ports, mbr_lags)
        l1.associate(self.test, self.mgid_hdl, xid)
        if test_param_get("target") != "bmv2":
            assoc = l1.is_associated(self.test)
            self.test.assertTrue(assoc.is_associated)
            self.test.assertEqual(assoc.mgrp_hdl, self.mgid_hdl)
        self.nodes.append(l1)
    def rmv_last_node(self):
        l1 = self.nodes[-1]
        self.nodes = self.nodes[:-1]
        l1.cleanUp(self.test)
    def add_ecmp(self, grp, xid):
        grp.associate(self.mgid_hdl, xid)
        grp_tup = (grp, xid)
        self.ecmps.append(grp_tup)
    def reprogram(self):
        for l1 in self.nodes:
            l1.dissociate(self.test)
        for grp, xid in self.ecmps:
            grp.dissociate(self.mgid_hdl)
        for l1 in self.nodes:
            l1.associate(self.test, self.mgid_hdl, l1.xid)
        for grp, xid in self.ecmps:
            grp.associate(self.mgid_hdl, xid)
    def cleanUp(self):
        for l1 in self.nodes:
            l1.cleanUp(self.test)
        for grp, _ in self.ecmps:
            grp.dissociate(self.mgid_hdl)
        self.test.mc.mc_mgrp_destroy(hex_to_i32(self.shdl), hex_to_i32(self.dev), hex_to_i32(self.mgid_hdl))
        self.nodes = []
        self.ecmps = []
    def get_ports(self, pkt_rid, pkt_xid, pkt_yid, pkt_hash1=0, pkt_hash2=0, verify_api=False):
        port_data = []
        ecmp_data = []
        for l1 in self.nodes:
            # Check L1 pruning
            if l1.xid is not None and pkt_xid == l1.xid:
                continue
            ports = l1.getPorts(self.test, pkt_rid, pkt_xid, pkt_yid, pkt_hash2)
            port_data.append( (l1.rid, ports) )

        for grp, xid in self.ecmps:
            if verify_api:
                ecmp_mbr = self.test.mc.mc_ecmp_get_mbr_from_hash(hex_to_i32(self.shdl),
                                                                  hex_to_i32(self.dev),
                                                                  hex_to_i32(self.mgid_hdl),
                                                                  hex_to_i32(grp.hdl),
                                                                  hex_to_i16(pkt_hash1),
                                                                  hex_to_i16(pkt_xid))
            if xid is not None and pkt_xid == xid:
                if verify_api:
                    assert ecmp_mbr.is_pruned == True
                continue
            l1 = grp.getMbrByHash(pkt_hash1)
            if verify_api:
                if l1 is None:
                    assert ecmp_mbr.is_pruned or ecmp_mbr.node_hdl == 0
                else:
                    assert l1.l1_hdl == ecmp_mbr.node_hdl and not ecmp_mbr.is_pruned
            if l1 is not None:
                ports = l1.getPorts(self.test, pkt_rid, pkt_xid, pkt_yid, pkt_hash2)
                ecmp_data.append( (l1.rid, ports) )
        return port_data + ecmp_data
    def print_tree(self):
        print("Dev:", self.dev, "MGID:", hex(self.mgid), "Num L1 Nodes:", len(self.nodes), "Num ECMPs:", len(self.ecmps))
        for l1 in self.nodes:
            if l1.xid is not None:
                print("  L1_Hdl:", hex(l1.l1_hdl), "RID:", hex(l1.rid), "XID:", hex(l1.xid), "Ports:", l1.mbr_ports, "LAGs:", l1.mbr_lags)
            else:
                print("  L1_Hdl:", hex(l1.l1_hdl), "RID:", hex(l1.rid), "XID:", l1.xid, "Ports:", l1.mbr_ports, "LAGs:", l1.mbr_lags)
        for grp, xid in self.ecmps:
            if xid is not None:
                print("  ECMP Hdl:", hex(grp.hdl), "XID:", hex(xid))
            else:
                print("  ECMP Hdl:", hex(grp.hdl), "L1 Members:",map(hex, grp.getMbrs()))
                for l1 in grp.mbrs:
                    if l1 is not None:
                        print("   ECMP->L1_Hdl:", hex(l1.l1_hdl), "RID:", hex(l1.rid), "XID:", l1.xid, "Ports:", l1.mbr_ports, "LAGs:", l1.mbr_lags)
                        for lag in l1.mbr_lags:
                            print("       ECMP->L1_Hdl->Lag_Hdl:", lag,  "Ports:",t.get_lag_tbl().getLag(lag).mbrs)



def add_port_entry(test, sess_hdl, dev_tgt, port, vlan_v, vlan, ifid):
    m_spec = multicast_test_ing_port_match_spec_t(port, vlan_v, vlan)
    a_spec = multicast_test_set_ifid_action_spec_t(hex_to_i32(ifid))
    return test.client.ing_port_table_add_with_set_ifid(sess_hdl, dev_tgt, m_spec, a_spec)
def add_ifid_entry(test, sess_hdl, dev_tgt, ifid, rid, yid, brid, h1, h2):
    m_spec = multicast_test_ing_src_ifid_match_spec_t(hex_to_i32(ifid))
    a_spec = multicast_test_set_src_ifid_md_action_spec_t(
                                                         hex_to_i16(rid),
                                                         yid,
                                                         hex_to_i16(brid),
                                                         hex_to_i16(h1),
                                                         hex_to_i16(h2))
    return test.client.ing_src_ifid_table_add_with_set_src_ifid_md(
                                              sess_hdl, dev_tgt, m_spec, a_spec)
def mod_ifid_entry(test, sess_hdl, dev_id, handle, rid, yid, brid, h1, h2):
    a_spec = multicast_test_set_src_ifid_md_action_spec_t(
                                                         hex_to_i16(rid),
                                                         yid,
                                                         hex_to_i16(brid),
                                                         hex_to_i16(h1),
                                                         hex_to_i16(h2))
    test.client.ing_src_ifid_table_modify_with_set_src_ifid_md(
                                              sess_hdl, dev_id, handle, a_spec)

def add_dmac_entry_to_flood(test, sess_hdl, dev_tgt, brid, mac):
    match_spec = multicast_test_ing_dmac_match_spec_t(hex_to_i16(brid), macAddr_to_string(mac))
    return test.client.ing_dmac_table_add_with_flood(sess_hdl, dev_tgt, match_spec)
def add_dmac_entry_to_route(test, sess_hdl, dev_tgt, brid, mac, vrf):
    match_spec = multicast_test_ing_dmac_match_spec_t(hex_to_i16(brid), macAddr_to_string(mac))
    action_spec = multicast_test_route_action_spec_t(hex_to_i16(vrf))
    return test.client.ing_dmac_table_add_with_route(sess_hdl, dev_tgt, match_spec, action_spec)
def add_ipv4_mcast_entry(test, sess_hdl, dev_tgt, priority, vrf, sip, sip_msk, dip, dip_msk, xid, mgid1, mgid2):
    match_spec = multicast_test_ing_ipv4_mcast_match_spec_t(hex_to_i16(vrf), sip, sip_msk, dip, dip_msk)
    action_spec = multicast_test_mcast_route_action_spec_t(hex_to_i16(xid), hex_to_i16(mgid1), hex_to_i16(mgid2))
    return test.client.ing_ipv4_mcast_table_add_with_mcast_route(sess_hdl, dev_tgt, match_spec, priority, action_spec)


swports = get_sw_ports()

# Set to true or false depending on if the testbed supports flapping the ports.
# Harlyn model supports it, other test environments might not...
support_hw_port_flap = True
if test_param_get("target") == "hw":
    support_hw_port_flap = False


class TestBasic(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])

    def runTest(self):
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        port_to_ifid = {}
        ifid_to_brid = {}
        brid_to_mgid = {}
        brid_to_l1 = {}
        ifids = set()
        brids = set()

        for p in swports:
            port_to_ifid[p] = p
            ifids.add( p )

        brids.update( random.sample(range(0, 0x10000), len(ifids)) )
        brid_list = list( brids )
        for ifid,brid in zip(ifids, brids):
            ifid_to_brid[ifid] = brid

        ing_port_entry_hdls = []
        ing_src_ifid_entry_hdls = []
        ucast_dmac_entry_hdls = []
        ucast_dmac_entries = []
        mcast_dmac_entry_hdls = []

        try:
            for port_num in swports:
                vlan_val = 0
                vlan_id = 0
                ifid = port_to_ifid[port_num]
                entry_hdl = add_port_entry(self, sess_hdl, dev_tgt, port_num, vlan_val, vlan_id, ifid)
                ing_port_entry_hdls.append(entry_hdl)

            for port_num in swports:
                ifid = port_to_ifid[port_num]
                rid = 0
                yid = 0
                brid = ifid_to_brid[ifid]
                hash1 = 1
                hash2 = 1
                entry_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, rid, yid, brid, hash1, hash2)
                ing_src_ifid_entry_hdls.append(entry_hdl)

            for i, port in enumerate(swports):
                ing_port = port
                egr_port = swports[(i+1) % len(swports)]
                ifid = port_to_ifid[ing_port]
                brid = ifid_to_brid[ifid]
                dmac = "00:11:11:11:11:00"
                match_spec = multicast_test_ing_dmac_match_spec_t(
                                    hex_to_i16(brid),
                                    macAddr_to_string(dmac))
                action_spec = multicast_test_switch_action_spec_t(
                                    hex_to_i16(egr_port))
                entry_hdl = self.client.ing_dmac_table_add_with_switch(
                                    sess_hdl,
                                    dev_tgt,
                                    match_spec,
                                    action_spec)
                ucast_dmac_entry_hdls.append(entry_hdl)

            for brid in brids:
                dmac = "FF:FF:FF:FF:FF:FF"
                match_spec = multicast_test_ing_dmac_match_spec_t(
                                    hex_to_i16(brid),
                                    macAddr_to_string(dmac))
                entry_hdl = self.client.ing_dmac_table_add_with_flood(
                                    sess_hdl,
                                    dev_tgt,
                                    match_spec)
                mcast_dmac_entry_hdls.append(entry_hdl)

            # Allocate an MGID per brid
            for brid in brids:
                mgrp_hdl = self.mc.mc_mgrp_create(mc_sess_hdl, dev_id, hex_to_i16(brid))
                brid_to_mgid[brid] = mgrp_hdl

            # Allocate a single L1 node for each brid
            for brid in brids:
                port_map = set_port_map([]);
                lag_map = set_lag_map([]);
                l1_hdl = self.mc.mc_node_create(mc_sess_hdl, dev_id, hex_to_i16((~brid)&0xFFFF), port_map, lag_map)
                brid_to_l1[brid] = l1_hdl

            # Add L2 nodes to the L1s
            ports_in_tree = {}
            # Add all ports to the first
            l2_node_ports = []
            l2_node_lags = []
            for p in swports:
                l2_node_ports.append(int(p))
            ports_in_tree[brid_list[0]] = sorted(l2_node_ports)
            port_map = set_port_map(l2_node_ports)
            lag_map = set_lag_map(l2_node_lags);
            self.mc.mc_node_update(mc_sess_hdl, dev_id, brid_to_l1[ brid_list[0] ], port_map, lag_map)
            self.mc.mc_associate_node(mc_sess_hdl, dev_id,
                                      brid_to_mgid[ brid_list[0] ],
                                      brid_to_l1[ brid_list[0] ],
                                      0,
                                      0)
            # Add no ports to the second
            l2_node_ports = []
            l2_node_lags = []
            ports_in_tree[brid_list[1]] = sorted(l2_node_ports)
            port_map = set_port_map(l2_node_ports);
            lag_map = set_lag_map(l2_node_lags);
            self.mc.mc_node_update(mc_sess_hdl, dev_id, brid_to_l1[ brid_list[1] ], port_map, lag_map)
            self.mc.mc_associate_node(mc_sess_hdl, dev_id,
                                      brid_to_mgid[ brid_list[1] ],
                                      brid_to_l1[ brid_list[1] ],
                                      0,
                                      0)
            # Add a random number of ports to the rest
            for i in range(2, len(brid_list)):
                num_ports = random.randint(0, len(swports)-1)
                l2_node_ports = random.sample(swports, num_ports)
                l2_node_lags = []
                ports_in_tree[brid_list[i]] = sorted(l2_node_ports)
                port_map = set_port_map(l2_node_ports)
                lag_map = set_lag_map(l2_node_lags);
                self.mc.mc_node_update(mc_sess_hdl, dev_id, brid_to_l1[ brid_list[i] ], port_map, lag_map)
                self.mc.mc_associate_node(mc_sess_hdl, dev_id,
                                          brid_to_mgid[ brid_list[i] ],
                                          brid_to_l1[ brid_list[i] ],
                                          0,
                                          0)

            # Need a dummy entry in the encode table.
            self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

            # Wait for a pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)

            # Wait for multicast APIs to complete.
            self.mc.mc_complete_operations(mc_sess_hdl)


            for i in range(0, len(swports)):
                print("Sending unicast on port", swports[i], "expecting receive on port", swports[(i+1)%len(swports)])
                sys.stdout.flush()
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:00',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst='10.0.0.1',
                                        ip_tos=255,
                                        ip_ttl=64,
                                        ip_id=101)
                send_packet(self,  swports[i] , pkt)
                epkt = simple_tcp_packet(eth_dst='00:11:11:11:11:00',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst='10.0.0.1',
                                        ip_tos=0,
                                        ip_ttl=64,
                                        ip_id=0)

                verify_packets(self, epkt, [swports[(i+1)%len(swports)]])

            for ing_port in swports:
                ifid = port_to_ifid[ing_port]
                brid = ifid_to_brid[ifid]
                rx_port_list = ports_in_tree[brid]
                rx_pkt_list = build_rx_packet_list(rx_port_list, simple_tcp_packet(eth_dst='FF:FF:FF:FF:FF:FF',
                                                                                   eth_src='00:22:22:22:22:22',
                                                                                   ip_src='1.1.1.1',
                                                                                   ip_dst='10.0.0.1',
                                                                                   ip_ttl=64,
                                                                                   ip_id=(~brid & 0xFFFF)))
                print("Sending multicast on port", ing_port, "expecting receive on ports", rx_port_list)
                sys.stdout.flush()
                pkt = simple_tcp_packet(eth_dst='FF:FF:FF:FF:FF:FF',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst='10.0.0.1',
                                        ip_tos=255,
                                        ip_ttl=64,
                                        ip_id=101)
                send_packet(self, ing_port, pkt)

                verify_packet_list(self, [rx_port_list], [rx_pkt_list])



        finally:
            for brid in brid_to_l1:
                self.mc.mc_dissociate_node(mc_sess_hdl, dev_id, brid_to_mgid[brid], brid_to_l1[brid])
                self.mc.mc_node_destroy(mc_sess_hdl, dev_id, brid_to_l1[brid])
            for brid in brid_to_mgid:
                self.mc.mc_mgrp_destroy(mc_sess_hdl, dev_id, brid_to_mgid[brid])
            for entry_hdl in ing_port_entry_hdls:
                self.client.ing_port_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ing_src_ifid_entry_hdls:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ucast_dmac_entry_hdls:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in mcast_dmac_entry_hdls:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestYid(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])

    def runTest(self):
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]
        num_ports = len(pipe_list)*72;

        # Legal YID values
        yid_space = []
        for yid in range(0, 288):
            yid_space.append(yid)
        # Shuffle them so we can get a random set just by taking a few from
        # the front of the list.
        random.shuffle(yid_space)

        # Legal port values
        port_space = []
        for bit_number in range(0,num_ports):
            port = BitIdxToPort(bit_number)
            if (test_param_get("arch") == "tofino3") and ((port % 2) != 0):
                continue
            port_space.append(port)

        port_list = swports

        brid_list = [0x0001,
                     0x8000]

        ifid_list = []
        ifid = 0
        port_to_ifid = {}
        for port in port_list:
            port_to_ifid[port] = ifid
            ifid_list.append(ifid)
            ifid = ifid + 1

        brid_to_mgid = {}
        brid_to_l1 = {}
        yid_to_prune_list = {}
        ifid_to_yid = {}
        ifid_to_brid = {}

        ing_port_entry_hdls = []
        ing_src_ifid_entry_hdls = []
        mcast_dmac_entry_hdls = []


        try:
            self.mc.mc_set_max_nodes_before_yield(mc_sess_hdl, dev_id, 255)
            # Add Ing Port table entries
            for port in port_list:
                entry_hdl = add_port_entry(self, sess_hdl, dev_tgt, port, 0, 0, port_to_ifid[port])
                ing_port_entry_hdls.append(entry_hdl)

            # Add Ing Src IFID table entries
            i = 0
            for ifid in ifid_list:
                ifid_to_yid[ifid] = yid_space[i]
                ifid_to_brid[ifid] = brid_list[i % 2]
                i = i + 1
                entry_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, 0x4321,
                                           ifid_to_yid[ifid], ifid_to_brid[ifid], 0xFFFF, 0xEEEE)
                ing_src_ifid_entry_hdls.append(entry_hdl)

            # Add DMAC entries to flood
            for brid  in brid_list:
                match_spec = multicast_test_ing_dmac_match_spec_t(
                                    hex_to_i16(brid),
                                    macAddr_to_string("E4:83:A6:93:02:4B"))
                #action_spec = multicast_test_flood_action_spec_t()
                entry_hdl = self.client.ing_dmac_table_add_with_flood(
                                    sess_hdl,
                                    dev_tgt,
                                    match_spec)
                                    #action_spec)
                mcast_dmac_entry_hdls.append(entry_hdl)





            # Allocate an MGID per brid
            for brid in brid_list:
                mgrp_hdl = self.mc.mc_mgrp_create(mc_sess_hdl, dev_id, hex_to_i16(brid))
                brid_to_mgid[brid] = mgrp_hdl

            # Allocate a single L1 node for each brid
            for brid in brid_list:
                port_map = set_port_map([])
                lag_map = set_lag_map([])
                l1_hdl = self.mc.mc_node_create(mc_sess_hdl, dev_id, hex_to_i16(0x4321), port_map, lag_map)
                brid_to_l1[brid] = l1_hdl

            # Add L2 nodes to the L1s
            ports_in_tree = {}
            # Add all ports to the L1s
            for i in range(0, len(brid_list)):
                ports_in_tree[brid_list[i]] = port_list
                port_map = set_port_map(port_list)
                lag_map = set_lag_map([])
                self.mc.mc_node_update(mc_sess_hdl, dev_id, brid_to_l1[ brid_list[i] ], port_map, lag_map)
                self.mc.mc_associate_node(mc_sess_hdl, dev_id,
                                          brid_to_mgid[ brid_list[i] ],
                                          brid_to_l1[ brid_list[i] ],
                                          0,
                                          0)

            # Need a dummy entry in the encode table.
            self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

            # Program the YID table.
            # The YID used by the first ifid will prune all ports
            # The YID used by the second table will prune no ports
            self.mc.mc_begin_batch( mc_sess_hdl )
            for i in range(0, len(ifid_list)):
                yid = yid_space[i]
                pruned_ports = []
                if 0 == i:
                    port_cnt = len(port_space)
                    pruned_ports = port_space
                elif 1 == i:
                    port_cnt = 0
                    pruned_ports = []
                else:
                    port_cnt = random.randint(0, len(port_space))
                    pruned_ports = random.sample(port_space, port_cnt)
                yid_to_prune_list[yid] = pruned_ports
                prune_map = set_port_map(pruned_ports)
                self.mc.mc_update_port_prune_table(mc_sess_hdl, dev_id, yid, prune_map)
                self.mc.mc_flush_batch( mc_sess_hdl )

            # Wait for a pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)

            # Wait for multicast APIs to complete.
            self.mc.mc_end_batch( mc_sess_hdl, True )

            if test_param_get("target") != "bmv2":
                for i in range(0, len(ifid_list)):
                    yid = yid_space[i]
                    pruned_ports = yid_to_prune_list[yid]
                    # For the get call we expect the extended port map.
                    prune_map = set_port_map(pruned_ports)

                    d = self.mc.mc_get_port_prune_table(mc_sess_hdl, dev_id, yid, False)
                    self.assertEqual(prune_map, d)
                    d = self.mc.mc_get_port_prune_table(mc_sess_hdl, dev_id, yid, True)
                    self.assertEqual(prune_map, d)

            self.mc.mc_set_max_nodes_before_yield( mc_sess_hdl, dev_id, 0xFF )
            self.mc.mc_set_max_node_threshold( mc_sess_hdl, dev_id, 16384, 250)
            self.mc.mc_complete_operations(mc_sess_hdl)

            for port in port_list:
                ifid = port_to_ifid[port]
                yid  = ifid_to_yid[ifid]
                brid = ifid_to_brid[ifid]
                prune_list = yid_to_prune_list[yid]
                mbr_port_list = ports_in_tree[brid]
                # Remove ports in prune_list from mbr_port_list
                rx_port_list = []
                for p in mbr_port_list:
                    if p not in prune_list:
                        rx_port_list.append(p)
                #rx_port_list.sort()
                #rx_pkt_list = []
                #last_pipe = -1
                #for rx_port in rx_port_list:
                #    if last_pipe == port_to_pipe(rx_port):
                #        rid_first = 0
                #    else:
                #        rid_first = 1
                #        last_pipe = port_to_pipe(rx_port)
                #    rx_pkt_list.append(simple_tcp_packet(eth_dst='E4:83:A6:93:02:4B',
                #                                         eth_src='00:22:22:22:22:22',
                #                                         ip_src='1.1.1.1',
                #                                         ip_dst='10.0.0.1',
                #                                         ip_tos=rid_first,
                #                                         ip_ttl=64,
                #                                         ip_id=0x4321))
                print("Sending multicast on port", port, "expecting receive on ports", rx_port_list)
                sys.stdout.flush()
                rx_pkt_list = build_rx_packet_list(rx_port_list, simple_tcp_packet(eth_dst='E4:83:A6:93:02:4B',
                                                                                   eth_src='00:22:22:22:22:22',
                                                                                   ip_src='1.1.1.1',
                                                                                   ip_dst='10.0.0.1',
                                                                                   ip_ttl=64,
                                                                                   ip_id=0x4321))
                pkt = simple_tcp_packet(eth_dst='E4:83:A6:93:02:4B',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                send_packet(self, port, pkt)
                verify_packet_list(self, [rx_port_list], [rx_pkt_list])

            # Change ingress RID so that it no longer matches the L1 node RID.
            x = list(ing_src_ifid_entry_hdls)
            for entry_hdl in x:
                ing_src_ifid_entry_hdls.remove(entry_hdl)
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
            i = 0
            for ifid in ifid_list:
                ifid_to_yid[ifid] = yid_space[i]
                ifid_to_brid[ifid] = brid_list[i % 2]
                i = i + 1
                entry_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, 0x4322,
                                           ifid_to_yid[ifid], ifid_to_brid[ifid], 0xFFFF, 0xEEEE)
                ing_src_ifid_entry_hdls.append(entry_hdl)

            # Send traffic again and nothing should be pruned
            self.conn_mgr.complete_operations(sess_hdl)
            for port in port_list:
                ifid = port_to_ifid[port]
                yid  = ifid_to_yid[ifid]
                brid = ifid_to_brid[ifid]
                mbr_port_list = ports_in_tree[brid]
                rx_port_list = []
                for p in mbr_port_list:
                    rx_port_list.append(p)
                print("Sending multicast on port", port, "expecting receive on ports (NO PRUNING)", rx_port_list)
                sys.stdout.flush()
                pkt = simple_tcp_packet(eth_dst='E4:83:A6:93:02:4B',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                rx_pkt_list = build_rx_packet_list(rx_port_list, simple_tcp_packet(eth_dst='E4:83:A6:93:02:4B',
                                                                                   eth_src='00:22:22:22:22:22',
                                                                                   ip_src='1.1.1.1',
                                                                                   ip_dst='10.0.0.1',
                                                                                   ip_id=0x4321,
                                                                                   ip_ttl=64))
                send_packet(self, port, pkt)
                verify_packet_list(self, [rx_port_list], [rx_pkt_list])

            # Wait for multicast APIs to complete.
            self.mc.mc_complete_operations(mc_sess_hdl)

            # Program the Global RID register to match the iRID and pruning
            # should happen again.
            self.mc.mc_set_global_rid(mc_sess_hdl, dev_id, hex_to_i16(0x4322))
            self.mc.mc_complete_operations(mc_sess_hdl)
            for port in port_list:
                ifid = port_to_ifid[port]
                yid  = ifid_to_yid[ifid]
                brid = ifid_to_brid[ifid]
                prune_list = yid_to_prune_list[yid]
                mbr_port_list = ports_in_tree[brid]
                # Remove ports in prune_list from mbr_port_list
                rx_port_list = []
                for p in mbr_port_list:
                    if p not in prune_list:
                        rx_port_list.append(p)
                print("Sending multicast on port", port, "expecting receive on ports (GLOBAL PRUNE)", rx_port_list)
                sys.stdout.flush()
                pkt = simple_tcp_packet(eth_dst='E4:83:A6:93:02:4B',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                rx_pkt_list = build_rx_packet_list(rx_port_list, simple_tcp_packet(eth_dst='E4:83:A6:93:02:4B',
                                                                                   eth_src='00:22:22:22:22:22',
                                                                                   ip_src='1.1.1.1',
                                                                                   ip_dst='10.0.0.1',
                                                                                   ip_id=0x4321,
                                                                                   ip_ttl=64))
                send_packet(self, port, pkt)
                verify_packet_list(self, [rx_port_list], [rx_pkt_list])

        finally:
            print("Cleaning up")
            sys.stdout.flush()
            self.mc.mc_set_max_nodes_before_yield(mc_sess_hdl, dev_id, 16)
            self.mc.mc_set_global_rid(mc_sess_hdl, dev_id, hex_to_i16(0x0000))
            for i in range(0, len(ifid_list)):
                yid = yid_space[i]
                prune_map = set_port_map([])
                self.mc.mc_update_port_prune_table(mc_sess_hdl, dev_id, yid, prune_map)
            for brid in brid_to_l1:
                self.mc.mc_dissociate_node(mc_sess_hdl, dev_id, brid_to_mgid[brid], brid_to_l1[brid])
                self.mc.mc_node_destroy(mc_sess_hdl, dev_id, brid_to_l1[brid])
            for brid in brid_to_mgid:
                self.mc.mc_mgrp_destroy(mc_sess_hdl, dev_id, brid_to_mgid[brid])
            for entry_hdl in ing_port_entry_hdls:
                self.client.ing_port_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ing_src_ifid_entry_hdls:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in mcast_dmac_entry_hdls:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestXid(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])

    def runTest(self):
        global t
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]
        num_ports = len(pipe_list) * 72
        t.setup(self, sess_hdl, mc_sess_hdl)

        # Legal YID values
        yid_space = []
        for yid in range(0, 288):
            yid_space.append(yid)
        # Shuffle them so we can get a random set just by taking a few from
        # the front of the list.
        random.shuffle(yid_space)

        # Legal port values
        port_space = []
        for bit_number in range(0,num_ports):
            port = BitIdxToPort(bit_number)
            port_space.append(port)

        # Ports this test uses.
        port_list = swports

        brid_list = [0x9347]

        ip_addr_str_list = ["224.100.0.0",
                            "224.100.0.1",
                            "224.100.0.2",
                            "224.100.0.3",
                            "224.100.0.4",
                            "224.100.0.5",
                            "224.100.0.6",
                            "224.100.0.7"]
        ip_addr_list = []
        for ip in ip_addr_str_list:
            ip_addr_list.append(ipv4Addr_to_i32(ip))
        ip_to_mgid1 = {}
        ip_to_mgid2 = {}
        ip_to_xid = {}
        xid_list = random.sample(list(range(0x10000)), len(ip_addr_list))
        mgid_sample_space = random.sample(list(range(1, 0x10000)), 2*len(ip_addr_list))
        for ip in ip_addr_list:
            ip_to_mgid1[ip] = mgid_sample_space.pop(0)
            ip_to_mgid2[ip] = mgid_sample_space.pop(0)
            ip_to_xid[ip]   = xid_list.pop(0)
            mgid_sample_space.append(ip_to_mgid1[ip])
            mgid_sample_space.append(ip_to_mgid2[ip])
            xid_list.append(ip_to_xid[ip])

        ifid_list = []
        ifid = 0
        port_to_ifid = {}
        for port in port_list:
            port_to_ifid[port] = ifid
            ifid_list.append(ifid)
            ifid = ifid + 1

        ifid_to_yid = {}
        ifid_to_brid = {}

        mc_trees = {}
        ing_port_entry_hdls = []
        ing_src_ifid_entry_hdls = []
        mcast_dmac_entry_hdls = []
        ip_addr_entry_hdls = []


        try:
            # Add Ing Port table entries
            for port in port_list:
                entry_hdl = add_port_entry(self, sess_hdl, dev_tgt, port, 0, 0, port_to_ifid[port])
                ing_port_entry_hdls.append(entry_hdl)

            # Add Ing Src IFID table entries
            i = 0
            for ifid in ifid_list:
                ifid_to_yid[ifid] = yid_space[i]
                ifid_to_brid[ifid] = brid_list[0]
                i = i + 1
                entry_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, 0x4321,
                                              ifid_to_yid[ifid], ifid_to_brid[ifid], 0x1FFF, 0x0EEE)
                ing_src_ifid_entry_hdls.append(entry_hdl)


            # Add DMAC entries to route
            for brid  in brid_list:
                entry_hdl = add_dmac_entry_to_route(self, sess_hdl, dev_tgt, brid, "EE:EE:EE:EE:EE:EE", brid)
                mcast_dmac_entry_hdls.append(entry_hdl)

            # Add IP entries
            for ip in ip_addr_list:
                src_ip = 0
                src_ip_mask = 0
                dst_ip_mask = ipv4Addr_to_i32("255.255.255.255")
                mgid1 = ip_to_mgid1[ip]
                mgid2 = ip_to_mgid2[ip]
                xid = ip_to_xid[ip]
                entry_hdl = add_ipv4_mcast_entry(self, sess_hdl, dev_tgt, 0, brid_list[0], src_ip, src_ip_mask, ip, dst_ip_mask, xid, mgid1, mgid2)
                ip_addr_entry_hdls.append(entry_hdl)


            # Build the multicast trees, four nodes for each MGID.
            for mgid in mgid_sample_space:
                mct = MCTree(self, mc_sess_hdl, dev_id, mgid)
                for x in range(4):
                    xid = random.choice(xid_list)
                    if 1 == random.choice([1,2,3,4]):
                        xid = None
                    mct.add_node(0x4320+x, xid, port_list, [])
                mc_trees[mgid] = mct


            # Need a dummy entry in the encode table.
            self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

            # Program the YID table.
            # The YID used by the first ifid will prune all ports
            # The YID used by the second table will prune no ports
            for i in range(0, len(ifid_list)):
                yid = yid_space[i]
                pruned_ports = []
                if 0 == i:
                    t.get_yid_tbl().set_pruned_ports(yid, port_list)
                elif 1 == i:
                    t.get_yid_tbl().set_pruned_ports(yid, [])
                else:
                    port_cnt = random.randint(0,len(port_list))
                    pruned_ports = random.sample(port_list, port_cnt)
                    t.get_yid_tbl().set_pruned_ports(yid, pruned_ports)

            # Wait for all pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)

            # Wait for multicast APIs to complete.
            self.mc.mc_complete_operations(mc_sess_hdl)

            test_ports = random.sample(port_list, len(port_list)//2)
            for port in test_ports:
                test_addrs = random.sample(ip_addr_list, 2)
                for ip in test_addrs:
                    rid = 0x4321
                    tree1 = mc_trees[ ip_to_mgid1[ip] ]
                    tree2 = mc_trees[ ip_to_mgid2[ip] ]
                    ifid = port_to_ifid[port]
                    yid  = ifid_to_yid[ifid]
                    xid = ip_to_xid[ip]
                    exp_data_list_1 = tree1.get_ports(rid, xid, yid)
                    exp_data_list_2 = tree2.get_ports(rid, xid, yid)
                    rx_port_list = []
                    rx_pkt_list = []
                    pkt = simple_tcp_packet(eth_dst='EE:EE:EE:EE:EE:EE',
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst=ip_addr_str_list[ ip_addr_list.index(ip) ],
                                            ip_ttl=64,
                                            ip_id=0xABCD)

                    #print("XID", hex(xid), "YID", hex(yid), "Grp1", hex(ip_to_mgid1[ip]), "Grp2", hex(ip_to_mgid2[ip]))
                    #tree1.print_tree()
                    #tree2.print_tree()
                    for exp_data in exp_data_list_1 + exp_data_list_2:
                        exp_rid, exp_ports = exp_data
                        rx_port_list.append(exp_ports)
                        exp_pkt = simple_tcp_packet(eth_dst='EE:EE:EE:EE:EE:EE',
                                                    eth_src='AA:BB:CC:DD:EE:FF',
                                                    ip_src='1.1.1.1',
                                                    ip_dst=ip_addr_str_list[ ip_addr_list.index(ip) ],
                                                    ip_ttl=63,
                                                    ip_id=exp_rid)
                        rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))
                        #print("RID:", hex(exp_rid), "Ports:", exp_ports)

                    print("Sending", ip_addr_str_list[ ip_addr_list.index(ip) ], "on port", port, "expecting receive on ports:", rx_port_list)
                    print()
                    sys.stdout.flush()
                    send_packet(self, port, pkt)
                    verify_packet_list(self, rx_port_list, rx_pkt_list)


        finally:
            print("Cleaning up")
            sys.stdout.flush()
            for entry_hdl in ing_port_entry_hdls:
                self.client.ing_port_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ing_src_ifid_entry_hdls:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in mcast_dmac_entry_hdls:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ip_addr_entry_hdls:
                self.client.ing_ipv4_mcast_table_delete(sess_hdl, dev_id, entry_hdl)
            for key in mc_trees:
                mc_trees[key].cleanUp()
            t.cleanUp()

            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestEcmp(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])

    def runTest(self):
        global t
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]
        num_ports = len(pipe_list) * 72
        t.setup(self, sess_hdl, mc_sess_hdl)

        # Legal YID values
        yid_space = []
        for yid in range(0, 288):
            yid_space.append(yid)
        # Shuffle them so we can get a random set just by taking a few from
        # the front of the list.
        random.shuffle(yid_space)

        # Legal port values
        port_space = []
        for bit_number in range(0,num_ports):
            port = BitIdxToPort(bit_number)
            port_space.append(port)

        # Ports this test uses.
        port_list = swports

        brid_list = [0x9347]

        ip_addr_str_list = ["224.100.0.0",
                            "224.100.0.1",
                            "224.100.0.2",
                            "224.100.0.3"]
        ip_addr_list = []
        for ip in ip_addr_str_list:
            ip_addr_list.append(ipv4Addr_to_i32(ip))
        ip_to_mgid1 = {}
        ip_to_mgid2 = {}
        ip_to_xid = {}
        xid_list = random.sample(list(range(0x10000)), len(ip_addr_list))
        mgid_sample_space = random.sample(list(range(1, 0x10000)), 2*len(ip_addr_list))
        for ip in ip_addr_list:
            ip_to_mgid1[ip] = mgid_sample_space.pop(0)
            ip_to_mgid2[ip] = mgid_sample_space.pop(0)
            ip_to_xid[ip]   = xid_list.pop(0)
            mgid_sample_space.append(ip_to_mgid1[ip])
            mgid_sample_space.append(ip_to_mgid2[ip])
            xid_list.append(ip_to_xid[ip])

        ifid_list = []
        ifid = 0
        port_to_ifid = {}
        for port in port_list:
            port_to_ifid[port] = ifid
            ifid_list.append(ifid)
            ifid = ifid + 1

        ifid_to_yid = {}
        ifid_to_brid = {}

        mc_trees = {}
        ecmp_grps = []
        ing_port_entry_hdls = []
        ing_src_ifid_entry_hdls = []
        mcast_dmac_entry_hdls = []
        ip_addr_entry_hdls = []


        try:
            # Add Ing Port table entries
            for port in port_list:
                entry_hdl = add_port_entry(self, sess_hdl, dev_tgt, port, 0, 0, port_to_ifid[port])
                ing_port_entry_hdls.append(entry_hdl)

            # Add Ing Src IFID table entries
            i = 0
            for ifid in ifid_list:
                ifid_to_yid[ifid] = yid_space[i]
                ifid_to_brid[ifid] = brid_list[0]
                i = i + 1
                entry_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, 0x4321,
                                              ifid_to_yid[ifid], ifid_to_brid[ifid], 0x1FFF, 0x0EEE)
                ing_src_ifid_entry_hdls.append(entry_hdl)


            # Add DMAC entries to route
            for brid  in brid_list:
                match_spec = multicast_test_ing_dmac_match_spec_t(
                                    hex_to_i16(brid),
                                    macAddr_to_string("EE:EE:EE:EE:EE:EE"))
                # Let brid equal vrf
                action_spec = multicast_test_route_action_spec_t(hex_to_i16(brid))
                entry_hdl = self.client.ing_dmac_table_add_with_route(
                                    sess_hdl,
                                    dev_tgt,
                                    match_spec,
                                    action_spec)
                mcast_dmac_entry_hdls.append(entry_hdl)

            # Add IP entries
            for ip in ip_addr_list:
                src_ip = 0
                src_ip_mask = 0
                dst_ip_mask = ipv4Addr_to_i32("255.255.255.255")
                mgid1 = ip_to_mgid1[ip]
                mgid2 = ip_to_mgid2[ip]
                xid = ip_to_xid[ip]
                match_spec = multicast_test_ing_ipv4_mcast_match_spec_t(
                        hex_to_i16(brid_list[0]), src_ip, src_ip_mask, ip, dst_ip_mask)
                #action_spec = multicast_test_mcast_route_action_spec_t(
                #        hex_to_i16(xid), hex_to_i16(mgid1), hex_to_i16(mgid2))
                action_spec = multicast_test_mcast_route_action_spec_t(
                        hex_to_i16(xid), hex_to_i16(mgid1), hex_to_i16(0))
                entry_hdl = self.client.ing_ipv4_mcast_table_add_with_mcast_route(sess_hdl, dev_tgt, match_spec, 0, action_spec)
                ip_addr_entry_hdls.append(entry_hdl)


            # Setup some LAGs for the ECMP groups to use.
            lag_ids = random.sample(list(range(255)), 8)
            for i in lag_ids:
                count = random.randint(0, len(port_list))
                mbrs = random.sample(port_list, count)
                t.get_lag_tbl().getLag(i).addMbr(mbrs)
                # Set left and right counts 20% of the time
                for i in lag_ids:
                    if random.randint(1,5) == 1:
                        left = random.randint(0, 8)
                        right = random.randint(0, 3)
                        lag = t.get_lag_tbl().getLag(i)
                        lag.setRmtCnt(left, right)

            # Build the ECMP groups.  Each group should have 0 to 32 members.
            for g_idx in range(8):
                grp = EcmpGrp(self, mc_sess_hdl, dev_id)
                # One group with no members
                if g_idx == 0:
                    pass
                # One group with 32 members
                elif g_idx == 1:
                    for rid in range(32):
                        ports = random.sample(port_list, random.randint(0,len(port_list)))
                        lags = random.sample(lag_ids, random.randint(0, len(lag_ids)))
                        node_rid = rid | (g_idx << 8)
                        grp.addMbr(node_rid, ports, lags)
                # Other groups with random members, do Add-Del to ensure there
                # are holes in the group
                else:
                    add_cnt = 32
                    del_cnt = random.randint(0, add_cnt)
                    for rid in range(add_cnt):
                        ports = random.sample(port_list, random.randint(0,len(port_list)))
                        lags = random.sample(lag_ids, random.randint(0, len(lag_ids)))
                        node_rid = rid | (g_idx << 8)
                        grp.addMbr(node_rid, ports, lags)
                    for del_idx in random.sample(list(range(add_cnt)), del_cnt):
                        grp.rmvMbr(del_idx)
                mbrs = grp.getMbrs()
                off = 0
                for i in range(0, len(mbrs)):
                    while(grp.mbrs[i + off] is None):
                        off += 1
                    self.assertEqual(mbrs[i], grp.mbrs[i + off].l1_hdl)
                ecmp_grps.append(grp)

            test_count = self.mc.mc_ecmp_get_count(mc_sess_hdl, dev_id)
            test_grp = []
            print(test_count)
            if test_count > 0:
                test_grp.append(self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id))
                if test_count > 1:
                    test_grp += self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, test_grp[0], test_count - 1)
            for i in range(0, len(ecmp_grps)):
                self.assertTrue(ecmp_grps[i].hdl in test_grp)



            # Build the multicast trees, four nodes for each MGID.
            for mgid in mgid_sample_space:
                mct = MCTree(self, mc_sess_hdl, dev_id, mgid)
                for x in range(4):
                    # Optionally add a node
                    if 1 == random.choice([1,2]):
                        xid = random.choice(xid_list)
                        if 1 == random.choice([1,2,3,4]):
                            xid = None
                        mct.add_node(0x4320+x, xid, port_list, [])

                # Add half the ECMP groups
                for g in ecmp_grps[:len(ecmp_grps)//2]:
                    xid = random.choice(xid_list)
                    if 1 == random.choice([1,2,3,4]):
                        xid = None
                    mct.add_ecmp(g, xid)

                    # Optionally add a node
                for x in range(4):
                    if 1 == random.choice([1,2]):
                        xid = random.choice(xid_list)
                        if 1 == random.choice([1,2,3,4]):
                            xid = None
                        mct.add_node(0x4320+x, xid, port_list, [])

                # Add remaining half of the ECMP groups
                for g in ecmp_grps[len(ecmp_grps)//2:]:
                    xid = random.choice(xid_list)
                    if 1 == random.choice([1,2,3,4]):
                        xid = None
                    mct.add_ecmp(g, xid)

                for x in range(4):
                    # Optionally add a node
                    if 1 == random.choice([1,2]):
                        xid = random.choice(xid_list)
                        if 1 == random.choice([1,2,3,4]):
                            xid = None
                        mct.add_node(0x4320+x, xid, port_list, [])

                mc_trees[mgid] = mct

            # Need a dummy entry in the encode table.
            self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

            # Program the YID table.
            # The YID used by the first ifid will prune all ports
            # The YID used by the second table will prune no ports
            for i in range(0, len(ifid_list)):
                yid = yid_space[i]
                pruned_ports = []
                if 0 == i:
                    t.get_yid_tbl().set_pruned_ports(yid, port_list)
                elif 1 == i:
                    t.get_yid_tbl().set_pruned_ports(yid, [])
                else:
                    port_cnt = random.randint(0,len(port_list))
                    pruned_ports = random.sample(port_list, port_cnt)
                    t.get_yid_tbl().set_pruned_ports(yid, pruned_ports)

            # Wait for all pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)

            # Wait for multicast APIs to complete.
            self.mc.mc_complete_operations(mc_sess_hdl)

            test_ports = random.sample(port_list, len(port_list)//2)
            for port in test_ports:
                for ip in ip_addr_list:
                    rid = 0x4321
                    tree1 = mc_trees[ ip_to_mgid1[ip] ]
                    #tree2 = mc_trees[ ip_to_mgid2[ip] ]
                    ifid = port_to_ifid[port]
                    yid  = ifid_to_yid[ifid]
                    xid = ip_to_xid[ip]
                    exp_data_list_1 = tree1.get_ports(rid, xid, yid, 0x1FFF, 0x0EEE)
                    #exp_data_list_2 = tree2.get_ports(rid, xid, yid, 0x1FFF, 0x0EEE)
                    rx_port_list = []
                    rx_pkt_list = []
                    pkt = simple_tcp_packet(eth_dst='EE:EE:EE:EE:EE:EE',
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst=ip_addr_str_list[ ip_addr_list.index(ip) ],
                                            ip_ttl=64,
                                            ip_id=0xABCD)

                    #print("XID", hex(xid), "YID", hex(yid), "Grp1", hex(ip_to_mgid1[ip]), "Grp2", hex(ip_to_mgid2[ip]))
                    #tree1.print_tree()
                    #tree2.print_tree()
                    for exp_data in exp_data_list_1:
                    #for exp_data in exp_data_list_1 + exp_data_list_2:
                        exp_rid, exp_ports = exp_data
                        rx_port_list.append(exp_ports)
                        exp_pkt = simple_tcp_packet(eth_dst='EE:EE:EE:EE:EE:EE',
                                                    eth_src='AA:BB:CC:DD:EE:FF',
                                                    ip_src='1.1.1.1',
                                                    ip_dst=ip_addr_str_list[ ip_addr_list.index(ip) ],
                                                    ip_ttl=63,
                                                    ip_id=exp_rid)
                        rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))
                        #print("RID:", hex(exp_rid), "Ports:", exp_ports)

                    print("Sending", ip_addr_str_list[ ip_addr_list.index(ip) ], "on port", port, "expecting receive on ports:", rx_port_list)
                    print()
                    sys.stdout.flush()
                    send_packet(self, port, pkt)
                    verify_packet_list(self, rx_port_list, rx_pkt_list)


        finally:
            print("Cleaning up")
            sys.stdout.flush()
            for entry_hdl in ing_port_entry_hdls:
                self.client.ing_port_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ing_src_ifid_entry_hdls:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in mcast_dmac_entry_hdls:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ip_addr_entry_hdls:
                self.client.ing_ipv4_mcast_table_delete(sess_hdl, dev_id, entry_hdl)
            for key in mc_trees:
                mc_trees[key].cleanUp()
            for grp in ecmp_grps:
                grp.cleanUp()
            t.cleanUp()

            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestLag(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        global t
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]

        port_list = list(swports)

        hw_down_ports = random.sample(port_list, len(port_list)//2)
        if not support_hw_port_flap:
            hw_down_ports = []
        print("HW down ports:", sorted(hw_down_ports))
        sw_down_ports = random.sample(port_list, len(port_list)//2)
        print("SW down ports:", sorted(sw_down_ports))

        t.setup(self, sess_hdl, mc_sess_hdl)
        t.disable_port_ff()

        port_hdl = None
        ifid_hdl = None
        dmac_hdl = None
        ipv4_hdl = None
        mct = None

        num_lags_to_test = 64
        lags_per_pass = 8
        use_remote_counts = True
        use_port_pruning = True
        use_random_lags = True

        try:
            if num_pipes < 4:
                try:
                    t.get_lag_tbl().getLag(5).addMbr([392])
                    print("Expected LAG modify to fail due to invalid port")
                    sys.stdout.flush()
                    self.assertTrue(False)
                except InvalidMcOperation as e:
                    pass
            all_lags = list(range(255))
            test_lags = all_lags[:num_lags_to_test]
            if use_random_lags:
                test_lags = random.sample(all_lags, num_lags_to_test)
            while len(test_lags):
                if len(test_lags) > lags_per_pass:
                    lag_ids = test_lags[:lags_per_pass]
                    test_lags = test_lags[lags_per_pass:]
                else:
                    lag_ids = list(test_lags)
                    test_lags = []
                print("Checking LAG IDs:", lag_ids)
                sys.stdout.flush()

                ing_port = random.choice( port_list )
                ifid = 3
                brid = 0x1234
                vrf = 0x4321
                mgid1 = 0
                mgid2 = random.randint(1, 0xFFFF)
                global_rid = 0
                irid = random.randint(1, 0xFFFF)
                xid = random.randint(0, 0xFFFF)
                yid = random.randint(0, 287)
                hash1 = random.randint(0, 0x1FFF)
                hash2 = random.randint(0, 0x1FFF)
                dmac = "00:11:22:33:44:55"

                port_hdl = add_port_entry(self, sess_hdl, dev_tgt, ing_port, 0, 0, ifid)
                ifid_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, irid, yid, brid, hash1, hash2)
                dmac_hdl = add_dmac_entry_to_route(self, sess_hdl, dev_tgt, brid, dmac, vrf)
                ipv4_hdl = add_ipv4_mcast_entry(self, sess_hdl, dev_tgt, 0, vrf, 0, 0, 0, 0, xid, mgid1, mgid2)
                self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

                print("Using MGID1=%d MGID2=%d globalRID=0x%x iRID=0x%x YID=%d Hash2=0x%x" % (mgid1, mgid2, global_rid, irid, yid, hash2))
                print("Port Flag: %s" % support_hw_port_flap)
                print("Use Remote Counts: %s" % use_remote_counts)
                print("Use Port Pruning: %s" % use_port_pruning)

                # Set pruning mask
                if use_port_pruning:
                    if len(port_list) <= 4:
                        cnt = 1
                    else:
                        cnt = len(port_list)//4
                    pdp = random.sample(port_list, cnt)
                    t.get_yid_tbl().set_pruned_ports(yid, pdp)
                    t.get_yid_tbl().set_global_rid(global_rid)
                else:
                    t.get_yid_tbl().set_pruned_ports(yid, [])
                    t.get_yid_tbl().set_global_rid(global_rid)

                # Build the multicast trees
                mct = MCTree(self, mc_sess_hdl, dev_id, mgid2)
                erid_space = [global_rid, irid] + random.sample(list(range(0x10000)), 8)
                erid = erid_space[ random.randint(0, len(erid_space)-1) ]
                mct.add_node(erid, None, [], lag_ids )

                # Add ports to the LAGs
                for i in lag_ids:
                    count = random.randint(0, len(port_list))
                    mbrs = random.sample(port_list, count)
                    t.get_lag_tbl().getLag(i).addMbr(mbrs)

                # Set left and right counts on 1 out of 4 lags
                if use_remote_counts:
                    for i in lag_ids:
                        if random.randint(1,4) == 4:
                            left = random.randint(0, 0x1FFF)
                            right = random.randint(0, 0x1FFF)
                            lag = t.get_lag_tbl().getLag(i)
                            lag.setRmtCnt(left, right)

                # Make sure all ports start as up.
                for p in port_list:
                    t.sw_port_up(p)
                    t.clr_hw_port_down(p)
                t.disable_port_ff()
                t.enable_port_ff()

                self.conn_mgr.complete_operations(sess_hdl)
                self.mc.mc_complete_operations(mc_sess_hdl)

                for step in range(1,6):
                    print("  Step", step)
                    sys.stdout.flush()
                    if 1 == step:
                        pass

                    elif 2 == step:
                        # Mark some ports as SW down and send again
                        for p in sw_down_ports:
                            t.sw_port_down(p)

                    elif 3 == step:
                        # Mark all ports as SW up, mark some ports HW down.
                        for p in sw_down_ports:
                            t.sw_port_up(p)
                        for p in hw_down_ports:
                            t.set_port_down(p)
                        time.sleep(3)
                        for p in hw_down_ports:
                            t.set_port_up(p)
                        time.sleep(3)

                    elif 4 == step:
                        # Clear HW down state
                        for p in hw_down_ports:
                            t.clr_hw_port_down(p)

                    elif 5 == step:
                        # Mark some ports as SW down, mark some ports as HW down.
                        # Bring ports back up.
                        for p in sw_down_ports:
                            t.sw_port_down(p)
                        for p in hw_down_ports:
                            t.set_port_down(p)
                        time.sleep(3)
                        for p in hw_down_ports:
                            t.set_port_up(p)
                        time.sleep(3)

                    self.conn_mgr.complete_operations(sess_hdl)
                    self.mc.mc_complete_operations(mc_sess_hdl)

                    exp_data_list = mct.get_ports(irid, xid, yid, hash1, hash2)
                    rx_port_list = []
                    rx_rid_list = []
                    rx_pkt_list = []
                    pkt = simple_tcp_packet(eth_dst=dmac,
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst='10.9.8.7',
                                            ip_ttl=64,
                                            ip_id=0xABCD)

                    for exp_rid, exp_ports in exp_data_list:
                        if not exp_ports:
                            continue
                        rx_port_list.append(exp_ports)
                        rx_rid_list.append(exp_rid)
                        exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                                    eth_src='AA:BB:CC:DD:EE:FF',
                                                    ip_src='1.1.1.1',
                                                    ip_dst='10.9.8.7',
                                                    ip_ttl=63,
                                                    ip_id=exp_rid)
                        rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))

                    print("  Sending on port", ing_port, "expecting receive on ports:")
                    for x, y in zip(rx_rid_list, rx_port_list):
                        print("    RID", hex(x), "Ports", sorted(y))
                    print()
                    sys.stdout.flush()
                    send_packet(self, ing_port, pkt)

                    verify_packet_list(self, rx_port_list, rx_pkt_list)

                # Clean up so we can add the next set of entries.
                t.get_yid_tbl().set_pruned_ports(yid, [])
                if port_hdl is not None:
                    self.client.ing_port_table_delete(sess_hdl, dev_id, port_hdl)
                    port_hdl = None
                if ifid_hdl is not None:
                    self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, ifid_hdl)
                    ifid_hdl = None
                if dmac_hdl is not None:
                    self.client.ing_dmac_table_delete(sess_hdl, dev_id, dmac_hdl)
                    dmac_hdl = None
                if ipv4_hdl is not None:
                    self.client.ing_ipv4_mcast_table_delete(sess_hdl, dev_id, ipv4_hdl)
                    ipv4_hdl = None
                if mct is not None:
                    mct.cleanUp()
                    mct = None

        finally:
            print("Cleaning up")
            sys.stdout.flush()
            if port_hdl is not None:
                self.client.ing_port_table_delete(sess_hdl, dev_id, port_hdl)
                port_hdl = None
            if ifid_hdl is not None:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, ifid_hdl)
                ifid_hdl = None
            if dmac_hdl is not None:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, dmac_hdl)
                dmac_hdl = None
            if ipv4_hdl is not None:
                self.client.ing_ipv4_mcast_table_delete(sess_hdl, dev_id, ipv4_hdl)
                ipv4_hdl = None
            if mct is not None:
                mct.cleanUp()
                mct = None

            for p in port_list:
                t.sw_port_up(p)
            for p in hw_down_ports:
                t.set_port_up(p)
            time.sleep(3)
            for p in hw_down_ports:
                t.clr_hw_port_down(p)

            t.disable_port_ff()
            t.cleanUp()

            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)

class TestEcmpDynamicTree(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])

    def runTest(self):
        global t
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]
        num_ports = len(pipe_list) * 72
        t.setup(self, sess_hdl, mc_sess_hdl)

        # Legal YID values
        yid_space = []
        for yid in range(0, 288):
            yid_space.append(yid)
        # Shuffle them so we can get a random set just by taking a few from
        # the front of the list.
        random.shuffle(yid_space)

        # Legal port values
        port_space = []
        for bit_number in range(0,num_ports):
            port = BitIdxToPort(bit_number)
            port_space.append(port)

        # Ports this test uses.
        port_list = swports

        brid_list = [0x9347]

        ip_addr_str_list = ["224.100.0.0",
                            "224.100.0.1",
                            "224.100.0.2",
                            "224.100.0.3"]
        ip_addr_list = []
        for ip in ip_addr_str_list:
            ip_addr_list.append(ipv4Addr_to_i32(ip))
        ip_to_mgid1 = {}
        ip_to_mgid2 = {}
        ip_to_xid = {}
        xid_list = random.sample(list(range(0x10000)), len(ip_addr_list))
        mgid_sample_space = random.sample(list(range(1, 0x10000)), 2*len(ip_addr_list))
        for ip in ip_addr_list:
            ip_to_mgid1[ip] = mgid_sample_space.pop(0)
            ip_to_mgid2[ip] = mgid_sample_space.pop(0)
            ip_to_xid[ip]   = xid_list.pop(0)
            mgid_sample_space.append(ip_to_mgid1[ip])
            mgid_sample_space.append(ip_to_mgid2[ip])
            xid_list.append(ip_to_xid[ip])

        ifid_list = []
        ifid = 0
        port_to_ifid = {}
        for port in port_list:
            port_to_ifid[port] = ifid
            ifid_list.append(ifid)
            ifid = ifid + 1

        ifid_to_yid = {}
        ifid_to_brid = {}

        mc_trees = {}
        ecmp_grps = []
        ing_port_entry_hdls = []
        ing_src_ifid_entry_hdls = []
        mcast_dmac_entry_hdls = []
        ip_addr_entry_hdls = []


        try:
            # Add Ing Port table entries
            for port in port_list:
                entry_hdl = add_port_entry(self, sess_hdl, dev_tgt, port, 0, 0, port_to_ifid[port])
                ing_port_entry_hdls.append(entry_hdl)

            # Add Ing Src IFID table entries
            i = 0
            for ifid in ifid_list:
                ifid_to_yid[ifid] = yid_space[i]
                ifid_to_brid[ifid] = brid_list[0]
                i = i + 1
                entry_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, 0x4321,
                                              ifid_to_yid[ifid], ifid_to_brid[ifid], 0x1FFF, 0x0EEE)
                ing_src_ifid_entry_hdls.append(entry_hdl)


            # Add DMAC entries to route
            for brid  in brid_list:
                match_spec = multicast_test_ing_dmac_match_spec_t(
                                    hex_to_i16(brid),
                                    macAddr_to_string("EE:EE:EE:EE:EE:EE"))
                # Let brid equal vrf
                action_spec = multicast_test_route_action_spec_t(hex_to_i16(brid))
                entry_hdl = self.client.ing_dmac_table_add_with_route(
                                    sess_hdl,
                                    dev_tgt,
                                    match_spec,
                                    action_spec)
                mcast_dmac_entry_hdls.append(entry_hdl)

            # Add IP entries
            for ip in ip_addr_list:
                src_ip = 0
                src_ip_mask = 0
                dst_ip_mask = ipv4Addr_to_i32("255.255.255.255")
                mgid1 = ip_to_mgid1[ip]
                mgid2 = ip_to_mgid2[ip]
                xid = ip_to_xid[ip]
                match_spec = multicast_test_ing_ipv4_mcast_match_spec_t(
                        hex_to_i16(brid_list[0]), src_ip, src_ip_mask, ip, dst_ip_mask)
                #action_spec = multicast_test_mcast_route_action_spec_t(
                #        hex_to_i16(xid), hex_to_i16(mgid1), hex_to_i16(mgid2))
                action_spec = multicast_test_mcast_route_action_spec_t(
                        hex_to_i16(xid), hex_to_i16(mgid1), hex_to_i16(0))
                entry_hdl = self.client.ing_ipv4_mcast_table_add_with_mcast_route(sess_hdl, dev_tgt, match_spec, 0, action_spec)
                ip_addr_entry_hdls.append(entry_hdl)


            # Setup three LAGs for the ECMP groups to use.
            # one is reserved for later use
            lag_ids = random.sample(list(range(255)), 3)
            for idx, i in enumerate(lag_ids):
                mbrs = [port_list[idx]]
                t.get_lag_tbl().getLag(i).addMbr(mbrs)

            # Build the ECMP groups. The group has 2 lag memebers
            for g_idx in range(1):
                g_idx = 1
                ecmp_grp = EcmpGrp(self, mc_sess_hdl, dev_id)
                for i in range(2):
                    node_rid = i | (g_idx << 8)
                    ports = []
                    lags = [lag_ids[i]]
                    ecmp_grp.addMbr(node_rid, ports, lags)
                mbrs = ecmp_grp.getMbrs()
                off = 0
                for i in range(0, len(mbrs)):
                    while(ecmp_grp.mbrs[i + off] is None):
                        off += 1
                    self.assertEqual(mbrs[i], ecmp_grp.mbrs[i + off].l1_hdl)
                ecmp_grps.append(ecmp_grp)

            test_count = self.mc.mc_ecmp_get_count(mc_sess_hdl, dev_id)
            test_grp = []
            print(test_count)
            if test_count > 0:
                test_grp.append(self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id))
                if test_count > 1:
                    test_grp += self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, test_grp[0], test_count - 1)
            for i in range(0, len(ecmp_grps)):
                self.assertTrue(ecmp_grps[i].hdl in test_grp)

            # Build the multicast trees, two ecmp grp for each MGID
            for mgid in mgid_sample_space:
                mct = MCTree(self, mc_sess_hdl, dev_id, mgid)
                for g in ecmp_grps:
                    xid = None
                    mct.add_ecmp(g, xid)
                mc_trees[mgid] = mct

            # Need a dummy entry in the encode table.
            self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

            # Program the YID table.
            # The YID used by the first ifid will prune all ports
            # The YID used by the second table will prune no ports
            for i in range(0, len(ifid_list)):
                yid = yid_space[i]
                pruned_ports = []
                if 0 == i:
                    t.get_yid_tbl().set_pruned_ports(yid, port_list)
                elif 1 == i:
                    t.get_yid_tbl().set_pruned_ports(yid, [])
                else:
                    port_cnt = random.randint(0,len(port_list))
                    pruned_ports = random.sample(port_list, port_cnt)
                    t.get_yid_tbl().set_pruned_ports(yid, pruned_ports)

            # Wait for all pipe/mc APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)
            self.mc.mc_complete_operations(mc_sess_hdl)

            for port in port_list[:1]:
                for ip in ip_addr_list[:1]:
                    rid = 0x4321
                    tree1 = mc_trees[ ip_to_mgid1[ip] ]
                    ifid = port_to_ifid[port]
                    yid  = ifid_to_yid[ifid]
                    xid = ip_to_xid[ip]
                    exp_data_list_1 = tree1.get_ports(rid, xid, yid, 0x1FFF, 0x0EEE)
                    rx_port_list = []
                    rx_pkt_list = []
                    pkt = simple_tcp_packet(eth_dst='EE:EE:EE:EE:EE:EE', eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1', ip_dst=ip_addr_str_list[ ip_addr_list.index(ip) ],
                                            ip_ttl=64, ip_id=0xABCD)
                    tree1.print_tree()
                    for exp_data in exp_data_list_1:
                        exp_rid, exp_ports = exp_data
                        rx_port_list.append(exp_ports)
                        exp_pkt = simple_tcp_packet(eth_dst='EE:EE:EE:EE:EE:EE', eth_src='AA:BB:CC:DD:EE:FF',
                                                    ip_src='1.1.1.1', ip_dst=ip_addr_str_list[ ip_addr_list.index(ip) ],
                                                    ip_ttl=63, ip_id=exp_rid)
                        rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))
                    print("Sending", ip_addr_str_list[ ip_addr_list.index(ip) ], "on port", port, "expecting receive on ports:", rx_port_list)
                    print()
                    sys.stdout.flush()
                    send_packet(self, port, pkt)
                    verify_packet_list(self, rx_port_list, rx_pkt_list)

                    # now update the ecmp group with a different node
                    ecmp_grp = tree1.ecmps[0][0]
                    # will update the first node
                    l1_node = ecmp_grp.mbrs[1]
                    l1_node.updateMbrs(self,[], [lag_ids[2]])
                    tree1.print_tree()

                    # Wait for all pipe/mc APIs to complete.
                    self.conn_mgr.complete_operations(sess_hdl)
                    self.mc.mc_complete_operations(mc_sess_hdl)

                    exp_data_list_1 = tree1.get_ports(rid, xid, yid, 0x1FFF, 0x0EEE)
                    rx_port_list = []
                    rx_pkt_list = []
                    pkt = simple_tcp_packet(eth_dst='EE:EE:EE:EE:EE:EE', eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1', ip_dst=ip_addr_str_list[ ip_addr_list.index(ip) ],
                                            ip_ttl=64, ip_id=0xABCD)
                    for exp_data in exp_data_list_1:
                        exp_rid, exp_ports = exp_data
                        rx_port_list.append(exp_ports)
                        exp_pkt = simple_tcp_packet(eth_dst='EE:EE:EE:EE:EE:EE', eth_src='AA:BB:CC:DD:EE:FF',
                                                    ip_src='1.1.1.1', ip_dst=ip_addr_str_list[ ip_addr_list.index(ip) ],
                                                    ip_ttl=63, ip_id=exp_rid)
                        rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))
                    print("Sending", ip_addr_str_list[ ip_addr_list.index(ip) ], "on port", port, "expecting receive on ports:", rx_port_list)
                    print()
                    sys.stdout.flush()
                    send_packet(self, port, pkt)
                    verify_packet_list(self, rx_port_list, rx_pkt_list)

        finally:
            print("Cleaning up")
            sys.stdout.flush()
            for entry_hdl in ing_port_entry_hdls:
                self.client.ing_port_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ing_src_ifid_entry_hdls:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in mcast_dmac_entry_hdls:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ip_addr_entry_hdls:
                self.client.ing_ipv4_mcast_table_delete(sess_hdl, dev_id, entry_hdl)
            for key in mc_trees:
                mc_trees[key].cleanUp()
            for grp in ecmp_grps:
                grp.cleanUp()
            t.cleanUp()
            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestBackup(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        global t
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]

        port_list = swports

        protected_ports = random.sample(port_list, 2)
        backup_ports  = random.sample([x for x in port_list if x not in protected_ports], 2)
        not_protected_ports = [x for x in port_list if x not in protected_ports]
        pdp = random.sample(port_list, len(swports)//2)
        if test_param_get("target") == "hw":
            # Keep the backup ports in the same pipe as the protect ports
            protected_ports = []
            backup_ports  = []
            not_protected_ports = []
            ports_by_pipe = {}

            for pipe in range(num_pipes):
                ports_by_pipe[pipe] = []
            for port in swports:
                pipe = port_to_pipe(port)
                ports_by_pipe[pipe].append(port)
            for pipe in ports_by_pipe:
                if len(ports_by_pipe[pipe]) >= 2:
                    x = random.sample(ports_by_pipe[pipe], 2)
                    protected_ports.append(x[0])
                    backup_ports.append(x[1])
            not_protected_ports = [x for x in port_list if x not in protected_ports]

        print("Protected Ports", protected_ports)
        print("Backup Ports", backup_ports)
        print("Pruned Ports", pdp)
        sys.stdout.flush()

        t.setup(self, sess_hdl, mc_sess_hdl)

        ifid = 0
        brid = 0x1234
        vrf = 0x4321
        mgid1 = random.randint(1, 0xFFFF)
        global_rid = 1
        irid = global_rid
        xid = random.randint(0, 0xFFFF)
        yid = random.randint(0, 287)
        hash1 = random.randint(0, 0x1FFF)
        hash2 = random.randint(0, 0x1FFF)

        print("MGID1:", hex(mgid1))
        print("XID:", hex(xid))
        print("YID:", yid)
        print("Hash:", hex(hash1), hex(hash2))

        ing_port_entry_hdls = []
        # Port Num, Vlan Valid, Vlan ID, IFId
        ing_port_entries = []
        for port in port_list:
            ing_port_entries.append( (port, 0, 0x000,  ifid) )

        ing_src_ifid_entry_hdls = []
        # IFId, RID, YID, BRID, Hash1, Hash2
        ing_src_ifid_entries = [(ifid, irid, yid, brid, hash1, hash2)]

        # DMAC entries
        mcast_dmac_entry_hdls = []
        dmac = "00:00:00:11:22:33"

        # IPv4 Mcast entries
        ip_addr_entry_hdls = []

        # Multicast Trees
        mct1 = None

        try:
            # Add Ing Port table entries
            for e_port_num, e_vlan_v, e_vlan, e_ifid in ing_port_entries:
                entry_hdl = add_port_entry(self, sess_hdl, dev_tgt, e_port_num, e_vlan_v, e_vlan, e_ifid)
                ing_port_entry_hdls.append(entry_hdl)

            # Add Ing Src IFID table entries
            for e_ifid, e_rid, e_yid, e_brid, e_hash1, e_hash2 in ing_src_ifid_entries:
                entry_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, e_ifid, e_rid, e_yid, e_brid, e_hash1, e_hash2)
                ing_src_ifid_entry_hdls.append(entry_hdl)

            # Add DMAC entries to route
            entry_hdl = add_dmac_entry_to_route(self, sess_hdl, dev_tgt, brid, dmac, vrf)
            mcast_dmac_entry_hdls.append(entry_hdl)

            # Add IPv4 Mcast entries
            entry_hdl = add_ipv4_mcast_entry(self, sess_hdl, dev_tgt, 0, vrf, 0, 0, 0, 0, xid, mgid1, 0)
            ip_addr_entry_hdls.append(entry_hdl)

            # Need a dummy entry in the encode table.
            self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

            # Wait for all pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)

            # Set pruning mask
            t.get_yid_tbl().set_pruned_ports(yid, pdp)
            t.get_yid_tbl().set_global_rid(global_rid)

            # Build the multicast trees
            mct1 = MCTree(self, mc_sess_hdl, dev_id, mgid1)
            erid_space = [global_rid, irid] + random.sample(list(range(0x10000)), 8)
            erid = erid_space[ random.randint(0, 9) ]
            mct1.add_node(erid, None, protected_ports, [])

            # Make sure all ports start as up.
            for p in port_list:
                t.clr_hw_port_down(p)
                t.sw_port_up(p)
            t.disable_port_ff()
            t.disable_backup_ports()
            if support_hw_port_flap:
                t.enable_port_ff()
            t.enable_backup_ports()

            # Wait for multicast APIs to complete.
            self.mc.mc_complete_operations(mc_sess_hdl)


            print("Sending without backup ports")
            sys.stdout.flush()
            pkt = simple_tcp_packet(eth_dst=dmac,
                                    eth_src='AA:BB:CC:DD:EE:FF',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.9.8.7',
                                    ip_ttl=64,
                                    ip_id=0xABCD)

            exp_data_list_1 = []
            rx_port_list = []
            rx_pkt_list = []
            for port in port_list:
                exp_data_list_1.extend( mct1.get_ports(irid, xid, yid, hash1, hash2) )
            for exp_data in exp_data_list_1:
                exp_rid, exp_ports = exp_data
                if not exp_ports:
                    continue
                rx_port_list.append(exp_ports)
                exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst='10.9.8.7',
                                            ip_ttl=63,
                                            ip_id=exp_rid)
                rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))

            print("Sending packet to ports:", port_list)
            sys.stdout.flush()
            for port in port_list:
                send_packet(self, port, pkt)
            print("Expecting Rx on:", rx_port_list)
            sys.stdout.flush()
            verify_packet_list(self, rx_port_list, rx_pkt_list)


            # Configure both backup ports and send traffic again.
            print("Sending with backup ports")
            sys.stdout.flush()
            for pport, bport in zip(protected_ports, backup_ports):
                t.set_backup_port(pport, bport)
            self.mc.mc_complete_operations(mc_sess_hdl)
            exp_data_list_1 = []
            rx_port_list = []
            rx_pkt_list = []
            for port in port_list:
                exp_data_list_1.extend( mct1.get_ports(irid, xid, yid, hash1, hash2) )
            for exp_data in exp_data_list_1:
                exp_rid, exp_ports = exp_data
                if not exp_ports:
                    continue
                rx_port_list.append(exp_ports)
                exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst='10.9.8.7',
                                            ip_ttl=63,
                                            ip_id=exp_rid)
                rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))
            print("Sending packet to ports:", port_list)
            sys.stdout.flush()
            for port in port_list:
                send_packet(self, port, pkt)
            print("Expecting Rx on:", rx_port_list)
            sys.stdout.flush()
            verify_packet_list(self, rx_port_list, rx_pkt_list)



            if support_hw_port_flap:
                # Bring down the primary ports and send again.
                print("Primary ports down, sending with backup ports")
                sys.stdout.flush()
                print("Disabling protected ports",protected_ports)
                for port in protected_ports:
                    t.set_port_down(port)
                time.sleep(3)
                #raw_input("Check tree programming now")
                exp_data_list_1 = []
                rx_port_list = []
                rx_pkt_list = []
                print("Adding not protected ports", not_protected_ports)
                for port in not_protected_ports:
                    exp_data_list_1.extend( mct1.get_ports(irid, xid, yid, hash1, hash2) )
                for exp_data in exp_data_list_1:
                    exp_rid, exp_ports = exp_data
                    if not exp_ports:
                        continue
                    rx_port_list.append(exp_ports)
                    exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                                eth_src='AA:BB:CC:DD:EE:FF',
                                                ip_src='1.1.1.1',
                                                ip_dst='10.9.8.7',
                                                ip_ttl=63,
                                                ip_id=exp_rid)
                    rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))
                print("Sending on not protected ports",not_protected_ports)
                for port in not_protected_ports:
                    send_packet(self, port, pkt)
                verify_packet_list(self, rx_port_list, rx_pkt_list)


                # Reprogram the tree and send traffic again.
                print("Primary ports down, sending with backup ports after reprogramming")
                sys.stdout.flush()
                mct1.reprogram()
                self.mc.mc_complete_operations(mc_sess_hdl)
                #raw_input("Check tree programming now")
                exp_data_list_1 = []
                rx_port_list = []
                rx_pkt_list = []
                for port in not_protected_ports:
                    exp_data_list_1.extend( mct1.get_ports(irid, xid, yid, hash1, hash2) )
                for exp_data in exp_data_list_1:
                    exp_rid, exp_ports = exp_data
                    if not exp_ports:
                        continue
                    rx_port_list.append(exp_ports)
                    exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                                eth_src='AA:BB:CC:DD:EE:FF',
                                                ip_src='1.1.1.1',
                                                ip_dst='10.9.8.7',
                                                ip_ttl=63,
                                                ip_id=exp_rid)
                    rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))
                for port in not_protected_ports:
                    send_packet(self, port, pkt)
                verify_packet_list(self, rx_port_list, rx_pkt_list)
            else:
                print("SKIPPING HW PORT FLAP")
                sys.stdout.flush()

            # Bring the protected ports back up, but mark them as SW down
            print("Primary SW ports down, sending with backup ports")
            sys.stdout.flush()
            for port in protected_ports:
                if support_hw_port_flap:
                    t.set_port_up(port)
                t.clr_hw_port_down(port)
                t.sw_port_down(port)
            time.sleep(3)
            self.mc.mc_complete_operations(mc_sess_hdl)
            exp_data_list_1 = []
            rx_port_list = []
            rx_pkt_list = []
            for port in not_protected_ports:
                exp_data_list_1.extend( mct1.get_ports(irid, xid, yid, hash1, hash2) )
            for exp_data in exp_data_list_1:
                exp_rid, exp_ports = exp_data
                if not exp_ports:
                    continue
                rx_port_list.append(exp_ports)
                exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst='10.9.8.7',
                                            ip_ttl=63,
                                            ip_id=exp_rid)
                rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))
            print("Sending packet to ports:", not_protected_ports)
            sys.stdout.flush()
            for port in not_protected_ports:
                send_packet(self, port, pkt)
            print("Expecting Rx on:", rx_port_list)
            sys.stdout.flush()
            verify_packet_list(self, rx_port_list, rx_pkt_list)


        finally:
            print("Cleaning up")
            sys.stdout.flush()
            for entry_hdl in ing_port_entry_hdls:
                self.client.ing_port_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ing_src_ifid_entry_hdls:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in mcast_dmac_entry_hdls:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ip_addr_entry_hdls:
                self.client.ing_ipv4_mcast_table_delete(sess_hdl, dev_id, entry_hdl)
            if mct1 is not None:
                mct1.cleanUp()
            for p in port_list:
                if support_hw_port_flap:
                    t.set_port_up(p)
                t.clr_backup_port(p)
                t.sw_port_up(p)
                t.clr_hw_port_down(p)
            time.sleep(3)

            t.disable_backup_ports()
            t.cleanUp()

            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)



class TestGetEntry(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        if test_param_get("target") == "bmv2":
            return
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        ing_port_entries = {}
        ing_src_ifid_entries = {}
        mcast_dmac_entries = {}
        ip_addr_entries = {}
        try:
            # Test the ing_port table
            num_ing_port_entries = 1000
            keys = random.sample(range(0x400000), num_ing_port_entries)
            for key in keys:
                port = key & 0x1FF
                vlan_v = (key >> 9) & 1
                vlan = key >> 10
                m_spec = multicast_test_ing_port_match_spec_t(port, vlan_v, vlan)
                val = random.randint(0,0xFFFFFFFF)
                a_spec = multicast_test_set_ifid_action_spec_t(hex_to_i32(val))
                hdl = self.client.ing_port_table_add_with_set_ifid(sess_hdl, dev_tgt, m_spec, a_spec)
                ing_port_entries[hdl] = (m_spec, a_spec)
            for hdl in ing_port_entries:
                x = self.client.ing_port_get_entry(sess_hdl, dev_id, hdl, False)
                m_spec,a_spec = ing_port_entries[hdl]
                self.assertEqual(x.match_spec, m_spec)
                self.assertEqual(x.action_desc.name, "set_ifid")
                self.assertEqual(x.action_desc.data.multicast_test_set_ifid, a_spec)

            # Test the ing_src_ifid table
            num_ing_src_ifid_entries = 1000
            keys = [random.randint(0,0xFFFFFFFF) for _ in range(num_ing_src_ifid_entries)]
            for key in keys:
                m_spec = multicast_test_ing_src_ifid_match_spec_t(hex_to_i32(key))
                rid = hex_to_i16( random.randint(0,0xFFFF) )
                yid = hex_to_i16( random.randint(0,0x1FF) )
                brid = hex_to_i16( random.randint(0,0xFFFF) )
                h1 = hex_to_i16( random.randint(0,0x1FFF) )
                h2 = hex_to_i16( random.randint(0,0x1FFF) )
                a_spec = multicast_test_set_src_ifid_md_action_spec_t(rid, yid, brid, h1, h2)
                hdl = self.client.ing_src_ifid_table_add_with_set_src_ifid_md(sess_hdl, dev_tgt, m_spec, a_spec)
                ing_src_ifid_entries[hdl] = (m_spec, a_spec)
            for hdl in ing_src_ifid_entries:
                x = self.client.ing_src_ifid_get_entry(sess_hdl, dev_id, hdl, False)
                m_spec,a_spec = ing_src_ifid_entries[hdl]
                self.assertEqual(x.match_spec, m_spec)
                self.assertEqual(x.action_desc.name, "set_src_ifid_md")
                self.assertEqual(x.action_desc.data.multicast_test_set_src_ifid_md, a_spec)

            # Test the ing_dmac table
            num_mcast_dmac_entries = 1000
            keys = random.sample(range(0x10000), num_mcast_dmac_entries)
            for key in keys:
                dmac = '%02x:%02x:%02x:%02x:%02x:%02x' % ( random.randint(0,255), random.randint(0,255), random.randint(0,255), random.randint(0,255), random.randint(0,255), random.randint(0,255) )
                m_spec = multicast_test_ing_dmac_match_spec_t(hex_to_i16(key), macAddr_to_string(dmac))
                act_name = random.choice(['flood', 'switch', 'route'])
                if act_name == 'flood':
                    a_spec = None
                    hdl = self.client.ing_dmac_table_add_with_flood(sess_hdl, dev_tgt, m_spec)
                elif act_name == 'switch':
                    egr_port = random.randint(0, 0x1FF)
                    a_spec = multicast_test_switch_action_spec_t( hex_to_i16(egr_port) )
                    hdl = self.client.ing_dmac_table_add_with_switch(sess_hdl, dev_tgt, m_spec, a_spec)
                else:
                    vrf = random.randint(0, 0xFFFF)
                    a_spec = multicast_test_route_action_spec_t( hex_to_i16(vrf) )
                    hdl = self.client.ing_dmac_table_add_with_route(sess_hdl, dev_tgt, m_spec, a_spec)
                mcast_dmac_entries[hdl] = (m_spec, a_spec)
            for hdl in mcast_dmac_entries:
                x = self.client.ing_dmac_get_entry(sess_hdl, dev_id, hdl, False)
                m_spec,a_spec = mcast_dmac_entries[hdl]
                self.assertEqual(x.match_spec, m_spec)
                self.assertIn(x.action_desc.name, ['flood', 'switch', 'route'])
                if x.action_desc.name == 'flood':
                    pass
                elif x.action_desc.name == 'switch':
                    self.assertEqual(x.action_desc.data.multicast_test_switch, a_spec)
                elif x.action_desc.name == 'route':
                    self.assertEqual(x.action_desc.data.multicast_test_route, a_spec)

            # Test the ing_ipv4_mcast table
            num_ip_addr_entries = 510
            keys = random.sample(range(0x10000), num_ip_addr_entries)
            for key in keys:
                src_mask = random.randint(0,0xFFFFFFFF)
                src = src_mask & random.randint(0,0xFFFFFFFF)
                dst_mask = random.randint(0,0xFFFFFFFF)
                dst = dst_mask & random.randint(0,0xFFFFFFFF)
                m_spec = multicast_test_ing_ipv4_mcast_match_spec_t(hex_to_i16(key), hex_to_i32(src), hex_to_i32(src_mask), hex_to_i32(dst), hex_to_i32(dst_mask))
                xid = hex_to_i16( random.randint(0,0xFFFF) )
                mgid1 = hex_to_i16( random.randint(0,0xFFFF) )
                mgid2 = hex_to_i16( random.randint(0,0xFFFF) )
                a_spec = multicast_test_mcast_route_action_spec_t(xid, mgid1, mgid2)
                pri = hex_to_i32( random.randint(0, 0xFFFFFFFF) )
                hdl = self.client.ing_ipv4_mcast_table_add_with_mcast_route(sess_hdl, dev_tgt, m_spec, pri, a_spec)
                ip_addr_entries[hdl] = (m_spec, a_spec, pri)
            for hdl in ip_addr_entries:
                x = self.client.ing_ipv4_mcast_get_entry(sess_hdl, dev_id, hdl, False)
                m_spec,a_spec,pri = ip_addr_entries[hdl]
                self.assertEqual(x.match_spec, m_spec)
                self.assertEqual(x.priority, pri)
                self.assertEqual(x.action_desc.name, "mcast_route")
                self.assertEqual(x.action_desc.data.multicast_test_mcast_route, a_spec)


        finally:
            print("Cleaning up")
            sys.stdout.flush()
            for entry_hdl in ing_port_entries:
                self.client.ing_port_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ing_src_ifid_entries:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in mcast_dmac_entries:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ip_addr_entries:
                self.client.ing_ipv4_mcast_table_delete(sess_hdl, dev_id, entry_hdl)

            self.conn_mgr.client_cleanup(sess_hdl)



def hex_to_i64(h):
    x = int(h)
    if (x > 0x7FFFFFFFFFFFFFFF): x-= 0x10000000000000000
    return x
def i64_to_hex(h):
    x = int(h)
    if (x & 0x8000000000000000): x+= 0x10000000000000000
    return x


class TestRegAccess(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        if test_param_get("target") == "bmv2":
            print("Skipping register test")
            sys.stdout.flush()
            return
        if test_param_get("arch") != "tofino":
            print("Skipping register test for", test_param_get("arch"))
            return
        try:

            # Read device_select tm_top tm_pre_top pre[0] ctrl
            x = self.conn_mgr.reg_rd(dev_id, 0x00600000)

            # Write it and make sure it reads back the same.
            y = (0xff << 24) | (64 << 9)
            self.conn_mgr.reg_wr(dev_id, 0x00600000, hex_to_i32(y))
            z = self.conn_mgr.reg_rd(dev_id, 0x00600000)

            # Reset it to the original value.
            self.conn_mgr.reg_wr(dev_id, 0x00600000, hex_to_i32(x))

            # Verify that the read back value matches the written value.
            z = i32_to_hex(z)
            s = "Wrote 0x%x, read back 0x%x" % (y, z)
            self.assertEqual(y, z, s)

            # Read LAG membership: tm_pre_lit0_bm_mem_word0_address
            addr = 0x6180000000
            x = self.conn_mgr.ind_reg_rd(dev_id, addr)
            self.assertEqual(i64_to_hex(x.hi), 0)
            self.assertEqual(i64_to_hex(x.lo), 0)
            hi = 0xF1
            lo = 0x87654321ABCD0123
            val = indirect_reg_data_t(hex_to_i64(hi), hex_to_i64(lo))
            self.conn_mgr.ind_reg_wr(dev_id, addr, val)
            x = self.conn_mgr.ind_reg_rd(dev_id, addr)
            val = indirect_reg_data_t(hex_to_i64(0), hex_to_i64(0))
            self.conn_mgr.ind_reg_wr(dev_id, addr, val)
            self.assertEqual(i64_to_hex(x.hi), hi)
            self.assertEqual(i64_to_hex(x.lo), lo)


        finally:
            print("Cleaning up")
            sys.stdout.flush()


class TestLongTree(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        global t
        if test_param_get("target") == "bmv2":
            return
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]

        port_list = list(swports)

        t.setup(self, sess_hdl, mc_sess_hdl)
        t.disable_port_ff()

        port_hdl = None
        ifid_hdl = None
        dmac_hdl = None
        mct = None

        try:
            self.mc.mc_set_max_node_threshold( mc_sess_hdl, dev_id, 16384, 300)
            for i in range(0, 255, 32):
                t.get_lag_tbl().getLag(i).addMbr([swports[0]])

            if test_param_get("arch") == "tofino3":
                mgid = 1
            else:
                mgid = 0
            mct = MCTree(self, mc_sess_hdl, dev_id, mgid)

            # Add one node with a short L2 chain (port 0 and lag 0)
            mct.add_node(2, None, [swports[0]], [0])

            # Add one node with a long L2 chain (all lags)
            mct.add_node(1, None, [], list(range(255)))

            ing_port = swports[0]
            ifid = 3
            brid = mgid
            vrf = 0x4321
            global_rid = 0
            irid = 0
            xid = 0
            yid = 0
            hash1 = 0
            hash2 = 0
            dmac = "00:11:22:33:44:55"

            port_hdl = add_port_entry(self, sess_hdl, dev_tgt, ing_port, 0, 0, ifid)
            ifid_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, irid, yid, brid, hash1, hash2)
            dmac_hdl = add_dmac_entry_to_flood(self, sess_hdl, dev_tgt, brid, dmac)
            self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

            self.conn_mgr.complete_operations(sess_hdl)
            self.mc.mc_complete_operations(mc_sess_hdl)

            exp_data_list = mct.get_ports(irid, xid, yid, hash1, hash2)
            rx_port_list = []
            rx_rid_list = []
            rx_pkt_list = []
            pkt = simple_tcp_packet(eth_dst=dmac,
                                    eth_src='AA:BB:CC:DD:EE:FF',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.9.8.7',
                                    ip_ttl=64,
                                    ip_id=0xABCD)

            for exp_rid, exp_ports in exp_data_list:
                if not exp_ports:
                    continue
                rx_port_list.append(exp_ports)
                rx_rid_list.append(exp_rid)
                exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst='10.9.8.7',
                                            ip_ttl=64,
                                            ip_id=exp_rid)
                rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))

            print("  Sending on port", ing_port, "expecting receive on ports:")
            for x, y in zip(rx_rid_list, rx_port_list):
                print("    RID", hex(x), "Ports", sorted(y))
            print()
            sys.stdout.flush()
            send_packet(self, ing_port, pkt)
            time.sleep(3)
            verify_packet_list(self, rx_port_list, rx_pkt_list)

        finally:
            print("Cleaning up")
            sys.stdout.flush()
            if port_hdl is not None:
                self.client.ing_port_table_delete(sess_hdl, dev_id, port_hdl)
                port_hdl = None
            if ifid_hdl is not None:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, ifid_hdl)
                ifid_hdl = None
            if dmac_hdl is not None:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, dmac_hdl)
                dmac_hdl = None
            if mct is not None:
                mct.cleanUp()
                mct = None

            t.cleanUp()

            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestLongTree2(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        global t
        if test_param_get("target") == "bmv2":
            return
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]

        port_list = list(swports)

        t.setup(self, sess_hdl, mc_sess_hdl)
        t.disable_port_ff()

        port_hdl = None
        ifid_hdl = None
        dmac_hdl = None
        mct = None

        try:
            if test_param_get("arch") == "tofino3":
                mgid = 1
            else:
                mgid = 0
            ing_port = swports[0]
            ifid = 3
            brid = mgid
            vrf = 0x4321
            global_rid = 0
            irid = global_rid
            xid = 0
            yid = 0
            hash1 = 0
            hash2 = 0
            dmac = "00:11:22:33:44:55"

            port_hdl = add_port_entry(self, sess_hdl, dev_tgt, ing_port, 0, 0, ifid)
            ifid_hdl = add_ifid_entry(self, sess_hdl, dev_tgt, ifid, irid, yid, brid, hash1, hash2)
            dmac_hdl = add_dmac_entry_to_flood(self, sess_hdl, dev_tgt, brid, dmac)
            self.client.egr_encode_set_default_action_do_egr_encode(sess_hdl, dev_tgt)

            self.mc.mc_set_max_node_threshold( mc_sess_hdl, dev_id, 16384, 300)
            t.get_yid_tbl().set_global_rid(global_rid)

            # Put a port in every lag group
            for i in range(0, 255):
                t.get_lag_tbl().getLag(i).addMbr([swports[0]])

            # Prune that port so we don't get so many copies
            t.get_yid_tbl().set_pruned_ports(yid, [swports[0]])

            # Add nodes to the tree in increasing length
            mct = MCTree(self, mc_sess_hdl, dev_id, mgid)
            mct.add_node(0, None, swports, [])
            for i in range(1, 255):
                port_mbrs = []
                lag_mbrs = list(range(i+1))
                mct.add_node(i, None, port_mbrs, lag_mbrs)

            # Change the last LAG group so it uses another port.
            t.get_lag_tbl().getLag(254).rmvMbr([swports[0]])
            t.get_lag_tbl().getLag(254).addMbr([swports[1]])

            self.conn_mgr.complete_operations(sess_hdl)
            self.mc.mc_complete_operations(mc_sess_hdl)

            exp_data_list = mct.get_ports(irid, xid, yid, hash1, hash2)
            rx_port_list = []
            rx_rid_list = []
            rx_pkt_list = []
            pkt = simple_tcp_packet(eth_dst=dmac,
                                    eth_src='AA:BB:CC:DD:EE:FF',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.9.8.7',
                                    ip_ttl=64,
                                    ip_id=0xABCD)

            for exp_rid, exp_ports in exp_data_list:
                if not exp_ports:
                    continue
                rx_port_list.append(exp_ports)
                rx_rid_list.append(exp_rid)
                exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst='10.9.8.7',
                                            ip_ttl=64,
                                            ip_id=exp_rid)
                rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))

            print("  Sending on port", ing_port, "expecting receive on ports:")
            for x, y in zip(rx_rid_list, rx_port_list):
                print("    RID", hex(x), "Ports", sorted(y))
            print()
            sys.stdout.flush()
            send_packet(self, ing_port, pkt)
            time.sleep(10)
            verify_packet_list(self, rx_port_list, rx_pkt_list)

            # Remove nodes in reverse order
            for _ in range(1,255):
                mct.rmv_last_node()
            # Only the port node is left now

            exp_data_list = mct.get_ports(irid, xid, yid, hash1, hash2)
            rx_port_list = []
            rx_rid_list = []
            rx_pkt_list = []
            pkt = simple_tcp_packet(eth_dst=dmac,
                                    eth_src='AA:BB:CC:DD:EE:FF',
                                    ip_src='1.1.1.1',
                                    ip_dst='10.9.8.7',
                                    ip_ttl=64,
                                    ip_id=0xABCD)

            for exp_rid, exp_ports in exp_data_list:
                if not exp_ports:
                    continue
                rx_port_list.append(exp_ports)
                rx_rid_list.append(exp_rid)
                exp_pkt = simple_tcp_packet(eth_dst=dmac,
                                            eth_src='AA:BB:CC:DD:EE:FF',
                                            ip_src='1.1.1.1',
                                            ip_dst='10.9.8.7',
                                            ip_ttl=64,
                                            ip_id=exp_rid)
                rx_pkt_list.append(build_rx_packet_list(exp_ports, exp_pkt))

            print("  Sending on port", ing_port, "expecting receive on ports:")
            for x, y in zip(rx_rid_list, rx_port_list):
                print("    RID", hex(x), "Ports", sorted(y))
            print()
            sys.stdout.flush()
            send_packet(self, ing_port, pkt)
            time.sleep(3)
            verify_packet_list(self, rx_port_list, rx_pkt_list)

        finally:
            print("Cleaning up")
            sys.stdout.flush()
            if port_hdl is not None:
                self.client.ing_port_table_delete(sess_hdl, dev_id, port_hdl)
                port_hdl = None
            if ifid_hdl is not None:
                self.client.ing_src_ifid_table_delete(sess_hdl, dev_id, ifid_hdl)
                ifid_hdl = None
            if dmac_hdl is not None:
                self.client.ing_dmac_table_delete(sess_hdl, dev_id, dmac_hdl)
                dmac_hdl = None
            if mct is not None:
                mct.cleanUp()
                mct = None

            t.cleanUp()

            self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestGetNode(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        global t
        if test_param_get("target") == "bmv2":
            return
        setup_random()
        mc_sess_hdl = self.mc.mc_create_session()
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]
        nodes = set()

        ports = set_port_map([])
        lags  = set_lag_map([])

        try:
            # Count should be zero
            c = self.mc.mc_node_get_count(mc_sess_hdl, dev_id)
            self.assertEqual(c, 0)
            # No nodes yet so this should fail

            try:
                node_hdl = self.mc.mc_node_get_first(mc_sess_hdl, dev_id)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass
            # Should not be able to get any "next" nodes
            try:
                node_hdls = self.mc.mc_node_get_next_i(mc_sess_hdl, dev_id, 0, 10)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass
            try:
                node_hdls = self.mc.mc_node_get_next_i(mc_sess_hdl, dev_id, 1, 1)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass
            try:
                node_hdls = self.mc.mc_node_get_next_i(mc_sess_hdl, dev_id, 0x12345678, 1000)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass

            # Create a single node
            nodes.add( self.mc.mc_node_create(mc_sess_hdl, dev_id, 0, ports, lags) )
            # Count should be one
            c = self.mc.mc_node_get_count(mc_sess_hdl, dev_id)
            self.assertEqual(c, 1)
            # Get the handle and make sure it is the correct handle
            node_hdl = self.mc.mc_node_get_first(mc_sess_hdl, dev_id)
            self.assertIn(node_hdl, nodes)
            # Should not be able to get anymore nodes
            try:
                node_hdls = self.mc.mc_node_get_next_i(mc_sess_hdl, dev_id, node_hdl, 1234)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass

            # Try with more nodes
            for i in range(1,25):
                nodes.add( self.mc.mc_node_create(mc_sess_hdl, dev_id, i, ports, lags) )

            # Can we get all of them with get-first?
            while len(nodes):
                n = self.mc.mc_node_get_first(mc_sess_hdl, dev_id)
                self.assertIn(n, nodes)
                nodes.remove(n)
                self.mc.mc_node_destroy(mc_sess_hdl, dev_id, n)
            # No more nodes, should fail again
            try:
                n = self.mc.mc_node_get_first(mc_sess_hdl, dev_id)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass

            # Add nodes again and get them all
            for i in range(5000):
                nodes.add( self.mc.mc_node_create(mc_sess_hdl, dev_id, i, ports, lags) )
            c = self.mc.mc_node_get_count(mc_sess_hdl, dev_id)
            self.assertEqual(c, 5000)
            N = [self.mc.mc_node_get_first(mc_sess_hdl, dev_id)]
            N += self.mc.mc_node_get_next_i(mc_sess_hdl, dev_id, N[0], 4999)
            self.assertEqual(nodes, set(N))

            # Get them one at a time
            N = [self.mc.mc_node_get_first(mc_sess_hdl, dev_id)]
            for _ in range(4999):
                N += self.mc.mc_node_get_next_i(mc_sess_hdl, dev_id, N[-1], 1)
            self.assertEqual(nodes, set(N))

            # Get more nodes than are actually present
            N = [self.mc.mc_node_get_first(mc_sess_hdl, dev_id)]
            N += self.mc.mc_node_get_next_i(mc_sess_hdl, dev_id, N[-1], 2000)
            N += self.mc.mc_node_get_next_i(mc_sess_hdl, dev_id, N[-1], 2*self.mc.mc_node_get_count(mc_sess_hdl, dev_id))
            self.assertEqual(nodes, set(N[:N.index(-1)]))


        finally:
            print("Cleaning up")
            sys.stdout.flush()
            for n in nodes:
                self.mc.mc_node_destroy(mc_sess_hdl, dev_id, n)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestGetEcmp(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        global t
        if test_param_get("target") == "bmv2":
            return
        setup_random()
        mc_sess_hdl = self.mc.mc_create_session()
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]
        ecmps = set()

        try:
            # Count should be zero
            c = self.mc.mc_ecmp_get_count(mc_sess_hdl, dev_id)
            self.assertEqual(c, 0)
            # No ecmps yet so this should fail

            try:
                ecmp_hdl = self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass
            # Should not be able to get any "next" ecmps
            try:
                ecmp_hdls = self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, 0, 10)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass
            try:
                ecmp_hdls = self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, 1, 1)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass
            try:
                ecmp_hdls = self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, 0x12345678, 1000)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass

            # Create a single ecmp
            ecmps.add( self.mc.mc_ecmp_create(mc_sess_hdl, dev_id) )
            # Count should be one
            c = self.mc.mc_ecmp_get_count(mc_sess_hdl, dev_id)
            self.assertEqual(c, 1)
            # Get the handle and make sure it is the correct handle
            ecmp_hdl = self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id)
            self.assertIn(ecmp_hdl, ecmps)
            # Should not be able to get anymore ecmps
            try:
                ecmp_hdls = self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, ecmp_hdl, 1234)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass

            # Try with more ecmps
            for i in range(1,25):
                ecmps.add( self.mc.mc_ecmp_create(mc_sess_hdl, dev_id) )

            # Can we get all of them with get-first?
            while len(ecmps):
                n = self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id)
                self.assertIn(n, ecmps)
                ecmps.remove(n)
                self.mc.mc_ecmp_destroy(mc_sess_hdl, dev_id, n)
            # No more ecmps, should fail again
            try:
                n = self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id)
                self.assertTrue(False)
            except InvalidMcOperation as e:
                pass

            # Add ecmps again and get them all
            for i in range(5000):
                ecmps.add( self.mc.mc_ecmp_create(mc_sess_hdl, dev_id) )
            c = self.mc.mc_ecmp_get_count(mc_sess_hdl, dev_id)
            self.assertEqual(c, 5000)
            N = [self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id)]
            N += self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, N[0], 4999)
            self.assertEqual(ecmps, set(N))

            # Get them one at a time
            N = [self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id)]
            for _ in range(4999):
                N += self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, N[-1], 1)
            self.assertEqual(ecmps, set(N))

            # Get more ecmps than are actually present
            N = [self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id)]
            N += self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, N[-1], 2000)
            N += self.mc.mc_ecmp_get_next_i(mc_sess_hdl, dev_id, N[-1], 2*self.mc.mc_ecmp_get_count(mc_sess_hdl, dev_id))
            self.assertEqual(ecmps, set(N[:N.index(-1)]))


        finally:
            print("Cleaning up")
            sys.stdout.flush()
            for n in ecmps:
                self.mc.mc_ecmp_destroy(mc_sess_hdl, dev_id, n)
            self.mc.mc_destroy_session(mc_sess_hdl)


class TestGetMgidMbrs(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["multicast_test"])
    def runTest(self):
        global t
        if test_param_get("target") == "bmv2":
            return
        setup_random()
        mc_sess_hdl = self.mc.mc_create_session()
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes is",num_pipes)
        pipe_list = [x for x in range(num_pipes)]

        grp_to_nodes = {}
        grp_to_ecmps = {}
        try:
            # Get a few groups.
            grp_hdls = set()
            if test_param_get("arch") == "tofino3":
                mgids = set(random.sample( range(1, 0x10000), 500 ))
            else:
                mgids = set(random.sample( range(0, 0x10000), 500 ))
            for mgid in mgids:
                h = self.mc.mc_mgrp_create(mc_sess_hdl, dev_id, hex_to_i16(mgid))
                grp_hdls.add(h)
                grp_to_nodes[h] = list()
                grp_to_ecmps[h] = list()
                # Add a few nodes.
                node_cnt = random.choice([0, 1, 2, 25, 6, 7, 200])
                for _ in range(node_cnt):
                    n = self.mc.mc_node_create(mc_sess_hdl, dev_id, hex_to_i16(random.randint(0, 0xFFFF)), set_port_map([]), set_lag_map([]))
                    xid_valid = random.choice([0, 1])
                    xid = 0
                    if xid_valid: xid = random.randint(0, 0xFFFF)
                    self.mc.mc_associate_node(mc_sess_hdl, dev_id, h, n, hex_to_i16(xid), xid_valid)
                    grp_to_nodes[h].append( mc_mgrp_node_mbr(n, xid_valid, hex_to_i16(xid)) )
                # Add a few ecmps.
                ecmp_cnt = random.choice([0, 0, 1, 2, 3, 12])
                for _ in range(ecmp_cnt):
                    e = self.mc.mc_ecmp_create(mc_sess_hdl, dev_id)
                    xid_valid = random.choice([0, 1])
                    xid = 0
                    if xid_valid: xid = random.randint(0, 0xFFFF)
                    self.mc.mc_associate_ecmp(mc_sess_hdl, dev_id, h, e, hex_to_i16(xid), xid_valid)
                    grp_to_ecmps[h].append( mc_mgrp_ecmp_mbr(e, xid_valid, hex_to_i16(xid)) )

            # Try the get count API.
            for h in grp_hdls:
                node_cnt = self.mc.mc_mgrp_get_node_mbr_count(mc_sess_hdl, dev_id, h)
                ecmp_cnt = self.mc.mc_mgrp_get_ecmp_mbr_count(mc_sess_hdl, dev_id, h)
                self.assertEqual(len(grp_to_nodes[h]), node_cnt)
                self.assertEqual(len(grp_to_ecmps[h]), ecmp_cnt)

            # Try the get first API.
            for h in grp_hdls:
                if len(grp_to_nodes[h]):
                    first = self.mc.mc_mgrp_get_first_node_mbr(mc_sess_hdl, dev_id, h)
                    self.assertIn(first, list(grp_to_nodes[h]))
                else:
                    # If there are no nodes it should return an error.
                    try:
                        self.mc.mc_mgrp_get_first_node_mbr(mc_sess_hdl, dev_id, h)
                        self.assertTrue(False)
                    except InvalidMcOperation as e:
                        pass
                if len(grp_to_ecmps[h]):
                    first = self.mc.mc_mgrp_get_first_ecmp_mbr(mc_sess_hdl, dev_id, h)
                    self.assertIn(first, grp_to_ecmps[h])
                else:
                    # If there are no ecmps it should return an error.
                    try:
                        self.mc.mc_mgrp_get_first_ecmp_mbr(mc_sess_hdl, dev_id, h)
                        self.assertTrue(False)
                    except InvalidMcOperation as e:
                        pass

            # Try the get next by 1
            for h in grp_hdls:
                if not len(grp_to_nodes[h]): continue
                mbrs = list()
                count = self.mc.mc_mgrp_get_node_mbr_count(mc_sess_hdl, dev_id, h)
                x = self.mc.mc_mgrp_get_first_node_mbr(mc_sess_hdl, dev_id, h)
                self.assertIn(x, grp_to_nodes[h])
                mbrs.append( mc_mgrp_node_mbr(x.node_hdl, x.xid_valid, x.xid) )
                for _ in range(count-1):
                    x = self.mc.mc_mgrp_get_next_i_node_mbr(mc_sess_hdl, dev_id, h, x.node_hdl, 1)
                    self.assertEqual(len(x), 1)
                    x = x[0]
                    self.assertIn(x, grp_to_nodes[h])
                    mbrs.append( mc_mgrp_node_mbr(x.node_hdl, x.xid_valid, x.xid) )
                self.assertEqual(len(mbrs), len(grp_to_nodes[h]))
            for h in grp_hdls:
                if not len(grp_to_ecmps[h]): continue
                mbrs = list()
                count = self.mc.mc_mgrp_get_ecmp_mbr_count(mc_sess_hdl, dev_id, h)
                x = self.mc.mc_mgrp_get_first_ecmp_mbr(mc_sess_hdl, dev_id, h)
                self.assertIn(x, grp_to_ecmps[h])
                mbrs.append( mc_mgrp_ecmp_mbr(x.ecmp_hdl, x.xid_valid, x.xid) )
                for _ in range(count-1):
                    x = self.mc.mc_mgrp_get_next_i_ecmp_mbr(mc_sess_hdl, dev_id, h, x.ecmp_hdl, 1)
                    self.assertEqual(len(x), 1)
                    self.assertIn(x[0], grp_to_ecmps[h])
                    x = x[0]
                    mbrs.append( mc_mgrp_ecmp_mbr(x.ecmp_hdl, x.xid_valid, x.xid) )
                self.assertEqual(len(mbrs), len(grp_to_ecmps[h]))

            # Try the get next by count-1
            for h in grp_hdls:
                if not len(grp_to_nodes[h]): continue
                mbrs = list()
                count = self.mc.mc_mgrp_get_node_mbr_count(mc_sess_hdl, dev_id, h)
                x = self.mc.mc_mgrp_get_first_node_mbr(mc_sess_hdl, dev_id, h)
                self.assertIn(x, grp_to_nodes[h])
                mbrs.append( mc_mgrp_node_mbr(x.node_hdl, x.xid_valid, x.xid) )
                if count > 1:
                    X = self.mc.mc_mgrp_get_next_i_node_mbr(mc_sess_hdl, dev_id, h, x.node_hdl, count-1)
                    self.assertEqual(len(X), count-1)
                    for x in X:
                        self.assertIn(x, grp_to_nodes[h])
                        mbrs.append( mc_mgrp_node_mbr(x.node_hdl, x.xid_valid, x.xid) )
                else:
                    try:
                        self.mc.mc_mgrp_get_next_i_node_mbr(mc_sess_hdl, dev_id, h, x.node_hdl, 1)
                        self.assertTrue(False)
                    except InvalidMcOperation as e:
                        pass
                self.assertEqual(len(mbrs), len(grp_to_nodes[h]))
            for h in grp_hdls:
                if not len(grp_to_ecmps[h]): continue
                mbrs = list()
                count = self.mc.mc_mgrp_get_ecmp_mbr_count(mc_sess_hdl, dev_id, h)
                x = self.mc.mc_mgrp_get_first_ecmp_mbr(mc_sess_hdl, dev_id, h)
                self.assertIn(x, grp_to_ecmps[h])
                mbrs.append( mc_mgrp_ecmp_mbr(x.ecmp_hdl, x.xid_valid, x.xid) )
                if count > 1:
                    X = self.mc.mc_mgrp_get_next_i_ecmp_mbr(mc_sess_hdl, dev_id, h, x.ecmp_hdl, count-1)
                    self.assertEqual(len(X), count-1)
                    for x in X:
                        self.assertIn(x, grp_to_ecmps[h])
                        mbrs.append( mc_mgrp_ecmp_mbr(x.ecmp_hdl, x.xid_valid, x.xid) )
                else:
                    try:
                        self.mc.mc_mgrp_get_next_i_ecmp_mbr(mc_sess_hdl, dev_id, h, x.ecmp_hdl, 1)
                        self.assertTrue(False)
                    except InvalidMcOperation as e:
                        pass
                self.assertEqual(len(mbrs), len(grp_to_ecmps[h]))

            # Try the get next by more than count
            for h in grp_hdls:
                if not len(grp_to_nodes[h]): continue
                mbrs = list()
                count = self.mc.mc_mgrp_get_node_mbr_count(mc_sess_hdl, dev_id, h)
                x = self.mc.mc_mgrp_get_first_node_mbr(mc_sess_hdl, dev_id, h)
                self.assertIn(x, grp_to_nodes[h])
                mbrs.append( mc_mgrp_node_mbr(x.node_hdl, x.xid_valid, x.xid) )
                if count > 1:
                    X = self.mc.mc_mgrp_get_next_i_node_mbr(mc_sess_hdl, dev_id, h, x.node_hdl, 2*count)
                    self.assertEqual(len(X), 2*count)
                    for x in X:
                        if x.node_hdl != -1:
                            self.assertIn(x, grp_to_nodes[h])
                            mbrs.append( mc_mgrp_node_mbr(x.node_hdl, x.xid_valid, x.xid) )
                else:
                    try:
                        self.mc.mc_mgrp_get_next_i_node_mbr(mc_sess_hdl, dev_id, h, x.node_hdl, 2*count)
                        self.assertTrue(False)
                    except InvalidMcOperation as e:
                        pass
                self.assertEqual(len(mbrs), len(grp_to_nodes[h]))
            for h in grp_hdls:
                if not len(grp_to_ecmps[h]): continue
                mbrs = list()
                count = self.mc.mc_mgrp_get_ecmp_mbr_count(mc_sess_hdl, dev_id, h)
                x = self.mc.mc_mgrp_get_first_ecmp_mbr(mc_sess_hdl, dev_id, h)
                self.assertIn(x, grp_to_ecmps[h])
                mbrs.append( mc_mgrp_ecmp_mbr(x.ecmp_hdl, x.xid_valid, x.xid) )
                if count > 1:
                    X = self.mc.mc_mgrp_get_next_i_ecmp_mbr(mc_sess_hdl, dev_id, h, x.ecmp_hdl, 2*count)
                    self.assertEqual(len(X), 2*count)
                    for x in X:
                        if x.ecmp_hdl != -1:
                            self.assertIn(x, grp_to_ecmps[h])
                            mbrs.append( mc_mgrp_ecmp_mbr(x.ecmp_hdl, x.xid_valid, x.xid) )
                else:
                    try:
                        self.mc.mc_mgrp_get_next_i_ecmp_mbr(mc_sess_hdl, dev_id, h, x.ecmp_hdl, 2*count)
                        self.assertTrue(False)
                    except InvalidMcOperation as e:
                        pass
                self.assertEqual(len(mbrs), len(grp_to_ecmps[h]))

            # Try a few invalid cases
            for h in grp_hdls: break
            # Bad session handle
            try:
                self.mc.mc_mgrp_get_node_mbr_count(63, dev_id, h)
                self.assertTrue(False)
            except InvalidMcOperation as e: pass
            try:
                self.mc.mc_mgrp_get_ecmp_mbr_count(63, dev_id, h)
                self.assertTrue(False)
            except InvalidMcOperation as e: pass
            # Bad group handle
            try:
                self.mc.mc_mgrp_get_first_node_mbr(mc_sess_hdl, dev_id, 0x7FFFFFF)
                self.assertTrue(False)
            except InvalidMcOperation as e: pass
            try:
                self.mc.mc_mgrp_get_first_ecmp_mbr(mc_sess_hdl, dev_id, 0x7FFFFFF)
                self.assertTrue(False)
            except InvalidMcOperation as e: pass
            # Bad member handle
            try:
                self.mc.mc_mgrp_get_next_i_node_mbr(mc_sess_hdl, dev_id, h, 0x7FFFFFF, 1)
                self.assertTrue(False)
            except InvalidMcOperation as e: pass
            try:
                self.mc.mc_mgrp_get_next_i_ecmp_mbr(mc_sess_hdl, dev_id, h, 0x7FFFFFF, 1)
                self.assertTrue(False)
            except InvalidMcOperation as e: pass


        finally:
            print("Cleaning up groups")
            while self.mc.mc_mgrp_get_count(mc_sess_hdl, dev_id):
                self.mc.mc_mgrp_destroy(mc_sess_hdl, dev_id, self.mc.mc_mgrp_get_first(mc_sess_hdl, dev_id))
            print("Cleaning up nodes")
            while self.mc.mc_node_get_count(mc_sess_hdl, dev_id):
                self.mc.mc_node_destroy(mc_sess_hdl, dev_id, self.mc.mc_node_get_first(mc_sess_hdl, dev_id))
            print("Cleaning up ECMPs")
            while self.mc.mc_ecmp_get_count(mc_sess_hdl, dev_id):
                self.mc.mc_ecmp_destroy(mc_sess_hdl, dev_id, self.mc.mc_ecmp_get_first(mc_sess_hdl, dev_id))
            print("Cleaning up session")
            self.mc.mc_destroy_session(mc_sess_hdl)
            print("Done")
