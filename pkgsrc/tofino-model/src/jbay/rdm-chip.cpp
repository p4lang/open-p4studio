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

  RdmDefs::RdmNodeType Rdm::encode_type(int value) const {
    switch (value) {
      case 0: // Invalidate
        return RdmDefs::RdmNodeType::kInvalid;
      case 1: // L1 RID
      case 13:
        return RdmDefs::RdmNodeType::kL1Rid;
      case 2: // L1 RID XID
      case 14:
        return RdmDefs::RdmNodeType::kL1RidXid;
      case 3: // L1 RID END
      case 15:
        return RdmDefs::RdmNodeType::kL1RidEnd;
      case 5: // L1 ECMP Ptr
        return RdmDefs::RdmNodeType::kL1EcmpPtr;
      case 6: // L1 ECMP Ptr XID
        return RdmDefs::RdmNodeType::kL1EcmpPtrXid;
      case 4: // L1 ECMP Vector
        return RdmDefs::RdmNodeType::kL1EcmpVector;
      case 8: // L2 Port 18
        return RdmDefs::RdmNodeType::kL2Port16;
      case 9: // L2 Port 72
        return RdmDefs::RdmNodeType::kL2Port64;
      case 12:// L2 LAG
        return RdmDefs::RdmNodeType::kL2Lag;
      default:
        RMT_ASSERT(0 && "Bad RDM Node Type");
    }
    return RdmDefs::RdmNodeType::kInvalid;
  }

  void Rdm::encode(int row, const BitVector<128> &data) {
    RMT_ASSERT (row >= 0 && row < RdmDefs::kRdmDepth);
    RMT_ASSERT(rdm_);
    // Pointer to the row being written.
    RdmLine *line = &((*rdm_)[row]);
    // The two types this row will hold.
    RdmDefs::RdmNodeType type0 = RdmDefs::RdmNodeType::kInvalid;
    RdmDefs::RdmNodeType type1 = RdmDefs::RdmNodeType::kInvalid;
    // Take a lock now to ensure no other thread is reading this row.
    std::lock_guard<std::mutex> lock(rdm_mutex_);

    type0 = encode_type(data.get_word(RdmDefs::kType1Shift, RdmDefs::kType1Width));
    if (typeIsHalfWidth(type0)) {
      type1 = encode_type(data.get_word(RdmDefs::kType2Shift, RdmDefs::kType2Width));
      // The second node in the row must also be a half-width node if the first
      // node was half-width.
      RMT_ASSERT(typeIsHalfWidth(type1));
    }

    // memset(line, 0, sizeof(struct RdmLine)); // GCC 9.3 did not like this so ...........
    // ... clear the existing RDM line by zeroising max size substruct RdmNode_L1EcmpPtrXid
    line->u.L1EcmpPtrXid.next_L1 = line->u.L1EcmpPtrXid.vector0 = line->u.L1EcmpPtrXid.vector1 = 0u;
    line->u.L1EcmpPtrXid.xid = 0;

    // Write the line
    line->type[0] = type0;
    line->type[1] = type1;
    switch (type0) {
      case RdmDefs::RdmNodeType::kL1Rid:
        line->u.L1Rid.next_L1  = data.get_word(RdmDefs::kL1RidL1NextShift, RdmDefs::kL1RidL1NextWidth);
        line->u.L1Rid.next_L2  = data.get_word(RdmDefs::kL1RidL2NextShift, RdmDefs::kL1RidL2NextWidth);
        line->u.L1Rid.rid      = data.get_word(RdmDefs::kL1RidRidShift, RdmDefs::kL1RidRidWidth);
        line->u.L1Rid.rid_hash = (data.get_word(RdmDefs::kType1Shift, RdmDefs::kType1Width) >> 2) == 3;
        break;
      case RdmDefs::RdmNodeType::kL1RidXid:
        line->u.L1RidXid.next_L1  = data.get_word(RdmDefs::kL1RidL1NextShift, RdmDefs::kL1RidL1NextWidth);
        line->u.L1RidXid.next_L2  = data.get_word(RdmDefs::kL1RidL2NextShift, RdmDefs::kL1RidL2NextWidth);
        line->u.L1RidXid.rid      = data.get_word(RdmDefs::kL1RidRidShift, RdmDefs::kL1RidRidWidth);
        line->u.L1RidXid.xid      = data.get_word(RdmDefs::kL1RidXidShift, RdmDefs::kL1RidXidWidth);
        line->u.L1RidXid.rid_hash = (data.get_word(RdmDefs::kType1Shift, RdmDefs::kType1Width) >> 2) == 3;
        break;
      case RdmDefs::RdmNodeType::kL1RidEnd:
        line->u.L1RidEnd[0].next_L2  = data.get_word(RdmDefs::kL1RidEndL2NextShift, RdmDefs::kL1RidEndL2NextWidth);
        line->u.L1RidEnd[0].rid      = data.get_word(RdmDefs::kL1RidEndRidShift, RdmDefs::kL1RidEndRidWidth);
        line->u.L1RidEnd[0].rid_hash = (data.get_word(RdmDefs::RdmDefs::kType1Shift, RdmDefs::kType1Width) >> 2) == 3;
        break;
      case RdmDefs::RdmNodeType::kL1EcmpPtr:
        line->u.L1EcmpPtr.next_L1 = data.get_word(RdmDefs::kL1EcmpPtrL1NextShift, RdmDefs::kL1EcmpPtrL1NextWidth);
        line->u.L1EcmpPtr.vector0 = data.get_word(RdmDefs::kL1EcmpPtrVector0Shift, RdmDefs::kL1EcmpPtrVector0Width);
        line->u.L1EcmpPtr.vector1 = data.get_word(RdmDefs::kL1EcmpPtrVector1Shift, RdmDefs::kL1EcmpPtrVector1Width);
        break;
      case RdmDefs::RdmNodeType::kL1EcmpPtrXid:
        line->u.L1EcmpPtrXid.next_L1 = data.get_word(RdmDefs::kL1EcmpPtrL1NextShift, RdmDefs::kL1EcmpPtrL1NextWidth);
        line->u.L1EcmpPtrXid.vector0 = data.get_word(RdmDefs::kL1EcmpPtrVector0Shift, RdmDefs::kL1EcmpPtrVector0Width);
        line->u.L1EcmpPtrXid.vector1 = data.get_word(RdmDefs::kL1EcmpPtrVector1Shift, RdmDefs::kL1EcmpPtrVector1Width);
        line->u.L1EcmpPtrXid.xid     = data.get_word(RdmDefs::kL1EcmpPtrXidShift, RdmDefs::kL1EcmpPtrXidWidth);
        break;
      case RdmDefs::RdmNodeType::kL1EcmpVector:
        line->u.L1EcmpVector.base_L1 = data.get_word(RdmDefs::kL1EcmpVecL1BaseShift, RdmDefs::kL1EcmpVecL1BaseWidth);
        line->u.L1EcmpVector.length  = data.get_word(RdmDefs::kL1EcmpVecLengthShift, RdmDefs::kL1EcmpVecLengthWidth);
        line->u.L1EcmpVector.vector  = data.get_word(RdmDefs::kL1EcmpVecVectorShift, RdmDefs::kL1EcmpVecVectorWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Port16:
	line->u.L2Port16[0].chip  = 0;
        line->u.L2Port16[0].pipe  = data.get_word(RdmDefs::kL2Port16PipeShift,  RdmDefs::kL2Port16PipeWidth);
        line->u.L2Port16[0].last  = data.get_word(RdmDefs::kL2Port16LastShift,  RdmDefs::kL2Port16LastWidth);
        line->u.L2Port16[0].spv   = data.get_word(RdmDefs::kL2Port16SpvShift,   RdmDefs::kL2Port16SpvWidth);
        line->u.L2Port16[0].ports = data.get_word(RdmDefs::kL2Port16PortsShift, RdmDefs::kL2Port16PortsWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Port64:
	line->u.L2Port64.chip  = 0;
        line->u.L2Port64.pipe  = data.get_word(RdmDefs::kL2Port64PipeShift,  RdmDefs::kL2Port64PipeWidth);
        line->u.L2Port64.last  = data.get_word(RdmDefs::kL2Port64LastShift,  RdmDefs::kL2Port64LastWidth);
        line->u.L2Port64.spv   = data.get_word(RdmDefs::kL2Port64SpvShift,   RdmDefs::kL2Port64SpvWidth);
        line->u.L2Port64.ports = data.get_word(RdmDefs::kL2Port64PortsShift, RdmDefs::kL2Port64PortsWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Lag:
        line->u.L2Lag[0].next_L2 = data.get_word(RdmDefs::kL2LagL2NextShift, RdmDefs::kL2LagL2NextWidth);
        line->u.L2Lag[0].lag_id  = data.get_word(RdmDefs::kL2LagLagIdShift,  RdmDefs::kL2LagLagIdWidth);
        break;
      default:
        break;
    }
    switch (type1) {
      case RdmDefs::RdmNodeType::kL1RidEnd:
        line->u.L1RidEnd[1].next_L2 = data.get_word(40+RdmDefs::kL1RidEndL2NextShift, RdmDefs::kL1RidEndL2NextWidth);
        line->u.L1RidEnd[1].rid     = data.get_word(40+RdmDefs::kL1RidEndRidShift, RdmDefs::kL1RidEndRidWidth);
        line->u.L1RidEnd[1].rid_hash = (data.get_word(RdmDefs::kType2Shift, RdmDefs::kType2Width) >> 2) == 3;
        break;
      case RdmDefs::RdmNodeType::kL2Port16:
	line->u.L2Port16[1].chip  = 0;
        line->u.L2Port16[1].pipe  = data.get_word(40+RdmDefs::kL2Port16PipeShift,  RdmDefs::kL2Port16PipeWidth);
        line->u.L2Port16[1].last  = data.get_word(40+RdmDefs::kL2Port16LastShift,  RdmDefs::kL2Port16LastWidth);
        line->u.L2Port16[1].spv   = data.get_word(40+RdmDefs::kL2Port16SpvShift,   RdmDefs::kL2Port16SpvWidth);
        line->u.L2Port16[1].ports = data.get_word(40+RdmDefs::kL2Port16PortsShift, RdmDefs::kL2Port16PortsWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Lag:
        line->u.L2Lag[1].next_L2 = data.get_word(40+RdmDefs::kL2LagL2NextShift, RdmDefs::kL2LagL2NextWidth);
        line->u.L2Lag[1].lag_id  = data.get_word(40+RdmDefs::kL2LagLagIdShift,  RdmDefs::kL2LagLagIdWidth);
        break;
      default:
        break;
    }
  }

  void Rdm::to_bits(int row, uint64_t &hi, uint64_t &lo) const {
    RMT_ASSERT (row >= 0 && row < RdmDefs::kRdmDepth);
    RMT_ASSERT(rdm_);
    // Pointer to the row being written.
    RdmLine *line = &((*rdm_)[row]);
    // Extract into a bitvector first for simplicity.
    BitVector<128> data;
    data.fill_all_zeros();
    // Take a lock now to ensure no other thread is writting this row.
    std::lock_guard<std::mutex> lock(rdm_mutex_);
    // Nodes types on the row.
    RdmDefs::RdmNodeType type0 = line->type[0];
    RdmDefs::RdmNodeType type1 = line->type[1];
    switch (type0) {
      case RdmDefs::RdmNodeType::kL1Rid:
        data.set_word(1, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        if (line->u.L1Rid.rid_hash)
          data.set_word(3, RdmDefs::kType1Shift+2, RdmDefs::kType1Width-2);
        data.set_word(line->u.L1Rid.next_L1, RdmDefs::kL1RidL1NextShift, RdmDefs::kL1RidL1NextWidth);
        data.set_word(line->u.L1Rid.next_L2, RdmDefs::kL1RidL2NextShift, RdmDefs::kL1RidL2NextWidth);
        data.set_word(line->u.L1Rid.rid,     RdmDefs::kL1RidRidShift,    RdmDefs::kL1RidRidWidth);
        break;
      case RdmDefs::RdmNodeType::kL1RidXid:
        data.set_word(2, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        if (line->u.L1RidXid.rid_hash)
          data.set_word(3, RdmDefs::kType1Shift+2, RdmDefs::kType1Width-2);
        data.set_word(line->u.L1RidXid.next_L1, RdmDefs::kL1RidL1NextShift, RdmDefs::kL1RidL1NextWidth);
        data.set_word(line->u.L1RidXid.next_L2, RdmDefs::kL1RidL2NextShift, RdmDefs::kL1RidL2NextWidth);
        data.set_word(line->u.L1RidXid.rid,     RdmDefs::kL1RidRidShift,    RdmDefs::kL1RidRidWidth);
        data.set_word(line->u.L1RidXid.xid,     RdmDefs::kL1RidXidShift,    RdmDefs::kL1RidXidWidth);
        break;
      case RdmDefs::RdmNodeType::kL1RidEnd:
        data.set_word(3, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        if (line->u.L1RidEnd[0].rid_hash)
          data.set_word(3, RdmDefs::kType1Shift+2, RdmDefs::kType1Width-2);
        data.set_word(line->u.L1RidEnd[0].next_L2, RdmDefs::kL1RidEndL2NextShift, RdmDefs::kL1RidEndL2NextWidth);
        data.set_word(line->u.L1RidEnd[0].rid,     RdmDefs::kL1RidEndRidShift,    RdmDefs::kL1RidEndRidWidth);
        break;
      case RdmDefs::RdmNodeType::kL1EcmpPtr:
        data.set_word(5, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        data.set_word(line->u.L1EcmpPtr.next_L1, RdmDefs::kL1EcmpPtrL1NextShift,  RdmDefs::kL1EcmpPtrL1NextWidth);
        data.set_word(line->u.L1EcmpPtr.vector0, RdmDefs::kL1EcmpPtrVector0Shift, RdmDefs::kL1EcmpPtrVector0Width);
        data.set_word(line->u.L1EcmpPtr.vector1, RdmDefs::kL1EcmpPtrVector1Shift, RdmDefs::kL1EcmpPtrVector1Width);
        break;
      case RdmDefs::RdmNodeType::kL1EcmpPtrXid:
        data.set_word(6, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        data.set_word(line->u.L1EcmpPtrXid.next_L1, RdmDefs::kL1EcmpPtrL1NextShift,  RdmDefs::kL1EcmpPtrL1NextWidth);
        data.set_word(line->u.L1EcmpPtrXid.vector0, RdmDefs::kL1EcmpPtrVector0Shift, RdmDefs::kL1EcmpPtrVector0Width);
        data.set_word(line->u.L1EcmpPtrXid.vector1, RdmDefs::kL1EcmpPtrVector1Shift, RdmDefs::kL1EcmpPtrVector1Width);
        data.set_word(line->u.L1EcmpPtrXid.xid,     RdmDefs::kL1EcmpPtrXidShift,     RdmDefs::kL1EcmpPtrXidWidth);
        break;
      case RdmDefs::RdmNodeType::kL1EcmpVector:
        data.set_word(4, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        data.set_word(line->u.L1EcmpVector.base_L1, RdmDefs::kL1EcmpVecL1BaseShift, RdmDefs::kL1EcmpVecL1BaseWidth);
        data.set_word(line->u.L1EcmpVector.length,  RdmDefs::kL1EcmpVecLengthShift, RdmDefs::kL1EcmpVecLengthWidth);
        data.set_word(line->u.L1EcmpVector.vector,  RdmDefs::kL1EcmpVecVectorShift, RdmDefs::kL1EcmpVecVectorWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Port16:
        data.set_word(8, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        data.set_word(line->u.L2Port16[0].pipe,  RdmDefs::kL2Port16PipeShift,  RdmDefs::kL2Port16PipeWidth);
        data.set_word(line->u.L2Port16[0].last,  RdmDefs::kL2Port16LastShift,  RdmDefs::kL2Port16LastWidth);
        data.set_word(line->u.L2Port16[0].spv,   RdmDefs::kL2Port16SpvShift,   RdmDefs::kL2Port16SpvWidth);
        data.set_word(line->u.L2Port16[0].ports, RdmDefs::kL2Port16PortsShift, RdmDefs::kL2Port16PortsWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Port64:
        data.set_word(9, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        data.set_word(line->u.L2Port64.pipe,  RdmDefs::kL2Port64PipeShift,  RdmDefs::kL2Port64PipeWidth);
        data.set_word(line->u.L2Port64.last,  RdmDefs::kL2Port64LastShift,  RdmDefs::kL2Port64LastWidth);
        data.set_word(line->u.L2Port64.spv,   RdmDefs::kL2Port64SpvShift,   RdmDefs::kL2Port64SpvWidth);
        data.set_word(line->u.L2Port64.ports, RdmDefs::kL2Port64PortsShift, RdmDefs::kL2Port64PortsWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Lag:
        data.set_word(12, RdmDefs::kType1Shift, RdmDefs::kType1Width);
        data.set_word(line->u.L2Lag[0].next_L2, RdmDefs::kL2LagL2NextShift, RdmDefs::kL2LagL2NextWidth);
        data.set_word(line->u.L2Lag[0].lag_id,  RdmDefs::kL2LagLagIdShift,  RdmDefs::kL2LagLagIdWidth);
        break;
      default:
        break;
    }
    switch (type1) {
      case RdmDefs::RdmNodeType::kL1RidEnd:
        data.set_word(3, RdmDefs::kType2Shift, RdmDefs::kType2Width);
        if (line->u.L1RidEnd[1].rid_hash)
          data.set_word(3, RdmDefs::kType2Shift+2, RdmDefs::kType2Width-2);
        data.set_word(line->u.L1RidEnd[1].next_L2, 40+RdmDefs::kL1RidEndL2NextShift, RdmDefs::kL1RidEndL2NextWidth);
        data.set_word(line->u.L1RidEnd[1].rid,     40+RdmDefs::kL1RidEndRidShift,    RdmDefs::kL1RidEndRidWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Port16:
        data.set_word(8, RdmDefs::kType2Shift, RdmDefs::kType2Width);
        data.set_word(line->u.L2Port16[1].pipe,  40+RdmDefs::kL2Port16PipeShift,  RdmDefs::kL2Port16PipeWidth);
        data.set_word(line->u.L2Port16[1].last,  40+RdmDefs::kL2Port16LastShift,  RdmDefs::kL2Port16LastWidth);
        data.set_word(line->u.L2Port16[1].spv,   40+RdmDefs::kL2Port16SpvShift,   RdmDefs::kL2Port16SpvWidth);
        data.set_word(line->u.L2Port16[1].ports, 40+RdmDefs::kL2Port16PortsShift, RdmDefs::kL2Port16PortsWidth);
        break;
      case RdmDefs::RdmNodeType::kL2Lag:
        data.set_word(12, RdmDefs::kType2Shift, RdmDefs::kType2Width);
        data.set_word(line->u.L2Lag[1].next_L2, 40+RdmDefs::kL2LagL2NextShift, RdmDefs::kL2LagL2NextWidth);
        data.set_word(line->u.L2Lag[1].lag_id,  40+RdmDefs::kL2LagLagIdShift,  RdmDefs::kL2LagLagIdWidth);
        break;
      default:
        break;
    }

    lo = data.get_word(0, 64);
    hi = data.get_word(64, 64);
  }

}
