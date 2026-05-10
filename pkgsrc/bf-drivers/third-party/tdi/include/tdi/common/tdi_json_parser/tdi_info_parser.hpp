// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifndef _TDI_INFO_PARSER_HPP
#define _TDI_INFO_PARSER_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

/* tdi_includes */
#include <tdi/common/tdi_json_parser/tdi_learn_info.hpp>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>

namespace tdi {
namespace tdi_json {

const std::string SCHEMA_VERSION = "schema_version";
const std::string TABLES = "tables";
const std::string TABLE_NAME = "name";
const std::string TABLE_ID = "id";
const std::string TABLE_TYPE = "table_type";
const std::string TABLE_SIZE = "size";
const std::string TABLE_ANNOTATIONS = "annotations";
const std::string TABLE_DEPENDS_ON = "depends_on";
const std::string TABLE_HAS_CONST_DEFAULT_ACTION = "has_const_default_action";
const std::string TABLE_IS_CONST = "is_const";

const std::string TABLE_KEY = "key";
const std::string TABLE_KEY_ID = "id";
const std::string TABLE_KEY_NAME = "name";
const std::string TABLE_KEY_REPEATED = "repeated";
const std::string TABLE_KEY_MANDATORY = "mandatory";
const std::string TABLE_KEY_ANNOTATIONS = "annotations";
const std::string TABLE_KEY_MATCH_TYPE = "match_type";
const std::string TABLE_KEY_TYPE = "type";
const std::string TABLE_KEY_TYPE_TYPE = "type";
const std::string TABLE_KEY_TYPE_WIDTH = "width";

const std::string TABLE_ACTION_SPECS = "action_specs";
const std::string TABLE_ACTION_ID = "id";
const std::string TABLE_ACTION_NAME = "name";
const std::string TABLE_ACTION_ACTION_SCOPE = "action_scope";
const std::string TABLE_ACTION_ANNOTATIONS = "annotations";
const std::string TABLE_ACTION_DATA = "data";

const std::string TABLE_DATA = "data";
const std::string TABLE_DATA_ID = "id";
const std::string TABLE_DATA_NAME = "name";
const std::string TABLE_DATA_REPEATED = "repeated";
const std::string TABLE_DATA_MANDATORY = "mandatory";
const std::string TABLE_DATA_ANNOTATIONS = "annotations";
const std::string TABLE_DATA_TYPE = "type";
const std::string TABLE_DATA_TYPE_TYPE = "type";
const std::string TABLE_DATA_TYPE_WIDTH = "width";
const std::string TABLE_DATA_TYPE_DEFAULT_VALUE = "default_value";

const std::string LEARN_FILTERS = "learn_filters";
const std::string LEARN_NAME = "name";
const std::string LEARN_ID = "id";
const std::string LEARN_ANNOTATIONS = "annotations";
const std::string LEARN_FIELDS = "fields";

}  // namespace tdi_json

// Forward declarations
class TdiInfoMapper;

class TdiInfoParser {
 public:
  TdiInfoParser(std::unique_ptr<TdiInfoMapper> tdi_info_mapper);

  tdi_status_t parseTdiInfo(
      const std::vector<std::string> &tdi_info_file_paths);

  const std::map<std::string, std::unique_ptr<TableInfo>> &tableInfoMapGet()
      const {
    return table_info_map_;
  };
  const std::map<std::string, std::unique_ptr<LearnInfo>> &learnInfoMapGet()
      const {
    return learn_info_map_;
  };

 private:
  std::unique_ptr<tdi::TableInfo> parseTable(const tdi::Cjson &table_tdi);
  std::unique_ptr<tdi::LearnInfo> parseLearn(const tdi::Cjson &learn_tdi);
  std::unique_ptr<KeyFieldInfo> parseKeyField(const tdi::Cjson &key_json);
  std::unique_ptr<DataFieldInfo> parseDataField(const tdi::Cjson &data_json,
                                                const uint64_t &oneof_index);
  std::unique_ptr<ActionInfo> parseAction(const tdi::Cjson &action_json);
  std::set<tdi::Annotation> parseAnnotations(
      const tdi::Cjson &annotation_cjson);
  void parseFieldWidth(const tdi::Cjson &node,
                       tdi_field_data_type_e &type,
                       size_t &width,
                       uint64_t &default_value,
                       float &default_fl_value,
                       std::string &default_str_value,
                       std::vector<std::string> &choices);
  tdi_table_type_e tableTypeStrToEnum(const std::string &type);
  tdi_match_type_e matchTypeStrToEnum(const std::string &type);
  tdi_operations_type_e operationsTypeStrToEnum(const std::string &type);
  tdi_attributes_type_e attributesTypeStrToEnum(const std::string &type);

  const std::unique_ptr<TdiInfoMapper> tdi_info_mapper_;
  std::map<std::string, std::unique_ptr<TableInfo>> table_info_map_;
  std::map<std::string, std::unique_ptr<LearnInfo>> learn_info_map_;
};

}  // namespace tdi

#endif
