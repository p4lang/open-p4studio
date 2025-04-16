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

#include <iostream>
#include "rate-lim.h"

namespace MODEL_CHIP_NAMESPACE {

void RateLim::set_rl_id (char* parent, int rl_id) {
  m_parent = parent;
  m_rl_id = rl_id;
}

void RateLim::set_port_rate (float port_rate) {
  m_port_rate = port_rate;
}

void RateLim::set_drain_time (float drain_time) {
  m_drain_time = drain_time;
}

void RateLim::set_drain_params() {
  m_drain_rate = m_port_rate * m_drain_time;
  // printf("%s_rl_%0d Draining %0f bits every %0fns\n",
  // m_parent, m_rl_id, m_drain_rate, m_drain_time);
}

void RateLim::set_outstanding_bits(int bytes) {
  m_outstanding_bits += (bytes*8);
}

void RateLim::set_xoff_thresh(int threshold) {
  m_xoff_thresh = threshold;
}

void RateLim::set_xon_thresh(int threshold) {
  m_xon_thresh = threshold;
}

void RateLim::set_decr_thresh(int threshold) {
  m_decr_thresh = threshold;
}

void RateLim::set_ipg_bytes(int ipg) {
  m_ipg_bytes = ipg;
}

void RateLim::set_preamble_bytes(int preamble) {
  m_preamble_bytes = preamble;
}

bool RateLim::check_rl_xoff() {
  return(m_issue_xoff);
}

void RateLim::process_line_sent(bool sop, bool eop, int bytes) {

  m_outstanding_bits += (bytes*8);
  if (sop)
    m_outstanding_bits += (m_preamble_bytes*8);
  if (eop)
    m_outstanding_bits +=  (m_ipg_bytes * 8);
  if ((((m_outstanding_bits + 7) / 8) > m_xoff_thresh) && (m_issue_xoff == 0)) {
    //printf("%s_rl_%0d Issued Xoff\n", m_parent, m_rl_id);
    m_issue_xoff = 1;
  }
}

void RateLim::drain_from_fifo() {
  if (m_outstanding_bits > (-m_drain_rate * m_decr_thresh)) {
    m_outstanding_bits -= m_drain_rate;
    if ((((m_outstanding_bits + 7) / 8) <= m_xon_thresh) && (m_issue_xoff == 1)) {
      //printf("%s_rl_%0d Issued Xon\n", m_parent, m_rl_id);
      m_issue_xoff = 0;
    }
  }
}

}
