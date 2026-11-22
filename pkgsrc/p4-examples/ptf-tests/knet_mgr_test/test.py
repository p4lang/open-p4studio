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
Thrift PD interface basic tests
"""
import time
from time import sleep
import unittest
import random
import six
import select
import socket
import subprocess
import pd_base_tests

from ptf import config
from ptf.packet import Ether, Packet
from ptf.testutils import *
from ptf.thriftutils import *

if config.get("packet_manipulation_module") == "bf_pktpy.ptf.packet_pktpy":
    from bf_pktpy.library.fields import BitField, XByteField, XShortField
else:
    from scapy.fields import BitField, XByteField, XShortField

from knet_mgr_test.p4_pd_rpc.ttypes import *
from knet_mgr_pd_rpc.ttypes import *
from pkt_pd_rpc.ttypes import *
from mc_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *

ETH_P_ALL = 3
swports = [x for x in range(65)]
dev_id = 0


class Mask(Packet):
    name = "Filter Mask"
    fields_desc = [
        BitField("dest_mac", 0, 48),
        BitField("src_mac", 0, 48),
        XShortField("outer_ether_type", 0),
        BitField("packet_type", 0, 3),
        BitField("header_version", 0, 2),
        BitField("packet_version", 0, 2),
        BitField("pad1", 0, 1),
        BitField("fabric_color", 0, 3),
        BitField("fabric_qos", 0, 5),
        XByteField("dst_device", 0),
        XShortField("dst_port_or_group", 0),
        BitField("egress_queue", 0, 5), BitField("tx_bypass", 0, 1),
        BitField("reserved1", 0, 2), XShortField("ingress_port", 0),
        XShortField("ingress_ifindex", 0), XShortField("ingress_bd", 0),
        XShortField("reason_code", 0),
        XShortField("inner_ether_type", 0),
    ]


class FabricHeader(Packet):
    name = "Fabric Header"
    fields_desc = [
        BitField("packet_type", 0, 3),
        BitField("header_version", 0, 2),
        BitField("packet_version", 0, 2),
        BitField("pad1", 0, 1),
        BitField("fabric_color", 0, 3),
        BitField("fabric_qos", 0, 5),
        XByteField("dst_device", 0),
        XShortField("dst_port_or_group", 0),
    ]


class FabricCpuHeader(Packet):
    name = "Fabric Cpu Header"
    fields_desc = [
        BitField("egress_queue", 0, 5), BitField("tx_bypass", 0, 1),
        BitField("reserved1", 0, 2), XShortField("ingress_port", 0),
        XShortField("ingress_ifindex", 0), XShortField("ingress_bd", 0),
        XShortField("reason_code", 0)
    ]


class FabricPayloadHeader(Packet):
    name = "Fabric Payload Header"
    fields_desc = [XShortField("ether_type", 0)]


class FabricCpuSflowHeader(Packet):
    name = "Fabric Cpu Sflow Header"
    fields_desc = [
        XShortField("sflow_sid", 0),
    ]


class FabricCpuBfdEventHeader(Packet):
    name = "Fabric Cpu BFD Event Header"
    fields_desc = [
        XShortField("bfd_sid", 0),
        XShortField("bfd_event", 0),
    ]


class VlanHeader(Packet):
    name = "vlan Header"

    fields_desc = [
        XShortField("ethtype", 0x8100),
        BitField("pcp", 0, 3), BitField("dei", 0, 1),
        BitField("vlanid", 0, 12),
        XShortField("tpid", 0),
    ]


def simple_cpu_vlan_packet(header_version=0,
                           packet_version=0,
                           fabric_color=0,
                           fabric_qos=0,
                           dst_device=0,
                           dst_port_or_group=0,
                           ingress_ifindex=0,
                           ingress_bd=0,
                           egress_queue=0,
                           tx_bypass=False,
                           ingress_port=1,
                           reason_code=0,
                           sflow_sid=0,
                           bfd_sid=0,
                           inner_pkt=None, vid=0):

    ether = Ether(bytes(inner_pkt))
    eth_type = ether.type
    ether.type = 0x9000

    fabric_header = FabricHeader(
        packet_type=0x5,
        header_version=header_version,
        packet_version=packet_version,
        pad1=0,
        fabric_color=fabric_color,
        fabric_qos=fabric_qos,
        dst_device=dst_device,
        dst_port_or_group=dst_port_or_group)

    fabric_cpu_header = FabricCpuHeader(
        egress_queue=egress_queue,
        tx_bypass=tx_bypass,
        reserved1=0,
        ingress_port=ingress_port,
        ingress_ifindex=ingress_ifindex,
        ingress_bd=ingress_bd,
        reason_code=reason_code)

    fabric_payload_header = FabricPayloadHeader(ether_type=eth_type)

    outer = bytes(ether)[:14] + bytes(VlanHeader(vlanid=vid))
    pkt = fabric_header / fabric_cpu_header

    if sflow_sid:
        pkt = pkt / FabricCpuSflowHeader(sflow_sid=sflow_sid)
    elif bfd_sid:
        pkt = pkt / FabricCpuBfdEventHeader(bfd_sid=bfd_sid)

    pkt = pkt / fabric_payload_header

    if inner_pkt:
        pkt = pkt / (bytes(inner_pkt)[14:])
    else:
        ip_pkt = simple_ip_only_packet()
        pkt = pkt / ip_pkt

    return outer + bytes(pkt)


def simple_cpu_packet(header_version=0,
                      packet_version=0,
                      fabric_color=0,
                      fabric_qos=0,
                      dst_device=0,
                      dst_port_or_group=0,
                      ingress_ifindex=0,
                      ingress_bd=0,
                      egress_queue=0,
                      tx_bypass=False,
                      ingress_port=1,
                      reason_code=0,
                      sflow_sid=0,
                      bfd_sid=0,
                      inner_pkt=None):

    ether = Ether(bytes(inner_pkt))
    eth_type = ether.type
    ether.type = 0x9000

    fabric_header = FabricHeader(
        packet_type=0x5,
        header_version=header_version,
        packet_version=packet_version,
        pad1=0,
        fabric_color=fabric_color,
        fabric_qos=fabric_qos,
        dst_device=dst_device,
        dst_port_or_group=dst_port_or_group)

    fabric_cpu_header = FabricCpuHeader(
        egress_queue=egress_queue,
        tx_bypass=tx_bypass,
        reserved1=0,
        ingress_port=ingress_port,
        ingress_ifindex=ingress_ifindex,
        ingress_bd=ingress_bd,
        reason_code=reason_code)

    fabric_payload_header = FabricPayloadHeader(ether_type=eth_type)

    outer = bytes(ether)[:14]
    pkt = fabric_header / fabric_cpu_header

    if sflow_sid:
        pkt = pkt / FabricCpuSflowHeader(sflow_sid=sflow_sid)
    elif bfd_sid:
        pkt = pkt / FabricCpuBfdEventHeader(bfd_sid=bfd_sid)

    pkt = pkt / fabric_payload_header

    if inner_pkt:
        pkt = pkt / (bytes(inner_pkt)[14:])
    else:
        ip_pkt = simple_ip_only_packet()
        pkt = pkt / ip_pkt

    return outer + bytes(pkt)


def dumpObject(obj):
    for attr in dir(obj):
        if hasattr(obj, attr):
            print("obj.%s = %s" % (attr, getattr(obj, attr)))


def socket_verify_packet(pkt, s, timeout=2):
    MAX_PKT_SIZE = 9100
    expected = bytes(pkt)
    deadline = time.monotonic() + timeout
    while True:
        remaining = deadline - time.monotonic()
        if remaining <= 0 or not select.select([s], [], [], remaining)[0]:
            return False
        if s.recv(MAX_PKT_SIZE) == expected:
            return True


def generate_filter(dest_mac=-1,
                    src_mac=-1,
                    outer_ether_type=-1,
                    packet_type=-1,
                    header_version=-1,
                    packet_version=-1,
                    pad1=-1,
                    fabric_color=-1,
                    fabric_qos=-1,
                    dst_device=-1,
                    dst_port_or_group=-1,
                    egress_queue=-1,
                    tx_bypass=-1,
                    reserved1=-1,
                    ingress_port=-1,
                    ingress_ifindex=-1,
                    ingress_bd=-1,
                    reason_code=-1,
                    inner_ether_type=-1):
    kfilter = Mask()
    kmask = Mask()
    if dest_mac != -1:
        kmask.dest_mac = 0xffffffffffff
        kfilter.dest_mac = dest_mac
    if src_mac != -1:
        kmask.src_mac = 0xffffffffffff
        kfilter.src_mac = src_mac
    if outer_ether_type != -1:
        kmask.outer_ether_type = 0xffff
        kfilter.outer_ether_type = outer_ether_type
    if packet_type != -1:
        kmask.packet_type = 7
        kfilter.packet_type = packet_type
    if header_version != -1:
        kmask.header_version = 3
        kfilter.header_version = header_version
    if packet_version != -1:
        kmask.packet_version = 3
        kfilter.packet_version = packet_version
    if pad1 != -1:
        kmask.pad1 = 1
        kfilter.pad1 = pad1
    if fabric_color != -1:
        kmask.fabric_color = 7
        kfilter.fabric_color = fabric_color
    if fabric_qos != -1:
        kmask.fabric_qos = 31
        kfilter.fabric_qos = fabric_qos
    if dst_device != -1:
        kmask.dst_device = 0xff
        kfilter.dst_device = dst_device
    if dst_port_or_group != -1:
        kmask.dst_port_or_group = 0xffff
        kfilter.dst_port_or_group = dst_port_or_group
    if egress_queue != -1:
        kmask.egress_queue = 31
        kfilter.egress_queue = egress_queue
    if tx_bypass != -1:
        kmask.tx_bypass = 1
        kfilter.tx_bypass = tx_bypass
    if reserved1 != -1:
        kmask.reserved1 = 3
        kfilter.reserved1 = reserved1
    if ingress_port != -1:
        kmask.ingress_port = 0xffff
        kfilter.ingress_port = ingress_port
    if ingress_ifindex != -1:
        kmask.ingress_ifindex = 0xffff
        kfilter.ingress_ifindex = ingress_ifindex
    if ingress_bd != -1:
        kmask.ingress_bd = 0xffff
        kfilter.ingress_bd = ingress_bd
    if reason_code != -1:
        kmask.reason_code = 0xffff
        kfilter.reason_code = reason_code
    if inner_ether_type != -1:
        kmask.inner_ether_type = 0xffff
        kfilter.inner_ether_type = inneer_ether_type

    return bytes(kfilter), bytes(kmask)


def open_packet_socket(hostif_name):
    s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW,
                      socket.htons(ETH_P_ALL))
    s.bind((hostif_name, ETH_P_ALL))
    s.setblocking(0)
    return s


def port_tbl_add(self, shdl, dev_tgt,
                 port, pad, fab_color,
                 fab_qos, dst_dev,
                 dst_port_or_grp,
                 reserved, ifindex,
                 bd, reason):

    match_spec = knet_mgr_test_port_tbl_match_spec_t(hex_to_i16(port))
    action_spec = knet_mgr_test_add_cpu_header_action_spec_t(pad, 0,
                                                             fab_qos, dst_dev,
                                                             dst_port_or_grp,
                                                             reserved, ifindex,
                                                             bd, reason)
    entry_hdl = self.client.port_tbl_table_add_with_add_cpu_header(
        shdl, dev_tgt, match_spec, action_spec)
    return entry_hdl


def verify_filter(ufilter, filter_res):
    if (ufilter.spec.priority != filter_res.spec.priority or ufilter.spec.filter_size != filter_res.spec.filter_size or ufilter.action.dest_type != filter_res.action.dest_type or ufilter.action.dest_proto != filter_res.action.dest_proto or ufilter.action.count != filter_res.action.count):
        return False

    for x in range(filter_res.spec.filter_size):
        if (ufilter.spec.filter[x] != filter_res.spec.filter[x] or ufilter.spec.mask[x] != filter_res.spec.mask[x]):
            return False

    for umut, rmut in zip(ufilter.action.pkt_mutation, filter_res.action.pkt_mutation):
        if(umut.mutation_type != rmut.mutation_type or umut.offset != rmut.offset or umut.len != rmut.len):
            return False
        if(umut.mutation_type == knet_mutation_type_t.BF_KNET_RX_MUT_INSERT):
            for x in range(rmut.len):
                if (umut.data[x] != rmut.data[x]):
                    return False
    return True

def verify_action(uaction, action_res):
    if(uaction.count != action_res.count):
        return False
    for umut, rmut in zip(uaction.pkt_mutation, action_res.pkt_mutation):
        if(umut.mutation_type != rmut.mutation_type or umut.offset != rmut.offset or umut.len != rmut.len):
            return False
        if(umut.mutation_type == knet_mutation_type_t.BF_KNET_RX_MUT_INSERT):
            for x in range(rmut.len):
                if (umut.data[x] != rmut.data[x]):
                    return False
    return True

def verify_list(plist,list_res):
    diff = [i for i in plist + list_res if i not in plist or i not in list_res]
    if diff:
        return False
    return True

class TestKnet(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(
            self, ["knet_mgr_test"])

    def baseApiTest(self):

        # Add veth251 as knet cpu port
        cpuif_ndev = self.knet_mgr.knet_cpuif_ndev_add("veth251")
        self.assertTrue(cpuif_ndev.status == 0, )

        # Add test_hostif1 as hostif_knetdev
        hostif_ndev = self.knet_mgr.knet_hostif_kndev_add(
            cpuif_ndev.knet_cpuif_id, "test_hostif1")

        # Create Rx Filter to strip from offset 12,21 bytes
        filter1, mask1 = generate_filter(ingress_port=1)
        filter_spec = knet_rx_filter_spec_t(
            priority=1, filter=filter1, mask=mask1, filter_size=len(filter1))
        mutation_strip = knet_packet_mutation_t(
            mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_STRIP, offset=12, len=21)
        mutation_list1 = list()
        mutation_list1.append(mutation_strip)
        filter_action = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=1, pkt_mutation=mutation_list1)
        rx_filter = knet_rx_filter_t(filter_spec, filter_action)
        filter_res = self.knet_mgr.knet_rx_filter_add(
            cpuif_ndev.knet_cpuif_id, rx_filter)

        # Add Tx action for test_hostif1
        fab_hdr = FabricHeader()
        cpu_hdr = FabricCpuHeader(ingress_port=1)
        hdr = bytes(fab_hdr / cpu_hdr)
        mutation_insert = knet_packet_mutation_t(
            mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_INSERT, data=hdr, offset=12, len=len(hdr))
        mutation_list2 = list()
        mutation_list2.append(mutation_insert)
        tx_action = knet_tx_action_t(count=1, pkt_mutation=mutation_list2)
        action_res = self.knet_mgr.knet_tx_action_add(
            cpuif_ndev.knet_cpuif_id, hostif_ndev.knet_hostif_id, tx_action)

        # Delete rx fitler
        self.knet_mgr.knet_rx_filter_delete(
            cpuif_ndev.knet_cpuif_id, filter_res.filter_id)

        # Delete tx action
        self.knet_mgr.knet_tx_action_delete(
            cpuif_ndev.knet_cpuif_id, hostif_ndev.knet_hostif_id)

        # Delete hostif
        self.knet_mgr.knet_hostif_kndev_delete(
            cpuif_ndev.knet_cpuif_id, hostif_ndev.knet_hostif_id)

        # Delete cpuif
        self.knet_mgr.knet_cpuif_ndev_delete(cpuif_ndev.knet_cpuif_id)

    def configureTbl(self, sess_hdl, dev_id, port_tbl_handles):

        default_action_spec = knet_mgr_test_add_cpu_header_action_spec_t(0, 0,
                                                                         0, 0,
                                                                         0, 0, 0,
                                                                         0, 0)
        dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))
        port_tbl_hdl1 = self.client.port_tbl_set_default_action_add_cpu_header(
            sess_hdl, dev_tgt, default_action_spec)
        fab_tbl_hdl1 = self.client.fabric_tbl_set_default_action_set_egress_port(
            sess_hdl, dev_tgt)

        # For packet coming on port 0 set pad to 1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0)
        port_tbl_handles.append(entry_hdl)

        # For packet coming on port 1 set fab_qos to  1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0)
        port_tbl_handles.append(entry_hdl)

        # For packet coming on port 2 set dst_dev to  1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0)
        port_tbl_handles.append(entry_hdl)

        # For packet coming on port 3 set dst_port_or_grp to  1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0)
        port_tbl_handles.append(entry_hdl)

        # For packet coming on port 4 set reserved to  1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 4, 0, 0, 0, 0, 1, 0, 0, 0, 0)
        port_tbl_handles.append(entry_hdl)

        # For packet coming on port 5 set ifindex as  1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0)
        port_tbl_handles.append(entry_hdl)

        # For packet coming on port 6 set ifindex as  1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 6, 0, 0, 0, 0, 0, 0, 1, 0, 0)
        port_tbl_handles.append(entry_hdl)

        # For packet coming on port 7 set bd as  1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 7, 0, 0, 0, 0, 0, 0, 0, 1, 0)
        port_tbl_handles.append(entry_hdl)

        # For packet coming on port 8 set reasonCode as  1
        entry_hdl = port_tbl_add(
            self, sess_hdl, dev_tgt, 8, 0, 0, 0, 0, 0, 0, 0, 0, 1)
        port_tbl_handles.append(entry_hdl)

    def runTest(self):
        self.assertTrue(self.knet_mgr.knet_module_is_inited(),
                        "The bf_knet control interface must be available in the test namespace")
        self.baseApiTest()
        port_tbl_handles = []

        try:
            filter_list = []
            cpuif_list = []
            hostif_list = []
            sess_hdl = self.conn_mgr.client_init()

            # Wait for all pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)
            # Configure Dataplane P4 Table
            self.configureTbl(sess_hdl, dev_id, port_tbl_handles)

            cpuif_ndev = self.knet_mgr.knet_cpuif_ndev_add("veth251")
            self.assertTrue(cpuif_ndev.status == 0,
                            "Cpu netdev add failed for veth251")
            if cpuif_ndev.status==0:
                cpuif = knet_cpuif_list_t(cpuif_ndev.knet_cpuif_id, six.text_type("veth251"))
                cpuif_list.append(cpuif)

            hostif_ndev = self.knet_mgr.knet_hostif_kndev_add(
                cpuif_ndev.knet_cpuif_id, "test_hostif1")
            self.assertTrue(hostif_ndev.status == 0,
                            "hostif netdev add failed for test_hostif1")
            if hostif_ndev.status==0:
                hostif = knet_hostif_list_t(hostif_ndev.knet_hostif_id, six.text_type("test_hostif1"))
                hostif_list.append(hostif)

            test_hostif2 = self.knet_mgr.knet_hostif_kndev_add(
                cpuif_ndev.knet_cpuif_id, "test_hostif2")
            self.assertTrue(hostif_ndev.status == 0,
                            "hostif netdev add failed for test_hostif2")
            if test_hostif2.status==0:
                hostif = knet_hostif_list_t(test_hostif2.knet_hostif_id, six.text_type("test_hostif2"))
                hostif_list.append(hostif)

            test_hostif3 = self.knet_mgr.knet_hostif_kndev_add(
                cpuif_ndev.knet_cpuif_id, "test_hostif3")
            self.assertTrue(hostif_ndev.status == 0,
                            "hostif netdev add failed for test_hostif3")

            # Test Scenario1: Basic Strip -Strip CPU Header , filter on Ingress Port
            filter1, mask1 = generate_filter(ingress_port=1)
            filter_spec1 = knet_rx_filter_spec_t(
                priority=100, filter=filter1, mask=mask1, filter_size=len(filter1))
            mutation_strip = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_STRIP, offset=12, len=16)
            mutation_list1 = list()
            mutation_list1.append(mutation_strip)
            filter_action1 = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                     knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=1, pkt_mutation=mutation_list1)
            rx_filter1 = knet_rx_filter_t(filter_spec1, filter_action1)
            filter_res1 = self.knet_mgr.knet_rx_filter_add(
                cpuif_ndev.knet_cpuif_id, rx_filter1)
            if filter_res1.status==0:
                filter_list.append(filter_res1.filter_id)

            # Test Scenario2: Basic Insert -Insert Vlan Header Before CPU Header , filter on bd
            filter2, mask2 = generate_filter(ingress_bd=1)
            filter_spec2 = knet_rx_filter_spec_t(
                priority=99, filter=filter2, mask=mask2, filter_size=len(filter2))
            hdr2 = bytes(VlanHeader(vlanid=1))
            mutation_insert = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_INSERT, data=hdr2, offset=14, len=len(hdr2))
            mutation_list2 = list()
            mutation_list2.append(mutation_insert)
            filter_action2 = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                     knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=1, pkt_mutation=mutation_list2)
            rx_filter2 = knet_rx_filter_t(filter_spec2, filter_action2)
            filter_res2 = self.knet_mgr.knet_rx_filter_add(
                cpuif_ndev.knet_cpuif_id, rx_filter2)
            if filter_res2.status==0:
                filter_list.append(filter_res2.filter_id)

            # Test Scenario3: Basic Strip - Strip CPU Header , filter on bit field dst_port_or_group
            filter3, mask3 = generate_filter(dst_port_or_group=1)
            filter_spec3 = knet_rx_filter_spec_t(
                priority=98, filter=filter3, mask=mask3, filter_size=len(filter3))
            mutation_strip3 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_STRIP, offset=12, len=16)
            mutation_list3 = list()
            mutation_list3.append(mutation_strip3)
            filter_action3 = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                     knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=1, pkt_mutation=mutation_list3)
            rx_filter3 = knet_rx_filter_t(filter_spec3, filter_action3)
            filter_res3 = self.knet_mgr.knet_rx_filter_add(
                cpuif_ndev.knet_cpuif_id, rx_filter3)
            if filter_res3.status==0:
                filter_list.append(filter_res3.filter_id)

            # Test Scenario4: Overlapping Strip and Insert: Overlapping offset, filter on fabric_qos
            filter4, mask4 = generate_filter(fabric_qos=1)
            filter_spec4 = knet_rx_filter_spec_t(
                priority=97, filter=filter4, mask=mask4, filter_size=len(filter4))
            mutation_strip4 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_STRIP, offset=12, len=2)
            hdr4 = bytes(FabricPayloadHeader(ether_type=0x8888))
            mutation_insert4 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_INSERT, data=hdr4, offset=12, len=len(hdr4))
            mutation_list4 = list()
            mutation_list4.append(mutation_strip4)
            mutation_list4.append(mutation_insert4)
            filter_action4 = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                     knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=2, pkt_mutation=mutation_list4)
            rx_filter4 = knet_rx_filter_t(filter_spec4, filter_action4)
            filter_res4 = self.knet_mgr.knet_rx_filter_add(
                cpuif_ndev.knet_cpuif_id, rx_filter4)
            if filter_res4.status==0:
                filter_list.append(filter_res4.filter_id)

            # Test Scenario5: Overlapping Strip and Insert region: insert offset > strip offset
            # and strip offset + strip len > insert_offset. filter on dst_device
            filter5, mask5 = generate_filter(dst_device=1)
            filter_spec5 = knet_rx_filter_spec_t(
                priority=96, filter=filter5, mask=mask5, filter_size=len(filter5))
            mutation_strip5 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_STRIP, offset=0, len=8)
            hdr5 = bytes(FabricPayloadHeader(ether_type=0x8888))
            mutation_insert5 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_INSERT, data=hdr5, offset=2, len=len(hdr5))
            mutation_list5 = list()
            mutation_list5.append(mutation_strip5)
            mutation_list5.append(mutation_insert5)
            filter_action5 = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                     knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=2, pkt_mutation=mutation_list5)
            rx_filter5 = knet_rx_filter_t(filter_spec5, filter_action5)
            filter_res5 = self.knet_mgr.knet_rx_filter_add(
                cpuif_ndev.knet_cpuif_id, rx_filter5)
            self.assertTrue(filter_res5.status != 0,
                            "Invalid Filter add Succeeded!Fatal:Kernel could Crash.Fix Knet!!")
            if filter_res5.status==0:
                filter_list.append(filter_res5.filter_id)

            # Test Scenario6: Strip Greater than packet length, filter on reason_code
            filter6, mask6 = generate_filter(reason_code=1)
            filter_spec6 = knet_rx_filter_spec_t(
                priority=95, filter=filter6, mask=mask6, filter_size=len(filter6))
            mutation_strip6 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_STRIP, offset=0, len=127)
            mutation_list6 = list()
            mutation_list6.append(mutation_strip6)
            filter_action6 = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                     knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=1, pkt_mutation=mutation_list6)
            rx_filter6 = knet_rx_filter_t(filter_spec6, filter_action6)
            filter_res6 = self.knet_mgr.knet_rx_filter_add(
                cpuif_ndev.knet_cpuif_id, rx_filter6)
            if filter_res6.status==0:
                filter_list.append(filter_res6.filter_id)

            # Test Scenario7: Multi Stip & Insert in order, filter on reserved bits
            filter7, mask7 = generate_filter(reserved1=1)
            filter_spec7 = knet_rx_filter_spec_t(
                priority=94, filter=filter7, mask=mask7, filter_size=len(filter7))
            strip_oether = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_STRIP, offset=14, len=2)
            strip_cpu = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_STRIP, offset=16, len=12)
            vlanhdr1 = bytes(VlanHeader(vlanid=1))
            vlanhdr2 = bytes(VlanHeader(vlanid=2))
            insertv1 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_INSERT, data=vlanhdr1, offset=14, len=len(vlanhdr1))
            insertv2 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_INSERT, data=vlanhdr2, offset=14, len=len(vlanhdr2))
            mutation_list7 = list()
            mutation_list7.append(strip_oether)
            mutation_list7.append(strip_cpu)
            mutation_list7.append(insertv1)
            mutation_list7.append(insertv2)
            filter_action7 = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                     knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=4, pkt_mutation=mutation_list7)
            rx_filter7 = knet_rx_filter_t(filter_spec7, filter_action7)
            filter_res7 = self.knet_mgr.knet_rx_filter_add(
                cpuif_ndev.knet_cpuif_id, rx_filter7)
            if filter_res7.status==0:
                filter_list.append(filter_res7.filter_id)

            # Test Scenario8: Multi Strip & Insert out of order, filter on pad bit
            filter8, mask8 = generate_filter(pad1=1)
            filter_spec8 = knet_rx_filter_spec_t(
                priority=93, filter=filter8, mask=mask8, filter_size=len(filter8))
            mutation_list8 = list()
            mutation_list8.append(strip_cpu)
            mutation_list8.append(insertv1)
            mutation_list8.append(strip_oether)
            mutation_list8.append(insertv2)
            filter_action8 = knet_rx_filter_action_t(dest_type=knet_filter_dest_t.BF_KNET_FILTER_DESTINATION_HOSTIF,
                                                     knet_hostif_id=hostif_ndev.knet_hostif_id, dest_proto=0, count=4, pkt_mutation=mutation_list8)
            rx_filter8 = knet_rx_filter_t(filter_spec8, filter_action8)
            filter_res8 = self.knet_mgr.knet_rx_filter_add(
                cpuif_ndev.knet_cpuif_id, rx_filter8)
            if filter_res8.status==0:
                filter_list.append(filter_res8.filter_id)

            # Test Scenario9: Add Tx action for test_hostif1
            fab_hdr = FabricHeader(packet_type=0x5)
            cpu_hdr = FabricCpuHeader(ingress_port=1)
            fab_ethtype = FabricPayloadHeader(ether_type=0x9000)
            hdr9 = bytes(fab_ethtype / fab_hdr / cpu_hdr)
            mutation_insert9 = knet_packet_mutation_t(
                mutation_type=knet_mutation_type_t.BF_KNET_RX_MUT_INSERT, data=hdr9, offset=12, len=len(hdr9))
            mutation_list9 = list()
            mutation_list9.append(mutation_insert9)
            tx_action = knet_tx_action_t(count=1, pkt_mutation=mutation_list9)
            action_res = self.knet_mgr.knet_tx_action_add(
                cpuif_ndev.knet_cpuif_id, hostif_ndev.knet_hostif_id, tx_action)

            for interface in ("test_hostif1", "test_hostif2", "test_hostif3"):
                subprocess.run(["ip", "link", "set", interface, "up"], check=True)

            s = open_packet_socket("test_hostif1")
            try:
                pkt = simple_arp_packet(
                    arp_op=1,
                    pktlen=100,
                    ip_snd='192.168.0.1',
                    ip_tgt='192.168.0.2')

                # Packet sent on port1 will have no field except ingress port set
                print("Sending Packet on Port1, fabric color will be set as 1")
                send_packet(self, swports[1], pkt)
                self.assertTrue(socket_verify_packet(pkt, s))

                print("Sending Packet on Port7, bd will be set as 1")
                exp_pkt2 = simple_cpu_vlan_packet(
                    vid=1, ingress_bd=1, ingress_port=7, inner_pkt=pkt)
                send_packet(self, swports[7], pkt)
                self.assertTrue(socket_verify_packet(exp_pkt2, s))

                print("Sending Packet on Port4, dst_port_or_grp will be set as 1")
                send_packet(self, swports[4], pkt)
                self.assertTrue(socket_verify_packet(pkt, s))

                exp_pkt = bytes(simple_cpu_packet(
                    ingress_port=2, fabric_qos=1, inner_pkt=pkt))

                print("Sending Packet on Port2, fabric_qps will be set as 1")
                exp_pkt4 = exp_pkt[:12] + b'\x88\x88' + exp_pkt[14:]
                send_packet(self, swports[2], pkt)
                self.assertTrue(socket_verify_packet(exp_pkt4, s))

                # Test 6: If filter 6 hits and mutations are done then Kernel Will crash, KNET
                # should skip these mutations by design
                print("Sending Packet on Port8, reasonCode will be set as 1")
                send_packet(self, swports[8], pkt)
                sleep(1)
                print("Test Scenario - Strip size greater than packet length passed")

                exp_pkt7 = exp_pkt[:14] + vlanhdr2 + vlanhdr1 + exp_pkt[28:]
                print("Sending Packet on Port5, reasonCode will be set as 1")
                send_packet(self, swports[5], pkt)
                self.assertTrue(socket_verify_packet(exp_pkt7, s))

                exp_pkt8 = exp_pkt[:14] + vlanhdr2 + vlanhdr1 + exp_pkt[28:]
                print("Sending Packet on Port0, pad will be set as 1")
                send_packet(self, swports[0], pkt)
                self.assertTrue(socket_verify_packet(exp_pkt8, s))

                s.send(bytes(pkt))

                print("Verifying number of filters")
                filter_count_res = self.knet_mgr.knet_get_rx_filter_cnt(
                    cpuif_ndev.knet_cpuif_id)
                self.assertTrue(filter_count_res.status == 0,
                                "Failed to retrieve filter count")
                self.assertTrue(filter_count_res.obj_count == 7,
                                "Number of filters doesn't match number of filters programmed")

                print("Verifying number of hostifs")
                hostif_count_res = self.knet_mgr.knet_get_hostif_cnt(
                    cpuif_ndev.knet_cpuif_id)
                self.assertTrue(hostif_count_res.status == 0,
                                "Failed to retrieve hostif count")
                self.assertTrue(hostif_count_res.obj_count == 3,
                                "Number of hostif doesn't match number of hostifs created")

                print("Verifying number of cpuifs")
                cpuif_count_res = self.knet_mgr.knet_get_cpuif_cnt()
                self.assertTrue(cpuif_count_res.status == 0,
                                "Failed to retrieve cpuif count")
                self.assertTrue(cpuif_count_res.obj_count == 1,
                                "Number of cpuif doesn't match number of cpuifs added")

                print("Verifying Rx mutation Count")
                rx_mutation_count_res = self.knet_mgr.knet_get_rx_mutation_cnt(
                    cpuif_ndev.knet_cpuif_id, filter_res4.filter_id)
                self.assertTrue(rx_mutation_count_res.status ==
                                0, "Failed to retrieve rx mutation count")
                self.assertTrue(rx_mutation_count_res.obj_count == 2,
                                "Number of rx mutation doesn't match number of mutations programmed")

                print("Verifying Tx mutation Count")
                tx_mutation_count_res = self.knet_mgr.knet_get_tx_mutation_cnt(
                    cpuif_ndev.knet_cpuif_id, hostif_ndev.knet_hostif_id)
                self.assertTrue(tx_mutation_count_res.status ==
                                0, "Failed to retrieve tx mutation count")
                self.assertTrue(tx_mutation_count_res.obj_count == 1,
                                "Number of tx mutation doesn't match number of mutations programmed")
                tx_mutation_count_res = self.knet_mgr.knet_get_tx_mutation_cnt(
                    cpuif_ndev.knet_cpuif_id, test_hostif2.knet_hostif_id)
                self.assertTrue(tx_mutation_count_res.status ==
                                0, "Failed to retrieve tx mutation count2")
                self.assertTrue(tx_mutation_count_res.obj_count == 0,
                                "Number of tx mutation2 doesn't match number of mutations programmed")

                print("Retrieving Rx Filter")
                rx_filter_res = self.knet_mgr.knet_rx_filter_get(
                    cpuif_ndev.knet_cpuif_id, filter_res2.filter_id, 2)
                self.assertTrue(rx_filter_res.status == 0,
                                "Failed to retrieve rx filter")
                self.assertTrue(verify_filter(rx_filter2, rx_filter_res.rx_filter),
                                "Retrieved Rx filter doesn't matched programmed filter")
                rx_filter_res2 = self.knet_mgr.knet_rx_filter_get(
                    cpuif_ndev.knet_cpuif_id, filter_res4.filter_id, 4)
                self.assertTrue(rx_filter_res2.status == 0,
                                "Failed to retrieve rx filter")
                self.assertTrue(verify_filter(rx_filter4, rx_filter_res2.rx_filter),
                                "Retrieved Rx filter doesn't matched programmed filter")

                print("Retrieving Tx Action")
                tx_action_res = self.knet_mgr.knet_tx_action_get(cpuif_ndev.knet_cpuif_id, hostif_ndev.knet_hostif_id, 1)
                self.assertTrue(tx_action_res.status == 0,
                                "Failed to retrieve tx action")
                self.assertTrue(verify_action(tx_action, tx_action_res.tx_action),
                                "Retrieved hostif list does not match added cpuifs")
                tx_action_res2 = self.knet_mgr.knet_tx_action_get(cpuif_ndev.knet_cpuif_id, test_hostif2.knet_hostif_id, 1)
                self.assertTrue(tx_action_res2.status != 0,
                                "tx action Invalid")

                print("Retrieving Cpuif List")
                cpuif_list_res = self.knet_mgr.knet_cpuif_list_get(len(cpuif_list))
                self.assertTrue(cpuif_list_res.status == 0,
                                "Failed to retrieve cpuif list")
                self.assertTrue(verify_list(cpuif_list, cpuif_list_res.cpuif_list),
                                "Retrieved cpuif list does not match added cpuifs")

                print("Retrieving Hostif List")
                hostif_list_res = self.knet_mgr.knet_hostif_list_get(cpuif_ndev.knet_cpuif_id, len(hostif_list))
                self.assertTrue(hostif_list_res.status == 0,
                                "Failed to retrieve hostif list")
                self.assertTrue(verify_list(hostif_list, hostif_list_res.hostif_list),
                                "Retrieved hostif list does not match added cpuifs")

                print("Retrieving Rx Filter List")
                rx_filter_list_res = self.knet_mgr.knet_rx_filter_list_get(
                    cpuif_ndev.knet_cpuif_id, len(filter_list))
                self.assertTrue(rx_filter_list_res.status == 0,
                                "Failed to retrieve rx filter list")
                self.assertTrue(verify_list(filter_list, rx_filter_list_res.filter_list),
                                "Retrieved Rx filter list does not match programmed filters")
            finally:
                s.close()

        finally:
            # Delete cpuif (This will clear out all state for KNET)
            self.knet_mgr.knet_cpuif_ndev_delete(cpuif_ndev.knet_cpuif_id)

            for handle in port_tbl_handles:
                sts = self.client.port_tbl_table_delete(
                    sess_hdl, dev_id, handle)

            sess_hdl = self.conn_mgr.client_cleanup(sess_hdl)
