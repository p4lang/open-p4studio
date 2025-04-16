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
#include "json.h"
#include "json-ref.h"
#include "log.h"
#include "map.h"
#include "p4name.h"
#include "script.h"
#include "table_config.h"
#include "util.h"
#include "write.h"

#include <common/rmt.h>
#include <model_core/model.h>
#include <register_utils.h>
#include <rmt-debug.h>
#include <rmt-object-manager.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

using namespace MODEL_CHIP_NAMESPACE;

struct pipe_config_t {
    unsigned                                 pipes;
    std::unique_ptr<json::obj>               context_json;
    std::map<std::string, json::map *>       tables_by_name;
    json::obj_ref                            stage_extend;
};
static std::vector<pipe_config_t>     pipe_config;

void ScanP4Tables(json::vector *tables, unsigned pipes, std::unique_ptr<json::obj> context_json,
                  json::obj_ref stage_extend = nullptr) {
    for (auto it = pipe_config.begin(); it != pipe_config.end();) {
        if ((it->pipes &= ~pipes) == 0)
            it = pipe_config.erase(it);
        else
            ++it; }
    pipe_config.emplace_back(pipe_config_t{ pipes, std::move(context_json), {}, stage_extend });
    auto &tables_by_name = pipe_config.back().tables_by_name;
    for (json::obj_ref tbl : *tables) {
        if (tbl["table_type"] == "condition") {
            // FIXME -- ignore condition tables as they get duplicated and we don't care about them
            continue; }
        if (tbl["name"]) {
            std::string name = tbl["name"]->to<std::string>();
            if (tables_by_name.count(name)) {
                if (tbl["table_type"] == "selection") {
                    // P4_14 selector tables have the same name as the action_profile, which
                    // causes problems.  We add a $sel suffix on the selector name
                    name += "$sel";
                } else if ((*tables_by_name.at(name))["table_type"] == "selection") {
                    tables_by_name[name + "$sel"] = tables_by_name[name];
                } else {
                    error("Duplicate table %s in context.json (%s and %s)", name.c_str(),
                          tbl["table_type"]->c_str(),
                          (*tables_by_name.at(name))["table_type"]->c_str()); } }
            tables_by_name[name] = tbl->as_map(); }
    }
    InitP4Tables(pipes);
}

static void InitDefaultActions(const pipe_config_t &pipe) {
    std::stringstream default_init;
    for (auto tbl : pipe.tables_by_name) {
        unsigned pipes = pipe.pipes;
        if (auto cfg = table_config::get(tbl.first.c_str(), pipes)) {
            if (!cfg->stage_tables.empty() &&
                cfg->stage_tables.back().type != table_config::MATCH_NO_KEY &&
                !cfg->default_action.empty()) {
                LOG3("default action for table " << cfg->name << ": " << cfg->default_action);
                default_init << "setdefault " << tbl.first << ' ' << cfg->default_action << '\n'; }
            if (cfg->counter_index >= 0) {
                for (int p = 0; (1U << p) <= pipe.pipes; p++) {
                    if (!((1U << p) & pipe.pipes)) continue;
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
                    int die = PIPE2DIE(p);
                    int phy_pipe = PIPE_WITHIN_DIE(p);
                    // FIXME -- differently named regs on flatorck
                    auto &merge = RegisterUtils::ref_mau(phy_pipe, cfg->stage_tables.front().stage)
                            .rams.match.merge;
                    LOG3("clearing stateful counter " << cfg->counter_index << " for table " <<
                         cfg->name);
                    write_reg(die, &merge.mau_stateful_log_counter_clear, 1 << cfg->counter_index);
#endif
                }
            }
        }
    }
    if (!default_init.str().empty())
        Script::run("<tbl-cfg>", default_init);
}

static void InitStaticEntries(const pipe_config_t &pipe) {
    std::stringstream static_entries_init;
    for (auto tbl : pipe.tables_by_name) {
        unsigned pipes = pipe.pipes;
        if (auto cfg = table_config::get(tbl.first.c_str(), pipes)) {
            for (auto& ent : cfg->static_entries)
                static_entries_init << "add " << tbl.first << ' ' << ent << '\n'; } }
    if (!static_entries_init.str().empty())
        Script::run("<tbl-cfg>", static_entries_init);
}

static void FillExtendedStages(json::obj_ref extend, int pipe) {
    try {
        auto &om = GLOBAL_MODEL;
        int die = PIPE2DIE(pipe);
        int phy_pipe = PIPE_WITHIN_DIE(pipe);
        int last_programmed_stage = extend["last_programmed_stage"];
        int last_stage = om->GetNumStages(die) - 1;
        if (last_stage <= last_programmed_stage) {
            if (last_stage < last_programmed_stage)
                error("program requires %d stages and hardware only has %d!",
                      last_programmed_stage+1, last_stage+1);
            return; }
        LOG1("filling stages " << (last_programmed_stage+1) << ".." << last_stage <<
             " in pipe " << pipe << " (die " << die << ")");
        auto &regs = extend["registers"]->to<json::vector>();
        for (int stage = last_programmed_stage; stage <= last_stage; ++stage) {
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            int mask_index = stage == last_programmed_stage ? 0 : stage < last_stage ? 1 : 2;
            // FIXME -- differently named regs on flatorck
            auto &base = RegisterUtils::ref_mau(phy_pipe, stage);
            for (json::obj_ref reg : regs) {
                uint32_t mask = reg["mask"][mask_index];
                if (mask == 0) continue;
                uint32_t offset = reg["offset"];
                for (auto &val : reg["value"].to<json::vector>()) {
                    om->OutWord(die, (char *)&base + offset, val->to<json::number>().val & mask);
                    offset += 4; } }
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,tofino) && !MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)
            if (stage > last_programmed_stage)
                om->OutWord(die, &base.rams.match.merge.pred_stage_id, stage);
#endif
#endif
        }
    } catch (std::bad_cast& ex) {
        error("corrupt mau_stage_extension json");
    }
}

void InitP4Tables(unsigned pipes) {
    for (auto &pc : pipe_config) {
        if ((pc.pipes & pipes) != 0) {
            InitDefaultActions(pc);
            InitStaticEntries(pc);
            if (pc.stage_extend) {
                unsigned mask = pc.pipes & pipes;
                for (int p = 0; mask; ++p, mask >>= 1) {
                    if (mask & 1)
                        FillExtendedStages(pc.stage_extend, p); } } } }
}

namespace {

// returns true iff 'str' ends with 'suffix'
bool endsWith(const std::string &str, const std::string &suffix) {
    return (str.size() >= suffix.size()) &&
        !str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

}  // namespace

json::map *P4TableConfig(const char *unique_suffix, unsigned &pipes) {
    for (auto &pc : pipe_config) {
        if ((pc.pipes & pipes) == 0) continue;
        auto it = pc.tables_by_name.find(unique_suffix);
        if (it != pc.tables_by_name.end()) {
            pipes = pc.pipes;
            LOG3("found exact match for table " << unique_suffix << " in " << pipe_set_str(pipes));
            return it->second; } }
    json::map *candidate = nullptr;
    unsigned candidate_pipes = 0;
    for (auto &pc : pipe_config) {
        if ((pc.pipes & pipes) == 0) continue;
        for (const auto &p : pc.tables_by_name) {
            if (!endsWith(p.first, unique_suffix)) continue;
            // name is a match for suffix
            LOG3("found suffix " << unique_suffix << " match for table " << p.first <<
                 " in " << pipe_set_str(pc.pipes));
            if (candidate != nullptr) {
                error("Suffix '%s' is not unique", unique_suffix);
                pipes = 0;
                return nullptr; }
            candidate_pipes = pc.pipes;
            candidate = p.second; } }
    pipes = candidate_pipes;
    return candidate;
}

int LoadP4Json(const char *fname, unsigned pipes) {
    std::unique_ptr<json::obj> json;
    std::ifstream in(fname);
    LOG1("Reading context.json from " << fname);
    if (!in) return -1;
    in >> json;
    if (!in) return -1;
    if (auto *m = json->as_map()) {
        if (m->count("tables")) {
            for (int die = 0; die < PKG_SIZE; die++) {
                RmtObjectManager *om = NULL;
                GLOBAL_MODEL->GetObjectManager(die, &om);
                for (int pipe = 0; pipe < RmtDefs::kPipesMax; ++pipe)
                    if (pipes & (1U << (pipe + die * RmtDefs::kPipesMax))) {
                        LOG1("Configuring pipe " << pipe << " in package " << die <<
                                " (logical pipe " << pipe + die * RmtDefs::kPipesMax << ")");
                        om->ConfigureP4NameLookup(pipe, fname);
                    }
            }
        }
    }
    return LoadP4Json(std::move(json), pipes);
}

int LoadP4Json(std::unique_ptr<json::obj> &&json, unsigned pipes) {
    if (auto *tables = json->as_vector()) {
        json::vector *t;
        while (!tables->empty() && (t = tables->at(0)->as_vector()))
            tables = t;
        LOG1("Reading raw tables json for " << pipe_set_str(pipes));
        ScanP4Tables(tables, pipes, std::move(json));
    } else if (auto *m = json->as_map()) {
        if (m->count("tables")) {
            if (auto *tables = (*m)["tables"]->as_vector()) {
                LOG1("Reading context.json tables for " << pipe_set_str(pipes));
                ScanP4Tables(tables, pipes, std::move(json), (*m)["mau_stage_extension"].get()); }
        } else if (m->count("ContextJsonNode")) {
            if (auto *tables = (*m)["ContextJsonNode"]->as_vector()) {
                LOG1("Reading ContextJsonNode for " << pipe_set_str(pipes));
                json::vector *t;
                while (!tables->empty() && (t = tables->at(0)->as_vector()))
                    tables = t;
                ScanP4Tables(tables, pipes, std::move(json)); }
        } else
            return -1;
    } else
        return -1;
    return 0;
}
