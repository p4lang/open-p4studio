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

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <mau-hash-generator-with-reg.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool mh_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  TEST(BFN_TEST_NAME(MauHashTest),Basic) {
    if (mh_print) RMT_UT_LOG_INFO("test_mauhash_basic()\n");

    MauHashGenerator mhg;
    
    MauHashGenerator::InputT inputBytes;
    MauHashGenerator::ValidT validBits;
    MauHashGenerator::GaloisFieldMatrixT galois_field_matrix_bv;
    MauHashGenerator::GaloisFieldValidMatrixT galois_field_valid_matrix_bv;
    MauHashGenerator::Hashgroup_ConfigT  hashgroup_config_bv;
    MauHashGenerator::HashSeedT          hash_seed_bv;
    for (int i=0; i< MauHashGenerator::kOutputWidth; ++i ) {
      // gf for output 0 will be 0x01010101... for output 1: 0x02020202... etc...
      galois_field_matrix_bv[i].fill_all( i+1 );
      galois_field_valid_matrix_bv[i].fill_all( 0xff );
    }
    for (auto &hgc : hashgroup_config_bv) {
      hgc.fill_all_ones();
    }
    for (auto &hs : hash_seed_bv) {
      // hash seeds are all 0x0202020202...
      hs.fill_all( 0x02 );
    }

    if (mh_print) {
      for (auto hs : hash_seed_bv) {
        hs.print();
      }
    }

    //for (auto gf : galois_field_matrix_bv) gf.print();

    inputBytes.fill_all( 0x00 );
    validBits.fill_all( 0x00 );

    // group 0, output bit 0 should be 0 as input is 0 only hash seed makes a difference
    if (mh_print) RMT_UT_LOG_INFO("Test 1\n");
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );
    //     but output bit 1 should be 1 as hash seed bit 1 is set for all groups
    if (mh_print) RMT_UT_LOG_INFO("Test 2\n");
    EXPECT_EQ( 1, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 1 ) );

    inputBytes.set_bit(0); // should now make output bit 0 = 1
    if (mh_print) RMT_UT_LOG_INFO("Test 3\n");
    EXPECT_EQ( 1, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );

    // turning off the first first stage should make output bit 0 = 0 again
    if (mh_print) hashgroup_config_bv[0].print();
    hashgroup_config_bv[0].clear_bit(0);
    hashgroup_config_bv[0].clear_bit(1);
    if (mh_print) hashgroup_config_bv[0].print();
    
    if (mh_print) RMT_UT_LOG_INFO("Test 4\n");
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );

    // setting the input bit in the first first stage, nothing should change as it disabled
    inputBytes.set_bit(127); 
    //inputBytes.print();
    if (mh_print) RMT_UT_LOG_INFO("Test 5\n");
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );

    // set some valid bits, nothing should change again because not enabled
    validBits.set_bit(0); 
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );
    validBits.set_bit(8); 
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );
    validBits.set_bit(15); 
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );

    
    // setting the input bit in the second first stage -> output bit 0 = 1
    inputBytes.set_bit(128); // should now make output bit 0 = 1
    //inputBytes.print();
    if (mh_print) RMT_UT_LOG_INFO("Test 6\n");
    EXPECT_EQ( 1, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );

    // set some valid bits in second first stage, should flip the output
    //  Note: this works in JBay too even though it doesn't have valid bits! The valid bits are actually present in the jbay has generator,
    //    they are just never turned on in the level above.
    validBits.set_bit(16);
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );
    validBits.set_bit(21);
    EXPECT_EQ( 1, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );
    validBits.set_bit(31);
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );

    // this is in the third, so should not change 
    hashgroup_config_bv[0].clear_bit(2); // disable the 3rd
    validBits.set_bit(32);
    EXPECT_EQ( 0, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );
    // enable the 3rd, now it should change
    hashgroup_config_bv[0].set_bit(2); // enable the 3rd
    EXPECT_EQ( 1, mhg.CalculateOutputBit( inputBytes, validBits, galois_field_matrix_bv, galois_field_valid_matrix_bv, hashgroup_config_bv, hash_seed_bv, 0, 0 ) );

    
  }

}




