# Introduction

Intel® P4 Studio Software Development Environment (SDE) is a set of packages for programming Intel’s line of programmable Ethernet Switches. The package also contains scripts for building and installing SDE. The fully automated script (`install.sh`) introduced in this release internally invokes the existing command line interface tool (`p4studio`) to ease the build and installation process. The following sections describe how to build, install, and run the Intel P4 Studio SDE.

# What is included

The following things are included in this repository:

+ Driver software sufficient for running with the simulation model.
+ The Barefoot Runtime Interface (BRI), which consists of:
  + The locally callable BfRt API, with bindings in C and C++.
  + The gRPC-based protocol, called BF Runtime, together with the
    server implementation in C++ and both C++ and Python client
    bindings (others can be generated using the provided proto files).
    + The additional bfrt_grpc.client Python module provides
      easier-to-use, simplified interface to BF Runtime (used for most
      PTF tests).
+ An x86_64 binary of the simulation model
  + Note: Source code for the simulation model is in the process of
    being released to open source, but that is not yet complete.
+ Example P4_16 programs and PTF tests
+ Git submodule referring to the latest version of the P4 compiler
  in the repository https://github.com/p4lang/p4c tested with this code.

This supports developing and compiling P4 programs for Tofino 1 and 2,
developing control plane software that runs with the simulation model
(but not a real hardware board with Tofino switch ASICs - see below),
and simulating its execution on the model.

Some things not included, that one must get from Intel:

+ P4Insight GUI for visualizing the hardware resources used by P4 programs
  after they have been compiled.
  + Note: P4.org personnel are in communication with Intel to see if this
    can be released as open source soon.
+ Some bfrt_python code is included, but it is not yet clear if all of
  the pieces required to use it are included yet.  Follow this issue
  if you are interested:
  https://github.com/p4lang/open-p4studio/issues/23
+ BSPs (Board Support Packages) that enable the SDE to access and
  configure hardware on a physical board, e.g. configuring physical
  Ethernet ports and manage related components, such as repeaters,
  retimers, SFPs, QSFPs, etc.
+ ASIC-specific Serdes drivers.  These are not necessary for running
  the simulation model, but essential for running the code on the real
  ASICs.  See instructions in the directory `hw`.  The necessary code
  is available from Intel RDC (Resource & Documentation Center) for
  those with authorization to access it.

# Setup

```bash
git clone https://github.com/p4lang/open-p4studio.git
cd open-p4studio
```

# Batch build and install Intel® P4 Studio Software Development Environment (SDE)

This script checks that you have the right processor type, and
sufficient RAM and free disk space, and then installs the SDE using a
default choice for options.  See later below for instructions that let
you customize the installation choices.

```bash
./batch-install.sh
```

# Run instructions

Do this command in each terminal where you wish to use the SDE:

```bash
source $HOME/setup-open-p4studio.bash
```

You may wish to add that line to your `$HOME/.bashrc` file to automate
this.

This command creates many virtual Ethernet interfaces that are used
when running tests involving the Tofino model, so it can send and
receive packets on those interfaces.  It only needs to be done once on
a system after it has been rebooted, before running tests that require
these interfaces:

```bash
sudo ${SDE_INSTALL}/bin/veth_setup.sh 128
```

# Installation with customization of options

1. Run the `install.sh` script

   **Note**: The `install.sh` script initially evaluates system capabilities such as free space and OS support to build and install SDE.

2. Install default settings or user-defined settings based on your need.

    **Install default setting**:

    **Note**: Default settings allow you to run P4-16 examples for all Tofino chip types on the ASIC model. This is more appropriate for a beginner as the entire   build and installation will be taken care of by the script.

    a)	Select **Yes** to start with default setting installation.

    b)	Script installs the following source packages:
    
      * bridge
      * libcli
      * thrift
      * grpc

    c)	Script installs the following configuration options:

     * tofino
     * tofino2
     * tofino2m
     * tofino3

    d)	Script installs the p4-16-programs.

    e)	Script installs all the dependencies one after the other.


    **Install user-defined setting**:

    **Note 1**: It is recommended to install user-defined settings by an experienced SDE user.

    a)	Select **No** to start with user-defined setting installation.
    
    b)	Select **Yes** to install missing third-party dependencies.
    
    c)	Select an appropriate deployment target.
    
      **Note**: The available targets are `Hardware` and `ASIC Model`. The following interactive questions varies based on the target selection. Here, we use `ASIC Model` to explain the concept. 
        
    d)	Select the Tofino chip type.
    
      **Note**: The available p4 architectures are `tofino`, `tofino2`, `tofino2m`, and `tofino3`.
      
    e)	Select **Yes** to build kernel modules (for example, `bf_kdrv`, `bf_kpkt`, and `bf_knet`).
    
    f)	Enter path to kernel headers directory or leave blank to autodetect.
    
    g)	Select p4 programs and corresponding control plane code.
    
      **Note**: The supported options are `P4 examples: P4-16`, `P4 examples: P4-14`, `switch-p4-16: x1_tofino`, `switch-p4-16: x2_tofino`, `switch-p4-16: x6_tofino`, and `bf-diags`.  
      
    h)	Select **Yes** to configure advanced options. Advanced options include p4rt, TDI, and SAI configurations.
    
    i)	The created profile details are displayed at this stage. Save the profile details in a YAML file for future use. 
    
3. Script completes the SDE installation process.


# Run the Tofino Model

The below steps outline how to run P4 tests against the model in a
three-window setup.

In first window, launch the model:
```bash
./run_tofino_model.sh -p [P4 TEST] --arch [tofino|tofino2]
```

In the second window, launch the driver:
```bash
./run_switchd.sh -p [P4 TEST] --arch [tofino|tofino2]
```

Note: It may take one to two minutes after the last of the two
commands above has been started before they complete their
initialization.  When it is, you will see a `bf_shell>` prompt in the
driver window.

In the third window, launch the P4 test suite:
```bash
./run_p4_tests.sh -p [P4 TEST] --arch [tofino|tofino2]
```

To run a specific test within a test suite:
```bash
./run_p4_tests.sh -p [P4 TEST] --arch [tofino|tofino2] -s test.[SINGLE TEST]
```


For example, to run the `tna_counter` test suite on the Tofino 1 model:
```bash
./run_tofino_model.sh -p tna_counter --arch tofino
./run_switchd.sh -p tna_counter --arch tofino
./run_p4_tests.sh -p tna_counter --arch tofino
```

To run just a specific test from the suite:
```bash
./run_p4_tests.sh -p tna_counter --arch tofino -s test.DirectCounterTest
```

For users who acquired the private RDC software release and want to adapt the existing open source code to run on Tofino hardware, follow the RDC_README instructions within the `open-p4studio/hw` directory. The script `open-p4studio/hw/rdc_setup.sh` automates the transfer of the RDC files necessary in the open source repository for running on hardware.

# P4Studio

The `p4studio` tool is available in `open-p4studio/p4studio` and is used to build the Intel P4 studio SDE as directed by the user’s YAML build profile.

The p4studio tool works in one of three different modes: Interactive Mode, Profile Mode, or Command Mode. All the user-defined installation process explained above falls under interactive mode.

Run `./p4studio/p4studio --help` to show all supported options.

**Syntax**

```
p4studio {
          app {
               activate
              }          
          check-system {
                        --install-dir
                        --asic
                        --kdir
                       }
          packages {
                    extract
                    list
                   }     
          dependencies {
                        install
                        list
                       }
          configure {
                     --verbosity
                     --log-file
                     --build-type [debug|release|relwithdebinfo|minsizerel]
                     --install-prefix
                     --bsp-path
                     --p4ppflags
                     --extra-cppflags
                     --p4flags
                     --kdir
                    }           
          build  {
                  --verbosity
                  --log-file
                  --jobs
                 }       
          profile {
                   apply
                   create
                   describe
                  }      
          Interactive {
                       --verbosity
                       --log-file                                  
                      }
          Clean {
                 --skip-logs       
                 --skip-build-dir 
                 -y, --yes
                }                      
}
```
