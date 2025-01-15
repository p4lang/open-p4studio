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

"""
Note: This change is temporary. Scapy and bf_pktpy will only be available 
      during the transition period. After this period, the default 
      tool will be bf-pktpy.
"""
pktpy_tool = (os.environ.get("PKTPY", "true")).lower()
if pktpy_tool == "false":
    from .dtel_utils_scapy import *
else:
    from .dtel_utils_pktpy import *

