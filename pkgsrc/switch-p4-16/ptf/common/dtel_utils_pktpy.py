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
Utilities needed to exercise PTF tests for DTel
"""
import sys
import logging
import ptf
from ptf.testutils import *
from ptf.packet import *
import ptf.mask
from bf_pktpy.packets import ModHeader, PostcardHeader, DtelReportHdr, \
    DtelReportV2Hdr, UDP, Ether, IP
from bf_pktpy.all import hexdump

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *
from p4testutils.misc_utils import mask_set_do_not_care_packet

import binascii

try:
    from pal_rpc.ttypes import *
except ImportError:
    sys.exit("Need to install scapy for packet parsing")

DTEL_REPORT_NEXT_PROTO_ETHERNET       = 0
DTEL_REPORT_NEXT_PROTO_MOD            = 1
DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL   = 2

INVALID_PORT_ID = 511

UDP_PORT_DTEL_REPORT = 32766

DEBUG = False

DTEL_REPORT_V2_HDR = "DtelReportV2Hdr"
DTEL_REPORT_HDR = "DtelReportHdr"


class SwitchConfig_Params():
    def __init__(self):
        self.device = 0
        self.swports = list(range(64))
        self.switch_id = 0x11111111
        self.dst_udp_port = UDP_PORT_DTEL_REPORT
        self.mac_self = '00:77:66:55:44:33'
        self.nports = 0
        # self.port_speed = SWITCH_PORT_SPEED_10G
        self.ipaddr_inf = ['172.16.0.1',  '172.17.0.1']
        self.ipaddr_nbr = ['172.16.0.2', '172.17.0.2']
        self.mac_nbr = ['00:11:22:33:44:55', '00:11:22:33:44:56']
        self.report_ports = [1]
        self.ipaddr_report_src = ['4.4.4.1']
        self.ipaddr_report_dst = ['4.4.4.3']
        self.routes = [('192.168.10.1', 1)]
        # self.urpf_mode = SWITCH_URPF_MODE_NONE
        self.vlans = None
        self.ecmps = []
        self.udp_src_port = 0


"""
Mirror on Drop utilities
"""


# NOTE(arydzyk): These are left due to backward compatibility with scapy
def bind_mirror_on_drop_pkt(int_v2):
    pass


def split_mirror_on_drop_pkt(int_v2):
    pass


# NOTE(sborkows): These functions are not yet implemented in bf_pktpy and we do not
# need this functionality in bf-ptf/bf_pktpy configuration, but to keep compatibility
# with p4lang-ptf/scapy one these 2 functions must be defined.
def bind_layers(layer1, layer2, **kwargs):
    pass


def split_layers(layer1, layer2, **kwargs):
    pass


def mod_report(packet,
               switch_id=0,
               ingress_port=0,
               egress_port=0,
               queue_id=0,
               drop_reason=0):
    mod_hdr = ModHeader(switch_id=switch_id,
                        ingress_port=ingress_port,
                        egress_port=egress_port,
                        queue_id=queue_id,
                        drop_reason=drop_reason,
                        pad=0)
    return mod_hdr / packet


"""
Postcard utilities
"""


# NOTE(sborkows): These are left due to backward compatibility with scapy
def bind_postcard_pkt(int_v2):
    pass


def split_postcard_pkt(int_v2):
    pass


def postcard_report(packet,
                    switch_id=0,
                    ingress_port=0,
                    egress_port=0,
                    queue_id=0,
                    queue_depth=0,
                    egress_tstamp=0):
    postcard_hdr = PostcardHeader(switch_id=switch_id,
                                  ingress_port=ingress_port,
                                  egress_port=egress_port,
                                  queue_id=queue_id,
                                  queue_depth=queue_depth,
                                  egress_tstamp=egress_tstamp)
    return postcard_hdr / packet




# TODO: workaround because of conditional fields in DTELv2 packet. When new arch in
#  bf_pktpy is fully introduced, this can be removed.
def _mask_in_dtel_pkt(exp_pkt_mask, hdr_type, field_name):
    exp_pkt = exp_pkt_mask.exp_pkt
    if hdr_type not in exp_pkt:
        exp_pkt_mask.valid = False
        print("Header of type %s not found in DTEL packet" % hdr_type.__name__)
        return

    field_details = exp_pkt[hdr_type].args_details()
    hdr_offset = exp_pkt_mask.size - len(exp_pkt[hdr_type])
    field_offset = 0
    bitwidth = 0
    # field_details is a list of 2-element tuple: (name, size in bits)
    for field_detail in field_details:
        bits = field_detail[1]
        if field_detail[0] == field_name:
            bitwidth = bits
            break
        else:
            field_offset += bits

    exp_pkt_mask.set_do_not_care(hdr_offset * 8 + field_offset, bitwidth)


def match_dtel_pkt(exp_pkt, pkt, int_v2, ignore_tstamp=True, ignore_seq_num=True,
                   ignore_queue_depth=True, ignore_udp_sport=True, ignore_hw_id=True,
                   ingore_queue_id=True, check_egress_tstamp=False, error_on_zero=True):
    """
    Compare DTel report packets, ignore the timestamp and sequence number
    values. Just make sure that the timestamp is non-zero.
    """
    seq_num = -1
    exp_pkt_mask = exp_pkt if isinstance(exp_pkt, ptf.mask.Mask) \
        else ptf.mask.Mask(exp_pkt)
    rcv_pkt_obj = exp_pkt_mask.exp_pkt.copy().load_bytes(pkt)

    # check that both expected and received packets have a DTel report header
    # NOTE(sborkows): in Tofino2 DTEL report V2 can contain fields from
    # PostcardHeader used in Tofino1
    dtel_header = DtelReportV2Hdr if int_v2 else DtelReportHdr
    second_cpu_header = DtelReportV2Hdr
    # NOTE(sborkows): in Tofino1 there are two options after DtelReportHdr:
    # PostcardHeader or ModHeader
    if not int_v2:
        second_cpu_header = ModHeader \
            if exp_pkt_mask.exp_pkt.haslayer(ModHeader.__name__) \
            else PostcardHeader
    if not exp_pkt_mask.exp_pkt.haslayer(dtel_header.__name__) \
            or not exp_pkt_mask.exp_pkt.haslayer(second_cpu_header.__name__) \
            or not rcv_pkt_obj.haslayer(dtel_header.__name__) \
            or not rcv_pkt_obj.haslayer(second_cpu_header.__name__):
        return False, seq_num
    seq_num = rcv_pkt_obj[dtel_header].sequence_number

    # check that timestamp is non-zero
    if error_on_zero:
        if int_v2 and rcv_pkt_obj[dtel_header].rep_md_bits & 0x0800 \
                and rcv_pkt_obj[dtel_header].timestamp == 0:
            return False, seq_num
        if not int_v2 and rcv_pkt_obj[dtel_header].timestamp == 0:
            return False, seq_num
    # check that egress timestamp is non-zero
    if check_egress_tstamp:
        if int_v2 and rcv_pkt_obj[dtel_header].rep_md_bits & 0x0400 \
                and rcv_pkt_obj[second_cpu_header].egress_tstamp == 0:
            return False, seq_num
        if not int_v2 and rcv_pkt_obj[second_cpu_header].egress_tstamp == 0:
            return False, seq_num

    # ignore timestamp
    if ignore_tstamp:
        _mask_in_dtel_pkt(exp_pkt_mask, dtel_header, "timestamp")
        _mask_in_dtel_pkt(exp_pkt_mask, second_cpu_header, "egress_tstamp")
    # ignore sequence number
    if ignore_seq_num:
        _mask_in_dtel_pkt(exp_pkt_mask, dtel_header, "sequence_number")
    # ignore queue_depth
    if ignore_queue_depth:
        _mask_in_dtel_pkt(exp_pkt_mask, second_cpu_header, "queue_depth")
    # ignore udp source port
    if ignore_udp_sport:
        mask_set_do_not_care_packet(exp_pkt_mask, UDP, "sport")
    # ignore hw id
    if ignore_hw_id:
        _mask_in_dtel_pkt(exp_pkt_mask, dtel_header, "hw_id")
    # ignore queue id
    if ingore_queue_id:
        _mask_in_dtel_pkt(exp_pkt_mask, dtel_header, "drop_queue_id")

    # compare
    return exp_pkt_mask.pkt_match(pkt), seq_num


def verify_postcard_packet(test, exp_pkt, port, ignore_seq_num=True,
                           ignore_udp_sport=True, fail_on_none=True):
    """
    Check that an expected postcard report is received
    while ignoring latency value and timestamp value.
    """

    logging.debug("Checking for pkt on port %r", port)
    (_, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
        port_number=port, timeout=2, exp_pkt=None)
    if rcv_pkt is None and not fail_on_none:
        return None
    test.assertTrue(rcv_pkt is not None, "Did not receive pkt on %r" % port)

    encoded_pkt = binascii.hexlify(rcv_pkt)
    ether_type = Ether.from_hex(encoded_pkt)
    while ether_type == 0x88cc:  # ignore LLDP packets
        (_, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
            port_number=port, timeout=2, exp_pkt=None)
        if rcv_pkt is None and not fail_on_none:
            return None
        test.assertTrue(rcv_pkt is not None, "Did not receive pkt on %r" % port)
        encoded_pkt = binascii.hexlify(rcv_pkt)
        ether_type = Ether.from_hex(encoded_pkt)

    is_match, _ = match_dtel_pkt(exp_pkt, rcv_pkt, test.int_v2,
                                 ignore_seq_num=ignore_seq_num,
                                 ignore_udp_sport=ignore_udp_sport,
                                 check_egress_tstamp=True, error_on_zero=False)
    if not is_match:
        print ("========== EXPECTED ==========")
        hexdump(exp_pkt)
        print ("========== RECEIVED ==========")
        hexdump(" ".join(encoded_pkt[i:i + 2].decode()
                         for i in range(0, len(encoded_pkt), 2)))
        test.assertTrue(0, "Received packet did not match expected packet")

    return exp_pkt.copy().load_bytes(rcv_pkt)


def receive_postcard_packet(test, exp_pkt, port,
                            ignore_seq_num=True, ignore_udp_sport=True):
    return verify_postcard_packet(test, exp_pkt, port, ignore_seq_num,
                                  ignore_udp_sport, fail_on_none=False)


"""
DTel Report Header v0.5 utilities
"""


def dtel_report(packet,
               ver=0,
               next_proto=0,
               dropped=0,
               congested_queue=0,
               path_tracking_flow=0,
               hw_id=0,
               sequence_number=0,
               timestamp=0):
    dtel_report_hdr = DtelReportHdr(ver = ver,
                      next_proto = next_proto,
                      dropped = dropped,
                      congested_queue = congested_queue,
                      path_tracking_flow = path_tracking_flow,
                      hw_id = hw_id,
                      sequence_number = sequence_number,
                      timestamp = timestamp)
    return dtel_report_hdr / packet


def ipv4_dtel_pkt(pktlen=0,
                      eth_dst='00:01:02:03:04:05',
                      eth_src='00:06:07:08:09:0a',
                      dl_vlan_enable=False,
                      vlan_vid=0,
                      vlan_pcp=0,
                      dl_vlan_cfi=0,
                      ip_src='192.168.0.1',
                      ip_dst='192.168.0.2',
                      ip_tos=0,
                      ip_ecn=None,
                      ip_dscp=None,
                      ip_ttl=64,
                      ip_id=0x0001,
                      ip_ihl=None,
                      ip_options=False,
                      udp_sport=0,
                      udp_dport=UDP_PORT_DTEL_REPORT,
                      with_udp_chksum=False,
                      ver=0,
                      next_proto=0,
                      dropped=0,
                      congested_queue=0,
                      path_tracking_flow=0,
                      hw_id=0,
                      sequence_number=0,
                      inner_frame=None
                      ):

    telem_pkt = DtelReportHdr(ver = ver,
                      next_proto = next_proto,
                      dropped = dropped,
                      congested_queue = congested_queue,
                      path_tracking_flow = path_tracking_flow,
                      hw_id = hw_id,
                      sequence_number = sequence_number)

    if inner_frame:
        telem_pkt = telem_pkt / inner_frame

    udp_pkt = simple_udp_packet(
                      pktlen=pktlen,
                      eth_dst=eth_dst,
                      eth_src=eth_src,
                      dl_vlan_enable=dl_vlan_enable,
                      vlan_vid=vlan_vid,
                      vlan_pcp=vlan_pcp,
                      dl_vlan_cfi=dl_vlan_cfi,
                      ip_src=ip_src,
                      ip_dst=ip_dst,
                      ip_tos=ip_tos,
                      ip_ecn=ip_ecn,
                      ip_dscp=ip_dscp,
                      ip_ttl=ip_ttl,
                      ip_id=ip_id,
                      ip_ihl=ip_ihl,
                      ip_options=ip_options,
                      udp_sport=udp_sport,
                      udp_dport=udp_dport,
                      with_udp_chksum=with_udp_chksum,
                      udp_payload=telem_pkt
    )

    udp_pkt[IP].flags = 0x2
    return udp_pkt


"""
DTel Report Header v2.0 utilities
"""


_REP_MD_BITS = [ 'drop_reason',
                 'reserved',
                 'reserved',
                 'reserved',
                 'reserved',
                 'reserved',
                 'reserved',
                 'buffer_occupancy',
                 'eg_port_tx_util',
                 'level_2_if_ids',
                 'egress_tstamp',
                 'ingress_tstamp',
                 'queue_occupancy',
                 'hop_latency',
                 'level_1_if_ids',
                 'reserved'
               ]


def ipv4_dtel_v2_pkt(pktlen=0,
                     eth_dst='00:01:02:03:04:05',
                     eth_src='00:06:07:08:09:0a',
                     dl_vlan_enable=False,
                     vlan_vid=0,
                     vlan_pcp=0,
                     dl_vlan_cfi=0,
                     ip_src='192.168.0.1',
                     ip_dst='192.168.0.2',
                     ip_tos=0,
                     ip_ecn=None,
                     ip_dscp=None,
                     ip_ttl=64,
                     ip_id=0x0001,
                     ip_ihl=None,
                     ip_options=False,
                     udp_sport=0,
                     udp_dport=UDP_PORT_DTEL_REPORT,
                     with_udp_chksum=False,
                     ver=0,
                     dropped=0,
                     congested_queue=0,
                     path_tracking_flow=0,
                     hw_id=0,
                     sequence_number=0,
                     switch_id=0,
                     int_v2=False,
                     inner_frame=None,
                     ingress_port=None,
                     egress_port=None,
                     hop_latency=None,
                     queue_id=0,
                     queue_depth=None,
                     ingress_tstamp=None,
                     egress_tstamp=None,
                     drop_reason=None):

    # compute rep_md_bits
    rep_md_bits = 0

    if (ingress_port != None) or (egress_port != None):
        rep_md_bits = rep_md_bits | 0x4000
    if not ingress_port:
        ingress_port = 0
    if not egress_port:
        egress_port = 0

    if (hop_latency != None):
        rep_md_bits = rep_md_bits | 0x1000
    else:
        hop_latency = 0

    if (ingress_tstamp != None):
        rep_md_bits = rep_md_bits | 0x0800
    else:
        ingress_tstamp = 0

    if (egress_tstamp != None):
        rep_md_bits = rep_md_bits | 0x0400
    else:
        egress_tstamp = 0

    if drop_reason == None or drop_reason == 0:
        drop_reason = 0
        drop_queue_id = 0
    else:
        rep_md_bits = rep_md_bits | 0x0001
        drop_queue_id = queue_id
    drop_reserved = 0

    if (queue_depth != None):
        rep_md_bits = rep_md_bits | 0x1000
    else:
        queue_depth = 0

    telem_pkt = DtelReportV2Hdr(ver=2,
                                hw_id=hw_id,
                                sequence_number=sequence_number,
                                switch_id=switch_id,
                                rep_type=1,
                                in_type=3,
                                dropped=dropped,
                                congested_queue=congested_queue,
                                path_tracking_flow=path_tracking_flow,
                                reserved=0,
                                rep_md_bits=rep_md_bits,
                                domain_specific_id=0,
                                ds_md_bits=0,
                                ds_md_status=0,
                                ingress_port=ingress_port,
                                egress_port=egress_port,
                                hop_latency=hop_latency,
                                queue_id=queue_id,
                                queue_depth=queue_depth,
                                timestamp=ingress_tstamp,
                                egress_tstamp=egress_tstamp,
                                drop_queue_id=drop_queue_id,
                                drop_reason=drop_reason,
                                drop_reserved=drop_reserved)

    if inner_frame:
        telem_pkt = telem_pkt / inner_frame

    udp_pkt = simple_udp_packet(
                      pktlen=pktlen,
                      eth_dst=eth_dst,
                      eth_src=eth_src,
                      dl_vlan_enable=dl_vlan_enable,
                      vlan_vid=vlan_vid,
                      vlan_pcp=vlan_pcp,
                      dl_vlan_cfi=dl_vlan_cfi,
                      ip_src=ip_src,
                      ip_dst=ip_dst,
                      ip_tos=ip_tos,
                      ip_ecn=ip_ecn,
                      ip_dscp=ip_dscp,
                      ip_ttl=ip_ttl,
                      ip_id=ip_id,
                      ip_ihl=ip_ihl,
                      ip_options=ip_options,
                      udp_sport=udp_sport,
                      udp_dport=udp_dport,
                      with_udp_chksum=with_udp_chksum,
                      udp_payload=telem_pkt
    )

    udp_pkt[IP].flags = 0x2
    udp_pkt[DtelReportV2Hdr].report_length = \
        (udp_pkt[DtelReportV2Hdr].total_len - 12) // 4
    return udp_pkt


"""
DTel Report Header common utilities
"""


def verify_dtel_packet(test, exp_pkt, port, ignore_tstamp=True,
                       ignore_seq_num=True, ignore_queue_depth=True,
                       ignore_udp_sport=True, ignore_hw_id=True,
                       fail_on_none=True):
    """
    Check that an expected packet is received
    """
    logging.debug("Checking for pkt on port %r", port)
    (_, rcv_port, rcv_pkt, pkt_time) = \
        test.dataplane.poll(port_number=port, timeout=2, exp_pkt=None)
    if rcv_pkt is None and not fail_on_none:
        return None
    test.assertTrue(rcv_pkt is not None, "Did not receive pkt on %r" % port)

    encoded_pkt = binascii.hexlify(rcv_pkt)
    is_match, _ = match_dtel_pkt(exp_pkt, rcv_pkt, test.int_v2,
                                 ignore_tstamp=ignore_tstamp,
                                 ignore_seq_num=ignore_seq_num,
                                 ignore_queue_depth=ignore_queue_depth,
                                 ignore_udp_sport=ignore_udp_sport,
                                 ignore_hw_id=ignore_hw_id)
    if not is_match:
        print ("========== EXPECTED ==========")
        hexdump(exp_pkt)
        print ("========== RECEIVED ==========")
        hexdump(" ".join(encoded_pkt[i:i+2].decode()
                         for i in range(0, len(encoded_pkt), 2)))
        test.assertTrue(0, "Received packet did not match expected packet")

    return exp_pkt.copy().load_bytes(rcv_pkt)


def receive_dtel_packet(test, exp_pkt, port):
    return verify_dtel_packet(test, exp_pkt, port, fail_on_none=False)


def verify_any_dtel_packet_any_port(test, pkts=None, ports=None,
                                    device_number=0,
                                    ignore_seq_num=True):
    """
    Check that _any_ of the packets is received on _any_ of the specified ports
    belonging to the given device (default device_number is 0).
    Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device (unless --relax
    is in effect).
    Returns the index of the port on which the packet is received.
    """
    received = False
    match_index = 0
    seq_num = -1
    if pkts is None:
        pkts = []
    if ports is None:
        ports = []

    (_, rcv_port, rcv_pkt, _) = test.dataplane.poll(device_number=device_number,
                                                    timeout=5)
    test.assertTrue(rcv_pkt is not None, "Did not receive pkt on any port from "
                                         "provided ports list")
    test.assertTrue(rcv_port in ports, "Pkt received, but not on any port from "
                                       "provided ports list")
    logging.debug("Checking for pkt on device %d, port list %r",
                  device_number, ports)

    for i in range(0, len(pkts)):
        exp_pkt = pkts[i]
        if exp_pkt is None:
            continue
        is_match, seq_num = match_dtel_pkt(exp_pkt, rcv_pkt, test.int_v2,
                                           ignore_seq_num=ignore_seq_num,
                                           check_egress_tstamp=True)
        if is_match:
            match_index = i
            received = True
            break

    test.assertTrue(
        received,
        "Did not receive expected pkt(s) on any of ports %r for device %d"
        % (ports, device_number))
    return match_index, seq_num


def verify_dtel_packet_any_port(test, pkt=None, ports=None,
                                    device_number=0,
                                    ignore_seq_num=True):
    """
    Check that packet is received on _any_ of the specified ports
    belonging to the given device (default device_number is 0).
    Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device (unless --relax
    is in effect).
    Returns the index of the port on which the packet is received.
    """
    match_index = 0
    seq_num = -1
    if ports is None:
        ports = []

    (_, rcv_port, rcv_pkt, _) = test.dataplane.poll(device_number=device_number,
                                                    timeout=5)
    test.assertTrue(rcv_pkt is not None, "Did not receive pkt on any port from "
                                         "provided ports list")
    test.assertTrue(rcv_port in ports, "Pkt received, but not on any port from "
                                       "provided ports list")
    logging.debug("Checking for pkt on device %d, port list %r",
                  device_number, ports)

    is_match, seq_num = match_dtel_pkt(pkt, rcv_pkt, test.int_v2,
                                       ignore_seq_num=ignore_seq_num,
                                       check_egress_tstamp=True)
    test.assertTrue(
        is_match,
        "Did not receive expected pkt on any of ports %r for device %d"
        % (ports, device_number))

    for j in range(0, len(ports)):
        if rcv_port == ports[j]:
            match_index = j
            break

    return match_index, seq_num


def receive_packet(test, port_id, timeout=2):
    dev, port = port_to_tuple(port_id)
    (rcv_device, rcv_port, rcv_pkt, pkt_time) = dp_poll(test, dev, port,
                                                        timeout=timeout)
    return rcv_pkt


def dtel_checkDoD(test, in_port, out_port, report_port,
                  pkt_in, exp_pkt_out, drop=True, exp_dod_pkt=None,
                  pkt_num=10, exp_e2e_pkt=None, exp_report_port=0):

    # Workaround for model deflect issue:
    # define separate exp_report_port for postcards vs report_port for dod

    # 10th pkt is dropped by the model
    # pay your attention that counter is enabled once MoD is enabled
    for n in range(0, pkt_num-1):
        print ("Sending pkt ", n)
        send_packet(test, in_port, pkt_in)
        verify_packet(test, exp_pkt_out, out_port, timeout=5)
        if exp_e2e_pkt is not None:
            verify_postcard_packet(test, exp_e2e_pkt, exp_report_port)

        verify_no_other_packets(test)

    send_packet(test, in_port, pkt_in)
    # verify mirror on drop packet
    if not drop:
        verify_packet(test, exp_pkt_out, out_port)
        if exp_e2e_pkt is not None:
            verify_postcard_packet(test, exp_e2e_pkt, exp_report_port)
    else:
        if exp_dod_pkt is not None:
            verify_dtel_packet(test, exp_dod_pkt, report_port)
    verify_no_other_packets(test, timeout=1)
