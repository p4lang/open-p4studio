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

from randomdict import RandomDict
from collections import OrderedDict

import time
import datetime
import sys
import logging
import math
import uuid
import pdb
import json
import copy

import unittest
import random

import pd_base_tests

from ptf import config
from ptf.testutils import *
from p4testutils.misc_utils import *
from ptf.thriftutils import *

import os

from drivers_test.p4_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *

from bf_tables import *
import bf_log

#from pycallgraph import PyCallGraph
#from pycallgraph.output import GraphvizOutput

swports = get_sw_ports()

def port_to_pipe(port):
    return port // 128

class BFTest():
    ''' Bf-drivers test helper class'''

    def __init__(self, tiface, sess_hdl, v_dev_id, p_dev_id, match_tbl, resource_tbls, resource_map):
        ''' Input is a dictionary of one match and associated tables '''
        self.resource_map = resource_map
        self.tiface = tiface
        self.match_tbl = match_tbl
        self.tbls = resource_tbls
        self.sess_hdl = sess_hdl
        self.p_dev_id = p_dev_id
        self.v_dev_id = v_dev_id
        self.action_funcs = resource_map['Action_funcs']
        self.default_init()
        self.commit_prob = 0.8
        self.batch_mode = 0
        self.do_hitless_ha = resource_map['Hitless_HA']
        if resource_map['Template'] == 'min':
            self.remove_scaling_factor = 0.75
            self.txn_prob = 0.33
        else:
            self.remove_scaling_factor = 0.25
            self.txn_prob = 0.15
        if resource_map['Txn'] == 'Off':
            self.txn_prob = 0

    def refresh_thrift(self):
        self.tiface.reconnect_thrift()
        self.match_tbl.refresh_thrift(self.tiface, self.resource_map)
        for t in self.tbls: t.refresh_thrift(self.tiface, self.resource_map)

    def default_init(self):
        self.valid_pipes = ['all']
        num_pipes = int(test_param_get('num_pipes'))
        print("Num pipes allowed for test is", num_pipes)
        self.pipe_list = [x for x in range(num_pipes)]
        self.init_all_tables()
        for tbl in self.tbls + [self.match_tbl]:
            tbl.default_init(self.sess_hdl, self.p_dev_id, self.v_dev_id)

    def enable_updates_after_hitless_ha(self):
        for tbl in self.tbls + [self.match_tbl]:
            tbl.enable_updates_after_hitless_ha(self.sess_hdl, self.p_dev_id, self.v_dev_id)

    def default_teardown(self):
        for tbl in self.tbls + [self.match_tbl]:
            tbl.default_teardown(self.sess_hdl, self.p_dev_id, self.v_dev_id)

    def teardown(self):
        self.clear_all_entries()
        self.default_teardown()

    def init_all_tables(self):
        self.modified_entries = RandomDict()
        self.added_entries = []
        # Resource id is a tuple of the name and a uniq_val
        #         res_id = (tbl.get_name(), uniq_val)
        # Resource record stores the main record for the resource
        # It is indexed as follows:
        # [#table_name] - Dict()
        #     [#records] - Dict() Stores the records
        #         [#res_id] - Dict() ---- This is the rdata  (The bf_tables can
        #                                 add other things in addition to below
        #            [#uniq_val] - Stores the uniq value for this resource
        #            [#resource_record] - Dict() Stores the resource record
        #               [#target]
        #               [#action_func]
        #               [#ref-by]
        #               [#resources]
        #                   [#resource_name] - [ids] List of ids of this type
        #            [#is_default_entry] -
        #     [#available_ids] - Dict() Stores the available resource ids
        #         [#pipe] - Dict()
        #            [#res_id] - True/False
        #     [#installed] - Dict() Stores the installed resource ids
        #         [#(pipe, action_func)] - Dict()
        #             [#res_id] - True/False
        self.resource_records = RandomDict()
        self.default_entries = dict()

        for resource_tbl in self.tbls + [self.match_tbl]:
            self.resource_records[resource_tbl.name] = RandomDict()
            self.resource_records[resource_tbl.name]['records'] = RandomDict()
            # Maintain available ids and installed ids for quick lookup/removal
            # Available ids are maintained per pipe
            # Installed ids are maintained per pipe + action_func
            self.resource_records[resource_tbl.name]['available_ids'] = OrderedDict()
            self.resource_records[resource_tbl.name]['installed'] = OrderedDict()


        uniq_keys = self.valid_pipes
        for tbl in self.tbls + [self.match_tbl]:
            records_count = tbl.get_size()
            uniq_vals = [x for x in range(1, records_count+1)]
            for uniq_val in uniq_vals:
                entry_map = OrderedDict()
                entry_map['uniq_val'] = uniq_val
                res_id = (tbl.get_name(), uniq_val)
                self.resource_records[tbl.name]['records'][res_id] = entry_map
            i = 0
            shuffled_keys = list(self.resource_records[tbl.name]['records'].keys())
            random.shuffle(shuffled_keys)

            # Equally divide the uniq-vals among available uniq-keys
            for key in uniq_keys:
                bf_log.log('Initing %s %s' % (tbl.get_name(), str(key)))
                self.resource_records[tbl.name]['available_ids'][key] = RandomDict()
                for _ in range(records_count//len(uniq_keys)):
                    res_id = shuffled_keys[i]
                    self.resource_records[tbl.name]['available_ids'][key][res_id] = None
                    i += 1

    def get_rdata(self, res_id, backup=True, lookup_db=None):
        res_name, _ = res_id
        tbl = self.get_resource_tbl_by_name(res_name)

        if lookup_db:
            return lookup_db[tbl.name]['records'][res_id]

        if self.batch_mode == 2 and backup:
            if res_id not in self.backup_resource_records[tbl.name]['records']:
                self.backup_resource_records[tbl.name]['records'][res_id] = copy.deepcopy(self.resource_records[tbl.name]['records'][res_id])
        return self.resource_records[tbl.name]['records'][res_id]

    def swap_uniq_vals(self, r1, r2):
        rd1 = self.get_rdata(r1)
        rd2 = self.get_rdata(r2)
        rd1['uniq_val'], rd2['uniq_val'] = rd2['uniq_val'], rd1['uniq_val']

    def get_random_available_id(self, tbl, key, count=1):
        random_ids = self.resource_records[tbl.name]['available_ids'][key].random_key_samples(count)
        return random_ids

    def pop_available_id(self, tbl, key, rid):
        assert rid in self.resource_records[tbl.name]['available_ids'][key]

        if self.batch_mode == 2:
            if key not in self.backup_resource_records[tbl.name]['available_ids']:
                self.backup_resource_records[tbl.name]['available_ids'][key] = RandomDict()
            if rid not in self.backup_resource_records[tbl.name]['available_ids'][key]:
                self.backup_resource_records[tbl.name]['available_ids'][key][rid] = True
        self.resource_records[tbl.name]['available_ids'][key].pop(rid)
        if rid in bf_log.debug_res_ids:
            bf_log.log('\n\t\tPopping ' + str(rid) + ' from available')

    def push_available_id(self, tbl, key, rid):
        assert rid not in self.resource_records[tbl.name]['available_ids'][key]

        if self.batch_mode == 2:
            if key not in self.backup_resource_records[tbl.name]['available_ids']:
                self.backup_resource_records[tbl.name]['available_ids'][key] = RandomDict()
            if rid not in self.backup_resource_records[tbl.name]['available_ids'][key]:
                self.backup_resource_records[tbl.name]['available_ids'][key][rid] = False
        self.resource_records[tbl.name]['available_ids'][key][rid] = None
        if rid in bf_log.debug_res_ids:
            bf_log.log('\n\t\tPushing ' + str(rid) + ' to available')

    def count_available_ids(self, tbl, key):
        return len(self.resource_records[tbl.name]['available_ids'][key])

    def get_random_installed_ids(self, tbl, action_func, pipe, count = 1):
        return self.resource_records[tbl.name]['installed'][(pipe, action_func)].random_key_samples(count)

    def is_id_installed(self, tbl, pipe, rid):
        return rid not in self.resource_records[tbl.name]['available_ids'][pipe]

    def push_installed_id(self, tbl, action_func, pipe, rid):
        k = (pipe, action_func)
        try:
            assert k not in self.resource_records[tbl.name]['installed'] or rid not in self.resource_records[tbl.name]['installed'][k]
        except:
            #pdb.set_trace()
            raise
        if self.batch_mode == 2:
            if k not in self.backup_resource_records[tbl.name]['installed']:
                self.backup_resource_records[tbl.name]['installed'][k] = RandomDict()
            if rid not in self.backup_resource_records[tbl.name]['installed'][k]:
                self.backup_resource_records[tbl.name]['installed'][k][rid] = False
        self.resource_records[tbl.name]['installed'].setdefault(k, RandomDict())
        self.resource_records[tbl.name]['installed'][k][rid] = None
        if rid in bf_log.debug_res_ids:
            bf_log.log('\n\t\tPushing ' + str(rid) + ' to installed')

    def pop_installed_id(self, tbl, action_func, pipe, rid):
        k = (pipe, action_func)
        assert rid in self.resource_records[tbl.name]['installed'][k]
        if self.batch_mode == 2:
            if k not in self.backup_resource_records[tbl.name]['installed']:
                self.backup_resource_records[tbl.name]['installed'][k] = RandomDict()
            if rid not in self.backup_resource_records[tbl.name]['installed'][k]:
                self.backup_resource_records[tbl.name]['installed'][k][rid] = True
        self.resource_records[tbl.name]['installed'][(pipe, action_func)].pop(rid)
        if rid in bf_log.debug_res_ids:
            bf_log.log('\n\t\tPopping ' + str(rid) + ' from installed')

    def installed_resources(self, tbl):
        ''' Returns a list of installed resource-ids'''
        rids = []
        for r in self.resource_records[tbl.name]['installed'].values():
            rids += r.keys()
        return rids

    def get_resource_tbl_by_name(self, name):
        if name == 'Match':
            return self.match_tbl
        for resource in self.tbls:
            if resource.get_name() == name:
                return resource
        return None

    def get_resource_record(self, resource_tbl, resource_id, backup=True, lookup_db=None):
        return self.get_rdata(resource_id, backup=backup, lookup_db=lookup_db)['resource_record']

    def pop_resource_record(self, resource_id):
        rdata = self.get_rdata(resource_id)
        rdata.pop('resource_record')
        if len(bf_log.debug_res_ids & set([resource_id])):
            bf_log.log("POPPED resource record for " + str(resource_id))


    def push_resource_record(self, resource_id, record):
        rdata = self.get_rdata(resource_id)
        rdata['resource_record'] = record
        if len(bf_log.debug_res_ids & set([resource_id])):
            bf_log.log("PUSHED resource record for " + str(resource_id))

    def get_action_func_for_match_entry(self, match_entry):
        resource_record = self.get_resource_record(self.match_tbl, match_entry, False)
        return resource_record['action_func']

    def _lookup_match_references(self, match_entries, resource_name, resource_id):
        tbl = self.get_resource_tbl_by_name(resource_name)
        resource_record = self.get_resource_record(tbl, resource_id, False)
        for ref_type, ref_ids in resource_record['ref-by'].items():
            if ref_type == 'Match':
                match_entries.extend(ref_ids.keys())
            for ref_id in ref_ids:
                self._lookup_match_references(match_entries, ref_type, ref_id)

    def get_match_entries_for_resource(self, resource_tbl, resource_id):
        ''' Return a list of match entry ids that refer to this resource '''
        if resource_tbl == self.match_tbl:
            return [resource_id]

        match_entries = []

        self._lookup_match_references(match_entries, resource_tbl.get_name(), resource_id)
        return match_entries

    def add_resource_reference(self, ref_id, ref_type, resource_id):
        res_name, _ = ref_id
        tbl = self.get_resource_tbl_by_name(res_name)
        resource_record = self.get_resource_record(tbl, ref_id)
        ref_list = resource_record['ref-by'].setdefault(ref_type, RandomDict())
        ref_list = resource_record['ref-by'][ref_type][resource_id] = None

    def remove_resource_reference(self, resource_tbl, ref_id, ref_type, resource_id):
        resource_record = self.get_resource_record(resource_tbl, ref_id)
        resource_record['ref-by'][ref_type].pop(resource_id)
        if len(resource_record['ref-by'][ref_type]) == 0:
            resource_record['ref-by'].pop(ref_type)

    def _lookup_resource(self, record, resource_name, resource_id):
        tbl = self.get_resource_tbl_by_name(resource_name)
        resource_record = self.get_resource_record(tbl, resource_id, False)['resources']
        for resource_name, rlist in resource_record.items():
            if resource_name in record:
                record[resource_name].extend(rlist[:])
            else:
                record[resource_name] = rlist[:]

        for resource_name, rlist in resource_record.items():
            for resource_id in rlist:
                self._lookup_resource(record, resource_name, resource_id)

    def get_resource_record_for_match_entry(self, match_id):
        ''' Get the combined resource record for match-entry including all
            the child resource records
        '''
        resource_record = copy.deepcopy(self.get_resource_record(self.match_tbl, match_id, False))
        resource_record['resources'] = dict()
        self._lookup_resource(resource_record['resources'], 'Match', match_id)
        return resource_record

    def get_resources_with_no_ref(self, resource_tbl):
        no_ref_list = []
        for resource_id in self.installed_resources(resource_tbl):
            resource_record = self.get_resource_record(resource_tbl, resource_id, False)
            if len(resource_record['ref-by']) == 0:
                no_ref_list.append(resource_id)
        return no_ref_list

    def add_entry(self, resource_tbl, resource_id, resource_record, lookup_db = None):

        pipe = resource_record['target']['pipe']
        action_func = resource_record['action_func']

        resource_params = RandomDict()

        bf_log.debug_fn('Adding entry to %s table with action %s' %
                            (resource_tbl.get_name(), action_func))

        for resource_name,ids in resource_record['resources'].items():
            tbl = self.get_resource_tbl_by_name(resource_name)
            rparams = []
            for ref_rsr_id in ids:
                # If the ref_rsr_id is not yet installed, add it first
                if self.is_id_installed(tbl, pipe, ref_rsr_id) is False:
                    self.add_resource_entry(tbl, action_func, pipe, directadd=True,
                            resource_id = ref_rsr_id, lookup_db = lookup_db)

                ref_rsr_data = self.get_rdata(ref_rsr_id)
                rparams.append(tbl.get_spec(ref_rsr_data, action_func))
            if len(rparams) == 1:
                resource_params[resource_name] = rparams[0]
            else:
                resource_params[resource_name] = rparams

        rdata = self.get_rdata(resource_id)
        replay = False
        pre_ha_rdata = None
        if lookup_db:
            pre_ha_rdata = self.get_rdata(resource_id, lookup_db=lookup_db)
            replay = True

        bf_log.debug_fn('%s entry add params:' % (resource_tbl.get_name()))
        bf_log.debug_fn(resource_params)
        resource_tbl.ent_add(rdata, action_func= action_func,
                             target=resource_record['target'], params = resource_params,
                             replay = replay, pre_ha_rdata = pre_ha_rdata, with_hitless_ha=self.do_hitless_ha)

        if 'is_default_entry' in rdata:
            self.default_entries[pipe] = resource_id

        d_str = 'Added ' + str(resource_id) + '\n\t\tRecord ' + str(resource_record['resources'])
        d_str += '\n\t\tRdata' + str(rdata)
        bf_log.debug_fn(d_str)

        rids_touched = set([resource_id])
        all_ids = [item for sublist in resource_record['resources'].values() for item in sublist]
        rids_touched |= set(all_ids)

        if len(bf_log.debug_res_ids & rids_touched) != 0:
            bf_log.log(d_str)

        for ref_rsr_id in all_ids:
            # Increment the reference pertaining to all ids used
            self.add_resource_reference(ref_rsr_id, resource_tbl.get_name(), resource_id)
        self.push_resource_record(resource_id, resource_record)
        self.push_installed_id(resource_tbl, action_func, pipe, resource_id)
        self.pop_available_id(resource_tbl, pipe, resource_id)

    def add_resource_entry(self, resource_tbl, action_func, pipe, directadd=False,
            resource_id = None, lookup_db = None):
        # Figure out the resources needed to add an entry to this resource
        # resources_needed is a list of table types from which a resource is needed
        if directadd == False and resource_tbl.get_ref_type() == 'DIRECT':
            return

        if lookup_db:
            resource_record = self.get_resource_record(resource_tbl, resource_id, lookup_db=lookup_db)
        else:
            resource_record = RandomDict()
            target = {
                      'sess_hdl' : self.sess_hdl,
                      'v_dev_id' : self.v_dev_id,
                      'p_dev_id' : self.p_dev_id,
                      'pipe' : pipe
                     }
            resource_record['target'] = target
            resource_record['resources'] = RandomDict()
            resource_record['action_func'] = action_func
            resource_record['ref-by'] = RandomDict()

            resources_needed = resource_tbl.get_resources_needed(action_func)

            if resource_id is None:
                resource_id = self.get_random_available_id(resource_tbl, pipe)[0]
            rdata = self.get_rdata(resource_id)

            for resource_name, count in resources_needed:
                tbl = self.get_resource_tbl_by_name(resource_name)
                ids = []
                rparams = []
                if tbl.get_ref_type() == 'DIRECT':
                    ids = self.get_random_available_id(tbl, pipe, count)
                else:
                    count = min(len(self.resource_records[tbl.name]['installed'][(pipe, action_func)]), count)
                    ids = self.get_random_installed_ids(tbl, action_func, pipe, count)
                resource_record['resources'][resource_name] = ids

        self.add_entry(resource_tbl, resource_id, resource_record)

        return resource_id

    def remove_all_resource_ref(self, resource_record, resource_tbl_name, resource_id):
        for ref_rsr_name, ref_rsr_ids in resource_record['resources'].items():
            for ref_rsr_id in ref_rsr_ids:
                ref_rsr_tbl = self.get_resource_tbl_by_name(ref_rsr_name)
                self.remove_resource_reference(ref_rsr_tbl, ref_rsr_id, resource_tbl_name, resource_id)

    def remove_resource_entry(self, resource_tbl, resource_id):
        bf_log.debug_fn('Removing entry ' + str(resource_id) + ' from ' +  resource_tbl.get_name())
        resource_record = self.get_resource_record(resource_tbl, resource_id)
        action_func = resource_record['action_func']
        assert len(resource_record['ref-by']) == 0

        # In each of the referenced resources, decrement the ref-count
        self.remove_all_resource_ref(resource_record, resource_tbl.get_name(), resource_id)

        rdata = self.get_rdata(resource_id)

        pipe = resource_record['target']['pipe']

        if 'is_default_entry' in rdata:
            assert resource_id == self.default_entries[pipe]
            del self.default_entries[pipe]

        resource_tbl.ent_del(rdata, target=resource_record['target'])

        # Move the resource-id into the created dictionary
        action_func = resource_record['action_func']

        d_str = 'Removed ' + str(resource_id) + '\n\t\tRecord ' + str(resource_record['resources'])
        d_str += '\n\t\tRdata' + str(rdata)
        bf_log.debug_fn(d_str)

        rids_touched = set([resource_id])
        all_ids = [item for sublist in resource_record['resources'].values() for item in sublist]
        rids_touched |= set(all_ids)

        if len(bf_log.debug_res_ids & rids_touched) != 0:
            bf_log.log(d_str)

        # Remove from resource records
        self.pop_resource_record(resource_id)
        self.push_available_id(resource_tbl, pipe, resource_id)
        self.pop_installed_id(resource_tbl, action_func, pipe, resource_id)


    def modify_resource_entry(self, resource_tbl, resource_id):
        bf_log.debug_fn('Modifying entry ' + str(resource_id) + ' from ' +  resource_tbl.get_name())
        resource_record = self.get_resource_record(resource_tbl, resource_id)
        target = resource_record['target']
        pipe = resource_record['target']['pipe']

        if resource_tbl == self.match_tbl:
            # Only match tbl can have different action function
            act_fns = copy.deepcopy(self.action_funcs)
            while len(act_fns) > 0:
                action_func = random.choice(act_fns)
                resources_needed = resource_tbl.get_resources_needed(action_func)
                empty = False
                for resource_name, count in resources_needed:
                    tbl = self.get_resource_tbl_by_name(resource_name)
                    if tbl.get_ref_type() == 'INDIRECT' and len(self.resource_records[tbl.name]['installed'][(pipe, action_func)]) == 0:
                        empty = True
                        act_fns.remove(action_func)
                        break
                if not empty:
                    break
            if len(act_fns) == 0:
                bf_log.debug_fn('No available resources to modify entry ' + str(resource_id) + ' from ' + resource_tbl.get_name())
                del act_fns
                return
            del act_fns
            self.pop_installed_id(resource_tbl, resource_record['action_func'], pipe, resource_id)
            resource_record['action_func'] = action_func
            self.push_installed_id(resource_tbl, action_func, pipe, resource_id)
        else:
            action_func = resource_record['action_func']


        rdata = self.get_rdata(resource_id)
        if resource_tbl.support_spec_change() is True:
            # Pick a random created resource id and
            # swap the specs by calling tbl APIs
            swap_resource_id = self.get_random_available_id(resource_tbl, pipe)[0]
            self.swap_uniq_vals(resource_id, swap_resource_id)

        # In each of the referenced resources, decrement the ref-count
        self.remove_all_resource_ref(resource_record, resource_tbl.get_name(), resource_id)

        old_resource_record = resource_record['resources']

        resource_record['resources'] = RandomDict()

        # Resources needed for the new action-func
        resource_params = RandomDict()
        if resource_tbl != self.match_tbl:
            # Use the previously found resource if it's a match table
            resources_needed = resource_tbl.get_resources_needed(action_func)

        for resource_name, count in resources_needed:
            tbl = self.get_resource_tbl_by_name(resource_name)
            ids = []
            rparams = []
            if tbl.get_ref_type() == 'DIRECT':
                for _ in range(count):
#                ref_rsr_id = self.add_resource_entry(tbl, action_func, directadd=True)
                    # TODO When count > 1 and direct referenced, the below lines need fixup
                    ref_rsr_id = self.add_resource_entry(tbl, action_func, pipe, directadd=True)
                    old_rsr_id = old_resource_record[resource_name][0]
                    tbl.swap_states(self.get_rdata(ref_rsr_id), self.get_rdata(old_rsr_id))
                    self.remove_resource_entry(tbl, old_rsr_id)
                    ids.append(ref_rsr_id)
            else:
                count = min(len(self.resource_records[tbl.name]['installed'][(pipe, action_func)]), count)
                ids = self.get_random_installed_ids(tbl, action_func, pipe, count)

            for ref_rsr_id in ids:
                # Increment the reference pertaining to this resource
                self.add_resource_reference(ref_rsr_id, resource_tbl.get_name(), resource_id)
                ref_rsr_data = self.get_rdata(ref_rsr_id)
                rparams.append(tbl.get_spec(ref_rsr_data, action_func))

            if count == 1:
                resource_params[resource_name] = rparams[0]
#                resource_record['resources'][resource_name] = resource_record[0]
            else:
                resource_params[resource_name] = rparams
            resource_record['resources'][resource_name] = ids

        bf_log.debug_fn('%s entry modify params:' % (resource_tbl.get_name()))
        bf_log.debug_fn(resource_params)

        resource_tbl.ent_modify(rdata, action_func= action_func,
                                target=target, params = resource_params, with_hitless_ha=self.do_hitless_ha)

        d_str = 'Modify ' + str(resource_id) + '\n\t\tOld Record ' + str(old_resource_record)
        d_str += '\n\t\tNew Record ' + str(resource_record['resources'])
        d_str += '\n\t\tRdata' + str(rdata)

        rids_touched = set([resource_id])
        all_ids = [item for sublist in old_resource_record.values() for item in sublist]
        rids_touched |= set(all_ids)
        all_ids = [item for sublist in resource_record['resources'].values() for item in sublist]
        rids_touched |= set(all_ids)

        if len(bf_log.debug_res_ids & rids_touched) != 0:
            bf_log.log(d_str)

    def config_scale_test(self):
        # Add entries in each of the resource tables and gather their
        # result to add into the match tables

        # The tables in self.tbls should be arranged based on their
        # dependency. i.e. The first tables should not have any dependencies

        for resource in self.tbls:
            for pipe in self.valid_pipes:
                count_entries_to_add = self.count_available_ids(resource, pipe)
                if self.do_hitless_ha and self.v_dev_id != self.p_dev_id:
                    # For virtual device hitless HA, do the test with a smaller number
                    # of entries owing to the time it takes to run.
                    count_entries_to_add = min(count_entries_to_add, 250)
                    # count_entries_to_add = min(count_entries_to_add, 20000)
                bf_log.log('Adding %d %s entries to pipe %s' % \
                        (count_entries_to_add, resource.get_name(), str(pipe)))
                for i in range(count_entries_to_add):
                    action_func = random.choice(resource.valid_action_funcs())
                    self.add_resource_entry(resource, action_func, pipe)
                    if (i+1) == count_entries_to_add:
                        self.flush_batch_or_txn(1)
                    elif ((i+1) % 1000) == 0:
                        self.flush_batch_or_txn(self.commit_prob)

        # Now add match entries

        self.added_entries = []
        for pipe in self.valid_pipes:
            count_entries = self.count_available_ids(self.match_tbl, pipe) * 0.80
            #count_entries = min(count_entries, 10000)
            if self.do_hitless_ha and self.v_dev_id != self.p_dev_id:
                count_entries = 10
            count_entries = int(count_entries)
            bf_log.log('Adding %d match entries to pipe %s ' % \
                    (count_entries, str(pipe)))
            for x in range(count_entries):
                action_func = random.choice(resource.valid_action_funcs())
                try:
                    match_id = self.add_resource_entry(self.match_tbl, action_func, pipe)
                except:
                    bf_log.log('Entry add failed count %d' % x)
#                raw_input('Entry add failed count %d' % x)
                    raise
                self.added_entries.append(match_id)
                self.append_modified_entry_list(match_id, (action_func, pipe))
                if (x+1) == count_entries:
                    self.flush_batch_or_txn(1)
                elif ((x+1) % 1000) == 0:
                    bf_log.log(str(x+1))
                    self.flush_batch_or_txn(self.commit_prob)

    def config_churn_flush(self, entries_removed_count, commit_prob):
        (did_flush, did_abort) = self.flush_batch_or_txn(commit_prob)
        if did_flush:
            if did_abort:
                for tbl_name in self.unflushed_tables:
                    entries_removed_count[tbl_name] = RandomDict()
            self.unflushed_tables = []

    def config_churn_test(self):

        for tbl in reversed(self.tbls + [self.match_tbl]):
            entries_with_no_ref = self.get_resources_with_no_ref(tbl)
            bf_log.log('Entries with no ref %d %s entries' % (len(entries_with_no_ref), tbl.get_name()))

        # Remove entries
        self.refresh_thrift()
        self.unflushed_tables = []
        entries_removed_count = RandomDict()
        commit_match_tbl = 0
        if random.random() < self.commit_prob:
            commit_match_tbl = 1
        for tbl in reversed(self.tbls + [self.match_tbl]):
            entries_removed_count[tbl.get_name()] = RandomDict()
            entries_with_no_ref = self.get_resources_with_no_ref(tbl)
            installed_entries = self.installed_resources(tbl)
            total_entries_installed = len(installed_entries)

            count_entries_to_remove = min(len(entries_with_no_ref), int(total_entries_installed * self.remove_scaling_factor))
            entries_to_remove = random.sample(entries_with_no_ref, count_entries_to_remove)

            bf_log.log('Removing %d %s entries' % (count_entries_to_remove, tbl.get_name()))

            for entry in entries_to_remove:
                pipe = self.get_resource_record(tbl, entry)['target']['pipe']
                self.remove_resource_entry(tbl, entry)
                entries_removed_count[tbl.get_name()].setdefault(pipe, 0)
                entries_removed_count[tbl.get_name()][pipe] += 1

            self.unflushed_tables += [tbl.get_name()]
            # Direct resources must commit/abort the same way as the match table
            if tbl.get_ref_type() == 'DIRECT' or tbl == self.match_tbl:
                self.config_churn_flush(entries_removed_count, commit_match_tbl)
            else:
                self.config_churn_flush(entries_removed_count, self.commit_prob)

        # Modify entries
        self.refresh_thrift()
        commit_match_tbl = 0
        if random.random() < self.commit_prob:
            commit_match_tbl = 1
        for tbl in self.tbls + [self.match_tbl]:
            # Entry removes aborted for this table, no spare handles to perform modify
            if len(entries_removed_count[tbl.get_name()]) == 0:
                continue

            if tbl.supports_modification() == False:
                continue
            installed_entries = self.installed_resources(tbl)
            count_entries_to_modify = len(installed_entries)//2
            entries_to_modify = random.sample(installed_entries, count_entries_to_modify)
            bf_log.log('Modifying %d %s entries' % (count_entries_to_modify, tbl.get_name()))
            for entry in entries_to_modify:
                self.modify_resource_entry(tbl, entry)
                match_entries = self.get_match_entries_for_resource(tbl, entry)
                # Create a set of modified match entries
                for match_entry in match_entries:
                    pipe = self.get_resource_record(self.match_tbl, match_entry)['target']['pipe']
                    action_func = self.get_action_func_for_match_entry(match_entry)
                    self.append_modified_entry_list(match_entry, (action_func, pipe))
            if tbl.get_ref_type() == 'DIRECT' or tbl == self.match_tbl:
                self.config_churn_flush(entries_removed_count, commit_match_tbl)
            else:
                self.config_churn_flush(entries_removed_count, self.commit_prob)

        # Re-add entries
        self.refresh_thrift()
        self.added_entries = []
        for tbl in self.tbls + [self.match_tbl]:
            for pipe, count_entries_to_readd in entries_removed_count[tbl.get_name()].items():
                # Don't readd if the remove was aborted
                if len(entries_removed_count[tbl.get_name()]) == 0:
                    continue
                bf_log.log('Re-adding %d %s entries to pipe %s' % (count_entries_to_readd, tbl.get_name(), str(pipe)))
                for _ in range(count_entries_to_readd):
                    action_func = random.choice(tbl.valid_action_funcs())
                    resource_id = self.add_resource_entry(tbl, action_func, pipe)
                    if tbl == self.match_tbl:
                        self.append_modified_entry_list(resource_id, (action_func, pipe))
                        self.added_entries.append(resource_id)
                self.config_churn_flush(entries_removed_count, self.commit_prob)

    def config_ha_replay(self, pre_ha_db, match_ids):
        bf_log.log("Replaying %d match entries" % len(match_ids))
        for match_id in match_ids:
            #bf_log.log("Replaying " + str(match_id))
            resource_record = self.get_resource_record(self.match_tbl, match_id, lookup_db=pre_ha_db)
            self.add_entry(self.match_tbl, match_id, resource_record, lookup_db=pre_ha_db)
            self.append_modified_entry_list(match_id,
                    (resource_record['action_func'], resource_record['target']['pipe']))

    def append_modified_entry_list(self, match_id, key):
        modified_entry_list = self.modified_entries.setdefault(key, [])
        modified_entry_list.append(match_id)

    def clear_modified_entry_list(self):
        self.modified_entries = RandomDict()

    def verify_match_id(self, match_id):
        mrdata = self.get_rdata(match_id)

        bf_log.debug_fn('Verifying match id ' + str(match_id))

        try:
            mat_ent_hdl = self.match_tbl.id_to_ent_hdl(mrdata)
        except:
            raise
        resource_record = self.get_resource_record_for_match_entry(match_id)
        mdata = dict()
        mdata['p_dev_id'] = resource_record['target']['p_dev_id']
        mdata['v_dev_id'] = resource_record['target']['v_dev_id']
        pipe = resource_record['target']['pipe']
        mdata['pipe'] = pipe
        mdata['sess_hdl'] = resource_record['target']['sess_hdl']
        action_func = resource_record['action_func']
        mdata['action_func'] = action_func
        mdata['match_entry_hdl'] = mat_ent_hdl

        if pipe == 'all':
            valid_ports = swports
        else:
            valid_ports = [port for port in swports if port_to_pipe(port) == pipe]
        send_port = random.choice(valid_ports)
        mdata['ingress_pipe'] = port_to_pipe(send_port)
        # exp_pkt_params = self.default_pkt_params()

        d_str = 'Verifying ' + str(match_id)
        d_str += '\n\t\t Resource ' + str(resource_record['resources'])

        rids_touched = set([match_id])
        all_ids = [item for sublist in resource_record['resources'].values() for item in sublist]
        rids_touched |= set(all_ids)

        if len(bf_log.debug_res_ids & rids_touched) != 0:
            bf_log.log(d_str)

        if 'is_default_entry' in mrdata:
            bf_log.log('VERIFYING DEFAULT ENTRY')
        else:
            try:
                # Do an idletime verify
                if self.match_tbl.has_idle_time:
                    self.match_tbl.verify_idletime(mrdata, mdata)
            except:
                bf_log.log('Idletime verify failed')
                bf_log.log('Match entry 0x%x %s' % (mat_ent_hdl, str(match_id)))
                bf_log.log('Match entry data %s' % (str(mrdata)))
                bf_log.log('Resource record %s' % (str(resource_record)))
                if bf_log.debug_mode == False:
                    raise
                bf_log.debug_trace_enable = True
                pdb.set_trace()

        exp_ports = set()
        pkt_count = 1
        # Figure out the expected pkt and the port
        for resource_name, rlist in resource_record['resources'].items():
            for resource_id in rlist:
                tbl = self.get_resource_tbl_by_name(resource_name)
                rdata = self.get_rdata(resource_id)
                _exp_port, _pkt_count = tbl.update_exp_port(rdata, action_func)
                if _exp_port is not None:
                    exp_ports.add(_exp_port)
                if _pkt_count is not None:
                    pkt_count = max(pkt_count, _pkt_count)
                # exp_port, pkt_count = resource.update_exp_pkt_params(resource_id, exp_pkt_params)

        for x in range(pkt_count):
            pkt_params = self.default_pkt_params()
            # Figure out the pkt params needed for this particular entry
            self.match_tbl.update_pkt_params(mrdata, mdata,
                    pkt_params)

            pkt_to_send = self.create_pkt(pkt_params)
            try:
                mdata['pkt_count'] = 1

                self.send_packet(send_port, pkt_to_send)
                rcv_pkt = self.receive_packet(exp_ports)
                # Verify that the received pkt matches all the required parameters

                if self.match_tbl.has_idle_time:
                    self.match_tbl.update_idle_time_param(mrdata)

                for resource_name, rlist in resource_record['resources'].items():
                    test_pass = False
                    # Try verify in all the resources except first one
                    # even if there's a failure, continue
                    tbl = self.get_resource_tbl_by_name(resource_name)
                    for resource_id in rlist[1:]:
                        try:
                            rdata = self.get_rdata(resource_id)
                            tbl.verify_pkt(rdata, rcv_pkt, mdata, strict=False)
                            test_pass = True
                            break
                        except:
                            pass

                    # If test is still failing, check the last
                    if test_pass == False:
                        resource_id = rlist[0]
                        tbl = self.get_resource_tbl_by_name(resource_name)
                        rdata = self.get_rdata(resource_id)
                        tbl.verify_pkt(rdata, rcv_pkt, mdata)

            except:
                bf_log.log('Verify failed')
                bf_log.log('Match entry 0x%x %s' % (mat_ent_hdl, str(match_id)))
                bf_log.log('Match entry data %s' % (str(mrdata)))
                bf_log.log('Resource record %s' % (str(resource_record)))
                if bf_log.debug_mode == False:
                    raise

                bf_log.debug_trace_enable = True
                pdb.set_trace()
                while True:
                    raw_input('Enter to send pkt')
                    try:
                        self.send_packet(send_port, pkt_to_send)
                        rcv_pkt = self.receive_packet(exp_ports)
#Verify that the received pkt matches all the required parameters
                        for resource_name, rlist in resource_record['resources'].items():
                            tbl = self.get_resource_tbl_by_name(resource_name)
                            for resource_id in rlist:
                                try:
                                    rdata = self.get_rdata(resource_id)
                                    tbl.verify_pkt(rdata, rcv_pkt, mdata)
                                except:
                                    pass
                    except:
                        pass

    def verify(self):
        ''' Verification function. All the entries that have been modified
            should be in a modification list and the verify function
            will verify all those entries in all the associated tables
        '''
        # Now send packets for a handful of entries
        for key, modified_entries in self.modified_entries.items():
            verify_count = min(len(modified_entries), 100)
#            verify_count = len(modified_entries)
            bf_log.log('Verify_count for ' + str(key) + ' ' + str(verify_count))
            for match_id in random.sample(modified_entries, verify_count):
                self.verify_match_id(match_id)


        for pipe, default_match_id in self.default_entries.items():
            bf_log.log('Verifying default entry for pipe ' + str(pipe) + str(default_match_id))
            self.verify_match_id(default_match_id)

        self.clear_modified_entry_list()

    def default_pkt_params(self):
        pkt_params = RandomDict()
        pkt_params['with_tcp_chksum'] = False
        pkt_params['ip_id'] = random.randint(0, 65535)
        return pkt_params

    def create_pkt(self, pkt_params):
        pkt = simple_tcp_packet(**pkt_params)
        return pkt

    def send_packet(self, port, pkt):
        send_packet(self.tiface, port, pkt)

    def receive_packet(self, ports):
        timeout = 10
        if len(ports) > 1:
            timeout = 0
        for port in ports:
            (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.tiface.dataplane.poll(port_number=port, device_number=self.p_dev_id, timeout=timeout)
            if rcv_pkt != None:
                break
        if len(ports) > 1:
            count = 0
            while rcv_pkt == None and count < 20:
                count += 1
                time.sleep(0.5)
                timeout = 0
                for port in ports:
                    (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.tiface.dataplane.poll(port_number=port, device_number=self.p_dev_id, timeout=timeout)
                    if rcv_pkt != None:
                        break
        assert rcv_pkt != None, "No packet received"
        return rcv_pkt

    def begin_txn(self):
        self.tiface.conn_mgr.begin_txn(self.sess_hdl, False)

        self.backup_resource_records = RandomDict()
        for resource_tbl in self.tbls + [self.match_tbl]:
            self.backup_resource_records[resource_tbl.name] = RandomDict()
            self.backup_resource_records[resource_tbl.name]['records'] = RandomDict()
            self.backup_resource_records[resource_tbl.name]['available_ids'] = OrderedDict()
            self.backup_resource_records[resource_tbl.name]['installed'] = OrderedDict()

        self.backup_modified_entries = copy.deepcopy(self.modified_entries)
        self.backup_added_entries = copy.deepcopy(self.added_entries)
        self.backup_default_entries = copy.deepcopy(self.default_entries)
        self.backup_mat_default_entry_hdl = copy.deepcopy(self.match_tbl.default_entry_hdl)

    def commit_txn(self):
        # print("COMMITTING")
        del self.backup_resource_records
        del self.backup_modified_entries
        del self.backup_added_entries
        del self.backup_default_entries
        del self.backup_mat_default_entry_hdl
        self.unflushed_tables = []

        self.tiface.conn_mgr.commit_txn(self.sess_hdl, True)

        if self.v_dev_id != self.p_dev_id:
            if 'program_all_updates' in self.match_tbl.fn_map and self.match_tbl.fn_map['program_all_updates']:
                self.match_tbl.fn_map['program_all_updates'](self.sess_hdl, self.v_dev_id, self.p_dev_id)

        self.tiface.conn_mgr.complete_operations(self.sess_hdl)

    def abort_txn(self):
        # print("ABORTING")
        self.tiface.conn_mgr.abort_txn(self.sess_hdl)
        self.tiface.conn_mgr.complete_operations(self.sess_hdl)

        del self.modified_entries
        del self.added_entries
        del self.default_entries
        del self.match_tbl.default_entry_hdl

        for resource_tbl in self.tbls + [self.match_tbl]:
            for res_id, res_record in self.backup_resource_records[resource_tbl.name]['records'].items():
                self.resource_records[resource_tbl.name]['records'][res_id] = res_record
            for key, rid_dict in self.backup_resource_records[resource_tbl.name]['available_ids'].items():
                for rid, exists in rid_dict.items():
                    if exists:
                        self.resource_records[resource_tbl.name]['available_ids'][key][rid] = None
                    else:
                        self.resource_records[resource_tbl.name]['available_ids'][key].pop(rid, None)
            for key, rid_dict in self.backup_resource_records[resource_tbl.name]['installed'].items():
                for rid, exists in rid_dict.items():
                    if exists:
                        self.resource_records[resource_tbl.name]['installed'][key][rid] = None
                    else:
                        self.resource_records[resource_tbl.name]['installed'][key].pop(rid, None)

        self.modified_entries = self.backup_modified_entries
        self.added_entries = self.backup_added_entries
        self.default_entries = self.backup_default_entries
        self.match_tbl.default_entry_hdl = self.backup_mat_default_entry_hdl

    def begin_batch_or_txn(self):
        is_txn = random.random()
        if is_txn < self.txn_prob:
            self.begin_txn()
            self.batch_mode = 2
        else:
            batch_or_txn = random.randint(0, 1)
            if batch_or_txn == 1:
                self.tiface.conn_mgr.begin_batch(self.sess_hdl)
            self.batch_mode = batch_or_txn

    def flush_batch_or_txn(self, commit_prob):
        did_abort = False
        if commit_prob == 1 or commit_prob == 0:
            to_flush = 1
        else:
            to_flush = random.randint(0,1)
        if to_flush == 1:
            if self.batch_mode == 1:
                self.tiface.conn_mgr.flush_batch(self.sess_hdl)
            elif self.batch_mode == 2:
                to_commit = random.random()
                if to_commit < commit_prob:
                    self.commit_txn()
                else:
                    self.abort_txn()
                    did_abort = True
                self.begin_txn()

        return (to_flush == 1, did_abort)

    def end_batch_or_txn(self):
        if self.batch_mode == 1:
            self.tiface.conn_mgr.end_batch(self.sess_hdl, True)
        elif self.batch_mode == 2:
            self.commit_txn()
        else:
            self.tiface.conn_mgr.complete_operations(self.sess_hdl)
        self.batch_mode = 0

    def run_scale_test(self):
        bf_log.log('Running scale test')
        self.refresh_thrift()
        self.begin_batch_or_txn()
        self.config_scale_test()
        bf_log.log('Configuration completed')
        self.end_batch_or_txn()
        self.update_match_entry_add_time()
        bf_log.log('Verifying entries')
        self.verify()
        bf_log.log('Verify completed')

    def run_churn_test(self):
        bf_log.log('Running churn test')
        # Batching cannot be done for idle tables, as they can cause inaccuracies
        # for entries that have been moved between logical tables.
        if not self.match_tbl.has_idle_time:
            self.begin_batch_or_txn()
        self.config_churn_test()
        bf_log.log('Churn config completed')
        self.end_batch_or_txn()
        self.update_match_entry_add_time()
        bf_log.log('Verifying entries')
        self.verify()
        bf_log.log('Verify completed')

    def clear_all_entries(self):
        bf_log.log('Clearing all entries')
        self.refresh_thrift()
        self.begin_batch_or_txn()
        for tbl in reversed(self.tbls + [self.match_tbl]):
            installed_entries = self.installed_resources(tbl)
            bf_log.log('Removing %d %s entries' % (len(installed_entries), tbl.get_name()))

            for entry in installed_entries:
                self.remove_resource_entry(tbl, entry)
            self.flush_batch_or_txn(1)
        self.end_batch_or_txn()

    def set_symmetric(self, val):
        bf_log.log('Setting symmetry to ' + str(val))
        for tbl in [self.match_tbl]:
            tbl.set_symmetric(self.sess_hdl, self.v_dev_id, val)
            if self.p_dev_id != self.v_dev_id:
                tbl.set_symmetric(self.sess_hdl, self.p_dev_id, val)

        if val == 1:
            self.valid_pipes = ['all']
        else:
            self.valid_pipes = self.pipe_list
            # Due to a bug in thrift 0.14.0 and later the server will disconnect
            # the client after 100MB of certain data fields.  When virtual
            # devices are used it greatly increases the messages between our
            # test client and server.
            # To work around this we will reduce the number of pipes tested.
            if self.v_dev_id != self.p_dev_id: # virtual device
                if len(self.valid_pipes) > 2:
                    self.valid_pipes = random.sample(self.pipe_list, 2)
                    bf_log.log('Reducing from all pipes, %s, to %s' % (self.pipe_list, self.valid_pipes))

        self.init_all_tables()

    def update_match_entry_add_time(self):
        if self.match_tbl.has_idle_time:
            time_added = datetime.datetime.now()
            for entry in self.added_entries:
                rdata = self.get_rdata(entry)
                self.match_tbl.update_entry_add_time(rdata, time_added)

    def clear_stats_state(self, stats_tbl):
        # Clear the count for each
        stats_rids = self.resource_records['Stats']['records'].keys()
        for each_rid in stats_rids:
            rdata = self.get_rdata(each_rid)
            stats_tbl.clear_count(rdata)

    def clear_stful_state(self, stful_tbl):
        # Clear the count for each
        stful_rids = self.resource_records['Stateful']['records'].keys()
        for each_rid in stful_rids:
            for each in self.resource_records['Stateful']['installed'].keys():
                if each_rid in self.resource_records['Stateful']['installed'][each].keys():
                    pipe = each[0]
                    rdata = self.get_rdata(each_rid)
                    stful_tbl.clear_stful(rdata, pipe)

    def add_stful_at_llp(self, stful_tbl):
        stful_rids = self.installed_resources(stful_tbl)
        target = {
                  'sess_hdl' : self.sess_hdl,
                  'v_dev_id' : self.v_dev_id,
                  'p_dev_id' : self.p_dev_id,
                  'pipe' : 'all'
                 }
        for each_rid in stful_rids:
            rdata = self.get_rdata(each_rid)
            stful_tbl.ent_add(rdata, target=target)

    def replay_move_list_to_llp(self, p_dev_id, adt_move_list, sel_move_list, mat_move_list):
        action_tbl = self.get_resource_tbl_by_name('Action')
        selector_tbl = self.get_resource_tbl_by_name('Selector')
        if action_tbl and action_tbl.get_ref_type() == 'INDIRECT':
            action_tbl.replay_move_list_to_llp(self.sess_hdl, p_dev_id, adt_move_list)
        if selector_tbl:
            selector_tbl.replay_move_list_to_llp(self.sess_hdl, p_dev_id, sel_move_list)
        self.match_tbl.replay_move_list_to_llp(self.sess_hdl, p_dev_id, mat_move_list)

    def run_hitless_ha_test(self, skip_churn):
        self.end_batch_or_txn()
        bf_log.log('Running hitless HA test')
        self.refresh_thrift()
        unmodified_entries = RandomDict()
        deleted_entries = RandomDict()
        modified_entries = RandomDict()
        added_entries = RandomDict()

        installed_entries = self.installed_resources(self.match_tbl)
        c_i = len(installed_entries)
        c_u = int(c_i*0.9)
        c_d = int(c_i*0.05)

        # Slice the installed entries into different flows
        unmodified_entries = installed_entries[:c_u]
        unmodified_entries = installed_entries
        deleted_entries = installed_entries[c_u:c_d]
        modified_entries = installed_entries[c_d:]

        pre_ha_db= self.resource_records

        self.tiface.devport_mgr.devport_mgr_warm_init_begin(self.v_dev_id, dev_init_mode.DEV_WARM_INIT_HITLESS, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, False)

        if self.v_dev_id != self.p_dev_id:
            # Enable LLP callbacks during restore
            self.match_tbl.register_cbs_for_hitless_ha(self.sess_hdl, self.p_dev_id)
            self.tiface.devport_mgr.devport_mgr_warm_init_begin(self.p_dev_id, dev_init_mode.DEV_WARM_INIT_HITLESS, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, False)
            self.match_tbl.register_cbs_for_hitless_ha(self.sess_hdl, self.p_dev_id)
            action_tbl = self.get_resource_tbl_by_name('Action')
            action_tbl.register_cbs_for_hitless_ha(self.sess_hdl, self.p_dev_id)
            if self.match_tbl.has_selector:
                selector_tbl = self.get_resource_tbl_by_name('Selector')
                selector_tbl.register_cbs_for_hitless_ha(self.sess_hdl, self.p_dev_id)

        bf_log.log('Hitless warm init begin')

        # TODO Hack to make idletime work during hitless HA. Remove
        if self.match_tbl.has_idle_time:
            self.match_tbl.idle_tmo_enabled = False


        # No config replay for virtual device scenario
        if self.v_dev_id == self.p_dev_id:
            self.default_init()
            self.config_ha_replay(pre_ha_db, unmodified_entries)
            bf_log.log('Hitless warm init old config replay completed. Adding new entries')
        else:
            # If match table has an idle table, enable time out before virtual device state restore
            if self.match_tbl.has_idle_time:
                self.match_tbl.idle_timeout_enable(self.sess_hdl, self.p_dev_id, self.v_dev_id)
            # After hitless HA, stateful/stats all are initialized to zero
            stats_tbl = self.get_resource_tbl_by_name('Stats')
            if stats_tbl:
                self.clear_stats_state(stats_tbl)
            stful_tbl = self.get_resource_tbl_by_name('Stateful')
            if stful_tbl:
                if stful_tbl.get_ref_type() == 'DIRECT':
                    self.clear_stful_state(stful_tbl)
        #self.config_scale_test()

        if self.v_dev_id != self.p_dev_id:
            self.tiface.devport_mgr.devport_mgr_warm_init_end(self.p_dev_id)
            # Get updates from LLP restore and funnel it to HLP
            updates = self.match_tbl.get_updates_after_hitless_ha(self.sess_hdl, self.p_dev_id)
            (adt_updates, sel_updates, mat_updates) = self.process_updates_after_hitless_ha(updates)
            new_updates = adt_updates + sel_updates + mat_updates
            action_tbl = self.get_resource_tbl_by_name('Action')
            for resource_id in self.installed_resources(self.match_tbl):
                action_func = self.get_action_func_for_match_entry(resource_id)
                self.append_modified_entry_list(resource_id, (action_func, 'all'))

        self.tiface.devport_mgr.devport_mgr_warm_init_end(self.v_dev_id)

        if self.v_dev_id != self.p_dev_id:
            self.tiface.devport_mgr.devport_mgr_warm_init_begin(self.p_dev_id, dev_init_mode.DEV_WARM_INIT_FAST_RECFG, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, False)
            if self.match_tbl.has_idle_time:
                self.match_tbl.idle_timeout_enable(self.sess_hdl, self.p_dev_id, self.p_dev_id)
            stful_tbl = self.get_resource_tbl_by_name('Stateful')
            if stful_tbl and stful_tbl.get_ref_type() == 'INDIRECT':
                self.add_stful_at_llp(stful_tbl)
            self.replay_move_list_to_llp(self.p_dev_id, adt_updates, sel_updates, mat_updates)
            self.tiface.devport_mgr.devport_mgr_warm_init_end(self.p_dev_id)

            self.enable_updates_after_hitless_ha()

            # Restore the virtual device state outside of warm-init begin & warm-init end
            self.match_tbl.restore_virtual_device_state(self.sess_hdl, self.v_dev_id, new_updates)

        bf_log.log('Hitless warm init ended ')
        self.tiface.conn_mgr.complete_operations(self.sess_hdl)
        # Wait until the internal session has completed
        # For now directly using internal session hdl 0. If it changes in future
        # need an API to figure out the internal session or such
        self.tiface.conn_mgr.complete_operations(0)
        bf_log.log('Complete operations completed')

        # The entries got pushed to hardware just now
        self.added_entries = self.installed_resources(self.match_tbl)
        self.update_match_entry_add_time()

        bf_log.log('Verifying entries')
        self.verify()
        bf_log.log('Verify completed')
        # Run a churn test make sure things are working fine
        if skip_churn is False:
            self.run_churn_test()

    def run_fast_reconfig_test(self):

        # TODO Hack to make idletime work during fast reconfig. Remove
        if self.match_tbl.has_idle_time:
            self.teardown()

        bf_log.log('Running fast reconfig test')
        self.refresh_thrift()
        self.tiface.devport_mgr.devport_mgr_warm_init_begin(self.v_dev_id, dev_init_mode.DEV_WARM_INIT_FAST_RECFG, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, False)
        if self.v_dev_id != self.p_dev_id:
            self.tiface.devport_mgr.devport_mgr_warm_init_begin(self.p_dev_id, dev_init_mode.DEV_WARM_INIT_FAST_RECFG, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, False)
        bf_log.log('Fast reconfig warm init begin')
        self.default_init()
        self.config_scale_test()
        bf_log.log('Scale config completed')
        self.tiface.devport_mgr.devport_mgr_warm_init_end(self.v_dev_id)
        if self.v_dev_id != self.p_dev_id:
            self.tiface.devport_mgr.devport_mgr_warm_init_end(self.p_dev_id)
        bf_log.log('Fast reconfig warm init ended ')
        self.tiface.conn_mgr.complete_operations(self.sess_hdl)
        # Wait until the internal session has completed
        # For now directly using internal session hdl 0. If it changes in future
        # need an API to figure out the internal session or such
        self.tiface.conn_mgr.complete_operations(0)
        bf_log.log('Complete operations completed')

        # The entries got pushed to hardware just now
        self.added_entries = self.installed_resources(self.match_tbl)
        self.update_match_entry_add_time()

        bf_log.log('Verifying entries')
        self.verify()
        bf_log.log('Verify completed')

    def process_updates_after_hitless_ha(self, updates):
        adt_move_list = []
        sel_updates = []
        mat_updates = []
        adt_updates = []
        for each_update in updates:
            update_type = each_update.update_type
            if update_type == drivers_test_tbl_update_type.MAT_UPDATE_TYPE:
                mat_updates.append(each_update)
            elif update_type == drivers_test_tbl_update_type.ADT_UPDATE_TYPE:
                adt_move_list.append(each_update.update_data.adt)
                adt_updates.append(each_update)
            elif update_type == drivers_test_tbl_update_type.SEL_UPDATE_TYPE:
                sel_updates.append(each_update)
        action_tbl = self.get_resource_tbl_by_name('Action')
        self.fixup_action_entry_hdls(action_tbl, adt_updates)
        self.fixup_mat_entry_hdls(self.match_tbl, mat_updates, sel_updates)
        return (adt_updates, sel_updates, mat_updates)

    def update_selector_mbr_state_from_match(self, match_rdata, sel_grp_hdl, processed_list):
        # First, fixup the sel grp hdl
        assert len(match_rdata['resource_record']['resources']['Selector']) == 1
        sel_rid = match_rdata['resource_record']['resources']['Selector'][0]
        if sel_rid in processed_list:
            return sel_rid
        sel_rdata = self.get_rdata(sel_rid)
        sel_rdata['grp_hdl'] = sel_grp_hdl
        action_rids = sel_rdata['resource_record']['resources']['Action']
        # Next, replace mbr hdls
        sel_rdata['mbrs'] = []
        for each_mbr in action_rids:
            action_rdata = self.get_rdata(each_mbr)
            sel_rdata['mbrs'].append(action_rdata['mbr_hdl'])

        mbr_idx = 0
        for each_update in sel_rdata['sel_grp_update']:
            assert each_update.update_type == drivers_test_tbl_update_type.SEL_UPDATE_TYPE
            sel_update = each_update.update_data.sel
            if sel_update.update_type == drivers_test_sel_update_type.SEL_UPDATE_ADD:
                sel_update.update_params.add.entry_hdl = sel_rdata['mbrs'][mbr_idx]
                mbr_idx += 1
            else:
                assert sel_update.update_type == drivers_test_sel_update_type.SEL_UPDATE_GROUP_CREATE
        return sel_rid

    def update_selector_mbr_state(self, sel_rdata):
        action_rids = sel_rdata['resource_record']['resources']['Action']
        # Replace mbr hdls
        sel_rdata['mbrs'] = []
        for each_mbr in action_rids:
            action_rdata = self.get_rdata(each_mbr)
            sel_rdata['mbrs'].append(action_rdata['mbr_hdl'])

        mbr_idx = 0
        for each_update in sel_rdata['sel_grp_update']:
            assert each_update.update_type == drivers_test_tbl_update_type.SEL_UPDATE_TYPE
            sel_update = each_update.update_data.sel
            if sel_update.update_type == drivers_test_sel_update_type.SEL_UPDATE_ADD:
                sel_update.update_params.add.entry_hdl = sel_rdata['mbrs'][mbr_idx]
                mbr_idx += 1
            else:
                assert sel_update.update_type == drivers_test_sel_update_type.SEL_UPDATE_GROUP_CREATE

    def fixup_mat_entry_hdls(self, match_tbl, mat_updates, sel_updates):
        installed_rids = self.installed_resources(self.match_tbl)
        available_hdls = [x for x in range(1, match_tbl.get_size())]

        # Replace action entry handles in drv_data for all match entries
        action_tbl = self.get_resource_tbl_by_name('Action')
        if action_tbl.get_ref_type() == 'INDIRECT':
            for rid in self.installed_resources(self.match_tbl):
                rdata = self.get_rdata(rid)
                resource_record = rdata['resource_record']
                for resource_name, ids in resource_record['resources'].items():
                    if resource_name == 'Action':
                        action_rdata = self.get_rdata(ids[0])
                        rdata['drv_data'] = match_tbl.replace_adt_ent_hdl(rdata['drv_data'], action_rdata['mbr_hdl'])
                    if 'is_default_entry' in rdata:
                        rdata['default_entry_add'][0].update_data.mat.update_params.set_dflt.drv_data = rdata['drv_data']

        sel_installed_rids = []
        processed_sel_rids = []
        sel_available_hdls = []

        if match_tbl.has_selector:
            selector_tbl = self.get_resource_tbl_by_name('Selector')
            sel_installed_rids = self.installed_resources(selector_tbl)
            SEL_GRP_HDL_BASE = 0x12 << 24
            sel_available_hdls = [SEL_GRP_HDL_BASE + x for x in range(selector_tbl.get_size())]

        for each in mat_updates:
            mat_update = each.update_data.mat
            assert mat_update.update_type == drivers_test_mat_update_type.MAT_UPDATE_ADD
            entry_hdl = mat_update.update_params.add.entry_hdl
            data = mat_update.update_params.add.drv_data
            match_spec = match_tbl.get_mspec_from_plcmt_data(self.v_dev_id, data)
            resource_id = match_tbl.get_resource_id_from_mspec(match_spec)
            installed_rids.remove(resource_id)
            rdata = self.get_rdata(resource_id)
            rdata['entry_hdl'] = entry_hdl
            available_hdls.remove(entry_hdl)
            if self.match_tbl.has_idle_time:
                #Fixup TTL in the move-list node
                rdata['ttl'] = match_tbl.idle.gen_rand_ttl()
                rdata['drv_data'] = match_tbl.replace_ttl(rdata['drv_data'], rdata['ttl'])

            # Selector handling
            if match_tbl.has_selector:
                if mat_update.update_params.add.sel_grp_exists:
                    sel_grp_hdl = mat_update.update_params.add.sel_grp_hdl
                    if sel_grp_hdl in sel_available_hdls:
                        sel_available_hdls.remove(sel_grp_hdl)
                    # Fixup the mbr handles
                    sel_rid = self.update_selector_mbr_state_from_match(rdata, sel_grp_hdl, processed_sel_rids)
                    if sel_rid not in processed_sel_rids:
                        processed_sel_rids.append(sel_rid)
                        sel_installed_rids.remove(sel_rid)
                    rdata['drv_data'] = match_tbl.replace_sel_grp_hdl(rdata['drv_data'], sel_grp_hdl)
            each.update_data.mat.update_params.add.drv_data = rdata['drv_data']

        # Now, install (both at HLP and LLP) all the groups that were not referred to by any match entry
        for each in sel_installed_rids:
            sel_rdata = self.get_rdata(each)
            self.update_selector_mbr_state(sel_rdata)
            # Fix up grp hdl in the selector update
            for each_update in sel_rdata['sel_grp_update']:
                assert each_update.update_type == drivers_test_tbl_update_type.SEL_UPDATE_TYPE
                sel_update = each_update.update_data.sel
                if sel_update.update_type == drivers_test_sel_update_type.SEL_UPDATE_GROUP_CREATE:
                    sel_update.update_params.grp_create.group_hdl = sel_available_hdls[0]
                    curr_sel_grp_hdl = sel_available_hdls[0]
                    sel_available_hdls.remove(sel_available_hdls[0])
                    sel_rdata['grp_hdl'] = curr_sel_grp_hdl
                else:
                    assert sel_update.update_type == drivers_test_sel_update_type.SEL_UPDATE_ADD
                    sel_update.update_params.add.group_hdl = curr_sel_grp_hdl

            sel_updates.extend(sel_rdata['sel_grp_update'])
            selector_tbl.install_resource_in_llp(self.sess_hdl, sel_rdata, self.p_dev_id)

        assert len(installed_rids) == 0 or len(installed_rids) == 1

        if len(installed_rids) == 1:
            rdata = self.get_rdata(installed_rids[0])
            assert 'is_default_entry' in rdata
            rdata['default_entry_add'][0].update_data.mat.update_params.set_dflt.entry_hdl = available_hdls[0]
            rdata['entry_hdl'] = available_hdls[0]
            mat_updates += rdata['default_entry_add']
            match_tbl.install_resource_in_llp(self.sess_hdl, rdata, self.p_dev_id)

        # Replace selector group handle MAT drv data
        installed_rids = self.installed_resources(self.match_tbl)
        for each in installed_rids:
            rdata = self.get_rdata(each)
            rdata['drv_data']

    def fixup_action_entry_hdls(self, action_tbl, adt_updates):
        action_tbl = self.get_resource_tbl_by_name('Action')
        if action_tbl.ref_type == 'DIRECT':
            #Nothing to do
            return
        installed_rids = self.installed_resources(action_tbl)
        available_mbr_hdls = [x for x in range(1, action_tbl.get_size())]
        for each in adt_updates:
            adt_update = each.update_data.adt
            assert adt_update.update_type == drivers_test_adt_update_type.ADT_UPDATE_ADD
            mbr_hdl = adt_update.update_params.add.entry_hdl
            data = adt_update.update_params.add.drv_data
            (action_name, action_spec) = action_tbl.get_info_from_plcmt_data(self.v_dev_id, mbr_hdl, data)
            resource_id = action_tbl.get_resource_id_from_aspec(action_name, action_spec, installed_rids, self.get_rdata)
            installed_rids.remove(resource_id)
            rdata = self.get_rdata(resource_id)
            rdata['mbr_hdl'] = mbr_hdl
            available_mbr_hdls.remove(mbr_hdl)

        # Now install the remaining resource ids
        # What is remaining are the ones that were not referred to by any match entry
        # before HA
        for each in installed_rids:
            rdata = self.get_rdata(each)
            rdata['adt_move_list'][0].update_data.adt.update_params.add.entry_hdl = available_mbr_hdls[0]
            rdata['mbr_hdl'] = available_mbr_hdls[0]
            available_mbr_hdls.remove(available_mbr_hdls[0])
            adt_updates += rdata['adt_move_list']
            # Install this resource in LLP as well
            action_tbl.install_resource_in_llp(self.sess_hdl, rdata, self.p_dev_id)

class TestPipelineTable(pd_base_tests.EmptyTableTest):
    program = "drivers_test"
    table = "match_tbl"


@group("thorough")
class Test(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["drivers_test"])

    def reconnect_thrift(self):
        pd_base_tests.ThriftInterfaceDataPlane.tearDown(self)
        pd_base_tests.ThriftInterfaceDataPlane.setUp(self)

    def runTest(self):
        print()
        seed = test_param_get('test_seed')
        if seed == 'None':
            seed = random.randint(0, 65536)
        else:
            seed = int(seed)
        bf_log.log('Seed used %d' % seed)
        random.seed(seed)
        sess_hdl = self.conn_mgr.client_init()

         # The resource classes should be arranged such that if there is dependency,
         # then the dependent classes should come later
        resource_classes = [
                            ('Stats', StatsTable),
                            ('Meter', None),
                            ('Stateful', StatefulTable),
                            ('Action', ActionTable),
                            ('Selector', SelectorTable)
                            ]

        p_dev_id = 0
        v_dev_id = 0

        test_info_file = test_param_get('drivers_test_info')
        if test_info_file == 'None':
            test_info_file = 'drivers-test-info.json'
            if not os.path.isfile(test_info_file):
                test_info_file = os.path.join(os.path.dirname(__file__), test_info_file)
        with open(test_info_file) as test_file:
            resource_map = json.load(test_file)

        resource_tbls = []
        for key,resource_class in resource_classes:
            if key in resource_map:
                tbl = resource_class(self, resource_map, swports)
                resource_tbls.append(tbl)

        match_tbl = MatchTable(self, resource_map, swports)
        if resource_map['Virtual_device']:
            v_dev_id = 1

        # Hacks to keep sanity working
        skip_churn = False
        if match_tbl.match_type == 'alpm' and match_tbl.has_idle_time:
            skip_churn = True

        skip_fastreconfig = False
        if v_dev_id == 1:
            skip_fastreconfig = True

        test = BFTest(self, sess_hdl, v_dev_id, p_dev_id, match_tbl, resource_tbls, resource_map)

        try:
        #with PyCallGraph(output=GraphvizOutput()):
            test.run_scale_test()

            if resource_map['Hitless_HA']:
                test.run_hitless_ha_test(True)

            if skip_churn is False:
                test.run_churn_test()

            test.clear_all_entries()

            test.set_symmetric(0)
            test.run_scale_test()

            if skip_churn is False:
                test.run_churn_test()
            # test.clear_all_entries()
            if skip_fastreconfig is False:
                test.run_fast_reconfig_test()
                if skip_churn is False:
                    test.run_churn_test()

        finally:
            try:
                pass
                # Teardown is not necessary as this is the only test in this file
                # test.teardown()
            except:
                pass
            self.conn_mgr.client_cleanup(sess_hdl)
            bf_log.log('Seed used %d' % seed)
