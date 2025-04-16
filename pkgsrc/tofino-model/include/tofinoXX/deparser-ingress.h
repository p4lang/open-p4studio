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

#ifndef _TOFINOXX_DEPARSER_INGRESS_
#define _TOFINOXX_DEPARSER_INGRESS_

#include <deparser.h>
#include <deparser-metadata.h>
#include <packet-gen-metadata.h>

namespace MODEL_CHIP_NAMESPACE {

  class DeparserIngress : public Deparser {

 public:
    DeparserIngress(RmtObjectManager *om, int pipeIndex, int ioIndex,
                    DeparserReg &deparser_reg);

    Packet *Deparse(const Phv &phv, Packet **mirror_pkt,
                    Packet **resubmit_pkt,
                    PacketGenMetadata **packet_gen_metadata); // not used on tofino

    Packet *HandleResubmit(Packet* pkt, const Phv &phv, uint8_t drop_ctl,
                           Packet **resubmit_pkt);

 protected:
    uint8_t GetPktVersion(Packet *pkt);
    bool CheckDeparserPhvGroupConfig(const int &phv_idx);
    std::set<int> GetInvalidPhvs();

 private:
    Packet *SetMetadata(const Phv &phv, Packet *pkt);
    Packet *SetMetadataChip(const Phv &phv, Packet *pkt); // Chip specific metadata
    uint8_t GetDropControl(const Phv &phv);
  };
}

#endif // _TOFINOXX_DEPARSER_INGRESS_
