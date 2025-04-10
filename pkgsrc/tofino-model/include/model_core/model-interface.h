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

#ifndef _MODEL_CORE_MODEL_INTERFACE__
#define _MODEL_CORE_MODEL_INTERFACE__

#include <memory>
#include <ostream>
#include <utility>

// We (must) have a concrete `GLOBAL_MODEL` object, but we don't know its concrete compiled type.
// Currently declared in dummy-model.h or model.h,
// and defined in dummy-model.cpp, rmt.cpp, test/main.cpp, or test/main_tofino.cpp (linking picks just one).
namespace model_core { class Model; }
extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

// User must make sure `GLOBAL_MODEL` is defined somewhere e.g. dummy-model.cpp.
// TEST() {
//   auto model = ModelInterface::GetGlobalModel();
//   if (model) model->SetLogDir("MyDirectory");

namespace model_core {

class ModelInterface {
 private:
  virtual void ContinueOnConfigErrorsImpl(bool enabled, std::ostream* o) = 0;
 public:
  // GetGlobalModel() definition is in either dummy-model.cpp or model.cpp (linking picks just one).
  static ModelInterface* GetGlobalModel();

  // We can move stuff into here as needed. Keeping everything virtual give maximum flexibility.
  virtual void SetLogDir(const char *log_dir) = 0;
  virtual const char *GetLogDir() const = 0;

  virtual void SetTrace(unsigned chip, unsigned pipe, bool enable) = 0;
  virtual bool TraceEnabled(unsigned chip, unsigned pipe) const = 0;

  void ContinueOnConfigErrors(bool enabled, std::ostream* o = nullptr) {
    ContinueOnConfigErrorsImpl(enabled, o);
  }
  virtual std::pair<bool, std::ostream*> ContinueOnConfigErrors() const = 0;

 protected:
  virtual ~ModelInterface() = default;  // Can't delete through this interface!
};

}  // namespace model_core

#endif  // _MODEL_CORE_MODEL_INTERFACE__
