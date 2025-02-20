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

#ifndef __RATE_LIM__
#define __RATE_LIM__

namespace MODEL_CHIP_NAMESPACE {

class RateLim {

 public:

  RateLim() {}
  void set_rl_id(char* parent, int rl_id);
  void set_port_rate (float port_rate);
  void set_drain_time (float drain_time);
  void set_drain_params();
  void set_outstanding_bits(int bytes);
  void set_xoff_thresh(int threshold);
  void set_xon_thresh(int threshold);
  void set_decr_thresh(int threshold);
  void set_ipg_bytes(int ipg);
  void set_preamble_bytes(int preamble);
  void process_line_sent(bool sop, bool eop, int bytes);
  void drain_from_fifo();
  bool check_rl_xoff();

 private:
  char* m_parent{nullptr};
  int m_rl_id{0};
  float m_port_rate{0};
  float m_drain_rate{0};
  float m_drain_time{0};
  double m_outstanding_bits{0};
  int m_xoff_thresh{256};
  int m_xon_thresh{100};
  int m_decr_thresh{1000};
  int m_ipg_bytes{12};
  int m_preamble_bytes{8};
  bool m_issue_xoff{0};
};

}
#endif
