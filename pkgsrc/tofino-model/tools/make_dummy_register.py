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

#!/usr/local/bin/python3

import fileinput
import re
import sys
import os
import argparse

# default read and write accessor actions, can be overridden per field using
#  command line options
read_accessor_action = 'assert(0); return 0;'
write_accessor_action = 'assert(0);'

# arg parser
parser = argparse.ArgumentParser(description="Generate a dummy register class file from a real register in another chip",
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('--read_action', nargs=2 , action='append',
                    metavar=('FIELD_NAME', 'ACTION'),
                    help="a read action for field, eg: 'return 1;'")
parser.add_argument('--write_action', nargs=2, action='append',
                    metavar=('FIELD_NAME', 'ACTION'),
                    help="an write action for field, eg blank to overide the default assert action.")
parser.add_argument('--chip_name',help="chip name for the output file can use notjbay for all others (default: %(default)s)",
                    default="jbay")
parser.add_argument('--out_dir',help="output directory (default: %(default)s)",
                    default="include/CHIP_NAME/dummy_registers/register_includes")
parser.add_argument('--stub_out_array',help="stub out array - ie don't call underlying registers, useful if the array dimension is changing",
                    default=1)

parser.add_argument('infile', help="input file name")

args = parser.parse_args()

# make dicts of per-field read and write action overrides
read_actions  = { }
write_actions = { }
if args.read_action :
    for a in args.read_action :
        read_actions[ a[0] ] = a[1]
if args.write_action :
    for a in args.write_action :
        write_actions[ a[0] ] = a[1]

chips_to_do = {}
if ( args.chip_name == "notjbay" ) :
    chips_to_do = { "tofino", "tofinoB0"}
else :
    chips_to_do = { args.chip_name }

for chip_name in chips_to_do :
    # work out and check output file name
    infile_base = os.path.basename( args.infile )
    out_dir = args.out_dir
    out_dir = re.sub( 'CHIP_NAME', chip_name , out_dir)
    outfile = os.path.join( out_dir, infile_base )
    if os.path.exists( outfile ) and os.path.samefile( args.infile, outfile ) :
        print("Error: Output file and input file are the same")
        exit(1)
    f = open(outfile, 'w')

    print ( "Writing " + outfile )

    # print the command line
    cmd_line=''
    for a in sys.argv :
        if re.search('\s',a) :
            # quote args with whitespace
            cmd_line += ' \'' + a + '\''
        else :
            cmd_line += ' ' + a
    f.write( "// Generated using:\n" )
    f.write( "//   " + cmd_line + "\n")

    callbacks=0
    # transform the input file
    for line in fileinput.input( args.infile ) :
        if re.search('write_callback.*read_callback',line) :
            callbacks=1
        line = re.sub('namespace (tofinoB0|tofino|jbay|cb)',
                      'namespace '+ chip_name,
                      line);
        line = re.sub(': public model_core::RegisterBlock',
                      ': public model_core::DummyRegisterBlock',
                      line);
        if callbacks == 1 :
            callbacks = 0
            line = re.sub(': RegisterBlock\(.*\)',
                          ': DummyRegisterBlock(write_callback,read_callback)',
                          line);
        else :
            line = re.sub(': RegisterBlock\(.*\)',
                          ': DummyRegisterBlock()',
                          line);
        # remove return by reference so constants can be returned
        #  after the function name there is either a '(' or the end of line
        line = re.sub('^(  uint[81632]+_t )&([a-z0-9_]+(?:\(|$))',
                      r'\1\2',
                      line)
        # look for accessor functions (read and write)
        #  then modify them using default or overridden values for actions
        match = re.match('^  (uint[81632]+_t|void) &?([a-z0-9_]+)\(.*?\)',line)
        if ( match ) :
            action = write_accessor_action
            name = match.group(2)
            if match.group(1) == 'void' :
                if name in write_actions :
                    action = write_actions[ name ]
                else :
                    action = write_accessor_action
            else :
                if name in read_actions :
                    action = read_actions[ name ]
                else :
                    action = read_accessor_action

            # look for the case where everything is on one line
            if re.search('\) \{.*\}',line) :
                line = re.sub('\) \{.*\}',
                              ') { '+action+' }',
                              line)
            # in the array case the accessors are not all on one line, but
            #  these will call the non-array class which will do the right thing
            #  (either assert or return a constant value)

        if args.stub_out_array == 1 :
            # Now deal with stubbing out the array case, we could leave this in for cases
            #  where the register is no longer being used, but in the case where the array
            #  dimension is changing we need to stub out the array to avoid clashes between
            #  the real register and the dummy register
            # Don't use this if you are relying on the return value of the underlying dummy register
            line = re.sub('std::vector<.*> array',
                          'std::vector<int> array',
                          line);
            # the only includes with "" (rather than <>) are for registers instantated in the array
            if ( re.search('#include "',line) ) :
                line = '// ' + line;

                line = re.sub('array.*\.to_string\(.*\)',
                              '""',
                              line);
            line = re.sub('array.*\.reset\(\)', '', line);
            line = re.sub('array.*\.read\(.*\)', '', line);
            line = re.sub('array.*\.write\(.*\)', '', line);
            line = re.sub('return array.*\(\)', 'return 0', line);


        f.write( line )


    f.close()
