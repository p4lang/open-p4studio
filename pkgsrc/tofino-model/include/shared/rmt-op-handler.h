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

#ifndef _SHARED_RMT_OP_HANDLER_
#define _SHARED_RMT_OP_HANDLER_

#include <memory>
#include <rmt-defs.h>
#include <rmt-object.h>

namespace MODEL_CHIP_NAMESPACE {

  class RmtOpHandler : public RmtObject {

 public:
    RmtOpHandler(RmtObjectManager *om);
    virtual ~RmtOpHandler();


    void set_tcam_writereg(int pipe, int stage, int mem, uint32_t address, 
                           uint64_t data_0, uint64_t data_1, bool write_tcam=true);


    void tcam_copy_word(int pipe, int stage,
                        int src_table_id, int dst_table_id, int num_tables,
                        int num_words, int adr_incr_dir,
                        uint32_t src_address, uint32_t dst_address);


    // These not implemented initially

    void set_memdata(int pipe, int stage, int mem, uint32_t address, uint8_t data_size,
                     uint64_t data_0, uint64_t data_1) { }

    void tcam_invalidate_word(int pipe, int stage,
                              int table_id, int num_tables,
                              uint32_t src_address) { }

    void ism_init(int pipe, int stage,
                  int table_id, uint32_t v_address,
                  bool init, bool srcloc_uses_idletime,
                  bool srcloc_uses_stats, bool unlock) { }


    // Couldn't find these - changed?

    // (set_table_qualif_reg  pipe stage table_id dm_address ????
    // (set_internal_version  pipe stage table_id ????
    // (disable_table_qualif_reg ????

  };
}

#endif // _SHARED_RMT_OP_HANDLER_
