// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tna_info.hpp
 *
 *  @brief Contains TDI Info classes for TNA
 *  Object
 */
#ifndef _TNA_INFO_HPP
#define _TNA_INFO_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_info.hpp>

#include <tdi/arch/tna/tna_defs.h>

/**
 * @brief Namespace for TDI
 */
namespace tdi {

namespace tdi_json {
namespace values {
namespace tna {
const std::string TABLE_KEY_MATCH_TYPE_ATCAM = "ATCAM";
}  // namespace tna
}  // namespace values
}  // namespace tdi_json

namespace tna {

class TdiInfoMapper : public tdi::TdiInfoMapper {
 public:
  TdiInfoMapper() {
    // Match types
    // ATCAM match type treated as exact match
    matchEnumMapAdd(tdi_json::values::tna::TABLE_KEY_MATCH_TYPE_ATCAM,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_EXACT));
  }
};

/**
 * @brief Class to help create the correct Table object with the
 * help of a map. Targets should override
 */
class TableFactory : public tdi::TableFactory {
 public:
  virtual std::unique_ptr<tdi::Table> makeTable(
      const TdiInfo * /*tdi_info*/,
      const tdi::TableInfo * /*table_info*/) const override {
    // No tables in TNA currently. Will eventually have Selector, Action profile
    // etc
    return nullptr;
  };
};

}  // namespace tna
}  // namespace tdi

#endif
