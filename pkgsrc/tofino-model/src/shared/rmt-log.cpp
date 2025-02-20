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

#include <mau.h>
#include <cinttypes>
#include <iomanip>

#include <common/rmt-assert.h>
#include <model_core/log-buffer.h>
#include <rmt-defs.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <p4-name-lookup.h>


extern uint32_t g_pkt_log_len;

namespace MODEL_CHIP_NAMESPACE {

inline const std::string BoolToUpperStr(bool val){
  return val ? "TRUE" : "FALSE";
}

inline const std::string Uint32ToHexStr(uint32_t val, int width=-1) {
  std::stringstream stream;
  if (width > 0) stream << std::setw(width);
  stream << "0x" << std::setfill('0') << &std::hex << val;
  return stream.str();
}

std::string rmt_get_pkt_id_sig(Packet *packet, bool use_sig)
{
  std::string pkt_sig;
  if (use_sig) {
    pkt_sig = packet->pkt_sig_get();
    if (pkt_sig.empty()) pkt_sig = "-";
    // truncate to (MAX_LOG_PKT_SIG_LEN - 1) before appending ":"
    else pkt_sig = pkt_sig.substr(0, MAX_LOG_PKT_SIG_LEN - 1);
    pkt_sig += ":";
  } else {
    pkt_sig = model_core::LogBuffer((2 * MAX_LOG_PKT_SIG_LEN) + 2,
                                    "0x%" PRIx64 ":", packet->pkt_id()).GetBuf();
  }
  return pkt_sig;
}


RmtLogger::RmtLogger(RmtObjectManager *om, int type_index_)
    : om_(om), type_index_(type_index_) {

  for (int t = 0; t < MAX_RMT_LOG_TYPE; t++) {
      log_type_flags_[t] = UINT64_C(0);
  }
  for (int p = 0; p < RmtDefs::kPipesMax; p++) {
    log_file_ptr_[p] = nullptr;
  }
  // Enable ERROR, WARN, SUMMARY and INFO by default
  log_type_flags_[RMT_LOG_TYPE_P4] = RmtDebug::kRmtDebugBasicMask;
  log_type_flags_[RMT_LOG_TYPE_PKT] = RmtDebug::kRmtDebugBasicMask;
  if (om) {
    log_type_flags_[RMT_LOG_TYPE_MODEL] = om->register_object(this);
  }
}

RmtLogger::~RmtLogger() {
  set_object_manager(nullptr);
}

void RmtLogger::set_object_manager(RmtObjectManager *om) {
  if (om_) {
    // unregister from existing om
    om_->unregister_object(this);
  }
  om_ = om;
  if (om_) {
    // register with new om
    log_type_flags_[RMT_LOG_TYPE_MODEL] = om_->register_object(this);
  }
}

void RmtLogger::refresh_log_flags(void) {
  if (om_) {
    log_type_flags_[RMT_LOG_TYPE_MODEL] = om_->log_flags(this);
  }
}

bool RmtLogger::pipe_is_valid(int pipe_id) const {
  return ((pipe_id >= 0) && (pipe_id < RmtDefs::kPipesMax));
}
bool RmtLogger::stage_is_valid(int stage_id) const {
  return ((stage_id >= 0) && (stage_id < RmtDefs::kStagesMax));
}

void RmtLogger::set_log_file(int pipe, FILE *file_ptr) {
  if (pipe_is_valid(pipe)) log_file_ptr_[pipe] = file_ptr;
}
void RmtLogger::set_log_file(FILE *file_ptr) {
  // Install a log destination that will be used instead of stdout
  // for all 'printf' logging. Note: file_ptr may be nullptr, in
  // which case logging goes to stdout.
  for (int p = 0; p < RmtDefs::kPipesMax; p++) {
    set_log_file(p, file_ptr);
  }
}

FILE* RmtLogger::get_log_file(int pipe) const { return pipe_is_valid(pipe) ?log_file_ptr_[pipe] :nullptr; }
FILE* RmtLogger::get_log_file()         const { return get_log_file(0); }

int RmtLogger::rmt_print_log(std::string msg) const {
  // All 'printf' logging should end up here.
  // Order of precedence for print destination is:
  //     this->log_file_ptr_[p] (if set), om_->log_file_ptr_[p] (if set), stdout
  int p = pipe_index();
  FILE* log_file = get_log_file(p);
  if ((nullptr == log_file) && (nullptr != om_)) log_file = om_->get_log_file(p);
  if (nullptr == log_file) log_file = stdout;
  int ret = fprintf(log_file, "%s", msg.c_str());
  return ret;
}

bool RmtLogger::is_valid_log_type(int log_type) const {
  return (log_type >= 0) && (log_type < MAX_RMT_LOG_TYPE);
}

/**
 *   Returns given log flags masked by enabled log flags.
 */
uint64_t RmtLogger::filter_log_type_flags(const uint64_t flag,
                                          const int log_type) const {
  // NB. kRmtDebugForce is considered to be always enabled
  return (log_type_flags_[log_type] | RmtDebug::kRmtDebugForce) & flag;
}

bool RmtLogger::rmt_log_check(uint64_t flag) const {
  // Keep track of how many error/warning logs there have been
  if (mau() != NULL) {
    if ((flag & RmtDebug::kRmtDebugError)  != UINT64_C(0)) mau()->mau_info_incr(MAU_ERRORS);
    if ((flag & RmtDebug::kRmtDebugError2) != UINT64_C(0)) mau()->mau_info_incr(MAU_ERRORS_2);
    if ((flag & RmtDebug::kRmtDebugWarn)   != UINT64_C(0)) mau()->mau_info_incr(MAU_WARNINGS);
  }
  return rmt_log_type_check(flag, RMT_LOG_TYPE_MODEL);
}

bool RmtLogger::rmt_log_type_check(uint64_t flag, int log_type) const {
  uint64_t filtered_flags = filter_log_type_flags(flag, log_type);
  return filtered_flags != UINT64_C(0);
}

/**
 * Prints a formatted message to a given buffer. If necessary, the message is
 * truncated and terminated with a new line and null character.
 * @param buf A char buffer that should be at least max_msg_size in length.
 * @param max_msg_size The maximum length of message that should be written to the buffer.
 * @param fmt A format string.
 * @param args List of args to be used with the format string.
 */
void RmtLogger::rmt_sprint_and_truncate(char *buf,
                                        const int max_msg_size,
                                        const char* fmt,
                                        va_list args) const {
  // Ensure we always have a newline and a null
  int msg_size = vsnprintf(buf,max_msg_size,fmt,args);
  if (msg_size <= 0) {
    buf[0] = '\n';
    buf[1] = 0;
  } else if (msg_size >= max_msg_size-1) {
    buf[max_msg_size-2] = '\n';
    buf[max_msg_size-1] = 0;
  } else if (buf[msg_size-1] != '\n') {
    buf[msg_size] = '\n';
    buf[msg_size+1] = 0;
  }
}

/**
 * Forward a formatted log message to a logging function.
 * @param flag Log level flag
 * @param msg the formatted message.
 */
void RmtLogger::rmt_route_log(uint64_t flag, std::string msg) const {
  // If log-level flag set, any installed log_fn takes precedence
  if ((flag & RmtDebug::kRmtDebugBasicMask) && om_ && om_->log_fn()) {
    (om_->log_fn())(om_->chip_index(), pipe_index(), msg.c_str());
  } else if (RmtDebug::is_rmt_debug_enabled()) {
    rmt_print_log(msg);
  }  // else nothing logged
}

/**
 * Test if matched_flags will result in message being routed to a logging
 * function.
 * @param matched_flags Masked log flags
 * @return
 */
bool RmtLogger::is_log_routable(uint64_t masked_flags) const {
  // if rmt debug logging is enabled then any matched_flag is sufficient
  if ((masked_flags != UINT64_C(0)) && RmtDebug::is_rmt_debug_enabled()) return true;
  // if a log_fn is installed then a basic log level matched_flag must be set
  if ((masked_flags & RmtDebug::kRmtDebugBasicMask) && om_ && om_->log_fn()) return true;
  return false;
}

std::string RmtLogger::rmt_sprint_indent(uint64_t flag) const {
  // Add indentation based on level
  if ((flag & RmtDebug::kRmtDebugVerbose) != 0) {
    return std::string(indent1);
  } else if ((flag & RmtDebug::kRmtDebugDebug) != 0) {
    return std::string(indent2);
  } else if ((flag & RmtDebug::kRmtDebugTrace) != 0) {
    return std::string(indent3);
  }
  return "";
}

std::string RmtLogger::rmt_sprint_model_prefix() const {
  std::string prefix;
  const char *log_pfx = (om_ == nullptr) ? nullptr : om_->log_prefix();
  if (nullptr != log_pfx) {
    prefix += log_pfx;
    if (*log_pfx != '\0') {
      prefix += " ";
    }
  }
  int depth = RmtTypes::PrintDepth[ type_index_ ];
  if (depth >= 1 && depth <= 4) {
    int ci = (om_ == nullptr) ? -1 : om_->chip_index();
    prefix += ("<" + std::to_string(ci) + "," + std::to_string(pipe_index()));
    if (depth >= 2) prefix += ( "," + std::to_string(s_index()));
    if (depth >= 3) prefix += ( "," + std::to_string(rt_index()));
    if (depth == 4) prefix += ( "," + std::to_string(c_index()));
    prefix += "> ";
  }
  return prefix;
}

std::string RmtLogger::rmt_sprint_p4_prefix() const {
  std::string prefix = ":";
  if (rmt_thd_info.ig_packet) {
      prefix += rmt_get_pkt_id_sig(rmt_thd_info.ig_packet,
                                      om_->log_use_pkt_sig());
  } else {
      prefix += "-:";
  }
  if (rmt_thd_info.eg_packet) {
      prefix += rmt_get_pkt_id_sig(rmt_thd_info.eg_packet,
                                      om_->log_use_pkt_sig());
  } else {
      prefix += "-:";
  }

  prefix += "<" + std::to_string(om_->chip_index()) + ",";
  if (pipe_is_valid(pipe_index())) {
      prefix += std::to_string(pipe_index()) + ",";
  } else {
      prefix += "-,";
  }
  if (stage_is_valid(s_index())) {
      prefix += std::to_string(s_index()) + ">:";
  } else {
      prefix += "->:";
  }
  return prefix;
}

// shim for backwards compatibility
void RmtLogger::rmt_log(uint64_t flag, const char* fmt, va_list args) const {
  rmt_log(RMT_LOG_TYPE_MODEL, flag, fmt, args);
}

/**
 * Handle logging.
 * @param log_type The type of logging e.g. model or p4. Different types may
 *                 result in different log formats.
 * @param flag Log flag
 * @param fmt A format string
 * @param args A list of args for the format string
 */
void RmtLogger::rmt_log(int log_type, uint64_t flag, const char* fmt, va_list args) const {
  // check of flags should have already been done, but repeat anyway
  if (!is_valid_log_type(log_type)) return;
  flag = filter_log_type_flags(flag, log_type);
  if (!is_log_routable(flag)) return;

  constexpr int extra_len = RmtObjectManager::kLogPrefixMaxLen + 1 + 64;
  constexpr int kMessageSize = 768;
  constexpr int kBufferSize = extra_len + 16 + kMessageSize;
  char buf[kBufferSize];
  std::string msg;
  switch (log_type) {
    case RMT_LOG_TYPE_MODEL: {
      msg += rmt_sprint_model_prefix();
      msg += RmtDebug::get_debug_level_max_str(flag);
      msg += " ";
      rmt_sprint_and_truncate(buf, kMessageSize, fmt, args);
      msg += buf;
      break;
    }
    default: {
      msg += rmt_sprint_indent(flag);
      msg += rmt_sprint_p4_prefix();
      rmt_sprint_and_truncate(buf, kBufferSize - 1 - msg.size(), fmt, args);
      msg += buf;
      break;
    }
  }
  rmt_route_log(flag, msg);
}

void RmtLogger::rmt_log_va(uint64_t flag, const char* fmt, ...) const {
  va_list args;
  va_start(args,fmt);
  rmt_log(flag, fmt, args );
  va_end(args);
}

void RmtLogger::rmt_log_va(int log_type, uint64_t flag, const char* fmt, ...) const {
  va_list args;
  va_start(args,fmt);
  rmt_log(log_type, flag, fmt, args );
  va_end(args);
}

void RmtLogger::set_log_flags(uint64_t f) {
  set_log_type_flags(f, RMT_LOG_TYPE_MODEL);
}

void RmtLogger::set_log_type_flags(uint64_t f, int log_type) {
  log_type_flags_[log_type] = f;
}

uint64_t RmtLogger::log_flags() const {
  return log_type_flags_[RMT_LOG_TYPE_MODEL];
}

uint64_t RmtLogger::log_type_flags(int log_type) const {
  return (is_valid_log_type(log_type) ? log_type_flags_[log_type] : UINT64_C(0));
}

void RmtLogger::update_log_type_flags(int log_type, uint64_t remove_levels, uint64_t add_levels) {
  if (is_valid_log_type(log_type)) {
    // remove before add
    log_type_flags_[log_type] &= ~remove_levels;
    log_type_flags_[log_type] |= add_levels;
  }
}

void RmtLogger::rmt_log_P4_table_key(Phv *match_phv, int table_index, bool egress)
{
    auto &p4_name_lookup = om_->p4_name_lookup(pipe_index());
    const auto match_fields = p4_name_lookup.GetMatchFields(s_index(),
                                                            table_index,
                                                            *match_phv,
                                                            egress);
    // log the messges at INFO level
    RMT_P4_LOG_VERBOSE_STRS(match_fields);
}

const std::string
RmtLogger::rmt_act_instr_to_name(int table_index,
                                 uint32_t act_instr_addr)
{
    auto &p4_name_lookup = om_->p4_name_lookup(pipe_index());
    return p4_name_lookup.GetActionName(s_index(), table_index,
                                        act_instr_addr);
}

void RmtLogger::rmt_log_pov(bool egress, Phv *phv)
{
    auto &p4_name_lookup = om_->p4_name_lookup(pipe_index());
    auto valid_header_names = p4_name_lookup.GetValidHeaderNames(egress, *phv);
    // log the messges at INFO level
    RMT_P4_LOG_VERBOSE_STRS(valid_header_names);
}

const std::string RmtLogger::rmt_log_p4_salu_pretty_print_operand(std::string& operand,
                                                uint32_t width,
                                                uint32_t ram_lo,
                                                uint32_t ram_hi,
                                                uint32_t phv_lo,
                                                uint32_t phv_hi) {
  model_core::LogBuffer log_buf(1024);
  if (operand == "singlebitmode") {
    // singlebit mode
    log_buf.Append("  %s: 0x%x", "SingleBitOperation Input Value", ram_lo);
  } else if (operand == "constant") {
    log_buf.Append("  %s", "Constant Input Value");
  } else if (operand == "register_lo") {
    log_buf.Append("  %s: 0x%08x", operand.c_str(), ram_lo);
  } else if (operand == "register_hi") {
    log_buf.Append("  %s: 0x%08x", operand.c_str(), ram_hi);
  } else {
    //conditin has p4-field
    if (width > 32) {
      uint64_t value = ((uint64_t)phv_hi << 32u) | phv_lo;
      log_buf.Append("  %s: 0x%016" PRIx64, operand.c_str(), value);
    } else {
      log_buf.Append("  %s: 0x%08x", operand.c_str(), phv_lo);
    }
  }
  return log_buf.GetBuf();
}

const std::string
RmtLogger::rmt_log_p4_salu_pretty_print_predicate_operand(std::string& operand,
                                                          uint32_t condition_hi,
                                                          uint32_t condition_lo)
{
  std::string str;
  if (operand == "condition_hi") {
    str = " " + operand + ": " + BoolToUpperStr(condition_hi);
  } else if (operand == "condition_lo") {
    str = " " + operand + ": " + BoolToUpperStr(condition_lo);
  }
  return str;
}

const std::list<std::string>
RmtLogger::rmt_log_p4_salu_pretty_print_condition(int table_index,
                                                  std::string action_name,
                                                  uint32_t width,
                                                  uint32_t cond_input_ram_hi,
                                                  uint32_t cond_input_ram_lo,
                                                  uint32_t cond_input_phv_hi,
                                                  uint32_t cond_input_phv_lo,
                                                  uint32_t cond_output_hi,
                                                  uint32_t cond_output_lo)
{
  std::list<std::string> strlist;
  auto &p4_name_lookup = om_->p4_name_lookup(pipe_index());

  std::string cond_lo_op1 =
    p4_name_lookup.GetSaluConditionOperand(s_index(),
                                           table_index, action_name, 1, 1);
  std::string cond_lo_op2 =
    p4_name_lookup.GetSaluConditionOperand(s_index(),
                                           table_index, action_name, 1, 2);
  std::string cond_hi_op1 =
    p4_name_lookup.GetSaluConditionOperand(s_index(),
                                           table_index, action_name, 0, 1);
  std::string cond_hi_op2 =
    p4_name_lookup.GetSaluConditionOperand(s_index(),
                                           table_index, action_name, 0, 2);

  strlist.emplace_back("\t--- SALU Condition ---");
  if (cond_lo_op1.empty()
      && cond_hi_op1.empty()) {
    strlist.emplace_back("\t  Not supplied by program.");
    strlist.emplace_back("\t    SALU ConditionLo: " + BoolToUpperStr(cond_output_lo));
    strlist.emplace_back("\t    SALU ConditionHi: " + BoolToUpperStr(cond_output_hi));
    return strlist;
  } else {
    strlist.emplace_back("\t  --- SALU Condition Lo ---");
    strlist.emplace_back("\t    ConditionResult: " + BoolToUpperStr(cond_output_lo));
    strlist.emplace_back("\t    Condition Lo result computed using fields:");
    strlist.emplace_back("\t      " +
                      rmt_log_p4_salu_pretty_print_operand(cond_lo_op1,
                                                           width,
                                                           cond_input_ram_lo,
                                                           cond_input_ram_hi,
                                                           cond_input_phv_lo,
                                                           cond_input_phv_hi));
    if (!cond_lo_op2.empty()) {
      strlist.emplace_back("\t      " +
                        rmt_log_p4_salu_pretty_print_operand(cond_lo_op2,
                                                             width,
                                                             cond_input_ram_lo,
                                                             cond_input_ram_hi,
                                                             cond_input_phv_lo,
                                                             cond_input_phv_hi));
    }
  }

  if (!cond_hi_op1.empty()) {
    strlist.emplace_back("\t  --- SALU Condition Hi ---");
    strlist.emplace_back("\t    ConditionResult: " + BoolToUpperStr(cond_output_hi));
    strlist.emplace_back("\t    Condition Hi result computed using fields:");
    strlist.emplace_back("\t      " +
                      rmt_log_p4_salu_pretty_print_operand(cond_hi_op1,
                                                           width,
                                                           cond_input_ram_lo,
                                                           cond_input_ram_hi,
                                                           cond_input_phv_lo,
                                                           cond_input_phv_hi));
    if (!cond_hi_op2.empty()) {
      strlist.emplace_back("\t      " +
                        rmt_log_p4_salu_pretty_print_operand(cond_hi_op2,
                                                             width,
                                                             cond_input_ram_lo,
                                                             cond_input_ram_hi,
                                                             cond_input_phv_lo,
                                                             cond_input_phv_hi));
    }
  }

  return strlist;
}

const std::list<std::string>
RmtLogger::rmt_log_p4_salu_pretty_print_update(int table_index,
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
                                               uint32_t update_lo_2_result) {
  std::list<std::string> strlist;

  auto &p4_name_lookup = om_->p4_name_lookup(pipe_index());

  std::string update_lo_1_value_op1 =
    p4_name_lookup.GetSaluUpdateValueOperand(s_index(),
                                            table_index, action_name,
                                            1 /*update lo / hi; 1 implies lo*/,
                                            1 /*Alu 1 / 2 within hi/lo */,
                                            1 /* operand 1 or 2 */);
  std::string update_lo_1_value_op2 =
    p4_name_lookup.GetSaluUpdateValueOperand(s_index(),
                                        table_index, action_name, 1, 1, 2);
  std::string update_lo_2_value_op1 =
    p4_name_lookup.GetSaluUpdateValueOperand(s_index(),
                                        table_index, action_name, 1, 2, 1);
  std::string update_lo_2_value_op2 =
    p4_name_lookup.GetSaluUpdateValueOperand(s_index(),
                                        table_index, action_name, 1, 2, 2);
  std::string update_hi_1_value_op1 =
    p4_name_lookup.GetSaluUpdateValueOperand(s_index(),
                                        table_index, action_name, 0, 1, 1);
  std::string update_hi_1_value_op2 =
    p4_name_lookup.GetSaluUpdateValueOperand(s_index(),
                                        table_index, action_name, 0, 1, 2);
  std::string update_hi_2_value_op1 =
    p4_name_lookup.GetSaluUpdateValueOperand(s_index(),
                                        table_index, action_name, 0, 2, 1);
  std::string update_hi_2_value_op2 =
    p4_name_lookup.GetSaluUpdateValueOperand(s_index(),
                                        table_index, action_name, 0, 2, 2);

  strlist.emplace_back("\t--- SALU Update ---");
  if (update_lo_1_value_op1.empty()
      &&update_lo_2_value_op1.empty()
      &&update_hi_1_value_op1.empty()
      && update_hi_2_value_op1.empty()) {
    strlist.emplace_back("\t  None");
    return strlist;
  }

  std::string update_lo_1_pred_op1 =
    p4_name_lookup.GetSaluUpdatePredicateOperand(s_index(),
                                        table_index, action_name, 1, 1, 1);
  std::string update_lo_1_pred_op2 =
    p4_name_lookup.GetSaluUpdatePredicateOperand(s_index(),
                                        table_index, action_name, 1, 1, 2);

  std::string update_lo_2_pred_op1 =
    p4_name_lookup.GetSaluUpdatePredicateOperand(s_index(),
                                        table_index, action_name, 1, 2, 1);
  std::string update_lo_2_pred_op2 =
    p4_name_lookup.GetSaluUpdatePredicateOperand(s_index(),
                                        table_index, action_name, 1, 2, 2);

  std::string update_hi_1_pred_op1 =
    p4_name_lookup.GetSaluUpdatePredicateOperand(s_index(),
                                        table_index, action_name, 0, 1, 1);
  std::string update_hi_1_pred_op2 =
    p4_name_lookup.GetSaluUpdatePredicateOperand(s_index(),
                                        table_index, action_name, 0, 1, 2);

  std::string update_hi_2_pred_op1 =
    p4_name_lookup.GetSaluUpdatePredicateOperand(s_index(),
                                        table_index, action_name, 0, 2, 1);
  std::string update_hi_2_pred_op2 =
    p4_name_lookup.GetSaluUpdatePredicateOperand(s_index(),
                                        table_index, action_name, 0, 2, 2);

  if (!update_lo_1_value_op1.empty()) {
    strlist.emplace_back("\t  --- SALU Update Lo 1 ---");
    if (!update_lo_1_pred_op1.empty()) {
      strlist.emplace_back("\t    Update Lo 1 PredicateResult: " + BoolToUpperStr(update_lo_1_pred));
      strlist.emplace_back("\t    Update Lo 1 predicate result computed using fields:");
      strlist.emplace_back("\t      " +
        rmt_log_p4_salu_pretty_print_predicate_operand(update_lo_1_pred_op1,
                                                       condition_hi,
                                                       condition_lo));
      if (!update_lo_1_pred_op2.empty()) {
        strlist.emplace_back("\t      " +
          rmt_log_p4_salu_pretty_print_predicate_operand(update_lo_1_pred_op2,
                                                         condition_hi,
                                                         condition_lo));
      }
    } else {
      strlist.emplace_back("\t    Update Lo 1 predicate not supplied by program");
      strlist.emplace_back("\t    Update Lo 1 PredicateResult: " + BoolToUpperStr(update_lo_1_pred));
    }
    strlist.emplace_back("\t    Update Lo 1 expression result: " + Uint32ToHexStr(update_lo_1_result, 8));
    strlist.emplace_back("\t    Update Lo 1 expression result computed using fields:");
    strlist.emplace_back("\t      " +
                    rmt_log_p4_salu_pretty_print_operand(update_lo_1_value_op1,
                                                         width,
                                                         register_lo,
                                                         register_hi,
                                                         phv_lo,
                                                         phv_hi));
    if (!update_lo_1_value_op2.empty()) {
      strlist.emplace_back("\t      " +
                        rmt_log_p4_salu_pretty_print_operand(update_lo_1_value_op2,
                                                             width,
                                                             register_lo,
                                                             register_hi,
                                                             phv_lo,
                                                             phv_hi));
    }
  }
  if (!update_lo_2_value_op1.empty()) {
    strlist.emplace_back("\t  --- SALU Update Lo 2 ---");
    if (!update_lo_2_pred_op1.empty()) {
      strlist.emplace_back("\t    Update Lo 2 PredicateResult: " + BoolToUpperStr(update_lo_2_pred));
      strlist.emplace_back("\t    Update Lo 2 predicate result computed using fields:");
      strlist.emplace_back("\t      " +
        rmt_log_p4_salu_pretty_print_predicate_operand(update_lo_2_pred_op1,
                                                       condition_hi,
                                                       condition_lo));
      if (!update_lo_2_pred_op2.empty()) {
        strlist.emplace_back("\t      " +
          rmt_log_p4_salu_pretty_print_predicate_operand(update_lo_2_pred_op2,
                                                         condition_hi,
                                                         condition_lo));
      }
    } else {
      strlist.emplace_back("\t    Update Lo 2 predicate not supplied by program");
      strlist.emplace_back("\t    Update Lo 2 PredicateResult: " + BoolToUpperStr(update_lo_2_pred));
    }
    strlist.emplace_back("\t    Update Lo 2 expression result: " + Uint32ToHexStr(update_lo_2_result, 8));
    strlist.emplace_back("\t    Update Lo 2 expression result computed using fields:");
    strlist.emplace_back("\t      " +
                    rmt_log_p4_salu_pretty_print_operand(update_lo_2_value_op1,
                                                         width,
                                                         register_lo,
                                                         register_hi,
                                                         phv_lo,
                                                         phv_hi));
    if (!update_lo_2_value_op2.empty()) {
      strlist.emplace_back("\t      " +
                        rmt_log_p4_salu_pretty_print_operand(update_lo_2_value_op2,
                                                             width,
                                                             register_lo,
                                                             register_hi,
                                                             phv_lo,
                                                             phv_hi));
    }
  }
  if (!update_hi_1_value_op1.empty()) {
    strlist.emplace_back("\t  --- SALU Update Hi 1 ---");
    if (!update_hi_1_pred_op1.empty()) {
      strlist.emplace_back("\t    Update Hi 1 PredicateResult: " + BoolToUpperStr(update_hi_1_pred));
      strlist.emplace_back("\t    Update Hi 1 predicate result computed using fields:");
      strlist.emplace_back("\t      " +
        rmt_log_p4_salu_pretty_print_predicate_operand(update_hi_1_pred_op1,
                                                       condition_hi,
                                                       condition_lo));
      if (!update_hi_1_pred_op2.empty()) {
        strlist.emplace_back("\t      " +
          rmt_log_p4_salu_pretty_print_predicate_operand(update_hi_1_pred_op2,
                                                         condition_hi,
                                                         condition_lo));
      }
    } else {
      strlist.emplace_back("\t    Update Hi 1 predicate not supplied by program");
      strlist.emplace_back("\t    Update Hi 1 PredicateResult: " + BoolToUpperStr(update_hi_1_pred));
    }
    strlist.emplace_back("\t    Update Hi 1 expression result: " + Uint32ToHexStr(update_hi_1_result, 8));
    strlist.emplace_back("\t    Update Hi 1 expression result computed using fields:");
    strlist.emplace_back("\t      " +
                    rmt_log_p4_salu_pretty_print_operand(update_hi_1_value_op1,
                                                         width,
                                                         register_lo,
                                                         register_hi,
                                                         phv_lo,
                                                         phv_hi));
    if (!update_hi_1_value_op2.empty()) {
      strlist.emplace_back("\t      " +
                        rmt_log_p4_salu_pretty_print_operand(update_hi_1_value_op2,
                                                             width,
                                                             register_lo,
                                                             register_hi,
                                                             phv_lo,
                                                             phv_hi));
    }
  }
  if (!update_hi_2_value_op1.empty()) {
    strlist.emplace_back("\t  --- SALU Update Hi 2 ---");
    if (!update_hi_2_pred_op1.empty()) {
      strlist.emplace_back("\t    Update Hi 2 PredicateResult: " + BoolToUpperStr(update_hi_2_pred));
      strlist.emplace_back("\t    Update Hi 2 predicate result computed using fields:");
      strlist.emplace_back("\t      " +
        rmt_log_p4_salu_pretty_print_predicate_operand(update_hi_2_pred_op1,
                                                       condition_hi,
                                                       condition_lo));
      if (!update_hi_2_pred_op2.empty()) {
        strlist.emplace_back("\t      " +
          rmt_log_p4_salu_pretty_print_predicate_operand(update_hi_2_pred_op2,
                                                         condition_hi,
                                                         condition_lo));
      }
    } else {
      strlist.emplace_back("\t    Update Hi 2 predicate not supplied by program");
      strlist.emplace_back("\t    Update Hi 2 PredicateResult: " + BoolToUpperStr(update_hi_2_pred));
    }
    strlist.emplace_back("\t    Update Hi 2 expression result: " + Uint32ToHexStr(update_hi_2_result, 8));
    strlist.emplace_back("\t    Update Hi 2 expression result computed using fields:");
    strlist.emplace_back("\t      " +
                    rmt_log_p4_salu_pretty_print_operand(update_hi_2_value_op1,
                                                         width,
                                                         register_lo,
                                                         register_hi,
                                                         phv_lo,
                                                         phv_hi));
    if (!update_hi_2_value_op2.empty()) {
      strlist.emplace_back("\t      " +
                        rmt_log_p4_salu_pretty_print_operand(update_hi_2_value_op2,
                                                             width,
                                                             register_lo,
                                                             register_hi,
                                                             phv_lo,
                                                             phv_hi));
    }
  }
  return strlist;
}

const std::list<std::string>
RmtLogger::rmt_log_p4_salu_pretty_print_output(int table_index,
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
                                               uint32_t new_ram_lo)
{
  std::list<std::string> strlist;
  auto &p4_name_lookup = om_->p4_name_lookup(pipe_index());

  std::string output_pred_op1 =
    p4_name_lookup.GetSaluOutputPredicateOperand(s_index(),
                                                 table_index, action_name, 1);
  std::string output_pred_op2 =
    p4_name_lookup.GetSaluOutputPredicateOperand(s_index(),
                                                 table_index, action_name, 2);
  std::string output_dest  =
    p4_name_lookup.GetSaluOutputDestinationP4Name(s_index(),
                                                  table_index, action_name);

  strlist.emplace_back("\t--- SALU Output ---");
  if (output_dest.empty()) {
      strlist.emplace_back("\t    Output destination supplied by program");
  } else {
    if (!output_pred_op1.empty()) {
      strlist.emplace_back("\t    Output PredicateResult: " + BoolToUpperStr(output_pred));
      strlist.emplace_back("\t    Output predicate result computed using fields:");
      strlist.emplace_back("\t      " +
      rmt_log_p4_salu_pretty_print_predicate_operand(output_pred_op1,
                                                     cond_output_hi,
                                                     cond_output_lo));
      if (!output_pred_op2.empty()) {
        strlist.emplace_back("\t      " +
        rmt_log_p4_salu_pretty_print_predicate_operand(output_pred_op2,
                                                       cond_output_hi,
                                                       cond_output_lo));
      }
    } else {
      strlist.emplace_back("\t    Output predicate not supplied by program");
      strlist.emplace_back("\t    Output PredicateResult: " + BoolToUpperStr(output_pred));
    }
    strlist.emplace_back(model_core::LogBuffer(1024, "\t    Output Destination Field: %s = 0x%x", output_dest.c_str(), output_result).GetBuf());
  }
  strlist.emplace_back("\t---  SALU Register ---");
  strlist.emplace_back(model_core::LogBuffer(1024, "\t   Register Index: 0x%x", register_index).GetBuf());
  if (width > 32) {
    uint64_t value = ((uint64_t)ram_hi << 32u) | ram_lo;
    strlist.emplace_back(model_core::LogBuffer(1024, "\t     Before stateful alu execution: 0x%016" PRIx64, value).GetBuf());
    value = ((uint64_t)new_ram_hi << 32u) | new_ram_lo;
    strlist.emplace_back(model_core::LogBuffer(1024, "\t     After stateful alu execution: 0x%016" PRIx64, value).GetBuf());
  } else {
    strlist.emplace_back(model_core::LogBuffer(1024, "\t     Before stateful alu execution: 0x%08x", ram_lo).GetBuf());
    strlist.emplace_back(model_core::LogBuffer(1024, "\t     After stateful alu execution: 0x%08x", new_ram_lo).GetBuf());
  }
  return strlist;
}

std::list<std::string>
RmtLogger::rmt_log_p4_salu_pretty_print(int table_index, std::string action_name,
                                        std::list<std::pair<int, uint32_t>>& salu_log_value_list)
{
  std::list<std::string> strlist;
  uint32_t width = 0, register_index = 0, register_lo = 0, register_hi = 0;
  uint32_t phv_lo = 0, phv_hi = 0;
  uint32_t cond_input_ram_hi = 0, cond_input_ram_lo = 0, cond_input_phv_hi = 0;
  uint32_t cond_input_phv_lo = 0, cond_output_hi = 0, cond_output_lo = 0;
  uint32_t update_lo_1_pred = 0, update_lo_2_pred = 0, update_hi_1_pred = 0;
  uint32_t update_hi_2_pred = 0, update_lo_1_result = 0, update_lo_2_result = 0;
  uint32_t update_hi_1_result= 0, update_hi_2_result = 0;
  uint32_t new_ram_hi = 0, new_ram_lo = 0, output_pred = 0, output_result = 0;

  for (auto valuepair = salu_log_value_list.begin();
       valuepair != salu_log_value_list.end(); valuepair++) {
    switch((*valuepair).first) {
      case RMT_LOG_SALU_WIDTH:
        width = (*valuepair).second;
      break;
      case RMT_LOG_SALU_INDEX:
        register_index = (*valuepair).second;
      break;
      case RMT_LOG_SALU_REGISTER_LO:
        register_lo = (*valuepair).second;
      break;
      case RMT_LOG_SALU_REGISTER_HI:
        register_hi = (*valuepair).second;
      break;
      case RMT_LOG_SALU_PHV_LO:
        phv_lo = (*valuepair).second;
      break;
      case RMT_LOG_SALU_PHV_HI:
        phv_hi = (*valuepair).second;
      break;
      case RMT_LOG_SALU_COND_INPUT_RAM_HI:
        cond_input_ram_hi = (*valuepair).second;
      break;
      case RMT_LOG_SALU_COND_INPUT_RAM_LO:
        cond_input_ram_lo = (*valuepair).second;
      break;
      case RMT_LOG_SALU_COND_INPUT_PHV_HI:
        cond_input_phv_hi = (*valuepair).second;
      break;
      case RMT_LOG_SALU_COND_INPUT_PHV_LO:
        cond_input_phv_lo = (*valuepair).second;
      break;
      case RMT_LOG_SALU_COND_OUTPUT_HI:
        cond_output_hi = (*valuepair).second;
      break;
      case RMT_LOG_SALU_COND_OUTPUT_LO:
        cond_output_lo = (*valuepair).second;
      break;
      case RMT_LOG_SALU_UPDATE_LO_1_PRED:
        update_lo_1_pred = (*valuepair).second;
      break;
      case RMT_LOG_SALU_UPDATE_LO_2_PRED:
        update_lo_2_pred = (*valuepair).second;
      break;
      case RMT_LOG_SALU_UPDATE_HI_1_PRED:
        update_hi_1_pred = (*valuepair).second;
      break;
      case RMT_LOG_SALU_UPDATE_HI_2_PRED:
        update_hi_2_pred = (*valuepair).second;
      break;
      case RMT_LOG_SALU_UPDATE_LO_1_RESULT:
        update_lo_1_result = (*valuepair).second;
      break;
      case RMT_LOG_SALU_UPDATE_LO_2_RESULT:
        update_lo_2_result = (*valuepair).second;
      break;
      case RMT_LOG_SALU_UPDATE_HI_1_RESULT:
        update_hi_1_result = (*valuepair).second;
      break;
      case RMT_LOG_SALU_UPDATE_HI_2_RESULT:
        update_hi_2_result = (*valuepair).second;
      break;
      case RMT_LOG_SALU_NEW_RAM_HI:
        new_ram_hi = (*valuepair).second;
      break;
      case RMT_LOG_SALU_NEW_RAM_LO:
        new_ram_lo = (*valuepair).second;
      break;
      case RMT_LOG_SALU_OUTPUT_PRED:
        output_pred = (*valuepair).second;
      break;
      case RMT_LOG_SALU_OUTPUT_RESULT:
        output_result = (*valuepair).second;
      break;
    }
  }
  // using name lookup json, format SALU output.
  //SALU condition_lo/hi
  auto cond_strlist =
           rmt_log_p4_salu_pretty_print_condition(table_index,
                                                  action_name,
                                                  width,
                                                  cond_input_ram_hi,
                                                  cond_input_ram_lo,
                                                  cond_input_phv_hi,
                                                  cond_input_phv_lo,
                                                  cond_output_hi,
                                                  cond_output_lo);
  strlist.insert(strlist.end(), cond_strlist.begin(), cond_strlist.end());

  // SALU Update_[hi/lo]_[1/2]_expression/predicate/result
  auto update_strlist = rmt_log_p4_salu_pretty_print_update(table_index,
                                                            action_name,
                                                            width,
                                                            register_hi,
                                                            register_lo,
                                                            phv_hi,
                                                            phv_lo,
                                                            cond_output_hi,
                                                            cond_output_lo,
                                                            update_hi_1_pred,
                                                            update_hi_2_pred,
                                                            update_hi_1_result,
                                                            update_hi_2_result,
                                                            update_lo_1_pred,
                                                            update_lo_2_pred,
                                                            update_lo_1_result,
                                                            update_lo_2_result);
  strlist.insert(strlist.end(), update_strlist.begin(), update_strlist.end());

  // SALU output predicate, destination, modified RAM/register
  auto output_strlist = rmt_log_p4_salu_pretty_print_output(table_index,
                                               action_name,
                                               width,
                                               cond_output_hi,
                                               cond_output_lo,
                                               output_pred,
                                               output_result,
                                               register_index,
                                               register_hi,
                                               register_lo,
                                               new_ram_hi,
                                               new_ram_lo);
  strlist.insert(strlist.end(), output_strlist.begin(), output_strlist.end());

  return (strlist);
}



void
RmtLogger::rmt_log_p4_action_info(int table_index, std::string action_name,
                                  Phv *iphv/* input phv */,
                                  Phv *ophv/* result phv */,
                                  std::list<std::pair<int, uint32_t>>& salu_log_value_list,
                                  MauLookupResult& result)
{
    auto &p4_name_lookup = om_->p4_name_lookup(pipe_index());
    const auto action_info = p4_name_lookup.GetActionInfo(s_index(),
                                                          table_index, action_name,
                                                          *iphv, *ophv, result);
    // log the messges at INFO level
    RMT_P4_LOG_VERBOSE_STRS(action_info);

    if (!salu_log_value_list.empty()) {
      auto strlist = rmt_log_p4_salu_pretty_print(table_index, action_name, salu_log_value_list);
      RMT_P4_LOG_VERBOSE_STRS(strlist);
      // now empty the list maintained in state structure.
      salu_log_value_list.clear();
    }
}

// If action has stateful primitive associated with it, the function
// should return the meter-alu row which is used for stateful operation.
int RmtLogger::rmt_log_get_salu_row(int t, std::string action_name)
{
  auto &p4_name_lookup = om_->p4_name_lookup(pipe_index());
  return (p4_name_lookup.GetStflTablePhysicalRow(s_index(), t, action_name));
}

const std::string RmtLogger::rmt_log_parser_state(int parser_id,
                                                  bool egress,
                                                  int state) {
    return om_->p4_name_lookup(pipe_index()).GetParserStateName(parser_id,
                                                                egress,
                                                                state);
}

void RmtLogger::rmt_log_packet(Packet *pkt, int len)
{
    // print upto 64 bytes of a packet
    uint8_t     pkt_buf[16];
    int         total_len, start, i;
    if (len < 0) len = g_pkt_log_len;


    RMT_PKT_LOG_INFO("Packet :\n");
    start = 0;
    if (len) {
        total_len = pkt->len() < len ? pkt->len() : len;
    } else {
        total_len = pkt->len();
    }

    while (total_len > 0) {
        model_core::LogBuffer print_buf(16*3 + 1);
        int print_len = total_len > 16 ? 16 : total_len;

        pkt->get_buf(pkt_buf, start, print_len);

        for (i=0; i < print_len; i++) {
            print_buf.Append("%02x ", (int)pkt_buf[i]);
        }
        RMT_PKT_LOG_VERBOSE("%s\n", print_buf.GetBuf());
        total_len -= print_len;
        start += print_len;
    }
}


// DefaultLogger

  DefaultLogger::DefaultLogger(RmtObjectManager *om, int type_index) :
    RmtLogger(om,type_index) { }
  DefaultLogger::~DefaultLogger() { }
  int DefaultLogger::pipe_index() const { return rmt_thd_info.pipe; }
  int DefaultLogger::s_index()    const { return 0; }
  int DefaultLogger::rt_index()   const { return 0; }
  int DefaultLogger::c_index()    const { return 0; }

}
