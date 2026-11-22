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

import logging
import random
import time

import ptf
from ptf import config
import ptf.testutils as testutils
from ptf.testutils import *
from p4testutils.misc_utils import *
from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as client
from ptf_port import *

logger = get_logger()
g_is_tofino3 = testutils.test_param_get("arch") == "tofino3"

class PortInfo:
    def __init__(self, port):
        self.devPort = int(port)
        self.speed = str(testutils.port_param_get(port, 'speed'))
        self.fec = str(testutils.port_param_get(port, 'fec'))
        auto_neg = testutils.port_param_get(port, 'auto_neg')
        if auto_neg is not None:
            self.autoNeg = str(auto_neg)
        conn_id = testutils.port_param_get(port, 'conn_id')
        if conn_id is not None:
            self.conn_id = int(conn_id)
        else:
            logger.info("Error: port %s has no conn_id defined. Use 'bf-sde>pm show' to get the mapping" % port)
            assert (0)
        chnl_id = testutils.port_param_get(port, 'chnl_id')
        if chnl_id is not None:
            self.chnl_id = int(chnl_id)
        else:
            logger.info("Error: port %s has no chnl_id defined. Use 'bf-sde>pm show' to get the mapping" % port)
            assert (0)
        self.front_port = None
        front_port = testutils.port_param_get(port, 'front_port')
        if front_port is not None:
            self.front_port = int(front_port)

    def getDevPort(self):
        return self.devPort

    def getSpeed(self):
        return {
            "10G": "BF_SPEED_10G",
            "25G": "BF_SPEED_25G",
            "40G": "BF_SPEED_40G",
            "40G_NB": "BF_SPEED_40G_NB",
            "50G": "BF_SPEED_50G",
            "100G": "BF_SPEED_100G",

            "200G": "BF_SPEED_200G",
            "400G": "BF_SPEED_400G",
            "40G_R2": "BF_SPEED_40G_R2"
        }.get(self.speed.upper(), "BF_SPEED_NONE")

    def getFec(self):
        return {
            "none": "BF_FEC_TYP_NONE",
            "rs": "BF_FEC_TYP_REED_SOLOMON",
            "fc": "BF_FEC_TYP_FIRECODE",
        }.get(self.fec, "BF_FEC_TYP_NONE")

    def getAutoNeg(self):
        return {
            "default": "PM_AN_DEFAULT",
            "enable": "PM_AN_FORCE_ENABLE",
            "disable": "PM_AN_FORCE_DISABLE"
        }.get(self.autoNeg, "PM_AN_DEFAULT")

    def getConnId(self):
        return self.conn_id

    def getChnlId(self):
        return self.chnl_id

    def getName(self):
        return "%d/%d" % (self.conn_id, self.chnl_id)

    def getFrontPort(self):
        return self.front_port

ports = []
for item in config["interfaces"]:
    port = str(item[1])
    ports.append(PortInfo(port))

def ValueCheck(field, resp, expect_value):
    data_dict = next(resp)[0].to_dict()
    value = data_dict[field]
    if (value != expect_value):
        logger.info("Error: data %d, expect %d", value, expect_value)
        assert (0)

def speed_to_num_lanes(speed):
    if speed in ["BF_SPEED_10G", "BF_SPEED_25G", "BF_SPEED_50G_R1"]:
        return 1
    elif speed in ["BF_SPEED_50G", "BF_SPEED_100G_R2"]:
        return 2
    elif speed in ["BF_SPEED_100G", "BF_SPEED_200G", "BF_SPEED_200G_R4"]:
        return 4
    elif speed in ["BF_SPEED_400G", "BF_SPEED_400G_R8"]:
        return 8


def PortStatusCbTest(self, port):
    logger.info("Test Port Up/Down Status Change Callback function")
    setup_random()
    target = client.Target(device_id=0, pipe_id=0xffff)
    logger.info("Add a port")
    self.port_table.entry_add(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$SPEED', str_val=port.getSpeed()),
                                    client.DataTuple('$FEC', str_val=port.getFec()),
                                    client.DataTuple('$PORT_ENABLE', bool_val=True)])])
    logger.info("Wait for Port Up")
    time.sleep(5)
    logger.info("Enable port status change notification")
    self.port_table.attribute_port_status_change_set(target, enable=True)
    logger.info("Take port down")
    self.port_table.entry_mod(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$PORT_ENABLE', bool_val=False)])])
    logger.info("Wait for Port Down")
    time.sleep(5)
    logger.info("Get port status change notification")
    port_status_chg = self.interface.portstatus_notification_get()
    assert (port_status_chg.port_up == False)
    logger.info("Disable port status change notification")
    self.port_table.attribute_port_status_change_set(target, enable=False)
    logger.info("Delete the port")
    self.port_table.entry_del(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port.getDevPort())])])


def PortHdlInfoTest(self, port):
    setup_random()
    target = client.Target(device_id=0, pipe_id=0xffff)
    logger.info("add port")
    self.port_table.entry_add(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$SPEED', str_val=port.getSpeed()),
                                    client.DataTuple('$FEC', str_val=port.getFec()),
                                    client.DataTuple('$PORT_ENABLE', bool_val=True)])])
    logger.info("dev_port get")
    resp = self.port_hdl_info_table.entry_get(
        target,
        [self.port_hdl_info_table.make_key([client.KeyTuple('$CONN_ID', port.getConnId()),
                                            client.KeyTuple('$CHNL_ID', port.getChnlId())])],
        {"from_hw": False})
    ValueCheck('$DEV_PORT', resp, port.getDevPort())
    if (port.getFrontPort() is not None):
        resp = self.port_fp_idx_info_table.entry_get(
            target,
            [self.port_fp_idx_info_table.make_key([client.KeyTuple('$FP_IDX', port.getDevPort())])],
            {"from_hw": False})
        ValueCheck('$DEV_PORT', resp, port.getFrontPort())
    resp = self.port_str_info_table.entry_get(
        target,
        [self.port_str_info_table.make_key([client.KeyTuple('$PORT_NAME', port.getName())])],
        {"from_hw": False})
    ValueCheck('$DEV_PORT', resp, port.getDevPort())
    logger.info("delete port")
    self.port_table.entry_del(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port.getDevPort())])])


def send_and_verify_packet(self, ingress_port, egress_port, pkt, exp_pkt):
    logger.info("Sending packet on port %d", ingress_port)
    testutils.send_packet(self, ingress_port, pkt)
    logger.info("Expecting packet on port %d", egress_port)
    testutils.verify_packets(self, exp_pkt, [egress_port])


def StatCheck(data_dict, field, expect_value):
    value = data_dict[field]
    if (value != expect_value):
        logger.info("Error: data %d, expect %d", value, expect_value)
        assert (0)


def PortStatTest(self, port1, port2):
    logger.info("Test Port Stats read and clear, mixed with p4 table entry operations")
    setup_random()
    target = client.Target(device_id=0, pipe_id=0xffff)
    ig_port = port1.getDevPort()
    eg_port = port2.getDevPort()
    dmac = '22:22:22:22:22:22'
    dmask = 'ff:ff:ff:ff:ff:f0'
    pkt = testutils.simple_tcp_packet(eth_dst=dmac)
    exp_pkt = pkt

    logger.info("Add forward table entry")
    self.forward_table.entry_add(
        target,
        [self.forward_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac)])],
        [self.forward_table.make_data([client.DataTuple('port', eg_port)],
                                      'SwitchIngress.hit')]
    )
    logger.info("Add two ports")
    self.port_table.entry_add(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port1.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$SPEED', str_val=port1.getSpeed()),
                                    client.DataTuple('$FEC', str_val=port1.getFec()),
                                    client.DataTuple('$PORT_ENABLE', bool_val=True)])])
    self.port_table.entry_add(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port2.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$SPEED', str_val=port2.getSpeed()),
                                    client.DataTuple('$FEC', str_val=port2.getFec()),
                                    client.DataTuple('$PORT_ENABLE', bool_val=True)])])
    logger.info("Wait for port up")
    time.sleep(5)
    send_and_verify_packet(self, ig_port, eg_port, pkt, exp_pkt)
    logger.info("Set poll interval")
    self.port_stat_table.attribute_port_stat_poll_intvl_set(target, intvl=int(600))
    logger.info("port stats read from hw")
    get_data_list = None
    resp = self.port_stat_table.entry_get(
        target,
        [self.port_stat_table.make_key([client.KeyTuple('$DEV_PORT', ig_port)])],
        {"from_hw": True},
        get_data_list)
    logger.info("Check pkt stats")
    data_dict = next(resp)[0].to_dict()
    StatCheck(data_dict, '$FramesReceivedOK', 1)
    StatCheck(data_dict, '$FramesReceivedAll', 1)
    StatCheck(data_dict, '$FramesReceivedwithFCSError', 0)
    StatCheck(data_dict, '$FramesTransmittedOK', 0)
    StatCheck(data_dict, '$FramesTransmittedAll', 0)
    StatCheck(data_dict, '$FramesTransmittedwithError', 0)

    resp = self.port_stat_table.entry_get(
        target,
        [self.port_stat_table.make_key([client.KeyTuple('$DEV_PORT', eg_port)])],
        {"from_hw": True},
        get_data_list)
    logger.info("Check pkt stats")
    data_dict = next(resp)[0].to_dict()
    StatCheck(data_dict, '$FramesReceivedOK', 0)
    StatCheck(data_dict, '$FramesReceivedAll', 0)
    StatCheck(data_dict, '$FramesReceivedwithFCSError', 0)
    StatCheck(data_dict, '$FramesTransmittedOK', 1)
    StatCheck(data_dict, '$FramesTransmittedAll', 1)
    StatCheck(data_dict, '$FramesTransmittedwithError', 0)

    logger.info("Sleep for a second waiting for stats got sync to sw")
    time.sleep(1)
    logger.info("port stats read from sw")
    resp = self.port_stat_table.entry_get(
        target,
        [self.port_stat_table.make_key([client.KeyTuple('$DEV_PORT', ig_port)])],
        {"from_hw": False},
        get_data_list)
    logger.info("Check pkt stats")
    data_dict = next(resp)[0].to_dict()
    StatCheck(data_dict, '$FramesReceivedOK', 1)
    StatCheck(data_dict, '$FramesReceivedAll', 1)
    StatCheck(data_dict, '$FramesReceivedwithFCSError', 0)
    StatCheck(data_dict, '$FramesTransmittedOK', 0)
    StatCheck(data_dict, '$FramesTransmittedAll', 0)
    StatCheck(data_dict, '$FramesTransmittedwithError', 0)
    resp = self.port_stat_table.entry_get(
        target,
        [self.port_stat_table.make_key([client.KeyTuple('$DEV_PORT', eg_port)])],
        {"from_hw": False},
        get_data_list)
    logger.info("Check pkt stats")
    data_dict = next(resp)[0].to_dict()
    StatCheck(data_dict, '$FramesReceivedOK', 0)
    StatCheck(data_dict, '$FramesReceivedAll', 0)
    StatCheck(data_dict, '$FramesReceivedwithFCSError', 0)
    StatCheck(data_dict, '$FramesTransmittedOK', 1)
    StatCheck(data_dict, '$FramesTransmittedAll', 1)
    StatCheck(data_dict, '$FramesTransmittedwithError', 0)
    logger.info("clear stats")
    self.port_stat_table.entry_mod(
        target,
        [self.port_stat_table.make_key([client.KeyTuple('$DEV_PORT', ig_port)])],
        [self.port_stat_table.make_data([client.DataTuple('$FramesReceivedOK', 0),
                                         client.DataTuple('$FramesReceivedAll', 0),
                                         client.DataTuple('$FramesReceivedwithFCSError', 0),
                                         client.DataTuple('$FramesTransmittedOK', 0),
                                         client.DataTuple('$FramesTransmittedAll', 0),
                                         client.DataTuple('$FramesTransmittedwithError', 0)])])

    logger.info("port stats read from hw again")
    resp = self.port_stat_table.entry_get(
        target,
        [self.port_stat_table.make_key([client.KeyTuple('$DEV_PORT', ig_port)])],
        {"from_hw": False},
        get_data_list)
    logger.info("Check pkt stats")
    data_dict = next(resp)[0].to_dict()
    StatCheck(data_dict, '$FramesReceivedOK', 0)
    StatCheck(data_dict, '$FramesReceivedAll', 0)
    StatCheck(data_dict, '$FramesReceivedwithFCSError', 0)
    StatCheck(data_dict, '$FramesTransmittedOK', 0)
    StatCheck(data_dict, '$FramesTransmittedAll', 0)
    StatCheck(data_dict, '$FramesTransmittedwithError', 0)

    # Instead of clearing the stats of eg_port, just clear the entire table
    # which should in turn clear the stats for eg_port as well
    self.port_stat_table.entry_del(target, key_list=None)

    resp = self.port_stat_table.entry_get(
        target,
        [self.port_stat_table.make_key([client.KeyTuple('$DEV_PORT', eg_port)])],
        {"from_hw": False},
        get_data_list)
    logger.info("Check pkt stats")

    data_dict = next(resp)[0].to_dict()
    StatCheck(data_dict, '$FramesReceivedOK', 0)
    StatCheck(data_dict, '$FramesReceivedAll', 0)
    StatCheck(data_dict, '$FramesReceivedwithFCSError', 0)
    StatCheck(data_dict, '$FramesTransmittedOK', 0)
    StatCheck(data_dict, '$FramesTransmittedAll', 0)
    StatCheck(data_dict, '$FramesTransmittedwithError', 0)

    logger.info("Delete the entry from the forward table")
    self.forward_table.entry_del(
        target,
        [self.forward_table.make_key([client.KeyTuple('hdr.ethernet.dst_addr', dmac)])])

    logger.info("delete ports")
    self.port_table.entry_del(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', ig_port)])])
    self.port_table.entry_del(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', eg_port)])])


def PortCfgTest(self, port1, port2):
    logger.info("Test Port cfg table add read and delete operations")
    target = client.Target(device_id=0, pipe_id=0xffff)

    logger.info("PortCfgTest: Adding entry for port %d", port1.getDevPort())
    self.port_table.entry_add(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port1.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$SPEED', str_val=port1.getSpeed()),
                                    client.DataTuple('$FEC', str_val=port1.getFec())])])

    logger.info("PortCfgTest: Adding entry for port %d", port2.getDevPort())
    n_lanes = speed_to_num_lanes(port2.getSpeed())

    self.port_table.entry_add(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port2.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$SPEED', str_val=port2.getSpeed()),
                                    client.DataTuple('$FEC', str_val=port2.getFec()),
                                    client.DataTuple('$N_LANES', n_lanes)])])

    logger.info("PortCfgTest: Modifying entry for port %d", port1.getDevPort())
    self.port_table.entry_mod(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port1.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$PORT_ENABLE', bool_val=True),
         client.DataTuple('$AUTO_NEGOTIATION', str_val="PM_AN_FORCE_ENABLE"),
         client.DataTuple('$TX_MTU', 1500),
         client.DataTuple('$RX_MTU', 1500),
         client.DataTuple('$TX_PFC_EN_MAP', 1),
         client.DataTuple('$RX_PFC_EN_MAP', 1),
         client.DataTuple('$RX_PRSR_PRI_THRESH', 1),
         client.DataTuple('$TX_PAUSE_FRAME_EN', bool_val=False),
         client.DataTuple('$RX_PAUSE_FRAME_EN', bool_val=False),
         client.DataTuple('$CUT_THROUGH_EN', bool_val=False),
         client.DataTuple('$PORT_DIR', str_val="PM_PORT_DIR_DEFAULT")])])

    logger.info("PortCfgTest: Modifying entry for port %d", port2.getDevPort())
    if g_is_tofino3:
        loop_str_val="BF_LPBK_PCS_NEAR"
    else:
        loop_str_val="BF_LPBK_MAC_NEAR"

    self.port_table.entry_mod(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port2.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$PORT_ENABLE', bool_val=True),
                                    client.DataTuple('$RX_PRSR_PRI_THRESH', 2),
                                    client.DataTuple('$LOOPBACK_MODE', str_val=loop_str_val)])])

    logger.info("PortCfgTest: Reading entry for port %d", port1.getDevPort())
    resp = self.port_table.entry_get(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port1.getDevPort())])])

    logger.info("PortCfgTest: Validating entry read for port %d", port1.getDevPort())
    for data, key in resp:
        data = data.to_dict()
        key = key.to_dict()
        assert(key['$DEV_PORT']['value'] == port1.getDevPort())
        assert(data['$SPEED'] == port1.getSpeed())
        assert(data['$FEC'] == port1.getFec())
        assert(data['$PORT_ENABLE'] == True)
        assert(data['$AUTO_NEGOTIATION'] == 'PM_AN_FORCE_ENABLE')
        assert(data['$TX_MTU'] == 1500)
        assert(data['$RX_MTU'] == 1500)
        assert(data['$TX_PFC_EN_MAP'] == 1)
        assert(data['$RX_PFC_EN_MAP'] == 1)
        assert(data['$TX_PAUSE_FRAME_EN'] == False)
        assert(data['$RX_PAUSE_FRAME_EN'] == False)
        assert(data['$CUT_THROUGH_EN'] == False)
        assert(data['$RX_PRSR_PRI_THRESH'] == 1)
        assert(data['$PORT_DIR'] == 'PM_PORT_DIR_DEFAULT')

    logger.info("PortCfgTest: Reading entry for port %d", port2.getDevPort())
    resp = self.port_table.entry_get(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port2.getDevPort())])])

    logger.info("PortCfgTest: Validating entry read for port %d", port2.getDevPort())
    for data, key in resp:
        data = data.to_dict()
        key = key.to_dict()
        assert(key['$DEV_PORT']['value'] == port2.getDevPort())
        assert(data['$SPEED'] == port2.getSpeed())
        assert(data['$FEC'] == port2.getFec())
        assert(data['$N_LANES'] == n_lanes)
        assert(data['$PORT_ENABLE'] == True)
        assert(data['$RX_PRSR_PRI_THRESH'] == 2)
        assert(data['$LOOPBACK_MODE'] == loop_str_val)

    logger.info("PortCfgTest: Wild card read")
    resp = self.port_table.entry_get(target, None)

    logger.info("PortCfgTest: Validating wild card read")
    for data, key in resp:
        data = data.to_dict()
        key = key.to_dict()
        port = key['$DEV_PORT']['value']
        if port == port1.getDevPort():
            assert(key['$DEV_PORT']['value'] == port1.getDevPort())
            assert(data['$SPEED'] == port1.getSpeed())
            assert(data['$FEC'] == port1.getFec())
            assert(data['$PORT_ENABLE'] == True)
            assert(data['$AUTO_NEGOTIATION'] == 'PM_AN_FORCE_ENABLE')
            assert(data['$TX_MTU'] == 1500)
            assert(data['$RX_MTU'] == 1500)
            assert(data['$TX_PFC_EN_MAP'] == 1)
            assert(data['$RX_PFC_EN_MAP'] == 1)
            assert(data['$TX_PAUSE_FRAME_EN'] == False)
            assert(data['$RX_PAUSE_FRAME_EN'] == False)
            assert(data['$RX_PRSR_PRI_THRESH'] == 1)
            assert(data['$CUT_THROUGH_EN'] == False)
            assert(data['$PORT_DIR'] == 'PM_PORT_DIR_DEFAULT')
        elif port == port2.getDevPort():
            assert(key['$DEV_PORT']['value'] == port2.getDevPort())
            assert(data['$SPEED'] == port2.getSpeed())
            assert(data['$FEC'] == port2.getFec())
            assert(data['$N_LANES'] == n_lanes)
            assert(data['$PORT_ENABLE'] == True)
            assert(data['$RX_PRSR_PRI_THRESH'] == 2)
            assert(data['$LOOPBACK_MODE'] == loop_str_val)

    logger.info("PortCfgTest: Delete entries for ports %d and %d", port1.getDevPort(), port2.getDevPort())
    self.port_table.entry_del(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port1.getDevPort())])])
    self.port_table.entry_del(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port2.getDevPort())])])


def CfgPortTableClearTest(self, port):
    target = client.Target(device_id=0, pipe_id=0xffff)
    self.port_table.entry_del(
        target,
        key_list=None)
    self.port_table.entry_add(
        target,
        [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port.getDevPort())])],
        [self.port_table.make_data([client.DataTuple('$SPEED', str_val=port.getSpeed()),
                                    client.DataTuple('$FEC', str_val=port.getFec())])])
    self.port_table.entry_del(
        target,
        key_list=None)
    try:
        get_data_list = self.port_table.make_data([client.DataTuple("$SPEED")])
        resp = self.port_table.entry_get(
            target,
            [self.port_table.make_key([client.KeyTuple('$DEV_PORT', port.getDevPort())])],
            {"from_hw": False},
            get_data_list)
        data_dict = next(resp)[0].to_dict()
        # since we have deleted all the ports, the above API call should have
        # failed. Assert if not
        logger.error("Unable to clear port cfg table")
        assert(0)
    except:
        logger.info("Cleared port cfg table successfully")


class ForwardTableTest(BfRuntimeTest):
    def setUp(self):
        BfRuntimeTest.setUp(self, 0, "tna_ports")

    def runTest(self):
        table = self.interface.bfrt_info_get().table_get("SwitchIngress.forward")
        target = client.Target(device_id=0, pipe_id=0xffff)
        ingress, egress = get_sw_ports()[:2]
        dst = "00:11:22:33:44:55"
        table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        key = table.make_key([client.KeyTuple("hdr.ethernet.dst_addr", dst)])
        data = table.make_data([client.DataTuple("port", egress)], "SwitchIngress.hit")
        table.entry_add(target, [key], [data])
        try:
            pkt = testutils.simple_tcp_packet(eth_dst=dst)
            testutils.send_packet(self, ingress, pkt)
            testutils.verify_packet(self, pkt, egress)
        finally:
            table.entry_del(target, [key])


@testutils.group("thorough")
class PortsTest(BfRuntimeTest):
    def setUp(self):
        client_id = 0
        p4_name = "tna_ports"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runTest(self):
        if testutils.test_param_get('target') != "hw":
            return
        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get()
        # Initializing all tables
        self.port_table = bfrt_info.table_get("$PORT")
        self.port_hdl_info_table = bfrt_info.table_get("$PORT_HDL_INFO")
        self.port_fp_idx_info_table = bfrt_info.table_get("$PORT_FP_IDX_INFO")
        self.port_str_info_table = bfrt_info.table_get("$PORT_STR_INFO")

        # Setting up PTF dataplane
        self.dataplane = ptf.dataplane_instance
        self.dataplane.flush()
        CfgPortTableClearTest(self, ports[0])
        PortCfgTest(self, ports[0], ports[1])
        PortHdlInfoTest(self, ports[2])


@testutils.group("thorough")
class PortsWithProgramTest(BfRuntimeTest):
    def setUp(self):
        client_id = 0
        p4_name = "tna_ports"
        BfRuntimeTest.setUp(self, client_id, p4_name)
        setup_random()

    def runTest(self):
        if testutils.test_param_get('target') != "hw":
            return
        # Get bfrt_info and set it as part of the test
        bfrt_info = self.interface.bfrt_info_get("tna_ports")
        # Initializing all tables
        self.forward_table = bfrt_info.table_get("SwitchIngress.forward")
        self.forward_table.info.key_field_annotation_add("hdr.ethernet.dst_addr", "mac")
        self.port_table = bfrt_info.table_get("$PORT")
        self.port_stat_table = bfrt_info.table_get("$PORT_STAT")
        self.port_hdl_info_table = bfrt_info.table_get("$PORT_HDL_INFO")
        self.port_fp_idx_info_table = bfrt_info.table_get("$PORT_FP_IDX_INFO")
        self.port_str_info_table = bfrt_info.table_get("$PORT_STR_INFO")
        # Setting up PTF dataplane
        self.dataplane = ptf.dataplane_instance
        self.dataplane.flush()
        CfgPortTableClearTest(self, ports[0])
        PortCfgTest(self, ports[0], ports[1])
        PortStatTest(self, ports[3], ports[2])
        # model 3/0, hw 18/0
        PortHdlInfoTest(self, ports[2])
        PortStatusCbTest(self, ports[3])
