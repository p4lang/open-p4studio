#  Copyright (c) 2022 Intel Corporation
#  SPDX-License-Identifier: Apache-2.0
from bf_pktpy.library.specs.packet import Packet
from bf_pktpy.library.fields import StrField


class Raw(Packet):

    name = "Raw"
    fields_desc = [StrField("load", b"")]

    def str(self):
        return self.load
