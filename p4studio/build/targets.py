#!/usr/bin/env python3

#  INTEL CONFIDENTIAL
#
#  Copyright (c) 2021 Intel Corporation
#  All Rights Reserved.
#
#  This software and the related documents are Intel copyrighted materials,
#  and your use of them is governed by the express license under which they
#  were provided to you ("License"). Unless the License provides otherwise,
#  you may not use, modify, copy, publish, distribute, disclose or transmit this
#  software or the related documents without Intel's prior written permission.
#
#  This software and the related documents are provided as is, with no express or
#  implied warranties, other than those that are expressly stated in the License.
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
