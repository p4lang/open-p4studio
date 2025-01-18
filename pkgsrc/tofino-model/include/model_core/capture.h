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

#ifndef MODEL_INCLUDE_MODEL_CORE_CAPTURE_
#define MODEL_INCLUDE_MODEL_CORE_CAPTURE_

#include <iostream>
#include <fstream>
#include <vector>
#include <common/rmt-util.h>
#include <mutex>

namespace model_core {

class Capture {

 public:

  static constexpr int  kMaxLenLogDirStr          = 256+1;
  static constexpr char kDefaultLogDirStr[]       = ".";
  static constexpr char kPathSeparatorChar        = '/';

  Capture();
  Capture(std::string log_dir);
  ~Capture();

  void              log_outword       (uint32_t address, uint32_t data);
  void              log_indirect_write(uint64_t address,
                                       uint64_t data0,
                                       uint64_t data1);
  void              log_packet        (uint16_t asic_port, uint8_t *buf, int len);

 private:
  std::string       log_dir_;
  std::mutex        mutex_;
  int               file_num_              =  0;

  std::ofstream     data_file_;
  std::ofstream     command_file_;
  std::string       current_file_name_     = "file1.bin";

  std::string       get_data_filename();
  void              log_new_file(std::string filename);
  void              generate_new_data_file();
  void              initialize_command_file();
  void              open_data_file(std::string filename);
  void              write_packet_data(int port, char * data);
  void              set_dir(std::string dir);
};

}
#endif //MODEL_INCLUDE_MODEL_CORE_CAPTURE_
