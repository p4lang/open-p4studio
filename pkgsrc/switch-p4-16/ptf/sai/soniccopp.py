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

#
# saicopp.${name_test}
#
# ARPTest
# DHCPTest
# LLDPTest
# BGPTest
# LACPTest
# SNMPTest
# SSHTest
# IP2METest
# DefaultTest

import ptf
from ptf.base_tests import BaseTest
from ptf import config
import ptf.testutils as testutils
from ptf.testutils import *
from ptf.packet import *
from ptf.dataplane import match_exp_pkt
import os
import signal
import datetime
import subprocess
import threading

import sai_base_test
import switchsai_thrift.switch_sai_rpc as switch_sai_rpc
from switchsai_thrift.sai_headers import  *
from switch_utils import *

class ControlPlaneBaseTest(sai_base_test.ThriftInterfaceDataPlane):
    MAX_PORTS = 128
    PPS_LIMIT = 600
    PPS_LIMIT_MIN = PPS_LIMIT * 0.9
    PPS_LIMIT_MAX = PPS_LIMIT * 1.1
    NO_POLICER_LIMIT = PPS_LIMIT * 1.4
    PKT_TX_COUNT = 100000
    TARGET_PORT = "3"  # historically we have port 3 as a target port
    TASK_TIMEOUT = 300 # Wait up to 5 minutes for tasks to complete
    POLICER_CIR = 600
    POLICER_CBS = 600

    trap_list = [
        SAI_HOSTIF_TRAP_TYPE_TTL_ERROR,
        SAI_HOSTIF_TRAP_TYPE_BGP,
        SAI_HOSTIF_TRAP_TYPE_LACP,
        SAI_HOSTIF_TRAP_TYPE_ARP_REQUEST,
        SAI_HOSTIF_TRAP_TYPE_ARP_RESPONSE,
        SAI_HOSTIF_TRAP_TYPE_LLDP,
        SAI_HOSTIF_TRAP_TYPE_DHCP,
        SAI_HOSTIF_TRAP_TYPE_IP2ME
        ]
    traps = []
    hostif_id = {}

    def __init__(self):
        BaseTest.__init__(self)
        self.log_fp = open('/tmp/copp.log', 'a')
        test_params = testutils.test_params_get()
        self.verbose = 'verbose' in test_params and test_params['verbose']

        self.pkt_tx_count = test_params.get('pkt_tx_count', self.PKT_TX_COUNT)
        if self.pkt_tx_count == 0:
            self.pkt_tx_count = self.PKT_TX_COUNT
        self.pkt_rx_limit = self.pkt_tx_count * 0.90

        target_port_str = test_params.get('test_port', self.TARGET_PORT)
        self.target_port = int(target_port_str)

        self.target_arch = test_params.get('arch', 'tofino')

        self.run_target = test_params.get('target', 'hw')
        print "Arch %s, Target %s, Port %d" % (self.target_arch, self.run_target, self.target_port)

        print "Packet tx count %d" % self.PKT_TX_COUNT

        self.timeout_thr = None

        self.myip = {}
        self.peerip = {}
        for i in xrange(self.MAX_PORTS):
            self.myip[i] = "10.0.0.%d" % (i*2+1)
            self.peerip[i] = "10.0.0.%d" % (i*2)

        return

    def log(self, message, debug=False):
        current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        if (debug and self.verbose) or (not debug):
            print "%s : %s" % (current_time, message)
        self.log_fp.write("%s : %s\n" % (current_time, message))

    def setUp(self):
        ThriftInterfaceDataPlane.setUp(self)
        self.dataplane = ptf.dataplane_instance

        self.my_mac = {}
        self.peer_mac = {}
        for port_id, port in self.dataplane.ports.iteritems():
            if port_id[0] == 0:
                self.my_mac[port_id[1]] = port.mac()
            elif port_id[0] == 1:
                self.peer_mac[port_id[1]] = port.mac()
            else:
                assert True

        self.dataplane.flush()
        if config["log_dir"] != None:
            filename = os.path.join(config["log_dir"], str(self)) + ".pcap"
            self.dataplane.start_pcap(filename)

        switch_init(self.client)
        for interface,front in interface_to_front_mapping.iteritems():
            port_id = port_list[int(interface)]
            hif_id = sai_thrift_create_hostif(self.client, SAI_HOSTIF_TYPE_NETDEV, port_id, front)
            self.hostif_id[front]=hif_id

        self.sai_policer_id = sai_thrift_create_policer(self.client,
                                                   meter_type=SAI_METER_TYPE_PACKETS,
                                                   mode=SAI_POLICER_MODE_SR_TCM,
                                                   cir=self.POLICER_CIR,
                                                   cbs=self.POLICER_CBS,
                                                   pir=0,
                                                   pbs=0,
                                                   red_action=SAI_PACKET_ACTION_DROP)

        self.trap_group = sai_thrift_create_hostif_trap_group(self.client, queue_id=4, policer_id=self.sai_policer_id)

        for trap_id in self.trap_list:
            trap1 = sai_thrift_create_hostif_trap(client=self.client,
                                              trap_type=trap_id,
                                              packet_action=SAI_PACKET_ACTION_TRAP,
                                              trap_group=self.trap_group)
            self.traps.append(trap1)

    def tearDown(self):
        for trap in self.traps:
            sai_thrift_remove_hostif_trap(self.client, trap)
        sai_thrift_remove_hostif_trap_group(self.client, self.trap_group)
        self.client.sai_thrift_remove_policer(thrift_policer_id=self.sai_policer_id)

        if config["log_dir"] != None:
            self.dataplane.stop_pcap()
        self.log_fp.close()

        for interface,front in interface_to_front_mapping.iteritems():
            self.client.sai_thrift_remove_hostif(self.hostif_id[front])
        ThriftInterfaceDataPlane.tearDown(self)

    def timeout(self, seconds, message):
        def timeout_exception(self, message):
            self.log('Timeout is reached: %s' % message)
            self.tearDown()
            os.kill(os.getpid(), signal.SIGINT)

        if self.timeout_thr is None:
            self.timeout_thr = threading.Timer(seconds, timeout_exception, args=(self, message))
            self.timeout_thr.start()
        else:
            raise Exception("Timeout already set")

    def cancel_timeout(self):
        if self.timeout_thr is not None:
            self.timeout_thr.cancel()
            self.timeout_thr = None

    def copp_test(self, packet, count, send_intf, recv_intf):
        '''
        Pre-send some packets for a second to absorb the CBS capacity.
        '''
        if self.needPreSend:
            sendInFirst=0
            endTime = datetime.datetime.now() + datetime.timedelta(seconds=1)
            while datetime.datetime.now() < endTime:
                testutils.send_packet(self, send_intf, packet)
                sendInFirst += 1
            rcv_pkt_cnt = testutils.count_matched_packets(self, packet, recv_intf[1], recv_intf[0],timeout=0.01)
            self.log("Send %d and receive %d packets in the first second (PolicyTest)" % (sendInFirst,  rcv_pkt_cnt))
            self.dataplane.flush()

        b_c_0 = self.dataplane.get_counters(*send_intf)
        b_c_1 = self.dataplane.get_counters(*recv_intf)
        b_n_0 = self.dataplane.get_nn_counters(*send_intf)
        b_n_1 = self.dataplane.get_nn_counters(*recv_intf)

        start_time=datetime.datetime.now()

        for i in xrange(count):
            testutils.send_packet(self, send_intf, packet)

        end_time=datetime.datetime.now()

        total_rcv_pkt_cnt = testutils.count_matched_packets(self, packet, recv_intf[1], recv_intf[0])

        e_c_0 = self.dataplane.get_counters(*send_intf)
        e_c_1 = self.dataplane.get_counters(*recv_intf)
        e_n_0 = self.dataplane.get_nn_counters(*send_intf)
        e_n_1 = self.dataplane.get_nn_counters(*recv_intf)
        self.log("", True)
        self.log("Counters before the test:", True)
        self.log("If counter (0, n): %s" % str(b_c_0), True)
        self.log("NN counter (0, n): %s" % str(b_n_0), True)
        self.log("If counter (1, n): %s" % str(b_c_1), True)
        self.log("NN counter (1, n): %s" % str(b_n_1), True)
        self.log("", True)
        self.log("Counters after the test:", True)
        self.log("If counter (0, n): %s" % str(e_c_0), True)
        self.log("NN counter (0, n): %s" % str(e_n_0), True)
        self.log("If counter (1, n): %s" % str(e_c_1), True)
        self.log("NN counter (1, n): %s" % str(e_n_1), True)
        self.log("")
        self.log("Sent through NN to local ptf_nn_agent:    %d" % int(e_c_0[1] - b_c_0[1]))
        self.log("Sent through If to remote ptf_nn_agent:   %d" % int(e_n_0[1] - b_n_0[1]))
        self.log("Recv from If on remote ptf_nn_agent:      %d" % int(e_c_1[0] - b_c_1[0]))
        self.log("Recv from NN on from remote ptf_nn_agent: %d" % int(e_n_1[0] - b_n_1[0]))

        time_delta = end_time - start_time
        time_delta_ms = (time_delta.microseconds + time_delta.seconds * 10**6) / 10**3
        tx_pps = int(count/(float(time_delta_ms)/1000))
        rx_pps = int(total_rcv_pkt_cnt/(float(time_delta_ms)/1000))

        return total_rcv_pkt_cnt, time_delta, time_delta_ms, tx_pps, rx_pps

    def contruct_packet(self, port_number):
        raise NotImplemented

    def check_constraints(self, total_rcv_pkt_cnt, time_delta_ms, rx_pps):
        raise NotImplemented

    def one_port_test(self, port_number):
        packet = self.contruct_packet(port_number)
        total_rcv_pkt_cnt, time_delta, time_delta_ms, tx_pps, rx_pps = self.copp_test(str(packet), self.pkt_tx_count, (0, port_number), (0, port_number))
        self.printStats(self.pkt_tx_count, total_rcv_pkt_cnt, time_delta, tx_pps, rx_pps)
        self.check_constraints(total_rcv_pkt_cnt, time_delta_ms, rx_pps)

        return

    def run_suite(self):
        if self.run_target != 'hw':
            print "Test only to run on hardware, returning"
            return
        self.timeout(self.TASK_TIMEOUT, "The test case hasn't been completed in %d seconds" % self.TASK_TIMEOUT) # FIXME: better make it decorator
        self.one_port_test(self.target_port)
        self.cancel_timeout()

    def printStats(self, pkt_send_count, total_rcv_pkt_cnt, time_delta, tx_pps, rx_pps):
        self.log("")
        self.log('test stats')
        self.log('Packet sent = %10d' % pkt_send_count)
        self.log('Packet rcvd = %10d' % total_rcv_pkt_cnt)
        self.log('Test time = %s' % str(time_delta))
        self.log('TX PPS = %d' % tx_pps)
        self.log('RX PPS = %d' % rx_pps)

        return

class NoPolicyTest(ControlPlaneBaseTest):
    def __init__(self):
        ControlPlaneBaseTest.__init__(self)
        self.needPreSend=False

    def check_constraints(self, total_rcv_pkt_cnt, time_delta_ms, rx_pps):
        self.log("")
        self.log("Checking constraints (NoPolicy):")
        self.log("rx_pps (%d) > NO_POLICER_LIMIT (%d): %s" % (int(rx_pps), int(self.NO_POLICER_LIMIT), str(rx_pps > self.NO_POLICER_LIMIT)))
        self.log("total_rcv_pkt_cnt (%d) > pkt_rx_limit (%d): %s" % \
                (int(total_rcv_pkt_cnt), int(self.pkt_rx_limit), str(total_rcv_pkt_cnt > self.pkt_rx_limit)))

        assert(rx_pps > self.NO_POLICER_LIMIT)
        assert(total_rcv_pkt_cnt > self.pkt_rx_limit)

class PolicyTest(ControlPlaneBaseTest):
    def __init__(self):
        ControlPlaneBaseTest.__init__(self)
        self.needPreSend=True

    def check_constraints(self, total_rcv_pkt_cnt, time_delta_ms, rx_pps):
        self.log("")
        self.log("Checking constraints (PolicyApplied):")
        self.log("PPS_LIMIT_MIN (%d) <= rx_pps (%d) <= PPS_LIMIT_MAX (%d): %s" % \
                (int(self.PPS_LIMIT_MIN), int(rx_pps), int(self.PPS_LIMIT_MAX), str(self.PPS_LIMIT_MIN <= rx_pps <= self.PPS_LIMIT_MAX)))

        assert(self.PPS_LIMIT_MIN <= rx_pps <= self.PPS_LIMIT_MAX)


# SONIC config contains policer CIR=600 for ARP
class ARPTest(PolicyTest):
    def __init__(self):
        PolicyTest.__init__(self)

    def runTest(self):
        self.log("ARPTest")
        self.run_suite()

    def contruct_packet(self, port_number):
        src_mac = self.my_mac[port_number]
        src_ip = self.myip[port_number]
        dst_ip = self.peerip[port_number]

        packet = simple_arp_packet(
                       eth_dst='ff:ff:ff:ff:ff:ff',
                       eth_src=src_mac,
                       arp_op=1,
                       ip_snd=src_ip,
                       ip_tgt=dst_ip,
                       hw_snd=src_mac,
                       hw_tgt='ff:ff:ff:ff:ff:ff')

        return packet

# SONIC configuration has no policer limiting for DHCP
class DHCPTest(NoPolicyTest):
    def __init__(self):
        NoPolicyTest.__init__(self)

    def runTest(self):
        self.log("DHCPTest")
        self.run_suite()

    def contruct_packet(self, port_number):
        src_mac = self.my_mac[port_number]
        packet = simple_udp_packet(pktlen=100,
                          eth_dst='ff:ff:ff:ff:ff:ff',
                          eth_src=src_mac,
                          dl_vlan_enable=False,
                          vlan_vid=0,
                          vlan_pcp=0,
                          dl_vlan_cfi=0,
                          ip_src='0.0.0.0',
                          ip_dst='255.255.255.255',
                          ip_tos=0,
                          ip_ttl=64,
                          udp_sport=68,
                          udp_dport=67,
                          ip_ihl=None,
                          ip_options=False,
                          with_udp_chksum=True
                          )

        return packet


# SONIC configuration has no policer limiting for LLDP
class LLDPTest(NoPolicyTest):
    def __init__(self):
        NoPolicyTest.__init__(self)

    def runTest(self):
        self.log("LLDPTest")
        self.run_suite()

    def contruct_packet(self, port_number):
        src_mac = self.my_mac[port_number]
        packet = simple_eth_packet(
                       eth_dst='01:80:c2:00:00:0e',
                       eth_src=src_mac,
                       eth_type=0x88cc
                 )
        return packet

# SONIC configuration has no policer limiting for UDLD
class UDLDTest(NoPolicyTest):
    def __init__(self):
        NoPolicyTest.__init__(self)

    def runTest(self):
        self.log("UDLDTest")
        self.run_suite()

    # UDLD uses Ethernet multicast address 01-00-0c-cc-cc-cc
    # as its destination MAC address. eth_type is to indicate
    # the length of the data in Ethernet 802.3 frame. pktlen
    # = 117 = 103 (0x67) + 6 (dst MAC) + 6 (dst MAC) + 2 (len)
    def contruct_packet(self, port_number):
        src_mac = self.my_mac[port_number]
        packet = simple_eth_packet(
                       pktlen=117,
                       eth_dst='01:00:0c:cc:cc:cc',
                       eth_src=src_mac,
                       eth_type=0x0067
                 )
        return packet

# SONIC configuration has no policer limiting for BGP
class BGPTest(NoPolicyTest):
    def __init__(self):
        NoPolicyTest.__init__(self)

    def runTest(self):
        self.log("BGPTest")
        self.run_suite()

    def contruct_packet(self, port_number):
        dst_mac = self.peer_mac[port_number]
        dst_ip = self.peerip[port_number]
        packet = simple_tcp_packet(
                      eth_dst=dst_mac,
                      ip_dst=dst_ip,
                      ip_ttl=1,
                      tcp_dport=179
                      )
        return packet

# SONIC configuration has no policer limiting for LACP
class LACPTest(NoPolicyTest):
    def __init__(self):
        NoPolicyTest.__init__(self)

    def runTest(self):
        self.log("LACPTest")
        self.run_suite()

    def contruct_packet(self, port_number):
        packet = simple_eth_packet(
               pktlen=14,
               eth_dst='01:80:c2:00:00:02',
               eth_type=0x8809
               ) / (chr(0x01)*50)

        return packet

# SNMP packets are trapped as IP2ME packets.
# IP2ME configuration in SONIC contains policer CIR=600
class SNMPTest(PolicyTest): #FIXME: trapped as ip2me. mellanox should add support for SNMP trap
    def __init__(self):
        PolicyTest.__init__(self)

    def runTest(self):
        self.log("SNMPTest")
        self.run_suite()

    def contruct_packet(self, port_number):
        src_mac = self.my_mac[port_number]
        dst_mac = self.peer_mac[port_number]
        dst_ip = self.peerip[port_number]
        packet = simple_udp_packet(
                          eth_dst=dst_mac,
                          ip_dst=dst_ip,
                          eth_src=src_mac,
                          udp_dport=161
                          )
        return packet

# SONIC configuration has no policer limiting for SSH
class SSHTest(PolicyTest): # FIXME: ssh is policed now
    def __init__(self):
        PolicyTest.__init__(self)

    def runTest(self):
        self.log("SSHTest")
        self.run_suite()

    def contruct_packet(self, port_number):
        dst_mac = self.peer_mac[port_number]
        src_ip = self.myip[port_number]
        dst_ip = self.peerip[port_number]

        packet = simple_tcp_packet(
                eth_dst=dst_mac,
                ip_dst=dst_ip,
                ip_src=src_ip,
                tcp_flags='F',
                tcp_sport=22,
                tcp_dport=22)

        return packet

# IP2ME configuration in SONIC contains policer CIR=600
class IP2METest(PolicyTest):
    def __init__(self):
        PolicyTest.__init__(self)

    def runTest(self):
        self.log("IP2METest")
        self.run_suite()

    def one_port_test(self, port_number):
        for port in self.dataplane.ports.iterkeys():
            if port[0] == 0:
                continue
            packet = self.contruct_packet(port[1])
            total_rcv_pkt_cnt, time_delta, time_delta_ms, tx_pps, rx_pps = self.copp_test(str(packet), self.pkt_tx_count, (0, port_number), (1, port_number))
            self.printStats(self.pkt_tx_count, total_rcv_pkt_cnt, time_delta, tx_pps, rx_pps)
            self.check_constraints(total_rcv_pkt_cnt, time_delta_ms, rx_pps)

        return

    def contruct_packet(self, port_number):
        src_mac = self.my_mac[port_number]
        dst_mac = self.peer_mac[port_number]
        dst_ip = self.peerip[port_number]

        packet = simple_tcp_packet(
                      eth_src=src_mac,
                      eth_dst=dst_mac,
                      ip_dst=dst_ip
                      )

        return packet


class DefaultTest(PolicyTest):
    def __init__(self):
        PolicyTest.__init__(self)

    def runTest(self):
        self.log("DefaultTest")
        self.run_suite()

    def contruct_packet(self, port_number):
        dst_mac = self.peer_mac[port_number]
        src_ip = self.myip[port_number]
        dst_port_number = (port_number + 1) % self.MAX_PORTS
        dst_ip = self.peerip[dst_port_number]

        packet = simple_tcp_packet(
                eth_dst=dst_mac,
                ip_dst=dst_ip,
                ip_src=src_ip,
                tcp_sport=10000,
                tcp_dport=10000,
                ip_ttl=1)

        return packet
