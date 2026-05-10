// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
// tdi includes
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_init.hpp>

// arch include
#include <tdi/arch/tna/tna_defs.h>
#include <tdi/arch/tna/tna_init.hpp>

//local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {
namespace tna {

tdi_status_t Device::createSession(std::shared_ptr<tdi::Session> * /*session*/) const {
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Device::createTarget(std::unique_ptr<tdi::Target> *target) const {
  *target = std::unique_ptr<tdi::Target>(new tdi::tna::Target(this->device_id_,
                                                              TNA_DEV_PIPE_ALL,
                                                              TNA_DIRECTION_ALL));
  return TDI_SUCCESS;
}

} // tna
}  // tdi
