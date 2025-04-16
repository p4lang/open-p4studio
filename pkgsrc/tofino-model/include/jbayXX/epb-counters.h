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

#ifndef _JBAYXX_EPB_COUNTERS_H_
#define _JBAYXX_EPB_COUNTERS_H_

#include <register_includes/epb_chnl_parser_send_pkt_mutable.h>
#include <register_includes/epb_chnl_deparser_send_pkt_mutable.h>
#include <register_includes/epb_chnl_warp_send_pkt.h>
#include <register_includes/epb_chnl_p2s_received_pkt.h>

namespace MODEL_CHIP_NAMESPACE {

class EpbCounters {
 public:
  // constructor
  EpbCounters(int chipIndex, int pipeIndex, int epbIndex, int chanIndex);
  // reset all counters
  void reset();

  register_classes::EpbChnlParserSendPktMutable    chnl_parser_send_pkt_;
  register_classes::EpbChnlDeparserSendPktMutable  chnl_deparser_send_pkt_;
  register_classes::EpbChnlWarpSendPkt             chnl_warp_send_pkt_;
  register_classes::EpbChnlP2sReceivedPkt          chnl_p2s_received_pkt_;

  void increment_chnl_deparser_send_pkt();
  void increment_chnl_parser_send_pkt();
  void increment_chnl_parser_send_pkt_err();
  // for compatibility with tofinoXX only; this counter not implemented on jbay
  void increment_egr_bypass_count() { };

 protected:
  int wrap_counter_size_ = 36;
};

}

#endif //_JBAYXX_EPB_COUNTERS_H_
