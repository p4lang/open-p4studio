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

from os.path import join, exists
from pathlib import Path

from dependencies.source.source_dependency_config import SourceDependencyConfig
from utils.processes import execute

import os

def download_libcrafter(config: SourceDependencyConfig) -> None:
    version = config.dependency_manager().source_dependency_attributes("libcrafter")['version']
    repo_dir = config.download_dir(ensure_exists=False)
    if not repo_dir.exists():
        execute("git clone https://github.com/pellegre/libcrafter.git {}".format(repo_dir))
        execute("git -c advice.detachedHead=false checkout {}".format(version), repo_dir)


def install_libcrafter(config: SourceDependencyConfig) -> None:
    version = config.dependency_manager().source_dependency_attributes("libcrafter")['version']

    if not config.force and _is_libcrafter_installed(config.install_dir, version):
        return

    download_libcrafter(config)
    build_dir = config.build_dir(copy_download_dir=True)
    build_dir = build_dir / 'libcrafter'

    # Set the environment variable
    env = os.environ.copy()
    env['LDFLAGS'] = "-Wl,-s"

    execute("./autogen.sh".format(config.jobs), build_dir)
    execute("./configure".format(config.jobs), build_dir)
    execute('make PREFIX={} install -j{}'.format(config.install_dir, config.jobs), build_dir)
    execute("sudo ldconfig")


def _is_libcrafter_installed(path: Path, version: str) -> bool:
    return (path / "lib/libcrafter.so.{}".format(version)).exists()
