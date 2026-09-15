// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifndef _TDI_INFO_TEST_HPP
#define _TDI_INFO_TEST_HPP

#include <stdio.h>
#include <assert.h>
#include <sched.h>
#include <string.h>

#include <memory>
#include <map>
#include <tuple>

/* tdi_includes */
#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_json_parser/tdi_info_parser.hpp>
#include <tdi/common/tdi_json_parser/tdi_cjson.hpp>
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_table.hpp>
#include <tdi/common/tdi_utils.hpp>

/* dummy object includes */
#include <dummy/tdi_dummy_init.hpp>
#include <dummy/tdi_dummy_info.hpp>

namespace tdi {
namespace tdi_test {

using ::testing::WithArgs;

namespace {
std::string target_name = "dummy";

std::string getTestJsonFileContent(const std::string &filename,
                                   const std::string &program_name) {
  std::string temp_file_name = std::string(JSONDIR) + "/" + target_name + "/" +
                               program_name + "/" + filename;
  std::ifstream file(temp_file_name);
  if (file.fail()) {
    LOG_CRIT("Unable to find Json File %s", temp_file_name.c_str());
    return "";
  }
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  return content;
}

template <class T>
void setupInternal(T *obj) {
  auto t = obj->GetParam();
  std::string filename1 = std::get<0>(t);
  std::string program_name = obj->program_name;

  std::string filePath1 = std::string(JSONDIR) + "/" + target_name + "/" +
                          program_name + "/" + filename1;

  obj->tdi_info = nullptr;
  std::vector<std::string> tdi_json_file_path_vect;
  tdi_json_file_path_vect.push_back(filePath1);

  std::vector<P4Pipeline> p4_pipelines;
  ProgramConfig program_config(
      obj->program_name, tdi_json_file_path_vect, p4_pipelines);

  auto tdi_info_mapper =
      std::unique_ptr<tdi::TdiInfoMapper>(new tdi::tna::dummy::TdiInfoMapper());
  auto table_factory =
      std::unique_ptr<tdi::TableFactory>(new tdi::tna::dummy::TableFactory());

  auto tdi_info_parser = std::unique_ptr<TdiInfoParser>(
      new TdiInfoParser(std::move(tdi_info_mapper)));
  auto status =
      tdi_info_parser->parseTdiInfo(program_config.tdi_info_file_paths_);
  ASSERT_EQ(status, TDI_SUCCESS)
      << "Failed to parse file : " << program_config.tdi_info_file_paths_[0];

  obj->tdi_info =
      std::move(tdi::TdiInfo::makeTdiInfo(program_config.prog_name_,
                                          std::move(tdi_info_parser),
                                          table_factory.get()));

  ASSERT_NE(obj->tdi_info, nullptr) << "Failed to open json files";
}
}  // Anonymous namespace

class TdiInfoTest
    : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {
 public:
  TdiInfoTest() : tdi_info(nullptr){};
  virtual void SetUp() {
    auto t = this->GetParam();
    program_name = std::get<1>(t);
    setupInternal<TdiInfoTest>(this);
    auto root = Cjson::createCjsonFromFile(
        getTestJsonFileContent(std::get<0>(t), program_name));
    // Now set up cjson for ground truth
    tdi_root_cjson = std::unique_ptr<Cjson>(new Cjson(root));
  }
  virtual void TearDown() {
    tdi_info.reset(nullptr);
    tdi_root_cjson.reset(nullptr);
  }
  std::unique_ptr<const TdiInfo> tdi_info;
  std::unique_ptr<Cjson> tdi_root_cjson;
  std::string program_name;
};

class TnaExactMatchInfo : public TdiInfoTest {};
class TnaCounterInfo : public TdiInfoTest {};
class TnaPort : public TdiInfoTest {};

INSTANTIATE_TEST_CASE_P(TdiJsonTest,
                        TnaExactMatchInfo,
                        ::testing::Values(std::make_tuple("tdi.json",
                                                          "tna_exact_match")));

INSTANTIATE_TEST_CASE_P(TdiJsonTest,
                        TnaCounterInfo,
                        ::testing::Values(std::make_tuple("tdi.json",
                                                          "tna_counter")));

INSTANTIATE_TEST_CASE_P(TdiJsonTest,
                        TnaPort,
                        ::testing::Values(std::make_tuple("tdi_ports.json",
                                                          "shared")));

}  // namespace tdi_test
}  // namespace tdi

#endif
