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

#include <mau-hash-generator-with-reg.h>

namespace MODEL_CHIP_NAMESPACE {
  // register callbacks
  void MauHashGeneratorWithReg::GaloisFieldMatrixCallback(uint32_t a1,uint32_t a0) {
    int byte_start = a1*16;
    galois_field_matrix_bv_[a0].set_word( galois_field_matrix_.byte0(a1,a0),
                                          byte_start,
                                          8 );
    galois_field_matrix_bv_[a0].set_word( galois_field_matrix_.byte1(a1,a0),
                                          byte_start+8,
                                          8 );
    // note: w.r.t tofinoxx, the valid bits have been removed in jbay
  }

}
