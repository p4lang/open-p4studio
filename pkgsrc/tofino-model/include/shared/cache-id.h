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

#ifndef _SHARED_CACHE_ID__
#define _SHARED_CACHE_ID__

#include <cinttypes>
#include <common/rmt-util.h>

/** Used for caching results within the lookup
 *
 */
class CacheId {
public:
  CacheId() : cache_id_(0) {}
  ~CacheId() {}

  void SetNewId() { cache_id_++; }
  // Maybe might need to set the ID to something in the future?
  //SetNewId(uint32_t v) { cache_id_ = v; }
  bool IsValid() const { return (cache_id_ != 0); }
  bool Equals(const CacheId& other) const { return cache_id_ == other.cache_id_; }
  void SetFrom(const CacheId& other) { cache_id_ = other.cache_id_; }
  void Invalidate() { cache_id_ = 0; }

private:
  uint32_t cache_id_;

  DISALLOW_COPY_AND_ASSIGN(CacheId);
};

#endif
