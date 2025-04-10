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

#include <utests/test_namespace.h>
#include "gtest.h"
#include "gmock.h"
#include <model_core/caching_interval_map.h>
#include <boost/icl/interval_map.hpp>


namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace boost::icl;
using namespace model_core;

template<typename DomainT, typename CodomainT>
class MockIntervalMap : public interval_map<DomainT, CodomainT> {
 public:
  typedef interval_map<DomainT, CodomainT> base_type;
  typedef typename base_type::const_iterator const_iterator;
  typedef typename base_type::domain_type domain_type;

  MOCK_METHOD1_T(find, const_iterator(
      const domain_type &key_value));

};

template<typename DomainT, typename CodomainT, typename IntervalMapT>
class TestCachingIntervalMap :
    public CachingIntervalMap<DomainT, CodomainT, IntervalMapT> {
  // This exists solely to provide access to a mocked backing interval map so
  // tests can make assertions about calls to the backing interval map.
 public:
  IntervalMapT *get_interval_map() {
    return &this->interval_map_;
  }
};


// workaround interval lacking required comparison operators for ASSERT_EQ
#define ASSERT_INTERVALS_EQUAL(expected, actual) do { \
    ASSERT_TRUE((expected == actual)) \
    << "Actual interval " << (actual) \
    << " does not equal expected interval " << (expected) << std::endl; \
} while(0)

TEST(BFN_TEST_NAME(CachingIntervalMapTest), TestFindResultIsCached) {
  // Verify that cached results are used when matching rather than a call to
  // the wrapped interval map. Use a TestCachingIntervalMap that wraps a
  // MockIntervalMap so that we can check how often the wrapped map's find()
  // method is called.
  TestCachingIntervalMap<int, std::set<std::string>,
                         MockIntervalMap<int, std::set<std::string>>> test_map;
  interval_map<int, std::set<std::string>> ref_map;
  // load up ref map so it has entries:
  //  [0, 5)   -> {"a"}
  //  [5, 8)   -> {"a", "b"}
  //  [8, 10)  -> {"a", "b", "c"}
  //  [10, 15) -> {"b", "c"}
  //  [15, 20) -> {"c"}
  ref_map += make_pair(
      interval<int>::right_open(0, 10), std::set<std::string>{"a"});
  ref_map += make_pair(
      interval<int>::right_open(5, 15), std::set<std::string>{"b"});
  ref_map += make_pair(
      interval<int>::right_open(8, 20), std::set<std::string>{"c"});
  ref_map += make_pair(
      interval<int>::right_open(25, 30), std::set<std::string>{"d"});
  // set expectations of *one* call to the backing interval map find() method
  // for each interval
  EXPECT_CALL(*test_map.get_interval_map(),
              find(0)).Times(1).WillOnce(testing::Return(ref_map.find(0)));
  EXPECT_CALL(*test_map.get_interval_map(),
              find(5)).Times(1).WillOnce(testing::Return(ref_map.find(5)));
  EXPECT_CALL(*test_map.get_interval_map(),
              find(8)).Times(1).WillOnce(testing::Return(ref_map.find(8)));
  EXPECT_CALL(*test_map.get_interval_map(),
              find(10)).Times(1).WillOnce(testing::Return(ref_map.find(10)));
  EXPECT_CALL(*test_map.get_interval_map(),
              find(15)).Times(1).WillOnce(testing::Return(ref_map.find(15)));
  EXPECT_CALL(*test_map.get_interval_map(),
              find(20)).Times(1).WillOnce(testing::Return(test_map.end()));

  // lookup multiple times in each component interval
  for (int i = 0; i < 5; i++) {
    auto actual = test_map.find(i);
    ASSERT_INTERVALS_EQUAL(interval<int>::right_open(0, 5), actual->first);
    std::set<std::string> expected = {"a"};
    ASSERT_EQ(expected, actual->second);
  }
  for (int i = 5; i < 8; i++) {
    auto actual = test_map.find(i);
    ASSERT_INTERVALS_EQUAL(interval<int>::right_open(5, 8), actual->first);
    std::set<std::string> expected = {"a", "b"};
    ASSERT_EQ(expected, actual->second);
  }
  for (int i = 8; i < 10; i++) {
    auto actual = test_map.find(i);
    ASSERT_INTERVALS_EQUAL(interval<int>::right_open(8, 10), actual->first);
    std::set<std::string> expected = {"a", "b", "c"};
    ASSERT_EQ(expected, actual->second);
  }
  for (int i = 10; i < 15; i++) {
    auto actual = test_map.find(i);
    ASSERT_INTERVALS_EQUAL(interval<int>::right_open(10, 15), actual->first);
    std::set<std::string> expected = {"b", "c"};
    ASSERT_EQ(expected, actual->second);
  }
  // first result is still in cache
  for (int i = 0; i < 4; i++) {
    auto actual = test_map.find(i);
    ASSERT_INTERVALS_EQUAL(interval<int>::right_open(0, 5), actual->first);
    std::set<std::string> expected = {"a"};
    ASSERT_EQ(expected, actual->second);
  }
  // this read evicts interval 5-8 from cache
  for (int i = 15; i < 20; i++) {
    auto actual = test_map.find(i);
    ASSERT_INTERVALS_EQUAL(interval<int>::right_open(15, 20), actual->first);
    std::set<std::string> expected = {"c"};
    ASSERT_EQ(expected, actual->second);
  }

  // cache miss, but no value found...
  ASSERT_EQ(test_map.end(), test_map.find(20));
  // ...should not clear the cache
  for (int i = 10; i < 15; i++) {
    auto actual = test_map.find(15);
    ASSERT_INTERVALS_EQUAL(interval<int>::right_open(15, 20), actual->first);
    std::set<std::string> expected = {"c"};
    ASSERT_EQ(expected, actual->second);
  }
}

TEST(BFN_TEST_NAME(CachingIntervalMapTest), TestEvictions) {
  CachingIntervalMap<int, std::set<std::string>> test_map;
  // load up test map so it has entries:
  //  [0, 5)   -> {"a"}
  //  [5, 8)   -> {"a", "b"}
  //  [8, 10)  -> {"a", "b", "c"}
  //  [10, 15) -> {"b", "c"}
  //  [15, 20) -> {"c"}
  test_map += make_pair(
      interval<int>::right_open(0, 10), std::set<std::string>{"a"});
  test_map += make_pair(
      interval<int>::right_open(5, 15), std::set<std::string>{"b"});
  test_map += make_pair(
      interval<int>::right_open(8, 20), std::set<std::string>{"c"});
  test_map += make_pair(
      interval<int>::right_open(25, 30), std::set<std::string>{"d"});

  // lookup multiple times in each component interval
  for (int cycle=0; cycle < 2; cycle++) {
    for (int i = 0; i < 5; i++) {
      auto actual = test_map.find(i);
      ASSERT_INTERVALS_EQUAL(interval<int>::right_open(0, 5), actual->first);
      std::set<std::string> expected = {"a"};
      ASSERT_EQ(expected, actual->second);
    }
    for (int i = 5; i < 8; i++) {
      auto actual = test_map.find(i);
      ASSERT_INTERVALS_EQUAL(interval<int>::right_open(5, 8), actual->first);
      std::set<std::string> expected = {"a", "b"};
      ASSERT_EQ(expected, actual->second);
    }
    for (int i = 8; i < 10; i++) {
      auto actual = test_map.find(i);
      ASSERT_INTERVALS_EQUAL(interval<int>::right_open(8, 10), actual->first);
      std::set<std::string> expected = {"a", "b", "c"};
      ASSERT_EQ(expected, actual->second);
    }
    for (int i = 10; i < 15; i++) {
      auto actual = test_map.find(i);
      ASSERT_INTERVALS_EQUAL(interval<int>::right_open(10, 15), actual->first);
      std::set<std::string> expected = {"b", "c"};
      ASSERT_EQ(expected, actual->second);
    }
    for (int i = 15; i < 20; i++) {
      auto actual = test_map.find(i);
      ASSERT_INTERVALS_EQUAL(interval<int>::right_open(15, 20), actual->first);
      std::set<std::string> expected = {"c"};
      ASSERT_EQ(expected, actual->second);
    }
  }
  test_map.clear();
}

TEST(BFN_TEST_NAME(CachingIntervalMapTest), TestAddEqOperator) {
  // Verify the += operator
  CachingIntervalMap<int, std::set<std::string>> test_map;
  interval<int>::interval_type
      given_interval = interval<int>::right_open(0, 10);
  std::set<std::string> value_set = {"x", "y"};
  test_map += make_pair(given_interval, value_set);

  for (int i = 0; i < 10; i++) {
    auto it = test_map.find(i);
    ASSERT_NE(it, test_map.end()) << "...failed for index " << i;
    ASSERT_INTERVALS_EQUAL(given_interval, it->first);
    ASSERT_EQ(value_set, it->second);
  }
  ASSERT_EQ(test_map.find(10), test_map.end());
}

TEST(BFN_TEST_NAME(CachingIntervalMapTest), TestModificationClearsCache) {
  // verify that the clear() method causes the cache to be cleared
  CachingIntervalMap<int, int> test_map;
  test_map += make_pair(interval<int>::right_open(0, 10), 1);
  // sanity check...
  auto actual = test_map.find(1);
  ASSERT_INTERVALS_EQUAL(interval<int>::right_open(0, 10), actual->first);
  ASSERT_EQ(1, actual->second);
  test_map.clear();
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(test_map.find(i), test_map.end()) << "...failed for index " << i;
  }

  // verify that we get updated values back after map has been updated
  test_map += make_pair(interval<int>::right_open(0, 10), 1);
  // sanity check...
  actual = test_map.find(1);
  ASSERT_INTERVALS_EQUAL(interval<int>::right_open(0, 10), actual->first);
  ASSERT_EQ(1, actual->second);
  test_map += make_pair(interval<int>::right_open(0, 5), 1);
  actual = test_map.find(1);
  ASSERT_INTERVALS_EQUAL(interval<int>::right_open(0, 5), actual->first);
  ASSERT_EQ(2, actual->second);
  actual = test_map.find(5);
  ASSERT_INTERVALS_EQUAL(interval<int>::right_open(5, 10), actual->first);
  ASSERT_EQ(1, actual->second);
}

TEST(BFN_TEST_NAME(CachingIntervalMapTest), TestIteration) {
  // Verify the iteration methods
  TestCachingIntervalMap<int, std::set<std::string>,
                         interval_map<int, std::set<std::string>>> test_map;
  ASSERT_EQ(test_map.get_interval_map()->end(), test_map.end());
  std::set<std::string> value_set_1 = {"x", "y"};
  std::set<std::string> value_set_2 = {"y", "z"};
  test_map += make_pair(interval<int>::right_open(0, 10), value_set_1);
  test_map += make_pair(interval<int>::right_open(5, 15), value_set_2);
  // sanity check...
  ASSERT_EQ(value_set_1, test_map.find(1)->second);
  auto it = test_map.begin();
  ASSERT_INTERVALS_EQUAL(interval<int>::right_open(0, 5), it->first);
  ASSERT_EQ(value_set_1, it->second);
  it++;
  ASSERT_INTERVALS_EQUAL(interval<int>::right_open(5, 10), it->first);
  std::set<std::string> overlap = {"x", "y", "z"};
  ASSERT_EQ(overlap, it->second);
  it++;
  ASSERT_INTERVALS_EQUAL(interval<int>::right_open(10, 15), it->first);
  ASSERT_EQ(value_set_2, it->second);
  it++;
  ASSERT_EQ(test_map.end(), it);
}

} // MODEL_CHIP_TEST_NAMESPACE
