// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_init.hpp
 *
 *  @brief Contains TDI Dev Manager APIs. These APIs help manage TdiInfo
 *  \n objects with respect to the devices and programs in the ecosystem.
 *
 *  \n Contains DevMgr, Device, Target, Flags
 */
#ifndef _TDI_INIT_HPP_
#define _TDI_INIT_HPP_

#include <functional>
#include <memory>
#include <mutex>

// tdi includes
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_target.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

/**
 * @brief Class which encapsulates static info of a device eg.
 * Arch type,
 * Mgrs it was started with, State information, TdiInfo of all
 * programs, Pipeline profile information.
 *
 * Static info means that none of this can be changed after
 * device add happens.
 */
class Device {
 public:
  Device(const tdi_dev_id_t &device_id,
         const tdi_arch_type_e &arch_type,
         const std::vector<tdi::ProgramConfig> &device_config,
         void *cookie)
      : device_id_(device_id),
        arch_type_(arch_type),
        device_config_(device_config),
        cookie_(cookie){};

  virtual ~Device(){};

  /**
   * @brief Get the TdiInfo object corresponding to the program name
   *
   * @param[in] prog_name Name of the P4 program
   * @param[out] tdi_info TdiInfo Obj associated with the Device
   *    and the program name
   *
   * @return Status of the API call
   */
  tdi_status_t tdiInfoGet(const std::string &prog_name,
                          const TdiInfo **tdi_info) const;

  /**
   * @brief Get a vector of loaded p4 program names on this device
   *
   * @param[out] p4_names Vector containing const string references to the
   * P4 names loaded on the device
   *
   * @return Status of the API call
   */
  tdi_status_t p4NamesGet(
      std::vector<std::reference_wrapper<const std::string>> &p4_names) const;

  tdi_status_t deviceConfigGet(
      const std::vector<tdi::ProgramConfig> **device_config) const;

  virtual tdi_status_t createSession(
      std::shared_ptr<tdi::Session> *session) const;
  virtual tdi_status_t createTarget(std::unique_ptr<tdi::Target> *target) const;
  virtual tdi_status_t createFlags(const uint64_t &flags_val,
                                   std::unique_ptr<tdi::Flags> *flags) const;

 protected:
  const tdi_dev_id_t device_id_;
  const tdi_arch_type_e arch_type_;
  const std::vector<tdi::ProgramConfig> device_config_;
  const void *cookie_;
  std::map<std::string, std::unique_ptr<const TdiInfo>> tdi_info_map_;
};

/**
 * @brief Class to store warmInit options
 */
class WarmInitOptions {
 public:
  virtual ~WarmInitOptions() = default;
};

/**
 * @brief Class to encapsulate warmInit operations
 * Driver to derive and implement the APIs
 */
class WarmInitImpl {
 public:
  virtual ~WarmInitImpl() = default;
  virtual tdi_status_t deviceWarmInitBegin(const tdi_dev_id_t & /*device_id*/,
                            const WarmInitOptions & /*warm_init_options*/) {
    return TDI_NOT_SUPPORTED;
  }
  virtual tdi_status_t deviceWarmInitEnd(const tdi_dev_id_t & /*device_id*/) {
    return TDI_NOT_SUPPORTED;
  }
 protected:
  WarmInitImpl() = default;
};


/**
 * @brief Class to manage Device per dev_id.<br>
 * <B>Creation: </B> Singleton....
 */
class DevMgr {
 public:

  /**
   * @brief initialize warmInitImpl object
   * This function sets warm_init_impl objects which is needed by
   * deviceWarmInitBegin and deviceWarmInitEnd APIs
   */
  static void warmInitImplSet(std::unique_ptr<WarmInitImpl> impl);

  /**
   * @brief Get the singleton ojbect
   *
   * @return Ref to Singleton DevMgr object
   */
  static DevMgr &getInstance();

  /**
   * @brief Get the TdiInfo object corresponding to the (device_id,
   * program name)
   *
   * @param[in] dev_id Device ID
   * @param[out] Device Device object
   *
   * @return Status of the API call
   */
  tdi_status_t deviceGet(const tdi_dev_id_t &dev_id,
                         const tdi::Device **device) const;

  /**
   * @brief Get a list of all device IDs currently added
   *
   * @param[out] device_id_list Set Contains list of device
   * IDs that are present.
   *
   * @return Status of the API call
   */
  tdi_status_t deviceIdListGet(std::set<tdi_dev_id_t> *device_id_list) const;

  /**
   * @brief Device Add function which creates a Device object and maintains it
   *
   * @param[in] device_id Device ID
   * @param[in] arch_type P4 architecture type
   * @param[in] device_config Vector of program configs which defines a Device config
   * @param[in] target_options Target-specific extra options if the target requires to
   *              pass any more info during deviceAdd
   * @param[in] cookie User defined cookie which platforms can use to
   *              send any additional information they want to help with inititalization
   *
   * @return Status of API call
   */
  template <typename T>
  tdi_status_t deviceAdd(const tdi_dev_id_t &device_id,
                         const tdi_arch_type_e &arch_type,
                         const std::vector<tdi::ProgramConfig> &device_config,
                         void *target_options,
                         void *cookie) {
    if (this->dev_map_.find(device_id) != this->dev_map_.end()) {
      LOG_ERROR("%s:%d Device obj exists for dev : %d",
                __func__,
                __LINE__,
                device_id);
      return TDI_ALREADY_EXISTS;
    }
    auto dev = std::unique_ptr<tdi::Device>(
        new T(device_id, arch_type, device_config, target_options, cookie));
    this->dev_map_[device_id] = std::move(dev);
    return TDI_SUCCESS;
  }

  tdi_status_t deviceRemove(const tdi_dev_id_t &device_id);

  tdi_status_t deviceWarmInitBegin(const tdi_dev_id_t &device_id, const WarmInitOptions &warm_init_options);
  tdi_status_t deviceWarmInitEnd(const tdi_dev_id_t &device_id);

  DevMgr(DevMgr const &) = delete;
  DevMgr(DevMgr &&) = delete;
  DevMgr &operator=(const DevMgr &) = delete;
  DevMgr &operator=(DevMgr &&) = delete;

 protected:
  std::map<tdi_dev_id_t, std::unique_ptr<Device>> dev_map_;

 private:
  DevMgr(){};

  static std::mutex dev_mgr_instance_mutex;
  static DevMgr *dev_mgr_instance;

  static std::unique_ptr<WarmInitImpl> warm_init_impl;
};  // DevMgr

/**
 * @brief Class to manage initialization of TDI <br>
 * <B>Creation: </B> Cannot be created
 */
class Init {
 public:
  /**
   * @brief TDI Module Init API. This function needs to be called to
   * initialize TDI. Some specific managers can be specified to be skipped
   * TDI initialization. This allows TDI session layer to not know about these
   * managers. By default, no mgr initialization is skipped if empty vector is
   * passed
   *
   * @param[in] target_options Target-specific args if the target needs to initialize
   *          TDI with certain parameters for example, by only initializing
   *          certain backend managers.
   * @return Status of the API call
   */
  static tdi_status_t tdiModuleInit(void *target_options);
};  // Init

}  // namespace tdi

#endif  // _TDI_INIT_HPP_
