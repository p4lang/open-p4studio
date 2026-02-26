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
from collections import OrderedDict
from pathlib import Path
from typing import List, Any, Dict, cast, Set, Optional

from dependencies.merge import merge_files
from utils.collections import nested_get
from utils.exceptions import ApplicationException
from utils.graphs import reverse_topological_sort
from utils.ordered_set import OrderedSet
from utils.os_info import os_info
from workspace import current_workspace

ALL_DEPENDENCY_GROUPS = [
    'minimal',
    'optional_packages',
    'source_packages',
    'bf_diags',
    'bf_platforms',
    'thrift',
    'grpc',
    'switch',
    'pi',
    'switch_p4_16',
    'p4i',
]

ALL_SOURCE_PACKAGES = [
    'boost',
    'grpc',
    'thrift',
    'libcli',
    'pi',
]


class Dependency:
    def __init__(
            self,
            name: str,
            type: str,
            systems: Set[str],
            tags: Set[str],
            dependencies: List[str],
            attributes: Dict[str, object]):
        self.name = name
        self.type = type
        self.tags = tags
        self.systems = systems
        self.dependencies = dependencies
        self.attributes = attributes

    def __repr__(self) -> str:
        return self.name


class DependencyMatcher:
    pattern = re.compile("^\\^?([a-zA-Z-0-9]+:)?[a-zA-Z-0-9<>=.]+$")
    supported_attributes = ['tag', 'name', 'type', None]

    def __init__(self, value: str):
        self._str = value
        if not self.pattern.match(value):
            raise ApplicationException("incorrect format of {}".format(value))
        if value[0] == '^':
            self.inclusion = False
            value = value[1:]
        else:
            self.inclusion = True
        if ':' not in value:
            self.attribute, self.value = [None, value]
        else:
            self.attribute, self.value = value.split(':')

        if self.attribute not in self.supported_attributes:
            raise ApplicationException("unknown attribute '{}'".format(self.attribute))

    def matches(self, dependency: Dependency) -> bool:
        if self.attribute == 'tag':
            return self.value in dependency.tags
        elif self.attribute == 'name':
            return dependency.name == self.value
        elif self.attribute == 'type':
            return dependency.type == self.value
        else:
            return dependency.name == self.value or self.value in dependency.tags

    def __str__(self) -> str:
        return self._str

    def __repr__(self) -> str:
        return self._str


class DependencyManager:
    """
    Reads single or multiple dependency files
    and provides simple method to get a list of packages of given type and group
    that are relevant for specific OS
    """

    def __init__(self, os_name: str, os_version: str, dependencies: List[Dependency]):
        self.os_name = os_name
        self.os_version = os_version
        self.dependencies = [it for it in dependencies if self.current_system in it.systems]

    @property
    def os_package_manager(self) -> str:
        return 'yum' if self.os_name == 'CentOS' else 'apt-get'

    @property
    def current_system(self) -> str:
        return "{}:{}".format(self.os_name, self.os_version).lower()

    @property
    def package_types(self) -> List[str]:
        return ['os', 'pip', 'source']

    def packages(self, package_type: str) -> List[str]:
        if package_type == 'pip3':
            package_type = 'pip'

        return [d.name for d in self.dependencies if d.type == package_type and self.current_system in d.systems]

    def package(self, name: str) -> Dependency:
        return next(d for d in self.dependencies if d.name == name)

    def source_dependency_attributes(self, name: str) -> Dict[str, Any]:
        return self.package(name).attributes

    def create_matchers(self, query: str) -> List[DependencyMatcher]:
        if query == "":
            matchers = []
        else:
            matchers = [DependencyMatcher(it) for it in query.split(" ")]

        return matchers

    def get_by_query(self, query: str) -> OrderedSet[Dependency]:
        if query == "":
            matchers = []
        else:
            matchers = [DependencyMatcher(it) for it in query.split(" ")]
            if not matchers[0].inclusion:
                matchers.insert(0, DependencyMatcher('tag:default'))

        return self.get(matchers)

    def get(self, matchers: List[DependencyMatcher], type: Optional[str] = None) -> OrderedSet[Dependency]:
        for matcher in matchers:
            if not any(matcher.matches(it) for it in self.dependencies):
                raise ApplicationException("'{}' does not match any dependency".format(matcher))

        if matchers == [] or not matchers[0].inclusion:
            matchers = [DependencyMatcher('tag:default')] + matchers

        result = cast(OrderedSet[Dependency], OrderedSet())
        for dependency in self.dependencies:
            should_be_added = False
            for matcher in matchers:
                if matcher.matches(dependency):
                    should_be_added = matcher.inclusion
            if should_be_added:
                result.add(dependency)

        if type is not None:
            result = OrderedSet(*filter(lambda it: it.type == type, result))

        return self._add_dependencies_of_dependencies(result)

    def _add_dependencies_of_dependencies(self, packages: OrderedSet[Dependency]) -> OrderedSet[Dependency]:
        dependency_graph = OrderedDict([(p.name, p.dependencies) for p in packages])
        return OrderedSet(*[self._get_by_name(it) for it in reverse_topological_sort(dependency_graph)])

    def _get_by_name(self, name: str) -> Dependency:
        for dependency in self.dependencies:
            if dependency.name == name:
                return dependency
        raise Exception("Cannot find definition of '{}'".format(name))


def _is_os_supported(deps: Dict[str, Any], os_name: str, os_version: str) -> bool:
    return "{}:{}".format(os_name, os_version).lower() in cast(List[str],
                                                               nested_get(deps, 'definitions/all_systems', []))


def is_os_supported(os_name: str, os_version: str) -> bool:
    deps = merge_files(current_workspace().dependency_files)
    return _is_os_supported(deps, os_name, os_version)


def dependency_manager(os_name: str, os_version: str) -> DependencyManager:
    os_name = os_info.canonicalize(os_name) or os_info.name
    os_version = os_version or os_info.version
    files = current_workspace().dependency_files
    return dependency_manager_from_files(os_name, os_version, files)


def dependency_manager_from_files(os_name: str, os_version: str, files: List[Path]) -> DependencyManager:
    data = merge_files(files)
    all_systems = cast(List[str], nested_get(data, 'definitions/all_systems', []))

    os_id = "{}:{}".format(os_name, os_version)
    if os_id in all_systems:
        raise ApplicationException("detected OS {} not supported".format(os_id))

    packages = []
    for (name, attrs) in data['packages'].items():
        systems = set(attrs.get('systems', all_systems))
        type = attrs['type']
        tags = attrs.get('tags', [])
        tags += ['all', 'default']
        attributes = attrs.get('attributes', None)
        dependencies = attrs.get('dependencies', [])
        dependency = Dependency(
            name=name,
            type=type,
            systems=systems,
            tags=set(tags),
            dependencies=dependencies,
            attributes=attributes
        )
        packages.append(dependency)
    return DependencyManager(os_name, os_version, packages)
