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

#ifndef _MODEL_CORE_CACHING_INTERVAL_MAP_
#define _MODEL_CORE_CACHING_INTERVAL_MAP_

#include <iostream>
#include <list>
#include <thread>
#include <boost/icl/interval_map.hpp>
#include <model_core/spinlock.h>
#include <common/disallow-copy-and-assign.h>

namespace model_core {

using namespace boost::icl;

template
    <
        typename DomainT,
        typename CodomainT,
        // the type of the wrapped interval map is parameterised so that tests
        // can modify it for mocking
        typename IntervalMapT=interval_map<DomainT, CodomainT>
    >
class CachingIntervalMap {
  // Wraps an interval_map instance in order to cache the most recent
  // successful result of a find() so that subsequent lookups for keys in the
  // same interval can be short-circuited.
  //
  // The cache values are held in a list that is also used to maintain an LRU
  // eviction policy. It is assumed that the cache size (and hence list length)
  // is small and that the time to iterate the list for each cache lookup is
  // therefore acceptable.
  //
  // Why wrap rather than subclass?
  // 1. Wrapping provides the opportunity to narrow the interface to the
  // wrapped interval_map to only those methods required for this use case.
  // This in turn minimises the places where the cached result must be
  // invalidated, and reduces the potential for the underlying map to be
  // accidentally modified without the cache being invalidated.
  // 2. Wrapping enables the wrapped interval map type to be parameterised
  // which in turn enables it to be mocked for testing.

 public:
  typedef typename IntervalMapT::const_iterator const_iterator;
  typedef typename IntervalMapT::domain_type domain_type;
  typedef typename IntervalMapT::interval_mapping_type interval_mapping_type;
  typedef typename std::list<typename IntervalMapT::const_iterator> list_type_;

  CachingIntervalMap<DomainT, CodomainT, IntervalMapT>() :
      // max_cache_entries must be greater than zero!
      max_cache_entries_(4) {
    clear_cache();
  }

  CachingIntervalMap<DomainT, CodomainT, IntervalMapT> &operator+=(
      const interval_mapping_type &operand) {
    // clear the cache; this is possibly unnecessary because the cached result
    // holds references to an interval and value that appear to be updated as
    // the map is modified, but do it nevertheless to be sure we never store
    // obsolete cached values.
    clear_cache();
    interval_map_.add(operand);
    return *this;
  }

  void clear() {
    clear_cache();
    interval_map_.clear();
  }

  const_iterator begin() const {
    return interval_map_.begin();
  }

  const_iterator end() const {
    return interval_map_.end();
  }

  const_iterator find(const domain_type &key_value) {
    bool hit = false;
    const_iterator result;
    spinlock_.lock();
    for (typename list_type_::iterator it = cache_.begin();
         it != cache_.end(); ++it) {
      if (contains((*it)->first, key_value)) {
        result = *it;
        hit = true;
        if (result != cache_.front()) {
          // promote this result to head of list
            cache_.splice(cache_.begin(), cache_, it);
        }
        break;
      }
    }
    spinlock_.unlock();
    if (!hit) {
      result = interval_map_.find(key_value);
      if (result != interval_map_.end()) {
        spinlock_.lock();
        if (cache_.size() >= max_cache_entries_) {
          cache_.pop_back();
        }
        cache_.push_front(result);
        spinlock_.unlock();
      }
    }
    return result;
  }

 protected:
  IntervalMapT interval_map_;
  list_type_ cache_ = list_type_();
  Spinlock spinlock_;
  uint max_cache_entries_;

  void clear_cache() {
    spinlock_.lock();
    cache_.erase(cache_.begin(), cache_.end());
    spinlock_.unlock();
  }

 private:
  CachingIntervalMap<DomainT, CodomainT, IntervalMapT>(const CachingIntervalMap<DomainT, CodomainT, IntervalMapT>& other) = delete;
  void operator=(const CachingIntervalMap<DomainT, CodomainT, IntervalMapT>& other) = delete;
};

} // model_core

#endif //_MODEL_CORE_CACHING_INTERVAL_MAP_
