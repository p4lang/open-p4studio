#  Copyright (c) 2022 Intel Corporation
#  SPDX-License-Identifier: Apache-2.0
from bf_pktpy.library.specs.packet import Packet
from bf_pktpy.library.fields import (
    BitField,
    ByteField,
    XBitField,
    ByteEnumField,
    ShortField,
    IntField,
    ConditionalField,
)


GTPMessageType = {
    1: "Echo Request",
    2: "Echo Response",
    16: "Create PDP Context Request",
    17: "Create PDP Context Response",
    18: "Update PDP Context Request",
    19: "Update PDP Context Response",
    20: "Delete PDP Context Request",
    21: "Delete PDP Context Response",
    26: "Error Indication",
    27: "PDU Notification Request",
    28: "PDU Notification Response",
    31: "Supported Extension Headers Notification",
    254: "End Marker",
    255: "G-PDU",
}

ExtensionHeadersTypes = {
    0: "No more extension headers",
    1: "Reserved",
    2: "Reserved",
    64: "UDP Port",
    133: "PDU Session Container",
    192: "PDCP PDU Number",
    193: "Reserved",
    194: "Reserved",
}


def gtpu_flag_condition(packet):
    return packet.e == 1 or packet.s == 1 or packet.pn == 1


class GTPU(Packet):

    name = "GTPU"
    fields_desc = [
        BitField("pn", 0, 1),
        BitField("s", 0, 1),
        BitField("e", 0, 1),
        BitField("reserved", 0, 1),
        BitField("pt", 0, 1),
        BitField("version", 0, 3),
        ByteEnumField("gtp_type", 0, GTPMessageType),
        ShortField("length", 0),
        IntField("teid", 0),
        ConditionalField(XBitField("seq", 0, 16), gtpu_flag_condition),
        ConditionalField(ByteField("npdu", 0), gtpu_flag_condition),
        ConditionalField(
            ByteEnumField("next_ex", 0, ExtensionHeadersTypes), gtpu_flag_condition
        ),
    ]

    @property
    def message(self):
        return GTPMessageType.get(self.gtp_type)

    @property
    def next_extension_header(self):
        return ExtensionHeadersTypes.get(self.next_ex)

    def _combine(self, body_copy):
        if body_copy.name in ("IP", "IPv6"):
            self._body = body_copy
            return self

        raise ValueError("Unsupported binding")
