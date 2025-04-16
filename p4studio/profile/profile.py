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

from collections import OrderedDict
from typing import Union, BinaryIO, TextIO, Optional, Dict, List, Set, cast

import yaml
from jsonschema import validate, ValidationError
from yaml.parser import ParserError

from config.config_option_utils import is_parent_option
from config.configuration_manager import current_configuration_manager, ConfigurationManager, config_option
from dependencies.merge import merge_all
from profile.backward_compatibility import adjust_for_backward_compatibility
from profile.profile_schema import create_profile_schema
from utils.collections import nested_get, nested_set
from utils.exceptions import ApplicationException


def load_profile_from_file(file: Union[bytes, str, BinaryIO, TextIO]) -> 'Profile':
    try:
        yaml_content = yaml.load(file, Loader=yaml.SafeLoader)
    except ParserError as e:
        message = "Profile is not valid YAML. Error at line {}, column {}".format(e.context_mark.line,  # type: ignore
                                                                                  e.context_mark.column)  # type: ignore
        raise ApplicationException(message)

    adjust_for_backward_compatibility(yaml_content)

    return Profile(current_configuration_manager(), yaml_content)


class Profile:
    def __init__(self, configuration_manager: ConfigurationManager, raw: Optional[OrderedDict] = None):
        self._configuration_manager = configuration_manager
        if raw is None:
            self.raw = OrderedDict()
            self.raw['global-options'] = {}
            self.raw['features'] = {}
            self.raw['architectures'] = []
        else:
            self._validate_against_schema(raw)
            self.raw = (raw if raw is not None else {}).copy()

    def skip_dependencies(self) -> None:
        self.raw['dependencies'] = {'source-packages': []}

    def enable(self, name: str) -> None:
        self.set_option(name, True)

    def disable(self, name: str) -> None:
        self.set_option(name, False)

    def is_option_modifiable(self, option: str) -> bool:
        if is_parent_option(option):
            return True
        category = self._configuration_manager.definition(option).category.lower()
        if self._is_option(category):
            parent_value = self._get_field("features/{}".format(category), None)
            if parent_value or parent_value == {}:
                return True
            else:
                return False
        return True

    def set_option(self, name: str, value: object) -> None:
        category = self._configuration_manager.definition(name).category.lower()
        category_path = 'features/{}'.format(category)

        if category == 'global':
            self._set_field('global-options/{}'.format(name), value)
        elif category == 'architecture':
            archs = self.raw.setdefault('architectures', [])
            if not value and name in archs:
                archs.remove(name)
            elif value and name not in archs:
                archs.append(name)
        elif name == category:
            if value:
                if not isinstance(self._get_field(category_path, None), dict):
                    self._set_field(category_path, {})
            else:
                self._set_field(category_path, False)
        else:
            category_value = self._get_field(category_path, False)
            is_category_option_itself = self._is_option(category)
            if not self.is_option_modifiable(name):
                raise ApplicationException("Option {} requires {} to be ON.".format(name, category))

            if (not category_value and not is_category_option_itself) or (
                    is_category_option_itself and category_value == True):  # noqa
                self._set_field(category_path, {})

            self._set_field('features/{}/{}'.format(category, name), value)

    def add_p4_program(self, name: str) -> None:
        self.raw['features'].setdefault('p4-examples', []).append(name)

    def source_packages(self) -> List[str]:
        return cast(List[str], self._get_field('dependencies/source-packages', self._calculate_source_packages()))

    def config_args(self) -> Set[str]:
        return {
            config_option(o, v).p4studio_arg
            for o, v in self.config_options().items()
        }

    def config_options(self) -> Dict[str, bool]:
        return merge_all(
            self.global_options_without_flags(),
            self.features_as_options(),
            self.architecture_options()
        )

    def global_options(self) -> Dict[str, object]:
        return cast(Dict[str, object], self._get_field('global-options', {})).copy()

    def global_options_without_flags(self) -> Dict[str, object]:
        result = self.global_options()
        result.pop('p4ppflags', None)
        result.pop('p4flags', None)
        result.pop('extra-cppflags', None)
        result.pop('kdir', None)
        return result

    def features_as_options(self) -> Dict[str, bool]:
        result = {}
        for feature, options in self.features().items():
            if self._is_option(feature):
                result[feature] = False if options is False else True
            if isinstance(options, Dict):
                for attr in options:
                    # some attributes like 'profile' in 'switch' are not options
                    if self._is_option(attr):
                        result[attr] = options[attr]
        return result

    def _is_option(self, name: str) -> bool:
        return name in self._configuration_manager.known_p4studio_options

    def architecture_options(self) -> Dict[str, bool]:
        return {
            definition.p4studio_name: (definition.p4studio_name in self.architectures())
            for definition in self._configuration_manager.definitions
            if definition.category == 'Architecture'
        }

    @property
    def switch_profile(self) -> Optional[str]:
        if isinstance(self._get_field('features/switch', None), bool):
            return None

        return cast(Optional[str], self._get_field('features/switch/profile', None))

    @switch_profile.setter
    def switch_profile(self, profile_name: str) -> None:
        self.set_option('switch', True)
        self._set_field('features/switch/profile', profile_name)

    @property
    def bsp_path(self) -> Optional[str]:
        return cast(Optional[str], self._get_field('features/bf-platforms/bsp-path', None))

    @bsp_path.setter
    def bsp_path(self, path: str) -> None:
        self._set_field('features/bf-platforms/bsp-path', path)

    @property
    def p4ppflags(self) -> Optional[str]:
        return cast(Optional[str], self._get_field('global-options/p4ppflags', None))

    @p4ppflags.setter
    def p4ppflags(self, value: str) -> None:
        self._set_field('global-options/p4ppflags', value)

    @property
    def p4flags(self) -> Optional[str]:
        return cast(Optional[str], self._get_field('global-options/p4flags', None))

    @p4flags.setter
    def p4flags(self, value: str) -> None:
        self._set_field('global-options/p4flags', value)

    @property
    def extra_cppflags(self) -> Optional[str]:
        return cast(Optional[str], self._get_field('global-options/extra-cppflags', None))

    @extra_cppflags.setter
    def extra_cppflags(self, value: str) -> None:
        self._set_field('global-options/extra-cppflags', value)

    @property
    def kdir(self) -> Optional[str]:
        return cast(Optional[str], self._get_field('global-options/kdir', None))

    @kdir.setter
    def kdir(self, value: str) -> None:
        self._set_field('global-options/kdir', value)

    def architectures(self) -> List[str]:
        return cast(List[str], self._get_field('architectures', []))

    def features(self) -> Dict[str, object]:
        return cast(Dict[str, object], self._get_field('features', {}))

    def build_targets(self) -> List[str]:
        result = cast(List[str], self._get_field('features/p4-examples', [])).copy()
        profile = self.switch_profile
        if profile:
            result.append(profile)
        return result

    def _get_field(self, path: str, default: object) -> object:
        return nested_get(self.raw, path, default)

    def _set_field(self, path: str, value: object) -> None:
        nested_set(self.raw, path, value)

    def _calculate_source_packages(self) -> List[str]:
        result = ['bridge', 'libcli', 'libcrafter']

        if any([
            self.config_options().get('thrift-driver', True),
            self.config_options().get('switch', False) and self.config_options().get('thrift-switch', True),
            self.config_options().get('bf-diags', False) and self.config_options().get('thrift-diags', True),
        ]):
            result.append('thrift')

        if self.config_options().get('grpc', True):
            result.append('grpc')
        if self.config_options().get('pi', False) or self.config_options().get('p4rt', False):
            result.append('pi')

        return result

    def _validate_against_schema(self, yaml_content: Dict[str, object]) -> None:
        try:
            validate(yaml_content, create_profile_schema(self._configuration_manager))
        except ValidationError as e:
            message = "[{}]: {}".format('/'.join(str(s) for s in e.path), e.message)
            raise ApplicationException(message) from e
