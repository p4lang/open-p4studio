// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file pna_init.hpp
 *
 *  @brief Contains PNA specific Device and init defs
 */
#ifndef _PNA_INIT_HPP_
#define _PNA_INIT_HPP_

#include <functional>
#include <memory>
#include <mutex>

// tdi includes
#include <tdi/common/tdi_init.hpp>

// pna includes
#include <tdi/arch/pna/pna_defs.h>
#include <tdi/arch/pna/pna_target.hpp>

namespace tdi {
namespace pna {

/**
 * @brief Class which encapsulates static info of a PNA
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

}  // namespace pna
}  // namespace tdi

#endif  // _PNA_INIT_HPP_
