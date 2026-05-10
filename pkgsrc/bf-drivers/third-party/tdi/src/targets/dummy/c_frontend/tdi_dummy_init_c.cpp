// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include "tdi_dummy_init.h"

tdi_status_t tdi_module_init(void *target_options) {
  return tdi::tna::dummy::Init::tdiModuleInit(target_options);
}
