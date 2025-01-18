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

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <string.h>
#include "json.h"
#include "json-ref.h"
#include "bfrt.h"
#include "map.h"

const char *load_file(const char *name, unsigned pipes);

int ScanPipelines(json::vector *pipelines) {
    int rv = 0;
    for (json::obj_ref pipe : *pipelines) {
        if (auto scopes = pipe["pipe_scope"]) {
            unsigned pipes = 0;
            for (json::obj_ref scope : *scopes->as_vector()) {
                pipes |= 1U << scope->as_number()->val;
            }
            auto config = pipe["config"]->to<std::string>();
            if (auto err = load_file(config.c_str(), pipes)) {
                if (load_file((config+".gz").c_str(), pipes)) {
                    std::cerr << err << " " << config << std::endl;
                    rv = -1; } }
            auto context = pipe["context"]->to<std::string>();
            if (auto err = load_file(context.c_str(), pipes)) {
                std::cerr << err << " " << context << std::endl;
                rv = -1; }
        }
    }
    return rv;
}

int ScanPrograms(json::vector *programs) {
    int rv = 0;
    for (json::obj_ref prog : *programs) {
        if (auto pipelines = prog["p4_pipelines"]->as_vector()) {
            rv |= ScanPipelines(pipelines);
        }
    }
    return rv;
}

int ScanDevices(json::vector *devices) {
    int rv = 0;
    for (json::obj_ref dev : *devices) {
        if (auto programs = dev["p4_programs"]->as_vector()) {
            rv |= ScanPrograms(programs);
        }
    }
    return rv;
}

int LoadConf(const char *fname) {
    std::unique_ptr<json::obj> json;
    std::ifstream in(fname);
    if (!in) return -1;
    in >> json;
    if (!in) return -1;

    if (auto *m = json->as_map()) {
        if (m->count("p4_devices")) {
            if (auto *devices = (*m)["p4_devices"]->as_vector()) {
                return ScanDevices(devices); }
        } else if (m->count("conf_file")) {
            if (auto *conf = (*m)["conf_file"]->as_string()) {
                auto *p = strrchr(fname, '/');
                if (p && conf->at(0) != '/') {
                    std::string dir(fname, p - fname + 1);
                    return LoadConf(dir + *conf); }
                return LoadConf(*conf); }
        } else {
            return -1; }
    } else {
        return -1; }
    return 0;
}
