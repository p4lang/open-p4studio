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

class TestException(runner_test.RunnerTest):
    def __init__(self, *args, **kwargs):
        super(TestException, self).__init__(*args, **kwargs)
        
    def test_exception(self):
        # comment start_runner out if you want to run the model in another window
        self.start_runner()
        self.open_sockets()
        self.telnet_expect(b"Tofino Model CLI")
        self.reset_chip()
        
        # this should cause a model assert:
        #   ERROR MauMemory::bad_memtype_phys_write(0x0000000040140000) - Invalid mem type 5
        self.do_indirect_write(0,  ((2<<40) | (2<<30) | (5<<18)), 0xBAD1, 0xBAD2 )
        self.telnet = None # model will exit, so prevent trying to send exit-model to telnet

        self.close_sockets()
        self.exit_runner_and_capture_output()

        if self.runner_output :
            #print(self.runner_output.decode())
            self.assertIn( b'Invalid mem type 5', self.runner_output )
        if self.runner_error :
            #print("STDERR:\n"+self.runner_error.decode())
            self.assertIn( b'(bad_memtype_phys_write): error -3 thrown.', self.runner_error )
            self.assertIn( b'tofino-model: internal error - please submit a bug report', self.runner_error )
        # fail the test if there wasn't any runner output
        self.assertTrue( self.runner_output, "Runner stdout empty" )
        self.assertTrue( self.runner_error, "Runner stderr empty" )

if __name__ == '__main__':
    unittest.main()

