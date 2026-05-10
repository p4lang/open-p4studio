// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
// tdi includes
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_init.hpp>

// arch include
#include <tdi/arch/pna/pna_defs.h>
#include <tdi/arch/pna/pna_init.hpp>

//local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {
namespace pna {

tdi_status_t Device::createSession(std::shared_ptr<tdi::Session> * /*session*/) const {
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Device::createTarget(std::unique_ptr<tdi::Target> *target) const {
  *target = std::unique_ptr<tdi::Target>(new tdi::pna::Target(this->device_id_,
                                                              PNA_DEV_PIPE_ALL,
                                                              PNA_DIRECTION_ALL));
  return TDI_SUCCESS;
}

} // pna
}  // tdi
