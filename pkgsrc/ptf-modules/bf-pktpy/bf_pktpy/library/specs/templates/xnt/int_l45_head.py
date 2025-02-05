#  Copyright (c) 2022 Intel Corporation
#  SPDX-License-Identifier: Apache-2.0
from bf_pktpy.library.specs.packet import Packet
from bf_pktpy.library.fields import XByteField


class XntIntL45Head(Packet):
    name = "INT_L45_HEAD"
    fields_desc = [
        XByteField("int_type", 0x01),
        XByteField("rsvd0", 0x00),
        XByteField("length", 0x00),
        XByteField("rsvd1", 0x00),
    ]

    def _combine(self, body_copy):
        if body_copy.name == "INT_META":
            self._body = body_copy
            return self

        raise ValueError("Unsupported binding")
