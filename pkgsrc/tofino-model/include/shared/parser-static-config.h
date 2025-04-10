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

#ifndef _SHARED_PARSER_CONFIG_
#define _SHARED_PARSER_CONFIG_

#include <bitvector.h>
#include <parser.h>
#include <deparser.h>

namespace MODEL_CHIP_NAMESPACE {

class ParserStaticConfig {
 public:
  static bool      T;
  static bool      F;
  static bool      b_FF[];
  static bool      b_FT[];
  static bool      b_FFFF[];
  static bool      b_FFFT[];
  static bool      b_TTTT[];
  static uint8_t   u8_00[];

  static uint16_t  NoX;

  static uint16_t  DA_HI_16;
  static uint16_t  DA_LO_32;
  static uint16_t  SA_HI_16;
  static uint16_t  SA_LO_32;
  static uint16_t  ETH_TYPE;
  static uint16_t  IP4_HL;
  static uint16_t  IP4_TTL;
  static uint16_t  IP4_PROTO;
  static uint16_t  IP4_ERR;
  static uint16_t  IP4_LEN;
  static uint16_t  IP4_ID;
  static uint16_t  IP4_FRAG;
  static uint16_t  IP4_CKSM;
  static uint16_t  IP4_SRC;
  static uint16_t  IP4_DST;
  static uint16_t  P_SPORT;
  static uint16_t  P_DPORT;
  static uint16_t  TCP_CKSM;
  static uint16_t  TCP_RESID;
  static uint16_t  PHV8_0;
  static uint16_t  PHV8_1;
  static uint16_t  PHV8_2;
  static uint16_t  PHV8_3;
  static uint16_t  PHV8_4;
  static uint16_t  PHV8_5;
  static uint16_t  PHV8_6;
  static uint16_t  PHV8_7;
  static uint16_t  PHV8_8;
  static uint16_t  PHV8_9;
  static uint16_t  PHV8_10;
  static uint16_t  PHV8_11;

  static void config_eth_header(Parser *parser, int index, int state, int next_state);
  static void config_ip_header(Parser *parser, int index, int state, int next_state);
  static void config_tcp_header(Parser *parser, int index, int state, int next_state, bool done);
  static void config_udp_header(Parser *parser, int index, int state, int next_state);
  static void config_catch_all(Parser *parser, int index, int state, int next_state);
  static void config_payload_n(Parser *parser, int index, int state, int next_state,
                               bool done, std::array<uint16_t, 4> dst_phv={NoX,NoX,NoX,NoX},
                               int shift=32);
  static void parser_config_basic_eth_ip_tcp(Parser *parser);
  static void deparser_config_basic_eth_ip_tcp(Deparser *deparser);
};

  void parser_config_basic_eth_ip_tcp(Parser *parser);
  void deparser_config_basic_eth_ip_tcp(Deparser *deparser);

}

#endif // _SHARED_PARSER_CONFIG_

