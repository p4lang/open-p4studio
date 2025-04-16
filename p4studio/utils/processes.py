# !/usr/bin/env python3

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
import subprocess
from datetime import timedelta
from pathlib import Path
from shlex import split
from timeit import default_timer as timer
from typing import List, Sequence, Union, Mapping, Dict, cast, IO, AnyStr, Optional

from utils.exceptions import ApplicationException
from utils.types import PathLike

try:
    from shlex import quote as cmd_quote
except ImportError:
    from pipes import quote as cmd_quote


def execute(command: Union[str, Sequence[str]], working_dir: Optional[PathLike] = None,
            override_env: Dict[str, str] = None) -> None:
    if not try_execute(command, working_dir=working_dir, override_env=override_env):
        raise ApplicationException("Failed to run: {}".format(command))


def try_execute(command: Union[str, Sequence[str]], working_dir: Optional[PathLike] = None,
                override_env: Dict[str, str] = None) -> bool:
    from utils.terminal import compact_log

    if isinstance(command, str):
        command = split(command)

    env = os.environ.copy()
    if override_env:
        env.update(override_env)

    try:
        if working_dir is None:
            working_dir = Path.cwd()
            compact_log().log("Executing: {}".format(' '.join(command)))
        else:
            compact_log().log("Executing: '{}' in {}".format(' '.join(command), working_dir))
        start = timer()
        process = run_subprocess(command, working_dir, env)

        while process.poll() is None:
            while True:
                stdout = process.stdout
                line = stdout.readline().decode('latin-1').rstrip() if stdout else ""
                if line:
                    compact_log().log(line)
                else:
                    break

        end = timer()
        compact_log().log("Cmd '{}' took: {}, status: {}".format(' '.join(command), timedelta(seconds=end - start),
                                                                 process.returncode))
        success = process.returncode == 0

        if success:
            compact_log().reset(include_last=False)
        else:
            compact_log().dump()

        return success

    except FileNotFoundError as e:
        raise ApplicationException from e


def run_subprocess(command: Union[str, Sequence[str]], working_dir: PathLike = Path.cwd(),
                   env: Mapping[str, str] = None) -> subprocess.Popen:
    return subprocess.Popen(
        split(command) if isinstance(command, str) else command,
        cwd=str(working_dir),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        env=env
    )


def command_output(command: List[str]) -> bytes:
    try:
        return subprocess.check_output(command)
    except subprocess.CalledProcessError as e:
        raise ApplicationException from e


def check_command(command: List[str], override_env: Mapping[str, str] = None) -> bool:
    env = os.environ.copy()
    if override_env:
        env.update(override_env)
    process = run_subprocess(command, env=env)
    process.communicate()
    return process.returncode == 0


def cmd_args_to_str(args: Sequence[str]) -> str:
    return " ".join(map(cmd_quote, args))
