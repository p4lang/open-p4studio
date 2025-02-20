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

#ifndef _MODEL_CORE_JSON_LOADER_H
#define _MODEL_CORE_JSON_LOADER_H

#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include <memory>
#include <sstream>
#include <string>
#include <functional>

namespace model_core {

class JSONLoader {
 public:
  /**
   * Loads a file as a rapidjson Document.
   * @param filename Name of file to load.
   * @param description Description of file, used in log messages.
   * @param max_characters The maximum number of characters that
   *        will be read from the file; if this is exceeded a runtime_error
   *        will be thrown during parsing. A value less than zero will result
   *        in no limit being applied.
   * @param doc_checker_fn (optional) A function that will be called during
   *        construction to check the loaded document.
   */
  JSONLoader(const std::string& filename,
             const std::string& description,
             int max_characters,
             std::function<bool(std::unique_ptr<rapidjson::Document>&)> doc_checker_fn = nullptr);

  virtual ~JSONLoader() {};

  /**
   * Test of a valid json file has been successfully loaded.
   * @return Boolean; true if json file has been loaded, false otherwise.
   */
  bool IsLoaded() const;

 protected:
  // Note: rapidjson::Document is type GenericDocument<UTF8<>> i.e. UTF8 encoding
  std::unique_ptr<rapidjson::Document> document_;
  bool loaded_ = false;

 private:
  static constexpr int kReadBufferSize = 65535;
  char readBuffer_[kReadBufferSize];
};

const rapidjson::Value& FindMember(const rapidjson::Value& obj,
                                   const char *key);

const rapidjson::Value& FindMemberArray(const rapidjson::Value& obj,
                                        const char *key);

}

#endif //_MODEL_CORE_JSON_LOADER_H
