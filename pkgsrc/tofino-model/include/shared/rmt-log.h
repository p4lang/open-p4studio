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

#ifndef _SHARED_RMT_LOG_H_
#define _SHARED_RMT_LOG_H_

#include <cstdio>
#include <cstdarg>
#include <list>

#include <common/rmt.h>
#include <rmt-defs.h>
#include <rmt-types.h>
#include <rmt-debug.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


  //extern void rmt_0bad_mode_set( int en );
  //typedef void *(rmt_logging_init_f)(int id);

  enum rmt_log_salu_values_ {
    RMT_LOG_SALU_WIDTH = 0,
    RMT_LOG_SALU_INDEX,
    RMT_LOG_SALU_REGISTER_LO,
    RMT_LOG_SALU_REGISTER_HI,
    RMT_LOG_SALU_PHV_LO,
    RMT_LOG_SALU_PHV_HI,
    RMT_LOG_SALU_COND_INPUT_RAM_HI,
    RMT_LOG_SALU_COND_INPUT_RAM_LO,
    RMT_LOG_SALU_COND_INPUT_PHV_HI,
    RMT_LOG_SALU_COND_INPUT_PHV_LO,
    RMT_LOG_SALU_COND_OUTPUT_HI,
    RMT_LOG_SALU_COND_OUTPUT_LO,
    RMT_LOG_SALU_UPDATE_LO_1_PRED,
    RMT_LOG_SALU_UPDATE_LO_2_PRED,
    RMT_LOG_SALU_UPDATE_HI_1_PRED,
    RMT_LOG_SALU_UPDATE_HI_2_PRED,
    RMT_LOG_SALU_UPDATE_LO_1_RESULT,
    RMT_LOG_SALU_UPDATE_LO_2_RESULT,
    RMT_LOG_SALU_UPDATE_HI_1_RESULT,
    RMT_LOG_SALU_UPDATE_HI_2_RESULT,
    RMT_LOG_SALU_NEW_RAM_HI,
    RMT_LOG_SALU_NEW_RAM_LO,
    RMT_LOG_SALU_OUTPUT_PRED,
    RMT_LOG_SALU_OUTPUT_RESULT,
  };
#ifdef __cplusplus
}

#define RMT_LOG_ERROR(...) do {                     \
    RMT_LOG(RmtDebug::error(), __VA_ARGS__);        \
} while (0);                                        \

#define RMT_LOG_ERROR2(...) do {                    \
    RMT_LOG(RmtDebug::error2(), __VA_ARGS__);       \
} while (0);                                        \

#define RMT_LOG_WARN(...) do {                      \
    RMT_LOG(RmtDebug::warn(), __VA_ARGS__);         \
} while (0);                                        \

#define RMT_LOG_INFO(...) do {                      \
    RMT_LOG(RmtDebug::info(), __VA_ARGS__);         \
} while (0);                                        \

#define RMT_LOG_VERBOSE(...) do {                   \
    RMT_LOG(RmtDebug::verbose(), __VA_ARGS__);      \
} while (0);                                        \

//Run RMT_LOG depending on if logging is on/off
#define RMT_LOG_IF_NOT_NULL(log_buf, flags, ...) do {  \
    if(log_buf) {                                      \
      RMT_LOG(flags, __VA_ARGS__);                     \
    }                                                  \
} while (0)                                           


// For performance these have to be macros because it must check
//  the flags to see if logging is needed before it calls rmt_log
//  and evaluates the ... args. This is because some of the args
//  are expensive to calculate (eg stringifying registers).
#define RMT_LOG_OBJ(OBJ,FLAGS,...) do {                                 \
    if (OBJ && OBJ->rmt_log_check(FLAGS))  {                            \
      OBJ->rmt_log_va(FLAGS,__VA_ARGS__);                                  \
    }                                                                   \
  } while(0)
#define RMT_LOG(FLAGS,...) do {                                 \
    if (this->rmt_log_check(FLAGS))  {                            \
      this->rmt_log_va(FLAGS,__VA_ARGS__);                                  \
    }                                                                   \
  } while(0)

// P4 logs
#define RMT_P4_LOG(FLAGS, ...) do { \
    RMT_TYPE_LOG(RMT_LOG_TYPE_P4, FLAGS, __VA_ARGS__); \
} while (0);
#define RMT_P4_LOG_FATAL(...) do {                     \
  RMT_P4_LOG(RmtDebug::kRmtDebugFatal, __VA_ARGS__);   \
} while (0);
#define RMT_P4_LOG_ERROR(...) do {                     \
  RMT_P4_LOG(RmtDebug::kRmtDebugError, __VA_ARGS__);   \
} while (0);

#define RMT_P4_LOG_INFO(...) do {                 \
  RMT_P4_LOG(RmtDebug::kRmtDebugInfo, __VA_ARGS__);    \
} while (0);

#define RMT_P4_LOG_VERBOSE(...) do {                    \
  RMT_P4_LOG(RmtDebug::kRmtDebugVerbose, __VA_ARGS__);    \
} while (0);

#define RMT_P4_LOG_DEBUG(...) do {                 \
  RMT_P4_LOG(RmtDebug::kRmtDebugDebug, __VA_ARGS__);    \
} while (0);

// log multiple strings at once
#define RMT_LOG_STRS(str_list, t_, level_) do { \
    for (auto it = str_list.begin(); it != str_list.end(); it++) {  \
      RMT_TYPE_LOG_STRING(t_, level_,                    \
                        (*it).c_str());                             \
    }                                                               \
} while (0)
#define RMT_P4_LOG_VERBOSE_STRS(str_list) do {                      \
      RMT_LOG_STRS(str_list, RMT_LOG_TYPE_P4, RmtDebug::kRmtDebugVerbose); \
} while (0)
#define RMT_P4_LOG_DEBUG_STRS(str_list) do {                      \
      RMT_LOG_STRS(str_list, RMT_LOG_TYPE_P4,RmtDebug::kRmtDebugDebug); \
} while (0)

// Pkt logs
#define RMT_PKT_LOG_FATAL(...) do {                     \
  RMT_TYPE_LOG(RMT_LOG_TYPE_PKT, RmtDebug::kRmtDebugFatal, __VA_ARGS__);   \
} while (0);
#define RMT_PKT_LOG_ERROR(...) do {                     \
  RMT_TYPE_LOG(RMT_LOG_TYPE_PKT, RmtDebug::kRmtDebugError, __VA_ARGS__);   \
} while (0);

#define RMT_PKT_LOG_INFO(...) do {                      \
  RMT_TYPE_LOG(RMT_LOG_TYPE_PKT, RmtDebug::kRmtDebugInfo, __VA_ARGS__);    \
} while (0);

#define RMT_PKT_LOG_VERBOSE(...) do {                      \
  RMT_TYPE_LOG(RMT_LOG_TYPE_PKT, RmtDebug::kRmtDebugVerbose, __VA_ARGS__);    \
} while (0);

#define RMT_PKT_LOG_DEBUG(...) do {                   \
  RMT_TYPE_LOG(RMT_LOG_TYPE_PKT, RmtDebug::kRmtDebugDebug, __VA_ARGS__); \
} while (0);

#define RMT_TYPE_LOG_OBJ(OBJ, t_, FLAGS,...) do {                                 \
    if (OBJ && OBJ->rmt_log_type_check(FLAGS, t_))  {                            \
      OBJ->rmt_log_va(t_, FLAGS,__VA_ARGS__);                                  \
    }                                                                   \
  } while(0);
#define RMT_TYPE_LOG(t_, FLAGS,...) do {                                 \
    if (this->rmt_log_type_check(FLAGS, t_))  {                            \
      this->rmt_log_va(t_, FLAGS,__VA_ARGS__);                                  \
    }                                                                   \
  } while(0);
#define RMT_TYPE_LOG_STRING(t_, FLAGS,...) do {                                 \
    if (this->rmt_log_type_check(FLAGS, t_))  {                            \
      this->rmt_log_va(t_, FLAGS,__VA_ARGS__);                                  \
    }                                                                   \
  } while(0);


namespace MODEL_CHIP_NAMESPACE {

class RmtObjectManager;
class Mau;
class Phv;
class Packet;
class MauStatefulAlu;
class MauLookupResult;
class RmtLogger {
public:
    static constexpr const char     *indent1 = "    ";
    static constexpr const char     *indent2 = "        ";
    static constexpr const char     *indent3 = "            ";
    static constexpr const char     *indent4 = "                ";
    static constexpr const char     *indent5 = "                    ";

    RmtLogger(RmtObjectManager *om, int type_index);
    virtual ~RmtLogger();

    void set_object_manager(RmtObjectManager *om);

    /**
     * Install a file ptr to which rmt log messages will be printed.
     * @param log_file A file ptr; if nullptr then subsequent log messages will
     *     be printed to stdout.
     */
    void set_log_file(int pipe, FILE *file_ptr);
    void set_log_file(FILE *file_ptr);
    FILE* get_log_file(int pipe) const;
    FILE* get_log_file() const;

    bool is_valid_log_type(int log_type) const;
    bool rmt_log_check(uint64_t flag) const;
    bool rmt_log_type_check(uint64_t flag, int log_type) const;
    void rmt_log(uint64_t flag, const char* fmt, va_list args) const;
    void rmt_log_va(uint64_t flag, const char* fmt, ...) const;

    void rmt_log(int log_type, uint64_t flag, const char* fmt, va_list args) const;
    void rmt_log_va(int log_type, uint64_t flag, const char* fmt, ...) const;

    uint64_t log_flags() const;
    uint64_t log_type_flags(int log_type) const;
    void update_log_type_flags(int log_type, uint64_t remove_levels, uint64_t add_levels);

    void rmt_log_P4_table_key(Phv *match_phv, int table_index, bool egress);
    const std::string rmt_act_instr_to_name(int table_index, uint32_t act_instr_addr);
    void rmt_log_pov(bool egress, Phv *phv);
    const std::string rmt_log_parser_state(int parser_id, bool egress, int state);
    /**
     * Log the given packet's content, up to a given length.
     * @param pkt A packet
     * @param len Number of bytes of the packet to log; if a negative value is
     * given the logged length defaults to g_pkt_log_len (which is 64 by
     * default)
     */
    void rmt_log_packet(Packet *pkt, int len=-1);
    void rmt_log_p4_action_info(int table_index, std::string action_name,
                                Phv *iphv/* input phv */,
                                Phv *ophv/* result phv */,
                                std::list<std::pair<int, uint32_t>>& salu_log_strs,
                                MauLookupResult& result);
    int rmt_log_get_salu_row(int t, std::string action_name);

    // Allow sub-classes to override this func so they can
    // update the log flags on associated objects
    virtual void set_log_flags(uint64_t f);
    virtual void set_log_type_flags(uint64_t f, int log_type);

    void refresh_log_flags(void);
    bool pipe_is_valid(int pipe_id) const;
    bool stage_is_valid(int stage_id) const;

    int t_index() { return type_index_; }

    // Sub-classes must define these funcs so they can
    // declare what their indexes are
    virtual int  pipe_index() const = 0;
    virtual int  s_index() const = 0;
    virtual int  rt_index() const = 0;
    virtual int  c_index() const = 0;
    virtual Mau *mau() const { return NULL; }

private:
  uint64_t filter_log_type_flags(const uint64_t flag, const int log_type) const;
  bool is_log_routable(uint64_t flag) const;
  std::string rmt_sprint_indent(uint64_t flag) const;
  std::string rmt_sprint_model_prefix() const;
  std::string rmt_sprint_p4_prefix() const;
  const std::string
  rmt_log_p4_salu_pretty_print_operand(std::string& operand,
                                       uint32_t width,
                                       uint32_t ram_lo,
                                       uint32_t ram_hi,
                                       uint32_t phv_lo,
                                       uint32_t phv_hi);
  const std::string
  rmt_log_p4_salu_pretty_print_predicate_operand(std::string& operand,
                                                 uint32_t condition_hi,
                                                 uint32_t condition_lo);
  const std::list<std::string>
  rmt_log_p4_salu_pretty_print_condition(int table_index,
                                         std::string action_name,
                                         uint32_t operation_width,
                                         uint32_t cond_input_ram_hi,
                                         uint32_t cond_input_ram_lo,
                                         uint32_t cond_input_phv_hi,
                                         uint32_t cond_input_phv_lo,
                                         uint32_t cond_output_hi,
                                         uint32_t cond_output_lo);
  const std::list<std::string>
  rmt_log_p4_salu_pretty_print_update(int table_index,
                                      std::string action_name,
                                      uint32_t width,
                                      uint32_t register_hi,
                                      uint32_t register_lo,
                                      uint32_t phv_hi,
                                      uint32_t phv_lo,
                                      uint32_t condition_hi,
                                      uint32_t condition_lo,
                                      uint32_t update_hi_1_pred,
                                      uint32_t update_hi_2_pred,
                                      uint32_t update_hi_1_result,
                                      uint32_t update_hi_2_result,
                                      uint32_t update_lo_1_pred,
                                      uint32_t update_lo_2_pred,
                                      uint32_t update_lo_1_result,
                                      uint32_t update_lo_2_result);
  const std::list<std::string>
  rmt_log_p4_salu_pretty_print_output(int table_index,
                                      std::string action_name,
                                      uint32_t width,
                                      uint32_t cond_output_hi,
                                      uint32_t cond_output_lo,
                                      uint32_t output_pred,
                                      uint32_t output_result,
                                      uint32_t register_index,
                                      uint32_t ram_hi,
                                      uint32_t ram_lo,
                                      uint32_t new_ram_hi,
                                      uint32_t new_ram_lo);
protected:  // allow unit test access
  std::list<std::string>
  rmt_log_p4_salu_pretty_print(int table_index,
                               std::string action_name,
                               std::list<std::pair<int, uint32_t>>& salu_log_value_list);
 private:
  void rmt_sprint_and_truncate(char *buf, const int max_msg_size, const char* fmt, va_list args) const;
  int rmt_print_log(std::string) const;
  void rmt_route_log(uint64_t flag, std::string) const;
  RmtObjectManager *om_;
  uint64_t          log_type_flags_[MAX_RMT_LOG_TYPE];
  int               type_index_;
  FILE             *log_file_ptr_[RmtDefs::kPipesMax];
};

/**
 * Provides concrete implementations of RmtLogger virtual methods
 */
class DefaultLogger : public RmtLogger {
 public:
  DefaultLogger(RmtObjectManager *om, int type_index);
  virtual ~DefaultLogger();
  int pipe_index() const final;
  int s_index()    const final;
  int rt_index()   const final;
  int c_index()    const final;
};

}
#endif /* __cplusplus */

#endif // _SHARED_RMT_LOG_
