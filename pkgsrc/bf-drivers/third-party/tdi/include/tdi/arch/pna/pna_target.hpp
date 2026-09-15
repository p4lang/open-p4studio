// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file pna_target.hpp
 *
 *  @brief Contains PNA specific common defs like Target
 */
#ifndef _PNA_TARGET_HPP_
#define _PNA_TARGET_HPP_

#include <functional>
#include <memory>
#include <mutex>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_target.hpp>

// pna include
#include <tdi/arch/pna/pna_defs.h>
#include <tdi/arch/pna/pna_init.hpp>

namespace tdi {
namespace pna {

class Device;

enum pna_target_e {
  PNA_TARGET_PIPE_ID = TDI_TARGET_ARCH+0,
  PNA_TARGET_DIRECTION = TDI_TARGET_ARCH+1,
};

/**
 * @brief Can be constructed by \ref pna::Device::createTarget()
 */
class Target : public tdi::Target {
 public:
  virtual ~Target() = default;
  virtual tdi_status_t setValue(const tdi_target_e & /*target*/,
                                const uint64_t & /*value*/) override;
  virtual tdi_status_t getValue(const tdi_target_e & /*target*/,
                                uint64_t * /*value*/) const override;

 protected:
  Target(tdi_dev_id_t dev_id, pna_pipe_id_t pipe_id, pna_direction_e direction)
      : tdi::Target(dev_id), pipe_id_(pipe_id), direction_(direction){};
  pna_pipe_id_t pipe_id_;
  pna_direction_e direction_;

  friend class tdi::pna::Device;
};

}  // namespace pna
}  // namespace tdi

#endif  // _PNA_TARGET_HPP_
