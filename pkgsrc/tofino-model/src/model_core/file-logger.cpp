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

#include <string.h>
#include <unistd.h>
#include <model_core/model.h>
#include <model_core/log-buffer.h>
#include <model_core/file-logger.h>


namespace model_core {

// Have to define these here too for C++11
constexpr char FileLogger::kDefaultLogDirStr[];
constexpr char FileLogger::kLogfilePrefixStr[];
constexpr char FileLogger::kLogfileDateTimeFmt[];
constexpr char FileLogger::kLogfileSuffixTypeFmt[];
constexpr char FileLogger::kLogfileSuffixTypeChipFmt[];
constexpr char FileLogger::kLogfileSuffixTypeChipPipeFmt[];
constexpr char FileLogger::kLogfileExtensionStr[];
//constexpr char* FileLogger::kLogDateTimeFmt[];
constexpr char FileLogger::kLogDateTimeFmt[];
constexpr char FileLogger::kLogOutputFmt[];


FileLogger::FileLogger(int chip, int pipe, FileLogger* parent, uint32_t flags)
    : chip_(chip), pipe_(pipe), parent_(parent), flags_(flags),
      when_created_(get_time()), log_dir_(nullptr), file_(nullptr), mutex_(), sub_loggers_() {
}
FileLogger::~FileLogger() {
  reset();
}

// These the main public methods
void FileLogger::reset() {
  int sz = sub_loggers_.size();
  for (int i = 0; i < sz; i++) {
    FileLogger *f = lookup_sub_logger(i);
    if (f != nullptr) delete f;
    set_sub_logger(i, nullptr);
  }

  flags_ = kConsoleLog;
  when_created_ = get_time();

  std::FILE *fp = file_;
  file_ = nullptr;
  if (fp != nullptr) fclose(fp);

  set_log_dir(nullptr);
}
void FileLogger::set_log_dir(const char *log_dir) {
  std::unique_lock<std::mutex> lock(mutex_);
  // Free any existing log_dir
  if (log_dir_ != nullptr) free((void*)log_dir_);
  log_dir_ = nullptr;

  if (log_dir == nullptr) return;

  // Allocate new buf to copy log_dir into but
  // insist log_dir path is less than configured max len
  const size_t maxsz = (size_t)(kMaxLenLogDirStr + 1);
  size_t sz = strnlen(log_dir, maxsz);
  if (sz == 0) return;
  RMT_ASSERT((sz < maxsz) && "set_log_dir pathname too long");

  LogBuffer buf(sz);
  bool ok = buf.Append("%s", log_dir);
  RMT_ASSERT(ok && "LogBuffer Append failed");
  log_dir_ = buf.ExtractBuf(); // Permanently removes
}
void FileLogger::set_per_chip_logging(bool tf) {
  if (tf) flags_ |= kCreateChipLogger; else flags_ &= ~kCreateChipLogger;
}
void FileLogger::set_per_pipe_logging(bool tf) {
  if (tf) flags_ |= kCreatePipeLogger; else flags_ &= ~kCreatePipeLogger;
}
void FileLogger::set_per_stage_logging(bool tf) {
  if (tf) flags_ |= kCreateStageLogger; else flags_ &= ~kCreateStageLogger;
}
void FileLogger::set_console_logging(bool tf) {
  if (tf) flags_ |= kConsoleLog; else flags_ &= ~kConsoleLog;
}

bool FileLogger::log(int chip, int pipe, const char *logstr) {
  bool done = false;
  // use_sub_logger/get_sub_logger both overridden in subclass
  if (use_sub_logger(chip, pipe)) {
    FileLogger *sub_logger = get_sub_logger(get_sub_logger_index(chip, pipe));
    done = sub_logger->log(chip, pipe, logstr);
  }
  if (!done) done = do_log(chip, pipe, logstr); // Log here (instead)
  return done;
}

// Various static funcs for setting up logfile pathname
bool FileLogger::append_logdir_str(LogBuffer *buf, const char* dir) {
  return buf->Append("%s%c", dir, kLogfilePathSeparatorChar);
}
bool FileLogger::append_prefix_str(LogBuffer *buf) {
  return buf->Append("%s", kLogfilePrefixStr);
}
bool FileLogger::append_datetime_str(LogBuffer *buf, time_t t) {
  char time_buf[32] = { 'u', 'n', 'k', 'n', 'o', 'w', 'n', '\0' };
  std::tm *ltm = std::localtime(&t);
  RMT_ASSERT_NOT_NULL(ltm);
  std::strftime(time_buf, sizeof(time_buf), kLogfileDateTimeFmt, ltm);
  return buf->Append("%s", time_buf);
}
bool FileLogger::append_suffix_str(LogBuffer *buf) {
  return true;
}
bool FileLogger::append_suffix_str(LogBuffer *buf, int type) {
  return buf->Append(kLogfileSuffixTypeFmt, type);
}
bool FileLogger::append_suffix_str(LogBuffer *buf, int chip, int type) {
  return buf->Append(kLogfileSuffixTypeChipFmt, type, chip);
}
bool FileLogger::append_suffix_str(LogBuffer *buf, int chip, int pipe, int type) {
  return buf->Append(kLogfileSuffixTypeChipPipeFmt, type, chip, pipe);
}
bool FileLogger::append_extension_str(LogBuffer *buf) {
  return buf->Append("%s", kLogfileExtensionStr);
}
time_t FileLogger::get_time(uint32_t *usecs) {
  auto now = std::chrono::system_clock::now();
  if (usecs != nullptr) {
    auto now_tp = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    auto now_us = now_tp.time_since_epoch();
    *usecs = now_us.count() % 1000000;
  }
  return std::chrono::system_clock::to_time_t(now);
}

// Create sub-loggers (ie per-chip, per-chip per-pipe)
FileLogger *FileLogger::set_sub_logger(int i, FileLogger *f) {
  sub_loggers_.at(i) = f;
  return f;
}
FileLogger *FileLogger::lookup_sub_logger(int i) {
  RMT_ASSERT((i >= 0) && (i <= 999));
  if (i >= static_cast<int>(sub_loggers_.size())) sub_loggers_.resize(i+1);
  return sub_loggers_.at(i);
}
FileLogger *FileLogger::create_sub_logger(int i) {
  FileLogger *ret = lookup_sub_logger(i);
  RMT_ASSERT(ret == nullptr);
  ret = new_sub_logger(i); // Overridden in sub-class
  RMT_ASSERT(ret != nullptr);
  return set_sub_logger(i, ret);
}
FileLogger *FileLogger::get_sub_logger(int i) {
  FileLogger *ret = lookup_sub_logger(i);
  if (ret == nullptr) ret = create_sub_logger(i);
  return ret;
}



FileLogger *FileLogger::get_root() {
  return (parent() == nullptr) ?this :parent()->get_root();
}
Model *FileLogger::get_model() {
  return get_root()->model();
}
int FileLogger::get_type(int chip) {
  Model *model = get_model();
  return (model == nullptr) ?0 :static_cast<int>(model->GetType(chip));
}
time_t FileLogger::get_when_created() {
  return get_root()->when_created();
}
const char* FileLogger::get_log_dir() {
  const char *logdir = get_root()->log_dir();
  return (logdir == nullptr) ?kDefaultLogDirStr :logdir;
}
void FileLogger::create_log_file() {
  int type = get_type(chip()); // Returns ChipType::kNone if chip -1
  const char *dir = get_log_dir();
  LogBuffer buf(256); // Space for Dir+Prefix<203>_Datetime<15>_Type<2>_Chip<2>_Pipe<2>Suffix<15>\0;
  if (append_logdir_str(&buf, dir) &&
      append_prefix_str(&buf) &&
      append_datetime_str(&buf, get_when_created()) && // Use datetime when root logger created
      append_suffix(&buf, type) && // Overridden in sub-class
      append_extension_str(&buf)) {
    std::unique_lock<std::mutex> lock(mutex_);
    const char *pathname = buf.GetBuf();
    std::FILE *fp = fopen(pathname, "w+");
    if (fp != nullptr) {
      file_ = fp;
      // Report new log file setup
      fprintf(stdout, "LOGS captured in: %s\n", buf.GetBuf());
      if (isatty(fileno(stdout)) == 0) fflush(stdout);
    }
  } else {
    fprintf(stderr, "ERROR - log path exceeds 256B\n");
  }
}


bool FileLogger::do_log(int chip, int pipe, const char *logstr) {
  if (file_ == nullptr) create_log_file(); // Log file created on demand

  bool logged = false;
  uint32_t usecs = 0u;
  time_t t = get_time(&usecs);
  char time_buf[32] = { 'u', 'n', 'k', 'n', 'o', 'w', 'n', '\0' };
  std::tm *ltm = std::localtime(&t);
  RMT_ASSERT_NOT_NULL(ltm);
  std::strftime(time_buf, sizeof(time_buf), kLogDateTimeFmt, ltm);

  if ((flags() & kConsoleLog) != 0u) {
    fprintf(stdout, kLogOutputFmt, time_buf, usecs, logstr);
    if (isatty(fileno(stdout)) == 0) fflush(stdout);
  }
  std::unique_lock<std::mutex> lock(mutex_);
  if (file_ != nullptr) {
    fprintf(file_, kLogOutputFmt, time_buf, usecs, logstr);
    fflush(file_);
    logged = true;
  }
  return logged;
}


}
