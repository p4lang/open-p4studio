// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include <stdio.h>
#include <tdi/common/c_frontend/tdi_table.h>

#include <tdi/common/c_frontend/tdi_attributes.h>
#include <tdi/common/c_frontend/tdi_operations.h>
#include <tdi/common/c_frontend/tdi_table.h>
#include <tdi/common/tdi_attributes.hpp>
#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_init.hpp>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>
#include <tdi/common/tdi_operations.hpp>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_table.hpp>
#include <tdi/common/tdi_table_data.hpp>
#include <tdi/common/tdi_table_key.hpp>
#include <tdi/common/tdi_target.hpp>
#include <tdi/common/tdi_utils.hpp>

tdi_status_t tdi_table_entry_add(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 const tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  // auto &devMgr=tdi::DevMgr::getInstance();
  // tdi_status_t status=tdi:devMgr->deviceGet(dev_tgt->dev_id, device);
  return table->entryAdd(*reinterpret_cast<const tdi::Session *>(session),
                         *reinterpret_cast<const tdi::Target *>(target),
                         *reinterpret_cast<const tdi::Flags *>(flags),
                         *reinterpret_cast<const tdi::TableKey *>(key),
                         *reinterpret_cast<const tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_mod(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 const tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryMod(*reinterpret_cast<const tdi::Session *>(session),
                         *reinterpret_cast<const tdi::Target *>(target),
                         *reinterpret_cast<const tdi::Flags *>(flags),
                         *reinterpret_cast<const tdi::TableKey *>(key),
                         *reinterpret_cast<const tdi::TableData *>(data));
}

tdi_status_t tdi_table_default_entry_mod(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *target,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->defaultEntryMod(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_del(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryDel(*reinterpret_cast<const tdi::Session *>(session),
                         *reinterpret_cast<const tdi::Target *>(target),
                         *reinterpret_cast<const tdi::Flags *>(flags),
                         *reinterpret_cast<const tdi::TableKey *>(key));
}

tdi_status_t tdi_table_clear(const tdi_table_hdl *table_hdl,
                             const tdi_session_hdl *session,
                             const tdi_target_hdl *target,
                             const tdi_flags_hdl *flags) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->clear(
      *reinterpret_cast<const tdi::Session *>(session), /**dev_tgt, flags*/
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags));
}

tdi_status_t tdi_table_entry_get(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryGet(*reinterpret_cast<const tdi::Session *>(session),
                         *reinterpret_cast<const tdi::Target *>(target),
                         *reinterpret_cast<const tdi::Flags *>(flags),
                         *reinterpret_cast<const tdi::TableKey *>(key),
                         reinterpret_cast<tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_get_by_handle(const tdi_table_hdl *table_hdl,
                                           const tdi_session_hdl *session,
                                           const tdi_target_hdl *target,
                                           const tdi_flags_hdl *flags,
                                           const uint32_t entry_handle,
                                           tdi_table_key_hdl *key,
                                           tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryGet(*reinterpret_cast<const tdi::Session *>(session),
                         *reinterpret_cast<const tdi::Target *>(target),
                         *reinterpret_cast<const tdi::Flags *>(flags),
                         static_cast<tdi_handle_t>(entry_handle),
                         reinterpret_cast<tdi::TableKey *>(key),
                         reinterpret_cast<tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_key_get(const tdi_table_hdl *table_hdl,
                                     const tdi_session_hdl *session,
                                     const tdi_target_hdl *target_in,
                                     const tdi_flags_hdl *flags,
                                     const uint32_t entry_handle,
                                     tdi_target_hdl *target_out,
                                     tdi_table_key_hdl *key) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryKeyGet(*reinterpret_cast<const tdi::Session *>(session),
                            *reinterpret_cast<const tdi::Target *>(target_in),
                            *reinterpret_cast<const tdi::Flags *>(flags),
                            static_cast<tdi_handle_t>(entry_handle),
                            reinterpret_cast<tdi::Target *>(target_out),
                            reinterpret_cast<tdi::TableKey *>(key));
}

tdi_status_t tdi_table_entry_handle_get(const tdi_table_hdl *table_hdl,
                                        const tdi_session_hdl *session,
                                        const tdi_target_hdl *target,
                                        const tdi_flags_hdl *flags,
                                        const tdi_table_key_hdl *key,
                                        uint32_t *entry_handle) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryHandleGet(*reinterpret_cast<const tdi::Session *>(session),
                               *reinterpret_cast<const tdi::Target *>(target),
                               *reinterpret_cast<const tdi::Flags *>(flags),
                               *reinterpret_cast<const tdi::TableKey *>(key),
                               entry_handle);
}

tdi_status_t tdi_table_entry_get_first(const tdi_table_hdl *table_hdl,
                                       const tdi_session_hdl *session,
                                       const tdi_target_hdl *target,
                                       const tdi_flags_hdl *flags,
                                       tdi_table_key_hdl *key,
                                       tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryGetFirst(*reinterpret_cast<const tdi::Session *>(session),
                              *reinterpret_cast<const tdi::Target *>(target),
                              *reinterpret_cast<const tdi::Flags *>(flags),
                              reinterpret_cast<tdi::TableKey *>(key),
                              reinterpret_cast<tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_get_next_n(const tdi_table_hdl *table_hdl,
                                        const tdi_session_hdl *session,
                                        const tdi_target_hdl *target,
                                        const tdi_flags_hdl *flags,
                                        const tdi_table_key_hdl *key,
                                        tdi_table_key_hdl **output_keys,
                                        tdi_table_data_hdl **output_data,
                                        uint32_t n,
                                        uint32_t *num_returned) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  tdi::Table::keyDataPairs key_data_pairs;
  unsigned i = 0;
  for (i = 0; i < n; i++) {
    key_data_pairs.push_back(
        std::make_pair(reinterpret_cast<tdi::TableKey *>(output_keys[i]),
                       reinterpret_cast<tdi::TableData *>(output_data[i])));
  }

  return table->entryGetNextN(*reinterpret_cast<const tdi::Session *>(session),
                              *reinterpret_cast<const tdi::Target *>(target),
                              *reinterpret_cast<const tdi::Flags *>(flags),
                              *reinterpret_cast<const tdi::TableKey *>(key),
                              n,
                              &key_data_pairs,
                              num_returned);
}

tdi_status_t tdi_table_usage_get(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 uint32_t *count) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->usageGet(*reinterpret_cast<const tdi::Session *>(session),
                         *reinterpret_cast<const tdi::Target *>(target),
                         *reinterpret_cast<const tdi::Flags *>(flags),
                         count);
}

tdi_status_t tdi_table_default_entry_set(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *target,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->defaultEntrySet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableData *>(data));
}

tdi_status_t tdi_table_default_entry_get(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *target,
                                         const tdi_flags_hdl *flags,
                                         tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->defaultEntryGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      reinterpret_cast<tdi::TableData *>(data));
}

tdi_status_t tdi_table_default_entry_reset(const tdi_table_hdl *table_hdl,
                                           const tdi_session_hdl *session,
                                           const tdi_target_hdl *target,
                                           const tdi_flags_hdl *flags) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->defaultEntryReset(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags));
}

tdi_status_t tdi_table_size_get(const tdi_table_hdl *table_hdl,
                                const tdi_session_hdl *session,
                                const tdi_target_hdl *target,
                                const tdi_flags_hdl *flags,
                                size_t *count) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->sizeGet(*reinterpret_cast<const tdi::Session *>(session),
                        *reinterpret_cast<const tdi::Target *>(target),
                        *reinterpret_cast<const tdi::Flags *>(flags),
                        count);
}
tdi_status_t tdi_action_id_from_data_get(const tdi_table_data_hdl *data,
                                         tdi_id_t *id_ret) {
  auto data_obj = reinterpret_cast<const tdi::TableData *>(data);
  // disable it now, will enable it once the method is available in the code
  // class
  *id_ret = data_obj->actionIdGet();
  return TDI_SUCCESS;
}

// Allocate APIs
tdi_status_t tdi_table_key_allocate(const tdi_table_hdl *table_hdl,
                                    tdi_table_key_hdl **key_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableKey> key_hdl;
  auto status = table->keyAllocate(&key_hdl);
  *key_hdl_ret = reinterpret_cast<tdi_table_key_hdl *>(key_hdl.release());
  return status;
}

tdi_status_t tdi_table_data_allocate(const tdi_table_hdl *table_hdl,
                                     tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  auto status = table->dataAllocate(&data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}

tdi_status_t tdi_table_action_data_allocate(const tdi_table_hdl *table_hdl,
                                            const tdi_id_t action_id,
                                            tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  auto status = table->dataAllocate(action_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());
  return status;
}

tdi_status_t tdi_table_data_allocate_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  auto status = table->dataAllocate(vec, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}

tdi_status_t tdi_table_action_data_allocate_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  auto status = table->dataAllocate(vec, action_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_table_data_allocate_container(
    tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  auto status = table->dataAllocateContainer(c_field_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());
  return status;
}

tdi_status_t tdi_table_action_data_allocate_container(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t action_id,
    tdi_table_data_hdl **data_hdl_ret) {
  auto tableData = reinterpret_cast<const tdi::TableData *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  auto status = table->dataAllocateContainer(c_field_id, action_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());
  return status;
}

tdi_status_t tdi_table_data_allocate_container_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto tableData = reinterpret_cast<const tdi::TableData *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  auto status = table->dataAllocateContainer(c_field_id, vec, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}

tdi_status_t tdi_table_action_data_allocate_container_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto tableData = reinterpret_cast<const tdi::TableData *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  auto status =
      tableData->dataAllocateContainer(c_field_id, vec, action_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}
#endif

tdi_status_t tdi_attributes_allocate(const tdi_table_hdl *table_hdl,
                                     const tdi_attributes_type_e type,
                                     tdi_attributes_hdl **tbl_attr_hdl) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(type, &attr);
  *tbl_attr_hdl = reinterpret_cast<tdi_attributes_hdl *>(attr.release());
  return status;
}

tdi_status_t tdi_operations_allocate(const tdi_table_hdl *table_hdl,
                                     const tdi_operations_type_e op_type,
                                     tdi_operations_hdl **tbl_ops) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableOperations> ops;
  auto status = table->operationsAllocate(op_type, &ops);
  *tbl_ops = reinterpret_cast<tdi_operations_hdl *>(ops.release());
  return status;
}

// Reset APIs
tdi_status_t tdi_table_key_reset(const tdi_table_hdl *table_hdl,
                                 tdi_table_key_hdl **key_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->keyReset(reinterpret_cast<tdi::TableKey *>(*key_hdl_ret));
}

tdi_status_t tdi_table_data_reset(const tdi_table_hdl *table_hdl,
                                  tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->dataReset(reinterpret_cast<tdi::TableData *>(*data_hdl_ret));
}

tdi_status_t tdi_table_action_data_reset(const tdi_table_hdl *table_hdl,
                                         const tdi_id_t action_id,
                                         tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->dataReset(action_id,
                          reinterpret_cast<tdi::TableData *>(*data_hdl_ret));
}

tdi_status_t tdi_table_data_reset_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  return table->dataReset(vec,
                          reinterpret_cast<tdi::TableData *>(*data_hdl_ret));
}

tdi_status_t tdi_table_action_data_reset_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  return table->dataReset(
      vec, action_id, reinterpret_cast<tdi::TableData *>(*data_hdl_ret));
}

// De-allocate APIs
tdi_status_t tdi_table_key_deallocate(tdi_table_key_hdl *key_hdl) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  if (key == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete key;
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_data_deallocate(tdi_table_data_hdl *data_hdl) {
  auto data = reinterpret_cast<tdi::TableData *>(data_hdl);
  if (data == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete data;
  return TDI_SUCCESS;
}

tdi_status_t tdi_attributes_deallocate(tdi_attributes_hdl *tbl_attr_hdl) {
  auto tbl_attr = reinterpret_cast<tdi::TableAttributes *>(tbl_attr_hdl);
  if (tbl_attr == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete tbl_attr;
  return TDI_SUCCESS;
}

tdi_status_t tdi_operations_deallocate(tdi_operations_hdl *tbl_op_hdl) {
  auto tbl_op = reinterpret_cast<tdi::TableOperations *>(tbl_op_hdl);
  if (tbl_op == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete tbl_op;
  return TDI_SUCCESS;
}

// KeyField APIs

// DataField APIs
tdi_status_t tdi_action_id_applicable(const tdi_table_hdl *table_hdl,
                                      bool *ret_val) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  *ret_val = table->actionIdApplicable();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_attributes_set(const tdi_table_hdl *table_hdl,
                                      const tdi_session_hdl *session,
                                      const tdi_target_hdl *target,
                                      const tdi_flags_hdl *flags,
                                      const tdi_attributes_hdl *tbl_attr) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->tableAttributesSet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableAttributes *>(tbl_attr));
}

tdi_status_t tdi_table_attributes_get(const tdi_table_hdl *table_hdl,
                                      const tdi_session_hdl *session,
                                      const tdi_target_hdl *target,
                                      const tdi_flags_hdl *flags,
                                      tdi_attributes_hdl *tbl_attr) {

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->tableAttributesGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      reinterpret_cast<tdi::TableAttributes *>(tbl_attr));
}

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_table_operations_execute(const tdi_table_hdl *table_hdl,
                                          const tdi_operations_hdl *tbl_ops) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return (table->tableOperationsExecute(
      reinterpret_cast<const tdi::TableOperations *>(tbl_ops)));
}
#endif
