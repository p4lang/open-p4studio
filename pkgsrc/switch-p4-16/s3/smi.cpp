/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/


#include "s3/smi.h"

#include <unordered_map>
#include <vector>
#include <set>
#include <cinttypes>
#include <algorithm>

#include "s3/switch_store.h"
#include "s3/attribute.h"
#include "./log.h"

namespace smi {
namespace api {
using ::smi::logging::switch_log;

/* cache flags */
std::unordered_map<switch_attr_id_t, switch_attr_flags_t> flags_cache;
static switch_attr_flags_t empty_flags = {};

/**
 * The reason we are performing these flag validations here instead of
 * switch_store is we want the capability to update immutable or read only
 * fields inside the framework
 */
const switch_attr_flags_t &get_flags(const switch_object_type_t object_type,
                                     const switch_attr_id_t attr_id) {
  auto flags_cache_it = flags_cache.find(attr_id);
  if (flags_cache_it == flags_cache.end()) {
    ModelInfo *model_info = switch_store::switch_model_info_get();
    const ObjectInfo *object_info = model_info->get_object_info(object_type);
    const AttributeMetadata *attr_md = object_info->get_attr_metadata(attr_id);
    if (attr_md) {
      flags_cache[attr_id] = attr_md->get_flags();
      return flags_cache[attr_id];
    } else {
      return empty_flags;
    }
  } else {
    return flags_cache_it->second;
  }
}

std::unordered_map<switch_object_type_t, std::set<switch_attr_id_t>>
    mandatory_attrs;

const std::set<switch_attr_id_t> &get_mandatory_on_create_attrs(
    const switch_object_type_t object_type) {
  auto mandatory_attrs_it = mandatory_attrs.find(object_type);
  if (mandatory_attrs_it == mandatory_attrs.end()) { /* if not cached */
    ModelInfo *model_info = switch_store::switch_model_info_get();
    mandatory_attrs[object_type];
    const ObjectInfo *object_info = model_info->get_object_info(object_type);
    for (const auto &attr_md : object_info->get_attribute_list()) {
      switch_attr_flags_t flags = attr_md.get_flags();
      if (flags.is_mandatory)
        mandatory_attrs[object_type].insert(attr_md.attr_id);
    }
    return mandatory_attrs[object_type];
  } else {
    return mandatory_attrs_it->second;
  }
}

switch_status_t smi_object_get(const switch_object_type_t object_type,
                               const std::set<attr_w> &attrs,
                               switch_object_id_t &object_handle) {
  return switch_store::object_id_get_wkey(object_type, attrs, object_handle);
}

switch_status_t smi_object_create_attr_check(
    const switch_object_type_t object_type, const std::set<attr_w> &attrs) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  ModelInfo *model_info = switch_store::switch_model_info_get();

  const std::set<switch_attr_id_t> &mandatory_attr_ids =
      get_mandatory_on_create_attrs(object_type);

  for (const auto &attr : attrs) {
    const switch_attr_flags_t &flags = get_flags(object_type, attr.id_get());
    if (flags.is_internal || flags.is_read_only) {
      status = SWITCH_STATUS_INVALID_PARAMETER;
      // switch_log(SWITCH_API_LEVEL_ERROR,
      //            object_type,
      //            "{}:{}: ot={}, create attrs {}",
      //            __func__,
      //            __LINE__,
      //            model_info->get_object_name_from_type(object_type),
      //            attrs);
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 "{}:{}: ot={}, incorrect flags status={} attr {} "
                 "immutable={} internal={} read_only={}",
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 status,
                 attr.getattr(),
                 flags.is_immutable,
                 flags.is_internal,
                 flags.is_read_only);
      return status;
    }
  }

  if (!std::includes(attrs.begin(),
                     attrs.end(),
                     mandatory_attr_ids.begin(),
                     mandatory_attr_ids.end())) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}: ot={}, mandatory attrs missing status={}",
               __func__,
               __LINE__,
               model_info->get_object_name_from_type(object_type),
               status);
    const ObjectInfo *object_info = model_info->get_object_info(object_type);
    for (const auto mand_attr_id : mandatory_attr_ids) {
      const AttributeMetadata *attr_md =
          object_info->get_attr_metadata(mand_attr_id);
      if (attr_md == NULL) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   "{}:{}: missing attr {}",
                   __func__,
                   __LINE__,
                   mand_attr_id);
      } else {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   "{}:{}: missing attr {}",
                   __func__,
                   __LINE__,
                   attr_md->get_attr_name());
      }
    }
    return status;
  }

  return status;
}

switch_status_t smi_object_create(const switch_object_type_t object_type,
                                  const std::set<attr_w> &attrs,
                                  switch_object_id_t &object_handle) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  ModelInfo *model_info = switch_store::switch_model_info_get();

  if (!switch_store::is_object_type_valid(object_type)) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: Unknown object_type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 "{}:{}: create ot={} oid={:#x}",
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 object_handle.data));

  status = smi_object_create_attr_check(object_type, attrs);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status = switch_store::object_create(object_type, attrs, object_handle);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}: status={}\n",
               __func__,
               __LINE__,
               status);
  }
  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              object_type,
                              "{}:{}: status={} oid={:#x}\n",
                              __func__,
                              __LINE__,
                              status,
                              object_handle.data));
  return status;
}

switch_status_t smi_object_delete(const switch_object_id_t object_handle) {
  ModelInfo *model_info = switch_store::switch_model_info_get();
  const auto object_type = switch_store::object_type_query(object_handle);

  if (object_handle.data == 0) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: invalid object_handle = 0",
               __func__,
               __LINE__);
    return SWITCH_STATUS_FAILURE;
  }

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 "{}:{}: ot={} oid={:#x}",
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 object_handle.data));

  const auto status = switch_store::object_delete(object_handle);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}: status={} oid={:#x}\n",
               __func__,
               __LINE__,
               status,
               object_handle.data);
  }
  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              object_type,
                              "{}:{}: status={} oid={:#x}\n",
                              __func__,
                              __LINE__,
                              status,
                              object_handle.data));
  return status;
}

switch_status_t smi_object_create_by_id(const switch_object_type_t object_type,
                                        const std::set<attr_w> &attrs,
                                        const uint64_t id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  ModelInfo *model_info = switch_store::switch_model_info_get();

  if (!switch_store::is_object_type_valid(object_type)) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: Unknown object_type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 "{}:{}: create ot={} id=%" PRIx64,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 id));

  status = smi_object_create_attr_check(object_type, attrs);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status = switch_store::object_create_by_id(object_type, attrs, id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}: create ot={} id=%" PRIx64 " fail, status={}\n",
               __func__,
               __LINE__,
               model_info->get_object_name_from_type(object_type),
               id,
               status);
    return status;
  }
  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 "{}:{}: create ot={} id=%" PRIx64 " success\n",
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 id,
                 status));
  return status;
}

switch_status_t smi_object_delete_by_id(const switch_object_type_t object_type,
                                        const uint64_t id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  ModelInfo *model_info = switch_store::switch_model_info_get();

  if (!switch_store::is_object_type_valid(object_type)) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: Unknown object_type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 "{}:{}: delete ot={} id=%" PRIx64,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 id));

  status = switch_store::object_delete_by_id(object_type, id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}: delete ot={} id=%" PRIx64 " fail, status={}\n",
               __func__,
               __LINE__,
               model_info->get_object_name_from_type(object_type),
               id,
               status);
    return status;
  }
  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 "{}:{}: delete ot={} id=%" PRIx64 " success\n",
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 id));
  return status;
}

bool smi_object_exists_by_id(const switch_object_type_t object_type,
                             const uint64_t id) {
  ModelInfo *model_info = switch_store::switch_model_info_get();
  switch_object_id_t object_id = {SWITCH_NULL_OBJECT_ID};

  object_id = switch_store::object_get_by_id(object_type, id);
  if (object_id.data == SWITCH_NULL_OBJECT_ID) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}: get ot={} id=%" PRIx64 " fail, status={}\n",
               __func__,
               __LINE__,
               model_info->get_object_name_from_type(object_type),
               id);
    return false;
  }

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 "{}:{}: Object exists ot={} id=%" PRIx64 " --> hdl {:#x}  \n",
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 id,
                 object_id));
  return true;
}

switch_status_t smi_attribute_set(const switch_object_id_t object_handle,
                                  const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  ModelInfo *model_info = switch_store::switch_model_info_get();

  if (object_handle.data == 0) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: invalid object_handle = 0",
               __func__,
               __LINE__);
    return SWITCH_STATUS_FAILURE;
  }

  switch_attr_flags_t flags = {};
  const switch_object_type_t object_type =
      switch_store::object_type_query(object_handle);
  flags = get_flags(object_type, attr.id_get());
  if (flags.is_immutable || flags.is_internal || flags.is_read_only ||
      flags.is_create_only) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}: incorrect flags status={} "
               "immutable={} internal={} read_only={} create_only={} for "
               "{}.{}, attr {}",
               __func__,
               __LINE__,
               status,
               flags.is_immutable,
               flags.is_internal,
               flags.is_read_only,
               flags.is_create_only,
               model_info->get_object_name_from_type(object_type),
               switch_store::handle_to_id(object_handle),
               attr.getattr());
    return status;
  }

  return switch_store::attribute_set(object_handle, attr);
}

switch_status_t smi_attribute_get(const switch_object_id_t object_handle,
                                  const switch_attr_id_t attr_id,
                                  attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  ModelInfo *model_info = switch_store::switch_model_info_get();

  if (object_handle.data == 0) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: invalid object_handle = 0 failed",
               __func__,
               __LINE__);
    return SWITCH_STATUS_FAILURE;
  }

  switch_attr_flags_t flags = {};
  const switch_object_type_t object_type =
      switch_store::object_type_query(object_handle);
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info == NULL) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: invalid object handle {} ",
               __func__,
               __LINE__,
               object_handle);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }
  const AttributeMetadata *attr_md = object_info->get_attr_metadata(attr_id);
  flags = get_flags(object_type, attr_id);
  if (flags.is_internal) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}: attr is internal status={} "
               "{}.{}, attr_id {}",
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               switch_store::handle_to_id(object_handle),
               attr_md->get_attr_name());
    return status;
  }

  return switch_store::attribute_get(object_handle, attr_id, attr);
}

switch_status_t smi_object_counters_get(switch_object_id_t object_id,
                                        std::vector<switch_counter_t> &cntrs) {
  return switch_store::object_counters_get(object_id, cntrs);
}

switch_status_t smi_object_counters_clear(
    switch_object_id_t object_id, const std::vector<uint16_t> &cntrs_ids) {
  return switch_store::object_counters_clear(object_id, cntrs_ids);
}

switch_status_t smi_object_counters_clear_all(switch_object_id_t object_id) {
  return switch_store::object_counters_clear_all(object_id);
}

switch_status_t smi_object_flush_all(const switch_object_type_t object_type) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t object_handle = {0};

  std::vector<switch_object_id_t> handles_list;
  status = switch_store::object_get_all_handles(object_type, handles_list);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  for (auto it = handles_list.begin(); it != handles_list.end(); it++) {
    object_handle = *it;
    status = switch_store::object_delete(object_handle);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 "{}:{}: Failed to delete  ot {} oid {:#x} status={}\n",
                 __func__,
                 __LINE__,
                 object_type,
                 object_handle.data,
                 status);
      return status;
    }
  }

  return status;
}

switch_status_t smi_object_get_first_handle(switch_object_type_t object_type,
                                            switch_object_id_t &object_handle) {
  return switch_store::object_get_first_handle(object_type, object_handle);
}

switch_status_t smi_object_get_next_handles(
    switch_object_id_t object_handle,
    uint32_t in_num_handles,
    std::vector<switch_object_id_t> &next_handles,
    uint32_t &out_num_handles) {
  return switch_store::object_get_next_handles(
      object_handle, in_num_handles, next_handles, out_num_handles);
}

switch_status_t smi_object_get_all_handles(
    switch_object_type_t object_type,
    std::vector<switch_object_id_t> &object_handles) {
  return switch_store::object_get_all_handles(object_type, object_handles);
}

switch_status_t smi_object_log_level_set(const switch_object_type_t object_type,
                                         const switch_verbosity_t verbosity) {
  if (object_type == 0)
    set_log_level_all_objects(verbosity);
  else
    set_log_level_object(object_type, verbosity);
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t smi_log_level_set(const switch_verbosity_t verbosity) {
  set_log_level(verbosity);
  return SWITCH_STATUS_SUCCESS;
}

}  // namespace api
}  // namespace smi
