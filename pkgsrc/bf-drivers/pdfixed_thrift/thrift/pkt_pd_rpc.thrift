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

include "res.thrift"

namespace py pkt_pd_rpc
namespace cpp pkt_pd_rpc
namespace c_glib pkt_pd_rpc

struct pkt_pkt_data {
    1: required i32 rx_ring;
    2: required i32 size;
    3: required binary buf;
}

exception InvalidPktOperation {
  1:i32 code
}

service pkt {
    # Test echo interface
    void echo(1:string s);

    void init();

    void cleanup();

    res.SessionHandle_t client_init() throws (1:InvalidPktOperation ouch);

    void test_send_pkt(1:res.SessionHandle_t sess_hdl, 2:binary buf, 3:i32 size, 4:i32 tx_ring) throws (1:InvalidPktOperation ouch);

    void test_verify_pkt_tx(1:res.SessionHandle_t sess_hdl) throws (1:InvalidPktOperation ouch);

    pkt_pkt_data test_get_pkt_rx(1:res.SessionHandle_t sess_hdl) throws (1:InvalidPktOperation ouch);

    void client_cleanup(1:res.SessionHandle_t sess_hdl) throws (1:InvalidPktOperation ouch);
}
