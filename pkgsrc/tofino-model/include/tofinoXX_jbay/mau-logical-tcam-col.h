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

#ifndef _TOFINOXX_JBAY_MAU_LOGICAL_TCAM_COL_
#define _TOFINOXX_JBAY_MAU_LOGICAL_TCAM_COL_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>

// Reg defs auto-generated from Semifore
#include <register_includes/tcam_table_map.h>


namespace MODEL_CHIP_NAMESPACE {

class MauLogicalTcam;

class MauLogicalTcamCol {
 public:
  MauLogicalTcamCol(int chipIndex, int pipeIndex, int mauIndex,
                    int ltcamIndex, int colIndex,
                    MauLogicalTcam *mauLogicalTcam);
  ~MauLogicalTcamCol();

  inline int      col_index()  const { return col_index_; }
  inline uint32_t table_map()        { return curr_table_map_; }

  void set_table_map(uint32_t v);

 private:
  void table_map_write_callback();

  MauLogicalTcam                 *mau_logical_tcam_;
  int                             col_index_;
  uint32_t                        curr_table_map_;
  register_classes::TcamTableMap  tcam_table_map_;
};

}
#endif // _TOFINOXX_JBAY_MAU_LOGICAL_TCAM_COL_
