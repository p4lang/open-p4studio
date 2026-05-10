// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_learn_info.hpp
 *
 *  @brief Contains TDI Learn Info APIs
 */
#ifndef _TDI_LEARN_INFO_HPP
#define _TDI_LEARN_INFO_HPP

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>

namespace tdi {

// Classes that need to be overridden by targets in order for them to
// target-specific information in the info

class LearnContextInfo {};

/**
 * @brief In memory representation of tdi.json Learn
 */
class LearnInfo {
 public:
  /**
   * @brief Get name of the learn
   *
   * @return Name of the learn
   */
  const std::string &nameGet() const { return name_; };

  /**
   * @brief Get ID of the learn
   * @return ID of the learn
   *
   */
  const tdi_id_t &idGet() const { return id_; };

  /**
   * @brief Get a set of annotations on a Learn
   *
   * @return Set of annotations on a Learn
   */
  const std::set<tdi::Annotation> annotationsGet() const {
    return annotations_;
  };

  /**
   * @brief Get vector of DataField IDs. Only applicable for learns
   * without Action IDs
   *
   * @return Vector of IDs
   *
   */
  std::vector<tdi_id_t> dataFieldIdListGet() const;

  /**
   * @brief Get the field ID of a Data Field from a name.
   *
   * @param[in] name Name of a Data field
   * @return Field ID. 0 if not found
   */
  tdi_id_t dataFieldIdGet(const std::string &name) const;

  /**
   * @brief Get the data Field info object from tdi_id.
   *
   * @param[in] id id of a Data field
   * @return DataFieldInfo object. nullptr if not found
   *
   */
  const DataFieldInfo *dataFieldGet(const tdi_id_t &field_id) const;

  /**
   * @brief Set learnContextInfo object.
   *
   * @return learnContextInfo object.
   *
   */
  void learnContextInfoSet(
      std::unique_ptr<LearnContextInfo> learn_context_info) const {
    learn_context_info_ = std::move(learn_context_info);
  };

  /**
   * @brief Get learnContextInfo object.
   *
   * @return learnContextInfo object.
   *
   */
  const LearnContextInfo *learnContextInfoGet() const {
    return learn_context_info_.get();
  };

 private:
  LearnInfo(tdi_id_t id,
            std::string name,
            std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> learn_field_map,
            std::set<Annotation> annotations)
      : id_(id),
        name_(name),
        learn_field_map_(std::move(learn_field_map)),
        annotations_(annotations){};
  tdi_id_t id_;
  std::string name_;
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> learn_field_map_;
  std::set<Annotation> annotations_{};
  mutable std::unique_ptr<LearnContextInfo> learn_context_info_;
  friend class TdiInfoParser;
};

}  // namespace tdi

#endif  // _TDI_LEARN_HPP
