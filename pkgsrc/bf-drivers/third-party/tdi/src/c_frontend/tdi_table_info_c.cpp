// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include <stdio.h>
#include <tdi/common/c_frontend/tdi_table_info.h>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_init.hpp>
//#include <tdi/common/tdi_table.hpp>
#include <tdi/common/c_frontend/tdi_attributes.h>
#include <tdi/common/c_frontend/tdi_operations.h>
#include <tdi/common/tdi_attributes.hpp>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>
//#include <tdi/common/tdi_table_data.hpp>
#//include <tdi/common/tdi_table_key.hpp>
#include <tdi/common/tdi_operations.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace {

template <typename T>
tdi_status_t key_field_allowed_choices_get_helper(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t &field_id,
    // std::vector<std::reference_wrapper<const std::string>> *cpp_choices,
    const std::vector<std::string> **cpp_choices,
    T out_param) {
  if (out_param == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  const tdi::KeyFieldInfo *keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *cpp_choices = &keyFieldInfo->choicesGet();
  return TDI_SUCCESS;
}

template <typename T>
tdi_status_t data_field_allowed_choices_get_helper(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t &field_id,
    const std::vector<std::string> **cpp_choices,
    T out_param,
    const tdi_id_t &action_id = 0) {
  if (out_param == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  const tdi::DataFieldInfo *dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *cpp_choices = &dataFieldInfo->allowedChoicesGet();
  return TDI_SUCCESS;
}

tdi_annotation_t convert_annotation(const tdi::Annotation &annotation) {
  tdi_annotation_t annotations_c;
  annotations_c.name = annotation.name_.c_str();
  annotations_c.value = annotation.value_.c_str();
  return annotations_c;
}
}  // anonymous namespace

tdi_status_t tdi_table_info_get(const tdi_table_hdl *table_hdl,
                                const tdi_table_info_hdl **table_info_hdl) {
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  *table_info_hdl =
      reinterpret_cast<const tdi_table_info_hdl *>(table->tableInfoGet());
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_id_from_handle_get(
    const tdi_table_info_hdl *table_info_hdl, tdi_id_t *id) {
  if (id == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *id = tableInfo->idGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_name_get(const tdi_table_info_hdl *table_info_hdl,
                                const char **table_name_ret) {
  if (table_name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  // We need the internal function which returns a const string ref
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *table_name_ret = tableInfo->nameGet().c_str();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_type_get(const tdi_table_info_hdl *table_info_hdl,
                                tdi_table_type_e *table_type) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *table_type = static_cast<tdi_table_type_e>(tableInfo->tableTypeGet());
  // reinterpret_cast<tdi::TableInfo::TableType *>(table_type));
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_has_const_default_action(
    const tdi_table_info_hdl *table_info_hdl, bool *has_const_default_action) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *has_const_default_action = tableInfo->hasConstDefaultAction();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_num_annotations_get(
    const tdi_table_info_hdl *table_info_hdl, uint32_t *num_annotations) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  const auto &cpp_annotations = tableInfo->annotationsGet();
  *num_annotations = cpp_annotations.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_annotations_get(const tdi_table_info_hdl *table_info_hdl,
                                       tdi_annotation_t *annotations_c) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  const auto &cpp_annotations = tableInfo->annotationsGet();
  int i = 0;
  for (const auto &annotation : cpp_annotations) {
    annotations_c[i++] = convert_annotation(annotation);
  }
  return TDI_SUCCESS;
}

// KeyField APIs
tdi_status_t tdi_key_field_num_allowed_choices_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    uint32_t *num_choices) {
  // std::vector<std::reference_wrapper<const std::string>> cpp_choices;
  const std::vector<std::string> *cpp_choices;
  tdi_status_t sts = ::key_field_allowed_choices_get_helper<uint32_t *>(
      table_info_hdl, field_id, &cpp_choices, num_choices);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num_choices = cpp_choices->size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_allowed_choices_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const char *choices[]) {
  // std::vector<std::reference_wrapper<const std::string>> cpp_choices;
  const std::vector<std::string> *cpp_choices;
  tdi_status_t sts = ::key_field_allowed_choices_get_helper<const char *[]>(
      table_info_hdl, field_id, &cpp_choices, choices);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  for (auto iter = cpp_choices->begin(); iter != cpp_choices->end(); iter++) {
    // choices[iter - cpp_choices.begin()] = iter->get().c_str();
    choices[iter - cpp_choices->begin()] = iter->c_str();
  }
  return TDI_SUCCESS;
}
tdi_status_t tdi_key_field_id_list_size_get(
    const tdi_table_info_hdl *table_info_hdl, uint32_t *num) {
  // Only here are we using TableObj rather than Table
  // since we need the hidden impl function
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *num = tableInfo->keyFieldIdListGet().size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_id_list_get(const tdi_table_info_hdl *table_info_hdl,
                                       tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  std::vector<tdi_id_t> temp_vec;
  temp_vec = tableInfo->keyFieldIdListGet();

  for (auto it = temp_vec.begin(); it != temp_vec.end(); ++it) {
    tdi_id_t field_id = *it;
    id_vec_ret[it - temp_vec.begin()] = field_id;
  }

  return TDI_SUCCESS;
}

/* match type get */
tdi_status_t tdi_key_field_match_type_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    tdi_match_type_e *field_type_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);

  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  auto keyFieldType =
      static_cast<tdi_match_type_e>(keyFieldInfo->matchTypeGet());
  *field_type_ret = static_cast<tdi_match_type_e>(keyFieldType);
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_data_type_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    tdi_field_data_type_e *field_type_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *field_type_ret =
      static_cast<tdi_field_data_type_e>(keyFieldInfo->dataTypeGet());
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_id_get(const tdi_table_info_hdl *table_info_hdl,
                                  const char *key_field_name,
                                  tdi_id_t *field_id) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto keyFieldInfo = tableInfo->keyFieldGet(key_field_name);
  *field_id = keyFieldInfo->idGet();
  if (*field_id == 0) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  } else {
    return TDI_SUCCESS;
  }
}

tdi_status_t tdi_key_field_size_get(const tdi_table_info_hdl *table_info_hdl,
                                    const tdi_id_t field_id,
                                    size_t *field_size_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *field_size_ret = keyFieldInfo->sizeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_is_ptr_get(const tdi_table_info_hdl *table_info_hdl,
                                      const tdi_id_t field_id,
                                      bool *is_ptr_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *is_ptr_ret = keyFieldInfo->isPtrGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_name_get(const tdi_table_info_hdl *table_info_hdl,
                                    const tdi_id_t field_id,
                                    const char **name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *name_ret = keyFieldInfo->nameGet().c_str();
  return TDI_SUCCESS;
}

// DataField APIs
tdi_status_t tdi_data_field_id_list_size_get(
    const tdi_table_info_hdl *table_info_hdl, uint32_t *num) {
  if (num == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  // Only here are we using TableObj rather than Table
  // since we need the hidden impl function
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *num = tableInfo->dataFieldIdListGet().size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_id_list_size_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t action_id,
    uint32_t *num) {
  // Only here are we using TableObj rather than Table
  // since we need the hidden impl function
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *num = tableInfo->dataFieldIdListGet(action_id).size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_list_get(const tdi_table_info_hdl *table_info_hdl,
                                     tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  std::vector<tdi_id_t> field_ids;
  field_ids = tableInfo->dataFieldIdListGet();
  for (auto it = field_ids.begin(); it != field_ids.end(); ++it) {
    tdi_id_t field_id = *it;
    id_vec_ret[it - field_ids.begin()] = field_id;
  }
  return TDI_SUCCESS;
}

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_container_data_field_list_get(
    const tdi_table_info_hdl *table_info_hdl,
    tdi_id_t container_field_id,
    tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  // std::vector<tdi_id_t> field_ids;
  auto field_ids = tableInfo->containerDataFieldIdListGet();
  if (sts == TDI_SUCCESS) {
    for (auto it = field_ids.begin(); it != field_ids.end(); ++it) {
      tdi_id_t field_id = *it;
      id_vec_ret[it - field_ids.begin()] = field_id;
    }
  }

  return TDI_SUCCESS;
}

tdi_status_t tdi_container_data_field_list_size_get(
    const tdi_table_info_hdl *table_info_hdl,
    tdi_id_t container_field_id,
    size_t *field_list_size_ret) {
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  *field_list_size_ret = 0;
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  std::vector<tdi_id_t> field_ids;
  tdi_status_t sts =
      table->containerDataFieldIdListGet(container_field_id, &field_ids);
  if (sts == TDI_SUCCESS) {
    *field_list_size_ret = field_ids.size();
  }

  return sts;
}
#endif

tdi_status_t tdi_data_field_list_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t action_id,
    tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  // std::vector<tdi_id_t> field_ids;
  auto field_ids = tableInfo->dataFieldIdListGet(action_id);
  for (auto it = field_ids.begin(); it != field_ids.end(); ++it) {
    tdi_id_t field_id = *it;
    id_vec_ret[it - field_ids.begin()] = field_id;
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_id_get(const tdi_table_info_hdl *table_info_hdl,
                                   const char *data_field_name,
                                   tdi_id_t *field_id_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *field_id_ret = tableInfo->dataFieldIdGet(data_field_name);
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_id_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const char *data_field_name,
    const tdi_id_t action_id,
    tdi_id_t *field_id_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  *field_id_ret = tableInfo->dataFieldIdGet(data_field_name, action_id);
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_size_get(const tdi_table_info_hdl *table_info_hdl,
                                     const tdi_id_t field_id,
                                     size_t *field_size_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *field_size_ret = dataFieldInfo->sizeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_size_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    size_t *field_size_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *field_size_ret = dataFieldInfo->sizeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_ptr_get(const tdi_table_info_hdl *table_info_hdl,
                                       const tdi_id_t field_id,
                                       bool *is_ptr_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *is_ptr_ret = dataFieldInfo->isPtrGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_ptr_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_ptr_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *is_ptr_ret = dataFieldInfo->isPtrGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_mandatory_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    bool *is_mandatory_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *is_mandatory_ret = dataFieldInfo->mandatoryGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_mandatory_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_mandatory_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *is_mandatory_ret = dataFieldInfo->mandatoryGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_read_only_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    bool *is_read_only_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *is_read_only_ret = dataFieldInfo->readOnlyGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_read_only_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_read_only_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *is_read_only_ret = dataFieldInfo->readOnlyGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_name_get(const tdi_table_info_hdl *table_info_hdl,
                                     const tdi_id_t field_id,
                                     const char **name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *name_ret = dataFieldInfo->nameGet().c_str();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_name_copy_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const uint32_t buf_sz,
    char *name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  std::string fname;
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  fname = dataFieldInfo->nameGet();

  if (fname.size() >= buf_sz) {
    LOG_ERROR("%s:%d Provided buffer too small", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  std::strncpy(name_ret, fname.c_str(), buf_sz);

  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_name_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    const char **name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *name_ret = dataFieldInfo->nameGet().c_str();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_type_get(const tdi_table_info_hdl *table_info_hdl,
                                     const tdi_id_t field_id,
                                     tdi_field_data_type_e *field_type_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  //*field_type_ret = dataFieldInfo->dataTypeGet(
  //    field_id, reinterpret_cast<tdi::DataType *>(field_type_ret));
  *field_type_ret = dataFieldInfo->dataTypeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_type_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_field_data_type_e *field_type_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *field_type_ret = dataFieldInfo->dataTypeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_annotations_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    uint32_t *num_annotations) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  const auto &annotations = dataFieldInfo->annotationsGet();
  *num_annotations = annotations.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_annotations_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    tdi_annotation_t *annotations_c) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  const auto &annotations = dataFieldInfo->annotationsGet();
  int i = 0;
  for (const auto &annotation : annotations) {
    annotations_c[i++] = convert_annotation(annotation);
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_annotations_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num_annotations) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  const auto &annotations = dataFieldInfo->annotationsGet();
  *num_annotations = annotations.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_annotations_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_annotation_t *annotations_c) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  const auto &annotations = dataFieldInfo->annotationsGet();
  int i = 0;
  for (const auto &annotation : annotations) {
    annotations_c[i++] = convert_annotation(annotation);
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_oneof_siblings_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    uint32_t *num_oneof_siblings) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  auto oneof_siblings = dataFieldInfo->oneofSiblingsGet();
  *num_oneof_siblings = oneof_siblings.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_oneof_siblings_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    tdi_id_t *oneof_siblings_c) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  auto oneof_siblings = dataFieldInfo->oneofSiblingsGet();
  int i = 0;
  for (const auto &oneof_sibling : oneof_siblings) {
    oneof_siblings_c[i++] = oneof_sibling;
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_oneof_siblings_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num_oneof_siblings) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  auto oneof_siblings = dataFieldInfo->oneofSiblingsGet();
  *num_oneof_siblings = oneof_siblings.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_oneof_siblings_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_id_t *oneof_siblings_c) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  auto oneof_siblings = dataFieldInfo->oneofSiblingsGet();
  int i = 0;
  for (const auto &oneof_sibling : oneof_siblings) {
    oneof_siblings_c[i++] = oneof_sibling;
  }
  return TDI_SUCCESS;
}

// disable it now, will enable it once the method is available in the code class
tdi_status_t tdi_action_id_list_size_get(
    const tdi_table_info_hdl *table_info_hdl, uint32_t *num) {
  if (num == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto action_ids = tableInfo->actionIdListGet();
  *num = action_ids.size();

  return TDI_SUCCESS;
}

tdi_status_t tdi_action_id_list_get(const tdi_table_info_hdl *table_info_hdl,
                                    tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto action_ids = tableInfo->actionIdListGet();
  for (auto it = action_ids.begin(); it != action_ids.end(); ++it) {
    tdi_id_t action_id = *it;
    id_vec_ret[it - action_ids.begin()] = action_id;
  }
  return TDI_SUCCESS;
}

// disable it now, will enable it once the method is available in the code class
tdi_status_t tdi_action_name_get(const tdi_table_info_hdl *table_info_hdl,
                                 const tdi_id_t action_id,
                                 const char **name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_info_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto actionInfo = tableInfo->actionGet(action_id);
  *name_ret = actionInfo->nameGet().c_str();

  return TDI_SUCCESS;
}

tdi_status_t tdi_action_name_to_id(const tdi_table_info_hdl *table_info_hdl,
                                   const char *action_name,
                                   tdi_id_t *action_id_ret) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto actionInfo = tableInfo->actionGet(std::string(action_name));
  *action_id_ret = actionInfo->idGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_action_num_annotations_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t action_id,
    uint32_t *num_annotations) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto actionInfo = tableInfo->actionGet(action_id);
  const auto &annotations = actionInfo->annotationsGet();
  *num_annotations = annotations.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_action_annotations_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t action_id,
    tdi_annotation_t *annotations_c) {
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto actionInfo = tableInfo->actionGet(action_id);
  const auto &annotations = actionInfo->annotationsGet();
  int i = 0;
  for (const auto &annotation : annotations) {
    annotations_c[i++] = convert_annotation(annotation);
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_num_attributes_supported(
    const tdi_table_info_hdl *table_info_hdl, uint32_t *num_attributes) {
  if (table_info_hdl == nullptr || num_attributes == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  // std::set<tdi::TableAttributesType> type_set;
  auto type_set = tableInfo->attributesSupported();
  *num_attributes = type_set.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_attributes_supported(
    const tdi_table_info_hdl *table_info_hdl,
    tdi_attributes_type_e *attributes,
    uint32_t *num_returned) {
  if (table_info_hdl == nullptr || num_returned == nullptr ||
      attributes == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  // std::set<tdi::TableAttributesType> type_set;
  auto type_set = tableInfo->attributesSupported();
  int i = 0;
  for (const auto &iter : type_set) {
    attributes[i] = static_cast<tdi_attributes_type_e>(iter);
    i++;
  }
  *num_returned = i;
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_num_operations_supported(
    const tdi_table_info_hdl *table_info_hdl, uint32_t *num_operations) {
  if (table_info_hdl == nullptr || num_operations == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  // std::set<tdi::TableOperationsType> type_set;
  auto type_set = tableInfo->operationsSupported();
  *num_operations = type_set.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_operations_supported(
    const tdi_table_info_hdl *table_info_hdl,
    tdi_operations_type_e *operations,
    uint32_t *num_returned) {
  if (table_info_hdl == nullptr || num_returned == nullptr ||
      operations == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  // std::set<tdi::TableOperationsType> type_set;
  auto type_set = tableInfo->operationsSupported();
  int i = 0;
  for (const auto &iter : type_set) {
    operations[i] = static_cast<tdi_operations_type_e>(iter);
    i++;
  }
  *num_returned = i;
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_num_api_supported(
    const tdi_table_info_hdl *table_info_hdl, uint32_t *num_apis) {
  if (table_info_hdl == nullptr || num_apis == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto type_set = tableInfo->apiSupportedGet();
  *num_apis = type_set.api_target_attributes_map_.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_api_supported(const tdi_table_info_hdl *table_info_hdl,
                                     tdi_table_api_type_e *apis,
                                     uint32_t *num_returned) {
  if (table_info_hdl == nullptr || num_returned == nullptr || apis == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(table_info_hdl);
  auto type_set = tableInfo->apiSupportedGet();
  int i = 0;
  for (const auto &iter : type_set.api_target_attributes_map_) {
    apis[i] = static_cast<tdi_table_api_type_e>(iter.first);
    i++;
  }
  *num_returned = i;
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_allowed_choices_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    uint32_t *num_choices) {
  const std::vector<std::string> *cpp_choices = nullptr;
  tdi_status_t sts = ::data_field_allowed_choices_get_helper<uint32_t *>(
      table_info_hdl, field_id, &cpp_choices, num_choices);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num_choices = cpp_choices->size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_allowed_choices_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const char *choices[]) {
  const std::vector<std::string> *cpp_choices = nullptr;
  tdi_status_t sts = ::data_field_allowed_choices_get_helper<const char *[]>(
      table_info_hdl, field_id, &cpp_choices, choices);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  for (auto iter = cpp_choices->begin(); iter != cpp_choices->end(); iter++) {
    choices[iter - cpp_choices->begin()] = iter->c_str();
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_allowed_choices_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num_choices) {
  const std::vector<std::string> *cpp_choices = nullptr;
  tdi_status_t sts = ::data_field_allowed_choices_get_helper<uint32_t *>(
      table_info_hdl, field_id, &cpp_choices, num_choices, action_id);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num_choices = cpp_choices->size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_allowed_choices_with_action_get(
    const tdi_table_info_hdl *table_info_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    const char *choices[]) {
  const std::vector<std::string> *cpp_choices = nullptr;
  tdi_status_t sts = ::data_field_allowed_choices_get_helper<const char *[]>(
      table_info_hdl, field_id, &cpp_choices, choices, action_id);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  for (auto iter = cpp_choices->begin(); iter != cpp_choices->end(); iter++) {
    choices[iter - cpp_choices->begin()] = iter->c_str();
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_num_tables_this_table_depends_on_get(
    const tdi_table_info_hdl *tdi, int *num_tables) {
  if (!tdi || !num_tables) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(tdi);
  std::vector<tdi_id_t> table_ids;
  auto &depends_on_set = tableInfo->dependsOnGet();
  *num_tables = static_cast<int>(depends_on_set.size());
  return TDI_SUCCESS;
}

tdi_status_t tdi_tables_this_table_depends_on_get(const tdi_table_info_hdl *tdi,
                                                  tdi_id_t *table_list) {
  if (!tdi || !table_list) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tableInfo = reinterpret_cast<const tdi::TableInfo *>(tdi);
  std::vector<tdi_id_t> table_ids;
  auto &depends_on_set = tableInfo->dependsOnGet();
  int i = 0;
  for (auto table_id : depends_on_set) {
    table_list[i++] = table_id;
  }
  return TDI_SUCCESS;
}
