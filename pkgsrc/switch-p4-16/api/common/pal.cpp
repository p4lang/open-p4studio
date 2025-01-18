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


extern "C" {
#include <stdio.h>
#include <tofino/bf_pal/bf_pal_port_intf.h>
#include <bf_pm/bf_pm_intf.h>
#include <tofino/bf_pal/bf_pal_types.h>
#include <tofino/pdfixed/pd_devport_mgr.h>
#include <tofino/pdfixed/pd_tm.h>
#include <lld/bf_lld_if.h>
#include <tofino/bf_pal/pltfm_intf.h>
#include <bf_types/bf_types.h>
#include <lld/lld_sku.h>
#include <traffic_mgr/traffic_mgr_sch_intf.h>
#include <port_mgr/bf_tof2_serdes_if.h>
#include <port_mgr/bf_serdes_if.h>
#include <port_mgr/bf_port_if.h>
#include <pipe_mgr/pktgen_intf.h>
}

#include <vector>
#include <set>
#include <unordered_set>
#include <mutex>   // NOLINT(build/c++11)
#include <thread>  // NOLINT(build/c++11)
#include <future>  // NOLINT(build/c++11)
#include <utility>
#include <algorithm>
#include <string>
#include <map>

#include "common/utils.h"
#include "common/qos_pdfixed.h"
#include "common/bfrt_tm.h"

// For internal Pipe ports in asymmetric folded pipeline the number of non
// default ppgs created is 0.
#define INTERNAL_PORT_NUM_NON_DEF_PPGS 0

namespace smi {
using namespace smi::bf_rt;  // NOLINT(build/namespaces)
using ::smi::logging::switch_log;

switch_status_t adv_speed_update(const switch_object_id_t port_handle,
                                 const uint64_t port_id,
                                 const bf_dev_id_t bf_dev_id,
                                 const bf_dev_port_t bf_dev_port);
switch_status_t adv_fec_type_update(const switch_object_id_t port_handle,
                                    const uint64_t port_id,
                                    const bf_dev_id_t bf_dev_id,
                                    const bf_dev_port_t bf_dev_port);

switch_status_t speed_set(switch_object_id_t port_handle,
                          uint64_t port_id,
                          bf_dev_id_t bf_dev_id,
                          bf_dev_port_t bf_dev_port,
                          uint32_t speed);

bf_port_speed_t port_speed_to_pal_port_speed(bf_dev_id_t dev_id,
                                             switch_object_id_t port_handle,
                                             uint32_t speed) {
  switch (speed) {
    case 1000:
      return BF_SPEED_1G;
    case 10000:
      return BF_SPEED_10G;
    case 25000:
      return BF_SPEED_25G;
    case 40000: {
      std::vector<uint16_t> lanes;
      switch_status_t status =
          switch_store::v_get(port_handle, SWITCH_PORT_ATTR_LANE_LIST, lanes);
      if (status != SWITCH_STATUS_SUCCESS) return BF_SPEED_NONE;
      return (lanes.size() == 2) ? BF_SPEED_40G_R2 : BF_SPEED_40G;
    }
    case 50000: {
      bf_dev_family_t dev_family =
          bf_dev_type_to_family(lld_sku_get_dev_type(dev_id));
      std::vector<uint16_t> lanes;
      switch_status_t status;
      if (dev_family == BF_DEV_FAMILY_TOFINO) return BF_SPEED_50G;
      status =
          switch_store::v_get(port_handle, SWITCH_PORT_ATTR_LANE_LIST, lanes);
      if (status != SWITCH_STATUS_SUCCESS) return BF_SPEED_NONE;
      if (dev_family == BF_DEV_FAMILY_TOFINO3) {
        return BF_SPEED_50G;
      }
      return (lanes.size() == 2) ? BF_SPEED_50G_CONS : BF_SPEED_50G;
    }
    case 100000:
      return BF_SPEED_100G;
    case 200000:
      return BF_SPEED_200G;
    case 400000:
      return BF_SPEED_400G;
    default:
      return BF_SPEED_NONE;
  }
}

uint32_t pal_port_speed_to_switch_port_speed(const bf_port_speed_t pal_speed) {
  std::map<uint32_t, uint32_t> bf_speed_map{{BF_SPEED_NONE, 0},
                                            {BF_SPEED_10G, 10000},
                                            {BF_SPEED_25G, 25000},
                                            {BF_SPEED_40G, 40000},
                                            {BF_SPEED_40G_R2, 40000},
                                            {BF_SPEED_50G, 50000},
                                            {BF_SPEED_50G_CONS, 50000},
                                            {BF_SPEED_100G, 100000},
                                            {BF_SPEED_200G, 200000},
                                            {BF_SPEED_400G, 400000}};

  if (bf_speed_map.find(pal_speed) != bf_speed_map.end()) {
    return bf_speed_map[pal_speed];
  } else {
    switch_log(SWITCH_API_LEVEL_WARN,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: Unsupported speed:{}",
               __func__,
               __LINE__,
               pal_speed);
    return 0;
  }
}

bf_port_speed_t switch_port_speed_to_pal_port_speed(bf_dev_id_t dev_id,
                                                    uint32_t port_speed,
                                                    uint32_t lane_count) {
  switch (port_speed) {
    case 0:
      return BF_SPEED_NONE;
    case 10000:
      return BF_SPEED_10G;
    case 25000:
      return BF_SPEED_25G;
    case 40000:
      return (lane_count == 2) ? BF_SPEED_40G_R2 : BF_SPEED_40G;
    case 50000: {
      if (lane_count == 2) {
        bf_dev_family_t dev_family =
            bf_dev_type_to_family(lld_sku_get_dev_type(dev_id));
        return (dev_family == BF_DEV_FAMILY_TOFINO ||
                dev_family == BF_DEV_FAMILY_TOFINO3)
                   ? BF_SPEED_50G
                   : BF_SPEED_50G_CONS;
      } else if (lane_count == 1) {
        return BF_SPEED_50G;
      } else {
        return BF_SPEED_NONE;
      }
    }
    case 100000:
      return BF_SPEED_100G;
    // tofino2
    case 200000:
      return BF_SPEED_200G;
    case 400000:
      return BF_SPEED_400G;
    default:
      return BF_SPEED_NONE;
  }
}

static const char *switch_port_speed_to_str(uint32_t speed) {
  std::map<uint32_t, std::string> str_speed_map{{0, "NONE"},
                                                {10000, "10G"},
                                                {25000, "25G"},
                                                {40000, "40G"},
                                                {50000, "50G"},
                                                {100000, "100G"},
                                                {200000, "200G"},
                                                {400000, "400G"}};

  if (str_speed_map.find(speed) != str_speed_map.end()) {
    return str_speed_map[speed].c_str();
  } else {
    return "NONE";
  }
}

static const char *bf_autoneg_to_str(bf_pm_port_autoneg_policy_e an) {
  switch (an) {
    case PM_AN_DEFAULT:
      return "PM_AN_DEFAULT";
    case PM_AN_FORCE_ENABLE:
      return "PM_AN_FORCE_ENABLE";
    case PM_AN_FORCE_DISABLE:
      return "PM_AN_FORCE_DISABLE";
    default:
      return "INVALID";
  }
}

bf_fec_type_t switch_port_fec_to_pal_fec(switch_port_attr_fec_type fec) {
  switch (fec) {
    case SWITCH_PORT_ATTR_FEC_TYPE_FC:
      return BF_FEC_TYP_FIRECODE;
    case SWITCH_PORT_ATTR_FEC_TYPE_RS:
      return BF_FEC_TYP_REED_SOLOMON;
    case SWITCH_PORT_ATTR_FEC_TYPE_NONE:
    default:
      return BF_FEC_TYP_NONE;
  }
}

switch_port_attr_fec_type pal_fec_to_switch_port_fec(bf_fec_type_t fec) {
  switch (fec) {
    case BF_FEC_TYP_FIRECODE:
      return SWITCH_PORT_ATTR_FEC_TYPE_FC;
    case BF_FEC_TYP_REED_SOLOMON:
      return SWITCH_PORT_ATTR_FEC_TYPE_RS;
    case BF_FEC_TYP_NONE:
    default:
      return SWITCH_PORT_ATTR_FEC_TYPE_NONE;
  }
}

static const char *switch_port_fec_to_str(switch_port_attr_fec_type fec) {
  switch (fec) {
    case SWITCH_PORT_ATTR_FEC_TYPE_NONE:
      return "NONE";
    case SWITCH_PORT_ATTR_FEC_TYPE_FC:
      return "FC";
    case SWITCH_PORT_ATTR_FEC_TYPE_RS:
      return "RS";
    default:
      return "NONE";
  }

  return "NONE";
}

static inline bf_loopback_mode_e switch_lb_mode_to_pd_lb_mode(
    switch_port_attr_loopback_mode lb_mode) {
  switch (lb_mode) {
    case SWITCH_PORT_ATTR_LOOPBACK_MODE_NONE:
      return BF_LPBK_NONE;
    case SWITCH_PORT_ATTR_LOOPBACK_MODE_MAC_NEAR:
      return BF_LPBK_MAC_NEAR;
    case SWITCH_PORT_ATTR_LOOPBACK_MODE_MAC_FAR:
      return BF_LPBK_MAC_FAR;
    case SWITCH_PORT_ATTR_LOOPBACK_MODE_PCS_NEAR:
      return BF_LPBK_PCS_NEAR;
    case SWITCH_PORT_ATTR_LOOPBACK_MODE_SERDES_NEAR:
      return BF_LPBK_SERDES_NEAR;
    case SWITCH_PORT_ATTR_LOOPBACK_MODE_SERDES_FAR:
      return BF_LPBK_SERDES_FAR;
    default:
      return BF_LPBK_NONE;
  }
}

bf_pm_port_autoneg_policy_e switch_port_an_to_pal_an(
    switch_port_attr_autoneg an) {
  switch (an) {
    case SWITCH_PORT_ATTR_AUTONEG_DEFAULT:
      return PM_AN_DEFAULT;
    case SWITCH_PORT_ATTR_AUTONEG_ENABLED:
      return PM_AN_FORCE_ENABLE;
    case SWITCH_PORT_ATTR_AUTONEG_DISABLED:
      return PM_AN_FORCE_DISABLE;
    case SWITCH_PORT_ATTR_AUTONEG_MAX:
      return PM_AN_MAX;
    default:
      return PM_AN_DEFAULT;
  }
}

bf_media_type_e switch_media_type_to_pal_mp(switch_port_attr_media_type mp) {
  switch (mp) {
    case SWITCH_PORT_ATTR_MEDIA_TYPE_UNKNOWN:
    case SWITCH_PORT_ATTR_MEDIA_TYPE_NOT_PRESENT:
      return BF_MEDIA_TYPE_UNKNOWN;
    case SWITCH_PORT_ATTR_MEDIA_TYPE_FIBER:
      return BF_MEDIA_TYPE_OPTICAL;
    case SWITCH_PORT_ATTR_MEDIA_TYPE_COPPER:
      return BF_MEDIA_TYPE_COPPER;
    default:
      return BF_MEDIA_TYPE_UNKNOWN;
  }
}

bf_status_t bf_pal_port_autoneg_info_get(bf_dev_id_t dev_id,
                                         bf_dev_port_t dev_port,
                                         bf_port_speed_t *hcd,
                                         int *hcd_lanes,
                                         bf_fec_type_t *fec) {
  bf_status_t bf_status = BF_SUCCESS;
  bf_dev_family_t dev_family =
      bf_dev_type_to_family(lld_sku_get_dev_type(dev_id));
  bf_status =
      bf_pal_port_autoneg_hcd_fec_get(dev_id, dev_port, hcd, hcd_lanes, fec);
  if (bf_status != BF_SUCCESS) return bf_status;
  if (*hcd == BF_SPEED_NONE || *hcd == BF_SPEED_1G) {
    return BF_EAGAIN;
  }
  if (dev_family == BF_DEV_FAMILY_TOFINO) {
    if (*hcd == BF_SPEED_100G) {
      *fec = BF_FEC_TYP_REED_SOLOMON;
    }
  } else {
    if (*hcd == BF_SPEED_50G) {
      *fec = BF_FEC_TYP_REED_SOLOMON;
    } else if (*hcd == BF_SPEED_100G) {
      // For AN 100G, force FEC RS for both NRZ and PAM4
      *fec = BF_FEC_TYP_REED_SOLOMON;
    } else if (*hcd == BF_SPEED_200G) {
      // For AN 200G, force FEC RS for both NRZ and PAM4
      *fec = BF_FEC_TYP_REED_SOLOMON;
    } else if (*hcd == BF_SPEED_400G) {
      *fec = BF_FEC_TYP_REED_SOLOMON;
    }
    return bf_status;
  }
  return BF_SUCCESS;
}

static switch_status_t update_supported_fec_types(
    bf_dev_id_t bf_dev_id,
    bf_dev_port_t bf_dev_port,
    switch_object_id_t port_handle,
    uint32_t port_speed,
    uint32_t num_lanes) {
  static const bf_fec_type_t fec_arr[]{
      BF_FEC_TYP_NONE, BF_FEC_TYP_FC, BF_FEC_TYP_RS};
  std::vector<switch_enum_t> supported_fec_types;
  bf_port_speed_t speed =
      switch_port_speed_to_pal_port_speed(bf_dev_id, port_speed, num_lanes);

  for (const auto fec : fec_arr) {
    bool valid = false;
    bf_status_t bf_status =
        bf_port_fec_type_validate(bf_dev_id, bf_dev_port, speed, fec, &valid);

    if (bf_status == BF_SUCCESS && valid) {
      supported_fec_types.push_back(
          switch_enum_t{.enumdata = pal_fec_to_switch_port_fec(fec)});
    }
  }

  attr_w supported_fec(SWITCH_PORT_ATTR_SUPPORTED_FEC_TYPES,
                       supported_fec_types);

  return switch_store::attribute_set(port_handle, supported_fec);
}

static bf_status_t recirc_port_to_dev_port_map(bf_dev_id_t dev_id,
                                               uint32_t recirc_port,
                                               bf_dev_port_t *dev_port) {
  if (!bf_lld_dev_is_tof3(dev_id)) {
    return bf_pal_recirc_port_to_dev_port_map(dev_id, recirc_port, dev_port);
  }
  uint32_t start_recirc_port, end_recirc_port;
  uint32_t pipe, port;
  uint8_t port_group = lld_get_chnls_per_mac(dev_id) * 2;

  bf_status_t status = bf_pal_recirc_port_range_get(
      dev_id, &start_recirc_port, &end_recirc_port);
  if (status != BF_SUCCESS) {
    return status;
  }

  pipe = (recirc_port - start_recirc_port) / port_group;
  port = (recirc_port - start_recirc_port) % port_group;
  *dev_port = MAKE_DEV_PORT(pipe, port);

  return BF_SUCCESS;
}

switch_status_t port_add(bf_dev_id_t bf_dev_id,
                         bf_dev_port_t bf_dev_port,
                         uint32_t port_speed,
                         switch_port_attr_fec_type fec,
                         uint32_t num_lanes) {
  bf_status_t bf_status = BF_SUCCESS;
  bf_fec_type_t bf_fec = BF_FEC_TYP_NONE;

  bf_port_speed_t bf_port_speed = BF_SPEED_NONE;
  bf_port_speed =
      switch_port_speed_to_pal_port_speed(bf_dev_id, port_speed, num_lanes);

  if (bf_dev_port == p4_devport_mgr_eth_cpu_port_get(bf_dev_id)) {
    if (fec != SWITCH_PORT_ATTR_FEC_TYPE_NONE) {
      switch_log(
          SWITCH_API_LEVEL_WARN,
          SWITCH_OBJECT_TYPE_PORT,
          "{}.{}: Eth CPU dev_port {} does not support FEC. Disabling FEC..",
          __func__,
          __LINE__,
          bf_dev_port);
      fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE;
    }
  }
  bf_fec = switch_port_fec_to_pal_fec(fec);

  bf_status = bf_pal_port_add_with_lanes(
      bf_dev_id, bf_dev_port, bf_port_speed, num_lanes, bf_fec);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: bf_pal_port_add_with_lanes failure device {} dev_port "
               "{} num_lanes {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               num_lanes,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  return pal_status_xlate(bf_status);
}

switch_status_t port_del(bf_dev_id_t bf_dev_id, bf_dev_port_t bf_dev_port) {
  bf_status_t bf_status = BF_SUCCESS;

  bf_status = bf_pal_port_del(bf_dev_id, bf_dev_port);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port_del failure device {} dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
  }
  return pal_status_xlate(bf_status);
}

switch_status_t port_enabled(uint64_t port_id,
                             bf_dev_id_t bf_dev_id,
                             bf_dev_port_t bf_dev_port,
                             bool enable) {
  bf_status_t bf_status = BF_SUCCESS;

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT) return SWITCH_STATUS_SUCCESS;

  if (enable) {
    bf_status = bf_pal_port_enable(bf_dev_id, bf_dev_port);
  } else {
    bf_status = bf_pal_port_disable(bf_dev_id, bf_dev_port);
  }
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port enable failure device {} dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
  }
  return pal_status_xlate(bf_status);
}

switch_status_t autoneg_set(switch_object_id_t port_handle,
                            uint64_t port_id,
                            bf_dev_id_t bf_dev_id,
                            bf_dev_port_t bf_dev_port,
                            switch_port_attr_autoneg autoneg) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return SWITCH_STATUS_SUCCESS;

  bf_pm_port_autoneg_policy_e an = switch_port_an_to_pal_an(autoneg);
  bf_pm_port_autoneg_policy_e bf_autoneg;
  bf_status =
      bf_pal_port_autoneg_policy_get(bf_dev_id, bf_dev_port, &bf_autoneg);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: falined to get autoneg policy from device {} dev_port "
               "{} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  if (an == bf_autoneg) {
    switch_log(SWITCH_API_LEVEL_DEBUG,
               SWITCH_OBJECT_TYPE_PORT,
               "{}:{} bf_an: {}, current_an: {}",
               bf_dev_id,
               bf_dev_port,
               bf_autoneg_to_str(bf_autoneg),
               bf_autoneg_to_str(an));
    return SWITCH_STATUS_SUCCESS;
  }
  if (bf_autoneg == PM_AN_DEFAULT ||
      autoneg == SWITCH_PORT_ATTR_AUTONEG_ENABLED) {
    bool admin_state = false;

    status = switch_store::v_get(
        port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);

    bf_status = bf_pal_port_disable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port disable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }

    status = adv_speed_update(port_handle, port_id, bf_dev_id, bf_dev_port);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port advertised speed update failure on device {} "
                 "dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 status);
      return status;
    }
    status = adv_fec_type_update(port_handle, port_id, bf_dev_id, bf_dev_port);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port advertised fec type update failure on device {} "
                 "dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 status);
      return status;
    }

    bf_status = bf_pal_port_autoneg_policy_set(bf_dev_id, bf_dev_port, an);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port autoneg policy set failure device {} dev_port {} "
                 "status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }

    if (admin_state) {
      bf_status = bf_pal_port_enable(bf_dev_id, bf_dev_port);
      if (bf_status != BF_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: port enable failure device {} dev_port {} status {}",
                   __func__,
                   __LINE__,
                   bf_dev_id,
                   bf_dev_port,
                   pal_status_xlate(bf_status));
        return pal_status_xlate(bf_status);
      }
    }
  } else {
    uint32_t speed;

    status = switch_store::v_get(port_handle, SWITCH_PORT_ATTR_SPEED, speed);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to get speed for port handle {} status {}",
                 __func__,
                 __LINE__,
                 port_handle,
                 status);
      return status;
    }

    status = speed_set(port_handle, port_id, bf_dev_id, bf_dev_port, speed);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to set port speed on AN disable for port "
                 "handle {} status {}",
                 __func__,
                 __LINE__,
                 port_handle,
                 status);
      return status;
    }
  }
  return status;
}

switch_status_t adv_speed_update(const switch_object_id_t port_handle,
                                 const uint64_t port_id,
                                 const bf_dev_id_t bf_dev_id,
                                 const bf_dev_port_t bf_dev_port) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  std::vector<bf_port_speed_t> adv_speed_arr;
  std::vector<uint32_t> advertised_speeds;

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return SWITCH_STATUS_SUCCESS;

  status = switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADVERTISED_SPEED, advertised_speeds);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  if (advertised_speeds.size() == 0) {
    // Advertise all supported speeds
    status = switch_store::v_get(
        port_handle, SWITCH_PORT_ATTR_SUPPORTED_SPEEDS, advertised_speeds);
    if (status != SWITCH_STATUS_SUCCESS) return status;
  }

  adv_speed_arr = std::vector<bf_port_speed_t>(advertised_speeds.size());
  for (uint32_t i = 0; i < advertised_speeds.size(); ++i) {
    adv_speed_arr[i] = port_speed_to_pal_port_speed(
        bf_dev_id, port_handle, advertised_speeds[i]);
  }

  bf_status = bf_pal_port_autoneg_adv_speed_set(
      bf_dev_id, bf_dev_port, adv_speed_arr.data(), adv_speed_arr.size());
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port advertised speed set failure device {} dev_port {} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }
  return status;
}

switch_status_t adv_fec_type_update(const switch_object_id_t port_handle,
                                    const uint64_t port_id,
                                    const bf_dev_id_t bf_dev_id,
                                    const bf_dev_port_t bf_dev_port) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  std::vector<bf_fec_type_t> adv_fec_type_arr;
  std::vector<switch_enum_t> advertised_fec_types;

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return SWITCH_STATUS_SUCCESS;

  status = switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADVERTISED_FEC_TYPE, advertised_fec_types);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  if (advertised_fec_types.size() == 0) {
    // Advertise all fec types
    adv_fec_type_arr = {
        BF_FEC_TYP_FC, BF_FEC_TYP_RS, BF_FEC_TYP_RS_IN, BF_FEC_TYP_RS_KL};
  } else {
    adv_fec_type_arr = std::vector<bf_fec_type_t>(advertised_fec_types.size());
    for (uint32_t i = 0; i < advertised_fec_types.size(); ++i) {
      adv_fec_type_arr[i] =
          switch_port_fec_to_pal_fec(static_cast<switch_port_attr_fec_type>(
              advertised_fec_types[i].enumdata));
    }
  }

  bf_status = bf_pal_port_autoneg_adv_fec_set(
      bf_dev_id, bf_dev_port, adv_fec_type_arr.data(), adv_fec_type_arr.size());
  if (bf_status != BF_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: port advertised fec type set failure device {} dev_port {} "
        "status {}",
        __func__,
        __LINE__,
        bf_dev_id,
        bf_dev_port,
        pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }
  return status;
}

switch_status_t adv_speed_set(const switch_object_id_t port_handle,
                              const uint64_t port_id,
                              const bf_dev_id_t bf_dev_id,
                              const bf_dev_port_t bf_dev_port,
                              const uint32_t *adv_speed,
                              const uint32_t adv_speed_cnt) {
  bf_status_t bf_status = BF_SUCCESS;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool admin_state = false;
  std::vector<bf_port_speed_t> adv_speed_arr(adv_speed_cnt);

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return SWITCH_STATUS_SUCCESS;

  if (adv_speed == nullptr || adv_speed_cnt == 0) {
    std::vector<uint32_t> supported_speeds;
    status = switch_store::v_get(
        port_handle, SWITCH_PORT_ATTR_SUPPORTED_SPEEDS, supported_speeds);
    if (status != SWITCH_STATUS_SUCCESS) return status;

    adv_speed_arr = std::vector<bf_port_speed_t>(supported_speeds.size());
    for (uint32_t i = 0; i < supported_speeds.size(); ++i) {
      adv_speed_arr[i] = port_speed_to_pal_port_speed(
          bf_dev_id, port_handle, supported_speeds[i]);
    }
  } else {
    for (uint32_t i = 0; i < adv_speed_cnt; ++i) {
      adv_speed_arr[i] =
          port_speed_to_pal_port_speed(bf_dev_id, port_handle, adv_speed[i]);
    }
  }
  status = switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  if (admin_state) {
    bf_status = bf_pal_port_disable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port disable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }
  bf_status = bf_pal_port_autoneg_adv_speed_set(
      bf_dev_id, bf_dev_port, adv_speed_arr.data(), adv_speed_arr.size());
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port advertised speed set failure device {} dev_port {} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }
  if (admin_state) {
    bf_status = bf_pal_port_enable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port enable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }
  return pal_status_xlate(bf_status);
}

switch_status_t adv_fec_type_set(const switch_object_id_t port_handle,
                                 const uint64_t port_id,
                                 const bf_dev_id_t bf_dev_id,
                                 const bf_dev_port_t bf_dev_port,
                                 const switch_enum_t *adv_fec_type,
                                 const uint32_t adv_fec_type_cnt) {
  bf_status_t bf_status = BF_SUCCESS;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool admin_state = false;
  std::vector<bf_fec_type_t> adv_fec_type_arr(adv_fec_type_cnt);

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return SWITCH_STATUS_SUCCESS;

  if (adv_fec_type == nullptr || adv_fec_type_cnt == 0) {
    adv_fec_type_arr = {
        BF_FEC_TYP_FC, BF_FEC_TYP_RS, BF_FEC_TYP_RS_IN, BF_FEC_TYP_RS_KL};
  } else {
    for (uint32_t i = 0; i < adv_fec_type_cnt; ++i) {
      adv_fec_type_arr[i] = switch_port_fec_to_pal_fec(
          static_cast<switch_port_attr_fec_type>(adv_fec_type[i].enumdata));
    }
  }
  status = switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  if (admin_state) {
    bf_status = bf_pal_port_disable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port disable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }
  bf_status = bf_pal_port_autoneg_adv_fec_set(
      bf_dev_id, bf_dev_port, adv_fec_type_arr.data(), adv_fec_type_arr.size());
  if (bf_status != BF_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: port advertised fec type set failure device {} dev_port {} "
        "status {}",
        __func__,
        __LINE__,
        bf_dev_id,
        bf_dev_port,
        pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }
  if (admin_state) {
    bf_status = bf_pal_port_enable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port enable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }
  return pal_status_xlate(bf_status);
}

switch_status_t link_pause_set(switch_object_id_t port_handle,
                               uint64_t port_id,
                               bf_dev_id_t bf_dev_id,
                               bf_dev_port_t bf_dev_port,
                               switch_port_attr_link_pause link_pause) {
  bf_status_t bf_status = BF_SUCCESS;
  bool rx_en = false, tx_en = false, admin_state = false;
  switch (link_pause) {
    case SWITCH_PORT_ATTR_LINK_PAUSE_DISABLE:
      break;
    case SWITCH_PORT_ATTR_LINK_PAUSE_TX_ONLY:
      tx_en = true;
      break;
    case SWITCH_PORT_ATTR_LINK_PAUSE_RX_ONLY:
      rx_en = true;
      break;
    case SWITCH_PORT_ATTR_LINK_PAUSE_BOTH_ENABLE:
      rx_en = true;
      tx_en = true;
      break;
    default:
      break;
  }

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return SWITCH_STATUS_SUCCESS;

  bf_status = switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);

  bf_status = bf_pal_port_disable(bf_dev_id, bf_dev_port);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port disable failure device {} dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  bf_status = bf_pal_port_flow_control_link_pause_set(
      bf_dev_id, bf_dev_port, tx_en, rx_en);
  if (bf_status != BF_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: port link pause set failure device {} dev_port {} rx/tx: {}/{}"
        "status {}",
        __func__,
        __LINE__,
        bf_dev_id,
        bf_dev_port,
        rx_en,
        tx_en,
        pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  if (admin_state) {
    bf_status = bf_pal_port_enable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port enable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }
  return pal_status_xlate(bf_status);
}

switch_status_t media_type_set(switch_object_id_t port_handle,
                               uint64_t port_id,
                               bf_dev_id_t bf_dev_id,
                               bf_dev_port_t bf_dev_port,
                               switch_port_attr_media_type media_type) {
  bf_status_t bf_status = BF_SUCCESS;
  bool admin_state = false;
  bf_media_type_e mp = switch_media_type_to_pal_mp(media_type);

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return SWITCH_STATUS_SUCCESS;

  bf_status = switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);

  bf_status = bf_pal_port_disable(bf_dev_id, bf_dev_port);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port disable failure device {} dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  bf_status = bf_pal_port_media_type_set(bf_dev_id, bf_dev_port, mp);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port media type set failure device {} dev_port {} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  if (admin_state) {
    bf_status = bf_pal_port_enable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port enable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }
  return pal_status_xlate(bf_status);
}

switch_status_t mtu_set(switch_object_id_t port_handle,
                        uint64_t port_id,
                        bf_dev_id_t bf_dev_id,
                        bf_dev_port_t bf_dev_port,
                        uint32_t rx_mtu,
                        uint32_t tx_mtu) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  bool admin_state = false;
  switch_object_id_t device_handle{};
  uint16_t max_mtu = 0;

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return status;

  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEVICE, device_handle);
  status |= switch_store::v_get(
      device_handle, SWITCH_DEVICE_ATTR_MAX_PORT_MTU, max_mtu);

  if (status != SWITCH_STATUS_SUCCESS) return status;
  if (rx_mtu > max_mtu || tx_mtu > max_mtu) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port mtu greater then max_mtu {}, rx_mtu {} "
               "tx_mtu {} device {} dev_port {} status {}",
               __func__,
               __LINE__,
               max_mtu,
               rx_mtu,
               tx_mtu,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  bf_status = bf_pal_port_disable(bf_dev_id, bf_dev_port);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port disable failure device {} dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  bf_status = bf_pal_port_mtu_set(bf_dev_id, bf_dev_port, tx_mtu, rx_mtu);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port mtu set failure device {} dev_port {} tx_mtu {} "
               "rx_mtu {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               tx_mtu,
               rx_mtu,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  if (admin_state) {
    bf_status = bf_pal_port_enable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port enable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }
  return pal_status_xlate(bf_status);
}

switch_status_t port_pfc_set(bf_dev_id_t bf_dev_id,
                             bf_dev_port_t dev_port,
                             uint32_t tx_pfc_map,
                             uint32_t rx_pfc_map) {
  bf_status_t bf_status = BF_SUCCESS;

  // Tofino2: PFC RX config in MAC is single bit -no per priority enable/disable
  if (bf_lld_dev_is_tof2(bf_dev_id)) {
    rx_pfc_map = rx_pfc_map ? 0xff : 0x0;
  }
  bf_status = bf_pal_port_flow_control_pfc_set(
      bf_dev_id, dev_port, tx_pfc_map, rx_pfc_map);

  CHECK_RET(bf_status != BF_SUCCESS, pal_status_xlate(bf_status));

  return SWITCH_STATUS_SUCCESS;
}

switch_status_t loopback_mode_set(bf_dev_id_t bf_dev_id,
                                  bf_dev_port_t bf_dev_port,
                                  switch_port_attr_loopback_mode lb_mode) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  bf_loopback_mode_e bf_lb_mode = switch_lb_mode_to_pd_lb_mode(lb_mode);
  (void)bf_dev_id;
  (void)bf_dev_port;
  (void)bf_lb_mode;

  bf_status = bf_pal_port_loopback_mode_set(bf_dev_id, bf_dev_port, bf_lb_mode);
  if (bf_status != BF_SUCCESS) {
    status = pal_status_xlate(bf_status);
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: loopback_set failure dev_port {} lb_mode {} status {}",
               __func__,
               __LINE__,
               bf_dev_port,
               lb_mode,
               status);
  }
  CHECK_RET(bf_status != BF_SUCCESS, pal_status_xlate(bf_status));

  return status;
}

switch_status_t fec_type_set(switch_object_id_t port_handle,
                             uint64_t port_id,
                             bf_dev_id_t bf_dev_id,
                             bf_dev_port_t bf_dev_port,
                             switch_port_attr_fec_type fec) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  bf_fec_type_t bf_fec = BF_FEC_TYP_NONE;
  switch_object_id_t device_hdl = {0};
  bool cut_thru = false;

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return status;

  bf_fec = switch_port_fec_to_pal_fec(fec);
  bf_status = bf_pal_port_fec_set(bf_dev_id, bf_dev_port, bf_fec);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: fec set failure device {} dev_port {} fec_type {} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               fec,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEVICE, device_hdl);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status |=
      switch_store::v_get(device_hdl, SWITCH_DEVICE_ATTR_CUT_THROUGH, cut_thru);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  if (cut_thru) {
    bf_status = bf_pal_port_cut_through_enable(bf_dev_id, bf_dev_port);
  } else {
    bf_status = bf_pal_port_cut_through_disable(bf_dev_id, bf_dev_port);
  }
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port cut thru set failure device {} dev_port {} value "
               "{} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               cut_thru,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  switch_enum_t _autoneg = {0};
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_AUTONEG, _autoneg);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  switch_port_attr_autoneg autoneg;
  autoneg = static_cast<switch_port_attr_autoneg>(_autoneg.enumdata);
  switch_status_t temp_status = SWITCH_STATUS_SUCCESS;
  temp_status =
      autoneg_set(port_handle, port_id, bf_dev_id, bf_dev_port, autoneg);
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port autoneg set failure device {} dev_port {} value {} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               autoneg,
               temp_status);
    return temp_status;
  }

  switch_enum_t _link_pause = {0};
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_LINK_PAUSE, _link_pause);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  switch_port_attr_link_pause link_pause;
  link_pause = static_cast<switch_port_attr_link_pause>(_link_pause.enumdata);
  temp_status =
      link_pause_set(port_handle, port_id, bf_dev_id, bf_dev_port, link_pause);
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: port link_pause set failure device {} dev_port {} value {} "
        "status {}",
        __func__,
        __LINE__,
        bf_dev_id,
        bf_dev_port,
        link_pause,
        temp_status);
    return temp_status;
  }

  switch_enum_t _media_type = {0};
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_MEDIA_TYPE, _media_type);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  switch_port_attr_media_type media_type;
  media_type = static_cast<switch_port_attr_media_type>(_media_type.enumdata);
  temp_status =
      media_type_set(port_handle, port_id, bf_dev_id, bf_dev_port, media_type);
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: port media_type set failure device {} dev_port {} value {} "
        "status {}",
        __func__,
        __LINE__,
        bf_dev_id,
        bf_dev_port,
        media_type,
        temp_status);
    return temp_status;
  }

  uint32_t _rx_mtu = 0;
  uint32_t _tx_mtu = 0;
  status |= switch_store::v_get(port_handle, SWITCH_PORT_ATTR_RX_MTU, _rx_mtu);
  status |= switch_store::v_get(port_handle, SWITCH_PORT_ATTR_TX_MTU, _tx_mtu);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  temp_status =
      mtu_set(port_handle, port_id, bf_dev_id, bf_dev_port, _rx_mtu, _tx_mtu);
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: port mtu set failure device {} dev_port {} rx_mtu {} tx_mtu {} "
        "status {}",
        __func__,
        __LINE__,
        bf_dev_id,
        bf_dev_port,
        _rx_mtu,
        _tx_mtu,
        temp_status);
    return temp_status;
  }

  uint32_t _rx_pfc_map = 0;
  uint32_t _tx_pfc_map = 0;
  switch_enum_t _pfc_mode = {SWITCH_PORT_ATTR_PFC_MODE_COMBINED};

  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_PFC_MODE, _pfc_mode);

  if (_pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_SEPARATE) {
    status |= switch_store::v_get(
        port_handle, SWITCH_PORT_ATTR_RX_PFC_MAP, _rx_pfc_map);
    status |= switch_store::v_get(
        port_handle, SWITCH_PORT_ATTR_TX_PFC_MAP, _tx_pfc_map);
  } else {
    status |=
        switch_store::v_get(port_handle, SWITCH_PORT_ATTR_PFC_MAP, _rx_pfc_map);
    _tx_pfc_map = _rx_pfc_map;
  }

  if (status != SWITCH_STATUS_SUCCESS) return status;
  temp_status = port_pfc_set(bf_dev_id, bf_dev_port, _tx_pfc_map, _rx_pfc_map);
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port pfc map set failure device {} dev_port {} "
               "tx_pfc_map {} rx_pfc_map {} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               _tx_pfc_map,
               _rx_pfc_map,
               temp_status);
    return temp_status;
  }

  switch_enum_t lb_mode{SWITCH_PORT_ATTR_LOOPBACK_MODE_NONE};
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_LOOPBACK_MODE, lb_mode);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  temp_status = loopback_mode_set(
      bf_dev_id,
      bf_dev_port,
      static_cast<switch_port_attr_loopback_mode>(lb_mode.enumdata));
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port loopback set failure device {} dev_port {} "
               "loopback mode{} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               lb_mode.enumdata,
               temp_status);
    return temp_status;
  }
  return temp_status;
}

switch_status_t port_flowcontrol_mode_set(
    bf_dev_port_t dev_port, switch_port_attr_flow_control flow_control) {
  switch_status_t status = 0;
  std::string flowcontrol_mode = "";
  std::map<switch_port_attr_flow_control, std::string> flowcontrol_mode_map{
      {SWITCH_PORT_ATTR_FLOW_CONTROL_NONE, "NONE"},
      {SWITCH_PORT_ATTR_FLOW_CONTROL_PFC, "PFC"},
      {SWITCH_PORT_ATTR_FLOW_CONTROL_PAUSE, "PAUSE"}};

  if (flowcontrol_mode_map.find(flow_control) != flowcontrol_mode_map.end()) {
    flowcontrol_mode = flowcontrol_mode_map[flow_control];
  } else {
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  status = bfrt_tm_port_flowcontrol_mode_tx_set(dev_port, flowcontrol_mode);
  return status;
}
switch_status_t supported_speeds_set(switch_object_id_t port_handle,
                                     bf_dev_id_t bf_dev_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  std::vector<uint32_t> supported_speeds;
  attr_w port_lane_list(SWITCH_PORT_ATTR_LANE_LIST);
  std::vector<uint32_t> lane_list;

  status =
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_LANE_LIST, lane_list);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get lane_list for port handle {} status {}",
               __func__,
               __LINE__,
               port_handle,
               status);
    return status;
  }

  bf_dev_family_t dev_family =
      bf_dev_type_to_family(lld_sku_get_dev_type(bf_dev_id));
  switch (lane_list.size()) {
    case 8:
      switch (dev_family) {
        case BF_DEV_FAMILY_TOFINO:
          break;
        case BF_DEV_FAMILY_TOFINO2:
          supported_speeds.push_back(static_cast<uint32_t>(200000));  // 8*25
          supported_speeds.push_back(static_cast<uint32_t>(400000));  // 8*50
          break;
        case BF_DEV_FAMILY_TOFINO3:
          supported_speeds.push_back(static_cast<uint32_t>(200000));  // 8*25
          supported_speeds.push_back(static_cast<uint32_t>(400000));  // 8*50
          break;
        default:
          break;
      }
      break;
    case 4:
      switch (dev_family) {
        case BF_DEV_FAMILY_TOFINO:
          supported_speeds.push_back(static_cast<uint32_t>(40000));   // 4*10
          supported_speeds.push_back(static_cast<uint32_t>(100000));  // 4*25
          break;
        case BF_DEV_FAMILY_TOFINO2:
          supported_speeds.push_back(static_cast<uint32_t>(40000));   // 4*10
          supported_speeds.push_back(static_cast<uint32_t>(100000));  // 4*25
          supported_speeds.push_back(static_cast<uint32_t>(200000));  // 4*50
          break;
        case BF_DEV_FAMILY_TOFINO3:
          supported_speeds.push_back(static_cast<uint32_t>(40000));   // 4*10
          supported_speeds.push_back(static_cast<uint32_t>(100000));  // 4*25
          supported_speeds.push_back(static_cast<uint32_t>(200000));  // 4*50
          supported_speeds.push_back(static_cast<uint32_t>(400000));  // 4*100
          break;
        default:
          break;
      }
      break;
    case 2:
      switch (dev_family) {
        case BF_DEV_FAMILY_TOFINO:
          supported_speeds.push_back(static_cast<uint32_t>(50000));  // 2*25
          break;
        case BF_DEV_FAMILY_TOFINO2:
          supported_speeds.push_back(static_cast<uint32_t>(40000));   // 2*20
          supported_speeds.push_back(static_cast<uint32_t>(50000));   // 2*25
          supported_speeds.push_back(static_cast<uint32_t>(100000));  // 2*50
          break;
        case BF_DEV_FAMILY_TOFINO3:
          supported_speeds.push_back(static_cast<uint32_t>(50000));   // 2*25
          supported_speeds.push_back(static_cast<uint32_t>(100000));  // 2*50
          supported_speeds.push_back(static_cast<uint32_t>(200000));  // 2*100
          break;
        default:
          break;
      }
      break;
    case 1:
      switch (dev_family) {
        case BF_DEV_FAMILY_TOFINO:
          supported_speeds.push_back(static_cast<uint32_t>(10000));  // 1*10
          supported_speeds.push_back(static_cast<uint32_t>(25000));  // 1*25
          break;
        case BF_DEV_FAMILY_TOFINO2:
          supported_speeds.push_back(static_cast<uint32_t>(10000));  // 1*10
          supported_speeds.push_back(static_cast<uint32_t>(25000));  // 1*25
          supported_speeds.push_back(static_cast<uint32_t>(50000));  // 1*50
          break;
        case BF_DEV_FAMILY_TOFINO3:
          supported_speeds.push_back(static_cast<uint32_t>(10000));   // 1*10
          supported_speeds.push_back(static_cast<uint32_t>(25000));   // 1*25
          supported_speeds.push_back(static_cast<uint32_t>(50000));   // 1*50
          supported_speeds.push_back(static_cast<uint32_t>(100000));  // 1*100
          break;
        default:
          break;
      }
      break;
    default:
      status = SWITCH_STATUS_INVALID_PARAMETER;
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: ERROR: Invalid lane number {} for dev_id {} status {}",
                 __func__,
                 __LINE__,
                 lane_list.size(),
                 bf_dev_id,
                 status);
      return status;
      break;
  }
  attr_w port_supported_speeds(SWITCH_PORT_ATTR_SUPPORTED_SPEEDS);
  port_supported_speeds.v_set(supported_speeds);
  status = switch_store::attribute_set(port_handle, port_supported_speeds);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: supported_speeds set failure port_handle {} status {}",
               __func__,
               __LINE__,
               port_handle,
               status);
    return status;
  }
  return status;
}

switch_enum_t get_default_fec_by_speed(switch_enum_t _fec,
                                       const uint32_t speed,
                                       uint32_t lanes) {
  bool warn = false;
  if ((speed == 100000) && (_fec.enumdata == SWITCH_PORT_ATTR_FEC_TYPE_FC) &&
      (lanes == 2)) {
    _fec.enumdata = SWITCH_PORT_ATTR_FEC_TYPE_RS;
    warn = true;
  }
  if ((speed == 40000 || speed == 10000) &&
      (_fec.enumdata == SWITCH_PORT_ATTR_FEC_TYPE_RS)) {
    _fec.enumdata = SWITCH_PORT_ATTR_FEC_TYPE_NONE;
    warn = true;
  }
  // Anything with 56G PAM4 serdes requires RS FEC
  if (((speed == 50000 * lanes) || (speed == 100000 * lanes)) &&
      (_fec.enumdata != SWITCH_PORT_ATTR_FEC_TYPE_RS)) {
    _fec.enumdata = SWITCH_PORT_ATTR_FEC_TYPE_RS;
    warn = true;
  }
  if (warn) {
    switch_log(SWITCH_API_LEVEL_WARN,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: speed {} (PAM4) and fec {} combination is incompatible."
               " Setting FEC to RS.",
               __func__,
               __LINE__,
               switch_port_speed_to_str(speed),
               switch_port_fec_to_str(
                   static_cast<switch_port_attr_fec_type>(_fec.enumdata)));
  }

  return _fec;
}

switch_status_t speed_set(switch_object_id_t port_handle,
                          uint64_t port_id,
                          bf_dev_id_t bf_dev_id,
                          bf_dev_port_t bf_dev_port,
                          uint32_t speed) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  switch_object_id_t device_hdl = {0};
  bool admin_state = false;
  bool fec_valid = false;
  std::vector<uint32_t> lane_list;
  attr_w port_lane_list(SWITCH_PORT_ATTR_LANE_LIST);

  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT)
    return status;

  switch_enum_t _an = {0};
  status = switch_store::v_get(port_handle, SWITCH_PORT_ATTR_AUTONEG, _an);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  switch_port_attr_autoneg autoneg;
  autoneg = static_cast<switch_port_attr_autoneg>(_an.enumdata);

  switch_port_attr_fec_type fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE;
  if (autoneg == SWITCH_PORT_ATTR_AUTONEG_ENABLED) {
    bf_port_speed_t hcd_speed;
    bf_fec_type_t hcd_fec;
    int hcd_lanes = 0;
    bf_status = bf_pal_port_autoneg_info_get(
        bf_dev_id, bf_dev_port, &hcd_speed, &hcd_lanes, &hcd_fec);
    if (bf_status == BF_SUCCESS) {
      fec = pal_fec_to_switch_port_fec(hcd_fec);
      fec_valid = true;
    }
  }

  status =
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_LANE_LIST, lane_list);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get lane_list for port_handle {} status {}",
               __func__,
               __LINE__,
               port_handle,
               status);
    return status;
  }

  if (!fec_valid) {
    switch_enum_t _fec = {0};
    status = switch_store::v_get(port_handle, SWITCH_PORT_ATTR_FEC_TYPE, _fec);
    if (status != SWITCH_STATUS_SUCCESS) return status;
    _fec = get_default_fec_by_speed(_fec, speed, lane_list.size());
    fec = static_cast<switch_port_attr_fec_type>(_fec.enumdata);
  }

  status =
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEVICE, device_hdl);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status = switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);

  if (admin_state) {
    bf_status = bf_pal_port_disable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port disable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }

  bf_status = bf_pal_port_del(bf_dev_id, bf_dev_port);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port delete failure device {} dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  status = port_add(bf_dev_id, bf_dev_port, speed, fec, lane_list.size());
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port_add failure device {} dev_port {} speed {} fec {}"
               "lanes {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               switch_port_speed_to_str(speed),
               switch_port_fec_to_str(fec),
               lane_list.size(),
               status);
    return status;
  }

  status = update_supported_fec_types(
      bf_dev_id, bf_dev_port, port_handle, speed, lane_list.size());
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: update supported fec types "
               "failure for device {} dev_port "
               "{} object_id {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               port_handle,
               status);
  }

  uint32_t rx_mtu = 0, tx_mtu = 0;
  status = switch_store::v_get(port_handle, SWITCH_PORT_ATTR_RX_MTU, rx_mtu);
  status = switch_store::v_get(port_handle, SWITCH_PORT_ATTR_TX_MTU, tx_mtu);
  status =
      mtu_set(port_handle, port_id, bf_dev_id, bf_dev_port, rx_mtu, tx_mtu);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: mtu set failure device {} dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               status);
    return status;
  }

  status = adv_speed_update(port_handle, port_id, bf_dev_id, bf_dev_port);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port advertised speed update failure on device {} "
               "dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               status);
    return status;
  }
  status = adv_fec_type_update(port_handle, port_id, bf_dev_id, bf_dev_port);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port advertised fec type update failure on device {} "
               "dev_port {} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               status);
    return status;
  }

  bf_status = bf_pal_port_autoneg_policy_set(bf_dev_id, bf_dev_port, autoneg);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port autoneg policy set failure device {} dev_port {} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  uint32_t _rx_pfc_map = 0;
  uint32_t _tx_pfc_map = 0;
  switch_enum_t _pfc_mode = {SWITCH_PORT_ATTR_PFC_MODE_COMBINED};

  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_PFC_MODE, _pfc_mode);

  if (_pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_SEPARATE) {
    status |= switch_store::v_get(
        port_handle, SWITCH_PORT_ATTR_RX_PFC_MAP, _rx_pfc_map);
    status |= switch_store::v_get(
        port_handle, SWITCH_PORT_ATTR_TX_PFC_MAP, _tx_pfc_map);
  } else {
    status |=
        switch_store::v_get(port_handle, SWITCH_PORT_ATTR_PFC_MAP, _rx_pfc_map);
    _tx_pfc_map = _rx_pfc_map;
  }

  if (status != SWITCH_STATUS_SUCCESS) return status;
  status = port_pfc_set(bf_dev_id, bf_dev_port, _tx_pfc_map, _rx_pfc_map);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port pfc map set failure device {} dev_port {} "
               "tx_pfc_map {} rx_pfc_map {} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               _tx_pfc_map,
               _rx_pfc_map,
               status);
    return status;
  }

  bool cut_thru = false;
  status |=
      switch_store::v_get(device_hdl, SWITCH_DEVICE_ATTR_CUT_THROUGH, cut_thru);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get cut_thru mode for device {} port "
               "handle {} status {}",
               __func__,
               __LINE__,
               device_hdl,
               port_handle,
               status);
    return status;
  }

  if (cut_thru) {
    bf_status = bf_pal_port_cut_through_enable(bf_dev_id, bf_dev_port);
  } else {
    bf_status = bf_pal_port_cut_through_disable(bf_dev_id, bf_dev_port);
  }
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port cut thru set failure device {} dev_port {} value "
               "{} status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               cut_thru,
               pal_status_xlate(bf_status));
    return pal_status_xlate(bf_status);
  }

  if (admin_state) {
    bf_status = bf_pal_port_enable(bf_dev_id, bf_dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port enable failure device {} dev_port {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  }

  port_lane_list.v_set(lane_list);
  status = switch_store::attribute_set(port_handle, port_lane_list);
  if (status != SWITCH_STATUS_SUCCESS) {
    // switch_log(SWITCH_API_LEVEL_ERROR,
    //            SWITCH_OBJECT_TYPE_PORT,
    //            "{}.{}: lane_list failure port_handle {} speed {} status {}",
    //            __func__,
    //            __LINE__,
    //            port_handle,
    //            port_lane_list,
    //            status);
  }

  switch_enum_t _link_pause = {0};
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_LINK_PAUSE, _link_pause);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  switch_port_attr_link_pause link_pause;
  link_pause = static_cast<switch_port_attr_link_pause>(_link_pause.enumdata);
  status =
      link_pause_set(port_handle, port_id, bf_dev_id, bf_dev_port, link_pause);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: port link_pause set failure device {} dev_port {} value {} "
        "status {}",
        __func__,
        __LINE__,
        bf_dev_id,
        bf_dev_port,
        link_pause,
        status);
    return status;
  }

  switch_enum_t _media_type = {0};
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_MEDIA_TYPE, _media_type);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  switch_port_attr_media_type media_type;
  media_type = static_cast<switch_port_attr_media_type>(_media_type.enumdata);
  status =
      media_type_set(port_handle, port_id, bf_dev_id, bf_dev_port, media_type);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: port media_type set failure device {} dev_port {} value {} "
        "status {}",
        __func__,
        __LINE__,
        bf_dev_id,
        bf_dev_port,
        media_type,
        status);
    return status;
  }

  switch_enum_t lb_mode{SWITCH_PORT_ATTR_LOOPBACK_MODE_NONE};
  status |=
      switch_store::v_get(port_handle, SWITCH_PORT_ATTR_LOOPBACK_MODE, lb_mode);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
  status = loopback_mode_set(
      bf_dev_id,
      bf_dev_port,
      static_cast<switch_port_attr_loopback_mode>(lb_mode.enumdata));
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port loopback set failure device {} dev_port {} "
               "loopback mode{} "
               "status {}",
               __func__,
               __LINE__,
               bf_dev_id,
               bf_dev_port,
               lb_mode.enumdata,
               status);
    return status;
  }

  return status;
}

void update_port_an_state(const switch_object_id_t &port_handle,
                          const uint64_t port_id,
                          const bf_dev_id_t bf_dev_id,
                          const bf_dev_port_t bf_dev_port) {
  bool admin_state = false;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  bf_port_speed_t hcd_speed;
  int hcd_lanes = 0;
  bf_fec_type_t hcd_fec;
  bf_port_speed_t current_speed;
  bf_fec_type_t current_fec;

  // Skip ports in admin down state
  status = switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);
  if (status != SWITCH_STATUS_SUCCESS) return;
  if (!admin_state) return;

  // Retrieve HCD and FEC values
  bf_status = bf_pal_port_autoneg_info_get(
      bf_dev_id, bf_dev_port, &hcd_speed, &hcd_lanes, &hcd_fec);
  if (bf_status != BF_SUCCESS) return;

  // Update port speed in case it differs from HCD.
  // This will re-create port.
  bf_status = bf_pal_port_speed_get(bf_dev_id, bf_dev_port, &current_speed);
  if (bf_status != BF_SUCCESS) return;

  bf_status = bf_pal_port_fec_get(bf_dev_id, bf_dev_port, &current_fec);
  if (bf_status != BF_SUCCESS) return;

  if (current_speed != hcd_speed || current_fec != hcd_fec) {
    auto speed = pal_port_speed_to_switch_port_speed(hcd_speed);
    switch_log(SWITCH_API_LEVEL_INFO,
               SWITCH_OBJECT_TYPE_PORT,
               "{}: current speed {}, HCD speed {}, HCD lanes {}, HCD FEC {}",
               __func__,
               current_speed,
               hcd_speed,
               hcd_lanes,
               hcd_fec);
    speed_set(port_handle, port_id, bf_dev_id, bf_dev_port, speed);
  }
}

switch_status_t port_get_default_scheduler_group(
    switch_object_id_t port_handle, switch_object_id_t &sch_grp_handle) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  std::vector<switch_object_id_t> g_list;
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_PORT_QUEUE_SCHEDULER_GROUP_HANDLES, g_list);

  for (auto const g_oid : g_list) {
    switch_enum_t type;
    status |=
        switch_store::v_get(g_oid, SWITCH_SCHEDULER_GROUP_ATTR_TYPE, type);
    if (type.enumdata == SWITCH_SCHEDULER_GROUP_ATTR_TYPE_PORT) {
      sch_grp_handle = g_oid;
      return status;
    }
  }

  return SWITCH_STATUS_FAILURE;
}

switch_status_t scheduler_profile_set(switch_object_id_t port_handle,
                                      switch_object_id_t scheduler_handle) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t port_sch_group_handle = {0};

  status = port_get_default_scheduler_group(port_handle, port_sch_group_handle);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: failed to get schedler_group for port handle {} status {}",
        __func__,
        __LINE__,
        port_handle,
        status);
    return status;
  }

  /* set group's scheduler profile  */
  status = switch_store::v_set(port_sch_group_handle,
                               SWITCH_SCHEDULER_GROUP_ATTR_SCHEDULER_HANDLE,
                               scheduler_handle);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: v_set failure port_handle {} scheduler_group {} "
               "status {}",
               __func__,
               __LINE__,
               port_handle,
               port_sch_group_handle,
               status);
    return status;
  }

  return status;
}

switch_status_t get_dev_port(uint16_t device,
                             uint64_t port_num,
                             uint16_t *dev_port) {
  bf_status_t bf_status = BF_SUCCESS;
  bf_dev_port_t bf_dev_port = 0;
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  if (port_num == SWITCH_CPU_PORT_PCIE_DEFAULT) {
    *dev_port = p4_devport_mgr_pcie_cpu_port_get(device);
  } else if (port_num == SWITCH_CPU_PORT_ETH_DEFAULT) {
    *dev_port = p4_devport_mgr_eth_cpu_port_get(device);
    // Workaround for tofino2 eth CPU port. API returns BF_TOF2_ETH_CPU_PORT=2
    // We want to use 4 which is 33/2 when on hw and stick with 2 for model
    if (bf_lld_dev_is_tof2(device)) {
      bool sw_model = false;
      status = bf_pal_pltfm_type_get(device, &sw_model);
      if (!sw_model) *dev_port += 2;
    }
  } else {
    bf_status = bf_pal_fp_idx_to_dev_port_map(device, port_num, &bf_dev_port);
    *dev_port = static_cast<uint16_t>(bf_dev_port);
    status = pal_status_xlate(bf_status);
  }
  return status;
}

switch_status_t validate_port_attributes(bf_dev_id_t dev_id,
                                         uint16_t dev_port,
                                         std::set<attr_w> &attrs) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  uint32_t port_speed = 0;
  switch_port_attr_fec_type fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE;
  bf_port_speed_t bf_port_speed = BF_SPEED_NONE;
  bf_fec_type_t bf_fec = BF_FEC_TYP_NONE;
  switch_enum_t _fec = {0};
  std::vector<uint32_t> lane_list;
  uint32_t num_lanes = 0;

  const auto attr_lane_list =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_LANE_LIST));
  if (attr_lane_list == attrs.end()) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: lane_list is not found on port dev_port {}",
               __func__,
               __LINE__,
               dev_port);
    return SWITCH_STATUS_INVALID_PARAMETER;
  } else {
    status = (*attr_lane_list).v_get(lane_list);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to get speed for port dev_port {} status {}",
                 __func__,
                 __LINE__,
                 dev_port,
                 status);
      return status;
    }
  }
  num_lanes = lane_list.size();

  const auto attr_speed =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_SPEED));
  if (attr_speed == attrs.end()) {
    // If the speed was not set at port create, it will be 10G as default
    port_speed = 10000;
  } else {
    status = (*attr_speed).v_get(port_speed);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to get speed for port dev_port {} status {}",
                 __func__,
                 __LINE__,
                 dev_port,
                 status);
      return status;
    }
  }

  if (port_speed == 0) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: speed {} is not configured on port dev_port {}",
               __func__,
               __LINE__,
               switch_port_speed_to_str(port_speed),
               dev_port);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }

  bf_port_speed =
      switch_port_speed_to_pal_port_speed(dev_id, port_speed, num_lanes);

  const auto attr_fec =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_FEC_TYPE));
  if (attr_fec == attrs.end()) {
    _fec.enumdata = SWITCH_PORT_ATTR_FEC_TYPE_NONE;
  } else {
    status = (*attr_fec).v_get(_fec);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to get fec type for port dev_port {} "
                 "status {}",
                 __func__,
                 __LINE__,
                 dev_port,
                 status);
      return status;
    }
  }
  _fec = get_default_fec_by_speed(_fec, port_speed, num_lanes);
  fec = static_cast<switch_port_attr_fec_type>(_fec.enumdata);
  bf_fec = switch_port_fec_to_pal_fec(fec);

  // Validate port attributes
  if (!bf_pm_port_valid_speed_and_channel(
          dev_id, dev_port, bf_port_speed, num_lanes, bf_fec)) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: validate speed and channel failure for port dev_port {}"
               " status {} dev_id {} bf_port_speed {} num_lanes {} bf_fec {}",
               __func__,
               __LINE__,
               dev_port,
               status,
               dev_id,
               bf_port_speed,
               num_lanes,
               bf_fec);
    return SWITCH_STATUS_INVALID_PARAMETER;
  }
  return status;
}

void create_port_object(std::set<attr_w> &port_attrs,
                        std::promise<switch_status_t> status,
                        std::promise<switch_object_id_t> oid) {
  switch_status_t _status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t _oid{};
  _status =
      switch_store::object_create(SWITCH_OBJECT_TYPE_PORT, port_attrs, _oid);
  status.set_value(_status);
  if (_status != SWITCH_STATUS_SUCCESS) return;
  oid.set_value(_oid);
  return;
}

void cleanup_port_object(std::vector<switch_object_id_t> &port_handles,
                         std::promise<switch_status_t> status) {
  switch_status_t _status = SWITCH_STATUS_SUCCESS;
  for (auto port_handle : port_handles) {
    _status |= switch_store::object_delete(port_handle);
    if (_status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}:{}: Port cleanup failed for internal port {}:{}",
                 __func__,
                 __LINE__,
                 port_handle,
                 _status);
    }
  }
  status.set_value(_status);
  return;
}

switch_status_t before_port_create_add_internal_ports(
    const switch_object_type_t object_type, std::set<attr_w> &attrs) {
  (void)object_type;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_enum_t port_type{SWITCH_PORT_ATTR_TYPE_NORMAL};
  std::vector<uint32_t> lane_list;
  uint32_t lane_list_size = 0;
  bool bmai_managed_internal_ports =
      SWITCH_CONTEXT.is_internal_pipe_ports_bmai_managed();
  if (!bmai_managed_internal_ports) return status;
  if (switch_store::smiContext::context().in_warm_init()) return status;

  // Add internal ports only for regular front panel ports
  const auto port_attr_type_it =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_TYPE));
  if (port_attr_type_it == attrs.end()) {
    status = SWITCH_STATUS_FAILURE;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}:{}: Port attr type missing",
               __func__,
               __LINE__);
    return status;
  } else {
    status = (*port_attr_type_it).v_get(port_type);
    if (status != SWITCH_STATUS_SUCCESS) return status;
  }
  if (port_type.enumdata != SWITCH_PORT_ATTR_TYPE_NORMAL) {
    return status;
  }

  // Find external port dev port
  const auto attr_lane_list =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_LANE_LIST));
  if (attr_lane_list == attrs.end()) {
    return SWITCH_STATUS_FAILURE;
  }
  uint64_t ext_port_id{};
  uint16_t ext_dev_port{};
  status = attr_lane_list->v_get(lane_list);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  ext_port_id = lane_list[0];
  lane_list_size = lane_list.size();

  status = SWITCH_CONTEXT.get_dev_port_for_fp_idx(
      static_cast<uint16_t>(ext_port_id), ext_dev_port);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  // Add Internal ports only for external ports. Skip if the current port
  // being added is an internal port
  if (!check_port_in_switch_pipe(ext_dev_port)) return status;

  // Internal ports are added with same attrs as the external ports except
  // their unique port id, loopback mode=MAC NEAR and mtu=max_mtu.
  // For 10G ports we add the internal ports at 25G speed to get additional
  // bandwith on internal pipe
  auto internal_port_attrs = attrs;

  uint32_t _port_speed = 10000;
  auto port_speed_it = internal_port_attrs.find(
      static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_SPEED));
  if (port_speed_it != internal_port_attrs.end()) {
    (port_speed_it)->v_get(_port_speed);
    if (_port_speed == 10000) internal_port_attrs.erase(port_speed_it);
  }
  if (_port_speed == 10000) {
    uint32_t speed = 25000;
    attr_w port_speed_attr(SWITCH_PORT_ATTR_SPEED);
    port_speed_attr.v_set(speed);
    internal_port_attrs.insert(port_speed_attr);
  }

  auto port_loopback_mode_it = internal_port_attrs.find(
      static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_LOOPBACK_MODE));
  switch_enum_t port_loopback_mode{SWITCH_PORT_ATTR_LOOPBACK_MODE_MAC_NEAR};
  if (port_loopback_mode_it != internal_port_attrs.end()) {
    internal_port_attrs.erase(port_loopback_mode_it);
  }
  attr_w port_loopback_attr(SWITCH_PORT_ATTR_LOOPBACK_MODE);
  port_loopback_attr.v_set(port_loopback_mode);
  internal_port_attrs.insert(port_loopback_attr);

  uint32_t rx_mtu{MAX_MAC_MTU}, tx_mtu{MAX_MAC_MTU};
  auto port_rx_mtu_it = internal_port_attrs.find(
      static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_RX_MTU));
  if (port_rx_mtu_it != internal_port_attrs.end()) {
    internal_port_attrs.erase(port_rx_mtu_it);
  }
  attr_w port_rx_mtu_attr(SWITCH_PORT_ATTR_RX_MTU);
  port_rx_mtu_attr.v_set(rx_mtu);
  internal_port_attrs.insert(port_rx_mtu_attr);

  auto port_tx_mtu_it = internal_port_attrs.find(
      static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_TX_MTU));
  if (port_tx_mtu_it != internal_port_attrs.end()) {
    internal_port_attrs.erase(port_tx_mtu_it);
  }
  attr_w port_tx_mtu_attr(SWITCH_PORT_ATTR_TX_MTU);
  port_tx_mtu_attr.v_set(tx_mtu);
  internal_port_attrs.insert(port_tx_mtu_attr);

  auto port_is_internal_it = internal_port_attrs.find(
      static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_INTERNAL_OBJECT));
  if (port_is_internal_it != internal_port_attrs.end()) {
    internal_port_attrs.erase(port_is_internal_it);
  }
  attr_w port_is_internal_attr(SWITCH_PORT_ATTR_INTERNAL_OBJECT);
  port_is_internal_attr.v_set(true);
  internal_port_attrs.insert(port_is_internal_attr);

  // Add no non-default ppgs for internal ports
  auto port_non_def_ppg_it = internal_port_attrs.find(
      static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_NON_DEF_PPGS));
  if (port_non_def_ppg_it != internal_port_attrs.end()) {
    internal_port_attrs.erase(port_non_def_ppg_it);
  }
  attr_w port_non_def_ppg_attr(SWITCH_PORT_ATTR_NON_DEF_PPGS);
  port_non_def_ppg_attr.v_set(
      static_cast<uint32_t>(INTERNAL_PORT_NUM_NON_DEF_PPGS));
  internal_port_attrs.insert(port_non_def_ppg_attr);

  // Get list of non-switch egress pipelines
  auto &switch_egress_pipes = SWITCH_CONTEXT.get_switch_egress_pipe_list();
  std::vector<bf_dev_pipe_t> egress_pipe_list(switch_egress_pipes.begin(),
                                              switch_egress_pipes.end());
  std::set<bf_dev_pipe_t> non_switch_egress_pipes;
  auto active_pipes = get_active_pipes();
  // non switch egress is all pipes - egress pipes
  std::set_difference(
      active_pipes.begin(),
      active_pipes.end(),
      switch_egress_pipes.begin(),
      switch_egress_pipes.end(),
      std::inserter(non_switch_egress_pipes, non_switch_egress_pipes.end()));

  std::vector<bf_dev_pipe_t> internal_pipes;
  if (non_switch_egress_pipes.size() == switch_egress_pipes.size()) {
    bf_dev_pipe_t ext_pipe = DEV_PORT_TO_PIPE(ext_dev_port);
    auto internal_pipe_it = non_switch_egress_pipes.begin();
    auto external_pipe_it = switch_egress_pipes.begin();
    while (internal_pipe_it != non_switch_egress_pipes.end()) {
      if (*external_pipe_it == ext_pipe) {
        internal_pipes.push_back(*internal_pipe_it);
        break;
      }
      internal_pipe_it++;
      external_pipe_it++;
    }
  } else if (switch_egress_pipes.size() == 1) {
    std::copy(non_switch_egress_pipes.begin(),
              non_switch_egress_pipes.end(),
              std::inserter(internal_pipes, internal_pipes.begin()));
  } else {
    status = SWITCH_STATUS_NOT_SUPPORTED;
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}:{}: (Asymmetric Folded Pipeline) Failed to add internal "
               "ports on internal pipes,"
               " number of external pipes:{} number of internal pipes: {}, AFP "
               "is supported only when #external pipes = #internal pipes or "
               "#external pipes = 1"
               "status: {}",
               __func__,
               __LINE__,
               switch_egress_pipes.size(),
               non_switch_egress_pipes.size(),
               status);
    return status;
  }
  std::vector<bf_dev_port_t> internal_dev_ports;
  std::vector<switch_object_id_t> internal_port_handles;

  // Add corresponding internal ports in non-switch egress pipelines
  attr_w internal_port_list_attr(SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST);
  for (auto pipe : internal_pipes) {
    bf_dev_port_t dp = (ext_dev_port & 0x7f) | (pipe << 7);
    uint64_t port_id{};
    status = SWITCH_CONTEXT.get_fp_idx_for_dev_port(dp, port_id);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}:{}: (Asymmetric Folded Pipeline) Failed to get port id "
                 "for internal with dev_port {} corresponding to external dev "
                 "port {}: {}",
                 __func__,
                 __LINE__,
                 dp,
                 ext_dev_port,
                 status);
      return status;
    }
    // Update Internal Port Port_ID
    const auto port_id_it = internal_port_attrs.find(
        static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_LANE_LIST));
    if (port_id_it != internal_port_attrs.end()) {
      internal_port_attrs.erase(port_id_it);
    }

    attr_w port_lane_list_attr(SWITCH_PORT_ATTR_LANE_LIST);
    std::vector<uint32_t> port_lane_list;
    for (uint32_t i = 0; i < lane_list_size; i++) {
      port_lane_list.push_back(port_id + i);
    }
    port_lane_list_attr.v_set(port_lane_list);
    internal_port_attrs.insert(port_lane_list_attr);

    std::promise<switch_status_t> fstatus;
    std::promise<switch_object_id_t> foid;
    auto oid = foid.get_future();
    auto port_status = fstatus.get_future();
    std::thread internal_port_create(&create_port_object,
                                     std::ref(internal_port_attrs),
                                     std::move(fstatus),
                                     std::move(foid));
    internal_port_create.join();
    status = port_status.get();
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}:{}: (Asymmetric Folded Pipeline) Failed to add internal "
                 "port {} (dev_port:{}) on pipe {} corresponding to external "
                 "dev port {}: {}",
                 __func__,
                 __LINE__,
                 port_id,
                 dp,
                 pipe,
                 ext_dev_port,
                 status);
      goto cleanup;
    }
    switch_object_id_t port_handle{oid.get()};
    internal_port_handles.push_back(port_handle);
  }
  internal_port_list_attr.v_set(internal_port_handles);
  attrs.insert(internal_port_list_attr);
  return status;
cleanup:
  std::promise<switch_status_t> fcstatus;
  auto cleanup_status = fcstatus.get_future();
  std::thread internal_port_cleanup(&cleanup_port_object,
                                    std::ref(internal_port_handles),
                                    std::move(fcstatus));
  internal_port_cleanup.join();
  status = cleanup_status.get();
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}:{}: (Asymmetric Folded Pipeline) Internal Ports cleanup "
               "failed:{}",
               __func__,
               __LINE__,
               status);
  }
  return status;
}

switch_status_t before_port_create(const switch_object_type_t object_type,
                                   std::set<attr_w> &attrs) {
  (void)object_type;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  uint16_t device = 0;
  bf_dev_id_t bf_dev_id = static_cast<bf_dev_id_t>(device);
  uint64_t port_num = 0;
  switch_enum_t port_type = {0};
  switch_object_id_t device_handle = {};
  uint16_t dev_port = 0;
  uint16_t yid = 0;
  uint32_t chnl_id = 0, conn_id = 0;
  std::vector<uint32_t> lane_list;

  const auto attr_dev =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_DEVICE));
  CHECK_RET(attr_dev == attrs.end(), SWITCH_STATUS_FAILURE);
  status = (*attr_dev).v_get(device_handle);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status =
      switch_store::v_get(device_handle, SWITCH_DEVICE_ATTR_DEV_ID, device);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  bf_dev_id = static_cast<bf_dev_id_t>(device);

  const auto attr_it =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_LANE_LIST));
  CHECK_RET(attr_it == attrs.end(), SWITCH_STATUS_FAILURE);
  status = (*attr_it).v_get(lane_list);
  port_num = lane_list[0];
  if (status != SWITCH_STATUS_SUCCESS) return status;

  const auto attr_type =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_TYPE));
  if (attr_type == attrs.end()) {
    port_type.enumdata = SWITCH_PORT_ATTR_TYPE_NORMAL;
  } else {
    status = (*attr_type).v_get(port_type);
    if (status != SWITCH_STATUS_SUCCESS) return status;
  }

  std::set<attr_w> port_attrs;
  switch_object_id_t port_handle = {};
  port_attrs.insert(attr_w(SWITCH_PORT_ATTR_DEVICE, device_handle));
  port_attrs.insert(attr_w(SWITCH_PORT_ATTR_PORT_ID, port_num));
  status = switch_store::object_id_get_wkey(
      SWITCH_OBJECT_TYPE_PORT, port_attrs, port_handle);
  if (status == SWITCH_STATUS_ITEM_NOT_FOUND) {
    // nothing to do, reset status and continue
    status = SWITCH_STATUS_SUCCESS;
  } else if (status == SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}:{}: Port already exists with port ID {} oid {:#x}",
               __func__,
               __LINE__,
               port_num,
               port_handle.data);
    return SWITCH_STATUS_ITEM_ALREADY_EXISTS;
  }

  if (port_type.enumdata == SWITCH_PORT_ATTR_TYPE_RECIRC) {
    bf_dev_port_t recirc_dev_port = 0;
    status = recirc_port_to_dev_port_map(device, port_num, &recirc_dev_port);
    if (status != SWITCH_STATUS_SUCCESS) return status;
    dev_port = static_cast<uint16_t>(recirc_dev_port);
  } else {
    status = get_dev_port(bf_dev_id, port_num, &dev_port);
    if (status != SWITCH_STATUS_SUCCESS) return status;
  }

  // Update CPU port
  if (port_type.enumdata == SWITCH_PORT_ATTR_TYPE_CPU) {
    bf_dev_pipe_t pipe = DEV_PORT_TO_PIPE(dev_port);
    auto &&switch_egress_pipes = SWITCH_CONTEXT.get_switch_egress_pipe_list();
    if (std::find(switch_egress_pipes.begin(),
                  switch_egress_pipes.end(),
                  pipe) != switch_egress_pipes.end()) {
      p4_devport_mgr_set_copy_to_cpu(device, true, dev_port);
    } else {
      // For asymmetric folded pipeline if the CPU port is on a non
      // SwitchEgress pipe, set the cpu port as the recirc port on
      // SwitchEgress pipeline. This ensure that CPU bound packets get
      // processed by Switch Egress. The recirc
      // port ensures that the packet is recirculated back to the ingress. In
      // the egress a separate table (see fold_cpu
      // in port.cpp) installs an entry to forward recirc packets to the
      // correct cpu dev_port
      uint16_t recirc_port{};
      status = get_recirc_port_in_pipe(
          device_handle, EGRESS_DEV_PORT_TO_PIPE(dev_port), recirc_port);
      CHECK_RET(status != SWITCH_STATUS_SUCCESS, status);
      p4_devport_mgr_set_copy_to_cpu(device, true, recirc_port);
      attr_w folded_cpu_port(SWITCH_DEVICE_ATTR_FOLDED_CPU_DEV_PORT);
      folded_cpu_port.v_set(translate_egress_port(recirc_port));
      status |= switch_store::attribute_set(device_handle, folded_cpu_port);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: Asymmetric Folded Pipeline: Failed to update folded "
                   "cpu dev port in store to {} "
                   "status {}",
                   __func__,
                   __LINE__,
                   recirc_port,
                   status);
        return status;
      }
    }
  }

  attrs.erase(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_DEV_PORT));
  auto insert_ret = attrs.insert(attr_w(SWITCH_PORT_ATTR_DEV_PORT, dev_port));
  CHECK_RET(insert_ret.second == false, SWITCH_STATUS_FAILURE);
  const auto attrs_it =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_PORT_ID));
  if (attrs_it == attrs.end()) {
    insert_ret = attrs.insert(attr_w(SWITCH_PORT_ATTR_PORT_ID, port_num));
    CHECK_RET(insert_ret.second == false, SWITCH_STATUS_FAILURE);
  }
  attrs.erase(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_CHANNEL_ID));
  attrs.erase(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_CONNECTOR_ID));
  if ((port_type.enumdata != SWITCH_PORT_ATTR_TYPE_CPU) &&
      (port_type.enumdata != SWITCH_PORT_ATTR_TYPE_RECIRC)) {
    bf_pal_front_port_handle_t fp_handle;
    status = bf_pal_dev_port_to_front_port_get(device, dev_port, &fp_handle);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to get front port handle for dev port {} "
                 "status {}",
                 __func__,
                 __LINE__,
                 dev_port,
                 status);
      return status;
    }
    chnl_id = fp_handle.chnl_id;
    conn_id = fp_handle.conn_id;

    status = validate_port_attributes(bf_dev_id, dev_port, attrs);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SMI_CREATE_OPERATION,
                 "{}.{}: Validate port attributes fail with status {}",
                 __func__,
                 __LINE__,
                 status);
      return status;
    }
  }
  insert_ret = attrs.insert(attr_w(SWITCH_PORT_ATTR_CHANNEL_ID, chnl_id));
  CHECK_RET(insert_ret.second == false, SWITCH_STATUS_FAILURE);
  insert_ret = attrs.insert(attr_w(SWITCH_PORT_ATTR_CONNECTOR_ID, conn_id));
  CHECK_RET(insert_ret.second == false, SWITCH_STATUS_FAILURE);

  // YID will be present only for warm-init case
  const auto attr_yid =
      attrs.find(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_YID));
  if (attr_yid != attrs.end()) {
    status = (*attr_yid).v_get(yid);
    SWITCH_CONTEXT.update_yid(yid, SWITCH_OBJECT_TYPE_PORT);
  } else {
    if (port_type.enumdata != SWITCH_PORT_ATTR_TYPE_RECIRC) {
      attrs.erase(static_cast<switch_attr_id_t>(SWITCH_PORT_ATTR_YID));
      auto yid_ret = attrs.insert(
          attr_w(SWITCH_PORT_ATTR_YID,
                 SWITCH_CONTEXT.reserve_yid(SWITCH_OBJECT_TYPE_PORT)));
      CHECK_RET(yid_ret.second == false, SWITCH_STATUS_FAILURE);
    }
  }
  return status;
}

switch_status_t adv_fec_apply(const attr_w &attr,
                              const switch_object_id_t object_id,
                              uint64_t port_id,
                              bf_dev_id_t bf_dev_id,
                              bf_dev_port_t bf_dev_port) {
  std::vector<switch_enum_t> adv_fec_type = {};
  switch_status_t status = attr.v_get(adv_fec_type);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status = adv_fec_type_set(object_id,
                            port_id,
                            bf_dev_id,
                            bf_dev_port,
                            adv_fec_type.data(),
                            adv_fec_type.size());
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: adv_fec_type failure for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }
  return status;
}
switch_status_t port_attribute_set(const switch_object_id_t object_id,
                                   const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  bool admin_state_enabled = 0;
  uint16_t device = 0;
  bf_dev_id_t bf_dev_id = 0;
  switch_object_id_t device_hdl = {};
  switch_port_attr_fec_type fec_type = {};
  switch_port_attr_loopback_mode lb_mode = {};
  switch_port_attr_autoneg autoneg = {};
  switch_port_attr_link_pause link_pause = {};
  switch_port_attr_media_type media_type = {};
  uint32_t speed = {};
  switch_enum_t _fec_type{}, _lb_mode{}, _autoneg{}, _link_pause{},
      _media_type{}, _port_type{};
  std::vector<uint32_t> adv_speed = {};
  uint32_t rx_mtu{0}, tx_mtu{0};
  bf_dev_port_t bf_dev_port = 0;
  uint16_t dev_port = 0;
  uint64_t port_id = 0;
  bool pkt_tx_enable = false;
  std::vector<uint32_t> lane_list;

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_DEVICE, device_hdl);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get device for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }

  status |= switch_store::v_get(device_hdl, SWITCH_DEVICE_ATTR_DEV_ID, device);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get dev_id for device handle {} status {}",
               __func__,
               __LINE__,
               device_hdl,
               status);
    return status;
  }
  bf_dev_id = static_cast<bf_dev_id_t>(device);

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_TYPE, _port_type);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get port type for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }
  // nothing to do for recirc ports
  if (_port_type.enumdata == SWITCH_PORT_ATTR_TYPE_RECIRC) return status;

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get dev port for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }
  bf_dev_port = static_cast<bf_dev_port_t>(dev_port);

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_PORT_ID, port_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get port id for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }

  switch (attr.id_get()) {
    case SWITCH_PORT_ATTR_ADMIN_STATE:
      status = attr.v_get(admin_state_enabled);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      if (admin_state_enabled) {
        status = port_enabled(port_id, bf_dev_id, bf_dev_port, true);
      } else {
        status = port_enabled(port_id, bf_dev_id, bf_dev_port, false);
      }
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: port_enable failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_AUTONEG:
      status = attr.v_get(_autoneg);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      autoneg = static_cast<switch_port_attr_autoneg>(_autoneg.enumdata);
      status = autoneg_set(object_id, port_id, bf_dev_id, bf_dev_port, autoneg);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: autoneg_set failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_ADVERTISED_SPEED:
      status = attr.v_get(adv_speed);
      if (status != SWITCH_STATUS_SUCCESS) return status;

      status = adv_speed_set(object_id,
                             port_id,
                             bf_dev_id,
                             bf_dev_port,
                             adv_speed.data(),
                             adv_speed.size());
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: adv_speed_set failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_ADVERTISED_FEC_TYPE:
      status = adv_fec_apply(attr, object_id, port_id, bf_dev_id, bf_dev_port);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      break;
    case SWITCH_PORT_ATTR_LINK_PAUSE:
      status = attr.v_get(_link_pause);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      link_pause =
          static_cast<switch_port_attr_link_pause>(_link_pause.enumdata);
      status = link_pause_set(
          object_id, port_id, bf_dev_id, bf_dev_port, link_pause);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: link_pause_set failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_MEDIA_TYPE:
      status = attr.v_get(_media_type);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      media_type =
          static_cast<switch_port_attr_media_type>(_media_type.enumdata);
      status = media_type_set(
          object_id, port_id, bf_dev_id, bf_dev_port, media_type);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: media_type_set failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_RX_MTU:
      status = attr.v_get(rx_mtu);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_TX_MTU, tx_mtu);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      status =
          mtu_set(object_id, port_id, bf_dev_id, bf_dev_port, rx_mtu, tx_mtu);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: rx_mtu set failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_TX_MTU:
      status = attr.v_get(tx_mtu);
      if (status != SWITCH_STATUS_SUCCESS) return status;

      status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_RX_MTU, rx_mtu);
      if (status != SWITCH_STATUS_SUCCESS) return status;

      status =
          mtu_set(object_id, port_id, bf_dev_id, bf_dev_port, rx_mtu, tx_mtu);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: tx_mtu set failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_FEC_TYPE:
      status = attr.v_get(_fec_type);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      fec_type = static_cast<switch_port_attr_fec_type>(_fec_type.enumdata);
      status =
          fec_type_set(object_id, port_id, bf_dev_id, bf_dev_port, fec_type);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: fec_type_set failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_LOOPBACK_MODE:
      if (_port_type.enumdata == SWITCH_PORT_ATTR_TYPE_CPU) break;
      status = attr.v_get(_lb_mode);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      lb_mode = static_cast<switch_port_attr_loopback_mode>(_lb_mode.enumdata);
      status = loopback_mode_set(bf_dev_id, bf_dev_port, lb_mode);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(
            SWITCH_API_LEVEL_ERROR,
            SWITCH_OBJECT_TYPE_PORT,
            "{}.{}: loopback_mode_set failure for port handle {} status {}",
            __func__,
            __LINE__,
            object_id,
            status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_SPEED:
      status = attr.v_get(speed);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      status = speed_set(object_id, port_id, bf_dev_id, bf_dev_port, speed);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: speed_set failure for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
      break;
    case SWITCH_PORT_ATTR_SCHEDULER_HANDLE: {
      switch_object_id_t sch_profile_handle = {0};

      status = attr.v_get(sch_profile_handle);
      if (status != SWITCH_STATUS_SUCCESS) return status;

      status = scheduler_profile_set(object_id, sch_profile_handle);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: scheduler profle set failure for port handle {} "
                   "status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
    } break;
    case SWITCH_PORT_ATTR_FLOW_CONTROL: {
      switch_enum_t e;
      _switch_port_attr_flow_control fc_mode;
      status = attr.v_get(e);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      fc_mode = static_cast<_switch_port_attr_flow_control>(e.enumdata);
      status = port_flowcontrol_mode_set(bf_dev_port, fc_mode);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: flow_control_mode_set failure for port handle {} "
                   "status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
    } break;
    case SWITCH_PORT_ATTR_PFC_MAP: {
      uint32_t pfc_map = 0;
      switch_enum_t pfc_mode = {SWITCH_PORT_ATTR_PFC_MODE_COMBINED};
      status |=
          switch_store::v_get(object_id, SWITCH_PORT_ATTR_PFC_MODE, pfc_mode);

      if (pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_COMBINED) {
        status = attr.v_get(pfc_map);
        if (status != SWITCH_STATUS_SUCCESS) return status;
        status = port_pfc_set(bf_dev_id, bf_dev_port, pfc_map, pfc_map);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT,
                     "{}.{}: pfc_map set failed for port handle {} status {}",
                     __func__,
                     __LINE__,
                     object_id,
                     status);
          return status;
        }
      }

      switch_object_id_t pfc_queue_qos_map_handle = {0};
      status |= switch_store::v_get(
          object_id,
          SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,
          pfc_queue_qos_map_handle);

      status =
          queue_tail_drop_set(object_id, pfc_queue_qos_map_handle, pfc_map);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(
            SWITCH_API_LEVEL_ERROR,
            SWITCH_OBJECT_TYPE_PORT,
            "{}.{}: Queue tail drop set failed for port handle {} status {}",
            __func__,
            __LINE__,
            object_id,
            status);
        return status;
      }
    } break;
    case SWITCH_PORT_ATTR_PFC_MODE: {
      switch_enum_t pfc_mode = {SWITCH_PORT_ATTR_PFC_MODE_COMBINED};
      uint32_t rx_pfc_map = 0;
      uint32_t tx_pfc_map = 0;

      status |= attr.v_get(pfc_mode);

      if (pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_SEPARATE) {
        status |= switch_store::v_get(
            object_id, SWITCH_PORT_ATTR_TX_PFC_MAP, tx_pfc_map);
        status |= switch_store::v_get(
            object_id, SWITCH_PORT_ATTR_RX_PFC_MAP, rx_pfc_map);
      } else {
        status |= switch_store::v_get(
            object_id, SWITCH_PORT_ATTR_PFC_MAP, tx_pfc_map);
        rx_pfc_map = tx_pfc_map;
      }

      if (status != SWITCH_STATUS_SUCCESS) return status;
      status = port_pfc_set(bf_dev_id, bf_dev_port, tx_pfc_map, rx_pfc_map);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: pfc_map set failed for port handle {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
    } break;
    case SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE:
    case SWITCH_PORT_ATTR_RX_PFC_MAP:
    case SWITCH_PORT_ATTR_TX_PFC_MAP: {
      auto attr_id = attr.id_get();
      switch_enum_t pfc_mode = {SWITCH_PORT_ATTR_PFC_MODE_COMBINED};
      uint32_t rx_pfc_map = 0;
      uint32_t tx_pfc_map = 0;
      uint32_t pfc_map = 0;
      switch_object_id_t pfc_queue_qos_map_handle = {0};

      status =
          switch_store::v_get(object_id, SWITCH_PORT_ATTR_PFC_MODE, pfc_mode);
      status |= switch_store::v_get(
          object_id, SWITCH_PORT_ATTR_TX_PFC_MAP, tx_pfc_map);
      status |= switch_store::v_get(
          object_id, SWITCH_PORT_ATTR_RX_PFC_MAP, rx_pfc_map);
      status |=
          switch_store::v_get(object_id, SWITCH_PORT_ATTR_PFC_MAP, pfc_map);
      status |= switch_store::v_get(
          object_id,
          SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE,
          pfc_queue_qos_map_handle);

      if (attr_id == SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE) {
        status |= attr.v_get(pfc_queue_qos_map_handle);
        if (pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_COMBINED) {
          rx_pfc_map = pfc_map;
          tx_pfc_map = pfc_map;
        }
      }
      if (pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_SEPARATE) {
        if (attr_id == SWITCH_PORT_ATTR_RX_PFC_MAP) {
          status |= attr.v_get(rx_pfc_map);
        } else if (attr_id == SWITCH_PORT_ATTR_TX_PFC_MAP) {
          status |= attr.v_get(tx_pfc_map);
        }
      }
      if (status != SWITCH_STATUS_SUCCESS) return status;
      if ((attr_id == SWITCH_PORT_ATTR_TX_PFC_MAP &&
           pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_SEPARATE) ||
          attr_id == SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE) {
        status = queue_tail_drop_set(
            object_id, pfc_queue_qos_map_handle, tx_pfc_map);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT,
                     "{}.{}: Queue tail drop set failed for port handle {} "
                     "status {}",
                     __func__,
                     __LINE__,
                     object_id,
                     status);
          return status;
        }
      }
      if ((pfc_mode.enumdata == SWITCH_PORT_ATTR_PFC_MODE_SEPARATE) &&
          (attr_id == SWITCH_PORT_ATTR_TX_PFC_MAP ||
           attr_id == SWITCH_PORT_ATTR_RX_PFC_MAP)) {
        status = port_pfc_set(bf_dev_id, bf_dev_port, tx_pfc_map, rx_pfc_map);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT,
                     "{}.{}: pfc_map set failed for port handle {} status {}",
                     __func__,
                     __LINE__,
                     object_id,
                     status);
          return status;
        }
      }
    } break;
    case SWITCH_PORT_ATTR_PKT_TX_ENABLE: {
      bool sw_model = false;
      bf_pal_pltfm_type_get(device, &sw_model);
      // Apply this attribute only to normal ports and on hardware
      // When pkt_tx is disabled, set the port-shaper to zero.
      // When pkt_tx is enabled, disable the port-shaper.
      // Todo: Need to check if the port shaper is enabled and apply the rate
      // properly.
      if (!sw_model && _port_type.enumdata == SWITCH_PORT_ATTR_TYPE_NORMAL) {
        status = attr.v_get(pkt_tx_enable);
        if (status != SWITCH_STATUS_SUCCESS) return status;

        status |= switch_store::v_get(
            object_id, SWITCH_PORT_ATTR_PKT_TX_ENABLE, pkt_tx_enable);
        if (status != SWITCH_STATUS_SUCCESS) return status;

        if (pkt_tx_enable) {
          status = bfrt_tm_port_sched_cfg_max_rate_enable_set(dev_port, false);
          if (status != SWITCH_STATUS_SUCCESS) {
            switch_log(SWITCH_API_LEVEL_ERROR,
                       SWITCH_OBJECT_TYPE_PORT,
                       "{}.{}: Failure to disable Port shaping for port handle"
                       "{} status {}",
                       __func__,
                       __LINE__,
                       object_id,
                       status);
            return status;
          }
        } else {
          status = bfrt_tm_port_sched_cfg_max_rate_enable_set(dev_port, true);
          if (status != SWITCH_STATUS_SUCCESS) {
            switch_log(SWITCH_API_LEVEL_ERROR,
                       SWITCH_OBJECT_TYPE_PORT,
                       "{}.{}: Failure to enable Port shaping for port handle"
                       "{} status {}",
                       __func__,
                       __LINE__,
                       object_id,
                       status);
            return status;
          }
          status = bfrt_tm_port_sched_shaping_rate_set(dev_port, "PPS", 0, 0);
          if (status != SWITCH_STATUS_SUCCESS) {
            switch_log(SWITCH_API_LEVEL_ERROR,
                       SWITCH_OBJECT_TYPE_PORT,
                       "{}.{}: Failure to set the port shaper rate to zero for "
                       "port handle"
                       "{} status {}",
                       __func__,
                       __LINE__,
                       object_id,
                       status);
            return status;
          }
        }
      }
    } break;
    default:
      break;
  }
  return status;
}

void delete_port_object(const switch_object_id_t object_id,
                        std::promise<switch_status_t> status) {
  attr_w internal_port_list(SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST);
  std::vector<switch_object_id_t> internal_port_handles;
  std::vector<switch_object_id_t> unclean_port_handles;

  switch_status_t _status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST, internal_port_list);
  switch_status_t temp_status = SWITCH_STATUS_SUCCESS;
  if (_status == SWITCH_STATUS_SUCCESS) {
    internal_port_list.v_get(internal_port_handles);
    for (auto handle : internal_port_handles) {
      temp_status = switch_store::object_delete(handle);
      if (temp_status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: Failure to delete internal port {} for port handle "
                   "{} status {}",
                   __func__,
                   __LINE__,
                   handle,
                   object_id,
                   temp_status);
        unclean_port_handles.push_back(handle);
      }
      _status |= temp_status;
    }
  }
  attr_w dangling_port_list(SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST);
  dangling_port_list.v_set(unclean_port_handles);
  temp_status = switch_store::attribute_set(object_id, dangling_port_list);
  if (temp_status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: Failure to update internal port list for port handle "
               "{} status {}",
               __func__,
               __LINE__,
               object_id,
               temp_status);
  }
  status.set_value(_status | temp_status);
}

switch_status_t before_port_delete_delete_internal_ports(
    const switch_object_id_t object_id) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;

  bool bmai_managed_internal_ports =
      SWITCH_CONTEXT.is_internal_pipe_ports_bmai_managed();
  if (!bmai_managed_internal_ports) return status;
  std::promise<switch_status_t> dstatus;
  auto delete_status = dstatus.get_future();
  std::thread internal_port_delete(
      &delete_port_object, object_id, std::move(dstatus));
  internal_port_delete.join();
  return delete_status.get();
}

void update_port_object(const switch_object_id_t object_id,
                        const attr_w &attr,
                        std::promise<switch_status_t> status) {
  attr_w internal_port_list(SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST);
  std::vector<switch_object_id_t> internal_port_handles;
  switch_status_t _status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_INTERNAL_PIPE_PORT_LIST, internal_port_list);
  if (_status == SWITCH_STATUS_SUCCESS) {
    internal_port_list.v_get(internal_port_handles);
    for (auto handle : internal_port_handles) {
      switch_status_t temp_status = switch_store::attribute_set(handle, attr);
      if (temp_status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: Failure to delete internal port {} for port handle "
                   "{} status {}",
                   __func__,
                   __LINE__,
                   handle,
                   object_id,
                   temp_status);
      }
      _status |= temp_status;
    }
  }
  status.set_value(_status);
  return;
}

switch_status_t before_port_update_update_internal_ports(
    const switch_object_id_t object_id, const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool bmai_managed_internal_ports =
      SWITCH_CONTEXT.is_internal_pipe_ports_bmai_managed();
  if (!bmai_managed_internal_ports) return status;
  if (switch_store::smiContext::context().in_warm_init()) return status;
  switch_attr_id_t attr_id = attr.id_get();
  switch (attr_id) {
    case SWITCH_PORT_ATTR_AUTONEG:
    case SWITCH_PORT_ATTR_LINK_PAUSE:
    case SWITCH_PORT_ATTR_MEDIA_TYPE:
    case SWITCH_PORT_ATTR_FEC_TYPE:
    case SWITCH_PORT_ATTR_LOOPBACK_MODE:
    case SWITCH_PORT_ATTR_SPEED:
    case SWITCH_PORT_ATTR_ADMIN_STATE:
    case SWITCH_PORT_ATTR_PKT_TX_ENABLE:
      break;
    // This will install icos ppg mapping for internal pipe ports in the TM
    case SWITCH_PORT_ATTR_ICOS_PPG_QOS_MAP_HANDLE:
    // This will enable pfc prio at MAC level for internal pipe ports
    case SWITCH_PORT_ATTR_RX_PFC_MAP:
    case SWITCH_PORT_ATTR_TX_PFC_MAP:
    case SWITCH_PORT_ATTR_PFC_MAP:
    case SWITCH_PORT_ATTR_PFC_MODE:
    case SWITCH_PORT_ATTR_FLOW_CONTROL:
    case SWITCH_PORT_ATTR_DROP_LIMIT:
    case SWITCH_PORT_ATTR_DROP_HYSTERISIS:
    case SWITCH_PORT_ATTR_PFC_COS_MAP:
    case SWITCH_PORT_ATTR_SKID_LIMIT:
    case SWITCH_PORT_ATTR_PFC_PRIORITY_QUEUE_QOS_MAP_HANDLE:
      if (feature::is_feature_set(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE)) {
        break;
      } else {
        return SWITCH_STATUS_SUCCESS;
      }
    default:
      // We update only the PAL attributes for internal ports
      return SWITCH_STATUS_SUCCESS;
      break;
  }
  std::promise<switch_status_t> fstatus;
  auto update_status = fstatus.get_future();
  std::thread internal_port_update(
      &update_port_object, object_id, std::ref(attr), std::move(fstatus));
  internal_port_update.join();
  return update_status.get();
}

switch_status_t before_port_update(const switch_object_id_t object_id,
                                   const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  // always update unless same as previous value
  bool update_pal = true;
  switch_attr_id_t attr_id = attr.id_get();

  switch (attr_id) {
    case SWITCH_PORT_ATTR_AUTONEG: {
      switch_enum_t _autoneg = {}, autoneg = {};
      attr.v_get(_autoneg);
      status = switch_store::v_get(object_id, attr_id, autoneg);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_autoneg.enumdata == autoneg.enumdata) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_LINK_PAUSE: {
      switch_enum_t _link_pause = {}, link_pause = {};
      attr.v_get(_link_pause);
      status = switch_store::v_get(object_id, attr_id, link_pause);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_link_pause.enumdata == link_pause.enumdata) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_MEDIA_TYPE: {
      switch_enum_t _media_type = {}, media_type = {};
      attr.v_get(_media_type);
      status = switch_store::v_get(object_id, attr_id, media_type);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_media_type.enumdata == media_type.enumdata) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_RX_MTU: {
      uint32_t _rx_mtu = 0, rx_mtu = 0;
      attr.v_get(_rx_mtu);
      status = switch_store::v_get(object_id, attr_id, rx_mtu);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_rx_mtu == rx_mtu) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_TX_MTU: {
      uint32_t _tx_mtu = 0, tx_mtu = 0;
      attr.v_get(_tx_mtu);
      status = switch_store::v_get(object_id, attr_id, tx_mtu);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_tx_mtu == tx_mtu) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_FEC_TYPE: {
      switch_enum_t _fec_type = {}, fec_type = {};
      attr.v_get(_fec_type);
      status = switch_store::v_get(object_id, attr_id, fec_type);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_fec_type.enumdata == fec_type.enumdata) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_LOOPBACK_MODE: {
      switch_enum_t _lb_mode = {}, lb_mode = {};
      attr.v_get(_lb_mode);
      status = switch_store::v_get(object_id, attr_id, lb_mode);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_lb_mode.enumdata == lb_mode.enumdata) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_SPEED: {
      uint32_t _speed = {}, speed = {};
      attr.v_get(_speed);
      status = switch_store::v_get(object_id, attr_id, speed);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_speed == speed) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_PKT_TX_ENABLE: {
      bool _tx_enable = {}, tx_enable = {};
      attr.v_get(_tx_enable);
      status = switch_store::v_get(object_id, attr_id, tx_enable);
      if (status != SWITCH_STATUS_SUCCESS) break;
      if (_tx_enable == tx_enable) update_pal = false;
    } break;
    case SWITCH_PORT_ATTR_ADVERTISED_SPEED: {
      std::vector<uint32_t> _adv_speed = {}, adv_speed = {};
      switch_enum_t autoneg = {};
      attr.v_get(_adv_speed);
      status =
          switch_store::v_get(object_id, SWITCH_PORT_ATTR_AUTONEG, autoneg);
      if (status != SWITCH_STATUS_SUCCESS) break;
      status = switch_store::v_get(object_id, attr_id, adv_speed);

      if (status != SWITCH_STATUS_SUCCESS) break;
      update_pal = (static_cast<switch_port_attr_autoneg>(autoneg.enumdata) ==
                        SWITCH_PORT_ATTR_AUTONEG_ENABLED &&
                    adv_speed != _adv_speed);
    } break;
    case SWITCH_PORT_ATTR_ADVERTISED_FEC_TYPE: {
      std::vector<switch_enum_t> _adv_fec_type = {}, adv_fec_type = {};
      switch_enum_t autoneg = {};
      attr.v_get(_adv_fec_type);
      status =
          switch_store::v_get(object_id, SWITCH_PORT_ATTR_AUTONEG, autoneg);
      if (status != SWITCH_STATUS_SUCCESS) break;

      status = switch_store::v_get(object_id, attr_id, adv_fec_type);
      if (status != SWITCH_STATUS_SUCCESS) break;

      update_pal = (static_cast<switch_port_attr_autoneg>(autoneg.enumdata) ==
                        SWITCH_PORT_ATTR_AUTONEG_ENABLED &&
                    adv_fec_type != _adv_fec_type);
    } break;
    case SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE:
    case SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE:
      return update_bind_point_flag(object_id, attr);
    default:
      update_pal = true;
      break;
  }

  status =
      switch_store::v_set(object_id, SWITCH_PORT_ATTR_UPDATE_PAL, update_pal);
  return status;
}
switch_status_t after_port_update(const switch_object_id_t object_id,
                                  const attr_w &attr) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bool update_pal = false;

  if (attr.id_get() == SWITCH_PORT_ATTR_PORT_VLAN_ID)
    return SWITCH_STATUS_SUCCESS;

  status =
      switch_store::v_get(object_id, SWITCH_PORT_ATTR_UPDATE_PAL, update_pal);
  if (update_pal == false) return status;

  return port_attribute_set(object_id, attr);
}

static switch_status_t create_default_ppg(uint32_t device,
                                          bf_dev_port_t dev_port,
                                          switch_object_id_t device_hdl,
                                          switch_object_id_t object_id,
                                          std::set<attr_w> &def_ppg_attrs) {
  p4_pd_tm_ppg_t pd_hdl;
  // Create default PPG handle
  switch_status_t status = p4_pd_tm_get_default_ppg(device, dev_port, &pd_hdl);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: p4_pd_tm_get_default_ppg failure for port handle {} "
               "dev_port {} status {}",
               __func__,
               __LINE__,
               object_id,
               dev_port,
               status);
    return status;
  }

  auto dev_ret = def_ppg_attrs.insert(
      attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_DEVICE, device_hdl));
  CHECK_RET(dev_ret.second == false, SWITCH_STATUS_FAILURE);

  auto port_ret = def_ppg_attrs.insert(
      attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE, object_id));
  CHECK_RET(port_ret.second == false, SWITCH_STATUS_FAILURE);

  auto index_ret = def_ppg_attrs.insert(
      attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX,
             static_cast<uint8_t>(SWITCH_DEFAULT_PPG_INDEX)));
  CHECK_RET(index_ret.second == false, SWITCH_STATUS_FAILURE);

  auto insert_ret = def_ppg_attrs.insert(
      attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PD_HDL, pd_hdl));
  CHECK_RET(insert_ret.second == false, SWITCH_STATUS_FAILURE);

  return status;
}

switch_status_t after_port_create(const switch_object_id_t object_id,
                                  const std::set<attr_w> &attrs) {
  (void)attrs;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  switch_object_id_t device_hdl = {0}, ppg_handle = {0};
  uint16_t device = 0;
  bf_dev_id_t bf_dev_id = 0;
  uint16_t dev_port = 0;
  uint64_t port_id = 0;
  bf_dev_port_t bf_dev_port = 0;
  uint32_t port_speed;
  bool cut_thru = false;
  std::vector<uint32_t> lane_list;
  switch_enum_t port_type = {0};
  std::set<smi::attr_w> serdes_attrs;
  attr_w port_lane_list(SWITCH_PORT_ATTR_LANE_LIST);

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_DEVICE, device_hdl);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get device_hdl for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }

  status |=
      switch_store::v_get(device_hdl, SWITCH_DEVICE_ATTR_CUT_THROUGH, cut_thru);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get cut_thru mode for device {} port "
               "handle {} status {}",
               __func__,
               __LINE__,
               device_hdl,
               object_id,
               status);
    return status;
  }

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_PORT_ID, port_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get port_id for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }

  status |=
      switch_store::v_get(object_id, SWITCH_PORT_ATTR_LANE_LIST, lane_list);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get lane_list for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_TYPE, port_type);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get port_type for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }

  port_lane_list.v_set(lane_list);
  status = switch_store::attribute_set(object_id, port_lane_list);
  if (status != SWITCH_STATUS_SUCCESS) {
    // switch_log(SWITCH_API_LEVEL_ERROR,
    //            SWITCH_OBJECT_TYPE_PORT,
    //            "{}.{}: lane_list failure port_handle {} speed {} status {}",
    //            __func__,
    //            __LINE__,
    //            object_id,
    //            port_lane_list,
    //            status);
  }

  status |= switch_store::v_get(device_hdl, SWITCH_DEVICE_ATTR_DEV_ID, device);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}.{}: failure to get dev_id for device {} port handle {} status {}",
        __func__,
        __LINE__,
        device_hdl,
        object_id,
        status);
    return status;
  }
  bf_dev_id = static_cast<bf_dev_id_t>(device);

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get dev_port for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }
  bf_dev_port = static_cast<bf_dev_port_t>(dev_port);

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_SPEED, port_speed);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to get speed for port handle {} status {}",
               __func__,
               __LINE__,
               object_id,
               status);
    return status;
  }

  if (port_speed == 0) return SWITCH_STATUS_INVALID_PARAMETER;

  if (feature::is_feature_set(SWITCH_FEATURE_PKTGEN) &&
      port_type.enumdata == SWITCH_PORT_ATTR_TYPE_RECIRC) {
    /* note currently all recirc ports with bmai objects are pktgen-enabled */
    /* XXX warm-reconfig? */
    bf_status = bf_pktgen_enable(
        get_bf_rt_session().sessHandleGet(), bf_dev_id, bf_dev_port);
    status = pal_status_xlate(bf_status);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OBJECT_TYPE_PORT,
          "{}.{}: pktgen enable failed port_handle {} dev port {} status {}",
          __func__,
          __LINE__,
          object_id,
          bf_dev_port,
          status);
    }
  }

  // nothing to do for recirc ports
  if (port_type.enumdata == SWITCH_PORT_ATTR_TYPE_RECIRC) return status;

  // port handle never created for recirc ports so don't worry about them here
  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT) {
    // For CPU port, if it is PCIE, then don't do port_add
    status = SWITCH_STATUS_SUCCESS;
  } else if (port_id == SWITCH_CPU_PORT_ETH_DEFAULT) {
    // For CPU port, if it is Eth port, then do port_add
    // Note that every other operation after this is skipped except for
    // port_enable for eth cpu port
    status = port_add(
        bf_dev_id, bf_dev_port, 10000, SWITCH_PORT_ATTR_FEC_TYPE_NONE, 1);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port_add failure for eth cpu port {} status {}",
                 __func__,
                 __LINE__,
                 object_id,
                 status);
      return status;
    }
    bf_pm_port_autoneg_policy_e an = PM_AN_FORCE_ENABLE;
    bf_status = bf_pal_port_autoneg_policy_set(bf_dev_id, bf_dev_port, an);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: eth cpu port autoneg policy set failure device {} "
                 "dev_port {} "
                 "status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      return pal_status_xlate(bf_status);
    }
  } else {
    switch_enum_t _fec = {};
    switch_enum_t _default_fec = {};
    switch_port_attr_fec_type fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE;
    attr_w fec_attr(SWITCH_PORT_ATTR_FEC_TYPE);
    status = switch_store::attribute_get(
        object_id, SWITCH_PORT_ATTR_FEC_TYPE, fec_attr);
    fec_attr.v_get(_fec);
    _default_fec = get_default_fec_by_speed(_fec, port_speed, lane_list.size());
    // Update fec type in the switch store
    if (_default_fec.enumdata != _fec.enumdata) {
      _fec.enumdata = _default_fec.enumdata;
      fec_attr.v_set(_fec);
      status = switch_store::attribute_set(object_id, fec_attr);
      if (status != SWITCH_STATUS_SUCCESS) {
        switch_log(SWITCH_API_LEVEL_ERROR,
                   SWITCH_OBJECT_TYPE_PORT,
                   "{}.{}: fec_attr set failure object_id {} status {}",
                   __func__,
                   __LINE__,
                   object_id,
                   status);
        return status;
      }
    }
    fec = static_cast<switch_port_attr_fec_type>(_fec.enumdata);
    status =
        port_add(bf_dev_id, bf_dev_port, port_speed, fec, lane_list.size());
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port_add failure for port handle {} status {}",
                 __func__,
                 __LINE__,
                 object_id,
                 status);
      return status;
    }

    status = update_supported_fec_types(
        bf_dev_id, bf_dev_port, object_id, port_speed, lane_list.size());
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: update supported fec types "
                 "failure for device {} dev_port "
                 "{} object_id {} status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 object_id,
                 status);
      return status;
    }
  }

  status = supported_speeds_set(object_id, bf_dev_id);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: supported_speeds_set failure for port handle {} "
               "port_id {} status {}",
               __func__,
               __LINE__,
               object_id,
               port_id,
               status);
    return status;
  }

  std::vector<attr_w> update_attrs;
  attr_w rx_mtu_attr(SWITCH_PORT_ATTR_RX_MTU);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_RX_MTU, rx_mtu_attr);
  update_attrs.push_back(rx_mtu_attr);

  attr_w tx_mtu_attr(SWITCH_PORT_ATTR_TX_MTU);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_TX_MTU, tx_mtu_attr);
  update_attrs.push_back(tx_mtu_attr);

  attr_w fec_attr(SWITCH_PORT_ATTR_FEC_TYPE);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_FEC_TYPE, fec_attr);
  update_attrs.push_back(fec_attr);

  attr_w admin_state_attr(SWITCH_PORT_ATTR_ADMIN_STATE);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state_attr);
  update_attrs.push_back(admin_state_attr);

  attr_w autoneg_attr(SWITCH_PORT_ATTR_AUTONEG);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_AUTONEG, autoneg_attr);
  update_attrs.push_back(autoneg_attr);

  attr_w link_pause_attr(SWITCH_PORT_ATTR_LINK_PAUSE);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_LINK_PAUSE, link_pause_attr);
  update_attrs.push_back(link_pause_attr);

  attr_w loopback_attr(SWITCH_PORT_ATTR_LOOPBACK_MODE);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_LOOPBACK_MODE, loopback_attr);
  update_attrs.push_back(loopback_attr);

  attr_w media_type_attr(SWITCH_PORT_ATTR_MEDIA_TYPE);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_MEDIA_TYPE, media_type_attr);
  update_attrs.push_back(media_type_attr);

  attr_w tx_enable_attr(SWITCH_PORT_ATTR_PKT_TX_ENABLE);
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_PKT_TX_ENABLE, tx_enable_attr);
  update_attrs.push_back(tx_enable_attr);

  for (const auto &update_attr : update_attrs) {
    status = port_attribute_set(object_id, update_attr);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: port_attribute_set failure for port handle {} "
                 "attribute {} status {}",
                 __func__,
                 __LINE__,
                 object_id,
                 update_attr.getattr(),
                 status);
      return status;
    }
  }

  SWITCH_CONTEXT.add_dev_port_to_port_handle(dev_port, object_id);
  if (port_type.enumdata == SWITCH_PORT_ATTR_TYPE_NORMAL) {
    if (cut_thru) {
      status = bf_pal_port_cut_through_enable(device, dev_port);
    } else {
      status = bf_pal_port_cut_through_disable(device, dev_port);
    }
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: cut_thru set failure for port handle {} status {}",
                 __func__,
                 __LINE__,
                 object_id,
                 status);
      return status;
    }
  }

  bool is_internal = false;
  status = switch_store::v_get(
      object_id, SWITCH_PORT_ATTR_INTERNAL_OBJECT, is_internal);
  CHECK_RET(status != SWITCH_STATUS_SUCCESS, SWITCH_STATUS_SUCCESS);

  bool bmai_managed_internal_ports =
      SWITCH_CONTEXT.is_internal_pipe_ports_bmai_managed();
  if (bmai_managed_internal_ports && is_internal) {
    status |= pal_status_xlate(bf_pal_port_disable(bf_dev_id, bf_dev_port));
    status |= pal_status_xlate(bf_port_ifg_set(
        bf_dev_id, bf_dev_port, FOLDED_PIPELINE_INTERNAL_PIPE_PORT_IFG, false));
    status |= pal_status_xlate(bf_port_preamble_length_set(
        bf_dev_id,
        bf_dev_port,
        FOLDED_PIPELINE_INTERNAL_PIPE_PORT_PREAMBLE_LEN));
    status |= pal_status_xlate(bf_pal_port_enable(bf_dev_id, bf_dev_port));
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}:{}: (Asymmetric Folded Pipeline) Failed to configure ifg "
                 "and preamble len for internal "
                 "port {} (dev_port:{}) on pipe {}",
                 __func__,
                 __LINE__,
                 port_id,
                 bf_dev_port,
                 status);
      return status;
    }
  }

  if (switch_store::smiContext::context().in_warm_init()) return status;

  if (port_type.enumdata != SWITCH_PORT_ATTR_TYPE_RECIRC) {
    // Non default ppg indexing range - 0 to #non_default_ppgs-1
    // default ppg index = 255
    uint32_t non_def_ppgs = 0;
    status |= switch_store::v_get(
        object_id, SWITCH_PORT_ATTR_NON_DEF_PPGS, non_def_ppgs);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OBJECT_TYPE_PORT,
          "{}.{}: failure to get non_def_ppg for port handle {} status {}",
          __func__,
          __LINE__,
          object_id,
          status);
      return status;
    }

    std::set<attr_w> def_ppg_attrs;
    status = create_default_ppg(
        device, dev_port, device_hdl, object_id, def_ppg_attrs);
    if (status != SWITCH_STATUS_SUCCESS) return status;

    // Default ppg is not user created and exists in hardware on port creation
    const auto hw_ret = def_ppg_attrs.insert(
        attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_CREATED_IN_HW, true));
    CHECK_RET(hw_ret.second == false, SWITCH_STATUS_FAILURE);

    status |= switch_store::object_create(
        SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP, def_ppg_attrs, ppg_handle);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to create PPG for port handle {} status {}",
                 __func__,
                 __LINE__,
                 object_id,
                 status);
      return status;
    }
    status |= switch_store::v_set(
        ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_INTERNAL_OBJECT, true);
    status |= switch_store::v_set(
        object_id, SWITCH_PORT_ATTR_DEFAULT_PPG, ppg_handle.data);
    if (status != SWITCH_STATUS_SUCCESS) return status;

    if (non_def_ppgs) {
      // All non-default PPG indices start with 1.
      for (uint8_t index = 1; index <= non_def_ppgs; index++) {
        std::set<attr_w> non_def_ppg_attrs;

        auto dev_ret = non_def_ppg_attrs.insert(
            attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_DEVICE, device_hdl));
        CHECK_RET(dev_ret.second == false, SWITCH_STATUS_FAILURE);

        auto port_ret = non_def_ppg_attrs.insert(
            attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PORT_HANDLE, object_id));
        CHECK_RET(port_ret.second == false, SWITCH_STATUS_FAILURE);

        auto insert_ret = non_def_ppg_attrs.insert(
            attr_w(SWITCH_PORT_PRIORITY_GROUP_ATTR_PPG_INDEX, index));
        CHECK_RET(insert_ret.second == false, SWITCH_STATUS_FAILURE);

        status |=
            switch_store::object_create(SWITCH_OBJECT_TYPE_PORT_PRIORITY_GROUP,
                                        non_def_ppg_attrs,
                                        ppg_handle);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OBJECT_TYPE_PORT,
                     "{}.{}: failure to create non_def PPG for port handle {} "
                     "status {}",
                     __func__,
                     __LINE__,
                     object_id,
                     status);
          return status;
        }
        status |= switch_store::v_set(
            ppg_handle, SWITCH_PORT_PRIORITY_GROUP_ATTR_INTERNAL_OBJECT, true);
      }
    }
  }

  uint8_t max_queue = 0;
  if (port_id == SWITCH_CPU_PORT_PCIE_DEFAULT ||
      port_id == SWITCH_CPU_PORT_ETH_DEFAULT) {
    status |= switch_store::v_get(
        device_hdl, SWITCH_DEVICE_ATTR_MAX_CPU_QUEUE, max_queue);
  } else {
    status |= switch_store::v_get(
        device_hdl, SWITCH_DEVICE_ATTR_MAX_PORT_QUEUE, max_queue);
  }

  /* create default schduler group type port */
  switch_object_id_t sch_grp_handle = {0};
  std::set<attr_w> sch_group_attrs;
  switch_enum_t grp_type = {.enumdata = SWITCH_SCHEDULER_GROUP_ATTR_TYPE_PORT};
  sch_group_attrs.insert(
      attr_w(SWITCH_SCHEDULER_GROUP_ATTR_DEVICE, device_hdl));
  sch_group_attrs.insert(attr_w(SWITCH_SCHEDULER_GROUP_ATTR_TYPE, grp_type));
  sch_group_attrs.insert(
      attr_w(SWITCH_SCHEDULER_GROUP_ATTR_PORT_HANDLE, object_id));

  status |= switch_store::object_create(
      SWITCH_OBJECT_TYPE_SCHEDULER_GROUP, sch_group_attrs, sch_grp_handle);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure to create scheduler_group {} for port handle {} "
               "status {}",
               __func__,
               __LINE__,
               sch_grp_handle,
               object_id,
               status);
    return status;
  }
  status |= switch_store::v_set(
      sch_grp_handle, SWITCH_SCHEDULER_GROUP_ATTR_INTERNAL_OBJECT, true);

  for (uint8_t i = 0; i < max_queue; i++) {
    std::set<attr_w> queue_attrs;
    switch_object_id_t queue_handle = {0};
    queue_attrs.insert(attr_w(SWITCH_QUEUE_ATTR_DEVICE, device_hdl));
    queue_attrs.insert(attr_w(SWITCH_QUEUE_ATTR_PORT_HANDLE, object_id));
    queue_attrs.insert(attr_w(SWITCH_QUEUE_ATTR_QUEUE_ID, i));
    status |= switch_store::object_create(
        SWITCH_OBJECT_TYPE_QUEUE, queue_attrs, queue_handle);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OBJECT_TYPE_PORT,
          "{}.{}: failure to create queue {} for port handle {} status {}",
          __func__,
          __LINE__,
          i,
          object_id,
          status);
      return status;
    }
    status |= switch_store::v_set(
        queue_handle, SWITCH_QUEUE_ATTR_INTERNAL_OBJECT, true);

    /* create queue schduler group */
    switch_object_id_t sch_grp_queue_handle = {0};
    std::set<attr_w> sch_group_queue_attrs;
    grp_type.enumdata = SWITCH_SCHEDULER_GROUP_ATTR_TYPE_QUEUE;
    sch_group_queue_attrs.insert(
        attr_w(SWITCH_SCHEDULER_GROUP_ATTR_DEVICE, device_hdl));
    sch_group_queue_attrs.insert(
        attr_w(SWITCH_SCHEDULER_GROUP_ATTR_TYPE, grp_type));
    sch_group_queue_attrs.insert(
        attr_w(SWITCH_SCHEDULER_GROUP_ATTR_PORT_HANDLE, object_id));
    sch_group_queue_attrs.insert(
        attr_w(SWITCH_SCHEDULER_GROUP_ATTR_QUEUE_HANDLE, queue_handle));
    status |= switch_store::object_create(SWITCH_OBJECT_TYPE_SCHEDULER_GROUP,
                                          sch_group_queue_attrs,
                                          sch_grp_queue_handle);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to create scheduler_group {} for port handle "
                 "{} queue_handle {} status {}",
                 __func__,
                 __LINE__,
                 i,
                 object_id,
                 queue_handle,
                 status);
      return status;
    }
    status |= switch_store::v_set(sch_grp_queue_handle,
                                  SWITCH_SCHEDULER_GROUP_ATTR_INTERNAL_OBJECT,
                                  true);
  }

  smi::event::port_event_notify(SWITCH_PORT_EVENT_ADD, object_id);

  return status;
}

switch_status_t before_port_delete(const switch_object_id_t object_id) {
  bf_status_t bf_status;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  switch_object_id_t device_hdl;
  uint64_t port_id = 0;
  uint16_t device = 0;
  bf_dev_id_t bf_dev_id = 0;
  uint16_t dev_port = 0;
  bf_dev_port_t bf_dev_port = 0;
  switch_object_id_t clear = {0};
  switch_enum_t port_type = {};
  std::set<switch_object_id_t> serdes;

  status |= switch_store::referencing_set_get(
      object_id, SWITCH_OBJECT_TYPE_PORT_SERDES, serdes);
  for (auto serdes_hdl : serdes) {
    status |= switch_store::object_delete(serdes_hdl);
  }

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_DEVICE, device_hdl);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status |= switch_store::v_get(device_hdl, SWITCH_DEVICE_ATTR_DEV_ID, device);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  bf_dev_id = static_cast<bf_dev_id_t>(device);

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_TYPE, port_type);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_PORT_ID, port_id);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  if (feature::is_feature_set(SWITCH_FEATURE_PKTGEN) &&
      port_type.enumdata == SWITCH_PORT_ATTR_TYPE_RECIRC) {
    /* XXX warm-reconfig? */
    bf_status = bf_pktgen_disable(
        get_bf_rt_session().sessHandleGet(), bf_dev_id, bf_dev_port);
    status = pal_status_xlate(bf_status);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OBJECT_TYPE_PORT,
          "{}.{}: pktgen enable failed port_handle {} dev port {} status {}",
          __func__,
          __LINE__,
          object_id,
          bf_dev_port,
          status);
    }
  }

  // nothing to do for recirc ports
  if (port_type.enumdata == SWITCH_PORT_ATTR_TYPE_RECIRC) return status;

  status |= switch_store::v_get(object_id, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  bf_dev_port = static_cast<bf_dev_port_t>(dev_port);

  status = port_del(bf_dev_id, bf_dev_port);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  SWITCH_CONTEXT.release_yid(object_id);
  SWITCH_CONTEXT.clear_dev_port_to_port_handle(dev_port);

  status =
      switch_store::v_set(object_id, SWITCH_PORT_ATTR_DEFAULT_PPG, clear.data);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  // Clear port rate stats from SW
  uint32_t max_ports = 0;
  status |=
      switch_store::v_get(device_hdl, SWITCH_DEVICE_ATTR_MAX_PORTS, max_ports);
  if (port_id < max_ports) {
    SWITCH_CONTEXT.port_rate_reset(device, port_id);
  }

  // delete ppgs.
  attr_w ppg_list(SWITCH_PORT_ATTR_PORT_PRIORITY_GROUPS);
  std::vector<switch_object_id_t> ppg_handles;
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_PORT_PRIORITY_GROUPS, ppg_list);
  ppg_list.v_get(ppg_handles);
  for (auto ppg_handle : ppg_handles) {
    status = switch_store::object_delete(ppg_handle);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to delete ppg_handle {} for port handle "
                 "{} status {}",
                 __func__,
                 __LINE__,
                 ppg_handle,
                 object_id,
                 status);
      return status;
    }
  }

  // delete scheduler group
  attr_w g_list(SWITCH_PORT_ATTR_PORT_QUEUE_SCHEDULER_GROUP_HANDLES);
  std::vector<switch_object_id_t> g_oid_handles;
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_PORT_QUEUE_SCHEDULER_GROUP_HANDLES, g_list);
  g_list.v_get(g_oid_handles);
  for (auto g_oid : g_oid_handles) {
    status = switch_store::object_delete(g_oid);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failure to delete scheduler_group {} for port handle "
                 "{} status {}",
                 __func__,
                 __LINE__,
                 g_oid,
                 object_id,
                 status);
      return status;
    }
  }

  // delete queues
  attr_w queue_list(SWITCH_PORT_ATTR_QUEUE_HANDLES);
  std::vector<switch_object_id_t> queue_handles;
  status = switch_store::attribute_get(
      object_id, SWITCH_PORT_ATTR_QUEUE_HANDLES, queue_list);
  queue_list.v_get(queue_handles);
  for (auto oid : queue_handles) {
    status = switch_store::object_delete(oid);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(
          SWITCH_API_LEVEL_ERROR,
          SWITCH_OBJECT_TYPE_PORT,
          "{}.{}: failure to delete queue {} for port handle {} status {}",
          __func__,
          __LINE__,
          oid,
          object_id,
          status);
      return status;
    }
  }

  smi::event::port_event_notify(SWITCH_PORT_EVENT_DELETE, object_id);

  return status;
}

bf_status_t switch_port_state_change_cb(int device,
                                        int device_port,
                                        bool up,
                                        void *cookie) {
  (void)cookie;
  switch_object_id_t port_handle = {0}, device_handle = {0};
  switch_port_oper_status_event_t oper_status = SWITCH_PORT_OPER_STATUS_UNKNOWN;
  switch_port_attr_oper_state oper_state = SWITCH_PORT_ATTR_OPER_STATE_UNKNOWN;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  uint16_t dev_port = static_cast<uint16_t>(device_port);
  uint16_t dev_id = static_cast<uint16_t>(device);
  uint32_t oper_speed = 0;
  bool autoneg_status = false;

  // Get device handle
  std::set<attr_w> dev_attrs;
  dev_attrs.insert(attr_w(SWITCH_DEVICE_ATTR_DEV_ID, dev_id));
  status = switch_store::object_id_get_wkey(
      SWITCH_OBJECT_TYPE_DEVICE, dev_attrs, device_handle);

  port_handle = SWITCH_CONTEXT.find_dev_port_to_port_handle(dev_port);
  if (port_handle.data == 0) {
    switch_log(SWITCH_API_LEVEL_DETAIL,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: port state {} change notification failed on device {} "
               "dev port {}: dev port to port handle get failed\n",
               __func__,
               __LINE__,
               up,
               dev_id,
               dev_port);
    return status;
  }
  status |= switch_store::v_get(
      port_handle, SWITCH_PORT_ATTR_AUTONEG_STATUS, autoneg_status);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  bool sw_model = false;
  bf_pal_pltfm_type_get(0, &sw_model);
  if (sw_model) {
    bool admin_state = false;
    switch_store::v_get(port_handle, SWITCH_PORT_ATTR_ADMIN_STATE, admin_state);
    if (admin_state) up = true;
  }
  if (up) {
    oper_status = SWITCH_PORT_OPER_STATUS_UP;
    oper_state = SWITCH_PORT_ATTR_OPER_STATE_UP;

    switch_enum_t enum_autoneg = {0};
    status |= switch_store::v_get(
        port_handle, SWITCH_PORT_ATTR_AUTONEG, enum_autoneg);
    if (status != SWITCH_STATUS_SUCCESS) return status;
    switch_port_attr_autoneg autoneg;
    autoneg = static_cast<switch_port_attr_autoneg>(enum_autoneg.enumdata);

    if (autoneg == SWITCH_PORT_ATTR_AUTONEG_ENABLED) {
      bf_port_speed_t hcd_speed;
      int hcd_lanes = 0;
      bf_fec_type_t hcd_fec;
      bf_status_t bf_status = bf_pal_port_autoneg_info_get(
          dev_id, dev_port, &hcd_speed, &hcd_lanes, &hcd_fec);
      switch_log(SWITCH_API_LEVEL_INFO,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}: status {}, HCD speed {}, HCD lanes {}, HCD FEC {}",
                 __func__,
                 bf_status,
                 hcd_speed,
                 hcd_lanes,
                 hcd_fec);
      if (bf_status != BF_SUCCESS) return pal_status_xlate(bf_status);

      oper_speed = pal_port_speed_to_switch_port_speed(hcd_speed);
      if (!autoneg_status) {
        autoneg_status = true;
        status |= switch_store::v_set(
            port_handle, SWITCH_PORT_ATTR_AUTONEG_STATUS, autoneg_status);
        if (status != SWITCH_STATUS_SUCCESS) return status;
      }
    } else {
      uint32_t speed = 0;
      status |= switch_store::v_get(port_handle, SWITCH_PORT_ATTR_SPEED, speed);
      if (status != SWITCH_STATUS_SUCCESS) return status;
      oper_speed = speed;
      if (autoneg_status) {
        autoneg_status = false;
        status |= switch_store::v_set(
            port_handle, SWITCH_PORT_ATTR_AUTONEG_STATUS, autoneg_status);
        if (status != SWITCH_STATUS_SUCCESS) return status;
      }
    }

  } else {
    oper_status = SWITCH_PORT_OPER_STATUS_DOWN;
    oper_state = SWITCH_PORT_ATTR_OPER_STATE_DOWN;
    oper_speed = 0;
    if (autoneg_status) {
      autoneg_status = false;
      status |= switch_store::v_set(
          port_handle, SWITCH_PORT_ATTR_AUTONEG_STATUS, autoneg_status);
      if (status != SWITCH_STATUS_SUCCESS) return status;
    }
  }
  switch_enum_t e = {.enumdata = static_cast<uint64_t>(oper_state)};
  attr_w oper_state_attr(SWITCH_PORT_ATTR_OPER_STATE, e);
  status = switch_store::attribute_set(port_handle, oper_state_attr);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}: oper state attribute set failed status={} for device {} port {}",
        __func__,
        status,
        dev_id,
        port_handle);
    return status;
  }

  attr_w oper_speed_attr(SWITCH_PORT_ATTR_OPER_SPEED, oper_speed);
  status = switch_store::attribute_set(port_handle, oper_speed_attr);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(
        SWITCH_API_LEVEL_ERROR,
        SWITCH_OBJECT_TYPE_PORT,
        "{}: oper speed attribute set failed status={} for device {} port {}",
        __func__,
        status,
        dev_id,
        port_handle);
    return status;
  }
  smi::event::port_status_notify(oper_status, port_handle);

  return status;
}

switch_status_t after_device_create_cache_fp_dev_port_mapping(
    const switch_object_id_t object_id, const std::set<attr_w> &attrs) {
  (void)attrs;
  uint16_t device{};
  uint32_t curr_idx{}, next_idx{};
  uint32_t num_fp_ports{}, fp_counter = 0;
  bf_dev_port_t dp{};
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;

  status = switch_store::v_get(object_id, SWITCH_DEVICE_ATTR_DEV_ID, device);
  if (status != SWITCH_STATUS_SUCCESS) return status;
  bf_dev_id_t bf_dev_id = static_cast<bf_dev_id_t>(device);
  bf_status = bf_pal_num_front_ports_get(bf_dev_id, &num_fp_ports);
  CHECK_RET(bf_status != BF_SUCCESS, pal_status_xlate(status));
  bf_status = bf_pal_fp_idx_get_first(bf_dev_id, &curr_idx);
  CHECK_RET(bf_status != BF_SUCCESS, pal_status_xlate(status));
  bf_status = bf_pal_fp_idx_to_dev_port_map(bf_dev_id, curr_idx, &dp);
  CHECK_RET(bf_status != BF_SUCCESS, pal_status_xlate(status));
  SWITCH_CONTEXT.add_fp_idx_to_dev_port(curr_idx, dp);

  while (bf_pal_fp_idx_get_next(bf_dev_id, curr_idx, &next_idx) == BF_SUCCESS) {
    bf_status = bf_pal_fp_idx_to_dev_port_map(bf_dev_id, next_idx, &dp);
    CHECK_RET(bf_status != BF_SUCCESS, pal_status_xlate(status));
    SWITCH_CONTEXT.add_fp_idx_to_dev_port(next_idx, dp);
    curr_idx = next_idx;
    fp_counter++;
    // Just a fail safe check so that we never end up with an infinite loop
    if (fp_counter == num_fp_ports) break;
  }
  return SWITCH_STATUS_SUCCESS;
}

void switch_ports_oper_status_update(
    std::vector<switch_port_oper_status_event_data_t> &status_change_events) {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  bf_status_t bf_status = BF_SUCCESS;
  std::vector<switch_object_id_t> port_handle_list;

  status = switch_store::object_get_all_handles(SWITCH_OBJECT_TYPE_PORT,
                                                port_handle_list);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_WARN,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failed to get port handles, status {}",
               __func__,
               __LINE__,
               status);
    return;
  }

  for (auto port_handle : port_handle_list) {
    bool up = false;
    uint16_t dev_port = 0;
    bf_dev_port_t bf_dev_port = 0;
    bf_dev_id_t bf_dev_id = 0;
    switch_enum_t type = {};
    attr_w port_type(SWITCH_PORT_ATTR_TYPE);

    status = switch_store::attribute_get(
        port_handle, SWITCH_PORT_ATTR_TYPE, port_type);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failed to get type for port handle {} status {}",
                 __func__,
                 __LINE__,
                 port_handle,
                 status);
      continue;
    }
    port_type.v_get(type);

    if (type.enumdata != SWITCH_PORT_ATTR_TYPE_NORMAL) continue;

    status =
        switch_store::v_get(port_handle, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failed to get dev_port for port handle {} status {}",
                 __func__,
                 __LINE__,
                 port_handle,
                 status);
      continue;
    }
    bf_dev_port = static_cast<bf_dev_port_t>(dev_port);

    bf_status = bf_pal_port_oper_state_get(bf_dev_id, bf_dev_port, &up);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}.{}: failed to get oper state for device {} "
                 "dev_port {} "
                 "status {}",
                 __func__,
                 __LINE__,
                 bf_dev_id,
                 bf_dev_port,
                 pal_status_xlate(bf_status));
      continue;
    }

    switch_port_attr_oper_state curr_oper_state =
        up ? SWITCH_PORT_ATTR_OPER_STATE_UP : SWITCH_PORT_ATTR_OPER_STATE_DOWN;
    switch_enum_t stored_oper_state = {};
    attr_w attr(SWITCH_PORT_ATTR_OPER_STATE);
    status = switch_store::attribute_get(
        port_handle, SWITCH_PORT_ATTR_OPER_STATE, attr);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}: oper state get failed for port {} status={}",
                 __func__,
                 status,
                 port_handle);
      continue;
    }
    attr.v_get(stored_oper_state);

    if (curr_oper_state == stored_oper_state.enumdata) {
      continue;
    }

    stored_oper_state.enumdata = curr_oper_state;
    attr_w store_attr{SWITCH_PORT_ATTR_OPER_STATE, stored_oper_state};
    status = switch_store::attribute_set(port_handle, store_attr);
    if (status != SWITCH_STATUS_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}: oper state set failed for port {} status={}",
                 __func__,
                 status,
                 port_handle);
      continue;
    }

    switch_port_oper_status_event_t oper_status_event =
        up ? SWITCH_PORT_OPER_STATUS_UP : SWITCH_PORT_OPER_STATUS_DOWN;
    status_change_events.push_back({oper_status_event, port_handle});
  }
}

switch_status_t after_device_create(const switch_object_id_t object_id,
                                    const std::set<attr_w> &attrs) {
  (void)attrs;
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  uint16_t device = 0;
  bf_dev_port_t dev_port = 0;
  uint32_t max_ports = 0, curr_pipe = 0, max_pipes = 0, index = 0,
           num_ports = 0;
  uint32_t start_recirc_index = 0, end_recirc_index = 0;
  uint32_t recirc_port_list[SWITCH_MAX_RECIRC_PORTS] = {};
  uint16_t recirc_dev_port_list[SWITCH_MAX_RECIRC_PORTS] = {};
  bf_status_t bf_status = BF_SUCCESS;
  uint16_t cpu_dev_port = 0;

  status |= switch_store::v_get(object_id, SWITCH_DEVICE_ATTR_DEV_ID, device);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  // Set max ports
  bf_status = bf_pm_num_front_ports_get(device, &max_ports);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}: max ports get failed status={} for device {}",
               __func__,
               pal_status_xlate(bf_status),
               object_id);
    return SWITCH_STATUS_FAILURE;
  }
  status |=
      switch_store::v_set(object_id, SWITCH_DEVICE_ATTR_MAX_PORTS, max_ports);

  // Set max pipes
  bf_status = bf_pal_num_pipes_get(device, &max_pipes);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}: max pipes get failed status={} for device {}",
               __func__,
               pal_status_xlate(bf_status),
               device);
    return SWITCH_STATUS_FAILURE;
  }
  status |= switch_store::v_set(
      object_id, SWITCH_DEVICE_ATTR_MAX_PIPES, static_cast<uint8_t>(max_pipes));

  bf_status = bf_pal_recirc_port_range_get(
      device, &start_recirc_index, &end_recirc_index);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}: recirc port range get failed status={} for device {}",
               __func__,
               pal_status_xlate(bf_status),
               device);
    return SWITCH_STATUS_FAILURE;
  }

  // BF PAL layer sends all 4 recirc port indices per pipe. We populate only
  // one per pipe
  if (bf_lld_dev_is_tof1(device)) {
    // for tofino, the recirc port range is 16 indices long with each index
    // being a 25G channel
    for (index = start_recirc_index;
         index <= end_recirc_index && curr_pipe < max_pipes;
         index += 4) {
      // only add recirc for valid pipes
      bool pipe_valid = false;
      for (auto pipe : get_active_pipes()) {
        if (curr_pipe == pipe) pipe_valid = true;
      }
      if (!pipe_valid) {
        curr_pipe++;
        continue;
      }

      // we choose index 0 in each 4 index range and combine 4x25G
      // channels into a single 100G recirc port per pipe
      recirc_port_list[num_ports] = index;
      num_ports++;
      curr_pipe++;
    }
  } else {
    // for tofino2, the recirc port range is 32 indices long with each
    // index being a 50G channel
    for (index = start_recirc_index;
         index <= end_recirc_index && curr_pipe < max_pipes;
         index += 8) {
      // only add recirc for valid pipes
      bool pipe_valid = false;
      for (auto pipe : get_active_pipes()) {
        if (curr_pipe == pipe) pipe_valid = true;
      }
      if (!pipe_valid) {
        curr_pipe++;
        continue;
      }

      // we choose the last 2 indices in each 8 index range and combine 2x50G
      // channels into a single 100G recirc port per pipe
      recirc_port_list[num_ports] = index + 6;
      num_ports++;
      curr_pipe++;
    }
  }

  for (uint32_t i = 0; i < num_ports; i++) {
    bf_status =
        recirc_port_to_dev_port_map(device, recirc_port_list[i], &dev_port);
    if (bf_status != BF_SUCCESS) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}: recirc port to dev port map failed status={} for device "
                 "{} index {}",
                 __func__,
                 pal_status_xlate(bf_status),
                 device,
                 index);
      return SWITCH_STATUS_FAILURE;
    }
    recirc_dev_port_list[i] = dev_port;
  }

  // One Recirc port per pipe.
  status |= switch_store::v_set(
      object_id, SWITCH_DEVICE_ATTR_MAX_RECIRC_PORTS, num_ports);

  attr_w port_list(SWITCH_DEVICE_ATTR_RECIRC_PORT_LIST);
  attr_w dev_port_list(SWITCH_DEVICE_ATTR_RECIRC_DEV_PORT_LIST);
  std::vector<uint16_t> list1;
  std::vector<uint16_t> list2;
  for (index = 0; index < num_ports; index++) {
    list1.push_back(static_cast<uint16_t>(recirc_port_list[index]));
    list2.push_back(recirc_dev_port_list[index]);
  }

  port_list.v_set(list1);
  status |= switch_store::attribute_set(object_id, port_list);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}: recirc port list set failed status={} for device {}",
               __func__,
               status,
               device);
    return status;
  }

  dev_port_list.v_set(list2);
  status |= switch_store::attribute_set(object_id, dev_port_list);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}: recirc dev port list set failed status={} for device {}",
               __func__,
               status,
               device);
    return status;
  }

  cpu_dev_port = p4_devport_mgr_pcie_cpu_port_get(device);
  status = switch_store::v_set(
      object_id, SWITCH_DEVICE_ATTR_PCIE_CPU_DEV_PORT, cpu_dev_port);
  cpu_dev_port = p4_devport_mgr_eth_cpu_port_get(device);
  status |= switch_store::v_set(
      object_id, SWITCH_DEVICE_ATTR_ETH_CPU_DEV_PORT, cpu_dev_port);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: failure device status={} for device {}",
               __func__,
               __LINE__,
               status,
               device);
    return status;
  }

  attr_w max_port_mtu(SWITCH_DEVICE_ATTR_MAX_PORT_MTU);
  uint32_t max_mtu = 0;
  bf_dev_id_t bf_dev_id = static_cast<bf_dev_id_t>(device);
  bf_status = bf_pal_port_max_mtu_get(bf_dev_id, &max_mtu);
  if (bf_status != BF_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}: failed to get max port mtu status={} for device "
               "{}",
               __func__,
               pal_status_xlate(bf_status),
               bf_dev_id);
    return SWITCH_STATUS_FAILURE;
  }
  status |= switch_store::v_set(object_id,
                                SWITCH_DEVICE_ATTR_MAX_PORT_MTU,
                                static_cast<uint16_t>(max_mtu));
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OBJECT_TYPE_PORT,
               "{}.{}: max mtu set failure device status={} for device"
               " {} max mtu {}",
               __func__,
               __LINE__,
               status,
               device,
               max_mtu);
    return status;
  }

  return status;
}

class port_serdes : public auto_object {
 private:
  static const switch_object_type_t auto_ot =
      SWITCH_OBJECT_TYPE_PORT_SERDES_TABLE;
  static const switch_attr_id_t parent_attr_id =
      SWITCH_PORT_SERDES_TABLE_ATTR_PARENT_HANDLE;
  bf_status_t bf_status = BF_SUCCESS;
  bool is_dummy_object = false;
  uint16_t device = 0;
  bf_dev_id_t bf_dev_id = 0;
  switch_object_id_t port_hdl = {}, device_hdl = {};
  uint16_t dev_port = 0;
  bf_dev_port_t bf_dev_port = 0;
  uint64_t port_id = 0;
  std::vector<int64_t> pre1, pre2, main, post1, post2, attn;
  std::vector<uint32_t> lane_list;

 public:
  port_serdes(const switch_object_id_t parent, switch_status_t &status)
      : auto_object(auto_ot, parent_attr_id, parent) {
    status |=
        switch_store::v_get(parent, SWITCH_PORT_SERDES_ATTR_PORT_ID, port_hdl);
    status |=
        switch_store::v_get(port_hdl, SWITCH_PORT_ATTR_DEVICE, device_hdl);
    status |=
        switch_store::v_get(device_hdl, SWITCH_DEVICE_ATTR_DEV_ID, device);
    bf_dev_id = static_cast<bf_dev_id_t>(device);
    status |=
        switch_store::v_get(port_hdl, SWITCH_PORT_ATTR_DEV_PORT, dev_port);
    bf_dev_port = static_cast<bf_dev_port_t>(dev_port);

    status |=
        switch_store::v_get(parent, SWITCH_PORT_SERDES_ATTR_TX_FIR_PRE1, pre1);
    status |=
        switch_store::v_get(parent, SWITCH_PORT_SERDES_ATTR_TX_FIR_PRE2, pre2);
    status |=
        switch_store::v_get(parent, SWITCH_PORT_SERDES_ATTR_TX_FIR_MAIN, main);
    status |= switch_store::v_get(
        parent, SWITCH_PORT_SERDES_ATTR_TX_FIR_POST1, post1);
    status |= switch_store::v_get(
        parent, SWITCH_PORT_SERDES_ATTR_TX_FIR_POST2, post2);
    status |=
        switch_store::v_get(port_hdl, SWITCH_PORT_ATTR_LANE_LIST, lane_list);
    status |=
        switch_store::v_get(parent, SWITCH_PORT_SERDES_ATTR_TX_FIR_ATTN, attn);
  }
  switch_status_t create_update() {
    bool sw_model = false;
    if (is_dummy_object) return SWITCH_STATUS_SUCCESS;
    size_t lane_n = lane_list.size();
    if (lane_n != pre1.size() || lane_n != pre2.size() ||
        lane_n != main.size() || lane_n != post1.size() ||
        lane_n != post2.size() || lane_n != attn.size()) {
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OBJECT_TYPE_PORT,
                 "{}: port serdes config list len != number of lanes '{}' "
                 "status={} for device {}",
                 __func__,
                 lane_n,
                 SWITCH_STATUS_INVALID_PARAMETER,
                 device_hdl);
      return SWITCH_STATUS_INVALID_PARAMETER;
    }

    bf_pal_pltfm_type_get(device, &sw_model);
    if (!sw_model) {
      // Apply this only on hardware
      if (bf_lld_dev_is_tof2(device)) {
        // Tofino2 hw
        for (uint i = 0; i < lane_list.size(); i++) {
          bf_status = bf_tof2_serdes_tx_taps_set(bf_dev_id,
                                                 bf_dev_port,
                                                 lane_list[i],
                                                 pre2[i],
                                                 pre1[i],
                                                 main[i],
                                                 post1[i],
                                                 post2[i],
                                                 true);
          if (bf_status != BF_SUCCESS) {
            switch_log(SWITCH_API_LEVEL_ERROR,
                       SWITCH_OBJECT_TYPE_PORT_SERDES,
                       "{}.{}: bf_tof2_serdes_tx_taps_set failure device {} "
                       "dev_port "
                       "{} num_lane {} status {}",
                       __func__,
                       __LINE__,
                       bf_dev_id,
                       bf_dev_port,
                       lane_list[i],
                       pal_status_xlate(bf_status));
            return pal_status_xlate(bf_status);
          }
        }
      } else {
        // Tofino hardware
        /*
         Driver api takes three parameters: pre, post, attn or main.
         now we are using pre1, post1, attn.
         If there are any discrepancies, we can change later.
        */
        for (uint i = 0; i < lane_list.size(); i++) {
          bf_status = bf_serdes_tx_drv_attn_set(
              bf_dev_id, bf_dev_port, lane_list[i], attn[i], post1[i], pre1[i]);
          if (bf_status != BF_SUCCESS) {
            switch_log(
                SWITCH_API_LEVEL_ERROR,
                SWITCH_OBJECT_TYPE_PORT_SERDES,
                "{}.{}: bf_serdes_tx_drv_attn_set failure device {} dev_port "
                "{} num_lane {} status {}",
                __func__,
                __LINE__,
                bf_dev_id,
                bf_dev_port,
                lane_list[i],
                pal_status_xlate(bf_status));
            return pal_status_xlate(bf_status);
          }
        }
      }
    }
    auto_object::create_update();
    return pal_status_xlate(bf_status);
  }
};

switch_status_t pal_init() {
  switch_status_t status = SWITCH_STATUS_SUCCESS;
  REGISTER_OBJECT(port_serdes, SWITCH_OBJECT_TYPE_PORT_SERDES_TABLE);
  status |= switch_store::reg_create_trigs_after(SWITCH_OBJECT_TYPE_DEVICE,
                                                 &after_device_create);
  status |= switch_store::reg_create_trigs_after(
      SWITCH_OBJECT_TYPE_DEVICE,
      &after_device_create_cache_fp_dev_port_mapping);

  status |= switch_store::reg_create_trigs_before(SWITCH_OBJECT_TYPE_PORT,
                                                  &before_port_create);
  status |= switch_store::reg_update_trigs_before(SWITCH_OBJECT_TYPE_PORT,
                                                  &before_port_update);
  status |= switch_store::reg_update_trigs_after(SWITCH_OBJECT_TYPE_PORT,
                                                 &after_port_update);
  status |= switch_store::reg_create_trigs_after(SWITCH_OBJECT_TYPE_PORT,
                                                 &after_port_create);
  status |= switch_store::reg_delete_trigs_before(SWITCH_OBJECT_TYPE_PORT,
                                                  &before_port_delete);
  status |= switch_store::reg_create_trigs_before(
      SWITCH_OBJECT_TYPE_PORT, &before_port_create_add_internal_ports);
  status |= switch_store::reg_update_trigs_before(
      SWITCH_OBJECT_TYPE_PORT, &before_port_update_update_internal_ports);
  status |= switch_store::reg_delete_trigs_before(
      SWITCH_OBJECT_TYPE_PORT, &before_port_delete_delete_internal_ports);
  if (status != SWITCH_STATUS_SUCCESS) return status;

  status = bf_pal_port_status_notif_reg(switch_port_state_change_cb, NULL);
  return status;
}

switch_status_t pal_clean() { return SWITCH_STATUS_SUCCESS; }

} /* namespace smi */
