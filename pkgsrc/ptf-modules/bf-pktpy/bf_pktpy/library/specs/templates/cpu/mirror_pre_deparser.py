#  Copyright (c) 2022 Intel Corporation
#  SPDX-License-Identifier: Apache-2.0
from bf_pktpy.library.specs.packet import Packet
from bf_pktpy.library.fields import ByteField, ShortField


class MirrorPreDeparser(Packet):
    name = "MirrorIntMdHeader"
    fields_desc = [
        ByteField("pkt_type", 0x01),
        ByteField("do_egr_mirroring", 0x01),
        ShortField("sid", 0x00),
    ]

    def _combine(self, body_copy):
        if body_copy.name == "Ether":
            self._body = body_copy
            return self

        raise ValueError("Unsupported binding")
