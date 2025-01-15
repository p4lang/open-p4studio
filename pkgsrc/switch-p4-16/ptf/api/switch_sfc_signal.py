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
import sys
import unittest

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))

from common.utils import *
from common.lib_sfc import *

from bf_pktpy.packets import SfcPause

from ptf.testutils import simple_udp_packet


class SfcSignalTestHelper(SfcTestHelper):
    def setUp(self):

        SfcTestHelper.__init__(self)
        super(SfcSignalTestHelper, self).setUp()

        self.port_speeds = dict()
        server_ports = self.devports
        switch_ports = []
        for p in server_ports:
            port_speed = PortSpeed.GbE_25
            self.port_speeds[p] = port_speed
        for p in switch_ports:
            port_speed = PortSpeed.GbE_100
            self.port_speeds[p] = port_speed
        print("\n")
        print("Packet initial")

        self.sfc_config = SfcConfig(
            # yle: it is strange that we have to set port_speed as GbE_400
            port_speed=PortSpeed.GbE_400,
            pipes=[0, 1, 2, 3],
            ports=self.devports,
            server_ports=server_ports,
            switch_ports=switch_ports,
            queues=[0],
            port_speeds=self.port_speeds,
            qlength_threshold=10000,
            target_queuedepth=10000,
            suppression_epoch_duration=2000000,
            sfc_pause_packet_dscp=1,
            # the function get_port_handle_from_port_num
            # does not get the right port handle for the normal ports
            # the function returns the correct port handle for the recirculation ports.
            mirror_egress_ports=[2],
            tcs=[0],
            dscp_mapping={40: 0x20, 0: 0x1},
            dscp_tc_map={0: 0, 1: 1},
            skip_trigger=True,
            always_trigger=True,
            ignore_suppression=True,
            skip_suppression=True,
        )

        self.pkt = simple_rocev2_bth(eth_dst='00:77:66:55:44:33',
                                                     eth_src='00:11:22:33:44:55',
                                                     ip_dst='10.10.10.2',
                                                     ip_src='11.11.11.2',
                                                     ip_dscp=0,
                                                     ip_ttl=64,
                                                     pktlen=1000)
        self.exp_pkt = simple_rocev2_bth(eth_dst='00:10:22:33:44:55',
                                                         eth_src='00:77:66:55:44:33',
                                                         ip_dst='10.10.10.2',
                                                         ip_src='11.11.11.2',
                                                         ip_dscp=0,
                                                         ip_ttl=63,
                                                         pktlen=1000)

        udp_payload = SfcPause(duration_us=0, dscp=0)
        print("SfcPause header length={}".format(len(udp_payload)))
        # as this version, we will trigger re_write_l3, thus, the ip_ttl will be reduced by 1.
        self.sfc_pkt_at_recir_port = simple_udp_packet(
            pktlen=60,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:10:22:33:44:55',
            ip_dst='11.11.11.2',
            ip_src='10.10.10.2',
            udp_sport=0,
            udp_dport=1674,
            ip_ttl=63,
            ip_dscp=self.sfc_config.sfc_pause_packet_dscp,
            with_udp_chksum=False,
            udp_payload=udp_payload)
        self.cpu_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_port=swport_to_devport(self, self.devports[1]),
            ingress_ifindex=(self.port1 & 0xFFFF),
            reason_code=0,
            ingress_bd=0x0,
            inner_pkt=self.sfc_pkt_at_recir_port
        )

        # as this version, we will trigger re_write_l3, thus, the ip_ttl will be reduced by 1.
        # because this packet goes twice of the egress, so the ttl should reduced by 2
        self.sfc_pkt_at_inter_switch = ptf.mask.Mask(simple_udp_packet(
            pktlen=60,
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.2',
            ip_src='10.10.10.2',
            udp_sport=0,
            udp_dport=1674,
            ip_dscp=self.sfc_config.sfc_pause_packet_dscp,
            ip_ttl=62,
            with_udp_chksum=False,
            udp_payload=udp_payload))

        self.pfc_pkt = simple_pfc_pkt(
            eth_dst='00:11:22:33:44:55',
            c0=1,
            pause0_time=0x0,
            pause1_time=0x0,
            pause2_time=0x0,
            pause3_time=0x0,
            pause4_time=0x0,
            pause5_time=0x0,
            pause6_time=0x0,
            pause7_time=0x0)


@group('sfc')
@requires_bfrtgrpc()
class SfcSignalNormalPortForClone(SfcSignalTestHelper):
    def setUp(self):
        super(SfcSignalNormalPortForClone, self).setUp()

    def runTest(self):
        try:
            print("========== {}".format(self.recirculation_ports_number))
            print("========== {}".format(self.devports))

            self.init_sfc_stats()
            self.init_sfc(self.sfc_config)
            self.base_l3_setup()

            print("\n")
            print(" SfcSignalNormalPortForClone")
            print("==========")

            print("self.devports[1] {}, devports[0] {}".format(
                self.devports[1], self.devports[0]))
            # t_cnt = self.get_sfc_type_counters()
            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            # print(repr(self.pkt))
            # print(repr(self.exp_pkt))
            print(len(self.pkt), len(self.exp_pkt))
            send_packet(self, self.devports[1], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[0])

            print("if1 -> eth1(DUT) - eth2(DUT) -> if2 -> sfc_pkt")
            print(repr(self.cpu_pkt))
            verify_packet(self, self.cpu_pkt, self.devports[2])

            print("sfc_pkt -> if0 -> eth0(DUT) - eth1(DUT) -> if1 -> pfc_pkt")
            # remove the mirror session
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)

        finally:
            pass


@group('sfc')
@requires_bfrtgrpc()
class SfcSignalRecircPortForClone(SfcSignalTestHelper):
    def setUp(self):
        super(SfcSignalRecircPortForClone, self).setUp()

        print("\n")
        print(" SfcSignalRecircPortForClone")
        print("==========")

    def TestForLinkToTypeServer(self):
        print("self.devports[1] {}, devports[0] {}".format(
            self.devports[1], self.devports[0]))
        print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
        send_packet(self, self.devports[1], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[0])
        print("expect pfc packet here")
        # time.sleep(200)
        verify_packet(self, self.pfc_pkt, self.devports[1])

    def TestForLinkToTypeSwitch(self):
        self.egress_pause_time_conversion.entry_del(self.dev_tgt)
        customize_sfc_config = self.sfc_config._replace(switch_ports=[self.devports[1]],
                                                        server_ports=[self.devports[0]])
        self._populate_time_conversion_table(customize_sfc_config)

        print("self.devports[1] {}, devports[0] {}".format(
            self.devports[1], self.devports[0]))
        print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
        send_packet(self, self.devports[1], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[0])
        # time.sleep(50)
        print("expect sfc packet here")

        # mask_set_do_not_care_packet(self.masked_sfc_pkt, UDP, "chksum")
        verify_packet(self, self.sfc_pkt_at_inter_switch, self.devports[1])

    def runTest(self):
        self.sfc_config = self.sfc_config._replace(mirror_egress_ports=[266])
        print("mirror_egress_ports {}".format(
            self.sfc_config.mirror_egress_ports))

        self.base_l3_setup()
        self.init_sfc_stats()
        self.init_sfc(self.sfc_config)
        print("run tests")
        print("LinkToType.switch {}".format(int(LinkToType.Server)))
        try:
            print(" SfcSignalRecircPortForClone.TestForLinkToTypeServer")
            print("====================================================")
            self.TestForLinkToTypeServer()
            print("\n")
            print(" SfcSignalRecircPortForClone.TestForLinkToTypeSwitch")
            print("====================================================")
            self.TestForLinkToTypeSwitch()

        finally:
            pass


@group('sfc')
@requires_bfrtgrpc()
class SfcSignalTimeConversion(SfcSignalTestHelper):
    def setUp(self):
        super(SfcSignalTimeConversion, self).setUp()

    def renew_port_speed(self, port_speed):
        self.egress_pause_time_conversion.entry_del(self.dev_tgt)
        _port_speeds = dict()
        for port in self.sfc_config.server_ports + self.sfc_config.switch_ports:
            _port_speeds[port] = port_speed
        self.sfc_config = self.sfc_config._replace(
            port_speeds=_port_speeds)
        self._populate_time_conversion_table(self.sfc_config)

    def qdepth_to_pause_duration_us(self, port_speed, qdepth):
        if port_speed == PortSpeed.GbE_25:
            pause_duration_us = qdepth >> 1
        elif port_speed == PortSpeed.GbE_50:
            pause_duration_us = qdepth >> 2
        elif port_speed == PortSpeed.GbE_100:
            pause_duration_us = qdepth >> 3
        else:
            raise Exception("Unknown port speed {}.".format(
                self.sfc_config.port_speeds[self.devports[0]]))

        return pause_duration_us

    def pause_duration_pfc_quota(self, port_speed, pause_duration_us):
        if port_speed == PortSpeed.GbE_25:
            expected_pause_quota = (
                                           pause_duration_us << 4) + (pause_duration_us << 5)
        elif port_speed == PortSpeed.GbE_50:
            expected_pause_quota = (
                                           pause_duration_us << 5) + (pause_duration_us << 6)
        elif port_speed == PortSpeed.GbE_100:
            expected_pause_quota = (
                                           pause_duration_us << 6) + (pause_duration_us << 7)
        else:
            raise Exception("Unknown port speed {}.".format(
                self.sfc_config.port_speeds[self.devports[0]]))
        return expected_pause_quota

    def TestSfcPauseTime(self):
        for port_speed in self.port_speed_list:
            self.renew_port_speed(port_speed)
            for qdepth_drain_cells in self.cells_list:

                self._populate_egress_threshold_reg(
                    self.sfc_config, qdepth_drain_cells=qdepth_drain_cells)
                actual_qdepth_drain_cells = qdepth_drain_cells & 0xffff
                print("qdepth_drain_cells = {}, actual_qdepth_drain_cells={}"
                      .format(qdepth_drain_cells, actual_qdepth_drain_cells))
                self.verify_egress_threshold_reg(self.sfc_config,
                                                 qdepth_drain_cells=qdepth_drain_cells)

                if self.sfc_config.port_speeds[self.devports[0]] in self.port_speed_list:
                    pause_duration_us = self.qdepth_to_pause_duration_us(
                        self.sfc_config.port_speeds[self.devports[0]], actual_qdepth_drain_cells)

                self.cpu_pkt['SfcPause'].duration_us = pause_duration_us
                print("self.devports[1] {}, devports[0] {}, devports[2] {}".format(
                    self.devports[1], self.devports[0], self.devports[2]))
                print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
                before_cnt = self.get_sfc_counters()
                print("===================== SEND PACKET =====================")
                send_packet(self, self.devports[1], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[0])
                print("expected pause_duration is {}".format(
                    self.cpu_pkt['SfcPause'].duration_us))

                verify_packet(self, self.cpu_pkt, self.devports[2])
                self.sfc_type_cnt_assert(before_cnt,
                                         ingress={
                                             SfcPacketType.Data: dict(pkt=1)},
                                         egress={SfcPacketType.Data: dict(pkt=1), SfcPacketType.Trigger: dict(pkt=1),
                                                 SfcPacketType.Signal: dict(pkt=0)},
                                         )

    def TestPfcPauseQuota(self):
        # remove the mirror entry;
        # note that this is the right way to remove the mirror entry
        # It does not work if we remove the mirror entry from bfrt mirror table
        self.attribute_set(
            self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
        self.ingress_sfc_decide_mirroring.entry_del(self.dev_tgt)
        self.sfc_config = self.sfc_config._replace(mirror_egress_ports=[266])
        key_dict_default = dict(
            sfc_type=(SfcPacketType.Data.value, 0b111),
            mirror_type=(0, 0b1),
            drop_reason=(0, 0b1),
        )
        key_dict_default['$MATCH_PRIORITY'] = 4
        self._populate_mirror_decision_table(self.sfc_config, key_dict_default)
        try:
            for port_speed in self.port_speed_list:
                self.renew_port_speed(port_speed)
                for qdepth_drain_cells in self.cells_list:
                    self._populate_egress_threshold_reg(
                        self.sfc_config,
                        qdepth_drain_cells=qdepth_drain_cells)
                    actual_qdepth_drain_cells = qdepth_drain_cells & 0xffff
                    if self.sfc_config.port_speeds[self.devports[0]] in self.port_speed_list:
                        pause_duration_us = self.qdepth_to_pause_duration_us(
                            self.sfc_config.port_speeds[self.devports[0]], actual_qdepth_drain_cells)
                        expected_pause_quota = self.pfc_pause_quoto = self.pause_duration_pfc_quota(
                            self.sfc_config.port_speeds[self.devports[0]], pause_duration_us)
                        if expected_pause_quota > 0xffff:
                            pfc_pause_quota = 0xffff
                        else:
                            pfc_pause_quota = expected_pause_quota
                    else:
                        raise Exception("Unknown port speed {}.".format(
                            self.sfc_config.port_speeds[self.devports[0]]))

                    expected_pfc_pkt = simple_pfc_pkt(
                        eth_dst='00:11:22:33:44:55',
                        c0=1,
                        pause0_time=pfc_pause_quota,
                        pause1_time=pfc_pause_quota,
                        pause2_time=pfc_pause_quota,
                        pause3_time=pfc_pause_quota,
                        pause4_time=pfc_pause_quota,
                        pause5_time=pfc_pause_quota,
                        pause6_time=pfc_pause_quota,
                        pause7_time=pfc_pause_quota)

                    print("===================== SEND PACKET =====================")
                    print("send from {},  output port {}".format(
                        self.devports[1], self.devports[0]))
                    print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
                    print("expected pfc pause_time is {}".format(pfc_pause_quota))
                    before_cnt = self.get_sfc_counters()
                    send_packet(self, self.devports[1], self.pkt)
                    verify_packet(self, self.exp_pkt, self.devports[0])
                    verify_packet(self, expected_pfc_pkt, self.devports[1])
                    self.sfc_type_cnt_assert(before_cnt,
                                             ingress={
                                                 SfcPacketType.Data: dict(pkt=1)},
                                             egress={SfcPacketType.Data: dict(pkt=1),
                                                     SfcPacketType.Trigger: dict(pkt=1),
                                                     SfcPacketType.Signal: dict(pkt=1)},
                                             )
        finally:
            pass

    def runTest(self):
        try:
            self.base_l3_setup()
            self.init_sfc_stats()
            self.init_sfc(self.sfc_config)

            self.cells_list = list()
            for i in range(0, 17, 4):
                cells = 0x80000000 | (1 << i)
                self.cells_list.append(cells)

            self.port_speed_list = [PortSpeed.GbE_25,
                                    PortSpeed.GbE_50, PortSpeed.GbE_100]
            print("\n")
            print(" SfcSignalTimeConversion")

            print(" SfcSignalTimeConversion.TestSfcPauseTime")
            print("====================================================")
            self.TestSfcPauseTime()
            print("\n")
            print(" SfcSignalTimeConversion.TestPfcPauseQuota")
            print("====================================================")
            self.TestPfcPauseQuota()
        finally:
            # super(SfcSignalTimeConversion, self).tearDown()
            pass
