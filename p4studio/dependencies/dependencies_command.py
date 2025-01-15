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
from collections import OrderedDict
from pathlib import Path
from typing import List, Tuple, Dict, cast, Optional

import click
from click import Context, ParamType, BadParameter

from dependencies import Dependency
from dependencies.dependency_installer import DependencyInstaller
from dependencies.dependency_manager import dependency_manager, DependencyMatcher
from utils.exceptions import ApplicationException
from utils.format_output import user_provided_format
from utils.log import logging_options, default_log_file_name
from utils.os_info import os_info
from utils.processes import run_subprocess
from utils.terminal import print_green, print_normal, print_separator
from workspace import current_workspace, in_workspace


@click.group('dependencies', short_help="Install SDE third-party dependencies")
def dependencies_command() -> None:
    """
    Install SDE third-party dependencies

    \b
    To install all third-party dependencies, run:
      p4studio dependencies install

    """


SUPPORTED_DEPENDENCY_TYPES = ['os', 'pip', 'source']


def _nice_name_of_dependency_type(type: str) -> str:
    return {
        'os': 'OS',
        'pip': 'pip',
        'source': 'Source'
    }[type]


def _split_types(ctx: Context, param: object, types_value: Optional[str]) -> Optional[List[str]]:
    # split columns by ',' and remove whitespace
    if types_value is None:
        return None
    types = types_value.split(',') if types_value else []
    types = ['pip' if t == 'pip3' else t for t in types]

    for type in types:
        if type not in SUPPORTED_DEPENDENCY_TYPES:
            raise click.BadOptionUsage("types", "Invalid dependency type: {}".format(type))

    return types


class DependencyMatcherParamType(ParamType):
    def convert(self, value: str, param: object, ctx: object) -> DependencyMatcher:
        try:
            return DependencyMatcher(value)
        except ApplicationException as e:
            raise BadParameter(str(e), ctx=ctx, param=param)


@click.command('list')
@click.option("--os-name", default=os_info.name, help="OS Name (Ubuntu, CentOS)")
@click.option("--os-version", default=os_info.version, help="OS Version (20.04, 9)")
@click.option("--format", 'line_format',
              help="List dependencies using specific format")
@click.option("--types",
              default=None,
              metavar="TYPE1,TYPE2,...",
              help="Comma separated list containing types of dependencies that should be listed: os, pip3, source",
              callback=_split_types
              )
@click.argument('matchers', metavar='[DEPENDENCIES]...', nargs=-1, type=DependencyMatcherParamType())
def list_dependencies_command(
        matchers: Tuple[DependencyMatcher],
        line_format: str,
        os_name: str,
        os_version: str,
        types: List[str],
) -> None:
    """List dependencies required to build and run SDE

    DEPENDENCIES = ATTR:VALUE|^ATTR:VALUE

    ATTR is a name of dependency attribute. It can be one of: name, type, tag
    if ATTR: is missing it matches both name and tag

    \b
    VALUE is a value of dependency attribute.
    \b

    ^ means that matched should be excluded

    \b
    Examples:
        - p4studio dependencies list tag:grpc ^type:pip
            list all dependencies tagged as grpc except python packages
        - p4studio dependencies list ^grpc
            list all default dependencies except tagged or named as grpc
    """

    dependencies = dependencies_grouped_and_ordered_by_type(
        os_name,
        os_version,
        list(matchers),
        types,
        None
    )

    if line_format:
        for type, d in dependencies.items():
            for dependency in d:
                name = dependency.name
                tags = '[' + ', '.join(dependency.tags) + ']'
                line = user_provided_format(line_format, name=name, type=type, tags=tags)
                print_normal(line)
    else:
        for type in dependencies:
            if dependencies[type]:
                names = map(lambda it: it.name, dependencies[type])
                sorted_names = sorted(names, key=lambda it: it.lower())
                print_green("{} dependencies:", _nice_name_of_dependency_type(type))
                print_normal(' '.join(sorted_names))
                print_separator()


def describe_source_packages_option() -> str:
    result = "Comma separated list of source packages that should be built"
    if in_workspace():
        source_packages = dependency_manager(os_info.name, os_info.version) \
            .packages('source_packages')
        result += ': ' + ', '.join(source_packages)
    return result


@click.command('install')
@logging_options('INFO', default_log_file_name())
@click.option("--os-name", default=os_info.name, help="OS Name (Ubuntu, CentOS)")
@click.option("--os-version", default=os_info.version, help="OS Version (20.04, 10)")
@click.option("--jobs", default=os.cpu_count(), help="Allow specific number of jobs used to build dependencies")
@click.option("--install-dir", default=None, metavar="DIR", help="Install Python/source packages in specific location",
              type=click.Path(file_okay=False, writable=True, resolve_path=True))
@click.option("--download-cache-dir", default=None, metavar="DIR", help="Cache downloaded files in specific location",
              type=click.Path(file_okay=False, writable=True, resolve_path=True))
@click.option("--source-packages", default=None, metavar="PKG1,PKG2,...",
              help=describe_source_packages_option())
@click.option("--types",
              default=None,
              metavar="TYPE1,TYPE2,...",
              help="Comma separated list containing types of dependencies that should be installed: os, pip3, source",
              callback=_split_types
              )
@click.option("--force", is_flag=True, default=False, help="Reinstall source dependencies even if present")
@click.argument('matchers', metavar='[DEPENDENCIES]...', nargs=-1, type=DependencyMatcherParamType())
def install_command(
        os_name: str,
        os_version: str,
        matchers: Tuple[DependencyMatcher],
        types: List[str],
        source_packages: Optional[str],
        install_dir: Optional[str],
        download_cache_dir: Optional[str],
        jobs: int,
        force: bool
) -> None:
    """
    Install dependencies required to build and run SDE

    DEPENDENCIES = ATTR:VALUE|^ATTR:VALUE

    ATTR is a name of dependency attribute. It can be one of: name, type, tag
    if ATTR: is missing it matches both name and tag

    \b
    VALUE is a value of dependency attribute.
    \b

    ^ means that matched should be excluded

    \b
    Examples:
        - p4studio dependencies install tag:grpc ^type:pip
            install all dependencies tagged as grpc except python packages
        - p4studio dependencies install ^grpc
            install all default dependencies except tagged or named as grpc
    """
    print_green("Installing {} dependencies...", current_workspace().name)
    install_dir_path = Path(install_dir) if install_dir else current_workspace().default_install_dir
    download_cache_path = Path(download_cache_dir) if download_cache_dir else None

    manager = dependency_manager(os_name, os_version)
    installer = DependencyInstaller(
        os_name,
        os_version,
        jobs,
        manager.os_package_manager,
        install_dir_path,
        download_cache_path,
        force)

    if source_packages is None:
        source_packages_list = None
    elif source_packages == "":
        source_packages_list = []
    else:
        source_packages_list = source_packages.split(',')

    dependencies = dependencies_grouped_and_ordered_by_type(
        os_name,
        os_version,
        list(matchers),
        types,
        source_packages_list
    )

    try:
        run_subprocess("sudo true").communicate()

        if 'os' in dependencies:
            installer.update_list_of_packages()
            installer.install_os_dependencies(names(dependencies['os']))
        if 'pip' in dependencies:
            installer.install_pip3_dependencies(names(dependencies['pip']))
        if 'source' in dependencies:
            if install_dir == "/usr/local":
                installer.install_source_dependencies(names(dependencies['source']))
            else:
                installer.install_source_dependencies_with_manifest_record(names(dependencies['source']))

        installer.remove_temporary_files()
    except ApplicationException:
        raise ApplicationException("{} dependencies not installed.".format(current_workspace().name))
    print_green("{} dependencies installed.", current_workspace().name)


def names(dependencies: List[Dependency]) -> List[str]:
    return list(map(lambda it: it.name, dependencies))


def dependencies_grouped_and_ordered_by_type(
        os_name: str,
        os_version: str,
        matchers: List[DependencyMatcher],
        types: List[str],
        source_packages: Optional[List[str]],
) -> Dict[str, List[Dependency]]:
    if (source_packages is not None or types is not None) and matchers:
        raise ApplicationException(
            "--source-packages/--types options are deprecated and cannot be used together with positional arguments"
        )

    if source_packages is not None:
        matchers = [DependencyMatcher("^source")]
        matchers += [DependencyMatcher(it) for it in source_packages]
    if types:
        matchers += [DependencyMatcher('^type:' + it) for it in ['os', 'pip', 'source'] if it not in types]

    manager = dependency_manager(os_name, os_version)
    dependencies = manager.get(matchers)

    result = cast(Dict[str, List[Dependency]], OrderedDict())

    for dependency in dependencies:
        result.setdefault(dependency.type, []).append(dependency)

    return result


dependencies_command.add_command(list_dependencies_command)
dependencies_command.add_command(install_command)
