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

import pdb
import time
import datetime
import sys
import logging
import math

import unittest
import random
from pprint import pprint
import pd_base_tests

from ptf import config
from ptf.testutils import *
from p4testutils.misc_utils import *
from ptf.thriftutils import *

import os

from smoke_large_tbls.p4_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *

import random

swports = get_sw_ports()

color_green = 0
color_yellow = 1
color_red = 2

seed = random.randint(0, 65536)
print('Seed used %d' % seed)
sys.stdout.flush()
random.seed(seed)


dev_id = 0
MAX_PORT_COUNT = 456
def tofLocalPortToOfPort(port, dev_id):
    assert port < MAX_PORT_COUNT
    return (dev_id * MAX_PORT_COUNT) + port

def port_to_pipe(port):
    local_port = port & 0x7F
    assert(local_port < 72)
    pipe = (port >> 7) & 0x3
    assert(port == ((pipe << 7) | local_port))
    return pipe


def_nop_entries_installed = False
def InstallAllDefaultEntries(self):
    global def_nop_entries_installed

    if def_nop_entries_installed == True:
        print("Default nop entries already installed")
        return

    sess_hdl = self.conn_mgr.client_init()
    dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))

    nop_default_fns = [
        self.client.idle_stats_tbl_set_default_action_nop,
        self.client.atcam_tbl_set_default_action_nop
        ]

    print("Installing default entries as NOP for all tables")

    for nop_fn in nop_default_fns:
        nop_fn(sess_hdl,
               dev_tgt)

    indirect_nop_default_fns = {'atcam_indirect_tbl':
        (
            self.client.atcam_indirect_tbl_set_default_entry,
         self.client.atcam_action_profile_add_member_with_nop
        )
                }

    for tbl in indirect_nop_default_fns:
        entry_add_fn, member_add_fn = indirect_nop_default_fns[tbl]
        mbr_hdl = member_add_fn(sess_hdl, dev_tgt)
        entry_add_fn(sess_hdl, dev_tgt, mbr_hdl)

    print("closing session")
    status = self.conn_mgr.client_cleanup(sess_hdl)
    def_nop_entries_installed = True


class IdleNotifyHelper():
    '''Helper function for notify tests'''
    def __init__(self, fns, params, idle_params, dataplane, thriftinterface, tbl_params, mat_type='TCAM', test_move=False, min_entry_ttl = None):
        supported_bit_widths = [2,3,6]
        self.thriftinterface = thriftinterface
        self.dataplane = dataplane
        self.fns = fns
        self.mat_type = mat_type
        self.sess_hdl, self.dev_id, self.port = params
        self.dev_tgt = DevTarget_t(self.dev_id, hex_to_i16(0xFFFF))
        self.test_move = test_move

        self.cookie = 12

        self.min_ttl, self.query_ttl, self.max_ttl = idle_params
        if min_entry_ttl is None:
            self.min_entry_ttl = self.min_ttl
        else:
            self.min_entry_ttl = min_entry_ttl
        self.bit_width, self.twoway, self.perflowdisable = tbl_params
        assert self.bit_width in supported_bit_widths, 'Bit width %d is not supported'

        assert self.max_ttl >= max(self.query_ttl, self.min_ttl)
        print('TTL Max %d Min %d query %d' % (self.max_ttl, self.min_ttl, self.query_ttl))

        self.calc_sweep_period()

        self.max_error = datetime.timedelta.min
        self.min_error = datetime.timedelta.max

        assert self.twoway == True

    def calc_sweep_period(self):
        if test_param_get('arch') == "tofino":
            if test_param_get('target') == "asic-model":
                clock_speed = 1271000000
            else:
                clock_speed = self.thriftinterface.devport_mgr.devport_mgr_get_clock_speed(self.dev_id).bps_clock_speed
        else:
            clock_speed = 1000000000
        notify_time = min(self.query_ttl, self.min_ttl)

        no_states = (1 << self.bit_width)-1

        if self.twoway:
            no_states -= 1
        if self.perflowdisable:
            no_states -= 1

        sweep_count = notify_time * (clock_speed//1000)
        sweep_count //= no_states
        sweep_count >>= 21

        sweep_interval = min(15, int(math.ceil(math.log(sweep_count, 2))))
        self.hw_sweep_period = (1000 << (sweep_interval + 21))//clock_speed

        self.hw_notify_period = self.hw_sweep_period * no_states

        if self.query_ttl == self.min_ttl and self.min_ttl == self.max_ttl:
            if notify_time > self.hw_notify_period:
                self.sw_sweep_period = notify_time
            else:
                self.sw_sweep_period = 0
        else:
            self.sw_sweep_period = notify_time

        print('HW notify period %d sw_sweep_period %d' % (self.hw_notify_period, self.sw_sweep_period))

        if self.twoway:
            self.twowayerr = self.hw_sweep_period + self.sw_sweep_period
        else:
            self.twowayerr = 0

    def calc_err_margin(self, ttl, active_err=None):
        if active_err is None:
            active_err = self.twowayerr
        if self.sw_sweep_period:
            err = self.sw_sweep_period
            err += self.sw_sweep_period - (abs(ttl-self.hw_notify_period) % self.sw_sweep_period)
        else:
            err = abs(ttl-self.hw_notify_period)
        err += active_err
        # Account for some more error due to the communication - 1000ms
        err += 1000

        # For inter-stage moves, account for the error due to approximation
        err += self.hw_notify_period
        return err

    def enable_notify_mode(self):
        idle_tbl_params = smoke_large_tbls_idle_time_params_t (
                smoke_large_tbls_idle_time_mode.NOTIFY_MODE,
                self.query_ttl,
                self.max_ttl,
                self.min_ttl,
                self.cookie
                )
        self.fns["idle_tmo_enable"](self.sess_hdl,
                self.dev_id,
                idle_tbl_params)

    def disable_notify_mode(self):
        self.fns["idle_tmo_disable"](self.sess_hdl,
                self.dev_id)

    def add_match_entry(self, match_entries, ip_dst, ttl, match_key_ip_dst=None, match_len = 32):
        if match_key_ip_dst is None:
            match_key_ip_dst = ip_dst

        if self.mat_type == 'TCAM':
            match_spec = self.fns["match_spec_create"](ipv4Addr_to_i32(match_key_ip_dst), match_len)
        else:
            match_spec = self.fns["match_spec_create"](ipv4Addr_to_i32(match_key_ip_dst))
        act_spec = self.fns["action_spec_create"](self.port)
        ent_hdl = self.fns["mat_ent_add"](self.sess_hdl, self.dev_tgt, match_spec, 33-match_len, act_spec, ttl)
        self.fns["complete_operations"](self.sess_hdl)
        now = datetime.datetime.now()
        match_entries[ent_hdl] = OrderedDict()
        match_entries[ent_hdl]["ip_dst"] = ip_dst
        match_entries[ent_hdl]["active_time"] = now
        match_entries[ent_hdl]["active_err"] = 0
        if ttl:
            state = "active"
        else:
            state = "inactive"
        match_entries[ent_hdl]["state"] = state
        match_entries[ent_hdl]["ttl"] = ttl
        print('Entry 0x%x add entry ttl %d state-%s time %s' % (ent_hdl, ttl, state, now))

    def del_match_entry(self, match_entries, ent_hdl, delete=True):
        self.fns["mat_ent_del"](self.sess_hdl, self.dev_id, ent_hdl)
        now = datetime.datetime.now()
        print('Entry 0x%x delete time %s' % (ent_hdl, now))
        if delete:
            del match_entries[ent_hdl]

    def set_ttl(self, match_entries, ent_hdl, ttl):
        self.fns["set_ttl"](self.sess_hdl, self.dev_id, ent_hdl, ttl)
        self.fns["complete_operations"](self.sess_hdl)
        now = datetime.datetime.now()
        old_state = match_entries[ent_hdl]["state"]
        old_ttl = match_entries[ent_hdl]["ttl"]
        old_active_time = match_entries[ent_hdl]["active_time"]

        new_active_time = None
        if (now - old_active_time) >= datetime.timedelta(milliseconds=old_ttl):
            new_active_time = now - datetime.timedelta(milliseconds=old_ttl)

        if old_state == 'inactive':
            new_active_time = now

        if new_active_time:
            match_entries[ent_hdl]["active_time"] = new_active_time

        match_entries[ent_hdl]["active_err"] = self.sw_sweep_period
        if ttl:
            state = "active"
        else:
            state = "inactive"
        match_entries[ent_hdl]["state"] = state
        match_entries[ent_hdl]["ttl"] = ttl
        print('Entry 0x%x set ttl %d old-ttl %d state-%s time %s' % (ent_hdl, ttl, old_ttl, state, now))

    def verify_ttl(self, match_entries, aged_entries):
        print('Verifying TTL')
        for ent_hdl in match_entries:
            active_time = match_entries[ent_hdl]["active_time"]
            ttl = match_entries[ent_hdl]["ttl"]
            state = match_entries[ent_hdl]["state"]
            active_err = match_entries[ent_hdl]["active_err"]

            now = datetime.datetime.now()
            cur_ttl = self.fns["get_ttl"](self.sess_hdl, self.dev_id, ent_hdl)
            match_entries[ent_hdl]["now"] = now
            match_entries[ent_hdl]["cur_ttl"] = cur_ttl

            if ttl == 0:
                print('Entry 0x%x DISABLED' % (ent_hdl))
                continue

            err_margin = self.calc_err_margin(ttl, active_err)
            print('Entry 0x%x Error margin %d active_err %d' % (ent_hdl, err_margin, active_err))
            max_entry_alive_time = datetime.timedelta(milliseconds=(ttl+err_margin))
            # If a sweep happens right after activate, then the state changes
            # a little ahead of time, so account for this

            abs_entry_alive_time = datetime.timedelta(milliseconds=ttl)
            min_entry_alive_time = datetime.timedelta(milliseconds=(ttl-self.hw_sweep_period))

            diff = now - active_time

            max_ttl_diff = datetime.timedelta(milliseconds=(ttl-cur_ttl+err_margin))
            min_ttl_diff = datetime.timedelta(milliseconds=(ttl-cur_ttl))

            print('Now %s Diff %s cur_ttl %d ttl %d ' % (now, diff,  cur_ttl, ttl))
            assert cur_ttl <= ttl, 'Current ttl %d is greater than entry-ttl %d' % (cur_ttl, ttl)
            if state == "idle":
                assert cur_ttl == 0, 'Current ttl %d is not zero for idle entry' % (cur_ttl)
                continue

            if diff < datetime.timedelta(milliseconds=active_err):
                # In this case, the cur_ttl may not be accurate
                pass
            else:
                if diff >= max_entry_alive_time:
                    assert cur_ttl == 0, 'Current ttl %d is not zero' % (cur_ttl)
                else:
                    if diff < min_entry_alive_time :
                        assert cur_ttl != 0, 'Current ttl is zero'
#                    assert diff <= max_ttl_diff, 'The TTL decrement is too less ent 0x%x' % ent_hdl
#                    assert diff >= min_ttl_diff, 'The TTL decrement is too fast ent 0x%x' % ent_hdl

                if cur_ttl == 0:
                    aged_entries.add(ent_hdl)
                    match_entries[ent_hdl]["state"] = "idle"
                    if (diff - abs_entry_alive_time) > self.max_error:
                        self.max_error = diff - abs_entry_alive_time
                    if (diff - abs_entry_alive_time) < self.min_error:
                        self.min_error = diff - abs_entry_alive_time

    def count_active_entries(self, match_entries):
        active_entries = [x for x in match_entries if match_entries[x]["state"] == "active"]
        return len(active_entries)

    def get_expired_entries(self, expired_entries):
        expired = self.fns["get_expired"](self.sess_hdl, self.dev_id)
        cur_expired_entries = set([x.entry for x in expired])

        expired_entries |= cur_expired_entries

    def sweep_check(self, match_entries):
        aged_entries = set()
        expired_entries = set()

        # Give some buffer
        time_period_msecs = min(self.min_ttl, self.query_ttl)+100
        time_period=datetime.timedelta(milliseconds=time_period_msecs)
        total_time_slept = datetime.timedelta()

        total_time_to_sleep = datetime.timedelta(milliseconds=self.max_ttl)
        # Add the maximum error margin
        total_time_to_sleep += datetime.timedelta(milliseconds=self.calc_err_margin(self.hw_notify_period))
        total_time_to_sleep += datetime.timedelta(milliseconds=2000)

        first_called = datetime.datetime.now()
        num_calls=0
        drift=datetime.timedelta()
        while total_time_slept < total_time_to_sleep:
            total_time_slept += time_period
            time.sleep((time_period_msecs/1000.0)-(drift.microseconds/1000000.0))
            current_time = datetime.datetime.now()
            self.verify_ttl(match_entries, aged_entries)
            self.get_expired_entries(expired_entries)
            if self.count_active_entries(match_entries) == 0:
                break
            num_calls += 1
            difference = current_time - first_called
            drift = difference - time_period* num_calls

        print(datetime.datetime.now(), "Getting expired entries")
        self.get_expired_entries(expired_entries)
        print(datetime.datetime.now(), "Got expired entries")

        print('Aged entries', aged_entries)
        print('Expired entries', expired_entries)
        if expired_entries < aged_entries:
            print('Aged but not expired', aged_entries - expired_entries)
            failing_entries = aged_entries - expired_entries
            for entry_hdl in failing_entries:
                print("Entry", hex(entry_hdl))
                print(match_entries[entry_hdl])
                print()
            print(datetime.datetime.now(), "Getting expired entries (again)")
            more_expired_entries = set()
            self.get_expired_entries(more_expired_entries)
            print(datetime.datetime.now(), "Got expired entries")
            print(more_expired_entries)
            print("Waiting to see if anything else ages out")
            time.sleep(180)
            print(datetime.datetime.now(), "Getting expired entries (again)")
            more_expired_entries = set()
            self.get_expired_entries(more_expired_entries)
            print(datetime.datetime.now(), "Got expired entries")
            print(more_expired_entries)
        assert expired_entries >= aged_entries

    def send_matching_pkts(self, match_entries, in_pkts):
        self.fns["complete_operations"](self.sess_hdl)
        debug = False

        for (pkt, ent_hdl) in in_pkts:
            print("Sending packet ent 0x%x ip_dst %s" % (ent_hdl, match_entries[ent_hdl]["ip_dst"]))
            send_packet(self.thriftinterface, tofLocalPortToOfPort(swports[1], self.dev_id), pkt)
            (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll(port_number=tofLocalPortToOfPort(self.port, self.dev_id),timeout=5)
            assert rcv_pkt != None, "No packet received"
            now = datetime.datetime.now()
            match_entries[ent_hdl]["active_time"] = now
            match_entries[ent_hdl]["active_err"] = self.twowayerr
            ttl = match_entries[ent_hdl]["ttl"]
            if ttl:
                match_entries[ent_hdl]["state"] = "active"
            state = match_entries[ent_hdl]["state"]
            print('Entry 0x%x send pkt %d state-%s time %s' % (ent_hdl, ttl, state, now))

        # Sleep for hw-notify-period so that msgs reach bf-drivers
        time.sleep(((self.hw_notify_period+500)/1000.0))

    def test(self, params):
        NUM_MAT_ENTRIES = params["num_mat_entries"]
        match_entries = OrderedDict()
        try:
            self.enable_notify_mode()

            if self.test_move == False:
                ip_dsts = ["10.0.%d.%d" % (x//255, x%255) for x in range(NUM_MAT_ENTRIES)]

                for ip_dst in ip_dsts:
                    ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                    self.add_match_entry(match_entries, ip_dst, ttl)

                self.sweep_check(match_entries)
            else:
                ip_dsts = []
                var_part = ["34.34", "18.34", "17.34", "17.18"]
                prefix_len = [16, 20, 24, 28]
                for x in range(NUM_MAT_ENTRIES):
                    ip_dst = "%d.%d.17.17" % (x//255, x%255)
                    match_key = "%d.%d.%s" %(x//255, x%255, var_part[x % len(var_part)])
                    match_len = prefix_len[x%len(var_part)]

                    ip_dsts.append((ip_dst, match_key, match_len))

                # Add 1/2 entries now
                for ip_dst, match_key, match_len in ip_dsts[:len(ip_dsts)//2]:
                    ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                    self.add_match_entry(match_entries, ip_dst, ttl, match_key_ip_dst=match_key, match_len=match_len)


                in_pkts = [(simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            ip_src='1.1.1.1',
                                            ip_dst=match_entries[ent_hdl]["ip_dst"],
                                            ip_ttl=64,
                                            with_tcp_chksum = False), ent_hdl) for ent_hdl in match_entries]

                self.send_matching_pkts(match_entries, in_pkts)


                for ip_dst, match_key, match_len in ip_dsts[len(ip_dsts)//2:]:
                    ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                    self.add_match_entry(match_entries, ip_dst, ttl, match_key_ip_dst=match_key, match_len=match_len)

                self.sweep_check(match_entries)


            in_pkts = [(simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=match_entries[ent_hdl]["ip_dst"],
                                        ip_ttl=64,
                                        with_tcp_chksum = False), ent_hdl) for ent_hdl in match_entries]

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))
            self.sweep_check(match_entries)

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))
            self.sweep_check(match_entries)

            #Make entries active again
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

            # Delete some entries
#            entries_to_delete = random.sample(match_entries.keys(), 1+ ((NUM_MAT_ENTRIES-1)//4))
            entries_to_delete = list(match_entries.keys())
            ip_dsts_deleted = []

            for ent_hdl in entries_to_delete:
                ip_dsts_deleted.append(match_entries[ent_hdl]["ip_dst"])
                self.del_match_entry(match_entries, ent_hdl)

            for ip_dst in ip_dsts_deleted:
                ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                self.add_match_entry(match_entries, ip_dst, ttl)

            self.sweep_check(match_entries)

            in_pkts = [(simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=match_entries[ent_hdl]["ip_dst"],
                                        ip_ttl=64,
                                        with_tcp_chksum = False), ent_hdl) for ent_hdl in match_entries]

            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

#            entries_to_set_ttl = random.sample(match_entries.keys(), 1+ ((NUM_MAT_ENTRIES-1)//4))
            entries_to_set_ttl = list(match_entries.keys())
            for ent_hdl in entries_to_set_ttl:
                ttl = random.choice([0]+ [random.randint(self.min_entry_ttl, self.max_ttl)] * 3)
                self.set_ttl(match_entries, ent_hdl, ttl)

            self.sweep_check(match_entries)

        except:
            print('Something failed')
            raise
        finally:
            for ent_hdl in match_entries:
                self.del_match_entry(match_entries, ent_hdl, delete=False)
            self.fns["complete_operations"](self.sess_hdl)
            self.disable_notify_mode()
            print('MAX ERROR %s MIN ERROR %s' % (self.max_error, self.min_error))

class IdlePollHelper():
    '''Helper function for poll tests'''
    def __init__(self, fns, params, dataplane, thriftinterface, mat_type='TCAM', test_move = False):
        self.dataplane = dataplane
        self.thriftinterface = thriftinterface
        self.fns = fns
        self.mat_type = mat_type
        self.sess_hdl, self.dev_id, self.port = params
        self.dev_tgt = DevTarget_t(self.dev_id, hex_to_i16(0xFFFF))
        self.test_move = test_move

    def enable_poll_mode(self):
        idle_tbl_params = smoke_large_tbls_idle_time_params_t (
                smoke_large_tbls_idle_time_mode.POLL_MODE
                )
        self.fns["idle_tmo_enable"](self.sess_hdl,
                self.dev_id,
                idle_tbl_params)

    def disable_poll_mode(self):
        self.fns["idle_tmo_disable"](self.sess_hdl,
                self.dev_id)

    def add_match_entry(self, match_entries, ip_dst, match_key_ip_dst=None, match_len = 32):
        if match_key_ip_dst is None:
            match_key_ip_dst = ip_dst

        if self.mat_type == 'TCAM':
            match_spec = self.fns["match_spec_create"](ipv4Addr_to_i32(match_key_ip_dst), match_len)
        else:
            match_spec = self.fns["match_spec_create"](ipv4Addr_to_i32(match_key_ip_dst))
        act_spec = self.fns["action_spec_create"](self.port)
        ttl = 0
        ent_hdl = self.fns["mat_ent_add"](self.sess_hdl, self.dev_tgt, match_spec, 33-match_len, act_spec, ttl)
        match_entries[ip_dst] = OrderedDict()
        match_entries[ip_dst]["entry_hdl"] = ent_hdl
        match_entries[ip_dst]["state"] = smoke_large_tbls_idle_time_hit_state.ENTRY_IDLE

    def del_match_entry(self, match_entries, ip_dst, delete=True):
        ent_hdl = match_entries[ip_dst]["entry_hdl"]
        self.fns["mat_ent_del"](self.sess_hdl, self.dev_id, ent_hdl)
        if delete:
            del match_entries[ip_dst]

    def verify_hit_state(self, match_entries, relax = False):
        self.fns["complete_operations"](self.sess_hdl)
        self.fns["update_hit_state"](self.sess_hdl, self.dev_id)

        for ip_dst in match_entries:
            ent_hdl = match_entries[ip_dst]["entry_hdl"]
            state = match_entries[ip_dst]["state"]

            hit_state = self.fns["get_hit_state"](self.sess_hdl, self.dev_id, ent_hdl)
            if relax == False:
                assert state == hit_state, "State %d hit_state %d ent_hdl 0x%x" % (state, hit_state, ent_hdl)

            # Reset the hit-state
            match_entries[ip_dst]["state"] = smoke_large_tbls_idle_time_hit_state.ENTRY_IDLE
        time.sleep(2)

    def send_matching_pkts(self, match_entries, in_pkts):
        self.fns["complete_operations"](self.sess_hdl)
        debug = False

        for (pkt, ip_dst) in in_pkts:
            print("Sending packet ip_dst %s" % ip_dst)
            send_packet(self.thriftinterface, tofLocalPortToOfPort(swports[1], self.dev_id), pkt)
            (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll(port_number=tofLocalPortToOfPort(self.port, self.dev_id),timeout=5)
            assert rcv_pkt != None
            match_entries[ip_dst]["state"] = smoke_large_tbls_idle_time_hit_state.ENTRY_ACTIVE

    def test(self, params):
        NUM_MAT_ENTRIES = params["num_mat_entries"]
        match_entries = OrderedDict()
        try:
            self.enable_poll_mode()
            self.verify_hit_state(match_entries)

            if self.test_move == False:
                ip_dsts = ["%d.%d.17.17" % (x//255, x%255) for x in range(NUM_MAT_ENTRIES)]
                match_key = ip_dsts[:]
                lens = [32] * NUM_MAT_ENTRIES

                ip_dsts = list(zip(ip_dsts, match_key, [32]*NUM_MAT_ENTRIES))
            else:
                ip_dsts = []
                var_part = ["34.34", "18.34", "17.34", "17.18"]
                prefix_len = [16, 20, 24, 28]
                for x in range(NUM_MAT_ENTRIES):
                    ip_dst = "%d.%d.17.17" % (x//255, x%255)
                    match_key = "%d.%d.%s" %(x//255, x%255, var_part[x % len(var_part)])
                    match_len = prefix_len[x%len(var_part)]

                    ip_dsts.append((ip_dst, match_key, match_len))

            in_pkts = [(simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        ip_src='1.1.1.1',
                                        ip_dst=ip_dst,
                                        ip_ttl=64,
                                        with_tcp_chksum = False), ip_dst) for ip_dst, match_key, match_len in ip_dsts]

            if self.test_move:
                # Add 1/2 entries now
                for ip_dst, match_key, match_len in ip_dsts[:len(ip_dsts)//2]:
                    self.add_match_entry(match_entries, ip_dst, match_key_ip_dst=match_key, match_len=match_len)
                self.verify_hit_state(match_entries)

                # Send pkts matching the entries
                assert len(ip_dsts) == len(in_pkts)
                self.send_matching_pkts(match_entries, in_pkts[:len(ip_dsts)//2])

                #Add the rest of the entries
                for ip_dst, match_key, match_len in ip_dsts[len(ip_dsts)//2:]:
                    self.add_match_entry(match_entries, ip_dst, match_key_ip_dst=match_key, match_len=match_len)
                self.verify_hit_state(match_entries, relax = True)

            else:
                for ip_dst, match_key, match_len in ip_dsts:
                    self.add_match_entry(match_entries, ip_dst, match_key_ip_dst=match_key, match_len=match_len)
                self.verify_hit_state(match_entries)


            # Send pkts matching some entries
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,1+((NUM_MAT_ENTRIES-1)//2))))

            self.verify_hit_state(match_entries)

            # Send pkts matching each entry
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

            # Delete some entries
            entries_to_delete = random.sample(ip_dsts, 1+ ((NUM_MAT_ENTRIES-1)//4))

            for ip_dst, match_key, match_len in entries_to_delete:
                self.del_match_entry(match_entries, ip_dst)

            for ip_dst, match_key, match_len in entries_to_delete:
                self.add_match_entry(match_entries, ip_dst, match_key_ip_dst=match_key, match_len=match_len)
            self.verify_hit_state(match_entries)
            self.send_matching_pkts(match_entries, random.sample(in_pkts, min(100,NUM_MAT_ENTRIES)))

            self.verify_hit_state(match_entries)
            self.verify_hit_state(match_entries)
        finally:
            for ip_dst in match_entries:
                self.del_match_entry(match_entries, ip_dst, delete=False)
            self.disable_poll_mode()

@group("thorough")
class TestIdle(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["smoke_large_tbls"])

    def create_match_spec(self, ipDst, match_len):
        ethDstAddr = "01:02:03:04:05:06"
        ethDstAddrMask = "00:00:00:00:00:00"
        ethSrcAddr = "01:02:03:04:05:06"
        ethSrcAddrMask = "00:00:00:00:00:00"

        ipSrc = "20.0.0.1"
        ipSrcMask = "0.0.0.0"

        assert match_len <= 32, "Match len %d > 32" % (match_len)

        ethernet_dstAddr = macAddr_to_string(ethDstAddr)
        ethernet_dstAddr_mask = macAddr_to_string(ethDstAddrMask)
        ethernet_srcAddr = macAddr_to_string(ethSrcAddr)
        ethernet_srcAddr_mask = macAddr_to_string(ethSrcAddrMask)
        ethernet_etherType = hex_to_i16(0)
        ethernet_etherType_mask = hex_to_i16(0)
        vlan_tag_pri = hex_to_byte(0)
        vlan_tag_pri_mask = hex_to_byte(0)
        vlan_tag_cfi = hex_to_byte(0)
        vlan_tag_cfi_mask = hex_to_byte(0)
        vlan_tag_vlan_id = hex_to_i16(0)
        vlan_tag_vlan_id_mask = hex_to_i16(0)
        vlan_tag_etherType = hex_to_i16(0)
        vlan_tag_etherType_mask = hex_to_i16(0)
        ipv4_version = hex_to_byte(0)
        ipv4_version_mask = hex_to_byte(0)
        ipv4_ihl = hex_to_byte(0)
        ipv4_ihl_mask = hex_to_byte(0)
        ipv4_diffserv = hex_to_byte(0)
        ipv4_diffserv_mask = hex_to_byte(0)
        ipv4_totalLen = hex_to_i16(0)
        ipv4_totalLen_mask = hex_to_i16(0)
        ipv4_identification = hex_to_i16(0)
        ipv4_identification_mask = hex_to_i16(0)
        ipv4_flags = hex_to_byte(0)
        ipv4_flags_mask = hex_to_byte(0)
        ipv4_fragOffset = hex_to_i16(0)
        ipv4_fragOffset_mask = hex_to_i16(0)
        ipv4_ttl = hex_to_byte(0)
        ipv4_ttl_mask = hex_to_byte(0)
        ipv4_protocol = hex_to_byte(0)
        ipv4_protocol_mask = hex_to_byte(0)
        ipv4_hdrChecksum = hex_to_i16(0)
        ipv4_hdrChecksum_mask = hex_to_i16(0)
        ipv4_dstAddr = ipDst
        ipv4_dstAddr_mask = hex_to_i32((0xFFFFFFFF >> (32-match_len)) << (32-match_len))
        ipv4_srcAddr = ipv4Addr_to_i32(ipSrc)
        ipv4_srcAddr_mask = ipv4Addr_to_i32(ipSrcMask)
        tcp_srcPort = hex_to_i16(0)
        tcp_srcPort_mask = hex_to_i16(0)
        tcp_dstPort = hex_to_i16(0)
        tcp_dstPort_mask = hex_to_i16(0)
        tcp_seqNo = hex_to_i32(0)
        tcp_seqNo_mask = hex_to_i32(0)
        tcp_ackNo = hex_to_i32(0)
        tcp_ackNo_mask = hex_to_i32(0)
        tcp_dataOffset = hex_to_byte(0)
        tcp_dataOffset_mask = hex_to_byte(0)
        tcp_res = hex_to_byte(0)
        tcp_res_mask = hex_to_byte(0)
        tcp_ecn = hex_to_byte(0)
        tcp_ecn_mask = hex_to_byte(0)
        tcp_ctrl = hex_to_byte(0)
        tcp_ctrl_mask = hex_to_byte(0)
        tcp_window = hex_to_i16(0)
        tcp_window_mask = hex_to_i16(0)
        tcp_checksum = hex_to_i16(0)
        tcp_checksum_mask = hex_to_i16(0)
        tcp_urgentPtr = hex_to_i16(0)
        tcp_urgentPtr_mask = hex_to_i16(0)
        udp_srcPort = hex_to_i16(0)
        udp_srcPort_mask = hex_to_i16(0)
        udp_dstPort = hex_to_i16(0)
        udp_dstPort_mask = hex_to_i16(0)
        udp_hdr_length = hex_to_i16(0)
        udp_hdr_length_mask = hex_to_i16(0)
        match_spec = smoke_large_tbls_idle_stats_tbl_match_spec_t (
            ethernet_dstAddr,
            ethernet_dstAddr_mask,
            ethernet_srcAddr,
            ethernet_srcAddr_mask,
            ethernet_etherType,
            ethernet_etherType_mask,
            vlan_tag_pri,
            vlan_tag_pri_mask,
            vlan_tag_cfi,
            vlan_tag_cfi_mask,
            vlan_tag_vlan_id,
            vlan_tag_vlan_id_mask,
            vlan_tag_etherType,
            vlan_tag_etherType_mask,
            ipv4_version,
            ipv4_version_mask,
            ipv4_ihl,
            ipv4_ihl_mask,
            ipv4_diffserv,
            ipv4_diffserv_mask,
            ipv4_totalLen,
            ipv4_totalLen_mask,
            ipv4_identification,
            ipv4_identification_mask,
            ipv4_flags,
            ipv4_flags_mask,
            ipv4_fragOffset,
            ipv4_fragOffset_mask,
            ipv4_ttl,
            ipv4_ttl_mask,
            ipv4_protocol,
            ipv4_protocol_mask,
            ipv4_hdrChecksum,
            ipv4_hdrChecksum_mask,
            ipv4_dstAddr,
            ipv4_dstAddr_mask,
            ipv4_srcAddr,
            ipv4_srcAddr_mask,
            tcp_srcPort,
            tcp_srcPort_mask,
            tcp_dstPort,
            tcp_dstPort_mask,
            tcp_seqNo,
            tcp_seqNo_mask,
            tcp_ackNo,
            tcp_ackNo_mask,
            tcp_dataOffset,
            tcp_dataOffset_mask,
            tcp_res,
            tcp_res_mask,
            tcp_ecn,
            tcp_ecn_mask,
            tcp_ctrl,
            tcp_ctrl_mask,
            tcp_window,
            tcp_window_mask,
            tcp_checksum,
            tcp_checksum_mask,
            tcp_urgentPtr,
            tcp_urgentPtr_mask,
            udp_srcPort,
            udp_srcPort_mask,
            udp_dstPort,
            udp_dstPort_mask,
            udp_hdr_length,
            udp_hdr_length_mask
                )
        return match_spec

    """ Basic test """
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        InstallAllDefaultEntries(self)
        print('Seed used %d' % seed)
        sys.stdout.flush()
        random.seed(seed)
        sess_hdl = self.conn_mgr.client_init()
        print("PIPE_MGR gave me that session handle:", sess_hdl)

        dev_id = 0
        port = swports[3]

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        try:
            ##########################################

            fns = OrderedDict()
            fns["mat_ent_add"] = self.client.idle_stats_tbl_table_add_with_set_egress
            fns["mat_ent_del"] = self.client.idle_stats_tbl_table_delete
            fns["idle_tmo_enable"] = self.client.idle_stats_tbl_idle_tmo_enable
            fns["idle_tmo_disable"] = self.client.idle_stats_tbl_idle_tmo_disable
            fns["match_spec_create"] = self.create_match_spec
            fns["action_spec_create"] = smoke_large_tbls_set_egress_action_spec_t
            fns["complete_operations"] = self.conn_mgr.complete_operations
            fns["set_ttl"] = self.client.idle_stats_tbl_set_ttl
            fns["get_ttl"] = self.client.idle_stats_tbl_get_ttl
            fns["get_expired"] = self.client.idle_stats_tbl_idle_tmo_get_expired
            fns["update_hit_state"] = self.client.idle_stats_tbl_update_hit_state
            fns["get_hit_state"] = self.client.idle_stats_tbl_get_hit_state

            params = sess_hdl, dev_id, port

            print('---------idle_stats_tbl Notify Test START---------')
            idle_params = (5000, 5000, 80000)
            tbl_params = (3, True, True)
            notify_test_helper = IdleNotifyHelper(fns, params, idle_params, self.dataplane, self, tbl_params, test_move=True, min_entry_ttl = 60000)
            test_params = OrderedDict()
            test_params["num_mat_entries"] = 2000
            notify_test_helper.test(test_params)
            print('---------idle_stats_tbl Notify Test COMPLETED---------')
            print('---------idle_stats_tbl Poll Test START---------')
            poll_test_helper = IdlePollHelper(fns, params, self.dataplane, self, test_move = True)
            test_params = OrderedDict()
            test_params["num_mat_entries"] = 2000
            poll_test_helper.test(test_params)
            print('---------idle_stats_tbl Poll Test COMPLETED---------')

            ##########################################


        finally:
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print('Seed used %d' % seed)
            sys.stdout.flush()
        return

class TestInterStageStats(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["smoke_large_tbls"])

    """ Basic test """
    def runTest(self):
        InstallAllDefaultEntries(self)
        print('Seed used %d' % seed)
        sys.stdout.flush()
        random.seed(seed)

        sess_hdl = self.conn_mgr.client_init()
        print("PIPE_MGR gave me that session handle:", sess_hdl)

        dev_id = 0

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        total_entries = 2000
        entries_of_each_prio = 4
        mask_str = ["FF:FF:FF:FF", "FF:FF:FF:00", "FF:FF:00:00", "FF:00:00:00"]
        pkt_str = ["01:02:03:04",  "01:02:03:AB",  "01:02:AB:CD", "01:AB:CD:EF"]

        entries_per_prio = (total_entries + entries_of_each_prio) // entries_of_each_prio

        eth_info = []
        for priority in reversed(list(range(entries_of_each_prio))):
            ethSrcAddr = ["%02x:%02x:01:02:03:04" % (x//256, x%256) for x in range(entries_per_prio)]
            ethSrcMask = ["FF:FF:%s" % (mask_str[priority]) for x in range(entries_per_prio)]
            pkt_eth_src = ["%02x:%02x:%s" % (x//256, x%256, pkt_str[priority]) for x in range(entries_per_prio)]
            priorities = [priority] * entries_per_prio
            num_pkts = [random.randint(1,4)] * entries_per_prio

            eth_info.extend(list(zip(ethSrcAddr, ethSrcMask, pkt_eth_src, priorities, num_pkts)))

        egr_ports = [ swports[((x % 3) + 2)] for x in range(total_entries)]

        test_matrix = list(zip(eth_info, egr_ports))

        test_matrix = test_matrix[:total_entries]
        initial_test_matrix = test_matrix[:total_entries//2]
        final_test_matrix = test_matrix[total_entries//2:]

        initial_entry_hdls = []
        final_entry_hdls = []

        try:
            ethDstAddr = "01:02:03:04:05:06"
            ethDstAddrMask = "FF:FF:FF:FF:FF:FF"

            ipDst = "10.0.0.1"
            match_len = 32
            ipSrc = "20.0.0.1"
            ipSrcMask = "255.255.255.255"

            ttl = 0

            eth_dst_pkt = ethDstAddr
            ip_src_pkt = ipSrc
            ip_dst_pkt = ipDst

            for match_param in initial_test_matrix:
                eth_info, egr_port = match_param
                ethSrcAddr, ethSrcAddrMask, eth_src_pkt, priority, num_pkts = eth_info

                ethernet_dstAddr = macAddr_to_string(ethDstAddr)
                ethernet_dstAddr_mask = macAddr_to_string(ethDstAddrMask)
                ethernet_srcAddr = macAddr_to_string(ethSrcAddr)
                ethernet_srcAddr_mask = macAddr_to_string(ethSrcAddrMask)
                ethernet_etherType = hex_to_i16(0)
                ethernet_etherType_mask = hex_to_i16(0)
                vlan_tag_pri = hex_to_byte(0)
                vlan_tag_pri_mask = hex_to_byte(0)
                vlan_tag_cfi = hex_to_byte(0)
                vlan_tag_cfi_mask = hex_to_byte(0)
                vlan_tag_vlan_id = hex_to_i16(0)
                vlan_tag_vlan_id_mask = hex_to_i16(0)
                vlan_tag_etherType = hex_to_i16(0)
                vlan_tag_etherType_mask = hex_to_i16(0)
                ipv4_version = hex_to_byte(0)
                ipv4_version_mask = hex_to_byte(0)
                ipv4_ihl = hex_to_byte(0)
                ipv4_ihl_mask = hex_to_byte(0)
                ipv4_diffserv = hex_to_byte(0)
                ipv4_diffserv_mask = hex_to_byte(0)
                ipv4_totalLen = hex_to_i16(0)
                ipv4_totalLen_mask = hex_to_i16(0)
                ipv4_identification = hex_to_i16(0)
                ipv4_identification_mask = hex_to_i16(0)
                ipv4_flags = hex_to_byte(0)
                ipv4_flags_mask = hex_to_byte(0)
                ipv4_fragOffset = hex_to_i16(0)
                ipv4_fragOffset_mask = hex_to_i16(0)
                ipv4_ttl = hex_to_byte(0)
                ipv4_ttl_mask = hex_to_byte(0)
                ipv4_protocol = hex_to_byte(0)
                ipv4_protocol_mask = hex_to_byte(0)
                ipv4_hdrChecksum = hex_to_i16(0)
                ipv4_hdrChecksum_mask = hex_to_i16(0)
                ipv4_dstAddr = ipv4Addr_to_i32(ipDst)
                ipv4_dstAddr_mask = hex_to_i32((0xFFFFFFFF >> (32-match_len)) << (32-match_len))
                ipv4_srcAddr = ipv4Addr_to_i32(ipSrc)
                ipv4_srcAddr_mask = ipv4Addr_to_i32(ipSrcMask)
                tcp_srcPort = hex_to_i16(0)
                tcp_srcPort_mask = hex_to_i16(0)
                tcp_dstPort = hex_to_i16(0)
                tcp_dstPort_mask = hex_to_i16(0)
                tcp_seqNo = hex_to_i32(0)
                tcp_seqNo_mask = hex_to_i32(0)
                tcp_ackNo = hex_to_i32(0)
                tcp_ackNo_mask = hex_to_i32(0)
                tcp_dataOffset = hex_to_byte(0)
                tcp_dataOffset_mask = hex_to_byte(0)
                tcp_res = hex_to_byte(0)
                tcp_res_mask = hex_to_byte(0)
                tcp_ecn = hex_to_byte(0)
                tcp_ecn_mask = hex_to_byte(0)
                tcp_ctrl = hex_to_byte(0)
                tcp_ctrl_mask = hex_to_byte(0)
                tcp_window = hex_to_i16(0)
                tcp_window_mask = hex_to_i16(0)
                tcp_checksum = hex_to_i16(0)
                tcp_checksum_mask = hex_to_i16(0)
                tcp_urgentPtr = hex_to_i16(0)
                tcp_urgentPtr_mask = hex_to_i16(0)
                udp_srcPort = hex_to_i16(0)
                udp_srcPort_mask = hex_to_i16(0)
                udp_dstPort = hex_to_i16(0)
                udp_dstPort_mask = hex_to_i16(0)
                udp_hdr_length = hex_to_i16(0)
                udp_hdr_length_mask = hex_to_i16(0)
                match_spec = smoke_large_tbls_idle_stats_tbl_match_spec_t (
                    ethernet_dstAddr,
                    ethernet_dstAddr_mask,
                    ethernet_srcAddr,
                    ethernet_srcAddr_mask,
                    ethernet_etherType,
                    ethernet_etherType_mask,
                    vlan_tag_pri,
                    vlan_tag_pri_mask,
                    vlan_tag_cfi,
                    vlan_tag_cfi_mask,
                    vlan_tag_vlan_id,
                    vlan_tag_vlan_id_mask,
                    vlan_tag_etherType,
                    vlan_tag_etherType_mask,
                    ipv4_version,
                    ipv4_version_mask,
                    ipv4_ihl,
                    ipv4_ihl_mask,
                    ipv4_diffserv,
                    ipv4_diffserv_mask,
                    ipv4_totalLen,
                    ipv4_totalLen_mask,
                    ipv4_identification,
                    ipv4_identification_mask,
                    ipv4_flags,
                    ipv4_flags_mask,
                    ipv4_fragOffset,
                    ipv4_fragOffset_mask,
                    ipv4_ttl,
                    ipv4_ttl_mask,
                    ipv4_protocol,
                    ipv4_protocol_mask,
                    ipv4_hdrChecksum,
                    ipv4_hdrChecksum_mask,
                    ipv4_dstAddr,
                    ipv4_dstAddr_mask,
                    ipv4_srcAddr,
                    ipv4_srcAddr_mask,
                    tcp_srcPort,
                    tcp_srcPort_mask,
                    tcp_dstPort,
                    tcp_dstPort_mask,
                    tcp_seqNo,
                    tcp_seqNo_mask,
                    tcp_ackNo,
                    tcp_ackNo_mask,
                    tcp_dataOffset,
                    tcp_dataOffset_mask,
                    tcp_res,
                    tcp_res_mask,
                    tcp_ecn,
                    tcp_ecn_mask,
                    tcp_ctrl,
                    tcp_ctrl_mask,
                    tcp_window,
                    tcp_window_mask,
                    tcp_checksum,
                    tcp_checksum_mask,
                    tcp_urgentPtr,
                    tcp_urgentPtr_mask,
                    udp_srcPort,
                    udp_srcPort_mask,
                    udp_dstPort,
                    udp_dstPort_mask,
                    udp_hdr_length,
                    udp_hdr_length_mask
                        )

                action_spec = smoke_large_tbls_set_egress_action_spec_t(egr_port)

                entry_hdl = self.client.idle_stats_tbl_table_add_with_set_egress (sess_hdl, dev_tgt,
                        match_spec, priority, action_spec, ttl)
                initial_entry_hdls.append(entry_hdl)

            self.conn_mgr.complete_operations(sess_hdl)

            print("Sending packets for the entries installed")

            for match_param in initial_test_matrix:
                eth_info, egr_port = match_param
                ethSrcAddr, ethSrcAddrMask, eth_src_pkt, priority, num_pkts = eth_info

                for i in range(num_pkts):
                    pkt = simple_tcp_packet(eth_dst=eth_dst_pkt,
                            eth_src=eth_src_pkt,
                            ip_src=ip_src_pkt,
                            ip_dst=ip_dst_pkt,
                            ip_ttl=64)

                    send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
                    verify_packet(self, pkt, tofLocalPortToOfPort(egr_port, dev_id))

            print("Installing the remaining entries")

            for match_param in final_test_matrix:
                eth_info, egr_port = match_param
                ethSrcAddr, ethSrcAddrMask, eth_src_pkt, priority, num_pkts = eth_info

                ethernet_dstAddr = macAddr_to_string(ethDstAddr)
                ethernet_dstAddr_mask = macAddr_to_string(ethDstAddrMask)
                ethernet_srcAddr = macAddr_to_string(ethSrcAddr)
                ethernet_srcAddr_mask = macAddr_to_string(ethSrcAddrMask)
                ethernet_etherType = hex_to_i16(0)
                ethernet_etherType_mask = hex_to_i16(0)
                vlan_tag_pri = hex_to_byte(0)
                vlan_tag_pri_mask = hex_to_byte(0)
                vlan_tag_cfi = hex_to_byte(0)
                vlan_tag_cfi_mask = hex_to_byte(0)
                vlan_tag_vlan_id = hex_to_i16(0)
                vlan_tag_vlan_id_mask = hex_to_i16(0)
                vlan_tag_etherType = hex_to_i16(0)
                vlan_tag_etherType_mask = hex_to_i16(0)
                ipv4_version = hex_to_byte(0)
                ipv4_version_mask = hex_to_byte(0)
                ipv4_ihl = hex_to_byte(0)
                ipv4_ihl_mask = hex_to_byte(0)
                ipv4_diffserv = hex_to_byte(0)
                ipv4_diffserv_mask = hex_to_byte(0)
                ipv4_totalLen = hex_to_i16(0)
                ipv4_totalLen_mask = hex_to_i16(0)
                ipv4_identification = hex_to_i16(0)
                ipv4_identification_mask = hex_to_i16(0)
                ipv4_flags = hex_to_byte(0)
                ipv4_flags_mask = hex_to_byte(0)
                ipv4_fragOffset = hex_to_i16(0)
                ipv4_fragOffset_mask = hex_to_i16(0)
                ipv4_ttl = hex_to_byte(0)
                ipv4_ttl_mask = hex_to_byte(0)
                ipv4_protocol = hex_to_byte(0)
                ipv4_protocol_mask = hex_to_byte(0)
                ipv4_hdrChecksum = hex_to_i16(0)
                ipv4_hdrChecksum_mask = hex_to_i16(0)
                ipv4_dstAddr = ipv4Addr_to_i32(ipDst)
                ipv4_dstAddr_mask = hex_to_i32((0xFFFFFFFF >> (32-match_len)) << (32-match_len))
                ipv4_srcAddr = ipv4Addr_to_i32(ipSrc)
                ipv4_srcAddr_mask = ipv4Addr_to_i32(ipSrcMask)
                tcp_srcPort = hex_to_i16(0)
                tcp_srcPort_mask = hex_to_i16(0)
                tcp_dstPort = hex_to_i16(0)
                tcp_dstPort_mask = hex_to_i16(0)
                tcp_seqNo = hex_to_i32(0)
                tcp_seqNo_mask = hex_to_i32(0)
                tcp_ackNo = hex_to_i32(0)
                tcp_ackNo_mask = hex_to_i32(0)
                tcp_dataOffset = hex_to_byte(0)
                tcp_dataOffset_mask = hex_to_byte(0)
                tcp_res = hex_to_byte(0)
                tcp_res_mask = hex_to_byte(0)
                tcp_ecn = hex_to_byte(0)
                tcp_ecn_mask = hex_to_byte(0)
                tcp_ctrl = hex_to_byte(0)
                tcp_ctrl_mask = hex_to_byte(0)
                tcp_window = hex_to_i16(0)
                tcp_window_mask = hex_to_i16(0)
                tcp_checksum = hex_to_i16(0)
                tcp_checksum_mask = hex_to_i16(0)
                tcp_urgentPtr = hex_to_i16(0)
                tcp_urgentPtr_mask = hex_to_i16(0)
                udp_srcPort = hex_to_i16(0)
                udp_srcPort_mask = hex_to_i16(0)
                udp_dstPort = hex_to_i16(0)
                udp_dstPort_mask = hex_to_i16(0)
                udp_hdr_length = hex_to_i16(0)
                udp_hdr_length_mask = hex_to_i16(0)
                match_spec = smoke_large_tbls_idle_stats_tbl_match_spec_t (
                    ethernet_dstAddr,
                    ethernet_dstAddr_mask,
                    ethernet_srcAddr,
                    ethernet_srcAddr_mask,
                    ethernet_etherType,
                    ethernet_etherType_mask,
                    vlan_tag_pri,
                    vlan_tag_pri_mask,
                    vlan_tag_cfi,
                    vlan_tag_cfi_mask,
                    vlan_tag_vlan_id,
                    vlan_tag_vlan_id_mask,
                    vlan_tag_etherType,
                    vlan_tag_etherType_mask,
                    ipv4_version,
                    ipv4_version_mask,
                    ipv4_ihl,
                    ipv4_ihl_mask,
                    ipv4_diffserv,
                    ipv4_diffserv_mask,
                    ipv4_totalLen,
                    ipv4_totalLen_mask,
                    ipv4_identification,
                    ipv4_identification_mask,
                    ipv4_flags,
                    ipv4_flags_mask,
                    ipv4_fragOffset,
                    ipv4_fragOffset_mask,
                    ipv4_ttl,
                    ipv4_ttl_mask,
                    ipv4_protocol,
                    ipv4_protocol_mask,
                    ipv4_hdrChecksum,
                    ipv4_hdrChecksum_mask,
                    ipv4_dstAddr,
                    ipv4_dstAddr_mask,
                    ipv4_srcAddr,
                    ipv4_srcAddr_mask,
                    tcp_srcPort,
                    tcp_srcPort_mask,
                    tcp_dstPort,
                    tcp_dstPort_mask,
                    tcp_seqNo,
                    tcp_seqNo_mask,
                    tcp_ackNo,
                    tcp_ackNo_mask,
                    tcp_dataOffset,
                    tcp_dataOffset_mask,
                    tcp_res,
                    tcp_res_mask,
                    tcp_ecn,
                    tcp_ecn_mask,
                    tcp_ctrl,
                    tcp_ctrl_mask,
                    tcp_window,
                    tcp_window_mask,
                    tcp_checksum,
                    tcp_checksum_mask,
                    tcp_urgentPtr,
                    tcp_urgentPtr_mask,
                    udp_srcPort,
                    udp_srcPort_mask,
                    udp_dstPort,
                    udp_dstPort_mask,
                    udp_hdr_length,
                    udp_hdr_length_mask
                        )

                action_spec = smoke_large_tbls_set_egress_action_spec_t(egr_port)

                entry_hdl = self.client.idle_stats_tbl_table_add_with_set_egress (sess_hdl, dev_tgt,
                        match_spec, priority, action_spec, ttl)
                final_entry_hdls.append(entry_hdl)

            self.conn_mgr.complete_operations(sess_hdl)

            print('Syncing stats')
            self.client.counter_hw_sync_dummy_cntr(sess_hdl, dev_tgt, True)
            flags = smoke_large_tbls_counter_flags_t(0)

            print('Querying stats')

            num_pkts_arr = []
            for match_param in initial_test_matrix:
                eth_info, egr_port = match_param
                ethSrcAddr, ethSrcAddrMask, eth_src_pkt, priority, num_pkts = eth_info

                num_pkts_arr.append(num_pkts)

            i = 0
            for entry_hdl in initial_entry_hdls:
                cntr = self.client.counter_read_dummy_cntr(sess_hdl, dev_tgt, entry_hdl, flags)
                if (cntr.packets != num_pkts_arr[i]):
                    print("Count value for entry handle %d did not match, expected : %d, received : %d" % (entry_hdl, num_pkts_arr[i], cntr.packets))
                    assert cntr.packets == num_pkts_arr[i]
                i = i + 1

        except:
            raise
        finally:
            for entry_hdl in initial_entry_hdls:
                status = self.client.idle_stats_tbl_table_delete( sess_hdl,
                        dev_id,
                        entry_hdl
                        )

            for entry_hdl in final_entry_hdls:
                status = self.client.idle_stats_tbl_table_delete( sess_hdl,
                        dev_id,
                        entry_hdl
                        )
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
        return

@group("thorough")
class TestAtcam(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["smoke_large_tbls"])
        self.snap_hdl = None

    def get_key_mask(self, x):
        bits_to_remove = int(math.log(self.count_diff_priorities, 2))
        bits_to_add = self.count_diff_priorities - 1

        y = x & ((1<<bits_to_remove)-1)
        entry_priority = y

        mask = (1<<bits_to_add)-1
        mask &= mask<<y

        # Based on y, figure out the dest mask
        ip_dst = ((x>>bits_to_remove)<<bits_to_add) & 0xffffffff
        ip_dst_mask = (0xffffffff<<bits_to_add) | mask
        ip_dst_mask &= 0xffffffff
        return hex_to_i32(ip_dst), hex_to_i32(ip_dst_mask), hex_to_i32(entry_priority)

    def get_pkt_ip_dst(self, x):
        bits_to_remove = int(math.log(self.count_diff_priorities, 2))
        bits_to_add = self.count_diff_priorities - 1

        y = x & ((1<<bits_to_remove)-1)
        mask = 0
        if y:
            mask = 1<<(y-1)

        ip_dst, _, _ = self.get_key_mask(x)
        ip_dst |= mask
        return hex_to_i32(ip_dst)

    def snap_set_up(self, ing_port):
        if self.snap_hdl:
            self.client.snapshot_delete(self.snap_hdl)
            self.snap_hdl = None
        pipe = port_to_pipe(ing_port)
        self.snap_dt = DevTarget_t(dev_id, pipe)
        start_stage = 0
        end_stage = 11
        dir_val = 0
        self.snap_hdl = self.client.snapshot_create(self.snap_dt, start_stage, end_stage, dir_val)
        self.assertIsNotNone(self.snap_hdl)
        self.client.snapshot_capture_trigger_set(self.snap_hdl,
                                                 smoke_large_tbls_snapshot_trig_spec_t("tcp_valid", 1, 1),
                                                 smoke_large_tbls_snapshot_trig_spec_t("ipv4_valid", 1, 1))
        self.client.snapshot_state_set(self.snap_hdl, 1, 0)
        snap_state = self.client.snapshot_state_get(self.snap_hdl, self.snap_dt.dev_pipe_id)
        self.assertEqual(snap_state, 1)

    def snap_get_tbl_hit_status(self):
        snap_state = self.client.snapshot_state_get(self.snap_hdl, self.snap_dt.dev_pipe_id)
        self.assertEqual(snap_state, 0)
        snap_data = self.client.snapshot_capture_tbl_data_get(self.snap_hdl,
                                                              self.snap_dt.dev_pipe_id,
                                                              "atcam_tbl")
        if snap_data.hit:
            return snap_data.hit_entry_handle
        return None

    def snap_cleanup(self):
        if self.snap_hdl:
            self.client.snapshot_delete(self.snap_hdl)
            self.snap_hdl = None
            self.snap_dt = None

    """ Basic test """
    def runTest(self):
        InstallAllDefaultEntries(self)
        print('Seed used %d' % seed)
        sys.stdout.flush()
        random.seed(seed)
        sess_hdl = self.conn_mgr.client_init()
        print("PIPE_MGR gave me that session handle:", sess_hdl)

        dev_id = 0
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        TABLE_SIZE=500000
        PARTITION_SIZE=4096
        NUM_PORTS=7

        self.count_diff_priorities = 16

        entries = dict()

        # Some partitions might have more entries due to sizing
        entries_per_partition = TABLE_SIZE//PARTITION_SIZE

        try:
            # Check the default entry
            default_egr_port = swports[6]
            action_spec = smoke_large_tbls_set_egress_action_spec_t (
                    default_egr_port
                    )

            self.client.atcam_tbl_set_default_action_set_egress(
                    sess_hdl,
                    dev_tgt,
                    action_spec
                    )

            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    dl_vlan_enable=True,
                                    vlan_vid=10,
                                    ip_src='1.1.1.1',
                                    ip_dst='30.30.30.30',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

            verify_packet(self, pkt, tofLocalPortToOfPort(default_egr_port, dev_id))

            test_matrix = []
            egr_port = swports[0]

            partitions = list(range(10, PARTITION_SIZE))
            partitions.remove(4000)
            for partition in partitions:
                vlan = partition
                start = random.randint(0, 1<<14)
                start = 0
                uniq_vals_to_use = list(range(start, start+entries_per_partition))
                for uniq_val in uniq_vals_to_use:
                    test_matrix.append((vlan, uniq_val, egr_port))
                    egr_port = swports[(egr_port + 1) % NUM_PORTS]

            random.shuffle(test_matrix)

            uniq_check = [a[1:] for a in test_matrix]
            if len(uniq_check) == len(set(uniq_check)):
                print('No duplicates')
            else:
                print('DUPLICATE entries present')
            test_matrixes = [test_matrix]#, test_matrix[::-1]]

            checked_vlan = dict()

            tcpV = 1
            tcpVmask = 1

            target = test_param_get('target')
            for test_matrix in test_matrixes:
                count = 0
                self.conn_mgr.begin_batch(sess_hdl)
                for vlan, uniq_val, egr_port in test_matrix:
                    ip_dst, mask, priority = self.get_key_mask(uniq_val)

                    match_spec = smoke_large_tbls_atcam_tbl_match_spec_t(
                            tcpV,
                            tcpVmask,
                            vlan,
                            ip_dst,
                            mask
                            )
                    action_spec = smoke_large_tbls_set_egress_action_spec_t (
                            egr_port
                            )

                    entry_hdl = self.client.atcam_tbl_table_add_with_set_egress(
                            sess_hdl,
                            dev_tgt,
                            match_spec,
                            priority,
                            action_spec
                            )
                    entries[(vlan, ip_dst, mask, priority, egr_port)] = entry_hdl
                    count += 1
                    if count % 1000 == 0:
                        print("Added %d entries" % count)

                self.conn_mgr.end_batch(sess_hdl, True )
                check = random.sample(test_matrix, 1000)
                for count, (vlan, uniq_val, egr_port) in enumerate(check):
                    ip_dst, mask, priority = self.get_key_mask(uniq_val)

                    entry_hdl = entries[(vlan, ip_dst, mask, priority, egr_port)]

                    checked_vlan[vlan] = checked_vlan.setdefault(vlan, 0) + 1
                    ip_dst = self.get_pkt_ip_dst(uniq_val)
                    ip_dst = i32_to_ipv4Addr(ip_dst)

                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            dl_vlan_enable=True,
                                            vlan_vid=vlan,
                                            ip_src='1.1.1.1',
                                            ip_dst=ip_dst,
                                            ip_id=101,
                                            ip_ttl=64)
                    ing_port = random.choice(swports)
                    snap_check = count % 25 == 0

                    # Skip snapshot test on bmv2
                    if target == 'bmv2':
                        snap_check = False

                    #print("Packet %d: Ing-port %d Egr-port %d Entry %d Snap %d" % (count, ing_port, egr_port, entry_hdl, snap_check))
                    if snap_check:
                        self.snap_set_up(ing_port)

                    send_packet(self, tofLocalPortToOfPort(ing_port, dev_id), pkt)
                    verify_packet(self, pkt, tofLocalPortToOfPort(egr_port, dev_id))

                    if snap_check:
                        hit_entry = self.snap_get_tbl_hit_status()
                        self.assertEqual(hit_entry, entry_hdl)

                pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                        eth_src='00:22:22:22:22:22',
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_src='1.1.1.1',
                                        ip_dst='30.30.30.30',
                                        ip_id=101,
                                        ip_ttl=64)
                send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

                verify_packet(self, pkt, tofLocalPortToOfPort(default_egr_port, dev_id))

                pprint(checked_vlan)
                self.conn_mgr.begin_batch(sess_hdl)
                for entry in list(entries.values()):
                    self.client.atcam_tbl_table_delete(
                            sess_hdl, dev_id, entry
                            )
                self.conn_mgr.end_batch(sess_hdl, True )
                entries = dict()


        finally:
            self.client.atcam_tbl_set_default_action_nop(
                    sess_hdl,
                    dev_tgt,
                    )

            self.conn_mgr.begin_batch(sess_hdl)
            for entry in list(entries.values()):
                self.client.atcam_tbl_table_delete(
                        sess_hdl, dev_id, entry
                        )
            self.conn_mgr.end_batch(sess_hdl, True )
            self.snap_cleanup()
            self.conn_mgr.complete_operations(sess_hdl)
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print('Seed used %d' % seed)
            sys.stdout.flush()

class TestSnapshot(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["smoke_large_tbls"])

    """ Basic test """
    def runTest(self):
        if test_param_get('target') == "bmv2":
            return
        InstallAllDefaultEntries(self)
        print('Seed used %d' % seed)
        sys.stdout.flush()
        random.seed(seed)
        sess_hdl = self.conn_mgr.client_init()

        dev_id = 0
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        entry_hdl = None
        snap_hdl = 0

        ig_port = swports[1]
        ig_pipe = port_to_pipe(ig_port)
        eg_port = swports[1]
        vlan = 10
        tcpV = 1
        tcpVmask = 1
        ip_dst = "1.2.3.4"
        mask = 32
        priority = 0
        try:

            match_spec = smoke_large_tbls_atcam_tbl_match_spec_t(
                    tcpV,
                    tcpVmask,
                    vlan,
                    ipv4Addr_to_i32(ip_dst),
                    mask
                    )
            action_spec = smoke_large_tbls_set_egress_action_spec_t (
                    eg_port
                    )

            entry_hdl = self.client.atcam_tbl_table_add_with_set_egress(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    priority,
                    action_spec
                    )

            snap_hdl = self.client.snapshot_create(dev_tgt, 0, 11, 0)
            print("SnapHdl %d" % (snap_hdl))
            sys.stdout.flush()
            self.assertNotEqual(snap_hdl, 0)
            # Set it to only capture for this packet's IPs
            trig_spec1 = smoke_large_tbls_snapshot_trig_spec_t("ipv4_dstAddr",
                            ipv4Addr_to_i32(ip_dst), ipv4Addr_to_i32('255.255.255.255'))
            trig_spec2 = smoke_large_tbls_snapshot_trig_spec_t("tcp_valid", 1, 1)
            self.client.snapshot_capture_trigger_set(snap_hdl, trig_spec1,
                                                     trig_spec2)
            # Enable the snapshot
            self.client.snapshot_state_set(snap_hdl, 1, 0)
            ip_pipe = 0
            snap_state = self.client.snapshot_state_get(snap_hdl, ig_pipe)
            self.assertEqual(snap_state, 1)
            self.conn_mgr.complete_operations(sess_hdl)

            # Send a packet
            pkt = simple_tcp_packet(dl_vlan_enable=True,
                                    vlan_vid=vlan,
                                    ip_dst=ip_dst)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            verify_packet(self, pkt, tofLocalPortToOfPort(eg_port, dev_id))

            # Verify the snapshot was taken
            snap_state = self.client.snapshot_state_get(snap_hdl, ig_pipe)
            self.assertEqual(snap_state, 0)
            # Verify the entry handle was matched
            t = self.client.snapshot_capture_tbl_data_get(snap_hdl, ig_pipe, "atcam_tbl")
            self.assertTrue(t.hit)
            self.assertTrue(t.executed)
            self.assertFalse(t.inhibited)
            self.assertEqual(t.hit_entry_handle, entry_hdl)


        finally:
            # Clean up the snapshot
            if snap_hdl != 0:
                self.client.snapshot_delete(snap_hdl)

            self.client.atcam_tbl_set_default_action_nop(
                    sess_hdl,
                    dev_tgt,
                    )

            if entry_hdl:
                self.client.atcam_tbl_table_delete(sess_hdl, dev_id, entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)
            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print('Seed used %d' % seed)
            sys.stdout.flush()

class TestAtcamTernaryValid(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["smoke_large_tbls"])

    def runTest(self):
        InstallAllDefaultEntries(self)
        print('Seed used %d' % seed)
        sys.stdout.flush()
        random.seed(seed)
        sess_hdl = self.conn_mgr.client_init()
        print("PIPE_MGR gave me that session handle:", sess_hdl)

        dev_id = 0
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        try:
            # Set default entry
            default_egr_port = swports[6]
            action_spec = smoke_large_tbls_set_egress_action_spec_t (
                    default_egr_port
                    )
            self.client.atcam_tbl_set_default_action_set_egress(
                    sess_hdl,
                    dev_tgt,
                    action_spec
                    )

            # Turn on tcp valid field
            tcpV = 1
            tcpVmask = 1
            vlan = 10
            outPort = swports[3]
            match_spec = smoke_large_tbls_atcam_tbl_match_spec_t(
                    tcpV,
                    tcpVmask,
                    vlan,
                    ipv4Addr_to_i32("10.0.0.1"),
                    hex_to_i32(0xffffffff)
                    )
            action_spec = smoke_large_tbls_set_egress_action_spec_t (outPort)

            entry_hdl = self.client.atcam_tbl_table_add_with_set_egress(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    0,
                    action_spec
                    )

            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    dl_vlan_enable=True,
                                    vlan_vid=vlan,
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            verify_packet(self, pkt, tofLocalPortToOfPort(outPort, dev_id))

            pkt = simple_udp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    dl_vlan_enable=True,
                                    vlan_vid=vlan,
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            verify_packet(self, pkt, tofLocalPortToOfPort(default_egr_port, dev_id))

            self.client.atcam_tbl_table_delete(
                        sess_hdl, dev_id, entry_hdl
                        )

            # Turn off tcp valid field
            tcpV = 1
            tcpVmask = 0
            vlan = 10
            outPort = swports[4]
            match_spec = smoke_large_tbls_atcam_tbl_match_spec_t(
                    tcpV,
                    tcpVmask,
                    vlan,
                    ipv4Addr_to_i32("10.0.0.1"),
                    hex_to_i32(0xffffffff)
                    )
            action_spec = smoke_large_tbls_set_egress_action_spec_t (outPort)

            entry_hdl = self.client.atcam_tbl_table_add_with_set_egress(
                    sess_hdl,
                    dev_tgt,
                    match_spec,
                    0,
                    action_spec
                    )

            self.conn_mgr.complete_operations(sess_hdl)
            pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    dl_vlan_enable=True,
                                    vlan_vid=vlan,
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_id=101,
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            verify_packet(self, pkt, tofLocalPortToOfPort(outPort, dev_id))

            pkt = simple_udp_packet(eth_dst='00:11:11:11:11:11',
                                    eth_src='00:22:22:22:22:22',
                                    dl_vlan_enable=True,
                                    vlan_vid=vlan,
                                    ip_src='1.1.1.1',
                                    ip_dst='10.0.0.1',
                                    ip_ttl=64)
            send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)
            verify_packet(self, pkt, tofLocalPortToOfPort(outPort, dev_id))

        finally:
            self.client.atcam_tbl_set_default_action_nop(
                    sess_hdl,
                    dev_tgt,
                    )
            self.client.atcam_tbl_table_delete(
                        sess_hdl, dev_id, entry_hdl
                        )
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print('Seed used %d' % seed)
            sys.stdout.flush()

class TestAtcamIndirectAction(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["smoke_large_tbls"])

    def get_key_mask(self, x):
        bits_to_remove = int(math.log(self.count_diff_priorities, 2))
        bits_to_add = self.count_diff_priorities - 1

        y = x & ((1<<bits_to_remove)-1)
        entry_priority = y

        mask = (1<<bits_to_add)-1
        mask &= mask<<y

        # Based on y, figure out the dest mask
        ip_src = ((x>>bits_to_remove)<<bits_to_add) & 0xffffffff
        ip_src_mask = (0xffffffff<<bits_to_add) | mask
        ip_src_mask &= 0xffffffff
        return hex_to_i32(ip_src), hex_to_i32(ip_src_mask), hex_to_i32(entry_priority)

    def get_pkt_ip_src(self, x):
        bits_to_remove = int(math.log(self.count_diff_priorities, 2))
        bits_to_add = self.count_diff_priorities - 1

        y = x & ((1<<bits_to_remove)-1)
        mask = 0
        if y:
            mask = 1<<(y-1)

        ip_src, _, _ = self.get_key_mask(x)
        ip_src |= mask
        return hex_to_i32(ip_src)

    """ Basic test """
    def runTest(self):
        InstallAllDefaultEntries(self)
        print('Seed used %d' % seed)
        sys.stdout.flush()
        random.seed(seed)
        sess_hdl = self.conn_mgr.client_init()
        print("PIPE_MGR gave me that session handle:", sess_hdl)

        dev_id = 0
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        TABLE_SIZE=100000
        ADT_SIZE=65536
        PARTITION_SIZE=4096
        NUM_PORTS=7

        self.count_diff_priorities = 16

        entries = dict()
        adt_entries = dict()

        # Some partitions might have more entries due to sizing
        entries_per_partition = TABLE_SIZE//PARTITION_SIZE

        try:
            ports = []
            for i in range(NUM_PORTS):
                ports.append(swports[i])
            action_params = list(zip(list(range(ADT_SIZE)), ports))

            for egr_ip_id, egr_port in action_params:
                action_spec = smoke_large_tbls_set_ip_id_action_spec_t (
                        hex_to_i16(egr_ip_id),
                        egr_port
                        )
                adt_hdl = self.client.atcam_action_profile_add_member_with_set_ip_id(
                        sess_hdl,
                        dev_tgt,
                        action_spec
                        )
                adt_entries[adt_hdl] = (egr_ip_id, egr_port)

            test_matrix = []

            partitions = list(range(10, PARTITION_SIZE))
            partitions.remove(4000)
            for partition in partitions:
                vlan = partition
                start = random.randint(0, 1<<14)
                start = 0
                uniq_vals_to_use = list(range(start, start+entries_per_partition))
                for uniq_val in uniq_vals_to_use:
                    test_matrix.append((vlan, uniq_val))

            random.shuffle(test_matrix)

            test_matrixes = [test_matrix]#, test_matrix[::-1]]

            checked_vlan = dict()

            for test_matrix in test_matrixes:
                count = 0
                self.conn_mgr.begin_batch(sess_hdl)
                for vlan, uniq_val in test_matrix:
                    ip_src, mask, priority = self.get_key_mask(uniq_val)

                    match_spec = smoke_large_tbls_atcam_indirect_tbl_match_spec_t(
                            vlan,
                            ip_src,
                            mask
                            )
                    action_entry = random.choice(list(adt_entries.keys()))

                    entry_hdl = self.client.atcam_indirect_tbl_add_entry(
                            sess_hdl,
                            dev_tgt,
                            match_spec,
                            priority,
                            action_entry
                            )
                    entries[(vlan, ip_src, mask, priority)] = (entry_hdl, action_entry)
                    count += 1
                    if count % 1000 == 0:
                        print("Added %d entries" % count)

                self.conn_mgr.end_batch(sess_hdl, True )
                check = random.sample(test_matrix, 1000)
                for vlan, uniq_val in check:
                    ip_src, mask, priority = self.get_key_mask(uniq_val)

                    entry_hdl, action_entry = entries[(vlan, ip_src, mask, priority)]

                    checked_vlan[vlan] = checked_vlan.setdefault(vlan, 0) + 1
                    ip_src = self.get_pkt_ip_src(uniq_val)
                    ip_src = i32_to_ipv4Addr(ip_src)

                    pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            dl_vlan_enable=True,
                                            vlan_vid=vlan,
                                            ip_dst='1.1.1.1',
                                            ip_src=ip_src,
                                            ip_id=101,
                                            ip_ttl=64)
                    send_packet(self, tofLocalPortToOfPort(swports[1], dev_id), pkt)

                    egr_ip_id, egr_port = adt_entries[action_entry]
                    exp_pkt = simple_tcp_packet(eth_dst='00:11:11:11:11:11',
                                            eth_src='00:22:22:22:22:22',
                                            dl_vlan_enable=True,
                                            vlan_vid=vlan,
                                            ip_dst='1.1.1.1',
                                            ip_src=ip_src,
                                            ip_id=egr_ip_id,
                                            ip_ttl=64)
                    verify_packet(self, exp_pkt, tofLocalPortToOfPort(egr_port, dev_id))
                pprint(checked_vlan)

        finally:
            self.conn_mgr.begin_batch(sess_hdl)
            for entry_hdl, action_entry in list(entries.values()):
                self.client.atcam_indirect_tbl_table_delete(
                        sess_hdl,
                        dev_id,
                        hex_to_i32(entry_hdl)
                        )

            for action_entry in adt_entries:
                self.client.atcam_action_profile_del_member(
                        sess_hdl,
                        dev_id,
                        hex_to_i32(action_entry)
                        )
            self.conn_mgr.end_batch(sess_hdl, True )

            print("closing session")
            status = self.conn_mgr.client_cleanup(sess_hdl)
            print('Seed used %d' % seed)
            sys.stdout.flush()
