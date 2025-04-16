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

#include <mirror.h>

namespace MODEL_CHIP_NAMESPACE {

void Mirror::SetI2QmetadataChip(Packet *pkt,
                                MirrorSessionReg &sess) {
  MirrorMetadata *mm = pkt->mirror_metadata();
  I2QueueingMetadata *i2q = pkt->i2qing_metadata();
  // MC_CTRL cfg:
  // if the cfg bit is set to choose from MAU and mc_ctrl is 1, then
  //    multicast information (mgid1, mgid2, pipe_vec) are from the per-session register.
  // If the cfg bit is set to choose from MAU and mc_ctrl is 0, then
  //    mcid_vld1, mcid_vld2 and pipe_vec are 0 (not multicast)
  // If the cfg bit is 0, then
  //    mcid_vld1, mcid_vld2 and pipe_vec are from the per-session register.
  //
  uint8_t mcid1_vld = sess.mcid1_vld(), mcid2_vld = sess.mcid2_vld();
  uint8_t pipe_vec = sess.pipe_vec();
  if ((sess.mc_cfg() & 1) == 1) {
    if (mm->mirr_mc_ctrl() == 0) { mcid1_vld = 0; mcid2_vld = 0; pipe_vec = 0; }
  }
  if (mcid1_vld == 1) i2q->set_mgid1( sess.mcid1_id() );
  if (mcid2_vld == 1) i2q->set_mgid2( sess.mcid2_id() );
  i2q->set_pipe_mask( pipe_vec );
}


}

