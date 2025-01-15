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



import json
from ptf import testutils
from bf_switcht_api_thrift.ttypes import *
import ctypes


def twos_comp(val, bits):
    """compute the 2's complement of int value val"""
    if (val & (1 << (bits - 1))) != 0: # if sign bit is set e.g., 8bit: 128-255
        val = val - (1 << bits)        # compute negative value
    return val                         # return positive value as is

def object_info_map_build():
    map = {}
    objects_k = 'objects'
    objects_v = model[objects_k]
    for obj_k  in objects_v:
        obj_v = objects_v[obj_k]
        ot_k = "object_type"
        ot_v = obj_v[ot_k]
        obj_v["object_name"] = obj_k
        map[ot_v] = obj_v
    return map

def attr_md_map_build():
    map = {}
    objects_k = 'objects'
    objects_v = model[objects_k]
    for obj_k in objects_v:
        obj_v = objects_v[obj_k]
        attrs_k = "attributes"
        attrs_v = obj_v[attrs_k]
        for attr_k in attrs_v:
            attr_v = attrs_v[attr_k]
            attr_id_k = "attr_id"
            attr_id_v = attr_v[attr_id_k]
            attr_v["attr_name"] = attr_k
            map[attr_id_v] = attr_v
    return map

def object_type_get( object_name):
    objects_k = 'objects'
    objects_v = model[objects_k]
    obj_k = object_name
    obj_v = objects_v[obj_k]
    ot_k = 'object_type'
    ot_v = obj_v[ot_k]
    assert (ot_v)
    return ot_v

def attr_id_get(object_name, attr_name):
    objects_k = 'objects'
    objects_v = model[objects_k]
    obj_k = object_name
    obj_v = objects_v[obj_k]
    attrs_k = 'attributes'
    attrs_v = obj_v[attrs_k]
    attr_k = attr_name
    attr_v = attrs_v[attr_k]
    attr_id_k = "attr_id"
    attr_id_v = attr_v[attr_id_k]
    assert (attr_id_v)
    return attr_id_v


# Hacky stuff ahead..
# Thrift doesn't have unsigned types, doing conversions here.
def attr_make( attr_id, val_in):

    attr_md = attrs[attr_id]
    type_info_k = "type_info"
    type_info_v = attr_md[type_info_k]
    type_k = "type"
    type_v = type_info_v[type_k]

    if type_v == 'SWITCH_TYPE_BOOL':
        val = switcht_value_t(type=switcht_value_type.BOOL, BOOL=val_in)
    elif type_v == 'SWITCH_TYPE_UINT8':
        val = switcht_value_t(type=switcht_value_type.UINT8, UINT8=val_in)
    elif type_v == 'SWITCH_TYPE_UINT16':
        val = switcht_value_t(type=switcht_value_type.UINT16, UINT16=val_in)
    elif type_v == 'SWITCH_TYPE_UINT32':
        val = switcht_value_t(type=switcht_value_type.UINT32, UINT32=val_in)
    elif type_v == 'SWITCH_TYPE_UINT64':
        val = switcht_value_t(type=switcht_value_type.UINT64, UINT64=val_in)
    elif type_v == 'SWITCH_TYPE_INT64':
        val = switcht_value_t(type=switcht_value_type.INT64, INT64=val_in)
    elif type_v == 'SWITCH_TYPE_ENUM':
        val = switcht_value_t(type=switcht_value_type.ENUM, ENUM=val_in)
    elif type_v == 'SWITCH_TYPE_OBJECT_ID':
        val = switcht_value_t(type=switcht_value_type.OBJECT_ID, OBJECT_ID=val_in)
    elif type_v == 'SWITCH_TYPE_MAC':
        val = switcht_value_t(type=switcht_value_type.MAC, MAC=val_in)
    elif type_v == 'SWITCH_TYPE_STRING':
        val = switcht_value_t(type=switcht_value_type.STRING, STRING=val_in)
    elif type_v == 'SWITCH_TYPE_IP_ADDRESS':
        val = switcht_value_t(type=switcht_value_type.IP_ADDRESS, IP_ADDRESS=val_in)
    elif type_v == 'SWITCH_TYPE_IP_PREFIX':
        val = switcht_value_t(type=switcht_value_type.IP_PREFIX, IP_PREFIX=val_in)
    elif type_v == 'SWITCH_TYPE_LIST':
        val = switcht_value_t(type=switcht_value_type.LIST, LIST=tuple(val_in))
    else:
        assert(0)
    return switcht_attribute_t(id=attr_id, value=val)


model_file_name = testutils.test_param_get('api_model_json')
if model_file_name is None:
    model_file_name = 'install/share/switch/aug_model.json'
print("Model file %s" % model_file_name)
assert(model_file_name)
with open(model_file_name) as model_file:
    model = json.load(model_file)
assert(model)
objects = object_info_map_build()
assert(objects)
attrs = attr_md_map_build()
assert(attrs)


def lane_list_t(lanes):
    lane_list = []
    for lane in list(lanes):
        lane_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=lane))
    return lane_list
