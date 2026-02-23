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


#ifndef S3_LOG_H__
#define S3_LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf_switch/bf_switch_types.h"

#define SWITCH_LOG_BUFFER_SIZE 600

// can't call this SWITCH_OBJECT_TYPE_NONE because it is a generated type
// Do not use this outside of S3
#define SWITCH_OT_NONE 0

typedef enum _switch_operation_t {
  SMI_CREATE_OPERATION,
  SMI_DELETE_OPERATION,
  SMI_GET_OPERATION,
  SMI_SET_OPERATION,
} switch_operation_t;

// operations for user to enable logging
void toggle_operation(switch_operation_t operation, bool isEnabled);
void toggle_all_operations(bool isEnabled);
void set_log_level(switch_verbosity_t new_verbosity);
switch_verbosity_t get_log_level();
void set_log_level_object(switch_object_type_t object_type,
                          switch_verbosity_t verbosity);
void set_log_level_all_objects(switch_verbosity_t verbosity);

static inline const char *switch_verbosity_to_string(
    switch_verbosity_t verbosity) {
  switch (verbosity) {
    case SWITCH_API_LEVEL_ERROR:
      return "error";
    case SWITCH_API_LEVEL_WARN:
      return "warn";
    case SWITCH_API_LEVEL_INFO:
      return "info";
    case SWITCH_API_LEVEL_DEBUG:
      return "debug";
    case SWITCH_API_LEVEL_DETAIL:
      return "detail";
    default:
      return "error";
  }
}

#ifdef __cplusplus
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <sstream>
#include "third_party/fmtlib/fmt/format.h"
#include "third_party/fmtlib/fmt/ostream.h"

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec);
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &vec);
std::ostream &operator<<(std::ostream &os, const switch_object_id_t &object_id);
std::ostream &operator<<(std::ostream &os, const switch_operation_t &operation);
std::ostream &operator<<(std::ostream &os, const switch_mac_addr_t &mac_struct);
std::ostream &operator<<(std::ostream &os, const switch_enum_t &enum_data);
std::ostream &operator<<(std::ostream &os, const switch_string_t &string_data);
std::ostream &operator<<(std::ostream &os, const switch_ip6_t &ipv6_address);
std::ostream &operator<<(std::ostream &os,
                         const switch_ip_addr_family_t &family);
std::ostream &operator<<(std::ostream &os,
                         const switch_ip_address_t &ip_address);
std::ostream &operator<<(std::ostream &os, const switch_ip_prefix_t &ip_prefix);
std::ostream &operator<<(std::ostream &os, const switch_range_t &range);
std::ostream &operator<<(std::ostream &os,
                         const switch_attr_type_t &attribute_type);
std::ostream &operator<<(std::ostream &os,
                         const switch_attr_list_t &attribute_list);
std::ostream &operator<<(std::ostream &os, const smi::attr_w &attr);
std::ostream &operator<<(std::ostream &os,
                         const std::vector<smi::attr_w> &attr);
std::ostream &operator<<(std::ostream &os, const std::set<smi::attr_w> &attr);
std::ostream &operator<<(std::ostream &os,
                         const switch_attribute_value_t &attribute_value);
std::ostream &operator<<(std::ostream &os, const switch_attribute_t &attribute);
namespace smi {
namespace logging {

template <typename T>
std::string formatNumber(T number) {
  return fmt::format("{:n}", number);
}

template <typename T>
std::string stream_to_string(const T &value) {
  std::ostringstream ss;
  ss << value;
  return ss.str();
}

// LCOV_EXCL_START
template <typename Arg1, typename... Args>
std::string createMessage(const char *format,
                          const Arg1 &arg1,
                          const Args &... args) {
  fmt::memory_buffer local_buf;
  fmt::format_to(local_buf, format, arg1, args...);
  return fmt::to_string(local_buf);
}
// LCOV_EXCL_STOP

// operations for actual logging
bool switch_log(switch_verbosity_t verbosity, const char *message);
bool switch_log(switch_verbosity_t verbosity,
                switch_object_type_t object_type,
                const char *message);
bool switch_log(switch_verbosity_t verbosity,
                switch_object_type_t object_type,
                switch_operation_t operation,
                const char *message);

template <typename Arg1, typename... Args>
bool switch_log(switch_verbosity_t verbosity,
                switch_object_type_t object_type,
                const char *format,
                const Arg1 &arg1,
                const Args &... args) {
  if (verbosity > get_log_level()) return true;
  std::string message = createMessage(format, arg1, args...);
  return switch_log(verbosity, object_type, message.c_str());
}

template <typename Arg1, typename... Args>
bool switch_log(switch_verbosity_t verbosity,
                switch_object_type_t object_type,
                switch_operation_t operation,
                const char *format,
                const Arg1 &arg1,
                const Args &... args) {
  if (verbosity > get_log_level()) return true;
  std::string message = createMessage(format, arg1, args...);
  return switch_log(verbosity, object_type, operation, message.c_str());
}

const std::unordered_map<switch_object_type_t, switch_verbosity_t>
    &get_log_level_all_objects();

#define SWITCH_DEBUG_LOG(__fn)                     \
  if (get_log_level() >= SWITCH_API_LEVEL_DEBUG) { \
    __fn;                                          \
  }

#define SWITCH_DETAIL_LOG(__fn)                     \
  if (get_log_level() >= SWITCH_API_LEVEL_DETAIL) { \
    __fn;                                           \
  }

switch_status_t logging_init(switch_verbosity_t verbosity,
                             bool override_log_level);

#define CHECK_RET(x, ret)                              \
  do {                                                 \
    if (unlikely(x)) {                                 \
      switch_log(SWITCH_API_LEVEL_ERROR,               \
                 static_cast<switch_object_type_t>(0), \
                 "{}:{} ERROR {} reason {}",           \
                 __func__,                             \
                 __LINE__,                             \
                 switch_error_to_string(ret),          \
                 #x);                                  \
      return ret;                                      \
    }                                                  \
  } while (0)

#define ENTER()                                      \
  do {                                               \
    switch_log(SWITCH_API_LEVEL_DEBUG,               \
               static_cast<switch_object_type_t>(0), \
               "{}: Enter",                          \
               __func__);                            \
  } while (0)
#define EXIT()                                       \
  do {                                               \
    switch_log(SWITCH_API_LEVEL_DEBUG,               \
               static_cast<switch_object_type_t>(0), \
               "{}: Exit",                           \
               __func__);                            \
  } while (0)

} /* namespace logging */
} /* namespace smi */

#endif /* __cplusplus */

#ifndef __cplusplus
#include <target-sys/bf_sal/bf_sys_intf.h>

#define SWITCH_LOG_INFO(...) \
  printf(__VA_ARGS__);       \
  printf("\n");

#define SWITCH_LOG_ERROR(...) SWITCH_LOG_INFO(__VA_ARGS__);
#define SWITCH_LOG_DEBUG(...) SWITCH_LOG_INFO(__VA_ARGS__)

#define CHECK_RET(x, ret)                            \
  do {                                               \
    if (unlikely(x)) {                               \
      SWITCH_LOG_ERROR("%s:%d ERROR %s reason %s\n", \
                       __FILE__,                     \
                       __LINE__,                     \
                       switch_error_to_string(ret),  \
                       #x);                          \
      return ret;                                    \
    }                                                \
  } while (0)

#endif
#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#endif  // S3_LOG_H__
