// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <tdi/common/c_frontend/tdi_table_key.h>

#ifdef __cplusplus
}
#endif
#include <string.h>
#include <tdi/common/tdi_table_key.hpp>
//#include <tdi/common/tdi_table_key_obj.hpp>
//#include <tdi_common/tdi_table_key_impl.hpp>

/** Set */
/* Exact */
tdi_status_t tdi_key_field_set_value(tdi_table_key_hdl *key_hdl,
                                      const tdi_id_t field_id,
                                      const uint64_t value) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueExact <const uint64_t> keyFieldValue(value);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_ptr(tdi_table_key_hdl *key_hdl,
                                          const tdi_id_t field_id,
                                          const uint8_t *value,
                                          const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueExact <const uint8_t *> keyFieldValue(value, size);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_string(tdi_table_key_hdl *key_hdl,
                                             const tdi_id_t field_id,
                                             const char *value) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  //tdi::KeyFieldValueExact<const std::string> keyFieldValue(std::string(value));
  tdi::KeyFieldValueExact<const char *> keyFieldValue(value, strlen(value));
  return key->setValue(field_id, keyFieldValue);
}

/* Ternary */
tdi_status_t tdi_key_field_set_value_and_mask(tdi_table_key_hdl *key_hdl,
                                               const tdi_id_t field_id,
                                               const uint64_t value,
                                               const uint64_t mask) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  
  //return key->setValueandMask(field_id, value, mask);
  //return key->setValue(field_id, value, mask);
  tdi::KeyFieldValueTernary <const uint64_t> keyFieldValue(value, mask);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_and_mask_ptr(tdi_table_key_hdl *key_hdl,
                                                   const tdi_id_t field_id,
                                                   const uint8_t *value1,
                                                   const uint8_t *mask,
                                                   const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueTernary <const uint8_t *> keyFieldValue(value1, mask, size);
  //return key->setValueandMask(field_id, value1, mask, size);
  return key->setValue(field_id, keyFieldValue);
}

/* Range */
tdi_status_t tdi_key_field_set_value_range(tdi_table_key_hdl *key_hdl,
                                            const tdi_id_t field_id,
                                            const uint64_t start,
                                            const uint64_t end) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueRange<const uint64_t> keyFieldValue(start, end);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_range_ptr(tdi_table_key_hdl *key_hdl,
                                                const tdi_id_t field_id,
                                                const uint8_t *start,
                                                const uint8_t *end,
                                                const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueRange<const uint8_t *> keyFieldValue(start, end, size);
  return key->setValue(field_id, keyFieldValue);
}

/* LPM */
tdi_status_t tdi_key_field_set_value_lpm(tdi_table_key_hdl *key_hdl,
                                          const tdi_id_t field_id,
                                          const uint64_t value1,
                                          const uint16_t p_length) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueLPM <const uint64_t> keyFieldValue(value1, p_length);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_lpm_ptr(tdi_table_key_hdl *key_hdl,
                                              const tdi_id_t field_id,
                                              const uint8_t *value1,
                                              const uint16_t p_length,
                                              const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueLPM <const uint8_t *> keyFieldValue(value1, p_length, size);
  return key->setValue(field_id, keyFieldValue);
}

/** Get */
/* Exact */
tdi_status_t tdi_key_field_get_value(const tdi_table_key_hdl *key_hdl,
                                     const tdi_id_t field_id,
                                     uint64_t *value) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueExact <uint64_t> keyFieldValue(*value);
  auto sts = key->getValue(field_id, &keyFieldValue);
  *value = keyFieldValue.value_;
  return(sts);
}

tdi_status_t tdi_key_field_get_value_ptr(const tdi_table_key_hdl *key_hdl,
                                         const tdi_id_t field_id,
                                         const size_t size,
                                         uint8_t *value) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueExact <uint8_t *> keyFieldValue(value, size);
  auto sts = key->getValue(field_id, &keyFieldValue);
  value = keyFieldValue.value_;
  return(sts);
}

/* string */
tdi_status_t tdi_key_field_get_value_string_size(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    uint32_t *str_size) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  std::string str;
  tdi::KeyFieldValueExact<std::string> keyFieldValue(str);
  tdi_status_t status = key->getValue(field_id, &keyFieldValue);
  str = keyFieldValue.value_;
  *str_size = str.size();
  return status;
}

tdi_status_t tdi_key_field_get_value_string(const tdi_table_key_hdl *key_hdl,
                                            const tdi_id_t field_id,
                                            char *value) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  std::string str(value);
  tdi::KeyFieldValueExact<std::string> keyFieldValue(str);
  tdi_status_t status = key->getValue(field_id, &keyFieldValue);
  str = keyFieldValue.value_;
  strncpy(value, str.c_str(), str.size());
  return status;
}

/* Ternary */
tdi_status_t tdi_key_field_get_value_and_mask(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    uint64_t *value1,
    uint64_t *value2) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueTernary<uint64_t> keyFieldValue(*value1, *value2);
  auto sts = key->getValue(field_id, &keyFieldValue);
  *value1 = keyFieldValue.value_;
  *value2 = keyFieldValue.mask_;
  return sts;
}

tdi_status_t tdi_key_field_get_value_and_mask_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *value1,
    uint8_t *value2) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueTernary<uint8_t *> keyFieldValue(value1, value2, size);
  auto sts = key->getValue(field_id, &keyFieldValue);
  value1 = keyFieldValue.value_;
  value2 = keyFieldValue.mask_;
  return sts;
}

/* Range */
tdi_status_t tdi_key_field_get_value_range(const tdi_table_key_hdl *key_hdl,
                                           const tdi_id_t field_id,
                                           uint64_t *start,
                                           uint64_t *end) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueRange<uint64_t> keyFieldValue(*start, *end);
  auto sts = key->getValue(field_id, &keyFieldValue);
  *start = keyFieldValue.low_;
  *end = keyFieldValue.high_;
  return sts;
}

tdi_status_t tdi_key_field_get_value_range_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *start,
    uint8_t *end) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueRange<uint8_t *> keyFieldValue(start, end, size);
  auto sts = key->getValue(field_id, &keyFieldValue);
  start = keyFieldValue.low_;
  end = keyFieldValue.high_;
  return sts;
}

/* LPM */
tdi_status_t tdi_key_field_get_value_lpm(const tdi_table_key_hdl *key_hdl,
                                         const tdi_id_t field_id,
                                         uint64_t *start,
                                         uint16_t *p_length) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueLPM<uint64_t> keyFieldValue(*start, *p_length);
  auto sts=key->getValue(field_id, &keyFieldValue);
  *start = keyFieldValue.value_;
  *p_length = keyFieldValue.prefix_len_;
  return(sts);
}

tdi_status_t tdi_key_field_get_value_lpm_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *value1,
    uint16_t *p_length) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueLPM<uint8_t *> keyFieldValue(value1, *p_length, size);
  auto sts = key->getValue(field_id, &keyFieldValue);
  value1 = keyFieldValue.value_;
  *p_length = keyFieldValue.prefix_len_;
  return(sts);
}
