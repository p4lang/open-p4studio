// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_table_info.hpp
 *
 *  @brief Contains TDI Table Info APIs
 */
#ifndef _TDI_TABLE_INFO_HPP
#define _TDI_TABLE_INFO_HPP

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>

namespace tdi {

// Forward declarations
class TableInfo;
class ActionInfo;
class KeyFieldInfo;
class DataFieldInfo;
class Cjson;
class TdiInfoMapper;

// Classes that need to be overridden by targets in order for them to
// target-specific information in the info

class TableContextInfo {};
class KeyFieldContextInfo {};
class DataFieldContextInfo {};
class ActionContextInfo {};

/**
 * @brief Class for Annotations. Contains 2 strings to uniquely identify an
 * annotation. These annotations can be on a Table, Action, Key or Data Field
 *
 * A possible list of annotations are below.
 *  1. ("$tdi_field_class","register_data") If a Data field is a register data.
 *      Register data fields are to be set using one value but while
 *      tableEntryGet, this field returns a vector(one value for each field)
 *  2. ("$tdi_port_type_class","dev_port") If the data field is a dev_port.
 *  3. ("$tdi_port_type_class","port_hdl_conn_id") If the data field is a port
 *      handle conn ID.
 *  4. ("$tdi_port_type_class","port_hdl_chnl_id") If the data field is a port
 *      handle channel ID.
 *  5. ("$tdi_port_type_class","port_name") If the data field is a port name.
 *  6. ("$tdi_port_type_class","fp_port") If the data field is a front panel
 *      port.
 *  7. ("isFieldSlice","true") If a Data field is a slice of bigger field.
 *  8. ("@defaultonly","") If an action is to be default only
 *  9. ("$tdi_field_imp_level", "level") Importance level of a field. All
 *fields
 *     start off with an importance level of 1
 */
class Annotation {
 public:
  Annotation(std::string name, std::string value) : name_(name), value_(value) {
    full_name_ = name_ + "." + value_;
  };
  bool operator<(const Annotation &other) const;
  bool operator==(const Annotation &other) const;
  bool operator==(const std::string &other_str) const;
  tdi_status_t fullNameGet(std::string *fullName) const;
  const std::string name_{""};
  const std::string value_{""};
  struct Less {
    bool operator()(const Annotation &lhs, const Annotation &rhs) const {
      return lhs < rhs;
    }
  };

 private:
  std::string full_name_{""};
};

class SupportedApis {
 public:
  SupportedApis(){};
  SupportedApis(
      std::map<tdi_table_api_type_e, std::vector<std::string>> api_map)
      : api_target_attributes_map_(api_map){};
  // Contains a map of supported API to the supported target attribtues like
  // dev_id, pipe_id, pipe_all etc. These target attribtues are device specific
  std::map<tdi_table_api_type_e, std::vector<std::string>>
      api_target_attributes_map_{};
};

class KeyFieldInfo {
 public:
  /**
   * @name Key APIs
   * @{
   */

  /**
   * @brief Get match type of Key Field
   *
   * @return Match Type (Exact/Ternary/LPM ... )
   *
   */
  const tdi_match_type_e &matchTypeGet() const { return match_type_; };

  /**
   * @brief Get the Data type of a Key field (INT/BOOL/ENUM/INT_ARR/BOOL_ARR)
   *
   * @return Field Type (uint64, float, string etc)
   */
  const tdi_field_data_type_e &dataTypeGet() const { return data_type_; };

  /**
   * @brief Get field size
   *
   * @return Field Size in bits
   */
  const size_t &sizeGet() const { return size_bits_; };

  /**
   * @brief Is field Slice
   *
   * @return if the key is a field slice of a bigger field
   */
  const bool &isFieldSlice() const { return is_field_slice_; };

  /**
   * @brief Get whether Key Field is of type ptr or not. This is true for
   * fields of size>64 bits and false if <=64
   * If the field is
   * of ptr type, then only byte array sets/gets are applicable on the field.
   * Else both the ptr versions and the primitive template versions work
   *
   * @return Boolean type indicating whether Field is of type ptr
   *
   */
  const bool &isPtrGet() const { return is_ptr_; };

  /**
   * @brief Get name of field
   *
   * @return Field name
   */
  const std::string &nameGet() const { return name_; };

  /**
   * @brief Get a list of all the allowed values that a particular field can
   * have. This API is only for fields with string type. If the returned
   * vector is empty, it indicates that the allowed choices have not been
   * published in tdi json
   *
   * @return choices Vector of the string values that are
   * allowed for this field
   *
   */
  const std::vector<std::string> &choicesGet() const { return enum_choices_; };

  /**
   * @brief Get field ID
   * @return field ID
   */
  const tdi_id_t &idGet() const { return field_id_; };

  /** @} */  // End of group Key

  /**
   * @brief Set keyFieldContextInfo object.
   *
   * @return keyFieldContextInfo object.
   *
   */
  void keyFieldContextInfoSet(
      std::unique_ptr<KeyFieldContextInfo> key_field_context_info) const {
    key_field_context_info_ = std::move(key_field_context_info);
  };

  /**
   * @brief Get keyFieldContextInfo object.
   *
   * @return keyFieldContextInfo object.
   *
   */
  const KeyFieldContextInfo *keyFieldContextInfoGet() const {
    return key_field_context_info_.get();
  };

 private:
  KeyFieldInfo(tdi_id_t field_id,
               std::string name,
               size_t size_bits,
               tdi_match_type_e match_type,
               tdi_field_data_type_e data_type,
               bool mandatory,
               const std::vector<std::string> &enum_choices,
               const std::set<tdi::Annotation> &annotations,
               uint64_t default_value,
               float default_fl_value,
               std::string default_str_value,
               bool is_field_slice,
               bool is_ptr,
               bool match_priority)
      : field_id_(field_id),
        name_(name),
        size_bits_(size_bits),
        match_type_(match_type),
        data_type_(data_type),
        mandatory_(mandatory),
        enum_choices_(enum_choices),
        annotations_(annotations),
        default_value_(default_value),
        default_fl_value_(default_fl_value),
        default_str_value_(default_str_value),
        is_field_slice_(is_field_slice),
        is_ptr_(is_ptr),
        match_priority_(match_priority){};
  const tdi_id_t field_id_;
  const std::string name_;
  const size_t size_bits_;
  const tdi_match_type_e match_type_;
  const tdi_field_data_type_e data_type_;
  const bool mandatory_;
  const std::vector<std::string> enum_choices_;
  const std::set<tdi::Annotation> annotations_;

  const uint64_t default_value_;
  const float default_fl_value_;
  const std::string default_str_value_;

  const bool is_field_slice_{false};
  const bool is_ptr_{false};
  const bool match_priority_{false};
  mutable std::unique_ptr<KeyFieldContextInfo> key_field_context_info_;
  friend class TdiInfoParser;
};  // class KeyFieldInfo

class DataFieldInfo {
 public:
  /**
   * @brief Get vector of DataField IDs for a container's field id.
   *
   * @param[out] id Vector of IDs
   *
   * @return Status of the API call
   */
  std::vector<tdi_id_t> containerDataFieldIdListGet() const;

  /**
   * @brief Get the Size of a field.
   * For container fields this function will return number
   * of elements inside the container.
   *
   * @return Size of the field in bits
   */
  const size_t &sizeGet() const { return size_bits_; };

  /**
   * @brief Get whether a field is a ptr type.
   * Only the ptr versions of setValue/getValue will work on fields
   * for which this API returns true
   *
   * @return Boolean value indicating if it is ptr type
   *
   */
  const bool &isPtrGet() const { return is_ptr_; };

  /**
   * @brief Get whether a field is mandatory.
   *
   * @return Boolean value indicating if it is mandatory
   */
  const bool &mandatoryGet() const { return mandatory_; };

  /**
   * @brief Get whether a field is ReadOnly.
   *
   * @return Boolean value indicating if it is ReadOnly
   */
  const bool &readOnlyGet() const { return read_only_; };

  /**
   * @brief Get the IDs of oneof siblings of a field. If a field is part of a
   * oneof , for example, consider $ACTION_MEMBER_ID and $SELECTOR_GROUP_ID.
   * then this API will return [field_ID($ACTION_MEMBER_ID)] for
   * $SELECTOR_GROUP_ID.
   *
   *
   * @return Set containing field IDs of oneof siblings
   *
   */
  const std::set<tdi_id_t> &oneofSiblingsGet() const {
    return oneof_siblings_;
  };

  /**
   * @brief Get the Name of a field.
   *
   * @return Name of the field
   */
  const std::string &nameGet() const { return name_; };

  /**
   * @brief Get the Data type of a field (INT/BOOL/ENUM/INT_ARR/BOOL_ARR)
   *
   * @return Data type of a data field
   */
  const tdi_field_data_type_e &dataTypeGet() const { return data_type_; };

  /**
   * @brief Get a list of all the allowed values that a particular field can
   * have. This API is only for fields with string type. If the returned
   * vector is empty, it indicates that the allowed choices have not been
   * published in tdi json. If called for non-string fields, then also empty
   * vector would be returned
   *
   * @return Vector of const references of the values that are
   * allowed for this field
   *
   */
  const std::vector<std::string> &allowedChoicesGet() const {
    return enum_choices_;
  };

  /**
   * @brief Get a set of annotations on a data field
   *
   * @param[out] annotations Set of annotations on a data field
   *
   * @return Status of the API call
   */
  const std::set<tdi::Annotation> &annotationsGet() const {
    return annotations_;
  };

  /**
   * @brief Get ID of field
   * @return ID of field
   */
  const tdi_id_t &idGet() const { return field_id_; }

  /** @} */  // End of group Data

  /**
   * @brief Set dataFieldContextInfo object.
   *
   * @return dataFieldContextInfo object.
   *
   */
  void dataFieldContextInfoSet(
      std::unique_ptr<DataFieldContextInfo> data_field_context_info) const {
    data_field_context_info_ = std::move(data_field_context_info);
  };

  /**
   * @brief Get dataFieldContextInfo object.
   *
   * @return dataFieldContextInfo object.
   *
   */
  const DataFieldContextInfo *dataFieldContextInfoGet() const {
    return data_field_context_info_.get();
  };

  /**
   * @brief Get defaultValue of data object.
   *
   * @return default value of data object as uint64.
   *
   */
  uint64_t defaultValueGet() const { return default_value_; }

  /**
   * @brief Get defaultValue of data object.
   *
   * @return default value of data object as string.
   *
   */
  std::string defaultStrValueGet() const { return default_str_value_; }

  /**
   * @brief Get defaultValue of data object.
   *
   * @return default value of data object as float.
   *
   */
  float defaultFlValueGet() const { return default_fl_value_; }

 private:
  DataFieldInfo(tdi_id_t field_id,
                std::string name,
                size_t size_bits,
                tdi_field_data_type_e data_type,
                bool mandatory,
                bool read_only,
                const std::vector<std::string> &enum_choices,
                const std::set<tdi::Annotation> annotations,
                uint64_t default_value,
                float default_fl_value,
                std::string default_str_value,
                bool repeated,
                bool container_valid,
                std::set<tdi_id_t> oneof_siblings)
      : field_id_(field_id),
        name_(name),
        size_bits_(size_bits),
        data_type_(data_type),
        mandatory_(mandatory),
        read_only_(read_only),
        enum_choices_(enum_choices),
        annotations_(annotations),
        default_value_(default_value),
        default_fl_value_(default_fl_value),
        default_str_value_(default_str_value),
        repeated_(repeated),
        container_valid_(container_valid),
        oneof_siblings_(oneof_siblings){};
  const tdi_id_t field_id_;
  const std::string name_;
  const size_t size_bits_;
  const tdi_field_data_type_e data_type_;
  const bool is_ptr_{false};
  const bool mandatory_;
  const bool read_only_;
  const std::vector<std::string> enum_choices_;
  const std::set<tdi::Annotation> annotations_;
  const uint64_t default_value_;
  const float default_fl_value_;
  const std::string default_str_value_;
  const bool repeated_;
  const bool container_valid_{false};
  const std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> container_;
  const std::map<std::string, tdi_id_t> container_names_;
  const std::set<tdi_id_t> oneof_siblings_;
  mutable std::unique_ptr<DataFieldContextInfo> data_field_context_info_;
  friend class TdiInfoParser;
};

// Action ID APIs
class ActionInfo {
 public:
  /**
   * @name Action ID APIs
   * @{
   */
  /**
   * @brief Get Action ID
   * @return Action ID
   */
  const tdi_id_t &idGet() const { return action_id_; };

  /**
   * @brief Get Action Name
   * @return Action Name
   */
  const std::string &nameGet() const { return name_; };

  /**
   * @brief Get a set of annotations for this action.
   *
   * @return Set of annotations
   */
  const std::set<tdi::Annotation> &annotationsGet() const {
    return annotations_;
  };
  /** @} */  // End of group Action IDs

  /**
   * @brief Set actionContextInfo object.
   *
   * @return actionContextInfo object.
   *
   */
  void actionContextInfoSet(
      std::unique_ptr<ActionContextInfo> action_context_info) const {
    action_context_info_ = std::move(action_context_info);
  };

  /**
   * @brief Get actionContextInfo object.
   *
   * @return actionContextInfo object.
   *
   */
  const ActionContextInfo *actionContextInfoGet() const {
    return action_context_info_.get();
  };

  /**
   * @brief Get action data fields map.
   *
   * @return action's id<->dataField map.
   *
   */
  const std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> &actionDataMapGet()
      const {
    return data_fields_;
  };

  // Map of table_data_fields with names
  std::map<std::string, const DataFieldInfo *> data_fields_names_;

 private:
  ActionInfo(tdi_id_t field_id,
             std::string name,
             std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> data_fields,
             std::set<tdi::Annotation> annotations)
      : action_id_(field_id),
        name_(name),
        data_fields_(std::move(data_fields)),
        annotations_(annotations) {
    // update relevant map
    for (const auto &kv : data_fields_) {
      const auto data_field = kv.second.get();
      data_fields_names_[data_field->nameGet()] = data_field;
    }
  };

  const tdi_id_t action_id_;
  const std::string name_;
  // Map of table_data_fields
  const std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> data_fields_;
  const std::set<tdi::Annotation> annotations_;
  mutable std::unique_ptr<ActionContextInfo> action_context_info_;
  friend class TableInfo;
  friend class TdiInfoParser;
};

/**
 * @brief In memory representation of tdi.json Table
 */
class TableInfo {
 public:
  /**
   * @brief Get name of the table
   *
   * @return Name of the table
   */
  const std::string &nameGet() const { return name_; };

  /**
   * @brief Get ID of the table
   * @return ID of the table
   */
  const tdi_id_t &idGet() const { return id_; };

  /**
   * @brief The type of the table
   * @return Type of the table
   */
  const tdi_table_type_e &tableTypeGet() const { return table_type_; };

  /**
   * @brief Size of a table acc to tdi.json
   *
   * @return Number of total available size
   */
  const size_t &sizeGet() const { return size_; };
  /**
   * @brief Get whether this table has a const default action
   *
   * @return If default action is const
   */
  const bool &hasConstDefaultAction() const {
    return has_const_default_action_;
  };

  /**
   * @brief Get whether this table is a const table
   *
   * @param[out] is_const If table is const
   *
   * @return Status of the API call
   */
  const bool &isConst() const { return is_const_; };

  /**
   * @brief Get a set of annotations on a Table
   * @return Set of annotations on a Table
   */
  const std::set<Annotation> &annotationsGet() const { return annotations_; };

  /**
   * @brief Get a set of APIs which are supported by this table.
   * @return The set of APIs which are supported by this table
   */
  const SupportedApis &apiSupportedGet() const { return table_apis_; };

  /**
   * @brief Set supported APIs
   * @return void
   */
  void apiSupportedSet(tdi::SupportedApis table_apis) const {
    table_apis_ = std::move(table_apis);
  };

  /**
   * @brief Get the set of supported attributes
   *
   * @return Set of the supported Attributes
   */
  const std::set<tdi_attributes_type_e> &attributesSupported() const {
    return attributes_type_set_;
  };
  /**
   * @brief Get set of supported Operations
   * @return Set of supported Operations
   */
  const std::set<tdi_operations_type_e> &operationsSupported() const {
    return operations_type_set_;
  };

  /**
   * @brief Get set of table_ids this table depends on
   * @return Set of table_ids this table depends on
   */
  const std::set<tdi_id_t> &dependsOnGet() const { return depends_on_set_; };

  /**
   * @brief Get a vector of Key field IDs
   * @return Vector of Key field IDs
   */
  std::vector<tdi_id_t> keyFieldIdListGet() const;

  /**
   * @brief Get Key Field from name
   *
   * @param[in] name name of field
   * @return key_field_info KeyFieldInfo object. nullptr if not found
   */
  const KeyFieldInfo *keyFieldGet(const std::string &name) const;

  /**
   * @brief Get Key Field from tdi_id
   *
   * @param[in] id Key Field ID
   * @return key_field_info KeyFieldInfo object. nullptr if not found
   */
  const KeyFieldInfo *keyFieldGet(const tdi_id_t &field_id) const;

  /**
   * @brief Get vector of DataField IDs. Will return non-action (common)
   * data field IDs if any actions exist
   *
   * @return Vector of IDs
   */
  std::vector<tdi_id_t> dataFieldIdListGet() const;

  /**
   * @brief Get vector of DataField IDs for a particular action. If action
   * doesn't exist, then common fields list is returned.
   *
   * @param[in] action_id Action ID
   * @return Vector of ID.
   */
  std::vector<tdi_id_t> dataFieldIdListGet(const tdi_id_t &action_id) const;

  /**
   * @brief Get the field ID of a Data Field from a name.
   *
   * @param[in] name Name of a Data field
   * @return Field ID. 0 if not found
   *
   */
  tdi_id_t dataFieldIdGet(const std::string &name) const;

  /**
   * @brief Get the field ID of a Data Field from a name
   *
   * @param[in] name Name of a Data field
   * @param[in] action_id Action ID
   * @return Field ID. 0 if not found
   *
   */
  tdi_id_t dataFieldIdGet(const std::string &name,
                          const tdi_id_t &action_id) const;

  /**
   * @brief Get the data Field info object from name.
   *
   * @param[in] name name of a Data field
   * @return DataFieldInfo object. nullptr if doesn't exist
   *
   */
  const DataFieldInfo *dataFieldGet(const std::string &name) const;

  /**
   * @brief Get the data Field info object from name.
   *
   * @param[in] name name of a Data field
   * @param[in] action_id Action ID
   * @return DataFieldInfo object. nullptr if doesn't exist
   *
   */
  const DataFieldInfo *dataFieldGet(const std::string &name,
                                    const tdi_id_t &action_id) const;

  /**
   * @brief Get the data Field info object from tdi_id.
   *
   * @param[in] id id of a Data field
   * @return DataFieldInfo object. nullptr if doesn't exist
   *
   */
  const DataFieldInfo *dataFieldGet(const tdi_id_t &field_id) const;

  /**
   * @brief Get the data Field info object from tdi_id.
   *
   * @param[in] id id of a Data field
   * @param[in] action_id Action ID
   * @return DataFieldInfo object. nullptr if doesn't exist
   *
   */
  const DataFieldInfo *dataFieldGet(const tdi_id_t &field_id,
                                    const tdi_id_t &action_id) const;

  /**
   * @brief Get vector of Action IDs
   * @return Vector of Action IDs
   */
  std::vector<tdi_id_t> actionIdListGet() const;

  /**
   * @brief Get ActionInfo object from action name
   *
   * @param[in] name Name of Action
   * @param[out] action_info ActionInfo object
   *
   * @return Status of the API call
   */
  const ActionInfo *actionGet(const std::string &name) const;

  /**
   * @brief Get ActionInfo object from tdi_id of action (action_id)
   *
   * @param[in] action_id tdi_id of Action
   * @param[out] action_info ActionInfo object
   *
   * @return Status of the API call
   */
  const ActionInfo *actionGet(const tdi_id_t &action_id) const;

  /**
   * @brief Set tableContextInfo object.
   *
   * @return tableContextInfo object.
   *
   */
  void tableContextInfoSet(
      std::unique_ptr<TableContextInfo> table_context_info) const {
    table_context_info_ = std::move(table_context_info);
  };

  /**
   * @brief Get tableContextInfo object.
   *
   * @return tableContextInfo object.
   *
   */
  const TableContextInfo *tableContextInfoGet() const {
    return table_context_info_.get();
  };

  /**
   * @brief Get table key fields map.
   *
   * @return table's id<->keyField map.
   *
   */
  const std::map<tdi_id_t, std::unique_ptr<KeyFieldInfo>> &tableKeyMapGet()
      const {
    return table_key_map_;
  }

  /**
   * @brief Get table data fields map.
   *
   * @return table's id<->dataField map.
   *
   */
  const std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> &tableDataMapGet()
      const {
    return table_data_map_;
  }

  /**
   * @brief Get table action map.
   *
   * @return table's id<->action map.
   *
   */
  const std::map<tdi_id_t, std::unique_ptr<ActionInfo>> &tableActionMapGet()
      const {
    return table_action_map_;
  }

  std::map<std::string, const KeyFieldInfo *> name_key_map_;
  std::map<std::string, const DataFieldInfo *> name_data_map_;
  std::map<std::string, const ActionInfo *> name_action_map_;

 private:
  TableInfo(tdi_id_t id,
            std::string name,
            tdi_table_type_e table_type,
            size_t size,
            bool has_const_default_action,
            bool is_const,
            std::map<tdi_id_t, std::unique_ptr<KeyFieldInfo>> table_key_map,
            std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> table_data_map,
            std::map<tdi_id_t, std::unique_ptr<ActionInfo>> table_action_map,
            std::set<tdi_id_t> depends_on_set,
            SupportedApis table_apis,
            std::set<tdi_operations_type_e> operations_type_set,
            std::set<tdi_attributes_type_e> attributes_type_set,
            std::set<Annotation> annotations)
      : id_(id),
        name_(name),
        table_type_(table_type),
        size_(size),
        has_const_default_action_(has_const_default_action),
        is_const_(is_const),
        table_key_map_(std::move(table_key_map)),
        table_data_map_(std::move(table_data_map)),
        table_action_map_(std::move(table_action_map)),
        depends_on_set_(depends_on_set),
        table_apis_(std::move(table_apis)),
        operations_type_set_(operations_type_set),
        attributes_type_set_(attributes_type_set),
        annotations_(annotations) {
    // update relevant maps
    for (const auto &kv : table_key_map_) {
      const KeyFieldInfo *key_field = kv.second.get();
      name_key_map_[key_field->nameGet()] = key_field;
    }

    for (const auto &kv : table_action_map_) {
      const auto action = kv.second.get();
      name_action_map_[action->nameGet()] = action;
    }

    for (const auto &kv : table_data_map_) {
      const auto data_field = kv.second.get();
      name_data_map_[data_field->nameGet()] = data_field;
    }
  };

  const tdi_id_t id_;
  const std::string name_;
  const tdi_table_type_e table_type_;
  const size_t size_;
  const bool has_const_default_action_{false};
  const bool is_const_{false};
  const std::map<tdi_id_t, std::unique_ptr<KeyFieldInfo>> table_key_map_;
  const std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> table_data_map_;
  const std::map<tdi_id_t, std::unique_ptr<ActionInfo>> table_action_map_;
  const std::set<tdi_id_t> depends_on_set_;
  mutable tdi::SupportedApis table_apis_{};
  const std::set<tdi_operations_type_e> operations_type_set_;
  const std::set<tdi_attributes_type_e> attributes_type_set_;
  const std::set<Annotation> annotations_{};

  mutable std::unique_ptr<TableContextInfo> table_context_info_;
  friend class TdiInfoParser;
};

}  // namespace tdi

#endif  // _TDI_TABLE_INFO_HPP
