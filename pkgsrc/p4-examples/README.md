P4 Examples Package
===================

P4_16 programs and PTF test scripts corresponding to each of them.
    
The `p4-examples` package is organized as follows
* `p4_16_programs`
Example P4_16 programs
* `programs`
Example P4_14 programs    
* `ptf-tests`
PTF tests corresponding to each of the example P4_14 programs in the `programs` directory
    Makefile:
        Makefile to install conf files needed by switchd application for
        PTF tests

Building and installing
=======================

Building and installation of the examples is done using the `p4studio` tool. It is possible to either build the examples during the `open-p4studio` installation or to build them later if desired. The latter option saves time during the initial installation and does eliminate possible issues with the examples that can occasionally become broken. 

It is possible to build all the examples, just P4_14 or P4_16 programs as well as each individual one. In all cases we assume that `open-p4studio` has already been installed and the critical environment variables have been set either by *sourcing* the script `~/setup-open-p4studio.bash`, manually or otherwise.

To build an individual example (be it a P4_14 or a P4_16 one) use the command below. Each example is located in its separate directory and the name of that directory is the example's name. The reason we use this terminology (i.e., "example_name" instead of "program_name" is because some examples (e.g. `tna_32q_multiprogram`) might consist of more than one P4 program:

```
$SDE/p4studio/p4studio build <example_name> 
```

To build all P4_16 examples use the command:

```
$SDE/p4studio/p4studio build p4-16-programs
```

To build all P4_14 examples use the command:

```
$SDE/p4studio/p4studio build p4-14-programs
```

To build all examples use the command:

```
$SDE/p4studio/p4studio build p4-examples
```

Running the PTF tests
=====================

It is assumed that the environment variables will be set correctly in all of them.

It is also critical to create the necessary VETH interfaces by running the following command:

```
sudo $SDE_INSTALL/bin/veth_setup.sh
```

To run the PTF tests on the Tofino Model you will need 3 separate windows (or, at least, shell sessions):

1. In the first one, you will need to run the model using the command:
   
   ```
   $SDE/run_tofino_model.sh [--arch tofino2] -p example_name
   ```
   
2. In the second window you will need to run the user-space driver using the command:

   ```
   $SDE/run_switchd.sh [--arch tofino2] -p example_name
   ```

3. In the third window you will need to run the PTF test itself using the command:

   ```
   $SDE/run_p4_tests.sh [--arch tofino2] -p example_name
   ```

Please, be aware that the scripts mentioned above have special code to make running the examples as easy as possible by being aware and properly consuming additional files stored in the examples' subdirectores. Running your own programs and their PTF tests might require additional options.
