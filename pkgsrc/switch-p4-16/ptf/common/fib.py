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

import re
from ipaddress import ip_address, ip_network

import six

from .lpm import LpmDict

# These subnets are excluded from FIB test
# reference: RFC 5735 Special Use IPv4 Addresses
#            RFC 5156 Special Use IPv6 Addresses

EXCLUDE_IPV4_PREFIXES = [
        '0.0.0.0/8',            # "This" Network        RFC 1122, Section 3.2.1.3
        '10.0.0.0/8',           # Private-Use Networks  RFC 1918
        '127.0.0.0/8',          # Loopback              RFC 1122, Section 3.2.1.3
        '169.254.0.0/16',       # Link Local            RFC RFC 3927
        '224.0.0.0/4',          # Multicast             RFC 3171
        '255.255.255.255/32'    # Limited Broadcast     RFC 919, Section 7
                                #                       RFC 922, Section 7
]

EXCLUDE_IPV6_PREFIXES = [
        '::/128',               # Unspecified           RFC 4291
        '::1/128',              # Loopback              RFC 4291
        'fe80::/10',            # Link local            RFC 4291
        'ff00::/8'              # Multicast             RFC 4291
        ]

class Fib():
    class NextHop():
        def __init__(self, next_hop = ''):
            self._next_hop = []
            matches = re.findall('\[([\s\d]+)\]', next_hop)
            for match in matches:
                self._next_hop.append([int(s) for s in match.split()])

        def __str__(self):
            return str(self._next_hop)

        def get_next_hop(self):
            return self._next_hop

        def get_next_hop_list(self):
            port_list = [p for intf in self._next_hop for p in intf]
            return port_list

    # Initialize FIB with FIB file
    def __init__(self, file_path):
        self._ipv4_lpm_dict = LpmDict()
        for ip in EXCLUDE_IPV4_PREFIXES:
            self._ipv4_lpm_dict[ip] = self.NextHop()

        self._ipv6_lpm_dict = LpmDict(ipv4=False)
        for ip in EXCLUDE_IPV6_PREFIXES:
            self._ipv6_lpm_dict[ip] = self.NextHop()

        # filter out empty lines and lines starting with '#'
        pattern = re.compile("^#.*$|^[ \t]*$")

        with open(file_path, 'r') as f:
            for line in f.readlines():
                if pattern.match(line): continue
                entry = line.split(' ', 1)
                prefix = ip_network(six.ensure_text(entry[0]))
                next_hop = self.NextHop(entry[1])
                if prefix.version == 4:
                    self._ipv4_lpm_dict[str(prefix)] = next_hop
                elif prefix.version == 6:
                    self._ipv6_lpm_dict[str(prefix)] = next_hop

    def __getitem__(self, ip):
        ip = ip_address(six.ensure_text(ip))
        if ip.version == 4:
            return self._ipv4_lpm_dict[str(ip)]
        elif ip.version == 6:
            return self._ipv6_lpm_dict[str(ip)]

    def __contains__(self, ip):
        ip_obj = ip_address(six.ensure_text(ip))
        if ip_obj.version == 4:
            return self._ipv4_lpm_dict.contains(ip)
        elif ip_obj.version == 6:
            return self._ipv6_lpm_dict.contains(ip)

    def ipv4_ranges(self):
        return self._ipv4_lpm_dict.ranges()

    def ipv6_ranges(self):
        return self._ipv6_lpm_dict.ranges()
