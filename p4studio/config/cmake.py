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
import re
from pathlib import Path
from typing import List, Iterable

import click
import cmakeast
from cmakeast.ast import FunctionCall

from utils.exceptions import ApplicationException
from utils.processes import try_execute
from workspace import current_workspace
import os

BUILD_TYPES = ['Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel']


def cmake(build_dir: Path, py_path : Path, options: List[str]) -> None:
    if build_dir.exists():
        if not build_dir.is_dir():
            message = '{} does not exist or is not a directory'.format(build_dir)
            raise click.ClickException(message)
    else:
        build_dir.mkdir(parents=True)
    env = os.environ.copy()
    # FIXME(#18): This pathing should be set up much earlier.
    current_pythonpath = env.get("PYTHONPATH", "")
    updated_pythonpath = os.pathsep.join([current_pythonpath, str(py_path)]) if current_pythonpath else str(py_path)
    env["PYTHONPATH"] = updated_pythonpath
    env["PATH"] = f'{py_path}/bin:{env["PATH"]}'

    command = ['cmake', str(current_workspace().root_path)] + options
    if not try_execute(command, build_dir, override_env=env):
        raise ApplicationException("cmake completed unsuccessfully")


class CmakeArg:

    def will_be_ambiguous_in(self, collection: Iterable['CmakeArg']) -> bool:
        return any([self.arg == x.arg and self.value != x.value for x in collection])

    def __init__(self, arg: str, value: str):
        self.arg = arg
        self.value = value

    def __str__(self) -> str:
        return "{}={}".format(self.arg, self.value)

    def __repr__(self) -> str:
        return str(self)

    @staticmethod
    def from_str(string: str) -> "CmakeArg":
        regex = r"(?P<name>-D[^=]+)=(?P<value>.*)"
        match = re.match(regex, string)
        if not match:
            raise ApplicationException("{} is not a valid cmake argument".format(string))
        groups = match.groupdict()
        return CmakeArg(groups["name"], CmakeArg.canonicalize_value(groups["value"]))

    @staticmethod
    def canonicalize_value(value: str) -> str:
        off_values = ["off", "no", "n", "false"]
        on_values = ["on", "yes", "y", "true"]
        if value.lower() in on_values:
            value = "ON"
        if value.lower() in off_values:
            value = "OFF"
        return value

    def __eq__(self, other: object) -> bool:
        return str(self) == str(other)


class CmakeOptionDefinition:
    DESCRIPTION_PATTERN = re.compile(r'^((?P<category>[a-zA-Z0-9-]+):)? +(?P<description>.+)$')

    def __init__(self, name: str, default: bool, description: str):
        self.name = name
        self.default = default

        match = self.DESCRIPTION_PATTERN.search(description)
        if match is not None:
            self.category = match.groupdict()['category'] or 'Global'
            self.description = match.groupdict()['description']
        else:
            self.category = 'Global'
            self.description = description

    def as_cmake_flag(self, enabled: bool) -> CmakeArg:
        return CmakeArg("-D{}".format(self.name), "ON" if enabled else "OFF")


def available_cmake_options() -> List[CmakeOptionDefinition]:
    """
    Returns list of cmake options available in current workspace
    """
    file_content = current_workspace().cmake_lists_txt.read_text()
    ast = cmakeast.ast.parse(file_content)
    return [
        CmakeOptionDefinition(
            name=statement.arguments[0].contents,
            description=statement.arguments[1].contents.strip('"'),
            default=statement.arguments[2].contents.lower() == 'on'
        )
        for statement in ast.statements
        if isinstance(statement, FunctionCall)
        if statement.name == 'option'
    ]
