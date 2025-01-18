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

// This is a test for the test_reader code, it uses DummyActions, so it is self contained and produces
//  an executable that will read a test, parser it and then print some stuff out (as defined in the dummy
//  actions). This is much faster to rebuild when the grammer changes than the utests.
// The DummyActions must be kept up to date with the real actions in reader_actions.[h|cpp] or you will
//  get strange compile errors.

#include "test_reader_grammar.h"

// Some dummy actions for testing the parser
struct DummyActions {
  void set(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,int valid)
  {
    int e = oe ? *oe : 0;
    printf("set %cphv_%s %de%d %d %x %d\n",c, s.c_str(), n, e,a0,a1,valid);
  }
  void phv_alloc(const char c, const std::string s, const int n,  boost::optional<int> oe)
  {
    int e = oe ? *oe : 0;
    printf("phv_alloc %cphv_%s %de%d\n",c, s.c_str(), n, e);
  }
  void set_ingress(const char c, const std::string s, const int n,  boost::optional<int> oe)
  {
    int e = oe ? *oe : 0;
    printf("set_ingress %cphv_%s %de%d\n",c, s.c_str(), n, e);
  }
  void set_egress(const char c, const std::string s, const int n,  boost::optional<int> oe)
  {
    int e = oe ? *oe : 0;
    printf("set_egress %cphv_%s %de%d\n",c, s.c_str(), n, e);
  }
  void set_ghost(const char c, const std::string s, const int n,  boost::optional<int> oe)
  {
    int e = oe ? *oe : 0;
    printf("set_ghost %cphv_%s %de%d\n",c, s.c_str(), n, e);
  }
  void set_version(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,bool a1)
  {
      int e = oe ? *oe : 0;
    printf("set_version %cphv_%s %de%d %d %s\n",c, s.c_str(), n, e,a0,a1?"true":"false");
  }
  void set_meter_tick_time(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,uint64_t a2)
  {
    int e = oe ? *oe : 0;
    printf("set_meter_tick_time %cphv_%s %de%d %d %d %" PRIi64 " \n",c, s.c_str(), n, e,a0,a1,a2);
  }
  void set_meter_random_value(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1,uint64_t a2)
  {
    int e = oe ? *oe : 0;
    printf("set_meter_tick_time %cphv_%s %de%d %d %d %" PRIi64 " \n",c, s.c_str(), n, e,a0,a1,a2);
  }
  void set_relative_time(const char c, const std::string s, const int n,  boost::optional<int> oe , uint64_t a0)
  {
    int e = oe ? *oe : 0;
    printf("set_relative_time %cphv_%s %de%d %" PRIi64 "\n",c, s.c_str(), n, e,a0);
  }
  void set_eopnum(const char c, const std::string s, const int n,  boost::optional<int> oe , int a0,int a1, bool a2)
  {
    int e = oe ? *oe : 0;
    printf("set_eopnum %cphv_%s %de%d 0x%x %d %s\n",c, s.c_str(), n, e,a0,a1,a2?"true":"false");
  }
  void selector_test(int a0,uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
  {
    printf("selector_test %d %x %x %x %x\n",a0,a1,a2,a3,a4);
  }

  void stateful_test(uint64_t phv_d, std::vector<uint32_t> data_in, uint32_t q3, bool forwarding, std::vector<uint32_t> data_out, boost::optional<int> action_out)
  {
    bool action_valid = action_out ? true:false;
    uint32_t action = action_valid ? *action_out : 0;
    printf("stateful_test phv=%" PRIx64 " (%x %x %x %x) %x %s (%x %x %x %x) %s %x\n",phv_d,data_in[0],data_in[1],data_in[2],data_in[3],q3,
           forwarding?"true":"false",data_out[0],data_out[1],data_out[2],data_out[3],action_valid?"":"no action",action);
  }
  
  void stateful_test_jbay(std::vector<uint32_t> phv_d, std::vector<uint32_t> data_in,
                          boost::fusion::vector4<int, uint64_t ,std::vector<bool>,std::vector<bool>> params,
                          boost::optional<uint64_t> random_nunmber,
                          std::vector<uint32_t> data_out, std::vector<uint32_t> action_out) 
  {
    using boost::fusion::at_c;
    printf("stateful_test phv=(%x %x %x %x) data_in=(%x %x %x %x) addr=%x present_time=%" PRIi64 "\n   data_out=(%x %x %x %x) action_out=(%x %x %x %x)\n",phv_d[3],phv_d[2],phv_d[1],phv_d[0],data_in[3],data_in[2],data_in[1],data_in[0],
           at_c<0>(params),at_c<1>(params),
           data_out[3],data_out[2],data_out[1],data_out[0],action_out[3],action_out[2],action_out[1],action_out[0]);
  }
  
  void set_pred(int i_st, int e_st, int g_st, uint16_t gex, uint8_t lbr) {
    printf("set_pred %d %d %d 0x%x 0x%x\n",i_st,e_st,g_st,gex,lbr);
  }
  void set_mpr(int i_st, int e_st, int g_st, uint16_t gex, uint8_t lbr) {
    printf("set_mpr %d %d %d 0x%x 0x%x\n",i_st,e_st,g_st,gex,lbr);
  }
  void set_ingress_snapshot_triggered(bool tf) {
    printf("set_ingress_snapshot_triggered %s\n", tf?"true":"false");
  }
  void set_egress_snapshot_triggered(bool tf) {
    printf("set_egress_snapshot_triggered %s\n", tf?"true":"false");
  }

  
  // pointers to start tables here because I can't work out how to get them to lazily evaluate in the grammer
  void process_match2(int a0, int a1, int* ingress_start_table,int* egress_start_table,int* ghost_start_table)
  {
    printf("process_match2 %de%d i_start=%d e_start=%d g_start=%d\n",
           a0, a1,
           *ingress_start_table,*egress_start_table,*ghost_start_table);
  }
  void process_match(int a0, int a1, int* ingress_start_table,int* egress_start_table)
  {
    printf("process_match %de%d i_start=%d e_start=%d\n",
           a0, a1,
           *ingress_start_table,*egress_start_table);
  }
  void process_action(int a0, int a1 )
  {
    printf("process_action %de%d\n",a0,a1);
  }
  void new_eop(int a0, int a1 )
  {
    printf("new_eop %d %d\n",a0,a1);
  }
  void handle_eop(int a0, int a1 )
  {
    printf("handle_eop %d %d\n",a0,a1);
  }
  void set_ingress_eopinfo(int a0, int a1, int a2, int a3, int a4 )
  {
    printf("set_ingress_eopinfo %d %d = 0x%x 0x%x err=%d\n",a0,a1,a2,a3,a4);
  }
  void set_egress_eopinfo(int a0, int a1, int a2, int a3, int a4 )
  {
    printf("set_egress_eopinfo %d %d = 0x%x 0x%x err=%d\n",a0,a1,a2,a3,a4);
  }
  void eop_set_meter_tick_time(int a0,int a1, int a2, int a3, uint64_t a4)
  {
    printf("eop_set_meter_tick_time %d %d %d %d %" PRIx64 "\n",a0,a1,a2,a3,a4);
  }
  void eop_set_meter_random_value(int a0,int a1, int a2, int a3, uint64_t a4)
  {
    printf("eop_set_meter_random_value %d %d %d %d %" PRIx64 "\n",a0,a1,a2,a3,a4);
  }
  void eop_set_relative_time(int a0,int a1, uint64_t a2)
  {
    printf("eop_set_relative_time %d %d %" PRIi64 "\n",a0,a1,a2);
  }  
  void new_teop(int a0, int a1 )
  {
    printf("new_teop %d %d\n",a0,a1);
  }
  void handle_dp_teop(int a0, int a1 )
  {
    printf("handle_dp_teop %d %d\n",a0,a1);
  }
  void teop_set_raw_addr(int a0, int a1, int a2, int a3 )
  {
    printf("teop_set_raw_addr %d %d = 0x%x 0x%x\n",a0,a1,a2,a3);
  }
  void teop_set_byte_len(int a0, int a1, int a2 )
  {
    printf("teop_set_byte_len %d %d = %d\n",a0,a1,a2);
  }
  void teop_set_error(int a0, int a1, int a2 )
  {
    printf("teop_set_error %d %d = 0x%x\n",a0,a1,a2);
  }
  void teop_set_meter_tick_time(int a0,int a1, int a2, int a3, uint64_t a4)
  {
    printf("teop_set_meter_tick_time %d %d %d %d %" PRIx64 "\n",a0,a1,a2,a3,a4);
  }
  void teop_set_meter_random_value(int a0,int a1, int a2, int a3, uint64_t a4)
  {
    printf("teop_set_meter_random_value %d %d %d %d %" PRIx64 "\n",a0,a1,a2,a3,a4);
  }
  void teop_set_relative_time(int a0,int a1, uint64_t a2)
  {
    printf("teop_set_relative_time %d %d %" PRIi64 "\n",a0,a1,a2);
  }
  void reset()
  {
    printf("reset\n");
  }
  void indirect_write(uint64_t a0,uint64_t a1, uint64_t a2,boost::optional<uint64_t> T)
  {
    printf("indirect_write  %" PRIx64 " %" PRIx64 " %" PRIx64 "\n",a0,a1,a2);
  }
  void remap_indirect_write(int a0,int a1,int a2, int a3,uint64_t a4,uint64_t a5, uint64_t a6)
  {
    printf("remap_indirect_write %d %d %d %d %" PRIx64 " %" PRIx64 " %" PRIx64 "\n",a0,a1,a2,a3,a4,a5,a6);
  }
  void out_word(std::vector<PathElement> a0,uint32_t a1)
  {
    
    printf("out_word ");
    for (PathElement el : a0) {
      // I am not sure why at<> needs a special int arg, but it seems to
      printf("%s",boost::fusion::at<boost::mpl::int_<0>>( el ).c_str() );
      auto el_array = boost::fusion::at<boost::mpl::int_<1>>( el );
      if (el_array) {
        for (int dim : *el_array) {
          printf("[%d]",dim);
        }
      }
      printf(".");
    }
    printf(" 0x%x\n",a1);
  }
  void out_word_addr(uint32_t a0,uint32_t a1)
  {
    printf("out_word_addr\n ");
  }
  void out_word_pit(int a0, int a1, std::vector<PathElement> a2,uint32_t a3)
  {
    
    printf("out_word_pit %d,%d ",a0,a1);
    for (PathElement el : a2) {
      printf("%s",boost::fusion::at<boost::mpl::int_<0>>( el ).c_str() );
      auto el_array = boost::fusion::at<boost::mpl::int_<1>>( el );
      if (el_array) { // boost optional means it may be null
        for (int dim : *el_array) {
          printf("[%d]",dim);
        }
      }
      printf(".");
    }
    printf(" 0x%x\n",a3);
  }
  void out_word_pet(int a0, int a1, std::vector<PathElement> a2,uint32_t a3)
  {
    
    printf("out_word_pet %d,%d ",a0,a1);
    for (PathElement el : a2) {
      printf("%s",boost::fusion::at<boost::mpl::int_<0>>( el ).c_str() );
      auto el_array = boost::fusion::at<boost::mpl::int_<1>>( el );
      if (el_array) { // boost optional means it may be null
        for (int dim : *el_array) {
          printf("[%d]",dim);
        }
      }
      printf(".");
    }
    printf(" 0x%x\n",a3);
  }
  void aht_new()
  {
    printf("aht_new\n");
  }
  void aht_ctl_word(int a0, int a1, int a2, int a3 )
  {
    printf("aht_ctl_word %d %d %d %d\n",a0,a1,a2,a3);
  }
  void aht_do_writes()
  {
    printf("aht_do_writes\n");
  }
  uint32_t rm_b4_x( boost::optional<int> a0, uint32_t a1 ) {
    // this is mapped by a define in test util in the real actions
    int v= a0 ? *a0 : 0;
    printf("rm_b4_x (%d) %x\n",v,a1);
    return a1;
  }
};



class DummyTestReader {
 public:
  DummyTestReader() {}

  typedef boost::spirit::istream_iterator iterator_type;
  void read_file(const char* file_name) {
    CommentSkipper<iterator_type> skipper;      

      // open file, disable skipping of whitespace
      std::ifstream in(file_name);
      in.unsetf(std::ios::skipws);
 
      // wrap istream into iterator
      iterator_type iter(in);
      iterator_type end;

      phrase_parse(iter, end, s, skipper );
  }
  DummyActions* get_action() { return &s.action; }
  TestReaderGrammar<iterator_type,DummyActions> s;

};


int main(int argc,char* argv[])
{

  DummyTestReader tr;
  if (argc==2) {
    tr.read_file(argv[1]);
  }

  return 0;

}
