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

// Sbox implementation and sps14, 15, 18
#include <mau.h>
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-sbox.h>

// TODO: enable all sboxes (and remove the ifdefs)
//  Current status: until 5/5/15 all sboxes were disabled. DV complained about the hash distribution so
//   we enabled all switch boxes. But that causes tests to fail in p4factory/switch (3 LAG tests)
//   so I have enabled MAU_SBOX_15 (which is used by the hash distribution) and left the other two
//   disabled (which are used in the selector ALU and were the cause of the failing tests). Note
//   that those tests do rely on some scrambling from the code in this file, it is only adding
//   the sboxes that break them.

#define MAU_SBOX_14 1
#define MAU_SBOX_15 1
#define MAU_SBOX_18 1

namespace MODEL_CHIP_NAMESPACE {

uint8_t MauSbox::sbox3x3(uint8_t value)
{
  /* 3x3 sbox implements following logic.
   *   Qa = ~ab  OR ~ac OR bc
   *   Qb = ab OR ~ca OR ~cb
   *   Qc = ~ab OR ~a~c OR ~cb
   */

  // computed value as per above boolean logic 
  uint8_t  s3x3[8] = {1, 4, 7, 5, 2, 0, 3, 6}; // value = 0 becomes 1,
                                               // value = 2 becomes 4,
                                               // ....
  RMT_ASSERT (value < 8);
  return (s3x3[value]);
}

uint8_t MauSbox::sbox4x4(uint8_t value)
{
  /* 4x4 sbox implements following logic.
   *    Qa = a~b~c~d  OR ~ab OR ~a~c~d OR bc
   *    Qb = a~bd OR ~ac~d OR ~cb
   *    Qc = a~bc OR a~b~d OR ~ab~d OR ~ac~d OR bc~d
   *    Qd = a~bc OR ~ab~c OR ab~d OR ~acd
   */
  // computed value as per above boolean logic 
  uint8_t s4x4[16] = {8, 0, 6, 1, 15, 13, 14, 11, 2, 12, 3, 7, 5, 4, 9, 10};
  RMT_ASSERT (value < 16);
  return (s4x4[value]);
}



uint16_t  MauSbox::sps14(uint16_t  hash)
{
  // SPS14 = 4x4, 4x4, 3x3, 3x3
  // Permutation positions
  uint8_t   sps14_matrix_sbox0[14] = {2, 9, 10, 3, 6, 11, 0, 4, 7, 12, 1, 5, 8, 13};
  uint8_t   sps14_matrix_sbox1[14] = {6, 10, 0, 3, 7, 11, 1, 4, 8, 12, 2, 5, 9, 13};
  // Apply stage1 sbox
#ifdef MAU_SBOX_14
  uint8_t sb4x4_1 = sbox4x4((hash >> 10) & 0xf);
  uint8_t sb4x4_2 = sbox4x4((hash >> 6) & 0xf);
  uint8_t sb3x3_1 = sbox3x3((hash >> 3) & 0x7);
  uint8_t sb3x3_2 = sbox3x3((hash) & 0x7);
  uint16_t stage1_hash = (sb4x4_1 << 10) | (sb4x4_2 << 6) | (sb3x3_1 << 3) | (sb3x3_2);
#else
  uint16_t stage1_hash = hash;
#endif

  uint16_t  scrambled_hash = 0;
  for (int i = 0; i < 14; i++) {

                        /* "hash & (1 << sps14_matrix_[i])" =  Get right bit from hash */
                        /* ">> sps14_matrix_[i]"            = shift into bit0 */
                        /* "<< i"                           = move original hash bit
                                                              into correct position in
                                                              scrambled hash */
    scrambled_hash |= (((stage1_hash & (1 << sps14_matrix_sbox0[i])) >> sps14_matrix_sbox0[i]) << i);
  }

  // Apply stage2 sbox
#ifdef MAU_SBOX_14
  sb4x4_1 = sbox4x4((scrambled_hash >> 10) & 0xf);
  sb4x4_2 = sbox4x4((scrambled_hash >> 6) & 0xf);
  sb3x3_1 = sbox3x3((scrambled_hash >> 3) & 0x7);
  sb3x3_2 = sbox3x3((scrambled_hash) & 0x7);
  uint16_t stage2_hash = (sb4x4_1 << 10) | (sb4x4_2 << 6) | (sb3x3_1 << 3) | (sb3x3_2);
#else
  uint16_t stage2_hash = scrambled_hash;
#endif
  scrambled_hash = 0;
  for (int i = 0; i < 14; i++) {
    scrambled_hash |= (((stage2_hash & (1 << sps14_matrix_sbox1[i])) >> sps14_matrix_sbox1[i]) << i);
  }
  return (scrambled_hash);
}




uint32_t  MauSbox::sps18(uint32_t  hash)
{
  // SPS14 = 4x4, 4x4, 4x4, 3x3, 3x3
  uint8_t   sps18_matrix_sbox0[18] = {6,13,14, 0, 5, 7, 10, 15, 1,
                                      3, 8, 11, 16, 2, 4, 9, 12, 17};
  uint8_t   sps18_matrix_sbox1[18] = {6, 10, 14, 0, 3, 7, 11, 15, 1,
                                      4, 8, 12, 16, 2, 5, 9, 13, 17};


  // Apply stage1 sbox
#ifdef MAU_SBOX_18
  uint8_t sb4x4_1 = sbox4x4((hash >> 14) & 0xf);
  uint8_t sb4x4_2 = sbox4x4((hash >> 10) & 0xf);
  uint8_t sb4x4_3 = sbox4x4((hash >> 6) & 0xf);
  uint8_t sb3x3_1 = sbox3x3((hash >> 3) & 0x7);
  uint8_t sb3x3_2 = sbox3x3((hash) & 0x7);

  uint32_t stage1_hash = (sb4x4_1 << 14) | (sb4x4_2 << 10) | (sb4x4_3 << 6) | (sb3x3_1 << 3) | (sb3x3_2);
#else
  uint32_t stage1_hash = hash;
#endif

  uint32_t  scrambled_hash = 0;
  for (int i = 0; i < 18; i++) {

                        /* "hash & (1 << sps18_matrix_[i])" =  Get right bit from hash */
                        /* ">> sps18_matrix_[i]"            = shift into bit0 */
                        /* "<< i"                           = move original hash bit
                                                              into correct position in
                                                              scrambled hash */
    scrambled_hash |= (((stage1_hash & (1 << sps18_matrix_sbox0[i])) >> sps18_matrix_sbox0[i]) << i);
  }

  // Apply stage2 sbox
#ifdef MAU_SBOX_18
  sb4x4_1 = sbox4x4((scrambled_hash >> 14) & 0xf);
  sb4x4_2 = sbox4x4((scrambled_hash >> 10) & 0xf);
  sb4x4_3 = sbox4x4((scrambled_hash >> 6) & 0xf);
  sb3x3_1 = sbox3x3((scrambled_hash >> 3) & 0x7);
  sb3x3_2 = sbox3x3((scrambled_hash) & 0x7);

  uint32_t stage2_hash = (sb4x4_1 << 14) | (sb4x4_2 << 10) | (sb4x4_3 << 6) | (sb3x3_1 << 3) | (sb3x3_2);
#else
  uint32_t stage2_hash = scrambled_hash;
#endif

  scrambled_hash = 0;
  for (int i = 0; i < 18; i++) {
    scrambled_hash |= (((stage2_hash & (1 << sps18_matrix_sbox1[i])) >> sps18_matrix_sbox1[i]) << i);
  }
  return (scrambled_hash);
}


uint16_t  MauSbox::sps15(uint16_t  hash)
{
  // SPS14 = 4x4, 4x4, 4x4, 3x3
  uint8_t   sps15_matrix_sbox0[15] = {5, 10, 11, 2, 6, 7, 12, 0, 3, 8, 13, 1, 4, 9, 14};
  uint8_t   sps15_matrix_sbox1[15] = {3, 7, 11, 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14};
  uint16_t  scrambled_hash = 0;

#ifdef MAU_SBOX_15
 // Apply stage1 sbox
  uint8_t sb4x4_1 = sbox4x4((hash >> 11) & 0xf);
  uint8_t sb4x4_2 = sbox4x4((hash >> 7) & 0xf);
  uint8_t sb4x4_3 = sbox4x4((hash >> 3) & 0xf);
  uint8_t sb3x3_1 = sbox3x3((hash) & 0x7);
  uint32_t stage1_hash = (sb4x4_1 << 11) | (sb4x4_2 << 7) | (sb4x4_3 << 3) | (sb3x3_1);
#else
  uint32_t stage1_hash = hash;
#endif
  for (int i = 0; i < 15; i++) {
    scrambled_hash |= ((stage1_hash >> sps15_matrix_sbox0[i]) & 1) << i;
  }
  // Apply stage2 sbox
#ifdef MAU_SBOX_15
  sb4x4_1 = sbox4x4(scrambled_hash >> 11);
  sb4x4_2 = sbox4x4((scrambled_hash >> 7) & 0xf);
  sb4x4_3 = sbox4x4((scrambled_hash >> 3) & 0xf);
  sb3x3_1 = sbox3x3((scrambled_hash) & 0x7);
  uint32_t stage2_hash = (sb4x4_1 << 11) | (sb4x4_2 << 7) | (sb4x4_3 << 3) | (sb3x3_1);
#else
  uint32_t stage2_hash = scrambled_hash;
#endif

  scrambled_hash = 0;
  for (int i = 0; i < 15; i++) {
    scrambled_hash |= ((stage2_hash >> sps15_matrix_sbox1[i]) & 1) << i;
  }
   return (scrambled_hash);
}

}; // namespace MODEL_CHIP_NAMESPACE
