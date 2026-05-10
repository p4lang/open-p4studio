// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include <memory>
// local includes
#include <tdi/common/tdi_utils.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <target-sys/bf_sal/bf_sys_mem.h>
#include <tdi/common/tdi_defs.h>

void tdi_err_str(tdi_status_t sts, const char **err_str) {
  *err_str = bf_err_str(sts);
}

#ifdef __cplusplus
}
#endif
