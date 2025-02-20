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

from dependencies.source.install_boost import install_boost
from dependencies.source.install_bridge_utils import install_bridge_utils
from dependencies.source.install_grpc import install_grpc
from dependencies.source.install_libcli import install_libcli
from dependencies.source.install_libcrafter import install_libcrafter
from dependencies.source.install_pi import install_pi
from dependencies.source.install_thrift import install_thrift
from dependencies.source.source_dependency_config import SourceDependencyConfig
from utils.exceptions import ApplicationException
from utils.terminal import compact_log

_DEP_TO_FUNC = {
    "libcli": install_libcli,
    "libcrafter": install_libcrafter,
    "pi": install_pi,
    "boost": install_boost,
    "grpc": install_grpc,
    "thrift": install_thrift,
    "bridge": install_bridge_utils
}


def install_source_dependency(name: str, config: SourceDependencyConfig) -> None:
    global _DEP_TO_FUNC
    func = _DEP_TO_FUNC.get(name)
    if not func:
        raise ApplicationException("Unknown dependency: {}".format(name))

    compact_log().start_new("  - {}: ".format(name))
    try:
        func(config)
        compact_log().done(True)
    except Exception as e:
        compact_log().done(False)
        raise e
