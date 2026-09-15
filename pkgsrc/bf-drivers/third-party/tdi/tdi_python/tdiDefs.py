# SPDX-FileCopyrightText: 2021 Intel Corporation
# Copyright (C) 2021 Intel Corporation.
#
# SPDX-License-Identifier: Apache-2.0
from enum import Enum, auto

"""
    These maps are based on the enums defined in tdi_defs.h If the
    enums there are changed, these maps must also be changed.
"""

# tdi_target_e
class TargetEnum(Enum):
  TDI_TARGET_CORE = 0
  TDI_TARGET_ARCH = 0x08
  TDI_TARGET_DEVICE = 0x80

"""
    Targets can derived these classes to support custom types
"""
# tdi_target_core_enum_e
class TargetType:
    class TargetEnumCore(Enum):
        TDI_TARGET_DEV_ID = TargetEnum.TDI_TARGET_CORE.value

    target_type_dict = {
        TargetEnumCore.TDI_TARGET_DEV_ID.value: "dev_id"
    }
    target_type_rev_dict = {
        "dev_id": TargetEnumCore.TDI_TARGET_DEV_ID.value
    }

    @classmethod
    def target_type_map(cls, target_type_enum=None, target_type_str=None):
        if target_type_enum is not None:
            return cls.target_type_dict[target_type_enum]
        if target_type_str is not None:
            return cls.target_type_rev_dict[target_type_str]

class FlagsEnum(Enum):
    TDI_FLAGS_CORE = 0
    TDI_FLAGS_ARCH = 0x08
    TDI_FLAGS_DEVICE = 0x10
    TDI_FLAGS_END = 0x40

class FlagsType:
    # No generic flags for now
    flags_dict = {}
    flags_rev_dict = {}

    @classmethod
    def flag_map(cls, flag_enum=None, flag_enum_str=None):
        if flag_enum is not None:
            return cls.flags_dict[flag_enum]
        if flag_enum_str is not None:
            return cls.flags_rev_dict[flag_enum_str]

# tdi_attributes_type_e
class AttributesEnum(Enum):
    TDI_ATTRIBUTES_TYPE_CORE = 0
    TDI_ATTRIBUTES_TYPE_ARCH = 0x08
    TDI_ATTRIBUTES_TYPE_DEVICE = 0x80

class AttributesType:
    # Attributes defined in target
    attributes_dict = {}

# tdi_operations_type_e
class OperationsEnum(Enum):
  TDI_OPERATIONS_TYPE_CORE = 0
  TDI_OPERATIONS_TYPE_ARCH = 0x08
  TDI_OPERATIONS_TYPE_DEVICE = 0x80

class OperationsType:
    # Operations defined in target'''
    operations_dict = {}

# tdi_match_type_e
class KeyMatchEnum(Enum):
    TDI_MATCH_TYPE_CORE = 0
    TDI_MATCH_TYPE_ARCH = 0x08
    TDI_MATCH_TYPE_DEVICE = 0x80

# tdi_match_type_core_e
class KeyMatchType:
    class KeyMatchEnumCore(Enum):
        TDI_MATCH_TYPE_EXACT = KeyMatchEnum.TDI_MATCH_TYPE_CORE.value
        TDI_MATCH_TYPE_TERNARY = auto()
        TDI_MATCH_TYPE_LPM = auto()
        TDI_MATCH_TYPE_RANGE = auto()
        TDI_MATCH_TYPE_OPTIONAL = auto()

    key_match_type_dict = {
        KeyMatchEnumCore.TDI_MATCH_TYPE_EXACT.value: "EXACT",
        KeyMatchEnumCore.TDI_MATCH_TYPE_TERNARY.value: "TERNARY",
        KeyMatchEnumCore.TDI_MATCH_TYPE_LPM.value: "LPM",
        KeyMatchEnumCore.TDI_MATCH_TYPE_RANGE.value: "RANGE",
        KeyMatchEnumCore.TDI_MATCH_TYPE_OPTIONAL.value: "OPTIONAL"}

    @classmethod
    def key_match_type_str(cls, key_type):
        if key_type in cls.key_match_type_dict.keys():
            return cls.key_match_type_dict[key_type]
        else:
            return "TDI_CLI_KEY_TYPE_NOT_IMPLEMENTED"

# tdi_field_data_type_e
class DataType:
    class DataTypeEnum(Enum):
        TDI_FIELD_DATA_TYPE_INT_ARR = 0
        TDI_FIELD_DATA_TYPE_BOOL_ARR = 1
        TDI_FIELD_DATA_TYPE_UINT64 = 2
        TDI_FIELD_DATA_TYPE_BYTE_STREAM = 3
        TDI_FIELD_DATA_TYPE_FLOAT = 4
        TDI_FIELD_DATA_TYPE_CONTAINER = 5
        TDI_FIELD_DATA_TYPE_STRING = 6
        TDI_FIELD_DATA_TYPE_BOOL = 7
        TDI_FIELD_DATA_TYPE_STRING_ARR = 8
        TDI_FIELD_DATA_TYPE_INT64 = 9
        TDI_FIELD_DATA_TYPE_UNKNOWN = 10

    data_type_dict = {
            DataTypeEnum.TDI_FIELD_DATA_TYPE_INT_ARR.value: "INT_ARR",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_BOOL_ARR.value: "BOOL_ARR",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_UINT64.value: "UINT64",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_BYTE_STREAM.value: "BYTE_STREAM",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_FLOAT.value: "FLOAT",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_CONTAINER.value: "CONTAINER",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_STRING.value: "STRING",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_BOOL.value: "BOOL",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_STRING_ARR.value: "STR_ARR",
            DataTypeEnum.TDI_FIELD_DATA_TYPE_INT64.value: "INT64"
    }

    @classmethod
    def data_type_str(cls, data_type):
        if data_type in cls.data_type_dict.keys():
            return cls.data_type_dict[data_type];
        else:
            return "TDI_CLI_DATA_TYPE_NOT_IMPLEMENTED"

# tdi_table_type_e
class TableTypeEnum(Enum):
    TDI_TABLE_TYPE_CORE = 0x0000
    TDI_TABLE_TYPE_ARCH = 0x0080
    TDI_TABLE_TYPE_DEVICE = 0x0800

class TableType:
    class TableTypeEnumCore(Enum):
        # No core tables
        pass

    table_type_dict = {}

    @classmethod
    def table_type_str(cls, table_type):
        if table_type in cls.table_type_dict.keys():
            return cls.table_type_dict[table_type]
        else:
            return "TDI_TABLE_TYPE_NOT_SUPPORTED"
