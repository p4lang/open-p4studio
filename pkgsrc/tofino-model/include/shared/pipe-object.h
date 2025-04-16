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

#ifndef _SHARED_PIPE_OBJECT_
#define _SHARED_PIPE_OBJECT_

#include <string>
#include <cstdint>
#include <atomic>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <rmt-log.h>
#include <rmt-event-emitter.h>
#include <model_core/spinlock.h>

namespace MODEL_CHIP_NAMESPACE {

  class Pipe;
  class Mau;

  class PipeObject : public RmtObject, public RmtLogger, public RmtEventEmitter {

 public:
    PipeObject(RmtObjectManager *om, int pipeIndex);
    PipeObject(RmtObjectManager *om, int pipeIndex, Pipe *pipe);
    PipeObject(RmtObjectManager *om, int pipeIndex, int stageIndex, int typeIndex);
    PipeObject(RmtObjectManager *om, int pipeIndex, int stageIndex, int typeIndex, int rowtabIndex, int colIndex);
    virtual ~PipeObject();
    int get_local_port_index(int portIndex);

    virtual int    pipe_index()         const { return pipe_index_;  }
    virtual int    s_index()            const { return stage_index_; }
    virtual int    rt_index()           const { return row_tab_index_; }
    virtual int    c_index()            const { return col_index_; }
    virtual Mau   *mau()                const { return NULL; }

    inline uint8_t version()            const { return version_; }
    inline bool    enabled()            const { return enabled_; }
    inline Pipe   *pipe()               const { return pipe_; }

    inline void    set_version(uint8_t v)     { version_ = v; }
    inline void    set_enabled(bool tf)       { enabled_ = tf; }
    virtual void   set_pipe(Pipe *pipe)       { pipe_ = pipe; }

    inline void spinlock() {
      spinlock_.lock();
    }
    inline void spinunlock() {
      spinlock_.unlock();
    }


 private:
    model_core::Spinlock spinlock_;
    uint8_t              pipe_index_;
    uint8_t              stage_index_;
    uint8_t              row_tab_index_;
    uint8_t              col_index_;
    uint8_t              version_;
    bool                 enabled_;
    Pipe                *pipe_;
  };
}
#endif // _SHARED_PIPE_OBJECT_
