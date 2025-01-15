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

import os
from ptf.testutils import *
from ptf.packet import *
from ptf.dataplane import DataPlane
import ptf.mask

try:
    from pal_rpc.ttypes import *
except ImportError:
    pass

import logging

"""
Note: This change is temporary. Scapy and bf_pktpy will only be available 
      during the transition period. After this period, the default 
      tool will be bf-pktpy.
"""
pktpy_tool = (os.environ.get("PKTPY", "true")).lower()

if pktpy_tool == "false":
    print('Using Scapy..')
    from .utils_scapy import *
else:
    print('Using PKTPY..')
    from .utils_pktpy import *


# Util functions independent of bf_pktpy/Scapy
def generate_mac_addresses(no_of_addr):
    """
        Generate list of different mac addresses

        Args:
            no_of_addr (int): number of requested MAC addresses (max 256^4)

        Return:
            list: mac_list with generated MAC addresses
    """
    mac_list = []
    i = 0
    for first_grp in range(0, 256):
        for second_grp in range(0, 256):
            for third_grp in range(0, 256):
                for fourth_grp in range(0, 256):
                    mac_list.append('00:00:' +
                                    ('%02x' % first_grp) + ':' +
                                    ('%02x' % second_grp) + ':' +
                                    ('%02x' % third_grp) + ':' +
                                    ('%02x' % fourth_grp))
                    i += 1
                    if i == no_of_addr:
                        return mac_list
    return mac_list
