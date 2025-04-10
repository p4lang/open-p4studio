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

#include <utests/test_util.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fstream>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <experimental/filesystem>
#include <rmt-packet-coordinator.h>
#include <model_core/capture.h>
#include "filesystem_helper.h"
#include "gtest.h"
#include "model_core/model.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(TestCapture) : public testing::Test {
  protected:
  virtual void TearDown() {
    (void)unlink("./file1.bin");
    (void)unlink("./file2.bin");
    (void)unlink("./commands.stf");
  }
};

TEST_F(BFN_TEST_NAME(TestCapture), OutWord) {
  int chip = 0;
  uint32_t address = 0x12345678;
  uint32_t data = 0xabcdef00;
  uint16_t port = 0x1;
  int len = 12;
  uint8_t buf[] = {1,2,3,4,5,6,7,8,0,0,0,0};
  ASSERT_TRUE(GLOBAL_MODEL.get());

  RmtObjectManager *om;
  GLOBAL_MODEL->DestroyChip(chip);
  GLOBAL_MODEL->CreateChip(chip, RmtDefs::kChipType);
  GLOBAL_MODEL->InitChip(chip);
  GLOBAL_MODEL->GetObjectManager(chip, &om);
  ASSERT_TRUE(om != NULL);
  GLOBAL_MODEL->StartCapture();

  GLOBAL_MODEL->OutWord(chip, address, data);
  GLOBAL_MODEL->PacketReceivePostBuf(chip, port, buf, len); 
  
  std::ifstream data_file;
  data_file.open("file1.bin", std::ios::binary | std::ios::in);
  ASSERT_TRUE(data_file.is_open()) 
  << "Error, file did not open successfully.";
    char expected[] = 
    {'\0', '\0', '\0', 'r', 'x', 'V', '4', '\022', //0-7
     '\0', '\357', '\315', '\253'}; //8-11
    char * buffer = new char[8];
    int count = 0;
    int multiplier = 0;
    std::streamsize chars_extracted;

    do {
      data_file.read(buffer, 8);
      chars_extracted = data_file.gcount();
      count++;
      multiplier = ((count - 1) * 8);

      for(int i = 0; i < chars_extracted; i++){
        EXPECT_EQ(buffer[i], expected[i + multiplier]) 
        << "Loop num: " << count
        << "\nBytes Read: " << chars_extracted
        << "\nMultiplier = " << multiplier 
        << "\ni = " << i
        << "\nexpected[i] = " << expected[i + multiplier] << std::endl;
      }

    } while(chars_extracted == 8);

       data_file.close();
  }

TEST_F(BFN_TEST_NAME(TestCapture), IndirectWrite) {
  int chip = 0;
  uint32_t address = 0x23456789;
  uint32_t data1 = 0xabcdef00;
  uint32_t data2 = 0x89abcdef;
  uint16_t port = 0x1;
  int len = 12;
  uint8_t buf[] = {1,2,3,4,5,6,7,8,0,0,0,0};
  
  ASSERT_TRUE(GLOBAL_MODEL.get());

  RmtObjectManager *om;
  GLOBAL_MODEL->DestroyChip(chip);
  GLOBAL_MODEL->CreateChip(chip, RmtDefs::kChipType);
  GLOBAL_MODEL->InitChip(chip);
  GLOBAL_MODEL->GetObjectManager(chip, &om);
  ASSERT_TRUE(om != NULL);
  GLOBAL_MODEL->StartCapture();

  GLOBAL_MODEL->IndirectWrite(chip, address, data1, data2, 0);
    
  GLOBAL_MODEL->PacketReceivePostBuf(chip, port, buf, len);
  
  std::ifstream data_file;
  data_file.open("file1.bin", std::ios::binary | std::ios::in);
  ASSERT_TRUE(data_file.is_open())
  << "Error, file did not open successfully.";

    // using od -c file1.bin to get the expected characters
    char expected[] = 
    {'\0', '\0', '\0', 'd', '\211', 'g', 'E', '#', //0-7. Loop Num 1
     '\0', '\0', '\0', '\0', '@', '\0', '\0', '\0', //8-15. Loop Num 2
     '\002', '\0', '\0', '\0', '\0', '\357', '\315', '\253', //16-23. Loop Num 3
     '\0', '\0', '\0', '\0', '\357', '\315', '\253', '\211',//24-31. Loop Num 4
     '\0', '\0', '\0', '\0'}; //32-35. Loop Num 5

    char * buffer = new char[8];
    int count = 0;
    int multiplier = 0;
    std::streamsize chars_extracted;

    do {
      data_file.read(buffer, 8);
      chars_extracted = data_file.gcount();
     
      count++;
      multiplier = ((count - 1) * 8);

      for(int i = 0; i < chars_extracted; i++){
        EXPECT_EQ(buffer[i], expected[i + multiplier]) 
        << "Loop num: " << count
        << "\nBytes Read: " << chars_extracted
        << "\nMultiplier = " << multiplier 
        << "\ni = " << i
        << "\nexpected[i] = " << static_cast<int>(expected[i + multiplier]) << std::endl;
      }
      

    } while(chars_extracted == 8);
       data_file.close();
}

TEST_F(BFN_TEST_NAME(TestCapture), Packet) {
  int chip = 0;
  uint32_t address = 0x12345678;
  uint32_t data = 0xabcdef00;
  uint16_t port = 0x1;
  int len = 12;
  uint8_t buf[] = {1,2,3,4,5,6,7,8,0,0,0,0};
  
  
  ASSERT_TRUE(GLOBAL_MODEL.get());

  RmtObjectManager *om;
  GLOBAL_MODEL->DestroyChip(chip);
  GLOBAL_MODEL->CreateChip(chip, RmtDefs::kChipType);
  GLOBAL_MODEL->InitChip(chip);
  GLOBAL_MODEL->GetObjectManager(chip, &om);
  ASSERT_TRUE(om != NULL);
  GLOBAL_MODEL->StartCapture();

  GLOBAL_MODEL->OutWord(chip, address, data);
  GLOBAL_MODEL->PacketReceivePostBuf(chip, port, buf, len);
  GLOBAL_MODEL->OutWord(chip, address+4, data);
  
  std::string expected = "helpload file1.binpacket 1 0102030405060708load file2.bin";
  std::ifstream commands;
  std::ifstream file2;
  file2.open("file2.bin", std::ios::binary | std::ios::in);
  ASSERT_TRUE(file2); 
  ASSERT_TRUE(file2.is_open());
  file2.close();
  commands.open("commands.stf", std::ios::in); // check if its open
  ASSERT_TRUE(commands.is_open())
  << "Error, command file did not open successfully.";

  std::string line = "";
  std::string str = "";

  while(std::getline(commands, line)){
      str += line;
  }
    
  ASSERT_EQ(str, expected) 
  << "Expected String = " << expected
  << "\nActual String = " << str; 

  commands.close();

}

  class BFN_TEST_NAME(TestCaptureDir) : public testing::Test {
   protected:
    virtual void SetUp() {
      TearDown();
      (void)mkdir("./c_test_1/", S_IFDIR | 0777);
      (void)mkdir("./c_test_2/", S_IFDIR | 0777);
      (void)mkdir("./c_test_3/", S_IFDIR | 0777);
      (void)mkdir("./c_test_3/sub_dir/", S_IFDIR | 0777);
    }
    virtual void TearDown() {
      (void)unlink("./file1.bin");
      (void)unlink("./commands.stf");
      (void)unlink("./c_test_1/file1.bin");
      (void)unlink("./c_test_1/commands.stf");
      (void)rmdir("./c_test_1/");
      (void)unlink("./c_test_2/file1.bin");
      (void)unlink("./c_test_2/commands.stf");
      (void)rmdir("./c_test_2/");
      (void)unlink("./c_test_3/sub_dir/file1.bin");
      (void)unlink("./c_test_3/sub_dir/commands.stf");
      (void)rmdir("./c_test_3/sub_dir/");
      (void)rmdir("./c_test_3/");
    }
  };

  TEST_F(BFN_TEST_NAME(TestCaptureDir), Directory) {
    char line_seperator = '/';
    std::string data_filename = "file1.bin";
    std::string command_filename = "commands.stf";
    std::string directory = ".";

    uint32_t address = 0x12345678;
    uint32_t data = 0xabcdef00;

    std::ifstream data_file;
    std::ifstream command_file;
    model_core::Capture* capture;
    // Use working dir (default)
    capture = new model_core::Capture();
    capture->log_outword(address, data);
    delete capture;

    command_file.open(directory + line_seperator + command_filename, std::ios::in);
    EXPECT_TRUE(command_file);
    EXPECT_TRUE(command_file.is_open());
    command_file.close();

    data_file.open(directory + line_seperator + data_filename, std::ios::binary | std::ios::in);
    EXPECT_TRUE(data_file);
    EXPECT_TRUE(data_file.is_open());
    data_file.close();
    
    // Use dir without '/'
    directory = "./c_test_1";
    capture = new model_core::Capture(directory);
    capture->log_outword(address, data);
    delete capture;

    command_file.open(directory + line_seperator + command_filename, std::ios::in);
    EXPECT_TRUE(command_file);
    EXPECT_TRUE(command_file.is_open());
    command_file.close();

    data_file.open(directory + line_seperator + data_filename, std::ios::binary | std::ios::in);
    EXPECT_TRUE(data_file);
    EXPECT_TRUE(data_file.is_open());
    data_file.close();

    // Use dir with '/'
    directory = "./c_test_2/";
    capture = new model_core::Capture(directory);
    capture->log_outword(address, data);
    delete capture;

    command_file.open(directory + command_filename, std::ios::in);
    EXPECT_TRUE(command_file);
    EXPECT_TRUE(command_file.is_open());
    command_file.close();

    data_file.open(directory + data_filename, std::ios::binary | std::ios::in);
    EXPECT_TRUE(data_file);
    EXPECT_TRUE(data_file.is_open());
    data_file.close();

    // Use subdir
    directory = "./c_test_3/sub_dir/";
    capture = new model_core::Capture(directory);
    capture->log_outword(address, data);
    delete capture;

    command_file.open(directory + command_filename, std::ios::in);
    EXPECT_TRUE(command_file);
    EXPECT_TRUE(command_file.is_open());
    command_file.close();

    data_file.open(directory + data_filename, std::ios::binary | std::ios::in);
    EXPECT_TRUE(data_file);
    EXPECT_TRUE(data_file.is_open());
    data_file.close();
  }
}
