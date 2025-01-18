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

#include <mau.h>
#include <p4-name-lookup.h>
#include <phv.h>
#include <mau-logical-table.h>
#include <address.h>

#include <common/rmt-assert.h>
#include <rapidjson/document.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <sstream>
#include <unordered_map>
#include <tuple>

using rapidjson::Document;
using rapidjson::FileReadStream;
using rapidjson::Value;
using rapidjson::SizeType;

namespace {
  using MODEL_CHIP_NAMESPACE::Phv;
  using MODEL_CHIP_NAMESPACE::MauLookupResult;
  using MODEL_CHIP_NAMESPACE::Mau;
  using MODEL_CHIP_NAMESPACE::MauLogicalTable;
  using MODEL_CHIP_NAMESPACE::Address;
  using MODEL_CHIP_NAMESPACE::P4TableCache;
  using model_core::FindMember;
  using model_core::FindMemberArray;
  static constexpr char kStageNumberKey[]         = "stage_number";
  static constexpr char kTablesKey[]              = "tables";
  static constexpr char kTableTypeKey[]           = "table_type";
  static constexpr char kMatchAttrKey[]           = "match_attributes";
  static constexpr char kStageTablesKey[]         = "stage_tables";
  static constexpr char kLogicalTableIdKey[]      = "logical_table_id";
  static constexpr char kMatchKeyFieldsKey[]      = "match_key_fields";
  static constexpr char kMatchTypeKey[]           = "match_type";
  static constexpr char kPhvAllocationKey[]       = "phv_allocation";
  static constexpr char kTableTypeMatchKey[]      = "match";
  static constexpr char kTableTypeSelectionKey[]  = "selection";
  static constexpr char kTableTypeStatsKey[]      = "statistics";
  static constexpr char kTableTypeActionKey[]     = "action";
  static constexpr char kTableTypeMeterKey[]      = "meter";
  static constexpr char kTableTypeStatefulKey[]   = "stateful";
  static constexpr char kTableTypeConditionKey[]  = "condition";
  static constexpr char kRecordsKey[]             = "records";
  static constexpr char kFieldNameKey[]           = "field_name";
  static constexpr char kPhvNumberKey[]           = "phv_number";
  static constexpr char kFieldLsbKey[]            = "field_lsb";
  static constexpr char kFieldMsbKey[]            = "field_msb";
  static constexpr char kPhvLsbKey[]              = "phv_lsb";
  static constexpr char kPhvMsbKey[]              = "phv_msb";
  static constexpr char kParserKey[]              = "parser";
  static constexpr char kParsersKey[]             = "parsers";
  static constexpr char kPovHeadersKey[]          = "pov_headers";
  static constexpr char kIsPovKey[]               = "is_pov";
  static constexpr char kPovHeaderNameKey[]       = "header_name";
  static constexpr char kPovBitIndexKey[]         = "bit_index";
  static constexpr char kIngressKey[]             = "ingress";
  static constexpr char kEgressKey[]              = "egress";

  static const char *gress_key(bool egress) { return egress ? kEgressKey : kIngressKey; }


  // we need to detect exceptions here because rapidjson will fail assertions instead of throw exceptions
  bool ExpectTrue(bool expect_true, const char* message) {
    if (!expect_true) {
      throw std::invalid_argument(message);
    }
    return expect_true;
  }

  const Value* GetNestedMatchTable(const Value& table, const int stage, const int table_idx) {
    const char* table_type = FindMember(table, kTableTypeKey).GetString();
    if (strcmp(table_type, "match") != 0) {
      throw "Not a match table!";
    }
    auto& match_attr = FindMember(table, kMatchAttrKey);
    const char* match_type = FindMember(match_attr, kMatchTypeKey).GetString();
    if (match_attr.HasMember(kStageTablesKey)) {
      auto& stage_tables = FindMemberArray(match_attr, kStageTablesKey);
      for (auto& stage_table : stage_tables.GetArray()) {
        int logical_table_id = -1;
        if (stage_table.HasMember(kLogicalTableIdKey)) {
          logical_table_id = stage_table[kLogicalTableIdKey].GetInt();
        }
        int stage_number = FindMember(stage_table, kStageNumberKey).GetInt();
        if (stage_number == stage && logical_table_id == table_idx) {
          return &table;
        }
      }
    }
    const Value* result = nullptr;
    if (strcmp(match_type, "algorithmic_tcam") == 0) {
      auto& units = FindMemberArray(match_attr, "units");
      for (auto& unit : units.GetArray()) {
        result = GetNestedMatchTable(unit, stage, table_idx);
        if (result != nullptr) {
          return result;
        }
      }
    }
    if (strcmp(match_type, "algorithmic_lpm") == 0) {
      result = GetNestedMatchTable(FindMember(match_attr, "pre_classifier"), stage, table_idx);
      if (result != nullptr) {
        return result;
      }
      result = GetNestedMatchTable(FindMember(match_attr, "atcam_table"), stage, table_idx);
      if (result != nullptr) {
        return result;
      }
    }
    if (strcmp(match_type, "chained_lpm") == 0) {
      auto& units = FindMemberArray(match_attr, "units");
      for (auto& unit : units.GetArray()) {
        result = GetNestedMatchTable(unit, stage, table_idx);
        if (result != nullptr) {
          return result;
        }
      }
    }
    return nullptr;
  }

  /**
   *  Get the table in context using stage & id (& table type)
   */
  const Value& GetTable(const Document& context, P4TableCache& tableCache_, const int stage,
                        const int table_idx, const char* table_type_key = kTableTypeMatchKey) {
    // Search the cache first
    auto lookup = std::make_tuple(stage, table_idx, std::string(table_type_key));
    auto cachedTable = tableCache_.find(lookup);
    if (cachedTable != tableCache_.end()) {
      return cachedTable->second;
    }
    // Cache miss! Search context.json
    auto& tables = FindMemberArray(context, kTablesKey);
    for (auto& table : tables.GetArray()) {
      ExpectTrue(table.IsObject(), "Expected table in table array to be an object");
      const char* table_type = FindMember(table, kTableTypeKey).GetString();
      if (strcmp(table_type, table_type_key) == 0) {
        // match tables are a special case
        if (strcmp(table_type_key, kTableTypeMatchKey) == 0) {
          const Value* match_table = GetNestedMatchTable(table, stage, table_idx);
          if (match_table != nullptr) {
            tableCache_.emplace(lookup, *match_table);
            return *match_table;
          }
        } else { // non-match table types
          auto& stage_tables = FindMemberArray(table, kStageTablesKey);
          for (auto& stage_table : stage_tables.GetArray()) {
            int logical_table_id = FindMember(stage_table, kLogicalTableIdKey).GetInt();
            int stage_number = FindMember(stage_table, kStageNumberKey).GetInt();
            if (stage_number == stage && logical_table_id == table_idx) {
              auto emplaced_table = tableCache_.emplace(lookup, table);
              return emplaced_table.first->second;
            }
          }
        }
      }
    }
    std::stringstream err;
    err << "Could not find table in context (stage: " << stage << ", table " << table_idx << ")";
    throw std::invalid_argument(err.str());
  }

  /**
   *  Search for a table in context using stage and id (searches all table types)
   *  There can be many stage tables at a given address - this function returns the first
   *  one that it finds.
   */
  const Value& FindTable(const Document& context, P4TableCache& tableCache_, const int stage, const int table_idx) {
    const std::array<const char*, 7> table_types = {{
      kTableTypeMatchKey,
      kTableTypeSelectionKey,
      kTableTypeStatsKey,
      kTableTypeMeterKey,
      kTableTypeStatefulKey,
      kTableTypeActionKey,
      kTableTypeConditionKey
    }};
    // Loop through all table types in the cache before referring to context.json
    // It's much quicker to search cache first for all table types. E.g.
    //  from left to right is time passing while we search for the table.
    //    c: cache search time period
    //    j: json search time period
    //  1) Cache first
    //    |c|c|c|c|c|c|c|  j  |c|  j  |c|  j  |c|  j  | ...etc
    //  2) GetTable() everytime
    //    |c|  j  |c|  j  |c|  j  |c|  j  |c|  j  | ...etc
    //  Hopefully this illustrates why, with a 'hot' cache, searching the cache
    //  for all table types first is quicker than looping GetTable() which
    //  also searches the cache.
    for (auto table_type : table_types) {
      auto lookup = std::make_tuple(stage, table_idx, std::string(table_type));
      auto cachedTable = tableCache_.find(lookup);
      if (cachedTable != tableCache_.end()) {
        return cachedTable->second;
      }
    }
    for (auto table_type : table_types) {
      try {
        return GetTable(context, tableCache_, stage, table_idx, table_type);
      } catch (std::invalid_argument&) {}
    }
    std::stringstream err;
    err << "Could not find table in context (stage: " << stage << ", table " << table_idx << ")";
    throw std::invalid_argument(err.str());
  }


  std::string DecStrToHexStr(const char* dec_str) {
    char* end;
    long dec_str_value = strtol(dec_str, &end, 10);
    std::stringstream hex;
    hex << std::uppercase << std::hex << dec_str_value;
    std::string hex_str = hex.str();
    return hex_str;
  }

}  // namespace


namespace MODEL_CHIP_NAMESPACE {

  /**
   *  Attempt to load and parse a context.json file into a DOM for querying.
   *  On failure, we continue anyway and queries will return error messages instead.
   */
  P4NameLookup::P4NameLookup(const std::string &file_name) :
    JSONLoader(file_name, "context file", kMaxContextFileSize, check_loaded_doc) { }

  P4NameLookup::~P4NameLookup() {
    for ( P4PhvContainerMap map : phv_containers_ ) {
      for ( auto it : map ) delete it.second;
    }
  }

  /**
   * Perform check that document root is an object.
   */
  const bool P4NameLookup::check_loaded_doc(std::unique_ptr<rapidjson::Document>& document) {
    if (!document->IsObject()) {
      std::cerr << "ERROR: Invalid context. Expected object at root." << std::endl;
      return false;
    }
    return true;
  }

  /**
   *  Get the match fields for a given table and their values in a given PHV
   *  field_name[msb:lsb] = 0x0000
   *  field_name[msb:lsb] = 0x0000
   */
  std::list<std::string> P4NameLookup::GetMatchFields(const int &stage,
                                                      const int &table_idx,
                                                      const Phv &phv,
                                                      const bool egress) const {
    std::list<std::string> field_names;
    std::list<std::string> match_fields;
    if (!loaded_) return match_fields;

    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx);
      // We're going to use match key fields to list field names for the matching table
      auto& match_key_fields = FindMemberArray(table, kMatchKeyFieldsKey);
      for (auto& match_key_field : match_key_fields.GetArray()) {
        // If there is a global name, the other name is an alias & is not what we want.
        auto global_name = match_key_field.FindMember("global_name");
        if (global_name != match_key_field.MemberEnd())  {
          field_names.push_back(global_name->value.GetString());
        } else {
          field_names.push_back(match_key_field["name"].GetString());
        }
      }
      field_names.sort();

      LoadPhvContainers(stage, egress);
      P4PovHeaderMap *pov_header_map = &phv_pov_headers_.at(stage).at(egress);
      for (const std::string& field_name : field_names) {
        auto it = pov_header_map->find(field_name);
        if (it != pov_header_map->end()) {
          auto pov_header = it->second;
          RMT_ASSERT(nullptr != pov_header);
          RMT_ASSERT(nullptr != pov_header->GetRecord());
          std::stringstream ss;
          ss << std::dec;
          ss << "\t" << std::string("--validity_check--").append(field_name);
          ss << " = 0x" <<std::uppercase << std::hex;
          ss << phv.get_field_x(pov_header->GetRecord()->GetContainer()->GetPhvNumber(),
                                pov_header->GetBitIndex(),
                                pov_header->GetBitIndex())
                                << std::endl;
          ss << std::dec;
          match_fields.push_back(ss.str());
        } else {
          DumpPhvFieldInfo(stage, phv, egress, field_name, match_fields);
        }
      }
    } catch (std::invalid_argument& ia) {
      // we probably failed to parse context.json
      match_fields.push_back(ia.what());
    }
    return match_fields;
  }

  /**
   * Get pointer to a P4PhvContainer that encapsulates details of the PHV
   * container for the given stage and container index.
   * @param stage - index of stage
   * @param container_index - index of container
   * @return - pointer to a P4PhvContainer; may be nullptr if no P4 context
   *     is found for the given stage and container.
   * @throws - std::invalid_argument if invalid context is encountered
   */
  const P4PhvContainer* P4NameLookup::GetPhvContainer(
      const int &stage,
      const int &container_index) const {
    if (!loaded_) return nullptr;

    P4PhvContainerMap *cacheMap = &phv_containers_.at(stage);
    auto cached = cacheMap->find(container_index);
    if (cached != cacheMap->end()) {
      return cached->second;
    }

    P4PhvContainer *container = ParsePhvContainer(stage, container_index);
    if (nullptr != container) cacheMap->emplace(container_index, container);
    return container;
  }

  /**
   * Same as GetPhvContainer but will return a nullptr instead of throwing
   * std::invalid_argument if invalid context is encountered.
   */
  const P4PhvContainer* P4NameLookup::GetPhvContainerSafe(
      const int &stage,
      const int &container_index) const {
    try {
      return GetPhvContainer(stage, container_index);
    } catch (std::invalid_argument&) {
      // squash exception
      return nullptr;
    }
  }

P4PovHeader* P4NameLookup::ParsePovHeader(const rapidjson::Value &header_json) const {
  RMT_ASSERT(header_json.IsObject());
  std::string header_name = FindMember(header_json, kPovHeaderNameKey).GetString();
  int bit_index = FindMember(header_json, kPovBitIndexKey).GetInt();
  return new P4PovHeader(header_name, bit_index);
}

P4PhvRecord* P4NameLookup::ParsePhvRecord(const rapidjson::Value &record_json) const {
  RMT_ASSERT(record_json.IsObject());
  std::string field_name = FindMember(record_json, kFieldNameKey).GetString();
  int field_lsb = FindMember(record_json, kFieldLsbKey).GetInt();
  int field_msb = FindMember(record_json, kFieldMsbKey).GetInt();
  int phv_lsb = FindMember(record_json, kPhvLsbKey).GetInt();
  int phv_msb = FindMember(record_json, kPhvMsbKey).GetInt();
  bool pov = FindMember(record_json, kIsPovKey).GetBool();
  P4PhvRecord* record = new P4PhvRecord(field_name, field_lsb, field_msb,
                                       phv_lsb, phv_msb, pov);
  if (pov) {
    auto &pov_headers = FindMemberArray(record_json, kPovHeadersKey);
    for (auto &header_json : pov_headers.GetArray()) {
      record->AddPovHeader(ParsePovHeader(header_json));
    }
  }
  return record;
}

P4PhvContainer *P4NameLookup::ParsePhvContainer(const rapidjson::Value &container_json,
                                            bool egress) const {
  RMT_ASSERT(container_json.IsObject());
  int phv_number = FindMember(container_json, kPhvNumberKey).GetInt();
  P4PhvContainer* container = new P4PhvContainer(phv_number, egress);
  auto& records = FindMemberArray(container_json, kRecordsKey);
  for (auto& record_json : records.GetArray()) {
    container->AddPhvRecord(ParsePhvRecord(record_json));
  }
  return container;
}

P4PhvContainer *P4NameLookup::ParsePhvContainer(const int stage, const int container_index) const {
  auto& phv_allocation = FindMemberArray(*document_, kPhvAllocationKey);
  for (auto& phv_stage_alloc : phv_allocation.GetArray()) {
    auto& stage_number = FindMember(phv_stage_alloc, kStageNumberKey);
    if (stage_number.GetInt() != stage)
      continue;
    for ( bool egress : { false, true } ) {
      auto& phv_stage_alloc_gress = FindMemberArray(
          phv_stage_alloc,  gress_key(egress));
      for (auto& phv_alloc : phv_stage_alloc_gress.GetArray()) {
        int phv_number = FindMember(phv_alloc, kPhvNumberKey).GetInt();
        if (phv_number != container_index)
          continue;
        P4PhvContainer *container = ParsePhvContainer(phv_alloc, egress);
        return container;
      }
    }
  }
  return nullptr;
}

std::vector<P4PhvContainer*> P4NameLookup::ParsePhvContainers(const int stage, bool egress) const {
  std::vector<P4PhvContainer*> containers;
  auto& phv_allocation = FindMemberArray(*document_, kPhvAllocationKey);
  for (auto& phv_stage_alloc : phv_allocation.GetArray()) {
    ExpectTrue(phv_stage_alloc.IsObject(), "Expected phv stage allocation to be an object.");

    auto& stage_number = FindMember(phv_stage_alloc, kStageNumberKey);
    if (stage_number.GetInt() == stage) {
      auto& phv_stage_gress_alloc = FindMemberArray(phv_stage_alloc, gress_key(egress));
      for (auto& phv_alloc : phv_stage_gress_alloc.GetArray()) {
        containers.push_back(ParsePhvContainer(phv_alloc, egress));
      }
    }
  }
  return containers;
}

  void P4NameLookup::LoadPhvContainers(int stage, bool egress) const {
    // iterate over PHV containers for given stage and gress, creating cached
    // P4PhvContainers; for each container also create P4PhvRecords that can be
    // looked up by container number, and if the record is a POV create
    // map entries to look up POV record by bit index.
    P4PhvContainerMap *container_map = &phv_containers_.at(stage);
    P4PhvRecordsMap *records_map = &phv_records_.at(stage).at(egress);
    P4PovHeaderMap *pov_header_map = &phv_pov_headers_.at(stage).at(egress);

    if (!records_map->empty()) return;

    std::vector<P4PhvContainer*> containers = ParsePhvContainers(stage, egress);
    for (P4PhvContainer* container : containers) {
      container_map->emplace(container->GetPhvNumber(), container);
      for (auto *record : container->GetRecords()) {
        // add entry for record to be looked up by field_name

        // in the POV the record's field_name is always POV, so in the records_map
        //  we need to use the pov_headers header_name (there seems to only ever be one)

        // TODO: change this into an assert
        if (record->IsPov() && ! (( record->GetFieldName() == "POV" ) &&
                                  ( record->GetPovHeaders().size() == 1 ))) {
          printf("Warning: unexpected POV field in stage=%d %s container=%d\n",
                 stage,egress?"egress":"ingress",container->GetPhvNumber());
        }

        auto map_name = record->IsPov() ? record->GetPovHeaders()[0]->GetHeaderName() :
                                          record->GetFieldName();

        (*records_map)[map_name].emplace(record->GetFieldLsb(), record);

        if (record->IsPov()) {
          // add entry for record to be looked up by POV header_name's
          for ( auto *pov_header : record->GetPovHeaders() ) {
            (*pov_header_map).emplace(pov_header->GetHeaderName(), pov_header);
          }
        }
      }
    }
  }

  /**
   *  Get the field names for a given container in a given stage.
   *  Examples:
   *  I [field_name[msb:lsb], field_name{msb:lsb], ...]
   *  E [field_name[msb:lsb], ...]
   */
  std::string
  P4NameLookup::GetFieldName(const int &stage, const int &container_index) const {
    if (!loaded_) return "JSON object not found.";
    std::string result;
    try {
      const P4PhvContainer *container = GetPhvContainer(stage, container_index);
      return (nullptr != container) ? container->GetFieldName() : "";
    } catch (std::invalid_argument&) {
      return "PHV container not valid";
    }
  }

  /**
   *  Return a table name given the stage and table index with that stage
   */
  std::string
  P4NameLookup::GetTableName(const int &stage, const int &table_idx) const {
    if (!loaded_) return "JSON object not found.";
    // special table address pointing to the end of the pipeline
    if (stage == -1 && table_idx == -1) {
      return "--END_OF_PIPELINE--";
    }
    try {
      auto& table = FindTable(*document_, tableCache_, stage, table_idx);
      return FindMember(table, "name").GetString();
    } catch (std::invalid_argument& ia) {
      return std::string("P4 table not valid - ") + ia.what();
    }
    return "";
  }

  std::string
  P4NameLookup::GetGatewayConditionName(const int &stage, const int &table_idx) const {
    if (!loaded_) return "JSON object not found.";
    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx, kTableTypeConditionKey);
      return FindMember(table, "condition").GetString();
    } catch (std::invalid_argument& ia) {
      return ia.what();
    }
  }

  bool
  P4NameLookup::GetGatewayHasAttachedTable(const int &stage, const int &table_idx) const {
    if (!loaded_) return false;
    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx, kTableTypeConditionKey);
      return FindMember(table, "attached_to").IsNull() == false;
    } catch (std::invalid_argument&) {
      return false;
    }
  }

  /**
   *  Return the action name for a given table and action instruction address
   */
  std::string
  P4NameLookup::GetActionName(const int &stage, const int &table_idx,
                              const unsigned int &act_instr_addr) const {
    if (!loaded_) return "JSON object not found.";

    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx, kTableTypeMatchKey);
      auto& match_attr = FindMember(table, kMatchAttrKey);
      const char* match_type = FindMember(match_attr, kMatchTypeKey).GetString();
      auto& stage_tables = FindMemberArray(match_attr, kStageTablesKey);
      for (auto& stage_table : stage_tables.GetArray()) {
        int logical_table_id = -1;
        if (stage_table.HasMember(kLogicalTableIdKey)) {
          logical_table_id = stage_table[kLogicalTableIdKey].GetInt();
        }
        auto& stage_number = FindMember(stage_table, kStageNumberKey);
        if (stage_number == stage && logical_table_id == table_idx) {
          // action_format is in a different place for ternary
          if (strcmp(match_type, "ternary") == 0) {
            if (stage_table.HasMember("ternary_indirection_stage_table")) {
              auto& tind_stage_table = FindMember(stage_table, "ternary_indirection_stage_table");
              auto& action_format_arr = FindMemberArray(tind_stage_table, "action_format");
              for (auto& action_format : action_format_arr.GetArray()) {
                auto& vliw_instruction_full = FindMember(action_format, "vliw_instruction_full");
                if (vliw_instruction_full.GetUint() == act_instr_addr) {
                  return FindMember(action_format, "action_name").GetString();
                }
              }
            }
          } else {
            auto& action_format_arr = FindMemberArray(stage_table, "action_format");
            for (auto& action_format : action_format_arr.GetArray()) {
              auto& vliw_instruction_full = FindMember(action_format, "vliw_instruction_full");
              if (vliw_instruction_full.GetUint() == act_instr_addr) {
                return FindMember(action_format, "action_name").GetString();
              }
            }
          }
        }
      }

    } catch (std::invalid_argument& ia) {
      return ia.what();
    }
    if (act_instr_addr & 0x40) { // PFE bit
      return "ERROR:Invalid-Action";
    } else {
      return "No-Action(PFE=0)";
    }
    // return "";
  }

  void P4NameLookup::DumpPhvFieldInfo(int stage,
                                      const Phv &phv,
                                      bool egress,
                                      const std::string& field_name,
                                      std::list<std::string>& results) const {
    LoadPhvContainers(stage, egress);  // load if not already loaded
    auto& stage_record = phv_records_.at(stage).at(egress);

    if (stage_record.find(field_name) == stage_record.end()) {
      std::stringstream ss;
      ss << "\tWarning: '" << field_name << "' not found in context.json for stage "
                           << stage << (egress? " egress" : " ingress") << "\n";
      results.push_back(ss.str());
      return;
    }

    for (auto it : stage_record.at(field_name)) {
      auto record = it.second;
      RMT_ASSERT(nullptr != record);
      RMT_ASSERT(nullptr != record->GetContainer());
      std::stringstream ss;
      ss << std::dec;
      ss << "\t" << field_name << "[" << record->GetFieldMsb() << ":" << record->GetFieldLsb() << "]";
      ss << " = 0x" <<std::uppercase << std::hex;
      ss << phv.get_field_x(record->GetContainer()->GetPhvNumber(), record->GetPhvLsb(), record->GetPhvMsb()) << std::endl;
      ss << std::dec;
      results.push_back(ss.str());
    }
  }

  /**
   *  Log action primitive source
   */
  void P4NameLookup::LogActionPrimitiveSrc(std::list<std::string> &log_str,
                                           const Value &src,
                                           const Phv &iphv,
                                           MauLookupResult &result) const {
    Mau* mau = result.mau();
    const char* src_name = FindMember(src, "name").GetString();
    const char* src_type = FindMember(src, "type").GetString();
    if (strcmp(src_type, "phv") == 0) {
      DumpPhvFieldInfo(mau->mau_index(), iphv, !result.ingress(), src_name, log_str);
    } else if(strcmp(src_type, "immediate") == 0) {
      // for immediate type, name stores the value
      log_str.push_back(std::string("\tVal=0x")+DecStrToHexStr(src_name)+"\n");
    } else {
      const char* src_algorithm = nullptr;
      if (src.HasMember("algorithm")) {
        src_algorithm = FindMember(src, "algorithm").GetString();
        log_str.push_back(std::string("\t")+src_name+"="+src_type+"("+src_algorithm+")\n");
      } else {
        log_str.push_back(std::string("\t")+src_name+"="+src_type+"\n");
      }
    }
  }

  /**
   *  Log action primitive data
   *  ----- PrimitiveType ------
   *    Operation: op
   *    Destination:
   *    blah
   *    Sources:
   *    blah
   */
  void P4NameLookup::LogActionPrimitive(std::list<std::string> &log_str,
                                        const Value &primitive,
                                        const Phv &iphv,
                                        const Phv &ophv,
                                        MauLookupResult &result,
                                        int table_idx) const {
    const char* prim_name = FindMember(primitive, "name").GetString();
    if (strcmp(prim_name, "DropPrimitive") == 0) {
      log_str.push_back("\t----- Drop\n");
      return;
    }
    Mau* mau = result.mau();
    auto tbl_idx = table_idx;
    MauLogicalTable* table = mau->logical_table_lookup(tbl_idx);

    log_str.push_back(std::string("\t")+"----- "+prim_name+" -----\n");
    if (primitive.HasMember("operation")) {
      const char* operation = FindMember(primitive, "operation").GetString();
      log_str.push_back(std::string("\tOperation:\n"));
      log_str.push_back(std::string("\t")+operation+std::string("\n"));
    }
    const char* dst_type = nullptr;
    const char* dst_name = nullptr;
    const char* dst_mask = nullptr;
    if (primitive.HasMember("dst")) {
      auto& dst = FindMember(primitive, "dst");
      dst_type = FindMember(dst, "type").GetString();
      dst_name = FindMember(dst, "name").GetString();
    }
    if (primitive.HasMember("dst_mask")) {
      auto& dst_mask_obj = FindMember(primitive, "dst_mask");
      dst_mask = FindMember(dst_mask_obj, "name").GetString();
    }
    if ((strcmp(prim_name, "ExecuteStatefulAluPrimitive") == 0) ||
        (strcmp(prim_name, "ExecuteStatefulAluFromHashPrimitive") == 0)) {
      // Stateful ALU executed. Fetch details of SALU
      auto& stfl_alu_details = FindMember(primitive, "stateful_alu_details");
      const char* stfl_alu_name = FindMember(stfl_alu_details, "name").GetString();
      log_str.push_back(std::string("\t")+"----- BlackBox: "+stfl_alu_name+" -----\n");
      if (dst_name) {
        // stfl table name is stored in dst_name
        log_str.push_back(std::string("\t")+"----- register: "+dst_name+" -----\n");
      }
      return;
    }
    if (dst_type) {
      auto stats_addr = result.extract_stats_addr(mau, tbl_idx);
      // First check PFE and then see if the address was indeed consumed
      bool addr_consumed = table->mau_addr_dist()->addr_consumed(stats_addr);
      bool counter_ran = (Address::stats_addr_enabled(stats_addr) && addr_consumed);
      const auto& pov_headers = phv_pov_headers_.at(mau->mau_index()).at(!result.ingress());
      if (strcmp(dst_type, "counter") == 0) {
        if (!counter_ran) {
          return;
        }
        auto vpn = Address::stats_addr_get_vpn(stats_addr);
        auto idx = Address::stats_addr_get_index(stats_addr);
        auto subword = Address::stats_addr_get_subword(stats_addr);
        char vpn_str[4];
        snprintf(vpn_str, 4, "%d ", vpn);
        char idx_str[6];
        snprintf(idx_str, 6, "%d ", idx);
        char subword_str[4];
        snprintf(subword_str, 4, "%d ", subword);
        log_str.push_back(std::string("\t----- Update counter: ")+dst_name+"\n");
        log_str.push_back(std::string("\t\t VPN : ")+std::string(vpn_str)+(" Ram Line : ")+std::string(idx_str)+" Subword(Including LSBs. Need to right shift based on stats table format) : "+std::string(subword_str)+"\n");
        return;
      } else if (strcmp(dst_type, "phv") == 0 && pov_headers.count(dst_name) == 0) {
        log_str.push_back("\tDestination:\n");
        int next_stage = mau->mau_index() + 1;  // Destinations become the next stage's source.
        // TODO do we need to check `dst` is not also a `src` in this stage? viz don't `+1`.
        DumpPhvFieldInfo(next_stage, ophv, !result.ingress(), dst_name, log_str);
      } else {
        log_str.push_back("\tDestination:\n");
        log_str.push_back(std::string("\t")+dst_name+"="+dst_type+"\n");
      }
    }
    if (dst_mask) {
      log_str.push_back(std::string("\tmask=0x")+DecStrToHexStr(dst_mask)+"\n");
    }
    // <XXX> "src" is from an older context schema and is deprecated.
    // remove when Brig updates to the latest context schema
    if (primitive.HasMember("src")) {
      auto& src_array = FindMemberArray(primitive, "src");
      log_str.push_back("\tSources:\n");
      for (auto& src : src_array.GetArray()) {
        LogActionPrimitiveSrc(log_str, src, iphv, result);
      }
    }
    // </XXX>
    if (primitive.HasMember("src1")) {
      auto& src1 = FindMember(primitive, "src1");
      log_str.push_back("\tSource 1:\n");
      LogActionPrimitiveSrc(log_str, src1, iphv, result);
      if (primitive.HasMember("src2")) {
        log_str.push_back("\tSource 2:\n");
        auto& src2 = FindMember(primitive, "src2");
        LogActionPrimitiveSrc(log_str, src2, iphv, result);
      }
      if (primitive.HasMember("src3")) {
        log_str.push_back("\tSource 3:\n");
        auto& src3 = FindMember(primitive, "src3");
        LogActionPrimitiveSrc(log_str, src3, iphv, result);
      }
    }
  }

  std::list<std::string>
  P4NameLookup::GetActionInfo(const int& stage, const int& table_idx,
                              const std::string& action_name,
                              const Phv& iphv/* input phv */,
                              const Phv& ophv/* result phv */,
                              MauLookupResult& result) const
  {
    std::list<std::string> action_info;
    if (!loaded_) return action_info;
    try {
      action_info.push_back("Action Results:\n");
      auto& table = FindTable(*document_, tableCache_, stage, table_idx);
      auto& actions = FindMemberArray(table, "actions");
      for (auto& action : actions.GetArray()) {
        const char* action_name_ctx = FindMember(action, "name").GetString();
        if (strcmp(action_name_ctx, action_name.c_str()) == 0) {
          auto& primitives = FindMemberArray(action, "primitives");
          for (auto& primitive : primitives.GetArray()) {
            LogActionPrimitive(action_info, primitive, iphv, ophv, result, table_idx);
          }
        }
      }
    } catch (std::invalid_argument& ia) {
      action_info.push_back(ia.what());
    }
    return action_info;
  }

  // If action has stateful primitive associated with it, the function
  // should return the meter-alu row which is used for stateful operation.
  /**
   *  Use table meter_alu_index to get physical row
   *  0 is on row 1
   *  1 is on row 3
   *  2 is on row 5
   *  3 is on row 7
   */
  int P4NameLookup::GetStflTablePhysicalRow(const int& stage,
                                            const int& table_idx,
                                            const std::string& action_name) const
  {
    if (!loaded_) return -1;
    int row = -1;
    const char* table_name = nullptr;
    const char* stateful_table_name_from_primitive = nullptr;
    try {
      // get stateful table name from action primitive
      auto& match_table = GetTable(*document_, tableCache_, stage, table_idx);
      auto& actions = FindMember(match_table, "actions");
      for (auto& action : actions.GetArray()) {
        const char* action_name_ctx = FindMember(action, "name").GetString();
        if (strcmp(action_name_ctx, action_name.c_str()) == 0) {
          auto& primitives = FindMemberArray(action, "primitives");
          for (auto& primitive : primitives.GetArray()) {
            auto& dst = FindMember(primitive, "dst");
            const char* dst_type = FindMember(dst, "type").GetString();
            if (strcmp(dst_type, "stateful") == 0) {
              stateful_table_name_from_primitive = FindMember(dst, "name").GetString();
            }
          }
        }
      }
      // find the stateful table name and physical row
      auto& tables = FindMemberArray(*document_, kTablesKey);
      for (auto& table : tables.GetArray()) {
        ExpectTrue(table.IsObject(), "Expected table in table array to be an object");
        const char* table_type = FindMember(table, kTableTypeKey).GetString();
        if (strcmp(table_type, "stateful") == 0) {
          auto& stage_tables = FindMemberArray(table, kStageTablesKey);
          for (auto& stage_table : stage_tables.GetArray()) {
            int logical_table_id = FindMember(stage_table, kLogicalTableIdKey).GetInt();
            int stage_number = FindMember(stage_table, kStageNumberKey).GetInt();
            if (stage_number == stage && logical_table_id == table_idx) {
              int meter_alu_index = FindMember(stage_table, "meter_alu_index").GetInt();
              // we can determine physical row from meter_alu_index
              row = 2 * meter_alu_index + 1;
              table_name = FindMember(table, "name").GetString();
              // if table name matches that from primitive
              // return physical row
              if (stateful_table_name_from_primitive && table_name &&
                  strcmp(stateful_table_name_from_primitive, table_name) == 0) {
                return row;
              }
            }
          }
        }
      }
    } catch (std::invalid_argument&) {
    }
    return -1;
  }

  // return conditionLo/Hi operand name (register_lo/hi or p4-field-name)
  std::string P4NameLookup::GetSaluConditionOperand(const int& stage,
                                                    const int& table_idx,
                                                    const std::string& action_name,
                                                    int cond_lo, int operand) const
  {
    if (!loaded_) return "";
    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx);
      auto& actions = FindMemberArray(table, "actions");
      for (auto& action : actions.GetArray()) {
        std::string action_name_ctx = FindMember(action, "name").GetString();
        if (action_name_ctx == action_name) {
          auto& primitives = FindMemberArray(action, "primitives");
          for (auto& primitive : primitives.GetArray()) {
            std::string prim_name = FindMember(primitive, "name").GetString();
            if (prim_name == "ExecuteStatefulAluPrimitive" ||
                prim_name == "ExecuteStatefulAluFromHashPrimitive") {
              auto& stfl_alu_details = FindMember(primitive, "stateful_alu_details");
              Value::ConstMemberIterator itr1, itr2;
              if (cond_lo) {
                itr1 = stfl_alu_details.FindMember("condition_lo");
              } else {
                itr1 = stfl_alu_details.FindMember("condition_hi");
              }
              if (itr1 == stfl_alu_details.MemberEnd()) {
                continue;
              }
              auto& condition_obj = itr1->value;
              if (operand & 0x1) {
                itr1 = condition_obj.FindMember("operand_1_type");
                itr2 = condition_obj.FindMember("operand_1_value");
              } else {
                itr1 = condition_obj.FindMember("operand_2_type");
                itr2 = condition_obj.FindMember("operand_2_value");
              }
              if (itr1 == condition_obj.MemberEnd()) {
                continue;
              }
              if (itr2 == condition_obj.MemberEnd()) {
                continue;
              }
              std::string operand_type = itr1->value.GetString();
              auto& operand_obj = itr2->value;
              if (operand_type == "binary") {
                // return memory or phv operand values
                std::string operand_values = "";
                std::string binary_operand_type = FindMember(operand_obj, "operand_1_type").GetString();
                if (binary_operand_type == "phv" || binary_operand_type == "memory") {
                  operand_values.append(FindMember(operand_obj, "operand_1_value").GetString());
                }
                binary_operand_type = FindMember(operand_obj, "operand_2_type").GetString();
                if (binary_operand_type == "phv" || binary_operand_type == "memory") {
                  if (!operand_values.empty()) {
                    operand_values.append(",");
                  }
                  operand_values.append(FindMember(operand_obj, "operand_2_value").GetString());
                }
                return operand_values;
              } else if (operand_type == "unary") {
                return FindMember(operand_obj, "operand_1_value").GetString();
              } else {
                // else we can assume we have a string to return
                // i.e. phv field name
                return operand_obj.GetString();
              }
            }
          }
        }
      }
    } catch (std::invalid_argument& ia) {
      return ia.what();
    }
    return "";
  }

  // return Update_[Lo/Hi][1/2]_value operand name (register_lo/hi or p4-field-name)
  std::string P4NameLookup::GetSaluUpdateValueOperand(const int& stage,
                                                      const int& table_idx,
                                                      const std::string& action_name,
                                                      int update_lo, int alu, int operand) const
  {
    if (!loaded_) return "";
    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx);
      auto& actions = FindMemberArray(table, "actions");
      for (auto& action : actions.GetArray()) {
        std::string action_name_ctx = FindMember(action, "name").GetString();
        if (action_name_ctx != action_name) continue;
        auto& primitives = FindMemberArray(action, "primitives");
        for (auto& primitive : primitives.GetArray()) {
          std::string prim_name = FindMember(primitive, "name").GetString();
          if (prim_name != "ExecuteStatefulAluPrimitive" &&
              prim_name != "ExecuteStatefulAluFromHashPrimitive")
            continue;

          auto& stfl_alu_details = FindMember(primitive, "stateful_alu_details");
          if (FindMember(stfl_alu_details, "single_bit_mode").GetBool()) {
            return "singlebitmode";
          }
          Value::ConstMemberIterator itr1, itr2;
          if (update_lo) {
            if (alu & 0x1) {
              itr1 = stfl_alu_details.FindMember("update_lo_1_value");
            } else {
              itr1 = stfl_alu_details.FindMember("update_lo_2_value");
            }
          } else {
            if (alu & 0x1) {
              itr1 = stfl_alu_details.FindMember("update_hi_1_value");
            } else {
              itr1 = stfl_alu_details.FindMember("update_hi_2_value");
            }
          }
          if (itr1 == stfl_alu_details.MemberEnd())
            continue;
          auto& update_obj = itr1->value;
          if (operand & 0x1) {
            itr1 = update_obj.FindMember("operand_1_type");
            itr2 = update_obj.FindMember("operand_1_value");
          } else {
            itr1 = update_obj.FindMember("operand_2_type");
            itr2 = update_obj.FindMember("operand_2_value");
          }
          if (itr1 == update_obj.MemberEnd())
            continue;
          if (itr2 == update_obj.MemberEnd())
            continue;
          std::string operand_type = itr1->value.GetString();
          auto& operand_obj = itr2->value;
          if (operand_type == "phv" || operand_type == "immediate") {
            return operand_obj.GetString();
          } else if (operand_type == "memory") {
            return "constant";
          }
        }
      }
    } catch (std::invalid_argument& ia) {
      return ia.what();
    }
    return "";
  }

  // return Update_[Lo/Hi][1/2]_predicate operand name (condition_lo / condition_hi)
  std::string P4NameLookup::GetSaluUpdatePredicateOperand(const int& stage,
                                                      const int& table_idx,
                                                      const std::string& action_name,
                                                      int update_lo, int alu, int operand) const
  {
    if (!loaded_) return "";
    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx);
      auto& actions = FindMemberArray(table, "actions");
      for (auto& action : actions.GetArray()) {
        std::string action_name_ctx = FindMember(action, "name").GetString();
        if (action_name_ctx != action_name)
          continue;

        auto& primitives = FindMemberArray(action, "primitives");
        for (auto& primitive : primitives.GetArray()) {
          std::string prim_name = FindMember(primitive, "name").GetString();
          if (prim_name != "ExecuteStatefulAluPrimitive" &&
              prim_name != "ExecuteStatefulAluFromHashPrimitive")
            continue;

          auto& stfl_alu_details = FindMember(primitive, "stateful_alu_details");
          Value::ConstMemberIterator itr1, itr2;
          if (update_lo) {
            if (alu & 0x1) {
              itr1 = stfl_alu_details.FindMember("update_lo_1_predicate");
            } else {
              itr1 = stfl_alu_details.FindMember("update_lo_2_predicate");
            }
          } else {
            if (alu & 0x1) {
              itr1 = stfl_alu_details.FindMember("update_hi_1_predicate");
            } else {
              itr1 = stfl_alu_details.FindMember("update_hi_2_predicate");
            }
          }
          if (itr1 == stfl_alu_details.MemberEnd())
            continue;
          auto& update_obj = itr1->value;
          if (operand & 0x1) {
            itr1 = update_obj.FindMember("operand_1_type");
            itr2 = update_obj.FindMember("operand_1_value");
          } else {
            itr1 = update_obj.FindMember("operand_2_type");
            itr2 = update_obj.FindMember("operand_2_value");
          }
          if (itr1 == update_obj.MemberEnd())
            continue;
          if (itr2 == update_obj.MemberEnd())
            continue;
          std::string operand_type = itr1->value.GetString();
          auto& operand_obj = itr2->value;
          if (operand_type == "binary") {
            // not implemented
          } else if (operand_type == "unary") {
            std::string operation = FindMember(operand_obj, "operation").GetString();
            std::string operand_1_value = FindMember(operand_obj, "operand_1_value").GetString();
            return operation + " " + operand_1_value;
          } else {
            return operand_obj.GetString();
          }
        }
      }

    } catch (std::invalid_argument& ia) {
      return ia.what();
    }
    return "";
  }

  // return output_predicate operand names (condition_lo / condition_hi)
  std::string P4NameLookup::GetSaluOutputPredicateOperand(const int& stage,
                                                          const int& table_idx,
                                                          const std::string& action_name,
                                                          int operand) const
  {
    if (!loaded_) return "";
    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx);
      auto& actions = FindMemberArray(table, "actions");
      for (auto& action : actions.GetArray()) {
        std::string action_name_ctx = FindMember(action, "name").GetString();
        if (action_name_ctx != action_name)
          continue;

        auto& primitives = FindMemberArray(action, "primitives");
        for (auto& primitive : primitives.GetArray()) {
          std::string prim_name = FindMember(primitive, "name").GetString();
          if (prim_name != "ExecuteStatefulAluPrimitive" &&
              prim_name != "ExecuteStatefulAluFromHashPrimitive")
            continue;

          auto& stfl_alu_details = FindMember(primitive, "stateful_alu_details");
          Value::ConstMemberIterator itr1, itr2;
          itr1 = stfl_alu_details.FindMember("output_predicate");
          if (itr1 == stfl_alu_details.MemberEnd())
            continue;
          auto& output_predicate = itr1->value;
          if (operand & 0x1) {
            itr1 = output_predicate.FindMember("operand_1_type");
            itr2 = output_predicate.FindMember("operand_1_value");
          } else {
            itr1 = output_predicate.FindMember("operand_2_type");
            itr2 = output_predicate.FindMember("operand_2_value");
          }
          if (itr1 == output_predicate.MemberEnd())
            continue;
          if (itr2 == output_predicate.MemberEnd())
            continue;
          auto& operand_value = itr2->value;
          // TODO do we care about unary or binary operands here?
          if (operand_value.IsString()) {
            return operand_value.GetString();
          }
        }
      }
    } catch  (std::invalid_argument& ia) {
      return ia.what();
    }
    return "";
  }

  // return output_destination p4 field name
  std::string P4NameLookup::GetSaluOutputDestinationP4Name(const int& stage,
                                                           const int& table_idx,
                                                           const std::string& action_name) const
  {
    if (!loaded_) return "";
    try {
      auto& table = GetTable(*document_, tableCache_, stage, table_idx);
      auto& actions = FindMemberArray(table, "actions");
      for (auto& action : actions.GetArray()) {
        std::string action_name_ctx = FindMember(action, "name").GetString();
        if (action_name_ctx != action_name)
          continue;

        auto& primitives = FindMemberArray(action, "primitives");
        for (auto& primitive : primitives.GetArray()) {
          std::string prim_name = FindMember(primitive, "name").GetString();
          if (prim_name != "ExecuteStatefulAluPrimitive" &&
              prim_name != "ExecuteStatefulAluFromHashPrimitive")
            continue;

          auto& stfl_alu_details = FindMember(primitive, "stateful_alu_details");
          Value::ConstMemberIterator itr1 = stfl_alu_details.FindMember("output_dst");
          if (itr1 == stfl_alu_details.MemberEnd())
            continue;
          return itr1->value.GetString();
        }
      }
    } catch (std::invalid_argument& ia) {
      return ia.what();
    }
    return "";
  }


  /**
   *  Return list of strings of form "Header <name> is valid" for those headers
   *  whose POV bit is set.
   */
  std::list<std::string> P4NameLookup::GetValidHeaderNames(const bool egress,
                                                           const Phv &phv) const {
    std::list<std::string> valid_header_names;
    if (!loaded_) return valid_header_names;

    // Note: this function looks up in stage 0 only regardless of calling
    // context; this has historically been the case and perhaps makes sense if
    // the POV bits are immutably set in stage 0
    int stage = 0;
    LoadPhvContainers(stage, egress);  // load if not already loaded
    try {
      const P4PovHeaderMap *povHeaderMap = GetPovHeaders(stage, egress);
      for (const auto& it : *povHeaderMap) {
        auto pov_header = it.second;
        int pov_bit = phv.get_field_x(pov_header->GetRecord()->GetContainer()->GetPhvNumber(),
                                      pov_header->GetBitIndex(),
                                      pov_header->GetBitIndex());
        if (pov_bit) {
          std::stringstream ss;
          ss << "Header " << pov_header->GetHeaderName() << " is valid\n";
          valid_header_names.push_back(ss.str());
        }
      }
    } catch (std::invalid_argument &ia) {
      valid_header_names.emplace_back(ia.what());
    }
    return valid_header_names;
  }

  const Value& P4NameLookup::GetPvsArray(const int &parser_id, const bool egress) const {
    std::string err;
    try {
      // first try the "parsers" node if present
      const Value& parsers = FindMember(*document_, kParsersKey);
      err = "Error: Could not find states for parser id " + std::to_string(parser_id);
      auto& parsers_array = FindMemberArray(parsers, gress_key(egress));
      for (auto& parser : parsers_array.GetArray()) {
        auto& default_parser_ids = FindMember(parser, "default_parser_id");
        for (auto& default_parser_id : default_parser_ids.GetArray()) {
          if (parser_id == default_parser_id) {
            return FindMember(parser, "states");
          }
        }
      }
      throw std::invalid_argument(err);
    } catch (std::invalid_argument &ia) {
      // if "parsers" node was found but not PVS then throw the exception
      if (!err.empty()) throw ia;
      // else fallback to try "parser" node
      auto& parser = FindMember(*document_, kParserKey);
      return FindMemberArray(parser, gress_key(egress));
    }
  }

  std::string
  P4NameLookup::GetParserStateName(const int &parser_id,
                                   const bool egress,
                                   const int &state) const {
    if (!loaded_) return "JSON object not found.";
    try {
      auto& pvs_array = GetPvsArray(parser_id, egress);
      for (auto& pvs : pvs_array.GetArray()) {
        int parser_state_id = FindMember(pvs, "parser_state_id").GetUint();
        if (parser_state_id == state) {
          return FindMember(pvs, "parser_name").GetString();
        }
      }
    } catch (std::invalid_argument &ia) {
      return ia.what();
    }
    return "Error : Unknown-State";
  }

  std::string P4NameLookup::GetSchemaVersion() const {
    return FindMember(*document_, "schema_version").GetString();
  }

}
