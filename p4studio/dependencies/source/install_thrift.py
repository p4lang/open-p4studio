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

from dependencies.source.source_dependency_config import SourceDependencyConfig
from utils.pkg_config import check_pkg_config
from utils.processes import execute

_THRIFT_FILE = 'thrift.tar.gz'


def download_thrift(config: SourceDependencyConfig) -> None:
    attrs = config.dependency_manager().source_dependency_attributes("thrift")

    thrift_package = config.download_dir(ensure_exists=True) / _THRIFT_FILE
    if not thrift_package.exists():
        execute("wget {} -nv -O {}".format(attrs["url"], thrift_package))


def install_thrift(config: SourceDependencyConfig) -> None:
    attrs = config.dependency_manager().source_dependency_attributes("thrift")
    version = attrs["version"]

    if not config.force and _is_thrift_installed(version, config.install_dir):
        return

    download_thrift(config)
    build_dir = config.build_dir()
    thrift_package = config.download_dir(ensure_exists=True) / _THRIFT_FILE
    execute("tar xf {} --strip-components 1 -C {}".format(thrift_package, build_dir))

    override_envs = {
        'PATH': '{}/bin:{}'.format(config.install_dir, os.environ['PATH']),
    }
    configure_command = 'CXXFLAGS="-std=c++17" ./configure PY_PREFIX={dir} {flags} --prefix={dir} --with-boost={dir}'.format(
        dir=config.install_dir,
        flags=attrs['flags']
    )
    execute(configure_command, build_dir, override_env=override_envs)

    execute('make -j{}'.format(config.jobs), build_dir)
    execute("make install -j{}".format(config.jobs), build_dir)
    execute("sudo ldconfig", build_dir)

    if config.os.lower() in {'ubuntu:20.04', 'ubuntu:22.04', 'ubuntu:24.04', 'debian:11'}:
        python_thrift_package = 'python3-thrift'
    else:
        python_thrift_package = 'python-thrift'

    os_package_manager = config.dependency_manager().os_package_manager
    execute('sudo {} -y remove {}'.format(os_package_manager, python_thrift_package), build_dir)


def _is_thrift_installed(version: str, path: Path) -> bool:
    return check_pkg_config(path, "thrift", version)
