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

from conn_mgr_pd_rpc.ttypes import *
from plcmt_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from iterator.p4_pd_rpc.ttypes import *

import os

logger = get_logger()
swports = get_sw_ports()

phy_dev_id = 0
vir_dev_id = 1

class TestIterator(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])
        self.key = 0
        self.h2e = {}

    def one_case(self, tbl_type, shdl, dev_id, add_entry, del_entry, get_entry, set_dflt, get_dflt, clr_dflt, get_first, get_next, get_count, set_property=None, pipes=[0xFFFF]):

        if len(pipes) > 1:
            self.assertTrue(set_property, "Set property function not available")
        sym = False
        for pipe in pipes:
            asym_suffix = ''
            if set_property: 
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                if pipe != 0xFFFF or sym:
                    if sym:
                        asym_suffix = 'Asymmetric'
                    prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
                    sym = False
                else:
                    prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                    sym = True
                set_property(shdl, dev_id, prop, prop_val, 0)
            logger.info("%s: Pipe: %d %s Dev %d", tbl_type, pipe, asym_suffix, dev_id)

            dt = DevTarget_t(dev_id, hex_to_i16(pipe))
            if pipe == 0xFFFF and not sym:
                pipe_range = pipes[:-2]
            else:
                pipe_range = [pipe]
            hdls = [set([]) for _ in range(len(pipe_range))]
            all_hdls = set([])

            # Table is empty, get first and next should fail.
            zero_hdl = 0
            if pipe != 0xFFFF:
                zero_hdl = pipe * (2**30)
                if pipe >= 2:
                    zero_hdl -= 0x100000000
            try:
                x = get_first(shdl, dt)
                msg = tbl_type + ": Get First: " + str(x) + " on " + str(dt)
                self.assertTrue(False, msg)
            except InvalidTableOperation as e:
                pass
            try:
                x = get_next(shdl, dt, zero_hdl, 1)
                msg = tbl_type + ": Get Next: " + str(x)
                self.assertTrue(False, msg)
            except InvalidTableOperation as e:
                pass

            # Set a default entry, it should not be included in the iterators so
            # both get first and get next should fail.
            iter_dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            if set_dflt is not None:
                # Check individual pipes
                for pipe_iter in pipe_range:
                    iter_dt.dev_pipe_id = hex_to_i16(pipe_iter)
                    zero_hdl = 0
                    if pipe_iter != 0xFFFF:
                        zero_hdl = pipe_iter * (2**30)
                        if pipe_iter >= 2:
                            zero_hdl -= 0x100000000
                    default_handle = set_dflt(shdl, iter_dt)
                    try:
                        x = get_first(shdl, iter_dt)
                        msg = tbl_type + ": Get_first returned " + str(x)
                        self.assertTrue(False, msg)
                    except InvalidTableOperation as e:
                        pass
                    try:
                        x = get_next(shdl, iter_dt, zero_hdl, 1)
                        msg = tbl_type + ": Get Next: " + str(x)
                        self.assertTrue(False, msg)
                    except InvalidTableOperation as e:
                        pass
                    if get_dflt is not None:
                        d_hdl = get_dflt(shdl, iter_dt)
                        self.assertTrue(default_handle, d_hdl)

                # Check all pipes in asymmetric mode
                if pipe == 0xFFFF and not sym:
                    iter_dt.dev_pipe_id = hex_to_i16(pipe)
                    try:
                        x = get_first(shdl, iter_dt)
                        msg = tbl_type + ": Get_first returned " + str(x)
                        self.assertTrue(False, msg)
                    except InvalidTableOperation as e:
                        pass
                    try:
                        x = get_next(shdl, iter_dt, 0, 1)
                        msg = tbl_type + ": Get Next: " + str(x)
                        self.assertTrue(False, msg)
                    except InvalidTableOperation as e:
                        pass

                for pipe_iter in pipe_range:
                    iter_dt.dev_pipe_id = hex_to_i16(pipe_iter)
                    clr_dflt(shdl, iter_dt)

            # Add and delete a few entries.
            for _ in range(100):
                iter_dt.dev_pipe_id = hex_to_i16(random.choice(pipe_range))
                hdl = add_entry(shdl, iter_dt)
                if len(pipe_range) == 1:
                    hdls[0].add(hdl)
                else:
                    hdls[iter_dt.dev_pipe_id].add(hdl)
                all_hdls.add(hdl)

            to_del = random.sample(sorted(all_hdls), min(30, len(all_hdls)))
            for h in to_del:
                del_entry(shdl, dev_id, h)
                all_hdls.remove(h)
                for i in range(len(hdls)):
                    if h in hdls[i]:
                        hdls[i].remove(h)

            for _ in range(30):
                iter_dt.dev_pipe_id = hex_to_i16(random.choice(pipe_range))
                hdl = add_entry(shdl, iter_dt)
                if len(pipe_range) == 1:
                    hdls[0].add(hdl)
                else:
                    hdls[iter_dt.dev_pipe_id].add(hdl)
                all_hdls.add(hdl)

            # Get handles one at a time.
            # Check individual pipes
            for pipe_iter in pipe_range:
                iter_dt = DevTarget_t(dev_id, hex_to_i16(pipe_iter))
                a = []
                try:
                    h = get_first(shdl, iter_dt)
                except:
                    # If the first get fails, this pipe must be empty
                    count = get_count(shdl, iter_dt)
                    self.assertEqual(count, 0)
                    h = -1
                while h != -1:
                    a.append( h )
                    try:
                        # Get-Next should return an error when there are no more
                        # handles to get.
                        hh = get_next(shdl, iter_dt, h, 1)
                        self.assertNotEqual(hh, [-1])
                        h = hh[0]
                    except InvalidTableOperation as e:
                        h = -1
                pipe_idx = 0
                if len(pipe_range) > 1:
                    pipe_idx = pipe_iter
                self.assertEqual(len(a), len(hdls[pipe_idx]))
                self.assertEqual(sorted(a), sorted(hdls[pipe_idx]))

            # Check all pipes in asymmetric mode
            if len(pipe_range) > 1:
                iter_dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
                a = []
                h = get_first(shdl, iter_dt)
                while h != -1:
                    a.append( h )
                    try:
                        # Get-Next should return an error when there are no more
                        # handles to get.
                        hh = get_next(shdl, iter_dt, h, 1)
                        self.assertNotEqual(hh, [-1])
                        h = hh[0]
                    except InvalidTableOperation as e:
                        h = -1
                self.assertEqual(len(a), len(all_hdls))
                self.assertEqual(sorted(a), sorted(all_hdls))

            # Get handles several at a time.
            # Check individual pipes
            for pipe_iter in pipe_range:
                iter_dt = DevTarget_t(dev_id, hex_to_i16(pipe_iter))
                try:
                    h = get_first(shdl, iter_dt)
                except:
                    count = get_count(shdl, iter_dt)
                    self.assertEqual(count, 0)
                    h = -1
                a = [h]
                while a[-1] != -1:
                    try:
                        x = get_next(shdl, iter_dt, a[-1], 83)
                    except:
                        # For pipes with exactly 1 or 84 entries, get_next will fail
                        a.append(-1)
                        break
                    for h in x:
                        a.append( h )
                        if -1 == h: break
                a = a[:-1]
                pipe_idx = 0
                if len(pipe_range) > 1:
                    pipe_idx = pipe_iter
                self.assertEqual(len(a), len(hdls[pipe_idx]))
                self.assertEqual(sorted(a), sorted(hdls[pipe_idx]))
                # Get-Next should return an error when there are no more handles to
                # get.
                try:
                    x = get_next(shdl, iter_dt, a[-1], 83)
                    self.assertTrue(False, "Get-Next should have failed")
                except InvalidTableOperation as e:
                    pass

            # Check all pipes in asymmetric mode
            if len(pipe_range) > 1:
                iter_dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
                h = get_first(shdl, iter_dt)
                a = [h]
                while a[-1] != -1:
                    try:
                        x = get_next(shdl, iter_dt, a[-1], 83)
                    except:
                        a.append(-1)
                        break
                    for h in x:
                        a.append( h )
                        if -1 == h: break
                a = a[:-1]
                self.assertEqual(len(a), len(all_hdls))
                self.assertEqual(sorted(a), sorted(all_hdls))
                # Get-Next should return an error when there are no more handles to
                # get.
                try:
                    x = get_next(shdl, iter_dt, a[-1], 83)
                    self.assertTrue(False, "Get-Next should have failed")
                except InvalidTableOperation as e:
                    pass

            # Add a default entry; this handle should not be included.
            if set_dflt is not None:
                if len(pipe_range) > 1:
                    iter_dt.dev_pipe_id = 0
                else:
                    iter_dt.dev_pipe_id = hex_to_i16(pipe)
                default_handle = set_dflt(shdl, iter_dt)
                a = []
                h = get_first(shdl, dt)
                while h != -1:
                    a.append( h )
                    try:
                        h = get_next(shdl, dt, h, 1)
                    except InvalidTableOperation as e:
                        h = [-1]
                    h = h[-1]
                self.assertEqual(len(a), len(all_hdls))
                self.assertEqual(sorted(a), sorted(all_hdls))

            self.conn_mgr.complete_operations(shdl)
            # Verify entries have the same key
            for h in a:
                get_entry(shdl, dev_id, h)

            # Clean Up
            if clr_dflt is not None:
                clr_dflt(shdl, iter_dt)
                clr_dflt(shdl, iter_dt)
                clr_dflt(shdl, iter_dt)
            while len(all_hdls):
                h = get_first(shdl, dt)
                all_hdls.remove( h )
                del_entry(shdl, dev_id, h)
            try:
                h = get_first(shdl, dt)
                self.assertTrue(False, "Extra entries at end of test!")
            except InvalidTableOperation as e:
                pass


        if set_property:
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
            set_property(shdl, dev_id, prop, prop_val, 0)
            logger.info("%s Symmetric Dev %d", tbl_type, dev_id)

    def exm_add(self, shdl, dt):
        ms = iterator_exm_match_spec_t(hex_to_i16(self.key&0x01FF), 0)
        self.key += 1
        h = self.client.exm_table_add_with_n(shdl, dt, ms)
        self.h2e[h] = ms
        return h
    def exm_del(self, shdl, dev_id, handle):
        self.client.exm_table_delete(shdl, dev_id, handle)
    def exm_set_dflt(self, shdl, dt):
        h = self.client.exm_set_default_action_n(shdl, dt)
        return h
    def exm_get_dflt(self, shdl, dt):
        h = self.client.exm_table_get_default_entry_handle(shdl, dt)
        return h
    def exm_clr_dflt(self, shdl, dt):
        self.client.exm_table_reset_default_entry(shdl, dt)
    def exm_get(self, shdl, dev_id, ent_hdl):
        ms2 = self.client.exm_get_entry(shdl, dev_id, ent_hdl, False)
        if dev_id != self.virtual_dev_id:
            ms1 = self.client.exm_get_entry(shdl, dev_id, ent_hdl, True)
            self.assertEqual(ms1, ms2)
        ms = self.h2e[ent_hdl]
        self.assertEqual(ms, ms2.match_spec)
        self.assertEqual('n', ms2.action_desc.name)
    def tcam_add(self, shdl, dt):
        ms = iterator_tcam_match_spec_t(hex_to_i16(self.key&0x01FF), hex_to_i16(0x01FF))
        self.key += 1
        h = self.client.tcam_table_add_with_n(shdl, dt, ms, 0)
        self.h2e[h] = ms
        return h
    def tcam_del(self, shdl, dev_id, handle):
        self.client.tcam_table_delete(shdl, dev_id, handle)
    def tcam_set_dflt(self, shdl, dt):
        h = self.client.tcam_set_default_action_n(shdl, dt)
        return h
    def tcam_get_dflt(self, shdl, dt):
        h = self.client.tcam_table_get_default_entry_handle(shdl, dt)
        return h
    def tcam_clr_dflt(self, shdl, dt):
        self.client.tcam_table_reset_default_entry(shdl, dt)
    def tcam_get(self, shdl, dev_id, ent_hdl):
        ms2 = self.client.tcam_get_entry(shdl, dev_id, ent_hdl, False)
        if dev_id != self.virtual_dev_id:
            ms1 = self.client.tcam_get_entry(shdl, dev_id, ent_hdl, True)
            self.assertEqual(ms1, ms2)
        ms = self.h2e[ent_hdl]
        self.assertEqual(ms, ms2.match_spec)
        self.assertEqual('n', ms2.action_desc.name)
    def ha_add(self, shdl, dt):
        ms = iterator_ha_match_spec_t(hex_to_i16(self.key & 0x1FF))
        self.key += 1
        h = self.client.ha_table_add_with_n(shdl, dt, ms)
        self.h2e[h] = ms
        return h
    def ha_del(self, shdl, dev_id, handle):
        self.client.ha_table_delete(shdl, dev_id, handle)
    def ha_set_dflt(self, shdl, dt):
        h = self.client.ha_set_default_action_n(shdl, dt)
        return h
    def ha_clr_dflt(self, shdl, dt):
        self.client.ha_table_reset_default_entry(shdl, dt)
    def ha_get(self, shdl, dev_id, ent_hdl):
        ms2 = self.client.ha_get_entry(shdl, dev_id, ent_hdl, False)
        if dev_id != self.virtual_dev_id:
            ms1 = self.client.ha_get_entry(shdl, dev_id, ent_hdl, True)
            self.assertEqual(ms1, ms2)
        ms = self.h2e[ent_hdl]
        self.assertEqual(ms, ms2.match_spec)
        self.assertEqual('n', ms2.action_desc.name)
    def alpm_add(self, shdl, dt):
        if test_param_get("arch") == "tofino3":
            ms = iterator_alpm_match_spec_t(hex_to_i16(self.key & 0x7FF), 11)
            self.key += 2
        else:
            ms = iterator_alpm_match_spec_t(hex_to_i16(self.key & 0x1FF), 9)
            self.key += 1
        h = self.client.alpm_table_add_with_n(shdl, dt, ms)
        self.h2e[h] = ms
        return h
    def alpm_del(self, shdl, dev_id, handle):
        self.client.alpm_table_delete(shdl, dev_id, handle)
    def alpm_set_dflt(self, shdl, dt):
        h = self.client.alpm_set_default_action_n(shdl, dt)
        return h
    def alpm_get_dflt(self, shdl, dt):
        h = self.client.alpm_table_get_default_entry_handle(shdl, dt)
        return h
    def alpm_clr_dflt(self, shdl, dt):
        self.client.alpm_table_reset_default_entry(shdl, dt)
    def alpm_get(self, shdl, dev_id, ent_hdl):
        ms2 = self.client.alpm_get_entry(shdl, dev_id, ent_hdl, False)
        if dev_id != self.virtual_dev_id:
            ms1 = self.client.alpm_get_entry(shdl, dev_id, ent_hdl, True)
            self.assertEqual(ms1, ms2)
        ms = self.h2e[ent_hdl]
        self.assertEqual(ms, ms2.match_spec)
        self.assertEqual('n', ms2.action_desc.name)
    def p0_add(self, shdl, dt):
        if dt.dev_pipe_id == hex_to_i16(0xFFFF):
            total_ports = 72 * int(test_param_get('num_pipes'))
            port_index = self.key % total_ports
            port_pipe = port_index // 72
            port_local = port_index % 72
        else:
            port_pipe = dt.dev_pipe_id
            port_local = self.key % 72
        key = (port_pipe << 7) | port_local
        ms = iterator_p0_match_spec_t(hex_to_i16(key))
        d = self.key+7
        a = iterator_N_action_spec_t(d)
        self.key += 1
        h = self.client.p0_table_add_with_N(shdl, dt, ms, a)
        self.h2e[h] = (ms,d)
        return h
    def p0_del(self, shdl, dev_id, handle):
        self.client.p0_table_delete(shdl, dev_id, handle)
    def p0_set_dflt(self, shdl, dt):
        h = self.client.p0_table_get_default_entry_handle(shdl, dt)
        a = iterator_N_action_spec_t(0)
        self.client.p0_table_modify_with_N(shdl, dt.dev_id, h, a)
        return h
    def p0_get_dflt(self, shdl, dt):
        h = self.client.p0_table_get_default_entry_handle(shdl, dt)
        return h
    def p0_clr_dflt(self, shdl, dt):
        self.client.p0_table_reset_default_entry(shdl, dt)
    def p0_get(self, shdl, dev_id, ent_hdl):
        ms2 = self.client.p0_get_entry(shdl, dev_id, ent_hdl, False)
        if dev_id != self.virtual_dev_id:
            ms1 = self.client.p0_get_entry(shdl, dev_id, ent_hdl, True)
            if ms1 != ms2:
                logger.error("Ent hdl %d ms1 port=%d ms1 action=%d ms2 port=%d ms2 action=%d",
                    ent_hdl, ms1.match_spec.ig_intr_md_ingress_port,
                    ms1.action_desc.data.iterator_N.action_x,
                    ms2.match_spec.ig_intr_md_ingress_port,
                    ms2.action_desc.data.iterator_N.action_x)
            self.assertEqual(ms1, ms2)
        ms,d = self.h2e[ent_hdl]
        self.assertEqual(ms, ms2.match_spec)
        aspec = iterator_N_action_spec_t(d)
        self.assertEqual(aspec, ms2.action_desc.data.iterator_N)
        self.assertEqual('N', ms2.action_desc.name)

    def runTest(self):
        setup_random()
        self.virtual_dev_id = 1
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        pipe_list = pipe_list + [0xFFFF, 0xFFFF]

        shdl = None

        shdl = self.conn_mgr.client_init()

        self.one_case("Phy-TCAM",
                      shdl,
                      phy_dev_id,
                      self.tcam_add,
                      self.tcam_del,
                      self.tcam_get,
                      self.tcam_set_dflt,
                      self.tcam_get_dflt,
                      self.tcam_clr_dflt,
                      self.client.tcam_get_first_entry_handle,
                      self.client.tcam_get_next_entry_handles,
                      self.client.tcam_get_entry_count,
                      set_property=self.client.tcam_set_property,
                      pipes=pipe_list)
        self.one_case("Vir-TCAM",
                      shdl,
                      vir_dev_id,
                      self.tcam_add,
                      self.tcam_del,
                      self.tcam_get,
                      self.tcam_set_dflt,
                      self.tcam_get_dflt,
                      self.tcam_clr_dflt,
                      self.client.tcam_get_first_entry_handle,
                      self.client.tcam_get_next_entry_handles,
                      self.client.tcam_get_entry_count,
                      set_property=self.client.tcam_set_property,
                      pipes=pipe_list)
        self.one_case("Phy-Exm",
                      shdl,
                      phy_dev_id,
                      self.exm_add,
                      self.exm_del,
                      self.exm_get,
                      self.exm_set_dflt,
                      self.exm_get_dflt,
                      self.exm_clr_dflt,
                      self.client.exm_get_first_entry_handle,
                      self.client.exm_get_next_entry_handles,
                      self.client.exm_get_entry_count,
                      set_property=self.client.exm_set_property,
                      pipes=pipe_list)
        self.one_case("Vir-Exm",
                      shdl,
                      vir_dev_id,
                      self.exm_add,
                      self.exm_del,
                      self.exm_get,
                      self.exm_set_dflt,
                      self.exm_get_dflt,
                      self.exm_clr_dflt,
                      self.client.exm_get_first_entry_handle,
                      self.client.exm_get_next_entry_handles,
                      self.client.exm_get_entry_count,
                      set_property=self.client.exm_set_property,
                      pipes=pipe_list)
        self.one_case("Phy-HashAction",
                      shdl,
                      phy_dev_id,
                      self.ha_add,
                      self.ha_del,
                      self.ha_get,
                      None,
                      None,
                      None,
                      self.client.ha_get_first_entry_handle,
                      self.client.ha_get_next_entry_handles,
                      self.client.ha_get_entry_count,
                      set_property=self.client.ha_set_property,
                      pipes=pipe_list)
        self.one_case("Vir-HashAction",
                      shdl,
                      vir_dev_id,
                      self.ha_add,
                      self.ha_del,
                      self.ha_get,
                      None,
                      None,
                      None,
                      self.client.ha_get_first_entry_handle,
                      self.client.ha_get_next_entry_handles,
                      self.client.ha_get_entry_count,
                      set_property=self.client.ha_set_property,
                      pipes=pipe_list)
        self.one_case("Phy-ALPM",
                      shdl,
                      phy_dev_id,
                      self.alpm_add,
                      self.alpm_del,
                      self.alpm_get,
                      self.alpm_set_dflt,
                      self.alpm_get_dflt,
                      self.alpm_clr_dflt,
                      self.client.alpm_get_first_entry_handle,
                      self.client.alpm_get_next_entry_handles,
                      self.client.alpm_get_entry_count,
                      set_property=self.client.alpm_set_property,
                      pipes=pipe_list)
        self.one_case("Phy-Phase0",
                      shdl,
                      phy_dev_id,
                      self.p0_add,
                      self.p0_del,
                      self.p0_get,
                      self.p0_set_dflt,
                      self.p0_get_dflt,
                      self.p0_clr_dflt,
                      self.client.p0_get_first_entry_handle,
                      self.client.p0_get_next_entry_handles,
                      self.client.p0_get_entry_count,
                      set_property=self.client.p0_set_property,
                      pipes=pipe_list)
        self.one_case("Vir-Phase0",
                      shdl,
                      vir_dev_id,
                      self.p0_add,
                      self.p0_del,
                      self.p0_get,
                      self.p0_set_dflt,
                      self.p0_get_dflt,
                      self.p0_clr_dflt,
                      self.client.p0_get_first_entry_handle,
                      self.client.p0_get_next_entry_handles,
                      self.client.p0_get_entry_count,
                      set_property=self.client.p0_set_property,
                      pipes=pipe_list)

        if shdl is not None:
            self.conn_mgr.client_cleanup( shdl )
        # Clear any updates this test may have generated
        self.client.get_tbl_updates(vir_dev_id)


class TestPlcmtGet(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])
    def runTest(self):
        setup_random()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]

        shdl = None
        dt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        phy_dt = DevTarget_t(phy_dev_id, hex_to_i16(0xFFFF))

        num_ap_entries = 16
        ap_handles = []
        ap_handle_to_data = {}
        exm_ap_entries = []
        tcam_ap_entries = []
        sel_ap_handles = []
        sel_ap_handle_to_data = {}
        sel_grps = []
        sel_grp_to_mbrs = {}
        exm_sel_entries = []
        tcam_sel_entries = []
        try:
            shdl = self.conn_mgr.client_init()
            self.client.exm_sel_register_mat_update_cb(shdl, vir_dev_id)
            self.client.tcam_sel_register_mat_update_cb(shdl, vir_dev_id)
            self.client.exm_ap_register_mat_update_cb(shdl, vir_dev_id)
            self.client.tcam_ap_register_mat_update_cb(shdl, vir_dev_id)
            self.client.ap_register_adt_update_cb(shdl, vir_dev_id)
            self.client.sel_ap_register_adt_update_cb(shdl, vir_dev_id)
            self.client.sel_ap_register_sel_update_cb(shdl, vir_dev_id)

            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # Add members to action table "ap"
            for _ in range(num_ap_entries):
                # Pick an action
                action = random.choice(['a', 'b', 'c', 'd', 'e'])
                aspec = self.make_action_spec(action)
                handle = self.install_action_spec_to_ap(shdl, dt, action, aspec)
                ap_handles.append(handle)
                ap_handle_to_data[handle] = (action, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            for adt_update in x:
                self.assertEqual(adt_update.update_type, iterator_tbl_update_type.ADT_UPDATE_TYPE)
                self.assertEqual(adt_update.update_data.adt.update_type, iterator_adt_update_type.ADT_UPDATE_ADD)
                self.assertIn(adt_update.update_data.adt.update_params.add.entry_hdl, ap_handles)
                mbr_hdl = adt_update.update_data.adt.update_params.add.entry_hdl
                action, aspec = ap_handle_to_data[adt_update.update_data.adt.update_params.add.entry_hdl]
                e = self.client.ap_get_full_member_info_from_plcmt_data( vir_dev_id, mbr_hdl, adt_update.update_data.adt.update_params.add.drv_data )
                self.assertEqual(e.name, action)
                if action == 'a':
                    self.assertEqual(e.data.iterator_a, aspec)
                elif action == 'b':
                    self.assertEqual(e.data.iterator_b, aspec)
                    # Here also verify invoking the second mbr get API which does not
                    # build indirect resources, because it does not take device id
                    # and mbr hdl. The indirect resources are set to a special value
                    # of all 'f's which is -1 as a signed number up at thrift
                    e = self.client.ap_get_member_from_plcmt_data( adt_update.update_data.adt.update_params.add.drv_data )
                    self.assertEqual(e.data.iterator_b.action_x, aspec.action_x)
                    self.assertEqual(e.data.iterator_b.action_i, -1)
                elif action == 'c':
                    self.assertEqual(e.data.iterator_c, aspec)
                elif action == 'd':
                    self.assertEqual(e.data.iterator_d, aspec)

                    # Here also verify invoking the second mbr get API which does not
                    # build indirect resources, because it does not take device id
                    # and mbr hdl. The indirect resources are set to a special value
                    # of all 'f's which is -1 as a signed number up at thrift
                    e = self.client.ap_get_member_from_plcmt_data( adt_update.update_data.adt.update_params.add.drv_data )
                    self.assertEqual(e.data.iterator_d.action_y, aspec.action_y)
                    self.assertEqual(e.data.iterator_d.action_x, aspec.action_x)
                    self.assertEqual(e.data.iterator_d.action_i, -1)

            # Add a table entry to exm_ap table
            mspec = self.make_exm_ms(False)
            mbr = random.choice( ap_handles )
            handle = self.client.exm_ap_add_entry(shdl, dt, mspec, mbr)
            exm_ap_entries.append(handle)

            # Get the data for the update
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.assertEqual(len(x), 1)
            x = x[0]
            self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x.update_data.mat.update_type, iterator_mat_update_type.MAT_UPDATE_ADD)
            self.assertEqual(x.update_data.mat.update_params.add.action_profile_mbr_exists, True)
            self.assertEqual(x.update_data.mat.update_params.add.action_profile_mbr, mbr)
            e = self.client.exm_ap_get_entry_from_plcmt_data( x.update_data.mat.update_params.add.drv_data )
            self.assertEqual(e.members, [mbr])
            self.assertEqual(e.match_spec, mspec)

            # Add a table entry to tcam_ap table
            mspec = self.make_tcam_ms(False)
            mbr = random.choice( ap_handles )
            handle = self.client.tcam_ap_add_entry(shdl, dt, mspec, mbr)
            tcam_ap_entries.append(handle)

            # Get the data for the update
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.assertEqual(len(x), 1)
            x = x[0]
            self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x.update_data.mat.update_type, iterator_mat_update_type.MAT_UPDATE_ADD)
            self.assertEqual(x.update_data.mat.update_params.add.action_profile_mbr_exists, True)
            self.assertEqual(x.update_data.mat.update_params.add.action_profile_mbr, mbr)
            e = self.client.tcam_ap_get_entry_from_plcmt_data( x.update_data.mat.update_params.add.drv_data )
            self.assertEqual(e.members, [mbr])
            self.assertEqual(e.match_spec, mspec)

            #pdb.set_trace()

            # Create selection groups
            sel_grps.append( self.client.sel_ap_create_group(shdl, dt, 100) )
            sel_grps.append( self.client.sel_ap_create_group(shdl, dt, 100) )
            # Add members to selection groups
            for g in sel_grps:
                sel_grp_to_mbrs[g] = set()
                # Add member to action table "sel_ap"
                action = random.choice( ['a', 'b', 'c', 'd', 'e'] )
                cntr = random.randint(1,999)
                for _ in range(3):
                    # Pick an action
                    aspec = self.make_action_spec(action, cntr)
                    handle = self.install_action_spec_to_sel_ap(shdl, dt, action, aspec)
                    sel_ap_handles.append(handle)
                    sel_ap_handle_to_data[handle] = (action, aspec)
                    self.client.sel_ap_add_member_to_group(shdl, vir_dev_id, g, handle)
                    sel_grp_to_mbrs[g].add( handle )
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            # Check group members in llp
            for update in x:
                if update.update_type == iterator_tbl_update_type.SEL_UPDATE_TYPE and \
                   update.update_data.sel.update_type == iterator_sel_update_type.SEL_UPDATE_GROUP_CREATE:
                    word_index = update.update_data.sel.update_params.grp_create.base_logical_index
                    grp_hdl = update.update_data.sel.update_params.grp_create.group_hdl
                    cnt = self.client.sel_ap_get_word_llp_active_member_count(shdl, phy_dt, word_index)
                    mbr_hdls = self.client.sel_ap_get_word_llp_active_members(shdl, phy_dt, word_index, cnt)
                    assert mbr_hdls == list(sel_grp_to_mbrs[grp_hdl])
            for adt_update in x:
                if adt_update.update_type != iterator_tbl_update_type.ADT_UPDATE_TYPE:
                    continue
                self.assertEqual(adt_update.update_data.adt.update_type, iterator_adt_update_type.ADT_UPDATE_ADD)
                self.assertIn(adt_update.update_data.adt.update_params.add.entry_hdl, sel_ap_handles)
                action, aspec = sel_ap_handle_to_data[adt_update.update_data.adt.update_params.add.entry_hdl]
                mbr_hdl = adt_update.update_data.adt.update_params.add.entry_hdl
                e = self.client.sel_ap_get_full_member_info_from_plcmt_data(vir_dev_id, mbr_hdl, adt_update.update_data.adt.update_params.add.drv_data )
                self.assertEqual(e.name, action)
                if action == 'a':
                    self.assertEqual(e.data.iterator_a, aspec)
                elif action == 'b':
                    self.assertEqual(e.data.iterator_b, aspec)
                elif action == 'c':
                    self.assertEqual(e.data.iterator_c, aspec)
                elif action == 'd':
                    self.assertEqual(e.data.iterator_d, aspec)

            # Add a table entry to exm_sel table
            mspec = self.make_exm_ms(True)
            grp = sel_grps[0]
            handle = self.client.exm_sel_add_entry_with_selector(shdl, dt, mspec, grp)
            exm_sel_entries.append(handle)

            # Get the data for the update
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.assertEqual(len(x), 1)
            x = x[0]
            self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x.update_data.mat.update_type, iterator_mat_update_type.MAT_UPDATE_ADD)
            self.assertEqual(x.update_data.mat.update_params.add.sel_grp_exists, True)
            self.assertEqual(x.update_data.mat.update_params.add.sel_grp_hdl, grp)
            e = self.client.exm_sel_get_entry_from_plcmt_data( x.update_data.mat.update_params.add.drv_data )
            self.assertEqual(e.members, [grp])
            self.assertEqual(e.match_spec, mspec)

            # Add a table entry to tcam_ap table
            mspec = self.make_tcam_ms(True)
            grp = sel_grps[-1]
            handle = self.client.tcam_sel_add_entry_with_selector(shdl, dt, mspec, grp)
            tcam_sel_entries.append(handle)

            # Get the data for the update
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.assertEqual(len(x), 1)
            x = x[0]
            self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x.update_data.mat.update_type, iterator_mat_update_type.MAT_UPDATE_ADD)
            self.assertEqual(x.update_data.mat.update_params.add.sel_grp_exists, True)
            self.assertEqual(x.update_data.mat.update_params.add.sel_grp_hdl, grp)
            e = self.client.tcam_sel_get_entry_from_plcmt_data( x.update_data.mat.update_params.add.drv_data )
            self.assertEqual(e.members, [grp])
            self.assertEqual(e.match_spec, mspec)

        finally:
            if shdl:
                for h in exm_ap_entries:
                    self.client.exm_ap_table_delete(shdl, vir_dev_id, h)
                for h in tcam_ap_entries:
                    self.client.tcam_ap_table_delete(shdl, vir_dev_id, h)
                for h in ap_handles:
                    self.client.ap_del_member(shdl, vir_dev_id, h)

                for h in exm_sel_entries:
                    self.client.exm_sel_table_delete(shdl, vir_dev_id, h)
                for h in tcam_sel_entries:
                    self.client.tcam_sel_table_delete(shdl, vir_dev_id, h)
                for g in sel_grps:
                    for mbr in sel_grp_to_mbrs[g]:
                        self.client.sel_ap_del_member_from_group(shdl, vir_dev_id, g, mbr)
                    self.client.sel_ap_del_group(shdl, vir_dev_id, g)
                for h in sel_ap_handles:
                    self.client.sel_ap_del_member(shdl, vir_dev_id, h)

                x = self.client.get_tbl_updates(vir_dev_id)
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)

                self.conn_mgr.client_cleanup( shdl )

    def make_ipv6_addr(self):
        ipv6_addr = "%04x" % (random.randint(0, 0xFFFF))
        for _ in range(7):
            ipv6_addr += ":%04x" % (random.randint(0, 0xFFFF))
        return ipv6_addr
    def make_action_spec(self, action, cntr=None):
        if not cntr:
            cntr = random.randint(1, 999)
        if action == 'a':
            aspec = iterator_a_action_spec_t( hex_to_i16(random.randint(0, 0xFFFF)) )
        elif action == 'b':
            aspec = iterator_b_action_spec_t( hex_to_i16(random.randint(0, 0xFFFF)), hex_to_i32(cntr) )
        elif action == 'c':
            aspec = iterator_c_action_spec_t( hex_to_i16(random.randint(0, 0xFFFF)) )
        elif action == 'd':
            ipv6_addr = self.make_ipv6_addr()
            flow_lbl = random.randint(0, 0xFFFFF)
            aspec = iterator_d_action_spec_t( ipv6Addr_to_string(ipv6_addr), hex_to_i32(flow_lbl), hex_to_i32(cntr) )
        elif action == 'e':
            aspec = None
        else:
            self.assertTrue(0)
        return aspec
    def install_action_spec_to_ap(self, shdl, dt, action, aspec):
        if action == 'a':
            handle = self.client.ap_add_member_with_a(shdl, dt, aspec)
        elif action == 'b':
            handle = self.client.ap_add_member_with_b(shdl, dt, aspec)
        elif action == 'c':
            handle = self.client.ap_add_member_with_c(shdl, dt, aspec)
        elif action == 'd':
            handle = self.client.ap_add_member_with_d(shdl, dt, aspec)
        elif action == 'e':
            handle = self.client.ap_add_member_with_e(shdl, dt)
        else:
            self.assertTrue(0)
        return handle
    def install_action_spec_to_sel_ap(self, shdl, dt, action, aspec):
        if action == 'a':
            handle = self.client.sel_ap_add_member_with_a(shdl, dt, aspec)
        elif action == 'b':
            handle = self.client.sel_ap_add_member_with_b(shdl, dt, aspec)
        elif action == 'c':
            handle = self.client.sel_ap_add_member_with_c(shdl, dt, aspec)
        elif action == 'd':
            handle = self.client.sel_ap_add_member_with_d(shdl, dt, aspec)
        elif action == 'e':
            handle = self.client.sel_ap_add_member_with_e(shdl, dt)
        else:
            self.assertTrue(0)
        return handle
    def make_exm_ms(self, sel):
        val = random.randint(0,1)
        srcAddr = self.make_ipv6_addr()
        dstAddr = self.make_ipv6_addr()
        if sel:
            return iterator_exm_sel_match_spec_t(hex_to_byte(val), ipv6Addr_to_string(srcAddr), ipv6Addr_to_string(dstAddr))
        else:
            return iterator_exm_ap_match_spec_t(hex_to_byte(val), ipv6Addr_to_string(srcAddr), ipv6Addr_to_string(dstAddr))
    def make_tcam_ms(self, sel):
        val = random.randint(0,1)
        srcAddr = self.make_ipv6_addr()
        dstAddr = self.make_ipv6_addr()
        prefix_len = 128
        if sel:
            return iterator_tcam_sel_match_spec_t(hex_to_byte(val), ipv6Addr_to_string(srcAddr), ipv6Addr_to_string(dstAddr), prefix_len)
        else:
            return iterator_tcam_ap_match_spec_t(hex_to_byte(val), ipv6Addr_to_string(srcAddr), ipv6Addr_to_string(dstAddr), prefix_len)


class TestPlcmtGetAll(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])

    def make_ipv6_addr(self):
        ipv6_addr = "%04x" % (random.randint(0, 0xFFFF))
        for _ in range(7):
            ipv6_addr += ":%04x" % (random.randint(0, 0xFFFF))
        return ipv6_addr

    def checkP0(self):
        shdl = self.conn_mgr.client_init()
        vdt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        self.client.p0_register_mat_update_cb(shdl, vir_dev_id)
        programmed = False

        try:
            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # Select a set of keys to use in the test.  Some entries will be added,
            # others added and modified, others added and deleted, others added,
            # modified, and deleted.
            all_keys = list(range(30))
            to_mod = random.sample(all_keys, 15)
            to_del = random.sample(to_mod, 5)
            to_del += random.sample(sorted(set(all_keys) - set(to_mod)), 5)

            # Issue the adds, mods, and deletes to the virtual device.
            key_to_ms = dict()
            key_to_as = dict()
            hdl_to_key = dict()
            for i in all_keys:
                key_to_ms[i] = iterator_p0_match_spec_t(hex_to_i16(i))
                key_to_as[i] = iterator_N_action_spec_t(random.randint(0,1000))
                h = self.client.p0_table_add_with_N(shdl, vdt, key_to_ms[i], key_to_as[i])
                hdl_to_key[h] = i
            for i in to_mod:
                key_to_as[i] = iterator_N_action_spec_t(random.randint(0,1000))
                self.client.p0_table_modify_with_N_by_match_spec(shdl, vdt, key_to_ms[i], key_to_as[i])
            for i in to_del:
                self.client.p0_table_delete_by_match_spec(shdl, vdt, key_to_ms[i])
            # Issue set the default entry, now it is implemented with a modify.
            def_hdl = self.client.p0_table_get_default_entry_handle(shdl, vdt)
            self.assertEqual(def_hdl, 73)
            hdl_to_key[def_hdl] = len(all_keys)
            def_x = random.randint(0,1000)
            a = iterator_N_action_spec_t(def_x)
            self.client.p0_table_modify_with_N(shdl, vdt.dev_id, def_hdl, a)
            key_to_as[len(all_keys)] = a

            # Keep track of the data in the test.
            key_to_update = dict()
            X = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(X), len(all_keys) + len(to_mod) + len(to_del) + 1)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                # Add and Mod are interchangable for Phase0 tables so handle them the same way
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    i = hdl_to_key[x.update_data.mat.update_params.add.entry_hdl]
                    key_to_update[i] = x.update_data.mat.update_params.add
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_MOD:
                    i = hdl_to_key[x.update_data.mat.update_params.mod.entry_hdl]
                    if i in key_to_update:
                        key_to_update[i].drv_data = x.update_data.mat.update_params.mod.drv_data
                    else:
                        key_to_update[i] = x.update_data.mat.update_params.mod
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    self.assertEqual(def_hdl, x.update_data.mat.update_params.set_dflt.entry_hdl)
                    i = hdl_to_key[x.update_data.mat.update_params.set_dflt.entry_hdl]
                    key_to_update[i] = x.update_data.mat.update_params.set_dflt
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_DEL:
                    pass
                else:
                    self.assertTrue(False)

            # Trigger a replay of the entries in the table and make sure the correct data comes back.
            self.client.p0_get_plcmt_data(shdl, vir_dev_id)
            X = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(X), len(all_keys) + 1 - len(to_del))
            seen = set()
            default_seen = False
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    self.assertFalse(default_seen)
                    self.assertEqual(def_hdl, x.update_data.mat.update_params.set_dflt.entry_hdl)
                    i = hdl_to_key[x.update_data.mat.update_params.set_dflt.entry_hdl]
                    self.assertEqual(key_to_update[i], x.update_data.mat.update_params.set_dflt)
                    default_seen = True
                else:
                    self.assertEqual(x.update_data.mat.update_type, iterator_mat_update_type.MAT_UPDATE_ADD)
                    i = hdl_to_key[x.update_data.mat.update_params.add.entry_hdl]
                    seen.add(i)
                    self.assertEqual(key_to_update[i], x.update_data.mat.update_params.add)

            self.assertEqual(set(all_keys) - set(to_del), seen)
            self.assertTrue(default_seen)

            # Send the entries to the physical device.
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, X)
            programmed = True
            self.conn_mgr.complete_operations(shdl)
            # Read them back and make sure they are correct.
            for x in X:
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    hdl = x.update_data.mat.update_params.set_dflt.entry_hdl
                else:
                    hdl = x.update_data.mat.update_params.add.entry_hdl
                i = hdl_to_key[hdl]
                # Read from HW not supported on TF3
                if test_param_get("arch") != "tofino3":
                    e = self.client.p0_get_entry(shdl, phy_dev_id, hdl, True)
                    if x.update_data.mat.update_type != iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                        self.assertEqual(i, e.match_spec.ig_intr_md_ingress_port)
                    self.assertEqual(key_to_as[i], e.action_desc.data.iterator_N)

        finally:
            # Clean up the entries
            self.client.get_tbl_updates(vir_dev_id)
            while self.client.p0_get_entry_count(shdl, vdt):
                h = self.client.p0_get_first_entry_handle(shdl, vdt)
                self.client.p0_table_delete(shdl, vir_dev_id, h)
            self.client.p0_table_reset_default_entry(shdl, vdt)
            X = self.client.get_tbl_updates(vir_dev_id)
            if programmed:
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, X)
            self.conn_mgr.client_cleanup(shdl)

    def checkTern(self):
        shdl = self.conn_mgr.client_init()
        vdt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        self.client.tcam_register_mat_update_cb(shdl, vir_dev_id)
        hdl_to_entry = dict()
        sym = True

        try:
            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # Fill the table enough such that more than one stage is used.
            logger.info("Adding entries")
            for i in range(500):
                mspec = iterator_tcam_match_spec_t(random.randint(0, 511), hex_to_i16(0x1ff))
                if i % 3 == 0:
                    aspec = iterator_n1_action_spec_t(action_x=hex_to_i16(random.randint(0, 65535)))
                    h = self.client.tcam_table_add_with_n1(shdl, vdt, mspec, i, aspec)
                    hdl_to_entry[h] = (mspec, 'n1', aspec)
                elif i % 3 == 1:
                    aspec = iterator_n2_action_spec_t(action_x=hex_to_i16(random.randint(0, 65535)))
                    h = self.client.tcam_table_add_with_n2(shdl, vdt, mspec, i, aspec)
                    hdl_to_entry[h] = (mspec, 'n2', aspec)
                else:
                    h = self.client.tcam_table_add_with_n(shdl, vdt, mspec, i)
                    hdl_to_entry[h] = (mspec, 'n', None)

            # Get the placement updates and keep track of the final entry location and data
            # in the form of an add operation.
            logger.info("Getting updates")
            hdl_to_update = dict()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    h = x.update_data.mat.update_params.add.entry_hdl
                    hdl_to_update[h] = x.update_data.mat.update_params.add
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_MOV:
                    h = x.update_data.mat.update_params.mov.entry_hdl
                    hdl_to_update[h].entry_index = x.update_data.mat.update_params.mov.entry_index
                    hdl_to_update[h].drv_data = x.update_data.mat.update_params.mov.drv_data
                else:
                    self.assertTrue(False)

            # Trigger a replay of the entries in the table and make sure the correct data comes back.
            logger.info("Triggering replay")
            self.client.tcam_get_plcmt_data(shdl, vir_dev_id)
            logger.info("Getting updates")
            X = self.client.get_tbl_updates(vir_dev_id)
            logger.info("Checking replayed entries")
            self.assertEqual(len(X), len(hdl_to_update) + 1) # Plus one for the dflt entry.
            seen = set()
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    h = x.update_data.mat.update_params.add.entry_hdl
                    self.assertEqual(hdl_to_update[h].entry_index, x.update_data.mat.update_params.add.entry_index)
                    e1 = self.client.tcam_get_entry_from_plcmt_data(hdl_to_update[h].drv_data)
                    e2 = self.client.tcam_get_entry_from_plcmt_data(x.update_data.mat.update_params.add.drv_data)
                    self.assertEqual(e1, e2)
                    mspec, a_name, aspec = hdl_to_entry[h]
                    self.assertEqual(e2.match_spec, mspec)
                    self.assertEqual(e2.action_desc.name, a_name)
                    if a_name == 'n1':
                        self.assertEqual(e2.action_desc.data.iterator_n1, aspec)
                    elif a_name == 'n2':
                        self.assertEqual(e2.action_desc.data.iterator_n2, aspec)
                    elif a_name == 'n':
                        # No action spec returned so nothing to check
                        pass
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    e = self.client.tcam_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n1')
                    self.assertEqual(e.action_desc.data.iterator_n1, iterator_n1_action_spec_t(action_x=hex_to_i16(0x1234)))
                else:
                    self.assertTrue(False)
                seen.add(h)
            self.assertEqual(len(seen), len(hdl_to_entry) + 1)

            # Empty the table
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.tcam_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            hdl_to_entry = dict()
            hdl_to_update = dict()

            # Switch to asymmetric mode.
            logger.info("Asymmetric mode")
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
            self.client.tcam_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            sym = False
            self.client.get_tbl_updates(vir_dev_id)

            # Get the default entry handles
            if test_param_get("arch") == "tofino" or test_param_get("arch") == "tofino2" or test_param_get("arch") == "tofino3":
            # Virtual device always have 4 pipes regardless of the actual hardware platform.
                num_pipes = 4
            else:
                num_pipes = int(test_param_get('num_pipes'))
            pipes = range(num_pipes)
            vdts = [DevTarget_t(vir_dev_id, p) for p in pipes]
            dflt_hdls = [self.client.tcam_table_get_default_entry_handle(shdl, d) for d in vdts]

            # Get the placement data again, there should only be the default entries now.
            self.client.tcam_get_plcmt_data(shdl, vir_dev_id)
            seen = set()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertIn(x.update_data.mat.dev_tgt, vdts)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    self.assertIn(h, dflt_hdls)
                    e = self.client.tcam_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n1')
                    self.assertEqual(e.action_desc.data.iterator_n1, iterator_n1_action_spec_t(action_x=hex_to_i16(0x1234)))
                    seen.add(h)
            self.assertEqual(len(seen), len(dflt_hdls))

        finally:
            if sym is False:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                self.client.tcam_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            # Clean up the entries
            self.client.get_tbl_updates(vir_dev_id)
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.tcam_table_delete(shdl, vir_dev_id, h)
            while self.client.tcam_get_entry_count(shdl, vdt):
                h = self.client.tcam_get_first_entry_handle(shdl, vdt)
                self.client.tcam_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            self.conn_mgr.client_cleanup(shdl)

    def checkATCAM(self):
        shdl = self.conn_mgr.client_init()
        vdt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        self.client.atcam_register_mat_update_cb(shdl, vir_dev_id)
        hdl_to_entry = dict()
        sym = True

        try:
            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # Fill the table enough such that more than one stage is used.
            logger.info("Adding entries")
            for i in range(5000):
                mspec = iterator_atcam_match_spec_t(i, random.randint(0, 511), hex_to_i16(0x1ff))
                priority = i % 4
                if i % 3 == 0:
                    aspec = iterator_n1_action_spec_t(action_x=hex_to_i16(random.randint(0, 65535)))
                    h = self.client.atcam_table_add_with_n1(shdl, vdt, mspec, priority, aspec)
                    hdl_to_entry[h] = (mspec, 'n1', aspec)
                elif i % 3 == 1:
                    aspec = iterator_n2_action_spec_t(action_x=hex_to_i16(random.randint(0, 65535)))
                    h = self.client.atcam_table_add_with_n2(shdl, vdt, mspec, priority, aspec)
                    hdl_to_entry[h] = (mspec, 'n2', aspec)
                else:
                    h = self.client.atcam_table_add_with_n(shdl, vdt, mspec, priority)
                    hdl_to_entry[h] = (mspec, 'n', None)

            # Get the placement updates and keep track of the final entry location and data
            # in the form of an add operation.
            logger.info("Getting updates")
            hdl_to_update = dict()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    h = x.update_data.mat.update_params.add.entry_hdl
                    hdl_to_update[h] = x.update_data.mat.update_params.add
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_MOV:
                    h = x.update_data.mat.update_params.mov.entry_hdl
                    hdl_to_update[h].entry_index = x.update_data.mat.update_params.mov.entry_index
                    hdl_to_update[h].drv_data = x.update_data.mat.update_params.mov.drv_data
                else:
                    self.assertTrue(False)

            # Trigger a replay of the entries in the table and make sure the correct data comes back.
            logger.info("Triggering replay")
            self.client.atcam_get_plcmt_data(shdl, vir_dev_id)
            logger.info("Getting updates")
            X = self.client.get_tbl_updates(vir_dev_id)
            logger.info("Checking replayed entries")
            self.assertEqual(len(X), len(hdl_to_update) + 1) # Plus one for the dflt entry.
            seen = set()
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    h = x.update_data.mat.update_params.add.entry_hdl
                    self.assertEqual(hdl_to_update[h].entry_index, x.update_data.mat.update_params.add.entry_index)
                    e1 = self.client.atcam_get_entry_from_plcmt_data(hdl_to_update[h].drv_data)
                    e2 = self.client.atcam_get_entry_from_plcmt_data(x.update_data.mat.update_params.add.drv_data)
                    self.assertEqual(e1, e2)
                    mspec, a_name, aspec = hdl_to_entry[h]
                    self.assertEqual(e2.match_spec, mspec)
                    self.assertEqual(e2.action_desc.name, a_name)
                    if a_name == 'n1':
                        self.assertEqual(e2.action_desc.data.iterator_n1, aspec)
                    elif a_name == 'n2':
                        self.assertEqual(e2.action_desc.data.iterator_n2, aspec)
                    elif a_name == 'n':
                        # No action spec returned so nothing to check
                        pass
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    e = self.client.atcam_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n1')
                    self.assertEqual(e.action_desc.data.iterator_n1, iterator_n1_action_spec_t(action_x=hex_to_i16(0xABCD)))
                else:
                    self.assertTrue(False)
                seen.add(h)
            self.assertEqual(len(seen), len(hdl_to_entry) + 1)

            # Empty the table
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.atcam_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            hdl_to_entry = dict()
            hdl_to_update = dict()

            # Switch to asymmetric mode.
            logger.info("Asymmetric mode")
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
            self.client.atcam_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            sym = False
            self.client.get_tbl_updates(vir_dev_id)

            # Get the default entry handles
            if test_param_get("arch") == "tofino" or test_param_get("arch") == "tofino2" or test_param_get("arch") == "tofino3":
                # Virtual device always have 4 pipes regardless of the actual hardware platform.
                num_pipes = 4
            else:
                num_pipes = int(test_param_get('num_pipes'))

            pipes = range(num_pipes)
            vdts = [DevTarget_t(vir_dev_id, p) for p in pipes]
            dflt_hdls = [self.client.atcam_table_get_default_entry_handle(shdl, d) for d in vdts]

            # Get the placement data again, there should only be the default entries now.
            self.client.atcam_get_plcmt_data(shdl, vir_dev_id)
            seen = set()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertIn(x.update_data.mat.dev_tgt, vdts)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    self.assertIn(h, dflt_hdls)
                    e = self.client.atcam_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n1')
                    self.assertEqual(e.action_desc.data.iterator_n1, iterator_n1_action_spec_t(action_x=hex_to_i16(0xABCD)))
                    seen.add(h)
            self.assertEqual(len(seen), len(dflt_hdls))

        finally:
            if sym is False:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                self.client.atcam_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            # Clean up the entries
            self.client.get_tbl_updates(vir_dev_id)
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.atcam_table_delete(shdl, vir_dev_id, h)
            while self.client.atcam_get_entry_count(shdl, vdt):
                h = self.client.atcam_get_first_entry_handle(shdl, vdt)
                self.client.atcam_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            self.conn_mgr.client_cleanup(shdl)

    def checkEXM(self):
        shdl = self.conn_mgr.client_init()
        vdt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        self.client.exm_register_mat_update_cb(shdl, vir_dev_id)
        hdl_to_entry = dict()
        sym = True

        try:
            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # Fill the table enough such that more than one stage is used.
            logger.info("Adding entries")
            for i in range(5000):
                mspec = iterator_exm_match_spec_t(random.randint(0, 511), hex_to_i16(i))
                if i % 3 == 0:
                    aspec = iterator_n1_action_spec_t(action_x=hex_to_i16(random.randint(0, 65535)))
                    h = self.client.exm_table_add_with_n1(shdl, vdt, mspec, aspec)
                    hdl_to_entry[h] = (mspec, 'n1', aspec)
                elif i % 3 == 1:
                    aspec = iterator_n2_action_spec_t(action_x=hex_to_i16(random.randint(0, 65535)))
                    h = self.client.exm_table_add_with_n2(shdl, vdt, mspec, aspec)
                    hdl_to_entry[h] = (mspec, 'n2', aspec)
                else:
                    h = self.client.exm_table_add_with_n(shdl, vdt, mspec)
                    hdl_to_entry[h] = (mspec, 'n', None)

            # Get the placement updates and keep track of the final entry location and data
            # in the form of an add operation.
            logger.info("Getting updates")
            hdl_to_update = dict()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    h = x.update_data.mat.update_params.add.entry_hdl
                    hdl_to_update[h] = x.update_data.mat.update_params.add
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_MOV:
                    h = x.update_data.mat.update_params.mov.entry_hdl
                    hdl_to_update[h].entry_index = x.update_data.mat.update_params.mov.entry_index
                    hdl_to_update[h].drv_data = x.update_data.mat.update_params.mov.drv_data
                else:
                    self.assertTrue(False)

            # Trigger a replay of the entries in the table and make sure the correct data comes back.
            logger.info("Triggering replay")
            self.client.exm_get_plcmt_data(shdl, vir_dev_id)
            logger.info("Getting updates")
            X = self.client.get_tbl_updates(vir_dev_id)
            logger.info("Checking replayed entries")
            self.assertEqual(len(X), len(hdl_to_update) + 1) # Plus one for the dflt entry.
            seen = set()
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    h = x.update_data.mat.update_params.add.entry_hdl
                    self.assertEqual(hdl_to_update[h].entry_index, x.update_data.mat.update_params.add.entry_index)
                    e1 = self.client.exm_get_entry_from_plcmt_data(hdl_to_update[h].drv_data)
                    e2 = self.client.exm_get_entry_from_plcmt_data(x.update_data.mat.update_params.add.drv_data)
                    self.assertEqual(e1, e2)
                    mspec, a_name, aspec = hdl_to_entry[h]
                    self.assertEqual(e2.match_spec, mspec)
                    self.assertEqual(e2.action_desc.name, a_name)
                    if a_name == 'n1':
                        self.assertEqual(e2.action_desc.data.iterator_n1, aspec)
                    elif a_name == 'n2':
                        self.assertEqual(e2.action_desc.data.iterator_n2, aspec)
                    elif a_name == 'n':
                        # No action spec returned so nothing to check
                        pass
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    e = self.client.exm_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n1')
                    self.assertEqual(e.action_desc.data.iterator_n1, iterator_n1_action_spec_t(action_x=hex_to_i16(0x1234)))
                else:
                    self.assertTrue(False)
                seen.add(h)
            self.assertEqual(len(seen), len(hdl_to_entry) + 1)

            # Empty the table
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.exm_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            hdl_to_entry = dict()
            hdl_to_update = dict()

            # Switch to asymmetric mode.
            logger.info("Asymmetric mode")
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
            self.client.exm_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            sym = False
            self.client.get_tbl_updates(vir_dev_id)

            # Get the default entry handles
            if test_param_get("arch") == "tofino" or test_param_get("arch") == "tofino2" or test_param_get("arch") == "tofino3":
                # Virtual device always have 4 pipes regardless of the actual hardware platform.
                num_pipes = 4
            else:
                num_pipes = int(test_param_get('num_pipes'))
            pipes = range(num_pipes)
            vdts = [DevTarget_t(vir_dev_id, p) for p in pipes]
            dflt_hdls = [self.client.exm_table_get_default_entry_handle(shdl, d) for d in vdts]

            # Get the placement data again, there should only be the default entries now.
            self.client.exm_get_plcmt_data(shdl, vir_dev_id)
            seen = set()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertIn(x.update_data.mat.dev_tgt, vdts)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    self.assertIn(h, dflt_hdls)
                    e = self.client.exm_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n1')
                    self.assertEqual(e.action_desc.data.iterator_n1, iterator_n1_action_spec_t(action_x=hex_to_i16(0x1234)))
                    seen.add(h)
            self.assertEqual(len(seen), len(dflt_hdls))


        finally:
            if sym is False:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                self.client.exm_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            # Clean up the entries
            self.client.get_tbl_updates(vir_dev_id)
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.exm_table_delete(shdl, vir_dev_id, h)
            while self.client.exm_get_entry_count(shdl, vdt):
                h = self.client.exm_get_first_entry_handle(shdl, vdt)
                self.client.exm_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            self.conn_mgr.client_cleanup(shdl)

    def checkHA(self):
        shdl = self.conn_mgr.client_init()
        vdt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        self.client.ha_register_mat_update_cb(shdl, vir_dev_id)
        hdl_to_entry = dict()
        sym = True

        try:
            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # Fill the table enough such that more than one stage is used.
            for i in range(512):
                mspec = iterator_ha_match_spec_t(hex_to_i16(i))
                aspec = None
                h = self.client.ha_table_add_with_n(shdl, vdt, mspec)
                hdl_to_entry[h] = (mspec, 'n', aspec)

            # Get the placement updates and keep track of the entry location and data
            # in the form of an add operation.
            logger.info("Getting updates")
            hdl_to_update = dict()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    h = x.update_data.mat.update_params.add.entry_hdl
                    hdl_to_update[h] = x.update_data.mat.update_params.add
                else:
                    self.assertTrue(False)

            # Trigger a replay of the entries in the table and make sure the correct data comes back.
            logger.info("Triggering replay")
            self.client.ha_get_plcmt_data(shdl, vir_dev_id)
            logger.info("Getting updates")
            X = self.client.get_tbl_updates(vir_dev_id)
            logger.info("Checking replayed entries")
            self.assertEqual(len(X), len(hdl_to_update) + 1) # Plus one for the dflt entry.
            seen = set()
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                    h = x.update_data.mat.update_params.add.entry_hdl
                    self.assertEqual(hdl_to_update[h].entry_index, x.update_data.mat.update_params.add.entry_index)
                    e1 = self.client.ha_get_entry_from_plcmt_data(hdl_to_update[h].drv_data)
                    e2 = self.client.ha_get_entry_from_plcmt_data(x.update_data.mat.update_params.add.drv_data)
                    self.assertEqual(e1, e2)
                    mspec, a_name, aspec = hdl_to_entry[h]
                    self.assertEqual(e2.match_spec, mspec)
                    self.assertEqual(e2.action_desc.name, a_name)
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    e = self.client.ha_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n')
                else:
                    self.assertTrue(False)
                seen.add(h)
            self.assertEqual(len(seen), len(hdl_to_entry) + 1)

            # Empty the table
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.ha_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            hdl_to_entry = dict()
            hdl_to_update = dict()

            # Switch to asymmetric mode.
            logger.info("Asymmetric mode")
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
            self.client.ha_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            sym = False
            self.client.get_tbl_updates(vir_dev_id)

            # Get the default entry handles
            if test_param_get("arch") == "tofino" or test_param_get("arch") == "tofino2" or test_param_get("arch") == "tofino3":
                # Virtual device always have 4 pipes regardless of the actual hardware platform.
                num_pipes = 4
            else:
                num_pipes = int(test_param_get('num_pipes'))
            pipes = range(num_pipes)
            vdts = [DevTarget_t(vir_dev_id, p) for p in pipes]
            dflt_hdls = [self.client.ha_table_get_default_entry_handle(shdl, d) for d in vdts]

            # Get the placement data again, there should only be the default entries now.
            self.client.ha_get_plcmt_data(shdl, vir_dev_id)
            seen = set()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertIn(x.update_data.mat.dev_tgt, vdts)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    self.assertIn(h, dflt_hdls)
                    e = self.client.ha_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n')
                    seen.add(h)
            self.assertEqual(len(seen), len(dflt_hdls))


        finally:
            if sym is False:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                self.client.ha_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            # Clean up the entries
            self.client.get_tbl_updates(vir_dev_id)
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.ha_table_delete(shdl, vir_dev_id, h)
            while self.client.ha_get_entry_count(shdl, vdt):
                h = self.client.ha_get_first_entry_handle(shdl, vdt)
                self.client.ha_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            self.conn_mgr.client_cleanup(shdl)

    def checkADT(self):
        shdl = self.conn_mgr.client_init()
        vdt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        self.client.ap_register_adt_update_cb(shdl, vir_dev_id)
        programmed = False

        try:
            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # Add a few entries to the virtual device.
            hdl_to_act = {}
            for _ in range(10):
                aspec = iterator_a_action_spec_t(action_x=random.randint(0,15))
                h = self.client.ap_add_member_with_a(shdl, vdt, aspec)
                hdl_to_act[h] = ('a', aspec)
            for _ in range(10):
                x = random.randint(100,150)
                i = random.randint(200,250)
                aspec = iterator_b_action_spec_t(action_x=x, action_i=i)
                h = self.client.ap_add_member_with_b(shdl, vdt, aspec)
                hdl_to_act[h] = ('b', aspec)
            for _ in range(10):
                aspec = iterator_c_action_spec_t(action_x=random.randint(30,50))
                h = self.client.ap_add_member_with_c(shdl, vdt, aspec)
                hdl_to_act[h] = ('c', aspec)
            for _ in range(10):
                ipv6_addr = self.make_ipv6_addr()
                flow_lbl = random.randint(500, 600)
                i = random.randint(800,850)
                aspec = iterator_d_action_spec_t(ipv6Addr_to_string(ipv6_addr), flow_lbl, i)
                h = self.client.ap_add_member_with_d(shdl, vdt, aspec)
                hdl_to_act[h] = ('d', aspec)
            for _ in range(10):
                h = self.client.ap_add_member_with_e(shdl, vdt)
                hdl_to_act[h] = ('e', None)

            # Modify a few of the entries
            to_mod = random.sample(list(hdl_to_act), len(hdl_to_act)//3)
            for h in to_mod:
                act,old_aspec = hdl_to_act[h]
                if act == 'a':
                    aspec = iterator_a_action_spec_t(action_x=random.randint(0,15))
                    self.client.ap_modify_member_with_a(shdl, vir_dev_id, h, aspec)
                    hdl_to_act[h] = ('a', aspec)
                if act == 'b':
                    x = random.randint(100,150)
                    i = random.randint(200,250)
                    i = old_aspec.action_i
                    aspec = iterator_b_action_spec_t(action_x=x, action_i=i)
                    self.client.ap_modify_member_with_b(shdl, vir_dev_id, h, aspec)
                    hdl_to_act[h] = ('b', aspec)
                if act == 'c':
                    aspec = iterator_c_action_spec_t(action_x=random.randint(30,50))
                    self.client.ap_modify_member_with_c(shdl, vir_dev_id, h, aspec)
                    hdl_to_act[h] = ('c', aspec)
                if act == 'd':
                    ipv6_addr = self.make_ipv6_addr()
                    flow_lbl = random.randint(500, 600)
                    i = random.randint(800,850)
                    i = old_aspec.action_i
                    aspec = iterator_d_action_spec_t(ipv6Addr_to_string(ipv6_addr), flow_lbl, i)
                    self.client.ap_modify_member_with_d(shdl, vir_dev_id, h, aspec)
                    hdl_to_act[h] = ('d', aspec)
                if act == 'e':
                    self.client.ap_modify_member_with_e(shdl, vir_dev_id, h)
                    hdl_to_act[h] = ('e', None)

            # Delete a few entries
            to_del = random.sample(list(hdl_to_act.keys()), len(hdl_to_act)//5)
            for h in to_del:
                self.client.ap_del_member(shdl, vir_dev_id, h)
                del hdl_to_act[h]

            # Keep track of the data in the test.
            hdl_to_data = dict()
            X = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(X), len(hdl_to_act) + len(to_mod) + 2*len(to_del))
            for x in X:
                if x.update_data.adt.update_type == iterator_adt_update_type.ADT_UPDATE_ADD:
                    h = x.update_data.adt.update_params.add.entry_hdl
                    d = x.update_data.adt.update_params.add.drv_data
                    hdl_to_data[h] = d
                elif x.update_data.adt.update_type == iterator_adt_update_type.ADT_UPDATE_MOD:
                    h = x.update_data.adt.update_params.mod.entry_hdl
                    d = x.update_data.adt.update_params.mod.drv_data
                    hdl_to_data[h] = d
                elif x.update_data.adt.update_type == iterator_adt_update_type.ADT_UPDATE_DEL:
                    h = x.update_data.adt.update_params.remove.entry_hdl
                    del hdl_to_data[h]
                else:
                    self.assertTrue(False)

            # Trigger a replay of the entries in the table and make sure the correct data comes back.
            self.client.ap_get_plcmt_data(shdl, vir_dev_id)
            X = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(X), len(hdl_to_data))

            # Get each entry from the plcmt data and make sure it is as expected.
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.ADT_UPDATE_TYPE)
                self.assertEqual(x.update_data.adt.update_type, iterator_adt_update_type.ADT_UPDATE_ADD)
                h = x.update_data.adt.update_params.add.entry_hdl
                d = x.update_data.adt.update_params.add.drv_data
                # The entry from the original callback
                e1 = self.client.ap_get_member_from_plcmt_data(hdl_to_data[h])
                # The entry from the "replay" callback
                e2 = self.client.ap_get_member_from_plcmt_data(d)
                self.assertEqual(e1, e2)

                e1 = self.client.ap_get_full_member_info_from_plcmt_data(vir_dev_id, h, hdl_to_data[h])
                e2 = self.client.ap_get_full_member_info_from_plcmt_data(vir_dev_id, h, d)
                self.assertEqual(e1, e2)
                exp_name, exp_aspec = hdl_to_act[h]
                self.assertEqual(e1.name, exp_name)
                if e1.name == 'a': self.assertEqual(exp_aspec, e1.data.iterator_a)
                if e1.name == 'b': self.assertEqual(exp_aspec, e1.data.iterator_b)
                if e1.name == 'c': self.assertEqual(exp_aspec, e1.data.iterator_c)
                if e1.name == 'd': self.assertEqual(exp_aspec, e1.data.iterator_d)
                if e1.name == 'e': self.assertEqual(exp_aspec, None)

            # Send the entries to the physical device.
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, X)
            programmed = True
            self.conn_mgr.complete_operations(shdl)

        finally:
            self.client.get_tbl_updates(vir_dev_id)
            while self.client.ap_get_act_prof_entry_count(shdl, vdt):
                h = self.client.ap_get_first_member(shdl, vdt)
                self.client.ap_del_member(shdl, vir_dev_id, h)
            X = self.client.get_tbl_updates(vir_dev_id)
            if programmed:
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, X)
            self.conn_mgr.client_cleanup(shdl)

    def checkSel(self):
        for tf in [True, False, True]:
            self.checkSelGroups(tf)

    def checkSelGroups(self, sym):
        shdl = self.conn_mgr.client_init()
        if sym:
            pipes = [0xFFFF]
        else:
            pipes = [p for p in range(int(test_param_get('num_pipes')))]
        vdt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        self.client.ap_register_adt_update_cb(shdl, vir_dev_id)
        self.client.sel_ap_register_sel_update_cb(shdl, vir_dev_id)
        self.client.exm_sel_register_mat_update_cb(shdl, vir_dev_id)
        grp_to_sz = dict()
        grp_mbr_to_loc = dict()

        try:
            # Set the symmetric mode on match tables so it propagates to the
            # action profile and action selector.
            if sym:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                self.client.exm_sel_set_property(shdl, vir_dev_id, prop, prop_val, 0)
                self.client.tcam_sel_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            else:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
                self.client.exm_sel_set_property(shdl, vir_dev_id, prop, prop_val, 0)
                self.client.tcam_sel_set_property(shdl, vir_dev_id, prop, prop_val, 0)

            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # There should be no data now since nothing has been created.
            self.client.sel_ap_sel_get_plcmt_data(shdl, vir_dev_id)
            X = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(X), 0)

            # Create a few groups of different sizes.
            grp_to_sz = dict()
            grp_to_dt = dict()
            for _ in range(30):
                max_sz = random.randint(2, 120)
                vdt = DevTarget_t(vir_dev_id, hex_to_i16(random.choice(pipes)))
                h = self.client.sel_ap_create_group(shdl, vdt, max_sz)
                grp_to_sz[h] = max_sz
                grp_to_dt[h] = vdt

            # Get the updates and hold on to them so we can compare against the
            # replayed group info.
            grp_to_update = dict()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.SEL_UPDATE_TYPE)
                self.assertEqual(x.update_data.sel.update_type, iterator_sel_update_type.SEL_UPDATE_GROUP_CREATE)
                h = x.update_data.sel.update_params.grp_create.group_hdl
                self.assertEqual(x.update_data.sel.dev_tgt, grp_to_dt[h])
                grp_to_update[h] = x.update_data.sel.update_params.grp_create

            # Retrigger the callbacks and make sure the exact same data is replayed.
            self.client.sel_ap_sel_get_plcmt_data(shdl, vir_dev_id)
            X = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(grp_to_update), len(X))
            seen = set()
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.SEL_UPDATE_TYPE)
                self.assertEqual(x.update_data.sel.update_type, iterator_sel_update_type.SEL_UPDATE_GROUP_CREATE)
                h = x.update_data.sel.update_params.grp_create.group_hdl
                seen.add(h)
                self.assertEqual(grp_to_update[h], x.update_data.sel.update_params.grp_create)
                self.assertEqual(grp_to_dt[h], x.update_data.sel.dev_tgt)
            self.assertEqual(len(grp_to_update), len(seen))

            # Add members to the groups.
            mbr_to_aspec = dict()
            grp_to_action = dict()
            grp_to_mbrs = dict()
            for grp in grp_to_sz.keys():
                max_sz = grp_to_sz[grp]
                dt = grp_to_dt[grp]
                mbr_cnt = random.randint(0, max_sz)
                action = random.choice(['a', 'b', 'c'])
                grp_to_action[grp] = action
                mbrs = list()
                i = random.randint(0, 999)
                for _ in range(mbr_cnt):
                    if action == 'a':
                        aspec = iterator_a_action_spec_t(action_x=random.randint(0, 511))
                        h = self.client.sel_ap_add_member_with_a(shdl, dt, aspec)
                        mbr_to_aspec[h] = aspec
                    elif action == 'b':
                        aspec = iterator_b_action_spec_t(action_x=random.randint(0, 511), action_i=i)
                        h = self.client.sel_ap_add_member_with_b(shdl, dt, aspec)
                        mbr_to_aspec[h] = aspec
                    elif action == 'c':
                        aspec = iterator_c_action_spec_t(action_x=random.randint(0, 511))
                        h = self.client.sel_ap_add_member_with_c(shdl, dt, aspec)
                        mbr_to_aspec[h] = aspec
                    mbrs.append(h)
                    self.client.sel_ap_add_member_to_group(shdl, vir_dev_id, grp, h)
                grp_to_mbrs[grp] = list(mbrs)

            # Get the updates and track where each member located in each group.
            grp_mbr_to_loc = dict()
            for g in grp_to_mbrs:
                for m in grp_to_mbrs[g]:
                    grp_mbr_to_loc[(g,m)] = set()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                # Ignore updates on the Action Profile, just check the selector table here.
                if x.update_type != iterator_tbl_update_type.SEL_UPDATE_TYPE:
                    continue
                if x.update_data.sel.update_type == iterator_sel_update_type.SEL_UPDATE_ADD:
                    g = x.update_data.sel.update_params.add.group_hdl
                    m = x.update_data.sel.update_params.add.entry_hdl
                    self.assertEqual(grp_to_dt[g], x.update_data.sel.dev_tgt)
                    idx = x.update_data.sel.update_params.add.entry_index
                    sub_idx = x.update_data.sel.update_params.add.entry_subindex
                    data = x.update_data.sel.update_params.add.drv_data
                    grp_mbr_to_loc[(g,m)].add((idx, sub_idx, data))
                elif x.update_data.sel.update_type == iterator_sel_update_type.SEL_UPDATE_DEL:
                    g = x.update_data.sel.update_params.remove.group_hdl
                    m = x.update_data.sel.update_params.remove.entry_hdl
                    idx = x.update_data.sel.update_params.remove.entry_index
                    sub_idx = x.update_data.sel.update_params.remove.entry_subindex
                    found = False
                    for i,e in enumerate(grp_mbr_to_loc[(g,m)]):
                        e_idx,e_sidx,e_data = e
                        if idx == e_idx and sub_idx == e_sidx:
                            found = True
                            break
                    self.assertTrue(found)
                    del grp_mbr_to_loc[(g,m)][i]
                else:
                    self.assertTrue(False)

            # Request another replay and make sure the members are replayed at the correct locations.
            self.client.sel_ap_sel_get_plcmt_data(shdl, vir_dev_id)
            X = self.client.get_tbl_updates(vir_dev_id)
            groups_seen = set()
            mbrs_seen = dict()
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.SEL_UPDATE_TYPE)
                if x.update_data.sel.update_type == iterator_sel_update_type.SEL_UPDATE_GROUP_CREATE:
                    h = x.update_data.sel.update_params.grp_create.group_hdl
                    self.assertEqual(x.update_data.sel.dev_tgt, grp_to_dt[h])
                    self.assertEqual(x.update_data.sel.update_params.grp_create, grp_to_update[h])
                    groups_seen.add(h)
                elif x.update_data.sel.update_type == iterator_sel_update_type.SEL_UPDATE_ADD:
                    g = x.update_data.sel.update_params.add.group_hdl
                    m = x.update_data.sel.update_params.add.entry_hdl
                    self.assertEqual(grp_to_dt[g], x.update_data.sel.dev_tgt)
                    idx = x.update_data.sel.update_params.add.entry_index
                    sub_idx = x.update_data.sel.update_params.add.entry_subindex
                    data = x.update_data.sel.update_params.add.drv_data
                    self.assertIn((idx, sub_idx, data), grp_mbr_to_loc[(g,m)])
                    if (g,m) not in mbrs_seen:
                        mbrs_seen[(g,m)] = set()
                    mbrs_seen[(g,m)].add((idx, sub_idx, data))
                else:
                    self.assertTrue(False)
            self.assertEqual(set(mbrs_seen.keys()), set(grp_mbr_to_loc.keys()))
            for g,h in grp_mbr_to_loc:
                expected = grp_mbr_to_loc[(g,h)]
                gotten = mbrs_seen[(g,h)]
                self.assertEqual(expected, gotten)

            # Disable a few members and check again.  Disable all members in one group plus a few other
            # members in other groups.
            all_grps = list(grp_to_mbrs.keys())
            disabled = set()
            for mbr in grp_to_mbrs[all_grps[0]]:
                self.client.sel_ap_group_member_state_set(shdl, vir_dev_id, all_grps[0], mbr, iterator_grp_mbr_state.MBR_INACTIVE)
                disabled.add((all_grps[0], mbr))
            for g in random.sample(all_grps[1:], 10):
                for mbr in random.sample(grp_to_mbrs[g], min(5, len(grp_to_mbrs[g]))):
                    self.client.sel_ap_group_member_state_set(shdl, vir_dev_id, g, mbr, iterator_grp_mbr_state.MBR_INACTIVE)
                    disabled.add((g, mbr))
            self.client.get_tbl_updates(vir_dev_id)
            self.client.sel_ap_sel_get_plcmt_data(shdl, vir_dev_id)
            X = self.client.get_tbl_updates(vir_dev_id)
            groups_seen = set()
            mbrs_seen = dict()
            disabled_seen = set()
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.SEL_UPDATE_TYPE)
                if x.update_data.sel.update_type == iterator_sel_update_type.SEL_UPDATE_GROUP_CREATE:
                    h = x.update_data.sel.update_params.grp_create.group_hdl
                    self.assertEqual(x.update_data.sel.dev_tgt, grp_to_dt[h])
                    self.assertEqual(x.update_data.sel.update_params.grp_create, grp_to_update[h])
                    groups_seen.add(h)
                elif x.update_data.sel.update_type == iterator_sel_update_type.SEL_UPDATE_ADD:
                    g = x.update_data.sel.update_params.add.group_hdl
                    m = x.update_data.sel.update_params.add.entry_hdl
                    self.assertEqual(grp_to_dt[g], x.update_data.sel.dev_tgt)
                    idx = x.update_data.sel.update_params.add.entry_index
                    sub_idx = x.update_data.sel.update_params.add.entry_subindex
                    data = x.update_data.sel.update_params.add.drv_data
                    self.assertIn((idx, sub_idx, data), grp_mbr_to_loc[(g,m)])
                    if (g,m) not in mbrs_seen:
                        mbrs_seen[(g,m)] = set()
                    mbrs_seen[(g,m)].add((idx, sub_idx, data))
                elif x.update_data.sel.update_type == iterator_sel_update_type.SEL_UPDATE_DEACTIVATE:
                    g = x.update_data.sel.update_params.deactivate.group_hdl
                    m = x.update_data.sel.update_params.deactivate.entry_hdl
                    self.assertEqual(grp_to_dt[g], x.update_data.sel.dev_tgt)
                    idx = x.update_data.sel.update_params.deactivate.entry_index
                    sub_idx = x.update_data.sel.update_params.deactivate.entry_subindex
                    disabled_seen.add((g,m))
                else:
                    self.assertTrue(False)
            self.assertEqual(set(mbrs_seen.keys()), set(grp_mbr_to_loc.keys()))
            for g,h in grp_mbr_to_loc:
                expected = grp_mbr_to_loc[(g,h)]
                gotten = mbrs_seen[(g,h)]
                self.assertEqual(expected, gotten)
            self.assertEqual(disabled_seen, disabled)

            # Clean up and make sure no replay events are generated.
            to_del = list(grp_to_sz.keys())
            for h in to_del:
                self.client.sel_ap_del_group(shdl, vir_dev_id, h)
                del grp_to_sz[h]
            for _,h in grp_mbr_to_loc:
                self.client.sel_ap_del_member(shdl, vir_dev_id, h)
            grp_mbr_to_loc = dict()

            self.client.get_tbl_updates(vir_dev_id)
            self.client.sel_ap_sel_get_plcmt_data(shdl, vir_dev_id)
            replayed = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(0, len(replayed))
        finally:
            for h in grp_to_sz:
                self.client.sel_ap_del_group(shdl, vir_dev_id, h)
            for _,h in grp_mbr_to_loc:
                self.client.sel_ap_del_member(shdl, vir_dev_id, h)
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
            self.client.exm_sel_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            self.client.tcam_sel_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            self.conn_mgr.client_cleanup(shdl)

    def checkRange(self):
        shdl = self.conn_mgr.client_init()
        vdt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        self.client.range_register_mat_update_cb(shdl, vir_dev_id)
        hdl_to_entry = dict()
        sym = True

        try:
            # Get updates now to clear anything left by previous test cases.
            self.client.get_tbl_updates(vir_dev_id)

            # Fill the table enough such that more than one stage is used.
            logger.info("Adding entries")
            for i in range(500):
                port_start = random.randint(0, 255)
                range_size = random.randint(2, 255)
                port_end = port_start + range_size;
                #logger.info("port_start=%d, range_size=%d, port_end=%d", port_start, range_size, port_end)
                mspec = iterator_range_match_spec_t(port_start, port_end)

                if i % 3 == 0:
                    aspec = iterator_n1_action_spec_t(action_x=hex_to_i16(random.randint(0, 65535)))
                    h = self.client.range_table_add_with_n1(shdl, vdt, mspec, i, aspec)
                    hdl_to_entry[h] = (mspec, 'n1', aspec)
                elif i % 3 == 1:
                    aspec = iterator_n2_action_spec_t(action_x=hex_to_i16(random.randint(0, 65535)))
                    h = self.client.range_table_add_with_n2(shdl, vdt, mspec, i, aspec)
                    hdl_to_entry[h] = (mspec, 'n2', aspec)
                else:
                    h = self.client.range_table_add_with_n(shdl, vdt, mspec, i)
                    hdl_to_entry[h] = (mspec, 'n', None)

            # Get the placement updates and keep track of the final entry location and data
            # in the form of an add operation.
            logger.info("Getting updates")
            hdl_to_update = dict()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD_MULTI:
                    h = x.update_data.mat.update_params.add_multi.entry_hdl
                    hdl_to_update[h] = x.update_data.mat.update_params.add_multi
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_MOV_MULTI:
                    h = x.update_data.mat.update_params.mov_multi.entry_hdl
                    hdl_to_update[h].selection_index = x.update_data.mat.update_params.mov_multi.selection_index
                    hdl_to_update[h].drv_data = x.update_data.mat.update_params.mov_multi.drv_data
                else:
                    self.assertTrue(False)

            # Trigger a replay of the entries in the table and make sure the correct data comes back.
            logger.info("Triggering replay")
            self.client.range_get_plcmt_data(shdl, vir_dev_id)
            logger.info("Getting updates")
            X = self.client.get_tbl_updates(vir_dev_id)
            logger.info("Checking replayed entries")
            self.assertEqual(len(X), len(hdl_to_update) + 1) # Plus one for the dflt entry.
            seen = set()
            #import pdb; pdb.set_trace()
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(x.update_data.mat.dev_tgt, vdt)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD_MULTI:
                    h = x.update_data.mat.update_params.add_multi.entry_hdl
                    self.assertEqual(hdl_to_update[h].selection_index, x.update_data.mat.update_params.add_multi.selection_index)
                    e1 = self.client.range_get_entry_from_plcmt_data(hdl_to_update[h].drv_data)
                    e2 = self.client.range_get_entry_from_plcmt_data(x.update_data.mat.update_params.add_multi.drv_data)
                    self.assertEqual(e1, e2)
                    mspec, a_name, aspec = hdl_to_entry[h]
                    self.assertEqual(e2.match_spec, mspec)
                    self.assertEqual(e2.action_desc.name, a_name)
                    if a_name == 'n1':
                        self.assertEqual(e2.action_desc.data.iterator_n1, aspec)
                    elif a_name == 'n2':
                        self.assertEqual(e2.action_desc.data.iterator_n2, aspec)
                    elif a_name == 'n':
                        # No action spec returned so nothing to check
                        pass
                #elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_ADD:
                #    h = x.update_data.mat.update_params.add.entry_hdl
                #    pass
                elif x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    e = self.client.range_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n1')
                    self.assertEqual(e.action_desc.data.iterator_n1, iterator_n1_action_spec_t(action_x=hex_to_i16(0xFACE)))
                else:
                    self.assertTrue(False)
                seen.add(h)
            self.assertEqual(len(seen), len(hdl_to_entry) + 1)

            # Empty the table
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.range_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            hdl_to_entry = dict()
            hdl_to_update = dict()

            # Switch to asymmetric mode.
            logger.info("Asymmetric mode")
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_SINGLE_PIPELINE
            self.client.range_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            sym = False
            self.client.get_tbl_updates(vir_dev_id)

            # Get the default entry handles
            if test_param_get("arch") == "tofino" or test_param_get("arch") == "tofino2" or test_param_get("arch") == "tofino3":
                # Virtual device always have 4 pipes regardless of the actual hardware platform.
                num_pipes = 4
            else:
                num_pipes = int(test_param_get('num_pipes'))
            pipes = range(num_pipes)
            vdts = [DevTarget_t(vir_dev_id, p) for p in pipes]
            dflt_hdls = [self.client.range_table_get_default_entry_handle(shdl, d) for d in vdts]

            # Get the placement data again, there should only be the default entries now.
            self.client.range_get_plcmt_data(shdl, vir_dev_id)
            seen = set()
            X = self.client.get_tbl_updates(vir_dev_id)
            for x in X:
                self.assertEqual(x.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertIn(x.update_data.mat.dev_tgt, vdts)
                if x.update_data.mat.update_type == iterator_mat_update_type.MAT_UPDATE_SET_DFLT:
                    h = x.update_data.mat.update_params.set_dflt.entry_hdl
                    self.assertIn(h, dflt_hdls)
                    e = self.client.range_get_entry_from_plcmt_data(x.update_data.mat.update_params.set_dflt.drv_data)
                    # Default entry set in P4.
                    self.assertEqual(e.action_desc.name, 'n1')
                    self.assertEqual(e.action_desc.data.iterator_n1, iterator_n1_action_spec_t(action_x=hex_to_i16(0xFACE)))
                    seen.add(h)
            self.assertEqual(len(seen), len(dflt_hdls))

        finally:
            if sym is False:
                prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
                prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
                self.client.range_set_property(shdl, vir_dev_id, prop, prop_val, 0)
            # Clean up the entries
            self.client.get_tbl_updates(vir_dev_id)
            logger.info("Removing entries")
            for h in hdl_to_entry.keys():
                self.client.range_table_delete(shdl, vir_dev_id, h)
            while self.client.range_get_entry_count(shdl, vdt):
                h = self.client.range_get_first_entry_handle(shdl, vdt)
                self.client.range_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)
            self.conn_mgr.client_cleanup(shdl)

    def runTest(self):
        setup_random()
        # Clear any updates left over by earlier tests
        self.client.get_tbl_updates(vir_dev_id)

        logger.info("\nChecking phase0 table")
        self.checkP0()
        logger.info("\nChecking TCAM table")
        self.checkTern()
        logger.info("\nChecking ATCAM table")
        self.checkATCAM()
        logger.info("\nChecking exact table")
        self.checkEXM()
        logger.info("\nChecking hash action table")
        self.checkHA()
        logger.info("\nChecking action data table")
        self.checkADT()
        logger.info("\nChecking selector table")
        self.checkSel()
        logger.info("\nChecking range table")
        self.checkRange()

        # Clear any updates this test may have generated
        self.client.get_tbl_updates(vir_dev_id)



class TestKeylessRegister(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])
    def runTest(self):
        setup_random()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        hw_async_flag = iterator_register_flags_t(read_hw_sync = False)
        hw_sync_flag  = iterator_register_flags_t(read_hw_sync = True)

        shdl = None
        dt = DevTarget_t(phy_dev_id, hex_to_i16(0xFFFF))
        vir_dt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))

        try:
            shdl = self.conn_mgr.client_init()
            h = self.client.r_set_default_action_r0_inc(shdl, dt, 3)
            self.conn_mgr.complete_operations(shdl)
            self.client.register_hw_sync_r0(shdl, dt)
            r = self.client.register_read_r0(shdl, dt, h, hw_async_flag)
            self.assertEqual(r, [3]*num_pipes)
            self.client.register_write_r0(shdl, phy_dev_id, h, 4)
            self.conn_mgr.complete_operations(shdl)
            r = self.client.register_read_r0(shdl, dt, h, hw_sync_flag)
            self.assertEqual(r, [4]*num_pipes)

        finally:
            if shdl:
                self.client.r_table_reset_default_entry(shdl, dt)
                # This is the physical device test, and it cleans up the virtual device
                # default entry as well. These default entries are P4 specified and are installed
                # at boot up, so when this test just cleans up the physical device then HLP/LLP
                # will be out of sync for the next virtual device test. Hence this cleans up
                # both physical device and virtual device default entry
                self.client.r_table_reset_default_entry(shdl, vir_dt)
                # The mat update is just thrown away since the physical device default entry
                # is already cleaned up.
                x = self.client.get_tbl_updates(vir_dev_id)
                self.conn_mgr.client_cleanup( shdl )


class TestKeylessRegisterVirtualDev(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])
    def runTest(self):
        setup_random()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        hw_async_flag = iterator_register_flags_t(read_hw_sync = False)
        hw_sync_flag  = iterator_register_flags_t(read_hw_sync = True)

        shdl = None
        dt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        phy_dt = DevTarget_t(phy_dev_id, hex_to_i16(0xFFFF))

        try:
            shdl = self.conn_mgr.client_init()
            self.client.r_register_mat_update_cb(shdl, vir_dev_id)
            h = self.client.r_set_default_action_r0_inc(shdl, dt, 3)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            self.client.register_hw_sync_r0(shdl, phy_dt)
            r = self.client.register_read_r0(shdl, phy_dt, h, hw_async_flag)
            self.assertEqual(r, [3]*num_pipes)
            self.client.register_write_r0(shdl, vir_dev_id, h, 4)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)
            r = self.client.register_read_r0(shdl, phy_dt, h, hw_sync_flag)
            self.assertEqual(r, [4]*num_pipes)

        finally:
            if shdl:
                self.client.r_table_reset_default_entry(shdl, dt)
                x = self.client.get_tbl_updates(vir_dev_id)
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
                # Same comment as the above test. For the same reason the virtual device
                # test cleans up the physical device default entry as well.
                self.client.r_table_reset_default_entry(shdl, phy_dt)
                self.conn_mgr.client_cleanup( shdl )


class TestNoKeyVirtualDevSetDflt(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])
    def runTest(self):
        setup_random()

        shdl = None
        dt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        phy_dt = DevTarget_t(phy_dev_id, hex_to_i16(0xFFFF))

        try:
            shdl = self.conn_mgr.client_init()

            # Check "t_no_key" first, this is a keyless table which runs an action
            # with action parameters.  Expect it to be managed as a tcam table with
            # no TCAM units assigned.
            self.client.t_no_key_register_mat_update_cb(shdl, vir_dev_id)
            smac = '00:11:22:33:44:55'
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string(smac))
            h = self.client.t_no_key_set_default_action_set_smac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)
            entry_data = self.client.t_no_key_get_entry(shdl, vir_dev_id, h, False)
            self.assertEqual('set_smac', entry_data.action_desc.name)
            self.assertEqual(aspec, entry_data.action_desc.data.iterator_set_smac)

            smac = '00:00:00:00:00:00'
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string(smac))
            self.client.t_no_key_table_reset_default_entry(shdl, dt)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)
            entry_data = self.client.t_no_key_get_entry(shdl, vir_dev_id, h, False)
            self.assertEqual('set_smac', entry_data.action_desc.name)
            self.assertEqual(aspec, entry_data.action_desc.data.iterator_set_smac)

            # Check "bf" next, this is a keyless table which drives an indirect
            # register indexed by PHV.  This should be presented as the 
            # TABLE_WITH_NO_KEY table type.
            self.client.bf_register_mat_update_cb(shdl, vir_dev_id)
            h = self.client.bf_set_default_action_run_bf(shdl, dt)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)
            entry_data = self.client.bf_get_entry(shdl, vir_dev_id, h, False)
            self.assertEqual('run_bf', entry_data.action_desc.name)
            self.client.bf_table_reset_default_entry(shdl, dt)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)
            entry_data = self.client.bf_get_entry(shdl, vir_dev_id, h, False)
            self.assertEqual('run_bf', entry_data.action_desc.name)


        finally:
            if shdl:
                self.client.t_no_key_table_reset_default_entry(shdl, dt)
                self.client.bf_table_reset_default_entry(shdl, dt)
                x = self.client.get_tbl_updates(vir_dev_id)
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
                self.conn_mgr.client_cleanup(shdl)


class TestDefaultEntryModifyVirtualDev(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])
    def runTest(self):
        setup_random()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        hw_async_flag = iterator_register_flags_t(read_hw_sync = False)
        hw_sync_flag  = iterator_register_flags_t(read_hw_sync = True)

        shdl = None
        dt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        phy_dt = DevTarget_t(phy_dev_id, hex_to_i16(0xFFFF))

        try:
            self.client.get_tbl_updates(vir_dev_id)
            shdl = self.conn_mgr.client_init()
            self.client.e_with_key_register_mat_update_cb(shdl, vir_dev_id)
            self.client.t_with_key_register_mat_update_cb(shdl, vir_dev_id)

            # Make sure the default entries are not set
            self.client.e_with_key_table_reset_default_entry(shdl, dt)
            self.client.t_with_key_table_reset_default_entry(shdl, dt)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)

            #
            # Test an exact match table
            #

            # Add a new default entry.  The update type could be SET_DFLT or MOD
            # depending on if the default entry was already present.  The reset
            # default call above may reset the table to use a no-action entry
            # since the P4C compiler always specifies a default action for tables.
            aspec = iterator_set_dmac_action_spec_t(macAddr_to_string('00:11:22:33:44:55'))
            h = self.client.e_with_key_set_default_action_set_dmac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            #self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_SET_DFLT)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the set-default-action API
            # The update type will now be a MAT_UPDATE_MOD instead of MAT_UPDATE_SET_DFLT
            aspec = iterator_set_dmac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            h = self.client.e_with_key_set_default_action_set_dmac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the modify-entry API
            # The update type will again be MAT_UPDATE_MOD
            aspec = iterator_set_dmac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            self.client.e_with_key_table_modify_with_set_dmac(shdl, vir_dev_id, h, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the set-default-action API
            # Force the update type to be MAT_UPDATE_SET_DFLT
            aspec = iterator_set_dmac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            h = self.client.e_with_key_set_default_action_set_dmac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            entry_hdl                 = x[0].update_data.mat.update_params.mod.entry_hdl
            action_profile_mbr        = x[0].update_data.mat.update_params.mod.action_profile_mbr
            action_index              = x[0].update_data.mat.update_params.mod.action_index
            action_profile_mbr_exists = x[0].update_data.mat.update_params.mod.action_profile_mbr_exists
            sel_grp_hdl               = x[0].update_data.mat.update_params.mod.sel_grp_hdl
            selection_index           = x[0].update_data.mat.update_params.mod.selection_index
            num_selector_indices      = x[0].update_data.mat.update_params.mod.num_selector_indices
            sel_grp_exists            = x[0].update_data.mat.update_params.mod.sel_grp_exists
            drv_data                  = x[0].update_data.mat.update_params.mod.drv_data
            x[0].update_data.mat.update_params.set_dflt = iterator_mat_update_set_dflt_params(entry_hdl                 = entry_hdl,
                                                                                              action_profile_mbr        = action_profile_mbr,
                                                                                              action_index              = action_index,
                                                                                              action_profile_mbr_exists = action_profile_mbr_exists,
                                                                                              sel_grp_hdl               = sel_grp_hdl,
                                                                                              selection_index           = selection_index,
                                                                                              num_selector_indices      = num_selector_indices,
                                                                                              sel_grp_exists            = sel_grp_exists,
                                                                                              drv_data                  = drv_data)
            x[0].update_data.mat.update_type = mat_update_type.MAT_UPDATE_SET_DFLT
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Clear the default entry and do it again...
            self.client.e_with_key_table_reset_default_entry(shdl, dt)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)

            # Add a new default entry
            aspec = iterator_set_dmac_action_spec_t(macAddr_to_string('00:11:22:33:44:55'))
            h = self.client.e_with_key_set_default_action_set_dmac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            #self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_SET_DFLT)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the set-default-action API
            # The update type will now be a MAT_UPDATE_MOD instead of MAT_UPDATE_SET_DFLT
            aspec = iterator_set_dmac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            h = self.client.e_with_key_set_default_action_set_dmac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the modify-entry API
            # The update type will again be MAT_UPDATE_MOD
            aspec = iterator_set_dmac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            self.client.e_with_key_table_modify_with_set_dmac(shdl, vir_dev_id, h, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the set-default-action API
            # Force the update type to be MAT_UPDATE_SET_DFLT
            aspec = iterator_set_dmac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            h = self.client.e_with_key_set_default_action_set_dmac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            entry_hdl                 = x[0].update_data.mat.update_params.mod.entry_hdl
            action_profile_mbr        = x[0].update_data.mat.update_params.mod.action_profile_mbr
            action_index              = x[0].update_data.mat.update_params.mod.action_index
            action_profile_mbr_exists = x[0].update_data.mat.update_params.mod.action_profile_mbr_exists
            sel_grp_hdl               = x[0].update_data.mat.update_params.mod.sel_grp_hdl
            selection_index           = x[0].update_data.mat.update_params.mod.selection_index
            num_selector_indices      = x[0].update_data.mat.update_params.mod.num_selector_indices
            sel_grp_exists            = x[0].update_data.mat.update_params.mod.sel_grp_exists
            drv_data                  = x[0].update_data.mat.update_params.mod.drv_data
            x[0].update_data.mat.update_params.set_dflt = iterator_mat_update_set_dflt_params(entry_hdl                 = entry_hdl,
                                                                                              action_profile_mbr        = action_profile_mbr,
                                                                                              action_index              = action_index,
                                                                                              action_profile_mbr_exists = action_profile_mbr_exists,
                                                                                              sel_grp_hdl               = sel_grp_hdl,
                                                                                              selection_index           = selection_index,
                                                                                              num_selector_indices      = num_selector_indices,
                                                                                              sel_grp_exists            = sel_grp_exists,
                                                                                              drv_data                  = drv_data)
            x[0].update_data.mat.update_type = mat_update_type.MAT_UPDATE_SET_DFLT
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            #
            # Test a ternary match table
            #

            # Add a new default entry
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string('00:11:22:33:44:55'))
            h = self.client.t_with_key_set_default_action_set_smac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            #self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_SET_DFLT)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the set-default-action API
            # The update type will now be a MAT_UPDATE_MOD instead of MAT_UPDATE_SET_DFLT
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            h = self.client.t_with_key_set_default_action_set_smac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the modify-entry API
            # The update type will again be MAT_UPDATE_MOD
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            self.client.t_with_key_table_modify_with_set_smac(shdl, vir_dev_id, h, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the set-default-action API
            # Force the update type to be MAT_UPDATE_SET_DFLT
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            h = self.client.t_with_key_set_default_action_set_smac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            entry_hdl                 = x[0].update_data.mat.update_params.mod.entry_hdl
            action_profile_mbr        = x[0].update_data.mat.update_params.mod.action_profile_mbr
            action_index              = x[0].update_data.mat.update_params.mod.action_index
            action_profile_mbr_exists = x[0].update_data.mat.update_params.mod.action_profile_mbr_exists
            sel_grp_hdl               = x[0].update_data.mat.update_params.mod.sel_grp_hdl
            selection_index           = x[0].update_data.mat.update_params.mod.selection_index
            num_selector_indices      = x[0].update_data.mat.update_params.mod.num_selector_indices
            sel_grp_exists            = x[0].update_data.mat.update_params.mod.sel_grp_exists
            drv_data                  = x[0].update_data.mat.update_params.mod.drv_data
            x[0].update_data.mat.update_params.set_dflt = iterator_mat_update_set_dflt_params(entry_hdl                 = entry_hdl,
                                                                                              action_profile_mbr        = action_profile_mbr,
                                                                                              action_index              = action_index,
                                                                                              action_profile_mbr_exists = action_profile_mbr_exists,
                                                                                              sel_grp_hdl               = sel_grp_hdl,
                                                                                              selection_index           = selection_index,
                                                                                              num_selector_indices      = num_selector_indices,
                                                                                              sel_grp_exists            = sel_grp_exists,
                                                                                              drv_data                  = drv_data)
            mat_update_type.MAT_UPDATE_SET_DFLT
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Clear the default entry and do it again...
            self.client.t_with_key_table_reset_default_entry(shdl, dt)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)

            # Add a new default entry
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string('00:11:22:33:44:55'))
            h = self.client.t_with_key_set_default_action_set_smac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            #self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_SET_DFLT)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the set-default-action API
            # The update type will now be a MAT_UPDATE_MOD instead of MAT_UPDATE_SET_DFLT
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            h = self.client.t_with_key_set_default_action_set_smac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the modify-entry API
            # The update type will again be MAT_UPDATE_MOD
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            self.client.t_with_key_table_modify_with_set_smac(shdl, vir_dev_id, h, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)

            # Modify the default entry using the set-default-action API
            # Force the update type to be MAT_UPDATE_SET_DFLT
            aspec = iterator_set_smac_action_spec_t(macAddr_to_string('55:44:33:22:11:00'))
            h = self.client.t_with_key_set_default_action_set_smac(shdl, dt, aspec)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.assertEqual(len(x), 1)
            self.assertEqual(x[0].update_type, tbl_update_type.MAT_UPDATE_TYPE)
            self.assertEqual(x[0].update_data.mat.update_type, mat_update_type.MAT_UPDATE_MOD)
            entry_hdl                 = x[0].update_data.mat.update_params.mod.entry_hdl
            action_profile_mbr        = x[0].update_data.mat.update_params.mod.action_profile_mbr
            action_index              = x[0].update_data.mat.update_params.mod.action_index
            action_profile_mbr_exists = x[0].update_data.mat.update_params.mod.action_profile_mbr_exists
            sel_grp_hdl               = x[0].update_data.mat.update_params.mod.sel_grp_hdl
            selection_index           = x[0].update_data.mat.update_params.mod.selection_index
            num_selector_indices      = x[0].update_data.mat.update_params.mod.num_selector_indices
            sel_grp_exists            = x[0].update_data.mat.update_params.mod.sel_grp_exists
            drv_data                  = x[0].update_data.mat.update_params.mod.drv_data
            x[0].update_data.mat.update_params.set_dflt = iterator_mat_update_set_dflt_params(entry_hdl = entry_hdl,
                                                                                              action_profile_mbr = action_profile_mbr,
                                                                                              action_index = action_index,
                                                                                              action_profile_mbr_exists = action_profile_mbr_exists,
                                                                                              sel_grp_hdl = sel_grp_hdl,
                                                                                              selection_index = selection_index,
                                                                                              num_selector_indices = num_selector_indices,
                                                                                              sel_grp_exists = sel_grp_exists,
                                                                                              drv_data = drv_data)
            x[0].update_data.mat.update_type = mat_update_type.MAT_UPDATE_SET_DFLT
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.complete_operations(shdl)


        finally:
            if shdl:
                self.client.e_with_key_table_reset_default_entry(shdl, dt)
                self.client.t_with_key_table_reset_default_entry(shdl, dt)
                x = self.client.get_tbl_updates(vir_dev_id)
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
                self.conn_mgr.client_cleanup(shdl)

class TestIListChkpt(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])

    def runTest(self):
        if test_param_get("arch") == "tofino3":
            # This test makes no sense on tofino3, since read from HW is not supported on P0
            logger.info("Test not supported on TF3, skipping")
            return

        setup_random()
        num_pipes = int(test_param_get('num_pipes'))
        pipe_list = [x for x in range(num_pipes)]
        hw_async_flag = iterator_register_flags_t(read_hw_sync = False)
        hw_sync_flag  = iterator_register_flags_t(read_hw_sync = True)

        shdl = None
        dt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        phy_dt = DevTarget_t(phy_dev_id, hex_to_i16(0xFFFF))

        try:
            shdl = self.conn_mgr.client_init()

            # Clear any updates left over by earlier tests
            self.client.get_tbl_updates(vir_dev_id)

            self.client.p0_register_mat_update_cb(shdl, vir_dev_id)
            # Use any port for the key
            key = 4
            ms = iterator_p0_match_spec_t(hex_to_i16(key))
            # Update the entry a few times with different action data
            for val in range(1,10):
                a = iterator_N_action_spec_t(val)
                entry_hdl = self.client.p0_table_add_with_N(shdl, dt, ms, a)

            # Push the operations to HW
            x = self.client.get_tbl_updates(vir_dev_id)
            self.conn_mgr.begin_batch(shdl)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.end_batch(shdl, True)

            # We expect the last update to be there.
            entry_data = self.client.p0_get_entry(shdl, phy_dev_id, entry_hdl, True)
            self.assertEqual(ms, entry_data.match_spec)
            aspec = iterator_N_action_spec_t(val)
            self.assertEqual('N', entry_data.action_desc.name)
            self.assertEqual(a, entry_data.action_desc.data.iterator_N)

            # Add the entry three times with different action data
            a = iterator_N_action_spec_t(100)
            self.client.p0_table_add_with_N(shdl, dt, ms, a)
            a = iterator_N_action_spec_t(101)
            self.client.p0_table_add_with_N(shdl, dt, ms, a)
            a = iterator_N_action_spec_t(102)
            self.client.p0_table_add_with_N(shdl, dt, ms, a)

            # Before proramming the hardware corrupt the data to cause a
            # programming failure on the second update
            x = self.client.get_tbl_updates(vir_dev_id)
            # Is it add or mod?  Who cares...
            if x[1].update_data.mat.update_type == mat_update_type.MAT_UPDATE_ADD:
                x[1].update_data.mat.update_params.add.entry_hdl = 0
            if x[1].update_data.mat.update_type == mat_update_type.MAT_UPDATE_MOD:
                x[1].update_data.mat.update_params.mod.entry_hdl = 0
            self.conn_mgr.begin_batch(shdl)
            try:
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
                self.conn_mgr.end_batch(shdl, True)
                self.assertFalse(True)
            except InvalidPlcmtOperation as e:
                pass
            self.conn_mgr.end_batch(shdl, True)

            # We expect the first update to be there as the second update was
            # rejected and the third was never processed.
            entry_data = self.client.p0_get_entry(shdl, phy_dev_id, entry_hdl, True)
            self.assertEqual(ms, entry_data.match_spec)
            self.assertEqual('N', entry_data.action_desc.name)
            self.assertEqual(iterator_N_action_spec_t(100),
                             entry_data.action_desc.data.iterator_N)

        finally:
            self.client.p0_table_delete(shdl, vir_dev_id, entry_hdl)
            x = self.client.get_tbl_updates(vir_dev_id)
            self.plcmt.process_plcmt_data(shdl, phy_dev_id, x)
            self.conn_mgr.client_cleanup(shdl)


class Test9Pack8(pd_base_tests.ThriftInterfaceDataPlane):
    """
    Check all 36 locations of an entry in a four way 9 entry packing EXM table
    with maximum width (8 SRAMs).
    The test will add the same key 36 times to a virtual device to discover all
    locations for the key.
    The test will then install dummy entries at all 36 locations with specific
    action data and a different action.  For each location the dummy entry will
    be replaced with the real entry and a packet set.  We verify the match by
    checking the correct action with the correct action data was run.  The
    entry will be replaced with a dummy entry before moving onto the next table
    location.
    """
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["iterator"])
    def runTest(self):
        shdl = None
        dt = DevTarget_t(vir_dev_id, hex_to_i16(0xFFFF))
        phy_dt = DevTarget_t(phy_dev_id, hex_to_i16(0xFFFF))
        port = swports[0]
        port_pipe = port_to_pipe(port)
        port_dt = DevTarget_t(phy_dev_id, port_pipe)

        # The P4 program has the table fixed at four ways with nine entry
        # packing so any given key can go in ways times pack number of table
        # locations.
        ways = 4
        pack = 9
        expected_location_count = ways * pack

        try:
            shdl = self.conn_mgr.client_init()

            # Clear any updates left over by earlier tests
            self.client.get_tbl_updates(vir_dev_id)
            self.client.wide_exm_register_mat_update_cb(shdl, vir_dev_id)

            # Turn off duplicate entry checks so we can insert the same key
            # multiple times to discover all table locations for the entry.
            self.client.wide_exm_set_property(shdl,
                                              vir_dev_id,
                                              tbl_property_t.TBL_PROP_DUPLICATE_ENTRY_CHECK,
                                              tbl_property_value_t.DUPLICATE_ENTRY_CHECK_DISABLE, 0)

            # Pick any entry to test and create a dummy entry as well.
            dmac = 'FF:FF:FF:FF:FF:FF'
            smac = '70:70:70:70:70:70'
            etyp = 0x86dd
            smac_mod = '80:80:80:80:80:80'
            dmac_dummy = 'FF:FF:FF:0F:FF:FF' # Note one nibble difference with "dmac"
            dmac_mod_dummy = '90:90:90:90:90:90'
            mspec = iterator_wide_exm_match_spec_t(ethernet_etherType=hex_to_i16(etyp),
                                                   ethernet_dstAddr=macAddr_to_string(dmac),
                                                   ethernet_srcAddr=macAddr_to_string(smac))
            aspec = iterator_exm_a1_action_spec_t(action_smac=macAddr_to_string(smac_mod),
                                                  action_etype=hex_to_i16(etyp))
            mspec_dummy = iterator_wide_exm_match_spec_t(ethernet_etherType=hex_to_i16(etyp),
                                                         ethernet_dstAddr=macAddr_to_string(dmac_dummy),
                                                         ethernet_srcAddr=macAddr_to_string(smac))
            aspec_dummy = iterator_exm_a2_action_spec_t(action_dmac=macAddr_to_string(dmac_mod_dummy),
                                                        action_etype=hex_to_i16(etyp))

            # Add both entries and get the placement data for later use in
            # building placement operations.
            h = self.client.wide_exm_table_add_with_exm_a1(shdl, dt, mspec, aspec, 0)
            ent_add = self.client.get_tbl_updates(vir_dev_id)[0]
            self.client.wide_exm_table_delete(shdl, vir_dev_id, h)
            ent_del = self.client.get_tbl_updates(vir_dev_id)[0]
            h = self.client.wide_exm_table_add_with_exm_a2(shdl, dt, mspec_dummy, aspec_dummy, 0)
            dummy_add = self.client.get_tbl_updates(vir_dev_id)[0]
            self.client.wide_exm_table_delete(shdl, vir_dev_id, h)
            dummy_del = self.client.get_tbl_updates(vir_dev_id)[0]

            # Add the entry until there are no more locations for it.  Expect
            # either "expected_location_count" or one less locations since we
            # may be unlucky and hash to a location reserved for the default
            # entry and have one less location to use.
            hdls = list()
            while True:
                try:
                    h = self.client.wide_exm_table_add_with_exm_a1(shdl, dt, mspec, aspec, 0)
                    hdls.append(h)
                except:
                    break
            self.assertTrue(len(hdls) == expected_location_count or len(hdls) == expected_location_count-1)
            ops = self.client.get_tbl_updates(vir_dev_id)
            # Now clean up these entries from the virtual device as they are no
            # longer needed.
            for h in hdls:
                self.client.wide_exm_table_delete(shdl, vir_dev_id, h)
            self.client.get_tbl_updates(vir_dev_id)

            # All the operations must be adds, one per entry.
            self.assertEqual(len(hdls), len(ops))
            for op in ops:
                self.assertEqual(op.update_type, iterator_tbl_update_type.MAT_UPDATE_TYPE)
                self.assertEqual(op.update_data.mat.update_type, iterator_mat_update_type.MAT_UPDATE_ADD)

            # Hold onto all entry locations
            locations = [op.update_data.mat.update_params.add.entry_index for op in ops]

            # First add the dummy entry at all locations.
            logger.info("\nHandle\tLocation")
            for i,loc in enumerate(locations):
                hdl = i + 1000 # Give each a unique entry handle
                dummy_add.update_data.mat.update_params.add.entry_hdl = hdl
                dummy_add.update_data.mat.update_params.add.entry_index = loc
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, [dummy_add])

                logger.info("%d\t %d (0x%x)", hdl, loc, loc)

            # Go through each table location, replace the dummy entry there with
            # our real entry and make sure it can match properly.
            first_failure = None
            for i,loc in enumerate(locations):
                # Remove the dummy entry
                dummy_del.update_data.mat.update_params.remove.entry_hdl = i + 1000
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, [dummy_del])
                # Add the real entry
                ent_add.update_data.mat.update_params.add.entry_hdl = 100
                ent_add.update_data.mat.update_params.add.entry_index = loc
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, [ent_add])
                # Send a packet to check if the entry is working properly
                self.conn_mgr.complete_operations(shdl)
                p_in  = simple_udpv6_packet(eth_dst=dmac, eth_src=smac)
                p_out = simple_udpv6_packet(eth_dst=dmac, eth_src=smac_mod)
                send_packet(self, swports[0], p_in)
                failure = None
                try:
                    verify_packet(self, p_out, swports[0])
                except AssertionError as e:
                    logger.info("Location %d (0x%x) FAILED", loc, loc)
                    if not first_failure: first_failure = e
                # Remove the real entry and replace it with the dummy entry
                ent_del.update_data.mat.update_params.remove.entry_hdl = 100
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, [ent_del])
                dummy_add.update_data.mat.update_params.add.entry_hdl = i + 1000
                dummy_add.update_data.mat.update_params.add.entry_index = loc
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, [dummy_add])

            # Clean up all the physical device entries
            for i,_ in enumerate(locations):
                dummy_del.update_data.mat.update_params.remove.entry_hdl = i + 1000
                self.plcmt.process_plcmt_data(shdl, phy_dev_id, [dummy_del])

            if first_failure:
                raise first_failure
        finally:
            if shdl:
                self.conn_mgr.client_cleanup(shdl)
