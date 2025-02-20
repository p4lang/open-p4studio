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

#include "log.h"
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>

int verbose = 0;
static std::vector<std::string> debug_specs;
static std::set<int *> log_levels;

static bool match(const char *pattern, const char *name) {
    const char *pend = pattern + strcspn(pattern, ",:");
    const char *pbackup = 0;
    while (1) {
        while (pattern < pend && *pattern == *name) {
            pattern++;
            name++; }
        if (pattern == pend) {
            if (!strcmp(name, ".cpp")) return true;
            return *name == 0; }
        if (*pattern++ != '*') return false;
        if (pattern == pend) return true;
        while (*name && *name != *pattern) {
            if (pbackup && *name == *pbackup) {
                pattern = pbackup;
                break; }
            name++; }
        pbackup = pattern;
    }
}

int get_file_log_level(const char *file, int *level) {
    if (auto *p = strrchr(file, '/'))
        file = p+1;
    log_levels.insert(level);
    for (auto &s : debug_specs)
        for (auto *p = s.c_str(); p; p = strchr(p, ',')) {
            while (*p == ',') p++;
            if (match(p, file))
                if (auto *l = strchr(p, ':'))
                    return *level = atoi(l+1); }
    return *level = 0;
}

void add_debug_spec(const char *spec) {
    bool ok = false;
    for (const char *p = strchr(spec, ':'); p; p = strchr(p, ':')) {
        ok = true;
        strtol(p+1, const_cast<char **>(&p), 10);
        if (*p && *p != ',') {
            ok = false;
            break; } }
    if (!ok)
        std::cerr << "Invalid debug trace spec '" << spec << "'" << std::endl;
    else {
        debug_specs.push_back(spec);
        for (auto *l : log_levels)
            *l = -1;
    }
}

std::ostream &operator<<(std::ostream &out, const output_log_prefix &pfx) {
#if 1
    const char *s = strrchr(pfx.fn, '/');
    const char *e = strrchr(pfx.fn, '.');
    s = s ? s + 1 : pfx.fn;
    if (e && e > s)
        out.write(s, e-s);
    else
        out << s;
    out << ':' << pfx.level << ':';
#endif
    return out;
}
