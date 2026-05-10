// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_info.hpp
 *
 *  @brief Contains TDI Info APIs. Get Table and Learn Object from TdiInfo
 *  Object
 */
#ifndef _TDI_INFO_HPP
#define _TDI_INFO_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_json_parser/tdi_info_parser.hpp>
#include <tdi/common/tdi_learn.hpp>
#include <tdi/common/tdi_table.hpp>

/**
 * @brief Namespace for TDI
 */
namespace tdi {

namespace tdi_json {
namespace values {
namespace core {
const std::string TABLE_KEY_MATCH_TYPE_EXACT = "Exact";
const std::string TABLE_KEY_MATCH_TYPE_TERNARY = "Ternary";
const std::string TABLE_KEY_MATCH_TYPE_LPM = "LPM";
const std::string TABLE_KEY_MATCH_TYPE_RANGE = "Range";
}  // namespace core
}  // namespace values
}  // namespace tdi_json

// Forward declaration
class TdiInfo;

class TdiInfoMapper {
 public:
  const std::map<std::string, tdi_table_type_e> &tableEnumMapGet() {
    return table_e_map_;
  };
  const std::map<std::string, tdi_match_type_e> &matchEnumMapGet() {
    return match_e_map_;
  };
  const std::map<std::string, tdi_operations_type_e> &operationsEnumMapGet() {
    return operations_e_map_;
  };
  const std::map<std::string, tdi_attributes_type_e> &attributesEnumMapGet() {
    return attributes_e_map_;
  };

  TdiInfoMapper() {
    // Match types
    matchEnumMapAdd(tdi_json::values::core::TABLE_KEY_MATCH_TYPE_EXACT,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_EXACT));
    matchEnumMapAdd(tdi_json::values::core::TABLE_KEY_MATCH_TYPE_TERNARY,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_TERNARY));
    matchEnumMapAdd(tdi_json::values::core::TABLE_KEY_MATCH_TYPE_LPM,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_LPM));
    matchEnumMapAdd(tdi_json::values::core::TABLE_KEY_MATCH_TYPE_RANGE,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_RANGE));
  }
  virtual ~TdiInfoMapper(){};

 protected:
  tdi_status_t tableEnumMapAdd(const std::string &str,
                               const tdi_table_type_e &type) {
    if (table_e_map_.find(str) != table_e_map_.end()) {
      return TDI_ALREADY_EXISTS;
    }
    table_e_map_[str] = type;
    return TDI_SUCCESS;
  }
  tdi_status_t matchEnumMapAdd(const std::string &str,
                               const tdi_match_type_e &type) {
    if (match_e_map_.find(str) != match_e_map_.end()) {
      return TDI_ALREADY_EXISTS;
    }
    match_e_map_[str] = type;
    return TDI_SUCCESS;
  }
  tdi_status_t operationsEnumMapAdd(const std::string &str,
                                    const tdi_operations_type_e &type) {
    if (operations_e_map_.find(str) != operations_e_map_.end()) {
      return TDI_ALREADY_EXISTS;
    }
    operations_e_map_[str] = type;
    return TDI_SUCCESS;
  }
  tdi_status_t attributesEnumMapAdd(const std::string &str,
                                    const tdi_attributes_type_e &type) {
    if (attributes_e_map_.find(str) != attributes_e_map_.end()) {
      return TDI_ALREADY_EXISTS;
    }
    attributes_e_map_[str] = type;
    return TDI_SUCCESS;
  }

 private:
  std::map<std::string, tdi_table_type_e> table_e_map_;
  std::map<std::string, tdi_match_type_e> match_e_map_;
  std::map<std::string, tdi_operations_type_e> operations_e_map_;
  std::map<std::string, tdi_attributes_type_e> attributes_e_map_;
};

/**
 * @brief Class to help create the correct Table object with the
 * help of a map. Targets/Arch should override
 */
class TableFactory {
 public:
  virtual std::unique_ptr<tdi::Table> makeTable(
      const TdiInfo * /*tdi_info*/,
      const tdi::TableInfo * /*table_info*/) const {
    // No tables in core currently
    return nullptr;
  };
};

/**
 * @brief Class to maintain metadata of all tables and learn objects. Note that
 *    all the objects returned are representations of the actual HW tables.\n So
 *    TdiInfo doesn't provide ownership of any of its internal structures.
 *    Furthermore, all the metadata is read-only hence only const pointers are
 *    returned.<br>
 * <B>Creation: </B> Cannot be created. Can only be retrived using \ref
 * tdi::Device::tdiInfoGet()
 */
class TdiInfo {
 public:
  /**
   * @brief Destructor destroys all metadata contained in this object.
   */
  virtual ~TdiInfo() = default;

  /**
   * @brief Static function to create TdiInfo from a program config.
   * The ProgramConfig creates information even if no "P4 Program"
   * is involved and they are all non-p4 tables
   *
   * @param program_config Program config
   *
   * @return unique_ptr to TdiInfo
   */
  std::unique_ptr<const TdiInfo> static makeTdiInfo(
      const std::string &p4_name,
      std::unique_ptr<TdiInfoParser> tdi_info_parser,
      const tdi::TableFactory *factory);

  /**
   * @brief Get all the tdi::Table objs.
   *
   * @param[out] table_vec_ret Vector of tdi::Table obj pointers
   *
   * @return Status of the API call
   */
  tdi_status_t tablesGet(std::vector<const tdi::Table *> *table_vec_ret) const;
  /**
   * @brief Get a tdi::Table obj from its fully qualified name
   *
   * @param[in] name Fully qualified P4 table name
   * @param[out] table_ret tdi::Table obj pointer
   *
   * @return Status of the API call
   */
  tdi_status_t tableFromNameGet(const std::string &name,
                                const tdi::Table **table_ret) const;
  /**
   * @brief Get a tdi::Table obj from its ID
   *
   * @param[in] id ID of the tdi::Table obj
   * @param[out] table_ret tdi::Table obj pointer
   *
   * @return Status of the API call
   */
  tdi_status_t tableFromIdGet(const tdi_id_t &id,
                              const tdi::Table **table_ret) const;

  /**
   * @brief Get all the tdi::Learn Objs
   *
   * @param[out] learn_vec_ret Vector of tdi::Learn Obj pointers
   *
   * @return Status of the API call
   */
  tdi_status_t learnsGet(std::vector<const tdi::Learn *> *learn_vec_ret) const;
  /**
   * @brief Get a tdi::Learn Object from its fully qualified name
   *
   * @param[in] name Fully qualified P4 Learn Obj name
   * @param[out] learn_ret tdi::Learn Obj Pointer
   *
   * @return Status of the API call
   */
  tdi_status_t learnFromNameGet(std::string name,
                                const tdi::Learn **learn_ret) const;
  /**
   * @brief Get a tdi::Learn Obj from its ID
   *
   * @param[in] id ID of the tdi::Learn Obj
   * @param[out] learn_ret tdi::Learn Obj pointer
   *
   * @return Status of the API call
   */
  tdi_status_t learnFromIdGet(tdi_id_t id, const tdi::Learn **learn_ret) const;

  /**
   * @brief Get the p4_name associated with this TdiInfo object. Each TdiInfo is
   * associated with a P4 program. If no P4 name exists, then the device can
   * choose to assign an empty string or preferabley a reserved name like
   * "$SHARED".
   *
   * @return std::string P4 name
   *
   */
  const std::string &p4NameGet() const { return p4_name_; };

  /**
   * @brief Get name and tdi::Table map
   *
   * @return map of name and tdi::Table
   */
  const std::map<std::string, std::unique_ptr<tdi::Table>> &tableMapGet() const;

  /**
   * @brief Get name and tdi::Learn map
   *
   * @return map of name and tdi::Learn
   */
  const std::map<std::string, std::unique_ptr<tdi::Learn>> &learnMapGet() const;

  TdiInfo(TdiInfo const &) = delete;
  TdiInfo(TdiInfo &&) = delete;
  TdiInfo() = delete;
  TdiInfo &operator=(const TdiInfo &) = delete;
  TdiInfo &operator=(TdiInfo &&) = delete;

  /* Main P4_info map. object_name --> tdi_info object */
  std::map<std::string, std::unique_ptr<tdi::Table>> tableMap;

 private:
  TdiInfo(const std::string &p4_name,
          std::unique_ptr<TdiInfoParser> tdi_info_parser,
          const tdi::TableFactory *factory);

  // This is the map which is to be queried when a name lookup for a table
  // happens. Multiple names can point to the same table because multiple
  // names can exist for a table. Example, switchingress.forward and forward
  // both are valid for a table if no conflicts with other table is present
  std::map<std::string, const tdi::Table *> fullTableMap;

  /* Reverse map in case lookup from ID is needed*/
  std::map<tdi_id_t, const tdi::Table *> tableIdMap;

  // Learn Map
  std::map<std::string, std::unique_ptr<tdi::Learn>> learnMap;
  std::map<std::string, const tdi::Learn *> fullLearnMap;
  std::map<tdi_id_t, const tdi::Learn *> learnIdMap;

  // Set of optimized out table names. Tables that may be present
  // in TDI.json but target decided not to have a table object present
  // for it at all.
  // Target can add tables to this set if needed
  mutable std::set<std::string> invalid_table_names;

  // Each TdiInfo is associated with a P4 program. If no P4 name exists, then
  // the device can choose to assign an empty string or preferabley a reserved
  // name like "$SHARED".
  const std::string p4_name_;
  std::unique_ptr<TdiInfoParser> tdi_info_parser_;
};

}  // namespace tdi

#endif
