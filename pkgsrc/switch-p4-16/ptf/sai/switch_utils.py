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
Thrift SAI interface basic tests
"""

import switchsai_thrift
from sai_base_test import *
import time
import sys
import logging

import unittest
import random

import sai_base_test

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os
import ptf.mask

from switchsai_thrift.ttypes import  *
from switchsai_thrift.sai_headers import  *


switch_ports = []
cpu_ports = []

'''
Not adding port 320 in the CPU port list.
WIll be taken care once PCIs is available for the HW.
'''

def get_switch_ports():
    test_params = testutils.test_params_get()
    for device, port, ifname in config["interfaces"]:
        if test_params['arch'] == "tofino2":
            if port == 320:
                continue
            if port == 2:
                cpu_ports.append(port)
                continue
        else:
            if port == 64 or port == 320:
                cpu_ports.append(port)
                continue

        switch_ports.append(port)

get_switch_ports()

class VlanObj:
    def __init__(self):
        self.oid = 0
        self.vid = 0

class SwitchObj:
    def __init__(self):
        self.default_1q_bridge = SAI_NULL_OBJECT_ID
        self.default_vlan = VlanObj()

"""
0xFFFF cannot pass the validation,
the reason is that sai u16 is mapped to thrift i16 which is checked to be in [-32768, 32767] range
but "-1" is serialized to 0xFFFF.
"""
U16MASKFULL = -1

MAX_QUANTIZATION = 30
switch_inited=0
port_list = {}
sai_port_list = []
table_attr_list = []
router_mac='00:77:66:55:44:00'
rewrite_mac1='00:77:66:55:44:01'
rewrite_mac2='00:77:66:55:44:02'
vxlan_router_mac='00:77:66:55:44:03'
switch = SwitchObj()

is_bmv2 = ('BMV2_TEST' in os.environ) and (int(os.environ['BMV2_TEST']) == 1)


def switch_init(client):
    global switch_inited

    if switch_inited:
        return
    switch.default_1q_bridge = client.sai_thrift_get_default_1q_bridge_id()
    assert (switch.default_1q_bridge != SAI_NULL_OBJECT_ID)

    ret = client.sai_thrift_get_default_vlan_id()
    assert (ret.status == SAI_STATUS_SUCCESS), "Failed to get default vlan"
    switch.default_vlan.oid = ret.data.oid

    #ret = client.sai_thrift_get_vlan_id(switch.default_vlan.oid)
    #assert (ret.status == SAI_STATUS_SUCCESS), "Failed obtain default vlan id"
    #switch.default_vlan.vid = ret.data.u16

    for interface,front in interface_to_front_mapping.iteritems():
      sai_port_id = client.sai_thrift_get_port_id_by_front_port(front);
      print front, ":", hex(sai_port_id), ":", int(interface)
      port_list[int(interface)]=sai_port_id

    switch_attr_list = client.sai_thrift_get_switch_attribute()
    attr_list = switch_attr_list.attr_list
    for attribute in attr_list:
        if attribute.id == SAI_SWITCH_ATTR_PORT_NUMBER:
            print "max ports: %d" % attribute.value.u32
        elif attribute.id == SAI_SWITCH_ATTR_PORT_LIST:
            for port_id in attribute.value.objlist.object_id_list:
                if port_id in port_list.values():
                    attr_value = sai_thrift_attribute_value_t(booldata=1)
                    attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_ADMIN_STATE, value=attr_value)
                    client.sai_thrift_set_port_attribute(port_id, attr)
                    sai_port_list.append(port_id)
        elif attribute.id == SAI_SWITCH_ATTR_DEFAULT_VIRTUAL_ROUTER_ID:
            print "VRF ID: 0x%x" % attribute.value.oid
        else:
            print "unknown switch attribute %d" % attribute.id
    attr_value = sai_thrift_attribute_value_t(mac=router_mac)
    attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_SRC_MAC_ADDRESS, value=attr_value)
    client.sai_thrift_set_switch_attribute(attr)
    all_ports_are_up = True
    for port in sai_port_list:
        port_attr_list = client.sai_thrift_get_port_attribute(port)

    '''
    for num_of_tries in range(200):
        time.sleep(1)
        # wait till the port are up
        for port in sai_port_list:
            port_attr_list = client.sai_thrift_get_port_attribute(port)
            attr_list = port_attr_list.attr_list
            for attribute in attr_list:
                if attribute.id == SAI_PORT_ATTR_OPER_STATUS:
                    if attribute.value.s32 != SAI_PORT_OPER_STATUS_UP:
                        all_ports_are_up = False
                        print "port 0x%x is down" % port
        if all_ports_are_up:
            break
        else:
            all_ports_are_up = True
    if not all_ports_are_up:
        raise RuntimeError('Not all of the  ports are up')
    '''

    switch_inited = 1

def sai_thrift_get_bridge_port_by_port(client, port_id):
    ret = client.sai_thrift_get_bridge_port_list(switch.default_1q_bridge)
    assert (ret.status == SAI_STATUS_SUCCESS)

    for bp in ret.data.objlist.object_id_list:
        attrs = client.sai_thrift_get_bridge_port_attribute(bp)
        bport = SAI_NULL_OBJECT_ID
        is_port = False
        for a in attrs.attr_list:
            if a.id == SAI_BRIDGE_PORT_ATTR_PORT_ID:
                bport = a.value.oid
            if a.id == SAI_BRIDGE_PORT_ATTR_TYPE:
                is_port = a.value.s32 == SAI_BRIDGE_PORT_TYPE_PORT

        if is_port and bport == port_id:
            return bp

    return SAI_NULL_OBJECT_ID

def sai_thrift_get_port_by_bridge_port(client, bp):
    attrs = client.sai_thrift_get_bridge_port_attribute(bp)
    port = SAI_NULL_OBJECT_ID

    for a in attrs.attr_list:
        if a.id == SAI_BRIDGE_PORT_ATTR_PORT_ID:
            return a.value.oid

    return SAI_NULL_OBJECT_ID

def sai_thrift_create_bridge(client, type, learning=False):
    attr_list = []
    attr_value = sai_thrift_attribute_value_t(s32=type)
    attr = sai_thrift_attribute_t(id=SAI_BRIDGE_ATTR_TYPE, value=attr_value)
    attr_list.append(attr)
    attr_value = sai_thrift_attribute_value_t(booldata=learning)
    attr = sai_thrift_attribute_t(id=SAI_BRIDGE_ATTR_LEARN_DISABLE, value=attr_value)
    attr_list.append(attr)
    bridge_id = client.sai_thrift_create_bridge(attr_list)
    return bridge_id

def sai_thrift_create_bridge_port(client, type, port_id, bridge_id=0, admin_state=True):
    if (bridge_id == 0):
        br_id = switch.default_1q_bridge
    else:
        br_id = bridge_id
    bport_attr1_value = sai_thrift_attribute_value_t(s32=type)
    bport_attr1 = sai_thrift_attribute_t(id=SAI_BRIDGE_PORT_ATTR_TYPE,
                                              value=bport_attr1_value)

    if (type == SAI_BRIDGE_PORT_TYPE_PORT):
        bport_attr2_value = sai_thrift_attribute_value_t(oid=port_id)
        bport_attr2 = sai_thrift_attribute_t(id=SAI_BRIDGE_PORT_ATTR_PORT_ID,
                                              value=bport_attr2_value)
    if (type == SAI_BRIDGE_PORT_TYPE_TUNNEL):
        bport_attr2_value = sai_thrift_attribute_value_t(oid=port_id)
        bport_attr2 = sai_thrift_attribute_t(id=SAI_BRIDGE_PORT_ATTR_TUNNEL_ID,
                                              value=bport_attr2_value)

    bport_attr3_value = sai_thrift_attribute_value_t(oid=br_id)
    bport_attr3 = sai_thrift_attribute_t(id=SAI_BRIDGE_PORT_ATTR_BRIDGE_ID,
                                          value=bport_attr3_value)

    bport_attr4_value = sai_thrift_attribute_value_t(booldata=admin_state)
    bport_attr4 = sai_thrift_attribute_t(id=SAI_BRIDGE_PORT_ATTR_ADMIN_STATE,
                                          value=bport_attr4_value)

    bport_attr_list = [bport_attr1, bport_attr2, bport_attr3, bport_attr4]

    ret = client.sai_thrift_create_bridge_port(bport_attr_list)
    assert (ret.status == SAI_STATUS_SUCCESS)
    assert (ret.data.oid != SAI_NULL_OBJECT_ID)

    return ret.data.oid

def sai_thrift_get_cpu_port_id(client):
    cpu_port = client.sai_thrift_get_cpu_port_id()
    return cpu_port

def sai_thrift_get_default_vlan_id(client):
    vlan_id = client.sai_thrift_get_default_vlan_id()
    return vlan_id

def sai_thrift_get_default_router_id(client):
    default_router_id = client.sai_thrift_get_default_router_id()
    return default_router_id

def sai_thrift_create_fdb(client, bv_id, mac, port, mac_action):
    bport_oid = sai_thrift_get_bridge_port_by_port(client, port)
    assert (bport_oid != SAI_NULL_OBJECT_ID)

    fdb_entry = sai_thrift_fdb_entry_t(mac_address=mac, bv_id=bv_id)
    #value 0 represents static entry, id=0, represents entry type
    fdb_attribute1_value = sai_thrift_attribute_value_t(s32=SAI_FDB_ENTRY_TYPE_STATIC)
    fdb_attribute1 = sai_thrift_attribute_t(id=SAI_FDB_ENTRY_ATTR_TYPE,
                                            value=fdb_attribute1_value)
    #value oid represents object id, id=1 represents port id
    fdb_attribute2_value = sai_thrift_attribute_value_t(oid=bport_oid)
    fdb_attribute2 = sai_thrift_attribute_t(id=SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID,
                                            value=fdb_attribute2_value)
    #value oid represents object id, id=1 represents port id
    fdb_attribute3_value = sai_thrift_attribute_value_t(s32=mac_action)
    fdb_attribute3 = sai_thrift_attribute_t(id=SAI_FDB_ENTRY_ATTR_PACKET_ACTION,
                                            value=fdb_attribute3_value)
    fdb_attr_list = [fdb_attribute1, fdb_attribute2, fdb_attribute3]
    client.sai_thrift_create_fdb_entry(thrift_fdb_entry=fdb_entry, thrift_attr_list=fdb_attr_list)

def sai_thrift_delete_fdb(client, bv_id, mac, port):
    fdb_entry = sai_thrift_fdb_entry_t(mac_address=mac, bv_id=bv_id)
    client.sai_thrift_delete_fdb_entry(thrift_fdb_entry=fdb_entry)

def sai_thrift_flush_fdb(client, vlan_oid=0, lag_port_oid=0, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC):
    fdb_attribute1_value = sai_thrift_attribute_value_t(oid=vlan_oid)
    fdb_attribute1 = sai_thrift_attribute_t(id=SAI_FDB_FLUSH_ATTR_BV_ID,
                                            value=fdb_attribute1_value)

    bport_oid = sai_thrift_get_bridge_port_by_port(client, lag_port_oid)
    fdb_attribute2_value = sai_thrift_attribute_value_t(oid=bport_oid)
    fdb_attribute2 = sai_thrift_attribute_t(id=SAI_FDB_FLUSH_ATTR_BRIDGE_PORT_ID,
                                            value=fdb_attribute2_value)

    fdb_attribute3_value = sai_thrift_attribute_value_t(s32=flush_type)
    fdb_attribute3 = sai_thrift_attribute_t(id=SAI_FDB_FLUSH_ATTR_ENTRY_TYPE,
                                            value=fdb_attribute3_value)
    fdb_attr_list = [fdb_attribute1, fdb_attribute2, fdb_attribute3]
    client.sai_thrift_flush_fdb_entries(thrift_attr_list=fdb_attr_list)

def sai_thrift_flush_fdb_by_vlan(client, vlan_oid):
    fdb_attribute1_value = sai_thrift_attribute_value_t(oid=vlan_oid)
    fdb_attribute1 = sai_thrift_attribute_t(id=SAI_FDB_FLUSH_ATTR_BV_ID,
                                            value=fdb_attribute1_value)
    fdb_attribute2_value = sai_thrift_attribute_value_t(s32=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
    fdb_attribute2 = sai_thrift_attribute_t(id=SAI_FDB_FLUSH_ATTR_ENTRY_TYPE,
                                            value=fdb_attribute2_value)
    fdb_attr_list = [fdb_attribute1, fdb_attribute2]
    client.sai_thrift_flush_fdb_entries(thrift_attr_list=fdb_attr_list)

def sai_thrift_flush_fdb_by_lag_port(client, lag_port_oid):
    bport_oid = sai_thrift_get_bridge_port_by_port(client, lag_port_oid)
    assert (bport_oid != SAI_NULL_OBJECT_ID)

    fdb_attribute1_value = sai_thrift_attribute_value_t(oid=bport_oid)
    fdb_attribute1 = sai_thrift_attribute_t(id=SAI_FDB_FLUSH_ATTR_BRIDGE_PORT_ID,
                                            value=fdb_attribute1_value)
    fdb_attribute2_value = sai_thrift_attribute_value_t(s32=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
    fdb_attribute2 = sai_thrift_attribute_t(id=SAI_FDB_FLUSH_ATTR_ENTRY_TYPE,
                                            value=fdb_attribute2_value)
    fdb_attr_list = [fdb_attribute1, fdb_attribute2]
    client.sai_thrift_flush_fdb_entries(thrift_attr_list=fdb_attr_list)

def sai_thrift_create_virtual_router(client, v4_enabled, v6_enabled):
    #v4 enabled
    vr_attribute1_value = sai_thrift_attribute_value_t(booldata=v4_enabled)
    vr_attribute1 = sai_thrift_attribute_t(id=SAI_VIRTUAL_ROUTER_ATTR_ADMIN_V4_STATE,
                                           value=vr_attribute1_value)
    #v6 enabled
    vr_attribute2_value = sai_thrift_attribute_value_t(booldata=v6_enabled)
    vr_attribute2 = sai_thrift_attribute_t(id=SAI_VIRTUAL_ROUTER_ATTR_ADMIN_V6_STATE,
                                           value=vr_attribute2_value)
    vr_attr_list = [vr_attribute1, vr_attribute2]
    vr_id = client.sai_thrift_create_virtual_router(thrift_attr_list=vr_attr_list)
    return vr_id

def sai_thrift_create_router_interface(client, vr_oid, type, port_oid, vlan_oid, v4_enabled, v6_enabled, mac, outer_vlan_id=0, inner_vlan_id=0, mtu=0, nat_zone=0):
    #vrf attribute
    rif_attr_list = []
    rif_attribute1_value = sai_thrift_attribute_value_t(oid=vr_oid)
    rif_attribute1 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_VIRTUAL_ROUTER_ID,
                                            value=rif_attribute1_value)
    rif_attr_list.append(rif_attribute1)
    rif_attribute2_value = sai_thrift_attribute_value_t(s32=type)
    rif_attribute2 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_TYPE,
                                            value=rif_attribute2_value)
    rif_attr_list.append(rif_attribute2)

    if type == SAI_ROUTER_INTERFACE_TYPE_PORT:
        #port type and port id
        rif_attribute3_value = sai_thrift_attribute_value_t(oid=port_oid)
        rif_attribute3 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_PORT_ID,
                                                value=rif_attribute3_value)
        rif_attr_list.append(rif_attribute3)
    elif type == SAI_ROUTER_INTERFACE_TYPE_VLAN:
        #vlan type and vlan id
        rif_attribute3_value = sai_thrift_attribute_value_t(oid=vlan_oid)
        rif_attribute3 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_VLAN_ID,
                                                value=rif_attribute3_value)
        rif_attr_list.append(rif_attribute3)

    elif type == SAI_ROUTER_INTERFACE_TYPE_BRIDGE:
        #no need to specify port or vlan
        pass
    elif type == SAI_ROUTER_INTERFACE_TYPE_SUB_PORT:
        rif_attribute3_value = sai_thrift_attribute_value_t(oid=port_oid)
        rif_attribute3 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_PORT_ID,
                                                value=rif_attribute3_value)
        rif_attr_list.append(rif_attribute3)

        rif_attribute4_value = sai_thrift_attribute_value_t(u16=outer_vlan_id)
        rif_attribute4 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_OUTER_VLAN_ID,
                                                value=rif_attribute4_value)
        rif_attr_list.append(rif_attribute4)
    elif type == SAI_ROUTER_INTERFACE_TYPE_QINQ_PORT:
        rif_attribute3_value = sai_thrift_attribute_value_t(oid=port_oid)
        rif_attribute3 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_PORT_ID,
                                                value=rif_attribute3_value)
        rif_attr_list.append(rif_attribute3)

        rif_attribute7_value = sai_thrift_attribute_value_t(u16=outer_vlan_id)
        rif_attribute7 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_OUTER_VLAN_ID,
                                                value = rif_attribute7_value)
        rif_attr_list.append(rif_attribute7)

        rif_attribute8_value = sai_thrift_attribute_value_t(u16=inner_vlan_id)
        rif_attribute8 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_INNER_VLAN_ID,
                                                value = rif_attribute8_value)
        rif_attr_list.append(rif_attribute8)

    #v4_enabled
    rif_attribute4_value = sai_thrift_attribute_value_t(booldata=v4_enabled)
    rif_attribute4 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE,
                                            value=rif_attribute4_value)
    rif_attr_list.append(rif_attribute4)
    #v6_enabled
    rif_attribute5_value = sai_thrift_attribute_value_t(booldata=v6_enabled)
    rif_attribute5 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_ADMIN_V6_STATE,
                                            value=rif_attribute5_value)
    rif_attr_list.append(rif_attribute5)

    if mac:
        rif_attribute6_value = sai_thrift_attribute_value_t(mac=mac)
        rif_attribute6 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_SRC_MAC_ADDRESS,
                                                value=rif_attribute6_value)
        rif_attr_list.append(rif_attribute6)

    #mtu
    rif_attribute7_value = sai_thrift_attribute_value_t(u32=mtu)
    rif_attribute7 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_MTU,
                                            value=rif_attribute7_value)
    rif_attr_list.append(rif_attribute7)

    if nat_zone:
      rif_attribute8_value = sai_thrift_attribute_value_t(u8=nat_zone)
      rif_attribute8 = sai_thrift_attribute_t(id=SAI_ROUTER_INTERFACE_ATTR_NAT_ZONE_ID,
                                              value=rif_attribute8_value)
      rif_attr_list.append(rif_attribute8)
    rif_id = client.sai_thrift_create_router_interface(rif_attr_list)

    return rif_id

def sai_thrift_create_route(client, vr_id, addr_family, ip_addr, ip_mask, nhop, packet_action=None):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        mask = sai_thrift_ip_t(ip4=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr, mask=mask)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        mask = sai_thrift_ip_t(ip6=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr, mask=mask)
    route_attribute1_value = sai_thrift_attribute_value_t(oid=nhop)
    route_attribute1 = sai_thrift_attribute_t(id=SAI_ROUTE_ENTRY_ATTR_NEXT_HOP_ID,
                                              value=route_attribute1_value)

    route = sai_thrift_route_entry_t(vr_id, ip_prefix)
    route_attr_list = [route_attribute1]

    if packet_action != None:
        route_packet_action_value = sai_thrift_attribute_value_t(s32=packet_action)
        route_packet_action_attr = sai_thrift_attribute_t(id=SAI_ROUTE_ENTRY_ATTR_PACKET_ACTION,
                                                          value=route_packet_action_value)
        route_attr_list.append(route_packet_action_attr)

    client.sai_thrift_create_route(thrift_route_entry=route, thrift_attr_list=route_attr_list)
    return

def sai_thrift_remove_route(client, vr_id, addr_family, ip_addr, ip_mask, nhop):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        mask = sai_thrift_ip_t(ip4=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr, mask=mask)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        mask = sai_thrift_ip_t(ip6=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr, mask=mask)
    route = sai_thrift_route_entry_t(vr_id, ip_prefix)
    client.sai_thrift_remove_route(thrift_route_entry=route)

def sai_thrift_create_nhop(client, addr_family, ip_addr, rif_id, is_tunnel=False, vni=0, mac=None):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr)
    nhop_attribute1_value = sai_thrift_attribute_value_t(ipaddr=ipaddr)
    nhop_attribute1 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_ATTR_IP,
                                             value=nhop_attribute1_value)

    if is_tunnel:#nhop for tunnel
        nhop_attribute2_value = sai_thrift_attribute_value_t(oid=rif_id)
        nhop_attribute2 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_ATTR_TUNNEL_ID,
                                                 value=nhop_attribute2_value)
        nhop_attribute3_value = sai_thrift_attribute_value_t(s32=SAI_NEXT_HOP_TYPE_TUNNEL_ENCAP)
        nhop_attribute3 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_ATTR_TYPE,
                                                 value=nhop_attribute3_value)
    else:
        nhop_attribute2_value = sai_thrift_attribute_value_t(oid=rif_id)
        nhop_attribute2 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_ATTR_ROUTER_INTERFACE_ID,
                                                 value=nhop_attribute2_value)
        nhop_attribute3_value = sai_thrift_attribute_value_t(s32=SAI_NEXT_HOP_TYPE_IP)
        nhop_attribute3 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_ATTR_TYPE,
                                                 value=nhop_attribute3_value)

    nhop_attr_list = [nhop_attribute1, nhop_attribute2, nhop_attribute3]
    if vni != 0:
        attr_value = sai_thrift_attribute_value_t(u32=vni)
        attr = sai_thrift_attribute_t(id=SAI_NEXT_HOP_ATTR_TUNNEL_VNI, value=attr_value)
        nhop_attr_list.append(attr)

    if mac is not None:
        attr_value = sai_thrift_attribute_value_t(mac=mac)
        attr = sai_thrift_attribute_t(id=SAI_NEXT_HOP_ATTR_TUNNEL_MAC, value=attr_value)
        nhop_attr_list.append(attr)

    nhop = client.sai_thrift_create_next_hop(thrift_attr_list=nhop_attr_list)
    return nhop

def sai_thrift_remove_nhop(client, nhop_list):
    for nhop in nhop_list:
        client.sai_thrift_remove_next_hop(nhop)

def sai_thrift_create_neighbor(client, addr_family, rif_id, ip_addr, dmac):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr)
    neighbor_attribute1_value = sai_thrift_attribute_value_t(mac=dmac)
    neighbor_attribute1 = sai_thrift_attribute_t(id=SAI_NEIGHBOR_ENTRY_ATTR_DST_MAC_ADDRESS,
                                                 value=neighbor_attribute1_value)
    neighbor_attr_list = [neighbor_attribute1]
    neighbor_entry = sai_thrift_neighbor_entry_t(rif_id=rif_id, ip_address=ipaddr)
    return client.sai_thrift_create_neighbor_entry(neighbor_entry, neighbor_attr_list)

def sai_thrift_remove_neighbor(client, addr_family, rif_id, ip_addr, dmac):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr)
    neighbor_entry = sai_thrift_neighbor_entry_t(rif_id=rif_id, ip_address=ipaddr)
    client.sai_thrift_remove_neighbor_entry(neighbor_entry)

def sai_thrift_create_next_hop_group(client, type=SAI_NEXT_HOP_GROUP_TYPE_ECMP,
                                     config_size=0):
    nhop_group_atr1_value = sai_thrift_attribute_value_t(s32 = type)
    nhop_group_atr1 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_GROUP_ATTR_TYPE,
                                             value=nhop_group_atr1_value)
    nhop_group_attr_list = [nhop_group_atr1]
    if(config_size != 0):
      nhop_group_config_size = sai_thrift_attribute_value_t(s32 = config_size)
      nhop_group_atr2 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_GROUP_ATTR_CONFIGURED_SIZE,
                                             value=nhop_group_config_size)
      nhop_group_attr_list.append(nhop_group_atr2)

    return client.sai_thrift_create_next_hop_group(nhop_group_attr_list)

def sai_thrift_remove_next_hop_group(client, nhop_group_list):
    for nhop_group in nhop_group_list:
        client.sai_thrift_remove_next_hop_group(nhop_group)

def sai_thrift_get_next_hop_group_attr(client, nhop_group_id, attr_name):
    data = client.sai_thrift_get_next_hop_group_attr(nhop_group_id)
    return data.value.u32

def sai_thrift_create_next_hop_group_member(client, nhop_group, nhop, weight=None, index=0):
    nhop_gmember_atr1_value = sai_thrift_attribute_value_t(oid=nhop_group)
    nhop_gmember_atr1 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_GROUP_ID,
                                               value=nhop_gmember_atr1_value)
    nhop_gmember_atr2_value = sai_thrift_attribute_value_t(oid=nhop)
    nhop_gmember_atr2 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_ID,
                                               value=nhop_gmember_atr2_value)
    nhop_gmember_atr3_value = sai_thrift_attribute_value_t(u32=index)
    nhop_gmember_atr3 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_INDEX,
                                               value=nhop_gmember_atr3_value)
    if weight != None:
        nhop_gmember_atr4_value = sai_thrift_attribute_value_t(u32=weight)
        nhop_gmember_atr4 = sai_thrift_attribute_t(id=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_WEIGHT,
                                                   value=nhop_gmember_atr4_value)
        nhop_gmember_attr_list = [nhop_gmember_atr1, nhop_gmember_atr2, nhop_gmember_atr3, nhop_gmember_atr4]
    else:
        nhop_gmember_attr_list = [nhop_gmember_atr1, nhop_gmember_atr2, nhop_gmember_atr3]
    return client.sai_thrift_create_next_hop_group_member(nhop_gmember_attr_list)

def sai_thrift_remove_next_hop_group_member(client, nhop_gmember_list):
    for nhop_gmember in nhop_gmember_list:
        client.sai_thrift_remove_next_hop_group_member(nhop_gmember)

def sai_thrift_set_next_hop_group_member_attribute(client, nhop_gmem_id, nexthop_handle):
    nhop_gmember_attr_value = sai_thrift_attribute_value_t(oid=nexthop_handle)
    nhop_gmember_attr = sai_thrift_attribute_t(id=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_ID,
                                               value=nhop_gmember_attr_value)
    return client.sai_thrift_set_next_hop_group_member_attr(nhop_gmem_id, nhop_gmember_attr)

def sai_thrift_get_next_hop_group_member_attribute(client, nhop_gmem_id, attr_name):
    nhop_gmember_attr_value = sai_thrift_attribute_value_t(oid=0)
    nhop_gmember_attr = sai_thrift_attribute_t(id=attr_name,
                                               value=nhop_gmember_attr_value)
    return client.sai_thrift_get_next_hop_group_member_attr(nhop_gmem_id, nhop_gmember_attr)

def sai_thrift_remove_next_hop_from_group(client, nhop_list):
    for hnop in nhop_list:
        client.sai_thrift_remove_next_hop_from_group(hnop)

def sai_thrift_create_lag(client, port_list, is_bridged=True):
    lag = client.sai_thrift_create_lag([])

    if is_bridged:
        sai_thrift_create_bridge_port(client, SAI_BRIDGE_PORT_TYPE_PORT, lag)

    return lag

def sai_thrift_remove_lag(client, lag_oid):
    bport_oid = sai_thrift_get_bridge_port_by_port(client, lag_oid)

    if bport_oid != SAI_NULL_OBJECT_ID:
        status = client.sai_thrift_remove_bridge_port(bport_oid)
        assert (status == SAI_STATUS_SUCCESS)

    status = client.sai_thrift_remove_lag(lag_oid)
    assert (status == SAI_STATUS_SUCCESS)

def sai_thrift_create_lag_member(client, lag_id, port_id):
    bport_oid = sai_thrift_get_bridge_port_by_port(client, port_id)
    assert (bport_oid != SAI_NULL_OBJECT_ID)

    status = client.sai_thrift_remove_bridge_port(bport_oid)
    assert (status == SAI_STATUS_SUCCESS)

    lag_member_attr1_value = sai_thrift_attribute_value_t(oid=lag_id)
    lag_member_attr1 = sai_thrift_attribute_t(id=SAI_LAG_MEMBER_ATTR_LAG_ID,
                                              value=lag_member_attr1_value)
    lag_member_attr2_value = sai_thrift_attribute_value_t(oid=port_id)
    lag_member_attr2 = sai_thrift_attribute_t(id=SAI_LAG_MEMBER_ATTR_PORT_ID,
                                              value=lag_member_attr2_value)
    lag_member_attr_list = [lag_member_attr1, lag_member_attr2]
    lag_member_id = client.sai_thrift_create_lag_member(lag_member_attr_list)
    return lag_member_id

def sai_thrift_remove_lag_member(client, lag_member_oid):
    attrs = client.sai_thrift_get_lag_member_attribute(lag_member_oid)
    port_id = SAI_NULL_OBJECT_ID

    for a in attrs.attr_list:
        if a.id == SAI_LAG_MEMBER_ATTR_PORT_ID:
	   port_id = a.value.oid
           break

    assert (port_id != SAI_NULL_OBJECT_ID)

    status = client.sai_thrift_remove_lag_member(lag_member_oid)
    assert (status == SAI_STATUS_SUCCESS)

    sai_thrift_create_bridge_port(client, SAI_BRIDGE_PORT_TYPE_PORT, port_id)

def sai_thrift_create_stp_entry(client, vlan_list):
    vlanlist=sai_thrift_vlan_list_t(vlan_count=len(vlan_list), vlan_list=vlan_list)
    stp_attribute1_value = sai_thrift_attribute_value_t(vlanlist=vlanlist)
    stp_attribute1 = sai_thrift_attribute_t(id=SAI_STP_ATTR_VLAN_LIST,
                                            value=stp_attribute1_value)
    stp_attr_list = [stp_attribute1]
    stp_id = client.sai_thrift_create_stp_entry(stp_attr_list)
    return stp_id


def sai_thrift_create_hash(client, hash_attr_list):
    hash_list = sai_thrift_s32_list_t (count=len(hash_attr_list), s32list=hash_attr_list)
    hash_attribute_value = sai_thrift_attribute_value_t(s32list=hash_list)
    hash_attribute = sai_thrift_attribute_t (id=SAI_HASH_ATTR_NATIVE_HASH_FIELD_LIST,
                                             value=hash_attribute_value)
    param_list = [hash_attribute]
    hash_id = client.sai_thrift_create_hash(param_list)
    return hash_id

def sai_thrift_update_hash_attribute(client, hash_id, hash_attr_list):
    hash_list = sai_thrift_s32_list_t (count=len(hash_attr_list), s32list=hash_attr_list)
    hash_attribute_value = sai_thrift_attribute_value_t(s32list=hash_list)
    hash_attribute = sai_thrift_attribute_t (id=SAI_HASH_ATTR_NATIVE_HASH_FIELD_LIST,
                                             value=hash_attribute_value)
    return client.sai_thrift_set_hash_attribute(hash_id, hash_attribute)

def sai_thrift_get_hash_attribute(client, hash_id):
    hash_attr_list = client.sai_thrift_get_hash_attribute(hash_id)
    return hash_attr_list

def sai_thrift_remove_hash(client, hash_id):
    return client.sai_thrift_remove_hash(hash_id)

def sai_thrift_create_hostif(client,
                             hif_type,
                             hif_obj_id,
                             hif_name):
    attr_list=[]

    atr_value=sai_thrift_attribute_value_t(s32=hif_type)
    atr=sai_thrift_attribute_t(id=SAI_HOSTIF_ATTR_TYPE,
                               value=atr_value)
    attr_list.append(atr)

    atr_value=sai_thrift_attribute_value_t(oid=hif_obj_id)
    atr=sai_thrift_attribute_t(id=SAI_HOSTIF_ATTR_OBJ_ID,
                               value=atr_value)
    attr_list.append(atr)

    atr_value=sai_thrift_attribute_value_t(chardata=hif_name)
    atr=sai_thrift_attribute_t(id=SAI_HOSTIF_ATTR_NAME,
                               value=atr_value)
    attr_list.append(atr)

    atr_value=sai_thrift_attribute_value_t(booldata=True)
    atr=sai_thrift_attribute_t(id=SAI_HOSTIF_ATTR_OPER_STATUS,
                               value=atr_value)

    attr_list.append(atr)

    hostif_id = client.sai_thrift_create_hostif(attr_list)
    os.system("sudo ifconfig %s up" %hif_name)
    return hostif_id

def sai_thrift_create_hostif_trap(client,
                                  trap_type,
                                  packet_action,
                                  trap_priority=None,
                                  exclude_port_list=None,
                                  trap_group=None):
    attr_list=[]

    atr_value=sai_thrift_attribute_value_t(s32=trap_type)
    atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_TRAP_TYPE,
                               value=atr_value)
    attr_list.append(atr)

    atr_value=sai_thrift_attribute_value_t(s32=packet_action)
    atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_PACKET_ACTION,
                               value=atr_value)
    attr_list.append(atr)

    if trap_priority != None:
        atr_value=sai_thrift_attribute_value_t(u32=trap_priority)
        atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_TRAP_PRIORITY,
                                   value=atr_value)
        attr_list.append(atr)

    if trap_priority != None:
        atr_value=sai_thrift_attribute_value_t(objlist=exclude_port_list)
        atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_EXCLUDE_PORT_LIST,
                                   value=atr_value)
        attr_list.append(atr)

    if trap_group != None:
        atr_value=sai_thrift_attribute_value_t(oid=trap_group)
        atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_TRAP_GROUP,
                                   value=atr_value)
        attr_list.append(atr)

    trap_id = client.sai_thrift_create_hostif_trap(attr_list)
    return trap_id

def sai_thrift_remove_hostif_trap(client,
                                  trap_id):
    client.sai_thrift_remove_hostif_trap(trap_id)

def sai_thrift_set_hostif_trap_attribute(client,
                                         trap_type,
                                         packet_action,
                                         trap_priority=None,
                                         exclude_port_list=None,
                                         trap_group=None):
    attr_list=[]

    atr_value=sai_thrift_attribute_value_t(s32=trap_type)
    atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_TRAP_TYPE,
                               value=atr_value)
    attr_list.append(atr)

    atr_value=sai_thrift_attribute_value_t(s32=packet_action)
    atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_PACKET_ACTION,
                               value=atr_value)
    attr_list.append(atr)

    if trap_priority != None:
        atr_value=sai_thrift_attribute_value_t(u32=trap_priority)
        atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_TRAP_PRIORITY,
                                   value=atr_value)
        attr_list.append(atr)

    if exclude_port_list != None:
        atr_value=sai_thrift_attribute_value_t(objlist=exclude_port_list)
        atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_EXCLUDE_PORT_LIST,
                                   value=atr_value)
        attr_list.append(atr)

    if trap_group != None:
        atr_value=sai_thrift_attribute_value_t(oid=trap_group)
        atr=sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_ATTR_TRAP_GROUP,
                                   value=atr_value)
        attr_list.append(atr)

    client.sai_thrift_set_hostif_trap_attribute(attr_list)

def sai_thrift_create_hostif_trap_group(client, queue_id, policer_id=None):
    attr_list = []
    attribute_value = sai_thrift_attribute_value_t(u32=queue_id)
    attribute = sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_GROUP_ATTR_QUEUE, value=attribute_value)
    attr_list.append(attribute)

    if policer_id != None:
        policer_attr_value = sai_thrift_attribute_value_t(oid=policer_id)
        policer_attr = sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_GROUP_ATTR_POLICER, value=policer_attr_value)
        attr_list.append(policer_attr)

    trap_group = client.sai_thrift_create_hostif_trap_group(thrift_attr_list=attr_list)
    return trap_group

def sai_thrift_set_hostif_trap_group_attribute(client, trap_group ,policer_id=None):

    if policer_id != None:
        policer_attr_value = sai_thrift_attribute_value_t(oid=policer_id)
        policer_attr = sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_GROUP_ATTR_POLICER, value=policer_attr_value)

    return client.sai_thrift_set_hostif_trap_group_attribute(trap_group, policer_attr)

def sai_thrift_remove_hostif_trap_group(client,
                                        trap_group):
    client.sai_thrift_remove_hostif_trap_group(trap_group)

def sai_thrift_set_hostif_trap_group(client, trap_group_id, policer_id):
    policer_attr_value = sai_thrift_attribute_value_t(oid=policer_id)
    policer_attr = sai_thrift_attribute_t(id=SAI_HOSTIF_TRAP_GROUP_ATTR_POLICER, value=policer_attr_value)
    status = client.sai_thrift_set_hostif_trap_group(trap_group_id, thrift_attr=policer_attr)
    return status

def sai_thrift_create_policer(client,
                              meter_type,
                              mode,
                              cir,
                              cbs,
                              pir,
                              pbs,
                              red_action):
    attr_list = []

    meter_attr_value = sai_thrift_attribute_value_t(s32=meter_type)
    meter_attr = sai_thrift_attribute_t(id=SAI_POLICER_ATTR_METER_TYPE, value=meter_attr_value)

    mode_attr_value = sai_thrift_attribute_value_t(s32=mode)
    mode_attr = sai_thrift_attribute_t(id=SAI_POLICER_ATTR_MODE, value=mode_attr_value)

    cir_attr_value = sai_thrift_attribute_value_t(u64=cir)
    cir_attr = sai_thrift_attribute_t(id=SAI_POLICER_ATTR_CIR, value=cir_attr_value)

    cbs_attr_value = sai_thrift_attribute_value_t(u64=cbs)
    cbs_attr = sai_thrift_attribute_t(id=SAI_POLICER_ATTR_CBS, value=cbs_attr_value)

    pir_attr_value = sai_thrift_attribute_value_t(u64=pir)
    pir_attr = sai_thrift_attribute_t(id=SAI_POLICER_ATTR_PIR, value=pir_attr_value)

    pbs_attr_value = sai_thrift_attribute_value_t(u64=pbs)
    pbs_attr = sai_thrift_attribute_t(id=SAI_POLICER_ATTR_PBS, value=pbs_attr_value)

    red_action_attr_val = sai_thrift_attribute_value_t(s32=red_action)
    red_action_attr = sai_thrift_attribute_t(id=SAI_POLICER_ATTR_RED_PACKET_ACTION, value=red_action_attr_val)

    attr_list.append(meter_attr)
    attr_list.append(mode_attr)
    attr_list.append(cir_attr)
    attr_list.append(cbs_attr)
    attr_list.append(pir_attr)
    attr_list.append(pbs_attr)
    attr_list.append(red_action_attr)
    policer_id = client.sai_thrift_create_policer(attr_list)

    return policer_id

def sai_thrift_set_policer_attribute(client, policer_id, id, value):

    if (id == SAI_POLICER_ATTR_CIR or
        id == SAI_POLICER_ATTR_CBS or
        id == SAI_POLICER_ATTR_PIR or
        id == SAI_POLICER_ATTR_PBS):
        attribute_value = sai_thrift_attribute_value_t(u64=value)
        attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
        client.sai_thrift_set_policer_attribute(policer_id, attribute)

def sai_thrift_create_acl_table(client,
                                table_stage=SAI_ACL_STAGE_INGRESS,
                                table_bind_point_list=None,
                                addr_family=SAI_IP_ADDR_FAMILY_IPV4,
                                mac_src=None, mac_dst=None,
                                ip_src=None, ip_dst=None,
                                ip_proto=None,
                                in_ports=None, out_ports=None,
                                in_port=None, out_port=None,
                                src_l4_port=None,
                                dst_l4_port=None,
                                range_list=None,
                                action = None,
                                dscp = None,
                                ip_frag = None, user_metadata = None):

    acl_attr_list = []

    if table_stage != None:
        attribute_value = sai_thrift_attribute_value_t(s32=table_stage)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_ACL_STAGE,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if table_bind_point_list != None:
        acl_table_bind_point_list = sai_thrift_s32_list_t(count=len(table_bind_point_list), s32list=table_bind_point_list)
        attribute_value = sai_thrift_attribute_value_t(s32list=acl_table_bind_point_list)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_ACL_BIND_POINT_TYPE_LIST,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if mac_src != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_SRC_MAC,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if mac_dst != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_DST_MAC,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if ip_src != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
            attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_SRC_IP,
                                               value=attribute_value)
        else:
            attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_SRC_IPV6,
                                               value=attribute_value)
        acl_attr_list.append(attribute)

    if ip_dst != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
            attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_DST_IP,
                                               value=attribute_value)
        else:
            attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_DST_IPV6,
                                               value=attribute_value)
        acl_attr_list.append(attribute)

    if ip_proto != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_IP_PROTOCOL,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if in_ports:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_IN_PORTS,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if out_ports:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_OUT_PORTS,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if in_port != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_IN_PORT,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if out_port != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_OUT_PORT,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if src_l4_port != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_L4_SRC_PORT,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if dst_l4_port != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_L4_DST_PORT,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if dscp != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_DSCP,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if ip_frag != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_ACL_IP_FRAG,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if range_list != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_ACL_RANGE_TYPE,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if user_metadata != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_ACL_USER_META,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if action != None:
        action_list = [action]
        s32list = sai_thrift_s32_list_t(s32list=action_list, count=1)
        attribute_value = sai_thrift_attribute_value_t(s32list=s32list)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_ACL_ACTION_TYPE_LIST,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    acl_table_id = client.sai_thrift_create_acl_table(acl_attr_list)
    return acl_table_id

def sai_thrift_create_acl_entry(client,
                                acl_table_id,
                                entry_priority,
                                action=None, addr_family=SAI_IP_ADDR_FAMILY_IPV4,
                                mac_src=None, mac_src_mask=None,
                                mac_dst=None, mac_dst_mask=None,
                                ip_src=None, ip_src_mask=None,
                                ip_dst=None, ip_dst_mask=None,
                                ip_proto=None,
                                in_port_list=None, out_port_list=None,
                                in_port=None, out_port=None,
                                src_l4_port=None, dst_l4_port=None,
                                ingress_mirror=None, egress_mirror=None,
                                range_list=None, dscp=None, acl_counter_id=None,
                                tcp_flags=None, ether_type=None, ip_frag=None,
                                dtel_drop_report_enable=None,
                                dtel_postcard_enable=None,
                                dtel_tail_drop_report_enable=None, dtel_report_all_packets=None,
                                ingress_samplepacket=None,
                                user_metadata=None, user_metadata_mask=None,
                                set_user_metadata=None, redirect_oid=None,
                                policer=None, tc=None,
                                ipv6_next_header=None):
    acl_attr_list = []

    #ACL table OID
    attribute_value = sai_thrift_attribute_value_t(oid=acl_table_id)
    attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_TABLE_ID,
                                       value=attribute_value)
    acl_attr_list.append(attribute)

    #Priority
    if entry_priority != None:
        attribute_value = sai_thrift_attribute_value_t(u32=entry_priority)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_PRIORITY,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #MAC source
    if mac_src != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(mac=mac_src), mask = sai_thrift_acl_mask_t(mac=mac_src_mask)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_SRC_MAC,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #MAC destination
    if mac_dst != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(mac=mac_dst), mask = sai_thrift_acl_mask_t(mac=mac_dst_mask)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_DST_MAC,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #Ether type
    if ether_type != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(u16=ether_type),
                                                                                            mask = sai_thrift_acl_mask_t(u16=U16MASKFULL)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_ETHER_TYPE,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #Ip source
    if ip_src != None:
        if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
	    attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(ip4=ip_src), mask =sai_thrift_acl_mask_t(ip4=ip_src_mask)))
	    attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_SRC_IP,
					       value=attribute_value)
        else:
	    attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(ip6=ip_src), mask =sai_thrift_acl_mask_t(ip6=ip_src_mask)))
	    attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_SRC_IPV6,
					       value=attribute_value)
        acl_attr_list.append(attribute)

    #Ip destination
    if ip_dst != None:
        if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
	    attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(ip4=ip_dst), mask =sai_thrift_acl_mask_t(ip4=ip_dst_mask)))
	    attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_DST_IP,
					       value=attribute_value)
        else:
	    attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(ip6=ip_dst), mask =sai_thrift_acl_mask_t(ip6=ip_dst_mask)))
	    attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_DST_IPV6,
					       value=attribute_value)
        acl_attr_list.append(attribute)

    #tcp flags
    if tcp_flags != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(u8=tcp_flags),
                                                                                            mask = sai_thrift_acl_mask_t(u8=U16MASKFULL)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_TCP_FLAGS,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #IP protocol
    if ip_proto != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(u8=ip_proto),
                                                                                            mask = sai_thrift_acl_mask_t(u8=U16MASKFULL)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_IP_PROTOCOL,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #IPv6 next header
    if ipv6_next_header != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(u8=ipv6_next_header),
                                                                                            mask = sai_thrift_acl_mask_t(u8=U16MASKFULL)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_IPV6_NEXT_HEADER,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #Input ports
    if in_port_list != None and len(in_port_list) == 1:
        acl_port_list = sai_thrift_object_list_t(count=len(in_port_list), object_id_list=in_port_list)
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(objlist=acl_port_list)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_IN_PORTS,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #Output ports
    if out_port_list:
        acl_port_list = sai_thrift_object_list_t(count=len(out_port_list), object_id_list=out_port_list)
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(objlist=acl_port_list)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_OUT_PORTS,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #Input port
    if in_port != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(oid=in_port)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_IN_PORT,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #Output port
    if out_port != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(oid=out_port)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_OUT_PORT,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #L4 Source port
    if src_l4_port != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(u16=src_l4_port),
                                                                                            mask = sai_thrift_acl_mask_t(u16=U16MASKFULL)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_L4_SRC_PORT,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #L4 Destination port
    if dst_l4_port != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(u16=dst_l4_port),
                                                                                            mask = sai_thrift_acl_mask_t(u16=U16MASKFULL)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_L4_DST_PORT,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #IP frag
    if ip_frag != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(s32=ip_frag)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_ACL_IP_FRAG,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if dscp != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(u8=dscp),
                                                                                            mask = sai_thrift_acl_mask_t(u8=U16MASKFULL)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_DSCP,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #range list
    if range_list != None:
        acl_range_list = sai_thrift_object_list_t(count=len(range_list), object_id_list=range_list)
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(objlist=acl_range_list)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_ACL_RANGE_TYPE,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #User Metadata
    if user_metadata != None:
        attribute_value = sai_thrift_attribute_value_t(aclfield=sai_thrift_acl_field_data_t(data = sai_thrift_acl_data_t(u32=user_metadata), mask =sai_thrift_acl_mask_t(u32=user_metadata_mask)))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_FIELD_ACL_USER_META,
                                           value=attribute_value)
        acl_attr_list.append(attribute)


    # action
    if redirect_oid != None:
        acl_action_data = sai_thrift_acl_action_data_t(parameter=sai_thrift_acl_parameter_t(oid=redirect_oid), enable =True)
        attribute_value =  sai_thrift_attribute_value_t(aclaction=acl_action_data)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_REDIRECT, value=attribute_value)
        acl_attr_list.append(attribute)
    elif action != None:
        attribute_value = sai_thrift_attribute_value_t(aclaction=sai_thrift_acl_action_data_t(parameter = sai_thrift_acl_data_t(s32=action),
                                                                                              enable = True))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_PACKET_ACTION,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    #Ingress mirroring
    if ingress_mirror != None:
        attribute_value = sai_thrift_attribute_value_t(aclaction=sai_thrift_acl_action_data_t(parameter = sai_thrift_acl_parameter_t(oid=ingress_mirror), enable=True))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_MIRROR_INGRESS, value=attribute_value)
        acl_attr_list.append(attribute)

    #Egress mirroring
    if egress_mirror != None:
        attribute_value = sai_thrift_attribute_value_t(aclaction=sai_thrift_acl_action_data_t(parameter = sai_thrift_acl_parameter_t(oid=egress_mirror), enable=True))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_MIRROR_EGRESS, value=attribute_value)
        acl_attr_list.append(attribute)

    if acl_counter_id != None:
        attribute_value = sai_thrift_attribute_value_t(aclaction=sai_thrift_acl_action_data_t(parameter = sai_thrift_acl_parameter_t(oid=acl_counter_id), enable=True))
        print "Acl counter handle 0x%lx"%(acl_counter_id)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_COUNTER, value=attribute_value)
        acl_attr_list.append(attribute)


    if set_user_metadata != None:
        attribute_value = sai_thrift_attribute_value_t(aclaction=sai_thrift_acl_action_data_t(parameter = sai_thrift_acl_parameter_t(u32=set_user_metadata), enable=True))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_SET_ACL_META_DATA, value=attribute_value)
        acl_attr_list.append(attribute)

    # Set policer
    if policer != None:
        attribute_value = sai_thrift_attribute_value_t(aclaction=sai_thrift_acl_action_data_t(parameter = sai_thrift_acl_parameter_t(oid=policer), enable=True))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_SET_POLICER, value=attribute_value)
        acl_attr_list.append(attribute)

    # Set TC
    if tc != None:
        attribute_value = sai_thrift_attribute_value_t(aclaction=sai_thrift_acl_action_data_t(parameter = sai_thrift_acl_parameter_t(u8=tc), enable=True))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_SET_TC, value=attribute_value)
        acl_attr_list.append(attribute)

    # Dtel drop report enable
    if dtel_drop_report_enable != None:
        action = sai_thrift_acl_action_data_t(enable=True,
            parameter=sai_thrift_acl_parameter_t(
                booldata=dtel_drop_report_enable))
        attr = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_ACTION_DTEL_DROP_REPORT_ENABLE,
            value=sai_thrift_attribute_value_t(aclaction=action))
        acl_attr_list.append(attr)

    # Dtel postcard enable
    if dtel_postcard_enable != None :
        action = sai_thrift_acl_action_data_t(enable=dtel_postcard_enable,
            parameter=sai_thrift_acl_parameter_t(s32=SAI_ACL_DTEL_FLOW_OP_POSTCARD))
        attr = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_ACTION_ACL_DTEL_FLOW_OP,
            value=sai_thrift_attribute_value_t(aclaction=action))
        acl_attr_list.append(attr)

    # Dtel tail drop report enable
    if dtel_tail_drop_report_enable != None:
        action = sai_thrift_acl_action_data_t(enable=True,
            parameter=sai_thrift_acl_parameter_t(
                booldata=dtel_tail_drop_report_enable))
        attr = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_ACTION_DTEL_TAIL_DROP_REPORT_ENABLE,
            value=sai_thrift_attribute_value_t(aclaction=action))
        acl_attr_list.append(attr)

    # Dtel report all packets
    if dtel_report_all_packets != None:
        action = sai_thrift_acl_action_data_t(enable=True,
            parameter=sai_thrift_acl_parameter_t(
                booldata=dtel_report_all_packets))
        attr = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_ACTION_DTEL_REPORT_ALL_PACKETS,
            value=sai_thrift_attribute_value_t(aclaction=action))
        acl_attr_list.append(attr)

    #Ingress sample packet
    if ingress_samplepacket != None:
        attribute_value = sai_thrift_attribute_value_t(aclaction=sai_thrift_acl_action_data_t(parameter = sai_thrift_acl_parameter_t(oid=ingress_samplepacket), enable=True))
        attribute = sai_thrift_attribute_t(id=SAI_ACL_ENTRY_ATTR_ACTION_INGRESS_SAMPLEPACKET_ENABLE, value=attribute_value)
        acl_attr_list.append(attribute)

    acl_entry_id = client.sai_thrift_create_acl_entry(acl_attr_list)

    return acl_entry_id

def sai_thrift_create_acl_counter(client,
                                  acl_table_id,
                                  packet_enable=True,
                                  byte_enable=True):
    attr_list = []

    attribute1_value = sai_thrift_attribute_value_t(oid=acl_table_id)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_ACL_COUNTER_ATTR_TABLE_ID, value=attribute1_value)
    attr_list.append(attribute1)

    attribute2_value = sai_thrift_attribute_value_t(booldata=True)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_ACL_COUNTER_ATTR_ENABLE_PACKET_COUNT, value=attribute2_value)
    attr_list.append(attribute2)

    attribute3_value = sai_thrift_attribute_value_t(booldata=True)
    attribute3 = sai_thrift_attribute_t(
        id=SAI_ACL_COUNTER_ATTR_ENABLE_BYTE_COUNT, value=attribute3_value)
    attr_list.append(attribute3)

    acl_counter_id = client.sai_thrift_create_acl_counter(attr_list)
    return acl_counter_id

def sai_thrift_get_acl_counter_attribute(client, acl_counter_id):
    attr_list = []

    attribute_id1 = SAI_ACL_COUNTER_ATTR_PACKETS
    attr_list.append(attribute_id1)

    attr_values = client.sai_thrift_get_acl_counter_attribute(acl_counter_id,
                                                              attr_list)
    return attr_values

def sai_thrift_create_acl_table_group(client,
                                      group_stage,
                                      group_bind_point_list,
                                      group_type):
    acl_attr_list = []

    if group_stage != None:
        attribute_value = sai_thrift_attribute_value_t(s32=group_stage)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_GROUP_ATTR_ACL_STAGE,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if group_bind_point_list != None:
        acl_group_bind_point_list = sai_thrift_s32_list_t(count=len(group_bind_point_list), s32list=group_bind_point_list)
        attribute_value = sai_thrift_attribute_value_t(s32list=acl_group_bind_point_list)

        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_GROUP_ATTR_ACL_BIND_POINT_TYPE_LIST,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if group_type != None:
        attribute_value = sai_thrift_attribute_value_t(s32=group_type)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_GROUP_ATTR_TYPE,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    acl_table_group_id = client.sai_thrift_create_acl_table_group(acl_attr_list)
    return acl_table_group_id

def sai_thrift_create_acl_table_group_member(client,
                                             acl_table_group_id,
                                             acl_table_id,
                                             group_member_priority):
    acl_attr_list = []

    if acl_table_group_id != None:
        attribute_value = sai_thrift_attribute_value_t(oid=acl_table_group_id)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_GROUP_MEMBER_ATTR_ACL_TABLE_GROUP_ID,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if acl_table_id != None:
        attribute_value = sai_thrift_attribute_value_t(oid=acl_table_id)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_GROUP_MEMBER_ATTR_ACL_TABLE_ID,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    if group_member_priority != None:
        attribute_value = sai_thrift_attribute_value_t(u32=group_member_priority)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_GROUP_MEMBER_ATTR_PRIORITY,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    acl_table_group_member_id = client.sai_thrift_create_acl_table_group_member(acl_attr_list)
    return acl_table_group_member_id

def sai_thrift_create_acl_range(client, range_type, range_value):
    attr_list = []
    attribute1_value = sai_thrift_attribute_value_t(s32=range_type)
    attribute1 = sai_thrift_attribute_t(id=SAI_ACL_RANGE_ATTR_TYPE, value=attribute1_value)
    attr_list.append(attribute1)

    attribute2_value = sai_thrift_attribute_value_t(u32range=range_value)
    attribute2 = sai_thrift_attribute_t(id=SAI_ACL_RANGE_ATTR_LIMIT, value=attribute2_value)
    attr_list.append(attribute2)

    acl_range_id = client.sai_thrift_create_acl_range(attr_list)
    return acl_range_id

def sai_thrift_create_mirror_session(client, mirror_type, port,
                                     vlan=None, vlan_priority=None, vlan_tpid=None, vlan_header_valid=False,
                                     src_mac=None, dst_mac=None,
                                     src_ip=None, dst_ip=None,
                                     encap_type=None, iphdr_version=None, ttl=None, tos=None, gre_type=None):
    mirror_attr_list = []

    #Mirror type
    attribute1_value = sai_thrift_attribute_value_t(s32=mirror_type)
    attribute1 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_TYPE,
                                        value=attribute1_value)
    mirror_attr_list.append(attribute1)

    #Monitor port
    attribute2_value = sai_thrift_attribute_value_t(oid=port)
    attribute2 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_MONITOR_PORT,
                                        value=attribute2_value)
    mirror_attr_list.append(attribute2)

    if mirror_type == SAI_MIRROR_SESSION_TYPE_REMOTE:
        #vlan
        attribute3_value = sai_thrift_attribute_value_t(u16=vlan)
        attribute3 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_ID,
                                            value=attribute3_value)
        mirror_attr_list.append(attribute3)

        #vlan tpid
        if vlan_tpid is not None:
            attribute4_value = sai_thrift_attribute_value_t(u32=vlan_tpid)
            attribute4 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_TPID,
                                               value=attribute4_value)
            mirror_attr_list.append(attribute4)

        #vlan priority
        attribute5_value = sai_thrift_attribute_value_t(u8=vlan_priority)
        attribute5 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_PRI,
                                            value=attribute5_value)
        mirror_attr_list.append(attribute5)
    elif mirror_type == SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE:
        #encap type
        attribute3_value = sai_thrift_attribute_value_t(s32=encap_type)
        attribute3 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_ERSPAN_ENCAPSULATION_TYPE,
                                            value=attribute3_value)
        mirror_attr_list.append(attribute3)

        #ip header version
        attribute4_value = sai_thrift_attribute_value_t(u8=iphdr_version)
        attribute4 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_IPHDR_VERSION,
                                            value=attribute4_value)
        mirror_attr_list.append(attribute4)

        assert((iphdr_version == 4) or (iphdr_version == 6))
        if iphdr_version == 4:
            addr_family = SAI_IP_ADDR_FAMILY_IPV4
        elif iphdr_version == 6:
            addr_family = SAI_IP_ADDR_FAMILY_IPV6

        #source ip
        addr = sai_thrift_ip_t(ip4=src_ip)
        src_ip_addr = sai_thrift_ip_address_t(addr_family=addr_family, addr=addr)
        attribute5_value = sai_thrift_attribute_value_t(ipaddr=src_ip_addr)
        attribute5 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_SRC_IP_ADDRESS,
                                            value=attribute5_value)
        mirror_attr_list.append(attribute5)

        #dst ip
        addr = sai_thrift_ip_t(ip4=dst_ip)
        dst_ip_addr = sai_thrift_ip_address_t(addr_family=addr_family, addr=addr)
        attribute6_value = sai_thrift_attribute_value_t(ipaddr=dst_ip_addr)
        attribute6 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_DST_IP_ADDRESS,
                                            value=attribute6_value)
        mirror_attr_list.append(attribute6)

        #source mac
        attribute7_value = sai_thrift_attribute_value_t(mac=src_mac)
        attribute7 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_SRC_MAC_ADDRESS,
                                            value=attribute7_value)
        mirror_attr_list.append(attribute7)

        #dst mac
        attribute8_value = sai_thrift_attribute_value_t(mac=dst_mac)
        attribute8 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_DST_MAC_ADDRESS,
                                            value=attribute8_value)
        mirror_attr_list.append(attribute8)

        attribute9_value = sai_thrift_attribute_value_t(u32=gre_type)
        attribute9 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_GRE_PROTOCOL_TYPE,value=attribute9_value)
        mirror_attr_list.append(attribute9)

        attribute10_value = sai_thrift_attribute_value_t(u16=ttl)
        attribute10 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_TTL,value=attribute10_value)
        mirror_attr_list.append(attribute10)

        if vlan_tpid is not None:
            attribute11_value = sai_thrift_attribute_value_t(u32=vlan_tpid)
            attribute11 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_TPID,
                                                value=attribute11_value)
            mirror_attr_list.append(attribute11)

        #vlan
        if vlan is not None:
            attribute12_value = sai_thrift_attribute_value_t(u16=vlan)
            attribute12 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_ID,
                                                value=attribute12_value)
            mirror_attr_list.append(attribute12)

        #tos
        attribute13_value = sai_thrift_attribute_value_t(u16=tos)
        attribute13 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_TOS,
                                            value=attribute13_value)
        mirror_attr_list.append(attribute13)

        attribute14_value = sai_thrift_attribute_value_t(booldata=vlan_header_valid)
        attribute14 = sai_thrift_attribute_t(id=SAI_MIRROR_SESSION_ATTR_VLAN_HEADER_VALID,
                                            value=attribute14_value)
        mirror_attr_list.append(attribute14)

    mirror_id = client.sai_thrift_create_mirror_session(mirror_attr_list)
    return mirror_id

def sai_thrift_create_scheduler_profile(client, prio, weight, max_rate, max_burst, min_rate, min_burst):
    if prio:
      attribute1_value = sai_thrift_attribute_value_t(u32=SAI_SCHEDULING_TYPE_STRICT)
    else:
      attribute1_value = sai_thrift_attribute_value_t(u32=SAI_SCHEDULING_TYPE_DWRR)

    attribute1 = sai_thrift_attribute_t(id=SAI_SCHEDULER_ATTR_SCHEDULING_TYPE, value = attribute1_value)

    attribute2_value = sai_thrift_attribute_value_t(u8 = weight)
    attribute2 = sai_thrift_attribute_t(id = SAI_SCHEDULER_ATTR_SCHEDULING_WEIGHT, value = attribute2_value)

    attribute3_value = sai_thrift_attribute_value_t(u32 = SAI_METER_TYPE_BYTES)
    attribute3 = sai_thrift_attribute_t(id = SAI_SCHEDULER_ATTR_METER_TYPE, value = attribute3_value)

    attribute4_value = sai_thrift_attribute_value_t(u64 = min_rate)
    attribute4 = sai_thrift_attribute_t(id = SAI_SCHEDULER_ATTR_MIN_BANDWIDTH_RATE, value = attribute4_value)

    attribute5_value = sai_thrift_attribute_value_t(u64 = min_burst)
    attribute5 = sai_thrift_attribute_t(id = SAI_SCHEDULER_ATTR_MIN_BANDWIDTH_BURST_RATE, value = attribute5_value)

    attribute6_value = sai_thrift_attribute_value_t(u64 = max_burst)
    attribute6 = sai_thrift_attribute_t(id = SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_BURST_RATE, value = attribute6_value)

    attribute7_value = sai_thrift_attribute_value_t(u64 = max_rate)
    attribute7 = sai_thrift_attribute_t(id = SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_RATE, value = attribute7_value)

    attr_list = [attribute1, attribute2, attribute3, attribute4, attribute5, attribute6, attribute7]
    return client.sai_thrift_create_scheduler_profile(attr_list)

def sai_thrift_create_scheduler_profile_max_rate(client, max_rate, algorithm=0):
    scheduler_attr_list = []
    attribute_value = sai_thrift_attribute_value_t(u64=max_rate)
    attribute = sai_thrift_attribute_t(id=SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_RATE ,
                                       value=attribute_value)
    scheduler_attr_list.append(attribute)
    attribute_value = sai_thrift_attribute_value_t(s32=algorithm)
    attribute = sai_thrift_attribute_t(id=SAI_SCHEDULER_ATTR_SCHEDULING_ALGORITHM ,
                                       value=attribute_value)
    scheduler_attr_list.append(attribute)
    scheduler_profile_id = client.sai_thrift_create_scheduler_profile(scheduler_attr_list)
    return scheduler_profile_id

def sai_thrift_create_buffer_profile(client, pool_id, size, threshold, xoff_th, xon_th, static=False):
    buffer_attr_list = []
    attribute_value = sai_thrift_attribute_value_t(oid=pool_id)
    attribute = sai_thrift_attribute_t(id=SAI_BUFFER_PROFILE_ATTR_POOL_ID ,
                                           value=attribute_value)
    buffer_attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(u32=size)
    attribute = sai_thrift_attribute_t(id=SAI_BUFFER_PROFILE_ATTR_BUFFER_SIZE ,
                                           value=attribute_value)
    buffer_attr_list.append(attribute)

    if static == False:
      attribute_value = sai_thrift_attribute_value_t(u8=threshold)
      attribute = sai_thrift_attribute_t(
          id=SAI_BUFFER_PROFILE_ATTR_SHARED_DYNAMIC_TH , value=attribute_value)
    else:
      attribute_value = sai_thrift_attribute_value_t(u64=threshold)
      attribute = sai_thrift_attribute_t(
          id=SAI_BUFFER_PROFILE_ATTR_SHARED_STATIC_TH , value=attribute_value)
    buffer_attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(u32=xoff_th)
    attribute = sai_thrift_attribute_t(id=SAI_BUFFER_PROFILE_ATTR_XOFF_TH ,
                                           value=attribute_value)
    buffer_attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(u32=xon_th)
    attribute = sai_thrift_attribute_t(id=SAI_BUFFER_PROFILE_ATTR_XON_TH ,
                                           value=attribute_value)
    buffer_attr_list.append(attribute)

    buffer_profile_id = client.sai_thrift_create_buffer_profile(buffer_attr_list)
    return buffer_profile_id

def sai_thrift_create_pool_profile(client, pool_type, size, threshold_mode):
    pool_attr_list = []
    attribute_value = sai_thrift_attribute_value_t(s32=pool_type)
    attribute = sai_thrift_attribute_t(id=SAI_BUFFER_POOL_ATTR_TYPE ,
                                           value=attribute_value)
    pool_attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(u32=size)
    attribute = sai_thrift_attribute_t(id=SAI_BUFFER_POOL_ATTR_SIZE ,
                                           value=attribute_value)
    pool_attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(s32=threshold_mode)
    attribute = sai_thrift_attribute_t(id=SAI_BUFFER_POOL_ATTR_THRESHOLD_MODE ,
                                           value=attribute_value)
    pool_attr_list.append(attribute)
    pool_id = client.sai_thrift_create_pool_profile(pool_attr_list)
    return pool_id

def sai_thrift_get_buffer_pool_attribute(client, pool_id, attr_id):
    thrift_attr = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_buffer_pool_attribute(pool_id, thrift_attr);

def sai_thrift_remove_buffer_profile(client, profile_id):
    return client.sai_thrift_remove_buffer_profile(profile_id)

def sai_thrift_remove_buffer_pool(client, pool_id):
    return client.sai_thrift_remove_buffer_pool(pool_id)

def sai_thrift_clear_all_counters(client):
    for port in sai_port_list:
        queue_list=[]
        client.sai_thrift_clear_port_all_stats(port)
        port_attr_list = client.sai_thrift_get_port_attribute(port)
        attr_list = port_attr_list.attr_list
        for attribute in attr_list:
            if attribute.id == SAI_PORT_ATTR_QOS_QUEUE_LIST:
                for queue_id in attribute.value.objlist.object_id_list:
                    queue_list.append(queue_id)

        cnt_ids=[]
        cnt_ids.append(SAI_QUEUE_STAT_PACKETS)
        for queue in queue_list:
            client.sai_thrift_clear_queue_stats(queue,cnt_ids,len(cnt_ids))

def sai_thrift_get_debug_counter_attribute(client, debug_counter_id, attr_id, reason_count):
    thrift_attr = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_debug_counter(debug_counter_id, reason_count, thrift_attr)

def sai_thrift_create_debug_counter(client, dc_reason):
    print("sai_thrift_create_debug_counter\n")

    dc_attr_list = []
    drop_reason_list = []
    dc_type = SAI_DEBUG_COUNTER_TYPE_PORT_IN_DROP_REASONS
    attribute1_value = sai_thrift_attribute_value_t(s32=dc_type)
    attribute1 = sai_thrift_attribute_t(id=SAI_DEBUG_COUNTER_ATTR_TYPE,
                value=attribute1_value)
    dc_attr_list.append(attribute1)

    dc_attr_list.append(attribute2)

    dc_id = client.sai_thrift_create_debug_counter(dc_attr_list)
    return dc_id


def sai_thrift_read_port_counters(client, port):
    port_cnt_ids=[]
    port_cnt_ids.append(SAI_PORT_STAT_IF_OUT_DISCARDS)
    port_cnt_ids.append(SAI_PORT_STAT_ETHER_STATS_DROP_EVENTS)
    port_cnt_ids.append(SAI_PORT_STAT_PFC_0_TX_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_PFC_1_TX_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_PFC_2_TX_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_PFC_3_TX_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_PFC_4_TX_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_PFC_5_TX_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_PFC_6_TX_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_PFC_7_TX_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_IF_OUT_OCTETS)
    port_cnt_ids.append(SAI_PORT_STAT_IP_IN_RECEIVES)
    port_cnt_ids.append(SAI_PORT_STAT_ETHER_STATS_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_IF_OUT_UCAST_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_IF_IN_UCAST_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_IF_OUT_UCAST_PKTS)
    port_cnt_ids.append(SAI_PORT_STAT_IF_IN_UCAST_PKTS)
    counters_results=[]
    counters_results = client.sai_thrift_get_port_stats(port,port_cnt_ids,len(port_cnt_ids))
    queue_list=[]

    port_attr_list = client.sai_thrift_get_port_attribute(port)
    attr_list = port_attr_list.attr_list
    for attribute in attr_list:
        if attribute.id == SAI_PORT_ATTR_QOS_QUEUE_LIST:
            for queue_id in attribute.value.objlist.object_id_list:
                queue_list.append(queue_id)
    cnt_ids=[]
    thrift_results=[]
    queue_counters_results=[]
    cnt_ids.append(SAI_QUEUE_STAT_PACKETS)
    queue1=0
    for queue in queue_list:
        if queue1 <= 7:
            thrift_results=client.sai_thrift_get_queue_stats(queue,cnt_ids,len(cnt_ids))
            queue_counters_results.append(thrift_results[0])
            queue1+=1
    return (counters_results, queue_counters_results)

def sai_thrift_read_queue_counters(client, queue):
    cnt_ids=[]
    thrift_results=[]
    cnt_ids.append(SAI_QUEUE_STAT_PACKETS)
    cnt_ids.append(SAI_QUEUE_STAT_BYTES)
    thrift_results=client.sai_thrift_get_queue_stats(queue, cnt_ids, len(cnt_ids))
    return thrift_results

def sai_thrift_read_ppg_counters(client, ppg):
    cnt_ids=[]
    thrift_results=[]
    cnt_ids.append(SAI_INGRESS_PRIORITY_GROUP_STAT_PACKETS)
    cnt_ids.append(SAI_INGRESS_PRIORITY_GROUP_STAT_BYTES)
    thrift_results=client.sai_thrift_get_pg_stats(ppg, cnt_ids, len(cnt_ids))
    return thrift_results

def sai_thrift_read_rif_counters(client, rif):
    cnt_ids=[]
    thrift_results=[]
    cnt_ids.append(SAI_ROUTER_INTERFACE_STAT_IN_OCTETS)
    cnt_ids.append(SAI_ROUTER_INTERFACE_STAT_OUT_OCTETS)
    cnt_ids.append(SAI_ROUTER_INTERFACE_STAT_IN_PACKETS)
    cnt_ids.append(SAI_ROUTER_INTERFACE_STAT_OUT_PACKETS)
    thrift_results=client.sai_thrift_get_router_interface_stats(rif, cnt_ids, len(cnt_ids))
    return thrift_results

def sai_thrift_create_vlan(client, vlan_id):
    vlan_attr_list = []
    attribute_value = sai_thrift_attribute_value_t(u16=vlan_id)
    attribute = sai_thrift_attribute_t(id=SAI_VLAN_ATTR_VLAN_ID, value=attribute_value)
    vlan_attr_list.append(attribute)
    vlan_oid = client.sai_thrift_create_vlan(vlan_attr_list)
    return vlan_oid

def sai_thrift_create_vlan_member(client, vlan_oid, port_oid, tagging_mode):
    bport_oid = sai_thrift_get_bridge_port_by_port(client, port_oid)
    assert (bport_oid != SAI_NULL_OBJECT_ID)

    vlan_member_attr_list = []
    attribute_value = sai_thrift_attribute_value_t(oid=vlan_oid)
    attribute = sai_thrift_attribute_t(id=SAI_VLAN_MEMBER_ATTR_VLAN_ID,
                                           value=attribute_value)
    vlan_member_attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(oid=bport_oid)
    attribute = sai_thrift_attribute_t(id=SAI_VLAN_MEMBER_ATTR_BRIDGE_PORT_ID,
                                           value=attribute_value)
    vlan_member_attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(s32=tagging_mode)
    attribute = sai_thrift_attribute_t(id=SAI_VLAN_MEMBER_ATTR_VLAN_TAGGING_MODE,
                                           value=attribute_value)
    vlan_member_attr_list.append(attribute)
    vlan_member_id = client.sai_thrift_create_vlan_member(vlan_member_attr_list)
    return vlan_member_id

def sai_thrift_vlan_remove_all_ports(client, vlan_oid):
        vlan_members_list = []

        vlan_attr_list = client.sai_thrift_get_vlan_attribute(vlan_oid)
        attr_list = vlan_attr_list.attr_list
        for attribute in attr_list:
            if attribute.id == SAI_VLAN_ATTR_MEMBER_LIST:
                for vlan_member in attribute.value.objlist.object_id_list:
                    vlan_members_list.append(vlan_member)

        for vlan_member in vlan_members_list:
            client.sai_thrift_remove_vlan_member(vlan_member)

def sai_thrift_vlan_remove_ports(client, vlan_oid, ports):
    vlan_members_list = []

    vlan_attr_list = client.sai_thrift_get_vlan_attribute(vlan_oid)
    attr_list = vlan_attr_list.attr_list
    for attribute in attr_list:
        if attribute.id == SAI_VLAN_ATTR_MEMBER_LIST:
            for vlan_member in attribute.value.objlist.object_id_list:
                attrs = client.sai_thrift_get_vlan_member_attribute(vlan_member)
                for a in attrs.attr_list:
                    if a.id == SAI_VLAN_MEMBER_ATTR_BRIDGE_PORT_ID:
                        port = sai_thrift_get_port_by_bridge_port(client, a.value.oid)
                        if port in ports:
                            vlan_members_list.append(vlan_member)

    for vlan_member in vlan_members_list:
        client.sai_thrift_remove_vlan_member(vlan_member)

def sai_thrift_read_vlan_counters(client, vlan):
    cnt_ids=[]
    thrift_results=[]
    cnt_ids.append(SAI_VLAN_STAT_IN_OCTETS)
    cnt_ids.append(SAI_VLAN_STAT_OUT_OCTETS)
    cnt_ids.append(SAI_VLAN_STAT_IN_PACKETS)
    cnt_ids.append(SAI_VLAN_STAT_IN_UCAST_PKTS)
    cnt_ids.append(SAI_VLAN_STAT_OUT_PACKETS)
    cnt_ids.append(SAI_VLAN_STAT_OUT_UCAST_PKTS)
    thrift_results=client.sai_thrift_get_vlan_stats(vlan, cnt_ids, len(cnt_ids))
    return thrift_results

def sai_thrift_set_port_shaper(client, port_id, max_rate):
    sched_prof_id=sai_thrift_create_scheduler_profile_max_rate(client, max_rate)
    attr_value = sai_thrift_attribute_value_t(oid=sched_prof_id)
    attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_QOS_SCHEDULER_PROFILE_ID, value=attr_value)
    client.sai_thrift_set_port_attribute(port_id,attr)

def sai_thrift_create_default_bridge_ports(client, port_list):
    for port in port_list:
      sai_thrift_create_bridge_port(client,SAI_BRIDGE_PORT_TYPE_PORT, port)

def sai_thrift_remove_default_bridge_ports(client, port_list):
    for port in port_list:
      bport_oid = sai_thrift_get_bridge_port_by_port(client, port)
      client.sai_thrift_remove_bridge_port(bport_oid)

def sai_thrift_create_qos_map(client, map_type, key_list, data_list):
    qos_map_key_list = []
    qos_map_data_list = []
    attr_list = []

    attribute1_value = sai_thrift_attribute_value_t(s32=map_type)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_QOS_MAP_ATTR_TYPE, value=attribute1_value)
    attr_list.append(attribute1)

    if (map_type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(dot1p=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(tc=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_DSCP_TO_TC):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(dscp=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(tc=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(dot1p=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(color=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_DSCP_TO_COLOR):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(dscp=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(color=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_TC_TO_QUEUE):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(tc=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(queue_index=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(tc=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(pg=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DOT1P):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(tc=i[0], color=i[1])
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(dot1p=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(tc=i[0], color=i[1])
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(dscp=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP):
       for i in key_list:
           qos_map_key = sai_thrift_qos_map_params_t(prio=i)
           qos_map_key_list.append(qos_map_key)
       for j in data_list:
           qos_map_data = sai_thrift_qos_map_params_t(pg=j)
           qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_QUEUE):
       for i in key_list:
           qos_map_key = sai_thrift_qos_map_params_t(prio=i)
           qos_map_key_list.append(qos_map_key)
       for j in data_list:
           qos_map_data = sai_thrift_qos_map_params_t(queue_index=j)
           qos_map_data_list.append(qos_map_data)

    qos_map_list = sai_thrift_qos_map_list_t(
        key=qos_map_key_list, data=qos_map_data_list)
    attribute2_value = sai_thrift_attribute_value_t(qosmap=qos_map_list)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST, value=attribute2_value)
    attr_list.append(attribute2)

    qos_map_id = client.sai_thrift_create_qos_map(attr_list)
    return qos_map_id

def sai_thrift_set_qos_map_attribute(client, qos_map_id, map_type, key_list, data_list):
    qos_map_key_list = []
    qos_map_data_list = []
    if (map_type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(dot1p=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(tc=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_DSCP_TO_TC):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(dscp=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(tc=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(dot1p=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(color=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_DSCP_TO_COLOR):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(dscp=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(color=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_TC_TO_QUEUE):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(tc=i)
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(queue_index=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DOT1P):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(tc=i[0], color=i[1])
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(dot1p=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP):
        for i in key_list:
            qos_map_key = sai_thrift_qos_map_params_t(tc=i[0], color=i[1])
            qos_map_key_list.append(qos_map_key)
        for j in data_list:
            qos_map_data = sai_thrift_qos_map_params_t(dscp=j)
            qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP):
       for i in key_list:
           qos_map_key = sai_thrift_qos_map_params_t(prio=i)
           qos_map_key_list.append(qos_map_key)
       for j in data_list:
           qos_map_data = sai_thrift_qos_map_params_t(pg=j)
           qos_map_data_list.append(qos_map_data)
    elif (map_type == SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_QUEUE):
       for i in key_list:
           qos_map_key = sai_thrift_qos_map_params_t(prio=i)
           qos_map_key_list.append(qos_map_key)
       for j in data_list:
           qos_map_data = sai_thrift_qos_map_params_t(queue_index=j)
           qos_map_data_list.append(qos_map_data)

    qos_map_list = sai_thrift_qos_map_list_t(
        key=qos_map_key_list, data=qos_map_data_list)
    attribute1_value = sai_thrift_attribute_value_t(qosmap=qos_map_list)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_QOS_MAP_ATTR_MAP_TO_VALUE_LIST, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(s32=map_type)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_QOS_MAP_ATTR_TYPE, value=attribute2_value)
    attr_list = [attribute2, attribute1]

    client.sai_thrift_set_qos_map(qos_map_id, attr_list)

def sai_thrift_get_qos_map_attribute(client, qos_map_id, attr_id, map_count):
    thrift_attr = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_qos_map(qos_map_id, map_count, thrift_attr)

def sai_thrift_remove_qos_map(client, qos_map_id):
    client.sai_thrift_remove_qos_map(qos_map_id)

def sai_thrift_set_port_attribute(client, port_id, id, value):
    if (id == SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP or
            id == SAI_PORT_ATTR_QOS_DOT1P_TO_COLOR_MAP or
            id == SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP or
            id == SAI_PORT_ATTR_QOS_DSCP_TO_COLOR_MAP or
            id == SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP or
            id == SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DOT1P_MAP or
            id == SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP or
            id == SAI_PORT_ATTR_INGRESS_ACL or id == SAI_PORT_ATTR_EGRESS_ACL or
            id == SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP or
            id == SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_QUEUE_MAP or
            id == SAI_PORT_ATTR_INGRESS_MIRROR_SESSION or
            id == SAI_PORT_ATTR_QOS_SCHEDULER_PROFILE_ID or
            id == SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP or
            id == SAI_PORT_ATTR_QOS_MAXIMUM_HEADROOM_SIZE):
        attribute_value = sai_thrift_attribute_value_t(oid=value)
        attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
        client.sai_thrift_set_port_attribute(port_id, attribute)
    elif (id == SAI_PORT_ATTR_DROP_UNTAGGED or
          id == SAI_PORT_ATTR_DROP_TAGGED):
        attribute_value = sai_thrift_attribute_value_t(booldata=value)
        attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
        client.sai_thrift_set_port_attribute(port_id, attribute)
    elif (id == SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL):
        attribute_value = sai_thrift_attribute_value_t(u8=value)
        attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
        client.sai_thrift_set_port_attribute(port_id, attribute)

def sai_thrift_set_lag_attribute(client, lag_id, id, value):
    if (id == SAI_LAG_ATTR_DROP_UNTAGGED or
        id == SAI_LAG_ATTR_DROP_TAGGED):
        attribute_value = sai_thrift_attribute_value_t(booldata=value)
        attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
        client.sai_thrift_set_lag_attribute(lag_id, attribute)
    elif (id == SAI_LAG_ATTR_PORT_VLAN_ID):
        attribute_value = sai_thrift_attribute_value_t(u16=value)
        attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
        client.sai_thrift_set_lag_attribute(lag_id, attribute)


def sai_thrift_get_port_attribute(client, port_id, id):
    attributes_list = client.sai_thrift_get_port_attribute(port_id)
    attribute = []
    for attribute in attributes_list.attr_list:
        if attribute.id == id:
            return attribute
    return None

def sai_thrift_create_tunnel(client, tunnel_type, addr_family, ip_addr, underlay_if, overlay_if,
        imap=None, emap=None, encap_ttl_mode=None, encap_dscp_mode=None, encap_dscp_val=None):
    attr_list=[]

    attribute1_value=sai_thrift_attribute_value_t(s32 = tunnel_type)
    attribute1=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_TYPE,value=attribute1_value)
    attr_list.append(attribute1)

    attribute2_value=sai_thrift_attribute_value_t(oid=underlay_if)
    attribute2=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_UNDERLAY_INTERFACE,value=attribute2_value)
    attr_list.append(attribute2)

    attribute3_value=sai_thrift_attribute_value_t(oid=overlay_if)
    attribute3=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_OVERLAY_INTERFACE,value=attribute3_value)
    attr_list.append(attribute3)

    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
         addr = sai_thrift_ip_t(ip4=ip_addr)
         ipaddr = sai_thrift_ip_address_t(addr_family = SAI_IP_ADDR_FAMILY_IPV4 ,addr=addr)
    elif addr_family == SAI_IP_ADDR_FAMILY_IPV6:
         addr = sai_thrift_ip_t(ip6=ip_addr)
         ipaddr = sai_thrift_ip_address_t(addr_family = SAI_IP_ADDR_FAMILY_IPV6 ,addr=addr)
    if ip_addr is not None:
        attribute4_value=sai_thrift_attribute_value_t(ipaddr=ipaddr)
        attribute4=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_ENCAP_SRC_IP,value=attribute4_value)
        attr_list.append(attribute4)

    if encap_ttl_mode is not None:
        attribute5_value=sai_thrift_attribute_value_t(u32=encap_ttl_mode)
        attribute5=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_ENCAP_TTL_MODE,value=attribute5_value)
        attr_list.append(attribute5)

    if encap_dscp_mode is not None:
        attribute6_value=sai_thrift_attribute_value_t(u32=encap_dscp_mode)
        attribute6=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_ENCAP_DSCP_MODE,value=attribute6_value)
        attr_list.append(attribute6)

    attribute7_value=sai_thrift_attribute_value_t(u32=SAI_TUNNEL_TTL_MODE_PIPE_MODEL)#TTL
    attribute7=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_DECAP_TTL_MODE,value=attribute7_value);
    attr_list.append(attribute7)

    if encap_dscp_val is not None:
        attribute8_value=sai_thrift_attribute_value_t(u16=encap_dscp_val)#DCSP
        attribute8=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_ENCAP_DSCP_VAL,value=attribute8_value);
        attr_list.append(attribute8)
    attribute9_value=sai_thrift_attribute_value_t(u32=SAI_TUNNEL_DSCP_MODE_PIPE_MODEL)
    attribute9=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_DECAP_DSCP_MODE,value=attribute9_value);
    attr_list.append(attribute9)

    #encap mapper
    if imap is not None:
        attr_value=sai_thrift_attribute_value_t(
            objlist=sai_thrift_object_list_t(count=1, object_id_list=[imap]))
        attr=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_ENCAP_MAPPERS, value=attr_value)
        attr_list.append(attr)

    #decap mapper
    if emap is not None:
        attr_value=sai_thrift_attribute_value_t(
            objlist=sai_thrift_object_list_t(count=1, object_id_list=[emap]))
        attr=sai_thrift_attribute_t(id=SAI_TUNNEL_ATTR_DECAP_MAPPERS, value=attr_value)
        attr_list.append(attr)

    tunnel_id=client.sai_thrift_create_tunnel(attr_list)
    return tunnel_id

def sai_thrift_create_tunnel_term_table(client, tunnel_type, addr_family, vr_id, ip_addr_dst, ip_addr_src, tunnel_oid, term_type):
    attribute1_value=sai_thrift_attribute_value_t(oid=vr_id)
    attribute1=sai_thrift_attribute_t(id=SAI_TUNNEL_TERM_TABLE_ENTRY_ATTR_VR_ID,value=attribute1_value);

    attribute2_value=sai_thrift_attribute_value_t(s32=term_type)
    attribute2=sai_thrift_attribute_t(id=SAI_TUNNEL_TERM_TABLE_ENTRY_ATTR_TYPE,value=attribute2_value)

    if addr_family == SAI_IP_ADDR_FAMILY_IPV4 :
         addr = sai_thrift_ip_t(ip4=ip_addr_dst)
         ipaddr_dst = sai_thrift_ip_address_t(addr_family = SAI_IP_ADDR_FAMILY_IPV4 ,addr=addr)
    elif addr_family == SAI_IP_ADDR_FAMILY_IPV6:
         addr = sai_thrift_ip_t(ip6=ip_addr_dst)
         ipaddr_dst = sai_thrift_ip_address_t(addr_family = SAI_IP_ADDR_FAMILY_IPV6 ,addr=addr)

    attribute3_value=sai_thrift_attribute_value_t(ipaddr=ipaddr_dst)
    attribute3=sai_thrift_attribute_t(id=SAI_TUNNEL_TERM_TABLE_ENTRY_ATTR_DST_IP,value=attribute3_value)

    attribute4_value=sai_thrift_attribute_value_t(s32=tunnel_type)
    attribute4=sai_thrift_attribute_t(id=SAI_TUNNEL_TERM_TABLE_ENTRY_ATTR_TUNNEL_TYPE,value=attribute4_value)

    attribute5_value=sai_thrift_attribute_value_t(oid=tunnel_oid)
    attribute5=sai_thrift_attribute_t(id=SAI_TUNNEL_TERM_TABLE_ENTRY_ATTR_ACTION_TUNNEL_ID,value=attribute5_value)

    tb_attr_list=[attribute1,attribute2,attribute3,attribute4,attribute5]
    if ip_addr_src != None:
        if addr_family == SAI_IP_ADDR_FAMILY_IPV4 :
             addr = sai_thrift_ip_t(ip4=ip_addr_src)
             ipaddr_src = sai_thrift_ip_address_t(addr_family = SAI_IP_ADDR_FAMILY_IPV4 ,addr=addr)
        elif addr_family == SAI_IP_ADDR_FAMILY_IPV6:
             addr = sai_thrift_ip_t(ip6=ip_addr_src)
             ipaddr_src = sai_thrift_ip_address_t(addr_family = SAI_IP_ADDR_FAMILY_IPV6 ,addr=addr)

        attribute6_value=sai_thrift_attribute_value_t(ipaddr=ipaddr_src)
        attribute6=sai_thrift_attribute_t(id=SAI_TUNNEL_TERM_TABLE_ENTRY_ATTR_SRC_IP,value=attribute6_value)
        tb_attr_list.append(attribute6)

    tunnel_entry_id=client.sai_thrift_create_tunnel_term(tb_attr_list)
    return tunnel_entry_id

def sai_thrift_create_samplepacket(client,
                                   rate=0,
                                   mode=SAI_SAMPLEPACKET_MODE_SHARED):
    # rate
    attr1_v=sai_thrift_attribute_value_t(u32=rate)
    attr1=sai_thrift_attribute_t(
        id=SAI_SAMPLEPACKET_ATTR_SAMPLE_RATE, value=attr1_v)

    # mode
    attr2_v=sai_thrift_attribute_value_t(s32=mode)
    attr2=sai_thrift_attribute_t(
        id=SAI_SAMPLEPACKET_ATTR_MODE, value=attr2_v)

    attr_list=[attr1,attr2]
    samplepacket_id=client.sai_thrift_create_samplepacket(attr_list)
    return samplepacket_id

def sai_thrift_set_samplepacket_attribute(client,
                                          samplepacket_oid,
                                          attr_id,
                                          value):
    if attr_id == SAI_SAMPLEPACKET_ATTR_SAMPLE_RATE:
        attr_value = sai_thrift_attribute_value_t(u32=value)
    elif attr_id == SAI_SAMPLEPACKET_ATTR_MODE:
        attr_value = sai_thrift_attribute_value_t(s32=value)
    else:
        return

    attr = sai_thrift_attribute_t(id=attr_id, value=attr_value)
    client.sai_thrift_set_samplepacket_attribute(samplepacket_oid, attr)

def sai_thrift_create_dtel(client,
                           switch_id=0x11111111,
                           postcard_enable=False,
                           drop_report_enable=False,
                           queue_report_enable=False,
                           int_endpoint_enable=None,
                           int_dscp_value=None,
                           int_dscp_mask=None,
                           int_edge_ports=None,
                           latency_sensitivity=MAX_QUANTIZATION):

    attr_list=[]

    # switch id
    attr1_v=sai_thrift_attribute_value_t(u32=switch_id)
    attr1=sai_thrift_attribute_t(id=SAI_DTEL_ATTR_SWITCH_ID,value=attr1_v)
    attr_list.append(attr1)

    # postcard enable
    attr2_v=sai_thrift_attribute_value_t(booldata=postcard_enable)
    attr2=sai_thrift_attribute_t(id=SAI_DTEL_ATTR_POSTCARD_ENABLE, value=attr2_v)
    attr_list.append(attr2)

    # drop report enable
    attr3_v=sai_thrift_attribute_value_t(booldata=drop_report_enable)
    attr3=sai_thrift_attribute_t(id=SAI_DTEL_ATTR_DROP_REPORT_ENABLE, value=attr3_v)
    attr_list.append(attr3)

    # queue report enable
    attr4_v=sai_thrift_attribute_value_t(booldata=queue_report_enable)
    attr4=sai_thrift_attribute_t(id=SAI_DTEL_ATTR_QUEUE_REPORT_ENABLE, value=attr4_v)
    attr_list.append(attr4)

    # latency sensitivity
    attr5_v=sai_thrift_attribute_value_t(u8=latency_sensitivity)
    attr5=sai_thrift_attribute_t(
        id=SAI_DTEL_ATTR_LATENCY_SENSITIVITY, value=attr5_v)
    attr_list.append(attr5)

    # INT endpoint enable
    if int_endpoint_enable:
        attr6_v=sai_thrift_attribute_value_t(booldata=int_endpoint_enable)
        attr6=sai_thrift_attribute_t(
            id=SAI_DTEL_ATTR_INT_ENDPOINT_ENABLE, value=attr6_v)
        attr_list.append(attr6)

    # INT DSCP
    if (int_dscp_value and int_dscp_mask):
        attr7_v=sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u8=int_dscp_value),
                mask=sai_thrift_acl_mask_t(u8=int_dscp_mask)))
        attr7=sai_thrift_attribute_t(
            id=SAI_DTEL_ATTR_INT_L4_DSCP, value=attr7_v)
        attr_list.append(attr7)

    # INT edge ports
    if int_edge_ports:
        attr8_v=sai_thrift_attribute_value_t(objlist=(sai_thrift_object_list_t(
            object_id_list=int_edge_ports, count=len(int_edge_ports))))
        attr8=sai_thrift_attribute_t(
            id=SAI_DTEL_ATTR_SINK_PORT_LIST, value=attr8_v)
        attr_list.append(attr8)

    dtel_id=client.sai_thrift_create_dtel(attr_list)
    return dtel_id

def sai_thrift_set_dtel_attribute(client, dtel_oid, attr_id, value):
    if attr_id == SAI_DTEL_ATTR_SWITCH_ID:
        attr_value = sai_thrift_attribute_value_t(u32=value)
    if attr_id == SAI_DTEL_ATTR_POSTCARD_ENABLE:
        attr_value=sai_thrift_attribute_value_t(booldata=value)
    if attr_id == SAI_DTEL_ATTR_DROP_REPORT_ENABLE:
        attr_value = sai_thrift_attribute_value_t(booldata=value)
    if attr_id == SAI_DTEL_ATTR_QUEUE_REPORT_ENABLE:
        attr_value = sai_thrift_attribute_value_t(booldata=value)
    if attr_id == SAI_DTEL_ATTR_INT_ENDPOINT_ENABLE:
        attr_value = sai_thrift_attribute_value_t(booldata=value)
    if attr_id == SAI_DTEL_ATTR_INT_L4_DSCP:
        attr_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u8=value[0]),
                mask=sai_thrift_acl_mask_t(u8=value[1])))
    if attr_id == SAI_DTEL_ATTR_SINK_PORT_LIST:
        attr_value = sai_thrift_attribute_value_t(
            objlist=(sai_thrift_object_list_t(
                object_id_list=value, count=len(value))))
    if attr_id == SAI_DTEL_ATTR_LATENCY_SENSITIVITY:
        attr_value = sai_thrift_attribute_value_t(u8=latency_sensitivity)

    attr = sai_thrift_attribute_t(id=attr_id, value=attr_value)
    client.sai_thrift_set_dtel_attribute(dtel_oid, attr)

def sai_thrift_create_dtel_report_session(client, src_ip, dst_ip_list, vr_id, truncate_size, udp_port):
    report_attr_list = []

    #source ip
    src_ip_addr = sai_thrift_ip_address_t(
        addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=sai_thrift_ip_t(ip4=src_ip))
    attr = sai_thrift_attribute_t(
        id=SAI_DTEL_REPORT_SESSION_ATTR_SRC_IP,
        value=sai_thrift_attribute_value_t(ipaddr=src_ip_addr))
    report_attr_list.append(attr)

    #dst ip list
    dst_ip_addrs = []
    for ip in dst_ip_list:
        dst_ip_addr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=sai_thrift_ip_t(ip4=ip))
        dst_ip_addrs.append(dst_ip_addr)
    dst_ip_addr_list = sai_thrift_ipaddr_list_t(
        ipaddr_list=dst_ip_addrs, count=len(dst_ip_list))
    attr = sai_thrift_attribute_t(
        id=SAI_DTEL_REPORT_SESSION_ATTR_DST_IP_LIST,
        value=sai_thrift_attribute_value_t(ipaddrlist=dst_ip_addr_list))
    report_attr_list.append(attr)

    #vrf id
    attr = sai_thrift_attribute_t(
        id=SAI_DTEL_REPORT_SESSION_ATTR_VIRTUAL_ROUTER_ID,
        value=sai_thrift_attribute_value_t(oid=vr_id))
    report_attr_list.append(attr)

    #truncate size
    attr = sai_thrift_attribute_t(
        id=SAI_DTEL_REPORT_SESSION_ATTR_TRUNCATE_SIZE,
        value=sai_thrift_attribute_value_t(u16=truncate_size))
    report_attr_list.append(attr)

    # udp port
    attr = sai_thrift_attribute_t(
        id=SAI_DTEL_REPORT_SESSION_ATTR_UDP_DST_PORT,
        value=sai_thrift_attribute_value_t(u16=udp_port))
    report_attr_list.append(attr)

    report_session_id = client.sai_thrift_create_dtel_report_session(report_attr_list)
    return report_session_id

def sai_thrift_create_dtel_queue_report(client, queue_id, depth, latency, quota, tail_drop):
    queue_list = []
    '''
    port_attr_list = self.client.sai_thrift_get_port_attribute(self.port)
    attr_list = port_attr_list.attr_list
    for attribute in attr_list:
        if attribute.id == SAI_PORT_ATTR_QOS_QUEUE_LIST:
            for queue_id in attribute.value.objlist.object_id_list:
                queue_list.append(queue_id)
    '''

    # queue id
    attr1 = sai_thrift_attribute_t(
        id=SAI_DTEL_QUEUE_REPORT_ATTR_QUEUE_ID,
        value=sai_thrift_attribute_value_t(oid=queue_id))

    # depth threshold
    attr2 = sai_thrift_attribute_t(
        id=SAI_DTEL_QUEUE_REPORT_ATTR_DEPTH_THRESHOLD,
        value=sai_thrift_attribute_value_t(u32=depth))

    # latency threshold
    attr3 = sai_thrift_attribute_t(
        id=SAI_DTEL_QUEUE_REPORT_ATTR_LATENCY_THRESHOLD,
        value=sai_thrift_attribute_value_t(u32=latency))

    # breach quota
    attr4 = sai_thrift_attribute_t(
        id=SAI_DTEL_QUEUE_REPORT_ATTR_BREACH_QUOTA,
        value=sai_thrift_attribute_value_t(u32=quota))

    # tail drop enable
    attr5 = sai_thrift_attribute_t(
        id=SAI_DTEL_QUEUE_REPORT_ATTR_TAIL_DROP,
        value=sai_thrift_attribute_value_t(booldata=tail_drop))

    attr_list = [attr1, attr2, attr3, attr4, attr5]
    queue_report_id = client.sai_thrift_create_dtel_queue_report(attr_list)
    return queue_report_id

def sai_thrift_create_nat_entry(client, nat_type, sip_key, dip_key, proto_key, sport_key, dport_key, xlate_sip, xlate_dip, xlate_sport, xlate_dport):
    thrift_nat_entry = sai_thrift_nat_entry_t(sip_key, dip_key, sport_key, dport_key, proto_key, nat_type)
    attr_list = []
    if xlate_sip:
      attr_value = sai_thrift_attribute_value_t(ip4=xlate_sip)
      attr = sai_thrift_attribute_t(id=SAI_NAT_ENTRY_ATTR_SRC_IP, value = attr_value)
      attr_list.append(attr)
    if xlate_dip:
      attr_value = sai_thrift_attribute_value_t(ip4=xlate_dip)
      attr = sai_thrift_attribute_t(id=SAI_NAT_ENTRY_ATTR_DST_IP, value = attr_value)
      attr_list.append(attr)
    if xlate_sport:
      attr_value = sai_thrift_attribute_value_t(u16=xlate_sport)
      attr = sai_thrift_attribute_t(id = SAI_NAT_ENTRY_ATTR_L4_SRC_PORT, value = attr_value)
      attr_list.append(attr)
    if xlate_dport:
      attr_value = sai_thrift_attribute_value_t(u16=xlate_dport)
      attr = sai_thrift_attribute_t(id = SAI_NAT_ENTRY_ATTR_L4_DST_PORT, value = attr_value)
      attr_list.append(attr)

    client.sai_thrift_create_nat_entry(thrift_nat_entry, attr_list)

def sai_thrift_delete_nat_entry(client, nat_type, sip_key, dip_key, proto_key, sport_key, dport_key):
    thrift_nat_entry = sai_thrift_nat_entry_t(sip_key, dip_key, sport_key, dport_key, proto_key, nat_type)
    client.sai_thrift_delete_nat_entry(thrift_nat_entry);

def sai_thrift_get_nat_entry(client, nat_type, sip_key, dip_key, proto_key, sport_key, dport_key,attr_id):
    thrift_nat_entry = sai_thrift_nat_entry_t(sip_key, dip_key, sport_key, dport_key, proto_key, nat_type)
    attribute = sai_thrift_attribute_t(id=attr_id)
    return client.sai_thrift_get_nat_entry_attribute(thrift_nat_entry, attribute);

def sai_thrift_set_nat_entry(client, nat_type, sip_key, dip_key, proto_key, sport_key, dport_key,attr_id):
    thrift_nat_entry = sai_thrift_nat_entry_t(sip_key, dip_key, sport_key, dport_key, proto_key, nat_type)
    attribute = sai_thrift_attribute_t(id=attr_id)
    return client.sai_thrift_set_nat_entry_attribute(thrift_nat_entry, attribute);
