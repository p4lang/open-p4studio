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
import time
import unittest

import ptf
from ptf.base_tests import BaseTest
from ptf import config
import ptf.dataplane as dataplane
from ptf.testutils import *

import model_utils as u

################################################################
#
# Thrift interface base tests
#
################################################################

import bf_switcht_api_thrift.bf_switcht_api_rpc as bf_switcht_api_rpc

from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.protocol import TMultiplexedProtocol
from devport_mgr_pd_rpc.ttypes import *

class ThriftInterface(BaseTest):
    def setUp(self):
        BaseTest.setUp(self)

        # Set up thrift client and contact server
        self.thrift_server = 'localhost'
        if test_param_get('thrift_server') is not None:
            self.thrift_server = test_param_get('thrift_server')
        self.transport = TSocket.TSocket(self.thrift_server, 9091)
        self.transport = TTransport.TBufferedTransport(self.transport)
        self.protocol = TBinaryProtocol.TBinaryProtocol(self.transport)
        self.switchtprotocol = TMultiplexedProtocol.TMultiplexedProtocol(
                    self.protocol, "bf_switcht_api_rpc")
        self.client = bf_switcht_api_rpc.Client(self.switchtprotocol)
        self.transport.open()

        self.pd_transport = TSocket.TSocket(self.thrift_server, 9090)
        self.pd_transport = TTransport.TBufferedTransport(self.pd_transport)
        self.pd_protocol = TBinaryProtocol.TBinaryProtocol(self.pd_transport)
        self.pd_protocol = TMultiplexedProtocol.TMultiplexedProtocol(
            self.pd_protocol, "devport_mgr")
        self.devport_mgr_client_module = importlib.import_module(
            ".".join(["devport_mgr_pd_rpc", "devport_mgr"]))
        self.devport_mgr = self.devport_mgr_client_module.Client(self.pd_protocol)

    def tearDown(self):
        if config["log_dir"] != None:
            self.dataplane.stop_pcap()
        BaseTest.tearDown(self)
        self.transport.close()

    def fast_reconfig_begin(self, device):
        self.pd_transport.open()
        self.transport.close()
        self.devport_mgr.devport_mgr_warm_init_begin(
            device, dev_init_mode.DEV_WARM_INIT_FAST_RECFG,
            dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, False)
        time.sleep(2)
        self.transport.open()

    def fast_reconfig_end(self, device):
        self.devport_mgr.devport_mgr_warm_init_end(device)
        self.pd_transport.close()

    def warm_init_begin(self, device):
        self.pd_transport.open()
        self.transport.close()
        self.devport_mgr.devport_mgr_warm_init_begin(
            device, dev_init_mode.DEV_WARM_INIT_HITLESS,
            dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, False)
        time.sleep(2)
        self.transport.open()

    def warm_init_end(self, device):
        self.devport_mgr.devport_mgr_warm_init_end(device)
        self.pd_transport.close()

def object_name_get(ot):
    object = u.objects[ot]
    return object["object_name"]

class ThriftInterfaceDataPlane(ThriftInterface):
    """
    Root class that sets up the thrift interface and dataplane
    """

    def setUp(self):
        ThriftInterface.setUp(self)
        self.dataplane = ptf.dataplane_instance
        self.dataplane.flush()
        if config["log_dir"] != None:
            filename = os.path.join(config["log_dir"], str(self)) + ".pcap"
            self.dataplane.start_pcap(filename)

    def tearDown(self):
        if config["log_dir"] != None:
            self.dataplane.stop_pcap()
        ThriftInterface.tearDown(self)

    handles = []
    def object_create(self, ot, attrs):
        ret = self.client.object_create(ot, attrs)
        self.assertTrue(ret.status == 0)
        handle = ret.object_id
        handle_hex = "0x{:02x}".format(handle)
        ot = handle >> 48

        print("created object handle {}, ot={}\n".format(handle_hex, object_name_get(ot)))
        self.handles.insert(0, handle)
        return ret

    def object_delete(self, object_handle):
        ret =self.client.object_delete(object_handle)
        self.assertTrue(ret.status == 0)
        self.handles.remove(object_handle)
        return ret

    def attribute_set(self, object_handle, attr):
        ret = self.client.attribute_set(object_handle, attr)
        self.assertTrue(ret.status == 0)
        return ret

    def delete_all_handles(self):
        handles = self.handles[:]
        for handle in handles:
            handle_hex = "0x{:02x}".format(handle)
            ot = handle >> 48
            print("deleting object handle {}, ot={}\n".format(handle_hex, object_name_get(ot)))
            self.object_delete(handle)
        self.assertTrue(len(self.handles) == 0)

