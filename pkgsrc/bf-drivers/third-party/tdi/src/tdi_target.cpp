// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
// tdi includes
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_init.hpp>
#include <tdi/common/tdi_defs.h>

// local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

tdi_status_t Target::setValue(const tdi_target_e &target_e,
                              const uint64_t &value) {
  if (target_e == static_cast<tdi_target_e>(TDI_TARGET_DEV_ID)) {
    this->dev_id_ = value;
  } else {
    return TDI_INVALID_ARG;
  }
  return TDI_SUCCESS;
}

tdi_status_t Target::getValue(const tdi_target_e &target_e,
                              uint64_t *value) const {
  if (target_e == static_cast<tdi_target_e>(TDI_TARGET_DEV_ID)) {
    *value = this->dev_id_;
  } else {
    return TDI_INVALID_ARG;
  }
  return TDI_SUCCESS;
}

tdi_status_t Flags::setValue(const tdi_flags_e &flags_e, const bool &val) {
  if (val)
    this->flags_ |= 1ull << flags_e;
  else
    this->flags_ &= ~(1ull << flags_e);
  return TDI_SUCCESS;
}

tdi_status_t Flags::getValue(const tdi_flags_e &flags_e, bool *value) const {
  *value = (this->flags_ & (1ull << flags_e));
  return TDI_SUCCESS;
}

}  // namespace tdi
