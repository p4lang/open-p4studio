# SPDX-FileCopyrightText: 2021 Intel Corporation
# Copyright (C) 2021 Intel Corporation.
#
# SPDX-License-Identifier: Apache-2.0
from __future__ import print_function
from traitlets.config import Config
from prompt_toolkit.input.defaults import create_input
from prompt_toolkit.output.defaults import create_output
from prompt_toolkit.application.current import get_app_session
from ctypes import *
import sys
import os
import builtins
import keyword
import IPython
import types
import tabulate
import pdb
import re
import atexit
import json
import operator
import copy
from tdiTable import TdiTable
from tdiTable import TdiTableError
from tdiInfo import TdiInfo
from tdiTableEntry import TableEntry
from tdiDefs import *

from ipaddress import ip_address as ip
from netaddr import EUI as mac
import logging

# below are loaded and unloaded in sys modules main everytime
# python cli is logged in and out
_tdi_context = {}
tdi = None

# below are loaded in sys modules main once at the
# first python cli login and maintained throughout
ipython_app = None
tdi_session_dict = {}

# regular global variables
sess_type = POINTER(c_uint)

class CIntfTdi:
    target_type_cls = TargetType

    def __init__(self, dev_id, table_cls, info_cls, driver_path):
        self._dev_id = dev_id
        self._driver = CDLL(driver_path)

        self.TdiTable = table_cls
        self.TdiInfo = info_cls
        sts = self._init_programs()
        if sts == -1:
            return sts
        self._init_wrapper()

    def _init_wrapper(self):
        #self.gflags = self._driver.tdi_generic_flag_support()
        def tdi_table_entry_get(tbl_hdl, session, dev_tgt, key, data, flag):
            return self._driver.tdi_table_entry_get(tbl_hdl, session, dev_tgt, flag, key, data)
        setattr(self, 'tdi_table_entry_get', tdi_table_entry_get)

        def tdi_table_entry_get_by_handle(tbl_hdl, session, dev_tgt, ent_hdl, key, data, flag):
            return self._driver.tdi_table_entry_get_by_handle(tbl_hdl, session, dev_tgt, flag, ent_hdl, key, data)
        setattr(self, 'tdi_table_entry_get_by_handle', tdi_table_entry_get_by_handle)

        def tdi_table_entry_handle_get(tbl_hdl, session, dev_tgt, key, handle):
            flags = self.get_flags()
            return self._driver.tdi_table_entry_handle_get(tbl_hdl, session, dev_tgt, flags, key, handle)
        setattr(self, 'tdi_table_entry_handle_get', tdi_table_entry_handle_get)

        def tdi_table_entry_key_get(tbl_hdl, session, dev_tgt, handle, e_tgt, key):
            flags = self.get_flags()
            return self._driver.tdi_table_entry_key_get(tbl_hdl, session, dev_tgt, flags, handle, e_tgt, key)
        setattr(self, 'tdi_table_entry_key_get', tdi_table_entry_key_get)

        def tdi_table_entry_get_first(tbl_hdl, session, dev_tgt, key, data, flag):
            return self._driver.tdi_table_entry_get_first(tbl_hdl, session, dev_tgt, flag, key, data)
        setattr(self, 'tdi_table_entry_get_first', tdi_table_entry_get_first)

        def tdi_table_entry_get_next_n(tbl_hdl, session, dev_tgt, key, keys, data, n, num_ret, flag):
            return self._driver.tdi_table_entry_get_next_n(tbl_hdl, session, dev_tgt, flag, key, keys, data, n, num_ret)
        setattr(self, 'tdi_table_entry_get_next_n', tdi_table_entry_get_next_n)

        def tdi_table_default_entry_get(tbl_hdl, session, dev_tgt, data, flag):
            return self._driver.tdi_table_default_entry_get(tbl_hdl, session, dev_tgt, flag, data)
        setattr(self, 'tdi_table_default_entry_get', tdi_table_default_entry_get)

        def tdi_table_default_entry_set(tbl_hdl, session, dev_tgt, data):
            flags = self.get_flags()
            return self._driver.tdi_table_default_entry_set(tbl_hdl, session, dev_tgt, flags, data)
        setattr(self, 'tdi_table_default_entry_set', tdi_table_default_entry_set)

        def tdi_table_default_entry_reset(tbl_hdl, session, dev_tgt):
            flags = self.get_flags()
            return self._driver.tdi_table_default_entry_reset(tbl_hdl, session, dev_tgt, flags)
        setattr(self, 'tdi_table_default_entry_reset', tdi_table_default_entry_reset)


        def tdi_table_usage_get(tbl_hdl, session, dev_tgt, count, flag):
            return self._driver.tdi_table_usage_get(tbl_hdl, session, dev_tgt, flag, count)
        setattr(self, 'tdi_table_usage_get', tdi_table_usage_get)

        def tdi_table_size_get(tbl_hdl, session, dev_tgt, count):
            flags = self.get_flags()
            return self._driver.tdi_table_size_get(tbl_hdl, session, dev_tgt, flags, count)
        setattr(self, 'tdi_table_size_get', tdi_table_size_get)

        def tdi_table_attributes_set(tbl_hdl, session, dev_tgt, attr):
            flags = self.get_flags()
            return self._driver.tdi_table_attributes_set(tbl_hdl, session, dev_tgt, flags, attr)
        setattr(self, 'tdi_table_attributes_set', tdi_table_attributes_set)

        def tdi_table_attributes_get(tbl_hdl, session, dev_tgt, attr):
            flags = self.get_flags()
            return self._driver.tdi_table_attributes_get(tbl_hdl, session, dev_tgt, flags, attr)
        setattr(self, 'tdi_table_attributes_get', tdi_table_attributes_get)

        def tdi_table_clear(tbl_hdl, session, dev_tgt):
            flags = self.get_flags()
            return self._driver.tdi_table_clear(tbl_hdl, session, dev_tgt, flags)
        setattr(self, 'tdi_table_clear', tdi_table_clear)

        def tdi_table_entry_del(tbl_hdl, session, dev_tgt, key):
            flags = self.get_flags()
            return self._driver.tdi_table_entry_del(tbl_hdl, session, dev_tgt, flags, key)
        setattr(self, 'tdi_table_entry_del', tdi_table_entry_del)

        def tdi_table_entry_add(tbl_hdl, session, dev_tgt, flags, key, data):
            return self._driver.tdi_table_entry_add(tbl_hdl, session, dev_tgt, flags, key, data)
        setattr(self, 'tdi_table_entry_add', tdi_table_entry_add)

        def tdi_table_entry_mod(tbl_hdl, session, dev_tgt, flags, key, data):
            return self._driver.tdi_table_entry_mod(tbl_hdl, session, dev_tgt, flags, key, data)
        setattr(self, 'tdi_table_entry_mod', tdi_table_entry_mod)

        def tdi_table_entry_mod_inc(tbl_hdl, session, dev_tgt, key, data, flag):
            # Translate flag to generic flags, MOD_INC_DEL is 2nd bit.
            flags = c_uint64(0)
            if flag == True:
                flags = c_uint64(1 << 1)
            return self._driver.tdi_table_entry_mod_inc(tbl_hdl, session, dev_tgt, flags, key, data)
        setattr(self, 'tdi_table_entry_mod_inc', tdi_table_entry_mod_inc)

    def _init_programs(self):
        # tdi session need _device hdl
        self.device_type = POINTER(c_uint)
        self._device = self.device_type()
        sts = self._driver.tdi_device_get(self._dev_id, byref(self._device))

        # testing for num devices
        num_devices = c_int(-1)
        sts = self._driver.tdi_num_device_id_list_get(byref(num_devices))
        array_type = c_uint32 * num_devices.value
        devices = array_type()
        sts = self._driver.tdi_device_id_list_get(byref(devices))
        for dev in devices:
          logging.debug("device_id_list="+str(dev))

        num_names = c_int(-1)
        self._driver.tdi_num_p4_names_get(self._dev_id, byref(num_names))
        print("We've found {} p4 programs for device {}:".format(num_names.value, self._dev_id))
        array_type = c_char_p * num_names.value
        p4_names = array_type()
        # why don't need to annotation for self._dev_id to c_int(self._dev_id)
        self._driver.tdi_p4_names_get(self._dev_id, p4_names)
        self.handle_type = POINTER(self.TdiHandle)
        self.handle_key_type = POINTER(self.TdiHandle)
        self.handle_target_type = POINTER(self.TdiHandle)
        self.annotation_type = self.TdiAnnotation

        self.flags_type = POINTER(c_uint)
        flag=0
        self._flags = self.flags_type()
        self._driver.tdi_flags_create(flag,  byref(self._flags))

        self.tdi_idle_timeout_cb_type = CFUNCTYPE(c_int, POINTER(self.TdiDevTgt), self.handle_type, c_void_p)

        self.tdi_tbl_operations_cb_type = CFUNCTYPE(None, POINTER(self.TdiDevTgt), c_void_p)

        self.learn_cb_type = CFUNCTYPE(c_int, POINTER(self.TdiDevTgt), sess_type, POINTER(self.handle_type), c_uint, self.handle_type, c_void_p)
        self.port_status_notif_cb_type = CFUNCTYPE(c_int, self.handle_target_type, self.handle_key_type, c_bool, c_void_p)
        self.selector_table_update_cb_type = CFUNCTYPE(None, sess_type, POINTER(self.TdiDevTgt), c_void_p, c_uint, c_uint, c_int, c_bool)
        self.infos = {}
        for name in p4_names:
            print(name.decode('ascii'))
            info = self.TdiInfo(self, name)
            if info == -1:
                return -1
            self.infos[name] = info
        self._flags = self.flags_type()
        self.target_type = POINTER(c_uint)
        self._target = self.target_type()
        sts = self._driver.tdi_target_create(self.get_device(), byref(self._target))

        if not sts == 0:
            print("Error, unable to create TDI Runtime session")
            return -1
    def _devcall(self):
        pdb.set_trace()

    def _complete_operations(self):
        sts = self._driver.tdi_session_complete_operations(tdi_session_dict[self._dev_id])
        if sts != 0:
            raise Exception("Error: complete_operations failed")

    def _begin_batch(self):
        sts = self._driver.tdi_begin_batch(tdi_session_dict[self._dev_id])
        if sts != 0:
            raise Exception("Error: begin_batch failed")

    def _end_batch(self):
        sts = self._driver.tdi_end_batch(tdi_session_dict[self._dev_id])
        if sts != 0:
            raise Exception("Error: end_batch failed")

    def _flush_batch(self, synchronous=True):
        sts = self._driver.tdi_flush_batch(tdi_session_dict[self._dev_id], c_bool(synchronous))
        if sts != 0:
            raise Exception("Error: flush_batch failed")

    def _begin_transaction(self, atomic=True):
        sts = self._driver.tdi_begin_transaction(tdi_session_dict[self._dev_id], c_bool(atomic))
        if sts != 0:
            raise Exception("Error: begin_transaction failed")

    def _verify_transaction(self):
        sts = self._driver.tdi_verify_transaction(tdi_session_dict[self._dev_id])
        if sts != 0:
            raise Exception("Error: verify_transaction failed")

    def _commit_transaction(self, synchronous=True):
        sts = self._driver.tdi_commit_transaction(tdi_session_dict[self._dev_id], c_bool(synchronous))
        if sts != 0:
            raise Exception("Error: commit_transaction failed")

    def _abort_transaction(self):
        sts = self._driver.tdi_abort_transaction(tdi_session_dict[self._dev_id])
        if sts != 0:
            raise Exception("Error: abort_transaction failed")

    def err_str(self, sts):
        estr = c_char_p()
        self._driver.bf_rt_err_str(c_int(sts), byref(estr))
        return estr.value.decode('ascii')
    class TdiHandle(Structure):
        _fields_ = [("unused", c_int)]

    class TdiDevTgt(Structure):
        _fields_ = [("dev_id", c_int)]
        def __str__(self):
            ret_val = ""
            for name, type_ in self._fields_:
                ret_val += name + ": " + str(getattr(self, name)) + "\n"
            return ret_val

    class TdiTargetHandle(Structure):
        _fields_ = [("unused", c_int)]

    class TdiFlagsHandle(Structure):
        _fields_ = [("unused", c_int)]

    class TdiAnnotation(Structure):
        _fields_ = [("name", c_char_p), ("value", c_char_p)]

    def get_driver(self):
        return self._driver

    def get_dev_id(self):
        return self._dev_id

    def get_session(self):
        return tdi_session_dict[self._dev_id]

    def get_device(self):
        return self._device

    def get_dev_tgt(self):
        return self._target

    def get_flags(self):
        return byref(self._flags)


class TableEntryDumper:
    def __init__(self, table, data_fields_imp_level_thresh=1):
        self.table = table
        self.entries = {}
        self.key_headers = []
        self.data_headers = {}
        self.data_fields_imp_level_thresh = data_fields_imp_level_thresh

    def prune_unwanted_data_fields(self, data_fields, data_content):
        data_fields_to_prune = []
        for name, info in data_fields.items():
            # By default, we assume the imp_level of the field to be 1
            imp_level = 1
            for item in info.annotations:
                if item[0] == "$bfrt_field_imp_level":
                    imp_level = int(item[1])
                    break
            if imp_level < self.data_fields_imp_level_thresh:
                data_fields_to_prune.append(name)
        for item in data_fields_to_prune:
            del data_fields[item]
            del data_content[item]

    def __call__(self, key_hdls, data_hdls, action_ids, print_zero=True):
        if not len(self.key_headers) == len(self.table.key_fields.keys()):
            for field in sorted(self.table.key_fields.values(), key=lambda x: x.id):
                self.key_headers.append(field.name.decode('ascii'))
        for i in range(0, len(key_hdls)):
            key = key_hdls[i]
            data = data_hdls[i]
            action = None

            if len(self.table.actions) > 0:
                action = self.table.action_id_name_map[action_ids[i]]

            key_content = self.table._get_key_fields(key)
            if not isinstance(key_content, dict):
                return
            data_content = self.table._get_data_fields(data, action)
            if not isinstance(data_content, dict):
                return

            data_fields = copy.copy(self.table.data_fields)
            act_str = ""
            if action is not None:
                data_fields = copy.copy(self.table.actions[action]["data_fields"])
                act_str = action.decode('ascii')
            # only display the fields whose importance level is above the set
            # threshold
            self.prune_unwanted_data_fields(data_fields, data_content)
            if act_str not in self.entries.keys():
                self.entries[act_str] = []
                data_headers = []
                for field in sorted(data_fields.values(), key=lambda x: x.id):
                    data_headers.append(field.name.decode('ascii'))
                self.data_headers[act_str] = data_headers

            ent = {'key_content' : key_content, 'data_content' : data_content, 'data_fields' : data_fields}

            self.entries[act_str].append(ent)

    def print_table(self, regex_strs=None, print_zero=True):
        for action, entries in self.entries.items():
            str_entries = []
            for e in entries:
                ent = []
                regex_accept = True
                for name in self.key_headers:
                    cname = name.encode('ascii')
                    str_rep = self.table.key_fields[cname].stringify_output(e['key_content'][cname])
                    if regex_strs is not None:
                        if not self._check_regex(str_rep, regex_strs[cname]):
                            regex_accept = False
                    ent.append(str_rep)
                if not regex_accept:
                    continue
                all_zero = True
                for name in self.data_headers[action]:
                    cname = name.encode('ascii')
                    if cname in e['data_content']:
                        if isinstance(e['data_content'][cname], list):
                            for elem in e['data_content'][cname]:
                                if elem != 0:
                                    all_zero = False
                                    break
                        elif e['data_content'][cname] != 0:
                            all_zero = False
                        #
                        ent.append(e['data_fields'][cname].stringify_output(e['data_content'][cname]))
                    else:
                        ent.append("N/A")

                if print_zero or not all_zero:
                    str_entries.append(ent)
            if not action == "":
                print("{} entries for action: {}".format(self.table.name, action))
            print(tabulate.tabulate(str_entries, headers=(self.key_headers + self.data_headers[action])))
            print('\n')

    def dump_entry_objs(self, regex_strs=None):
        objs = []
        for action, entries in self.entries.items():
            for e in entries:
                regex_accept = True
                for name in self.key_headers:
                    cname = name.encode('ascii')
                    str_rep = self.table.key_fields[cname].stringify_output(e['key_content'][cname])
                    if regex_strs is not None:
                        if not self._check_regex(str_rep, regex_strs[cname]):
                            regex_accept = False
                if not regex_accept:
                    continue
                if action == '':
                    action = None
                objs.append(TableEntry(self.table, e['key_content'], e['data_content'], action))
        return objs

    def dump_json(self):
        objs = self.dump_entry_objs()
        raws = [x.raw() for x in objs]
        return json.dumps(raws)

    def _check_regex(self, str_rep, pattern):
        if pattern is None:
            return True
        if isinstance(pattern, list):
            for idx, val in enumerate(pattern):
                if val is None:
                    continue
                try:
                    res = re.search(str(val), str_rep[idx])
                except Exception as e:
                    print("regex error: {}".format(str(e)))
                    continue
                if res is None:
                    return False
            return True
        pattern = str(pattern)
        try:
            res = re.search(pattern, str_rep)
        except Exception as e:
            print("Error: {}".format(str(e)))
            return True
        if res is not None:
            return True
        return False

class TDIContext:
    """
    This superclass provides node and leaf subclasses with callability.
    When called, the aforementioned objects' available commands are
    loaded into python's global namespace. When another object is
    called, the old commands are unloaded and replaced.
    """
    # TDIContext obj() will call obj.__call__()
    def __call__(self):
        global _tdi_context
        for name in _tdi_context['cur_context']:
            delattr(sys.modules['__main__'],name)

        _tdi_context['cur_context'] = []
        for name, child in self._get_children().items():
            _tdi_context['cur_context'].append(name)
            setattr(sys.modules['__main__'], name, child)

        print(self.__doc__)

        _tdi_context['parent'] = self._parent_node
        _tdi_context['cur_node'] = self

        tmp = self
        name = []
        while tmp != None:
            name.append(tmp._name)
            tmp = tmp._parent_node
        name.reverse()
        set_prompt(".".join(name))

    def _get_name(self):
        return self._name

    def _set_name(self, name):
        self._name = name.replace('$', '')

class TesterInt:
    def __init__(self, val, iterlen=8):
        self.intval = val
        self.iterlen = iterlen

    def __int__(self):
        return self.intval

    def __iter__(self):
        self.curiter = 0
        return self

    def __next__(self):
        if self.curiter == self.iterlen:
            raise StopIteration
        self.curiter += 1
        return self.intval

    def __getitem__(self, idx):
        if not isinstance(idx, int):
            raise TypeError
        if not idx < self.iterlen:
            raise IndexError
        return self.intval

    def __len__(self):
        return self.iterlen



class TDINode(TDIContext):
    """
    This class represents non-leaf nodes in the TDI Runtime CLI's command tree.
    Its sole purpose is to organize available commands.
    """
    def __init__(self, name, cintf, parent_node=None):
        self._set_name(name)
        self._cintf = cintf
        if parent_node is not None and isinstance(parent_node, TDINode):
            self._parent_node = parent_node
            self._parent_node._add_child(self)
        else:
            self._parent_node = None
        self._children = []
        self._commands = {}
        self._commands["dump"] = getattr(self, "dump")
        self._commands["info"] = getattr(self, "info")

    def dump(self, table=False, from_hw=False, return_ents=False, print_zero=True):
        for child in self._children:
            if isinstance(child, TDINode):
                child.dump(table=table, from_hw=from_hw, return_ents=return_ents, print_zero=print_zero)
            elif (isinstance(child, TDILeaf) and "dump" in child._c_tbl.supported_commands):
                child.dump(table=table, json=False, from_hw=from_hw, return_ents=return_ents, print_zero=print_zero)

    def clear(self, batch=True):
        for child in self._children:
            if ((isinstance(child, TDILeaf) and "clear" in child._c_tbl.supported_commands) or (isinstance(child, TDINode))):
                child.clear(batch)

    def tdi_info(self, return_info=True, print_info=True):
        print("This a test tdi")
        info = self._get_full_leaf_info()
        table_rows = []
        res = []
        for i in info:
            table_rows.append(i[:-1])
            if return_info:
                res.append({"full_name": i[0],
                            "type": i[1],
                            "usage": i[2],
                            "capacity": i[3],
                            "node": i[4]})
        if print_info:
            print("Tables under this node:")
            print(tabulate.tabulate(table_rows, headers=["Full Table Name", "Type", "Usage", "Capacity"]))
        if return_info:
            return res
    def info(self, return_info=False, print_info=True):
        info = self._get_full_leaf_info()
        table_rows = []
        res = []
        for i in info:
            table_rows.append(i[:-1])
            if return_info:
                res.append({"full_name": i[0],
                            "type": i[1],
                            "usage": i[2],
                            "capacity": i[3],
                            "node": i[4]})
        if print_info:
            print("Tables under this node:")
            print(tabulate.tabulate(table_rows, headers=["Full Table Name", "Type", "Usage", "Capacity"]))
        if return_info:
            return res

    def _get_full_leaf_info(self):
        info = []
        for k, v in self._get_children().items():
            if isinstance(v, TDILeaf) or isinstance(v, TDINode):
                info += v._get_full_leaf_info()
        return info

    def _add_child(self, child):
        self._children.append(child)
        exec("self.{} = child".format(child._get_name()))

    def _get_children(self):
        children = {}
        for child in self._children:
            children[child._get_name()] = child
        for name, command in self._commands.items():
            children[name] = command
        return children

    def _set_doc(self):
        docstr = "Available symbols:\n"
        child_names = []
        for child in self._children:
            type_ = "Node"
            if isinstance(child, TDILeaf):
                type_ = "Table"
            if isinstance(child, TDILrnLeaf):
                type_ = "Learn-digest"
            child_names.append((child._get_name(), type_))
        for name, command in self._commands.items():
            child_names.append((name, "Command"))
        for name, description in sorted(child_names):
            docstr += "{:<20} - {}\n".format(name, description)
        self.__doc__ = docstr

    def _simple_tester(self):
        for child in self._children:
            child._simple_tester()

    def _make_dep_tester_funcs(self):
        tested = []
        def test_table(leaf):
            sts = 0
            if leaf in tested:
                return sts
            if not leaf.has_keys:
                return sts
            if leaf._c_tbl.table_type_cls.table_type_str(leaf._c_tbl.get_type()) in leaf._c_tbl.no_usage_tables():
                return sts
            if leaf._c_tbl.get_id() in leaf._c_tbl._tdi_info.tbl_dep_map:
                for dep_id in leaf._c_tbl._tdi_info.tbl_dep_map[leaf._c_tbl.get_id()]:
                    dep_leaf = leaf._c_tbl._tdi_info.tbl_id_map[dep_id].frontend
                    if dep_leaf is None:
                        print("Error: dep_test initialization did not finish!")
                        return -1
                    sts = test_table(dep_leaf)
                    if not sts == 0:
                        return sts
            print("Testing table: {}".format(leaf._name))
            sts = leaf._test_all_adds()
            tested.append(leaf)
            return sts
        def cleanup():
            tested.reverse()
            for leaf in tested:
                for ent in leaf.get(regex=True, print_ents=False):
                    ent.remove()

        return test_table, cleanup

    def _dep_tester(self, test_func=None):
        sts = 0
        root = False
        cleanup = None
        if test_func is None:
            test_func, cleanup = self._make_dep_tester_funcs()
            root = True
        for child in self._children:
            sts |= child._dep_tester(test_func)
        if cleanup is not None:
            cleanup()
        if root:
            if sts == 0:
                print("dep_test_success")
            else:
                print("dep_test_failed")
        return sts


# default callbacks
def _default_operation_callback(dev_id, pipe_id, direction, parser_id):
    print("operation complete - dev_id: {}, pipe_id: {}, direction: {}, parser_id: {}".format(dev_id, pipe_id, direction, parser_id))

def _learn_fields_print(dev_id, pipe_id, direction, parser_id, session, data):
    print("Device id: {}\n"
          "Pipe id: {}\n"
          "Direction: {}\n"
          "Parser id: {}\n".format(dev_id, pipe_id, direction, parser_id))
    print("Learn data:\n{}".format(data))
    return 0

class TDILeaf(TDIContext):
    """
    This class creates easy to type, autocompleted python entrypoints to the
    TDI Runtime API. Each instance of the class represents one table. It exposes
    Add, modify commands for each action type (or one of each for the table if
    said table has no actions). It also exposes get and dump commands for
    retrieving information from TDI Runtime.
    """
    def __init__(self, name, c_tbl, cintf, parent_node=None, children=None):
        self._set_name(name)
        self._c_tbl = c_tbl
        self._cintf = cintf
        c_tbl.set_frontend(self)
        self._children = {}
        #
        # Link the TDI Command Object Tree
        #
        if parent_node is not None and isinstance(parent_node, TDINode):
            self._parent_node = parent_node
            self._parent_node._add_child(self)
        else:
            self._parent_node = None
        #
        # If some children were provided add them
        #
        if children is not None:
            for c in children:
                self._add_child(c)
        #
        # Setup Methods and Attributes for Leaf Command Node
        #
        self._adds = {}
        self._mods = {}
        self._mods_inc = {}
        self._dels = []
        self._gets = []
        self._entries = {}
        self._init_table_methods()
        self._init_string_choices()
        for cmd in self._c_tbl.supported_commands:
            try:
                self._children[cmd] = getattr(self,cmd)
            except:
                logging.debug("CLI log: Command {} is not ready or available".format(cmd))
                pass
        self._set_docstring()
        # if self._c_tbl.table_type_cls.table_type_str(self._c_tbl.get_type()) in self._c_tbl.unimplemented_tables:
        #     print("CLI Err: Unimplemented Command Object {}".format(self._name))
        #     return
        # else:
        #     print("CLI Log: Implemented Command Object {}".format(self._name))
        #     for cmd in self._children:
        #         print("CLI log: Command {}".format(cmd))

    def _add_child(self, child):
        self._children[child._name] = child
        child._parent_node = self
        exec("self.{} = child".format(child._get_name()))

    def info(self, return_info=False, print_info=True):
        res = {
            "table_name": self._name,
            "full_name": self._c_tbl.name,
            "type": self._c_tbl.table_type_cls.table_type_str(self._c_tbl.get_type()),
            "usage": self._c_tbl.get_usage(),
            "capacity": self._c_tbl.get_capacity(),
            "key_fields": [],
            "data_fields": [],
        }
        if print_info:
            print("Table Name: {}".format(res["table_name"]))
            print("Full Name: {}".format(res["full_name"]))
            print("Type: {}".format(res["type"]))
            print("Usage: {}".format(res["usage"]))
            print("Capacity: {}\n".format(res["capacity"]))
            print("Key Fields:")
        key_rows = []
        headers = ["Name", "Type", "Size", "Required", "Read Only"]
        for info in sorted(self._c_tbl.key_fields.values(), key=lambda x: x.id):
            field = [info.name.decode('ascii'), self._c_tbl.key_match_type_cls.key_match_type_str(info.type), info.size, info.required, info.read_only]
            key_rows += [field]
            res["key_fields"] += [{"name": field[0],
                                   "type": field[1],
                                   "size": field[2],
                                   "required": field[3],
                                   "read_only": field[4]}]
        if print_info:
            print(tabulate.tabulate(key_rows, headers=headers))

        if len(self._c_tbl.actions) == 0:
            data_rows = []
            for info in sorted(self._c_tbl.data_fields.values(), key=lambda x: x.id):
                field = [info.name.decode('ascii'), self._c_tbl.data_type_cls.data_type_str(info.data_type), info.size, info.required, info.read_only]
                data_rows += [field]
                res["data_fields"] += [{"name": field[0],
                                        "type": field[1],
                                        "size": field[2],
                                        "required": field[3],
                                        "read_only": field[4]}]
            if print_info:
                print("\nData Fields:\n{}\n".format(tabulate.tabulate(data_rows, headers=headers)))
        else:
            res["data_fields"] = {}
        print()

        for name, act_info in self._c_tbl.actions.items():
            data_rows = []
            res["data_fields"][name.decode('ascii')] = []
            for info in sorted(act_info['data_fields'].values(), key=lambda x: x.id):
                field = [info.name.decode('ascii'), self._c_tbl.data_type_cls.data_type_str(info.data_type), info.size, info.required, info.read_only]
                data_rows += [field]
                res["data_fields"][name.decode('ascii')] += [{"name": field[0],
                                                              "type": field[1],
                                                              "size": field[2],
                                                              "required": field[3],
                                                              "read_only": field[4]}]
            if print_info:
                print("Data Fields for action {}:\n{}\n".format(name.decode('ascii'), tabulate.tabulate(data_rows, headers=headers)))

        if return_info:
            return res

    def clear(self, batch=True):
        self._c_tbl.clear(batch)

    def dump(self, table=False, json=False, from_hw=False, return_ents=False, print_zero=True):
        """Dump all entries of table including default entry if applicable
        """
        table_type = self._c_tbl.table_type_cls.table_type_str(self._c_tbl.get_type())

        print("----- {} Dump Start -----".format(self._name))
        if "get_default" in self._c_tbl.supported_commands and (not (return_ents or json)):
            print("Default Entry:")
            try:
                d_ent = self.get_default(print_ent=False)
                pstr, _ = self._c_tbl.print_entry(d_ent.key, d_ent.data, d_ent.action.encode('ascii') if d_ent.action else d_ent.action)
                print(pstr[11:])
            except TdiTableError as e:
                print(e)
        if not table and not json and not return_ents:
            stream_printer = self._c_tbl.make_entry_stream_printer()
            try:
                self._c_tbl.dump(stream_printer, from_hw, print_zero=print_zero)
            except TdiTableError as e:
                print(e)
            print("----- {} Dump End -----\n".format(self._name))
            if json:
                print("Table must be true to produce json output")
                return None
        else:
            field_display_default_threshold = 1
            # For the time being, we only support printing a subset of the data
            # fields when the table is either "PORT_CFG" or "PORT_STAT"
            # Thus if the table is neither of those, then always print all the
            # fields. Thus we set the level_thresh to 1
            if table_type in ["PORT_CFG", "PORT_STAT"] and table is True:
                field_display_default_threshold = 2
            printer = TableEntryDumper(self._c_tbl, field_display_default_threshold)
            ret = self._c_tbl.dump(printer, from_hw)
            if ret == 0:
                if json:
                    return printer.dump_json()
                elif return_ents:
                    return printer.dump_entry_objs()
                else:
                    printer.print_table(print_zero=print_zero)
                print("----- {} Dump End -----".format(self._name))
            else:
                return None

    def add_from_json(self, entry_blob):
        if isinstance(entry_blob, str):
            jents = json.loads(entry_blob)
            try:
                for ent in jents:
                    if not ent['table_name'] == self._c_tbl.name:
                        print("Error: table mismatch for entry {}.".format(ent))
                    skey = ent['key']
                    sdata = ent['data']
                    saction = ent['action']
                    key = {}
                    data = {}
                    action = None
                    if saction is not None:
                        action = saction.encode('ascii')
                    for k, v in skey.items():
                        key[k.encode('ascii')] = v
                    for k, v in sdata.items():
                        data[k.encode('ascii')] = v
                        # In dump output, the Register datafield is in the form of list of 4 elements, one per pipe.
                        # But the Register datafield is BYTE_STREAM
                        # so picking the first element of the list as input for Register datafield.
                        if type(v) is list:
                            try:
                                if action is None:
                                    data_field = self._c_tbl.data_fields[k.encode('ascii')]
                                else:
                                    data_field = self._c_tbl.actions[action]["data_fields"][k.encode('ascii')]
                                data_type = self._c_tbl.data_type_cls.data_type_str(data_field.data_type)
                                if data_type in ["UINT64", "BYTE_STREAM", "BOOL"] and ('$bfrt_field_class', 'register_data') in data_field.annotations:
                                    data[k.encode('ascii')] = v[0]
                            except:
                                pass
                    parsed_keys, parsed_data = self._c_tbl.parse_str_input("add_from_json", key, data, action)
                    if parsed_keys == -1:
                        return
                    if parsed_data == -1:
                        return
                    self._c_tbl.add_entry(parsed_keys, parsed_data, action)
            except Exception as e:
                print("Error: {}".format(str(e)))
        else:
            print("Input must be string produced by dump command for this table.")


    def _create_attributes(self, key_fields={}, data_fields={}):
        return

    def _create_operations(self):
        method_name = "operation_register_sync"
        if method_name in self._c_tbl.supported_commands:
            def operation_register_sync(self, callback=None):
                if callback is None:
                    callback = _default_operation_callback
                self._c_tbl.operation_register_sync(callback)
            bound_method = operation_register_sync.__get__(self, self.__class__)
            self.operation_register_sync = bound_method
            self._children[method_name] = getattr(self, method_name)

        method_name = "operation_counter_sync"
        if method_name in self._c_tbl.supported_commands:
            def operation_counter_sync(self, callback=None):
                if callback is None:
                    callback = _default_operation_callback
                self._c_tbl.operation_counter_sync(callback)
            bound_method = operation_counter_sync.__get__(self, self.__class__)
            self.operation_counter_sync = bound_method
            self._children[method_name] = getattr(self, method_name)

        method_name = "operation_hit_state_update"
        if method_name in self._c_tbl.supported_commands:
            def operation_hit_state_update(self, callback=None):
                if callback is None:
                    callback = _default_operation_callback
                self._c_tbl.operation_hit_state_update(callback)
            bound_method = operation_hit_state_update.__get__(self, self.__class__)
            self.operation_hit_state_update = bound_method
            self._children[method_name] = getattr(self, method_name)

    def _get_children(self):
        return self._children

    def _get_full_leaf_info(self):
        return [[self._c_tbl.name,
                 self._c_tbl.table_type_cls.table_type_str(self._c_tbl.get_type()),
                 self._c_tbl.get_usage(),
                 self._c_tbl.get_capacity(),
                 self]]

    def _test_all_adds(self):
        for i, (ent_gen, params) in enumerate(self._entries.values()):
            if i > 1:
                return 0
            kwargs = {}
            for p in params:
                kwargs[p] = TesterInt(i)
            ent = ent_gen(**kwargs)
            if ent is None:
                return -1
            ent.push()
        return 0

    def _simple_tester(self):
        print("Testing adds for table {}".format(self._name))
        for add, params in self._adds.values():
            for i in range(1, 9):
                kwargs = {}
                for p in params:
                    kwargs[p] = TesterInt(i)
                add(**kwargs)
            for i in range(1, 9):
                for d_func, d_params in self._dels:
                    kwargs = {}
                    for p in d_params:
                        kwargs[p] = TesterInt(i)
                    d_func(**kwargs)
        print("Testing mods for table {}".format(self._name))
        add, a_params = list(self._adds.values())[0]
        kwargs = {}
        for p in a_params:
            kwargs[p] = TesterInt(1)
        add(**kwargs)
        for mod, params in self._mods.values():
            kwargs = {}
            for p in params:
                kwargs[p] = TesterInt(1)
            mod(**kwargs)
        for d_func, d_params in self._dels:
            kwargs = {}
            for p in d_params:
                kwargs[p] = TesterInt(1)
            d_func(**kwargs)
        print("Test for table {} completed\n".format(self._name))

    def _dep_tester(self, test_func):
        sts = test_func(self)
        if not sts == 0:
            print("Error: dep test for table {} failed!".format(self._name))
        return sts

    """
    This function dynamically creates a docstring for our leaf exposing
    user-relevant information about key fields, actions, data fields for the
    corresponding table.
    """
    def _set_docstring(self):
        full_table_name = self._c_tbl.name
        key_docstring = ""
        actdata_docstring = ""

        for readable in self._c_tbl.key_field_readables:
            key_docstring += "    {}\n".format(readable)

        if len(self._c_tbl.data_field_readables) > 0:
            actdata_docstring += "Data fields:\n"
            for readable in self._c_tbl.data_field_readables:
                actdata_docstring += "    {}\n".format(readable)
        else:
            actdata_docstring += "Actions, Data fields:\n"
            for action_name, readable in self._c_tbl.action_readables.items():
                actdata_docstring += "    {}\n".format(readable)
                num_data_fields = len(self._c_tbl.action_data_readables[action_name])
                actdata_docstring += "        {} data fields:\n".format(num_data_fields)
                for d_readable in self._c_tbl.action_data_readables[action_name]:
                    actdata_docstring += "            {}\n".format(d_readable)

        child_names = sorted(list(self._get_children().keys()))
        commands = ""
        for name in child_names:
            commands += "{}\n".format(name)

        extra = ""
        if self._c_tbl.compress_input:
            extra += "Because of large number of key and data fields,\n"
            extra += "this table uses dictionaries to pass them as arguments.\n"
            extra += "Format example:\n"
            extra += "add(key_dict={'hdr.ethernet.dst_addr':0x001122334455}, data_dict={'port':5})\n"

        docstr = """
TDI Runtime CLI Object for {} table

Key fields:
{}

{}
{}
Available Commands:
{}
        """.format(full_table_name, key_docstring, actdata_docstring, extra, commands).strip()
        self.__doc__ = docstr

    def _init_string_choices(self):
        if len(self._c_tbl.string_choices) > 0:
            self.string_choices = {}
            for name, choices in self._c_tbl.string_choices.items():
                self.string_choices[name] = []
                self.string_choices[name].extend(choices)
            self._children["string_choices"] = self.string_choices

    """
    The following functions create appropriate add, modify, delete, get, dump
    commands for our leaf. These commands are generated based on metadata
    pulled from the TdiInfo objects exposed by TDI Runtime's APIs.
    """
    def _init_table_methods(self):
        key_fields = self._c_tbl.key_fields
        self.has_keys = len(key_fields) > 0

        self._create_get(key_fields)
        self._create_get_handle(key_fields)
        self._create_del(key_fields)

        for action_name, info in self._c_tbl.actions.items():
            data_fields = info["data_fields"]
            annotations = info["annotations"]
            # print("annotaions {}".format(str(annotations)))
            if not self._c_tbl.has_const_default_action:
                self._create_set_default_with_action(data_fields, action_name)
            if ("@defaultonly","") not in annotations:
                self._create_mod_with_action(key_fields, data_fields, action_name)
                self._create_mod_inc_with_action(key_fields, data_fields, action_name)
                self._create_entry_with_action(key_fields, data_fields, action_name)
                self._create_add_with_action(key_fields, data_fields, action_name)
        if len(self._c_tbl.actions) == 0:
            data_fields = self._c_tbl.data_fields
            self._create_set_default(data_fields)
            self._create_mod(key_fields, data_fields)
            self._create_mod_inc(key_fields, data_fields)
            self._create_entry(key_fields, data_fields)
            self._create_add(key_fields, data_fields)

        if not self._c_tbl.has_const_default_action:
            self._create_reset_default()
        self._create_get_default()
        self._create_attributes(key_fields)
        self._create_operations()
        self._create_get_key()

    def _set_dynamic_method(self, method_def, method_name):
        d = {}
        try:
            exec(method_def.strip(), globals(), d)
        except Exception as e:
            print(e)
            raise e
        setattr(self, method_name, types.MethodType(d[method_name], self))
        self._children[method_name] = getattr(self, method_name)
        return getattr(self, method_name)

    def _build_names(self, names, p_names, cur_collide, ridx):
        collision = 0
        for name in names:
            p_name = name.decode('ascii').replace('$', '').replace('-', '_').replace(':', '_').replace('[', '_').replace(']', '_')
            cidx = len(p_name)
            for i in range(ridx):
                cidx = p_name.rfind('.', 0, cidx)
                if cidx == -1:
                    break
            p_name = p_name[cidx + 1:].replace('.', '_')
            p_names[name] = p_name
            if p_name in cur_collide:
                if cidx == -1:
                    collision = -2
                if collision == 0:
                    collision = -1
            cur_collide[p_name] = 0
        return collision

    def _make_param_names(self, names, collide={}, ridx=1):
        # ridx is the dot value we are at while shortening the name
        # For names ["a.b.c.d", "b.c.d", "e.b.c.d"], we are looking at
        # feasibilty of using just "d" when ridx==1. When collisions
        # happen, we increase ridx to next value.
        p_names = {}
        cur_collide = {}
        sts = -1
        max_depth_allowed = 20
        while sts < 0:
            cur_collide = copy.deepcopy(collide)
            p_names = {}
            sts = self._build_names(names, p_names, cur_collide, ridx)
            if sts < 0:
                ridx += 1
                if ridx > max_depth_allowed and sts == -2:
                    # sts -2 can mean 2 cases.
                    # Case 1: one of the names is a subset of another
                    # cidx would hit -1. Like for "b.c.d" we will hit
                    # -1. In this case, we want to continue
                    # Case 2: 2 names are exactly equal. This is actually
                    # an error condition. Let us not get into an infinte
                    # loop over an error condition. Let it error out
                    # elsewhere
                    break
        return p_names, cur_collide, ridx

    def _make_core_method_strs(self, method_name, key_fields={}, data_fields={}):
        key_idx = []
        key_params = []
        key_types = []
        key_sizes = []
        key_pnames, collide, k_ridx = self._make_param_names(key_fields.keys())

        data_idx = []
        data_params = []
        data_types = []
        data_sizes = []
        data_pnames, _, d_ridx = self._make_param_names(data_fields.keys(), collide, k_ridx)

        """Python3.4 supports up to 256 arguments per function.
        Since it is possible to go over that amount, by creating table
        with a lot of keys/data, those arguments must be compressed.
        In such case use dictionary for key and dictionary for data.
        Check is done on 245 to support around 10 other arguments (pipe,
        gress_dir etc.).
        """
        compress_input = self._c_tbl.compress_input
        tries = 0
        while k_ridx != d_ridx:
            key_pnames, collide, k_ridx = self._make_param_names(key_fields.keys(), ridx=d_ridx)
            data_pnames, _, d_ridx = self._make_param_names(data_fields.keys(), collide, k_ridx)
            tries += 1
            if tries >= 50:
                for name in key_pnames.keys():
                    key_pnames[name] = key_pnames[name] + "_key"
                for name in data_pnames.keys():
                    data_pnames[name] = data_pnames[name] + "_data"
                break

        for name, info in sorted(key_fields.items(), key=lambda x: x[1].id):
            key_idx += [name.decode('ascii')]
            key_types += [info.table.key_match_type_cls.key_match_type_str(info.type)]
            key_sizes += [info.size]
            p_name = key_pnames[name]
            if info.table.key_match_type_cls.key_match_type_str(info.type) == "TERNARY":
                key_params += [[p_name, p_name + "_mask"]]
            elif info.table.key_match_type_cls.key_match_type_str(info.type) == "RANGE":
                key_params += [[p_name + "_start", p_name + "_end"]]
            elif info.table.key_match_type_cls.key_match_type_str(info.type) == "LPM":
                key_params += [[p_name, p_name + "_p_length"]]
            elif info.table.key_match_type_cls.key_match_type_str(info.type) == "OPTIONAL":
                key_params += [[p_name, p_name + "_is_valid"]]
            else:
                key_params += [p_name]

        for name, info in sorted(data_fields.items(), key=lambda x: x[1].id):
            data_idx += [name.decode('ascii')]
            p_name = data_pnames[name]
            data_params += [p_name]
            data_types += [info.table.data_type_cls.data_type_str(info.data_type)]
            data_sizes += [info.size]

        param_str = ""
        param_list = []
        if compress_input:
            param_str = "key_dict=None, data_dict=None, "
        else:
            for p in key_params:
                if isinstance(p, list):
                    for sub_p in p:
                        param_str += "{}=None, ".format(sub_p)
                        param_list.append(sub_p)
                else:
                    param_str += "{}=None, ".format(p)
                    param_list.append(p)
            for p in data_params:
                param_str += "{}=None, ".format(p)
                param_list.append(p)

        param_docstring = ""
        for idx, p in enumerate(key_params):
            param_docstring += "{!s:30} type={!s:10} size={:^2d} default=0\n    ".format(p, key_types[idx], key_sizes[idx])
        for idx, p in enumerate(data_params):
            param_docstring += "{!s:30} type={!s:10} size={:^2d} default=0\n    ".format(p, data_types[idx], data_sizes[idx])
        if compress_input:
            param_docstring += "\n    "
            param_docstring += "This method uses dictionaries to pass key and data fields as arguments.\n    "
            param_docstring += "Format example:\n    "
            param_docstring += "{}(key_dict={{'hdr.ethernet.dst_addr':0x001122334455}}, data_dict={{'port':5}})'\n    ".format(method_name)

        parse_key_call = "{{{}}}"
        parse_key_elems = ""
        for idx, p in enumerate(key_params):
            p_value = p
            if isinstance(p, list):
                p_value = "[{}, {}]".format(p[0], p[1])
            parse_key_elems += "b'{}' : {}, ".format(key_idx[idx], p_value)
        if compress_input:
            parse_key_call = "key_dict"
        else:
            parse_key_call = parse_key_call.format(parse_key_elems)

        parse_data_call = "{{{}}}"
        parse_data_elems = ""
        for idx, p in enumerate(data_params):
            parse_data_elems += "b'{}' : {}, ".format(data_idx[idx], p)
        if compress_input:
            parse_data_call = "data_dict"
        else:
            parse_data_call = parse_data_call.format(parse_data_elems)

        return param_str, param_docstring, parse_key_call, parse_data_call, param_list

    # Generating add_with_<action> APIs dynamically
    def _create_add_with_action(self, key_fields, data_fields, action_name, code_str=None):
        if "add" not in self._c_tbl.supported_commands:
            return

        full_strname = action_name.decode('ascii')
        strname = full_strname[full_strname.rfind('.') + 1:].replace("$","")
        method_name = "add_with_{}".format(strname)

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields, data_fields)

        if code_str == None:
            code_str = '''
def {}(self, {}):
    """Add entry to {} table with action: {}

    Parameters:
    {}
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}",{}, {}, b'{}')
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return

    self._c_tbl.add_entry(parsed_keys, parsed_data, b'{}')
        '''

        code = code_str.format(method_name, param_str, self._name, full_strname, param_docstring, method_name, parse_key_call, parse_data_call, full_strname, full_strname)
        add_method = self._set_dynamic_method(code, method_name)
        self._adds[full_strname] = (add_method, param_list)

    # Generating add APIs dynamically
    def _create_add(self, key_fields, data_fields, code_str=None):
        method_name = "add"
        if method_name not in self._c_tbl.supported_commands:
            return

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields, data_fields)

        if code_str == None:
            code_str = '''
def {}(self, {}):
    """Add entry to {} table.

    Parameters:
    {}
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {})
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return
    self._c_tbl.add_entry(parsed_keys, parsed_data)
        '''

        code = code_str.format(method_name, param_str, self._name, param_docstring, method_name, parse_key_call, parse_data_call)
        add_method = self._set_dynamic_method(code, method_name)
        self._adds["addwithnoaction"] = (add_method, param_list)

    # Generating set_default_with_<action> APIs dynamically
    def _create_set_default_with_action(self, data_fields, action_name, code_str=None):
        if "set_default" not in self._c_tbl.supported_commands:
            return

        full_strname = action_name.decode('ascii')
        strname = full_strname[full_strname.rfind('.') + 1:].replace("$","")
        method_name = "set_default_with_{}".format(strname)
        if (strname == "NoAction"):
            data_fields = {}

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, data_fields=data_fields)

        if code_str == None:
            code_str = '''
def {}(self, {}):
    """Set default action for {} table with action: {}

    Parameters:
    {}
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {}, b'{}')
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return
    self._c_tbl.set_default_entry(parsed_data, b'{}')
        '''
        code = code_str.format(method_name, param_str, self._name, full_strname, param_docstring, method_name, parse_key_call, parse_data_call, full_strname, full_strname)
        add_method = self._set_dynamic_method(code, method_name)

    # Generating set_default API dynamically
    def _create_set_default(self, data_fields, code_str=None):
        method_name = "set_default"
        if method_name not in self._c_tbl.supported_commands or self._c_tbl.has_const_default_action:
            return

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, data_fields=data_fields)

        if code_str == None:
            code_str = '''
def {}(self, {}):
    """Set default action for {} table.

    Parameters:
    {}
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {})
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return
    self._c_tbl.set_default_entry(parsed_data)
        '''
        code = code_str.format(method_name, param_str, self._name, param_docstring, method_name, parse_key_call, parse_data_call)
        self._set_dynamic_method(code, method_name)

    # Generating reset_default API dynamically
    def _create_reset_default(self, code_str=None):
        method_name = "reset_default"
        if method_name not in self._c_tbl.supported_commands:
            return

        if code_str == None:
            code_str = '''
def {}(self):
    """Set default action for {} table.
    """
    self._c_tbl.reset_default_entry()
        '''
        code = code_str.format(method_name, self._name)
        self._set_dynamic_method(code, method_name)

    # Generating mod_with_<action> APIs dynamically
    def _create_mod_with_action(self, key_fields, data_fields, action_name, code_str=None):
        if "mod" not in self._c_tbl.supported_commands:
            return

        full_strname = action_name.decode('ascii')
        strname = full_strname[full_strname.rfind('.') + 1:].replace("$","")
        method_name = "mod_with_{}".format(strname)

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields, data_fields)

        if code_str == None:
            code_str = '''
def {}(self, {} ttl_reset=True):
    """Modify entry in {} table using action: {}

    Parameters:
    {}
    ttl_reset: default=True, set to False in order to not start aging entry from the beggining.
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {}, b'{}')
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return

    self._c_tbl.mod_entry(parsed_keys, parsed_data, b'{}', ttl_reset=ttl_reset)
        '''
        code= code_str.format(method_name, param_str, self._name, full_strname, param_docstring, method_name, parse_key_call, parse_data_call, full_strname, full_strname)
        mod_method = self._set_dynamic_method(code, method_name)
        self._mods[full_strname] = (mod_method, param_list)

    # Generating mod_inc_with_<action> APIs dynamically
    def _create_mod_inc_with_action(self, key_fields, data_fields, action_name, code_str=None):
        if "mod_inc" not in self._c_tbl.supported_commands:
            return

        full_strname = action_name.decode('ascii')
        strname = full_strname[full_strname.rfind('.') + 1:].replace("$","")
        method_name = "mod_inc_with_{}".format(strname)

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields, data_fields)

        if code_str is None:
            code_str = '''
def {}(self, {} mod_flag=0):
    """ Incremental Add/Delete items in the fields that are array for the matched entry in {} table.

    Parameters:
    {}
    mod_flag: 0/1 value for Add/Delete Item in the field that is array type.
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {}, b'{}')
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return

    self._c_tbl.mod_inc_entry(parsed_keys, parsed_data, mod_flag, b'{}')
        '''
        code = code_str.format(method_name, param_str, self._name, full_strname, param_docstring, method_name, parse_key_call, parse_data_call, full_strname, full_strname)
        mod_method = self._set_dynamic_method(code, method_name)
        self._mods_inc[full_strname] = (mod_method, param_list)

    # Generating mod API dynamically
    def _create_mod(self, key_fields, data_fields, code_str=None):
        method_name = "mod"
        if method_name not in self._c_tbl.supported_commands:
            return

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields, data_fields)

        if code_str == None:
            code_str = '''
def {}(self, {} ttl_reset=True):
    """Modify any field in the entry at once in {} table.

    Parameters:
    {}
    ttl_reset: default=True, set to False in order to not start aging entry from the beggining.
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {})
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return

    self._c_tbl.mod_entry(parsed_keys, parsed_data, ttl_reset=ttl_reset)
        '''
        code = code_str.format(method_name, param_str, self._name, param_docstring, method_name, parse_key_call, parse_data_call)
        mod_method = self._set_dynamic_method(code, method_name)
        self._mods[""] = (mod_method, param_list)

    # Generating mod_inc API dynamically
    def _create_mod_inc(self, key_fields, data_fields, code_str=None):
        method_name = "mod_inc"
        if method_name not in self._c_tbl.supported_commands:
            return

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields, data_fields)

        if code_str == None:
            code_str = '''
def {}(self, {} mod_flag=0):
    """ Incremental Add/Delete items in the fields that are array for the matched entry in {} table.

    Parameters:
    {}
    mod_flag: 0/1 value for Add/Delete Item in the field that is array type.
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {})
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return

    self._c_tbl.mod_inc_entry(parsed_keys, parsed_data, mod_flag)
        '''
        code = code_str.format(method_name, param_str, self._name, param_docstring, method_name, parse_key_call, parse_data_call)
        mod_method = self._set_dynamic_method(code, method_name)
        self._mods_inc[""] = (mod_method, param_list)

    # Generating delete API dynamically
    def _create_del(self, key_fields, code_str=None):
        method_name = "delete"
        if method_name not in self._c_tbl.supported_commands:
            return

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields=key_fields)

        if code_str == None:
            code_str = '''
def {}(self, {} handle=None):
    """Delete entry from {} table.

    Parameters:
    {}
    """
    parsed_keys, _ = self._c_tbl.parse_str_input("{}", {}, {})
    if parsed_keys == -1:
        return

    self._c_tbl.del_entry(parsed_keys, entry_handle=handle)
        '''
        code = code_str.format(method_name, param_str, self._name, param_docstring, method_name, parse_key_call, parse_data_call)
        del_method = self._set_dynamic_method(code, method_name)
        self._dels.append((del_method, param_list))

    # Generating get API dynamically
    def _create_get(self, key_fields, code_str=None):
        method_name = "get"
        if method_name not in self._c_tbl.supported_commands:
            return

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields=key_fields)

        if code_str == None:
            code_str = '''
def {}(self, {} regex=False, return_ents=True, print_ents=True, table=False, from_hw=False, handle=None):
    """Get entry from {} table.
    If regex param is set to True, perform regex search on key fields.
    When regex is true, default for each field is to accept all.
    We use the python regex search() function:
    https://docs.python.org/3/library/re.html

    Parameters:
    {}
    regex: default=False
    from_hw: default=False
    """
    if regex or table:
        etd = TableEntryDumper(self._c_tbl)
        sts = self._c_tbl.dump(etd, from_hw=from_hw, print_ents=print_ents)
        if sts == 0 and print_ents:
            if regex:
                etd.print_table(regex_strs={})
            else:
                etd.print_table()
        if return_ents:
            return etd.dump_entry_objs(regex_strs={})
        return
    parsed_keys, _ = self._c_tbl.parse_str_input("{}", {}, {})
    if parsed_keys == -1:
        return -1
    objs = self._c_tbl.get_entry(parsed_keys, print_entry=print_ents, from_hw=from_hw, entry_handle=handle)
    if return_ents:
        return objs
    return
        '''
        code = code_str.format(method_name, param_str, self._name, param_docstring, parse_key_call, parse_key_call, method_name, parse_key_call, parse_data_call)
        get_method = self._set_dynamic_method(code, method_name)
        self._gets.append((get_method, param_list))

    # Generating get_handle API dynamically
    def _create_get_handle(self, key_fields, code_str=None):
        method_name = "get_handle"
        if method_name not in self._c_tbl.supported_commands:
            return

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields=key_fields)

        if code_str == None:
            code_str = '''
def {}(self, {} regex=False, return_ents=True, print_ents=True, table=False, from_hw=False):
    """Get entry handle for specific key from {} table.
    If regex param is set to True, perform regex search on key fields.
    When regex is true, default for each field is to accept all.
    We use the python regex search() function:
    https://docs.python.org/3/library/re.html

    Parameters:
    {}
    regex: default=False
    from_hw: default=False
    handle: default=None
    """
    if regex or table:
        etd = TableEntryDumper(self._c_tbl)
        sts = self._c_tbl.dump(etd, from_hw=from_hw, print_ents=print_ents)
        if sts == 0 and print_ents:
            if regex:
                etd.print_table(regex_strs={})
            else:
                etd.print_table()
        if return_ents:
            return etd.dump_entry_objs(regex_strs={})
        return
    parsed_keys, _ = self._c_tbl.parse_str_input("{}", {}, {})
    if parsed_keys == -1:
        return -1
    objs = self._c_tbl.get_entry_handle(parsed_keys, print_entry=print_ents, from_hw=from_hw)
    if return_ents:
        return objs
    return
        '''
        code = code_str.format(method_name, param_str, self._name, param_docstring,  parse_key_call, parse_key_call, method_name, parse_key_call, parse_data_call)
        get_method = self._set_dynamic_method(code, method_name)
        self._gets.append((get_method, param_list))

    # Generating get_key API dynamically
    def _create_get_key(self, code_str=None):
        method_name = "get_key"
        if method_name not in self._c_tbl.supported_commands:
            return
        if code_str is None:
            code_str = '''
def {}(self, handle, return_ent=True, print_ent=True, from_hw=False):
    """Get entry key from {} table by entry handle.

    Parameters:
    handle: type=UINT64 size=32
    from_hw: default=False
    print_ents: default=True
    """
    if handle < 0 or handle >= 2**(sizeof(c_uint32)*8):
        return -1
    objs = self._c_tbl.get_entry_key(entry_handle=handle, print_entry=print_ent, from_hw=from_hw)
    if return_ent:
        return objs
    return
        '''
        code = code_str.format(method_name, self._name)
        self._set_dynamic_method(code, method_name)

    # Generating get_default API dynamically
    def _create_get_default(self, code_str=None):
        method_name = "get_default"
        if method_name not in self._c_tbl.supported_commands:
            return

        if code_str == None:
            code_str = '''
def {}(self, return_ent=True, print_ent=True, from_hw=False):
    """Get default entry from {} table.

    Parameters:
    from_hw: default=False
    print_ents: default=True
    table: default=False
    """
    objs = self._c_tbl.get_default_entry(print_entry=print_ent, from_hw=from_hw)
    if return_ent:
        return objs
    return
        '''
        code = code_str.format(method_name, self._name)
        self._set_dynamic_method(code, method_name)

    # Generating entry_with_<action> APIs dynamically
    def _create_entry_with_action(self, key_fields, data_fields, action_name):
        # The purpose of this function is for the user to be able to create an
        # and entry object and then eventually add it to the table or potentially
        # modify an existing entry in the table. Thus if both "add" and "mod" are
        # not supported by the table, then there no use of having this function in
        # which case we should simply return
        if "add" not in self._c_tbl.supported_commands and "mod" not in self._c_tbl.supported_commands:
            return

        full_strname = action_name.decode('ascii')
        strname = full_strname[full_strname.rfind('.') + 1:].replace("$","")
        method_name = "entry_with_{}".format(strname)

        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields, data_fields)

        code = '''
def {}(self, {}):
    """create a pending entry object for {} table with action: {}
    the entry object are programable and can later be pushed into current table

    Parameters:
    {}
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {}, b'{}')
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return
    return self._c_tbl.create_entry_obj(parsed_keys, parsed_data, b'{}')
        '''.format(method_name, param_str, self._name, full_strname, param_docstring, method_name,parse_key_call, parse_data_call, full_strname, full_strname)
        entry_method = self._set_dynamic_method(code, method_name)
        self._entries[full_strname] = (entry_method, param_list)

    def _create_entry(self, key_fields, data_fields):
        # The purpose of this function is for the user to be able to create an
        # entry object and then eventually add it to the table or potentially
        # modify an existing entry in the table. Thus if both "add" and "mod" are
        # not supported by the table, then there no use of having this function in
        # which case we should simply return
        if "add" not in self._c_tbl.supported_commands and "mod" not in self._c_tbl.supported_commands:
            return

        method_name = "entry"
        param_str, param_docstring, parse_key_call, parse_data_call, param_list = self._make_core_method_strs(method_name, key_fields, data_fields)

        code = '''
def {}(self, {}):
    """create an entry object for {} table if the entrie is not existed.
    Otherwise, it modify the existing entry.

    Parameters:
    {}
    """
    parsed_keys, parsed_data = self._c_tbl.parse_str_input("{}", {}, {})
    if parsed_keys == -1:
        return
    if parsed_data == -1:
        return
    return self._c_tbl.create_entry_obj(parsed_keys, parsed_data)
        '''.format(method_name, param_str, self._name, param_docstring, method_name, parse_key_call, parse_data_call)
        entry_method = self._set_dynamic_method(code, method_name)
        self._entries[""] = (entry_method, param_list)

class TDILrnLeaf(TDIContext):
    """
    This class creates easy to type, autocompleted python entrypoints to the
    TDI Runtime API. Each instance of the class represents one table. It exposes
    Add, modify commands for each action type (or one of each for the table if
    said table has no actions). It also exposes get and dump commands for
    retrieving information from TDI Runtime.
    """
    def __init__(self, name, c_tbl, cintf, parent_node=None):
        self._set_name(name)
        self._c_tbl = c_tbl
        self._cintf = cintf
        c_tbl.set_frontend(self)
        self._children = {}
        self._children["info"] = getattr(self, "info")
        self._children["callback_register"] = getattr(self, "callback_register")
        self._children["callback_deregister"] = getattr(self, "callback_deregister")
        if parent_node is not None and isinstance(parent_node, TDINode):
            self._parent_node = parent_node
            self._parent_node._add_child(self)
        else:
            self._parent_node = None

        self._set_docstring()

    def info(self, return_info=False, print_info=True):
        res = {
            "learn_name": self._name,
            "full_name": self._c_tbl.name,
            "fields": [],
        }
        if print_info:
            print("Learn Name: {}".format(res["learn_name"]))
            print("Full Name: {}".format(res["full_name"]))
            print("Fields:")
        field_rows = []
        headers = ["Name", "Size"]
        for info in sorted(self._c_tbl.fields.values(), key=lambda x: x.id):
            field = [info.name.decode('ascii'), info.size]
            field_rows += [field]
            res["fields"] += [{"name": field[0],
                                   "size": field[1]}]
        if print_info:
            print(tabulate.tabulate(field_rows, headers=headers))
        if return_info:
            return res

    def callback_register(self, callback=None):
        if callback is None:
            callback = _learn_fields_print
        self._c_tbl.callback_register(callback)

    def callback_deregister(self):
        self._c_tbl.callback_deregister()

    def _get_full_leaf_info(self):
        return [[self._c_tbl.name, 'LEARN', 'N/A', 'N/A', self]]

    def _get_children(self):
        return self._children

    def _test_all_adds(self):
        return 0

    def _simple_tester(self):
        return

    def _dep_tester(self, test_func):
        return 0

    """
    This function dynamically creates a docstring for our leaf exposing
    user-relevant information about key fields, actions, data fields for the
    corresponding table.
    """
    def _set_docstring(self):
        full_table_name = self._c_tbl.name
        key_docstring = ""

        for readable in self._c_tbl.field_readables:
            key_docstring += "    {}\n".format(readable)

        child_names = sorted(list(self._get_children().keys()))
        commands = ""
        for name in child_names:
            commands += "{}\n".format(name)

        docstr = """
TDI Runtime CLI node for {} learn

fields:
{}

Available Commands:
{}
        """.format(full_table_name, key_docstring, commands).strip()
        self.__doc__ = docstr


"""
Call this function on the root node of the TDI Runtime CLI to generate the
docstrings for the organizational nodes. Note that leaves have already
generated their docstrings during initialization.
"""
def set_node_docstrs(node):
    if isinstance(node, TDINode):
        node._set_doc()
        for child in node._children:
            set_node_docstrs(child)

def get_readable_table_name(name, tree, prev):
    pnames = name.split('.')
    if len(tree[pnames[-1]].keys()) < 2:
        if pnames[-1] not in prev:
            return pnames[-1]
    return '_'.join(pnames)


"""
The P4 program name given might be an invalid Python identifier or it may
overwrite another object with the same name either in the parent node or
in the global name context. This function will try to check and resolve these
conflicts, if possible, by replacing incorrect symbols with underscores and
prepending the name by 'p4_' prefix.
"""
def validate_program_name(p4_name, p_node, fixed_nodes=None):
    def print_name_warn_(err_txt_, o_name_, n_name_):
        print("WARNING: The P4 program name '%s' is " %(o_name_) + err_txt_ + ". It is changed to '%s'." %(n_name_), file = sys.stderr)
    #
    p4_name_str_ = p4_name.decode('ascii')
    p4_name_res_ = p4_name_str_
    p4_pref_ = (p4_name_str_[0:3].upper() == "P4_")

    if not p4_name_str_.isidentifier():
        p4_name_res_ = re.sub(r"[^\w]", "_", p4_name_str_, flags = re.ASCII)
        if not p4_pref_:
            p4_name_res_ = "p4_" + p4_name_res_
        print_name_warn_("not a valid Python identifier", p4_name_str_, p4_name_res_)
        p4_name_str_ = p4_name_res_
        p4_pref_ = True

    if not p4_pref_ and keyword.iskeyword(p4_name_str_):
        p4_name_res_ = "p4_" + p4_name_str_
        print_name_warn_("a Python keyword", p4_name_str_, p4_name_res_)
        p4_name_str_ = p4_name_res_
        p4_pref_ = True

    if not p4_pref_ and p4_name_str_ in globals():
        p4_name_res_ = "p4_" + p4_name_str_
        print_name_warn_("in use by tdi_python", p4_name_str_, p4_name_res_)
        p4_name_str_ = p4_name_res_
        p4_pref_ = True
    if p4_name_str_ in globals():
        print("ERROR: The P4 program name '%s' is in use by tdi_python." %(p4_name_str_), file = sys.stderr)
        return ""

    if p_node is not None and isinstance(p_node, TDINode):
        if not p4_pref_ and (p4_name_str_ in dir(p_node) or p4_name_str_ in fixed_nodes):
            p4_name_res_ = "p4_" + p4_name_str_
            print_name_warn_("reserved for another item in the object tree", p4_name_str_, p4_name_res_)
            p4_name_str_ = p4_name_res_
            p4_pref_ = True
        if p4_name_str_ in dir(p_node) or p4_name_str_ in fixed_nodes:
            print("ERROR: The P4 program name '%s' is reserved for another item in the object tree." %(p4_name_str_), file = sys.stderr)
            return ""
    #
    return p4_name_res_

def update_node_tree(parent_node, prefs, cintf):
    for p in prefs[:-1]:
        contained = False
        next_node = None
        for child in parent_node._children:
            if p == child._name:
                contained = True
                next_node = child
                break
        if not contained:
            parent_node = TDINode(p, cintf, parent_node=parent_node)
        else:
            parent_node = next_node
    return parent_node

def is_node(obj):
    if isinstance(obj, TDINode) or isinstance(obj, TDILeaf):
        return True
    return False

def make_deep_tree(p4_name, tdi_info, dev_node, cintf, fixed_nodes=None):
    if p4_name != b"$SHARED":
        p4_name_str_ = validate_program_name(p4_name, dev_node, fixed_nodes)
        if len(p4_name_str_) == 0:
            return -1
        p4_node = TDINode(p4_name_str_, cintf, parent_node=dev_node)
    else:
        p4_name_str_ = p4_name
    # Sort tables in reverse order, so nested table children are added first.
    for table_name, tbl_obj in sorted(tdi_info.tables.items(), reverse=True):
        logging.debug("table_name:"+str(table_name))
        prefs = table_name.split('.')
        if prefs[0] in fixed_nodes:
            parent_node = update_node_tree(dev_node, prefs, cintf)
        else:
            parent_node = update_node_tree(p4_node, prefs, cintf)
        # Only add the table as a leaf if it is ready and also the same table already doesn't
        # exist under the same parent. Which is possible while adding fixed tables for
        # multiprogram scenario
        if tbl_obj.table_ready:
            # In nested table, children can be a string, need to filter this out
            # only for node class instances.
            node_list = []
            for c in parent_node._children:
                if is_node(c):
                    node_list.append(c._name)

            # If there is no node add it
            if prefs[-1] not in node_list:
                cintf.leaf_type_cls(name=prefs[-1], c_tbl=tbl_obj, cintf=cintf, parent_node=parent_node)
            # If it is nested table recreate it with proper list of children and
            # update the parent, but don't modify fixed nodes.
            elif prefs[0] not in fixed_nodes:
                for c in parent_node._children:
                    if is_node(c) and c._name == prefs[-1]:
                        cintf.leaf_type_cls(name=prefs[-1], c_tbl=tbl_obj, cintf=cintf, parent_node=parent_node, children=c._children)
                        parent_node._children.remove(c)
                        break

    #
    dev_node.p4_programs_list.append(p4_name_str_)
    return 0

"""
This function creates the global state required to manage context switching
between CLI nodes. Note that it never unloads the tdi node, so the full
command tree is still available from any context.
"""
def setup_context():
    global _tdi_context
    _tdi_context['cur_context'] = []
    _tdi_context['parent'] = None
    _tdi_context['cur_node'] = None

"""
This function sets the command line prompt to the parameterized string.
"""
def set_prompt(next_node='tdi_python'):
    global prompt_node
    prompt_node = next_node
    class NextPrompt(IPython.terminal.prompts.Prompts):
        def in_prompt_tokens(self, cli=None):
            return [(IPython.terminal.prompts.Token, prompt_node), (IPython.terminal.prompts.Token.Prompt, '> ')]
    try:
        ipy = IPython.get_ipython()
        ipy.prompts = NextPrompt(ipy)
    except:
        pass

"""
This function is called by IPython when '..' is entered on the command line.
If the currently loaded context's node has a parent, the parent's commands
replace the current ones in the global scope. Otherwise, the currently loaded
commands, if any, are removed and we reset to the "root" scope.
"""
def set_tdi_parent_context():
    if _tdi_context['parent'] is None:
        for name in _tdi_context['cur_context']:
            delattr(sys.modules['__main__'], name)
        _tdi_context['cur_context'] = []
        set_prompt()
        _tdi_context['cur_node'] = None
        return
    _tdi_context['parent']()

"""
By default, python uses the C default values for stdin, stdout, stderr.
Replace these with the terminal connection fd associated with the caller
of TDI Runtime CLI. Also set the working directory to driver's install dir.
"""
def set_io(in_fd, out_fd):
    inf = open(in_fd, closefd=False)
    outf = open(out_fd, mode='w', closefd=False)
    sys.stdin = inf
    sys.stdout = outf
    sys.stderr = outf
    print("cwd : {}\n".format(os.getcwd()))
    import pydoc
    pydoc.help._input = sys.stdin
    pydoc.help._output = sys.stdout
    return (inf, outf)

"""
Instead of invoking less pager in the shell, we do plain io write
the function will effectively disable the pager to avoid terminal io disorder
"""
def page_printer(data, start=0, screen_lines=0, pager_cmd=None):
    if isinstance(data, dict):
        data = data['text/plain']
    print(data)

def ipython_reinitialize_io():
    ipython_app.input = create_input()
    ipython_app.output = create_output()
    session = get_app_session()
    session._input = ipython_app.input
    session._output = ipython_app.output
    if not ipython_app.shell.pt_app is None:
        ipython_app.shell.pt_app.app.output = ipython_app.output
        ipython_app.shell.pt_app.app.input = ipython_app.input
        ipython_app.shell.pt_app.app.renderer.output = ipython_app.output

def tdi_input_transform(lines):
    new_lines = []
    for line in lines:
        if line == "?\n" or line == ".\n":
            if(_tdi_context['cur_node'] == None):
                new_lines.append(f"?\n")
            else:
                new_lines.append(f"print(_tdi_context['cur_node'].__doc__)\n")
        elif line == "..\n":
            new_lines.append("set_tdi_parent_context()\n")
        elif line == ".?\n":
            if(_tdi_context['cur_node'] == None):
                new_lines.append(f"?\n")
            else:
                # it grabs the full symbol path and append with ? to display help message
                new_lines.append(f"{IPython.get_ipython().prompts.in_prompt_tokens()[0][1]}?\n")
        else:
            new_lines.append(line)
    return new_lines

"""
choose which prompt to use, return True if we use simple_prompt or vt100 if function returns False
- vt100 terminal if stdin, stdout, stderr file descriptor refers to a terminal
- simple_prompt if stdin, stdout, stderr file descriptor not refers to a terminal
"""
def use_simple_prompt():
    for name in ('stdin', 'stdout', 'stderr'):
        stream = getattr(sys, name)
        if not stream or not hasattr(stream, 'isatty') or not stream.isatty():
            is_tty = False
            break
    else:
        is_tty = True

    return ('IPY_TEST_SIMPLE_PROMPT' in os.environ) or (not is_tty)

"""
# load_ipython_extension is the the symbol for ipython extension
  The `ipython` argument is the currently active `InteractiveShell`
  instance, which can be used in any way. This allows you to register
  new magics or aliases, for example.
  We use this namesapce to maniuplate what user can type on cli directly
  During setup_context, command like dump, info will be callable without tdi prefix
"""
def load_ipython_extension(ipython):
    setattr(sys.modules['__main__'], "_tdi_context", _tdi_context)
    setattr(sys.modules['__main__'], "set_tdi_parent_context", set_tdi_parent_context)
    setattr(sys.modules['__main__'], "tdi", tdi)
    if not tdi_input_transform in ipython.input_transformers_cleanup:
        ipython.input_transformers_cleanup.append(tdi_input_transform)
    IPython.core.usage.interactive_usage = """
TDI-PYTHON Usage:

.    : examine the current object (context) with documents/helps (like cd .)
..   : go up one layer for tdi object context (like cd ..)
?    : same as .
?/?? : examine documents on tdi objects. e.g. tdi? or .?
tdi  : the global symbol for accessing tdi objects interactively

Please checkout ipython syntax for interactive usages.
Otherwise python syntax/conventions are exacly the same.

Tips:

%load   : can load python script from filesystem without running
%run -i : can run python script within current namespace (with symbols available)
%edit   : edit the file in $EDITOR
!pwd    : invoke system shell to run pwd command
%%capture name : capture long console output to the name variable

"""

def unload_ipython_extension(ipython):
    if tdi_input_transform in ipython.input_transformers_cleanup:
        ipython.input_transformers_cleanup.remove(tdi_input_transform)
    pass

def destroy_tdi_python_session(dev_id, driver_intf):
    if dev_id not in tdi_session_dict:
        return 0
    sts = driver_intf.tdi_session_destroy(tdi_session_dict[dev_id])
    if not sts == 0:
        print("ERROR: destroying session for device:", dev_id)
        return -1
    del tdi_session_dict[dev_id]
    return 0

def create_tdi_python_session(dev_id, driver_intf, device_hdl):
    if dev_id in tdi_session_dict:
        return 0
    tdi_session_dict[dev_id] = sess_type()
    sts = driver_intf.tdi_session_create(device_hdl, byref(tdi_session_dict[dev_id]))
    if not sts == 0:
        print("Error, unable to create TDI Runtime session")
        return -1
    return 0


def tdi_exit_handler():
    unload_ipython_extension(ipython_app.shell)
    for name in _tdi_context['cur_context']:
        delattr(sys.modules['__main__'],name)
    delattr(sys.modules['__main__'], "_tdi_context")
    delattr(sys.modules['__main__'], "set_tdi_parent_context")
    delattr(sys.modules['__main__'], "tdi")

    sys.stdin = sys.__stdin__
    sys.stdout = sys.__stdout__
    sys.stderr = sys.__stderr__

class TdiCli:
    """
    Initialize TDI Runtime CLI, create IPython's configuration, start TDI Runtime
    CLI, and reset python's IO streams before teardown.
    """
    # Fixed Tables created at child Nodes of the root 'tdi' Node.
    # targets to override this list with their fixed nodes.
    fixed_nodes = []

    def start_tdi(self, in_fd, out_fd, dev_id_list, udf=None, interactive=False, cIntf_cls=None):
        inf, outf = set_io(in_fd, out_fd)

        global tdi_session_dict
        if hasattr(sys.modules['__main__'], "tdi_session_dict"):
            tdi_session_dict = getattr(sys.modules['__main__'], "tdi_session_dict")
        else:
            setattr(sys.modules['__main__'], "tdi_session_dict", tdi_session_dict)

        # set level to DEBUG to see the debug infomration
        logging.basicConfig(level=logging.ERROR)
        sts = self.load_tdi(dev_id_list)
        if not sts == 0:
            return sts

        # Workaround to prevent Python from installing its SIGWINCH signal handler
        import threading
        threading.current_thread().__class__.__name__ = "tdi"

        global ipython_app

        # save udf in storage, this part of code can be executed by two ways:
        # - using ipython_app.initialize by ipython internally
        # - using ipython_app._run_exec_files to run it manually
        # we run ipython_app.initialize once time to prevent memory leakage,
        #  next runs will be manually using _run_exec_files command
        exec_files_list = []
        if udf is not None:
            exec_files_list.append(udf)

        if hasattr(sys.modules['__main__'], "ipython_app"):
            ipython_app = getattr(sys.modules['__main__'], "ipython_app")
        if ipython_app is None:
            print("Devices found : ", dev_id_list)
            c = Config()
            c.Completer.use_jedi = False
            c.IPCompleter.use_jedi = False
            c.InteractiveShell.autocall = 2
            c.TerminalInteractiveShell.autocall = 2
            c.ZMQInteractiveShell.autocall = 2
            c.InteractiveShell.automagic = False
            c.TerminalInteractiveShell.automagic = False
            c.ZMQInteractiveShell.automagic = False
            c.TerminalInteractiveShell.display_page = False
            c.TerminalInteractiveShell.simple_prompt = use_simple_prompt()
            c.ZMQInteractiveShell.display_page = True
            # save udf in exec_files, that will be executed by ipython internally
            # on ipython_app initialize step
            c.InteractiveShellApp.exec_files = exec_files_list
            c.InteractiveShellApp.extensions = [
                'tdicli'
            ]

            # save TerminalIPythonApp and TerminalInteractiveShell for reusing it in next iteration
            ipython_app = IPython.terminal.ipapp.TerminalIPythonApp(config=c)
            ipython_app.init_shell()

            # reinitialize input output streams in case switching api
            ipython_reinitialize_io()
            ipython_app.initialize()
            setattr(sys.modules['__main__'], "ipython_app", ipython_app)

        else:

            # use saved instances of TerminalIPythonApp, but we need reinitialize input output streams
            # for ability use new shell from another terminal
            ipython_reinitialize_io()
            load_ipython_extension(ipython_app.shell)
            if udf is not None:
                ipython_app.exec_files = exec_files_list
                ipython_app._run_exec_files()
            set_tdi_parent_context()

        if udf is not None:
            if interactive:
                ipython_app.start()
        else:
            ipython_app.start()

        tdi_exit_handler()
        inf.close()
        outf.close()
        return 0

    def load_tdi(self, dev_id_list):
        sts = self.populate_tdi(dev_id_list)
        if sts == -1:
            print("TDI Runtime CLI init failed.", file = sys.stderr)
            return -1
        setup_context()
        tdi.reload = self.load_tdi
        return 0

    """
    This function initializes the TDI Runtime CLI objects, generating a tree of
    objects that serve as CLI command nodes.
    """
    def populate_tdi(self, dev_id_list):
        global tdi
        # tdi node shouldn't have a cintf since cintf is dev dependent
        tdi = TDINode("tdi", None)
        tdi.device_list =  dev_id_list
        # For each device_id, create a cintf and a dev node
        # If only one device is present, then skip creating the
        # device node for now for backward compatibility.
        # TODO take care of it later especially when device level
        # APIs are introduced.
        single_device = True
        if len(dev_id_list) > 1:
            single_device = False
        for dev_id in dev_id_list:
            cintf = self.cIntf_cls(dev_id, TdiTable, TdiInfo)
            if cintf == -1:
                return -1
            if single_device:
                tdi = TDINode("tdi", cintf, parent_node=None)
                tdi.device_list =  dev_id_list
                dev_node = tdi
            else:
                dev_node = TDINode("dev_"+str(dev_id), cintf, parent_node=tdi)

            #
            # This function fills dev_node with options from cintf
            # To be overriden by targets as needed
            # TODO: Need to find a better way of doing this
            #
            self.fill_dev_node(cintf, dev_node)
            for p4_name, tdi_info in cintf.infos.items():
                print("Creating tree for dev %d and program %s\n" %(dev_id, p4_name.decode()))
                if 0 != make_deep_tree(p4_name, tdi_info, dev_node, cintf, self.fixed_nodes):
                    print("ERROR: Can't create object tree for tdi_python.", file = sys.stderr)
                    return -1

            sts = create_tdi_python_session(dev_id, dev_node._cintf._driver, dev_node._cintf.get_device())
            if sts != 0:
                return sts

        self.create_warm_init_node(tdi)
        set_node_docstrs(tdi)
        return 0

    """
        To be overriden by the targets on need basis
    """
    def fill_dev_node(self, cintf, dev_node):
        pass

    """
        To be overwritten by targets with custom implementation of warm_init_begin/end functions
    """
    def create_warm_init_node(self, tdi):
        warm_init_node = TDINode("warm_init", None, parent_node=tdi)

        def warm_init_begin(self):
            print("Not supported")

        def warm_init_end(self):
            print("Not supported")

        # add warm_init APIs to the node
        warm_init_node.warm_init_begin = types.MethodType(warm_init_begin, warm_init_node)
        warm_init_node.warm_init_end = types.MethodType(warm_init_end, warm_init_node)
        warm_init_node._commands = {}
        warm_init_node._commands["warm_init_begin"] = getattr(warm_init_node, "warm_init_begin")
        warm_init_node._commands["warm_init_end"] = getattr(warm_init_node, "warm_init_end")

