// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include <algorithm>
#include <tdi/common/tdi_table.hpp>

// local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {
const std::string tdiNullStr = "";

tdi_status_t Table::entryAdd(const Session & /*session*/,
                             const Target & /*dev_tgt*/,
                             const Flags & /*flags*/,
                             const TableKey & /*key*/,
                             const TableData & /*data*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table Entry add not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::entryMod(const Session & /*session*/,
                             const Target & /*dev_tgt*/,
                             const Flags & /*flags*/,
                             const TableKey & /*key*/,
                             const TableData & /*data*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table entry mod not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::entryDel(const Session & /*session*/,
                             const Target & /*dev_tgt*/,
                             const Flags & /*flags*/,
                             const TableKey & /*key*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table entry Delete not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::clear(const Session & /*session*/,
                          const Target & /*dev_tgt*/,
                          const Flags & /*flags*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table Clear not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::defaultEntrySet(const Session & /*session*/,
                                    const Target & /*dev_tgt*/,
                                    const Flags & /*flags*/,
                                    const TableData & /*data*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table default entry set not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::defaultEntryMod(const Session & /*session*/,
                                    const Target & /*dev_tgt*/,
                                    const Flags & /*flags*/,
                                    const TableData & /*data*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table default entry mod not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::defaultEntryReset(const Session & /* session */,
                                      const Target & /* dev_tgt */,
                                      const Flags & /*flags*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table default entry reset not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::defaultEntryGet(const Session & /* session */,
                                    const Target & /* dev_tgt */,
                                    const Flags & /*flags*/,
                                    TableData * /* data */) const {
  LOG_ERROR("%s:%d %s ERROR : Table default entry get not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::entryGet(const Session & /*session */,
                             const Target & /* dev_tgt */,
                             const Flags & /*flags*/,
                             const TableKey & /* &key */,
                             TableData * /* data */) const {
  LOG_ERROR("%s:%d %s ERROR Table entry get not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::entryGetFirst(const Session & /*session*/,
                                  const Target & /*dev_tgt*/,
                                  const Flags & /*flags*/,
                                  TableKey * /*key*/,
                                  TableData * /*data*/) const {
  LOG_ERROR("%s:%d %s ERROR Table entry get first not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::entryGet(const Session & /*session*/,
                             const Target & /*dev_tgt*/,
                             const Flags & /*flags*/,
                             const tdi_handle_t & /*entry_handle*/,
                             TableKey * /*key*/,
                             TableData * /*data*/) const {
  LOG_ERROR("%s:%d %s ERROR Table entry get by handle not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::entryKeyGet(const Session & /*session*/,
                                const Target & /*dev_tgt*/,
                                const Flags & /*flags*/,
                                const tdi_handle_t & /*entry_handle*/,
                                Target * /*entry_tgt*/,
                                TableKey * /*key*/) const {
  LOG_ERROR("%s:%d %s ERROR Table entry get key not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::entryHandleGet(const Session & /*session*/,
                                   const Target & /*dev_tgt*/,
                                   const Flags & /*flags*/,
                                   const TableKey & /*key*/,
                                   tdi_handle_t * /*entry_handle*/) const {
  LOG_ERROR("%s:%d %s ERROR Table entry get handle not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::entryGetNextN(const Session & /*session*/,
                                  const Target & /*dev_tgt*/,
                                  const Flags & /*flags*/,
                                  const TableKey & /*key*/,
                                  const uint32_t & /*n*/,
                                  keyDataPairs * /*key_data_pairs*/,
                                  uint32_t * /*num_returned*/) const {
  LOG_ERROR("%s:%d %s ERROR Table entry get next_n not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::usageGet(const Session & /*session*/,
                             const Target & /*dev_tgt*/,
                             const Flags & /*flags*/,
                             uint32_t * /*count*/) const {
  LOG_ERROR("%s:%d %s Not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::sizeGet(const Session & /*session*/,
                            const Target & /*dev_tgt*/,
                            const Flags & /*flags*/,
                            size_t *size) const {
  if (nullptr == size) {
    LOG_ERROR("%s:%d Outparam passed is nullptr", __func__, __LINE__);
    return TDI_INVALID_ARG;
  } else {
    *size = tableInfoGet()->sizeGet();
    return TDI_SUCCESS;
  }
}

tdi_status_t Table::keyAllocate(std::unique_ptr<TableKey> * /*key_ret*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table Key allocate not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::keyReset(TableKey * /* key */) const {
  LOG_ERROR("%s:%d %s ERROR : Table Key reset not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataAllocate(
    std::unique_ptr<TableData> * /*data_ret*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table data allocate not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataAllocate(
    const tdi_id_t & /*action_id*/,
    std::unique_ptr<TableData> * /*data_ret*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table data allocate not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataAllocate(
    const std::vector<tdi_id_t> & /*fields*/,
    const tdi_id_t & /* action_id */,
    std::unique_ptr<TableData> * /*data_ret*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table data allocate not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataAllocate(
    const std::vector<tdi_id_t> & /* fields */,
    std::unique_ptr<TableData> * /*data_ret*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table data allocate not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataReset(TableData * /*data */) const {
  LOG_ERROR("%s:%d %s ERROR : Table data reset not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataReset(const tdi_id_t & /* action_id */,
                              TableData * /* data */) const {
  LOG_ERROR("%s:%d %s ERROR : Table data reset not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataReset(const std::vector<tdi_id_t> & /* fields */,
                              TableData * /* data */) const {
  LOG_ERROR("%s:%d %s ERROR : Table data reset not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataReset(const std::vector<tdi_id_t> & /* fields */,
                              const tdi_id_t & /* action_id */,
                              TableData * /* data */) const {
  LOG_ERROR("%s:%d %s ERROR : Table data reset not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::attributeAllocate(
    const tdi_attributes_type_e &attr_type,
    std::unique_ptr<TableAttributes> *table_attr) const {
  auto op_found = tableInfoGet()->attributesSupported().find(attr_type);
  if (op_found == tableInfoGet()->attributesSupported().end()) {
    *table_attr = nullptr;
    LOG_ERROR("%s:%d %s Operation not supported for this table",
              __func__,
              __LINE__,
              tableInfoGet()->nameGet().c_str());
    return TDI_NOT_SUPPORTED;
  }

  *table_attr =
      std::unique_ptr<TableAttributes>(new TableAttributes(this, attr_type));
  return TDI_SUCCESS;
}

tdi_status_t Table::attributeReset(
    const tdi_attributes_type_e & /*type*/,
    std::unique_ptr<TableAttributes> * /*attr*/) const {
  LOG_ERROR("%s:%d %s ERROR : Table attribute allocate not supported",
            __func__,
            __LINE__,
            tableInfoGet()->nameGet().c_str());
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::tableAttributesSet(
    const Session & /*session*/,
    const Target & /*dev_tgt*/,
    const Flags & /*flags*/,
    const TableAttributes & /*tableAttributes*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::tableAttributesGet(
    const Session & /*session*/,
    const Target & /*dev_tgt*/,
    const Flags & /*flags*/,
    TableAttributes * /*tableAttributes*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::operationsAllocate(
    const tdi_operations_type_e &op_type,
    std::unique_ptr<TableOperations> *table_ops) const {
  auto op_found = tableInfoGet()->operationsSupported().find(op_type);
  if (op_found == tableInfoGet()->operationsSupported().end()) {
    *table_ops = nullptr;
    LOG_ERROR("%s:%d %s Operation not supported for this table",
              __func__,
              __LINE__,
              tableInfoGet()->nameGet().c_str());
    return TDI_NOT_SUPPORTED;
  }

  *table_ops =
      std::unique_ptr<TableOperations>(new TableOperations(this, op_type));
  return TDI_SUCCESS;
}

tdi_status_t Table::tableOperationsExecute(
    const tdi::TableOperations & /*tableOperations*/) const {
  return TDI_NOT_SUPPORTED;
}

}  // namespace tdi
