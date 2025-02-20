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

# Run using: python3 -m unittest discover
import time
import random
import string
import re
import runner_test
import telnetlib

class TestTelnet(runner_test.RunnerTest):

    def test_cli_basic(self):
        """
        Test the cli can be reached
        """
        self.start_runner()
        self.telnet_expect( b"Tofino Model CLI")
        self.telnet_expect( b">")
        self.exit_runner_and_capture_output()

    #@unittest.skip("temp")
    def test_cli_bad_input(self):
        """
        Test the cli handles bad input
        """
        self.start_runner()
        try :
            self.telnet_expect( b"Tofino Model CLI")
            self.telnet_expect( b">")

            # try a super long command (the messages won't work perfectly)
            random_cmd = b'X'
            for i in range(10000) :
                random_cmd += b'dsfjkhdsfhkdsfhkdksjfhdksjfhdsjfhdsj'
            self.telnet.write(random_cmd + b"\n\r")
            # can't use expect here because echoed command will be truncated
            prompt = self.telnet.read_until(b'Invalid command',2)
            self.assertIn( b'Invalid command', prompt )
            prompt = self.telnet.read_until(b'>',2)
            self.assertIn( b'>', prompt )
            
            # try a long command (but not too long that the messages don't work)
            random_cmd = b'X'
            for i in range(100) :
                random_cmd += b'dsfjkhdsfhkdsfhkdksjfhdksjfhdsjfhdsj'
            self.telnet.write(random_cmd + b"\n\r")
            self.telnet_expect( random_cmd ) # it echos back
            self.telnet_expect( b'Invalid command "' + random_cmd + b'"')

            self.telnet_expect( b">")
            
            self.exit_runner_and_capture_output()
        except:
            self.fail("Exception while sending random commands")
            
    #@unittest.skip("temp")
    def test_no_cli(self):
        """
        Test the no cli option works
        """
        self.start_runner('--no-cli', open_telnet=False)
        with self.assertRaises( ConnectionRefusedError ) :
            telnetlib.Telnet( "localhost", 8000, 2 )

    #@unittest.skip("temp")
    def test_cli_username_password(self):
        """
        Test the cli username/password option works
        """
        self.start_runner('--cli-credentials', '-', userpass=b'User1:Pass1\n')

        try :
            
            # Test that a bad password doesn't work
            self.telnet.read_until(b"Username: ")
            self.telnet.write(b"User1\n\r")
            self.telnet.read_until(b"Password:")
            self.telnet.write(b"BadPass1\n\r")
            self.telnet_expect( b"Access denied" )

            # Test that a good password works
            self.telnet.read_until(b"Username: ")
            self.telnet.write(b"User1\n\r")
            self.telnet.read_until(b"Password:")
            self.telnet.write(b"Pass1\n\r")
            self.telnet_expect( b">" )

        except:
            self.fail("Exception during telnet exchange")
        self.exit_runner_and_capture_output()

    # to test setting logging need to open the sockets and reset the chip
    def test_cli_set_logging(self):
        """
        Test the cli can be reached
        """
        self.start_runner()
        
        self.open_sockets()
        self.telnet_expect(b"Tofino Model CLI")
        self.reset_chip()

        self.telnet.write(b"rmt-set-log-flags -f 0xFFFFFFFFFFFFFFFF\n\r")
        self.telnet_expect(b"rmt-set-log-flags -f 0xFFFFFFFFFFFFFFFF")

        # Do a read and check it returns 0bad0bad
        self.assertEqual( self.do_read( 0, 0xAA ), 0x0bad0bad, 'Address did not return expected 0x0bad0bad' )
        #self.telnet_expect(b"", timeout=120)

        self.close_sockets()
        self.exit_runner_and_capture_output()
        #print(self.runner_output)

        # check the message from turning logging on
        self.assertIn( b'RmtObjectManager::update_log_flags(pipes=0xffffffffffffffff stages=0xffffffffffffffff types=0xffffffffffffffff rows_tabs=0xffffffffffffffff cols=0xffffffffffffffff OR=0xffffffff 0xffffffff  AND=0xffffffff 0xffffffff  VER=000)', self.runner_output )
        # check that the read is logged
        self.assertIn( b'V Read from unimplemented address 0x000000aa', self.runner_output )
        self.assertFalse( self.runner_error, "Runner stderr not empty" )

if __name__ == '__main__':
    unittest.main()
