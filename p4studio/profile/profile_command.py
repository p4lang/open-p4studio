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
import sys
from typing import Optional, List, Sequence

import click
import yaml
from click import Choice
from click import Context
from click.utils import LazyFile
from yaml.representer import SafeRepresenter

from build import build_command
from config.config_option_utils import sorted_by_parenthood
from config.configuration_manager import current_configuration_manager, p4studio_arg_to_config_option
from config.configure_command import _allowed_options, configure_command
from dependencies.dependencies_command import install_command
from system.check_system_command import check_system_command
from utils.default_directory_file import DefaultDirectoryFile
from utils.log import logging_options, default_log_file_name
from utils.terminal import print_green, print_separator
from workspace import current_workspace
from .profile import Profile, load_profile_from_file
from .profile_execution_plan import ProfileExecutionPlan


@click.group("profile", short_help="Build and install SDE using profiles")
def profile_command() -> None:
    """Build and install SDE using profiles

    \b
    If you want to build and install SDE using profile profiles/switch-p4-16.yaml, run:
      p4studio profile apply profiles/switch-p4-16.yaml
    or:
      p4studio profile apply switch-p4-16

    """


def _default_options() -> str:
    options = {
        o.p4studio_name: o.default
        for o in current_configuration_manager().definitions
    }
    options['switch'] = True
    options['bf-diags'] = True

    return ','.join([('' if v else '^') + o for o, v in options.items()])


@click.command('create')
@click.argument("file", type=click.File('w'))
@click.option('--configure', 'options', default=None,
              help="Configure profile with comma separated list of options"
              )
@click.option('--switch-profile', help="Switch profile")
@click.option('--p4-examples', default="tna_exact_match", help="Comma separated list of P4 programs")
@click.option("--bsp-path", type=click.Path(exists=True), help="BSP to be used and installed")
def profile_create_command(file: LazyFile,
                           options: str,
                           switch_profile: str,
                           p4_examples: str,
                           bsp_path: Optional[str]
                           ) -> None:
    """
    Create new profile
    """
    config_manager = current_configuration_manager()
    profile = Profile(config_manager)

    if bsp_path:
        profile.bsp_path = str(bsp_path)

    if options is None:
        options = _default_options()

    config_options = config_manager.convert_to_config_options(options.split(','))
    for opt in sorted_by_parenthood(config_options):
        profile.set_option(opt.p4studio_name, opt.enabled)

    if switch_profile:
        profile.switch_profile = switch_profile

    if p4_examples:
        for program in p4_examples.split(','):
            profile.add_p4_program(program)

    file.write(yaml.dump(profile.raw))


@click.command('describe')
@click.argument("file",
                type=DefaultDirectoryFile(default_directory=str(current_workspace().p4studio_profiles_dir),
                                          accepted_extensions=[".yaml", ".yml"], logging_name="profile", mode='r'))
@click.option("--bsp-path", type=click.Path(exists=True), help="BSP to be used and installed")
def profile_describe_command(file: LazyFile, bsp_path: Optional[str]) -> None:
    """Describe existing profile"""
    plan = create_plan(file, bsp_path)

    plan.describe_profile()
    plan.show_commands()


def profile_file_autocompletion(ctx: Context, args: List[str], incomplete: str) -> List[str]:
    files_in_profile_dir = current_workspace().p4studio_profiles_dir.iterdir()
    yaml_file_names = [f.stem for f in files_in_profile_dir if f.suffix == ".yaml" or f.suffix == ".yml"]
    return [fn for fn in yaml_file_names if incomplete in fn]


# Example first line of /proc/meminfo file on Linux:
# MemTotal:        8108300 kB
def available_mem_MBytes() -> int:
    firstline = None
    with open('/proc/meminfo', 'r') as f:
        firstline = f.readline()
    mem_KBytes = int(firstline.split()[1])
    mem_MBytes = mem_KBytes // 1024
    return mem_MBytes


# When p4c is built with the unity option, at least one individual
# process uses 4.5 GBytes of RAM, and there are others that use
# 2-3 GBytes of RAM that might run in parallel with that one.
#
# To be safe, overestimate slightly using this formula for
# p4c unity builds, where N is the number of parallel jobs:
#
# expected_max_mem_usage(N) = 2 GBytes + N * (4 GBytes)
#
# If later we decide to disable unity build for p4c, I suspect
# we could use this formula instead, but I have not tested this:
#
# expected_max_mem_usage(N) = 2 GBytes + N * (2 GBytes)

def expected_max_mem_usage_MBytes(num_jobs) -> int:
    return 2048 + num_jobs * 4096


def max_parallel_jobs(avail_mem_MBytes) -> int:
    cpu_count = os.cpu_count()
    num_jobs = 0
    while True:
        required_mem_MBytes = expected_max_mem_usage_MBytes(num_jobs + 1)
        if required_mem_MBytes > avail_mem_MBytes:
            break
        num_jobs += 1
    #print("Available memory (MBytes): %s" % (avail_mem_MBytes))
    #print("Max number of parallel jobs for available mem: %s" % (num_jobs))
    #print("Max number of parallel jobs for processors: %s" % (cpu_count))
    if num_jobs > cpu_count:
        num_jobs = cpu_count
    return num_jobs


def calculate_jobs_from_available_cpus_and_memory() -> int:
    cpu_count = os.cpu_count()
    avail_mem_MBytes = available_mem_MBytes()
    mem_comment = ""
    num_jobs = max_parallel_jobs(avail_mem_MBytes)
    abort = False
    if num_jobs < 1:
        mem_comment = "too low"
        abort = True
    else:
        mem_comment = "enough for %s parallel jobs" % (num_jobs)
    print("Minimum recommended memory to run this script: %s MBytes"
          "" % (expected_max_mem_usage_MBytes(1)))
    print("Memory on this system from /proc/meminfo:      %s MBytes -> %s"
          "" % (avail_mem_MBytes, mem_comment))
    if abort:
        print("Aborting because system has too little RAM.", file=sys.stderr)
    return num_jobs


@click.command('apply')
@click.argument("file",
                type=DefaultDirectoryFile(default_directory=str(current_workspace().p4studio_profiles_dir),
                                          accepted_extensions=[".yaml", ".yml"], logging_name="profile", mode='r',
                                          ), autocompletion=profile_file_autocompletion)
@logging_options('INFO', default_log_file_name())
@click.option("--override-option", "override_options", type=Choice(_allowed_options()), multiple=True,
              metavar="[CONFIG|^CONFIG]", help="Override any option in a profile")
@click.option("--jobs", default=None, help="Allow specific number of jobs to be used")
@click.option("--bsp-path", type=click.Path(exists=True), help="BSP to be used and installed")
@click.option('--skip-dependencies', default=False, is_flag=True, help="Do not install dependencies")
@click.option('--skip-system-check', default=False, is_flag=True, help="Do not check system")
@click.pass_context
def profile_apply_command(context: Context,
                          file: LazyFile,
                          jobs: Optional[int],
                          bsp_path: Optional[str],
                          skip_dependencies: bool,
                          skip_system_check: bool,
                          override_options: List[str]
                          ) -> None:
    """
    Build and install SDE using existing profile
    """
    if jobs is None:
        jobs = calculate_jobs_from_available_cpus_and_memory()
    plan = create_plan(file, bsp_path, jobs, override_options)

    if not skip_system_check:
        check_system(context, plan)

    execute_plan(context, plan, skip_dependencies)


def check_system(context: Context, plan: ProfileExecutionPlan) -> None:
    asic = 'asic' in plan.profile.config_args()
    kdir = plan.profile.kdir
    context.invoke(check_system_command, asic=asic, kdir=kdir)


def execute_plan(context: Context, plan: ProfileExecutionPlan, skip_dependencies: bool = False) -> None:
    print_separator()
    plan.describe_profile()
    print_separator()

    if not skip_dependencies:
        context.invoke(install_command, **plan.dependencies_install_args())
        print_separator()

    context.invoke(configure_command, **plan.configure_args())
    print_separator()
    context.invoke(build_command, **plan.build_args())


def create_plan(file: LazyFile, bsp_path: Optional[str] = None, jobs: Optional[int] = None,
                overriden_options: Sequence[str] = []) -> ProfileExecutionPlan:
    print_green("Loading profile from {} file...", file.name)
    profile = load_profile_from_file(file)

    for option in sorted_by_parenthood([p4studio_arg_to_config_option(o) for o in overriden_options]):
        if option.enabled:
            profile.enable(option.p4studio_name)
        elif option.can_be_disabled():
            profile.disable(option.p4studio_name)

    if bsp_path:
        profile.enable("bsp")

    plan = ProfileExecutionPlan(profile, bsp_path, jobs)
    print_green("Profile is correct.")
    print_separator()
    return plan


profile_command.add_command(profile_create_command)
profile_command.add_command(profile_apply_command)
profile_command.add_command(profile_describe_command)

yaml.add_representer(OrderedDict, lambda self, data: SafeRepresenter.represent_dict(self, data.items()))
