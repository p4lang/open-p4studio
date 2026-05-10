// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_table_operations.hpp
 *
 *  @brief Contains TDI Table Operations APIs
 */
#ifndef _TDI_TABLE_OPERATIONS_HPP
#define _TDI_TABLE_OPERATIONS_HPP

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>
#include <functional>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_session.hpp>

namespace tdi {

class Table;

class TableOperations {
public:
    virtual ~TableOperations() = default;
    TableOperations(const Table* table, const tdi_operations_type_e &oper_type)
        : table_(table), oper_type_(oper_type){};
    tdi_status_t setValue(tdi_operations_field_type_e  /*type*/, const uint64_t & /*value*/) { return TDI_NOT_SUPPORTED; };
    tdi_status_t getValue(tdi_operations_field_type_e  /*type*/, uint64_t * /*value*/) {return TDI_NOT_SUPPORTED;};
private:
    const Table* table_;
    tdi_operations_type_e oper_type_;
};

}  // tdi

#endif  // _TDI_TABLE_OPERATIONS_HPP
