// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>   // std::ifstream
#include <iterator>  // std::distance
#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>
#include <cstring>  // std::memcmp

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_json_parser/tdi_info_parser.hpp>
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_table.hpp>

#include "tdi_info_test.hpp"

// using ::testing::WithParamInterface;
namespace tdi {
namespace tdi_test {

/**
 * @brief Test TdiInfo->tablesGet() and check
 * If number of items are expected in the vector
 */
TEST_P(TnaExactMatchInfo, TestTablesGet) {
  // Check tablesGet()
  std::vector<const tdi::Table *> table_vec;
  auto status = tdi_info->tablesGet(&table_vec);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table_vec.size(), 3);
}

TEST_P(TnaCounterInfo, TestTablesGet) {
  // Check tablesGet()
  std::vector<const tdi::Table *> table_vec;
  auto status = tdi_info->tablesGet(&table_vec);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table_vec.size(), 4);
}

TEST_P(TnaPort, TestTablesGet) {
  // Check tablesGet()
  std::vector<const tdi::Table *> table_vec;
  auto status = tdi_info->tablesGet(&table_vec);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table_vec.size(), 2);
}

/**
 * @brief Test TdiInfo->tableFromIdGet().
 * Correct Table object should be returned
 * with the ID
 */
TEST_P(TnaExactMatchInfo, tableFromIdGet) {
  const tdi::Table *table;
  auto status = tdi_info->tableFromIdGet(37882547, &table);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table->tableInfoGet()->nameGet(), "pipe.SwitchIngress.forward");
}

/**
 * @brief Test TdiInfo->tableFromNameGet().
 * Test whether the correct table is returned when
 * queried with name or a shortened name. Shortened names
 * are valid if there are no clashes
 */
TEST_P(TnaExactMatchInfo, tableFromNameGet) {
  const tdi::Table *table;
  auto status =
      tdi_info->tableFromNameGet("pipe.SwitchIngress.forward", &table);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table->tableInfoGet()->nameGet(), "pipe.SwitchIngress.forward");

  status = tdi_info->tableFromNameGet("SwitchIngress.forward", &table);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table->tableInfoGet()->nameGet(), "pipe.SwitchIngress.forward");

  status = tdi_info->tableFromNameGet("forward", &table);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table->tableInfoGet()->nameGet(), "pipe.SwitchIngress.forward");
}

/**
 * @brief Test TableInfo->idGet()
 */
TEST_P(TnaExactMatchInfo, tableInfo_idGet) {
  const tdi::Table *table;
  auto status =
      tdi_info->tableFromNameGet("pipe.SwitchIngress.forward", &table);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table->tableInfoGet()->idGet(), 37882547);
}

/**
 * @brief Test TableInfo->sizeGet()
 */
TEST_P(TnaExactMatchInfo, tableInfo_sizeGet) {
  const tdi::Table *table;
  auto status =
      tdi_info->tableFromNameGet("pipe.SwitchIngress.forward", &table);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table->tableInfoGet()->sizeGet(), 1024);
}

/**
 * @brief Test TableInfo->tableTypeGet()
 */
TEST_P(TnaExactMatchInfo, tableInfo_tableTypeGet) {
  const tdi::Table *table;
  auto status =
      tdi_info->tableFromNameGet("pipe.SwitchIngress.forward", &table);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table->tableInfoGet()->tableTypeGet(),
            TDI_DUMMY_TABLE_TYPE_MATCH_DIRECT);
}

TEST_P(TnaCounterInfo, tableInfo_tableTypeGet) {
  const tdi::Table *table;
  auto status = tdi_info->tableFromNameGet("indirect_counter", &table);
  ASSERT_EQ(status, TDI_SUCCESS);
  ASSERT_EQ(table->tableInfoGet()->tableTypeGet(),
            TDI_DUMMY_TABLE_TYPE_COUNTER);
}

}  // namespace tdi_test
}  // namespace tdi
