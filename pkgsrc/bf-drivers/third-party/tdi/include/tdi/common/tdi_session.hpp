// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_session.hpp
 *
 *  @brief Contains TDI Session APIs
 */
#ifndef _TDI_SESSION_HPP
#define _TDI_SESSION_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <tdi/common/tdi_defs.h>

namespace tdi {

/**
 * @brief Class to create session objects. <br>
 * <B>Creation: </B> Can only be created using \ref
 * tdi::Device::sessionCreate()
 */
class Session {
 public:
  /**
   * @brief Destructor which destroys the connection if it is established
   */
  virtual ~Session() = default;

  Session() = delete;
  Session(const std::vector<tdi_mgr_type_e> &mgr_type_list)
      : mgr_type_list_(mgr_type_list){};

  /**
   * @name Session APIs
   * @{
   */

  /**
   * @brief Create session. Creation and destruction
   * of a session is not tied to the ctor and dtor of this
   * class but rather a separate API is provided for
   * ease of error handling
   *
   * @return Status of the API call
   */
  virtual tdi_status_t create() = 0;
  /**
   * @brief Destroy session
   *
   * @return Status of the API call
   *
   */
  virtual tdi_status_t destroy() = 0;

  /**
   * @brief Wait for all operations to complete under this session
   *
   * @return Status of the API call
   */
  virtual tdi_status_t completeOperations() const = 0;

  /**
   * @brief Get the Session Handle
   *
   * @param [in]
   * @returns Session Handle of an active session
   */
  virtual tdi_handle_t handleGet(const tdi_mgr_type_e &mgr_type) const = 0;

  /**
   * @brief Check whether this Session Object's handle is still valid or not
   *
   * @retval true if session exists
   * @retval false if session has been destroyed
   */
  const bool &isValid() const { return is_valid_; };
  /** @} */  // End of group Session

  // Batching functions
  /**
   * @name Batching Batching APIs
   * @{
   */
  /**
   * @brief Begin a batch on a session. Only one batch can be in progress
   * on any given session.  Updates to the hardware will be batch together
   * and delayed until the batch is ended.
   *
   * @return Status of the API call
   */
  virtual tdi_status_t beginBatch() const = 0;

  /**
   * @brief Flush a batch on a session pushing all pending updates to hardware.
   *
   * @return Status of the API call
   */
  virtual tdi_status_t flushBatch() const = 0;

  /**
   * @brief End a batch on a session and push all batched updated to hardware.
   *
   * @param[in] hwSynchronous If @c true, block till all operations are
   *complete,
   *  in this transaction.
   *
   * @return Status of the API call
   */
  virtual tdi_status_t endBatch(bool hwSynchronous) const = 0;
  /** @} */  // End of group Batching

  /**
   * @name Transaction
   * Transaction APIs
   * @{
   */
  // Transaction functions
  /**
   * @brief Begin a transaction on a session. Only one transaction can be in
   *progress
   *      \n on any given session
   *
   * @param[in] isAtomic If @c true, upon committing the transaction, all
   *changes
   *        \n will be applied atomically such that a packet being processed
   *will
   *        \n see either all of the changes or none of the changes.
   * @return Status of the API call
   */
  virtual tdi_status_t beginTransaction(bool isAtomic) const = 0;

  /**
   * @brief Verify if all the API requests against the transaction in progress
   *have
   *        \n resources to be committed. This also ends the transaction
   *implicitly
   *
   * @return Status of the API call
   */
  virtual tdi_status_t verifyTransaction() const = 0;

  /**
   * @brief Commit all changes in a transaction
   *
   * @param[in] hwSynchronous If @c true, block till all operations are
   *complete,
   *  in this transaction.
   *
   * @return Status of the API call
   */
  virtual tdi_status_t commitTransaction(bool hwSynchronous) const = 0;

  /**
   * @brief Abort and rollback all API requests against the transaction in
   *progress.
   * \n This also ends the transaction implicitly
   *
   * @return Status of the API call
   */
  virtual tdi_status_t abortTransaction() const = 0;
  /** @} */  // End of group Transaction
 protected:
  std::vector<tdi_mgr_type_e> mgr_type_list_;
  bool is_valid_ = true;
};

}  // namespace tdi

#endif
