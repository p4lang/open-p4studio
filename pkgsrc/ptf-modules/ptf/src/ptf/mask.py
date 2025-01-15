import warnings

from io import StringIO
import sys
from . import packet


class MaskException(Exception):
    """Generic Mask Exception"""

    pass


class Mask:
    def __init__(self, exp_pkt, ignore_extra_bytes=False, dont_care_all=False):
        self.exp_pkt = exp_pkt
        self.size = len(exp_pkt)
        self.valid = True
        self.mask = [0] * self.size if dont_care_all else [0xFF] * self.size
        self.ignore_extra_bytes = ignore_extra_bytes

    def set_care(self, offset, bitwidth):
        for idx in range(offset, offset + bitwidth):
            offsetB = idx // 8
            offsetb = idx % 8
            self.mask[offsetB] = self.mask[offsetB] | (1 << (7 - offsetb))

    def set_care_all(self):
        self.mask = [0xFF] * self.size

    def set_care_packet(self, hdr_type, field_name):
        offset, bitwidth = self._calculate_fields_offset_and_bitwidth(
            hdr_type, field_name
        )
        self.set_care(offset, bitwidth)

    def set_do_not_care(self, offset, bitwidth):
        # a very naive but simple method
        # we do it bit by bit :)
        for idx in range(offset, offset + bitwidth):
            offsetB = idx // 8
            offsetb = idx % 8
            self.mask[offsetB] = self.mask[offsetB] & (~(1 << (7 - offsetb)))

    def set_do_not_care_all(self):
        self.mask = [0] * self.size

    def set_do_not_care_packet(self, hdr_type, field_name):
        offset, bitwidth = self._calculate_fields_offset_and_bitwidth(
            hdr_type, field_name
        )
        self.set_do_not_care(offset, bitwidth)

    def set_do_not_care_scapy(self, hdr_type, field_name):
        warnings.warn(
            '"set_do_not_care_scapy" is going to be deprecated, please '
            'switch to the new one: "set_do_not_care_packet"',
            DeprecationWarning,
        )
        self.set_do_not_care_packet(hdr_type, field_name)

    def set_ignore_extra_bytes(self):
        self.ignore_extra_bytes = True

    def is_valid(self):
        return self.valid

    def pkt_match(self, pkt):
        # just to be on the safe side
        pkt = bytearray(bytes(pkt))
        # we fail if we don't match on sizes, or if ignore_extra_bytes is set,
        # fail if we have not received at least size bytes
        if (not self.ignore_extra_bytes and len(pkt) != self.size) or len(
            pkt
        ) < self.size:
            return False
        exp_pkt = bytearray(bytes(self.exp_pkt))
        for i in range(self.size):
            if (exp_pkt[i] & self.mask[i]) != (pkt[i] & self.mask[i]):
                return False
        return True

    def _calculate_fields_offset_and_bitwidth(self, hdr_type, field_name):
        if hdr_type not in self.exp_pkt:
            self.valid = False
            raise MaskException("Unknown header type")

        try:
            fields_desc = [
                field
                for field in hdr_type.fields_desc
                if field.name
                in list(
                    self.exp_pkt[hdr_type]
                    .__class__(bytes(self.exp_pkt[hdr_type]))
                    .fields.keys()
                )
            ]  # build & parse packet to be sure all fields are correctly filled
        except Exception:  # noqa
            self.valid = False
            raise MaskException("Can not build or decode Packet")

        if field_name not in [x.name for x in fields_desc]:
            self.valid = False
            raise MaskException("Field %s does not exist in frame" % field_name)

        hdr_offset = self.size - len(self.exp_pkt[hdr_type])
        offset = 0
        bitwidth = 0
        for f in fields_desc:
            try:
                bits = f.size
            except Exception:  # noqa
                bits = 8 * f.sz
            if f.name == field_name:
                bitwidth = bits
                break
            else:
                offset += bits
        return hdr_offset * 8 + offset, bitwidth

    def __str__(self):
        old_stdout = sys.stdout
        sys.stdout = buffer = StringIO()
        print("\npacket status: %s" % "OK" if self.valid else "INVALID")
        print("packet:")
        packet.hexdump(self.exp_pkt)  # noqa
        print("\npacket's mask:")
        packet.hexdump(self.mask)  # noqa

        sys.stdout = old_stdout
        return buffer.getvalue()
