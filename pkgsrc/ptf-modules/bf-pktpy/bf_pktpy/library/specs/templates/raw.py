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
from bf_pktpy.library.fields import StrField


class Raw(Packet):

    name = "Raw"
    fields_desc = [StrField("load", b"")]

    def str(self):
        return self.load
