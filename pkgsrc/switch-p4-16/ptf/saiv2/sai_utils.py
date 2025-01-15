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
Thrift SAI interface basic utils.
"""

import time
import socket
import struct

from functools import wraps

from ptf.packet import *
from ptf.testutils import *
from ptf.mask import Mask

from sai_adapter import *

# pylint: disable=too-many-arguments,too-many-branches,line-too-long
# pylint: disable=invalid-name


def sai_thrift_query_attribute_enum_values_capability(
        client, obj_type, attr_id=None):
    """
    Call sai_thrift_query_attribute_enum_values_capability() function
    and return the list of supported aattr_is enum capabilities

    Args:
        client (Client): SAI RPC client
        obj_type (enum): SAI object type
        attr_id (attr): SAI attribute name

    Returns:
        list: list of switch object type enum capabilities
    """
    max_cap_no = 256

    enum_cap_list = client.sai_thrift_query_attribute_enum_values_capability(
        obj_type, attr_id, max_cap_no)

    return enum_cap_list


def sai_thrift_object_type_get_availability(
        client, obj_type, attr_id=None, attr_type=None):
    """
    sai_thrift_object_type_get_availability() RPC client function
    implementation

    Args:
        client (Client): SAI RPC client
        obj_type (enum): SAI object type
        attr_id (attr): SAI attribute name
        attr_type (type): SAI attribute type

    Returns:
        uint: number of available resources with given parameters
    """
    availability_cnt = client.sai_thrift_object_type_get_availability(
        obj_type, attr_id, attr_type)

    return availability_cnt


def sai_thrift_get_debug_counter_port_stats(client, port_oid, counter_ids):
    """
    Get port statistics for given debug counters

    Args:
        client (Client): SAI RPC client
        port_oid (sai_thrift_object_id_t): object_id IN argument
        counter_ids (sai_stat_id_t): list of requested counters

    Returns:
        Dict[str, sai_thrift_uint64_t]: stats
    """
    defaulf_mode = SAI_STATS_MODE_READ

    stats = {}
    counters = client.sai_thrift_get_port_stats_ext(
        port_oid, counter_ids, mode=defaulf_mode)

    for i, counter_id in enumerate(counter_ids):
        stats[counter_id] = counters[i]

    return stats


def sai_thrift_get_debug_counter_switch_stats(client, counter_ids):
    """
    Get switch statistics for given debug counters

    Args:
        client (Client): SAI RPC client
        counter_ids (sai_stat_id_t): list of requested counters

    Returns:
        Dict[str, sai_thrift_uint64_t]: stats
    """
    default_mode = SAI_STATS_MODE_READ

    stats = dict()
    counters = client.sai_thrift_get_switch_stats_ext(
        counter_ids, default_mode)

    for i, counter_id in enumerate(counter_ids):
        stats[counter_id] = counters[i]

    return stats


def sai_ipaddress(addr_str):
    """
    Sets SAI ip address parameters, assigns the appropriate type
    to these parameters and returns a sai_thrift_ip_address_t struct
    containing them

    Args:
        addr_str (str): SAI IP address

    Returns:
        sai_thrift_ip_address_t: object containing
        family and addr parameters
    """

    if '.' in addr_str:
        family = SAI_IP_ADDR_FAMILY_IPV4
        addr = sai_thrift_ip_addr_t(ip4=addr_str)
    if ':' in addr_str:
        family = SAI_IP_ADDR_FAMILY_IPV6
        addr = sai_thrift_ip_addr_t(ip6=addr_str)
    ip_addr = sai_thrift_ip_address_t(addr_family=family, addr=addr)
    return ip_addr


def sai_ipprefix(prefix_str):
    """
    Sets IP address prefix and mask and returns ip_prefix object

    Args:
        prefix_str (str): contains an IP address with mask

    Return:
        sai_thrift_ip_prefix_t: IP prefix object
    """
    addr_mask = prefix_str.split('/')
    if len(addr_mask) != 2:
        print('Invalid IP prefix format')
        return None

    if '.' in prefix_str:
        family = SAI_IP_ADDR_FAMILY_IPV4
        addr = sai_thrift_ip_addr_t(ip4=addr_mask[0])
        mask = num_to_dotted_quad(addr_mask[1])
        mask = sai_thrift_ip_addr_t(ip4=mask)
    if ':' in prefix_str:
        family = SAI_IP_ADDR_FAMILY_IPV6
        addr = sai_thrift_ip_addr_t(ip6=addr_mask[0])
        mask = num_to_dotted_quad(int(addr_mask[1]), ipv4=False)
        mask = sai_thrift_ip_addr_t(ip6=mask)

    ip_prefix = sai_thrift_ip_prefix_t(
        addr_family=family, addr=addr, mask=mask)
    return ip_prefix


def num_to_dotted_quad(number, ipv4=True):
    """
    Converts the ip address to the appropriate format

    Args:
        number (str): IP address in the form of a number
        ipv4 (boolean): determines whether IPv4 address standard is handled

    Returns:
        str: formatted IP address
    """
    if ipv4 is True:
        mask = (1 << 32) - (1 << 32 >> int(number))
        return socket.inet_ntop(socket.AF_INET, struct.pack('>L', mask))

    mask = (1 << 128) - (1 << 128 >> int(number))
    i = 0
    result = ''
    mask_str = '%032x' % mask
    for sign in mask_str:
        if (i + 1) % 4 == 0:
            result = result + sign + ':'
        else:
            result = result + sign
        i += 1
    return result[:-1]


def generate_mac_addresses(no_of_addr=1, base_mac="00:00:00:00:00:00"):
    '''
    Generate list of different mac addresses
    Args:
        no_of_addr (int): number of requested MAC addresses (max 256^4)
        base_mac (str): base MAC address of the generated range
    Return:
        list: mac_list with generated MAC addresses
    '''
    mac_list = []
    base_mac_list = base_mac.split(':')
    mac_dec = 0
    for i in range(6):
        mac_dec |= (int(base_mac_list[5 - i], 16) << (i * 8))

    for _ in range(no_of_addr):
        mac_dec = mac_dec + 1
        mac = ""
        for i in range(6):
            if mac:
                mac = ':' + mac
            mac = ('%02x' % ((mac_dec >> (i * 8)) & 0xFF)) + mac
        mac_list.append(mac)

    return mac_list


def open_packet_socket(hostif_name):
    """
    Open a linux socket

    Args:
        hostif_name (str): socket interface name

    Return:
        sock: socket ID
    """
    eth_p_all = 3
    sock = socket.socket(socket.AF_PACKET, socket.SOCK_RAW,
                         socket.htons(eth_p_all))
    sock.bind((hostif_name, eth_p_all))
    sock.setblocking(0)
    return sock


def socket_verify_packet(pkt, sock, timeout=2):
    """
    Verify packet on a socket

    Args:
        pkt (packet): packet to match with
        sock (int): socket ID
        timeout (int): timeout

    Return:
        match: match or no
    """
    max_pkt_size = 9100
    timeout = time.time() + timeout
    match = False

    if isinstance(pkt, Mask):
        if not pkt.is_valid():
            return False

    while time.time() < timeout:
        try:
            packet_from_tap_device = Ether(sock.recv(max_pkt_size))

            if isinstance(pkt, Mask):
                match = pkt.pkt_match(packet_from_tap_device)
            else:
                match = (str(packet_from_tap_device) == str(pkt))

            if match:
                break

        except BaseException:
            pass

    return match


def delay_wrapper(func, delay=2):
    """
    A wrapper extending given function by a delay.

    Args:
        func (function): function to be wrapped
        delay (int): delay period

    Return:
        wrapped_function: wrapped function
    """
    @wraps(func)
    def wrapped_function(*args, **kwargs):
        """
        A wrapper function adding a delay.

        Args:
            args (tuple): function arguments
            kwargs (dict): keyword function arguments

        Return:
            status: original function return value
        """
        test_params = test_params_get()
        if test_params['target'] != "hw":
            time.sleep(delay)

        status = func(*args, **kwargs)
        return status

    return wrapped_function


sai_thrift_flush_fdb_entries = delay_wrapper(sai_thrift_flush_fdb_entries)  # noqa pylint: disable=invalid-name
