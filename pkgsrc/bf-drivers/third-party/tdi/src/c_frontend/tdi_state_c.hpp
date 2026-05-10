// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifndef _TDI_STATE_C_HPP
#define _TDI_STATE_C_HPP

#include <mutex>
#include <unordered_map>

namespace tdi {
namespace tdi_c {

class TdiCFrontEndSessionState {
 public:
  // To get the singleton instance. Threadsafe
  static TdiCFrontEndSessionState &getInstance();

  // Get the shared_ptr from the raw pointer
  std::shared_ptr<Session> getSharedPtr(const Session *session_raw);

  // Insert shared_ptr in the state
  void insertShared(std::shared_ptr<Session> session);
  // Delete an entry from the raw ptr
  void removeShared(const Session *session_raw);
  TdiCFrontEndSessionState(TdiCFrontEndSessionState const &) = delete;
  void operator=(TdiCFrontEndSessionState const &) = delete;

 private:
  TdiCFrontEndSessionState() {}
  std::mutex state_lock;
  std::map<const Session *, std::shared_ptr<Session> > sessionStateMap;
};

}  // tdi_c
}  // tdi

#endif  // _TDI_STATE_C_HPP
