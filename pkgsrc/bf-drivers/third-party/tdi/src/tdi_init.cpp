// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
// tdi includes
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_init.hpp>

// local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

DevMgr *DevMgr::dev_mgr_instance = nullptr;
std::mutex DevMgr::dev_mgr_instance_mutex;
std::unique_ptr<WarmInitImpl> DevMgr::warm_init_impl(nullptr);

tdi_status_t Device::tdiInfoGet(const std::string &prog_name,
                                const TdiInfo **tdi_info) const {
  if (this->tdi_info_map_.find(prog_name) == this->tdi_info_map_.end()) {
    LOG_ERROR("%s:%d TDI Info Object not found for dev : %d",
              __func__,
              __LINE__,
              this->device_id_);
    return TDI_OBJECT_NOT_FOUND;
  }

  *tdi_info = this->tdi_info_map_.at(prog_name).get();
  return TDI_SUCCESS;
}

tdi_status_t Device::p4NamesGet(
    std::vector<std::reference_wrapper<const std::string>> &p4_names) const {
  for (const auto &it : this->tdi_info_map_) {
    p4_names.push_back(std::cref(it.first));
  }
  return TDI_SUCCESS;
}

tdi_status_t Device::deviceConfigGet(
    const std::vector<tdi::ProgramConfig> **device_config) const {
  *device_config = &(this->device_config_);
  return TDI_SUCCESS;
}

tdi_status_t Device::createSession(
    std::shared_ptr<tdi::Session> * /*session*/) const {
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Device::createTarget(std::unique_ptr<tdi::Target> *target) const {
  *target = std::unique_ptr<Target>(new Target(this->device_id_));
  return TDI_SUCCESS;
}

tdi_status_t Device::createFlags(const uint64_t &flags_val,
                                 std::unique_ptr<tdi::Flags> *flags) const {
  *flags = std::unique_ptr<Flags>(new Flags(flags_val));
  return TDI_SUCCESS;
}

DevMgr &DevMgr::getInstance() {
  if (dev_mgr_instance == nullptr) {
    dev_mgr_instance_mutex.lock();
    if (dev_mgr_instance == nullptr) {
      dev_mgr_instance = new DevMgr();
    }
    dev_mgr_instance_mutex.unlock();
  }
  return *(DevMgr::dev_mgr_instance);
}

tdi_status_t DevMgr::deviceGet(const tdi_dev_id_t &dev_id,
                               const tdi::Device **device) const {
  if (this->dev_map_.find(dev_id) == this->dev_map_.end()) {
    LOG_ERROR("%s:%d Device Object not found for dev : %d",
              __func__,
              __LINE__,
              dev_id);
    return TDI_OBJECT_NOT_FOUND;
  }

  *device = this->dev_map_.at(dev_id).get();
  return TDI_SUCCESS;
}

tdi_status_t DevMgr::deviceIdListGet(
    std::set<tdi_dev_id_t> *device_id_list) const {
  if (device_id_list == nullptr) {
    LOG_ERROR("%s:%d Please allocate space for out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  for (const auto &pair : this->dev_map_) {
    if ((*device_id_list).find(pair.first) == (*device_id_list).end()) {
      (*device_id_list).insert(pair.first);
    }
  }
  return TDI_SUCCESS;
}

tdi_status_t DevMgr::deviceRemove(const tdi_dev_id_t &dev_id) {

  if (this->dev_map_.find(dev_id) != this->dev_map_.end())
    this->dev_map_.erase(dev_id);

  LOG_DBG(
      "%s:%d  Device Remove called for dev : %d", __func__, __LINE__, dev_id);
  return TDI_SUCCESS;
}

void DevMgr::warmInitImplSet(std::unique_ptr<WarmInitImpl> impl) {
  warm_init_impl = std::move(impl);
}

tdi_status_t DevMgr::deviceWarmInitBegin(const tdi_dev_id_t &device_id,
                              const WarmInitOptions &warm_init_options) {
  if(warm_init_impl == nullptr) {
    LOG_ERROR("%s:%d warmInitImpl not initialized", __func__, __LINE__);
    return TDI_INTERNAL_ERROR;
  }
  return warm_init_impl->deviceWarmInitBegin(device_id, warm_init_options);
}

tdi_status_t DevMgr::deviceWarmInitEnd(const tdi_dev_id_t &device_id) {
  if(warm_init_impl == nullptr) {
    LOG_ERROR("%s:%d warmInitImpl not initialized", __func__, __LINE__);
    return TDI_INTERNAL_ERROR;
  }
  return warm_init_impl->deviceWarmInitEnd(device_id);
}

tdi_status_t Init::tdiModuleInit(void * /*target_options*/) {
  return TDI_SUCCESS;
}

}  // namespace tdi
