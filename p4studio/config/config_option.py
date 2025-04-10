#!/usr/bin/env python3
from typing import List, cast, Dict, Union, Optional

from config.cmake import CmakeArg
from utils.yaml_utils import safe_read_yaml
from workspace import current_workspace


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


class ConfigOption:
    """
        Provides abstraction for configurable build option.
        Encapsulates knowledge about differences between naming conventions: cmake vs p4studio
        Example: ( -DSWITCH=off vs ^switch)
        Exposed methods make it explicit if we work with cmake or p4studio names.
        """

    @property
    def _caret(self) -> str:
        return '^' if not self.enabled else ''

    @property
    def p4studio_name(self) -> str:
        return self._p4studio_name

    @property
    def default(self) -> Optional[bool]:
        return self._enabled

    @property
    def enabled(self) -> bool:
        if self._enabled is not None:
            return self._enabled
        else:
            raise IllegalStateError(
                "Option {} is neither enabled or disabled. This property shouldn't be used in this state".format(
                    self.p4studio_name))

    def can_be_disabled(self) -> bool:
        return self._off_args is not None

    @property
    def p4studio_arg(self) -> str:
        return self._caret + self.p4studio_name

    def __init__(self, name: str, on_args: List[CmakeArg], off_args: Optional[List[CmakeArg]] = None,
                 enabled: bool = None):
        self._p4studio_name = name
        self._on_args = on_args
        self._off_args = off_args
        self._enabled = enabled

    def copy_with(self, enabled: bool) -> 'ConfigOption':
        return ConfigOption(self.p4studio_name, self._on_args, self._off_args, enabled)

    @property
    def cmake_args(self) -> List[CmakeArg]:
        if self.enabled:
            return self._on_args
        elif self._off_args:
            return self._off_args
        else:
            return []

    def __eq__(self, other: object) -> bool:
        return isinstance(other, ConfigOption) and \
            self.p4studio_name == other.p4studio_name and \
            self.cmake_args == other.cmake_args and \
            self.enabled == other.enabled

    def __hash__(self) -> int:
        return hash((self.p4studio_name, self.enabled))

    def __repr__(self) -> str:
        return "ConfigOption({},{},{})".format(self.p4studio_name, self.cmake_args, self.enabled)

    def __str__(self) -> str:
        return "ConfigOption({},{},{})".format(self.p4studio_name, self.cmake_args, self.enabled)


class ConfigOptionDefinition:
    def __init__(self, option: ConfigOption, category: str, description: str):
        self._option = option
        self.category = category
        self.description = description

    @property
    def p4studio_name(self) -> str:
        return self._option.p4studio_name

    @property
    def default(self) -> Optional[bool]:
        return self._option.default

    @property
    def can_be_disabled(self) -> bool:
        return self._option.can_be_disabled()

    def option(self, enabled: bool) -> ConfigOption:
        return self._option.copy_with(enabled=enabled)

    def __str__(self) -> str:
        return "ConfigOptionDefinition({},{},{})".format(self._option, self.category, self.description)

    def __repr__(self) -> str:
        return "ConfigOptionDefinition({},{},{})".format(self._option, self.category, self.description)


def user_defined_options_definitions() -> List[ConfigOptionDefinition]:
    result = []
    yaml_content = cast(Dict[str, Dict[str, Union[List[str], str]]],
                        safe_read_yaml(current_workspace().p4studio_config()))
    for name, values in yaml_content.items():
        disabled_args = values.get("disabled")
        enabled_args = values["enabled"]
        on_cmake_args = [CmakeArg.from_str(x) for x in enabled_args]
        off_cmake_args = [CmakeArg.from_str(x) for x in disabled_args] if disabled_args else None

        category = cast(Optional[str], values.get("category", None))

        provided_description = cast(Optional[str], values.get("description", None))
        on_args_description = " ".join(enabled_args)
        off_args_description = " ".join(disabled_args) if disabled_args else None

        default_description = provided_description or "Enabled: {}".format(on_args_description) + (
            "; Disabled: {}".format(off_args_description) if off_args_description else "")

        result.append(
            ConfigOptionDefinition(ConfigOption(name, on_cmake_args, off_cmake_args), category or "Global",
                                   provided_description or default_description))
    return result


class IllegalStateError(ValueError):
    def __init__(self, message: str):
        super().__init__(message)
