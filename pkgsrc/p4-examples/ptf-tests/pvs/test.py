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

import sys
import random
import pd_base_tests
from ptf import config
from ptf.packet import *
from ptf.testutils import *
from p4testutils.misc_utils import *
from ptf.thriftutils import *
from mirror_pd_rpc.ttypes import *
from pvs.p4_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *

g_num_pipes = int(test_param_get('num_pipes'))

PORT_MAX = 72

g_swports = get_sw_ports()

print()
"Using ports:", g_swports
sys.stdout.flush()

ether_types = [0x1111, 0x8100]
g_no_1q_port = -1

swports = []
ports_by_pipe = {}
prsrs_by_pipe = {}
for p in range(g_num_pipes):
    ports_by_pipe[p] = []
    prsrs_by_pipe[p] = {}


def get_parser_id_for_port(test, port):
    parser_id = test.devport_mgr.devport_mgr_get_parser_id(0, port)
    return parser_id

def categorize_ports_to_pipes(test):
    # Sort the ports into lists per pipe. Do not use ports that share the same
    # parser though.
    seen_parsers = {}
    for port in g_swports:
        pipe = port_to_pipe(port)
        prsr = get_parser_id_for_port(test, port)
        if (pipe,prsr) in seen_parsers:
            continue
        ports_by_pipe[pipe].append(port)
        prsrs_by_pipe[pipe][port] = prsr

def form_swports(test):
    global swports
    global g_no_1q_port
    categorize_ports_to_pipes(test)

    # Find the pipe with the most ports.  From that pipe pick one port to be
    # the "miss" port.
    max_len = -1
    lengths = [len(ports_by_pipe[p]) for p in range(g_num_pipes)]
    for which,val in enumerate(lengths):
        if val > max_len:
            max_len = val
            max_len_pipe = which
    g_no_1q_port = ports_by_pipe[max_len_pipe].pop()
    del prsrs_by_pipe[max_len_pipe][g_no_1q_port]

    # Limit each pipe to use two ports.  Try to find two ports in the pipe that
    # use different parsers.
    for p in range(g_num_pipes):
        if len(ports_by_pipe[p]) > 2:
            ports_to_keep = set()
            for i in range(len(ports_by_pipe[p])):
                prsr_i = get_parser_id_for_port(test, ports_by_pipe[p][i])
                for j in range(i+1, len(ports_by_pipe[p])):
                    prsr_j = get_parser_id_for_port(test, ports_by_pipe[p][j])
                    if prsr_i != prsr_j:
                        ports_to_keep.add(ports_by_pipe[p][i])
                        ports_to_keep.add(ports_by_pipe[p][j])
                        break
                if len(ports_to_keep): break

            # If we couldn't find two ports that have unique parsers, fall back
            # to using a pair that share a parser.
            if len(ports_to_keep) == 0:
                ports_to_keep.add(ports_by_pipe[p][0])
                ports_to_keep.add(ports_by_pipe[p][1])

            # Remove any extra ports from our structures.
            ports_to_rmv = set(ports_by_pipe[p]).difference(ports_to_keep)
            for port in ports_to_rmv:
                del prsrs_by_pipe[p][port]
                ports_by_pipe[p].remove(port)

        swports = swports + ports_by_pipe[p]

    print("Miss port:", g_no_1q_port)
    print("Hit ports:", swports)


def mirror_session(mir_type, mir_dir, sid, egr_port=0, egr_port_v=False,
                   egr_port_queue=0, packet_color=0, mcast_grp_a=0,
                   mcast_grp_a_v=False, mcast_grp_b=0, mcast_grp_b_v=False,
                   max_pkt_len=0, level1_mcast_hash=0, level2_mcast_hash=0,
                   mcast_l1_xid=0, mcast_l2_xid=0, mcast_rid=0, cos=0, c2c=0, extract_len=0, timeout=0,
                   int_hdr=[], hdr_len=0):
    return MirrorSessionInfo_t(mir_type,
                               mir_dir,
                               sid,
                               egr_port,
                               egr_port_v,
                               egr_port_queue,
                               packet_color,
                               mcast_grp_a,
                               mcast_grp_a_v,
                               mcast_grp_b,
                               mcast_grp_b_v,
                               max_pkt_len,
                               level1_mcast_hash,
                               level2_mcast_hash,
                               mcast_l1_xid,
                               mcast_l2_xid,
                               mcast_rid,
                               cos,
                               c2c,
                               extract_len,
                               timeout,
                               int_hdr,
                               hdr_len)

def get_new_vid(client, vid):
    new_vid = vid + 11
    return new_vid

def add_vlans(client, sess_hdl, vid, port):
    vid = get_new_vid(client, vid)
    dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))
    match_spec = pvs_vlan_match_spec_t(vid, 1)
    action_spec = pvs_vlan_hit_action_spec_t(
        action_egress_port=port)
    h = client.vlan_table_add_with_vlan_hit(sess_hdl, dev_tgt,
                                            match_spec, action_spec)
    return h

def add_vlans2(client, sess_hdl, vid, mod_vid):
    vid = get_new_vid(client, vid)
    mod_vid = get_new_vid(client, mod_vid)
    dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))
    match_spec = pvs_vlan2_match_spec_t(vid, 1)
    action_spec = pvs_mod_vid_action_spec_t(action_val=mod_vid)
    h = client.vlan2_table_add_with_mod_vid(sess_hdl, dev_tgt,
                                            match_spec, action_spec)
    return h

def add_ttl(client, sess_hdl, ttl, new_ttl):
    dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))
    match_spec = pvs_read_ttl_match_spec_t(ttl, 1)
    action_spec = pvs_mod_ttl_action_spec_t(action_val=new_ttl)
    h = client.read_ttl_table_add_with_mod_ttl(sess_hdl, dev_tgt, match_spec, action_spec)
    return h

def init_vlan_table(client, sess_hdl):
    dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))
    print("Set table vlan's default action to use port", g_no_1q_port)
    action_spec = pvs_vlan_miss_action_spec_t(
        action_egress_port=g_no_1q_port)
    client.vlan_set_default_action_vlan_miss(sess_hdl, dev_tgt,
                                             action_spec)

def init_vlan2_table(client, sess_hdl):
    dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))
    client.vlan2_set_default_action_noop(sess_hdl, dev_tgt)

def init_read_ttl_table(client, sess_hdl):
    dev_tgt = DevTarget_t(0, hex_to_i16(0xFFFF))
    action_spec = pvs_mod_ttl_action_spec_t(action_val = 80)
    client.read_ttl_set_default_action_mod_ttl(sess_hdl, dev_tgt, action_spec)

def set_pvs_gress_scope(client, sess_hdl, dev_id, gress_scope):
    return client.pvs_pvs2_set_property(sess_hdl, dev_id, pvs_property_t.PVS_GRESS_SCOPE, gress_scope, 0)

def set_pvs_pipe_scope(client, sess_hdl, dev_id, gress_id, pipe_scope):
    return client.pvs_pvs2_set_property(sess_hdl, dev_id, pvs_property_t.PVS_PIPE_SCOPE, pipe_scope, gress_id)

def set_pvs_parser_scope(client, sess_hdl, dev_id, gress_id, parser_scope):
    return client.pvs_pvs2_set_property(sess_hdl, dev_id, pvs_property_t.PVS_PARSER_SCOPE, parser_scope, gress_id)

def get_pvs_gress_scope(client, sess_hdl, dev_id):
    return client.pvs_pvs2_get_property(sess_hdl, dev_id, pvs_property_t.PVS_GRESS_SCOPE, 0)

def get_pvs_pipe_scope(client, sess_hdl, dev_id, gress_id):
    return client.pvs_pvs2_get_property(sess_hdl, dev_id, pvs_property_t.PVS_PIPE_SCOPE, gress_id)

def get_pvs_parser_scope(client, sess_hdl, dev_id, gress_id):
    return client.pvs_pvs2_get_property(sess_hdl, dev_id, pvs_property_t.PVS_PARSER_SCOPE, gress_id)

def add_pvs_entry(client, sess_hdl, dev_id, gress, pipe, parser, value, mask):
    prsr_tgt = DevParserTarget_t(0, gress, pipe, parser)
    return client.pvs_pvs2_entry_add(sess_hdl, prsr_tgt, value, mask)

def modify_pvs_entry(client, sess_hdl, dev_id, entry_hdl, value, mask):
    return client.pvs_pvs2_entry_modify(sess_hdl, dev_id, entry_hdl, value, mask)

def delete_pvs_entry(client, sess_hdl, dev_id, entry_hdl):
    return client.pvs_pvs2_entry_delete(sess_hdl, dev_id, entry_hdl)

def get_pvs_entry(client, sess_hdl, dev_id, entry_hdl):
    return client.pvs_pvs2_entry_get(sess_hdl, dev_id, entry_hdl)

def get_pvs_entry_handle(client, sess_hdl, dev_id, gress, pipe, parser, value, mask):
    prsr_tgt = DevParserTarget_t(0, gress, pipe, parser)
    return client.pvs_pvs2_entry_handle_get(sess_hdl, prsr_tgt, value, mask)

def InjectPktVerifyForPipesMirroring(self, pipes, inj_port, vlan, mirr_asic_port, exp_port, eth_value, mirr_port_vid, exp_port_vid):
    pipe = port_to_pipe(inj_port)
    if pipes != hex_to_i16(0xFFFF):
        if pipe != pipes:
            return

    vlan = get_new_vid(self, vlan)
    mirr_port_vid = get_new_vid(self, mirr_port_vid)
    exp_port_vid = get_new_vid(self, exp_port_vid)

    mac_addr = '00:00:00:00:' + hex(pipe)[2:].zfill(2)
    mac_addr = mac_addr + ':' + hex(inj_port % 256)[2:].zfill(2)

    pkt = simple_arp_packet(pktlen=100, eth_dst=mac_addr, eth_src='00:00:00:00:01:01', vlan_vid=vlan, vlan_pcp=7)
    mir_pkt = simple_arp_packet(pktlen=100, eth_dst=mac_addr, eth_src='00:00:00:00:01:01', vlan_vid=mirr_port_vid, vlan_pcp=7)
    exp_pkt = simple_arp_packet(pktlen=100, eth_dst=mac_addr, eth_src='00:00:00:00:01:01', vlan_vid=exp_port_vid, vlan_pcp=7)

    pkt.type = eth_value
    mir_pkt.type = eth_value
    exp_pkt.type = eth_value
    print("Sending on", inj_port)
    sys.stdout.flush()
    send_packet(self, inj_port, pkt)
    print("  Verifying packet is on ", mirr_asic_port)
    print("  Verifying packet is on ", exp_port)
    verify_packet(self, exp_pkt, exp_port)
    verify_packet(self, mir_pkt, mirr_asic_port)
    verify_no_other_packets(self, timeout=2)

def InjectPktVerifyForPipes(self, pipes, inj_port, vlan, exp_asic_port, not_exp_port, eth_value, mod_vid):
    pipe = port_to_pipe(inj_port)
    if pipes != hex_to_i16(0xFFFF):
        if pipe != pipes:
            return
    mac_addr = '00:00:00:00:' + hex(pipe)[2:].zfill(2)
    mac_addr = mac_addr + ':' + hex(inj_port % 256)[2:].zfill(2)

    vlan = get_new_vid(self, vlan)
    mod_vid = get_new_vid(self, mod_vid)

    pkt = simple_tcp_packet(eth_dst=mac_addr,
                            eth_src='00:00:00:00:01:01',
                            dl_vlan_enable=True,
                            vlan_vid=vlan,
                            vlan_pcp=7,
                            ip_dst='10.10.3.3',
                            ip_src='10.10.1.1',
                            ip_id=105,
                            ip_ttl=4)
    exp_pkt = simple_tcp_packet(eth_dst=mac_addr,
                                eth_src='00:00:00:00:01:01',
                                ip_dst='10.10.3.3',
                                ip_src='10.10.1.1',
                                ip_id=105,
                                dl_vlan_enable=True,
                                vlan_vid=mod_vid,
                                vlan_pcp=7,
                                ip_ttl=4)
    pkt.type = eth_value
    exp_pkt.type = eth_value
    print("Sent pkt on", inj_port, "expected rx on", exp_asic_port)
    send_packet(self, inj_port, pkt)
    try:
        verify_packet(self, exp_pkt, exp_asic_port, timeout=15)
    except AssertionError:
        sys.stdout.flush()
        raise

def InjectPktVerifyForPipesMultiplePVS(self, pipes, inj_port, vlan, exp_asic_port, not_exp_port, eth_value, mod_vid, mod_ttl):
    pipe = port_to_pipe(inj_port)
    if pipes != hex_to_i16(0xFFFF):
        if pipe != pipes:
            return

    vlan = get_new_vid(self, vlan)
    mod_vid = get_new_vid(self, mod_vid)

    mac_addr = '00:00:00:00:' + hex(pipe)[2:].zfill(2)
    mac_addr = mac_addr + ':' + hex(inj_port % 256)[2:].zfill(2)

    pkt = simple_tcp_packet(eth_dst=mac_addr,
                            eth_src='00:00:00:00:01:01',
                            dl_vlan_enable=True,
                            vlan_vid=vlan,
                            vlan_pcp=7,
                            ip_dst='10.10.3.3',
                            ip_src='10.10.1.1',
                            ip_id=105,
                            ip_ttl=70)
    exp_pkt = simple_tcp_packet(eth_dst=mac_addr,
                                eth_src='00:00:00:00:01:01',
                                ip_dst='10.10.3.3',
                                ip_src='10.10.1.1',
                                ip_id=105,
                                dl_vlan_enable=True,
                                vlan_vid=mod_vid,
                                vlan_pcp=7,
                                ip_ttl=mod_ttl)
    pkt.type = eth_value
    exp_pkt.type = eth_value
    send_packet(self, inj_port, pkt)
    try:
        verify_packet(self, exp_pkt, exp_asic_port)
    except AssertionError:
        print("Sent on", inj_port, "expected on", exp_asic_port)
        sys.stdout.flush()
        raise
    if exp_asic_port != not_exp_port:
        print("  Verifying no packet on", not_exp_port)
        sys.stdout.flush()
        try:
            verify_no_packet(self, exp_pkt, not_exp_port)
        except AssertionError:
            print("Sent on", inj_port, "expected no packet on ", exp_asic_port)
            raise

class TestPipelineTable(pd_base_tests.EmptyTableTest):
    program = "pvs"
    table = "p0"


@group("thorough")
class Config(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["pvs"])

    def setUp(self):
        pd_base_tests.ThriftInterfaceDataPlane.setUp(self)
        print()
        # Set the seed for pseudo random number generator
        seed = random.randint(0, 10000000)
        print("Seed used for PVS Test is ", seed)
        random.seed(seed)

        form_swports(self)
        print("Ports by Pipe\n")
        for pipe in range(g_num_pipes):
            print(" Pipe ", pipe, ": ", ports_by_pipe[pipe])
        print("Parsers by Pipe\n")
        for pipe in range(g_num_pipes):
            print(" Pipe ", pipe, ": ", ports_by_pipe[pipe])

        print('Configuring the devices')
        self.vlan_entry_hdls = []
        self.vlan_entry_hdls2 = []
        self.ttl_entry_hdls = []
        self.sess_hdl = None
        self.sess_hdl = self.conn_mgr.client_init()
        init_vlan_table(self.client, self.sess_hdl)
        init_vlan2_table(self.client, self.sess_hdl)
        init_read_ttl_table(self.client, self.sess_hdl)
        for pipe in range(0,g_num_pipes):
            for port in range(0,PORT_MAX):
                asic_port = make_port(pipe, port)
                h = add_vlans(self.client, self.sess_hdl, vid=0x100+asic_port,
                              port=asic_port)
                self.vlan_entry_hdls.append(h)
                h = add_vlans2(self.client, self.sess_hdl, vid=0x100+asic_port, mod_vid=asic_port)
                self.vlan_entry_hdls2.append(h)
        h = add_ttl(self.client, self.sess_hdl, ttl=70, new_ttl=90)
        self.ttl_entry_hdls.append(h)
        self.conn_mgr.complete_operations(self.sess_hdl)

    def tearDown(self):
        print('Cleaning up')
        sys.stdout.flush()
        if self.sess_hdl:
            for h in self.vlan_entry_hdls:
                self.client.vlan_table_delete(self.sess_hdl, 0, h)
            for h in self.vlan_entry_hdls2:
                self.client.vlan2_table_delete(self.sess_hdl, 0, h)
            for h in self.ttl_entry_hdls:
                self.client.read_ttl_table_delete(self.sess_hdl, 0, h)
            self.conn_mgr.client_cleanup(self.sess_hdl)

    def runTest(self):
        def runTestAllGressAllPipesAllParsers(self):
            print()
            print('Running runTestAllGressAllPipesAllParsers...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Use each pvs set element (its value represents ether-type)
            # pvs2 (check pvs.p4) set size is 7
            # Program Parser Value set element with ether-type = .1q
            # .1Q packet will be switched using vlan-id as match key.

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Since pvs scope is all pipe, all parsers, use same setting to add pvs element
            print('Case 1')
            sys.stdout.flush()
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("PVS ent handle is %x"%(pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestAllGressAllPipesSingleParser(self):
            print()
            print('Running runTestAllGressAllPipesSingleParser...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            programmed = set()
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Since pvs scope is all pipe, all parsers, use same setting to add pvs element
            print('Case 1')
            sys.stdout.flush()
            # Program the values for the first port in each pipe symmetrically across all pipes.
            prsrs_to_prog = set()
            for p in range(g_num_pipes):
                if len(ports_by_pipe[p]):
                    prsrs_to_prog.add( prsrs_by_pipe[p][ ports_by_pipe[p][0] ] )
            for p in swports:
                for prsr in prsrs_to_prog:
                    if get_parser_id_for_port(self, p) == prsr:
                        programmed.add(p)
            for value in ether_types:
                for prsr in prsrs_to_prog:
                    pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), prsr, value, 0xffff)
                    print("PVS ent handle is %x"%(pvs_handle))
                    pvs_handle_arr.append(pvs_handle)
                    pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in programmed: # These ports should hit in pvs
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                    elif get_parser_id_for_port(self, g_no_1q_port) in prsrs_to_prog:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Now program the other parser in each of the pipes
            prsrs_to_prog2 = set()
            for p in range(g_num_pipes):
                if len(ports_by_pipe[p]) > 1:
                    if prsrs_by_pipe[p][ ports_by_pipe[p][1] ] not in prsrs_to_prog:
                        prsrs_to_prog2.add( prsrs_by_pipe[p][ ports_by_pipe[p][1] ] )
            for p in swports:
                for prsr in prsrs_to_prog2:
                    if get_parser_id_for_port(self, p) == prsr:
                        programmed.add(p)
            for value in ether_types:
                for prsr in prsrs_to_prog2:
                    pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), prsr, value, 0xffff)
                    self.conn_mgr.complete_operations(self.sess_hdl)
                    print("PVS ent handle is %x"%(pvs_handle))
                    pvs_handle_arr.append(pvs_handle)
                    pvs_handle_map[pvs_handle] = value
            for eth_value in ether_types:
                for in_port in swports: #All ports should hit
                    if in_port in programmed: # These ports should hit in pvs
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestAllGressSinglePipeAllParsers(self):
            print()
            print('Running runTestAllGressSinglePipeAllParsers...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            parser_value_prog_arr = []
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Since pvs scope is all pipe, all parsers, use same setting to add pvs element
            print('Case 1')
            sys.stdout.flush()
            programmed = set()
            for p in range(g_num_pipes):
                # program all parsers in each pipe one by one.
                for value in ether_types:
                    pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), p, hex_to_byte(0xFF), value, 0xffff)
                    print("Pipe %d PVS ent handle is 0x%x" % (p, pvs_handle))
                    pvs_handle_arr.append(pvs_handle)
                    pvs_handle_map[pvs_handle] = value
                self.conn_mgr.complete_operations(self.sess_hdl)
                programmed.add(p)
                for eth_value in ether_types:
                    for in_port in swports:
                        if port_to_pipe(in_port) in programmed: # All ports in the programmed pipes should hit
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                        else:
                            if port_to_pipe(g_no_1q_port) in programmed:
                                InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                            else:
                                InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestAllGressSinglePipeSingleParser(self):
            print()
            print('Running runTestAllGressSinglePipeSingleParser...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            ports_programmed = set()
            prsrs_programmed = set()
            to_skip = random.sample(swports, 2)
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            print('Case 1')
            sys.stdout.flush()
            for port in swports:
                if port not in to_skip:
                    pipe = port_to_pipe(port)
                    prsr = get_parser_id_for_port(self, port)
                    if (pipe,prsr) not in prsrs_programmed:
                        prsrs_programmed.add( (pipe,prsr) )
                        for value in ether_types:
                            pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), pipe, prsr, value, 0xffff)
                            print("Port %d Pipe %d Prsr %d Val 0x%x PVS ent handle is 0x%x" % (port, pipe, prsr, value, pvs_handle))
                            pvs_handle_arr.append(pvs_handle)
                            pvs_handle_map[pvs_handle] = value
                        self.conn_mgr.complete_operations(self.sess_hdl)
                    # Now loop over all ports and check which ports use the parsers
                    # programmed.  This covers the case of one of the ports sharing a
                    # parser with a port that was just programmed.
                    for p in swports + [g_no_1q_port]:
                        if pipe == port_to_pipe(p) and prsr == get_parser_id_for_port(self, p):
                            ports_programmed.add(p)
                for eth_value in ether_types:
                    for in_port in swports:
                        if in_port in ports_programmed:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                        else:
                            if g_no_1q_port in ports_programmed:
                                InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                            else:
                                InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in ports_programmed:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                    else:
                        if g_no_1q_port in ports_programmed:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestSingleGressAllPipesAllParsers(self):
            print()
            print('Running runTestSingleGressAllPipesAllParsers...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Use each pvs set element (its value represents ether-type)
            # pvs2 (check pvs.p4) set size is 7
            # Program Parser Value set element with ether-type = .1q
            # .1Q packet will be switched using vlan-id as match key.

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Since pvs scope is all pipe, all parsers, use same setting to add pvs element
            print('Case 1')
            sys.stdout.flush()
            # Program the pvs value only in the ingress. Thus the packet would be switched properly switched in TM
            # but it will miss the egress table and thus the vlan id should be unchanged
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("PVS ent handle is %x"%(pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)

            # Now program the value in the egress as well. Now the vlan id would change as the table in egress would hit
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("PVS ent handle is %x"%(pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestSingleGressAllPipesSingleParserSW(self):
            print()
            print('Running runTestSingleGressAllPipesSingleParserSW...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            parser_value_prog_arr = []
            parser_value_prog_egress_arr = []
            pipe0_port_0 = ports_by_pipe[0][0]
            # Without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Since pvs scope is all pipe, all parsers, use same setting to add pvs element
            print('Case 1')
            sys.stdout.flush()
            # Ingress Programming
            # program the value with first port on pipe 0.
            prsr_list = set()
            prsr_ing_id = prsrs_by_pipe[0][pipe0_port_0]
            prsr_list.add(prsr_ing_id)
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, hex_to_i16(0xFFFF), prsr_ing_id, value, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Ingress All Pipes, prsr %d PVS ent handle is 0x%x" % (prsr_ing_id, pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            # Add only ports with programmed prsr id
            for pipe in range(g_num_pipes):
                for port, prsr in prsrs_by_pipe[pipe].items():
                    if prsr == prsr_ing_id and port not in parser_value_prog_arr:
                        parser_value_prog_arr.append(port)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in parser_value_prog_arr: # These ports should hit in pvs
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Now program the other parsers in each of the pipes
            for pipe in range(g_num_pipes):
                for port, prsr_id in prsrs_by_pipe[pipe].items():
                    if prsr_id in prsr_list:  # Skip already added parser id
                        continue
                    prsr_list.add(prsr_id)
                    for value in ether_types:
                        pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, hex_to_i16(0xFFFF), prsr_id, value, 0xffff)
                        self.conn_mgr.complete_operations(self.sess_hdl)
                        print("Ingress All Pipes, prsr %d PVS ent handle is 0x%x" % (prsr_id, pvs_handle))
                        pvs_handle_arr.append(pvs_handle)
                        pvs_handle_map[pvs_handle] = value
            for eth_value in ether_types:
                for in_port in swports: #All ports should hit
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)

            # Egress Programming
            prsr_list = set()
            prsr_egr_id = prsrs_by_pipe[0][pipe0_port_0]
            prsr_list.add(prsr_egr_id)
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, hex_to_i16(0xFFFF), prsr_egr_id, value, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Egress All Pipes, prsr %d PVS ent handle is 0x%x" % (prsr_egr_id, pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            # Add only ports with programmed prsr id
            for pipe in range(g_num_pipes):
                for port, prsr in prsrs_by_pipe[pipe].items():
                    if prsr == prsr_egr_id and port not in parser_value_prog_egress_arr:
                        parser_value_prog_egress_arr.append(port)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in parser_value_prog_egress_arr: # These ports should hit in egress table and have the mod vlan id
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)

            # Now program the other parsers in each of the pipes
            for pipe in range(g_num_pipes):
                for port, prsr_id in prsrs_by_pipe[pipe].items():
                    if prsr_id in prsr_list:  # Skip already added parser id
                        continue
                    prsr_list.add(prsr_id)
                    for value in ether_types:
                        pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, hex_to_i16(0xFFFF), prsr_id, value, 0xffff)
                        self.conn_mgr.complete_operations(self.sess_hdl)
                        print("Egress All Pipes, prsr %d PVS ent handle is 0x%x" % (prsr_id, pvs_handle))
                        pvs_handle_arr.append(pvs_handle)
                        pvs_handle_map[pvs_handle] = value
            for eth_value in ether_types:
                for in_port in swports: #All ports should hit in egress table and have the mod vlan id
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestSingleGressAllPipesSingleParser(self):
            print()
            print('Running runTestSingleGressAllPipesSingleParser...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            programmed_ing = set()
            programmed_egr = set()
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Look through the available ports and see if there is a set of ports
            # across multiple pipes which share the same parser id.
            shared_parser_id = None
            for i in range(g_num_pipes):
                pipe_i_prsrs = set([get_parser_id_for_port(self, p) for p in ports_by_pipe[i]])
                for j in range(i+1, g_num_pipes):
                    pipe_j_prsrs = set([get_parser_id_for_port(self, p) for p in ports_by_pipe[j]])
                    common_prsrs = pipe_i_prsrs.intersection( pipe_j_prsrs )
                    if len(common_prsrs):
                        shared_parser_id = common_prsrs.pop()
                        break
                if shared_parser_id: break
            if not shared_parser_id:
                print("runTestSingleGressAllPipesSingleParser cannot find a parser id shared across pipes, skipping test case")
                return

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Since pvs scope is all pipe, all parsers, use same setting to add pvs element
            print('Case 1')
            sys.stdout.flush()
            # Ingress Programming
            prsrs_to_prog = set([shared_parser_id])
            for value in ether_types:
                for prsr in prsrs_to_prog:
                    pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, hex_to_i16(0xFFFF), prsr, value, 0xffff)
                    print("PVS ent handle is %x"%(pvs_handle))
                    pvs_handle_arr.append(pvs_handle)
                    pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for p in swports:
                if get_parser_id_for_port(self, p) in prsrs_to_prog:
                    programmed_ing.add(p)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in programmed_ing:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Egress Programming
            for value in ether_types:
                for prsr in prsrs_to_prog:
                    pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, hex_to_i16(0xFFFF), prsr, value, 0xffff)
                    print("PVS ent handle is %x"%(pvs_handle))
                    pvs_handle_arr.append(pvs_handle)
                    pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for p in swports + [g_no_1q_port]:
                if get_parser_id_for_port(self, p) in prsrs_to_prog:
                    programmed_egr.add(p)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in programmed_ing:
                        if in_port in programmed_egr:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        if g_no_1q_port in programmed_egr:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in programmed_ing:
                        if in_port in programmed_egr:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        if g_no_1q_port in programmed_egr:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestSingleGressSinglePipeAllParsers(self):
            print()
            print('Running runTestSingleGressSinglePipeAllParsers...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}

            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Program PVS entries per pipe, one at a time, sending traffic to
            # ensure that only ports on the pipes programmed match PVS entries.
            programmed_ports = []
            for p in range(g_num_pipes):
                programmed_ports = programmed_ports + ports_by_pipe[p]

                for value in ether_types:
                    pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, p, hex_to_byte(0xFF), value, 0xffff)
                    print("Pipe %d val 0x%x ingress PVS ent handle is 0x%x" % (p, value, pvs_handle))
                    pvs_handle_arr.append(pvs_handle)
                    pvs_handle_map[pvs_handle] = value
                self.conn_mgr.complete_operations(self.sess_hdl)
                for eth_value in ether_types:
                    for in_port in swports:
                        if in_port in programmed_ports:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)


            # Egress Programming
            programmed_ports = []
            for p in range(g_num_pipes):
                programmed_ports = programmed_ports + ports_by_pipe[p]
                for value in ether_types:
                    pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, p, hex_to_byte(0xFF), value, 0xffff)
                    print("Pipe %d val 0x%x egress PVS ent handle is 0x%x" % (p, value, pvs_handle))
                    pvs_handle_arr.append(pvs_handle)
                    pvs_handle_map[pvs_handle] = value
                self.conn_mgr.complete_operations(self.sess_hdl)
                for eth_value in ether_types:
                    for in_port in swports:
                        if in_port in programmed_ports:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)


            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestSingleGressSinglePipeSingleParser(self):
            print()
            print('Running runTestSingleGressSinglePipeSingleParser...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}

            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Pick two ingress ports and program their PVS
            ing_ports = random.sample(swports, 2)
            ing_prsrs_programmed = set()
            for port in ing_ports:
                pipe = port_to_pipe(port)
                prsr = get_parser_id_for_port(self, port)
                if (pipe,prsr) not in ing_prsrs_programmed:
                    ing_prsrs_programmed.add( (pipe,prsr) )
                    for value in ether_types:
                        pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, pipe, prsr, value, 0xffff)
                        print("Port %d pipe %d prsr %d value 0x%x ingress PVS ent handle is 0x%x" % (port, pipe, prsr, value, pvs_handle))
                        pvs_handle_arr.append(pvs_handle)
                        pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            ing_ports_programmed = set()
            # Loop over all ports and check which now have PVS entries.  Looping
            # here covers the case where a parser from ing_ports is shared with a
            # port not in ing_ports.
            for port in swports:
                for programmed_port in ing_ports:
                    if port_to_pipe(port) == port_to_pipe(programmed_port):
                        if get_parser_id_for_port(self, port) == get_parser_id_for_port(self, programmed_port):
                            ing_ports_programmed.add(port)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in ing_ports_programmed:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)


            # Egress Programming
            # Pick a few egress ports and program their PVS
            egr_ports = random.sample(swports, len(swports)//2)
            egr_prsrs_programmed = set()
            egr_ports_programmed = set()
            for port in egr_ports:
                pipe = port_to_pipe(port)
                prsr = get_parser_id_for_port(self, port)
                if (pipe,prsr) not in egr_prsrs_programmed:
                    egr_prsrs_programmed.add( (pipe,prsr) )
                    for value in ether_types:
                        pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, pipe, prsr, value, 0xffff)
                        print("Port %d pipe %d prsr %d value 0x%x egress PVS ent handle is 0x%x" % (port, pipe, prsr, value, pvs_handle))
                        pvs_handle_arr.append(pvs_handle)
                        pvs_handle_map[pvs_handle] = value
                    # Now loop over all ports and check which ports use the parsers
                    # programmed.  This covers the case of one of the egr_ports sharing a
                    # parser with a port not in egr_ports.
                    for p in swports + [g_no_1q_port]:
                        if (port_to_pipe(p), get_parser_id_for_port(self, p)) in egr_prsrs_programmed:
                            egr_ports_programmed.add(p)

                self.conn_mgr.complete_operations(self.sess_hdl)
                for eth_value in ether_types:
                    for in_port in swports:
                        if in_port in ing_ports_programmed:
                            if in_port in egr_ports_programmed:
                                InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                            else:
                                InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                        else:
                            if g_no_1q_port in egr_ports_programmed:
                                InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                            else:
                                InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in ing_ports_programmed:
                        if in_port in egr_ports_programmed:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        if g_no_1q_port in egr_ports_programmed:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestMiscellaneousScope1(self):
            print()
            print('Running runTestMiscellaneousScope1...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Ingress scope is per-pipe per-parser, egress scope is all-pipes all-parsers
            # Pick a couple of ports to program on the ingress
            ing_ports = random.sample(swports, 2)
            ing_ports_programmed = set()
            ing_prsrs_programmed = set()
            print('Case 1')
            # Ingress Programming
            sys.stdout.flush()
            for port in ing_ports:
                pipe = port_to_pipe(port)
                prsr = get_parser_id_for_port(self, port)
                if (pipe,prsr) not in ing_prsrs_programmed:
                    ing_prsrs_programmed.add( (pipe,prsr) )
                    for value in ether_types:
                        pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, pipe, prsr, value, 0xffff)
                        print("PVS ent handle is %x"%(pvs_handle))
                        pvs_handle_arr.append(pvs_handle)
                        pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            # Now loop over all ports and check which ports use the parsers
            # programmed.  This covers the case of one of the ing_ports sharing a
            # parser with a port not in ing_ports.
            for port in swports:
                if (port_to_pipe(port), get_parser_id_for_port(self, port)) in ing_prsrs_programmed:
                    ing_ports_programmed.add(port)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in ing_ports_programmed:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Egress Programming
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("PVS ent handle is %x"%(pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in ing_ports_programmed:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in ing_ports_programmed:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runTestMiscellaneousScope2(self):
            print()
            print('Running runTestMiscellaneousScope2...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Scope is ingress per-pipe all-parser, egress all-pipe, per-parser
            print('Case 1')
            sys.stdout.flush()
            # Ingress Programming
            # Pick an ingress pipe to program.
            pipes = set()
            for p in swports:
                pipes.add(port_to_pipe(p))
            ing_pipe = pipes.pop()
            programmed_ing = set()
            for p in swports:
                if ing_pipe == port_to_pipe(p):
                    programmed_ing.add(p)
            # program the value in one of the pipes (pipe 1) in all the parsers
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, ing_pipe, hex_to_byte(0xFF), value, 0xffff)
                print("PVS ent handle is %x"%(pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in programmed_ing:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Egress Programming
            ################################################
            no_1q_port_parser = get_parser_id_for_port(self, g_no_1q_port)
            programmed_egr = set([g_no_1q_port])
            for p in swports:
                if no_1q_port_parser == get_parser_id_for_port(self, p):
                    programmed_egr.add(p)
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, hex_to_i16(0xFFFF), no_1q_port_parser, value, 0xffff)
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            ################################################

            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in programmed_ing:
                        if in_port in programmed_egr:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        if g_no_1q_port in programmed_egr:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                        else:
                            assert(0)

            # Case 2: Inject packet with std eth-type but parser is programmed to match on different value.
            # Since pvs scope is all pipe, all parsers, use same setting to modify pvs element
            print('Case 2')
            sys.stdout.flush()
            # Modify the pipe 0 port 0 entry
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, 0x9900, 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)

            # After modify PVS to value different from 0x8100,
            # all packets should be sent to drop port
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Case 3: modify PVS, back to value to 0x8100.
            #         all packets should be switched correctly.
            print('Case 3')
            sys.stdout.flush()
            # Modify the pipe 0 port 0 entry back to the correct value
            for handle in pvs_handle_arr:
                modify_pvs_entry(self.client, self.sess_hdl, 0, handle, pvs_handle_map[handle], 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    if in_port in programmed_ing:
                        if in_port in programmed_egr:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)
                        else:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)
                    else:
                        if g_no_1q_port in programmed_egr:
                            InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                        else:
                            assert(0)

            # Case 4: After deleting PVS, verify that no packets, switched like before
            # and instead are seen on drop port.
            print('Case 4')
            sys.stdout.flush()
            # Delete the pipe 0 port 0 entry
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

        def runEdgeCases(self):
            print()
            print('Running runEdgeCases...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}
            parser_value_prog_arr = []
            # without programming pvs (ether-type), check if parse-vlan is skipped
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Use each pvs set element (its value represents ether-type)
            # pvs2 (check pvs.p4) set size is 2
            # Program Parser Value set element with ether-type = .1q
            # .1Q packet will be switched using vlan-id as match key.

            # Case 0-A: First set the gress scope as single gress. Then set the pipe scope different in one gress. then try to set
            # the gress scope as ALL. this should fail. then undo the pipe scope change. then try to set the gress scope as ALL.
            # this should pass. Repeat the above but change the parser scope this time
            print('Case 0-A')
            set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
            set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
            try:
                set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
                print("Able to set gress scope as ALL when the pipe scopes in the 2 gresses didn't match")
                assert(0)
            except:
                print("Setting gress scope as ALL when the pipe scopes in the 2 gresses didn't match expectedly failed")
            set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
            try:
                set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
                print("Able to set gress scope to ALL when the pipe scopes in the 2 gresses matched")
            except:
                print("Unable to set gress scope to ALL even when the pipe scopes in the 2 gresses matched")
                assert(0)

            set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
            set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
            try:
                set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
                print("Able to set gress scope as ALL when the parser scopes in the 2 gresses didn't match")
                assert(0)
            except:
                print("Setting gress scope as ALL when the parser scopes in the 2 gresses didn't match expectedly failed")
            set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
            try:
                set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
                print("Able to set gress scope to ALL when the parser scopes in the 2 gresses matched")
            except:
                print("Unable to set gress scope to ALL even when the parser scopes in the 2 gresses matched")
                assert(0)
            self.conn_mgr.complete_operations(self.sess_hdl)

            # Case 0-B: Get the gress, pipe and parser scopes and see if they match the programmed ones
            print('Case 0-B')
            gress_scope = get_pvs_gress_scope(self.client, self.sess_hdl, dev_id)
            pipe_scope = get_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF))
            parser_scope = get_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF))
            if gress_scope != pvs_property_value_t.PVS_SCOPE_ALL_GRESS:
                print("Read back gress scope didn't match with the set one")
                assert(0)
            if pipe_scope != pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES:
                print("Read back pipe scope didn't match with the set one")
                assert(0)
            if parser_scope != pvs_property_value_t.PVS_SCOPE_ALL_PARSERS:
                print("Read back parser cscope didn't match with the set one")
                assert(0)
            print("Read back Gress, Pipe and Parser scopes matched with the get ones")

            # Case 1: Inject packet with eth-type same as what is programmed in parser
            # Since pvs scope is all pipe, all parsers, use same setting to add pvs element
            print('Case 1')
            sys.stdout.flush()
            for value in ether_types + [1, 2, 3, 4]:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, 0xffff)
                print("PVS ent handle is %x"%(pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Case 2: Try adding another pvs element. This should fail as it will exceed the pvs set size (mentioned in pvs.p4)
            print('Case 2')
            test_failed = 0
            try:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), hex_to_byte(0xFF), 0x9999, 0xffff)
                test_failed = 1
                print("Able to add num of entries exceeding size of PVS. This should not be allowed")
                assert(0)
            except InvalidTableOperation:
                print("Adding an entry to a full PVS expectedly failed")

            # Case 3: Try changing the parser scope while the pvs is non-empty
            print('Case 3')
            test_failed = 0
            try:
                set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Able to change parser scope when the parser was non empty. This should not be allowed")
                test_failed = 1
                assert(0)
            except InvalidTableOperation:
                print("Changing parser scope of a non-empty PVS expectedly failed")

            # Case 4: Try changing the pipe scope while the pvs is non-empty
            print('Case 4')
            test_failed = 0
            try:
                set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Able to change pipe scope when the parser was non empty. This should not be allowed")
                test_failed = 1
                assert(0)
            except InvalidTableOperation:
                print("Changing pipe scope of a non-empty PVS expectedly failed")

            # Case 5: Try changing the gress scope while the pvs is non-empty
            print('Case 5')
            test_failed = 0
            try:
                set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF))
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Able to change gress scope when the parser was non empty. This should not be allowed")
                test_failed = 1
                assert(0)
            except InvalidTableOperation:
                print("Changing gress scope of a non-empty PVS expectedly failed")

            # Delete all the PVS entries
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)
            pvs_handle_arr = []

            # Case 6: Try setting the pipe scope with incorrect gress (remember at this time, the gress scope is symmetric)
            print('Case 6')
            test_failed = 0
            try:
                set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Able to change pipe scope with incorrect gress id. This should not be allowed")
                test_failed = 1
                assert(0)
            except InvalidTableOperation:
                print("Changing pipe scope with an incorrect gress id of PVS expectedly failed")

            # Case 7: Try setting the parser scope with incorrect gress (remember at this time, the gress scope is symmetric)
            print('Case 7')
            test_failed = 0
            try:
                set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Able to change parser scope with incorrect gress id. This should not be allowed")
                test_failed = 1
                assert(0)
            except InvalidTableOperation:
                print("Changing parser scope with an incorrect gress id of PVS expectedly failed")
            # Set the gress scope of the PVS to single gress
            set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)

            # Case 8: Try setting the pipe scope with incorrect gress (remember at this time, the gress scope is asymmetric)
            print('Case 8')
            test_failed = 0
            try:
                set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Able to change pipe scope with incorrect gress id. This should not be allowed")
                test_failed = 1
                assert(0)
            except InvalidTableOperation:
                print("Changing pipe scope with an incorrect gress id of PVS expectedly failed")

            # Case 9: Try setting the parser scope with incorrect gress (remember at this time, the gress scope is asymmetric)
            print('Case 9')
            test_failed = 0
            try:
                set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("Able to change parser scope with incorrect gress id. This should not be allowed")
                test_failed = 1
                assert(0)
            except InvalidTableOperation:
                print("Changing parser scope with an incorrect gress id of PVS expectedly failed")

            # Set the pipe and parser scope of egress
            set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
            set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)

            # At this point egress pvs instance has pipe and parser scope as single whereas ingress pvs instance still has pipe and parser scope as all
            print('Case 10')
            # Case 10: Check if ingress instance has pipe and parser scope as all
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_INGRESS, hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, 0xffff)
                print("PVS ent handle is %x"%(pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, 0x100 + in_port)

            # Delete all the PVS entries
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)
            pvs_handle_arr = []

            print('Case 11')
            # Case 11: Exhaust all the entries in one of the parsers in a pipe in egress and then try adding one more to the same parser and one to another parser in the same pipe. The former should fail while the second one should go through
            egr_pipe = -1
            for p in range(g_num_pipes):
                # Look for a pipe that has two ports in different parsers.
                if len(ports_by_pipe[p]) >= 2:
                    if prsrs_by_pipe[p][ports_by_pipe[p][0]] != prsrs_by_pipe[p][ports_by_pipe[p][1]]:
                        egr_pipe = p
                        break
            assert egr_pipe != -1
            egr_port_0 = ports_by_pipe[egr_pipe][0]
            egr_port_1 = ports_by_pipe[egr_pipe][1]
            for value in ether_types + [1, 2, 3, 4]:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, egr_pipe, prsrs_by_pipe[egr_pipe][egr_port_0], value, 0xffff)
                print("Egress, pipe %d, prsr %d, PVS ent handle is %x"%(egr_pipe, prsrs_by_pipe[egr_pipe][egr_port_0], pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports: # None of the packets should hit the parser in ingress and hence must be switched to the "drop" port.
                    if get_parser_id_for_port(self, egr_port_0) == get_parser_id_for_port(self, g_no_1q_port):
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Try adding one more entry in the same parser
            test_failed = 0
            try:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, egr_pipe, prsrs_by_pipe[egr_pipe][egr_port_0], 0x9999, 0xffff)
                test_failed = 1
                print("Able to add num of entries exceeding size of PVS. This should not be allowed")
                assert(0)
            except InvalidTableOperation:
                print("Adding an entry to a full PVS expectedly failed")
            # Try adding entries in a different parser in the same pipe
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, pvs_gress_t.PVS_GRESS_EGRESS, egr_pipe, prsrs_by_pipe[egr_pipe][egr_port_1], value, 0xffff)
                print("Egress, pipe %d, prsr %d, PVS ent handle is %x"%(egr_pipe, prsrs_by_pipe[egr_pipe][egr_port_1], pvs_handle))
                pvs_handle_arr.append(pvs_handle)
                pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    if get_parser_id_for_port(self, egr_port_0) == get_parser_id_for_port(self, g_no_1q_port):
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                    elif get_parser_id_for_port(self, egr_port_1) == get_parser_id_for_port(self, g_no_1q_port):
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, in_port)
                    else:
                        InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)

            # Delete all the entries
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)
            pvs_handle_arr = []

            # Set the pipe and parser scope of egress to ALL
            set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
            set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)

        def runTestMultiplePVS(self):
            # We have 2 PVS in the p4 program. One decides if we should extract the vlan header and the other decides
            # if we should subsequently extract the ipv4 header. In the case when the second pvs is not programmed,
            # we should not be extracting the ipv4 header and thus the subsequent read_ttl table should not have any effect on the
            # ttl of the outcoming packet. Thus the ttl should be untouched. If we do program the second PVS, the
            # read_ttl table should hit in the egress and the ttl of the outcoming packet should be set to the new value (90)
            print()
            print('Running runTestMultiplePVS...')
            sys.stdout.flush()
            pvs_handle_arr = []
            ttl_pvs_handle_arr = []

            print('Case 1')
            sys.stdout.flush()
            for value in ether_types:
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, 0xffff)
                self.conn_mgr.complete_operations(self.sess_hdl)
                print("PVS ent handle is %x"%(pvs_handle))
                pvs_handle_arr.append(pvs_handle)
            # Since the second pvs is not programmed, the packet should have the default ttl
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipesMultiplePVS(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port, 70)

            print('Case 2')
            # Now program the second pvs. Now the ttl of the packet should be non default ttl = 90
            prsr_tgt = DevParserTarget_t(0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), hex_to_byte(0xFF))
            pvs_handle = self.client.pvs_pvs1_entry_add(self.sess_hdl, prsr_tgt, 0x800, 0xffff)
            self.conn_mgr.complete_operations(self.sess_hdl)
            ttl_pvs_handle_arr.append(pvs_handle)

            # Since the second pvs is programmed, the packet should have the non default ttl
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipesMultiplePVS(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port, 90)

            print('Case 3')
            # Now delete all the entries from the first and the second PVS
            sys.stdout.flush()
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for handle in ttl_pvs_handle_arr:
                self.client.pvs_pvs1_entry_delete(self.sess_hdl, dev_id, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)

            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipesMultiplePVS(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port, 70)

        def runTestExhaustAll(self):
            print()
            print('Running runTestExhaustAll...')
            sys.stdout.flush()
            pvs_handle_arr = []
            pvs_handle_map = {}

            print('Case 1')
            # Case 1: Full blown test. Exhaust both gresses, pipes and parsers
            set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
            set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
            set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
            set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
            set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
            # Get a list of parser IDs
            parser_ids = set()
            for i in range(PORT_MAX):
                parser_ids.add(get_parser_id_for_port(self, i))
            for value in ether_types:
                for gress_id in range (0,2):
                    for pipe_id in range (g_num_pipes):
                        for parser_id in parser_ids:
                            pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, gress_id, pipe_id, parser_id, value, 0xffff)
                            pvs_handle_arr.append(pvs_handle)
                            pvs_handle_map[pvs_handle] = value
            self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, eth_value, in_port)

            # Delete all the PVS entries
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)
            for eth_value in ether_types:
                for in_port in swports:
                    InjectPktVerifyForPipes(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, g_no_1q_port, in_port, eth_value, 0x100 + in_port)
            pvs_handle_arr = []

            set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
            set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
            set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)

        def runTestEgressOnlyPVS(self):
            print()
            print('Running runTestEgressOnlyPVS...')
            sys.stdout.flush()
            mirr_ports = random.sample(swports, 1)
            mirr_pvs_handle_arr = []
            ing_mir = 1
            ing_sid = 1
            egr_mir = 0
            egr_sid = 0
            sid = ing_sid
            dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            p0_hdl = []
            for in_port in mirr_ports:
                mspec = pvs_p0_match_spec_t(in_port)
                aspec = pvs_set_md_action_spec_t(ing_mir, ing_sid, egr_mir, egr_sid)
                h = self.client.p0_table_add_with_set_md(self.sess_hdl, dev_tgt, mspec, aspec)
                p0_hdl.append(h)
                info = mirror_session(MirrorType_e.PD_MIRROR_TYPE_NORM,
                                Direction_e.PD_DIR_INGRESS,
                                sid,
                                in_port,
                                True)
                self.mirror.mirror_session_create(self.sess_hdl, dev_tgt, info)
            self.conn_mgr.complete_operations(self.sess_hdl)

            # Since none of the PVS in ingress are programmed, neither the vlan tag nor the ipv4 header should be extracted
            # Thus the packets should be switched to the drop port (0) by the vlan table. However since the mirror session is
            # active, the packets will show up on the same port on the egress side. Now at this point, pvs5 which exists only
            # only in the egress has not been programmed so the vlan2 table in the egress should miss and thus the vid should of
            # the outcoming packet should be have its vid untouched.
            for in_port in mirr_ports:
                InjectPktVerifyForPipesMirroring(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, 0x8100, 0x100 + in_port, 0x100 + in_port)

            # Now program an entry into pvs5
            prsr_tgt = DevParserTarget_t(0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), hex_to_byte(0xFF))
            pvs_handle = self.client.pvs_pvs5_entry_add(self.sess_hdl, prsr_tgt, 0x8100, 0xffff)
            mirr_pvs_handle_arr.append(pvs_handle)
            self.conn_mgr.complete_operations(self.sess_hdl)

            # Now the packets coming out of egress ports should have the modified vid
            for in_port in mirr_ports:
                InjectPktVerifyForPipesMirroring(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, 0x8100, in_port, 0x100 + in_port)

            # Now delete the entry from pvs5
            for handle in mirr_pvs_handle_arr:
                self.client.pvs_pvs5_entry_delete(self.sess_hdl, dev_id, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)

            # Now the packets coming out of egress ports should have the unmodified vid
            InjectPktVerifyForPipesMirroring(self, hex_to_i16(0xFFFF), in_port, 0x100 + in_port, in_port, g_no_1q_port, 0x8100, 0x100 + in_port, 0x100 + in_port)

            # Finally delete the entries from the phase 0 table
            for h in p0_hdl:
                self.client.p0_table_delete(self.sess_hdl, 0, h)

            # Delete the mirror session
            self.mirror.mirror_session_delete(self.sess_hdl, dev_tgt, sid)
            self.conn_mgr.complete_operations(self.sess_hdl)

        def runTestGetEntry(self):
            print()
            print('Running runTestGetEntry...')
            sys.stdout.flush()
            pvs_handle_arr = []
            num_entries = 2

            # Add and read back and verify entries
            for i in range (num_entries):
                mask = random.randint(0, 65535)
                value = random.randint(0, 65535)
                pvs_handle = add_pvs_entry(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, mask)
                print("PVS ent handle is %x : value is %x : mask is %x "%(pvs_handle, value, mask))
                pvs_handle_arr.append(pvs_handle)
                retrieved_pvs_handle = get_pvs_entry_handle(self.client, self.sess_hdl, 0, hex_to_byte(0xFF), hex_to_i16(0xFFFF), hex_to_byte(0xFF), value, mask)
                if pvs_handle != retrieved_pvs_handle:
                    print("Retrieved Entry Handle %x doesn't match the expected Entry Handle %x"%(retrieved_pvs_handle, pvs_handle))
                    assert(0)
                retrieved_pvs_spec = get_pvs_entry(self.client, self.sess_hdl, 0, retrieved_pvs_handle)
                print("Retrieved PVS ent handle is %x : value is %x : mask is %x "%(retrieved_pvs_handle, retrieved_pvs_spec.parser_value, retrieved_pvs_spec.parser_value_mask))
                if retrieved_pvs_spec.parser_value != value:
                    print("Retrieved parser value %x for entry handle %x does not match the expected parser value %x"%(retrieved_pvs_spec.parser_value, retrieved_pvs_handle, value))
                    assert(0)
                if retrieved_pvs_spec.parser_value_mask != mask:
                    print("Retrieved parser value mask %x for entry handle %x does not match the expected parser value mask %x"%(retrieved_pvs_spec.parser_value_mask, retrieved_pvs_handle, mask))
                    assert(0)

            # Delete the added entries
            for handle in pvs_handle_arr:
                delete_pvs_entry(self.client, self.sess_hdl, 0, handle)
                self.conn_mgr.complete_operations(self.sess_hdl)

        def runTestIterator(self):
            print()
            print('Running runTestGetEntry...')
            sys.stdout.flush()

            prop_gress   = pvs_property_t.PVS_GRESS_SCOPE
            prop_val_gress_all    = pvs_property_value_t.PVS_SCOPE_ALL_GRESS
            prop_val_gress_single = pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS

            prop_pipe   = pvs_property_t.PVS_PIPE_SCOPE
            prop_val_pipe_all    = pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES
            prop_val_pipe_single = pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE

            prop_prsr   = pvs_property_t.PVS_PARSER_SCOPE
            prop_val_prsr_all    = pvs_property_value_t.PVS_SCOPE_ALL_PARSERS
            prop_val_prsr_single = pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER

            gress_all = hex_to_byte(0xFF)
            gress_i   = 0
            gress_e   = 1
            pipe_all  = hex_to_i16(0xFFFF)
            prsr_all  = hex_to_byte(0xFF)

            pipe_to_prsrs = {}
            for pipe in range(g_num_pipes):
                pipe_to_prsrs[pipe] = set()
                for port in ports_by_pipe[pipe]:
                    pipe_to_prsrs[pipe].add( get_parser_id_for_port(self, port) )

            #
            # Test with scope-all
            #
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_gress, prop_val_gress_all, 0)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_pipe, prop_val_pipe_all, 0xFF)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_prsr, prop_val_prsr_all, 0xFF)
            # Get first with a few single scopes, expected to fail since PVS is scope-all.
            for prsr_tgt in [DevParserTarget_t(dev_id, gress_i, pipe_all, prsr_all),
                             DevParserTarget_t(dev_id, gress_e, pipe_all, prsr_all),
                             DevParserTarget_t(dev_id, gress_all, 0, prsr_all),
                             DevParserTarget_t(dev_id, gress_all, pipe_all, 0),
                             DevParserTarget_t(dev_id, gress_i, 1, 4)]:
                try:
                    self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, prsr_tgt)
                    # Expecting the API to fail.
                    self.assertTrue(False)
                except InvalidTableOperation as e:
                    # Expecting an INVALID_PARAM code here.
                    self.assertEqual(e.code, 3)
            # Get first with no entries, expected to fail.
            prsr_tgt = DevParserTarget_t(dev_id, gress_all, pipe_all, prsr_all)
            try:
                self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, prsr_tgt)
                # Expecting the API to fail.
                self.assertTrue(False)
            except InvalidTableOperation as e:
                # Expecting an OBJ_NOT_FOUND code here.
                self.assertEqual(e.code, 6)
            c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, prsr_tgt, False)
            self.assertEqual(0, c)
            # Add one entry, get first should get it, get next should fail.
            h0 = self.client.pvs_pvs2_entry_add(self.sess_hdl, prsr_tgt, 1, 1)
            r = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, prsr_tgt)
            self.assertEqual(h0, r)
            c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, prsr_tgt, False)
            self.assertEqual(1, c)
            try:
                self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, prsr_tgt, r, 1)
                # Expecting the API to fail.
                self.assertTrue(False)
            except InvalidTableOperation as e:
                # Expecting an OBJ_NOT_FOUND code here.
                self.assertEqual(e.code, 6)
            # Add a couple more entries and get all of them.
            h1 = self.client.pvs_pvs2_entry_add(self.sess_hdl, prsr_tgt, 0xF, 0xF)
            h2 = self.client.pvs_pvs2_entry_add(self.sess_hdl, prsr_tgt, 0xA, 0xF)
            hdls = set([h0, h1, h2])
            c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, prsr_tgt, False)
            self.assertEqual(3, c)
            r0 = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, prsr_tgt)
            r1 = self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, prsr_tgt, r0, 1)[0]
            r2 = self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, prsr_tgt, r1, 1)[0]
            try:
                self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, prsr_tgt, r2, 1)
                # Expecting the API to fail.
                self.assertTrue(False)
            except InvalidTableOperation as e:
                # Expecting an OBJ_NOT_FOUND code here.
                self.assertEqual(e.code, 6)
            # No duplicate handles should be returned.
            self.assertNotEqual(r0, r1)
            self.assertNotEqual(r1, r2)
            self.assertNotEqual(r2, r0)
            self.assertIn(r0, hdls)
            self.assertIn(r1, hdls)
            self.assertIn(r2, hdls)
            # Remove the entries and the gets should fail again.
            self.client.pvs_pvs2_entry_delete(self.sess_hdl, dev_id, h0)
            self.client.pvs_pvs2_entry_delete(self.sess_hdl, dev_id, h1)
            self.client.pvs_pvs2_entry_delete(self.sess_hdl, dev_id, h2)
            try:
                self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, prsr_tgt)
                # Expecting the API to fail.
                self.assertTrue(False)
            except InvalidTableOperation as e:
                # Expecting an OBJ_NOT_FOUND code here.
                self.assertEqual(e.code, 6)
            for h in [h0, h1, h2]:
                try:
                    self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, prsr_tgt, h, 1)
                    # Expecting the API to fail.
                    self.assertTrue(False)
                except InvalidTableOperation as e:
                    # Expecting an OBJ_NOT_FOUND code here.
                    self.assertEqual(e.code, 6)
            c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, prsr_tgt, False)
            self.assertEqual(0, c)

            #
            # Test with scope-single
            #
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_gress, prop_val_gress_single, 0)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_pipe, prop_val_pipe_single, 0)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_pipe, prop_val_pipe_single, 1)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_prsr, prop_val_prsr_single, 0)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_prsr, prop_val_prsr_single, 1)
            # Get first with no entries, expected to fail.
            prsr_tgt = DevParserTarget_t(dev_id, gress_all, pipe_all, prsr_all)
            try:
                self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, prsr_tgt)
                # Expecting the API to fail.
                self.assertTrue(False)
            except InvalidTableOperation as e:
                # Expecting an OBJ_NOT_FOUND code here.
                self.assertEqual(e.code, 6)
            # Add some entries keeping track of the handles
            hdls = set()
            hdls_by_gress = {}
            hdls_by_pipe = {}
            hdls_by_prsr = {}
            for gress in [0,1]:
                hdls_by_gress[gress] = set()
                for pipe in range(g_num_pipes):
                    if pipe not in hdls_by_pipe: hdls_by_pipe[pipe] = set()
                    for prsr in pipe_to_prsrs[pipe]:
                        if prsr not in hdls_by_prsr: hdls_by_prsr[prsr] = set()
                        tgt = DevParserTarget_t(dev_id, gress, pipe, prsr)
                        h = self.client.pvs_pvs2_entry_add(self.sess_hdl, tgt, 1, 7);
                        hdls.add(h)
                        hdls_by_gress[gress].add(h)
                        hdls_by_pipe[pipe].add(h)
                        hdls_by_prsr[prsr].add(h)
            # Get all entries
            tgt = DevParserTarget_t(dev_id, gress_all, pipe_all, prsr_all)
            f = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, tgt)
            n = self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, f, 2*len(hdls))
            self.assertEqual(len(n), 2*len(hdls))
            c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, tgt, False)
            self.assertEqual(len(hdls), c)
            # Remove the bad handles
            while -1 in n: n.remove(-1)
            got = set(n + [f])
            self.assertEqual(len(hdls), len(got))
            for h in hdls:
                self.assertIn(h, got)
            # Get next for a specific scope will fail since there is a single entry.
            for gress in [0,1]:
                for pipe in range(g_num_pipes):
                    for prsr in pipe_to_prsrs[pipe]:
                        tgt = DevParserTarget_t(dev_id, gress, pipe, prsr)
                        expected_first = hdls_by_gress[gress] & hdls_by_pipe[pipe] & hdls_by_prsr[prsr]
                        self.assertEqual(len(expected_first), 1)
                        expected_first = expected_first.pop()
                        f = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, tgt)
                        self.assertEqual(f, expected_first)
                        try:
                            self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, f, 1)
                            # Expecting the API to fail.
                            self.assertTrue(False)
                        except InvalidTableOperation as e:
                            # Expecting an OBJ_NOT_FOUND code here.
                            self.assertEqual(e.code, 6)
                        c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, tgt, False)
                        self.assertEqual(1, c)
            # Add a few more entries and try again.
            for gress in [0,1]:
                for pipe in range(g_num_pipes):
                    for prsr in pipe_to_prsrs[pipe]:
                        tgt = DevParserTarget_t(dev_id, gress, pipe, prsr)
                        h1 = self.client.pvs_pvs2_entry_add(self.sess_hdl, tgt, 5, 19);
                        h2 = self.client.pvs_pvs2_entry_add(self.sess_hdl, tgt, 1, 28);
                        h3 = self.client.pvs_pvs2_entry_add(self.sess_hdl, tgt, 9, 17);
                        for h in [h1, h2, h3]:
                            hdls.add(h)
                            hdls_by_gress[gress].add(h)
                            hdls_by_pipe[pipe].add(h)
                            hdls_by_prsr[prsr].add(h)
            # Get all entries
            tgt = DevParserTarget_t(dev_id, gress_all, pipe_all, prsr_all)
            f = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, tgt)
            n = self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, f, 2*len(hdls))
            self.assertEqual(len(n), 2*len(hdls))
            c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, tgt, False)
            self.assertEqual(len(hdls), c)
            # Remove the bad handles
            while -1 in n: n.remove(-1)
            got = set(n + [f])
            self.assertEqual(len(hdls), len(got))
            for h in hdls:
                self.assertIn(h, got)
            # Get for each specific scope
            for gress in [0,1]:
                for pipe in range(g_num_pipes):
                    for prsr in pipe_to_prsrs[pipe]:
                        tgt = DevParserTarget_t(dev_id, gress, pipe, prsr)
                        expected = hdls_by_gress[gress] & hdls_by_pipe[pipe] & hdls_by_prsr[prsr]
                        f = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, tgt)
                        self.assertIn(f, expected)
                        gotten = set([f])
                        h = f
                        for _ in range( len(expected) - 1 ):
                            h = self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, h, 1)[0]
                            gotten.add(h)
                        gotten2 = set(self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, f, 2*len(expected)))
                        gotten2.add(f)
                        while -1 in gotten2: gotten2.remove(-1)
                        self.assertEqual(len(expected), len(gotten))
                        self.assertEqual(len(expected), len(gotten2))
                        for h in expected:
                            self.assertIn(h, gotten)
                            self.assertIn(h, gotten2)
                        c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, tgt, False)
                        self.assertEqual(len(expected), c)
            # Get for less specific scopes (all gress, specific pipe and parser).
            for pipe in range(g_num_pipes):
                for prsr in pipe_to_prsrs[pipe]:
                    tgt = DevParserTarget_t(dev_id, gress_all, pipe, prsr)
                    expected = hdls_by_pipe[pipe] & hdls_by_prsr[prsr]
                    f = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, tgt)
                    gotten = set(self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, f, 2*len(expected)))
                    gotten.add(f)
                    while -1 in gotten: gotten.remove(-1)
                    self.assertEqual(len(expected), len(gotten))
                    for h in expected:
                        self.assertIn(h, gotten)
                    c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, tgt, False)
                    self.assertEqual(len(expected), c)
            # Get for less specific scopes (all gress, all pipe and specific parser).
            for prsr in pipe_to_prsrs[pipe]:
                tgt = DevParserTarget_t(dev_id, gress_all, pipe_all, prsr)
                expected = hdls_by_prsr[prsr]
                f = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, tgt)
                gotten = set(self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, f, 2*len(expected)))
                gotten.add(f)
                while -1 in gotten: gotten.remove(-1)
                self.assertEqual(len(expected), len(gotten))
                for h in expected:
                    self.assertIn(h, gotten)
                c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, tgt, False)
                self.assertEqual(len(expected), c)
            # Get for less specific scopes (all gress, specific pipe and all parser).
            for pipe in range(g_num_pipes):
                tgt = DevParserTarget_t(dev_id, gress_all, pipe, prsr_all)
                expected = hdls_by_pipe[pipe]
                if len(expected) == 0: continue
                f = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, tgt)
                gotten = set(self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, f, 2*len(expected)))
                gotten.add(f)
                while -1 in gotten: gotten.remove(-1)
                self.assertEqual(len(expected), len(gotten))
                for h in expected:
                    self.assertIn(h, gotten)
                c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, tgt, False)
                self.assertEqual(len(expected), c)
            # Get for less specific scopes (specific gress, all pipe and all parser).
            for gress in [0,1]:
                tgt = DevParserTarget_t(dev_id, gress, pipe_all, prsr_all)
                expected = hdls_by_gress[gress]
                f = self.client.pvs_pvs2_entry_get_first_entry_handle(self.sess_hdl, tgt)
                gotten = set(self.client.pvs_pvs2_entry_get_next_entry_handles(self.sess_hdl, tgt, f, 2*len(expected)))
                gotten.add(f)
                while -1 in gotten: gotten.remove(-1)
                self.assertEqual(len(expected), len(gotten))
                for h in expected:
                    self.assertIn(h, gotten)
                c = self.client.pvs_pvs2_entry_get_count(self.sess_hdl, tgt, False)
                self.assertEqual(len(expected), c)

            # Clean up entries.
            for h in hdls:
                self.client.pvs_pvs2_entry_delete(self.sess_hdl, dev_id, h)
            # Reset scope for other tests.
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_prsr, prop_val_prsr_all, 0)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_prsr, prop_val_prsr_all, 1)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_pipe, prop_val_pipe_all, 0)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_pipe, prop_val_pipe_all, 1)
            self.client.pvs_pvs2_set_property(self.sess_hdl, dev_id, prop_gress, prop_val_gress_all, 0)

        dev_id = 0

        runTestIterator(self)

        # Case : Test pvs instance only in egress
        runTestEgressOnlyPVS(self)

        # Case : Exhaust both gresses, all pipes and all parsers
        runTestExhaustAll(self)

        # Case : Testing Multiple PVSs
        runTestMultiplePVS(self)

        # Case : Entry Get
        runTestGetEntry(self)

        # Case : Edge Cases
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        runEdgeCases(self)

        # Case : Scope : All Gress, All Pipes, All Parsers
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        self.conn_mgr.complete_operations(self.sess_hdl)
        runTestAllGressAllPipesAllParsers(self)
        verify_no_other_packets(self, timeout=2)

        # Case : Scope : All Gress, All Pipes, Single Parser
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
        self.conn_mgr.complete_operations(self.sess_hdl)
        runTestAllGressAllPipesSingleParser(self)
        verify_no_other_packets(self, timeout=2)

        # Case : Scope : All Gress, Single Pipe, All Parsers
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        self.conn_mgr.complete_operations(self.sess_hdl)
        runTestAllGressSinglePipeAllParsers(self)
        verify_no_other_packets(self, timeout=2)

        # Case : Scope : All Gress, Single Pipe, Single Parser
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_ALL_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, hex_to_byte(0xFF), pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
        self.conn_mgr.complete_operations(self.sess_hdl)
        runTestAllGressSinglePipeSingleParser(self)
        verify_no_other_packets(self, timeout=2)

        # Case : Scope : Single Gress, All Pipes, All Parsers
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        self.conn_mgr.complete_operations(self.sess_hdl)
        runTestSingleGressAllPipesAllParsers(self)
        verify_no_other_packets(self, timeout=2)

        # Case : Scope : Single Gress, All Pipes, Single Parser
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
        self.conn_mgr.complete_operations(self.sess_hdl)
        if test_param_get('target') == 'hw':
            runTestSingleGressAllPipesSingleParser(self)
        else:
            # This version of the test case assumes the ports in each pipe use
            # the same parser ids (e.g. ports 0, 128, 256, and 384).  This is
            # easy to guarentee on the model but not always the case for every
            # hardware target.
            runTestSingleGressAllPipesSingleParserSW(self)
        verify_no_other_packets(self, timeout=2)

        # Case : Scope : Single Gress, Single Pipe, All Parsers
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        self.conn_mgr.complete_operations(self.sess_hdl)
        runTestSingleGressSinglePipeAllParsers(self)
        verify_no_other_packets(self, timeout=2)

        # Case : Scope : Single Gress, Single Pipe, Single Parser
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
        self.conn_mgr.complete_operations(self.sess_hdl)
        runTestSingleGressSinglePipeSingleParser(self)
        verify_no_other_packets(self, timeout=2)

        #case : Miscellaneous 1
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        runTestMiscellaneousScope1(self)
        verify_no_other_packets(self, timeout=2)

        #case : Miscellaneous 2
        set_pvs_gress_scope(self.client, self.sess_hdl, dev_id, pvs_property_value_t.PVS_SCOPE_SINGLE_GRESS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PIPELINE)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_INGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PARSERS)
        set_pvs_pipe_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_ALL_PIPELINES)
        set_pvs_parser_scope(self.client, self.sess_hdl, dev_id, pvs_gress_t.PVS_GRESS_EGRESS, pvs_property_value_t.PVS_SCOPE_SINGLE_PARSER)
        runTestMiscellaneousScope2(self)
        verify_no_other_packets(self, timeout=2)

        print('All Tests Passed.. Hurray!!!')
        sys.stdout.flush()
