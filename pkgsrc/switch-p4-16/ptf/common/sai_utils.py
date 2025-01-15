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

import time
import sys
import logging

import unittest
import random

from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os
import six

from switchsai_thrift.ttypes import *
from switchsai_thrift.sai_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))

switch_inited = 0
port_list = []
table_attr_list = []


def sai_thrift_create_bridge(client, bridge_type):
    attribute1_value = sai_thrift_attribute_value_t(s32=bridge_type)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_BRIDGE_ATTR_TYPE, value=attribute1_value)
    attr_list = [attribute1]
    bridge_port = client.sai_thrift_create_bridge(thrift_attr_list=attr_list)
    return bridge_port


def sai_thrift_create_bridge_port(client,
                                  type,
                                  bridge_id,
                                  port_lag_id=None,
                                  vlan=None,
                                  rif_id=None,
                                  tunnel_id=None,
                                  admin_state=True):

    attr_list = []
    attribute_value = sai_thrift_attribute_value_t(s32=type)
    attribute = sai_thrift_attribute_t(
        id=SAI_BRIDGE_PORT_ATTR_TYPE, value=attribute_value)
    attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(oid=bridge_id)
    attribute = sai_thrift_attribute_t(
        id=SAI_BRIDGE_PORT_ATTR_BRIDGE_ID, value=attribute_value)
    attr_list.append(attribute)

    if port_lag_id:
        attribute_value = sai_thrift_attribute_value_t(oid=port_lag_id)
        attribute = sai_thrift_attribute_t(
            id=SAI_BRIDGE_PORT_ATTR_PORT_ID, value=attribute_value)
        attr_list.append(attribute)

    if vlan:
        attribute_value = sai_thrift_attribute_value_t(u16=vlan)
        attribute = sai_thrift_attribute_t(
            id=SAI_BRIDGE_PORT_ATTR_VLAN_ID, value=attribute_value)
        attr_list.append(attribute)

    if rif_id:
        attribute_value = sai_thrift_attribute_value_t(oid=rif_id)
        attribute = sai_thrift_attribute_t(
            id=SAI_BRIDGE_PORT_ATTR_RIF_ID, value=attribute_value)
        attr_list.append(attribute)

    if tunnel_id:
        attribute_value = sai_thrift_attribute_value_t(oid=tunnel_id)
        attribute = sai_thrift_attribute_t(
            id=SAI_BRIDGE_PORT_ATTR_TUNNEL_ID, value=attribute_value)
        attr_list.append(attribute)

    if admin_state:
        attribute_value = sai_thrift_attribute_value_t(booldata=admin_state)
        attribute = sai_thrift_attribute_t(
            id=SAI_BRIDGE_PORT_ATTR_ADMIN_STATE, value=attribute_value)
        attr_list.append(attribute)

    bridge_port = client.sai_thrift_create_bridge_port(attr_list)
    return bridge_port


def sai_thrift_get_vlan_attribute(client, vlan_id, attr_id, member_count=0):
    attr = sai_thrift_attribute_t(id = attr_id)
    thrift_list = client.sai_thrift_get_vlan_attribute(vlan_id, member_count, attribute=attr)
    return thrift_list

def sai_thrift_set_vlan_attribute(client, vlan_id, stp_id):
    attribute_value = sai_thrift_attribute_value_t(oid=stp_id)
    attribute = sai_thrift_attribute_t(
        id=SAI_VLAN_ATTR_STP_INSTANCE, value=attribute_value)
    client.sai_thrift_set_vlan_attribute(vlan_id, attribute=attribute)


def sai_thrift_create_vlan(client, vlan_id):
    attribute1_value = sai_thrift_attribute_value_t(u16=vlan_id)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_VLAN_ATTR_VLAN_ID, value=attribute1_value)
    attr_list = [attribute1]
    vlan_handle = client.sai_thrift_create_vlan(thrift_attr_list=attr_list)
    return vlan_handle


def sai_thrift_create_vlan_member(client, vlan_id, bridge_port, tagging_mode):

    attribute1_value = sai_thrift_attribute_value_t(u16=vlan_id)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_VLAN_MEMBER_ATTR_VLAN_ID, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(oid=bridge_port)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_VLAN_MEMBER_ATTR_BRIDGE_PORT_ID, value=attribute2_value)
    attribute3_value = sai_thrift_attribute_value_t(s32=tagging_mode)
    attribute3 = sai_thrift_attribute_t(
        id=SAI_VLAN_MEMBER_ATTR_VLAN_TAGGING_MODE, value=attribute3_value)
    attr_list = [attribute1, attribute2, attribute3]
    vlan_member = client.sai_thrift_create_vlan_member(
        thrift_attr_list=attr_list)
    return vlan_member

def sai_thrift_set_vlan_member_attribute(client, vlan_member_handle, attr_id, attr_value):
    print(attr_value)
    sai_attr_value = sai_thrift_attribute_value_t(u32=attr_value)
    sai_attr = sai_thrift_attribute_t(id = attr_id, value=sai_attr_value)
    print ("Sai attr value %s"%(sai_attr))
    return client.sai_thrift_set_vlan_member_attribute(vlan_member_handle, sai_attr)

def sai_thrift_get_vlan_member_attribute(client, vlan_member_handle, attr_id):
    attr = sai_thrift_attribute_t(id = attr_id)
    thrift_list = client.sai_thrift_get_vlan_member_attribute(vlan_member_handle, attribute=attr)
    return thrift_list

def sai_thrift_create_fdb(client,
                          mac,
                          bridge_port,
                          mac_action,
                          bridge_type,
                          vlan_id=0,
                          bridge_id=0):

    fdb_entry = sai_thrift_fdb_entry_t(
        mac_address=mac,
        vlan_id=vlan_id,
        bridge_type=bridge_type,
        bridge_id=bridge_id)
    #value 0 represents static entry, id=0, represents entry type
    fdb_attribute1_value = sai_thrift_attribute_value_t(
        s32=SAI_FDB_ENTRY_TYPE_STATIC)
    fdb_attribute1 = sai_thrift_attribute_t(
        id=SAI_FDB_ENTRY_ATTR_TYPE, value=fdb_attribute1_value)
    #value oid represents object id, id=1 represents port id
    fdb_attribute2_value = sai_thrift_attribute_value_t(oid=bridge_port)
    fdb_attribute2 = sai_thrift_attribute_t(
        id=SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID, value=fdb_attribute2_value)
    #value oid represents object id, id=1 represents port id
    fdb_attribute3_value = sai_thrift_attribute_value_t(s32=mac_action)
    fdb_attribute3 = sai_thrift_attribute_t(
        id=SAI_FDB_ENTRY_ATTR_PACKET_ACTION, value=fdb_attribute3_value)
    fdb_attr_list = [fdb_attribute1, fdb_attribute2, fdb_attribute3]
    client.sai_thrift_create_fdb_entry(
        thrift_fdb_entry=fdb_entry, thrift_attr_list=fdb_attr_list)


def sai_thrift_set_fdb_attribute(client, mac, vlan_id, bridge_type, attr_id, attr_value, bridge_id = 0):

    fdb_entry = sai_thrift_fdb_entry_t(
                    mac_address=mac, vlan_id = vlan_id,
                    bridge_type=bridge_type,bridge_id=bridge_id)
    fdb_attribute = sai_thrift_attribute_t(id=attr_id, value=sai_thrift_attribute_value_t(oid=attr_value[1]))
    client.sai_thrift_set_fdb_entry(fdb_entry, fdb_attribute)

def sai_thrift_get_fdb_attribute(client, attr_count, attr_id, mac, bridge_port, bridge_type, vlan_id, bridge_id=0):
    fdb_entry = sai_thrift_fdb_entry_t(
                    mac_address=mac, vlan_id = vlan_id,
                    bridge_type=bridge_type,bridge_id=bridge_id)

    fdb_attribute = sai_thrift_attribute_t(id=attr_id)
    attr_list = client.sai_thrift_get_fdb_attribute(fdb_entry, fdb_attribute)
    return attr_list

def sai_thrift_delete_fdb(client,
                          mac,
                          bridge_port,
                          bridge_type,
                          vlan_id=0,
                          bridge_id=0):
    fdb_entry = sai_thrift_fdb_entry_t(
        mac_address=mac,
        bridge_type=bridge_type,
        vlan_id=vlan_id,
        bridge_id=bridge_id)
    client.sai_thrift_delete_fdb_entry(thrift_fdb_entry=fdb_entry)


def sai_thrift_flush_fdb_by_vlan(client, vlan_id):
    fdb_attribute1_value = sai_thrift_attribute_value_t(u16=vlan_id)
    fdb_attribute1 = sai_thrift_attribute_t(
        id=SAI_FDB_FLUSH_ATTR_VLAN_ID, value=fdb_attribute1_value)
    fdb_attribute2_value = sai_thrift_attribute_value_t(
        s32=SAI_FDB_FLUSH_ENTRY_TYPE_STATIC)
    fdb_attribute2 = sai_thrift_attribute_t(
        id=SAI_FDB_FLUSH_ATTR_ENTRY_TYPE, value=fdb_attribute2_value)
    fdb_attr_list = [fdb_attribute1, fdb_attribute2]
    client.sai_thrift_flush_fdb_entries(thrift_attr_list=fdb_attr_list)

    #flush both static and dynamic entries
    fdb_attribute2_value = sai_thrift_attribute_value_t(
        s32=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
    fdb_attribute2 = sai_thrift_attribute_t(
        id=SAI_FDB_FLUSH_ATTR_ENTRY_TYPE, value=fdb_attribute2_value)
    fdb_attr_list = [fdb_attribute1, fdb_attribute2]
    client.sai_thrift_flush_fdb_entries(thrift_attr_list=fdb_attr_list)


def sai_thrift_create_virtual_router(client, v4_enabled, v6_enabled):
    #v4 enabled
    vr_attribute1_value = sai_thrift_attribute_value_t(booldata=v4_enabled)
    vr_attribute1 = sai_thrift_attribute_t(
        id=SAI_VIRTUAL_ROUTER_ATTR_ADMIN_V4_STATE, value=vr_attribute1_value)
    #v6 enabled
    vr_attribute2_value = sai_thrift_attribute_value_t(booldata=v6_enabled)
    vr_attribute2 = sai_thrift_attribute_t(
        id=SAI_VIRTUAL_ROUTER_ATTR_ADMIN_V6_STATE, value=vr_attribute2_value)
    vr_attr_list = [vr_attribute1, vr_attribute2]
    vr_id = client.sai_thrift_create_virtual_router(
        thrift_attr_list=vr_attr_list)
    return vr_id


def sai_thrift_set_router_interface(client, rif_id, attr_id, attr_value):
    if (attr_id == SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE or \
        attr_id == SAI_ROUTER_INTERFACE_ATTR_ADMIN_V6_STATE or \
        attr_id == SAI_ROUTER_INTERFACE_ATTR_V4_MCAST_ENABLE or \
        attr_id == SAI_ROUTER_INTERFACE_ATTR_V6_MCAST_ENABLE):
        attr_val = sai_thrift_attribute_value_t(booldata=attr_value)
    else:
        attr_val = sai_thrift_attribute_value_t(u32=attr_value)
    attr = sai_thrift_attribute_t(
         id=attr_id, value=attr_val)
    client.sai_thrift_set_router_interface_attribute(rif_id, attr)

def sai_thrift_get_router_interface_attribute(client, rif_id, attr_id):
    rif_attribute = sai_thrift_attribute_t(id = attr_id)
    attr_list = client.sai_thrift_get_router_interface_attribute(rif_id, 1, rif_attribute)
    return attr_list

def sai_thrift_create_router_interface(client,
                                       type,
                                       vr_id,
                                       port_id=None,
                                       vlan_id=None,
                                       v4_enabled=None,
                                       v6_enabled=None,
                                       mac=None):
    #vrf attribute
    rif_attr_list = []
    attr_val = sai_thrift_attribute_value_t(oid=vr_id)
    attr = sai_thrift_attribute_t(
        id=SAI_ROUTER_INTERFACE_ATTR_VIRTUAL_ROUTER_ID, value=attr_val)
    rif_attr_list.append(attr)

    attr_val = sai_thrift_attribute_value_t(s32=type)
    attr = sai_thrift_attribute_t(
        id=SAI_ROUTER_INTERFACE_ATTR_TYPE, value=attr_val)
    rif_attr_list.append(attr)

    if port_id:
        attr_val = sai_thrift_attribute_value_t(oid=port_id)
        attr = sai_thrift_attribute_t(
            id=SAI_ROUTER_INTERFACE_ATTR_PORT_ID, value=attr_val)
        rif_attr_list.append(attr)

        rif_attr_list.append(attr)
    if vlan_id:
        attr_val = sai_thrift_attribute_value_t(u16=vlan_id)
        attr = sai_thrift_attribute_t(
            id=SAI_ROUTER_INTERFACE_ATTR_VLAN_ID, value=attr_val)
        rif_attr_list.append(attr)
    if v4_enabled:
        #v4_enabled
        attr_val = sai_thrift_attribute_value_t(booldata=v4_enabled)
        attr = sai_thrift_attribute_t(
            id=SAI_ROUTER_INTERFACE_ATTR_ADMIN_V4_STATE, value=attr_val)
        rif_attr_list.append(attr)
    if v6_enabled:
        #v6_enabled
        attr_val = sai_thrift_attribute_value_t(booldata=v6_enabled)
        attr = sai_thrift_attribute_t(
            id=SAI_ROUTER_INTERFACE_ATTR_ADMIN_V6_STATE, value=attr_val)
        rif_attr_list.append(attr)

    if mac:
        attr_val = sai_thrift_attribute_value_t(mac=mac)
        attr = sai_thrift_attribute_t(
            id=SAI_ROUTER_INTERFACE_ATTR_SRC_MAC_ADDRESS, value=attr_val)
        rif_attr_list.append(attr)

    rif_id = client.sai_thrift_create_router_interface(rif_attr_list)
    return rif_id


def sai_thrift_create_route(client, vr_id, addr_family, ip_addr, ip_mask, nhop):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        mask = sai_thrift_ip_t(ip4=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr, mask=mask)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        mask = sai_thrift_ip_t(ip6=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr, mask=mask)
    route_attribute1_value = sai_thrift_attribute_value_t(oid=nhop)
    route_attribute1 = sai_thrift_attribute_t(
        id=SAI_ROUTE_ENTRY_ATTR_NEXT_HOP_ID, value=route_attribute1_value)
    route = sai_thrift_route_entry_t(vr_id, ip_prefix)
    route_attr_list = [route_attribute1]
    client.sai_thrift_create_route(
        thrift_unicast_route_entry=route, thrift_attr_list=route_attr_list)

def sai_thrift_set_route_attribute(client, vrf_id, addr_family, ip_addr, ip_mask, attr_id, attr_value):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        mask = sai_thrift_ip_t(ip4=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr, mask=mask)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        mask = sai_thrift_ip_t(ip6=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr, mask=mask)

    route = sai_thrift_route_entry_t(vrf_id, ip_prefix)
    if attr_id == SAI_ROUTE_ENTRY_ATTR_NEXT_HOP_ID:
        attribute_value = sai_thrift_attribute_value_t(oid=attr_value)
    else:
        attribute_value = sai_thrift_attribute_value_t(u32=attr_value)
    route_attribute = sai_thrift_attribute_t(id=attr_id, value=attribute_value)
    client.sai_thrift_set_route_attribute(thrift_unicast_route_entry=route, thrift_attr=route_attribute)

def sai_thrift_get_route_attribute(client, vrf_id, addr_family, ip_addr, ip_mask, attr_id):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        mask = sai_thrift_ip_t(ip4=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr, mask=mask)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        mask = sai_thrift_ip_t(ip6=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr, mask=mask)
    route_entry = sai_thrift_route_entry_t(vrf_id, ip_prefix)
    route_attribute = sai_thrift_attribute_t(id=attr_id)
    attr_list = client.sai_thrift_get_route_attribute(route_entry, route_attribute)
    return attr_list

def sai_thrift_remove_route(client, vr_id, addr_family, ip_addr, ip_mask, nhop):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        mask = sai_thrift_ip_t(ip4=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr, mask=mask)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        mask = sai_thrift_ip_t(ip6=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr, mask=mask)
    route = sai_thrift_route_entry_t(vr_id, ip_prefix)
    client.sai_thrift_remove_route(thrift_unicast_route_entry=route)


def sai_thrift_create_nhop(client, addr_family, ip_addr, rif_id):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr)
    nhop_attribute1_value = sai_thrift_attribute_value_t(ipaddr=ipaddr)
    nhop_attribute1 = sai_thrift_attribute_t(
        id=SAI_NEXT_HOP_ATTR_IP, value=nhop_attribute1_value)
    nhop_attribute2_value = sai_thrift_attribute_value_t(oid=rif_id)
    nhop_attribute2 = sai_thrift_attribute_t(
        id=SAI_NEXT_HOP_ATTR_ROUTER_INTERFACE_ID, value=nhop_attribute2_value)
    nhop_attr_list = [nhop_attribute1, nhop_attribute2]
    nhop = client.sai_thrift_create_next_hop(thrift_attr_list=nhop_attr_list)
    return nhop


def sai_thrift_create_neighbor(client, addr_family, rif_id, ip_addr, dmac):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr)
    neighbor_attribute1_value = sai_thrift_attribute_value_t(mac=dmac)
    neighbor_attribute1 = sai_thrift_attribute_t(
        id=SAI_NEIGHBOR_ENTRY_ATTR_DST_MAC_ADDRESS,
        value=neighbor_attribute1_value)
    neighbor_attr_list = [neighbor_attribute1]
    neighbor_entry = sai_thrift_neighbor_entry_t(
        rif_id=rif_id, ip_address=ipaddr)
    client.sai_thrift_create_neighbor_entry(neighbor_entry, neighbor_attr_list)

def sai_thrift_get_neighbor(client, addr_family, rif_id, ip_addr):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr)
    neighbor_attribute1_value = sai_thrift_attribute_value_t(mac=' ')
    neighbor_attribute1 = sai_thrift_attribute_t(
        id=SAI_NEIGHBOR_ENTRY_ATTR_DST_MAC_ADDRESS,
        value=neighbor_attribute1_value)
    neighbor_entry = sai_thrift_neighbor_entry_t(
        rif_id=rif_id, ip_address=ipaddr)
    return client.sai_thrift_get_neighbor_entry(neighbor_entry, neighbor_attribute1)

def sai_thrift_set_neighbor(client, addr_family, rif_id, ip_addr, dmac):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr)
    neighbor_attribute1_value = sai_thrift_attribute_value_t(mac=dmac)
    neighbor_attribute1 = sai_thrift_attribute_t(
        id=SAI_NEIGHBOR_ENTRY_ATTR_DST_MAC_ADDRESS,
        value=neighbor_attribute1_value)
    #neighbor_attr_list = [neighbor_attribute1]
    neighbor_entry = sai_thrift_neighbor_entry_t(
        rif_id=rif_id, ip_address=ipaddr)
    client.sai_thrift_set_neighbor_entry(neighbor_entry, neighbor_attribute1)

def sai_thrift_remove_neighbor(client, addr_family, rif_id, ip_addr, dmac):
    if addr_family == SAI_IP_ADDR_FAMILY_IPV4:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(
            addr_family=SAI_IP_ADDR_FAMILY_IPV6, addr=addr)
    neighbor_entry = sai_thrift_neighbor_entry_t(
        rif_id=rif_id, ip_address=ipaddr)
    client.sai_thrift_remove_neighbor_entry(neighbor_entry)


def sai_thrift_create_next_hop_group(client):
    nhop_group_attribute1_value = sai_thrift_attribute_value_t(
        s32=SAI_NEXT_HOP_GROUP_TYPE_ECMP)
    nhop_group_attribute1 = sai_thrift_attribute_t(
        id=SAI_NEXT_HOP_GROUP_ATTR_TYPE, value=nhop_group_attribute1_value)
    nhop_group_attr_list = [nhop_group_attribute1]
    nhop_group = client.sai_thrift_create_next_hop_group(
        thrift_attr_list=nhop_group_attr_list)
    return nhop_group

def sai_thrift_get_next_hop_group(client, nhop_grp_id, attr_id):
    nhop_group_attribute1 = sai_thrift_attribute_t(id=attr_id)
    return client.sai_thrift_get_next_hop_group_attribute(nhop_grp_id, nhop_group_attribute1)


def sai_thrift_create_next_hop_group_member(client, nhop_group_id, nhop_id):
    nhop_member_attribute1_value = sai_thrift_attribute_value_t(
        oid=nhop_group_id)
    nhop_member_attribute1 = sai_thrift_attribute_t(
        id=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_GROUP_ID,
        value=nhop_member_attribute1_value)
    nhop_member_attribute2_value = sai_thrift_attribute_value_t(oid=nhop_id)
    nhop_member_attribute2 = sai_thrift_attribute_t(
        id=SAI_NEXT_HOP_GROUP_MEMBER_ATTR_NEXT_HOP_ID,
        value=nhop_member_attribute2_value)
    nhop_member_attr_list = [nhop_member_attribute1, nhop_member_attribute2]
    nhop_group_member = client.sai_thrift_create_next_hop_group_member(
        thrift_attr_list=nhop_member_attr_list)
    return nhop_group_member


def sai_thrift_get_next_hop_group_member(client, nhop_grp_id, attr_id):
    nhop_group_attribute1 = sai_thrift_attribute_t(id=attr_id)
    return client.sai_thrift_get_next_hop_group_member_attribute(nhop_grp_id, nhop_group_attribute1)

def sai_thrift_create_lag_member(client,
                                 lag_id,
                                 port_id,
                                 ingress_disable=False,
                                 egress_disable=False):
    attr_list = []
    attribute_value = sai_thrift_attribute_value_t(oid=lag_id)
    attribute = sai_thrift_attribute_t(
        id=SAI_LAG_MEMBER_ATTR_LAG_ID, value=attribute_value)
    attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(oid=port_id)
    attribute = sai_thrift_attribute_t(
        id=SAI_LAG_MEMBER_ATTR_PORT_ID, value=attribute_value)
    attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(booldata=ingress_disable)
    attribute = sai_thrift_attribute_t(
        id=SAI_LAG_MEMBER_ATTR_INGRESS_DISABLE, value=attribute_value)
    attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(booldata=egress_disable)
    attribute = sai_thrift_attribute_t(
        id=SAI_LAG_MEMBER_ATTR_EGRESS_DISABLE, value=attribute_value)
    attr_list.append(attribute)

    lag_member = client.sai_thrift_create_lag_member(attr_list)
    return lag_member


def sai_thrift_create_stp(client):
    stp_attr_list = []
    stp_id = client.sai_thrift_create_stp(stp_attr_list)
    return stp_id

def sai_thrift_get_stp(client, stp_id, attr_id, count):
    thrift_sai_stp = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_stp_attribute(stp_id, thrift_sai_stp, count);

def sai_thrift_create_stp_port(client, stp_id, bridge_port, stp_state):
    stp_attribute1_value = sai_thrift_attribute_value_t(oid=stp_id)
    stp_attribute1 = sai_thrift_attribute_t(
        id=SAI_STP_PORT_ATTR_STP, value=stp_attribute1_value)
    stp_attribute2_value = sai_thrift_attribute_value_t(oid=bridge_port)
    stp_attribute2 = sai_thrift_attribute_t(
        id=SAI_STP_PORT_ATTR_BRIDGE_PORT, value=stp_attribute2_value)
    stp_attribute3_value = sai_thrift_attribute_value_t(u32=stp_state)
    stp_attribute3 = sai_thrift_attribute_t(
        id=SAI_STP_PORT_ATTR_STATE, value=stp_attribute3_value)
    stp_attr_list = [stp_attribute1, stp_attribute2, stp_attribute3]
    stp_port_id = client.sai_thrift_create_stp_port(stp_attr_list)
    return stp_port_id

def sai_thrift_get_stp_port(client, stp_id, attr_id):
    thrift_sai_port_attr = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_stp_port_attribute(stp_id, thrift_sai_port_attr)

def sai_thrift_create_hostif_trap_group(client, queue_id, policer_id):
    attribute1_value = sai_thrift_attribute_value_t(u32=queue_id)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TRAP_GROUP_ATTR_QUEUE, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(oid=policer_id)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TRAP_GROUP_ATTR_POLICER, value=attribute2_value)
    attr_list = [attribute1, attribute2]
    trap_group_id = client.sai_thrift_create_hostif_trap_group(
        thrift_attr_list=attr_list)
    return trap_group_id


def sai_thrift_create_hostif_trap(client, trap_id, action, priority,
                                  trap_group_id):
    attribute1_value = sai_thrift_attribute_value_t(u32=trap_id)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TRAP_ATTR_TRAP_TYPE, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(oid=trap_group_id)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TRAP_ATTR_TRAP_GROUP, value=attribute2_value)
    attribute3_value = sai_thrift_attribute_value_t(u32=action)
    attribute3 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TRAP_ATTR_PACKET_ACTION, value=attribute3_value)
    attribute4_value = sai_thrift_attribute_value_t(u32=priority)
    attribute4 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TRAP_ATTR_TRAP_PRIORITY, value=attribute4_value)
    attr_list = [attribute1, attribute2, attribute3, attribute4]
    hif_trap_id = client.sai_thrift_create_hostif_trap(attr_list)
    return hif_trap_id


def sai_thrift_create_hostif(client, intf_name, handle, type):
    attribute1_value = sai_thrift_attribute_value_t(u32=type)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_ATTR_TYPE, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(oid=handle)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_ATTR_OBJ_ID, value=attribute2_value)
    attribute3_value = sai_thrift_attribute_value_t(chardata=intf_name)
    attribute3 = sai_thrift_attribute_t(
        id=SAI_HOSTIF_ATTR_NAME, value=attribute3_value)
    attr_list = [attribute1, attribute2, attribute3]
    hif_id = client.sai_thrift_create_hostif(attr_list)
    return hif_id


def sai_thrift_create_hostif_table_entry(client, type, handle, rc, channel_type,
                                         hostif_id):
    attr_list = []
    attr_val = sai_thrift_attribute_value_t(u32=type)
    attr = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TABLE_ENTRY_ATTR_TYPE, value=attr_val)
    attr_list.append(attr)
    attr_val = sai_thrift_attribute_value_t(oid=handle)
    attr = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TABLE_ENTRY_ATTR_OBJ_ID, value=attr_val)
    attr_list.append(attr)
    attr_val = sai_thrift_attribute_value_t(oid=rc)
    attr = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TABLE_ENTRY_ATTR_TRAP_ID, value=attr_val)
    attr_list.append(attr)
    attr_val = sai_thrift_attribute_value_t(u32=channel_type)
    attr = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TABLE_ENTRY_ATTR_CHANNEL_TYPE, value=attr_val)
    attr_list.append(attr)
    attr_val = sai_thrift_attribute_value_t(oid=hostif_id)
    attr = sai_thrift_attribute_t(
        id=SAI_HOSTIF_TABLE_ENTRY_ATTR_HOST_IF, value=attr_val)
    attr_list.append(attr)
    hif_table_entry_id = client.sai_thrift_create_hostif_table_entry(attr_list)
    return hif_table_entry_id


def sai_thrift_create_acl_table(client,
                                acl_stage=SAI_ACL_STAGE_INGRESS,
                                addr_family=False,
                                ip_src=False,
                                ip_dst=False,
                                ip_proto=False,
                                out_ports=False,
                                in_port=False,
                                out_port=False,
                                bp_point=[SAI_ACL_BIND_POINT_TYPE_PORT],
                                outer_vlan_id=None):
    acl_attr_list = []

    attribute_value = sai_thrift_attribute_value_t(s32=acl_stage)
    attribute = sai_thrift_attribute_t(
        id=SAI_ACL_TABLE_ATTR_ACL_STAGE, value=attribute_value)
    acl_attr_list.append(attribute)

    attribute_value = sai_thrift_attribute_value_t(s32list=sai_thrift_s32_list_t(
        s32list=bp_point, count=len(bp_point)))
    attribute = sai_thrift_attribute_t(
        id=SAI_ACL_TABLE_GROUP_ATTR_ACL_BIND_POINT_TYPE_LIST,
        value=attribute_value)
    acl_attr_list.append(attribute)

    if ip_src:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_TABLE_ATTR_FIELD_SRC_IP, value=attribute_value)
        acl_attr_list.append(attribute)
    if ip_dst:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_TABLE_ATTR_FIELD_DST_IP, value=attribute_value)
        acl_attr_list.append(attribute)
    if ip_proto:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_TABLE_ATTR_FIELD_IP_PROTOCOL, value=attribute_value)
        acl_attr_list.append(attribute)
    if out_ports:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_TABLE_ATTR_FIELD_OUT_PORTS, value=attribute_value)
        acl_attr_list.append(attribute)
    if in_port:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_TABLE_ATTR_FIELD_IN_PORT, value=attribute_value)
        acl_attr_list.append(attribute)
    if out_port:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_TABLE_ATTR_FIELD_OUT_PORT, value=attribute_value)
        acl_attr_list.append(attribute)
    if outer_vlan_id != None:
        attribute_value = sai_thrift_attribute_value_t(booldata=1)
        attribute = sai_thrift_attribute_t(id=SAI_ACL_TABLE_ATTR_FIELD_OUTER_VLAN_ID,
                                           value=attribute_value)
        acl_attr_list.append(attribute)

    acl_table_id = client.sai_thrift_create_acl_table(acl_attr_list)
    return acl_table_id


def sai_thrift_set_acl_entry_action(client, ace_id,
                                    ace_action, packet_action):

    if(ace_action == SAI_ACL_ENTRY_ATTR_ACTION_PACKET_ACTION):

        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                s32=packet_action)))
    else:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                oid=packet_action)))

    attribute = sai_thrift_attribute_t(id = ace_action,
                value=attribute_value)
    return client.sai_thrift_set_acl_entry_attribute(ace_id, attribute)

def sai_thrift_get_acl_entry_attribute(client, ace_id, attr_id):
    ace_attr = sai_thrift_attribute_t(id = attr_id)
    attr_list = client.sai_thrift_get_acl_entry_attribute(ace_id, ace_attr)
    return attr_list

def sai_thrift_create_acl_entry(client,
                                acl_table_id,
                                action_list=None,
                                acl_priority=10,
                                addr_family=None,
                                ip_src=None,
                                ip_src_mask=None,
                                ip_dst=None,
                                ip_dst_mask=None,
                                ip_proto=None,
                                ip_proto_mask=None,
                                in_ports=None,
                                out_ports=None,
                                in_port=None,
                                out_port=None,
                                range_list=None,
                                packet_action=None,
                                ingress_mirror_id=None,
                                egress_mirror_id=None,
                                acl_counter_id=None,
                                policer_id=None,
                                ether_type=None,
                                ether_type_mask=None,
                                l4_src_port=None,
                                l4_src_port_mask=None,
                                l4_dst_port=None,
                                l4_dst_port_mask=None,
                                tunnel_vni=None,
                                tunnel_vni_mask=None,
                                inner_ether_type=None,
                                inner_ether_type_mask=None,
                                inner_src_ip=None,
                                inner_src_ip_mask=None,
                                inner_dst_ip=None,
                                inner_dst_ip_mask=None,
                                inner_ip_proto=None,
                                inner_ip_proto_mask=None,
                                inner_l4_src_port=None,
                                inner_l4_src_port_mask=None,
                                inner_l4_dst_port=None,
                                inner_l4_dst_port_mask=None,
                                dtel_int_session=None,
                                dtel_int_enable=None,
                                dtel_postcard_enable=None,
                                dtel_mod_enable=None,
                                dtel_report_all=None,
                                outer_vlan_id=None,
                                outer_vlan_id_mask=None,
                                admin_state=True):
    acl_attr_list = []

    #OID
    attribute_value = sai_thrift_attribute_value_t(oid=acl_table_id)
    attribute = sai_thrift_attribute_t(
        id=SAI_ACL_ENTRY_ATTR_TABLE_ID, value=attribute_value)
    acl_attr_list.append(attribute)

    #Priority
    attribute_value = sai_thrift_attribute_value_t(u32=acl_priority)
    attribute = sai_thrift_attribute_t(
        id=SAI_ACL_ENTRY_ATTR_PRIORITY, value=attribute_value)
    acl_attr_list.append(attribute)

    #Ip source
    if ip_src != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(ip4=ip_src),
                mask=sai_thrift_acl_mask_t(ip4=ip_src_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_SRC_IP, value=attribute_value)
        acl_attr_list.append(attribute)

    if ip_dst != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(ip4=ip_dst),
                mask=sai_thrift_acl_mask_t(ip4=ip_dst_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_DST_IP, value=attribute_value)
        acl_attr_list.append(attribute)

    if ip_proto != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u8=ip_proto),
                mask=sai_thrift_acl_mask_t(u8=ip_proto_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_IP_PROTOCOL, value=attribute_value)
        acl_attr_list.append(attribute)

    #Input ports
    if in_ports:
        acl_port_list = sai_thrift_object_list_t(
            count=len(in_ports), object_id_list=in_ports)
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                objlist=acl_port_list)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_IN_PORTS, value=attribute_value)
        acl_attr_list.append(attribute)

    #Output ports
    if out_ports:
        acl_port_list = sai_thrift_object_list_t(
            count=len(out_ports), object_id_list=out_ports)
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                objlist=acl_port_list)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_OUT_PORTS, value=attribute_value)
        acl_attr_list.append(attribute)

    if in_port != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                oid=in_port)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_IN_PORT, value=attribute_value)
        acl_attr_list.append(attribute)

    if out_port != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                oid=out_port)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_OUT_PORT, value=attribute_value)
        acl_attr_list.append(attribute)

    if range_list != None:
        acl_range_list = sai_thrift_object_list_t(
            count=len(range_list), object_id_list=range_list)
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                objlist=acl_range_list)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_ACL_RANGE_TYPE, value=attribute_value)
        acl_attr_list.append(attribute)

    if ether_type != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u16=ether_type),
                mask=sai_thrift_acl_mask_t(u16=ether_type_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_ETHER_TYPE, value=attribute_value)
        acl_attr_list.append(attribute)

    if l4_src_port != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u16=l4_src_port),
                mask=sai_thrift_acl_mask_t(u16=l4_src_port_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_L4_SRC_PORT, value=attribute_value)
        acl_attr_list.append(attribute)

    if l4_dst_port != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u16=l4_dst_port),
                mask=sai_thrift_acl_mask_t(u16=l4_dst_port_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_L4_DST_PORT, value=attribute_value)
        acl_attr_list.append(attribute)

    if tunnel_vni != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u16=tunnel_vni),
                mask=sai_thrift_acl_mask_t(u16=tunnel_vni_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_TUNNEL_VNI, value=attribute_value)
        acl_attr_list.append(attribute)

    if inner_ether_type != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u16=inner_ether_type),
                mask=sai_thrift_acl_mask_t(u16=inner_ether_type_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_INNER_ETHER_TYPE, value=attribute_value)
        acl_attr_list.append(attribute)

    if inner_src_ip != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(ip4=inner_src_ip),
                mask=sai_thrift_acl_mask_t(ip4=inner_src_ip_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_INNER_SRC_IP, value=attribute_value)
        acl_attr_list.append(attribute)

    if inner_dst_ip != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(ip4=inner_dst_ip),
                mask=sai_thrift_acl_mask_t(ip4=inner_dst_ip_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_INNER_DST_IP, value=attribute_value)
        acl_attr_list.append(attribute)

    if inner_ip_proto != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u8=inner_ip_proto),
                mask=sai_thrift_acl_mask_t(u8=inner_ip_proto_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_INNER_IP_PROTOCOL, value=attribute_value)
        acl_attr_list.append(attribute)

    if inner_l4_src_port != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u16=inner_l4_src_port),
                mask=sai_thrift_acl_mask_t(u16=inner_l4_src_port_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_INNER_L4_SRC_PORT, value=attribute_value)
        acl_attr_list.append(attribute)

    if inner_l4_dst_port != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u16=inner_l4_dst_port),
                mask=sai_thrift_acl_mask_t(u16=inner_l4_dst_port_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_INNER_L4_DST_PORT, value=attribute_value)
        acl_attr_list.append(attribute)

    if outer_vlan_id != None:
        attribute_value = sai_thrift_attribute_value_t(
            aclfield=sai_thrift_acl_field_data_t(
                data=sai_thrift_acl_data_t(u16=outer_vlan_id),
                mask=sai_thrift_acl_mask_t(u16=outer_vlan_id_mask)))
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_FIELD_OUTER_VLAN_ID, value=attribute_value)
        acl_attr_list.append(attribute)

    if admin_state:
        attribute_value = sai_thrift_attribute_value_t(booldata=admin_state)
        attribute = sai_thrift_attribute_t(
            id=SAI_ACL_ENTRY_ATTR_ADMIN_STATE, value=attribute_value)
        acl_attr_list.append(attribute)

    #Packet action
    for action in action_list:
        if action == SAI_ACL_ENTRY_ATTR_ACTION_PACKET_ACTION:
            #Drop
            attribute_value = sai_thrift_attribute_value_t(
                aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                    s32=packet_action)))
            attribute = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_ACTION_PACKET_ACTION,
                value=attribute_value)
            acl_attr_list.append(attribute)

        elif action == SAI_ACL_ENTRY_ATTR_ACTION_MIRROR_INGRESS:
            #Ingress mirroring
            attribute_value = sai_thrift_attribute_value_t(
                aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                    oid=ingress_mirror_id)))
            attribute = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_ACTION_MIRROR_INGRESS,
                value=attribute_value)
            acl_attr_list.append(attribute)

        elif action == SAI_ACL_ENTRY_ATTR_ACTION_MIRROR_EGRESS:
            #Egress mirroring
            attribute_value = sai_thrift_attribute_value_t(
                aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                    oid=egress_mirror_id)))
            attribute = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_ACTION_MIRROR_EGRESS,
                value=attribute_value)
            acl_attr_list.append(attribute)
        elif action == SAI_ACL_ENTRY_ATTR_ACTION_COUNTER:
            attribute_value = sai_thrift_attribute_value_t(
                aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                    oid=acl_counter_id)))
            attribute = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_ACTION_COUNTER, value=attribute_value)
            acl_attr_list.append(attribute)
        elif action == SAI_ACL_ENTRY_ATTR_ACTION_SET_POLICER:
            attribute_value = sai_thrift_attribute_value_t(
                aclfield=sai_thrift_acl_field_data_t(data=sai_thrift_acl_data_t(
                    oid=policer_id)))
            attribute = sai_thrift_attribute_t(
                id=SAI_ACL_ENTRY_ATTR_ACTION_SET_POLICER, value=attribute_value)
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


def sai_thrift_create_acl_range(client, range_type, range_value):
    attr_list = []

    attribute1_value = sai_thrift_attribute_value_t(s32=range_type)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_ACL_RANGE_ATTR_TYPE, value=attribute1_value)
    attr_list.append(attribute1)

    attribute2_value = sai_thrift_attribute_value_t(u32range=range_value)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_ACL_RANGE_ATTR_LIMIT, value=attribute2_value)
    attr_list.append(attribute2)

    acl_range_id = client.sai_thrift_create_acl_range(attr_list)
    return acl_range_id


def sai_thrift_create_mirror_session(
        client, mirror_type, port, vlan, vlan_priority, vlan_tpid, src_mac,
        dst_mac, addr_family, src_ip, dst_ip, encap_type, protocol, ttl, tos,
        vr_id, erspan_id=0, truncate_size=0):
    mirror_attr_list = []

    #Mirror type
    attribute1_value = sai_thrift_attribute_value_t(s32=mirror_type)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_MIRROR_SESSION_ATTR_TYPE, value=attribute1_value)
    mirror_attr_list.append(attribute1)

    #Monitor port
    attribute2_value = sai_thrift_attribute_value_t(oid=port)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_MIRROR_SESSION_ATTR_MONITOR_PORT, value=attribute2_value)
    mirror_attr_list.append(attribute2)

    if mirror_type == SAI_MIRROR_SESSION_TYPE_LOCAL:
        attribute4_value = sai_thrift_attribute_value_t(u16=vlan)
        attribute4 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_VLAN_ID, value=attribute4_value)
        mirror_attr_list.append(attribute4)
    elif mirror_type == SAI_MIRROR_SESSION_TYPE_REMOTE:
        #vlan tpid
        attribute3_value = sai_thrift_attribute_value_t(u16=vlan_tpid)
        attribute3 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_VLAN_TPID, value=attribute3_value)
        mirror_attr_list.append(attribute3)

        #vlan
        attribute4_value = sai_thrift_attribute_value_t(u16=vlan)
        attribute4 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_VLAN_ID, value=attribute4_value)
        mirror_attr_list.append(attribute4)

        #vlan priority
        attribute5_value = sai_thrift_attribute_value_t(u16=vlan_priority)
        attribute4 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_VLAN_PRI, value=attribute5_value)
        mirror_attr_list.append(attribute5)
    elif mirror_type == SAI_MIRROR_SESSION_TYPE_ENHANCED_REMOTE:
        #encap type
        attribute3_value = sai_thrift_attribute_value_t(s32=encap_type)
        attribute3 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_ERSPAN_ENCAPSULATION_TYPE,
            value=attribute3_value)
        mirror_attr_list.append(attribute3)

        #source ip
        addr = sai_thrift_ip_t(ip4=src_ip)
        src_ip_addr = sai_thrift_ip_address_t(
            addr_family=addr_family, addr=addr)
        attribute4_value = sai_thrift_attribute_value_t(ipaddr=src_ip_addr)
        attribute4 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_SRC_IP_ADDRESS, value=attribute4_value)
        mirror_attr_list.append(attribute4)

        #dst ip
        addr = sai_thrift_ip_t(ip4=dst_ip)
        dst_ip_addr = sai_thrift_ip_address_t(
            addr_family=addr_family, addr=addr)
        attribute5_value = sai_thrift_attribute_value_t(ipaddr=dst_ip_addr)
        attribute5 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_DST_IP_ADDRESS, value=attribute5_value)
        mirror_attr_list.append(attribute5)

        #source mac
        attribute6_value = sai_thrift_attribute_value_t(mac=src_mac)
        attribute6 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_SRC_MAC_ADDRESS, value=attribute6_value)
        mirror_attr_list.append(attribute6)

        #dst mac
        attribute7_value = sai_thrift_attribute_value_t(mac=dst_mac)
        attribute7 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_DST_MAC_ADDRESS, value=attribute7_value)
        mirror_attr_list.append(attribute7)

        #vrf id
        attribute8_value = sai_thrift_attribute_value_t(oid=vr_id)
        attribute8 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_CUSTOM_VR_ID, value=attribute8_value)
        mirror_attr_list.append(attribute8)

        #span id
        attribute9_value = sai_thrift_attribute_value_t(u16=erspan_id)
        attribute9 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_CUSTOM_ERSPAN_ID, value=attribute9_value)
        mirror_attr_list.append(attribute9)

        #truncate size
        attribute10_value = sai_thrift_attribute_value_t(u16=truncate_size)
        attribute10 = sai_thrift_attribute_t(
            id=SAI_MIRROR_SESSION_ATTR_TRUNCATE_SIZE, value=attribute10_value)
        mirror_attr_list.append(attribute10)

    mirror_id = client.sai_thrift_create_mirror_session(mirror_attr_list)
    return mirror_id

def sai_thrift_set_mirror_session(client, mirror_id, attr_id, attr_value):
    if attr_id == SAI_MIRROR_SESSION_ATTR_MONITOR_PORT:
        attribute_value = sai_thrift_attribute_value_t(oid = attr_value)
    elif attr_id == SAI_MIRROR_SESSION_ATTR_VLAN_ID:
        attribute_value = sai_thrift_attribute_value(u16 = attr_value)
    else:
        attribute_value = sai_thrift_attribute_value(oid = attr_value)
    attribute = sai_thrift_attribute_t(id = attr_id, value=attribute_value)
    client.sai_thrift_set_mirror_session(mirror_id, attribute)

def sai_thrift_get_mirror_session(client, mirror_id, attr_id):
    attribute = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_mirror_session(mirror_id, attribute)


def sai_thrift_get_vlan_stats(client, vlan_id, ingress=True, egress=True):
    counter_ids = []
    if ingress:
        counter_ids.append(SAI_VLAN_STAT_IN_OCTETS)
        counter_ids.append(SAI_VLAN_STAT_IN_PACKETS)
        counter_ids.append(SAI_VLAN_STAT_IN_UCAST_PKTS)
        counter_ids.append(SAI_VLAN_STAT_IN_NON_UCAST_PKTS)
        counter_ids.append(SAI_VLAN_STAT_IN_DISCARDS)
        counter_ids.append(SAI_VLAN_STAT_IN_ERRORS)
        counter_ids.append(SAI_VLAN_STAT_IN_UNKNOWN_PROTOS)
    if egress:
        counter_ids.append(SAI_VLAN_STAT_OUT_OCTETS)
        counter_ids.append(SAI_VLAN_STAT_OUT_PACKETS)
        counter_ids.append(SAI_VLAN_STAT_OUT_UCAST_PKTS)
        counter_ids.append(SAI_VLAN_STAT_OUT_NON_UCAST_PKTS)
        counter_ids.append(SAI_VLAN_STAT_OUT_DISCARDS)
        counter_ids.append(SAI_VLAN_STAT_OUT_ERRORS)
        counter_ids.append(SAI_VLAN_STAT_OUT_QLEN)

    number_of_counters = len(counter_ids)
    counters = client.sai_thrift_get_vlan_stats(vlan_id, counter_ids,
                                                number_of_counters)
    return counter_ids, counters


def sai_thrift_print_vlan_stats(counter_ids, counter):
    if SAI_VLAN_STAT_IN_OCTETS in counter_ids:
        print ("In octets: ", counter[SAI_VLAN_STAT_IN_OCTETS])
    if SAI_VLAN_STAT_IN_UCAST_PKTS in counter_ids:
        print ("In ucast pkts: ", counter[SAI_VLAN_STAT_IN_UCAST_PKTS])
    if SAI_VLAN_STAT_IN_NON_UCAST_PKTS in counter_ids:
        print ("In non ucast pkts: ", counter[SAI_VLAN_STAT_IN_NON_UCAST_PKTS])
    if SAI_VLAN_STAT_IN_DISCARDS in counter_ids:
        print ("In discards: ", counter[SAI_VLAN_STAT_IN_DISCARDS])
    if SAI_VLAN_STAT_IN_ERRORS in counter_ids:
        print ("In errors: ", counter[SAI_VLAN_STAT_IN_ERRORS])
    if SAI_VLAN_STAT_IN_UNKNOWN_PROTOS in counter_ids:
        print ("In unknown protos: ", counter[SAI_VLAN_STAT_IN_UNKNOWN_PROTOS])

    if SAI_VLAN_STAT_OUT_OCTETS in counter_ids:
        print ("Out octets: ", counter[SAI_VLAN_STAT_OUT_OCTETS])
    if SAI_VLAN_STAT_OUT_UCAST_PKTS in counter_ids:
        print ("Out ucast pkts: ", counter[SAI_VLAN_STAT_OUT_UCAST_PKTS])
    if SAI_VLAN_STAT_OUT_NON_UCAST_PKTS in counter_ids:
        print ("Out non ucast pkts: ", counter[SAI_VLAN_STAT_OUT_NON_UCAST_PKTS])
    if SAI_VLAN_STAT_OUT_DISCARDS in counter_ids:
        print ("Out discards: ", counter[SAI_VLAN_STAT_OUT_DISCARDS])
    if SAI_VLAN_STAT_OUT_ERRORS in counter_ids:
        print ("Out errors: ", counter[SAI_VLAN_STAT_OUT_ERRORS])
    if SAI_VLAN_STAT_OUT_QLEN in counter_ids:
        print ("Out qlen: ", counter[SAI_VLAN_STAT_OUT_QLEN])


def sai_thrift_get_acl_counter_attribute(client, acl_counter_id):
    attr_list = []

    attribute_id1 = SAI_ACL_COUNTER_ATTR_PACKETS
    attr_list.append(attribute_id1)

    attribute_id2 = SAI_ACL_COUNTER_ATTR_BYTES
    attr_list.append(attribute_id2)

    attr_values = client.sai_thrift_get_acl_counter_attribute(acl_counter_id,
                                                              attr_list)
    return attr_values


def sai_thrift_create_policer(client,
                              meter_type=SAI_METER_TYPE_BYTES,
                              meter_mode=SAI_POLICER_MODE_TR_TCM,
                              color_source=SAI_POLICER_COLOR_SOURCE_BLIND,
                              cbs=0,
                              cir=0,
                              pbs=0,
                              pir=0,
                              green_action=SAI_PACKET_ACTION_FORWARD,
                              yellow_action=SAI_PACKET_ACTION_FORWARD,
                              red_action=SAI_PACKET_ACTION_FORWARD):

    attr_list = []

    attribute1_value = sai_thrift_attribute_value_t(s32=meter_type)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_METER_TYPE, value=attribute1_value)
    attr_list.append(attribute1)

    attribute2_value = sai_thrift_attribute_value_t(s32=meter_mode)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_MODE, value=attribute2_value)
    attr_list.append(attribute2)

    attribute3_value = sai_thrift_attribute_value_t(s32=color_source)
    attribute3 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_COLOR_SOURCE, value=attribute3_value)
    attr_list.append(attribute3)

    attribute4_value = sai_thrift_attribute_value_t(u64=cbs)
    attribute4 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_CBS, value=attribute4_value)
    attr_list.append(attribute4)

    attribute5_value = sai_thrift_attribute_value_t(u64=cir)
    attribute5 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_CIR, value=attribute5_value)
    attr_list.append(attribute5)

    attribute6_value = sai_thrift_attribute_value_t(u64=pbs)
    attribute6 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_PBS, value=attribute6_value)
    attr_list.append(attribute6)

    attribute7_value = sai_thrift_attribute_value_t(u64=pir)
    attribute7 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_PIR, value=attribute7_value)
    attr_list.append(attribute7)

    attribute8_value = sai_thrift_attribute_value_t(s32=green_action)
    attribute8 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_GREEN_PACKET_ACTION, value=attribute8_value)
    attr_list.append(attribute8)

    attribute9_value = sai_thrift_attribute_value_t(s32=yellow_action)
    attribute9 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_YELLOW_PACKET_ACTION, value=attribute9_value)
    attr_list.append(attribute9)

    attribute10_value = sai_thrift_attribute_value_t(s32=red_action)
    attribute10 = sai_thrift_attribute_t(
        id=SAI_POLICER_ATTR_RED_PACKET_ACTION, value=attribute10_value)
    attr_list.append(attribute10)

    policer_id = client.sai_thrift_create_policer(attr_list)
    return policer_id


def sai_thrift_get_policer_stats(client, policer_id):
    attr_list = []

    attr_list.append(SAI_POLICER_STAT_PACKETS)
    attr_list.append(SAI_POLICER_STAT_ATTR_BYTES)
    attr_list.append(SAI_POLICER_STAT_GREEN_PACKETS)
    attr_list.append(SAI_POLICER_STAT_GREEN_BYTES)
    attr_list.append(SAI_POLICER_STAT_YELLOW_PACKETS)
    attr_list.append(SAI_POLICER_STAT_YELLOW_BYTES)
    attr_list.append(SAI_POLICER_STAT_RED_PACKETS)
    attr_list.append(SAI_POLICER_STAT_RED_BYTES)

    attr_value_list = client.sai_thrift_get_policer_stats(policer_id, attr_list)
    return attr_value_list

def sai_thrift_get_policer_attribute(client, policer_id, attr_id):
    thrift_attr = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_policer_attribute(policer_id, thrift_attr)

def sai_thrift_get_qos_map_attribute(client, qos_map_id, attr_id, map_count):
    thrift_attr = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_qos_map(qos_map_id, map_count, thrift_attr)

def sai_thrift_remove_qos_map(client, qos_map_id):
    client.sai_thrift_remove_qos_map(qos_map_id)

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
            id == SAI_PORT_ATTR_INGRESS_MIRROR_SESSION):
        attribute_value = sai_thrift_attribute_value_t(oid=value)
        attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
        client.sai_thrift_set_port_attribute(port_id, attribute)
    #elif (id == SAI_PORT_ATTR_BIND_MODE):
    #    attribute_value = sai_thrift_attribute_value_t(u32=value)
    #    attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
    #    client.sai_thrift_set_port_attribute(port_id, attribute)
    elif (id == SAI_PORT_ATTR_DROP_UNTAGGED or
          id == SAI_PORT_ATTR_DROP_TAGGED):
        attribute_value = sai_thrift_attribute_value_t(booldata=value)
        attribute = sai_thrift_attribute_t(id=id, value=attribute_value)
        client.sai_thrift_set_port_attribute(port_id, attribute)


def sai_thrift_create_buffer_pool(client, pool_type, pool_size):
    attribute1_value = sai_thrift_attribute_value_t(u32=pool_size)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_BUFFER_POOL_ATTR_SIZE, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(u8=pool_type)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_BUFFER_POOL_ATTR_TYPE, value=attribute2_value)
    attr_list = [attribute1, attribute2]
    return client.sai_thrift_create_buffer_pool(attr_list)

def sai_thrift_remove_buffer_pool(client, pool_id):
    return client.sai_thrift_remove_buffer_pool(pool_id)

def sai_thrift_create_buffer_profile(client, pool_id, profile_size, dyn_thr):
    attribute1_value = sai_thrift_attribute_value_t(oid=pool_id)
    attribute1 = sai_thrift_attribute_t(
        id=SAI_BUFFER_PROFILE_ATTR_POOL_ID, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(u32=profile_size)
    attribute2 = sai_thrift_attribute_t(
        id=SAI_BUFFER_PROFILE_ATTR_BUFFER_SIZE, value=attribute2_value)
    attribute3_value = sai_thrift_attribute_value_t(u8=dyn_thr)
    attribute3 = sai_thrift_attribute_t(
        id=SAI_BUFFER_PROFILE_ATTR_SHARED_DYNAMIC_TH, value=attribute3_value)
    attribute4_value = sai_thrift_attribute_value_t(u32 = SAI_BUFFER_PROFILE_THRESHOLD_MODE_DYNAMIC)
    attribute4 = sai_thrift_attribute_t(
        id=SAI_BUFFER_PROFILE_ATTR_THRESHOLD_MODE, value = attribute4_value)

    attr_list = [attribute1, attribute2, attribute3, attribute4]
    return client.sai_thrift_create_buffer_profile(attr_list)

def sai_thrift_remove_buffer_profile(client, profile_id):
    return client.sai_thrift_remove_buffer_profile(profile_id)

def sai_thrift_get_buffer_pool_attribute(client, pool_id, attr_id):
    thrift_attr = sai_thrift_attribute_t(id = attr_id)
    return client.sai_thrift_get_buffer_pool_attribute(pool_id, thrift_attr);

def sai_thrift_create_scheduler_profile(client, prio, weight, max_rate, max_burst, min_rate, min_burst):
    if prio:
        attribute1_value = sai_thrift_attribute_value_t(u8=SAI_SCHEDULING_TYPE_STRICT)
    else:
        attribute1_value = sai_thrift_attribute_value_t(u8=SAI_SCHEDULING_TYPE_DWRR)
    attribute1 = sai_thrift_attribute_t(id=SAI_SCHEDULER_ATTR_SCHEDULING_TYPE, value = attribute1_value)

    attribute2_value = sai_thrift_attribute_value_t(u8 = weight)
    attribute2 = sai_thrift_attribute_t(id = SAI_SCHEDULER_ATTR_SCHEDULING_WEIGHT, value = attribute2_value)

    attribute3_value = sai_thrift_attribute_value_t(u8 = SAI_METER_TYPE_BYTES)
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

def sai_thrift_create_wred_profile(client, attr_list):
    thrift_attr_list = []
    for attr_id, attr_values in six.iteritems(attr_list):
        attribute_value = sai_thrift_attribute_value_t(u32 = attr_values[1])
        attribute = sai_thrift_attribute_t(id = attr_id, value = attribute_value)
        thrift_attr_list.append(attribute)

    return client.sai_thrift_create_wred_profile(thrift_attr_list)
