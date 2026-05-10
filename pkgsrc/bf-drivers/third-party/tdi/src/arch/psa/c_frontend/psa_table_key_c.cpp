// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <tdi/common/c_frontend/tdi_table_key.h>
#include <tdi/arch/psa/c_frontend/psa_table_key.h>

#ifdef __cplusplus
}
#endif
#include <tdi/arch/psa/psa_table_key.hpp>
//#include <tdi/common/tdi_table_key_obj.hpp>
//#include <tdi_common/tdi_table_key_impl.hpp>

/** Set */
/* Optional */
tdi_status_t tdi_key_field_set_value_optional(tdi_table_key_hdl *key_hdl,
                                              const tdi_id_t field_id,
                                              const uint64_t value1,
                                              const bool is_valid) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueOptional<const uint8_t> keyFieldValue(value1, is_valid);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_optional_ptr(tdi_table_key_hdl *key_hdl,
                                                  const tdi_id_t field_id,
                                                  const uint8_t *value1,
                                                  const bool is_valid,
                                                  const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueOptional<const uint8_t *> keyFieldValue(value1, is_valid, size);
  return key->setValue(field_id, keyFieldValue);
}

/** Get */
/* OPTIONAL */
tdi_status_t tdi_key_field_get_value_optional(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    uint64_t *value,
    bool *is_valid) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueOptional<long unsigned int> keyFieldValue(*value, *is_valid);
  auto sts = key->getValue(field_id, &keyFieldValue);
  *value = keyFieldValue.value_;
  *is_valid = keyFieldValue.is_valid_;
  return sts; 
}

tdi_status_t tdi_key_field_get_value_optional_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *value1,
    bool *is_valid) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueOptional<uint8_t *> keyFieldValue(value1, *is_valid, size);
  auto sts = key->getValue(field_id, &keyFieldValue);
  value1 = keyFieldValue.value_;
  *is_valid = keyFieldValue.is_valid_;
  return sts; 
}
