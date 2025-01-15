import ptf
from ptf.base_tests import BaseTest
from ptf.mask import Mask
from ptf import config
import ptf.testutils as testutils


class DataplaneBaseTest(BaseTest):
    def __init__(self):
        BaseTest.__init__(self)

    def setUp(self):
        self.dataplane = ptf.dataplane_instance
        self.dataplane.flush()
        if config["log_dir"] != None:
            filename = os.path.join(config["log_dir"], str(self)) + ".pcap"
            self.dataplane.start_pcap(filename)

    def tearDown(self):
        if config["log_dir"] != None:
            self.dataplane.stop_pcap()


class OneTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pkt = "ab" * 20
        pkt = pkt.encode()
        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_packet(self, pkt, (1, 1))


class GetMacTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        def check_mac(device, port):
            mac = self.dataplane.get_mac(device, port)
            self.assertIsNotNone(mac)
            self.assertEqual(mac.decode().count(":"), 5)

        check_mac(0, 1)
        pkt = "ab" * 20
        pkt = pkt.encode()
        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_packet(self, pkt, (1, 1))
        check_mac(1, 1)


class GetCountersTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        def check_counters(device, port):
            counters = self.dataplane.get_nn_counters(device, port)
            self.assertIsNotNone(counters)
            self.assertTrue(type(counters) is tuple)
            self.assertEqual(len(counters), 2)

            return counters

        counters_01_b = check_counters(0, 1)
        counters_11_b = check_counters(1, 1)
        print("Counters:")
        print(" (0, 1) %d:%d" % counters_01_b)
        print(" (1, 1) %d:%d" % counters_11_b)
        pkt = "ab" * 20
        pkt = pkt.encode()
        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_packet(self, pkt, (1, 1))
        counters_01_e = check_counters(0, 1)
        counters_11_e = check_counters(1, 1)
        print("Counters:")
        print(" (0, 1) %d:%d" % counters_01_e)
        print(" (1, 1) %d:%d" % counters_11_e)
        self.assertTrue(counters_01_e[1] > counters_01_b[1])
        self.assertTrue(counters_11_e[0] > counters_11_b[0])


class VerifyAnyPacketAnyPort(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pkt = "ab" * 20
        pkt = pkt.encode()

        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_any_packet_any_port(
            self, pkts=[pkt], ports=[3, 1], device_number=1
        )

        # negative test: if the packet is indeed received, but not on one of the
        # expected ports, the test should fail
        with self.assertRaises(AssertionError):
            testutils.send_packet(self, (0, 1), pkt)
            print("packet sent")
            testutils.verify_any_packet_any_port(
                self, pkts=[pkt], ports=[0, 2, 3], device_number=1
            )

        print("Verify masked packets")
        pkt1 = testutils.simple_udp_packet(eth_dst="00:11:11:11:11:11")
        pkt2 = testutils.simple_udp_packet(eth_dst="00:22:22:22:22:22")
        exp_pkt = Mask(pkt2)
        exp_pkt.set_do_not_care_packet(Ether, "dst")

        testutils.send_packet(self, (0, 1), pkt1)
        print("Packet sent")
        # pkt2 will not be received
        # pkt2 with masked eth_dst field will match
        testutils.verify_any_packet_any_port(
            self, pkts=[pkt2, exp_pkt], ports=[0, 1], device_number=1
        )

        # negative tests
        with self.assertRaises(AssertionError):
            testutils.send_packet(self, (0, 1), pkt1)
            print("Packet sent")
            # incorrect ports
            testutils.verify_any_packet_any_port(
                self, pkts=[exp_pkt], ports=[0, 2, 3], device_number=1
            )

        with self.assertRaises(AssertionError):
            testutils.send_packet(self, (0, 1), pkt1)
            print("Packet sent")
            # incorrect packet
            testutils.verify_any_packet_any_port(
                self, pkts=[pkt2], ports=[0, 1], device_number=1
            )


class RemovePort(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pkt = "ab" * 20
        pkt = pkt.encode()

        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_packet(self, pkt, (1, 1))

        # We remove a port to test port_remove, but in order to execute
        # subsequent tests, we need to make sure we re-add the port
        # afterwards. In order to re-add the port, we need the interface name,
        # which is what this method is for. This is a little hacky but fine for
        # testing. In practice, you would not be removing ports which are part
        # of the original ptf config.
        def find_ifname(device_number, port_number):
            for port_id, ifname in config["port_map"].items():
                if (device_number, port_number) == port_id:
                    return ifname

        ifname = find_ifname(1, 1)
        self.assertTrue(self.dataplane.port_remove(1, 1))
        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_no_other_packets(self, device_number=1)

        self.dataplane.port_add(ifname, 1, 1)
        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_packet(self, pkt, (1, 1))
        testutils.verify_no_other_packets(self, 1)


class SimpleTcpPacketTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pktlen = 400
        pkt = testutils.simple_tcp_packet(pktlen=pktlen)
        self.assertEqual(len(pkt), pktlen)
        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_packet(self, pkt, (1, 1))
        testutils.verify_no_other_packets(self, 1)


class SimpleIpv4PacketTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pktlen = 70
        pkt = testutils.simple_ipv4ip_packet(pktlen=pktlen)
        self.assertEqual(len(pkt), pktlen)
        testutils.send_packet(self, (0, 1), pkt)

        print("packet sent")
        testutils.verify_packet(self, pkt, (1, 1))
        testutils.verify_no_other_packets(self, 1)


class SimpleIpv6PacketTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pktlen = 400
        pkt = testutils.simple_ipv6ip_packet(pktlen=pktlen)
        self.assertEqual(len(pkt), pktlen)
        testutils.send_packet(self, (0, 1), pkt)
        print("packet sent")
        testutils.verify_packet(self, pkt, (1, 1))
        testutils.verify_no_other_packets(self, 1)


class Ipv4InIpv4PacketTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pktlen = 70
        pkt = testutils.simple_ipv4ip_packet(pktlen=pktlen)
        pkt2 = testutils.simple_ipv4ip_packet(pktlen=pktlen, inner_frame=pkt["IP"])

        testutils.send_packet(self, (0, 1), pkt2)
        print("packet sent")
        testutils.verify_packet(self, pkt2, (1, 1))
        testutils.verify_no_other_packets(self, 1)


class Ipv6InGREPacketTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pktlen = 1000
        udp = testutils.simple_udp_packet()
        ipv6 = testutils.simple_ipv6ip_packet(inner_frame=udp["UDP"])
        gre = testutils.simple_grev6_packet(pktlen=pktlen, inner_frame=ipv6["IPv6"])

        self.assertEqual(gre["GRE"].proto, 0x86DD)
        testutils.send_packet(self, (0, 1), gre)
        print("packet sent")
        testutils.verify_packet(self, gre, (1, 1))
        testutils.verify_no_other_packets(self, 1)


class VerifyPacketsOnMultiplePortListsTest(DataplaneBaseTest):
    def __init__(self):
        DataplaneBaseTest.__init__(self)

    def runTest(self):
        pkt1 = testutils.simple_udp_packet(eth_dst="00:11:11:11:11:11")
        pkt2 = testutils.simple_udp_packet(eth_dst="00:22:22:22:22:22")

        testutils.send_packet(self, (0, 1), pkt1)
        testutils.send_packet(self, (0, 1), pkt2)
        print("Packets sent")
        # pkt1 will be received on one of ports (0, 1)
        # pkt2 will be received on one of ports (1, 2, 3)
        testutils.verify_each_packet_on_multiple_port_lists(
            self, pkts=[pkt1, pkt2], ports=[[0, 1], [1, 2, 3]], device_number=1
        )

        # negative test
        with self.assertRaises(AssertionError):
            testutils.send_packet(self, (0, 1), pkt1)
            testutils.send_packet(self, (0, 1), pkt2)
            print("Packets sent")
            # pkt1 will not be received on one of ports (0, 2, 3)
            # pkt1 will not be received on one of ports (1, 2, 3); it will be pkt2
            testutils.verify_each_packet_on_multiple_port_lists(
                self, pkts=[pkt1, pkt1], ports=[[0, 2, 3], [0, 1]], device_number=1
            )
