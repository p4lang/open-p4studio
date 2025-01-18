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

#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <deparser-learning.h>
#include <register_adapters.h>
#include <deparser-reg.h>


namespace MODEL_CHIP_NAMESPACE {

DeparserLearning::DeparserLearning(RmtObjectManager *om, int pipeIndex, DeparserReg &deparser_reg)
    : PipeObject(om, pipeIndex, 0x3f, RmtTypes::kRmtTypeDeparser),
      learn_table_entry_array_(deparser_in_hdr_adapter(learn_table_entry_array_,chip_index(), pipeIndex)),
      learn_mask_entry_array_(deparser_in_hdr_adapter(learn_mask_entry_array_,chip_index(), pipeIndex)),
      deparser_reg_( deparser_reg )
{
  RMT_LOG_VERBOSE("DeparserLearning::create\n");
  learn_table_entry_array_.reset();
  learn_mask_entry_array_.reset();
}
DeparserLearning::~DeparserLearning() {
  RMT_LOG_VERBOSE("DeparserLearning::delete\n");
}

void DeparserLearning::CalculateLearningQuantum(const Phv &phv, const BitVector<Deparser::kPovWidth>& pov, LearnQuantumType* lq) {

  uint64_t table_index;
  bool valid = deparser_reg_.get_learn_sel_info(phv,pov,&table_index);

  if (valid) {
    lq->valid = true;

    // TODO: this mask should not be needed for a real config, but for now
    //  I'm not programming the phv's properly so table_index could be anything
    table_index &= 0x7;
    RMT_ASSERT( table_index < 8 ); // TODO: get constant from reg.h? Or somewhere

    if (learn_table_entry_array_.valid(table_index)) {

      lq->length = learn_table_entry_array_.len(table_index);
      int last_source_phv = -1;
      int shift = 0;
      for (int i=0;i<48;++i) {
        uint8_t byte=0; // bytes after the lq length are zeroed
        if ( i < lq->length ) {
          int p_index = 47 - i;
          int source_phv = learn_table_entry_array_.phvs(table_index,p_index);
          if ( phv.is_valid_phv_d( source_phv ) ) {
            uint32_t phv_word = phv.get_d( source_phv );
            int width = phv.which_width_d( source_phv );
            if (last_source_phv == source_phv) {
              if (width > 8) {
                shift -= 8;
                // check haven't used all the bytes already
                RMT_ASSERT( shift >= 0 );
              }
              else {
                // OK to have consecutive 8bit PHVs, leave shift at zero.
              }
            }
            else {
              // the first byte of a new phv word
              shift = width - 8; // big endian order
              // Check that all the bytes of a phv word are choosen as this
              //  code assumes that they are. You don't have to output
              //  all bytes of a phv word, but this case will have strange
              //  behaviour that I haven't modelled (due to the auto swizzle)
              for (int j=0;j<(width/8);++j) {
                RMT_ASSERT( (j + i) < 48 );
                RMT_ASSERT( phv_word == phv.get_d( learn_table_entry_array_.phvs(table_index,p_index) ) );
              }
            }
            uint8_t mask = 0xff & (learn_mask_entry_array_.mask(table_index,p_index/4) >> (8*(p_index%4)));
            byte = (phv_word >> shift) & mask;
            // printf(">> table_index=%d p_index=%d mask32=0x%08x mask8=0x%02x\n",table_index, p_index,
            //        learn_mask_entry_array_.mask(table_index,p_index/4),mask);
            // printf(">> byte=0x%02x phv_word=0x%x shift=%d\n",byte,phv_word,shift);
            last_source_phv = source_phv;

            if (shift == 0) {
              // XXX: Add P4 level logging
              uint64_t log_flag = RmtDebug::kRmtDebugVerbose;
              if (rmt_log_type_check(log_flag, RMT_LOG_TYPE_P4)) {
                uint32_t mask32 = (0xFFFFFFFFu >> (32-width)) &
                    learn_mask_entry_array_.mask(table_index, p_index/4);
                std::string name = get_object_manager()->get_phv_name(pipe_index(), 19, source_phv);
                RMT_TYPE_LOG(
                    RMT_LOG_TYPE_P4, log_flag,
                    "LearningFilter::LQ[%2d..%2d] = %*s%0*x  Mask %*s%0*x  PHV word %3d  %s\n",
                    p_index, p_index + (width/8) - 1,
                    10-(width/4), "0x", width/4, phv_word & mask32,
                    10-(width/4), "0x", width/4, mask32,
                    source_phv, name.c_str());
              }
            }
          }
          else {
            // phv word selected does not exist
            //  TODO: warn?
            printf("Phvs[ %d ] = %d\n",i,source_phv);
            RMT_ASSERT(0);
          }
        }
        lq->data.set_byte(byte,47-i);
      }
    }
    else {
      // entry is not valid
      lq->valid = false;
    }
  }
  else {
    // cfg not valid or configured phv that is not valid
    lq->valid = false;
  }

}

}
