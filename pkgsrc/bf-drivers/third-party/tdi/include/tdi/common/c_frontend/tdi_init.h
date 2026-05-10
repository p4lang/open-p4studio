/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_init.h
 *
 *  @brief C frontend for init
 */
#ifndef _TDI_INIT_H_
#define _TDI_INIT_H_

// tdi includes
#include <stdbool.h>
#include <tdi/common/tdi_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the TdiDevice object corresponding to the (device_id)
 *
 * @param[in] dev_id Device ID
 * @param[out] tdi_device Device Obj associated with the Device.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_device_get(const tdi_dev_id_t dev_id,
                            const tdi_device_hdl **device_hdl_ret);

/**
 * @brief create the Flags corresponding to the device obj and flag
 *
 * @param[in] flag_value to initialize the Flags.
 * @param[out] flags_hdl Flags Obj Pointer associated with the Flags.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_flags_create(const uint64_t flag_value,
                              tdi_flags_hdl **flags_hdl);

/**
 * @brief delete the Flags corresponding to the device obj and flag
 *
 *
 * @param[in] flags_hdl Flags Obj Pointer associated with the Flags.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_flags_delete(tdi_flags_hdl *flags_hdl);

tdi_status_t tdi_flags_set_value(tdi_flags_hdl *flags_hdl,
                                  enum tdi_flags_e flags_field,
                                  bool value);

tdi_status_t tdi_flags_get_value(const tdi_flags_hdl *flags_hdl,
                                  enum tdi_flags_e flags_field,
                                  bool *value);

/**
 * @brief Get the TdiInfo object corresponding to the (device_id,
 * program name)
 *
 * @param[in] dev_id Device ID
 * @param[in] prog_name Name of the P4 program
 * @param[out] tdi_info TdiInfo Obj associated with the Device
 *    and the program name.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_info_get(const tdi_dev_id_t dev_id,
                          const char *prog_name,
                          const tdi_info_hdl **info_hdl_ret);

/**
 * @brief Get size of list of all device IDs currently added
 *
 * @param[out] num Size of list of device IDs
 * IDs that are present.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_num_device_id_list_get(uint32_t *num);

/**
 * @brief Get a list of all device IDs currently added
 *
 * @param[out] device_id_list Set Contains list of device
 * IDs that are present.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_device_id_list_get(tdi_dev_id_t *device_id_list);

/**
 * @brief create the Target corresponding to the device obj
 *
 * @param[in] device_hdl to initialize the Target.
 * @param[out] target_hdl Obj Pointer associated with the device obj.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_target_create(const tdi_device_hdl *device_hdl,
                               tdi_target_hdl **target_hdl);

/**
 * @brief delete the Target corresponding to the target obj
 *
 * @param[in] target_hdl to delete the Target.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_target_delete(tdi_target_hdl *target_hdl);

/**
 * @brief Set value of a target field like device_id in the
 * target object
 *
 * @param[in] target_hdl hdl to the target object
 * @param[in] target_field enum specifying target field
 * @param[in] value value of the target field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_target_set_value(tdi_target_hdl *target_hdl,
                                  tdi_target_e target_field,
                                  uint64_t value);

/**
 * @brief Get value of a target field like device_id from the
 * target object
 *
 * @param[in] target_hdl hdl to the target object
 * @param[in] target_field enum specifying target field
 * @param[out] value value of the target field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_target_get_value(const tdi_target_hdl *target_hdl,
                                  tdi_target_e target_field,
                                  uint64_t *value);

/**
 * @brief Get size of list of loaded p4 program names on a particular device
 *
 * @param[in] dev_id Device ID
 * @param[out] num_names Size of the list
 *
 * @return Status of the API call
 */
tdi_status_t tdi_num_p4_names_get(const tdi_dev_id_t dev_id, int *num_names);

/**
 * @brief Get a list of loaded p4 program names on a particular device
 *
 * @param[in] dev_id Device ID
 * @param[out] p4_names Array of char*. User needs to allocate the array of ptrs
 * to pass as input to this function.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_p4_names_get(const tdi_dev_id_t dev_id, const char **p4_names);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_INIT_H
