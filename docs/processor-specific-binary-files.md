# Introduction

As of 2026-Apr-26, there are a few files containing compiled object
code specific to a particular kind of processor in the open-p4studio
repository.  They are all for one of the processors / architectures
x86_64, amd64, or i686.  Below is a complete list of those files known
at this time, from a fairly thorough search.


## Files with suffix `.gz`

+ File: `pkgsrc/p4o/p4o-1.0.x86_64.tar.gz`
  + when unpacked contains file: `p4o-1.0/bin/p4obfuscator`
  + `file` utility reports `p4obfuscator` contents as: `ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 2.6.32, BuildID[sha1]=dfbc183e3d33ee1a466f7bb8a14820cc3512d01a, with debug_info, not stripped`
  + The file is unpacked and installed during open-p4studio
    installation, depending upon install options.

When Intel published open-p4studio, they did not include any source
code for the `p4obfuscator` program, only the pre-compiled x86_64
binary.


## `libport_mgr_hw` compiled libraries

+ File: `pkgsrc/bf-drivers/src/port_mgr/port_mgr_lib/libport_mgr_hw.so`
  + `ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, BuildID[sha1]=3e3b4411c3dd9dcc344f410564fa6d75ddf347e6, not stripped`
+ File `pkgsrc/bf-drivers/src/port_mgr/port_mgr_lib/libport_mgr_hw.a`
  + when unpacked with command `ar x ...`, it contains 63 files with `.o` suffix
    + `file` utility reports all 63 files contents as: `ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped`

When Intel published open-p4studio, they did not include any source
code for the compiled object files in `libport_mgr_hw`, perhaps
because the library includes proprietary source code for
[Serdes](https://en.wikipedia.org/wiki/SerDes) drivers that Intel did
not acquire the permission to release its source code under an open
source license such as Apache-2.0.

Before 2026-Jan-01, owners of Tofino ASICs could contact the Intel RDC
(Intel® Resource & Documentation Center) and get permission to obtain
a copy of this proprietary source code.  The file
[`RDC_README`](../hw/RDC_README) describes how to take the software
obtained from Intel RDC and copy it to appropriate places within
open-p4studio so that it can be compiled from source and included in
the `bf-drivers` component.

Unfortunately, Intel RDC no longer satisfies these requests.

Warning: If you are considering purchasing a Tofino ASIC, you should
try to get a copy of the proprietary portions of this driver code,
perhaps from those you purchased the Tofino ASIC from.  Be sure to
keep backups of any proprietary Tofino software you have, as getting
it from Intel in the future, if that is even possible, would probably
require very special arrangements, e.g. Intel Vice President approval
or something similar.


### More details about where `libport_mgr_hw` source file names are mentioned within the open-p4studio repository

The files `libport_mgr_hw.so` and `libport_mgr_hw.a` are mentioned by
name in two CMakeLists.txt files, so removing them causes the build to
fail, at least if you attempt to build the `bf-drivers` component.

```
$ find . \! -type d | xargs grep libport_mgr_hw
./pkgsrc/bf-drivers/src/port_mgr/CMakeLists.txt:  ${CMAKE_CURRENT_SOURCE_DIR}/port_mgr_lib/libport_mgr_hw.a
./pkgsrc/bf-drivers/src/port_mgr/CMakeLists.txt:  ${CMAKE_CURRENT_SOURCE_DIR}/port_mgr_lib/libport_mgr_hw.so
./pkgsrc/bf-drivers/bf_switchd/CMakeLists.txt:find_program(MY_PROGRAM libport_mgr_hw.a
```

If you use `ar x libport_mgr_hw.a` to extract the contents of that
archive, it creates 63 files with names of the form `<name>.c.o`.
Note: Many of those names are mentioned in:

+ The documentation file `hw/RDC_README`
+ the shell script `hw/rdc_setup.sh`

Many of the names are mentioned either exactly, or as C/C++ symbol
names that start with the names, in `.h` or `.c` files somewhere
within one of these directories, or their subdirectories:

+ `pkgsrc/bf-drivers/src/port_mgr`
+ `pkgsrc/bf-drivers/include/port_mgr`
+ `pkgsrc/bf-drivers/src/bm_pm`
+ `pkgsrc/bf-drivers/src/lld`
+ several other directories within `pkgsrc/bf-drivers` that I will not bother to list here.

Below are a few other files outside of the `pkgsrc/bf-drivers`
directories where `.h` files with related names are included:

```
./pkgsrc/switch-p4-16/api/common/pal.cpp:#include <port_mgr/bf_tof2_serdes_if.h>
./pkgsrc/bf-diags/api/src/diag_api.c:#include <port_mgr/bf_serdes_if.h>
./pkgsrc/bf-diags/api/src/diag_util.c:#include <port_mgr/bf_serdes_if.h>
./pkgsrc/bf-diags/api/src/diag_vlan.c:#include <port_mgr/bf_serdes_if.h>
./pkgsrc/switch-p4-16/api/common/pal.cpp:#include <port_mgr/bf_serdes_if.h>
./pkgsrc/p4-examples/ptf-tests/port_mgr_ha/test.py:    app_serdes_ca = self.devport_mgr.devport_mgr_serdes_ca_get(0, port);
```


## Files in directory `pkgsrc/bf-drivers/src/firmware`

All of the files in this directory are empty.

+ `microp_fw.bin`
+ `pcie_serdes.rom`
+ `sbus_master.rom`
+ `serdes_A0.rom`
+ `serdes_B0.rom`
+ `tof2_A0_grp_0_7_serdes.bin`
+ `tof2_A0_grp_8_serdes.bin`

TODO: Can the empty files be removed from open-p4studio?  They are
currently mentioned in CMakeLists.txt files and other source files, so
probably not.  Confirm whether removing them and attempting to build
causes problems, and if so, document it here.


## Other files with suffix `.rom`

There are none, other than those listed in an earlier section.


## Other files with suffix `.bin`

+ `pkgsrc/tofino-model/bin/tofino-model.i686.bin`
  + `ELF 32-bit LSB executable, Intel 80386, version 1 (GNU/Linux), dynamically linked, interpreter /lib/ld-linux.so.2, for GNU/Linux 2.6.32, BuildID[sha1]=5a470ef89d0608fa16742bb586c64e9099779ebe, not stripped`
+ `pkgsrc/tofino-model/bin/tofino-model.x86_64.bin`
  + `ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 2.6.32, BuildID[sha1]=ca0ee1b6a2c780ef0bb6c9a8bb11b965ff5d7b42, not stripped`

TODO: Is it reasonable to remove the `tofino-model.*` files from the
repository, now that the Tofino model source code is included in
open-p4studio?  Note that their names are still mentioned in the top
level CMakeLists.txt file.


## Other files with suffix `.so`

+ File: `p4studio/third_party/_yaml.cpython-35m-x86_64-linux-gnu.so`
  + `ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, BuildID[sha1]=5b67f5cf630d08c2490517119c45570ae76fb012, with debug_info, not stripped`

TODO: It seems likely that the `yaml` file above was created during a
build of open-p4studio code and added to this repository by accident.
Test builds after removing the file to check whether they succeed, and
if no problems, remove it.


## Other files with suffix `.a`

+ File: `pkgsrc/target-utils/third-party/tommyds/benchmark/lib/judy/libJudyMalloc.a`
  + when unpacked contains file: `JudyMalloc.o`
    + `file` utility reports `JudyMalloc.o` contents as: `ELF 32-bit LSB relocatable, Intel 80386, version 1 (SYSV), with debug_info, not stripped`
+ File: `pkgsrc/target-utils/third-party/tommyds/benchmark/lib/judy/libJudyL.a`
  + when unpacked contains 15 files with `.o` suffix, including: `JudyLCascade.o`
    + `file` utility reports all 15 files contents as: `ELF 32-bit LSB relocatable, Intel 80386, version 1 (SYSV), with debug_info, not stripped`

These files are in the original repository
https://github.com/amadvance/tommyds from which all of the files in
directory `pkgsrc/target-utils/third-party/tommyds` were copied from.
May as well leave them there.  They are probably only ever used if you
do benchmark performance testing of the tommyds project code.


## Files with suffix `.zip`

+ File: `p4studio/third_party/pkg_resources/tests/data/my-test-package-zip/my-test-package.zip`
  + Contains only files describing a test Python package.  No processor-specific binaries.

This file is in the original repository
https://github.com/pypa/setuptools from which all of the files in
directory `p4studio/third_party/pkg_resources` were copied from.  Thus
it seems best to leave it there.  Also the files inside the zip
archive do not contain any compiled object code.
