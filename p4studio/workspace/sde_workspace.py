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
import tarfile
from glob import glob
from pathlib import Path
from typing import List, Dict, Optional

from utils.exceptions import ApplicationException
from utils.filesystem_utils import find_file_in_directories
from workspace.workspace import Workspace


def get_sde_workspace() -> Workspace:
    return SdeWorkspace()


class SdeWorkspace(Workspace):
    def __init__(self) -> None:
        super(SdeWorkspace, self).__init__()
        self._profiles_yaml = ""

    @property
    def name(self) -> str:
        return 'SDE'

    @property
    def p4_dirs(self) -> Dict[str, Path]:
        return {
            'p4-14-programs': self.submodules_path / 'p4-examples/programs',
            'p4-16-programs': self.submodules_path / 'p4-examples/p4_16_programs'
        }

    @property
    def submodules_path(self) -> Path:
        return self.root_path / 'pkgsrc'

    @property
    def compressed_packages_path(self) -> Path:
        return self.root_path / 'packages'

    def _required_files(self) -> List[str]:
        return [
            'p4studio/dependencies/dependencies.yaml',
            'CMakeLists.txt'
        ]

    @property
    def is_package_extraction_required(self) -> bool:
        return True

    def bsp_path(self) -> Optional[Path]:
        version = self._sde_version()
        if version is None:
            return None
        return find_file_in_directories("bf-reference-bsp-" + version + ".tgz", self.possible_bsp_location)

    def _sde_version(self) -> Optional[str]:
        manifest_possible_file_paths = glob(str(self.root_path / "bf-sde-*.manifest"))
        if len(manifest_possible_file_paths) == 0:
            return None
        manifest_filename = Path(manifest_possible_file_paths[0]).name
        regex = r"bf-sde-(?P<version>\d+\.\d+\.\d+(?:\.\d+)*(?:-cpr|-pr)?).manifest"
        match = re.match(regex, manifest_filename)
        return match.group("version") if match else None

    @property
    def dependency_files(self) -> List[Path]:
        return [
            self.p4studio_path / 'dependencies/dependencies.yaml',
            self.p4studio_path / 'dependencies/p4i.dependencies.yaml'
        ]

    @property
    def switch_profiles_yaml(self) -> str:
        if self._profiles_yaml:
            return self._profiles_yaml

        possible_profile_yaml = self._try_read_switch_profiles_yaml()
        if possible_profile_yaml:
            self._profiles_yaml = possible_profile_yaml
            return self._profiles_yaml

        self._profiles_yaml = self.read_profile_yaml_from_tgz()
        return self._profiles_yaml

    def read_profile_yaml_from_tgz(self) -> str:
        path_to_switch_tgz = find_file_in_directories(r"switch-p4-16-.*\.tgz", [self.compressed_packages_path])
        if not path_to_switch_tgz:
            raise ApplicationException(
                "Cannot find switch-p4-16 source archive in {}".format(self.compressed_packages_path))
        with tarfile.open(path_to_switch_tgz.as_posix(), "r:gz") as tar_content:
            extracted_file = tar_content.extractfile(path_to_switch_tgz.stem + "/profiles.yaml")
            return extracted_file.read().decode()  # type: ignore

    @property
    def switch_submodule_path(self) -> Path:
        return self.submodules_path / 'switch-p4-16'
