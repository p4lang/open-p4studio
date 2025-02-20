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

class TestCommandLineArgs(runner_test.RunnerTest):
    def __init__(self, *args, **kwargs):
        super(TestCommandLineArgs, self).__init__(*args, **kwargs)

    def bad_args_test(self,arg, err=b'unrecognized option'):
        self.start_runner(arg,open_telnet=False)
        self.exit_runner_and_capture_output()
        self.assertIn( b'-h,--help Display this help message and exit', self.runner_output )
        self.assertIn( err, self.runner_error )
        
    #@unittest.skip("temp")
    def test_bad_long_arg(self):
        self.bad_args_test('--bad-arg')

    def test_bad_short_arg(self):
        self.bad_args_test('-Q', err=b'invalid option')

    def test_bad_no_devices(self):
        self.bad_args_test('-d', err=b'option requires an argument')
        
    def test_bad_random_long(self):
        arg = b'--' + bytes( [ random.randint(1,255) for _ in range(100) ] )
        #print(arg)
        self.bad_args_test(arg)
        
    def test_bad_random_really_long(self):
        arg = b'--' + bytes( [ random.randint(1,255) for _ in range(100000) ] )
        #print(arg)
        self.bad_args_test(arg)
        
if __name__ == '__main__':
    unittest.main()

