import pytest
from scapy.layers.inet import IP, TCP, UDP
from scapy.layers.vxlan import VXLAN
from scapy.packet import Packet

from ptf.mask import Mask, MaskException


class TestMask:
    def test_mask__mask_simple_packet(self, scapy_simple_tcp_packet):
        packet = scapy_simple_tcp_packet
        mask_packet = Mask(packet)
        assert mask_packet.pkt_match(packet)

        modified_packet = packet.copy()
        modified_packet[TCP].sport = 97
        assert not mask_packet.pkt_match(modified_packet)

        # try to mask only sport (still packet will be different on chksum!)
        mask_packet.set_do_not_care_packet(TCP, "sport")
        assert not mask_packet.pkt_match(modified_packet)

        mask_packet.set_do_not_care_packet(TCP, "chksum")
        assert mask_packet.pkt_match(modified_packet)

    def test_mask__set_do_not_care_all(self):
        expected_packet = "\x01\x02\x03\x04\x05\x06"
        packet = "\x08\x07\x06\x05\x04\x03\x02\x01"
        mask = Mask(expected_packet.encode(), ignore_extra_bytes=True)
        mask.set_do_not_care_all()
        assert mask.pkt_match(packet.encode())

    def test_mask__set_do_not_care(self):
        expected_packet = "\x01\x02\x03\x04\x05\x06"
        packet = "\x01\x00\x00\x04\x05\x06\x07\x08"
        mask = Mask(expected_packet.encode(), ignore_extra_bytes=True)
        mask.set_do_not_care(8, 16)
        assert mask.pkt_match(packet.encode())

    def test_mask__set_care_all(self):
        expected_packet = "\x01\x02\x03\x04\x05\x06"
        packet = "\x00\x02\x03\x04\x05\x06"
        mask = Mask(expected_packet.encode(), ignore_extra_bytes=True)
        mask.set_care_all()
        assert not mask.pkt_match(packet.encode())

    def test_mask__set_care(self):
        expected_packet = "\x01\x02\x03\x04\x05\x06"
        packet = "\x01\x02\x00\x04\x05\x06\x07\x08"
        mask = Mask(
            expected_packet.encode(), ignore_extra_bytes=True, dont_care_all=True
        )
        mask.set_care(0, 16)
        assert mask.pkt_match(packet.encode())
        mask.set_care(16, 8)
        assert not mask.pkt_match(packet.encode())

    def test_mask__set_care_packet(self):
        packet = IP(src="1.1.1.1")
        mask = Mask(packet.copy(), ignore_extra_bytes=True, dont_care_all=True)
        packet[IP].src = "2.2.2.2"
        assert mask.pkt_match(packet)
        mask.set_care_packet(IP, "src")
        assert not mask.pkt_match(packet)

    def test_mask__check_masking_conditional_field(self, scapy_simple_vxlan_packet):
        simple_vxlan = scapy_simple_vxlan_packet
        simple_vxlan[VXLAN].flags = "G"

        masked_simple_vxlan = Mask(simple_vxlan)
        masked_simple_vxlan.set_do_not_care_packet(VXLAN, "gpid")  # gpflags, gpid

        second_masked_packet = Mask(simple_vxlan)
        second_masked_packet.set_do_not_care_packet(VXLAN, "gpflags")

        assert (
            masked_simple_vxlan.mask != second_masked_packet.mask
        ), "Masks should not be equal"

    def test_mask__mask_has_problem_with_conditional_fields(self):
        pkt = VXLAN(flags=0x80, gpflags=0x23, gpid=0x1234, vni=0x1337)
        mask_pkt = Mask(pkt)
        mask_pkt.set_do_not_care_packet(VXLAN, "gpid")

        assert mask_pkt.mask == [
            255,
            255,
            0,
            0,
            255,
            255,
            255,
            255,
        ], "Only gpid field should be masked"

    def test_mask__conditional_field__gpid_should_be_masked_correctly(
        self, scapy_simple_vxlan_packet
    ):
        simple_vxlan = scapy_simple_vxlan_packet  # type: Packet
        simple_vxlan[VXLAN].flags = "G"
        masked_simple_vxlan = Mask(simple_vxlan)  # type: Mask
        masked_simple_vxlan.set_do_not_care_packet(VXLAN, "gpid")
        # UDP chksum will be different when we change VXLAN
        masked_simple_vxlan.set_do_not_care_packet(UDP, "chksum")

        packet_wth_custom_gpid = simple_vxlan.copy()  # type: Packet
        packet_wth_custom_gpid[VXLAN].gpid = 0x15  # 21

        assert masked_simple_vxlan.pkt_match(
            packet_wth_custom_gpid
        ), "Packets should match"

    @pytest.mark.parametrize(
        "elements_to_ignore", ((UDP, "sport"), (IP, "not_existing_field"))
    )
    def test_mask__negative__try_to_mask_not_existing_layer_or_field(
        self, elements_to_ignore, scapy_simple_tcp_packet
    ):
        masked_packet = Mask(scapy_simple_tcp_packet)  # type: Mask
        with pytest.raises(MaskException):
            masked_packet.set_do_not_care_packet(
                elements_to_ignore[0], elements_to_ignore[1]
            )
        assert not masked_packet.valid

    def test_mask__validate_str_conversion(self, scapy_simple_tcp_packet):
        masked_packet = Mask(scapy_simple_tcp_packet)  # type: Mask
        masked_packet.set_do_not_care_packet(IP, "chksum")
        assert str(masked_packet) == EXPECTED_MASKED_PACKET


EXPECTED_MASKED_PACKET = """
packet status: OK
packet:
0000  00 01 02 03 04 05 00 06 07 08 09 0A 08 00 45 00  ..............E.
0010  00 28 00 01 00 00 40 06 F9 7B C0 A8 00 01 C0 A8  .(....@..{......
0020  00 02 04 D2 00 50 00 00 00 00 00 00 00 00 50 02  .....P........P.
0030  20 00 09 6D 00 00                                 ..m..

packet's mask:
0000  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF  ................
0010  FF FF FF FF FF FF FF FF 00 00 FF FF FF FF FF FF  ................
0020  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF  ................
0030  FF FF FF FF FF FF                                ......
"""
