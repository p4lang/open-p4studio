// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file pna_info.hpp
 *
 *  @brief Contains TDI Info classes for PNA
 *  Object
 */
#ifndef _PNA_INFO_HPP
#define _PNA_INFO_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_info.hpp>

#include <tdi/arch/pna/pna_defs.h>

/**
 * @brief Namespace for TDI
 */
namespace tdi {

namespace tdi_json {
namespace values {
namespace pna {
const std::string RANGE = "RANGE";
}  // namespace pna
}  // namespace values
}  // namespace tdi_json

namespace pna {

class TdiInfoMapper : public tdi::TdiInfoMapper {
 public:
  TdiInfoMapper() {
    // Match types
    matchEnumMapAdd(tdi_json::values::pna::RANGE,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_RANGE));
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
    // No tables in PNA currently. Will eventually have Selector, Action profile
    // etc
    return nullptr;
  };
};

}  // namespace pna
}  // namespace tdi

#endif
