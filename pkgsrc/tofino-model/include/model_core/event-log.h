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

#ifndef _MODEL_CORE_EVENT_LOG_
#define _MODEL_CORE_EVENT_LOG_

#include <model_core/event.h>
#include <cinttypes>
#include <memory>
#include <mutex>
#include <string>

namespace model_core {

/* EventLog template accepts
 *  + An event stream S which implements the rapidjson writeable stream concept
 *  + An event writer W which implements the rapidjson writer concept
 */
template<typename S, typename W>
class EventLog {
 public:
  // SchemaVer
  static const int kSchemaVersionMod = 1; // Model
  static const int kSchemaVersionRev = 0; // Revision
  static const int kSchemaVersionAdd = 0; // Addition

  /* ctor */
  explicit EventLog(const std::string& filename) :
      fp_(fopen(filename.c_str(), "w")),
      es_(new S(fp_, writeBuffer_, sizeof(writeBuffer_))),
      writer_(new W(*es_)) {
    // throw if we couldn't open the log file
    if (fp_ == nullptr) {
      throw std::runtime_error(std::string("Could not open log for writing (") + filename + std::string(")"));
    }
    // write the log header
    writer_->StartObject();
    writer_->Key("schema_version");
    const auto version_str = std::to_string(kSchemaVersionMod) + "-"
        + std::to_string(kSchemaVersionRev) + "-"
        + std::to_string(kSchemaVersionAdd);
    writer_->String(version_str.c_str());
    writer_->EndObject();
    es_->Put('\n');
    writer_->Reset(*es_);
  }

  /* dtor */
  ~EventLog() {
    es_->Flush();
    fclose(fp_);
  }

  /* Add event to this log */
  void Add(const Event<W>& event) {
    event.Serialize(*writer_);
    if (writer_->IsComplete()) {
      es_->Put('\n');
      writer_->Reset(*es_);
    }
  }

  /* Flush the stream */
  void Flush() {
    es_->Flush();
  }

 private:
  /* Prohibit copy and assignment construction */
  EventLog(const EventLog&) = delete;
  EventLog& operator=(const EventLog&) = delete;

  static constexpr int kWriteBufferSize = 65535;
  FILE* fp_;
  char writeBuffer_[kWriteBufferSize];
  std::unique_ptr<S> es_;
  std::unique_ptr<W> writer_;
  std::mutex writeMutex_;
};

}  // model_core
#endif // _MODEL_CORE_EVENT_LOG_
