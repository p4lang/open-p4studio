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

def pktpy_skip(cls):
    """
    Test Case decorator that marks the test as being disabled for
    specific vars. In this case is designed to skip test which is not ready
    to work with bf_pkpty (replacement for the Scapy).
    """
    import os
    if os.getenv("PKTPY", "true").lower() != "false":
        cls._disabled = True
    return cls


def pktpy_skip_test(func):
    """
    Test Case decorator that marks the test as being skipped.
    In this case is designed to skip test which is not ready to work
    with bf_pkpty (replacement for the Scapy).
    """
    def inner(*args, **kwargs):
        try:
            print("Test {test_name} was "
                  "skipped due to incompatibility "
                  "with bf-pktpy".format(test_name=func.__name__))
        except:
            print(
                "Test was skipped due to incompatibility with bf-pktpy")
    return inner
