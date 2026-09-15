# SPDX-FileCopyrightText: 2021 Intel Corporation
# Copyright (C) 2021 Intel Corporation.
#
# SPDX-License-Identifier: Apache-2.0
from __future__ import print_function
from ctypes import *
from ipaddress import ip_address as ip
from netaddr import EUI as mac
from tdiTableEntry import TableEntry
import pdb
import json
import time
import logging
from tdiDefs import *

class TdiTableError(Exception):
    def __init__(self, str_rep, table_obj, sts, *args,**kwargs):
        self.sts = sts
        self.table = table_obj
        self.str_rep = str_rep
        Exception.__init__(self, str_rep, *args,**kwargs)

class TdiTable:

    key_match_type_cls = KeyMatchType
    data_type_cls = DataType
    table_type_cls = TableType
    attributes_type_cls = AttributesType
    operations_type_cls = OperationsType
    flags_type_cls = FlagsType
    table_entry_cls = TableEntry

    """
    This class manages the exchange of information between
    the CLI and TDI Runtime's C API. It has two primary parts:
      - Looking up the action and field metadata for a table
      - Parsing and reformatting data as the user makes calls

    Note that keys in this object are the c-string representation
    (byte-streams in python) of data, not python strings.
    """
    def __init__(self, cintf, handle, info, info_handle):
        self._cintf = cintf
        self._tdi_info = info
        self._handle = handle
        self._info_handle = info_handle
        # get get_device
        self._device_hdl = cintf.get_device()
        self.key_field_readables = []
        self.data_field_readables = []
        self.action_readables = {}
        self.action_data_readables = {}
        self.tdi_id = -1
        self.frontend = None
        self.idle_tmo_callback = None
        self.register_sync_cb = None
        self.counter_sync_cb = None
        self.hit_state_update_cb = None
        self.port_status_notif_callback = None
        self.selector_table_update_cb = None
        self.string_choices = {}
        self.annotations = {}
        self.has_const_default_action = False
        self.unimplemented_tables = ["TODO"]
        self.table_type = self.table_type_cls.table_type_str((self.get_type()))
        self.table_ready = False if self.table_type in self.unimplemented_tables else True
        #
        # Ignore INVALID Table Type from Driver
        #
        if self.table_type_cls.table_type_str(self.get_type()) == "INVLD":
            return
        #
        # Determine the **Table Name** from Driver
        #
        table_name = c_char_p()
        sts = self._cintf.get_driver().tdi_table_name_get(self._info_handle, byref(table_name))
        if not sts == 0:
            print("CLI Error: get table name failed. [{}]".format(self._cintf.err_str(sts)))
            raise TdiTableError("Table init name failed.", None, -1)

        #
        # this function to be overriden by targets on need basis
        # TODO: Need to find a better way of doing this
        #
        self.modify_table_names(table_name)
        self.supported_commands = ["info", "entry", "string_choices"]
        self.set_supported_attributes_to_supported_commands()
        self.set_supported_operations_to_supported_commands()
        self.set_supported_apis_to_supported_commands()
        if not self.table_ready:
            return
        #
        # Determine the **Table Key**, **Table Data**, and **Table Action** from Driver
        #
        sts = self._init_key()
        if not sts == 0:
            print("CLI Error: Init key fields for table {} failed.".format(self.name))
            raise TdiTableError("Table init field failed.", None, -1)

        sts = self._init_actions()
        if not sts == 0:
            print("CLI Error: Init actions for table {} failed.".format(self.name))
            raise TdiTableError("Table init field failed.", None, -1)

        sts = self._init_data()
        if not sts == 0:
            print("CLI Error: Init data fields for table {} failed.".format(self.name))
            raise TdiTableError("Table init field failed.", None, -1)
        # Python only allows up to 255 literal function arguments
        # we exclude some fix args and
        # prepare room for lmp/tenary type params that doubles the param list
        self.compress_input = False
        if len(self.key_fields) * 2 + len(self.data_fields) > (255-10):
            self.compress_input = True

    def set_frontend(self, tdi_leaf):
        self.frontend = tdi_leaf

    def set_id(self, tdi_id):
        self.tdi_id = tdi_id

    def set_has_const_default_action(self, value):
        self.has_const_default_action = value

    def get_id(self):
        return self.tdi_id

    def get_cintf(self):
        """@brief return the cintf associated with this table object
        """
        return self._cintf

    class TdiTableField:

        """
        This class acts as a storage container for table field info. It
        also exposes a parse method for transforming user input into data
        the TDI Runtime C API can accept and a deparse method for printing
        API outputs in an easy to read format.
        """
        def __init__(self, name, id_, size, is_ptr, read_only, required, category, tbl, action_name=None, action_id=None, data_type_=None, type_=None, is_cont_field=False):
            self.name = name
            self.id = id_
            self.type = type_ # match type for key only
            self.data_type = data_type_
            self.size = size
            self.is_ptr = is_ptr
            self.read_only = read_only
            self.required = required
            self.category = category
            self.action_name = action_name
            self.action_id = action_id
            self.table = tbl
            self.is_cont_field = is_cont_field
            self.ipv4addr = False
            self.annotations = []
            self._init_choices()
            self._init_annotations()
            self._cont_data_fields = {}

        def _init_annotations(self):
            nannotations_func = None
            get_annotations_func = None
            # Container fields may be anything but are passed as data.
            # Annotations are not supported.
            if self.category == "data" and self.is_cont_field == False:
                if self.action_id is None:
                    nannotations_func = self.table._cintf.get_driver().tdi_data_field_num_annotations_get
                    get_annotations_func = self.table._cintf.get_driver().tdi_data_field_annotations_get
                else:
                    nannotations_func = self.table._cintf.get_driver().tdi_data_field_num_annotations_with_action_get
                    get_annotations_func = self.table._cintf.get_driver().tdi_data_field_annotations_with_action_get
            else:
                return

            annotations = []
            num_annotations = c_uint(0)
            if self.action_id is None:
                sts = nannotations_func(self.table._info_handle, self.id, byref(num_annotations))
            else:
                sts = nannotations_func(self.table._info_handle, self.id, self.action_id, byref(num_annotations))
            if sts != 0:
                print("Error: num annotations for field {} in table {} failed. [{}]".format(self.name, self.table.name, self.table._cintf.err_str(sts)))
                return
            arr_type = self.table._cintf.annotation_type * num_annotations.value
            annotations_arr = arr_type()
            if self.action_id is None:
                sts = get_annotations_func(self.table._info_handle, self.id, byref(annotations_arr))
            else:
                sts = get_annotations_func(self.table._info_handle, self.id, self.action_id, byref(annotations_arr))
            if sts != 0:
                print("Error: get annotations for field {} in table {} failed. [{}]".format(self.name, self.table.name, self.table._cintf.err_str(sts)))
                return
            for ann in annotations_arr:
                logging.debug("num ann {} table name {}  field name = {} annotations = {} {} ".format(str(num_annotations.value), self.table.name,  str(self.name), str(ann.name), str(ann.value.decode('ascii'))))
                annotations += [(ann.name.decode('ascii'), ann.value.decode('ascii'))]
            self.annotations = annotations

        def _init_choices(self):
            nchoices_func = None
            get_choices_func = None
            if self.category == "key":
                if self.table.data_type_cls.data_type_str(self.data_type) != "STRING":
                    return
                nchoices_func = self.table._cintf.get_driver().tdi_key_field_num_allowed_choices_get
                get_choices_func = self.table._cintf.get_driver().tdi_key_field_allowed_choices_get
            else:
                if self.table.data_type_cls.data_type_str(self.data_type) != "STRING" and self.table.data_type_cls.data_type_str(self.data_type) != "STR_ARR":
                    return
                if self.action_id is None:
                    nchoices_func = self.table._cintf.get_driver().tdi_data_field_num_allowed_choices_get
                    get_choices_func = self.table._cintf.get_driver().tdi_data_field_allowed_choices_get
                else:
                    nchoices_func = self.table._cintf.get_driver().tdi_data_field_num_allowed_choices_with_action_get
                    get_choices_func = self.table._cintf.get_driver().tdi_data_field_allowed_choices_with_action_get

            choices = []
            num_choices = c_uint(0)
            if self.action_id is None:
                sts = nchoices_func(self.table._info_handle, self.id, byref(num_choices))
            else:
                sts = nchoices_func(self.table._info_handle, self.id, self.action_id, byref(num_choices))
            if sts != 0:
                print("Error: num choices for field {} in table {} failed. [{}]".format(self.name, self.table.name, self.table._cintf.err_str(sts)))
                return
            arr_type = c_char_p * num_choices.value
            choices_arr = arr_type()
            if self.action_id is None:
                sts = get_choices_func(self.table._info_handle, self.id, choices_arr)
            else:
                sts = get_choices_func(self.table._info_handle, self.id, self.action_id, choices_arr)
            if sts != 0:
                print("Error: get choices for field {} in table {} failed. [{}]".format(self.name, self.table.name, self.table._cintf.err_str(sts)))
                return
            for choice in choices_arr:
                choices += [choice.decode('ascii')]
            self.table.string_choices[self.name.decode('ascii')] = choices

        # Try to parse an integer regardless of how the user gave us the data.
        def _parse_int(self, value, skip_size_check=False, mask_type=False):
            if value is None:
                if self.read_only:
                    return True, None
                if not self.required or (
                           self.table.table_type_cls.table_type_str(self.table.get_type()) == "MATCH_INDIRECT_SELECTOR" and
                           self.name.decode('ascii') in ["$ACTION_MEMBER_ID", "$SELECTOR_GROUP_ID"]):
                    return True, None
                value = 0
                if mask_type:
                    value = (1 << self.size) - 1
            elif self.read_only:
                print("Skipping input for read only field: {}".format(self.name))
                return True, None
            try:
                value_saved = value
                if isinstance(value, str):
                    if not value.find('.') == -1:
                        value = ip(value)
            except:
                value = value_saved
            try:
                value_saved = value
                if isinstance(value, str):
                    if not value.find('-') == -1:
                        value = mac(value)
            except:
                value = value_saved
            try:
                value_saved = value
                if isinstance(value, str):
                    if not value.find(':') == -1:
                        value = mac(value)
            except:
                value = value_saved
            try:
                if isinstance(value, str):
                    parsed = int(value, 0)
                else:
                    parsed = int(value)
            except Exception as e:
                print("Error: {}".format(str(e)))
                return False, -1
            if mask_type and parsed < 0:
                parsed = parsed & ((1 << self.size) - 1)
            if skip_size_check:
                return True, parsed
            if (parsed >> self.size) > 0:
                raise TdiTableError("Error: input {} (parsed: {}) for {} field {} is greater than {} bits.".format(value, parsed, self.category, self.name, self.size), self.table, -1)
            return True, parsed

        # Try to parse a float regardless of how the user gave us the data.
        def _parse_float(self, value):
            if value is None:
                if self.read_only:
                    return True, None
                if not self.required:
                    return True, None
                value = 0
            elif self.read_only:
                print("Skipping input for read only field: {}".format(self.name))
                return True, None
            try:
                parsed = float(value)
            except Exception as e:
                raise TdiTableError("Error: {}".format(str(e)), self.table, -1)
            return True, parsed

        # Try to parse a bool regardless of how the user gave us the data.
        def _parse_bool(self, value):
            if value is None:
                if self.read_only:
                    return True, None
                if not self.required:
                    return True, None
                value = 0
            elif self.read_only:
                print("Skipping input for read only field: {}".format(self.name))
                return True, None
            try:
                parsed = bool(value)
            except Exception as e:
                raise TdiTableError("Error: {}".format(str(e)), self.table, -1)
            return True, parsed

        # Try to parse a list of integers. List type required.
        def _parse_int_arr(self, value):
            if value is None:
                if self.name.decode('ascii') == "$ACTION_MEMBER_STATUS":
                    from tdicli import TesterInt
                    return True, TesterInt(True, self.size)
                if self.read_only:
                    return True, None
                if not self.required:
                    return True, None
                value = []
            elif self.read_only:
                print("Skipping input for read only field: {}".format(self.name))
                return True, None
            try:
                is_iterable = iter(value)
            except:
                raise TdiTableError("Error: INT/BOOL array info must be an iterable type (e.g. list).", self.table, -1)
            parsed = []
            for s in value:
                res, v = self._parse_int(s, skip_size_check=True)
                if not res:
                    return False, -1
                parsed += [v]
            return True, parsed

        # Try to parse a string regardless of how the user gave us the data.
        def _parse_string(self, value):
            if value is None:
                if self.read_only:
                    return True, None
                if not self.required:
                    return True, None
                value = 0
            elif self.read_only:
                print("Skipping input for read only field: {}".format(self.name))
                return True, None
            try:
                parsed = str(value)
            except Exception as e:
                print("Error: {}".format(str(e)))
                return False, -1
            return True, parsed

        # Try to parse a list of strings. List type required.
        def _parse_str_arr(self, value):
            if value is None:
                if self.read_only:
                    return True, None
                if not self.required:
                    return True, None
                value = []
            elif self.read_only:
                print("Skipping input for read only field: {}".format(self.name))
                return True, None
            try:
                is_iterable = iter(value)
            except:
                raise TdiTableError("Error: STR array info must be an iterable type (e.g. list).", self.table, -1)
            parsed = []
            for s in value:
                res, v = self._parse_string(s)
                if not res:
                    return False, -1
                parsed += [v]
            return True, parsed

        def _parse_cont(self, method_name, value):
            # Recursively parse it as an input again
            ret_dict = {}
            if isinstance(value, dict):
                for name, v in value.items():
                    if isinstance(name, str):
                        name = name.encode('ascii')
                    ret_dict[name] = self._cont_data_fields[name].parse_input(method_name, v)[1]
            else:
                return False, ret_dict
            return True, ret_dict


        def _parse_cont_list(self, method_name, value):
            if (value == None):
                return True, ""
            try:
                is_iterable = iter(value)
            except:
                raise TdiTableError("Error: Container array info must be an iterable type (e.g. list).", self.table, -1)
            parsed = []
            for s in value:
                # For every item in the list, find the corresponding
                res, v = self._parse_cont(method_name, s)
                if not res:
                    return False, -1
                parsed += [v]
            return True, parsed

        # Parse input as if it is the type encoded in this object.
        def parse_input(self, method_name, value):
            arg_name = self.name
            arg_size = self.size
            arg_key_type = self.table.key_match_type_cls.key_match_type_str(self.type)
            arg_data_type = self.table.data_type_cls.data_type_str(self.data_type)
            #TODO cleanup fields that should not have a default to be mandatory
            #TODO cleanup fields that have default value to not be mandatory
            #TODO check for mandatory key/data field in the future
            # if (self.required and value == None):
            #     if self.category == "key":
            #         raise TdiTableError("Error: Table {}'s method {} has Key {} is mandatory field of type {} with size {} that cannot be None.".format(
            #             self.table.name, method_name, arg_name, arg_data_type, arg_size), self.table, -1)
            if ((self.category == "key" and arg_key_type == "EXACT" and arg_data_type in ["UINT64", "BYTE_STREAM"])
                or (self.category == "data" and arg_data_type in ["UINT64", "BYTE_STREAM", "BOOL", "INT64"])):
                return self._parse_int(value)
            elif self.category == "key" and arg_key_type == "EXACT" and arg_data_type == "STRING":
                return self._parse_string(value)
            elif self.category == "key" and arg_key_type in ["TERNARY"]:
                s0, p0 = True, 0
                s1, p1 = True, 0
                if value[0] != None or value[1] != None:
                    s0, p0 = self._parse_int(value[0])
                    s1, p1 = self._parse_int(value[1], mask_type=True)
                return (s0 and s1), [p0, p1]
            elif self.category == "key" and arg_key_type in ["RANGE", "LPM"]:
                s0, p0 = self._parse_int(value[0])
                s1, p1 = self._parse_int(value[1])
                return (s0 and s1), [p0, p1]
            elif self.category == "key" and arg_key_type in ["OPTIONAL"]:
                if value[0] != None and value[1] == None:
                    s0, p0 = self._parse_int(value[0])
                    s1, p1 = True, True
                else:
                    s0, p0 = self._parse_int(value[0])
                    s1, p1 = self._parse_bool(value[1])
                return (s0 and s1), [p0, p1]
            elif self.category == "data" and arg_data_type == "FLOAT":
                return self._parse_float(value)
            elif self.category == "data" and arg_data_type == "BOOL":
                return self._parse_bool(value)
            elif self.category == "data" and arg_data_type == "STRING":
                return self._parse_string(value)
            elif self.category == "data" and arg_data_type == "STR_ARR":
                return self._parse_str_arr(value)
            elif self.category == "data" and arg_data_type in ["INT_ARR", "BOOL_ARR"]:
                return self._parse_int_arr(value)
            elif self.category == "data" and arg_data_type == "CONTAINER":
                return self._parse_cont_list(method_name, value)
            else:
                if self.category == "data":
                    raise TdiTableError("Error: type {} for data field {} not yet supported in the CLI.".format(arg_data_type, self.name), self.table, -1)
                elif self.category == "key":
                    raise TdiTableError("Error: type {} for key field {} not yet supported in the CLI.".format(arg_key_type, self.name), self.table, -1)
                else:
                    raise TdiTableError("Error: unknown field type - category={}".format(self.category), self.table, -1)

        def _deparse_int(self, value):
            try:
                return int(value)
            except:
                return value

        def _deparse_float(self, value):
            try:
                return float(value)
            except:
                return value

        def _deparse_bool(self, value):
            try:
                return bool(value)
            except:
                return value

        def _deparse_string(self, value):
            return str(value)

        def _deparse_int_arr(self, value):
            return [value[i] for i in range(len(value))]

        def deparse_output(self, value):
            if self.category == "data" and (self.table.data_type_cls.data_type_str(self.data_type) in ["INT_ARR", "BOOL_ARR"]):
                return self._deparse_int_arr(value)
            elif ((self.category == "key" and self.table.key_match_type_cls.key_match_type_str(self.type) == "EXACT") or (self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) in ["UINT64", "BYTE_STREAM", "INT64"])):
                return self._deparse_int(value)
            elif self.category == "key" and self.table.key_match_type_cls.key_match_type_str(self.type) in ["TERNARY", "RANGE", "LPM"]:
                p0 = self._deparse_int(value[0])
                p1 = self._deparse_int(value[1])
                return p0, p1
            elif self.category == "key" and self.table.key_match_type_cls.key_match_type_str(self.type) in ["OPTIONAL"]:
                p0 = self._deparse_int(value[0])
                p1 = self._deparse_bool(value[1])
                return p0, p1
            elif self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) == "FLOAT":
                return self._deparse_float(value)
            elif self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) == "BOOL":
                return self._deparse_bool(value)
            elif self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) == "STRING":
                return self._deparse_string(value)
            else: # don't raise errors in case user is working with multiple entries
                if self.category == "data":
                    print("Error: type {} for data field {} not yet supported in the CLI.".format(self.table.data_type_cls.data_type_str(self.data_type), self.name))
                elif self.category == "key":
                    print("Error: type {} for key field {} not yet supported in the CLI.".format(self.table.key_match_type_cls.key_match_type_str(self.type), self.name))
                else:
                    print("Error: unknown field type - category={}".format(self.category))
                return "Error"

        def _stringify_int(self, value):
            if self.ipv4addr:
                return '{}'.format(ip(value))
            if value < 0:
                return '{}'.format(value)
            format_str = '0x{{:0{}X}}'.format(self.size // 4)
            if self.name.decode('ascii') in ["$MATCH_PRIORITY", "$COUNTER_SPEC_BYTES", "$COUNTER_SPEC_PKTS"]:
                format_str = "{}"
            return format_str.format(value)

        def _stringify_float(self, value):
            return '{}'.format(value)

        def _stringify_bool(self, value):
            return '{}'.format(value)

        def _stringify_int_arr(self, value):
            return '{}'.format(value)

        def _stringify_str_arr(self, value):
            return '{}'.format(value)

        # Container value is a list of dictionaries
        def _stringify_cont(self, cont_val, cont_name, i_lvl=1):
            indent = "  " * i_lvl
            to_print = ""
            if type(cont_name) != str:
                cont_name = cont_name.decode('ascii')
            for el in cont_val:
                if len(cont_val) > 1:
                    # Add index line only for lists that have more than 1 entry.
                    # Use indent reduced by 1 level.
                    to_print += "\n  {}{}[{}]".format(indent, cont_name, cont_val.index(el))
                for f_name, f_val in sorted(el.items(),  key=lambda x: x[0]):
                    name = f_name.decode('ascii')
                    if type(f_val) == list:
                        to_print += "\n    {}{} : {}".format(indent, name.ljust(30), self._stringify_cont(f_val, name, i_lvl+1))
                    else:
                        # Ignore error "not found" in container context.
                        # Data fields are not mandatory if invalid.
                        if type(f_val) == str and self.table._cintf.err_str(6) == f_val:
                            continue
                        if f_name in self._cont_data_fields.keys():
                            to_print += "\n    {}{} : {}".format(indent, name.ljust(30), self._cont_data_fields[f_name].stringify_output(f_val))
                        else:
                            to_print += "\n    {}{} : {}".format(indent, name.ljust(30), f_val)
            return to_print

        def stringify_output(self, value):
            if isinstance(value, str):
                return value
 
            # target specific handling
            output = self.table._stringify_output_target_specific(self, value)
            if output is not None:
                return output

            if self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) in ["INT_ARR", "BOOL_ARR"]:
                return self._stringify_int_arr(value)
            elif ((self.category == "key" and self.table.key_match_type_cls.key_match_type_str(self.type) == "EXACT") or (self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) in ["UINT64", "BYTE_STREAM", "INT64"])):
                return self._stringify_int(value)
            elif self.category == "key" and self.table.key_match_type_cls.key_match_type_str(self.type) in ["TERNARY", "RANGE", "LPM"]:
                p0 = self._stringify_int(value[0])
                p1 = self._stringify_int(value[1])
                return p0, p1
            elif self.category == "key" and self.table.key_match_type_cls.key_match_type_str(self.type) in ["OPTIONAL"]:
                p0 = self._stringify_int(value[0])
                p1 = self._stringify_bool(value[1])
                return p0, p1
            elif self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) == "FLOAT":
                return self._stringify_float(value)
            elif self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) == "BOOL":
                return self._stringify_bool(value)
            elif self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) == "STRING":
                return str(value)
            elif self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) == "STR_ARR":
                return self._stringify_str_arr(value)
            elif self.category == "data" and self.table.data_type_cls.data_type_str(self.data_type) == "CONTAINER":
                return self._stringify_cont(value, self.name)
            else: # don't raise errors in case user is working with multiple entries
                if self.category == "data":
                    print("Error: type {} for data field {} not yet supported in the CLI.".format(self.table.data_type_cls.data_type_str(self.data_type), self.name))
                elif self.category == "key":
                    print("Error: type {} for key field {} not yet supported in the CLI.".format(self.table.key_match_type_cls.key_match_type_str(self.type), self.name))
                else:
                    print("Error: unknown field type - category={}".format(self.category))
                return "Error"

    @staticmethod
    def mod_inc_type_map(mod_inc_type):
        mod_inc_type_dic = {
            0: "MOD_INC_ADD",
            1: "MOD_INC_DELETE" }
        if mod_inc_type in mod_inc_type_dic.keys():
            return mod_inc_type_dic[mod_inc_type]

    @staticmethod
    def no_usage_tables():
        return ["COUNTER",
                "METER",
                "REGISTER",
                "LPF",
                "WRED",
                "PVS",
                "PORT_CFG",
                "PORT_STAT",
                "PORT_HDL_INFO",
                "PORT_FRONT_PANEL_IDX_INFO",
                "PORT_STR_INFO",
                "PRE_MGID",
                "PRE_NODE",
                "PRE_ECMP",
                "PRE_LAG",
                "PRE_PRUNE",
                "PRE_PORT",
                "DEV_CFG",
                "REG_PARAM",
                "TM_PIPE_CFG",
                "TM_PIPE_SCHED_CFG",
                "TM_MIRROR_DPG",
                "MIRROR_CFG",]

    @staticmethod
    def idle_table_mode(mode):
        if mode == 0:
            return "POLL_MODE"
        if mode == 1:
            return "NOTIFY_MODE"
        if mode == 2:
            return "INVALID_MODE"
        return "ERR: Table Mode %d not implemented" . format(mode)

    """
    A convenience method for transforming a python integer to a
    network-order byte array.
    """
    @staticmethod
    def fill_c_byte_arr(content, size):
        bytes_ = (size + 7) // 8 # floor division
        value_type = c_ubyte * bytes_
        value = value_type()
        for i in range(0, bytes_):
            value[i] = (content >> 8 * (bytes_ - (i + 1))) & 0xFF
        return value, bytes_

    @staticmethod
    def from_c_byte_arr(value, size):
        bytes_ = (size + 7) // 8 # floor division
        content = 0
        for i in range(0, bytes_):
            content += value[i] << (bytes_ - (i + 1)) * 8
        return content

    """
    - Find the number of keys fields in the table
    - Get the list of key field ids
    - Get info about each key field (name, type, size, is_ptr)
    """
    def _init_key(self):
        self.key_fields = {}
        num_ids = c_uint(-1)
        sts = self._cintf.get_driver().tdi_key_field_id_list_size_get(self._info_handle, byref(num_ids))
        if not sts == 0:
            print("CLI Error: get num key fields for {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
            return sts
        if num_ids.value == 0:
            return 0
        array_type = c_uint * num_ids.value
        field_ids = array_type()
        sts = self._cintf.get_driver().tdi_key_field_id_list_get(self._info_handle, field_ids)
        if not sts == 0:
            print("CLI Error: get key field ids for {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
            return sts
        for field_id in field_ids:
            field_name = c_char_p()
            sts = self._cintf.get_driver().tdi_key_field_name_get(self._info_handle, field_id, byref(field_name))
            if not sts == 0:
                print("CLI Error: get key field name for {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
                return sts
            field_type = c_int(-1)
            sts = self._cintf.get_driver().tdi_key_field_match_type_get(self._info_handle, field_id, byref(field_type))
            if not sts == 0:
                print("CLI Error: get key field type for {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
                return sts
            field_data_type = c_int(-1)
            sts = self._cintf.get_driver().tdi_key_field_data_type_get(self._info_handle, field_id, byref(field_data_type))
            if not sts == 0:
                print("CLI Error: get key field data type for {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
                return sts
            field_size = c_uint()
            sts = self._cintf.get_driver().tdi_key_field_size_get(self._info_handle, field_id, byref(field_size))
            if not sts == 0:
                print("CLI Error: get key field size for {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
                return sts
            field_is_ptr = c_bool()
            sts = self._cintf.get_driver().tdi_key_field_is_ptr_get(self._info_handle, field_id, byref(field_is_ptr))
            if not sts == 0:
                print("CLI Error: get key field is_ptr for {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
                return sts
            readable = "{!s:30} type={!s:10} size={:^2}".format(field_name.value.decode('ascii'), self.key_match_type_cls.key_match_type_str(field_type.value), field_size.value)
            self.key_field_readables.append(readable.strip())
            self.key_fields[field_name.value] = self.TdiTableField(field_name.value, field_id, field_size.value, field_is_ptr.value, False, True, "key", self, data_type_=field_data_type.value, type_=field_type.value)
        return 0

    def _init_data_fields(self, input_dict, data_field_readables, field_id, action_id=None, action_name=None, depth=0):
        if not action_id:
            action_id = c_uint(0)
        field_type = c_int(-1)
        sts = self._cintf.get_driver().tdi_data_field_type_with_action_get(self._info_handle, field_id, action_id, byref(field_type))
        if not sts == 0:
            print("CLI Error: get data field type for action {} failed. [{}]".format(name.decode('ascii'), self._cintf.err_str(sts)))
            return sts
        field_name = c_char_p()
        sts = self._cintf.get_driver().tdi_data_field_name_with_action_get(self._info_handle, field_id, action_id, byref(field_name))
        if not sts == 0:
            print("CLI Error: get data field name for action {} failed. [{}]".format(name.decode('ascii'), self._cintf.err_str(sts)))
            return sts
        field_size = c_uint()
        sts = self._cintf.get_driver().tdi_data_field_size_with_action_get(self._info_handle, field_id, action_id, byref(field_size))
        if not sts == 0:
            print("CLI Error: get data field size for action {} failed. [{}]".format(name.decode('ascii'), self._cintf.err_str(sts)))
            return sts
        field_is_ptr = c_bool()
        sts = self._cintf.get_driver().tdi_data_field_is_ptr_with_action_get(self._info_handle, field_id, action_id, byref(field_is_ptr))
        if not sts == 0:
            print("CLI Error: get data field is_ptr for action {} failed. [{}]".format(name.decode('ascii'), self._cintf.err_str(sts)))
            return sts
        read_only = c_bool()
        sts = self._cintf.get_driver().tdi_data_field_is_read_only_with_action_get(self._info_handle, field_id, action_id, byref(read_only))
        if not sts == 0:
            print("CLI Error: get data field is_read_only for action {} failed. [{}]".format(name.decode('ascii'), self._cintf.err_str(sts)))
            return sts
        mandatory = c_bool()
        sts = self._cintf.get_driver().tdi_data_field_is_mandatory_with_action_get(self._info_handle, field_id, action_id, byref(mandatory))
        if not sts == 0:
            print("CLI Error: get data field is_mandatory for action {} failed. [{}]".format(name.decode('ascii'), self._cintf.err_str(sts)))
            return sts
        readable = "{!s:30} type={!s:10} size={:^2}".format(field_name.value.decode('ascii'), self.data_type_cls.data_type_str(field_type.value), field_size.value)
        data_field_readables.append("\t"*depth + readable.strip())
        input_dict[field_name.value] = self.TdiTableField(field_name.value, field_id, field_size.value, field_is_ptr.value, read_only.value, mandatory.value, "data", self, action_name=action_name, action_id=action_id, data_type_=field_type.value)
        # If field is a container field, then we need to get list of container fields and process them
        # recursively
        if self.data_type_cls.data_type_str(field_type.value) == "CONTAINER":
            # Get container field ids.
            arr_t = c_uint * field_size.value
            c_fields = arr_t()
            sts = self._cintf.get_driver().tdi_container_data_field_list_get(self._info_handle, field_id, c_fields)
            if not sts == 0:
                print("CLI Error: get container data field list failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
                return sts
            for cont_f_id in c_fields:
                self._init_data_fields(input_dict[field_name.value]._cont_data_fields, data_field_readables, cont_f_id, depth=depth+1)



    """
    - Find the number of data fields in the table
    - Get the list of data field ids
    - Get info about each data field (name, type, size, is_ptr)
    - If the table has actions instead of data fields, do this for
      each of the actions.
    """
    def _init_data(self):
        self.data_fields = {}

        if len(self.actions) == 0:
            num_ids = c_uint(-1)
            sts = self._cintf.get_driver().tdi_data_field_id_list_size_get(self._info_handle, byref(num_ids))
            if not sts == 0:
                print("CLI Error: get num data fields for table {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
                return sts
            if num_ids.value == 0:
                return 0
            array_type = c_uint * num_ids.value
            field_ids = array_type()
            sts = self._cintf.get_driver().tdi_data_field_list_get(self._info_handle, field_ids)
            if not sts == 0:
                print("CLI Error: get data field ids for table {} failed. [{}]".format(self.name, self._cintf.err_str(sts)))
                return sts
            for field_id in field_ids:
                self._init_data_fields(self.data_fields, self.data_field_readables, field_id)
        else:
            for name, info in self.actions.items():
                action_readable = ""
                if ("@defaultonly","") in info["annotations"]:
                    action_readable = "      {} (DefaultOnly)".format(name.decode('ascii'))
                else:
                    action_readable = "      {}".format(name.decode('ascii'))
                self.action_readables[name] = action_readable.strip()
                num_ids = c_uint(-1)
                sts = self._cintf.get_driver().tdi_data_field_id_list_size_with_action_get(self._info_handle, info["id"], byref(num_ids))
                if not sts == 0:
                    print("CLI Error: get num data fields for action {} failed. [{}]".format(name.decode('ascii'), self._cintf.err_str(sts)))
                    return sts
                if num_ids.value == 0:
                    continue
                array_type = c_uint * num_ids.value
                field_ids = array_type()
                sts = self._cintf.get_driver().tdi_data_field_list_with_action_get(self._info_handle, info["id"], field_ids)
                if not sts == 0:
                    print("CLI Error: get data field ids for action {} failed. [{}]".format(name.decode('ascii'), self._cintf.err_str(sts)))
                    return sts
                data_fields = info["data_fields"]
                data_field_readables = self.action_data_readables[name]
                for field_id in field_ids:
                    self._init_data_fields(data_fields, data_field_readables, field_id, info["id"], name)

        return 0

    """
    - Find the number of actions fields in the table
    - Get the list of action ids
    - Get each action's name
    """
    def _init_actions(self):
        self.actions = {}
        self.action_id_name_map = {}
        '''
        is_action_applicable = c_bool()
        self._cintf.get_driver().tdi_action_id_applicable(self._handle, byref(is_action_applicable))
        if (not is_action_applicable.value):
            return 0
        '''
        num_ids = c_uint(-1)
        
        sts = self._cintf.get_driver().tdi_action_id_list_size_get(self._info_handle, byref(num_ids))
        if not sts == 0 or num_ids.value == 0:
            return 0
        array_type = c_uint * num_ids.value
        action_ids = array_type()
        sts = self._cintf.get_driver().tdi_action_id_list_get(self._info_handle, action_ids)
        if not sts == 0:
            print("CLI Error: get action id list failed for {}.".format(self.name))
            return sts
        
        logging.debug("self.name="+str(self.name))
        '''
        if self.name == "pipe.ingress.mymac":
            action_ids = [21257015]
            num_ids = c_uint(1)
        elif self.name == "pipe.ingress.l2_fwd":
            action_ids = [18638031,33281717,21257015]
            num_ids = c_uint(3)
        elif self.name == "pipe.ingress.ipv4_host":
            action_ids = [18638031,33281717,21257015]
            num_ids = c_uint(3)
        elif self.name == "pipe.ingress.ipv4_lpm":
            action_ids = [30582131,18638031,33281717]
            num_ids = c_uint(3)
        else: return 0
        '''
        for action_id in action_ids:
            action_name = c_char_p()
            sts = self._cintf.get_driver().tdi_action_name_get(self._info_handle, action_id, byref(action_name))
            if not sts == 0:
                print("CLI Error: get action name failed for {}.".format(self.name))
                return sts
            # Get action annotations
            nannotations_func = self._cintf.get_driver().tdi_action_num_annotations_get
            get_annotations_func = self._cintf.get_driver().tdi_action_annotations_get

            annotations = []
            num_annotations = c_uint(0)
            sts = nannotations_func(self._info_handle, action_id, byref(num_annotations))

            arr_type = self._cintf.annotation_type * num_annotations.value
            annotations_arr = arr_type()
            sts = get_annotations_func(self._info_handle, action_id, byref(annotations_arr))
            for ann in annotations_arr:
                annotations += [(ann.name.decode('ascii'), ann.value.decode('ascii'))]
            
            # Create and insert action info
            self.actions[action_name.value] = {"id" : action_id,
                                               "data_fields" : {},
                                               "annotations" : annotations}
           
            self.action_id_name_map[action_id] = action_name.value
            self.action_data_readables[action_name.value] = []
        return 0

    def _init_fields(self):
        return 0

    """
    Fill a key allocated by TDI Runtime with appropriate values provided
    by the user. These values must have already been parsed by the
    TdiTableField class.
    """
    def _set_key_fields(self, content, key_handle):
        for name, info in self.key_fields.items():
            sts = -1
            if name not in content.keys():
                continue
            if self.key_match_type_cls.key_match_type_str(info.type) == "EXACT":
                if self.data_type_cls.data_type_str(info.data_type) == "STRING":
                    value = c_char_p(content[name].encode('ascii'))
                    sts = self._cintf.get_driver().tdi_key_field_set_value_string(key_handle, info.id, value)
                elif not info.is_ptr:
                    value = c_ulonglong(content[name])
                    sts = self._cintf.get_driver().tdi_key_field_set_value(key_handle, info.id, value)
                else:
                    value, bytes_ = self.fill_c_byte_arr(content[name], info.size)
                    sts = self._cintf.get_driver().tdi_key_field_set_value_ptr(key_handle, info.id, value, bytes_)
            elif self.key_match_type_cls.key_match_type_str(info.type) == "TERNARY":
                if not info.is_ptr:
                    value = c_ulonglong(content[name][0])
                    mask = c_ulonglong(content[name][1])
                    sts = self._cintf.get_driver().tdi_key_field_set_value_and_mask(key_handle, info.id, value, mask)
                else:
                    v0, b0 = self.fill_c_byte_arr(content[name][0], info.size)
                    v1, b1 = self.fill_c_byte_arr(content[name][1], info.size)
                    sts = self._cintf.get_driver().tdi_key_field_set_value_and_mask_ptr(key_handle, info.id, v0, v1, b0)
            elif self.key_match_type_cls.key_match_type_str(info.type) == "RANGE":
                if not info.is_ptr:
                    start = c_ulonglong(content[name][0])
                    end = c_ulonglong(content[name][1])
                    sts = self._cintf.get_driver().tdi_key_field_set_value_range(key_handle, info.id, start, end)
                else:
                    s, b0 = self.fill_c_byte_arr(content[name][0], info.size)
                    e, b1 = self.fill_c_byte_arr(content[name][1], info.size)
                    sts = self._cintf.get_driver().tdi_key_field_set_value_range_ptr(key_handle, info.id, s, e, b0)
            elif self.key_match_type_cls.key_match_type_str(info.type) == "LPM":
                if not info.is_ptr:
                    value = c_ulonglong(content[name][0])
                    p_len = c_ushort(content[name][1])
                    sts = self._cintf.get_driver().tdi_key_field_set_value_lpm(key_handle, info.id, value, p_len)
                else:
                    v, b = self.fill_c_byte_arr(content[name][0], info.size)
                    p_len = c_ushort(content[name][1])
                    sts = self._cintf.get_driver().tdi_key_field_set_value_lpm_ptr(key_handle, info.id, v, p_len, b)
            elif self.key_match_type_cls.key_match_type_str(info.type) == "OPTIONAL":
                if not info.is_ptr:
                    value = c_ulonglong(content[name][0])
                    is_valid = c_bool(content[name][1])
                    sts = self._cintf.get_driver().tdi_key_field_set_value_optional(key_handle, info.id, value, is_valid)
                else:
                    v, b = self.fill_c_byte_arr(content[name][0], info.size)
                    is_valid = c_bool(content[name][1])
                    sts = self._cintf.get_driver().tdi_key_field_set_value_optional_ptr(key_handle, info.id, v, is_valid, b)
            if not sts == 0:
                raise TdiTableError("CLI Error: set key field failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
        return 0

    def _get_key_fields(self, key_handle):
        content = {}
        for name, info in self.key_fields.items():
            sts = -1
            if self.key_match_type_cls.key_match_type_str(info.type) == "EXACT":
                if self.data_type_cls.data_type_str(info.data_type) == "STRING":
                    size = c_uint(0)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_string_size(key_handle, info.id, byref(size))
                    if sts == 0:
                        value = create_string_buffer(size.value)
                        sts = self._cintf.get_driver().tdi_key_field_get_value_string(key_handle, info.id, value)
                        content[name] = value.value.decode('ascii')
                elif not info.is_ptr:
                    value = c_ulonglong(0)
                    sts = self._cintf.get_driver().tdi_key_field_get_value(key_handle, info.id, byref(value))
                    content[name] = value.value
                else:
                    value, bytes_ = self.fill_c_byte_arr(0, info.size)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_ptr(key_handle, info.id, bytes_, value)
                    content[name] = self.from_c_byte_arr(value, info.size)
            elif self.key_match_type_cls.key_match_type_str(info.type) == "TERNARY":
                if not info.is_ptr:
                    value = c_ulonglong(0)
                    mask = c_ulonglong(0)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_and_mask(key_handle, info.id, byref(value), byref(mask))
                    content[name] = (value.value, mask.value)
                else:
                    v0, b0 = self.fill_c_byte_arr(0, info.size)
                    v1, b1 = self.fill_c_byte_arr(0, info.size)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_and_mask_ptr(key_handle, info.id, b0, v0, v1)
                    content[name] = (self.from_c_byte_arr(v0, info.size), self.from_c_byte_arr(v1, info.size))
            elif self.key_match_type_cls.key_match_type_str(info.type) == "RANGE":
                if not info.is_ptr:
                    start = c_ulonglong(0)
                    end = c_ulonglong(0)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_range(key_handle, info.id, byref(start), byref(end))
                    content[name] = (start.value, end.value)
                else:
                    s, b0 = self.fill_c_byte_arr(0, info.size)
                    e, b1 = self.fill_c_byte_arr(0, info.size)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_range_ptr(key_handle, info.id, b0, s, e)
                    content[name] = (self.from_c_byte_arr(s, info.size), self.from_c_byte_arr(e, info.size))
            elif self.key_match_type_cls.key_match_type_str(info.type) == "LPM":
                if not info.is_ptr:
                    value = c_ulonglong(0)
                    p_len = c_ushort(0)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_lpm(key_handle, info.id, byref(value), byref(p_len))
                    content[name] = (value.value, p_len.value)
                else:
                    v, b = self.fill_c_byte_arr(0, info.size)
                    p_len = c_ushort(0)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_lpm_ptr(key_handle, info.id, b, v, byref(p_len))
                    content[name] = (self.from_c_byte_arr(v, info.size), p_len.value)
            elif self.key_match_type_cls.key_match_type_str(info.type) == "OPTIONAL":
                if not info.is_ptr:
                    value = c_ulonglong(0)
                    is_valid = c_bool(False)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_optional(key_handle, info.id, byref(value), byref(is_valid))
                    content[name] = (value.value, is_valid.value)
                else:
                    v, b = self.fill_c_byte_arr(0, info.size)
                    is_valid = c_bool(False)
                    sts = self._cintf.get_driver().tdi_key_field_get_value_optional_ptr(key_handle, info.id, b, v, byref(is_valid))
                    content[name] = (self.from_c_byte_arr(v, info.size), is_valid.value)
            if not sts == 0:
                #print("CLI Error: get key field failed. [{}].".format(self._cintf.err_str(sts)))
                content[name] = self._cintf.err_str(sts)
        return content

    def _deallocate_data_array(self, data_handles, array_len):
        for i in range(array_len):
            self._cintf.get_driver().tdi_table_data_deallocate(data_handles[i])


    def _set_data_field(self, content, data_handle, data_fields):
        for name, info in data_fields.items():
            if (isinstance(name, str)):
                name = name.encode('UTF-8')
            if name not in content.keys():
                continue
            if content[name] is None:
                continue
            sts = -1
            #print("data_type="+self.data_type_cls.data_type_str(info.data_type))
            if self.data_type_cls.data_type_str(info.data_type) == "BYTE_STREAM":
                value, bytes_ = self.fill_c_byte_arr(content[name], info.size)
                sts = self._cintf.get_driver().tdi_data_field_set_value_ptr(data_handle, info.id, value, bytes_)
            if self.data_type_cls.data_type_str(info.data_type) == "UINT64":
                if not info.is_ptr:
                    value = c_ulonglong(content[name])
                    sts = self._cintf.get_driver().tdi_data_field_set_value(data_handle, info.id, value)
                else:
                    value, bytes_ = self.fill_c_byte_arr(content[name], info.size)
                    sts = self._cintf.get_driver().tdi_data_field_set_value_ptr(data_handle, info.id, value, bytes_)
            if self.data_type_cls.data_type_str(info.data_type) == "INT64":
                value = c_int64(content[name])
                sts = self._cintf.get_driver().tdi_data_field_set_value_int64(data_handle, info.id, value)
            if self.data_type_cls.data_type_str(info.data_type) == "INT_ARR":
                arrlen = len(content[name])
                arrtype = c_uint * arrlen
                value = arrtype()
                for idx, v in enumerate(content[name]):
                    value[idx] = v
                sts = self._cintf.get_driver().tdi_data_field_set_value_array(data_handle, info.id, value, arrlen)
            if self.data_type_cls.data_type_str(info.data_type) == "BOOL_ARR":
                arrlen = len(content[name])
                arrtype = c_bool * arrlen
                value = arrtype()
                for idx, v in enumerate(content[name]):
                    value[idx] = v
                sts = self._cintf.get_driver().tdi_data_field_set_value_bool_array(data_handle, info.id, value, arrlen)
            if self.data_type_cls.data_type_str(info.data_type) == "STR_ARR":
                value_str = ' '.join([str(i) for i in content[name]])
                value = c_char_p(value_str.encode('ascii'))
                sts = self._cintf.get_driver().tdi_data_field_set_value_str_array(data_handle, info.id, value)
            if self.data_type_cls.data_type_str(info.data_type) == "FLOAT":
                value = c_float(content[name])
                sts = self._cintf.get_driver().tdi_data_field_set_float(data_handle, info.id, value)
            if self.data_type_cls.data_type_str(info.data_type) == "BOOL":
                value = c_bool(content[name])
                sts = self._cintf.get_driver().tdi_data_field_set_bool(data_handle, info.id, value)
            if self.data_type_cls.data_type_str(info.data_type) == "STRING":
                value = c_char_p(content[name].encode('ascii'))
                sts = self._cintf.get_driver().tdi_data_field_set_string(data_handle, info.id, value)
            """
            if self.data_type_cls.data_type_str(info.data_type) == "CONTAINER":
                cont_list_len = len(content[name])
                if (cont_list_len == 0):
                    sts = 0
                    continue;
                # Allocate a list of inner data objects
                arr_inner = self._cintf.handle_type * cont_list_len
                inner_data_handles = arr_inner()
                for i in range(cont_list_len):
                    # allocate ith inner data object and set it recursively
                    # Any error in the loop might require deallocating previous iterations' objects
                    # Top data object deallocation is taken care of outside
                    sts =  self._cintf.get_driver().tdi_table_data_allocate_container(self._handle, info.id, byref(inner_data_handles[i]))
                    if not sts == 0:
                        print("CLI Error: table data allocate failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
                        self._deallocate_data_array(inner_data_handles, i)
                        return sts
                    sts = self._set_data_field(content[name][i], inner_data_handles[i], info._cont_data_fields)

                    if not sts == 0:
                        print("CLI Error: table data field set failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
                        self._deallocate_data_array(inner_data_handles, i+1)
                        return sts

                # Set the list of inner data object in top level container object
                sts = self._cintf.get_driver().tdi_data_field_set_value_data_field_array(data_handle, info.id, inner_data_handles, cont_list_len)
            """
            if not sts == 0:
                raise TdiTableError("CLI Error: set data field failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
        return 0

    """
    Fill data object allocated by TDI Runtime with appropriate values
    provided by the user. These values must have already been parsed by the
    TdiTableField class.
    """
    def _set_data_fields(self, content, data_handle, action):
        data_fields = self.data_fields
        
        if action != None:
            data_fields = self.actions[action]["data_fields"]
       
        return self._set_data_field(content, data_handle, data_fields)

    def _get_cont_data_fields(self, info, data_handle):
        content = []
        field_id = info.id
        # Get container data handles.
        f_size = c_uint()
        sts = self._cintf.get_driver().tdi_data_field_get_value_data_field_array_size(data_handle, field_id, byref(f_size))
        if not sts == 0:
            raise TdiTableError("CLI Error: get data field array size failed. [{}].".format(self._cintf.err_str(sts)), self, sts)

        arr_df = self._cintf.handle_type * f_size.value
        data_handles = arr_df()
        sts = self._cintf.get_driver().tdi_data_field_get_value_data_field_array(data_handle, field_id, byref(data_handles))
        if not sts == 0:
            raise TdiTableError("CLI Error: get data field array failed. [{}].".format(self._cintf.err_str(sts)), self, sts)

        for data_h in data_handles:
            content.append(self._process_data_fields(info._cont_data_fields, data_h, True))
        return content


    '''
        Certain fields might need target specific handling.
        Targets can override this method to handle such cases.
        Targets should return True/None if a data field is handled in that code so that they can be skipped here.
    '''
    def _stringify_output_target_specific(self, data_field, value):
        return None

    def _process_target_specific_data_field(self, data_field, data_handle, content):
        return False

    def _process_data_fields(self, data_fields, data_handle, force=False):
        content = {}
        for name, info in data_fields.items():
            sts = -1
            is_active = c_bool(False)
            self._cintf.get_driver().tdi_data_field_is_active(data_handle, info.id, byref(is_active))

            if not is_active and not force:
                continue

            # target specific handling
            processed = self._process_target_specific_data_field(info, data_handle, content)
            if processed is True:
                continue

            if self.data_type_cls.data_type_str(info.data_type) == "BYTE_STREAM":
                value, bytes_ = self.fill_c_byte_arr(0, info.size)
                sts = self._cintf.get_driver().tdi_data_field_get_value_ptr(data_handle, info.id, bytes_, value)
                content[name] = self.from_c_byte_arr(value, info.size)
            if self.data_type_cls.data_type_str(info.data_type) == "UINT64":
                if not info.is_ptr:
                    value = c_ulonglong(0)
                    sts = self._cintf.get_driver().tdi_data_field_get_value(data_handle, info.id, byref(value))
                    content[name] = value.value
                else:
                    value, bytes_ = self.fill_c_byte_arr(0, info.size)
                    sts = self._cintf.get_driver().tdi_data_field_get_value_ptr(data_handle, info.id, bytes_, value)
                    content[name] = self.from_c_byte_arr(value, info.size)
            if self.data_type_cls.data_type_str(info.data_type) == "INT64":
                if not info.is_ptr:
                    value = c_int64(0)
                    sts = self._cintf.get_driver().tdi_data_field_get_value_int64(data_handle, info.id, byref(value))
                    content[name] = value.value
            if self.data_type_cls.data_type_str(info.data_type) == "FLOAT":
                if not info.is_ptr:
                    value = c_float(0)
                    sts = self._cintf.get_driver().tdi_data_field_get_float(data_handle, info.id, byref(value))
                    content[name] = value.value
            if self.data_type_cls.data_type_str(info.data_type) == "BOOL_ARR":
                size = c_uint(0)
                sts = self._cintf.get_driver().tdi_data_field_get_value_bool_array_size(data_handle, info.id, byref(size))
                if sts == 0:
                    arrtype = c_bool * size.value
                    value = arrtype()
                    sts = self._cintf.get_driver().tdi_data_field_get_value_bool_array(data_handle, info.id, value)
                    content[name] = []
                    for i in range(0, size.value):
                        content[name].append(value[i])
            if self.data_type_cls.data_type_str(info.data_type) == "INT_ARR":
                size = c_uint(0)
                sts = self._cintf.get_driver().tdi_data_field_get_value_array_size(data_handle, info.id, byref(size))
                if sts == 0:
                    arrtype = c_uint * size.value
                    value = arrtype()
                    sts = self._cintf.get_driver().tdi_data_field_get_value_array(data_handle, info.id, value)
                    content[name] = []
                    for i in range(0, size.value):
                        content[name].append(value[i])
            if self.data_type_cls.data_type_str(info.data_type) == "STR_ARR":
                size = c_uint(0)
                # Size returns string length
                sts = self._cintf.get_driver().tdi_data_field_get_value_str_array_size(data_handle, info.id, byref(size))
                if sts == 0:
                    value = create_string_buffer(size.value)
                    sts = self._cintf.get_driver().tdi_data_field_get_value_str_array(data_handle, info.id, size.value, value)
                    content[name] = []
                    val_str = value.value.decode('ascii')
                    content[name] = val_str.split()

            if self.data_type_cls.data_type_str(info.data_type) == "BOOL":
                value = c_bool(0)
                sts = self._cintf.get_driver().tdi_data_field_get_bool(data_handle, info.id, byref(value))
                content[name] = value.value

            if self.data_type_cls.data_type_str(info.data_type) == "STRING":
                size = c_uint(0)
                sts = self._cintf.get_driver().tdi_data_field_get_string_size(data_handle, info.id, byref(size))
                if sts == 0:
                    value = create_string_buffer(size.value)
                    sts = self._cintf.get_driver().tdi_data_field_get_string(data_handle, info.id, value)
                    content[name] = value.value.decode('ascii')

            if self.data_type_cls.data_type_str(info.data_type) == "CONTAINER":
                content[name] = self._get_cont_data_fields(info, data_handle)
                sts = 0
            if not sts == 0:
                #print("CLI Error: get data field failed. [{}].".format(self._cintf.err_str(sts)))
                content[name] = self._cintf.err_str(sts)
        return content

    def _get_data_fields(self, data_handle, action):
        data_fields = self.data_fields

        if action != None:
            data_fields = self.actions[action]["data_fields"]
        if action == b'NoAction':
            data_fields = {}

        return self._process_data_fields(data_fields, data_handle)

    """
    These next two functions allocate a key or data object via TDI Runtime.
    These objects are then filled via the corresponding methods for filling
    the objects with parsed user-provided information.
    """
    def _make_call_keys(self, key_content):
        key_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_table_key_allocate(self._handle, byref(key_handle))
        if not sts == 0:
            raise TdiTableError("CLI Error: table key allocate failed. [{}].".format(self._cintf.err_str(sts)), self, sts)

        sts = self._set_key_fields(key_content, key_handle)
        if not sts == 0:
            print("CLI Error: table key field set failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
            self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
            return -1
        return key_handle

    def _make_call_data(self, data_content, action):
        data_handle = self._cintf.handle_type()
        if action != None:
            sts = self._cintf.get_driver().tdi_table_action_data_allocate(self._handle, self.actions[action]["id"], byref(data_handle))
        else:
            sts = self._cintf.get_driver().tdi_table_data_allocate(self._handle, byref(data_handle))
        if not sts == 0:
            print("CLI Error: table data allocate failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
            return -1
        sts = self._set_data_fields(data_content, data_handle, action)
        if not sts == 0:
            print("CLI Error: table data field set failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
            self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
            return -1
        return data_handle

    def _make_call_flags(self, flags_value):
        flags_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_flags_create(flags_value, byref(flags_handle))
        if not sts == 0:
            raise TdiTableError("CLI Error: flags create failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
        '''
        sts = self._set_flags_fields(flags_value, flags_handle)
        if not sts == 0:
            print("CLI Error: table key field set failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
            self._cintf.get_driver().tdi_flags_delete(flags_handle)
            return -1
        '''
        return flags_handle

    """
    A helper function for adding or modifying table entries, as these
    two operations are essentially the same - but an add must have been
    called on a key before said key can be modified.
    """
    def _call_add_mod(self, key_content, data_content, action, c_func, flags=0):
        key_handle = self._make_call_keys(key_content)
        data_handle = self._make_call_data(data_content, action)
        flags_handle = self._make_call_flags(flags)
        #if key_handle == -1 or data_handle == -1 or flags_handle == -1:
        if key_handle == -1 or data_handle == -1:
            # If one was -1 but the other was created, then it must be freed
            if key_handle != -1:
                self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
            if data_handle != -1:
                self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
            if flags_handle != -1:
                self._cintf.get_driver().tdi_flags_delete(flags_handle)
            return -1

        sts = c_func(self._handle, self._cintf.get_session(), self._cintf.get_dev_tgt(), self._cintf.get_flags(), key_handle, data_handle)
        self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
        self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
        self._cintf.get_driver().tdi_flags_delete(flags_handle)
        return sts

    """
    A helper function for adding or modifying table entries, as these
    two operations are essentially the same - but an add must have been
    called on a key before said key can be modified.
    """
    def _call_add_mod_inc(self, key_content, data_content, action, flag_type, c_func):
        key_handle = self._make_call_keys(key_content)
        data_handle = self._make_call_data(data_content, action)
        if key_handle == -1 or data_handle == -1:
            # If one was -1 but the other was created, then it must be freed
            if key_handle != -1:
                self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
            if data_handle != -1:
                self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
            return -1
        sts = c_func(self._handle, self._cintf.get_session(), self._cintf.get_dev_tgt(), key_handle, data_handle, flag_type)
        self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
        self._cintf.get_driver().tdi_table_data_deallocate(data_handle)

        return sts

    """
    These methods essentially are the highest level of abstraction on our
    TDI Runtime C API. All data passed to them must be eligible for use with
    Python's ctypes foriegn function interface.
    """
    def add_entry(self, key_content, data_content, action=None):
        sts = self._call_add_mod(key_content, data_content, action, self._cintf.tdi_table_entry_add)
        if not sts == 0:
            raise TdiTableError("Error: table_entry_add failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)

    def mod_entry(self, key_content, data_content, action=None, ttl_reset=True):
        flags = 0
        if ttl_reset == False:
            # SKIP_TTL_RESET is second bit, currently only reset supported.
            flags |= 1 << 2
        sts = self._call_add_mod(key_content, data_content, action, self._cintf.tdi_table_entry_mod, flags)
        if not sts == 0:
            raise TdiTableError("Error: table_entry_mod failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)

    '''
    def mod_inc_entry(self, key_content, data_content, flag_type=0, action=None):
        sts = self._call_add_mod_inc(key_content, data_content, action, flag_type, self._cintf.tdi_table_entry_mod_inc)
        if not sts == 0:
            raise TdiTableError("Error: table_entry_mod failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
    '''

    def set_default_entry(self, data_content, action=None):
        table_type = self.table_type_cls.table_type_str(self.get_type())
        if table_type == -1:
            return -1
        data_handle = self._make_call_data(data_content, action)
        if data_handle == -1:
            return -1
        sts = self._cintf.tdi_table_default_entry_set(self._handle,
                                                        self._cintf.get_session(),
                                                        self._cintf.get_dev_tgt(),
                                                        data_handle)
        self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
        if not sts == 0:
            raise TdiTableError("Error: set_default_entry failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)

    def reset_default_entry(self):
        table_type = self.table_type_cls.table_type_str(self.get_type())
        if table_type == -1:
            return -1
        sts = self._cintf.tdi_table_default_entry_reset(self._handle,
                                                          self._cintf.get_session(),
                                                          self._cintf.get_dev_tgt())
        if not sts == 0:
            raise TdiTableError("Error: reset_default_entry failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)

    def get_default_entry(self, print_entry=True, from_hw=False):
        table_type = self.table_type_cls.table_type_str(self.get_type())
        if table_type == -1:
            return -1
        data_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_table_data_allocate(self._handle, byref(data_handle))
        if not sts == 0:
            raise TdiTableError("Error: tdi_table_data_allocate failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        flag = 0
        flags_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_flags_create(flag,  byref(flags_handle))
        if not sts == 0:
            return -1
        if from_hw:
            flag = 1
        sts = self._cintf.get_driver().tdi_flags_set_value(flags_handle, self.flags_type_cls.flag_map(flag_enum_str="from_hw"), flag);
        sts = self._cintf.tdi_table_default_entry_get(self._handle,
                                                        self._cintf.get_session(),
                                                        self._cintf.get_dev_tgt(),
                                                        data_handle,
                                                        flags_handle)
        if not sts == 0:
            self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
            self._cintf.get_driver().tdi_flags_delete(flags_handle)
            raise TdiTableError("Error: get_default_entry failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        action = None

        if len(self.actions) > 0:
            action = self.action_id_name_map[self._action_from_data(data_handle)]

        entry = TableEntry(self, {}, self._get_data_fields(data_handle, action), action)
        entry.push = entry.default_entry_block
        entry.update = entry.default_entry_block
        entry.remove = entry.default_entry_block
        if print_entry:
            stream_printer = self.make_entry_stream_printer()
            action = []

            if len(self.actions) > 0:
                action = [self._action_from_data(data_handle)]

            stream_printer(None, [data_handle], action)
        self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
        self._cintf.get_driver().tdi_flags_delete(flags_handle)
        return entry

    def del_entry(self, key_content, entry_handle=None):
        if entry_handle != None and "get_by_handle" not in self.supported_commands:
            raise TdiTableError("{} Error: deleting entry by handle not supported.".format(self.name), self, -1)

        key_handle = None
        #entry_tgt = byref(self._cintf.BfDevTgt(0, 0, 0, 0))
        entry_tgt = self._cintf.get_dev_tgt()
        if entry_handle != None:
            key_handle = self._cintf.handle_type()
            sts = self._cintf.get_driver().tdi_table_key_allocate(self._handle, byref(key_handle))
            if sts != 0:
                raise TdiTableError("CLI Error: table key allocate failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
            sts = self._cintf.tdi_table_entry_key_get(self._handle,
                                                  self._cintf.get_session(),
                                                  self._cintf.get_dev_tgt(),
                                                  entry_handle,
                                                  entry_tgt,
                                                  key_handle)
            if sts != 0:
                raise TdiTableError("Error: table_entry_delete failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        else:
            key_handle = self._make_call_keys(key_content)
            if key_handle == -1:
                return -1
            entry_tgt = self._cintf.get_dev_tgt()
        sts = self._cintf.tdi_table_entry_del(self._handle,
                                                self._cintf.get_session(),
                                                entry_tgt,
                                                key_handle)
        self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
        if sts != 0:
            raise TdiTableError("Error: table_entry_delete failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)

    def get_entry(self, key_content, from_hw=False, print_entry=True, key_handle=None, entry_handle=None, dev_tgt=None):
        is_key_set = False if key_handle==None else True
        if dev_tgt == None:
            dev_tgt = self._cintf.get_dev_tgt()
        if key_content != None and key_handle != None:
            raise TdiTableError("{} Error: only one of key_content and key_handle can be passed.".format(self.name), self, -1)
        if entry_handle != None and "get_by_handle" not in self.supported_commands:
            raise TdiTableError("{} Error: getting entry by handle not supported.".format(self.name), self, -1)

        data_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_table_data_allocate(self._handle, byref(data_handle))
        if not sts == 0:
            return -1
        flag = 0
        flags_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_flags_create(flag,  byref(flags_handle))
        if not sts == 0:
            return -1
        if from_hw:
            flag = 1
        sts = self._cintf.get_driver().tdi_flags_set_value(flags_handle, self.flags_type_cls.flag_map(flag_enum_str="from_hw"), flag)
        if entry_handle != None:
            key_handle = self._cintf.handle_type()
            sts = self._cintf.get_driver().tdi_table_key_allocate(self._handle, byref(key_handle))
            if not sts == 0:
                self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
                return -1
            sts = self._cintf.tdi_table_entry_get_by_handle(
                    self._handle,
                    self._cintf.get_session(),
                    dev_tgt,
                    entry_handle,
                    key_handle, data_handle,
                    flags_handle)
        else:
            if key_handle is None:
                key_handle = self._make_call_keys(key_content)
                if key_handle == -1:
                    self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
                    return -1
            sts = self._cintf.tdi_table_entry_get(self._handle,
                    self._cintf.get_session(),
                    dev_tgt,
                    key_handle, data_handle,
                    flags_handle)

        if not sts == 0:
            if not (sts == 6 and not print_entry):
                print("Error: table_entry_get failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)))
            if not is_key_set:
                self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
            self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
            self._cintf.get_driver().tdi_flags_delete(flags_handle)
            return -1
        if print_entry:
            stream_printer = self.make_entry_stream_printer()
            action = []
    
            if len(self.actions) > 0:
                action = [self._action_from_data(data_handle)]

            stream_printer([key_handle], [data_handle], action)
        action = None

        if len(self.actions) > 0:
            action = self.action_id_name_map[self._action_from_data(data_handle)]

        entry = TableEntry(self, self._get_key_fields(key_handle), self._get_data_fields(data_handle, action), action)
        # If the key was passed from outside, don't deallocate here since it is the caller's
        # responsibility to do so. This is required in case of callbacks where the key is freed
        # by TDI function itself
        if not is_key_set:
            self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
        self._cintf.get_driver().tdi_table_data_deallocate(data_handle)
        self._cintf.get_driver().tdi_flags_delete(flags_handle)
        return entry

    def get_entry_handle(self, key_content, from_hw=False, print_entry=True, key_handle=None):
        if key_content != None and key_handle != None:
            raise TdiTableError("{} Error: only one of key_content and key_handle can be passed.".format(self.name), self, -1)
        if key_handle is None:
            key_handle = self._make_call_keys(key_content)
            if key_handle == -1:
                return -1
        handle = c_uint32(0)
        sts = self._cintf.tdi_table_entry_handle_get(
                self._handle,
                self._cintf.get_session(),
                self._cintf.get_dev_tgt(),
                key_handle,
                byref(handle))
        if not sts == 0:
            if not (sts == 6 and not print_entry):
                print("Error: get entry handle get  failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)))
            self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
            return -1
        if print_entry:
            print("Entry handle : {}".format(handle.value))
        return handle.value

    def get_entry_key(self, entry_handle, from_hw=False, print_entry=True):
        key_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_table_key_allocate(self._handle, byref(key_handle))
        if not sts == 0:
            return -1

        entry_tgt = self._cintf.target_type()
        sts = self._cintf.get_driver().tdi_target_create(self._cintf.get_device(), byref(entry_tgt))
        if not sts == 0:
            return -1
        sts = self._cintf.tdi_table_entry_key_get(
                self._handle,
                self._cintf.get_session(),
                self._cintf.get_dev_tgt(),
                entry_handle,
                entry_tgt,
                key_handle)
        if not sts == 0:
            if not (sts == 6 and not print_entry):
                print("Error: get_key failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)))
            self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
            return -1
        key = self._get_key_fields(key_handle)
        if print_entry:
            # Print key in copy-paste format
            key_str = ""
            for fname, fvalue in key.items():
                key_str += "{}={}, ".format(fname.decode("ascii"), fvalue)
            key_str += self._cintf.print_target(entry_tgt)
            print(key_str)
            stream_printer = self.make_entry_stream_printer()
            stream_printer([key_handle], None, None)
        self._cintf.get_driver().tdi_table_key_deallocate(key_handle)
        return key, entry_tgt

    def create_entry_obj(self, key_content, data_content, action=None):
        entry = self.table_entry_cls(self, key_content, data_content, action)
        return entry

    def _allocate_keydata_handles(self):
        key_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_table_key_allocate(self._handle, byref(key_handle))
        if not sts == 0:
            raise TdiTableError("CLI Error: table key allocate failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
        data_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_table_data_allocate(self._handle, byref(data_handle))
        if not sts == 0:
            raise TdiTableError("CLI Error: table data allocate failed. [{}].".format(self._cintf.err_str(sts)), self, sts)
        return key_handle, data_handle

    def _deallocate_hdls(self, key_hdls, data_hdls):
        for hdl in key_hdls:
            self._cintf.get_driver().tdi_table_key_deallocate(hdl)
        for hdl in data_hdls:
            self._cintf.get_driver().tdi_table_data_deallocate(hdl)

    def get_first(self, from_hw=False, print_ents=True):
        key_handle, data_handle = self._allocate_keydata_handles()
        if key_handle == -1:
            return -1
        flag = c_int(0)
        flags_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_flags_create(flag, byref(flags_handle))
        if from_hw:
            flag = c_int(1)
        sts = self._cintf.get_driver().tdi_flags_set_value(flags_handle, self.flags_type_cls.flag_map(flag_enum_str="from_hw"), flag);
        sts = self._cintf.tdi_table_entry_get_first(self._handle,
                                                    self._cintf.get_session(),
                                                    self._cintf.get_dev_tgt(),
                                                    key_handle,
                                                    data_handle,
                                                    flags_handle)
        if sts == 6 and print_ents:
            print("Table {} has no entries.".format(self.name))
            return -1, -1
        if not sts == 0:
            raise TdiTableError("Error: entry_get_first failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        return key_handle, data_handle

    def get_next(self, prev_key, n=20, from_hw=False):
        if "get_next_n" not in self.supported_commands:
            # If not supported, then just return empty lists
            return [], []
        arrtype = self._cintf.handle_type * n
        key_hdls = arrtype()
        data_hdls = arrtype()
        for i in range(0, n):
            new_key, new_data = self._allocate_keydata_handles()
            key_hdls[i] = new_key
            data_hdls[i] = new_data
        flag = c_int(0)
        flags_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_flags_create(flag, byref(flags_handle))
        if not sts == 0:
            return -1
        if from_hw:
            flag = c_int(1)
        sts = self._cintf.get_driver().tdi_flags_set_value(flags_handle, self.flags_type_cls.flag_map(flag_enum_str="from_hw"), flag);
        num_returned = c_uint(0)
        sts = self._cintf.tdi_table_entry_get_next_n(self._handle,
                                                     self._cintf.get_session(),
                                                     self._cintf.get_dev_tgt(),
                                                     prev_key,
                                                     key_hdls,
                                                     data_hdls,
                                                     n,
                                                     byref(num_returned),
                                                     flags_handle)
        if not sts == 0 and not sts == 6:
            # Once we are done reading all the entries from the table, tdi will
            # return an TDI_OBJECT_NOT_FOUND error
            raise TdiTableError("Error: entry_get_next {} failed on table {}. [{}]".format(n, self.name, self._cintf.err_str(sts)), self, sts)
        key_ret = []
        data_ret = []
        for idx in range(0, num_returned.value):
            key_ret.append(key_hdls[idx])
            data_ret.append(data_hdls[idx])
        return key_ret, data_ret

    def get_usage(self, from_hw=False):
        flag = 0
        flags_handle = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_flags_create(flag, byref(flags_handle))
        if not sts == 0:
            return -1
        if from_hw:
            flag = 1
        sts = self._cintf.get_driver().tdi_flags_set_value(flags_handle, self.flags_type_cls.flag_map(flag_enum_str="from_hw"), flag);

        table_type = self.table_type_cls.table_type_str(self.get_type())
        if table_type == -1:
            return -1
        if "usage_get" not in self.supported_commands:
            return "n/a"
        count = c_uint(0)
        session = self._cintf.get_session()
        dev_target = self._cintf.get_dev_tgt()
        sts = self._cintf.tdi_table_usage_get(self._handle,
                                              session,
                                              dev_target,
                                              byref(count),
                                              flags_handle)
        if not sts == 0:
            raise TdiTableError("Error: get usage failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        return count.value

    def get_capacity(self):
        count = c_size_t(0)
        session = self._cintf.get_session()
        dev_target = self._cintf.get_dev_tgt()
        sts = self._cintf.tdi_table_size_get(self._handle, session, dev_target, byref(count))
        if not sts == 0:
            raise TdiTableError("Error: get capacity failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        return count.value

    def get_type(self):
        table_type = c_int(-1)
        sts = self._cintf.get_driver().tdi_table_type_get(self._info_handle, byref(table_type))
        #print("table name= "+str(self.name)+" table_type="+str(table_type))
        if not sts == 0:
            raise TdiTableError("Error: get table_type failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        return table_type.value

    def set_supported_attributes_to_supported_commands(self):
        num_attrs = c_uint(0)
        sts = self._cintf.get_driver().tdi_table_num_attributes_supported(self._info_handle, byref(num_attrs))
        if sts != 0:
            raise TdiTableError("Error: num attributes supported get failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        arr_type = c_uint * num_attrs.value
        attributes_arr = arr_type()
        sts = self._cintf.get_driver().tdi_table_attributes_supported(self._info_handle, byref(attributes_arr), byref(num_attrs))
        if sts != 0:
            raise TdiTableError("Error: attributes supported get failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        logging.debug("For Table={} attributes_arr ==={}".format(self.name, str(attributes_arr[0:])));
        for i in range(len(attributes_arr)):
            self.supported_commands += self.attributes_type_cls.attributes_dict[attributes_arr[i]]

    def set_supported_operations_to_supported_commands(self):
        num_opers = c_uint(0)
        sts = self._cintf.get_driver().tdi_table_num_operations_supported(self._info_handle, byref(num_opers))
        if sts != 0:
            raise TdiTableError("Error: num operations supported get failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        arr_type = c_uint * num_opers.value
        operations_arr = arr_type()
        sts = self._cintf.get_driver().tdi_table_operations_supported(self._info_handle, byref(operations_arr), byref(num_opers))
        if sts != 0:
            raise TdiTableError("Error: operations supported get failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        logging.debug("For Table={} operations_arr ==={}".format(self.name, str(operations_arr[0:])));
        keys_list = self.operations_type_cls.operations_dict.keys()

        # TODO: Enable this when operations are supported
        '''
        for i in range(len(operations_arr)):
            if operations_arr[i] in keys_list:
                self.supported_commands.append(self.operations_type_cls.operations_dict[operations_arr[i]])
        '''

    def set_supported_apis_to_supported_commands(self):
        num_api = c_uint(0)
        sts = self._cintf.get_driver().tdi_table_num_api_supported(self._info_handle, byref(num_api))
        if sts != 0:
            raise TdiTableError("Error: num apis supported get failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        arr_type = c_uint * num_api.value
        api_arr = arr_type()
        sts = self._cintf.get_driver().tdi_table_api_supported(self._info_handle, byref(api_arr), byref(num_api))
        if sts != 0:
            raise TdiTableError("Error: apis supported get failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        # based on enum tdi_table_api_type_e
        logging.debug("For Table={} api_arr ==={}".format(self.name, str(api_arr[0:])));
        apis_dic = {0:  "add",
                    1:  "mod",
                    2:  "mod_inc",
                    3:  "delete",
                    4:  "clear",
                    5:  "set_default",
                    6:  "mod_default",
                    7:  "reset_default",
                    8:  "get_default",
                    9:  "get",
                    10: "get_first",
                    11: "get_next_n",
                    12: "usage_get",
                    13: "get_size",
                    14: "get_by_handle",
                    15: "get_key",
                    16: "get_handle",
                    17: "invalid_api"}
        keys_list = apis_dic.keys()
        for i in range(len(api_arr)):
            if api_arr[i] in keys_list:
                self.supported_commands.append(apis_dic[api_arr[i]])
            if apis_dic[api_arr[i]] == "get_default":
                if "dump" not in self.supported_commands:
                    self.supported_commands.append("dump")
            elif apis_dic[api_arr[i]] == "get_first":
                if "dump" not in self.supported_commands:
                    self.supported_commands.append("dump")

        if "add" in self.supported_commands:
            self.supported_commands.append("add_from_json")

    def _attr_deallocate(self, attr_hdl):
        sts = self._cintf.get_driver().tdi_attributes_deallocate(attr_hdl)
        if not sts == 0:
            raise TdiTableError("Error: attributes_deallocate failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)

    def _attr_set(self, attr_hdl):
        sts = self._cintf.tdi_table_attributes_set(self._handle,
                                                     self._cintf.get_session(),
                                                     self._cintf.get_dev_tgt(),
                                                     attr_hdl)
        if not sts == 0:
            raise TdiTableError("Error: table_attributes_set failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)

    def _attr_get(self, attr_hdl):
        sts = self._cintf.tdi_table_attributes_get(self._handle,
                                                     self._cintf.get_session(),
                                                     self._cintf.get_dev_tgt(),
                                                     attr_hdl)
        if not sts == 0:
            raise TdiTableError("Error: table_attributes_get failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)), self, sts)
        return sts


    def _table_operation_set(self, callback, operation):
        ops_hdl = self._cintf.handle_type()
        sts = self._cintf.get_driver().tdi_table_operations_allocate(self._handle, operation, byref(ops_hdl))
        if not sts == 0:
            print("table_operations_allocate failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)))
            return -1

        cb_setter = self._cintf.get_driver().tdi_operations_counter_sync_set
        if operation == 1:
            cb_setter = self._cintf.get_driver().tdi_operations_register_sync_set
        elif operation == 2:
            cb_setter = self._cintf.get_driver().tdi_operations_hit_state_update_set
        sts = cb_setter(ops_hdl, self._cintf.get_session(), self._cintf.get_dev_tgt(), callback, c_void_p(0))
        if not sts == 0:
            print("table operations setter failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)))
            self._cintf.get_driver().tdi_table_operations_deallocate(ops_hdl)
            return -1

        sts = self._cintf.get_driver().tdi_table_operations_execute(self._handle, ops_hdl)
        if not sts == 0:
            print("table_operations_execute failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)))
        sts1 = self._cintf.get_driver().tdi_table_operations_deallocate(ops_hdl)
        if not sts1 == 0:
            print("table_operations_deallocate failed on table {}. [{}]".format(self.name, self._cintf.err_str(sts)))
        return sts

    def _wrap_operation_callback(self, callback, op_type):
        def callback_wrapper(target, cookie):
            dev_id = target.contents.dev_id
            pipe_id = target.contents.pipe_id
            direction = target.contents.direction
            parser_id = target.contents.prsr_id
            callback(dev_id, pipe_id, direction, parser_id)
            if (op_type == "reg"):
                self.register_sync_cb = None
            elif (op_type == "counter"):
                self.counter_sync_cb = None
            elif (op_type == "hs"):
                self.hit_state_update_cb = None
            return
        return self._cintf.tbl_operations_cb_type(callback_wrapper)

    def operation_register_sync(self, callback=None):
        if not self.register_sync_cb == None:
            print("ERROR: Only 1 register sync operation allowed at time")
            return -1
        self.register_sync_cb = self._wrap_operation_callback(callback, "reg")
        sts = self._table_operation_set(self.register_sync_cb, 1)
        if not sts == 0:
            self.register_sync_cb = None

    def operation_counter_sync(self, callback=None):
        if not self.counter_sync_cb == None:
            print("ERROR: Only 1 counter sync operation allowed at time")
            return -1
        self.counter_sync_cb = self._wrap_operation_callback(callback, "counter")
        sts = self._table_operation_set(self.counter_sync_cb, 0)
        if not sts == 0:
            self.counter_sync_cb = None

    def operation_hit_state_update(self, callback=None):
        if not self.hit_state_update_cb == None:
            print("ERROR: Only 1 hit state operation allowed at time")
            return -1
        self.hit_state_update_cb = self._wrap_operation_callback(callback, "hs")
        sts = self._table_operation_set(self.hit_state_update_cb, 2)
        if not sts == 0:
            self.hit_state_update_cb = None

    def _action_from_data(self, data_hdl):
        if len(self.actions) < 0:
            return -1
       
        action_id = c_uint(-1)
        sts = self._cintf.get_driver().tdi_data_action_id_get(data_hdl, byref(action_id))
        if not sts == 0:
            print("action_id_from_data_get failed. [{}]".format(self._cintf.err_str(sts)))
            return -2
        return action_id.value

    def clear(self, batch):
        if batch:
            self._cintf._begin_batch()
        sts = self._cintf.tdi_table_clear(self._handle,
                                            self._cintf.get_session(),
                                            self._cintf.get_dev_tgt())
        if batch:
            self._cintf._end_batch()
        if not sts == 0:
            raise TdiTableError("Error: Table clear failed on table {}. [{}]".format(self.name, sts), self, sts)

    def dump(self, entry_handler, from_hw=False, print_ents=True, print_zero=True):
        if "get_first" not in self.supported_commands:
            return 0
        key_hdl, data_hdl = self.get_first(from_hw, print_ents)
        if key_hdl == -1:
            return -1

        key_hdls = [key_hdl]
        data_hdls = [data_hdl]
        prev_key_hdl = key_hdl
        while True:
            next_key_hdls, next_data_hdls = self.get_next(prev_key_hdl, 20 - len(key_hdls), from_hw)
            if next_key_hdls == -1:
                return -1
            key_hdls += next_key_hdls
            data_hdls += next_data_hdls
            if len(key_hdls) == 0:
                self._cintf.get_driver().tdi_table_key_deallocate(prev_key_hdl)
                return 0;

            action_ids = []

            if len(self.actions) > 0:
                for d_hdl in data_hdls:
                    action_ids.append(self._action_from_data(d_hdl))

            entry_handler(key_hdls, data_hdls, action_ids, print_zero)
            self._deallocate_hdls(key_hdls[:-1], data_hdls)
            prev_key_hdl = key_hdls[-1]

            if len(key_hdls) < 20:
                self._cintf.get_driver().tdi_table_key_deallocate(prev_key_hdl)
                return 0;
            key_hdls = []
            data_hdls = []

    def raw_entry(self, key_content, data_content, action):
        raw_key = {}
        raw_data = {}
        raw_action = None
        for name, val in key_content.items():
            deparsed = self.key_fields[name].deparse_output(val)
            raw_key[name.decode('ascii')] = deparsed
        data_fields = self.data_fields
        if not action == None:
            data_fields = self.actions[action.encode('ascii')]["data_fields"]
            raw_action = action
        for name, val in data_content.items():
            deparsed = data_fields[name].deparse_output(val)
            raw_data[name.decode('ascii')]  = deparsed
        return {"table_name": self.name, "action": raw_action, "key": raw_key, "data": raw_data}

    def print_entry(self, key_content, data_content, action):
        to_print = ""
        if key_content != None:
            to_print += "Entry key:\n"
            for name, val in sorted(key_content.items(), key=lambda i: self.key_fields[i[0]].id):
                deparsed = self.key_fields[name].stringify_output(val)
                to_print += "    {} : {}\n".format(name.decode('ascii').ljust(30), deparsed)
        if data_content is None:
            return to_print, True
        data_fields = self.data_fields

        if action != None:
            data_fields = self.actions[action]["data_fields"]
            to_print += "Entry data (action : {}):\n".format(action.decode('ascii'))
        else:
            to_print += "Entry data:\n"
        all_zero = True
        for name, val in sorted(data_content.items(), key=lambda i: data_fields[i[0]].id):
            if 0 != val:
                if isinstance(val, list):
                    for elem in val:
                        if elem != 0:
                            all_zero = False
                            break
                else:
                    all_zero = False
            deparsed = data_fields[name].stringify_output(val)
            to_print += "    {} : {}\n".format(name.decode('ascii').ljust(30), deparsed)
        return to_print, all_zero

    def make_entry_stream_printer(self):
        entry_num = 0
        table = self
        def print_entry_stream(key_hdls, data_hdls, action_ids, print_zero=True):
            nonlocal entry_num
            nonlocal table
            # Default entries
            if key_hdls != None:
                iterval = len(key_hdls)
            else:
                iterval = len(data_hdls)
            for i in range(0, iterval):
                action = None
                if action_ids != None:
                    if len(table.actions) > 0:
                        action = table.action_id_name_map[action_ids[i]]

                key_content = None
                if key_hdls != None:
                    key = key_hdls[i]
                    key_content = table._get_key_fields(key)
                    if not isinstance(key_content, dict):
                        return
                data_content = None
                if data_hdls != None:
                    data = data_hdls[i]
                    data_content = table._get_data_fields(data, action)
                    if not isinstance(data_content, dict):
                        return
                to_print, data_zero = table.print_entry(key_content, data_content, action)
                if print_zero or not data_zero:
                    print("Entry {}:".format(entry_num))
                    print(to_print)
                entry_num += 1
        return print_entry_stream

    """
    This method parses information provided by the user into the
    appropriate types for use with the ctypes foriegn function interface.
    The type information is mapped from the information encoded in the
    TDI Runtime leafs' method definitions.
    """
    def parse_str_input(self, method_name, key_strs, data_strs, action=None):
        parsed_keys = {}
        if key_strs != None:
            for name, input_ in key_strs.items():
                if isinstance(name, str):
                    name = name.encode('UTF-8')
                success, parsed = self.key_fields[name].parse_input(method_name, input_)
                if not success:
                    return -1, {}
                parsed_keys[name] = parsed

        parsed_data = {}
        if data_strs != None:
            data_fields = self.data_fields

            if action != None:
                data_fields = self.actions[action]["data_fields"]

            for name, input_ in data_strs.items():
                if isinstance(name, str):
                    name = name.encode('UTF-8')
                success, parsed = data_fields[name].parse_input(method_name, input_)
                if not success:
                    return -1, {}
                parsed_data[name] = parsed

        return parsed_keys, parsed_data

    """
        To be overriden by the targets on need basis
    """
    def modify_table_names(self, table_name):
        pass
