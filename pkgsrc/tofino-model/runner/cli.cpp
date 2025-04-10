/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

#include "cli.h"

#include <climits>
#include <cstdlib>
#include <list>
#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <common/rmt.h>
#include <model_core/rmt-debug.h>
#include <model_core/rmt-types.h>

#include <model_core/rmt-phv-modification.h>

extern "C" {
#include <libcli.h>
#include "portmanager.h"
}

/* @fixme This is for pcap dumps only right now - from main.cpp */
//static const char *switch_name = "harlyn";
//static int do_pcap_dump = 1;

static char **
get_getopt_argv(int argc, char *argv[]) {
    auto getopt_argv = new char *[argc + 1];
    getopt_argv[0] = (char *)"dummy-command";
    for (int j = 0; j < argc; ++j) {
      getopt_argv[j + 1] = argv[j];
    }
    return getopt_argv;
}

static int
cmd_test(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    int i;
    cli_print(cli, "called %s with \"%s\"", __FUNCTION__, command);
    cli_print(cli, "%d arguments:", argc);
    for (i = 0; i < argc; i++)
        cli_print(cli, "        %s", argv[i]);

    return CLI_OK;
}

static int
cmd_exit_model(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    cli_print(cli, "Exiting");
    exit(0);
    return CLI_OK;
}

static int
cmd_mem_dump(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    extern       char *optarg;
    extern       int   optind;
    int c, dflag, aflag;
    char *d_arg, *a_arg;
    uint64_t addr = 0, data_hi = 0, data_lo = 0;

    uint64_t device_id = 0;
    static const char usage[] = "Usage : mem-dump -d <dev_id> -a <mem_addr>";

    dflag = aflag = 0;
    d_arg = a_arg = NULL;
    optind = 1;

    std::unique_ptr<char *[]> getopt_argv(get_getopt_argv(argc, argv));
    while ((c = getopt(argc + 1, getopt_argv.get(), "d:a:")) != -1)
    {
        switch (c)
        {
            case 'd':
                dflag = 1;
                d_arg = optarg;
                if (!d_arg)
                {
                    cli_print(cli, "%s", usage);
                    return CLI_OK;
                }
                break;
            case 'a':
                aflag = 1;
                a_arg = optarg;
                if (!a_arg)
                {
                    cli_print(cli, "%s", usage);
                    return CLI_OK;
                }
                break;
            default:
                cli_print(cli, "%s", usage);
                return CLI_OK;
        }
    }

    if (dflag == 0)
    {
        cli_print(cli, "%s", usage);
        return CLI_OK;
    }
    else if (dflag == 1)
    {
        device_id = strtoul(d_arg, NULL, 10);
        if ((device_id == ULONG_MAX))
        {
            cli_print(cli, "tbl_info : Invalid device_id %s\n", d_arg);
            return CLI_OK;
        }
    }

    if (aflag == 0)
    {
        cli_print(cli, "%s", usage);
        return CLI_OK;
    }
    else if (aflag == 1)
    {
        addr = strtoul(a_arg, NULL, 16);
    }

    /* Invoke the model indirect read */
    model_ind_read(device_id, addr, &data_lo, &data_hi);

    cli_print(cli, "Data (128 bits) : %016" PRIx64 " %016" PRIx64 "\n", data_hi, data_lo);

    return CLI_OK;
}

static int
cmd_start_pkt_processing(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    rmt_start_packet_processing();
    return CLI_OK;
}

static int
cmd_stop_pkt_processing(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    rmt_stop_packet_processing();
    return CLI_OK;
}

// helper function to parse commonly used log command options; returns true if
// parsing succeeded, false otherwise
static bool parse_log_cmd_options(
  struct cli_def *cli,
  char *argv[],
  int argc,
  char *options,
  int &chip,
  int &pipe,
  int &stage,
  int &type,
  int &row_tab,
  int &col,
  bool &flags_specified,
  bool &complete,
  uint64_t &flags,
  uint64_t &types
) {
  extern       char *optarg;
  extern       int   optind;
  optind = 1;

  char* endp;
  int c;
  uint64_t opt_flags = 0;
  complete = false;
  std::unique_ptr<char *[]> getopt_argv(get_getopt_argv(argc, argv));
  while ((c = getopt(argc + 1, getopt_argv.get(), options)) != -1) {
    opt_flags = UINT64_C(0);
    endp = nullptr;
    char *name = optarg;
    switch (c) {
      case 'd':
        chip = strtoul(optarg, &endp, 10);
        break;
      case 'p':
        pipe = strtoul(optarg, &endp, 10);
        break;
      case 's':
        stage = strtoul(optarg, &endp, 10);
        break;
      case 't': {
        int val = strtoul(optarg, &endp, 10);
        if (!val) {         // did it contain number input
          val = model_core::RmtTypes::toInt(endp); // cast to number
          if (!val) {        // was it a valid string input
            cli_print(cli, "No flag type found for %s\n", endp);
            return false;   // wasn't int or valid string
          }
        }
        endp = nullptr;
        types |= UINT64_C(1)<<(val%64);
        break;
      }
      case 'T':
        endp = nullptr;  // pretend the option strtoul was successful
        type = model_core::RmtDebug::log_type_for_string(optarg);
        if (type < 0) {
          cli_print(cli, "No log type found for %s\n", optarg);
          return false;
        }
        break;
      case 'r':
        row_tab = strtoul(optarg, &endp, 10);
        break;
      case 'c':
        col = strtoul(optarg, &endp, 10);
        break;
      case 'f':
        flags_specified = true;
        opt_flags = strtoul(optarg, &endp, 0);  // not portable to 32 bits!
        if (endp != optarg) {
          flags |= opt_flags;
        } else {
          endp = nullptr;  // pretend the option strtoul was successful
          opt_flags = model_core::RmtDebug::flags_for_string(optarg);
          if (opt_flags) {
            flags |= opt_flags;
          } else {
            cli_print(cli, "No flags found for %s\n", optarg);
            return false;
          }
        }
        break;
      case 'i': {
        if (name == nullptr) name = (char *)"*";

        cli_print(cli, "Flags found: ");
        for (auto it : model_core::RmtDebug::flags_map(name)) {
          cli_print(cli, "  %-30s = 0x%016" PRIx64 " (%" PRId64 ")",
                    it.first.c_str(), it.second, it.second);
        }

        cli_print(cli, "\nTypes found: ");

        std::string s{name};  // for std::string::find 
        if(name[strlen(name)-1] == '*')
          s.pop_back();  // remove wildcard character
        for (auto ent : model_core::RmtTypes::kRmtTypeArray)
        {
//          if(strcmp(ent.name,"undef") && std::string{ent.name}.rfind(s, 0) == 0)  // if you only want prefix comparision
          if(strcmp(ent.name,"undef") && std::string{ent.name}.find(s) != std::string::npos)  // will find any substring
              cli_print(cli, "%s", ent.name);
        }
        complete = true;
        return true;
      }
      case 'l':
        endp = nullptr;  // pretend the option strtoul was successful
        opt_flags = model_core::RmtDebug::flags_for_string(optarg);
        if (opt_flags && (opt_flags <= model_core::RmtDebug::kRmtDebugTrace)) {
          flags |= (opt_flags | (opt_flags - 1));
          flags_specified = true;
        } else {
          cli_print(cli, "No log level matching %s\n", optarg);
          return false;
        }
        break;
      case 'h':
      default:
        return false;
    }
    if (endp == optarg) {
      cli_print(cli, "-%c needs a number arg\n", c);
      return false;
    }
  }
  return true;
}

static int
update_log_flags(struct cli_def *cli, const char *command, char *argv[], int argc, int set) {
  static char usage[] =
      " Change log flags for different parts of the model\n\n"
      " Usage: args: [-i name] [-d device] [-p pipe] [-s stage] [-t type]\n"
      "              [-r row/table] [-c column] [-f <log flags bits or name>]\n\n"
      " See rmt-debug.h for flag values, see rmt-types for type values\n\n"
      " Options:\n"
      "     -i : list all flags matching <name>; <name> may end with '*' for \n"
      "          a wildcard match. Use '-i *' to list all flags.\n"
      "     -d : specifies the chip number. Defaults to 0.\n"
      "     -p : specifies a pipe index to configure. Default to all pipes.\n"
      "     -s : specifies a stage index to configure. Defaults to all stages.\n"
      "     -t : specifies an object type to enable. Defaults to all object types.\n"
      "     -r : specifies a row index to configure. Defaults to all indexes.\n"
      "     -c : specifies a column index to configure. Defaults to all indexes.\n"
      "     -f : specifies a log flags mask. The value may be either an int\n"
      "          value or the name of a flag; flag names and bit masks may be\n"
      "          listed using the -i option. Flag names may end with a \n"
      "          wildcard * to match all flags starting with that prefix.\n"
      "          If multiple -f options are specified then their flag masks\n"
      "          will be OR'd.\n\n";
  int chip = 0;
  int pipe = -1;
  int stage = -1;
  int type = -1;
  int row_tab = -1;
  int col = -1;
  bool flags_specified = false, complete = false;
  uint64_t flags = 0;
  uint64_t types = 0;

  bool success = parse_log_cmd_options(
      cli, argv, argc, (char *) "p:s:t:f:r:c:d:l:i:h",
      chip, pipe, stage, type, row_tab, col,
      flags_specified, complete, flags, types);
  if (!success) cli_print(cli, "%s", usage);
  if ((!success) || complete) return CLI_OK;

  if (!flags_specified) flags = model_core::RmtDebug::ALL;
  uint64_t pipes     = pipe  < 0 ? model_core::RmtDebug::ALL : UINT64_C(1)<<pipe;
  uint64_t stages    = stage < 0 ? model_core::RmtDebug::ALL : UINT64_C(1)<<stage;
  uint64_t rows_tabs = row_tab < 0 ? model_core::RmtDebug::ALL : UINT64_C(1)<<row_tab;
  uint64_t cols      = col   < 0 ? model_core::RmtDebug::ALL : UINT64_C(1)<<col;
  if (types == 0) 
    types = model_core::RmtDebug::ALL;
    
  if (set) {
    rmt_update_log_flags(chip, pipes, stages,
                         types, rows_tabs, cols,
                         flags, model_core::RmtDebug::ALL);
  }
  else {
    rmt_update_log_flags(chip, pipes, stages,
                         types, rows_tabs, cols,
                         model_core::RmtDebug::NONE, ~flags);
  }
  return CLI_OK;
}

static int
update_log_type_flags(struct cli_def *cli, const char *command, char *argv[], int argc, int set) {
    // This command has been deprecated so that users no longer need to
    // understand the different masks for model vs P4 log levels (see below).
    // Rather, users should use the rmt-set-log-level command which is
    // sufficient for all modifications of P4 and PKT log flags, or use
    // rmt-set-log-flags for modifying model logging flags.
  static char usage[] =
      " Change log flags for different types of logging\n\n"
      " Usage: args: [-d device] [-p pipe] [-s stage]\n"
      "              [-T log_type_name] [-f 0xflags]\n"
      " Options:\n"
      "     -d : specifies the chip number. Defaults to 0.\n"
      "     -p : specifies a pipe index to configure. Default to all pipes.\n"
      "     -s : specifies a stage index to configure. Defaults to all stages.\n"
      "     -t : specifies the logging type as an integer i.e.:\n"
      "          0->Model, 1->P4, 3->Packet. Defaults to 1 (P4).\n"
      "     -f : specifies a log flags mask. If multiple -f options are specified\n"
      "          then their flag masks will be OR'd. Existing log flag bits\n"
      "          are cleared when a new mask is set.\n\n";
  int chip = 0;
  int pipe = -1;
  int stage = -1;
  int row_tab = -1;  // not used
  int col = -1;  // not used
  int log_type = RMT_LOG_TYPE_P4;
  bool flags_specified = false, complete = false;
  uint64_t flags = 0;
  uint64_t types = 0;

  // NB the -t option for specifying logging type has been kept for backwards
  // compatibility, but -T is preferred because (a) it decouples the user
  // interface from the internal representation of types and (b) it avoids
  // confusion with the other uses of -t to specify object types.
  bool success = parse_log_cmd_options(
      cli, argv, argc, (char *) "d:p:s:t:f:h",
      chip, pipe, stage, log_type, row_tab, col,
      flags_specified, complete, flags, types);
  cli_print(cli,
      " Note: this command is DEPRECATED. To set P4 logging level use\n"
      "'rmt-set-log-level'. To set fine-grained model logging flags use\n"
      "'rmt-set-log-flags'.\n\n"
      );
  if (!success) cli_print(cli, "%s", usage);
  if ((!success) || complete) return CLI_OK;
  if (!flags_specified) {
    flags = model_core::RmtDebug::ALL;
  } else if (log_type != RMT_LOG_TYPE_MODEL) {
    // Shenanigans to maintain backwards compatibility with historic
    // inconsistent log level flags for P4 and PKT type logging. Previously,
    // for P4 and PKT logging, bits [2:4] corresponded to SUMMARY, INFO,
    // VERBOSE. Internally these levels have been remapped to use bits [4:6]
    // (INFO, VERBOSE, DEBUG). Users of this deprecated command will be
    // familiar with the historic flags so a translation is performed here to
    // the new levels here for backwards compatibility only.
    uint64_t given_flags = flags;
    uint64_t old_levels_mask = 0x1C;
    flags |= ((flags & old_levels_mask) << 2);
    cli_print(cli,
        " User-specified deprecated flags   0x%016" PRIx64 "\n"
        " were translated to internal flags 0x%016" PRIx64 ".\n\n",
        given_flags, flags
        );

  }

  uint64_t pipes     = pipe  < 0 ? model_core::RmtDebug::ALL : UINT64_C(1)<<pipe;
  uint64_t stages    = stage < 0 ? model_core::RmtDebug::ALL : UINT64_C(1)<<stage;

  if (set) {
    rmt_update_log_type_levels(chip, pipes, stages, log_type,
                               model_core::RmtDebug::ALL, // clear
                               flags);                    // set
  }
  else {
    rmt_update_log_type_levels(chip, pipes, stages, log_type,
                               flags,  // clear
                               0);     // set
  }
  return CLI_OK;
}

static int
cmd_rmt_set_log_flags(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    return update_log_flags(cli, command, argv, argc, 1);
}

static int
cmd_rmt_clear_log_flags(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    return update_log_flags(cli, command, argv, argc, 0);
}

static int
cmd_rmt_set_log_type_flags(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    return update_log_type_flags(cli, command, argv, argc, 1);
}

static int
cmd_rmt_clear_log_type_flags(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    return update_log_type_flags(cli, command, argv, argc, 0);
}

static int
cmd_rmt_set_log_level(struct cli_def *cli, const char *command, char *argv[], int argc)
{
  static char usage[] =
      " Sets the logging level for different types of logging.\n\n"
      " Usage: args: [-d device] [-p pipe] [-s stage] [-T log_type_name]\n"
      "              [-l <Fatal|Error|Error2|Warn|Info|Verbose|Debug|Trace>]\n\n"
      " Note: this command does not change the log flags selecting the\n"
      " *object types* for which logging is enabled. Use rmt-set-log-flags\n"
      " to modify the log flags selecting object types.\n\n"
      " Options:\n"
      "     -d : specifies the chip number. Defaults to 0.\n"
      "     -p : specifies a pipe index to configure. Default to all pipes.\n"
      "     -s : specifies a stage index to configure. Defaults to all stages.\n"
      "     -T : specifies the logging type as a string i.e.:\n"
      "          Model, P4 or Packet. Defaults to P4.\n"
      "     -l : specifies the log level. Required. Sets the log flags bits\n"
      "          corresponding to the given level and all more severe levels.\n"
      "          e.g. '-l Verbose' will enable logging at Verbose, Info, Warn,\n"
      "          Error2, Error, Fatal. Log flags bits for less severe levels\n"
      "          are cleared.\n\n";
  int chip = 0;
  int pipe = -1;
  int stage = -1;
  int row_tab = -1;  // not used
  int col = -1;  // not used
  int log_type = RMT_LOG_TYPE_P4;
  bool flags_specified = false, complete = false;
  uint64_t flags = 0;
  uint64_t types = 0;

  bool success = parse_log_cmd_options(
      cli, argv, argc, (char *) "d:p:s:T:l:h",
      chip, pipe, stage, log_type, row_tab, col,
      flags_specified, complete, flags, types);
  if (!success) cli_print(cli, "%s", usage);
  if ((!success) || complete) return CLI_OK;

  if (!flags_specified) {
      cli_print(cli, "-l option is required\n");
      cli_print(cli, "%s", usage);
      return CLI_OK;
  };

  uint64_t pipes     = pipe  < 0 ? model_core::RmtDebug::ALL : UINT64_C(1)<<pipe;
  uint64_t stages    = stage < 0 ? model_core::RmtDebug::ALL : UINT64_C(1)<<stage;
  // clear all level bits and add new level bits; ensure we only touch the 8
  // log level bits
  rmt_update_log_type_levels(chip, pipes, stages, log_type, 0xFF, flags & 0xFF);
  return CLI_OK;
}


static int
cmd_rmt_get_mau_info(struct cli_def *cli, const char *command, char *argv[], int argc)
{
  static char rmt_get_mau_info_usage[] = "\tGet mau info for some MAU\n"
      " Usage: args: [-d device] [-p pipe] [-s stage]\n"
      " Unspecified arguments -d,  -p, -s  default to 0.\n";

  int chip = 0;
  int pipe = 0;
  int stage = 0;
  char* endp;

  extern       char *optarg;
  extern       int   optind;
  int c;

  optind = 1;

  std::unique_ptr<char *[]> getopt_argv(get_getopt_argv(argc, argv));
  while ((c = getopt(argc + 1, getopt_argv.get(), "p:s:d:h")) != -1) {
    endp = 0;
    switch (c) {
      case 'd':
        chip = strtoul(optarg, &endp, 10);
        break;
      case 'p':
        pipe = strtoul(optarg, &endp, 10);
        break;
      case 's':
        stage = strtoul(optarg, &endp, 10);
        break;
      case 'h':
      default:
        cli_print(cli, "%s", rmt_get_mau_info_usage);
        return CLI_OK;
    }
  }


  uint32_t array[256];
  const char *name_array[256];

  for (int i = 0; i < 256; i++) {
    array[i] = 0u;
    name_array[i] = NULL;
  }

  rmt_get_mau_info(chip, pipe, stage, array, 256, name_array, true);

  for (int i = 0; i < 256; i++) {
    if ((name_array[i] != NULL) && (array[i] != 0u)) {
      cli_print(cli, "%s = %d", name_array[i], array[i]);
    }
  }

  return CLI_OK;

}

// XXX: functions no longer used
//static int
//rmt_port_add_remove(struct cli_def *cli, const char *command, char *argv[], int argc, int add) {
//  static char rmt_port_add_remove_usage[] = "\trmt-port-add|remove:\n"
//    " Usage: args: [-d device] [-p port] [-i interface_name]\n"
//    " port : device port number [0-n] to add\n"
//    " interface_name : Linux interface_name to be connected/removed as a port\n";
//
//  int chip = 0;
//  int port = -1;
//  char* endp;
//  char *interface = NULL;
//
//  extern       char *optarg;
//  extern       int   optind;
//  int c;
//
//  optind = 1;
//
//  std::unique_ptr<char *[]> getopt_argv(get_getopt_argv(argc, argv));
//  while ((c = getopt(argc + 1, getopt_argv.get(), "d:p:i:h")) != -1) {
//    endp = 0;
//    switch (c) {
//      case 'd':
//        chip = strtoul(optarg, &endp, 0);
//        break;
//      case 'p':
//        port = strtoul(optarg, &endp, 0);
//        break;
//      case 'i':
//        interface = strdup(optarg);
//        break;
//      case 'h':
//      default:
//        cli_print(cli, "%s", rmt_port_add_remove_usage);
//        goto done;
//    }
//    if (chip && (port > 0) && endp == optarg) {
//      cli_print(cli, "-%c needs a number arg\n%s", c, rmt_port_add_remove_usage);
//      goto done;
//    }
//  }
//
//  if (add) {
//    bfm_port_interface_add(interface, port, switch_name, do_pcap_dump);
//  }
//  else {
//    bfm_port_interface_remove(interface);
//  }
//done:
//  if (interface) {
//    free(interface);
//  }
//  return CLI_OK;
//}

//static int
//cmd_rmt_port_add(struct cli_def *cli, const char *command, char *argv[], int argc)
//{
//    return rmt_port_add_remove(cli, command, argv, argc, 1);
//}

static int
cmd_rmt_version(struct cli_def *cli, const char *command, char *argv[], int argc)
{
  // Only display tmodel internal and register versions if internal build
  bool bfn_internal = false;
#ifdef BFN_INTERNAL
  bfn_internal = true;
#endif

  cli_print(cli, "TMODEL_SW_VERSION         = %s", tmodel_get_sw_version());
  if (bfn_internal) {
    cli_print(cli, "TMODEL_INTERNAL_VERSION   = %s", tmodel_get_internal_version());
  }
  cli_print(cli, "RMT_VERSION               = %s", rmt_version());
  if (bfn_internal) {
    for (int chip = 0; chip <= 255; chip++) {
      char *reg_ver = rmt_register_version(chip);
      if ((reg_ver != NULL) && (*reg_ver != '\0')) {
        cli_print(cli, "RMT_REGISTER_VERSION[%3d] = %s", chip, rmt_register_version(chip));
      }
    }
  }
  return CLI_OK;
}

#ifdef BFN_INTERNAL
static bool
parse_modify_cmd_options( // TODO: share the duplicate code with @parse_log_cmd_options()
  struct cli_def *cli,
  char *argv[],
  int argc,
  char *options,
  int &chip,
  int &pipe,
  int &stage,

  model_core::RmtPhvModification::ModifyEnum &which,
  model_core::RmtPhvModification::ActionEnum &action,
  int &index,
  uint32_t &value
) {
  extern       char *optarg;
  extern       int   optind;

  char* endp;
  int c;

  optind = 1;

  std::unique_ptr<char *[]> getopt_argv(get_getopt_argv(argc, argv));
  while ((c = getopt(argc + 1, getopt_argv.get(), options)) != -1) {
    endp = 0;
    switch (c) {
      case 'd':
        chip = strtoul(optarg, &endp, 10);
        break;
      case 'p':
        pipe = strtoul(optarg, &endp, 10);
        break;
      case 's':
        stage = strtoul(optarg, &endp, 10);
        break;
      case 'w':
        which = model_core::RmtPhvModification::phv_for_string(optarg);
        if (which == model_core::RmtPhvModification::ModifyEnum::kErr) {
          cli_print(cli, "Value: [%s] for flag -%c is invalid.\n", optarg, c);
          return false;
        }
        break;
      case 'a':
        action = model_core::RmtPhvModification::action_for_string(optarg);
        if (action == model_core::RmtPhvModification::ActionEnum::kErr) {
          cli_print(cli, "Value: [%s] for flag -%c is invalid.\n", optarg, c);
          return false;
        }
        break;
      case 'i':
        index = strtoul(optarg, &endp, 10);
        break;
      case 'v':
        value = strtoul(optarg, &endp, 0);
        break;
      default:
        return false;
    }

    if (endp == optarg) {
        cli_print(cli, "-%c needs an arg\n", c);
        return false;
    }
  }
  return true;
}

static int
cmd_rmt_set_phv_modification (struct cli_def *cli, const char *command, char *argv[], int argc) {
  const char usage[] =
      " Sets the PHV modification \n\n"
      " Usage: args: [-d device] [-p pipe] [-s stage] [-w which]\n"
      "              [-a action] [-i PHV index] [-v value]\n\n"
      " Note: this command allows configurations to accumulate. Many \n"
      " bits can be configured to be flipped (using XOR), set (using OR),\n"
      " cleared (using CLR) or masked (using AND). Actions are applied in\n"
      " CLR|OR|XOR order to allow for setting an absolute value.\n"
      " Tagalong PHV words (Tofino1) are not modifiable.\n\n"
      " Options:\n"
      "     -d : specifies the chip number.\n"
      "     -p : specifies a pipe index to configure.\n"
      "     -s : specifies a stage index to configure.\n"
      "     -w : specifies which PHV in the MAU to modify, as a \n"
      "          string i.e.: MATCH, ACTION, OUTPUT (or M, A, O).\n"
      "     -a : specifies the action type as a string i.e.:\n"
      "          XOR, OR, CLR, AND. \n"
      "     -i : specifies the PHV index. Must be 0-223 for Tofino1,\n"
      "          0-279 on Tofino2/Tofino3.\n"
      "     -v : specifies the value to be XOR/OR/AND'ed with PHV word.\n"
      "          The width of values will be truncated to be 8b, 16b or\n"
      "          32b when they are placed into the PHVs used to store\n"
      "          the values. Can be expressed in hexadecimal (-v0xff)\n"
      "          as well as decimal (-v255).\n\n";

  int chip = -1;
  int pipe = -1;
  int stage = -1;

  model_core::RmtPhvModification::ModifyEnum which = model_core::RmtPhvModification::ModifyEnum::kErr;
  model_core::RmtPhvModification::ActionEnum action = model_core::RmtPhvModification::ActionEnum::kErr;
  int index = -1;
  uint32_t value = 0u;

  bool success = parse_modify_cmd_options(
    cli, argv, argc, (char *) "d:p:s:w:a:i:v:",
    chip, pipe, stage, which, action, index, value);
  if (!success) { // Check for parse error
    cli_print(cli, "Parse error, check flag arguments.\n%s", usage);
    return CLI_OK;
  }

  // Map: AND => CLR and ~value
  if (action == model_core::RmtPhvModification::ActionEnum::kAnd) {
    action = model_core::RmtPhvModification::ActionEnum::kClr;
    value = ~value;
  }

  int code = rmt_set_phv_modification(chip, pipe, stage, which, action, index, value);
  const char *msg = "", *out = (code == 0) ?"SUCCESS" :"ERROR", *use = (code == 0) ?"" :usage;
  switch (code) {
    case -1: // global model (rmt.cpp)
      msg = "GLOBAL_MODEL not created properly.";
      break;
    case -2: // chip (model.cpp)
      msg = "Chip (-d) argument is chip.";
      break;
    case -3: // pipe and stage (rmt-object-manager.cpp)
      msg = "Pipe (-p) and/or stage (-s) argument(s) is invalid.";
      break;
    case -4: // which (mau.cpp)
      msg = "Which (-w) argument is invalid.";
      break;
    case -5: // action (phv-modification.cpp)
      msg = "Action (-a) argument is invalid.";
      break;
    case -6: // index (phv-modification.cpp)
      msg = "Index (-i) argument is out of bounds.";
      break;
  }
  cli_print(cli, "RMT_SET_PHV_MODIFICATION %s - EXIT CODE: %i.\n\t%s\n%s",
            out, code, msg, use);

  return CLI_OK;
}
#endif

Cli::Cli() : __cli(cli_init()) {
  cli_set_banner(__cli, "Tofino Model CLI");
  cli_telnet_protocol(__cli, 1);
  cli_regular_interval(__cli, 5); // Defaults to 1 second
  cli_register_command(__cli, NULL, "test", cmd_test, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
  cli_register_command(__cli, NULL, "exit-model", cmd_exit_model,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "exit(0) the model process");
  cli_register_command(__cli, NULL, "mem-dump", cmd_mem_dump,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Dump the actual contents of a memory location of the device");
  cli_register_command(__cli, NULL, "start-pkt-processing",
                       cmd_start_pkt_processing,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Enable packet processing in the model");
  cli_register_command(__cli, NULL, "stop-pkt-processing",
                       cmd_stop_pkt_processing,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Disable packet processing in the model");
  cli_register_command(__cli, NULL, "rmt-set-log-level",
                       cmd_rmt_set_log_level,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Set logging level (rmt-set-log-level -h for more help)");
  cli_register_command(__cli, NULL, "rmt-set-log-flags",
                       cmd_rmt_set_log_flags,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Set log flags (rmt-set-log-flags -h for more help)");
  cli_register_command(__cli, NULL, "rmt-clear-log-flags",
                       cmd_rmt_clear_log_flags,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Clear log flags (rmt-set-log-flags -h for more help)");
  cli_register_command(__cli, NULL, "rmt-set-log-type-flags",
                       cmd_rmt_set_log_type_flags,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "(Deprecated) Set log flags for a given log type (P4=1, TOFINO=2..) Use -h for more help");
  cli_register_command(__cli, NULL, "rmt-clear-log-type-flags",
                       cmd_rmt_clear_log_type_flags,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "(Deprecated) Clear log flags for a given log type (P4=1, TOFINO=2..) Use -h for more help");
  cli_register_command(__cli, NULL, "rmt-get-mau-info",
                       cmd_rmt_get_mau_info,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Get informational counters for a MAU. Use -h for more help");
#ifdef BFN_INTERNAL
  cli_register_command(__cli, NULL, "rmt-set-phv-modification",
                       cmd_rmt_set_phv_modification,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Allow modification of match/action/output PHVs in arbitrary pipe/stage");
#endif
  // XXX: Remove cli command rmt-port-add as it is incompatible with running
  // at a reduced privilege level (CAP_NET_RAW is required to create veths)
  //cli_register_command(__cli, NULL, "rmt-port-add",
  //                     cmd_rmt_port_add,
  //                     PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
  //                     "Add a port to rmt model. Use -h for more help");
  cli_register_command(__cli, NULL, "rmt-version",
                       cmd_rmt_version,
                       PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                       "Display model and register versions");
}

Cli::~Cli() {
  cli_done(__cli);
  StopCli();
}

void
Cli::StartCli(int cli_port,const std::string username, const std::string password) {
  std::thread *t = new std::thread(&Cli::ServerThread, this, cli_port,username,password);
  t->detach();
}

void
Cli::StopCli() {
}

int
Cli::ServerThread(int cli_port,const std::string username, const std::string password) {
  int s, x, on = 1;
  struct sockaddr_in addr;
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
      perror("socket");
      return 1;
  }
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(cli_port);
  if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0)
  {
      perror("bind");
      return 1;
  }

  if (listen(s, 50) < 0)
  {
      perror("listen");
      return 1;
  }

  if (!username.empty() && !password.empty())
  {
      cli_allow_user(__cli, username.c_str(), password.c_str());
  }
  printf("CLI listening on port %d\n", cli_port);
  while ((x = accept(s, NULL, 0)))
  {
    std::thread *t = new std::thread(cli_loop, __cli, x);
    t->detach();
  }

  return 0;
}
