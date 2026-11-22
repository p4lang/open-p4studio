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

import time
import sys
import logging

import pd_base_tests
from ptf.testutils import *
from ptf.thriftutils import *

from pal_rpc.ttypes import *
from pcie_pkt_test.p4_pd_rpc.ttypes import *
from pkt_pd_rpc.ttypes import *
from mc_pd_rpc.ttypes import *
from res_pd_rpc.ttypes import *
from devport_mgr_pd_rpc.ttypes import *
import p4testutils.misc_utils as misc_utils
import p4testutils.pal_utils as pal_utils

dev_id = 0

global sess_pkt0
global sess_pkt1
global sess_pkt2
global sess_pkt3

# First grab all ports
logger = misc_utils.get_logger()
swports = misc_utils.get_sw_ports()

def port_tbl_add(test, shdl, dev_tgt, port, dport):
    match_spec = pcie_pkt_test_port_tbl_match_spec_t( hex_to_i16(port) )
    action_spec = pcie_pkt_test_set_md_action_spec_t( hex_to_i16(dport) )
    test.client.port_tbl_table_add_with_set_md( shdl, dev_tgt, match_spec, action_spec )

def match_pkt_string(test, to_pkt, str1):
    if (str(to_pkt) == str(str1)):
        logging.debug("matched")
        return True
    else:
        logging.debug("not matched")
        return False

class TestPipelineTable(pd_base_tests.EmptyTableTest):
    program = "pcie_pkt_test"
    table = "port_tbl"


@group("thorough")
class TestPG(pd_base_tests.ThriftInterfaceDataPlane):
    def __init__(self):
        pd_base_tests.ThriftInterfaceDataPlane.__init__(self, ["pcie_pkt_test"])
        sys.stdout.flush()

    def verify_one_pass(self):
        sess_pkt0 = sess_pkt1 = sess_pkt2 = sess_pkt3 = -1
        try:
            self.pkt.init()
            sess_pkt0 = self.pkt.client_init()
            sess_pkt1 = self.pkt.client_init()
            sess_pkt2 = self.pkt.client_init()
            sess_pkt3 = self.pkt.client_init()
            p0 = simple_eth_packet(pktlen=1000)
            p1 = simple_eth_packet(pktlen=100)
            p2 = simple_eth_packet(pktlen=500)
            p3 = simple_eth_packet(pktlen=1400)

            for s,p,ring in zip([sess_pkt0, sess_pkt1, sess_pkt2, sess_pkt3], [p0, p1, p2, p3], range(4)):
                self.pkt.test_send_pkt(s, bytes(p), len(p), ring)
                time.sleep(1)
                try:
                    self.pkt.test_verify_pkt_tx(s)
                    logger.info("Tx verify okay on ring {}".format(ring))
                except:
                    logger.info("Session {} Tx verify failed".format(ring))
                    sys.stdout.flush()
                    raise
                try:
                    verify_packet(self, p, swports[0])
                    logger.info("Tx okay on ring {}".format(ring))
                except:
                    logger.info("Rx for pkt sent through ring {} failed".format(ring))
                    sys.stdout.flush()
                    raise

            try:
                verify_no_other_packets(self, timeout=2)
            except:
                logger.info("Unexpected packet received")
                sys.stdout.flush()
                raise

            pkt_lens = [256, 708, 1200, 500, 873, 1105, 302, 83, 803]
            rx_rings = [0, 1, 5, 2, 7, 4, 6, 3, 3]
            for pkt_len, rx_ring in zip(pkt_lens, rx_rings):
                dmac = "8%d:01:02:03:04:05" % rx_ring
                p = simple_eth_packet(pktlen=pkt_len, eth_dst=dmac)
                send_packet(self, swports[0], p)
                time.sleep(1)
                rx_pkt = self.pkt.test_get_pkt_rx(sess_pkt0)
                retValue = match_pkt_string(self, p, rx_pkt.buf)
                try:
                    self.assertTrue(retValue == True, "Rx pkt did not match the Tx pkt")
                    logger.info("Rx okay {} bytes  on ring {}".format(pkt_len, rx_ring))
                    sys.stdout.flush()
                except:
                    logger.info("Expected: {}".format(format_packet(p)))
                    if rx_pkt is not None:
                        logger.info("Received:")
                        logger.info(" RxRing: {}".format(rx_pkt.rx_ring))
                        logger.info(" Size: {}".format(rx_pkt.size))
                        logger.info(" {}".format(hex_dump_buffer(rx_pkt.buf)))
                    else:
                        logger.info("No rx packet")
                    sys.stdout.flush()
                    raise

        finally:
            if sess_pkt3 != -1:
                self.pkt.client_cleanup(sess_pkt3)
            if sess_pkt0 != -1:
                self.pkt.client_cleanup(sess_pkt0)
            if sess_pkt2 != -1:
                self.pkt.client_cleanup(sess_pkt2)
            if sess_pkt1 != -1:
                self.pkt.client_cleanup(sess_pkt1)

            self.pkt.cleanup()

    def configure(self, sess_hdl, dev_id):

        dev_tgt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))

        cpu_pcie_port = self.devport_mgr.devport_mgr_pcie_cpu_port_get(dev_id)

        port_tbl_add(self, sess_hdl, dev_tgt, swports[0], cpu_pcie_port)

        # Program tables such that a packet coming in on port 320 (CPU PCIe)
        # will go to port 1.
        port_tbl_add(self, sess_hdl, dev_tgt, cpu_pcie_port, swports[0])

    def runTest(self):
        num_pipes = int(test_param_get('num_pipes'))
        logger.info("Num pipes is {}".format(num_pipes))
        sys.stdout.flush()
        cleanup = True

        try:
            sess_hdl = self.conn_mgr.client_init()
            self.configure(sess_hdl, dev_id)

            # Wait for all pipe APIs to complete.
            self.conn_mgr.complete_operations(sess_hdl)

            logger.info('Testing after cold boot')
            sys.stdout.flush()
            self.verify_one_pass()
            logger.info('  Done')
            sys.stdout.flush()

            logger.info('Initiating fast reconfig')
            sys.stdout.flush()
            #Perform a fast reconfig
            try:
                self.devport_mgr.devport_mgr_warm_init_begin(dev_id, dev_init_mode.DEV_WARM_INIT_FAST_RECFG, dev_serdes_upgrade_mode.DEV_SERDES_UPD_NONE, True)
            except InvalidDevportMgrOperation:
                logger.info('Failure in warm_init_begin')
                sys.stdout.flush()
                cleanup = False
                raise

            if test_param_get('target') == 'hw':
                pal_utils.add_ports(self)

            logger.info('Testing during warm init')
            sys.stdout.flush()
            self.verify_one_pass()
            logger.info('  Done')
            sys.stdout.flush()

            self.configure(sess_hdl, dev_id)
            self.devport_mgr.devport_mgr_warm_init_end(dev_id)

            self.conn_mgr.complete_operations(sess_hdl)
            logger.info('Testing after warm init completed')
            sys.stdout.flush()

            pal_utils.check_port_status(self, swports)

            self.verify_one_pass()
            logger.info('  Done')
            sys.stdout.flush()

            logger.info('Success!')
            sys.stdout.flush()

        finally:
            if not cleanup: return
            logger.info('Cleaning up')
            sys.stdout.flush()
            dt = DevTarget_t(dev_id, hex_to_i16(0xFFFF))
            c = self.client.port_tbl_get_entry_count(sess_hdl, dt)
            logger.info("{} entries in port_tbl".format(c))
            sys.stdout.flush()
            for _ in range(c):
                h = self.client.port_tbl_get_first_entry_handle(sess_hdl, dt)
                logger.info("Removing entry {}".format(h))
                sys.stdout.flush()
                self.client.port_tbl_table_delete(sess_hdl, dev_id, h)
            self.conn_mgr.client_cleanup(sess_hdl)
            logger.info('Cleanup done')
            sys.stdout.flush()
