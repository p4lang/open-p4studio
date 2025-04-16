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

#ifndef _RMT_P4_NAME_LOOKUP_H
#define _RMT_P4_NAME_LOOKUP_H

#include <list>
#include <string>
#include <tuple>
#include <map>
#include <model_core/json-loader.h>
#include <model_core/p4-objects.h>

namespace MODEL_CHIP_NAMESPACE {

class Phv;
class MauLookupResult;

using model_core::P4PovHeader;
using model_core::P4PhvRecord;
using model_core::P4PhvContainer;

/**
 *  Lookup with <stage, table_idx, table_type>
 */
using P4TableLookup = std::tuple<const int, const int, const std::string>;
using P4TableCache = std::map<P4TableLookup, const rapidjson::Value &>;

// cache for container lookup by (stage, index)
using P4PhvContainerMap = std::map<int, P4PhvContainer*>;
using P4PhvContainerCache = std::array<P4PhvContainerMap, RmtDefs::kStagesMax+1>;

// cache for record lookup by (gress, stage, field_name, field_lsb)
using P4PhvRecordMap = std::map<int, P4PhvRecord*>;
using P4PhvRecordsMap = std::map<std::string, P4PhvRecordMap>;
using P4PhvRecordsCache = std::array<std::array<P4PhvRecordsMap, 2>, RmtDefs::kStagesMax+1>;

// For P4PovHeader lookup by (stage, egress, header_name)
using P4PovHeaderMap = std::map<std::string, P4PovHeader*>;
using P4PovHeaderCache = std::array<std::array<P4PovHeaderMap, 2>, RmtDefs::kStagesMax+1>;

class P4NameLookup : public model_core::JSONLoader {
 public:
    static constexpr int kMaxContextFileSize = 200000000;  // max chars to read from context.json
    P4NameLookup(const std::string &file_name);
    ~P4NameLookup();

    // passed to base constructor to perform check on document during construction
    static const bool check_loaded_doc(std::unique_ptr<rapidjson::Document>& document);

    /**
     * Get the schema version
     * @throws invalid_argument if the schema version is not found.
     * @return a string containing the schema version
     */
    std::string GetSchemaVersion() const;

    /**
     *  Get the match fields for a given table and their values in a given PHV
     *  E.g.
     *    field_name1[msb:lsb] = 0x0000
     *    field_name2[msb:lsb] = 0x0000
     */
    std::list<std::string>
    GetMatchFields(const int &stage, const int &table_idx,
                   const Phv &phv, const bool egress) const;


    P4PovHeader *ParsePovHeader(const rapidjson::Value &header_json) const;
    P4PhvRecord *ParsePhvRecord(const rapidjson::Value &record_json) const;
    P4PhvContainer *ParsePhvContainer(const rapidjson::Value &container_json, bool egress) const;
    P4PhvContainer *ParsePhvContainer(const int stage, const int container_index) const;
    std::vector<P4PhvContainer*> ParsePhvContainers(const int stage, bool egress) const;
    void LoadPhvContainers(int stage, bool egress) const;

    const P4PovHeaderMap* GetPovHeaders(const int &stage, bool egress) const {
      return &phv_pov_headers_.at(stage).at(egress);
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
    const P4PhvContainer* GetPhvContainer(const int &stage, const int &container_index) const;

    /**
     * Same as GetPhvContainer but will return a nullptr instead of throwing
     * std::invalid_argument if invalid context is encountered.
     */
    const P4PhvContainer* GetPhvContainerSafe(const int &stage, const int &container_index) const;

    /**
     *  Get the field names for a given container in a given stage
     *  E.g.
     *    I [field_name[msb:lsb], field_name[msb:lsb], ...]
     *    E [field_name[msb:lsb], ...]
     */
    std::string
    GetFieldName(const int &stage, const int &container) const;

    /**
     *  Get a table P4 name
     */
    std::string
    GetTableName(const int &stage, const int &table_idx) const;

    /**
     *  Get gateway condition name
     */
    std::string
    GetGatewayConditionName(const int &stage, const int &table_idx) const;

    /**
     *  Get whether gateway has attached table
     */
    bool
    GetGatewayHasAttachedTable(const int &stage, const int &table_idx) const;

    /**
     *  Get an action p4 name
     */
    std::string
    GetActionName(const int &stage, const int &table_idx,
                  const unsigned int &act_instr_addr) const;

    /**
     *  Get valid header names for a PHV
     *  E.g.
     *   "Header <name1> is valid"
     *   "Header <name2> is valid"
     */
    std::list<std::string>
    GetValidHeaderNames(const bool egress, const Phv &phv) const;

    /**
     * Get array of parser PVS
     * @param parser_id index of parser
     * @param egress true if egress
     * @return an array
     */
    const rapidjson::Value& GetPvsArray(const int &parser_id, const bool egress) const;

    /**
     *  Get a parser state p4 name
     */
    std::string
    GetParserStateName(const int &parser_id, const bool egress, const int &state) const;

    std::list<std::string> GetActionInfo(const int &stage,
                                         const int &table_index,
                                         const std::string &action_name,
                                         const Phv &iphv, const Phv &ophv,
                                         MauLookupResult &result) const;

    int GetStflTablePhysicalRow(const int& stage, const int& table_index,
                                const std::string& action_name) const;
    std::string GetSaluConditionOperand(const int& stage,
                                        const int& table_idx,
                                        const std::string& action_name,
                                        int cond_lo, int operand) const;
    std::string GetSaluUpdateValueOperand(const int& stage,
                                          const int& table_idx,
                                          const std::string& action_name,
                                          int update_lo, int alu, int operand) const;
    std::string GetSaluUpdatePredicateOperand(const int& stage,
                                              const int& table_idx,
                                              const std::string& action_name,
                                              int update_lo, int alu, int operand) const;
    std::string GetSaluOutputPredicateOperand(const int& stage,
                                              const int& table_idx,
                                              const std::string& action_name,
                                              int operand) const;
    std::string GetSaluOutputDestinationP4Name(const int& stage,
                                               const int& table_idx,
                                               const std::string& action_name) const;

 private:
   void DumpPhvFieldInfo(int stage,
                        const Phv &phv,
                        bool egress,
                        const std::string& field_name,
                        std::list<std::string> &results) const;

  void LogActionPrimitiveSrc(std::list<std::string> &log_str,
                             const rapidjson::Value &src,
                             const Phv &iphv,
                             MauLookupResult &result) const;

  void LogActionPrimitive(std::list<std::string> &log_str,
                          const rapidjson::Value &primitive,
                          const Phv &iphv,
                          const Phv &ophv,
                          MauLookupResult &result,
                          int table_idx) const;

    mutable P4TableCache tableCache_;
    mutable P4PhvContainerCache phv_containers_;
    mutable P4PhvRecordsCache phv_records_;
    mutable P4PovHeaderCache phv_pov_headers_;
};

}

#endif /* !_RMT_P4_NAME_LOOKUP_H */
