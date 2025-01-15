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

import os
from pathlib import Path
from typing import List

import click
from click import Choice

from build.targets import all_targets_by_group
from utils.exceptions import ApplicationException
from utils.log import logging_options
from utils.processes import try_execute
from utils.terminal import print_green, columnize
from workspace import current_workspace, in_workspace
from utils.pip_utils import python_packages_target_path


def _describe_build_command() -> str:
    result = "Build SDE components and P4 programs\n\n"
    result += "TARGET is the name of P4 program or switch profile\n\n"

    if in_workspace():
        result += "Following list cover all acceptable names:\n\n"
        for name, targets in all_targets_by_group().items():
            if targets:
                result += "\b\n{}:\n{}\n".format(name, columnize(targets, 2))
    return result


def _allowed_targets() -> List[str]:
    return sorted(sum(all_targets_by_group().values(), [])) if in_workspace() else []


@click.command(name='build', help=_describe_build_command(), short_help="Build SDE components and P4 programs\n\n")
@click.argument('targets', type=Choice(_allowed_targets()), nargs=-1, metavar='[TARGET]...')
@logging_options()
@click.option('--jobs', default=4, help="Allow N jobs at once")
def build_command(targets: List[str], jobs: int) -> None:
    print_green("Building and installing {}...", current_workspace().name)

    build_dir = current_workspace().build_path

    if not build_dir.is_dir():
        msg = "Build not configured. check p4studio configure --help for more details"
        raise ApplicationException(msg)
    # FIXME(#18): This pathing should be set up much earlier.
    # FIXME(#18): We have a TODO missing here.
    py_path = python_packages_target_path(current_workspace().default_install_dir, "TODO")
    print_green("Building...")
    make("Build", build_dir, py_path, jobs, list(targets))
    print_green("Built successfully")
    print_green("Installing...")
    make("Installation", build_dir, py_path, jobs, ['install'])
    print_green("Installed successfully")
    print_green("{} built and installed.", current_workspace().name)


def make(name: str, working_dir: Path, py_path : Path, jobs: int, targets: List[str]) -> None:
    jobs_arg = '--jobs={}'.format(jobs) if jobs else '--jobs'

    # FIXME(#18): This pathing should be set up much earlier.
    env = os.environ.copy()
    current_pythonpath = env.get("PYTHONPATH", "")
    updated_pythonpath = os.pathsep.join([current_pythonpath, str(py_path)]) if current_pythonpath else str(py_path)
    env["PYTHONPATH"] = updated_pythonpath
    env["PATH"] = f'{py_path}/bin:{env["PATH"]}'

    command = ['make', jobs_arg] + targets
    if not try_execute(command, working_dir, override_env=env):
        message = "{} completed unsuccessfully".format(name)
        raise ApplicationException(message)
