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


#include "common/qos_pdfixed.h"

extern "C" {
#include <tofino/bf_pal/bf_pal_port_intf.h>
#include <tofino/pdfixed/pd_common.h>
#include <tofino/pdfixed/pd_tm.h>
#include <tofino/bf_pal/pltfm_intf.h>
#include <lld/bf_lld_if.h>
}

#include <vector>
#include <set>
#include <memory>
#include <utility>
#include <bitset>
#include <string>
#include <map>
#include <mutex>   // NOLINT(build/c++11)
#include <thread>  // NOLINT(build/c++11)
#include <future>  // NOLINT(build/c++11)

#include "common/utils.h"
#include "common/bfrt_tm.h"
#include "s3/smi.h"

static void switch_port_convert_cos_bmap_icos_map(
    uint8_t pfc_map, uint8_t cos_to_icos[SWITCH_BUFFER_PFC_ICOS_MAX]) {
  uint8_t index = 0;
  for (index = 0; index < SWITCH_BUFFER_PFC_ICOS_MAX; index++) {
    if (pfc_map & (1 << index)) {
      cos_to_icos[index] = index;
    } else {
      cos_to_icos[index] = 0;  // SWITCH_BUFFER_PFC_ICOS_MAX;
    }
  }
}

namespace smi {
using namespace smi::bf_rt;  // NOLINT(build/namespaces)
using ::smi::logging::switch_log;

class port_qos_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_PORT_QOS_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_PORT_QOS_HELPER_ATTR_PARENT_HANDLE;
  static const switch_attr_id_t status_attr_id =
      SWITCH_PORT_QOS_HELPER_ATTR_STATUS;
  uint16_t dev_id = 0, dev_port = 0;
  uint8_t cos_map = 0;
  uint32_t drop_limit = 0, drop_hysterisis = 0, skid_limit = 0;
  uint8_t cos_to_icos[SWITCH_BUFFER_PFC_ICOS_MAX]{};

 public:
  port_qos_helper(const switch_object_id_t parent, switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    switch_object_id_t device_handle = {0};

    status |=
        switch_store::v_get(parent, SWITCH_PORT_ATTR_DEVICE, device_handle);
    status |=
        switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, dev_id);
    status |= switch_store::v_get(parent, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
    status |=
        switch_store::v_get(parent, SWITCH_PORT_ATTR_DROP_LIMIT, drop_limit);
    status |= switch_store::v_get(
        parent, SWITCH_PORT_ATTR_DROP_HYSTERISIS, drop_hysterisis);
    status |=
        switch_store::v_get(parent, SWITCH_PORT_ATTR_PFC_COS_MAP, cos_map);
    switch_port_convert_cos_bmap_icos_map(cos_map, cos_to_icos);
    status |=
        switch_store::v_get(parent, SWITCH_PORT_ATTR_SKID_LIMIT, skid_limit);
  }

  switch_status_t bfrt_tm_port_buffer_skid_limit_cells_set(
      const uint32_t skid_limit_cells) {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    _Table table(get_dev_tgt(), get_bf_rt_info(), smi_id::T_TM_PORT_BUFFER);
    _MatchKey match_key(smi_id::T_TM_PORT_BUFFER);
    _ActionEntry entry(smi_id::T_TM_PORT_BUFFER);

    entry.init_indirect_data();
    status = match_key.set_exact(smi_id::F_TM_PORT_BUFFER_DEV_PORT, dev_port);
    status |= entry.set_arg(smi_id::D_TM_PORT_BUFFER_SKID_LIMIT_CELLS,
                            skid_limit_cells);
    status |= table.entry_modify(match_key, entry);

    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                 "{}.{}:{}: Failed to set skid_limit_cells for ",
                 "dev_port {}, status {}",
                 "port_qos_helper",
                 __func__,
                 __LINE__,
                 dev_port,
                 status);
    }
    return status;
  }

  switch_status_t create_update() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    // status = p4_pd_tm_set_ingress_port_drop_limit(dev_id, dev_port,
    // drop_limit);
    // if (status) {
    //  switch_log(SWITCH_API_LEVEL_ERROR,
    //             SWITCH_OBJECT_TYPE_PORT,
    //             "{}.{}:{}: failed to set drop limit status {} drop_limit {}",
    //             "port_qos_helper",
    //             __func__,
    //             __LINE__,
    //             status,
    //             drop_limit);
    //  return status;
    //}

    // status =
    //    p4_pd_tm_set_ingress_port_hysteresis(dev_id, dev_port,
    //    drop_hysterisis);
    // if (status) {
    //  switch_log(
    //      SWITCH_API_LEVEL_ERROR,
    //      SWITCH_OBJECT_TYPE_PORT,
    //      "{}.{}:{}: failed to set drop limit status {} drop_hysterisis {}",
    //      "port_qos_helper",
    //      __func__,
    //      __LINE__,
    //      status,
    //      drop_hysterisis);
    //  return status;
    //}
    auto_object::create_update();
    status = bfrt_tm_port_flowcontrol_cos_to_icos_set(dev_port, cos_to_icos);
    if (status) {
      auto parent = get_parent();
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OBJECT_TYPE_PORT,
          "{}.{}:{}: failed to set cos to icos mapping with cos_map {:#x} "
          "for port {:#x} status {}",
          "port_qos_helper",
          __func__,
          __LINE__,
          (uint64_t)cos_to_icos,
          parent.data,
          status);
      return status;
    }

    if (bf_lld_dev_is_tof2(0)) {
      bool sw_model = false;
      bf_pal_pltfm_type_get(dev_id, &sw_model);

      uint32_t skid_limit_cells{};
      if (skid_limit != 0 && !sw_model) {
        status = compute_pd_buffer_bytes_to_cells(
            dev_id, skid_limit, &skid_limit_cells);
        if (status != SWITCH_STATUS_SUCCESS) {
          auto parent = get_parent();
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                     "{}.{}:{}: failed to convert skid limit value {} for port "
                     "{:#x} from bytes to cells status {}",
                     "ppg_helper",
                     __func__,
                     __LINE__,
                     skid_limit,
                     parent.data,
                     status);
          return status;
        }
        status = bfrt_tm_port_buffer_skid_limit_cells_set(skid_limit_cells);
      }
    }
    return status;
  }
  switch_status_t del() {
    auto_object::del();
    return SWITCH_STATUS_SUCCESS;
  }
};

class buffer_pool_usage {
 private:
  bool ingress_pool_in_use[SWITCH_BUFFER_POOL_INGRESS_MAX];
  bool egress_pool_in_use[SWITCH_BUFFER_POOL_EGRESS_MAX];

 public:
  p4_pd_pool_id_t ingress_pd_pool[SWITCH_BUFFER_POOL_INGRESS_MAX];
  p4_pd_pool_id_t egress_pd_pool[SWITCH_BUFFER_POOL_EGRESS_MAX];
  uint8_t ingress_use_pool_count = 0;
  uint8_t egress_use_pool_count = 0;
  buffer_pool_usage() {
    for (uint16_t i = 0; i < SWITCH_BUFFER_POOL_INGRESS_MAX; i++) {
      ingress_pool_in_use[i] = false;
      switch (i) {
        case 0:
          ingress_pd_pool[i] = PD_INGRESS_POOL_0;
          break;
        case 1:
          ingress_pd_pool[i] = PD_INGRESS_POOL_1;
          break;
        case 2:
          ingress_pd_pool[i] = PD_INGRESS_POOL_2;
          break;
        case 3:
          ingress_pd_pool[i] = PD_INGRESS_POOL_3;
          break;
        default:
          return;
      }
    }
    for (uint16_t i = 0; i < SWITCH_BUFFER_POOL_EGRESS_MAX; i++) {
      egress_pool_in_use[i] = false;
      switch (i) {
        case 0:
          egress_pd_pool[i] = PD_EGRESS_POOL_0;
          break;
        case 1:
          egress_pd_pool[i] = PD_EGRESS_POOL_1;
          break;
        case 2:
          egress_pd_pool[i] = PD_EGRESS_POOL_2;
          break;
        case 3:
          egress_pd_pool[i] = PD_EGRESS_POOL_3;
          break;
        default:
          return;
      }
    }
  }
  switch_status_t buffer_pool_alloc(switch_buffer_pool_attr_direction dir,
                                    p4_pd_pool_id_t *pool_id) {
    if (dir == SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS) {
      for (uint16_t i = 0; i < SWITCH_BUFFER_POOL_INGRESS_MAX; i++) {
        if (ingress_pool_in_use[i] == false) {
          *pool_id = ingress_pd_pool[i];
          ingress_pool_in_use[i] = true;
          ingress_use_pool_count++;
          return SWITCH_STATUS_SUCCESS;
        }
      }
    } else if (dir == SWITCH_BUFFER_POOL_ATTR_DIRECTION_EGRESS) {
      for (uint16_t i = 0; i < SWITCH_BUFFER_POOL_EGRESS_MAX; i++) {
        if (egress_pool_in_use[i] == false) {
          *pool_id = egress_pd_pool[i];
          egress_pool_in_use[i] = true;
          egress_use_pool_count++;
          return SWITCH_STATUS_SUCCESS;
        }
      }
    }
    return SWITCH_STATUS_INSUFFICIENT_RESOURCES;
  }
  switch_status_t buffer_pool_free(switch_buffer_pool_attr_direction dir,
                                   p4_pd_pool_id_t pool_id) {
    if (dir == SWITCH_BUFFER_POOL_ATTR_DIRECTION_INGRESS) {
      for (uint16_t i = 0; i < SWITCH_BUFFER_POOL_INGRESS_MAX; i++) {
        if (pool_id == ingress_pd_pool[i]) {
          ingress_pool_in_use[i] = false;
          --ingress_use_pool_count;
          break;
        }
      }
    } else if (dir == SWITCH_BUFFER_POOL_ATTR_DIRECTION_EGRESS) {
      for (uint16_t i = 0; i < SWITCH_BUFFER_POOL_EGRESS_MAX; i++) {
        if (pool_id == egress_pd_pool[i]) {
          egress_pool_in_use[i] = false;
          --egress_use_pool_count;
          break;
        }
      }
    }
    return SWITCH_STATUS_SUCCESS;
  }
  ~buffer_pool_usage() {}
};

static buffer_pool_usage buffer_pools[SWITCH_MAX_DEVICE];

class buffer_pool_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_BUFFER_POOL_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_BUFFER_POOL_HELPER_ATTR_PARENT_HANDLE;
  static const switch_attr_id_t pool_attr_id =
      SWITCH_BUFFER_POOL_HELPER_ATTR_POOL_ID;
  uint16_t dev_id = 0;
  uint64_t pool_size = 0, xoff_size = 0;
  uint64_t device_default_buffer = 0;
  switch_buffer_pool_attr_direction dir;

 public:
  buffer_pool_helper(const switch_object_id_t parent, switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    switch_object_id_t device_handle = {0};
    switch_enum_t e = {0};

    status |= switch_store::v_get(
        parent, SWITCH_BUFFER_POOL_ATTR_DEVICE, device_handle);
    status |=
        switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, dev_id);
    status |= switch_store::v_get(device_handle,
                                  SWITCH_DEVICE_ATTR_DEFAULT_BUFFER_POOL_SIZE,
                                  device_default_buffer);
    status |= switch_store::v_get(
        parent, SWITCH_BUFFER_POOL_ATTR_POOL_SIZE, pool_size);
    status |= switch_store::v_get(
        parent, SWITCH_BUFFER_POOL_ATTR_XOFF_SIZE, xoff_size);
    status |= switch_store::v_get(parent, SWITCH_BUFFER_POOL_ATTR_DIRECTION, e);
    dir = static_cast<switch_buffer_pool_attr_direction>(e.enumdata);
  }
  switch_status_t create_update() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    p4_pd_pool_id_t pool_id = PD_INGRESS_POOL_0;
    uint32_t max_threshold = 0, xoff_threshold = 0;
    uint16_t pool_id_temp = 0;
    bool hw_created = false;

    if (device_default_buffer == pool_size) {
      /*
       * Buffer pool is created with device default buffer, keep the
       * driver configured Buffer pool configs for this object.
       */
      status |= switch_store::v_set(
          get_parent(), SWITCH_BUFFER_POOL_ATTR_CREATED_IN_HW, hw_created);
      return status;
    }
    if (get_auto_oid().data == 0) {
      status |= buffer_pools[dev_id].buffer_pool_alloc(dir, &pool_id);
      if (status) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_BUFFER_POOL,
                   "{}.{}:{}: buffer pool alloc failure status {}",
                   "buffer_pool_helper",
                   __func__,
                   __LINE__,
                   status);
        return status;
      }
    } else {
      status |= switch_store::v_get(get_auto_oid(), pool_attr_id, pool_id_temp);
      pool_id = static_cast<p4_pd_pool_id_t>(pool_id_temp);
    }

    status =
        compute_pd_buffer_bytes_to_cells(dev_id, pool_size, &max_threshold);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_BUFFER_POOL,
                 "{}.{}:{}: Failed to get cell size for max threshold for "
                 "device {} ,status {}",
                 "buffer_pool_helper",
                 __func__,
                 __LINE__,
                 dev_id,
                 status);
      return status;
    }

    status = bfrt_tm_pool_cfg_size_cells_set(pool_id, max_threshold);
    if (status != SWITCH_STATUS_SUCCESS) {
      status = SWITCH_STATUS_FAILURE;
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_BUFFER_POOL,
                 "{}.{}:{}: Failed to set app pool size pool_size {} "
                 "max_threshold {} status {}",
                 "buffer_pool_helper",
                 __func__,
                 __LINE__,
                 pool_size,
                 max_threshold,
                 status);
      return status;
    }
    hw_created = true;
    status |= switch_store::v_set(
        get_parent(), SWITCH_BUFFER_POOL_ATTR_CREATED_IN_HW, hw_created);

    if (xoff_size) {
      status =
          compute_pd_buffer_bytes_to_cells(dev_id, xoff_size, &xoff_threshold);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_BUFFER_POOL,
                   "{}.{}:{}: Failed to get cell size for xoff threshold for "
                   "device {} ,status {}",
                   "buffer_pool_helper",
                   __func__,
                   __LINE__,
                   dev_id,
                   status);
        return status;
      }

      for (uint8_t icos = 0; icos < SWITCH_BUFFER_PFC_ICOS_MAX; icos++) {
        status =
            bfrt_tm_pool_app_pfc_limit_cells_set(pool_id, icos, xoff_threshold);
        if (status != SWITCH_STATUS_SUCCESS) {
          status = SWITCH_STATUS_FAILURE;
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_BUFFER_POOL,
                     "{}.{}:{}: Failed to set app pool pfc limit icos {} "
                     "xoff_size {} status {}",
                     "buffer_pool_helper",
                     __func__,
                     __LINE__,
                     icos,
                     xoff_size,
                     status);
          return status;
        }
      }
    }
    auto_object::create_update();
    pool_id_temp = static_cast<uint16_t>(pool_id);
    status |= switch_store::v_set(get_auto_oid(), pool_attr_id, pool_id_temp);

    return status;

    // TODO(bfn): cleanup
  }
  switch_status_t del() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    p4_pd_pool_id_t pool_id;
    uint16_t pool_id_temp = 0;

    if (get_auto_oid().data == 0) return status;

    status |= switch_store::v_get(get_auto_oid(), pool_attr_id, pool_id_temp);
    pool_id = static_cast<p4_pd_pool_id_t>(pool_id_temp);

    status = bfrt_tm_pool_cfg_size_cells_set(pool_id, 0);
    if (status != SWITCH_STATUS_SUCCESS) {
      status = SWITCH_STATUS_FAILURE;
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_BUFFER_POOL,
                 "{}.{}:{}: Failed to set app pool size pd_status {}",
                 "buffer_pool_helper",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }

    status |= buffer_pools[dev_id].buffer_pool_free(dir, pool_id);
    if (status) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_BUFFER_POOL,
                 "{}.{}:{}: buffer pool free failure status {}",
                 "buffer_pool_helper",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }

    auto_object::del();
    return status;
  }
  switch_status_t counters_get(const switch_object_id_t handle,
                               std::vector<switch_counter_t> &cntrs) {
    (void)handle;
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    uint32_t cell_size = 0, co_cell = 0, wm_cell = 0, max_cell = 0;
    p4_pd_pool_id_t pool_id;
    uint16_t pool_id_temp = 0;

    status = bfrt_tm_cfg_cell_size_bytes_get(dev_id, cell_size);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_BUFFER_POOL,
                 "{}.{}: Failed to get cell size in bytes, device {} "
                 "status {}",
                 __func__,
                 __LINE__,
                 dev_id,
                 switch_error_to_string(status));
      return status;
    }

    status = switch_store::v_get(get_auto_oid(), pool_attr_id, pool_id_temp);
    pool_id = static_cast<p4_pd_pool_id_t>(pool_id_temp);

    status = bfrt_tm_counter_pool_usage_get(pool_id, co_cell, wm_cell);
    if (status != SWITCH_STATUS_SUCCESS) {
      status = SWITCH_STATUS_FAILURE;
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_BUFFER_POOL,
                 "{}.{}:{}: Failed to get tm counter pool usage pd_status {}",
                 "buffer_pool_helper",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }

    status = bfrt_tm_pool_cfg_size_cells_get(pool_id, max_cell);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_BUFFER_POOL,
                 "{}.{}:{}: Failed to get tm pool max status {}",
                 "buffer_pool_helper",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }

    switch_counter_t wm_bytes = {
        .counter_id = SWITCH_BUFFER_POOL_COUNTER_ID_WATERMARK_BYTES,
        .count = wm_cell * cell_size};
    switch_counter_t occ_bytes = {
        .counter_id = SWITCH_BUFFER_POOL_COUNTER_ID_CURR_OCCUPANCY_BYTES,
        .count = co_cell * cell_size};
    switch_counter_t max_bytes = {
        .counter_id = SWITCH_BUFFER_POOL_COUNTER_ID_MAX_OCCUPANCY_BYTES,
        .count = max_cell * cell_size};
    cntrs[SWITCH_BUFFER_POOL_COUNTER_ID_WATERMARK_BYTES] = wm_bytes;
    cntrs[SWITCH_BUFFER_POOL_COUNTER_ID_CURR_OCCUPANCY_BYTES] = occ_bytes;
    cntrs[SWITCH_BUFFER_POOL_COUNTER_ID_MAX_OCCUPANCY_BYTES] = max_bytes;
    return status;
  }
  switch_status_t counters_set(const switch_object_id_t handle) {
    (void)handle;
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    p4_pd_pool_id_t pool_id;
    uint16_t pool_id_temp = 0;
    status = switch_store::v_get(get_auto_oid(), pool_attr_id, pool_id_temp);
    pool_id = static_cast<p4_pd_pool_id_t>(pool_id_temp);
    status = bfrt_tm_counter_pool_watermark_cells_clear(pool_id);
    if (status != SWITCH_STATUS_SUCCESS) {
      status = SWITCH_STATUS_FAILURE;
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_BUFFER_POOL,
                 "{}.{}:{}: Failed to clear tm counter pool watermark "
                 "pd_status {}",
                 "buffer_pool_helper",
                 __func__,
                 __LINE__,
                 status);
    }
    return status;
  }

  switch_status_t counters_set(const switch_object_id_t handle,
                               const std::vector<uint16_t> &cntr_ids) {
    for (auto cntr_id : cntr_ids) {
      switch (cntr_id) {
        case SWITCH_BUFFER_POOL_COUNTER_ID_WATERMARK_BYTES:
        case SWITCH_BUFFER_POOL_COUNTER_ID_CURR_OCCUPANCY_BYTES:
          return counters_set(handle);
        default:
          break;
      }
    }
    return SWITCH_STATUS_SUCCESS;
  }
};

class queue_buffer_config : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_QUEUE_BUFFER_CONFIG;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_QUEUE_BUFFER_CONFIG_ATTR_PARENT_HANDLE;
  uint16_t _dev_id = 0, dev_port = 0;
  uint8_t qid = 0;
  switch_object_id_t _buffer_profile_handle = {};
  switch_object_id_t device_default_buffer_profile = {};

 public:
  queue_buffer_config(const switch_object_id_t parent, switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    switch_object_id_t device_handle = {0}, port_handle = {0};

    status |=
        switch_store::v_get(parent, SWITCH_QUEUE_ATTR_DEVICE, device_handle);
    status |=
        switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, _dev_id);
    status |= switch_store::v_get(parent, SWITCH_QUEUE_ATTR_QUEUE_ID, qid);
    status |=
        switch_store::v_get(parent, SWITCH_QUEUE_ATTR_PORT_HANDLE, port_handle);
    status |=
        switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
    status |= switch_store::v_get(parent,
                                  SWITCH_QUEUE_ATTR_BUFFER_PROFILE_HANDLE,
                                  _buffer_profile_handle);
    status |= switch_store::v_get(
        device_handle,
        SWITCH_DEVICE_ATTR_DEFAULT_EGRESS_BUFFER_PROFILE_HANDLE,
        device_default_buffer_profile);
    // Create device default buffer profile if it is not created yet.
    if (device_default_buffer_profile.data == 0) {
      switch_object_id_t ingress_profile = {};
      status |= create_default_buffer_profile(
          device_handle, &ingress_profile, &device_default_buffer_profile);
      status |= switch_store::v_set(
          device_handle,
          SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE,
          ingress_profile);
      status |= switch_store::v_set(
          device_handle,
          SWITCH_DEVICE_ATTR_DEFAULT_EGRESS_BUFFER_PROFILE_HANDLE,
          device_default_buffer_profile);
    }
  }

  switch_status_t update_q_buffer_profile(
      uint16_t dev_id, switch_object_id_t buffer_profile_handle) {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    uint64_t buffer_size = 0;
    uint32_t static_shared_cells = 0;
    switch_enum_t threshold_mode;
    uint64_t threshold = 0;
    uint32_t default_hysteresis = 0;
    // uint16_t pool_id=0;
    p4_pd_pool_id_t pool_id;
    uint16_t pool_id_tmp;
    switch_object_id_t buffer_pool_handle = {};
    switch_object_id_t buffer_pool_helper_handle = {};
    p4_pd_status_t pd_status = 0;
    // p4_pd_tm_ppg_baf_t dyn_baf;
    p4_pd_tm_queue_baf_t dyn_baf = PD_Q_BAF_DISABLE;
    uint64_t xon_threshold = 0;
    uint32_t xon_threshold_cells = 0;

    if (buffer_profile_handle.data != 0) {
      status |=
          switch_store::v_get(buffer_profile_handle,
                              SWITCH_BUFFER_PROFILE_ATTR_BUFFER_POOL_HANDLE,
                              buffer_pool_handle);
      status |= find_auto_oid(buffer_pool_handle,
                              SWITCH_OBJECT_TYPE_BUFFER_POOL_HELPER,
                              buffer_pool_helper_handle);
      if (buffer_pool_helper_handle.data == 0) {
        pool_id_tmp = static_cast<p4_pd_pool_id_t>(PD_EGRESS_POOL_0);
      } else {
        status |= switch_store::v_get(buffer_pool_helper_handle,
                                      SWITCH_BUFFER_POOL_HELPER_ATTR_POOL_ID,
                                      pool_id_tmp);
      }
      pool_id = static_cast<p4_pd_pool_id_t>(pool_id_tmp);
      status |=
          switch_store::v_get(buffer_profile_handle,
                              SWITCH_BUFFER_PROFILE_ATTR_BUFFER_POOL_HANDLE,
                              buffer_pool_handle);
      status |= switch_store::v_get(buffer_profile_handle,
                                    SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD,
                                    threshold);
      status |= switch_store::v_get(buffer_profile_handle,
                                    SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE,
                                    threshold_mode);
      status |= switch_store::v_get(buffer_profile_handle,
                                    SWITCH_BUFFER_PROFILE_ATTR_BUFFER_SIZE,
                                    buffer_size);
      status |= switch_store::v_get(buffer_profile_handle,
                                    SWITCH_BUFFER_PROFILE_ATTR_XON_THRESHOLD,
                                    xon_threshold);
      if (threshold_mode.enumdata ==
          SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_STATIC) {
        dyn_baf = PD_Q_BAF_DISABLE;
      } else {
        if (threshold == SWITCH_BUFFER_MAX_THRESHOLD) {
          dyn_baf = PD_Q_BAF_80_PERCENT;
        } else {
          dyn_baf = (p4_pd_tm_queue_baf_t)(
              threshold / SWITCH_BUFFER_DYNAMIC_THRESHOLD_FACTOR);
        }
      }

      if (xon_threshold) {
        status = compute_pd_buffer_bytes_to_cells(
            dev_id, xon_threshold, &xon_threshold_cells);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_QUEUE,
                     "{}.{}:{}: Failed to get cell size for xon threshold for "
                     "device  {} , status {}",
                     "queue",
                     __func__,
                     __LINE__,
                     dev_id,
                     status);
          return status;
        }
      }

      status = compute_pd_buffer_bytes_to_cells(
          dev_id, threshold, &static_shared_cells);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_QUEUE,
                   "{}.{}:{}: Failed to convert buffer size from bytes to cells"
                   "device {} ,status {}",
                   "queue_buffer_config",
                   __func__,
                   __LINE__,
                   dev_id,
                   status);
        return status;
      }

      p4_pd_pool_id_t default_pool_id;
      uint32_t default_static_shared_cells = 0;
      p4_pd_tm_queue_baf_t default_dyn_baf = PD_Q_BAF_DISABLE;
      pd_status = p4_pd_tm_get_q_app_pool_usage(dev_id,
                                                dev_port,
                                                qid,
                                                &default_pool_id,
                                                &default_static_shared_cells,
                                                &default_dyn_baf,
                                                &default_hysteresis);
      if (pd_status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_QUEUE,
                   "{}.{}:{}: Failed to get default hysteresis for "
                   "device {} dev_port {} qid {}, pd_status {}",
                   "queue_buffer_config",
                   __func__,
                   __LINE__,
                   dev_id,
                   dev_port,
                   qid,
                   pd_status);
        return SWITCH_STATUS_FAILURE;
      }

      bool sw_model = false;
      bf_pal_pltfm_type_get(dev_id, &sw_model);
      if (!sw_model) {
        pd_status = p4_pd_tm_set_q_app_pool_usage(
            dev_id,
            dev_port,
            qid,
            pool_id,
            static_shared_cells,
            static_cast<p4_pd_tm_queue_baf_t>(dyn_baf),
            (xon_threshold ? xon_threshold_cells : default_hysteresis));

        if (pd_status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_QUEUE,
                     "{}.{}:{}: Failed to set shared cells limits for "
                     "device {} dev_port {} qid {}, pd_status {}",
                     "queue_buffer_config",
                     __func__,
                     __LINE__,
                     dev_id,
                     dev_port,
                     qid,
                     pd_status);
          return SWITCH_STATUS_FAILURE;
        }

        uint32_t guar_limit_cells = 0;
        status = compute_pd_buffer_bytes_to_cells(
            dev_id, buffer_size, &guar_limit_cells);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                     "{}.{}:{}: failed to convert guaranteed limit value {} "
                     "for dev_port {} qid {} from bytes to cells, status {}",
                     "queue_buffer_config",
                     __func__,
                     __LINE__,
                     buffer_size,
                     dev_port,
                     qid,
                     status);
          return status;
        }
        pd_status = p4_pd_tm_set_q_guaranteed_min_limit(
            dev_id, dev_port, qid, guar_limit_cells);
        if (pd_status) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                     "{}.{}:{}: failed to configure guaranteed limit for "
                     "dev_port {} qid {} pd_status {}",
                     "queue_buffer_config",
                     __func__,
                     __LINE__,
                     dev_port,
                     qid,
                     pd_status);
          return SWITCH_STATUS_FAILURE;
        }
      }
    } else {
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 SWITCH_OBJECT_TYPE_QUEUE,
                 "{}.{}:{} No buffer profile configuration specified for "
                 "device {} dev_port {}  qid {}",
                 "queue_buffer_config",
                 __func__,
                 __LINE__,
                 dev_id,
                 dev_port,
                 qid);
      // pd_status =
      //    p4_pd_tm_disable_q_app_pool_usage(dev_id, pool_id, qid);
    }

    return status;
  }

  switch_status_t create_update() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    switch_object_id_t buffer_profile_to_use = {};

    if (_buffer_profile_handle.data) {
      buffer_profile_to_use.data = _buffer_profile_handle.data;
    } else if (device_default_buffer_profile.data) {
      buffer_profile_to_use.data = device_default_buffer_profile.data;
    } else {
      buffer_profile_to_use.data = 0;
    }

    status = update_q_buffer_profile(_dev_id, buffer_profile_to_use);
    if (status) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_QUEUE,
                 "{}.{}:{}: failed to update queue_id {} for dev_port {} with "
                 "buffer profile {} "
                 "status {}",
                 "queue_buffer_config",
                 __func__,
                 __LINE__,
                 qid,
                 dev_port,
                 buffer_profile_to_use,
                 status);
      return status;
    }
    status |=
        switch_store::v_set(get_parent(),
                            SWITCH_QUEUE_ATTR_BUFFER_PROFILE_IN_USE,
                            static_cast<uint64_t>(buffer_profile_to_use.data));

    auto_object::create_update();
    return status;
  }

  switch_status_t del() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;

    bool sw_model = false;
    bf_pal_pltfm_type_get(_dev_id, &sw_model);
    if (!sw_model) {
      uint32_t pd_status = 0;
      pd_status = p4_pd_tm_disable_q_app_pool_usage(_dev_id, dev_port, qid);
      if (pd_status != 0) {
        status = SWITCH_STATUS_FAILURE;
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_QUEUE,
                   "{}.{}:{}: failed to disable buffer pool usage for queue_id "
                   "{} dev_port {}"
                   "pd_status {}",
                   "queue_buffer_config",
                   __func__,
                   __LINE__,
                   qid,
                   dev_port,
                   pd_status);
        return status;
      }
    }
    status |= switch_store::v_set(get_parent(),
                                  SWITCH_QUEUE_ATTR_BUFFER_PROFILE_IN_USE,
                                  static_cast<uint64_t>(0));
    auto_object::del();
    return status;
  }
};

class ppg_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot = SWITCH_OBJECT_TYPE_PPG_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_PPG_HELPER_ATTR_PARENT_HANDLE;
  uint16_t _dev_id = 0, dev_port = 0;
  bool lossless_admin_enable = false;
  uint32_t skid_limit = 0, skid_hysterisis = 0;
  switch_object_id_t _buffer_profile_handle = {};
  switch_object_id_t device_default_buffer_profile = {};

 public:
  ppg_helper(const switch_object_id_t parent, switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    switch_object_id_t device_handle = {0}, port_handle = {0};

    status |= switch_store::v_get(
        parent, SWITCH_PORT_PRIORITY_GROUP_ATTR_DEVICE, device_handle);
    status |=
        switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, _dev_id);
    status |= switch_store::v_get(
        parent, SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE, port_handle);
    status |=
        switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
    if (!feature::is_feature_set(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE)) {
      /* For Asymmetric Folded pipeline, the external user facing pipe ports are
       * on a non-switch pipeline. Packets
       * come in on the external ports and are processed by a custom P4 pipeline
       * before they reach the Regular
       * Switching Pipeline. The path from the external pipe to the internal
       * pipe that implements this Switching
       * functionality is plumbed/stitched in a special way such that all
       * traffic from the external ports is mapped
       * 1:1 to an internal pipe port. Since entire switching pipeline lies in
       * this internal pipe, the QoS
       * functionality that understands and deals with ppgs also exists on the
       * internal pipe. Hence to be able to
       * correctly implement ingress QoS we map the ppgs created on external
       * pipe port to the corresponding internal
       * pipe port and vice versa
       */
      auto pipe = DEV_PORT_TO_PIPE(dev_port);
      // If Port is in Switch Egress Pipeline, add the PPG in corresponding
      // Switch Ingress Pipeline
      // If Port is in Switch Ingress Pipeline, add the PPG in corresponding
      // Switch Egress Pipeline
      // If Port is added in neither Switch Egress nor Switch Ingress Pipeline,
      // do no translations
      auto &&external_pipes = SWITCH_CONTEXT.get_switch_egress_pipe_list();
      auto &&non_switch_ingress_pipes =
          SWITCH_CONTEXT.get_switch_non_ingress_pipe_list();
      if (std::find(external_pipes.begin(), external_pipes.end(), pipe) !=
          external_pipes.end()) {
        dev_port = translate_egress_port(dev_port);
      } else if (std::find(non_switch_ingress_pipes.begin(),
                           non_switch_ingress_pipes.end(),
                           pipe) == non_switch_ingress_pipes.end()) {
        dev_port = translate_ingress_port(dev_port);
      }
    }
    status |= switch_store::v_get(
        parent,
        SWITCH_PORT_PRIORITY_GROUP_ATTR_LOSSLESS_ADMIN_ENABLE,
        lossless_admin_enable);
    status |= switch_store::v_get(
        parent, SWITCH_PORT_PRIORITY_GROUP_ATTR_SKID_LIMIT, skid_limit);
    status |=
        switch_store::v_get(parent,
                            SWITCH_PORT_PRIORITY_GROUP_ATTR_SKID_HYSTERISIS,
                            skid_hysterisis);
    status |= switch_store::v_get(
        parent,
        SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE,
        _buffer_profile_handle);
    status |= switch_store::v_get(
        device_handle,
        SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE,
        device_default_buffer_profile);
    // Create device default buffer profile if it is not created yet.
    if (device_default_buffer_profile.data == 0) {
      switch_object_id_t egress_profile = {};
      status |= create_default_buffer_profile(
          device_handle, &device_default_buffer_profile, &egress_profile);
      status |= switch_store::v_set(
          device_handle,
          SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE,
          device_default_buffer_profile);
      status |= switch_store::v_set(
          device_handle,
          SWITCH_DEVICE_ATTR_DEFAULT_EGRESS_BUFFER_PROFILE_HANDLE,
          egress_profile);
    }
  }

  switch_status_t update_ppg_buffer_profile(
      uint16_t dev_id,
      switch_object_id_t buffer_profile_handle,
      p4_pd_tm_ppg_t tm_ppg_handle) {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    uint64_t xon_threshold = 0;
    uint32_t xon_threshold_cells = 0;
    uint32_t static_shared_cells = 0;
    switch_enum_t threshold_mode;
    uint64_t threshold = 0;
    // uint16_t pool_id=0;
    p4_pd_pool_id_t pool_id;
    uint16_t pool_id_tmp;
    switch_object_id_t buffer_pool_handle = {};
    switch_object_id_t buffer_pool_helper_handle = {};
    p4_pd_status_t pd_status = 0;
    // p4_pd_tm_ppg_baf_t dyn_baf;
    p4_pd_tm_queue_baf_t dyn_baf = PD_Q_BAF_DISABLE;

    if (buffer_profile_handle.data != 0) {
      status |=
          switch_store::v_get(buffer_profile_handle,
                              SWITCH_BUFFER_PROFILE_ATTR_BUFFER_POOL_HANDLE,
                              buffer_pool_handle);
      status |= find_auto_oid(buffer_pool_handle,
                              SWITCH_OBJECT_TYPE_BUFFER_POOL_HELPER,
                              buffer_pool_helper_handle);
      if (buffer_pool_helper_handle.data == 0) {
        pool_id_tmp = static_cast<p4_pd_pool_id_t>(PD_INGRESS_POOL_0);
      } else {
        status |= switch_store::v_get(buffer_pool_helper_handle,
                                      SWITCH_BUFFER_POOL_HELPER_ATTR_POOL_ID,
                                      pool_id_tmp);
      }
      pool_id = static_cast<p4_pd_pool_id_t>(pool_id_tmp);
      status |=
          switch_store::v_get(buffer_profile_handle,
                              SWITCH_BUFFER_PROFILE_ATTR_BUFFER_POOL_HANDLE,
                              buffer_pool_handle);
      status |= switch_store::v_get(buffer_profile_handle,
                                    SWITCH_BUFFER_PROFILE_ATTR_XON_THRESHOLD,
                                    xon_threshold);
      status |= switch_store::v_get(buffer_profile_handle,
                                    SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE,
                                    threshold_mode);
      status |= switch_store::v_get(buffer_profile_handle,
                                    SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD,
                                    threshold);
      if (xon_threshold) {
        status = compute_pd_buffer_bytes_to_cells(
            dev_id, xon_threshold, &xon_threshold_cells);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                     "{}.{}:{}: Failed to get cell size for xon threshold fo "
                     "device  {} , status {}",
                     "ppg_helper",
                     __func__,
                     __LINE__,
                     dev_id,
                     status);
          return status;
        }
      }

      if (threshold_mode.enumdata ==
          SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_STATIC) {
        dyn_baf = PD_Q_BAF_DISABLE;
      } else {
        if (threshold == SWITCH_BUFFER_MAX_THRESHOLD) {
          dyn_baf = PD_Q_BAF_80_PERCENT;
        } else {
          dyn_baf =
              (p4_pd_tm_queue_baf_t)(static_cast<uint32_t>(threshold) /
                                     SWITCH_BUFFER_DYNAMIC_THRESHOLD_FACTOR);
        }
      }

      status = compute_pd_buffer_bytes_to_cells(
          dev_id, threshold, &static_shared_cells);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                   "{}.{}:{}: Failed to get cell size for xon threshold for "
                   "device {} ,status {}",
                   "ppg_helper",
                   __func__,
                   __LINE__,
                   dev_id,
                   status);
        return status;
      }

      pd_status = p4_pd_tm_set_ppg_app_pool_usage(
          dev_id,
          tm_ppg_handle,
          pool_id,
          static_shared_cells,
          static_cast<p4_pd_tm_ppg_baf_t>(dyn_baf),
          xon_threshold_cells);
    } else {
      switch_log(
          SWITCH_API_LEVEL_INFO,
          SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
          "{}.{}:{} No buffer profile configuration specified for tm_ppg {}",
          "ppg_helper",
          __func__,
          __LINE__,
          tm_ppg_handle);
      // pd_status =
      //    p4_pd_tm_disable_ppg_app_pool_usage(dev_id, pool_id, tm_ppg_handle);
    }

    if (pd_status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                 "{}.{}:{}: Failed to set PPG pool usage for dev_id  {} , "
                 "pd_status {}",
                 "ppg_helper",
                 __func__,
                 __LINE__,
                 dev_id,
                 pd_status);
      return SWITCH_STATUS_FAILURE;
    }
    return status;
  }

  switch_status_t create_update() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    p4_pd_tm_ppg_t pd_hdl = 0, def_ppg_pd_hdl = 0;
    bool hw_created = false;
    switch_object_id_t buffer_profile_to_use = {};
    uint64_t xon_threshold = 0;
    uint64_t xoff_threshold = 0;
    uint64_t buffer_size = 0;
    bool lossless_oper_enable = false;

    // If pd hdl is already created for default ppg, nothing to do here
    status |= switch_store::v_get(
        get_parent(), SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL, pd_hdl);
    status |= switch_store::v_get(get_parent(),
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW,
                                  hw_created);

    status |= p4_pd_tm_get_default_ppg(_dev_id, dev_port, &def_ppg_pd_hdl);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                 "{}.{}: {} p4_pd_tm_get_default_ppg failure for dev_id {} "
                 "dev_port {} status {}",
                 "ppg_helper",
                 __func__,
                 __LINE__,
                 _dev_id,
                 dev_port,
                 status);
      return status;
    }

    if (pd_hdl == def_ppg_pd_hdl) {
      /*
       * skip configuring buffer, lossless for default ppg.
       */
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                 "{}.{}: {} No create/update action required on dev_id {} "
                 "dev_port {}, for default ppg {}",
                 "ppg_helper",
                 __func__,
                 __LINE__,
                 _dev_id,
                 dev_port,
                 pd_hdl);
      return status;
    }

    // 1. If PPG exists in hardware and buffer profile is applied, then update
    // the
    // ppg with buffer profile changes.
    // 2. If PPG does not exist in hardware and buffer profile is specified,
    // then
    // cache the buffer profile changes, these will be applied later when ppg is
    // created
    // 3. If the PPG does not exist in hardware and no buffer profile is
    // specified
    // then nothing needs to be done
    // 4. If ppg exists in hardware and buffer profile is dereferenced from the
    // ppg then use default ppg buffer profile if one exists, if no default
    // profile is specified for the ppg then use global device level default
    // buffer profile. If none
    // of the buffer profiles is specified then this is an error condition
    if (_buffer_profile_handle.data) {
      buffer_profile_to_use.data = _buffer_profile_handle.data;
    } else if (device_default_buffer_profile.data) {
      buffer_profile_to_use.data = device_default_buffer_profile.data;
    } else {
      buffer_profile_to_use.data = 0;
    }
    if (hw_created) {
      status =
          update_ppg_buffer_profile(_dev_id, buffer_profile_to_use, pd_hdl);
      if (status) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                   "{}.{}:{}: failed to update ppg {} with buffer profile {} "
                   "status {}",
                   "ppg_helper",
                   __func__,
                   __LINE__,
                   pd_hdl,
                   buffer_profile_to_use,
                   status);
        return status;
      }
      status |= switch_store::v_set(
          get_parent(),
          SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_IN_USE,
          static_cast<uint64_t>(buffer_profile_to_use.data));

      status |= switch_store::v_get(buffer_profile_to_use,
                                    SWITCH_BUFFER_PROFILE_ATTR_XON_THRESHOLD,
                                    xon_threshold);
      status |= switch_store::v_get(buffer_profile_to_use,
                                    SWITCH_BUFFER_PROFILE_ATTR_XOFF_THRESHOLD,
                                    xoff_threshold);
      status |= switch_store::v_get(buffer_profile_to_use,
                                    SWITCH_BUFFER_PROFILE_ATTR_BUFFER_SIZE,
                                    buffer_size);

      /*
       * lossless should be enabled only if
       *   admin_enable && (both xon and xoff threshold should be not zero)
           enable lossless .
       * in all other cases like
       *   if (admin_enable is true and one of xon and xoff threshold is zero)
               then lossless ppg would not work without having xon or xoff
       threshold.
               so lossless enable will be false.
       */

      lossless_oper_enable = lossless_admin_enable;
      if (lossless_admin_enable && (!xon_threshold || !xoff_threshold)) {
        lossless_oper_enable = false;
      }

      if (lossless_oper_enable) {
        status = p4_pd_tm_enable_lossless_treatment(_dev_id, pd_hdl);
      } else {
        status = p4_pd_tm_disable_lossless_treatment(_dev_id, pd_hdl);
      }
      if (status) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                   "{}.{}:{}: failed to configure lossless treatment status {}",
                   "ppg_helper",
                   __func__,
                   __LINE__,
                   status);
        return status;
      }

      uint32_t guar_limit_cells = 0;
      status = compute_pd_buffer_bytes_to_cells(
          _dev_id, buffer_size, &guar_limit_cells);
      if (status != SWITCH_STATUS_SUCCESS) {
        auto parent = get_parent();
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                   "{}.{}:{}: failed to convert guaranteed limit value {} "
                   "for ppg {:#x} from bytes to cells status {}",
                   "ppg_helper",
                   __func__,
                   __LINE__,
                   buffer_size,
                   parent.data,
                   status);
        return status;
      }
      status = p4_pd_tm_set_ppg_guaranteed_min_limit(
          _dev_id, pd_hdl, guar_limit_cells);
      if (status) {
        auto parent = get_parent();
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                   "{}.{}:{}: failed to configure guaranteed limit for ppg "
                   "{:#x} status {}",
                   "ppg_helper",
                   __func__,
                   __LINE__,
                   parent.data,
                   status);
        return status;
      }

      if (!skid_limit) {
        skid_limit = xoff_threshold;
      }

      if (skid_limit) {
        uint32_t skid_limit_cells{};
        status = compute_pd_buffer_bytes_to_cells(
            _dev_id, skid_limit, &skid_limit_cells);
        if (status != SWITCH_STATUS_SUCCESS) {
          auto parent = get_parent();
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                     "{}.{}:{}: failed to convert skid limit value {} for ppg "
                     "{:#x} from bytes to cells status {}",
                     "ppg_helper",
                     __func__,
                     __LINE__,
                     skid_limit,
                     parent.data,
                     status);
          return status;
        }
        status = p4_pd_tm_set_ppg_skid_limit(_dev_id, pd_hdl, skid_limit_cells);
        if (status) {
          auto parent = get_parent();
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                     "{}.{}:{}: failed to configure skid limit for ppg {:#x} "
                     "status {}",
                     "ppg_helper",
                     __func__,
                     __LINE__,
                     parent.data,
                     status);
          return status;
        }
      }

      uint32_t skid_hysterisis_cells = 0;
      if (skid_hysterisis) {
        status = compute_pd_buffer_bytes_to_cells(
            _dev_id, skid_hysterisis, &skid_hysterisis_cells);
        if (status != SWITCH_STATUS_SUCCESS) {
          auto parent = get_parent();
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                     "{}.{}:{}: failed to convers skid hysteresis value {} for "
                     "ppg {:#x} from bytes to cells  status {}",
                     "ppg_helper",
                     __func__,
                     __LINE__,
                     skid_hysterisis,
                     parent.data,
                     status);
          return status;
        }
        status = p4_pd_tm_set_guaranteed_min_skid_hysteresis(
            _dev_id, pd_hdl, skid_hysterisis);
        if (status) {
          auto parent = get_parent();
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                     "{}.{}:{}: failed to configure skid hysterisis for ppg "
                     "{:#x} status {}",
                     "ppg_helper",
                     __func__,
                     __LINE__,
                     parent.data,
                     status);
          return status;
        }
      }

      status |= switch_store::v_set(
          get_parent(),
          SWITCH_PORT_PRIORITY_GROUP_ATTR_LOSSLESS_OPER_ENABLE,
          lossless_oper_enable);

      switch_log(
          SWITCH_API_LEVEL_DEBUG,
          SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
          "{}.{}:{}: buffer profile configuration update successful for ppg "
          " ppg_oper_enable: {}, guar_limit: {} , skid_limit: {} "
          ",skid_hysterisis: {}",
          __func__,
          __LINE__,
          get_parent(),
          lossless_oper_enable,
          buffer_size,
          skid_limit,
          skid_hysterisis_cells);
    } else {
      switch_log(SWITCH_API_LEVEL_DEBUG,
                 SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                 "{}.{}:{}: Skipping buffer profile configuration for ppg {}"
                 "until it is referenced",
                 "ppg_helper",
                 __func__,
                 __LINE__,
                 get_parent());
    }

    auto_object::create_update();
    return status;
  }

  switch_status_t del() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    bool hw_created = false;
    p4_pd_tm_ppg_t pd_hdl = 0;
    p4_pd_tm_ppg_t def_ppg_pd_hdl;

    status |= switch_store::v_get(
        get_parent(), SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL, pd_hdl);

    /*
     * ppg handle could be zero which is a valid one.
     * tm_ppg_handle: pipe+dev_port+ppg_num
     */
    status |= p4_pd_tm_get_default_ppg(_dev_id, dev_port, &def_ppg_pd_hdl);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                 "{}.{}: {} p4_pd_tm_get_default_ppg failure for dev_id {} "
                 "dev_port {} status {}",
                 "ppg_helper",
                 __func__,
                 __LINE__,
                 _dev_id,
                 dev_port,
                 status);
      return status;
    }

    if (pd_hdl != def_ppg_pd_hdl) {
      status |=
          switch_store::v_get(get_parent(),
                              SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW,
                              hw_created);
      CHECK_RET(hw_created == true, SWITCH_STATUS_RESOURCE_IN_USE);
    }

    status |= switch_store::v_set(get_parent(),
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL,
                                  static_cast<uint32_t>(0));
    auto_object::del();
    return status;
  }
};

switch_status_t switch_pd_queue_pfc_cos_mapping(uint16_t device,
                                                uint16_t dev_port,
                                                uint8_t queue_id,
                                                uint8_t cos) {
  p4_pd_status_t pd_status;

  pd_status = p4_pd_tm_set_q_pfc_cos_mapping(device, dev_port, queue_id, cos);
  if (pd_status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}:{}: Failed to set icos pfc_cos to queue mapping, device  "
               "{} , dev_port {} "
               "queue_id {} cos: {} pd_status {}",
               __func__,
               __LINE__,
               device,
               dev_port,
               queue_id,
               cos,
               pd_status);
    return SWITCH_STATUS_FAILURE;
  }

  switch_log(SWITCH_API_LEVEL_DEBUG,
             SWITCH_OBJECT_TYPE_NONE,
             "{}:{}: set icos pfc_cos to queue mapping success, device  {} "
             ", dev_port {} "
             "queue_id {} cos: {} pd_status {}",
             __func__,
             __LINE__,
             device,
             dev_port,
             queue_id,
             cos,
             pd_status);

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t switch_pd_port_ppg_icos_mapping_update(
    uint16_t device, p4_pd_tm_ppg_t tm_ppg_handle, uint8_t icos, bool add) {
  p4_pd_status_t pd_status;

  uint8_t icos_bmap = 0;
  pd_status = p4_pd_tm_ppg_icos_mapping_get(device, tm_ppg_handle, &icos_bmap);
  if (pd_status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}:{}: Failed to get icos bitmap for ppg, device  {} , "
               "pd_status {}",
               __func__,
               __LINE__,
               device,
               pd_status);
    return SWITCH_STATUS_FAILURE;
  }
  if (add) {
    icos_bmap |= (1 << icos);
  } else {
    icos_bmap &= ~(1 << icos);
  }

  switch_log(SWITCH_API_LEVEL_DEBUG,
             SWITCH_OBJECT_TYPE_NONE,
             "{}:{}: device {} PPG handle {}, icos_bmap {}",
             __func__,
             __LINE__,
             device,
             tm_ppg_handle,
             icos_bmap);

  pd_status = p4_pd_tm_set_ppg_icos_mapping(device, tm_ppg_handle, icos_bmap);
  if (pd_status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}:{}: Failed to set icos bitmap for ppg, device  {} , "
               "pd_status {}",
               __func__,
               __LINE__,
               device,
               pd_status);
    return SWITCH_STATUS_FAILURE;
  }

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t update_port_pfc_priority_to_queue(
    switch_object_id_t egress_qos_map_handle, switch_object_id_t port_handle) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::vector<switch_object_id_t> queue_list;
  switch_object_id_t device_handle = {0};
  uint16_t dev_id = 0, dev_port = 0;

  std::vector<switch_object_id_t> qos_map_list;

  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEVICE, device_handle);
  status |=
      switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, dev_id);
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);

  status |= switch_store::v_get(egress_qos_map_handle,
                                SWITCH_QOS_MAP_EGRESS_ATTR_QOS_MAP_LIST,
                                qos_map_list);
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_QUEUE_HANDLES, queue_list);
  std::set<uint8_t> port_qids;
  for (auto const queue : queue_list) {
    uint8_t qid = 0;
    status |= switch_store::v_get(queue, SWITCH_QUEUE_ATTR_QUEUE_ID, qid);
    port_qids.insert(qid);
  }

  for (auto const &qos_map : qos_map_list) {
    uint8_t pfc_priority = 0;
    uint8_t qid = 0;

    // Get the data from the qos_map handle
    status |= switch_store::v_get(
        qos_map, SWITCH_QOS_MAP_ATTR_PFC_PRIORITY, pfc_priority);
    status |= switch_store::v_get(qos_map, SWITCH_QOS_MAP_ATTR_QID, qid);
    switch_pd_queue_pfc_cos_mapping(dev_id, dev_port, qid, pfc_priority);
    port_qids.erase(qid);
  }

  for (auto qid : port_qids) {
    switch_pd_queue_pfc_cos_mapping(dev_id, dev_port, qid, 0);
  }

  return status;
}

// static switch_status_t port_get_ppg_handle(switch_object_id_t port_handle,
//                                           uint8_t ppg_index_lookup,
//                                           switch_object_id_t &ppg_handle) {
//  switch_status_t status = SWITCH_STATUS_SUCCESS;
//  uint8_t ppg_index = 0;
//
//  std::vector<switch_object_id_t> port_ppg_handles_list;
//  status =
//      switch_store::v_get(port_handle,
//                          SWITCH_PORT_ATTR_PORT_PRIORITY_GROUPS,
//                          port_ppg_handles_list);
//  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
//
//  for (auto const &port_ppg_handle : port_ppg_handles_list) {
//    status = switch_store::v_get(
//        port_ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX,
//        ppg_index);
//    if (status != SWITCH_STATUS_SUCCESS) continue;
//    if (ppg_index_lookup == ppg_index) {
//      ppg_handle = port_ppg_handle;
//      return status;
//    }
//  }
//
//  return SWITCH_STATUS_ITEM_NOT_FOUND;
//}

template <typename T>
static inline void BIT_SET(T &bit_map, uint8_t n) {
  T bit = static_cast<T>(1UL);

  bit_map |= (bit << n);
}

template <typename T>
static inline void BIT_UNSET(T &bit_map, uint8_t n) {
  T bit = static_cast<T>(1UL);
  bit_map &= ~(1UL << n);
}

template <typename T>
static inline bool CHECK_BIT_SET(T &bit_map, uint8_t n) {
  if ((bit_map >> n) & 1) {
    return true;
  } else {
    return false;
  }
}

switch_status_t delete_hardware_ppg(uint16_t dev_id,
                                    switch_object_id_t ppg_handle) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  p4_pd_status_t pd_status = BF_SUCCESS;
  p4_pd_tm_ppg_t pd_hdl = 0;
  status = switch_store::v_get(
      ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL, pd_hdl);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  pd_status = p4_pd_tm_free_ppg(dev_id, pd_hdl);
  if (pd_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}:{}: failed to delete ppg from hardware {:#x} (pd_hdl: {}) "
               "pd_status {}",
               __func__,
               __LINE__,
               ppg_handle.data,
               pd_hdl,
               pd_status);
    return SWITCH_STATUS_FAILURE;
  }
  status |= switch_store::v_set(ppg_handle,
                                SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL,
                                static_cast<uint32_t>(0));
  status |= switch_store::v_set(
      ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW, false);
  status |= switch_store::v_set(ppg_handle,
                                SWITCH_PORT_PRIORITY_GROUP_ATTR_ICOS_BMP,
                                static_cast<uint8_t>(0));
  /* Cached ppg stats are reset every time ppg is deleted from hardware */
  status |= switch_store::v_set(ppg_handle,
                                SWITCH_PORT_PRIORITY_GROUP_ATTR_PACKET_COUNT,
                                static_cast<uint64_t>(0));
  status |= switch_store::v_set(ppg_handle,
                                SWITCH_PORT_PRIORITY_GROUP_ATTR_BYTE_COUNT,
                                static_cast<uint64_t>(0));
  return status;
}

static switch_status_t update_ppg_icos_bmp(uint16_t dev_id,
                                           switch_object_id_t ppg_handle,
                                           uint8_t icos_bmp) {
  p4_pd_tm_ppg_t pd_hdl = 0;
  p4_pd_status_t pd_status = BF_SUCCESS;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  status = switch_store::v_get(
      ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL, pd_hdl);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  pd_status = p4_pd_tm_set_ppg_icos_mapping(dev_id, pd_hdl, icos_bmp);
  if (pd_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}:{}: Failed to set icos bmap:{} for ppg "
               "{:#x} (pd_hdl: {})"
               " pd_status {}",
               __func__,
               __LINE__,
               std::bitset<8>(icos_bmp),
               ppg_handle.data,
               pd_hdl,
               pd_status);
    return SWITCH_STATUS_FAILURE;
  }
  status |= switch_store::v_set(
      ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_ICOS_BMP, icos_bmp);
  return status;
}

static switch_status_t update_port_icos_to_default_ppg(
    uint16_t dev_id, switch_object_id_t port_handle) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t default_ppg_handle{0};
  p4_pd_tm_ppg_t tm_ppg_handle = 0;
  std::vector<switch_object_id_t> ppg_list;
  uint8_t def_ppg_old_icos_bmp{0};
  /* Map all icos value to def ppg */
  uint8_t def_ppg_new_icos_bmp{0xFF};
  uint16_t dev_port{};

  bool is_switch_pipe_port_ppg = true;
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
  auto &switch_ingress_pipes = SWITCH_CONTEXT.get_switch_ingress_pipe_list();
  if (feature::is_feature_set(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) &&
      std::find(switch_ingress_pipes.begin(),
                switch_ingress_pipes.end(),
                DEV_PORT_TO_PIPE(dev_port)) == switch_ingress_pipes.end()) {
    is_switch_pipe_port_ppg = false;
  }
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_PORT_PRIORITY_GROUPS, ppg_list);

  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_DEFAULT_PPG, default_ppg_handle.data);
  status |= switch_store::v_get(default_ppg_handle,
                                SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL,
                                tm_ppg_handle);
  status |= switch_store::v_get(default_ppg_handle,
                                SWITCH_PORT_PRIORITY_GROUP_ATTR_ICOS_BMP,
                                def_ppg_old_icos_bmp);

  /* Query port icos stats and cache packet and byte count for all icos which
   * were previously mapped to default ppg. This
   * is required as the port icos stat table is going to be reset at the end of
   * this function, due to ppg icos mapping
   * change. This reset will cause a loss in ppg stats if not cached */
  uint64_t def_ppg_pkts{0}, def_ppg_bytes{0};
  std::unique_ptr<object> port_ppg_stats_helper(factory::get_instance().create(
      SWITCH_OBJECT_TYPE_PORT_PPG_STATS, port_handle, status));
  std::vector<switch_counter_t> port_icos_cntrs;
  if (is_switch_pipe_port_ppg &&
      !switch_store::smiContext::context().in_warm_init()) {
    status = port_ppg_stats_helper->counters_get(port_handle, port_icos_cntrs);
    CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
    for (uint8_t icos = 0; icos < SWITCH_BUFFER_PFC_ICOS_MAX; icos++) {
      if (CHECK_BIT_SET(def_ppg_old_icos_bmp, icos)) {
        def_ppg_pkts +=
            port_icos_cntrs[(SWITCH_BUFFER_PFC_ICOS_MAX - 1 - icos) * 2].count;
        def_ppg_bytes +=
            port_icos_cntrs[(SWITCH_BUFFER_PFC_ICOS_MAX - 1 - icos) * 2 + 1]
                .count;
      }
    }
  }

  status =
      update_ppg_icos_bmp(dev_id, default_ppg_handle, def_ppg_new_icos_bmp);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}:{}: Failed to update default ppg icos bmap for port"
               "{:#x} "
               "status {}",
               __func__,
               __LINE__,
               port_handle.data,
               status);
  }

  // Delete all the non default ppgs that are no longer referenced
  for (auto const ppg_handle : ppg_list) {
    if (ppg_handle == default_ppg_handle) continue;
    bool hw_created = false;
    status |= switch_store::v_get(
        ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW, hw_created);
    if (hw_created) {
      status = delete_hardware_ppg(dev_id, ppg_handle);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_NONE,
                   "{}:{}: failed to delete unreferenced ppg for port {:#x} "
                   "status: {}",
                   __func__,
                   __LINE__,
                   port_handle.data,
                   status);
      }
    }
  }

  // Update default ppg packet & byte cached stats
  if (!switch_store::smiContext::context().in_warm_init()) {
    uint64_t cached_pkt_count{0}, cached_byte_count{0};
    status |= switch_store::v_get(default_ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_PACKET_COUNT,
                                  cached_pkt_count);
    status |= switch_store::v_get(default_ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_BYTE_COUNT,
                                  cached_byte_count);
    cached_pkt_count += def_ppg_pkts;
    cached_byte_count += def_ppg_bytes;
    status |= switch_store::v_set(default_ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_PACKET_COUNT,
                                  cached_pkt_count);
    status |= switch_store::v_set(default_ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_BYTE_COUNT,
                                  cached_byte_count);
  }

  /*
   * one time clear ppg stats as icos to ppg mapping changed.
   * This is awkward. We need to ensure a port_ppg_stats asicObj is created for
   * all implementations
   */
  if (is_switch_pipe_port_ppg && port_ppg_stats_helper) {
    status |= port_ppg_stats_helper->counters_set(port_handle);
  }
  return status;
}

switch_status_t update_port_icos_to_ppg(
    switch_object_id_t ingress_qos_map_handle, switch_object_id_t port_handle) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t device_handle{0}, default_ppg_handle{0};
  uint16_t dev_id{0}, dev_port{0};
  p4_pd_tm_ppg_t tm_default_ppg_handle{0};
  std::vector<switch_object_id_t> qos_map_list, ppg_handles;
  bool in_warm_init = switch_store::smiContext::context().in_warm_init();

  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEVICE, device_handle);
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
  status |=
      switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, dev_id);
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_DEFAULT_PPG, default_ppg_handle.data);
  status |= switch_store::v_get(default_ppg_handle,
                                SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL,
                                tm_default_ppg_handle);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);

  // Bit map to track all referenced icos values
  uint8_t icos_bit_map = 0;
  // Per PPG icos bit maps
  std::vector<uint8_t> ppg_icos_bit_map(SWITCH_MAX_PPGS_PER_PIPE, 0);
  // Per PPG old icos bit maps
  std::vector<uint8_t> old_ppg_icos_bit_map(SWITCH_MAX_PPGS_PER_PIPE, 0);
  // PPGs that exist in hardware
  std::vector<bool> ppgs_created(SWITCH_MAX_PPGS_PER_PIPE, false);
  uint8_t def_ppg_old_icos_bmp = 0;

  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_PORT_PRIORITY_GROUPS, ppg_handles);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  for (auto &&ppg_handle : ppg_handles) {
    uint8_t ppg_index = 0;
    uint8_t icos_bmp = 0;
    status |= switch_store::v_get(
        ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_ICOS_BMP, icos_bmp);
    CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
    if (ppg_handle == default_ppg_handle) {
      def_ppg_old_icos_bmp = icos_bmp;
      continue;
    }
    status |= switch_store::v_get(
        ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX, ppg_index);
    if (ppg_index >= SWITCH_MAX_PPGS_PER_PIPE) {
      // ideally can not happend
      return SWITCH_STATUS_FAILURE;
    }
    ppgs_created[ppg_index] = true;
    old_ppg_icos_bit_map[ppg_index] = icos_bmp;
  }

  status |= switch_store::v_get(ingress_qos_map_handle,
                                SWITCH_QOS_MAP_INGRESS_ATTR_QOS_MAP_LIST,
                                qos_map_list);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  for (auto &&qos_map : qos_map_list) {
    uint8_t icos = 0;
    uint8_t ppg = 0;
    status |= switch_store::v_get(qos_map, SWITCH_QOS_MAP_ATTR_ICOS, icos);
    status |= switch_store::v_get(qos_map, SWITCH_QOS_MAP_ATTR_PPG, ppg);
    CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
    if (ppg >= SWITCH_MAX_PPGS_PER_PIPE) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_NONE,
                 "{}.{}: Failed to update port icos to ppg map for port {:#x}, "
                 "qos_map_ingress {:#x}. Reference to invalid ppg index {} in "
                 "qos_map {:#x} ",
                 __func__,
                 __LINE__,
                 port_handle.data,
                 ingress_qos_map_handle.data,
                 ppg,
                 qos_map.data);
      return SWITCH_STATUS_FAILURE;
    } else if (!ppgs_created[ppg]) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_NONE,
                 "{}.{}: Failed to update port icos to ppg map for port {:#x}, "
                 "qos_map_ingress {:#x}. ppg with index {} referred in "
                 "qos_map {:#x} does not exist",
                 __func__,
                 __LINE__,
                 port_handle.data,
                 ingress_qos_map_handle.data,
                 ppg,
                 qos_map.data);
      return SWITCH_STATUS_FAILURE;
    } else if (icos >= 8) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_NONE,
                 "{}.{}: Failed to update port icos to ppg map for port {:#x}, "
                 "qos_map_ingress {:#x}. Invalid icos: {} referred in in "
                 "qos_map {:#x} ",
                 __func__,
                 __LINE__,
                 port_handle.data,
                 ingress_qos_map_handle.data,
                 icos,
                 qos_map.data);
      return SWITCH_STATUS_FAILURE;
    }
    BIT_SET<uint8_t>(ppg_icos_bit_map[ppg], icos);
    BIT_SET<uint8_t>(icos_bit_map, icos);
  }
  uint8_t def_ppg_icos_bmp = ~icos_bit_map;

  bool is_switch_pipe_port_ppg = true;
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
  auto &switch_ingress_pipes = SWITCH_CONTEXT.get_switch_ingress_pipe_list();
  if (feature::is_feature_set(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) &&
      std::find(switch_ingress_pipes.begin(),
                switch_ingress_pipes.end(),
                DEV_PORT_TO_PIPE(dev_port)) == switch_ingress_pipes.end()) {
    is_switch_pipe_port_ppg = false;
  }
  /* Query port icos stats and cache packet and byte count for all ppgs. This
   * is required as the port icos stat table is going to be reset at the end of
   * this function, due to ppg icos mapping
   * change. This reset will cause a loss in ppg stats if not cached */
  std::vector<switch_counter_t> port_icos_cntrs;
  std::unique_ptr<object> port_ppg_stats_helper(factory::get_instance().create(
      SWITCH_OBJECT_TYPE_PORT_PPG_STATS, port_handle, status));
  if (is_switch_pipe_port_ppg && !in_warm_init) {
    status = port_ppg_stats_helper->counters_get(port_handle, port_icos_cntrs);
    CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  }

  // Since we have reached here, we now know that all the ppgs referred have
  // already been created, atleast in the
  // software. So we now proceed ahead with remapping ppg icos bmap, whilst
  // creating newly referenced ppgs and
  // deleting (in HW) any unreferenced ppgs
  // TM cannot have same icos values mapped to two different ppgs.So in first
  // iteration we remove all non-current
  // references and in second iteration we remap these icos to currently mapped
  // ppgs
  for (uint8_t index = 0; index < 2; index++) {
    for (auto &&ppg_handle : ppg_handles) {
      uint8_t ppg_index = 0;
      p4_pd_tm_ppg_t tm_ppg_handle = 0;
      bool hw_created = false;
      p4_pd_status_t pd_status = BF_SUCCESS;
      status |= switch_store::v_get(
          ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX, ppg_index);
      status |=
          switch_store::v_get(ppg_handle,
                              SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW,
                              hw_created);
      CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
      if (ppg_handle == default_ppg_handle) continue;

      // No icos bmap for ppg, delete from HW if created
      if (hw_created && index == 0) {
        if (!ppg_icos_bit_map[ppg_index]) {
          status = delete_hardware_ppg(dev_id, ppg_handle);
          if (status != SWITCH_STATUS_SUCCESS) {
            switch_log(
                SWITCH_API_LEVEL_ERROR,
                SWITCH_OBJECT_TYPE_NONE,
                "{}:{}: failed to delete unreferenced ppg for port {:#x} "
                "status: {}",
                __func__,
                __LINE__,
                port_handle.data,
                status);
            return status;
          }
          continue;
        } else if (ppg_icos_bit_map[ppg_index] ==
                   old_ppg_icos_bit_map[ppg_index]) {
          continue;
        } else {
          // If an icos is no longer mapped to this ppg_index then remove it
          // from ppg icos_bmp. This is required because
          // two different ppgs can not have save icos value in their bitmaps.
          // New icos values mapped to this ppg will
          // be set in the second iteration.
          uint8_t transient_bmp =
              ppg_icos_bit_map[ppg_index] & old_ppg_icos_bit_map[ppg_index];
          status = update_ppg_icos_bmp(dev_id, ppg_handle, transient_bmp);
          if (status != SWITCH_STATUS_SUCCESS) {
            switch_log(SWITCH_API_LEVEL_ERROR,
                       SWITCH_OBJECT_TYPE_NONE,
                       "{}:{}: failed to update ppg {:#x}, transient bitmap to "
                       "{}, for port {:#x} "
                       "status: {}",
                       __func__,
                       __LINE__,
                       ppg_handle.data,
                       std::bitset<8>(transient_bmp),
                       port_handle.data,
                       status);
            return status;
          }
        }
      } else if (ppg_icos_bit_map[ppg_index] && index == 1) {
        /*
         * on hitless bf-switch should call p4_pd_tm_allocate_ppg for
         * non-default ppgs, since bf_tm_restore_ppg_cfg only handles default
         * ppg
         */
        if (!hw_created || in_warm_init) {
          // create in hw
          if (in_warm_init) {
            // For hitless case, driver APIs need old handle to be passed to ppg
            // create APIs for reconcilation
            status |=
                switch_store::v_get(ppg_handle,
                                    SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL,
                                    tm_ppg_handle);
            CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
          }
          pd_status = p4_pd_tm_allocate_ppg(dev_id, dev_port, &tm_ppg_handle);
          if (pd_status != BF_SUCCESS) {
            status = SWITCH_STATUS_FAILURE;
            switch_log(SWITCH_API_LEVEL_ERROR,
                       SWITCH_OBJECT_TYPE_NONE,
                       "{}.{}: Failed to allocate ppg on device {} "
                       "port {} for ppg_index",
                       __func__,
                       __LINE__,
                       dev_id,
                       port_handle,
                       ppg_index);
            return status;
          }
          status |= switch_store::v_set(ppg_handle,
                                        SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL,
                                        tm_ppg_handle);
          status |= switch_store::v_set(
              ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW, true);
          CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
          // Once the ppg is created in hardware we need to ensure that the
          // associated buffer profile for ppg is applied
          auto mobject =
              std::unique_ptr<ppg_helper>(new ppg_helper(ppg_handle, status));
          status |= mobject->create_update();
          CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
        } else {
          status |= switch_store::v_get(ppg_handle,
                                        SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL,
                                        tm_ppg_handle);
          CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
        }
        if (old_ppg_icos_bit_map[ppg_index] != ppg_icos_bit_map[ppg_index]) {
          pd_status = p4_pd_tm_set_ppg_icos_mapping(
              dev_id, tm_ppg_handle, ppg_icos_bit_map[ppg_index]);
          if (pd_status != BF_SUCCESS) {
            switch_log(SWITCH_API_LEVEL_ERROR,
                       SWITCH_OBJECT_TYPE_NONE,
                       "{}:{}: Failed to set port icos to ppg mapping, for "
                       "device {:#x}"
                       "ppg_handle {:#x} icos_bmp: {} pd_status {}",
                       __func__,
                       __LINE__,
                       device_handle.data,
                       ppg_handle.data,
                       std::bitset<8>(ppg_icos_bit_map[ppg_index]),
                       pd_status);
            return SWITCH_STATUS_FAILURE;
          }
          status |=
              switch_store::v_set(ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_ICOS_BMP,
                                  ppg_icos_bit_map[ppg_index]);
          CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
        }
        if (is_switch_pipe_port_ppg && !in_warm_init) {
          uint64_t ppg_pkts{0}, ppg_bytes{0};
          status |=
              switch_store::v_get(ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_PACKET_COUNT,
                                  ppg_pkts);
          status |=
              switch_store::v_get(ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_BYTE_COUNT,
                                  ppg_bytes);
          for (uint8_t icos = 0; icos < SWITCH_BUFFER_PFC_ICOS_MAX; icos++) {
            if (CHECK_BIT_SET(old_ppg_icos_bit_map[ppg_index], icos)) {
              ppg_pkts +=
                  port_icos_cntrs[(SWITCH_BUFFER_PFC_ICOS_MAX - 1 - icos) * 2]
                      .count;
              ppg_bytes +=
                  port_icos_cntrs[(SWITCH_BUFFER_PFC_ICOS_MAX - 1 - icos) * 2 +
                                  1]
                      .count;
            }
          }
          /* Cached ppg stats everytime icos ppg mapping changes */
          status |=
              switch_store::v_set(ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_PACKET_COUNT,
                                  ppg_pkts);
          status |=
              switch_store::v_set(ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_BYTE_COUNT,
                                  ppg_bytes);
          CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
        }
      }
    }
  }

  // Finally map all unmapped icos to default PPG
  if (def_ppg_icos_bmp != def_ppg_old_icos_bmp) {
    p4_pd_status_t pd_status = 0;
    pd_status = p4_pd_tm_set_ppg_icos_mapping(
        dev_id, tm_default_ppg_handle, def_ppg_icos_bmp);
    if (pd_status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OBJECT_TYPE_NONE,
          "{}:{}: Failed to map unreferenced icos bmap:{} to default ppg "
          "{} for port {:#x} device {:#x}"
          " pd_status {}",
          __func__,
          __LINE__,
          std::bitset<8>(def_ppg_icos_bmp),
          tm_default_ppg_handle,
          port_handle.data,
          device_handle.data,
          pd_status);
      return SWITCH_STATUS_FAILURE;
    }
    status |= switch_store::v_set(default_ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_ICOS_BMP,
                                  def_ppg_icos_bmp);
    CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  }
  if (is_switch_pipe_port_ppg && !in_warm_init) {
    uint64_t def_ppg_pkts{0}, def_ppg_bytes{0};
    for (uint8_t icos = 0; icos < SWITCH_BUFFER_PFC_ICOS_MAX; icos++) {
      status |=
          switch_store::v_get(default_ppg_handle,
                              SWITCH_PORT_PRIORITY_GROUP_ATTR_PACKET_COUNT,
                              def_ppg_pkts);
      status |= switch_store::v_get(default_ppg_handle,
                                    SWITCH_PORT_PRIORITY_GROUP_ATTR_BYTE_COUNT,
                                    def_ppg_bytes);
      if (CHECK_BIT_SET(def_ppg_old_icos_bmp, icos)) {
        def_ppg_pkts +=
            port_icos_cntrs[(SWITCH_BUFFER_PFC_ICOS_MAX - 1 - icos) * 2].count;
        def_ppg_bytes +=
            port_icos_cntrs[(SWITCH_BUFFER_PFC_ICOS_MAX - 1 - icos) * 2 + 1]
                .count;
      }
    }
    /* Cached ppg stats everytime icos ppg mapping changes */
    status |= switch_store::v_set(default_ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_PACKET_COUNT,
                                  def_ppg_pkts);
    status |= switch_store::v_set(default_ppg_handle,
                                  SWITCH_PORT_PRIORITY_GROUP_ATTR_BYTE_COUNT,
                                  def_ppg_bytes);
    CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  }

  /* If this is warm init, then the port might not have been created yet, in
  such case skip this for now. The stats table will be updated correctly when
  the
  port is created, since the qos_map_ingress object would already have been
  created by then. */
  if (in_warm_init && is_switch_pipe_port_ppg) {
    switch_object_id_t ppg_stats_handle;
    status |= find_auto_oid(
        port_handle, SWITCH_OBJECT_TYPE_PORT_PPG_STATS, ppg_stats_handle);
    std::vector<bool> status_list;
    status |= switch_store::v_get(
        ppg_stats_handle, SWITCH_PORT_PPG_STATS_ATTR_STATUS, status_list);
    bool port_ppg_stats_created = false;
    for (auto const st : status_list) {
      port_ppg_stats_created = port_ppg_stats_created || st;
    }
    if (!port_ppg_stats_created) return status;
  }
  /*
   * one time clear ppg stats as icos to ppg mapping changed.
   * This is awkward. We need to ensure a port_ppg_stats asicObj is created for
   * all implementations
   */
  if (is_switch_pipe_port_ppg && port_ppg_stats_helper) {
    status |= port_ppg_stats_helper->counters_set(port_handle);
  }

  return status;
}

/*
 * parent_handle: ingress_qos_group
 */
class ingress_icos_ppg_qos_map_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_INGRESS_ICOS_PPG_QOS_MAP_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_INGRESS_ICOS_PPG_QOS_MAP_HELPER_ATTR_PARENT_HANDLE;

 public:
  ingress_icos_ppg_qos_map_helper(const switch_object_id_t parent,
                                  switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    switch_enum_t e = {0};
    switch_qos_map_ingress_attr_type type;

    status |= switch_store::v_get(parent, SWITCH_QOS_MAP_INGRESS_ATTR_TYPE, e);
    type = static_cast<switch_qos_map_ingress_attr_type>(e.enumdata);

    // FIXME(bfn): Ideally we should only be checking for one type here. But
    // currently the json schema defines two
    // different enum values for the same type. ICOS_TO_PPG is widely used in
    // API tests while PFC_PRIORITY_TO_PPG is more
    // consistent with SAI nomenclature and sai mappings and also with egress
    // type enums. This needs to be fixed to use
    // only one of the two enum values for the type mapping
    if (type != SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_ICOS_TO_PPG &&
        type != SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PFC_PRIORITY_TO_PPG) {
      return;
    }

    if (!switch_store::smiContext::context().in_warm_init()) {
      // walk through all port_handles and apply the update;
      std::set<switch_object_id_t> port_handles;

      status |= switch_store::referencing_set_get(
          parent, SWITCH_OBJECT_TYPE_PORT, port_handles);
      for (auto const &port_handle : port_handles) {
        status |= update_port_icos_to_ppg(parent, port_handle);
      }
    }

    return;
  }
};

/*
 * parent_handle: port
 */
class ingress_port_icos_ppg_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_INGRESS_PORT_ICOS_PPG_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_INGRESS_PORT_ICOS_PPG_HELPER_ATTR_PARENT_HANDLE;
  switch_object_id_t icos_to_ppg_qos_map_handle = {};
  switch_enum_t port_type = {};

 public:
  ingress_port_icos_ppg_helper(const switch_object_id_t parent,
                               switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    // switch_object_id_t icos_to_ppg_qos_map_handle = {};

    status |= switch_store::v_get(parent,
                                  SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE,
                                  icos_to_ppg_qos_map_handle);
    status |= switch_store::v_get(parent, SWITCH_PORT_ATTR_TYPE, port_type);
    return;
  }

  switch_status_t create_update() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    if (port_type.enumdata != SWITCH_PORT_ATTR_TYPE_NORMAL) return status;

    if (icos_to_ppg_qos_map_handle.data) {
      /*
       * port attached with icos_to_ppg qos map.
       * inform TM about non def ppg mapping .
       */
      status |=
          update_port_icos_to_ppg(icos_to_ppg_qos_map_handle, get_parent());
    } else {
      if (get_auto_oid().data) {
        /*
         * update case and qos_map removed from port.
         * now we need to re-map all icos to def ppg.
         */
        switch_object_id_t device_handle = {0};
        uint16_t dev_id = 0;
        status |= switch_store::v_get(
            get_parent(), SWITCH_PORT_ATTR_DEVICE, device_handle);
        status |= switch_store::v_get(
            device_handle, SWITCH_DEVICE_ATTR_DEV_ID, dev_id);

        status |= update_port_icos_to_default_ppg(dev_id, get_parent());
      }
    }

    status |= auto_object::create_update();
    return status;
  }
};

/*
 * parent_handle: egress_qos_group
 */
class egress_pfc_priority_queue_qos_map_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_EGRESS_PFC_PRIORITY_QUEUE_QOS_MAP_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_EGRESS_PFC_PRIORITY_QUEUE_QOS_MAP_HELPER_ATTR_PARENT_HANDLE;

 public:
  egress_pfc_priority_queue_qos_map_helper(const switch_object_id_t parent,
                                           switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    switch_enum_t e = {0};
    switch_qos_map_egress_attr_type type;

    status |= switch_store::v_get(parent, SWITCH_QOS_MAP_EGRESS_ATTR_TYPE, e);
    type = static_cast<switch_qos_map_egress_attr_type>(e.enumdata);

    if (type != SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_PFC_PRIORITY_TO_QUEUE) {
      return;
    }

    // walk through all port_handles and apply the update;
    std::set<switch_object_id_t> port_handles;

    status |= switch_store::referencing_set_get(
        parent, SWITCH_OBJECT_TYPE_PORT, port_handles);
    for (auto const &port_handle : port_handles) {
      status |= update_port_pfc_priority_to_queue(parent, port_handle);

      uint32_t pfc_map = 0;
      switch_enum_t _pfc_mode = {SWITCH_PORT_ATTR_PFC_MODE_COMBINED};
      status |= switch_store::v_get(
          port_handle, SWITCH_PORT_ATTR_PFC_MODE, _pfc_mode);
      if (_pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_SEPARATE) {
        status |= switch_store::v_get(
            port_handle, SWITCH_PORT_ATTR_TX_PFC_MAP, pfc_map);
      } else {
        status |=
            switch_store::v_get(port_handle, SWITCH_PORT_ATTR_PFC_MAP, pfc_map);
      }
      status |= queue_tail_drop_set(port_handle, parent, pfc_map);
    }

    return;
  }
};

class egress_port_pfc_priority_queue_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_EGRESS_PORT_PFC_PRIORITY_QUEUE_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_EGRESS_PORT_PFC_PRIORITY_QUEUE_HELPER_ATTR_PARENT_HANDLE;

 public:
  egress_port_pfc_priority_queue_helper(const switch_object_id_t parent,
                                        switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    uint32_t pfc_map = 0;
    switch_object_id_t device_handle = {0}, pfc_prio_to_queue_map{0};
    uint16_t dev_id = 0;
    uint16_t dev_port = 0;
    uint8_t index = 0;
    std::vector<switch_object_id_t> queue_list;

    status |=
        switch_store::v_get(parent,
                            SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,
                            pfc_prio_to_queue_map);
    status |= switch_store::v_get(parent, SWITCH_PORT_ATTR_PFC_MAP, pfc_map);
    if (pfc_prio_to_queue_map.data) {
      status |=
          update_port_pfc_priority_to_queue(pfc_prio_to_queue_map, parent);
      switch_enum_t _pfc_mode = {SWITCH_PORT_ATTR_PFC_MODE_COMBINED};
      status |=
          switch_store::v_get(parent, SWITCH_PORT_ATTR_PFC_MODE, _pfc_mode);
      if (_pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_SEPARATE) {
        status |=
            switch_store::v_get(parent, SWITCH_PORT_ATTR_TX_PFC_MAP, pfc_map);
      }
      status |= queue_tail_drop_set(parent, pfc_prio_to_queue_map, pfc_map);
    } else {
      // SONiC PFC WD manages pfc to queue mapping through
      // SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL
      // which in turn changes port pfc_map. So, let's use it for queue pfc cos
      // mapping set.
      status |=
          switch_store::v_get(parent, SWITCH_PORT_ATTR_DEVICE, device_handle);
      status |=
          switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, dev_id);
      status |=
          switch_store::v_get(parent, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
      status |= switch_store::v_get(
          parent, SWITCH_PORT_ATTR_QUEUE_HANDLES, queue_list);

      for (auto const queue : queue_list) {
        uint8_t pfc_priority = 0;
        uint8_t qid = 0;
        status |= switch_store::v_get(queue, SWITCH_QUEUE_ATTR_QUEUE_ID, qid);
        pfc_priority = ((1 << index) & (uint8_t)pfc_map) ? index : 0;
        status |= switch_pd_queue_pfc_cos_mapping(
            dev_id, dev_port, qid, pfc_priority);
        index++;
      }
    }
  }
};

inline uint32_t switch_pd_shaping_rate_kbps(uint64_t rate_bps) {
  uint32_t rate_kbps = ceil(rate_bps / 1000);
  return rate_kbps;
}

switch_status_t switch_pd_queue_guaranteed_rate_set(uint16_t device,
                                                    uint16_t dev_port,
                                                    uint8_t queue_id,
                                                    bool pps,
                                                    uint32_t burst_size,
                                                    uint64_t rate) {
  switch_status_t status = 0;
  uint32_t min_rate = 0;

  if (pps) {
    min_rate = rate;
  } else {
    min_rate = switch_pd_shaping_rate_kbps(rate);
  }
  status = p4_pd_tm_set_q_guaranteed_rate(
      device, dev_port, queue_id, pps, burst_size, min_rate);

  if (status != SWITCH_STATUS_SUCCESS) {
    return status;
  }

  if (min_rate) {
    status = p4_pd_tm_q_min_rate_shaper_enable(device, dev_port, queue_id);
  } else {
    status = p4_pd_tm_q_min_rate_shaper_disable(device, dev_port, queue_id);
  }

  return status;
}

switch_status_t switch_pd_queue_scheduling_dwrr_weight_set(uint16_t device,
                                                           uint16_t dev_port,
                                                           uint8_t queue_id,
                                                           uint16_t weight) {
  switch_status_t status = 0;

  uint16_t pd_weight = 0;
  if (weight == 0) {
    pd_weight = SWITCH_PD_QUEUE_DEFAULT_DWRR_WEIGHT;
  } else {
    pd_weight = SWITCH_PD_WEIGHT(weight);
  }
  status = p4_pd_tm_set_q_dwrr_weight(device, dev_port, queue_id, pd_weight);
  return status;
}

p4_pd_tm_sched_prio_t switch_scheduler_priority_to_pd_scheduler_priority(
    switch_scheduler_attr_priority priority) {
  switch (priority) {
    case SWITCH_SCHEDULER_ATTR_PRIORITY_LOW:
      return PD_TM_SCH_PRIO_0;
    case SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_1:
      return PD_TM_SCH_PRIO_1;
    case SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_2:
      return PD_TM_SCH_PRIO_2;
    case SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_3:
      return PD_TM_SCH_PRIO_3;
    case SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_4:
      return PD_TM_SCH_PRIO_4;
    case SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_5:
      return PD_TM_SCH_PRIO_5;
    case SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_6:
      return PD_TM_SCH_PRIO_6;
    case SWITCH_SCHEDULER_ATTR_PRIORITY_HIGH:
      return PD_TM_SCH_PRIO_7;
    default:
      return PD_TM_SCH_PRIO_LOW;
  }
}

switch_scheduler_attr_priority switch_scheduler_priority_from_index(
    uint32_t index) {
  switch (index) {
    case 0:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_LOW;
    case 1:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_1;
    case 2:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_2;
    case 3:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_3;
    case 4:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_4;
    case 5:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_5;
    case 6:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_NORMAL_6;
    case 7:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_HIGH;
    default:
      return SWITCH_SCHEDULER_ATTR_PRIORITY_LOW;
  }
}

switch_status_t switch_pd_queue_scheduling_strict_priority_set(
    uint16_t device,
    uint16_t dev_port,
    uint8_t queue_id,
    switch_scheduler_attr_priority priority) {
  switch_status_t status = 0;

  p4_pd_tm_sched_prio_t pd_scheduler_priority =
      switch_scheduler_priority_to_pd_scheduler_priority(priority);
  status = p4_pd_tm_set_q_remaining_bw_sched_priority(
      device, dev_port, queue_id, pd_scheduler_priority);
  return status;
}

switch_status_t switch_pd_port_shaping_set(uint16_t device,
                                           uint16_t dev_port,
                                           bool pps,
                                           uint32_t burst_size,
                                           uint64_t rate) {
  switch_status_t status = 0;
  uint32_t shaping_rate = 0;
  std::string str_rate;

  if (pps) {
    shaping_rate = rate;
    str_rate = "PPS";
  } else {
    shaping_rate = switch_pd_shaping_rate_kbps(rate);
    str_rate = "BPS";
  }
  status = bfrt_tm_port_sched_shaping_rate_set(
      dev_port, str_rate, shaping_rate, burst_size);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}:{}: failed to set scheduler shaping rate on device {} "
               "dev_port {} with status: {}",
               __func__,
               __LINE__,
               device,
               dev_port,
               switch_error_to_string(status));
    return status;
  }

  if (rate) {
    status = bfrt_tm_port_sched_cfg_max_rate_enable_set(dev_port, true);
  } else {
    status = bfrt_tm_port_sched_cfg_max_rate_enable_set(dev_port, false);
  }
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}:{}: failed to set scheduler max_rate_enable on device {} "
               "dev_port {} with status: {}",
               __func__,
               __LINE__,
               device,
               dev_port,
               switch_error_to_string(status));
    return status;
  }
  return status;
}

switch_status_t switch_pd_queue_shaping_set(uint16_t device,
                                            uint16_t dev_port,
                                            uint8_t queue_id,
                                            bool pps,
                                            uint32_t burst_size,
                                            uint64_t rate) {
  switch_status_t status = 0;
  uint32_t shaping_rate;

  if (pps) {
    shaping_rate = rate;
  } else {
    shaping_rate = switch_pd_shaping_rate_kbps(rate);
  }
  status = p4_pd_tm_set_q_shaping_rate(
      device, dev_port, queue_id, pps, burst_size, shaping_rate);

  if (status != SWITCH_STATUS_SUCCESS) {
    return status;
  }

  if (shaping_rate) {
    status = p4_pd_tm_q_max_rate_shaper_enable(device, dev_port, queue_id);
  } else {
    status = p4_pd_tm_q_max_rate_shaper_disable(device, dev_port, queue_id);
  }
  return status;
}

switch_status_t update_scheduler_profile(
    switch_object_id_t scheduler_handle,
    switch_scheduler_group_attr_type group_type,
    switch_object_id_t port_handle,
    switch_object_id_t queue_handle) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t device_handle = {0};
  uint16_t dev_id = 0, dev_port = 0;
  switch_enum_t shaper_type;
  uint64_t max_rate = 0;
  uint64_t max_burst_size = 0;

  status |= switch_store::v_get(
      scheduler_handle, SWITCH_SCHEDULER_ATTR_DEVICE, device_handle);
  status |=
      switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, dev_id);
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);

  status |= switch_store::v_get(
      scheduler_handle, SWITCH_SCHEDULER_ATTR_MAX_RATE, max_rate);
  status |= switch_store::v_get(
      scheduler_handle, SWITCH_SCHEDULER_ATTR_MAX_BURST_SIZE, max_burst_size);
  status |= switch_store::v_get(
      scheduler_handle, SWITCH_SCHEDULER_ATTR_SHAPER_TYPE, shaper_type);

  if (group_type == SWITCH_SCHEDULER_GROUP_ATTR_TYPE_PORT) {
    /* configure port shaper profile. */
    status |= switch_pd_port_shaping_set(
        dev_id,
        dev_port,
        (shaper_type.enumdata == SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_PPS),
        max_burst_size,
        max_rate);
  } else {
    /* configure queue sch/shaper profile */
    switch_enum_t sch_type;
    uint8_t qid;

    status |= switch_store::v_get(
        scheduler_handle, SWITCH_SCHEDULER_ATTR_TYPE, sch_type);

    status |=
        switch_store::v_get(queue_handle, SWITCH_QUEUE_ATTR_QUEUE_ID, qid);

    // Extracting existing queue_priotity_list for the given port
    attr_w attribute_list(SWITCH_PORT_ATTR_QUEUE_PRIORITIES);
    status |= switch_store::attribute_get(
        port_handle, SWITCH_PORT_ATTR_QUEUE_PRIORITIES, attribute_list);
    std::vector<uint8_t> queue_priority_list;
    attribute_list.v_get(queue_priority_list);
    switch_scheduler_attr_priority queue_priority =
        SWITCH_SCHEDULER_ATTR_PRIORITY_LOW;

    /* configure queue priority based on sch type.*/
    if (sch_type.enumdata == SWITCH_SCHEDULER_ATTR_TYPE_STRICT) {
      // First queue in strict priority will start from highest priority
      std::set<uint8_t> strict_queue;
      for (uint8_t idx = 0; idx < queue_priority_list.size(); ++idx) {
        if (queue_priority_list[idx] != 0) strict_queue.emplace(idx);
      }
      strict_queue.emplace(qid);

      if (queue_priority_list.empty()) {
        queue_priority_list = {0, 0, 0, 0, 0, 0, 0, 0};
      }

      uint32_t priority_idx = SWITCH_SCHEDULER_ATTR_PRIORITY_HIGH;
      for (auto rit = strict_queue.crbegin(); rit != strict_queue.crend();
           ++rit) {
        queue_priority = switch_scheduler_priority_from_index(priority_idx);
        queue_priority_list[*rit] = queue_priority;
        --priority_idx;

        status |= switch_pd_queue_scheduling_strict_priority_set(
            dev_id, dev_port, *rit, queue_priority);
      }

      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_NONE,
                   "{}:{}: failed to update scheduler info on device {} "
                   "dev_port {} queue {} strict "
                   "priority set failed for sch handle {:#x} {}",
                   __func__,
                   __LINE__,
                   dev_id,
                   dev_port,
                   qid,
                   scheduler_handle.data,
                   switch_error_to_string(status));
        return status;
      }
      // Updating the queue priority list
      attr_w queue_attr_list(SWITCH_PORT_ATTR_QUEUE_PRIORITIES);
      queue_attr_list.v_set(queue_priority_list);
      switch_store::attribute_set(port_handle, queue_attr_list);
    } else if (sch_type.enumdata == SWITCH_SCHEDULER_ATTR_TYPE_DWRR) {
      uint16_t weight = 0;

      status |= switch_store::v_get(
          scheduler_handle, SWITCH_SCHEDULER_ATTR_WEIGHT, weight);
      assert(weight && "SCH type DWRR with invalid weight!");
      status |= switch_pd_queue_scheduling_dwrr_weight_set(
          dev_id, dev_port, qid, weight);
      status |= switch_pd_queue_scheduling_strict_priority_set(
          dev_id, dev_port, qid, SWITCH_SCHEDULER_ATTR_PRIORITY_LOW);

      // Updating the queue priority list
      if (!queue_priority_list.empty()) {
        attr_w queue_attr_list(SWITCH_PORT_ATTR_QUEUE_PRIORITIES);
        queue_priority_list[qid] = queue_priority;
        queue_attr_list.v_set(queue_priority_list);
        switch_store::attribute_set(port_handle, queue_attr_list);
      }
    }

    status |= switch_pd_queue_shaping_set(
        dev_id,
        dev_port,
        qid,
        shaper_type.enumdata == SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_PPS,
        max_burst_size,
        max_rate);

    uint64_t min_rate = 0;
    uint64_t min_burst_size = 0;
    status |= switch_store::v_get(
        scheduler_handle, SWITCH_SCHEDULER_ATTR_MIN_RATE, min_rate);
    status |= switch_store::v_get(
        scheduler_handle, SWITCH_SCHEDULER_ATTR_MIN_BURST_SIZE, min_burst_size);
    status |= switch_pd_queue_guaranteed_rate_set(
        dev_id,
        dev_port,
        qid,
        shaper_type.enumdata == SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_PPS,
        min_burst_size,
        min_rate);
  }

  return status;
}

/*
 * scheduler_group1---+ scheduler
 *                         +
 *                         |
 * scheduler_group2------- +
 */
class scheduler_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_SCHEDULER_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_SCHEDULER_HELPER_ATTR_PARENT_HANDLE;

 public:
  scheduler_helper(const switch_object_id_t parent, switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    (void)status;
  }

  switch_status_t create_update() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    std::set<switch_object_id_t> sch_grps;
    switch_object_id_t sch_profile_handle = {};
    switch_object_id_t port_handle = {};
    switch_object_id_t queue_handle = {};
    switch_enum_t grp_type = {};

    status |= switch_store::referencing_set_get(
        get_parent(), SWITCH_OBJECT_TYPE_SCHEDULER_GROUP, sch_grps);
    for (auto const &sch_grp : sch_grps) {
      status |=
          switch_store::v_get(sch_grp,
                              SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                              sch_profile_handle);
      status |= switch_store::v_get(
          sch_grp, SWITCH_SCHEDULER_GROUP_ATTR_PORT_HANDLE, port_handle);
      status |= switch_store::v_get(
          sch_grp, SWITCH_SCHEDULER_GROUP_ATTR_QUEUE_HANDLE, queue_handle);
      status |= switch_store::v_get(
          sch_grp, SWITCH_SCHEDULER_GROUP_ATTR_TYPE, grp_type);
      status |= update_scheduler_profile(
          sch_profile_handle,
          static_cast<switch_scheduler_group_attr_type>(grp_type.enumdata),
          port_handle,
          queue_handle);
    }

    auto_object::create_update();

    return status;
  }
};

class scheduler_group_helper : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_SCHEDULER_GROUP_HELPER;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_SCHEDULER_GROUP_HELPER_ATTR_PARENT_HANDLE;

 public:
  scheduler_group_helper(const switch_object_id_t parent,
                         switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    (void)status;
  }

  switch_status_t create_update() {
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    switch_object_id_t sch_profile_handle = {0};
    switch_object_id_t port_handle;
    switch_object_id_t queue_handle;
    switch_enum_t grp_type;

    status |= switch_store::v_get(get_parent(),
                                  SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                                  sch_profile_handle);
    if (sch_profile_handle.data) {
      status |= switch_store::v_get(
          get_parent(), SWITCH_SCHEDULER_GROUP_ATTR_PORT_HANDLE, port_handle);
      status |= switch_store::v_get(
          get_parent(), SWITCH_SCHEDULER_GROUP_ATTR_QUEUE_HANDLE, queue_handle);
      status |= switch_store::v_get(
          get_parent(), SWITCH_SCHEDULER_GROUP_ATTR_TYPE, grp_type);
      status |= update_scheduler_profile(
          sch_profile_handle,
          static_cast<switch_scheduler_group_attr_type>(grp_type.enumdata),
          port_handle,
          queue_handle);
    }

    auto_object::create_update();
    return status;
  }
};

/*
 * This API queries driver for default PPG and Queue buffer values
 * and create a default ingress and egress buffer profile.
 */
switch_status_t create_default_buffer_profile(
    switch_object_id_t device_handle,
    switch_object_id_t *ingress_buffer_profile,
    switch_object_id_t *egress_buffer_profile) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  const switch_object_type_t object_type = SWITCH_OBJECT_TYPE_BUFFER_PROFILE;
  uint64_t buffer_size = 0;
  uint32_t ppg_handle = SWITCH_ASIC_FIRST_DEFAULT_PPG_HANDLE;
  uint32_t portid = 0, qid = 0;
  p4_pd_status_t pd_status = 0;
  uint16_t dev_id = 0;
  uint32_t buffer_size_cells = 0;

  for (int i = 0; i < 2; i++) {
    switch_object_id_t hdl;
    if (i == 0) {
      status |= switch_store::v_get(
          device_handle,
          SWITCH_DEVICE_ATTR_DEFAULT_INGRESS_BUFFER_PROFILE_HANDLE,
          hdl);
      if (hdl.data) {
        // default buffer profile handle already exists
        *ingress_buffer_profile = hdl;
        continue;
      }
      pd_status = p4_pd_tm_get_ppg_guaranteed_min_limit(
          dev_id, ppg_handle, &buffer_size_cells);
      if (pd_status != SWITCH_STATUS_SUCCESS) {
        status = SWITCH_STATUS_FAILURE;
        switch_log(SWITCH_API_LEVEL_INFO,
                   SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                   "{}:{}: Failed to get default ppg gmin limit pd_status {}",
                   __func__,
                   __LINE__,
                   pd_status);
        return status;
      }
    } else {
      status |= switch_store::v_get(
          device_handle,
          SWITCH_DEVICE_ATTR_DEFAULT_EGRESS_BUFFER_PROFILE_HANDLE,
          hdl);
      if (hdl.data) {
        // default buffer profile handle already exists
        *egress_buffer_profile = hdl;
        continue;
      }
      pd_status = p4_pd_tm_get_q_guaranteed_min_limit(
          dev_id, portid, qid, &buffer_size_cells);
      if (pd_status != SWITCH_STATUS_SUCCESS) {
        status = SWITCH_STATUS_FAILURE;
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_QUEUE,
                   "{}:{}: Failed to get default q gmin limit pd_status {} "
                   "dev_port {} queue_id {}",
                   __func__,
                   __LINE__,
                   pd_status,
                   portid,
                   qid);
        return status;
      }
    }
    buffer_size = 0;
    compute_pd_buffer_cells_to_bytes(dev_id, buffer_size_cells, &buffer_size);

    std::set<attr_w> attrs_key;
    attrs_key.insert(attr_w(SWITCH_BUFFER_PROFILE_ATTR_DEVICE, device_handle));
    switch_enum_t th_mode = {
        .enumdata = SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE_DYNAMIC};
    attrs_key.insert(
        attr_w(SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD_MODE, th_mode));
    attrs_key.insert(
        attr_w(SWITCH_BUFFER_PROFILE_ATTR_BUFFER_SIZE, buffer_size));
    attrs_key.insert(
        attr_w(SWITCH_BUFFER_PROFILE_ATTR_THRESHOLD,
               static_cast<uint64_t>(SWITCH_BUFFER_MAX_THRESHOLD)));
    switch_object_id_t buffer_profile_handle = {0};
    status = switch_store::object_create(
        object_type, attrs_key, buffer_profile_handle);
    if (i == 0) {
      *ingress_buffer_profile = buffer_profile_handle;
    } else {
      *egress_buffer_profile = buffer_profile_handle;
    }
  }
  return status;
}

void delete_ppg_object(const switch_object_id_t object_id,
                       std::promise<switch_status_t> status) {
  switch_status_t temp_status = SWITCH_STATUS_SUCCESS;
  temp_status = switch_store::object_delete(object_id);
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
               "{}.{}: Failed to delete internal ppg {}"
               "status {}",
               __func__,
               __LINE__,
               object_id,
               temp_status);
  }
  status.set_value(temp_status);
  return;
}

switch_status_t before_ppg_delete_delete_internal_ports_ppg(
    const switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  bool is_internal_object = true;
  switch_object_id_t port_handle{};
  status |= switch_store::v_get(
      object_id, SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE, port_handle);
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_INTERNAL_OBJECT, is_internal_object);
  if (is_internal_object) return status;

  switch_object_id_t device{};
  status |= switch_store::v_get(
      object_id, SWITCH_PORT_PRIORITY_GROUP_ATTR_DEVICE, device);
  // Get Internal Port List for this PPG
  std::vector<switch_object_id_t> internal_ports;
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST, internal_ports);
  // Get PPG Index for PPG
  uint8_t index = 0;
  status |= switch_store::v_get(
      object_id, SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX, index);
  // For Each Port in Internal Port List
  //  Get PPG with PPG Index
  std::vector<switch_object_id_t> ppg_handles;
  for (auto &port : internal_ports) {
    std::set<attr_w> ppg_attrs;
    switch_object_id_t ppg_handle = {};
    ppg_attrs.insert(attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_DEVICE, device));
    ppg_attrs.insert(attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE, port));
    ppg_attrs.insert(attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX, index));
    status |= switch_store::object_id_get_wkey(
        SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP, ppg_attrs, ppg_handle);
    if (ppg_handle.data) ppg_handles.push_back(ppg_handle);
  }

  for (const auto handle : ppg_handles) {
    std::promise<switch_status_t> dstatus;
    auto delete_status = dstatus.get_future();
    std::thread internal_port_delete(
        &delete_ppg_object, handle, std::move(dstatus));
    internal_port_delete.join();
    status |= delete_status.get();
  }
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
               "{}.{}: Failed to delete internal ppgs for external ppg {}"
               "status {}",
               __func__,
               __LINE__,
               object_id,
               status);
  }
  return status;
}

static void update_internal_ppg(const switch_object_id_t object_id,
                                const attr_w &attr,
                                std::promise<switch_status_t> status) {
  switch_status_t temp_status = SWITCH_STATUS_SUCCESS;
  temp_status = switch_store::attribute_set(object_id, attr);
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    // switch_log(SWITCH_API_LEVEL_ERROR,
    //            SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
    //            "{}.{}: Failure to update internal ppg {} attr {}"
    //            "status {}",
    //            __func__,
    //            __LINE__,
    //            object_id,
    //            attr,
    //            temp_status);
  }
  status.set_value(temp_status);
  return;
}

void create_ppg_object(std::set<attr_w> &ppg_attrs,
                       std::promise<switch_status_t> status,
                       std::promise<switch_object_id_t> oid) {
  switch_status_t _status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t _oid{};
  _status = switch_store::object_create(
      SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP, ppg_attrs, _oid);
  status.set_value(_status);
  if (_status != SWITCH_STATUS_SUCCESS) return;
  oid.set_value(_oid);
  return;
}
void cleanup_ppg_object(std::vector<switch_object_id_t> &ppg_handles,
                        std::promise<switch_status_t> status) {
  switch_status_t temp_status = SWITCH_STATUS_SUCCESS;
  for (auto ppg_handle : ppg_handles) {
    switch_status_t _status = SWITCH_STATUS_SUCCESS;
    _status = switch_store::object_delete(ppg_handle);
    if (_status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}:{}: Ppg cleanup failed for internal ppg {}:{}",
                 __func__,
                 __LINE__,
                 ppg_handle,
                 _status);
      temp_status |= _status;
    }
  }
  status.set_value(temp_status);
  return;
}

switch_status_t before_ppg_create_add_internal_ports_ppg(
    const switch_object_type_t object_type, std::set<attr_w> &attrs) {
  (void)object_type;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (switch_store::smiContext::context().in_warm_init()) return status;
  switch_object_id_t port_handle{};
  auto port_handle_it = attrs.find(static_cast<switch_attr_id_t>(
      SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE));
  if (port_handle_it == attrs.end()) {
    status = SWITCH_STATUS_FAILURE;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
               "{}:{}: attr port_handle missing",
               __func__,
               __LINE__);
    return status;
  } else {
    status = port_handle_it->v_get(port_handle);
    if (status != SWITCH_STATUS_SUCCESS) return status;
  }
  bool is_internal_object = false;
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_INTERNAL_OBJECT, is_internal_object);
  if (status != SWITCH_STATUS_SUCCESS || is_internal_object) {
    return status;
  }
  uint8_t ppg_index{SWITCH_DEFAULT_PPG_INDEX};
  auto ppg_index_it = attrs.find(
      static_cast<switch_attr_id_t>(SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX));
  if (ppg_index_it == attrs.end()) {
    status = SWITCH_STATUS_FAILURE;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
               "{}:{}: attr ppg_index missing",
               __func__,
               __LINE__);
    return status;
  } else {
    status = ppg_index_it->v_get(ppg_index);
    // Default PPG is created implicitly for every port internal/external when
    // the port is created, no need to create it again
    if (ppg_index == SWITCH_DEFAULT_PPG_INDEX) return status;
  }
  std::vector<switch_object_id_t> internal_port_list;
  status = switch_store::v_get(port_handle,
                               SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST,
                               internal_port_list);

  // Create Internal Pipe Port PPGs as internal objects
  auto internal_ppg_attrs = attrs;
  auto is_internal_ppg_it =
      internal_ppg_attrs.find(static_cast<switch_attr_id_t>(
          SWITCH_PORT_PRIORITY_GROUP_ATTR_INTERNAL_OBJECT));
  if (is_internal_ppg_it != internal_ppg_attrs.end()) {
    internal_ppg_attrs.erase(is_internal_ppg_it);
  }
  attr_w ppg_is_internal_attr(SWITCH_PORT_PRIORITY_GROUP_ATTR_INTERNAL_OBJECT);
  ppg_is_internal_attr.v_set(true);
  internal_ppg_attrs.insert(ppg_is_internal_attr);

  std::vector<switch_object_id_t> internal_ppg_handles;
  for (auto port : internal_port_list) {
    auto ppg_port_handle_it =
        internal_ppg_attrs.find(SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE);
    if (ppg_port_handle_it != internal_ppg_attrs.end()) {
      internal_ppg_attrs.erase(ppg_port_handle_it);
    }
    attr_w ppg_port_handle_attr(SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE);
    ppg_port_handle_attr.v_set(port);
    internal_ppg_attrs.insert(ppg_port_handle_attr);
    std::promise<switch_status_t> fstatus;
    std::promise<switch_object_id_t> foid;
    auto oid = foid.get_future();
    auto ppg_status = fstatus.get_future();
    std::thread internal_ppg_create(&create_ppg_object,
                                    std::ref(internal_ppg_attrs),
                                    std::move(fstatus),
                                    std::move(foid));
    internal_ppg_create.join();
    status = ppg_status.get();
    if (status != SWITCH_STATUS_SUCCESS) {
      // switch_log(SWITCH_API_LEVEL_ERROR,
      //            SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
      //            "{}:{}: Failed to add internal ppg for port {} corresponding "
      //            "to external port {} with attrs {}",
      //            __func__,
      //            __LINE__,
      //            port,
      //            port_handle,
      //            attrs,
      //            status);
      goto cleanup;
    }
    switch_object_id_t internal_ppg_handle{oid.get()};
    internal_ppg_handles.push_back(internal_ppg_handle);
  }
  return status;
cleanup:
  std::promise<switch_status_t> fcstatus;
  auto cleanup_status = fcstatus.get_future();
  std::thread internal_ppg_cleanup(
      &cleanup_ppg_object, std::ref(internal_ppg_handles), std::move(fcstatus));
  internal_ppg_cleanup.join();
  status = cleanup_status.get();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}:{}: Internal PPGs cleanup "
               "failed:{}",
               __func__,
               __LINE__,
               status);
  }
  return status;
}

switch_status_t before_ppg_update_update_internal_ports_ppg(
    const switch_object_id_t object_id, const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  if (switch_store::smiContext::context().in_warm_init()) return status;
  const auto attr_id = attr.id_get();
  switch (attr_id) {
    // WE update only the following attrs for internal pipe port ppgs
    case SWITCH_PORT_PRIORITY_GROUP_ATTR_BUFFER_PROFILE_HANDLE:
    case SWITCH_PORT_PRIORITY_GROUP_ATTR_LOSSLESS_ADMIN_ENABLE:
    case SWITCH_PORT_PRIORITY_GROUP_ATTR_SKID_LIMIT:
    case SWITCH_PORT_PRIORITY_GROUP_ATTR_SKID_HYSTERISIS:
      break;
    default:
      return status;
  }
  switch_object_id_t port_handle{}, device{};
  // Get Port for this PPG
  status |= switch_store::v_get(
      object_id, SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE, port_handle);
  status |= switch_store::v_get(
      object_id, SWITCH_PORT_PRIORITY_GROUP_ATTR_DEVICE, device);
  // Return if not internal port or warm init
  bool is_internal_port = true;
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_INTERNAL_OBJECT, is_internal_port);
  if (is_internal_port || switch_store::smiContext::context().in_warm_init())
    return status;
  // Get Internal Port List for this PPG
  std::vector<switch_object_id_t> internal_ports;
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST, internal_ports);
  // Get PPG Index for PPG
  uint8_t index = 0;
  status |= switch_store::v_get(
      object_id, SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX, index);
  // For Each Port in Internal Port List
  //  Get PPG with PPG Index
  std::vector<switch_object_id_t> ppg_handles;
  for (auto &port : internal_ports) {
    std::set<attr_w> ppg_attrs;
    switch_object_id_t ppg_handle = {};
    ppg_attrs.insert(attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_DEVICE, device));
    ppg_attrs.insert(attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE, port));
    ppg_attrs.insert(attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX, index));
    status |= switch_store::object_id_get_wkey(
        SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP, ppg_attrs, ppg_handle);
    if (ppg_handle.data) ppg_handles.push_back(ppg_handle);
  }
  if (status != SWITCH_STATUS_SUCCESS) {
    // switch_log(SWITCH_API_LEVEL_ERROR,
    //            SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
    //            "{}.{}: Failure to update attr {} for internal ppgs "
    //            "corresponding to external port {} ppg {}"
    //            "status {}",
    //            __func__,
    //            __LINE__,
    //            attr,
    //            port_handle,
    //            object_id,
    //            status);
  }
  if (ppg_handles.empty()) return status;
  //  Attribute Set
  for (auto &ppg : ppg_handles) {
    std::promise<switch_status_t> fstatus;
    auto update_status = fstatus.get_future();
    std::thread internal_ppg_update(
        &update_internal_ppg, ppg, std::ref(attr), std::move(fstatus));
    internal_ppg_update.join();
    status |= update_status.get();
  }
  if (status != SWITCH_STATUS_SUCCESS) {
    // switch_log(SWITCH_API_LEVEL_ERROR,
    //            SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
    //            "{}.{}: Failure to update attr {} for internal ppgs "
    //            "corresponding to external port {} ppg {}"
    //            "status {}",
    //            __func__,
    //            __LINE__,
    //            attr,
    //            port_handle,
    //            object_id,
    //            status);
  }
  return status;
}

/*
 * attaching scheduler directly to the queue must result in
 * immediate setting of scheduling parameters in HW
 */
switch_status_t after_scheduler_added_to_queue(
    const switch_object_id_t queue_handle, const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t sch_profile_handle = {0};
  switch_object_id_t port_handle = {0};

  if (attr.id_get() == SWITCH_QUEUE_ATTR_SCHEDULER_HANDLE) {
    status |= switch_store::v_get(
        queue_handle, SWITCH_QUEUE_ATTR_SCHEDULER_HANDLE, sch_profile_handle);

    if (sch_profile_handle.data) {
      status |= switch_store::v_get(
          queue_handle, SWITCH_QUEUE_ATTR_PORT_HANDLE, port_handle);
      status |= update_scheduler_profile(sch_profile_handle,
                                         SWITCH_SCHEDULER_GROUP_ATTR_TYPE_QUEUE,
                                         port_handle,
                                         queue_handle);
    }
    if (status != SWITCH_STATUS_SUCCESS) {
      // switch_log(SWITCH_API_LEVEL_ERROR,
      //            SWITCH_OBJECT_TYPE_QUEUE,
      //            "{}.{}: Failure to update attr {} for a queue {} "
      //            "corresponding to port {}"
      //            "status {}",
      //            __func__,
      //            __LINE__,
      //            attr,
      //            queue_handle,
      //            port_handle,
      //            status);
    }
  }
  return status;
}

/*
 * detaching scheduler from queue must result in resetting all scheduling
 * parameters in HW
 */
switch_status_t before_scheduler_removed_from_queue(
    const switch_object_id_t queue_handle, const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t sch_new_handle = {0}, sch_curr_handle = {0};

  if (attr.id_get() == SWITCH_QUEUE_ATTR_SCHEDULER_HANDLE) {
    status |= attr.v_get(sch_new_handle);

    // check if existing scheduler is detached from queue
    // (if it is replaced with new valid handle,
    //  after_scheduler_added_to_queue() will take care)
    if (sch_new_handle.data == 0) {
      switch_object_id_t port_handle = {0}, device_handle = {0};
      uint16_t dev_id = 0, dev_port = 0;
      uint8_t q_id = 0;
      status |= switch_store::v_get(
          queue_handle, SWITCH_QUEUE_ATTR_SCHEDULER_HANDLE, sch_curr_handle);
      status |= switch_store::v_get(
          sch_curr_handle, SWITCH_SCHEDULER_ATTR_DEVICE, device_handle);
      status |=
          switch_store::v_get(queue_handle, SWITCH_QUEUE_ATTR_QUEUE_ID, q_id);
      status |= switch_store::v_get(
          queue_handle, SWITCH_QUEUE_ATTR_PORT_HANDLE, port_handle);
      status |=
          switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, dev_id);
      status |=
          switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
      // reset all scheduling parameters - is the below set enough?
      status |=
          switch_pd_queue_shaping_set(dev_id,
                                      dev_port,
                                      q_id,
                                      SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_PPS,
                                      0 /* max_burst_size */,
                                      0 /* max_rate */);
      status |= switch_pd_queue_guaranteed_rate_set(
          dev_id,
          dev_port,
          q_id,
          SWITCH_SCHEDULER_ATTR_SHAPER_TYPE_PPS,
          0 /* min_burst_size */,
          0 /* min_rate */);

      if (status != SWITCH_STATUS_SUCCESS) {
        // switch_log(SWITCH_API_LEVEL_ERROR,
        //            SWITCH_OBJECT_TYPE_QUEUE,
        //            "{}.{}: Failure to detach scheduler from a queue {} "
        //            "corresponding to port {}"
        //            "status {}",
        //            __func__,
        //            __LINE__,
        //            queue_handle,
        //            port_handle,
        //            status);
      }
    }
  }
  return status;
}

switch_status_t qos_pdfixed_init() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  REGISTER_OBJECT(buffer_pool_helper, SWITCH_OBJECT_TYPE_BUFFER_POOL_HELPER);
  REGISTER_OBJECT(ppg_helper, SWITCH_OBJECT_TYPE_PPG_HELPER);
  REGISTER_OBJECT(scheduler_helper, SWITCH_OBJECT_TYPE_SCHEDULER_HELPER);
  REGISTER_OBJECT(scheduler_group_helper,
                  SWITCH_OBJECT_TYPE_SCHEDULER_GROUP_HELPER);
  REGISTER_OBJECT(ingress_icos_ppg_qos_map_helper,
                  SWITCH_OBJECT_TYPE_INGRESS_ICOS_PPG_QOS_MAP_HELPER);
  REGISTER_OBJECT(ingress_port_icos_ppg_helper,
                  SWITCH_OBJECT_TYPE_INGRESS_PORT_ICOS_PPG_HELPER);
  REGISTER_OBJECT(egress_pfc_priority_queue_qos_map_helper,
                  SWITCH_OBJECT_TYPE_EGRESS_PFC_PRIORITY_QUEUE_QOS_MAP_HELPER);
  REGISTER_OBJECT(egress_port_pfc_priority_queue_helper,
                  SWITCH_OBJECT_TYPE_EGRESS_PORT_PFC_PRIORITY_QUEUE_HELPER);
  REGISTER_OBJECT(queue_buffer_config, SWITCH_OBJECT_TYPE_QUEUE_BUFFER_CONFIG);
  REGISTER_OBJECT(port_qos_helper, SWITCH_OBJECT_TYPE_PORT_QOS_HELPER);
  status |= switch_store::reg_update_trigs_after(
      SWITCH_OBJECT_TYPE_QUEUE, &after_scheduler_added_to_queue);
  if (feature::is_feature_set(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE)) {
    status |= switch_store::reg_create_trigs_before(
        SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
        &before_ppg_create_add_internal_ports_ppg);
    status |= switch_store::reg_update_trigs_before(
        SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
        &before_ppg_update_update_internal_ports_ppg);
    status |= switch_store::reg_delete_trigs_before(
        SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
        &before_ppg_delete_delete_internal_ports_ppg);
  }
  status |= switch_store::reg_update_trigs_before(
      SWITCH_OBJECT_TYPE_QUEUE, &before_scheduler_removed_from_queue);
  status |= switch_store::reg_update_trigs_after(
      SWITCH_OBJECT_TYPE_QUEUE, &after_scheduler_added_to_queue);
  return status;
}

switch_status_t qos_pdfixed_clean() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_type_t ot = SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP;
  switch_object_id_t ppg_hdl = {};

  status = switch_store::object_get_first_handle(ot, ppg_hdl);
  if (status != SWITCH_STATUS_SUCCESS) {
    if (status == SWITCH_STATUS_ITEM_NOT_FOUND) {
      // No objects of this type, return
      return SWITCH_STATUS_SUCCESS;
    }
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_NONE,
               "{}: failed to get first handle for object_type={} status={}",
               __func__,
               ot,
               status);
    return status;
  }
  status |= switch_store::v_set(
      ppg_hdl, SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW, false);
  bool next_cycle = false;
  do {
    std::vector<switch_object_id_t> next_object_handles;
    uint32_t out_num_handles = 0;
    next_cycle = false;
    switch_status_t st = SWITCH_STATUS_SUCCESS;

    st = switch_store::object_get_next_handles(
        ppg_hdl, 128, next_object_handles, out_num_handles);
    if (st != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_NONE,
                 "{}: PPG cleanup Error:failed to get next handle for "
                 "object_type={} status={}",
                 __func__,
                 ot,
                 status);
      return SWITCH_STATUS_SUCCESS;
    }
    for (auto it = next_object_handles.begin(); it != next_object_handles.end();
         it++) {
      ppg_hdl = *it;
      status |= switch_store::v_set(
          ppg_hdl, SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW, false);
    }
    if (next_object_handles.size() == 128) {
      next_cycle = true;
    }
  } while (next_cycle == true);
  return status;
}

}  // namespace smi
