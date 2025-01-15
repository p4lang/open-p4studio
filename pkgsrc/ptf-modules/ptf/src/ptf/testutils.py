import sys
import copy
import logging
import types
import time
import re
from . import packet

import ptf
import ptf.dataplane
import ptf.parse
import ptf.ptfutils
import codecs
from io import StringIO

global skipped_test_count
skipped_test_count = 0

_import_blacklist = set(locals().keys())

# Some useful defines
IP_ETHERTYPE = 0x800
TCP_PROTOCOL = 0x6
UDP_PROTOCOL = 0x11

MINSIZE = 0
TEST_PARAMS = None
PORT_INFO = None

_import_blacklist.add("FILTERS")
FILTERS = []


def reset_filters():
    FILTERS = []


# Needs to be a callable
def add_filter(my_filter):
    FILTERS.append(my_filter)


def get_filters():
    return FILTERS


def ether_filter(pkt_str):
    try:
        pkt = packet.Ether(pkt_str)
        return True
    except:
        return False


def ipv6_filter(pkt_str):
    try:
        pkt = packet.Ether(pkt_str)
        return packet.IPv6 in pkt
    except:
        return False


def not_ipv6_filter(pkt_str):
    return not ipv6_filter(pkt_str)


def ip_make_tos(tos, ecn, dscp):
    if ecn is not None:
        tos = (tos & ~(0x3)) | ecn

    if dscp is not None:
        tos = (tos & ~(0xFC)) | (dscp << 2)

    return tos


def simple_tcp_packet_ext_taglist(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_taglist_enable=False,
    dl_vlan_pcp_list=[0],
    dl_vlan_cfi_list=[0],
    dl_tpid_list=[0x8100],
    dl_vlanid_list=[1],
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_frag=0,
    tcp_sport=1234,
    tcp_dport=80,
    tcp_flags="S",
    ip_ihl=None,
    ip_options=False,
    with_tcp_chksum=True,
):
    """
    Return a simple dataplane TCP packet without tag or with one or multiple tags
    The size of the list of the argument "dl_vlan_pcp_list", "dl_vlan_cfi",
    "dl_tpid_list" and "dl_vlanid_list" must be the same when "dl_taglist_enable" is True.

    Supports a few parameters:
    @param pktlen Length of packet in bytes w/o CRC
    @param eth_dst Destinatino MAC
    @param eth_src Source MAC
    @param dl_taglist_enable False if no tag. True if the packet contains one or multiple tags.
    @param dl_vlan_pcp_list VLAN priority list. Not used when dl_taglist_enable is False.
    @param dl_vlan_cfi_list The VLAN CFI list. Not used when dl_taglist_enable is False.
    @param dl_tpid_list The TPID list. Not used when dl_taglist_enable is False.
    @param dl_vlanid_list The VLAN ID list. Not used when dl_taglist_enable is False.
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param tcp_sport TCP source port
    @param tcp_dport TCP destination port
    @param tcp_flags TCP Control flags
    @param with_tcp_chksum Valid TCP checksum

    Generates a simple TCP request.  Users
    shouldn't assume anything about this packet other than that
    it is a valid ethernet/IP/TCP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    if with_tcp_chksum:
        tcp_hdr = packet.TCP(sport=tcp_sport, dport=tcp_dport, flags=tcp_flags)
    else:
        tcp_hdr = packet.TCP(
            sport=tcp_sport, dport=tcp_dport, flags=tcp_flags, chksum=0
        )

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_taglist_enable:
        pkt = packet.Ether(dst=eth_dst, src=eth_src)

        for i in range(0, len(dl_vlanid_list)):
            pkt = pkt / packet.Dot1Q(
                prio=dl_vlan_pcp_list[i], id=dl_vlan_cfi_list[i], vlan=dl_vlanid_list[i]
            )

        pkt = (
            pkt
            / packet.IP(
                src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, id=ip_id, ihl=ip_ihl
            )
            / tcp_hdr
        )

        for i in range(1, len(dl_tpid_list)):
            pkt[packet.Dot1Q : i].type = dl_tpid_list[i]
        pkt.type = dl_tpid_list[0]

    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    ihl=ip_ihl,
                    frag=ip_frag,
                )
                / tcp_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    ihl=ip_ihl,
                    frag=ip_frag,
                    options=ip_options,
                )
                / tcp_hdr
            )
    pkt = pkt / codecs.decode(
        "".join(["%02x" % (x % 256) for x in range(pktlen - len(pkt))]), "hex"
    )

    return pkt


def simple_tcp_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_frag=0,
    tcp_sport=1234,
    tcp_dport=80,
    tcp_flags="S",
    ip_ihl=None,
    ip_options=False,
    with_tcp_chksum=True,
):
    """
    Return a simple dataplane TCP packet

    Supports a few parameters:
    @param pktlen Length of packet in bytes w/o CRC
    @param eth_dst Destinatino MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param dl_vlan_cfi The VLAN CFI value
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param tcp_dport TCP destination port
    @param tcp_sport TCP source port
    @param tcp_flags TCP Control flags
    @param with_tcp_chksum Valid TCP checksum

    Generates a simple TCP request.  Users
    shouldn't assume anything about this packet other than that
    it is a valid ethernet/IP/TCP frame.
    """

    pcp_list = []
    cfi_list = []
    tpid_list = []
    vlan_list = []

    if dl_vlan_enable:
        pcp_list.append(vlan_pcp)
        cfi_list.append(dl_vlan_cfi)
        tpid_list.append(0x8100)
        vlan_list.append(vlan_vid)

    pkt = simple_tcp_packet_ext_taglist(
        pktlen=pktlen,
        eth_dst=eth_dst,
        eth_src=eth_src,
        dl_taglist_enable=dl_vlan_enable,
        dl_vlan_pcp_list=pcp_list,
        dl_vlan_cfi_list=cfi_list,
        dl_tpid_list=tpid_list,
        dl_vlanid_list=vlan_list,
        ip_src=ip_src,
        ip_dst=ip_dst,
        ip_tos=ip_tos,
        ip_ecn=ip_ecn,
        ip_dscp=ip_dscp,
        ip_ttl=ip_ttl,
        ip_id=ip_id,
        ip_frag=ip_frag,
        tcp_sport=tcp_sport,
        tcp_dport=tcp_dport,
        tcp_flags=tcp_flags,
        ip_ihl=ip_ihl,
        ip_options=ip_options,
        with_tcp_chksum=with_tcp_chksum,
    )
    return pkt


def simple_tcpv6_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    ipv6_src="2001:db8:85a3::8a2e:370:7334",
    ipv6_dst="2001:db8:85a3::8a2e:370:7335",
    ipv6_tc=0,
    ipv6_ecn=None,
    ipv6_dscp=None,
    ipv6_hlim=64,
    ipv6_fl=0,
    tcp_sport=1234,
    tcp_dport=80,
    tcp_flags="S",
):
    """
    Return a simple IPv6/TCP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ecn IPv6 traffic class ECN
    @param ipv6_dscp IPv6 traffic class DSCP
    @param ipv6_ttl IPv6 hop limit
    @param ipv6_fl IPv6 flow label
    @param tcp_dport TCP destination port
    @param tcp_sport TCP source port
    @param tcp_flags TCP Control flags

    Generates a simple TCP request. Users shouldn't assume anything about this
    packet other than that it is a valid ethernet/IPv6/TCP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ipv6_tc = ip_make_tos(ipv6_tc, ipv6_ecn, ipv6_dscp)

    pkt = packet.Ether(dst=eth_dst, src=eth_src)
    if dl_vlan_enable or vlan_vid or vlan_pcp:
        pkt /= packet.Dot1Q(vlan=vlan_vid, prio=vlan_pcp)
    pkt /= packet.IPv6(
        src=ipv6_src, dst=ipv6_dst, fl=ipv6_fl, tc=ipv6_tc, hlim=ipv6_hlim
    )
    pkt /= packet.TCP(sport=tcp_sport, dport=tcp_dport, flags=tcp_flags)
    pkt /= "D" * (pktlen - len(pkt))

    return pkt


def simple_udp_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    udp_sport=1234,
    udp_dport=80,
    ip_ihl=None,
    ip_options=False,
    ip_flag=0,
    ip_id=1,
    with_udp_chksum=True,
    udp_payload=None,
):
    """
    Return a simple dataplane UDP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param udp_dport UDP destination port
    @param udp_sport UDP source port
    @param with_udp_chksum Valid UDP checksum

    Generates a simple UDP packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/UDP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    if with_udp_chksum:
        udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport)
    else:
        udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport, chksum=0)

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, ihl=ip_ihl, id=ip_id
            )
            / udp_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    ihl=ip_ihl,
                    id=ip_id,
                    flags=ip_flag,
                )
                / udp_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    ihl=ip_ihl,
                    options=ip_options,
                    id=ip_id,
                    flags=ip_flag,
                )
                / udp_hdr
            )

    if udp_payload:
        pkt = pkt / udp_payload

    pkt = pkt / codecs.decode(
        "".join(["%02x" % (x % 256) for x in range(pktlen - len(pkt))]), "hex"
    )

    return pkt


def simple_ipv6_sr_packet(
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    ipv6_src="2000::1",
    ipv6_dst="2000::2",
    ipv6_plen=None,
    ipv6_tc=0,
    ipv6_hlim=64,
    ipv6_fl=0,
    srh_seg_left=0,
    srh_first_seg=0,
    srh_flags=0,
    srh_seg_list=[],
    srh_nh=0,
    inner_frame=None,
):
    """
    Return a simple dataplane IPv6 segment routing packet

    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ttl IPv6 hop limit
    @param ipv6_fl IPv6 flow label
    @param srh_seg_left IPV6 SRH segment left
    @param srh_first_seg IPV6 SRH first segment
    @param srh_flags IPV6 SRH flags
    @param srh_nh IPV6 SRH next header
    """

    pkt = packet.Ether(dst=eth_dst, src=eth_src)
    pkt /= packet.IPv6(
        src=ipv6_src,
        dst=ipv6_dst,
        fl=ipv6_fl,
        tc=ipv6_tc,
        nh=43,
        hlim=ipv6_hlim,
        plen=ipv6_plen,
    )
    reserved = (srh_first_seg << 24) + (srh_flags << 8)
    pkt /= packet.IPv6ExtHdrRouting(
        nh=srh_nh,
        type=4,
        segleft=srh_seg_left,
        reserved=reserved,
        addresses=srh_seg_list,
    )
    if inner_frame is not None:
        pkt /= inner_frame

    return pkt


def simple_geneve_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_flags=0x0,
    udp_sport=1234,
    with_udp_chksum=True,
    ip_ihl=None,
    ip_options=False,
    geneve_ver=0x0,
    geneve_reserved=0x0,
    geneve_vni=0x1234,
    geneve_reserved2=0x0,
    geneve_proto=0x6558,
    inner_frame=None,
):
    """
    Return a simple dataplane GENEVE packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param ip_flags IP Flags
    @param udp_sport UDP source port
    @param geneve_ver version
    @param geneve_reserved reserved field
    @param geneve_vni GENEVE Network Identifier
    @param geneve_reserved2 reserved field
    @param inner_frame The inner Ethernet frame
    """
    if packet.GENEVE is None:
        logging.error(
            "A GENEVE packet was requested but GENEVE is not supported by your Scapy. See README for more information"
        )
        return None

    udp_dport = 6081  # UDP port assigned by IANA for GENEVE

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    if with_udp_chksum:
        udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport)
    else:
        udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport, chksum=0)

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
            / udp_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                )
                / udp_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                    options=ip_options,
                )
                / udp_hdr
            )

    pkt = pkt / packet.GENEVE(vni=geneve_vni, proto=geneve_proto)

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / simple_tcp_packet(pktlen=pktlen - len(pkt))

    return pkt


def simple_nvgre_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_flags=0x0,
    ip_ihl=None,
    ip_options=False,
    nvgre_version=0,
    nvgre_tni=None,
    nvgre_flowid=0,
    inner_frame=None,
):
    """
    Return a simple dataplane GRE packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param ip_flags IP Flags
    @param nvgre_version Version
    @param nvgre_tni
    @param nvgre_flowid
    @param inner_frame payload of the GRE packet

    Generates a simple GRE packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/NVGRE frame.
    """
    if packet.NVGRE is None:
        logging.error(
            "A NVGRE packet was requested but NVGRE is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    nvgre_hdr = packet.NVGRE(vsid=nvgre_tni, flowid=nvgre_flowid)

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
            / nvgre_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                )
                / nvgre_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                    options=ip_options,
                )
                / nvgre_hdr
            )

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / packet.IP()
        pkt = pkt / ("D" * (pktlen - len(pkt)))

    return pkt


def simple_vxlan_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_flags=0x0,
    udp_sport=1234,
    udp_dport=4789,
    with_udp_chksum=True,
    ip_ihl=None,
    ip_options=False,
    vxlan_flags=0x08,
    vxlan_reserved0=0,
    vxlan_reserved1=0x000000,
    vxlan_vni=0xABA,
    vxlan_reserved2=0x00,
    inner_frame=None,
):
    """
    Return a simple dataplane VXLAN packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param ip_flags IP Flags
    @param udp_sport UDP source port
    @param udp_dport UDP dest port (IANA) = 4789 (VxLAN)
    @param vxlan_flags Flags
    @param vxlan_reserved0
    @param vxlan_reserved1 reserved field (3B)
    @param vxlan_vni VXLAN Network Identifier
    @param vxlan_reserved2 reserved field (1B)
    @param inner_frame The inner Ethernet frame

    Generates a simple VXLAN packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/UDP/VXLAN frame.
    """
    if packet.VXLAN is None:
        logging.error(
            "A VXLAN packet was requested but VXLAN is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    if with_udp_chksum:
        udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport)
    else:
        udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport, chksum=0)

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
            / udp_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                )
                / udp_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                    options=ip_options,
                )
                / udp_hdr
            )

    pkt = pkt / packet.VXLAN(
        flags=vxlan_flags,
        vni=vxlan_vni,
        reserved0=vxlan_reserved0,
        reserved1=vxlan_reserved1,
        reserved2=vxlan_reserved2,
    )

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / simple_tcp_packet(pktlen=pktlen - len(pkt))

    return pkt


def simple_vxlanv6_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ipv6_src="1::2",
    ipv6_dst="3::4",
    ipv6_fl=0,
    ipv6_tc=0,
    ipv6_ecn=None,
    ipv6_dscp=None,
    ipv6_hlim=64,
    udp_sport=1234,
    udp_dport=4789,
    with_udp_chksum=True,
    vxlan_flags=0x08,
    vxlan_reserved1=0x000000,
    vxlan_vni=0xABA,
    vxlan_reserved2=0x00,
    inner_frame=None,
):
    """
    Return a simple dataplane VXLAN packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_fl IPv6 flowlabel
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ecn IPv6 traffic class ECN
    @param ipv6_dscp IPv6 traffic class DSCP
    @param ipv6_hlim IPv6 hop limit
    @param udp_sport UDP source port
    @param udp_dport UDP dest port (IANA) = 4789 (VxLAN)
    @param vxlan_flags Flags
    @param vxlan_reserved1 reserved field (3B)
    @param vxlan_vni VXLAN Network Identifier
    @param vxlan_reserved2 reserved field (1B)
    @param inner_frame The inner Ethernet frame

    Generates a simple VXLAN packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/UDP/VXLAN frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    if with_udp_chksum:
        udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport)
    else:
        udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport, chksum=0)

    ipv6_tc = ip_make_tos(ipv6_tc, ipv6_ecn, ipv6_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IPv6(
                src=ipv6_src, dst=ipv6_dst, fl=ipv6_fl, tc=ipv6_tc, hlim=ipv6_hlim
            )
            / udp_hdr
        )
    else:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.IPv6(
                src=ipv6_src, dst=ipv6_dst, fl=ipv6_fl, tc=ipv6_tc, hlim=ipv6_hlim
            )
            / udp_hdr
        )

    pkt = pkt / packet.VXLAN(
        flags=vxlan_flags,
        vni=vxlan_vni,
        reserved1=vxlan_reserved1,
        reserved2=vxlan_reserved2,
    )

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / simple_tcp_packet(pktlen=pktlen - len(pkt))

    return pkt


def simple_gre_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_flags=0x0,
    ip_ihl=None,
    ip_options=False,
    gre_chksum_present=0,
    gre_routing_present=0,  # begin reserved0
    gre_key_present=0,
    gre_seqnum_present=0,
    gre_strict_route_source=0,
    gre_flags=0,  # end reserved0
    gre_version=0,
    gre_offset=None,  # reserved1
    gre_key=None,
    gre_sequence_number=None,
    inner_frame=None,
):
    """
    Return a simple dataplane GRE packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param ip_flags IP Flags
    @param gre_chkum_present with or without checksum
    @param gre_routing_present
    @param gre_key_present
    @param gre_seqnum_present
    @param gre_strict_route_source
    @param gre_flags
    @param gre_version Version
    @param gre_offset
    @param gre_key
    @param gre_sequence_number
    @param inner_frame payload of the GRE packet

    Generates a simple GRE packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/GRE frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    # proto (ethertype) is set by Scapy based on the payload
    gre_hdr = packet.GRE(
        chksum_present=gre_chksum_present,
        routing_present=gre_routing_present,
        key_present=gre_key_present,
        seqnum_present=gre_seqnum_present,
        strict_route_source=gre_strict_route_source,
        flags=gre_flags,
        version=gre_version,
        offset=gre_offset,
        key=gre_key,
        sequence_number=gre_sequence_number,
    )

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
            / gre_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                )
                / gre_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                    options=ip_options,
                )
                / gre_hdr
            )

    if inner_frame:
        pkt = pkt / inner_frame
        inner_frame_bytes = bytearray(bytes(inner_frame))
        if (inner_frame_bytes[0] & 0xF0) == 0x60:
            pkt["GRE"].proto = 0x86DD
    else:
        pkt = pkt / packet.IP()
        pkt = pkt / ("D" * (pktlen - len(pkt)))

    return pkt


def simple_grev6_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ipv6_src="1::2",
    ipv6_dst="3::4",
    ipv6_fl=0,
    ipv6_tc=0,
    ipv6_ecn=None,
    ipv6_dscp=None,
    ipv6_hlim=64,
    gre_chksum_present=0,
    gre_routing_present=0,  # begin reserved0
    gre_key_present=0,
    gre_seqnum_present=0,
    gre_strict_route_source=0,
    gre_flags=0,  # end reserved0
    gre_version=0,
    gre_offset=None,  # reserved1
    gre_key=None,
    gre_sequence_number=None,
    inner_frame=None,
):
    """
    Return a simple dataplane GRE packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_fl IPv6 flowlabel
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ecn IPv6 traffic class ECN
    @param ipv6_dscp IPv6 traffic class DSCP
    @param ipv6_hlim IPv6 hop limit
    @param gre_chkum_present with or without checksum
    @param gre_routing_present
    @param gre_key_present
    @param gre_seqnum_present
    @param gre_strict_route_source
    @param gre_flags
    @param gre_version Version
    @param gre_offset
    @param gre_key
    @param gre_sequence_number
    @param inner_frame payload of the GRE packet

    Generates a simple GRE packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/GRE frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    # proto (ethertype) is set by Scapy based on the payload
    gre_hdr = packet.GRE(
        chksum_present=gre_chksum_present,
        routing_present=gre_routing_present,
        key_present=gre_key_present,
        seqnum_present=gre_seqnum_present,
        strict_route_source=gre_strict_route_source,
        flags=gre_flags,
        version=gre_version,
        offset=gre_offset,
        key=gre_key,
        sequence_number=gre_sequence_number,
    )

    ipv6_tc = ip_make_tos(ipv6_tc, ipv6_ecn, ipv6_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IPv6(
                src=ipv6_src,
                dst=ipv6_dst,
                fl=ipv6_fl,
                tc=ipv6_tc,
                hlim=ipv6_hlim,
                nh=47,
            )
            / gre_hdr
        )
    else:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.IPv6(
                src=ipv6_src,
                dst=ipv6_dst,
                fl=ipv6_fl,
                tc=ipv6_tc,
                hlim=ipv6_hlim,
                nh=47,
            )
            / gre_hdr
        )

    if inner_frame:
        pkt = pkt / inner_frame
        inner_frame_bytes = bytearray(bytes(inner_frame))
        if (inner_frame_bytes[0] & 0xF0) == 0x60:
            pkt["GRE"].proto = 0x86DD
    else:
        pkt = pkt / packet.IP()
        pkt = pkt / ("D" * (pktlen - len(pkt)))

    return pkt


def simple_gre_erspan_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_flags=0x0,
    ip_ihl=None,
    ip_options=False,
    gre_chksum_present=0,
    gre_routing_present=0,  # begin reserved0
    gre_key_present=0,
    gre_seqnum_present=0,
    gre_strict_route_source=0,
    gre_flags=0,  # end reserved0
    gre_version=0,
    gre_offset=None,  # reserved1
    gre_key=None,
    gre_sequence_number=None,
    erspan_vlan=0,
    erspan_cos=0,
    erspan_en=0,
    erspan_t=0,
    erspan_session_id=0,
    erspan_index=0,
    inner_frame=None,
):
    """
    Return a simple dataplane GRE/ERSPAN packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param ip_flags IP Flags
    @param gre_chkum_present with or without checksum
    @param gre_routing_present
    @param gre_key_present
    @param gre_seqnum_present
    @param gre_strict_route_source
    @param gre_flags
    @param gre_version Version
    @param gre_offset
    @param gre_key
    @param gre_sequence_number
    @param inner_frame payload of the GRE packet
    @param erspan_vlan
    @param erspan_cos
    @param erspan_en
    @param erspan_t
    @param erspan_session_id
    @param erspan_index

    Generates a simple GRE/ERSPAN packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/GRE/ERSPAN frame.
    """
    if packet.GRE is None or packet.ERSPAN is None:
        logging.error(
            "A GRE/ERSPAN packet was requested but GRE or ERSPAN is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    # proto (ethertype) is set by Scapy based on the payload
    gre_hdr = packet.GRE(
        chksum_present=gre_chksum_present,
        routing_present=gre_routing_present,
        key_present=gre_key_present,
        seqnum_present=gre_seqnum_present,
        strict_route_source=gre_strict_route_source,
        flags=gre_flags,
        version=gre_version,
        offset=gre_offset,
        key=gre_key,
        sequence_number=gre_sequence_number,
    )

    erspan_hdr = packet.ERSPAN(
        vlan=erspan_vlan,
        cos=erspan_cos,
        en=erspan_en,
        t=erspan_t,
        session_id=erspan_session_id,
        index=erspan_index,
    )

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
            / gre_hdr
            / erspan_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                )
                / gre_hdr
                / erspan_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                    options=ip_options,
                )
                / gre_hdr
                / erspan_hdr
            )

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / packet.IP()
        pkt = pkt / ("D" * (pktlen - len(pkt)))

    return pkt


def ipv4_erspan_pkt(
    pktlen=350,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_flags=0x0,
    ip_ihl=None,
    ip_options=False,
    version=2,
    mirror_id=0x3FF,
    sgt_other=0,
    erspan_p=0,
    erspan_ft=0,
    erspan_hw=0,
    erspan_d=0,
    erspan_gra=0,
    erspan_o=0,
    inner_frame=None,
):
    """
    Return a GRE ERSPAN packet

    Supports a few parameters:
    @param pktlen Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param ip_flags IP Flags
    @param ip_ihl IP ihl
    @param ip_options list of IP options (False if don't want to)
    @param version ERSPAN version
    @param mirror_id (mirror_session_id)
    @param sgt_other for ERSPAN_III it is sgt field
    @param erspan_p ERSPAN_III p field
    @param erspan_ft ERSPAN_III ft field
    @param erspan_hw ERSPAN_III hw field
    @param erspan_d ERSPAN_III d field
    @param erspan_gra ERSPAN_III gra field
    @param erspan_o ERSPAN_III o field
    @param inner_frame payload of the GRE packet
    """
    if packet.GRE is None or packet.ERSPAN is None or packet.ERSPAN_III is None:
        logging.error(
            "A GRE/ERSPAN packet was requested but GRE or ERSPAN is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    if version == 2:
        erspan_hdr = packet.GRE(proto=0x22EB) / packet.ERSPAN_III(
            session_id=mirror_id,
            sgt_other=sgt_other,
            p=erspan_p,
            ft=erspan_ft,
            hw=erspan_hw,
            d=erspan_d,
            gra=erspan_gra,
            o=erspan_o,
        )
    else:
        erspan_hdr = packet.GRE(proto=0x88BE) / packet.ERSPAN(session_id=mirror_id)

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
            / erspan_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                )
                / erspan_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                    options=ip_options,
                )
                / erspan_hdr
            )

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / packet.IP()
        pkt = pkt / ("D" * (pktlen - len(pkt)))

    return pkt


def ipv4_erspan_platform_pkt(
    pktlen=350,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_flags=0x0,
    ip_ihl=None,
    ip_options=False,
    version=2,
    mirror_id=0x3FF,
    sgt_other=1,
    erspan_p=0,
    erspan_ft=0,
    erspan_hw=0,
    erspan_d=0,
    erspan_gra=0,
    erspan_o=1,
    platf_id=0,
    info1=0,
    info2=0,
    inner_frame=None,
):
    """
    Return a GRE ERSPAN packet with Platform Specific Subheader

    Supports a few parameters:
    @param pktlen Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param ip_flags IP Flags
    @param ip_ihl IP ihl
    @param ip_options list of IP options (False if don't want to)
    @param version ERSPAN version
    @param mirror_id (mirror_session_id)
    @param sgt_other for ERSPAN_III it is sgt field
    @param erspan_p ERSPAN_III p field
    @param erspan_ft ERSPAN_III ft field
    @param erspan_hw ERSPAN_III hw field
    @param erspan_d ERSPAN_III d field
    @param erspan_gra ERSPAN_III gra field
    @param erspan_o ERSPAN_III o field
    @param platf_id Specific Platform Subheader Platf Id
    @param info1 Specific Platform Subheader 26 bit field
    @param info2 Specific Platform Subheader 32 bit field
    @param inner_frame payload of the GRE packet
    """
    if (
        packet.GRE is None
        or packet.ERSPAN is None
        or packet.ERSPAN_III is None
        or packet.PlatformSpecific is None
    ):
        logging.error(
            "A GRE/ERSPAN packet was requested but GRE or ERSPAN is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    if version == 2:
        erspan_hdr = packet.GRE(proto=0x22EB) / packet.ERSPAN_III(
            session_id=mirror_id,
            sgt_other=sgt_other,
            p=erspan_p,
            ft=erspan_ft,
            hw=erspan_hw,
            d=erspan_d,
            gra=erspan_gra,
            o=erspan_o,
        )
        if sgt_other & 0x01 == 1:
            erspan_hdr = erspan_hdr / packet.PlatformSpecific(
                platf_id=platf_id, info1=info1, info2=info2
            )
    else:
        erspan_hdr = packet.GRE(proto=0x88BE) / packet.ERSPAN(session_id=mirror_id)

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
            / erspan_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                )
                / erspan_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    id=ip_id,
                    flags=ip_flags,
                    ihl=ip_ihl,
                    options=ip_options,
                )
                / erspan_hdr
            )

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / packet.IP()
        pkt = pkt / ("D" * (pktlen - len(pkt)))

    return pkt


def simple_udpv6_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    ipv6_src="2001:db8:85a3::8a2e:370:7334",
    ipv6_dst="2001:db8:85a3::8a2e:370:7335",
    ipv6_tc=0,
    ipv6_ecn=None,
    ipv6_dscp=None,
    ipv6_hlim=64,
    ipv6_fl=0,
    udp_sport=1234,
    udp_dport=80,
    with_udp_chksum=True,
    udp_payload=None,
):
    """
    Return a simple IPv6/UDP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ecn IPv6 traffic class ECN
    @param ipv6_dscp IPv6 traffic class DSCP
    @param ipv6_ttl IPv6 hop limit
    @param ipv6_fl IPv6 flow label
    @param udp_dport UDP destination port
    @param udp_sport UDP source port
    @param with_udp_chksum Valid UDP checksum

    Generates a simple UDP request. Users shouldn't assume anything about this
    packet other than that it is a valid ethernet/IPv6/UDP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ipv6_tc = ip_make_tos(ipv6_tc, ipv6_ecn, ipv6_dscp)
    pkt = packet.Ether(dst=eth_dst, src=eth_src)
    if dl_vlan_enable or vlan_vid or vlan_pcp:
        pkt /= packet.Dot1Q(vlan=vlan_vid, prio=vlan_pcp)
    pkt /= packet.IPv6(
        src=ipv6_src, dst=ipv6_dst, fl=ipv6_fl, tc=ipv6_tc, hlim=ipv6_hlim
    )
    if with_udp_chksum:
        pkt /= packet.UDP(sport=udp_sport, dport=udp_dport)
    else:
        pkt /= packet.UDP(sport=udp_sport, dport=udp_dport, chksum=0)
    if udp_payload:
        pkt = pkt / udp_payload
    pkt /= "D" * (pktlen - len(pkt))

    return pkt


def simple_ipv4ip_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_flags=0x0,
    ip_ihl=None,
    ip_options=False,
    inner_frame=None,
):
    """
    Return a simple dataplane IPv4 encapsulated packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param ip_flags IP Flags
    @param inner_frame payload of the packet

    Generates a simple IPv4 encapsulated packet.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
        )
    else:
        if not ip_options:
            pkt = packet.Ether(dst=eth_dst, src=eth_src) / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
            )
        else:
            pkt = packet.Ether(dst=eth_dst, src=eth_src) / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                flags=ip_flags,
                ihl=ip_ihl,
                options=ip_options,
            )

    if inner_frame:
        pkt = pkt / inner_frame
        inner_frame_bytes = bytearray(bytes(inner_frame))
        if (inner_frame_bytes[0] & 0xF0) == 0x40:
            pkt["IP"].proto = 4
        elif (inner_frame_bytes[0] & 0xF0) == 0x60:
            pkt["IP"].proto = 41
    else:
        pkt = pkt / packet.IP()
        pkt = pkt / ("D" * (pktlen - len(pkt)))
        pkt["IP"].proto = 4

    return pkt


def simple_ipv6ip_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ipv6_src="1::2",
    ipv6_dst="3::4",
    ipv6_fl=0,
    ipv6_tc=0,
    ipv6_ecn=None,
    ipv6_dscp=None,
    ipv6_hlim=64,
    inner_frame=None,
):
    """
    Return a simple dataplane IPv6 encapsulated packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_fl IPv6 flowlabel
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ecn IPv6 traffic class ECN
    @param ipv6_dscp IPv6 traffic class DSCP
    @param ipv6_hlim IPv6 hop limit
    @param inner_frame payload of the GRE packet

    Generates a simple IPv6 encapsulated packet.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ipv6_tc = ip_make_tos(ipv6_tc, ipv6_ecn, ipv6_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IPv6(
                src=ipv6_src, dst=ipv6_dst, fl=ipv6_fl, tc=ipv6_tc, hlim=ipv6_hlim
            )
        )
    else:
        pkt = packet.Ether(dst=eth_dst, src=eth_src) / packet.IPv6(
            src=ipv6_src, dst=ipv6_dst, fl=ipv6_fl, tc=ipv6_tc, hlim=ipv6_hlim
        )

    if inner_frame:
        pkt = pkt / inner_frame
        inner_frame_bytes = bytearray(bytes(inner_frame))
        if (inner_frame_bytes[0] & 0xF0) == 0x40:
            pkt["IPv6"].nh = 4
        elif (inner_frame_bytes[0] & 0xF0) == 0x60:
            pkt["IPv6"].nh = 41
    else:
        pkt = pkt / packet.IP()
        pkt = pkt / ("D" * (pktlen - len(pkt)))
        pkt["IPv6"].nh = 4

    return pkt


def simple_icmp_packet(
    pktlen=60,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=1,
    icmp_type=8,
    icmp_code=0,
    icmp_data="",
):
    """
    Return a simple ICMP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destinatino MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP Identification
    @param icmp_type ICMP type
    @param icmp_code ICMP code
    @param icmp_data ICMP data

    Generates a simple ICMP ECHO REQUEST.  Users
    shouldn't assume anything about this packet other than that
    it is a valid ethernet/ICMP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=0, vlan=vlan_vid)
            / packet.IP(src=ip_src, dst=ip_dst, ttl=ip_ttl, tos=ip_tos, id=ip_id)
            / packet.ICMP(type=icmp_type, code=icmp_code)
            / icmp_data
        )
    else:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.IP(src=ip_src, dst=ip_dst, ttl=ip_ttl, tos=ip_tos, id=ip_id)
            / packet.ICMP(type=icmp_type, code=icmp_code)
            / icmp_data
        )

    pkt = pkt / ("0" * (pktlen - len(pkt)))

    return pkt


def simple_icmpv6_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    ipv6_src="2001:db8:85a3::8a2e:370:7334",
    ipv6_dst="2001:db8:85a3::8a2e:370:7335",
    ipv6_tc=0,
    ipv6_ecn=None,
    ipv6_dscp=None,
    ipv6_hlim=64,
    ipv6_fl=0,
    icmp_type=8,
    icmp_code=0,
):
    """
    Return a simple ICMPv6 packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ecn IPv6 traffic class ECN
    @param ipv6_dscp IPv6 traffic class DSCP
    @param ipv6_ttl IPv6 hop limit
    @param ipv6_fl IPv6 flow label
    @param icmp_type ICMP type
    @param icmp_code ICMP code

    Generates a simple ICMP ECHO REQUEST. Users shouldn't assume anything
    about this packet other than that it is a valid ethernet/IPv6/ICMP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ipv6_tc = ip_make_tos(ipv6_tc, ipv6_ecn, ipv6_dscp)

    pkt = packet.Ether(dst=eth_dst, src=eth_src)
    if dl_vlan_enable or vlan_vid or vlan_pcp:
        pkt /= packet.Dot1Q(vlan=vlan_vid, prio=vlan_pcp)
    pkt /= packet.IPv6(
        src=ipv6_src, dst=ipv6_dst, fl=ipv6_fl, tc=ipv6_tc, hlim=ipv6_hlim
    )
    pkt /= packet.ICMPv6Unknown(type=icmp_type, code=icmp_code)
    pkt /= "D" * (pktlen - len(pkt))

    return pkt


def simple_ipv6_mld_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ipv6_src="1::2",
    ipv6_dst="3::4",
    ipv6_fl=0,
    ipv6_tc=0,
    ipv6_ecn=None,
    ipv6_dscp=None,
    ipv6_hlim=64,
    mld_type=130,
    mld_mladdr="ff02::16",
    mld_mrd=64,
    inner_frame=None,
):
    """
    Return a simple dataplane IPv6 MLD packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_fl IPv6 flowlabel
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ecn IPv6 traffic class ECN
    @param ipv6_dscp IPv6 traffic class DSCP
    @param ipv6_hlim IPv6 hop limit
    @param mld_type IPv6 MLD type
    @param mld_mladdr IPv6 MLD multicast address
    @param mld_mrd IPv6 MLD maximum response delay
    @param inner_frame The inner Ethernet frame

    Generates a simple IPv6 MLD packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IPv6/MLD frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ipv6_tc = ip_make_tos(ipv6_tc, ipv6_ecn, ipv6_dscp)
    # Next header should be 58 for IPv6 MLD
    next_header = 58

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IPv6(
                src=ipv6_src,
                dst=ipv6_dst,
                fl=ipv6_fl,
                tc=ipv6_tc,
                hlim=ipv6_hlim,
                nh=next_header,
            )
        )
    else:
        pkt = packet.Ether(dst=eth_dst, src=eth_src) / packet.IPv6(
            src=ipv6_src,
            dst=ipv6_dst,
            fl=ipv6_fl,
            tc=ipv6_tc,
            hlim=ipv6_hlim,
            nh=next_header,
        )

    pkt = pkt / packet.ICMPv6MLReport(type=mld_type, mladdr=mld_mladdr, mrd=mld_mrd)

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / simple_tcp_packet(pktlen=pktlen - len(pkt))

    return pkt


def simple_arp_packet(
    pktlen=60,
    eth_dst="ff:ff:ff:ff:ff:ff",
    eth_src="00:06:07:08:09:0a",
    vlan_vid=0,
    vlan_pcp=0,
    arp_op=1,
    ip_snd="192.168.0.1",
    ip_tgt="192.168.0.2",
    hw_snd="00:06:07:08:09:0a",
    hw_tgt="00:00:00:00:00:00",
):
    """
    Return a simple ARP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destinatino MAC
    @param eth_src Source MAC
    @param arp_op Operation (1=request, 2=reply)
    @param ip_snd Sender IP
    @param ip_tgt Target IP
    @param hw_snd Sender hardware address
    @param hw_tgt Target hardware address

    Generates a simple ARP REQUEST.  Users
    shouldn't assume anything about this packet other than that
    it is a valid ethernet/ARP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    pkt = packet.Ether(dst=eth_dst, src=eth_src)
    if vlan_vid or vlan_pcp:
        pkt /= packet.Dot1Q(vlan=vlan_vid, prio=vlan_pcp)
    pkt /= packet.ARP(hwsrc=hw_snd, hwdst=hw_tgt, pdst=ip_tgt, psrc=ip_snd, op=arp_op)

    pkt = pkt / ("\0" * (pktlen - len(pkt)))

    return pkt


def simple_eth_packet(
    pktlen=60, eth_dst="00:01:02:03:04:05", eth_src="00:06:07:08:09:0a", eth_type=0x88CC
):

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    pkt = packet.Ether(dst=eth_dst, src=eth_src, type=eth_type)

    pkt = pkt / ("0" * (pktlen - len(pkt)))

    return pkt


def simple_eth_raw_packet_with_taglist(
    pktlen=60,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_taglist_enable=False,
    dl_vlan_pcp_list=[0],
    dl_vlan_cfi_list=[0],
    dl_tpid_list=[0x8100],
    dl_vlanid_list=[1],
):
    """
    Return a simple dataplane Layer 2 packet without tag or with one or multiple tags
    The size of the list of the argument "dl_vlan_pcp_list", "dl_vlan_cfi",
    "dl_tpid_list" and "dl_vlanid_list" must be the same when "dl_taglist_enable" is True.

    Supports a few parameters:
    @param pktlen Length of packet in bytes w/o CRC
    @param eth_dst Destinatino MAC
    @param eth_src Source MAC
    @param dl_taglist_enable False if no tag. True if the packet contains one or multiple tags.
    @param dl_vlan_pcp_list VLAN priority list. Not used when dl_taglist_enable is False.
    @param dl_vlan_cfi_list The VLAN CFI list. Not used when dl_taglist_enable is False.
    @param dl_tpid_list The TPID list. Not used when dl_taglist_enable is False.
    @param dl_vlanid_list The VLAN ID list. Not used when dl_taglist_enable is False.

    Generates a simple Layer 2 packet.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    pkt = packet.Ether(dst=eth_dst, src=eth_src)

    if dl_taglist_enable:
        for i in range(0, len(dl_vlanid_list)):
            pkt = pkt / packet.Dot1Q(
                prio=dl_vlan_pcp_list[i], id=dl_vlan_cfi_list[i], vlan=dl_vlanid_list[i]
            )

        for i in range(1, len(dl_tpid_list)):
            pkt[packet.Dot1Q : i].type = dl_tpid_list[i]
        pkt.type = dl_tpid_list[0]
        pkt[packet.Dot1Q : len(dl_tpid_list)].type = pktlen - len(pkt)
    else:
        pkt.type = pktlen - len(pkt)

    # Fill payload length
    pkt = pkt / codecs.decode(
        "".join(["%02x" % (x % 256) for x in range(pktlen - len(pkt))]), "hex"
    )
    return pkt


def simple_ip_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_ihl=None,
    ip_options=False,
    ip_proto=0,
):
    """
    Return a simple dataplane IP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destinatino MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID

    Generates a simple IP packet.  Users
    shouldn't assume anything about this packet other than that
    it is a valid ethernet/IP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                ihl=ip_ihl,
                proto=ip_proto,
            )
        )
    else:
        if not ip_options:
            pkt = packet.Ether(dst=eth_dst, src=eth_src) / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                ihl=ip_ihl,
                proto=ip_proto,
            )
        else:
            pkt = packet.Ether(dst=eth_dst, src=eth_src) / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                ihl=ip_ihl,
                proto=ip_proto,
                options=ip_options,
            )

    pkt = pkt / codecs.decode(
        "".join(["%02x" % (x % 256) for x in range(pktlen - len(pkt))]), "hex"
    )

    return pkt


def simple_ip_only_packet(
    pktlen=100,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_ihl=None,
    ip_options=False,
    tcp_sport=1234,
    tcp_dport=80,
    tcp_flags="S",
    with_tcp_chksum=True,
):
    """
    Return a simple dataplane IP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID

    Generates a simple IP packet.  Users
    shouldn't assume anything about this packet other than that
    it is a valid IP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    if with_tcp_chksum:
        tcp_hdr = packet.TCP(sport=tcp_sport, dport=tcp_dport, flags=tcp_flags)
    else:
        tcp_hdr = packet.TCP(
            sport=tcp_sport, dport=tcp_dport, flags=tcp_flags, chksum=0
        )

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    if not ip_options:
        pkt = (
            packet.IP(
                src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, id=ip_id, ihl=ip_ihl
            )
            / tcp_hdr
        )
    else:
        pkt = (
            packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                ihl=ip_ihl,
                options=ip_options,
            )
            / tcp_hdr
        )

    pkt = pkt / codecs.decode(
        "".join(["%02x" % (x % 256) for x in range(pktlen - len(pkt))]), "hex"
    )

    return pkt


def simple_mpls_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    mpls_type=0x8847,
    mpls_tags=[],
    dl_vlan_cfi=0,
    inner_frame=None,
):
    """
    Return a simple dataplane MPLS packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param mpls_tags mpls tag stack
    @param inner_frame The inner frame

    """
    if packet.MPLS is None:
        logging.error(
            "A MPLS packet was requested but MPLS is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    pkt = packet.Ether(dst=eth_dst, src=eth_src)
    pkt[packet.Ether].setfieldval("type", mpls_type)

    if dl_vlan_enable:
        pkt / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
        pkt[packet.Dot1Q].setfieldval("type", mpls_type)

    mpls_tags = list(mpls_tags)
    while len(mpls_tags):
        tag = mpls_tags.pop(0)
        mpls = packet.MPLS()
        if "label" in tag:
            mpls.label = tag["label"]
        if "tc" in tag:
            mpls.cos = tag["tc"]
        if "ttl" in tag:
            mpls.ttl = tag["ttl"]
        if "s" in tag:
            mpls.s = tag["s"]
        pkt = pkt / mpls

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / simple_tcp_packet(pktlen=pktlen - len(pkt))

    return pkt


def simple_qinq_tcp_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_outer=20,
    dl_vlan_pcp_outer=0,
    dl_vlan_cfi_outer=0,
    vlan_vid=10,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    tcp_sport=1234,
    tcp_dport=80,
    ip_ihl=None,
    ip_options=False,
):
    """
    Return a doubly tagged dataplane TCP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destinatino MAC
    @param eth_src Source MAC
    @param dl_vlan_outer Outer VLAN ID
    @param dl_vlan_pcp_outer Outer VLAN priority
    @param dl_vlan_cfi_outer Outer VLAN cfi bit
    @param vlan_vid Inner VLAN ID
    @param vlan_pcp VLAN priority
    @param dl_vlan_cfi VLAN cfi bit
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param tcp_dport TCP destination port
    @param ip_sport TCP source port

    Generates a TCP request.  Users
    shouldn't assume anything about this packet other than that
    it is a valid ethernet/IP/TCP frame.
    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    pkt = (
        packet.Ether(dst=eth_dst, src=eth_src)
        / packet.Dot1Q(prio=dl_vlan_pcp_outer, id=dl_vlan_cfi_outer, vlan=dl_vlan_outer)
        / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
        / packet.IP(src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, ihl=ip_ihl)
        / packet.TCP(sport=tcp_sport, dport=tcp_dport)
    )

    pkt = pkt / codecs.decode(
        "".join(["%02x" % (x % 256) for x in range(pktlen - len(pkt))]), "hex"
    )

    return pkt


def simple_igmp_packet(
    pktlen=300,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    ip_id=0x0001,
    ip_ihl=None,
    ip_options=False,
    igmp_type=0x12,
    igmp_gaddr="224.2.3.4",
    igmp_mrtime=64,
    inner_frame=None,
):
    """
    Return a simple dataplane IGMP packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param igmp_type IGMP type
    @param igmp_gaddr IGMP group addr
    @param igmp_mrtime IGMP maximum response time
    @param inner_frame The inner Ethernet frame

    Generates a simple IGMP packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/IGMP frame.
    """
    if packet.IGMP is None:
        logging.error(
            "An IGMP packet was requested but IGMP is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, id=ip_id, ihl=ip_ihl
            )
        )
    else:
        if not ip_options:
            pkt = packet.Ether(dst=eth_dst, src=eth_src) / packet.IP(
                src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, id=ip_id, ihl=ip_ihl
            )
        else:
            pkt = packet.Ether(dst=eth_dst, src=eth_src) / packet.IP(
                src=ip_src,
                dst=ip_dst,
                tos=ip_tos,
                ttl=ip_ttl,
                id=ip_id,
                ihl=ip_ihl,
                options=ip_options,
            )

    pkt = pkt / packet.IGMP(type=igmp_type, gaddr=igmp_gaddr, mrcode=igmp_mrtime)

    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / simple_tcp_packet(pktlen=pktlen - len(pkt))

    return pkt


"""
   DHCP Packet Creation functions

    BOOTP params explained:
    op:     Operation Code, 1 = request (client), 2 = reply (server)
    htype:  Hardware Type, 1 = Ethernet
    hlen:   Hardware Address Length, 6 bytes for Ethernet
    hops:   Incremented by relay agent when forwarding messages.
            Client sends 0 in requests.
    xid:    Transaction Identifier, 32 bit field that identifies transaction
    secs:   Time elapsed since client started trying to boot in seconds
    flags:  Flags, 16 bits; MSB is broadcast flag, set by client that
            doesn't know its own IP yet, indicating that server should broadcast reply
    ciaddr: Client IP Address, if client has a current IP address otherwise set to zeros
    yiaddr: "Your" IP Address, address that server is offering to client
    siaddr: Server IP Address, address of server
    giaddr: Gateway IP Address, address of relay agent if encountered

"""

DHCP_MAC_BROADCAST = "ff:ff:ff:ff:ff:ff"
DHCP_IP_BROADCAST = "255.255.255.255"
DHCP_IP_DEFAULT_ROUTE = "0.0.0.0"
DHCP_PORT_CLIENT = 68
DHCP_PORT_SERVER = 67
DHCP_LEASE_TIME_OFFSET = 292
DHCP_LEASE_TIME_LEN = 6
DHCP_LEASE_TIME = 86400
DHCP_ETHER_TYPE_IP = 0x0800
DHCP_BOOTP_OP_REQUEST = 1
DHCP_BOOTP_OP_REPLY = 2
DHCP_BOOTP_HTYPE_ETHERNET = 1
DHCP_BOOTP_HLEN_ETHERNET = 6
DHCP_BOOTP_FLAGS_BROADCAST_REPLY = 0x8000


def __dhcp_mac_to_chaddr(mac_addr="00:01:02:03:04:05"):
    """
    Private helper function to convert a 6-byte MAC address of form:
      '00:01:02:03:04:05'
    into a 16-byte chaddr byte string of form:
      '\x00\x01\x02\x03\x04\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'

    """
    chaddr = "".join([chr(int(octet, 16)) for octet in mac_addr.split(":")])
    chaddr += "\x00" * 10
    return chaddr


def dhcp_discover_packet(eth_client="00:01:02:03:04:05", set_broadcast_bit=False):
    """
    Return a DHCPDISCOVER packet

    Supported parameters:
    @param eth_client MAC address of DHCP client


    Destination MAC is always broadcast (ff:ff:ff:ff:ff:ff)
    Source IP is always default route IP (0.0.0.0)
    Destination IP is always broadcast (255.255.255.255)
    Source port is always 68 (DHCP client port)
    Destination port is always 67 (DHCP server port)

    """

    pkt = packet.Ether(dst=DHCP_MAC_BROADCAST, src=eth_client, type=DHCP_ETHER_TYPE_IP)
    pkt /= packet.IP(src=DHCP_IP_DEFAULT_ROUTE, dst=DHCP_IP_BROADCAST)
    pkt /= packet.UDP(sport=DHCP_PORT_CLIENT, dport=DHCP_PORT_SERVER)
    pkt /= packet.BOOTP(
        op=DHCP_BOOTP_OP_REQUEST,
        htype=DHCP_BOOTP_HTYPE_ETHERNET,
        hlen=DHCP_BOOTP_HLEN_ETHERNET,
        hops=0,
        xid=0,
        secs=0,
        flags=DHCP_BOOTP_FLAGS_BROADCAST_REPLY if set_broadcast_bit else 0,
        ciaddr=DHCP_IP_DEFAULT_ROUTE,
        yiaddr=DHCP_IP_DEFAULT_ROUTE,
        siaddr=DHCP_IP_DEFAULT_ROUTE,
        giaddr=DHCP_IP_DEFAULT_ROUTE,
        chaddr=__dhcp_mac_to_chaddr(eth_client),
    )
    pkt /= packet.DHCP(options=[("message-type", "discover"), ("end")])
    return pkt


def dhcp_offer_packet(
    eth_server="00:01:02:03:04:05",
    eth_dst="06:07:08:09:10:11",
    eth_client="12:13:14:15:16:17",
    ip_server="0.1.2.3",
    ip_dst="255.255.255.255",
    ip_offered="8.9.10.11",
    port_dst=DHCP_PORT_CLIENT,
    netmask_client="255.255.255.0",
    ip_gateway=DHCP_IP_DEFAULT_ROUTE,
    dhcp_lease=256,
    padding_bytes=0,
    set_broadcast_bit=False,
):
    """
    Return a DHCPOFFER packet

    Supports a few parameters:
    @param eth_server MAC address of DHCP server
    @param eth_dst MAC address of destination (DHCP Client, Relay agent) or broadcast (ff:ff:ff:ff:ff:ff)
    @param eth_client MAC address of DHCP client
    @param ip_server IP address of DHCP server
    @param ip_dst IP address of destination (DHCP Client, Relay agent) or broadcast (255.255.255.255)
    @param ip_offered IP address that server is assigning to client
    @param ip_gateway Gateway IP Address, address of relay agent if encountered
    @param port_dst Destination port of packet (default: DHCP_PORT_CLIENT)
    @param netmask_client Subnet mask of client
    @param dhcp_lease Time in seconds of DHCP lease
    @param padding_bytes Number of '\x00' bytes to append to end of packet


    Destination IP can be unicast or broadcast (255.255.255.255)
    Source port is always 67 (DHCP server port)
    Destination port by default is 68 (DHCP client port), but can be also be 67 (DHCP server port) if being sent to a DHCP relay agent

    """

    ip_tos = ip_make_tos(tos=16, ecn=None, dscp=None)

    pkt = packet.Ether(dst=eth_dst, src=eth_server, type=DHCP_ETHER_TYPE_IP)
    pkt /= packet.IP(src=ip_server, dst=ip_dst, tos=ip_tos, ttl=128, id=0)
    pkt /= packet.UDP(sport=DHCP_PORT_SERVER, dport=port_dst)
    pkt /= packet.BOOTP(
        op=DHCP_BOOTP_OP_REPLY,
        htype=DHCP_BOOTP_HTYPE_ETHERNET,
        hlen=DHCP_BOOTP_HLEN_ETHERNET,
        hops=0,
        xid=0,
        secs=0,
        flags=DHCP_BOOTP_FLAGS_BROADCAST_REPLY if set_broadcast_bit else 0,
        ciaddr=DHCP_IP_DEFAULT_ROUTE,
        yiaddr=ip_offered,
        siaddr=ip_server,
        giaddr=ip_gateway,
        chaddr=__dhcp_mac_to_chaddr(eth_client),
    )
    pkt /= packet.DHCP(
        options=[
            ("message-type", "offer"),
            ("server_id", ip_server),
            ("lease_time", int(dhcp_lease)),
            ("subnet_mask", netmask_client),
            ("end"),
        ]
    )
    pkt /= packet.PADDING("\x00" * padding_bytes)
    return pkt


def dhcp_request_packet(
    eth_client="00:01:02:03:04:05",
    ip_server="0.1.2.3",
    ip_requested="4.5.6.7",
    set_broadcast_bit=False,
):
    """
    Return a DHCPREQUEST packet

    Supports a few parameters:
    @param eth_client MAC address of DHCP client
    @param ip_server IP address of DHCP server
    @param ip_requested IP Address offered to client ('Your IP Address' from DHCPOFFER message)


    Destination MAC is always broadcast (ff:ff:ff:ff:ff:ff)
    Source IP is always default route IP (0.0.0.0)
    Destination IP is always broadcast (255.255.255.255)
    Source port is always 68 (DHCP client port)
    Destination port is always 67 (DHCP server port)

    """

    pkt = packet.Ether(dst=DHCP_MAC_BROADCAST, src=eth_client, type=DHCP_ETHER_TYPE_IP)
    pkt /= packet.IP(src=DHCP_IP_DEFAULT_ROUTE, dst=DHCP_IP_BROADCAST)
    pkt /= packet.UDP(sport=DHCP_PORT_CLIENT, dport=DHCP_PORT_SERVER)
    pkt /= packet.BOOTP(
        op=DHCP_BOOTP_OP_REQUEST,
        htype=DHCP_BOOTP_HTYPE_ETHERNET,
        hlen=DHCP_BOOTP_HLEN_ETHERNET,
        hops=0,
        xid=0,
        secs=0,
        flags=DHCP_BOOTP_FLAGS_BROADCAST_REPLY if set_broadcast_bit else 0,
        ciaddr=DHCP_IP_DEFAULT_ROUTE,
        yiaddr=DHCP_IP_DEFAULT_ROUTE,
        siaddr=DHCP_IP_DEFAULT_ROUTE,
        giaddr=DHCP_IP_DEFAULT_ROUTE,
        chaddr=__dhcp_mac_to_chaddr(eth_client),
    )
    pkt /= packet.DHCP(
        options=[
            ("message-type", "request"),
            ("requested_addr", ip_requested),
            ("server_id", ip_server),
            ("end"),
        ]
    )
    return pkt


def dhcp_ack_packet(
    eth_server="00:01:02:03:04:05",
    eth_dst="06:07:08:09:10:11",
    eth_client="12:13:14:15:16:17",
    ip_server="0.1.2.3",
    ip_dst="255.255.255.255",
    ip_offered="8.9.10.11",
    port_dst=DHCP_PORT_CLIENT,
    netmask_client="255.255.255.0",
    ip_gateway=DHCP_IP_DEFAULT_ROUTE,
    dhcp_lease=256,
    padding_bytes=0,
    set_broadcast_bit=False,
):
    """
    Return a DHCPACK packet

    Supports a few parameters:
    @param eth_server MAC address of DHCP server
    @param eth_dst MAC address of destination (DHCP Client, Relay agent) or broadcast (ff:ff:ff:ff:ff:ff)
    @param eth_client MAC address of DHCP client
    @param ip_server IP address of DHCP server
    @param ip_dst IP address of destination (DHCP Client, Relay agent) or broadcast (255.255.255.255)
    @param ip_offered IP address that server is assigning to client
    @param ip_gateway Gateway IP Address, address of relay agent if encountered
    @param port_dst Destination port of packet (default: DHCP_PORT_CLIENT)
    @param netmask_client Subnet mask of client
    @param dhcp_lease Time in seconds of DHCP lease
    @param padding_bytes Number of '\x00' bytes to append to end of packet


    Destination IP can be unicast or broadcast (255.255.255.255)
    Source port is always 67 (DHCP server port)
    Destination port by default is 68 (DHCP client port), but can be also be 67 (DHCP server port) if being sent to a DHCP relay agent

    """

    ip_tos = ip_make_tos(tos=16, ecn=None, dscp=None)

    pkt = packet.Ether(dst=eth_dst, src=eth_server, type=DHCP_ETHER_TYPE_IP)
    pkt /= packet.IP(src=ip_server, dst=ip_dst, tos=ip_tos, ttl=128, id=0)
    pkt /= packet.UDP(sport=DHCP_PORT_SERVER, dport=port_dst)
    pkt /= packet.BOOTP(
        op=DHCP_BOOTP_OP_REPLY,
        htype=DHCP_BOOTP_HTYPE_ETHERNET,
        hlen=DHCP_BOOTP_HLEN_ETHERNET,
        hops=0,
        xid=0,
        secs=0,
        flags=DHCP_BOOTP_FLAGS_BROADCAST_REPLY if set_broadcast_bit else 0,
        ciaddr=DHCP_IP_DEFAULT_ROUTE,
        yiaddr=ip_offered,
        siaddr=ip_server,
        giaddr=ip_gateway,
        chaddr=__dhcp_mac_to_chaddr(eth_client),
    )
    pkt /= packet.DHCP(
        options=[
            ("message-type", "ack"),
            ("server_id", ip_server),
            ("lease_time", int(dhcp_lease)),
            ("subnet_mask", netmask_client),
            ("end"),
        ]
    )
    pkt /= packet.PADDING("\x00" * padding_bytes)
    return pkt


def dhcp_release_packet(
    eth_client="00:01:02:03:04:05", ip_client="0.1.2.3", ip_server="1.2.3.4"
):
    """
    Return a DHCPRELEASE packet

    Supports a few parameters:
    @param eth_client MAC address of DHCP client requesting release
    @param ip_client IP address of DHCP client requesting release
    @param ip_server IP address of DHCP server


    Destination MAC is always broadcast (ff:ff:ff:ff:ff:ff)
    Source IP is always default route IP (0.0.0.0)
    Destination IP is always broadcast (255.255.255.255)
    Source port is always 68 (DHCP client port)
    Destination port is always 67 (DHCP server port)

    """

    pkt = packet.Ether(dst=DHCP_MAC_BROADCAST, src=eth_client, type=DHCP_ETHER_TYPE_IP)
    pkt /= packet.IP(src=DHCP_IP_DEFAULT_ROUTE, dst=DHCP_IP_BROADCAST)
    pkt /= packet.UDP(sport=DHCP_PORT_CLIENT, dport=DHCP_PORT_SERVER)
    pkt /= packet.BOOTP(ciaddr=ip_client, chaddr=__dhcp_mac_to_chaddr(eth_client))
    pkt /= packet.DHCP(
        options=[("message-type", "release"), ("server_id", ip_server), ("end")]
    )
    return pkt


"""
   End DHCP Packet Creation functions

"""


def get_egr_list(parent, ports, how_many, exclude_list=[]):
    """
    Generate a list of ports avoiding those in the exclude list
    @param parent Supplies logging
    @param ports List of OF port numbers
    @param how_many Number of ports to be added to the list
    @param exclude_list List of ports not to be used
    @returns An empty list if unable to find enough ports
    """

    if how_many == 0:
        return []

    count = 0
    egr_ports = []
    for egr_idx in range(len(ports)):
        if ports[egr_idx] not in exclude_list:
            egr_ports.append(ports[egr_idx])
            count += 1
            if count >= how_many:
                return egr_ports
    logging.debug("Could not generate enough egress ports for test")
    return []


def test_params_get(default={}):
    """
    Return all the values passed via test-params if present

    @param default Default dictionary to use if no params were provided or if
    the provided string could not be "exec'd".

    WARNING: TEST PARAMETERS MUST BE PYTHON IDENTIFIERS;
    AND CANNOT START WITH "__";
    eg egr_count, not egr-count.
    """
    if TEST_PARAMS is None:
        return default
    return TEST_PARAMS


def test_param_get(key, default=None):
    """
    Return value passed via test-params if present

    @param key The lookup key
    @param default Default value to use if not found

    WARNING: TEST PARAMETERS MUST BE PYTHON IDENTIFIERS;
    eg egr_count, not egr-count.
    """
    params = test_params_get()

    try:
        return params[key]
    except:
        return default


def port_params_get(default={}):
    """
    Return all the port info values passed via --interface if present

    @param default Default dictionary to use if no params were provided or if
    the provided string could not be "exec'd".

    """
    if PORT_INFO is None:
        return default
    return PORT_INFO


def port_param_get(port, key, default=None):
    """
    Return port info values passed via --interface if present

    @param port The lookup key
    @param key The lookup key
    @param default Default value to use if not found

    """
    params = port_params_get()

    try:
        return params[int(port)][key.lower()]
    except:
        return default


_import_blacklist.add("FILTER")
FILTER = "".join([(len(repr(chr(x))) == 3) and chr(x) or "." for x in range(256)])


def hex_dump_buffer(src, length=16):
    """
    Convert src to a hex dump string and return the string
    @param src The source buffer
    @param length The number of bytes shown in each line
    @returns A string showing the hex dump
    """
    result = ["\n"]
    for i in range(0, len(src), length):
        chars = bytearray(src[i : i + length])
        hex = " ".join(["%02X" % x for x in chars])
        printable = "".join(["%s" % ((x <= 127 and FILTER[x]) or ".") for x in chars])
        result.append("%04x  %-*s  %s\n" % (i, length * 3, hex, printable))
    return "".join(result)


def format_packet(pkt):
    return "Packet length %d \n%s" % (len(bytes(pkt)), hex_dump_buffer(bytes(pkt)))


def inspect_packet(pkt):
    """
    Wrapper around scapy's show() method.
    @returns A string showing the dissected packet.
    """
    out = None
    backup = sys.stdout
    try:
        tmp = StringIO()
        sys.stdout = tmp
        pkt.show2()
        out = tmp.getvalue()
        tmp.close()
    finally:
        sys.stdout = backup
    return out


def nonstandard(cls):
    """
    Testcase decorator that marks the test as being non-standard.
    These tests are not automatically added to the "standard" group.
    """
    cls._nonstandard = True
    return cls


def disabled(cls):
    """
    Testcase decorator that marks the test as being disabled.
    These tests are not automatically added to the "standard" group or
    their module's group.
    """
    cls._disabled = True
    return cls


def group(name):
    """
    Testcase decorator that adds the test to a group.
    """

    def fn(cls):
        if not hasattr(cls, "_groups"):
            cls._groups = []
        cls._groups.append(name)
        return cls

    return fn


def testtimeout(seconds):
    """
    Testcase decorator that adds a timeout to the test.
    """

    def fn(cls):
        cls._testtimeout = seconds
        return cls

    return fn


def ptf_ports(num=None):
    """
    Return a list of 'num' port ids (device_number, port_number)

    If 'num' is None, return all available ports. Otherwise, limit the length
    of the result to 'num' and raise an exception if not enough ports are
    available.
    """
    ports = sorted(ptf.config["port_map"].keys())
    if num != None and len(ports) < num:
        raise Exception(
            "test requires %d ports but only %d are available" % (num, len(ports))
        )
    return ports[:num]


def port_to_tuple(port):
    if type(port) is int:
        return 0, port
    if type(port) is tuple:
        return port
    if type(port) is str:
        try:
            return 0, int(port)
        except:
            pass
    return None


def send_packet(test, port_id, pkt, count=1):
    """
    Send a packet (or a number of packets) out of port_id
    port_id can either be a single integer (port_number on default device 0)
    or a tuple of 2 integers (device_number, port_number)
    """
    device, port = port_to_tuple(port_id)
    pkt = bytes(pkt)
    sent = 0

    for n in range(count):
        test.before_send(pkt, device_number=device, port_number=port)
        sent += test.dataplane.send(device, port, pkt)

    return sent


def send(test, port_id, pkt, count=1):
    """
    See send_packet.
    """
    return send_packet(test, port_id, pkt, count=count)


def dp_poll(test, device_number=0, port_number=None, timeout=None, exp_pkt=None):
    """
    Wrapper function around dataplane.poll
    """
    result = test.dataplane.poll(
        device_number=device_number,
        port_number=port_number,
        timeout=timeout,
        exp_pkt=exp_pkt,
        filters=FILTERS,
    )
    if isinstance(result, test.dataplane.PollSuccess):
        test.at_receive(
            result.packet, device_number=result.device, port_number=result.port
        )
    return result


def verify_packet(test, pkt, port_id, timeout=None):
    """
    Check that an expected packet is received
    port_id can either be a single integer (port_number on default device 0)
    or a tuple of 2 integers (device_number, port_number)
    """
    if not timeout:
        timeout = ptf.ptfutils.default_timeout
    device, port = port_to_tuple(port_id)
    logging.debug("Checking for pkt on device %d, port %d", device, port)
    result = dp_poll(
        test, device_number=device, port_number=port, timeout=timeout, exp_pkt=pkt
    )
    if isinstance(result, test.dataplane.PollFailure):
        test.fail(
            "Expected packet was not received on device %d, port %r.\n%s"
            % (device, port, result.format())
        )


def verify_no_packet(test, pkt, port_id, timeout=None):
    """
    Check that a particular packet is not received
    port_id can either be a single integer (port_number on default device 0)
    or a tuple of 2 integers (device_number, port_number)
    """
    if timeout is None:
        timeout = ptf.ptfutils.default_negative_timeout
    device, port = port_to_tuple(port_id)
    logging.debug("Negative check for pkt on device %d, port %d", device, port)
    result = dp_poll(
        test, device_number=device, port_number=port, exp_pkt=pkt, timeout=timeout
    )
    if isinstance(result, test.dataplane.PollSuccess):
        test.fail(
            "Received packet that we expected not to receive on device %d, "
            "port %r.\n%s" % (device, port, result.format())
        )


def verify_no_other_packets(test, device_number=0, timeout=None):
    """
    Check that no unexpected packets are received on specified device

    This is a no-op if the --relax option is in effect.
    """
    if ptf.config["relax"]:
        return
    if timeout is None:
        timeout = ptf.ptfutils.default_negative_timeout
    logging.debug(
        "Checking for unexpected packets on all ports of device %d" % device_number
    )
    result = dp_poll(test, device_number=device_number, timeout=timeout)
    if isinstance(result, test.dataplane.PollSuccess):
        test.fail(
            "A packet was received on device %d, port %r, but we expected no "
            "packets.\n%s" % (result.device, result.port, result.format())
        )


def verify_packets(test, pkt, ports=[], device_number=0, timeout=None, n_timeout=None):
    """
    a.) Check that a packet is received on each of the specified port numbers for a
    given device (default device number is 0).

    b.) Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device (unless --relax
    is in effect).

    The parameter timeout will be passed as is for each individual verify calls.

    This covers the common and simplest cases for checking dataplane outputs.
    For more complex usage, like multiple different packets being output, or
    multiple packets on the same port, use the primitive verify_packet,
    verify_no_packet, and verify_no_other_packets functions directly.

    Timeout for this function is used as +ve timeout for (a) and n_timeout is used for
    -ve timeout for (b)
    Note: +ve timeout here means timeout in which we are expecting pkt to arrive in
    -ve timeout here means timeout for which we will wait for to check for unexpected pkts
    """
    if not timeout:
        timeout = ptf.ptfutils.default_timeout
    if not n_timeout:
        n_timeout = ptf.ptfutils.default_negative_timeout
    for device, port in ptf_ports():
        if device != device_number:
            continue
        if port in ports:
            verify_packet(test, pkt, (device, port), timeout=timeout)
        else:
            verify_no_packet(test, pkt, (device, port), timeout=n_timeout)
    verify_no_other_packets(test, device_number=device_number, timeout=n_timeout)


def verify_no_packet_any(test, pkt, ports=[], device_number=0, timeout=None):
    """
    Check that a packet is NOT received on _any_ of the specified ports belonging to
    the given device (default device_number is 0).
    """
    test.assertTrue(
        len(ports) != 0,
        "No port available to validate receiving packet on device %d, " % device_number,
    )
    for device, port in ptf_ports():
        if device != device_number:
            continue
        if port in ports:
            print("verifying packet on port device", device_number, "port", port)
            verify_no_packet(test, pkt, (device, port), timeout=timeout)


def verify_packets_any(
    test, pkt, ports=[], device_number=0, timeout=None, n_timeout=None
):
    """
    a.) Check that a packet is received on _any_ of the specified ports belonging to
    the given device (default device_number is 0).

    b.) Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device (unless --relax
    is in effect).
    Timeout for this function is used as +ve timeout for (a) and n_timeout is used for
    -ve timeout for (b)
    Note: +ve timeout here means timeout in which we are expecting pkt to arrive in
    -ve timeout here means timeout for which we will wait for to check for unexpected pkts
    """
    received = False
    failures = []
    if not timeout:
        timeout = ptf.ptfutils.default_timeout
    if not n_timeout:
        n_timeout = ptf.ptfutils.default_negative_timeout
    for device, port in ptf_ports():
        if device != device_number:
            continue
        if port in ports:
            logging.debug("Checking for pkt on device %d, port %d", device_number, port)
            result = dp_poll(
                test,
                device_number=device,
                port_number=port,
                timeout=timeout,
                exp_pkt=pkt,
            )
            if isinstance(result, test.dataplane.PollSuccess):
                received = True
            else:
                failures.append((port, result))
        else:
            verify_no_packet(test, pkt, (device, port), timeout=n_timeout)
    verify_no_other_packets(test, timeout=n_timeout)

    if not received:

        def format_failure(port, failure):
            return "On port %d:\n%s" % (port, failure.format())

        failure_report = "\n".join([format_failure(*f) for f in failures])
        test.fail(
            "Did not receive expected packet on any of ports %r for device %d.\n%s"
            % (ports, device_number, failure_report)
        )


def verify_packet_any_port(
    test, pkt, ports=[], device_number=0, timeout=None, n_timeout=None
):
    """
    a.) Check that the packet is received on _any_ of the specified ports belonging to
    the given device (default device_number is 0).

    The function returns when either the expected packet is received or timeout (1 second).

    b.) Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device (unless --relax
    is in effect).

    Returns the index of the port on which the packet is received and the packet.
    Timeout for this function is used as +ve timeout for (a) and n_timeout is used for
    -ve timeout for (b)
    Note: +ve timeout here means timeout in which we are expecting pkt to arrive in
    -ve timeout here means timeout for which we will wait for to check for unexpected pkts
    """
    if not timeout:
        timeout = ptf.ptfutils.default_timeout
    if not n_timeout:
        n_timeout = ptf.ptfutils.default_negative_timeout
    logging.debug("Checking for pkt on device %d, port %r", device_number, ports)
    result = dp_poll(test, device_number=device_number, timeout=timeout, exp_pkt=pkt)
    verify_no_other_packets(test, device_number=device_number, timeout=n_timeout)

    if isinstance(result, test.dataplane.PollSuccess):
        if result.port in ports:
            return (ports.index(result.port), result.packet)
        else:
            test.fail(
                "Received expected packet on port %r for device %d, but "
                "it should have arrived on one of these ports: %r.\n%s"
                % (result.port, device_number, ports, result.format())
            )
            return (0, None)

    assert isinstance(result, test.dataplane.PollFailure)
    test.fail(
        "Did not receive expected packet on any of ports %r for device %d.\n%s"
        % (ports, device_number, result.format())
    )
    return (0, None)


def verify_any_packet_any_port(
    test, pkts=[], ports=[], device_number=0, timeout=None, n_timeout=None
):
    """
    a.) Check that _any_ of the packet is received on _any_ of the specified ports belonging to
    the given device (default device_number is 0).

    b.) Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device (unless --relax
    is in effect).

    Returns the index of the port on which the packet is received.
    Timeout for this function is used as +ve timeout for (a) and n_timeout is used for
    -ve timeout for (b)
    Note: +ve timeout here means timeout in which we are expecting pkt to arrive in
    -ve timeout here means timeout for which we will wait for to check for unexpected pkts

    The function may verify both: not masked and masked packets
    """
    if not timeout:
        timeout = ptf.ptfutils.default_timeout
    if not n_timeout:
        n_timeout = ptf.ptfutils.default_negative_timeout

    if timeout <= 0 or n_timeout <= 0:
        raise Exception(
            "%s() requires positive timeout value." % sys._getframe().f_code.co_name
        )

    received = False
    match_index = 0
    logging.debug("Checking for pkt on device %d, port %r", device_number, ports)
    result = dp_poll(test, device_number=device_number, timeout=timeout)

    if isinstance(result, test.dataplane.PollSuccess) and result.port in ports:
        for pkt in pkts:
            if ptf.dataplane.match_exp_pkt(pkt, result.packet):
                match_index = ports.index(result.port)
                received = True
    verify_no_other_packets(test, device_number=device_number, timeout=n_timeout)

    if isinstance(result, test.dataplane.PollFailure):
        test.fail(
            "Did not receive any expected packet on any of ports %r for "
            "device %d.\n%s" % (ports, device_number, result.format())
        )

    if result.port not in ports:
        test.fail(
            "One of the expected packets was received on device %d on an "
            "unexpected port: %d\n%s" % (device_number, result.port, result.format())
        )

    if not received:
        test.fail(
            "Did not receive expected packet on any of ports for device %d.\n%s"
            % (device_number, result.format())
        )

    return match_index


def verify_each_packet_on_each_port(
    test, pkts=[], ports=[], device_number=0, timeout=None, n_timeout=None
):
    """
    a.) Check that each packet is received on corresponding port in the port list belonging to
    the given device (default device_number is 0).

    b.) Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device (unless --relax
    is in effect).
    Timeout for this function is used as +ve timeout for (a) and n_timeout is used for
    -ve timeout for (b)
    Note: +ve timeout here means timeout in which we are expecting pkt to arrive in
    -ve timeout here means timeout for which we will wait for to check for unexpected pkts
    """
    pkt_cnt = 0
    test.assertTrue(
        len(pkts) == len(ports), "packet list count does not match port list count"
    )
    if not timeout:
        timeout = ptf.ptfutils.default_timeout
    if not n_timeout:
        n_timeout = ptf.ptfutils.default_negative_timeout
    for port, pkt in zip(ports, pkts):
        logging.debug("Checking for pkt on device %d, port %d", device_number, port)
        result = dp_poll(
            test,
            device_number=device_number,
            port_number=port,
            timeout=timeout,
            exp_pkt=pkt,
        )
        if isinstance(result, test.dataplane.PollFailure):
            test.fail(
                "Did not receive expected packets on port %d for device %d.\n%s"
                % (port, device_number, result.format())
            )

    verify_no_other_packets(test, device_number=device_number, timeout=n_timeout)


def verify_each_packet_on_multiple_port_lists(
    test, pkts=[], ports=[], device_number=0, timeout=None, n_timeout=None
):
    """
    a.) Check that each of the packets from pkts[] list is received once on any port belonging to the given device (default device_number is 0) specified
    in an associated sublist in ports[] list.

    b.) Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device (unless --relax is in effect).

    @param pkts A list of expected packets
    @param ports A list of lists of ports on which subsequent packets are expected
    @returns A list of sets with indexes of ports the packets were received

    Timeout for this function is used as +ve timeout for (a) and n_timeout is used for
    -ve timeout for (b)
    Note: +ve timeout here means timeout in which we are expecting pkt to arrive in
    -ve timeout here means timeout for which we will wait for to check for unexpected pkts
    """
    test.assertTrue(
        len(pkts) == len(ports), "Packet list count does not match port list count"
    )

    if not timeout:
        timeout = ptf.ptfutils.default_timeout
    if not n_timeout:
        n_timeout = ptf.ptfutils.default_negative_timeout

    if timeout <= 0 or n_timeout <= 0:
        raise Exception(
            "%s() requires positive timeout value." % sys._getframe().f_code.co_name
        )

    pkt_cnt = 0
    rcv_idx = []
    for port_list, pkt in zip(ports, pkts):
        rcv_ports = set()
        for port in port_list:
            (rcv_device, rcv_port, rcv_pkt, _) = dp_poll(
                test, device_number=device_number, port_number=port, timeout=timeout
            )
            if rcv_device != device_number:
                continue
            logging.debug("Checking for pkt on device %d, port %d", device_number, port)
            if ptf.dataplane.match_exp_pkt(pkt, rcv_pkt):
                pkt_cnt += 1
                rcv_ports.add(port_list.index(rcv_port))
                break
        rcv_idx.append(rcv_ports)

    verify_no_other_packets(test, device_number=device_number, timeout=n_timeout)

    test.assertTrue(
        pkt_cnt == len(pkts),
        "Did not receive pkt on one of ports %r for device %d" % (ports, device_number),
    )
    return rcv_idx


def verify_packet_prefix(test, pkt, port, len, device_number=0, timeout=None):
    """
    Check that an expected packet is received
    """
    logging.debug("Checking for pkt on port %r", port)
    if timeout is None:
        timeout = ptf.ptfutils.default_timeout
    result = test.dataplane.poll(
        port_number=port, timeout=timeout, exp_pkt=bytes(pkt)[:len]
    )
    if isinstance(result, test.dataplane.PollFailure):
        test.fail(
            "Did not receive expected packet on port %r\n.%s" % (port, result.format())
        )


def count_matched_packets(test, exp_packet, port, device_number=0, timeout=None):
    """
    Receive all packets on the port and count how many expected packets were received.
    As soon as the packets stop arriving, the function waits for the timeout value and
    returns the counter. Therefore, this function requires a positive timeout value.
    """
    if timeout is None:
        timeout = ptf.ptfutils.default_timeout
    if timeout <= 0:
        raise Exception(
            "%s() requires positive timeout value." % sys._getframe().f_code.co_name
        )

    total_rcv_pkt_cnt = 0
    while True:
        result = dp_poll(
            test, device_number=device_number, port_number=port, timeout=timeout
        )
        if isinstance(result, test.dataplane.PollSuccess):
            if ptf.dataplane.match_exp_pkt(exp_packet, result.packet):
                total_rcv_pkt_cnt += 1
        else:
            break

    return total_rcv_pkt_cnt


def count_matched_packets_all_ports(
    test, exp_packet, ports=[], device_number=0, timeout=None
):
    """
    Receive all packets on all specified ports and count how many expected packets were received.
    This function will return the cumulative count of matched packets received once it stops
    receiving matched packets for the specified timeout duration. Therefore, this function
    requires a positive timeout value.
    """
    if timeout is None:
        timeout = ptf.ptfutils.default_timeout
    if timeout <= 0:
        raise Exception(
            "%s() requires positive timeout value." % sys._getframe().f_code.co_name
        )

    last_matched_packet_time = time.time()
    total_rcv_pkt_cnt = 0
    while True:
        if (time.time() - last_matched_packet_time) > timeout:
            break

        result = dp_poll(test, device_number=device_number, timeout=timeout)
        if isinstance(result, test.dataplane.PollSuccess):
            if result.port in ports and ptf.dataplane.match_exp_pkt(
                exp_packet, result.packet
            ):
                total_rcv_pkt_cnt += 1
                last_matched_packet_time = time.time()
        else:
            break

    return total_rcv_pkt_cnt


def simple_rocev2_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    dl_vlan_cfi=0,
    ip_src="192.168.0.1",
    ip_dst="192.168.0.2",
    ip_tos=0,
    ip_ecn=None,
    ip_dscp=None,
    ip_ttl=64,
    udp_sport=1234,
    udp_dport=4791,
    ip_ihl=None,
    ip_options=False,
    ip_flag=0,
    ip_id=1,
    bth_opcode=0x81,
    bth_se=0,
    bth_migration_req=0,
    bth_pad_count=0,
    bth_transport_version=0,
    bth_partition_key=0xFFFF,
    bth_f_res1=0,
    bth_b_res1=0,
    bth_dst_qp=0x0000D2,
    bth_ack_req=0,
    bth_psn=0,
    rocev2_payload=None,
):
    """
    Return a simple dataplane ROCEv2 packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param dl_vlan_cfi The VLAN CFI value
    @param ip_src IP source
    @param ip_dst IP destination
    @param ip_tos IP ToS
    @param ip_ecn IP ToS ECN
    @param ip_dscp IP ToS DSCP
    @param ip_ttl IP TTL
    @param ip_id IP ID
    @param udp_sport UDP source port
    @param udp_dport UDP destination port
    @param bth_opcode BTH OpCode,
    @param bth_se BTH Solicited Event,
    @param bth_migration_req BTH MigReq,
    @param bth_pad_count BTH Pad Count,
    @param bth_transport_version BTH Transport Header Version,
    @param bth_partition_key BTH Partition Key,
    @param bth_f_res1 BTH F/Res1* (F/R),
    @param bth_b_res1 BTH B/Res1* (B/R),
    @param bth_dst_qp BTH Destination QP,
    @param bth_ack_req BTH Acknowledge Request,
    @param bth_psn BTH Packet Sequence Number,
    @param rocev2_payload

    Generates a simple ROCEv2 packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/UDP/ROCEv2 frame.
    """

    if packet.BTH is None:
        logging.error(
            "A ROCEv2 packet was requested but ROCEv2 is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    bth_hdr = packet.BTH(
        opcode=bth_opcode,
        solicited=bth_se,
        migreq=bth_migration_req,
        padcount=bth_pad_count,
        version=bth_transport_version,
        pkey=bth_partition_key,
        fecn=bth_f_res1,
        becn=bth_b_res1,
        dqpn=bth_dst_qp,
        ackreq=bth_ack_req,
        psn=bth_psn,
    )

    udp_hdr = packet.UDP(sport=udp_sport, dport=udp_dport)

    ip_tos = ip_make_tos(ip_tos, ip_ecn, ip_dscp)

    # Note Dot1Q.id is really CFI
    if dl_vlan_enable:
        pkt = (
            packet.Ether(dst=eth_dst, src=eth_src)
            / packet.Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
            / packet.IP(
                src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, ihl=ip_ihl, id=ip_id
            )
            / udp_hdr
            / bth_hdr
        )
    else:
        if not ip_options:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    ihl=ip_ihl,
                    id=ip_id,
                    flags=ip_flag,
                )
                / udp_hdr
                / bth_hdr
            )
        else:
            pkt = (
                packet.Ether(dst=eth_dst, src=eth_src)
                / packet.IP(
                    src=ip_src,
                    dst=ip_dst,
                    tos=ip_tos,
                    ttl=ip_ttl,
                    ihl=ip_ihl,
                    options=ip_options,
                    id=ip_id,
                    flags=ip_flag,
                )
                / udp_hdr
                / bth_hdr
            )

    if rocev2_payload:
        pkt = pkt / rocev2_payload

    pkt = pkt / codecs.decode(
        "".join(["%02x" % (x % 256) for x in range(pktlen - len(pkt))]), "hex"
    )

    return pkt


def simple_rocev2v6_packet(
    pktlen=100,
    eth_dst="00:01:02:03:04:05",
    eth_src="00:06:07:08:09:0a",
    dl_vlan_enable=False,
    vlan_vid=0,
    vlan_pcp=0,
    ipv6_src="2001:db8:85a3::8a2e:370:7334",
    ipv6_dst="2001:db8:85a3::8a2e:370:7335",
    ipv6_tc=0,
    ipv6_ecn=None,
    ipv6_dscp=None,
    ipv6_hlim=64,
    ipv6_fl=0,
    udp_sport=1234,
    udp_dport=4791,
    ip_ihl=None,
    ip_options=False,
    ip_flag=0,
    ip_id=1,
    bth_opcode=0x81,
    bth_se=0,
    bth_migration_req=0,
    bth_pad_count=0,
    bth_transport_version=0,
    bth_partition_key=0xFFFF,
    bth_f_res1=0,
    bth_b_res1=0,
    bth_dst_qp=0x0000D2,
    bth_ack_req=0,
    bth_psn=0,
    rocev2_payload=None,
):
    """
    Return a simple dataplane IPv6/ROCEv2 packet

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param eth_dst Destination MAC
    @param eth_src Source MAC
    @param dl_vlan_enable True if the packet is with vlan, False otherwise
    @param vlan_vid VLAN ID
    @param vlan_pcp VLAN priority
    @param ipv6_src IPv6 source
    @param ipv6_dst IPv6 destination
    @param ipv6_tc IPv6 traffic class
    @param ipv6_ecn IPv6 traffic class ECN
    @param ipv6_dscp IPv6 traffic class DSCP
    @param ipv6_ttl IPv6 hop limit
    @param ipv6_fl IPv6 flow label
    @param udp_sport UDP source port
    @param udp_dport UDP destination port
    @param bth_opcode BTH OpCode,
    @param bth_se BTH Solicited Event,
    @param bth_migration_req BTH MigReq,
    @param bth_pad_count BTH Pad Count,
    @param bth_transport_version BTH Transport Header Version,
    @param bth_partition_key BTH Partition Key,
    @param bth_f_res1 BTH F/Res1* (F/R),
    @param bth_b_res1 BTH B/Res1* (B/R),
    @param bth_dst_qp BTH Destination QP,
    @param bth_ack_req BTH Acknowledge Request,
    @param bth_psn BTH Packet Sequence Number,
    @param rocev2_payload

    Generates a simple IPv6/ROCEv2 packet. Users shouldn't assume anything about
    this packet other than that it is a valid ethernet/IP/UDP/ROCEv2 frame.
    """

    if packet.BTH is None:
        logging.error(
            "A ROCEv2 packet was requested but ROCEv2 is not supported by your Scapy. See README for more information"
        )
        return None

    if MINSIZE > pktlen:
        pktlen = MINSIZE

    bth_hdr = packet.BTH(
        opcode=bth_opcode,
        solicited=bth_se,
        migreq=bth_migration_req,
        padcount=bth_pad_count,
        version=bth_transport_version,
        pkey=bth_partition_key,
        fecn=bth_f_res1,
        becn=bth_b_res1,
        dqpn=bth_dst_qp,
        ackreq=bth_ack_req,
        psn=bth_psn,
    )

    ipv6_tc = ip_make_tos(ipv6_tc, ipv6_ecn, ipv6_dscp)
    pkt = packet.Ether(dst=eth_dst, src=eth_src)
    if dl_vlan_enable or vlan_vid or vlan_pcp:
        pkt /= packet.Dot1Q(vlan=vlan_vid, prio=vlan_pcp)
    pkt /= packet.IPv6(
        src=ipv6_src, dst=ipv6_dst, fl=ipv6_fl, tc=ipv6_tc, hlim=ipv6_hlim
    )
    pkt /= packet.UDP(sport=udp_sport, dport=udp_dport)
    pkt /= bth_hdr

    if rocev2_payload:
        pkt = pkt / rocev2_payload

    pkt /= "D" * (pktlen - len(pkt))

    return pkt


__all__ = list(set(locals()) - _import_blacklist)
