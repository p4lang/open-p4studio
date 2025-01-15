#!/usr/bin/env python3

# Copyright (C) 2024 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.  You may obtain
# a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
#
# SPDX-License-Identifier: Apache-2.0

from collections import OrderedDict
from typing import Dict, List, Any, cast

import yaml

from program import all_p4_programs
from utils.collections import nested_get
from workspace import current_workspace


def all_switch_profiles() -> List[str]:
    switch_profiles_yaml = _load_switch_profiles_yaml()
    switch_p4_16_profiles = switch_profiles_yaml["switch_p4_16_profiles"]
    architectures = switch_p4_16_profiles.keys() if switch_p4_16_profiles else []
    result = []
    for architecture in architectures:
        supported_profiles = cast(List[str],
                                  nested_get(switch_p4_16_profiles, '{}/supported_profiles'.format(architecture), []))
        result.extend(supported_profiles)
    return result


def get_switch_profiles_by_arch(arch: str) -> List[str]:
    return [profile for profile in all_switch_profiles() if profile.endswith(arch)]


def _load_switch_profiles_yaml() -> Dict[str, Any]:
    return yaml.safe_load(current_workspace().switch_profiles_yaml)


def all_targets_by_group() -> Dict[str, List[str]]:
    result = p4_program_names_by_group()
    result['Profiles'] = all_switch_profiles()

    result['Grouped'] = list(current_workspace().p4_dirs.keys()) + ['p4-examples']
    return result


def p4_program_names_by_group() -> Dict[str, List[str]]:
    result = OrderedDict()
    programs = all_p4_programs()

    for program in programs:
        group = program.group
        if group not in result:
            result[group] = []
        result[group].append(program.name)

    return result
