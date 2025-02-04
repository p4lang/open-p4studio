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

from pathlib import Path

from dependencies.source.source_dependency_config import SourceDependencyConfig
from utils.processes import execute

_BOOST_FILE = 'boost.tar.bz2'


def download_boost(config: SourceDependencyConfig) -> None:
    boost_attrs = config.dependency_manager().source_dependency_attributes("boost")
    download_dir = config.download_dir(ensure_exists=True)
    boost_file = download_dir / _BOOST_FILE

    if not boost_file.exists():
        execute("wget {} -nv -O {}".format(boost_attrs["url"], boost_file.as_posix()))


def install_boost(config: SourceDependencyConfig) -> None:
    boost_attrs = config.dependency_manager().source_dependency_attributes("boost")

    if not config.force and _is_boost_installed(config.install_dir, boost_attrs['version']):
        return

    download_boost(config)
    boost_file = (config.download_dir(ensure_exists=False) / _BOOST_FILE)
    build_dir = config.build_dir()

    execute("tar xf {} --strip-components 1 -C {}".format(boost_file, build_dir))
    execute("./bootstrap.sh --prefix={} --without-libraries=python".format(config.install_dir), build_dir)
    # Use C++17 here.
    execute("./b2 cxxstd=17 -j{} {}".format(config.jobs, boost_attrs["flags"]), build_dir)
    execute("sudo ldconfig", build_dir)


def _is_boost_installed(path: Path, version: str) -> bool:
    required_boost_files = [
        'include/boost/variant.hpp',
        ('lib/libboost_system.so.%s' % version),
        ('lib/libboost_filesystem.so.%s' % version),
        ('lib/libboost_thread.so.%s' % version)
    ]

    return all((path / f).exists() for f in required_boost_files)
