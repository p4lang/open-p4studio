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

from build import build_command
from config import configure_command
from dependencies.dependencies_command import install_command
from profile.profile import Profile
from utils.click import command_call_to_str
from utils.terminal import print_green, print_normal, print_separator
from typing import Optional, Dict, Union, cast


class ProfileExecutionPlan:
    def __init__(self, profile: Profile, bsp_path: Optional[str], jobs: Optional[int],
                 dependencies_working_dir: Optional[str] = None):
        self.profile = profile
        self.bsp_path = bsp_path or profile.bsp_path
        self.jobs = jobs
        self.dependencies_working_dir = dependencies_working_dir

    def describe_profile(self) -> None:
        print_green('Source packages to install: ')
        for package in self.profile.source_packages():
            print_normal(" - {}", package)

        print_green('Configuration options: ')
        for option, enabled in self.profile.config_options().items():
            print_normal(" {} {}", '✓' if enabled else '✗', option)

        print_green('Targets to build: ')
        for target in self.profile.build_targets():
            print_normal(" - {}", target)

        print_separator()

    def show_commands(self) -> None:
        commands = [
            command_call_to_str(install_command, **self.dependencies_install_args()),
            command_call_to_str(configure_command, **self.configure_args()),
            command_call_to_str(build_command, **self.build_args()),
        ]

        print_green("Profile is equivalent to below list of commands:")
        for command in commands:
            print_normal(command)

    def dependencies_install_args(self) -> Dict[str, Union[str, int]]:
        result = cast(Dict[str, Union[str, int]], {
            'source_packages': ','.join(self.profile.source_packages()),
        })

        if self.jobs:
            result['jobs'] = self.jobs
        if self.dependencies_working_dir:
            result['working_dir'] = self.dependencies_working_dir
        return result

    def configure_args(self) -> Dict[str, object]:
        return {
            'options': tuple(self.profile.config_args()),
            'bsp_path': self.bsp_path,
            'p4ppflags': self.profile.p4ppflags,
            'p4flags': self.profile.p4flags,
            'extra_cppflags': self.profile.extra_cppflags,
            'kdir': self.profile.kdir
        }

    def build_args(self) -> Dict[str, object]:
        result = cast(Dict[str, object], {
            'targets': tuple(self.profile.build_targets()),
        })
        if self.jobs:
            result['jobs'] = self.jobs
        return result
