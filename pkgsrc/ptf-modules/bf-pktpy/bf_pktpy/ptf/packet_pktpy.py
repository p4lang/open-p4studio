#!/usr/bin/env python


# Copyright (c) 2021 Intel Corporation.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

###############################################################################
"""
Bf_pktpy implementation of packet manipulation module. For more information,
see PTF documentation (section "Pluggable packet manipulation module").
"""
import bf_pktpy.packets
import bf_pktpy.commands
from bf_pktpy.all import hexdump as bf_pktpy_hexdump, ls as bf_pktpy_ls
from ptf import config


# Headers set to None are not yet implemented (or conditionally being set)
Packet = bf_pktpy.packets.Packet
Ether = bf_pktpy.packets.Ether
LLC = None
SNAP = None
Dot1Q = bf_pktpy.packets.Dot1Q
GRE = bf_pktpy.packets.GRE
IP = bf_pktpy.packets.IP
IPOption = bf_pktpy.packets.IPOptionPlaceholder
ARP = bf_pktpy.packets.ARP
TCP = bf_pktpy.packets.TCP
UDP = bf_pktpy.packets.UDP
ICMP = bf_pktpy.packets.ICMP
DHCP = bf_pktpy.packets.DHCP
BOOTP = bf_pktpy.packets.BOOTP
PADDING = None
VXLAN = bf_pktpy.packets.VXLAN
BTH = None

IPv6 = None
IPv6ExtHdrRouting = None
ICMPv6Unknown = None
ICMPv6EchoRequest = None
ICMPv6MLReport = None
if not config.get("disable_ipv6", False):
    IPv6 = bf_pktpy.packets.IPv6
    IPv6ExtHdrRouting = bf_pktpy.packets.IPv6ExtHdrRouting
    ICMPv6Unknown = bf_pktpy.packets.ICMPv6Unknown

ERSPAN = None
ERSPAN_III = None
PlatformSpecific = None
if not config.get("disable_erspan", False):
    try:
        ERSPAN = bf_pktpy.packets.ERSPAN
        ERSPAN_III = bf_pktpy.packets.ERSPAN_III
        PlatformSpecific = bf_pktpy.packets.ERSPAN_PlatformSpecific
    except ImportError as e:
        print("ERSPAN support not found in bf_pktpy. Details:\n%s" % e)

GENEVE = None

MPLS = None
if not config.get("disable_mpls", False):
    MPLS = bf_pktpy.packets.MPLS

NVGRE = None

IGMP = None
if not config.get("disable_igmp", False):
    try:
        IGMP = bf_pktpy.packets.IGMP
    except ImportError as e:
        print("IGMP support not found in bf_pktpy. Details:\n%s" % e)


##############################################################################

# Headers implemented, but not in Scapy version of packet module
SimpleL3SwitchCpuHeader = bf_pktpy.packets.SimpleL3SwitchCpuHeader
BFD = bf_pktpy.packets.BFD
TCPOption = bf_pktpy.packets.TCPOptionPlaceholder
MirrorPreDeparser = bf_pktpy.packets.MirrorPreDeparser
GTPU = bf_pktpy.packets.GTPU

XntIntMeta = bf_pktpy.packets.XntIntMeta
XntIntL45Head = bf_pktpy.packets.XntIntL45Head
XntIntL45Tail = bf_pktpy.packets.XntIntL45Tail


def get_erspan_alternative():
    """
    Return ERSPAN alternative implementation
    Example usage:
    ERSPAN, ERSPAN_III, PlatformSpecific = get_erspan_alternative()
    :return:
    """
    if config.get("disable_erspan", False):
        return (None,) * 3

    from bf_pktpy.library.specs.templates.erspan.alternative.erspan import (
        ERSPAN as alt_ERSPAN,
    )
    from bf_pktpy.library.specs.templates.erspan.alternative.erspan_iii import (
        ERSPAN_III as alt_ERSPAN_III,
    )
    from bf_pktpy.library.specs.templates.erspan.alternative.platform_specific import (
        PlatformSpecific as alt_PlatformSpecific,
    )

    return alt_ERSPAN, alt_ERSPAN_III, alt_PlatformSpecific


# bf_pktpy implementation of hexdump
hexdump = bf_pktpy_hexdump
ls = bf_pktpy_ls

# The names below are assigned here so that, like the other names
# above, they can be used by importers of the ptf.packet module as if
# they were defined inside of ptf.packet, and they are commonly
# available as ptf.packet.<name> regardless whether you use scapy or
# bf-pktpy as the packet manipulation module.

send = bf_pktpy.commands.send
sendp = bf_pktpy.commands.sendp
sr = bf_pktpy.commands.sr
sr1 = bf_pktpy.commands.sr1
srp = bf_pktpy.commands.srp
srp1 = bf_pktpy.commands.srp1
srloop = bf_pktpy.commands.srloop
srploop = bf_pktpy.commands.srploop
sniff = bf_pktpy.commands.sniff
bridge_and_sniff = bf_pktpy.commands.bridge_and_sniff
