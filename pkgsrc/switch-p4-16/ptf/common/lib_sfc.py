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


import ast
import copy
import time
import types
import pprint
import unittest
from enum import Enum, IntEnum

import pd_base_tests
from collections import namedtuple

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

from switch_helpers import ApiHelper

try:
    from bfruntime_client_base_tests import BfRuntimeTest

    import bfrt_grpc.client as gc

    print("BFRUNTIME-GRPC ENABLED!")
    BFRUNTIME_ENABLE = True

    from bf_pktpy.packets import (
        Ether,
        IP,
        UDP,
        IB_BTH,
        MACControlClassBasedFlowControl,
        RoceOpcode
    )


    def simple_pfc_pkt(pktlen=60,
                       eth_dst='01:80:c2:00:00:01',
                       c0=0,
                       c1=0,
                       c2=0,
                       c3=0,
                       c4=0,
                       c5=0,
                       c6=0,
                       c7=0,
                       pause0_time=0,
                       pause1_time=0,
                       pause2_time=0,
                       pause3_time=0,
                       pause4_time=0,
                       pause5_time=0,
                       pause6_time=0,
                       pause7_time=0):
        pkt = Ether(dst=eth_dst, src='00:00:00:00:00:00', type=0x8808)
        pkt /= MACControlClassBasedFlowControl(_op_code=0x0101, _reserved=0,
                                               c0_enabled=c0,
                                               c1_enabled=c1,
                                               c2_enabled=c2,
                                               c3_enabled=c3,
                                               c4_enabled=c4,
                                               c5_enabled=c5,
                                               c6_enabled=c6,
                                               c7_enabled=c7,
                                               c0_pause_time=pause0_time,
                                               c1_pause_time=pause1_time,
                                               c2_pause_time=pause2_time,
                                               c3_pause_time=pause3_time,
                                               c4_pause_time=pause4_time,
                                               c5_pause_time=pause5_time,
                                               c6_pause_time=pause6_time,
                                               c7_pause_time=pause7_time)
        if (len(pkt) < pktlen):
            pkt /= bytes([0] * (60 - len(pkt)))

        return pkt


    def simple_rocev2_bth(eth_dst,
                          eth_src,
                          ip_dst,
                          ip_src,
                          opcode=RoceOpcode.UC_SEND_ONLY.value,
                          ip_dscp=0,
                          ip_ttl=64,
                          udp_sport=0,
                          dst_qp=0,
                          psn=0,
                          pktlen=64,
                          ):
        header = (
                Ether(dst=eth_dst, src=eth_src) /
                IP(dst=ip_dst, src=ip_src, tos=ip_dscp, ttl=ip_ttl) /
                UDP(dport=4791, sport=udp_sport, chksum=0) /
                IB_BTH(opcode=opcode,
                       dst_qp=dst_qp,
                       psn=psn)
        )

        if pktlen > len(header):
            return header / bytes([0] * (pktlen - len(header)))
        else:
            return header


    class LinkToType(IntEnum):
        Unknown = 0,
        Switch = 1,
        Server = 2


    class PortSpeed(IntEnum):
        Unset = 0x0
        GbE_10 = 0x1
        GbE_25 = 0x2
        GbE_40 = 0x3
        GbE_50 = 0x4
        GbE_100 = 0x5
        GbE_200 = 0x6
        GbE_400 = 0x7
        GbE_800 = 0x8


    class TmQstatMode(Enum):
        Any = 0
        Color = 1


    class TmColor(Enum):
        Green = 0
        Yellow = 1
        Red = 2


    class MauSnapMatch(Enum):
        Ingress = 0
        Ghost = 1
        Both = 2


    class TmColorLimits(Enum):
        Percent_0 = 0
        Percent_12_5 = 1
        Percent_25 = 2
        Percent_37_5 = 3
        Percent_50 = 4
        Percent_62_5 = 5
        Percent_75 = 6
        Percent_87_5 = 7
        Percent_100 = 8


    QidMau = namedtuple("QueueIdMau", ["port_id_9b", "queue_id_7b"])
    QidTm = namedtuple("QidTm", ["pipe_id_2b", "queue_id_11b"])


    class Bit(object):
        """
        TODO: Add all operators
        https://docs.python.org/3/library/operator.html
        """

        class Fmt(Enum):
            Bin = 0
            Dec = 1
            Hex = 2

        class BitIterator(object):

            def __init__(self, bit):
                self.bit = bit
                self.idx = 0

            def __next__(self):
                if self.idx < self.bit.size:
                    retval = self.bit[self.idx]
                else:
                    raise StopIteration
                self.idx += 1
                return retval

            def next(self):
                return self.__next__()

            def __iter__(self):
                return self

        def __init__(self, size, value=0, fmt=Fmt.Dec):
            self.size = size
            self.mask = 2 ** size - 1
            self.fmt = fmt
            if isinstance(value, tuple) and len(value) == 2:
                self.value = 0
                self.set_pattern(value[0], value_width=value[1])
            elif isinstance(value, int) or isinstance(value, types.LongType):
                self.value = value & self.mask
                assert self.value == value, "Value too large for bitsize."
            else:
                raise Exception(
                    "Unknown value type: {}({})".format(type(value), value))

        def __iter__(self):
            return Bit.BitIterator(self)

        def __add__(self, other):
            assert isinstance(other, Bit)
            result_val = (self.value + other.value) & self.mask
            return Bit(self.size, result_val)

        def __getitem__(self, key):
            if isinstance(key, int):
                return (self.value >> key) & 1
            elif isinstance(key, slice):
                assert key.step is None
                assert key.start >= key.stop
                assert key.start < self.size
                assert key.stop < self.size

                shift_right = key.stop
                result_size = key.start - key.stop + 1
                result_mask = int(2 ** result_size - 1)
                # print("shift={}, mask={}, size={}".format(shift_right, result_mask, result_size))
                result_value = (self.value >> shift_right) & result_mask
                return Bit(result_size, value=result_value, fmt=self.fmt)
            else:
                raise Exception("Unknown key type: {}({})".format(key, type(key)))

        def __setitem__(self, key, value):
            if isinstance(key, int):
                assert value in [0, 1]
                self.value = self.value | (value << key)
            elif isinstance(key, slice):
                # print("__setitem: key={} value={}".format(key, value))
                self.__setslice__(key.start, key.stop, value)
            else:
                raise Exception("Unknown key type: {}({})".format(key, type(key)))

        def __delslice__(self, i, j):
            print("__delslice__")
            raise NotImplementedError()

        def __delitem__(self, key):
            print("__delitem__")
            raise NotImplementedError()

        def __setslice__(self, i, j, sequence):
            assert isinstance(i, int)
            assert isinstance(j, int)
            assert isinstance(sequence, int) or isinstance(sequence, Bit)
            if isinstance(sequence, Bit):
                assert (i - j + 1) >= sequence.size
            assert i >= j
            assert 0 <= i < self.size
            assert 0 <= j < self.size

            mask = (2 ** (i - j + 1) - 1) << j
            mask_neg = self.mask ^ mask
            self.value = self.value & mask_neg

            new_part = int(sequence) << j
            assert new_part == (new_part & mask)

            self.value = self.value | new_part

        def __index__(self):
            return self.value

        def __int__(self):
            return self.__index__()

        def __invert__(self):
            return self.value ^ self.mask

        def __repr__(self):
            if self.fmt == Bit.Fmt.Bin:
                return "{size}w0b{value:0{size}b}".format(size=self.size,
                                                          value=self.value)
            elif self.fmt == Bit.Fmt.Dec:
                return "{}w0{:d}".format(self.size, self.value)
            elif self.fmt == Bit.Fmt.Hex:
                return "{}w0x{:x}".format(self.size, self.value)

        def __eq__(self, other):
            if not isinstance(other, Bit):
                return False
            return self.size == other.size and self.value == other.value

        def set_pattern(self, value, value_width):
            assert 0 <= value < (2 ** value_width)
            for i in range(0, self.size, value_width):
                if value_width == 1:
                    self[i] = value
                else:
                    if i + value_width - 1 < self.size:
                        self[i + value_width - 1:i] = value
                    else:
                        self[self.size:i] = value

        def concat(self, other):
            assert isinstance(other, Bit)
            return Bit(self.size + other.size, (self.value << other.size) + other.value)

        def popcnt(self):
            result = 0
            for i in self:
                if i == 1:
                    result += 1
            return result


    class QueueMapping(object):
        """
        Map queue ids between
        ingress: logical port and relative queue ids
        egress: physical port and relative queue ids
        semantics and different encodings each.
        """

        class PortSpeed(IntEnum):
            """
            The value is width in bits of the interface ids within the mac
            """
            GbE_400 = -1
            GbE_200 = 0
            GbE_100 = 1
            GbE_50 = 2
            GbE_25 = 2
            GbE_10 = 2

        def generate_ports_json(self):
            ports = dict(
                PortToIf=[]
            )
            cnt = 0
            veth_cnt = 0
            for pipe in range(4):
                for mac in range(1, 9):
                    print("{}:type={}".format(
                        str(self.port_speed), self.port_speed.value))
                    for port_in_mac_id in range(2 ** (self.port_speed.value + 1)):
                        print("port_in_mac_id={}".format(port_in_mac_id))
                        pipe_id = Bit(size=2, value=pipe)
                        mac_id = Bit(size=4, value=mac)
                        port_in_mac_id = Bit(size=3, value=port_in_mac_id)
                        ports['PortToIf'].append(
                            {
                                "device_port": pipe_id.concat(mac_id).concat(port_in_mac_id).value,
                                "if": "veth{}".format(cnt),
                            },
                        )
                        cnt += 1
                        veth_cnt += 2

            import json
            return json.dumps(ports)


    TableEntry = namedtuple("TableEntry", ["keys", "action", "parameters"])

    SfcConfig = namedtuple("SfcConfig", [
        "port_speed",
        "pipes",
        "ports",
        "server_ports",
        "switch_ports",
        "queues",
        "port_speeds",
        "qlength_threshold",
        "target_queuedepth",
        "suppression_epoch_duration",
        "mirror_egress_ports",
        "tcs",
        "dscp_mapping",
        "sfc_pause_packet_dscp",
        "dscp_tc_map",
        # Debug flags
        "skip_trigger",
        "always_trigger",
        "skip_suppression",
        "ignore_suppression",
    ])


    class SfcPacketType(IntEnum):
        Unset = 0
        TNone = 1  # No SFC packet
        Data = 2  # Normal SFC data packet, SFC is enabled
        Trigger = 3  # SFC pause packet after mirroring, SFC is enabled
        Signal = 4  # SFC pause packet after SFC pause packet construction, SFC is enabled
        TcSignalEnabled = 5  # No SFC packet, but a packet on a SignalingEnabledTC


    class SfcTestHelper(ApiHelper, BfRuntimeTest):
        def __init__(self):
            self.tidp = None
            ApiHelper.__init__(self)
            BfRuntimeTest.__init__(self)
            # The following two variables are used for cleanup.
            # Tables/registers added here will be cleaned up.
            self.tables = []
            self.registers = []

            self.pipe1 = [136, 144, 152, 160, 168, 176, 184, 192]
            self.pipe2 = [264, 272, 280, 288, 296, 304, 312, 320]
            self.pipe3 = [392, 400, 408, 416, 424, 432, 440, 448]
            self.pipe4 = [8, 16, 24, 32, 40, 48, 56, 64]

            self.sfc_config = None
            self.suppression_regs = None

            self.recirculation_ports_number = [266, 274, 282, 290]
            self.recirculation_dev_ports = [6, 134, 262, 390]

            self.mau_q2idx = None
            self.tm_q2idx = None
            self.mau_egress_q2idx = None

        def setUp(self):
            print("\n")
            print("Test Setup")
            print("==========")

            ApiHelper.setUp(self)
            print("  Checking for required switch features")
            if (self.client.is_feature_enable(SWITCH_FEATURE_SFC) == 0):
                # Need to tearDown manually since self.configure()
                # calls ApiHelper.setUp()
                ApiHelper.tearDown(self)
                raise unittest.SkipTest("SFC feature not enabled, skipping")

            self.client_id = 0
            self.p4_name = "switch"  # Specialization
            self.dev = 0
            self.dev_tgt = gc.Target(device_id=self.dev, pipe_id=0xffff)
            notif = gc.Notifications(enable_learn=False)
            BfRuntimeTest.setUp(self, self.client_id, self.p4_name, notif)
            self.bfrt_info = self.interface.bfrt_info_get(self.p4_name)

            self.tidp = pd_base_tests.ThriftInterfaceDataPlane(["switch"])
            self.tidp.setUp()
            # This try-except block is required since unittest does not seem
            # to be able to handle Thrift exceptions correctly.
            try:
                self.tidp.shdl = self.tidp.conn_mgr.client_init()
            except Exception as exc:
                raise Exception("Failed to initialize ThriftInterfaceDataPlane") from exc

            self.configure()

            self.suppression_regs = dict(
                epoch_switch=self.bfrt_info.table_get(
                    'SwitchIngress.sfc_epoch_init.reg_filter_epoch'),
                banks={
                    0: {
                        'name': 'bank0',
                        0: self.bfrt_info.table_get('SwitchIngress.sfc_trigger.pause_filter_bank0_filter0'),
                        1: self.bfrt_info.table_get('SwitchIngress.sfc_trigger.pause_filter_bank0_filter1'),
                    },
                    1: {
                        'name': 'bank1',
                        0: self.bfrt_info.table_get('SwitchIngress.sfc_trigger.pause_filter_bank1_filter0'),
                        1: self.bfrt_info.table_get('SwitchIngress.sfc_trigger.pause_filter_bank1_filter1'),
                    }
                }
            )

            self.ghost_regs = dict(
                qdepth=self.bfrt_info.table_get('pipe.sfc_reg_qdepth'),
            )
            self.egress_regs = dict(
                qdepth=self.bfrt_info.table_get('SwitchEgress.sfc.reg_qdepth')
            )
            self.cpu_port_parser = self.bfrt_info.table_get(
                "recirc_port_cpu_hdr")
            self.dscp_tc_map = self.bfrt_info.table_get(
                "ingress_qos_map.dscp_tc_map")
            self.ingress_sfc_classify_sfc = self.bfrt_info.table_get(
                "SwitchIngress.sfc_prepare.classify_sfc")
            self.ingress_sfc_queue_register_idx = self.bfrt_info.table_get(
                "SwitchIngress.sfc_prepare.set_queue_register_idx")
            self.ingress_sfc_decide_suppression = self.bfrt_info.table_get(
                "SwitchIngress.sfc_trigger.decide_suppression")
            self.ingress_sfc_decide_mirroring = self.bfrt_info.table_get(
                "SwitchIngress.sfc_trigger.decide_mirroring")
            self.egress_pause_time_conversion = self.bfrt_info.table_get(
                "SwitchEgress.sfc.pause_time_conversion")
            self.egress_set_sfc_pause_dscp = self.bfrt_info.table_get(
                "SwitchEgress.sfc.set_sfc_pause_dscp")
            self.convert_to_pfc_pause = self.bfrt_info.table_get(
                "SwitchEgress.sfc_packet.convert_to_pfc_pause")

            self.suppression_epoch_duration = self.bfrt_info.table_get(
                'SwitchIngress.sfc_epoch_init.suppression_epoch_duration')

            self.mirror_cfg = self.bfrt_info.table_get("$mirror.cfg")

            self.tables.extend([
                self.cpu_port_parser,
                self.dscp_tc_map,
                self.ingress_sfc_classify_sfc,
                self.ingress_sfc_queue_register_idx,
                self.ingress_sfc_decide_suppression,
                self.ingress_sfc_decide_mirroring,
                self.egress_pause_time_conversion,
                self.egress_set_sfc_pause_dscp,
                self.convert_to_pfc_pause,
            ])

            # pprint.pprint(self.bfrt_info.table_dict.keys())
            if "SwitchGhost.sfc_init.ghost_set_register_idx_tbl" in self.bfrt_info.table_dict.keys():
                print("  Found ghost_set_register_idx_tbl")
                self.ghost_sfc_register_idx = self.bfrt_info.table_get(
                    "SwitchGhost.sfc_init.ghost_set_register_idx_tbl")
                self.ghost_check_threshold_tbl = None
                self.tables.append(self.ghost_sfc_register_idx)
            else:
                print("  Found ghost_check_threshold_tbl")
                self.ghost_sfc_register_idx = None
                self.ghost_check_threshold_tbl = self.bfrt_info.table_get(
                    "SwitchGhostNew.sfc.ghost_check_threshold_tbl")
                self.tables.append(self.ghost_check_threshold_tbl)

            # # switch.p4 tables we use but shouldn't
            # self.egress_port_mapping = self.bfrt_info.table_get("SwitchEgress.egress_port_mapping.port_mapping")
            # we need this table to set up the trust mode as TRUST DSCP.
            self.ingress_port_mapping = self.bfrt_info.table_get(
                "ingress_port_mapping")

            # self.mirror_cfg_table = self.bfrt_info.table_get("$mirror.cfg")

        def tearDown(self):
            if self.sfc_config:
                print("  Found sfc_config, resetting..")
                for port in self.sfc_config.ports:
                    self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)

            # remove the mirror entry;
            # note that this is the right way to remove the mirror entry
            # It does not work if we remove the mirror entry from bfrt mirror table
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)

            self.cleanUp()
            self.cleanup()
            ApiHelper.tearDown(self)
            BfRuntimeTest.tearDown(self)
            self.tidp.conn_mgr.client_cleanup(self.tidp.shdl)

        def cleanUp(self):
            print("\n")
            print("Table Cleanup:")
            print("==============")
            print("  Found {} tables to clean.".format(len(self.tables)))
            try:
                for t in self.tables:
                    print("  Clearing Table {}".format(t.info.name_get()))
                    keys = []
                    for (d, k) in t.entry_get(self.dev_tgt, flags=dict(from_hw=False)):
                        if k is not None:
                            keys.append(k)

                    t.entry_del(self.dev_tgt, keys)
                    # Not all tables support default entry
                    try:
                        t.default_entry_reset(self.dev_tgt)
                    except:
                        pass
            except Exception as e:
                print("Error cleaning up tables: {}".format(e))

            print("Register Cleanup:")
            print("==============")
            print("  Found {} register arrays to clean.".format(len(self.registers)))
            try:
                for r in self.registers:
                    print("  Clearing Register Array {}".format(r.info.name_get()))
                    data_rst = [gc.DataTuple(f, 0)
                                for f in r.info.data_dict.keys()]
                    keys = []
                    data = []
                    for (d, k) in r.entry_get(self.dev_tgt, flags=dict(from_hw=False)):
                        if k is not None:
                            keys.append(k)
                            data.append(r.make_data(data_rst, None))

                    r.entry_mod(self.dev_tgt, keys, data)
            except Exception as e:
                print("Error cleaning up registers: {}".format(e))

        #
        # This is a simple helper method that takes a list of entries and programs
        # them in a specified table
        #
        # Each entry is a tuple, consisting of 3 elements:
        #  key         -- a list of tuples for each element of the key
        #  action_name -- the action to use. Must use full name of the action
        #  data        -- a list (may be empty) of the tuples for each action
        #                 parameter
        #
        def programTable(self, table, entries, target=None, mod=False):
            if target is None:
                target = self.dev_tgt
            key_list = []
            data_list = []
            for k, a, d in entries:
                key_list.append(table.make_key([gc.KeyTuple(*f) for f in k]))
                data_list.append(table.make_data([gc.DataTuple(*p) for p in d], a))
            if mod:
                table.entry_mod(target, key_list, data_list)
            else:
                table.entry_add(target, key_list, data_list)

        def getEntryFromKey(self, table, key):
            key_list = []
            key_list.append(table.make_key([gc.KeyTuple(*f) for f in key]))
            for (d, k) in table.entry_get(self.dev_tgt, key_list, flags=dict(from_hw=False)):
                entry = str(d)
                entry_dict = ast.literal_eval(entry)
                del entry_dict['is_default_entry']
                action_name = entry_dict['action_name']
                print(action_name)
                del entry_dict['action_name']
                return entry_dict, action_name

        @staticmethod
        def _convert_entry(table, entry):
            key_elements = []
            if entry.keys is not None:
                for n, v in entry.keys.items():
                    # print("n={}({}), v={}({})".format(n, type(n), v, type(v)))
                    # name, value=None, mask=None
                    if isinstance(v, tuple):
                        key_elements.append(gc.KeyTuple(
                            name=n, value=v[0], mask=v[1]))
                    elif isinstance(v, dict):
                        key_elements.append(gc.KeyTuple(name=n, **v))
                    else:
                        key_elements.append(gc.KeyTuple(name=n, value=v))
            key = table.make_key(key_elements)

            data_elements = []
            if entry.parameters is not None:
                for n, v in entry.parameters.items():
                    # print("n={}, v={}".format(n, v))
                    # name, value=None, mask=None
                    if isinstance(v, tuple):
                        tmp = {v[1]: v[0]}
                        data_elements.append(gc.DataTuple(name=n, **tmp))
                    if isinstance(v, dict):
                        data_elements.append(gc.DataTuple(name=n, **v))
                    else:
                        data_elements.append(gc.DataTuple(name=n, val=v))

            data = table.make_data(data_elements, action_name=entry.action)

            return key, data

        def program_table(self, table, entries, target=None, default_entry=False, mod=False):
            start_time = time.time()
            print("Programming table {} with {} entries".format(
                table.info.name, len(entries)))
            if target is None:
                target = self.dev_tgt
            key_list = []
            data_list = []
            if default_entry and isinstance(entries, TableEntry):
                # table.set
                _, data = self._convert_entry(table, entries)
                table.default_entry_set(target, data)
            elif isinstance(entries, tuple):
                for k, a, d in entries:
                    key_list.append(table.make_key([gc.KeyTuple(*f) for f in k]))
                    data_list.append(table.make_data(
                        [gc.DataTuple(*p) for p in d], a))
                table.entry_add(target, key_list, data_list)
            elif isinstance(entries, list) and isinstance(entries[0], TableEntry):
                for entry in entries:
                    key, data = self._convert_entry(table, entry)
                    key_list.append(key)
                    data_list.append(data)

                if not mod:
                    table.entry_add(target, key_list, data_list)
                else:
                    table.entry_mod(target, key_list, data_list)
            else:
                raise Exception(
                    "Unexpected data type for entries argument: {}".format(type(entries)))
            print("  Programming the table took {}s total.".format(
                time.time() - start_time))

        def _generate_pipe_session_ids(self, sfc_config, key_dict_default,
                                       action=None, mirror_sessions=None):
            """

            :rtype: object
            """
            entry_templates = []
            for pipe, port in zip(sfc_config.pipes, sfc_config.mirror_egress_ports):
                if not action:
                    action = "SwitchIngress.sfc_trigger.do_set_mirroring"

                print("Mirror sessions: {}".format(mirror_sessions))

                if mirror_sessions is None:
                    port_handle = self.get_port_handle_from_port_num(port)
                    ing_mirror = self.add_mirror(self.device,
                                                 type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                                                 direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
                                                 egress_port_handle=port_handle)
                    self.attribute_set(
                        self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, ing_mirror)
                    session_id = self.attribute_get(
                        ing_mirror, SWITCH_MIRROR_ATTR_SESSION_ID)

                    print("  pipe {}, port {}, session_id {}, ing_mirror {}".format(
                        pipe, port, session_id, ing_mirror))
                    assert session_id is not False, "find the false session_id"
                else:
                    session_id = mirror_sessions[pipe]

                entry_templates.append(
                    TableEntry(
                        keys=dict(
                            key_dict_default,
                            pipe_id=(pipe, 0b11),
                        ),
                        action=action,
                        parameters=dict(
                            sid=session_id,
                        ))
                )

            return entry_templates

        def _configure_epoch(self, sfc_config):
            print("Configuring suppression epoch")
            print("  Set to value={}".format(sfc_config.suppression_epoch_duration))
            self.suppression_epoch_duration.default_entry_set(
                self.dev_tgt,
                self.suppression_epoch_duration.make_data(
                    [gc.DataTuple("value", sfc_config.suppression_epoch_duration)]
                ))
            data, _ = next(
                self.suppression_epoch_duration.default_entry_get(self.dev_tgt))
            assert sfc_config.suppression_epoch_duration == data.to_dict()['value']

        def _populate_cpu_port_at_parser(self, sfc_config):
            # self.cpu_port_parser.entry_del(self.dev_tgt)
            cpu_port_entries = list()
            for port in [6]:
                cpu_port_entries.append(
                    TableEntry(keys=dict(ether_type=(0x9000, 0xFFFF), port=(port, 0x1FF)),
                               action=None,
                               parameters=None
                               )
                )
            self.program_table(
                self.cpu_port_parser, cpu_port_entries)

        def _populate_dscp_tc_map(self, sfc_config):
            # set qos_group = 0
            print("Program dscp_tc_map table")
            table_entries = []
            ingress_port_mapping_table_entries = []
            qos_group = 0
            trust_dscp = 1
            res = self.ingress_port_mapping.entry_get(self.dev_tgt)
            recirculation_dev_ports = list()
            for mirror_port_handle in map(self.get_port_handle_from_port_num, self.recirculation_ports_number):
                recirculation_dev_ports.append(self.attribute_get(mirror_port_handle, SWITCH_PORT_ATTR_DEV_PORT))

            for data, key in res:
                keys = key.to_dict()
                values = data.to_dict()
                port = keys['local_md.ingress_port']['value']
                if port in recirculation_dev_ports:
                    continue
                print("ingress_port_mapping port: {}".format(port))
                exclusion_id = values['exclusion_id']
                action_name = values['action_name']
                # trust_mode =  trust_dscp #values['trust_mode']
                values['trust_mode'] = trust_dscp
                qos_group = values['qos_group']
                if action_name == 'set_port_properties':
                    del values['action_name']
                    del values['is_default_entry']
                    ingress_port_mapping_table_entries.append(
                        TableEntry(
                            keys=keys,
                            action=action_name,
                            parameters=values
                        )
                    )
            self.program_table(self.ingress_port_mapping,
                               ingress_port_mapping_table_entries, mod=True)

            for dscp in sfc_config.dscp_tc_map:
                new_entry_key = dict()
                new_entry_key['local_md.qos.group'] = qos_group
                new_entry_key['local_md.lkp.ip_tos[7:2]'] = dscp
                table_entries.append(
                    TableEntry(
                        keys=new_entry_key,
                        action="ingress_qos_map.set_ingress_tc",
                        parameters=dict(tc=sfc_config.dscp_tc_map[dscp])
                    )
                )
            self.program_table(self.dscp_tc_map, table_entries)

        def _generate_suppression_decision_table_entries(self, sfc_config, key_dict_default):
            print("Program suppression decision table")
            table_entries = []

            if not sfc_config.skip_suppression:
                table_entries.append(
                    TableEntry(
                        keys=dict(
                            key_dict_default,
                            qlength_over_threshold=(1, 0b1),
                            bank_idx=(0, 0b1)),
                        action="SwitchIngress.sfc_trigger.do_check_suppression_bank0",
                        parameters=dict()
                    )
                )
                table_entries.append(
                    TableEntry(
                        keys=dict(
                            key_dict_default,
                            qlength_over_threshold=(1, 0b1),
                            bank_idx=(1, 0b1)),
                        action="SwitchIngress.sfc_trigger.do_check_suppression_bank1",
                        parameters=dict()
                    )
                )
            return table_entries

        def _populate_suppression_decision_table(self, sfc_config, key_dict_default):
            print("Program suppression decision table")

            table_entries = self._generate_suppression_decision_table_entries(
                sfc_config,
                key_dict_default)

            if table_entries:
                self.program_table(self.ingress_sfc_decide_suppression,
                                   table_entries)

        def _verify_table_entries(self,
                                  table,
                                  expected_table_entries,
                                  result_filter=None,
                                  reg_pipes=None,
                                  counter=False,
                                  ):

            if len(expected_table_entries) > 0 and isinstance(expected_table_entries[0], TableEntry):
                print("Found TableEntry")
                expected_k_d_list = []
                for entry in expected_table_entries:
                    key, data = self._convert_entry(table, entry)
                    expected_k_d_list.append((key.to_dict(), data.to_dict()))
            else:
                print("Found dict")
                expected_k_d_list = expected_table_entries

            print("Expected table entries")
            pprint.pprint(expected_k_d_list)

            resp = table.entry_get(self.dev_tgt, flags=dict(from_hw=True))

            values_found = []
            values_missed = []
            for data, key in resp:
                keys = key.to_dict()
                values = data.to_dict()
                if counter:
                    del values['$COUNTER_SPEC_BYTES']
                    del values['$COUNTER_SPEC_PKTS']
                if result_filter:
                    result_filter(keys, values)

                entry_found = False
                for exp_k, exp_d in expected_k_d_list:
                    assert entry_found is not True
                    if exp_k == keys:
                        if not reg_pipes:
                            assert values == exp_d, "ERROR: (reg_pipes={}), key={} \n expected {}, found {}". \
                                format(reg_pipes, exp_k, exp_d, values)
                        else:
                            act_exp_d = {}
                            for k, v in exp_d.items():
                                if isinstance(v, list):
                                    act_exp_d[k] = len(reg_pipes) * v
                                else:
                                    act_exp_d[k] = v
                            assert values == act_exp_d, "ERROR: (reg_pipes={}) \n    expected: {}\n    found: {}" \
                                .format(reg_pipes, act_exp_d, values)
                        values_found.append(exp_k)
                        entry_found = True
                        break

                if not entry_found and not reg_pipes:
                    values_missed.append((keys, values))

            print("Results:")
            print("  Verified entries:")
            pprint.pprint(values_found)
            print("  Missed entries:")
            pprint.pprint(values_missed)

            assert len(values_found) == len(expected_k_d_list), \
                "ERROR: Did not find the expected number of entries, expected {}," \
                "found {}".format(len(expected_k_d_list), len(values_found))
            assert len(values_missed) == 0, \
                "ERROR: found {} superfluous entries: {}".format(
                    len(values_missed),
                    pprint.pformat(values_missed)
                )

        def verify_recirc_pvs(self, enabled):
            entry_template = ({'ether_type': {'mask': 65535, 'value': 0x9000},
                               'port': {'mask': 511, 'value': 6}},
                              {'action_name': None, 'is_default_entry': False},
                              )

            expected_table_entries = []
            if enabled:
                pprint.pprint(self.recirculation_dev_ports)
                for recirc_dev_port in self.recirculation_dev_ports:
                    new_entry = copy.deepcopy(entry_template)
                    new_entry[0]['port']['value'] = recirc_dev_port
                    expected_table_entries.append(new_entry)

            self._verify_table_entries(self.cpu_port_parser,
                                       expected_table_entries,
                                       counter=False)

        def verify_qstat_config(self, sfc_config, enabled):
            print("Verifying per-pipe Qstat configuration:")

            results = dict()
            expected_results = dict()
            for pipe in sfc_config.pipes:
                results[pipe] = self.tidp.tm.tm_qstat_report_mode_get(0, pipe)
                expected_results[pipe] = enabled

            assert results == expected_results, "Expected: {}, found: {}".format(
                expected_results, results)

        def verify_suppression_decision_table(self, sfc_config):

            if sfc_config:
                print("  Found sfc_config: {}".format(pprint.pformat(sfc_config)))
                key_dict_default = self._generate_default_entries()

                expected_table_entries = self._generate_suppression_decision_table_entries(
                    sfc_config,
                    key_dict_default)
            else:
                print("  No sfc_config, expect empty tables.")
                expected_table_entries = []

            self._verify_table_entries(self.ingress_sfc_decide_suppression,
                                       expected_table_entries,
                                       counter=True)

        def _generate_mirror_decision_table_entries(self, sfc_config,
                                                    key_dict_default, mirror_sessions=None):
            table_entries = []
            # Non-stat entries start here
            if sfc_config.always_trigger:
                entry_templates = self._generate_pipe_session_ids(
                    sfc_config, key_dict_default, mirror_sessions=mirror_sessions)
                for entry in entry_templates:
                    entry.keys['qlength_over_threshold'] = (0, 0b0)
                    table_entries.append(entry)
            else:
                if sfc_config.ignore_suppression:
                    entry_templates = self._generate_pipe_session_ids(
                        sfc_config, key_dict_default, mirror_sessions=mirror_sessions)
                    for entry in entry_templates:
                        new_entry = copy.deepcopy(entry)
                        new_entry.keys['qlength_over_threshold'] = (1, 0b1)
                        new_entry.keys['fr0'] = (0, 0b0)
                        new_entry.keys['fr1'] = (0, 0b0)
                        table_entries.append(new_entry)
                else:
                    new_entry = TableEntry(
                        keys=dict(
                            key_dict_default,
                            pipe_id=(0b00, 0b00),
                            qlength_over_threshold=(1, 0b1),
                            fr0=(1, 0b1),
                            fr1=(1, 0b1),
                        ),
                        action="SwitchIngress.sfc_trigger.do_update_stats_mirror",
                        parameters=dict()
                    )
                    new_entry.keys['$MATCH_PRIORITY'] = 1
                    table_entries.append(new_entry)

                    entry_templates = self._generate_pipe_session_ids(
                        sfc_config, key_dict_default, mirror_sessions=mirror_sessions)
                    for entry in entry_templates:
                        entry.keys['qlength_over_threshold'] = (1, 0b1)
                        entry.keys['fr0'] = (0, 0b0)
                        entry.keys['fr1'] = (0, 0b0)
                        table_entries.append(entry)

            return table_entries

        def _populate_mirror_decision_table(self, sfc_config, key_dict_default):
            print("Program per-pipe mirroring decision table")
            table_entries = self._generate_mirror_decision_table_entries(
                sfc_config,
                key_dict_default)

            if table_entries:
                self.program_table(
                    self.ingress_sfc_decide_mirroring, table_entries)

        def get_and_verify_mirror_sessions(self):
            table = self.mirror_cfg

            entry_template = ({'$sid': {'value': 3}},
                              {'$c2c_cfg_flag': False,
                               '$copy_to_cpu': False,
                               '$direction': 'INGRESS',
                               '$dod_cfg_flag': False,
                               '$egress_port_queue': 0,
                               '$epipe_cfg_flag': False,
                               '$hash_cfg_flag': False,
                               '$hash_cfg_flag_p': False,
                               '$icos_cfg_flag': False,
                               '$icos_for_copy_to_cpu': 0,
                               '$ingress_cos': 0,
                               '$level1_mcast_hash': 0,
                               '$level2_mcast_hash': 0,
                               '$max_pkt_len': 64,
                               '$mc_cfg_flag': False,
                               '$mcast_grp_a': 0,
                               '$mcast_grp_a_valid': False,
                               '$mcast_grp_b': 0,
                               '$mcast_grp_b_valid': False,
                               '$mcast_l1_xid': 57328,
                               '$mcast_l2_xid': 15219,
                               '$mcast_rid': 32719,
                               '$packet_color': 'GREEN',
                               '$session_enable': True,
                               '$session_priority': True,
                               '$ucast_egress_port': 262,
                               '$ucast_egress_port_valid': True,
                               'action_name': '$normal',
                               'is_default_entry': False})

            resp = table.entry_get(self.dev_tgt, flags=dict(from_hw=True))

            sfc_mirror_sessions = dict()
            for data, key in resp:
                keys = key.to_dict()
                values = data.to_dict()

                # pprint.pprint(values)

                if values['$direction'] == 'INGRESS' \
                        and values['$max_pkt_len'] == 64 \
                        and values['$ucast_egress_port'] in self.recirculation_dev_ports:
                    # print("  Found a candidate entry")
                    exp_data = copy.deepcopy(entry_template[1])
                    cmp_data = copy.deepcopy(values)
                    del exp_data['$ucast_egress_port']
                    del exp_data['$mcast_l1_xid']
                    del exp_data['$mcast_l2_xid']
                    del exp_data['$mcast_rid']

                    del cmp_data['$ucast_egress_port']
                    del cmp_data['$mcast_l1_xid']
                    del cmp_data['$mcast_l2_xid']
                    del cmp_data['$mcast_rid']

                    # pprint.pprint(self.expect_dict_diff(cmp_data, exp_data))

                    if exp_data == cmp_data:
                        print("  MATCH FOUND!")
                        pipe_id = values['$ucast_egress_port'] >> 7
                        sfc_mirror_sessions[pipe_id] = keys['$sid']['value']
                    else:
                        # pass
                        print("  NO MATCH FOUND!")
                        print("======DIFF=========")
                        pprint.pprint(self.expect_dict_diff(cmp_data, exp_data))
                        # pprint.pprint(cmp_data)

            return sfc_mirror_sessions

        def verify_mirror_sessions_and_decision_table(self, sfc_config):
            mirror_sessions = self.get_and_verify_mirror_sessions()

            print("Verify mirror sessions and decision table")

            if sfc_config:
                print("  Found sfc_config: {}".format(pprint.pformat(sfc_config)))
                assert len(mirror_sessions) == len(sfc_config.pipes), \
                    "Number of SFC mirror sessions ({}) does not match the number of " \
                    "sfc_config.pipes ({}).".format(mirror_sessions, sfc_config.pipes)

                key_dict_default = self._generate_default_entries()

                expected_table_entries = self._generate_mirror_decision_table_entries(
                    sfc_config,
                    key_dict_default,
                    mirror_sessions=mirror_sessions)
            else:
                print("  No sfc_config, expect empty tables.")

                assert mirror_sessions == dict(), "Expected no matching mirror entries, found: {}".format(
                    pprint.pformat(mirror_sessions))

                expected_table_entries = []

            # Since we are the only user of mirroring at this point,
            # assume that all INGRESS mirror sessions that match our patterns
            # are for SFC.

            self._verify_table_entries(
                self.ingress_sfc_decide_mirroring,
                expected_table_entries,
                counter=True)

        def _generate_classify_signal_entries(self, sfc_config):
            table_entries = []
            if sfc_config.sfc_pause_packet_dscp is not None:
                table_entries.append(
                    TableEntry(keys=dict(tc=sfc_config.dscp_tc_map[sfc_config.sfc_pause_packet_dscp],
                                         bth=0,
                                         ethertype=0x800,
                                         routed=1
                                         ),
                               action="SwitchIngress.sfc_prepare.do_set_sfc",
                               parameters=dict(
                                   sfc_packet_type=SfcPacketType.TcSignalEnabled.value,
                               ))
                )

            return table_entries

        def verify_classify_signaling(self, sfc_config, enabled=True):
            expected_table_entries = []
            if enabled:
                expected_table_entries = self._generate_classify_signal_entries(sfc_config)

            self._verify_table_entries(self.ingress_sfc_classify_sfc,
                                       expected_table_entries,
                                       counter=True)

        def _generate_classify_data_entries(self, sfc_config):
            print("Program SFC detection table")
            table_entries = []
            for tc in sfc_config.tcs:
                table_entries.append(
                    TableEntry(keys=dict(tc=tc,
                                         bth=1,
                                         ethertype=0x800,
                                         routed=1
                                         ),
                               action="SwitchIngress.sfc_prepare.do_set_sfc",
                               parameters=dict(
                                   sfc_packet_type=SfcPacketType.Data.value,
                               ))
                )

            return table_entries

        def _populate_sfc_detection_table(self, sfc_config):
            table_entries = self._generate_classify_data_entries(sfc_config)
            table_entries.extend(self._generate_classify_signal_entries(sfc_config))

            if table_entries:
                self.program_table(self.ingress_sfc_classify_sfc, table_entries)

        def verify_classify_data(self, sfc_config, enabled=True):
            expected_table_entries = []
            if enabled:
                expected_table_entries = self._generate_classify_data_entries(sfc_config)

            pprint.pprint(expected_table_entries)

            self._verify_table_entries(self.ingress_sfc_classify_sfc,
                                       expected_table_entries,
                                       counter=True)

        def _generate_qid_mapping_entries(self, sfc_config):
            print("Map tm pipe,qid to port_qid_idx and qlength thresholds in ghost and ingress")
            print("  ports={}, queues={}".format(
                sfc_config.ports, sfc_config.queues))

            self.mau_q2idx = dict()
            self.tm_q2idx = dict()
            self.mau_egress_q2idx = dict()

            ghost_table_entries = []
            ingress_table_entries = []
            queues_qstat_enable = []
            # Ghost & Ingress
            queue_register_idx = 1
            for p in sfc_config.ports:
                for q in sfc_config.queues:
                    qid_mau = QidMau(port_id_9b=p, queue_id_7b=q)
                    # qid_tm = Tf2QueueMapping.mau2tm(port_id_9b=p, queue_id_7b=q)
                    # pprint.pprint(dir(self.dataplane))
                    qid_drv = self.tidp.tm.tm_get_port_pipe_phys_q(0, p, q)
                    qid_tm = QidTm(pipe_id_2b=qid_drv.pipe,
                                   queue_id_11b=qid_drv.phys_queue)
                    q_idx = (qid_mau.port_id_9b << 7) | qid_mau.queue_id_7b

                    qlength_threshold_16bit = int(
                        Bit(size=19, value=sfc_config.qlength_threshold)[18:3])

                    self.mau_q2idx[qid_mau] = q_idx
                    self.tm_q2idx[qid_tm] = q_idx
                    self.mau_egress_q2idx[qid_mau] = queue_register_idx
                    ghost_table_entries.append(
                        TableEntry(keys=dict(tm_pipe_id=qid_tm.pipe_id_2b,
                                             tm_absqid=qid_tm.queue_id_11b,
                                             # tm_mac_id=qid_tm_new.tm_mac_id_4b,
                                             # tm_mac_q_id=qid_tm_new.tm_mac_q_id_7b
                                             ),
                                   action="SwitchGhost.sfc_init.do_check_threshold_set_q_idx",
                                   parameters=dict(
                                       ingress_port_queue_idx=q_idx,
                                       qdepth_threshold=qlength_threshold_16bit,
                                   ))
                    )

                    ingress_table_entries.append(
                        TableEntry(keys=dict(port=qid_mau.port_id_9b, qid=qid_mau.queue_id_7b),
                                   action="SwitchIngress.sfc_prepare.do_set_queue_register_idx",
                                   parameters=dict(
                                       queue_register_idx=queue_register_idx)
                                   )
                    )
                    queues_qstat_enable.append(qid_mau)
                    queue_register_idx += 1

            return ghost_table_entries, ingress_table_entries, queues_qstat_enable

        def _populate_qid_mapping(self, sfc_config):

            ghost_table_entries, ingress_table_entries, _ = self._generate_qid_mapping_entries(sfc_config)

            if ghost_table_entries and ingress_table_entries:
                self.program_table(self.ghost_sfc_register_idx,
                                   ghost_table_entries)
                self.program_table(
                    self.ingress_sfc_queue_register_idx, ingress_table_entries)

        def verify_data_ghost(self, sfc_config, enabled=True):

            ghost_table_entries, _, queues_qstat_enable = \
                self._generate_qid_mapping_entries(sfc_config)

            if not enabled:
                ghost_table_entries = []

            # pprint.pprint(ghost_table_entries)
            # pprint.pprint(self.ghost_sfc_register_idx)

            def result_filter(_, data):
                del data['SwitchGhost.sfc_init.reg_ghost_counter.f1']

            self._verify_table_entries(self.ghost_sfc_register_idx,
                                       ghost_table_entries,
                                       counter=False,
                                       result_filter=result_filter)

            qstat_results = dict()
            for qid in queues_qstat_enable:
                qstat_results[qid] = self.tidp.tm.tm_q_visible_get(0, qid.port_id_9b, qid.queue_id_7b)

            pprint.pprint(qstat_results)
            if enabled:
                assert all(qstat_results.values()), "Qstat reporting is not enabled on all queues: {}".format(
                    pprint.pformat(qstat_results)
                )
            else:
                assert not any(qstat_results.values()), "Qstat reporting is not disabled on all queues: {}".format(
                    pprint.pformat(qstat_results)
                )

        def verify_data_ingress(self, sfc_config, enabled=True):

            _, ingress_table_entries, _ = \
                self._generate_qid_mapping_entries(sfc_config)

            if not enabled:
                ingress_table_entries = []

            pprint.pprint(ingress_table_entries)
            pprint.pprint(self.ingress_sfc_queue_register_idx)

            self._verify_table_entries(self.ingress_sfc_queue_register_idx,
                                       ingress_table_entries,
                                       counter=False)

        def _populate_qid_mapping_new(self, sfc_config):
            print("Map tm pipe,qid to port_qid_idx and qlength thresholds in ghost and ingress")
            print("  ports={}, queues={}".format(
                sfc_config.ports, sfc_config.queues))

            self.mau_q2idx = dict()
            self.tm_q2idx = dict()
            self.mau_egress_q2idx = dict()

            ghost_table_entries = []
            ingress_table_entries = []
            # Ghost & Ingress
            queue_register_idx = 1
            for p in sfc_config.ports:
                for q in sfc_config.queues:
                    qid_mau = QidMau(port_id_9b=p, queue_id_7b=q)
                    qid_drv = self.tidp.tm.tm_get_port_pipe_phys_q(0, p, q)
                    qid_tm = QidTm(pipe_id_2b=qid_drv.pipe,
                                   queue_id_11b=qid_drv.phys_queue)
                    q_idx = (qid_mau.port_id_9b << 7) | qid_mau.queue_id_7b

                    self.mau_q2idx[qid_mau] = q_idx
                self.tm_q2idx[qid_tm] = q_idx
                self.mau_egress_q2idx[qid_mau] = queue_register_idx
                ghost_table_entries.append(
                    TableEntry(keys=dict(tm_pipe_id=qid_tm.pipe_id_2b,
                                         tm_qid=qid_tm.queue_id_11b,
                                         qdepth=dict(
                                             low=0, high=sfc_config.qlength_threshold),
                                         ),
                               action="SwitchGhostNew.sfc.do_set_under_threshold",
                               parameters=dict(ingress_port_queue_idx=q_idx))
                )
                ghost_table_entries.append(
                    TableEntry(keys=dict(tm_pipe_id=qid_drv.pipe,
                                         tm_qid=qid_drv.phys_queue,
                                         qdepth=dict(low=sfc_config.qlength_threshold,
                                                     high=int(Bit(size=18, value=(1, 1))))
                                         ),
                               action="SwitchGhostNew.sfc.do_set_over_threshold",
                               parameters=dict(ingress_port_queue_idx=q_idx))
                )

                ingress_table_entries.append(
                    TableEntry(keys=dict(port=qid_mau.port_id_9b, qid=qid_mau.queue_id_7b),
                               action="SwitchIngress.sfc_prepare.do_set_queue_register_idx",
                               parameters=dict(
                                   queue_register_idx=queue_register_idx)
                               )
                )
                queue_register_idx += 1

            if ghost_table_entries and ingress_table_entries:
                self.program_table(
                    self.ghost_check_threshold_tbl, ghost_table_entries)
                self.program_table(
                    self.ingress_sfc_queue_register_idx, ingress_table_entries)

        def _generate_egress_qdepth_regs_entries(self, sfc_config, qdepth_drain_cells=0):
            """
            Set values for SwitchEgress.sfc.reg_qdepth. The qdepth_drain_cells values will
            only survive the next passing packet if the value is > 2^31. The target_qdepth
            value is always taken from sfc_config.

            :param sfc_config: A SfcConfig
            :param egress_regs: BfRtInfo table object
            :param qdepth_drain_cells: Value to be written to qdepth_drain_cells. To make it stick write
                                       effective_value + 2^31 to the field.
            """
            table_entries = []
            for p in sfc_config.ports:
                for q in sfc_config.queues:
                    qid_mau = QidMau(port_id_9b=p, queue_id_7b=q)
                    table_entries.append(
                        TableEntry(keys={'$REGISTER_INDEX': self.mau_egress_q2idx[qid_mau]},
                                   action=None,
                                   parameters={
                                       'SwitchEgress.sfc.reg_qdepth.qdepth_drain_cells': qdepth_drain_cells,
                                       'SwitchEgress.sfc.reg_qdepth.target_qdepth': sfc_config.qlength_threshold,
                                   }
                                   )
                    )
                    print("  Adding table entry for {}:{}: {}".format(
                        p, q, pprint.pformat(table_entries[-1])))

            return table_entries

        def _populate_egress_threshold_reg(self, sfc_config, qdepth_drain_cells=0):
            egress_qdepth_table_entries = self._generate_egress_qdepth_regs_entries(
                sfc_config,
                qdepth_drain_cells=qdepth_drain_cells)
            self.program_table(self.egress_regs['qdepth'],
                               egress_qdepth_table_entries, mod=True)

        def verify_egress_threshold_reg(self, sfc_config,
                                        qdepth_drain_cells=0,
                                        enabled=True):
            if enabled:
                expected_table_entries = self._generate_egress_qdepth_regs_entries(
                    sfc_config,
                    qdepth_drain_cells=qdepth_drain_cells)
            else:
                expected_table_entries = []

            self._verify_table_entries(self.egress_regs['qdepth'],
                                       expected_table_entries,
                                       counter=False,
                                       reg_pipes=sfc_config.pipes)

        def _generate_time_conversion_entries(self, sfc_config):
            actions_signal = {
                PortSpeed.GbE_25: "SwitchEgress.sfc.do_calc_pause_to_pfc_time_25g",
                PortSpeed.GbE_50: "SwitchEgress.sfc.do_calc_pause_to_pfc_time_50g",
                PortSpeed.GbE_100: "SwitchEgress.sfc.do_calc_pause_to_pfc_time_100g",
                'None': "SwitchEgress.sfc.do_time_conversion_count",
            }
            actions_trigger = {
                PortSpeed.GbE_25: "SwitchEgress.sfc.do_calc_cells_to_pause_25g",
                PortSpeed.GbE_50: "SwitchEgress.sfc.do_calc_cells_to_pause_50g",
                PortSpeed.GbE_100: "SwitchEgress.sfc.do_calc_cells_to_pause_100g",
                'None': "SwitchEgress.sfc.do_time_conversion_count",
            }

            # Max pause durations
            # * 25GBE: 48 = 32 + 16; the max pause_time_us to avoid overflow is 1365; for safety, 1200;
            # * 50GBE: 92 = 64 + 32; the max pause_time_us to avoid overflow is 712; for safety, 700;
            # * 100GBE: 196 = 128 + 64; the max pause_time_us to avoid overflow is 334; for safety, 320;
            pause_duration_max = {
                PortSpeed.GbE_25: 1200,
                PortSpeed.GbE_50: 700,
                PortSpeed.GbE_100: 320
            }
            # Egress
            table_entries = []
            # print("mau_egress_q2idx len {}".format(self.mau_egress_q2idx))
            for ingress_queue_spec, queue_register_idx in self.mau_egress_q2idx.items():
                print("queue_register_idx {}, port {}".format(queue_register_idx,
                                                              ingress_queue_spec.port_id_9b))
                if ingress_queue_spec.port_id_9b in sfc_config.server_ports:
                    link_to_type = LinkToType.Server
                elif ingress_queue_spec.port_id_9b in sfc_config.switch_ports:
                    link_to_type = LinkToType.Switch
                else:
                    continue

                # for sfc_type in [SfcPacketType.Signal, SfcPacketType.Trigger]:
                for sfc_type in SfcPacketType:
                    if sfc_type is SfcPacketType.Unset:
                        continue
                    elif sfc_type is SfcPacketType.Signal:
                        action = actions_signal[sfc_config.port_speeds[ingress_queue_spec.port_id_9b]]
                        parameters = dict(_link_to_type=link_to_type)

                        if sfc_config.port_speeds[ingress_queue_spec.port_id_9b] is not None:
                            new_entry_key = dict(
                                sfc_type=sfc_type.value,
                                queue_register_index=queue_register_idx,
                                pause_duration_us=dict(
                                    low=0,
                                    high=pause_duration_max[sfc_config.port_speeds[ingress_queue_spec.port_id_9b]] - 1)
                            )
                            # new_entry_key['$MATCH_PRIORITY'] = 1
                            table_entries.append(TableEntry(
                                keys=new_entry_key,
                                action=action,
                                parameters=parameters
                            ))

                            action = "SwitchEgress.sfc.do_cal_pause_to_pfc_time_overflow_conversion"
                            parameters = dict(_link_to_type=link_to_type)
                            new_entry_key = dict(
                                sfc_type=sfc_type.value,
                                queue_register_index=queue_register_idx,
                                pause_duration_us=dict(
                                    low=pause_duration_max[sfc_config.port_speeds[ingress_queue_spec.port_id_9b]],
                                    high=0xffff)
                            )
                            # new_entry_key['$MATCH_PRIORITY'] = 1
                            table_entries.append(TableEntry(
                                keys=new_entry_key,
                                action=action,
                                parameters=parameters
                            ))
                        else:
                            new_entry_key = dict(
                                sfc_type=sfc_type.value,
                                queue_register_index=queue_register_idx,
                                pause_duration_us=dict(low=0, high=0xffff)
                            )
                            # new_entry_key['$MATCH_PRIORITY'] = 1
                            table_entries.append(TableEntry(
                                keys=new_entry_key,
                                action=action,
                                parameters=parameters
                            ))
                    elif sfc_type == SfcPacketType.Trigger:
                        action = actions_trigger[sfc_config.port_speeds[ingress_queue_spec.port_id_9b]]
                        # parameters = dict(_sfc_pause_dscp=(
                        #         sfc_config.sfc_pause_packet_dscp << 2))
                        parameters = dict()
                        new_entry_key = dict(
                            sfc_type=sfc_type.value,
                            queue_register_index=queue_register_idx,
                            pause_duration_us=dict(low=0, high=0xffff)
                        )
                        # new_entry_key['$MATCH_PRIORITY'] = 1
                        table_entries.append(TableEntry(
                            keys=new_entry_key,
                            action=action,
                            parameters=parameters
                        ))
                    else:
                        print("Adding stats entry for sfc_type '{}'".format(sfc_type))
                        new_entry_key = dict(
                            sfc_type=sfc_type.value,
                            queue_register_index=queue_register_idx,
                            pause_duration_us=dict(low=0, high=0xffff)
                        )
                        table_entries.append(TableEntry(
                            keys=new_entry_key,
                            action="SwitchEgress.sfc.do_time_conversion_count",
                            parameters=None
                        ))

            return table_entries

        def _populate_time_conversion_table(self, sfc_config):
            table_entries = self._generate_time_conversion_entries(sfc_config)
            self.program_table(self.egress_pause_time_conversion, table_entries)

        def verify_time_conversion_table(self, sfc_config, enabled=True):
            expected_table_entries = []
            if enabled:
                expected_table_entries = self._generate_time_conversion_entries(sfc_config)

            def result_filter(key, _):
                del key['$MATCH_PRIORITY']

            self._verify_table_entries(self.egress_pause_time_conversion,
                                       expected_table_entries,
                                       counter=True,
                                       result_filter=result_filter)

        def _generate_set_sfc_pause_dscp_table(self, sfc_config, enable=True):
            if enable:
                action = "SwitchEgress.sfc.do_set_sfc_pause_dscp"
                parameters = dict(_sfc_pause_dscp=(sfc_config.sfc_pause_packet_dscp << 2))
            else:
                action = "NoAction"
                parameters = dict()

            table_entries = [
                TableEntry(keys=None,
                           action=action,
                           parameters=parameters)
            ]

            return table_entries

        def _populate_set_sfc_pause_dscp_table(self, sfc_config):
            table_entries = self._generate_set_sfc_pause_dscp_table(sfc_config)

            self.program_table(self.egress_set_sfc_pause_dscp,
                               table_entries[0],
                               default_entry=True)

        def verify_set_sfc_pause_dscp_table(self, sfc_config, enable=True):
            expected_table_entries = self._generate_set_sfc_pause_dscp_table(
                sfc_config,
                enable=enable)

            self._verify_table_entries(self.egress_set_sfc_pause_dscp,
                                       expected_table_entries)

        def _generate_convert_to_sfc_pause_entries(self, sfc_config):
            table_entries = []
            for port in sfc_config.ports:
                if port not in sfc_config.server_ports:
                    continue
                for dscp, pfc_prio_enable_bitmap in sfc_config.dscp_mapping.items():
                    table_entries.append(
                        TableEntry(keys=dict(egress_port=(port, 0x1ff),
                                             pause_dscp=(dscp, 0xff)),
                                   action="SwitchEgress.sfc_packet.do_convert_to_pfc_pause",
                                   parameters=dict(pfc_prio_enable_bitmap=pfc_prio_enable_bitmap)
                                   )
                    )

            return table_entries

        def _populate_convert_to_sfc_pause_table(self, sfc_config):
            table_entries = self._generate_convert_to_sfc_pause_entries(sfc_config)

            if table_entries:
                self.program_table(self.convert_to_pfc_pause,
                                   table_entries)

        def verify_convert_to_sfc_pause_table(self, sfc_config, enabled=True):
            expected_table_entries = []
            if enabled:
                expected_table_entries = self._generate_convert_to_sfc_pause_entries(sfc_config)

            print("verify_convert_to_sfc_pause_table")
            pprint.pprint(expected_table_entries)

            def result_filter(key, _):
                del key['$MATCH_PRIORITY']

            self._verify_table_entries(self.convert_to_pfc_pause,
                                       expected_table_entries,
                                       result_filter=result_filter)

        def _generate_default_entries(self):
            key_dict_default = dict(
                sfc_type=(SfcPacketType.Data.value, 0b111),
                mirror_type=(0, 0b1),
                drop_reason=(0, 0b1),
            )
            key_dict_default['$MATCH_PRIORITY'] = 4

            return key_dict_default

        def init_sfc(self, sfc_config):
            try:
                # self.queue_mapping = QueueMapping(sfc_config.port_speed)
                key_dict_default = self._generate_default_entries()

                try:
                    # When we separate the dscps for SFC trigger packet and SFC signal packets,
                    # we expect they are mapped to different TCs.
                    # Thus, we need change the ingress_port_mapping table to make the trust_mode to be TRUST_DSCP mode
                    # and add the entry to the dscp_tc_mapping table.
                    self._configure_epoch(sfc_config)
                    self._populate_cpu_port_at_parser(sfc_config)
                    self._populate_dscp_tc_map(sfc_config)
                    self._populate_suppression_decision_table(
                        sfc_config, key_dict_default)
                    self._populate_mirror_decision_table(
                        sfc_config, key_dict_default)
                    self._populate_sfc_detection_table(sfc_config)
                    if self.ghost_sfc_register_idx:
                        self._populate_qid_mapping(sfc_config)
                    elif self.ghost_check_threshold_tbl:
                        self._populate_qid_mapping_new(sfc_config)
                    else:
                        raise Exception(
                            "Could not identify ghost thread control style.")
                    self._populate_egress_threshold_reg(sfc_config)
                    self._populate_time_conversion_table(sfc_config)
                    self._populate_set_sfc_pause_dscp_table(sfc_config)
                    self._populate_convert_to_sfc_pause_table(sfc_config)

                except Exception as e:
                    import traceback
                    traceback.print_exc(file=sys.stdout)
                    raise Exception("init_sfc failed: {}".format(e))
            except Exception as e:
                self.fail("init failed: {}".format(e))

        def init_sfc_stats(self):
            try:
                # Ingress
                table_entries = []
                for sfc_type in SfcPacketType:
                    new_entry_key = dict(
                        sfc_type=(sfc_type.value, 0b111),
                        mirror_type=(0, 0),
                        drop_reason=(0, 0),
                        pipe_id=(0, 0),
                        qlength_over_threshold=(0, 0),
                        fr0=(0, 0),
                        fr1=(0, 0),
                    )
                    new_entry_key['$MATCH_PRIORITY'] = self.ingress_sfc_decide_mirroring.info.size - 1
                    table_entries.append(TableEntry(
                        keys=new_entry_key,
                        action="SwitchIngress.sfc_trigger.do_update_stats_mirror",
                        parameters=None
                    ))
                self.program_table(
                    self.ingress_sfc_decide_mirroring, table_entries)

                # Egress
                table_entries = []
                for sfc_type in SfcPacketType:
                    new_entry_key = dict(
                        sfc_type=sfc_type.value,
                        queue_register_index=0,
                    )
                    new_entry_key['$MATCH_PRIORITY'] = self.egress_pause_time_conversion.info.size - 1
                    table_entries.append(TableEntry(
                        keys=new_entry_key,
                        action="SwitchEgress.sfc.do_time_conversion_count",
                        parameters=None
                    ))
                self.program_table(
                    self.egress_pause_time_conversion, table_entries)
            except Exception as e:
                self.fail("init failed: {}".format(e))

        def get_sfc_counters(self):
            def read_sfc_type_cntr(dc_tbl):
                dc_tbl.operations_execute(self.dev_tgt, 'SyncCounters')
                resp = dc_tbl.entry_get(self.dev_tgt, flags=dict(from_hw=False))
                result = {}
                for data, key in resp:
                    keys = key.to_dict()
                    values = data.to_dict()
                    result_val = result.setdefault(SfcPacketType(
                        keys['sfc_type']['value']), dict(pkt=0, bytes=0))
                    result_val['pkt'] += values['$COUNTER_SPEC_PKTS']
                    result_val['bytes'] += values['$COUNTER_SPEC_BYTES']
                return result

            def read_qid_cntr(dc_tbl):
                dc_tbl.operations_execute(self.dev_tgt, 'SyncRegisters')
                resp = dc_tbl.entry_get(self.dev_tgt, flags=dict(from_hw=False))
                result = {}
                for data, key in resp:
                    keys = key.to_dict()
                    values = data.to_dict()
                    queue_id_tm = QidTm(pipe_id_2b=keys['tm_pipe_id']['value'],
                                        queue_id_11b=keys['tm_absqid']['value'])
                    result_val = result.setdefault(queue_id_tm, dict(msgs=0))
                    result_val['msgs'] = values['SwitchGhost.sfc_init.reg_ghost_counter.f1'][0]
                    for p, p_val in enumerate(values['SwitchGhost.sfc_init.reg_ghost_counter.f1']):
                        if not p_val == result_val['msgs']:
                            print("Ghost message imbalance!")
                return result

            ingress = read_sfc_type_cntr(self.ingress_sfc_decide_mirroring)
            egress = read_sfc_type_cntr(self.egress_pause_time_conversion)
            if self.ghost_sfc_register_idx:
                ghost = read_qid_cntr(self.ghost_sfc_register_idx)
            else:
                ghost = {}
            return dict(
                ingress=ingress,
                egress=egress,
                ghost=ghost,
            )

        @staticmethod
        def expect_dict_diff(val, exp):
            """
            All keys in val must be in exp.
            :param val:
            :param exp:
            :return:
            """

            stack = [([], copy.deepcopy(val))]
            result = {}

            if val is None or exp is None:
                return result

            def path_setdefault(element_path):
                r = result
                for pe in element_path:
                    r = r.setdefault(pe, dict())
                return r

            def cmp_val(element_path):
                v = val
                e = exp

                last_p = element_path[-1]
                for p in element_path[:-1]:
                    v = v[p]
                    if p in e:
                        e = e[p]
                    else:
                        break
                else:
                    if last_p in e and not e[last_p] == v[last_p]:
                        r = path_setdefault(element_path[:-1])
                        r[last_p] = dict(
                            expected=e[last_p],
                            received=v[last_p]
                        )

            while stack:
                path, element = stack.pop()
                if isinstance(element, dict):
                    stack.extend(
                        map(lambda e: (path + [e[0]], e[1]), element.items()))
                else:
                    cmp_val(path)

            return result

        def sfc_type_cnt_assert(self, before_cnt, ingress, egress, ghost={}):
            print("  checking SFC packet counters")
            t_cnt = self.diff_counters(before_cnt, self.get_sfc_counters())
            diff_cnt = self.expect_dict_diff(t_cnt, dict(ingress=ingress,
                                                         egress=egress,
                                                         ghost=ghost))
            assert len(diff_cnt) == 0, pprint.pformat(diff_cnt)
            return diff_cnt

        def reset_sfc_type_counter(self):
            """
            I am pretty sure that this function should be able to reset the counter table.
            Unfortunately, it does not, and I have no idea why:
            BfruntimeReadWriteRpcException: Error(s):
            * At index 0: INVALID_ARGUMENT, 'Table Entry modify failed table:pipe.SwitchIngress.sfc_stats.count_by_sfc_type Invalid arguments'
            * At index 1: INVALID_ARGUMENT, 'Table Entry modify failed table:pipe.SwitchIngress.sfc_stats.count_by_sfc_type Invalid arguments'
            * At index 2: INVALID_ARGUMENT, 'Table Entry modify failed table:pipe.SwitchIngress.sfc_stats.count_by_sfc_type Invalid arguments'
            * At index 3: INVALID_ARGUMENT, 'Table Entry modify failed table:pipe.SwitchIngress.sfc_stats.count_by_sfc_type Invalid arguments'
            * At index 4: INVALID_ARGUMENT, 'Table Entry modify failed table:pipe.SwitchIngress.sfc_stats.count_by_sfc_type Invalid arguments'
            * At index 5: INVALID_ARGUMENT, 'Table Entry modify failed table:pipe.SwitchIngress.sfc_stats.count_by_sfc_type Invalid arguments'
            :return:
            """
            keys = []
            data = []

            # res = self.ingress_sfc_type_cnt.entry_get(self.dev_tgt)
            for tid in range(6):
                keys.append(self.ingress_sfc_type_cnt.make_key(
                    [
                        gc.KeyTuple('$MATCH_PRIORITY', value=tid),
                        gc.KeyTuple('sfc_type', value=tid, mask=7)
                    ])
                )
                data.append(self.ingress_sfc_type_cnt.make_data(
                    [
                        gc.DataTuple('$COUNTER_SPEC_BYTES', 0),
                        gc.DataTuple('$COUNTER_SPEC_PKTS', 0)
                    ],
                    'SwitchIngress.sfc_stats.count')
                )

            self.ingress_sfc_type_cnt.entry_mod(self.dev_tgt, keys, data)

        @staticmethod
        def diff_counters(old, new):
            result = dict(ingress={}, egress={}, ghost={})
            for g, cnt in new.items():
                for k, v in cnt.items():
                    r_v = {}
                    old_v = old[g][k]
                    for vk, vv in v.items():
                        r_v[vk] = vv - old_v[vk]
                    result[g][k] = r_v
            return result

        def get_suppression_regs(self, suppression_regs, include_filters=False):

            def get_reg_vals(reg, b_idx, f_idx):
                reg.operations_execute(self.dev_tgt, 'Sync')
                r = reg.entry_get(self.dev_tgt, flags=dict(from_hw=False))
                reg_val = Bit(reg.info.size, value=0, fmt=Bit.Fmt.Hex)
                for d, k in r:
                    kd = k.to_dict()
                    dd = d.to_dict()
                    reg_val[kd['$REGISTER_INDEX']['value']] = \
                        dd['SwitchIngress.sfc_trigger.pause_filter_bank{}_filter{}.f1'.format(
                            b_idx, f_idx)][0]
                return reg_val

            suppression_regs['epoch_switch'].operations_execute(
                self.dev_tgt, 'Sync')
            resp = suppression_regs['epoch_switch'].entry_get(self.dev_tgt)

            result = {}
            for data, key in resp:
                values = data.to_dict()
                bank_change = (0b1 & (
                        values['SwitchIngress.sfc_epoch_init.reg_filter_epoch.bank_idx_changed'][0] >> 1))
                bank_idx = (0b1 & (
                    values['SwitchIngress.sfc_epoch_init.reg_filter_epoch.bank_idx_changed'][0]))
                result = dict(
                    bank_change=bank_change,
                    bank_idx=bank_idx,
                    current_epoch_start=values['SwitchIngress.sfc_epoch_init.reg_filter_epoch.current_epoch_start'][0]
                )

            if include_filters:
                time_start = time.time()
                result['banks'] = {0: {0: None, 1: None}, 1: {0: None, 1: None}}
                for bank_idx in result['banks'].keys():
                    for filter_idx in result['banks'][bank_idx].keys():
                        bank = suppression_regs['banks'][bank_idx][filter_idx]
                        result['banks'][bank_idx][filter_idx] = get_reg_vals(
                            bank, bank_idx, filter_idx)
                time_entry_get = time.time() - time_start
                print("  Done: {}s".format(time_entry_get))

            return result

        def set_suppression_regs(self, suppression_regs, reg_values):
            if 'bank_change' in reg_values.keys() or 'bank_idx' in reg_values.keys():
                assert 'bank_change' in reg_values.keys() and 'bank_idx' in reg_values.keys(
                ), "Both, if either bank_change or bank_idx are specified, both must be specified"

                assert reg_values['bank_idx'] in [0, 1]
                assert reg_values['bank_change'] in [0, 1]

            for bank_idx in reg_values['banks'].keys():
                for filter_idx in reg_values['banks'][bank_idx].keys():
                    keys = []
                    data = []
                    tbl = suppression_regs['banks'][bank_idx][filter_idx]
                    for reg_idx, reg_val in enumerate(reg_values['banks'][bank_idx][filter_idx]):
                        keys.append(tbl.make_key(
                            [
                                gc.KeyTuple('$REGISTER_INDEX', value=reg_idx),
                            ])
                        )
                        data.append(tbl.make_data(
                            [
                                gc.DataTuple(
                                    'SwitchIngress.sfc_trigger.pause_filter_bank{}_filter{}.f1'.format(bank_idx,
                                                                                                       filter_idx),
                                    int(reg_val))
                            ],
                            None
                        )
                        )
                    tbl.entry_mod(self.dev_tgt, keys, data)

            # reset Epoch and banck switch
            print("Set Epoch switch")
            new_eps_value = (reg_values['bank_change']
                             << 1) | reg_values['bank_idx']
            eps_tbl = suppression_regs['epoch_switch']
            keys = [eps_tbl.make_key([gc.KeyTuple('$REGISTER_INDEX', value=0)])]
            data = [
                eps_tbl.make_data([gc.DataTuple('SwitchIngress.sfc_epoch_init.reg_filter_epoch.bank_idx_changed',
                                                new_eps_value)], None)]
            eps_tbl.entry_mod(self.dev_tgt, keys, data)

        def reset_current_epoch_start(self, suppression_regs):
            print("Reset current_epoch_start ")
            suppression_regs['epoch_switch'].operations_execute(
                self.dev_tgt, 'Sync')
            resp = suppression_regs['epoch_switch'].entry_get(self.dev_tgt)

            old_eps_value = 0
            for data, key in resp:
                values = data.to_dict()
                old_eps_value = values['SwitchIngress.sfc_epoch_init.reg_filter_epoch.bank_idx_changed'][0]

            eps_tbl = suppression_regs['epoch_switch']
            keys = [eps_tbl.make_key([gc.KeyTuple('$REGISTER_INDEX', value=0)])]
            data = [
                eps_tbl.make_data([gc.DataTuple('SwitchIngress.sfc_epoch_init.reg_filter_epoch.current_epoch_start',
                                                0),
                                   gc.DataTuple('SwitchIngress.sfc_epoch_init.reg_filter_epoch.bank_idx_changed',
                                                old_eps_value)], None)]
            eps_tbl.entry_mod(self.dev_tgt, keys, data)

        def get_ghost_regs(self, ghost_regs):
            qd_size = self.ghost_regs['qdepth'].info.size

            ghost_regs['qdepth'].operations_execute(self.dev_tgt, 'Sync')
            resp_qdepth = ghost_regs['qdepth'].entry_get(
                self.dev_tgt, flags=dict(from_hw=False))

            result_qdepth = Bit(qd_size, value=0, fmt=Bit.Fmt.Hex)
            for (data, key) in resp_qdepth:
                keys = key.to_dict()
                values = data.to_dict()

                result_qdepth[keys['$REGISTER_INDEX']['value']
                ] = values['sfc_reg_qdepth.f1'][0]

            return result_qdepth

        def set_ghost_regs(self, ghost_regs, qd_values):
            assert isinstance(qd_values, Bit)
            qd_size = self.ghost_regs['qdepth'].info.size
            assert qd_values.size == qd_size

            qdepth = ghost_regs['qdepth']
            if qdepth not in self.registers:
                self.registers.append(qdepth)
            qdepth_keys = []
            qdepth_data = []
            for i, v in enumerate(qd_values):
                qdepth_keys.append(qdepth.make_key(
                    [gc.KeyTuple('$REGISTER_INDEX', i)]))
                qdepth_data.append(qdepth.make_data(
                    [gc.DataTuple('sfc_reg_qdepth.f1', v)]))

            qdepth.entry_mod(self.dev_tgt, qdepth_keys, qdepth_data)

        def get_egress_qdepth_regs(self, egress_regs):
            # Ensure a proper cleanup is run at the end of the test
            if egress_regs['qdepth'] not in self.registers:
                self.registers.append(egress_regs['qdepth'])

            egress_regs['qdepth'].operations_execute(self.dev_tgt, 'Sync')
            resp = egress_regs['qdepth'].entry_get(
                self.dev_tgt, flags=dict(from_hw=False))

            result = dict()
            for data, key in resp:
                keys = key.to_dict()
                values = data.to_dict()
                result[keys['$REGISTER_INDEX']['value']] = dict(
                    qdepth_drain_cells=values['SwitchEgress.sfc.reg_qdepth.qdepth_drain_cells'][0],
                    target_qdepth=values['SwitchEgress.sfc.reg_qdepth.target_qdepth'][0],
                )
            return result

        def base_l3_setup(self):
            try:
                # create a nexthop for myip packets
                self.nhop_glean = self.add_nexthop(self.device,
                                                   type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)

                # vrf id can get from self.vrf10 & 0xff, because the vrf id is 8 bits
                print("vrf= {}, rmac= {}".format(
                    hex(self.vrf10 & 0xff), self.rmac))
                print("vrf id = {}".format(self.attribute_get(
                    self.vrf10, SWITCH_VRF_ATTR_ID)))
                # create an RIF object on port0 and assign an IP address
                self.rif0 = self.add_rif(self.device,
                                         type=SWITCH_RIF_ATTR_TYPE_PORT,
                                         port_handle=self.port0,
                                         vrf_handle=self.vrf10,
                                         src_mac=self.rmac)
                self.rif0_myip_route = self.add_route(self.device,
                                                      ip_prefix='10.10.10.1',
                                                      vrf_handle=self.vrf10,
                                                      nexthop_handle=self.nhop_glean)

                # create an RIF object on port1 and assign an IP address
                self.rif1 = self.add_rif(self.device,
                                         type=SWITCH_RIF_ATTR_TYPE_PORT,
                                         port_handle=self.port1,
                                         vrf_handle=self.vrf10,
                                         src_mac=self.rmac)
                self.rif1_myip_route = self.add_route(self.device,
                                                      ip_prefix='11.11.11.1',
                                                      vrf_handle=self.vrf10,
                                                      nexthop_handle=self.nhop_glean)

                # simulate a nexthop on if0 with a host route
                self.nhop0 = self.add_nexthop(self.device,
                                              handle=self.rif0,
                                              dest_ip='10.10.10.2')
                self.neighbor0 = self.add_neighbor(self.device,
                                                   mac_address='00:10:22:33:44:55',
                                                   handle=self.rif0,
                                                   dest_ip='10.10.10.2')
                self.add_route(self.device,
                               ip_prefix='10.10.10.2',
                               vrf_handle=self.vrf10,
                               nexthop_handle=self.nhop0)

                # simulate a nexthop on if1 with a host route
                self.nhop1 = self.add_nexthop(self.device,
                                              handle=self.rif1,
                                              dest_ip='11.11.11.2')
                self.neighbor1 = self.add_neighbor(self.device,
                                                   mac_address='00:11:22:33:44:55',
                                                   handle=self.rif1,
                                                   dest_ip='11.11.11.2')
                rv = self.add_route(self.device,
                                    ip_prefix='11.11.11.2',
                                    vrf_handle=self.vrf10,
                                    nexthop_handle=self.nhop1)

            except Exception as e:
                self.fail("init failed: {}".format(e))

        # The input of port_num is "port_id" collum in the "bf_switch:0> show port all"
        def get_port_handle_from_port_num(self, port_num=266):
            # print("get_port_handle_from_port_num {}".format(port_num))
            attrs = list()
            value = switcht_value_t(
                type=switcht_value_type.OBJECT_ID, OBJECT_ID=self.device)
            attr = switcht_attribute_t(id=SWITCH_PORT_ATTR_DEVICE, value=value)
            attrs.append(attr)

            ret = self.client.object_get(SWITCH_OBJECT_TYPE_PORT, attrs)
            print(self.attribute_get(ret.object_id, SWITCH_PORT_ATTR_DEV_PORT))
            print("get_port_handle_from_port_num {}, {}".format(
                port_num, ret.object_id))
            return ret.object_id

except ImportError:
    print("bfruntime_client_base_tests not found, disabling bf_runtime tests")
    print("BFRUNTIME-GRPC DISABLED!")
    BFRUNTIME_ENABLE = False

    class SfcTestHelper(ApiHelper):
        pass

def requires_bfrtgrpc():
    print("Checking GRPC availability")
    return unittest.skipUnless(BFRUNTIME_ENABLE,
                               "This test requires bf_runtime over grcp support, which is not available")
