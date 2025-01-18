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
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <string>
#include <algorithm>
#include <mau.h>
#include <mau-stateful-alu.h>
#include <mau-meter-alu.h>
#include <utests/reader_actions.h>
#include <register_includes/register_map.h>
#include "gtest.h"
#include "cmp_helper.h"

using namespace MODEL_CHIP_TEST_NAMESPACE;
using namespace MODEL_CHIP_NAMESPACE;

namespace MODEL_CHIP_TEST_NAMESPACE {

bool operator==(const PhvKey& lhs, const PhvKey& rhs) {
  return std::get<0>(lhs) == std::get<0>(rhs) &&
         std::get<1>(lhs) == std::get<1>(rhs) &&
         std::get<2>(lhs) == std::get<2>(rhs) &&
         std::get<3>(lhs) == std::get<3>(rhs) ;
}

bool operator==(const EopKey& lhs, const EopKey& rhs) {
  return std::get<0>(lhs) == std::get<0>(rhs) &&
         std::get<1>(lhs) == std::get<1>(rhs) ;
}


ReaderActions::ReaderActions() :
    mau_addrmap_map_( new Mau_addrmap_map )
{
}
ReaderActions::~ReaderActions()
{
  delete mau_addrmap_map_;
}

void ReaderActions::clear() {
  pipe_=0;
  stage_=0;
  phvs_.clear();
  eops_.clear();
  teops_.clear();
}


void ReaderActions::set(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,int valid)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set %cphv_%s %de%d %d %x\n",c, s.c_str(), n, e,a0,a1);
  if (valid) {
    phvs_[ PhvKey{c,s,n,e} ]->set( a0, a1 );
  }
  if (debug && !valid) printf("set %cphv_%s %de%d %d %x skipped as invalid\n",c, s.c_str(), n, e,a0,a1);

}
void ReaderActions::phv_alloc(const char c, const std::string s, const int n,  boost::optional<int> oe)
{
  int e = oe ? *oe : 0;
  if (debug) printf("phv_alloc %cphv_%s %de%d\n",c, s.c_str(), n, e);

  phvs_.emplace( PhvKey( c, s, n, e ) , std::unique_ptr<Phv>(tu_->phv_alloc()) );
}
void ReaderActions::set_ingress(const char c, const std::string s, const int n,  boost::optional<int> oe)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set_ingress %cphv_%s %de%d\n",c, s.c_str(), n, e);
  phvs_[ PhvKey{c,s,n,e} ]->set_ingress();
}
void ReaderActions::set_egress(const char c, const std::string s, const int n,  boost::optional<int> oe)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set_egress %cphv_%s %de%d\n",c, s.c_str(), n, e);
  phvs_[ PhvKey{c,s,n,e} ]->set_egress();
}
void ReaderActions::set_ghost(const char c, const std::string s, const int n,  boost::optional<int> oe)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set_ghost %cphv_%s %de%d\n",c, s.c_str(), n, e);
  phvs_[ PhvKey{c,s,n,e} ]->set_ghost();
}
void ReaderActions::set_version(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,bool a1)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set_version %cphv_%s %de%d %d %s\n",c, s.c_str(), n, e,a0,a1?"true":"false");
  phvs_[ PhvKey{c,s,n,e} ]->set_version( a0, a1 );
}
void ReaderActions::set_meter_tick_time(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,uint64_t a2)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set_meter_tick_time %cphv_%s %de%d %d %d %" PRIi64 " \n",c, s.c_str(), n, e,a0,a1,a2);
  phvs_[ PhvKey{c,s,n,e} ]->set_meter_tick_time( a0,a1,a2 );
}
void ReaderActions::set_meter_random_value(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,uint64_t a2)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set_meter_tick_time %cphv_%s %de%d %d %d %" PRIi64 " \n",c, s.c_str(), n, e,a0,a1,a2);
  phvs_[ PhvKey{c,s,n,e} ]->set_meter_random_value( a0,a1,a2 );
}
void ReaderActions::set_relative_time(const char c, const std::string s, const int n,  boost::optional<int> oe , uint64_t a0)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set_relative_time %cphv_%s %de%d %" PRIi64 " \n",c, s.c_str(), n, e,a0);
  phvs_[ PhvKey{c,s,n,e} ]->set_relative_time( a0 );
}
void ReaderActions::set_eopnum(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1, bool a2)
{
  int e = oe ? *oe : 0;
  if (debug) printf("set_eopnum %cphv_%s %de%d 0x%x %d %s\n",c, s.c_str(), n, e,a0,a1,a2?"true":"false");
  phvs_[ PhvKey{c,s,n,e} ]->set_eopnum( Eop::make_eopnum(a0,a1), a2 );
}
void ReaderActions::selector_test(int a0,uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
  if (debug) printf("selector_test %d %x %x %x %x\n",a0,a1,a2,a3,a4);
  MauLogicalRow *logrow = tu_->get_mau()->logical_row_lookup(a0);

  BitVector<MauDefs::kDataBusWidth> data_in(UINT64_C(0));

  //logrow->stats_alu_rd_data(&data_in);
  logrow->get_selector_alu_input_data(&data_in);
  EXPECT_EQ( a1, data_in.get_word(0,  32));
  EXPECT_EQ( a2, data_in.get_word(32, 32));
  EXPECT_EQ( a3, data_in.get_word(64, 32));
  EXPECT_EQ( a4, data_in.get_word(96, 32));
}

void ReaderActions::stateful_test_jbay(std::vector<uint32_t> phv_d, std::vector<uint32_t> data_in,
                                       boost::fusion::vector4<uint32_t, int64_t ,std::vector<bool>,std::vector<bool>> params,
                                       boost::optional<uint64_t> random_number,
                                       std::vector<uint32_t> data_out, std::vector<uint32_t> action_out) {
  using boost::fusion::at_c;
  stateful_test_internal(phv_d, data_in, at_c<0>(params) /*addr*/, at_c<1>(params) /*present_time*/,
                         at_c<2>(params) /*match_bus*/, at_c<3>(params) /*learn_or_match_bus*/,
                         random_number,
                         data_out, action_out, true);
}

void ReaderActions::stateful_test_internal(std::vector<uint32_t> phv_d, std::vector<uint32_t> data_in, uint32_t addr, uint64_t present_time,
                                           std::vector<bool> match_bus, std::vector<bool> learn_or_match_bus,
                                           boost::optional<uint64_t> random_number,
                                           std::vector<uint32_t> data_out, std::vector<uint32_t> action_out, bool check_action)
{
  BitVector<MauDefs::kStatefulMeterAluDataBits> phv_data_word{};
  phv_data_word.set32(0,phv_d[0]);
  phv_data_word.set32(1,phv_d[1]);
  if (MauDefs::kStatefulMeterAluDataBits==128) {
    phv_data_word.set32(2,phv_d[2]);
    phv_data_word.set32(3,phv_d[3]);
  }
  BitVector<MauDefs::kDataBusWidth> data_in_bv;
  BitVector<MauDefs::kDataBusWidth> data_out_bv;
  BitVector<MauDefs::kActionOutputBusWidth> action_out_bv;

  data_in_bv.set32(0,data_in[0]);
  data_in_bv.set32(1,data_in[1]);
  data_in_bv.set32(2,data_in[2]);
  data_in_bv.set32(3,data_in[3]);

  std::array<bool, 4> match_bus_arr;
  std::copy_n(match_bus.begin(), 4, match_bus_arr.begin());
  std::array<bool, 4> learn_or_match_bus_arr;
  std::copy_n(learn_or_match_bus.begin(), 4, learn_or_match_bus_arr.begin());

  int which_alu=0;
  int logrow = MauMeterAlu::get_meter_alu_logrow_index(which_alu);
  MauStatefulAlu* salu = tu_->get_mau()->logical_row_lookup(logrow)->mau_meter_alu()->get_salu();
  assert( salu );
  salu->reset_resources();
  if ( random_number ) {
    salu->set_random_number_value( *random_number );
  }
  salu->set_check_total_correct(false);
  salu->calculate_output(addr,phv_data_word,&data_in_bv,&data_out_bv,&action_out_bv,present_time,true,
                         match_bus_arr,learn_or_match_bus_arr);
  EXPECT_PRED_FORMAT2(CmpHelperIntHex ,data_out[0],data_out_bv.get_word( 0,32));
  EXPECT_PRED_FORMAT2(CmpHelperIntHex ,data_out[1],data_out_bv.get_word(32,32));
  EXPECT_PRED_FORMAT2(CmpHelperIntHex ,data_out[2],data_out_bv.get_word(64,32));
  EXPECT_PRED_FORMAT2(CmpHelperIntHex ,data_out[3],data_out_bv.get_word(96,32));

  if (check_action) {
    EXPECT_PRED_FORMAT2(CmpHelperIntHex ,action_out[0],action_out_bv.get_word( 0,32));
    if (MauDefs::kActionOutputBusWidth == 128) {
      EXPECT_PRED_FORMAT2(CmpHelperIntHex ,action_out[1],action_out_bv.get_word(32,32));
      EXPECT_PRED_FORMAT2(CmpHelperIntHex ,action_out[2],action_out_bv.get_word(64,32));
      EXPECT_PRED_FORMAT2(CmpHelperIntHex ,action_out[3],action_out_bv.get_word(96,32));
    }
  }

}

void ReaderActions::stateful_test(uint64_t phv_d, std::vector<uint32_t> data_in, uint32_t addr, bool forwarding, std::vector<uint32_t> data_out, boost::optional<int> action_out)
{
  bool action_valid = action_out ? true:false;
  uint32_t action = action_valid ? *action_out : 0;

  uint32_t phv_d_lo = phv_d & 0xFFFFFFFF;
  uint32_t phv_d_hi = (phv_d>>32) & 0xFFFFFFFF;

  std::vector<uint32_t> phv_d_v { phv_d_lo, phv_d_hi, 0, 0 };
  std::vector<uint32_t> action_out_v { action, 0, 0, 0 };

  // The old tofino logs that are read by this don't have the present_time in, they have a forwarding flag, so fake it here.
  // salu will only detect forwarding if present_time is last_time+1
  present_time_ += forwarding ? 1 : 2;

  stateful_test_internal(phv_d_v, data_in, addr, present_time_,
                         { false, false, false, false }, { false, false, false, false },
                         boost::optional<uint64_t>( boost::none ),
                         data_out, action_out_v, action_valid);
}

void ReaderActions::set_pred(int i_st, int e_st, int g_st, uint16_t gex, uint8_t lbr) {
  if (debug) printf("set_pred %d %d %d 0x%x 0x%x\n",i_st,e_st,g_st,gex,lbr);
  tu_->get_mau()->set_pred(i_st, e_st, g_st, gex, lbr);
}
void ReaderActions::set_mpr(int i_st, int e_st, int g_st, uint16_t gex, uint8_t lbr) {
  if (debug) printf("set_mpr %d %d %d 0x%x 0x%x\n",i_st,e_st,g_st,gex,lbr);
  tu_->get_mau()->set_mpr(i_st, e_st, g_st, gex, lbr);
}

void ReaderActions::set_ingress_snapshot_triggered(bool tf) {
  if (debug) printf("set_ingress_snapshot_triggered %s\n",tf?"true":"false");
  tu_->get_mau()->mau_io_input()->set_ingress_snapshot_triggered(tf);
}
void ReaderActions::set_egress_snapshot_triggered(bool tf) {
  if (debug) printf("set_egress_snapshot_triggered %s\n",tf?"true":"false");
  tu_->get_mau()->mau_io_input()->set_egress_snapshot_triggered(tf);
}

// pointers to start tables here because I can't work out how to get them to lazily evaluate in the grammer
void ReaderActions::process_match2(int a0, int a1, int* ingress_start_table,int* egress_start_table,int* ghost_start_table)
{
  if (debug) printf("process_match2 %de%d i_start=%d e_start=%d g_start=%d\n",
         a0, a1,
           *ingress_start_table,*egress_start_table,*ghost_start_table);

  /*Phv *iphv_out_got =*/ (void) tu_->get_mau()->process_match2(phvs_[ PhvKey{'i',"in",a0,a1} ].get(), phvs_[ PhvKey{'o',"in",a0,a1} ].get(),
                                                                 ingress_start_table,egress_start_table,ghost_start_table);

  Phv *cmp_phv = phvs_[ PhvKey{'i',"in",a0,a1} ].get();
  assert( cmp_phv ); // if this is not true the phv to compare to probably was not defined in the test file

}
void ReaderActions::process_match(int a0, int a1, int* ingress_start_table,int* egress_start_table)
{
  if (debug) printf("process_match %de%d i_start=%d e_start=%d\n",
         a0, a1,
           *ingress_start_table,*egress_start_table);

  /*Phv *iphv_out_got =*/ (void) tu_->get_mau()->process_match(phvs_[ PhvKey{'i',"in",a0,a1} ].get(), phvs_[ PhvKey{'o',"in",a0,a1} ].get(),
                                                               ingress_start_table,egress_start_table);

  Phv *cmp_phv = phvs_[ PhvKey{'i',"in",a0,a1} ].get();
  assert( cmp_phv ); // if this is not true the phv to compare to probably was not defined in the test file

}
void ReaderActions::process_action(int a0, int a1 )
{
  if (debug) printf("process_action %de%d\n",a0,a1);
  Phv *ophv_out_got = tu_->get_mau()->process_action(phvs_[ PhvKey{'i',"in",a0,a1} ].get(), phvs_[ PhvKey{'o',"in",a0,a1} ].get());
  Phv *cmp_phv = phvs_[ PhvKey{'o',"out",a0,a1} ].get();
  //assert( cmp_phv ); // if this is not true the phv to compare to probably was not defined in the test file
  EXPECT_TRUE(cmp_phv && "If this fails ophv_out_ is probably not defined in the test file");
  if (cmp_phv) {
    EXPECT_TRUE( TestUtil::compare_phvs(ophv_out_got, cmp_phv ,false) );
  }
}
void ReaderActions::new_eop(int a0, int a1 )
{
  if (debug) printf("new_eop %d %d\n",a0,a1);
  eops_.emplace( EopKey(a0, a1) , std::unique_ptr<Eop>(new Eop{}) );
}
void ReaderActions::handle_eop(int a0, int a1 )
{
  if (debug) printf("handle_eop %d %d\n",a0,a1);
  tu_->get_mau()->handle_eop( *eops_[ EopKey{a0,a1} ].get() );
}
void ReaderActions::set_ingress_eopinfo(int a0, int a1, int a2, int a3, int a4 )
{
  if (debug) printf("set_ingress_eopinfo %d %d = 0x%x 0x%x err=%d\n",a0,a1,a2,a3,a4);
  eops_[ EopKey{a0,a1} ]->set_ingress_eopinfo( a2, a3, a4 );
}
void ReaderActions::set_egress_eopinfo(int a0, int a1, int a2, int a3, int a4 )
{
  if (debug) printf("set_egress_eopinfo %d %d = 0x%x 0x%x err=%d\n",a0,a1,a2,a3,a4);
  eops_[ EopKey{a0,a1} ]->set_egress_eopinfo( a2, a3, a4 );
}
void ReaderActions::eop_set_meter_tick_time(int a0,int a1, int a2, int a3, uint64_t a4)
{
  if (debug) printf("eop_set_meter_tick_time %d %d %d %d %" PRIx64 "\n",a0,a1,a2,a3,a4);
  eops_[ EopKey{a0,a1} ]->set_meter_tick_time( a2, a3, a4 );
}
void ReaderActions::eop_set_meter_random_value(int a0,int a1, int a2, int a3, uint64_t a4)
{
  if (debug) printf("eop_set_meter_random_value %d %d %d %d %" PRIx64 "\n",a0,a1,a2,a3,a4);
  eops_[ EopKey{a0,a1} ]->set_meter_random_value( a2, a3, a4 );
}
void ReaderActions::eop_set_relative_time(int a0,int a1, uint64_t a2)
{
  if (debug) printf("eop_set_relative_time %d %d %" PRIi64 "\n",a0,a1,a2);
  eops_[ EopKey{a0,a1} ]->set_relative_time( a2);
}

void ReaderActions::new_teop(int a0, int a1 )
{
  if (debug) printf("new_teop %d %d\n",a0,a1);
  teops_.emplace( EopKey(a0, a1) , std::unique_ptr<Teop>(new Teop{}) );
}
void ReaderActions::handle_dp_teop(int a0, int a1 )
{
  if (debug) printf("handle_dp_teop %d %d\n",a0,a1);
  tu_->get_mau()->handle_dp_teop( *teops_[ EopKey{a0,a1} ].get() );
}
void ReaderActions::teop_set_raw_addr(int a0, int a1, int a2, int a3 )
{
  if (debug) printf("teop_set_raw_addr %d %d = 0x%x 0x%x\n",a0,a1,a2,a3);
  teops_[ EopKey{a0,a1} ]->set_raw_addr( a2, a3 );
}
void ReaderActions::teop_set_byte_len(int a0, int a1, int a2 )
{
  if (debug) printf("teop_set_byte_len %d %d = %d\n",a0,a1,a2);
  teops_[ EopKey{a0,a1} ]->set_byte_len( a2 );
}
void ReaderActions::teop_set_error(int a0, int a1, int a2 )
{
  if (debug) printf("teop_set_error %d %d = 0x%x\n",a0,a1,a2);
  teops_[ EopKey{a0,a1} ]->set_error( a2 );
}
void ReaderActions::teop_set_meter_tick_time(int a0,int a1, int a2, int a3, uint64_t a4)
{
  if (debug) printf("teop_set_meter_tick_time %d %d %d %d %" PRIx64 "\n",a0,a1,a2,a3,a4);
  teops_[ EopKey{a0,a1} ]->set_meter_tick_time( a2, a3, a4 );
}
void ReaderActions::teop_set_meter_random_value(int a0,int a1, int a2, int a3, uint64_t a4)
{
  if (debug) printf("teop_set_meter_random_value %d %d %d %d %" PRIx64 "\n",a0,a1,a2,a3,a4);
  teops_[ EopKey{a0,a1} ]->set_meter_random_value( a2, a3, a4 );
}
void ReaderActions::teop_set_relative_time(int a0,int a1, uint64_t a2)
{
  if (debug) printf("teop_set_relative_time %d %d %" PRIi64 "\n",a0,a1,a2);
  teops_[ EopKey{a0,a1} ]->set_relative_time( a2);
}

void ReaderActions::reset()
{
  if (debug) printf("reset\n");
  tu_->Reset();
  //tu_->update_log_flags(ALL, ALL, ALL, ALL, ALL, ALL, ALL);
}
void ReaderActions::indirect_write(uint64_t a0,uint64_t a1, uint64_t a2,boost::optional<uint64_t> T)
{
  if (T) {
    if (debug) printf("indirect_write  %" PRIx64 " %" PRIx64 " %" PRIx64 " %" PRIx64 "\n",a0,a1,a2,*T);
    tu_->IndirectWrite(a0,a1,a2,*T); // version that takes a time.
  }
  else {
    if (debug) printf("indirect_write  %" PRIx64 " %" PRIx64 " %" PRIx64 "\n",a0,a1,a2);
    tu_->IndirectWrite(a0,a1,a2);
  }
}
void ReaderActions::remap_indirect_write(int a0,int a1,int a2, int a3,uint64_t a4,uint64_t a5, uint64_t a6)
{
  if (debug) printf("remap_indirect_write %d %d %d %d %" PRIx64 " %" PRIx64 " %" PRIx64 "\n",a0,a1,a2,a3,a4,a5,a6);
  tu_->RemapIndirectWrite(a0,a1,a2,a3,a4,a5,a6);
}

void ReaderActions::out_word(std::vector<PathElement> a0,uint32_t a1)
{

  PathElement first=a0[0];
  if ( boost::fusion::at<boost::mpl::int_<0>>( first ) == "mau_reg_map" ) {
    // start at index 1 as index 0 just tells us it's mau_addr_map
    auto addr = mau_addrmap_map_->map(a0, 1);

    uint64_t addr64 = reinterpret_cast<uint64_t>(addr);
    // print out which word if something went wrong in the remapping
    if (debug || (addr64 > UINT64_C(0xFFFFFFFF))) {
      printf("out_word ");
      for (PathElement el : a0) {
        printf("%s",boost::fusion::at<boost::mpl::int_<0>>( el ).c_str() );
        auto el_array = boost::fusion::at<boost::mpl::int_<1>>( el );
        if (el_array) {
          for (int dim : *el_array) {
            printf("[%d]",dim);
          }
        }
        printf(".");
      }
      printf(" (0x%" PRIx64 ") 0x%x\n",addr64,a1);
    }


    tu_->OutWord( reinterpret_cast<void*>(
        reinterpret_cast<intptr_t>(RegisterUtils::addr_mau(pipe_,stage_)) +
        reinterpret_cast<intptr_t>(addr)),
                  a1 );
  }
  else {
    // could add more capabilities here, eg could address whole of chip
    assert(0);
  }
}

void ReaderActions::out_word_addr(uint32_t a0,uint32_t a1)
{
  tu_->OutWord( a0, a1 );
}

void ReaderActions::out_word_pit(int a0, int a1, std::vector<PathElement> a2,uint32_t a3)
{

  if (debug) {
    printf("out_word_pit %d,%d ",a0,a1);
    for (PathElement el : a2) {
      printf("%s",boost::fusion::at<boost::mpl::int_<0>>( el ).c_str() );
      auto el_array = boost::fusion::at<boost::mpl::int_<1>>( el );
      if (el_array) {
        for (int dim : *el_array) {
          printf("[%d]",dim);
        }
      }
      printf(".");
    }
    printf(" 0x%x\n",a3);
  }

  PathElement first=a2[0];
  if ( boost::fusion::at<boost::mpl::int_<0>>( first ) == "mau_reg_map" ) {
    // start at index 1 as index 0 just tells us it's mau_addr_map
    auto addr = mau_addrmap_map_->map(a2, 1);
    tu_->OutWordPiT(a0,a1, reinterpret_cast<void*>(
        reinterpret_cast<intptr_t>(RegisterUtils::addr_mau(pipe_,stage_)) +
        reinterpret_cast<intptr_t>(addr)),
                  a3 );
  }
  else {
    // could add more capabilities here, eg could address whole of chip
    assert(0);
  }
}
void ReaderActions::out_word_pet(int a0, int a1, std::vector<PathElement> a2,uint32_t a3)
{

  if (debug) {
    printf("out_word_pet %d,%d ",a0,a1);
    for (PathElement el : a2) {
      printf("%s",boost::fusion::at<boost::mpl::int_<0>>( el ).c_str() );
      auto el_array = boost::fusion::at<boost::mpl::int_<1>>( el );
      if (el_array) {
        for (int dim : *el_array) {
          printf("[%d]",dim);
        }
      }
      printf(".");
    }
    printf(" 0x%x\n",a3);
  }

  PathElement first=a2[0];
  if ( boost::fusion::at<boost::mpl::int_<0>>( first ) == "mau_reg_map" ) {
    // start at index 1 as index 0 just tells us it's mau_addr_map
    auto addr = mau_addrmap_map_->map(a2, 1);
    tu_->OutWordPeT(a0,a1, reinterpret_cast<void*>(
        reinterpret_cast<intptr_t>(RegisterUtils::addr_mau(pipe_,stage_)) +
        reinterpret_cast<intptr_t>(addr)),
                  a3 );
  }
  else {
    // could add more capabilities here, eg could address whole of chip
    assert(0);
  }
}

void ReaderActions::aht_new()
{
  if (debug) printf("aht_new\n");
  action_hv_translator_ = new ActionHvTranslator;
}
void ReaderActions::aht_ctl_word(int a0, int a1, int a2, int a3 )
{
  if (debug) printf("aht_ctl_word %d %d %d %d\n",a0,a1,a2,a3);
  assert(action_hv_translator_);
  action_hv_translator_->ctl_word(a0,a1,a2,a3);
}
void ReaderActions::aht_do_writes()
{
  if (debug) printf("aht_do_writes\n");
  assert(action_hv_translator_);
  action_hv_translator_->do_writes(tu_);
}
uint32_t ReaderActions::rm_b4_x( boost::optional<int> a0, uint32_t a1 ) {
  // this is mapped by a define in test util in the real actions
  int v= a0 ? *a0 : 0;
  if (debug) printf("rm_b4_x (%d) %x\n",v,a1);
  switch (v) {
    case 0:  return RM_B4(a1);
    case 8:  return RM_B4_8(a1);
    case 16: return RM_B4_16(a1);
    case 32: return RM_B4_32(a1);
  }
  return 0;
}

}
