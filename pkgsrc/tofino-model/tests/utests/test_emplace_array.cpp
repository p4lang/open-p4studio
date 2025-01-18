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

#include <array>

#include "gtest.h"
#include "common/emplace_array.h"
#include "common/ebuf_reg_array.h"

namespace rmt_utests {


TEST(TestEmplaceArray, Generic) {
  struct Data {
    int a, b;
    Data(int a, int b) : a(a), b(b) {};
  };

  int a = 10;
  int b = 20;
  auto ctor = [&](){return Data(a++, b++);};

  // For these to compile, we must be returning somthing that can initialise an array!
  std::array<Data,1> a1(model_common::emplace_array<Data, 1>(ctor));
  EXPECT_EQ(a1.size(), 1);
  EXPECT_EQ(a1[0].a, 10);
  EXPECT_EQ(a1[0].b, 20);

  a = 10;
  b = 20;
  auto a2(model_common::emplace_array<Data, 2>(ctor));
  EXPECT_EQ(a2.size(), 2);
  EXPECT_EQ(a2[1].a, 11);
  EXPECT_EQ(a2[1].b, 21);

  a = 10;
  b = 20;
  auto a3 = model_common::emplace_array<Data, 3>(ctor);
  EXPECT_EQ(a3.size(), 3);
  EXPECT_EQ(a3[0].a, 10);
  EXPECT_EQ(a3[2].b, 22);

  // Negative and zero Array sized fail to compile.
  // auto a0(model_common::emplace_array<Data, 0>(ctor));
  // auto aNeg(model_common::emplace_array<Data, -1>(ctor));
}


TEST(TestEmplaceArray, EBufRegArrayMultiwayEBufs) {
  struct Data {
    int chip, pipe, slice, ebuf, chan;
    // 5 arguments construtor called by first template operator().
    // It wont compile if we only offer a 4 argument constructor.
    Data(int chip, int pipe, int slice, int ebuf, int chan) :
      chip(chip), pipe(pipe) , slice(slice), ebuf(ebuf), chan(chan) {}
  };

  const int ChipIndex = 11;
  const int PipeIndex = 22;
  const int NumSlices = 2;
  const int NumEbufs = 3; // This exercises the first template operator().
  const int NumChans = 4;
  const int Size = NumSlices * NumEbufs * NumChans;

  std::array<Data, Size> a(model_common::EBufRegArray<Data, NumSlices, NumEbufs, NumChans>
                            ::create(ChipIndex, PipeIndex));
  EXPECT_EQ(a.size(), Size);

  int counter = 0;
  for (auto& d : a) {
    EXPECT_EQ(d.chip, 11);
    EXPECT_EQ(d.pipe, 22);
    int chanI = counter % NumChans;
    int ebufI = (counter / NumChans) % NumEbufs;
    int sliceI = counter / (NumChans * NumEbufs);
    EXPECT_EQ(d.slice, sliceI);
    EXPECT_EQ(d.ebuf, ebufI);
    EXPECT_EQ(d.chan, chanI);
    ++counter;
  }
}


TEST(TestEmplaceArray, EBufRegArrayOnewayEbuf) {
  struct Data {
    int chip, pipe, slice, ebuf = -1, chan;
    // 4 arguments construtor called by second template operator().
    // It wont compile if we only offer a 5 argument constructor.
    Data(int chip, int pipe, int slice, int chan) :
      chip(chip), pipe(pipe) , slice(slice), chan(chan) {}
  };

  const int ChipIndex = 11;
  const int PipeIndex = 22;
  const int NumSlices = 2;
  const int NumEbufs = 1; // This exercises the second template operator().
  const int NumChans = 4;
  const int Size = NumSlices * NumEbufs * NumChans;

  auto a(model_common::EBufRegArray<Data, NumSlices, NumEbufs, NumChans>
         ::create(ChipIndex, PipeIndex));
  EXPECT_EQ(a.size(), Size);

  int counter = 0;
  for (auto& d : a) {
    EXPECT_EQ(d.chip, 11);
    EXPECT_EQ(d.pipe, 22);
    int chanI = counter % NumChans;
    int ebufI = -1;
    int sliceI = counter / (NumChans * NumEbufs);
    EXPECT_EQ(d.slice, sliceI);
    EXPECT_EQ(d.ebuf, ebufI);
    EXPECT_EQ(d.chan, chanI);
    ++counter;
  }
}

}  // namespace rmt_utests
