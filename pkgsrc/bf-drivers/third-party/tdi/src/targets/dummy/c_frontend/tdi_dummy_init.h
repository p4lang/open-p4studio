/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_dummy_init.h
 *
 *  @brief C frontend for init
 */
#ifndef _TDI_DUMMY_INIT_H_
#define _TDI_DUMMY_INIT_H_

// tdi includes
#include <tdi/common/tdi_defs.h>

#include "../tdi_dummy_init.hpp"

#ifdef __cplusplus
extern "C" {
#endif

tdi_status_t tdi_module_init(void *target_options);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_DUMMY_INIT_H_
