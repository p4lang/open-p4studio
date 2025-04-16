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

#ifndef _table_config_h_
#define _table_config_h_

#include "json-ref.h"
#include "map.h"
#include <set>
#include <string>
#include <vector>

/* per-table layout information, extracted from context.json */


class table_config {
public:
    enum gress_t { INGRESS, EGRESS, GHOST };
    enum memory_type_t { unknown, tcam, sram, map_ram, gateway, ingress_buffer,
                         stm, scm, scm_tind, lamb };
    static memory_type_t memory_type(std::string);
    static memory_type_t memory_type(json::obj_ref o) { return memory_type(o.to<std::string>()); }

    static const table_config *get(const char *name, unsigned &pipes);
    static inline const table_config *get(std::string name, unsigned &pipes) {
        return get(name.c_str(), pipes); }

    enum field_source_t { NONE, ADT_PTR, CONSTANT, IMMEDIATE, INSTRUCTION, METER_PTR,
                          NEXT_TABLE, PARITY, PAYLOAD, PROXY_HASH, RANGE, SEL_LEN, SEL_PTR,
                          SEL_SHIFT, SPEC, STATS_PTR, STFUL_PTR, VALID, VERSION, ZERO };
    static const char *field_source_type[];
    struct field {
        std::string     name;
        field_source_t  source;
        int start_offset,  // msb bit within table word, big-endian numbering
            start_bit,     // lsb bit within field, little-endian numbering
            bit_width;
        bool enable_pfe;   // top bit of the field is actually a pfe
        uint64_t        const_value; };
    typedef std::multimap<std::string, field>   field_map_t;
    struct table_entry {
        field_map_t                             by_name;
        std::multimap<field_source_t, field *>  by_type;
        void clear() { by_name.clear(); by_type.clear(); }
        field *get(field_source_t type) const {
            auto it = by_type.find(type);
            if (it == by_type.end()) return nullptr;
            if (by_type.count(type) > 1)
                error("mulitple %s fields in entry", field_source_type[type]);
            return it->second; }
        MapForKey<const field_map_t> iter(std::string name) const {
            return ValuesForKey(by_name, name); }
        MapForKey<const std::multimap<field_source_t, field *>> iter(field_source_t type) const {
            return ValuesForKey(by_type, type); }
        };

    /// bit matrix -- first index is output bit, col index is offsets in the
    /// field_map_t hash_input used as input to the hash function.
    struct hash_column_t {
        bool                    seed;
        std::vector<uint64_t>   col; };
    typedef std::map<int, hash_column_t>  hash_func_t;

    struct immed_field_remap {
        std::string     name, immed_name, res_name;
        field_source_t  type;
        bool            constant;
        long            constant_value;
        int             immed_bit, start_bit, bit_width;
    };
    typedef std::multimap<std::string, immed_field_remap>       remap_t;
    struct mem {
        // one unit or one vpn
        std::vector<int>        units;
        std::vector<int>        vpn; };
    struct hash_way {
        int                     group;
        int                     entry_lo, entry_bits, subword_bits;
        int                     select_lo, select_bits, select_mask;
        memory_type_t           mtype;
        std::vector<table_config::mem>          mem; };
    struct table_t {
        int                                     width = 0;  // in words
        int                                     bit_width = 0;  // in bits
        int                                     mem_width = 0;  // width of words in bits
        std::vector<table_entry>                fields;  // indexed by entry #
        memory_type_t                           mtype;
        std::vector<table_config::mem>          mem;
        int                                     spare_bank = -1;  // stateful tables
        bool                                    meter_color_aware = false;
        std::map<int, const table_config::mem *>        vpn2mem;
        std::map<uint, int> stateful_alu_per_action;
        explicit operator bool() const { return !fields.empty() && !mem.empty(); }
        int entries_per_word() const { return fields.size(); }
        field *find_field(field_source_t type, int start_bit) const;
        const field *find_field(std::string name) const;
        bool all_zero_fields() const;
        std::vector<field *> find_all_fields(field_source_t type) const;
        int find_meter_type(uint action_handle) const {
            if (stateful_alu_per_action.count(action_handle))
                return stateful_alu_per_action.at(action_handle);
            return -1;
        }
    };
    struct field_override_t {
        field_source_t          type;
        long                    value;
        bool                    pfe;
    };
    struct stage;
    struct action {
        std::string             name;
        int                     code;
        int                     full_address = -1;
        int                     next_table = -1;
        int                     next_table_full = -1;
        int                     next_table_global_exec = 0;
        int                     next_table_local_exec = 0;
        int                     next_table_long_brch = 0;
        table_t                 action_data;
        remap_t                 immed_remap;
        std::vector<field_override_t>   field_override;
        std::set<std::string>   p4_parameters;
        uint                    handle;
        action(json::obj_ref &, int);
    };
    enum type_t { UNKNOWN, MATCH, HASH_MATCH, ATCAM_MATCH, TERNARY_MATCH, MATCH_NO_KEY,
                  PHASE0_MATCH, ACTION_DATA, SELECTOR, STATISTICS, METER, STATEFUL, CONDITION,
                  HASH_ACTION };
    static const char *table_type[];
    struct stage {
        int                                     stage = -1;
        int                                     logical_id = -1;
        int                                     physical_id = -1;
        enum type_t                             type;
        int                                     entries = 0;
        field_map_t                             hash_input;
        std::vector<hash_func_t>                hash_functions;
        std::vector<hash_way>                   ways;
        table_t                                 table, indirect;
        bool                                    adt_indirect = false;  // has an indirect adt
        std::vector<action>                     actions;

        field *find_field(field_source_t type, int start_bit) const;
        const field *find_field(std::string name) const;
        std::vector<field *> find_all_fields(field_source_t type) const;
    private:
        friend class table_config;
        int setup(json::obj_ref, const std::map<int, std::string> &action_handles,
                  const std::map<std::string, int> &action_indexes, json::obj_ref parent);
        void setup_action_data(json::obj_ref);
        int setup_action_data(const table_config &, json::obj_ref);
        void setup_field_aliases(const table_config &); };

    std::string                                 name;
    gress_t                                     direction;
    unsigned                                    pipes;
    enum type_t                                 type;
    int                                         entries = -1;
    std::vector<stage>                          stage_tables;
    std::string                                 binding;
    std::vector<std::string>                    match_tables;
    std::string                                 default_action;
    std::vector<std::string>                    static_entries;
    bool                                        setup_start = false;
    int                                         counter_index = -1;
    int                                         selector_pool_size = -1;

    // Avoid using this constructor; use table_config::get() instead.
    table_config() {}
    field *find_field(field_source_t type, int start_bit) const {
        if (stage_tables.empty()) throw std::runtime_error("no stage tables");
        return stage_tables.front().find_field(type, start_bit); }
    const field *find_field(std::string name) const {
        if (stage_tables.empty()) throw std::runtime_error("no stage tables");
        return stage_tables.front().find_field(name); }

private:
    table_config *setup(json::obj_ref);
    table_config::field_source_t access_type(json::obj_ref obj);
    void bind_to_tables(std::string name, json::obj_ref bindings);
    void setup_action_indirect_resources(json::obj_ref &act, table_config::action &action,
                                         table_config &tbl, table_config::stage &st);
    static std::map<std::string, table_config> all;
    // suffix cache: map values are pointers to all's table_config objects
    static std::map<std::string, table_config *> all_by_suffix;
    friend struct std::pair<const std::string, table_config>;
};

#endif /* _table_config_h_ */
