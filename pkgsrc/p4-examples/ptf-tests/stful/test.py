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

from __future__ import print_function

from collections import OrderedDict

import time
import sys
import logging
import copy
import pdb
import datetime

import unittest
import random

import pd_base_tests

from ptf import config
from ptf.testutils import *
from ptf.thriftutils import *
import ptf.dataplane as dataplane
from p4testutils.misc_utils import *

import os

from pal_rpc.ttypes import *
from stful.p4_pd_rpc.ttypes import *
from conn_mgr_pd_rpc.ttypes import *
from mc_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from ptf_port import *


dev_id = 0
PORT_TBL_SIZE = 288
IFID_TBL_SIZE = 25000
SIP_SAMPLER_TBL_SIZE = 85000
SIP_SAMPLER_REG_SIZE = 143360
SCRATCH_REG_SIZE = 4096
NEXT_HOP_TBL_SIZE = 4096
IP_ROUTE_TBL_SIZE = 512
EGR_PORT_TBL_SIZE = 16384
BLOOM_FILTER_REG_SIZE = 256*1024
MAX_PORT_COUNT = 456
ECMP_REG_SIZE = 128*1024
LAG_REG_SIZE = 128*1024

RECIRC_TYPE_PG_PORT_DOWN = 1
RECIRC_TYPE_PG_RECIRC    = 2


swports = get_sw_ports()
print("Using ports:", swports)
sys.stdout.flush()

"""
Port Table
==============
 - Add random, unique, entries.
 - Remove all entries.
 - Map a port to ifid
 - Provide list of valid ifids
"""
class PortTable:
    def __init__(self, test, shdl, dev):
        self.test = test
        self.shdl = shdl
        self.dev = dev
        self.tbl_sz = PORT_TBL_SIZE
        self.entry_hdls = []
        self.port_to_hdl = {}
        self.port_to_ifid = {}
        self.sym = 1
        num_pipes = int(test_param_get('num_pipes'))
        self.pipe_list = [x for x in range(num_pipes)]
        self.pipe_list_len = len(self.pipe_list)

    def set_sym(self, tf):
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
        if tf == 0 or tf == False:
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
        self.test.client.ing_port_set_property(self.shdl, self.dev, prop, prop_val, 0)
        self.sym = tf

    def add(self, port, ifid):
        if self.sym:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
        else:
            pipe = port_to_pipe(port)
            dev_tgt = DevTarget_t(self.dev, pipe)
        ms = stful_ing_port_match_spec_t(port)
        act = stful_set_ifid_action_spec_t( hex_to_i16(ifid) )
        eh = self.test.client.ing_port_table_add_with_set_ifid(self.shdl, dev_tgt, ms, act)

        self.entry_hdls.append(eh)
        self.port_to_hdl[port] = eh
        self.port_to_ifid[port] = ifid

    def rmv(self, port):
        entry_handle = self.port_to_hdl[port]
        self.test.client.ing_port_table_delete(self.shdl, self.dev, entry_handle)
        self.entry_hdls.remove(entry_handle)
        del self.port_to_hdl[port]
        del self.port_to_ifid[port]

    def add_all_ports(self):
        act_data = random.sample(range(0x10000), self.tbl_sz)
        for x in range(self.tbl_sz):
            port = ((x//72) << 7) | (x % 72)
            ifid = act_data[x]
            self.add(port, ifid)

    def rmv_all(self, remove_state=True):
        for entry_handle in self.entry_hdls:
            self.test.client.ing_port_table_delete(self.shdl, self.dev, entry_handle)
        if remove_state:
            self.entry_hdls = []
            self.port_to_hdl = {}
            self.port_to_ifid = {}

    def get_ifid(self, port):
        return self.port_to_ifid[port]

    def ifid_to_ports(self, ifid):
        port_list = []
        for port in self.port_to_ifid:
            if self.port_to_ifid[port] == ifid:
                port_list.append(port)
        return port_list

    def valid_ifids(self):
        all_ifids = []
        for port in self.port_to_ifid:
            ifid = self.port_to_ifid[port]
            if ifid not in all_ifids:
                all_ifids.append(ifid)
        return all_ifids

"""
IFID Table
================
 - Add entry for each ifid in use
 - Add entries for ifids not in use
 - Get counters for an IFID
   - 4 counters for symmertic
   - 4 for asymmetric, but 3 will be zero
"""
class IfidTbl:
    def __init__(self, test, shdl, dev, port_tbl):
        num_pipes = int(test_param_get('num_pipes'))

        self.test = test
        self.shdl = shdl
        self.dev = dev
        self.tbl_sz = IFID_TBL_SIZE
        self.entry_hdls = []
        self.default_entry_hdls = {} # Pipe to handle
        self.ifid_to_hdl = {}
        self.ifid_to_data = {}
        self.ifid_to_cntr = {}
        self.ifid_dflt_cntr = [0]*num_pipes
        self.sym = 1
        self.port_tbl = port_tbl
        self.pipe_list = [x for x in range(num_pipes)]
        self.pipe_list_len = len(self.pipe_list)

    def set_sym(self, tf):
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
        if tf == 0 or tf == False:
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
        self.test.client.ifid_set_property(self.shdl, self.dev, prop, prop_val, 0)
        self.sym = tf

    def set_cntr(self, ifid, pipe, val):
        if ifid is not None:
            if ifid not in self.ifid_to_cntr:
                self.ifid_to_cntr[ifid] = []
            for i in range(self.pipe_list_len):
                self.ifid_to_cntr[ifid].append(None)

            counters = self.ifid_to_cntr[ifid]

            if pipe == 0xFFFF:
                for i in range(self.pipe_list_len):
                    counters[i] = val
            else:
                counters[pipe] = val
            self.ifid_to_cntr[ifid] = counters
        else:
            if pipe == 0xFFFF:
                for i in range(self.pipe_list_len):
                    self.ifid_dflt_cntr[i] = val
            else:
                self.ifid_dflt_cntr[pipe] = val

    def inc_cntr(self, ifid, pipe, val):
        self.test.assertTrue(pipe >= 0 and pipe < 4)
        if ifid is not None:
            self.test.assertTrue(ifid in self.ifid_to_cntr)
            self.test.assertTrue(self.ifid_to_cntr[ifid][pipe] is not None)
            self.ifid_to_cntr[ifid][pipe] += val
        else:
            self.ifid_dflt_cntr[pipe] += val

    def mod(self, ifid, ts=None, offset=None, pipe=None, counter=None):
        if self.sym and pipe is not None:
            self.test.assertTrue(0) # Cannot specify a pipe if symmetric
        elif self.sym:
            pipes = [0xFFFF]
        else:
            # Not symmetric, if pipe was specified use it.  Otherwise, check
            # which ports are using the ifid and install in those pipes.
            if pipe is not None:
                pipes = [pipe]
            elif ifid is None:
                # Default entry modify for an asymmetric table, pipe is required
                self.test.assertTrue(pipe is not None)
            else:
                ports = self.port_tbl.ifid_to_ports(ifid)
                pipes = []
                for port in ports:
                    pipes.append( port_to_pipe(port) )
                pipes = list( set(pipes) ) # Remove duplicates
                assert(len(pipes))

        if counter is None:
            counter = 0
        for pipe in pipes:
            if ifid is not None:
                eh = self.ifid_to_hdl[(ifid,pipe)]
                if ts is None and offset is None:
                    eh = self.test.client.ifid_table_modify_with_drop_it(self.shdl, self.dev, eh, counter)
                else:
                    if ts is None:
                        ts = 0
                    if offset is None:
                        offset = 0
                    ts_bytes = [8]*6 # 6 byte (48 bit) value
                    for i in range(6):
                        ts_bytes[i] = (ts >> ((5-i)*8)) & 0xFF
                    act = stful_set_ifid_based_params_action_spec_t(bytes_to_string(ts_bytes), hex_to_i32(offset))
                    eh = self.test.client.ifid_table_modify_with_set_ifid_based_params(self.shdl, self.dev, eh, act, counter)
                self.set_cntr(ifid, pipe, counter)
            else:
                eh = self.test.client.ifid_set_default_action_drop_it(self.shdl, DevTarget_t(self.dev, hex_to_i16(pipe)), counter)
                self.default_entry_hdls[pipe] = eh
                self.set_cntr(ifid, pipe, counter)

    def ifid_to_offset(self, ifid, pipe=0xFFFF):
        if self.sym:
            pipe = 0xFFFF
        self.test.assertIn( (ifid,pipe), self.ifid_to_data)
        offset,_ = self.ifid_to_data[ (ifid,pipe) ]
        return offset

    def add(self, ifid, ts=None, offset=None, pipe=None, counter=None):
        if self.sym and pipe is not None and pipe != 0xFFFF:
            self.test.assertEqual(pipe, 0xFFFF) # Cannot specify a pipe if symmetric
        elif self.sym:
            pipes = [0xFFFF]
        else:
            # Not symmetric, if pipe was specified use it.  Otherwise, check
            # which ports are using the ifid and install in those pipes.
            if pipe is not None:
                pipes = [pipe]
            elif ifid is None:
                # Default entry add for an asymmetric table, pipe is required
                self.test.assertTrue(pipe is not None)
            else:
                ports = self.port_tbl.ifid_to_ports(ifid)
                pipes = []
                for port in ports:
                    pipes.append( port_to_pipe(port) )
                pipes = list( set(pipes) ) # Remove duplicates
                assert(len(pipes))

        # Ensure the entry isn't there yet.
        for pipe in pipes:
            if (ifid, pipe) in self.ifid_to_hdl:
                self.test.assertTrue(0)

        if counter is None:
            counter = 0
        if ifid is not None:
            ms = stful_ifid_match_spec_t(ifid)
            for pipe in pipes:
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
                if ts is None and offset is None:
                    eh = self.test.client.ifid_table_add_with_drop_it(self.shdl, dev_tgt, ms, counter)
                else:
                    if ts is None:
                        ts = 0
                    if offset is None:
                        offset = 0
                    ts_bytes = [8]*6 # 6 byte (48 bit) value
                    for i in range(6):
                        ts_bytes[i] = (ts >> ((5-i)*8)) & 0xFF
                    act = stful_set_ifid_based_params_action_spec_t(bytes_to_string(ts_bytes), hex_to_i32(offset))
                    eh = self.test.client.ifid_table_add_with_set_ifid_based_params(self.shdl, dev_tgt, ms, act, counter)
                self.entry_hdls.append(eh)
                self.ifid_to_hdl[(ifid, pipe)] = eh
                self.ifid_to_data[(ifid, pipe)] = (offset,ts)
                self.set_cntr(ifid, pipe, counter)
        else:
            eh = self.test.client.ifid_set_default_action_drop_it(self.shdl, DevTarget_t(self.dev, hex_to_i16(pipe)), counter)
            self.default_entry_hdls[pipe] = eh
            self.set_cntr(ifid, pipe, counter)

    def rmv(self, ifid, pipe=None):
        if self.sym and pipe is None:
            pipe = 0xFFFF
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)

        if ifid is not None:
            entry_handle = self.ifid_to_hdl[(ifid, pipe)]
            self.test.client.ifid_table_delete(self.shdl, self.dev, entry_handle)
            self.entry_hdls.remove(entry_handle)
            del self.ifid_to_hdl[(ifid, pipe)]
            del self.ifid_to_data[(ifid, pipe)]
            if self.sym:
                del self.ifid_to_cntr[ifid]
            else:
                self.ifid_to_cntr[ifid][pipe] = None
                empty = 1
                for i in range(self.pipe_list_len):
                    if self.ifid_to_cntr[ifid][i] is not None:
                        empty = 0
                if empty == 1:
                    del self.ifid_to_cntr[ifid]
        else:
            self.test.client.ifid_table_reset_default_entry(self.shdl, DevTarget_t(self.dev, hex_to_i16(pipe)))
            if pipe in self.default_entry_hdls:
                del self.default_entry_hdls[pipe]


    def add_some(self, ifids):
        for ifid in ifids:
            self.add(ifid, ts=None, offset=None, pipe=None, counter=None)

    def rmv_all(self, remove_state=True):
        for entry_handle in self.entry_hdls:
            self.test.client.ifid_table_delete(self.shdl, self.dev, entry_handle)
        if self.sym:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
            self.test.client.ifid_table_reset_default_entry(self.shdl, dev_tgt)
        else:
            for p in range(self.pipe_list_len):
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
                self.test.client.ifid_table_reset_default_entry(self.shdl, dev_tgt)

        for pipe in self.default_entry_hdls:
            self.test.client.ifid_table_reset_default_entry(self.shdl, DevTarget_t(self.dev, hex_to_i16(pipe)))

        if remove_state:
            self.entry_hdls = []
            self.ifid_to_hdl = {}
            self.ifid_to_data = {}
            self.ifid_to_cntr = {}
            self.default_entry_hdls = {}

    def get_cntrs(self, ifid):
        hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
        if self.sym:
            if ifid is not None:
                eh = self.ifid_to_hdl[(ifid, 0xFFFF)]
            else:
                eh = self.default_entry_hdls[0xFFFF]
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
            counts = self.test.client.register_read_ifid_cntr(self.shdl, dev_tgt, eh, hw_sync_flag)
        else:
            counts = [0]*4
            for pipe in range(self.pipe_list_len):
                if ifid is None or (ifid, pipe) in self.ifid_to_hdl:
                    if ifid is not None:
                        eh = self.ifid_to_hdl[(ifid, pipe)]
                    else:
                        eh = self.default_entry_hdls[pipe]
                    dev_tgt = DevTarget_t(self.dev, pipe)
                    x = self.test.client.register_read_ifid_cntr(self.shdl, dev_tgt, eh, hw_sync_flag)
                    counts[pipe] = x[0]
        if ifid is not None:
            saved_counters = self.ifid_to_cntr[ifid]
        else:
            saved_counters = self.ifid_dflt_cntr
        if self.sym:
            for i in range(self.pipe_list_len):
                if saved_counters[i] != counts[i]:
                    print("IFID counter error:")
                    if ifid is not None:
                        print("ifid", ifid)
                    else:
                        print("Default entry")
                    print("Expected", saved_counters)
                    print("Got     ", counts)
                    sys.stdout.flush()
                    self.test.assertTrue(saved_counters[i] == counts[i])
        else:
            for i in range(len(saved_counters)):
                if saved_counters[i] is None:
                    self.test.assertTrue(counts[i] == 0)
                else:
                    self.test.assertTrue(counts[i] == saved_counters[i])

        return counts

    def verify_cntrs(self):
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
        print(datetime.datetime.now(), "Syncing ifid_cntr register")
        sys.stdout.flush()
        self.test.client.register_hw_sync_ifid_cntr(self.shdl, dev_tgt)
        print(datetime.datetime.now(), "  Done")
        sys.stdout.flush()

        print(datetime.datetime.now(), "Verifying", len(self.ifid_to_hdl), "entries")
        sys.stdout.flush()
        hw_async_flag = stful_register_flags_t(read_hw_sync = False)
        for ifid,pipe in self.ifid_to_hdl:
            if pipe == 0xFFFF:
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
                eh = self.ifid_to_hdl[ (ifid,pipe) ]
                counts = self.test.client.register_read_ifid_cntr(self.shdl, dev_tgt, eh, hw_async_flag)
                saved_counters = self.ifid_to_cntr[ifid]
                if counts != saved_counters:
                    print("IFID counter error: ifid", ifid)
                    print("Expected", saved_counters)
                    print("Got     ", counts)
                    sys.stdout.flush()
                self.test.assertEqual(counts, saved_counters)
            else:
                counts = [0]*4
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
                eh = self.ifid_to_hdl[ (ifid,pipe) ]
                x = self.test.client.register_read_ifid_cntr(self.shdl, dev_tgt, eh, hw_async_flag)
                counts[pipe] = x[0]
                saved_counters = self.ifid_to_cntr[ifid]
                self.test.assertEqual(counts[pipe], saved_counters[pipe])
        for pipe in self.default_entry_hdls:
            eh = self.default_entry_hdls[pipe]
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
            x = self.test.client.register_read_ifid_cntr(self.shdl, dev_tgt, eh, hw_async_flag)
            saved_counters = self.ifid_dflt_cntr
            if pipe == 0xFFFF:
                self.test.assertEqual(x, saved_counters)
            else:
                counts = [0]*4
                counts[pipe] = x[0]
                self.test.assertEqual(counts[pipe], saved_counters[pipe])
        print(datetime.datetime.now(), "  Done")
        sys.stdout.flush()

    def reg_reset(self, pipe=0xFFFF):
        val = 0
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)
        else:
            self.test.assertTrue(pipe >= 0 and pipe < 4)
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        for ifid,pipe in self.ifid_to_hdl:
            eh = self.ifid_to_hdl[(ifid,pipe)]
            self.test.client.register_write_ifid_cntr(self.shdl, self.dev, eh, 0)
        for x in self.ifid_to_cntr:
            if 0xFFFF == pipe:
                self.ifid_to_cntr[x] = []
                if 0 in self.pipe_list:
                    self.ifid_to_cntr[x].append(val)
                if 1 in self.pipe_list:
                    self.ifid_to_cntr[x].append(val)
                if 2 in self.pipe_list:
                    self.ifid_to_cntr[x].append(val)
                if 3 in self.pipe_list:
                    self.ifid_to_cntr[x].append(val)
            else:
                self.ifid_to_cntr[x][pipe] = val

    def get_handle(self, ifid, pipe=0xFFFF):
        return self.ifid_to_hdl[ (ifid, pipe) ]

"""
Sampler Table
==============
"""
class SamplerTable:
    def __init__(self, test, shdl, dev):
        self.test = test
        self.shdl = shdl
        self.dev = dev
        self.tbl_sz = SIP_SAMPLER_TBL_SIZE
        self.reg_sz = SIP_SAMPLER_REG_SIZE
        self.entry_hdls = {}
        self.sip_to_index = {}
        self.index_to_cntr = {}
        self.default_indexes = [None]*4
        self.sym = 1
        num_pipes = int(test_param_get('num_pipes'))
        self.pipe_list = [x for x in range(num_pipes)]
        self.pipe_list_len = len(self.pipe_list)

    def set_sym(self, tf):
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
        if tf == 0 or tf == False:
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
        self.test.client.sip_sampler_set_property(self.shdl, self.dev, prop, prop_val, 0)
        self.sym = tf

    def add(self, sip, pipe=0xFFFF, sample_idx=None):
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)
        else:
            self.test.assertTrue(pipe >= 0 and pipe < 4)
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        if sip is not None:
            if (sip,pipe) in self.entry_hdls:
                #print("Adding", sip, "to pipe", hex(pipe), "but it exists with handle", hex(self.entry_hdls[(sip,pipe)]))
                self.test.assertFalse((sip,pipe) in self.entry_hdls)
                return
            mspec = stful_sip_sampler_match_spec_t( ipv4Addr_to_i32(sip) )
            if sample_idx is not None:
                self.test.assertTrue(sample_idx < 143360)
                aspec = stful_sample_action_spec_t(hex_to_i32(sample_idx))
                eh = self.test.client.sip_sampler_table_add_with_sample(self.shdl, dev_tgt, mspec, aspec)
                #print("Added", sip, "to pipe", hex(pipe), "with index", sample_idx, "handle", hex(eh))
                self.test.assertNotEqual(eh, 0)
            else:
                eh = self.test.client.sip_sampler_table_add_with_no_sample(self.shdl, dev_tgt, mspec)
                self.test.assertNotEqual(eh, 0)
            self.entry_hdls[(sip, pipe)] = eh
            self.sip_to_index[(sip, pipe)] = sample_idx
        else:
            if sample_idx is not None:
                self.test.assertTrue(sample_idx < 143360)
                aspec = stful_sample_action_spec_t(hex_to_i32(sample_idx))
                eh = self.test.client.sip_sampler_set_default_action_sample(self.shdl, dev_tgt, aspec)
            else:
                eh = self.test.client.sip_sampler_set_default_action_no_sample(self.shdl, dev_tgt)
            if self.sym:
                self.default_indexes = [sample_idx]*4
            else:
                self.default_indexes[pipe] = sample_idx
        return eh

    def set_counter_index(self, sip, pipe=0xFFFF, sample_idx=None):
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)
        else:
            self.test.assertTrue(pipe >= 0 and pipe < 4)
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        eh = self.entry_hdls[(sip, pipe)]
        if sample_idx is not None:
            aspec = stful_sample_action_spec_t(hex_to_i32(sample_idx))
            self.test.client.sip_sampler_table_modify_with_sample(self.shdl, self.dev, eh, aspec)
        else:
            self.test.client.sip_sampler_table_modify_with_no_sample(self.shdl, self.dev, eh)
        self.sip_to_index[(sip, pipe)] = sample_idx

    def get_counter_index(self, sip, pipe=0xFFFF):
        if self.sym:
            pipe = 0xFFFF
        self.test.assertIn( (sip,pipe), self.sip_to_index )
        return self.sip_to_index[(sip, pipe)]

    def rmv(self, sip, pipe=0xFFFF):
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)
        else:
            self.test.assertTrue(pipe >= 0 and pipe < 4)
        eh = self.entry_hdls[(sip, pipe)]
        self.test.client.sip_sampler_table_delete(self.shdl, self.dev, hex_to_i32(eh))
        del self.entry_hdls[(sip, pipe)]
        del self.sip_to_index[(sip, pipe)]

    def rmv_all(self, remove_state=True):
        for key in self.entry_hdls:
            eh = self.entry_hdls[key]
            self.test.client.sip_sampler_table_delete(self.shdl, self.dev, hex_to_i32(eh))
        if remove_state:
            self.entry_hdls = {}
            self.sip_to_index = {}
        if self.sym:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
            self.test.client.sip_sampler_table_reset_default_entry(self.shdl, dev_tgt)
        else:
            for p in range(self.pipe_list_len):
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
                self.test.client.sip_sampler_table_reset_default_entry(self.shdl, dev_tgt)

    def sync_cntr(self, pipe=0xFFFF):
        print(datetime.datetime.now(), "Syncing sampling_cntr register")
        sys.stdout.flush()
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        self.test.client.register_hw_sync_sampling_cntr(self.shdl, dev_tgt)
        print(datetime.datetime.now(), "  Done")
        sys.stdout.flush()

    def get_cntr(self, sample_idx, sync=True):
        if sample_idx not in self.index_to_cntr:
            self.index_to_cntr[sample_idx] = [1]*self.pipe_list_len
        expected = self.index_to_cntr[sample_idx]
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
        hw_sync = stful_register_flags_t(read_hw_sync = sync)
        x = self.test.client.register_read_sampling_cntr(self.shdl, dev_tgt, sample_idx, hw_sync)
        self.test.assertEqual(len(x), len(expected))
        for i in range(len(x)):
            if x[i] != expected[i]:
                print("Index", sample_idx, "expected", expected)
                print("Index", sample_idx, "actual  ", x)
                sys.stdout.flush()
            self.test.assertEqual(x[i], expected[i])
        return x

    def inc_cntr(self, sample_idx, pipe):
        self.test.assertTrue(pipe < 4)
        self.test.assertTrue(pipe >= 0)
        len_a = self.pipe_list_len
        if sample_idx not in self.index_to_cntr:
            self.index_to_cntr[sample_idx] = [1]*len_a
        self.index_to_cntr[sample_idx][pipe] += 1
        if self.index_to_cntr[sample_idx][pipe] > 10:
            self.index_to_cntr[sample_idx][pipe] = 1
            return True
        return False

    def set_cntr(self, sample_idx, pipe, val):
        self.test.assertTrue(pipe < 4 or pipe == 0xFFFF)
        self.test.assertTrue(pipe >= 0)
        len_a = self.pipe_list_len
        if sample_idx not in self.index_to_cntr:
            self.index_to_cntr[sample_idx] = [1]*len_a
        if pipe == 0xFFFF:
            self.index_to_cntr[sample_idx] = [val]*len_a
        else:
            self.index_to_cntr[sample_idx][pipe] = val

    def verify_cntrs(self):
        self.sync_cntr()
        print(datetime.datetime.now(), "Verifying", self.reg_sz, "entries")
        sys.stdout.flush()
        for i in range(self.reg_sz):
            self.get_cntr(i, sync=False)
        print(datetime.datetime.now(), "  Done")
        sys.stdout.flush()

    def reset_all_counters(self):
        if self.sym:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
            self.test.client.register_reset_all_sampling_cntr(self.shdl, dev_tgt)
        else:
            for p in range(self.pipe_list_len):
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
                self.test.client.register_reset_all_sampling_cntr(self.shdl, dev_tgt)

    def reg_reset(self, pipe=0xFFFF):
        val = 1
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)
        else:
            self.test.assertTrue(pipe >= 0 and pipe < 4)
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        self.test.client.register_reset_all_sampling_cntr(self.shdl, dev_tgt)
        for x in self.index_to_cntr:
            if 0xFFFF == pipe:
                self.index_to_cntr[x] = []
                for i in range(self.pipe_list_len):
                    self.index_to_cntr[x].append(val)
            else:
                self.index_to_cntr[x][pipe] = val

"""
Bloom Filter
================
 - Setup
   - Add a dummy entry to each of the "prepare" tables to set the hashes
"""
class BloomFilterTbl:
    def __init__(self, test, shdl, dev, symmetric=1):
        self.test = test
        self.shdl = shdl
        self.dev = dev
        self.tbl_sz = BLOOM_FILTER_REG_SIZE
        self.sym = symmetric

    def reg_reset(self, pipe=0xFFFF):
        val = 1
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)
        else:
            self.test.assertTrue(pipe >= 0 and pipe < 4)
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        self.test.client.register_reset_all_bloom_filter_1(self.shdl, dev_tgt)
        self.test.client.register_reset_all_bloom_filter_2(self.shdl, dev_tgt)
        self.test.client.register_reset_all_bloom_filter_3(self.shdl, dev_tgt)





def mask_lower_ip_bits(ip, bits):
    byte_array = [int(b) for b in ip.split('.')]
    ip_int = 0
    for b in byte_array: ip_int = ip_int * 256 + b

    ip_mask = 0xFFFFFFFF >> bits
    ip_mask = ip_mask << bits
    ip_int = ip_int & ip_mask
    ip = "%d.%d.%d.%d" % ((ip_int >> 24) & 0xFF, (ip_int >> 16) & 0xFF, (ip_int >> 8) & 0xFF, (ip_int >> 0) & 0xFF)
    return ip

class RteTbl:
    def __init__(self, test, shdl, dev, symmetric=1):
        self.test = test
        self.shdl = shdl
        self.dev = dev
        self.tbl_sz = IP_ROUTE_TBL_SIZE
        self.sym = symmetric
        self.entry_handles = {}
        self.next_hops = {}

    def set_sym(self, tf):
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
        if tf == 0 or tf == False:
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
        self.test.client.ipv4_route_set_property(self.shdl, self.dev, prop, prop_val, 0)
        self.sym = tf

    def add(self, dip, prefix_len, nh, pipe=0xFFFF, ecmp=False):
        if self.sym:
            self.test.assertEqual(pipe, 0xFFFF)
            pipes = [0xFFFF]
        elif pipe == 0xFFFF:
            pipes = list(range(self.pipe_list_len))
        else:
            self.test.assertIn(pipe, [1,2,3,4])
            pipes = [pipe]
        for p in pipes:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
            mspec = stful_ipv4_route_match_spec_t( ipv4Addr_to_i32(dip), hex_to_i16(prefix_len) )
            aspec = stful_set_next_hop_action_spec_t( hex_to_i32(nh) )
            if ecmp:
                eh = self.test.client.ipv4_route_table_add_with_set_ecmp(self.shdl, dev_tgt, mspec, aspec)
            else:
                eh = self.test.client.ipv4_route_table_add_with_set_next_hop(self.shdl, dev_tgt, mspec, aspec)
            self.test.assertNotEqual(eh, 0)
            dip = mask_lower_ip_bits(dip, 32-prefix_len)
            self.entry_handles[(dip,p)] = eh
            self.next_hops[(dip,p)] = nh
        return eh

    def get_nh(self, ip, pipe=0xFFFF):
        if self.sym:
            pipe = 0xFFFF

        for i in range(33):
            ip_masked = mask_lower_ip_bits(ip, i)
            if (ip_masked,pipe) in self.next_hops:
                return self.next_hops[(ip_masked,pipe)]
        self.test.assertIn( (ip_masked,pipe), self.next_hops )
        return -1

    def rmv_all(self):
        for dip, pipe in self.entry_handles:
            eh = self.entry_handles[(dip,pipe)]
            self.test.client.ipv4_route_table_delete(self.shdl, self.dev, eh)
        self.entry_handles = {}
        self.next_hops = {}
        if self.sym:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
            self.test.client.ipv4_route_table_reset_default_entry(self.shdl, dev_tgt)
        else:
            for p in range(self.pipe_list_len):
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
                self.test.client.ipv4_route_table_reset_default_entry(self.shdl, dev_tgt)





class NextHopTbl:
    def __init__(self, test, shdl, dev, symmetric=1):
        self.test = test
        self.shdl = shdl
        self.dev = dev
        self.tbl_sz = NEXT_HOP_TBL_SIZE
        self.reg_sz = SCRATCH_REG_SIZE
        self.sym = symmetric
        self.entry_handles = {}
        self.nh_to_index = {}
        self.nh_to_ifid = {}
        self.reg_val = []
        num_pipes = int(test_param_get('num_pipes'))
        self.pipe_list = [x for x in range(num_pipes)]
        self.pipe_list_len = len(self.pipe_list)
        for _ in range(SCRATCH_REG_SIZE):
            self.reg_val.append( [0]*self.pipe_list_len )

    def set_sym(self, tf):
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
        if tf == 0 or tf == False:
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
        self.test.client.ipv4_route_set_property(self.shdl, self.dev, prop, prop_val, 0)
        self.sym = tf

    def add(self, next_hop, ifid, mode=None, index=None, pipe=0xFFFF):
        if self.sym:
            self.test.assertEqual(pipe, 0xFFFF)
            pipes = [0xFFFF]
        elif pipe == 0xFFFF:
            pipes = list(range(self.pipe_list_len))
        else:
            self.test.assertIn(pipe, [1,2,3,4])
            pipes = [pipe]
        for p in pipes:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
            priority = next_hop
            mspec = stful_next_hop_match_spec_t(hex_to_i16(next_hop), hex_to_i16(0xFFFF))
            if mode is None:
                aspec = stful_set_egr_ifid_action_spec_t(hex_to_i32(ifid))
                eh = self.test.client.next_hop_table_add_with_set_egr_ifid(self.shdl, dev_tgt, mspec, priority, aspec)
            elif mode == "ADD":
                aspec = stful_scratch_add_action_spec_t(index, hex_to_i32(ifid))
                eh = self.test.client.next_hop_table_add_with_scratch_add(self.shdl, dev_tgt, mspec, priority, aspec)
            elif mode == "SUB":
                aspec = stful_scratch_sub_action_spec_t(index, hex_to_i32(ifid))
                eh = self.test.client.next_hop_table_add_with_scratch_sub(self.shdl, dev_tgt, mspec, priority, aspec)
            elif mode == "INVERT":
                aspec = stful_scratch_invert_action_spec_t(index, hex_to_i32(ifid))
                eh = self.test.client.next_hop_table_add_with_scratch_invert(self.shdl, dev_tgt, mspec, priority, aspec)
            elif mode == "ZERO":
                aspec = stful_scratch_zero_action_spec_t(index, hex_to_i32(ifid))
                eh = self.test.client.next_hop_table_add_with_scratch_zero(self.shdl, dev_tgt, mspec, priority, aspec)
            else:
                self.test.assertTrue(False)
            self.nh_to_index[(next_hop,pipe)] = (index,mode)
            self.nh_to_ifid[(next_hop,pipe)] = ifid
            self.entry_handles[(next_hop,pipe)] = eh

    def get_handle(self, next_hop, pipe=0xFFFF):
        return self.entry_handles[(next_hop,pipe)]

    def set_down(self, next_hop, mgid, pipe=0xFFFF):
        if self.sym:
            self.test.assertEqual(pipe, 0xFFFF)
        hdl = self.entry_handles[ (next_hop, pipe) ]
        aspec = stful_next_hop_down_action_spec_t( hex_to_i32(mgid) )
        self.test.client.next_hop_table_modify_with_next_hop_down(self.shdl, self.dev, hdl, aspec)

    def get_ifid(self, next_hop, pipe=0xFFFF):
        if self.sym:
            pipe = 0xFFFF
        self.test.assertIn( (next_hop,pipe), self.nh_to_ifid )
        return self.nh_to_ifid[ (next_hop,pipe) ]

    def get_index(self, next_hop, pipe=0xFFFF):
        if self.sym:
            pipe = 0xFFFF
        self.test.assertIn( (next_hop,pipe), self.nh_to_index )
        return self.nh_to_index[ (next_hop,pipe) ]

    def update_cntr(self, next_hop, pipe):
        self.test.assertIn(pipe, list(range(self.pipe_list_len)))
        idx, mode = self.get_index(next_hop, pipe)
        if mode == 'ADD':
            self.reg_val[idx][pipe] = (self.reg_val[idx][pipe] + next_hop) & 0xFFFF
        elif mode == 'SUB':
            self.reg_val[idx][pipe] = (next_hop - self.reg_val[idx][pipe]) & 0xFFFF
        elif mode == 'INVERT':
            self.reg_val[idx][pipe] = (~self.reg_val[idx][pipe]) & 0xFFFF
        elif mode == 'ZERO':
            self.reg_val[idx][pipe] = 0
        else:
            self.test.assertIn( mode, ['ADD', 'SUB', 'INVERT', 'ZERO'] )

    def rmv_all(self):
        for nh,pipe in self.entry_handles:
            eh = self.entry_handles[(nh,pipe)]
            self.test.client.next_hop_table_delete(self.shdl, self.dev, eh)
        self.entry_handles = {}
        self.nh_to_index = {}
        self.nh_to_ifid = {}
        if self.sym:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
            self.test.client.next_hop_table_reset_default_entry(self.shdl, dev_tgt)
        else:
            for p in range(self.pipe_list_len):
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
                self.test.client.next_hop_table_reset_default_entry(self.shdl, dev_tgt)

    def sync_cntrs(self, pipe=0xFFFF):
        print(datetime.datetime.now(), "Syncing scratch register")
        sys.stdout.flush()
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        self.test.client.register_hw_sync_scratch(self.shdl, dev_tgt)
        print(datetime.datetime.now(), "  Done")
        sys.stdout.flush()

    def verify_cntrs(self):
        self.sync_cntrs()
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
        async_flag = stful_register_flags_t(read_hw_sync = False)
        print(datetime.datetime.now(), "Verifying", self.reg_sz, "entries")
        sys.stdout.flush()
        for i in range(self.reg_sz):
            reg = self.test.client.register_read_scratch(self.shdl, dev_tgt, i, async_flag)
            x = [i16_to_hex(r) for r in reg]
            self.test.assertEqual( len(self.reg_val[i]), len(x) )
            if self.reg_val[i] != x:
                print("Scratch counter mismatch at index", i)
            self.test.assertEqual( self.reg_val[i], x )
        print(datetime.datetime.now(), "  Done")
        sys.stdout.flush()

    def reg_reset(self, pipe=0xFFFF):
        val = 0
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)
        else:
            self.test.assertTrue(pipe >= 0 and pipe < 4)
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        self.test.client.register_reset_all_scratch(self.shdl, dev_tgt)
        for x in range( len(self.reg_val) ):
            if 0xFFFF == pipe:
                self.reg_val[x] = []
                for i in range(self.pipe_list_len):
                    self.reg_val[x].append(val)
            else:
                self.reg_val[x][pipe] = val

class EgressPortTbl:
    def __init__(self, test, shdl, dev, symmetric=1):
        self.test = test
        self.shdl = shdl
        self.dev = dev
        self.tbl_sz = EGR_PORT_TBL_SIZE
        self.sym = symmetric
        self.entry_handles = {}
        self.reg_vals = {}
        self.ifid_to_port = {}
        num_pipes = int(test_param_get('num_pipes'))
        self.pipe_list = [x for x in range(num_pipes)]
        self.pipe_list_len = len(self.pipe_list)

    def set_sym(self, tf):
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
        if tf == 0 or tf == False:
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
        self.test.client.ipv4_route_set_property(self.shdl, self.dev, prop, prop_val, 0)
        self.sym = tf

    def add(self, egr_ifid, port, pipe=0xFFFF, cntr=0):
        reg = stful_port_cntr_value_t(f0=cntr>>32, f1=cntr&0xFFFFFFFF);
        if self.sym:
            self.test.assertEqual(pipe, 0xFFFF)
            pipes = [0xFFFF]
        elif pipe == 0xFFFF:
            pipes = list(range(self.pipe_list_len))
        else:
            self.test.assertIn(pipe, [1,2,3,4])
            pipes = [pipe]
        for p in pipes:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
            priority = port
            mspec = stful_egr_port_match_spec_t(hex_to_i16(egr_ifid), hex_to_i16(0xFFFF))
            priority = port
            aspec = stful_set_dest_action_spec_t(hex_to_i32(port))
            eh = self.test.client.egr_port_table_add_with_set_dest(self.shdl, dev_tgt, mspec, priority, aspec, reg)
            self.entry_handles[(egr_ifid,pipe)] = eh
            if self.sym:
                self.reg_vals[eh] = [cntr]*self.pipe_list_len
            else:
                self.reg_vals[eh] = [cntr]
            self.ifid_to_port[(egr_ifid,pipe)] = port
            return eh

    def rmv_all(self):
        for egr_ifid,pipe in self.entry_handles:
            eh = self.entry_handles[(egr_ifid,pipe)]
            self.test.client.egr_port_table_delete(self.shdl, self.dev, eh)
        self.entry_handles = {}
        self.reg_vals = {}
        self.ifid_to_port = {}
        if self.sym:
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(0xFFFF))
            self.test.client.egr_port_table_reset_default_entry(self.shdl, dev_tgt)
        else:
            for p in range(self.pipe_list_len):
                dev_tgt = DevTarget_t(self.dev, hex_to_i16(p))
                self.test.client.egr_port_table_reset_default_entry(self.shdl, dev_tgt)

    def get_port(self, egr_ifid, pipe=0xFFFF):
        if self.sym:
            pipe = 0xFFFF
        self.test.assertIn( (egr_ifid,pipe), self.ifid_to_port )
        return self.ifid_to_port[(egr_ifid,pipe)]

    def update_cntr(self, egr_ifid, pipe, val):
        self.test.assertIn(pipe, list(range(self.pipe_list_len)))
        if self.sym:
            eh = self.entry_handles[(egr_ifid,0xFFFF)]
            self.reg_vals[eh][pipe] += val
        else:
            eh = self.entry_handles[(egr_ifid,pipe)]
            self.reg_vals[eh][0] += val

    def sync_cntrs(self, pipe=0xFFFF):
        print(datetime.datetime.now(), "Syncing port_cntr register")
        sys.stdout.flush()
        dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
        self.test.client.register_hw_sync_port_cntr(self.shdl, dev_tgt)
        print(datetime.datetime.now(), "  Done")
        sys.stdout.flush()

    def verify_cntrs(self):
        self.sync_cntrs()
        async_flag = stful_register_flags_t(read_hw_sync = False)
        print(datetime.datetime.now(), "Verifying", len(self.entry_handles), "entries")
        sys.stdout.flush()
        for ifid,pipe in self.entry_handles:
            eh = self.entry_handles[(ifid,pipe)]
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
            reg = self.test.client.register_read_port_cntr(self.shdl, dev_tgt, eh, async_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in reg]
            self.test.assertEqual( len(self.reg_vals[eh]), len(x) )
            self.test.assertEqual( self.reg_vals[eh], x )
        print(datetime.datetime.now(), "  Done")
        sys.stdout.flush()

    def reg_reset(self, pipe=0xFFFF):
        val = 0
        if self.sym:
            self.test.assertTrue(pipe == 0xFFFF)
        else:
            self.test.assertTrue(pipe >= 0 and pipe < 4)

        for egr_ifid,pipe in self.entry_handles:
            eh = self.entry_handles[(egr_ifid,pipe)]
            dev_tgt = DevTarget_t(self.dev, hex_to_i16(pipe))
            reg = stful_port_cntr_value_t(f0=0, f1=0);
            self.test.client.register_write_port_cntr(self.shdl, self.dev, eh, reg)

        for x in self.reg_vals:
            if 0xFFFF == pipe:
                self.reg_vals[x] = []
                for i in range(self.pipe_list_len):
                    self.reg_vals[x].append(val)
            else:
                self.reg_vals[x] = [val]





class TestPktGenClear(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        try:
            sess_hdl = self.conn_mgr.client_init()
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            self.num_pipes = int(test_param_get('num_pipes'))
            self.pipe_list = [x for x in range(self.num_pipes)]
            self.pipe_list_len = len(self.pipe_list)
            pkt_len = 64

            set_vals = [1]*self.pipe_list_len
            clr_vals = [0]*self.pipe_list_len
            # Write register
            self.conn_mgr.begin_batch(sess_hdl)
            self.client.register_write_all_bloom_filter_1(sess_hdl, dev_tgt, 1)
            self.client.register_write_all_bloom_filter_2(sess_hdl, dev_tgt, 1)
            self.client.register_write_all_bloom_filter_3(sess_hdl, dev_tgt, 1)
            self.conn_mgr.end_batch(sess_hdl, True)
            # Read a few register values to make sure they are all set
            sync = stful_register_flags_t(read_hw_sync = True)
            r0 = self.client.register_read_bloom_filter_1(sess_hdl, dev_tgt, 0, sync)
            r1 = self.client.register_read_bloom_filter_2(sess_hdl, dev_tgt, 1, sync)
            r2 = self.client.register_read_bloom_filter_3(sess_hdl, dev_tgt, 2, sync)
            r3 = self.client.register_read_bloom_filter_1(sess_hdl, dev_tgt, 0+0x10000, sync)
            r4 = self.client.register_read_bloom_filter_2(sess_hdl, dev_tgt, 1+0x10000, sync)
            r5 = self.client.register_read_bloom_filter_3(sess_hdl, dev_tgt, 2+0x10000, sync)
            self.assertEqual(r0, set_vals)
            self.assertEqual(r1, set_vals)
            self.assertEqual(r2, set_vals)
            self.assertEqual(r3, set_vals)
            self.assertEqual(r4, set_vals)
            self.assertEqual(r5, set_vals)

            # Setup and run pkt-gen
            pkt_gen_cnts = []
            for p in range(self.pipe_list_len):
                self.conn_mgr.pktgen_enable( sess_hdl, dev_id, make_port(p,68) )
                pkt_gen_cnts.append( self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, DevTarget_t(dev_id,p), 3) )
            print("Initial pkt-gen pkt counts:", pkt_gen_cnts)
            self.conn_mgr.begin_batch(sess_hdl)
            pktgen_app = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.TIMER_ONE_SHOT,
                                                batch_count=1, pkt_count=1,
                                                pattern_key=0, pattern_msk=0,
                                                timer=0,
                                                ibg=0, ibg_jitter=0,
                                                ipg=0, ipg_jitter=0,
                                                src_port=0, src_port_inc=0,
                                                buffer_offset=0, length=64 )
            self.conn_mgr.pktgen_app_disable( sess_hdl, dev_tgt, 3 )
            pkt = simple_udp_packet(pktlen=pkt_len)
            self.conn_mgr.pktgen_write_pkt_buffer( sess_hdl, dev_tgt, 0, pkt_len, bytes(pkt) )
            self.conn_mgr.pktgen_cfg_app( sess_hdl, dev_tgt, 3, pktgen_app)
            self.conn_mgr.flush_batch(sess_hdl)
            #self.conn_mgr.pktgen_app_enable( sess_hdl, DevTarget_t(dev_id,0), 3 )
            self.conn_mgr.pktgen_app_enable( sess_hdl, dev_tgt, 3 )
            self.conn_mgr.end_batch(sess_hdl, True)

            final_pkt_gen_cnts = [0] * self.pipe_list_len
            while [final_pkt_gen_cnts[i] - pkt_gen_cnts[i] for i in range(self.pipe_list_len)] != [4] * self.pipe_list_len:
                for p in range(self.pipe_list_len):
                    final_pkt_gen_cnts[p] = self.conn_mgr.pktgen_get_pkt_counter(sess_hdl, DevTarget_t(dev_id,p), 3)
            print("Final pkt-gen pkt counts:", final_pkt_gen_cnts)

            time.sleep(5)

            # Read register
            print("Syncing Registers")
            print(datetime.datetime.now(), "Syncing way 1...")
            self.client.register_hw_sync_bloom_filter_1(sess_hdl, dev_tgt)
            print(datetime.datetime.now(), "Syncing way 2...")
            self.client.register_hw_sync_bloom_filter_2(sess_hdl, dev_tgt)
            print(datetime.datetime.now(), "Syncing way 3...")
            self.client.register_hw_sync_bloom_filter_3(sess_hdl, dev_tgt)
            print(datetime.datetime.now(), "Done syncing registers")
            sys.stdout.flush()
            sync = stful_register_flags_t(read_hw_sync = False)
            for idx in range(BLOOM_FILTER_REG_SIZE):
                r0 = self.client.register_read_bloom_filter_1(sess_hdl, dev_tgt, idx, sync)
                r1 = self.client.register_read_bloom_filter_2(sess_hdl, dev_tgt, idx, sync)
                r2 = self.client.register_read_bloom_filter_3(sess_hdl, dev_tgt, idx, sync)
                if idx == 0 or idx == 1 or idx == 0x10000 or idx == 0x10001:
                    if r0 != clr_vals or r1 != clr_vals or r2 != clr_vals:
                        print("Index", idx, "was not cleared")
                        sys.stdout.flush()
                    self.assertEqual(r0, clr_vals)
                    self.assertEqual(r1, clr_vals)
                    self.assertEqual(r2, clr_vals)
                else:
                    if r0 != set_vals or r1 != set_vals or r2 != set_vals:
                        print("Index", idx, "was not set")
                        sys.stdout.flush()
                    self.assertEqual(r0, set_vals)
                    self.assertEqual(r1, set_vals)
                    self.assertEqual(r2, set_vals)
            print(datetime.datetime.now(), "Done verifying registers")

        finally:
            self.conn_mgr.begin_batch(sess_hdl)
            self.client.register_reset_all_bloom_filter_1(sess_hdl, dev_tgt)
            self.client.register_reset_all_bloom_filter_2(sess_hdl, dev_tgt)
            self.client.register_reset_all_bloom_filter_3(sess_hdl, dev_tgt)
            self.conn_mgr.pktgen_app_disable( sess_hdl, dev_tgt, 3 )
            self.conn_mgr.end_batch(sess_hdl, True)
            self.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))

class TestTwoInstrNoIdx(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        print()
        try:
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)
            pkt = simple_udp_packet(eth_dst='00:FF:FF:FF:FF:FF', pktlen=80)

            pipe = portToPipe(swports[0])

            # Set port 0 with ifid 0 to drop
            port_tbl.add(swports[0], 0)
            ifid_tbl.add(0)

            # Initialize the register with a known value
            self.client.register_write_two_instr_no_idx_reg(sess_hdl, dev_tgt, swports[0], 0)

            # Send two packets to match two match entries and run both instructions
            mspec = stful_two_instr_no_idx_match_spec_t(hex_to_i16(swports[0]))
            eh = self.client.two_instr_no_idx_table_add_with_add_9(sess_hdl, dev_tgt, mspec)
            self.conn_mgr.complete_operations(sess_hdl)
            send_packet(self, swports[0], pkt)
            verify_packets(self, pkt, [swports[0]])

            x = self.client.register_read_two_instr_no_idx_reg(sess_hdl, dev_tgt, swports[0], stful_register_flags_t(read_hw_sync = True) )
            self.assertEqual(x[pipe], 9)

            self.client.two_instr_no_idx_table_modify_with_rsub_17(sess_hdl, dev_id, eh)
            self.conn_mgr.complete_operations(sess_hdl)
            send_packet(self, swports[0], pkt)
            verify_packets(self, pkt, [swports[0]])

            x = self.client.register_read_two_instr_no_idx_reg(sess_hdl, dev_tgt, swports[0], stful_register_flags_t(read_hw_sync = True) )
            self.assertEqual(x[pipe], 17-9)

        finally:
            port_tbl.rmv_all()
            ifid_tbl.rmv_all()
            x = self.client.two_instr_no_idx_get_entry_count(sess_hdl, dev_tgt)
            for _ in range(x):
                h = self.client.two_instr_no_idx_get_first_entry_handle(sess_hdl, dev_tgt)
                self.client.two_instr_no_idx_table_delete(sess_hdl, dev_id, h)
            self.client.register_write_two_instr_no_idx_reg(sess_hdl, dev_tgt, swports[0], 0)
            self.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))

class TestOneBit(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        print()
        try:
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)

            # Set default action entries for tables with stateful
            self.client.one_bit_read_1_set_default_action_run_one_bit_read_1(sess_hdl, dev_tgt)
            self.client.one_bit_read_2_set_default_action_run_one_bit_read_2(sess_hdl, dev_tgt)

            # Set port 0 with ifid 0 to drop
            port_tbl.add(swports[-1], 0)
            ifid_tbl.add(0)
            self.conn_mgr.complete_operations(sess_hdl)
            x = self.client.register_read_ob1(sess_hdl, dev_tgt, 1, stful_register_flags_t(read_hw_sync = True) )
            print("Reg ob1[1] =", x)
            x = self.client.register_read_ob2(sess_hdl, dev_tgt, 2, stful_register_flags_t(read_hw_sync = True) )
            print("Reg ob2[2] =", x)
            sys.stdout.flush()

            pkt = simple_udp_packet(eth_dst='00:DD:DD:DD:DD:DD', pktlen=100)
            send_packet(self, swports[-1], pkt)
            verify_no_other_packets(self)

            # Set all one bit tables to have a value of 1
            self.client.register_write_ob1(sess_hdl, dev_tgt, 1, 1)
            self.client.register_write_ob2(sess_hdl, dev_tgt, 2, 1)
            self.conn_mgr.complete_operations(sess_hdl)
            x = self.client.register_read_ob1(sess_hdl, dev_tgt, 1, stful_register_flags_t(read_hw_sync = True) )
            print("Reg ob1[1] =", x)
            x = self.client.register_read_ob2(sess_hdl, dev_tgt, 2, stful_register_flags_t(read_hw_sync = True) )
            print("Reg ob2[2] =", x)
            sys.stdout.flush()

            pkt = simple_udp_packet(eth_dst='00:FF:FF:FF:FF:FF', pktlen=80)
            send_packet(self, swports[-1], pkt)
            verify_packets(self, pkt, [swports[-1]])
            verify_no_other_packets(self)

        finally:
            port_tbl.rmv_all()
            ifid_tbl.rmv_all()
            self.client.register_write_ob1(sess_hdl, dev_tgt, 1, 0)
            self.client.register_write_ob2(sess_hdl, dev_tgt, 2, 0)
            self.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))

class TestDirectHashCounter(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        print()
        wait_time = 2*len(swports)
        try:
            num_pipes = int(test_param_get('num_pipes'))
            pipe_list = [x for x in range(num_pipes)]
            pipe_list_len = len(pipe_list)
            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)


            # Use one ifid for each port
            test_ifids = []
            ifid = 0x7000
            for _ in swports:
                test_ifids.append(ifid)
                ifid += 1

            # Add port table entries
            for port,ifid in zip(swports, test_ifids):
                port_tbl.add(port, ifid)

            # Add ifid table entries
            for ifid in test_ifids:
                ifid_tbl.add(ifid, counter=ifid)
            self.conn_mgr.complete_operations(sess_hdl)

            # Read each of the 16 counters
            for ifid in test_ifids:
                x = ifid_tbl.get_cntrs(ifid)

            # Modify the entries and read them again
            for ifid in test_ifids:
                ifid_tbl.mod(ifid, counter=ifid+1)
            self.conn_mgr.complete_operations(sess_hdl)
            for ifid in test_ifids:
                x = ifid_tbl.get_cntrs(ifid)

            # Send packets on each port
            count_per_pkt = 5
            pkt = simple_udp_packet(pktlen=64,
                                    eth_dst='00:DD:DD:DD:DD:DD',
                                    eth_src='55:55:55:55:55:55',
                                    ip_src='1.1.1.1',
                                    ip_dst='1.1.1.2',
                                    ip_ttl=count_per_pkt,
                                    udp_sport=0x1234,
                                    udp_dport=0x432)
            packet_per_port = 10
            for port in swports:
                for _ in range(packet_per_port):
                    send_packet(self, port, pkt)
                    ifid_tbl.inc_cntr(port_tbl.get_ifid(port), port_to_pipe(port), count_per_pkt)

            # All packets should be dropped.
            print("All packets sent, waiting...")
            time.sleep(wait_time)
            print("Checking no packets came back...")
            sys.stdout.flush()
            verify_no_other_packets(self)

            # Read each of the 16 counters
            print("Checking IFID counters")
            for ifid in test_ifids:
                x = ifid_tbl.get_cntrs(ifid)

            # Fill tables
            print("Adding more entries")
            sys.stdout.flush()
            self.conn_mgr.begin_batch(sess_hdl)
            extra_ifids = list(range(20000))
            ifid_tbl.add_some(extra_ifids)
            print("  Ending batch")
            sys.stdout.flush()
            self.conn_mgr.end_batch(sess_hdl, True)

            # Send more packets
            count_per_pkt = 10
            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_udp_packet(pktlen=64,
                                    eth_dst='00:DD:DD:DD:DD:DD',
                                    eth_src='55:55:55:55:55:55',
                                    ip_src='1.1.1.1',
                                    ip_dst='1.1.1.2',
                                    ip_ttl=count_per_pkt,
                                    udp_sport=0x1234,
                                    udp_dport=0x432)
            packet_per_port = 3
            for port in swports:
                for _ in range(packet_per_port):
                    send_packet(self, port, pkt)
                    ifid_tbl.inc_cntr(port_tbl.get_ifid(port), port_to_pipe(port), count_per_pkt)

            # All packets should be dropped.
            print("All packets sent, waiting...")
            time.sleep(wait_time)
            print("Checking no packets came back...")
            sys.stdout.flush()
            verify_no_other_packets(self)

            # Read all counters
            print("Checking IFID counters")
            sys.stdout.flush()
            for ifid in test_ifids:
                x = ifid_tbl.get_cntrs(ifid)

            print("Passed")
            sys.stdout.flush()

            # Add a default entry.
            print("Testing default entry")
            sys.stdout.flush()
            self.conn_mgr.begin_batch(sess_hdl)
            ifid_tbl.rmv_all()
            ifid_tbl.add(None, counter = 3)
            self.conn_mgr.end_batch(sess_hdl, True)
            print("  Checking value after add")
            sys.stdout.flush()
            x = ifid_tbl.get_cntrs(None)

            # Send more packets
            print("  Sending traffic for default entry")
            sys.stdout.flush()
            count_per_pkt = 10
            pkt = simple_udp_packet(pktlen=64,
                                    eth_dst='00:DD:DD:DD:DD:EE',
                                    eth_src='55:55:55:55:55:55',
                                    ip_src='1.1.1.1',
                                    ip_dst='1.1.1.2',
                                    ip_ttl=count_per_pkt,
                                    udp_sport=0x1234,
                                    udp_dport=0x432)
            packet_per_port = 1
            for port in swports:
                for _ in range(packet_per_port):
                    send_packet(self, port, pkt)
                    ifid_tbl.inc_cntr(None, port_to_pipe(port), count_per_pkt)

            # All packets should be dropped.
            print("All packets sent, waiting...")
            time.sleep(wait_time)
            print("Checking no packets came back...")
            sys.stdout.flush()
            verify_no_other_packets(self)

            # Read all counters
            print("  Checking value after traffic")
            sys.stdout.flush()
            x = ifid_tbl.get_cntrs(None)

            print("  Modifying default entry to have new counter value")
            sys.stdout.flush()
            ifid_tbl.mod(None, counter = 3)
            self.conn_mgr.complete_operations(sess_hdl)
            x = ifid_tbl.get_cntrs(None)

            print("  Modifying default entry to have new counter value (again)")
            sys.stdout.flush()
            ifid_tbl.add(None, counter = 2)
            self.conn_mgr.complete_operations(sess_hdl)
            x = ifid_tbl.get_cntrs(None)

            print("Switching to asymmetric table")
            sys.stdout.flush()
            self.conn_mgr.begin_batch(sess_hdl)
            port_tbl.rmv_all()
            ifid_tbl.rmv_all()
            self.conn_mgr.end_batch(sess_hdl, True)


            port_tbl.set_sym(0)
            ifid_tbl.set_sym(0)

            pipe_ifids = [0x7777, 0x7888, 0x7999, 0x7AAA]
            for pipe in pipe_list:
                ifid_tbl.add(pipe_ifids[pipe], counter=pipe_ifids[pipe], pipe=pipe)
            for port in swports:
                pipe = port_to_pipe(port)
                port_tbl.add(port, pipe_ifids[pipe])
            self.conn_mgr.complete_operations(sess_hdl)

            print("Checking IFID counters")
            sys.stdout.flush()
            for i in range(len(pipe_list)):
                ifid = pipe_ifids[i]
                x = ifid_tbl.get_cntrs(ifid)
                for j in range(len(pipe_list)):
                    if i == j:
                        self.assertTrue(x[j] == ifid)
                    else:
                        self.assertTrue(x[j] == 0)

        finally:
            port_tbl.rmv_all()
            ifid_tbl.rmv_all()
            port_tbl.set_sym(1)
            ifid_tbl.set_sym(1)
            self.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))




class TestIndirectHashSampler(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        print()
        try:
            test_ports = []
            for pipe in pipe_list:
                for port in swports:
                    if pipe == port_to_pipe(port):
                        test_ports.append(port)
                        break

            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)
            smpl_tbl = SamplerTable(self, sess_hdl, dev_id)

            hw_sync = stful_register_flags_t(read_hw_sync = True)

            # Reset the sampler table to the initial value.
            print("Reset sampler register")
            sys.stdout.flush()
            smpl_tbl.reset_all_counters()
            print("  Done")
            sys.stdout.flush()

            # Add an entry for every port
            test_ifid = 0x1
            for port in test_ports:
                port_tbl.add(port, test_ifid)

            # Add the IFID entry
            ifid_tbl.add(test_ifid, ts=0, offset=0)

            # Add dummy IPv4 route entry
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_next_hop_action_spec_t(hex_to_i32(0))
            self.client.ipv4_route_set_default_action_set_next_hop(sess_hdl, dev_tgt, aspec)
            # Add a dummy next hop entry
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_egr_ifid_action_spec_t(hex_to_i32(0))
            self.client.next_hop_set_default_action_set_egr_ifid(sess_hdl, dev_tgt, aspec)
            # Add a dummy egress port entry
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_dest_action_spec_t(hex_to_i32(test_ports[0]))
            self.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt, aspec, stful_port_cntr_value_t(f0=0,f1=0))

            # Set a default entry to point to a counter.
            smpl_tbl.add(None, sample_idx=SIP_SAMPLER_REG_SIZE-1)
            print("Complete operations")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)
            print("  Done")
            sys.stdout.flush()

            # Send traffic to hit the default entry
            for port in test_ports:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='0.0.0.0',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                print("Sending on port", port,"to hit default entry")
                sys.stdout.flush()
                send_packet(self, port, pkt)
                print("  Checking Rx on port", test_ports[0])
                sys.stdout.flush()
                verify_packet(self, pkt, test_ports[0])
                pipe = port_to_pipe(port)
                smpl_tbl.inc_cntr(SIP_SAMPLER_REG_SIZE-1, pipe)

                x = smpl_tbl.get_cntr( SIP_SAMPLER_REG_SIZE-1 )
            verify_no_other_packets(self)

            # Set a default entry to point to no counter.
            smpl_tbl.add(None)
            self.conn_mgr.complete_operations(sess_hdl)
            # Send traffic to hit the default entry
            for port in test_ports:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='0.0.0.0',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                print("Sending on port", port,"to hit default entry w/o register")
                sys.stdout.flush()
                send_packet(self, port, pkt)
                print("  Checking Rx on port", test_ports[0])
                sys.stdout.flush()
                verify_packet(self, pkt, test_ports[0])
                x = smpl_tbl.get_cntr( SIP_SAMPLER_REG_SIZE-1 )
            verify_no_other_packets(self)

            # Add a few sampler entry with a few stateful pointer values.
            sample_idxs = []
            # Use the last index in the table
            sample_idxs.append(SIP_SAMPLER_REG_SIZE-1)
            # Use a few arbitrary locations
            sample_idxs.extend([20, 1000, 100000, 135000])
            # Use the first row in each of the 35 RAMs
            for ram in range(35):
                base = 1024*4 # 4k 32 bit entries per RAM.
                entry = random.randint(0,3) # One of the four 32-bit entries per row
                sample_idxs.append(ram*base+entry)

            print("Adding", len(sample_idxs), "sampler table entries")
            sys.stdout.flush()
            sip_index_pairs = []
            for i in range(len(sample_idxs)):
                idx = sample_idxs[i]
                ip = "10.1.%d.%d" % (i >> 8, i & 0xFF)
                smpl_tbl.add(ip, sample_idx=idx)
                sip_index_pairs.append( (ip, idx) )

            print("Complete operations")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)
            print("  Done")
            sys.stdout.flush()

            for port in test_ports:
                pipe = port_to_pipe(port)
                print("Sending to port", port, "in pipe", pipe)
                sys.stdout.flush()
                for ip,idx in sip_index_pairs:
                    #print("Sending with", ip)
                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_src=ip,
                                            ip_dst='10.0.0.1',
                                            ip_id=101,
                                            ip_ttl=64)
                    send_packet(self, port, pkt)
                    verify_packet(self, pkt, test_ports[0])
                    smpl_tbl.inc_cntr(idx, pipe)

                    x = smpl_tbl.get_cntr( idx )
                    #print("Index", idx, "--", x)

            verify_no_other_packets(self)

            # Read all the allocated indexes and verify their counts
            print("Verifying counters:", sample_idxs)
            sys.stdout.flush()
            for idx in sample_idxs:
                print("  Checking", idx)
                sys.stdout.flush()
                x = smpl_tbl.get_cntr( idx )
            print("  Done")
            sys.stdout.flush()

            # Let all entries point to one of two counters
            print("Changing all entries to point to one of two registers")
            sys.stdout.flush()
            for i in range( len(sip_index_pairs) ):
                ip,idx = sip_index_pairs[i]
                if i & 1:
                    new_idx = SIP_SAMPLER_REG_SIZE-1
                else:
                    new_idx = 0
                smpl_tbl.set_counter_index(ip, sample_idx=new_idx)
                sip_index_pairs[i] = (ip, new_idx)
            print("  Complete operations")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)
            print("  Done")
            sys.stdout.flush()

            # Send traffic again and verify counts
            for port in test_ports:
                pipe = port_to_pipe(port)
                print("Sending to port", port, "in pipe", pipe)
                sys.stdout.flush()
                for ip,idx in sip_index_pairs:
                    #print("Sending with", ip, "for index", idx, "in pipe", pipe)
                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_src=ip,
                                            ip_dst='10.0.0.1',
                                            ip_id=101,
                                            ip_ttl=64)
                    send_packet(self, port, pkt)
                    verify_packet(self, pkt, test_ports[0])
                    smpl_tbl.inc_cntr(idx, pipe)

                    x = smpl_tbl.get_cntr( idx )
                    #print("Index", idx, "--", x)

            verify_no_other_packets(self)

            # Sync the entire table and read every entry
            print("Verifying all counters")
            sys.stdout.flush()
            smpl_tbl.verify_cntrs()
            print("  Done")
            sys.stdout.flush()

            # Let all entries point to no counter
            print("Modify all entries to not use a register")
            sys.stdout.flush()
            for i in range( len(sip_index_pairs) ):
                ip,idx = sip_index_pairs[i]
                new_idx = None
                smpl_tbl.set_counter_index(ip, sample_idx=new_idx)
                sip_index_pairs[i] = (ip, new_idx)
            print("  Complete operations")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)
            print("  Done")
            sys.stdout.flush()

            # Send traffic again and verify counts
            for port in test_ports:
                pipe = port_to_pipe(port)
                print("Sending to port", port, "in pipe", pipe)
                sys.stdout.flush()
                for ip,idx in sip_index_pairs:
                    #print("Sending with", ip)
                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_src=ip,
                                            ip_dst='10.0.0.1',
                                            ip_id=101,
                                            ip_ttl=64)
                    send_packet(self, port, pkt)
                    verify_packet(self, pkt, test_ports[0])
            verify_no_other_packets(self)

            # Sync the entire table and read every entry
            print("Verifying all counters")
            sys.stdout.flush()
            smpl_tbl.verify_cntrs()
            print("  Done")
            sys.stdout.flush()

            # Delete all entries and change to asymmetric mode
            print("Removing all entries")
            sys.stdout.flush()
            self.conn_mgr.begin_batch(sess_hdl)
            smpl_tbl.rmv_all()
            self.conn_mgr.end_batch(sess_hdl, False)
            print("Setting asymmetric mode")
            sys.stdout.flush()
            smpl_tbl.set_sym(0)
            # Add to one pipe
            smpl_tbl.add("3.3.3.3", pipe=0, sample_idx=123)
            # Set defaults in each pipe
            if 0 in pipe_list:
                smpl_tbl.add(None, pipe=0, sample_idx=1)
            if 1 in pipe_list:
                smpl_tbl.add(None, pipe=1, sample_idx=2)
            if 2 in pipe_list:
                smpl_tbl.add(None, pipe=2, sample_idx=3)
            if 3 in pipe_list:
                smpl_tbl.add(None, pipe=3, sample_idx=4)
            self.conn_mgr.complete_operations(sess_hdl)
            # Send packets
            for port in test_ports:
                pipe = port_to_pipe(port)
                print("Sending to port", port, "in pipe", pipe)
                sys.stdout.flush()
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='3.3.3.3',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                send_packet(self, port, pkt)
                verify_packet(self, pkt, test_ports[0])
                if pipe == 0:
                    smpl_tbl.inc_cntr(123, pipe)
                else:
                    smpl_tbl.inc_cntr(pipe+1, pipe)
            verify_no_other_packets(self)
            # Ensure only that pipe has counts and other pipes counted on default
            smpl_tbl.verify_cntrs()

            print("Cleaning up after success")
            sys.stdout.flush()

        finally:
            print("Starting Cleanup")
            sys.stdout.flush()
            self.conn_mgr.begin_batch(sess_hdl)
            port_tbl.rmv_all()
            ifid_tbl.rmv_all()
            self.conn_mgr.end_batch(sess_hdl, False)
            ifid_tbl.set_sym(1)
            self.conn_mgr.begin_batch(sess_hdl)
            smpl_tbl.rmv_all()
            self.conn_mgr.end_batch(sess_hdl, False)
            smpl_tbl.set_sym(1)
            self.conn_mgr.begin_batch(sess_hdl)
            smpl_tbl.reset_all_counters()
            self.conn_mgr.end_batch(sess_hdl, True)
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            self.client.sip_sampler_set_default_action_no_sample(sess_hdl, dev_tgt)
            self.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))




def direct_tcam_setup(test, port_list, ifid, port_tbl):
    sess_hdl = test.conn_mgr.client_init()

    for port in port_list:
        port_tbl.add(port, ifid)

    # Add dummy sip sampler entries
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    test.client.sip_sampler_set_default_action_no_sample(sess_hdl, dev_tgt)

    # Add dummy IPv4 route entry
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    aspec = stful_set_next_hop_action_spec_t(hex_to_i32(0))
    test.client.ipv4_route_set_default_action_set_next_hop(sess_hdl, dev_tgt, aspec)

    # Add a dummy next hop entry
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    aspec = stful_set_egr_ifid_action_spec_t(hex_to_i32(0))
    test.client.next_hop_set_default_action_set_egr_ifid(sess_hdl, dev_tgt, aspec)

    test.conn_mgr.complete_operations(sess_hdl)
    test.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))


def direct_tcam_tear_down(test, port_tbl, remove_state=True):
    sess_hdl = test.conn_mgr.client_init()

    port_tbl.rmv_all(remove_state)

    # Remove sip sampler default entry
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    test.client.sip_sampler_table_reset_default_entry(sess_hdl, dev_tgt)

    # Remove IPv4 route default entry
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    test.client.ipv4_route_table_reset_default_entry(sess_hdl, dev_tgt)

    # Remove next hop default entry
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    test.client.next_hop_table_reset_default_entry(sess_hdl, dev_tgt)

    test.conn_mgr.complete_operations(sess_hdl)
    test.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))

class TestDirectTcamCounter_default(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        print()
        try:
            num_pipes = int(test_param_get('num_pipes'))
            pipe_list = [x for x in range(num_pipes)]
            pipe_list_len = len(pipe_list)
            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)

            # Add the IFID entry
            test_ifid = 1
            ifid_tbl.add(test_ifid, ts=0, offset=0x80000000)

            # Add miscellanous entries
            direct_tcam_setup(self, list(swports), test_ifid, port_tbl)

            # Add a default entry
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            cntr_val = 0x0000000012345678 + len(swports) * 0x80000000
            reg = stful_port_cntr_value_t(f0=hex_to_i32(cntr_val>>32), f1=hex_to_i32(cntr_val & 0xFFFFFFFF))
            eh = self.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt, aspec, reg)
            self.conn_mgr.complete_operations(sess_hdl)

            # Ensure the counter value is correct
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr( sess_hdl, dev_tgt, hex_to_i32(eh), hw_sync_flag )
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            print("Counter values:")
            for v in x:
                print(hex(v), end=' ')
            print()
            print("Expecting", hex(cntr_val))
            sys.stdout.flush()
            if 0 in pipe_list:
                self.assertEqual(x[0], cntr_val)
            if 1 in pipe_list:
                self.assertEqual(x[1], cntr_val)
            if 2 in pipe_list:
                self.assertEqual(x[2], cntr_val)
            if 3 in pipe_list:
                self.assertEqual(x[3], cntr_val)

            # Send a packet to increment the counter
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='0.0.0.0',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            pipe_count = [0,0,0,0]
            for port in swports:
                pipe = port_to_pipe(port)
                send_packet(self, port, pkt)
                pipe_count[pipe] += 1
            for p in range(len(swports)):
                verify_packet(self, pkt, swports[0])

            # Verify the counts are correct
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr( sess_hdl, dev_tgt, hex_to_i32(eh), hw_sync_flag )
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            print("Counter values:")
            for v in x:
                print(hex(v), end=' ')
            print()
            print("Expecting:")
            for p in range(num_pipes):
                print(hex(cntr_val), " - ", pipe_count[p], " * 0x80000000 == ", hex(cntr_val - pipe_count[p]*0x80000000))
            sys.stdout.flush()
            if 0 in pipe_list:
                self.assertEqual(x[0], cntr_val - pipe_count[0]*0x80000000)
            if 1 in pipe_list:
                self.assertEqual(x[1], cntr_val - pipe_count[1]*0x80000000)
            if 2 in pipe_list:
                self.assertEqual(x[2], cntr_val - pipe_count[2]*0x80000000)
            if 3 in pipe_list:
                self.assertEqual(x[3], cntr_val - pipe_count[3]*0x80000000)

            ifid_tbl.mod(test_ifid, ts=0, offset=0x7FFFFFFF)
            self.conn_mgr.complete_operations(sess_hdl)
            for port in swports:
                send_packet(self, port, pkt)
            for p in range(len(swports)):
                verify_packet(self, pkt, swports[0])

            # Verify the counts are correct
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr( sess_hdl, dev_tgt, hex_to_i32(eh), hw_sync_flag )
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            print("Counter values:")
            for v in x:
                print(hex(v), end=' ')
            print()
            print("Expecting:")
            for p in range(num_pipes):
                print(hex(cntr_val), " - ", pipe_count[p], " * 0x80000000 + ", pipe_count[p], " * 0x7FFFFFFF == ", hex(cntr_val - pipe_count[p]*0x80000000 + pipe_count[p]*0x7FFFFFFF))
            sys.stdout.flush()
            if 0 in pipe_list:
                self.assertEqual(x[0], cntr_val - pipe_count[0]*0x80000000 + pipe_count[0]*0x7FFFFFFF)
            if 1 in pipe_list:
                self.assertEqual(x[1], cntr_val - pipe_count[1]*0x80000000 + pipe_count[1]*0x7FFFFFFF)
            if 2 in pipe_list:
                self.assertEqual(x[2], cntr_val - pipe_count[2]*0x80000000 + pipe_count[2]*0x7FFFFFFF)
            if 3 in pipe_list:
                self.assertEqual(x[3], cntr_val - pipe_count[3]*0x80000000 + pipe_count[3]*0x7FFFFFFF)


            aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            cntr_val = 0
            reg = stful_port_cntr_value_t(f0=cntr_val>>32, f1=cntr_val & 0xFFFFFFFF)
            eh = self.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt, aspec, reg)
            self.conn_mgr.complete_operations(sess_hdl)
            regs = self.client.register_read_port_cntr( sess_hdl, dev_tgt, hex_to_i32(eh), hw_sync_flag )
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            print("Counter values:")
            for v in x:
                print(hex(v), end=' ')
            print()
            print("Expecting: 0")
            sys.stdout.flush()
            for pipe in range(pipe_list_len):
                self.assertEqual(x[pipe], 0)

            # Change symmetric mode
            self.client.egr_port_table_reset_default_entry(sess_hdl, dev_tgt)
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
            self.client.egr_port_set_property(sess_hdl, dev_id, prop, prop_val, 0)

            dev_tgt_0 = DevTarget_t(dev_id, hex_to_i16(0))
            dev_tgt_1 = DevTarget_t(dev_id, hex_to_i16(1))
            dev_tgt_2 = DevTarget_t(dev_id, hex_to_i16(2))
            dev_tgt_3 = DevTarget_t(dev_id, hex_to_i16(3))
            aspec_0 = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            aspec_1 = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            aspec_2 = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            aspec_3 = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            cntr_val_0 = 0x1000000000000000
            cntr_val_1 = 0x1000000000000001
            cntr_val_2 = 0x1000000000000002
            cntr_val_3 = 0x1000000000000003
            reg_0 = stful_port_cntr_value_t(f0=cntr_val_0>>32,f1=cntr_val_0& 0xFFFFFFFF)
            reg_1 = stful_port_cntr_value_t(f0=cntr_val_1>>32,f1=cntr_val_1& 0xFFFFFFFF)
            reg_2 = stful_port_cntr_value_t(f0=cntr_val_2>>32,f1=cntr_val_2& 0xFFFFFFFF)
            reg_3 = stful_port_cntr_value_t(f0=cntr_val_3>>32,f1=cntr_val_3& 0xFFFFFFFF)
            if 0 in pipe_list:
                eh_0 = self.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt_0, aspec_0, reg_0)
            if 1 in pipe_list:
                eh_1 = self.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt_1, aspec_1, reg_1)
            if 2 in pipe_list:
                eh_2 = self.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt_2, aspec_2, reg_2)
            if 3 in pipe_list:
                eh_3 = self.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt_3, aspec_3, reg_3)
            self.conn_mgr.complete_operations(sess_hdl)

            print("Sending a packet to ports:", swports)
            sys.stdout.flush()
            for port in swports:
                send_packet(self, port, pkt)
            print("Receiving all packets back on port", swports[0])
            sys.stdout.flush()
            for p in range(len(swports)):
                verify_packet(self, pkt, swports[0])

            regs = []
            if 0 in pipe_list:
                regs.extend( self.client.register_read_port_cntr( sess_hdl, dev_tgt_0, hex_to_i32(eh_0), hw_sync_flag ) )
            if 1 in pipe_list:
                regs.extend( self.client.register_read_port_cntr( sess_hdl, dev_tgt_1, hex_to_i32(eh_1), hw_sync_flag ) )
            if 2 in pipe_list:
                regs.extend( self.client.register_read_port_cntr( sess_hdl, dev_tgt_2, hex_to_i32(eh_2), hw_sync_flag ) )
            if 3 in pipe_list:
                regs.extend( self.client.register_read_port_cntr( sess_hdl, dev_tgt_3, hex_to_i32(eh_3), hw_sync_flag ) )
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            print("Counter values:", x)

            if 0 in pipe_list:
                self.assertEqual(x[0], cntr_val_0+0x7FFFFFFF*pipe_count[0])
                self.client.register_write_port_cntr( sess_hdl, dev_id, hex_to_i32(eh_0), stful_port_cntr_value_t(0,0))
            if 1 in pipe_list:
                self.assertEqual(x[1], cntr_val_1+0x7FFFFFFF*pipe_count[1])
                self.client.register_write_port_cntr( sess_hdl, dev_id, hex_to_i32(eh_1), stful_port_cntr_value_t(0,0))
            if 2 in pipe_list:
                self.assertEqual(x[2], cntr_val_2+0x7FFFFFFF*pipe_count[2])
                self.client.register_write_port_cntr( sess_hdl, dev_id, hex_to_i32(eh_2), stful_port_cntr_value_t(0,0))
            if 3 in pipe_list:
                self.assertEqual(x[3], cntr_val_3+0x7FFFFFFF*pipe_count[3])
                self.client.register_write_port_cntr( sess_hdl, dev_id, hex_to_i32(eh_3), stful_port_cntr_value_t(0,0))
            self.conn_mgr.complete_operations(sess_hdl)
            regs = []
            if 0 in pipe_list:
                regs.extend( self.client.register_read_port_cntr( sess_hdl, dev_tgt_0, hex_to_i32(eh_0), hw_sync_flag ) )
            if 1 in pipe_list:
                regs.extend( self.client.register_read_port_cntr( sess_hdl, dev_tgt_1, hex_to_i32(eh_1), hw_sync_flag ) )
            if 2 in pipe_list:
                regs.extend( self.client.register_read_port_cntr( sess_hdl, dev_tgt_2, hex_to_i32(eh_2), hw_sync_flag ) )
            if 3 in pipe_list:
                regs.extend( self.client.register_read_port_cntr( sess_hdl, dev_tgt_3, hex_to_i32(eh_3), hw_sync_flag ) )
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            print("Counter values:", x)
            for pipe in range(pipe_list_len):
                self.assertEqual(x[pipe], 0)

            if 0 in pipe_list:
                self.client.egr_port_table_reset_default_entry(sess_hdl, dev_tgt_0)
            if 1 in pipe_list:
                self.client.egr_port_table_reset_default_entry(sess_hdl, dev_tgt_1)
            if 2 in pipe_list:
                self.client.egr_port_table_reset_default_entry(sess_hdl, dev_tgt_2)
            if 3 in pipe_list:
                self.client.egr_port_table_reset_default_entry(sess_hdl, dev_tgt_3)

            verify_no_other_packets(self)
        finally:
            direct_tcam_tear_down(self, port_tbl)
            ifid_tbl.rmv_all()
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
            self.client.egr_port_set_property(sess_hdl, dev_id, prop, prop_val, 0)

            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.client_cleanup( sess_hdl )


class TestDirectTcamCounter_move(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        entry_hdls = []
        test_ifid = 1
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        try:
            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)

            # Add miscellanous entries
            direct_tcam_setup(self, list(swports), test_ifid, port_tbl)

            # Add the IFID entry
            ifid_tbl.add(test_ifid, ts=0, offset=0x00000064)

            # Set a default next-hop entry to provided the egress ifid.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_egr_ifid_action_spec_t(hex_to_i32(100))
            self.client.next_hop_set_default_action_set_egr_ifid(sess_hdl, dev_tgt, aspec)

            # Add a TCAM entry with a stateful index to test
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            mspec = stful_egr_port_match_spec_t(hex_to_i16(100), hex_to_i16(0xFFFF))
            priority = 100
            aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            cntr_val = stful_port_cntr_value_t(f0=0,f1=0)
            eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
            entry_hdls.append(eh)
            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet to match it.
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='10.0.0.1',
                                    ip_dst='10.0.0.2')
            send_packet(self, swports[0], pkt)
            verify_packet(self, pkt, swports[0])

            # Read the counter and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            pipe_counts = [0,0,0,0]
            pipe = port_to_pipe(swports[0])
            pipe_counts[pipe] += 100
            for p in pipe_list:
                self.assertEqual(x[p], pipe_counts[p])

            # Add more entries to move this entry.
            for i in range(1,51):
                dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
                mspec = stful_egr_port_match_spec_t(hex_to_i16(100-i), hex_to_i16(0xFFFF))
                priority = 100-i
                aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
                cntr_val = stful_port_cntr_value_t(f0=0,f1=0)
                eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
                entry_hdls.append(eh)

                mspec = stful_egr_port_match_spec_t(hex_to_i16(100+i), hex_to_i16(0xFFFF))
                priority = 100+i
                eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
                entry_hdls.append(eh)

            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet to match it.
            for port in swports:
                pipe = port_to_pipe(port)
                send_packet(self, port, pkt)
                verify_packet(self, pkt, swports[0])
                pipe_counts[pipe] += 100

            # Read the counter and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            for p in pipe_list:
                self.assertEqual(x[p], pipe_counts[p])

            # Write the counter then read it and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            self.client.register_write_port_cntr(sess_hdl, dev_id, entry_hdls[0], stful_port_cntr_value_t(0,500))
            self.conn_mgr.complete_operations(sess_hdl)
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            for pipe in range(pipe_list_len):
                self.assertEqual(x[pipe], 500)
                pipe_counts[pipe] = 500

            # Send a packet to change the counter.
            for port in swports:
                pipe = port_to_pipe(port)
                send_packet(self, port, pkt)
                verify_packet(self, pkt, swports[0])
                pipe_counts[pipe] += 100

            # Sync the table and query the entries again.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            self.client.register_hw_sync_port_cntr(sess_hdl, dev_tgt)
            hw_sync_flag = stful_register_flags_t(read_hw_sync = False)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            for p in pipe_list:
                self.assertEqual(x[p], pipe_counts[p])
            for i in range(1, len(entry_hdls)):
                regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[i], hw_sync_flag)
                x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
                self.assertEqual(len(x), pipe_list_len)
                for pipe in range(pipe_list_len):
                    self.assertEqual(x[pipe], 0)


        finally:
            direct_tcam_tear_down(self, port_tbl)
            for eh in entry_hdls:
                self.client.egr_port_table_delete(sess_hdl, dev_id, eh)
            ifid_tbl.rmv_all()
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
            self.client.egr_port_set_property(sess_hdl, dev_id, prop, prop_val, 0)

            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.client_cleanup( sess_hdl )


class TestDirectTcamCounter_twoStage(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        entry_hdls = []
        test_ifid = 1
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        test_pipe = port_to_pipe(swports[-1])
        test_port = swports[-1]
        print()
        print("Pipe ",test_pipe," is being used for test")
        try:
            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)

            # Add miscellanous entries
            direct_tcam_setup(self, list(swports), test_ifid, port_tbl)

            # Add the IFID entry
            ifid_tbl.add(test_ifid, ts=0, offset=0x00000064)

            # Set a default next-hop entry to provided the egress ifid.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_egr_ifid_action_spec_t(hex_to_i32(100))
            self.client.next_hop_set_default_action_set_egr_ifid(sess_hdl, dev_tgt, aspec)

            # Set asymmetric mode.
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
            self.client.egr_port_set_property(sess_hdl, dev_id, prop, prop_val, 0)
            self.conn_mgr.complete_operations(sess_hdl)

            # Add a TCAM entry with a stateful index to test
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(test_pipe))
            mspec = stful_egr_port_match_spec_t(hex_to_i16(100), hex_to_i16(0xFFFF))
            priority = 100
            aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            cntr_val = stful_port_cntr_value_t(f0=0, f1=1)
            eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
            entry_hdls.append(eh)
            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet to match it.
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='10.0.0.1',
                                    ip_dst='10.0.0.2')
            send_packet(self, test_port, pkt)
            verify_packet(self, pkt, swports[0])

            # Read the counter and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(test_pipe))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            time.sleep(2*5)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0], 101)

            # Add more entries to move this entry to the next stage.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(test_pipe))
            for i in range(12288):
                mspec = stful_egr_port_match_spec_t(hex_to_i16(99), hex_to_i16(0xFFFF))
                priority = 99
                aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
                cntr_val = stful_port_cntr_value_t(f0=0, f1=0)
                eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
                entry_hdls.append(eh)

            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet to match it.
            send_packet(self, test_port, pkt)
            verify_packet(self, pkt, swports[0])

            # Read the counter and verify it.  The count should be reset back
            # to 1 (the value in the stateful spec) due to the move across
            # stages.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(test_pipe))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0], 101)

            # Write the counter then read it and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(test_pipe))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            self.client.register_write_port_cntr(sess_hdl, dev_id, entry_hdls[0], stful_port_cntr_value_t(0,500))
            self.conn_mgr.complete_operations(sess_hdl)
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(test_pipe))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0], 500)

            # Send a packet to change the counter.
            send_packet(self, test_port, pkt)
            verify_packet(self, pkt, swports[0])

            # Sync the table and query the entries again.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(test_pipe))
            self.client.register_hw_sync_port_cntr(sess_hdl, dev_tgt)
            hw_sync_flag = stful_register_flags_t(read_hw_sync = False)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0], 600)
            for i in range(1, len(entry_hdls)):
                regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[i], hw_sync_flag)
                x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
                self.assertEqual(len(x), 1)
                self.assertEqual(x[0], 0)


        finally:
            direct_tcam_tear_down(self, port_tbl)
            for eh in entry_hdls:
                self.client.egr_port_table_delete(sess_hdl, dev_id, eh)
            ifid_tbl.rmv_all()
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
            self.client.egr_port_set_property(sess_hdl, dev_id, prop, prop_val, 0)

            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.client_cleanup( sess_hdl )



class TestInDirectTcamCounter_rdwr(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        try:
            sess_hdl = self.conn_mgr.client_init()
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            hw_async_flag = stful_register_flags_t(read_hw_sync = False)

            for idx in range(SCRATCH_REG_SIZE):
                self.client.register_write_scratch(sess_hdl, dev_tgt, idx, hex_to_i16(idx))
            self.conn_mgr.complete_operations(sess_hdl)
            for idx in range(SCRATCH_REG_SIZE):
                reg = self.client.register_read_scratch(sess_hdl, dev_tgt, idx, hw_sync_flag)
                x = [i16_to_hex(r) for r in reg]
                result = []
                for i in range(pipe_list_len):
                    result.append(idx)
                if x != result:
                    print("Index", idx, "mismatch!")
                    print("Expected", result)
                    print("Read    ", x)
                    sys.stdout.flush()
                self.assertEqual(x, result)

            self.conn_mgr.begin_batch(sess_hdl)
            for idx in range(SCRATCH_REG_SIZE):
                self.client.register_write_scratch(sess_hdl, dev_tgt, idx, hex_to_i16((SCRATCH_REG_SIZE-1)-idx))
            self.conn_mgr.end_batch(sess_hdl, True)
            for idx in range(SCRATCH_REG_SIZE):
                reg = self.client.register_read_scratch(sess_hdl, dev_tgt, idx, hw_sync_flag)
                x = [i16_to_hex(r) for r in reg]
                y = (SCRATCH_REG_SIZE-1) - idx
                result = []
                for i in range(pipe_list_len):
                    result.append(y)
                if x != result:
                    print("Index", idx, "mismatch!")
                    print("Expected", result)
                    print("Read    ", x)
                    sys.stdout.flush()
                self.assertEqual(x, result)

            self.conn_mgr.begin_txn(sess_hdl, False)
            for idx in range(SCRATCH_REG_SIZE):
                self.client.register_write_scratch(sess_hdl, dev_tgt, idx, hex_to_i16(idx+10))
            self.conn_mgr.commit_txn(sess_hdl, True)
            for idx in range(SCRATCH_REG_SIZE):
                reg = self.client.register_read_scratch(sess_hdl, dev_tgt, idx, hw_sync_flag)
                x = [i16_to_hex(r) for r in reg]
                y = idx + 10
                result = []
                for i in range(pipe_list_len):
                    result.append(y)
                self.assertEqual(x, result)

            self.conn_mgr.begin_txn(sess_hdl, False)
            for idx in range(SCRATCH_REG_SIZE):
                self.client.register_write_scratch(sess_hdl, dev_tgt, idx, hex_to_i16(0))
            self.conn_mgr.abort_txn(sess_hdl)
            for idx in range(SCRATCH_REG_SIZE):
                reg = self.client.register_read_scratch(sess_hdl, dev_tgt, idx, hw_sync_flag)
                x = [i16_to_hex(r) for r in reg]
                y = idx + 10
                result = []
                for i in range(pipe_list_len):
                    result.append(y)
                self.assertEqual(x, result)

            self.conn_mgr.begin_batch(sess_hdl)
            for idx in range(SCRATCH_REG_SIZE):
                self.client.register_write_scratch(sess_hdl, dev_tgt, idx, hex_to_i16(~idx & 0xFFFF))
            self.conn_mgr.end_batch(sess_hdl, True)
            self.client.register_hw_sync_scratch(sess_hdl, dev_tgt)
            for idx in range(SCRATCH_REG_SIZE):
                reg = self.client.register_read_scratch(sess_hdl, dev_tgt, idx, hw_async_flag)
                x = [i16_to_hex(r) for r in reg]
                y = ~idx & 0xFFFF
                result = []
                for i in range(pipe_list_len):
                    result.append(y)
                self.assertEqual(x, result)

            self.client.register_reset_all_scratch(sess_hdl, dev_tgt)
            self.conn_mgr.complete_operations(sess_hdl)
            self.client.register_hw_sync_scratch(sess_hdl, dev_tgt)
            for idx in range(SCRATCH_REG_SIZE):
                reg = self.client.register_read_scratch(sess_hdl, dev_tgt, idx, hw_async_flag)
                x = [i16_to_hex(r) for r in reg]
                result = []
                for i in range(pipe_list_len):
                    result.append(0)
                self.assertEqual(x, result)

        finally:
            for idx in range(SCRATCH_REG_SIZE):
                self.client.register_write_scratch(sess_hdl, dev_tgt, idx, hex_to_i16(0))
            self.conn_mgr.client_cleanup(sess_hdl)


def indirect_tcam_setup(test, port_list, ifid, port_tbl):
    sess_hdl = test.conn_mgr.client_init()

    for port in port_list:
        port_tbl.add(port, ifid)

    # Add dummy sip sampler entries
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    test.client.sip_sampler_set_default_action_no_sample(sess_hdl, dev_tgt)

    # Add a dummy egr_port entry
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
    test.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt, aspec, stful_port_cntr_value_t(f0=0,f1=0))

    test.conn_mgr.complete_operations(sess_hdl)
    test.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))

def indirect_tcam_tear_down(test, port_tbl):
    port_tbl.rmv_all()
    sess_hdl = test.conn_mgr.client_init()
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
    test.client.egr_port_table_reset_default_entry(sess_hdl, dev_tgt)
    test.client.sip_sampler_table_reset_default_entry(sess_hdl, dev_tgt)
    test.conn_mgr.complete_operations(sess_hdl)
    test.conn_mgr.complete_operations(sess_hdl)

class TestInDirectTcamCounter_entries(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        if pipe_list_len < 4:
            expected_cntrs = [[0,0]]*SCRATCH_REG_SIZE
        else:
            expected_cntrs = [[0,0,0,0]]*SCRATCH_REG_SIZE
        route_hdls = []
        next_hop_handles = []
        try:
            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)
            smpl_tbl = SamplerTable(self, sess_hdl, dev_id)
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            hw_async_flag = stful_register_flags_t(read_hw_sync = False)

            # Add the IFID entry
            test_ifid = 1
            ifid_tbl.add(test_ifid, ts=0, offset=0)

            # Add miscellanous entries
            indirect_tcam_setup(self, list(swports), test_ifid, port_tbl)

            # Add a few route entries to go to a few different next hop values.
            # Next hop values will be 100 through 109.
            routes = ['10.0.0.1', '10.0.0.2', '10.0.0.3', '10.0.0.4',
                      '10.0.0.5', '10.0.0.6', '10.0.0.7', '10.0.0.8',
                      '10.0.0.9', '10.0.0.10']
            next_hops = []
            for i in range(len(routes)):
                dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
                mspec = stful_ipv4_route_match_spec_t( ipv4Addr_to_i32(routes[i]), hex_to_i16(32) )
                val = 100+i
                aspec = stful_set_next_hop_action_spec_t( hex_to_i32(val) )
                eh = self.client.ipv4_route_table_add_with_set_next_hop(sess_hdl, dev_tgt, mspec, aspec)
                self.assertNotEqual(eh, 0)
                route_hdls.append(eh)
                next_hops.append(val)

            pkts = {}
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            pri = 100
            for i in range(SCRATCH_REG_SIZE):
                self.client.register_write_scratch(sess_hdl, dev_tgt, i, 0)
            self.conn_mgr.complete_operations(sess_hdl)
            for i in range(len(next_hops) // 5):
                stful_idx = 4095 * i
                mspec = stful_next_hop_match_spec_t(hex_to_i16(next_hops[5*i+0]), hex_to_i16(0xFFFF))
                aspec = stful_set_egr_ifid_action_spec_t(3)
                eh = self.client.next_hop_table_add_with_set_egr_ifid(sess_hdl, dev_tgt, mspec, pri, aspec)
                next_hop_handles.append(eh)
                pkt = simple_tcp_packet( ip_dst=routes[5*i+0] )
                pkts[(stful_idx, "nop")] = (pkt, next_hops[5*i+0])

                mspec = stful_next_hop_match_spec_t(hex_to_i16(next_hops[5*i+1]), hex_to_i16(0xFFFF))
                aspec = stful_scratch_add_action_spec_t(stful_idx, 3)
                eh = self.client.next_hop_table_add_with_scratch_add(sess_hdl, dev_tgt, mspec, pri, aspec)
                next_hop_handles.append(eh)
                pkt = simple_tcp_packet( ip_dst=routes[5*i+1] )
                pkts[(stful_idx, "add")] = (pkt, next_hops[5*i+1])

                mspec = stful_next_hop_match_spec_t(hex_to_i16(next_hops[5*i+2]), hex_to_i16(0xFFFF))
                aspec = stful_scratch_sub_action_spec_t(stful_idx, 3)
                eh = self.client.next_hop_table_add_with_scratch_sub(sess_hdl, dev_tgt, mspec, pri, aspec)
                next_hop_handles.append(eh)
                pkt = simple_tcp_packet( ip_dst=routes[5*i+2] )
                pkts[(stful_idx, "sub")] = (pkt, next_hops[5*i+2])

                mspec = stful_next_hop_match_spec_t(hex_to_i16(next_hops[5*i+3]), hex_to_i16(0xFFFF))
                aspec = stful_scratch_zero_action_spec_t(stful_idx, 3)
                eh = self.client.next_hop_table_add_with_scratch_zero(sess_hdl, dev_tgt, mspec, pri, aspec)
                next_hop_handles.append(eh)
                pkt = simple_tcp_packet( ip_dst=routes[5*i+3] )
                pkts[(stful_idx, "zero")] = (pkt, next_hops[5*i+3])

                mspec = stful_next_hop_match_spec_t(hex_to_i16(next_hops[5*i+4]), hex_to_i16(0xFFFF))
                aspec = stful_scratch_invert_action_spec_t(stful_idx, 3)
                eh = self.client.next_hop_table_add_with_scratch_invert(sess_hdl, dev_tgt, mspec, pri, aspec)
                next_hop_handles.append(eh)
                pkt = simple_tcp_packet( ip_dst=routes[5*i+4] )
                pkts[(stful_idx, "invert")] = (pkt, next_hops[5*i+4])

            pkt, offset = pkts[(0, "nop")]
            for port in swports:
                send_packet(self, port, pkt)
                verify_packet(self, pkt, swports[0])
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

            self.client.register_hw_sync_scratch(sess_hdl, dev_tgt)
            for i in range(SCRATCH_REG_SIZE):
                x = self.client.register_read_scratch(sess_hdl, dev_tgt, i, hw_async_flag)
                self.assertEqual(x, expected_cntrs[i])

            test_port = swports[-1]
            test_pipe = port_to_pipe(test_port)
            (pkt, offset) = pkts[(0, "add")]
            send_packet(self, test_port, pkt)
            send_packet(self, test_port, pkt)
            send_packet(self, test_port, pkt)
            verify_packet(self, pkt, swports[0])
            verify_packet(self, pkt, swports[0])
            verify_packet(self, pkt, swports[0])
            expected_cntrs[0][test_pipe] = (expected_cntrs[0][test_pipe] + 3*offset) & 0xFFFF
            x = self.client.register_read_scratch(sess_hdl, dev_tgt, 0, hw_sync_flag)
            self.assertEqual(x, expected_cntrs[0])

            (pkt, offset) = pkts[(0, "sub")]
            send_packet(self, test_port, pkt)
            verify_packet(self, pkt, swports[0])
            expected_cntrs[0][test_pipe] = (offset - expected_cntrs[0][test_pipe]) & 0xFFFF
            x = self.client.register_read_scratch(sess_hdl, dev_tgt, 0, hw_sync_flag)
            y = [i16_to_hex(X) for X in x]
            self.assertEqual(y, expected_cntrs[0])

            (pkt, offset) = pkts[(0, "invert")]
            send_packet(self, test_port, pkt)
            verify_packet(self, pkt, swports[0])
            expected_cntrs[0][test_pipe] = ~expected_cntrs[0][test_pipe] & 0xFFFF
            x = self.client.register_read_scratch(sess_hdl, dev_tgt, 0, hw_sync_flag)
            self.assertEqual(x, expected_cntrs[0])

            (pkt, offset) = pkts[(0, "zero")]
            send_packet(self, test_port, pkt)
            verify_packet(self, pkt, swports[0])
            expected_cntrs[0][test_pipe] = 0
            x = self.client.register_read_scratch(sess_hdl, dev_tgt, 0, hw_sync_flag)
            self.assertEqual(x, expected_cntrs[0])



        finally:
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            for i in range(SCRATCH_REG_SIZE):
                self.client.register_write_scratch(sess_hdl, dev_tgt, i, 0)
            for eh in route_hdls:
                self.client.ipv4_route_table_delete(sess_hdl, dev_id, eh)
            for eh in next_hop_handles:
                self.client.next_hop_table_delete(sess_hdl, dev_id, eh)

            ifid_tbl.rmv_all()
            indirect_tcam_tear_down(self, port_tbl)
            self.conn_mgr.client_cleanup(sess_hdl)



def fast_reconfig_setup(test, pipe, sips, routes, ifid_ports, port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl, pipe_list_len):
    test_ports = list(swports)
    test_ifids = list(swports)

    # Add port table entries
    for port,ifid in zip(test_ports, test_ifids):
        port_tbl.add(port, ifid)

    # Add ifid table entries
    for ifid in test_ifids:
        ifid_tbl.add(ifid, pipe=pipe, counter=ifid, ts=0, offset=1)

    # Add the SIP entries for sampling
    idx = SIP_SAMPLER_REG_SIZE-1
    for sip in sips:
        smpl_tbl.add(sip, pipe, sample_idx=idx)
        idx = idx - 1

    # Add the route entries and next hops
    nh_modes = ['ADD', 'SUB', 'INVERT', 'ZERO']
    for i in range(len(routes)):
        dip, prefix_len, nh, ifid = routes[i]
        rte_tbl.add(dip, prefix_len, nh, pipe)
        nh_tbl.add(nh, ifid, mode=nh_modes[i % len(nh_modes)], index=i//4, pipe=pipe)

    # Add egress port entries
    for egr_ifid, egr_port in ifid_ports:
        egr_port_tbl.add(egr_ifid, egr_port, pipe, cntr=egr_port)

    # Set CPU Port
    test.devport_mgr.devport_mgr_set_copy_to_cpu(dev_id, True, test.cpu_port);

def fast_reconfig_teardown(test, port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl):
    port_tbl.rmv_all()
    ifid_tbl.rmv_all()
    smpl_tbl.rmv_all()
    rte_tbl.rmv_all()
    nh_tbl.rmv_all()
    egr_port_tbl.rmv_all()

    # Remove CPU Port
    test.devport_mgr.devport_mgr_set_copy_to_cpu(dev_id, False, 0);

def fast_reconfig_verify(test, ip_addrs, test_ports_in, port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl):

    test_ports = sorted(test_ports_in)

    for sip,dip in ip_addrs:
        ttl = 10
        pkt = simple_tcp_packet(ip_src=sip,
                                ip_dst=dip,
                                ip_ttl=ttl)
        cur_pipe = -1
        ifid_cntr_val = ttl
        for i_port in sorted(test_ports):
            # Check if this is the first time this packet is sent to this pipe
            # since the bloom filter will trigger a C2C for the first.
            first_in_pipe = cur_pipe != port_to_pipe(i_port)
            cur_pipe = port_to_pipe(i_port)

            # Map ingress port to IFID and IFID to counter
            i_ifid = port_tbl.get_ifid( i_port )
            ifid_tbl.inc_cntr(i_ifid, cur_pipe, ifid_cntr_val)

            # Map SIP to counter index and check if a sample is taken
            smpl_idx = smpl_tbl.get_counter_index(sip, cur_pipe)
            sampled = smpl_tbl.inc_cntr(smpl_idx, cur_pipe)

            # Map DIP to next hop and next hop to counter+action
            nh = rte_tbl.get_nh(dip, cur_pipe)
            nh_tbl.update_cntr(nh, cur_pipe)

            # Map IFID to offset and adjust egr_port cntr
            cntr_offset = ifid_tbl.ifid_to_offset(i_ifid, cur_pipe)
            e_ifid = nh_tbl.get_ifid(nh, cur_pipe)
            egr_port_tbl.update_cntr(e_ifid, cur_pipe, cntr_offset)

            # Map IFID to egress port
            e_port = egr_port_tbl.get_port(e_ifid, cur_pipe)

            # Send the packet.
            send_packet(test, i_port, pkt)
            # Verify the return packet.
            verify_packet(test, pkt, e_port)
            # Verify the C2C packet if it should be created
            if first_in_pipe or sampled:
                verify_packet(test, pkt, test.cpu_port)

    ifid_tbl.verify_cntrs()
    smpl_tbl.verify_cntrs()
    nh_tbl.verify_cntrs()
    egr_port_tbl.verify_cntrs()


class TestFastReconfig(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def addPorts(self):
        for i in swports:
            if test_param_get("port_mode") == "100g":
                self.pal.pal_port_add(dev_id, i,
                                    pal_port_speed_t.BF_SPEED_100G,
                                    pal_fec_type_t.BF_FEC_TYP_NONE)
            elif test_param_get("port_mode") == "50g":
                self.pal.pal_port_add(dev_id, i,
                                    pal_port_speed_t.BF_SPEED_50G,
                                    pal_fec_type_t.BF_FEC_TYP_NONE)
            elif test_param_get("port_mode") == "25g":
                self.pal.pal_port_add(dev_id, i,
                                    pal_port_speed_t.BF_SPEED_25G,
                                    pal_fec_type_t.BF_FEC_TYP_NONE)
            else:
                self.pal.pal_port_add(dev_id, i,
                                    pal_port_speed_t.BF_SPEED_10G,
                                    pal_fec_type_t.BF_FEC_TYP_NONE)

            self.pal.pal_port_an_set(dev_id, i, pal_autoneg_policy_t.BF_AN_FORCE_DISABLE)
            self.pal.pal_port_enable(dev_id, i)
        ports_not_up = True
        print("Waiting for ports to come up...")
        sys.stdout.flush()
        num_tries = 12
        i = 0
        while ports_not_up:
            ports_not_up = False
            for p in swports:
                x = self.pal.pal_port_oper_status_get(dev_id, p)
                if x == pal_oper_status_t.BF_PORT_DOWN:
                    ports_not_up = True
                    print("  port", p, "is down")
                    sys.stdout.flush()
                    time.sleep(5)
                    break
            i += 1
            if i >= num_tries:
                break
        assert ports_not_up == False

    def runTest(self):
        sess_hdl = self.conn_mgr.client_init()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)

        port_tbl = PortTable(self, sess_hdl, dev_id)
        ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)
        smpl_tbl = SamplerTable(self, sess_hdl, dev_id)
        rte_tbl = RteTbl(self, sess_hdl, dev_id)
        nh_tbl = NextHopTbl(self, sess_hdl, dev_id)
        egr_port_tbl = EgressPortTbl(self, sess_hdl, dev_id)
        bf_tbl = BloomFilterTbl(self, sess_hdl, dev_id)

        self.cpu_port = swports[-1]

        pipe = 0xFFFF
        max_range = 11
        sips = []
        for i in range(1,max_range):
            sip = "10.0.1.%d" % (i)
            sips.append(sip)
        routes = []
        dips = []
        ifid_ports = []
        for i in range(1,max_range):
            dip = "20.0.1.%d" % (i)
            dips.append(dip)
            nh = i
            egr_ifid = i+0x1000
            egr_port = swports[i% len(swports)]
            routes.append( (dip, 32, nh, egr_ifid) )
            ifid_ports.append( (egr_ifid, egr_port) )

        print("Initial configuration started")
        ifid_tbl.reg_reset()
        smpl_tbl.reg_reset()
        nh_tbl.reg_reset()
        egr_port_tbl.reg_reset()
        bf_tbl.reg_reset()
        self.conn_mgr.complete_operations(sess_hdl)
        fast_reconfig_setup(self, pipe, sips, routes, ifid_ports, port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl, pipe_list_len)
        self.conn_mgr.complete_operations(sess_hdl)
        print("Initial configuration completed")

        print("Traffic verification started")
        fast_reconfig_verify(self, list(zip(sips, dips)), list(swports), port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl)
        print("Traffic verification completed")

        # Start the fast reconfig sequence (remove and add-locked)
        print("Fast Reconfig Stage 1 started")
        self.devport_mgr.devport_mgr_warm_init_begin(dev_id, dev_init_mode.DEV_WARM_INIT_FAST_RECFG, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, True)
        print("Fast Reconfig Stage 1 completed")


        if test_param_get('target') == 'hw':
            self.addPorts()

        # Reconfigure the ASIC
        print("Reconfiguration started")
        port_tbl = PortTable(self, sess_hdl, dev_id)
        ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)
        smpl_tbl = SamplerTable(self, sess_hdl, dev_id)
        rte_tbl = RteTbl(self, sess_hdl, dev_id)
        nh_tbl = NextHopTbl(self, sess_hdl, dev_id)
        egr_port_tbl = EgressPortTbl(self, sess_hdl, dev_id)
        bf_tbl = BloomFilterTbl(self, sess_hdl, dev_id)
        fast_reconfig_setup(self, pipe, sips, routes, ifid_ports, port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl, pipe_list_len)
        print("Reconfiguration completed")

        # Continue the fast reconfig sequence (go from locked to unlocked)
        print("Fast Reconfig Stage 2 started")
        self.devport_mgr.devport_mgr_warm_init_end(dev_id)
        print("Fast Reconfig Stage 2 completed")
        # Ensure all config is pushed.
        self.conn_mgr.complete_operations(sess_hdl)

        # Make sure everything works
        print("Traffic verification after reconfig started")
        fast_reconfig_verify(self, list(zip(sips, dips)), list(swports), port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl)
        print("Traffic verification after reconfig completed")

        # Reset registers
        print("Register reset after reconfig started")
        ifid_tbl.reg_reset()
        smpl_tbl.reg_reset()
        nh_tbl.reg_reset()
        egr_port_tbl.reg_reset()
        bf_tbl.reg_reset()
        self.conn_mgr.complete_operations(sess_hdl)
        print("Register reset after reconfig completed")
        print("Traffic verification after register reset started")
        fast_reconfig_verify(self, list(zip(sips, dips)), list(swports), port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl)
        print("Traffic verification after register reset completed")

        # Clean up!
        print("Cleanup started")
        fast_reconfig_teardown(self, port_tbl, ifid_tbl, smpl_tbl, rte_tbl, nh_tbl, egr_port_tbl)
        print("Cleanup completed")

        self.conn_mgr.client_cleanup(sess_hdl)




class TestResetAPIs(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        sess_hdl = None
        in_batch = False
        async_flag = stful_register_flags_t(read_hw_sync = False)
        sync = stful_register_flags_t(read_hw_sync = True)
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        try:
            sess_hdl = self.conn_mgr.client_init()
            self.conn_mgr.begin_batch(sess_hdl)
            in_batch = True

            print("Write all")
            self.client.register_write_all_bloom_filter_1(sess_hdl, dev_tgt, 1)
            self.conn_mgr.flush_batch(sess_hdl)
            print("Range reset")
            self.client.register_range_reset_bloom_filter_1(sess_hdl, dev_tgt, 10, 100)
            self.client.register_range_reset_bloom_filter_1(sess_hdl, dev_tgt, 6789, 1)
            self.client.register_range_reset_bloom_filter_1(sess_hdl, dev_tgt, 256, 128)
            self.client.register_range_reset_bloom_filter_1(sess_hdl, dev_tgt, 200000, 30000)

            print("End batch")
            self.conn_mgr.end_batch(sess_hdl, True)
            in_batch = False

            print("Syncing register")
            self.client.register_hw_sync_bloom_filter_1(sess_hdl, dev_tgt)
            print("Verifying register")
            for idx in range(BLOOM_FILTER_REG_SIZE):
                r = self.client.register_read_bloom_filter_1(sess_hdl, dev_tgt, idx, async_flag)
                if idx >= 10 and idx < 110:
                    self.assertEqual(r[0], 0, "index %d is %d, not %d" % (idx, r[0], 0))
                elif idx == 6789:
                    self.assertEqual(r[0], 0, "index %d is %d, not %d" % (idx, r[0], 0))
                elif idx >= 256 and idx < 384:
                    self.assertEqual(r[0], 0, "index %d is %d, not %d" % (idx, r[0], 0))
                elif idx >= 200000 and idx < 230000:
                    self.assertEqual(r[0], 0, "index %d is %d, not %d" % (idx, r[0], 0))
                else:
                    self.assertEqual(r[0], 1, "index %d is %d, not %d" % (idx, r[0], 1))

        finally:
            if sess_hdl:
                if in_batch:
                    self.conn_mgr.end_batch(sess_hdl, True)
                self.client.register_reset_all_bloom_filter_1(sess_hdl, dev_tgt)
                self.conn_mgr.client_cleanup(sess_hdl)



class TestRangeRead(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        sess_hdl = None
        in_batch = False
        async_flag = stful_register_flags_t(read_hw_sync = False)
        sync = stful_register_flags_t(read_hw_sync = True)
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        try:
            sess_hdl = self.conn_mgr.client_init()
            print("Reset sampling_cntr")
            self.client.register_reset_all_sampling_cntr(sess_hdl, dev_tgt)
            self.conn_mgr.complete_operations(sess_hdl)
            print("Read sampling_cntr in batch")
            x = self.client.register_range_read_sampling_cntr(sess_hdl, dev_tgt, 0, 100, sync)
            print("Got", len(x), "values")
            print("Read sampling_cntr one-by-one")
            X = []
            for i in range(100):
                X += self.client.register_read_sampling_cntr(sess_hdl, dev_tgt, i, async_flag)
            self.assertEqual(len(x), len(X))
            any_non_default = []
            for idx,val in enumerate(x):
                if val != 1:
                    any_non_default.append( (idx,val, X[idx]) )
            if len(any_non_default):
                print("Read non-default counts")
                print(any_non_default)
                sys.stdout.flush()
                self.assertEqual(len(any_non_default), 0)
            print("Write some values")
            self.conn_mgr.begin_batch(sess_hdl)
            in_batch = True;
            for i in range(12815, 91717):
                self.client.register_write_sampling_cntr(sess_hdl, dev_tgt, i, i)
            print("Ending batch")
            self.conn_mgr.end_batch(sess_hdl, True)
            in_batch = False;
            print("Reading values back")
            cnt = 91717-12815
            x = self.client.register_range_read_sampling_cntr(sess_hdl, dev_tgt, 12815, cnt, sync)
            expected_len = num_pipes * cnt
            self.assertEqual(len(x), expected_len)
            unexpected = []
            for idx,val in enumerate(x):
                expected_val = (idx // num_pipes) + 12815
                if val != expected_val:
                    unexpected.append( (val, expected_val, idx, idx//num_pipes) )
            for val,exp_val,idx,reg_idx in unexpected:
                print("Reg", reg_idx, "read-data-index", idx, "has value", val, "not", exp_val)
            sys.stdout.flush()
            self.assertEqual(len(unexpected), 0)

        finally:
            if sess_hdl:
                if in_batch:
                    self.conn_mgr.end_batch(sess_hdl, True)
                print("Reset sampling_cntr")
                self.client.register_reset_all_sampling_cntr(sess_hdl, dev_tgt)
                self.conn_mgr.complete_operations(sess_hdl)
                self.conn_mgr.client_cleanup(sess_hdl)


def portToPipe(port):
    return port >> 7
def portToPipeLocalId(port):
    return port & 0x7F
def portToBitIdx(port):
    pipe = portToPipe(port)
    index = portToPipeLocalId(port)
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

class TestStfulSelTbl(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def setUp(self):
        print()
        print(datetime.datetime.now(), "Setup started")
        sys.stdout.flush()
        pd_base_tests.ThriftInterfaceDataPlane.setUp(self)
        self.num_pipes = int(test_param_get('num_pipes'))
        self.pipe_list = [x for x in range(self.num_pipes)]
        self.pipe_list_len = len(self.pipe_list)
        self.recirc_ports = []
        for pipe in range(self.pipe_list_len):
            self.recirc_ports.append(make_port(pipe, 64))
        self.ingr_ports = list(swports)
        self.egr_ports = list(swports)
        for rp in self.recirc_ports:
            if rp in self.ingr_ports:
                self.ingr_ports.remove(rp)
            if rp in self.egr_ports:
                self.egr_ports.remove(rp)
        self.test_ifids = [x for x in self.pipe_list]

        self.num_lags = 5
        self.lag_size = 200
        self.num_mbrs_per_lag = self.lag_size
        self.mbrs_per_port = 64
        self.pgen_trig_wait_time = 12
        #self.num_lags = 3
        #self.lag_size = 121
        #self.num_mbrs_per_lag = self.lag_size
        #self.mbrs_per_port = 45
        #self.pgen_trig_wait_time = 3

        self.in_batch = False
        self.async_flag = stful_register_flags_t(read_hw_sync = False)
        self.sync_flag = stful_register_flags_t(read_hw_sync = True)
        self.pass_1_hdls  = []
        self.lag_mbr_hdls = {}
        self.eifid_hdls = set()
        self.port_to_lag_tbl_idx_list = {}
        for port in self.egr_ports:
            self.port_to_lag_tbl_idx_list[port] = []
        self.lag_index_to_hdl = {}
        self.lag_upd_hdl_to_index = {}

        # Create sessions for pipeline driver and multicast driver.
        self.shdl = hex_to_i32( self.conn_mgr.client_init() )
        self.mc_shdl = hex_to_i32( self.mc.mc_create_session() )

        # Use a dev_target for all pipes for most operations.
        self.dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        # Create helper classes for the usual tables
        self.port_tbl = PortTable(self, self.shdl, dev_id)
        self.ifid_tbl = IfidTbl(self, self.shdl, dev_id, self.port_tbl)
        self.smpl_tbl = SamplerTable(self, self.shdl, dev_id)
        self.rte_tbl  = RteTbl(self, self.shdl, dev_id)
        self.nh_tbl   = NextHopTbl(self, self.shdl, dev_id)
        self.egr_port_tbl = EgressPortTbl(self, self.shdl, dev_id)
        self.bf_tbl   = BloomFilterTbl(self, self.shdl, dev_id)

        # Define the pkt-gen app ids that will be needed
        self.port_down_app_id = 1
        self.recirc_app_id = 2

        # Define the pkt-gen initial app configurations.
        self.port_down_app = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.PORT_DOWN,
                                            batch_count=0, pkt_count=0,
                                            pattern_key=0, pattern_msk=0,
                                            timer=0,
                                            ibg=0, ibg_jitter=0,
                                            ipg=0, ipg_jitter=0,
                                            src_port=68, src_port_inc=0,
                                            buffer_offset=0, length=54 )
        self.recirc_app = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.RECIRC_PATTERN,
                                         batch_count=0, pkt_count=0,
                                         pattern_key=0, pattern_msk=0,
                                         timer=0,
                                         ibg=0, ibg_jitter=0,
                                         ipg=0, ipg_jitter=0,
                                         src_port=68, src_port_inc=0,
                                         buffer_offset=0, length=54 )
        self.conn_mgr.pktgen_cfg_app( self.shdl, self.dt, self.port_down_app_id, self.port_down_app)
        self.conn_mgr.pktgen_cfg_app( self.shdl, self.dt, self.recirc_app_id, self.recirc_app)

        # Define the mgid to use for recirculation to all pipes
        self.mgid = 0

        # Set up a multicast tree using our mgid and recirc ports.
        self.l1_hdl = self.mc.mc_node_create(self.mc_shdl, dev_id, 0, set_port_map(self.recirc_ports), set_lag_map([]))
        self.mgid_hdl = self.mc.mc_mgrp_create(self.mc_shdl, dev_id, hex_to_i16(self.mgid))
        self.mc.mc_associate_node(self.mc_shdl, dev_id,
                                  hex_to_i32(self.mgid_hdl),
                                  hex_to_i32(self.l1_hdl),
                                  0, False)

        # Enable packet gen and recirculation.
        for p in range(self.pipe_list_len):
            self.conn_mgr.pktgen_enable( self.shdl, dev_id, make_port(p,68) )
        speed_100g = 64
        for p in self.recirc_ports:
            # Delete the ports, change recirculation config and add ports back
            for port in range(0, 4):
                try:
                    self.devport_mgr.devport_mgr_remove_port(dev_id, p+port)
                except InvalidDevportMgrOperation as e:
                    pass
            self.conn_mgr.recirculation_enable(self.shdl, dev_id, p);
            self.devport_mgr.devport_mgr_add_port(dev_id, p, speed_100g, 0)

        # Clear the port down status in pkt-gen for all ports.
        self.conn_mgr.begin_batch(self.shdl)
        for pipe in range(self.pipe_list_len):
            for port in range (72):
                self.conn_mgr.pktgen_clear_port_down( self.shdl, dev_id, make_port(pipe, port) )
        self.conn_mgr.end_batch(self.shdl, False)

        # Add two match entries for the first pass packet gen packets.  The
        # first pass simply adds a recirc header (with recirc type) and sets
        # the MGID to recirculate back to all pipes.
        mspec = stful_prepare_for_recirc_match_spec_t(self.port_down_app_id)
        aspec = stful_prepare_for_recirc_action_spec_t(RECIRC_TYPE_PG_PORT_DOWN, self.mgid)
        self.pass_1_hdls.append( self.client.prepare_for_recirc_table_add_with_prepare_for_recirc(self.shdl, self.dt, mspec, aspec) )
        mspec = stful_prepare_for_recirc_match_spec_t(self.recirc_app_id)
        aspec = stful_prepare_for_recirc_action_spec_t(RECIRC_TYPE_PG_RECIRC, self.mgid)
        self.pass_1_hdls.append( self.client.prepare_for_recirc_table_add_with_prepare_for_recirc(self.shdl, self.dt, mspec, aspec) )

        # Enable update tracking on the two stateful selection tables.
        self.useless_cookie_1 = 0xABC12300
        self.client.next_hop_ecmp_ap_sel_track_updates(dev_id, hex_to_i32(self.useless_cookie_1))
        self.useless_cookie_2 = 0x12345678
        self.client.lag_ap_sel_track_updates(dev_id, hex_to_i32(self.useless_cookie_2))

        # Create a few LAG selection groups
        self.lag_groups = set()
        for _ in range(self.num_lags):
            grp_hdl = self.client.lag_ap_create_group(self.shdl, self.dt, self.lag_size)
            self.lag_groups.add( grp_hdl )

        # Add the egress ports to the LAG action data table.
        for p in self.egr_ports:
            aspec = stful_set_egr_port_action_spec_t(hex_to_i32(p))
            mbr_hdls = set()
            for _ in range(self.mbrs_per_port):
                x = self.client.lag_ap_add_member_with_set_egr_port(self.shdl, self.dt, aspec)
                mbr_hdls.add( x )
            self.lag_mbr_hdls[p] = mbr_hdls
            print("Port", p, "Members:", self.lag_mbr_hdls[p])
            sys.stdout.flush()
        self.all_lag_mbr_hdls = set()
        for key in self.lag_mbr_hdls:
            self.all_lag_mbr_hdls = self.all_lag_mbr_hdls | self.lag_mbr_hdls[key]

        # Populate the minimum set of table entries to get packets through.
        self.port_tbl.set_sym(False)
        self.ifid_tbl.set_sym(False)
        set_of_pipes = set()
        for port in self.ingr_ports:
            pipe = port_to_pipe(port)
            ifid = self.test_ifids[pipe]
            self.port_tbl.add(port,ifid)
            if pipe not in set_of_pipes:
                self.ifid_tbl.add(ifid, ts=0, offset=0)
                set_of_pipes.add(pipe)
        self.smpl_tbl.add(None)

        print(datetime.datetime.now(), "Setup complete")
        sys.stdout.flush()


    def lag_mbr_hdl_to_port(self, mbr_hdl):
        for port in self.lag_mbr_hdls:
            if mbr_hdl in self.lag_mbr_hdls[port]: return port
        self.assertTrue(0)

    def handle_lag_update(self, u):
        shdl    = u.sess_hdl
        dev_tgt = u.dev_tgt
        cookie  = u.cookie
        grp_hdl = u.grp_hdl
        mbr_hdl = u.mbr_hdl
        index   = u.index
        is_add  = u.is_add
        # Some sanity checks about the update to process
        self.assertEqual(cookie, self.useless_cookie_2)
        self.assertEqual(shdl, self.shdl)
        self.assertEqual(dev_tgt, self.dt)
        self.assertIn(grp_hdl, self.lag_groups)
        self.assertIn(mbr_hdl, self.all_lag_mbr_hdls)
        self.assertGreaterEqual(index, 0)
        self.assertLess(index, LAG_REG_SIZE)
        self.assertLess(index % 128, 120)

        port = self.lag_mbr_hdl_to_port(mbr_hdl)
        if is_add:
            pkt_id = len(self.port_to_lag_tbl_idx_list[port])
            mspec = stful_egr_ifid_fast_update_make_key_match_spec_t(hex_to_i16(port), hex_to_i16(pkt_id))
            aspec = stful_set_lag_fast_update_key_action_spec_t(hex_to_i32(index))
            h = self.client.egr_ifid_fast_update_make_key_table_add_with_set_lag_fast_update_key(shdl, dev_tgt, mspec, aspec)
            self.port_to_lag_tbl_idx_list[port].append( index )
            self.lag_index_to_hdl[index] = h
            self.lag_upd_hdl_to_index[h] = index
        else:
            for i in self.port_to_lag_tbl_idx_list[port]:
                h = self.lag_index_to_hdl.pop(i)
                self.lag_upd_hdl_to_index.pop(h)
                self.client.egr_ifid_fast_update_make_key_table_delete(shdl, dev_id, h)
            self.port_to_lag_tbl_idx_list[port].remove(index)
            for i in self.port_to_lag_tbl_idx_list[port]:
                pkt_id = self.port_to_lag_tbl_idx_list[port].index(i)
                mspec = stful_egr_ifid_fast_update_make_key_match_spec_t(hex_to_i16(port), hex_to_i16(pkt_id))
                aspec = stful_set_lag_fast_update_key_action_spec_t(hex_to_i32(i))
                h = self.client.egr_ifid_fast_update_make_key_table_add_with_set_lag_fast_update_key(shdl, dev_tgt, mspec, aspec)
                self.lag_index_to_hdl[i] = h
                self.lag_upd_hdl_to_index[h] = i
        self.assertEqual(len(self.lag_index_to_hdl), len(self.lag_upd_hdl_to_index))

        l = 0
        for key in self.port_to_lag_tbl_idx_list:
            l = max(l, len(self.port_to_lag_tbl_idx_list[key]))
        if l>0 and l-1 != self.port_down_app.pkt_count:
            self.port_down_app.pkt_count = l-1
            self.conn_mgr.pktgen_cfg_app( self.shdl, self.dt, self.port_down_app_id, self.port_down_app)

    def lag_group_handle_to_mbr_ports(self, grp_hdl):
        mbr_ports = set()
        for mbr_hdl in self.lag_grp_to_mbrs[grp_hdl]:
            for port in self.lag_mbr_hdls:
                if mbr_hdl in self.lag_mbr_hdls[port]:
                    mbr_ports = mbr_ports | set([port])
        return mbr_ports

    def verify_lag_stateful_tbl(self, down_ports=[]):
        # Verify that for each index used in the selection table the
        # bit at that index is set and for each index not used in the
        # selection table the bit at that index is not set.
        print(datetime.datetime.now(), "Syncing LAG selection table...")
        sys.stdout.flush()
        self.client.register_hw_sync_lag_reg(self.shdl, self.dt)
        print(datetime.datetime.now(), "Verifying selection bit values...")
        sys.stdout.flush()
        up_indices = {i for port, indices in self.port_to_lag_tbl_idx_list.items()
                      if port not in down_ports for i in indices}
        chunk = 1024
        for start in range(0, LAG_REG_SIZE, chunk):
            count = min(chunk, LAG_REG_SIZE - start)
            values = self.client.register_range_read_lag_reg(
                self.shdl, self.dt, start, count, self.async_flag)
            self.assertEqual(len(values), count * self.pipe_list_len)
            for offset in range(count):
                i = start + offset
                if i % 128 >= 120:  # The top byte holds the member count.
                    continue
                first = offset * self.pipe_list_len
                self.assertEqual(values[first:first + self.pipe_list_len],
                                 [int(i in up_indices)] * self.pipe_list_len,
                                 "LAG register index %d" % i)
        print(datetime.datetime.now(), "Done...")
        sys.stdout.flush()

    def verify_all_traffic_with_port_flap(self, ports_to_flap):
        for port in ports_to_flap:
            dt = DevTarget_t(dev_id, port_to_pipe(port))
            # Get packet gen app counters for the port down app.
            pkt_cnt_start = self.conn_mgr.pktgen_get_pkt_counter(self.shdl, dt, self.port_down_app_id)

            # Bring down a port
            print(datetime.datetime.now(), "Flapping port", port)
            print("The following indexes should be turned off:", self.port_to_lag_tbl_idx_list[port])
            sys.stdout.flush()
            take_port_down(port)
            time.sleep(2*3) # Wait to ensure model notices the down event
            bring_port_up(port) # Bring it back up so we don't accidently leave it down
            #raw_input("Make port down then up")

            # Wait for pkt-gen to complete.
            print(datetime.datetime.now(), "Waiting for pkt-gen to make", self.port_down_app.pkt_count + 1, "copies")
            sys.stdout.flush()
            pkt_cnt_end = pkt_cnt_start + self.port_down_app.pkt_count + 1
            deadline = time.monotonic() + 30
            while True:
                now = self.conn_mgr.pktgen_get_pkt_counter(self.shdl, dt, self.port_down_app_id)
                if now >= pkt_cnt_end or time.monotonic() >= deadline:
                    self.assertEqual(now, pkt_cnt_end, "Packet generator count for port %d" % port)
                    break
                time.sleep(2)
            # Wait some extra time since the pkt-gen packets are recirculated and multicasted.
            print(datetime.datetime.now(), "Pkt-gen done, waiting extra time for model to recirc, multicast, and process copies")
            sys.stdout.flush()
            time.sleep(self.pgen_trig_wait_time)

            # Clear the port down event in pkt-gen
            print(datetime.datetime.now(), "Clearing port down status in pkt-gen")
            sys.stdout.flush()
            self.conn_mgr.pktgen_clear_port_down( self.shdl, dev_id, port )

            print(datetime.datetime.now(), "Verifying HW was updated")
            sys.stdout.flush()
            self.verify_lag_stateful_tbl([port])

            # Verify that no traffic uses the port.
            exp_pkts = []
            exp_ports = []
            sent_count = 0
            for pkt,grp in zip(self.pkts, self.lag_groups):
                print(datetime.datetime.now(), "Sending pkt %d of %d for group 0x%x" % (self.pkts.index(pkt)+1, len(self.pkts), grp))
                sys.stdout.flush()
                expected_egress_ports = self.lag_group_handle_to_mbr_ports(grp)
                expected_egress_ports = expected_egress_ports - set([port])
                if len(expected_egress_ports):
                    print(datetime.datetime.now(), "  Using", len(self.ingr_ports), "ingress ports")
                    sys.stdout.flush()
                    for iport in self.ingr_ports:
                        send_packet(self, iport, pkt)
                        sent_count += 1
                        exp_pkts.append(pkt)
                        exp_ports.append( list(expected_egress_ports) )
            print(datetime.datetime.now(), "  Expecting", sent_count, "packets")
            verify_packets_any_fast(self, exp_pkts, exp_ports, timeout=sent_count*5)

            # Re-activate the member so traffic can use it again
            print(datetime.datetime.now(), "Reactivating", len(self.lag_mbr_hdls[port]), "members for port", port)
            sys.stdout.flush()
            for mbr_hdl in self.lag_mbr_hdls[port]:
                for grp_hdl in self.lag_groups:
                    if mbr_hdl not in self.lag_grp_to_mbrs[grp_hdl]: continue
                    self.client.lag_ap_group_member_state_set(self.shdl, dev_id, grp_hdl, mbr_hdl, stful_grp_mbr_state.MBR_ACTIVE)
            print(datetime.datetime.now(), "Getting selection table updates...")
            sys.stdout.flush()
            updates = self.client.lag_ap_sel_get_updates(dev_id)
            print(datetime.datetime.now(), "  Done")
            print(datetime.datetime.now(), "Handling", len(updates), "LAG updates")
            sys.stdout.flush()
            for u in updates:
                self.handle_lag_update(u)
            print(datetime.datetime.now(), "  Done")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(self.shdl)

    def runTest(self):
        # For each group, add some handles
        print(datetime.datetime.now(), "Adding members to selection groups...")
        self.conn_mgr.begin_batch(self.shdl)
        self.batching = True
        self.lag_grp_to_mbrs = {}
        for grp in self.lag_groups:
            print(datetime.datetime.now(), "  Group", hex(grp))
            sys.stdout.flush()
            mbrs = random.sample(sorted(self.all_lag_mbr_hdls), self.num_mbrs_per_lag)
            self.lag_grp_to_mbrs[grp] = mbrs
            for m in mbrs:
                self.client.lag_ap_add_member_to_group(self.shdl, dev_id, grp, m)
                updates = self.client.lag_ap_sel_get_updates(dev_id)
                for u in updates:
                    self.handle_lag_update(u)
        self.conn_mgr.end_batch(self.shdl, False)
        self.batching = False
        print(datetime.datetime.now(), "Done...")

        # Enable the port down pkt-gen app.
        self.conn_mgr.pktgen_app_enable( self.shdl, self.dt, self.port_down_app_id )

        # Use a few different routes to get different next-hops which give
        # different eifids.
        num_groups = len(self.lag_groups)
        self.pkts = []
        for x,y in zip(list(range(1, num_groups+1)), self.lag_groups):
            ip = "100.100.100.%d" % x
            self.pkts.append( simple_tcp_packet(ip_dst=ip, pktlen=90+x-1) )
            # Route 100.100.100.x maps to nh x
            self.rte_tbl.add(ip, 32, x)
            # Next hop x maps to eifid x
            self.nh_tbl.add(x, x)
            # eifid x maps to group y
            self.eifid_hdls.add( self.client.egr_ifid_add_entry_with_selector(self.shdl, self.dt,
                                                                              stful_egr_ifid_match_spec_t(hex_to_i16(x)),
                                                                              y) )
        self.conn_mgr.complete_operations(self.shdl)

        self.verify_lag_stateful_tbl()

        # Verify that each port can be flapped and traffic will stop using
        # that port
        #self.verify_all_traffic_with_port_flap(self.egr_ports)

        # Shuffle members in the groups
        print(datetime.datetime.now(), "Shuffling members...")
        sys.stdout.flush()
        self.conn_mgr.begin_batch(self.shdl)
        self.batching = True
        for grp in self.lag_groups:
            print("  shuffling grp", hex(grp))
            # Ensure at least one member remains in the group
            to_rmv = random.sample(self.lag_grp_to_mbrs[grp], random.randint(0, len(self.lag_grp_to_mbrs[grp]) - 1))
            for mbr_hdl in to_rmv:
                self.client.lag_ap_del_member_from_group(self.shdl, dev_id, grp, mbr_hdl)
                updates = self.client.lag_ap_sel_get_updates(dev_id)
                for u in updates:
                    self.handle_lag_update(u)
                self.lag_grp_to_mbrs[grp].remove(mbr_hdl)
            could_add = set(self.all_lag_mbr_hdls) - set(self.lag_grp_to_mbrs[grp])
            to_add = random.sample(sorted(could_add), random.randint(0, self.num_mbrs_per_lag-len(self.lag_grp_to_mbrs[grp])))
            self.lag_grp_to_mbrs[grp] = self.lag_grp_to_mbrs[grp] + to_add
            for m in to_add:
                self.client.lag_ap_add_member_to_group(self.shdl, dev_id, grp, m)
                updates = self.client.lag_ap_sel_get_updates(dev_id)
                for u in updates:
                    self.handle_lag_update(u)
        self.conn_mgr.end_batch(self.shdl, True)
        self.batching = False
        print(datetime.datetime.now(), "Done...")
        sys.stdout.flush()

        # Verify that each port can be flapped and traffic will stop using
        # that port
        ports_to_test = self.egr_ports
        self.verify_all_traffic_with_port_flap(ports_to_test)


    def tearDown(self):
        print(datetime.datetime.now(), "Tear down started")
        sys.stdout.flush()
        if self.batching is False:
            self.conn_mgr.begin_batch(self.shdl)
        for h in self.eifid_hdls:
            self.client.egr_ifid_table_delete(self.shdl, dev_id, h)
        for grp in self.lag_groups:
            if grp in self.lag_grp_to_mbrs:
                for mbr in self.lag_grp_to_mbrs[grp]:
                    self.client.lag_ap_del_member_from_group(self.shdl, dev_id, grp, mbr)
            self.client.lag_ap_del_group(self.shdl, dev_id, grp)
        for mbr in self.all_lag_mbr_hdls:
            self.client.lag_ap_del_member(self.shdl, dev_id, mbr)
        self.conn_mgr.pktgen_app_disable( self.shdl, self.dt, self.port_down_app_id )
        self.conn_mgr.pktgen_app_disable( self.shdl, self.dt, self.recirc_app_id )
        updates = self.client.lag_ap_sel_get_updates(dev_id)
        for u in updates:
            self.handle_lag_update(u)
        self.assertEqual(len(self.lag_index_to_hdl), 0)
        self.assertEqual(len(self.lag_upd_hdl_to_index), 0)
        for h in self.pass_1_hdls:
            self.client.prepare_for_recirc_table_delete(self.shdl, dev_id, h)
        for p in range(self.pipe_list_len):
            self.conn_mgr.pktgen_disable( self.shdl, dev_id, make_port(p,68) )
        speed_100g = 64
        for p in self.recirc_ports:
            # Delete the ports, change recirculation config and add ports back
            for port in range(0, 4):
                try:
                    self.devport_mgr.devport_mgr_remove_port(dev_id, p+port)
                except InvalidDevportMgrOperation as e:
                    pass
            self.conn_mgr.recirculation_disable(self.shdl, dev_id, p);
            self.devport_mgr.devport_mgr_add_port(dev_id, p, speed_100g, 0)
        self.mc.mc_dissociate_node(self.mc_shdl, dev_id, hex_to_i32(self.mgid_hdl), hex_to_i32(self.l1_hdl))
        self.mc.mc_mgrp_destroy(self.mc_shdl, dev_id, hex_to_i32(self.mgid_hdl))
        self.mc.mc_node_destroy(self.mc_shdl, dev_id, hex_to_i32(self.l1_hdl))

        self.port_tbl.rmv_all()
        self.ifid_tbl.rmv_all()
        self.smpl_tbl.rmv_all()
        self.rte_tbl.rmv_all()
        self.nh_tbl.rmv_all()
        self.egr_port_tbl.rmv_all()
        self.port_tbl.set_sym(True)
        self.ifid_tbl.set_sym(True)
        self.conn_mgr.end_batch(self.shdl, False)

        self.conn_mgr.client_cleanup(self.shdl)
        self.mc.mc_destroy_session(self.mc_shdl)

        print(datetime.datetime.now(), "Tear down complete")
        sys.stdout.flush()



class TestStfulSelTbl2(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def setUp(self):
        pd_base_tests.ThriftInterfaceDataPlane.setUp(self)
        self.num_pipes = int(test_param_get('num_pipes'))
        self.pipe_list = [x for x in range(self.num_pipes)]
        self.pipe_list_len = len(self.pipe_list)

        self.ingr_ports = list(swports)
        self.egr_ports = list(swports)
        self.test_ifids = [x for x in self.pipe_list]
        # Use port 68 in each pipe for recirculation
        self.recirc_ports = []
        for pipe in range(self.pipe_list_len):
            self.recirc_ports.append(make_port(pipe, 68))
        for rp in self.recirc_ports:
            if rp in self.ingr_ports:
                self.ingr_ports.remove(rp)
            if rp in self.egr_ports:
                self.egr_ports.remove(rp)
        # Temporarily skip port 64
        if 64 in self.ingr_ports:
            self.ingr_ports.remove(64)
        if 64 in self.egr_ports:
            self.egr_ports.remove(64)

        self.num_ecmps = 1
        self.ecmp_size = 200

        # Create sessions for pipeline driver and multicast driver.
        self.shdl = hex_to_i32( self.conn_mgr.client_init() )
        self.mc_shdl = hex_to_i32( self.mc.mc_create_session() )

        # Use a dev_target for all pipes for most operations.
        self.dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        self.nhid_to_tbl_idx_list = {}
        self.ecmp_index_to_hdl = {}
        self.ecmp_upd_hdl_to_index = {}

        # Create helper classes for the usual tables
        self.port_tbl = PortTable(self, self.shdl, dev_id)
        self.ifid_tbl = IfidTbl(self, self.shdl, dev_id, self.port_tbl)
        self.smpl_tbl = SamplerTable(self, self.shdl, dev_id)
        self.rte_tbl  = RteTbl(self, self.shdl, dev_id)
        self.nh_tbl   = NextHopTbl(self, self.shdl, dev_id)
        self.egr_port_tbl = EgressPortTbl(self, self.shdl, dev_id)
        self.bf_tbl   = BloomFilterTbl(self, self.shdl, dev_id)

        # Define the pkt-gen app ids that will be needed
        self.port_down_app_id = 1
        self.recirc_app_id = 2

        # Define the pkt-gen initial app configurations.
        self.port_down_app = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.PORT_DOWN,
                                            batch_count=0, pkt_count=0,
                                            pattern_key=0, pattern_msk=0,
                                            timer=0,
                                            ibg=0, ibg_jitter=0,
                                            ipg=0, ipg_jitter=0,
                                            src_port=68, src_port_inc=0,
                                            buffer_offset=0, length=60 )
        self.recirc_app = PktGenAppCfg_t(trigger_type=PktGenTriggerType_t.RECIRC_PATTERN,
                                         batch_count=0, pkt_count=0,
                                         pattern_key=hex_to_i32(0xF2000000), pattern_msk=hex_to_i32(0xFF000000),
                                         timer=0,
                                         ibg=0, ibg_jitter=0,
                                         ipg=0, ipg_jitter=0,
                                         src_port=68, src_port_inc=0,
                                         buffer_offset=0, length=60 )
        self.conn_mgr.pktgen_cfg_app( self.shdl, self.dt, self.port_down_app_id, self.port_down_app)
        self.conn_mgr.pktgen_cfg_app( self.shdl, self.dt, self.recirc_app_id, self.recirc_app)

        # Define the mgid to use for recirculation to all pipes
        self.mgid = 0

        # Set up a multicast tree using our mgid and recirc ports.
        self.l1_hdl = self.mc.mc_node_create(self.mc_shdl, dev_id, 0, set_port_map(self.recirc_ports), set_lag_map([]))
        self.mgid_hdl = self.mc.mc_mgrp_create(self.mc_shdl, dev_id, hex_to_i16(self.mgid))
        self.mc.mc_associate_node(self.mc_shdl, dev_id,
                                  hex_to_i32(self.mgid_hdl),
                                  hex_to_i32(self.l1_hdl),
                                  0, False)

        # Enable packet gen and recirculation and recirculation snooping.
        for p in range(self.pipe_list_len):
            self.conn_mgr.pktgen_enable( self.shdl, dev_id, make_port(p,68) )
        for p in self.recirc_ports:
            #self.conn_mgr.recirculation_enable(self.shdl, dev_id, p);
            self.conn_mgr.pktgen_enable_recirc_pattern_matching( self.shdl, dev_id, p )

        # Enable update tracking on the two stateful selection tables.
        self.useless_cookie_1 = 0x7BC12300
        self.client.next_hop_ecmp_ap_sel_track_updates(dev_id, hex_to_i32(self.useless_cookie_1))
        self.useless_cookie_2 = 0x12345678
        self.client.lag_ap_sel_track_updates(dev_id, hex_to_i32(self.useless_cookie_2))

        # Create a few ECMP selection groups
        self.ecmp_groups = set()
        for _ in range(self.num_ecmps):
            grp_hdl = self.client.next_hop_ecmp_ap_create_group(self.shdl, self.dt, self.ecmp_size)
            self.ecmp_groups.add( grp_hdl )

        # Add some next hops to the action data table and match tbl.
        self.nhids = set()
        for egr_port in self.egr_ports:
            x = egr_port | 0x4000
            self.nhids = self.nhids | set([x])
            self.nh_tbl.add(x, x)
            self.egr_port_tbl.add(x, egr_port)
        for nhid in self.nhids:
            self.nhid_to_tbl_idx_list[nhid] = []

        self.nh_mbr_hdls = set()
        self.nhid_to_hdl = {}
        self.hdl_to_nhid = {}
        for nh in self.nhids:
            aspec = stful_set_next_hop_action_spec_t(nh)
            hdl = self.client.next_hop_ecmp_ap_add_member_with_set_next_hop(self.shdl, self.dt, aspec)
            self.nh_mbr_hdls.add( hdl )
            self.nhid_to_hdl[nh] = hdl
            self.hdl_to_nhid[hdl] = nh

        # Add the next hops to the group
        for grp_hdl in self.ecmp_groups:
            for mbr_hdl in self.nh_mbr_hdls:
                self.client.next_hop_ecmp_ap_add_member_to_group(self.shdl, dev_id, grp_hdl, mbr_hdl)
        updates = self.client.next_hop_ecmp_ap_sel_get_updates(dev_id)
        for u in updates:
            self.handle_ecmp_update(u)

        # Enable the recirc pkt-gen app.
        self.conn_mgr.pktgen_app_enable( self.shdl, self.dt, self.recirc_app_id )

        # Populate the minimum set of table entries to get packets through.
        self.port_tbl.set_sym(False)
        self.ifid_tbl.set_sym(False)
        set_of_pipes = set()
        for port in self.ingr_ports:
            pipe = port_to_pipe(port)
            ifid = self.test_ifids[pipe]
            self.port_tbl.add(port,ifid)
            if pipe not in set_of_pipes:
                self.ifid_tbl.add(ifid, ts=0, offset=0)
                set_of_pipes.add(pipe)
        self.smpl_tbl.add(None)

        self.rte_tbl.add('1.2.3.4', 32, 0, ecmp=True)
        mspec = stful_next_hop_ecmp_match_spec_t( hex_to_i16(0) )
        self.h1 = self.client.next_hop_ecmp_add_entry_with_selector(self.shdl, self.dt, mspec, list(self.ecmp_groups)[0])

        self.conn_mgr.complete_operations(self.shdl)


    def handle_ecmp_update(self, u):
        shdl    = u.sess_hdl
        dev_tgt = u.dev_tgt
        cookie  = u.cookie
        grp_hdl = u.grp_hdl
        mbr_hdl = u.mbr_hdl
        index   = u.index
        is_add  = u.is_add
        # Some sanity checks about the update to process
        self.assertEqual(cookie, self.useless_cookie_1)
        self.assertEqual(shdl, self.shdl)
        self.assertEqual(dev_tgt, self.dt)
        self.assertIn(grp_hdl, self.ecmp_groups)
        self.assertIn(mbr_hdl, self.nh_mbr_hdls)
        self.assertGreaterEqual(index, 0)
        self.assertLess(index, ECMP_REG_SIZE)
        self.assertLess(index % 128, 120)

        nhid = self.hdl_to_nhid[mbr_hdl]
        if is_add:
            pkt_id = len(self.nhid_to_tbl_idx_list[nhid])
            mspec = stful_make_key_ecmp_fast_update_match_spec_t(hex_to_i32(nhid), hex_to_i16(pkt_id))
            aspec = stful_set_ecmp_fast_update_key_action_spec_t(hex_to_i32(index))
            h = self.client.make_key_ecmp_fast_update_table_add_with_set_ecmp_fast_update_key(shdl, dev_tgt, mspec, aspec)
            self.nhid_to_tbl_idx_list[nhid].append( index )
            self.ecmp_index_to_hdl[index] = h
            self.ecmp_upd_hdl_to_index[h] = index
        else:
            for i in self.nhid_to_tbl_idx_list[nhid]:
                h = self.ecmp_index_to_hdl.pop(i)
                self.ecmp_upd_hdl_to_index.pop(h)
                self.client.make_key_ecmp_fast_update_table_delete(shdl, dev_id, h)
            self.nhid_to_tbl_idx_list[nhid].remove(index)
            for i in self.nhid_to_tbl_idx_list[nhid]:
                pkt_id = self.nhid_to_tbl_idx_list[nhid].index(i)
                mspec = stful_make_key_ecmp_fast_update_match_spec_t(hex_to_i32(nhid), hex_to_i16(pkt_id))
                aspec = stful_set_ecmp_fast_update_key_action_spec_t(hex_to_i32(i))
                h = self.client.make_key_ecmp_fast_update_table_add_with_set_ecmp_fast_update_key(shdl, dev_tgt, mspec, aspec)
                self.ecmp_index_to_hdl[i] = h
                self.ecmp_upd_hdl_to_index[h] = i
        self.assertEqual(len(self.ecmp_index_to_hdl), len(self.ecmp_upd_hdl_to_index))

        l = 0
        for key in self.nhid_to_tbl_idx_list:
            l = max(l, len(self.nhid_to_tbl_idx_list[key]))
        if l>0 and l-1 != self.recirc_app.pkt_count:
            self.recirc_app.pkt_count = l-1
            self.conn_mgr.pktgen_cfg_app( self.shdl, self.dt, self.recirc_app_id, self.recirc_app)

    def runTest(self):

        self.pkt = []
        self.pkt_dst = []
        sip = "100.100.100.5"
        self.pkt.append( simple_tcp_packet(ip_dst='1.2.3.4', ip_src=sip) )
        self.pkt_dst.append( self.egr_ports )

        down = []
        for i in range(len(self.egr_ports) - 1):
            print("Sending for iteration", i)
            sys.stdout.flush()
            send_packet(self, self.ingr_ports[0], self.pkt[0])
            dst = verify_packets_any_fast(self, self.pkt, self.pkt_dst)
            print("\tPacket came on port", dst)
            sys.stdout.flush()

            nh = dst[0] | 0x4000
            print("\tMarking next hop id 0x%x as down" % nh)
            sys.stdout.flush()
            self.nh_tbl.set_down(nh, self.mgid)
            down.append( dst[0] )
            self.conn_mgr.complete_operations(self.shdl)
            self.pkt_dst[0].remove( dst[0] )
            print("\tThese ports should not see traffic:", down)
            sys.stdout.flush()

            print("\tSending traffic which will mark next hops down.  Ingress port is", self.ingr_ports[0])
            sys.stdout.flush()
            send_packet(self, self.ingr_ports[0], self.pkt[0])
            time.sleep(3*1)
            verify_no_other_packets(self)

            print("\tSending traffic again... Expect Rx on one of:", self.pkt_dst)
            sys.stdout.flush()
            send_packet(self, self.ingr_ports[0], self.pkt[0])
            dst = verify_packets_any_fast(self, self.pkt, self.pkt_dst)
            print("\tPacket came on port", dst[0])
            sys.stdout.flush()
            self.assertNotIn(dst[0], down)


    def tearDown(self):
        self.conn_mgr.pktgen_app_disable( self.shdl, self.dt, self.port_down_app_id )
        self.conn_mgr.pktgen_app_disable( self.shdl, self.dt, self.recirc_app_id )
        for p in range(self.pipe_list_len):
            self.conn_mgr.pktgen_disable( self.shdl, dev_id, make_port(p,68) )
        for p in self.recirc_ports:
            #self.conn_mgr.recirculation_disable(self.shdl, dev_id, p);
            self.conn_mgr.pktgen_disable_recirc_pattern_matching( self.shdl, dev_id, p )
        self.mc.mc_dissociate_node(self.mc_shdl, dev_id, hex_to_i32(self.mgid_hdl), hex_to_i32(self.l1_hdl))
        self.mc.mc_mgrp_destroy(self.mc_shdl, dev_id, hex_to_i32(self.mgid_hdl))
        self.mc.mc_node_destroy(self.mc_shdl, dev_id, hex_to_i32(self.l1_hdl))

        self.client.next_hop_ecmp_table_delete(self.shdl, dev_id, self.h1)
        self.port_tbl.rmv_all()
        self.ifid_tbl.rmv_all()
        self.smpl_tbl.rmv_all()
        self.rte_tbl.rmv_all()
        self.nh_tbl.rmv_all()
        self.egr_port_tbl.rmv_all()
        self.port_tbl.set_sym(True)
        self.ifid_tbl.set_sym(True)

        for grp in self.ecmp_groups:
            for mbr in self.nh_mbr_hdls:
                self.client.next_hop_ecmp_ap_del_member_from_group(self.shdl, dev_id, grp, mbr)
        updates = self.client.next_hop_ecmp_ap_sel_get_updates(dev_id)
        for u in updates:
            self.handle_ecmp_update(u)

        for grp in self.ecmp_groups:
            self.client.next_hop_ecmp_ap_del_group(self.shdl, dev_id, grp)

        for mbr in self.nh_mbr_hdls:
            self.client.next_hop_ecmp_ap_del_member(self.shdl, dev_id, mbr)

        self.conn_mgr.client_cleanup(self.shdl)
        self.mc.mc_destroy_session(self.mc_shdl)








def verify_packets_any_fast(test, pkts, ports, device_number=0, timeout=30):
    test.assertEqual(len(pkts), len(ports))
    expected = [bytes(pkt) for pkt in pkts]
    rx_ports = [None] * len(pkts)
    for _ in pkts:
        _, rcv_port, rcv_pkt, _ = dp_poll(
            test, device_number=device_number, timeout=timeout
        )
        if rcv_pkt is None:
            missing = [i for i, port in enumerate(rx_ports) if port is None]
            test.fail("Timed out waiting for packets at indices %s" % missing)
        for i, pkt in enumerate(expected):
            if rx_ports[i] is None and rcv_pkt == pkt and rcv_port in ports[i]:
                rx_ports[i] = rcv_port
                break
        else:
            test.fail("Unexpected packet on port %d:\n%s" % (rcv_port, format_packet(rcv_pkt)))
    return rx_ports


class TestNoKeySymSet(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])
    def runTest(self):
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val_s = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
        prop_val_a = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
        hw_sync_flag = stful_register_flags_t(read_hw_sync = True)

        shdl = self.conn_mgr.client_init()
        self.client.bloom_filter_1_set_property(shdl, dev_id, prop, prop_val_s, 0)

        # Symmetric write, expected to work
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.client.register_write_bloom_filter_1(shdl, dt, 123, 1)

        # Asymmetric write to symmetric table, expected to work
        dt = DevTarget_t(dev_id, hex_to_i16(0))
        self.client.register_write_bloom_filter_1(shdl, dt, 123, 1)

        # Make the table asymmetric
        self.client.bloom_filter_1_set_property(shdl, dev_id, prop, prop_val_a, 0)

        # Symmetric write to asymmetric table, expected to work
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.client.register_write_bloom_filter_1(shdl, dt, 123, 1)

        # Asymmetric write to asymmetric table, expected to work
        dt = DevTarget_t(dev_id, hex_to_i16(0))
        self.client.register_write_bloom_filter_1(shdl, dt, 123, 1)

        self.client.bloom_filter_1_set_property(shdl, dev_id, prop, prop_val_s, 0)
        dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.client.register_write_bloom_filter_1(shdl, dt, 123, 0)
        self.conn_mgr.client_cleanup(hex_to_i32(shdl))

class TestPhase0Iterator(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        try:
            print()
            handles = []
            first_port = 128
            no_of_ports = 5
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            sess_hdl = self.conn_mgr.client_init()

            for port in range(first_port, first_port+no_of_ports):
                match_spec = stful_ing_port_match_spec_t(port)
                action_spec = stful_set_ifid_action_spec_t(port)
                entry_handle = self.client.ing_port_table_add_with_set_ifid(
                        sess_hdl, dev_tgt,
                        match_spec, action_spec)
                print("Inserted with entry handle ", hex(entry_handle))
                handles.append(entry_handle)

            print("Test: Get First Handle ")
            first_entry_handle = self.client.ing_port_get_first_entry_handle(
                    sess_hdl, dev_tgt)
            print("  Got First entry handle ", first_entry_handle)
            self.assertIn(first_entry_handle, handles)

            # Get handles one at a time.
            a = []
            h = first_entry_handle
            while h != -1:
                a.append( h )
                try:
                    # Get-Next should return an error when there are no more
                    # handles to get.
                    hh = self.client.ing_port_get_next_entry_handles(sess_hdl, dev_tgt, h, 1)
                    self.assertNotEqual(hh, [-1])
                    h = hh[0]
                except InvalidTableOperation as e:
                    h = -1
            self.assertEqual(len(a), len(handles))
            self.assertEqual(sorted(a), sorted(handles))

            # Get handles back in bulk
            print("Test: Get Next Handles in bulk ")
            no_of_handles = 100
            entry_handle = first_entry_handle
            next_handles = self.client.ing_port_get_next_entry_handles(
                    sess_hdl, dev_tgt,
                    entry_handle, no_of_handles)
            next_handles = next_handles[:-1]
            a = [first_entry_handle]
            for x in next_handles:
                if x == -1: break
                a.append(x)
            self.assertEqual(len(a), len(handles))
            self.assertEqual(sorted(a), sorted(handles))

            self.conn_mgr.complete_operations(sess_hdl)

        finally:
            for hdl in handles:
                self.client.ing_port_table_delete(sess_hdl, dev_id, hdl)
            self.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))


class TestGetEntry(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        ipv4_entries = {}
        sip_entries = {}
        next_hop_ecmp_ap_grp_hdl = None
        ap_entries = {}
        try:
            # Test the sip sampler table
            num_sip_entries = 1000
            keys = [random.randint(0,0xFFFFFFFF) for _ in range(num_sip_entries)]
            for key in keys:
                mspec = stful_sip_sampler_match_spec_t( hex_to_i32(key) )
                act = random.choice(['sample', 'no_sample'])
                if act == 'sample':
                    idx = random.randint(0, 143360-1)
                    aspec = stful_sample_action_spec_t(hex_to_i32(idx))
                    hdl = self.client.sip_sampler_table_add_with_sample(sess_hdl, dev_tgt, mspec, aspec)
                else:
                    aspec = None
                    hdl = self.client.sip_sampler_table_add_with_no_sample(sess_hdl, dev_tgt, mspec)
                sip_entries[hdl] = (mspec, aspec)
            for hdl in sip_entries:
                x = self.client.sip_sampler_get_entry(sess_hdl, dev_id, hdl, False)
                mspec,aspec = sip_entries[hdl]
                self.assertEqual(x.match_spec, mspec)
                if aspec:
                    self.assertEqual(x.action_desc.name, "sample")
                    self.assertEqual(x.action_desc.data.stful_sample, aspec)
                else:
                    self.assertEqual(x.action_desc.name, "no_sample")

            # Test the ipv4 route table
            num_ipv4_entries = 511
            keys = []
            lpm_lens = []
            for _ in range(num_ipv4_entries):
                key = random.randint(0, 0xFFFFFFFF)
                lpm_len = random.randint(0, 32)
                key = key & ((0xFFFFFFFF << (32 - lpm_len)) & 0xFFFFFFFF)
                while key in keys:
                    key = random.randint(0, 0xFFFFFFFF)
                    lpm_len = random.randint(0, 32)
                    key = key & ((0xFFFFFFFF << (32 - lpm_len)) & 0xFFFFFFFF)
                keys.append(key)
                lpm_lens.append(lpm_len)
            for key , lpm_len in zip(keys, lpm_lens):
                mspec = stful_ipv4_route_match_spec_t( hex_to_i32(key), lpm_len )
                act = random.choice(['set_next_hop', 'set_ecmp'])
                val = random.randint(0, 0xFFFF)
                if act == 'set_next_hop':
                    aspec = stful_set_next_hop_action_spec_t(hex_to_i16(val))
                    hdl = self.client.ipv4_route_table_add_with_set_next_hop(sess_hdl, dev_tgt, mspec, aspec)
                else:
                    aspec = stful_set_ecmp_action_spec_t(hex_to_i16(val))
                    hdl = self.client.ipv4_route_table_add_with_set_ecmp(sess_hdl, dev_tgt, mspec, aspec)
                ipv4_entries[hdl] = (mspec, aspec, act)
            for hdl in ipv4_entries:
                x = self.client.ipv4_route_get_entry(sess_hdl, dev_id, hdl, False)
                mspec,aspec,aname = ipv4_entries[hdl]
                self.assertEqual(x.match_spec, mspec)
                if aname == 'set_next_hop':
                    self.assertEqual(x.action_desc.name, "set_next_hop")
                    self.assertEqual(x.action_desc.data.stful_set_next_hop, aspec)
                else:
                    self.assertEqual(x.action_desc.name, "set_ecmp")
                    self.assertEqual(x.action_desc.data.stful_set_ecmp, aspec)

            # Test the next_hop_ecmp_ap action profile
            num_ap_entries = 1
            next_hop_ecmp_ap_grp_hdl = self.client.next_hop_ecmp_ap_create_group(sess_hdl, dev_tgt, num_ap_entries)
            keys = random.sample(range(0x10000), num_ap_entries)
            for key in keys:
                a_spec = stful_set_next_hop_action_spec_t( hex_to_i16(key) )
                hdl = self.client.next_hop_ecmp_ap_add_member_with_set_next_hop(sess_hdl, dev_tgt, a_spec)
                self.client.next_hop_ecmp_ap_add_member_to_group(sess_hdl, dev_id, next_hop_ecmp_ap_grp_hdl, hdl)
                ap_entries[hdl] = a_spec
            for hdl in ap_entries:
                x = self.client.next_hop_ecmp_ap_get_member(sess_hdl, dev_id, hdl, False)
                a_spec = ap_entries[hdl]
                self.assertEqual(x.name, "set_next_hop")
                self.assertEqual(x.data.stful_set_next_hop, a_spec)

        finally:
            print("Cleaning up")
            sys.stdout.flush()
            for entry_hdl in ipv4_entries:
                self.client.ipv4_route_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in sip_entries:
                self.client.sip_sampler_table_delete(sess_hdl, dev_id, entry_hdl)
            for entry_hdl in ap_entries:
                if next_hop_ecmp_ap_grp_hdl:
                    self.client.next_hop_ecmp_ap_del_member_from_group(sess_hdl, dev_id, next_hop_ecmp_ap_grp_hdl, entry_hdl)
                self.client.next_hop_ecmp_ap_del_member(sess_hdl, dev_id, entry_hdl)
            if next_hop_ecmp_ap_grp_hdl:
                self.client.next_hop_ecmp_ap_del_group(sess_hdl, dev_id, next_hop_ecmp_ap_grp_hdl)

            self.conn_mgr.client_cleanup(sess_hdl)

class TestDirectStateRestore(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        entry_hdls = []
        test_ifid = 1
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        try:
            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)

            # Add miscellanous entries
            direct_tcam_setup(self, list(swports), test_ifid, port_tbl)

            # Add the IFID entry
            ifid_tbl.add(test_ifid, ts=0, offset=0x00000064)

            # Set a default next-hop entry to provide the egress ifid.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_egr_ifid_action_spec_t(hex_to_i32(100))
            self.client.next_hop_set_default_action_set_egr_ifid(sess_hdl, dev_tgt, aspec)

            # Add a TCAM entry with a stateful index to test
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            mspec = stful_egr_port_match_spec_t(hex_to_i16(100), hex_to_i16(0xFFFF))
            priority = 100
            aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
            cntr_val = stful_port_cntr_value_t(f0=0,f1=0)
            eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
            entry_hdls.append(eh)
            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet to match it.
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    ip_src='10.0.0.1',
                                    ip_dst='10.0.0.2')
            send_packet(self, swports[0], pkt)
            verify_packet(self, pkt, swports[0])

            # Read the counter and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            pipe_counts = [0,0,0,0]
            pipe = port_to_pipe(swports[0])
            pipe_counts[pipe] += 100
            for p in pipe_list:
                self.assertEqual(x[p], pipe_counts[p])

            # Add more entries to move this entry.
            for i in range(1,26):
                dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
                mspec = stful_egr_port_match_spec_t(hex_to_i16(100-i), hex_to_i16(0xFFFF))
                priority = 100-i
                aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
                cntr_val = stful_port_cntr_value_t(f0=0,f1=0)
                eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
                entry_hdls.append(eh)

                mspec = stful_egr_port_match_spec_t(hex_to_i16(100+i), hex_to_i16(0xFFFF))
                priority = 100+i
                eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
                entry_hdls.append(eh)

            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet to match it.
            for port in swports:
                pipe = port_to_pipe(port)
                send_packet(self, port, pkt)
                verify_packet(self, pkt, swports[0])
                pipe_counts[pipe] += 100

            # Read the counter and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            for p in pipe_list:
                self.assertEqual(x[p], pipe_counts[p])

            # Log and restore the state
            print("Logging state")
            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.log_state(dev_id, "test_log_file".encode())
            print("State logged, cleaning up")
            direct_tcam_tear_down(self, port_tbl, False)
            for eh in entry_hdls:
                self.client.egr_port_table_delete(sess_hdl, dev_id, eh)
            self.client.egr_port_table_reset_default_entry(sess_hdl, dev_tgt)
            ifid_tbl.rmv_all(False)
            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.client_cleanup( sess_hdl )

            print("Restoring state")
            sess_hdl = self.conn_mgr.client_init()
            self.conn_mgr.restore_state(dev_id, "test_log_file".encode())
            print("State restored")

            # Send a packet to match it.
            for port in swports:
                pipe = port_to_pipe(port)
                send_packet(self, port, pkt)
                verify_packet(self, pkt, swports[0])
                pipe_counts[pipe] += 100

            # Read the counter and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            for p in pipe_list:
                self.assertEqual(x[p], pipe_counts[p])

            # Add more entries to move this entry.
            for i in range(26, 51):
                dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
                mspec = stful_egr_port_match_spec_t(hex_to_i16(100-i), hex_to_i16(0xFFFF))
                priority = 100-i
                aspec = stful_set_dest_action_spec_t(hex_to_i32(swports[0]))
                cntr_val = stful_port_cntr_value_t(f0=0,f1=0)
                eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
                entry_hdls.append(eh)

                mspec = stful_egr_port_match_spec_t(hex_to_i16(100+i), hex_to_i16(0xFFFF))
                priority = 100+i
                eh = self.client.egr_port_table_add_with_set_dest(sess_hdl, dev_tgt, mspec, priority, aspec, cntr_val)
                entry_hdls.append(eh)

            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet to match it.
            for port in swports:
                pipe = port_to_pipe(port)
                send_packet(self, port, pkt)
                verify_packet(self, pkt, swports[0])
                pipe_counts[pipe] += 100

            # Read the counter and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            for p in pipe_list:
                self.assertEqual(x[p], pipe_counts[p])

            # Write the counter then read it and verify it.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            self.client.register_write_port_cntr(sess_hdl, dev_id, entry_hdls[0], stful_port_cntr_value_t(0,500))
            self.conn_mgr.complete_operations(sess_hdl)
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            hw_sync_flag = stful_register_flags_t(read_hw_sync = True)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            for pipe in range(pipe_list_len):
                self.assertEqual(x[pipe], 500)
                pipe_counts[pipe] = 500

            # Send a packet to change the counter.
            for port in swports:
                pipe = port_to_pipe(port)
                send_packet(self, port, pkt)
                verify_packet(self, pkt, swports[0])
                pipe_counts[pipe] += 100

            # Sync the table and query the entries again.
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            self.client.register_hw_sync_port_cntr(sess_hdl, dev_tgt)
            hw_sync_flag = stful_register_flags_t(read_hw_sync = False)
            regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[0], hw_sync_flag)
            x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
            self.assertEqual(len(x), pipe_list_len)
            for p in pipe_list:
                self.assertEqual(x[p], pipe_counts[p])
            for i in range(1, len(entry_hdls)):
                regs = self.client.register_read_port_cntr(sess_hdl, dev_tgt, entry_hdls[i], hw_sync_flag)
                x = [i32_to_hex(r.f0) << 32 | i32_to_hex(r.f1) for r in regs]
                self.assertEqual(len(x), pipe_list_len)
                for pipe in range(pipe_list_len):
                    self.assertEqual(x[pipe], 0)


        finally:
            direct_tcam_tear_down(self, port_tbl)
            for eh in entry_hdls:
                self.client.egr_port_table_delete(sess_hdl, dev_id, eh)
            ifid_tbl.rmv_all()
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
            self.client.egr_port_set_property(sess_hdl, dev_id, prop, prop_val, 0)

            self.conn_mgr.complete_operations(sess_hdl)
            self.conn_mgr.client_cleanup( sess_hdl )

class TestIndirectStateRestore(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list_len = len(pipe_list)
        print()
        try:
            test_ports = []
            for pipe in pipe_list:
                for port in swports:
                    if pipe == port_to_pipe(port):
                        test_ports.append(port)
                        break

            sess_hdl = self.conn_mgr.client_init()
            port_tbl = PortTable(self, sess_hdl, dev_id)
            ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)
            smpl_tbl = SamplerTable(self, sess_hdl, dev_id)

            hw_sync = stful_register_flags_t(read_hw_sync = True)

            # Reset the sampler table to the initial value.
            print("Reset sampler register")
            sys.stdout.flush()
            smpl_tbl.reset_all_counters()
            print("  Done")
            sys.stdout.flush()

            # Add an entry for every port
            test_ifid = 0x1
            for port in test_ports:
                port_tbl.add(port, test_ifid)

            # Add the IFID entry
            ifid_tbl.add(test_ifid, ts=0, offset=0)

            # Add dummy IPv4 route entry
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_next_hop_action_spec_t(hex_to_i32(0))
            self.client.ipv4_route_set_default_action_set_next_hop(sess_hdl, dev_tgt, aspec)
            # Add a dummy next hop entry
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_egr_ifid_action_spec_t(hex_to_i32(0))
            self.client.next_hop_set_default_action_set_egr_ifid(sess_hdl, dev_tgt, aspec)
            # Add a dummy egress port entry
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            aspec = stful_set_dest_action_spec_t(hex_to_i32(test_ports[0]))
            self.client.egr_port_set_default_action_set_dest(sess_hdl, dev_tgt, aspec, stful_port_cntr_value_t(f0=0,f1=0))

            # Set a default entry to point to a counter.
            smpl_tbl.add(None, sample_idx=SIP_SAMPLER_REG_SIZE-1)
            print("Complete operations")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)
            print("  Done")
            sys.stdout.flush()

            # Send traffic to hit the default entry
            for port in test_ports:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='0.0.0.0',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                print("Sending on port", port,"to hit default entry")
                sys.stdout.flush()
                send_packet(self, port, pkt)
                print("  Checking Rx on port", test_ports[0])
                sys.stdout.flush()
                verify_packet(self, pkt, test_ports[0])
                pipe = port_to_pipe(port)
                smpl_tbl.inc_cntr(SIP_SAMPLER_REG_SIZE-1, pipe)

                x = smpl_tbl.get_cntr( SIP_SAMPLER_REG_SIZE-1 )
            verify_no_other_packets(self)

            # Set a default entry to point to no counter.
            smpl_tbl.add(None)
            self.conn_mgr.complete_operations(sess_hdl)
            # Send traffic to hit the default entry
            for port in test_ports:
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='0.0.0.0',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                print("Sending on port", port,"to hit default entry w/o register")
                sys.stdout.flush()
                send_packet(self, port, pkt)
                print("  Checking Rx on port", test_ports[0])
                sys.stdout.flush()
                verify_packet(self, pkt, test_ports[0])
                x = smpl_tbl.get_cntr( SIP_SAMPLER_REG_SIZE-1 )
            verify_no_other_packets(self)

            # Add a few sampler entry with a few stateful pointer values.
            sample_idxs = []
            # Use the last index in the table
            sample_idxs.append(SIP_SAMPLER_REG_SIZE-1)
            # Use a few arbitrary locations
            sample_idxs.extend([20, 1000, 100000, 135000])
            # Use the first row in each of the 35 RAMs
            for ram in range(35):
                base = 1024*4 # 4k 32 bit entries per RAM.
                entry = random.randint(0,3) # One of the four 32-bit entries per row
                sample_idxs.append(ram*base+entry)

            print("Adding", len(sample_idxs), "sampler table entries")
            sys.stdout.flush()
            sip_index_pairs = []
            for i in range(len(sample_idxs)):
                idx = sample_idxs[i]
                ip = "10.1.%d.%d" % (i >> 8, i & 0xFF)
                smpl_tbl.add(ip, sample_idx=idx)
                sip_index_pairs.append( (ip, idx) )

            print("Complete operations")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)
            print("  Done")
            sys.stdout.flush()

            print("Logging state")
            self.conn_mgr.log_state(dev_id, "log_test_file".encode())
            print("State logged, starting cleanup")
            sys.stdout.flush()
            self.conn_mgr.begin_batch(sess_hdl)
            port_tbl.rmv_all(False)
            ifid_tbl.rmv_all(False)
            self.client.ipv4_route_table_reset_default_entry(sess_hdl, dev_tgt)
            self.client.next_hop_table_reset_default_entry(sess_hdl, dev_tgt)
            self.client.egr_port_table_reset_default_entry(sess_hdl, dev_tgt)
            self.conn_mgr.end_batch(sess_hdl, False)
            self.conn_mgr.begin_batch(sess_hdl)
            smpl_tbl.rmv_all(False)
            self.conn_mgr.end_batch(sess_hdl, False)
            self.conn_mgr.begin_batch(sess_hdl)
            smpl_tbl.reset_all_counters()
            self.conn_mgr.end_batch(sess_hdl, True)
            self.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))

            sess_hdl = self.conn_mgr.client_init()
            print("Restoring state")
            self.conn_mgr.restore_state(dev_id, "log_test_file".encode())
            print("State restored")

            for port in test_ports:
                pipe = port_to_pipe(port)
                print("Sending to port", port, "in pipe", pipe)
                sys.stdout.flush()
                for ip,idx in sip_index_pairs:
                    #print("Sending with", ip)
                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_src=ip,
                                            ip_dst='10.0.0.1',
                                            ip_id=101,
                                            ip_ttl=64)
                    send_packet(self, port, pkt)
                    verify_packet(self, pkt, test_ports[0])
                    smpl_tbl.inc_cntr(idx, pipe)

                    x = smpl_tbl.get_cntr( idx )
                    #print("Index", idx, "--", x)

            verify_no_other_packets(self)

            # Read all the allocated indexes and verify their counts
            print("Verifying counters:", sample_idxs)
            sys.stdout.flush()
            for idx in sample_idxs:
                print("  Checking", idx)
                sys.stdout.flush()
                x = smpl_tbl.get_cntr( idx )
            print("  Done")
            sys.stdout.flush()

            # Let all entries point to one of two counters
            print("Changing all entries to point to one of two registers")
            sys.stdout.flush()
            for i in range( len(sip_index_pairs) ):
                ip,idx = sip_index_pairs[i]
                if i & 1:
                    new_idx = SIP_SAMPLER_REG_SIZE-1
                else:
                    new_idx = 0
                smpl_tbl.set_counter_index(ip, sample_idx=new_idx)
                sip_index_pairs[i] = (ip, new_idx)
            print("  Complete operations")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)
            print("  Done")
            sys.stdout.flush()

            # Send traffic again and verify counts
            for port in test_ports:
                pipe = port_to_pipe(port)
                print("Sending to port", port, "in pipe", pipe)
                sys.stdout.flush()
                for ip,idx in sip_index_pairs:
                    #print("Sending with", ip, "for index", idx, "in pipe", pipe)
                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_src=ip,
                                            ip_dst='10.0.0.1',
                                            ip_id=101,
                                            ip_ttl=64)
                    send_packet(self, port, pkt)
                    verify_packet(self, pkt, test_ports[0])
                    smpl_tbl.inc_cntr(idx, pipe)

                    x = smpl_tbl.get_cntr( idx )
                    #print("Index", idx, "--", x)

            verify_no_other_packets(self)

            # Sync the entire table and read every entry
            print("Verifying all counters")
            sys.stdout.flush()
            smpl_tbl.verify_cntrs()
            print("  Done")
            sys.stdout.flush()

            # Let all entries point to no counter
            print("Modify all entries to not use a register")
            sys.stdout.flush()
            for i in range( len(sip_index_pairs) ):
                ip,idx = sip_index_pairs[i]
                new_idx = None
                smpl_tbl.set_counter_index(ip, sample_idx=new_idx)
                sip_index_pairs[i] = (ip, new_idx)
            print("  Complete operations")
            sys.stdout.flush()
            self.conn_mgr.complete_operations(sess_hdl)
            print("  Done")
            sys.stdout.flush()

            # Send traffic again and verify counts
            for port in test_ports:
                pipe = port_to_pipe(port)
                print("Sending to port", port, "in pipe", pipe)
                sys.stdout.flush()
                for ip,idx in sip_index_pairs:
                    #print("Sending with", ip)
                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_src=ip,
                                            ip_dst='10.0.0.1',
                                            ip_id=101,
                                            ip_ttl=64)
                    send_packet(self, port, pkt)
                    verify_packet(self, pkt, test_ports[0])
            verify_no_other_packets(self)

            # Sync the entire table and read every entry
            print("Verifying all counters")
            sys.stdout.flush()
            smpl_tbl.verify_cntrs()
            print("  Done")
            sys.stdout.flush()

            # Delete all entries and change to asymmetric mode
            print("Removing all entries")
            sys.stdout.flush()
            self.conn_mgr.begin_batch(sess_hdl)
            smpl_tbl.rmv_all()
            self.conn_mgr.end_batch(sess_hdl, False)
            print("Setting asymmetric mode")
            sys.stdout.flush()
            smpl_tbl.set_sym(0)
            # Add to one pipe
            smpl_tbl.add("3.3.3.3", pipe=0, sample_idx=123)
            # Set defaults in each pipe
            if 0 in pipe_list:
                smpl_tbl.add(None, pipe=0, sample_idx=1)
            if 1 in pipe_list:
                smpl_tbl.add(None, pipe=1, sample_idx=2)
            if 2 in pipe_list:
                smpl_tbl.add(None, pipe=2, sample_idx=3)
            if 3 in pipe_list:
                smpl_tbl.add(None, pipe=3, sample_idx=4)
            self.conn_mgr.complete_operations(sess_hdl)
            # Send packets
            for port in test_ports:
                pipe = port_to_pipe(port)
                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='3.3.3.3',
                                        ip_dst='10.0.0.1',
                                        ip_id=101,
                                        ip_ttl=64)
                send_packet(self, port, pkt)
                verify_packet(self, pkt, test_ports[0])
                if pipe == 0:
                    smpl_tbl.inc_cntr(123, pipe)
                else:
                    smpl_tbl.inc_cntr(pipe+1, pipe)
            verify_no_other_packets(self)
            # Ensure only that pipe has counts and other pipes counted on default
            smpl_tbl.verify_cntrs()

            print("Cleaning up after success")
            sys.stdout.flush()

        finally:
            print("Starting Cleanup")
            sys.stdout.flush()
            self.conn_mgr.begin_batch(sess_hdl)
            port_tbl.rmv_all()
            ifid_tbl.rmv_all()
            self.conn_mgr.end_batch(sess_hdl, False)
            ifid_tbl.set_sym(1)
            self.conn_mgr.begin_batch(sess_hdl)
            smpl_tbl.rmv_all()
            self.conn_mgr.end_batch(sess_hdl, False)
            smpl_tbl.set_sym(1)
            self.conn_mgr.begin_batch(sess_hdl)
            smpl_tbl.reset_all_counters()
            self.conn_mgr.end_batch(sess_hdl, True)
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            self.client.sip_sampler_set_default_action_no_sample(sess_hdl, dev_tgt)
            self.conn_mgr.client_cleanup(hex_to_i32(sess_hdl))


class TestSnapshot(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["stful"])

    def verify_tbl_state(self, snap_hdl, pipe, tbl_name, executed, inhibited, hit, hit_hdl):
        t = self.client.snapshot_capture_tbl_data_get(snap_hdl, pipe, tbl_name)
        self.assertEqual(t.executed, executed)
        self.assertEqual(t.inhibited, inhibited)
        self.assertEqual(t.hit, hit)
        self.assertEqual(t.hit_entry_handle, hit_hdl)

    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        setup_random()
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        ing_port = swports[0]
        egr_port = swports[1]
        ing_pipe = port_to_pipe(ing_port)
        ing_ifid = 123
        snap_hdl = 0

        port_tbl = PortTable(self, sess_hdl, dev_id)
        ifid_tbl = IfidTbl(self, sess_hdl, dev_id, port_tbl)
        smpl_tbl = SamplerTable(self, sess_hdl, dev_id)
        rte_tbl = RteTbl(self, sess_hdl, dev_id)
        nh_tbl = NextHopTbl(self, sess_hdl, dev_id)
        egr_port_tbl = EgressPortTbl(self, sess_hdl, dev_id)
        bf_tbl = BloomFilterTbl(self, sess_hdl, dev_id)
        pipe_all = 0xFFFF
        batching = False

        sip_to_hdl = {}
        dip_to_hdl = {}
        egr_ifid_to_hdl = {}

        # Place enough entries in the sampling table to utilize both stages
        sips = set()
        while len(sips) < 770:
            x = random.randint(0,0xFFFFFFFF)
            sip = "%d.%d.%d.%d" % (x >> 24, (x >> 16) & 0xFF, (x >> 8) & 0xFF, x & 0xFF)
            sips.add(sip)

        # Place enough entries in the egr_port table to utilize both stages
        eifids = list(range(15000))

        routes = []
        for i in range(1, 101):
            dip = "200.100.64.%d" % (i)
            nh = i
            eifid = random.choice(eifids)
            routes.append( (dip, 32, nh, eifid) )

        try:
            # Remove CPU Port so we don't get extra packets generated by the
            # sampler table.
            self.devport_mgr.devport_mgr_set_copy_to_cpu(dev_id, False, 0);
            #bf_tbl.reg_reset()
            self.conn_mgr.begin_batch(sess_hdl)
            batching = True
            port_tbl.add(ing_port, ing_ifid)
            ifid_tbl.add(ing_ifid, counter=ing_ifid, ts=0, offset=1)

            smpl_idx = 0
            for sip in sips:
                hdl = smpl_tbl.add(sip, pipe_all, smpl_idx)
                smpl_idx = (smpl_idx + 1) % SIP_SAMPLER_REG_SIZE
                sip_to_hdl[sip] = hdl

            for i in range(len(routes)):
                dip, prefix_len, nh, egr_ifid = routes[i]
                hdl = rte_tbl.add(dip, prefix_len, nh, pipe_all)
                dip_to_hdl[dip] = hdl
                nh_tbl.add(nh, egr_ifid, mode='ZERO', index=i, pipe=pipe_all)

            for eifid in eifids:
                hdl = egr_port_tbl.add(eifid, egr_port, pipe_all, cntr=egr_port)
                egr_ifid_to_hdl[eifid] = hdl
            self.conn_mgr.end_batch(sess_hdl, True)
            batching = False
            #ing_port
            #ifid
            #  one_bit_read_1 <-- inhibit
            #  one_bit_read_2 <-- inhibit
            #  undrop <-- inhibit
            #bloom_filter_1,2,3
            #  bloom_filter_sample <-- inhibit
            #sip_sampler
            #flowlet_prepare
            #ipv4_route - provides next-hop-id
            # next_hop_ecmp <-- inhibit or not executed
            #next_hop - maps next-hop-id to eifid
            #egr_ifid <-- Must be a miss
            #egr_port

            test_sips = random.sample(sorted(sips), 100)
            test_routes = routes
            for sip, (dip, _, nh, eifid) in zip(test_sips, test_routes):
                pkt = simple_tcp_packet(ip_src=sip, ip_dst=dip)
                iport = ing_port
                eport = egr_port
                iifid = ing_ifid
                ifid_hdl = ifid_tbl.get_handle( iifid )
                sip_hdl = sip_to_hdl[sip]
                rte_hdl = dip_to_hdl[dip]
                nh_hdl = nh_tbl.get_handle(nh)
                egr_port_hdl = egr_ifid_to_hdl[eifid]

                # Ingress snapshot for all stages
                snap_hdl = self.client.snapshot_create(dev_tgt, 0, 11, 0)
                print("SnapHdl %d for sip %s dip %s" % (snap_hdl, sip, dip))
                sys.stdout.flush()
                self.assertNotEqual(snap_hdl, 0)
                # Set it to only capture for this packet's IPs but wildcard one
                # byte of source IP just because we can
                trig_spec1 = stful_snapshot_trig_spec_t("ipv4_dip",
                                ipv4Addr_to_i32(dip), ipv4Addr_to_i32(dip))
                x = sip.split(".")
                key = "%s.%s.%s.123" % (x[0], x[1], x[2])
                trig_spec2 = stful_snapshot_trig_spec_t("ipv4_sip",
                                ipv4Addr_to_i32(key), ipv4Addr_to_i32("255.255.255.0"))
                self.client.snapshot_capture_trigger_set(snap_hdl, trig_spec1, trig_spec2)
                # Enable the snapshot
                self.client.snapshot_state_set(snap_hdl, 1, 0)
                snap_state = self.client.snapshot_state_get(snap_hdl, ing_pipe)
                self.assertEqual(snap_state, 1)

                self.conn_mgr.complete_operations(sess_hdl)

                send_packet(self, ing_port, pkt)
                verify_packet(self, pkt, egr_port)

                snap_state = self.client.snapshot_state_get(snap_hdl, ing_pipe)
                self.assertEqual(snap_state, 0)

                # Verify captured table state
                self.verify_tbl_state(snap_hdl, ing_pipe, "ifid", True, False, True, ifid_hdl)

                # Clean up the snapshot
                self.client.snapshot_delete(snap_hdl)
                snap_hdl = 0

        finally:
            if batching:
                self.conn_mgr.end_batch(sess_hdl, False)
            if snap_hdl != 0:
                self.client.snapshot_delete(snap_hdl)
            port_tbl.rmv_all()
            ifid_tbl.rmv_all()
            smpl_tbl.rmv_all()
            rte_tbl.rmv_all()
            nh_tbl.rmv_all()
            egr_port_tbl.rmv_all()
            self.conn_mgr.client_cleanup(sess_hdl)
