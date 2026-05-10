// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0

#include "tdi_dummy_info.hpp"

namespace tdi {

namespace {
const std::map<std::string, tdi_dummy_table_type_e> dummy_table_type_map = {
    {"MatchAction_Direct", TDI_DUMMY_TABLE_TYPE_MATCH_DIRECT},
    {"MatchAction_Indirect", TDI_DUMMY_TABLE_TYPE_MATCH_INDIRECT},
    {"MatchAction_Indirect_Selector",
     TDI_DUMMY_TABLE_TYPE_MATCH_INDIRECT_SELECTOR},
    {"Action", TDI_DUMMY_TABLE_TYPE_ACTION_PROFILE},
    {"Selector", TDI_DUMMY_TABLE_TYPE_SELECTOR},
    {"Meter", TDI_DUMMY_TABLE_TYPE_METER},
    {"Counter", TDI_DUMMY_TABLE_TYPE_COUNTER},
    {"Register", TDI_DUMMY_TABLE_TYPE_REGISTER},
    {"PortConfigure", TDI_DUMMY_TABLE_TYPE_PORT_CFG},
    {"PortStat", TDI_DUMMY_TABLE_TYPE_PORT_STAT},
}
}  // namespace

namespace tna {
namespace dummy {

class TdiInfoMapper : public tdi::tna::TdiInfoMapper {
 public:
  TdiInfoMapper() {
    // table types
    for (const auto &kv : dummy_table_type_map) {
      tableEnumMapAdd(kv.first, static_cast<tdi_table_type_e>(kv.second));
    }
  }
};

/**
 * @brief Class to help create the correct Table object with the
 * help of a map. Targets should override
 */
class TableFactory : public tdi::tna::TableFactory {
 public:
  virtual std::unique_ptr<tdi::Table> makeTable(
      const tdi::TableInfo *table_info) const override {
    if (!table_info) {
      LOG_ERROR("%s:%d No table info received", __func__, __LINE__);
      return nullptr;
    }
    auto table_type =
        static_cast<tdi_dummy_table_type_e>(table_info->tableTypeGet());
    switch (table_type) {
      case TDI_DUMMY_TABLE_TYPE_MATCH_DIRECT:
        return std::unique_ptr<tdi::Table>(new MatchActionDirect(table_info));
      case TDI_DUMMY_TABLE_TYPE_MATCH_INDIRECT:
      case TDI_DUMMY_TABLE_TYPE_MATCH_INDIRECT_SELECTOR:
        return std::unique_ptr<tdi::Table>(new MatchActionIndirect(table_info));
      case TDI_DUMMY_TABLE_TYPE_ACTION_PROFILE:
        return std::unique_ptr<tdi::Table>(new ActionProfile(table_info));
      case TDI_DUMMY_TABLE_TYPE_SELECTOR:
        return std::unique_ptr<tdi::Table>(new Selector(table_info));
      case TDI_DUMMY_TABLE_TYPE_COUNTER:
        return std::unique_ptr<tdi::Table>(new CounterIndirect(table_info));
      case TDI_DUMMY_TABLE_TYPE_METER:
        return std::unique_ptr<tdi::Table>(new MeterIndirect(table_info));
      default:
        return nullptr;
    }
    return nullptr;
  };
};

}  // namespace dummy
}  // namespace tna
}  // namespace tdi
