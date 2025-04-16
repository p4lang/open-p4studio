################################################################################
 #  Copyright (C) 2024 Intel Corporation
 #
 #  Licensed under the Apache License, Version 2.0 (the "License");
 #  you may not use this file except in compliance with the License.
 #  You may obtain a copy of the License at
 #
 #  http://www.apache.org/licenses/LICENSE-2.0
 #
 #  Unless required by applicable law or agreed to in writing,
 #  software distributed under the License is distributed on an "AS IS" BASIS,
 #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 #  See the License for the specific language governing permissions
 #  and limitations under the License.
 #
 #
 #  SPDX-License-Identifier: Apache-2.0
################################################################################

#!/usr/bin/env python
#
# This script reports info about the git version as a single line
# to stdout

import sys
import os
import subprocess
import argparse

git_tag_head_cmd = "git describe --tags --dirty=modified"
git_rev_parse_cmd = "git rev-parse HEAD"
git_status_cmd = "git status -s" # Search for M at start of line after strip

global debug
debug = False

def run_command(cmd, target_dir=None, fail_okay=True):
    """
    Run the command and return output
    Call error_occurred if exception
    """
    global debug

    if debug:
        print("Running cmd %s" % cmd)
        if target_dir:
            print("In directory %s" % target_dir)


    if target_dir:
        original_dir = os.getcwd()
        os.chdir(target_dir)

    try:
        p1 = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        result = p1.communicate()[0]
        if debug:
            print("Got result %s" % result)
        if p1.wait() != 0:
            if fail_okay:
                return ""
            error_occurred("Bad return code for cmd", cmd=cmd)
    except: # FIXME: Need to catch proper errors
        if fail_okay:
            return ""
        e = sys.exc_info()[0]
        sys.stderr.write("ERROR: %s. Aborting\n" %str(e))
        sys.exit(1)

    if target_dir:
        os.chdir(original_dir)

    return result.strip().decode()

def repo_is_dirty(target_dir=None):
    """
    Run git status and check for M at start of string
    """
    result = run_command(git_status_cmd.split(' '), target_dir=target_dir)
    for line in result.split("\n"):
        line = line.strip()
        if line and line[0] == "M":
            return True
    return False

def get_tag(submod=None, try_tag=False, root_dir=None):
    """
    Get the tag name of the given submodule, if any
    @param submod If None, get the top level tag if any
    """
    if root_dir:
        if submod:
            target_dir = root_dir + "/submodules/" + submod
        else:
            target_dir = root_dir
    elif submod:
        target_dir = "submodules/" + submod
    else:
        target_dir = "."

    if try_tag:
        rev = run_command(git_tag_head_cmd.split(' '), target_dir=target_dir)
    else:
        rev = run_command(git_rev_parse_cmd.split(' '), target_dir=target_dir)[:8]
        if repo_is_dirty(target_dir):
            rev += "-modified"
    return rev

def generate_version(args):
    return get_tag(args.submodule, args.tag)

if __name__ == "__main__":
    usage = "%(prog)s [options]"
    desc = 'Report the current repository or submodule version string to stdout'
    parser = argparse.ArgumentParser(description=desc, usage=usage)

    parser.add_argument('--debug', action='store_true',
                        help="Run with debugging enabled")
    parser.add_argument('-s', '--submodule', action='store', type=str,
                        help="Get version of this submodule; otherwise top level")
    parser.add_argument("-d", "--define", action='store', type=str,
                        help="Generate a C define using this name")
    parser.add_argument("-t", "--tag", action='store_true',
                        help="Try to get the tag name for the current revision")
    args = parser.parse_args()

    if args.debug:
        debug = True

    version = generate_version(args)

    if args.define:
        sys.stdout.write("""\
#define %s "%s"
""" % (args.define, version))
    else:
        sys.stdout.write(version + "\n")
