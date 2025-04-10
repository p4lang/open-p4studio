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

#include <mau.h>
#include <rmt-op-handler.h>
#include <string>
#include <memory>
#include <rmt-log.h>
#include <rmt-object-manager.h>


namespace MODEL_CHIP_NAMESPACE {

  RmtOpHandler::RmtOpHandler(RmtObjectManager *om) : RmtObject(om) {
  }
  RmtOpHandler::~RmtOpHandler() { }

  void RmtOpHandler::set_tcam_writereg(int pipe, int stage,
                                       int mem,
                                       uint32_t address, 
                                       uint64_t data_0, uint64_t data_1,
                                       bool write_tcam) {
    RmtObjectManager *om = get_object_manager();
    RMT_ASSERT (om != NULL);
    Mau *mau = om->mau_lookup(pipe, stage);
    RMT_ASSERT (mau != NULL);
    MauOpHandler *mau_ops = mau->mau_op_handler();
    RMT_ASSERT (mau_ops != NULL);
    (void)mau_ops->set_tcam_writereg(mem, address,
                                     data_0, data_1,
                                     write_tcam);
  }

  void RmtOpHandler::tcam_copy_word(int pipe, int stage,
                                    int src_table_id,
                                    int dst_table_id,
                                    int num_tables,
                                    int num_words,
                                    int adr_incr_dir,
                                    uint32_t src_address,
                                    uint32_t dst_address) {
    RmtObjectManager *om = get_object_manager();
    RMT_ASSERT (om != NULL);
    Mau *mau = om->mau_lookup(pipe, stage);
    RMT_ASSERT (mau != NULL);
    MauOpHandler *mau_ops = mau->mau_op_handler();
    RMT_ASSERT (mau_ops != NULL);
    (void)mau_ops->tcam_copy_word(src_table_id, dst_table_id,
                                  num_tables, num_words,
                                  adr_incr_dir,
                                  src_address, dst_address);                                  
  }
}
