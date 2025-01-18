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

#include <string>
#include <rmt-log.h>
#include <rmt-types.h>
#include <rmt-object-manager.h>
#include <mau-gateway-table.h>
#include <mau-sram-row.h>
#include <mau-result-bus.h>

namespace MODEL_CHIP_NAMESPACE {

MauGatewayTable::MauGatewayTable(RmtObjectManager *om, int pipeIndex, int mauIndex,
                                 int rowIndex, int tableIndex, Mau *mau, MauSramRow *row)
    : MauObject(om, pipeIndex, mauIndex, RmtTypes::kRmtTypeMauGatewayTable, rowIndex, tableIndex, mau),
    mau_gateway_table_reg_(om,pipeIndex,mauIndex,rowIndex,tableIndex,this),
    row_(row)
{
  RMT_LOG_VERBOSE("MAU_GATEWAY_TABLE::create\n");
  tcam_.reset();
  // set the start to the highest entry (otherwise 0 is highest pri entry!)
  tcam_.set_tcam_start(kEntries-1);
}

void MauGatewayTable::VersionValidUpdate(int index) {
  RMT_ASSERT(index<kEntries);

  BitVector<kTcamWidth> word1;
  BitVector<kTcamWidth> word0;

  // TODO: should lock between the get and the set
  tcam_.get(index,&word0,&word1);

  word0.set_word( mau_gateway_table_reg_.GetVersionValid0(index), kTcamVersionPos, kVersionWidth );
  word1.set_word( mau_gateway_table_reg_.GetVersionValid1(index), kTcamVersionPos, kVersionWidth );

  tcam_.set_word0_word1_nolock( index, word0, word1 );

}

void MauGatewayTable::DataUpdate(int index,int word) {
  RMT_ASSERT(index<kEntries);

  BitVector<kTcamWidth> word1;
  BitVector<kTcamWidth> word0;

  // TODO: should lock between the get and the set
  tcam_.get(index,&word0,&word1);

  if (word==0) {
    word0.set_word( mau_gateway_table_reg_.GetDataEntryWord0(index), kTcamExpressionAPos, kExpressionAWidth );
  }
  else {
    word1.set_word( mau_gateway_table_reg_.GetDataEntryWord1(index), kTcamExpressionAPos, kExpressionAWidth );
  }

  tcam_.set_word0_word1_nolock( index, word0, word1 );
}


void MauGatewayTable::MatchDataUpdate(int index,int word) {
  RMT_ASSERT(index<kEntries);

  BitVector<kTcamWidth> word1;
  BitVector<kTcamWidth> word0;

  // TODO: should lock between the get and the set
  tcam_.get(index,&word0,&word1);

  if (word == 0) {
    word0.set_word( mau_gateway_table_reg_.GetMatchData(index,word), kTcamExpressionBPos, kExpressionBWidth );
  }
  else {
    word1.set_word( mau_gateway_table_reg_.GetMatchData(index,word), kTcamExpressionBPos, kExpressionBWidth );
  }

  tcam_.set_word0_word1_nolock( index, word0, word1 );
}

void MauGatewayTable::lookup(Phv *phv, bool *hit, int *hit_index) {
  *hit_index = -1;

  // if nothing is enabled the gateway table always misses
  if ( ! ( mau_gateway_table_reg_.GetHash0Select() ||
           mau_gateway_table_reg_.GetHash1Select() ||
           mau_gateway_table_reg_.GetData0Select() ||
           mau_gateway_table_reg_.GetData1Select() ) ) {
    *hit = false;
    return;
  }

  BitVector<kTcamWidth> tcam_s0;
  BitVector<kTcamWidth> tcam_s1;

  // Start with all zeros and only fill in ExpressionA or ExpressionB parts if
  //  they are active. This works because s0=0,s1=0 always matches.
  tcam_s1.fill_all_zeros();
  tcam_s0.fill_all_zeros();

  // Work out the version/valid and put them into the tcam s1,s0 words
  static_assert( kVersionWidth<=8, "Can only handle 8 bits of version/valid");
  uint8_t version_bits;
  if ( mau_gateway_table_reg_.GetEgressThread() ) {
    version_bits = phv->egress_version();
  }
  else {
    version_bits = phv->ingress_version();
  }
  tcam_s1.set_word(  version_bits, kTcamVersionPos, kVersionWidth );
  tcam_s0.set_word( ~version_bits, kTcamVersionPos, kVersionWidth );

  // If either of the hash selects is enabled, expression A will be active
  if ( mau_gateway_table_reg_.GetHash0Select()  ||
       mau_gateway_table_reg_.GetHash1Select() ) {
    uint64_t expression_A_data = CalculateExpressionAData(phv);
    // Now work out the tcam's s0 and s1 depending on the mode
    uint64_t expression_A_data_s0,expression_A_data_s1;
    switch ( mau_gateway_table_reg_.GetMode() ) {
      case MauGatewayTableReg::kNormal:
        expression_A_data_s1 =  expression_A_data;
        expression_A_data_s0 = ~expression_A_data;
        break;
      case MauGatewayTableReg::k2bDirtCAM:
        Calculate2bDirtCAM(expression_A_data,&expression_A_data_s0,&expression_A_data_s1);
        break;
      case MauGatewayTableReg::k4bDirtCAM:
        Calculate4bDirtCAM(expression_A_data,&expression_A_data_s0,&expression_A_data_s1);
        break;
      default:
        RMT_ASSERT(0);
        break;
    }
    RMT_LOG(RmtDebug::verbose(),
            "MauGatewayTable::lookup expA_data=0x%016" PRIx64 " expA_s0=0x%016" PRIx64
            " expA_s1=0x%016" PRIx64 " \n",
            expression_A_data,expression_A_data_s0,expression_A_data_s1);
    tcam_s1.set_word( expression_A_data_s1, kTcamExpressionAPos, kExpressionAWidth );
    tcam_s0.set_word( expression_A_data_s0, kTcamExpressionAPos, kExpressionAWidth );
  }

  // If either of the data selects is enabled, expression B will be active
  if ( mau_gateway_table_reg_.GetData0Select() ||
       mau_gateway_table_reg_.GetData1Select() ) {
    uint32_t expression_B_data = CalculateExpressionBData(phv);
    tcam_s1.set_word(  expression_B_data, kTcamExpressionBPos, kExpressionBWidth );
    tcam_s0.set_word( ~expression_B_data, kTcamExpressionBPos, kExpressionBWidth );
  }

  *hit_index = tcam_.tcam_lookup( tcam_s0, tcam_s1 );
  *hit = ( *hit_index != -1 );

  RMT_LOG(RmtDebug::verbose(),
          "MauGatewayTable::lookup hit=%d index=%d (s0=%s s1=%s)\n",*hit?1:0,*hit_index,
          tcam_s0.to_string().c_str(),tcam_s1.to_string().c_str());
  return;
}

uint64_t MauGatewayTable::CalculateExpressionAData(Phv *phv) {
  MauSramRow *rowobj = row();
  RMT_ASSERT (rowobj != NULL);

  BitVector<kWholeHashWidth> all_hash;
  if ( mau_gateway_table_reg_.GetHash0Select() ) {
    // only one hash should be selected
    RMT_ASSERT( 0 == mau_gateway_table_reg_.GetHash1Select() );
    rowobj->get_hash(phv,0,&all_hash);
    return all_hash.get_word(kHashExtractStart,kHashExtractWidth);
  }
  else if ( mau_gateway_table_reg_.GetHash1Select() ) {
    rowobj->get_hash(phv,1,&all_hash);
    return all_hash.get_word(kHashExtractStart,kHashExtractWidth);
  }
  else {
    return 0;
  }
}

uint32_t MauGatewayTable::CalculateExpressionBData(Phv *phv) {
  MauSramRow *rowobj = row();
  RMT_ASSERT (rowobj != NULL);
  static_assert( kExpressionBWidth == 32, "Can anly handle 32 bit ExpressionB");
  BitVector<kWholeMatchDataWidth> all_match_data;
  uint32_t match_data31_0;
  uint32_t match_data63_32;
  if ( mau_gateway_table_reg_.GetData0Select() ) {
    // only one match data bus should be selected
    RMT_ASSERT( 0 == mau_gateway_table_reg_.GetData1Select() );
    rowobj->get_match_data(phv,0,&all_match_data);
    match_data31_0  = all_match_data.get_word(  0, 32 );
    match_data63_32 = all_match_data.get_word( 32, 32 );
  }
  else if ( mau_gateway_table_reg_.GetData1Select() ) {
    rowobj->get_match_data(phv,1,&all_match_data);
    match_data31_0  = all_match_data.get_word(  0, 32 );
    match_data63_32 = all_match_data.get_word( 32, 32 );
  }
  else {
    match_data63_32 = match_data31_0 = 0;
  }
  uint32_t xor_enable = mau_gateway_table_reg_.GetXorEnable();
  uint32_t expression_B_data = (   xor_enable  & ( match_data63_32 ^ match_data31_0 ) ) |
                                ((~xor_enable) &   match_data31_0);
  return expression_B_data;

  }

void MauGatewayTable::Calculate2bDirtCAM(uint64_t in,uint64_t *s0,uint64_t *s1) {
  constexpr int width=kHashExtractWidth;
  static_assert( (width%2)==0 , "Calculate2bDirtCAM width must be even");
  static_assert( width<=64 , "Calculate2bDirtCAM max width is 64");
  *s0=*s1=0;
  for (int i=0;i<(width/2);++i) {
    uint64_t bit_pair = (in >> (i*2)) & 0x3;
    uint64_t one_hot = 1 << bit_pair;
    uint64_t one_hot_high = (one_hot >> 2) & 0x3;
    uint64_t one_hot_low  = (one_hot     ) & 0x3;
    *s0 |= (one_hot_low  << (i*2));
    *s1 |= (one_hot_high << (i*2));
  }

}
void MauGatewayTable::Calculate4bDirtCAM(uint64_t in,uint64_t *s0,uint64_t *s1) {
  constexpr int width=kHashExtractWidth;
  static_assert( (width%2)==0 , "Calculate4bDirtCAM width must be whole number of nibbles");
  static_assert( width<=32 , "Calculate4bDirtCAM max width is 32");
  *s0=*s1=0;
  for (int i=0;i<(width/4);++i) {
    uint64_t nibble = (in >> (i*4)) & 0xf;
    uint64_t one_hot = 1 << nibble;
    uint64_t one_hot_high_byte = (one_hot >> 8) & 0xff;
    uint64_t one_hot_low_byte  = (one_hot     ) & 0xff;
    *s0 |= (one_hot_low_byte  << (i*8));
    *s1 |= (one_hot_high_byte << (i*8));
  }
}

MauGatewayTable::~MauGatewayTable()
{
  RMT_LOG_VERBOSE("MAU_GATEWAY_TABLE::delete\n");
}

}
