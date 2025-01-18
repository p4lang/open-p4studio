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

#ifndef _MODEL_CORE_P4_OBJECTS_H_
#define _MODEL_CORE_P4_OBJECTS_H_

#include <list>
#include <map>
#include <string>
#include <vector>

namespace model_core {

class P4PhvContainer;
class P4PhvRecord;

/**
 * Encapsulates the parameters of a P4 POV bit.
 */
class P4PovHeader {
 public:
  P4PovHeader(std::string header_name,
              int bit_index);
  ~P4PovHeader() = default;

  void SetRecord(P4PhvRecord *record);
  P4PhvRecord *GetRecord() const { return record_; }
  std::string GetHeaderName() const { return header_name_; }
  int GetBitIndex() const { return bit_index_; }

  std::string ToString();

 private:
  P4PhvRecord *record_;
  std::string header_name_;
  int bit_index_;
};

/**
 * Encapsulates parameters of a P4 PHV container record. Each record specifies
 * a field name and range of bits occupied in the PHV container. If record
 * represents POV bits then it populates a vector of P4PovHeaders
 */
class P4PhvRecord {
 public:
  P4PhvRecord(std::string field_name,
              int field_lsb,
              int field_msb,
              int phv_lsb,
              int phv_msb,
              bool pov);
  ~P4PhvRecord();

  void SetContainer(P4PhvContainer *container);
  P4PhvContainer *GetContainer() const { return container_; }
  bool IsPov() const { return pov_; }
  int GetPhvLsb() const { return phv_lsb_; }
  int GetPhvMsb() const { return phv_msb_; }
  int GetFieldLsb() const { return field_lsb_; }
  int GetFieldMsb() const { return field_msb_; }
  std::string GetFieldName() const { return field_name_; }
  std::vector<P4PovHeader*> GetPovHeaders() const { return pov_headers_; }

  std::string ToString();
  void AddPovHeader(P4PovHeader *pov_header);

 private:
  P4PhvContainer *container_;
  std::string field_name_;
  int field_lsb_;
  int field_msb_;
  int phv_lsb_;
  int phv_msb_;
  bool pov_;
  std::vector<P4PovHeader*> pov_headers_;
};

/**
 * Encapsulates P4 records associated with a single PHV container.
 * Each P4PhvContainer instance has a vector of P4PhvRecords.
 */
class P4PhvContainer {
 public:
  P4PhvContainer(const int phv_number, const bool egress);
  ~P4PhvContainer();

  bool IsEgress() const { return egress_; }
  int GetPhvNumber() const { return phv_number_; }
  std::map<int,P4PovHeader*> GetPovHeaders() const { return pov_headers_; };
  std::vector<P4PhvRecord*> GetRecords() const { return records_; };

  std::string GetFieldName() const;
  void AddPhvRecord(P4PhvRecord *record);

 private:
  int phv_number_;
  bool egress_;
  std::vector<P4PhvRecord*> records_;
  std::map<int,P4PovHeader*> pov_headers_;  // map POV bit_index -> P4PovHeader
  mutable std::string field_name_str_;
};


}  // namespace model_core

#endif //_MODEL_CORE_P4_OBJECTS_H_
