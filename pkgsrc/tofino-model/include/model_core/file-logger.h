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

#ifndef _MODEL_CORE_FILE_LOGGER_
#define _MODEL_CORE_FILE_LOGGER_

#include <mutex>
#include <vector>
#include <time.h>

#include <common/rmt-util.h>

namespace model_core {

class Model;
class LogBuffer;

// Helper class to provide file logging
class FileLogger {

 public:
  // Note C++11 requires these strings be defined in file-logger.cpp; without the .cpp defs linker fails
  static constexpr char     kLogfilePathSeparatorChar       = '/';
  static constexpr char     kDefaultLogDirStr[]             = ".";
  static constexpr char     kLogfilePrefixStr[]             = "model_";
  static constexpr char     kLogfileDateTimeFmt[]           = "%Y%m%d_%H%M%S";
  static constexpr char     kLogfileSuffixTypeFmt[]         = "_Type%d";
  static constexpr char     kLogfileSuffixTypeChipFmt[]     = "_Type%d_Chip%d";
  static constexpr char     kLogfileSuffixTypeChipPipeFmt[] = "_Type%d_Chip%d_Pipe%d";
  static constexpr char     kLogfileExtensionStr[]          = ".log";
  //static constexpr char   kLogDateTimeFmt[]               = "%Y%m%d_%H%M%S";
  static constexpr char     kLogDateTimeFmt[]               = "%m-%d %T";
  static constexpr char     kLogOutputFmt[]                 = ":%s.%06d:    %s"; // TimeStr TimeUsecs LogStr

  static constexpr int      kMaxLenLogDirStr   = 256+1;

  static constexpr uint32_t kCreateChipLogger  = 0x01;
  static constexpr uint32_t kCreatePipeLogger  = 0x02;
  static constexpr uint32_t kCreateStageLogger = 0x04;
  static constexpr uint32_t kConsoleLog        = 0x10;


  FileLogger(int chip, int pipe, FileLogger* parent, uint32_t flags);
  virtual ~FileLogger();

  int           chip()                const { return chip_; }
  int           pipe()                const { return pipe_; }
  FileLogger*   parent()              const { return parent_; }
  uint32_t      flags()               const { return flags_; }
  bool          flag_set(uint32_t f)  const { return (flags() & f) != 0; }
  time_t        when_created()        const { return when_created_; }
  const char*   log_dir()             const { return log_dir_; }

  void          reset();
  void          set_log_dir(const char *log_dir);
  void          set_per_chip_logging(bool tf);
  void          set_per_pipe_logging(bool tf);
  void          set_per_stage_logging(bool tf);
  void          set_console_logging(bool tf);
  bool          log(int chip, int pipe, const char *logstr);


 protected:
  static bool   append_logdir_str(LogBuffer *buf, const char* log_dir);
  static bool   append_prefix_str(LogBuffer *buf);
  static bool   append_datetime_str(LogBuffer *buf, time_t t);
  static bool   append_suffix_str(LogBuffer *buf);
  static bool   append_suffix_str(LogBuffer *buf, int type);
  static bool   append_suffix_str(LogBuffer *buf, int chip, int type);
  static bool   append_suffix_str(LogBuffer *buf, int chip, int pipe, int type);
  static bool   append_extension_str(LogBuffer *buf);


 private:
  static time_t get_time(uint32_t *micros=nullptr);

  FileLogger*   set_sub_logger(int i, FileLogger *f);
  FileLogger*   lookup_sub_logger(int i);
  FileLogger*   create_sub_logger(int i);
  FileLogger*   get_sub_logger(int i);

  FileLogger*   get_root();
  Model*        get_model();
  int           get_type(int chip);
  time_t        get_when_created();
  const char*   get_log_dir();
  void          create_log_file();
  bool          do_log(int chip, int pipe, const char *logstr);


  // These overridden in ModelLogger ChipLogger PipeLogger subclasses
  virtual bool        use_sub_logger(int chip, int pipe) = 0;
  virtual int         get_sub_logger_index(int chip, int pipe) = 0;
  virtual FileLogger* new_sub_logger(int i) = 0;
  virtual bool        append_suffix(LogBuffer *buf, int type) = 0;
  virtual Model*      model() { return nullptr; }


  DISALLOW_COPY_AND_ASSIGN(FileLogger);

  int                         chip_;
  int                         pipe_;
  FileLogger*                 parent_;
  uint32_t                    flags_;
  time_t                      when_created_;
  char*                       log_dir_;
  std::FILE*                  file_;
  std::mutex                  mutex_;
  std::vector< FileLogger* >  sub_loggers_;
};




class PipeLogger : public FileLogger {
 public:
  PipeLogger(int chip, int pipe, FileLogger *parent, uint32_t flags) : FileLogger(chip, pipe, parent, flags) { }

  bool        use_sub_logger(int chip, int pipe)       override { (void)chip; (void)pipe; return flag_set(kCreateStageLogger); }
  int         get_sub_logger_index(int chip, int pipe) override { (void)chip; (void)pipe; return -1; }
  FileLogger* new_sub_logger(int i)                    override { (void)i; return nullptr; }
  bool        append_suffix(LogBuffer *buf, int type)  override { return append_suffix_str(buf, chip(), pipe(), type); }
};


class ChipLogger : public FileLogger {
 public:
  ChipLogger(int chip, FileLogger *parent, uint32_t flags) : FileLogger(chip, -1, parent, flags) { }

  bool        use_sub_logger(int chip, int pipe)       override { (void)chip; return (pipe >= 0) && flag_set(kCreatePipeLogger); }
  int         get_sub_logger_index(int chip, int pipe) override { (void)chip; return pipe; }
  FileLogger* new_sub_logger(int i)                    override { return new PipeLogger(chip(), i, this, flags()); }
  bool        append_suffix(LogBuffer *buf, int type)  override { return append_suffix_str(buf, chip(), type); }
};

class ModelLogger : public FileLogger {
 public:
  ModelLogger(Model *model) : FileLogger(-1, -1, nullptr, 0u), model_(model) { }

  bool        use_sub_logger(int chip, int pipe)       override { (void)pipe; return (chip >= 0) && flag_set(kCreateChipLogger); }
  int         get_sub_logger_index(int chip, int pipe) override { (void)pipe; return chip; }
  FileLogger* new_sub_logger(int i)                    override { return new ChipLogger(i, this, flags()); }
  bool        append_suffix(LogBuffer *buf, int type)  override { (void)type; return append_suffix_str(buf); }
  Model*      model()                                  override { return model_; }

 private:
  Model *model_;
};





}  // model_core


#endif //_MODEL_CORE_FILE_LOGGER_
