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

#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <model_core/json-loader.h>

namespace model_core {

class CappedFileReadStream {
// wraps a FileReadStream rather than
 public:
  /**
   * Wraps a rapidjson::FileReadStream instance and imposes a cap on the number
   * of characters that can be read.
   * @param fstream An instance of a rapidjson::FileReadStream
   * @param max_characters The maximum number of characters that
   *        will be read from the file; if this is exceeded a runtime_error
   *        will be thrown during parsing. A value less than zero will result
   *        in no limit being applied.
   */
  explicit CappedFileReadStream(rapidjson::FileReadStream& fstream,
                                int max_characters) :
      fstream_(fstream),
      max_characters_(max_characters) {};
  typedef char Ch;

  // intercept calls to this method in order to check the character count
  Ch Take() {
    Ch ch = fstream_.Take();
    character_count_++;
    if ((max_characters_ >= 0) && (character_count_ > max_characters_)) {
      throw std::runtime_error("Maximum character count exceeded");
    }
    return ch;
  }

  // the remaining methods are forwarded to the FileReadStram instance...
  Ch Peek() const { return fstream_.Peek(); }
  size_t Tell() const { return fstream_.Tell(); }
  void Put(Ch ch) { return fstream_.Put(ch); }
  void Flush() { return fstream_.Flush(); }
  Ch* PutBegin() { return fstream_.PutBegin(); }
  size_t PutEnd(Ch* ch) { return fstream_.PutEnd(ch); }
  const Ch* Peek4() const { return fstream_.Peek4(); }

 private:
  rapidjson::FileReadStream fstream_;
  int max_characters_;
  int character_count_{0};
};


JSONLoader::JSONLoader(const std::string& filename,
                       const std::string& description,
                       int max_characters,
                       std::function<bool(std::unique_ptr<rapidjson::Document>&)> doc_checker_fn) {
  FILE* fp = nullptr;
  loaded_ = false;
  if (!filename.empty()) {
    std::cout << "Opening " << description << " '" << filename << "' ..." << std::endl;
    fp = fopen(filename.c_str(), "r");
    if (!fp) {
      std::cerr << "ERROR: Failed to open " << description << std::endl;
    } else {
      rapidjson::FileReadStream read_stream(fp, readBuffer_, sizeof(readBuffer_));
      document_ = std::unique_ptr<rapidjson::Document>(new rapidjson::Document());
      CappedFileReadStream capped_read_stream(read_stream, max_characters);
      try {
        // XXX: use kParseValidateEncodingFlag to switch on UTF8
        // validation when parsing. Note that the document_ type
        // rapidjson::Document uses UTF8 encoding.
        document_->ParseStream<rapidjson::kParseValidateEncodingFlag
                               | rapidjson::kParseDefaultFlags>(capped_read_stream);
        loaded_ = true;
      } catch (const std::runtime_error& e) {
        std::cerr << "ERROR: Failed parsing " << description << ": "
                  << e.what() << std::endl;
      }
      fclose(fp);
      if (loaded_ && document_->HasParseError()) {
        std::cerr << "ERROR: Failed parsing " << description
                  << " at offset " << document_->GetErrorOffset() << ": "
                  << GetParseError_En(document_->GetParseError()) << std::endl;
        loaded_ = false;
      }
    }
    if (loaded_ && (nullptr != doc_checker_fn)) {
      loaded_ = loaded_ && doc_checker_fn(document_);
    }
    if (loaded_) std::cout << "Loaded " << description << std::endl;
  }
}


/**
 * Test of a valid json file has been successfully loaded.
 * @return Boolean; true if json file has been loaded, false otherwise.
 */
bool JSONLoader::IsLoaded() const {
  return loaded_;
}

const rapidjson::Value& FindMember(const rapidjson::Value& obj,
                                   const char *key) {
  if (!obj.IsObject()) {
    std::stringstream err;
    err << "Tried to find member in non-object of type " << obj.GetType();
    throw std::invalid_argument(err.str());
  }
  rapidjson::Value::ConstMemberIterator itr = obj.FindMember(key);
  if (itr == obj.MemberEnd()) {
    std::stringstream err;
    err << "Could not find key (" << key << ") in JSON object.";
    throw std::invalid_argument(err.str());
  }
  const rapidjson::Value& val = itr->value;
  return val;
}

const rapidjson::Value& FindMemberArray(const rapidjson::Value& obj,
                                        const char *key) {
  auto& member = FindMember(obj, key);
  if (!member.IsArray()) {
    std::stringstream err;
    err << "Expected JSON object member (" << key << ") to be an array.";
    throw std::invalid_argument(err.str());
  }
  return member;
}

}
