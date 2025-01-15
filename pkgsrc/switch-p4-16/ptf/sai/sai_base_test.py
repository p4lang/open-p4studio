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

"""
Base classes for test cases

Tests will usually inherit from one of these classes to have the controller
and/or dataplane automatically set up.
"""

import os
import logging
import unittest


import ptf
from ptf.base_tests import BaseTest
from ptf import config
import ptf.dataplane as dataplane
import ptf.testutils as testutils

################################################################
#
# Thrift interface base tests
#
################################################################

import switchsai_thrift.switch_sai_rpc as switch_sai_rpc
import bf_switcht_api_thrift.bf_switcht_api_rpc as switch_api_rpc
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

interface_to_front_mapping = {}
port_map_loaded = 0


class ThriftInterface(BaseTest):

    def loadPortMap(self):
        global port_map_loaded
        if port_map_loaded:
            print 'port_map already loaded'
            return

        if "port_map" in self.test_params:
            user_input = self.test_params['port_map']
            splitted_map = user_input.split(",")
            for item in splitted_map:
                interface_front_pair = item.split("@")
                interface_to_front_mapping[interface_front_pair[0]
                                           ] = interface_front_pair[1]
        elif "port_map_file" in self.test_params:
            user_input = self.test_params['port_map_file']
            f = open(user_input, 'r')
            for line in f:
                if (len(line) > 0 and (
                        line[0] == '#' or line[0] == ';' or line[0] == '/')):
                    continue
                interface_front_pair = line.split("@")
                interface_to_front_mapping[interface_front_pair[0]
                                           ] = interface_front_pair[1].strip()
            print interface_to_front_mapping
        else:
            exit("No ptf interface<-> switch front port mapping, please specify as parameter or in external file")
        return

    def createRpcClient(self):
        # Set up thrift client and contact server

        if "thrift_server" in self.test_params:
            server = self.test_params['thrift_server']
        else:
            server = 'localhost'

        self.transport = TSocket.TSocket(server, 9093)
        self.transport = TTransport.TBufferedTransport(self.transport)
        self.protocol = TBinaryProtocol.TBinaryProtocol(self.transport)

        self.client = switch_sai_rpc.Client(self.protocol)
        self.transport.open()
        return

    def createSwitchApiRpcClient(self):
        # Set up thrift client and contact server

        if "thrift_server" in self.test_params:
            server = self.test_params['thrift_server']
        else:
            server = 'localhost'

        self.swtransport = TSocket.TSocket(server, 9091)
        self.swtransport = TTransport.TBufferedTransport(self.swtransport)
        self.swprotocol = TBinaryProtocol.TBinaryProtocol(self.swtransport)

        self.swclient = switch_api_rpc.Client(self.swprotocol)
        self.swtransport.open()
        return

    def closeSwitchApiRpcClient(self):
        self.swtransport.close()

    def setUp(self):
        global interface_to_front_mapping
        BaseTest.setUp(self)
        self.test_params = testutils.test_params_get()
        self.loadPortMap()
        self.createRpcClient()
        return

    def tearDown(self):
        if config["log_dir"] is not None:
            self.dataplane.stop_pcap()
        BaseTest.tearDown(self)
        self.transport.close()


class ThriftInterfaceDataPlane(ThriftInterface):
    """
    Root class that sets up the thrift interface and dataplane
    """

    def setUp(self):
        ThriftInterface.setUp(self)
        self.dataplane = ptf.dataplane_instance
        if self.dataplane is not None:
            self.dataplane.flush()
            if config["log_dir"] is not None:
                filename = os.path.join(config["log_dir"], str(self)) + ".pcap"
                self.dataplane.start_pcap(filename)

    def tearDown(self):
        if config["log_dir"] is not None:
            self.dataplane.stop_pcap()
        ThriftInterface.tearDown(self)
