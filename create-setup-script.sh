#!/bin/sh -e
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

THIS_SCRIPT_FILE_MAYBE_RELATIVE="$0"
THIS_SCRIPT_DIR_MAYBE_RELATIVE="${THIS_SCRIPT_FILE_MAYBE_RELATIVE%/*}"
THIS_SCRIPT_DIR_ABSOLUTE=`readlink -f "${THIS_SCRIPT_DIR_MAYBE_RELATIVE}"`

cd "${HOME}"
cp /dev/null setup-open-p4studio.bash
echo "export SDE=\"${THIS_SCRIPT_DIR_ABSOLUTE}\"" >> setup-open-p4studio.bash
echo "export SDE_INSTALL=\"\${SDE}/install\"" >> setup-open-p4studio.bash
echo "export LD_LIBRARY_PATH=\"\${SDE_INSTALL}/lib\"" >> setup-open-p4studio.bash
echo "export PATH=\"\${SDE_INSTALL}/bin:\${PATH}\"" >> setup-open-p4studio.bash

echo "If you use a Bash-like command shell, you may wish to add a line like"
echo "the following to your .bashrc or other shell rc file:"
echo ""
echo "    source \$HOME/setup-open-p4studio.bash"