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

#include <register_adapters.h>
#include <epb-counters.h>
#include <common/rmt-util.h>

namespace MODEL_CHIP_NAMESPACE {

using namespace model_common;

EpbCounters::EpbCounters(int chipIndex, int pipeIndex, int epbIndex, int chanIndex) :
  chnl_parser_send_pkt_(
      epb_adapter(chnl_parser_send_pkt_, chipIndex, pipeIndex, epbIndex, chanIndex)),
  chnl_deparser_send_pkt_(
      epb_adapter(chnl_deparser_send_pkt_, chipIndex, pipeIndex, epbIndex, chanIndex)),
  chnl_warp_send_pkt_(
      epb_adapter(chnl_warp_send_pkt_, chipIndex, pipeIndex, epbIndex, chanIndex)),
  chnl_p2s_received_pkt_(
      epb_adapter(chnl_p2s_received_pkt_, chipIndex, pipeIndex, epbIndex, chanIndex)),
  // 36 bit wide counters
  wrap_counter_size_(36) {
}

void EpbCounters::reset() {
  chnl_parser_send_pkt_.reset();
  chnl_deparser_send_pkt_.reset();
  chnl_warp_send_pkt_.reset();
  chnl_p2s_received_pkt_.reset();
}

void EpbCounters::increment_chnl_parser_send_pkt() {
  chnl_parser_send_pkt_.epb_chnl_parser_send_pkt(
      Util::increment_and_wrap(chnl_parser_send_pkt_.epb_chnl_parser_send_pkt(),
                               wrap_counter_size_));
}

void EpbCounters::increment_chnl_parser_send_pkt_err() {
  chnl_parser_send_pkt_.err_pkt(
      Util::increment_and_wrap(chnl_parser_send_pkt_.err_pkt(),
                               wrap_counter_size_));
}

void EpbCounters::increment_chnl_deparser_send_pkt() {
  chnl_deparser_send_pkt_.epb_chnl_deparser_send_pkt(
      Util::increment_and_wrap(chnl_deparser_send_pkt_.epb_chnl_deparser_send_pkt(),
                               wrap_counter_size_));
}

}
