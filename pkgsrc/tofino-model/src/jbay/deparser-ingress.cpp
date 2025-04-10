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

#include <deparser-ingress.h>
#include <deparser-reg.h>

namespace MODEL_CHIP_NAMESPACE {

void DeparserIngress::SetPipeVector(Packet *pkt) {
  I2QueueingMetadata *i2q_md = pkt->i2qing_metadata();
  // calculate pipe mask bits

  uint16_t multicast_pipe_vector = 0;
  if (i2q_md->has_mgid1()) {
    multicast_pipe_vector |= deparser_reg_.get_multicast_pipe_vector(0, i2q_md->mgid1());
    RMT_LOG_VERBOSE("DEPARSER::deparse pipe_vector after mgid1=%x\n", multicast_pipe_vector );
  }
  if (i2q_md->has_mgid2()) {
    multicast_pipe_vector |= deparser_reg_.get_multicast_pipe_vector(1, i2q_md->mgid2());
    RMT_LOG_VERBOSE("DEPARSER::deparse pipe_vector after mgid2=%x\n", multicast_pipe_vector );
  }
  if (i2q_md->cpu_needs_copy()) {
    multicast_pipe_vector |= static_cast<uint16_t>(deparser_reg_.get_copy_to_cpu_pipe_vector());
    RMT_LOG_VERBOSE("DEPARSER::deparse pipe_vector after copy to cpu=%x\n", multicast_pipe_vector );
  }
  i2q_md->set_multicast_pipe_vector(multicast_pipe_vector);
  RMT_LOG_VERBOSE("DEPARSER::deparse pipe_vector final=%x\n", multicast_pipe_vector );
}

}
