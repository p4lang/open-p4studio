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
import unittest
from subprocess import Popen, PIPE
import telnetlib
from os import kill
import socket
import threading
from ctypes import *
from enum import Enum
import shutil

# structures for the driver socket
class pcie_op_e(Enum):
    pcie_op_rd     = 0
    pcie_op_wr     = 1
    pcie_op_dma_rd = 2
    pcie_op_dma_wr = 3

class pcie_msg_t(Structure):
     _fields_ = [
         ("typ", c_int),
         ("asic", c_uint32),
         # Direct Access
         ("addr", c_uint64),
         ("value", c_uint32),
         # Indirect Access
         ("ind_addr", c_uint64),
         # only for DMA rd/wr
         ("len", c_uint32),
     ]


class RunnerTest(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(RunnerTest, self).__init__(*args, **kwargs)
        self.runner = None
        self.telnet = None
        self.runner_output = None
        self.runner_error  = None

        
    def tearDown(self) :
        self.kill_runner()
        
    def start_runner(self,*args, userpass=None, open_telnet=True) :
        search_path='.:../..:../../../../../install/bin/'
        model = shutil.which('tofino-model',path=search_path)
        #print(f'starting {model}')
        self.assertTrue(model,f'Could not find tofino-model in {search_path}')
        self.runner = Popen([model] + list(args), stdin=PIPE, stdout=PIPE, stderr=PIPE)
        # If required, have to do the user/password sending before trying to open telnet
        if userpass :
            self.runner.stdin.write(userpass)
            self.runner.stdin.flush()
        time.sleep(2)
        if open_telnet :
            self.telnet = telnetlib.Telnet( "localhost", 8000, 2 )
        time.sleep(1)
        
    def kill_runner(self) :
        if self.runner :
            self.runner.stdin.close()
            self.runner.stdout.close()
            self.runner.stderr.close()
            if self.runner.pid :
                kill(self.runner.pid,9)
            self.runner.wait()
            self.runner = False
            time.sleep(2)

    def telnet_expect( self, expected, timeout=2 ) :
        idx, obj, response = self.telnet.expect([expected],timeout)
        self.assertEqual( idx, 0, f'telnet expected: "{expected.decode()}" got "{response.decode()}"' )
            
    ########### Driver socket functions
    def open_sockets(self):
        # need to listen on the dma socket before data will be accepted on reg socket
        try: 
            self.dma_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.dma_socket.bind( ("localhost", 8002) )
            self.dma_socket.listen(2)
        except:
            self.fail("Exception while opening dma socket")
        try :
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.connect(("localhost", 8001))
        except:
            self.fail("Exception while opening socket")

    def close_sockets(self):
        self.s.close()
        self.dma_socket.close()

    def reset_chip(self):
        # might only work for Tofino, as addresses may be different
        #  in other chips

        # From model/include/tofino/register_includes/reg.h
        soft_reset_address = 0x40000
        dbg_rst1_address = 0x40128
        # From model/include/tofinoXX/rmt-defs-tofinoXX.h
        kSoftResetCtrlVal      = 0x0040
        kSoftResetStatusMask   = 0x0039
        kSoftResetStatusMask   = 0x0039
        kSoftResetStatusValOk  = 0x0001
        kSoftResetStatusValErr = 0x0000

        status_mask = kSoftResetStatusMask
        status_ok = kSoftResetStatusValOk
        reset_val = kSoftResetCtrlVal

        #print("assert SwRESET")
        self.do_write( 0, soft_reset_address, reset_val)  # Assert SwRESET
        time.sleep(2)
        status_val = self.do_read( 0, dbg_rst1_address )
        #print(status_val)
        # does not seem to be OK yet, could poll?
        #self.assertEqual( status_ok, status_val & status_mask )

        #print("deassert SwRESET")
        self.do_write( 0, soft_reset_address, 0)  # Deassert SwRESET
        time.sleep(2)
        status_val = self.do_read( 0, dbg_rst1_address )
        self.assertEqual( status_ok, status_val & status_mask, 'Status not OK' )

        # set the log flags, this will block util the reset has finished
        if self.telnet :
            self.telnet.write(b"rmt-set-log-flags -f 0xf\n\r")
            self.telnet_expect(b"rmt-set-log-flags", timeout=60)
            self.telnet_expect(b">", timeout=60)
        

    def exit_runner_and_capture_output(self):
        # need to set up another thread to do the capture so we can send the exit commands
        if self.runner :
            capture_thread = threading.Thread(target=self.capture_runner_output)
            capture_thread.start()
            time.sleep(2)

            # Not sure why 2 exits are needed to get the runner to exit!
            if self.telnet :
                self.telnet.write(b"exit-model\n\r")
                self.telnet = telnetlib.Telnet("localhost",8000,2)
                self.telnet.write(b"exit-model\n\r")

            capture_thread.join()
            self.runner = None   # prevent killing the model in teardown


    def send_data(self, data):
        sent_count = 0
        while sent_count < len(data):
            sent = self.s.send(data[sent_count:])
            self.assertNotEqual( sent, 0, "socket error" )
            sent_count += sent

    def receive_data(self):
        chunks = []
        bytes_recd = 0
        message_len = len(bytearray(pcie_msg_t()))
        while bytes_recd < message_len :
            chunk = self.s.recv(min(message_len - bytes_recd, 256))
            self.assertNotEqual( chunk, b'', "socket connection broken" )
            chunks.append(chunk)
            bytes_recd = bytes_recd + len(chunk)
        r = pcie_msg_t.from_buffer_copy( b''.join(chunks) )
        return r
    
    def capture_runner_output(self):
        if self.runner :
            self.runner_output, self.runner_error = self.runner.communicate()

    def do_indirect_write(self, asic, addr, data0, data1 ) :
        # Register addresses
        cpu_ind_addr_low = 0x38
        cpu_ind_addr_high = 0x3c
        cpu_ind_data00 = 0x40
        cpu_ind_data01 = 0x44
        cpu_ind_data10 = 0x48
        cpu_ind_data11 = 0x4c
        # send the address
        addr_lo = addr & 0xFFFFFFFF;
        addr_hi = addr >> 32
        self.do_write( asic, cpu_ind_addr_low,  addr_lo)
        self.do_write( asic, cpu_ind_addr_high,  addr_hi)
        # Send the data
        d11 = data1 >> 32
        d10 = data1 & 0xFFFFFFFF
        d01 = data0 >> 32
        d00 = data0 & 0xFFFFFFFF
        self.do_write( asic, cpu_ind_data11, d11)
        self.do_write( asic, cpu_ind_data10, d10)
        self.do_write( asic, cpu_ind_data01, d01)
        self.do_write( asic, cpu_ind_data00, d00)
        return
    
    
    def do_write(self, asic, addr, data) :
        buf = pcie_msg_t()
        buf.typ = int(pcie_op_e.pcie_op_wr.value)
        buf.asic = asic
        buf.addr = addr
        buf.value = data
        buf.ind_addr = 0
        buf.len = 0
        self.send_data( bytearray(buf) )
        return

    def do_read(self, asic, addr) :
        buf = pcie_msg_t()
        buf.typ = int(pcie_op_e.pcie_op_rd.value)
        buf.asic = asic
        buf.addr = addr
        buf.value = 0
        buf.ind_addr = 0
        buf.len = 0
        self.send_data( bytearray(buf) )
        r = self.receive_data()
        self.assertEqual( r.addr, addr, 'Unexpected address in reply to read' )
        return r.value
    

