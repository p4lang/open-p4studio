// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tna_init.hpp
 *
 *  @brief Contains TNA specific Device and init defs
 */
#ifndef _TNA_INIT_HPP_
#define _TNA_INIT_HPP_

#include <functional>
#include <memory>
#include <mutex>

// tdi includes
#include <tdi/common/tdi_init.hpp>

// tna includes
#include <tdi/arch/tna/tna_defs.h>
#include <tdi/arch/tna/tna_target.hpp>

namespace tdi {
namespace tna {

/**
 * @brief Class which encapsulates static info of a TNA
 * device eg.
 */
class Device : public tdi::Device {
 public:
  Device(const tdi_dev_id_t &device_id,
         const tdi_arch_type_e &arch_type,
         const std::vector<tdi::ProgramConfig> &device_config,
         void *cookie)
      : tdi::Device(
            device_id, arch_type, device_config, cookie){};

  virtual tdi_status_t createSession(
      std::shared_ptr<tdi::Session> *session) const override;
  virtual tdi_status_t createTarget(
      std::unique_ptr<tdi::Target> *target) const override;

 private:
};

}  // namespace tna
}  // namespace tdi

#endif  // _TNA_INIT_HPP_
