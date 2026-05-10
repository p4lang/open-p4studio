// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_json_parser/tdi_info_parser.hpp>

#include <tdi/common/tdi_utils.hpp>
#include <tdi/common/tdi_json_parser/tdi_cjson.hpp>

namespace tdi {

namespace {

tdi_field_data_type_e dataTypeStrToEnum(const std::string &type,
                                        const bool &repeated) {
  if (type == "bytes") {
    return TDI_FIELD_DATA_TYPE_BYTE_STREAM;
  } else if (type == "uint64" || type == "uint32" || type == "uint16" ||
             type == "uint8") {
    if (!repeated)
      return TDI_FIELD_DATA_TYPE_UINT64;
    else
      return TDI_FIELD_DATA_TYPE_INT_ARR;
  } else if (type == "bool") {
    if (!repeated)
      return TDI_FIELD_DATA_TYPE_BOOL;
    else
      return TDI_FIELD_DATA_TYPE_BOOL_ARR;
  } else if (type == "float") {
    return TDI_FIELD_DATA_TYPE_FLOAT;
  } else if (type == "string") {
    if (!repeated)
      return TDI_FIELD_DATA_TYPE_STRING;
    else
      return TDI_FIELD_DATA_TYPE_STRING_ARR;
  } else if (type == "int64" || type == "int32" || type == "int16" ||
             type == "int8") {
    if (!repeated)
      return TDI_FIELD_DATA_TYPE_INT64;
    else
      return TDI_FIELD_DATA_TYPE_INT_ARR;
  }
  return TDI_FIELD_DATA_TYPE_UNKNOWN;
}

// This function returns if a key field is a field slice or not
bool checkIsFieldSlice(const tdi::Cjson &key_field) {
  tdi::Cjson key_annotations = key_field["annotations"];
  for (const auto &annotation : key_annotations.getCjsonChildVec()) {
    std::string annotation_name = (*annotation)["name"];
    std::string annotation_value = (*annotation)["value"];
    if ((annotation_name == "isFieldSlice") && (annotation_value == "true")) {
      return true;
    }
  }
  return false;
}

}  // anonymous namespace

TdiInfoParser::TdiInfoParser(std::unique_ptr<TdiInfoMapper> tdi_info_mapper)
    : tdi_info_mapper_(std::move(tdi_info_mapper)) {}

tdi_table_type_e TdiInfoParser::tableTypeStrToEnum(const std::string &type) {
  if (tdi_info_mapper_->tableEnumMapGet().find(type) !=
      tdi_info_mapper_->tableEnumMapGet().end()) {
    return tdi_info_mapper_->tableEnumMapGet().at(type);
  }
  return TDI_TABLE_TYPE_CORE;
}
tdi_match_type_e TdiInfoParser::matchTypeStrToEnum(const std::string &type) {
  if (tdi_info_mapper_->matchEnumMapGet().find(type) !=
      tdi_info_mapper_->matchEnumMapGet().end()) {
    return tdi_info_mapper_->matchEnumMapGet().at(type);
  }
  return TDI_MATCH_TYPE_CORE;
}
tdi_operations_type_e TdiInfoParser::operationsTypeStrToEnum(
    const std::string &type) {
  if (tdi_info_mapper_->operationsEnumMapGet().find(type) !=
      tdi_info_mapper_->operationsEnumMapGet().end()) {
    return tdi_info_mapper_->operationsEnumMapGet().at(type);
  }
  return TDI_OPERATIONS_TYPE_CORE;
}
tdi_attributes_type_e TdiInfoParser::attributesTypeStrToEnum(
    const std::string &type) {
  if (tdi_info_mapper_->attributesEnumMapGet().find(type) !=
      tdi_info_mapper_->attributesEnumMapGet().end()) {
    return tdi_info_mapper_->attributesEnumMapGet().at(type);
  }
  return TDI_ATTRIBUTES_TYPE_CORE;
}

void TdiInfoParser::parseFieldWidth(const tdi::Cjson &node,
                                    tdi_field_data_type_e &type,
                                    size_t &width,
                                    uint64_t &default_value,
                                    float &default_fl_value,
                                    std::string &default_str_value,
                                    std::vector<std::string> &choices) {
  auto type_str =
      std::string(node[tdi_json::TABLE_DATA_TYPE][tdi_json::TABLE_DATA_TYPE]);
  bool repeated = false;
  if (node[tdi_json::TABLE_DATA_REPEATED].exists()) {
    repeated = node[tdi_json::TABLE_DATA_REPEATED];
  }
  type = dataTypeStrToEnum(type_str, repeated);
  if (type_str == "bytes") {
    width = static_cast<unsigned int>(node["type"]["width"]);
  } else if (type_str == "uint64") {
    width = 64;
  } else if (type_str == "uint32") {
    width = 32;
  } else if (type_str == "uint16") {
    width = 16;
  } else if (type_str == "uint8") {
    width = 8;
  } else if (type_str == "int64") {
    width = 64;
  } else if (type_str == "int32") {
    width = 32;
  } else if (type_str == "int16") {
    width = 16;
  } else if (type_str == "int8") {
    width = 8;
  } else if (type_str == "bool") {
    width = 1;
  } else if (type_str == "float") {
    width = 0;
    // If default value is listed, populate it, else its zero
    if (node["type"]["default_value"].exists()) {
      default_fl_value = static_cast<float>(node["type"]["default_value"]);
    } else {
      default_fl_value = 0.0;
    }
  } else if (type_str == "string") {
    width = 0;
    if (node["type"]["width"].exists()) {
        width = static_cast<unsigned int>(node["type"]["width"]);
    }
    for (const auto &choice : node["type"]["choices"].getCjsonChildVec()) {
      choices.push_back(static_cast<std::string>(*choice));
    }
    // If string default value is listed populate it, else its empty
    if (node["type"]["default_value"].exists()) {
      default_str_value =
          static_cast<std::string>(node["type"]["default_value"]);
    } else {
      default_str_value = std::string("INVALID");
    }
  } else if (node["container"].exists()) {
    width = 0;
    type = TDI_FIELD_DATA_TYPE_CONTAINER;
  } else {
    width = 0;
  }

  // If default value is listed, populate it, else its zero
  if (node["type"]["default_value"].exists()) {
    default_value = static_cast<uint64_t>(node["type"]["default_value"]);
  } else {
    default_value = 0;
  }
}

std::unique_ptr<KeyFieldInfo> TdiInfoParser::parseKeyField(
    const tdi::Cjson &table_key_cjson) {
  // parses the table_key json node and extracts all the relevant
  tdi_id_t id = static_cast<tdi_id_t>(table_key_cjson["id"]);
  std::string name = table_key_cjson[tdi_json::TABLE_KEY_NAME];
  bool mandatory = table_key_cjson[tdi_json::TABLE_KEY_MANDATORY];
  tdi_match_type_e match_type =
      matchTypeStrToEnum(table_key_cjson[tdi_json::TABLE_KEY_MATCH_TYPE]);
  tdi_field_data_type_e field_data_type;
  size_t width;
  uint64_t default_value;
  float default_fl_value;
  std::string default_str_value;
  std::vector<std::string> choices;
  std::string data_type;
  parseFieldWidth(table_key_cjson,
                  field_data_type,
                  width,
                  default_value,
                  default_fl_value,
                  default_str_value,
                  choices);

  // create key_field structure and fill it
  auto tmp = new KeyFieldInfo(id,
                              name,
                              width,
                              match_type,
                              field_data_type,
                              mandatory,
                              choices,
                              parseAnnotations(table_key_cjson["annotations"]),
                              default_value,
                              default_fl_value,
                              default_str_value,
                              checkIsFieldSlice(table_key_cjson),
                              false,
                              false);
  if (tmp == nullptr) {
    LOG_ERROR("%s:%d Error forming key_field %d",
              __func__,
              __LINE__,
              static_cast<uint32_t>(table_key_cjson["id"]));
  }

  std::unique_ptr<KeyFieldInfo> key_field(tmp);
  if (key_field == nullptr) {
    LOG_ERROR("%s:%d Error forming key_field %d",
              __func__,
              __LINE__,
              static_cast<uint32_t>(table_key_cjson["id"]));
  }
  return key_field;
}

std::set<tdi::Annotation> TdiInfoParser::parseAnnotations(
    const tdi::Cjson &annotation_cjson) {
  std::set<tdi::Annotation> annotations;
  for (const auto &annotation : annotation_cjson.getCjsonChildVec()) {
    std::string annotation_name = (*annotation)["name"];
    std::string annotation_value = (*annotation)["value"];
    annotations.emplace(annotation_name, annotation_value);
  }
  return annotations;
}

std::unique_ptr<struct DataFieldInfo> TdiInfoParser::parseDataField(
    const tdi::Cjson &data_json_in, const uint64_t &oneof_index) {
  tdi::Cjson data_json = data_json_in;

  // first get mandatory and read_only
  // because it exists outside oneof and singleton
  auto mandatory = bool(data_json["mandatory"]);
  auto read_only = bool(data_json["read_only"]);

  // if "singleton" field exists, then
  // lets point to it for further parsing
  if (data_json["singleton"].exists()) {
    data_json = data_json["singleton"];
  }

  std::set<tdi_id_t> oneof_siblings;
  if (data_json["oneof"].exists()) {
    // Create a set of all the oneof members IDs
    for (const auto &oneof_data : data_json["oneof"].getCjsonChildVec()) {
      oneof_siblings.insert(static_cast<tdi_id_t>((*oneof_data)["id"]));
    }
    data_json = data_json["oneof"][oneof_index];
    // remove this field's ID from the siblings. One
    // cannot be their own sibling now, can they?
    oneof_siblings.erase(static_cast<tdi_id_t>(data_json["id"]));
  }
  std::string data_name = static_cast<std::string>(data_json["name"]);
  // get "type"
  tdi_field_data_type_e field_data_type;
  size_t width;
  std::vector<std::string> choices;
  // Default value of the field. We currently only support listing upto 64 bits
  // of default value
  uint64_t default_value;
  float default_fl_value;
  std::string default_str_value;
  parseFieldWidth(data_json,
                  field_data_type,
                  width,
                  default_value,
                  default_fl_value,
                  default_str_value,
                  choices);

  bool repeated = data_json["repeated"];
  // Get Annotations if any
  std::set<Annotation> annotations = parseAnnotations(data_json["annotations"]);

  tdi_id_t data_id = static_cast<tdi_id_t>(data_json["id"]);
  // Parse the container if it exists
  bool container_valid = false;
  if (data_json["container"].exists()) {
    container_valid = true;
  }
  std::unique_ptr<struct DataFieldInfo> data_field(
      new struct DataFieldInfo(data_id,
                               data_name,
                               width,
                               field_data_type,
                               mandatory,
                               read_only,
                               std::move(choices),
                               annotations,
                               default_value,
                               default_fl_value,
                               default_str_value,
                               repeated,
                               container_valid,
                               oneof_siblings));

  // Parse the container if it exists

#if 0
  if (container_valid) {
    tdi::Cjson c_table_data = data_json["container"];
    parseContainer(c_table_data, tdiTable, data_field.get());
  }
#endif
  return data_field;
}

// This variant of the function is to parse the action specs for all the tables
// except phase0 table
std::unique_ptr<ActionInfo> TdiInfoParser::parseAction(
    const tdi::Cjson &action_json) {
  std::string name = action_json["name"];
  tdi_id_t id = action_json["id"];
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> data_fields;

  // get action profile data_json
  tdi::Cjson action_data_cjson = action_json["data"];
  for (const auto &action_data : action_data_cjson.getCjsonChildVec()) {
    uint64_t oneof_index = 0;
    auto data_field = parseDataField(*action_data, oneof_index);
    if (data_fields.find(data_field->idGet()) != data_fields.end()) {
      LOG_ERROR("%s:%d ID \"%u\" Exists for data ",
                __func__,
                __LINE__,
                data_field->idGet());
      continue;
    }
    data_fields[data_field->idGet()] = std::move(data_field);
  }
  std::unique_ptr<ActionInfo> action_info(
      new ActionInfo(id,
                     name,
                     std::move(data_fields),
                     parseAnnotations(action_json["annotations"])));
  return action_info;
}

std::unique_ptr<tdi::LearnInfo> TdiInfoParser::parseLearn(
    const tdi::Cjson &learn_tdi) {
  tdi_id_t learn_id = learn_tdi[tdi_json::LEARN_ID];
  std::string learn_name = learn_tdi[tdi_json::LEARN_NAME];
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> learn_field_map;

  LOG_DBG("Learn : %s :: ID :: %d ", learn_name.c_str(), learn_id);

  // parse each field
  int oneof_size = 1;
  for (const auto &field :
       learn_tdi[tdi_json::LEARN_FIELDS].getCjsonChildVec()) {
    auto learn_field = parseDataField(*field, oneof_size);
    if (learn_field == nullptr) {
      continue;
    }
    tdi_id_t learn_field_id = learn_field->idGet();
    if (learn_field_map.find(learn_field_id) != learn_field_map.end()) {
      LOG_ERROR("%s:%d Id \"%u\" Exists for field of learn %s",
                __func__,
                __LINE__,
                learn_field_id,
                learn_name.c_str());
      continue;
    }
    // insert data_field in learn
    learn_field_map[learn_field_id] = std::move(learn_field);
  }

  //////////////////////////
  // create learnInfo     //
  //////////////////////////
  auto learn_info = std::unique_ptr<LearnInfo>(
      new LearnInfo(learn_id,
                    learn_name,
                    std::move(learn_field_map),
                    parseAnnotations(learn_tdi[tdi_json::LEARN_ANNOTATIONS])));
  if (learn_info == nullptr) {
    LOG_ERROR("%s:%d Error forming learnInfo for learn id %d",
              __func__,
              __LINE__,
              learn_id);
  }
  return learn_info;
}

/* The function parses the provided json schema file and return TableInfo
 * object
 */
std::unique_ptr<tdi::TableInfo> TdiInfoParser::parseTable(
    const tdi::Cjson &table_tdi) {
  tdi_id_t table_id = table_tdi[tdi_json::TABLE_ID];
  std::string table_name = table_tdi[tdi_json::TABLE_NAME];
  std::string table_type_s = table_tdi[tdi_json::TABLE_TYPE];
  auto table_type = tableTypeStrToEnum(table_type_s);
  size_t table_size =
      static_cast<unsigned int>(table_tdi[tdi_json::TABLE_SIZE]);
  bool has_const_default_action =
      table_tdi[tdi_json::TABLE_HAS_CONST_DEFAULT_ACTION];
  bool is_const = table_tdi[tdi_json::TABLE_IS_CONST];
  std::map<tdi_id_t, std::unique_ptr<KeyFieldInfo>> table_key_map;
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> table_data_map;
  std::map<tdi_id_t, std::unique_ptr<ActionInfo>> table_action_map;
  std::set<tdi_id_t> depends_on_set;
  SupportedApis table_apis;
  std::set<tdi_operations_type_e> operations_type_set;
  std::set<tdi_attributes_type_e> attributes_type_set;

  LOG_DBG("Table : %s :: Type :: %s ID :: %d SIZE :: %lu",
          table_name.c_str(),
          table_type_s.c_str(),
          table_id,
          table_size);

  ////////////////////
  // getting key   //
  ///////////////////
  tdi::Cjson table_key_cjson = table_tdi[tdi_json::TABLE_KEY];
  for (const auto &key : table_key_cjson.getCjsonChildVec()) {
    std::unique_ptr<KeyFieldInfo> key_field = parseKeyField(*key);
    if (key_field == nullptr) {
      continue;
    }
    auto elem = table_key_map.find(key_field->idGet());
    if (elem == table_key_map.end()) {
      table_key_map[key_field->idGet()] = (std::move(key_field));
    } else {
      // Field id is repeating, log an error message
      LOG_ERROR("%s:%d Field ID %d is repeating",
                __func__,
                __LINE__,
                key_field->idGet());
      return nullptr;
    }
  }

  ////////////////////
  // getting data   //
  ////////////////////
  tdi::Cjson table_data_cjson = table_tdi[tdi_json::TABLE_DATA];
  for (const auto &data_json : table_data_cjson.getCjsonChildVec()) {
    std::string data_name;
    int oneof_size = 1;
    if ((*data_json)["oneof"].exists()) {
      oneof_size = (*data_json)["oneof"].array_size();
    }
    tdi::Cjson temp;
    for (int oneof_loop = 0; oneof_loop < oneof_size; oneof_loop++) {
      auto data_field = parseDataField(*data_json, oneof_loop);
      tdi_id_t data_field_id = data_field->idGet();
      if (table_data_map.find(data_field_id) != table_data_map.end()) {
        LOG_ERROR("%s:%d Id \"%u\" Exists for common data of table %s",
                  __func__,
                  __LINE__,
                  data_field_id,
                  table_name.c_str());
        continue;
      }
      // insert data_field in table
      table_data_map[data_field_id] = std::move(data_field);
    }
  }

  ////////////////////////
  // getting depends on //
  ////////////////////////
  tdi::Cjson depends_on_cjson = table_tdi[tdi_json::TABLE_DEPENDS_ON];
  for (const auto &tbl_id : depends_on_cjson.getCjsonChildVec()) {
    depends_on_set.insert(*tbl_id);
  }

  ////////////////////////
  // getting operations //
  ////////////////////////
  std::vector<std::string> operations_v =
      table_tdi["supported_operations"].getCjsonChildStringVec();
  for (auto const &item : operations_v) {
    operations_type_set.insert(operationsTypeStrToEnum(item));
  }

  ////////////////////////
  // getting attributes //
  ////////////////////////
  std::vector<std::string> attributes_v =
      table_tdi["attributes"].getCjsonChildStringVec();
  for (auto const &item : attributes_v) {
    attributes_type_set.insert(attributesTypeStrToEnum(item));
  }

  ////////////////////
  // getting action //
  ////////////////////
  tdi::Cjson table_action_spec_cjson = table_tdi[tdi_json::TABLE_ACTION_SPECS];
  for (const auto &action : table_action_spec_cjson.getCjsonChildVec()) {
    auto action_info = parseAction(*action);
    auto elem = table_action_map.find(action_info->idGet());
    if (elem == table_action_map.end()) {
      table_action_map[action_info->idGet()] = (std::move(action_info));
    } else {
      // Action id is repeating. Log an error message
      LOG_ERROR("%s:%d Action ID %d is repeating",
                __func__,
                __LINE__,
                action_info->idGet());
      return nullptr;
    }
  }

  //////////////////////////
  // create tableInfo     //
  //////////////////////////
  auto table_info = std::unique_ptr<TableInfo>(
      new TableInfo(table_id,
                    table_name,
                    table_type,
                    table_size,
                    has_const_default_action,
                    is_const,
                    std::move(table_key_map),
                    std::move(table_data_map),
                    std::move(table_action_map),
                    depends_on_set,
                    std::move(table_apis),
                    operations_type_set,
                    attributes_type_set,
                    parseAnnotations(table_tdi[tdi_json::TABLE_ANNOTATIONS])));
  if (table_info == nullptr) {
    LOG_ERROR("%s:%d Error forming tableInfo for table id %d",
              __func__,
              __LINE__,
              table_id);
  }
  return table_info;
}

tdi_status_t TdiInfoParser::parseTdiInfo(
    const std::vector<std::string> &tdi_info_file_paths) {
  // A. read file form a list of schema files
  if (tdi_info_file_paths.empty()) {
    LOG_CRIT("Unable to find any TDI Json Schema File");
    return TDI_OBJECT_NOT_FOUND;
  }
  for (auto const &tdiJsonFile : tdi_info_file_paths) {
    std::ifstream file(tdiJsonFile);
    if (file.fail()) {
      LOG_CRIT("Unable to find TDI Json File %s", tdiJsonFile.c_str());
      return TDI_OBJECT_NOT_FOUND;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    tdi::Cjson root_cjson = tdi::Cjson::createCjsonFromFile(content);
    tdi::Cjson tables_cjson = root_cjson[tdi_json::TABLES];
    for (const auto &table : tables_cjson.getCjsonChildVec()) {
      // B. parse file to form tdi_table_info object
      std::string table_name =
          static_cast<std::string>((*table)[tdi_json::TABLE_NAME]);
      table_info_map_[table_name] = this->parseTable(*table);
    }

    tdi::Cjson learns_cjson = root_cjson[tdi_json::LEARN_FILTERS];
    for (const auto &learn : learns_cjson.getCjsonChildVec()) {
      // C. parse file to form tdi_learn_info object
      std::string learn_name = static_cast<std::string>((*learn)["name"]);
      learn_info_map_[learn_name] = this->parseLearn(*learn);
    }
  }
  return TDI_SUCCESS;
}

}  // namespace tdi
