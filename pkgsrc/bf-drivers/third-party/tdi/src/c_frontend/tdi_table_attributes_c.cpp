// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifdef __cplusplus
extern "C" {
#endif
#include <tdi/common/tdi_defs.h>

#include <tdi/common/c_frontend/tdi_attributes.h>

#ifdef __cplusplus
}
#endif
#include <tdi/common/tdi_attributes.hpp>

tdi_status_t tdi_attributes_set_value(tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                      const uint64_t value) {
  auto attributes_field = reinterpret_cast<tdi::TableAttributes *>(attributes_hdl);
  return attributes_field->setValue(type, value);
}

tdi_status_t tdi_attributes_get_value(const tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                      uint64_t *value) {
  auto attributes_field = reinterpret_cast<const tdi::TableAttributes *>(attributes_hdl);
  return attributes_field->getValue(type, value);
}

tdi_status_t tdi_attributes_set_value_ptr(tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                           const uint8_t *val,
                                           const size_t s) {
  auto attributes_field = reinterpret_cast<tdi::TableAttributes *>(attributes_hdl);
  return attributes_field->setValue(type, val, s);
}

tdi_status_t tdi_attributes_get_value_ptr(const tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                           const size_t size,
                                           uint8_t *val) {
  auto attributes_field = reinterpret_cast<const tdi::TableAttributes *>(attributes_hdl);
  return attributes_field->getValue(type, size, val);
}
