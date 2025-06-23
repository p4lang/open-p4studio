Compiling your own P4 code
==========================

This document explains how to compile your own P4 code, that can be located anywhere outside the open-P4studio tree.

## A little theory...

On the surface, the P4 compiler for Tofino (`p4c`) works like any other compiler. In other words, it is totally ok to compile your program using the following command line:

```
p4c my_program.p4
```

This should work for any simple program, written for Tofino. At the end of the compilation process the compiler will create the directory `my_program.tofino`, containing all compilation artifacts. If your program is intended for Tofino2, add the parameter `--target tofino2`; the name of the compilation directory will be `my_program.tofino2` then.

In more complex cases, you might need to provide additional parameters, similar to the ones used by most C compilers, such as `-I <include_dir>`, `-DCPP_VAR[=value]` or `-UCPP_VAR`. It is also highly recommended to add the `-g` parameter, so that the compiler can output the detailed logs and placement information (especially if you have access to the `p4i` (P4Insight) tool). 

The compiler has many other special options and they might sometimes be required, but they are not the focus of this document.

While the command above is the easiest way to invoke the compiler and is often used to quickly check whether the program compiles or not, it is rarely employed to compile the P4 programs when the intent is to **run** them. The reason is that after compiling a program we need to exercise it by starting the user-space driver and loading the program onto the target, be it either a Tofino model or the real ASIC. In the simulation case it is also important to allow the model to load the program layout information so that it can produce human-readable logs by translating low-level operations back into the familiar symbols from your program. All these components need to have access to various compilation artifacts and the standard scripts that launch them are not designed to work with the `.tofino` or `.tofino2` directory produced by the compiler. 

Let's take a look behind the scenes...

Generally speaking, the compiler produces three main artifacts that are neccessary to run a P4 program:

1. The compiled program binary, called `tofino.bin` (or `tofino2.bin`) that needs to be loaded into the ASIC or the model so that they can execute the P4 code
2. The `bf-rt.json` file (sometimes also called `bfrt.json`) that describes all the objects, defined in the P4 program, such as tables and their key fields, actions and their action data fields, parser value sets and externs
3. The `context.json` file that describes the detailed layout of the objects, contained in the `bf-rt.json` file inside the device resources

While in theory these files can be located anywhere, the standard programs and scripts provided as a part of open-P4studio assume that there is a **fourth**, so-called _config file_, named `my_program.conf` that contains the information about the location of the three files above. The config file contains some additional information, which comes especially handy in more complex use cases (such as running different programs on different Tofino pipes, running multi-pipe programs or both). 

The most important things to know is that when you run a standard script, such as 

```
$SDE/run_tofino_model.sh [--arch tofino2] -p my_program
```

or 

```
$SDE/run_switchd.sh [ --arch tofino2] -p my_program
```

they look for the config file `my_program.conf` in the directory `$SDE_INSTALL/share/p4/targets/tofino/` (or `tofino2`), read it and then load the rest. It _is_ possible to run these scripts while having the config file located in another place (and pass its location via an additional `-c` parameter), but that's more complicated and still requires correctly composed config file.

Therefore, after compiling the P4 program it is important to also **install** the compilation artifacts into the proper locations, specifically:

1. Install both `tofino.bin` and `context.json` into the director(ies) named `$SDE_INSTALL/share/tofinopd/my_program/<pipeline_package_instance_name>/`
   a. For Tofino2 the names will be `tofino2.bin`, `context.json` and `$SDE_INSTALL/share/tofino2pd/my_program/<pipeline_package_instance_name>/`
   b. The typical pipeline package instance name is `pipe`, but it is under the ultimate control of the P4 programmer
   c. The files are produced per `Pipeline()` package. Thus, multi-pipeline programs (such as `tna_32q_multipipe`) might contain more than one pipeline directory
2. Install the file `bf_rt.json` into the directory `$SDE_INSTALL/share/tofinopd/my_program/`
   a. In Tofino2 case, the directory name will be `$SDE_INSTALL/share/tofino2pd/my_program/`
3. Create and install the file `my_program.conf`, containing the proper paths to the files above into the directory `$SDE_INSTALL/share/p4/targets/tofino/`
   a. In Tofino2 case the directory name will be `$SDE_INSTALL/share/p4/targets/tofino2/`

Therefore, `open-p4studio` provides a special framework that not only invokes the compiler, but performs the correct installation of all the artifacts into the proper places. This is what you should be using if you plan to exercise your P4 programs using the standard tools and scripts.

## The standard P4 build process

The standard P4 program build and installation process is `cmake`-based and consists of the following steps:

1. Choose and create a build directory, which will be used to compile your P4 program. It can be located anywhere in the file system.
   a. **Important note** If the directory already exists and is going to be re-used, it is highly recommended to wipe it clean. `cmake` caches its parameters and if the directory is not wiped, there is a very good chance that at least some of your new settings (especially compiler flags, etc.) will not take effect
3. CD into that new directory
4. Invoke the `cmake` as will be shown below
5. Invoke `make install` as shown below to perform the actual compilation and installation

Below is an example:

```
rm –rf   $SDE/build/p4-build/my_program

mkdir –p $SDE/build/p4-build/my_program
cd       $SDE/build/p4-build/my_program
cmake $SDE/p4studio                                         \
     -DCMAKE_MODULE_PATH="$SDE/cmake"                       \
     -DCMAKE_INSTALL_PREFIX="$SDE_INSTALL"                  \
     -DP4C=$SDE_INSTALL/bin/p4c                             \
     -DP4_PATH=<full_path_to_my_program.p4>/my_program.p4   \
     -DP4_NAME=my_program                                   \
     -DP4_LANG=p4_16                                        \
     -DTOFINO={ON | OFF} –DTOFINO2={ON | OFF}

make [VERBOSE=1] -j install
```

## Additional details

### Specifying preprocessor flags

If you need to specify additional preprocessor flags, such as additional include directories or to define some preprocessor variables on the command line, it can be done by adding `-DP4PPFLAGS=` parameter to the `cmake` command line, e.g.

```
-DP4PPFLAGS="-I ./includes -DLPM_TABLE_SIZE=10240"
```

### Specifying compiler flags

If you need to specify additional compiler flags, it can be done by adding an additional parameter `-DP4FLAGS=` to the `cmake` command, e.g:

```
-DP4FLAGS="--no-dead-code-elimination"
```

### Building for both Tofino and Tofino2

Some programs are designed to be compiled for both Tofino and Tofino2. The build process above can efficiently support this by compiling the code for Tofino and Tofino2 in parallel. Simply set both `-DTOFINO=ON` and `-DTOFINO2=ON` and use parallel `make` (with the `-j` option). Please, be careful, since not all programs that can be compiled for Tofino can be compiled for Tofino2 even when written using TNA (and not T2NA).

### Build directory

While in theory the build directories for P4 programs' compilation can be located anywhere, it is quite common to keep them under `$SDE/build/p4-build`.

During the program optimization, it is not uncommon to try multiple variants of the same program, different compiler options or different pre-processor settings and then compare them. In this case, it is customary to name a build directory as `$SDE/build/p4-build/my_program.variant1` or similar. This way you can keep as many compilation variants of the same program side-by-side if you want.

### P4_14 program compilation

The procedure above supports P4_14 program compilation and PD API generation as well.
