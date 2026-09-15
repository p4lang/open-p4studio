// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include <tdi/common/c_frontend/tdi_session.h>
#include <tdi/common/tdi_init.hpp>
#include <tdi/common/tdi_session.hpp>
//#include <tdi_common/tdi_session_impl.hpp>
#include <tdi/common/tdi_utils.hpp>

#include "tdi_state_c.hpp"
tdi_status_t tdi_session_create(const tdi_device_hdl *device_hdl,
                                tdi_session_hdl **session) {
  auto device = reinterpret_cast<const tdi::Device *>(device_hdl);
  if (device == nullptr) {
    LOG_ERROR("%s:%d Unable to create session", __func__, __LINE__);
    return TDI_UNEXPECTED;
  }
  std::shared_ptr<tdi::Session> sess;
  device->createSession(&sess);
  if (sess == nullptr) {
    LOG_ERROR("%s:%d Unable to create session", __func__, __LINE__);
    return TDI_UNEXPECTED;
  }
  *session = reinterpret_cast<tdi_session_hdl *>(sess.get());
  // insert the shared_ptr in the state map
  auto &c_state = tdi::tdi_c::TdiCFrontEndSessionState::getInstance();
  c_state.insertShared(sess);
  return TDI_SUCCESS;
}

tdi_status_t tdi_session_destroy(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  auto status = sess->destroy();
  if (status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d Failed to destroy session", __func__, __LINE__);
    return status;
  }
  // remove the shared_ptr from the state map. It will get destroyed
  // automatically if not already
  // temperate disable for compile
  auto &c_state = tdi::tdi_c::TdiCFrontEndSessionState::getInstance();
  c_state.removeShared(sess);
  return TDI_SUCCESS;
}

tdi_id_t tdi_sess_handle_get(const tdi_session_hdl *const session,
                             tdi_mgr_type_e &mgr_type) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<const tdi::Session *>(session);
  return sess->handleGet(mgr_type);
}
bool tdi_session_is_valid(const tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return false;
  }
  auto sess = reinterpret_cast<const tdi::Session *>(session);
  return sess->isValid();
}

tdi_status_t tdi_session_complete_operations(
    const tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return false;
  }
  auto sess = reinterpret_cast<const tdi::Session *>(session);
  return sess->completeOperations();
}

tdi_status_t tdi_begin_batch(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->beginBatch();
}

tdi_status_t tdi_flush_batch(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->flushBatch();
}

tdi_status_t tdi_end_batch(tdi_session_hdl *const session, bool hwSynchronous) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->endBatch(hwSynchronous);
}

tdi_status_t tdi_begin_transaction(tdi_session_hdl *const session,
                                   bool isAtomic) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->beginTransaction(isAtomic);
}

tdi_status_t tdi_verify_transaction(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->verifyTransaction();
}

tdi_status_t tdi_commit_transaction(tdi_session_hdl *const session,
                                    bool hwSynchronous) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->commitTransaction(hwSynchronous);
}

tdi_status_t tdi_abort_transaction(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return false;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->abortTransaction();
}
