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
Helper functions for PTF testcases
"""

import bf_switcht_api_thrift

import time
from time import sleep
from os import getenv
import sys
import logging

import unittest
import random

import ptf.dataplane as dataplane
import api_base_tests
import pd_base_tests
import model_utils as u

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os
import ptf.mask
import six

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *
from bf_switcht_api_thrift.api_adapter import ApiAdapter

device = 0
pkt_len = int(test_param_get('pkt_size'))
swports = []
'''
config["interfaces"] is [ <device>, <dev_port> ]
For model
tofino  - (0,0), (0,1), (0,2) etc
tofino2 - (0,8), (0,9), (0,10) etc
So for tofino2, we will use (dev_port - 8) as port_id
For asic
just use what is given in the config
'''
for device, port, ifname in config["interfaces"]:
    test_params = ptf.testutils.test_params_get()
    arch = test_params['arch']
    if arch == 'tofino2':
        if test_params['target'] == 'hw':
            swports.append(port)
        else:
            if port >= 8:
                swports.append(port - 8)
    else:
        swports.append(port)
swports.sort()

if swports == []:
    swports = [x for x in range(65)]


port_speed_map = {
    "10g": 10000,
    "25g": 25000,
    "40g": 40000,
    "40g_r2": 40000,
    "50g": 50000,
    "50g_r1": 50000,
    "100g": 100000,
    "100g_r2": 100000,
    "200g": 200000,
    "200g_r8": 200000,
    "400g": 400000,
    "default": 25000
}

class ApiHelper(ApiAdapter):
    dev_id = device
    pipe1_internal_ports = range(128, 144)
    pipe3_internal_ports = range(384, 400)

    def setUp(self):
        super(ApiAdapter, self).setUp()

    def tearDown(self):
        test_params = ptf.testutils.test_params_get()
        if 'reboot' in list(test_params.keys()):
            if test_params['reboot'] == 'hitless':
                if hasattr(self, 'port_list'):
                    for port_oid in self.port_list:
                        self.client.object_delete(port_oid)
        self.port_list = []
        super(ApiAdapter, self).tearDown()

    def _push(self, keyword, status, object_id, *args, **kwargs):
        if not hasattr(self, 'stack'):
            self.stack = []
        if (status == 0):
            self.stack.append((keyword, status, object_id, args, kwargs))
        else:
            self.stack.append((keyword, status, 0, args, kwargs))

    def _pop(self, keyword, *args, **kwargs):
        match_this = (keyword, args, kwargs)
        for idx, pack in enumerate(self.stack):
            if pack == match_this:
                del self.stack[idx]
                break

    def _clean(self, pack):
        keyword, status, object_id, args, kwargs = pack
        try:
            func = None
            if keyword == 'object_create':
                func = self.client.object_delete

            if func:
                if (status == 0):
                    func(object_id, *args, **kwargs)
            else:
                print('Clean up method not found')
        except:
            msg = ('Calling %r with params\n  object_id : %s\n  args  : %s\n  kwargs: %s'
                   '') % (func.__name__, object_id, args, kwargs)
            raise ValueError(msg)

    def cleanlast(self, count = 1):
        if hasattr(self, 'stack'):
            for i in range(count):
                if len(self.stack) > 0:
                    self._clean(self.stack.pop())
                else:
                    break

    def clean_to_object(self, obj):
        if hasattr(self, 'stack'):
            while len(self.stack) > 0:
                pack = self.stack.pop()
                self._clean(pack)
                keyword, status, object_id, args, kwargs = pack
                if object_id == obj:
                    break

    # hitless tests run together clears out thrift state
    # Remove ports for each test when running hitless tests
    def cleanup(self):
        if hasattr(self, 'stack'):
            while len(self.stack) > 0:
                self._clean(self.stack.pop())
        test_params = ptf.testutils.test_params_get()
        if 'reboot' in list(test_params.keys()):
          if test_params['reboot'] == 'hitless' or test_params['reboot'] == 'fastreconfig':
                if hasattr(self, 'port_list'):
                    for port_oid in self.port_list:
                        self.client.object_delete(port_oid)
        self.port_list = []
        super(ApiAdapter, self).tearDown()

    def status(self):
        if hasattr(self, 'stack'):
            keyword, ret, object_id, args, kwargs = self.stack[len(self.stack) - 1]
            return ret

    def attribute_set(self, object, attribute, value):
        attr = u.attr_make(attribute, value)
        return self.client.attribute_set(object, attr)

    def object_counters_get(self, object):
        if self.test_params['target'] == 'hw':
            print("HW: Sleeping for 3 sec before fetching stats")
            time.sleep(3)
        return self.client.object_counters_get(object)

    def object_get_all_handles(self, type):
        if self.test_params['target'] == 'hw':
            print("HW: Sleeping for 3 sec before fetching stats")
            time.sleep(3)
        return self.client.object_get_all_handles(type)

    def attr_get(self, val):
        t = val.type
        if t == switcht_value_type.BOOL:
            return val.BOOL
        elif t == switcht_value_type.UINT8:
            return val.UINT8
        elif t == switcht_value_type.UINT16:
            return val.UINT16
        elif t == switcht_value_type.UINT32:
            return val.UINT32
        elif t == switcht_value_type.UINT64:
            return val.UINT64
        elif t == switcht_value_type.INT64:
            return val.INT64
        elif t == switcht_value_type.ENUM:
            return val.ENUM
        elif t == switcht_value_type.MAC:
            return val.MAC
        elif t == switcht_value_type.STRING:
            return val.STRING
        elif t == switcht_value_type.OBJECT_ID:
            return val.OBJECT_ID
        elif t == switcht_value_type.LIST:
            return val.LIST
        elif t == switcht_value_type.RANGE:
            return val.RANGE

    def attribute_get(self, object, attribute):
        ret = self.client.attribute_get(object, attribute)
        return self.attr_get(ret.attr.value)

    def get_device_handle(self, dev_id):
        attrs = list()
        value = switcht_value_t(type=switcht_value_type.UINT16, UINT16=dev_id)
        attr = switcht_attribute_t(id=SWITCH_DEVICE_ATTR_DEV_ID, value=value)
        attrs.append(attr)
        ret = self.client.object_get(SWITCH_OBJECT_TYPE_DEVICE, attrs)
        return ret.object_id

    def get_port_ifindex(self, port_handle, port_type=0):
        return (port_type << 9 | (port_handle & 0x1FF))

    def wait_for_interface_up(self, port_list):
        self.test_params = ptf.testutils.test_params_get()
        link_up_timeout = test_params["link_up_timeout"] if "link_up_timeout" in test_params.keys() else 20
        if self.test_params['target'] == 'hw':
            print("Waiting for ports UP...")
            for num_of_tries in range(link_up_timeout):
                time.sleep(1)
                all_ports_are_up = True
                # wait till the port are up
                for port in port_list:
                    state = self.attribute_get(port, SWITCH_PORT_ATTR_OPER_STATE)
                    if state != SWITCH_PORT_ATTR_OPER_STATE_UP:
                        all_ports_are_up = False
                if all_ports_are_up:
                    break
            if not all_ports_are_up:
                raise RuntimeError('Not all of the ports are up')


    def configure(self):
        # To avoid double-config in case ApiHelper.setUp() has been executed first
        if not hasattr(self, 'client'):
            super(ApiAdapter, self).setUp()

        # L2 configuration
        self.port_list = []
        self.devports = []
        self.device = self.get_device_handle(device)
        self.cpu_port_hdl = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_CPU_PORT)
        self.cpu_port = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_DEV_PORT)
        self.default_vrf = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_DEFAULT_VRF)
        self.vlan10 = self.add_vlan(self.device, vlan_id=10)
        self.vlan20 = self.add_vlan(self.device, vlan_id=20)
        self.vlan30 = self.add_vlan(self.device, vlan_id=30)
        self.vlan40 = self.add_vlan(self.device, vlan_id=40)
        port_speed = port_speed_map["default"]
        if "port_speed" in test_params_get().keys():
            if test_param_get("port_speed") in port_speed_map.keys():
                port_speed = port_speed_map[test_param_get("port_speed")]

        if self.client.thrift_ports_present():
            self.port_list = self.client.port_list_get(self.device)
        else:
            for swport in swports[:16]:
                #l_list = []
                #l_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=swport))
                #lane_list_t
                #port_oid = self.add_port(self.device, lane_list=l_list, speed=port_speed)
                port_oid = self.add_port(self.device, lane_list=u.lane_list_t([swport]), speed=port_speed)
                self.port_list.append(port_oid)
                self.stack.pop()
            self.client.add_thrift_ports()

        for x, port_oid in zip(list(range(0, len(self.port_list))), self.port_list):
            dev_port = self.attribute_get(port_oid, SWITCH_PORT_ATTR_DEV_PORT)
            self.devports.append(dev_port)
            setattr(self, 'port%s' % x, port_oid)

        if "skip_ports_check" in test_params_get().keys() and test_param_get("skip_ports_check"):
            print("\nPort status check skipped")
        else:
            self.wait_for_interface_up(self.port_list)

        #L3 configuration
        self.default_rmac = "00:BA:7E:F0:00:00"  # from bf_switch_device_add
        self.rmac = '00:77:66:55:44:33'
        self.vrf10 = self.add_vrf(self.device, id=10, src_mac=self.rmac)

        self.test_params = ptf.testutils.test_params_get()
        self.arch = self.test_params['arch']
        # on model, we poll devports since veths are mapped to the devports
        if self.test_params['target'] == 'hw':
            self.devports = []
            self.devports = swports

    def CheckObjectAttributes(self, object, attr_dict):
        for attr_id, exp_attr_value in six.iteritems(attr_dict):
            thrift_attr_value = self.attribute_get(object, attr_id)
            if exp_attr_value[1] == thrift_attr_value:
                print("Attr {} is correct: {}".format(exp_attr_value[0], thrift_attr_value))
            else:
                print("Attr {} is incorrect, got {}, expected {}"\
                       .format(exp_attr_value[0], thrift_attr_value, exp_attr_value[1]))
                return 0
        return 1


    def runL2Test(self):

        # send packet from port0 to port2
        self.vlan10_pkt1 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:44:44:44:44:42',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        # send packet from port0 to lag1
        self.vlan10_lag_pkt1 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:44:44:44:44:45',
            eth_src='00:44:44:44:44:42',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        # send packet from port2 to lag1
        self.vlan10_lag_pkt2 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:44:44:44:44:46',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        # send packet from lag1 to port2
        self.vlan10_lag_pkt3 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:55:55:55:55:55',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        # send packet from port5 to port4
        self.vlan20_pkt1 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:66:66:66:66:66',
            eth_src='00:77:77:77:77:77',
            ip_dst='20.20.20.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        # send packet from port4 to port5
        self.vlan20_pkt2 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:77:77:77:77:77',
            eth_src='00:66:66:66:66:66',
            ip_dst='20.20.20.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        # send and verify
        print("Sending vlan10 L2 packet from port 0 -> port 2")
        send_packet(self, self.devports[0], self.vlan10_pkt1)
        verify_packet(self, self.vlan10_pkt1, self.devports[2])

        print("Sending vlan10 L2 packet from port 0 -> lag1")
        send_packet(self, self.devports[0], self.vlan10_lag_pkt1)
        verify_any_packet_any_port(self, [self.vlan10_lag_pkt1, self.vlan10_lag_pkt1], [self.devports[6], self.devports[7]])

        print("Sending vlan10 L2 packet from port 2 -> lag1")
        send_packet(self, self.devports[2], self.vlan10_lag_pkt2)
        verify_any_packet_any_port(self, [self.vlan10_lag_pkt2, self.vlan10_lag_pkt2],
                    [self.devports[6], self.devports[7]])

        print("Sending IPv4 packet lag 1 -> port 2")
        send_packet(self, self.devports[6], self.vlan10_lag_pkt3)
        verify_packet(self, self.vlan10_lag_pkt3, self.devports[2])

        send_packet(self, self.devports[7], self.vlan10_lag_pkt3)
        verify_packet(self, self.vlan10_lag_pkt3, self.devports[2])

        print("Sending vlan20 L2 packet from port 5 -> port 4")
        send_packet(self, self.devports[5], self.vlan20_pkt1)
        verify_packet(self, self.vlan20_pkt1, self.devports[4])

        print("Sending vlan20 L2 packet from port 4 -> port 5")
        send_packet(self, self.devports[4], self.vlan20_pkt2)
        verify_packet(self, self.vlan20_pkt2, self.devports[5])
        return


    def CleanupL2(self):
        """
         cleanup: ingress L2 property for
            vlan_members:
                   vlan10 - port0,port1,port2,lag1
                   vlan20 - port3, port4,port5
        """
        print("reset port vlan setting")
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        return

    def ConfigureL2(self):
        """
         configure:
            lag1: port6,port7
            L2 ports: port0,port1,port2,port3,port4,port5,lag1
            vlan_members:
                   vlan10 - port0,port1,port2,lag1
                   vlan20 - port3, port4,port5
        """
        print("configure basic l2")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)

        #configure vlan10 vlan_members
        vlan_member0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_member1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        self.lag1 = self.add_lag(self.device)
        lag_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port6)
        lag_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port7)
        vlan_member3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag1)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        # configure vlan20 vlan_members
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port3)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port4)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port5)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)

        #vlan10 fdb entries.
        self.vlan10_mac0 = '00:44:44:44:44:44'
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address= self.vlan10_mac0,
                                 destination_handle=self.port2)

        self.vlan10_mac1 = '00:44:44:44:44:45'
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address=self.vlan10_mac1,
                                 destination_handle=self.lag1)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                        mac_address='00:44:44:44:44:46',
                        destination_handle=self.lag1)

        #vlan20 fdb entries
        self.vlan20_mac0 = '00:66:66:66:66:66'
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address=self.vlan20_mac0,
                                  destination_handle=self.port4)

        self.vlan20_mac1 = '00:77:77:77:77:77'
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address=self.vlan20_mac1,
                         destination_handle=self.port5)

        try:
            print("run L2 base test")
            self.runL2Test()
        except AssertionError:
            self.CleanupL2()
            self.cleanup()
            raise
        finally:
            pass
        return

    def runL3Test(self):
        # send the test packet(s)
        self.l3_pkt1 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.l3_exp_pkt1 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.l3_pkt2 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.l3_exp_pkt2 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:55:55:55:55:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.l3_lag_pkt1 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.l3_exp_lag_pkt1 = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:11:22:33:44:57',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.l3_pkt1_v6 = simple_tcpv6_packet(
            pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        self.l3_exp_pkt1_v6 = simple_tcpv6_packet(
            pktlen=pkt_len,
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        # send packet from port2 to lag1
        self.svi_vlan10_lag_pkt = simple_tcp_packet(
            pktlen=pkt_len,
            eth_dst='00:55:55:55:55:55',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        print("Sending IPv4 packet port 8 -> port 9")
        send_packet(self, self.devports[8], self.l3_pkt1)
        verify_packet(self, self.l3_exp_pkt1, self.devports[9])

        print("Sending IPv4 packet vlan 10 -> port 9")
        send_packet(self, self.devports[1], self.l3_pkt1)
        verify_packet(self, self.l3_exp_pkt1, self.devports[9])

        send_packet(self, self.devports[2], self.l3_pkt1)
        verify_packet(self, self.l3_exp_pkt1, self.devports[9])

        send_packet(self, self.devports[6], self.l3_pkt1)
        verify_packet(self, self.l3_exp_pkt1, self.devports[9])

        send_packet(self, self.devports[7], self.l3_pkt1)
        verify_packet(self, self.l3_exp_pkt1, self.devports[9])

        print("Sending IPv4 packet port 8 -> lag 2")
        send_packet(self, self.devports[8], self.l3_lag_pkt1)
        verify_any_packet_any_port(self, [self.l3_exp_lag_pkt1, self.l3_exp_lag_pkt1],
                    [self.devports[11], self.devports[12]])

        print("Sending IPv4 packet port 8 -> lag 1")
        send_packet(self, self.devports[8], self.l3_pkt2)
        verify_any_packet_any_port(self, [self.l3_exp_pkt2, self.l3_exp_pkt2],
                    [self.devports[6], self.devports[7]])

        print("Sending IPv4 packet lag 2 -> port 9")
        send_packet(self, self.devports[11], self.l3_pkt1)
        verify_packet(self, self.l3_exp_pkt1, self.devports[9])

        send_packet(self, self.devports[12], self.l3_pkt1)
        verify_packet(self, self.l3_exp_pkt1, self.devports[9])
        print("Sending IPv6 packet port 0 -> port 9")

        send_packet(self, self.devports[8], self.l3_pkt1_v6)
        verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])

        print("Sending vlan10  packet from port 2 -> lag1")
        send_packet(self, self.devports[2], self.svi_vlan10_lag_pkt)
        verify_any_packet_any_port(self, [self.svi_vlan10_lag_pkt, self.svi_vlan10_lag_pkt],
                    [self.devports[6], self.devports[7]])
        return

    def ConfigureL3(self):
        """
          configure:
               lag2: port11,port12
               l3 ports: port8, port9, port10, lag2 ,SVI_VLAN10
        """
        print("configure basic l3")

        # Create L3 interfaces on physical ports - port8,port9 and port10
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port8,
                           vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.0.1')
        self.neighbor0 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif0, dest_ip='10.10.0.1')

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port9, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port10, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.3')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif2, dest_ip='10.10.0.3')

        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10,
                            nexthop_handle=self.nhop1)
        self.route1 = self.add_route(self.device, ip_prefix='10.10.10.2', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)

        self.route1 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10,
                                     nexthop_handle=self.nhop1)
        self.route2 = self.add_route(self.device, ip_prefix='10.10.10.3', vrf_handle=self.vrf10, nexthop_handle=self.nhop2)

        # Create L3 interfaces on LAG - lag2
        self.lag2 = self.add_lag(self.device)
        lag_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port11)
        lag_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port12)
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag2,
                              vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop3 = self.add_nexthop(self.device, handle=self.rif3, dest_ip='10.10.0.4')
        self.neighbor3 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=self.rif3, dest_ip='10.10.0.4')
        self.route3 = self.add_route(self.device, ip_prefix='10.10.10.4', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)


        # Create SVI on vlan 10
        self.vlan10_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN,
                         vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                        mac_address='00:55:55:55:55:55',
                        destination_handle=self.lag1)
        self.nhop4 = self.add_nexthop(self.device, handle=self.vlan10_rif, dest_ip='10.10.10.5')
        self.neighbor4 = self.add_neighbor(self.device, mac_address='00:55:55:55:55:55', handle=self.vlan10_rif, dest_ip='10.10.10.5')
        self.route4 = self.add_route(self.device, ip_prefix='10.10.10.5', vrf_handle=self.vrf10, nexthop_handle=self.nhop4)

        try:
            print("run L3 base test")
            self.runL3Test()
        except AssertionError:
            self.cleanup()
            raise
        finally:
            pass
        return

    def startFastReconfig(self):
        time.sleep(2)
        print(" -- Start FastReconfig -- ")
        self.fast_reconfig_begin(self.dev_id)
        self.fast_reconfig_end(self.dev_id)
        print(" -- Finish FastReconfig -- ")
        print(" -- FastReconfig sequence complete -- ")

    def startWarmReboot(self):
        time.sleep(2)
        print(" -- Start WarmReboot --")
        self.warm_init_begin(self.dev_id)
        self.warm_init_end(self.dev_id)
        print(" -- Finish WarmReboot --")
        print(" -- WarmReboot sequence complete -- ")

    # Run test case with the below param option
    # --wr will run WarmReboot sequence
    # --fr will run FastReconfig sequence
    # Function param:
    #   mode - if set to 0 [or if no param options]
    #     then neither sequence will run
    #   cb - if appropriate, invoke cb function after FR/WR replay
    # Function returns true if FR/WR seq was run, else False
    def startFrWrReplay(self, mode, cb=None):
        if (mode == 0):
            return False

        reboot_type = None
        test_params = ptf.testutils.test_params_get()
        if 'reboot' in list(test_params.keys()):
            reboot_type = test_params['reboot']

        if (reboot_type == 'hitless'):
            self.startWarmReboot()
            if cb:
                cb()
            return True
        elif (reboot_type == 'fastreconfig'):
            self.startFastReconfig()
            if cb:
                cb()
            return True

        return False
