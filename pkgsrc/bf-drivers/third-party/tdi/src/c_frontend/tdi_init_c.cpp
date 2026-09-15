// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include <algorithm>
#include <iterator>
#include <vector>
// tdi includes
#include <tdi/common/tdi_init.hpp>
#include <tdi/common/tdi_target.hpp>
// c_frontend includes
#include <tdi/common/c_frontend/tdi_init.h>

tdi_status_t tdi_device_get(const tdi_dev_id_t dev_id,
                            const tdi_device_hdl **device_hdl_ret) {
  tdi_status_t sts = TDI_SUCCESS;
  const tdi::Device *device = nullptr;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  sts = devMgrObj.deviceGet(dev_id, &device);
  *device_hdl_ret = reinterpret_cast<const tdi_device_hdl *>(device);
  return sts;
}

tdi_status_t tdi_flags_create(const uint64_t flag_value,
                              tdi_flags_hdl **flags) {
  if (flags == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  tdi::Flags *flgs = new tdi::Flags(flag_value);
  *flags = reinterpret_cast<tdi_flags_hdl *>(flgs);
  return TDI_SUCCESS;
}

tdi_status_t tdi_flags_delete(tdi_flags_hdl *flags_hdl) {
  auto flags = reinterpret_cast<tdi::Flags *>(flags_hdl);
  if (flags == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete flags;
  return TDI_SUCCESS;
}

tdi_status_t tdi_flags_set_value(tdi_flags_hdl *flags_hdl,
                                  enum tdi_flags_e flags_field,
                                  bool value) {
  auto flags = reinterpret_cast<tdi::Flags *>(flags_hdl);
  auto sts = flags->setValue(flags_field, value);
  return sts;
}

tdi_status_t tdi_flags_get_value(const tdi_flags_hdl *flags_hdl,
                                  enum tdi_flags_e flags_field,
                                  bool *value) {
  auto flags = reinterpret_cast<const tdi::Flags *>(flags_hdl);
  auto sts = flags->getValue(flags_field, value);
  return sts;
}

tdi_status_t tdi_info_get(const tdi_dev_id_t dev_id,
                          const char *prog_name,
                          const tdi_info_hdl **info_hdl_ret) {
  tdi_status_t sts = TDI_SUCCESS;
  const tdi::TdiInfo *tdiInfo = nullptr;
  std::string program_name(prog_name);
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  // From the dev_id, to get devObj
  const tdi::Device *devObj = nullptr;
  sts = devMgrObj.deviceGet(dev_id, &devObj);
  if (sts != TDI_SUCCESS) {
    return sts;
  }

  sts = devObj->tdiInfoGet(program_name, &tdiInfo);
  if (sts != TDI_SUCCESS) {
    return sts;
  }

  *info_hdl_ret = reinterpret_cast<const tdi_info_hdl *>(tdiInfo);
  return TDI_SUCCESS;
}

tdi_status_t tdi_num_device_id_list_get(uint32_t *num) {
  std::set<tdi_dev_id_t> device_id_list;
  tdi_status_t sts = TDI_SUCCESS;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  sts = devMgrObj.deviceIdListGet(&device_id_list);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num = device_id_list.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_device_id_list_get(tdi_dev_id_t *device_id_list_out) {
  std::set<tdi_dev_id_t> device_id_list;
  tdi_status_t sts = TDI_SUCCESS;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  sts = devMgrObj.deviceIdListGet(&device_id_list);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  int i = 0;
  for (const auto &id : device_id_list) {
    device_id_list_out[i++] = id;
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_target_create(const tdi_device_hdl *device_hdl,
                               tdi_target_hdl **target_hdl) {
  const tdi::Device *device = reinterpret_cast<const tdi::Device *>(device_hdl);
  std::unique_ptr<tdi::Target> target;
  device->createTarget(&target);
  *target_hdl = reinterpret_cast<tdi_target_hdl *>(target.release());
  return TDI_SUCCESS;
}

tdi_status_t tdi_target_delete(tdi_target_hdl *target_hdl) {
  const tdi::Target *target = reinterpret_cast<const tdi::Target *>(target_hdl);
  delete (target);
  return TDI_SUCCESS;
}

tdi_status_t tdi_target_set_value(tdi_target_hdl *target_hdl,
                                  tdi_target_e target_field,
                                  uint64_t value) {
  auto target = reinterpret_cast<tdi::Target *>(target_hdl);
  auto sts = target->setValue(target_field, value);
  return sts;
}

tdi_status_t tdi_target_get_value(const tdi_target_hdl *target_hdl,
                                  tdi_target_e target_field,
                                  uint64_t *value) {
  auto target = reinterpret_cast<const tdi::Target *>(target_hdl);
  auto sts = target->getValue(target_field, value);
  return sts;
}

tdi_status_t tdi_num_p4_names_get(const tdi_dev_id_t dev_id, int *num_names) {
  tdi_status_t sts = TDI_SUCCESS;
  std::vector<std::reference_wrapper<const std::string>> p4_names;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  // From the dev_id, to get devObj
  const tdi::Device *devObj = nullptr;
  sts = devMgrObj.deviceGet(dev_id, &devObj);
  if (sts != TDI_SUCCESS) {
    return sts;
  }

  sts = devObj->p4NamesGet(p4_names);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num_names = static_cast<int>(p4_names.size());
  return sts;
}

tdi_status_t tdi_p4_names_get(const tdi_dev_id_t dev_id,
                              const char **prog_names) {
  tdi_status_t sts = TDI_SUCCESS;
  std::vector<std::reference_wrapper<const std::string>> p4_names;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  // From the dev_id, to get devObj
  const tdi::Device *devObj = nullptr;
  sts = devMgrObj.deviceGet(dev_id, &devObj);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  sts = devObj->p4NamesGet(p4_names);

  /*
   * Iterate through the p4 names and fill the name string array.
   * We use iterators rather than auto so we can use iterator
   * arithmetic to increment our array index rather than a variable.
   * We use the reference_wrapper because we iterate through a list
   * of string references, not string values.
   */
  std::vector<std::reference_wrapper<const std::string>>::iterator it;
  for (it = p4_names.begin(); it != p4_names.end(); ++it) {
    prog_names[it - p4_names.begin()] = it->get().c_str();
  }
  return sts;
}
