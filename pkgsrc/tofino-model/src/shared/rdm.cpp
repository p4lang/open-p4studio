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

#include <cinttypes>
#include <string.h>
#include <thread>
#include <rdm.h>

namespace MODEL_CHIP_NAMESPACE {

  bool Rdm::lineIsShareable(int line) const {
    RMT_ASSERT(line >= 0 && line < RdmDefs::kRdmDepth);
    RMT_ASSERT(rdm_);
    // If lower half of the line is not empty or a half width node type then
    // the line cannot be shared.
    if (!(RdmDefs::RdmNodeType::kInvalid  == (*rdm_)[line].type[0] ||
          RdmDefs::RdmNodeType::kL1RidEnd == (*rdm_)[line].type[0] ||
          RdmDefs::RdmNodeType::kL2Port16 == (*rdm_)[line].type[0] ||
          RdmDefs::RdmNodeType::kL2Lag    == (*rdm_)[line].type[0])) {
      return false;
    }
    // The lower half is either empty or contains a half width node.
    // If the upper half is empty the line is shareable.
    if (RdmDefs::RdmNodeType::kInvalid == (*rdm_)[line].type[1]) {
      return true;
    }
    // The upper half contains a node, therefore the node must be a half
    // width node.
    RMT_ASSERT(RdmDefs::RdmNodeType::kL1RidEnd == (*rdm_)[line].type[1] ||
	       RdmDefs::RdmNodeType::kL2Port16 == (*rdm_)[line].type[1] ||
	       RdmDefs::RdmNodeType::kL2Lag    == (*rdm_)[line].type[1]);
    return true;
  }

  void Rdm::write_line(int addr, RdmLine l) {
    RMT_ASSERT(addr >= 0 && addr < RdmDefs::kRdmDepth);
    RMT_ASSERT(rdm_);

    // Take a lock now to ensure no other thread is writing this row.
    std::lock_guard<std::mutex> lock(rdm_mutex_);

    (*rdm_)[addr] = l;
  }

  RdmDefs::RdmNodeType Rdm::node_type(int addr) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount);
    RMT_ASSERT(rdm_);
    return (*rdm_)[addr/2].type[addr & 1];
  }

  bool Rdm::typeIsHalfWidth(RdmDefs::RdmNodeType t) const {
    return RdmDefs::RdmNodeType::kInvalid == t ||
           RdmDefs::RdmNodeType::kL1RidEnd == t ||
           RdmDefs::RdmNodeType::kL2Port16 == t ||
           RdmDefs::RdmNodeType::kL2Lag == t;
  }

  void Rdm::decode_L1Rid(int addr, uint32_t &next_L1, uint32_t &next_L2,
                         uint16_t &rid, bool &rid_hash) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount && !(addr & 1));
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL1Rid == (*rdm_)[addr/2].type[addr & 1]);
    next_L1 = (*rdm_)[addr/2].u.L1Rid.next_L1;
    next_L2 = (*rdm_)[addr/2].u.L1Rid.next_L2;
    rid     = (*rdm_)[addr/2].u.L1Rid.rid;
    rid_hash = (*rdm_)[addr/2].u.L1Rid.rid_hash;
  }
  void Rdm::decode_L1RidXid(int addr, uint32_t &next_L1, uint32_t &next_L2,
                            uint16_t &rid, uint16_t &xid, bool &rid_hash) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount && !(addr & 1));
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL1RidXid == (*rdm_)[addr/2].type[addr & 1]);
    next_L1 = (*rdm_)[addr/2].u.L1RidXid.next_L1;
    next_L2 = (*rdm_)[addr/2].u.L1RidXid.next_L2;
    rid     = (*rdm_)[addr/2].u.L1RidXid.rid;
    xid     = (*rdm_)[addr/2].u.L1RidXid.xid;
    rid_hash= (*rdm_)[addr/2].u.L1RidXid.rid_hash;
  }
  void Rdm::decode_L1RidEnd(int addr, uint32_t &next_L2, uint16_t &rid,
                            bool &rid_hash) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount);
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL1RidEnd == (*rdm_)[addr/2].type[addr & 1]);
    next_L2 = (*rdm_)[addr/2].u.L1RidEnd[addr & 1].next_L2;
    rid     = (*rdm_)[addr/2].u.L1RidEnd[addr & 1].rid;
    rid_hash= (*rdm_)[addr/2].u.L1RidEnd[addr & 1].rid_hash;
  }
  void Rdm::decode_L1EcmpPtr(int addr, uint32_t &next_L1, uint32_t &ptr0,
                             uint32_t &ptr1) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount && !(addr & 1));
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL1EcmpPtr == (*rdm_)[addr/2].type[addr & 1]);
    next_L1 = (*rdm_)[addr/2].u.L1EcmpPtr.next_L1;
    ptr0    = (*rdm_)[addr/2].u.L1EcmpPtr.vector0;
    ptr1    = (*rdm_)[addr/2].u.L1EcmpPtr.vector1;
  }
  void Rdm::decode_L1EcmpPtrXid(int addr, uint32_t &next_L1, uint32_t &ptr0,
                                uint32_t &ptr1, uint16_t &xid) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount && !(addr & 1));
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL1EcmpPtrXid == (*rdm_)[addr/2].type[addr & 1]);
    next_L1 = (*rdm_)[addr/2].u.L1EcmpPtrXid.next_L1;
    ptr0    = (*rdm_)[addr/2].u.L1EcmpPtrXid.vector0;
    ptr1    = (*rdm_)[addr/2].u.L1EcmpPtrXid.vector1;
    xid     = (*rdm_)[addr/2].u.L1EcmpPtrXid.xid;
  }
  void Rdm::decode_L1EcmpVector(int addr, uint32_t &base, uint8_t &length,
                                uint32_t &vec) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount && !(addr & 1));
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL1EcmpVector == (*rdm_)[addr/2].type[addr & 1]);
    base    = (*rdm_)[addr/2].u.L1EcmpVector.base_L1;
    length  = (*rdm_)[addr/2].u.L1EcmpVector.length;
    vec     = (*rdm_)[addr/2].u.L1EcmpVector.vector;
  }
  void Rdm::decode_L2Port16(int addr, uint64_t &ports, uint8_t &spv,
                            int &chip, int &pipe, bool &last) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount);
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL2Port16 == (*rdm_)[addr/2].type[addr & 1]);
    uint64_t x = (*rdm_)[addr/2].u.L2Port16[addr & 1].ports;
    uint8_t  y = (*rdm_)[addr/2].u.L2Port16[addr & 1].spv;
    chip       = (*rdm_)[addr/2].u.L2Port16[addr & 1].chip;
    pipe       = (*rdm_)[addr/2].u.L2Port16[addr & 1].pipe;
    last       = (*rdm_)[addr/2].u.L2Port16[addr & 1].last;
    ports = 0;

    for (int i=0; i<16; ++i) {
      ports |= ((uint64_t) ((x >> i) & 1)) << (4*i);
    }
    spv = 0;
    for (int i=0; i<2; ++i) {
      spv |= ((y >> i) & 1) << (4*i);
    }
  }
  void Rdm::decode_L2Port64(int addr, uint64_t &ports, uint8_t &spv,
                            int &chip, int &pipe, bool &last) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount && (!(addr & 1)));
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL2Port64 == (*rdm_)[addr/2].type[addr & 1]);
    ports = (*rdm_)[addr/2].u.L2Port64.ports;
    spv   = (*rdm_)[addr/2].u.L2Port64.spv;
    chip  = (*rdm_)[addr/2].u.L2Port64.chip;
    pipe  = (*rdm_)[addr/2].u.L2Port64.pipe;
    last  = (*rdm_)[addr/2].u.L2Port64.last;
  }
  void Rdm::decode_L2Lag(int addr, uint32_t &next_L2, uint8_t &lag) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::RdmDefs::kRdmNodeCount);
    RMT_ASSERT(rdm_);
    RMT_ASSERT (RdmDefs::RdmNodeType::kL2Lag == (*rdm_)[addr/2].type[addr & 1]);
    next_L2 = (*rdm_)[addr/2].u.L2Lag[addr & 1].next_L2;
    lag     = (*rdm_)[addr/2].u.L2Lag[addr & 1].lag_id;
  }

  uint32_t Rdm::ecmp_hash(uint16_t hash, uint32_t members, uint8_t len,
                          uint32_t base) const {
    // Mask off members based on the configured length.  Note that the "len"
    // argument is zero based so a value of 31 corresponds to a length of 32.
    if (len < 31) {
      uint32_t mask = (1 << (len+1)) - 1;
      members = members & mask;
    }

    // If all member bits are zero, then the base pointer is treated as a
    // next pointer.
    if (!members) return base;

    // Note that len is zero based (2 ECMP members have a len of 1).  First
    // try to select the member by taking a mod of the hash for the number of
    // ECMP members and then checking if the corresponding bit (counting from
    // the lsb) is set.  If it is set, that is the selected ECMP member.
    // If the selected bit is not set, then count the number of members
    // (which is the number of bits set), take a mod of the hash using that
    // count, then use the resulting value as the index into a packed bit
    // array of all the set bits.  For example binary 00110100 would have a
    // length of 3, the index to the array would be hash % 3, the array would
    // be 00000111 (all set bits are packed to the lsbs).
    unsigned indx = hash % (len+1);
    if ((members >> indx) & 1) {
      return base + indx;
    } else {
      // Count the number of bits set in members:
      uint32_t c = __builtin_popcount(members);
      RMT_ASSERT(c < 32 && c);
      indx = hash % c;
      // Rather than packing and unpacking the bits, just find the n'th bit
      // set.
      ++indx; // Extra increment because the loop below decrements and then
              // checks for zero.
      unsigned i=0;
      for (i=0; indx && i<32; ++i) {
        if ((members >> i) & 1) --indx;
        if (!indx) break;
      }
      return base + i;
    }
  }

  void Rdm::decode_L1(int addr, uint16_t hash, int ver,
		      uint32_t &next_L1, uint16_t &xid, bool &xid_valid,
		      uint16_t &rid, uint32_t &next_L2, bool &rid_hash) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount);
    RMT_ASSERT(ver == 0 || ver == 1);
    uint32_t ptr0 = 0, ptr1 = 0;
    uint32_t base = 0, vec = 0;
    uint8_t len = 0;

    // Take a lock now to ensure no other thread is writing this row.
    std::lock_guard<std::mutex> lock(rdm_mutex_);

    RdmDefs::RdmNodeType type = node_type(addr);
    next_L1 = 0;
    xid = 0;
    xid_valid = false;
    rid = 0;
    next_L2 = 0;
    rid_hash = false;
    switch (type) {
      case RdmDefs::RdmNodeType::kL1Rid:
        decode_L1Rid(addr, next_L1, next_L2, rid, rid_hash);
        break;
      case RdmDefs::RdmNodeType::kL1RidXid:
        xid_valid = true;
        decode_L1RidXid(addr, next_L1, next_L2, rid, xid, rid_hash);
        break;
      case RdmDefs::RdmNodeType::kL1RidEnd:
        decode_L1RidEnd(addr, next_L2, rid, rid_hash);
        break;
      case RdmDefs::RdmNodeType::kL1EcmpPtr:
        decode_L1EcmpPtr(addr, next_L1, ptr0, ptr1);
        decode_L1EcmpVector(ver ? ptr1 : ptr0, base, len, vec);
        base = ecmp_hash(hash, vec, len, base);
        decode_L1RidEnd(base, next_L2, rid, rid_hash);
        break;
      case RdmDefs::RdmNodeType::kL1EcmpPtrXid:
        xid_valid = true;
        decode_L1EcmpPtrXid(addr, next_L1, ptr0, ptr1, xid);
        decode_L1EcmpVector(ver ? ptr1 : ptr0, base, len, vec);
        base = ecmp_hash(hash, vec, len, base);
        decode_L1RidEnd(base, next_L2, rid, rid_hash);
        break;
      default:
//      LOG_ERROR("Invalid L1 node type (%d) at addr 0x%x at %s:%d",
//                 type, addr, __PRETTY_FUNCTION__, __LINE__);
        RMT_ASSERT(0 && "Invalid L1 node type");
        break;
    }
  }

  void Rdm::decode_L2(int addr, uint32_t &next_L2, int &chip, int &pipe, 
		      uint8_t &spv, uint64_t &port_vec, uint8_t &lag_id, bool &lag_valid) const {
    RMT_ASSERT (addr >= 0 && addr < RdmDefs::kRdmNodeCount);
    bool last;
    next_L2 = 0;
    spv = 0;
    port_vec = 0;
    chip = 0;
    pipe = 0;
    lag_id = 0;
    lag_valid = false;

    // Take a lock now to ensure no other thread is writing this row.
    std::lock_guard<std::mutex> lock(rdm_mutex_);

    RdmDefs::RdmNodeType type = node_type(addr);
    switch (type) {
      case RdmDefs::RdmNodeType::kL2Port16:
        decode_L2Port16(addr, port_vec, spv, chip, pipe, last);
        if (!last) {
          next_L2 = addr + 1;
        }
        break;
      case RdmDefs::RdmNodeType::kL2Port64:
        decode_L2Port64(addr, port_vec, spv, chip, pipe, last);
        if (!last) {
          next_L2 = addr + 2;
        }
        break;
      case RdmDefs::RdmNodeType::kL2Lag:
        decode_L2Lag(addr, next_L2, lag_id);
        lag_valid = true;
        break;
      default:
//      LOG_ERROR("Invalid L2 node type (%d) at addr 0x%x at %s:%d",
//                 type, addr, __PRETTY_FUNCTION__, __LINE__);
        RMT_ASSERT(0 && "Invalid L2 node type");
    }
  }

}
