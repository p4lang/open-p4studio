// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
// tdi includes
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_init.hpp>

// arch include
#include <tdi/arch/tna/tna_target.hpp>

// local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {
namespace tna {

tdi_status_t Target::setValue(const tdi_target_e &target_field,
                              const uint64_t &value) {
  if (target_field == static_cast<tdi_target_e>(TDI_TNA_TARGET_PIPE_ID)) {
    this->pipe_id_ = value;
  } else if (target_field ==
             static_cast<tdi_target_e>(TDI_TNA_TARGET_DIRECTION)) {
    this->direction_ = static_cast<tna_direction_e>(value);
  } else {
    return tdi::Target::setValue(target_field, value);
  }
  return TDI_SUCCESS;
}

tdi_status_t Target::getValue(const tdi_target_e &target_field,
                              uint64_t *value) const {
  if (target_field == static_cast<tdi_target_e>(TDI_TNA_TARGET_PIPE_ID)) {
    *value = this->pipe_id_;
  } else if (target_field ==
             static_cast<tdi_target_e>(TDI_TNA_TARGET_DIRECTION)) {
    *value = static_cast<uint64_t>(this->direction_);
  } else {
    return tdi::Target::getValue(target_field, value);
  }
  return TDI_SUCCESS;
}

}  // namespace tna
}  // namespace tdi
