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
from typing import List, Tuple

import click
import yaml
from click import Context
from inquirer.errors import ValidationError

from build.targets import get_switch_profiles_by_arch, all_switch_profiles
from config.configuration_manager import current_configuration_manager
from interactive.input_validation import validate_path_to_write, validate_kernel_headers_input, validate_file_to_read
from profile.profile import Profile
from profile.profile_command import execute_plan, calculate_jobs_from_available_cpus_and_memory
from profile.profile_execution_plan import ProfileExecutionPlan
from system.check_system_utils import print_multiple_checks
from system.checks import get_initial_checks
from utils.default_directory_file import is_only_filename
from utils.filesystem_utils import ensure_path_is_absolute
from utils.inquirer_utils import list_confirm, list_input, checkbox, path
from utils.log import default_log_file_name, logging_options
from utils.tuple_utils import to_tuple_by_repetition_twice
from workspace import current_workspace

ADVANCED_OPTIONS = ["tdi", "sai"]

CHIP_TO_HW_OPTIONS_DICT = {"tofino2": "newport"}


@click.command('interactive')
@logging_options('INFO', default_log_file_name())
@click.pass_context
def interactive_command(context: Context) -> None:
    """
        Run SDE installation in interactive mode
    """
    profile = Profile(current_configuration_manager())
    initial_checks_ok = run_initial_checks()
    if not initial_checks_ok and not list_confirm(
            'Some checks have failed, so the installation may fail. Do you want to continue?', default=False):
        exit(1)
    click.echo("""Default settings allow to run P4-16 examples for all tofino chip types on ASIC model.""")
    if list_confirm(message="Do you want to install SDE using default settings (suitable for beginners)?",
                    default=True):
        profile = create_default_profile()
        build_sde(context, profile)
        return

    if not list_confirm(message="Do you want to install missing third-party dependencies?",
                        default=True):
        profile.skip_dependencies()
    deployment_targets_choices = [to_tuple_by_repetition_twice(x) for x in
                                  ["Hardware", "ASIC Model"]]
    deployment_target = list_input("Please select deployment target",
                                   choices=deployment_targets_choices)

    build_kernel_modules = None
    if deployment_target == "Hardware":
        profile.enable('asic')
        chip_type = list_input("Please select platform", choices=[("montara/mavericks (tofino)", "tofino"),
                                                                  ("newport (tofino2)", "tofino2")])
        platform_option = CHIP_TO_HW_OPTIONS_DICT.get(chip_type)
        if platform_option:
            profile.enable(platform_option)

        workspace = current_workspace()
        profile.bsp_path = ensure_path_is_absolute(path("Please provide the path to BSP",
                                                        default=workspace.bsp_path(),
                                                        validate=validate_file_to_read))
    else:
        supported_architectures_choices = [to_tuple_by_repetition_twice(x) for x in supported_architectures()]
        chip_type = list_input("Please select tofino chip type", choices=supported_architectures_choices)
        build_kernel_modules = list_confirm("Do you want to build kernel modules(bf_kdrv, bf_kpkt, bf_knet)?",
                                            default=False)

        if build_kernel_modules:
            profile.enable("kernel-modules")

    if build_kernel_modules or deployment_target == "Hardware":
        kdir = path("Please provide path to kernel headers directory (or leave blank to autodetect)",
                    validate=validate_kernel_headers_input)
        if kdir:
            profile.kdir = ensure_path_is_absolute(kdir)

    profile.enable(chip_type)
    available_switch_profiles = [("switch-p4-16: " + profile, profile) for profile in
                                 get_switch_profiles_by_arch(chip_type)]
    available_p4_examples = [("P4 examples: P4-16", "p4-16-programs"),
                             ("P4 examples: P4-14", "p4-14-programs")]
    components_to_build = checkbox("Please select components(p4 programs + corresponding control plane code) to build",
                                   choices=available_p4_examples + available_switch_profiles +
                                   [("bf-diags", "bf-diags")],
                                   default=["p4-16-programs"], validate=switch_profiles_constraint)

    for component in components_to_build:
        if component in [x[1] for x in available_p4_examples]:
            profile.add_p4_program(component)
        elif component in [x[1] for x in available_switch_profiles]:
            profile.enable("switch")
            profile.switch_profile = component
        else:
            profile.enable(component)

    if list_confirm("Do you want to configure advanced options?", default=False):
        choices, defaults = generate_advanced_options_choices_with_defaults()
        choices = [choice for choice in choices if profile.is_option_modifiable(choice[1])]
        p4studio_options = [choice[1] for choice in choices]
        advanced_options_config = checkbox("Advanced options configuration", choices=choices,
                                           default=defaults)
        turned_on_options = [option for option in p4studio_options if option in advanced_options_config]
        turned_off_options = [option for option in p4studio_options if option not in turned_on_options]
        for option in turned_on_options:
            profile.enable(option)
        for option in turned_off_options:
            profile.disable(option)

    click.echo("Based on your selections the following SDE configuration profile was created: \n\n{}".format(
        yaml.dump(profile.raw))
    )

    if list_confirm('Do you want to write it to a file for future use?', default=False):
        filename_or_path = path('Please provide the profile name - [Example: my-profile]',
                                validate=validate_path_to_write)
        path_to_write = filename_or_path
        if is_only_filename(filename_or_path):
            path_to_write = (current_workspace().p4studio_profiles_dir / (filename_or_path + ".yaml")).as_posix()
        else:
            path_to_write = ensure_path_is_absolute(path_to_write)
        with open(path_to_write, "w") as file:
            file.write(yaml.dump(profile.raw))
        echo_separator()
        click.echo('Profile saved in {}'.format(path_to_write))
        print_profile_command(filename_or_path)
    if list_confirm('Do you want to continue building SDE?', default=True):
        build_sde(context, profile)


def echo_separator() -> None:
    click.echo()


def create_default_profile() -> Profile:
    profile = Profile(current_configuration_manager())
    profile.add_p4_program("p4-16-programs")
    for arch in supported_architectures():
        profile.enable(arch)
    return profile


def build_sde(context: Context, profile: Profile) -> None:
    jobs = calculate_jobs_from_available_cpus_and_memory()
    plan = ProfileExecutionPlan(profile, None, jobs)
    execute_plan(context, plan)


def print_profile_command(file_path: str) -> None:
    click.echo("You can use it with command:\n")
    click.secho("p4studio profile apply {}\n".format(file_path), bold=True, fg="green")


def generate_advanced_options_choices_with_defaults() -> Tuple[List[Tuple[str, str]], List[str]]:
    config_manager = current_configuration_manager()
    advanced_options_definitions = [config_manager.definition(option) for option in ADVANCED_OPTIONS]
    defaults = [definition.p4studio_name for definition in advanced_options_definitions if definition.default]
    choices = [("{} - {}".format(definition.p4studio_name, definition.description), definition.p4studio_name) for
               definition in advanced_options_definitions]
    return choices, defaults


def switch_profiles_constraint(_: object, selected: List[str]) -> bool:
    if len([x for x in selected if x in all_switch_profiles()]) > 1:
        raise ValidationError("", reason="Cannot build more than one switch-p4-16 profile.")
    return True


def run_initial_checks() -> bool:
    checks = get_initial_checks()
    print_multiple_checks(checks)
    echo_separator()
    return all([check.ok for check in checks])


def supported_architectures() -> List[str]:
    return [d.p4studio_name for d in
            current_configuration_manager().definitions_by_category("Architecture")]
