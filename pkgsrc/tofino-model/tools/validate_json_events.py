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


from jsonschema import validate
import json
import argparse
import os

PATH = os.path.dirname(os.path.realpath(__file__))

parser = argparse.ArgumentParser(description="Validate model events")
parser.add_argument("json", type=argparse.FileType('r'))
args = parser.parse_args()

with open(PATH + '/../schema/model-events.json') as f:
    SCHEMA = json.loads(f.read())

for line in args.json:
    try:
        JSON = json.loads(line)
        validate(JSON, SCHEMA)
    except Exception as e:
        print("Validation failed!")
        print(line)
        raise e
print("Validation success.")
