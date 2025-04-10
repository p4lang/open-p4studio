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

#include "gtest.h"
#include <utests/test_util.h>
#include <s2p.h>
#include <p2s.h>
#include <rmt-object-manager.h>
#include <mau-defs.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;


class BFN_TEST_NAME(S2p): public BaseTest {};

// S2P Pipe Mapping ONLY on JBay
TEST_F(BFN_TEST_NAME(S2p), CheckLogPhysPipeMapping) {
  Packet *pkt = om_->pkt_create();
  I2QueueingMetadata *i2q = pkt->i2qing_metadata();

  // Test all S2Ps - determined by inpipe
  for (int inpipe = 0; inpipe < 4; inpipe++) {
    S2p *s2p = om_->s2p_lookup(inpipe);
    auto s2p_regs = RegisterUtils::addr_s2p(inpipe);

    // ...iterate through all mapping increments..
    for (int mapinc = 0; mapinc < 4; mapinc++) {

      // ...iterate some possible values of S2P c2c CSR - 0/3/6/9/12/15
      // (This gets ORed into pipemask on JBayB0 for mirrored pkts with c2c set)
      for (uint8_t v_c2c = 0; v_c2c <= 0xF; v_c2c += 3) {

        int p[4] = { (0 + mapinc) % 4, (1 + mapinc) % 4, (2 + mapinc) % 4, (3 + mapinc) % 4 };
        uint32_t v_pipe_map = static_cast<uint32_t>(p[0] << 0) | (p[1] << 2) | (p[2] << 4) | (p[3] << 6);
        // Program up bits of s2p we care about
        // Note we don't insist pipe_map is 1-1 invertible
        tu_->OutWord((void*)&s2p_regs->pipe_map, v_pipe_map);
        tu_->OutWord((void*)&s2p_regs->copy2cpu, static_cast<uint32_t>(v_c2c));

        // ...iterate all possible values of outpipe
        for (int outpipe = 0; outpipe < 4; outpipe++) {

          // Put other 2 pipes (so not inpipe or outpipe) in base_pipemask
          uint8_t base_pipemask = 0xF & ~(1 << inpipe) & ~(1 << outpipe);
          // OR in v_c2c to form c2c_pipemask
          uint8_t c2c_pipemask = base_pipemask | v_c2c;

          // Now inject packets into S2P and check correct pipe mapping occurs
          // Try with mirror/copy_to_cpu off/on - and use all possible in/out port combinations

          for (uint8_t mirr = 0; mirr <= 1; mirr++) {
            for (uint8_t copy_to_cpu = 0; copy_to_cpu <= 1; copy_to_cpu++) {
              for (int inport = 0; inport <= 71; inport++) {
                for (int outport = 0; outport <= 71; outport++) {

                  i2q->reset();
                  i2q->set_physical_ingress_port(Port::make_port_index(inpipe, inport));
                  i2q->set_egress_unicast_port(Port::make_port_index(outpipe, outport));
                  i2q->set_pipe_mask(base_pipemask); // metadata gets base_pipemask
                  i2q->set_copy_to_cpu(copy_to_cpu);

                  bool mirrored = (mirr == 1);
                  bool b0_mirrored_c2c = RmtObject::is_jbayB0() && (mirr == 1) && (copy_to_cpu == 1);
                  // Internally S2P log_pipemask will be:
                  //   c2c_pipemask - if JBayB0 and mirrored pkt and c2c set
                  //  base_pipemask - otherwise
                  int log_pipemask = static_cast<int>( (b0_mirrored_c2c) ?c2c_pipemask :base_pipemask );
                  // So expected output pipemask will be log->phys mapping of log_pipemask
                  int exp_pipemask = ((log_pipemask << mapinc) & 0xF) | (log_pipemask >> (4-mapinc));

                  // Check bug emulation: JBayA0 also does log->phys mapping on physical_ingress_port!
                  int exp_inpipe = RmtObject::is_jbayA0() ?(inpipe + mapinc) % 4 :inpipe;
                  int exp_inport = Port::make_port_index(exp_inpipe, inport);

                  // Pipe in egress_uc_port always mapped
                  int exp_outpipe = (outpipe + mapinc) % 4;
                  int exp_outport = Port::make_port_index(exp_outpipe, outport);

                  s2p->map_logical_to_physical(pkt, mirrored);

                  EXPECT_EQ(exp_pipemask, i2q->pipe_mask());
                  EXPECT_EQ(exp_inport, i2q->physical_ingress_port());
                  EXPECT_EQ(exp_outport, i2q->egress_uc_port());
                }
              }
            }
          }
        }
      }
    }
  }
  om_->pkt_delete(pkt);
}


}
