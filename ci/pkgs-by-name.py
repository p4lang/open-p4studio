#! /usr/bin/env python3

import os, sys, re, fileinput, copy
import math
import pprint as pp
import subprocess
import shell_var_parser as svp

if not os.path.exists('/etc/os-release'):
    print("No file /etc/os-release to determine Linux distribution.  Aborting.", file=sys.stderr)
    sys.exit(1)

#print("File /etc/os-release exists.  Parsing")


######################################################################
# Hack up my own simple code attempting to parse /etc/os-release
# in package I named shell_var_parser
######################################################################

# This simply assumes that every line contains an `=` character, and
# everything before the first `=` on a line is the name of a shell
# variable, and everything after that is the value, optionally
# enclosed in double quotes.

osinfo = svp.load('/etc/os-release')

#print(len(sys.argv))
if len(sys.argv) != 1 and len(sys.argv) != 2:
    print("usage: %s [ <dpkg-status-filename-to-read> ]"
          "" % (sys.argv[0]), file=sys.stderr)
    sys.exit(1)


def parse_dnf_info(lines):
    pkgs = []
    pkg = {}
#    print("parse_dnf_info parsing %d lines" % (len(lines)), file=sys.stderr)
    for line in lines:
        line = line.rstrip()
        #print("dbg: line='%s'" % (line), file=sys.stderr)
        # Blank lines separate info about different packages
        if line == '':
            if 'installed_size' not in pkg:
                pkg['installed_size'] = 0
            if ('name' not in pkg):
                print('problem with pkg=%s' % (pkg))
            assert pkg['name'] is not None and pkg['installed_size'] is not None and pkg['version'] is not None
            pkgs.append(pkg)
            pkg = {}
            continue
        match = re.search(r"^Name\s*:\s*(.*)\s*$", line)
        if match:
            pkg['name'] = match.group(1)
#            print("dbg: Found package name '%s' in line '%s'"
#                  "" % (pkg['name'], line), file=sys.stderr)
            continue
        match = re.search(r"^\s*Version\s*:\s*(.*)\s*$", line)
        if match:
            pkg['version'] = match.group(1)
        match = re.search(r"^\s*Size\s*:\s*(.*)\s*$", line)
        if match:
            size_str = match.group(1)
            match = re.search(r"^(\d+(\.\d+)?)(\s+([kM]))?$", size_str)
            if not match:
                print("Found line '%s' with size value '%s' that was in an unrecognized format"
                      "" % (line, size_str))
                sys.exit(1)
            value = float(match.group(1))
            units = match.group(4)
#            print("Found size string '%s' parsed into value=%s units=%s"
#                  "" % (size_str, value, units), file=sys.stderr)
            if units == 'k':
                installed_size_kbytes = math.ceil(value)
            elif units == 'M':
                installed_size_kbytes = math.ceil(1024.0 * value)
            else:
                if value < 1:
                    installed_size_kbytes = 0
                else:
                    # Round up to 1 kbyte
                    installed_size_kbytes = 1
            pkg['installed_size'] = installed_size_kbytes
            continue
    # Put the last pkg in pkgs, if there was no blank line at the end
    # of the file
    if (len(pkg) > 0 and pkg['name'] is not None and
        pkg['installed_size'] is not None):
        pkgs.append(pkg)
    return pkgs


def parse_dpkg_status(dpkg_status_fname):
    # I have seen /var/lib/dpkg/status files containing descriptions
    # of multiple packages installed with the same package name, with
    # different architectures.  Make pkgs a list, not a map indexed by
    # package name, otherwise we will not report information about all
    # installed packages.
    pkgs = []
    pkg = {}
    for line in fileinput.input(dpkg_status_fname):
        line = line.rstrip()
        # Blank lines separate info about different packages
        if line == '':
            if 'installed_size' not in pkg:
                pkg['installed_size'] = 0
            if ('name' not in pkg):
                print('problem with pkg=%s' % (pkg))
            assert pkg['name'] is not None and pkg['installed_size'] is not None
            pkgs.append(pkg)
            pkg = {}
            continue
        match = re.search(r"^Package:\s*(.*)$", line)
        if match:
            pkg['name'] = match.group(1)
            continue
        match = re.search(r"^\s*Version\s*:\s*(.*)\s*$", line)
        if match:
            pkg['version'] = match.group(1)
        match = re.search(r"^Installed-Size:\s*(.*)$", line)
        if match:
            size_str = match.group(1)
            match = re.search(r"^\d+$", size_str)
            if not match:
                print("Found line '%s' with value that wasn't a decimal integer"
                      "" % (line))
                sys.exit(1)
            pkg['installed_size'] = int(size_str)
            continue
    # Put the last pkg in pkgs, if there was no blank line at the end
    # of the file
    if (len(pkg) > 0 and pkg['name'] is not None and
        pkg['installed_size'] is not None):
        pkgs.append(pkg)
    return pkgs


parse_format = None
if osinfo['ID'] == 'fedora':
    parse_format = 'dnf-info'
elif osinfo['ID'] == 'rocky':
    parse_format = 'dnf-info'
elif osinfo['ID'] == 'ubuntu':
    parse_format = 'dpkg-status'
elif osinfo['ID'] == 'debian':
    parse_format = 'dpkg-status'

dpkg_status_fname = '/var/lib/dpkg/status'
if len(sys.argv) == 2:
    dpkg_status_fname = sys.argv[1]
    # Force parsing of 'dpkg-status' format file, even if we are
    # running on a Fedora system, if the user provides such an input
    # file.
    parse_format = 'dpkg-status'

if parse_format == 'dpkg-status':
    pkgs = parse_dpkg_status(dpkg_status_fname)
elif parse_format == 'dnf-info':
    dnf_info_lines = subprocess.getoutput('dnf info --installed').splitlines()
    pkgs = parse_dnf_info(dnf_info_lines)
else:
    print("Cannot determine format of packet list to parse.", file=sys.stderr)
    print("    ID=%s found in /etc/os-release"
          "" % (osinfo['ID']), file=sys.stderr)
    sys.exit(1)

total_size = 0
for pkg in pkgs:
    total_size += pkg['installed_size']

pkgs_by_name = sorted(pkgs,
                      key=lambda pkg: (pkg['name'], pkg['version']))

for pkg in pkgs_by_name:
    version_str = ''
    if 'version' in pkg:
        version_str = ' ' + pkg['version']
    print("%s%s" % (pkg['name'], version_str))
