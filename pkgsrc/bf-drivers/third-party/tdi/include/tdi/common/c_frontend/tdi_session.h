/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_session.h
 *
 *  @brief Contains TDI Session APIs
 */
#ifndef _TDI_SESSION_H
#define _TDI_SESSION_H

#include <tdi/common/tdi_defs.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief API to create session
 *
 * @param[out] session Ptr to allocated session object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_session_create(const tdi_device_hdl *device_hdl, tdi_session_hdl **session);

/**
 * @brief Destroy a session
 *
 * @param[in] session Ptr to session object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_session_destroy(tdi_session_hdl *const session);

/**
 * @brief Get the internal session ID.
 *
 * @param[in] session Ptr to session object
 *
 * @return Internal session ID
 */
tdi_id_t tdi_sess_handle_get(const tdi_session_hdl *const session);

/**
 * @brief Check whether this Session Object  is still valid or not
 *
 * @param[in] session Ptr to session object
 *
 * @retval true if session exists
 * @retval false if session has been destroyed
 */
bool tdi_session_is_valid(const tdi_session_hdl *const session);

/**
 * @brief Wait for all operations to complete under this session
 *
 * @param[in] session Ptr to session object
 * @return Status of the API call
 */
tdi_status_t tdi_session_complete_operations(
    const tdi_session_hdl *const session);

// Batching functions
/**
 * @brief Begin a batch on a session. Only one batch can be in progress
 * on any given session.  Updates to the hardware will be batch together
 * and delayed until the batch is ended.
 *
 * @param[in] session Ptr to session object
 * @return Status of the API call
 */
tdi_status_t tdi_begin_batch(tdi_session_hdl *const session);

/**
 * @brief Flush a batch on a session pushing all pending updates to hardware.
 *
 * @param[in] session Ptr to session object
 * @return Status of the API call
 */
tdi_status_t tdi_flush_batch(tdi_session_hdl *const session);

/**
 * @brief End a batch on a session and push all batched updated to hardware.
 *
 * @param[in] session Ptr to session object
 * @param[in] hwSynchronous If @c true, block till all operations are
 * complete,
 *  in this transaction.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_end_batch(tdi_session_hdl *const session,
                            bool hwSynchronous);

// Transaction functions
/**
 * @brief Begin a transaction on a session. Only one transaction can be in
 *progress
 *      \n on any given session
 *
 * @param[in] session Ptr to session object
 * @param[in] isAtomic If @c true, upon committing the transaction, all
 *changes
 *        \n will be applied atomically such that a packet being processed
 *will
 *        \n see either all of the changes or none of the changes.
 * @return Status of the API call
 */
tdi_status_t tdi_begin_transaction(tdi_session_hdl *const session,
                                    bool isAtomic);

/**
 * @brief Verify if all the API requests against the transaction in progress
 *have
 *        \n resources to be committed. This also ends the transaction
 *implicitly
 *
 * @param[in] session Ptr to session object
 * @return Status of the API call
 */
tdi_status_t tdi_verify_transaction(tdi_session_hdl *const session);

/**
 * @brief Commit all changes in a transaction
 *
 * @param[in] session Ptr to session object
 * @param[in] hwSynchronous If @c true, block till all operations are
 *complete,
 *  in this transaction.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_commit_transaction(tdi_session_hdl *const session,
                                     bool hwSynchronous);

/**
 * @brief Abort and rollback all API requests against the transaction in
 *progress.
 * \n This also ends the transaction implicitly
 *
 * @param[in] session Ptr to session object
 * @return Status of the API call
 */
tdi_status_t tdi_abort_transaction(tdi_session_hdl *const session);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_SESSION_H
