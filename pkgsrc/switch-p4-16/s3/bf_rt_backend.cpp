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


#include "s3/bf_rt_backend.h"

#include <arpa/inet.h>

#include <vector>
#include <set>
#include <unordered_map>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <sstream>
#include <algorithm>

#include "s3/switch_store.h"
#include "s3/attribute_util.h"
#include "./log.h"

extern "C" {
#include "tofino/bf_pal/pltfm_intf.h"
#include "tofino/pdfixed/pd_common.h"
}
namespace smi {
namespace bf_rt {
#define __NS__ "bf_rt_backend"
using ::smi::logging::switch_log;
using namespace ::bfrt;  // NOLINT(build/namespaces)
namespace {
std::string tableNameGetInternal(const BfRtTable *table) {
  std::string name;
  table->tableNameGet(&name);
  return name;
}
std::string keyFieldNameGetInternal(const BfRtTable *table,
                                    bf_rt_id_t field_id) {
  std::string name;
  table->keyFieldNameGet(field_id, &name);
  return name;
}
std::string dataFieldNameGetInternal(const BfRtTable *table,
                                     bf_rt_id_t field_id,
                                     bf_rt_id_t action_id,
                                     bool indirect) {
  std::string name;
  if (indirect)
    table->dataFieldNameGet(field_id, &name);
  else
    table->dataFieldNameGet(field_id, action_id, &name);
  return name;
}
}  // namespace

const BfRtInfo *bfrtinfo = NULL;

const BfRtInfo *get_bf_rt_info() { return bfrtinfo; }

std::shared_ptr<BfRtSession> session(nullptr);
const BfRtSession &get_bf_rt_session() { return *session; }
const std::shared_ptr<BfRtSession> get_bf_rt_session_ptr() { return session; }

const bf_rt_target_t global_dev_tgt = {.dev_id = 0,
                                       .pipe_id = BF_DEV_PIPE_ALL};  // 0xFFFF
bf_rt_target_t get_dev_tgt() { return global_dev_tgt; }

p4_pd_sess_hdl_t g_mc_sess_hdl = 0;
p4_pd_sess_hdl_t get_mc_sess_hdl() { return g_mc_sess_hdl; }

BfRtTable::BfRtTableGetFlag table_get_flag =
    BfRtTable::BfRtTableGetFlag::GET_FROM_SW;

std::set<bf_dev_pipe_t> active_pipes;
const std::set<bf_dev_pipe_t> get_active_pipes() { return active_pipes; }

std::vector<bfrtCacheObject> &default_entry_cache =
    bfrtCache::cache().default_entry_cache();
std::vector<bfrtCacheObject> &table_cache = bfrtCache::cache().table_cache();
std::unordered_map<uint64_t, bfrtCacheObject> &p4_match_action_cache =
    bfrtCache::cache().p4_match_action_cache();
std::unordered_map<uint64_t, std::vector<bfrtCacheObject>>
    &p4_match_action_list_cache =
        bfrtCache::cache().p4_match_action_list_cache();
std::unordered_map<uint64_t, bfrtCacheObject> &p4_selector_cache =
    bfrtCache::cache().p4_selector_cache();
std::unordered_map<uint64_t, std::vector<bfrtCacheObject>>
    &p4_selector_list_cache = bfrtCache::cache().p4_selector_list_cache();
std::unordered_map<uint64_t, bfrtCacheObject> &p4_selector_group_cache =
    bfrtCache::cache().p4_selector_group_cache();
std::unordered_map<uint8_t, bfrtCacheObject> &traffic_class_cache =
    bfrtCache::cache().traffic_class_cache();
std::vector<bfrtCacheObject> &pd_fixed_cache =
    bfrtCache::cache().pd_fixed_cache();

// std::unordered_map<uint8_t, bfrtCacheObject> &get_tc_cache() { return
// traffic_class_cache; }

#define SMI_WARM_INIT switch_store::smiContext::context().in_warm_init()

std::vector<std::string> tokenize(const std::string &str, const char token) {
  std::string tmp;
  std::vector<std::string> ret;
  std::istringstream iss(str);

  while (getline(iss, tmp, token)) ret.push_back(tmp);

  return ret;
}

switch_status_t switch_bf_rt_init() {
  bf_status_t bf_status = BF_SUCCESS;
  bool sw_model = false;
  PipelineProfInfoVec pipe_vector;
  BfRtDevMgr &bfrtdevmgr = BfRtDevMgr::getInstance();
  bf_status = bfrtdevmgr.bfRtInfoGet(0, "switch", &bfrtinfo);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to get bfrtinfo",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return bf_rt_status_xlate(bf_status);
  }

  bf_status = bfrtinfo->bfRtInfoPipelineInfoGet(&pipe_vector);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to get bfrtinfoPipelineInfo",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return bf_rt_status_xlate(bf_status);
  }
  for (auto pipe : pipe_vector) {
    std::string pipeline_name = pipe.first;
    std::vector<bf_dev_pipe_t> pipe_vec = pipe.second;
    for (auto p : pipe_vec) {
      active_pipes.insert(p);
    }
  }

  bf_status = bf_pal_pltfm_type_get(global_dev_tgt.dev_id, &sw_model);
  if (sw_model) table_get_flag = BfRtTable::BfRtTableGetFlag::GET_FROM_HW;

  session = BfRtSession::sessionCreate();

  g_mc_sess_hdl = session->preSessHandleGet();
  if (g_mc_sess_hdl == 0) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: Invalid mc sess handle from bfrt session",
               __NS__,
               __func__,
               __LINE__);
    return SWITCH_STATUS_FAILURE;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t switch_bf_rt_clean() {
  // bf_status_t bf_status = BF_SUCCESS;
  active_pipes.clear();
  // TODO(bfn) disable until we figure how to cleanly stop the stats sync thread
  // bf_status = session->sessionDestroy();
  // return bf_rt_status_xlate(bf_status);
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t switch_bf_rt_flush() {
  switch_status_t rc = SWITCH_STATUS_SUCCESS;

  // flush default mau and stateful entries
  for (const auto &object : table_cache) {
    if (object) {
      rc = object->flush();
      if (rc != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}.{}:{}: status: {} failed to flush table_cache",
                   __NS__,
                   __func__,
                   __LINE__,
                   rc);
        // return bf_rt_status_xlate(rc);
      }
    }
  }
  // flush selector and action profile tables
  for (auto const &entry : p4_selector_cache) {
    if (entry.second) {
      rc = entry.second->flush();
      if (rc != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}.{}:{}: status: {} failed to flush p4_selector_cache",
                   __NS__,
                   __func__,
                   __LINE__,
                   rc);
        // return bf_rt_status_xlate(rc);
      }
    }
  }
  for (auto const &entry : p4_selector_list_cache) {
    for (const auto &object : entry.second) {
      if (object) {
        rc = object->flush();
        if (rc != SWITCH_STATUS_SUCCESS) {
          switch_log(
              SWITCH_API_LEVEL_ERROR,
              SWITCH_OT_NONE,
              "{}.{}:{}: status: {} failed to flush p4_selector_list_cache",
              __NS__,
              __func__,
              __LINE__,
              rc);
          // return bf_rt_status_xlate(rc);
        }
      }
    }
  }
  // flush selector group tables
  for (auto const &entry : p4_selector_group_cache) {
    if (entry.second) {
      rc = entry.second->flush();
      if (rc != SWITCH_STATUS_SUCCESS) {
        switch_log(
            SWITCH_API_LEVEL_ERROR,
            SWITCH_OT_NONE,
            "{}.{}:{}: status: {} failed to flush p4_selector_group_cache",
            __NS__,
            __func__,
            __LINE__,
            rc);
        // return bf_rt_status_xlate(rc);
      }
    }
  }
  // flush match action PD fixed tables
  for (auto const &object : pd_fixed_cache) {
    if (object) {
      rc = object->flush();
      if (rc != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}.{}:{}: status: {} failed to flush pd_fixed_cache",
                   __NS__,
                   __func__,
                   __LINE__,
                   rc);
        // return bf_rt_status_xlate(rc);
      }
    }
  }
  // flush match action direct tables
  for (auto const &entry : p4_match_action_cache) {
    if (entry.second) {
      rc = entry.second->flush();
      if (rc != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}.{}:{}: status: {} failed to flush p4_match_action_cache",
                   __NS__,
                   __func__,
                   __LINE__,
                   rc);
        // return bf_rt_status_xlate(rc);
      }
    }
  }
  for (auto const &entry : p4_match_action_list_cache) {
    for (const auto &object : entry.second) {
      if (object) {
        rc = object->flush();
        if (rc != SWITCH_STATUS_SUCCESS) {
          switch_log(
              SWITCH_API_LEVEL_ERROR,
              SWITCH_OT_NONE,
              "{}.{}:{}: status: {} failed to flush p4_match_action_list_cache",
              __NS__,
              __func__,
              __LINE__,
              rc);
          // return bf_rt_status_xlate(rc);
        }
      }
    }
  }
  // Flush tc entries (one per tc)
  for (auto const &entry : traffic_class_cache) {
    if (entry.second) {
      rc = entry.second->flush();
      if (rc != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}.{}:{}: status: {} failed to flush traffic_class_cache",
                   __NS__,
                   __func__,
                   __LINE__,
                   rc);
        // return bf_rt_status_xlate(rc);
      }
    }
  }
  // flush table default entries
  for (const auto &object : default_entry_cache) {
    if (object) {
      rc = object->defaultEntryFlush();
      if (rc != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}.{}:{}: status: {} failed to flush default_entry_cache",
                   __NS__,
                   __func__,
                   __LINE__,
                   rc);
        // return bf_rt_status_xlate(rc);
      }
    }
  }
  table_cache.clear();
  p4_selector_cache.clear();
  p4_selector_list_cache.clear();
  p4_selector_group_cache.clear();
  pd_fixed_cache.clear();
  p4_match_action_cache.clear();
  p4_match_action_list_cache.clear();
  traffic_class_cache.clear();
  default_entry_cache.clear();
  return bf_rt_status_xlate(rc);
}

switch_status_t start_transaction() {
  bf_status_t bf_status = BF_SUCCESS;

  if (!session) return SWITCH_STATUS_FAILURE;

  bf_status = session->beginTransaction(true);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to start transaction",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return bf_rt_status_xlate(bf_status);
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t commit_transaction() {
  bf_status_t bf_status = BF_SUCCESS;

  if (!session) return SWITCH_STATUS_FAILURE;

  bf_status = session->commitTransaction(true);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to commit transaction",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return bf_rt_status_xlate(bf_status);
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t start_batch() {
  bf_status_t bf_status = BF_SUCCESS;

  if (!session) return SWITCH_STATUS_FAILURE;

  bf_status = session->beginBatch();
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to start batching",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return bf_rt_status_xlate(bf_status);
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t end_batch() {
  bf_status_t bf_status = BF_SUCCESS;

  if (!session) return SWITCH_STATUS_FAILURE;

  bf_status = session->endBatch(true);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to end batching",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return bf_rt_status_xlate(bf_status);
  }

  return SWITCH_STATUS_SUCCESS;
}

/******************************************************************************
 * Batch operations for user created sessions
 *****************************************************************************/
switch_status_t start_batch(std::shared_ptr<BfRtSession> user_session) {
  bf_status_t bf_status = BF_SUCCESS;

  if (!user_session) return SWITCH_STATUS_FAILURE;

  bf_status = user_session->beginBatch();
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to start batching",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return bf_rt_status_xlate(bf_status);
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t end_batch(std::shared_ptr<BfRtSession> user_session) {
  bf_status_t bf_status = BF_SUCCESS;

  if (!user_session) return SWITCH_STATUS_FAILURE;

  bf_status = user_session->endBatch(true);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to end batching",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return bf_rt_status_xlate(bf_status);
  }

  return SWITCH_STATUS_SUCCESS;
}

/******************************************************************************
 * MatchKey
 *****************************************************************************/
_MatchKey::_MatchKey(bf_rt_table_id_t table_id)
    : _table_id(table_id), table_key(nullptr) {
  bf_status_t rc = BF_SUCCESS;
  if (table_id == 0) return;

  rc = (get_bf_rt_info())->bfrtTableFromIdGet(table_id, &table);
  if (!table) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed bfrtTableFromIdGet: {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               table_id);
    return;
  }
  rc = table->keyAllocate(&table_key);
  if (!table_key) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed keyAllocate: {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               table_id);
    return;
  }
}

_MatchKey::_MatchKey(const _MatchKey &other)
    : _table_id(other._table_id), table_key(nullptr) {}

switch_status_t _MatchKey::get_exact(bf_rt_field_id_t field_id,
                                     uint64_t *key) const {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) return status;

  rc = table_key->getValue(field_id, key);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed getValue: \"{}.{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               keyFieldNameGetInternal(table, field_id));
    return bf_rt_status_xlate(rc);
  }

  return status;
}

template <typename T>
switch_status_t _MatchKey::set_exact(bf_rt_field_id_t field_id, T key) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  rc = table_key->setValue(field_id, key);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValue: \"{}.{}\" value {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               keyFieldNameGetInternal(table, field_id),
               key);
    return bf_rt_status_xlate(rc);
  }

  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "exact: {} -> {}",
                              keyFieldNameGetInternal(table, field_id),
                              key));

  return status;
}

template switch_status_t _MatchKey::set_exact<uint8_t>(bf_rt_field_id_t mf_id,
                                                       uint8_t key);
template switch_status_t _MatchKey::set_exact<uint16_t>(bf_rt_field_id_t mf_id,
                                                        uint16_t key);
template switch_status_t _MatchKey::set_exact<uint32_t>(bf_rt_field_id_t mf_id,
                                                        uint32_t key);
template switch_status_t _MatchKey::set_exact<uint64_t>(bf_rt_field_id_t mf_id,
                                                        uint64_t key);
template switch_status_t _MatchKey::set_exact<std::string>(
    bf_rt_field_id_t mf_id, std::string key);

template <>
switch_status_t _MatchKey::set_exact(bf_rt_field_id_t mf_id, bool key) {
  return (set_exact<uint8_t>(mf_id, key));
}

template <>
switch_status_t _MatchKey::set_exact(bf_rt_field_id_t mf_id,
                                     switch_object_id_t oid) {
  return (set_exact<uint16_t>(mf_id, static_cast<uint16_t>(oid.data)));
}

switch_status_t _MatchKey::set_exact(bf_rt_field_id_t field_id,
                                     const char *key,
                                     size_t sz) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  rc = table_key->setValue(field_id, (unsigned char *)key, sz);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValue: \"{}.{}\" value {} sz {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               keyFieldNameGetInternal(table, field_id),
               key,
               sz);
    return bf_rt_status_xlate(rc);
  }

  return status;
}

switch_status_t _MatchKey::set_exact(const bf_rt_field_id_t mf_id,
                                     const switch_object_id_t oid,
                                     const switch_attr_id_t attr_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const char *v_ptr = NULL;
  size_t sz = 0;
  switch_ip4_t ip4 = 0;

  if (!table) return status;
  if (!table_key) return status;
  if (!mf_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               mf_id,
               tableNameGetInternal(table));
    return status;
  }

  attr_w m_attr(attr_id);
  status = switch_store::attribute_get(oid, attr_id, m_attr);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} attribute_get failed mf_id {} oid {:#x} "
               "attr_id {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               mf_id,
               oid.data,
               attr_id);
    return SWITCH_STATUS_FAILURE;
  }

  auto const type = m_attr.type_get();
  const switch_attribute_t &attr = m_attr.getattr();

  switch (type) {
    case SWITCH_TYPE_BOOL:
      return set_exact(mf_id, attr.value.booldata);
    case SWITCH_TYPE_UINT8:
      return set_exact(mf_id, attr.value.u8);
    case SWITCH_TYPE_UINT16:
      return set_exact(mf_id, attr.value.u16);
    case SWITCH_TYPE_UINT32:
      return set_exact(mf_id, attr.value.u32);
    case SWITCH_TYPE_UINT64:
      return set_exact(mf_id, attr.value.u64);
    case SWITCH_TYPE_ENUM:
      return set_exact(mf_id, attr.value.enumdata.enumdata);
    case SWITCH_TYPE_MAC:
      v_ptr = reinterpret_cast<const char *>(&attr.value.mac.mac);
      sz = sizeof(attr.value.mac.mac);
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 SWITCH_OT_NONE,
                 "exact: MAC -> {}",
                 attr.value.mac);
      return set_exact(mf_id, v_ptr, sz);
    case SWITCH_TYPE_OBJECT_ID:
      return set_exact(mf_id, attr.value.oid);
    case SWITCH_TYPE_IP_ADDRESS:
      if (attr.value.ipaddr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV4) {
        ip4 = htonl(attr.value.ipaddr.ip4);
        v_ptr = reinterpret_cast<const char *>(&ip4);
        sz = sizeof(attr.value.ipaddr.ip4);
      } else if (attr.value.ipaddr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV6) {
        v_ptr = reinterpret_cast<const char *>(&attr.value.ipaddr.ip6);
        sz = sizeof(attr.value.ipaddr.ip6);
      } else {
        return SWITCH_STATUS_SUCCESS;
      }
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 SWITCH_OT_NONE,
                 "exact: IP -> {}",
                 attr.value.ipaddr);
      return set_exact(mf_id, v_ptr, sz);
    case SWITCH_TYPE_IP_PREFIX:
      if (attr.value.ipprefix.addr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV4) {
        ip4 = htonl(attr.value.ipprefix.addr.ip4);
        v_ptr = reinterpret_cast<const char *>(&ip4);
        sz = sizeof(attr.value.ipprefix.addr.ip4);
      } else if (attr.value.ipprefix.addr.addr_family ==
                 SWITCH_IP_ADDR_FAMILY_IPV6) {
        v_ptr = reinterpret_cast<const char *>(&attr.value.ipprefix.addr.ip6);
        sz = sizeof(attr.value.ipprefix.addr.ip6);
      } else {
        return SWITCH_STATUS_SUCCESS;
      }
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 SWITCH_OT_NONE,
                 "exact: IP -> {}",
                 attr.value.ipprefix.addr);
      return set_exact(mf_id, v_ptr, sz);
    default:
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}.{}:{}: failed invalid attr type: {} {}",
                 __NS__,
                 __func__,
                 __LINE__,
                 type,
                 attr);
      status = SWITCH_STATUS_INVALID_PARAMETER;
  }
  return status;
}

template <>
switch_status_t _MatchKey::set_exact(bf_rt_field_id_t field_id,
                                     const switch_mac_addr_t addr) {
  const char *addr_ptr = NULL;
  size_t sz = 0;
  addr_ptr = reinterpret_cast<const char *>(&addr.mac);
  sz = sizeof(addr.mac);
  switch_log(SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "MAC-> {}", addr);
  return set_exact(field_id, addr_ptr, sz);
}

template <typename T>
switch_status_t _MatchKey::set_ternary(bf_rt_field_id_t field_id,
                                       T key,
                                       T mask) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  rc = table_key->setValueandMask(field_id, key, mask);
  if (rc != BF_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OT_NONE,
        "{}.{}:{}: rc: {} failed setValueandMask: \"{}.{}\" value {} mask {}",
        __NS__,
        __func__,
        __LINE__,
        bf_err_str(rc),
        tableNameGetInternal(table),
        keyFieldNameGetInternal(table, field_id),
        key,
        mask);
    return bf_rt_status_xlate(rc);
  }

  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "ternary: {} -> {}/{}",
                              keyFieldNameGetInternal(table, field_id),
                              key,
                              mask));
  return status;
}

template switch_status_t _MatchKey::set_ternary<bool>(bf_rt_field_id_t mf_id,
                                                      bool key,
                                                      bool mask);
template switch_status_t _MatchKey::set_ternary<uint8_t>(bf_rt_field_id_t mf_id,
                                                         uint8_t key,
                                                         uint8_t mask);
template switch_status_t _MatchKey::set_ternary<uint16_t>(
    bf_rt_field_id_t mf_id, uint16_t key, uint16_t mask);
template switch_status_t _MatchKey::set_ternary<uint32_t>(
    bf_rt_field_id_t mf_id, uint32_t key, uint32_t mask);
template switch_status_t _MatchKey::set_ternary<uint64_t>(
    bf_rt_field_id_t mf_id, uint64_t key, uint64_t mask);

switch_status_t _MatchKey::set_ternary(bf_rt_field_id_t field_id,
                                       const char *key,
                                       const char *mask,
                                       size_t sz) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  std::string name;
  table->keyFieldNameGet(field_id, &name);

  rc = table_key->setValueandMask(
      field_id, (unsigned char *)key, (unsigned char *)mask, sz);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValueandMask: \"{}.{}\" value {} "
               "mask {} sz {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               keyFieldNameGetInternal(table, field_id),
               key,
               mask,
               sz);
    return bf_rt_status_xlate(rc);
  }

  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "ternary: {} -> {}/{} sz {}",
                              keyFieldNameGetInternal(table, field_id),
                              key,
                              mask,
                              sz));
  return status;
}

switch_status_t _MatchKey::set_ternary(bf_rt_field_id_t field_id,
                                       const switch_mac_addr_t addr,
                                       const switch_mac_addr_t mask) {
  const char *addr_ptr = NULL, *mask_ptr = NULL;
  size_t sz = 0;
  addr_ptr = reinterpret_cast<const char *>(&addr.mac);
  mask_ptr = reinterpret_cast<const char *>(&mask.mac);
  sz = sizeof(addr.mac);
  switch_log(
      SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "MAC/MASK -> {}/{}", addr, mask);
  return set_ternary(field_id, addr_ptr, mask_ptr, sz);
}

switch_status_t _MatchKey::set_ternary(bf_rt_field_id_t field_id,
                                       const switch_ip_address_t addr,
                                       const switch_ip_address_t mask) {
  const char *addr_ptr = NULL, *mask_ptr = NULL;
  size_t sz = 0;
  uint32_t ip4 = 0, ip4_mask = 0;
  if (addr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV4) {
    ip4 = htonl(addr.ip4);
    ip4_mask = htonl(mask.ip4);
    addr_ptr = reinterpret_cast<const char *>(&ip4);
    mask_ptr = reinterpret_cast<const char *>(&ip4_mask);
    sz = sizeof(switch_ip4_t);
  } else if (addr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV6) {
    addr_ptr = reinterpret_cast<const char *>(&addr.ip6);
    mask_ptr = reinterpret_cast<const char *>(&mask.ip6);
    sz = sizeof(switch_ip6_t);
  } else {
    return SWITCH_STATUS_SUCCESS;
  }
  switch_log(
      SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "IP/MASK -> {}/{}", addr, mask);
  return set_ternary(field_id, addr_ptr, mask_ptr, sz);
}

switch_status_t _MatchKey::set_ip_unified_ternary(
    bf_rt_field_id_t field_id,
    const switch_ip_address_t addr,
    const switch_ip_address_t mask) {
  const char *addr_ptr = NULL, *mask_ptr = NULL;
  size_t sz = 0;
  switch_ip6_t ip6 = {}, ip6_mask = {};
  if (addr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV4) {
    // copy v4 address to bits [95:64] of v6 address and zero out rest of the v6
    // address
    for (int i = 0; i < 4; i++) {
      ip6[i] = ip6_mask[i] = 0;
    }
    for (int i = 8; i > 4; i--) {
      ip6[i - 1] = ((addr.ip4 >> ((8 - i) * 8)) & 0xff);
      ip6_mask[i - 1] = ((mask.ip4 >> ((8 - i) * 8)) & 0xff);
    }
    for (int i = 8; i < IPV6_LEN; i++) {
      ip6[i] = ip6_mask[i] = 0;
    }
    addr_ptr = reinterpret_cast<const char *>(&ip6);
    mask_ptr = reinterpret_cast<const char *>(&ip6_mask);
  } else if (addr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV6) {
    addr_ptr = reinterpret_cast<const char *>(&addr.ip6);
    mask_ptr = reinterpret_cast<const char *>(&mask.ip6);
  } else {
    return SWITCH_STATUS_SUCCESS;
  }
  sz = sizeof(switch_ip6_t);
  switch_log(
      SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "IP/MASK -> {}/{}", addr, mask);
  return set_ternary(field_id, addr_ptr, mask_ptr, sz);
}

template <typename T>
switch_status_t _MatchKey::get_ternary(bf_rt_field_id_t field_id,
                                       T *key,
                                       T *mask) const {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) return status;

  rc = table_key->getValueandMask(field_id, 1, key, mask);
  if (rc != BF_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OT_NONE,
        "{}.{}:{}: rc: {} failed getValueandMask: \"{}.{}\" value {} mask {}",
        __NS__,
        __func__,
        __LINE__,
        bf_err_str(rc),
        tableNameGetInternal(table),
        keyFieldNameGetInternal(table, field_id),
        key,
        mask);
    return bf_rt_status_xlate(rc);
  }

  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "get_ternary: {} -> {}/{}",
                              keyFieldNameGetInternal(table, field_id),
                              key,
                              mask));
  return status;
}

template switch_status_t _MatchKey::get_ternary<uint8_t>(bf_rt_field_id_t mf_id,
                                                         uint8_t *key,
                                                         uint8_t *mask) const;

template <typename T>
switch_status_t _MatchKey::set_lpm(bf_rt_field_id_t field_id,
                                   T key,
                                   size_t len) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) return status;

  rc = table_key->setValueLpm(field_id, key, len);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValueLpm: \"{}.{}\" value {} len {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               keyFieldNameGetInternal(table, field_id),
               key,
               len);
    return bf_rt_status_xlate(rc);
  }

  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "lpm: {} -> key {} len {}",
                              keyFieldNameGetInternal(table, field_id),
                              key,
                              len));
  return status;
}

template switch_status_t _MatchKey::set_lpm<uint8_t>(bf_rt_field_id_t mf_id,
                                                     uint8_t key,
                                                     size_t len);
template switch_status_t _MatchKey::set_lpm<uint16_t>(bf_rt_field_id_t mf_id,
                                                      uint16_t key,
                                                      size_t len);
template switch_status_t _MatchKey::set_lpm<uint32_t>(bf_rt_field_id_t mf_id,
                                                      uint32_t key,
                                                      size_t len);

switch_status_t _MatchKey::set_lpm(bf_rt_field_id_t field_id,
                                   const char *key,
                                   size_t sz,
                                   size_t len) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  rc = table_key->setValueLpm(field_id, (unsigned char *)key, len, sz);
  if (rc != BF_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OT_NONE,
        "{}.{}:{}: rc: {} failed setValueLpm: \"{}.{}\" value {} len {} sz {}",
        __NS__,
        __func__,
        __LINE__,
        bf_err_str(rc),
        tableNameGetInternal(table),
        keyFieldNameGetInternal(table, field_id),
        key,
        len,
        sz);
    return bf_rt_status_xlate(rc);
  }

  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "lpm: {} -> key {} len {} sz {}",
                              keyFieldNameGetInternal(table, field_id),
                              key,
                              len,
                              sz));
  return status;
}

switch_status_t _MatchKey::set_range(const bf_rt_field_id_t field_id,
                                     const switch_range_t range) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;

  if (!table) return status;
  if (table_key == nullptr) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  rc = table_key->setValueRange(field_id,
                                static_cast<uint64_t>(range.min),
                                static_cast<uint64_t>(range.max));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValueRange: \"{}.{}\" min {} max {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               keyFieldNameGetInternal(table, field_id),
               range.min,
               range.max);
    return bf_rt_status_xlate(rc);
  }

  return status;
}

/******************************************************************************
 * ActionEntry
 *****************************************************************************/
_ActionEntry::_ActionEntry(bf_rt_table_id_t table_id)
    : _table_id(table_id), table_data(nullptr) {
  if (table_id == 0) return;
  (get_bf_rt_info())->bfrtTableFromIdGet(table_id, &table);
  if (!table) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: failed bfrtTableFromIdGet: {}",
               __NS__,
               __func__,
               __LINE__,
               table_id);
    return;
  }
}

_ActionEntry::_ActionEntry(const _ActionEntry &other)
    : _table_id(other._table_id), table_data(nullptr) {}

void _ActionEntry::init_action_data(bf_rt_action_id_t act_id) {
  bf_status_t rc = BF_SUCCESS;
  if (!table) return;
  if (_table_id == 0) return;
  if (act_id == 0) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to init invalid action {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               act_id,
               tableNameGetInternal(table));
    return;
  }

  rc = table->dataAllocate(act_id, &table_data);
  if (!table_data) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed dataAllocate table \"{}\" action {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               act_id);
    return;
  }
  _action_id = act_id;
  init_status = true;

  std::string name;
  table->actionNameGet(act_id, &name);
  switch_log(SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "action: {}", name);
}

void _ActionEntry::init_action_data(bf_rt_action_id_t act_id,
                                    std::vector<bf_rt_id_t> &fields) {
  bf_status_t rc = BF_SUCCESS;
  if (!table) return;
  if (_table_id == 0) return;
  if (act_id == 0) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to init invalid action {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               act_id,
               tableNameGetInternal(table));
    return;
  }

  rc = table->dataAllocate(fields, act_id, &table_data);
  if (!table_data) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed dataAllocate table \"{}\" action {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               act_id);
    return;
  }
  _action_id = act_id;
  init_status = true;

  std::string name;
  table->actionNameGet(act_id, &name);
  switch_log(SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "action: {}", name);
}

switch_status_t _ActionEntry::init_indirect_data() {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return SWITCH_STATUS_SUCCESS;
  if (!table) return SWITCH_STATUS_SUCCESS;

  indirect = true;

  rc = table->dataAllocate(&table_data);
  if (!table_data) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed dataAllocate table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return SWITCH_STATUS_FAILURE;
  }
  init_status = true;

  switch_log(SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "action: indirect");
  return SWITCH_STATUS_SUCCESS;
}

template <typename T>
switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id, T key) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table) return status;
  if (!table_data) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  rc = table_data->setValue(field_id, static_cast<uint64_t>(key));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValue: \"{}.{}\" value {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               dataFieldNameGetInternal(table, field_id, _action_id, indirect),
               key);
    return bf_rt_status_xlate(rc);
  }

  SWITCH_DEBUG_LOG(switch_log(
      SWITCH_API_LEVEL_DEBUG,
      SWITCH_OT_NONE,
      "arg: {} -> {}",
      dataFieldNameGetInternal(table, field_id, _action_id, indirect),
      key));
  return status;
}

template switch_status_t _ActionEntry::set_arg<uint8_t>(
    const bf_rt_field_id_t ap_id, uint8_t key);
template switch_status_t _ActionEntry::set_arg<uint16_t>(
    const bf_rt_field_id_t ap_id, uint16_t key);
template switch_status_t _ActionEntry::set_arg<uint32_t>(
    const bf_rt_field_id_t ap_id, uint32_t key);
template switch_status_t _ActionEntry::set_arg<uint64_t>(
    const bf_rt_field_id_t ap_id, uint64_t key);

template <typename T>
switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id,
                                      std::vector<T> &key,
                                      bool dummy) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table) return status;
  if (!table_data) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  (void)dummy;

  rc = table_data->setValue(field_id, key);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValue: \"{}.{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               dataFieldNameGetInternal(table, field_id, _action_id, indirect));
    return bf_rt_status_xlate(rc);
  }

  // SWITCH_DEBUG_LOG(switch_log(
  //     SWITCH_API_LEVEL_DEBUG,
  //     SWITCH_OT_NONE,
  //     "arg: {} -> {}",
  //     dataFieldNameGetInternal(table, field_id, _action_id, indirect),
  //     key));
  return status;
}

// keep in sync with log.cpp
template switch_status_t _ActionEntry::set_arg<bool>(
    const bf_rt_field_id_t field_id, std::vector<bool> &key, bool dummy);
template switch_status_t _ActionEntry::set_arg<uint32_t>(
    const bf_rt_field_id_t field_id, std::vector<uint32_t> &key, bool dummy);

switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id,
                                      float key) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table) return status;
  if (!table_data) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  rc = table_data->setValue(field_id, key);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValue: \"{}.{}\" value {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               dataFieldNameGetInternal(table, field_id, _action_id, indirect),
               key);
    return bf_rt_status_xlate(rc);
  }

  SWITCH_DEBUG_LOG(switch_log(
      SWITCH_API_LEVEL_DEBUG,
      SWITCH_OT_NONE,
      "arg: {} -> {}",
      dataFieldNameGetInternal(table, field_id, _action_id, indirect),
      key));
  return status;
}

switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id,
                                      std::vector<bf_rt_id_t> &key) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table) return SWITCH_STATUS_INVALID_PARAMETER;
  if (!table_data) return SWITCH_STATUS_INVALID_PARAMETER;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  rc = table_data->setValue(field_id, key);
  if (rc != BF_SUCCESS) {
    // switch_log(SWITCH_API_LEVEL_ERROR,
    //            SWITCH_OT_NONE,
    //            "{}.{}:{}: rc: {} failed setValue: \"{}.{}\" key {}",
    //            __NS__,
    //            __func__,
    //            __LINE__,
    //            bf_err_str(rc),
    //            tableNameGetInternal(table),
    //            dataFieldNameGetInternal(table, field_id, _action_id, indirect),
    //            key);
    return bf_rt_status_xlate(rc);
  }
  return status;
}

switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id,
                                      std::string &key) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table) return SWITCH_STATUS_INVALID_PARAMETER;
  if (!table_data) return SWITCH_STATUS_INVALID_PARAMETER;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  rc = table_data->setValue(field_id, key);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValue: \"{}.{}\" key {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               dataFieldNameGetInternal(table, field_id, _action_id, indirect),
               key);
    return bf_rt_status_xlate(rc);
  }
  return status;
}

switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id,
                                      bool key) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table) return SWITCH_STATUS_INVALID_PARAMETER;
  if (!table_data) return SWITCH_STATUS_INVALID_PARAMETER;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  DataType data_type;
  rc = table->dataFieldDataTypeGet(field_id, _action_id, &data_type);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed dataFieldDataTypeGet: \"{}.{}\" key {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               dataFieldNameGetInternal(table, field_id, _action_id, indirect),
               key);
    return bf_rt_status_xlate(rc);
  }

  if (data_type == DataType::BYTE_STREAM)
    rc = table_data->setValue(field_id, static_cast<uint64_t>(key));
  else
    rc = table_data->setValue(field_id, key);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValue: \"{}.{}\" key {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               dataFieldNameGetInternal(table, field_id, _action_id, indirect),
               key);
    return bf_rt_status_xlate(rc);
  }
  return status;
}

switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id,
                                      const char *key,
                                      size_t sz) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table) return status;
  if (!table_data) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  rc = table_data->setValue(field_id, (unsigned char *)key, sz);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed setValue: \"{}.{}\" key {} sz {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               dataFieldNameGetInternal(table, field_id, _action_id, indirect),
               key,
               sz);
    return bf_rt_status_xlate(rc);
  }

  return status;
}

switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id,
                                      switch_ip_address_t key) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (!table) return status;
  if (!table_data) return status;
  if (!field_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               field_id,
               tableNameGetInternal(table));
    return status;
  }

  const char *v_ptr = NULL;
  size_t sz = 0;
  switch_ip4_t ip4 = 0;

  if (key.addr_family == SWITCH_IP_ADDR_FAMILY_IPV4) {
    ip4 = htonl(key.ip4);
    v_ptr = reinterpret_cast<const char *>(&ip4);
    sz = sizeof(key.ip4);
  } else if (key.addr_family == SWITCH_IP_ADDR_FAMILY_IPV6) {
    v_ptr = reinterpret_cast<const char *>(&key.ip6);
    sz = sizeof(key.ip6);
  } else {
    return SWITCH_STATUS_SUCCESS;
  }
  switch_log(SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "IP -> {}", key);
  return set_arg(field_id, v_ptr, sz);
}

template <>
switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t ap_id,
                                      const switch_object_id_t oid) {
  return set_arg<uint16_t>(ap_id, static_cast<uint16_t>(oid.data));
}

template <>
switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t field_id,
                                      switch_mac_addr_t mac) {
  const char *v_ptr = NULL;
  size_t sz = 0;
  v_ptr = reinterpret_cast<const char *>(mac.mac);
  sz = sizeof(mac.mac);
  switch_log(SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "MAC -> {}", mac);
  return set_arg(field_id, v_ptr, sz);
}

switch_status_t _ActionEntry::set_arg(const bf_rt_field_id_t ap_id,
                                      const switch_object_id_t oid,
                                      const switch_attr_id_t attr_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const char *v_ptr = NULL;
  size_t sz = 0;
  switch_ip4_t ip4 = 0;

  if (!table) return status;
  if (!table_data) return status;
  if (!ap_id) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OT_NONE,
               "{}.{}:{}: Trying to set invalid field_id {} in table {}",
               __NS__,
               __func__,
               __LINE__,
               ap_id,
               tableNameGetInternal(table));
    return status;
  }

  attr_w m_attr(attr_id);
  status = switch_store::attribute_get(oid, attr_id, m_attr);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} attribute_get failed ap_id {} oid {:#x} "
               "attr_id {}",
               __NS__,
               __func__,
               __LINE__,
               status,
               ap_id,
               oid.data,
               attr_id);
    return SWITCH_STATUS_FAILURE;
  }

  auto const type = m_attr.type_get();
  const switch_attribute_t &attr = m_attr.getattr();

  switch (type) {
    case SWITCH_TYPE_BOOL:
      return set_arg(ap_id, attr.value.booldata);
    case SWITCH_TYPE_UINT8:
      return set_arg(ap_id, attr.value.u8);
    case SWITCH_TYPE_UINT16:
      return set_arg(ap_id, attr.value.u16);
    case SWITCH_TYPE_UINT32:
      return set_arg(ap_id, attr.value.u32);
    case SWITCH_TYPE_UINT64:
      return set_arg(ap_id, attr.value.u64);
    case SWITCH_TYPE_ENUM:
      return set_arg(ap_id, attr.value.enumdata.enumdata);
    case SWITCH_TYPE_OBJECT_ID:
      return set_arg(ap_id, attr.value.oid);
    case SWITCH_TYPE_MAC:
      v_ptr = reinterpret_cast<const char *>(&attr.value.mac.mac);
      sz = sizeof(attr.value.mac.mac);
      switch_log(
          SWITCH_API_LEVEL_DEBUG, SWITCH_OT_NONE, "MAC -> {}", attr.value.mac);
      return set_arg(ap_id, v_ptr, sz);
    case SWITCH_TYPE_IP_ADDRESS:
      if (attr.value.ipaddr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV4) {
        ip4 = htonl(attr.value.ipaddr.ip4);
        v_ptr = reinterpret_cast<const char *>(&ip4);
        sz = sizeof(attr.value.ipaddr.ip4);
      } else if (attr.value.ipaddr.addr_family == SWITCH_IP_ADDR_FAMILY_IPV6) {
        v_ptr = reinterpret_cast<const char *>(&attr.value.ipaddr.ip6);
        sz = sizeof(attr.value.ipaddr.ip6);
      } else {
        return SWITCH_STATUS_SUCCESS;
      }
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 SWITCH_OT_NONE,
                 "IP -> {}",
                 attr.value.ipaddr);
      return set_arg(ap_id, v_ptr, sz);
    default:
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}.{}:{}: failed invalid attr type:{} attr_id:{}",
                 __NS__,
                 __func__,
                 __LINE__,
                 type,
                 attr_id);
      status = SWITCH_STATUS_INVALID_PARAMETER;
  }
  return status;
}

switch_status_t _ActionEntry::get_arg(const bf_rt_field_id_t field_id,
                                      uint16_t *key) const {
  uint64_t tmp = 0;
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  status = get_arg(field_id, &tmp);
  *key = static_cast<uint16_t>(tmp);

  return status;
}

switch_status_t _ActionEntry::get_arg(const bf_rt_field_id_t field_id,
                                      uint8_t *key) const {
  uint64_t tmp = 0;
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  status = get_arg(field_id, &tmp);
  *key = static_cast<uint8_t>(tmp);

  return status;
}

switch_status_t _ActionEntry::get_arg(const bf_rt_field_id_t field_id,
                                      std::string *key) const {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table_data) return status;
  if (!field_id) return status;
  if (!table) return status;

  rc = table_data->getValue(field_id, key);
  if (rc != BF_SUCCESS) {
    std::string name;
    table->dataFieldNameGet(field_id, &name);
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed getValue: \"{}.{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               name);
    return bf_rt_status_xlate(rc);
  }

  return status;
}

switch_status_t _ActionEntry::get_arg(const bf_rt_field_id_t field_id,
                                      uint64_t *key) const {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table_data) return status;
  if (!field_id) return status;
  if (!table) return status;

  rc = table_data->getValue(field_id, key);
  if (rc != BF_SUCCESS) {
    std::string name;
    table->dataFieldNameGet(field_id, &name);
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed getValue: \"{}.{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               name);
    return bf_rt_status_xlate(rc);
  }

  return status;
}

switch_status_t _ActionEntry::get_arg(const bf_rt_field_id_t field_id,
                                      uint32_t *key) const {
  uint64_t tmp = 0;
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  status = get_arg(field_id, &tmp);
  *key = static_cast<uint32_t>(tmp);

  return status;
}

switch_status_t _ActionEntry::get_arg(const bf_rt_field_id_t field_id,
                                      const bf_rt_action_id_t act_id,
                                      uint64_t *val) const {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table_data) return status;
  if (!field_id) return status;
  if (!act_id) return status;
  if (!table) return status;

  std::string name;
  table->dataFieldNameGet(field_id, act_id, &name);

  rc = table_data->getValue(field_id, val);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed getValue: \"{}.{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               name);
    return bf_rt_status_xlate(rc);
  }

  return status;
}

switch_status_t _ActionEntry::get_arg(const bf_rt_field_id_t field_id,
                                      std::vector<bf_rt_id_t> *arr) const {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table_data) return status;
  if (!field_id) return status;
  if (!table) return status;

  std::string name;
  table->dataFieldNameGet(field_id, &name);

  rc = table_data->getValue(field_id, arr);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed getValue: \"{}.{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               name);
    return bf_rt_status_xlate(rc);
  }

  return status;
}

switch_status_t _ActionEntry::get_arg(const bf_rt_field_id_t field_id,
                                      bool *var) const {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t rc = BF_SUCCESS;
  if (!table_data) return status;
  if (!field_id) return status;
  if (!table) return status;

  std::string name;
  table->dataFieldNameGet(field_id, &name);

  rc = table_data->getValue(field_id, var);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed getValue: \"{}.{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table),
               name);
    return bf_rt_status_xlate(rc);
  }

  return status;
}

/******************************************************************************
 * _bfrtCacheObject
 *****************************************************************************/
_bfrtCacheObject::_bfrtCacheObject(bf_rt_table_id_t table_id,
                                   const bf_rt_target_t table_dev_tgt,
                                   const BfRtTableKey &table_key,
                                   const BfRtTableData &table_data)
    : dev_tgt(table_dev_tgt) {
  if (table_id == 0) return;
  (get_bf_rt_info())->bfrtTableFromIdGet(table_id, &table);
  setKey(table_key);
  setData(table_data);
}

_bfrtCacheObject::_bfrtCacheObject(bf_rt_table_id_t table_id,
                                   const bf_rt_target_t table_dev_tgt,
                                   const BfRtTableData &table_data)
    : dev_tgt(table_dev_tgt) {
  if (table_id == 0) return;
  (get_bf_rt_info())->bfrtTableFromIdGet(table_id, &table);
  setData(table_data);
}

void _bfrtCacheObject::setKey(const BfRtTableKey &table_key) {
  bf_status_t rc = BF_SUCCESS;
  std::vector<bf_rt_id_t> key_fields;
  rc = table->keyFieldIdListGet(&key_fields);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "ERROR in getting key field id list table:{}",
               tableNameGetInternal(table));
    return;
  }
  rc = table->keyAllocate(&key);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "ERROR allocating key table:{}",
               tableNameGetInternal(table));
    return;
  }

  for (const auto &field_id : key_fields) {
    KeyFieldType type;
    rc = table->keyFieldTypeGet(field_id, &type);
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "ERROR getting Field type for field {} table:{}",
                 field_id,
                 tableNameGetInternal(table));
      return;
    }
    DataType data_type;
    rc = table->keyFieldDataTypeGet(field_id, &data_type);
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "ERROR getting Field data type for field {} table:{}",
                 field_id,
                 tableNameGetInternal(table));
      return;
    }
    size_t size;
    rc = table->keyFieldSizeGet(field_id, &size);
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "ERROR getting Field size for field {} table:{}",
                 field_id,
                 tableNameGetInternal(table));
      return;
    }
    size = (size + 7) / 8;
    switch (type) {
      case KeyFieldType::EXACT: {
        if (data_type == DataType::STRING) {
          std::string value;
          rc = table_key.getValue(field_id, &value);
          rc = key->setValue(field_id, value);
        } else {
          std::vector<uint8_t> value(size);
          rc = table_key.getValue(field_id, size, &value[0]);
          rc = key->setValue(field_id, &value[0], size);
        }
      } break;
      case KeyFieldType::TERNARY: {
        std::vector<uint8_t> value(size);
        std::vector<uint8_t> mask(size);
        rc = table_key.getValueandMask(field_id, size, &value[0], &mask[0]);
        rc = key->setValueandMask(field_id, &value[0], &mask[0], size);
      } break;
      case KeyFieldType::LPM: {
        std::vector<uint8_t> value(size);
        uint16_t prefix_len;
        rc = table_key.getValueLpm(field_id, size, &value[0], &prefix_len);
        rc = key->setValueLpm(field_id, &value[0], prefix_len, size);
      } break;
      case KeyFieldType::RANGE: {
        std::vector<uint8_t> start(size);
        std::vector<uint8_t> end(size);
        rc = table_key.getValueRange(field_id, size, &start[0], &end[0]);
        rc = key->setValueRange(field_id, &start[0], &end[0], size);
      } break;
      default:
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "ERROR invalid type {} for field {} table:{}",
                   static_cast<uint64_t>(type),
                   field_id,
                   tableNameGetInternal(table));
        break;
    }
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "ERROR copying key field {} type {} table:{}",
                 field_id,
                 static_cast<uint64_t>(type),
                 tableNameGetInternal(table));
      return;
    }
  }
  return;
}

void _bfrtCacheObject::printKey() {
  bf_status_t rc = BF_SUCCESS;
  std::vector<bf_rt_id_t> key_fields;
  rc = table->keyFieldIdListGet(&key_fields);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "ERROR in getting key field id list table:{}",
               tableNameGetInternal(table));
    return;
  }

  for (const auto &field_id : key_fields) {
    KeyFieldType type;
    rc = table->keyFieldTypeGet(field_id, &type);
    if (rc) continue;
    DataType data_type;
    rc = table->keyFieldDataTypeGet(field_id, &data_type);
    if (rc) continue;
    size_t size;
    rc = table->keyFieldSizeGet(field_id, &size);
    if (rc) continue;
    size = (size + 7) / 8;
    switch (type) {
      case KeyFieldType::EXACT: {
        std::vector<uint8_t> value(size);
        key.get()->getValue(field_id, size, &value[0]);
      } break;
      case KeyFieldType::TERNARY: {
        std::vector<uint8_t> value(size);
        std::vector<uint8_t> mask(size);
        key.get()->getValueandMask(field_id, size, &value[0], &mask[0]);
      } break;
      case KeyFieldType::LPM: {
        std::vector<uint8_t> value(size);
        uint16_t prefix_len;
        key.get()->getValueLpm(field_id, size, &value[0], &prefix_len);
      } break;
      case KeyFieldType::RANGE: {
        std::vector<uint8_t> start(size);
        std::vector<uint8_t> end(size);
        key.get()->getValueRange(field_id, size, &start[0], &end[0]);
      } break;
      default:
        break;
    }
  }
  return;
}

void _bfrtCacheObject::setData(const BfRtTableData &table_data) {
  std::vector<bf_rt_id_t> data_fields;
  bf_rt_id_t action_id = 0;
  bf_status_t rc = BF_SUCCESS;

  if (table->actionIdApplicable()) {
    rc = table_data.actionIdGet(&action_id);
  }

  if (action_id) {
    rc = table->dataFieldIdListGet(action_id, &data_fields);
  } else {
    rc = table->dataFieldIdListGet(&data_fields);
  }
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "ERROR in getting data field id list table:{} action_id:{}",
               tableNameGetInternal(table),
               action_id);
    return;
  }
  if (action_id) {
    rc = table->dataAllocate(action_id, &data);
  } else {
    rc = table->dataAllocate(&data);
  }
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "ERROR allocating data table:{} action_id:{}",
               tableNameGetInternal(table),
               action_id);
    return;
  }

  for (const auto &field_id : data_fields) {
    bool is_read_only = false;
    if (action_id) {
      rc = table->dataFieldReadOnlyGet(field_id, action_id, &is_read_only);
    } else {
      rc = table->dataFieldReadOnlyGet(field_id, &is_read_only);
    }
    if (rc != BF_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OT_NONE,
          "ERROR getting Field read_only for action {} field {} table:{}",
          action_id,
          field_id,
          tableNameGetInternal(table));
      return;
    }
    if (is_read_only) continue;

    std::string field_name;
    if (action_id) {
      rc = table->dataFieldNameGet(field_id, action_id, &field_name);
    } else {
      rc = table->dataFieldNameGet(field_id, &field_name);
    }
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "ERROR getting Field name for action {} field {} table:{}",
                 action_id,
                 field_id,
                 tableNameGetInternal(table));
      return;
    }
    if (field_name.compare("$ENTRY_HIT_STATE") == 0) continue;

    bool is_active = false;
    rc = table_data.isActive(field_id, &is_active);
    if (rc != BF_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OT_NONE,
          "ERROR getting Field is_active for action {} field {} table:{}",
          action_id,
          field_id,
          tableNameGetInternal(table));
      return;
    }
    if (!is_active) continue;

    DataType type;
    if (action_id) {
      rc = table->dataFieldDataTypeGet(field_id, action_id, &type);
    } else {
      rc = table->dataFieldDataTypeGet(field_id, &type);
    }
    if (rc != BF_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OT_NONE,
          "ERROR getting Field data type for action {} field {} table:{}",
          action_id,
          field_id,
          tableNameGetInternal(table));
      return;
    }
    size_t size;
    if (action_id) {
      rc = table->dataFieldSizeGet(field_id, action_id, &size);
    } else {
      rc = table->dataFieldSizeGet(field_id, &size);
    }
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "ERROR getting Field size for action {} field {} table:{}",
                 action_id,
                 field_id,
                 tableNameGetInternal(table));
      return;
    }
    bool is_register = false;
    auto reg_an = Annotation("$bfrt_field_class", "register_data");
    // See if this field id is a register
    AnnotationSet annotations{};
    if (action_id) {
      rc = table->dataFieldAnnotationsGet(field_id, action_id, &annotations);
    } else {
      rc = table->dataFieldAnnotationsGet(field_id, &annotations);
    }
    if (rc != BF_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OT_NONE,
          "ERROR getting Field annotation for action {} field {} table:{}",
          action_id,
          field_id,
          tableNameGetInternal(table));
      return;
    }
    if (annotations.find(reg_an) != annotations.end()) is_register = true;
    (void)is_register;
    size = (size + 7) / 8;
    switch (type) {
      case DataType::BYTE_STREAM:
      case DataType::UINT64: {
        if (is_register) {
          // do not allocate a size. bfrt does a push_back on this vector
          std::vector<uint64_t> value;
          rc = table_data.getValue(field_id, &value);
          // only dereference it size > 0
          if (value.size() > 0) {
            uint64_t val = value[0];
            rc = data->setValue(field_id, val);
          }
        } else {
          std::vector<uint8_t> value(size);
          rc = table_data.getValue(field_id, size, &value[0]);
          rc = data->setValue(field_id, &value[0], size);
        }
      } break;
      case DataType::BOOL: {
        bool val = false;
        rc = table_data.getValue(field_id, &val);
        rc = data->setValue(field_id, val);
      } break;
      case DataType::FLOAT: {
        float val = 0;
        rc = table_data.getValue(field_id, &val);
        rc = data->setValue(field_id, val);
      } break;
      case DataType::STRING: {
        std::string val;
        // pd fixed objects do not auto-fill action params like MAU objects.
        // Check for return status and only then copy.
        rc = table_data.getValue(field_id, &val);
        if (rc == BF_SUCCESS)
          rc = data->setValue(field_id, val);
        else
          rc = BF_SUCCESS;
      } break;
      case DataType::INT_ARR: {
        std::vector<uint32_t> int_arr;
        rc = table_data.getValue(field_id, &int_arr);
        rc = data->setValue(field_id, int_arr);
      } break;
      case DataType::BOOL_ARR: {
        std::vector<bool> bool_arr;
        rc = table_data.getValue(field_id, &bool_arr);
        rc = data->setValue(field_id, bool_arr);
      } break;
      default:
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "ERROR invalid type {} for field {} table:{}",
                   static_cast<uint64_t>(type),
                   field_id,
                   tableNameGetInternal(table));
        break;
    }
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "ERROR copying data field {} type {} table:{}",
                 field_id,
                 static_cast<uint64_t>(type),
                 tableNameGetInternal(table));
      return;
    }
  }
}

switch_status_t _bfrtCacheObject::flush() {
  bf_status_t rc = BF_SUCCESS;

  BfRtTable::TableApiSet tableApis;

  rc = table->tableApiSupportedGet(&tableApis);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} bfrtCacheObject tableApiSupportedGet failed "
               "table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  if (std::end(tableApis) == std::find(std::begin(tableApis),
                                       std::end(tableApis),
                                       BfRtTable::TableApi::ADD)) {
    if (std::end(tableApis) == std::find(std::begin(tableApis),
                                         std::end(tableApis),
                                         BfRtTable::TableApi::MODIFY)) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}.{}:{}:Unable to flush cache value for table \"{}\"",
                 __NS__,
                 __func__,
                 __LINE__,
                 tableNameGetInternal(table));
      return SWITCH_STATUS_FAILURE;
    } else {
      rc = table->tableEntryMod(
          get_bf_rt_session(), dev_tgt, *key.get(), *data.get());
      if (rc != BF_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}.{}:{}: rc: {} bfrtCacheObject entry mod table \"{}\"",
                   __NS__,
                   __func__,
                   __LINE__,
                   bf_err_str(rc),
                   tableNameGetInternal(table));
        return bf_rt_status_xlate(rc);
      }
    }
  } else {
    rc = table->tableEntryAdd(
        get_bf_rt_session(), dev_tgt, *key.get(), *data.get());
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}.{}:{}: rc: {} bfrtCacheObject flush failed table \"{}\"",
                 __NS__,
                 __func__,
                 __LINE__,
                 bf_err_str(rc),
                 tableNameGetInternal(table));
      return bf_rt_status_xlate(rc);
    }
  }

  return bf_rt_status_xlate(rc);
}

switch_status_t _bfrtCacheObject::defaultEntryFlush() {
  bf_status_t rc = BF_SUCCESS;
  rc = table->tableDefaultEntrySet(get_bf_rt_session(), dev_tgt, *data.get());
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} bfrtCacheObject defaultEntryFlush failed "
               "table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }
  return bf_rt_status_xlate(rc);
}

/******************************************************************************
 * MatchTable
 *****************************************************************************/
_Table::_Table(bf_rt_table_id_t table_id)
    : _table_id(table_id),
      table_dev_tgt(get_dev_tgt()),
      table_session(get_bf_rt_session_ptr()) {
  if (table_id == 0) return;
  (get_bf_rt_info())->bfrtTableFromIdGet(table_id, &table);
}

_Table::_Table(const bf_rt_target_t dev_tgt,
               const BfRtInfo *bf_rt_info,
               bf_rt_table_id_t table_id)
    : _table_id(table_id),
      table_dev_tgt(dev_tgt),
      table_session(get_bf_rt_session_ptr()) {
  (void)bf_rt_info;
  if (table_id == 0) return;
  (get_bf_rt_info())->bfrtTableFromIdGet(table_id, &table);
}

_Table::_Table(const bf_rt_target_t dev_tgt,
               const BfRtInfo *bf_rt_info,
               bf_rt_table_id_t table_id,
               std::shared_ptr<BfRtSession> user_session)
    : _table_id(table_id), table_dev_tgt(dev_tgt), table_session(user_session) {
  (void)bf_rt_info;
  if (table_id == 0) return;
  (get_bf_rt_info())->bfrtTableFromIdGet(table_id, &table);
}

switch_status_t _Table::table_bfrt_session_set(
    const std::shared_ptr<BfRtSession> user_session) {
  if (!user_session) return SWITCH_STATUS_FAILURE;
  table_session = user_session;

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t _Table::table_bfrt_session_get(
    std::shared_ptr<BfRtSession> &user_session) {
  user_session = table_session;

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t _Table::entry_add(const _MatchKey &match_key,
                                  const _ActionEntry &action_entry,
                                  bool &bf_rt_status,
                                  const bool cache) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!match_key.table_key || !action_entry.table_data) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  if (SMI_WARM_INIT) {
    if (cache) {
      _Table Table(table_dev_tgt, get_bf_rt_info(), _table_id);
      table_cache.push_back(
          Table.create_cache_object(table_dev_tgt, match_key, action_entry));
    }
    bf_rt_status = true;
    return bf_rt_status_xlate(rc);
  }

  rc = table->tableEntryAdd(*table_session,
                            table_dev_tgt,
                            *(match_key.table_key),
                            *(action_entry.table_data));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableEntryAdd table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    table_session->sessionCompleteOperations();
    return bf_rt_status_xlate(rc);
  }
  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "{}: tableEntryAdd success for {}",
                              __func__,
                              tableNameGetInternal(table)));
  bf_rt_status = true;

  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::entry_delete(const _MatchKey &match_key) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!match_key.table_key) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  rc = table->tableEntryDel(
      *table_session, table_dev_tgt, *(match_key.table_key));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableEntryDelete table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    table_session->sessionCompleteOperations();
    return bf_rt_status_xlate(rc);
  }
  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "{}: tableEntryDel success for {}",
                              __func__,
                              tableNameGetInternal(table)));
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::entry_modify(const _MatchKey &match_key,
                                     const _ActionEntry &action_entry,
                                     const bool cache) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!match_key.table_key || !action_entry.table_data) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  if (SMI_WARM_INIT) {
    if (cache) {
      _Table Table(table_dev_tgt, get_bf_rt_info(), _table_id);
      table_cache.push_back(
          Table.create_cache_object(table_dev_tgt, match_key, action_entry));
    }
    return bf_rt_status_xlate(rc);
  }

  rc = table->tableEntryMod(*table_session,
                            table_dev_tgt,
                            *(match_key.table_key),
                            *(action_entry.table_data));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableEntryModify table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    table_session->sessionCompleteOperations();
    return bf_rt_status_xlate(rc);
  }
  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "{}: tableEntryMod success for {}",
                              __func__,
                              tableNameGetInternal(table)));
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

// if entry_add and entry_modify are used outside of backend, then the entries
// get cached in the event of warm init
// the p4 wrapper classes will manage adding to cache here
switch_status_t _Table::entry_add(const _MatchKey &match_key,
                                  const _ActionEntry &action_entry,
                                  bool &bf_rt_status) {
  return entry_add(match_key, action_entry, bf_rt_status, true);
}
switch_status_t _Table::entry_modify(const _MatchKey &match_key,
                                     const _ActionEntry &action_entry) {
  return entry_modify(match_key, action_entry, true);
}

switch_status_t _Table::clear_entries() {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  rc = table->tableClear(*table_session, table_dev_tgt);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableClear for table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    table_session->sessionCompleteOperations();
    return bf_rt_status_xlate(rc);
  }
  SWITCH_DEBUG_LOG(switch_log(SWITCH_API_LEVEL_DEBUG,
                              SWITCH_OT_NONE,
                              "{}: tableEntryMod success for {}",
                              __func__,
                              tableNameGetInternal(table)));
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::entry_get(const _MatchKey &match_key,
                                  const _ActionEntry &action_entry) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!match_key.table_key) return rc;
  if (!action_entry.table_data) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  rc = table->tableEntryGet(*table_session,
                            table_dev_tgt,
                            *(match_key.table_key),
                            table_get_flag,  // global variable
                            (action_entry.table_data).get());
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableEntryGet table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::default_entry_set(const _ActionEntry &action_entry,
                                          const bool cache) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!action_entry.table_data) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  if (SMI_WARM_INIT) {
    if (cache) {
      _Table Table(table_dev_tgt, get_bf_rt_info(), _table_id);
      default_entry_cache.push_back(bfrtCacheObject(new _bfrtCacheObject(
          _table_id, table_dev_tgt, *(action_entry.table_data))));
      return bf_rt_status_xlate(rc);
    }
  }

  rc = table->tableDefaultEntrySet(
      *table_session, table_dev_tgt, *(action_entry.table_data));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableDefaultEntryAdd table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  switch_log(SWITCH_API_LEVEL_DEBUG,
             SWITCH_OT_NONE,
             "{}.{}:{}: default action set for table \"{}\"",
             __NS__,
             __func__,
             __LINE__,
             tableNameGetInternal(table));

  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::default_entry_get(const _ActionEntry &action_entry) {
  bf_status_t rc = BF_SUCCESS;
  uint64_t flag = 0;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!action_entry.table_data) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  rc = table->tableDefaultEntryGet(
      *table_session, table_dev_tgt, flag, &(*action_entry.table_data));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableDefaultEntryGet table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  switch_log(SWITCH_API_LEVEL_DEBUG,
             SWITCH_OT_NONE,
             "{}.{}:{}: default action set for table \"{}\"",
             __NS__,
             __func__,
             __LINE__,
             tableNameGetInternal(table));

  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::pvs_scope_set(const GressTarget &gress_target,
                                      bool is_asym) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return bf_rt_status_xlate(rc);
  if (!table) return bf_rt_status_xlate(rc);
  if (!table_session) return SWITCH_STATUS_FAILURE;

  std::unique_ptr<BfRtTableAttributes> table_attributes = nullptr;

  rc = table->attributeAllocate(TableAttributesType::ENTRY_SCOPE,
                                &table_attributes);
  if (table_attributes == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed attributeAllocate for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  std::unique_ptr<BfRtTableEntryScopeArguments> entry_scope_args = nullptr;
  rc = table_attributes->entryScopeArgumentsAllocate(&entry_scope_args);
  if (rc != BF_SUCCESS || entry_scope_args == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed entryScopeArgumentsAllocate for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = entry_scope_args->setValue(static_cast<uint32_t>(gress_target));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed entryScopeArgs setValue for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table_attributes->entryScopeParamsSet(
      TableGressScope::GRESS_SCOPE_SINGLE_GRESS,
      (is_asym ? TableEntryScope::ENTRY_SCOPE_SINGLE_PIPELINE
               : TableEntryScope::ENTRY_SCOPE_ALL_PIPELINES),
      *entry_scope_args,
      TablePrsrScope::PRSR_SCOPE_ALL_PRSRS_IN_PIPE,
      gress_target);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed entryScopeParamsSet for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table->tableAttributesSet(
      *table_session, table_dev_tgt, *table_attributes.get());
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed to set table attributes for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}
switch_status_t _Table::dynamic_hash_field_set(
    std::vector<bfrt_container_data_t> &container_data_list) {
  uint64_t flag = 0;
  bf_status_t rc = BF_SUCCESS;

  if (_table_id == 0) return bf_rt_status_xlate(rc);
  if (!table) return bf_rt_status_xlate(rc);
  if (!table_session) return SWITCH_STATUS_FAILURE;

  std::unique_ptr<BfRtTableData> bfrtTableData = nullptr;
  rc = table->dataAllocate(&bfrtTableData);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed dataAllocate for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  // Store data for each container to this list
  std::vector<std::unique_ptr<BfRtTableData>> data_list;

  for (auto &data : container_data_list) {
    std::unique_ptr<BfRtTableData> bfrtTableInnerData = nullptr;
    rc = table->dataAllocateContainer(data.container_id, &bfrtTableInnerData);
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}.{}: rc: {} failed dataAllocate for inner data {}",
                 __func__,
                 __LINE__,
                 bf_err_str(rc),
                 tableNameGetInternal(table));
      return bf_rt_status_xlate(rc);
    }

    auto &data_map = data.container_data_map;
    for (auto data_it = data_map.begin(); data_it != data_map.end();
         ++data_it) {
      rc = bfrtTableInnerData->setValue(data_it->first, data_it->second);
      if (rc != BF_SUCCESS) {
        switch_log(
            SWITCH_API_LEVEL_ERROR,
            SWITCH_OT_NONE,
            "{}.{}: rc: {} failed to set data for inner container data {}",
            __func__,
            __LINE__,
            bf_err_str(rc),
            tableNameGetInternal(table));
        return bf_rt_status_xlate(rc);
      }
    }

    data_list.push_back(std::move(bfrtTableInnerData));
    rc = bfrtTableData->setValue(data.container_id, std::move(data_list));
    if (rc != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}.{}: rc: {} failed set data for container{}",
                 __func__,
                 __LINE__,
                 bf_err_str(rc),
                 tableNameGetInternal(table));
      return bf_rt_status_xlate(rc);
    }
  }

  rc = table->tableDefaultEntrySet(
      *table_session, table_dev_tgt, flag, *(bfrtTableData));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableDefaultEntrySet table {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::dynamic_hash_algo_get(bf_rt_id_t action_id,
                                              std::string &algo,
                                              uint32_t &seed,
                                              uint32_t &rotate) {
  uint64_t flag = 0;
  bf_status_t rc = BF_SUCCESS;
  bf_rt_field_id_t algo_field_id{0};
  bf_rt_field_id_t seed_field_id{0};
  bf_rt_field_id_t rotate_field_id{0};

  if (_table_id == 0) return bf_rt_status_xlate(rc);
  if (!table) return bf_rt_status_xlate(rc);
  if (!table_session) return SWITCH_STATUS_FAILURE;

  rc = table->dataFieldIdGet("algorithm_name", action_id, &algo_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("seed", action_id, &seed_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("rotate", action_id, &rotate_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);

  std::unique_ptr<BfRtTableData> bfrtTableData = nullptr;
  rc = table->dataAllocate(action_id, &bfrtTableData);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed dataAllocate for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table->tableDefaultEntryGet(
      (*table_session), table_dev_tgt, flag, &(*bfrtTableData));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableDefaultEntryGet table {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  rc = bfrtTableData->getValue(algo_field_id, &algo);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed get algorithm value for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }
  uint64_t seed_val = 0;
  rc = bfrtTableData->getValue(seed_field_id, &seed_val);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed get seed value for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }
  seed = static_cast<uint32_t>(seed_val);
  uint64_t rotate_val = 0;
  rc = bfrtTableData->getValue(rotate_field_id, &rotate_val);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed get rotate value for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }
  rotate = static_cast<uint32_t>(rotate_val);

  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::dynamic_hash_algo_get(bf_rt_id_t action_id,
                                              bool &reverse,
                                              uint64_t &polynomial,
                                              uint64_t &init,
                                              uint64_t &final_xor,
                                              uint64_t &final_bit_width,
                                              uint32_t &seed,
                                              uint32_t &rotate) {
  uint64_t flag = 0;
  bf_status_t rc = BF_SUCCESS;
  uint64_t seed_val = 0, rotate_val = 0;

  if (_table_id == 0) return bf_rt_status_xlate(rc);
  if (!table) return bf_rt_status_xlate(rc);
  if (!table_session) return SWITCH_STATUS_FAILURE;

  bf_rt_field_id_t reverse_field_id{false};
  bf_rt_field_id_t polynomial_field_id{0};
  bf_rt_field_id_t init_field_id{0};
  bf_rt_field_id_t final_xor_field_id{0};
  bf_rt_field_id_t final_bit_width_field_id{0};
  bf_rt_field_id_t seed_field_id{0};
  bf_rt_field_id_t rotate_field_id{0};

  rc = table->dataFieldIdGet("reverse", action_id, &reverse_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("polynomial", action_id, &polynomial_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("init", action_id, &init_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("final_xor", action_id, &final_xor_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet(
      "hash_bit_width", action_id, &final_bit_width_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("seed", action_id, &seed_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("rotate", action_id, &rotate_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);

  std::unique_ptr<BfRtTableData> bfrtTableData = nullptr;
  rc = table->dataAllocate(&bfrtTableData);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed dataAllocate for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table->tableDefaultEntryGet(
      (*table_session), table_dev_tgt, flag, &(*bfrtTableData));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableDefaultEntryGet table {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  rc |= bfrtTableData->getValue(reverse_field_id, &reverse);
  rc |= bfrtTableData->getValue(polynomial_field_id, &polynomial);
  rc |= bfrtTableData->getValue(init_field_id, &init);
  rc |= bfrtTableData->getValue(final_xor_field_id, &final_xor);
  rc |= bfrtTableData->getValue(final_bit_width_field_id, &final_bit_width);
  rc |= bfrtTableData->getValue(seed_field_id, &seed_val);
  seed = static_cast<uint32_t>(seed_val);
  rc |= bfrtTableData->getValue(rotate_field_id, &rotate_val);
  rotate = static_cast<uint32_t>(rotate_val);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed get algorithm value for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::dynamic_hash_algo_set(bf_rt_id_t action_id,
                                              bool reverse,
                                              uint64_t polynomial,
                                              uint64_t init,
                                              uint64_t final_xor,
                                              uint64_t final_bit_width,
                                              uint32_t seed,
                                              uint32_t rotate) {
  uint64_t flag = 0;
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return bf_rt_status_xlate(rc);
  if (!table) return bf_rt_status_xlate(rc);
  if (!table_session) return SWITCH_STATUS_FAILURE;

  bf_rt_field_id_t reverse_field_id{false};
  bf_rt_field_id_t polynomial_field_id{0};
  bf_rt_field_id_t init_field_id{0};
  bf_rt_field_id_t final_xor_field_id{0};
  bf_rt_field_id_t final_bit_width_field_id{0};
  bf_rt_field_id_t seed_field_id{0};
  bf_rt_field_id_t rotate_field_id{0};

  rc = table->dataFieldIdGet("reverse", action_id, &reverse_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("polynomial", action_id, &polynomial_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("init", action_id, &init_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("final_xor", action_id, &final_xor_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet(
      "hash_bit_width", action_id, &final_bit_width_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("seed", action_id, &seed_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("rotate", action_id, &rotate_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);

  std::unique_ptr<BfRtTableData> table_data;
  rc = table->dataAllocate(action_id, &table_data);
  if (rc != BF_SUCCESS || !table_data) return bf_rt_status_xlate(rc);
  rc = table_data->setValue(reverse_field_id, reverse);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table_data->setValue(polynomial_field_id, polynomial);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table_data->setValue(init_field_id, init);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table_data->setValue(final_xor_field_id, final_xor);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table_data->setValue(final_bit_width_field_id, final_bit_width);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table_data->setValue(seed_field_id, static_cast<uint64_t>(seed));
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table_data->setValue(rotate_field_id, static_cast<uint64_t>(rotate));
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);

  rc = table->tableDefaultEntrySet(
      *table_session, table_dev_tgt, flag, *(table_data));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableDefaultEntrySet table {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::dynamic_hash_algo_set(bf_rt_id_t action_id,
                                              std::string algo,
                                              uint32_t seed,
                                              uint32_t rotate) {
  uint64_t flag = 0;
  bf_status_t rc = BF_SUCCESS;
  bf_rt_field_id_t algo_field_id{0};
  bf_rt_field_id_t seed_field_id{0};
  bf_rt_field_id_t rotate_field_id{0};

  if (_table_id == 0) return bf_rt_status_xlate(rc);
  if (!table) return bf_rt_status_xlate(rc);
  if (!table_session) return SWITCH_STATUS_FAILURE;

  rc = table->dataFieldIdGet("algorithm_name", action_id, &algo_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("seed", action_id, &seed_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);
  rc = table->dataFieldIdGet("rotate", action_id, &rotate_field_id);
  if (rc != BF_SUCCESS) return bf_rt_status_xlate(rc);

  std::unique_ptr<BfRtTableData> bfrtTableData = nullptr;
  rc = table->dataAllocate(action_id, &bfrtTableData);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed dataAllocate for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = bfrtTableData->setValue(algo_field_id, algo);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed set algorithm value {} for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               algo,
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = bfrtTableData->setValue(seed_field_id, static_cast<uint64_t>(seed));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed set seed value {} for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               seed,
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = bfrtTableData->setValue(rotate_field_id, static_cast<uint64_t>(rotate));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed set rotate value {} for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               rotate,
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table->tableDefaultEntrySet(
      *table_session, table_dev_tgt, flag, *(bfrtTableData));
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableDefaultEntrySet table {}",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::asymmetric_scope_set() {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return bf_rt_status_xlate(rc);
  if (!table) return bf_rt_status_xlate(rc);
  if (!table_session) return SWITCH_STATUS_FAILURE;

  std::unique_ptr<BfRtTableAttributes> table_attributes = nullptr;

  rc = table->attributeAllocate(TableAttributesType::ENTRY_SCOPE,
                                &table_attributes);
  if (table_attributes == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed attributeAllocate for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table_attributes->entryScopeParamsSet(
      TableEntryScope::ENTRY_SCOPE_SINGLE_PIPELINE);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed entryScopeParamsSet for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table->tableAttributesSet(
      *table_session, table_dev_tgt, *table_attributes.get());
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed to set table attributes for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

void stats_update_cb(const bf_rt_target_t &dev_tgt, void *cookie) {
  (void)dev_tgt;
  (void)cookie;
  return;
}

switch_status_t _Table::do_hw_stats_sync() {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return bf_rt_status_xlate(rc);
  if (!table) return bf_rt_status_xlate(rc);
  if (!table_session) return SWITCH_STATUS_FAILURE;

  std::unique_ptr<BfRtTableOperations> table_operations = nullptr;

  rc = table->operationsAllocate(TableOperationsType::COUNTER_SYNC,
                                 &table_operations);
  if (table_operations == nullptr) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed operationsAllocate for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table_operations->counterSyncSet(
      *table_session, table_dev_tgt, stats_update_cb, NULL);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed counterSyncSet for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }

  rc = table->tableOperationsExecute(*table_operations.get());
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}: rc: {} failed to execute table oprations for table {}",
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
    return bf_rt_status_xlate(rc);
  }
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::table_size_get(size_t *size) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  rc = table->tableSizeGet(*table_session, table_dev_tgt, size);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableSizeGet table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::match_key_field_size_get(bf_rt_id_t field_id,
                                                 size_t *size) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;
  if (field_id == 0) return rc;
  if (size == 0) return SWITCH_STATUS_INVALID_PARAMETER;

  rc = table->keyFieldSizeGet(field_id, size);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed keyFieldSizeGet table \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::table_usage_get(uint32_t *usage) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;
  if (!table_session) return SWITCH_STATUS_FAILURE;

  rc = table->tableUsageGet(*table_session,
                            table_dev_tgt,
                            BfRtTable::BfRtTableGetFlag::GET_FROM_HW,
                            usage);
  if (rc != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: rc: {} failed tableUsageGettable \"{}\"",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(rc),
               tableNameGetInternal(table));
  }

  table_session->sessionCompleteOperations();
  return bf_rt_status_xlate(rc);
}

switch_status_t _Table::name_get(std::string &name) {
  bf_status_t rc = BF_SUCCESS;
  if (_table_id == 0) return rc;
  if (!table) return rc;

  name = tableNameGetInternal(table);
  return rc;
}

std::set<bf_dev_pipe_t> _Table::get_active_pipes() {
  bf_status_t bf_status = BF_SUCCESS;
  std::set<bf_dev_pipe_t> pipes;
  PipelineProfInfoVec pipe_vector;

  if (_table_id == 0) return pipes;
  if (!table) return pipes;

  std::string name = tableNameGetInternal(table);
  auto table_name_tokens = tokenize(name, '.');
  if (table_name_tokens.size() == 0) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: unsupported table name {}",
               __NS__,
               __func__,
               __LINE__,
               name);
    return pipes;
  }
  std::string &pipeline_name = table_name_tokens[0];
  bf_status = bfrtinfo->bfRtInfoPipelineInfoGet(&pipe_vector);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}.{}:{}: status: {} failed to get bfrtinfoPipelineInfo",
               __NS__,
               __func__,
               __LINE__,
               bf_err_str(bf_status));
    return pipes;
  }
  for (auto pipe : pipe_vector) {
    std::string pipename = pipe.first;
    if (pipename.compare(pipeline_name) == 0) {
      std::vector<bf_dev_pipe_t> pipe_vec = pipe.second;
      for (auto p : pipe_vec) pipes.insert(p);
      break;
    }
  }

  return pipes;
}

std::shared_ptr<_bfrtCacheObject> _Table::create_cache_object(
    const bf_rt_target_t dev_tgt,
    const _MatchKey &match_key,
    const _ActionEntry &action_entry) {
  if (_table_id == 0) return nullptr;
  if (!table) return nullptr;
  if (!match_key.table_key || !action_entry.table_data) return nullptr;
  if (!table_session) return nullptr;

  return bfrtCacheObject(new _bfrtCacheObject(
      _table_id, dev_tgt, *(match_key.table_key), *(action_entry.table_data)));
}

/******************************************************************************
 * p4_object_match_action
 *****************************************************************************/
p4_object_match_action::p4_object_match_action(
    const bf_rt_table_id_t table_id,
    const switch_attr_id_t status_attr_id,
    const switch_object_type_t auto_ot,
    const switch_attr_id_t parent_attr_id,
    const switch_object_id_t parent)
    : auto_obj(auto_ot, parent_attr_id, parent),
      match_key(table_id),
      action_entry(table_id),
      _table_id(table_id),
      table_dev_tgt(global_dev_tgt),
      _status_attr_id(status_attr_id),
      table_session(get_bf_rt_session_ptr()) {}

switch_status_t p4_object_match_action::create_update() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status = false;
  bool is_update = false;

  if (auto_obj.get_auto_oid() != 0) {
    is_update = true;
  }

  status = auto_obj.create_update();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}: auto_obj.create_update failure status {}",
               __func__,
               status);
    return status;
  }

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status |= switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to get bf_rt_status status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_create_update(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_create_update failure status {}",
               __func__,
               __LINE__,
               status);
    /* For create case factory will delete all n-1 auto objects that were
     * created before this nth auto object as part of
     * auto cleanup or rollback. So we need to delete this auto object since no
     * one is going to clean it up. For update
     * case the entry might already exist in hardware. If we clean up the auto
     * object here then when the next
     * update/delete happens, the factory will not find this auto object and
     * skip delete/update, so we skip this step
     * here */
    if (!is_update) {
      status |= auto_obj.del();
      return status;
    }
  }

  status |= switch_store::v_set(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to set bf_rt_status status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  return status;
}

switch_status_t p4_object_match_action::del() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status = false;

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status |= switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to get bf_rt_status status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_del(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_del failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= auto_obj.del();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed auto_obj.del status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_match_action::del_pi_only() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status = false;

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status |= switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to get bf_rt_status status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_del(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_del failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_match_action::counters_get(
    const switch_object_id_t handle, std::vector<switch_counter_t> &cntrs) {
  (void)handle;
  (void)cntrs;
  return SWITCH_STATUS_NOT_SUPPORTED;
}

switch_status_t p4_object_match_action::counters_set(
    const switch_object_id_t handle) {
  (void)handle;
  return SWITCH_STATUS_NOT_SUPPORTED;
}

switch_status_t p4_object_match_action::data_get() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  status = pi_data_get();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed pi_data_get status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t p4_object_match_action::data_set() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  status = pi_data_set();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed pi_data_set status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t p4_object_match_action::pi_create_update(bool &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (_table_id == 0) return status;

  _Table table(table_dev_tgt, get_bf_rt_info(), _table_id, table_session);

  if (bf_rt_status) {
    status = table.entry_modify(match_key, action_entry, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_modify status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  } else {
    status = table.entry_add(match_key, action_entry, bf_rt_status, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_add status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  status = add_to_cache();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed add_to_cache status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_match_action::pi_del(bool &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (_table_id == 0) return status;
  _Table table(table_dev_tgt, get_bf_rt_info(), _table_id, table_session);

  if (bf_rt_status) {
    status = table.entry_delete(match_key);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_delete status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  return status;
}

switch_status_t p4_object_match_action::pi_data_get() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (_table_id == 0) return status;
  _Table table(table_dev_tgt, get_bf_rt_info(), _table_id, table_session);

  status |= table.entry_get(match_key, action_entry);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed table.entry_get status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  return status;
}

switch_status_t p4_object_match_action::pi_data_set() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (_table_id == 0) return status;
  _Table table(table_dev_tgt, get_bf_rt_info(), _table_id, table_session);

  status |= table.entry_modify(match_key, action_entry, false);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed table.entry_modify status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  return status;
}

switch_status_t p4_object_match_action::add_to_cache() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (_table_id == 0) return status;

  if (!SMI_WARM_INIT) return status;

  uint64_t oid = get_auto_oid().data;
  if (oid == 0) return status;

  _Table table(table_dev_tgt, get_bf_rt_info(), _table_id, table_session);
  if (p4_match_action_cache.find(oid) != p4_match_action_cache.end()) {
    p4_match_action_cache.erase(oid);
  }
  p4_match_action_cache[oid] =
      table.create_cache_object(table_dev_tgt, match_key, action_entry);

  return SWITCH_STATUS_SUCCESS;
}

/******************************************************************************
 * p4_object_match_action_list
 *****************************************************************************/
p4_object_match_action_list::p4_object_match_action_list(
    const bf_rt_table_id_t table_id,
    const switch_attr_id_t status_attr_id,
    const switch_object_type_t auto_ot,
    const switch_attr_id_t parent_attr_id,
    const switch_object_id_t parent,
    const bool auto_cache /*=true*/)
    : auto_obj(auto_ot, parent_attr_id, parent),
      _table_id(table_id),
      table_dev_tgt(global_dev_tgt),
      _status_attr_id(status_attr_id),
      _auto_cache(auto_cache) {}

switch_status_t p4_object_match_action_list::create_update() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::vector<bool> bf_rt_status({});
  bool is_update = false;

  if (auto_obj.get_auto_oid() != 0) {
    is_update = true;
  }

  status = auto_obj.create_update();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed auto_obj.create_update status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status = get_status_from_list_attr(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed get_status_from_list_attr status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_create_update(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed pi_create_update status {}",
               __func__,
               __LINE__,
               status);
    if (!is_update) {
      status |= auto_obj.del();
      return status;
    }
  }

  status = set_status_to_list_attr(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed set_status_to_list_attr status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}
switch_status_t p4_object_match_action_list::del() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::vector<bool> bf_rt_status({});

  if (auto_obj.get_auto_oid() == 0) return status;

  status = get_status_from_list_attr(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed get_status_from_list_attr status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_del(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed pi_del status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= auto_obj.del();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed auto_obj.del status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_match_action_list::counters_get(
    const switch_object_id_t handle, std::vector<switch_counter_t> &cntrs) {
  (void)handle;
  (void)cntrs;
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t p4_object_match_action_list::counters_set(
    const switch_object_id_t handle) {
  (void)handle;
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t p4_object_match_action_list::data_get() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  status = pi_data_get();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed pi_data_get status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t p4_object_match_action_list::data_set() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  status = pi_data_set();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed pi_data_set status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  return SWITCH_STATUS_SUCCESS;
}

switch_status_t p4_object_match_action_list::pi_create_update(
    std::vector<bool> &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status_local = false;
  std::vector<bool> status_list(bf_rt_status);

  if (_table_id == 0) return status;
  _Table mt(table_dev_tgt, get_bf_rt_info(), _table_id);

  std::vector<bool>::iterator it = status_list.begin();
  for (auto const &entry : match_action_list) {
    if (!entry.second.is_initialized()) {
      continue;
    }
    if (it != status_list.end() && *it == true) {
      status |= mt.entry_modify(entry.first, entry.second, false);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}:{}: failed mt.entry_modify status {}",
                   __func__,
                   __LINE__,
                   status);
        return status;
      }
      ++it;
    } else {
      status |=
          mt.entry_add(entry.first, entry.second, bf_rt_status_local, false);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}:{}: failed mt.entry_add status {}",
                   __func__,
                   __LINE__,
                   status);
        return status;
      }
      if (it == status_list.end()) {
        bf_rt_status.push_back(bf_rt_status_local);
      } else {
        *it = true;
        ++it;
      }
    }
  }

  if (_auto_cache) {
    status = add_to_cache();
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed add_to_cache status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }
  return status;
}

switch_status_t p4_object_match_action_list::pi_del(
    std::vector<bool> &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  (void)bf_rt_status;

  if (_table_id == 0) return status;
  _Table mt(table_dev_tgt, get_bf_rt_info(), _table_id);

  for (auto const &entry : match_action_list) {
    status |= mt.entry_delete(entry.first);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed mt.entry_delete status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }
  return status;
}

switch_status_t p4_object_match_action_list::pi_data_get() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (_table_id == 0) return status;
  _Table mt(table_dev_tgt, get_bf_rt_info(), _table_id);

  for (auto const &entry : match_action_list) {
    status |= mt.entry_get(entry.first, entry.second);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed mt.entry_get status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }
  return status;
}

switch_status_t p4_object_match_action_list::pi_data_set() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (_table_id == 0) return status;
  _Table mt(table_dev_tgt, get_bf_rt_info(), _table_id);

  for (auto const &entry : match_action_list) {
    status |= mt.entry_modify(entry.first, entry.second, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed mt.entry_modify status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }
  return status;
}

switch_status_t p4_object_match_action_list::get_status_from_list_attr(
    const switch_object_id_t object_id,
    const switch_attr_id_t attr_id,
    std::vector<bool> &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  size_t len = 0;
  bool entry_status = false;

  status = switch_store::list_len(object_id, attr_id, len);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  for (size_t i = 0; i < len; i++) {
    status = switch_store::list_v_get(object_id, attr_id, i, entry_status);
    bf_rt_status.push_back(entry_status);
  }
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);

  return status;
}

switch_status_t p4_object_match_action_list::set_status_to_list_attr(
    const switch_object_id_t object_id,
    const switch_attr_id_t attr_id,
    std::vector<bool> &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  attr_w attr(attr_id);
  attr.v_set(bf_rt_status);
  status = switch_store::attribute_set(object_id, attr);
  return status;
}

switch_status_t p4_object_match_action_list::add_to_cache() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (_table_id == 0) return status;

  if (!SMI_WARM_INIT) return status;

  uint64_t oid = get_auto_oid().data;
  if (oid == 0) return status;

  if (p4_match_action_list_cache.find(oid) !=
      p4_match_action_list_cache.end()) {
    p4_match_action_list_cache.erase(oid);
  }

  std::vector<bfrtCacheObject> cache;
  for (auto const &entry : match_action_list) {
    _Table table(table_dev_tgt, get_bf_rt_info(), _table_id);
    p4_match_action_list_cache[oid].push_back(
        table.create_cache_object(table_dev_tgt, entry.first, entry.second));
  }

  return SWITCH_STATUS_SUCCESS;
}

/******************************************************************************
 * p4_object_action_selector
 *****************************************************************************/
p4_object_action_selector::p4_object_action_selector(
    const bf_rt_table_id_t act_prof_id,
    const bf_rt_field_id_t action_member_id,
    const switch_attr_id_t status_attr_id,
    const switch_object_type_t auto_ot,
    const switch_attr_id_t parent_attr_id,
    const switch_object_id_t parent)
    : auto_obj(auto_ot, parent_attr_id, parent),
      _act_prof_id(act_prof_id),
      table_dev_tgt(global_dev_tgt),
      _action_member_id(action_member_id),
      _status_attr_id(status_attr_id),
      match_key(act_prof_id),
      action_entry(act_prof_id) {}

/* This is a departure from the usual object creation behavior. Usually,
 * the auto_obj is created and the pi_create_update is called for match
 * action objetcs. In this case, an indirect table may require the action
 * to be ready to point to.
 * So first we create action in the selector table and then create the
 * child objects
 */
switch_status_t p4_object_action_selector::create_update() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  status = pi_create_update();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_create_update failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = auto_obj.create_update();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: auto_obj create_update failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;
  if (attrs_size() == 0) return SWITCH_STATUS_SUCCESS;
  status |= switch_store::v_set(auto_obj.get_auto_oid(), _status_attr_id, true);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: v_set failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_action_selector::del() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status = false;

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status |= switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: v_get failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= auto_obj.del();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: auto_obj del failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= pi_del(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_del failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_action_selector::pi_create_update() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status = false;

  if (_act_prof_id == 0) return status;
  if (_action_member_id == 0) return status;

  if (attrs_size() == 0) return SWITCH_STATUS_SUCCESS;

  _Table table(table_dev_tgt, get_bf_rt_info(), _act_prof_id);

  // If this is the first time this is called auto_oid is 0
  if (auto_obj.get_auto_oid() == 0) {
    bf_rt_status = false;
  } else {
    status |= switch_store::v_get(
        auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
    if (status != SWITCH_STATUS_SUCCESS) return status;
  }

  if (bf_rt_status) {
    status = table.entry_modify(match_key, action_entry, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_modify status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  } else {
    status = table.entry_add(match_key, action_entry, bf_rt_status, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_add status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  status = add_to_cache();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed add_to_cache status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_action_selector::pi_del(bool &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (_act_prof_id == 0) return status;
  if (_action_member_id == 0) return status;

  _Table table(table_dev_tgt, bfrtinfo, _act_prof_id);

  if (bf_rt_status) {
    status = table.entry_delete(match_key);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_delete status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  return status;
}

switch_status_t p4_object_action_selector::add_to_cache() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (_act_prof_id == 0) return status;

  if (!SMI_WARM_INIT) return status;

  uint64_t oid = auto_obj.get_auto_oid().data;
  if (oid == 0) return status;

  _Table table(table_dev_tgt, get_bf_rt_info(), _act_prof_id);
  if (p4_selector_cache.find(oid) != p4_selector_cache.end()) {
    p4_selector_cache.erase(oid);
  }
  p4_selector_cache[oid] =
      table.create_cache_object(table_dev_tgt, match_key, action_entry);

  return SWITCH_STATUS_SUCCESS;
}

/******************************************************************************
 * p4_object_action_selector
 *****************************************************************************/
p4_object_action_selector_list::p4_object_action_selector_list(
    const bf_rt_table_id_t act_prof_id,
    const bf_rt_field_id_t action_member_id,
    const switch_attr_id_t status_attr_id,
    const switch_object_type_t auto_ot,
    const switch_attr_id_t parent_attr_id,
    const switch_object_id_t parent)
    : auto_obj(auto_ot, parent_attr_id, parent),
      table_dev_tgt(global_dev_tgt),
      _act_prof_id(act_prof_id),
      _action_member_id(action_member_id),
      _status_attr_id(status_attr_id) {}

switch_status_t p4_object_action_selector_list::create_update() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::vector<bool> bf_rt_status({});

  status = auto_obj.create_update();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: auto_obj create_update failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status = switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to v_get bf_rt_status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_create_update(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_create_update failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= switch_store::v_set(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to v_set status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_action_selector_list::pi_create_update(
    std::vector<bool> &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status_local = false;
  std::vector<bool> status_list(bf_rt_status);

  if (_act_prof_id == 0) return status;
  if (_action_member_id == 0) return status;

  if (attrs_size() == 0) return SWITCH_STATUS_SUCCESS;

  _Table table(table_dev_tgt, get_bf_rt_info(), _act_prof_id);

  std::vector<bool>::iterator it = status_list.begin();
  for (auto const &entry : match_action_list) {
    if (!entry.second.is_initialized()) {
      continue;
    }
    if (it != status_list.end() && *it == true) {
      status |= table.entry_modify(entry.first, entry.second, false);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}:{}: failed table.entry_modify status {}",
                   __func__,
                   __LINE__,
                   status);
        return status;
      }
      ++it;
    } else {
      status |=
          table.entry_add(entry.first, entry.second, bf_rt_status_local, false);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OT_NONE,
                   "{}:{}: failed table.entry_add status {}",
                   __func__,
                   __LINE__,
                   status);
        return status;
      }
      bf_rt_status.push_back(bf_rt_status_local);
    }
  }

  status = add_to_cache();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed add_to_cache status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_action_selector_list::pi_del(
    std::vector<bool> &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  (void)bf_rt_status;

  if (_act_prof_id == 0) return status;
  if (_action_member_id == 0) return status;

  _Table table(table_dev_tgt, bfrtinfo, _act_prof_id);

  for (auto const &entry : match_action_list) {
    status |= table.entry_delete(entry.first);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_delete status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  return status;
}

switch_status_t p4_object_action_selector_list::del() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::vector<bool> bf_rt_status({});

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status = switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed get_status_to_list_attr status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  status |= auto_obj.del();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: auto_obj del failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_del(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed pi_del status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_action_selector_list::add_to_cache() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (_act_prof_id == 0) return status;

  if (!SMI_WARM_INIT) return status;

  uint64_t oid = auto_obj.get_auto_oid().data;
  if (oid == 0) return status;

  if (p4_selector_list_cache.find(oid) != p4_selector_list_cache.end()) {
    p4_selector_list_cache.erase(oid);
  }

  std::vector<bfrtCacheObject> cache;
  for (auto const &entry : match_action_list) {
    _Table table(table_dev_tgt, get_bf_rt_info(), _act_prof_id);
    p4_selector_list_cache[oid].push_back(
        table.create_cache_object(table_dev_tgt, entry.first, entry.second));
  }

  return SWITCH_STATUS_SUCCESS;
}

/******************************************************************************
 * p4_object_selector_group
 *****************************************************************************/
p4_object_selector_group::p4_object_selector_group(
    const bf_rt_table_id_t selector_grp_table,
    const switch_attr_id_t status_attr_id,
    const uint32_t max_group_size_field,
    const uint32_t mbr_array_field,
    const uint32_t mbr_status_field,
    const switch_object_type_t auto_ot,
    const switch_attr_id_t parent_attr_id,
    const switch_object_id_t parent)
    : auto_obj(auto_ot, parent_attr_id, parent),
      match_key(selector_grp_table),
      action_entry(selector_grp_table),
      _selector_grp_table(selector_grp_table),
      table_dev_tgt(global_dev_tgt),
      _max_group_size_field(max_group_size_field),
      _mbr_array_field(mbr_array_field),
      _mbr_status_field(mbr_status_field),
      _status_attr_id(status_attr_id) {}

switch_status_t p4_object_selector_group::create_update() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status = false;

  // Same behavior as action selector. pi_create_update followed
  // by auto_object creation
  status = pi_create_update(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_create_update failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = auto_obj.create_update();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: auto_obj.create_update failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= switch_store::v_set(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: v_set failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_selector_group::del() {
  bool bf_rt_status = false;
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status |= switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: v_get failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= auto_obj.del();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: auto_obj.del failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= pi_del(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_del failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_selector_group::pi_create_update(bool &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  _Table table(table_dev_tgt, get_bf_rt_info(), _selector_grp_table);

  // If this is the first time this is called auto_oid is 0
  if (auto_obj.get_auto_oid() == 0) {
    bf_rt_status = false;
  } else {
    status |= switch_store::v_get(
        auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
    if (status != SWITCH_STATUS_SUCCESS) return status;
  }

  if (bf_rt_status) {
    status = table.entry_modify(match_key, action_entry, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_modify status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  } else {
    status = table.entry_add(match_key, action_entry, bf_rt_status, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_add status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  status = add_to_cache();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed add_to_cache status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_selector_group::pi_del(bool &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  _Table table(table_dev_tgt, bfrtinfo, _selector_grp_table);

  if (bf_rt_status) {
    status = table.entry_delete(match_key);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_delete status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  return status;
}

static inline uint32_t compute_next_power_of_2(uint32_t v) {
  if (v == 0) return 64;
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

switch_status_t p4_object_selector_group::add_delete_member(
    std::vector<bf_rt_id_t> &mbrs, std::vector<bool> &mbr_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  uint32_t requested_size = compute_next_power_of_2(mbrs.size());
  action_entry.set_arg(_max_group_size_field, requested_size);
  action_entry.set_arg(_mbr_array_field, mbrs, true);
  action_entry.set_arg(_mbr_status_field, mbr_status, true);

  _Table table(table_dev_tgt, bfrtinfo, _selector_grp_table);

  status = table.entry_modify(match_key, action_entry, false);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OT_NONE,
        "{}:{}: failed table.entry_modify status {}, requested {}, actual {}",
        __func__,
        __LINE__,
        status,
        requested_size,
        mbrs.size());
    return status;
  }

  status = add_to_cache();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed add_to_cache status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_selector_group::add_delete_member_with_weight(
    std::vector<bf_rt_id_t> &mbrs,
    std::vector<bool> &mbr_status,
    std::vector<uint32_t> &weights,
    uint32_t sum) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  (void)weights;

  if (mbrs.size() == sum) {
    action_entry.set_arg(_mbr_array_field, mbrs, true);
    action_entry.set_arg(_mbr_status_field, mbr_status, true);
  } else {
#ifdef USE_SCALED_WEIGHTS
    // scale the weights to _max_grp_size
    // weights: 10, 20, 30, 10
    // sum = 70
    // max_grp_size=256
    // scale_factor = 256/70 = 3.6571
    // clang-format off
    //       deficit from   Scaled   Scaled        Rounded  %error
    //       prev stage     weight   wgt + deficit weight
    // 10    0.00           36.57    36.57         37       1.17%
    // 20    -0.43          73.14    72.71         73       -0.20%
    // 30    0.14           109.71   109.86        110      0.26%
    // 10    -0.29          36.57    36.29         36       -1.56%
    // clang-format on
    double scale_factor = static_cast<double>(_max_grp_size / sum);
    double deficit = 0;
    std::vector<uint32_t> scaled_weights;
    for (const auto weight : weights) {
      double scaled_weight = weight * scale_factor;
      double scaled_weight_with_deficit = scaled_weight + deficit;
      double rounded_weight = std::round(scaled_weight_with_deficit);
      scaled_weights.push_back(rounded_weight);
      deficit = scaled_weight - rounded_weight;
    }
#else
    std::vector<bf_rt_id_t> new_mbrs;
    std::vector<bool> new_mbr_status;
    action_entry.set_arg(_mbr_array_field, new_mbrs, true);
    action_entry.set_arg(_mbr_status_field, new_mbr_status, true);
#endif
  }

  _Table table(table_dev_tgt, bfrtinfo, _selector_grp_table);

  status = table.entry_modify(match_key, action_entry, false);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed table.entry_modify status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = add_to_cache();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed add_to_cache status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_selector_group::get_member_size(size_t &size) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::vector<bf_rt_id_t> mbrs;

  _Table table(table_dev_tgt, bfrtinfo, _selector_grp_table);
  status = table.entry_get(match_key, action_entry);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed table.entry_get status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = action_entry.get_arg(_mbr_array_field, &mbrs);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  size = mbrs.size();

  return status;
}

switch_status_t p4_object_selector_group::add_to_cache() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (_selector_grp_table == 0) return status;

  if (!SMI_WARM_INIT) return status;

  uint64_t oid = auto_obj.get_auto_oid().data;
  if (oid == 0) return status;

  _Table table(table_dev_tgt, get_bf_rt_info(), _selector_grp_table);
  if (p4_selector_group_cache.find(oid) != p4_selector_group_cache.end()) {
    p4_selector_group_cache.erase(oid);
  }
  p4_selector_group_cache[oid] =
      table.create_cache_object(table_dev_tgt, match_key, action_entry);

  return SWITCH_STATUS_SUCCESS;
}

/******************************************************************************
 * p4_object_pd_fixed
 *****************************************************************************/
p4_object_pd_fixed::p4_object_pd_fixed(const bf_rt_table_id_t table_id,
                                       const switch_attr_id_t status_attr_id,
                                       const switch_object_type_t auto_ot,
                                       const switch_attr_id_t parent_attr_id,
                                       const switch_object_id_t parent)
    : auto_obj(auto_ot, parent_attr_id, parent),
      match_key(table_id),
      action_entry(table_id),
      _table_id(table_id),
      table_dev_tgt(global_dev_tgt),
      _status_attr_id(status_attr_id) {}

switch_status_t p4_object_pd_fixed::create_update() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status = false;
  bool is_update = false;

  if (auto_obj.get_auto_oid() != 0) {
    is_update = true;
  }

  status = auto_obj.create_update();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}: auto_obj.create_update failure status {}",
               __func__,
               status);
    return status;
  }

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status |= switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to get bf_rt_status status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_create_update(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_create_update failure status {}",
               __func__,
               __LINE__,
               status);
    /* For create case factory will delete all n-1 auto objects that were
     * created before this nth auto object as part of
     * auto cleanup or rollback. So we need to delete this auto object since no
     * one is going to clean it up. For update
     * case the entry might already exist in hardware. If we clean up the auto
     * object here then when the next
     * update/delete happens, the factory will not find this auto object and
     * skip delete/update, so we skip this step
     * here */
    if (!is_update) {
      status |= auto_obj.del();
      return status;
    }
  }

  status |= switch_store::v_set(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to set bf_rt_status status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }
  return status;
}

switch_status_t p4_object_pd_fixed::del() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bf_rt_status = false;

  if (auto_obj.get_auto_oid() == 0) return SWITCH_STATUS_SUCCESS;

  status |= switch_store::v_get(
      auto_obj.get_auto_oid(), _status_attr_id, bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed to get bf_rt_status status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status = pi_del(bf_rt_status);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: pi_del failure status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  status |= auto_obj.del();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed auto_obj.del status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_pd_fixed::pi_create_update(bool &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (_table_id == 0) return status;

  _Table table(table_dev_tgt, get_bf_rt_info(), _table_id);

  if (bf_rt_status) {
    status = table.entry_modify(match_key, action_entry, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_modify status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  } else {
    status = table.entry_add(match_key, action_entry, bf_rt_status, false);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_add status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  status = add_to_cache();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "{}:{}: failed add_to_cache status {}",
               __func__,
               __LINE__,
               status);
    return status;
  }

  return status;
}

switch_status_t p4_object_pd_fixed::pi_del(bool &bf_rt_status) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (_table_id == 0) return status;
  _Table table(table_dev_tgt, get_bf_rt_info(), _table_id);

  if (bf_rt_status) {
    status = table.entry_delete(match_key);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}:{}: failed table.entry_delete status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }

  return status;
}

switch_status_t p4_object_pd_fixed::add_to_cache() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (_table_id == 0) return status;

  if (!SMI_WARM_INIT) return status;

  uint64_t oid = get_auto_oid().data;
  if (oid == 0) return status;

  _Table table(table_dev_tgt, get_bf_rt_info(), _table_id);
  pd_fixed_cache.push_back(
      table.create_cache_object(table_dev_tgt, match_key, action_entry));

  return SWITCH_STATUS_SUCCESS;
}

/******************************************************************************
 * p4_object_match_action_list
 *****************************************************************************/

}  // namespace bf_rt
}  // namespace smi
