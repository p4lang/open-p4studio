# SPDX-FileCopyrightText: 2021 Intel Corporation
# Copyright (C) 2021 Intel Corporation.
#
# SPDX-License-Identifier: Apache-2.0
from __future__ import print_function
from ctypes import *
import pdb

class TdiInfo:

    """
    This class contains abstractions on the TDI Runtime C API for all
    tables described by a TDI Runtime Info object. This includes infos
    for both p4 programs and for pd_fixed APIs exposed through
    TDI Runtime.
    """
    def __init__(self, cintf, name):
        self._cintf = cintf
        self.name = name
        self.tbl_id_map = {}
        self.tbl_dep_map = {}
        self.lrn_id_map = {}
        self.nested_tables = []
        sts = self._init_handle()
        if not sts == 0:
            print("TdiInfo init hanle failed for {}!".format(self.name))
            return -1
        sts = self._init_tables()
        if not sts == 0:
            print("TdiInfo init tables failed for {}!".format(self.name))
            return -1

    def _init_handle(self):
        # get tdi_info handler
        self._handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_info_get(self._cintf.get_dev_id(), self.name, byref(self._handle))
        if not sts == 0:
            print("CLI Error: get info failed for {}".format(self.name))
        return sts

    def _init_tables(self):
        num_tables = c_int(-1)
        sts = self._cintf.get_driver().tdi_num_tables_get(self._handle, byref(num_tables))
        if not sts == 0:
            print("CLI Error: get num tables for {} failed with status {}.".format(self.name, self._cintf.err_str(sts)))
            return sts
        array_type = self._cintf.handle_type * num_tables.value
        tables = array_type()
        sts = self._cintf.get_driver().tdi_tables_get(self._handle, tables)
        if not sts == 0:
            print("CLI Error: get table handles for {} failed with status {}.".format(self.name, self._cintf.err_str(sts)))
            return sts
        # Python Tables Object Initialzation
        # print("{:40s} | {:30s} | {:10s}".format("TableName","Table Type","Status"))
        self.tables = {}
        for table in tables:
            table_info = self._cintf.handle_type()
            self._cintf.get_driver().tdi_table_info_get(table, byref(table_info));
            tbl_obj = self._cintf.TdiTable(self._cintf, table, self, table_info)
            if tbl_obj == -1:
                print("CLI Error: bad table object init")
                return -1
            elif tbl_obj.table_type_cls.table_type_str(tbl_obj.get_type()) == "INVLD":
                print("CLI Error: bad table type init")
                return -1
            else:
                tbl_id = c_uint(0)
                sts = self._cintf.get_driver().tdi_table_id_from_handle_get(table_info, byref(tbl_id))
                tbl_obj.set_id(tbl_id.value)
                has_const_default_action = c_bool(False)
                sts = self._cintf.get_driver().tdi_table_has_const_default_action(table_info,
                        byref(has_const_default_action))
                tbl_obj.set_has_const_default_action(has_const_default_action)
                self.tables[tbl_obj.name] = tbl_obj
                self.tbl_id_map[tbl_id.value] = tbl_obj
        # Tables Dependencies Initialzation
        for tbl_id, tbl_obj in self.tbl_id_map.items():
            table_hdl = self._cintf.handle_type()
            self._cintf.get_driver().tdi_table_from_id_get(self._handle, tbl_id, byref(table_hdl))
            table_info = self._cintf.handle_type()
            self._cintf.get_driver().tdi_table_info_get(table_hdl, byref(table_info))
            num_deps = c_int()
            self._cintf.get_driver().tdi_num_tables_this_table_depends_on_get(table_info, byref(num_deps))
            if num_deps.value == 0:
                continue
            array_type = c_uint * num_deps.value
            deps = array_type()
            self._cintf.get_driver().tdi_tables_this_table_depends_on_get(table_info, deps)
            self.tbl_dep_map[tbl_id] = deps
            # Nested tables are to be included in the parent table from depends_on field
            if tbl_obj.table_type in self.nested_tables:
                prefix = self.tbl_id_map[deps[0]].name
                # New name is used to create node tree
                new_name = prefix + tbl_obj.name[tbl_obj.name.rfind('.'):]
                self.tables[new_name] = tbl_obj
                del self.tables[tbl_obj.name]
                tbl_obj.name = new_name
        return 0

    def _init_learns(self):
        num_learns = c_int(-1)
        sts = self._cintf.get_driver().tdi_num_learns_get(self._handle, byref(num_learns))
        if not sts == 0:
            print("CLI Error: get num learns for {} failed with status {}.".format(self.name, self._cintf.err_str(sts)))
            return sts
        array_type = self._cintf.handle_type * num_learns.value
        learns = array_type()
        sts = self._cintf.get_driver().tdi_learns_get(self._handle, learns)
        if not sts == 0:
            print("CLI Error: get learn handles for {} failed with status {}.".format(self.name, self._cintf.err_str(sts)))
            return sts
        self.learns = {}
        for learn in learns:
            lrn_obj = self._cintf.TdiLearn(self._cintf, learn, self)
            if lrn_obj == -1:
                return -1
            lrn_id = c_uint(0)
            sts = self._cintf.get_driver().tdi_learn_id_get(learn, byref(lrn_id))
            lrn_obj.set_id(lrn_id.value)
            self.learns[lrn_obj.name] = lrn_obj
            self.lrn_id_map[lrn_id.value] = lrn_obj
        return 0
