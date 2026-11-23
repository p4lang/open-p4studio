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
from p4testutils.misc_utils import *

import os

prefix=[]
mod_name=[""]
DEF_MGID=0

# Only supported on tofino1, for Tofino2/Tofino3 use pgrs_tof2
if test_param_get("arch") != "tofino":
    print("pgrs only Supported for tofino1, use pgrs_tof2 for tofino2/tofino3")
    assert test_param_get("arch") == "tofino"

if test_param_get("arch") == "tofino":
    from pgrs.p4_pd_rpc.ttypes import *
    mod_name=["pgrs"]
    DEF_MGID=0xffff
else:
    from p4_pd_rpc.ttypes import *
    prefix=["pgrs"]

from conn_mgr_pd_rpc.ttypes import *
from mc_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *
from ptf_port import *

dev_id = 0

g_target_hw = test_param_get('target').lower() == "hw"
swports = get_sw_ports()

def verify_multiple_packets(test, port, pkts=[], pkt_lens=[], device_number=0, tmo=None, slack=0):

    """
    Checks for packets on a specific port, and compares them to a list of packets provided
    by the user. This is useful where the order of the packet arrival is unknown
    For e.g., if the ingress ports are different
    """


    rx_pkt_status = [False] * len(pkts)
    if tmo is None:
        tmo = ptf.ptfutils.default_negative_timeout
    rx_pkts = 0
    while rx_pkts < len(pkts):
        (rcv_device, rcv_port, rcv_pkt, pkt_time) = dp_poll(
                test,
                device_number=device_number,
                port_number=port,
                timeout=tmo)
        if not rcv_pkt:
            if slack:
                test.assertTrue((slack > (len(pkts) - rx_pkts)), "Timeout:Port[%d]:Got:[%d]:Allowed slack[%d]:Left[%d]\n" %(port, rx_pkts, slack, len(pkts)-rx_pkts))
                return
            else:
                print("No more packets but still expecting", len(pkts)-rx_pkts)
                sys.stdout.flush()
                for i, a_pkt in enumerate(pkts):
                    if not rx_pkt_status[i]:
                        print(format_packet(a_pkt))
                sys.stdout.flush()
                test.assertTrue(False, "Timeout:Port:[%d]:Got[%d]:Left[%d]\n" %(port,rx_pkts,len(pkts)-rx_pkts))
                return

        rx_pkts = rx_pkts + 1
        found = False
        for i, a_pkt in enumerate(pkts):
            if bytes(a_pkt) == bytes(rcv_pkt[:pkt_lens[i]]) and not rx_pkt_status[i]:
                rx_pkt_status[i] = True
                found = True
                print("Matched packet", i)
                break
        if not found:
            test.assertTrue(False, "RxPort:[%u]:Pkt#[%u]:Pkt:%s:Unmatched\n" %(port, rx_pkts, ":".join("{:02x}".format(ord(c)) for c in rcv_pkt[:pkt_lens[0]])))


def port_tbl_add(test, hndl_list, shdl, dev_tgt, port, dport, skip, pg_port=0, test_recirc=0, mgid1=DEF_MGID, mgid2=DEF_MGID, pfe=0):
    match_spec = pgrs_port_tbl_match_spec_t( hex_to_i16(port) )
    action_spec = pgrs_set_md_action_spec_t( hex_to_i16(dport), skip, pg_port, test_recirc, hex_to_i16(mgid1), hex_to_i16(mgid2), pfe)
    h = test.client.port_tbl_table_add_with_set_md( hex_to_i32(shdl), dev_tgt, match_spec, action_spec )
    if hndl_list is not None:
        hndl_list.append(h)
    return h
def port_tbl_del(test, hdl_list, shdl, dev, entry_hdl):
    test.client.port_tbl_table_delete(hex_to_i32(shdl), dev, hex_to_i32(entry_hdl))
    hdl_list.remove(entry_hdl)

def pg_verify_timer_add(test, hndl_list, shdl, dev_tgt, sport):
    match_spec = pgrs_pg_verify_timer_match_spec_t( hex_to_i16(sport) )
    h = test.client.pg_verify_timer_table_add_with_timer_ok( hex_to_i32(shdl), dev_tgt, match_spec )
    hndl_list.append(h)

def pg_verify_recirc_add(test, hndl_list, shdl, dev_tgt, sport):
    match_spec = pgrs_pg_verify_recirc_match_spec_t( hex_to_i16(sport) )
    h = test.client.pg_verify_recirc_table_add_with_recirc_ok( hex_to_i32(shdl), dev_tgt, match_spec )
    hndl_list.append(h)

def portToBitIdx(port):
    pipe = port_to_pipe(port)
    index = port_to_pipe_local_port(port)
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

def ports_down(test, ports):
    if g_target_hw:
        for port in ports:
            test.pal.pal_port_dis(dev_id, port)
            waitTime = 0
            while(test.pal.pal_port_oper_status_get(dev_id, port) == 1):
                time.sleep(1)
                waitTime += 1
                if (waitTime > 10):
                    print("Port {} not coming down after 60s wait".format(port))
                    break
    else:
        for port in ports:
            take_port_down(port)
        time.sleep(3)

def ports_up(test, ports):
    if g_target_hw:
        for port in ports:
            test.pal.pal_port_enable(dev_id, port)
            waitTime = 0
            while(test.pal.pal_port_oper_status_get(dev_id, port) == 0):
               time.sleep(1)
               waitTime += 1
               if (waitTime > 70):
                   print("Port {} not coming up after 70s wait".format(port))
                   break
    else:
        for port in ports:
            bring_port_up(port)
        time.sleep(3)

class TestRecirc(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):

        sess_hdl = self.conn_mgr.client_init()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_0 = DevTarget_t(dev_id, hex_to_i16(0))
        dev_tgt_1 = DevTarget_t(dev_id, hex_to_i16(1))
        dev_tgt_2 = DevTarget_t(dev_id, hex_to_i16(2))
        dev_tgt_3 = DevTarget_t(dev_id, hex_to_i16(3))

        port_tbl_handles = []

        added_ports = []
        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64
        try:
            self.client.port_tbl_set_property(sess_hdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);

            # The harlyn runner is adding all ports as 10g ports.  Remove the
            # PGR ports first so the test can re-create them with the speeds it
            # needs.
            for pipe in range(pipe_list_len):
                for port in range(64,72):
                    try:
                        #print("Attempting to remove port",make_port(pipe, port), "on device", dev_id)
                        #sys.stdout.flush()
                        self.devport_mgr.devport_mgr_remove_port(dev_id, make_port(pipe, port))
                        #print("  Success!")
                        #sys.stdout.flush()
                    except InvalidDevportMgrOperation as e:
                        #print("  Failed, continuing on")
                        #sys.stdout.flush()
                        pass

            # Before adding ports, enable recirculation

            # Enable recirculation on the port-group 16 ports
            if 0 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 64);
            if 1 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 192);
            if 2 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 320);
            if 3 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 448);

            # Add ports now
            # Pipe 0
            if 0 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, 64, speed_100g, 0)
                added_ports.append(make_port(0,64))
                self.devport_mgr.devport_mgr_add_port(dev_id, 68, speed_50g, 0)
                added_ports.append(make_port(0,68))
                self.devport_mgr.devport_mgr_add_port(dev_id, 70, speed_50g, 0)
                added_ports.append(make_port(0,70))

            # Pipe 1
            if 1 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(1,64), speed_10g, 0)
                added_ports.append(make_port(1,64))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(1,65), speed_25g, 0)
                added_ports.append(make_port(1,65))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(1,66), speed_50g, 0)
                added_ports.append(make_port(1,66))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(1,68), speed_100g, 0)
                added_ports.append(make_port(1,68))

            # Pipe 2
            if 2 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,64), speed_50g, 0)
                added_ports.append(make_port(2,64))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,66), speed_25g, 0)
                added_ports.append(make_port(2,66))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,67), speed_25g, 0)
                added_ports.append(make_port(2,67))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,68), speed_25g, 0)
                added_ports.append(make_port(2,68))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,69), speed_25g, 0)
                added_ports.append(make_port(2,69))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,70), speed_25g, 0)
                added_ports.append(make_port(2,70))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,71), speed_25g, 0)
                added_ports.append(make_port(2,71))

            # Pipe 3
            if 3 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,64), speed_25g, 0)
                added_ports.append(make_port(3,64))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,65), speed_25g, 0)
                added_ports.append(make_port(3,65))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,66), speed_25g, 0)
                added_ports.append(make_port(3,66))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,67), speed_25g, 0)
                added_ports.append(make_port(3,67))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,68), speed_50g, 0)
                added_ports.append(make_port(3,68))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,70), speed_25g, 0)
                added_ports.append(make_port(3,70))
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,71), speed_25g, 0)
                added_ports.append(make_port(3,71))

            # Program tables to snake a packet from port zero through all the
            # port group 16 ports and then send it back out port zero.
            p1 = swports[0]
            p2 = swports[-1]
            p1_dt = DevTarget_t(dev_id, port_to_pipe(swports[0]))
            p2_dt = DevTarget_t(dev_id, port_to_pipe(swports[-1]))

            if 3 in pipe_list:
                port_tbl_add(self, port_tbl_handles, sess_hdl, p1_dt,      p1,  64, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_0,  64, 192, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 192, 193, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 193, 194, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 194, 320, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_2, 320, 322, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_2, 322, 323, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_2, 323, 448, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_3, 448, 449, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_3, 449, 450, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_3, 450, 451, 1, 0)
                port_handle_451 = port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_3, 451, p2, 1, 0)
                last_port = 451
                last_dev_tgt = dev_tgt_3
            else:
                port_tbl_add(self, port_tbl_handles, sess_hdl, p1_dt,      p1,  64, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_0,  64, 192, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 192, 193, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 193, 194, 1, 0)
                port_handle_451 = port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 194, p2, 1, 0)
                last_port = 194
                last_dev_tgt = dev_tgt_1

            # Program tables to snake a packet from port one through all the
            # port group 17 ports and then send it back out port one.
            if 3 in pipe_list:
                port_tbl_add(self, port_tbl_handles, sess_hdl, p2_dt,      p2,  68, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_0,  68,  70, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_0,  70, 196, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 196, 324, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_2, 324, 325, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_2, 325, 326, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_2, 326, 327, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_2, 327, 452, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_3, 452, 454, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_3, 454, 455, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_3, 455,  p1, 1, 0)
            else:
                port_tbl_add(self, port_tbl_handles, sess_hdl, p2_dt,      p2,  68, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_0,  68,  70, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_0,  70, 196, 1, 0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 196,  p1, 1, 0)

            # Wait for all pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)

            pkt = simple_eth_packet(pktlen=1000, eth_dst='80:01:02:03:04:05')
            print("Sending packet through group 16, in", p1, "out", p2)
            sys.stdout.flush()
            send_packet(self, p1, pkt)
            verify_packet(self, pkt, p2, timeout=15)

            print("Sending packet through group 17, in", p2, "out", p1)
            sys.stdout.flush()
            send_packet(self, p2, pkt)
            verify_packet(self, pkt, p1, timeout=15)

            # Program tables to snake a packet from port zero through all the
            # port group 16 ports and then all the port group 17 ports and then
            # send it back out port one.
            port_tbl_del(self, port_tbl_handles, sess_hdl, dev_id, port_handle_451)
            port_tbl_add(self, port_tbl_handles, sess_hdl, last_dev_tgt, last_port,  68, 1, 0)
            self.conn_mgr.complete_operations(sess_hdl)
            print("Sending packet through group 16 and 17, in", p1, "out", p1)
            sys.stdout.flush()
            send_packet(self, p1, pkt)
            verify_packet(self, pkt, p1, timeout=15)
            verify_no_other_packets(self)

        finally:
            print("ports to remove are:", added_ports)
            sys.stdout.flush()
            for p in added_ports:
                #print("Attempting to remove port", p, "on device", dev_id)
                #sys.stdout.flush()
                self.devport_mgr.devport_mgr_remove_port(dev_id, p)

            # Add the ports back as 10g ports since that is how they started.
            print("Adding back ports in 10g mode")
            sys.stdout.flush()
            for pipe in range(pipe_list_len):
                for port in range(64,72):
                    self.devport_mgr.devport_mgr_add_port(dev_id, make_port(pipe, port), speed_10g, 0)

            print("Cleaning up port table")
            sys.stdout.flush()
            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))

            print("Closing sessions")
            sys.stdout.flush()
            sess_hdl = self.conn_mgr.client_cleanup(sess_hdl)


class TestRecircLocal(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_0 = DevTarget_t(dev_id, hex_to_i16(0))
        dev_tgt_1 = DevTarget_t(dev_id, hex_to_i16(1))
        dev_tgt_2 = DevTarget_t(dev_id, hex_to_i16(2))
        dev_tgt_3 = DevTarget_t(dev_id, hex_to_i16(3))

        port_tbl_handles = []

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64
        try:

            self.client.port_tbl_set_property(sess_hdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);

            # The harlyn runner is adding all ports as 10g ports.  Remove the
            # PGR ports first so the test can re-create them with the speeds it
            # needs.
            for pipe in range(pipe_list_len):
                for port in range(64,72):
                    try:
                        self.devport_mgr.devport_mgr_remove_port(dev_id, make_port(pipe, port))
                    except InvalidDevportMgrOperation as e:
                        pass

            # Before adding ports, enable recirculation

            # Enable recirculation on the port-group 16 ports
            if 0 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 64);
            if 1 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 192);
            if 2 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 320);
            if 3 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 448);

            # Add ports now
            # Pipe 0
            if 0 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, 64, speed_100g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 68, speed_50g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 70, speed_50g, 0)

            # Pipe 1
            if 1 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, 192, speed_10g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 193, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 194, speed_50g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 196, speed_100g, 0)

            # Pipe 2
            if 2 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, 320, speed_50g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 322, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 323, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 324, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 325, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 326, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 327, speed_25g, 0)

            # Pipe 3
            if 3 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, 448, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 449, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 450, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 451, speed_25g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 452, speed_50g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 454, speed_10g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, 455, speed_10g, 0)



            # Let each port in the test run the do_local_recirc table.
            recirc_ports = [make_port(p,64) for p in pipe_list]
            non_recirc_ports = list(set(swports) - set(recirc_ports))
            non_recirc_ports.sort()
            for p in non_recirc_ports:
                port_tbl_add(self, port_tbl_handles, sess_hdl, DevTarget_t(dev_id,port_to_pipe(p)), p, 9, 1, 0, 1)
            act_spec = pgrs_local_recirc_action_spec_t( hex_to_i32(64) )
            self.client.do_local_recirc_set_default_action_local_recirc(sess_hdl, dev_tgt, act_spec)


            # Let local port 64 in each pipe send out of one of the test's ports
            if 0 in pipe_list:
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_0, recirc_ports[0], non_recirc_ports[0],   1, 0, 0)
            if 1 in pipe_list:
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, recirc_ports[1], non_recirc_ports[1%len(non_recirc_ports)], 1, 0, 0)
            if 2 in pipe_list:
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_2, recirc_ports[2], non_recirc_ports[2%len(non_recirc_ports)], 1, 0, 0)
            if 3 in pipe_list:
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_3, recirc_ports[3], non_recirc_ports[3%len(non_recirc_ports)], 1, 0, 0)

            # Wait for all pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)

            pkt = simple_eth_packet(pktlen=1000, eth_dst='80:01:02:03:04:05')

            for port in non_recirc_ports:
                pipe = port_to_pipe(port)
                eport = non_recirc_ports[pipe % len(swports)]
                print("Sending to port", port, "expecting rx on port", eport)
                sys.stdout.flush()
                send_packet(self, port, pkt)
                verify_packet(self, pkt, eport, timeout=5)
            verify_no_other_packets(self)

        finally:
            if 0 in pipe_list:
                self.devport_mgr.devport_mgr_remove_port(dev_id, 64)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 68)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 70)

            if 1 in pipe_list:
                self.devport_mgr.devport_mgr_remove_port(dev_id, 192)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 193)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 194)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 196)

            if 2 in pipe_list:
                self.devport_mgr.devport_mgr_remove_port(dev_id, 320)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 322)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 323)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 324)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 325)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 326)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 327)

            if 3 in pipe_list:
                self.devport_mgr.devport_mgr_remove_port(dev_id, 448)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 449)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 450)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 451)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 452)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 454)
                self.devport_mgr.devport_mgr_remove_port(dev_id, 455)

            # Add the ports back as 10g ports since that is how they started.
            for pipe in range(pipe_list_len):
                for port in range(64,72):
                    self.devport_mgr.devport_mgr_add_port(dev_id, make_port(pipe, port), speed_10g, 0)

            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))

            sess_hdl = self.conn_mgr.client_cleanup(sess_hdl)


# This test case needs to be reworked.
# mgid needs to be non-zero to work on behavioral
# All recirculation ports that can get packets need to be enabled

class TestRecircAll(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        mc_sess_hdl = self.mc.mc_create_session()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_0 = DevTarget_t(dev_id, hex_to_i16(0))
        dev_tgt_1 = DevTarget_t(dev_id, hex_to_i16(1))
        dev_tgt_2 = DevTarget_t(dev_id, hex_to_i16(2))
        dev_tgt_3 = DevTarget_t(dev_id, hex_to_i16(3))

        port_tbl_handles = []
        mgrp_hdls = []
        mgrp_node_hdls = []
        mgid_to_hdl = {}
        hdl_to_mgid = {}
        mgrp_copies = {}
        mgrp_members = {}
        mgrp_mbr_to_egr_port = {}

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64

        setup_random()

        all_recirc_ports = []
        for pipe in range(pipe_list_len):
            for port in range(64,72):
                all_recirc_ports.append( make_port(pipe,port) )
        recirc_ports = []
        recirc_ports_0 = [make_port(0,64),
                        make_port(0,71)]
        recirc_ports_1 = [make_port(1,64),
                        make_port(1,68)]
        recirc_ports_2 = [make_port(2,64),
                        make_port(2,68)]
        recirc_ports_3 = [make_port(3,66),
                        make_port(3,68)]
        if 0 in pipe_list:
            recirc_ports.extend(recirc_ports_0)
        if 1 in pipe_list:
            recirc_ports.extend(recirc_ports_1)
        if 2 in pipe_list:
            recirc_ports.extend(recirc_ports_2)
        if 3 in pipe_list:
            recirc_ports.extend(recirc_ports_3)
        normal_ports = list(set(swports) - set(recirc_ports))


        try:
            self.client.port_tbl_set_property(sess_hdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);

            # The harlyn runner is adding all ports as 10g ports.  Remove the
            # PGR ports first so the test can re-create them with the speeds it
            # needs.
            for port in all_recirc_ports:
                try:
                    self.devport_mgr.devport_mgr_remove_port(dev_id, port)
                except InvalidDevportMgrOperation as e:
                    pass

            # Before adding ports, enable recirculation

            # Enable recirculation on the port-group 16 ports
            if 0 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 64);
            if 1 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 192);
            if 2 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 320);
            if 3 in pipe_list:
                self.conn_mgr.recirculation_enable(sess_hdl, dev_id, 448);

            # Add ports now
            # Pipe 0
            if 0 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(0,64), speed_10g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(0,71), speed_10g, 0)

            # Pipe 1
            if 1 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(1,64), speed_100g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(1,68), speed_100g, 0)

            # Pipe 2
            if 2 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,64), speed_100g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(2,68), speed_100g, 0)

            # Pipe 3
            if 3 in pipe_list:
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,66), speed_50g, 0)
                self.devport_mgr.devport_mgr_add_port(dev_id, make_port(3,68), speed_50g, 0)



            # Let each port set an MGID
            x = 1
            mgids = []
            for port in normal_ports:
                dt = DevTarget_t(dev_id, port_to_pipe(port))
                port_tbl_add(self, port_tbl_handles, sess_hdl, dt, port, 511, 1, mgid1=x, mgid2=x+1)
                mgids.extend( [x, x+1] )
                x += 2

            # Let each recirc port sent out of a local port by using its port
            # number as an index to the port list.
            for port in recirc_ports:
                dt = DevTarget_t(dev_id, port_to_pipe(port))
                exp_port = normal_ports[port % len(normal_ports)]
                port_tbl_add(self, port_tbl_handles, sess_hdl, dt, port, exp_port, 1)
                mgrp_mbr_to_egr_port[port] = exp_port

            # Create MGIDs
            for mgid in mgids:
                mgrp_hdl = self.mc.mc_mgrp_create(mc_sess_hdl, dev_id, hex_to_i16(mgid))
                mgrp_hdls.append(mgrp_hdl)
                mgid_to_hdl[mgid] = mgrp_hdl
                hdl_to_mgid[mgrp_hdl] = mgid
            # Add nodes to the MGIDs
            for mgid in mgids:
                mgrp_hdl = mgid_to_hdl[mgid]
                # Determine how many L1s per MGID
                copy_count = random.randint(1,2)
                mgrp_copies[mgid] = copy_count
                # Determine the membership for the group
                members = random.sample(recirc_ports, random.randint(0,len(recirc_ports)))
                mgrp_members[mgid] = list(members)
                port_map = set_port_map(members)
                lag_map = set_port_map( [] )
                # Create the nodes
                l1_hdls = []
                for rid in range(copy_count):
                    l1_hdl = self.mc.mc_node_create(mc_sess_hdl, dev_id, hex_to_i16(rid), port_map, lag_map)
                    l1_hdls.append( l1_hdl )
                mgrp_node_hdls.append( list(l1_hdls) )
                # Add the nodes to the group
                for l1_hdl in l1_hdls:
                    status = self.mc.mc_associate_node(mc_sess_hdl, dev_id, mgrp_hdl, l1_hdl, 0, 0)

            # Wait for all pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)
            # Wait for multicast APIs to complete.
            self.mc.mc_complete_operations(mc_sess_hdl)

            p = simple_eth_packet(pktlen=85, eth_dst='80:01:02:03:04:05')

            for i,port in enumerate(normal_ports):
                mgid_base = 2*i
                rx_ports = []
                all_copies = []
                for mgid_idx in [mgid_base,mgid_base+1]:
                    mgid = mgids[mgid_idx]
                    for mbr in mgrp_members[mgid]:
                        mbr_list = [mgrp_mbr_to_egr_port[mbr]] * mgrp_copies[mgid]
                        all_copies = all_copies + mbr_list
                        if mbr in recirc_ports:
                            rx_ports = rx_ports + mbr_list

                all_copies.sort()
                rx_ports.sort()
                print("Sending to port", port)
                sys.stdout.flush()
                #print("AllCopies:", all_copies)
                print("Expecting", len(rx_ports), "packets:", rx_ports)
                sys.stdout.flush()
                send_packet(self, port, p)
                for i,rx_port in enumerate(rx_ports):
                    sys.stdout.flush()
                    verify_packet(self, p, rx_port, timeout=7)
                verify_no_other_packets(self)

        finally:
            for port in recirc_ports:
                self.devport_mgr.devport_mgr_remove_port(dev_id, port)

            # Add the ports back as 10g ports since that is how they started.
            for pipe in range(pipe_list_len):
                for port in range(64,72):
                    try:
                        self.devport_mgr.devport_mgr_remove_port(dev_id, make_port(pipe, port))
                    except InvalidDevportMgrOperation as e:
                        pass
            for pipe in range(pipe_list_len):
                for port in range(64,72):
                    self.devport_mgr.devport_mgr_add_port(dev_id, make_port(pipe, port), speed_10g, 0)

            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))

            for mgrp, mbrs in zip(mgrp_hdls, mgrp_node_hdls):
                for mbr in mbrs:
                    self.mc.mc_dissociate_node(mc_sess_hdl, dev_id, mgrp, mbr)
                    self.mc.mc_node_destroy(mc_sess_hdl, dev_id, mbr)
            for mgrp in mgrp_hdls:
                self.mc.mc_mgrp_destroy(mc_sess_hdl, dev_id, mgrp)

            sess_hdl = self.conn_mgr.client_cleanup(sess_hdl)
            self.mc.mc_destroy_session(mc_sess_hdl)



@group("thorough")
class TestPortDownAll(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_ = []
        for pipe in range(pipe_list_len):
            dev_tgt_.append(DevTarget_t(dev_id, hex_to_i16(pipe)))
        pktlen = 100

        # Flap all ports but the first port
        ports_to_flap = swports[1:]

        port_tbl_handles = []
        try:
            self.client.port_tbl_set_property(shdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);

            for pipe in range(pipe_list_len):
                self.conn_mgr.pktgen_enable(shdl, dev_id, make_port(pipe, 68))
                enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, 68))
                self.assertTrue(enabled)

            # All pkt-gen traffic is sent to the first port
            for pipe in range(pipe_list_len):
                port_tbl_add(self, port_tbl_handles, shdl, dev_tgt_[pipe], make_port(pipe, 68), swports[0], 1)

            pda = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.PORT_DOWN,
                                  batch_count=0,
                                  pkt_count=0,
                                  ibg=0,
                                  ipg=0,
                                  src_port=68,
                                  src_port_inc=0,
                                  buffer_offset=0,
                                  length=pktlen-6)


            p = simple_eth_packet(pktlen=pktlen, eth_dst='80:01:02:03:04:05')
            self.conn_mgr.pktgen_write_pkt_buffer(shdl, dev_tgt, 0, pktlen-6, bytes(p)[6:])
            buff = self.conn_mgr.pktgen_read_pkt_buffer(shdl, dev_tgt, 0, pktlen-6)
            self.assertEqual(buff, bytes(p)[6:])

            for pipe in range(pipe_list_len):
                self.conn_mgr.pktgen_cfg_app(shdl, dev_tgt_[pipe], 7-pipe, pda)
                cfg = self.conn_mgr.pktgen_cfg_app_get(shdl, dev_tgt_[pipe], 7-pipe)
                self.assertEqual(cfg, pda)
                self.conn_mgr.pktgen_app_enable(shdl, dev_tgt_[pipe], 7-pipe)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(shdl, dev_tgt_[pipe], 7-pipe)
                self.assertTrue(app_enabled)

            for port in ports_to_flap:
                self.conn_mgr.pktgen_clear_port_down(shdl, dev_id, port)
                self.conn_mgr.complete_operations(shdl)
                cleared = self.conn_mgr.pktgen_port_down_state_get(shdl, dev_id, port)
                self.assertTrue(cleared)

            self.conn_mgr.complete_operations(shdl)

            pkt_lst = []
            pkt_len = [pktlen] * len(ports_to_flap)
            for port in ports_to_flap:
                pipe = port_to_pipe(port)
                app_id = 7-pipe
                dmac='%02x:%02x:%02x:%02x:%02x:%02x' % (app_id | (pipe << 3), 0, port >> 8, port & 0xFF, 0, 0)
                print("Expected pgen header when flapping port", port, dmac)
                sys.stdout.flush()
                p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac)
                pkt_lst.append(p_exp)

            ports_down(self, ports_to_flap)
            ports_up(self, ports_to_flap)

            time.sleep(15)

            print("Expecting", len(pkt_lst), "packets")
            sys.stdout.flush()
            verify_multiple_packets(self, swports[0], pkt_lst, pkt_len, tmo=2)

        finally:
            for pipe in range(pipe_list_len):
                for app_id in range(8):
                    self.conn_mgr.pktgen_app_disable(shdl, dev_tgt_[pipe], app_id)
                    app_enabled = self.conn_mgr.pktgen_app_enable_state_get(shdl, dev_tgt_[pipe], app_id)
                    self.assertFalse(app_enabled)
            for pipe in range(pipe_list_len):
                for port in range (72):
                    self.conn_mgr.pktgen_clear_port_down(shdl, dev_id, make_port(pipe, port))
                    self.conn_mgr.complete_operations(shdl)
                    cleared = self.conn_mgr.pktgen_port_down_state_get(shdl, dev_id, make_port(pipe, port))
                    self.assertTrue(cleared)
            for pipe in range(pipe_list_len):
                self.conn_mgr.pktgen_disable(shdl, dev_id, make_port(pipe, 68))
                enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, 68))
                self.assertFalse(enabled)
            for h in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(h))
            self.conn_mgr.client_cleanup(shdl)

@group("thorough")
class TestPortDown(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        shdl = sess_hdl
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_ = []
        for pipe in range(pipe_list_len):
            dev_tgt_.append(DevTarget_t(dev_id, hex_to_i16(pipe)))

        port_tbl_handles = []

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64
        dst_port = swports[0]
        pktlen = 81
        try:
            self.client.port_tbl_set_property(sess_hdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);

            for pipe in range(pipe_list_len):
                self.conn_mgr.pktgen_enable(sess_hdl, dev_id, make_port(pipe,68))
                enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, 68))
                self.assertTrue(enabled)

            # Packets will be generated on ports 0...p_count-1 in the pipe and
            # be sent out of one fixed test port.
            p_count = 6
            for i in range(p_count):
                for pipe in range(pipe_list_len):
                    port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_[pipe], make_port(pipe,i), dst_port, 1)
            pda_list = []
            for i in range(4):
                cfg = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.PORT_DOWN,
                                    batch_count=0,
                                    pkt_count=p_count-1,
                                    ibg=0,
                                    ipg=1000, # 1 Microsecond
                                    src_port=0,
                                    src_port_inc=1,
                                    buffer_offset=0,
                                    length=pktlen-6)
                pda_list.append(cfg)

            p = simple_eth_packet(pktlen=pktlen, eth_dst='80:01:02:03:04:05')
            #p_str = ''.join(["%02x" % ord(x) for x in str(p)])

            for i in range(4):
                if i in pipe_list:
                    self.conn_mgr.pktgen_cfg_app(sess_hdl, dev_tgt_[i], i, pda_list[i])
                    cfg = self.conn_mgr.pktgen_cfg_app_get(sess_hdl, dev_tgt_[i], i)
                    self.assertEqual(cfg, pda_list[i])

            for pipe in range(pipe_list_len):
                self.conn_mgr.pktgen_write_pkt_buffer(sess_hdl, dev_tgt_[pipe], 0, pktlen-6, bytes(p)[6:])
                buff = self.conn_mgr.pktgen_read_pkt_buffer(sess_hdl, dev_tgt_[pipe], 0, pktlen-6)
                self.assertEqual(buff, bytes(p)[6:])

            # Mark all ports down
            ports_down(self, swports)

            self.dataplane.set_qlen(1024)

            # Enable apps
            for pipe in range(pipe_list_len):
                self.conn_mgr.pktgen_app_enable(sess_hdl, dev_tgt_[pipe], pipe)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[pipe], pipe)
                self.assertTrue(app_enabled)

            # Bring all ports up
            ports_up(self, swports)

            # Wait for a pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)
            verify_no_other_packets(self)

            # Reset value of port_down_dis should prevent any events from being
            # triggered yet.
            ports_down(self, [swports[-1]])
            ports_up(self, [swports[-1]])
            verify_no_other_packets(self)

            # Clear port down state on all ports
            for port in swports:
                self.conn_mgr.pktgen_clear_port_down(sess_hdl, dev_id, port)
                self.conn_mgr.complete_operations(shdl)
                cleared = self.conn_mgr.pktgen_port_down_state_get(shdl, dev_id, port)
                self.assertTrue(cleared)
            self.conn_mgr.complete_operations(sess_hdl)

            # Bring down one port, check packets
            port = swports[1]
            print("Triggering packets for port", port, "down")
            sys.stdout.flush()
            ports_down(self, [port])
            time.sleep(3)

            pkt_lst = []
            pkt_len = [pktlen] * p_count

            for pkt_num in range(p_count):
                pipe = port_to_pipe(port)
                app_id = pipe
                dmac='%02x:%02x:%02x:%02x:%02x:%02x' % (app_id | (pipe << 3), 0, port >> 8, port & 0xFF, pkt_num >> 8, pkt_num & 0xFF)
                p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac)
                pkt_lst.append(p_exp)

            verify_multiple_packets(self, dst_port, pkt_lst, pkt_len, tmo=2)

            # Bring port back up and then down, check no packets
            print("Flapping port", port, "no more packets should be generated")
            sys.stdout.flush()
            for _ in range(10):
                ports_up(self, [port])
                ports_down(self, [port])
            ports_up(self, [port])
            verify_no_other_packets(self)

            # Bring down a second port, check packets
            port = swports[2]
            print("Triggering packets for port", port, "down")
            sys.stdout.flush()
            ports_down(self, [port])
            time.sleep(3)

            pkt_lst = []
            pkt_len = [pktlen] * p_count

            for pkt_num in range(p_count):
                pipe = port_to_pipe(port)
                app_id = pipe
                dmac='%02x:%02x:%02x:%02x:%02x:%02x' % (app_id | (pipe << 3), 0, port >> 8, port & 0xFF, pkt_num >> 8, pkt_num & 0xFF)
                p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac)
                pkt_lst.append(p_exp)

            verify_multiple_packets(self, dst_port, pkt_lst, pkt_len, tmo=2)

            # Bring down first port again, check no packets
            port = swports[1]
            print("Flapping port", port, "again, expecting no packets")
            sys.stdout.flush()
            ports_down(self, [port])
            ports_up(self, [port])
            verify_no_other_packets(self)

            # Clear port down state on first port
            print("Clearing port down state on port", port)
            sys.stdout.flush()
            self.conn_mgr.pktgen_clear_port_down(sess_hdl, dev_id, port)
            self.conn_mgr.complete_operations(shdl)
            cleared = self.conn_mgr.pktgen_port_down_state_get(shdl, dev_id, port)
            self.assertTrue(cleared)
            self.conn_mgr.complete_operations(sess_hdl)
            # Bring port down, check packets
            print("Flapping port", port, "again, expecting packets")
            sys.stdout.flush()
            ports_down(self,[port])
            time.sleep(3)

            pkt_lst = []
            pkt_len = [pktlen] * p_count

            for pkt_num in range(p_count):
                pipe = port_to_pipe(port)
                app_id = pipe
                dmac='%02x:%02x:%02x:%02x:%02x:%02x' % (app_id | (pipe << 3), 0, port >> 8, port & 0xFF, pkt_num >> 8, pkt_num & 0xFF)
                p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac)
                pkt_lst.append(p_exp)

            verify_multiple_packets(self, dst_port, pkt_lst, pkt_len, tmo=2)

            # Bring port up and down, check no packets
            print("Flapping port", port, "again, expecting no packets")
            sys.stdout.flush()
            ports_up(self, [port])
            ports_down(self, [port])
            ports_up(self, [port])
            verify_no_other_packets(self)



        finally:
            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))

            ports_up(self, swports)

            for aid in range(8):
                for pipe in range(pipe_list_len):
                    self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_[pipe], aid)
                    app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[pipe], aid)
                    self.assertFalse(app_enabled)
            for pipe in range(pipe_list_len):
                for port in range(68,72):
                    self.conn_mgr.pktgen_disable(sess_hdl, dev_id, make_port(pipe, port))
                    enabled = self.conn_mgr.pktgen_enable_state_get(sess_hdl, dev_id, make_port(pipe, port))
                    self.assertFalse(enabled)

            self.conn_mgr.client_cleanup(sess_hdl)

class TestTimerOneShot(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_ = []
        for pipe in range(pipe_list_len):
            dev_tgt_.append(DevTarget_t(dev_id, hex_to_i16(pipe)))

        p_count = 2
        pktlen = 100
        polling_limit = 30

        port_tbl_handles = []
        timer_verify_handles = []

        trigger_cntrs = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        trigger_cntrs_exp = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs_exp   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs_exp  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64
        try:
            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(sess_hdl, dev_tgt_[pipe], app)
                    batch_cntrs[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(sess_hdl, dev_tgt_[pipe], app)
                    packet_cntrs[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, dev_tgt_[pipe], app)

                    trigger_cntrs_exp[pipe][app] = trigger_cntrs[pipe][app]
                    batch_cntrs_exp[pipe][app]   = batch_cntrs[pipe][app]
                    packet_cntrs_exp[pipe][app]  = packet_cntrs[pipe][app]

            self.client.port_tbl_set_property(sess_hdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);
            #self.client.pg_verify_timer_set_property(sess_hdl, dev_id, 1, 0, 0);

            for pipe in pipe_list:
                self.conn_mgr.pktgen_enable(sess_hdl, dev_id,  make_port(pipe,68))
                enabled = self.conn_mgr.pktgen_enable_state_get(sess_hdl, dev_id, make_port(pipe, 68))
                self.assertTrue(enabled)

            # All generated packets will go to swport[0]
            # Generated packets will show up with incremental port numbers
            per_pipe_base_ports = [make_port(x,x*4+x) for x in pipe_list]
            for pipe,base in enumerate(per_pipe_base_ports):
                for i in range(p_count):
                    port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_[pipe], base+i, swports[0], 0, 1)
                    pg_verify_timer_add(self, timer_verify_handles, sess_hdl, dev_tgt, base+i)

            self.client.pg_verify_timer_set_default_action_timer_nok(sess_hdl, dev_tgt);
            self.client.pg_verify_port_down_set_default_action_port_down_nok(sess_hdl, dev_tgt);
            self.client.pg_verify_recirc_set_default_action_recirc_nok(sess_hdl, dev_tgt);

            app_cfgs = []
            for pipe in pipe_list:
                x = port_to_pipe_local_port(per_pipe_base_ports[pipe])
                cfg = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.TIMER_ONE_SHOT,
                                     batch_count=3-pipe,
                                     pkt_count=p_count-1,
                                     timer=100,
                                     ibg=1,
                                     ipg=1000, # 1 Microsecond
                                     ipg_jitter=500,
                                     src_port=x,
                                     src_port_inc=1,
                                     buffer_offset=144,
                                     length=pktlen-6 )
                app_cfgs.append(cfg)

            p = simple_eth_packet(pktlen=pktlen)
            p_str = ''.join(["%02x" % ord(x) for x in str(p)])
            for pipe in pipe_list:
                app_id = pipe+1
                batches = 3-pipe+1
                pkt_count = batches*p_count
                trigger_cntrs_exp[pipe][app_id] += 1
                batch_cntrs_exp[pipe][app_id] += batches
                packet_cntrs_exp[pipe][app_id] += pkt_count
                self.conn_mgr.pktgen_cfg_app(sess_hdl, dev_tgt_[pipe], app_id, app_cfgs[pipe])
                cfg = self.conn_mgr.pktgen_cfg_app_get(sess_hdl, dev_tgt_[pipe], app_id)
                self.assertEqual(cfg, app_cfgs[pipe])
                self.conn_mgr.pktgen_write_pkt_buffer(sess_hdl, dev_tgt_[pipe], 144, pktlen-6, bytes(p)[6:])
                buff = self.conn_mgr.pktgen_read_pkt_buffer(sess_hdl, dev_tgt_[pipe], 144, pktlen-6)
                self.assertEqual(buff, bytes(p)[6:])

            for pipe in pipe_list:
                app_id = pipe+1
                # Enable the app, the timer should fire immediately
                print("Enabling app", app_id, "in pipe", pipe)
                print("  Expecting", 3-pipe+1, "batches of", p_count, "packets, total", (3-pipe+1)*p_count, "packets")
                sys.stdout.flush()
                self.conn_mgr.pktgen_app_enable(sess_hdl, dev_tgt_[pipe], app_id)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[pipe], app_id)
                self.assertTrue(app_enabled)
                self.conn_mgr.complete_operations(sess_hdl)
                # Check counters to ensure it has fired.  On SW models (asic model
                # or behaviorial model) this can be slow if the CPU is busy.  Check
                # the counters a few times and wait for it to trigger.
                success = False
                for _ in range(polling_limit):
                    trigger_cntrs[pipe][app_id] = self.conn_mgr.pktgen_get_trigger_counter(sess_hdl, dev_tgt_[pipe], app_id)
                    batch_cntrs[pipe][app_id]   = self.conn_mgr.pktgen_get_batch_counter(sess_hdl, dev_tgt_[pipe], app_id)
                    packet_cntrs[pipe][app_id]  = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, dev_tgt_[pipe], app_id)
                    if trigger_cntrs[pipe][app_id] == trigger_cntrs_exp[pipe][app_id]:
                        # Timer has fired, check if all batches have been made
                        if batch_cntrs[pipe][app_id] == batch_cntrs_exp[pipe][app_id]:
                            # All batches have been made, check if all packets have been made
                            if packet_cntrs[pipe][app_id] == packet_cntrs_exp[pipe][app_id]:
                                success = True
                                break
                    time.sleep(1)
                if not success:
                    print("Pipe", pipe, "app", app_id, "didn't fire/complete.")
                    print("Triggers:", trigger_cntrs[pipe][app_id])
                    print("Batches :", batch_cntrs[pipe][app_id])
                    print("Packets :", packet_cntrs[pipe][app_id])
                    print("Triggers:", trigger_cntrs_exp[pipe][app_id], "(expected)")
                    print("Batches :", batch_cntrs_exp[pipe][app_id], "(expected)")
                    print("Packets :", packet_cntrs_exp[pipe][app_id], "(expected)")
                    sys.stdout.flush()
                    self.assertTrue(success)

                # Check that the correct packets came back.
                batches = 3-pipe+1
                pkt_count = batches*p_count
                pkt_lst = []
                pkt_len = [pktlen] * pkt_count
                for batch in range(batches):
                    for pkt_num in range(p_count):
                        dmac='%02x:00:%02x:%02x:%02x:%02x' % (app_id | (pipe<<3), batch >> 8, batch & 0xFF, pkt_num >> 8, pkt_num & 0xFF)
                        p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac)
                        pkt_lst.append(p_exp)

                verify_multiple_packets(self, swports[0], pkt_lst, pkt_len, tmo=1)

            print("Ensuring there are no extra packets")
            sys.stdout.flush()
            verify_no_other_packets(self)

            # One last verify of all the counters
            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(sess_hdl, dev_tgt_[pipe], app)
                    batch_cntrs[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(sess_hdl, dev_tgt_[pipe], app)
                    packet_cntrs[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, dev_tgt_[pipe], app)
            for pipe in range(pipe_list_len):
                for app in range(8):
                    if trigger_cntrs[pipe][app] != trigger_cntrs_exp[pipe][app] or batch_cntrs[pipe][app] != batch_cntrs_exp[pipe][app] or packet_cntrs[pipe][app] == packet_cntrs_exp[pipe][app]:
                        print("Pipe", pipe, "App", app, "Actual Triggers", hex(trigger_cntrs[pipe][app]), "Expected Triggers", hex(trigger_cntrs_exp[pipe][app]))
                        print("Pipe", pipe, "App", app, "Actual Batches ", hex(batch_cntrs[pipe][app]), "Expected Batches ", hex(batch_cntrs_exp[pipe][app]))
                        print("Pipe", pipe, "App", app, "Actual Packets ", hex(packet_cntrs[pipe][app]), "Expected Packets ", hex(packet_cntrs_exp[pipe][app]))
                        sys.stdout.flush()
                    self.assertTrue(trigger_cntrs[pipe][app] == trigger_cntrs_exp[pipe][app])
                    self.assertTrue(batch_cntrs[pipe][app] == batch_cntrs_exp[pipe][app])
                    self.assertTrue(packet_cntrs[pipe][app] == packet_cntrs_exp[pipe][app])

        finally:
            for pipe in range(pipe_list_len):
                for app in range(8):
                    self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_[pipe], app)
                    app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[pipe], app)
                    self.assertFalse(app_enabled)
            for pipe in range(pipe_list_len):
                for port in range(68,72):
                    self.conn_mgr.pktgen_disable(sess_hdl, dev_id, make_port(pipe, port))
                    enabled = self.conn_mgr.pktgen_enable_state_get(sess_hdl, dev_id, make_port(pipe, port))
                    self.assertFalse(enabled)
            for x in timer_verify_handles:
                self.client.pg_verify_timer_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(x))
            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))

            sess_hdl = self.conn_mgr.client_cleanup(sess_hdl)

class TestTimerPeriodic(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        shdl = sess_hdl
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_ = []
        for pipe in range(pipe_list_len):
            dev_tgt_.append(DevTarget_t(dev_id, hex_to_i16(pipe)))

        p_count = 3
        batch_count = 2
        pktlen = 85

        port_tbl_handles = []
        timer_verify_handles0 = []
        timer_verify_handles2 = []

        trigger_cntrs = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        trigger_cntrs_base= [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs_base  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs_base = [ [0]*8, [0]*8, [0]*8, [0]*8 ]

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64

        try:
            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs_base[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(sess_hdl, dev_tgt_[pipe], app)
                    batch_cntrs_base[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(sess_hdl, dev_tgt_[pipe], app)
                    packet_cntrs_base[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, dev_tgt_[pipe], app)

            self.client.port_tbl_set_property(sess_hdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);
            # Before enabling pktgen, increase the qdepth
            self.dataplane.qlen = 1024

            # Enable packet gen
            for pipe in pipe_list:
                self.conn_mgr.pktgen_enable(sess_hdl, dev_id, make_port(pipe,68))
                enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, 68))
                self.assertTrue(enabled)

            # All generated packets will go out the last test port if they are
            # in pipe 0 and the first test port if they are in pipe 2
            egr_port0 = swports[-1]
            egr_port2 = swports[0]
            per_pipe_base_ports = []
            per_pipe_base_ports = [make_port(x,x+62) for x in pipe_list]
            for pipe in pipe_list:
                if 0 == pipe:
                    egr_port = egr_port0
                else:
                    egr_port = egr_port2
                for p in range(p_count):
                    port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_[pipe], per_pipe_base_ports[pipe]+p, egr_port, 0, 1)
                    pg_verify_timer_add(self, timer_verify_handles0, shdl, dev_tgt, per_pipe_base_ports[pipe]+p)

            self.client.pg_verify_timer_set_default_action_timer_nok(sess_hdl, dev_tgt);
            self.client.pg_verify_port_down_set_default_action_port_down_nok(sess_hdl, dev_tgt);
            self.client.pg_verify_recirc_set_default_action_recirc_nok(sess_hdl, dev_tgt);

            app_cfgs = []
            for pipe in pipe_list:
                if pipe == 0:
                    scale = 1
                else:
                    scale = 2
                x = port_to_pipe_local_port(per_pipe_base_ports[pipe])
                cfg = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.TIMER_PERIODIC,
                                     batch_count=batch_count-1,
                                     pkt_count=p_count-1,
                                     timer=500000000*scale,
                                     ibg=0,
                                     ipg=0,
                                     src_port=x,
                                     src_port_inc=1,
                                     buffer_offset=144,
                                     length=pktlen-6)
                app_cfgs.append(cfg)



            p0 = simple_eth_packet(pktlen=pktlen, eth_src='00:11:22:33:44:55')
            p2 = simple_eth_packet(pktlen=pktlen, eth_src='00:AA:BB:CC:DD:EE')
            if 0 in pipe_list:
                self.conn_mgr.pktgen_cfg_app(sess_hdl, dev_tgt_[0], 7, app_cfgs[0])
                cfg = self.conn_mgr.pktgen_cfg_app_get(sess_hdl, dev_tgt_[0], 7)
                self.assertEqual(cfg, app_cfgs[0])
                self.conn_mgr.pktgen_write_pkt_buffer(sess_hdl, dev_tgt_[0], 144, pktlen-6, bytes(p0)[6:])
                buff = self.conn_mgr.pktgen_read_pkt_buffer(sess_hdl, dev_tgt_[0], 144, pktlen-6)
                self.assertEqual(buff, bytes(p0)[6:])
                self.conn_mgr.pktgen_app_enable(sess_hdl, dev_tgt_[0], 7)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[0], 7)
                self.assertTrue(app_enabled)
            if 2 in pipe_list:
                self.conn_mgr.pktgen_cfg_app(sess_hdl, dev_tgt_[2], 6, app_cfgs[2])
                cfg = self.conn_mgr.pktgen_cfg_app_get(sess_hdl, dev_tgt_[2], 6)
                self.assertEqual(cfg, app_cfgs[2])
                self.conn_mgr.pktgen_write_pkt_buffer(sess_hdl, dev_tgt_[2], 144, pktlen-6, bytes(p2)[6:])
                buff = self.conn_mgr.pktgen_read_pkt_buffer(sess_hdl, dev_tgt_[2], 144, pktlen-6)
                self.assertEqual(buff, bytes(p2)[6:])
                self.conn_mgr.pktgen_app_enable(sess_hdl, dev_tgt_[2], 6)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[2], 6)
                self.assertTrue(app_enabled)

            print("Pushing configuration")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)

            # Wait some time for a few triggers to fire
            print("Waiting for trigger to fire a few times")
            sys.stdout.flush()
            time.sleep(6)

            print("Disabling Apps")
            sys.stdout.flush()
            if 0 in pipe_list:
                self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_[0], 7)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[0], 7)
            if 2 in pipe_list:
                self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_[2], 6)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[2], 6)
            self.assertFalse(app_enabled)
            self.conn_mgr.complete_operations(sess_hdl)
            print("Apps disabled, waiting a few seconds for packets to finish processing")
            sys.stdout.flush()
            time.sleep(8)


            # Read the trigger counters to determine how many packets need to be received.
            if 0 in pipe_list:
                trigger_cntrs[0][7] = self.conn_mgr.pktgen_get_trigger_counter(sess_hdl, dev_tgt_[0], 7)
                batch_cntrs[0][7] = self.conn_mgr.pktgen_get_batch_counter(sess_hdl, dev_tgt_[0], 7)
                packet_cntrs[0][7] = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, dev_tgt_[0], 7)
                p0_triggers = trigger_cntrs[0][7] - trigger_cntrs_base[0][7]
                p0_batches  = batch_cntrs[0][7] - batch_cntrs_base[0][7]
                p0_packets  = packet_cntrs[0][7] - packet_cntrs_base[0][7]
                print("Trigger count in pipe0:", p0_triggers)
                print("Batch counter in pipe0:", p0_batches, "Ideal is", batch_count*p0_triggers, "with", batch_count, "batches per trigger")
                print("Packet counter in pipe0:",p0_packets, "Ideal is", p_count*batch_count*p0_triggers, "with", p_count, "packets per batch")
                sys.stdout.flush()
            if 2 in pipe_list:
                trigger_cntrs[2][6] = self.conn_mgr.pktgen_get_trigger_counter(sess_hdl, dev_tgt_[2], 6)
                batch_cntrs[2][6] = self.conn_mgr.pktgen_get_batch_counter(sess_hdl, dev_tgt_[2], 6)
                packet_cntrs[2][6] = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, dev_tgt_[2], 6)
                p2_triggers = trigger_cntrs[2][6] - trigger_cntrs_base[2][6]
                p2_batches  = batch_cntrs[2][6] - batch_cntrs_base[2][6]
                p2_packets  = packet_cntrs[2][6] - packet_cntrs_base[2][6]
                print("Trigger count in pipe2:", p2_triggers)
                print("Batch counter in pipe2:", p2_batches, "Ideal is", batch_count*p2_triggers, "with", batch_count, "batches per trigger")
                print("Packet counter in pipe2:",p2_packets, "Ideal is", p_count*batch_count*p2_triggers, "with", p_count, "packets per batch")
                sys.stdout.flush()
            if 0 in pipe_list and 2 in pipe_list:
                print("Trigger Counts", p0_triggers, p2_triggers)
                sys.stdout.flush()
                self.assertTrue(p0_triggers > p2_triggers)

            #print("Expecting (", len(timer_verify_handles), " times)", format_packet(p))

            if 0 in pipe_list:
                self.assertGreater(p0_triggers, 0)
                self.assertGreaterEqual(p0_batches, batch_count*(p0_triggers-1))
                self.assertGreaterEqual(p0_packets, p_count*(p0_batches-1))
                pkt_lst = []
                pkt_len = [pktlen] * p0_packets

                p0_pkt_cnt = 0
                for _ in range(p0_triggers):
                    for batch in range(batch_count):
                        for pkt_num in range(p_count):
                            if p0_pkt_cnt == p0_packets: continue
                            app_id = 7
                            dmac='%02x:00:%02x:%02x:%02x:%02x' % (app_id, batch >> 8, batch & 0xFF, pkt_num >> 8, pkt_num & 0xFF)
                            p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac, eth_src='00:11:22:33:44:55')
                            pkt_lst.append(p_exp)
                            p0_pkt_cnt += 1

                verify_multiple_packets(self, egr_port0, pkt_lst, pkt_len, tmo=2)

            if 2 in pipe_list:
                self.assertGreater(p2_triggers, 0)
                self.assertGreaterEqual(p2_batches, batch_count*(p2_triggers-1))
                self.assertGreaterEqual(p2_packets, p_count*(p2_batches-1))
                pkt_lst = []
                pkt_len = [pktlen] * p2_packets

                p2_pkt_cnt = 0
                for _ in range(p2_triggers):
                    for batch in range(batch_count):
                        for pkt_num in range(p_count):
                            if p2_pkt_cnt == p2_packets: continue
                            app_id = 6
                            dmac='%02x:00:%02x:%02x:%02x:%02x' % (app_id | (2 << 3), batch >> 8, batch & 0xFF, pkt_num >> 8, pkt_num & 0xFF)
                            p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac, eth_src='00:AA:BB:CC:DD:EE')
                            pkt_lst.append(p_exp)
                            p2_pkt_cnt += 1

                verify_multiple_packets(self, egr_port2, pkt_lst, pkt_len, tmo=2)

            (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=0.5 )
            if rcv_port != None:
                print("Extra Rx: port", rcv_port, "Packet", format_packet(rcv_pkt))
                sys.stdout.flush()
                test.assertTrue(rcv_pkt == None, "Receive extra packet")


        finally:
            for aid in range(8):
                for pipe in range(pipe_list_len):
                    self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_[pipe], aid)
                    app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[pipe], aid)
                    self.assertFalse(app_enabled)
            for pipe in range(pipe_list_len):
                for port in range(68,72):
                    self.conn_mgr.pktgen_disable(sess_hdl, dev_id, make_port(pipe, port))
                    enabled = self.conn_mgr.pktgen_enable_state_get(sess_hdl, dev_id, make_port(pipe, port))
                    self.assertFalse(enabled)
            for x in timer_verify_handles0:
                self.client.pg_verify_timer_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(x))
            for x in timer_verify_handles2:
                self.client.pg_verify_timer_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(x))
            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))

            sess_hdl = self.conn_mgr.client_cleanup(sess_hdl)


class TestPattern(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        shdl = sess_hdl
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_0 = DevTarget_t(dev_id, hex_to_i16(0))
        dev_tgt_1 = DevTarget_t(dev_id, hex_to_i16(1))
        dev_tgt_2 = DevTarget_t(dev_id, hex_to_i16(2))
        dev_tgt_3 = DevTarget_t(dev_id, hex_to_i16(3))

        port_tbl_handles = []
        recirc_verify_handles = []

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64
        try:
            self.client.port_tbl_set_property(sess_hdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);

            self.conn_mgr.pktgen_enable(sess_hdl, dev_id, 196) # Pipe1, Port 68, Chan 0
            enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, 196)
            self.assertTrue(enabled)

            # Packet comes in port 0, sent to recirc port 196
            iport = swports[0]
            ipipe = port_to_pipe(iport)
            port_tbl_add(self, port_tbl_handles, sess_hdl, DevTarget_t(dev_id, ipipe), iport, 196, 1, 0)

            # Packet comes back from 196 and goes out port 1
            port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 196, swports[1], 1, 0)

            # Generated packets from 130 go out port 2
            port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_1, 130, swports[2], 0, 1)


            self.client.pg_verify_timer_set_default_action_timer_nok(sess_hdl, dev_tgt)
            self.client.pg_verify_port_down_set_default_action_port_down_nok(sess_hdl, dev_tgt)
            self.client.pg_verify_recirc_set_default_action_recirc_nok(sess_hdl, dev_tgt)

            pg_verify_recirc_add(self, recirc_verify_handles, sess_hdl, dev_tgt, 130)

            batches = 4
            pkt_per_batch = 3

            ibg = 0
            ipg = hex_to_i32(0xFFFF)
            x = 2

            pda1 = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.RECIRC_PATTERN,
                                  batch_count=batches,
                                  pkt_count=pkt_per_batch,
                                  pattern_key=hex_to_i32(0x12345678),
                                  pattern_msk=hex_to_i32(0xF0F0F0F0),
                                  timer=hex_to_i32(0),
                                  ibg=ibg,
                                  ipg=ipg,
                                  src_port=x,
                                  src_port_inc=0,
                                  buffer_offset=15*1024,
                                  length=300-6)


            # Trigger packets
            p0 = simple_eth_packet(pktlen=100, eth_dst='80:01:02:03:AA:AA')
            p1 = simple_eth_packet(pktlen=100, eth_dst='12:34:56:78:BB:BB')
            p2 = simple_eth_packet(pktlen=100, eth_dst='01:3F:51:7A:CC:CC')
            p3 = simple_eth_packet(pktlen=100, eth_dst='10:3F:51:7A:DD:DD')

            # Generated packet
            p = simple_tcp_packet(pktlen=300, eth_src='00:22:22:22:22:22')

            self.conn_mgr.pktgen_cfg_app(sess_hdl, dev_tgt_1, 3, pda1)
            cfg = self.conn_mgr.pktgen_cfg_app_get(sess_hdl, dev_tgt_1, 3)
            self.assertEqual(cfg, pda1)

            self.conn_mgr.pktgen_write_pkt_buffer(sess_hdl, dev_tgt_1, 15*1024, 300-6, bytes(p)[6:])
            buff = self.conn_mgr.pktgen_read_pkt_buffer(sess_hdl, dev_tgt_1, 15*1024, 300-6)
            self.assertEqual(buff, bytes(p)[6:])

            self.conn_mgr.pktgen_enable_recirc_pattern_matching(sess_hdl, dev_id, 196) # Pipe1, Port 68, Chan 0
            recirc_en = self.conn_mgr.pktgen_recirc_pattern_matching_state_get(sess_hdl, dev_id, 196)
            self.assertTrue(recirc_en)

            self.conn_mgr.pktgen_app_enable(sess_hdl, dev_tgt_1, 3)
            app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_1, 3)
            self.assertTrue(app_enabled)

            self.conn_mgr.complete_operations(sess_hdl)

            # Packet 0 should not trigger an event.
            print("Sending pattern 0")
            sys.stdout.flush()
            send_packet(self, iport, p0)

            verify_packets(self, p0, [swports[1]])

            # Packet 1 should trigger an event.
            print("Sending pattern 1 (will trigger event)")
            sys.stdout.flush()
            send_packet(self, iport, p1)
            verify_packet(self, p1, swports[1])
            pkt_lst = []
            pkt_len = [300] * (1+batches) * (1+pkt_per_batch)
            for bid in range(1+batches):
                for pid in range(1+pkt_per_batch):
                    dmac = '%02x:34:%02x:%02x:%02x:%02x' % ((1<<3)|3, (0x5678 + bid) >> 8, (0x5678 + bid) & 0xFF, pid >> 8, pid & 0xFF)
                    p_exp = simple_tcp_packet(pktlen=300, eth_dst=dmac, eth_src='00:22:22:22:22:22')
                    pkt_lst.append(p_exp)
            verify_multiple_packets(self, swports[2], pkt_lst, pkt_len, tmo=120)

            # Packet 2 should not trigger an event.
            print("Sending pattern 2")
            sys.stdout.flush()
            send_packet(self, iport, p2)

            verify_packets(self, p2, [swports[1]])

            # Packet 3 should trigger an event.
            print("Sending pattern 3 (will trigger event)")
            sys.stdout.flush()
            send_packet(self, iport, p3)
            verify_packet(self, p3, swports[1])
            pkt_lst = []
            for bid in range(1+batches):
                for pid in range(1+pkt_per_batch):
                    dmac = '%02x:3F:%02x:%02x:%02x:%02x' % ((1<<3)|3, (0x517A + bid) >> 8, (0x517A + bid) & 0xFF, pid >> 8, pid & 0xFF)
                    p_exp = simple_tcp_packet(pktlen=300, eth_dst=dmac, eth_src='00:22:22:22:22:22')
                    pkt_lst.append(p_exp)

            verify_multiple_packets(self, swports[2], pkt_lst, pkt_len, tmo=120)

            self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_1, 3)
            app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_1, 3)
            self.assertFalse(app_enabled)
            self.conn_mgr.pktgen_disable_recirc_pattern_matching(sess_hdl, dev_id, 196) # Pipe1, Port 68, Chan 0
            recirc_en = self.conn_mgr.pktgen_recirc_pattern_matching_state_get(sess_hdl, dev_id, 196)
            self.assertFalse(recirc_en)
            self.conn_mgr.pktgen_app_enable(sess_hdl, dev_tgt_1, 3)
            app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_1, 3)
            self.assertTrue(app_enabled)

            self.conn_mgr.complete_operations(sess_hdl)

            # No packets should trigger an event now.
            print("Pkt-Gen disabled, sending all patterns")
            sys.stdout.flush()
            send_packet(self, iport, p0)
            verify_packet(self, p0, swports[1])
            send_packet(self, iport, p1)
            verify_packet(self, p1, swports[1])
            send_packet(self, iport, p2)
            verify_packet(self, p2, swports[1])
            send_packet(self, iport, p3)
            verify_packets(self, p3, [swports[1]])


        finally:
            for aid in range(8):
                if 0 in pipe_list:
                    self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_0, aid)
                    app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_0, aid)
                if 1 in pipe_list:
                    self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_1, aid)
                    app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_1, aid)
                if 2 in pipe_list:
                    self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_2, aid)
                    app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_2, aid)
                if 3 in pipe_list:
                    self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_3, aid)
                    app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_3, aid)
                self.assertFalse(app_enabled)
                self.conn_mgr.pktgen_disable_recirc_pattern_matching(sess_hdl, dev_id, 196)
                recirc_en = self.conn_mgr.pktgen_recirc_pattern_matching_state_get(sess_hdl, dev_id, 196)
                self.assertFalse(recirc_en)
            for pipe in range(pipe_list_len):
                for port in range(68,72):
                    self.conn_mgr.pktgen_disable(sess_hdl, dev_id, make_port(pipe, port))
                    enabled = self.conn_mgr.pktgen_enable_state_get(sess_hdl, dev_id, make_port(pipe, port))
                    self.assertFalse(enabled)
            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))
            for eh in recirc_verify_handles:
                self.client.pg_verify_recirc_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))

            sess_hdl = self.conn_mgr.client_cleanup(sess_hdl)



class TestTimerAllPipe(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        shdl = sess_hdl
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_ = []
        for pipe in range(pipe_list_len):
            dev_tgt_.append(DevTarget_t(dev_id, hex_to_i16(pipe)))

        port_tbl_handles = []
        timer_verify_handles = []

        trigger_cntrs = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        trigger_cntrs_exp = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs_exp   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs_exp  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]

        pktlen = 1500

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64
        try:
            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(sess_hdl, dev_tgt_[pipe], app)
                    batch_cntrs[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(sess_hdl, dev_tgt_[pipe], app)
                    packet_cntrs[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, dev_tgt_[pipe], app)
                    trigger_cntrs_exp[pipe][app] = trigger_cntrs[pipe][app]
                    batch_cntrs_exp[pipe][app]   = batch_cntrs[pipe][app]
                    packet_cntrs_exp[pipe][app]  = packet_cntrs[pipe][app]

            self.client.port_tbl_set_property(sess_hdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);
            #self.client.pg_verify_timer_set_property(sess_hdl, dev_id, 1, 0, 0);

            for pipe in pipe_list:
                self.conn_mgr.pktgen_enable(sess_hdl, dev_id, make_port(pipe,68))
                enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, 68))
                self.assertTrue(enabled)

            # All generated packets will go to the first test port
            eport = swports[0]
            for pipe in range(pipe_list_len):
                iport = make_port(pipe,0)
                port_tbl_add(self, port_tbl_handles, sess_hdl, dev_tgt_[pipe], iport, eport, 0, 1)


            self.client.pg_verify_timer_set_default_action_timer_nok(sess_hdl, dev_tgt);
            self.client.pg_verify_port_down_set_default_action_port_down_nok(sess_hdl, dev_tgt);
            self.client.pg_verify_recirc_set_default_action_recirc_nok(sess_hdl, dev_tgt);

            for pipe in range(pipe_list_len):
                pg_verify_timer_add(self, timer_verify_handles, shdl, dev_tgt, make_port(pipe,0))
            pda0 = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.TIMER_ONE_SHOT,
                                   batch_count=0,
                                   pkt_count=0,
                                   timer=100,
                                   ibg=1,
                                   ipg=0,
                                   src_port=0,
                                   src_port_inc=0,
                                   buffer_offset=0,
                                   length=pktlen-6)

            for i in range(pipe_list_len):
                trigger_cntrs_exp[i][7] += 1
                batch_cntrs_exp[i][7] += 1
                packet_cntrs_exp[i][7] += 1
            self.conn_mgr.pktgen_cfg_app(sess_hdl, dev_tgt, 7, pda0)
            cfg = self.conn_mgr.pktgen_cfg_app_get(sess_hdl, dev_tgt, 7)
            self.assertEqual(cfg, pda0)

            p = simple_eth_packet(pktlen=pktlen)
            self.conn_mgr.pktgen_write_pkt_buffer(sess_hdl, dev_tgt, 0, pktlen-6, bytes(p)[6:])
            buff = self.conn_mgr.pktgen_read_pkt_buffer(sess_hdl, dev_tgt, 0, pktlen-6)
            self.assertEqual(buff, bytes(p)[6:])


            self.conn_mgr.pktgen_app_enable(sess_hdl, dev_tgt, 7)
            app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt, 7)
            self.assertTrue(app_enabled)
            self.conn_mgr.complete_operations(sess_hdl)
            # Wait for the timers to go off.  Should be almost immediate but
            # wait anyways to ensure they only go off once.
            time.sleep(10)

            pkt_lst = []
            pkt_len = [pktlen] * 4
            for pipe in range(pipe_list_len):
                app_id = (7 << 0) | (pipe << 3)
                dmac='%02x:00:%02x:%02x:%02x:%02x' % (app_id, 0, 0, 0, 0)
                p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac)
                pkt_lst.append(p_exp)

            verify_multiple_packets(self, eport, pkt_lst, pkt_len, tmo=1)


            verify_no_other_packets(self)

            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(sess_hdl, dev_tgt_[pipe], app)
                    batch_cntrs[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(sess_hdl, dev_tgt_[pipe], app)
                    packet_cntrs[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, dev_tgt_[pipe], app)

            for pipe in range(pipe_list_len):
                for app in range(8):
                    self.assertEqual(trigger_cntrs[pipe][app], trigger_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, trigger_cntrs[pipe][app], trigger_cntrs_exp[pipe][app]))
                    self.assertEqual(batch_cntrs[pipe][app], batch_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, batch_cntrs[pipe][app], batch_cntrs_exp[pipe][app]))
                    self.assertEqual(packet_cntrs[pipe][app], packet_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, packet_cntrs[pipe][app], packet_cntrs_exp[pipe][app]))

        finally:
            for pipe in range(pipe_list_len):
                self.conn_mgr.pktgen_app_disable(sess_hdl, dev_tgt_[pipe], 7)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(sess_hdl, dev_tgt_[pipe], 7)
                self.assertFalse(app_enabled)
            for pipe in range(pipe_list_len):
                for port in range(68,72):
                    self.conn_mgr.pktgen_disable(sess_hdl, dev_id, make_port(pipe, port))
                    enabled = self.conn_mgr.pktgen_enable_state_get(sess_hdl, dev_id, make_port(pipe, port))
                    self.assertFalse(enabled)
            for x in timer_verify_handles:
                self.client.pg_verify_timer_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(x))
            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(sess_hdl), dev_id, hex_to_i32(eh))

            sess_hdl = self.conn_mgr.client_cleanup(sess_hdl)




class TestBatch(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        shdl_extra = self.conn_mgr.client_init()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_ = []
        for pipe in range(pipe_list_len):
            dev_tgt_.append(DevTarget_t(dev_id, hex_to_i16(pipe)))

        port_tbl_handles = []
        timer_verify_handles = []

        trigger_cntrs = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        trigger_cntrs_exp = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs_exp   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs_exp  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]

        pktlen = 1500

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64
        try:
            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(shdl, dev_tgt_[pipe], app)
                    batch_cntrs[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(shdl, dev_tgt_[pipe], app)
                    packet_cntrs[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(shdl, dev_tgt_[pipe], app)
                    trigger_cntrs_exp[pipe][app] = trigger_cntrs[pipe][app]
                    batch_cntrs_exp[pipe][app]   = batch_cntrs[pipe][app]
                    packet_cntrs_exp[pipe][app]  = packet_cntrs[pipe][app]

            self.client.port_tbl_set_property(shdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);

            for pipe in pipe_list:
                self.conn_mgr.pktgen_enable(shdl, dev_id, make_port(pipe,68))
                enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, 68))
                self.assertTrue(enabled)

            # All generated packets will go to the first port
            eport = swports[0]
            for pipe in range(pipe_list_len):
                iport = make_port(pipe,0)
                port_tbl_add(self, port_tbl_handles, shdl_extra, dev_tgt_[pipe], iport, eport, 0, 1)

            self.client.pg_verify_timer_set_default_action_timer_nok(shdl, dev_tgt);
            self.client.pg_verify_port_down_set_default_action_port_down_nok(shdl, dev_tgt);
            self.client.pg_verify_recirc_set_default_action_recirc_nok(shdl, dev_tgt);

            for pipe in range(pipe_list_len):
                pg_verify_timer_add(self, timer_verify_handles, shdl, dev_tgt, make_port(pipe,0))
            pda0 = PktGenAppCfg_t( trigger_type=PktGenTriggerType_t.TIMER_ONE_SHOT,
                                   batch_count=0,
                                   pkt_count=0,
                                   timer=100,
                                   ibg=1,
                                   ipg=0,
                                   src_port=0,
                                   src_port_inc=0,
                                   buffer_offset=0,
                                   length=pktlen-6 )

            # Start a batch
            self.conn_mgr.begin_batch( shdl )
            for i in range(pipe_list_len):
                trigger_cntrs_exp[i][7] += 1
                batch_cntrs_exp[i][7] += 1
                packet_cntrs_exp[i][7] += 1
            self.conn_mgr.pktgen_cfg_app(shdl, dev_tgt, 7, pda0)
            cfg = self.conn_mgr.pktgen_cfg_app_get(shdl, dev_tgt, 7)
            self.assertEqual(cfg, pda0)

            # Another session should not be able to access pkt gen now.
            try:
                self.conn_mgr.pktgen_cfg_app(shdl_extra, dev_tgt, 0, pda0)
                self.assertTrue(0)
            except InvalidPktGenOperation as e:
                pass

            p = simple_eth_packet(pktlen=pktlen)
            self.conn_mgr.pktgen_write_pkt_buffer(shdl, dev_tgt, 0, pktlen-6, bytes(p)[6:])
            buff = self.conn_mgr.pktgen_read_pkt_buffer(shdl, dev_tgt, 0, pktlen-6)
            self.assertEqual(buff, bytes(p)[6:])

            self.conn_mgr.pktgen_app_enable(shdl, dev_tgt, 7)
            app_enabled = self.conn_mgr.pktgen_app_enable_state_get(shdl, dev_tgt, 7)
            self.assertTrue(app_enabled)

            # Wait for 10 seconds to ensure that the timers do NOT go off since
            # the batch has not been pushed yet.
            time.sleep(10)
            verify_no_other_packets(self)

            # Close the batch and wait for the timers to go off.
            self.conn_mgr.end_batch(shdl, True)
            time.sleep(3)

            pkt_lst = []
            pkt_len = [pktlen] * 4
            for pipe in range(pipe_list_len):
                app_id = (7 << 0) | (pipe << 3)
                print("APP id is ", app_id)
                dmac='%02x:00:%02x:%02x:%02x:%02x' % (app_id, 0, 0, 0, 0)
                p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac)
                pkt_lst.append(p_exp)
            sys.stdout.flush()
            verify_multiple_packets(self, eport, pkt_lst, pkt_len, tmo=1)
            verify_no_other_packets(self)
            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(shdl, dev_tgt_[pipe], app)
                    batch_cntrs[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(shdl, dev_tgt_[pipe], app)
                    packet_cntrs[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(shdl, dev_tgt_[pipe], app)
            for pipe in range(pipe_list_len):
                for app in range(8):
                    self.assertEqual(trigger_cntrs[pipe][app], trigger_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, trigger_cntrs[pipe][app], trigger_cntrs_exp[pipe][app]))
                    self.assertEqual(batch_cntrs[pipe][app], batch_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, batch_cntrs[pipe][app], batch_cntrs_exp[pipe][app]))
                    self.assertEqual(packet_cntrs[pipe][app], packet_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, packet_cntrs[pipe][app], packet_cntrs_exp[pipe][app]))

        finally:
            self.conn_mgr.pktgen_app_disable(shdl, dev_tgt, 7)
            app_enabled = self.conn_mgr.pktgen_app_enable_state_get(shdl, dev_tgt, 7)
            self.assertFalse(app_enabled)
            for pipe in range(pipe_list_len):
                for port in range(68,72):
                    self.conn_mgr.pktgen_disable(shdl, dev_id, make_port(pipe, port))
                    enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, 68))
                    self.assertFalse(enabled)
            for x in timer_verify_handles:
                self.client.pg_verify_timer_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(x))
            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(eh))

            self.conn_mgr.client_cleanup(shdl)
            self.conn_mgr.client_cleanup(shdl_extra)





class TestTxn(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        shdl_extra = self.conn_mgr.client_init()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        dev_tgt_ = []
        for pipe in range(pipe_list_len):
            dev_tgt_.append(DevTarget_t(dev_id, hex_to_i16(pipe)))

        port_tbl_handles = []
        timer_verify_handles = []

        trigger_cntrs = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        trigger_cntrs_exp = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        batch_cntrs_exp   = [ [0]*8, [0]*8, [0]*8, [0]*8 ]
        packet_cntrs_exp  = [ [0]*8, [0]*8, [0]*8, [0]*8 ]

        pktlen = 765

        speed_10g = 2
        speed_25g = 4
        speed_40g = 8
        speed_40g_nb = 16
        speed_50g = 32
        speed_100g = 64
        try:
            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(shdl, dev_tgt_[pipe], app)
                    batch_cntrs[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(shdl, dev_tgt_[pipe], app)
                    packet_cntrs[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(shdl, dev_tgt_[pipe], app)
                    trigger_cntrs_exp[pipe][app] = trigger_cntrs[pipe][app]
                    batch_cntrs_exp[pipe][app]   = batch_cntrs[pipe][app]
                    packet_cntrs_exp[pipe][app]  = packet_cntrs[pipe][app]

            self.client.port_tbl_set_property(shdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE, 0);

            for pipe in pipe_list:
                self.conn_mgr.pktgen_enable(shdl, dev_id, make_port(pipe,68))
                enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, 68))
                self.assertTrue(enabled)

            # All generated packets will go to the first test port
            eport = swports[0]
            for pipe in range(pipe_list_len):
                iport = make_port(pipe,0)
                port_tbl_add(self, port_tbl_handles, shdl, dev_tgt_[pipe], iport, eport, 0, 1)

            self.client.pg_verify_timer_set_default_action_timer_nok(shdl, dev_tgt);
            self.client.pg_verify_port_down_set_default_action_port_down_nok(shdl, dev_tgt);
            self.client.pg_verify_recirc_set_default_action_recirc_nok(shdl, dev_tgt);

            for pipe in range(pipe_list_len):
                pg_verify_timer_add(self, timer_verify_handles, shdl, dev_tgt, make_port(pipe,0))
            pda0 = PktGenAppCfg_t( trigger_type=PktGenTriggerType_t.TIMER_ONE_SHOT,
                                   batch_count=0,
                                   pkt_count=1,
                                   timer=100,
                                   ibg=1,
                                   ipg=0,
                                   src_port=0,
                                   src_port_inc=0,
                                   buffer_offset=0,
                                   length=pktlen-6 )
            for i in range(pipe_list_len):
                trigger_cntrs_exp[i][7] += 1
                batch_cntrs_exp[i][7] += 1
                packet_cntrs_exp[i][7] += 2

            # Start a transaction
            self.conn_mgr.begin_txn( shdl, False )

            # Configure the app
            self.conn_mgr.pktgen_cfg_app(shdl, dev_tgt, 7, pda0)
            cfg = self.conn_mgr.pktgen_cfg_app_get(shdl, dev_tgt, 7)
            self.assertEqual(cfg, pda0)

            # Another session should not be able to access pkt gen now.
            try:
                self.conn_mgr.pktgen_cfg_app(shdl_extra, dev_tgt, 0, pda0)
                self.assertTrue(0)
            except InvalidPktGenOperation as e:
                pass

            p = simple_eth_packet(pktlen=pktlen)
            self.conn_mgr.pktgen_write_pkt_buffer(shdl, dev_tgt, 0, pktlen-6, bytes(p)[6:])
            buff = self.conn_mgr.pktgen_read_pkt_buffer(shdl, dev_tgt, 0, pktlen-6)
            self.assertEqual(buff, bytes(p)[6:])

            self.conn_mgr.commit_txn( shdl, True )

            # Start another transaction
            self.conn_mgr.begin_txn( shdl_extra, False )
            if 0 in pipe_list:
                p = simple_tcp_packet(ip_src='0.0.0.0', pktlen=pktlen)
                self.conn_mgr.pktgen_write_pkt_buffer(shdl_extra, dev_tgt_[0], 0, pktlen-6, bytes(p)[6:])
                buff = self.conn_mgr.pktgen_read_pkt_buffer(shdl_extra, dev_tgt_[0], 0, pktlen-6)
                self.assertEqual(buff, bytes(p)[6:])
            if 1 in pipe_list:
                p = simple_tcp_packet(ip_src='1.1.1.1', pktlen=pktlen)
                self.conn_mgr.pktgen_write_pkt_buffer(shdl_extra, dev_tgt_[1], 0, pktlen-6, bytes(p)[6:])
                buff = self.conn_mgr.pktgen_read_pkt_buffer(shdl_extra, dev_tgt_[1], 0, pktlen-6)
                self.assertEqual(buff, bytes(p)[6:])
            if 2 in pipe_list:
                p = simple_tcp_packet(ip_src='2.2.2.2', pktlen=pktlen)
                self.conn_mgr.pktgen_write_pkt_buffer(shdl_extra, dev_tgt_[2], 0, pktlen-6, bytes(p)[6:])
                buff = self.conn_mgr.pktgen_read_pkt_buffer(shdl_extra, dev_tgt_[2], 0, pktlen-6)
                self.assertEqual(buff, bytes(p)[6:])
            if 3 in pipe_list:
                p = simple_tcp_packet(ip_src='3.3.3.3', pktlen=pktlen)
                self.conn_mgr.pktgen_write_pkt_buffer(shdl_extra, dev_tgt_[3], 0, pktlen-6, bytes(p)[6:])
                buff = self.conn_mgr.pktgen_read_pkt_buffer(shdl_extra, dev_tgt_[3], 0, pktlen-6)
                self.assertEqual(buff, bytes(p)[6:])
            p = simple_tcp_packet(ip_src='4.4.4.4', pktlen=pktlen)
            self.conn_mgr.pktgen_write_pkt_buffer(shdl_extra, dev_tgt, 0, pktlen-6, bytes(p)[6:])
            buff = self.conn_mgr.pktgen_read_pkt_buffer(shdl_extra, dev_tgt, 0, pktlen-6)
            self.assertEqual(buff, bytes(p)[6:])
            pda = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.TIMER_ONE_SHOT,
                                 batch_count=100,
                                 pkt_count=100,
                                 timer=100,
                                 ibg=1,
                                 ipg=0,
                                 src_port=0,
                                 src_port_inc=0,
                                 buffer_offset=0,
                                 length=pktlen-6)
            self.conn_mgr.pktgen_cfg_app(shdl_extra, dev_tgt, 0, pda)
            cfg = self.conn_mgr.pktgen_cfg_app_get(shdl_extra, dev_tgt, 0)
            self.assertEqual(cfg, pda)
            if 1 in pipe_list:
                self.conn_mgr.pktgen_app_enable(shdl_extra, dev_tgt_[1], 7)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(shdl_extra, dev_tgt_[1], 7)
                self.assertTrue(app_enabled)
            if 3 in pipe_list:
                self.conn_mgr.pktgen_app_enable(shdl_extra, dev_tgt_[3], 7)
                app_enabled = self.conn_mgr.pktgen_app_enable_state_get(shdl_extra, dev_tgt_[3], 7)
                self.assertTrue(app_enabled)
            self.conn_mgr.abort_txn( shdl_extra )

            verify_no_other_packets(self)

            self.conn_mgr.begin_txn(shdl_extra, False)
            self.conn_mgr.pktgen_app_enable(shdl_extra, dev_tgt, 7)
            app_enabled = self.conn_mgr.pktgen_app_enable_state_get(shdl_extra, dev_tgt, 7)
            self.assertTrue(app_enabled)
            self.conn_mgr.commit_txn(shdl_extra, True)

            time.sleep(3)

            pkt_lst = []
            pkt_len = [pktlen] * 8
            for pipe in range(pipe_list_len):
                for pkt in range(2):
                    app_id = (7 << 0) | (pipe << 3)
                    dmac='%02x:00:%02x:%02x:%02x:%02x' % (app_id, 0, 0, 0, pkt)
                    p_exp = simple_eth_packet(pktlen=pktlen, eth_dst=dmac)
                    pkt_lst.append(p_exp)
            verify_multiple_packets(self, eport, pkt_lst, pkt_len, tmo=1)
            verify_no_other_packets(self)
            for app in range(8):
                for pipe in range(pipe_list_len):
                    trigger_cntrs[pipe][app] = self.conn_mgr.pktgen_get_trigger_counter(shdl, dev_tgt_[pipe], app)
                    batch_cntrs[pipe][app] = self.conn_mgr.pktgen_get_batch_counter(shdl, dev_tgt_[pipe], app)
                    packet_cntrs[pipe][app] = self.conn_mgr.pktgen_get_pkt_counter(shdl, dev_tgt_[pipe], app)
            for pipe in range(pipe_list_len):
                for app in range(8):
                    self.assertEqual(trigger_cntrs[pipe][app], trigger_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, trigger_cntrs[pipe][app], trigger_cntrs_exp[pipe][app]))
                    self.assertEqual(batch_cntrs[pipe][app], batch_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, batch_cntrs[pipe][app], batch_cntrs_exp[pipe][app]))
                    self.assertEqual(packet_cntrs[pipe][app], packet_cntrs_exp[pipe][app], "Pipe %d App %d Got 0x%x Exp 0x%x" % (pipe, app, packet_cntrs[pipe][app], packet_cntrs_exp[pipe][app]))

        finally:
            self.conn_mgr.pktgen_app_disable(shdl_extra, dev_tgt, 7)
            app_enabled = self.conn_mgr.pktgen_app_enable_state_get(shdl_extra, dev_tgt, 7)
            self.assertFalse(app_enabled)
            for pipe in range(pipe_list_len):
                for port in range(68,72):
                    self.conn_mgr.pktgen_disable(shdl, dev_id, make_port(pipe, port))
                    enabled = self.conn_mgr.pktgen_enable_state_get(shdl, dev_id, make_port(pipe, port))
                    self.assertFalse(enabled)
            for x in timer_verify_handles:
                self.client.pg_verify_timer_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(x))
            for eh in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(eh))

            self.conn_mgr.client_cleanup(shdl)
            self.conn_mgr.client_cleanup(shdl_extra)

class TestAddrOverride(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, mod_name, prefix)
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        reg_flag = pgrs_register_flags_t(read_hw_sync = False)
        cntr_flag = pgrs_counter_flags_t(read_hw_sync = False)
        port_tbl_handles = []
        t_or_e_handles = []
        t_hdl = None
        e_hdl = None
        t_hdls = [None]*7
        e_hdls = [None]*7

        # Install two ingress entries so we can forward to egress ports 1 and 2
        self.client.port_tbl_set_property(shdl, dev_id, tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE, tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES, 0);
        h = port_tbl_add(self, None, shdl, dt, swports[1],  swports[1], 0, pfe=1)
        port_tbl_handles.append(h)
        h = port_tbl_add(self, None, shdl, dt, swports[2],  swports[2], 0, pfe=1)
        port_tbl_handles.append(h)

        pkt = simple_eth_packet(pktlen=64)

        cntr_zero = pgrs_counter_value_t(packets=0, bytes=0)
        reg_zero = 0
        p1_pipe = port_to_pipe(swports[1])
        p2_pipe = port_to_pipe(swports[2])
        try:
            h = self.client.eg_tcam_or_exm_table_add_with_do_tcam(shdl, dt, pgrs_eg_tcam_or_exm_match_spec_t(hex_to_i16(swports[1])))
            t_or_e_handles.append(h)
            h = self.client.eg_tcam_or_exm_table_add_with_do_exm(shdl, dt, pgrs_eg_tcam_or_exm_match_spec_t(hex_to_i16(swports[2])))
            t_or_e_handles.append(h)

            # Test TCAM default actions
            # First check without the counter or register
            self.conn_mgr.begin_batch( shdl )
            self.client.t1_table_reset_default_entry(shdl, dt)
            self.client.t2_table_reset_default_entry(shdl, dt)
            self.client.t4_table_reset_default_entry(shdl, dt)
            self.client.t5_table_reset_default_entry(shdl, dt)
            self.client.e1_table_reset_default_entry(shdl, dt)
            self.client.e2_table_reset_default_entry(shdl, dt)
            self.client.e4_table_reset_default_entry(shdl, dt)
            self.client.e5_table_reset_default_entry(shdl, dt)
            for i in range(500):
                self.client.counter_write_c1(shdl, dt, i, cntr_zero)
                self.client.counter_write_c2(shdl, dt, i, cntr_zero)
                self.client.register_write_r1(shdl, dt, i, reg_zero)
                self.client.register_write_r2(shdl, dt, i, reg_zero)
            self.client.t1_set_default_action_a1(shdl, dt)
            self.client.t2_set_default_action_a1(shdl, dt)
            self.client.t4_set_default_action_a1(shdl, dt)
            self.client.t5_set_default_action_a1(shdl, dt)
            self.client.e1_set_default_action_a1(shdl, dt)
            self.client.e2_set_default_action_a1(shdl, dt)
            self.client.e4_set_default_action_a1(shdl, dt)
            self.client.e5_set_default_action_a1(shdl, dt)
            self.conn_mgr.end_batch(shdl, True)
            send_packet(self, swports[1], pkt)
            verify_packet(self, pkt, swports[1])
            send_packet(self, swports[2], pkt)
            verify_packets(self, pkt, [swports[2]])
            self.client.counter_hw_sync_c1(shdl, dt, True)
            self.client.counter_hw_sync_c2(shdl, dt, True)
            self.client.register_hw_sync_r1(shdl, dt)
            self.client.register_hw_sync_r2(shdl, dt)
            for i in range(500):
                c = self.client.counter_read_c1(shdl, dt, i, cntr_flag)
                self.assertEqual(c, cntr_zero)
                c = self.client.counter_read_c2(shdl, dt, i, cntr_flag)
                self.assertEqual(c, cntr_zero)
                r = self.client.register_read_r1(shdl, dt, i, reg_flag)
                self.assertEqual(r, [0]*num_pipes)
                r = self.client.register_read_r2(shdl, dt, i, reg_flag)
                self.assertEqual(r, [0]*num_pipes)

            # Next check with the counter and register
            # Note that default actions cannot use hash distribution so a3 and
            # a6 are not set here.
            self.conn_mgr.begin_batch( shdl )
            for i in range(500):
                self.client.counter_write_c1(shdl, dt, i, cntr_zero)
                self.client.counter_write_c2(shdl, dt, i, cntr_zero)
                self.client.register_write_r1(shdl, dt, i, reg_zero)
                self.client.register_write_r2(shdl, dt, i, reg_zero)
            self.client.t1_set_default_action_a2(shdl, dt, pgrs_a2_action_spec_t(499))
            self.client.t2_set_default_action_a1(shdl, dt)
            self.client.t4_set_default_action_a5(shdl, dt, pgrs_a5_action_spec_t(499))
            self.client.t5_set_default_action_a1(shdl, dt)
            self.client.e1_set_default_action_a2(shdl, dt, pgrs_a2_action_spec_t(499))
            self.client.e2_set_default_action_a1(shdl, dt)
            self.client.e4_set_default_action_a5(shdl, dt, pgrs_a5_action_spec_t(499))
            self.client.e5_set_default_action_a1(shdl, dt)
            self.conn_mgr.end_batch(shdl, True)
            send_packet(self, swports[1], pkt)
            verify_packet(self, pkt, swports[1])
            send_packet(self, swports[2], pkt)
            verify_packets(self, pkt, [swports[2]])
            self.client.counter_hw_sync_c1(shdl, dt, True)
            self.client.counter_hw_sync_c2(shdl, dt, False)
            self.client.register_hw_sync_r1(shdl, dt)
            self.client.register_hw_sync_r2(shdl, dt)
            c_exp2 = pgrs_counter_value_t(packets=2, bytes=0)
            c_exp1 = pgrs_counter_value_t(packets=1, bytes=0)
            r_zero = [0]*num_pipes
            r_exp_p1p2 = [0]*num_pipes
            r_exp_p1p2[p1_pipe] += 1
            r_exp_p1p2[p2_pipe] += 1
            for i in range(500):
                c = self.client.counter_read_c1(shdl, dt, i, cntr_flag)
                if i == 499:
                    self.assertEqual(c, c_exp2)
                else:
                    self.assertEqual(c, cntr_zero)
                c = self.client.counter_read_c2(shdl, dt, i, cntr_flag)
                self.assertEqual(c, cntr_zero)

                r = self.client.register_read_r1(shdl, dt, i, reg_flag)
                if i == 499:
                    self.assertEqual(r, r_exp_p1p2)
                else:
                    self.assertEqual(r, r_zero)
                r = self.client.register_read_r2(shdl, dt, i, reg_flag)
                self.assertEqual(r, r_zero)

            # Check again using a4 and a7.
            self.conn_mgr.begin_batch( shdl )
            for i in range(500):
                self.client.counter_write_c1(shdl, dt, i, cntr_zero)
                self.client.counter_write_c2(shdl, dt, i, cntr_zero)
                self.client.register_write_r1(shdl, dt, i, reg_zero)
                self.client.register_write_r2(shdl, dt, i, reg_zero)
            self.client.t1_set_default_action_a4(shdl, dt)
            self.client.t2_set_default_action_a1(shdl, dt)
            self.client.t4_set_default_action_a7(shdl, dt)
            self.client.t5_set_default_action_a1(shdl, dt)
            self.client.e1_set_default_action_a4(shdl, dt)
            self.client.e2_set_default_action_a1(shdl, dt)
            self.client.e4_set_default_action_a7(shdl, dt)
            self.client.e5_set_default_action_a1(shdl, dt)
            self.conn_mgr.end_batch(shdl, True)
            send_packet(self, swports[1], pkt)
            verify_packet(self, pkt, swports[1])
            send_packet(self, swports[2], pkt)
            verify_packets(self, pkt, [swports[2]])
            self.client.counter_hw_sync_c1(shdl, dt, True)
            self.client.counter_hw_sync_c2(shdl, dt, False)
            self.client.register_hw_sync_r1(shdl, dt)
            self.client.register_hw_sync_r2(shdl, dt)
            c_exp2 = pgrs_counter_value_t(packets=2, bytes=0)
            c_exp1 = pgrs_counter_value_t(packets=1, bytes=0)
            r_zero = [0]*num_pipes
            r_exp_p1p2 = [0]*num_pipes
            r_exp_p1p2[p1_pipe] += 1
            r_exp_p1p2[p2_pipe] += 1
            for i in range(500):
                c = self.client.counter_read_c1(shdl, dt, i, cntr_flag)
                if i == 12:
                    self.assertEqual(c, c_exp2)
                else:
                    self.assertEqual(c, cntr_zero)
                c = self.client.counter_read_c2(shdl, dt, i, cntr_flag)
                self.assertEqual(c, cntr_zero)

                r = self.client.register_read_r1(shdl, dt, i, reg_flag)
                if i == 12:
                    self.assertEqual(r, r_exp_p1p2)
                else:
                    self.assertEqual(r, r_zero)
                r = self.client.register_read_r2(shdl, dt, i, reg_flag)
                self.assertEqual(r, r_zero)

            # Check again using a4 and a8.
            self.conn_mgr.begin_batch( shdl )
            for i in range(500):
                self.client.counter_write_c1(shdl, dt, i, cntr_zero)
                self.client.counter_write_c2(shdl, dt, i, cntr_zero)
                self.client.register_write_r1(shdl, dt, i, reg_zero)
                self.client.register_write_r2(shdl, dt, i, reg_zero)
            self.client.t1_set_default_action_a4(shdl, dt)
            self.client.t2_set_default_action_a1(shdl, dt)
            self.client.t4_set_default_action_a8(shdl, dt)
            self.client.t5_set_default_action_a1(shdl, dt)
            self.client.e1_set_default_action_a4(shdl, dt)
            self.client.e2_set_default_action_a1(shdl, dt)
            self.client.e4_set_default_action_a8(shdl, dt)
            self.client.e5_set_default_action_a1(shdl, dt)
            self.conn_mgr.end_batch(shdl, True)
            send_packet(self, swports[1], pkt)
            verify_packet(self, pkt, swports[1])
            send_packet(self, swports[2], pkt)
            verify_packets(self, pkt, [swports[2]])
            self.client.counter_hw_sync_c1(shdl, dt, True)
            self.client.counter_hw_sync_c2(shdl, dt, False)
            self.client.register_hw_sync_r1(shdl, dt)
            self.client.register_hw_sync_r2(shdl, dt)
            c_exp2 = pgrs_counter_value_t(packets=2, bytes=0)
            c_exp1 = pgrs_counter_value_t(packets=1, bytes=0)
            r_zero = [0]*num_pipes
            r_exp_p1p2 = [0]*num_pipes
            r_exp_p1p2[p1_pipe] += 10
            r_exp_p1p2[p2_pipe] += 10
            for i in range(500):
                c = self.client.counter_read_c1(shdl, dt, i, cntr_flag)
                if i == 12:
                    self.assertEqual(c, c_exp2)
                else:
                    self.assertEqual(c, cntr_zero)
                c = self.client.counter_read_c2(shdl, dt, i, cntr_flag)
                self.assertEqual(c, cntr_zero)

                r = self.client.register_read_r1(shdl, dt, i, reg_flag)
                if i == 13:
                    self.assertEqual(r, r_exp_p1p2)
                else:
                    self.assertEqual(r, r_zero)
                r = self.client.register_read_r2(shdl, dt, i, reg_flag)
                self.assertEqual(r, r_zero)


            #
            # Check non-default actions
            #
            self.conn_mgr.begin_batch( shdl )
            self.client.t1_table_reset_default_entry(shdl, dt)
            self.client.t2_table_reset_default_entry(shdl, dt)
            self.client.t4_table_reset_default_entry(shdl, dt)
            self.client.t5_table_reset_default_entry(shdl, dt)
            self.client.e1_table_reset_default_entry(shdl, dt)
            self.client.e2_table_reset_default_entry(shdl, dt)
            self.client.e4_table_reset_default_entry(shdl, dt)
            self.client.e5_table_reset_default_entry(shdl, dt)
            for i in range(500):
                self.client.counter_write_c1(shdl, dt, i, cntr_zero)
                self.client.counter_write_c2(shdl, dt, i, cntr_zero)
                self.client.register_write_r1(shdl, dt, i, reg_zero)
                self.client.register_write_r2(shdl, dt, i, reg_zero)
            t_hdls[1] = self.client.t1_table_add_with_a1(shdl, dt, pgrs_t1_match_spec_t(hex_to_i16(swports[1]), hex_to_i16(0xFFFF)), 0)
            t_hdls[2] = self.client.t2_table_add_with_a1(shdl, dt, pgrs_t2_match_spec_t(hex_to_i16(swports[1]), hex_to_i16(0xFFFF)), 0)
            t_hdls[4] = self.client.t4_table_add_with_a1(shdl, dt, pgrs_t4_match_spec_t(hex_to_i16(swports[1]), hex_to_i16(0xFFFF)), 0)
            t_hdls[5] = self.client.t5_table_add_with_a1(shdl, dt, pgrs_t5_match_spec_t(hex_to_i16(swports[1]), hex_to_i16(0xFFFF)), 0)
            e_hdls[1] = self.client.e1_table_add_with_a1(shdl, dt, pgrs_e1_match_spec_t(hex_to_i16(swports[2])))
            e_hdls[2] = self.client.e2_table_add_with_a1(shdl, dt, pgrs_e2_match_spec_t(hex_to_i16(swports[2])))
            e_hdls[4] = self.client.e4_table_add_with_a1(shdl, dt, pgrs_e4_match_spec_t(hex_to_i16(swports[2])))
            e_hdls[5] = self.client.e5_table_add_with_a1(shdl, dt, pgrs_e5_match_spec_t(hex_to_i16(swports[2])))
            self.conn_mgr.end_batch(shdl, True)
            send_packet(self, swports[1], pkt)
            verify_packet(self, pkt, swports[1])
            send_packet(self, swports[2], pkt)
            verify_packets(self, pkt, [swports[2]])
            self.client.counter_hw_sync_c1(shdl, dt, True)
            self.client.counter_hw_sync_c2(shdl, dt, True)
            self.client.register_hw_sync_r1(shdl, dt)
            self.client.register_hw_sync_r2(shdl, dt)
            for i in range(500):
                c = self.client.counter_read_c1(shdl, dt, i, cntr_flag)
                self.assertEqual(c, cntr_zero)
                c = self.client.counter_read_c2(shdl, dt, i, cntr_flag)
                self.assertEqual(c, cntr_zero)
                r = self.client.register_read_r1(shdl, dt, i, reg_flag)
                self.assertEqual(r, [0]*num_pipes)
                r = self.client.register_read_r2(shdl, dt, i, reg_flag)
                self.assertEqual(r, [0]*num_pipes)
            # Next check with the counter and register
            self.conn_mgr.begin_batch( shdl )
            for i in range(500):
                self.client.counter_write_c1(shdl, dt, i, cntr_zero)
                self.client.counter_write_c2(shdl, dt, i, cntr_zero)
                self.client.register_write_r1(shdl, dt, i, reg_zero)
                self.client.register_write_r2(shdl, dt, i, reg_zero)
            self.client.t1_table_modify_with_a2(shdl, dev_id, t_hdls[1], pgrs_a2_action_spec_t(499))
            self.client.t2_table_modify_with_a3(shdl, dev_id, t_hdls[2])
            self.client.t4_table_modify_with_a5(shdl, dev_id, t_hdls[4], pgrs_a5_action_spec_t(499))
            self.client.t5_table_modify_with_a6(shdl, dev_id, t_hdls[5])
            self.client.e1_table_modify_with_a2(shdl, dev_id, e_hdls[1], pgrs_a2_action_spec_t(499))
            self.client.e2_table_modify_with_a3(shdl, dev_id, e_hdls[2])
            self.client.e4_table_modify_with_a5(shdl, dev_id, e_hdls[4], pgrs_a5_action_spec_t(499))
            self.client.e5_table_modify_with_a6(shdl, dev_id, e_hdls[5])
            self.conn_mgr.end_batch(shdl, True)
            send_packet(self, swports[1], pkt)
            verify_packet(self, pkt, swports[1])
            send_packet(self, swports[2], pkt)
            verify_packets(self, pkt, [swports[2]])
            self.client.counter_hw_sync_c1(shdl, dt, True)
            self.client.counter_hw_sync_c2(shdl, dt, True)
            self.client.register_hw_sync_r1(shdl, dt)
            self.client.register_hw_sync_r2(shdl, dt)
            c_exp2 = pgrs_counter_value_t(packets=2, bytes=0)
            c_exp1 = pgrs_counter_value_t(packets=1, bytes=0)
            r_zero = [0]*num_pipes
            r_exp_p1p2 = [0]*num_pipes
            r_exp_p1p2[p1_pipe] += 1
            r_exp_p1p2[p2_pipe] += 1
            r_exp_p1 = [0]*num_pipes
            r_exp_p1[p1_pipe] += 1
            r_exp_p2 = [0]*num_pipes
            r_exp_p2[p2_pipe] += 1
            for i in range(500):
                c = self.client.counter_read_c1(shdl, dt, i, cntr_flag)
                if i == 499:
                    self.assertEqual(c, c_exp2)
                else:
                    self.assertEqual(c, cntr_zero)
                c = self.client.counter_read_c2(shdl, dt, i, cntr_flag)
                if i == swports[1]:
                    self.assertEqual(c, c_exp1)
                elif i == swports[2]:
                    self.assertEqual(c, c_exp1)
                else:
                    self.assertEqual(c, cntr_zero)

                r = self.client.register_read_r1(shdl, dt, i, reg_flag)
                if i == 499:
                    self.assertEqual(r, r_exp_p1p2)
                else:
                    self.assertEqual(r, r_zero)
                r = self.client.register_read_r2(shdl, dt, i, reg_flag)
                if i == swports[1]:
                    self.assertEqual(r, r_exp_p1)
                elif i == swports[2]:
                    self.assertEqual(r, r_exp_p2)
                else:
                    self.assertEqual(r, r_zero)
            # Next check with the counter and register at fixed indexes
            self.conn_mgr.begin_batch( shdl )
            for i in range(500):
                self.client.counter_write_c1(shdl, dt, i, cntr_zero)
                self.client.counter_write_c2(shdl, dt, i, cntr_zero)
                self.client.register_write_r1(shdl, dt, i, reg_zero)
                self.client.register_write_r2(shdl, dt, i, reg_zero)
            self.client.t1_table_modify_with_a4(shdl, dev_id, t_hdls[1])
            self.client.t2_table_modify_with_a3(shdl, dev_id, t_hdls[2])
            self.client.t4_table_modify_with_a7(shdl, dev_id, t_hdls[4])
            self.client.t5_table_modify_with_a6(shdl, dev_id, t_hdls[5])
            self.client.e1_table_modify_with_a4(shdl, dev_id, e_hdls[1])
            self.client.e2_table_modify_with_a3(shdl, dev_id, e_hdls[2])
            self.client.e4_table_modify_with_a7(shdl, dev_id, e_hdls[4])
            self.client.e5_table_modify_with_a6(shdl, dev_id, e_hdls[5])
            self.conn_mgr.end_batch(shdl, True)
            send_packet(self, swports[1], pkt)
            verify_packets(self, pkt, [swports[1]])
            send_packet(self, swports[2], pkt)
            verify_packets(self, pkt, [swports[2]])
            self.client.counter_hw_sync_c1(shdl, dt, True)
            self.client.counter_hw_sync_c2(shdl, dt, False)
            self.client.register_hw_sync_r1(shdl, dt)
            self.client.register_hw_sync_r2(shdl, dt)
            c_exp2 = pgrs_counter_value_t(packets=2, bytes=0)
            c_exp1 = pgrs_counter_value_t(packets=1, bytes=0)
            r_zero = [0]*num_pipes
            r_exp_p1p2 = [0]*num_pipes
            r_exp_p1p2[p1_pipe] += 1
            r_exp_p1p2[p2_pipe] += 1
            r_exp_p1 = [0]*num_pipes
            r_exp_p1[p1_pipe] += 1
            r_exp_p2 = [0]*num_pipes
            r_exp_p2[p2_pipe] += 1
            for i in range(500):
                c = self.client.counter_read_c1(shdl, dt, i, cntr_flag)
                if i == 12:
                    self.assertEqual(c, c_exp2)
                else:
                    self.assertEqual(c, cntr_zero)
                c = self.client.counter_read_c2(shdl, dt, i, cntr_flag)
                if i == swports[1]:
                    self.assertEqual(c, c_exp1)
                elif i == swports[2]:
                    self.assertEqual(c, c_exp1)
                else:
                    self.assertEqual(c, cntr_zero)

                r = self.client.register_read_r1(shdl, dt, i, reg_flag)
                if i == 12:
                    self.assertEqual(r, r_exp_p1p2)
                else:
                    self.assertEqual(r, r_zero)
                r = self.client.register_read_r2(shdl, dt, i, reg_flag)
                if i == swports[1]:
                    self.assertEqual(r, r_exp_p1)
                elif i == swports[2]:
                    self.assertEqual(r, r_exp_p2)
                else:
                    self.assertEqual(r, r_zero)
            # Next check with the counter and register at fixed indexes using
            # the other register operation.
            self.conn_mgr.begin_batch( shdl )
            for i in range(500):
                self.client.counter_write_c1(shdl, dt, i, cntr_zero)
                self.client.counter_write_c2(shdl, dt, i, cntr_zero)
                self.client.register_write_r1(shdl, dt, i, reg_zero)
                self.client.register_write_r2(shdl, dt, i, reg_zero)
            self.client.t1_table_modify_with_a4(shdl, dev_id, t_hdls[1])
            self.client.t2_table_modify_with_a3(shdl, dev_id, t_hdls[2])
            self.client.t4_table_modify_with_a8(shdl, dev_id, t_hdls[4])
            self.client.t5_table_modify_with_a9(shdl, dev_id, t_hdls[5])
            self.client.e1_table_modify_with_a4(shdl, dev_id, e_hdls[1])
            self.client.e2_table_modify_with_a3(shdl, dev_id, e_hdls[2])
            self.client.e4_table_modify_with_a8(shdl, dev_id, e_hdls[4])
            self.client.e5_table_modify_with_a9(shdl, dev_id, e_hdls[5])
            self.conn_mgr.end_batch(shdl, True)
            send_packet(self, swports[1], pkt)
            verify_packets(self, pkt, [swports[1]])
            send_packet(self, swports[2], pkt)
            verify_packets(self, pkt, [swports[2]])
            self.client.counter_hw_sync_c1(shdl, dt, True)
            self.client.counter_hw_sync_c2(shdl, dt, True)
            self.client.register_hw_sync_r1(shdl, dt)
            self.client.register_hw_sync_r2(shdl, dt)
            c_exp2 = pgrs_counter_value_t(packets=2, bytes=0)
            c_exp1 = pgrs_counter_value_t(packets=1, bytes=0)
            r_zero = [0]*num_pipes
            r_exp_p1p2 = [0]*num_pipes
            r_exp_p1p2[p1_pipe] += 10
            r_exp_p1p2[p2_pipe] += 10
            r_exp_p1 = [0]*num_pipes
            r_exp_p1[p1_pipe] += 10
            r_exp_p2 = [0]*num_pipes
            r_exp_p2[p2_pipe] += 10
            for i in range(500):
                c = self.client.counter_read_c1(shdl, dt, i, cntr_flag)
                if i == 12:
                    self.assertEqual(c, c_exp2)
                else:
                    self.assertEqual(c, cntr_zero)
                c = self.client.counter_read_c2(shdl, dt, i, cntr_flag)
                if i == swports[1]:
                    self.assertEqual(c, c_exp1)
                elif i == swports[2]:
                    self.assertEqual(c, c_exp1)
                else:
                    self.assertEqual(c, cntr_zero)

                r = self.client.register_read_r1(shdl, dt, i, reg_flag)
                if i == 13:
                    self.assertEqual(r, r_exp_p1p2)
                else:
                    self.assertEqual(r, r_zero)
                r = self.client.register_read_r2(shdl, dt, i, reg_flag)
                if i == swports[1]:
                    self.assertEqual(r, r_exp_p1)
                elif i == swports[2]:
                    self.assertEqual(r, r_exp_p2)
                else:
                    self.assertEqual(r, r_zero)

        finally:
            for h in port_tbl_handles:
                self.client.port_tbl_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(h))
            for h in t_or_e_handles:
                self.client.eg_tcam_or_exm_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(h))
            if t_hdls[1]: self.client.t1_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(t_hdls[1]))
            if t_hdls[2]: self.client.t2_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(t_hdls[2]))
            if t_hdls[4]: self.client.t4_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(t_hdls[4]))
            if t_hdls[5]: self.client.t5_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(t_hdls[5]))
            if e_hdls[1]: self.client.e1_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(e_hdls[1]))
            if e_hdls[2]: self.client.e2_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(e_hdls[2]))
            if e_hdls[4]: self.client.e4_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(e_hdls[4]))
            if e_hdls[5]: self.client.e5_table_delete(hex_to_i32(shdl), dev_id, hex_to_i32(e_hdls[5]))
            self.client.eg_tcam_or_exm_table_reset_default_entry(shdl, dt)
            self.client.t1_table_reset_default_entry(shdl, dt)
            self.client.t2_table_reset_default_entry(shdl, dt)
            self.client.t4_table_reset_default_entry(shdl, dt)
            self.client.t5_table_reset_default_entry(shdl, dt)
            self.client.e1_table_reset_default_entry(shdl, dt)
            self.client.e2_table_reset_default_entry(shdl, dt)
            self.client.e4_table_reset_default_entry(shdl, dt)
            self.client.e5_table_reset_default_entry(shdl, dt)

            self.conn_mgr.client_cleanup(shdl)
