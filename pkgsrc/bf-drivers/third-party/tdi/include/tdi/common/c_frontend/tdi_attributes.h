/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_attributes.h
 *
 *  @brief Contains TDI Attribute APIs
 */
#ifndef _TDI_ATTRIBUTES_H
#define _TDI_ATTRIBUTES_H

#include <tdi/common/tdi_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

tdi_status_t tdi_attributes_set_value(tdi_attributes_hdl *attributes,
                                      tdi_attributes_field_type_e type,
                                      const uint64_t value);

tdi_status_t tdi_attributes_get_value(const tdi_attributes_hdl *attributes,
                                      tdi_attributes_field_type_e type,
                                      uint64_t *value);

tdi_status_t tdi_attributes_set_value_ptr(tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                           const uint8_t *val,
                                           const size_t s);

tdi_status_t tdi_attributes_get_value_ptr(const tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                           const size_t size,
                                           uint8_t *val);
#ifdef __cplusplus
}
#endif

#endif  // _TDI_TABLE_ATTRIBUTES_H
