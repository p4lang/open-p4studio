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

#include <model_core/capture.h>
#include <model_core/model.h>
#include <model_core/log-buffer.h>

namespace model_core {

constexpr char Capture::kDefaultLogDirStr[];
constexpr char Capture::kPathSeparatorChar;

Capture::Capture() : Capture(kDefaultLogDirStr) {}

Capture::Capture(std::string log_dir) : mutex_() {
  /**
   * Insert header metadata into the data file.
   * This will probably be passed as parameters in the constructor.
   */
  set_dir(log_dir);
  initialize_command_file();
}

Capture::~Capture() {
  data_file_.flush();
  command_file_.flush();
  data_file_.close();
  command_file_.close();
}

void Capture::log_indirect_write(uint64_t address,
                                 uint64_t data0,
                                 uint64_t data1) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (!data_file_.is_open()) generate_new_data_file();

  /** We perform a left shift on 'd' in order to 
   *  detect what type of data is being passed through and 
   *  fit the format of the simple_test_harness. 
   *  As per the code in tests/simple_test_harness/input.cpp:189.
   *  The range is calculated because the data is a range of 128-bit memory
   *  and the size of the range is specified as count * width (in bits), 
   *  which must always be a multiple of 64. 
   *  Since we are writing 2 64-bit uint64_t values, our range will
   *  always be 2 * 64. Note that we follow big endian when writing to file.
   */
  uint32_t v = static_cast<uint32_t>('d') << 24;
  uint64_t range = (UINT64_C(2) << 32) | UINT64_C(64);
  data_file_.write(reinterpret_cast<char *>(&v), sizeof(v));
  data_file_.write(reinterpret_cast<char *>(&address), sizeof(address));
  data_file_.write(reinterpret_cast<char *>(&range), sizeof(range));
  data_file_.write(reinterpret_cast<char *>(&data0), sizeof(data0));
  data_file_.write(reinterpret_cast<char *>(&data1), sizeof(data1));
}

void Capture::log_outword(uint32_t address, uint32_t data) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (!data_file_.is_open()) generate_new_data_file();

  /** We perform a left shift on 'r' in order to 
   *  detect what type of data is being passed through and 
   *  fit the format of the simple_test_harness.
   *  As per the code in tests/simple_test_harness/input.cpp:181.
   */
  uint32_t v = static_cast<uint32_t>('r') << 24;
  data_file_.write(reinterpret_cast<char *>(&v), sizeof(v));
  data_file_.write(reinterpret_cast<char *>(&address), sizeof(address));
  data_file_.write(reinterpret_cast<char *>(&data), sizeof(data));
}

void Capture::log_packet(uint16_t asic_port, uint8_t *buf, int len) {
  std::unique_lock<std::mutex> lock(mutex_);
  LogBuffer log_buf(len*2);
  /**
   * Len is decreased by 4 since it is increased by 4 for a CRC
   * in simple_test_harness/packet.cpp:227
   */
  len -= 4;
  for (int i = 0; i < len; i++) {
    log_buf.Append("%02x", buf[i]);
  }

  char *hex_data = log_buf.GetBuf();

  write_packet_data(asic_port, hex_data);
  if (data_file_.is_open()) {
    data_file_.flush();
    data_file_.close();
  }
}

void Capture::set_dir(std::string dir) {
  std::unique_lock<std::mutex> lock(mutex_);
  RMT_ASSERT(!command_file_.is_open() && "command_file_ is open");
  RMT_ASSERT(!data_file_.is_open() && "data_file_ is open");
  log_dir_ = dir;
  if (dir.back() != kPathSeparatorChar)
     log_dir_.push_back(kPathSeparatorChar);
}

std::string Capture::get_data_filename() {
 return "file" + std::to_string(file_num_) + ".bin";
}

void Capture::open_data_file(std::string filename) {
  data_file_.open(log_dir_ + filename, std::ios::binary | std::ios::out);
  current_file_name_ = filename;
}

void Capture::log_new_file(std::string filename) {
  command_file_ << "load " + filename << std::endl;
}

void Capture::initialize_command_file() {
  command_file_.open(log_dir_ + "commands.stf");
  command_file_ << "help" << std::endl;
}

void Capture::write_packet_data(int port, char * data) {
  command_file_ << "packet " << port << " " << data << std::endl;
}

void Capture::generate_new_data_file() {
  file_num_++;
  std::string filename = get_data_filename();
  open_data_file(filename);
  log_new_file(filename);
}

}
