// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <tdi/common/c_frontend/tdi_learn.h>

#ifdef __cplusplus
}
#endif

#include <tdi/common/tdi_learn.hpp>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_info.hpp>
//#include <tdi_common/tdi_session_impl.hpp>
//#include <tdi_p4/tdi_learn_impl.hpp>
#include <src/tdi_utils.hpp>
#include "tdi_state_c.hpp"
// learn obj apis
tdi_status_t tdi_learn_callback_register(const tdi_learn_hdl *learn_hdl,
                                          const tdi_session_hdl *session,
                                          const tdi_target_t *dev_tgt,
                                          const tdi_cb_function callback_fn,
                                          const void *cookie) {
  // Casting to learnObj here since we need the internal function
  auto learn = reinterpret_cast<const tdi::Learn *>(learn_hdl);
  // Get the shared_ptr from the state
  auto &c_state = tdi::tdi_c::TdiCFrontEndSessionState::getInstance();
  auto session_ptr = c_state.getSharedPtr(
      reinterpret_cast<const tdi::Session *>(session));

  //return learn->tdiLearnCallbackRegisterHelper(
  return learn->tdiLearnCallbackRegister(
      session_ptr, dev_tgt, callback_fn, cookie);
}

tdi_status_t tdi_learn_callback_deregister(const tdi_learn_hdl *learn_hdl,
                                            const tdi_session_hdl *session,
                                            const tdi_target_t *dev_tgt) {
  auto learn = reinterpret_cast<const tdi::Learn *>(learn_hdl);
  // Get the shared_ptr from the state
  auto &c_state = tdi::tdi_c::TdiCFrontEndSessionState::getInstance();
  auto session_ptr = c_state.getSharedPtr(
      reinterpret_cast<const tdi::Session *>(session));

  return learn->tdiLearnCallbackDeregister(session_ptr, dev_tgt);
}

tdi_status_t tdi_learn_notify_ack(const tdi_learn_hdl *learn_hdl,
                                   const tdi_session_hdl *session,
                                   tdi_learn_msg_hdl *const learn_msg_hdl) {
  auto learn = reinterpret_cast<const tdi::Learn *>(learn_hdl);
  // Get the shared_ptr from the state
  auto &c_state = tdi::tdi_c::TdiCFrontEndSessionState::getInstance();
  auto session_ptr = c_state.getSharedPtr(
      reinterpret_cast<const tdi::Session *>(session));
  return learn->tdiLearnNotifyAck(session_ptr, learn_msg_hdl);
}

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_learn_id_get(const tdi_learn_info_hdl *learn_hdl,
                               tdi_id_t *learn_id_ret) {
  auto learn = reinterpret_cast<const tdi::LearnInfo *>(learn_hdl);
  return learn->learnIdGet(learn_id_ret);
}

tdi_status_t tdi_learn_name_get(const tdi_learn_field_info_hdl *learn_hdl,
                                 const char **learn_name_ret) {
  auto learn = reinterpret_cast<const tdi::LearnFieldInfo *>(learn_hdl);
  *learn_name_ret = learn->getName().c_str();
  return TDI_SUCCESS;
}

tdi_status_t tdi_learn_field_id_list_size_get(const tdi_learn_hdl *learn_hdl,
                                               uint32_t *num_ret) {
  // Only here are we using LearnObj than Learn
  // since we need the hidden impl function
  auto learn = reinterpret_cast<const tdi::Learn *>(learn_hdl);
  *num_ret = learn->learnFieldListSize();
  return TDI_SUCCESS;
}

tdi_status_t tdi_learn_field_id_list_get(const tdi_learn_info_hdl *learn_hdl,
                                          tdi_id_t *id_vec_ret) {
  auto learn = reinterpret_cast<const tdi::LearnInfo *>(learn_hdl);
  std::vector<tdi_id_t> temp_vec;
  auto status = learn->learnFieldIdListGet(&temp_vec);
  int i = 0;
  for (auto const &item : temp_vec) {
    id_vec_ret[i++] = item;
  }
  return status;
}

tdi_status_t tdi_learn_field_id_get(const tdi_learn_info_hdl *learn_hdl,
                                     const char *name,
                                     tdi_id_t *field_id) {
  auto learn = reinterpret_cast<const tdi::LearnInfo *>(learn_hdl);
  std::string temp(name);
  return learn->learnFieldIdGet(temp, field_id);
}

tdi_status_t tdi_learn_field_size_get(const tdi_learn_field_info_hdl *learn_hdl,
                                       size_t *size) {
  auto learn = reinterpret_cast<const tdi::LearnFieldInfo *>(learn_hdl);
  return learn->learnFieldSizeGet(size);
}

tdi_status_t tdi_learn_field_is_ptr_get(const tdi_learn_field_info_hdl *learn_hdl,
                                         bool *is_ptr) {
  auto learn = reinterpret_cast<const tdi::LearnFieldInfo *>(learn_hdl);
  return learn->learnFieldIsPtrGet(is_ptr);
}

tdi_status_t tdi_learn_field_name_get(const tdi_learn_info_hdl *learn_hdl,
                                       const tdi_id_t field_id,
                                       const char **field_name) {
  auto learn = reinterpret_cast<const tdi::LearnInfo *>(learn_hdl);
  *field_name = learn->getLearnField(field_id)->getName().c_str();
  return TDI_SUCCESS;
}
#endif
