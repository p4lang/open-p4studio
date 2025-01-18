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

#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <common/rmt.h>
#include <model_core/model.h>
#include <rmt-debug.h>
#include <rmt-object-manager.h>
#include <rmt-packet-coordinator.h>
#include <mau.h>

#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
// FIXME -- should fix config to get this properly
#include "../mimic/utility/include/report.h"
#undef LOGGING
#endif /* !rsvd1 */

#include "bfrt.h"
#include "input.h"
#include "log.h"
#include "p4name.h"
#include "packet.h"
#include "script.h"
#include "table_config.h"
#include "util.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

using namespace MODEL_CHIP_NAMESPACE;

#define FLAGS_TO_SKIP   (RmtDebug::kRmtDebugVerbose|RmtDebug::kRmtDebugMauLogicalRowAddr|\
                         RmtDebug::kRmtDebugDelete)

bool raw_mode = false;
bool logging = false;
int p4log = 0;

bool start_log() {
    fflush(stdout);
    fflush(stderr);
    std::cout << std::flush;
    std::cerr << std::flush;
    std::clog << std::flush;
    LOG2("*** start_log ***");
    bool rv = logging;
    logging = verbose || p4log;
    for (int die = 0; die < PKG_SIZE; die++) {
        if (verbose)
            rmt_update_log_flags(die, ~0UL, ~0UL, ~0UL, ~0UL, ~0UL,
                                 verbose>1 ? ~0UL : ~FLAGS_TO_SKIP, ~0UL);
        if (p4log)
            rmt_update_log_type_levels(die, ~0UL, ~0UL, RMT_LOG_TYPE_P4, 0, ~0UL);
    }
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    if (logging) {
        mimic::utility::report::EnableLogging("Enable_All", true);
        if (verbose <= 7) {
            mimic::utility::report::EnableLogging("Tcams", false);
        }
        // set bus-print
        if (verbose <= 8) {
            mimic::utility::report::SetCompressedValuesFlag(true);
        } else {
            mimic::utility::report::SetCompressedValuesFlag(false);
        }
        // Set trace logging
        if (verbose < 5) {
            mimic::utility::report::EnableLogging("FrameworkBuses", false);  // turns off bus logging completely
        } else if (verbose == 5) {
            mimic::utility::report::SetProduceMode(mimic::utility::report::BusLogMode::kNon_Zero);
            mimic::utility::report::SetConsumeMode(mimic::utility::report::BusLogMode::kNoPrint);
        } else if (verbose == 6) {
            mimic::utility::report::SetProduceMode(mimic::utility::report::BusLogMode::kValue);
            mimic::utility::report::SetConsumeMode(mimic::utility::report::BusLogMode::kNon_Zero);
        } else if (verbose == 7) {
            mimic::utility::report::SetProduceMode(mimic::utility::report::BusLogMode::kShowAll);
            mimic::utility::report::SetConsumeMode(mimic::utility::report::BusLogMode::kValue);
        } else {
            mimic::utility::report::SetProduceMode(mimic::utility::report::BusLogMode::kShowAll);
            mimic::utility::report::SetConsumeMode(mimic::utility::report::BusLogMode::kShowAll);
        }
    }
#endif /* !rsvd1 */
    return rv;
}
bool stop_log() {
    fflush(stdout);
    fflush(stderr);
    std::cout << std::flush;
    std::cerr << std::flush;
    std::clog << std::flush;
    LOG2("*** stop_log ***");
    bool rv = logging;
    logging = false;
    for (int die = 0; die < PKG_SIZE; die++) {
        if (verbose)
            rmt_update_log_flags(die, ~0UL, ~0UL, ~0UL, ~0UL, ~0UL, 0, 0);
        if (p4log)
            rmt_update_log_type_levels(die, ~0UL, ~0UL, RMT_LOG_TYPE_P4, ~0UL, 0);
    }
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    if (logging)
        mimic::utility::report::EnableLogging("Enable_All", false);  // will also turn off bus logging
#endif /* !rsvd1 */
    return rv;
}

static std::map<int, bool> packet_trace_spec;

int add_packet_trace(const char *p) {
    while (isspace(*p)) ++p;
    if (!*p) return -1;
    while (*p) {
        char *e;
        int start = strtol(p, &e, 0);
        int end = start;
        if (e == p) return -1;
        while (isspace(*e)) ++e;
        if (*e == '-') {
            end = strtol((p = e+1), &e, 0);
            if (end < start) return -1;
            if (e == p) return -1;
            while (isspace(*e)) ++e; }
        if (*(p = e) == ',') ++p;
        while (isspace(*p)) ++p;
        LOG4("add " << start << "-" << end);
        auto it = packet_trace_spec.lower_bound(start);
        if (it == packet_trace_spec.begin() || !(--it)->second) {
            LOG4(" add start at " << start);
            packet_trace_spec[start] = true;
        } else
            LOG4(" don't add start at " << start << "; start exists at " << it->first);
        it = --packet_trace_spec.upper_bound(end+1);
        while (!it->second || it->first > start) {
            LOG4(" remove " << (it->second ? "start" : "stop") << " at " << it->first);
            it = --packet_trace_spec.erase(it); }
        it = packet_trace_spec.upper_bound(end+1);
        if (it == packet_trace_spec.end() || it->second) {
            LOG4(" add stop at " << end+1);
            packet_trace_spec[end+1] = false;
        } else {
            LOG4(" don't add stop at " << end+1 << "; stop exists at " << it->first);
        }
    }
    if (LOGGING(2)) {
        std::clog << "packet trace: ";
        bool first = true;
        for (auto &el : packet_trace_spec) {
            if (el.second) {
                if (!first) std::clog << ',';
                std::clog << el.first << '-';
            } else {
                if (first) std::clog << '-';
                std::clog << (el.first-1); }
            first = false; }
        std::clog << std::endl; }
    stop_log();
    return 0;
}

void update_packet_trace()
{
    static int packet_index;
    if (packet_trace_spec.count(++packet_index)) {
        wait_for_idle();
        if (packet_trace_spec.at(packet_index))
            start_log();
        else
            stop_log();
    }
}

const char *load_file(const char *name, unsigned pipes) {
    struct stat fstat;
    LOG1("load_file(" << name << ", " << pipes << "), " << pipe_set_str(pipes));
    if (stat(name, &fstat) >= 0 && S_ISDIR(fstat.st_mode)) {
        char buf[512];
        const char *rv;
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
        snprintf(buf, sizeof(buf), "%s/tofino5.bin", name);
        if ((rv = load_file(buf, pipes))) {
            snprintf(buf, sizeof(buf), "%s/tofino5.bin.gz", name);
            if (!load_file(buf, pipes)) rv = nullptr; }
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
        snprintf(buf, sizeof(buf), "%s/tofino3.bin", name);
        if ((rv = load_file(buf, pipes))) {
            snprintf(buf, sizeof(buf), "%s/tofino3.bin.gz", name);
            if (!load_file(buf, pipes)) rv = nullptr; }
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0)
        snprintf(buf, sizeof(buf), "%s/tofino2.bin", name);
        if ((rv = load_file(buf, pipes))) {
            snprintf(buf, sizeof(buf), "%s/tofino2.bin.gz", name);
            if (load_file(buf, pipes)) {
                snprintf(buf, sizeof(buf), "%s/jbay.bin", name);
                if (load_file(buf, pipes)) {
                    snprintf(buf, sizeof(buf), "%s/jbay.bin.gz", name);
                    if (!load_file(buf, pipes))
                        rv = nullptr;
                } else {
                    rv = nullptr; }
            } else {
                rv = nullptr; } }
#else
        snprintf(buf, sizeof(buf), "%s/tofino.bin", name);
        if ((rv = load_file(buf, pipes))) {
            snprintf(buf, sizeof(buf), "%s/tofino.bin.gz", name);
            if (!load_file(buf, pipes)) rv = nullptr; }
#endif
        if (rv) {
            snprintf(buf, sizeof(buf), "%s/manifest.json", name);
            if (!load_file(buf, pipes))
                rv = nullptr;
            return rv; }
        snprintf(buf, sizeof(buf), "%s/context.json", name);
        if (load_file(buf, pipes)) {
            snprintf(buf, sizeof(buf), "%s/tbl-cfg", name);
            load_file(buf, pipes); }
        return rv;
    } else if (FILE *fp = fopen(name, "rb")) {
        char magic[4];
        if (fread(magic, sizeof(magic), 1, fp) != 1) {
            fclose(fp);
            return "Unknown file format";
        } else if (magic[0] == 0 && (magic[3] == 'R' || magic[3] == 'D' || magic[3] == 'H' || magic[3] == 'r' || magic[3] == 'd')) {
            LOG2("loading binary image from " << name << ", pipes: " << pipes);
            rewind(fp);
            bool log = verbose < 3 ? stop_log() : 0;
            if (load_cfg_blob(fp, GLOBAL_MODEL.get(), pipes) < 0) {
                fclose(fp);
                return "Error reading"; }
            if (!raw_mode) {
                reinit_all_ports();
                InitP4Tables(pipes);
            }
            if (log) start_log();
            fclose(fp);
        } else if (magic[0] == 0x1f && (magic[1] & 0xff) == 0x8b) {
            LOG2("loading gzipped binary from " << name);
            fclose(fp);
            fp = popen((std::string("zcat < ") + name).c_str(), "r");
            bool log = verbose < 3 ? stop_log() : 0;
            if (load_cfg_blob(fp, GLOBAL_MODEL.get(), pipes) < 0) {
                pclose(fp);
                return "Error reading"; }
            reinit_all_ports();
            InitP4Tables(pipes);
            if (log) start_log();
            pclose(fp);
        } else if (magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F') {
            LOG2("loading elf binary from " << name);
            fclose(fp);
            if (/*auto *lib =*/ dlopen(name, RTLD_NOW | RTLD_GLOBAL)) {
                /* do something? */
            } else return "Error reading";
        } else if (magic[0] == '[' || magic[0] == '{') {
            LOG2("loading json from " << name);
            fclose(fp);
            if ((pipes != ALL_PIPES || LoadConf(name) < 0) && LoadP4Json(name, pipes) < 0) {
                return "Error reading"; }
        } else {
            return "Unknown file format";
            fclose(fp);
        }
    } else return "Can't open";
    return 0;
}

ScriptCommand(load, "<file>") {
    while (char *name = token(&args)) {
        if (auto err = load_file(name, ALL_PIPES))
            error("%s %s", err, name); }
}

int rmt_log_fn(int chip, int pipe, const char *buffer) {
    if (logging)  {
        fflush(stdout);
        fflush(stderr);
        std::cout << std::flush;
        std::cerr << std::flush;
        std::clog << std::flush;
        fprintf(stderr, "%s", buffer);
        fflush(stderr); }
    return 0;
}

void sth_init(void) {
    uint8_t chip_id = 0;
    rmt_init(PKG_SIZE);
    rmt_pcie_veth_init(true);
    GLOBAL_MODEL->DestroyAllChips();
    bool ok = rmt_create_package(chip_id, RmtDefs::kChipType, PKG_SIZE);
    if (!ok) {
        fprintf(stderr, "Could not create package for chip id: %d, chip type: %d, package size: %d\n",
                chip_id, RmtDefs::kChipType, PKG_SIZE);
        exit(1);
    }
    rmt_set_log_fn(rmt_log_fn);
    for (int die = 0; die < PKG_SIZE; die++) {
        RmtObjectManager *om = NULL;
        GLOBAL_MODEL->InitChip(die);
        GLOBAL_MODEL->GetObjectManager(die, &om);
        om->set_log_fn(rmt_log_fn);
    }
    rmt_transmit_register(packet_output);
    // for now, trace chip 0, pipe 0 (only), as that is all that works
    GLOBAL_MODEL->SetTrace(0, 0, true);
    GLOBAL_MODEL->SetLogDir(".");
    GLOBAL_MODEL->ContinueOnConfigErrors(true);
}

int sth_uninit(void) {
    wait_for_idle();
    int err = check_missing_expected();
    rmt_stop_packet_processing();
    stop_log();
    rmt_uninit();
    return err;
}

int main(int ac, char **av) {
    std::string options_msg("[-vpL:T:P:] [-l file]"
                            " [--observation-log file] [--parser-other-gress-fill] script...");
    int err = 0;
#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino) || MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)
    setenv("TOFINO", "1", 1);
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0)
    setenv("TOFINO", "2", 1);
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    setenv("TOFINO", "3", 1);
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    setenv("TOFINO", "5", 1);
#else
#error "unknown model" MODEL_CHIP_NAMESPACE
#define STRINGIFY_RAW(X)  #X
#define STRINGIFY(X) STRINGIFY_RAW(X)
#pragma message "model: " STRINGIFY(MODEL_CHIP_NAMESPACE)
#endif
    sth_init();
    for (int i = 1; i < ac; i++) {
        if (av[i][0] == '-' && av[i][1] == 0) {
            err += Script::run("<stdin>", std::cin);
        } else if (av[i][0] == '-' && av[i][1] == '-') {
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            if (!strcmp(av[i], "--disable-din-power")) {
                Mau::kMauDinPowerMode = false;
            } else
#endif // !WIP
            if (!strcmp(av[i], "--observation-log")) {
                if (i + 1 >= ac) {
                    std::cerr << "Missing argument to '--observation-log'"
                              << std::endl;
                    err++;
                }
                char * obsLog = av[++i];
                if (!set_observationLog(obsLog)) {
                    std::cerr << "Cannot open observation log file '"
                              << obsLog << "'" << std::endl;
                    err++;
                }
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            } else if (!strcmp(av[i], "--parser-other-gress-fill")) {
                RmtPacketCoordinator::kProcessGressesSeparately = true;
                ParserShared::kPhvFillOtherGress = true;
#endif
            } else {
                std::cerr << "Unknown option " << av[i] << std::endl
                          << "usage: " << av[0] << " " << options_msg << std::endl;
                    err++; }
        } else if (av[i][0] == '-' || av[i][0] == '+') {
            bool flag = av[i][0] == '+';
            for (const char *arg = av[i]+1; *arg;)
                switch (*arg++) {
                case 'l': {
                    unsigned pipes = ALL_PIPES;
                    if (isdigit(*arg)) {
                        pipes = 0;
                        // TODO:
                        // This can work for up to 10 pipes (0-9).
                        // When adding more pipes, we need to change this.
                        while (isdigit(*arg)) {
                            if (*arg - '0' >= MAX_PIPE_COUNT) {
                                std::cerr << "pipe " << *arg << " out of range (max " <<
                                        (MAX_PIPE_COUNT - 1) << ")" << std::endl;
                                err++;
                            } else {
                                pipes |= 1U << (*arg - '0'); }
                            arg++; } }
                    if (pipes >= 0) {
                        if (auto msg = load_file(av[++i], pipes)) {
                            std::cerr << msg << " " << av[i] << std::endl;
                            err++; } }
                    break; }
                case 'L':
                    GLOBAL_MODEL->SetLogDir(*arg ? arg : av[++i]);
                    arg="";
                    break;
                case 'v': verbose++; start_log(); break;
                case 'p': p4log++; start_log(); break;
                case 'T':
                    add_debug_spec(*arg ? arg : av[++i]);
                    arg="";
                    break;
                case 'P':
                    if (add_packet_trace(*arg ? arg : av[++i]) < 0)
                        std::cerr << "Invalid packet trace: " << (*arg ? arg : av[i]) << std::endl;
                    arg="";
                    break;
                case 'r': raw_mode = true; break;
                default:
                    std::cerr << "Unknown option " << (flag ? '+' : '-') << arg[-1] << std::endl
                              << "usage: " << av[0] << " " << options_msg << std::endl;
                    err++; }
        } else {
            std::ifstream in(av[i]);
            if (in) err += Script::run(av[i], in);
            else {
                std::cerr << "Can't open " << av[i] << std::endl;
                err++; } } }
    if (err > 0)
        _exit(1);
    err += sth_uninit();
    return err > 0;
}
