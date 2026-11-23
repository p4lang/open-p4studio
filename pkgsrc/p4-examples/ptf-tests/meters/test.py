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
Thrift PD interface basic tests
"""

import time
import sys
import logging
import pdb

import unittest

import pd_base_tests

from ptf import config
from ptf.testutils import *
from p4testutils.misc_utils import *
from ptf.packet import *
from ptf.thriftutils import *
from ptf.mask import *
from ptf.packet import *

import os

from meters.p4_pd_rpc.ttypes import *
from p4testutils.misc_utils import *
from res_pd_rpc.ttypes import *
from collections import defaultdict

import random

this_dir = os.path.dirname(os.path.abspath(__file__))

meter_index = 5
iteration = 1
num_pass = 0

color_green = 0
color_yellow = 1
color_red = 3

dev_id = 0

swports = get_sw_ports()

if test_param_get('target') == "hw":
    print("===========================================")
    print("This test is only supported on Tofino model")
    print("===========================================")
    exit(1)

def port_to_pipe(port):
    local_port = port & 0x7F
    assert(local_port < 72)
    pipe = (port >> 7) & 0x3
    assert(port == ((pipe << 7) | local_port))
    return pipe

swports_0 = []
swports_1 = []
swports_2 = []
swports_3 = []
for port in swports:
    pipe = port_to_pipe(port)
    if pipe == 0:
        swports_0.append(port)
    elif pipe == 1:
        swports_1.append(port)
    elif pipe == 2:
        swports_2.append(port)
    elif pipe == 3:
        swports_3.append(port)

if test_param_get('arch') == "tofino":
    clock_speed = 1271000000
else:
    clock_speed = 1000000000

def b2v(x):
    if sys.version_info[0] == 2:
        return ord(x)
    return x

def advance_model_time_by_clocks(test, shdl, clocks):
    # Convert clocks to pico-seconds and advance time.
    pico_per_clock = 1000
    if test_param_get("arch") == "tofino":
        pico_per_clock = 800

    picos = clocks * pico_per_clock
    test.conn_mgr.advance_model_time(shdl, dev_id, picos)
    test.conn_mgr.complete_operations(shdl)

def run_meter_test(self, test_params, expected_results, logfile):
    global meter_index
    global iteration
    global color_green
    global color_yellow
    global color_red
    global num_pass

    offered_rate = test_params['offered_rate']
    run_time = test_params['run_time']
    pir = test_params['pir']
    pbs_time = test_params['pbs_time']
    cir = test_params['cir']
    cbs_time = test_params['cbs_time']
    pkt_size = test_params['packet_size']


    expected_green = expected_results['expected_green']
    expected_yellow = expected_results['expected_yellow']
    expected_red = expected_results['expected_red']

    # Convert the CIR and PIR from bits/second to kilobits/second
    pir_kbps = pir//(1000)
    cir_kbps = cir//(1000)
    # Convert the PBS and CBS from mseconds to bytes based on the rate
    pbs_kbits = (pbs_time)*(pir//(1000*1000))
    cbs_kbits = (cbs_time)*(cir//(1000*1000))

    cycle_count = 0
    packets_per_second = offered_rate//(pkt_size*8)
    cycles_per_packet = clock_speed//packets_per_second

    # Now that all parameters for the test are calculated, set up the match entry
    sess_hdl = self.conn_mgr.client_init()
    dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

    meter_idx = meter_index
    meter_index = meter_index + 1

    green_stats_idx = (color_green + 3) * iteration
    yellow_stats_idx = green_stats_idx + 1
    red_stats_idx = yellow_stats_idx + 1

    iteration = iteration + 1

    #print("Cycles per packet:", cycles_per_packet)
    #print("Cycle Cnt", cycle_count)
    #print("Clock Speed", clock_speed)
    #print("Run Time", run_time)
    #print("PIR kbps:", pir_kbps)
    #print("CIR kbps:", cir_kbps)
    #print("PBS kbits", pbs_kbits)
    #print("CBS kbits", cbs_kbits)
    #print("Meter Idx", meter_idx)
    #print("Green  Idx", green_stats_idx)
    #print("Yellow Idx", yellow_stats_idx)
    #print("Red    Idx", red_stats_idx)

    ipAddr = "10.11.12.13"
    match_spec = meters_meter_tbl_match_spec_t(ipv4Addr_to_i32(ipAddr))

    action_spec = meters_meter_action_action_spec_t(meter_idx)

    meter_0_spec = meters_bytes_meter_spec_t(cir_kbps, cbs_kbits, pir_kbps, pbs_kbits, False)
    self.client.meter_set_meter_0(sess_hdl, dev_tgt, meter_idx, meter_0_spec)

    mat_entry_hdl = self.client.meter_tbl_table_add_with_meter_action(sess_hdl, dev_tgt, match_spec, action_spec)

    # Now install the match entry for counting colors
    entry_hdls = []

    match_spec = meters_color_match_match_spec_t(hex_to_i32(meter_idx), color_green)
    action_spec = meters_count_color_action_spec_t(green_stats_idx)
    entry_hdls.append(self.client.color_match_table_add_with_count_color(sess_hdl, dev_tgt, match_spec, action_spec))

    match_spec = meters_color_match_match_spec_t(hex_to_i32(meter_idx), color_yellow)
    action_spec = meters_count_color_action_spec_t(yellow_stats_idx)
    entry_hdls.append(self.client.color_match_table_add_with_count_color(sess_hdl, dev_tgt, match_spec, action_spec))

    match_spec = meters_color_match_match_spec_t(hex_to_i32(meter_idx), color_red)
    action_spec = meters_count_color_action_spec_t(red_stats_idx)
    entry_hdls.append(self.client.color_match_table_add_with_count_color(sess_hdl, dev_tgt, match_spec, action_spec))

    # Harlyn, puts in 4 bytes of CRC, so the packet seen by MAU will be the packet length + 4 Bytes.
    # Hence, the packet size we send is 4 bytes lesser than desired value.
    pkt = simple_tcp_packet(pktlen=pkt_size-4,
                            ip_dst=ipAddr,
                            with_tcp_chksum=False)

    i = 0

    msk = Mask(pkt)
    mask_set_do_not_care_packet(msk, IP, "tos")
    mask_set_do_not_care_packet(msk, IP, "chksum")
    while (cycle_count < (clock_speed*run_time)):
        advance_model_time_by_clocks(self, sess_hdl, cycles_per_packet)
        cycle_count = cycle_count + cycles_per_packet
        send_packet(self, swports[1], pkt)
        verify_packet(self, msk, swports[1])
        #time.sleep(0.05)
        #(rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll(timeout=2)

    # Now verify if the results match up the expected results.

    # Tolerance factor of 1.1%
    tolerance = 0.011

    #assert (abs(red_count.packets - expected_green) < (expected_red * tolerance))


    # Delete the entry
    self.client.meter_tbl_table_delete(sess_hdl, dev_id, mat_entry_hdl)

    #self.client.counter_hw_sync_colorCntr(sess_hdl, dev_tgt)
    #self.conn_mgr.complete_operations(sess_hdl)

    flags = meters_counter_flags_t(1)

    green_count = self.client.counter_read_colorCntr(sess_hdl, dev_tgt, green_stats_idx, flags)
    self.conn_mgr.complete_operations(sess_hdl)
    yellow_count = self.client.counter_read_colorCntr(sess_hdl, dev_tgt, yellow_stats_idx, flags)
    self.conn_mgr.complete_operations(sess_hdl)
    red_count = self.client.counter_read_colorCntr(sess_hdl, dev_tgt, red_stats_idx, flags)
    self.conn_mgr.complete_operations(sess_hdl)

    pass_0 = 0
    pass_1 = 0
    pass_2 = 0

    # Set the counter to zero
    #counter_value = meters_counter_value_t(packets=0, bytes=0)

    #self.client.counter_write_colorCntr(sess_hdl, dev_tgt, green_stats_idx, counter_value)
    #self.client.counter_write_colorCntr(sess_hdl, dev_tgt, yellow_stats_idx, counter_value)
    #self.client.counter_write_colorCntr(sess_hdl, dev_tgt, red_stats_idx, counter_value)

    print("Expected green = %d, green count = %d" % (expected_green, green_count.packets))
    #assert (abs(green_count.packets - expected_green) < (expected_green * tolerance))
    logfile.write("Expected green = %d, green count = %d\n" % (expected_green, green_count.packets))

    print("Expected yellow = %d, yellow count = %d" % (expected_yellow, yellow_count.packets))
    logfile.write("Expected Yellow = %d, Yellow count = %d\n" % (expected_yellow, yellow_count.packets))
    #assert (abs(yellow_count.packets - expected_yellow) < (expected_yellow * tolerance))

    print("Expected red = %d, red count = %d" % (expected_red, red_count.packets))
    logfile.write("Expected Red = %d, Red count = %d\n" % (expected_red, red_count.packets))

    if (abs(green_count.packets - expected_green) <= (expected_green * tolerance)):
        pass_0 = 1
    else:
        if (expected_green * tolerance == 0):
            if (green_count.packets - expected_green <= 2):
                pass_0 = 1

    if (abs(yellow_count.packets - expected_yellow) <= (expected_yellow * tolerance)):
        pass_1 = 1
    else:
        if (expected_yellow * tolerance == 0):
            if (yellow_count.packets - expected_yellow <= 2):
                pass_1 = 1

    if (abs(red_count.packets - expected_red) <= (expected_red * tolerance)):
        pass_2 = 1
    else:
        if (expected_red * tolerance == 0):
            if (red_count.packets - expected_red <= 2):
                pass_1 = 1

    if (pass_0 == 1 and pass_1 == 1 and pass_2 == 1):
        logfile.write("PASS\n")
        num_pass = num_pass + 1
    else:
        logfile.write("FAIL\n")

    for entry_hdl in entry_hdls:
        self.client.color_match_table_delete(sess_hdl, dev_id, entry_hdl)
    print("closing session")
    advance_model_time_by_clocks(self, sess_hdl, 25000000)
    self.conn_mgr.client_cleanup(sess_hdl)

class TestExmMeterIndirect(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Basic meter test with Exact match table, indirectly addressed """
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_ExmMeterIndirect(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)

        port = swports[1]
        meter_idx = 5
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        global color_green
        global color_yellow
        global color_red

        match_spec = meters_meter_tbl_match_spec_t(ipv4Addr_to_i32("10.1.1.10"))

        action_spec = meters_meter_action_action_spec_t(meter_idx)

        # CIR of 1Mbps, with burst size of 500 bytes (5 packets worth), PIR of 2Mbps with burstsize of 1000 bytes (10 packets worth)
        meter_0_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, False)



        pkt = simple_tcp_packet(pktlen=96,
                                ip_dst='10.1.1.10',
                                with_tcp_chksum=False)

        green_pkt = simple_tcp_packet(pktlen=96,
                                      ip_dst='10.1.1.10',
                                      ip_tos=color_green,
                                      with_tcp_chksum=False)
        yellow_pkt = simple_tcp_packet(pktlen=96,
                                       ip_dst='10.1.1.10',
                                       ip_tos=color_yellow,
                                       with_tcp_chksum=False)
        red_pkt = simple_tcp_packet(pktlen=96,
                                    ip_dst='10.1.1.10',
                                    ip_tos=color_red,
                                    with_tcp_chksum=False)

        # Do not set the time for meters. Let it stay at zero, since we do not want
        # the model to queue color writes to MapRam which is done to simulate the mapram color write latency.
        try:
            sess_hdl = self.conn_mgr.client_init()
            self.client.meter_set_meter_0(sess_hdl, dev_tgt, meter_idx, meter_0_spec)
            entry_hdl = self.client.meter_tbl_table_add_with_meter_action(sess_hdl, dev_tgt, match_spec, action_spec)
            self.conn_mgr.complete_operations(sess_hdl)
            # 10 packets would exhaust the committed burst size, but since color
            # based meters operate on EOP, the color output on the
            # packet will be seen only on the next packet. i,e.. at the
            # end of processing 11 packets, color would be updated, and
            # on the 12th packet, color will take effect on the packet

            print("Sending 11 packets, all should be green")
            sys.stdout.flush()
            for i in range (0, 11):
                send_packet(self, port, pkt)
                try:
                    verify_packet(self, green_pkt, port)
                except AssertionError as e:
                    print("Failed to verify packet", i+1, "of", len(list(range(0,11))))
                    sys.stdout.flush()
                    raise e

            # Now send one more packet to see the color change to yellow

            # Set the time to something > than the mapram color write latency (which is 13 clock cycles currently in the model)
            # This is so that the mapram color writes that are queued get executed.
            print("Advancing model time to allow the color mapram to be updated")
            sys.stdout.flush()
            advance_model_time_by_clocks(self, sess_hdl, 20)

            print("Sending one more packet, it should be marked yellow")
            sys.stdout.flush()
            send_packet(self, port, pkt)
            verify_packet(self, yellow_pkt, port)

            # So far we have sent 12 packets. Send 9 more to exhaust the
            # peak burst size and change the color.
            print("Sending 9 packets, all should be yellow")
            sys.stdout.flush()
            for i in range (0, 9):
                send_packet(self, port, pkt)
                try:
                    verify_packet(self, yellow_pkt, port)
                except AssertionError as e:
                    print("Failed to verify packet", i+1, "of", len(list(range(0,9))))
                    sys.stdout.flush()
                    raise e

            # Bump up the time so that the map-ram color write gets executed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            print("Sending one more packet, it should be marked red")
            sys.stdout.flush()
            send_packet(self, port, pkt)
            verify_packet(self, red_pkt, port)


            # Now set the time to a value such that both the buckets get
            # get filled on the next execution of the meter and subsequent
            # packet gets colored green

            print("Advancing time to refill the meter bucket")
            sys.stdout.flush()
            advance_model_time_by_clocks(self, sess_hdl, 25000000)

            # Now send a packet, so that the color gets updated. However
            # the packet color still remains red.

            print("Sending one more packet, it should be marked red")
            sys.stdout.flush()
            send_packet(self, port, pkt)
            verify_packet(self, red_pkt, port)

            print("Advancing model time to allow the color mapram to be updated")
            sys.stdout.flush()
            advance_model_time_by_clocks(self, sess_hdl, 20)

            # Now, this packet should have a color of green

            print("Sending one more packet, it should be marked green")
            sys.stdout.flush()
            send_packet(self, port, pkt)
            verify_packets(self, green_pkt, [port])

        finally:
            print("Deleting entry")
            self.client.meter_tbl_table_delete(sess_hdl,
                                               dev_id,
                                               entry_hdl)
            advance_model_time_by_clocks(self, sess_hdl, 25000000)
            self.conn_mgr.client_cleanup(sess_hdl)


class TestExmMeterDirect(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Basic meter test with Exact match table, directly addressed """
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_ExmMeterDirect(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)

        sess_hdl = self.conn_mgr.client_init()

        port = swports[1]
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        global color_green
        global color_yellow
        global color_red

        match_spec = meters_meter_tbl_direct_match_spec_t(ipv4Addr_to_i32("10.1.1.10"))


        # CIR of 1Mbps, with burst size of 500 bytes (5 packets worth), PIR of 2Mbps with burstsize of 1000 bytes (10 packets worth)
        meter_1_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, False)
        entry_hdl = self.client.meter_tbl_direct_table_add_with_nop(sess_hdl, dev_tgt, match_spec, meter_1_spec)

        print("Adding entry")

        advance_model_time_by_clocks(self, sess_hdl, 1000000)

        # Validate meter entry
        # Since meter values are stored in a different format in hw (mantissa/exponent),
        # the values read may not exactly match the user's values.
        tolerance = 0.2
        read_meter_spec = self.client.meter_read_meter_1(sess_hdl, dev_tgt, entry_hdl)
        assert (abs(read_meter_spec.cburst_kbits - meter_1_spec.cburst_kbits) < tolerance * meter_1_spec.cburst_kbits)
        assert (abs(read_meter_spec.pburst_kbits - meter_1_spec.pburst_kbits) < tolerance * meter_1_spec.pburst_kbits)
        assert (abs(read_meter_spec.cir_kbps - meter_1_spec.cir_kbps) < tolerance * meter_1_spec.cir_kbps)
        assert (abs(read_meter_spec.pir_kbps - meter_1_spec.pir_kbps) < tolerance * meter_1_spec.pir_kbps)
        assert (read_meter_spec.color_aware == False)

        pkt = simple_tcp_packet(pktlen=96,
                                ip_dst='10.1.1.10',
                                with_tcp_chksum=False)

        green_pkt = simple_tcp_packet(pktlen=96,
                                      ip_dst='10.1.1.10',
                                      ip_tos=color_green,
                                      with_tcp_chksum=False)
        yellow_pkt = simple_tcp_packet(pktlen=96,
                                       ip_dst='10.1.1.10',
                                       ip_tos=color_yellow,
                                       with_tcp_chksum=False)
        red_pkt = simple_tcp_packet(pktlen=96,
                                    ip_dst='10.1.1.10',
                                    ip_tos=color_red,
                                    with_tcp_chksum=False)

        try:
            # 10 packets would exhaust the committed burst size, but since color
            # based meters operate on EOP, the color output on the
            # packet will be seen only on the next packet. i,e.. at the
            # end of processing 11 packets, color would be updated, and
            # on the 12th packet, color will take effect on the packet

            for i in range (0, 11):
                send_packet(self, port, pkt)
                try:
                    verify_packet(self, green_pkt, port)
                except AssertionError as e:
                    print("Failed to verify green packet", i+1, "of", len(list(range(0,11))))
                    sys.stdout.flush()
                    raise e
            # Now send one more packet to see the color change to yellow

# Set the time to something > than the mapram color write latency (which is 13 clock cycles currently in the model)
# This is so that the mapram color writes that are queued get executed.

            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port, pkt)
            verify_packet(self, yellow_pkt, port)

            # So far we have sent 12 packets. Send 9 more to exhaust the
            # peak burst size and change the color.
            for i in range (0, 9):
                send_packet(self, port, pkt)
                try:
                    verify_packet(self, yellow_pkt, port)
                except AssertionError as e:
                    print("Failed to verify yellow packet", i+1, "of", len(list(range(0,9))))
                    sys.stdout.flush()
                    raise e

            # Now send one more packet to see the packet color change to red

            # Bump up the time so that the map-ram color write gets executed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port, pkt)
            verify_packet(self, red_pkt, port)


            # Now set the time to a value such that both the buckets get
            # get filled on the next execution of the meter and subsequent
            # packet gets colored green

            advance_model_time_by_clocks(self, sess_hdl, 25000000)

            # Now send a packet, so that the color gets updated. However
            # the packet color still remains red.

            send_packet(self, port, pkt)
            verify_packet(self, red_pkt, port)

            # Bump up the time so that the map-ram color write gets executed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            # Now, this packet should have a color of green

            send_packet(self, port, pkt)
            verify_packets(self, green_pkt, [port])

        finally:
            print("Deleting entry")
            self.client.meter_tbl_direct_table_delete(sess_hdl,
                                                      dev_id,
                                                      entry_hdl)
            print("closing session")
            advance_model_time_by_clocks(self, sess_hdl, 25000000)
            self.conn_mgr.client_cleanup(sess_hdl)



class TestExmDirectRed(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Basic meter test with Exact match table, directly addressed """
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_ExmMeterDirect(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)

        sess_hdl = self.conn_mgr.client_init()

        port_r = swports[1]
        port_g = swports[2]
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        global color_green
        global color_yellow
        global color_red

        match_spec_r = meters_meter_tbl_direct_match_spec_t(ipv4Addr_to_i32("10.1.1.10"))
        match_spec_g = meters_meter_tbl_direct_match_spec_t(ipv4Addr_to_i32("10.1.1.11"))

        spec_r = meters_bytes_meter_spec_t(0, 0, 0, 0, False)
        spec_g = meters_bytes_meter_spec_t(0x7FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, False)
        hdl_r = self.client.meter_tbl_direct_table_add_with_nop(sess_hdl, dev_tgt, match_spec_r, spec_r)
        hdl_g = self.client.meter_tbl_direct_table_add_with_nop(sess_hdl, dev_tgt, match_spec_g, spec_g)

        advance_model_time_by_clocks(self, sess_hdl, 1000000)

        pkt_r = simple_tcp_packet(pktlen=96,
                                 ip_dst='10.1.1.10',
                                 with_tcp_chksum=False)
        pkt_g = simple_tcp_packet(pktlen=96,
                                 ip_dst='10.1.1.11',
                                 with_tcp_chksum=False)

        green_pkt = simple_tcp_packet(pktlen=96,
                                      ip_dst='10.1.1.11',
                                      ip_tos=color_green,
                                      with_tcp_chksum=False)
        red_pkt = simple_tcp_packet(pktlen=96,
                                    ip_dst='10.1.1.10',
                                    ip_tos=color_red,
                                    with_tcp_chksum=False)

        try:
            for i in range (0, 20):
                send_packet(self, port_r, pkt_r)
                verify_packet(self, red_pkt, port_r)
                send_packet(self, port_g, pkt_g)
                verify_packet(self, green_pkt, port_g)

            # Advance model time to allow color map ram updates to be processed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            for i in range(20):
                send_packet(self, port_r, pkt_r)
                verify_packet(self, red_pkt, port_r)
                send_packet(self, port_g, pkt_g)
                verify_packet(self, green_pkt, port_g)

            # Advance model time to allow color map ram updates to be processed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            for i in range(20):
                send_packet(self, port_r, pkt_r)
                verify_packet(self, red_pkt, port_r)
                send_packet(self, port_g, pkt_g)
                verify_packet(self, green_pkt, port_g)

            # Advance time again, the packets color should not change
            advance_model_time_by_clocks(self, sess_hdl, 25000000)

            for i in range(10):
                send_packet(self, port_r, pkt_r)
                verify_packet(self, red_pkt, port_r)
                send_packet(self, port_g, pkt_g)
                verify_packet(self, green_pkt, port_g)

            # Advance model time to allow color map ram updates to be processed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port_r, pkt_r)
            verify_packets(self, red_pkt, [port_r])
            send_packet(self, port_g, pkt_g)
            verify_packets(self, green_pkt, [port_g])

        finally:
            print("Deleting entry")
            self.client.meter_tbl_direct_table_delete(sess_hdl,
                                                      dev_id,
                                                      hdl_r)
            self.client.meter_tbl_direct_table_delete(sess_hdl,
                                                      dev_id,
                                                      hdl_g)
            print("closing session")
            # Advance model time otherwise see intermittent failures.
            # This change applied to *all* (except Omnet) meter tests in this file
            advance_model_time_by_clocks(self, sess_hdl, 25000000)
            self.conn_mgr.client_cleanup(sess_hdl)

class TestExmMeterColorAwareIndirect(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Basic Color aware meter test with Exact match table, indirectly addressed """
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_ExmMeterColorAwareIndirect(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)

        sess_hdl = self.conn_mgr.client_init()

        port = swports[1]
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        global color_green
        global color_yellow
        global color_red

        ipdstAddr = "10.1.1.1"

        ipdstAddr1 = "10.2.2.2"

        match_spec_color_aware = meters_meter_tbl_color_aware_indirect_match_spec_t(ipv4Addr_to_i32(ipdstAddr))

        match_spec_color_unaware = meters_meter_tbl_color_aware_indirect_match_spec_t(ipv4Addr_to_i32(ipdstAddr1))

        meter_idx_color_aware = 52
        meter_idx_color_unaware = 33

        action_spec_color_aware = meters_meter_action_color_aware_action_spec_t(meter_idx_color_aware)

        action_spec_color_unaware = meters_meter_action_color_aware_action_spec_t(meter_idx_color_unaware)

        # Install a color-aware meter entry
        meter_2_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, True)
        self.client.meter_set_meter_2(sess_hdl, dev_tgt, meter_idx_color_aware, meter_2_spec)
        # Install a color-unaware meter entry
        meter_2_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, False)
        self.client.meter_set_meter_2(sess_hdl, dev_tgt, meter_idx_color_unaware, meter_2_spec)

        # Add two match entries, one pointing to color-aware meter and the other pointing to a
        # color unaware meter
        entry_hdl_color_aware = self.client.meter_tbl_color_aware_indirect_table_add_with_meter_action_color_aware(sess_hdl, dev_tgt, match_spec_color_aware, action_spec_color_aware)
        entry_hdl_color_unaware = self.client.meter_tbl_color_aware_indirect_table_add_with_meter_action_color_unaware(sess_hdl, dev_tgt, match_spec_color_unaware, action_spec_color_unaware)

        # Validate meter entries
        # Since meter values are stored in a different format in hw (mantissa/exponent),
        # the values read may not exactly match the user's values.
        tolerance = 0.2
        read_meter_spec = self.client.meter_read_meter_2(sess_hdl, dev_tgt, meter_idx_color_aware)
        assert (abs(read_meter_spec.cburst_kbits - meter_2_spec.cburst_kbits) < tolerance * meter_2_spec.cburst_kbits)
        assert (abs(read_meter_spec.pburst_kbits - meter_2_spec.pburst_kbits) < tolerance * meter_2_spec.pburst_kbits)
        assert (abs(read_meter_spec.cir_kbps - meter_2_spec.cir_kbps) < tolerance * meter_2_spec.cir_kbps)
        assert (abs(read_meter_spec.pir_kbps - meter_2_spec.pir_kbps) < tolerance * meter_2_spec.pir_kbps)
        assert (read_meter_spec.color_aware == True)

        read_meter_spec = self.client.meter_read_meter_2(sess_hdl, dev_tgt, meter_idx_color_unaware)
        assert (abs(read_meter_spec.cburst_kbits - meter_2_spec.cburst_kbits) < tolerance * meter_2_spec.cburst_kbits)
        assert (abs(read_meter_spec.pburst_kbits - meter_2_spec.pburst_kbits) < tolerance * meter_2_spec.pburst_kbits)
        assert (abs(read_meter_spec.cir_kbps - meter_2_spec.cir_kbps) < tolerance * meter_2_spec.cir_kbps)
        assert (abs(read_meter_spec.pir_kbps - meter_2_spec.pir_kbps) < tolerance * meter_2_spec.pir_kbps)
        assert (read_meter_spec.color_aware == False)

        # Install another entry to validate that we can read the rate back correctly.
        self.client.meter_set_meter_2(sess_hdl, dev_tgt, 123, meters_bytes_meter_spec_t(1,1,2,2, True))
        self.conn_mgr.complete_operations(sess_hdl)
        read_meter_spec = self.client.meter_read_meter_2(sess_hdl, dev_tgt, 123)
        assert(read_meter_spec.cir_kbps == 1)
        assert(read_meter_spec.cburst_kbits == 1)
        assert(read_meter_spec.pir_kbps == 2)
        assert(read_meter_spec.pburst_kbits == 2)
        assert(read_meter_spec.color_aware == True)
        self.client.meter_set_meter_2(sess_hdl, dev_tgt, 123, meters_bytes_meter_spec_t(0,0,0,0, True))
        self.conn_mgr.complete_operations(sess_hdl)
        read_meter_spec = self.client.meter_read_meter_2(sess_hdl, dev_tgt, 123)
        assert(read_meter_spec.cir_kbps == 0)
        assert(read_meter_spec.cburst_kbits == 0)
        assert(read_meter_spec.pir_kbps == 0)
        assert(read_meter_spec.pburst_kbits == 0)
        assert(read_meter_spec.color_aware == True)

        # Try to write to an invalid index, this should fail.
        try:
            self.client.meter_set_meter_2(sess_hdl, dev_tgt, 500, meters_bytes_meter_spec_t(0,0,0,0, True))
            self.assertTrue(False)
        except InvalidMeterOperation as e:
            pass
        try:
            self.client.meter_set_meter_2(sess_hdl, dev_tgt, 1023, meters_bytes_meter_spec_t(0,0,0,0, True))
            self.assertTrue(False)
        except InvalidMeterOperation as e:
            pass
        try:
            self.client.meter_set_meter_2(sess_hdl, dev_tgt, 1000000, meters_bytes_meter_spec_t(0,0,0,0, True))
            self.assertTrue(False)
        except InvalidMeterOperation as e:
            pass

        # Try to read from an invalid index, this should fail.
        try:
            self.client.meter_read_meter_2(sess_hdl, dev_tgt, 500)
            self.assertTrue(False)
        except InvalidMeterOperation as e:
            pass
        try:
            self.client.meter_read_meter_2(sess_hdl, dev_tgt, 1023)
            self.assertTrue(False)
        except InvalidMeterOperation as e:
            pass
        try:
            self.client.meter_read_meter_2(sess_hdl, dev_tgt, 1000000)
            self.assertTrue(False)
        except InvalidMeterOperation as e:
            pass

        advance_model_time_by_clocks(self, sess_hdl, 1000000)

        # Now send a packet which would hit the match-entry which is attached to a
        # a color aware meter, pre color the packet to red and verify the color is
        # not upgraded.

        pkt_c_aware = simple_tcp_packet(pktlen=96,
                                        ip_dst=ipdstAddr,
                                        ip_tos=color_red,
                                        with_tcp_chksum=False)
        exp_c_aware_g = simple_tcp_packet(pktlen=96,
                                          ip_dst=ipdstAddr,
                                          ip_tos=color_green,
                                          with_tcp_chksum=False)
        exp_c_aware_y = simple_tcp_packet(pktlen=96,
                                          ip_dst=ipdstAddr,
                                          ip_tos=color_yellow,
                                          with_tcp_chksum=False)
        exp_c_aware_r = simple_tcp_packet(pktlen=96,
                                          ip_dst=ipdstAddr,
                                          ip_tos=color_red,
                                          with_tcp_chksum=False)
        pkt_c_unaware = simple_tcp_packet(pktlen=96,
                                          ip_dst=ipdstAddr1,
                                          with_tcp_chksum=False)
        exp_c_unaware_g = simple_tcp_packet(pktlen=96,
                                            ip_dst=ipdstAddr1,
                                            ip_tos=color_green,
                                            with_tcp_chksum=False)
        exp_c_unaware_y = simple_tcp_packet(pktlen=96,
                                            ip_dst=ipdstAddr1,
                                            ip_tos=color_yellow,
                                            with_tcp_chksum=False)
        exp_c_unaware_r = simple_tcp_packet(pktlen=96,
                                            ip_dst=ipdstAddr1,
                                            ip_tos=color_red,
                                            with_tcp_chksum=False)

        try:
            send_packet(self, port, pkt_c_aware)
            verify_packet(self, exp_c_aware_r, port)

            # Set the time to something > than the mapram color write latency (which is 13 clock cycles currently in the model)
            # This is so that the mapram color writes that are queued get executed.

            advance_model_time_by_clocks(self, sess_hdl, 20)

            # Now send another packet.

            send_packet(self, port, pkt_c_aware)
            verify_packet(self, exp_c_aware_r, port)

            # Now send packets for color unaware entry

            # 10 packets would exhaust the committed burst size, but since color
            # based meters operate on EOP, the color output on the
            # packet will be seen only on the next packet. i,e.. at the
            # end of processing 11 packets, color would be updated, and
            # on the 12th packet, color will take effect on the packet

            for i in range (0, 11):
                send_packet(self, port, pkt_c_unaware)
                try:
                    verify_packet(self, exp_c_unaware_g, port)
                except AssertionError as e:
                    print("Failed to verify green packet", i+1, "of", len(list(range(0,11))))
                    sys.stdout.flush()
                    raise e

            # Now send one more packet to see the color change to yellow

            # Set the time to something > than the mapram color write latency (which is 13 clock cycles currently in the model)
            # This is so that the mapram color writes that are queued get executed.

            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port, pkt_c_unaware)
            verify_packet(self, exp_c_unaware_y, port)

            # So far we have sent 12 packets. Send 9 more to exhaust the
            # peak burst size and change the color.
            for i in range (0, 9):
                send_packet(self, port, pkt_c_unaware)
                try:
                    verify_packet(self, exp_c_unaware_y, port)
                except AssertionError as e:
                    print("Failed to verify yellow packet", i+1, "of", len(list(range(0,9))))
                    sys.stdout.flush()
                    raise e

            # Now send one more packet to see the packet color change to red

            # Bump up the time so that the map-ram color write gets executed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port, pkt_c_unaware)
            verify_packet(self, exp_c_unaware_r, port)


            # Now set the time to a value such that both the buckets get
            # get filled on the next execution of the meter and subsequent
            # packet gets colored green

            advance_model_time_by_clocks(self, sess_hdl, 25000000)

            # Now send a packet, so that the color gets updated. However
            # the packet color still remains red.

            send_packet(self, port, pkt_c_unaware)
            verify_packet(self, exp_c_unaware_r, port)

            advance_model_time_by_clocks(self, sess_hdl, 20)

            # Now, this packet should have a color of green

            send_packet(self, port, pkt_c_unaware)
            verify_packets(self, exp_c_unaware_g, [port])

        finally:
            print("Deleting entries")
            self.client.meter_tbl_color_aware_indirect_table_delete(sess_hdl,
                                                                    dev_id,
                                                                    entry_hdl_color_aware)

            self.client.meter_tbl_color_aware_indirect_table_delete(sess_hdl,
                                                                    dev_id,
                                                                    entry_hdl_color_unaware)
            advance_model_time_by_clocks(self, sess_hdl, 25000000)
            self.conn_mgr.client_cleanup(sess_hdl)

@group("thorough")
class TestMeterOmnet(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Meter tests that were run on omnet """
    def runTest(self):
        global num_pass
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_MeterOmnet(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)
        num_tests = 6
        logfile = open('Omnet_test_log', 'w')
        testMatrix = defaultdict(lambda:defaultdict(defaultdict))

        # Test case names correspond to the exact same name in the model (modules/tofino_rmt/utest/test_meters.cpp)

################ TC3 ################################################
        # Commenting out the 64 byte packet test. Since model adds 4 bytes, the packet that will be seen
        # by the meter ALU will be 68 bytes, which throws off the omnet expected packet color counts.
        # When we send a packet less than 64 bytes, it is padded to 64 bytes the kernel.

        #testMatrix['TC3']['test_params']['offered_rate'] = 9000000
        #testMatrix['TC3']['test_params']['run_time'] = 0.1
        #testMatrix['TC3']['test_params']['pir'] = 9000000
        #testMatrix['TC3']['test_params']['pbs_time'] = 350
        #testMatrix['TC3']['test_params']['cir'] = 1000000
        #testMatrix['TC3']['test_params']['cbs_time'] = 300
        #testMatrix['TC3']['test_params']['packet_size'] = 64

        #testMatrix['TC3']['expected_results']['expected_green'] = 782
        #testMatrix['TC3']['expected_results']['expected_yellow'] = 977
        #testMatrix['TC3']['expected_results']['expected_red'] = 0
###################################################################

################ TC4 ################################################
        testMatrix['TC4']['test_params']['offered_rate'] = 30000000
        testMatrix['TC4']['test_params']['run_time'] = 0.1
        testMatrix['TC4']['test_params']['pir'] = 40000000
        testMatrix['TC4']['test_params']['pbs_time'] = 190
        testMatrix['TC4']['test_params']['cir'] = 10000000
        testMatrix['TC4']['test_params']['cbs_time'] = 100
        testMatrix['TC4']['test_params']['packet_size'] = 768

        testMatrix['TC4']['expected_results']['expected_green'] = 326
        testMatrix['TC4']['expected_results']['expected_yellow'] = 163
        testMatrix['TC4']['expected_results']['expected_red'] = 0
###################################################################

################ TC5 ################################################
        testMatrix['TC5']['test_params']['offered_rate'] = 50000000
        testMatrix['TC5']['test_params']['run_time'] = 0.1
        testMatrix['TC5']['test_params']['pir'] = 60000000
        testMatrix['TC5']['test_params']['pbs_time'] = 150
        testMatrix['TC5']['test_params']['cir'] = 50000000
        testMatrix['TC5']['test_params']['cbs_time'] = 100
        testMatrix['TC5']['test_params']['packet_size'] = 256

        testMatrix['TC5']['expected_results']['expected_green'] = 2442
        testMatrix['TC5']['expected_results']['expected_yellow'] = 0
        testMatrix['TC5']['expected_results']['expected_red'] = 0
###################################################################

################ TC6 ################################################
        testMatrix['TC6']['test_params']['offered_rate'] = 80000000
        testMatrix['TC6']['test_params']['run_time'] = 0.1
        testMatrix['TC6']['test_params']['pir'] = 88000000
        testMatrix['TC6']['test_params']['pbs_time'] = 90
        testMatrix['TC6']['test_params']['cir'] = 80000000
        testMatrix['TC6']['test_params']['cbs_time'] = 50
        testMatrix['TC6']['test_params']['packet_size'] = 1560

        testMatrix['TC6']['expected_results']['expected_green'] = 642
        testMatrix['TC6']['expected_results']['expected_yellow'] = 0
        testMatrix['TC6']['expected_results']['expected_red'] = 0
#####################################################################

################ TC7 ################################################
        testMatrix['TC7']['test_params']['offered_rate'] = 199000000
        testMatrix['TC7']['test_params']['run_time'] = 0.1
        testMatrix['TC7']['test_params']['pir'] = 230000000
        testMatrix['TC7']['test_params']['pbs_time'] = 77
        testMatrix['TC7']['test_params']['cir'] = 200000000
        testMatrix['TC7']['test_params']['cbs_time'] = 50
        testMatrix['TC7']['test_params']['packet_size'] = 400

        testMatrix['TC7']['expected_results']['expected_green'] = 6219
        testMatrix['TC7']['expected_results']['expected_yellow'] = 0
        testMatrix['TC7']['expected_results']['expected_red'] = 0
#####################################################################

################ TC8 ################################################
#testMatrix['TC8']['test_params']['offered_rate'] = 400000000
#        testMatrix['TC8']['test_params']['run_time'] = 0.1
#        testMatrix['TC8']['test_params']['pir'] = 410000000
#        testMatrix['TC8']['test_params']['pbs_time'] = 130
#        testMatrix['TC8']['test_params']['cir'] = 400000000
#        testMatrix['TC8']['test_params']['cbs_time'] = 100
#        testMatrix['TC8']['test_params']['packet_size'] = 74

#        testMatrix['TC8']['expected_results']['expected_green'] = 67568
#        testMatrix['TC8']['expected_results']['expected_yellow'] = 0
#        testMatrix['TC8']['expected_results']['expected_red'] = 0
#####################################################################

################ TC9 ################################################
        testMatrix['TC9']['test_params']['offered_rate'] = 900000000
        testMatrix['TC9']['test_params']['run_time'] = 0.1
        testMatrix['TC9']['test_params']['pir'] = 1500000000
        testMatrix['TC9']['test_params']['pbs_time'] = 12
        testMatrix['TC9']['test_params']['cir'] = 1000000000
        testMatrix['TC9']['test_params']['cbs_time'] = 11
        testMatrix['TC9']['test_params']['packet_size'] = 1560

        testMatrix['TC9']['expected_results']['expected_green'] = 7212
        testMatrix['TC9']['expected_results']['expected_yellow'] = 0
        testMatrix['TC9']['expected_results']['expected_red'] = 0
###################################################################

################ TC10 ################################################
        testMatrix['TC10']['test_params']['offered_rate'] = 500000000
        testMatrix['TC10']['test_params']['run_time'] = 0.1
        testMatrix['TC10']['test_params']['pir'] = 3400000000
        testMatrix['TC10']['test_params']['pbs_time'] = 6
        testMatrix['TC10']['test_params']['cir'] = 3300000000
        testMatrix['TC10']['test_params']['cbs_time'] = 5
        testMatrix['TC10']['test_params']['packet_size'] = 800

        testMatrix['TC10']['expected_results']['expected_green'] = 7813
        testMatrix['TC10']['expected_results']['expected_yellow'] = 0
        testMatrix['TC10']['expected_results']['expected_red'] = 0
###################################################################

        #for each_test in ['TC4','TC5','TC6','TC7','TC9','TC10']:
        for each_test in testMatrix:
            logfile.write("Test : %s\n" % (each_test))
            print(each_test)
            run_meter_test(self, testMatrix[each_test]['test_params'], testMatrix[each_test]['expected_results'], logfile)
            logfile.write("########################\n")

        if (num_pass != num_tests):
            print("FAIL : Check Omnet_test_logs for failures")
            print(" Num pass = %d" % (num_pass))
            assert(num_pass == num_tests)



class TestExmLpfIndirect(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])
    """ LPF test with Exact match table """
    def runTest(self):

        port = swports[1]
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        # [ input_val, [ (time, expected output value) ] ]
        lpf_test0 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 40000),
                             (0.000001437466, 60000),
                             (0.000001639913, 79533),
                             (0.000002570436, 98914),
                             (0.000004805749, 117381),
                             (0.000005406021, 136467),
                             (0.000005434843, 156467),
                             (0.000006052835, 175249),
                             (0.000006059165, 195249)]]

        lpf_test1 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 27500),
                             (0.000001437466, 20482),
                             (0.000001639913, 22720),
                             (0.000002570436, 20006),
                             (0.000004805749, 20000),
                             (0.000005406021, 20005),
                             (0.000005434843, 20351),
                             (0.000006052835, 20005),
                             (0.000006059165, 35629)]]

        lpf_test2 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 22656), # 0.000000115532
                             (0.000001437466, 20006), # 0.000000502734
                             (0.000001639913, 20351), # 0.000000202447
                             (0.000002570436, 20000), # 0.000000930523
                             (0.000004805749, 20000), # 0.000002235313
                             (0.000005406021, 20000), # 0.000000600272
                             (0.000005434843, 20005), # 0.000000028822
                             (0.000006052835, 20000), # 0.000000617992
                             (0.000006059165, 32500)]]# 0.000000006330

        lpf_meter_spec0 = meters_lpf_spec_t(False, 0, 0, 104857.6, 0, 0, True)
        lpf_meter_spec1 = meters_lpf_spec_t(False, 0, 0, 102.4, 0, 0, True)
        lpf_meter_spec2 = meters_lpf_spec_t(False, 0, 0, 51.2, 0, 0, True)

        lpf_test_matrix = [lpf_test0, lpf_test1, lpf_test2]
        lpf_spec_matrix = [lpf_meter_spec0, lpf_meter_spec1, lpf_meter_spec2]

        lpf_test = list(zip([0,1,2], lpf_test_matrix, lpf_spec_matrix))
        for which_test, each_lpf_test, lpf_meter_spec in lpf_test:
            self.client.test_select_set_default_action_GetTimeForLPFTest(sess_hdl, dev_tgt)
            self.conn_mgr.complete_operations(sess_hdl)
            send_packet(self, port, simple_ip_packet())
            try:
                (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=3.0 )
                assert(rcv_pkt is not None)
                pass
            except AssertionError as e:
                self.conn_mgr.client_cleanup(sess_hdl)
                raise e
            rx_ts = 0
            for b in range(6):
                rx_ts = (rx_ts << 8) | b2v(rcv_pkt[b])

            print("Starting test", which_test, "at time", hex(rx_ts))
            current_time = rx_ts
            tgt_time = (1 + (rx_ts // 2**28)) * 2**28
            print("Advancing time to", hex(tgt_time))
            advance_model_time_by_clocks(self, sess_hdl, tgt_time - current_time)
            current_time += tgt_time - current_time
            sys.stdout.flush()

            self.client.test_select_set_default_action_ExmLpfIndirect(sess_hdl, dev_tgt)

            time = 0
            input_val = each_lpf_test[0]
            lpf_test_samples = each_lpf_test[1]

            ipSrcAddr_tx = i32_to_ipv4Addr(input_val)
            ipDstAddr_tx = "10.0.0.1"
            pkt = simple_tcp_packet(ip_src=ipSrcAddr_tx,
                                    ip_dst=ipDstAddr_tx,
                                    with_tcp_chksum=False)
            lpf_idx = 25 + which_test

            print("Setting LPF at index", lpf_idx)
            self.client.lpf_set_meter_lpf(sess_hdl, dev_tgt, lpf_idx, lpf_meter_spec)
            print("Match spec:", ipDstAddr_tx)
            match_spec = meters_match_tbl_lpf_match_spec_t(ipv4Addr_to_i32(ipDstAddr_tx))
            print("Action spec", lpf_idx)
            action_spec = meters_lpf_indirect_action_spec_t(lpf_idx)
            entry_hdl = self.client.match_tbl_lpf_table_add_with_lpf_indirect(sess_hdl, dev_tgt, match_spec, action_spec)
            print("Match entry handle", entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)

            # Read it back as a sanity check
            read_lpf_meter_spec = self.client.lpf_read_meter_lpf(sess_hdl, dev_tgt, lpf_idx)
            self.assertEqual(lpf_meter_spec.gain_decay_separate_time_constant, read_lpf_meter_spec.gain_decay_separate_time_constant)
            self.assertEqual(lpf_meter_spec.output_scale_down_factor, read_lpf_meter_spec.output_scale_down_factor)
            self.assertEqual(lpf_meter_spec.lpf_type, read_lpf_meter_spec.lpf_type)
            self.assertEqual(lpf_meter_spec.is_set, read_lpf_meter_spec.is_set)
            tolerance = 0.2
            assert (abs(lpf_meter_spec.gain_time_constant - read_lpf_meter_spec.gain_time_constant) <= tolerance * lpf_meter_spec.gain_time_constant)
            assert (abs(lpf_meter_spec.decay_time_constant - read_lpf_meter_spec.decay_time_constant) <= tolerance * lpf_meter_spec.decay_time_constant)
            assert (abs(lpf_meter_spec.time_constant - read_lpf_meter_spec.time_constant) <= tolerance * lpf_meter_spec.time_constant)

            cycles_now = 0

            for j,each_sample in enumerate(lpf_test_samples):
                print("Sample", j+1, "of", len(lpf_test_samples))
                time = each_sample[0]
                output_val = each_sample[1]
                ipSrcAddr_rx = i32_to_ipv4Addr(output_val)

                exp_pkt = simple_tcp_packet(ip_src=ipSrcAddr_rx,
                                            ip_dst=ipDstAddr_tx,
                                            with_tcp_chksum=False)

                cycles_target = (int) (time * clock_speed)
                cycles_increment = cycles_target - cycles_now
                print("   Time           :", time)
                print("   Increment Value:", cycles_increment)
                sys.stdout.flush()
                # The LPF table is placed in stage 4. When we set the time in the model
                # a latency of 20 clocks is added per MAU stage. Omnet test samples do not
                # include this latency. Hence subtract that latency before setting the time.
                print("   Advance time by", cycles_increment-80)
                current_time += cycles_increment-80
                advance_model_time_by_clocks(self, sess_hdl, cycles_increment-80)
                cycles_now = cycles_now + cycles_increment - 80

                send_packet(self, port, pkt)
                try:
                    #verify_packet(self, exp_pkt, port)
                    (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=3.0 )
                    assert(rcv_pkt is not None)
                    rx_val = (b2v(rcv_pkt[26]) << 24) | (b2v(rcv_pkt[27]) << 16) | (b2v(rcv_pkt[28]) << 8) | b2v(rcv_pkt[29])
                    pass
                except AssertionError as e:
                    print("Failed to verify packet", j+1, "of", len(lpf_test_samples))
                    sys.stdout.flush()
                    self.client.match_tbl_lpf_table_delete(sess_hdl, dev_id, entry_hdl)
                    self.conn_mgr.client_cleanup(sess_hdl)
                    raise e
                delta = output_val - rx_val
                v = abs(delta) / (1.0 * output_val)
                print("   Got 0x%x expected 0x%x, off by %d %.3f" % (rx_val, output_val, delta, v * 100.0))
                self.assertLess(v, 0.10)


            print("Deleting match entry", entry_hdl)
            self.client.match_tbl_lpf_table_delete(sess_hdl, dev_id, entry_hdl)
            print("Done with pass", which_test)

        print("closing session")
        advance_model_time_by_clocks(self, sess_hdl, 25000000)
        self.conn_mgr.client_cleanup(sess_hdl)

class TestTCAMLpfIndirect(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])
    """ LPF test with Exact match table """
    def runTest(self):

        port = swports[1]
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        lpf_test0 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 38789),
                             (0.000001437466, 50304),
                             (0.000001639913, 64409),
                             (0.000002570436, 60256),
                             (0.000004805749, 42596),
                             (0.000005406021, 46622),
                             (0.000005434843, 56423),
                             (0.000006052835, 55264),
                             (0.000006059165, 74407)]]

        lpf_test1 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 20000),
                             (0.000001437466, 20000),
                             (0.000001639913, 20000),
                             (0.000002570436, 20000),
                             (0.000004805749, 20000),
                             (0.000005406021, 20000),
                             (0.000005434843, 20000),
                             (0.000006052835, 20000),
                             (0.000006059165, 20000)]]

        lpf_test2 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 32500),
                             (0.000001437466, 24315),
                             (0.000001639913, 29117),
                             (0.000002570436, 20510),
                             (0.000004805749, 20006),
                             (0.000005406021, 20351),
                             (0.000005434843, 22701),
                             (0.000006052835, 20398),
                             (0.000006059165, 38008)]]

        lpf_meter_spec0 = meters_lpf_spec_t(False, 0, 0, 1638.4, 0, 0, True)
        lpf_meter_spec1 = meters_lpf_spec_t(False, 0, 0, 1.6, 0, 0, True)
        lpf_meter_spec2 = meters_lpf_spec_t(False, 0, 0, 204.8, 0, 0, True)

        lpf_test_matrix = [lpf_test0, lpf_test1, lpf_test2]
        lpf_spec_matrix = [lpf_meter_spec0, lpf_meter_spec1, lpf_meter_spec2]

        lpf_test = list(zip([0,1,2], lpf_test_matrix, lpf_spec_matrix))
        for which_test, each_lpf_test, lpf_meter_spec in lpf_test:
            self.client.test_select_set_default_action_GetTimeForLPFTest(sess_hdl, dev_tgt)
            self.conn_mgr.complete_operations(sess_hdl)
            send_packet(self, port, simple_ip_packet())
            try:
                (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=3.0 )
                assert(rcv_pkt is not None)
                pass
            except AssertionError as e:
                self.conn_mgr.client_cleanup(sess_hdl)
                raise e
            rx_ts = 0
            for b in range(6):
                rx_ts = (rx_ts << 8) | b2v(rcv_pkt[b])

            print("Starting test", which_test, "at time", hex(rx_ts))
            current_time = rx_ts
            tgt_time = (1 + (rx_ts // 2**28)) * 2**28
            print("Advancing time to", hex(tgt_time))
            advance_model_time_by_clocks(self, sess_hdl, tgt_time - current_time)
            current_time += tgt_time - current_time
            sys.stdout.flush()

            self.client.test_select_set_default_action_TCAMLpfIndirect(sess_hdl, dev_tgt)

            time = 0
            input_val = each_lpf_test[0]
            lpf_test_samples = each_lpf_test[1]

            ipSrcAddr_tx = i32_to_ipv4Addr(input_val)
            ipDstAddr_tx = "10.0.0.1"
            pkt = simple_tcp_packet(ip_src=ipSrcAddr_tx,
                                    ip_dst=ipDstAddr_tx,
                                    with_tcp_chksum=False)
            lpf_idx = 25 + which_test

            print("Setting LPF at index", lpf_idx)
            self.client.lpf_set_meter_lpf_tcam(sess_hdl, dev_tgt, lpf_idx, lpf_meter_spec)
            print("Match spec:", ipDstAddr_tx)
            match_spec = meters_match_tbl_tcam_lpf_match_spec_t(ipv4Addr_to_i32(ipDstAddr_tx), hex_to_i32(0xFFFFFFFF))
            print("Action spec", lpf_idx)
            action_spec = meters_lpf_tcam_indirect_action_spec_t(lpf_idx)
            entry_hdl = self.client.match_tbl_tcam_lpf_table_add_with_lpf_tcam_indirect(sess_hdl, dev_tgt, match_spec, 0, action_spec)
            print("Match entry handle", entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)

            # Read it back as a sanity check
            read_lpf_meter_spec = self.client.lpf_read_meter_lpf_tcam(sess_hdl, dev_tgt, lpf_idx)
            self.assertEqual(lpf_meter_spec.gain_decay_separate_time_constant, read_lpf_meter_spec.gain_decay_separate_time_constant)
            self.assertEqual(lpf_meter_spec.output_scale_down_factor, read_lpf_meter_spec.output_scale_down_factor)
            self.assertEqual(lpf_meter_spec.lpf_type, read_lpf_meter_spec.lpf_type)
            self.assertEqual(lpf_meter_spec.is_set, read_lpf_meter_spec.is_set)
            tolerance = 0.2
            assert (abs(lpf_meter_spec.gain_time_constant - read_lpf_meter_spec.gain_time_constant) <= tolerance * lpf_meter_spec.gain_time_constant)
            assert (abs(lpf_meter_spec.decay_time_constant - read_lpf_meter_spec.decay_time_constant) <= tolerance * lpf_meter_spec.decay_time_constant)
            assert (abs(lpf_meter_spec.time_constant - read_lpf_meter_spec.time_constant) <= tolerance * lpf_meter_spec.time_constant)

            cycles_now = 0

            for j,each_sample in enumerate(lpf_test_samples):
                print("Sample", j+1, "of", len(lpf_test_samples))
                time = each_sample[0]
                output_val = each_sample[1]
                ipSrcAddr_rx = i32_to_ipv4Addr(output_val)

                exp_pkt = simple_tcp_packet(ip_src=ipSrcAddr_rx,
                                            ip_dst=ipDstAddr_tx,
                                            with_tcp_chksum=False)

                cycles_target = (int) (time * clock_speed)
                cycles_increment = cycles_target - cycles_now
                print("   Time           :", time)
                print("   Increment Value:", cycles_increment)
                sys.stdout.flush()
                # The LPF table is placed in stage 4. When we set the time in the model
                # a latency of 20 clocks is added per MAU stage. Omnet test samples do not
                # include this latency. Hence subtract that latency before setting the time.
                print("   Advance time by", cycles_increment-80)
                current_time += cycles_increment-80
                advance_model_time_by_clocks(self, sess_hdl, cycles_increment-80)
                cycles_now = cycles_now + cycles_increment - 80

                send_packet(self, port, pkt)
                try:
                    #verify_packet(self, exp_pkt, port)
                    (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=3.0 )
                    assert(rcv_pkt is not None)
                    rx_val = (b2v(rcv_pkt[26]) << 24) | (b2v(rcv_pkt[27]) << 16) | (b2v(rcv_pkt[28]) << 8) | b2v(rcv_pkt[29])
                    pass
                except AssertionError as e:
                    print("Failed to verify packet", j+1, "of", len(lpf_test_samples))
                    sys.stdout.flush()
                    self.client.match_tbl_tcam_lpf_table_delete(sess_hdl, dev_id, entry_hdl)
                    self.conn_mgr.client_cleanup(sess_hdl)
                    raise e
                delta = output_val - rx_val
                v = abs(delta) / (1.0 * output_val)
                print("   Got 0x%x expected 0x%x, off by %d %.3f" % (rx_val, output_val, delta, v * 100.0))
                self.assertLess(v, 0.10)

            print("Deleting match entry", entry_hdl)
            self.client.match_tbl_tcam_lpf_table_delete(sess_hdl, dev_id, entry_hdl)
            print("Done with pass", which_test)

        print("closing session")
        advance_model_time_by_clocks(self, sess_hdl, 25000000)
        self.conn_mgr.client_cleanup(sess_hdl)



class TestExmLpfdirect(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])
    """ LPF test with Exact match table """
    def runTest(self):

        port = swports[1]
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        # [ input_val, [ (time, expected output value) ] ]
        lpf_test0 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 20005),
                             (0.000001437466, 20000),
                             (0.000001639913, 20000),
                             (0.000002570436, 20000),
                             (0.000004805749, 20000),
                             (0.000005406021, 20000),
                             (0.000005434843, 20000),
                             (0.000006052835, 20000),
                             (0.000006059165, 20005)]]

        lpf_test1 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 39844),
                             (0.000001437466, 58618),
                             (0.000001639913, 77710),
                             (0.000002570436, 93005),
                             (0.000004805749, 102106),
                             (0.000005406021, 115924),
                             (0.000005434843, 132358),
                             (0.000006052835, 144344),
                             (0.000006059165, 164344)]]

        lpf_meter_spec0 = meters_lpf_spec_t(False, 0, 0, 12.8, 0, 0, True)
        lpf_meter_spec1 = meters_lpf_spec_t(False, 0, 0, 13107.2, 0, 0, True)

        lpf_test_matrix = [lpf_test0, lpf_test1]
        lpf_spec_matrix = [lpf_meter_spec0, lpf_meter_spec1]

        lpf_test = list(zip([0,1,2], lpf_test_matrix, lpf_spec_matrix))
        for which_test, each_lpf_test, lpf_meter_spec in lpf_test:
            self.client.test_select_set_default_action_GetTimeForLPFTest(sess_hdl, dev_tgt)
            self.conn_mgr.complete_operations(sess_hdl)
            send_packet(self, port, simple_ip_packet())
            try:
                (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=3.0 )
                assert(rcv_pkt is not None)
                pass
            except AssertionError as e:
                self.conn_mgr.client_cleanup(sess_hdl)
                raise e
            rx_ts = 0
            for b in range(6):
                rx_ts = (rx_ts << 8) | b2v(rcv_pkt[b])

            print("Starting test", which_test, "at time", hex(rx_ts))
            current_time = rx_ts
            tgt_time = (1 + (rx_ts // 2**28)) * 2**28
            print("Advancing time to", hex(tgt_time))
            advance_model_time_by_clocks(self, sess_hdl, tgt_time - current_time)
            current_time += tgt_time - current_time
            sys.stdout.flush()

            self.client.test_select_set_default_action_ExmLpfDirect(sess_hdl, dev_tgt)

            time = 0
            input_val = each_lpf_test[0]
            lpf_test_samples = each_lpf_test[1]

            ipSrcAddr_tx = i32_to_ipv4Addr(input_val)
            ipDstAddr_tx = "10.0.0.1"
            pkt = simple_tcp_packet(ip_src=ipSrcAddr_tx,
                                    ip_dst=ipDstAddr_tx,
                                    with_tcp_chksum=False)

            print("Match spec:", ipDstAddr_tx)
            match_spec = meters_match_tbl_lpf_direct_match_spec_t(ipv4Addr_to_i32(ipDstAddr_tx))
            entry_hdl = self.client.match_tbl_lpf_direct_table_add_with_direct_lpf(sess_hdl, dev_tgt, match_spec, lpf_meter_spec)
            print("Match entry handle", entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)

            # Read it back as a sanity check
            read_lpf_meter_spec = self.client.lpf_read_meter_lpf_direct(sess_hdl, dev_tgt, entry_hdl)
            self.assertEqual(lpf_meter_spec.gain_decay_separate_time_constant, read_lpf_meter_spec.gain_decay_separate_time_constant)
            self.assertEqual(lpf_meter_spec.output_scale_down_factor, read_lpf_meter_spec.output_scale_down_factor)
            self.assertEqual(lpf_meter_spec.lpf_type, read_lpf_meter_spec.lpf_type)
            self.assertEqual(lpf_meter_spec.is_set, read_lpf_meter_spec.is_set)
            tolerance = 0.2
            assert (abs(lpf_meter_spec.gain_time_constant - read_lpf_meter_spec.gain_time_constant) <= tolerance * lpf_meter_spec.gain_time_constant)
            assert (abs(lpf_meter_spec.decay_time_constant - read_lpf_meter_spec.decay_time_constant) <= tolerance * lpf_meter_spec.decay_time_constant)
            assert (abs(lpf_meter_spec.time_constant - read_lpf_meter_spec.time_constant) <= tolerance * lpf_meter_spec.time_constant)

            cycles_now = 0

            for j,each_sample in enumerate(lpf_test_samples):
                print("Sample", j+1, "of", len(lpf_test_samples))
                time = each_sample[0]
                output_val = each_sample[1]
                ipSrcAddr_rx = i32_to_ipv4Addr(output_val)

                exp_pkt = simple_tcp_packet(ip_src=ipSrcAddr_rx,
                                            ip_dst=ipDstAddr_tx,
                                            with_tcp_chksum=False)

                cycles_target = (int) (time * clock_speed)
                cycles_increment = cycles_target - cycles_now
                print("   Time           :", time)
                print("   Increment Value:", cycles_increment)
                sys.stdout.flush()
                # The LPF table is placed in stage 4. When we set the time in the model
                # a latency of 20 clocks is added per MAU stage. Omnet test samples do not
                # include this latency. Hence subtract that latency before setting the time.
                print("   Advance time by", cycles_increment-80)
                current_time += cycles_increment-80
                advance_model_time_by_clocks(self, sess_hdl, cycles_increment-80)
                cycles_now = cycles_now + cycles_increment - 80

                send_packet(self, port, pkt)
                try:
                    #verify_packet(self, exp_pkt, port)
                    (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=3.0 )
                    assert(rcv_pkt is not None)
                    rx_val = (b2v(rcv_pkt[26]) << 24) | (b2v(rcv_pkt[27]) << 16) | (b2v(rcv_pkt[28]) << 8) | b2v(rcv_pkt[29])
                    pass
                except AssertionError as e:
                    print("Failed to verify packet", j+1, "of", len(lpf_test_samples))
                    sys.stdout.flush()
                    self.client.match_tbl_lpf_direct_table_delete(sess_hdl, dev_id, entry_hdl)
                    self.conn_mgr.client_cleanup(sess_hdl)
                    raise e
                delta = output_val - rx_val
                v = abs(delta) / (1.0 * output_val)
                print("   Got 0x%x expected 0x%x, off by %d %.3f" % (rx_val, output_val, delta, v * 100.0))
                self.assertLess(v, 0.10)


            print("Deleting match entry", entry_hdl)
            self.client.match_tbl_lpf_direct_table_delete(sess_hdl, dev_id, entry_hdl)
            print("Done with pass", which_test)

        print("closing session")
        advance_model_time_by_clocks(self, sess_hdl, 25000000)
        self.conn_mgr.client_cleanup(sess_hdl)


class TestTCAMLpfdirect(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])
    """ LPF test with Exact match table """
    def runTest(self):

        port = swports[1]
        sess_hdl = self.conn_mgr.client_init()
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        lpf_test0 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 37656),
                             (0.000001437466, 43535),
                             (0.000001639913, 54012),
                             (0.000002570436, 40254),
                             (0.000004805749, 25345),
                             (0.000005406021, 29504),
                             (0.000005434843, 38440),
                             (0.000006052835, 34415),
                             (0.000006059165, 53356)]]

        lpf_test1 = [20000, [(0.0000008192,   20000),
                             (0.000000934732, 35625),
                             (0.000001437466, 33359),
                             (0.000001639913, 40849),
                             (0.000002570436, 25425),
                             (0.000004805749, 20446),
                             (0.000005406021, 22714),
                             (0.000005434843, 28517),
                             (0.000006052835, 23786),
                             (0.000006059165, 42346)]]

        lpf_meter_spec0 = meters_lpf_spec_t(False, 0, 0, 819.2, 0, 0, True)
        lpf_meter_spec1 = meters_lpf_spec_t(False, 0, 0, 409.6, 0, 0, True)

        lpf_test_matrix = [lpf_test0, lpf_test1]
        lpf_spec_matrix = [lpf_meter_spec0, lpf_meter_spec1]

        lpf_test = list(zip([0,1,2], lpf_test_matrix, lpf_spec_matrix))
        for which_test, each_lpf_test, lpf_meter_spec in lpf_test:
            self.client.test_select_set_default_action_GetTimeForLPFTest(sess_hdl, dev_tgt)
            self.conn_mgr.complete_operations(sess_hdl)
            send_packet(self, port, simple_ip_packet())
            try:
                (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=3.0 )
                assert(rcv_pkt is not None)
                pass
            except AssertionError as e:
                self.conn_mgr.client_cleanup(sess_hdl)
                raise e
            rx_ts = 0
            for b in range(6):
                rx_ts = (rx_ts << 8) | b2v(rcv_pkt[b])

            print("Starting test", which_test, "at time", hex(rx_ts))
            current_time = rx_ts
            tgt_time = (1 + (rx_ts // 2**28)) * 2**28
            print("Advancing time to", hex(tgt_time))
            advance_model_time_by_clocks(self, sess_hdl, tgt_time - current_time)
            current_time += tgt_time - current_time
            sys.stdout.flush()

            self.client.test_select_set_default_action_TCAMLpfDirect(sess_hdl, dev_tgt)

            time = 0
            input_val = each_lpf_test[0]
            lpf_test_samples = each_lpf_test[1]

            ipSrcAddr_tx = i32_to_ipv4Addr(input_val)
            ipDstAddr_tx = "10.0.0.1"
            pkt = simple_tcp_packet(ip_src=ipSrcAddr_tx,
                                    ip_dst=ipDstAddr_tx,
                                    with_tcp_chksum=False)

            print("Match spec:", ipDstAddr_tx)
            match_spec = meters_match_tbl_tcam_lpf_direct_match_spec_t(ipv4Addr_to_i32(ipDstAddr_tx), hex_to_i32(0xFFFFFFFF))
            entry_hdl = self.client.match_tbl_tcam_lpf_direct_table_add_with_lpf_direct_tcam(sess_hdl, dev_tgt, match_spec, 0, lpf_meter_spec)
            print("Match entry handle", entry_hdl)
            self.conn_mgr.complete_operations(sess_hdl)

            # Read it back as a sanity check
            read_lpf_meter_spec = self.client.lpf_read_meter_lpf_tcam_direct(sess_hdl, dev_tgt, entry_hdl)
            self.assertEqual(lpf_meter_spec.gain_decay_separate_time_constant, read_lpf_meter_spec.gain_decay_separate_time_constant)
            self.assertEqual(lpf_meter_spec.output_scale_down_factor, read_lpf_meter_spec.output_scale_down_factor)
            self.assertEqual(lpf_meter_spec.lpf_type, read_lpf_meter_spec.lpf_type)
            self.assertEqual(lpf_meter_spec.is_set, read_lpf_meter_spec.is_set)
            tolerance = 0.2
            assert (abs(lpf_meter_spec.gain_time_constant - read_lpf_meter_spec.gain_time_constant) <= tolerance * lpf_meter_spec.gain_time_constant)
            assert (abs(lpf_meter_spec.decay_time_constant - read_lpf_meter_spec.decay_time_constant) <= tolerance * lpf_meter_spec.decay_time_constant)
            assert (abs(lpf_meter_spec.time_constant - read_lpf_meter_spec.time_constant) <= tolerance * lpf_meter_spec.time_constant)

            cycles_now = 0

            for j,each_sample in enumerate(lpf_test_samples):
                print("Sample", j+1, "of", len(lpf_test_samples))
                time = each_sample[0]
                output_val = each_sample[1]
                ipSrcAddr_rx = i32_to_ipv4Addr(output_val)

                exp_pkt = simple_tcp_packet(ip_src=ipSrcAddr_rx,
                                            ip_dst=ipDstAddr_tx,
                                            with_tcp_chksum=False)

                cycles_target = (int) (time * clock_speed)
                cycles_increment = cycles_target - cycles_now
                print("   Time           :", time)
                print("   Increment Value:", cycles_increment)
                sys.stdout.flush()
                # The LPF table is placed in stage 4. When we set the time in the model
                # a latency of 20 clocks is added per MAU stage. Omnet test samples do not
                # include this latency. Hence subtract that latency before setting the time.
                print("   Advance time by", cycles_increment-80)
                current_time += cycles_increment-80
                advance_model_time_by_clocks(self, sess_hdl, cycles_increment-80)
                cycles_now = cycles_now + cycles_increment - 80

                send_packet(self, port, pkt)
                try:
                    #verify_packet(self, exp_pkt, port)
                    (rcv_device, rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll( timeout=3.0 )
                    assert(rcv_pkt is not None)
                    rx_val = (b2v(rcv_pkt[26]) << 24) | (b2v(rcv_pkt[27]) << 16) | (b2v(rcv_pkt[28]) << 8) | b2v(rcv_pkt[29])
                    pass
                except AssertionError as e:
                    print("Failed to verify packet", j+1, "of", len(lpf_test_samples))
                    sys.stdout.flush()
                    self.client.match_tbl_tcam_lpf_direct_table_delete(sess_hdl, dev_id, entry_hdl)
                    self.conn_mgr.client_cleanup(sess_hdl)
                    raise e
                delta = output_val - rx_val
                v = abs(delta) / (1.0 * output_val)
                print("   Got 0x%x expected 0x%x, off by %d %.3f" % (rx_val, output_val, delta, v * 100.0))
                self.assertLess(v, 0.10)


            print("Deleting match entry", entry_hdl)
            self.client.match_tbl_tcam_lpf_direct_table_delete(sess_hdl, dev_id, entry_hdl)
            print("Done with pass", which_test)

        print("closing session")
        advance_model_time_by_clocks(self, sess_hdl, 25000000)
        self.conn_mgr.client_cleanup(sess_hdl)


class TestMeterDirectStateRestore(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Basic meter test with Exact match table, directly addressed """
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_ExmMeterDirect(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)

        sess_hdl = self.conn_mgr.client_init()

        port = swports[1]
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        global color_green
        global color_yellow
        global color_red

        match_spec = meters_meter_tbl_direct_match_spec_t(ipv4Addr_to_i32("10.1.1.10"))


        # CIR of 1Mbps, with burst size of 500 bytes (5 packets worth), PIR of 2Mbps with burstsize of 1000 bytes (10 packets worth)
        meter_1_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, False)
        entry_hdl = self.client.meter_tbl_direct_table_add_with_nop(sess_hdl, dev_tgt, match_spec, meter_1_spec)

        print("Adding entry")

        advance_model_time_by_clocks(self, sess_hdl, 1000000)

        # Log and restore the state
        self.conn_mgr.log_state(dev_id, "test_log_file".encode())
        print("State logged, cleaning up")
        self.client.meter_tbl_direct_table_delete(sess_hdl,
                                                  dev_id,
                                                  entry_hdl)
        self.conn_mgr.complete_operations(sess_hdl)
        self.conn_mgr.client_cleanup(sess_hdl)

        sess_hdl = self.conn_mgr.client_init()
        print("Restoring state")
        self.conn_mgr.restore_state(dev_id, "test_log_file".encode())
        print("State restored")

        # Validate meter entry
        # Since meter values are stored in a different format in hw (mantissa/exponent),
        # the values read may not exactly match the user's values.
        tolerance = 0.2
        read_meter_spec = self.client.meter_read_meter_1(sess_hdl, dev_tgt, entry_hdl)
        assert (abs(read_meter_spec.cburst_kbits - meter_1_spec.cburst_kbits) < tolerance * meter_1_spec.cburst_kbits)
        assert (abs(read_meter_spec.pburst_kbits - meter_1_spec.pburst_kbits) < tolerance * meter_1_spec.pburst_kbits)
        assert (abs(read_meter_spec.cir_kbps - meter_1_spec.cir_kbps) < tolerance * meter_1_spec.cir_kbps)
        assert (abs(read_meter_spec.pir_kbps - meter_1_spec.pir_kbps) < tolerance * meter_1_spec.pir_kbps)
        assert (read_meter_spec.color_aware == False)

        pkt = simple_tcp_packet(pktlen=96,
                                ip_dst='10.1.1.10',
                                with_tcp_chksum=False)

        green_pkt = simple_tcp_packet(pktlen=96,
                                      ip_dst='10.1.1.10',
                                      ip_tos=color_green,
                                      with_tcp_chksum=False)
        yellow_pkt = simple_tcp_packet(pktlen=96,
                                       ip_dst='10.1.1.10',
                                       ip_tos=color_yellow,
                                       with_tcp_chksum=False)
        red_pkt = simple_tcp_packet(pktlen=96,
                                    ip_dst='10.1.1.10',
                                    ip_tos=color_red,
                                    with_tcp_chksum=False)

        try:
            # 10 packets would exhaust the committed burst size, but since color
            # based meters operate on EOP, the color output on the
            # packet will be seen only on the next packet. i,e.. at the
            # end of processing 11 packets, color would be updated, and
            # on the 12th packet, color will take effect on the packet

            for i in range (0, 11):
                send_packet(self, port, pkt)
                try:
                    verify_packet(self, green_pkt, port)
                except AssertionError as e:
                    print("Failed to verify green packet", i+1, "of", len(list(range(0,11))))
                    sys.stdout.flush()
                    raise e
            # Now send one more packet to see the color change to yellow

            # Set the time to something > than the mapram color write latency (which is 13 clock cycles currently in the model)
            # This is so that the mapram color writes that are queued get executed.

            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port, pkt)
            verify_packet(self, yellow_pkt, port)

            # So far we have sent 12 packets. Send 9 more to exhaust the
            # peak burst size and change the color.
            for i in range (0, 9):
                send_packet(self, port, pkt)
                try:
                    verify_packet(self, yellow_pkt, port)
                except AssertionError as e:
                    print("Failed to verify yellow packet", i+1, "of", len(list(range(0,9))))
                    sys.stdout.flush()
                    raise e

            # Now send one more packet to see the packet color change to red

            # Bump up the time so that the map-ram color write gets executed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port, pkt)
            verify_packet(self, red_pkt, port)


            # Now set the time to a value such that both the buckets get
            # get filled on the next execution of the meter and subsequent
            # packet gets colored green

            advance_model_time_by_clocks(self, sess_hdl, 25000000)

            # Now send a packet, so that the color gets updated. However
            # the packet color still remains red.

            send_packet(self, port, pkt)
            verify_packet(self, red_pkt, port)

            # Bump up the time so that the map-ram color write gets executed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            # Now, this packet should have a color of green

            send_packet(self, port, pkt)
            verify_packets(self, green_pkt, [port])

        finally:
            print("Deleting entry")
            self.client.meter_tbl_direct_table_delete(sess_hdl,
                                                      dev_id,
                                                      entry_hdl)
            print("closing session")
            advance_model_time_by_clocks(self, sess_hdl, 25000000)
            self.conn_mgr.client_cleanup(sess_hdl)

class TestMeterIndirectStateRestore(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Basic Color aware meter test with Exact match table, indirectly addressed """
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_ExmMeterColorAwareIndirect(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)

        sess_hdl = self.conn_mgr.client_init()

        port = swports[1]
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        global color_green
        global color_yellow
        global color_red

        ipdstAddr = "10.1.1.1"

        ipdstAddr1 = "10.2.2.2"

        match_spec_color_aware = meters_meter_tbl_color_aware_indirect_match_spec_t(ipv4Addr_to_i32(ipdstAddr))

        match_spec_color_unaware = meters_meter_tbl_color_aware_indirect_match_spec_t(ipv4Addr_to_i32(ipdstAddr1))

        meter_idx_color_aware = 52
        meter_idx_color_unaware = 33

        action_spec_color_aware = meters_meter_action_color_aware_action_spec_t(meter_idx_color_aware)

        action_spec_color_unaware = meters_meter_action_color_aware_action_spec_t(meter_idx_color_unaware)

        # Install a color-aware meter entry
        meter_2_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, True)
        self.client.meter_set_meter_2(sess_hdl, dev_tgt, meter_idx_color_aware, meter_2_spec)
        # Install a color-unaware meter entry
        meter_2_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, False)
        self.client.meter_set_meter_2(sess_hdl, dev_tgt, meter_idx_color_unaware, meter_2_spec)

        # Add two match entries, one pointing to color-aware meter and the other pointing to a
        # color unaware meter
        entry_hdl_color_aware = self.client.meter_tbl_color_aware_indirect_table_add_with_meter_action_color_aware(sess_hdl, dev_tgt, match_spec_color_aware, action_spec_color_aware)
        entry_hdl_color_unaware = self.client.meter_tbl_color_aware_indirect_table_add_with_meter_action_color_unaware(sess_hdl, dev_tgt, match_spec_color_unaware, action_spec_color_unaware)

        # Log and restore the state
        self.conn_mgr.log_state(dev_id, "test_log_file".encode())
        print("State logged, cleaning up")
        self.client.meter_tbl_color_aware_indirect_table_delete(sess_hdl,
                                                                dev_id,
                                                                entry_hdl_color_aware)

        self.client.meter_tbl_color_aware_indirect_table_delete(sess_hdl,
                                                                dev_id,
                                                                entry_hdl_color_unaware)

        self.conn_mgr.complete_operations(sess_hdl)
        self.conn_mgr.client_cleanup(sess_hdl)

        sess_hdl = self.conn_mgr.client_init()
        print("Restoring state")
        self.conn_mgr.restore_state(dev_id, "test_log_file".encode())
        print("State restored")

        # Validate meter entries
        # Since meter values are stored in a different format in hw (mantissa/exponent),
        # the values read may not exactly match the user's values.
        tolerance = 0.2
        read_meter_spec = self.client.meter_read_meter_2(sess_hdl, dev_tgt, meter_idx_color_aware)
        assert (abs(read_meter_spec.cburst_kbits - meter_2_spec.cburst_kbits) < tolerance * meter_2_spec.cburst_kbits)
        assert (abs(read_meter_spec.pburst_kbits - meter_2_spec.pburst_kbits) < tolerance * meter_2_spec.pburst_kbits)
        assert (abs(read_meter_spec.cir_kbps - meter_2_spec.cir_kbps) < tolerance * meter_2_spec.cir_kbps)
        assert (abs(read_meter_spec.pir_kbps - meter_2_spec.pir_kbps) < tolerance * meter_2_spec.pir_kbps)
        assert (read_meter_spec.color_aware == True)

        read_meter_spec = self.client.meter_read_meter_2(sess_hdl, dev_tgt, meter_idx_color_unaware)
        assert (abs(read_meter_spec.cburst_kbits - meter_2_spec.cburst_kbits) < tolerance * meter_2_spec.cburst_kbits)
        assert (abs(read_meter_spec.pburst_kbits - meter_2_spec.pburst_kbits) < tolerance * meter_2_spec.pburst_kbits)
        assert (abs(read_meter_spec.cir_kbps - meter_2_spec.cir_kbps) < tolerance * meter_2_spec.cir_kbps)
        assert (abs(read_meter_spec.pir_kbps - meter_2_spec.pir_kbps) < tolerance * meter_2_spec.pir_kbps)
        assert (read_meter_spec.color_aware == False)

        advance_model_time_by_clocks(self, sess_hdl, 1000000)

        # Now send a packet which would hit the match-entry which is attached to a
        # a color aware meter, pre color the packet to red and verify the color is
        # not upgraded.

        pkt_c_aware = simple_tcp_packet(pktlen=96,
                                        ip_dst=ipdstAddr,
                                        ip_tos=color_red,
                                        with_tcp_chksum=False)
        exp_c_aware_g = simple_tcp_packet(pktlen=96,
                                          ip_dst=ipdstAddr,
                                          ip_tos=color_green,
                                          with_tcp_chksum=False)
        exp_c_aware_y = simple_tcp_packet(pktlen=96,
                                          ip_dst=ipdstAddr,
                                          ip_tos=color_yellow,
                                          with_tcp_chksum=False)
        exp_c_aware_r = simple_tcp_packet(pktlen=96,
                                          ip_dst=ipdstAddr,
                                          ip_tos=color_red,
                                          with_tcp_chksum=False)
        pkt_c_unaware = simple_tcp_packet(pktlen=96,
                                          ip_dst=ipdstAddr1,
                                          with_tcp_chksum=False)
        exp_c_unaware_g = simple_tcp_packet(pktlen=96,
                                            ip_dst=ipdstAddr1,
                                            ip_tos=color_green,
                                            with_tcp_chksum=False)
        exp_c_unaware_y = simple_tcp_packet(pktlen=96,
                                            ip_dst=ipdstAddr1,
                                            ip_tos=color_yellow,
                                            with_tcp_chksum=False)
        exp_c_unaware_r = simple_tcp_packet(pktlen=96,
                                            ip_dst=ipdstAddr1,
                                            ip_tos=color_red,
                                            with_tcp_chksum=False)

        try:
            send_packet(self, port, pkt_c_aware)
            verify_packet(self, exp_c_aware_r, port)

            # Set the time to something > than the mapram color write latency (which is 13 clock cycles currently in the model)
            # This is so that the mapram color writes that are queued get executed.

            advance_model_time_by_clocks(self, sess_hdl, 20)

            # Now send another packet.

            send_packet(self, port, pkt_c_aware)
            verify_packet(self, exp_c_aware_r, port)

            # Now send packets for color unaware entry

            # 10 packets would exhaust the committed burst size, but since color
            # based meters operate on EOP, the color output on the
            # packet will be seen only on the next packet. i,e.. at the
            # end of processing 11 packets, color would be updated, and
            # on the 12th packet, color will take effect on the packet

            for i in range (0, 11):
                send_packet(self, port, pkt_c_unaware)
                try:
                    verify_packet(self, exp_c_unaware_g, port)
                except AssertionError as e:
                    print("Failed to verify green packet", i+1, "of", len(list(range(0,11))))
                    sys.stdout.flush()
                    raise e

            # Now send one more packet to see the color change to yellow

            # Set the time to something > than the mapram color write latency (which is 13 clock cycles currently in the model)
            # This is so that the mapram color writes that are queued get executed.

            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port, pkt_c_unaware)
            verify_packet(self, exp_c_unaware_y, port)

            # So far we have sent 12 packets. Send 9 more to exhaust the
            # peak burst size and change the color.
            for i in range (0, 9):
                send_packet(self, port, pkt_c_unaware)
                try:
                    verify_packet(self, exp_c_unaware_y, port)
                except AssertionError as e:
                    print("Failed to verify yellow packet", i+1, "of", len(list(range(0,9))))
                    sys.stdout.flush()
                    raise e

            # Now send one more packet to see the packet color change to red

            # Bump up the time so that the map-ram color write gets executed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port, pkt_c_unaware)
            verify_packet(self, exp_c_unaware_r, port)


            # Now set the time to a value such that both the buckets get
            # get filled on the next execution of the meter and subsequent
            # packet gets colored green

            advance_model_time_by_clocks(self, sess_hdl, 25000000)

            # Now send a packet, so that the color gets updated. However
            # the packet color still remains red.

            send_packet(self, port, pkt_c_unaware)
            verify_packet(self, exp_c_unaware_r, port)

            advance_model_time_by_clocks(self, sess_hdl, 20)

            # Now, this packet should have a color of green

            send_packet(self, port, pkt_c_unaware)
            verify_packets(self, exp_c_unaware_g, [port])

        finally:
            print("Deleting entries")
            self.client.meter_tbl_color_aware_indirect_table_delete(sess_hdl,
                                                                    dev_id,
                                                                    entry_hdl_color_aware)

            self.client.meter_tbl_color_aware_indirect_table_delete(sess_hdl,
                                                                    dev_id,
                                                                    entry_hdl_color_unaware)

            advance_model_time_by_clocks(self, sess_hdl, 25000000)
            self.conn_mgr.client_cleanup(sess_hdl)

class TestMeterScopes(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Basic meter test with Exact match table, indirectly addressed """
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_ExmMeterIndirect(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)

        num_pipes = int(test_param_get('num_pipes'))
        if (num_pipes < 4):
            return

        if (len(swports_0) < 1) or (len(swports_2) < 1):
            print("Skipping test as pipe0 and pipe2 do not have a port")
            return

        # Exm indirect meter
        sess_hdl = self.conn_mgr.client_init()
        print("PIPE_MGR gave me that session handle:", sess_hdl)

        port_0 = swports_0[0]
        port_2 = swports_2[0]
        meter_idx = 6
        pipe_0 = 0
        pipe_2 = 2
        dev_tgt_0 = DevTarget_t(dev_id, pipe_0)
        dev_tgt_2 = DevTarget_t(dev_id, pipe_2)

        global color_green
        global color_yellow
        global color_red

        # Reset the default entry
        dev_tgt_all = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
        self.client.meter_tbl_table_reset_default_entry(sess_hdl, dev_tgt_all)

        #Set the scopes
        prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
        prop_val = tbl_property_value_t.ENTRY_SCOPE_USER_DEFINED
        # pipes 0,1 in one scope, pipes 2,3 in other scope
        prop_args = 0x0c03
        self.client.meter_tbl_set_property(sess_hdl, dev_id, prop, prop_val, prop_args)


        print("Adding entry in first scope (pipes 0,1) ")
        match_spec = meters_meter_tbl_match_spec_t(ipv4Addr_to_i32("30.1.1.10"))
        action_spec = meters_meter_action_action_spec_t(meter_idx)

        meter_0_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, False)
        self.client.meter_set_meter_0(sess_hdl, dev_tgt_0, meter_idx, meter_0_spec)
        entry_hdl_0 = self.client.meter_tbl_table_add_with_meter_action(sess_hdl, dev_tgt_0, match_spec, action_spec)

        print("Adding entry in second scope (pipes 2,3) ")
        match_spec = meters_meter_tbl_match_spec_t(ipv4Addr_to_i32("20.3.3.3"))
        action_spec = meters_meter_action_action_spec_t(meter_idx)

        meter_2_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, False)
        self.client.meter_set_meter_0(sess_hdl, dev_tgt_2, meter_idx, meter_2_spec)
        entry_hdl_2 = self.client.meter_tbl_table_add_with_meter_action(sess_hdl, dev_tgt_2, match_spec, action_spec)


        advance_model_time_by_clocks(self, sess_hdl, 1000000)

        pkt = simple_tcp_packet(ip_dst='30.1.1.10',
                                with_tcp_chksum=False)

        exp_pkt = simple_tcp_packet(ip_dst='30.1.1.10',
                                    with_tcp_chksum=False)

        pkt_2 = simple_tcp_packet(ip_dst='20.3.3.3',
                                  with_tcp_chksum=False)

        exp_pkt_2 = simple_tcp_packet(ip_dst='20.3.3.3',
                                      with_tcp_chksum=False)


        try:
            # 10 packets would exhaust the committed burst size
            print("Testing first scope ")
            for i in range (0, 11):
                send_packet(self, port_0, pkt)
                verify_packet(self, exp_pkt, port_0)

            # Now send one more packet to see the color change to yellow
            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port_0, pkt)
            exp_pkt = simple_tcp_packet(ip_dst='30.1.1.10',
                                        ip_tos=color_yellow,
                                        with_tcp_chksum=False)

            verify_packet(self, exp_pkt, port_0)

            print("Testing second scope ")
            for i in range (0, 11):
                send_packet(self, port_2, pkt_2)
                verify_packet(self, exp_pkt_2, port_2)

            # Now send one more packet to see the color change to yellow
            advance_model_time_by_clocks(self, sess_hdl, 20)

            send_packet(self, port_2, pkt_2)
            exp_pkt_2 = simple_tcp_packet(ip_dst='20.3.3.3',
                                          ip_tos=color_yellow,
                                          with_tcp_chksum=False)

            verify_packets(self, exp_pkt_2, [port_2])

        finally:
            print("Deleting entry")
            self.client.meter_tbl_table_delete(sess_hdl,
                                               dev_id,
                                               entry_hdl_0)
            self.client.meter_tbl_table_delete(sess_hdl,
                                               dev_id,
                                               entry_hdl_2)
            self.conn_mgr.complete_operations(sess_hdl)
            prop = tbl_property_t.TBL_PROP_TBL_ENTRY_SCOPE
            prop_val = tbl_property_value_t.ENTRY_SCOPE_ALL_PIPELINES
            self.client.meter_tbl_set_property(sess_hdl, dev_id, prop, prop_val, 0)

            print("closing session")
            advance_model_time_by_clocks(self, sess_hdl, 25000000)
            self.conn_mgr.client_cleanup(sess_hdl)

class TestByteAdj(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

    """ Basic meter test with Exact match table, indirectly addressed """
    def runTest(self):
        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_ExmMeterIndirect(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        self.conn_mgr.client_cleanup(shdl)

        sess_hdl = self.conn_mgr.client_init()

        port = swports[1]
        meter_idx = 5
        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        global color_green
        global color_yellow
        global color_red

        match_spec = meters_meter_tbl_match_spec_t(ipv4Addr_to_i32("10.1.1.10"))

        action_spec = meters_meter_action_action_spec_t(meter_idx)

        meter_0_spec = meters_bytes_meter_spec_t(1000, 8, 2000, 16, False)
        self.client.meter_set_meter_0(sess_hdl, dev_tgt, meter_idx, meter_0_spec)
        print("Adding entry")

        entry_hdl = self.client.meter_tbl_table_add_with_meter_action(sess_hdl, dev_tgt, match_spec, action_spec)

        pkt = simple_tcp_packet(pktlen=96+10,
                                ip_dst='10.1.1.10',
                                with_tcp_chksum=False)

        green_pkt = simple_tcp_packet(pktlen=96+10,
                                      ip_dst='10.1.1.10',
                                      ip_tos=color_green,
                                      with_tcp_chksum=False)
        yellow_pkt = simple_tcp_packet(pktlen=96+10,
                                       ip_dst='10.1.1.10',
                                       ip_tos=color_yellow,
                                       with_tcp_chksum=False)
        red_pkt = simple_tcp_packet(pktlen=96+10,
                                    ip_dst='10.1.1.10',
                                    ip_tos=color_red,
                                    with_tcp_chksum=False)

        bytecount_origin = self.client.meter_bytecount_adjust_get_meter_0(sess_hdl, dev_tgt)
        try:
            print("test set and get bytecount_adjust")
            self.client.meter_bytecount_adjust_set_meter_0(sess_hdl, dev_tgt, -50)
            bytecount = self.client.meter_bytecount_adjust_get_meter_0(sess_hdl, dev_tgt)
            if (bytecount != -50):
                print("test fails, expected value is -50, real value is ",bytecount)
                assert(bytecount == -50)
            self.client.meter_bytecount_adjust_set_meter_0(sess_hdl, dev_tgt, 50)
            bytecount = self.client.meter_bytecount_adjust_get_meter_0(sess_hdl, dev_tgt)
            if (bytecount != 50):
                print("test fails, expected value is 50, real value is ",bytecount)
                assert(bytecount == 50)
            self.client.meter_bytecount_adjust_set_meter_0(sess_hdl, dev_tgt, 0)
            bytecount = self.client.meter_bytecount_adjust_get_meter_0(sess_hdl, dev_tgt)
            if (bytecount != 0):
                print("test fails, expected value is 0, real value is ",bytecount)
                assert(bytecount == 0)

            print("try to configure to one pipe, expect fail")
            dev_tgt1 = DevTarget_t(dev_id, hex_to_i16(0x1))
            try:
                self.client.meter_bytecount_adjust_set_meter_0(sess_hdl, dev_tgt1, -50)
                assert(False)
            except:
                pass

            print("sending pkts to verify byte adjust behavior")
            self.client.meter_bytecount_adjust_set_meter_0(sess_hdl, dev_tgt, bytecount_origin-10)
            bytecount = self.client.meter_bytecount_adjust_get_meter_0(sess_hdl, dev_tgt)
            if (bytecount != -10):
                print("test fails, expected value is -10, real value is ",bytecount)
                assert(bytecount == -10)

            self.conn_mgr.complete_operations(sess_hdl)
            # 10 packets would exhaust the committed burst size, but since color
            # based meters operate on EOP, the color output on the
            # packet will be seen only on the next packet. i,e.. at the
            # end of processing 11 packets, color would be updated, and
            # on the 12th packet, color will take effect on the packet

            print("Sending 11 packets, all should be green")
            sys.stdout.flush()
            for i in range (0, 11):
                send_packet(self, port, pkt)
                try:
                    verify_packet(self, green_pkt, port)
                except AssertionError as e:
                    print("Failed to verify packet", i+1, "of", len(list(range(0,11))))
                    sys.stdout.flush()
                    raise e

            # Now send one more packet to see the color change to yellow

            # Set the time to something > than the mapram color write latency (which is 13 clock cycles currently in the model)
            # This is so that the mapram color writes that are queued get executed.
            print("Advancing model time to allow the color mapram to be updated")
            sys.stdout.flush()
            advance_model_time_by_clocks(self, sess_hdl, 20)

            print("Sending one more packet, it should be marked yellow")
            sys.stdout.flush()
            send_packet(self, port, pkt)
            verify_packet(self, yellow_pkt, port)

            # So far we have sent 12 packets. Send 9 more to exhaust the
            # peak burst size and change the color.
            print("Sending 9 packets, all should be yellow")
            sys.stdout.flush()
            for i in range (0, 9):
                send_packet(self, port, pkt)
                try:
                    verify_packet(self, yellow_pkt, port)
                except AssertionError as e:
                    print("Failed to verify packet", i+1, "of", len(list(range(0,9))))
                    sys.stdout.flush()
                    raise e

            # Bump up the time so that the map-ram color write gets executed
            advance_model_time_by_clocks(self, sess_hdl, 20)

            print("Sending one more packet, it should be marked red")
            sys.stdout.flush()
            send_packet(self, port, pkt)
            verify_packet(self, red_pkt, port)


            # Now set the time to a value such that both the buckets get
            # get filled on the next execution of the meter and subsequent
            # packet gets colored green

            print("Advancing time to refill the meter bucket")
            sys.stdout.flush()
            advance_model_time_by_clocks(self, sess_hdl, 25000000)

            # Now send a packet, so that the color gets updated. However
            # the packet color still remains red.

            print("Sending one more packet, it should be marked red")
            sys.stdout.flush()
            send_packet(self, port, pkt)
            verify_packet(self, red_pkt, port)

            print("Advancing model time to allow the color mapram to be updated")
            sys.stdout.flush()
            advance_model_time_by_clocks(self, sess_hdl, 20)

            # Now, this packet should have a color of green

            print("Sending one more packet, it should be marked green")
            sys.stdout.flush()
            send_packet(self, port, pkt)
            verify_packets(self, green_pkt, [port])

        finally:
            print("Deleting entry")
            self.client.meter_tbl_table_delete(sess_hdl,
                                               dev_id,
                                               entry_hdl)
            print("Reset byte count adjust")
            self.client.meter_bytecount_adjust_set_meter_0(sess_hdl, dev_tgt, bytecount_origin)

            advance_model_time_by_clocks(self, sess_hdl, 25000000)
            self.conn_mgr.client_cleanup(sess_hdl)
