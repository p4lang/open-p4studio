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

#include "test_wrapper.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// defined in test_jbay_deparse.cpp
void setup_deparser(int pipe, TestUtil& tu, Phv *phv, int stage, bool is_egress);


TEST_F(BFN_TEST_NAME(WrapperTestFixture),Deparser) {
  //tu_->get_objmgr()->deparser_get(pipe)->ingress()->set_log_flags(0xFF);
  Phv phv(nullptr, nullptr);
  setup_deparser(pipe_index(), *tu_, &phv, stage_index(), false);
  tu_->deparser_set_egress_unicast_port_info(pipe_index(),
                                             0,  // phv
                                             16,  // pov bit
                                             false);  // disable

  auto do_deparse = [this](uint16_t input_die,
                           uint16_t egress_die,
                           uint32_t* meta_data){
    for (int i = 0; i < ING_META_FIELDS; i++) meta_data[i] = 0u;
    uint32_t is_ingr = 1;
    uint32_t phv_data[224] {};
    // set PHV bit pointed to for POV index 16
  //  phv_data[Phv::make_word_d(6,1)] = 0xFFFFFFFFu;
    phv_data[Phv::make_word_d(6,2)] = 0x1u;
    uint16_t die_local_input_port = Port::make_port_index(pipe_index(), 0x4);
    uint16_t input_port_num = die_local_input_port | (input_die << 9);
    // set egress unicast port in phv
    uint16_t die_local_egress_port = 0x7;
    uint16_t egress_port = die_local_egress_port | (egress_die << 9);
    phv_data[0] = egress_port;

    uint32_t phv_vld[7] {};
    for (int i=0; i<7; i++) phv_vld[i] = 0xFFFFFFFF;
    uint32_t phvt_data[112] {0};
    uint32_t phvt_vld[4] {0};

    uint32_t *mau_err = nullptr;
    uint32_t orig_hdr_len = 54;
    uint32_t version = 0;
    uint32_t output_pkt_length = 0, mirror_pkt_length = 0, lfltr_pkt_valid = 0,
        resubmit_meta_data_valid = 0, pkt_dropped = 0;
    std::unique_ptr<uint8_t[]> output_pkt_data, mirror_pkt_data, lfltr_pkt_data,
        resubmit_meta_data;

    // this method is intended for tofinoXX since it takes phvt data, but we
    // can nevertheless test it with a jbay config
    wrapper_->deparser_model_process_packet(
        is_ingr,
        phv_data, phv_vld,
        phvt_data, phvt_vld,
        mau_err,
        tcp_pktstr_,
        orig_hdr_len,
        version,
        output_pkt_length, output_pkt_data,
        mirror_pkt_length, mirror_pkt_data,
        input_port_num,
        meta_data,
        lfltr_pkt_valid, lfltr_pkt_data,
        resubmit_meta_data_valid, resubmit_meta_data,
        pkt_dropped);
    EXPECT_EQ(0u, pkt_dropped);
    // jbay port ids are 9 bits wide, WIP are 11 bits wide
    uint32_t port_mask = RmtObject::is_chip1() ? 0x7FFu : 0x1FFu;
    EXPECT_EQ(input_port_num & port_mask, meta_data[ING_INPORT_PORT]);
    EXPECT_EQ(1u, meta_data[ING_EPIPE_PORT_VLD]);
    EXPECT_EQ(egress_port & port_mask, meta_data[ING_EPIPE_PORT]);
  };

  auto do_deparse_clot = [this](uint16_t input_die,
                                uint16_t egress_die,
                                uint32_t* meta_data){
    for (int i = 0; i < ING_META_FIELDS; i++) meta_data[i] = 0u;
    uint32_t is_ingr = 1;
    uint32_t phv_data[224] {};
    // set PHV bit pointed to for POV index 16
  //  phv_data[Phv::make_word_d(6,1)] = 0xFFFFFFFFu;
    phv_data[Phv::make_word_d(6,2)] = 0x1u;
    uint16_t die_local_input_port = Port::make_port_index(pipe_index(), 0x4);
    uint16_t input_port_num = die_local_input_port | (input_die << 9);
    // set egress unicast port in phv
    uint16_t die_local_egress_port = 0x7;
    uint16_t egress_port = die_local_egress_port | (egress_die << 9);
    phv_data[0] = egress_port;

    uint32_t phv_vld[7] {};
    for (int i=0; i<7; i++) phv_vld[i] = 0xFFFFFFFF;
    uint32_t clot_data[128] {0};
    uint32_t *clot_vld = nullptr;

    uint32_t *mau_err = nullptr;
    uint32_t orig_hdr_len = 54;
    uint32_t version = 0;
    uint32_t output_pkt_length = 0, mirror_pkt_length = 0, lfltr_pkt_valid = 0,
        resubmit_meta_data_valid = 0, pgen_meta_data_valid = 0,
        pgen_address = 0, pgen_length = 0, pkt_dropped = 0;
    std::unique_ptr<uint8_t[]> output_pkt_data, mirror_pkt_data, lfltr_pkt_data,
        resubmit_meta_data, pgen_meta_data;

    wrapper_->deparser_model_process_packet_clot(
        0,
        0,
        is_ingr,
        phv_data, phv_vld,
        clot_data, clot_vld,
        mau_err,
        tcp_pktstr_,
        orig_hdr_len,
        version,
        output_pkt_length, output_pkt_data,
        mirror_pkt_length, mirror_pkt_data,
        input_port_num,
        meta_data,
        lfltr_pkt_valid, lfltr_pkt_data,
        resubmit_meta_data_valid, resubmit_meta_data,
        pgen_meta_data_valid, pgen_meta_data, pgen_address, pgen_length,
        pkt_dropped);
    EXPECT_EQ(0u, pkt_dropped);
    // jbay port ids are 9 bits wide, WIP are 11 bits wide
    uint32_t port_mask = RmtObject::is_chip1() ? 0x7FFu : 0x1FFu;
    EXPECT_EQ(input_port_num & port_mask, meta_data[ING_INPORT_PORT]);
    EXPECT_EQ(1u, meta_data[ING_EPIPE_PORT_VLD]);
    EXPECT_EQ(egress_port & port_mask, meta_data[ING_EPIPE_PORT]);
  };

  uint32_t meta_data[ING_META_FIELDS] {};

  {
    SCOPED_TRACE("input die 0, egress die 1");
    uint32_t expected_tm_vec = RmtObject::is_chip1() ? 0x1u : 0x0u;
    do_deparse(0x0, 0x1, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
    do_deparse_clot(0x0, 0x1, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
  }
  {
    SCOPED_TRACE("input die 0, egress die 2");
    uint32_t expected_tm_vec = RmtObject::is_chip1() ? 0x2u : 0x0u;
    do_deparse(0x0, 0x2, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
    do_deparse_clot(0x0, 0x2, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
  }
  {
    SCOPED_TRACE("input die 1, egress die 2");
    uint32_t expected_tm_vec = RmtObject::is_chip1() ? 0x2u : 0x0u;
    do_deparse(0x1, 0x2, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
    do_deparse_clot(0x1, 0x2, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
  }
  // Had to change the WIP expected TM VEC vals post
  // 2-die wrapper mod changes of April 2020
  {
    SCOPED_TRACE("input die 2, egress die 2");
    uint32_t expected_tm_vec = RmtObject::is_chip1() ? 0x2u : 0x0u;
    do_deparse(0x2, 0x2, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
    do_deparse_clot(0x2, 0x2, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
  }
  {
    SCOPED_TRACE("input die 2, egress die 3");
    uint32_t expected_tm_vec = RmtObject::is_chip1() ? 0x2u : 0x0u;
    do_deparse(0x2, 0x3, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
    do_deparse_clot(0x2, 0x3, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
  }
  {
    SCOPED_TRACE("input die 3, egress die 0");
    uint32_t expected_tm_vec = RmtObject::is_chip1() ? 0x1u : 0x0u;
    do_deparse(0x3, 0x0, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
    do_deparse_clot(0x3, 0x0, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
  }
  {
    SCOPED_TRACE("input die 3, egress die 1");
    uint32_t expected_tm_vec = RmtObject::is_chip1() ? 0x1u : 0x0u;
    do_deparse(0x3, 0x1, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
    do_deparse_clot(0x3, 0x1, meta_data);
    EXPECT_EQ(expected_tm_vec, meta_data[ING_TM_VEC]);
  }
}

}
