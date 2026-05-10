# SPDX-FileCopyrightText: 2021 Intel Corporation
# Copyright (C) 2021 Intel Corporation.
#
# SPDX-License-Identifier: Apache-2.0
import json

class TableEntry:
    def __init__(self, table, key, data, action=None):
        self._c_tbl = table
        self._cintf = table.get_cintf()
        self.key = key
        self.data = data
        if isinstance(action, bytes):
            action = action.decode('ascii')
        self.action = action

    def _get_raw_key(self):
        return self.key

    def _get_raw_data(self):
        return self.data

    def _get_raw_action(self):
        return self.action

    def push(self, verbose=False):
        if verbose:
            print("Checking for entry...")
        if self._c_tbl.get_entry(self.key, print_entry=False) != -1:
            if verbose:
                print("Entry found. Modifying existing entry.")
            if self.action is not None:
                self._c_tbl.mod_entry(self.key, self.data, self.action.encode('ascii'))
            else:
                self._c_tbl.mod_entry(self.key, self.data, self.action)
        else:
            if verbose:
                print("Adding entry to table.")
            if self.action is not None:
                self._c_tbl.add_entry(self.key, self.data, self.action.encode('ascii'))
            else:
                self._c_tbl.add_entry(self.key, self.data, self.action)

    def update(self):
        entry = self._c_tbl.get_entry(self.key, print_entry=False)
        if isinstance(entry, list):
            if len(entry > 1):
                print("TDI Runtime CLI Internal Error! Duplicate entry for key found.")
                return
            entry = entry[0]
        if entry == -1:
            print("Entry not found in table {}".format(self._c_tbl.name))
            return
        self.key = entry._get_raw_key()
        self.data = entry._get_raw_data()
        self.action = entry._get_raw_action()

    def remove(self):
        self._c_tbl.del_entry(self.key)

    def json(self):
        return json.dumps(self._c_tbl.raw_entry(self.key, self.data, self.action))

    def raw(self):
        return self._c_tbl.raw_entry(self.key, self.data, self.action)

    def default_entry_block(self):
        print("push, update, and remove are not available for default entry.")
        print("use <table_node>.get/set_default() instead.")

    def __str__(self):
        saction = None
        if self.action is not None:
            saction = self.action.encode('ascii')
        to_print, _ = self._c_tbl.print_entry(self.key, self.data, saction)
        return "Entry for {} table.\n{}".format(self._c_tbl.name, to_print)

    def __repr__(self):
        return "Entry for {} table.\n".format(self._c_tbl.name)
