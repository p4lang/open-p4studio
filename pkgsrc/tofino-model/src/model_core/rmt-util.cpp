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

#include <string>
#include <common/rmt-util.h>

namespace model_common {

  bool Util::is_little_endian() {
    short int word = 0x0001;
    return (*(char*)&word == 0x01);
  }
  int Util::hexchar2int(char in) {
    if (in >= '0' && in <= '9')
      return in - '0';
    else if (in >= 'A' && in <= 'F')
      return in - 'A' + 10;
    else if (in >= 'a' && in <= 'f')
      return in - 'a' + 10;
    else
      return -1;
  }
  int Util::hexmakebuf(const std::string hexstr, int len, uint8_t buf[]) {
    int i = 0, j = 0;
    if ((len % 2) == 0) {
      while (i < len) {
        int c0 = hexchar2int(hexstr[i]);
        int c1 = hexchar2int(hexstr[i+1]);
        if ((c0 >= 0) && (c1 >= 0)) {
          uint8_t c = (uint8_t)((c0 * 16) + c1);
          buf[j++] = c;
          i += 2;
        } else break;
      }
    }
    return i;
  }
  int Util::hexmakebuf(const char *hexstr, int len, uint8_t buf[]) {
    int i = 0, j = 0;
    if ((len % 2) == 0) {
      while (i < len) {
        int c0 = hexchar2int(hexstr[i]);
        int c1 = hexchar2int(hexstr[i+1]);
        if ((c0 >= 0) && (c1 >= 0)) {
          uint8_t c = (uint8_t)((c0 * 16) + c1);
          buf[j++] = c;
          i += 2;
        } else break;
      }
    }
    return i;
  }
}
