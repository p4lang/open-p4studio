# Copyright 2013-present Barefoot Networks, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Thrift SAI interface basic tests
"""

import time
import logging

import ptf.dataplane as dataplane
import sai_base_test

from ptf.testutils import *

from switch_sai_thrift.ttypes import *

from ptf.mask import Mask

switch_inited = 0
port_list = []
table_attr_list = []


def verify_packet_list_any(test, pkt_list, ofport_list):
    logging.debug("Checking for packet on given ports")
    (rcv_device, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(timeout=1)
    test.assertTrue(rcv_pkt != None, "No packet received")

    i = 0
    match_found = 0
    for ofport in ofport_list:
        pkt = pkt_list[i]
        if (str(rcv_pkt) == str(pkt)) and (ofport == rcv_port):
            match_index = i
            match_found = 1
        i = i + 1
    test.assertTrue(match_found == 1, "Packet not received on expected port")
    return match_index


def switch_init(client):
    global switch_inited
    if switch_inited:
        return

    switch_attr_list = client.sai_thrift_get_switch_attribute()
    attr_list = switch_attr_list.attr_list
    for attribute in attr_list:
        if attribute.id == 0:
            print("max ports: " + attribute.value.u32)
        elif attribute.id == 1:
            for x in attribute.value.objlist.object_id_list:
                port_list.append(x)
        else:
            print("unknown switch attribute")

    attr_value = sai_thrift_attribute_value_t(mac="00:77:66:55:44:33")
    attr = sai_thrift_attribute_t(id=22, value=attr_value)
    client.sai_thrift_set_switch_attribute(attr)
    switch_inited = 1


def sai_thrift_create_fdb(client, vlan_id, mac, port, mac_action):
    fdb_entry = sai_thrift_fdb_entry_t(mac_address=mac, vlan_id=vlan_id)
    # value 0 represents static entry, id=0, represents entry type
    fdb_attribute1_value = sai_thrift_attribute_value_t(u8=1)
    fdb_attribute1 = sai_thrift_attribute_t(id=0, value=fdb_attribute1_value)
    # value oid represents object id, id=1 represents port id
    fdb_attribute2_value = sai_thrift_attribute_value_t(oid=port)
    fdb_attribute2 = sai_thrift_attribute_t(id=1, value=fdb_attribute2_value)
    # value oid represents object id, id=1 represents port id
    fdb_attribute3_value = sai_thrift_attribute_value_t(u8=mac_action)
    fdb_attribute3 = sai_thrift_attribute_t(id=2, value=fdb_attribute3_value)
    fdb_attr_list = [fdb_attribute1, fdb_attribute2, fdb_attribute3]
    client.sai_thrift_create_fdb_entry(
        thrift_fdb_entry=fdb_entry, thrift_attr_list=fdb_attr_list
    )


def sai_thrift_delete_fdb(client, vlan_id, mac, port):
    fdb_entry = sai_thrift_fdb_entry_t(mac_address=mac, vlan_id=vlan_id)
    client.sai_thrift_delete_fdb_entry(thrift_fdb_entry=fdb_entry)


def sai_thrift_create_virtual_router(client, v4_enabled, v6_enabled):
    # v4 enabled
    vr_attribute1_value = sai_thrift_attribute_value_t(booldata=v4_enabled)
    vr_attribute1 = sai_thrift_attribute_t(id=0, value=vr_attribute1_value)
    # v6 enabled
    vr_attribute2_value = sai_thrift_attribute_value_t(booldata=v6_enabled)
    vr_attribute2 = sai_thrift_attribute_t(id=1, value=vr_attribute1_value)
    vr_attr_list = [vr_attribute1, vr_attribute2]
    vr_id = client.sai_thrift_create_virtual_router(thrift_attr_list=vr_attr_list)
    return vr_id


def sai_thrift_create_router_interface(
    client, vr_id, is_port, port_id, vlan_id, v4_enabled, v6_enabled, mac
):
    # vrf attribute
    rif_attribute1_value = sai_thrift_attribute_value_t(oid=vr_id)
    rif_attribute1 = sai_thrift_attribute_t(id=0, value=rif_attribute1_value)
    if is_port:
        # port type and port id
        rif_attribute2_value = sai_thrift_attribute_value_t(u8=0)
        rif_attribute2 = sai_thrift_attribute_t(id=1, value=rif_attribute2_value)
        rif_attribute3_value = sai_thrift_attribute_value_t(oid=port_id)
        rif_attribute3 = sai_thrift_attribute_t(id=2, value=rif_attribute3_value)
    else:
        # vlan type and vlan id
        rif_attribute2_value = sai_thrift_attribute_value_t(u8=1)
        rif_attribute2 = sai_thrift_attribute_t(id=1, value=rif_attribute2_value)
        rif_attribute3_value = sai_thrift_attribute_value_t(u16=vlan_id)
        rif_attribute3 = sai_thrift_attribute_t(id=3, value=rif_attribute3_value)

    # v4_enabled
    rif_attribute4_value = sai_thrift_attribute_value_t(booldata=v4_enabled)
    rif_attribute4 = sai_thrift_attribute_t(id=5, value=rif_attribute4_value)
    # v6_enabled
    rif_attribute5_value = sai_thrift_attribute_value_t(booldata=v6_enabled)
    rif_attribute5 = sai_thrift_attribute_t(id=6, value=rif_attribute5_value)

    if mac:
        rif_attribute6_value = sai_thrift_attribute_value_t(mac=mac)
        rif_attribute6 = sai_thrift_attribute_t(id=4, value=rif_attribute6_value)
        rif_attr_list = [
            rif_attribute1,
            rif_attribute2,
            rif_attribute3,
            rif_attribute4,
            rif_attribute5,
            rif_attribute6,
        ]
    else:
        rif_attr_list = [
            rif_attribute1,
            rif_attribute2,
            rif_attribute3,
            rif_attribute4,
            rif_attribute5,
        ]

    rif_id = client.sai_thrift_create_router_interface(rif_attr_list)
    return rif_id


def sai_thrift_create_route(client, vr_id, addr_family, ip_addr, ip_mask, nhop):
    if addr_family == 0:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        mask = sai_thrift_ip_t(ip4=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(addr_family=0, addr=addr, mask=mask)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        mask = sai_thrift_ip_t(ip6=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(addr_family=1, addr=addr, mask=mask)
    route_attribute1_value = sai_thrift_attribute_value_t(oid=nhop)
    route_attribute1 = sai_thrift_attribute_t(id=2, value=route_attribute1_value)
    route = sai_thrift_unicast_route_entry_t(vr_id, ip_prefix)
    route_attr_list = [route_attribute1]
    client.sai_thrift_create_route(
        thrift_unicast_route_entry=route, thrift_attr_list=route_attr_list
    )


def sai_thrift_remove_route(client, vr_id, addr_family, ip_addr, ip_mask, nhop):
    if addr_family == 0:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        mask = sai_thrift_ip_t(ip4=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(addr_family=0, addr=addr, mask=mask)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        mask = sai_thrift_ip_t(ip6=ip_mask)
        ip_prefix = sai_thrift_ip_prefix_t(addr_family=1, addr=addr, mask=mask)
    route = sai_thrift_unicast_route_entry_t(vr_id, ip_prefix)
    client.sai_thrift_remove_route(thrift_unicast_route_entry=route)


def sai_thrift_create_nhop(client, addr_family, ip_addr, rif_id):
    if addr_family == 0:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=0, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=1, addr=addr)
    nhop_attribute1_value = sai_thrift_attribute_value_t(ipaddr=ipaddr)
    nhop_attribute1 = sai_thrift_attribute_t(id=1, value=nhop_attribute1_value)
    nhop_attribute2_value = sai_thrift_attribute_value_t(oid=rif_id)
    nhop_attribute2 = sai_thrift_attribute_t(id=2, value=nhop_attribute2_value)
    nhop_attr_list = [nhop_attribute1, nhop_attribute2]
    nhop = client.sai_thrift_create_next_hop(thrift_attr_list=nhop_attr_list)
    return nhop


def sai_thrift_create_neighbor(client, addr_family, rif_id, ip_addr, dmac):
    if addr_family == 0:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=0, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=1, addr=addr)
    neighbor_attribute1_value = sai_thrift_attribute_value_t(mac=dmac)
    neighbor_attribute1 = sai_thrift_attribute_t(id=0, value=neighbor_attribute1_value)
    neighbor_attr_list = [neighbor_attribute1]
    neighbor_entry = sai_thrift_neighbor_entry_t(rif_id=rif_id, ip_address=ipaddr)
    client.sai_thrift_create_neighbor_entry(neighbor_entry, neighbor_attr_list)


def sai_thrift_remove_neighbor(client, addr_family, rif_id, ip_addr, dmac):
    if addr_family == 0:
        addr = sai_thrift_ip_t(ip4=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=0, addr=addr)
    else:
        addr = sai_thrift_ip_t(ip6=ip_addr)
        ipaddr = sai_thrift_ip_address_t(addr_family=1, addr=addr)
    neighbor_entry = sai_thrift_neighbor_entry_t(rif_id=rif_id, ip_address=ipaddr)
    client.sai_thrift_remove_neighbor_entry(neighbor_entry)


def sai_thrift_create_next_hop_group(client, nhop_list):
    nhop_group_attribute1_value = sai_thrift_attribute_value_t(u8=0)
    nhop_group_attribute1 = sai_thrift_attribute_t(
        id=1, value=nhop_group_attribute1_value
    )
    nhop_objlist = sai_thrift_object_list_t(
        count=len(nhop_list), object_id_list=nhop_list
    )
    nhop_group_attribute2_value = sai_thrift_attribute_value_t(objlist=nhop_objlist)
    nhop_group_attribute2 = sai_thrift_attribute_t(
        id=2, value=nhop_group_attribute2_value
    )
    nhop_group_attr_list = [nhop_group_attribute1, nhop_group_attribute2]
    nhop_group = client.sai_thrift_create_next_hop_group(
        thrift_attr_list=nhop_group_attr_list
    )
    return nhop_group


def sai_thrift_create_lag(client, port_list):
    lag_port_list = sai_thrift_object_list_t(
        count=len(port_list), object_id_list=port_list
    )
    lag1_attr_value = sai_thrift_attribute_value_t(objlist=lag_port_list)
    lag1_attr = sai_thrift_attribute_t(id=0, value=lag1_attr_value)
    lag_attr_list = [lag1_attr]
    lag = client.sai_thrift_create_lag(lag_attr_list)
    return lag


def sai_thrift_create_stp_entry(client, vlan_list):
    vlanlist = sai_thrift_vlan_list_t(vlan_count=len(vlan_list), vlan_list=vlan_list)
    stp_attribute1_value = sai_thrift_attribute_value_t(vlanlist=vlanlist)
    stp_attribute1 = sai_thrift_attribute_t(id=0, value=stp_attribute1_value)
    stp_attr_list = [stp_attribute1]
    stp_id = client.sai_thrift_create_stp_entry(stp_attr_list)
    return stp_id


def sai_thrift_create_hostif_trap_group(client, queue_id, priority):
    attribute1_value = sai_thrift_attribute_value_t(u32=priority)
    attribute1 = sai_thrift_attribute_t(id=1, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(u32=queue_id)
    attribute2 = sai_thrift_attribute_t(id=2, value=attribute2_value)
    attr_list = [attribute1, attribute2]
    trap_group_id = client.sai_thrift_create_hostif_trap_group(
        thrift_attr_list=attr_list
    )
    return trap_group_id


def sai_thrift_create_hostif_trap(
    client, trap_id, action, priority, channel, trap_group_id
):
    attribute3_value = sai_thrift_attribute_value_t(u32=channel)
    attribute3 = sai_thrift_attribute_t(id=2, value=attribute3_value)
    client.sai_thrift_set_hostif_trap(trap_id, attribute3)
    attribute4_value = sai_thrift_attribute_value_t(oid=trap_group_id)
    attribute4 = sai_thrift_attribute_t(id=5, value=attribute4_value)
    client.sai_thrift_set_hostif_trap(trap_id, attribute4)
    attribute1_value = sai_thrift_attribute_value_t(u32=action)
    attribute1 = sai_thrift_attribute_t(id=0, value=attribute1_value)
    client.sai_thrift_set_hostif_trap(trap_id, attribute1)
    attribute2_value = sai_thrift_attribute_value_t(u32=priority)
    attribute2 = sai_thrift_attribute_t(id=1, value=attribute2_value)
    client.sai_thrift_set_hostif_trap(trap_id, attribute2)


def sai_thrift_create_hostif(client, rif_or_port_id, intf_name):
    attribute1_value = sai_thrift_attribute_value_t(u32=0)
    attribute1 = sai_thrift_attribute_t(id=0, value=attribute1_value)
    attribute2_value = sai_thrift_attribute_value_t(oid=rif_or_port_id)
    attribute2 = sai_thrift_attribute_t(id=1, value=attribute2_value)
    attribute3_value = sai_thrift_attribute_value_t(chardata=intf_name)
    attribute3 = sai_thrift_attribute_t(id=2, value=attribute3_value)
    attr_list = [attribute1, attribute2, attribute3]
    hif_id = client.sai_thrift_create_hostif(attr_list)
    return hif_id


class L2AccessToAccessVlanTest(sai_base_test.SAIThriftDataplaneTest):
    def runTest(self):
        print()
        print("Sending L2 packet port 1 -> port 2 [access vlan=10])")
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[1]
        port2 = port_list[2]
        mac1 = "00:11:11:11:11:11"
        mac2 = "00:22:22:22:22:22"
        mac_action = 1

        self.client.sai_thrift_create_vlan(vlan_id)
        vlan_port1 = sai_thrift_vlan_port_t(port_id=port1, tagging_mode=0)
        vlan_port2 = sai_thrift_vlan_port_t(port_id=port2, tagging_mode=0)
        self.client.sai_thrift_add_ports_to_vlan(vlan_id, [vlan_port1, vlan_port2])

        sai_thrift_create_fdb(self.client, vlan_id, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_id, mac2, port2, mac_action)

        pkt = simple_tcp_packet(
            eth_dst="00:11:11:11:11:11",
            eth_src="00:22:22:22:22:22",
            ip_dst="10.0.0.1",
            ip_id=101,
            ip_ttl=64,
        )

        try:
            # in tuple: 0 is device number, 2 is port number
            # this tuple uniquely identifies a port
            send_packet(self, (0, 2), pkt)
            verify_packets(self, pkt, device_number=0, ports=[1])
            # or simply
            # send_packet(self, 2, pkt)
            # verify_packets(self, pkt, ports=[1])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_id, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_id, mac2, port2)

            self.client.sai_thrift_remove_ports_from_vlan(
                vlan_id, [vlan_port1, vlan_port2]
            )
            self.client.sai_thrift_delete_vlan(vlan_id)


# illustrates how to put test in a group
@group("group_1")
# illustrates how to disable a test
@disabled
class L2AccessToTrunkVlanTest(sai_base_test.SAIThriftDataplaneTest):
    def runTest(self):
        print("Sending L2 packet - port 1 -> port 2 [trunk vlan=10])")
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[1]
        port2 = port_list[2]
        mac1 = "00:11:11:11:11:11"
        mac2 = "00:22:22:22:22:22"
        mac_action = 1

        self.client.sai_thrift_create_vlan(vlan_id)
        vlan_port1 = sai_thrift_vlan_port_t(port_id=port1, tagging_mode=1)
        vlan_port2 = sai_thrift_vlan_port_t(port_id=port2, tagging_mode=0)
        self.client.sai_thrift_add_ports_to_vlan(vlan_id, [vlan_port1, vlan_port2])

        sai_thrift_create_fdb(self.client, vlan_id, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_id, mac2, port2, mac_action)

        pkt = simple_tcp_packet(
            eth_dst="00:11:11:11:11:11",
            eth_src="00:22:22:22:22:22",
            ip_dst="10.0.0.1",
            ip_id=102,
            ip_ttl=64,
        )
        exp_pkt = simple_tcp_packet(
            eth_dst="00:11:11:11:11:11",
            eth_src="00:22:22:22:22:22",
            ip_dst="10.0.0.1",
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=102,
            ip_ttl=64,
            pktlen=104,
        )
        try:
            send_packet(self, 2, pkt)
            verify_packets(self, exp_pkt, [1])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_id, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_id, mac2, port2)

            self.client.sai_thrift_remove_ports_from_vlan(
                vlan_id, [vlan_port1, vlan_port2]
            )
            self.client.sai_thrift_delete_vlan(vlan_id)


@group("group_1")
class L2AccessToTrunkVlanTest_Mask(sai_base_test.SAIThriftDataplaneTest):
    def runTest(self):
        print("Sending L2 packet - port 1 -> port 2 [trunk vlan=10])")
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[1]
        port2 = port_list[2]
        mac1 = "00:11:11:11:11:11"
        mac2 = "00:22:22:22:22:22"
        mac_action = 1

        self.client.sai_thrift_create_vlan(vlan_id)
        vlan_port1 = sai_thrift_vlan_port_t(port_id=port1, tagging_mode=1)
        vlan_port2 = sai_thrift_vlan_port_t(port_id=port2, tagging_mode=0)
        self.client.sai_thrift_add_ports_to_vlan(vlan_id, [vlan_port1, vlan_port2])

        sai_thrift_create_fdb(self.client, vlan_id, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_id, mac2, port2, mac_action)

        pkt = simple_tcp_packet(
            eth_dst="00:11:11:11:11:11",
            eth_src="00:22:22:22:22:22",
            ip_dst="10.0.0.1",
            ip_id=102,
            ip_ttl=64,
        )
        exp_pkt = simple_tcp_packet(
            eth_dst="00:11:11:11:11:11",
            eth_src="00:22:22:22:22:22",
            ip_dst="10.0.0.1",
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=102,
            ip_ttl=64,
            pktlen=104,
        )

        # illustrates how to use a mask even if no impact here
        m = Mask(exp_pkt)
        m.set_do_not_care_packet(IP, "ttl")
        try:
            send_packet(self, 2, pkt)
            verify_packets(self, m, [1])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_id, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_id, mac2, port2)

            self.client.sai_thrift_remove_ports_from_vlan(
                vlan_id, [vlan_port1, vlan_port2]
            )
            self.client.sai_thrift_delete_vlan(vlan_id)


@group("group_1")
@group("group_2")
class L2TrunkToAccessVlanTest(sai_base_test.SAIThriftDataplaneTest):
    def runTest(self):
        print()
        print("Sending L2 packet - port 1 -> port 2 [trunk vlan=10])")
        switch_init(self.client)
        vlan_id = 10
        port1 = port_list[1]
        port2 = port_list[2]
        mac1 = "00:11:11:11:11:11"
        mac2 = "00:22:22:22:22:22"
        mac_action = 1

        self.client.sai_thrift_create_vlan(vlan_id)
        vlan_port1 = sai_thrift_vlan_port_t(port_id=port1, tagging_mode=0)
        vlan_port2 = sai_thrift_vlan_port_t(port_id=port2, tagging_mode=1)
        self.client.sai_thrift_add_ports_to_vlan(vlan_id, [vlan_port1, vlan_port2])

        sai_thrift_create_fdb(self.client, vlan_id, mac1, port1, mac_action)
        sai_thrift_create_fdb(self.client, vlan_id, mac2, port2, mac_action)

        pkt = simple_tcp_packet(
            eth_dst="00:11:11:11:11:11",
            eth_src="00:22:22:22:22:22",
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_dst="10.0.0.1",
            ip_id=102,
            ip_ttl=64,
        )
        exp_pkt = simple_tcp_packet(
            eth_dst="00:11:11:11:11:11",
            eth_src="00:22:22:22:22:22",
            ip_dst="10.0.0.1",
            ip_id=102,
            ip_ttl=64,
            pktlen=96,
        )
        try:
            send_packet(self, 2, pkt)
            verify_packets(self, exp_pkt, [1])
        finally:
            sai_thrift_delete_fdb(self.client, vlan_id, mac1, port1)
            sai_thrift_delete_fdb(self.client, vlan_id, mac2, port2)

            self.client.sai_thrift_remove_ports_from_vlan(
                vlan_id, [vlan_port1, vlan_port2]
            )
            self.client.sai_thrift_delete_vlan(vlan_id)
