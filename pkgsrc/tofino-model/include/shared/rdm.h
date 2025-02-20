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

#ifndef _SHARED_RDM_
#define _SHARED_RDM_
#include <array>
#include <mutex>
#include <bitvector.h>
#include <rdm-defs.h>

namespace MODEL_CHIP_NAMESPACE {
  class Rdm {
  public:
    //
    // Structures representing the different node types.
    //
    struct RdmNode_L1Rid {
      uint32_t next_L1;
      uint32_t next_L2;
      uint16_t rid;
      bool rid_hash;
    };
    struct RdmNode_L1RidXid {
      uint32_t next_L1;
      uint32_t next_L2;
      uint16_t rid;
      uint16_t xid;
      bool rid_hash;
    };
    struct RdmNode_L1RidEnd {
      uint32_t next_L2;
      uint16_t rid;
      bool rid_hash;
    };
    struct RdmNode_L1EcmpPtr {
      uint32_t next_L1;
      uint32_t vector0;
      uint32_t vector1;
    };
    struct RdmNode_L1EcmpPtrXid {
      uint32_t next_L1;
      uint32_t vector0;
      uint32_t vector1;
      uint16_t xid;
    };
    struct RdmNode_L1EcmpVector {
      uint32_t base_L1;
      uint8_t  length;
      uint32_t vector;
    };
    struct RdmNode_L2Port16 {
      uint8_t chip;
      uint8_t pipe;
      bool last;
      uint8_t  spv;
      uint16_t ports;
    };
    struct RdmNode_L2Port64 {
      uint8_t chip;
      uint8_t pipe;
      bool last;
      uint8_t  spv;
      uint64_t ports;
    };
    struct RdmNode_L2Lag {
      uint32_t next_L2;
      uint8_t  lag_id;
    };
    //
    // Structure representing one row of the RDM which contains a single
    // 80-bit node or two 40-bit nodes.
    struct RdmLine {
      RdmDefs::RdmNodeType type[2] = {RdmDefs::RdmNodeType::kReset, RdmDefs::RdmNodeType::kReset};
      union {
        RdmNode_L1Rid        L1Rid;
        RdmNode_L1RidXid     L1RidXid;
        RdmNode_L1RidEnd     L1RidEnd[2];
        RdmNode_L1EcmpPtr    L1EcmpPtr;
        RdmNode_L1EcmpPtrXid L1EcmpPtrXid;
        RdmNode_L1EcmpVector L1EcmpVector;
        RdmNode_L2Port16     L2Port16[2];
        RdmNode_L2Port64     L2Port64;
        RdmNode_L2Lag        L2Lag[2];
      } u = {};
    };

    inline int addr_to_blk(int addr) const { return addr >> 12; }
    void encode(int addr, const BitVector<128> &data1);
    void to_bits(int row, uint64_t &hi, uint64_t &lo) const;
    void write_line(int addr, RdmLine l);
    void decode_L1(int addr, uint16_t hash, int ver,
                   uint32_t &next_L1, uint16_t &xid, bool &xid_valid,
                   uint16_t &rid, uint32_t &next_L2, bool &rid_hash) const;
    void decode_L2(int addr, uint32_t &next_L2, int &chip, int &pipe, uint8_t &spv,
                   uint64_t &port_vec, uint8_t &lag_id, bool &lag_valid) const;

    Rdm() { rdm_ = new std::array<RdmLine, RdmDefs::kRdmDepth>; }
    Rdm(const Rdm& other) = delete;  // XXX
    ~Rdm() { delete rdm_; }

  private:
    Rdm& operator=(const Rdm&){ return *this; } // XXX
    //
    // Decode functions per RDM node type.  Given the RDM address, extract all
    // the node specific fields into variables.
    void decode_L1Rid(int addr, uint32_t &next_L1, uint32_t &next_L2,
                      uint16_t &rid, bool &rid_hash) const;
    void decode_L1RidXid(int addr, uint32_t &next_L1, uint32_t &next_L2,
                         uint16_t &rid, uint16_t &xid, bool &rid_hash) const;
    void decode_L1RidEnd(int addr, uint32_t &next_L2, uint16_t &rid,
                         bool &rid_hash) const;
    void decode_L1EcmpPtr(int addr, uint32_t &next_L1, uint32_t &ptr0,
                          uint32_t &ptr1) const;
    void decode_L1EcmpPtrXid(int addr, uint32_t &next_L1, uint32_t &ptr0,
                             uint32_t &ptr1, uint16_t &xid) const;
    void decode_L1EcmpVector(int addr, uint32_t &base, uint8_t &length,
                             uint32_t &vec) const;
    void decode_L2Port16(int addr, uint64_t &ports, uint8_t &spv, int &chip, int &pipe, bool &last) const;
    void decode_L2Port64(int addr, uint64_t &ports, uint8_t &spv, int &chip, int &pipe, bool &last) const;
    void decode_L2Lag(int addr, uint32_t &next_L2, uint8_t &lag) const;
    // Given an ECMP group and hash value return the selected member.
    uint32_t ecmp_hash(uint16_t hash, uint32_t members, uint8_t len, uint32_t base) const;

    RdmDefs::RdmNodeType encode_type(int value) const;
    bool lineIsShareable(int line) const;
    RdmDefs::RdmNodeType node_type(int addr) const;
    bool typeIsHalfWidth(RdmDefs::RdmNodeType t) const;


    std::array<RdmLine, RdmDefs::kRdmDepth> *rdm_ = nullptr; // If not a ptr g++ uses way too much memory!
    mutable std::mutex                       rdm_mutex_ = {};
  };
}
#endif
