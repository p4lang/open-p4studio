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

import os
import sys
import json
import argparse
from collections import OrderedDict
import csv
from termcolor import colored

def read_file(filename):
    with open(filename) as data_file:
        data = json.load(data_file)
        return data

def appendObjInDict(json_dict, file_name):
    with open(file_name, "rb") as infile:
        json_data = infile.read()
        json_objects = json.loads(json_data, object_pairs_hook=OrderedDict)

    for obj in json_objects:
        for obj_key, obj_value in list(obj.items()):
            json_dict[obj_key] = obj_value

if __name__ == "__main__":
    cmd = "cd api && rgrep \", SWITCH_.*_ATTR_\" | grep -v attribute_get | awk '{print substr($3, 1, length($3)-1)}' | sort | uniq | grep SWITCH & cd -"
    attrs = os.popen(cmd).read()

    json_data = {}
    arr = os.listdir(os.path.join("../schema/appObj"))
    for file in arr:
        appendObjInDict(json_data, os.path.join("../schema/appObj", file))

    f_count = 0
    t_count = 0
    outfile = open("ptf_coverage.csv", "wb")
    csv_writer = csv.writer(outfile, delimiter=',')

    for obj_key, obj_value in iter(sorted(json_data.items())):
        if obj_value['class'] == "user":
            total = 0
            tested = 0
            for attr_key, attr_value in list(obj_value["attributes"].items()):
                if "is_create_only" not in list(attr_value.keys()) and "is_read_only" not in list(attr_value.keys()) and "is_internal" not in list(attr_value.keys()) and "is_immutable" not in list(attr_value.keys()):
                    attr = "SWITCH_" + obj_key.upper() + "_ATTR_" + attr_key.upper()
                    present = False
                    f_count += 1
                    total += 1
                    if attr in attrs:
                        t_count += 1
                        tested += 1
                        present = True
                    csv_writer.writerow([obj_key, attr, present])
            if total > tested:
                percent = float(tested)/float(total) * 100.0
                print(("%20s: Total: %-3d Tested: %-2d : %d %%") % (obj_key, total, tested, percent))

    outfile.close()
    print("---------------------------------------------------")
    percent = float(t_count)/float(f_count) * 100.0
    print(("%20s: Total: %-3d Tested: %-2d : %d %%") % ("All Objects", f_count, t_count, percent))
