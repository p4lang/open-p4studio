// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_target.hpp
 *
 *  @brief Contains common cpp defines like Target, Flags
 */
#ifndef _TDI_TARGET_HPP_
#define _TDI_TARGET_HPP_

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

// tdi includes
#include <tdi/common/tdi_defs.h>

namespace tdi {

class Device;

/**
 * @brief Contains pipeline Info of a basic TDI device.
 */
class P4Pipeline {
 public:
  P4Pipeline(const std::string &name,
             const std::string context_path,
             const std::string &binary_path,
             const std::vector<uint32_t> &scope_vec)
      : name_(name),
        context_path_(context_path),
        binary_path_(binary_path),
        scope_vec_(scope_vec){};
  const std::string name_;
  const std::string context_path_;
  const std::string binary_path_;
  const std::vector<uint32_t> scope_vec_;
};

/**
 * @brief This class contains Program configuration information
 * like program name, TDI json path and information regarding
 * all the pipeline_profiles of this Program. Maintained in
 * tdi::Device
 */
class ProgramConfig {
 public:
  ProgramConfig(const std::string &prog_name,
                const std::vector<std::string> &tdi_info_file_paths,
                const std::vector<tdi::P4Pipeline> &p4_pipelines)
      : prog_name_(prog_name),
        tdi_info_file_paths_(tdi_info_file_paths),
        p4_pipelines_(p4_pipelines){};
  const std::string prog_name_;
  const std::vector<std::string> tdi_info_file_paths_;
  const std::vector<tdi::P4Pipeline> p4_pipelines_;
};

/**
 * @brief Can be constructed by \ref tdi::Device::createTarget()
 */
class Target {
 public:
  virtual ~Target() = default;
  virtual tdi_status_t setValue(const tdi_target_e &target,
                                const uint64_t &value);
  virtual tdi_status_t getValue(const tdi_target_e &target,
                                uint64_t *value) const;

 protected:
  Target(const tdi_dev_id_t &dev_id) : dev_id_(dev_id){};
  tdi_dev_id_t dev_id_;

  friend class tdi::Device;
};

/**
 * @brief Simple wrapper over a uin64_t flags. Each bit is controlled by
 * tdi_flags_e values.
 * Can be constructed by \ref tdi::Device::createFlags()
 */
class Flags {
 public:
  Flags(const uint64_t &flags) : flags_(flags){};
  virtual ~Flags() = default;
  tdi_status_t setValue(const tdi_flags_e &flags, const bool &val);
  tdi_status_t getValue(const tdi_flags_e &flags, bool *val) const;
  const uint64_t &getFlags() { return flags_; };
  uint64_t flags_ = 0;
};

}  // namespace tdi

#endif  // _TDI_DEFS_HPP_
