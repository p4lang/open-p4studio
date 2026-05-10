// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifdef __cplusplus
extern "C" {
#endif
//#include <tdi/common/c_frontend/tdi_common.h>
#include <tdi/common/tdi_defs.h>

#include <tdi/common/c_frontend/tdi_table_data.h>

#ifdef __cplusplus
}
#endif

#include <tdi/common/tdi_table_data.hpp>
//#include <tdi_common/tdi_table_data_impl.hpp>

/* Data field setters/getter */
tdi_status_t tdi_data_field_set_value(tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       const uint64_t val) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  return data_field->setValue(field_id, val);
}

tdi_status_t tdi_data_field_set_value_int64(tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       const int64_t val) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  return data_field->setValue(field_id, val);
}

tdi_status_t tdi_data_field_set_float(tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       const float val) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  return data_field->setValue(field_id, val);
}

tdi_status_t tdi_data_field_set_value_ptr(tdi_table_data_hdl *data_hdl,
                                           const tdi_id_t field_id,
                                           const uint8_t *val,
                                           const size_t s) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  return data_field->setValue(field_id, val, s);
}

tdi_status_t tdi_data_field_set_value_array(tdi_table_data_hdl *data_hdl,
                                             const tdi_id_t field_id,
                                             const uint32_t *val,
                                             const uint32_t num_array) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  // array pointers work as iterators
  const auto vec = std::vector<tdi_id_t>(val, val + num_array);
  return data_field->setValue(field_id, vec);
}

tdi_status_t tdi_data_field_set_value_bool_array(
    tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    const bool *val,
    const uint32_t num_array) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  // array pointers work as iterators
  const auto vec = std::vector<bool>(val, val + num_array);
  return data_field->setValue(field_id, vec);
}

tdi_status_t tdi_data_field_set_value_str_array(tdi_table_data_hdl *data_hdl,
                                                 const tdi_id_t field_id,
                                                 const char *val) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  std::vector<std::string> vec;
  char *val_dup = strdup(val);
  char *token = strtok(val_dup, " ");
  while (token != NULL) {
    vec.push_back(std::string(token));
    token = strtok(NULL, " ");
  }
  free(val_dup);
  return data_field->setValue(field_id, vec);
}

tdi_status_t tdi_data_field_set_value_data_field_array(
    tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    tdi_table_data_hdl *val[],
    const uint32_t num_array) {
  std::vector<std::unique_ptr<tdi::TableData>> inner_vec;
  for (uint32_t i = 0; i < num_array; i++) {
    auto inner_data = std::unique_ptr<tdi::TableData>(
        reinterpret_cast<tdi::TableData *>(val[i]));
    inner_vec.push_back(std::move(inner_data));
  }
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  return data_field->setValue(field_id, std::move(inner_vec));
}

tdi_status_t tdi_data_field_set_bool(tdi_table_data_hdl *data_hdl,
                                      const tdi_id_t field_id,
                                      const bool val) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  return data_field->setValue(field_id, val);
}

tdi_status_t tdi_data_field_set_string(tdi_table_data_hdl *data_hdl,
                                        const tdi_id_t field_id,
                                        const char *val) {
  auto data_field = reinterpret_cast<tdi::TableData *>(data_hdl);
  const std::string str_val(val);
  return data_field->setValue(field_id, str_val);
}

tdi_status_t tdi_data_field_get_value(const tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       uint64_t *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  return data_field->getValue(field_id, val);
}

tdi_status_t tdi_data_field_get_value_int64(const tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       int64_t *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  return data_field->getValue(field_id, val);
}

tdi_status_t tdi_data_field_get_value_ptr(const tdi_table_data_hdl *data_hdl,
                                           const tdi_id_t field_id,
                                           const size_t size,
                                           uint8_t *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  return data_field->getValue(field_id, size, val);
}

tdi_status_t tdi_data_field_get_value_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<tdi_id_t> vec;
  auto status = data_field->getValue(field_id, &vec);
  int i = 0;
  for (auto const &item : vec) {
    val[i++] = item;
  }
  return status;
}

tdi_status_t tdi_data_field_get_value_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<tdi_id_t> vec;
  auto status = data_field->getValue(field_id, &vec);
  *array_size = vec.size();
  return status;
}

tdi_status_t tdi_data_field_get_value_bool_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    bool *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<bool> vec;
  auto status = data_field->getValue(field_id, &vec);
  int i = 0;
  for (auto const &item : vec) {
    val[i++] = item;
  }
  return status;
}

tdi_status_t tdi_data_field_get_value_bool_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<bool> vec;
  auto status = data_field->getValue(field_id, &vec);
  *array_size = vec.size();
  return status;
}

tdi_status_t tdi_data_field_get_value_str_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t size,
    char *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<std::string> vec;
  auto status = data_field->getValue(field_id, &vec);
  if ((size == 0) && (vec.size() == 0)) return status;
  bool first_item = true;
  for (auto const &item : vec) {
    if (first_item) {
      first_item = false;
    } else {
      if (size == 0) return TDI_INVALID_ARG;
      val[0] = ' ';
      val++;
      size--;
    }
    if (size <= item.size()) return TDI_INVALID_ARG;
    item.copy(val, item.size());
    val += item.size();
    size -= item.size();
  }
  if (size == 0) return TDI_INVALID_ARG;
  val[0] = '\0';
  return status;
}

tdi_status_t tdi_data_field_get_value_str_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<std::string> vec;
  auto status = data_field->getValue(field_id, &vec);
  /* Return the size of all strings plus white spaces between them. */
  for (auto const &item : vec) *array_size += (item.size() + 1);
  return status;
}

tdi_status_t tdi_data_field_get_float(const tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       float *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  return data_field->getValue(field_id, val);
}

tdi_status_t tdi_data_field_get_bool(const tdi_table_data_hdl *data_hdl,
                                      const tdi_id_t field_id,
                                      bool *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  return data_field->getValue(field_id, val);
}

tdi_status_t tdi_data_field_get_string_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *str_size) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::string str;
  auto status = data_field->getValue(field_id, &str);
  *str_size = str.size();
  return status;
}

tdi_status_t tdi_data_field_get_string(const tdi_table_data_hdl *data_hdl,
                                        const tdi_id_t field_id,
                                        char *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::string str;
  auto status = data_field->getValue(field_id, &str);
  str.copy(val, str.size());
  return status;
}

tdi_status_t tdi_data_field_get_value_u64_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint64_t *val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<uint64_t> vec;
  auto status = data_field->getValue(field_id, &vec);
  int i = 0;
  for (auto const &item : vec) {
    val[i++] = item;
  }
  return status;
}

tdi_status_t tdi_data_field_get_value_u64_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<uint64_t> vec;
  auto status = data_field->getValue(field_id, &vec);
  *array_size = vec.size();
  return status;
}

tdi_status_t tdi_data_field_get_value_data_field_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    tdi_table_data_hdl **val) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<tdi::TableData *> vec;
  auto status = data_field->getValue(field_id, &vec);
  int i = 0;
  for (auto const &item : vec) {
    val[i++] = reinterpret_cast<tdi_table_data_hdl *>(item);
  }
  return status;
}

tdi_status_t tdi_data_field_get_value_data_field_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  std::vector<tdi::TableData *> vec;
  auto status = data_field->getValue(field_id, &vec);
  *array_size = vec.size();
  return status;
}

tdi_status_t tdi_data_action_id_get(const tdi_table_data_hdl *data_hdl,
                                     uint32_t *action_id) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  *action_id = data_field->actionIdGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_active(const tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       bool *is_active) {
  auto data_field = reinterpret_cast<const tdi::TableData *>(data_hdl);
  return data_field->isActive(field_id, is_active);
}
