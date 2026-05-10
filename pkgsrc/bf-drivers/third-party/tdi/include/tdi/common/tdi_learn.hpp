// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_learn.hpp
 *
 *  @brief Contains TDI Learn Object APIs
 */
#ifndef _TDI_LEARN_HPP
#define _TDI_LEARN_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_table_data.hpp>
#include <tdi/common/tdi_target.hpp>

namespace tdi {

// Fwd declaration
class TdiInfo;

/**
 * @brief Only the below functions in comments are supported for tdi::LearnData.
 * \n Use of anything else will return a TDI_NOT_SUPPORTED error
 * \n tdi::TableData::getValue(const tdi_id_t &field_id, uint64_t *val) const;
 * \n tdi::TableData::getValue(const tdi_id_t &field_id, const size_t &size,
 *                     uint8_t *val) const;
 */
using LearnData = TableData;

class LearnInfo;

/**
 * @brief Learn Callback Function. This gets called upon receiving learn digests
 * from the device. Onlye one cb can be associated with a Learn object at a
 *time.
 * If a session already has registered, then another session cannot overwrite
 *
 * @param[in] tdi_tgt TDI target associated with the learn data
 * @param[in] session @c std::shared_ptr to the session
 * @param[in] learnData Vector of learn data objs
 * @param[in] learn_msg_hdl Handle for the msg which can be used to notify ack
 * @param[in] cookie Cookie registered
 */
typedef std::function<tdi_status_t(
    const tdi::Target &tdi_tgt,
    const std::shared_ptr<tdi::Session> session,
    std::vector<std::unique_ptr<tdi::LearnData>> learnDataVec,
    tdi_learn_msg_hdl *const learn_msg_hdl,
    const void *cookie)>
    tdiCbFunction;

/**
 * @brief Class to contain metadata of Learn Obj and perform functions
 *  like register and deregister Learn Callback <br>
 * <B>Creation: </B> Cannot be created. can only be queried from \ref
 * tdi::TdiInfo
 */
class Learn {
 public:
  virtual ~Learn() = default;

  /**
   * @brief Register Callback function to be called on a Learn event
   *
   * @param[in] session @c std::shared_ptr to the session. shared_ptr is used
   * in all of learn APIs because the callbacks return back a valid session
   * object as well. So it is required for us to make sure that the session
   * exists when the callback is being called. Hence the necessity to create
   * a shared_ptr to share ownership
   * @param[in] dev_tgt Device target
   * @param[in] callback_fn Callback function to be called upon incurring a
   *learn event
   * @param[in] cookie Optional cookie to be received with the callback
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tdiLearnCallbackRegister(
      const std::shared_ptr<tdi::Session> /*session*/,
      const Target & /*dev_tgt*/,
      const tdiCbFunction & /*callback_fn*/,
      const void * /*cookie*/) const {
    return TDI_SUCCESS;
  };

  /**
   * @brief Deregister the callback from the device
   *
   * @param[in] session @c std::shared_ptr to the session
   * @param[in] dev_tgt Device target
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tdiLearnCallbackDeregister(
      const std::shared_ptr<tdi::Session> /*session*/,
      const Target & /*dev_tgt*/) const {
    return TDI_SUCCESS;
  };

  /**
   * @brief Notify the device that the learn_msg was received and
   * processed. This is required to be done in order to free up digest
   * related resources in the device
   *
   * @param[in] session Shared_ptr to the session
   * @param[in] learn_msg_hdl Handle of the msg to be notified
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tdiLearnNotifyAck(
      const std::shared_ptr<tdi::Session> /*session*/,
      tdi_learn_msg_hdl *const /*learn_msg_hdl*/) const {
    return TDI_SUCCESS;
  };

  const LearnInfo *learnInfoGet() const { return learn_info_; }

 protected:
  Learn(const LearnInfo *learn_info) : learn_info_(learn_info){};

 private:
  const LearnInfo *learn_info_{nullptr};
  friend tdi::TdiInfo;
};

}  // namespace tdi

#endif
