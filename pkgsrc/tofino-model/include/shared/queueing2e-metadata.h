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

#ifndef _SHARED_QUEUEING2E_METADATA_
#define _SHARED_QUEUEING2E_METADATA_

#include <rmt-defs.h>

namespace MODEL_CHIP_NAMESPACE {

  // Metadata passed from ingress deparser to queueing block.
  class Queueing2EMetadata {

  public:
    // Getter functions.
    inline uint8_t  version()         const { return version_; }
    inline uint32_t ing_q_depth()     const { return ing_q_depth_; }
    inline uint8_t  ing_q_status()    const { return ing_q_status_; }
    inline bool     ing_congested()   const { return (ing_q_status() & 0x1) != 0; }
    inline uint32_t ing_q_ts()        const { return ing_q_ts_; }
    inline uint32_t egr_q_depth()     const { return egr_q_depth_; }
    inline uint8_t  egr_q_status()    const { return egr_q_status_; }
    inline bool     egr_congested()   const { return (egr_q_status() & 0x1) != 0; }
    inline uint8_t  app_pool_status() const { return app_pool_status_; }
    inline uint32_t delay()           const { return delay_; }
    inline uint16_t erid()            const { return erid_; }
    inline uint8_t  rid_first()       const { return rid_first_; }
    inline uint16_t egress_port()     const { return egress_port_; }
    inline uint8_t  ecos()            const { return ecos_; }
    inline uint32_t eqid()            const { return eqid_; }
    inline uint8_t  redc()            const { return redc_; }
    inline uint32_t len()             const { return queued_sz_; }
    inline uint8_t  mir_copy()        const { return mir_copy_; }

    // Setter functions.
    inline void     set_version(uint64_t x)         { version_ = x & 0x3; }
    inline void     set_ing_q_depth(uint64_t x)     {
      ing_q_depth_ = x & RmtDefs::kTmQueueOccupancyMask;
    }
    inline void     set_ing_q_status(uint64_t x)    { ing_q_status_ = x & 0x3; }
    inline void     set_ing_congested(bool tf = true) {
      set_ing_q_status(tf ?(ing_q_status() |0x1) :(ing_q_status() & ~0x1));
    }
    inline void     set_ing_q_ts(uint64_t x)        { ing_q_ts_ = x & 0xFFFFFFFF; }
    inline void     set_egr_q_depth(uint64_t x)     {
      egr_q_depth_ = x & RmtDefs::kTmQueueOccupancyMask;
    }
    inline void     set_egr_q_status(uint64_t x)    { egr_q_status_ = x & 0x3; }
    inline void     set_egr_congested(bool tf = true) {
      set_egr_q_status(tf ?(egr_q_status() |0x1) :(egr_q_status() & ~0x1));
    }
    inline void     set_app_pool_status(uint64_t x) { app_pool_status_ = x & 0xFF; }
    inline void     set_delay(uint64_t x)           { delay_ = x & 0xFFFFFFFF; }
    inline void     set_erid(uint64_t x)            { erid_ = x & 0xFFFF;}
    inline void     set_rid_first(uint64_t x)       { rid_first_ = x & 0x1;}
    inline void     set_egress_port(uint64_t x)     { egress_port_ = x & ((1 << RmtDefs::kPortWidth) - 1); }
    inline void     set_ecos(uint64_t x)            { ecos_ = x & 0x7; }
    inline void     set_eqid(uint64_t x)            { eqid_ = (x & RmtDefs::kI2qQidMask); }
    inline void     set_redc(uint64_t x)            { redc_ = x & 0x1; }
    inline void     set_len(uint64_t x)             { queued_sz_ = x & 0x3FFF; }
    inline void     set_mir_copy(uint64_t x)        { mir_copy_ = x & 0x1; }

    // Reset function.
    inline void     reset() {
                      set_version(0);
                      set_ing_q_depth(0);
                      set_ing_q_status(0);
                      set_ing_q_ts(0);
                      set_egr_q_depth(0);
                      set_egr_q_status(0);
                      set_app_pool_status(0);
                      set_delay(0);
                      set_erid(0);
                      set_rid_first(0);
                      set_egress_port(0);
                      set_ecos(0);
                      set_eqid(0);
                      set_redc(0);
                      set_len(0);
                      set_mir_copy(0); }

    // Assign from.
    inline void     copy_from(const Queueing2EMetadata s) {
                      version_         = s.version_;
                      ing_q_depth_     = s.ing_q_depth_;
                      ing_q_status_    = s.ing_q_status_;
                      ing_q_ts_        = s.ing_q_ts_;
                      egr_q_depth_     = s.egr_q_depth_;
                      egr_q_status_    = s.egr_q_status_;
                      app_pool_status_ = s.app_pool_status_;
                      delay_           = s.delay_;
                      erid_            = s.erid_;
                      rid_first_       = s.rid_first_;
                      egress_port_     = s.egress_port_;
                      ecos_            = s.ecos_;
                      eqid_            = s.eqid_;
                      redc_            = s.redc_;
                      queued_sz_       = s.queued_sz_;
                      mir_copy_        = s.mir_copy_; }

  private:
    uint8_t     version_         = 0;
    uint32_t    ing_q_depth_     = 0;
    uint8_t     ing_q_status_    = 0;
    uint32_t    ing_q_ts_        = 0;
    uint32_t    egr_q_depth_     = 0;
    uint8_t     egr_q_status_    = 0;
    uint8_t     app_pool_status_ = 0;
    uint32_t    delay_           = 0;
    uint16_t    erid_            = 0;
    uint8_t     rid_first_       = 0;
    uint16_t    egress_port_     = 0;
    uint8_t     ecos_            = 0;
    uint8_t     eqid_            = 0;
    uint8_t     redc_            = 0;   // redirect (deflect)
    uint32_t    queued_sz_       = 0;
    uint8_t     mir_copy_        = 0;
    // N.B. Below variables exist in the FTR HAS however are not modelled, as they are
    //      only used by the QAC/PEX blocks (for PRC counter management/deQ process).
    // uint8_t     first_copy_      = 0;
    // uint8_t     last_mc_         = 0;
    // uint8_t     last_mir_        = 0;
  };
}
#endif // _SHARED_QUEUEING2E_METADATA_
