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

#ifndef _SHARED_MAU_OBJECT_
#define _SHARED_MAU_OBJECT_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <mau-event-emitter.h>

namespace MODEL_CHIP_NAMESPACE {

  class Mau;

  class MauObject : public PipeObject, public MauEventEmitter {

 public:
    MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex);
    MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
              Mau *mau);
    MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
              int typeIndex);
    MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
              int typeIndex, Mau *mau);
    MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
              int typeIndex, int rowtabIndex);
    MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
              int typeIndex, int rowtabIndex, Mau *mau);
    MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
              int typeIndex, int rowtabIndex, int colIndex);
    MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
              int typeIndex, int rowtabIndex, int colIndex, Mau *mau);
    MauObject(const MauObject& other) = delete;  // XXX
    virtual ~MauObject();

    inline int   mau_index()  const { return mau_index_; }
    inline Mau  *mau()        const { return mau_; }
    inline void  set_mau(Mau *mau)  { mau_ = mau; }

    Mau *mau_lookup();

 private:
    MauObject& operator=(const MauObject&){ return *this; } // XXX
    int    mau_index_;
    Mau   *mau_;
  };
}
#endif // _SHARED_MAU_OBJECT_
