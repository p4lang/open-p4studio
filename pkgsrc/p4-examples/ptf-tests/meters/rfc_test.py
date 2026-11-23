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
import math
import datetime

import pd_base_tests

from ptf import config
from ptf.testutils import *
from ptf.thriftutils import *

import os

from meters.p4_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from collections import defaultdict

import random

this_dir = os.path.dirname(os.path.abspath(__file__))

meter_index   = 5
iteration     = 1
num_pass      = 0

color_green   = 0
color_yellow  = 1
color_red     = 3

dev_id        = 0

g_arch        = test_param_get("arch").lower()
g_port_mode   = test_param_get("port_mode").lower()
g_is_tofino   = ( g_arch == "tofino" )
g_is_tofino2  = ( g_arch == "tofino2" )
g_is_tofino3  = ( g_arch == "tofino3" )
g_is_sw_model = ( test_param_get('target') != "hw" )

assert g_is_tofino or g_is_tofino2 or g_is_tofino3

swports = []
for device, port, ifname in config["interfaces"]:
    swports.append(port)
    swports.sort()

if swports == []:
    swports = range(9)

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

if ( test_param_get('target') == "emulation" ):
    g_clock_speed = 200000         # emulator freq
else:
    if g_is_tofino:
        g_clock_speed = 1250000000 # 1.25GHz
    elif g_is_tofino2 or g_is_tofino3:
        g_clock_speed = 1000000000 # 1.0GHz
    else:
        g_clock_speed = 7500 # 7.5KHz

def advance_model_time_by_clocks(test, shdl, clocks):
    # Convert clocks to pico-seconds and advance time.
    pico_per_clock = 1000
    if test_param_get("arch") == "tofino":
        pico_per_clock = 800

    picos = clocks * pico_per_clock
    test.conn_mgr.advance_model_time(shdl, dev_id, picos)
    test.conn_mgr.complete_operations(shdl)

def calculate_exp_results(self, test_params):
    test_name       = test_params['test_params']['name']

    OIR             = float(test_params['test_params']['oir'])
    CIR             = float(test_params['test_params']['cir'])
    PIR             = float(test_params['test_params']['pir'])
    # used for bursty traffic
    TIR             = float(test_params['test_params']['traffic_rate'])
    CBS             = float(test_params['test_params']['cbs'])
    PBS             = float(test_params['test_params']['pbs'])

    IPG             = float( test_params['test_params']['ipg'])
    IPG_bytes       = float( IPG / 8 )
    pkt_size        = float( test_params['test_params']['packet_size'])
    pkt_size_bytes  = float( pkt_size / 8 )
    pkt_per_burst   = float( test_params['test_params']['packet_burst'])
    num_burst       = float( test_params['test_params']['burst_cnt'])
    pkt_count       = float( pkt_per_burst * num_burst )

    clk_scale       = float( test_params['test_params']['clk_scale'])
    run_time        = float( test_params['test_params']['run_time'])

    # this is the packet size including the IPG (i.e. preamble)
    pkt_size_actual = float( pkt_size + IPG )

    OIR             = float( OIR / clk_scale )
    CIR             = float( CIR / clk_scale )
    PIR             = float( PIR / clk_scale )
    TIR             = float( TIR / clk_scale )

    # this is the offered rate that IXIA sends with the IPG included
    OIR_actual      = float( OIR * ( pkt_size_actual ) / pkt_size )
    TIR_actual      = float( TIR * ( pkt_size_actual ) / pkt_size )

    OBS             = float( pkt_size_actual * pkt_per_burst )

    #
    IBG_ns          = float( float( OBS / OIR_actual ) - float( OBS / TIR_actual ) ) * 1000000000 # ns

    if ( g_is_tofino2 ):
        sat_control     = test_params['test_params']['sat_ctl']
    else:
        sat_control     = 0

    excess_yellow   = 0
    excess_red      = 0

    expected_green  = 0
    expected_yellow = 0
    expected_red    = 0

    margin_green    = 0
    margin_yellow   = 0
    margin_red      = 0

    inertia_crd     = 0
    inertia_margin  = 0

    total_pkt = 0

    if (sat_control):
        inertia_crd = 0
    else:
        # See: TOFLAB-38
        inertia_crd = 3200                      # bits (400*8)

    print("inertia_crd = %d" % ( inertia_crd ))

    margin_fudge   = 1

    if ( test_params['test_params']['type'] == "BURST" ):
        total_pkt      = ( pkt_per_burst * num_burst )
        inertia_margin = ( inertia_crd // pkt_size )
        margin_total   = ( ( inertia_margin + margin_fudge ) * num_burst )

        # the complex credit (bits) math equals burst length*rate; burst length equals pkt_per_burst*pkt_size/offered rate
        # credit (pkt) = credit (bits)/pkt_size
        # (i.e. (pkt_per_burst*pkt_size/OIR)*CIR/pkt_size)
        crd_yellow  = ( ( pkt_per_burst / OIR ) * CIR )
        crd_red     = ( ( pkt_per_burst / OIR ) * PIR )

        if ( OBS > PBS ):
            excess_red    = float( ( OBS - PBS ) / ( pkt_size_actual ) )
            excess_yellow = float( ( OBS - CBS ) / ( pkt_size_actual ) )

            pkt_red       = max( ( ( excess_red ) - crd_red ) * num_burst, 0 )
            pkt_yellow    = max( ( ( ( excess_yellow ) - crd_yellow ) * num_burst ) - pkt_red, 0 )
        elif ( OBS > CBS ):
            excess_yellow = float( ( OBS - CBS ) / ( pkt_size_actual ) )

            pkt_red       = 0
            pkt_yellow    = max( ( ( excess_yellow ) - crd_yellow ) * num_burst, 0 )
        else:
            pkt_red       = 0
            pkt_yellow    = 0
    else:
        total_pkt    = pkt_count
        margin_total = 0

        crd_yellow   = 0
        crd_red      = 0

        # The full excess computation is: ((OIR-CIR)*((pkt_count*pkt_size)/OIR))/pkt_size but that simplifies
        # down to ((OIR-CIR)/OIR)
        # (CBS/pkt_size): is the number of packets before coloring begins, so we need to subtract that from excess
        if ( OIR_actual > PIR ):
            excess_red    = math.ceil( float( ( ( OIR_actual - PIR ) * pkt_count ) / OIR_actual ) ) - ( PBS / pkt_size_actual )
            excess_yellow = math.ceil( float( ( ( OIR_actual - CIR ) * pkt_count ) / OIR_actual ) ) - ( CBS / pkt_size_actual )

            pkt_red       = max( excess_red, 0 )
            pkt_yellow    = max( ( excess_yellow - pkt_red ), 0 )
        elif ( OIR_actual > CIR ):
            excess_yellow = math.ceil( float( ( ( OIR_actual - CIR ) * pkt_count ) / OIR_actual ) ) - ( CBS / pkt_size_actual )

            pkt_red       = 0
            pkt_yellow    = max( ( excess_yellow - pkt_red ), 0 )
        else:
            pkt_red       = 0
            pkt_yellow    = 0

    expected_green  = ( total_pkt - pkt_yellow - pkt_red )
    expected_yellow = pkt_yellow
    expected_red    = pkt_red

    # adding 1 packet margin to remove noise
    margin_yellow   = max( margin_total, ( expected_yellow * 0.01 ) )
    margin_red      = max( margin_total, ( expected_red * 0.01 ) )
    margin_green    = ( margin_yellow + margin_red )

    print(datetime.datetime.now(), "[INFO] test name = {0}".format( test_name ))

    print(datetime.datetime.now(), "[INFO] ++++ IXIA programming +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
    print(datetime.datetime.now(), "[INFO] packet_size=   {0:8.0f} bytes,   IPG=            {1:8.0f} bytes,   IBG=         {2:8.0f} ns".format( pkt_size_bytes, IPG_bytes, IBG_ns ))
    print(datetime.datetime.now(), "[INFO] pkt_in_burst=  {0:8.0f},         num_burst=      {1:8.0f},         packet_count={2:8.0f}".format( pkt_per_burst, num_burst, pkt_count ))
    print(datetime.datetime.now(), "[INFO] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")

    print(datetime.datetime.now(), "[INFO] offered_rate=  {0:8.0f} Mbps,    commited_rate=  {1:8.0f} Mbps,    peak_rate=   {2:8.0f} Mbps".format((OIR/1000000.0), (CIR/1000000.0), (PIR/1000000.0)))
    print(datetime.datetime.now(), "[INFO] offered_rate=  {0:8.0f} Mbps (including IPG)".format((OIR_actual/1000000.0)))
    print(datetime.datetime.now(), "[INFO] offered_burst= {0:8.0f} Kbits,   commited_burst= {1:8.0f} Kbits,   peak_burst=  {2:8.0f} Kbits".format((OBS/1000.0), (CBS/1000.0), (PBS/1000.0)))
    print(datetime.datetime.now(), "[INFO] excess_yellow= {0:8.0f} packets, excess_red=     {1:8.0f} packets".format(excess_yellow, excess_red))
    print(datetime.datetime.now(), "[INFO] crd_yellow=    {0:8.0f} packets, crd_red=        {1:8.0f} packets".format(crd_yellow, crd_red))
    print(datetime.datetime.now(), "[INFO] pkt_total=     {0:8.0f} packets, pkt_yellow=     {1:8.0f} packets, pkt_red=     {2:8.0f} packets".format(total_pkt, pkt_yellow, pkt_red))
    print(datetime.datetime.now(), "[INFO] expected_green={0:8.0f} packets, expected_yellow={1:8.0f} packets, expected_red={2:8.0f} packets".format(expected_green, expected_yellow, expected_red))
    print(datetime.datetime.now(), "[INFO] margin_green=  {0:8.0f} packets, margin_yellow=  {1:8.0f} packets, margin_red=  {2:8.0f} packets".format(margin_green, margin_yellow, margin_red))

    test_params['test_params']['obs'] = OBS
    test_params['test_params']['ibg'] = IBG_ns

    test_params['expected_results']['expected_green']  = (total_pkt-pkt_yellow-pkt_red)
    test_params['expected_results']['expected_yellow'] = pkt_yellow
    test_params['expected_results']['expected_red']    = pkt_red

    test_params['expected_results']['margin_green']    = margin_green
    test_params['expected_results']['margin_yellow']   = margin_yellow
    test_params['expected_results']['margin_red']      = margin_red


def run_meter_test(self, test_params, expected_results, logfile):
    global meter_index
    global iteration
    global color_green
    global color_yellow
    global color_red
    global num_pass

    test_name = test_params['name']

    pir       = test_params['pir']
    cir       = test_params['cir']
    oir       = test_params['oir']

    clk_scale = float( test_params['clk_scale'] )

    # need to scale for emulator
    cir             = float( cir / clk_scale )
    pir             = float( pir / clk_scale )

    pbs             = test_params['pbs']
    cbs             = test_params['cbs']

    run_time        = test_params['run_time']
    pkt_size        = test_params['packet_size']
    pkt_per_burst   = test_params['packet_burst']
    num_burst       = test_params['burst_cnt']
    pkt_count       = ( pkt_per_burst * num_burst )

    ipg             = test_params['ipg']

    expected_green  = float(expected_results['expected_green'])
    expected_yellow = float(expected_results['expected_yellow'])
    expected_red    = float(expected_results['expected_red'])

    margin_green    = expected_results['margin_green']
    margin_yellow   = expected_results['margin_yellow']
    margin_red      = expected_results['margin_red']

    print(datetime.datetime.now(), "[INFO] computing meter test parameters")

    # Convert the CIR and PIR from bits/second to kilobits/second
    pir_kbps  = ( pir / 1000 )
    cir_kbps  = ( cir / 1000 )

    # Convert the PBS and CBS to kilobits
    pbs_kbits = ( pbs / 1000 )
    cbs_kbits = ( cbs / 1000 )

    #
    pkt_size_byte = ( pkt_size / 8 )

    # Now that all parameters for the test are calculated, set up the match entry
    sess_hdl = self.conn_mgr.client_init()
    dev_tgt  = DevTarget_t( dev_id, hex_to_i16( 0xFFFF ) )

    meter_idx        = meter_index
    meter_index      = ( meter_index + 1 )

    green_stats_idx  = ( ( color_green + 3 ) * iteration )
    yellow_stats_idx = ( green_stats_idx + 1 )
    red_stats_idx    = ( yellow_stats_idx + 1 )

    iteration        = ( iteration + 1 )

    ip_address_dst   = '10.11.12.13'

    match_spec   = meters_meter_tbl_match_spec_t( ipv4Addr_to_i32( ip_address_dst ) )

    action_spec  = meters_meter_action_action_spec_t( meter_idx )

    meter_0_spec = meters_bytes_meter_spec_t(cir_kbps, cbs_kbits, pir_kbps, pbs_kbits, False)
    self.client.meter_set_meter_0(sess_hdl, dev_tgt, meter_idx, meter_0_spec)

    print(datetime.datetime.now(), "[INFO] adding entries...")

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

    try:
        # Set the counter to zero
        counter_value = meters_counter_value_t(packets=0, bytes=0)

        self.client.counter_write_colorCntr(sess_hdl, dev_tgt, green_stats_idx, counter_value)
        self.client.counter_write_colorCntr(sess_hdl, dev_tgt, yellow_stats_idx, counter_value)
        self.client.counter_write_colorCntr(sess_hdl, dev_tgt, red_stats_idx, counter_value)

        if test_param_get('target') == "asic-model":
            cycle_count = 0
            packets_per_second = ( oir / pkt_size )
            cycles_per_packet  = ( g_clock_speed / packets_per_second )

            print(datetime.datetime.now(), "cycles_per_packet = %f" % ( cycles_per_packet ))

            # Harlyn, puts in 4 bytes of CRC, so the packet seen by MAU will be the packet length + 4 Bytes.
            # Hence, the packet size we send is 4 bytes lesser than desired value.
            pkt = simple_tcp_packet( pktlen = ( pkt_size - 4 ),
                                     ip_dst = ip_address_dst,
                                     with_tcp_chksum = False )

            i = 0

            while (cycle_count < (g_clock_speed*run_time)):
                advance_model_time_by_clocks(self, sess_hdl, cycles_per_packet)
                cycle_count = cycle_count + cycles_per_packet
                send_packet(self, swports[1], pkt)
                time.sleep(0.05)
                #(rcv_port, rcv_pkt, pkt_time) = self.dataplane.poll(timeout=2)
        else:
            num_pkt = int( math.ceil( ( oir / pkt_size ) * run_time ) )

            # using number of packets because API cannot handle decimal run_time
            #self.dv_initialize( oir, pkt_size_byte, ip_address_dst, pkt_count, self.ipg_bytes )
            self.dv_start_transmit()
            #time.sleep( run_time )
            self.dv_stop_transmit()

        print(datetime.datetime.now(), "[INFO] done driving traffic...")

        # Now verify if the results match up the expected results.

        flags = meters_counter_flags_t(1)

        print(datetime.datetime.now(), "[INFO] calling counter_read_colorCntr[stats_idx={0}](green)...".format( green_stats_idx ))
        green_count = self.client.counter_read_colorCntr(sess_hdl, dev_tgt, green_stats_idx, flags)
        self.conn_mgr.complete_operations(sess_hdl)

        print(datetime.datetime.now(), "[INFO] green_count={0}...".format( green_count ))

        print(datetime.datetime.now(), "[INFO] calling counter_read_colorCntr[stats_idx={0}](yellow)...".format( yellow_stats_idx ))
        yellow_count = self.client.counter_read_colorCntr(sess_hdl, dev_tgt, yellow_stats_idx, flags)
        self.conn_mgr.complete_operations(sess_hdl)

        print(datetime.datetime.now(), "[INFO] yellow_count={0}...".format( yellow_count ))

        print(datetime.datetime.now(), "[INFO] calling counter_read_colorCntr[stats_idx={0}](red)...".format( red_stats_idx ))
        red_count = self.client.counter_read_colorCntr(sess_hdl, dev_tgt, red_stats_idx, flags)
        self.conn_mgr.complete_operations(sess_hdl)

        print(datetime.datetime.now(), "[INFO] red_count={0}...".format( red_count ))

        pass_0 = 0
        pass_1 = 0
        pass_2 = 0

        if (expected_green != 0):
            error       = float(abs(expected_green - green_count.packets)*100.0/expected_green)
            margin_perc = float(abs(margin_green)*100.0/expected_green)
        else:
            error       = 0
            margin_perc = 0

        print(datetime.datetime.now(), "[INFO] Expected green = %8d, green count = %8d, error= %3.2f%% (accepted_margin=%3.2f%%)" % (expected_green, green_count.packets, error, margin_perc))

        if (expected_yellow != 0):
            error       = float(abs(expected_yellow - yellow_count.packets)*100.0/expected_yellow)
            margin_perc = float(abs(margin_yellow)*100.0/max(expected_yellow, 1))
        else:
            error       = 0
            margin_perc = 0

        print(datetime.datetime.now(), "[INFO] Expected yellow= %8d, yellow count= %8d, error= %3.2f%% (accepted_margin=%3.2f%%)" % (expected_yellow, yellow_count.packets, error, margin_perc))

        if (expected_red != 0):
            error       = float(abs(expected_red - red_count.packets)*100.0/expected_red)
            margin_perc = float(abs(margin_red)*100.0/max(expected_red, 1))
        else:
            error       = 0
            margin_perc = 0

        print(datetime.datetime.now(), "[INFO] Expected red   = %8d, red count   = %8d, error= %3.2f%% (accepted_margin=%3.2f%%)" % (expected_red, red_count.packets, error, margin_perc))

        if (abs(green_count.packets - expected_green) <= (margin_green)):
            pass_0 = 1

        if (abs(yellow_count.packets - expected_yellow) <= (margin_yellow)):
            pass_1 = 1

        if (abs(red_count.packets - expected_red) <= (margin_red)):
            pass_2 = 1

        if (pass_0 == 1 and pass_1 == 1 and pass_2 == 1):
            print(datetime.datetime.now(), "[INFO] test passed.")
            logfile.write("PASS\n")
            num_pass = num_pass + 1
        else:
            print(datetime.datetime.now(), "[INFO] test failed.")
            logfile.write("FAIL\n")

        print(datetime.datetime.now(), "[INFO] pass_0={0}, pass_1={1}, pass_2={2}.".format(pass_0, pass_1, pass_2))

    finally:
        # Delete the entry
        self.client.meter_tbl_table_delete(sess_hdl, dev_id, mat_entry_hdl)

        for entry_hdl in entry_hdls:
            self.client.color_match_table_delete(sess_hdl, dev_id, entry_hdl)

        print(datetime.datetime.now(), "[INFO] closing session...")
        status = self.conn_mgr.client_cleanup(sess_hdl)

@group("thorough")
class TestMeterRFC(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["meters"])

        if ( g_is_tofino2 ):
            self.meter_ctl__base_addr     = 0x0407300c
            self.meter_ctl__alu_shift     = 10
            self.meter_ctl__stage_shift   = 19
            self.meter_ctl__pipe_shift    = 24
            self.meter_ctl__sat_ctl_shift = 24
        else:
            self.meter_ctl__base_addr     = 0x0407300c
            self.meter_ctl__alu_shift     = 10
            self.meter_ctl__stage_shift   = 19
            self.meter_ctl__pipe_shift    = 24
            self.meter_ctl__sat_ctl_shift = 24

        self.sat_ctl   = 1
        self.ipg_bytes = 20             # in bytes

    def dv_initialize(  self, pkt_rate, pkt_size, dst_ip_addr, num_pkt ):
        pass

    def dv_start_transmit( self ):
        print(datetime.datetime.now(), "[INFO] start traffic...")
        pdb.set_trace()
        pass

    def dv_stop_transmit( self ):
        print(datetime.datetime.now(), "[INFO] stop traffic...")
        pdb.set_trace()
        pass

    def write_meter_ctl( self, shdl, pipe, stage, alu ):
        address  = self.meter_ctl__base_addr
        address |= ( pipe  << self.meter_ctl__pipe_shift )
        address |= ( stage << self.meter_ctl__stage_shift )
        address |= ( alu   << self.meter_ctl__alu_shift )

        sat_ctl_mask  = 0xFFFFFFFF & ~( 1 << self.meter_ctl__sat_ctl_shift )
        byte_adj_mask = 0xFFFFFF00

        data  = self.conn_mgr.reg_rd( dev_id, address )

        data &= sat_ctl_mask
        data &= byte_adj_mask

        data |= ( data | self.ipg_bytes | ( self.sat_ctl << self.meter_ctl__sat_ctl_shift ) )

        # dev_0 pipes[pipe] mau[stage] rams map_alu meter_group[alu] meter meter_ctl
        print(datetime.datetime.now(), "[INFO] Writing meter_ctl[0x%x] for pipe%0d, stage%0d, alu%0d = 0x%x" % ( address, pipe, stage, alu, data ))
        self.conn_mgr.reg_wr( dev_id, address, data )

    def dv_finalize( self ):
        pass

    def runTest( self ):
        print("")
        if test_param_get("target") == "asic-model":
            return

        print(datetime.datetime.now(), "[INFO] running test...")

        global num_pass

        shdl = self.conn_mgr.client_init()
        self.client.test_select_set_default_action_MeterOmnet(shdl, DevTarget_t(dev_id, hex_to_i16(0xFFFF)))
        #
        self.write_meter_ctl( shdl, pipe=0, stage=1, alu=2 )
        self.conn_mgr.client_cleanup(shdl)

        num_tests = 9
        logfile = open('rfc_test_log', 'w')
        testMatrix = defaultdict(lambda:defaultdict(defaultdict))

        pkt_size       = 256*8
        pkt_multiplier = 100000

        if ( test_param_get('target') == "emulation" ):
            clk_scale  = 5000
        else:
            clk_scale  = 1

        print(datetime.datetime.now(), "[INFO] configuring test params")

        ####################################################################
        # PIR: Peak Information Rate
        # CIR: Committed Information Rate
        # PBS: Peak Burst Size
        # CBS: Committed Burst Size
        ################ BT1 ###############################################
        # OBS < CBS: All packets are green
        testMatrix[0]['test_params']['name']         = 'BT1'
        testMatrix[0]['test_params']['type']         = 'BURST'
        testMatrix[0]['test_params']['cir']          = 50000000000              # bps  (=50Gbps)
        testMatrix[0]['test_params']['pir']          = 100000000000             # bps  (=100Gbps)
        testMatrix[0]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[0]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[0]['test_params']['clk_scale']    = clk_scale

        testMatrix[0]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[0]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[0]['test_params']['packet_burst'] = 230                      #
        testMatrix[0]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[0]['test_params']['oir']          = 400000000000             # bps
        testMatrix[0]['test_params']['traffic_rate'] = 400000000000             # bps  (=400Gbps)
        testMatrix[0]['test_params']['run_time']     = 30                       # secs
        testMatrix[0]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        ################ BT2 ###############################################
        # OBS > CBS && OBS < PBS
        testMatrix[1]['test_params']['name']         = 'BT2'
        testMatrix[1]['test_params']['type']         = 'BURST'
        testMatrix[1]['test_params']['cir']          = 50000000000              # bps  (=50Gbps)
        testMatrix[1]['test_params']['pir']          = 100000000000             # bps  (=100Gbps)
        testMatrix[1]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[1]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[1]['test_params']['clk_scale']    = clk_scale

        testMatrix[1]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[1]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[1]['test_params']['packet_burst'] = 300                      #
        testMatrix[1]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[1]['test_params']['oir']          = 400000000000             # bps
        testMatrix[1]['test_params']['traffic_rate'] = 400000000000             # bps  (=400Gbps)
        testMatrix[1]['test_params']['run_time']     = 30                       # secs
        testMatrix[1]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        ################ BT3 ###############################################
        # OBS > PBS
        testMatrix[2]['test_params']['name']         = 'BT3'
        testMatrix[2]['test_params']['type']         = 'BURST'
        testMatrix[2]['test_params']['cir']          = 50000000000              # bps  (=50Gbps)
        testMatrix[2]['test_params']['pir']          = 100000000000             # bps  (=100Gbps)
        testMatrix[2]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[2]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[2]['test_params']['clk_scale']    = clk_scale

        testMatrix[2]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[2]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[2]['test_params']['packet_burst'] = 800                      #
        testMatrix[2]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[2]['test_params']['oir']          = 400000000000             # bps
        testMatrix[2]['test_params']['traffic_rate'] = 400000000000             # bps  (=400Gbps)
        testMatrix[2]['test_params']['run_time']     = 30                       # secs
        testMatrix[2]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        ################ RT1 ###############################################
        # OIR < CIR: All packets are green
        testMatrix[3]['test_params']['name']         = 'RT1'
        testMatrix[3]['test_params']['type']         = 'RATE'
        testMatrix[3]['test_params']['cir']          = 50000000000              # bps  (=50Gbps)
        testMatrix[3]['test_params']['pir']          = 100000000000             # bps  (=100Gbps)
        testMatrix[3]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[3]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[3]['test_params']['clk_scale']    = clk_scale

        testMatrix[3]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[3]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[3]['test_params']['packet_burst'] = 230                      #
        testMatrix[3]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[3]['test_params']['oir']          = 40000000000              # bps
        testMatrix[3]['test_params']['traffic_rate'] = 40000000000              # bps  (=40Gbps)
        testMatrix[3]['test_params']['run_time']     = 10                       # secs
        testMatrix[3]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        ################ RT2 ###############################################
        # OIR < CIR: All packets are green
        testMatrix[4]['test_params']['name']         = 'RT2'
        testMatrix[4]['test_params']['type']         = 'RATE'
        testMatrix[4]['test_params']['cir']          = 50000000000              # bps  (=50Gbps)
        testMatrix[4]['test_params']['pir']          = 100000000000             # bps  (=100Gbps)
        testMatrix[4]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[4]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[4]['test_params']['clk_scale']    = clk_scale

        # OIR: 40Gbps
        # OBS: (2048+160)*230=507840
        # BR:  400Gbps
        testMatrix[4]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[4]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[4]['test_params']['packet_burst'] = 230                      #
        testMatrix[4]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[4]['test_params']['oir']          = 40000000000              # bps
        # traffic_rate cannot be max line rate because IPG is assumed in computation
        testMatrix[4]['test_params']['traffic_rate'] = 300000000000             # bps  (=300Gbps)
        testMatrix[4]['test_params']['run_time']     = 10                       # secs
        testMatrix[4]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        ################ RT3 ###############################################
        # OBS > CIR && OBS < PIR
        testMatrix[5]['test_params']['name']         = 'RT3'
        testMatrix[5]['test_params']['type']         = 'RATE'
        testMatrix[5]['test_params']['cir']          = 50000000000              # bps  (=50Gbps)
        testMatrix[5]['test_params']['pir']          = 100000000000             # bps  (=100Gbps)
        testMatrix[5]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[5]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[5]['test_params']['clk_scale']    = clk_scale

        testMatrix[5]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[5]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[5]['test_params']['packet_burst'] = 230                      #
        testMatrix[5]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[5]['test_params']['oir']          = 80000000000              # bps  (=80Gbps)
        testMatrix[5]['test_params']['traffic_rate'] = 80000000000              # bps  (=80Gbps)
        testMatrix[5]['test_params']['run_time']     = 10                       # secs
        testMatrix[5]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        ################ RT4 ###############################################
        # OBS > CIR && OBS < PIR
        testMatrix[6]['test_params']['name']         = 'RT4'
        testMatrix[6]['test_params']['type']         = 'RATE'
        testMatrix[6]['test_params']['cir']          = 50000000000              # bps  (=50Gbps)
        testMatrix[6]['test_params']['pir']          = 100000000000             # bps  (=100Gbps)
        testMatrix[6]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[6]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[6]['test_params']['clk_scale']    = clk_scale

        testMatrix[6]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[6]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[6]['test_params']['packet_burst'] = 230                      #
        testMatrix[6]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[6]['test_params']['oir']          = 80000000000              # bps  (=80Gbps)
        # traffic_rate cannot be max line rate because IPG is assumed in computation
        testMatrix[6]['test_params']['traffic_rate'] = 300000000000             # bps  (=300Gbps)
        testMatrix[6]['test_params']['run_time']     = 10                       # secs
        testMatrix[6]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        ################ RT5 ###############################################
        # OBS > PIR
        testMatrix[7]['test_params']['name']         = 'RT5'
        testMatrix[7]['test_params']['type']         = 'RATE'
        testMatrix[7]['test_params']['cir']          = 40000000000              # bps  (=50Gbps)
        testMatrix[7]['test_params']['pir']          = 80000000000              # bps  (=100Gbps)
        testMatrix[7]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[7]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[7]['test_params']['clk_scale']    = clk_scale

        testMatrix[7]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[7]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[7]['test_params']['packet_burst'] = 230                      #
        testMatrix[7]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[7]['test_params']['oir']          = 120000000000             # bps  (=120Gbps)
        testMatrix[7]['test_params']['traffic_rate'] = 120000000000             # bps  (=120Gbps)
        testMatrix[7]['test_params']['run_time']     = 10                       # secs
        testMatrix[7]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        ################ RT6 ###############################################
        # OBS > PIR
        testMatrix[8]['test_params']['name']         = 'RT6'
        testMatrix[8]['test_params']['type']         = 'RATE'
        testMatrix[8]['test_params']['cir']          = 50000000000              # bps  (=50Gbps)
        testMatrix[8]['test_params']['pir']          = 100000000000             # bps  (=100Gbps)
        testMatrix[8]['test_params']['cbs']          = 512000                   # bits (cbs_kbits=512)
        testMatrix[8]['test_params']['pbs']          = 1024000                  # bits (pbs_kbits=1024)

        testMatrix[8]['test_params']['clk_scale']    = clk_scale

        testMatrix[8]['test_params']['packet_size']  = 248*8                    # bits
        testMatrix[8]['test_params']['ipg']          = ( self.ipg_bytes * 8 )   # bits
        testMatrix[8]['test_params']['packet_burst'] = 230                      #
        testMatrix[8]['test_params']['burst_cnt']    = ( 100 * pkt_multiplier ) #

        testMatrix[8]['test_params']['oir']          = 120000000000             # bps  (=120Gbps)
        # traffic_rate cannot be max line rate because IPG is assumed in computation
        testMatrix[8]['test_params']['traffic_rate'] = 300000000000             # bps  (=300Gbps)
        testMatrix[8]['test_params']['run_time']     = 10                       # secs
        testMatrix[8]['test_params']['sat_ctl']      = self.sat_ctl
        ####################################################################

        #num_tests = 1
        #each_test = 8
        #calculate_exp_results(self, testMatrix[each_test])
        #run_meter_test(self, testMatrix[each_test]['test_params'], testMatrix[each_test]['expected_results'], logfile)
        for each_test in testMatrix:
            logfile.write("Test : %s\n" % (each_test))
            calculate_exp_results(self, testMatrix[each_test])
            run_meter_test(self, testMatrix[each_test]['test_params'], testMatrix[each_test]['expected_results'], logfile)
            logfile.write("########################\n")

        if (num_pass != num_tests):
            print("FAIL : Check Omnet_test_logs for failures")
            print(" Num pass = %d" % (num_pass))
            assert(num_pass == num_tests)
