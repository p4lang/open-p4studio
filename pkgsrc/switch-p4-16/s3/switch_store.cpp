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


#include "s3/switch_store.h"

#include <arpa/inet.h>

#include <map>
#include <mutex>  // NOLINT(build/c++11)
#include <set>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <iterator>
#include <cinttypes>
#include <list>
#include <utility>

#include "s3/factory.h"
#ifndef TESTING
#include "s3/bf_rt_backend.h"
#endif
#include "s3/meta/meta.h"
#include "s3/attribute.h"
#include "s3/attribute_util.h"
#include "s3/event.h"
#include "s3/record.h"
#include "./id_gen.h"
#include "./store.h"
#include "./log.h"
#include "./third_party/fmtlib/fmt/format.h"

namespace smi {
namespace switch_store {
using ::smi::logging::switch_log;
using ::smi::logging::logging_init;
#ifndef TESTING
using ::smi::bf_rt::switch_bf_rt_flush;
#endif

#define __NS__ "switch_store"
std::unique_ptr<ModelInfo> model_info = nullptr;
std::vector<std::unique_ptr<std::recursive_mutex>> mutexCache;
std::vector<std::set<attr_w>> defaults_cache;

std::vector<std::vector<create_handler_before_fn>> create_trigger_fns_before;
std::vector<std::vector<delete_handler_before_fn>> delete_trigger_fns_before;
std::vector<std::vector<update_handler_before_fn>> update_trigger_fns_before;
std::vector<std::vector<create_handler_after_fn>> create_trigger_fns_after;
std::vector<std::vector<delete_handler_after_fn>> delete_trigger_fns_after;
std::vector<std::vector<update_handler_after_fn>> update_trigger_fns_after;
std::vector<std::vector<counter_get_handler_fn>> counter_get_trigger_fns;
std::vector<std::vector<counters_set_handler_fn>> counters_set_trigger_fns;
std::vector<std::vector<all_counters_set_handler_fn>>
    all_counters_set_trigger_fns;
std::vector<debug_cli_fn> debug_cli_fns;
std::vector<std::vector<skip_auto_objects_fn>> skip_auto_objects_trigger_fns;

/* TODO: should be db backed and stateless. Just using what we have for now..*/
typedef std::vector<std::unique_ptr<idAllocator>> id_allocator;
id_allocator *allocators = nullptr;

objectGraph *ObjectRefs = nullptr;
secondaryIndex *secondary_index = nullptr;

thread_local std::unordered_set<switch_object_type_t> trigger_context;

ModelInfo *switch_model_info_get() { return model_info.get(); }

static inline bool object_type_valid(switch_object_type_t ot) {
  return ot < model_info->get_objects().size();
}

switch_status_t object_info_init(const char *const object_info_path,
                                 bool warm_init,
                                 const char *const warm_init_file,
                                 bool override_log_level) {
  allocators = new id_allocator();
  smiContext::context();

  switch_status_t status = SWITCH_STATUS_SUCCESS;
  model_info = build_model_info_from_file(object_info_path, false);
  if (model_info == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: BFN SDK Invalid model error: {}",
               __func__,
               __LINE__,
               SWITCH_STATUS_FAILURE);
    return SWITCH_STATUS_FAILURE;
  }
  auto objects = model_info->get_objects();
  auto object_count = objects.size();
  ObjectRefs = new objectGraph(object_count);
  secondary_index = new secondaryIndex(object_count);

  for (auto it = model_info->begin(); it != model_info->end(); it++) {
    mutexCache.push_back(
        std::unique_ptr<std::recursive_mutex>(new std::recursive_mutex));
  }

  for (auto it = model_info->begin(); it != model_info->end(); it++) {
    allocators->push_back(std::unique_ptr<idAllocator>(new idAllocator(500)));
  }

  defaults_cache.resize(object_count);
  for (auto it = model_info->begin(); it != model_info->end(); it++) {
    auto object_info = *it;

    if (object_info.object_type >= defaults_cache.size()) {
      return SWITCH_STATUS_FAILURE;
    }
    for (const auto &attr_md : object_info.get_attribute_list()) {
      const ValueMetadata *value_md = attr_md.get_value_metadata();
      const switch_attribute_t attribute = {
          .id = attr_md.attr_id, .value = value_md->get_default_value()};
      attr_w attr(0);
      status = attr.attr_import(attribute);
      if (status != SWITCH_STATUS_SUCCESS) return status;

      defaults_cache[object_info.object_type].insert(attr);
    }
  }

  create_trigger_fns_before.resize(object_count);
  delete_trigger_fns_before.resize(object_count);
  update_trigger_fns_before.resize(object_count);
  create_trigger_fns_after.resize(object_count);
  delete_trigger_fns_after.resize(object_count);
  update_trigger_fns_after.resize(object_count);
  counter_get_trigger_fns.resize(object_count);
  counters_set_trigger_fns.resize(object_count);
  all_counters_set_trigger_fns.resize(object_count);
  debug_cli_fns.resize(object_count);
  skip_auto_objects_trigger_fns.resize(object_count);

#ifdef TESTING
  status = logging_init(SWITCH_API_LEVEL_DEBUG, override_log_level);
#else
  status = logging_init(SWITCH_API_LEVEL_INFO, override_log_level);
#endif
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: BFN SDK logging init failure error: {}",
               __func__,
               __LINE__,
               status);
  }

  status = factory_init();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: BFN SDK factory init failure error: {}",
               __func__,
               __LINE__,
               status);
  }

  status = db::db_load(warm_init, warm_init_file);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: BFN SDK DB load failure error: {}",
               __func__,
               __LINE__,
               status);
  }

  return status;
}

/*
 * Dump state to file
 */
switch_status_t object_info_dump(const char *dump_file) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (!dump_file) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}: dump_file is NULL",
               __func__);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  status = object_backup_stats_cache();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}: Failed to backup stats cache",
               __func__);
    return SWITCH_STATUS_FAILURE;
  }

  status = db::db_dump(dump_file);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}: Failed to dump state",
               __func__);
    return SWITCH_STATUS_FAILURE;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t object_info_clean() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  factory_clean();

  mutexCache.clear();

  for (auto it = model_info->begin(); it != model_info->end(); it++) {
    auto object_info = *it;
    defaults_cache[object_info.object_type].clear();
  }
  defaults_cache.clear();

  create_trigger_fns_before.clear();
  delete_trigger_fns_before.clear();
  update_trigger_fns_before.clear();
  create_trigger_fns_after.clear();
  delete_trigger_fns_after.clear();
  update_trigger_fns_after.clear();
  counter_get_trigger_fns.clear();
  counters_set_trigger_fns.clear();
  all_counters_set_trigger_fns.clear();
  debug_cli_fns.clear();
  skip_auto_objects_trigger_fns.clear();

  allocators->clear();

  ObjectRefs->clear();

  secondary_index->clear();

  db::db_clear();

  model_info.reset();

  return status;
}
#define LOCK_GUARD(ot, __fn)                 \
  if (ot == 0) {                             \
    switch_log(SWITCH_API_LEVEL_ERROR,       \
               SWITCH_OT_NONE,               \
               "{}.{}: Invalid object type", \
               __func__,                     \
               __LINE__);                    \
    return SWITCH_STATUS_INVALID_PARAMETER;  \
  }                                          \
  switch_store::switch_store_lock();         \
  auto ret_val = __fn;                       \
  switch_store::switch_store_unlock();       \
  return ret_val

void switch_store_lock(void) { db::switch_store_lock(); }
void switch_store_unlock(void) { db::switch_store_unlock(); }

int object_try_lock(const switch_object_id_t oid) {
  return db::object_lock(oid);
}

void object_unlock(const switch_object_id_t oid) { db::object_unlock(oid); }

bool object_exists(const switch_object_id_t oid) {
  return db::object_exists(oid);
}

// public api
switch_status_t object_ready_for_delete(const switch_object_id_t oid,
                                        bool &del) {
  if (!db::object_exists(oid)) return SWITCH_STATUS_ITEM_NOT_FOUND;

  auto const &type_map_iter = ObjectRefs->find(oid);
  if (type_map_iter == ObjectRefs->end(oid)) {
    del = true;  // not referenced by anything
    return SWITCH_STATUS_SUCCESS;
  }

  // check if this object is referenced by any other user object
  for (const auto &type : type_map_iter->second) {
    if (type.second.size() != 0) {
      const ObjectInfo *object_info = model_info->get_object_info(type.first);
      if (object_info != nullptr &&
          object_info->get_object_class() == OBJECT_CLASS_USER) {
        del = false;
        return SWITCH_STATUS_SUCCESS;
      }
    }
  }
  del = true;
  return SWITCH_STATUS_SUCCESS;
}
/* The below function checks for references to object with oid by other user
 * objects.
 * Auto object references need not be checked as they get deleted recursively.
 * However, recursive deleted is not
 * permitted for use objects. User objects get deleted with explicit object
 * delete calls by the User */
switch_status_t object_ref_validate_before_delete(switch_object_id_t oid) {
  CHECK_RET(oid.data == SWITCH_NULL_OBJECT_ID, SWITCH_STATUS_FAILURE);
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  auto const &type_map_iter = ObjectRefs->find(oid);
  if (type_map_iter == ObjectRefs->end(oid))
    return SWITCH_STATUS_SUCCESS;  // not referenced by anything

  for (const auto &type : type_map_iter->second) {
    if (type.second.size() != 0) {
      const ObjectInfo *object_info = model_info->get_object_info(type.first);
      if (object_info != nullptr &&
          object_info->get_object_class() == OBJECT_CLASS_USER) {
        auto const src = type.second.begin()->oid;
        auto internal_obj_attr_id =
            object_info->get_attr_id_from_name("internal_object");
        bool internal = false;
        switch_store::v_get(src, internal_obj_attr_id, internal);
        if (!internal) {
          status = SWITCH_STATUS_RESOURCE_IN_USE;
          switch_log(
              SWITCH_API_LEVEL_ERROR,
              object_type_query(oid),
              SMI_DELETE_OPERATION,
              "{}.{}:{} References exist, from {}.{} to {}.{}, failed to "
              "delete",
              __NS__,
              __func__,
              __LINE__,
              model_info->get_object_name_from_type(object_type_query(src)),
              switch_store::handle_to_id(src),
              model_info->get_object_name_from_type(object_type_query(oid)),
              switch_store::handle_to_id(oid));
        } else {
          switch_log(
              SWITCH_API_LEVEL_DETAIL,
              object_type_query(oid),
              SMI_DELETE_OPERATION,
              "{}.{}:{} Reference exist, from internal object {}.{} to {}.{}, "
              "reference validation for this object will be skipped since it "
              "is internal",
              __NS__,
              __func__,
              __LINE__,
              model_info->get_object_name_from_type(object_type_query(src)),
              switch_store::handle_to_id(src),
              model_info->get_object_name_from_type(object_type_query(oid)),
              switch_store::handle_to_id(oid));
        }
      }
    }
  }

  return status;
}

switch_status_t object_ref_add(switch_object_id_t src,
                               switch_object_id_t dst,
                               switch_attr_id_t attr_id) {
  if (dst == 0) return SWITCH_STATUS_SUCCESS;

  bool ret = ObjectRefs->insert(dst, object_type_query(src), src, attr_id);
  if (!ret) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type_query(src),
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Reference exists from {}.{} to {}.{}",
               __NS__,
               __func__,
               __LINE__,
               model_info->get_object_name_from_type(object_type_query(src)),
               handle_to_id(src),
               model_info->get_object_name_from_type(object_type_query(dst)),
               handle_to_id(dst));
    return SWITCH_STATUS_FAILURE;
  }

  SWITCH_DETAIL_LOG(
      switch_log(SWITCH_API_LEVEL_DETAIL,
                 object_type_query(src),
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: added reference about {}.{} to {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type_query(src)),
                 handle_to_id(src),
                 model_info->get_object_name_from_type(object_type_query(dst)),
                 handle_to_id(dst)));

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t object_ref_remove(switch_object_id_t src,
                                  switch_object_id_t dst) {
  if (dst == 0) return SWITCH_STATUS_SUCCESS;

  uint32_t num_erased = ObjectRefs->erase(dst, object_type_query(src), src);

  if (!num_erased) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type_query(src),
               SMI_DELETE_OPERATION,
               "object_ref_remove src object doesn't exist! deleted {} items",
               num_erased);
    return SWITCH_STATUS_FAILURE;
  }
  SWITCH_DETAIL_LOG(
      switch_log(SWITCH_API_LEVEL_DETAIL,
                 object_type_query(src),
                 SMI_DELETE_OPERATION,
                 "object_ref_remove removed reference from {}.{} to {}.{}",
                 model_info->get_object_name_from_type(object_type_query(src)),
                 switch_store::handle_to_id(src),
                 model_info->get_object_name_from_type(object_type_query(dst)),
                 switch_store::handle_to_id(dst)));

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t object_ref_remove_by_attr_id(switch_object_id_t src,
                                             switch_object_id_t dst,
                                             switch_attr_id_t attr_id) {
  if (dst == 0) return SWITCH_STATUS_SUCCESS;

  uint32_t num_erased =
      ObjectRefs->erase(dst, object_type_query(src), src, attr_id);

  if (num_erased != 1) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type_query(src),
               SMI_DELETE_OPERATION,
               "object_ref_remove_by_attr_id src object doesn't exist! deleted "
               "{} items",
               num_erased);
    return SWITCH_STATUS_FAILURE;
  }
  SWITCH_DETAIL_LOG(switch_log(
      SWITCH_API_LEVEL_DETAIL,
      object_type_query(src),
      SMI_DELETE_OPERATION,
      "object_ref_remove removed reference from {}.{}:attr_id({}) to {}.{}",
      model_info->get_object_name_from_type(object_type_query(src)),
      switch_store::handle_to_id(src),
      attr_id,
      model_info->get_object_name_from_type(object_type_query(dst)),
      switch_store::handle_to_id(dst)));

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t object_refs_get(std::set<switch_object_id_t> &oids,
                                const switch_object_id_t object_id,
                                bool internal_references) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const ObjectInfo *object_info =
      model_info->get_object_info(object_type_query(object_id));
  for (const auto &attr_md : object_info->get_attribute_list()) {
    switch_object_id_t oid = {0};
    switch (attr_md.type) {
      case SWITCH_TYPE_OBJECT_ID:
        if (!((attr_md.get_flags()).is_internal ||
              (attr_md.get_flags()).is_read_only) ||
            internal_references) {
          status = switch_store::v_get(object_id, attr_md.attr_id, oid);
          if (status != SWITCH_STATUS_SUCCESS) return status;
          oids.insert(oid);
        }
        break;

      case SWITCH_TYPE_LIST: {
        if (!((attr_md.get_flags()).is_internal ||
              (attr_md.get_flags()).is_read_only) ||
            internal_references) {
          const ValueMetadata *value_md = attr_md.get_value_metadata();
          if (value_md->type != SWITCH_TYPE_OBJECT_ID) break;
          size_t cnt = 0;
          status = switch_store::list_len(object_id, attr_md.attr_id, cnt);
          if (status != SWITCH_STATUS_SUCCESS) return status;

          for (uint32_t i = 0; i < cnt; i++) {
            status =
                switch_store::list_v_get(object_id, attr_md.attr_id, i, oid);
            if (status != SWITCH_STATUS_SUCCESS) return status;

            oids.insert(oid);
          }
        }
        break;
      }
      default:
        CHECK_RET(is_composite_type(attr_md.type), SWITCH_STATUS_FAILURE);
        break;
    }
  }
  return status;
}

switch_status_t oid_create(switch_object_type_t object_type,
                           switch_object_id_t &object_id_out,
                           bool set) {
  if (object_type == 0) return SWITCH_STATUS_INVALID_PARAMETER;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  idAllocator *allocator = nullptr;

  if (object_type >= allocators->size()) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Invalid object type {} for allocator"
               "status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  allocator = allocators->operator[](object_type).get();

  if (set) {
    uint32_t id = switch_store::handle_to_id(object_id_out);
    status = allocator->reserve(id);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "oid_create: ID reserve fail for OID: {}",
                 object_id_out);
      return SWITCH_STATUS_FAILURE;
    }
  } else {
    uint32_t id = allocator->allocate();
    if (id == 0) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "oid_create: ID Allocation fail for object_type: {}",
                 object_type);
      return SWITCH_STATUS_FAILURE;
    }
    object_id_out = switch_store::id_to_handle(object_type, id);
  }
  return status;
}

switch_status_t oid_free(switch_object_id_t oid) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_type_t object_type = switch_store::object_type_query(oid);
  idAllocator *allocator = nullptr;

  if (object_type >= allocators->size()) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Invalid object type {} for allocator"
               "status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  allocator = allocators->operator[](object_type).get();

  const uint32_t id = switch_store::handle_to_id(oid);
  status = allocator->release(id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "oid_free: ID release fail for id: {}",
               id);
  }
  return status;
}

switch_status_t oid_get_first_handle(switch_object_type_t object_type,
                                     switch_object_id_t &object_handle) {
  if (object_type == 0) return SWITCH_STATUS_INVALID_PARAMETER;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  idAllocator *allocator = nullptr;

  if (object_type >= allocators->size()) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Invalid object type {} for allocator"
               "status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  allocator = allocators->operator[](object_type).get();

  uint32_t id = allocator->get_first();
  if (id == 0) return SWITCH_STATUS_ITEM_NOT_FOUND;
  object_handle = switch_store::id_to_handle(object_type, id);
  return status;
}

switch_status_t oid_get_next_handles(
    switch_object_id_t object_handle,
    uint32_t num_handles,
    std::vector<switch_object_id_t> &next_handles,
    uint32_t &out_count) {
  idAllocator *allocator = nullptr;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_type_t object_type =
      switch_store::object_type_query(object_handle);
  uint32_t curr_switch_id = switch_store::handle_to_id(object_handle);
  if (curr_switch_id == 0) return SWITCH_STATUS_INVALID_PARAMETER;

  if (object_type >= allocators->size()) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Invalid object type {} for allocator"
               "status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  allocator = allocators->operator[](object_type).get();

  std::vector<uint32_t> next_n_handles;
  allocator->get_next_n(next_n_handles, curr_switch_id, num_handles);

  for (auto id : next_n_handles) {
    object_handle = switch_store::id_to_handle(object_type, id);
    next_handles.push_back(object_handle);
  }
  out_count = next_handles.size();

  return status;
}

switch_status_t oid_get_all_handles(
    switch_object_type_t object_type,
    std::vector<switch_object_id_t> &object_handles) {
  idAllocator *allocator = nullptr;
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (object_type >= allocators->size()) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Invalid object type {} for allocator"
               "status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  allocator = allocators->operator[](object_type).get();

  std::vector<uint32_t> ids;
  allocator->get_all_in_use(ids);

  for (auto id : ids) {
    auto object_handle = switch_store::id_to_handle(object_type, id);
    object_handles.push_back(object_handle);
  }

  return status;
}

// const std::unordered_map<switch_object_id_t, std::set<switch_attr_id_t>> &
const std::vector<object_and_attribute_t> &get_object_references(
    const switch_object_id_t dst, const switch_object_type_t type) {
  return ObjectRefs->getRefs(dst, type);
}

switch_status_t referencing_set_get(const switch_object_id_t dst,
                                    const switch_object_type_t type,
                                    std::set<switch_object_id_t> &ref_set) {
  for (const auto &reference : ObjectRefs->getRefs(dst, type)) {
    ref_set.insert(reference.oid);
  }
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_create_trigs_after(const switch_object_type_t object_type,
                                       const create_handler_after_fn fn) {
  if (object_type < create_trigger_fns_after.size()) {
    create_trigger_fns_after[object_type].push_back(fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register after create trigger. Unknown object "
               "type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_create_trigs_before(const switch_object_type_t object_type,
                                        const create_handler_before_fn fn) {
  auto it = create_trigger_fns_before[object_type].begin();
  if (object_type < create_trigger_fns_before.size()) {
    create_trigger_fns_before[object_type].insert(it, fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register before create trigger. Unknown "
               "object type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_delete_trigs_after(const switch_object_type_t object_type,
                                       const delete_handler_after_fn fn) {
  if (object_type < delete_trigger_fns_after.size()) {
    delete_trigger_fns_after[object_type].push_back(fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register after delete trigger. Unknown object "
               "type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_delete_trigs_before(const switch_object_type_t object_type,
                                        const delete_handler_before_fn fn) {
  auto it = delete_trigger_fns_before[object_type].begin();
  if (object_type < delete_trigger_fns_before.size()) {
    delete_trigger_fns_before[object_type].insert(it, fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register before delete trigger. Unknown "
               "object type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_update_trigs_after(const switch_object_type_t object_type,
                                       const update_handler_after_fn fn) {
  if (object_type < update_trigger_fns_after.size()) {
    update_trigger_fns_after[object_type].push_back(fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register after update trigger. Unknown object "
               "type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_update_trigs_before(const switch_object_type_t object_type,
                                        const update_handler_before_fn fn) {
  auto it = update_trigger_fns_before[object_type].begin();
  if (object_type < update_trigger_fns_before.size()) {
    update_trigger_fns_before[object_type].insert(it, fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register before update trigger. Unknown "
               "object type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_counter_get_trigs(const switch_object_type_t object_type,
                                      const counter_get_handler_fn fn) {
  if (object_type < counter_get_trigger_fns.size()) {
    counter_get_trigger_fns[object_type].push_back(fn);
  } else {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OT_NONE,
        "{}:{}: Failed to register counter get trigger. Unknown object type {}",
        __func__,
        __LINE__,
        object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_counters_set_trigs(const switch_object_type_t object_type,
                                       const counters_set_handler_fn fn) {
  if (object_type < counters_set_trigger_fns.size()) {
    counters_set_trigger_fns[object_type].push_back(fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register counters set trigger. Unknown object "
               "type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_all_counters_set_trigs(
    const switch_object_type_t object_type,
    const all_counters_set_handler_fn fn) {
  if (object_type < all_counters_set_trigger_fns.size()) {
    all_counters_set_trigger_fns[object_type].push_back(fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register all counters set trigger. Unknown "
               "object type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t reg_debug_cli_callback(const switch_object_type_t object_type,
                                       const debug_cli_fn fn) {
  if (object_type < debug_cli_fns.size()) {
    debug_cli_fns[object_type] = fn;
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register debug cli callback for "
               "object type {}",
               __func__,
               __LINE__,
               object_type);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }
  return SWITCH_STATUS_SUCCESS;
}

std::string object_pkt_path_counter_print(const switch_object_id_t oid) {
  switch_object_type_t ot = object_type_query(oid);
  std::string ret = "Invalid object handle or type";
  if (ot < debug_cli_fns.size()) {
    auto fn = debug_cli_fns[ot];
    if (fn) ret = (fn)(oid);
  }
  return ret;
}

switch_status_t reg_skip_auto_object_trigs(
    const switch_object_type_t object_type, const skip_auto_objects_fn fn) {
  if (object_type < skip_auto_objects_trigger_fns.size()) {
    skip_auto_objects_trigger_fns[object_type].push_back(fn);
  } else {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: Failed to register skip auto objects trigger. Unknown "
               "object type {}",
               __func__,
               __LINE__,
               object_name_get_from_type(object_type));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  return SWITCH_STATUS_SUCCESS;
}

bool check_skip_auto_objects_for_object(const switch_object_id_t object_id,
                                        const switch_object_type_t auto_ot) {
  const switch_object_type_t object_type = object_type_query(object_id);
  bool skip = false;

  if (object_type == 0) return false;

  for (auto fn : skip_auto_objects_trigger_fns[object_type]) {
    skip = (fn)(object_id, auto_ot);
    if (skip) {
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 "{}.{}:{}: Skipping auto object {} for parent type {} hdl {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(auto_ot),
                 model_info->get_object_name_from_type(object_type),
                 object_id);
      return true;
    }
  }
  return false;
}

/*
 * Helper functions for base types:
 * vget, vset
 *
 */
template <typename T>
switch_status_t v_get(const switch_object_id_t object_id,
                      const switch_attr_id_t attr_id,
                      T &val) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  attr_w attr(attr_id);
  status = attribute_get(object_id, attr_id, attr);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status = attr.v_get(val);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  return SWITCH_STATUS_SUCCESS;
}

template switch_status_t v_get<switch_enum_t>(const switch_object_id_t,
                                              const switch_attr_id_t,
                                              switch_enum_t &val);
template switch_status_t v_get<uint8_t>(const switch_object_id_t,
                                        const switch_attr_id_t,
                                        uint8_t &val);
template switch_status_t v_get<uint16_t>(const switch_object_id_t,
                                         const switch_attr_id_t,
                                         uint16_t &val);
template switch_status_t v_get<uint32_t>(const switch_object_id_t,
                                         const switch_attr_id_t,
                                         uint32_t &val);
template switch_status_t v_get<uint64_t>(const switch_object_id_t,
                                         const switch_attr_id_t,
                                         uint64_t &val);
template switch_status_t v_get<int64_t>(const switch_object_id_t,
                                        const switch_attr_id_t,
                                        int64_t &val);
template switch_status_t v_get<switch_object_id_t>(const switch_object_id_t,
                                                   const switch_attr_id_t,
                                                   switch_object_id_t &val);
template switch_status_t v_get<bool>(const switch_object_id_t,
                                     const switch_attr_id_t,
                                     bool &val);
template switch_status_t v_get<switch_mac_addr_t>(const switch_object_id_t,
                                                  const switch_attr_id_t,
                                                  switch_mac_addr_t &val);
template switch_status_t v_get<switch_string_t>(const switch_object_id_t,
                                                const switch_attr_id_t,
                                                switch_string_t &val);
template switch_status_t v_get<switch_ip_address_t>(const switch_object_id_t,
                                                    const switch_attr_id_t,
                                                    switch_ip_address_t &val);
template switch_status_t v_get<switch_ip_prefix_t>(const switch_object_id_t,
                                                   const switch_attr_id_t,
                                                   switch_ip_prefix_t &val);
template switch_status_t v_get<switch_range_t>(const switch_object_id_t,
                                               const switch_attr_id_t,
                                               switch_range_t &val);

// Support vectors for each of the above types, except for enums,
// since the data store does not support lists of enums
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<bool> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<uint8_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<uint16_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<uint32_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<uint64_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<int64_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_object_id_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_mac_addr_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_string_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_ip_address_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_ip_prefix_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_range_t> &val);
template switch_status_t v_get(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_enum_t> &val);

template <typename T>
switch_status_t v_set(const switch_object_id_t object_id,
                      const switch_attr_id_t attr_id,
                      const T val) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  attr_w attr(attr_id, val);

  status = attribute_set(object_id, attr);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  return SWITCH_STATUS_SUCCESS;
}

template switch_status_t v_set<switch_object_id_t>(const switch_object_id_t,
                                                   const switch_attr_id_t,
                                                   const switch_object_id_t);
template switch_status_t v_set<uint64_t>(const switch_object_id_t,
                                         const switch_attr_id_t,
                                         const uint64_t);
template switch_status_t v_set<int64_t>(const switch_object_id_t,
                                        const switch_attr_id_t,
                                        const int64_t);
template switch_status_t v_set<uint32_t>(const switch_object_id_t,
                                         const switch_attr_id_t,
                                         const uint32_t);
template switch_status_t v_set<uint16_t>(const switch_object_id_t,
                                         const switch_attr_id_t,
                                         const uint16_t);
template switch_status_t v_set<uint8_t>(const switch_object_id_t,
                                        const switch_attr_id_t,
                                        const uint8_t);
template switch_status_t v_set<bool>(const switch_object_id_t,
                                     const switch_attr_id_t,
                                     const bool);
template switch_status_t v_set<switch_enum_t>(const switch_object_id_t,
                                              const switch_attr_id_t,
                                              const switch_enum_t);
template switch_status_t v_set<switch_mac_addr_t>(const switch_object_id_t,
                                                  const switch_attr_id_t,
                                                  const switch_mac_addr_t);
template switch_status_t v_set<switch_ip_address_t>(const switch_object_id_t,
                                                    const switch_attr_id_t,
                                                    const switch_ip_address_t);
template switch_status_t v_set<switch_ip_prefix_t>(const switch_object_id_t,
                                                   const switch_attr_id_t,
                                                   const switch_ip_prefix_t);
template switch_status_t v_set<switch_range_t>(const switch_object_id_t,
                                               const switch_attr_id_t,
                                               const switch_range_t);

// Support vectors for each of the above types, except for enums,
// since the data store does not support lists of enums
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_object_id_t>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<uint64_t>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<int64_t>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<uint32_t>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<uint16_t>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<uint8_t>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<bool>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_ip_address_t>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_ip_prefix_t>);
template switch_status_t v_set(const switch_object_id_t,
                               const switch_attr_id_t,
                               std::vector<switch_range_t>);

switch_status_t list_v_del(const switch_object_id_t object_id,
                           const switch_attr_id_t mbr_list_attr_id,
                           const switch_object_id_t value_del) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  attr_w old_list(mbr_list_attr_id);
  attr_w new_list(mbr_list_attr_id);
  std::vector<switch_object_id_t> old_oid_list, new_oid_list;

  status |= switch_store::attribute_get(object_id, mbr_list_attr_id, old_list);

  old_list.v_get(old_oid_list);
  for (auto oid : old_oid_list) {
    if (oid.data != value_del.data) new_oid_list.push_back(oid);
  }
  new_list.v_set(new_oid_list);
  status |= switch_store::attribute_set(object_id, new_list);

  return status;
}

/**
 * The store gets updated the following way.
 * object_id[hash(attr_id + index)] -> list_value
 * The downside to this is we can't arbitrarily remove items from the list.
 * We can only append to the list and pop at the tail of the list.
 * The head of the list holds the current count
 */
switch_status_t list_v_push(const switch_object_id_t object_id,
                            const switch_attr_id_t mbr_list_attr_id,
                            const switch_object_id_t oid) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  attr_w old_list(mbr_list_attr_id);
  attr_w new_list(mbr_list_attr_id);
  std::vector<switch_object_id_t> oid_list;

  status |= switch_store::attribute_get(object_id, mbr_list_attr_id, old_list);

  old_list.v_get(oid_list);
  oid_list.push_back(oid);
  new_list.v_set(oid_list);
  status |= switch_store::attribute_set(object_id, new_list);

  return status;
}

template <typename T>
switch_status_t list_push(const switch_object_id_t object_id,
                          const switch_attr_id_t attr_id,
                          const std::vector<T> list) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_attribute_value_t value_in = {};
  uint64_t index = 0;
  const ObjectInfo *object_info =
      model_info->get_object_info(object_type_query(object_id));
  const AttributeMetadata *attr_md = object_info->get_attr_metadata(attr_id);
  CHECK_RET(attr_md == NULL, SWITCH_STATUS_FAILURE);
  CHECK_RET(attr_md->type != SWITCH_TYPE_LIST, SWITCH_STATUS_FAILURE);
  const ValueMetadata *value_md = attr_md->get_value_metadata();

  // store index 0 with new count
  memset(&value_in, 0, sizeof(switch_attribute_value_t));
  value_in.type = SWITCH_TYPE_LIST;
  value_in.list.count = list.size();
  value_in.list.list_type = value_md->type;
  status = db::value_set(object_id, attr_id, index, value_in);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  index++;
  for (auto val : list) {
    memset(&value_in, 0, sizeof(switch_attribute_value_t));
    attr_util::v_set(value_in, val);
    if (value_md->type == SWITCH_TYPE_OBJECT_ID) {
      if (value_in.oid != 0 &&
          !value_md->is_allowed_object_type(object_type_query(value_in.oid))) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}:{}: Invalid object type, given={}",
                   __func__,
                   __LINE__,
                   model_info->get_object_name_from_type(
                       object_type_query(value_in.oid)));
        continue;
      }
    }
    status = db::value_set(object_id, attr_id, index, value_in);
    if (status != SWITCH_STATUS_SUCCESS) return status;
    index++;

    // ref tracking
    if (value_md->type == SWITCH_TYPE_OBJECT_ID &&
        !((attr_md->get_flags()).is_internal ||
          (attr_md->get_flags()).is_read_only)) {
      status = object_ref_add(object_id, value_in.oid, attr_id);
      if (status != SWITCH_STATUS_SUCCESS) return status;
    }
  }
  return status;
}

template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<bool> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<uint8_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<uint16_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<uint32_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<uint64_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<int64_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<switch_object_id_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<switch_string_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<switch_ip_address_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<switch_ip_prefix_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<switch_mac_addr_t> list);
template switch_status_t list_push(const switch_object_id_t object_id,
                                   const switch_attr_id_t attr_id,
                                   const std::vector<switch_enum_t> list);

switch_status_t list_clear(const switch_object_id_t object_id,
                           const switch_attr_id_t attr_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const ObjectInfo *object_info =
      model_info->get_object_info(object_type_query(object_id));
  const AttributeMetadata *attr_md = object_info->get_attr_metadata(attr_id);
  CHECK_RET(attr_md == NULL, SWITCH_STATUS_FAILURE);
  CHECK_RET(attr_md->type != SWITCH_TYPE_LIST, SWITCH_STATUS_FAILURE);
  const ValueMetadata *value_md = attr_md->get_value_metadata();

  // index 0 has count of elements
  switch_attribute_value_t list_value = {};
  status = db::value_get(object_id, attr_id, 0, list_value);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  if (list_value.list.count == 0) {
    return SWITCH_STATUS_ITEM_NOT_FOUND;
  }

  while (list_value.list.count > 0) {
    switch_attribute_value_t value_out = {};
    status =
        db::value_get(object_id, attr_id, list_value.list.count, value_out);
    if (status != SWITCH_STATUS_SUCCESS) return status;
    status = db::value_delete(object_id, attr_id, list_value.list.count);
    if (status != SWITCH_STATUS_SUCCESS) return status;

    list_value.list.count--;
    // ref tracking
    if (value_md->type == SWITCH_TYPE_OBJECT_ID &&
        !((attr_md->get_flags()).is_internal ||
          (attr_md->get_flags()).is_read_only)) {
      status = object_ref_remove_by_attr_id(object_id, value_out.oid, attr_id);
      if (status != SWITCH_STATUS_SUCCESS) return status;
    }
  }
  // store index 0 with count 0
  status = db::value_set(object_id, attr_id, 0, list_value);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  return status;
}

template <>
switch_status_t list_v_get(const switch_object_id_t object_id,
                           const switch_attr_id_t attr_id,
                           const size_t index,
                           switch_attribute_value_t &value_out) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  // index 0 has count of elements
  switch_attribute_value_t value = {};
  status = db::value_get(object_id, attr_id, 0, value);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  // index in range?
  CHECK_RET(index >= value.list.count, SWITCH_STATUS_FAILURE);

  // get value from db
  status = db::value_get(object_id, attr_id, index + 1, value_out);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  return status;
}

template <typename T>
switch_status_t list_v_get(const switch_object_id_t object_id,
                           const switch_attr_id_t attr_id,
                           const size_t index,
                           T &value_out) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_attribute_t attr = {};

  static_assert(std::is_same<T, switch_attribute_value_t &>::value == 0,
                "unexpected implicit instantiation..");

  status = list_v_get(object_id, attr_id, index, attr.value);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status = attr_util::v_get(attr.value, value_out);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  return status;
}

template switch_status_t list_v_get<uint64_t>(
    const switch_object_id_t object_id,
    const switch_attr_id_t attr_id,
    const size_t index,
    uint64_t &value_out);

template switch_status_t list_v_get<switch_object_id_t>(
    const switch_object_id_t object_id,
    const switch_attr_id_t attr_id,
    const size_t index,
    switch_object_id_t &value_out);

template switch_status_t list_v_get<bool>(const switch_object_id_t object_id,
                                          const switch_attr_id_t attr_id,
                                          const size_t index,
                                          bool &value_out);

switch_status_t list_len(const switch_object_id_t object_id,
                         const switch_attr_id_t attr_id,
                         size_t &len_out) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  // index 0 has count of elements
  switch_attribute_value_t value = {};
  status = db::value_get(object_id, attr_id, 0, value);
  if (status == SWITCH_STATUS_SUCCESS) {
    len_out = value.list.count;
  } else if (status == SWITCH_STATUS_ITEM_NOT_FOUND) {
    len_out = 0;
    status = SWITCH_STATUS_SUCCESS;
  }

  return status;
}

static inline void attr_to_buffer(
    const attr_w &mattr,
    const switch_attr_id_t attr_id,
    std::back_insert_iterator<fmt::memory_buffer> &str) {
  const switch_attribute_t &attr = mattr.getattr();

  switch (mattr.type_get()) {
    case SWITCH_TYPE_BOOL:
      fmt::format_to(str, "{}.{}:", attr_id, attr.value.booldata);
      return;
    case SWITCH_TYPE_UINT8:
      fmt::format_to(str, "{}.{}:", attr_id, attr.value.u8);
      return;
    case SWITCH_TYPE_UINT16:
      fmt::format_to(str, "{}.{}:", attr_id, attr.value.u16);
      return;
    case SWITCH_TYPE_UINT32:
      fmt::format_to(str, "{}.{}:", attr_id, attr.value.u32);
      return;
    case SWITCH_TYPE_UINT64:
      fmt::format_to(str, "{}.{}:", attr_id, attr.value.u64);
      return;
    case SWITCH_TYPE_INT64:
      fmt::format_to(str, "{}.{}:", attr_id, attr.value.s64);
      return;
    case SWITCH_TYPE_ENUM:
      fmt::format_to(str, "{}.{}:", attr_id, attr.value.enumdata.enumdata);
      return;
    case SWITCH_TYPE_OBJECT_ID:
      fmt::format_to(str,
                     "{}.{}:",
                     attr.value.oid.data >> 48,
                     attr.value.oid.data & 0xFFFFFFFFUL);
      return;
    case SWITCH_TYPE_STRING:
      fmt::format_to(str, "{}.{}:", mattr.id_get(), attr.value.text.text);
      return;
    case SWITCH_TYPE_RANGE:
      fmt::format_to(str,
                     "{}.{}-{}:",
                     attr_id,
                     attr.value.range.min,
                     attr.value.range.max);
      return;
    case SWITCH_TYPE_MAC:
      fmt::format_to(str,
                     "{:02x}{:02x}.{:02x}{:02x}.{:02x}{:02x}:",
                     attr.value.mac.mac[0],
                     attr.value.mac.mac[1],
                     attr.value.mac.mac[2],
                     attr.value.mac.mac[3],
                     attr.value.mac.mac[4],
                     attr.value.mac.mac[5]);
      return;
    case SWITCH_TYPE_IP_ADDRESS:
      if (attr.value.ipaddr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV4) {
        fmt::format_to(str, "{}:", attr.value.ipaddr.ip4);
      } else if (attr.value.ipaddr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV6) {
        const unsigned char *a = attr.value.ipaddr.ip6;
        fmt::format_to(str,
                       "{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:",
                       (a[0] << 8) + a[1],
                       (a[2] << 8) + a[3],
                       (a[4] << 8) + a[5],
                       (a[6] << 8) + a[7],
                       (a[8] << 8) + a[9],
                       (a[10] << 8) + a[11],
                       (a[12] << 8) + a[13],
                       (a[14] << 8) + a[15]);
      } else {
        return;
      }
      return;
    case SWITCH_TYPE_IP_PREFIX:
      if (attr.value.ipprefix.addr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV4) {
        fmt::format_to(str,
                       "{}/{}:",
                       attr.value.ipprefix.addr.ip4,
                       attr.value.ipprefix.len);
      } else if (attr.value.ipprefix.addr.addr_family ==
                 SWITCH_IP_ADDR_FAMILY_IPV6) {
        const unsigned char *a = attr.value.ipprefix.addr.ip6;
        fmt::format_to(str,
                       "{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}/{}:",
                       (a[0] << 8) + a[1],
                       (a[2] << 8) + a[3],
                       (a[4] << 8) + a[5],
                       (a[6] << 8) + a[7],
                       (a[8] << 8) + a[9],
                       (a[10] << 8) + a[11],
                       (a[12] << 8) + a[13],
                       (a[14] << 8) + a[15],
                       attr.value.ipprefix.len);
      } else {
        return;
      }
      return;
    case SWITCH_TYPE_LIST: {
      std::string list_string;
      mattr.attr_to_string(list_string);
      switch_log(SWITCH_API_LEVEL_DETAIL,
                 SWITCH_OT_NONE,
                 "List format string: {}",
                 list_string);
      fmt::format_to(str, "{}.{}:", attr_id, list_string);
    }
      return;
    default:
      assert(0 && "type not supported in key groups!");
      return;
  }
}
// Pass in both the augmented attributes set and the set of attributes passed
// from the object_create API call
switch_status_t create_keys(switch_object_id_t object_id,
                            const ObjectInfo *object_info,
                            const std::set<attr_w> &api_attrs,
                            const std::set<attr_w> &def_attrs) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const auto object_type = object_type_query(object_id);
  const std::vector<KeyGroup> &key_groups = object_info->get_key_groups();

  for (const auto &key_group_list : key_groups) {
    std::vector<attr_w> key_attrs;
    key_attrs.reserve(10);
    for (const auto attr_id : key_group_list.attr_list) {
      // First go through the api_attrs. These are mandatory attributes and
      // in most cases match the key group list
      // If we can't find an attr_id in api_attrs then search in DB
      auto it = std::find_if(
          api_attrs.begin(), api_attrs.end(), [&](attr_w const &attr) {
            return attr_id == attr.id_get();
          });
      if (it != api_attrs.end()) {
        key_attrs.push_back(*it);
        continue;
      }
      auto def_it = std::find_if(
          def_attrs.begin(), def_attrs.end(), [&](attr_w const &attr) {
            return attr_id == attr.id_get();
          });
      if (def_it != def_attrs.end()) {
        key_attrs.push_back(*def_it);
        continue;
      }
    }
    const auto ret = secondary_index->insert(key_attrs, object_id);
    if (!ret.second) {
      // switch_log(SWITCH_API_LEVEL_ERROR,
      //            object_type,
      //            SMI_GET_OPERATION,
      //            "{}: key already exists for {}, key {}",
      //            __func__,
      //            object_info->get_object_name(),
      //            key_attrs);
      return SWITCH_STATUS_ITEM_ALREADY_EXISTS;
    }
    // SWITCH_DETAIL_LOG(switch_log(
    //     SWITCH_API_LEVEL_DETAIL, object_type, "create_keys: {}", key_attrs));
  }

  return status;
}

switch_status_t update_keys(
    switch_object_id_t object_id,
    bool is_add /* true if adding, false if removing*/) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const auto object_type = object_type_query(object_id);
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  const std::vector<KeyGroup> &key_groups = object_info->get_key_groups();

  for (const auto &key_group_list : key_groups) {
    std::vector<attr_w> key_attrs;
    key_attrs.reserve(10);

    for (auto attr_id : key_group_list.attr_list) {
      attr_w attr(attr_id);
      status = attribute_get(object_id, attr_id, attr);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      key_attrs.push_back(attr);
    }

    if (is_add) {
      const auto ret = secondary_index->insert(key_attrs, object_id);
      if (!ret.second) {
        // switch_log(SWITCH_API_LEVEL_ERROR,
        //            object_type,
        //            "{}.{}: failed to insert key {}, object_id {}",
        //            __func__,
        //            __LINE__,
        //            key_attrs,
        //            object_id.data);
        return SWITCH_STATUS_FAILURE;
      }
    } else {
      const auto num_erased = secondary_index->erase(object_type, key_attrs);
      if (num_erased != 1) {
        // switch_log(SWITCH_API_LEVEL_ERROR,
        //            object_type,
        //            "{}.{}: failed to erase key {}, object_id {}",
        //            __func__,
        //            __LINE__,
        //            object_id.data,
        //            key_attrs);
        return SWITCH_STATUS_FAILURE;
      }
    }
    // SWITCH_DETAIL_LOG(switch_log(SWITCH_API_LEVEL_DETAIL,
    //                              object_type,
    //                              "update_keys: {}: {}",
    //                              is_add,
    //                              key_attrs));
  }

  return status;
}

static inline switch_status_t add_keys(switch_object_id_t object_id) {
  return update_keys(object_id, true);
}
static inline switch_status_t remove_keys(switch_object_id_t object_id) {
  return update_keys(object_id, false);
}

switch_status_t get_wkey(switch_object_type_t object_type,
                         std::set<attr_w> attrs,
                         switch_object_id_t &object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  const std::vector<KeyGroup> &key_groups = object_info->get_key_groups();
  bool kg_found = false;

  std::set<switch_attr_id_t> attr_ids_in;
  for (const auto &attr : attrs) {
    attr_ids_in.insert(attr.id_get());
  }

  std::vector<attr_w> key_attrs;
  for (const auto &key_group_list : key_groups) {
    bool found_all_attrs = true;
    for (const auto attr_id : key_group_list.attr_list) {
      // if attr_id not found in supplied list, try next key group
      if (attr_ids_in.find(attr_id) == attr_ids_in.end())
        found_all_attrs = false;
    }
    // goto next key group
    if (!found_all_attrs) continue;

    // Found a key group
    for (const auto attr_id : key_group_list.attr_list) {
      // The search key has to be built in the order it was created
      auto it = attrs.find(attr_id);
      // unlikely
      if (it == attrs.end()) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_GET_OPERATION,
                   "{}: Invalid attribute data for attr_id {} for {}",
                   __func__,
                   attr_id,
                   object_info->get_object_name());
        return SWITCH_STATUS_INVALID_KEY_GROUP;
      }
      key_attrs.push_back(*it);
    }
    kg_found = true;
    break;
  }

  if (!kg_found) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_GET_OPERATION,
               "{}: key group not found for {}",
               __func__,
               object_info->get_object_name());
    return SWITCH_STATUS_INVALID_KEY_GROUP;
  }

  const auto it = secondary_index->find(object_type, key_attrs);
  if (it != secondary_index->end(object_type)) {
    object_id = it->second;
  } else {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
  }
  // SWITCH_DETAIL_LOG(switch_log(SWITCH_API_LEVEL_DETAIL,
  //                              object_type,
  //                              SMI_GET_OPERATION,
  //                              "{}: key used {}, result={:#x}",
  //                              __func__,
  //                              key_attrs,
  //                              object_id.data));

  return status;
}

// check for existing objects with same ot and attrs
switch_status_t check_for_existing(switch_object_type_t object_type,
                                   const std::set<attr_w> &attrs,
                                   switch_object_id_t &object_id) {
  std::set<attr_w> kg_attrs;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  const std::vector<KeyGroup> &key_groups = object_info->get_key_groups();

  std::set<switch_attr_id_t> attr_ids_in;
  for (const auto &attr : attrs) {
    attr_ids_in.insert(attr.id_get());
  }

  // find matching key group
  for (auto key_group_list : key_groups) {
    bool goto_next = false;
    // Iterate through each group and check if we can find an existing object
    kg_attrs.clear();
    // First make sure we have all the attribute ids for the kg
    for (auto attr_id : key_group_list.attr_list) {
      if (attr_ids_in.find(attr_id) == attr_ids_in.end()) {
        // Skip this group
        goto_next = true;
        break;
      }
    }
    if (goto_next) continue;

    // Now fill in the kg_attrs for this key group
    for (auto attr_id : key_group_list.attr_list) {
      for (const auto &attr : attrs) {
        if (attr.id_get() == attr_id) kg_attrs.insert(attr);
      }
    }

    status = get_wkey(object_type, kg_attrs, object_id);
    if (status == SWITCH_STATUS_SUCCESS) {
      return SWITCH_STATUS_ITEM_ALREADY_EXISTS;
    }
  }
  return SWITCH_STATUS_ITEM_NOT_FOUND;
}

// Check attr value have correct type and also object type OID types
switch_status_t validate_attrs(const ObjectInfo *object_info,
                               const std::set<attr_w> &attrs) {
  // type check against schema
  for (const auto &attr : attrs) {
    const AttributeMetadata *attr_md =
        object_info->get_attr_metadata(attr.id_get());
    if (attr_md == NULL) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_info->object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: failed to get attr metadata ot={} attr {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 object_info->get_object_name(),
                 attr.getattr());
      return SWITCH_STATUS_FAILURE;
    }
    if (attr.type_get() != attr_md->type) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_info->object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: attr type mismatch given={} exp={} attr {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 attr.type_get(),
                 attr_md->type,
                 attr.getattr());
      return SWITCH_STATUS_INVALID_PARAMETER;
    }
    // check OID is of allowed types only
    if (attr.type_get() == SWITCH_TYPE_OBJECT_ID) {
      const ValueMetadata *value_md = attr_md->get_value_metadata();
      switch_object_id_t oid = {0};
      attr.v_get(oid);
      if (oid != 0 &&
          !value_md->is_allowed_object_type(object_type_query(oid))) {
        switch_log(
            SWITCH_API_LEVEL_ERROR,
            object_info->object_type,
            SMI_CREATE_OPERATION,
            "{}.{}:{}: oid type mismatch given={} oid={}",
            __NS__,
            __func__,
            __LINE__,
            model_info->get_object_name_from_type(object_type_query(oid)),
            oid);
        return SWITCH_STATUS_INVALID_PARAMETER;
      }
    } else if (attr.type_get() == SWITCH_TYPE_ENUM) {
      // Check given value is within enum range
      switch_enum_t enum_attr = {};
      attr.v_get(enum_attr);
      if (!attr_md->is_allowed_enum_value(enum_attr.enumdata)) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_info->object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: Invalid enum value {} for attr {} of object {}",
                   __NS__,
                   __func__,
                   __LINE__,
                   enum_attr.enumdata,
                   attr_md->get_attr_name(),
                   object_info->get_object_name());
        return SWITCH_STATUS_INVALID_PARAMETER;
      }
    }
  }
  return SWITCH_STATUS_SUCCESS;
}

/**
 * This will automate addition of members to a group object
 * For example, creating a vlan member will cause it to be added to a vlan
 * member list
 * For this to happen, the vlan_member will have a "membership" attribute which
 * looks like below.
 *   "membership" :
 *      {
 *        "object" : "vlan",
 *        "attribute" : "vlan_members"
 *      }
 * This also means there is a vlan_members attribute in the vlan object of type
 * list
 */
switch_status_t create_membership(const switch_object_id_t member_oid,
                                  const ObjectInfo *object_info,
                                  const std::set<attr_w> &attrs) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const switch_object_type_t member_ot = object_info->object_type;

  // Iterate through each group and add the member_oid to it
  for (const auto &object_attr_pair : object_info->get_membership_list()) {
    // Loop through each attribute to find our required group_ot
    for (const auto &attr : attrs) {
      if (attr.type_get() == SWITCH_TYPE_OBJECT_ID) {
        switch_object_id_t group_oid = {0};
        attr.v_get(group_oid);
        // If our type is found, add the member_oid to this group_oid's member
        // list
        if (group_oid.data == 0) continue;
        if (object_type_query(group_oid) == object_attr_pair.object_type) {
          status = switch_store::list_v_push(
              group_oid, object_attr_pair.attr_id, member_oid);
          if (status != SWITCH_STATUS_SUCCESS) {
            switch_log(SWITCH_API_LEVEL_ERROR,
                       member_ot,
                       SMI_CREATE_OPERATION,
                       "{}.{}:{}: failed to add {}.{} to {}.{}",
                       __NS__,
                       __func__,
                       __LINE__,
                       model_info->get_object_name_from_type(member_ot),
                       handle_to_id(member_oid),
                       model_info->get_object_name_from_type(
                           object_attr_pair.object_type),
                       handle_to_id(group_oid));
            return status;
          }
        }
      }
    }
  }
  return status;
}

switch_status_t delete_membership(const switch_object_id_t member_oid,
                                  const switch_object_type_t member_ot) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const ObjectInfo *object_info = model_info->get_object_info(member_ot);

  // Iterate through each group and delete the member_oid from it
  for (const auto object_attr_pair : object_info->get_membership_list()) {
    switch_object_id_t group_oid = {0};
    // this is the group object (vlan)
    switch_object_type_t group_ot = object_attr_pair.object_type;
    // this is the list attr_id in the group object (vlan_members)
    const auto grp_attr_id = object_attr_pair.attr_id;

    // loop through member objects attr list
    for (const auto &attr_md : object_info->get_attribute_list()) {
      if (attr_md.type == SWITCH_TYPE_OBJECT_ID) {
        const ValueMetadata *value_md = attr_md.get_value_metadata();
        for (const auto &ot : value_md->get_allowed_object_types()) {
          // found group object type in the member attr list
          if (group_ot == ot) {
            // now find group oid using this attr_id
            status =
                switch_store::v_get(member_oid, attr_md.attr_id, group_oid);
            if (status != SWITCH_STATUS_SUCCESS) {
              switch_log(SWITCH_API_LEVEL_ERROR,
                         member_ot,
                         SMI_DELETE_OPERATION,
                         "{}.{}:{}: failed to get oid for {} from {}.{}",
                         __NS__,
                         __func__,
                         __LINE__,
                         model_info->get_object_name_from_type(group_ot),
                         model_info->get_object_name_from_type(member_ot),
                         handle_to_id(member_oid));
              return status;
            }
          }
        }
      }
    }
    if (group_oid.data == 0) continue;
    // delete the member_oid from the group
    status = switch_store::list_v_del(group_oid, grp_attr_id, member_oid);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 member_ot,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: failed to delete {}.{} from {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(member_ot),
                 handle_to_id(member_oid),
                 model_info->get_object_name_from_type(group_ot),
                 handle_to_id(group_oid));
      return status;
    }
  }
  return status;
}

// This function takes care of re-evaluating auto objects for a previously
// referred USER object
// This routine is invoked for User object Create, Set and Delete operations,
// under below scenarios.
// All the below scenarios are valid only for re-evaluation attrs i.e. attr of
// type OBJECT_ID, having re_evaluate flag set to true in the schema
// Scenarios:
// 1. Object is being created and it contains re-evaluation attrs:
//    In this case the routine is a NOP
// 2. Object is being deleted and it contains re-evaluations attrs:
//    A. If the re-evaluate attr points to a USER Object (referred object)
//    and this is the last USER reference for that referred object, then it
//    triggers
//    deletion of auto objects of the referred object
//    B. In all other cases this routine is a NOP
// 3. Object attr is being set and it is re-evaluation attr:
//    A. If the re-evaluate attr was previously pointing to a USER Object
//    (referred
//    object) and this is the last USER reference for that referred object, then
//    it triggers
//    deletion of auto objects for that referred object
//    B. In all other cases this is a NOP
switch_status_t reevaluate_auto_obj_pre(const switch_object_id_t referrer_oid,
                                        const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  auto const referrer_ot = object_type_query(referrer_oid);
  std::set<switch_object_id_t> ref_oids;
  std::set<switch_object_id_t> ref_auto_oids;
  std::set<attr_w> attrs;
  switch_object_id_t referred_oid = {0};

  status = v_get(referrer_oid, attr.id_get(), referred_oid);
  auto const referred_ot = object_type_query(referred_oid);
  bool trigger = false;
  if (status == SWITCH_STATUS_ITEM_NOT_FOUND) {
    // Nothing do there, return
    status = SWITCH_STATUS_SUCCESS;
    goto exit;
  }
  if (status != SWITCH_STATUS_SUCCESS) goto exit;
  if (referred_oid == 0) goto exit;
  // Find all OTs of class User that refer to referred ot
  // and also the auto objects if any exist for the referred object
  for (const auto ref_ot : model_info->inverse_refs_get(referred_ot)) {
    const ObjectInfo *object_info = model_info->get_object_info(ref_ot);
    if (object_info == nullptr) continue;
    if (object_info->get_object_class() == OBJECT_CLASS_USER) {
      status |=
          switch_store::referencing_set_get(referred_oid, ref_ot, ref_oids);
    } else if (object_info->get_object_class() == OBJECT_CLASS_AUTO) {
      status |= switch_store::referencing_set_get(
          referred_oid, ref_ot, ref_auto_oids);
    }
  }

  trigger = trigger_context.insert(referred_ot).second;
  // 1. If I am  the last one referring this object, then re-evaluate.
  // References and reverse references (Object whom I refer and who refer
  // me) for objects of type USER are kept untouched. These are evaluated only
  // when the actual user
  // delete of these objects happens.
  if (trigger && (ref_oids.size() == 1) &&
      (ref_oids.find(referrer_oid) != ref_oids.end())) {
    if (!ref_auto_oids.empty()) {
      for (auto fn : delete_trigger_fns_before[referred_ot]) {
        status = (fn)(referred_oid);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     referred_ot,
                     SMI_DELETE_OPERATION,
                     "{}.{}:{}: trigger pre delete fail status={} for "
                     "referred obj {}.{} of obj {}.{}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(referred_ot),
                     handle_to_id(referred_oid),
                     model_info->get_object_name_from_type(referrer_ot),
                     handle_to_id(referrer_oid));
          goto exit;
        }
      }

      if (!delete_trigger_fns_after[referred_ot].empty()) {
        status = attribute_get_all(referred_oid, attrs);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     referred_ot,
                     SMI_DELETE_OPERATION,
                     "{}.{}:{}: trigger post delete failed status={}. Failed "
                     "to get all attributes for "
                     "referred obj {}.{} of obj {}.{}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(referred_ot),
                     handle_to_id(referred_oid),
                     model_info->get_object_name_from_type(referrer_ot),
                     handle_to_id(referrer_oid));
          goto exit;
        }
      }
      for (auto fn : delete_trigger_fns_after[referred_ot]) {
        status = (fn)(referred_ot, attrs);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     referred_ot,
                     SMI_DELETE_OPERATION,
                     "{}.{}:{}: trigger post delete fail status={} for "
                     "referred obj {}.{} of obj {}.{}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(referred_ot),
                     handle_to_id(referred_oid),
                     model_info->get_object_name_from_type(referrer_ot),
                     handle_to_id(referrer_oid));
          goto exit;
        }
      }
    }
  }

exit:
  if (trigger) {
    const auto num_erased = trigger_context.erase(referred_ot);
    CHECK_RET(num_erased != 1, SWITCH_STATUS_FAILURE);
  }
  SWITCH_DETAIL_LOG(
      switch_log(SWITCH_API_LEVEL_DETAIL,
                 referrer_ot,
                 "{}.{}:{}: status={} for referrer {}.{} referred {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(referrer_ot),
                 handle_to_id(referrer_oid),
                 attr.getattr()));
  return status;
}

// This function takes care of re-evaluating auto objects for newly referred
// USER object
// This routine is invoked for USER object Create & Set operations,
// under below scenarios.
// All the below scenarios are valid only for re-evaluation attrs i.e. attr of
// type OBJECT_ID, having re_evaluate flag set to true in the schema
// Scenarios:
// 1. Object is being created and it contains re-evaluation attrs:
//    A. If the re-evaluate attr points to a USER Object (referred object)
//    and this is the first USER reference for that referred object, then
//    it triggers creation of auto objects for the referred object. If the
//    referred object already had auto objects then those objects are deleted
//    before recreating them again.
//    B. In all other cases this is a NOP
// 2. Object attr is being set and it is a re-evaluation attr:
//    A. If the re-evaluate attr set points to a USER Object (referred object)
//    and
//    this is the first USER reference for that referred object, then it
//    triggers
//    creation of auto objects for that referred object. If the referred object
//    already
//    had auto objects then those objects are deleted before recreating them
//    again.
//    B. In all other cases this is a NOP
switch_status_t reevaluate_auto_obj_post(const switch_object_id_t referrer_oid,
                                         const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  auto const referrer_ot = object_type_query(referrer_oid);
  std::set<switch_object_id_t> ref_oids;
  std::set<switch_object_id_t> ref_auto_oids;
  std::set<attr_w> attrs;

  switch_object_id_t referred_oid = {0};
  status = attr.v_get(referred_oid);
  auto const referred_ot = object_type_query(referred_oid);
  bool trigger = false;
  if (status != SWITCH_STATUS_SUCCESS) goto exit;
  if (referred_oid == 0) goto exit;
  // Find all OTs of class User that refer to referred ot
  // and also the auto objects if any exist for the referred object
  for (const auto ref_ot : model_info->inverse_refs_get(referred_ot)) {
    const ObjectInfo *object_info = model_info->get_object_info(ref_ot);
    if (object_info == nullptr) continue;
    if (object_info->get_object_class() == OBJECT_CLASS_USER) {
      status |=
          switch_store::referencing_set_get(referred_oid, ref_ot, ref_oids);
    } else if (object_info->get_object_class() == OBJECT_CLASS_AUTO) {
      status |= switch_store::referencing_set_get(
          referred_oid, ref_ot, ref_auto_oids);
    }
  }

  trigger = trigger_context.insert(referred_ot).second;
  // 1. If I am  the first one referring this object, then re-evaluate.
  // 2. This routine is commmon to for both create and attribute set case.
  if (trigger && (ref_oids.size() == 1) &&
      (ref_oids.find(referrer_oid) != ref_oids.end())) {
    if (!ref_auto_oids.empty()) {
      for (auto fn : delete_trigger_fns_before[referred_ot]) {
        status = (fn)(referred_oid);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     referred_ot,
                     SMI_DELETE_OPERATION,
                     "{}.{}:{}: trigger pre delete fail status={} for "
                     "referred obj {}.{} of obj {}.{}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(referred_ot),
                     handle_to_id(referred_oid),
                     model_info->get_object_name_from_type(referrer_ot),
                     handle_to_id(referrer_oid));
          goto exit;
        }
      }

      if (!delete_trigger_fns_after[referred_ot].empty()) {
        status = attribute_get_all(referred_oid, attrs);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     referred_ot,
                     SMI_DELETE_OPERATION,
                     "{}.{}:{}: trigger post delete failed status={}. Failed "
                     "to get all attributes for "
                     "referred obj {}.{} of obj {}.{}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(referred_ot),
                     handle_to_id(referred_oid),
                     model_info->get_object_name_from_type(referrer_ot),
                     handle_to_id(referrer_oid));
          goto exit;
        }
      }
      for (auto fn : delete_trigger_fns_after[referred_ot]) {
        status = (fn)(referred_ot, attrs);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     referred_ot,
                     SMI_DELETE_OPERATION,
                     "{}.{}:{}: trigger post delete fail status={} for "
                     "referred obj {}.{} of obj {}.{}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(referred_ot),
                     handle_to_id(referred_oid),
                     model_info->get_object_name_from_type(referrer_ot),
                     handle_to_id(referrer_oid));
          goto exit;
        }
      }
    }

    const ObjectInfo *object_info = model_info->get_object_info(referred_ot);
    // const bool trigger = trigger_context.insert(referred_ot).second;

    std::set<attr_w> attrs_local;
    for (const auto &attr_md : object_info->get_attribute_list()) {
      if (!attr_md.get_flags().is_read_only &&
          !attr_md.get_flags().is_internal) {
        attr_w tmp_attr(attr_md.attr_id);
        status = attribute_get(referred_oid, attr_md.attr_id, tmp_attr);
        attrs_local.insert(tmp_attr);
      }
    }

    for (auto fn : create_trigger_fns_before[referred_ot]) {
      status = (fn)(referred_ot, attrs_local);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   referred_ot,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: trigger before fail status={} for referred obj "
                   "{}.{} of obj {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(referred_ot),
                   handle_to_id(referred_oid),
                   model_info->get_object_name_from_type(referrer_ot),
                   handle_to_id(referrer_oid));
        goto exit;
      }
    }

    for (auto fn : create_trigger_fns_after[referred_ot]) {
      status = (fn)(referred_oid, attrs_local);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   referred_ot,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: trigger after fail status={} for referred obj "
                   "{}.{} of obj {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(referred_ot),
                   handle_to_id(referred_oid),
                   model_info->get_object_name_from_type(referrer_ot),
                   handle_to_id(referrer_oid));
        goto exit;
      }
    }
  }
exit:
  if (trigger) {
    const auto num_erased = trigger_context.erase(referred_ot);
    CHECK_RET(num_erased != 1, SWITCH_STATUS_FAILURE);
  }
  SWITCH_DETAIL_LOG(
      switch_log(SWITCH_API_LEVEL_DETAIL,
                 referrer_ot,
                 "{}.{}:{}: status={} for referrer {}.{} referred {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(referrer_ot),
                 handle_to_id(referrer_oid),
                 attr.getattr()));
  return status;
}

switch_status_t attr_reevaluate_before_delete(
    const switch_object_id_t referrer_oid) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  auto const referrer_ot = object_type_query(referrer_oid);
  const ObjectInfo *object_info = model_info->get_object_info(referrer_ot);
  switch_attr_flags_t flags = {};

  // loop through objects attr list to find and re-evaluate attrs
  for (const auto &attr_md : object_info->get_attribute_list()) {
    if (attr_md.type == SWITCH_TYPE_OBJECT_ID) {
      flags = attr_md.get_flags();
      if (flags.re_evaluate) {
        attr_w attr(attr_md.attr_id);
        status = attribute_get(referrer_oid, attr_md.attr_id, attr);
        if (status != SWITCH_STATUS_SUCCESS) return status;
        // We need to call only reevaluate-pre as this will take care of
        // any existing references held by referrer object that need to be
        // re-evaluated
        // Since the object is being deleted no post re-evaluation is needed
        status = reevaluate_auto_obj_pre(referrer_oid, attr);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     referrer_ot,
                     "{}.{}:{}: obj pre re-evaluate task fail status={} for "
                     "{}.{} for attribute {}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(referrer_ot),
                     handle_to_id(referrer_oid),
                     attr.getattr());
          return status;
        }
      }
    }
  }

  return status;
}

static inline switch_status_t attr_list_push(switch_object_id_t object_id,
                                             const AttributeMetadata *attr_md,
                                             const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const ValueMetadata *value_md = attr_md->get_value_metadata();
  switch (value_md->type) {
    case SWITCH_TYPE_BOOL: {
      std::vector<bool> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_UINT8: {
      std::vector<uint8_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_UINT16: {
      std::vector<uint16_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_UINT32: {
      std::vector<uint32_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_UINT64: {
      std::vector<uint64_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_INT64: {
      std::vector<int64_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_OBJECT_ID: {
      std::vector<switch_object_id_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_STRING: {
      std::vector<switch_string_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_IP_ADDRESS: {
      std::vector<switch_ip_address_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_IP_PREFIX: {
      std::vector<switch_ip_prefix_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_MAC: {
      std::vector<switch_mac_addr_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    case SWITCH_TYPE_ENUM: {
      std::vector<switch_enum_t> list;
      attr.v_get(list);
      status = list_push(object_id, attr_md->attr_id, list);
      break;
    }
    default:
      status = SWITCH_STATUS_INVALID_PARAMETER;
      break;
  }
  return status;
}

switch_status_t attr_set_on_create(const ObjectInfo *object_info,
                                   const switch_object_id_t &object_id,
                                   const attr_w &attr,
                                   const AttributeMetadata *attr_md,
                                   std::vector<db::value_wrapper> &object_attrs,
                                   bool db_set,
                                   std::set<switch_object_id_t> &references) {
  (void)db_set;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const switch_object_type_t object_type = object_info->object_type;
  switch_attr_id_t attr_id = attr.id_get();

  if (attr.type_get() != attr_md->type) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_info->object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: attr type mismatch given={} exp={} attr {}",
               __NS__,
               __func__,
               __LINE__,
               attr.type_get(),
               attr_md->type,
               attr.getattr());
    return SWITCH_STATUS_INVALID_PARAMETER;
  }
  switch_attr_flags_t flags = attr_md->get_flags();

  if (!is_composite_type(attr_md->type)) {
    switch_object_id_t oid = {0};
    if (attr_md->type == SWITCH_TYPE_OBJECT_ID) {
      const ValueMetadata *value_md = attr_md->get_value_metadata();
      status = attr_util::v_get(attr.getattr().value, oid);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: attr get oid fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type),
                   handle_to_id(object_id));
        return status;
      }

      if (oid != 0 &&
          !value_md->is_allowed_object_type(object_type_query(oid))) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: oid type mismatch given={}",
                   __NS__,
                   __func__,
                   __LINE__,
                   object_type_query(oid));
        return SWITCH_STATUS_INVALID_PARAMETER;
      }
    } else if (attr_md->type == SWITCH_TYPE_ENUM) {
      // Check given value is within enum range
      switch_enum_t enum_attr = {};
      attr.v_get(enum_attr);
      if (!attr_md->is_allowed_enum_value(enum_attr.enumdata)) {
        switch_log(
            SWITCH_API_LEVEL_ERROR,
            object_info->object_type,
            SMI_CREATE_OPERATION,
            "{}.{}:{}: Invalid enum value {} for attr {} of object {}.{}",
            __NS__,
            __func__,
            __LINE__,
            enum_attr.enumdata,
            attr_md->get_attr_name(),
            model_info->get_object_name_from_type(object_type),
            handle_to_id(object_id));
        return SWITCH_STATUS_INVALID_PARAMETER;
      }
    }

    object_attrs.emplace_back(attr_id, 0, attr.getattr().value);

    if (attr_md->type == SWITCH_TYPE_OBJECT_ID) {
      // add reference
      if (!(flags.is_internal || flags.is_read_only)) {
        status |= object_ref_add(object_id, oid, attr_id);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(
              SWITCH_API_LEVEL_ERROR,
              object_type,
              SMI_CREATE_OPERATION,
              "{}.{}:{}: obj ref add fail status={} for {}.{} via attr:{} to "
              "{}.{}",
              __NS__,
              __func__,
              __LINE__,
              status,
              model_info->get_object_name_from_type(object_type),
              handle_to_id(object_id),
              attr_md->get_attr_name(),
              model_info->get_object_name_from_type(object_type_query(oid)),
              handle_to_id(oid));
          return status;
        }
        references.insert(oid);
      }
    }
  } else {
    switch (attr_md->type) {
      case SWITCH_TYPE_LIST: {
        switch_attribute_value_t value_in = {};
        uint16_t extra = 0;
        const ValueMetadata *value_md = attr_md->get_value_metadata();
        const auto &attr_list = attr.getattr_list();
        value_in.type = SWITCH_TYPE_LIST;
        value_in.list.count = attr_list.size();
        value_in.list.list_type = value_md->type;
        object_attrs.emplace_back(attr_id, extra, value_in);
        for (const auto &val : attr.getattr_list()) {
          object_attrs.emplace_back(attr_id, ++extra, val);
          if (value_md->type == SWITCH_TYPE_OBJECT_ID) {
            if (!(flags.is_internal || flags.is_read_only)) {
              status = object_ref_add(object_id, val.oid, attr_id);
              if (status != SWITCH_STATUS_SUCCESS) {
                switch_log(SWITCH_API_LEVEL_ERROR,
                           object_type,
                           SMI_CREATE_OPERATION,
                           "{}.{}:{}: obj ref add fail status={} for {}.{} via "
                           "attr:{} to "
                           "{}.{}",
                           __NS__,
                           __func__,
                           __LINE__,
                           status,
                           model_info->get_object_name_from_type(object_type),
                           handle_to_id(object_id),
                           attr_md->get_attr_name(),
                           model_info->get_object_name_from_type(
                               object_type_query(val.oid)),
                           handle_to_id(val.oid));
                return status;
              }
              references.insert(val.oid);
            }
          }
        }
      } break;
      default:
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: Unexpected",
                   __NS__,
                   __func__,
                   __LINE__);
        return SWITCH_STATUS_FAILURE;
    }
  }
  return status;
}

switch_status_t object_replay_stage_1(const switch_object_type_t object_type,
                                      const switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::vector<db::value_wrapper> empty_attrs;

  const ObjectInfo *object_info = model_info->get_object_info(object_type);

  if (object_info == nullptr) return SWITCH_STATUS_FAILURE;

  std::set<switch_object_id_t> refs;
  // Loop through all attributes and update object references
  for (const auto &attr_md : object_info->get_attribute_list()) {
    attr_w attr(attr_md.attr_id);
    status = attribute_get(object_id, attr_md.attr_id, attr);
    status = attr_set_on_create(
        object_info, object_id, attr, &attr_md, empty_attrs, false, refs);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 "{}.{}:{}: attr set fail for default attrs status={} for "
                 "{}.{} attr {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id),
                 attr.getattr());
      return status;
    }
  }

  return status;
}

switch_status_t object_replay_stage_2(const switch_object_type_t object_type,
                                      const switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool add_keys_success = false;
  std::set<switch_object_id_t> dereference;
  std::set<attr_w> attrs_local;

  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Unknown object_type {} status {}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }

  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  const bool trigger = trigger_context.insert(object_type).second;

  for (const auto &attr_md : object_info->get_attribute_list()) {
    attr_w attr(attr_md.attr_id);
    status = attribute_get(object_id, attr_md.attr_id, attr);
    attrs_local.insert(attr);
  }

  if (trigger) {
    for (auto fn : create_trigger_fns_before[object_type]) {
      status = (fn)(object_type, attrs_local);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: trigger before fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type),
                   switch_store::handle_to_id(object_id));
        goto exit;
      }
    }
  }

  /* Add keys */
  status = add_keys(object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: create_keys fail status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               switch_store::handle_to_id(object_id));
    goto exit;
  }
  add_keys_success = true;

  // This trigger doesn't invoke the factory callback in warm init mode. It will
  // execute all other registered callbacks. Factory callbacks are handled in a
  // special manner in stage 3
  if (trigger) {
    for (auto fn : create_trigger_fns_after[object_type]) {
      status = (fn)(object_id, attrs_local);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: trigger after fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type),
                   switch_store::handle_to_id(object_id));
        goto exit;
      }
    }
  }

exit:
  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: status={} {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 switch_store::handle_to_id(object_id)));
  if (trigger) {
    const auto num_erased = trigger_context.erase(object_type);
    CHECK_RET(num_erased != 1, SWITCH_STATUS_FAILURE);
  }

  /* Clear db and oid if failure anywhere above */
  if (status) {
    if (add_keys_success) remove_keys(object_id);
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: failure status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               switch_store::handle_to_id(object_id));
  }
  return status;
}

switch_status_t object_replay_stage_3(const switch_object_type_t object_type,
                                      const switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  // All factory callbacks are now executed here
  status = factory_create_all_auto_objects(object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: factory auto create fail status={} for {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    return status;
  }

  return status;
}

/* Replay happens in 4 stages
 * Stage 0: db_load. This happens in object_info_init above. This involves
 *   simply reading from the DB file and updating the store. The oid is also set
 *   in this stage
 * Stage 1: This stage updates the object references internally. Basically
 *   updating the ObjectRefs map seen above. This is required for stage 3 when
 *   auto objects start getting created.
 * Stage 2: This stage sets up the secondary index and invokes all the pre/post
 *   triggers for user objects
 * Stage 3: This is the final stage where all the objects are replayed by
 *   simulating object create
 */
switch_status_t object_replay(bool warm_init) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (!warm_init) return status;

  switch_log(SWITCH_API_LEVEL_WARN, SWITCH_OT_NONE, "Begin Object replay");
  const db::db_store *object_attr_hash = db::get_db();
  if (object_attr_hash == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: BFN SDK object replay failed to get object hash: {}",
               __func__,
               __LINE__,
               status);
    return SWITCH_STATUS_FAILURE;
  }

  std::vector<switch_object_id_t> ordered_creation_list =
      db::get_creation_list();

  // Notify factory and backend that we are entering warm init
  smiContext::context().warm_init_begin();

  switch_log(
      SWITCH_API_LEVEL_WARN, SWITCH_OT_NONE, "Begin Object replay stage 1");
  for (auto it = object_attr_hash->begin(); it != object_attr_hash->end();
       it++) {
    switch_object_id_t object = it->first;
    const ObjectInfo *object_info =
        model_info->get_object_info(switch_store::object_type_query(object));
    status = switch_store::object_replay_stage_1(
        switch_store::object_type_query(object), object);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          switch_store::object_type_query(object),
          "{}.{}: BFN SDK DB load stage 1 failure object {}.{} error: {}",
          __func__,
          __LINE__,
          object_info->get_object_name(),
          switch_store::handle_to_id(object),
          status);
    }
  }

  switch_log(
      SWITCH_API_LEVEL_WARN, SWITCH_OT_NONE, "Begin Object replay stage 2");
  for (const auto object : ordered_creation_list) {
    const ObjectInfo *object_info =
        model_info->get_object_info(switch_store::object_type_query(object));
    if (object_info == nullptr) continue;
    if (object_info->get_object_class() == OBJECT_CLASS_AUTO) {
      add_keys(object);
      continue;
    }
    status = switch_store::object_replay_stage_2(
        switch_store::object_type_query(object), object);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          switch_store::object_type_query(object),
          "{}.{}: BFN SDK DB load stage 2 failure object {}.{} error: {}",
          __func__,
          __LINE__,
          object_info->get_object_name(),
          switch_store::handle_to_id(object),
          status);
    }
  }

  switch_log(
      SWITCH_API_LEVEL_WARN, SWITCH_OT_NONE, "Begin Object replay stage 3");
  for (const auto object : ordered_creation_list) {
    const ObjectInfo *object_info =
        model_info->get_object_info(switch_store::object_type_query(object));

    if (object_info == nullptr) continue;
    if (object_info->get_object_class() == OBJECT_CLASS_AUTO) continue;

    status = switch_store::object_replay_stage_3(
        switch_store::object_type_query(object), object);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          switch_store::object_type_query(object),
          "{}.{}: BFN SDK DB load stage 3 failure object {}.{} error: {}",
          __func__,
          __LINE__,
          object_info->get_object_name(),
          switch_store::handle_to_id(object),
          status);
    }
  }
  smiContext::context().warm_init_end();

#ifndef TESTING
  switch_log(
      SWITCH_API_LEVEL_WARN, SWITCH_OT_NONE, "Begin Object replay stage 4");
  status = switch_bf_rt_flush();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: BFN SDK DB load stage 4 failure error: {}",
               __func__,
               __LINE__,
               status);
  }
#endif

  switch_log(SWITCH_API_LEVEL_WARN, SWITCH_OT_NONE, "End Object replay");
  return status;
}

switch_status_t object_backup_stats_cache() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  uint64_t num_objects = 0;
  uint64_t num_counter_objects = 0;
  uint64_t num_stats_cache_objects = 0;

  ModelInfo *model = switch_store::switch_model_info_get();
  if (model == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: model is null",
               __func__,
               __LINE__);
    return SWITCH_STATUS_FAILURE;
  }

  const db::db_store *object_attr_hash = db::get_db();
  if (object_attr_hash == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: object_attr_hash is null",
               __func__,
               __LINE__);
    return SWITCH_STATUS_FAILURE;
  }

  switch_log(
      SWITCH_API_LEVEL_WARN,
      SWITCH_OT_NONE,
      "{}.{}: Counters Save start: Saving counters from sw-cache to store",
      __func__,
      __LINE__);

  for (auto it = object_attr_hash->begin(); it != object_attr_hash->end();
       it++) {
    switch_object_id_t object_id = it->first;
    switch_object_type_t object_type =
        switch_store::object_type_query(object_id);
    const ObjectInfo *obj_info = model->get_object_info(object_type);

    num_objects++;
    // check if parent object has counter
    if (obj_info && obj_info->get_counter()) {
      num_counter_objects++;
      for (const auto ref_ot : model->counter_refs_get(object_type)) {
        const ObjectInfo *obj_ref_info = model->get_object_info(ref_ot);
        // check if object has stats cache
        if (obj_ref_info && obj_ref_info->get_stats_cache()) {
          num_stats_cache_objects++;
          std::unique_ptr<object> object(
              factory::get_instance().create(ref_ot, object_id, status));
          if (object != nullptr) {
            status = object->counters_save(object_id);
            if (status != SWITCH_STATUS_SUCCESS) {
              switch_log(SWITCH_API_LEVEL_ERROR,
                         SWITCH_OT_NONE,
                         "{}.{}: counters save to store fail status={}, "
                         " object type {} parent {:#x}",
                         __func__,
                         __LINE__,
                         status,
                         obj_ref_info->get_object_name(),
                         object_id.data);
              continue;
            }
          }
        }
      }
    }
  }

  switch_log(SWITCH_API_LEVEL_WARN,
             SWITCH_OT_NONE,
             "{}.{}: Counters Save complete: {} objects with stats-cache, "
             "Total objects with counters {}, Total objects {}",
             __func__,
             __LINE__,
             num_stats_cache_objects,
             num_counter_objects,
             num_objects);

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t object_restore_stats_cache(bool warm_init) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  uint64_t num_objects = 0;
  uint64_t num_counter_objects = 0;
  uint64_t num_stats_cache_objects = 0;

  // If not warm init, just return
  if (!warm_init) {
    return SWITCH_STATUS_SUCCESS;
  }

  ModelInfo *model = NULL;
  model = switch_store::switch_model_info_get();
  if (model == nullptr) {
    return SWITCH_STATUS_SUCCESS;
  }

  const db::db_store *object_attr_hash = db::get_db();
  if (object_attr_hash == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: Restoring counter values failed to get object hash: {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  switch_log(SWITCH_API_LEVEL_WARN,
             SWITCH_OT_NONE,
             "{}.{}: Counters Restore start: Restoring counters to sw-cache "
             "from store",
             __func__,
             __LINE__);

  for (auto it = object_attr_hash->begin(); it != object_attr_hash->end();
       it++) {
    switch_object_id_t object_id = it->first;
    switch_object_type_t object_type =
        switch_store::object_type_query(object_id);
    const ObjectInfo *obj_info = model->get_object_info(object_type);

    num_objects++;
    // check if parent object has counter
    if (obj_info && obj_info->get_counter()) {
      num_counter_objects++;

      for (const auto ref_ot : model->counter_refs_get(object_type)) {
        const ObjectInfo *obj_ref_info = model->get_object_info(ref_ot);
        // check if object has stats cache
        if (obj_ref_info && obj_ref_info->get_stats_cache()) {
          num_stats_cache_objects++;

          std::unique_ptr<object> object(
              factory::get_instance().create(ref_ot, object_id, status));
          if (object != nullptr) {
            status = object->counters_restore(object_id);
            if (status != SWITCH_STATUS_SUCCESS) {
              switch_log(SWITCH_API_LEVEL_ERROR,
                         SWITCH_OT_NONE,
                         "{}.{}: counters restore from store fail status={}, "
                         " object type {} parent {:#x}",
                         __func__,
                         __LINE__,
                         status,
                         obj_ref_info->get_object_name(),
                         object_id.data);
              continue;
            }
          }
        }
      }
    }
  }

  switch_log(SWITCH_API_LEVEL_WARN,
             SWITCH_OT_NONE,
             "{}.{}: Counters Restore complete: {} objects with stats-cache, "
             "Total objects with counters {}, Total objects {}",
             __func__,
             __LINE__,
             num_stats_cache_objects,
             num_counter_objects,
             num_objects);

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t object_create_internal(const switch_object_type_t object_type,
                                       const std::set<attr_w> &attrs,
                                       const switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool db_success = false, add_keys_success = false, before_trigs = false,
       after_trigs = false;
  std::set<switch_object_id_t> dereference;
  std::vector<db::value_wrapper> object_attrs;
  std::list<attr_w> reeval_attrs;
  bool comment_mode;

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: object create for {}, oid={:#x}",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 object_id.data));

  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info == nullptr) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OT_NONE,
        SMI_CREATE_OPERATION,
        "{}.{}:{}: Object info get failed. Unknown object_type {}, status={}",
        __NS__,
        __func__,
        __LINE__,
        object_type,
        status);
    return status;
  }

  // If object id is not allocated, return error
  if (object_id.data == SWITCH_NULL_OBJECT_ID) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: oid is null, object type {}",
               __NS__,
               __func__,
               __LINE__,
               model_info->get_object_name_from_type(object_type));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  const auto &attr_md_list = object_info->get_attribute_list();
  const std::set<attr_w> &defaults = defaults_cache[object_type];
  auto attrs_local(attrs);
  const bool trigger = trigger_context.insert(object_type).second;

  comment_mode = record::record_comment_mode_get();
  record::record_comment_mode_set(true);
  if (trigger) {
    for (auto fn : create_trigger_fns_before[object_type]) {
      status = (fn)(object_type, attrs_local);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: trigger before fail status={} for {}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type));
        goto exit;
      }
      before_trigs = true;
    }
  }
  record::record_comment_mode_set(comment_mode);

  object_attrs.reserve(attr_md_list.size() + 1);
  for (const auto &attr_md : attr_md_list) {
    switch_attr_flags_t flags = attr_md.get_flags();
    auto it = attrs_local.find(attr_md.attr_id);
    if (it == attrs_local.end()) {
      it = defaults.find(attr_md.attr_id);
      if (it == defaults.end()) {
        status = SWITCH_STATUS_INVALID_PARAMETER;
      } else {
        status = attr_set_on_create(object_info,
                                    object_id,
                                    *it,
                                    &attr_md,
                                    object_attrs,
                                    true,
                                    dereference);
      }
    } else {
      status = attr_set_on_create(object_info,
                                  object_id,
                                  *it,
                                  &attr_md,
                                  object_attrs,
                                  true,
                                  dereference);
    }
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: attr set fail for default attrs status={} for "
                 "{}.{} attr {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id),
                 attr_md.get_attr_name());
      goto exit;
    }
    if (flags.re_evaluate) {
      reeval_attrs.push_back(*it);
      switch_log(
          SWITCH_API_LEVEL_ERROR, object_type, "{}", attr_md.get_attr_name());
    }
  }

  status = db::object_create_with_attrs(object_id, object_attrs);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: db::object_create fail status={} for {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    goto exit;
  }
  db_success = true;

  record::record_add_create(object_type, attrs, object_id, status);

  for (const auto &reeval : reeval_attrs) {
    status = reevaluate_auto_obj_post(object_id, reeval);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 "{}.{}:{}: obj post re-evaluate task fail status={} for "
                 "{}.{} for attribute {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id),
                 reeval.getattr());
      goto exit;
    }
  }

  /* Create keys */
  status = create_keys(object_id, object_info, attrs_local, defaults);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: create_keys fail status={} for {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    goto exit;
  }
  add_keys_success = true;

  comment_mode = record::record_comment_mode_get();
  record::record_comment_mode_set(true);
  if (trigger) {
    for (auto fn : create_trigger_fns_after[object_type]) {
      status = (fn)(object_id, attrs_local);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: trigger after fail status={} for {}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type));
        goto exit;
      }
      // Set after trigs flag to true, so that rollback operation is peformed
      // during cleanup. It is the responsibilty of the
      // corresponding delete before triggers to ensure that rollback is done
      // only if its corresponding create after
      // trigger was evaluated.
      after_trigs = true;
    }
  }
  record::record_comment_mode_set(comment_mode);

  status = create_membership(object_id, object_info, attrs_local);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: create_membership fail status={} for {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    goto exit;
  }

exit:
  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: status={} {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 switch_store::handle_to_id(object_id)));

  /* Clear db and oid if failure anywhere above */
  if (status) {
    switch_status_t tmp_status = SWITCH_STATUS_SUCCESS;
    if (trigger && after_trigs) {
      for (auto fn : delete_trigger_fns_before[object_type]) {
        tmp_status = (fn)(object_id);
        if (tmp_status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     object_type,
                     SMI_CREATE_OPERATION,
                     "{}.{}:{}: trigger pre delete failure during rollback "
                     "status={} for {}.{}",
                     __NS__,
                     __func__,
                     __LINE__,
                     tmp_status,
                     model_info->get_object_name_from_type(object_type),
                     handle_to_id(object_id));
        }
      }
    }
    if (add_keys_success) remove_keys(object_id);
    if (db_success) db::object_delete(object_id);
    // Delete references. Because DB delete has already happened in above step
    // we can no longer use object_refs_get to
    // get object referenced by object_id. Hence we use attrs and default attrs
    // to get all referenced objects
    if (object_id.data) {
      // object_refs_get(dereference, object_id, attrs_local, defaults, false);
      for (auto const i : dereference) {
        object_ref_remove(object_id, i);
      }
    }
    if (trigger && before_trigs) {
      // This is weird, since the object_id being passed has already been
      // removed from the DB. However currently the delete after trigger
      // accepts object_id as the only input parameter. May be this can be
      // changed to object type. But until then we continue to pass the
      // object_id. The callback function should, however, not use this oid
      // for db fetches or fetching and other state (references etc.)
      for (auto fn : delete_trigger_fns_after[object_type]) {
        tmp_status = (fn)(object_type, attrs_local);
        if (tmp_status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     object_type,
                     SMI_CREATE_OPERATION,
                     "{}.{}:{}: trigger post delete failure during rollback "
                     "status={} for {}.{}",
                     __NS__,
                     __func__,
                     __LINE__,
                     tmp_status,
                     model_info->get_object_name_from_type(object_type),
                     handle_to_id(object_id));
        }
      }
    }

    // switch_log(SWITCH_API_LEVEL_ERROR,
    //            object_type,
    //            SMI_CREATE_OPERATION,
    //            "{}.{}:{}: {}",
    //            __NS__,
    //            __func__,
    //            __LINE__,
    //            attrs);
  }
  if (trigger) {
    const auto num_erased = trigger_context.erase(object_type);
    CHECK_RET(num_erased != 1, SWITCH_STATUS_FAILURE);
  }

#ifdef EVENTS_ENABLED
  switch_attribute_t attr;
  memset(&attr, 0, sizeof(attr));
  smi::event::object_notify(
      SWITCH_OBJECT_EVENT_CREATE, object_type, object_id, attr, status);
#endif
  return status;
}

// Existing object create API - this internally allocates next available oid
switch_status_t object_create(const switch_object_type_t object_type,
                              const std::set<attr_w> &attrs,
                              switch_object_id_t &object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Unknown object_type {}, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }

  // initialize the object  id before allocating
  object_id.data = SWITCH_NULL_OBJECT_ID;
  status = oid_create(object_type, object_id, false);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: oid create fail status={} for type {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    goto fail;
  }

  status = object_create_internal(object_type, attrs, object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: object_create_internal fail "
               "status={} oid {} for object type {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               object_id,
               model_info->get_object_name_from_type(object_type));
    goto fail;
  }

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: object create success type {} oid {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 object_id));
  return status;

fail:
  if (object_id.data != SWITCH_NULL_OBJECT_ID) oid_free(object_id);
  object_id.data = SWITCH_NULL_OBJECT_ID;
  return status;
}

// New object create by id API - This internally reserves oid mapped to id
switch_status_t object_create_by_id(const switch_object_type_t object_type,
                                    const std::set<attr_w> &attrs,
                                    const uint64_t id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t object_id = {SWITCH_NULL_OBJECT_ID};
  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Unknown object_type {}, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }

  object_id = switch_store::id_to_handle(object_type, id);
  if (object_id.data == SWITCH_NULL_OBJECT_ID) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Id {} is incorrect for object type {}",
               __NS__,
               __func__,
               __LINE__,
               id,
               model_info->get_object_name_from_type(object_type));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  // first reserve the oid
  status = oid_create(object_type, object_id, true);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: oid {} reserve fail status={} for type {}, id {}",
               __NS__,
               __func__,
               __LINE__,
               object_id,
               status,
               model_info->get_object_name_from_type(object_type),
               id);
    goto fail;
  }

  status = object_create_internal(object_type, attrs, object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: object_create_internal fail "
               "status={} oid {} for object type {}, id {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               object_id,
               model_info->get_object_name_from_type(object_type),
               id);
    goto fail;
  }

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: object create success type {} oid {} id {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 object_id,
                 id));
  return status;

fail:
  if (object_id.data != SWITCH_NULL_OBJECT_ID) oid_free(object_id);
  object_id.data = SWITCH_NULL_OBJECT_ID;
  return status;
}

// New object create by hdl API - this requires valid oid  by caller
// Allocation/reserve of oid must happen before calling this, for ex - SAL
switch_status_t object_create_by_hdl(const switch_object_type_t object_type,
                                     const std::set<attr_w> &attrs,
                                     const switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Unknown object_type {}, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }

  // Logic for object allocate  by hdl
  if (object_id.data == SWITCH_NULL_OBJECT_ID) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: oid  is null for object type {}",
               __NS__,
               __func__,
               __LINE__,
               model_info->get_object_name_from_type(object_type));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  status = object_create_internal(object_type, attrs, object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: object_create_internal fail "
               "status={} oid {} for object type {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               object_id,
               model_info->get_object_name_from_type(object_type));
    return status;
  }

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: object create success type {} oid {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 object_id));
  return status;
}

switch_status_t object_create_minimal(const switch_object_type_t object_type,
                                      const std::set<attr_w> &attrs,
                                      switch_object_id_t &object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::set<switch_object_id_t> dereference;
  std::vector<db::value_wrapper> object_attrs;
  bool db_success = false;

  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: Unknown object_type {}, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }

  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info == nullptr) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OT_NONE,
        SMI_CREATE_OPERATION,
        "{}.{}:{}: Object info get failed. Unknown object_type {}, status={}",
        __NS__,
        __func__,
        __LINE__,
        object_type,
        status);
    return status;
  }

  const std::set<attr_w> &defaults = defaults_cache[object_type];
  object_id.data = SWITCH_NULL_OBJECT_ID;
  status = oid_create(object_type, object_id, false);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: oid create fail status={} for type {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    goto fail;
  }

  for (const auto &attr_md : object_info->get_attribute_list()) {
    auto it = attrs.find(attr_md.attr_id);
    if (it == attrs.end()) {
      it = defaults.find(attr_md.attr_id);
      if (it == defaults.end()) {
        status = SWITCH_STATUS_INVALID_PARAMETER;
      } else {
        status = attr_set_on_create(object_info,
                                    object_id,
                                    *it,
                                    &attr_md,
                                    object_attrs,
                                    true,
                                    dereference);
      }
    } else {
      status = attr_set_on_create(object_info,
                                  object_id,
                                  *it,
                                  &attr_md,
                                  object_attrs,
                                  true,
                                  dereference);
    }
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: attr set fail for attrs status={} for "
                 "{}.{} attr {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id),
                 attr_md.get_attr_name());
      goto fail;
    }
  }

  status = db::object_create_with_attrs(object_id, object_attrs);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: db::object_create fail status={} for {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    goto fail;
  }
  db_success = true;

  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: object create success type {} oid {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 object_id));

  return status;

fail:
  switch_log(SWITCH_API_LEVEL_ERROR,
             object_type,
             SMI_CREATE_OPERATION,
             "{}.{}:{}: status={} {}.{}",
             __NS__,
             __func__,
             __LINE__,
             status,
             model_info->get_object_name_from_type(object_type),
             switch_store::handle_to_id(object_id));

  /* Clear db and oid if failure anywhere above */
  if (status) {
    if (object_id.data) {
      object_refs_get(dereference, object_id, false);
      for (auto const i : dereference) {
        object_ref_remove(object_id, i);
      }
    }
    if (db_success) db::object_delete(object_id);
    for (const auto &attr : attrs) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 attr.getattr());
    }
  }

  if (object_id.data != SWITCH_NULL_OBJECT_ID) oid_free(object_id);
  object_id.data = SWITCH_NULL_OBJECT_ID;
  return status;
}

switch_status_t object_delete(switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::set<switch_object_id_t> dereference;
  std::set<attr_w> attrs;
  auto object_type = object_type_query(object_id);
  bool comment_mode;

  db::object_lock(object_id);

  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: Unknown object_type {}, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }

  if (!db::object_exists(object_id)) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}:{}: Object not found ID {}",
               __NS__,
               __func__,
               __LINE__,
               handle_to_id(object_id));
    return SWITCH_STATUS_ITEM_NOT_FOUND;
  }

  const bool trigger = trigger_context.insert(object_type).second;

  if (!delete_trigger_fns_after[object_type].empty()) {
    status = attribute_get_all(object_id, attrs);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_DELETE_OPERATION,
                 "{}.{}:{}: Failed to fetch all attributes of {}.{}, for "
                 "trigger post delete failed status={}.",
                 __NS__,
                 __func__,
                 __LINE__,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id),
                 status);
      goto exit;
    }
  }

  comment_mode = record::record_comment_mode_get();
  record::record_comment_mode_set(true);
  status = attr_reevaluate_before_delete(object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: attr re-evaluate before delete fail status={} for {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    goto exit;
  }
  record::record_comment_mode_set(comment_mode);

  // reference tracking:
  status = object_ref_validate_before_delete(object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: pre-delete validation fail status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               handle_to_id(object_id));
    goto exit;
  }

  status = delete_membership(object_id, object_type);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: delete_membership fail status={} for {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
    goto exit;
  }

  comment_mode = record::record_comment_mode_get();
  record::record_comment_mode_set(true);
  if (trigger) {
    for (auto fn : delete_trigger_fns_before[object_type]) {
      status = (fn)(object_id);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_DELETE_OPERATION,
                   "{}.{}:{}: trigger pre delete fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type),
                   handle_to_id(object_id));
        goto exit;
      }
    }
  }
  record::record_comment_mode_set(comment_mode);

  status = object_refs_get(dereference, object_id, false);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: object ref get fail status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               handle_to_id(object_id));
    goto exit;
  }

  for (auto const i : dereference) {
    status = object_ref_remove(object_id, i);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_DELETE_OPERATION,
                 "{}.{}:{}: object ref remove fail status={} for {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id));
      goto exit;
    }
  }

  status = remove_keys(object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: remove keys fail status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               handle_to_id(object_id));
    goto exit;
  }

  status = db::object_delete(object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: db::object_delete fail status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               handle_to_id(object_id));
    goto exit;
  }

  status = oid_free(object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: oid_free fail status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               handle_to_id(object_id));
    goto exit;
  }

  comment_mode = record::record_comment_mode_get();
  record::record_comment_mode_set(true);
  if (trigger) {
    for (auto fn : delete_trigger_fns_after[object_type]) {
      status = (fn)(object_type, attrs);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_DELETE_OPERATION,
                   "{}.{}:{}: trigger post delete fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type),
                   handle_to_id(object_id));
        goto exit;
      }
    }
  }
  record::record_comment_mode_set(comment_mode);
  SWITCH_DEBUG_LOG(
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 object_type,
                 SMI_DELETE_OPERATION,
                 "{}.{}:{}: delete status={} {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id)));
exit:
  if (trigger) {
    const auto num_erased = trigger_context.erase(object_type);
    CHECK_RET(num_erased != 1, SWITCH_STATUS_FAILURE);
  }
  if (status) {
    db::object_unlock(object_id);
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: failure status={} for {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type));
  }

#ifdef EVENTS_ENABLED
  switch_attribute_t attr;
  memset(&attr, 0, sizeof(attr));
  smi::event::object_notify(
      SWITCH_OBJECT_EVENT_DELETE, object_type, object_id, attr, status);
#endif
  record::record_add_remove(object_id, status);
  return status;
}

switch_status_t object_delete_by_id(const switch_object_type_t object_type,
                                    const uint64_t id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t object_id = {SWITCH_NULL_OBJECT_ID};
  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: Unknown object_type {}, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }

  object_id = switch_store::id_to_handle(object_type, id);
  if (object_id.data == SWITCH_NULL_OBJECT_ID) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: Id {}"
               " is incorrect for object type {}, status {}",
               __NS__,
               __func__,
               __LINE__,
               id,
               model_info->get_object_name_from_type(object_type),
               status);
    return status;
  }

  status = object_delete(object_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_DELETE_OPERATION,
               "{}.{}:{}: Object delete failed for Id {}"
               "status {} object type {}",
               __NS__,
               __func__,
               __LINE__,
               id,
               status,
               model_info->get_object_name_from_type(object_type));
  }
  return status;
}

switch_object_id_t object_get_by_id(const switch_object_type_t object_type,
                                    const uint64_t id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t null_object_id = {SWITCH_NULL_OBJECT_ID};
  switch_object_id_t object_id = {SWITCH_NULL_OBJECT_ID};
  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_INVALID_PARAMETER;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_GET_OPERATION,
               "{}.{}:{}: Unknown object_type {}, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return null_object_id;
  }

  object_id = switch_store::id_to_handle(object_type, id);
  if (object_id.data == SWITCH_NULL_OBJECT_ID) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_GET_OPERATION,
               "{}.{}:{}: Id {} is incorrect for object type {}",
               __NS__,
               __func__,
               __LINE__,
               id,
               model_info->get_object_name_from_type(object_type));
    return null_object_id;
  }

  if (db::object_exists(object_id)) return object_id;

  return null_object_id;
}

// helper utility to create an attr from DB
switch_status_t db_value_get_wrapper(switch_object_id_t object_id,
                                     const switch_attr_id_t attr_id,
                                     attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  switch_attribute_t &attr_tmp = attr.getattr_mutable();
  status |= db::value_get(object_id, attr_id, 0, attr_tmp.value);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  if (is_composite_type(attr_tmp.value.type)) {
    // composite types..
    switch (attr_tmp.value.type) {
      case SWITCH_TYPE_LIST: {
        size_t count = attr_tmp.value.list.count;
        switch (attr_tmp.value.list.list_type) {
          case SWITCH_TYPE_BOOL: {
            std::vector<bool> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.booldata);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_UINT8: {
            std::vector<uint8_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.u8);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_UINT16: {
            std::vector<uint16_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.u16);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_UINT32: {
            std::vector<uint32_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.u32);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_UINT64: {
            std::vector<uint64_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.u64);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_INT64: {
            std::vector<int64_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.s64);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_OBJECT_ID: {
            std::vector<switch_object_id_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.oid);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_STRING: {
            std::vector<switch_string_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.text);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_IP_ADDRESS: {
            std::vector<switch_ip_address_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.ipaddr);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_IP_PREFIX: {
            std::vector<switch_ip_prefix_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.ipprefix);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_MAC: {
            std::vector<switch_mac_addr_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.mac);
            }
            attr.v_set(list);
            break;
          }
          case SWITCH_TYPE_RANGE: {
            std::vector<switch_range_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.range);
            }
            attr.v_set(list);
          } break;
          case SWITCH_TYPE_ENUM: {
            std::vector<switch_enum_t> list;
            for (size_t index = 0; index < count; index++) {
              switch_attribute_value_t value_out = {};
              status = db::value_get(object_id, attr_id, index + 1, value_out);
              list.push_back(value_out.enumdata);
            }
            attr.v_set(list);
            break;
          }
          default:
            switch_log(SWITCH_API_LEVEL_ERROR,
                       object_type_query(object_id),
                       "{}:{}: Unsupported list type {}",
                       __func__,
                       __LINE__,
                       attr_tmp.value.list.list_type);
            break;
        }
      } break;

      default:
        assert(0 && "not implemented!");
        break;
    }
  }
  return status;
}

switch_status_t attribute_set(switch_object_id_t object_id,
                              const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  attr_w get_attr(attr.id_get());
  bool comment_mode;

  if (object_id.data == 0) {
    switch_object_type_t ot =
        model_info->get_object_type_from_attr_id(attr.id_get());
    if (ot == 0) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}:{}: Invalid object ID {}, Invalid attribute ID {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 handle_to_id(object_id),
                 attr.id_get());
      return SWITCH_STATUS_INVALID_PARAMETER;
    } else {
      const ObjectInfo *object_info_tmp = model_info->get_object_info(ot);
      const AttributeMetadata *attr_md_tmp =
          object_info_tmp->get_attr_metadata(attr.id_get());
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OT_NONE,
          "{}:{}:{}: Invalid object ID {} for object_type {}, set attr {}",
          __NS__,
          __func__,
          __LINE__,
          handle_to_id(object_id),
          object_info_tmp->get_object_name(),
          attr_md_tmp->get_attr_name());
    }
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  auto const object_type = object_type_query(object_id);
  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: object_type "
               "{} not found, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}:{}: Invalid object type {}, ID {}, set attr {}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               handle_to_id(object_id),
               attr.id_get());
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  const std::vector<KeyGroup> &key_groups = object_info->get_key_groups();
  bool attr_in_key_group = false;
  if (std::any_of(key_groups.begin(), key_groups.end(), [&attr](KeyGroup kg) {
        return std::find(kg.attr_list.begin(),
                         kg.attr_list.end(),
                         attr.id_get()) != kg.attr_list.end();
      })) {
    attr_in_key_group = true;
  }

  switch_attr_id_t attr_id = attr.id_get();
  const AttributeMetadata *attr_md = object_info->get_attr_metadata(attr_id);
  if (attr_md == NULL) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               "{}:{}:{}: Failed to get attr metadata, attr_id {} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               attr_id,
               model_info->get_object_name_from_type(object_type),
               handle_to_id(object_id));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }
  switch_attr_flags_t flags = {};
  flags = attr_md->get_flags();

  /* type check against schema */
  if (attr.type_get() != attr_md->type) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_SET_OPERATION,
               "{}.{}:{}: attr type mismatch for {}.{} given={} exp={} attr {}",
               __NS__,
               __func__,
               __LINE__,
               model_info->get_object_name_from_type(object_type),
               handle_to_id(object_id),
               attr.type_get(),
               attr_md->type,
               attr.getattr());
    return SWITCH_STATUS_INVALID_PARAMETER;
  }
  if (attr.type_get() == SWITCH_TYPE_ENUM) {
    // Check given value is within enum range
    switch_enum_t enum_attr = {};
    attr.v_get(enum_attr);
    if (!attr_md->is_allowed_enum_value(enum_attr.enumdata)) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_info->object_type,
                 SMI_CREATE_OPERATION,
                 "{}.{}:{}: Invalid enum value {} for attr {} of object {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 enum_attr.enumdata,
                 attr_md->get_attr_name(),
                 object_info->get_object_name(),
                 handle_to_id(object_id));
      return SWITCH_STATUS_INVALID_PARAMETER;
    }
  }

  comment_mode = record::record_comment_mode_get();
  record::record_comment_mode_set(true);
  const bool trigger = trigger_context.insert(object_type).second;
  if (trigger) {
    for (auto fn : update_trigger_fns_before[object_type]) {
      status = (fn)(object_id, attr);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_SET_OPERATION,
                   "{}.{}:{}: trigger pre update fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type),
                   handle_to_id(object_id));
        goto exit;
      }
    }
  }
  record::record_comment_mode_set(comment_mode);

  if (trigger && attr_in_key_group) {
    /* Remove keys */
    status = remove_keys(object_id);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_SET_OPERATION,
                 "{}.{}:{}: remove keys fail status={} for {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id));
      goto exit;
    }
  }

  // get the db value in case we revert on failure later in this func
  status = db_value_get_wrapper(object_id, attr_id, get_attr);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_SET_OPERATION,
               "{}.{}:{}: db_get failed for set attr for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               model_info->get_object_name_from_type(object_type),
               handle_to_id(object_id));
    goto exit;
  }

  if (!is_composite_type(attr_md->type)) {
    if (attr_md->type == SWITCH_TYPE_OBJECT_ID) {
      // remove current reference
      const ValueMetadata *value_md = attr_md->get_value_metadata();
      switch_object_id_t oid = {0};
      status = v_get(object_id, attr.id_get(), oid);
      if (status == SWITCH_STATUS_ITEM_NOT_FOUND)
        status = SWITCH_STATUS_SUCCESS;
      if (status != SWITCH_STATUS_SUCCESS) goto exit;

      if (oid != 0 &&
          !value_md->is_allowed_object_type(object_type_query(oid))) {
        status = SWITCH_STATUS_FAILURE;
        switch_log(
            SWITCH_API_LEVEL_ERROR,
            object_type,
            SMI_SET_OPERATION,
            "{}.{}:{}: oid type mismatch given={}",
            __NS__,
            __func__,
            __LINE__,
            model_info->get_object_name_from_type(object_type_query(oid)));
        goto exit;
      }

      if (flags.re_evaluate) {
        status = reevaluate_auto_obj_pre(object_id, attr);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     object_type,
                     "{}.{}:{}: obj pre re-evaluate task fail status={} for "
                     "{}.{} for attribute {}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(object_type),
                     handle_to_id(object_id),
                     attr.getattr());
          goto exit;
        }
      }

      if (!((attr_md->get_flags()).is_internal ||
            (attr_md->get_flags()).is_read_only)) {
        status = object_ref_remove_by_attr_id(object_id, oid, attr.id_get());
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(
              SWITCH_API_LEVEL_ERROR,
              object_type,
              SMI_SET_OPERATION,
              "{}.{}:{}: obj ref remove fail status={} for {}.{} via attr:{} "
              "to {}.{}",
              __NS__,
              __func__,
              __LINE__,
              status,
              model_info->get_object_name_from_type(object_type),
              handle_to_id(object_id),
              attr_md->get_attr_name(),
              model_info->get_object_name_from_type(object_type_query(oid)),
              handle_to_id(oid));
          goto exit;
        }
      }
    }

    // set the attribute value
    status = db::value_set(object_id, attr.id_get(), 0, attr.getattr().value);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_SET_OPERATION,
                 "{}.{}:{}: db value set fail status={} for {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id));
      goto exit;
    }

    if (attr_md->type == SWITCH_TYPE_OBJECT_ID) {
      // add reference
      switch_object_id_t oid = {0};
      status |= attr.v_get(oid);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_SET_OPERATION,
                   "{}.{}:{}: attr get oid fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type),
                   handle_to_id(object_id));
        goto exit;
      }

      if (!((attr_md->get_flags()).is_internal ||
            (attr_md->get_flags()).is_read_only)) {
        status |= object_ref_add(object_id, oid, attr.id_get());
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(
              SWITCH_API_LEVEL_ERROR,
              object_type,
              SMI_SET_OPERATION,
              "{}.{}:{}: obj ref add fail status={} for {}.{} via attr:{} to "
              "{}.{}",
              __NS__,
              __func__,
              __LINE__,
              status,
              model_info->get_object_name_from_type(object_type),
              handle_to_id(object_id),
              attr_md->get_attr_name(),
              model_info->get_object_name_from_type(object_type_query(oid)),
              handle_to_id(oid));
          goto exit;
        }
      }

      if (flags.re_evaluate) {
        status = reevaluate_auto_obj_post(object_id, attr);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     object_type,
                     "{}.{}:{}: obj post re-evaluate task fail status={} for "
                     "{}.{} for attribute {}",
                     __NS__,
                     __func__,
                     __LINE__,
                     status,
                     model_info->get_object_name_from_type(object_type),
                     handle_to_id(object_id),
                     attr.getattr());
          goto exit;
        }
      }
    }
  } else {
    switch (attr_md->type) {
      case SWITCH_TYPE_LIST: {
        // pop all values
        status = list_clear(object_id, attr.id_get());
        if (status == SWITCH_STATUS_ITEM_NOT_FOUND)
          status = SWITCH_STATUS_SUCCESS;
        if (status != SWITCH_STATUS_SUCCESS) goto exit;
        status = attr_list_push(object_id, attr_md, attr);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     object_type,
                     "{}:{}: Unsupported list type status {}",
                     __func__,
                     __LINE__,
                     status);
          goto exit;
        }
      } break;
      default:
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_SET_OPERATION,
                   "{}.{}:{}: Unexpected",
                   __NS__,
                   __func__,
                   __LINE__);
        goto exit;
    }
  }

  if (trigger && attr_in_key_group) {
    /* Add keys */
    status = add_keys(object_id);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 object_type,
                 SMI_SET_OPERATION,
                 "{}.{}:{}: add keys fail status={} for {}.{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id));
      goto exit;
    }
  }

  comment_mode = record::record_comment_mode_get();
  record::record_comment_mode_set(true);
  if (trigger) {
    for (auto fn : update_trigger_fns_after[object_type]) {
      status = (fn)(object_id, attr);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_SET_OPERATION,
                   "{}.{}:{}: trigger post update fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   model_info->get_object_name_from_type(object_type),
                   handle_to_id(object_id));
        // if there is a failure within an auto object implementation revert
        // the
        // db and update keys with old value
        remove_keys(object_id);
        db::value_set(object_id, attr.id_get(), 0, get_attr.getattr().value);
        add_keys(object_id);
        // no need of more verbose logs here
        goto exit;
      }
    }
  }
  record::record_comment_mode_set(comment_mode);

exit:
  SWITCH_DETAIL_LOG(
      switch_log(SWITCH_API_LEVEL_DETAIL,
                 object_type,
                 SMI_SET_OPERATION,
                 "{}.{}:{}: status={} for {}.{} attr {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 status,
                 model_info->get_object_name_from_type(object_type),
                 handle_to_id(object_id),
                 attr.getattr()));
  if (trigger) {
    const auto num_erased = trigger_context.erase(object_type);
    CHECK_RET(num_erased != 1, SWITCH_STATUS_FAILURE);
  }
#ifdef EVENTS_ENABLED
  smi::event::object_notify(
      SWITCH_OBJECT_EVENT_SET, object_type, object_id, attr.getattr(), status);
#endif
  record::record_add_set(object_id, attr, status);
  return status;
}

// Retrieves and constructs an attribute from DB.
switch_status_t attribute_get(const switch_object_id_t object_id,
                              const switch_attr_id_t attr_id,
                              attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  attr.id_set(attr_id);

  if (object_id.data == 0) {
    switch_object_type_t ot = model_info->get_object_type_from_attr_id(attr_id);
    if (ot == 0) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}:{}: Invalid object ID {}, Invalid attribute ID {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 handle_to_id(object_id),
                 attr_id);
      return SWITCH_STATUS_INVALID_PARAMETER;
    } else {
      const ObjectInfo *object_info_tmp = model_info->get_object_info(ot);
      const AttributeMetadata *attr_md_tmp =
          object_info_tmp->get_attr_metadata(attr_id);
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OT_NONE,
          "{}:{}:{}: Invalid object ID {} for object_type {}, get attr {}",
          __NS__,
          __func__,
          __LINE__,
          handle_to_id(object_id),
          object_info_tmp->get_object_name(),
          attr_md_tmp->get_attr_name());
    }
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  status = db_value_get_wrapper(object_id, attr_id, attr);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_object_type_t ot = model_info->get_object_type_from_attr_id(attr_id);
    const ObjectInfo *object_info_tmp = model_info->get_object_info(ot);
    const AttributeMetadata *attr_md_tmp =
        object_info_tmp->get_attr_metadata(attr_id);
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        object_type_query(object_id),
        SMI_GET_OPERATION,
        "{}.{}:{}: db_get failed for attr_id {} for {}.{}",
        __NS__,
        __func__,
        __LINE__,
        attr_md_tmp->get_attr_name(),
        model_info->get_object_name_from_type(object_type_query(object_id)),
        handle_to_id(object_id));
    return status;
  }

#ifdef EVENTS_ENABLED
  smi::event::object_notify(SWITCH_OBJECT_EVENT_GET,
                            object_type_query(object_id),
                            object_id,
                            attr.getattr(),
                            status);
#endif
  record::record_add_get(object_id, attr_id, attr, status);
  return status;
}

// Use this API carefully as it is not performance optimized. This API makes
// multiple calls to the switch store DB and iterates over all the attributes
// for a given object to fetch a set of all attributes for a given object_id
switch_status_t attribute_get_all(const switch_object_id_t object_id,
                                  std::set<attr_w> &attrs) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const ObjectInfo *object_info =
      model_info->get_object_info(object_type_query(object_id));
  if (!object_info) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type_query(object_id),
               SMI_GET_OPERATION,
               "{}:{}:{}: Object type {} invalid for Object ID {:#x}, ",
               __NS__,
               __func__,
               __LINE__,
               object_type_query(object_id),
               object_id.data);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }
  for (const auto &attr_md : object_info->get_attribute_list()) {
    attr_w attr{attr_md.attr_id};
    status = attribute_get(object_id, attr_md.attr_id, attr);
    CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
    attrs.insert(attr);
  }
  return status;
}

// Only for perf optimizations. Does not support returning lists.
switch_status_t attribute_get_all(
    const switch_object_id_t object_id,
    std::vector<std::reference_wrapper<const switch_attribute_t>> &value_out) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (object_id.data == 0) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_GET_OPERATION,
               "{}.{}:{}: invalid oid=0 for attr get all",
               __NS__,
               __func__,
               __LINE__);
    return SWITCH_STATUS_FAILURE;
  }

  status = db::value_get_all(object_id, value_out);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        object_type_query(object_id),
        SMI_GET_OPERATION,
        "{}.{}:{}: attribute get all failed for {}.{}",
        __NS__,
        __func__,
        __LINE__,
        model_info->get_object_name_from_type(object_type_query(object_id)),
        handle_to_id(object_id));
    return status;
  }

  return status;
}

switch_status_t object_id_get_wkey_internal(switch_object_type_t object_type,
                                            std::set<attr_w> attrs,
                                            switch_object_id_t &object_id) {
  return get_wkey(object_type, attrs, object_id);
}

switch_status_t object_counters_get_internal(
    switch_object_id_t object_id, std::vector<switch_counter_t> &cntrs) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  auto object_type = object_type_query(object_id);
  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: object_type "
               "{} not found, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info == nullptr) return SWITCH_STATUS_FAILURE;

  if (!object_info->get_counter()) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_GET_OPERATION,
               "{}.{}:{}: object does not support counters status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               object_info->get_object_name(),
               handle_to_id(object_id));
    return SWITCH_STATUS_FAILURE;
  }

  const bool trigger = trigger_context.insert(object_type).second;
  if (trigger) {
    for (auto fn : counter_get_trigger_fns[object_type]) {
      status = (fn)(object_id, cntrs);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: trigger counter get fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   object_info->get_object_name(),
                   handle_to_id(object_id));
        goto exit;
      }
    }
  }

exit:
  if (trigger) {
    trigger_context.erase(object_type);
  }
  return status;
}

switch_status_t object_counters_clear_internal(
    switch_object_id_t object_id, const std::vector<uint16_t> &cntrs_ids) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  auto object_type = object_type_query(object_id);
  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: object_type "
               "{} not found, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info == nullptr) return SWITCH_STATUS_FAILURE;

  if (!object_info->get_counter()) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_GET_OPERATION,
               "{}.{}:{}: object does not support counters status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               object_info->get_object_name(),
               handle_to_id(object_id));
    return SWITCH_STATUS_FAILURE;
  }

  const bool trigger = trigger_context.insert(object_type).second;
  if (trigger) {
    for (auto fn : counters_set_trigger_fns[object_type]) {
      status = (fn)(object_id, cntrs_ids);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   object_type,
                   SMI_CREATE_OPERATION,
                   "{}.{}:{}: trigger counters set fail status={} for {}.{}",
                   __NS__,
                   __func__,
                   __LINE__,
                   status,
                   object_info->get_object_name(),
                   handle_to_id(object_id));
        goto exit;
      }
    }
  }

exit:
  if (trigger) {
    trigger_context.erase(object_type);
  }
  return status;
}

switch_status_t object_counters_clear_all_internal(
    switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  auto object_type = object_type_query(object_id);
  if (!object_type_valid(object_type)) {
    status = SWITCH_STATUS_ITEM_NOT_FOUND;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               SMI_CREATE_OPERATION,
               "{}.{}:{}: object_type "
               "{} not found, status={}",
               __NS__,
               __func__,
               __LINE__,
               object_type,
               status);
    return status;
  }
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info == nullptr) return SWITCH_STATUS_FAILURE;

  if (!object_info->get_counter()) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               object_type,
               SMI_GET_OPERATION,
               "{}.{}:{}: object does not support counters status={} for {}.{}",
               __NS__,
               __func__,
               __LINE__,
               status,
               object_info->get_object_name(),
               handle_to_id(object_id));
    return SWITCH_STATUS_FAILURE;
  }

  const bool trigger = trigger_context.insert(object_type).second;
  if (trigger) {
    for (auto fn : all_counters_set_trigger_fns[object_type]) {
      status = (fn)(object_id);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(
            SWITCH_API_LEVEL_ERROR,
            object_type,
            SMI_CREATE_OPERATION,
            "{}.{}:{}: trigger all counters set fail status={} for {}.{}",
            __NS__,
            __func__,
            __LINE__,
            status,
            object_info->get_object_name(),
            handle_to_id(object_id));
        goto exit;
      }
    }
  }

exit:
  if (trigger) {
    trigger_context.erase(object_type);
  }
  return status;
}

switch_status_t object_get_first_handle(switch_object_type_t object_type,
                                        switch_object_id_t &object_handle) {
  /* get first object handle */
  LOCK_GUARD(object_type, oid_get_first_handle(object_type, object_handle));
}

switch_status_t object_get_next_handles(
    switch_object_id_t &cur_object_handle,
    uint32_t num_handles,
    std::vector<switch_object_id_t> &next_object_handles,
    uint32_t &out_num_ids) {
  /* get next object handles */
  LOCK_GUARD(
      object_type_query(cur_object_handle),
      oid_get_next_handles(
          cur_object_handle, num_handles, next_object_handles, out_num_ids));
}

switch_status_t object_get_all_handles(
    switch_object_type_t object_type,
    std::vector<switch_object_id_t> &object_handles) {
  /* get all object handles of ot object_type */
  LOCK_GUARD(object_type, oid_get_all_handles(object_type, object_handles));
}

/*
switch_status_t object_create(const switch_object_type_t object_type,
                              const std::set<attr_w> &attrs,
                              switch_object_id_t &object_id) {
  LOCK_GUARD(object_type,
             object_create_internal(object_type, attrs, object_id));
}

switch_status_t object_delete(const switch_object_id_t object_id) {
  LOCK_GUARD(object_type_query(object_id), object_delete_internal(object_id));
}

switch_status_t attribute_set(const switch_object_id_t object_id,
                              const attr_w &attr) {
  LOCK_GUARD(object_type_query(object_id),
             attribute_set_internal(object_id, attr));
}

switch_status_t attribute_get(const switch_object_id_t object_id,
                              const switch_attr_id_t attr_id,
                              attr_w &attr) {
  LOCK_GUARD(object_type_query(object_id),
             attribute_get_internal(object_id, attr_id, attr));
}
*/

switch_status_t object_id_get_wkey(switch_object_type_t object_type,
                                   std::set<attr_w> attrs,
                                   switch_object_id_t &object_id) {
  LOCK_GUARD(object_type,
             object_id_get_wkey_internal(object_type, attrs, object_id));
}

switch_status_t object_counters_get(switch_object_id_t object_id,
                                    std::vector<switch_counter_t> &cntrs) {
  LOCK_GUARD(object_type_query(object_id),
             object_counters_get_internal(object_id, cntrs));
}

switch_status_t object_counters_clear(switch_object_id_t object_id,
                                      const std::vector<uint16_t> &cntrs_ids) {
  LOCK_GUARD(object_type_query(object_id),
             object_counters_clear_internal(object_id, cntrs_ids));
}

switch_status_t object_counters_clear_all(switch_object_id_t object_id) {
  LOCK_GUARD(object_type_query(object_id),
             object_counters_clear_all_internal(object_id));
}

bool is_object_type_valid(switch_object_type_t object_type) {
  return object_type_valid(object_type);
}

std::string object_name_get_from_object(switch_object_id_t object_id) {
  return model_info->get_object_name_from_type(object_type_query(object_id));
}

std::string object_name_get_from_type(switch_object_type_t object_type) {
  return model_info->get_object_name_from_type(object_type);
}

} /* namespace switch_store */
} /* namespace smi */
