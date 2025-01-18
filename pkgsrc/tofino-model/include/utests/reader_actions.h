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

#ifndef _UTESTS_READER_ACTIONS_
#define _UTESTS_READER_ACTIONS_

#include <string>
#include <unordered_map>
#include <tuple>
#include <memory>
#include <utests/test_namespace.h>
#include <utests/register_mapper.h>
#include <phv.h>
#include <eop.h>
#include <teop.h>

namespace MODEL_CHIP_TEST_NAMESPACE {
class TestUtil;
class ActionHvTranslator;
}

namespace MODEL_CHIP_TEST_NAMESPACE {

struct Mau_addrmap_map;


typedef std::tuple<char, std::string, int, int> PhvKey;
typedef std::unordered_map< PhvKey, std::unique_ptr<MODEL_CHIP_NAMESPACE::Phv> > PhvMap;
typedef std::tuple<int, int> EopKey;
typedef std::unordered_map< EopKey, std::unique_ptr<MODEL_CHIP_NAMESPACE::Eop> > EopMap;
typedef std::unordered_map< EopKey, std::unique_ptr<MODEL_CHIP_NAMESPACE::Teop> > TeopMap;

bool operator==(const PhvKey& lhs, const PhvKey& rhs);

}

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<> struct hash<MODEL_CHIP_TEST_NAMESPACE::PhvKey>
    {
        std::size_t operator()(MODEL_CHIP_TEST_NAMESPACE::PhvKey const& k) const
        {
          int index=0;
          index += std::get<0>(k) == 'i'  ? 1 : 0; // either i or o
          index += std::get<1>(k) == "in" ? 2 : 0; // in or out
          index += std::get<3>(k) ==  1   ? 4 : 0; // 1 or 0
          index += std::get<2>(k) << 3;
          return std::hash<int>()( index );
        }
    };

    template<> struct hash<MODEL_CHIP_TEST_NAMESPACE::EopKey>
    {
        std::size_t operator()(MODEL_CHIP_TEST_NAMESPACE::EopKey const& k) const
        {
          int index=0;
          index += std::get<1>(k) == 'i'  ? 1 : 0; // either 1 or 0
          index += std::get<0>(k) << 1;
          return std::hash<int>()( index );
        }
    };

}

namespace MODEL_CHIP_TEST_NAMESPACE {

struct ReaderActions {
  static constexpr int debug = 0;
  ReaderActions();
  ~ReaderActions();

  // clear internal structures ready to parse another file
  void clear();

  void set_test_util(MODEL_CHIP_TEST_NAMESPACE::TestUtil* const tu) { tu_ = tu; }

  void set_pipe(int pipe) { pipe_=pipe; }
  void set_stage(int stage) { stage_=stage; }

  MODEL_CHIP_NAMESPACE::Phv* get_phv( const char c, const std::string s, const int n, int e ) {
    auto k = PhvKey{c,s,n,e};
    return phvs_[ k ].get();
  }
  MODEL_CHIP_NAMESPACE::Eop* get_eop( const int n, int e ) {
    auto k = EopKey{n,e};
    return eops_[ k ].get();
  }
  MODEL_CHIP_NAMESPACE::Teop* get_teop( const int n, int e ) {
    auto k = EopKey{n,e};
    return teops_[ k ].get();
  }

  //////////////////////////////////////////////////////////////////////////
  // called by the parser
  void set(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,int valid);
  void phv_alloc(const char c, const std::string s, const int n,  boost::optional<int> oe);
  void set_ingress(const char c, const std::string s, const int n,  boost::optional<int> oe);
  void set_egress(const char c, const std::string s, const int n,  boost::optional<int> oe);
  void set_ghost(const char c, const std::string s, const int n,  boost::optional<int> oe);
  void set_version(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,bool a1);
  void set_meter_tick_time(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,uint64_t a2);
  void set_meter_random_value(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,uint64_t a2);
  void set_relative_time(const char c, const std::string s, const int n,  boost::optional<int> oe , uint64_t a0);
  void set_eopnum(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1, bool a2);
  void selector_test(int a0,uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
  void stateful_test(uint64_t phv_d, std::vector<uint32_t> data_in, uint32_t addr, bool forwarding, std::vector<uint32_t> data_out, boost::optional<int> action_out);
  void stateful_test_jbay(std::vector<uint32_t> phv_d, std::vector<uint32_t> data_in,
                          boost::fusion::vector4<uint32_t, int64_t ,std::vector<bool>,std::vector<bool>> params,
                          boost::optional<uint64_t> random_nunmber,
                          std::vector<uint32_t> data_out, std::vector<uint32_t> action_out);

  void set_pred(int i_st, int e_st, int e_gt, uint16_t gex, uint8_t lbr);
  void set_mpr(int i_st, int e_st, int e_gt, uint16_t gex, uint8_t lbr);
  void set_ingress_snapshot_triggered(bool tf);
  void set_egress_snapshot_triggered(bool tf);
  // pointers to start tables here because I can't work out how to get them to lazily evaluate in the grammer
  void process_match2(int a0, int a1, int* ingress_start_table,int* egress_start_table,int* ghost_start_table);
  void process_match(int a0, int a1, int* ingress_start_table,int* egress_start_table);
  void process_action(int a0, int a1 );
  void new_eop(int a0, int a1 );
  void handle_eop(int a0, int a1 );
  void set_ingress_eopinfo(int a0, int a1, int a2, int a3 , int a4 );
  void set_egress_eopinfo(int a0, int a1, int a2, int a3, int a4 );
  void eop_set_meter_tick_time(int a0,int a1, int a2, int a3, uint64_t a4);
  void eop_set_meter_random_value(int a0,int a1, int a2, int a3, uint64_t a4);
  void eop_set_relative_time(int a0,int a1, uint64_t a2);
  void new_teop(int a0, int a1 );
  void handle_dp_teop(int a0, int a1 );
  void teop_set_raw_addr(int a0,int a1, int a2, int a3);
  void teop_set_byte_len(int a0,int a1, int a2);
  void teop_set_error(int a0,int a1, int a2);
  void teop_set_meter_tick_time(int a0,int a1, int a2, int a3, uint64_t a4);
  void teop_set_meter_random_value(int a0,int a1, int a2, int a3, uint64_t a4);
  void teop_set_relative_time(int a0,int a1, uint64_t a2);
  void reset();
  void indirect_write(uint64_t a0,uint64_t a1, uint64_t a2, boost::optional<uint64_t> T);
  void remap_indirect_write(int a0,int a1,int a2, int a3,uint64_t a4,uint64_t a5, uint64_t a6);
  void out_word(std::vector<PathElement> a0,uint32_t a1);
  void out_word_addr(uint32_t a0,uint32_t a1);
  void out_word_pit(int a0, int a1, std::vector<PathElement> a2,uint32_t a3);
  void out_word_pet(int a0, int a1, std::vector<PathElement> a2,uint32_t a3);
  void aht_new();
  void aht_ctl_word(int a0, int a1, int a2, int a3 );
  void aht_do_writes();
  uint32_t rm_b4_x( boost::optional<int> a0, uint32_t a1 );

  int pipe_=0;
  int stage_=0;
  MODEL_CHIP_TEST_NAMESPACE::TestUtil* tu_=0;
  MODEL_CHIP_TEST_NAMESPACE::ActionHvTranslator* action_hv_translator_=0;
  MODEL_CHIP_TEST_NAMESPACE::Mau_addrmap_map* mau_addrmap_map_=0;
  uint64_t present_time_=0; // to communicate forwarding to SALU

  PhvMap phvs_;
  EopMap eops_;
  TeopMap teops_;

 private:
  void stateful_test_internal(std::vector<uint32_t> phv_d, std::vector<uint32_t> data_in, uint32_t addr, uint64_t present_time,
                              std::vector<bool> match_bus, std::vector<bool> learn_or_match_bus,
                              boost::optional<uint64_t> random_number,
                              std::vector<uint32_t> data_out, std::vector<uint32_t> action_out, bool check_action);

};
}

#endif // _UTESTS_READER_ACTIONS_
