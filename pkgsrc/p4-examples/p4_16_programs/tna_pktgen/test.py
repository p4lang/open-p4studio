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

import random
import time
import pdb

import ptf
from ptf import config
from ptf.thriftutils import *
import ptf.testutils as testutils
import p4testutils.misc_utils as misc_utils
import p4testutils.ptf_port as ptf_port
from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as gc
import grpc

g_is_tofino = testutils.test_param_get('arch') == 'tofino'
g_is_tofino2 = testutils.test_param_get('arch') == 'tofino2'
assert g_is_tofino or g_is_tofino2
g_is_hw = testutils.test_param_get('target') == 'hw'
g_is_model = not g_is_hw

dev_id = 0
client_id = 0
p4_name = 'tna_pktgen'

logger = misc_utils.get_logger()
swports = misc_utils.get_sw_ports()


def get_pipes(bfrt_info):
    trgt = gc.Target(device_id=dev_id)
    t = bfrt_info.table_get('device_configuration')
    resp = t.default_entry_get(trgt)
    data,_ = next(resp)
    data_dict = data.to_dict()
    num_pipes = data_dict['num_pipes']
    return list(range(num_pipes))

def tbl_set_asymmetric(tbl):
      mode = bfruntime_pb2.Mode.SINGLE
      tbl.attribute_entry_scope_set(gc.Target(device_id=dev_id),
                                    predefined_pipe_scope=True,
                                    predefined_pipe_scope_val=mode)

def configure_vs(bfrt_info, pipes, timer_app_id=None, port_down_app_ids=[], recirc_app_id=None, dprsr_app_id=None):
    vs_names = ['IPrsr.pgen_timer', 'IPrsr.pgen_port_down', 'IPrsr.pgen_recirc']
    if not g_is_tofino:
        vs_names.append('IPrsr.pgen_dprsr')
    mode = bfruntime_pb2.Mode.SINGLE
    for name in vs_names:
        vs = bfrt_info.table_get(name)
        vs.attribute_entry_scope_set(gc.Target(device_id=dev_id),
                                     config_pipe_scope=True, predefined_pipe_scope=True, predefined_pipe_scope_val=mode,
                                     config_gress_scope=True, predefined_gress_scope_val=bfruntime_pb2.Mode.ALL,
                                     config_prsr_scope=True, predefined_prsr_scope_val=mode)
    # Tofino-1 uses the last port-group on each pipe (port 68 in the pipe) for
    # packet generation, this is tied to the last parser (17) in the pipe.
    # Tofino-2 uses the first port-group on each pipe (ports 0-7) so we use a
    # parser id of zero there.
    prsr_id = 17 if g_is_tofino else 0
    # Tofino-1 uses a 3-bit (0-7) app id while Tofino-2 uses a 4-bit (0-15) app
    # id.
    def make_vs_key(pipe_id, app_id):
        if g_is_tofino:
            return (pipe_id << 3) | app_id
        else:
            return (pipe_id << 4) | app_id
    def make_vs_msk():
        return 0x1F if g_is_tofino else 0x3F

    if timer_app_id is not None:
        app_id = timer_app_id
        vs_name = 'IPrsr.pgen_timer'
        vs = bfrt_info.table_get(vs_name)
        for pipe in pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe, prsr_id=prsr_id)
            vs.entry_add(trgt,
                         [vs.make_key([gc.KeyTuple('f1', make_vs_key(pipe, app_id), make_vs_msk())])])
    for port_down_app_id in port_down_app_ids:
        app_id = port_down_app_id
        vs_name = 'IPrsr.pgen_port_down'
        vs = bfrt_info.table_get(vs_name)
        for pipe in pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe, prsr_id=prsr_id)
            vs.entry_add(trgt,
                         [vs.make_key([gc.KeyTuple('f1', make_vs_key(pipe, app_id), make_vs_msk())])])
    if recirc_app_id is not None:
        app_id = recirc_app_id
        vs_name = 'IPrsr.pgen_recirc'
        vs = bfrt_info.table_get(vs_name)
        for pipe in pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe, prsr_id=prsr_id)
            vs.entry_add(trgt,
                         [vs.make_key([gc.KeyTuple('f1', make_vs_key(pipe, app_id), make_vs_msk())])])
    if dprsr_app_id is not None:
        app_id = dprsr_app_id
        vs_name = 'IPrsr.pgen_dprsr'
        vs = bfrt_info.table_get(vs_name)
        for pipe in pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe, prsr_id=prsr_id)
            vs.entry_add(trgt,
                         [vs.make_key([gc.KeyTuple('f1', make_vs_key(pipe, app_id), make_vs_msk())])])

def clean_pvs(bfrt_info, pipes):
    vs_names = ['IPrsr.pgen_timer', 'IPrsr.pgen_port_down', 'IPrsr.pgen_recirc']
    if not g_is_tofino:
        vs_names.append('IPrsr.pgen_dprsr')
    prsr_id = 17 if g_is_tofino else 0
    for name in vs_names:
        vs = bfrt_info.table_get(name)
        for pipe in pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe, prsr_id=prsr_id)
            vs.entry_del(trgt, [])

def configure_recirc_vs(bfrt_info, recirc_port, recirc_tag_msb):
    vs = bfrt_info.table_get('IPrsr.recirc')
    logger.info("Value Set IPrsr.recirc: port %d tag_msb 0x%x", recirc_port, recirc_tag_msb)
    trgt = gc.Target(device_id=dev_id)
    vs.entry_add(trgt,
                 [vs.make_key([gc.KeyTuple('port', recirc_port, 0x1FF),
                               gc.KeyTuple('recirc_tag_msb', recirc_tag_msb, 0xFF)])])

def clean_recirc_pvs(bfrt_info):
    vs = bfrt_info.table_get('IPrsr.recirc')
    trgt = gc.Target(device_id=dev_id)
    vs.entry_del(trgt, [])


class OneShotTimer(BfRuntimeTest):
    """
    Example usage of the One-Shot Timer Packet Generator Application.
    This app type will trigger the Packet Generator exactly once when the timer
    expires.  Once triggered the Packet Generator will create N batches of
    packets, each batch containing M packets.  This test shows the required
    steps to use the Packet Generator.  It also has a few examples of get-APIs
    for these tables.  The steps this test performs are:
      - Configure the Value Sets in the ingress parser to match the generated
        packets.
      - Add match table entries to verify the generated packets.
      - Enables packet generation on the internal Packet Generator ports.
      - Programs a packet into the Packet Generator's buffer.
      - Configures a Packet Generator application.
      - Verifies the expected packets are created by the application.
    """
    def setUp(self):
        misc_utils.setup_random()
        BfRuntimeTest.setUp(self, client_id, p4_name)
        self.bfrt_info = self.interface.bfrt_info_get(p4_name)
        self.pipes = get_pipes(self.bfrt_info)
        self.num_pipes = len(self.pipes)

    def tearDown(self):
        # Clear the Parser Value Set entries.
        clean_pvs(self.bfrt_info, self.pipes)
        # Clean up the entries in the verify table.
        verify_tbl = self.bfrt_info.table_get('t_timer_app')
        for pipe in self.pipes:
            verify_tbl.entry_del(gc.Target(device_id=dev_id, pipe_id=pipe), [])
        # Ensure all apps are disabled
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        for app_id in range(t.info.size_get()):
            t.entry_mod(gc.Target(device_id=dev_id),
                                  [t.make_key([gc.KeyTuple('app_id', app_id)])],
                                  [t.make_data([gc.DataTuple('app_enable', bool_val=False)])])
        # Clear config in the app table
        t.entry_del(gc.Target(device_id=dev_id), [])

        # We are leaving the data in the Packet Generator Buffer table, there is
        # no harm in leaving that config in place since the app table has been
        # cleaned up.

        BfRuntimeTest.tearDown(self)

    def runTest(self):
        # This test requires at least one port for each pipe on the chip.
        if len(swports) < self.num_pipes:
            logger.error("Requires a minimum of %d ports to run correctly but have only %d", self.num_pipes, len(swports))
            self.assertGreaterEqual(len(swports), self.num_pipes)

        app_tbl = self.bfrt_info.table_get('pktgen.app_cfg')
        pkt_buf_tbl = self.bfrt_info.table_get('pktgen.pkt_buffer')

        app_id = random.choice(range(app_tbl.info.size_get()))
        generated_pkt_len = 100
        generated_pkt = testutils.simple_ip_packet(pktlen=generated_pkt_len)
        batch_cnt = 3
        pkt_per_batch = 4

        # Our parser program uses Value Sets to identify packets from the Packet
        # Generator (see P4 for details).  Program the VSs now to identify the
        # packets our timer app will create.
        logger.info("Configuring value sets")
        configure_vs(self.bfrt_info, self.pipes, timer_app_id=app_id)

        # Each pipe has a Packet Generator, setup the verification table to be
        # asymmetric so we can program it per pipe to verify the generated
        # packets on that pipe.
        logger.info("Configuring verify table")
        verify_tbl = self.bfrt_info.table_get('t_timer_app')
        tbl_set_asymmetric(verify_tbl)

        # Install entries in the t_timer_app table to match the generated
        # packets.  The packets generated by the per-pipe Packet Generators will
        # forward out a port, all packets from a given pipe will forward out a
        # specific port.  Packets from different pipes will not use the same
        # port.
        ids = [(b,p) for b in range(batch_cnt) for p in range(pkt_per_batch)]
        verify_keys_by_pipe = dict()
        for pipe in self.pipes:
            keys = [verify_tbl.make_key([gc.KeyTuple('hdr.timer.pipe_id', pipe),
                                         gc.KeyTuple('hdr.timer.app_id', app_id),
                                         gc.KeyTuple('hdr.timer.batch_id', b),
                                         gc.KeyTuple('hdr.timer.packet_id', p)]) for b,p in ids]
            verify_keys_by_pipe[pipe] = keys
        eg_port_by_pipe = {pipe: swports[pipe] for pipe in self.pipes}

        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            d = verify_tbl.make_data([gc.DataTuple('dst_port', eg_port_by_pipe[pipe])],
                                     'Ing.count_timer_app_and_fwd')
            verify_tbl.entry_add(trgt,
                                 verify_keys_by_pipe[pipe],
                                 [d]*(batch_cnt*pkt_per_batch))

        # The following steps are required and can be done in any order, except
        # for enabling the Packet Generator application which should be done
        # last.
        #   - Enable packet generation on the generator port
        #   - Program the packet to generate into the packet buffer
        #   - Configure the Packet Generator application

        # Enable packet generation on the generator port.  For Tofino-1 this
        # should be port 68 in each pipe (68, 196, 324, 452).  For Tofino-2
        # this can be any of the ports in the 0-7 range on each pipe.  We will
        # use port 6 in each pipe for this example since that port has been
        # created by default in each pipe.  Note that in pipe 0 the CPU ports
        # are on 0 and 2-5.  To use them for packet generation those ports must
        # be removed, changed from "CPU mode" to "recirculation mode" and then
        # recreated.
        pgen_port = 68 if g_is_tofino else 6
        pgen_ports = [misc_utils.make_port(pipe, pgen_port) for pipe in self.pipes]
        logger.info("Configuring packet gen ports: %s", pgen_ports)
        t = self.bfrt_info.table_get('pktgen.port_cfg')
        keys = [t.make_key([gc.KeyTuple('dev_port', port)]) for port in pgen_ports]
        data = [t.make_data([gc.DataTuple('pktgen_enable', bool_val=True)]) for _ in self.pipes]
        t.entry_mod(gc.Target(device_id=dev_id), keys, data)
        # Do an entry get to read it back.  This is not required, it is just an
        # example to illustrate gets on this table.
        resp = t.entry_get(gc.Target(device_id=dev_id), keys, {'from_hw':False})
        seen = list()
        for d,k in resp:
            self.assertTrue(d.to_dict()['pktgen_enable'])
            seen.append(k.to_dict()['dev_port']['value'])
        self.assertEqual(sorted(seen), sorted(pgen_ports))

        # Program the packet into the packet buffer.  The packet buffer is 16kB
        # divided into 1024 "lines", each holding 16B.  The packet should be
        # aligned with a 16B alignment in the buffer.  For this example we will
        # pick a random location within the packet buffer.
        num_lines = (generated_pkt_len + 15) // 16
        first_line = 0
        last_line = 1024 - num_lines
        line = random.randint(first_line, last_line)
        buf_offset = line * 16
        logger.info("Configuring packet buffer at offset: %d", buf_offset)
        t = self.bfrt_info.table_get('pktgen.pkt_buffer')
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('pkt_buffer_offset', buf_offset),
                                 gc.KeyTuple('pkt_buffer_size', generated_pkt_len)])],
                    [t.make_data([gc.DataTuple('buffer', bytearray(bytes(generated_pkt)))])])
        # Read it back as an example to illustrate how the get works.
        # Note that the get returns a list of bytes, so we transform the packet
        # into that format when comparing with the get result.
        resp = t.entry_get(gc.Target(device_id=dev_id),
                           [t.make_key([gc.KeyTuple('pkt_buffer_offset', buf_offset),
                                        gc.KeyTuple('pkt_buffer_size', generated_pkt_len)])],
                           {'from_hw': False})
        data,_ = next(resp)
        data_dict = data.to_dict()
        self.assertEqual([int(x) for x in bytes(generated_pkt)], data_dict['buffer'])

        # Configure the Packet Generator app.
        logger.info("Configuring app number %d", app_id)
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        data_flds = [gc.DataTuple('timer_nanosec', 1000),
                     # Configure the app to generate the packet we programmed
                     # into the packet buffer
                     gc.DataTuple('pkt_len', generated_pkt_len),
                     gc.DataTuple('pkt_buffer_offset', buf_offset),
                     # Program the number of packets to generate.  Note these
                     # values are zero based, so a value of zero makes one
                     # packet and a value of ten makes eleven packets.
                     gc.DataTuple('batch_count_cfg', batch_cnt-1),
                     gc.DataTuple('packets_per_batch_cfg', pkt_per_batch-1),
                     # Program the Inter-Packet-Gap (IPG) and Inter-Batch-Gap
                     # (IBG) in nanoseconds.  Note that on Tofino-1 this is
                     # the time from the start of one packet/batch to the start
                     # of the next.  On Tofino-2 this is the time from the end
                     # of a packet/batch to the start of the next.  We will use
                     # values of zero here since we do not need any additional
                     # delay.
                     gc.DataTuple('ipg', 0),
                     gc.DataTuple('ibg', 0),
                     # Reset the per-app counters to zero since we are setting
                     # up a new application.
                     gc.DataTuple('trigger_counter', 0),
                     gc.DataTuple('batch_counter', 0),
                     gc.DataTuple('pkt_counter', 0),
                     # Leave the app disabled for now, once it is enabled the
                     # hardware will immediately start a count down equal to the
                     # timer_nanosec field and the app will trigger once it
                     # reaches zero.
                     gc.DataTuple('app_enable', bool_val=False)]
        # Tofino-2 has an additional required field.  Tofino-1 would generally
        # only have port 68 in a pipe enabled for packet generation and that
        # port will be used.  Tofino-2 however may have multiple ports enabled
        # for packet generation (within the 0-7 range on the pipe) and each
        # app must be explicitly configured to use one specfic port.
        if not g_is_tofino:
            data_flds.append(gc.DataTuple('assigned_chnl_id', pgen_port))
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('app_id', app_id)])],
                    [t.make_data(data_flds, 'trigger_timer_one_shot')])
        # Perform an entry get to demonstrate the get API.
        resp = t.entry_get(gc.Target(device_id=dev_id),
                           [t.make_key([gc.KeyTuple('app_id', app_id)])])
        d,k = next(resp)
        self.assertEqual(k, t.make_key([gc.KeyTuple('app_id', app_id)]))
        data_dict = d.to_dict()
        self.assertEqual(data_dict['timer_nanosec'], 1000)
        self.assertEqual(data_dict['pkt_len'], generated_pkt_len)
        self.assertEqual(data_dict['pkt_buffer_offset'], buf_offset)
        self.assertEqual(data_dict['batch_count_cfg'], batch_cnt-1)
        self.assertEqual(data_dict['packets_per_batch_cfg'], pkt_per_batch-1)
        self.assertEqual(data_dict['ipg'], 0)
        self.assertEqual(data_dict['ibg'], 0)
        self.assertEqual(data_dict['trigger_counter'], 0)
        self.assertEqual(data_dict['batch_counter'], 0)
        self.assertEqual(data_dict['pkt_counter'], 0)
        self.assertEqual(data_dict['app_enable'], False)
        self.assertEqual(data_dict['action_name'], 'trigger_timer_one_shot')

        # Enable the app.  Since we are using a target addressing "Pipe All",
        # the Packet Generator in each pipe will turn on, count down its
        # timer, and create "batch_cnt" batches of "pkt_per_batch" packets.
        logger.info("Enabling app")
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('app_id', app_id)])],
                    [t.make_data([gc.DataTuple('app_enable', bool_val=True)])])

        # Read the app config table to get the per-app counters to check if
        # packet generation is complete.  Again, we expect a number of
        # triggers equal to the number of pipes.  For each trigger we expect
        # the full batch count and we expect a packet count equal to the
        # number of batches times the packets per batch.
        # On hardware this should complete immediately but on the model it
        # may take a little time depending on the number of packets.
        num_polls = 0
        exp_triggers = self.num_pipes
        exp_batches = exp_triggers * batch_cnt
        exp_packets = exp_batches * pkt_per_batch
        while True:
            resp = t.entry_get(gc.Target(device_id=dev_id),
                               [t.make_key([gc.KeyTuple('app_id', app_id)])])
            data_dict = next(resp)[0].to_dict()
            num_polls += 1
            triggers = data_dict['trigger_counter']
            batches = data_dict['batch_counter']
            packets = data_dict['pkt_counter']

            if triggers == exp_triggers and batches == exp_batches and packets == exp_packets:
                # All packets are received, we can stop polling
                break
            if triggers > exp_triggers or batches > exp_batches or packets > exp_packets:
                # Oops, too many triggers/batches/packets!
                logger.error("Unexpected app counters")
                logger.error("Expected triggers %d, got %d", exp_triggers, triggers)
                logger.error("Expected batches %d, got %d", exp_batches, batches)
                logger.error("Expected packets %d, got %d", exp_packets, packets)
                self.assertTrue(False)
            # Not all packets have been generated yet.  If this is hardware
            # give it one additional polling attempt (it should have finished
            # very quickly).  If it is the model assume a worst case of one
            # packet per second processing rate in the model, wait and poll
            # once more.
            if g_is_model:
                if num_polls > exp_packets:
                    logger.error("Not all packets have been generated")
                    self.assertTrue(False)
                time.sleep(1)
            else:
                if num_polls > 1:
                    logger.error("Not all packets have been generated")
                    self.assertTrue(False)
                time.sleep(0.1)
        # Even though the app has completed the packet generation, it is still
        # in the enabled state; it does not "auto-disable".  It must be disabled
        # and then re-enabled to trigger again.
        self.assertTrue(data_dict['app_enable'])

        # All packets have been generated, receive all the copies to ensure
        # the expected packet data was created.  Since we are not re-writting
        # the packets in the pipeline and the internal Packet Generator header
        # is removed at the ingress deparser, all packets will be the same.
        # If running on the model it is possible that the packets have been
        # generated but the model is still (slowly) processing them.  Wait a
        # little extra time in case the model still has packets in the pipeline.
        if g_is_model:
            time.sleep(batch_cnt * pkt_per_batch)
        for pipe in self.pipes:
            eg_port = swports[pipe]
            for _ in range(batch_cnt*pkt_per_batch):
                testutils.verify_packet(self, generated_pkt, swports[pipe])

        # Ensure the verify entries were correctly matched.
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            resp = verify_tbl.entry_get(trgt, verify_keys_by_pipe[pipe])
            resp_cnt = 0
            for d,k in resp:
                resp_cnt += 1
                dd = d.to_dict()
                # The generated packets will have an Ethernet FCS (4 bytes)
                # added to them as well as a 6 byte internal Packet Generator
                # header.  These increase the expected size of the packet.
                exp_size = generated_pkt_len + 6 + 4
                if dd['$COUNTER_SPEC_PKTS'] != 1 or dd['$COUNTER_SPEC_BYTES'] != exp_size:
                    logger.error("Pipe %d: Verify entry had unexpected counts:\n%s\n%s", pipe, k.to_dict(), dd)
                    self.assertEqual(dd['$COUNTER_SPEC_PKTS'], 1)
                    self.assertEqual(dd['$COUNTER_SPEC_BYTES'], exp_size)
            self.assertEqual(resp_cnt, batch_cnt * pkt_per_batch)



class PeriodicTimer(BfRuntimeTest):
    """
    Example usage of the Periodic Timer Packet Generator Application.
    This app type will trigger the Packet Generator exactly once when the timer
    expires.  Once triggered the Packet Generator will create N batches of
    packets, each batch containing M packets.  The application will continue to
    trigger at the interval specified by the timer value.
    This test shows the basic steps to configure a periodic timer application.
    It is similar to the OneShotTimer test but uses a Periodic Timer app type
    and ommits some of the get-API examples.
    """
    def setUp(self):
        misc_utils.setup_random()
        BfRuntimeTest.setUp(self, client_id, p4_name)
        self.bfrt_info = self.interface.bfrt_info_get(p4_name)
        self.pipes = get_pipes(self.bfrt_info)
        self.num_pipes = len(self.pipes)

    def tearDown(self):
        # Clear the Parser Value Set entries.
        clean_pvs(self.bfrt_info, self.pipes)
        # Clean up the entries in the verify table.
        verify_tbl = self.bfrt_info.table_get('t_timer_app')
        for pipe in self.pipes:
            verify_tbl.entry_del(gc.Target(device_id=dev_id, pipe_id=pipe), [])
        # Ensure all apps are disabled
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        for app_id in range(t.info.size_get()):
            t.entry_mod(gc.Target(device_id=dev_id),
                                  [t.make_key([gc.KeyTuple('app_id', app_id)])],
                                  [t.make_data([gc.DataTuple('app_enable', bool_val=False)])])
        # Clear config in the app table
        t.entry_del(gc.Target(device_id=dev_id), [])

        # We are leaving the data in the Packet Generator Buffer table, there is
        # no harm in leaving that config in place since the app table has been
        # cleaned up.

        BfRuntimeTest.tearDown(self)

    def runTest(self):
        app_tbl = self.bfrt_info.table_get('pktgen.app_cfg')
        pkt_buf_tbl = self.bfrt_info.table_get('pktgen.pkt_buffer')

        app_id = random.choice(range(app_tbl.info.size_get()))
        generated_pkt_len = 100
        generated_pkt = testutils.simple_ip_packet(pktlen=generated_pkt_len)
        batch_cnt = 3
        pkt_per_batch = 4

        # Our parser program uses Value Sets to identify packets from the Packet
        # Generator (see P4 for details).  Program the VSs now to identify the
        # packets our timer app will create.
        logger.info("Configuring value sets")
        configure_vs(self.bfrt_info, self.pipes, timer_app_id=app_id)

        # Each pipe has a Packet Generator, setup the verification table to be
        # asymmetric so we can program it per pipe to verify the generated
        # packets on that pipe.
        logger.info("Configuring verify table")
        verify_tbl = self.bfrt_info.table_get('t_timer_app')
        tbl_set_asymmetric(verify_tbl)

        # Install entries in the t_timer_app table to match the generated
        # packets.  The packets generated by the per-pipe Packet Generators will
        # be counted and then dropped.
        ids = [(b,p) for b in range(batch_cnt) for p in range(pkt_per_batch)]
        verify_keys_by_pipe = dict()
        for pipe in self.pipes:
            keys = [verify_tbl.make_key([gc.KeyTuple('hdr.timer.pipe_id', pipe),
                                         gc.KeyTuple('hdr.timer.app_id', app_id),
                                         gc.KeyTuple('hdr.timer.batch_id', b),
                                         gc.KeyTuple('hdr.timer.packet_id', p)]) for b,p in ids]
            verify_keys_by_pipe[pipe] = keys

        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            d = verify_tbl.make_data([], 'Ing.count_timer_app_and_drop')
            verify_tbl.entry_add(trgt,
                                 verify_keys_by_pipe[pipe],
                                 [d]*(batch_cnt*pkt_per_batch))

        # The following steps are required and can be done in any order, except
        # for enabling the Packet Generator application which should be done
        # last.
        #   - Enable packet generation on the generator port
        #   - Program the packet to generate into the packet buffer
        #   - Configure the Packet Generator application

        # Enable packet generation on the generator port.  For Tofino-1 this
        # should be port 68 in each pipe (68, 196, 324, 452).  For Tofino-2
        # this can be any of the ports in the 0-7 range on each pipe.  We will
        # use port 6 in each pipe for this example since that port has been
        # created by default in each pipe.  Note that in pipe 0 the CPU ports
        # are on 0 and 2-5.  To use them for packet generation those ports must
        # be removed, changed from "CPU mode" to "recirculation mode" and then
        # recreated.
        pgen_port = 68 if g_is_tofino else 6
        pgen_ports = [misc_utils.make_port(pipe, pgen_port) for pipe in self.pipes]
        logger.info("Configuring packet gen ports: %s", pgen_ports)
        t = self.bfrt_info.table_get('pktgen.port_cfg')
        keys = [t.make_key([gc.KeyTuple('dev_port', port)]) for port in pgen_ports]
        data = [t.make_data([gc.DataTuple('pktgen_enable', bool_val=True)]) for _ in self.pipes]
        t.entry_mod(gc.Target(device_id=dev_id), keys, data)

        # Program the packet into the packet buffer.  The packet buffer is 16kB
        # divided into 1024 "lines", each holding 16B.  The packet should be
        # aligned with a 16B alignment in the buffer.  For this example we will
        # pick a random location within the packet buffer.
        num_lines = (generated_pkt_len + 15) // 16
        first_line = 0
        last_line = 1024 - num_lines
        line = random.randint(first_line, last_line)
        buf_offset = line * 16
        logger.info("Configuring packet buffer at offset: %d", buf_offset)
        t = self.bfrt_info.table_get('pktgen.pkt_buffer')
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('pkt_buffer_offset', buf_offset),
                                 gc.KeyTuple('pkt_buffer_size', generated_pkt_len)])],
                    [t.make_data([gc.DataTuple('buffer', bytearray(bytes(generated_pkt)))])])

        # Configure the Packet Generator app.
        logger.info("Configuring app number %d", app_id)
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        data_flds = [gc.DataTuple('timer_nanosec', 1000000), # 1ms
                     # Configure the app to generate the packet we programmed
                     # into the packet buffer
                     gc.DataTuple('pkt_len', generated_pkt_len),
                     gc.DataTuple('pkt_buffer_offset', buf_offset),
                     # Program the number of packets to generate.  Note these
                     # values are zero based, so a value of zero makes one
                     # packet and a value of ten makes eleven packets.
                     gc.DataTuple('batch_count_cfg', batch_cnt-1),
                     gc.DataTuple('packets_per_batch_cfg', pkt_per_batch-1),
                     # Program the Inter-Packet-Gap (IPG) and Inter-Batch-Gap
                     # (IBG) in nanoseconds.  Note that on Tofino-1 this is
                     # the time from the start of one packet/batch to the start
                     # of the next.  On Tofino-2 this is the time from the end
                     # of a packet/batch to the start of the next.  We will use
                     # values of zero here since we do not need any additional
                     # delay.
                     gc.DataTuple('ipg', 0),
                     gc.DataTuple('ibg', 0),
                     # Reset the per-app counters to zero since we are setting
                     # up a new application.
                     gc.DataTuple('trigger_counter', 0),
                     gc.DataTuple('batch_counter', 0),
                     gc.DataTuple('pkt_counter', 0),
                     # Enable the app, as soon as the table entry is modified
                     # the timer will begin counting.
                     gc.DataTuple('app_enable', bool_val=True)]
        # Tofino-2 has an additional required field.  Tofino-1 would generally
        # only have port 68 in a pipe enabled for packet generation and that
        # port will be used.  Tofino-2 however may have multiple ports enabled
        # for packet generation (within the 0-7 range on the pipe) and each
        # app must be explicitly configured to use one specfic port.
        if not g_is_tofino:
            data_flds.append(gc.DataTuple('assigned_chnl_id', pgen_port))
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('app_id', app_id)])],
                    [t.make_data(data_flds, 'trigger_timer_periodic')])

        # The app is now running.  Since it has a 1ms timer packets will be
        # generated at a relatively high rate.  Since the model runs slower
        # than the hardware, it cannot keep up with a 1ms timer and we'll wait
        # longer to give it a chance to process packets.
        if g_is_model:
            time.sleep(3)
        else:
            time.sleep(0.1)

        # Disable the app.
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('app_id', app_id)])],
                    [t.make_data([gc.DataTuple('app_enable', bool_val=False)])])
        logger.info("Disabled app")

        # If we are running on the model it is possible that we have several
        # packets which have been generated but not yet made it through the
        # pipeline.  Usually a verify_packet call can deal with that since it
        # has a timeout.  In this case we are not receiving the packets, they
        # are dropped in the pipeline, so give the model a little time to
        # process the packets before reading the drop counters.
        if g_is_model:
            time.sleep(2*self.num_pipes)

        # The app may have been disabled in the middle of a trigger meaning it
        # may not have completed all batches and/or all packets inside a batch
        # when it was disabled.  We'll check the per-app stats and compare to
        # the verify table on a pipe-by-pipe basis to handle this.
        app_key = t.make_key([gc.KeyTuple('app_id', app_id)])
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            resp = t.entry_get(trgt, [app_key])
            data_dict = next(resp)[0].to_dict()
            triggers = data_dict['trigger_counter']
            batches = data_dict['batch_counter']
            packets = data_dict['pkt_counter']
            logger.info("App counts, pipe %d:", pipe)
            logger.info("  Triggers: %d", triggers)
            logger.info("  Batches : %d (%d batches per trigger)", batches, batch_cnt)
            logger.info("  Packets : %d (%d packets per batch)", packets, pkt_per_batch)

            # We should have triggered more than once
            self.assertGreater(triggers, 1)
            # We should have between trigger count minus one times batch-count
            # and trigger count times batch-count batches.
            min_batches = (triggers-1) * batch_cnt
            max_batches = triggers * batch_cnt
            if batches < min_batches or batches > max_batches:
                logger.error("Pipe %d: expected between %d and %d batches, got %d", pipe, min_batches, max_batches, batches)
                self.assertTrue(False)
            # Similar case for the packet counter, we may have one partial batch
            # due to the app-disable.
            min_pkts = (batches-1) * pkt_per_batch
            max_pkts = batches * pkt_per_batch
            if packets < min_pkts or packets > max_pkts:
                logger.error("Pipe %d: expected between %d and %d packets, got %d", pipe, min_pkts, max_pkts, packets)
                self.assertTrue(False)
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            resp = t.entry_get(trgt, [app_key])
            data_dict = next(resp)[0].to_dict()
            triggers = data_dict['trigger_counter']
            batches = data_dict['batch_counter']
            packets = data_dict['pkt_counter']
            exp_min = triggers - 1
            exp_max = triggers
            exp_size = generated_pkt_len + 6 + 4
            pkts_in_final_batch = packets - (batches-1)*pkt_per_batch
            for bid in range(batch_cnt):
                for pid in range(pkt_per_batch):
                    k_flds = [gc.KeyTuple('hdr.timer.pipe_id', pipe), gc.KeyTuple('hdr.timer.app_id', app_id), gc.KeyTuple('hdr.timer.batch_id', bid), gc.KeyTuple('hdr.timer.packet_id', pid)]
                    k = verify_tbl.make_key(k_flds)
                    resp = verify_tbl.entry_get(trgt, [k], {'from_hw': True})
                    d,_ = next(resp)
                    dd = d.to_dict()
                    if (dd['$COUNTER_SPEC_PKTS'] != exp_min and dd['$COUNTER_SPEC_PKTS'] != exp_max) or \
                       (dd['$COUNTER_SPEC_BYTES'] != exp_min * exp_size and dd['$COUNTER_SPEC_BYTES'] != exp_max * exp_size):
                        logger.info("Pipe %d: Verify entry had unexpected counts: Batch-Id %d Packet-Id %d, got %d %d, expected %d %d or %d %d", pipe, bid, pid, dd['$COUNTER_SPEC_PKTS'], dd['$COUNTER_SPEC_BYTES'], exp_min, exp_min * exp_size, exp_max, exp_max * exp_size)
                        self.assertTrue(False)



class RecircApp(BfRuntimeTest):
    """
    Example usage of the Recirculation Packet Generator Application.
    This app type will trigger the Packet Generator each time a matching packet
    is recirculated. A matching packet is a packet where the first B bytes has
    a successful ternary match against a configured key and mask in the app.
    Once the app is triggered it will generate N batches of M packets.
    On Tofino-1 B is four bytes, on Tofino-2 it is 16 bytes.
    On Tofino-1 N should be one while on Tofino-2 it can be greater than one.
    On Tofino-1 the three LSBs of the four byte pattern (first four bytes of
    recirculated trigger packet) are placed into the six byte Packet Gen header.
    On Tofino-2 the first 16 bytes of recirculated packet is placed after the
    generated packet's 6 byte packet gen header.
    """
    def setUp(self):
        misc_utils.setup_random()
        BfRuntimeTest.setUp(self, client_id, p4_name)
        self.bfrt_info = self.interface.bfrt_info_get(p4_name)
        self.pipes = get_pipes(self.bfrt_info)
        self.num_pipes = len(self.pipes)

    def tearDown(self):
        # Clear the Parser Value Set entries.
        clean_pvs(self.bfrt_info, self.pipes)
        clean_recirc_pvs(self.bfrt_info)
        # Clean up entries in the recirc tables.
        t = self.bfrt_info.table_get('t_recirc')
        t.entry_del(gc.Target(device_id=dev_id), [])
        t = self.bfrt_info.table_get('t_handle_external_packet')
        t.entry_del(gc.Target(device_id=dev_id), [])
        # Clean up the entries in the verify table.
        verify_tbl = self.bfrt_info.table_get('t_recirc_app')
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            verify_tbl.entry_del(trgt, [])
        # Ensure all apps are disabled
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        for app_id in range(t.info.size_get()):
            t.entry_mod(gc.Target(device_id=dev_id),
                                  [t.make_key([gc.KeyTuple('app_id', app_id)])],
                                  [t.make_data([gc.DataTuple('app_enable', bool_val=False)])])
        # Clear config in the app table
        t.entry_del(gc.Target(device_id=dev_id), [])

        # We are leaving the data in the Packet Generator Buffer table, there is
        # no harm in leaving that config in place since the app table has been
        # cleaned up.

        BfRuntimeTest.tearDown(self)

    def runTest(self):
        app_tbl = self.bfrt_info.table_get('pktgen.app_cfg')
        pkt_buf_tbl = self.bfrt_info.table_get('pktgen.pkt_buffer')
        verify_tbl = self.bfrt_info.table_get('t_recirc_app')
        tbl_set_asymmetric(verify_tbl)

        app_id = random.choice(range(app_tbl.info.size_get()))
        generated_pkt_len = 100
        generated_pkt = testutils.simple_ip_packet(pktlen=generated_pkt_len)
        batch_cnt = 1 if g_is_tofino else 2
        pkt_per_batch = 2
        eg_port_recirc = swports[0]
        eg_port_generated = swports[1]

        dmac = '00:00:00:00:00:01'
        recirc_pkt = testutils.simple_tcpv6_packet(eth_dst=dmac)

        # Our parser program uses Value Sets to identify packets from the Packet
        # Generator (see P4 for details).  Program the VSs now to identify the
        # packets our timer app will create.
        logger.info("Configuring value sets")
        configure_vs(self.bfrt_info, self.pipes, recirc_app_id=app_id)

        # Configure the forwarding table to recirculate a packet.
        # First choose a recirculation port to use, then program the Value Set
        # to identify recirculated traffic using a fixed tag of 0xFF.
        # Then add entries to the first and second pass tables so the packet is
        # sent to the selected recirc port and then out a front panel port.
        logger.info("Configuring forwarding table for recirc")
        recirc_ports = list()
        if g_is_tofino:
            for pipe in self.pipes:
                # For Tofino-1 we are limiting ourselves to the last port group
                # for recirculation.  This is because on the last port group
                # supports Packet Generation so only packets recirculated on
                # this port group can trigger the recirc Packet Generator app.
                for port in range(68, 72, 4):
                    recirc_ports.append(misc_utils.make_port(pipe, port))
        else:
            for pipe in self.pipes:
                for port in range(0, 8, 2):
                    if pipe == 0 and port == 0:
                        # PCIe CPU port, not recirc, but add port 1 since the
                        # PCIe CPU port only consumes a single channel
                        recirc_ports.append(misc_utils.make_port(pipe, 1))
                        continue
                    if pipe == 0 and port >= 2 and port <= 5:
                        # Eth CPU port, not recirc
                        continue
                    recirc_ports.append(misc_utils.make_port(pipe, port))
        recirc_port = random.choice(recirc_ports)
        logger.info("Possible recirc ports: %s", recirc_ports)
        logger.info("Using recirc port %d", recirc_port)
        tag_width = 32 if g_is_tofino else 128
        recirc_tag = random.getrandbits(tag_width)
        # We don't want the recirc-tag to conflict with the Value Set checks
        # for the generated packets so we set a fixed value in the first byte.
        recirc_tag |= 0xFF << (tag_width-8)
        logger.info("Using recirc tag value: 0x%x", recirc_tag)
        t = self.bfrt_info.table_get('t_handle_external_packet')
        t.entry_add(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('hdr.ethernet.dmac', 1)])],
                    [t.make_data([gc.DataTuple('dst_port', recirc_port),
                                  gc.DataTuple('tag', recirc_tag)],
                                 'Ing.fwd_recirc')])
        t = self.bfrt_info.table_get('t_recirc')
        t.entry_add(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('hdr.recirc_tag.tag', recirc_tag)])],
                    [t.make_data([gc.DataTuple('dst_port', eg_port_recirc)],
                                 'Ing.count_recirc_and_fwd')])
        configure_recirc_vs(self.bfrt_info, recirc_port, 0xFF)

        # Send a packet to ensure basic recirculation works
        logger.info("Sending one packet for basic recirculation")
        testutils.send_packet(self, random.choice(swports), recirc_pkt)
        testutils.verify_packet(self, recirc_pkt, eg_port_recirc)

        # Next program the verification table to match and count each of the
        # generated packets.  The generated packets will also be forwarded out
        # a port so we can receive them.
        logger.info("Configuring verify table")
        verify_keys_by_pipe = dict()
        if g_is_tofino:
            for pipe in self.pipes:
                keys = [verify_tbl.make_key([gc.KeyTuple('hdr.recirc.pipe_id', pipe),
                                             gc.KeyTuple('hdr.recirc.app_id', app_id),
                                             gc.KeyTuple('hdr.recirc.packet_id', p),
                                             gc.KeyTuple('hdr.recirc.key', recirc_tag & 0x00FFFFFF)]) for p in range(pkt_per_batch)]
                verify_keys_by_pipe[pipe] = keys
        else:
            ids = [(b,p) for b in range(batch_cnt) for p in range(pkt_per_batch)]
            for pipe in self.pipes:
                keys = [verify_tbl.make_key([gc.KeyTuple('hdr.recirc.pipe_id', pipe),
                                             gc.KeyTuple('hdr.recirc.app_id', app_id),
                                             gc.KeyTuple('hdr.recirc.batch_id', b),
                                             gc.KeyTuple('hdr.recirc.packet_id', p),
                                             gc.KeyTuple('hdr.recirc_context.f', recirc_tag)]) for b,p in ids]
                verify_keys_by_pipe[pipe] = keys

        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            d = verify_tbl.make_data([gc.DataTuple('dst_port', eg_port_generated)],
                                     'Ing.count_recirc_app_and_fwd')
            verify_tbl.entry_add(trgt,
                                 verify_keys_by_pipe[pipe],
                                 [d]*len(verify_keys_by_pipe[pipe]))

        # The following steps are required and can be done in any order, except
        # for enabling the Packet Generator application which should be done
        # last.
        #   - Enable packet generation on the generator port
        #   - Enabling snooping on the recirc port
        #   - Program the packet to generate into the packet buffer
        #   - Configure the Packet Generator application

        # Enable packet generation on the generator port.  For Tofino-1 this
        # should be port 68 in each pipe (68, 196, 324, 452).  For Tofino-2
        # this can be any of the ports in the 0-7 range on each pipe.
        # Since this is a recirculation app, on Tofino-2 the port used for
        # generation must be the same port that we recirculate the trigger
        # packets on.
        pgen_port = recirc_port
        logger.info("Enabling Packet Generation on port %d", pgen_port)
        t = self.bfrt_info.table_get('pktgen.port_cfg')
        keys = [t.make_key([gc.KeyTuple('dev_port', pgen_port)])]
        data = [t.make_data([gc.DataTuple('pktgen_enable', bool_val=True)])]
        t.entry_mod(gc.Target(device_id=dev_id), keys, data)

        logger.info("Enabling pattern-matching on port %d (pipe %d local port %d)", recirc_port, misc_utils.port_to_pipe(recirc_port), misc_utils.port_to_pipe_local_port(recirc_port))
        keys = [t.make_key([gc.KeyTuple('dev_port', recirc_port)])]
        data = [t.make_data([gc.DataTuple('pattern_matching_enable', bool_val=True)])]
        t.entry_mod(gc.Target(device_id=dev_id), keys, data)

        # Program the packet into the packet buffer.  The packet buffer is 16kB
        # divided into 1024 "lines", each holding 16B.  The packet should be
        # aligned with a 16B alignment in the buffer.  For this example we will
        # pick a random location within the packet buffer.
        num_lines = (generated_pkt_len + 15) // 16
        first_line = 0
        last_line = 1024 - num_lines
        line = random.randint(first_line, last_line)
        buf_offset = line * 16
        logger.info("Configuring packet buffer at offset: %d", buf_offset)
        t = self.bfrt_info.table_get('pktgen.pkt_buffer')
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('pkt_buffer_offset', buf_offset),
                                 gc.KeyTuple('pkt_buffer_size', generated_pkt_len)])],
                    [t.make_data([gc.DataTuple('buffer', bytearray(bytes(generated_pkt)))])])

        # Configure the Packet Generator app.
        logger.info("Configuring app number %d", app_id)
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        data_flds = [gc.DataTuple('pattern_value', recirc_tag),
                     gc.DataTuple('pattern_mask', (1<<tag_width)-1),
                     # Configure the app to generate the packet we programmed
                     # into the packet buffer
                     gc.DataTuple('pkt_len', generated_pkt_len),
                     gc.DataTuple('pkt_buffer_offset', buf_offset),
                     # Program the number of packets to generate.  Note these
                     # values are zero based, so a value of zero makes one
                     # packet and a value of ten makes eleven packets.
                     gc.DataTuple('batch_count_cfg', batch_cnt-1),
                     gc.DataTuple('packets_per_batch_cfg', pkt_per_batch-1),
                     # Program the Inter-Packet-Gap (IPG) and Inter-Batch-Gap
                     # (IBG) in nanoseconds.  Note that on Tofino-1 this is
                     # the time from the start of one packet/batch to the start
                     # of the next.  On Tofino-2 this is the time from the end
                     # of a packet/batch to the start of the next.  We will use
                     # values of zero here since we do not need any additional
                     # delay.
                     gc.DataTuple('ipg', 0),
                     gc.DataTuple('ibg', 0),
                     # Reset the per-app counters to zero since we are setting
                     # up a new application.
                     gc.DataTuple('trigger_counter', 0),
                     gc.DataTuple('batch_counter', 0),
                     gc.DataTuple('pkt_counter', 0),
                     gc.DataTuple('app_enable', bool_val=True)]
        # Tofino-2 has an additional required field.  Tofino-1 would generally
        # only have port 68 in a pipe enabled for packet generation and that
        # port will be used.  Tofino-2 however may have multiple ports enabled
        # for packet generation (within the 0-7 range on the pipe) and each
        # app must be explicitly configured to use one specfic port.
        if not g_is_tofino:
            data_flds.append(gc.DataTuple('assigned_chnl_id', misc_utils.port_to_pipe_local_port(pgen_port)))
        d = t.make_data(data_flds, 'trigger_recirc_pattern')
        logger.info("  Pattern Value: 0x%x", d.to_dict()['pattern_value'])
        logger.info("  Pattern Mask : 0x%x", d.to_dict()['pattern_mask'])
        logger.info("  Batch Count  : %d (%d batches)", d.to_dict()['batch_count_cfg'], d.to_dict()['batch_count_cfg']+1)
        logger.info("  Packet Count : %d (%d packets)", d.to_dict()['packets_per_batch_cfg'], d.to_dict()['packets_per_batch_cfg']+1)
        if 'assigned_chnl_id' in d.to_dict():
            logger.info("  Assigned Chnl: %d", d.to_dict()['assigned_chnl_id'])
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('app_id', app_id)])],
                    [d])

        # Send one packet, which will be recirculated, to trigger the app.
        # The recirculated packet will have an internal header added by the
        # ingress pipeline which will match the pattern configured in the app
        # and cause the app to trigger.  Note that only a single pipe will
        # trigger since the packet is recirculated to a single port.
        testutils.send_packet(self, random.choice(swports), recirc_pkt)
        # If running on the model it may take extra time for the packets to come
        # back.  This packet will make two passes due to the recirculation and
        # also trigger the packet generator which will inject more traffic.
        # Since the model may process packets a bit slowly, give it some extra
        # time.
        if g_is_model:
            time.sleep(batch_cnt * pkt_per_batch)
        testutils.verify_packet(self, recirc_pkt, eg_port_recirc)

        # Read the app config table to get the per-app counters to check if
        # packet generation is complete.  Again, we expect a single trigger
        # since the recirculated packet is sent to a single pipe/recirc-port.
        # On hardware this should complete immediately but on the model it
        # may take a little time depending on the number of packets.
        num_polls = 0
        exp_triggers = 1
        exp_batches = exp_triggers * batch_cnt
        exp_packets = exp_batches * pkt_per_batch
        while True:
            resp = t.entry_get(gc.Target(device_id=dev_id),
                               [t.make_key([gc.KeyTuple('app_id', app_id)])])
            data_dict = next(resp)[0].to_dict()
            num_polls += 1
            triggers = data_dict['trigger_counter']
            batches = data_dict['batch_counter']
            packets = data_dict['pkt_counter']

            if triggers == exp_triggers and batches == exp_batches and packets == exp_packets:
                # All packets are received, we can stop polling
                break
            if triggers > exp_triggers or batches > exp_batches or packets > exp_packets:
                # Oops, too many triggers/batches/packets!
                logger.error("Unexpected app counters")
                logger.error("Expected triggers %d, got %d", exp_triggers, triggers)
                logger.error("Expected batches %d, got %d", exp_batches, batches)
                logger.error("Expected packets %d, got %d", exp_packets, packets)
                self.assertTrue(False)
            # Not all packets have been generated yet.  If this is hardware
            # give it one additional polling attempt (it should have finished
            # very quickly).  If it is the model assume a worst case of one
            # packet per second processing rate in the model, wait and poll
            # once more.
            if g_is_model:
                if num_polls > exp_packets:
                    logger.error("Not all packets have been generated")
                    self.assertTrue(False)
                time.sleep(1)
            else:
                if num_polls > 1:
                    logger.error("Not all packets have been generated")
                    self.assertTrue(False)
                time.sleep(0.1)

        # All packets have been generated, receive all the copies to ensure
        # the expected packet data was created.  Since we are not re-writting
        # the packets in the pipeline and the internal Packet Generator header
        # is removed at the ingress deparser, all packets will be the same.
        for _ in range(exp_packets):
            testutils.verify_packet(self, generated_pkt, eg_port_generated)

        # Ensure the verify entries were correctly matched.
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            if pipe == misc_utils.port_to_pipe(recirc_port):
                exp_cnt = 1
                # The generated packets will have an Ethernet FCS (4 bytes)
                # added to them as well as a 6 byte internal Packet Generator
                # header.  These increase the expected size of the packet.
                # On Tofino-2 there is also 16B of context from the triggering
                # packet carried after the 6 byte internal Packet Generator
                # header
                exp_size = generated_pkt_len + 6 + 4
                if g_is_tofino2: exp_size += 16
            else:
                exp_size = 0
                exp_cnt = 0
            resp = verify_tbl.entry_get(trgt, verify_keys_by_pipe[pipe])
            resp_cnt = 0
            for d,k in resp:
                resp_cnt += 1
                dd = d.to_dict()
                if dd['$COUNTER_SPEC_PKTS'] != exp_cnt or dd['$COUNTER_SPEC_BYTES'] != exp_size:
                    logger.error("Pipe %d: Verify entry had unexpected counts:\n%s\n%s", pipe, k.to_dict(), dd)
                    self.assertEqual(dd['$COUNTER_SPEC_PKTS'], exp_cnt)
                    self.assertEqual(dd['$COUNTER_SPEC_BYTES'], exp_size)
            self.assertEqual(resp_cnt, batch_cnt * pkt_per_batch)



class DprsrApp(BfRuntimeTest):
    """
    Example usage of the Deparser Trigger Packet Generator Application.
    This app type will trigger the Packet Generator by allowing an ingress
    packet to pass 16B of data to the Packet Generator.  The Packet Generator
    will ternary match that 16B of data against a pattern and mask configured
    in the Deparser Trigger app.  If it matches the app will trigger and
    generate packets.  The batch count and size is specified in the app as usual
    but the packet buffer offset and length come from metadata setup by the
    triggering packet.
    The generated packets will carry the 16B of data from the triggering packet
    immediately after the six byte Packet Generator header.
    """
    def setUp(self):
        # Tofino-1 does not support Deparser Packet Generator apps
        if g_is_tofino: return

        misc_utils.setup_random()
        BfRuntimeTest.setUp(self, client_id, p4_name)
        self.bfrt_info = self.interface.bfrt_info_get(p4_name)
        self.pipes = get_pipes(self.bfrt_info)
        self.num_pipes = len(self.pipes)

    def tearDown(self):
        # Tofino-1 does not support Deparser Packet Generator apps
        if g_is_tofino: return

        # Clear the Parser Value Set entries.
        clean_pvs(self.bfrt_info, self.pipes)
        # Clean up entries in the foward table.
        t = self.bfrt_info.table_get('t_handle_external_packet')
        t.entry_del(gc.Target(device_id=dev_id), [])
        # Clean up the entries in the verify table.
        verify_tbl = self.bfrt_info.table_get('t_dprsr_app')
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            verify_tbl.entry_del(trgt, [])
        # Ensure all apps are disabled
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        for app_id in range(t.info.size_get()):
            t.entry_mod(gc.Target(device_id=dev_id),
                                  [t.make_key([gc.KeyTuple('app_id', app_id)])],
                                  [t.make_data([gc.DataTuple('app_enable', bool_val=False)])])
        # Clear config in the app table
        t.entry_del(gc.Target(device_id=dev_id), [])

        # We are leaving the data in the Packet Generator Buffer table, there is
        # no harm in leaving that config in place since the app table has been
        # cleaned up.

        BfRuntimeTest.tearDown(self)

    def runTest(self):
        # Tofino-1 does not support Deparser Packet Generator apps
        if g_is_tofino: return

        app_tbl = self.bfrt_info.table_get('pktgen.app_cfg')
        pkt_buf_tbl = self.bfrt_info.table_get('pktgen.pkt_buffer')
        verify_tbl = self.bfrt_info.table_get('t_dprsr_app')
        tbl_set_asymmetric(verify_tbl)

        app_id = random.choice(range(app_tbl.info.size_get()))
        generated_pkt_len = 100
        generated_pkt = testutils.simple_ip_packet(pktlen=generated_pkt_len)
        batch_cnt = 3
        pkt_per_batch = 2
        eg_port = swports[0]
        eg_port_generated = swports[-1]

        dmac = '00:00:00:00:00:01'
        trig_pkt = testutils.simple_tcpv6_packet(eth_dst=dmac)

        # Our parser program uses Value Sets to identify packets from the Packet
        # Generator (see P4 for details).  Program the VSs now to identify the
        # packets our timer app will create.
        logger.info("Configuring value sets")
        configure_vs(self.bfrt_info, self.pipes, dprsr_app_id=app_id)

        # Next program the verification table to match and count each of the
        # generated packets.  The generated packets will also be forwarded out
        # a port so we can receive them.
        logger.info("Configuring verify table")
        dprsr_ctx = random.getrandbits(128)
        verify_keys_by_pipe = dict()
        ids = [(b,p) for b in range(batch_cnt) for p in range(pkt_per_batch)]
        for pipe in self.pipes:
            keys = [verify_tbl.make_key([gc.KeyTuple('hdr.dprsr.pipe_id', pipe),
                                         gc.KeyTuple('hdr.dprsr.app_id', app_id),
                                         gc.KeyTuple('hdr.dprsr.batch_id', b),
                                         gc.KeyTuple('hdr.dprsr.packet_id', p),
                                         gc.KeyTuple('hdr.dprsr_context.f', dprsr_ctx)]) for b,p in ids]
            verify_keys_by_pipe[pipe] = keys

        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            d = verify_tbl.make_data([gc.DataTuple('dst_port', eg_port_generated)],
                                     'Ing.count_dprsr_app_and_fwd')
            verify_tbl.entry_add(trgt,
                                 verify_keys_by_pipe[pipe],
                                 [d]*len(verify_keys_by_pipe[pipe]))

        # The following steps are required and can be done in any order, except
        # for enabling the Packet Generator application which should be done
        # last.
        #   - Enable packet generation on the generator port
        #   - Program the packet to generate into the packet buffer
        #   - Configure the Packet Generator application

        # Enable packet generation on the generator port.
        # For Tofino-2 this can be any of the ports in the 0-7 range on each
        # pipe which are NOT used for CPU ports.  By default port 6 in each pipe
        # is already configured as a recirc port (i.e. not a CPU port) so we'll
        # use it
        pgen_port = 6
        pgen_ports = [misc_utils.make_port(pipe, pgen_port) for pipe in self.pipes]
        logger.info("Configuring packet gen ports: %s", pgen_ports)
        t = self.bfrt_info.table_get('pktgen.port_cfg')
        keys = [t.make_key([gc.KeyTuple('dev_port', port)]) for port in pgen_ports]
        data = [t.make_data([gc.DataTuple('pktgen_enable', bool_val=True)]) for _ in self.pipes]
        t.entry_mod(gc.Target(device_id=dev_id), keys, data)

        # Program the packet into the packet buffer.  The packet buffer is 16kB
        # divided into 1024 "lines", each holding 16B.  The packet should be
        # aligned with a 16B alignment in the buffer.  For this example we will
        # pick a random location within the packet buffer.
        num_lines = (generated_pkt_len + 15) // 16
        first_line = 0
        last_line = 1024 - num_lines
        line = random.randint(first_line, last_line)
        buf_offset = line * 16
        logger.info("Configuring packet buffer at offset: %d", buf_offset)
        t = self.bfrt_info.table_get('pktgen.pkt_buffer')
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('pkt_buffer_offset', buf_offset),
                                 gc.KeyTuple('pkt_buffer_size', generated_pkt_len)])],
                    [t.make_data([gc.DataTuple('buffer', bytearray(bytes(generated_pkt)))])])

        # Configure the forwarding table to both forward a packet out of the
        # switch and trigger the deparser app.
        # Notice that the value programmed for the location of the packet in the
        # buffer is a 10-bit value.  The offset always needs to be 16B aligned
        # and when setting up the intrinsic metadata for the deparser that
        # metadata only carries the upper 10 bits of the 14-bit offset.
        logger.info("Configuring forwarding table")
        t = self.bfrt_info.table_get('t_handle_external_packet')
        t.entry_add(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('hdr.ethernet.dmac', 1)])],
                    [t.make_data([gc.DataTuple('dst_port', eg_port),
                                  gc.DataTuple('ctx', dprsr_ctx),
                                  gc.DataTuple('pkt_buf_start', line),
                                  gc.DataTuple('pkt_len', generated_pkt_len)],
                                 'Ing.dprsr_trig_and_fwd')])

        # Configure the Packet Generator app.
        logger.info("Configuring app number %d", app_id)
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        data_flds = [# Program the pattern to match and the mask
                     gc.DataTuple('pattern_value', dprsr_ctx),
                     gc.DataTuple('pattern_mask', (1<<128)-1),
                     # Even though the packet offset and length come from the
                     # metadata when the deparser app is triggered, we need
                     # to program a legal value here.
                     gc.DataTuple('pkt_len', generated_pkt_len),
                     gc.DataTuple('pkt_buffer_offset', buf_offset),
                     # Program the number of packets to generate.  Note these
                     # values are zero based, so a value of zero makes one
                     # packet and a value of ten makes eleven packets.
                     gc.DataTuple('batch_count_cfg', batch_cnt-1),
                     gc.DataTuple('packets_per_batch_cfg', pkt_per_batch-1),
                     # Program the Inter-Packet-Gap (IPG) and Inter-Batch-Gap
                     # (IBG) in nanoseconds.  Note that on Tofino-1 this is
                     # the time from the start of one packet/batch to the start
                     # of the next.  On Tofino-2 this is the time from the end
                     # of a packet/batch to the start of the next.  We will use
                     # values of zero here since we do not need any additional
                     # delay.
                     gc.DataTuple('ipg', 0),
                     gc.DataTuple('ibg', 0),
                     # Reset the per-app counters to zero since we are setting
                     # up a new application.
                     gc.DataTuple('trigger_counter', 0),
                     gc.DataTuple('batch_counter', 0),
                     gc.DataTuple('pkt_counter', 0),
                     gc.DataTuple('app_enable', bool_val=True),
                     gc.DataTuple('assigned_chnl_id', pgen_port)]
        d = t.make_data(data_flds, 'trigger_dprsr')
        logger.info("  Pattern Value: 0x%x", d.to_dict()['pattern_value'])
        logger.info("  Pattern Mask : 0x%x", d.to_dict()['pattern_mask'])
        logger.info("  Batch Count  : %d (%d batches)", d.to_dict()['batch_count_cfg'], d.to_dict()['batch_count_cfg']+1)
        logger.info("  Packet Count : %d (%d packets)", d.to_dict()['packets_per_batch_cfg'], d.to_dict()['packets_per_batch_cfg']+1)
        logger.info("  Assigned Chnl: %d", d.to_dict()['assigned_chnl_id'])
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('app_id', app_id)])],
                    [d])

        # Send one packet which will forward through the switch and come back on
        # "eg_port".  It will also trigger the deparser app in the Packet
        # Generator of the ingress port's pipe.
        ig_port = random.choice(swports)
        ig_pipe = misc_utils.port_to_pipe(ig_port)
        testutils.send_packet(self, ig_port, trig_pkt)
        # If running on the model it may take extra time for the packets to come
        # back as the packet generator will begin multiple packets due to the
        # deparser trigger.  Give the model a little extra time to process all
        # these packets in case it is running a bit slowly.
        if g_is_model:
            time.sleep(batch_cnt * pkt_per_batch)
        testutils.verify_packet(self, trig_pkt, eg_port)

        # Read the app config table to get the per-app counters to check if
        # packet generation is complete.  Again, we expect a single trigger
        # since the trigger packet is sent to a single pipe/deparser.
        # On hardware this should complete immediately but on the model it
        # may take a little time depending on the number of packets.
        num_polls = 0
        exp_triggers = 1
        exp_batches = exp_triggers * batch_cnt
        exp_packets = exp_batches * pkt_per_batch
        while True:
            resp = t.entry_get(gc.Target(device_id=dev_id),
                               [t.make_key([gc.KeyTuple('app_id', app_id)])])
            data_dict = next(resp)[0].to_dict()
            num_polls += 1
            triggers = data_dict['trigger_counter']
            batches = data_dict['batch_counter']
            packets = data_dict['pkt_counter']

            if triggers == exp_triggers and batches == exp_batches and packets == exp_packets:
                # All packets are received, we can stop polling
                break
            if triggers > exp_triggers or batches > exp_batches or packets > exp_packets:
                # Oops, too many triggers/batches/packets!
                logger.error("Unexpected app counters")
                logger.error("Expected triggers %d, got %d", exp_triggers, triggers)
                logger.error("Expected batches %d, got %d", exp_batches, batches)
                logger.error("Expected packets %d, got %d", exp_packets, packets)
                self.assertTrue(False)
            # Not all packets have been generated yet.  If this is hardware
            # give it one additional polling attempt (it should have finished
            # very quickly).  If it is the model assume a worst case of one
            # packet per second processing rate in the model, wait and poll
            # once more.
            if g_is_model:
                if num_polls > exp_packets:
                    logger.error("Not all packets have been generated")
                    self.assertTrue(False)
                time.sleep(1)
            else:
                if num_polls > 1:
                    logger.error("Not all packets have been generated")
                    self.assertTrue(False)
                time.sleep(0.1)

        # All packets have been generated, receive all the copies to ensure
        # the expected packet data was created.  Since we are not re-writting
        # the packets in the pipeline and the internal Packet Generator header
        # is removed at the ingress deparser, all packets will be the same.
        for _ in range(exp_packets):
            testutils.verify_packet(self, generated_pkt, eg_port_generated)

        # Ensure the verify entries were correctly matched.
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            if pipe == ig_pipe:
                exp_cnt = 1
                # The generated packets will have an Ethernet FCS (4 bytes)
                # added to them as well as a 6 byte internal Packet Generator
                # header and the 16 bytes of context from the triggering packet.
                exp_size = generated_pkt_len + 6 + 4 + 16
            else:
                exp_size = 0
                exp_cnt = 0
            resp = verify_tbl.entry_get(trgt, verify_keys_by_pipe[pipe])
            resp_cnt = 0
            for d,k in resp:
                resp_cnt += 1
                dd = d.to_dict()
                if dd['$COUNTER_SPEC_PKTS'] != exp_cnt or dd['$COUNTER_SPEC_BYTES'] != exp_size:
                    logger.error("Pipe %d: Verify entry had unexpected counts:\n%s\n%s", pipe, k.to_dict(), dd)
                    self.assertEqual(dd['$COUNTER_SPEC_PKTS'], exp_cnt)
                    self.assertEqual(dd['$COUNTER_SPEC_BYTES'], exp_size)
            self.assertEqual(resp_cnt, batch_cnt * pkt_per_batch)


def set_port_state(bfrt_info, port, up_or_down):
    if g_is_model:
        if up_or_down:
            ptf_port.bring_port_up(port)
        else:
            ptf_port.take_port_down(port)
    else:
        t = bfrt_info.table_get('$PORT')
        k = t.make_key([gc.KeyTuple('$DEV_PORT', port)])
        d = t.make_data([gc.DataTuple('$PORT_ENABLE', bool_val=up_or_down)])
        t.entry_mod(gc.Target(device_id=dev_id), [k], [d])
        poll_cnt = 0
        while up_or_down:
            # Wait for the port to come up
            resp = t.entry_get(gc.Target(device_id=dev_id), [k])
            d,_ = next(resp)
            dd = d.to_dict()
            poll_cnt += 1
            if dd['$PORT_UP'] == True:
                break
            if poll_cnt >= 200:
                logger.error("Port %d not coming up...", port)
                break
            time.sleep(0.25)

def make_port_down(bfrt_info, port):
    set_port_state(bfrt_info, port, False)
def make_port_up(bfrt_info, port):
    set_port_state(bfrt_info, port, True)
def flap_port(bfrt_info, port):
    logger.info("Flapping port %d", port)
    set_port_state(bfrt_info, port, False)
    # Add a small wait if running on the model.  The model uses a thread to
    # monitor interface state via polling, if we toggle up-down-up faster than
    # the polling interval the transition may be missed.
    if g_is_model:
        time.sleep(3)
    set_port_state(bfrt_info, port, True)

@testutils.group("thorough")
class PortDownAppTF1(BfRuntimeTest):
    """
    Example usage of the Port Down Packet Generator Application on Tofino-1.
    This app will trigger when a port goes down.  The generated packets will
    carry the port number of the port that went down.
    The example shows how to configure a Port Down application and how to update
    the "port down" state in the Packet Generator to allow the app to trigger
    for a specific port.
    """
    def setUp(self):
        # This example is for Tofino-1 only.
        if not g_is_tofino: return

        misc_utils.setup_random()
        BfRuntimeTest.setUp(self, client_id, p4_name)
        self.bfrt_info = self.interface.bfrt_info_get(p4_name)
        self.pipes = get_pipes(self.bfrt_info)
        self.num_pipes = len(self.pipes)

    def tearDown(self):
        # This example is for Tofino-1 only.
        if not g_is_tofino: return

        # Clear the Parser Value Set entries.
        clean_pvs(self.bfrt_info, self.pipes)
        # Clean up the entries in the verify table.
        verify_tbl = self.bfrt_info.table_get('t_port_down_app')
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            verify_tbl.entry_del(trgt, [])
        # Ensure all apps are disabled
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        for app_id in range(t.info.size_get()):
            t.entry_mod(gc.Target(device_id=dev_id),
                                  [t.make_key([gc.KeyTuple('app_id', app_id)])],
                                  [t.make_data([gc.DataTuple('app_enable', bool_val=False)])])
        # Clear config in the app table
        t.entry_del(gc.Target(device_id=dev_id), [])

        # We are leaving the data in the Packet Generator Buffer table, there is
        # no harm in leaving that config in place since the app table has been
        # cleaned up.

        BfRuntimeTest.tearDown(self)

    def runTest(self):
        # This example is for Tofino-1 only.
        if not g_is_tofino: return

        app_tbl = self.bfrt_info.table_get('pktgen.app_cfg')
        pkt_buf_tbl = self.bfrt_info.table_get('pktgen.pkt_buffer')
        verify_tbl = self.bfrt_info.table_get('t_port_down_app')
        tbl_set_asymmetric(verify_tbl)

        app_id = random.choice(range(app_tbl.info.size_get()))
        generated_pkt_len = 100
        generated_pkt = testutils.simple_ip_packet(pktlen=generated_pkt_len)
        # Port down apps must use a single batch.  The internal Packet Generator
        # header carries the port-id instead of the batch id.
        batch_cnt = 1
        pkt_per_batch = 5
        eg_port = swports[0]

        # Our parser program uses Value Sets to identify packets from the Packet
        # Generator (see P4 for details).  Program the VSs now to identify the
        # packets our timer app will create.
        logger.info("Configuring value sets")
        configure_vs(self.bfrt_info, self.pipes, port_down_app_ids=[app_id])

        # Next program the verification table to match and count each of the
        # generated packets.  The generated packets will also be forwarded out
        # a port so we can receive them.
        logger.info("Configuring verify table")
        verify_keys_by_port = dict()
        ids = [pid for pid in range(pkt_per_batch)]
        for port in swports:
            keys = [verify_tbl.make_key([gc.KeyTuple('hdr.port_down.pipe_id', misc_utils.port_to_pipe(port)),
                                         gc.KeyTuple('hdr.port_down.app_id', app_id),
                                         gc.KeyTuple('hdr.port_down.port_num', port),
                                         gc.KeyTuple('hdr.port_down.packet_id', pid)]) for pid in ids]
            verify_keys_by_port[port] = keys

        for port in verify_keys_by_port:
            trgt = gc.Target(device_id=dev_id, pipe_id=misc_utils.port_to_pipe(port))
            d = verify_tbl.make_data([gc.DataTuple('dst_port', eg_port)],
                                     'Ing.count_port_down_app_and_fwd')
            verify_tbl.entry_add(trgt,
                                 verify_keys_by_port[port],
                                 [d]*len(verify_keys_by_port[port]))

        # The following steps are required and can be done in any order, except
        # for enabling the Packet Generator application which should be done
        # last.
        #   - Enable packet generation on the generator port
        #   - Program the packet to generate into the packet buffer
        #   - Configure the Packet Generator application
        #   - Clear the "port down" state for the ports we want to monitor

        # Enable packet generation on the generator port.
        pgen_port = 68
        pgen_ports = [misc_utils.make_port(pipe, pgen_port) for pipe in self.pipes]
        logger.info("Configuring packet gen ports: %s", pgen_ports)
        port_cfg = self.bfrt_info.table_get('pktgen.port_cfg')
        keys = [port_cfg.make_key([gc.KeyTuple('dev_port', port)]) for port in pgen_ports]
        data = [port_cfg.make_data([gc.DataTuple('pktgen_enable', bool_val=True)]) for _ in self.pipes]
        port_cfg.entry_mod(gc.Target(device_id=dev_id), keys, data)

        # Program the packet into the packet buffer.  The packet buffer is 16kB
        # divided into 1024 "lines", each holding 16B.  The packet should be
        # aligned with a 16B alignment in the buffer.  For this example we will
        # pick a random location within the packet buffer.
        num_lines = (generated_pkt_len + 15) // 16
        first_line = 0
        last_line = 1024 - num_lines
        line = random.randint(first_line, last_line)
        buf_offset = line * 16
        logger.info("Configuring packet buffer at offset: %d", buf_offset)
        t = self.bfrt_info.table_get('pktgen.pkt_buffer')
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('pkt_buffer_offset', buf_offset),
                                 gc.KeyTuple('pkt_buffer_size', generated_pkt_len)])],
                    [t.make_data([gc.DataTuple('buffer', bytearray(bytes(generated_pkt)))])])

        # Configure the Packet Generator app.
        logger.info("Configuring app number %d", app_id)
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        data_flds = [# Configure the app to generate the packet we programmed
                     # into the packet buffer
                     gc.DataTuple('pkt_len', generated_pkt_len),
                     gc.DataTuple('pkt_buffer_offset', buf_offset),
                     # Program the number of packets to generate.  Note these
                     # values are zero based, so a value of zero makes one
                     # packet and a value of ten makes eleven packets.
                     gc.DataTuple('batch_count_cfg', batch_cnt-1),
                     gc.DataTuple('packets_per_batch_cfg', pkt_per_batch-1),
                     # Program the Inter-Packet-Gap (IPG) and Inter-Batch-Gap
                     # (IBG) in nanoseconds.  Note that on Tofino-1 this is
                     # the time from the start of one packet/batch to the start
                     # of the next.
                     gc.DataTuple('ipg', 0),
                     gc.DataTuple('ibg', 0),
                     # Reset the per-app counters to zero since we are setting
                     # up a new application.
                     gc.DataTuple('trigger_counter', 0),
                     gc.DataTuple('batch_counter', 0),
                     gc.DataTuple('pkt_counter', 0),
                     gc.DataTuple('app_enable', bool_val=True)]
        d = t.make_data(data_flds, 'trigger_port_down')
        logger.info("  Batch Count  : %d (%d batches)", d.to_dict()['batch_count_cfg'], d.to_dict()['batch_count_cfg']+1)
        logger.info("  Packet Count : %d (%d packets)", d.to_dict()['packets_per_batch_cfg'], d.to_dict()['packets_per_batch_cfg']+1)
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('app_id', app_id)])],
                    [d])

        # The Packet Generator keeps state on whether it has already seen a port
        # go down.  It will only trigger for a port the first time that port
        # goes down; so if a port starts down, comes up, goes down, comes back
        # up, and finally goes down a second time only a single event is
        # triggered.  An API must be called to reset this state for a given port
        # in the Packet Generator.  We'll call that once here for each port
        # since all ports start out in a down state.
        keys = [port_cfg.make_key([gc.KeyTuple('dev_port', port)]) for port in swports]
        data = [port_cfg.make_data([gc.DataTuple('clear_port_down_enable', bool_val=True)]) for _ in swports]
        port_cfg.entry_mod(gc.Target(device_id=dev_id), keys, data)

        # Make each port go down and verify the expected packets were generated
        # by the Packet Generator app.
        # Don't bring down the port which forwards the generated packets
        # back to the test
        ports_to_flap = [port for port in swports]
        ports_to_flap.remove(eg_port)
        exp_triggers = 0
        for port in ports_to_flap:
            flap_port(self.bfrt_info, port)
            exp_triggers += 1
            for _ in range(batch_cnt * pkt_per_batch):
                testutils.verify_packet(self, generated_pkt, eg_port)

        # Verify the app counters have the expected values.
        resp = t.entry_get(gc.Target(device_id=dev_id),
                           [t.make_key([gc.KeyTuple('app_id', app_id)])])
        data_dict = next(resp)[0].to_dict()
        triggers = data_dict['trigger_counter']
        batches = data_dict['batch_counter']
        packets = data_dict['pkt_counter']
        self.assertEqual(triggers, exp_triggers)
        self.assertEqual(batches, exp_triggers * batch_cnt)
        self.assertEqual(packets, exp_triggers * batch_cnt * pkt_per_batch)

        # Ensure the verify entries were correctly matched.
        for port in swports:
            trgt = gc.Target(device_id=dev_id, pipe_id=misc_utils.port_to_pipe(port))
            resp = verify_tbl.entry_get(trgt, verify_keys_by_port[port])
            if port == eg_port:
                exp_cnt = 0
                exp_size = 0
            else:
                exp_cnt = 1
                # Each generated packet has a 6 byte Packet Gen header and a 4 byte
                # Ethernet FCS added to it
                exp_size = generated_pkt_len + 6 + 4
            resp_cnt = 0
            for d,k in resp:
                resp_cnt += 1
                dd = d.to_dict()
                if dd['$COUNTER_SPEC_PKTS'] != exp_cnt or dd['$COUNTER_SPEC_BYTES'] != exp_size:
                    logger.error("Port %d: Verify entry had unexpected counts:\n%s\n%s", port, k.to_dict(), dd)
                    self.assertEqual(dd['$COUNTER_SPEC_PKTS'], exp_cnt)
                    self.assertEqual(dd['$COUNTER_SPEC_BYTES'], exp_size)
            self.assertEqual(resp_cnt, batch_cnt * pkt_per_batch)

        # Flap a few ports one more time, nothing should be generated since we
        # have not cleared state in the Packet Generator yet.
        for port in ports_to_flap:
            flap_port(self.bfrt_info, port)
        testutils.verify_no_other_packets(self)
        resp = t.entry_get(gc.Target(device_id=dev_id),
                           [t.make_key([gc.KeyTuple('app_id', app_id)])])
        data_dict = next(resp)[0].to_dict()
        triggers = data_dict['trigger_counter']
        batches = data_dict['batch_counter']
        packets = data_dict['pkt_counter']
        self.assertEqual(triggers, exp_triggers)
        self.assertEqual(batches, exp_triggers * batch_cnt)
        self.assertEqual(packets, exp_triggers * batch_cnt * pkt_per_batch)

        # Pick a few ports, clear their "down" state, and flap them again.  We
        # expect the Packet Generator to trigger for these.
        to_trig_again = random.sample(ports_to_flap, min(len(ports_to_flap), 3))
        keys = [port_cfg.make_key([gc.KeyTuple('dev_port', port)]) for port in to_trig_again]
        data = [port_cfg.make_data([gc.DataTuple('clear_port_down_enable', bool_val=True)]) for _ in to_trig_again]
        port_cfg.entry_mod(gc.Target(device_id=dev_id), keys, data)
        for port in ports_to_flap:
            flap_port(self.bfrt_info, port)
            if port in to_trig_again:
                exp_triggers += 1
                for _ in range(batch_cnt * pkt_per_batch):
                    testutils.verify_packet(self, generated_pkt, eg_port)
            else:
                testutils.verify_no_other_packets(self)

        # Verify the app counters have the expected values.
        resp = t.entry_get(gc.Target(device_id=dev_id),
                           [t.make_key([gc.KeyTuple('app_id', app_id)])])
        data_dict = next(resp)[0].to_dict()
        triggers = data_dict['trigger_counter']
        batches = data_dict['batch_counter']
        packets = data_dict['pkt_counter']
        self.assertEqual(triggers, exp_triggers)
        self.assertEqual(batches, exp_triggers * batch_cnt)
        self.assertEqual(packets, exp_triggers * batch_cnt * pkt_per_batch)

        # Ensure the verify entries were correctly matched.
        for port in swports:
            trgt = gc.Target(device_id=dev_id, pipe_id=misc_utils.port_to_pipe(port))
            resp = verify_tbl.entry_get(trgt, verify_keys_by_port[port])
            if port == eg_port:
                exp_cnt = 0
            elif port in to_trig_again:
                exp_cnt = 2
            else:
                exp_cnt = 1
            exp_size = exp_cnt * (generated_pkt_len + 6 + 4)
            resp_cnt = 0
            for d,k in resp:
                resp_cnt += 1
                dd = d.to_dict()
                if dd['$COUNTER_SPEC_PKTS'] != exp_cnt or dd['$COUNTER_SPEC_BYTES'] != exp_size:
                    logger.error("Port %d: Verify entry had unexpected counts:\n%s\n%s", port, k.to_dict(), dd)
                    self.assertEqual(dd['$COUNTER_SPEC_PKTS'], exp_cnt)
                    self.assertEqual(dd['$COUNTER_SPEC_BYTES'], exp_size)
            self.assertEqual(resp_cnt, batch_cnt * pkt_per_batch)



@testutils.group("thorough")
class PortDownAppTF2(BfRuntimeTest):
    """
    Example usage of the Port Down Packet Generator Application on Tofino-1.
    This app will trigger when a port goes down.  The generated packets will
    carry the port number of the port that went down.
    The example shows how to configure a Port Down application and how to update
    the "port down" state in the Packet Generator to allow the app to trigger
    for a specific port.
    """
    def setUp(self):
        # This example is for Tofino-2 only.
        if g_is_tofino: return

        misc_utils.setup_random()
        BfRuntimeTest.setUp(self, client_id, p4_name)
        self.bfrt_info = self.interface.bfrt_info_get(p4_name)
        self.pipes = get_pipes(self.bfrt_info)
        self.num_pipes = len(self.pipes)

    def tearDown(self):
        # This example is for Tofino-2 only.
        if g_is_tofino: return

        # Clear the Parser Value Set entries.
        clean_pvs(self.bfrt_info, self.pipes)
        # Clean up the entries in the verify table.
        verify_tbl = self.bfrt_info.table_get('t_port_down_app')
        for pipe in self.pipes:
            trgt = gc.Target(device_id=dev_id, pipe_id=pipe)
            verify_tbl.entry_del(trgt, [])
        # Ensure all apps are disabled
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        for app_id in range(t.info.size_get()):
            t.entry_mod(gc.Target(device_id=dev_id),
                                  [t.make_key([gc.KeyTuple('app_id', app_id)])],
                                  [t.make_data([gc.DataTuple('app_enable', bool_val=False)])])
        # Clear config in the app table
        t.entry_del(gc.Target(device_id=dev_id), [])

        # We are leaving the data in the Packet Generator Buffer table, there is
        # no harm in leaving that config in place since the app table has been
        # cleaned up.

        BfRuntimeTest.tearDown(self)

    def runTest(self):
        # This example is for Tofino-2 only.
        if g_is_tofino: return

        app_tbl = self.bfrt_info.table_get('pktgen.app_cfg')
        pkt_buf_tbl = self.bfrt_info.table_get('pktgen.pkt_buffer')
        verify_tbl = self.bfrt_info.table_get('t_port_down_app')
        tbl_set_asymmetric(verify_tbl)

        # This example will use two different port-down apps.
        app_ids = random.sample(range(app_tbl.info.size_get()), 2)
        app_id_a = app_ids[0]
        app_id_b = app_ids[1]
        generated_pkt_len = 100
        generated_pkt = testutils.simple_ip_packet(pktlen=generated_pkt_len)
        # Port down apps must use a single batch.  The internal Packet Generator
        # header carries the port-id instead of the batch id.
        batch_cnt = 1
        pkt_per_batch = 5
        eg_port = swports[0]

        # Our parser program uses Value Sets to identify packets from the Packet
        # Generator (see P4 for details).  Program the VSs now to identify the
        # packets our timer app will create.
        logger.info("Configuring value sets")
        configure_vs(self.bfrt_info, self.pipes, port_down_app_ids=[app_id_a, app_id_b])

        # Next program the verification table to match and count each of the
        # generated packets.  The generated packets will also be forwarded out
        # a port so we can receive them.
        logger.info("Configuring verify table")
        verify_keys_by_port = dict()
        ids = [(aid, pid) for aid in app_ids for pid in range(pkt_per_batch)]
        for port in swports:
            keys = [verify_tbl.make_key([gc.KeyTuple('hdr.port_down.pipe_id', misc_utils.port_to_pipe(port)),
                                         gc.KeyTuple('hdr.port_down.app_id', aid),
                                         gc.KeyTuple('hdr.port_down.port_num', port),
                                         gc.KeyTuple('hdr.port_down.packet_id', pid)]) for aid,pid in ids]
            verify_keys_by_port[port] = keys

        for port in verify_keys_by_port:
            trgt = gc.Target(device_id=dev_id, pipe_id=misc_utils.port_to_pipe(port))
            d = verify_tbl.make_data([gc.DataTuple('dst_port', eg_port)],
                                     'Ing.count_port_down_app_and_fwd')
            verify_tbl.entry_add(trgt,
                                 verify_keys_by_port[port],
                                 [d]*len(verify_keys_by_port[port]))

        # The following steps are required and can be done in any order, except
        # for enabling the Packet Generator application which should be done
        # last.
        #   - Enable packet generation on the generator port
        #   - Program the packet to generate into the packet buffer
        #   - Configure the Port Mask to include the ports we want to monitor
        #   - Configure the Packet Generator application
        #   - Clear the "port down" state for the ports we want to monitor

        # Enable packet generation on the generator port.
        pgen_port = 6
        pgen_ports = [misc_utils.make_port(pipe, pgen_port) for pipe in self.pipes]
        logger.info("Configuring packet gen ports: %s", pgen_ports)
        port_cfg = self.bfrt_info.table_get('pktgen.port_cfg')
        keys = [port_cfg.make_key([gc.KeyTuple('dev_port', port)]) for port in pgen_ports]
        data = [port_cfg.make_data([gc.DataTuple('pktgen_enable', bool_val=True)]) for _ in self.pipes]
        port_cfg.entry_mod(gc.Target(device_id=dev_id), keys, data)

        # Program the packet into the packet buffer.  The packet buffer is 16kB
        # divided into 1024 "lines", each holding 16B.  The packet should be
        # aligned with a 16B alignment in the buffer.  For this example we will
        # pick a random location within the packet buffer.
        num_lines = (generated_pkt_len + 15) // 16
        first_line = 0
        last_line = 1024 - num_lines
        line = random.randint(first_line, last_line)
        buf_offset = line * 16
        logger.info("Configuring packet buffer at offset: %d", buf_offset)
        t = self.bfrt_info.table_get('pktgen.pkt_buffer')
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('pkt_buffer_offset', buf_offset),
                                 gc.KeyTuple('pkt_buffer_size', generated_pkt_len)])],
                    [t.make_data([gc.DataTuple('buffer', bytearray(bytes(generated_pkt)))])])

        # Decide on the set of ports to flap, we will flap all ports except the
        # single port which is forwarding the generated packets out of the
        # switch.
        # We will divide this set of ports into two groups and use the two masks
        # to allow the two apps to monitor the two sets of ports.
        ports_to_flap = [port for port in swports]
        ports_to_flap.remove(eg_port)
        random.shuffle(ports_to_flap)
        port_set_a = ports_to_flap[:len(ports_to_flap)//2]
        port_set_b = ports_to_flap[len(ports_to_flap)//2:]

        # Use the two sets of ports to program the two masks.  The interface is
        # a little cumbersome, we set a 72-bit value per pipe representing the
        # 72 ports on that pipe.  A value of 1 means the port will be monitored
        # while a value of 0 means the port will be ignored.
        t = self.bfrt_info.table_get('pktgen.port_mask')
        for mask,ports in [(0,port_set_a), (1,port_set_b)]:
            k = t.make_key([gc.KeyTuple('port_mask_sel', mask)])
            for pipe in self.pipes:
                x = 0
                for port in ports:
                    if pipe != misc_utils.port_to_pipe(port): continue
                    lport = misc_utils.port_to_pipe_local_port(port)
                    x = x | (1 << lport)
                d = t.make_data([gc.DataTuple('mask', x)])
                t.entry_mod(gc.Target(device_id=dev_id, pipe_id=pipe), [k], [d])

        # Configure the Packet Generator app.
        logger.info("Configuring app number %d and %d", app_id_a, app_id_b)
        t = self.bfrt_info.table_get('pktgen.app_cfg')
        # Tofino-2 supports two port-masks, the app will only respond to port
        # down events for ports which are set in the mask.  Here we specify
        # which of the two masks we are using.
        data_flds_a = [gc.DataTuple('port_mask_sel', 0)]
        data_flds_b = [gc.DataTuple('port_mask_sel', 1)]
        data_flds = [# Configure the app to generate the packet we programmed
                     # into the packet buffer
                     gc.DataTuple('pkt_len', generated_pkt_len),
                     gc.DataTuple('pkt_buffer_offset', buf_offset),
                     # Program the number of packets to generate.  Note these
                     # values are zero based, so a value of zero makes one
                     # packet and a value of ten makes eleven packets.
                     gc.DataTuple('batch_count_cfg', batch_cnt-1),
                     gc.DataTuple('packets_per_batch_cfg', pkt_per_batch-1),
                     # Program the Inter-Packet-Gap (IPG) and Inter-Batch-Gap
                     # (IBG) in nanoseconds.  Note that on Tofino-1 this is
                     # the time from the start of one packet/batch to the start
                     # of the next.
                     gc.DataTuple('ipg', 0),
                     gc.DataTuple('ibg', 0),
                     # Reset the per-app counters to zero since we are setting
                     # up a new application.
                     gc.DataTuple('trigger_counter', 0),
                     gc.DataTuple('batch_counter', 0),
                     gc.DataTuple('pkt_counter', 0),
                     # Enable the app
                     gc.DataTuple('app_enable', bool_val=True),
                     # Specify the pipe-local port used to generate packets
                     gc.DataTuple('assigned_chnl_id', pgen_port)]
        d_a = t.make_data(data_flds + data_flds_a, 'trigger_port_down')
        d_b = t.make_data(data_flds + data_flds_b, 'trigger_port_down')
        t.entry_mod(gc.Target(device_id=dev_id),
                    [t.make_key([gc.KeyTuple('app_id', app_id_a)]),
                     t.make_key([gc.KeyTuple('app_id', app_id_b)])],
                    [d_a, d_b])

        # The Packet Generator keeps state on whether it has already seen a port
        # go down.  It will only trigger for a port the first time that port
        # goes down; so if a port starts down, comes up, goes down, comes back
        # up, and finally goes down a second time only a single event is
        # triggered.  An API must be called to reset this state for a given port
        # in the Packet Generator.  We'll call that once here for each port
        # since all ports start out in a down state.
        keys = [port_cfg.make_key([gc.KeyTuple('dev_port', port)]) for port in swports]
        data = [port_cfg.make_data([gc.DataTuple('clear_port_down_enable', bool_val=True)]) for _ in swports]
        port_cfg.entry_mod(gc.Target(device_id=dev_id), keys, data)
        logger.info("Cleared port-down state for ports: %s", swports)

        # Make each port go down and verify the expected packets were generated
        # by the expected Packet Generator app.
        exp_triggers = [0,0]
        for port in ports_to_flap:
            flap_port(self.bfrt_info, port)
            if port in port_set_a:
                exp_triggers[0] += 1
                logger.info("Expecting app %d to trigger", app_id_a)
            if port in port_set_b:
                exp_triggers[1] += 1
                logger.info("Expecting app %d to trigger", app_id_b)
            for _ in range(batch_cnt * pkt_per_batch):
                testutils.verify_packet(self, generated_pkt, eg_port)

        # Verify the app counters have the expected values.
        for app_id,exp_trig in zip(app_ids, exp_triggers):
            resp = t.entry_get(gc.Target(device_id=dev_id),
                               [t.make_key([gc.KeyTuple('app_id', app_id)])])
            data_dict = next(resp)[0].to_dict()
            triggers = data_dict['trigger_counter']
            batches = data_dict['batch_counter']
            packets = data_dict['pkt_counter']
            self.assertEqual(triggers, exp_trig)
            self.assertEqual(batches, exp_trig * batch_cnt)
            self.assertEqual(packets, exp_trig * batch_cnt * pkt_per_batch)

        # Ensure the verify entries were correctly matched.
        for port in swports:
            trgt = gc.Target(device_id=dev_id, pipe_id=misc_utils.port_to_pipe(port))
            resp = verify_tbl.entry_get(trgt, verify_keys_by_port[port])
            exp_cnt_a = 0
            exp_cnt_b = 0
            if port in port_set_a:
                exp_cnt_a += 1
            if port in port_set_b:
                exp_cnt_b += 1
            # Each generated packet has a 6 byte Packet Gen header and a 4 byte
            # Ethernet FCS added to it
            exp_size_a = exp_cnt_a * (generated_pkt_len + 6 + 4)
            exp_size_b = exp_cnt_b * (generated_pkt_len + 6 + 4)
            resp_cnt = 0
            for d,k in resp:
                resp_cnt += 1
                dd = d.to_dict()
                kd = k.to_dict()
                if kd['hdr.port_down.app_id']['value'] == app_id_a:
                    exp_cnt = exp_cnt_a
                    exp_size = exp_size_a
                elif kd['hdr.port_down.app_id']['value'] == app_id_b:
                    exp_cnt = exp_cnt_b
                    exp_size = exp_size_b
                if dd['$COUNTER_SPEC_PKTS'] != exp_cnt or dd['$COUNTER_SPEC_BYTES'] != exp_size:
                    logger.error("Port %d: Verify entry had unexpected counts:\n%s\n%s", port, k.to_dict(), dd)
                    self.assertEqual(dd['$COUNTER_SPEC_PKTS'], exp_cnt)
                    self.assertEqual(dd['$COUNTER_SPEC_BYTES'], exp_size)
            self.assertEqual(resp_cnt, len(app_ids) * batch_cnt * pkt_per_batch)

        # Flap a few ports one more time, nothing should be generated since we
        # have not cleared state in the Packet Generator yet.
        for port in ports_to_flap:
            flap_port(self.bfrt_info, port)
        logger.info("Expecting no packets to be generated")
        testutils.verify_no_other_packets(self)
        for app_id,exp_trig in zip(app_ids, exp_triggers):
            resp = t.entry_get(gc.Target(device_id=dev_id),
                               [t.make_key([gc.KeyTuple('app_id', app_id)])])
            data_dict = next(resp)[0].to_dict()
            triggers = data_dict['trigger_counter']
            batches = data_dict['batch_counter']
            packets = data_dict['pkt_counter']
            self.assertEqual(triggers, exp_trig)
            self.assertEqual(batches, exp_trig * batch_cnt)
            self.assertEqual(packets, exp_trig * batch_cnt * pkt_per_batch)

        # Pick a few ports, clear their "down" state, and flap them again.  We
        # expect the Packet Generator to trigger for these.
        to_trig_again = random.sample(ports_to_flap, min(len(ports_to_flap), 3))
        keys = [port_cfg.make_key([gc.KeyTuple('dev_port', port)]) for port in to_trig_again]
        data = [port_cfg.make_data([gc.DataTuple('clear_port_down_enable', bool_val=True)]) for _ in to_trig_again]
        port_cfg.entry_mod(gc.Target(device_id=dev_id), keys, data)
        logger.info("Cleared port-down state for ports: %s", to_trig_again)
        for port in ports_to_flap:
            flap_port(self.bfrt_info, port)
            if port in to_trig_again:
                if port in port_set_a:
                    exp_triggers[0] += 1
                    logger.info("Expecting app %d to trigger", app_id_a)
                if port in port_set_b:
                    exp_triggers[1] += 1
                    logger.info("Expecting app %d to trigger", app_id_b)
                for _ in range(batch_cnt * pkt_per_batch):
                    testutils.verify_packet(self, generated_pkt, eg_port)
            else:
                logger.info("Expecting no packets to be generated")
                testutils.verify_no_other_packets(self)

        # Verify the app counters have the expected values.
        for app_id,exp_trig in zip(app_ids, exp_triggers):
            resp = t.entry_get(gc.Target(device_id=dev_id),
                               [t.make_key([gc.KeyTuple('app_id', app_id)])])
            data_dict = next(resp)[0].to_dict()
            triggers = data_dict['trigger_counter']
            batches = data_dict['batch_counter']
            packets = data_dict['pkt_counter']
            self.assertEqual(triggers, exp_trig)
            self.assertEqual(batches, exp_trig * batch_cnt)
            self.assertEqual(packets, exp_trig * batch_cnt * pkt_per_batch)

        # Ensure the verify entries were correctly matched.
        for port in swports:
            trgt = gc.Target(device_id=dev_id, pipe_id=misc_utils.port_to_pipe(port))
            resp = verify_tbl.entry_get(trgt, verify_keys_by_port[port])
            exp_cnt_a = 0
            exp_cnt_b = 0
            if port in port_set_a:
                exp_cnt_a += 1
                if port in to_trig_again:
                    exp_cnt_a += 1
            if port in port_set_b:
                exp_cnt_b += 1
                if port in to_trig_again:
                    exp_cnt_b += 1
            exp_size_a = exp_cnt_a * (generated_pkt_len + 6 + 4)
            exp_size_b = exp_cnt_b * (generated_pkt_len + 6 + 4)
            resp_cnt = 0
            for d,k in resp:
                resp_cnt += 1
                dd = d.to_dict()
                kd = k.to_dict()
                if kd['hdr.port_down.app_id']['value'] == app_id_a:
                    exp_cnt = exp_cnt_a
                    exp_size = exp_size_a
                elif kd['hdr.port_down.app_id']['value'] == app_id_b:
                    exp_cnt = exp_cnt_b
                    exp_size = exp_size_b
                if dd['$COUNTER_SPEC_PKTS'] != exp_cnt or dd['$COUNTER_SPEC_BYTES'] != exp_size:
                    logger.error("Port %d: Verify entry had unexpected counts:\n%s\n%s", port, k.to_dict(), dd)
                    self.assertEqual(dd['$COUNTER_SPEC_PKTS'], exp_cnt)
                    self.assertEqual(dd['$COUNTER_SPEC_BYTES'], exp_size)
            self.assertEqual(resp_cnt, len(app_ids) * batch_cnt * pkt_per_batch)
