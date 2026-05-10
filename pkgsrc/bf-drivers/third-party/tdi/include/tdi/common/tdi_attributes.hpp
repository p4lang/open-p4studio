// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_table_attributes.hpp
 *
 *  @brief Contains TDI Table Attribute APIs
 */

#ifndef _TDI_TABLE_ATTRIBUTES_HPP
#define _TDI_TABLE_ATTRIBUTES_HPP

#include <bitset>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_table_key.hpp>
#include <tdi/common/tdi_target.hpp>

namespace tdi {

class Table;

class TableAttributes {
 public:
  virtual ~TableAttributes() = default;
  TableAttributes(const Table *table, const tdi_attributes_type_e &attr_type)
      : table_(table), attr_type_(attr_type){};
  virtual tdi_status_t setValue(const tdi_attributes_field_type_e &/*type*/,
                        const uint64_t & /*value*/) {
    return TDI_NOT_SUPPORTED;
  };
  virtual tdi_status_t getValue(const tdi_attributes_field_type_e &/*type*/,
                        uint64_t * /*value*/) const {
    return TDI_NOT_SUPPORTED;
  };
  virtual tdi_status_t setValue(const tdi_attributes_field_type_e &/*type*/,
                        const uint8_t * /*value*/,
                        const size_t & /*size_bytes*/) {
    return TDI_NOT_SUPPORTED;
  };
  virtual tdi_status_t getValue(const tdi_attributes_field_type_e &/*type*/,
                        const size_t & /*size_bytes*/,
                        uint8_t * /*value*/) const {
    return TDI_NOT_SUPPORTED;
  };
  const tdi_attributes_type_e &attributeTypeGet() const { return attr_type_; };
  const Table* tableGet() const { return table_; };

 private:
  const Table *table_;
  tdi_attributes_type_e attr_type_;
};

}  // namespace tdi

#endif  // _TDI_TABLE_ATTRIBUTES_HPP
