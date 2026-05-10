// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_dummy_init.hpp
 *
 *  @brief Contains common target-specific init function decls
 */
#ifndef _TDI_DUMMY_INIT_HPP_
#define _TDI_DUMMY_INIT_HPP_

#include <memory>
#include <vector>

// tdi includes
#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_init.hpp>

// tna includes
#include <tdi/arch/tna/tna_init.hpp>

// dummy includes
//#include "tdi_dummy_info.hpp"

namespace tdi {
namespace tna {
namespace dummy {

/**
 * @brief Class which encapsulates static info of a device eg.
 * Arch type,
 * Mgrs it was started with, State information, TdiInfo of all
 * programs, Pipeline profile information.
 *
 * Static info means that none of this can be changed after
 * device add happens.
 */
class Device : public tdi::tna::Device {
 public:
  Device(const tdi_dev_id_t &device_id,
         const tdi_arch_type_e &arch_type,
         const std::vector<tdi::ProgramConfig> &device_config,
         void *target_options,
         void *cookie);

  virtual tdi_status_t createSession(
      std::shared_ptr<tdi::Session> * /*session*/) const override final {
    return TDI_SUCCESS;
  }
  virtual tdi_status_t createTarget(
      std::unique_ptr<tdi::Target> * /*target*/) const override final {
    return TDI_SUCCESS;
  }
  virtual tdi_status_t createFlags(
      const uint64_t & /*flags_val*/,
      std::unique_ptr<tdi::Flags> * /*flags*/) const override final {
    return TDI_SUCCESS;
  }
};

/**
 * @brief Class to manage initialization of TDI <br>
 * <B>Creation: </B> Cannot be created
 */
class Init : public tdi::Init {
 public:
  /**
   * @brief TDI Module Init API. This function needs to be called to
   * initialize TDI. Some specific managers can be specified to be skipped
   * TDI initialization. This allows TDI session layer to not know about these
   * managers. By default, no mgr initialization is skipped if empty vector is
   * passed
   *
   * @param[in] target_options
   * @return Status of the API call
   */
  static tdi_status_t tdiModuleInit(void *target_options);
};  // Init

}  // namespace dummy
}  // namespace tna
}  // namespace tdi

#endif  // _TDI_DUMMY_INIT_HPP_
