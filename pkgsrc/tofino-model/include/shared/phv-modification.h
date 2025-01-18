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

#ifndef _SHARED_PHV_MODIFICATION_
#define _SHARED_PHV_MODIFICATION_

#include <mau-object.h>
#include <phv.h>
#include <model_core/rmt-phv-modification.h>

namespace MODEL_CHIP_NAMESPACE {

  class PhvModification : public MauObject {
      public: 
        PhvModification(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
        virtual ~PhvModification();

        int set_modification(model_core::RmtPhvModification::ActionEnum action, 
                            int index, uint32_t value);
        void apply_modification(Phv *p, bool verbose);

        inline Phv* get_xor_phv() {return xor_phv_;}
        inline Phv* get_or_phv() {return or_phv_;}
        inline Phv* get_clr_phv() {return clr_phv_;}

        inline void set_xor_phv(Phv* xor_phv) {xor_phv_ = xor_phv;}
        inline void set_or_phv(Phv* or_phv) {or_phv_ = or_phv;}
        inline void set_clr_phv(Phv* clr_phv) {clr_phv_ = clr_phv;}

        inline void set_xor_phv(int index, uint32_t value) {
          if (xor_phv_ == nullptr) set_xor_phv(new Phv (get_object_manager())); 
          xor_phv_->clobber_x(index, value);
        }
        inline void set_or_phv(int index, uint32_t value) {
          if (or_phv_ == nullptr) set_or_phv(new Phv (get_object_manager())); 
          or_phv_->clobber_x(index, value);
        }
        inline void set_clr_phv(int index, uint32_t value) {
          if (clr_phv_ == nullptr) set_clr_phv(new Phv (get_object_manager())); 
          clr_phv_->clobber_x(index, value);
        }
      private:
        Phv* xor_phv_{nullptr};
        Phv* or_phv_{nullptr};
        Phv* clr_phv_{nullptr};
  };
}
#endif // _SHARED_PHV_MODIFICATION_
