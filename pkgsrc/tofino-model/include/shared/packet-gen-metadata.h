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

#ifndef _SHARED_PACKET_GEN_METADATA_
#define _SHARED_PACKET_GEN_METADATA_
#include <packet-buffer.h>

namespace MODEL_CHIP_NAMESPACE {

class PacketGenMetadata {

 public:
    PacketGenMetadata() {}
    ~PacketGenMetadata() {
      delete trigger_;
    }
    PacketGenMetadata(const PacketGenMetadata& other) = delete; // XXX

    int  address() const       { return address_; }
    bool address_valid() const { return address_valid_; }
    void set_address(int v)    { address_valid_=true; address_=v; }
    void clr_address()         { address_valid_=false; }

    int  length() const        { return length_; }
    bool length_valid() const  { return length_valid_; }
    void set_length(int v)     { length_valid_=true; length_=v; }
    void clr_length()          { length_valid_=false; }

    int   which() const        { return which_; }
    void  which(int w)         { which_ = w; }

    void set_trigger(PacketBuffer *trigger) {trigger_ = trigger;}
    PacketBuffer *get_trigger() {return trigger_;}

    std::string to_string(std::string indent_string = "") const {
      std::string r("");
      r += indent_string;
      if (nullptr == trigger_) r+= "trigger NOT SET\n";
      else r += std::string("trigger = ") + trigger_->to_string() + "\n";

      r += indent_string + std::string("address = ");
      r += (address_valid_ ? boost::str( boost::format("%d") % address_):"invalid") + "\n";
      r += indent_string + std::string("length =  ");
      r += (length_valid_ ? boost::str( boost::format("%d") % length_):"invalid") + "\n";

      return r;
    }

 private:
    PacketGenMetadata& operator=(const PacketGenMetadata&){ return *this; } // XXX
    int  address_= 0;
    bool address_valid_ = false;
    int  length_ = 0;
    bool length_valid_ = false;
    int  which_ = 0;
    PacketBuffer *trigger_ = nullptr;

  };
}
#endif // _SHARED_PACKET_GEN_METADATA_
