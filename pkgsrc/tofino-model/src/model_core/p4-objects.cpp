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

#include <sstream>
#include <common/rmt-assert.h>
#include <model_core/p4-objects.h>

namespace model_core {

// ********************** P4PovHeader *****************************************

P4PovHeader::P4PovHeader(std::string header_name,
                         int bit_index) :
    record_(nullptr),
    header_name_(std::move(header_name)),
    bit_index_(bit_index) {
}

void P4PovHeader::SetRecord(P4PhvRecord *record) {
  RMT_ASSERT(nullptr != record);
  record_ = record;
}

std::string P4PovHeader::ToString() {
  return GetHeaderName() + " " + std::to_string(GetBitIndex());
}


// ********************** P4PhvRecord *****************************************

P4PhvRecord::P4PhvRecord(std::string field_name,
                         int field_lsb,
                         int field_msb,
                         int phv_lsb,
                         int phv_msb,
                         bool pov) :
    container_(nullptr),
    field_name_(std::move(field_name)),
    field_lsb_(field_lsb),
    field_msb_(field_msb),
    phv_lsb_(phv_lsb),
    phv_msb_(phv_msb),
    pov_(pov) {}

P4PhvRecord::~P4PhvRecord() {
  for (auto& pov_header : pov_headers_) delete pov_header;
}

void P4PhvRecord::SetContainer(P4PhvContainer *container) {
  RMT_ASSERT(nullptr != container);
  container_ = container;
}

void P4PhvRecord::AddPovHeader(P4PovHeader *pov_header) {
  pov_header->SetRecord(this);
  pov_headers_.push_back(pov_header);
}

std::string P4PhvRecord::ToString() {
  std::stringstream ss;
  ss << field_name_ << "[" << field_lsb_ << ":" << field_msb_ << "]"
     << std::endl;
  for (auto pov_header : pov_headers_) {
    ss << "  " << pov_header->ToString() << std::endl;
  }
  return ss.str();
}


// ********************** P4PhvContainer **************************************

P4PhvContainer::P4PhvContainer(const int phv_number,
                               const bool egress) :
    phv_number_(phv_number),
    egress_(egress) {}

P4PhvContainer::~P4PhvContainer() {
  for (auto& record : records_) delete record;
}

void P4PhvContainer::AddPhvRecord(P4PhvRecord *record) {
  record->SetContainer(this);
  records_.push_back(record);
  if (record->IsPov()) {
    for (auto *pov_header : record->GetPovHeaders()) {
      // add to map of pov_headers so that they can be iterated over in
      // bit_index order
      pov_headers_.emplace(pov_header->GetBitIndex(), pov_header);
    }
  }
}

std::string P4PhvContainer::GetFieldName() const {
  if (!field_name_str_.empty()) return field_name_str_;

  // compose field names string...
  std::stringstream field_name;
  if (IsEgress()) field_name << "E [";
  else field_name << "I [";
  int records_in_field = 0;
  bool has_pov = false;
  for (auto *record : records_) {
    // iterate over records accumulating field names
    if (record->IsPov()) {
      // do not append POV field name - we'll add it once at end of loop
      has_pov = true;
      continue;
    }
    if (records_in_field) field_name << ", ";
    field_name << record->GetFieldName() << "[" << record->GetFieldMsb()
               << ":" << record->GetFieldLsb() << "]";
    records_in_field++;
  }
  if (has_pov) {
    if (records_in_field) field_name << ", ";
    field_name << "POV";  // include this once for all POV records
  }
  field_name << "]";
  field_name_str_ = field_name.str();
  return field_name_str_;
}

}
