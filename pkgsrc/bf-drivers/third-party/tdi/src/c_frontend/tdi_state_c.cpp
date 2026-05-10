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

#include <tdi/common/tdi_session.hpp>
#include "tdi_state_c.hpp"

namespace tdi {
namespace tdi_c {

TdiCFrontEndSessionState &TdiCFrontEndSessionState::getInstance() {
  static TdiCFrontEndSessionState instance;
  return instance;
}

std::shared_ptr<Session> TdiCFrontEndSessionState::getSharedPtr(
    const Session *session_raw) {
  if (session_raw == nullptr) {
    return nullptr;
  }
  std::lock_guard<std::mutex> lock(state_lock);
  if (sessionStateMap.find(session_raw) != sessionStateMap.end()) {
    return sessionStateMap.at(session_raw);
  }
  return nullptr;
}

void TdiCFrontEndSessionState::insertShared(
    std::shared_ptr<Session> session) {
  std::lock_guard<std::mutex> lock(state_lock);
  if (sessionStateMap.find(session.get()) != sessionStateMap.end()) {
    return;
  }
  sessionStateMap[session.get()] = session;
}

void TdiCFrontEndSessionState::removeShared(const Session *session) {
  std::lock_guard<std::mutex> lock(state_lock);
  if (sessionStateMap.find(session) == sessionStateMap.end()) {
    return;
  }
  sessionStateMap.erase(sessionStateMap.find(session));
}

}  // tdi_c
}  // tdi
