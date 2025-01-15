#  INTEL CONFIDENTIAL
#
#  Copyright (c) 2022 Intel Corporation
#  All Rights Reserved.
#
#  This software and the related documents are Intel copyrighted materials,
#  and your use of them is governed by the express license under which they
#  were provided to you ("License"). Unless the License provides otherwise,
#  you may not use, modify, copy, publish, distribute, disclose or transmit this
#  software or the related documents without Intel's prior written permission.
#
#  This software and the related documents are provided as is, with no express or
#  implied warranties, other than those that are expressly stated in the License.
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
