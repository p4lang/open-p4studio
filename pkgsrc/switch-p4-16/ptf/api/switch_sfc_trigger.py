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

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))

from common.utils import *
from common.lib_sfc import *


@group('sfc')
@requires_bfrtgrpc()
class BasicL3Test(SfcTestHelper):
    def runTest(self):
        self.base_l3_setup()
        self.init_sfc_stats()

        try:
            t_cnt = self.get_sfc_counters()
            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            pkt = simple_tcp_packet(eth_dst=self.rmac,  # '00:77:66:55:44:33',
                                    eth_src='00:22:22:22:22:22',
                                    ip_dst='10.10.10.2',
                                    ip_src='11.11.11.2',
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst='00:10:22:33:44:55',
                                        eth_src=self.rmac,  # '00:77:66:55:44:33',
                                        ip_dst='10.10.10.2',
                                        ip_src='11.11.11.2',
                                        ip_ttl=63)
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            self.sfc_type_cnt_assert(t_cnt,
                                     ingress={SfcPacketType.TNone: dict(pkt=1)},
                                     egress={SfcPacketType.TNone: dict(pkt=1)},
                                     )
        finally:
            pass


@group('sfc')
@requires_bfrtgrpc()
class BasicNeverTriggerTest(SfcTestHelper):
    def runTest(self):
        self.base_l3_setup()
        self.init_sfc_stats()

        self.sfc_config = SfcConfig(
            port_speed=PortSpeed.GbE_400,
            pipes=[0, 1, 2, 3],
            ports=[self.pipe4[0]],
            queues=[0],
            qlength_threshold=200,
            target_queuedepth=200,
            suppression_epoch_duration=2000000,
            mirror_egress_ports=[2] * 4,
            tcs=[0],
            dscp_mapping={},
            skip_trigger=False,
            always_trigger=False,
            ignore_suppression=False,
            skip_suppression=False,
            server_ports=self.devports,
            switch_ports=[],
            port_speeds=dict.fromkeys(self.devports, PortSpeed.GbE_25),
            sfc_pause_packet_dscp=1,
            dscp_tc_map={0: 0, 1: 1}
        )
        self.init_sfc(self.sfc_config)

        try:
            t_cnt = self.get_sfc_counters()
            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            pkt = simple_rocev2_bth(eth_dst='00:77:66:55:44:33',
                                                    eth_src='00:22:22:22:22:22',
                                                    ip_dst='10.10.10.2',
                                                    ip_src='11.11.11.2',
                                                    ip_dscp=0,
                                                    ip_ttl=64)
            exp_pkt = simple_rocev2_bth(eth_dst='00:10:22:33:44:55',
                                                        eth_src='00:77:66:55:44:33',
                                                        ip_dst='10.10.10.2',
                                                        ip_src='11.11.11.2',
                                                        ip_dscp=0,
                                                        ip_ttl=63)

            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            self.sfc_type_cnt_assert(t_cnt,
                                     ingress={SfcPacketType.Data: dict(pkt=1)},
                                     egress={SfcPacketType.Data: dict(pkt=1)},
                                     )
        finally:
            pass


@group('sfc')
@requires_bfrtgrpc()
class BasicAlwaysTriggerTest(SfcTestHelper):
    def runTest(self):
        self.base_l3_setup()
        self.init_sfc_stats()

        self.sfc_config = SfcConfig(
            port_speed=PortSpeed.GbE_400,
            pipes=[0],
            ports=[self.pipe4[0]],
            queues=[0],
            qlength_threshold=200,
            target_queuedepth=200,
            suppression_epoch_duration=2000000,
            mirror_egress_ports=[2] * 4,
            tcs=[0],
            dscp_mapping={},
            skip_trigger=True,
            always_trigger=True,
            ignore_suppression=True,
            skip_suppression=False,
            server_ports=self.devports,
            switch_ports=[],
            port_speeds=dict.fromkeys(self.devports, PortSpeed.GbE_25),
            sfc_pause_packet_dscp=1,
            dscp_tc_map={0: 0, 1: 1}
        )
        self.init_sfc(self.sfc_config)

        try:
            t_cnt = self.get_sfc_counters()
            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            pkt = simple_rocev2_bth(eth_dst='00:77:66:55:44:33',
                                                    eth_src='00:22:22:22:22:22',
                                                    ip_dst='10.10.10.2',
                                                    ip_src='11.11.11.2',
                                                    ip_dscp=0,
                                                    ip_ttl=64)
            exp_pkt = simple_rocev2_bth(eth_dst='00:10:22:33:44:55',
                                                        eth_src='00:77:66:55:44:33',
                                                        ip_dst='10.10.10.2',
                                                        ip_src='11.11.11.2',
                                                        ip_dscp=0,
                                                        ip_ttl=63)
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            self.sfc_type_cnt_assert(t_cnt,
                                     ingress={SfcPacketType.Data: dict(pkt=1)},
                                     egress={SfcPacketType.Data: dict(pkt=1), SfcPacketType.Trigger: dict(pkt=1)},
                                     )

        finally:
            pass


@group('sfc')
@requires_bfrtgrpc()
class SuppressionEpochSwitch(SfcTestHelper):
    def runTest(self):
        self.base_l3_setup()
        self.init_sfc_stats()

        self.sfc_config = SfcConfig(
            port_speed=PortSpeed.GbE_400,
            pipes=[0],
            ports=[self.pipe4[0]],
            queues=[0],
            qlength_threshold=200,
            target_queuedepth=200,
            suppression_epoch_duration=2000000,
            mirror_egress_ports=[2] * 4,
            tcs=[],
            dscp_mapping={},
            skip_trigger=True,
            always_trigger=True,
            ignore_suppression=False,
            skip_suppression=False,
            server_ports=self.devports,
            switch_ports=[],
            port_speeds=dict.fromkeys(self.devports, PortSpeed.GbE_25),
            sfc_pause_packet_dscp=1,
            dscp_tc_map={0: 0, 1: 1}
        )

        self.init_sfc(self.sfc_config)

        pkt = simple_rocev2_bth(eth_dst='00:77:66:55:44:33',
                                                eth_src='00:22:22:22:22:22',
                                                ip_dst='10.10.10.2',
                                                ip_src='11.11.11.2',
                                                ip_dscp=0,
                                                ip_ttl=64)
        exp_pkt = simple_rocev2_bth(eth_dst='00:10:22:33:44:55',
                                                    eth_src='00:77:66:55:44:33',
                                                    ip_dst='10.10.10.2',
                                                    ip_src='11.11.11.2',
                                                    ip_dscp=0,
                                                    ip_ttl=63)

        # Assuming the epoch interval is 2048 ns
        sleep_time_s = 4.0

        def check_packet(pkt_id):
            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            return self.get_suppression_regs(self.suppression_regs)

        try:
            t_cnt = self.get_sfc_counters()
            # Done: reset bank switch register
            filter_size = self.suppression_regs['banks'][0][0].info.size
            all_zeros = Bit(filter_size, value=0, fmt=Bit.Fmt.Hex)
            new_values_zero = {
                'bank_change': 0,
                'bank_idx': 0,
                'banks': {0: {0: all_zeros, 1: all_zeros}, 1: {0: all_zeros, 1: all_zeros}},
            }
            print("Set suppression registers to 0")
            self.set_suppression_regs(self.suppression_regs, new_values_zero)

            results = [dict(ts=time.time(),
                            before_packet=True,
                            value=self.get_suppression_regs(self.suppression_regs))]
            # TODO: set pattern in banks
            self.reset_current_epoch_start(self.suppression_regs)
            time.sleep(sleep_time_s)
            for i in range(3):
                results.append(dict(ts=time.time(),
                                    before_packet=False,
                                    value=check_packet(i)))
            print(results)
            results.append(dict(ts=time.time(),
                                before_packet=True,
                                value=self.get_suppression_regs(self.suppression_regs)))
            self.reset_current_epoch_start(self.suppression_regs)
            time.sleep(sleep_time_s)
            for i in range(3):
                results.append(dict(ts=time.time(),
                                    before_packet=False,
                                    value=check_packet(i)))
            print(results)

            results.append(dict(ts=time.time(),
                                before_packet=True,
                                value=self.get_suppression_regs(self.suppression_regs)))
            self.reset_current_epoch_start(self.suppression_regs)
            time.sleep(sleep_time_s)

            for i in range(3):
                results.append(dict(ts=time.time(),
                                    before_packet=False,
                                    value=check_packet(i)))
            print(results)

            self.sfc_type_cnt_assert(t_cnt,
                                     ingress={SfcPacketType.TNone: dict(pkt=9)},
                                     egress={SfcPacketType.TNone: dict(pkt=9)},
                                     )

            self._validate_results(results)

        finally:
            pass

    @staticmethod
    def _validate_results(results):
        def assert_result(condition, state, last_result, result):
            assert condition, \
                "Assertion failed in state: {}, results: {}".format(
                    "/".join(state),
                    pprint.pformat(dict(last_result=last_result, current_result=result))
                )

        last_result = None
        for i, result in enumerate(results):
            print(last_result, result, i)
            state = ["Result #{}".format(i)]
            if result['before_packet'] and last_result is None:
                pass
            elif result['before_packet'] and last_result is not None:
                state.append("before_packet and last_result")
                assert_result(result['value']['current_epoch_start'] == last_result['value']['current_epoch_start']
                              and result['value']['bank_change'] == 0
                              and result['value']['bank_idx'] == last_result['value']['bank_idx'],
                              state, last_result, result)
            elif not result['before_packet'] and last_result is not None:
                state.append("not before_packet and last_result")
                if not last_result['before_packet'] and (result['ts'] - last_result['ts']) < 2:
                    state.append("last_result is not before_packet and ts diff is < 2")
                    assert_result(result['value']['current_epoch_start'] == last_result['value']['current_epoch_start'],
                                  state, last_result, result)
                    assert_result(result['value']['bank_change'] == 0, state, last_result, result)
                    assert_result(result['value']['bank_idx'] == last_result['value']['bank_idx'], state, last_result,
                                  result)
                else:
                    state.append("last_result is before_packet or ts diff is >= 2")
                    assert_result(result['value']['current_epoch_start'] > last_result['value']['current_epoch_start'],
                                  state, last_result, result)
                    assert_result(
                        (result['value']['current_epoch_start'] - last_result['value']['current_epoch_start']) >= 2.0,
                        state, last_result, result)
                    assert_result(result['value']['bank_change'] == 1, state, last_result, result)
                    assert_result(result['value']['bank_idx'] != last_result['value']['bank_idx'], state, last_result,
                                  result)
            else:
                raise Exception("Illegal state in validate_results.")
            last_result = result


@group('sfc')
@requires_bfrtgrpc()
class SuppressionBankSwitch(SfcTestHelper):
    def runTest(self):
        self.base_l3_setup()
        self.init_sfc_stats()

        self.sfc_config = SfcConfig(
            port_speed=PortSpeed.GbE_400,
            pipes=[0],
            ports=[self.pipe4[0]],
            queues=[0],
            qlength_threshold=200,
            target_queuedepth=200,
            suppression_epoch_duration=2000000,
            mirror_egress_ports=[2] * 4,
            tcs=[],
            dscp_mapping={},
            skip_trigger=True,
            always_trigger=True,
            ignore_suppression=False,
            skip_suppression=False,
            server_ports=self.devports,
            switch_ports=[],
            port_speeds=dict.fromkeys(self.devports, PortSpeed.GbE_25),
            sfc_pause_packet_dscp=1,
            dscp_tc_map={0: 0, 1: 1}

        )
        self.init_sfc(self.sfc_config)

        pkt = simple_rocev2_bth(eth_dst='00:77:66:55:44:33',
                                                eth_src='00:22:22:22:22:22',
                                                ip_dst='10.10.10.2',
                                                ip_src='11.11.11.2',
                                                ip_dscp=0,
                                                ip_ttl=64)
        exp_pkt = simple_rocev2_bth(eth_dst='00:10:22:33:44:55',
                                                    eth_src='00:77:66:55:44:33',
                                                    ip_dst='10.10.10.2',
                                                    ip_src='11.11.11.2',
                                                    ip_dscp=0,
                                                    ip_ttl=63)

        filter_size = self.suppression_regs['banks'][0][0].info.size
        all_zeros = Bit(filter_size, value=0, fmt=Bit.Fmt.Hex)
        all_ones = Bit(filter_size, value=(1, 1), fmt=Bit.Fmt.Hex)

        new_values_zero = {
            'bank_change': 0,
            'bank_idx': 0,
            'banks': {0: {0: all_zeros, 1: all_zeros}, 1: {0: all_zeros, 1: all_zeros}},
        }
        new_values_one = {
            'bank_change': 1,
            'bank_idx': 1,
            'banks': {0: {0: all_ones, 1: all_ones}, 1: {0: all_ones, 1: all_ones}},
        }

        exp_values_after_switch = {
            'bank_change': 1,
            'bank_idx': 0,
            'banks': {0: {0: all_ones, 1: all_ones}, 1: {0: all_zeros, 1: all_zeros}},
        }
        try:
            print("Set suppression registers to 0")
            self.set_suppression_regs(self.suppression_regs, new_values_zero)

            print("Get suppression registers")
            suppression_epoch_regs_expect_zero = self.get_suppression_regs(self.suppression_regs, include_filters=True)

            print("Set suppression registers to 1")
            self.set_suppression_regs(self.suppression_regs, new_values_one)

            print("Get suppression registers")
            suppression_epoch_regs_expect_one = self.get_suppression_regs(self.suppression_regs, include_filters=True)

            print("Get SFC type counters")
            t_cnt = self.get_sfc_counters()

            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            print("Check SFC type counters")
            self.sfc_type_cnt_assert(t_cnt,
                                     ingress={SfcPacketType.TNone: dict(pkt=1)},
                                     egress={SfcPacketType.TNone: dict(pkt=1)},
                                     )

            print("Check initial values of suppression registers")
            suppression_epoch_after_switch = self.get_suppression_regs(self.suppression_regs, include_filters=True)

            exp_zero_diff = self.expect_dict_diff(suppression_epoch_regs_expect_zero, new_values_zero)
            exp_one_diff = self.expect_dict_diff(suppression_epoch_regs_expect_one, new_values_one)
            assert len(exp_zero_diff) == 0, "Writing 0s failed: {}".format(pprint.pformat(exp_zero_diff))
            assert len(exp_one_diff) == 0, "Writing 1s failed: {}".format(pprint.pformat(exp_one_diff))

            print("Check values of suppression registers after bank switch")
            exp_switch_diff = self.expect_dict_diff(suppression_epoch_after_switch, exp_values_after_switch)
            assert len(exp_switch_diff) == 0, "Switching from 1s failed: {}".format(pprint.pformat(exp_switch_diff))
        finally:
            pass


@group('sfc')
@requires_bfrtgrpc()
class SuppressionTest(SfcTestHelper):
    def runTest(self):
        self.base_l3_setup()
        self.init_sfc_stats()

        self.sfc_config = SfcConfig(
            port_speed=PortSpeed.GbE_400,
            pipes=[0],
            ports=[self.pipe4[0]],
            queues=[0],
            qlength_threshold=200,
            target_queuedepth=200,
            suppression_epoch_duration=2000000,
            mirror_egress_ports=[2] * 4,
            tcs=[0],
            dscp_mapping={},
            skip_trigger=False,
            always_trigger=False,
            ignore_suppression=False,
            skip_suppression=False,
            server_ports=self.devports,
            switch_ports=[],
            port_speeds=dict.fromkeys(self.devports, PortSpeed.GbE_25),
            sfc_pause_packet_dscp=1,
            dscp_tc_map={0: 0, 1: 1}
        )
        self.init_sfc(self.sfc_config)

        pkt = simple_rocev2_bth(eth_dst='00:77:66:55:44:33',
                                                eth_src='00:22:22:22:22:22',
                                                ip_dst='10.10.10.2',
                                                ip_src='11.11.11.2',
                                                ip_dscp=0,
                                                ip_ttl=64)
        exp_pkt = simple_rocev2_bth(eth_dst='00:10:22:33:44:55',
                                                    eth_src='00:77:66:55:44:33',
                                                    ip_dst='10.10.10.2',
                                                    ip_src='11.11.11.2',
                                                    ip_dscp=0,
                                                    ip_ttl=63)

        filter_size = self.suppression_regs['banks'][0][0].info.size
        all_zeros = Bit(filter_size, value=0, fmt=Bit.Fmt.Hex)

        ghost_reg_size = self.ghost_regs['qdepth'].info.size
        ghost_all_zeros = Bit(ghost_reg_size, value=0, fmt=Bit.Fmt.Hex)
        ghost_all_ones = Bit(ghost_reg_size, value=(1, 1), fmt=Bit.Fmt.Hex)

        new_values_zero = {
            'bank_change': 0,
            'bank_idx': 0,
            'banks': {0: {0: all_zeros, 1: all_zeros}, 1: {0: all_zeros, 1: all_zeros}},
        }

        try:
            print("Set ghost over_threshold registers to 1")
            self.set_ghost_regs(self.ghost_regs, ghost_all_ones)
            ghost_val = self.get_ghost_regs(self.ghost_regs)
            assert ghost_val == ghost_all_ones, "Writing 1s to ghost registers failed."

            print("Set suppression registers to 0")
            self.set_suppression_regs(self.suppression_regs, new_values_zero)

            print("Get suppression registers")
            suppression_epoch_regs_expect_zero = self.get_suppression_regs(self.suppression_regs, include_filters=True)

            print("Get SFC type counters")
            t_cnt = self.get_sfc_counters()

            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            print("Check suppression registers")
            suppression_epoch_after_switch = self.get_suppression_regs(self.suppression_regs, include_filters=True)

            exp_zero_diff = self.expect_dict_diff(suppression_epoch_regs_expect_zero, new_values_zero)
            assert len(exp_zero_diff) == 0, "Writing 0s failed: {}".format(pprint.pformat(exp_zero_diff))

            # suppression_epoch_after_switch
            print("  Check if bits were set in bloom filter")
            bank_idx = suppression_epoch_after_switch['bank_idx']
            assert suppression_epoch_after_switch['banks'][bank_idx][0].popcnt() == 1
            assert suppression_epoch_after_switch['banks'][bank_idx][1].popcnt() == 1

            print("Check SFC type counters")
            self.sfc_type_cnt_assert(t_cnt,
                                     ingress={SfcPacketType.Data: dict(pkt=3)},
                                     egress={SfcPacketType.Data: dict(pkt=3), SfcPacketType.Trigger: dict(pkt=1)},
                                     )
        finally:
            try:
                self.set_ghost_regs(self.ghost_regs, ghost_all_zeros)
            except:
                pass


# TODO: complete test once the ghost thread feature is available in the model
@disabled
# @group('sfc')
# @requires_bfrtgrpc()
class GhostRead(SfcTestHelper):
    def runTest(self):
        self.base_l3_setup()
        self.init_sfc_stats()

        self.sfc_config = SfcConfig(
            port_speed=PortSpeed.GbE_400,
            pipes=[0],
            ports=[self.pipe4[0]],
            queues=[0],
            qlength_threshold=200,
            target_queuedepth=200,
            suppression_epoch_duration=2000000,
            mirror_egress_ports=[2] * 4,
            tcs=[0],
            dscp_mapping={},
            skip_trigger=False,
            always_trigger=False,
            ignore_suppression=True,
            skip_suppression=False,
            server_ports=self.devports,
            switch_ports=[],
            port_speeds=dict.fromkeys(self.devports, PortSpeed.GbE_25),
            sfc_pause_packet_dscp=1,
            dscp_tc_map={0: 0, 1: 1}
        )
        self.init_sfc(self.sfc_config)

        ghost_reg_size = self.ghost_regs['qdepth'].info.size
        all_zeros = Bit(ghost_reg_size, value=0, fmt=Bit.Fmt.Hex)
        all_ones = Bit(ghost_reg_size, value=(1, 1), fmt=Bit.Fmt.Hex)

        pkt = simple_rocev2_bth(eth_dst='00:77:66:55:44:33',
                                                eth_src='00:22:22:22:22:22',
                                                ip_dst='10.10.10.2',
                                                ip_src='11.11.11.2',
                                                ip_dscp=0,
                                                ip_ttl=64)
        exp_pkt = simple_rocev2_bth(eth_dst='00:10:22:33:44:55',
                                                    eth_src='00:77:66:55:44:33',
                                                    ip_dst='10.10.10.2',
                                                    ip_src='11.11.11.2',
                                                    ip_dscp=0,
                                                    ip_ttl=63)
        try:
            print("Ghost read.")
            self.set_ghost_regs(self.ghost_regs, all_zeros)
            ghost_val = self.get_ghost_regs(self.ghost_regs)
            assert ghost_val == all_zeros, "Writing 0s to ghost registers failed."

            self.set_ghost_regs(self.ghost_regs, all_ones)
            ghost_val = self.get_ghost_regs(self.ghost_regs)
            assert ghost_val == all_ones, "Writing 1s to ghost registers failed."

            t_cnt = self.get_sfc_counters()

            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            self.sfc_type_cnt_assert(t_cnt,
                                     ingress=dict(data=dict(pkt=1)),
                                     egress=dict(data=dict(pkt=1), trigger=dict(pkt=1)),
                                     )
            print("Done")
        finally:
            pass


@disabled
# @group('sfc')
# @requires_bfrtgrpc()
class EgressQueuedepthProgramming(SfcTestHelper):
    """
    This test ensures that queue depth values stored in reg_qd_egress are picked up correctly.
    It does not check the correct deposition of the values from egress packets.
    For the latter case, an additional test is required that will only run on the hardware for now.
    """

    def runTest(self):
        self.base_l3_setup()
        self.init_sfc_stats()

        self.sfc_config = SfcConfig(
            port_speed=PortSpeed.GbE_400,
            pipes=[0],
            ports=[self.pipe4[0]],
            queues=[0],
            qlength_threshold=200,
            target_queuedepth=200,
            suppression_epoch_duration=2000000,
            mirror_egress_ports=[2] * 4,
            tcs=[0],
            dscp_mapping={},
            skip_trigger=True,
            always_trigger=False,
            ignore_suppression=False,
            skip_suppression=True,
            server_ports=self.devports,
            switch_ports=[],
            port_speeds=dict.fromkeys(self.devports, PortSpeed.GbE_25),
            sfc_pause_packet_dscp=1,
            dscp_tc_map={0: 0, 1: 1}
        )
        self.init_sfc(self.sfc_config)

        pkt = simple_rocev2_bth(eth_dst='00:77:66:55:44:33',
                                                eth_src='00:22:22:22:22:22',
                                                ip_dst='10.10.10.2',
                                                ip_src='11.11.11.2',
                                                ip_dscp=0,
                                                ip_ttl=64)
        exp_pkt = simple_rocev2_bth(eth_dst='00:10:22:33:44:55',
                                                    eth_src='00:77:66:55:44:33',
                                                    ip_dst='10.10.10.2',
                                                    ip_src='11.11.11.2',
                                                    ip_dscp=0,
                                                    ip_ttl=63)
        try:
            target_drain_override = self.sfc_config.qlength_threshold
            target_drain_prevent_override = (1 << 31) + self.sfc_config.qlength_threshold
            print("target_drain_prevent_override={}".format(target_drain_prevent_override))

            t_cnt = self.get_sfc_counters()

            # Test if the normal value get overridden
            self.set_egress_qdepth_regs(self.sfc_config, self.egress_regs, target_drain_override)
            egress_qdepth_regs_override = self.get_egress_qdepth_regs(self.egress_regs)
            diff_qd_regs = self.expect_dict_diff(egress_qdepth_regs_override,
                                                 {0: {'qdepth_drain_cells': target_drain_override,
                                                      'target_qdepth': self.sfc_config.qlength_threshold}})
            assert len(diff_qd_regs) == 0, \
                "Programming egress queue depth register array failed: {}".format(diff_qd_regs)

            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            # Ensure that the data packet did override the qdepth in the register
            egress_qdepth_regs_overridden = self.get_egress_qdepth_regs(self.egress_regs)
            diff_qd_regs = self.expect_dict_diff(egress_qdepth_regs_overridden,
                                                 {0: {'qdepth_drain_cells': 0,
                                                      'target_qdepth': self.sfc_config.qlength_threshold}})
            assert len(diff_qd_regs) == 0, \
                "Overriding egress queue depth register array failed: {}".format(diff_qd_regs)

            # Now test if the override protection makes the qdepth_drain_cells value stick
            self.set_egress_qdepth_regs(self.sfc_config, self.egress_regs, target_drain_prevent_override)
            egress_qdepth_sticky = self.get_egress_qdepth_regs(self.egress_regs)
            diff_qd_regs = self.expect_dict_diff(egress_qdepth_sticky,
                                                 {0: {'qdepth_drain_cells': target_drain_prevent_override,
                                                      'target_qdepth': self.sfc_config.qlength_threshold}})
            assert len(diff_qd_regs) == 0, \
                "Programming egress queue depth register array failed: {}".format(diff_qd_regs)

            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            # Ensure that the data packet did not override the qdepth in the register
            egress_qdepth_still_sticky = self.get_egress_qdepth_regs(self.egress_regs)
            diff_qd_regs = self.expect_dict_diff(egress_qdepth_still_sticky,
                                                 {0: {'qdepth_drain_cells': target_drain_prevent_override,
                                                      'target_qdepth': self.sfc_config.qlength_threshold}})
            assert len(diff_qd_regs) == 0, \
                "Override prevention in egress queue depth register array failed: {}".format(diff_qd_regs)

            self.sfc_type_cnt_assert(t_cnt,
                                     ingress={SfcPacketType.Data: dict(pkt=2)},
                                     egress={SfcPacketType.Data: dict(pkt=2)},
                                     )
        finally:
            pass
