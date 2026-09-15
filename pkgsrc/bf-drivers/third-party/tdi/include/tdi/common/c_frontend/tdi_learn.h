/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_learn.h
 *
 *
 *  @brief Contains TDI Learn Object APIs
 */
#ifndef _TDI_LEARN_H
#define _TDI_LEARN_H

#include <tdi/common/tdi_defs.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief C Learn Callback Function. This gets called upon receiving
 * learn digests from the device.
 * Only one cb can be associated with a Learn object at a time.
 * If a session already has registered, then another session cannot overwrite
 *
 * @param[in] tdi_tgt TDI target associated with the learn data
 * @param[in] session Session object
 * @param[in] learnData Array of learn data objects
 * @param[in] num Size of array of learn data objects
 * @param[in] learn_msg_hdl Handle for the msg which can be used to notify ack
 * @param[in] cookie Optional user Cookie registered
 */
typedef tdi_status_t (*tdi_cb_function)(
    const tdi_target_hdl *tdi_tgt,
    const tdi_session_hdl *session,
    tdi_learn_data_hdl **learnData,
    uint32_t num,
    tdi_learn_msg_hdl *const learn_msg_hdl,
    const void *cookie);

/**
 * @brief Register Callback function to be called on a Learn event
 *
 * @param[in] learn Learn object handle
 * @param[in] session Ptr to the session
 * @param[in] dev_tgt Device target
 * @param[in] callback_fn Callback function to be called upon incurring a
 * learn event
 * @param[in] cookie Optional cookie to be received with the callback
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_callback_register(const tdi_learn_hdl *learn,
                                          const tdi_session_hdl *session,
                                          const tdi_target_hdl *dev_tgt,
                                          const tdi_cb_function callback_fn,
                                          const void *cookie);

/**
 * @brief Deregister the callback from the device
 *
 * @param[in] learn Learn object handle
 * @param[in] session Ptr to the session
 * @param[in] dev_tgt Device target
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_callback_deregister(const tdi_learn_hdl *learn,
                                            const tdi_session_hdl *session,
                                            const tdi_target_hdl *dev_tgt);

/**
 * @brief Notify the device that the learn_msg was received and
 * processed. This is required to be done in order to free up digest
 * related resources in the device
 *
 * @param[in] learn Learn object handle
 * @param[in] session Shared_ptr to the session
 * @param[in] learn_msg_hdl Handle of the msg to be notified
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_notify_ack(const tdi_learn_hdl *learn,
                                   const tdi_session_hdl *session,
                                   tdi_learn_msg_hdl *const learn_msg_hdl);

/**
 * @brief Get ID of the learn Object
 *
 * @param[in] learn Learn object handle
 * @param [out] learn_id ID of the learn Object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_id_get(const tdi_learn_hdl *learn,
                               tdi_id_t *learn_id);

/**
 * @brief Get Name of the learn Object
 *
 * @param[in] learn Learn object handle
 * @param [out] learn_name_ret Name of the learn Object. User doesn't need
 * to allocate space for the string
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_name_get(const tdi_learn_hdl *learn,
                                 const char **learn_name_ret);

/**
 * @brief Get the size of list of IDs of the Data Fields associated with
 *    the Learn obj
 *
 * @param[in] learn Learn object handle
 * @param[out] num Size of Array of IDs to contain the list of IDs.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_field_id_list_size_get(const tdi_learn_hdl *learn,
                                               uint32_t *num);

/**
 * @brief Get the list of IDs of the Data Fields associated with
 *    the Learn obj
 *
 * @param[in] learn Learn object handle
 * @param[out] field_id_list Array of IDs to contain the list of IDs. User
 * needs to allocate space for the array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_field_id_list_get(const tdi_learn_hdl *learn,
                                          tdi_id_t *field_id_list);

/**
 * @brief Get the ID of the Learn data field
 *
 * @param[in] learn Learn object handle
 * @param[in] name Data field Name
 * @param[out] field_id Field ID of the Data field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_field_id_get(const tdi_learn_hdl *learn,
                                     const char *name,
                                     tdi_id_t *field_id);

/**
 * @brief Get the size of the Learn Data Field
 *
 * @param[in] learn Learn object handle
 * @param[in] field_id Data field ID
 * @param[out] size Size of the Data field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_field_size_get(const tdi_learn_hdl *learn,
                                       const tdi_id_t field_id,
                                       size_t *size);

/**
 * @brief Find out whether the Learn Data field is a pointer or not
 *
 * @param[in] learn Learn object handle
 * @param[in] field_id Data field ID
 * @param[out] is_ptr Is it a pointer?
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_field_is_ptr_get(const tdi_learn_hdl *learn,
                                         const tdi_id_t field_id,
                                         bool *is_ptr);

/**
 * @brief Get the Name of the Learn Data field
 *
 * @param[in] learn Learn object handle
 * @param[in] field_id Data Field ID
 * @param[out] field_name_ret Field name. Memory allocation by user is not
 *required
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_field_name_get(const tdi_learn_info_hdl *learn,
                                       const tdi_id_t field_id,
                                       const char **field_name_ret);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_LEARN_H
