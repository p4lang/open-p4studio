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

#include <model_core/event.h>
#include <model_core/model.h>
#include <common/rmt-assert.h>
#include <chrono>
#include <ctime>

namespace model_core {

/* Utility to convert Severity enum to JSON enum names */
const char* severityToString(const Severity severity) {
  switch (severity) {
    case Severity::kDebug:
      return "DEBUG";
    case Severity::kInfo:
      return "INFO";
    case Severity::kWarn:
      return "WARN";
    case Severity::kError:
      return "ERROR";
    default:
      return "UNKNOWN";
  }
}

/* Utility to convert ChipType to JSON enum names */
const char* chipTypeToString(const uint8_t type) {
  switch (type) {
    case ChipType::kTofino:
    // case ChipType::kTofinoA0: duplicate value
      return "tofino";
    case ChipType::kTofinoB0:
      return "tofinob0";
    case ChipType::kJbay:
      return "jbay";
    case ChipType::kRsvd0:
      return "rsvd0";
    default:
      return "undef";
  }
}

/* Get date string in the format of YYYY-MM-DDTHH:MM:SS.mmmmmmZ */
std::string iso8601(std::chrono::system_clock::time_point time_pt,
                    bool use_local_time) {
  using std::chrono::duration;
  using std::chrono::hours;
  using std::chrono::minutes;
  using std::chrono::seconds;
  using std::chrono::microseconds;
  using std::chrono::system_clock;
  using std::chrono::duration_cast;
  using std::ratio;
  using std::ratio_multiply;
  using days = duration<int, ratio_multiply<hours::period, ratio<24> >::type>;
  system_clock::duration tp = time_pt.time_since_epoch();
  days d = duration_cast<days>(tp);
  tp -= d;
  hours h = duration_cast<hours>(tp);
  tp -= h;
  minutes m = duration_cast<minutes>(tp);
  tp -= m;
  seconds s = duration_cast<seconds>(tp);
  tp -= s;
  microseconds ms = duration_cast<microseconds>(tp);
  time_t tt = system_clock::to_time_t(time_pt);
  tm* lcltm = nullptr;
  if (use_local_time) lcltm = localtime(&tt);
  else lcltm = gmtime(&tt);
  RMT_ASSERT_NOT_NULL(lcltm);
  char time_str[81];
  snprintf(time_str, 81, "%d-%.02d-%.02dT%.02d:%.02d:%.02d.%.06" PRId64 "Z",
      lcltm->tm_year + 1900, lcltm->tm_mon + 1, lcltm->tm_mday,
      lcltm->tm_hour, lcltm->tm_min, lcltm->tm_sec, ms.count());
  return time_str;
}

}  // model_core
