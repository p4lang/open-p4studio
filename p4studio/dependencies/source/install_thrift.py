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

    build_dir = build_dir / "thrift_build"
    os.makedirs(build_dir, exist_ok=True)

    # Set the C++ standard to 17 and other flags
    cmake_flags = (
        f"{attrs['flags']} "
        f"-DCMAKE_CXX_STANDARD=17 "
        f"-DCMAKE_PREFIX_PATH={config.install_dir} "
        f"-DCMAKE_INSTALL_PREFIX={config.install_dir} "
        f"-DCMAKE_INSTALL_RPATH={config.install_dir} "
        "-DBUILD_SHARED_LIBS=ON "
    )

    # Run CMake to configure Thrift
    execute(f"cmake {cmake_flags} ..", build_dir)

    # Build and install Thrift
    execute(f"cmake --build . --target install --config Release -- -j{config.jobs}", build_dir)


def _is_thrift_installed(version: str, path: Path) -> bool:
    return check_pkg_config(path, "thrift", version)
