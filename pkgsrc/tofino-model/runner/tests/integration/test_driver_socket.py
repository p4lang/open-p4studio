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
import random
import runner_test

class TestDriverSocket(runner_test.RunnerTest):
    def __init__(self, *args, **kwargs):
        super(TestDriverSocket, self).__init__(*args, **kwargs)
        
    def test_driver_basic(self):
        # comment start_runner out if you want to run the model in another window
        self.start_runner()
        self.open_sockets()
        self.telnet_expect(b"Tofino Model CLI")

        # check can do a write
        self.do_write(0, 0xAA, 0x12345678 )
        # Do a read and check it returns 0bad0bad
        self.assertEqual( self.do_read( 0, 0xAA ), 0x0bad0bad, 'Address did not return expected 0x0bad0bad' )

        self.reset_chip()
        
        self.close_sockets()
        self.exit_runner_and_capture_output()
        if self.runner_output :
            self.assertIn( b'Tofino Verification Model - Version', self.runner_output, "Sanity Check" )
        self.assertFalse( self.runner_error, "Runner stderr not empty"  )

    def test_driver_random_bad_input(self):
        # comment start_runner out if you want to run the model in another window
        self.start_runner()
        self.open_sockets()
        self.telnet_expect(b"Tofino Model CLI")
        self.reset_chip()

        # put some random 40 byte messages in the driver socket
        for i in range(100000) :
            msg = bytes( [ random.getrandbits(8) for _ in range(40) ] )
            #print(msg+ b'\n')
            self.send_data(msg)
        
        self.close_sockets()
        self.exit_runner_and_capture_output()

        self.assertTrue( self.runner_output, "Runner stdout empty" )
        self.assertIn( b'Tofino Verification Model - Version', self.runner_output, "Sanity Check" )
        self.assertFalse( self.runner_error, "Runner stderr not empty"  )


        
if __name__ == '__main__':
    unittest.main()

