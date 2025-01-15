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


#ifndef _BF_PAL_PLTFM_PORTING_H
#define _BF_PAL_PLTFM_PORTING_H

// File includes
#include <tofino/bf_pal/bf_pal_types.h>
#include <dvm/bf_drv_intf.h>

// Defined Macros

// CALL-IN Functions
// (These functions are defined by the PM and called by the platform modules)
/**
 * @brief Indicates if a particular port is ready to link up. The port bring up
 * FSM will be started only when a port is enabled && it's ready for bringup
 *
 * [MUST BE CALLED BY PLATFORMS MODULE]
 *
 * @param dev_id Device id
 * @param port_hdl Front panel port in the system
 * @param is_ready Indicates if a port is ready to link up
 *
 * @return Status of the API call
 */
bf_status_t bf_pal_pm_front_port_ready_for_bringup(
    bf_dev_id_t dev_id, bf_pal_front_port_handle_t *port_hdl, bool is_ready);

/**
 * @brief Indicates if a particular port desires autonegotation during
 * being up. If not reported, the PM will assume that a port doesn't desire
 * autonegotation
 *
 * [MUST BE CALLED BY PLATFORMS MODULE]
 *
 * @param dev_id Device id
 * @param port_hdl Front panel port in the system
 * @param is_eligible Indicates if a port should autoneg while bring up
 *
 * @return Status of the API call
 */
bf_status_t bf_pal_pm_front_port_eligible_for_autoneg(
    bf_dev_id_t dev_id, bf_pal_front_port_handle_t *port_hdl, bool is_eligible);

/**
 * @brief Return the number of MAC lanes consumed by a given port
 *
 * [OPTIONALLY CALLED BY PLATFORMS MODULE]
 *
 * @param dev_id Device id
 * @param port_hdl Front panel port in the system
 * @param num_lanes Number of lanes consumed by the port
 *
 * @return Status of the API call
 */
bf_status_t bf_pal_pm_front_port_num_lanes_get(
    bf_dev_id_t dev_id, bf_pal_front_port_handle_t *port_hdl, int *num_lanes);

// CALLBACK Functions
// (These functions are defined and registered by the platform modules and
// called by the PM)

/**
 * @brief Notify the platform modules that it is safe for it to make calls
 * into bf_pm using the Call-In functions
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_safe_to_call_in_notify)();

/**
 * @brief Get the first front panel port as determined by the platforms module
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @param first_port_info First front panel port handle
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_front_panel_port_get_first)(
    bf_pal_front_port_info_t *first_port_info);

/**
 * @brief Given a front panel port, get the next front panel port as determined
 * by the platforms module
 * curr_port_info == NULL -> Return the first front panel port in the system
 * return == BF_OBJECT_NOT_FOUND -> curr_port is the last front panel port in
 *the
 * system
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @param curr_port_info Current front panel port handle
 * @param next_port_info Next front panel port handle
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_front_panel_port_get_next)(
    bf_pal_front_port_info_t *curr_port_info,
    bf_pal_front_port_info_t *next_port_info);

/**
 * @brief Given a front panel port, get the corresponding MAC lane to serdes
 * lane mappings
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param mac_blk_map MAC lane to serdes TX/RX lane mapping
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_mac_to_serdes_map_get)(
    bf_pal_front_port_handle_t *port_hdl,
    bf_pal_mac_to_serdes_lane_map_t *mac_blk_map);

/**
 * @brief Given a front panel port, get parameters of the corresponding serdes
 * lane
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param serdes_info Serdes lane info
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_serdes_info_get)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_serdes_info_t *serdes_info);

/**
 * @brief On a port add call, configure required platforms modules (if any)
 * before a port is actually added
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_pre_port_add_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief On a port add call, configure required platforms modules (if any)
 * after a port is added
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_post_port_add_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief On a port delete call, configure required platforms modules (if any)
 * before a port is actually deleted
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_pre_port_delete_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief On a port delete call, configure required platforms modules (if any)
 * after a port is deleted
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_post_port_delete_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief On a port enable call, configure required platforms modules (if any)
 * before a port is actually enabled
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_pre_port_enable_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief On a port enable call, configure required platforms modules (if any)
 * after a port is enabled
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_post_port_enable_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief On a port disable call, configure required platforms modules (if any)
 * before a port is actually disabled
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_pre_port_disable_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief On a port disable call, configure required platforms modules (if any)
 * after a port is disabled
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_post_port_disable_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief Configure required platforms modules (if any) after a port comes UP
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_port_link_up_actions)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief Configure required platforms modules (if any) after a port goes DOWN
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_port_link_down_actions)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief Return the media type of the cable connected to the port
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param media_type Type of the cable connected to the port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_port_media_type_get)(
    bf_pal_front_port_handle_t *port_hdl, bf_media_type_t *media_type);

/**
 * @brief Return the port handle corresponding to the port string
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @param port_str String describing the port
 * @param port_hdl Corresponding port handle
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_port_str_to_hdl_get)(
    const char *port_str, bf_pal_front_port_handle_t *port_hdl);

/**
 * @brief Read necessary hardware information (if applicable) at init time
 * so that port bringup can proceed smoothly
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_init)(bf_dev_init_mode_t warm_init_mode);

/**
 * @brief Invoke  hardware deinit (if applicable) at deinit time
 * so that platform gets a chance shutdow/free it's resource
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param dev_id Device id
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_deinit)(bf_dev_id_t dev_id);

/**
 * @brief On prbs setup, configure required platforms modules (if any)
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param port_cfg Configuration of the front panel port
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_port_prbs_cfg_set)(
    bf_pal_front_port_handle_t *port_hdl, bf_pal_front_port_cb_cfg_t *port_cfg);

/**
 * @brief when warm restart is initiated, set ha mode flag (if supported)
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_ha_mode_set)();

/**
 * @brief when warm restart is steps complete, clear ha mode flag (if supported)
 *
 * [OPTIONALLY DEFINED BY PLATFORMS MODULE]
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_ha_mode_clear)();

/**
 * @brief At the end of warm-init this is called by switchd to notify the
 * platforms, so that it can move its state machines. Link up state passed
 * here is read from hardware.
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @param dev_id Device id of the port
 * @param port_hdl Front panel port handle
 * @param link_up Link up or down state
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_ha_link_state_notify)(
    bf_dev_id_t dev_id, bf_pal_front_port_handle_t *port_hdl, bool link_up);

/**
 * @brief Given a front panel port, get the corresponding MAC lane to serdes
 * lanes mappings
 *
 * [MUST BE DEFINED BY PLATFORMS MODULE]
 *
 * @param port_hdl Front panel port handle
 * @param mac_blk_map MAC lane to serdes TX/RX lanes mapping
 *
 * @return Status of the API call
 */
typedef bf_status_t (*bf_pal_pltfm_mac_to_multi_serdes_map_get)(
    bf_pal_front_port_handle_t *port_hdl,
    bf_pal_mac_to_multi_serdes_lane_map_t *mac_blk_map);

typedef bf_status_t (*bf_pal_pltfm_ha_verify_port_state)(bf_dev_id_t dev_id);
typedef int (*bf_pal_pltfm_check_port_non_func)(
    bf_dev_id_t dev_id, bf_pal_front_port_handle_t *port_hdl);

// Defined data structures

/**
 * Identifies the functions registered by the platforms module
 */
typedef struct bf_pal_pltfm_reg_interface_t {
  bf_pal_pltfm_safe_to_call_in_notify pltfm_safe_to_call_in_notify;
  bf_pal_pltfm_front_panel_port_get_first pltfm_front_panel_port_get_first;
  bf_pal_pltfm_front_panel_port_get_next pltfm_front_panel_port_get_next;
  bf_pal_pltfm_mac_to_serdes_map_get pltfm_mac_to_serdes_map_get;
  bf_pal_pltfm_serdes_info_get pltfm_serdes_info_get;
  bf_pal_pltfm_pre_port_add_cfg_set pltfm_pre_port_add_cfg_set;
  bf_pal_pltfm_post_port_add_cfg_set pltfm_post_port_add_cfg_set;
  bf_pal_pltfm_pre_port_delete_cfg_set pltfm_pre_port_delete_cfg_set;
  bf_pal_pltfm_post_port_delete_cfg_set pltfm_post_port_delete_cfg_set;
  bf_pal_pltfm_pre_port_enable_cfg_set pltfm_pre_port_enable_cfg_set;
  bf_pal_pltfm_post_port_enable_cfg_set pltfm_post_port_enable_cfg_set;
  bf_pal_pltfm_pre_port_disable_cfg_set pltfm_pre_port_disable_cfg_set;
  bf_pal_pltfm_post_port_disable_cfg_set pltfm_post_port_disable_cfg_set;
  bf_pal_pltfm_port_link_up_actions pltfm_port_link_up_actions;
  bf_pal_pltfm_port_link_down_actions pltfm_port_link_down_actions;
  bf_pal_pltfm_port_media_type_get pltfm_port_media_type_get;
  bf_pal_pltfm_port_str_to_hdl_get pltfm_port_str_to_hdl_get;
  bf_pal_pltfm_init pltfm_init;
  bf_pal_pltfm_deinit pltfm_deinit;
  bf_pal_pltfm_port_prbs_cfg_set pltfm_port_prbs_cfg_set;
  bf_pal_pltfm_ha_mode_set pltfm_ha_mode_set;
  bf_pal_pltfm_ha_mode_clear pltfm_ha_mode_clear;
  bf_pal_pltfm_ha_link_state_notify pltfm_ha_link_state_notify;
  bf_pal_pltfm_mac_to_multi_serdes_map_get pltfm_mac_to_multi_serdes_map_get;
  bf_pal_pltfm_ha_verify_port_state pltfm_ha_wait_port_cfg_done;
  bf_pal_pltfm_check_port_non_func pltfm_is_port_non_func;
} bf_pal_pltfm_reg_interface_t;

/**
 * @brief Set all the function pointers to the functions defined by the
 * platforms module
 *
 * @param pltfm_interface All the platform functions to be registered
 *
 * @return Status of the API call
 */
bf_status_t bf_pal_pltfm_all_interface_set(
    bf_pal_pltfm_reg_interface_t *pltfm_interface);

/**
 * @brief Get all the functions defined by the platforms module
 *
 * @param pltfm_interface All the registered platform functions
 *
 * @return Status of the API call
 */
bf_status_t bf_pal_pltfm_all_interface_get(
    bf_pal_pltfm_reg_interface_t *pltfm_interface);

#endif
