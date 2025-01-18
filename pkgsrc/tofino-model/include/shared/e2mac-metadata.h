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

#ifndef _SHARED_E2MAC_METADATA_
#define _SHARED_E2MAC_METADATA_
#include <boost/optional.hpp>
#include <afc-metadata.h>

namespace MODEL_CHIP_NAMESPACE {

  class E2MacMetadata {

 public:
    // Getter functions.
    inline bool     capture_tx_ts()          const { return capture_tx_ts_; }
    inline uint8_t  ecos()                   const { return ecos_; }
    inline bool     force_tx_error()         const { return force_tx_error_; }
    inline bool     update_delay_on_tx()     const { return update_delay_on_tx_; }

    inline bool     is_egress_uc()   const { return egress_unicast_valid_; }
    inline uint16_t egress_unicast_port() const { return is_egress_uc() ? egress_unicast_port_ : 0; }

    // new for JBay
    inline boost::optional<AFCMetadata> afc()        const { return afc_; }
    inline boost::optional<int>    mtu_trunc_len()   const { return mtu_trunc_len_; }
    inline int      mtu_trunc_err_f() const { return mtu_trunc_err_f_; }


    // Setter functions.
    inline void     set_capture_tx_ts(uint64_t x)       { capture_tx_ts_ = (bool)(x & 0x01); }
    inline void     set_egress_unicast_port(uint64_t x) {
      egress_unicast_valid_ = true;
      egress_unicast_port_ = (x & ((1 << RmtDefs::kDeparserEgressUnicastPortWidth) - 1));
    }

    inline void     set_ecos(uint64_t x)                { ecos_ = (x & 0x07); }
    inline void     set_force_tx_error(uint64_t x)      { force_tx_error_ = (bool)(x & 0x01); }
    inline void     set_update_delay_on_tx(uint64_t x)  { update_delay_on_tx_ = (bool)(x & 0x01); }

    inline void     clr_egress_unicast_port() { egress_unicast_valid_ = false; }

    // new for JBay
    inline void     set_afc(AFCMetadata x)     { afc_ = x; }
    inline void     set_mtu_trunc_len(int x)   { mtu_trunc_len_ = x; }
    inline void     set_mtu_trunc_err_f(int x) { mtu_trunc_err_f_ = x; }

    void reset() {
      capture_tx_ts_ = false;
      ecos_ = 0;
      egress_unicast_port_ = 0;
      egress_unicast_valid_ = false;
      force_tx_error_ = false;
      update_delay_on_tx_ = false;
      afc_.reset();
      mtu_trunc_len_ = boost::none;
      mtu_trunc_err_f_ = 0;
    }

 private:
    bool                       capture_tx_ts_ = false;
    uint8_t                    ecos_ = 0;
    uint16_t                   egress_unicast_port_ = 0;
    bool                       egress_unicast_valid_ = false;
    bool                       force_tx_error_ = false;
    bool                       update_delay_on_tx_ = false;

    // new for JBay
    boost::optional<AFCMetadata> afc_;
    boost::optional<int> mtu_trunc_len_{};
    int            mtu_trunc_err_f_ = 0;

  };
}
#endif // _SHARED_E2MAC_METADATA_
