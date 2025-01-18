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

#include <rmt-object-manager.h>
#include <mau.h>
#include <mau-input-xbar-with-reg.h>
#include <model_core/model.h>
#include <register_includes/reg.h> // for with registers test
#include "input_xbar_util.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

void Break1() {
}

void ResetModel() {
  // Reset Model in case we have register subscribers from some previous tests
  GLOBAL_MODEL->Reset();
}

void FillPhvWithTestPattern(Phv *phv) {
  // Fill phv words:
  //  32 bit words:
  //          03020100
  //          04030201
  //          05040302
  //          06050403 etc...
  //  Similar for 16 bits. Eight bits is just the PHV word number
  for (int i=0; i < Phv::kWordsMax; ++i) {
    int size = phv->which_width_in_bytes(i);
    switch (size) {
      case 1:
        phv->set(i,i);
        break;
      case 2:
        phv->set(i,((i+1)<<8) | i);
        break;
      case 4:
        phv->set(i,((i+3)<<24) | ((i+2)<<16) | ((i+1)<<8) | i);
        break;
      default:
        ASSERT_TRUE(0);
        break;
    }
  }
}


  bool ix_print = false;

  using namespace std;

  TEST(BFN_TEST_NAME(MauInputXbar),Basic) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_basic()\n");

    MauInputXbar mix;
    RmtObjectManager om;
    Phv phv(&om);
    uint8_t output_byte=0;
    bool  output_valid=0;
    FillPhvWithTestPattern(&phv);

    mix.CalculateOutputByte(phv, 0, 0, true, &output_byte, &output_valid );
    ASSERT_TRUE( output_byte == 0  &&  output_valid == true);

    mix.CalculateOutputByte(phv, 1, 0, true, &output_byte, &output_valid );
    ASSERT_TRUE( output_byte == 1  &&  output_valid == true);
  }


  TEST(BFN_TEST_NAME(MauInputXbar),Words32) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_words_32()\n");

    MauInputXbar mix;
    RmtObjectManager om;
    Phv phv(&om);
    uint8_t output_byte=0;
    bool  output_valid=0;
    FillPhvWithTestPattern(&phv);

    // For all of the output bytes (grouped by word)
    // check all of the 32 bit phv words (numbered 0->63) and with in them each
    //  byte (which is selected by the output byte within a word)
    for (int ow=0; ow<MauInputXbarWithReg::kOutputBytes; ow+=4) {
      for (int which_phv_word=0;which_phv_word<64;++which_phv_word) {
        for (int ob=0;ob<4;++ob) {
          int which_output_byte = ob+ow;
          mix.CalculateOutputByte(phv, which_phv_word, which_output_byte, true, &output_byte, &output_valid );
          ASSERT_TRUE( output_byte == (ob+which_phv_word)  &&  output_valid == true);

        }
      }
    }
  }

  TEST(BFN_TEST_NAME(MauInputXbar),Words8) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_words_8()\n");

    MauInputXbar mix;
    RmtObjectManager om;
    Phv phv(&om);
    uint8_t output_byte=0;
    bool  output_valid=0;
    FillPhvWithTestPattern(&phv);
    // For all of the output bytes
    // check all of the 8 bit phv words (numbered 64->127)
    for (int ob=0; ob<MauInputXbarWithReg::kOutputBytes; ob+=4) {
      for (int which_phv_word=64;which_phv_word<127;++which_phv_word) {
        int which_output_byte = ob;
        mix.CalculateOutputByte(phv, which_phv_word, which_output_byte, true, &output_byte, &output_valid );
        ASSERT_TRUE( output_byte == which_phv_word  &&  output_valid == true);
      }
    }
  }

  TEST(BFN_TEST_NAME(MauInputXbar),Words16) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_words_16()\n");

    MauInputXbar mix;
    RmtObjectManager om;
    Phv phv(&om);
    uint8_t output_byte=0;
    bool  output_valid=0;
    FillPhvWithTestPattern(&phv);
    // For all of the output bytes (grouped by 16 bit word)
    // check all of the 16 bit phv words (numbered 128->223) and with in them each
    //  byte (which is selected by the output byte within a word)
    for (int ow=0; ow<MauInputXbarWithReg::kOutputBytes; ow+=2) {
      for (int which_phv_word=128;which_phv_word<223;++which_phv_word) {
        for (int ob=0;ob<2;++ob) {
          int which_output_byte = ob+ow;
          mix.CalculateOutputByte(phv, which_phv_word, which_output_byte, true, &output_byte, &output_valid );
          ASSERT_TRUE( output_byte == (ob+which_phv_word)  &&  output_valid == true);

        }
      }
    }
  }


  TEST(BFN_TEST_NAME(MauInputXbar),Words32UsingRegisters) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_words_32_using_registers()\n");

    ResetModel();
    TestUtil tu(GLOBAL_MODEL.get(),0,0,0);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    // test the string_map_lookup stuff while we're here!
    Mau::kMauDinPowerMode = false;
    EXPECT_TRUE( Mau::kMauDinPowerMode == false );
    om->string_map_lookup("MauDinPowerMode","true");
    EXPECT_TRUE( Mau::kMauDinPowerMode == true );
    om->string_map_lookup("MauDinPowerMode","false");
    EXPECT_TRUE( Mau::kMauDinPowerMode == false );

    const MauConfig mau_config{ 0 };
    Mau mau(om,0,0,mau_config);
    MauInputXbarWithReg mix(0/*chip*/,0/*pipe*/,0/*mau*/,&mau);
    tu.set_phv_range_all(0,0,true); // Set all PHV words in ingress thread

    Phv phv(om);
    uint8_t output_byte=0;
    bool  output_valid=0;
    FillPhvWithTestPattern(&phv);

    // For all of the output bytes (grouped by word)
    // check all of the 32 bit phv words (numbered 0->63) and with in them each
    //  byte (which is selected by the output byte within a word)
    for (int ow=0; ow<MauInputXbarWithReg::kOutputBytes; ow+=4) {
      for (int which_phv_word=0;which_phv_word<64;++which_phv_word) {
        for (int ob=0;ob<4;++ob) {
          output_valid = false;
          int which_output_byte = ob+ow;
          input_xbar_util::set_byte_src(0,0,0,which_output_byte, true, which_phv_word );
          mix.CalculateOutputByte(phv, which_output_byte, &output_byte, &output_valid );
          if (ix_print) RMT_UT_LOG_INFO("which_output_byte = %d,output_byte = %02x valid=%d\n",which_output_byte,output_byte,output_valid);
          ASSERT_TRUE( output_byte == (ob+which_phv_word) );
          ASSERT_TRUE( output_valid == true);
        }
      }
    }
  }


  TEST(BFN_TEST_NAME(MauInputXbar),Words8UsingRegisters) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_words_8_using_register()\n");

    ResetModel();
    TestUtil tu(GLOBAL_MODEL.get(),0,0,0);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    Mau::kMauDinPowerMode = false;
    const MauConfig mau_config{ 0 };
    Mau mau(om,1,0,mau_config);
    MauInputXbarWithReg mix(0/*chip*/,1/*pipe*/,0/*mau*/,&mau);
    tu.set_phv_range_all(1,0,true); // Set all PHV words in ingress thread

    Phv phv(om);
    uint8_t output_byte=0;
    bool  output_valid=0;
    FillPhvWithTestPattern(&phv);
    // For all of the output bytes
    // check all of the 8 bit phv words (numbered 64->127)
    for (int ob=0; ob<MauInputXbarWithReg::kOutputBytes; ob+=4) {
      for (int which_phv_word=64;which_phv_word<127;++which_phv_word) {
        int which_output_byte = ob;
        input_xbar_util::set_byte_src(0,1,0,which_output_byte, true, which_phv_word );
        mix.CalculateOutputByte(phv, which_output_byte, &output_byte, &output_valid );
        ASSERT_TRUE( output_byte == which_phv_word );
        ASSERT_TRUE( output_valid == true);
      }
    }
  }

  TEST(BFN_TEST_NAME(MauInputXbar),Words16UsingRegisters) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_words_16()\n");

    ResetModel();
    TestUtil tu(GLOBAL_MODEL.get(),0,0,0);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    Mau::kMauDinPowerMode = false;
    const MauConfig mau_config{ 0 };
    Mau mau(om,3,7,mau_config);
    MauInputXbarWithReg mix(0/*chip*/,3/*pipe*/,7/*mau*/,&mau);
    tu.set_phv_range_all(3,7,true); // Set all PHV words in ingress thread

    Phv phv(om);
    uint8_t output_byte=0;
    bool  output_valid=0;
    FillPhvWithTestPattern(&phv);
    // For all of the output bytes (grouped by 16 bit word)
    // check all of the 16 bit phv words (numbered 128->223) and with in them each
    //  byte (which is selected by the output byte within a word)
    for (int ow=0; ow<MauInputXbarWithReg::kOutputBytes; ow+=2) {
      for (int which_phv_word=128;which_phv_word<223;++which_phv_word) {
        for (int ob=0;ob<2;++ob) {
          int which_output_byte = ob+ow;
          input_xbar_util::set_byte_src(0,3,7,which_output_byte, true, which_phv_word );
          mix.CalculateOutputByte(phv, which_output_byte, &output_byte, &output_valid );
          ASSERT_TRUE( output_byte == (ob+which_phv_word)  &&  output_valid == true);

        }
      }
    }
  }




  TEST(BFN_TEST_NAME(MauInputXbar),Words32UsingRegistersCalculateOutput) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_words_32_using_registers_calculate_output()\n");

    ResetModel();
    TestUtil tu(GLOBAL_MODEL.get(),0,0,0);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    Mau::kMauDinPowerMode = false;
    const MauConfig mau_config{ 0 };
    Mau mau(om,2,4,mau_config);
    MauInputXbarWithReg mix(0/*chip*/,2/*pipe*/,4/*mau*/,&mau);
    tu.set_phv_range_all(2,4,true); // Set all PHV words in ingress thread

    Phv phv(om);
    BitVector< MauInputXbarWithReg::kOutputBits > output;
    BitVector< MauInputXbarWithReg::kOutputBytes > output_valid;
    FillPhvWithTestPattern(&phv);

    // For all of the output bytes (grouped by word)
    // check all of the 32 bit phv words (numbered 0->63) and with in them each
    //  byte (which is selected by the output byte within a word)
    for (int which_phv_word=0;which_phv_word<64;++which_phv_word) {
      for (int ob=0; ob<MauInputXbarWithReg::kOutputBytes; ob++) {
        input_xbar_util::set_byte_src(0,2,4,ob, true, (which_phv_word + (ob%2))%64  );
      }
      mix.CalculateOutput(phv, output, output_valid );

      for (int ob=0; ob<MauInputXbarWithReg::kOutputBytes; ob++) {
        ASSERT_TRUE( output.get_byte(ob) == ((ob%4)+ ((which_phv_word + (ob%2))%64) )
                     &&  output_valid.get_bit(ob) == true);
      }
    }
  }

  TEST(BFN_TEST_NAME(MauInputXbar),TernaryWords32UsingRegisters) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_ternary_words_32_using_registers()\n");

    ResetModel();
    TestUtil tu(GLOBAL_MODEL.get(),0,0,0);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    Mau::kMauDinPowerMode = false;
    const MauConfig mau_config{ 0 };
    Mau mau(om,2,5,mau_config);
    MauInputXbarWithReg mix(0/*chip*/,2/*pipe*/,5/*mau*/,&mau);
    tu.set_phv_range_all(2,5,true); // Set all PHV words in ingress thread

    Phv phv(om);
    BitVector< MauInputXbarWithReg::kOutputBits > output;
    BitVector< MauInputXbarWithReg::kOutputBytes > output_valid;
    FillPhvWithTestPattern(&phv);

    // For all of the output bytes (grouped by word)
    // check all of the 32 bit phv words (numbered 0->63) and with in them each
    //
    for (int which_phv_word=0;which_phv_word<64;++which_phv_word) {
      for (int ob=MauInputXbarWithReg::kTernaryFirstByte; ob<MauInputXbarWithReg::kOutputBytes-3; ob++) {
        if (ix_print) printf("which_phv_word=%d ob=%d\n",which_phv_word,ob);
        input_xbar_util::set_32_bit_word_src(0,2,5,ob,true, which_phv_word );
        mix.CalculateOutput(phv, output, output_valid );

        for (int rsvd0=MauInputXbarWithReg::kTernaryFirstByte; rsvd0<MauInputXbarWithReg::kOutputBytes; rsvd0++) {
          if ((rsvd0 >= ob) && ( rsvd0 < (ob+4))) {
            if (ix_print) printf(" output[%d]=%02x\n",rsvd0,output.get_byte(rsvd0));
            ASSERT_TRUE( (output.get_byte(rsvd0) == (which_phv_word + (rsvd0-ob)))
                         && output_valid.get_bit(rsvd0) == true);
          }
          else {
            ASSERT_TRUE( (output.get_byte(rsvd0) == 0) && output_valid.get_bit(rsvd0) == false);
          }
        }
        // disable that word again
        input_xbar_util::set_32_bit_word_src(0,2,5,ob,false, which_phv_word );
      }
    }

  }

  TEST(BFN_TEST_NAME(MauInputXbar),TernaryWords16UsingRegisters) {
    if (ix_print) RMT_UT_LOG_INFO("test_mau_input_xbar_ternary_words_16_using_registers()\n");

    ResetModel();
    TestUtil tu(GLOBAL_MODEL.get(),0,0,0);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    Mau::kMauDinPowerMode = false;
    const MauConfig mau_config{ 0 };
    Mau mau(om,2,6,mau_config);
    MauInputXbarWithReg mix(0/*chip*/,2/*pipe*/,6/*mau*/,&mau);
    tu.set_phv_range_all(2,6,true); // Set all PHV words in ingress thread

    Phv phv(om);
    BitVector< MauInputXbarWithReg::kOutputBits > output;
    BitVector< MauInputXbarWithReg::kOutputBytes > output_valid;
    FillPhvWithTestPattern(&phv);

    // For all of the output bytes (grouped by word)
    // check all of the 96 16-bit phv words (numbered 128->223) can be
    //  routed to any of the bytes
    for (int which_phv_word=128;which_phv_word<224;++which_phv_word) {
      for (int ob=MauInputXbarWithReg::kTernaryFirstByte; ob<MauInputXbarWithReg::kOutputBytes-1; ob++) {
        if (ix_print) printf("which_phv_word=%d ob=%d\n",which_phv_word,ob);
        input_xbar_util::set_16_bit_word_src(0,2,6,ob,true, which_phv_word );
        mix.CalculateOutput(phv, output, output_valid );

        for (int rsvd0=MauInputXbarWithReg::kTernaryFirstByte; rsvd0<MauInputXbarWithReg::kOutputBytes; rsvd0++) {
          if ((rsvd0 >= ob) && ( rsvd0 < (ob+2))) {
            if (ix_print) printf(" output[%d]=%02x\n",rsvd0,output.get_byte(rsvd0));
            ASSERT_TRUE( (output.get_byte(rsvd0) == (which_phv_word + (rsvd0-ob)))
                         && output_valid.get_bit(rsvd0) == true);
          }
          else {
            ASSERT_TRUE( (output.get_byte(rsvd0) == 0) && output_valid.get_bit(rsvd0) == false);
          }
        }
        // disable that word again
        input_xbar_util::set_16_bit_word_src(0,2,6,ob,false, which_phv_word );
      }
    }

  }

}



