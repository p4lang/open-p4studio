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
import ptf
import ptf.dataplane as dataplane
from ptf.testutils import *
from ptf.ptfutils import *
import ptf.mask
from scapy.all import Packet
from scapy.fields import *
from scapy.all import Ether
from scapy.all import bind_layers, split_layers
from scapy.utils import hexdump


from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *
from common.utils import mask_set_do_not_care_packet

import scapy.layers.l2
import scapy.layers.inet
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

class SwitchConfig_Params():
    def __init__(self):
        self.device = 0
        self.swports = list(range(64))
        self.switch_id = 0x11111111;
        self.dst_udp_port = UDP_PORT_DTEL_REPORT
        self.mac_self = '00:77:66:55:44:33'
        self.nports = 0
        #self.port_speed = SWITCH_PORT_SPEED_10G
        self.ipaddr_inf = ['172.16.0.1',  '172.17.0.1']
        self.ipaddr_nbr = ['172.16.0.2', '172.17.0.2']
        self.mac_nbr = ['00:11:22:33:44:55', '00:11:22:33:44:56']
        self.report_ports = [1]
        self.ipaddr_report_src = ['4.4.4.1']
        self.ipaddr_report_dst = ['4.4.4.3']
        self.routes = [('192.168.10.1', 1)]
        #self.urpf_mode = SWITCH_URPF_MODE_NONE
        self.vlans=None
        self.ecmps=[]
        self.udp_src_port = 0

"""
Mirror on Drop utilities
"""
class MOD_HDR(Packet):
    name = "MoD header"
    fields_desc = [ IntField("switch_id", 0x0),
                    ShortField("ingress_port", 0x0),
                    ShortField("egress_port", 0x0),
                    XByteField("queue_id", 0x0),
                    XByteField("drop_reason", 0x0),
                    ShortField("pad", 0x0)]

def bind_mirror_on_drop_pkt(int_v2):
    if int_v2:
        bind_layers(UDP, DTEL_REPORT_V2_HDR, dport=UDP_PORT_DTEL_REPORT)
        bind_layers(DTEL_REPORT_V2_HDR, Ether, in_type=3)
    else:
        bind_layers(UDP, DTEL_REPORT_HDR, dport=UDP_PORT_DTEL_REPORT)
        bind_layers(DTEL_REPORT_HDR, MOD_HDR,
                    next_proto=DTEL_REPORT_NEXT_PROTO_MOD)
        bind_layers(MOD_HDR, Ether)

def split_mirror_on_drop_pkt(int_v2):
    if int_v2:
        split_layers(UDP, DTEL_REPORT_V2_HDR, dport=UDP_PORT_DTEL_REPORT)
        split_layers(DTEL_REPORT_V2_HDR, Ether, in_type=3)
    else:
        split_layers(UDP, DTEL_REPORT_HDR, dport=UDP_PORT_DTEL_REPORT)
        split_layers(DTEL_REPORT_HDR, MOD_HDR,
                     next_proto=DTEL_REPORT_NEXT_PROTO_MOD)
        split_layers(MOD_HDR, Ether)

def mod_report(packet,
               switch_id=0,
               ingress_port=0,
               egress_port=0,
               queue_id=0,
               drop_reason=0):
    mod_hdr = MOD_HDR(switch_id = switch_id,
                      ingress_port = ingress_port,
                      egress_port = egress_port,
                      queue_id = queue_id,
                      drop_reason = drop_reason,
                      pad = 0)
    return mod_hdr / packet

"""
Postcard utilities
"""
class POSTCARD_HDR(Packet):
    name = "Postcard header"
    fields_desc = [ IntField("switch_id", 0x0),
                    ShortField("ingress_port", 0x0),
                    ShortField("egress_port", 0x0),
                    XByteField("queue_id", 0x0),
                    X3BytesField("queue_depth", 0x0),
                    IntField("egress_tstamp", 0x0)]

def bind_postcard_pkt(int_v2):
    if int_v2:
        bind_layers(UDP, DTEL_REPORT_V2_HDR, dport=UDP_PORT_DTEL_REPORT)
        bind_layers(DTEL_REPORT_V2_HDR, Ether, in_type=3)
    else:
        bind_layers(UDP, DTEL_REPORT_HDR, dport=UDP_PORT_DTEL_REPORT)
        bind_layers(DTEL_REPORT_HDR, POSTCARD_HDR,
                    next_proto=DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL)
        bind_layers(POSTCARD_HDR, Ether)

def split_postcard_pkt(int_v2):
    if int_v2:
        split_layers(UDP, DTEL_REPORT_V2_HDR, dport=UDP_PORT_DTEL_REPORT)
        split_layers(DTEL_REPORT_V2_HDR, Ether, in_type=3)
    else:
        split_layers(UDP, DTEL_REPORT_HDR, dport=UDP_PORT_DTEL_REPORT)
        split_layers(DTEL_REPORT_HDR, POSTCARD_HDR,
                     next_proto=DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL)
        split_layers(POSTCARD_HDR, Ether)

def postcard_report(packet,
                    switch_id=0,
                    ingress_port=0,
                    egress_port=0,
                    queue_id=0,
                    queue_depth=0,
                    egress_tstamp=0):
    postcard_hdr = POSTCARD_HDR(switch_id = switch_id,
                                ingress_port = ingress_port,
                                egress_port = egress_port,
                                queue_id = queue_id,
                                queue_depth = queue_depth,
                                egress_tstamp = egress_tstamp)
    return postcard_hdr / packet

def ignore_postcard_values(exp_pkt, pkt, int_v2, error_on_zero=False):
    """
    Reset latency values to zero in postcard reports.
    Just make sure the latency and tstamp values are non-zero.
    """
    if int_v2:
        exp_pkt_postcard_hdr = exp_pkt.getlayer(DTEL_REPORT_V2_HDR)
        pkt_postcard_hdr = pkt.getlayer(DTEL_REPORT_V2_HDR)
    else:
        exp_pkt_postcard_hdr = exp_pkt.getlayer(POSTCARD_HDR)
        pkt_postcard_hdr = pkt.getlayer(POSTCARD_HDR)


    if exp_pkt_postcard_hdr == None or pkt_postcard_hdr == None:
        print ("No postcard header in the packet")
        return False

    if error_on_zero and int_v2 and (pkt_postcard_hdr.rep_md_bits & 0x0400) and pkt_postcard_hdr.egress_tstamp == 0:
        print ("Egress timestamp is zero")
        return False
    if error_on_zero and not int_v2 and pkt_postcard_hdr.egress_tstamp == 0:
        print ("Egress timestamp is zero")
        return False

    exp_pkt_postcard_hdr.egress_tstamp = 0
    pkt_postcard_hdr.egress_tstamp = 0

    return True


def truncate_packet(full_pkt, truncate_size):
    return Ether(bytes(full_pkt)[:-truncate_size])


def verify_postcard_dtel_packet(test, rcv_pkt, exp_pkt,
                                ignore_seq_num = True, ignore_udp_sport = True):

    # convert rcv_pkt string back to layered pkt
    nrcv = exp_pkt.__class__(rcv_pkt)
    test.assertTrue(ignore_postcard_values(exp_pkt, nrcv, test.int_v2, False),
                    "Received packet did not match expected postcard packet")
    if match_dtel_pkt(exp_pkt, nrcv, test.int_v2,
                      ignore_seq_num = ignore_seq_num,
                      ignore_udp_sport = ignore_udp_sport,
                      ignore_hw_id = True, error_on_zero=False) == False:
        print ("========== EXPECTED ==========")
        exp_pkt.show2()
        hexdump(exp_pkt)
        print ("========== RECEIVED ==========")
        nrcv.show2()
        hexdump(nrcv)
        test.assertTrue(0, "Received packet did not match expected postcard packet")
    return nrcv

def verify_postcard_packet(test, exp_pkt, port, ignore_seq_num=True,
                           ignore_udp_sport=True, fail_on_none=True):
    """
    Check that an expected postcard report is received
    while ignoring latency value and timestamp value.
    Just make sure the latency and tstamp values are non-zero.
    """
    logging.debug("Checking for pkt on port %r", port)
    (_, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
        port_number=port, timeout=ptf.ptfutils.default_timeout, exp_pkt=None)
    nrcv = exp_pkt.__class__(rcv_pkt)
    while nrcv[Ether].type == 0x88cc: # ignore LLDP packets
        (_, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
            port_number=port, timeout=ptf.ptfutils.default_timeout, exp_pkt=None)
        nrcv = exp_pkt.__class__(rcv_pkt)
    test.assertTrue(rcv_pkt != None, "Did not receive pkt on %r" % port)
    verify_postcard_dtel_packet(test, rcv_pkt, exp_pkt,
                                ignore_seq_num = ignore_seq_num,
                                ignore_udp_sport = ignore_udp_sport)

def receive_postcard_packet(test, exp_pkt, port):
    (_, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
        port_number=port, timeout=ptf.ptfutils.default_timeout, exp_pkt=None)
    nrcv = None
    if rcv_pkt:
        nrcv = exp_pkt.__class__(rcv_pkt)
        while nrcv[Ether].type == 0x88cc: # ignore LLDP packets
            (_, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
                port_number=port, timeout=ptf.ptfutils.default_timeout, exp_pkt=None)
            nrcv = exp_pkt.__class__(rcv_pkt)
        if rcv_pkt:
            verify_postcard_dtel_packet(test, rcv_pkt, exp_pkt)
    return nrcv

"""
DTel Report Header v0.5 utilities
"""
class DTEL_REPORT_HDR(Packet):
    name = "DTel Report header"
    fields_desc = [ BitField("ver", 0, 4), BitField("next_proto", 0, 4),
                    BitField("dropped", 0, 1),
                    BitField("congested_queue", 0, 1),
                    BitField("path_tracking_flow", 0, 1),
                    BitField("reserved", 0, 15),
                    BitField("hw_id", 0, 6),
                    IntField("sequence_number", 0),
                    XIntField("timestamp", 0x00000000)]

def dtel_report(packet,
               ver=0,
               next_proto=0,
               dropped=0,
               congested_queue=0,
               path_tracking_flow=0,
               hw_id=0,
               sequence_number=0,
               timestamp=0):
    dtel_report_hdr = DTEL_REPORT_HDR(ver = ver,
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

    telem_pkt = DTEL_REPORT_HDR(ver = ver,
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
class MDLengthLenField(FieldLenField):
    """
    Counts number of bits that are set in count_of field,
    and double counts bits that represent double length fields.
    """
    def i2m(self, pkt, x):
        if x is None:
            if self.count_of is not None:
                fld, fval = pkt.getfield_and_val(self.count_of)
                fbits = fld.i2repr(pkt, fval).split('+')
                f = len(fbits)
                f += fbits.count('ingress_tstamp')
                f += fbits.count('egress_tstamp')
                f += fbits.count('level_2_if_ids')
                x = self.adjust(pkt, f)
        return x

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

class DTEL_REPORT_V2_HDR(Packet):
    name = "DTel Report v2 header"
    fields_desc = [ BitField("ver", 0, 4),
                    BitField("hw_id", 0, 6),
                    BitField("sequence_number", 0, 22),
                    XIntField("switch_id", 0x0),
                    BitField("rep_type", 0, 4),
                    BitField("in_type", 0, 4),
                    ByteField("report_length", 0),
                    #LenField("report_length", None, fmt='B'),
                    MDLengthLenField("md_length", 0, fmt='B',
                                     count_of="rep_md_bits"),
                    BitField("dropped", 0, 1),
                    BitField("congested_queue", 0, 1),
                    BitField("path_tracking_flow", 0, 1),
                    XBitField("reserved", 0, 5),
                    FlagsField("rep_md_bits", 0x0, 16, _REP_MD_BITS),
                    XShortField("domain_specific_id", 0x0),
                    XShortField("ds_md_bits", 0x0),
                    XShortField("ds_md_status", 0x0),
                    #PacketListField("variable_optional_metadata", [],
                    #                DTEL_REPORT_V2_VARIABLE_OPTIONAL_METADATA,
                    #                length_from=lambda x: (x.md_length*4))
                    ConditionalField(
                        ShortField("ingress_port", 0x0),
                        #lambda pkt: pkt.rep_md_bits.level_1_if_ids),
                        lambda pkt: pkt.rep_md_bits & 0x4000),
                    ConditionalField(
                        ShortField("egress_port", 0x0),
                        #lambda pkt: pkt.rep_md_bits.level_1_if_ids),
                        lambda pkt: pkt.rep_md_bits & 0x4000),
                    ConditionalField(
                        IntField("hop_latency", 0x0),
                        #lambda pkt: pkt.rep_md_bits.hop_latency),
                        lambda pkt: pkt.rep_md_bits & 0x2000),
                    ConditionalField(
                        ByteField("queue_id", 0x0),
                        #lambda pkt: pkt.rep_md_bits.queue_occupancy),
                        lambda pkt: pkt.rep_md_bits & 0x1000),
                    ConditionalField(
                        ThreeBytesField("queue_depth", 0x0),
                        #lambda pkt: pkt.rep_md_bits.queue_occupancy),
                        lambda pkt: pkt.rep_md_bits & 0x1000),
                    ConditionalField(
                        XLongField("timestamp", 0x0),
                        #lambda pkt: pkt.rep_md_bits.ingress_tstamp),
                        lambda pkt: pkt.rep_md_bits & 0x0800),
                    ConditionalField(
                        XLongField("egress_tstamp", 0x0),
                        #lambda pkt: pkt.rep_md_bits.egress_tstamp),
                        lambda pkt: pkt.rep_md_bits & 0x0400),
                    ConditionalField(
                        IntField("ingress_if_id", 0x0),
                        #lambda pkt: pkt.rep_md_bits.level_2_if_ids),
                        lambda pkt: pkt.rep_md_bits & 0x0200),
                    ConditionalField(
                        IntField("egress_if_id", 0x0),
                        #lambda pkt: pkt.rep_md_bits.level_2_if_ids),
                        lambda pkt: pkt.rep_md_bits & 0x0200),
                    ConditionalField(
                        IntField("eg_port_tx_util", 0x0),
                        #lambda pkt: pkt.rep_md_bits.eg_port_tx_util),
                        lambda pkt: pkt.rep_md_bits & 0x0100),
                    ConditionalField(
                        ByteField("buffer_id", 0x0),
                        #lambda pkt: pkt.rep_md_bits.buffer_occupancy),
                        lambda pkt: pkt.rep_md_bits & 0x0080),
                    ConditionalField(
                        ThreeBytesField("buffer_occupancy", 0x0),
                        #lambda pkt: pkt.rep_md_bits.buffer_occupancy),
                        lambda pkt: pkt.rep_md_bits & 0x0080),
                    ConditionalField(
                        ByteField("drop_queue_id", 0x0),
                        #lambda pkt: pkt.rep_md_bits.drop_reason),
                        lambda pkt: pkt.rep_md_bits & 0x0001),
                    ConditionalField(
                        ByteField("drop_reason", 0x0),
                        #lambda pkt: pkt.rep_md_bits.drop_reason),
                        lambda pkt: pkt.rep_md_bits & 0x0001),
                    ConditionalField(
                        ShortField("drop_reserved", 0x0),
                        #lambda pkt: pkt.rep_md_bits.drop_reason)
                        lambda pkt: pkt.rep_md_bits & 0x0001),
                  ]

    def post_build(self, p, pay):
        p += pay
        tmp_len = self.report_length
        if tmp_len is None:
            tmp_len = (len(p) - 12) // 4
            p = p[:9] + struct.pack("!B", tmp_len) + p[10:]
        return p

    def extract_padding(self, s):
        tmp_len = (self.report_length * 4) + 12
        return s[:tmp_len], s[tmp_len:]

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

    telem_pkt = DTEL_REPORT_V2_HDR(ver = 2,
                                   hw_id = hw_id,
                                   sequence_number = sequence_number,
                                   switch_id = switch_id,
                                   rep_type = 1,
                                   in_type = 3,
                                   report_length = None,
                                   md_length = None,
                                   dropped = dropped,
                                   congested_queue = congested_queue,
                                   path_tracking_flow = path_tracking_flow,
                                   reserved = 0,
                                   rep_md_bits = rep_md_bits,
                                   domain_specific_id = 0,
                                   ds_md_bits = 0,
                                   ds_md_status = 0,
                                   ingress_port = ingress_port,
                                   egress_port = egress_port,
                                   hop_latency = hop_latency,
                                   queue_id = queue_id,
                                   queue_depth = queue_depth,
                                   timestamp = ingress_tstamp,
                                   egress_tstamp = egress_tstamp,
                                   drop_queue_id = drop_queue_id,
                                   drop_reason = drop_reason,
                                   drop_reserved = drop_reserved)

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
DTel Report Header common utilities
"""
def match_dtel_pkt(exp_pkt, pkt, int_v2, ignore_tstamp=True, ignore_seq_num=True,
    ignore_queue_depth=True, ignore_udp_sport=True, ignore_hw_id=True, error_on_zero=True):
    """
    Compare DTel report packets, ignore the timestamp and sequence number
    values. Just make sure that the timestamp is non-zero.
    """
    #check that received packet has a DTel report header
    if int_v2:
        dtel_report = pkt.getlayer(DTEL_REPORT_V2_HDR)
    else:
        dtel_report = pkt.getlayer(DTEL_REPORT_HDR)
    if dtel_report == None:
        #self.logger.error("No DTel report pkt received")
        return False

    #check that timestamp is non-zero
    if error_on_zero and not int_v2 and dtel_report.timestamp == 0:
        #self.logger.error("Invalid DTel report timestamp")
        return False
    if error_on_zero and int_v2 and (dtel_report.rep_md_bits & 0x0800) and dtel_report.timestamp == 0:
        #self.logger.error("Invalid DTel report timestamp")
        return False

    #check that expected packet has a DTel report header
    if int_v2:
        exp_dtel_report = exp_pkt.getlayer(DTEL_REPORT_V2_HDR)
    else:
        exp_dtel_report = exp_pkt.getlayer(DTEL_REPORT_HDR)
    if exp_dtel_report == None:
        #self.logger.error("exp_pkt is not DTel report packet")
        return False

    #ignore timestamp
    if ignore_tstamp:
        exp_dtel_report.timestamp = 0
        dtel_report.timestamp = 0

    #ignore sequence number
    if ignore_seq_num:
        exp_dtel_report.sequence_number = 0
        dtel_report.sequence_number = 0

    #ignore hw id
    if ignore_hw_id:
        exp_dtel_report.hw_id = 0
        dtel_report.hw_id = 0

    #ignore udp source port
    if ignore_udp_sport:
        exp_pkt[UDP].sport = 0
        pkt[UDP].sport = 0

    #ignore queue depth
    try:
        # used round assuming that bridge metadata < 40
        if ignore_queue_depth or \
        (dtel_report.queue_depth <= round(1.*pkt.len/TM_CELL_SIZE) + 1):
            exp_dtel_report.queue_depth = 0
            dtel_report.queue_depth = 0
    except AttributeError:
        pass

    # ignore the chksum
    new_exp_pkt = ptf.mask.Mask(exp_pkt)
    mask_set_do_not_care_packet(new_exp_pkt, IP, "chksum")

    #compare
    return dataplane.match_exp_pkt(new_exp_pkt, pkt)

def receive_dtel_packet(test, pkt, port):
    """
    Check whether an expected packet is received
    """
    logging.debug("Checking for pkt on port %r", port)
    (_, rcv_port, rcv_pkt, pkt_time) = \
            test.dataplane.poll(port_number=port, timeout=ptf.ptfutils.default_timeout, exp_pkt=None)
    if rcv_pkt == None:
        return rcv_pkt
    test.assertTrue(rcv_pkt != None, "Did not receive pkt on %r" % port)
    # convert rcv_pkt string back to layered pkt
    nrcv = pkt.__class__(rcv_pkt)


    if match_dtel_pkt(pkt, nrcv) == False:
        print ("========== EXPECTED ==========")
        pkt.show2()
        hexdump(pkt)
        print ("========== RECEIVED ==========")
        nrcv.show2()
        hexdump(nrcv)
        test.assertTrue(0, "Received packet did not match expected packet")
    return nrcv;

def verify_dtel_packet(test, pkt, port, ignore_tstamp=True,
                       ignore_seq_num=True, ignore_queue_depth=True,
                       ignore_udp_sport=True, ignore_hw_id=True,
                       fail_on_none=True):
    """
    Check that an expected packet is received
    """
    logging.debug("Checking for pkt on port %r", port)
    (_, rcv_port, rcv_pkt, pkt_time) = \
            test.dataplane.poll(port_number=port, timeout=5, exp_pkt=None)
    test.assertTrue(rcv_pkt != None, "Did not receive pkt on %r" % port)
    # convert rcv_pkt string back to layered pkt
    nrcv = pkt.__class__(rcv_pkt)

    if match_dtel_pkt(pkt, nrcv, test.int_v2) == False:
        print ("========== EXPECTED ==========")
        pkt.show2()
        hexdump(pkt)
        print ("========== RECEIVED ==========")
        nrcv.show2()
        hexdump(nrcv)
        test.assertTrue(0, "Received packet did not match expected packet")

def verify_any_dtel_packet_any_port(test, pkts=[], ports=[],
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
    (rcv_device, rcv_port, rcv_pkt, pkt_time) = dp_poll(
        test,
        device_number=device_number,
        timeout=5
    )

    logging.debug("Checking for pkt on device %d, port %r",
                  device_number, ports)
    if rcv_port in ports:
        for i in range(0, len(pkts)):
            pkt = pkts[i]
            nrcv = pkt.__class__(rcv_pkt)
            if test.int_v2:
                if pkt.haslayer(DTEL_REPORT_V2_HDR):
                    seq_num = nrcv[DTEL_REPORT_V2_HDR].sequence_number
                    if not ignore_postcard_values(pkt, nrcv, test.int_v2, True):
                        continue
            else:
                if pkt.haslayer(POSTCARD_HDR):
                    seq_num = nrcv[DTEL_REPORT_HDR].sequence_number
                    if not ignore_postcard_values(pkt, nrcv, test.int_v2, True):
                        continue
            if match_dtel_pkt(pkt, nrcv, test.int_v2,
                              ignore_seq_num = ignore_seq_num):
                match_index = i
                received = True

    test.assertTrue(
        received == True,
        "Did not receive expected pkt(s) on any of ports %r for device %d"
        % (ports, device_number))
    return (match_index, seq_num)

def verify_dtel_packet_any_port(test, pkt=None, ports=[],
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
    (rcv_device, rcv_port, rcv_pkt, pkt_time) = dp_poll(
        test,
        device_number=device_number,
        timeout=5
    )

    logging.debug("Checking for pkt on device %d, port %r",
                  device_number, ports)
    nrcv = pkt.__class__(rcv_pkt)
    if test.int_v2:
        if pkt.haslayer(DTEL_REPORT_V2_HDR):
            seq_num = nrcv[DTEL_REPORT_V2_HDR].sequence_number
    else:
        if pkt.haslayer(POSTCARD_HDR):
            seq_num = nrcv[DTEL_REPORT_HDR].sequence_number
    test.assertTrue(ignore_postcard_values(pkt, nrcv, test.int_v2, True),
                    "Received packet did not match expected postcard packet")
    test.assertTrue(
        match_dtel_pkt(pkt, nrcv, test.int_v2, ignore_seq_num = ignore_seq_num),
        "Did not receive expected pkt on any of ports %r for device %d"
        % (ports, device_number))

    for i in range(0, len(ports)):
        if rcv_port == ports[i]:
            match_index = i
            break

    return (match_index, seq_num)

def receive_packet(test, port_id, timeout=None):
    dev, port = port_to_tuple(port_id)
    if timeout == None:
        timeout = ptf.ptfutils.default_timeout
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
        if exp_e2e_pkt!=None:
            verify_postcard_packet(test, exp_e2e_pkt, exp_report_port)

        verify_no_other_packets(test)

    send_packet(test, in_port, pkt_in)
    # verify mirror on drop packet
    if not drop:
        verify_packet(test, exp_pkt_out, out_port)
        if exp_e2e_pkt!=None:
            verify_postcard_packet(test, exp_e2e_pkt, exp_report_port)
    else:
        if exp_dod_pkt!=None:
            verify_dtel_packet(test, exp_dod_pkt, report_port)
    verify_no_other_packets(test, timeout=1)
