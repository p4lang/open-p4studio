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
import tempfile
from pathlib import Path
from typing import List, Optional

from dependencies.source.source_dependency_config import SourceDependencyConfig
from dependencies.source.source_dependency_installer import install_source_dependency
from utils.collections import partition
from utils.download import download
from utils.importlib_utils import invalidate_python_modules_cache
from utils.install_dir_info import InstallDirCategory, install_dir_info_manager
from utils.pip_utils import pip_install, python_packages_target_path, check_if_pip_packages_installed, pip_download
from utils.processes import command_output, execute
from utils.subprocess_builder import subprocess_builder, SubprocessBuilder
from utils.terminal import print_normal, compact_log

SOURCE_DEPENDENCIES_MANIFEST_CATEGORY = InstallDirCategory.DEPENDENCIES


class DependencyInstaller:
    DEPENDENCIES_OF_DEPENDENCIES = {
        'grpc': ['boost'],
        'thrift': ['boost'],
    }
    def __init__(
            self,
            os_name: str,
            os_version: str,
            jobs: int,
            os_package_manager: str,
            install_dir: Path,
            download_cache_dir: Optional[Path],
            working_dir: Optional[Path],
            force: bool
    ):
        self.os_name = os_name
        self.os_version = os_version
        self.jobs = jobs
        self.os_package_manager = os_package_manager
        self.install_dir = install_dir
        if working_dir:
            self.common_working_dir = working_dir
            self.common_working_dir.mkdir(parents=True, exist_ok=True)
        else:
            self.common_working_dir = Path(tempfile.mkdtemp(prefix="dependencies-"))
        self.download_cache_dir = download_cache_dir or self.common_working_dir / 'download'
        self.force = force

    def remove_temporary_files(self) -> None:
        execute("rm -rf {}".format(self.common_working_dir))

    def update_list_of_packages(self) -> None:
        name = "updating list of packages"
        if self.os_name == 'CentOS':
            self.command(name) \
                .args('yum', 'install', '-y', 'dnf-plugins-core') \
                .run_or_raise()
            if self.os_version == '7':
                self.command(name) \
                    .args('yum-config-manager', '--enable', 'PowerTools') \
                    .run_or_raise()
            elif self.os_version == '8':
                available_repos = command_output(['yum', 'repolist', 'all']).decode("utf-8")
                power_tools_repo_name = 'powertools' if 'powertools' in available_repos else 'PowerTools'
                self.command(name) \
                    .args('yum', 'config-manager', '--set-enabled', power_tools_repo_name) \
                    .run_or_raise()
            else:
                self.command(name) \
                    .args('yum', 'config-manager', '--set-enabled', 'crb') \
                    .run_or_raise()
        else:
            self.command(name) \
                .args(self.os_package_manager, 'update') \
                .run_or_raise()

    def install_os_dependencies(self, deps: List[str]) -> None:
        urls, names = partition(deps, lambda x: x.startswith("https://") or x.startswith("http://"))
        if self.os_name in ['Ubuntu', 'Debian'] and urls:
            compact_log().start_new("- downloading deb packages: ")
            files = []
            for url in urls:

                try:
                    files.append(download(url))
                except Exception as e:
                    compact_log().done(False)
                    raise e

            deps = names + files
            compact_log().done(True)

        additional_options = ['--no-install-recommends'] if self.os_name in ['Ubuntu', 'Debian'] else []
        self.command("installing OS dependencies") \
            .args(self.os_package_manager, 'install', '-y', *additional_options, *deps) \
            .run_or_raise()

        # if we install packages like python3-distutils, python modules caches needs to be invalidated
        invalidate_python_modules_cache()

    def install_pip3_dependencies(self, deps: List[str]) -> None:
        compact_log().start_new("  - {}: ".format("installing pip3 dependencies"))
        try:
            if self.force or not check_if_pip_packages_installed(self.install_dir, self.os_name, deps):
                working_dir = self.common_working_dir / 'pip'
                download_dir = self.download_cache_dir / 'pip'

                pip_dir = working_dir / 'upgraded-pip'
                pip_packages = [
                    "pip<21.0; python_version < '3.12'",
                    "pip>21.0; python_version >= '3.12'",
                    # it could be removed once support for python 3.5 will be dropped
                    "setuptools==44.1.1; python_version < '3.10'",
                    "setuptools==62.3.3; python_version >= '3.10' and python_version <'3.12'",
                    "setuptools==75.6.0; python_version >= '3.12'",
                    'wheel>=0.29,<=0.37.1',
                    "pipenv==2024.4.0; python_version >= '3.12'",
                ]
                pip_install(pip_dir, self.os_name, pip_packages, download_dir)

                packages_dir = working_dir / 'packages'
                pythonpath = python_packages_target_path(pip_dir, self.os_name).as_posix()

                if self.install_dir == Path('/usr/local'):
                    pip_install(self.install_dir, self.os_name, deps, download_dir, pythonpath=pythonpath)
                else:
                    # as pip may remove files from target dir
                    # we install dependencies to tmp_dir first and then copy them to install_dir
                    pip_install(packages_dir, self.os_name, deps, download_dir, pythonpath=pythonpath)
                    execute("cp -a {}/. {}".format(packages_dir, self.install_dir))
            compact_log().done(True)
        except Exception as e:
            compact_log().done(False)
            raise e

    def install_source_dependencies_with_manifest_record(self, deps: List[str]) -> None:
        install_dir_info_manager().record_changes(self.install_source_dependencies,
                                                  SOURCE_DEPENDENCIES_MANIFEST_CATEGORY,
                                                  self.install_dir,
                                                  deps)

    def install_source_dependencies(self, deps: List[str]) -> None:
        print_normal("- installing source dependencies:")
        deps = self.resolve_dependencies(deps)
        working_dir = self.common_working_dir / 'source'
        download_dir = self.download_cache_dir / 'source'
        for dependency in deps:
            config = SourceDependencyConfig(
                os_name=self.os_name,
                os_version=self.os_version,
                download_dir=download_dir / dependency,
                build_dir=working_dir / dependency,
                install_dir=self.install_dir,
                jobs=self.jobs,
                with_proto='yes' if 'grpc' in deps else 'no',
                force=self.force
            )
            install_source_dependency(dependency, config)

    def resolve_dependencies(self, dependencies: List[str]) -> List[str]:
        pending = dependencies[::-1]
        result = []
        while pending:
            dependency = pending.pop()
            if dependency in result:
                continue
            missing = [d for d in self._dependencies_of(dependency, dependencies) if d not in result]
            if missing:
                pending.append(dependency)
                pending.extend(missing)
            else:
                result.append(dependency)
        return result

    def _dependencies_of(self, dependency: str, all_dependencies: List[str]) -> List[str]:
        result = self.DEPENDENCIES_OF_DEPENDENCIES.get(dependency, [])

        # in case when installing both PI and GRPC, GRPC should be installed first -
        # it will allow to build PI with "--with-proto" option
        if dependency == 'pi' and 'grpc' in all_dependencies:
            result.append('grpc')

        return result

    @staticmethod
    def command(name: str, home: bool = False) -> SubprocessBuilder:
        return subprocess_builder(name).sudo(home=home)
