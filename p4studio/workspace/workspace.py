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
from abc import abstractmethod
from pathlib import Path
from typing import List, Dict, Generator, Optional

from external_modules import add_third_party_deps
from utils.p4studio_path import p4studio_path


class Workspace:
    """
    Encapsulates knowledge about specific workspace like SDE package.
    It provides paths to well-known files that are important for p4studio.
    """

    def __init__(self) -> None:
        self._root_path = None
        for candidate in self._root_path_candidates():
            if self.check_if_root_path(candidate):
                self._root_path = candidate
                return

    @property
    @abstractmethod
    def name(self) -> str:
        pass

    @property
    def is_valid(self) -> bool:
        return self._root_path is not None

    @property
    def root_path(self) -> Path:
        if self._root_path is None:
            raise Exception("Not in {} directory".format(self.name))
        return self._root_path

    def check_if_root_path(self, path: Path) -> bool:
        return path.is_dir() and \
            all((path / f).is_file() for f in self._required_files())

    @abstractmethod
    def _required_files(self) -> List[str]:
        pass

    def ensure_p4studio_dependencies(self) -> None:
        add_third_party_deps()

    @property
    def p4studio_path(self) -> Path:
        return p4studio_path()

    @property
    def build_path(self) -> Path:
        return self.root_path / 'build'

    @property
    def cmake_lists_txt(self) -> Path:
        return self.root_path / 'CMakeLists.txt'

    @property
    @abstractmethod
    def submodules_path(self) -> Path:
        pass

    @property
    def default_install_dir(self) -> Path:
        return self.root_path / 'install'

    @abstractmethod
    def bsp_path(self) -> Optional[Path]:
        pass

    def package_installation_script(self, name: str) -> Path:
        if name in ['boost', 'grpc', 'libcli', 'libcrafter', 'pi', 'thrift']:
            return self.p4studio_path / 'dependencies/source/install_{}.py'.format(name)
        elif name == 'bridge':
            return self.p4studio_path / 'dependencies/source/install_bridge_utils.py'
        else:
            raise Exception('Package {} is not supported'.format(name))

    @property
    def dependency_files(self) -> List[Path]:
        return [self.p4studio_path / 'dependencies/dependencies.yaml']

    @property
    @abstractmethod
    def p4_dirs(self) -> Dict[str, Path]:
        pass

    @property
    def p4studio_profiles_dir(self) -> Path:
        return self.p4studio_path / 'profiles'

    @staticmethod
    def _root_path_candidates() -> Generator[Path, None, None]:
        if os.environ.get("P4STUDIO_CONTEXTUAL_WORKSPACE", "") == "1":
            path = Path(os.getcwd())
        else:
            path = p4studio_path()
        yield path
        while path.parent != path:
            path = path.parent
            yield path

    @property
    @abstractmethod
    def is_package_extraction_required(self) -> bool:
        pass

    @property
    @abstractmethod
    def compressed_packages_path(self) -> Path:
        pass

    @property
    def possible_bsp_location(self) -> List[Path]:
        return [self.root_path, self.root_path.parent, Path.home()]

    def _try_read_switch_profiles_yaml(self) -> Optional[str]:
        if self.profiles_yaml_exists():
            with open((self.switch_submodule_path / "profiles.yaml").as_posix(), "r") as profiles_yaml:
                return profiles_yaml.read()
        return None

    @property
    @abstractmethod
    def switch_profiles_yaml(self) -> str:
        pass

    def profiles_yaml_exists(self) -> bool:
        return (self.switch_submodule_path / "profiles.yaml").exists()

    @property
    @abstractmethod
    def switch_submodule_path(self) -> Path:
        pass

    def p4studio_config(self) -> Path:
        return self.p4studio_path / "additional-options.yaml"
