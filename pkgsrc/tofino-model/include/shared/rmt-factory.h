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

#ifndef _RMT_FACTORY_H_
#define _RMT_FACTORY_H_

namespace MODEL_CHIP_NAMESPACE {

/**
 * Base factory class for managing a DIMENSION1 sized array of instances of
 * type T. Instances of T at each index in the array will be constructed on
 * demand.
 *
 * @tparam T type of objects to be managed by the factory
 * @tparam DIMENSION1 size of the array of instances
 */
template <typename T, int DIMENSION1>
class RmtBaseFactory {
 public:
  /**
   * Constructs a factory
   * @param om pointer to a RmtObjectManager
   */
  explicit RmtBaseFactory(RmtObjectManager *om) : om_(om) { }

  virtual ~RmtBaseFactory() {
    free_all();
  };

  virtual bool is_index_valid(int index) const {
    return (index >= 0) && (index < DIMENSION1);
  }

  T *lookup(int index) const {
    if (!is_index_valid(index)) return nullptr;
    return instances_[index];
  }

  virtual void set(int index, T *instance) {
    if (!is_index_valid(index)) return;
    instances_[index] = instance;
  }

  // subclasses must override this method to return new instances of T
  virtual T *create(RmtObjectManager *om, int index) = 0;

  T *get(int index) {
    if (!is_index_valid(index)) return nullptr;
    T *instance = lookup(index);
    if (nullptr == instance) {
      instance = create(om_, index);
      if (nullptr != instance) {
        set(index, instance);
      }
    }
    return instance;
  }

  void free(int index) {
    T *instance = lookup(index);
    if (nullptr != instance) {
      delete instance;
      set(index, nullptr);
    }
  }

  void free_all() {
    for (int index=0; index<DIMENSION1; index++) {
      free(index);
    }
  }
  void reset() {
    for (T *instance : instances_) {
      if(nullptr != instance) instance->reset();
    }
  }

 protected:
  RmtObjectManager *om_;
  std::array<T*,DIMENSION1> instances_ = { };
};


/**
 * Factory class for managing a singleton instance of type T.
 * @tparam T type of object to be managed by the factory
 */
template <typename T>
class RmtSingletonFactory : public RmtBaseFactory<T, 1> {
  // for convenience this factory uses a factory with array size 1 to hold the
  // singleton
 public:
  explicit RmtSingletonFactory(RmtObjectManager *om) :
    RmtBaseFactory<T, 1>(om) { }

  ~RmtSingletonFactory() { }

  T* create(RmtObjectManager *om, int index) {
    return new T(om);
  }

  T *get() { return RmtBaseFactory<T, 1>::get(0); }
};

}

#endif //_RMT_FACTORY_H_
