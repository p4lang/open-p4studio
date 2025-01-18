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

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-object.h>


namespace MODEL_CHIP_NAMESPACE {

  MauObject::MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex)
      : PipeObject(om,pipeIndex,mauIndex,0x3F,0x3F,0x3F),
        MauEventEmitter(om,this),
        mau_index_(mauIndex), mau_(NULL) {
  }
  MauObject::MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       Mau *mau)
      : PipeObject(om,pipeIndex,mauIndex,0x3F,0x3F,0x3F),
        MauEventEmitter(om,this),
        mau_index_(mauIndex), mau_(mau) {
  }
  MauObject::MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       int typeIndex)
      : PipeObject(om,pipeIndex,mauIndex,typeIndex,0x3F,0x3F),
        MauEventEmitter(om,this),
        mau_index_(mauIndex), mau_(NULL) {
  }
  MauObject::MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       int typeIndex, Mau *mau) 
      : PipeObject(om,pipeIndex,mauIndex,typeIndex,0x3F,0x3F),
        MauEventEmitter(om,this),
        mau_index_(mauIndex), mau_(mau) {
  }
  MauObject::MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       int typeIndex, int rowtabIndex)
      : PipeObject(om,pipeIndex,mauIndex,typeIndex,rowtabIndex,0x3F),
        MauEventEmitter(om,this),
        mau_index_(mauIndex), mau_(NULL) {
  }
  MauObject::MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       int typeIndex, int rowtabIndex, Mau *mau) 
      : PipeObject(om,pipeIndex,mauIndex,typeIndex,rowtabIndex,0x3F),
        MauEventEmitter(om,this),
        mau_index_(mauIndex), mau_(mau) {
  }
  MauObject::MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       int typeIndex, int rowtabIndex, int colIndex) 
      : PipeObject(om,pipeIndex,mauIndex,typeIndex,rowtabIndex,colIndex),
        MauEventEmitter(om,this),
        mau_index_(mauIndex), mau_(NULL) {
  }
  MauObject::MauObject(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       int typeIndex, int rowtabIndex, int colIndex, Mau *mau) 
      : PipeObject(om,pipeIndex,mauIndex,typeIndex,rowtabIndex,colIndex),
        MauEventEmitter(om,this),
        mau_index_(mauIndex), mau_(mau) {
  }
  MauObject::~MauObject() {
  }  

  Mau *MauObject::mau_lookup() {
    if (mau_ != NULL) return mau_;
    // Fill in mau_
    RmtObjectManager *om = get_object_manager();
    if (om != NULL) mau_ = om->mau_lookup(pipe_index(), mau_index());
    return mau_;
  }

}
