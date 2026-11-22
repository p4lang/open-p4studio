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

import importlib
import os
import logging
import unittest
import sys

import ptf
from ptf.base_tests import BaseTest
from ptf import config
import ptf.testutils as testutils

################################################################
#
# Thrift interface base tests
#
################################################################

from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.protocol import TMultiplexedProtocol

class ThriftInterface(BaseTest):
    def __init__(self, p4_names, p4_prefixes = []):
        BaseTest.__init__(self)
        assert( (type(p4_names) is list) and (len(p4_names) > 0) )
        if p4_prefixes:
            assert( (type(p4_prefixes) is list) and (len(p4_prefixes) == len(p4_names)) )
        else:
            p4_prefixes = p4_names
        self.p4_names = p4_names
        self.p4_prefixes = p4_prefixes
        self.p4_client_modules = {}
        for p4_name, p4_prefix in zip(p4_names, p4_prefixes):
            try:
                if p4_name == "":
                    self.p4_client_modules[p4_name] = importlib.import_module(".".join(["p4_pd_rpc", p4_prefix]))
                else:
                    self.p4_client_modules[p4_name] = importlib.import_module(".".join([p4_name, "p4_pd_rpc", p4_prefix]))
            except:
                    self.p4_client_modules[p4_name] = None
        self.mc_client_module = importlib.import_module(".".join(["mc_pd_rpc", "mc"]))
        try:
            self.mirror_client_module = importlib.import_module(".".join(["mirror_pd_rpc", "mirror"]))
        except:
            self.mirror_client_module = None
        try:
            self.sd_client_module = importlib.import_module(".".join(["sd_pd_rpc", "sd"]))
        except:
            self.sd_client_module = None
        try:
            self.plcmt_client_module = importlib.import_module(".".join(["plcmt_pd_rpc", "plcmt"]))
        except:
            self.plcmt_client_module = None
        try:
            self.devport_mgr_client_module = importlib.import_module(".".join(["devport_mgr_pd_rpc", "devport_mgr"]))
        except:
            self.devport_mgr_client_module = None
        try:
            self.port_mgr_client_module = importlib.import_module(".".join(["port_mgr_pd_rpc", "port_mgr"]))
        except:
            self.port_mgr_client_module = None
        try:
            self.pal_client_module = importlib.import_module(".".join(["pal_rpc", "pal"]))
        except:
            self.pal_client_module = None
        self.conn_mgr_client_module = importlib.import_module(".".join(["conn_mgr_pd_rpc", "conn_mgr"]))
        try:
            self.pkt_client_module = importlib.import_module(".".join(["pkt_pd_rpc", "pkt"]))
        except:
            self.pkt_client_module = None
        try:
            self.tm_client_module = importlib.import_module(".".join(["tm_api_rpc", "tm"]))
        except:
            self.tm_client_module = None

        try:
            self.pltfm_pm_client_module = importlib.import_module(".".join(["pltfm_pm_rpc", "pltfm_pm_rpc"]))
        except:
            self.pltfm_pm_client_module = None
        try:
            self.pltfm_mgr_client_module = importlib.import_module(".".join(["pltfm_mgr_rpc", "pltfm_mgr_rpc"]))
        except:
            self.pltfm_mgr_client_module = None
        try:
            self.knet_mgr_client_module = importlib.import_module(".".join(["knet_mgr_pd_rpc", "knet_mgr"]))
        except:
            self.knet_mgr_client_module = None


    def setUp(self):
        BaseTest.setUp(self)

        # Set up thrift client and contact server
        thrift_server = 'localhost'
        if testutils.test_param_get('thrift_server') != "":
            thrift_server = testutils.test_param_get('thrift_server')
        self.transport = TSocket.TSocket(thrift_server, 9090)

        self.transport = TTransport.TBufferedTransport(self.transport)
        bprotocol = TBinaryProtocol.TBinaryProtocol(self.transport)

        self.mc_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "mc")
        if self.mirror_client_module:
            self.mirror_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "mirror")
        if self.sd_client_module:
            self.sd_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "sd")
        if self.plcmt_client_module:
            self.plcmt_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "plcmt")
        if self.devport_mgr_client_module:
            self.devport_mgr_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "devport_mgr")
        else:
            self.devport_mgr_protocol = None
        if self.port_mgr_client_module:
            self.port_mgr_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "port_mgr")
        else:
            self.port_mgr_protocol = None
        if self.pal_client_module:
            self.pal_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "pal")
        else:
            self.pal_protocol = None
        self.conn_mgr_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "conn_mgr")
        if self.pkt_client_module:
            self.pkt_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "pkt")
        else:
            self.pkt_protocol = None
        if self.tm_client_module:
            self.tm_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "tm")
        else:
            self.tm_protocol = None

        if self.pltfm_pm_client_module:
            self.pltfm_pm_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "pltfm_pm_rpc")
        else:
            self.pltfm_pm_protocol = None
        if self.pltfm_mgr_client_module:
            self.pltfm_mgr_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "pltfm_mgr_rpc")
        else:
            self.pltfm_mgr_protocol = None
        if self.knet_mgr_client_module:
            self.knet_mgr_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "knet_mgr")
        else:
            self.knet_mgr_protocol = None

        self.p4_protocols = {}
        self.clients = {}
        self.client = None
        for p4_name, p4_prefix in zip(self.p4_names, self.p4_prefixes):
            p4_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, p4_prefix)
            self.p4_protocols[p4_name] = p4_protocol
            if self.p4_client_modules[p4_name] is not None:
                self.clients[p4_name] = self.p4_client_modules[p4_name].Client(p4_protocol)

        if len(self.clients) == 1:
            if sys.version_info[0]==2:
                self.client = self.clients.values()[0]
            if sys.version_info[0]==3:
                self.client = list(self.clients.values())[0]

        self.mc = self.mc_client_module.Client(self.mc_protocol)
        if self.mirror_client_module:
            self.mirror = self.mirror_client_module.Client(self.mirror_protocol)
        else:
            self.mirror = None
        if self.sd_client_module:
            self.sd = self.sd_client_module.Client(self.sd_protocol)
        else:
            self.sd = None
        if self.plcmt_client_module:
            self.plcmt = self.plcmt_client_module.Client(self.plcmt_protocol)
        else:
            self.plcmt = None
        if self.devport_mgr_client_module:
            self.devport_mgr = self.devport_mgr_client_module.Client(self.devport_mgr_protocol)
        else:
            self.devport_mgr = None
        if self.port_mgr_client_module:
            self.port_mgr = self.port_mgr_client_module.Client(self.port_mgr_protocol)
        else:
            self.port_mgr = None
        if self.pal_client_module:
            self.pal = self.pal_client_module.Client(self.pal_protocol)
        else:
            self.pal = None
        self.conn_mgr = self.conn_mgr_client_module.Client(self.conn_mgr_protocol)
        if self.pkt_client_module:
            self.pkt = self.pkt_client_module.Client(self.pkt_protocol)
        else:
            self.pkt = None
        if self.tm_client_module:
            self.tm = self.tm_client_module.Client(self.tm_protocol)
        else:
            self.tm = None

        if self.pltfm_pm_client_module:
            self.pltfm_pm = self.pltfm_pm_client_module.Client(self.pltfm_pm_protocol)
        else:
            self.pltfm_pm = None
        if self.pltfm_mgr_client_module:
            self.pltfm_mgr = self.pltfm_mgr_client_module.Client(self.pltfm_mgr_protocol)
        else:
            self.pltfm_mgr = None
        if self.knet_mgr_client_module:
            self.knet_mgr = self.knet_mgr_client_module.Client(self.knet_mgr_protocol)
        else:
            self.knet_mgr = None

        self.transport.open()

        return self.client

    def tearDown(self):
        if config["log_dir"] != None:
            self.dataplane.stop_pcap()
        BaseTest.tearDown(self)
        self.transport.close()


class ThriftInterfaceDataPlane(ThriftInterface):
    """
    Root class that sets up the thrift interface and dataplane
    """
    def __init__(self, p4_names, p4_prefixes = []):
        ThriftInterface.__init__(self, p4_names, p4_prefixes = p4_prefixes)

    def setUp(self):
        self.client = ThriftInterface.setUp(self)
        self.dataplane = ptf.dataplane_instance
        self.dataplane.flush()
        if config["log_dir"] != None:
            filename = os.path.join(config["log_dir"], str(self)) + ".pcap"
            self.dataplane.start_pcap(filename)
        return self.client

    def tearDown(self):
        if config["log_dir"] != None:
            self.dataplane.stop_pcap()
        ThriftInterface.tearDown(self)


class EmptyTableTest(ThriftInterfaceDataPlane):
    """Check the generated PD service and its match table on a loaded pipeline."""

    def __init__(self):
        super().__init__([self.program])

    def runTest(self):
        from res_pd_rpc.ttypes import DevTarget_t

        session = self.conn_mgr.client_init()
        try:
            get_count = getattr(self.client, self.table + "_get_entry_count")
            self.assertEqual(get_count(session, DevTarget_t(0, -1)), 0, self.table)
        finally:
            self.conn_mgr.client_cleanup(session)
