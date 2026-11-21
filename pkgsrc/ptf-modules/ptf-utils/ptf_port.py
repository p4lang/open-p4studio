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


import sys
import os
import json
import six
import subprocess
from ptf import config

cur_dir = os.path.dirname(os.path.realpath(__file__))

def take_port_down(port_num):
    device = 0
    pm = config['port_map']
    veth = pm[(device, port_num)]
    veth_idx = veth.strip('veth')
    veth_num = int(veth_idx)
    veth_pair = "veth%d" % (veth_num - 1)
    six.print_(port_num, veth, veth_pair)
    subprocess.check_call(['port_ifdown', veth_pair])

def bring_port_up(port_num):
    device = 0
    pm = config['port_map']
    veth = pm[(device, port_num)]
    veth_idx = veth.strip('veth')
    veth_num = int(veth_idx)
    veth_pair = "veth%d" % (veth_num - 1)
    subprocess.check_call(['port_ifup', veth_pair])
