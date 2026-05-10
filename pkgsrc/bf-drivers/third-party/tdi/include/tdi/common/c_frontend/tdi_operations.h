/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_operations.h
 *
 * @brief Contains TDI Table operations APIs
 */
#ifndef _TDI_OPERATIONS_H
#define _TDI_OPERATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

tdi_status_t tdi_operations_set_value(tdi_operations_hdl operations,
                                      tdi_operations_field_type_e type,
                                      const uint64_t value);

tdi_status_t tdi_operations_get_value(tdi_operations_hdl operations,
                                      tdi_operations_field_type_e type,
                                      uint64_t *value);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_OPERATIONS_H
