#  Copyright (c) 2022 Intel Corporation
#  SPDX-License-Identifier: Apache-2.0
from bf_pktpy.library.specs.packet import Packet
from bf_pktpy.library.fields import (
    BitField,
    ShortField,
)


class XntIntMeta(Packet):
    name = "INT_META"
    fields_desc = [
        BitField("ver", 0, 4),
        BitField("rep", 0, 2),
        BitField("c", 0, 1),
        BitField("e", 0, 1),
        BitField("rsvd1", 0, 3),
        BitField("ins_cnt", 0, 5),
        BitField("max_hop_cnt", 32, 8),
        BitField("total_hop_cnt", 0, 8),
        ShortField("inst_mask", 0),
        ShortField("rsvd2", 0x0000),
    ]
