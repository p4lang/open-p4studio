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


#include <time.h>
#include <chrono>
#include <ctime>
#include <future>
#include <sstream>
#include <thread>

#ifdef BFRT_PERF_TEST
#include <gperftools/profiler.h>
#endif

#include <bf_rt_common/bf_rt_utils.hpp>
#include "bf_rt_server_impl.hpp"

namespace bfrt {
namespace server {

namespace callbacks {
namespace {

inline uint32_t getClientIdFromCookie(const uint32_t &cookie) {
  return (cookie & 0x00ffffff);
}

inline bf_dev_id_t getDeviceIdFromCookie(const uint32_t &cookie) {
  return (cookie & 0xff000000);
}

// Bitmap signatrure of valid operation cookie object in memory.
#define OP_COOKIE_FINGERPRINT 0xF146641F

class OperationCookie {
 public:
  OperationCookie(const uint32_t &client_id, const bf_rt_id_t &table_id)
      : client_id_(client_id), table_id_(table_id) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &time_created_);
    checksum_ = client_id_ ^ table_id_;
    fingerprint_ = OP_COOKIE_FINGERPRINT;
    LOG_TRACE("new cookie: client_id=%u table_id=%u (0x%0lx+%0lx)",
              client_id_,
              table_id_,
              time_created_.tv_sec,
              time_created_.tv_nsec);
  }

  OperationCookie() = delete;

  ~OperationCookie() {
    LOG_TRACE("delete %scookie: client_id=%u table_id=%u (0x%0lx+%0lx)",
              (isValid()) ? "" : "INVALID ",
              client_id_,
              table_id_,
              time_created_.tv_sec,
              time_created_.tv_nsec);
    fingerprint_ = 0;  // ensure the memory is an invalid cookie
  }

  bool isValid() const {
    return (fingerprint_ == OP_COOKIE_FINGERPRINT) &&
           (!(client_id_ ^ table_id_ ^ checksum_));
  }

  uint32_t client_id_{0};
  bf_rt_id_t table_id_{0};

  struct timespec time_created_ {
    0
  };                         // as unique id and to age if needed
  uint32_t checksum_{0};     // for client_id_ and table_id_ consistency check
  uint32_t fingerprint_{0};  // cookie typecast memory check
};
}  // anonymous namespace

void server_op_cb(const bf_rt_target_t & /*target*/, void *cookie) {
  // The table id and client id are encoded in the cookie.
  const OperationCookie *my_cookie =
      reinterpret_cast<const OperationCookie *>(cookie);
  if (nullptr == my_cookie) {
    LOG_ERROR("%s:%d invalid cookie", __func__, __LINE__);
    return;
  }
  const uint32_t client_id = my_cookie->client_id_;
  const bf_rt_id_t table_id = my_cookie->table_id_;

  // The cookie must be a correct gRPC callback cookie.
  if (!(my_cookie->isValid())) {
    LOG_WARN(
        "%s:%d ignore invalid cookie: client_id=%u table_id=%u (0x%0lx+%0lx)",
        __func__,
        __LINE__,
        client_id,
        table_id,
        my_cookie->time_created_.tv_sec,
        my_cookie->time_created_.tv_nsec);
    // Neither procced with invalid cookie nor delete it.
    return;
  }

  // Delete the cookie
  delete my_cookie;

  SetFwdConfigLockGuard read_lock(BfRtServer::getInstance().setFwdRwLockget(),
                                  false);
  if (!read_lock) {
    LOG_ERROR(" %s:%d Failed to acquire read lock", __func__, __LINE__);
    return;
  }

  // Get the connection data for this client
  std::shared_ptr<ConnectionData> connection_sp;
  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  if (!grpc_status.ok()) {
    // This is not an error condition. It might have happened that the client
    // exited immediately after issuing the sync operations commands.
    LOG_ERROR("%s:%d Unable to retrieve connection data for client with id %d",
              __func__,
              __LINE__,
              client_id);
    return;
  }
  if (connection_sp->isIndependent()) {
    connection_sp->insertTableIdInList(table_id);
    return;
  }
  const BoundProgram *bound_program = nullptr;
  grpc_status = connection_sp->boundProgramGet(&bound_program);
  if (!grpc_status.ok() || !bound_program) {
    LOG_ERROR(
        "%s:%d Unable to retrieve bound_program data for client with id %d",
        __func__,
        __LINE__,
        client_id);
    return;
  }
  const auto info = bound_program->bfRtInfoGet();
  std::vector<const BfRtTable *> table_obj_list;
  auto status = info->bfrtInfoGetTables(&table_obj_list);
  if (status != BF_SUCCESS) {
    LOG_ERROR("%s:%d Unable to retrieve table objects for client with id %d",
              __func__,
              __LINE__,
              client_id);
    return;
  }
  const BfRtTable *table_obj = nullptr;
  status = info->bfrtTableFromIdGet(table_id, &table_obj);
  if (status != BF_SUCCESS) {
    // Indicates something really bad because the table id which was encoded
    // in the client is no longer valid???
    LOG_ERROR(
        "%s:%d Unable to get table object for table id %d ,client id %d "
        "retrieved from the cookie",
        __func__,
        __LINE__,
        table_id,
        client_id);
    return;
  }

  // We found a match. Thus we need to insert this table id in the list
  // so that the grpc thread waiting on this will be unblocked
  connection_sp->insertTableIdInList(table_id);

  LOG_DBG("%s:%d Operation completion cb called for client %d table %d",
          __func__,
          __LINE__,
          client_id,
          table_id);
}

bf_status_t portstatuschg_callback(const bf_dev_id_t &dev_id,
                                   const BfRtTableKey *key,
                                   const bool &port_up,
                                   void *cookie) {
  // Retrieve the client_id from the cookie
  const auto cookie_data = reinterpret_cast<uintptr_t>(cookie);
  uint32_t client_id = getClientIdFromCookie(cookie_data);
  uint32_t device_id = getDeviceIdFromCookie(cookie_data);

  if (static_cast<uint32_t>(dev_id) != device_id) {
    LOG_ERROR(
        "%s:%d Device id (%d) retrieved from the cookie does not match the "
        "device id (%d) passed into portstatuschg_callback",
        __func__,
        __LINE__,
        device_id,
        dev_id);
    BF_RT_DBGCHK(0);
    return BF_UNEXPECTED;
  }
  // Get the connection data for this client
  std::shared_ptr<ConnectionData> connection_sp;
  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  if (!grpc_status.ok()) {
    LOG_ERROR("%s:%d Unable to retrieve connection data for client with id %d",
              __func__,
              __LINE__,
              client_id);
    return BF_UNEXPECTED;
  }

  const BoundProgram *bound_program = nullptr;
  grpc_status = connection_sp->boundProgramGet(&bound_program);
  if (!grpc_status.ok() || bound_program == nullptr) {
    LOG_ERROR(
        "%s:%d Unable to retrieve bound_program data for client with id %d",
        __func__,
        __LINE__,
        client_id);
    return BF_UNEXPECTED;
  }

  const BfRtTable *table;
  std::vector<bf_rt_id_t> field_ids;

  auto bf_status = key->tableGet(&table);
  if (bf_status != BF_SUCCESS) {
    return bf_status;
  }

  bf_status = table->keyFieldIdListGet(&field_ids);
  if (bf_status != BF_SUCCESS) {
    return bf_status;
  }

  bfrt_proto::StreamMessageResponse response;
  auto notification = response.mutable_port_status_change_notification();
  auto table_entry = notification->mutable_table_entry();
  notification->set_port_up(port_up);

  bf_rt_id_t table_id;
  bf_status = table->tableIdGet(&table_id);
  table_entry->set_table_id(table_id);
  auto table_key = table_entry->mutable_key();

  for (const auto &field_id : field_ids) {
    auto field = table_key->add_fields();
    field->set_field_id(field_id);
    KeyFieldType type;
    bf_status = table->keyFieldTypeGet(field_id, &type);
    size_t size;
    table->keyFieldSizeGet(field_id, &size);
    size = (size + 7) / 8;
    switch (type) {
      case KeyFieldType::EXACT: {
        std::vector<uint8_t> value(size);
        bf_status = key->getValue(field_id, size, &value[0]);
        auto exact = field->mutable_exact();
        exact->set_value(&value[0], size);
      } break;
      default:
        break;
    }
  }

  LOG_DBG("%s:%d Port Status Change notification: %s",
          __func__,
          __LINE__,
          notification->DebugString().c_str());
  connection_sp->sendStreamMessage(response);

  return BF_SUCCESS;
}

bf_status_t selectorupdatechg_callback(
    const std::shared_ptr<BfRtSession> session,
    const bf_rt_target_t &bf_rt_tgt,
    const void *cookie,
    const bf_rt_id_t sel_grp_id,
    const bf_rt_id_t act_mbr_id,
    const int &logical_entry_index,
    const bool &is_add) {
  // Retrieve the client_id from the cookie
  (void)session;
  (void)bf_rt_tgt;
  const auto cookie_data = reinterpret_cast<uintptr_t>(cookie);
  uint32_t client_id = getClientIdFromCookie(cookie_data);

  // Get the connection data for this client
  std::shared_ptr<ConnectionData> connection_sp;
  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  if (!grpc_status.ok()) {
    LOG_ERROR("%s:%d Unable to retrieve connection data for client with id %d",
              __func__,
              __LINE__,
              client_id);
    return BF_UNEXPECTED;
  }

  const BoundProgram *bound_program = nullptr;
  grpc_status = connection_sp->boundProgramGet(&bound_program);
  if (!grpc_status.ok() || bound_program == nullptr) {
    LOG_ERROR(
        "%s:%d Unable to retrieve bound_program data for client with id %d",
        __func__,
        __LINE__,
        client_id);
    return BF_UNEXPECTED;
  }

  bfrt_proto::StreamMessageResponse response;
  std::vector<bf_rt_id_t> field_ids;
  auto notification = response.mutable_selector_update_change_notification();

  notification->set_sel_group_id(sel_grp_id);
  notification->set_act_member_id(act_mbr_id);
  notification->set_logical_entry_index(logical_entry_index);
  notification->set_is_add(is_add);

  LOG_DBG("%s:%d Selector Update Change notification: %s",
          __func__,
          __LINE__,
          notification->DebugString().c_str());
  connection_sp->sendStreamMessage(response);

  return BF_SUCCESS;
}

static bf_status_t idletime_notification_send(
    const bf_rt_target_t &target,
    const BfRtTableKey *key,
    const bfrt_proto::IdleTimeoutNotification_NotificationType notif_type,
    const ConnectionData &connection_sp) {
  const BfRtTable *table;
  std::vector<bf_rt_id_t> field_ids;

  auto bf_status = key->tableGet(&table);
  if (bf_status != BF_SUCCESS) {
    return bf_status;
  }

  bf_status = table->keyFieldIdListGet(&field_ids);
  if (bf_status != BF_SUCCESS) {
    return bf_status;
  }

  bfrt_proto::StreamMessageResponse response;
  auto notification = response.mutable_idle_timeout_notification();
  notification->set_type(notif_type);
  auto table_entry = notification->mutable_table_entry();
  auto dev_tgt = notification->mutable_target();
  dev_tgt->set_device_id(target.dev_id);
  dev_tgt->set_pipe_id(target.pipe_id);

  bf_rt_id_t table_id;
  bf_status = table->tableIdGet(&table_id);
  table_entry->set_table_id(table_id);
  auto table_key = table_entry->mutable_key();

  for (const auto &field_id : field_ids) {
    auto field = table_key->add_fields();
    field->set_field_id(field_id);
    KeyFieldType type;
    bf_status = table->keyFieldTypeGet(field_id, &type);
    size_t size;
    table->keyFieldSizeGet(field_id, &size);
    size = (size + 7) / 8;
    switch (type) {
      case KeyFieldType::EXACT: {
        std::vector<uint8_t> value(size);
        bf_status = key->getValue(field_id, size, &value[0]);
        auto exact = field->mutable_exact();
        exact->set_value(&value[0], size);
      } break;
      case KeyFieldType::TERNARY: {
        std::vector<uint8_t> value(size);
        std::vector<uint8_t> mask(size);
        bf_status = key->getValueandMask(field_id, size, &value[0], &mask[0]);
        auto ternary = field->mutable_ternary();
        ternary->set_value(&value[0], size);
        ternary->set_mask(&mask[0], size);
      } break;
      case KeyFieldType::LPM: {
        std::vector<uint8_t> value(size);
        uint16_t prefix_len;
        bf_status = key->getValueLpm(field_id, size, &value[0], &prefix_len);
        auto lpm = field->mutable_lpm();
        lpm->set_value(&value[0], size);
        lpm->set_prefix_len(prefix_len);
      } break;
      case KeyFieldType::RANGE: {
        std::vector<uint8_t> start(size);
        std::vector<uint8_t> end(size);
        bf_status = key->getValueRange(field_id, size, &start[0], &end[0]);
        auto range = field->mutable_range();
        range->set_low(&start[0], size);
        range->set_high(&end[0], size);
      } break;
      case KeyFieldType::OPTIONAL: {
        std::vector<uint8_t> value(size);
        bool is_valid;
        bf_status = key->getValueOptional(field_id, size, &value[0], &is_valid);
        auto optional = field->mutable_optional();
        optional->set_value(&value[0], size);
        optional->set_is_valid(is_valid);
      } break;
      default:
        break;
    }
  }

  LOG_DBG("%s:%d IdleTimeout notification: %s",
          __func__,
          __LINE__,
          notification->DebugString().c_str());
  connection_sp.sendStreamMessage(response);

  return BF_SUCCESS;
}

bf_status_t idle_active_callback(const bf_rt_target_t &target,
                                 const BfRtTableKey *key,
                                 void *cookie) {
  // Retrieve the client_id from the cookie
  const auto cookie_data = reinterpret_cast<uintptr_t>(cookie);
  uint32_t client_id = getClientIdFromCookie(cookie_data);
  // Get the connection data for this client
  std::shared_ptr<const ConnectionData> connection_sp;
  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  if (!grpc_status.ok()) {
    LOG_ERROR("%s:%d Unable to retrieve connection data for client with id %d",
              __func__,
              __LINE__,
              client_id);
    return BF_UNEXPECTED;
  }
  // Nothing to do if active notification and client did not subscribe for it
  if (!connection_sp->getNotifications().enable_entry_active_)
    return BF_SUCCESS;

  return idletime_notification_send(
      target,
      key,
      bfrt_proto::IdleTimeoutNotification_NotificationType_ENTRY_ACTIVE,
      *connection_sp);
}

bf_status_t idletime_callback(const bf_rt_target_t &target,
                              const BfRtTableKey *key,
                              void *cookie) {
  // Retrieve the client_id from the cookie
  const auto cookie_data = reinterpret_cast<uintptr_t>(cookie);
  uint32_t client_id = getClientIdFromCookie(cookie_data);
  // Get the connection data for this client
  std::shared_ptr<const ConnectionData> connection_sp;
  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  if (!grpc_status.ok()) {
    LOG_ERROR("%s:%d Unable to retrieve connection data for client with id %d",
              __func__,
              __LINE__,
              client_id);
    return BF_UNEXPECTED;
  }
  // Nothing to do if idle notification and client did not subscribe for it
  if (!connection_sp->getNotifications().enable_idletimeout_) return BF_SUCCESS;

  return idletime_notification_send(
      target,
      key,
      bfrt_proto::IdleTimeoutNotification_NotificationType_ENTRY_IDLE,
      *connection_sp);
}

bf_status_t learn_callback(const bf_rt_target_t &bf_rt_target,
                           const std::shared_ptr<BfRtSession> session,
                           std::vector<std::unique_ptr<BfRtLearnData>> vec,
                           bf_rt_learn_msg_hdl *learn_msg_hdl,
                           const void *cookie) {
  // Retrieve the client_id from the cookie
  const auto cookie_data = reinterpret_cast<uintptr_t>(cookie);
  uint32_t client_id = getClientIdFromCookie(cookie_data);
  SetFwdConfigLockGuard read_lock(BfRtServer::getInstance().setFwdRwLockget(),
                                  false);
  if (!read_lock) {
    LOG_ERROR(" %s:%d Failed to acquire read lock for client %d",
              __func__,
              __LINE__,
              client_id);
    return BF_IN_USE;
  }

  // Get the connection data for this client
  std::shared_ptr<const ConnectionData> connection_sp;
  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  if (!grpc_status.ok()) {
    LOG_ERROR("%s:%d Unable to retrieve connection data for client with id %d",
              __func__,
              __LINE__,
              client_id);
    return BF_UNEXPECTED;
  }

  const BoundProgram *bound_program = nullptr;
  grpc_status = connection_sp->boundProgramGet(&bound_program);
  if (!grpc_status.ok() || !bound_program) {
    LOG_ERROR(
        "%s:%d Unable to retrieve bound_program data for client with id %d",
        __func__,
        __LINE__,
        client_id);
    return BF_UNEXPECTED;
  }

  const auto info = bound_program->bfRtInfoGet();
  if (!info) {
    LOG_ERROR("%s:%d BfRtInfo obj retrieved for the client is null",
              __func__,
              __LINE__);
    BF_RT_DBGCHK(0);
    return BF_UNEXPECTED;
  }

  const BfRtLearn *learn;
  auto bf_status = vec.front()->getParent(&learn);
  if (bf_status != BF_SUCCESS) {
    LOG_ERROR("%s:%d Error getting learn_obj", __func__, __LINE__);
    return bf_status;
  }

  bf_rt_id_t learn_id;
  bf_status = learn->learnIdGet(&learn_id);
  if (bf_status != BF_SUCCESS) {
    LOG_ERROR("%s:%d Error getting ID for learn obj", __func__, __LINE__);
    return bf_status;
  }

  std::vector<bf_rt_id_t> field_ids;
  bf_status = learn->learnFieldIdListGet(&field_ids);
  if (bf_status != BF_SUCCESS) {
    LOG_ERROR("%s:%d Error getting field_list for learn obj %d",
              __func__,
              __LINE__,
              learn_id);
    return bf_status;
  }

  bfrt_proto::StreamMessageResponse response;
  auto digest = response.mutable_digest();
  digest->set_digest_id(learn_id);
  digest->set_list_id(0);
  auto target = digest->mutable_target();
  target->set_device_id(bf_rt_target.dev_id);
  target->set_pipe_id(bf_rt_target.pipe_id);
  target->set_direction(bf_rt_target.direction);
  target->set_prsr_id(bf_rt_target.prsr_id);
  // TODO(msharif) Save the learn_msg_hdl, when DigestListAck is implemented.
  for (const auto &learn_data : vec) {
    auto data = digest->add_data();
    size_t size_bits, size;
    uint8_t *val;
    for (const auto &field_id : field_ids) {
      learn->learnFieldSizeGet(field_id, &size_bits);
      size = (size_bits + 7) / 8;
      val = new uint8_t[size];
      bf_status = learn_data->getValue(field_id, size, val);
      if (bf_status != BF_SUCCESS) {
        LOG_ERROR("%s:%d Error getting value for field_id %d",
                  __func__,
                  __LINE__,
                  field_id);
        return bf_status;
      }

      auto field = data->add_fields();
      field->set_field_id(field_id);
      // For learn data, learn fields are all treated as byte stream. There is
      // no possibility of a learn field being a float value
      field->set_stream(val, size);
      delete[] val;
    }
  }

  // Ack the learn digest so that the hardware can learn more
  bf_status = learn->bfRtLearnNotifyAck(session, learn_msg_hdl);
  if (bf_status != BF_SUCCESS) {
    LOG_ERROR(
        "%s:%d Error while acknowledging the learn notification for learn obj "
        "%d learn_msg_hdl %d",
        __func__,
        __LINE__,
        learn_id,
        learn_msg_hdl->unused);
  }

  LOG_DBG("%s:%d Learn notification: %s",
          __func__,
          __LINE__,
          digest->DebugString().c_str());
  connection_sp->sendStreamMessage(response);

  return BF_SUCCESS;
}

}  // namespace callbacks

namespace {
#ifdef BFRT_PERF_TEST
Status calculateRate(const struct timespec &start,
                     const int &num_entries,
                     BfRtServerErrorReporter *error_reporter) {
  if (!error_reporter->get_status().ok()) {
    // Indicates that were one or more errors while installing entries in the
    // hardware, so why even bother sending the rate to the client. So just
    // return
    LOG_ERROR(
        "%s:%d : Errors encountered while adding entries, thus not calculating "
        "the rate",
        __func__,
        __LINE__);
    return error_reporter->get_status();
  }

  struct timespec end {
    0
  }, diff{0};
  /*ProfilerFlush();*/
  /*ProfilerStop();*/

  clock_gettime(CLOCK_MONOTONIC, &end);
  diff.tv_sec = end.tv_sec - start.tv_sec;
  diff.tv_nsec = end.tv_nsec - start.tv_nsec;
  if (diff.tv_nsec < 0) {
    diff.tv_sec -= 1;
    diff.tv_nsec += 1000000000;
  }

  uint64_t microseconds = diff.tv_sec * 1000000 + diff.tv_nsec / 1000;
  double ops_per_microsecond =
      (static_cast<double>(num_entries)) / microseconds;
  double rate = ops_per_microsecond * 1000000;

  LOG_DBG("Bf-Rt Server Rate for addind %d entries is %lf", num_entries, rate);

  // This would mean that all the entries were successfully added to the hw.
  // Thus send the rate embedded in the status. We chose the grpc::StatusCode::
  // UNKNOWN to communicate the rate to the client.
  std::ostringstream error_msg;
  error_msg << "Bf-Rt Rate for " << num_entries << " entries is : " << rate;
  Status sts(StatusCode::UNKNOWN, error_msg.str());
  return sts;
}
#endif

struct bfRtInfoNotFoundException : public std::exception {
  const char *what() const throw() { return "BfRtInfo Not Found"; }
};

struct fwdConfigError : public std::exception {
  const char *what() const throw() { return "Failed to set fwd config"; }
};

enum class BfRtProtoAtomicityType {
  CONTINUE_ON_ERROR,
  ROLLBACK_ON_ERROR,
  DATAPLANE_ATOMIC
};

enum class BfRtDirectionType { INGRESS = 0, EGRESS = 1, ALL_GRESS = 0xFF };

BfRtProtoAtomicityType getAtomicity(const bfrt_proto::WriteRequest *request) {
  switch (request->atomicity()) {
    case bfrt_proto::WriteRequest_Atomicity_CONTINUE_ON_ERROR: {
      return BfRtProtoAtomicityType::CONTINUE_ON_ERROR;
    }
    case bfrt_proto::WriteRequest_Atomicity_ROLLBACK_ON_ERROR: {
      return BfRtProtoAtomicityType::ROLLBACK_ON_ERROR;
    }
    case bfrt_proto::WriteRequest_Atomicity_DATAPLANE_ATOMIC: {
      return BfRtProtoAtomicityType::DATAPLANE_ATOMIC;
    }
    default: {
      return BfRtProtoAtomicityType::CONTINUE_ON_ERROR;
    }
  }
  return BfRtProtoAtomicityType::CONTINUE_ON_ERROR;
}

std::string table_name_get(const BfRtTable *table) {
  std::string name;
  auto bf_status = table->tableNameGet(&name);
  if (bf_status == BF_SUCCESS) {
    return name;
  }
  return "";
}

TableOperationsType getOperationType(const std::string &op_name,
                                     const BfRtTable *table) {
  std::set<TableOperationsType> type_set;
  bf_status_t sts = table->tableOperationsSupported(&type_set);
  if (sts != BF_SUCCESS) {
    return TableOperationsType::INVALID;
  }
  for (const auto &type : type_set) {
    if (type == TableOperationsType::COUNTER_SYNC &&
        (op_name == "Sync" || op_name == "SyncCounters")) {
      return type;
    } else if (type == TableOperationsType::REGISTER_SYNC &&
               (op_name == "Sync" || op_name == "SyncRegisters")) {
      return type;
    } else if (type == TableOperationsType::HIT_STATUS_UPDATE &&
               op_name == "UpdateHitState") {
      return type;
    }
  }
  return TableOperationsType::INVALID;
}

Status getBfRtInfo(const uint32_t & /*client_id*/,
                   const bf_dev_id_t &device_id,
                   const std::shared_ptr<const ConnectionData> &connection_sp,
                   const BfRtInfo **info) {
  const BoundProgram *bound_program = nullptr;
  auto grpc_status = connection_sp->boundProgramGet(&bound_program);
  if (!grpc_status.ok() || !bound_program) {
    check_and_return(BF_UNEXPECTED, "Unable to get bound_program");
  } else if (bound_program->deviceIdGet() != device_id) {
    check_and_return(
        BF_INVALID_ARG,
        "The device id retrieved for this client connection (%d) does "
        "not match the device id %d passed in",
        bound_program->deviceIdGet(),
        device_id);
  } else {
    *info = bound_program->bfRtInfoGet();
  }

  return Status();
}

Status getBfRtInfoIndependent(const bf_dev_id_t &device_id,
                              const std::string p4_name,
                              const BfRtInfo **info) {
  auto bf_status =
      BfRtDevMgr::getInstance().bfRtInfoGet(device_id, p4_name, info);
  check_and_return(
      bf_status, "Unable to obtain BF_RT info for (%s)", p4_name.c_str());
  return Status();
}

inline uint32_t formCookieFromDeviceAndClient(const bf_dev_id_t &dev_id,
                                              const uint32_t &client_id) {
  return ((dev_id << 24) | (client_id & 0x00ffffff));
}

Status setAttributeIdleTable(const uint32_t &client_id,
                             const BfRtSession &session,
                             const bf_rt_target_t &target,
                             const BfRtTable &table,
                             const bfrt_proto::IdleTable &idle_table) {
  std::unique_ptr<BfRtTableAttributes> attr;
  auto ttl_query_interval = idle_table.ttl_query_interval();
  auto max_ttl = idle_table.max_ttl();
  auto min_ttl = idle_table.min_ttl();
  auto enable = idle_table.enable();
  bf_status_t bf_status = BF_SUCCESS;
  // 1. Allocate attribute
  // 2. Idle params set on attribute
  // 3. Attribute set on Table
  switch (idle_table.idle_table_mode()) {
    case (bfrt_proto::IdleTable_IdleTableMode_IDLE_TABLE_POLL_MODE): {
      bf_status =
          table.attributeAllocate(TableAttributesType::IDLE_TABLE_RUNTIME,
                                  TableAttributesIdleTableMode::POLL_MODE,
                                  &attr);
      check_and_return(bf_status,
                       "Failed to allocate Attribute of type Idle Table");
      bf_status = attr->idleTablePollModeSet(enable);
      check_and_return(bf_status, "Failed to set Idle Poll Mode");
      break;
    }
    case (bfrt_proto::IdleTable_IdleTableMode_IDLE_TABLE_NOTIFY_MODE): {
      bf_status =
          table.attributeAllocate(TableAttributesType::IDLE_TABLE_RUNTIME,
                                  TableAttributesIdleTableMode::NOTIFY_MODE,
                                  &attr);
      check_and_return(bf_status,
                       "Failed to allocate Attribute of type Idle Table");
      std::shared_ptr<const ConnectionData> connection_sp;
      auto grpc_status =
          BfRtServer::getInstance().getConnection(client_id, &connection_sp);
      grpc_check_and_return(grpc_status, "Falied to get connection");

      if (!connection_sp->getNotifications().enable_idletimeout_) {
        check_and_return(BF_INVALID_ARG,
                         "idletimeout notifications are not enabled"
                         "for client %d",
                         client_id);
      }
      bf_status = attr->idleTableNotifyModeSet(
          enable,
          callbacks::idletime_callback,     // idle
          callbacks::idle_active_callback,  // active
          ttl_query_interval,
          max_ttl,
          min_ttl,
          reinterpret_cast<const void *>(
              formCookieFromDeviceAndClient(target.dev_id, client_id)));
      check_and_return(bf_status, "Failed to set Idle Notify Mode");
      break;
    }
    default: {
      check_and_return(BF_INVALID_ARG, "Invalid Idle Table Mode");
    }
  }
  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status, "Failed to set Idle Params");
  return Status();
}

Status getAttributeIdleTable(const BfRtSession &session,
                             const bf_rt_target_t &target,
                             const BfRtTable &table,
                             bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  // It doesn't matter which mode is used to allocate for get
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status =
      table.attributeAllocate(TableAttributesType::IDLE_TABLE_RUNTIME,
                              TableAttributesIdleTableMode::POLL_MODE,
                              &attr);
  check_and_return(bf_status, "Failed to allocate Attribute for idletimeout");

  // 2. Attribute get on table
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status, "Failed to get Attribute for idleTimeout");

  // 3. get on the attribute object
  TableAttributesIdleTableMode mode;
  bool enable;
  BfRtIdleTmoExpiryCb callback;
  uint32_t ttl_query_interval;
  uint32_t max_ttl;
  uint32_t min_ttl;

  bf_status = attr->idleTableGet(&mode,
                                 &enable,
                                 &callback,
                                 &ttl_query_interval,
                                 &max_ttl,
                                 &min_ttl,
                                 nullptr);
  check_and_return(bf_status, "Failed to get Attribute for idleTimeout");

  // 4. set response
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();

  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);
  auto read_idle_table = read_attribute_response->mutable_idle_table();

  switch (mode) {
    case (TableAttributesIdleTableMode::POLL_MODE): {
      read_idle_table->set_idle_table_mode(
          bfrt_proto::IdleTable_IdleTableMode_IDLE_TABLE_POLL_MODE);
      read_idle_table->set_enable(enable);
      break;
    }
    case (TableAttributesIdleTableMode::NOTIFY_MODE): {
      read_idle_table->set_idle_table_mode(
          bfrt_proto::IdleTable_IdleTableMode_IDLE_TABLE_NOTIFY_MODE);
      read_idle_table->set_enable(enable);
      read_idle_table->set_ttl_query_interval(ttl_query_interval);
      read_idle_table->set_max_ttl(max_ttl);
      read_idle_table->set_min_ttl(min_ttl);
      break;
    }
    default: {
      check_and_return(BF_INVALID_ARG, "Invalid IdleTimeout Mode");
    }
  }
  return Status();
}

Status getAttributePortStatusChgNotify(const BfRtSession &session,
                                       const bf_rt_target_t &target,
                                       const BfRtTable &table,
                                       bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status =
      table.attributeAllocate(TableAttributesType::PORT_STATUS_NOTIF, &attr);
  check_and_return(
      bf_status, "Failed to allocate Attribute for port status change notify");
  // 2. Attribute get on Table
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status,
                   "Failed to get Attribute for Port status change notify");

  // 3. get on the attribute object
  bool enable;
  BfRtPortStatusNotifCb callback;
  bf_status =
      attr->portStatusChangeNotifGet(&enable, &callback, nullptr, nullptr);

  check_and_return(bf_status,
                   "Failed to get Port status change notif attribute");

  // 4. set response
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();

  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);
  read_attribute_response->mutable_port_status_notify()->set_enable(enable);
  return Status();
}

Status getAttributeSelectorUpdateChgNotify(const BfRtSession &session,
                                           const bf_rt_target_t &target,
                                           const BfRtTable &table,
                                           bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status = table.attributeAllocate(
      TableAttributesType::SELECTOR_UPDATE_CALLBACK, &attr);
  check_and_return(
      bf_status, "Failed to allocate Attribute for Selector Update CB notify");
  // 2. Attribute get on Table
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status,
                   "Failed to get Attribute for Selector Update CB notify");

  // 3. get on the attribute object
  bool enable;
  selUpdateCb callback;
  bfrt::BfRtSession *read_session;

  bf_status =
      attr->selectorUpdateCbGet(&enable, &read_session, &callback, nullptr);

  check_and_return(bf_status, "Failed to get Selector Update CB attribute");

  // 4. set response
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();

  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);

  return Status();
}

Status setAttributePortStatIntvl(const BfRtSession &session,
                                 const bf_rt_target_t &target,
                                 const BfRtTable &table,
                                 const bfrt_proto::StatePullIntvl &intvl_ms) {
  std::unique_ptr<BfRtTableAttributes> attr;
  auto time_ms = intvl_ms.intvl_val();
  bf_status_t bf_status = BF_SUCCESS;
  bf_status = table.attributeAllocate(
      TableAttributesType::PORT_STAT_POLL_INTVL_MS, &attr);
  check_and_return(bf_status,
                   "Failed to allocate Attribute for port stat poll interval");
  bf_status = attr->portStatPollIntvlMsSet(time_ms);
  check_and_return(bf_status, "Failed to set port stat poll interval");
  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status, "Failed to set Port Stat Poll Intvl Params");
  return Status();
}

Status getAttributePortStatIntvl(const BfRtSession &session,
                                 const bf_rt_target_t &target,
                                 const BfRtTable &table,
                                 bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status = table.attributeAllocate(
      TableAttributesType::PORT_STAT_POLL_INTVL_MS, &attr);
  check_and_return(bf_status,
                   "Failed to allocate Attribute for port stat poll interval");
  // 2. Attribute get on Table
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status,
                   "Failed to get Attribute for Port Stat Poll Intvl");

  // 3. get on the attribute object
  uint32_t time_ms;
  bf_status = attr->portStatPollIntvlMsGet(&time_ms);
  check_and_return(bf_status, "Failed to get Stat poll interval");

  // 4. set response
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();

  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);
  read_attribute_response->mutable_intvl_ms()->set_intvl_val(time_ms);
  return Status();
}

Status setAttributePreDeviceConfig(
    const BfRtSession &session,
    const bf_rt_target_t &target,
    const BfRtTable &table,
    const bfrt_proto::PreDeviceConfig &pre_device_config) {
  bf_status_t bf_status = BF_SUCCESS;
  std::unique_ptr<BfRtTableAttributes> attr;

  bf_status =
      table.attributeAllocate(TableAttributesType::PRE_DEVICE_CONFIG, &attr);
  check_and_return(
      bf_status, "Failed to allocate Attribute Object for PRE port protection");

  // Since pre device config attribute has multiple configs and can be
  // set selectively, check whether each config exists or not and set it
  // in the attribute object if it exists (this will implicity also set the
  // active attributes list which will be used to configure each config
  // finally in MC mgr based on whether that config is present in
  // active attributes set or not).
  if (pre_device_config.has_pre_global_rid()) {
    auto global_rid = pre_device_config.pre_global_rid().global_rid();

    bf_status = attr->preGlobalRidSet(global_rid);
    check_and_return(bf_status, "Failed to set global rid in attribute object");
  }

  if (pre_device_config.has_pre_port_protection()) {
    auto enable = pre_device_config.pre_port_protection().enable();

    bf_status = attr->prePortProtectionSet(enable);
    check_and_return(bf_status,
                     "Failed to set port protection in attribute object");
  }

  if (pre_device_config.has_pre_fast_failover()) {
    auto enable = pre_device_config.pre_fast_failover().enable();

    bf_status = attr->preFastFailoverSet(enable);
    check_and_return(bf_status,
                     "Failed to set fast failover in attribute object");
  }

  if (pre_device_config.has_pre_max_nodes_before_yield()) {
    auto count = pre_device_config.pre_max_nodes_before_yield().count();

    bf_status = attr->preMaxNodesBeforeYieldSet(count);
    check_and_return(
        bf_status, "Failed to set max nodes before yield in attribute object");
  }

  if (pre_device_config.has_pre_max_node_threshold()) {
    auto node_count = pre_device_config.pre_max_node_threshold().node_count();
    auto port_lag_count =
        pre_device_config.pre_max_node_threshold().port_lag_count();

    bf_status = attr->preMaxNodeThresholdSet(node_count, port_lag_count);
    check_and_return(
        bf_status,
        "Failed to set max node threshold values in attribute object");
  }

  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status, "Failed to set pre device config attribute");

  return Status();
}

Status getAttributePreDeviceConfig(const BfRtSession &session,
                                   const bf_rt_target_t &target,
                                   const BfRtTable &table,
                                   bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status =
      table.attributeAllocate(TableAttributesType::PRE_DEVICE_CONFIG, &attr);
  check_and_return(bf_status,
                   "Failed to allocate Attribute for pre device config");
  // 2. Attribute get on Table
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status, "Failed to get Attribute for pre device config");

  // 3. get on the attribute object and set in response for each param
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();
  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);
  auto pre_device_config = read_attribute_response->mutable_pre_device_config();

  uint32_t global_rid;
  bf_status = attr->preGlobalRidGet(&global_rid);
  if (bf_status != BF_OBJECT_NOT_FOUND) {
    check_and_return(bf_status, "Failed to get global rid in attribute object");
    pre_device_config->mutable_pre_global_rid()->set_global_rid(global_rid);
  }

  bool port_protection_enable;
  bf_status = attr->prePortProtectionGet(&port_protection_enable);
  if (bf_status != BF_OBJECT_NOT_FOUND) {
    check_and_return(bf_status,
                     "Failed to pre_port_protection in attribute object");
    pre_device_config->mutable_pre_port_protection()->set_enable(
        port_protection_enable);
  }

  bool pre_fast_failover_enable;
  bf_status = attr->preFastFailoverGet(&pre_fast_failover_enable);
  if (bf_status != BF_OBJECT_NOT_FOUND) {
    check_and_return(bf_status,
                     "Failed to get pre_fast_failover in attribute object");
    pre_device_config->mutable_pre_fast_failover()->set_enable(
        pre_fast_failover_enable);
  }

  uint32_t pre_max_nodes_before_yield_count;
  bf_status =
      attr->preMaxNodesBeforeYieldGet(&pre_max_nodes_before_yield_count);
  if (bf_status != BF_OBJECT_NOT_FOUND) {
    check_and_return(
        bf_status,
        "Failed to get pre max nodes before yield in attribute object");
    pre_device_config->mutable_pre_max_nodes_before_yield()->set_count(
        pre_max_nodes_before_yield_count);
  }

  uint32_t node_count;
  uint32_t port_lag_count;
  bf_status = attr->preMaxNodeThresholdGet(&node_count, &port_lag_count);
  if (bf_status != BF_OBJECT_NOT_FOUND) {
    check_and_return(
        bf_status, "Failed to get pre_max_node_threshold in attribute object");
    pre_device_config->mutable_pre_max_node_threshold()->set_node_count(
        node_count);
    pre_device_config->mutable_pre_max_node_threshold()->set_port_lag_count(
        port_lag_count);
  }
  return Status();
}
Status setAttributeEntryScope(const BfRtSession &session,
                              const bf_rt_target_t &target,
                              const BfRtTable &table,
                              const bfrt_proto::EntryScope &entry_scope) {
  std::unique_ptr<BfRtTableAttributes> attr;
  // 1. Allocate attribute
  // 2. Entry params set on attribute
  // 3. Attribute set on Table
  auto bf_status =
      table.attributeAllocate(TableAttributesType::ENTRY_SCOPE, &attr);
  check_and_return(bf_status, "Failed to allocate Attribute for entry scope");
  bf_rt_target_t target_temp = {
      target.dev_id, target.pipe_id, target.direction, target.prsr_id};
  if (entry_scope.has_gress_scope() && entry_scope.has_pipe_scope() &&
      entry_scope.has_prsr_scope()) {
    TableGressScope gress_scope = TableGressScope::GRESS_SCOPE_ALL_GRESS;
    TableEntryScope pipe_scope = TableEntryScope::ENTRY_SCOPE_ALL_PIPELINES;
    std::unique_ptr<BfRtTableEntryScopeArguments> scope_args;
    TablePrsrScope prsr_scope = TablePrsrScope::PRSR_SCOPE_ALL_PRSRS_IN_PIPE;
    GressTarget prsr_gress = GressTarget::GRESS_TARGET_ALL;
    bf_status = attr->entryScopeArgumentsAllocate(&scope_args);
    check_and_return(bf_status,
                     "Failed to allocate BfRtTableEntryScopeArguments");
    // gress
    auto &gress_msg = entry_scope.gress_scope();
    if (gress_msg.scope_case() != bfrt_proto::Mode::kPredef) {
      check_and_return(BF_INVALID_ARG,
                       "Invalid Argument for entry_scope. gress_scope can only "
                       "be PredefinedMode");
    }
    switch (gress_msg.predef()) {
      case (bfrt_proto::Mode_PredefinedMode_ALL): {
        gress_scope = TableGressScope::GRESS_SCOPE_ALL_GRESS;
        break;
      }
      case (bfrt_proto::Mode_PredefinedMode_SINGLE): {
        gress_scope = TableGressScope::GRESS_SCOPE_SINGLE_GRESS;
        break;
      }
      default:
        check_and_return(BF_INVALID_ARG,
                         "Invalid Argument for entry_scope. gress_scope can "
                         "only be ALL_PIPES or SINGLE_PIPE");
    }
    // pipe
    auto &pipe_msg = entry_scope.pipe_scope();
    switch (pipe_msg.scope_case()) {
      case (bfrt_proto::Mode::kPredef): {
        switch (pipe_msg.predef()) {
          case (bfrt_proto::Mode_PredefinedMode_ALL): {
            pipe_scope = TableEntryScope::ENTRY_SCOPE_ALL_PIPELINES;
            break;
          }
          case (bfrt_proto::Mode_PredefinedMode_SINGLE): {
            pipe_scope = TableEntryScope::ENTRY_SCOPE_SINGLE_PIPELINE;
            break;
          }
          default:
            check_and_return(BF_INVALID_ARG,
                             "Invalid Argument for entry_scope. predef "
                             "pipe_scope can only be ALL_PIPES or SINGLE_PIPE");
        }
        std::bitset<64> bitval(pipe_msg.args());
        bf_status = scope_args.get()->setValue(bitval);
        check_and_return(bf_status, "Failed to set pipe_scope Arguments");
        break;
      }
      case (bfrt_proto::Mode::kUserDefined): {
        pipe_scope = TableEntryScope::ENTRY_SCOPE_USER_DEFINED;
        auto scope_val = pipe_msg.user_defined();
        std::bitset<64> bitval(scope_val);
        bf_status = scope_args.get()->setValue(bitval);
        check_and_return(bf_status, "Failed to set pipe_scope Arguments");
        auto pipe_gress = pipe_msg.args();
        switch (pipe_gress) {
          case static_cast<uint32_t>(BfRtDirectionType::INGRESS):
            target_temp.direction = BF_DEV_DIR_INGRESS;
            break;
          case static_cast<uint32_t>(BfRtDirectionType::EGRESS):
            target_temp.direction = BF_DEV_DIR_EGRESS;
            break;
          case static_cast<uint32_t>(BfRtDirectionType::ALL_GRESS):
            target_temp.direction = BF_DEV_DIR_ALL;
            break;
          default:
            check_and_return(BF_INVALID_ARG,
                             "Invalid Argument for pipe_scope. args can only "
                             "be 0-Ingress, 1-Egress, 0xFF-All gress from "
                             "target");
        }
        break;
      }
      default:
        check_and_return(BF_INVALID_ARG, "Invalid entry_scope type");
    }
    (void)target_temp;
    // prsr
    auto &prsr_msg = entry_scope.prsr_scope();
    if (prsr_msg.scope_case() != bfrt_proto::Mode::kPredef) {
      check_and_return(BF_INVALID_ARG,
                       "Invalid Argument for prsr_scope. prsr_scope can only "
                       "be PredefinedMode");
    }
    switch (prsr_msg.predef()) {
      case (bfrt_proto::Mode_PredefinedMode_ALL): {
        prsr_scope = TablePrsrScope::PRSR_SCOPE_ALL_PRSRS_IN_PIPE;
        break;
      }
      case (bfrt_proto::Mode_PredefinedMode_SINGLE): {
        prsr_scope = TablePrsrScope::PRSR_SCOPE_SINGLE_PRSR;
        break;
      }
      default:
        check_and_return(BF_INVALID_ARG,
                         "Invalid Argument for prsr_scope. prsr_scope can only "
                         "be ALL_PIPES or SINGLE_PIPE");
    }
    switch (prsr_msg.args()) {
      case 0:
        prsr_gress = GressTarget::GRESS_TARGET_INGRESS;
        break;
      case 1:
        prsr_gress = GressTarget::GRESS_TARGET_EGRESS;
        break;
      case 0xFF:
        prsr_gress = GressTarget::GRESS_TARGET_ALL;
        break;
      default:
        check_and_return(BF_INVALID_ARG,
                         "Invalid Argument for prsr_scope. args can only be "
                         "0-Ingress, 2-Egress, 0xFF-All gress");
    }

    // configure
    bf_status = attr->entryScopeParamsSet(
        gress_scope, pipe_scope, *(scope_args.get()), prsr_scope, prsr_gress);
    check_and_return(bf_status, "Failed to set Entry Scope Params");
  } else if (!entry_scope.has_gress_scope() && entry_scope.has_pipe_scope() &&
             !entry_scope.has_prsr_scope()) {
    TableEntryScope pipe_entry_scope =
        TableEntryScope::ENTRY_SCOPE_ALL_PIPELINES;
    auto pipe_msg = entry_scope.pipe_scope();
    switch (pipe_msg.scope_case()) {
      case (bfrt_proto::Mode::kPredef): {
        switch (pipe_msg.predef()) {
          case (bfrt_proto::Mode_PredefinedMode_ALL): {
            pipe_entry_scope = TableEntryScope::ENTRY_SCOPE_ALL_PIPELINES;
            break;
          }
          case (bfrt_proto::Mode_PredefinedMode_SINGLE): {
            pipe_entry_scope = TableEntryScope::ENTRY_SCOPE_SINGLE_PIPELINE;
            break;
          }
          default:
            check_and_return(BF_INVALID_ARG,
                             "Invalid Argument for entry_scope");
        }
        attr->entryScopeParamsSet(pipe_entry_scope);
        break;
      }
      case (bfrt_proto::Mode::kUserDefined): {
        pipe_entry_scope = TableEntryScope::ENTRY_SCOPE_USER_DEFINED;
        auto scope_val = pipe_msg.user_defined();
        std::bitset<64> bitval(scope_val);
        std::unique_ptr<BfRtTableEntryScopeArguments> scope_args;
        bf_status = attr->entryScopeArgumentsAllocate(&scope_args);
        bf_status = scope_args.get()->setValue(bitval);
        check_and_return(bf_status, "Failed to set Entry Scope Arguments");

        bf_status =
            attr->entryScopeParamsSet(pipe_entry_scope, *(scope_args.get()));
        check_and_return(bf_status, "Failed to set Entry Scope Params");
        break;
      }
      default:
        check_and_return(BF_INVALID_ARG, "Invalid entry_scope type");
    }
  } else {
    check_and_return(BF_INVALID_ARG,
                     "Invalid entry_scope. Either only configure pipe_scope or "
                     "configure all three scopes");
  }
  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status, "Failed to set Attribute for entry_scope");
  return Status();
}

Status getAttributeEntryScope(const BfRtSession &session,
                              const bf_rt_target_t &target,
                              const BfRtTable &table,
                              bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  // 2. Attribute get on table
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status =
      table.attributeAllocate(TableAttributesType::ENTRY_SCOPE, &attr);
  check_and_return(bf_status, "Failed to allocate Attribute for entry scope");
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status, "Failed to get Attribute for entry_scope");

  // 3. Allocate individual args
  // 4. get on the attribute object
  TableGressScope gress_scope;
  TableEntryScope pipe_scope;
  TablePrsrScope prsr_scope;
  GressTarget prsr_gress;
  std::unique_ptr<BfRtTableEntryScopeArguments> scope_args;
  bf_status = attr->entryScopeArgumentsAllocate(&scope_args);
  check_and_return(bf_status,
                   "Failed to allocate BfRtTableEntryScopeArguments");

  bf_status = attr->entryScopeParamsGet(
      &gress_scope, &pipe_scope, scope_args.get(), &prsr_scope, &prsr_gress);
  check_and_return(bf_status, "Failed to get entry scope params");
  // 5. set response
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();

  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);
  auto read_entry_scope = read_attribute_response->mutable_entry_scope();

  // gress
  auto read_entry_scope_gress_scope = read_entry_scope->mutable_gress_scope();
  switch (gress_scope) {
    case TableGressScope::GRESS_SCOPE_ALL_GRESS:
      read_entry_scope_gress_scope->set_predef(
          bfrt_proto::Mode_PredefinedMode_ALL);
      break;
    case TableGressScope::GRESS_SCOPE_SINGLE_GRESS:
      read_entry_scope_gress_scope->set_predef(
          bfrt_proto::Mode_PredefinedMode_SINGLE);
      break;
    default:
      check_and_return(BF_INVALID_ARG,
                       "Invalid Argument for entry_scope. gress_scope can "
                       "only be ALL_PIPES or SINGLE_PIPE");
  }
  // pipe
  auto read_entry_scope_pipe_scope = read_entry_scope->mutable_pipe_scope();
  std::bitset<64> bitval(0);
  scope_args.get()->getValue(&bitval);
  switch (pipe_scope) {
    case TableEntryScope::ENTRY_SCOPE_ALL_PIPELINES:
      read_entry_scope_pipe_scope->set_predef(
          bfrt_proto::Mode_PredefinedMode_ALL);
      read_entry_scope_pipe_scope->set_args(
          static_cast<uint64_t>(bitval.to_ullong()));
      break;
    case TableEntryScope::ENTRY_SCOPE_SINGLE_PIPELINE:
      read_entry_scope_pipe_scope->set_predef(
          bfrt_proto::Mode_PredefinedMode_SINGLE);
      read_entry_scope_pipe_scope->set_args(
          static_cast<uint64_t>(bitval.to_ullong()));
      break;
    case TableEntryScope::ENTRY_SCOPE_USER_DEFINED:
      read_entry_scope_pipe_scope->set_user_defined(
          static_cast<uint64_t>(bitval.to_ullong()));
      break;
    default:
      check_and_return(BF_INVALID_ARG,
                       "Invalid Argument for entry_scope. pipe_scope can "
                       "only be ALL_PIPES or SINGLE_PIPE");
  }
  // prsr
  auto read_entry_scope_prsr_scope = read_entry_scope->mutable_prsr_scope();
  scope_args.get()->getValue(&bitval);
  switch (prsr_scope) {
    case TablePrsrScope::PRSR_SCOPE_ALL_PRSRS_IN_PIPE:
      read_entry_scope_prsr_scope->set_predef(
          bfrt_proto::Mode_PredefinedMode_ALL);
      read_entry_scope_prsr_scope->set_args(
          static_cast<uint64_t>(bitval.to_ullong()));
      break;
    case TablePrsrScope::PRSR_SCOPE_SINGLE_PRSR:
      read_entry_scope_prsr_scope->set_predef(
          bfrt_proto::Mode_PredefinedMode_SINGLE);
      read_entry_scope_prsr_scope->set_args(
          static_cast<uint64_t>(bitval.to_ullong()));
      break;
    default:
      check_and_return(BF_INVALID_ARG,
                       "Invalid Argument for entry_scope. prsr_scope can "
                       "only be ALL_PIPES or SINGLE_PIPE");
  }
  switch (prsr_gress) {
    case GressTarget::GRESS_TARGET_INGRESS:
      read_entry_scope_prsr_scope->set_args(0);
      break;
    case GressTarget::GRESS_TARGET_EGRESS:
      read_entry_scope_prsr_scope->set_args(1);
      break;
    case GressTarget::GRESS_TARGET_ALL:
      read_entry_scope_prsr_scope->set_args(0xff);
      break;
    default:
      check_and_return(BF_INVALID_ARG,
                       "Invalid Argument for prsr_scope. args can only be "
                       "0-Ingress, 2-Egress, 0xFF-All gress");
  }
  return Status();
}

Status setAttributeDynKeyMask(const BfRtSession &session,
                              const bf_rt_target_t &target,
                              const BfRtTable &table,
                              const bfrt_proto::DynKeyMask &dyn_key_mask) {
  std::unique_ptr<BfRtTableAttributes> attr;
  // 1. Allocate attribute
  // 2. Entry params set on attribute
  // 3. Attribute set on Table
  auto bf_status =
      table.attributeAllocate(TableAttributesType::DYNAMIC_KEY_MASK, &attr);
  check_and_return(bf_status,
                   "Failed to allocate Attribute for dynamic key mask");
  std::unordered_map<bf_rt_id_t, std::vector<uint8_t>> field_mask;
  for (const auto &field : dyn_key_mask.fields()) {
    std::vector<uint8_t> mask(field.mask().size());
    const uint8_t *mask_p =
        reinterpret_cast<const uint8_t *>(field.mask().data());
    for (uint32_t i = 0; i < field.mask().size(); i++) {
      mask[i] = (mask_p[i]);
    }
    field_mask.insert(std::make_pair(field.field_id(), mask));
  }
  bf_status = attr->dynKeyMaskSet(field_mask);
  check_and_return(bf_status, "Failed to set Dynamic Key Mask");
  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status, "Failed to set Attribute for dynamic key mask");
  return Status();
}

Status getAttributeDynKeyMask(const BfRtSession &session,
                              const bf_rt_target_t &target,
                              const BfRtTable &table,
                              bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status =
      table.attributeAllocate(TableAttributesType::DYNAMIC_KEY_MASK, &attr);
  check_and_return(bf_status,
                   "Failed to allocate Attribute for dynamic key mask");
  // 2. Attribute get on Table
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status, "Failed to get Attribute for dynamic key mask");

  // 3. get on the attribute object
  std::unordered_map<bf_rt_id_t, std::vector<uint8_t>> field_mask;
  bf_status = attr->dynKeyMaskGet(&field_mask);
  check_and_return(bf_status, "Failed to get Dynamic Key Mask");

  // 4. set response
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();

  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);
  auto read_dyn_key_mask = read_attribute_response->mutable_dyn_key_mask();
  for (const auto &field_vector_pair : field_mask) {
    auto field = read_dyn_key_mask->add_fields();
    field->set_field_id(field_vector_pair.first);
    field->set_mask(
        reinterpret_cast<const void *>(field_vector_pair.second.data()),
        field_vector_pair.second.size());
  }
  return Status();
}

Status setAttributeMeterByteCountAdj(
    const BfRtSession &session,
    const bf_rt_target_t &target,
    const BfRtTable &table,
    const bfrt_proto::ByteCountAdj &byte_count) {
  std::unique_ptr<BfRtTableAttributes> attr;
  // 1. Allocate attribute
  // 2. Entry params set on attribute
  // 3. Attribute set on Table
  auto bf_status =
      table.attributeAllocate(TableAttributesType::METER_BYTE_COUNT_ADJ, &attr);
  check_and_return(bf_status,
                   "Failed to allocate Attribute for meter byte count adjust");
  auto meter_byte_count = static_cast<int>(byte_count.byte_count_adjust());
  bf_status = attr->meterByteCountAdjSet(meter_byte_count);
  check_and_return(bf_status, "Failed to set meter byte count adjust");
  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status,
                   "Failed to set Attribute for meter byte count adjust");
  return Status();
}

Status getAttributeMeterByteCountAdj(const BfRtSession &session,
                                     const bf_rt_target_t &target,
                                     const BfRtTable &table,
                                     bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status =
      table.attributeAllocate(TableAttributesType::METER_BYTE_COUNT_ADJ, &attr);
  check_and_return(bf_status,
                   "Failed to allocate Attribute for meter byte count adjust");
  // 2. Attribute get on Table
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status,
                   "Failed to get Attribute for meter byte count adjust");

  // 3. get on the attribute object
  int meter_byte_count;
  bf_status = attr->meterByteCountAdjGet(&meter_byte_count);
  check_and_return(bf_status, "Failed to get meter byte count adjust");

  // 4. set response
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();

  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);
  read_attribute_response->mutable_byte_count_adj()->set_byte_count_adjust(
      meter_byte_count);
  return Status();
}

Status setAttributeDynHashing(const BfRtSession &session,
                              const bf_rt_target_t &target,
                              const BfRtTable &table,
                              const bfrt_proto::DynHashing &dyn_hashing) {
  std::unique_ptr<BfRtTableAttributes> attr;
  // 1. Allocate attribute
  // 2. Entry params set on attribute
  // 3. Attribute set on Table
  auto bf_status = table.attributeAllocate(
      TableAttributesType::DYNAMIC_HASH_ALG_SEED, &attr);
  check_and_return(bf_status, "Failed to allocate Attribute for dyn hashing");
  auto alg_hdl = static_cast<uint32_t>(dyn_hashing.alg());
  auto seed = static_cast<uint64_t>(dyn_hashing.seed());
  bf_status = attr->dynHashingSet(alg_hdl, seed);
  check_and_return(bf_status, "Failed to set Dynamic Hashing");
  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status, "Failed to set Attribute for dyn hashing");
  return Status();
}

Status getAttributeDynHashing(const BfRtSession &session,
                              const bf_rt_target_t &target,
                              const BfRtTable &table,
                              bfrt_proto::ReadResponse *response) {
  // 1. Allocate attribute
  std::unique_ptr<BfRtTableAttributes> attr;
  auto bf_status = table.attributeAllocate(
      TableAttributesType::DYNAMIC_HASH_ALG_SEED, &attr);
  check_and_return(bf_status, "Failed to allocate Attribute for dyn hashing");
  // 2. Attribute get on Table
  bf_status = table.tableAttributesGet(session, target, 0, attr.get());
  check_and_return(bf_status, "Failed to get Attribute for dynamic hashing");

  // 3. get on the attribute object
  uint32_t alg_hdl;
  uint64_t seed;
  bf_status = attr->dynHashingGet(&alg_hdl, &seed);
  check_and_return(bf_status, "Failed to get Dynamic Hashing");

  // 4. set response
  auto read_attribute_response =
      response->add_entities()->mutable_table_attribute();

  bf_rt_id_t table_id;
  table.tableIdGet(&table_id);
  read_attribute_response->set_table_id(table_id);
  read_attribute_response->mutable_dyn_hashing()->set_alg(alg_hdl);
  read_attribute_response->mutable_dyn_hashing()->set_seed(seed);
  return Status();
}

Status populateReadResponseWithNonRegisterFields(
    const BfRtTable *table,
    const bf_rt_id_t &action_id,
    const bf_rt_id_t &field_id,
    const BfRtTableData *data,
    bfrt_proto::TableData *msg_data,
    bfrt_proto::DataField *field) {
  size_t size = 0;
  DataType type;
  std::string field_name;
  bf_status_t bf_status;
  if (action_id) {
    bf_status = table->dataFieldDataTypeGet(field_id, action_id, &type);
    check_and_return(bf_status,
                     "ERROR in getting Data type for table %s field id %d",
                     table_name_get(table).c_str(),
                     field_id);
    /* Do not get size if it is a container field */
    if (type != DataType::CONTAINER) {
      bf_status = table->dataFieldSizeGet(field_id, action_id, &size);
      check_and_return(bf_status,
                       "ERROR in getting Data size for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      size = (size + 7) / 8;
    }
    bf_status = table->dataFieldNameGet(field_id, action_id, &field_name);
    check_and_return(bf_status,
                     "ERROR in getting Data name for table %s field id %d",
                     table_name_get(table).c_str(),
                     field_id);
  } else {
    bf_status = table->dataFieldDataTypeGet(field_id, &type);
    check_and_return(bf_status,
                     "ERROR in getting Data type for table %s field id %d",
                     table_name_get(table).c_str(),
                     field_id);
    /* Do not get size if it is a container field */
    if (type != DataType::CONTAINER) {
      bf_status = table->dataFieldSizeGet(field_id, &size);
      check_and_return(bf_status,
                       "ERROR in getting Data size for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      size = (size + 7) / 8;
    }

    bf_status = table->dataFieldNameGet(field_id, &field_name);
    check_and_return(bf_status,
                     "ERROR in getting Data name for table %s field id %d",
                     table_name_get(table).c_str(),
                     field_id);
  }

  /* Either msg_data or field would be valid */
  if (msg_data) {
    field = msg_data->add_fields();
    field->set_field_id(field_id);
  } else {
    field->set_field_id(field_id);
  }
  std::vector<uint8_t> value(size);
  // The type system used by BF-RT is exposed via the above API. Based on
  // the
  // type of the field, use the appropriate getValue and the appropriate
  // setter for the proto
  // Ignore OBJECT_NOT_FOUND as data fields are not mandatory in every case.
  switch (type) {
    case DataType::BYTE_STREAM:
    case DataType::UINT64: {
      bf_status = data->getValue(field_id, size, &value[0]);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      field->set_stream(&value[0], size);
      break;
    }
    case DataType::BOOL: {
      bool val = false;
      bf_status = data->getValue(field_id, &val);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      field->set_bool_val(val);
      break;
    }
    case DataType::INT64: {
      int64_t val = 0;
      bf_status = data->getValue(field_id, &val);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      field->set_stream(&val, size);
      break;
    }

    case DataType::FLOAT: {
      float val = 0;
      bf_status = data->getValue(field_id, &val);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      field->set_float_val(val);
      break;
    }
    case DataType::STRING: {
      std::string str_value;
      bf_status = data->getValue(field_id, &str_value);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      field->set_str_val(str_value);
      break;
    }
    case DataType::INT_ARR: {
      std::vector<uint32_t> int_arr;
      bf_status = data->getValue(field_id, &int_arr);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);

      auto int_arr_proto = field->mutable_int_arr_val();
      for (const auto &val : int_arr) {
        int_arr_proto->add_val(val);
      }
      break;
    }
    case DataType::BOOL_ARR: {
      std::vector<bool> bool_arr;
      bf_status = data->getValue(field_id, &bool_arr);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      auto bool_arr_proto = field->mutable_bool_arr_val();
      for (const auto &val : bool_arr) {
        bool_arr_proto->add_val(val);
      }
      break;
    }
    case DataType::STRING_ARR: {
      std::vector<std::string> str_arr;
      bf_status = data->getValue(field_id, &str_arr);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);
      auto str_arr_proto = field->mutable_str_arr_val();
      for (const auto &val : str_arr) {
        str_arr_proto->add_val(val);
      }
      break;
    }

    case DataType::CONTAINER: {
      std::vector<BfRtTableData *> all_container_items;
      bf_status = data->getValue(field_id, &all_container_items);
      if (bf_status == BF_OBJECT_NOT_FOUND) {
        bf_status = BF_SUCCESS;
        break;
      }
      check_and_return(bf_status,
                       "ERROR in getting value for table %s field id %d",
                       table_name_get(table).c_str(),
                       field_id);

      auto container_arr_val = field->mutable_container_arr_val();

      for (auto container_item : all_container_items) {
        auto *container_proto = container_arr_val->add_container();

        /* Get list of fields in this container */
        std::vector<bf_rt_id_t> field_id_vec;
        bf_status = table->containerDataFieldIdListGet(field_id, &field_id_vec);
        check_and_return(
            bf_status,
            "ERROR in getting data field Id list table:%s for field-id %d",
            table_name_get(table).c_str(),
            field_id);
        for (auto const &c_field_id : field_id_vec) {
          bfrt_proto::DataField *c_elem = container_proto->add_val();
          populateReadResponseWithNonRegisterFields(
              table, action_id, c_field_id, container_item, NULL, c_elem);
        }
      }
      break;
    }
    default:
      check_and_return(BF_NOT_SUPPORTED, "Not supported");

  }  // switch case
  check_and_return(bf_status, "Error Formulating read response");
  return Status();
}

Status populateReadResponseWithRegisterFields(
    const BfRtTable *table,
    const size_t &register_field_size,
    const std::set<bf_rt_id_t> &register_field_id_vec,
    const BfRtTableData *data,
    bfrt_proto::TableData *msg_data) {
  for (const auto &field_id : register_field_id_vec) {
    std::vector<std::vector<uint8_t>> value_vec;
    data->getValue(field_id, &value_vec);
    for (const auto &v_value : value_vec) {
      auto field = msg_data->add_fields();
      // Since we have set the register values using the pointer versions
      // of the setValue versions (which expect the values to be in big
      // endian format), we ensured that the input to those APIs is in
      // big endian format. However, for registers, the getValue API does
      // not expose a pointer version and it just returns a vector of
      // register values. As a result, these register values in the vector
      // are in host byte order. However, since we are setting those values
      // in the stream, we need to ensure that the values are in big endian
      // order
      field->set_field_id(field_id);
      auto register_field_size_bytes = (register_field_size + 7) / 8;
      uint64_t value;
      memcpy(&value, v_value.data(), 8);
      switch (register_field_size) {
        case 1:
        case 8: {
          field->set_stream(&value, register_field_size_bytes);
          break;
        }
        case 16: {
          const auto be_value = htobe16(value);
          field->set_stream(&be_value, register_field_size_bytes);
          break;
        }
        case 32: {
          const auto be_value = htobe32(value);
          field->set_stream(&be_value, register_field_size_bytes);
          break;
        }
        case 64: {
          const auto be_value = htobe64(value);
          field->set_stream(&be_value, register_field_size_bytes);
          break;
        }
        case 128: {
          auto be_value = v_value;
          std::reverse(be_value.begin(), be_value.end());
          field->set_stream(be_value.data(), register_field_size_bytes);
          break;
        }
        default:
          check_and_return(BF_INVALID_ARG,
                           "Invalid register field size field_id:%d table:%s",
                           field_id,
                           table_name_get(table).c_str());
      }
    }
  }
  return Status();
}

Status formResponseKey(const BfRtTable *table,
                       const BfRtTableKey *table_key,
                       bfrt_proto::TableKey *response_key) {
  // Get the list of keyFields associated with the table.
  // For each of the key Field, get the value and set it in the response
  std::vector<bf_rt_id_t> key_fields;
  auto bf_status = table->keyFieldIdListGet(&key_fields);
  check_and_return(bf_status,
                   "ERROR in getting key field id list table:%s",
                   table_name_get(table).c_str());

  for (const auto &field_id : key_fields) {
    auto field = response_key->add_fields();
    field->set_field_id(field_id);
    KeyFieldType type;
    bf_status = table->keyFieldTypeGet(field_id, &type);
    check_and_return(bf_status,
                     "ERROR getting Field type for field %d table:%s",
                     field_id,
                     table_name_get(table).c_str());
    DataType data_type;
    bf_status = table->keyFieldDataTypeGet(field_id, &data_type);
    check_and_return(bf_status,
                     "ERROR getting Field data type for field %d table:%s",
                     field_id,
                     table_name_get(table).c_str());
    size_t size;
    bf_status = table->keyFieldSizeGet(field_id, &size);
    check_and_return(bf_status,
                     "ERROR getting Field size for field %d table:%s",
                     field_id,
                     table_name_get(table).c_str());
    size = (size + 7) / 8;
    switch (type) {
      case KeyFieldType::EXACT: {
        auto exact = field->mutable_exact();
        if (data_type == DataType::STRING) {
          std::string str_value;
          bf_status = table_key->getValue(field_id, &str_value);
          exact->set_value((uint8_t *)str_value.c_str(), str_value.length());
        } else {
          std::vector<uint8_t> value(size);
          bf_status = table_key->getValue(field_id, size, &value[0]);
          exact->set_value(&value[0], size);
        }
      } break;
      case KeyFieldType::TERNARY: {
        std::vector<uint8_t> value(size);
        std::vector<uint8_t> mask(size);
        bf_status =
            table_key->getValueandMask(field_id, size, &value[0], &mask[0]);
        auto ternary = field->mutable_ternary();
        ternary->set_value(&value[0], size);
        ternary->set_mask(&mask[0], size);
      } break;
      case KeyFieldType::LPM: {
        std::vector<uint8_t> value(size);
        uint16_t prefix_len;
        bf_status =
            table_key->getValueLpm(field_id, size, &value[0], &prefix_len);
        auto lpm = field->mutable_lpm();
        lpm->set_value(&value[0], size);
        lpm->set_prefix_len(prefix_len);
      } break;
      case KeyFieldType::RANGE: {
        std::vector<uint8_t> start(size);
        std::vector<uint8_t> end(size);
        bf_status =
            table_key->getValueRange(field_id, size, &start[0], &end[0]);
        auto range = field->mutable_range();
        range->set_low(&start[0], size);
        range->set_high(&end[0], size);
      } break;
      case KeyFieldType::OPTIONAL: {
        std::vector<uint8_t> value(size);
        bool is_valid;
        bf_status =
            table_key->getValueOptional(field_id, size, &value[0], &is_valid);
        auto optional = field->mutable_optional();
        optional->set_value(&value[0], size);
        optional->set_is_valid(is_valid);
      } break;
      default:
        break;
    }
  }
  return Status();
}

Status formResponseData(const BfRtTable *table,
                        const BfRtTableData *data,
                        const std::vector<bf_rt_id_t> &field_id_vec_param,
                        bfrt_proto::TableData *response_data) {
  bf_rt_id_t action_id;
  // Action ID might not be applicable for cases like Counter table
  // So we just want to initialize with 0 in such cases
  bf_status_t bf_status = BF_SUCCESS;

  if (table->actionIdApplicable()) {
    bf_status = data->actionIdGet(&action_id);
    BF_RT_DBGCHK(bf_status == BF_SUCCESS);
    check_and_return(bf_status,
                     "ERROR in getting action id from object for table:%s",
                     table_name_get(table).c_str());
  } else {
    action_id = 0;
  }
  response_data->set_action_id(action_id);

  std::vector<bf_rt_id_t> field_id_vec;
  if (!field_id_vec_param.size()) {
    if (action_id) {
      bf_status = table->dataFieldIdListGet(action_id, &field_id_vec);
    } else {
      bf_status = table->dataFieldIdListGet(&field_id_vec);
    }
    check_and_return(bf_status,
                     "ERROR in getting data field Id list table:%s",
                     table_name_get(table).c_str());
  } else {
    field_id_vec = field_id_vec_param;
  }

  std::set<bf_rt_id_t> register_field_id_vec;
  auto reg_an = Annotation("$bfrt_field_class", "register_data");
  for (const auto &field_id : field_id_vec) {
    // See if this field id is a register
    AnnotationSet annotations{};
    if (action_id) {
      bf_status =
          table->dataFieldAnnotationsGet(field_id, action_id, &annotations);
    } else {
      bf_status = table->dataFieldAnnotationsGet(field_id, &annotations);
    }
    check_and_return(bf_status,
                     "ERROR Field ID %d doesn't exist table:%s",
                     field_id,
                     table_name_get(table).c_str());
    if (annotations.find(reg_an) != annotations.end()) {
      register_field_id_vec.insert(field_id);
    }
  }

  bool register_fields_read = false;
  size_t register_field_size;
  Status grpc_status;
  for (auto const &field_id : field_id_vec) {
    if (register_field_id_vec.find(field_id) != register_field_id_vec.end()) {
      if (!register_fields_read) {
        if (action_id) {
          bf_status = table->dataFieldSizeGet(
              field_id, action_id, &register_field_size);
        } else {
          bf_status = table->dataFieldSizeGet(field_id, &register_field_size);
        }
        check_and_return(bf_status,
                         "ERROR Field ID %d doesn't exist table:%s",
                         field_id,
                         table_name_get(table).c_str());
        // Read all the register fields in one shot and populate the
        // response message
        grpc_status =
            populateReadResponseWithRegisterFields(table,
                                                   register_field_size,
                                                   register_field_id_vec,
                                                   data,
                                                   response_data);
        register_fields_read = true;
      }
      continue;
    } else {
      bool is_active;
      bf_status = data->isActive(field_id, &is_active);
      if (is_active) {
        grpc_status = populateReadResponseWithNonRegisterFields(
            table, action_id, field_id, data, response_data, NULL);
      }
    }
  }  // for loop
  return grpc_status;
}

// This function forms the read response contained in the out param (response)
// for a given table, field ids and retrieved data from BFRT to send to the
// client
Status formulateReadResponse(const BfRtTable &table,
                             const std::vector<bf_rt_id_t> &field_id_vec_param,
                             const BfRtTableKey &key,
                             const BfRtTableData &data,
                             bfrt_proto::TableEntry *response_table_entry) {
  // set up the data in the proto response
  // A Read response consists of forming the key and data
  auto msg_key = response_table_entry->mutable_key();
  auto grpc_status = formResponseKey(&table, &key, msg_key);
  grpc_check_and_return(grpc_status, "Error forming response key");

  // Don't need data if only key was requested
  if (!response_table_entry->table_read_flag().key_only() &&
      !response_table_entry->table_flags().key_only()) {
    auto msg_data = response_table_entry->mutable_data();
    grpc_status = formResponseData(&table, &data, field_id_vec_param, msg_data);
    grpc_check_and_return(grpc_status, "Error forming response Data");
  }

  return Status();
}

Status formulateDefaultEntryReadResponse(
    const BfRtTable &table,
    const std::vector<bf_rt_id_t> &field_id_vec_param,
    const BfRtTableData &data,
    bfrt_proto::TableEntry *response_table_entry) {
  // set up the data in the proto response
  auto msg_data = response_table_entry->mutable_data();

  // A Read response for default entry just  consists of forming the data
  auto grpc_status =
      formResponseData(&table, &data, field_id_vec_param, msg_data);
  grpc_check_and_return(grpc_status, "Error forming response Data");

  return Status();
}

Status make_empty_key(const BfRtTable &table,
                      std::unique_ptr<BfRtTableKey> *table_key) {
  auto bf_status = table.keyAllocate(table_key);
  check_and_return(bf_status,
                   "Table empty key allocation failed. table:%s",
                   table_name_get(&table).c_str());
  return Status();
}

Status make_key(const bfrt_proto::TableKey &key,
                const BfRtTable &table,
                std::unique_ptr<BfRtTableKey> *table_key) {
  bf_status_t bf_status;
  bf_status = table.keyAllocate(table_key);
  check_and_return(bf_status,
                   "Table key allocation failed. table:%s",
                   table_name_get(&table).c_str());

  for (const auto &field : key.fields()) {
    bf_rt_id_t key_field_id = field.field_id();
    switch (field.match_type_case()) {
      case (bfrt_proto::KeyField::kExact): {
        DataType data_type;
        bf_status = table.keyFieldDataTypeGet(key_field_id, &data_type);
        check_and_return(bf_status,
                         "ERROR getting Field data type for field %d table:%s",
                         key_field_id,
                         table_name_get(&table).c_str());
        auto val = field.exact().value();
        if (data_type == DataType::STRING) {
          bf_status =
              (*table_key)
                  ->setValue(key_field_id, std::string(val.data(), val.size()));
        } else {
          bf_status =
              (*table_key)
                  ->setValue(key_field_id,
                             reinterpret_cast<const uint8_t *>(val.data()),
                             val.size());
        }
        break;
      }
      case (bfrt_proto::KeyField::kTernary): {
        auto val = field.ternary().value();
        auto mask = field.ternary().mask();
        bf_status = (*table_key)
                        ->setValueandMask(
                            key_field_id,
                            reinterpret_cast<const uint8_t *>(val.data()),
                            reinterpret_cast<const uint8_t *>(mask.data()),
                            val.size());
        break;
      }
      case (bfrt_proto::KeyField::kLpm): {
        auto val = field.lpm().value();
        bf_status =
            (*table_key)
                ->setValueLpm(key_field_id,
                              reinterpret_cast<const uint8_t *>(val.data()),
                              field.lpm().prefix_len(),
                              val.size());
        break;
      }
      case (bfrt_proto::KeyField::kRange): {
        auto low = field.range().low();
        auto high = field.range().high();
        bf_status =
            (*table_key)
                ->setValueRange(key_field_id,
                                reinterpret_cast<const uint8_t *>(low.data()),
                                reinterpret_cast<const uint8_t *>(high.data()),
                                low.size());
        break;
      }
      case (bfrt_proto::KeyField::kOptional): {
        auto val = field.optional().value();
        bf_status = (*table_key)
                        ->setValueOptional(
                            key_field_id,
                            reinterpret_cast<const uint8_t *>(val.data()),
                            field.optional().is_valid(),
                            val.size());
        break;
      }

      default:
        check_and_return(BF_INVALID_ARG,
                         "Invalid match type table:%s",
                         table_name_get(&table).c_str());
    }

    check_and_return(bf_status,
                     "Failed to create key for field:%d table:%s",
                     key_field_id,
                     table_name_get(&table).c_str());
  }
  return Status();
}

Status table_read_default_entry(const BfRtInfo &info,
                                const BfRtSession &session,
                                const bf_rt_target_t &target,
                                const bfrt_proto::TableEntry &table_entry,
                                bfrt_proto::ReadResponse *response) {
  const BfRtTable *table;
  auto bf_status = info.bfrtTableFromIdGet(table_entry.table_id(), &table);
  check_and_return(
      bf_status, "Table not found for id %u", table_entry.table_id());
  BfRtTable::TableApiSet supported_apis;
  bf_status = table->tableApiSupportedGet(&supported_apis);
  check_and_return(bf_status,
                   "Unable to determine supported APIs for table %s: %d",
                   table_name_get(table).c_str(),
                   bf_status);
  if (std::end(supported_apis) ==
      std::find(std::begin(supported_apis),
                std::end(supported_apis),
                BfRtTable::TableApi::DEFAULT_ENTRY_GET)) {
    // DEFAULT_ENTRY_GET not supported.
    // just return quietly
    return Status();
  }
  // Get Action ID
  bf_rt_id_t action_id;
  action_id = table_entry.data().action_id();

  std::vector<bf_rt_id_t> field_id_vec;
  for (const auto &field : table_entry.data().fields()) {
    bf_rt_id_t data_field_id = field.field_id();
    field_id_vec.push_back(data_field_id);
  }

  // Do data allocate
  std::unique_ptr<BfRtTableData> data_single;
  if (field_id_vec.size()) {
    if (table->actionIdApplicable())
      bf_status = table->dataAllocate(field_id_vec, action_id, &data_single);
    else
      bf_status = table->dataAllocate(field_id_vec, &data_single);
  } else {
    if (table->actionIdApplicable())
      bf_status = table->dataAllocate(action_id, &data_single);
    else
      bf_status = table->dataAllocate(&data_single);
  }
  check_and_return(bf_status,
                   "Data Allocate failed during default entry read table:%s",
                   table_name_get(table).c_str());
  // Set up flag
  uint64_t flags = 0;
  if (table_entry.table_read_flag().from_hw() ||
      table_entry.table_flags().from_hw()) {
    BF_RT_FLAG_SET(flags, BF_RT_FROM_HW);
  }

  bf_status =
      table->tableDefaultEntryGet(session, target, flags, data_single.get());
  if (bf_status == BF_OBJECT_NOT_FOUND) {
    // If the default entry doesn't exist just return
    return Status();
  } else {
    check_and_return(bf_status,
                     "Error getting default entry of table:%s",
                     table_name_get(table).c_str());
  }

  Status grpc_status;
  auto response_table_entry = response->add_entities()->mutable_table_entry();
  response_table_entry->set_is_default_entry(true);
  response_table_entry->set_table_id(table_entry.table_id());

  // Update entry_tgt in response only if the request has entry_tgt under
  // table_entry
  if (table_entry.has_entry_tgt()) {
    response_table_entry->mutable_entry_tgt()->set_device_id(target.dev_id);
    response_table_entry->mutable_entry_tgt()->set_direction(target.direction);
    response_table_entry->mutable_entry_tgt()->set_pipe_id(target.pipe_id);
    response_table_entry->mutable_entry_tgt()->set_prsr_id(target.prsr_id);
  }
  grpc_status = formulateDefaultEntryReadResponse(
      *table, field_id_vec, *data_single, response_table_entry);

  check_and_return(bf_status,
                   "%s:%d %s ERROR in forming read response for default entry",
                   __func__,
                   __LINE__,
                   table_name_get(table).c_str());
  return grpc_status;
}

Status table_read_all(const BfRtInfo &info,
                      const BfRtSession &session,
                      const bf_rt_target_t &target,
                      const bfrt_proto::TableEntry &table_entry,
                      bfrt_proto::ReadResponse *response) {
  LOG_DBG("%s:%d Read all request: %s",
          __func__,
          __LINE__,
          table_entry.DebugString().c_str());
  // 1. Get table
  const BfRtTable *table;
  auto bf_status = info.bfrtTableFromIdGet(table_entry.table_id(), &table);
  check_and_return(
      bf_status, "Table not found for id %u", table_entry.table_id());

  // 2 Prepare for tableEntryGetFirst
  // Allocate key for an out param for tableEntryGetFirst
  std::unique_ptr<BfRtTableKey> table_key;
  bf_status = table->keyAllocate(&table_key);
  check_and_return(bf_status,
                   "Table key allocation failed. table:%s",
                   table_name_get(table).c_str());

  // Get Action ID
  bf_rt_id_t action_id;
  action_id = table_entry.data().action_id();

  // get list of field_ids if any from protobuf msg
  std::vector<bf_rt_id_t> field_id_vec;
  for (const auto &field : table_entry.data().fields()) {
    // The request can contain either name or ID. Act accordingly
    bf_rt_id_t data_field_id = field.field_id();
    field_id_vec.push_back(data_field_id);
  }

  // Allocate data for first entry
  // We do not care about the action_id for first entry
  // because we do not want it to fail due to a filter.
  // We can filter it out of the response later
  std::unique_ptr<BfRtTableData> data_single;
  if (field_id_vec.size()) {
    bf_status = table->dataAllocate(field_id_vec, &data_single);
  } else {
    bf_status = table->dataAllocate(&data_single);
  }

  check_and_return(bf_status,
                   "Data Allocate failed during entry read table:%s",
                   table_name_get(table).c_str());
  // Set up flag
  uint64_t flags = 0;
  if (table_entry.table_read_flag().from_hw() ||
      table_entry.table_flags().from_hw()) {
    BF_RT_FLAG_SET(flags, BF_RT_FROM_HW);
  }
  // 3. Get first entry. If fails with OBJ_NOT_FOUND then just return
  // success since we don't want to send an error on empty table
  bf_status = table->tableEntryGetFirst(
      session, target, flags, table_key.get(), data_single.get());
  if (bf_status == BF_OBJECT_NOT_FOUND) {
    return Status();
  } else {
    check_and_return(bf_status,
                     "Error getting first entry of table:%s",
                     table_name_get(table).c_str());
  }
  // 4. Add first entry to readResponse but only if the recvd action_id is
  // the same as the requested one
  bf_rt_id_t actual_action_id;
  bf_status = data_single->actionIdGet(&actual_action_id);
  Status grpc_status;
  if (!action_id || (action_id && actual_action_id == action_id)) {
    auto response_table_entry = response->add_entities()->mutable_table_entry();
    response_table_entry->set_table_id(table_entry.table_id());

    // Update entry_tgt in response only if the request has entry_tgt under
    // table_entry
    if (table_entry.has_entry_tgt()) {
      response_table_entry->mutable_entry_tgt()->set_device_id(target.dev_id);
      response_table_entry->mutable_entry_tgt()->set_direction(
          target.direction);
      response_table_entry->mutable_entry_tgt()->set_pipe_id(target.pipe_id);
      response_table_entry->mutable_entry_tgt()->set_prsr_id(target.prsr_id);
    }

    grpc_status = formulateReadResponse(
        *table, field_id_vec, *table_key, *data_single, response_table_entry);
    grpc_check_and_return(grpc_status, "Error forming Read response");
  }

  // 5. Get table usage
  uint32_t n = 0;
  BfRtTable::TableApiSet supported_apis;
  bf_status = table->tableApiSupportedGet(&supported_apis);
  check_and_return(bf_status,
                   "Unable to determine supported APIs for table %s: %d",
                   table_name_get(table).c_str(),
                   bf_status);
  if (std::end(supported_apis) == std::find(std::begin(supported_apis),
                                            std::end(supported_apis),
                                            BfRtTable::TableApi::USAGE_GET)) {
    size_t tbl_size = 0;
    // For some tables usage is not applicable, in which case getAll
    // needs to get as many entries as the size of the table.
    // Examples of such tables are counters, meters, LPF, WRED, register
    bf_status = table->tableSizeGet(session, target, &tbl_size);
    n = tbl_size;
  } else {
    bf_status = table->tableUsageGet(session, target, flags, &n);
  }

  // If the reported usage is zero (which it should not be since get-first was
  // successful) report an error before we go down to step 6 and allocate a
  // vector of size equal to unsigned negative one.  If the status was good but
  // the user is zero report an unexpected error code, otherwise let the
  // check_and_return macro error out with the exising error code.
  if (bf_status == BF_SUCCESS && n == 0) bf_status = BF_UNEXPECTED;

  check_and_return(
      bf_status, "Unable to get usage table:%s", table_name_get(table).c_str());

  // If usage is 1, then return success from here
  if (n == 1) {
    return Status();
  }
  // 6 Prepare for get_all_entries
  // Allocate data and key for the n
  unsigned i = 0;
  BfRtTable::keyDataPairs key_data_pairs;
  std::vector<std::unique_ptr<BfRtTableKey>> keys(n - 1);
  std::vector<std::unique_ptr<BfRtTableData>> data(n - 1);

  // Make vector of pair of out params for tableEntryGetNext_n
  for (i = 0; i < n - 1; i++) {
    bf_status = table->keyAllocate(&keys[i]);
    check_and_return(bf_status,
                     "Key Allocate failed for table:%s",
                     table_name_get(table).c_str());
    if (action_id == 0) {
      if (field_id_vec.size()) {
        bf_status = table->dataAllocate(field_id_vec, &data[i]);
      } else {
        bf_status = table->dataAllocate(&data[i]);
      }
    } else {
      if (field_id_vec.size()) {
        bf_status = table->dataAllocate(field_id_vec, action_id, &data[i]);
      } else {
        bf_status = table->dataAllocate(action_id, &data[i]);
      }
    }
    check_and_return(bf_status,
                     "Data Allocate failed for table:%s",
                     table_name_get(table).c_str());
    key_data_pairs.push_back(std::make_pair(keys[i].get(), data[i].get()));
  }

  // 7 Get next n entries
  uint32_t num_returned = 0;
  bf_status = table->tableEntryGetNext_n(session,
                                         target,
                                         flags,
                                         *table_key.get(),
                                         n - 1,
                                         &key_data_pairs,
                                         &num_returned);

  // BF_OBJECT_NOT_FOUND is not an error here, it is returned in case the
  // function didn't find all 'n - 1' entries, which is possible and this
  // is not an error. In such case we shall use num_returned as a number
  // of entries really read from the table.
  if (bf_status != BF_OBJECT_NOT_FOUND) {
    check_and_return(bf_status,
                     "Get last %d entries failed table:%s",
                     n - 1,
                     table_name_get(table).c_str());
  }

  // 8 Formulate read response for all the n entries
  for (i = 0; i < num_returned; i++) {
    // Start constructing the proto response
    auto response_table_entry = response->add_entities()->mutable_table_entry();
    response_table_entry->set_table_id(table_entry.table_id());

    // Update entry_tgt in response only if the request has entry_tgt under
    // table_entry
    if (table_entry.has_entry_tgt()) {
      response_table_entry->mutable_entry_tgt()->set_device_id(target.dev_id);
      response_table_entry->mutable_entry_tgt()->set_direction(
          target.direction);
      response_table_entry->mutable_entry_tgt()->set_pipe_id(target.pipe_id);
      response_table_entry->mutable_entry_tgt()->set_prsr_id(target.prsr_id);
    }

    if ((key_data_pairs[i].second) == nullptr) {
      continue;
    }
    grpc_status = formulateReadResponse(*table,
                                        field_id_vec,
                                        *(key_data_pairs[i].first),
                                        *(key_data_pairs[i].second),
                                        response_table_entry);
    if (grpc_status.error_code() != grpc::OK) {
      LOG_ERROR("%s:%d ERROR in forming read response for the %dth entry",
                __func__,
                __LINE__,
                i + 2);
      return grpc_status;
    }
  }

  // 9 Debug log of returned values.
  if (grpc_status.error_code() == grpc::OK) {
    LOG_DBG("%s:%d Sending read all reply for %d entries : \n%s",
            __func__,
            __LINE__,
            n,
            response->DebugString().c_str());
  }

  return Status();
}

Status table_read(const BfRtInfo &info,
                  const BfRtSession &session,
                  const bf_rt_target_t &target,
                  const bfrt_proto::TableEntry &table_entry,
                  bfrt_proto::ReadResponse *response) {
  const BfRtTable *table;
  auto bf_status = info.bfrtTableFromIdGet(table_entry.table_id(), &table);

  check_and_return(
      bf_status, "Table not found for id %u", table_entry.table_id());

  LOG_DBG("%s:%d Read request (%s): %s",
          __func__,
          __LINE__,
          table_name_get(table).c_str(),
          table_entry.DebugString().c_str());
  // Construct bfrt::BfRtTableKey from bfrt_proto::TableKey
  std::unique_ptr<BfRtTableKey> table_key;
  bf_rt_target_t entry_tgt = target;

  Status grpc_status;
  if (table_entry.table_read_flag().key_only() ||
      table_entry.table_flags().key_only()) {
    grpc_status = make_empty_key(*table, &table_key);
  } else {
    grpc_status = make_key(table_entry.key(), *table, &table_key);
  }
  if (grpc_status.error_code() != grpc::OK) {
    return grpc_status;
  }

  bf_rt_id_t action_id;
  action_id = table_entry.data().action_id();
  // get list of field_ids if any from protobuf msg
  std::vector<bf_rt_id_t> field_id_vec;
  for (const auto &field : table_entry.data().fields()) {
    // The request can contain either name or ID. Act accordingly
    bf_rt_id_t data_field_id = field.field_id();
    field_id_vec.push_back(data_field_id);
  }

  // Set up flag
  uint64_t flags = 0;
  if (table_entry.table_read_flag().from_hw() ||
      table_entry.table_flags().from_hw()) {
    BF_RT_FLAG_SET(flags, BF_RT_FROM_HW);
  }
  std::unique_ptr<BfRtTableData> data_single;
  bf_rt_handle_t handle_id =
      static_cast<bf_rt_handle_t>(table_entry.handle_id());

  if (!table_entry.table_read_flag().key_only() &&
      !table_entry.table_flags().key_only()) {
    // dataAllocate
    if (action_id == 0) {
      if (field_id_vec.size()) {
        bf_status = table->dataAllocate(field_id_vec, &data_single);
      } else {
        bf_status = table->dataAllocate(&data_single);
      }
    } else {
      if (field_id_vec.size()) {
        bf_status = table->dataAllocate(field_id_vec, action_id, &data_single);
      } else {
        bf_status = table->dataAllocate(action_id, &data_single);
      }
    }
    check_and_return(bf_status,
                     "Data Allocate failed during entry read table:%s",
                     table_name_get(table).c_str());
  }

  if (table_entry.value_case() == 7) {
    if (table_entry.table_read_flag().key_only() ||
        table_entry.table_flags().key_only()) {
      bf_status = table->tableEntryKeyGet(
          session, target, 0, handle_id, &entry_tgt, table_key.get());
      check_and_return(bf_status,
                       "Entry not found in table:%s",
                       table_name_get(table).c_str());
    } else {
      bf_status = table->tableEntryGet(session,
                                       target,
                                       flags,
                                       handle_id,
                                       table_key.get(),
                                       data_single.get());
      check_and_return(bf_status,
                       "Entry not found in table:%s",
                       table_name_get(table).c_str());
    }
  } else {
    bf_status = table->tableEntryGet(
        session, target, flags, *(table_key.get()), data_single.get());
    check_and_return(bf_status,
                     "Entry not found in table:%s",
                     table_name_get(table).c_str());
  }
  // Start constructing the proto response
  auto response_table_entry = response->add_entities()->mutable_table_entry();
  response_table_entry->set_table_id(table_entry.table_id());
  response_table_entry->mutable_table_read_flag()->set_key_only(
      table_entry.table_read_flag().key_only());
  response_table_entry->mutable_table_flags()->set_key_only(
      table_entry.table_flags().key_only());

  // Update entry_tgt in response only if the request has entry_tgt under
  // table_entry
  if (table_entry.has_entry_tgt() ||
      (table_entry.value_case() == 7 &&
       (table_entry.table_read_flag().key_only() ||
        table_entry.table_flags().key_only()))) {
    response_table_entry->mutable_entry_tgt()->set_device_id(entry_tgt.dev_id);
    response_table_entry->mutable_entry_tgt()->set_direction(
        entry_tgt.direction);
    response_table_entry->mutable_entry_tgt()->set_pipe_id(entry_tgt.pipe_id);
    response_table_entry->mutable_entry_tgt()->set_prsr_id(entry_tgt.prsr_id);
  }

  grpc_status = formulateReadResponse(*table,
                                      field_id_vec,
                                      *(table_key.get()),
                                      *data_single,
                                      response_table_entry);

  if (grpc_status.error_code() == grpc::OK) {
    LOG_DBG("%s:%d Table:%s Sending read reply: %s",
            __func__,
            __LINE__,
            table_name_get(table).c_str(),
            response_table_entry->DebugString().c_str());
  }

  return grpc_status;
}

Status read_entry(const BfRtInfo &info,
                  const BfRtSession &session,
                  const bf_rt_target_t &target,
                  const bfrt_proto::TableEntry &table_entry,
                  bfrt_proto::ReadResponse *response) {
  // If key doesn't exist then get all entries
  Status grpc_status = Status();
  if (table_entry.value_case()) {
    grpc_status = table_read(info, session, target, table_entry, response);
  } else {
    if (table_entry.is_default_entry()) {
      grpc_status = table_read_default_entry(
          info, session, target, table_entry, response);
    } else {
      // Even though the read_all might fail, continue reading the default
      // entry before checking and returning error
      auto grpc_all_status =
          table_read_all(info, session, target, table_entry, response);
      auto grpc_default_status = table_read_default_entry(
          info, session, target, table_entry, response);
      grpc_check_and_return(grpc_all_status, "Error reading all entries");
      // Ignoring default entry read errors with wildcard read
    }
  }
  return grpc_status;
}

Status read_usage(const BfRtInfo &info,
                  const BfRtSession &session,
                  const bf_rt_target_t &target,
                  const bfrt_proto::TableUsage &request,
                  bfrt_proto::ReadResponse *response) {
  const BfRtTable *table;
  auto bf_status = info.bfrtTableFromIdGet(request.table_id(), &table);
  check_and_return(bf_status, "Unable to find table");

  LOG_DBG("%s:%d Table Usage read req (%s): %s",
          __func__,
          __LINE__,
          table_name_get(table).c_str(),
          request.DebugString().c_str());

  uint32_t count;
  // Set up flag
  uint64_t flags = 0;
  if (request.table_read_flag().from_hw() || request.table_flags().from_hw()) {
    BF_RT_FLAG_SET(flags, BF_RT_FROM_HW);
  }
  bf_status = table->tableUsageGet(session, target, flags, &count);
  check_and_return(bf_status, "Unable to get usage");

  // Start constructing the proto response
  auto read_table_usage_response =
      response->add_entities()->mutable_table_usage();
  read_table_usage_response->set_table_id(request.table_id());
  read_table_usage_response->set_usage(count);

  return Status();
}
Status read_attribute(const BfRtInfo &info,
                      const BfRtSession &session,
                      const bf_rt_target_t &target,
                      const bfrt_proto::TableAttribute &request,
                      bfrt_proto::ReadResponse *response) {
  const auto table_id = request.table_id();
  const BfRtTable *table;
  auto bf_status = info.bfrtTableFromIdGet(table_id, &table);
  check_and_return(bf_status, "Table not found.");

  Status grpc_status;
  switch (request.attribute_case()) {
    case (bfrt_proto::TableAttribute::kIdleTable): {
      grpc_status = getAttributeIdleTable(session, target, *table, response);
      break;
    }
    case (bfrt_proto::TableAttribute::kEntryScope): {
      grpc_status = getAttributeEntryScope(session, target, *table, response);
      break;
    }
    case (bfrt_proto::TableAttribute::kDynKeyMask): {
      grpc_status = getAttributeDynKeyMask(session, target, *table, response);
      break;
    }
    case (bfrt_proto::TableAttribute::kDynHashing): {
      grpc_status = getAttributeDynHashing(session, target, *table, response);
      break;
    }
    case (bfrt_proto::TableAttribute::kByteCountAdj): {
      grpc_status =
          getAttributeMeterByteCountAdj(session, target, *table, response);
      break;
    }
    case (bfrt_proto::TableAttribute::kPortStatusNotify): {
      grpc_status =
          getAttributePortStatusChgNotify(session, target, *table, response);
      break;
    }
    case (bfrt_proto::TableAttribute::kIntvlMs): {
      grpc_status =
          getAttributePortStatIntvl(session, target, *table, response);
      break;
    }
    case (bfrt_proto::TableAttribute::kPreDeviceConfig): {
      grpc_status =
          getAttributePreDeviceConfig(session, target, *table, response);
      break;
    }
    case (bfrt_proto::TableAttribute::kSelectorUpdateNotify): {
      grpc_status = getAttributeSelectorUpdateChgNotify(
          session, target, *table, response);
      break;
    }
    default:
      grpc_status = to_grpc_status(BF_INVALID_ARG, "Invalid Attribute Type.");
  }
  return grpc_status;
}

Status read_handle(const BfRtInfo &info,
                   const BfRtSession &session,
                   const bf_rt_target_t &target,
                   const bfrt_proto::HandleId &request,
                   bfrt_proto::ReadResponse *response) {
  Status grpc_status;
  if (!request.has_key()) {
    grpc_status = to_grpc_status(
        BF_INVALID_ARG, "Invalid key is mandatory for get handle operation.");
  } else {
    const BfRtTable *table;
    auto bf_status = info.bfrtTableFromIdGet(request.table_id(), &table);

    check_and_return(
        bf_status, "Table %s not found.", table_name_get(table).c_str());

    LOG_DBG("%s:%d Read request (%s): %s",
            __func__,
            __LINE__,
            table_name_get(table).c_str(),
            request.DebugString().c_str());
    // Construct bfrt::BfRtTableKey from bfrt_proto::TableKey
    std::unique_ptr<BfRtTableKey> table_key;
    grpc_status = make_key(request.key(), *table, &table_key);
    if (grpc_status.error_code() != grpc::OK) {
      return grpc_status;
    }
    bf_rt_handle_t handle_id = 0;
    bf_status = table->tableEntryHandleGet(
        session, target, 0, *(table_key.get()), &handle_id);
    check_and_return(bf_status,
                     "Entry not found in table:%s",
                     table_name_get(table).c_str());
    auto read_handle_response = response->add_entities()->mutable_handle();
    read_handle_response->set_handle_id(handle_id);
    read_handle_response->set_table_id(request.table_id());
  }
  return grpc_status;
}

Status read_id(const BfRtInfo &info,
               const bfrt_proto::ObjectId &request,
               bfrt_proto::ReadResponse *response) {
  bf_rt_id_t id = 0;
  bf_status_t bf_status = BF_SUCCESS;
  switch (request.object_case()) {
    case bfrt_proto::ObjectId::kTableObject: {
      const BfRtTable *table;
      auto &table_msg = request.table_object();
      bf_status = info.bfrtTableFromNameGet(table_msg.table_name(), &table);
      check_and_return(bf_status, "Table not found.");
      switch (table_msg.names_case()) {
        case bfrt_proto::ObjectId::TableObject::kActionName: {
          bf_status = table->actionIdGet(table_msg.action_name().action(), &id);
          break;
        }
        case bfrt_proto::ObjectId::TableObject::kDataFieldName: {
          bf_rt_id_t action_id;
          if (table_msg.data_field_name().action() != "") {
            table->actionIdGet(table_msg.data_field_name().action(),
                               &action_id);
            table->dataFieldIdGet(
                table_msg.data_field_name().field(), action_id, &id);
          } else {
            table->dataFieldIdGet(table_msg.data_field_name().field(), &id);
          }
          break;
        }
        case bfrt_proto::ObjectId::TableObject::kKeyFieldName: {
          table->keyFieldIdGet(table_msg.key_field_name().field(), &id);
          break;
        }
        default: {
          bf_status = table->tableIdGet(&id);
          break;
        }
      }
      break;
    }
    case bfrt_proto::ObjectId::kLearnObject: {
      const BfRtLearn *learn_obj;
      auto &learn_msg = request.learn_object();
      bf_status = info.bfrtLearnFromNameGet(learn_msg.learn_name(), &learn_obj);
      check_and_return(bf_status, "Learn Obj not found.");
      if (learn_msg.data_field_name().action() != "") {
        check_and_return(BF_INVALID_ARG,
                         "Action IDs not applicable for Learn objs");
      }
      bf_status =
          learn_obj->learnFieldIdGet(learn_msg.data_field_name().field(), &id);
      check_and_return(bf_status, "Failed to get Learn ID");
      break;
    }
    default: {
      bf_status = BF_INVALID_ARG;
      check_and_return(bf_status, "Wrong Option for ID get");
      break;
    }
  }

  check_and_return(bf_status, "Id get faild.");

  // Start constructing the proto response
  auto read_object_id_response = response->add_entities()->mutable_object_id();
  read_object_id_response->set_id(id);
  return Status();
}

Status make_empty_data(const BfRtTable &table,
                       std::unique_ptr<BfRtTableData> *table_data) {
  auto bf_status = table.dataAllocate(table_data);
  check_and_return(bf_status,
                   "ERROR Unable to allocate empty data object for table : %s",
                   table_name_get(&table).c_str());
  return Status();
}

Status set_data_field(const BfRtTable &table,
                      const bfrt_proto::DataField &field,
                      BfRtTableData *table_data) {
  bf_status_t bf_status = BF_SUCCESS;
  bf_rt_id_t data_field_id = field.field_id();
  switch (field.value_case()) {
    case bfrt_proto::DataField::kStream: {
      const auto &val = field.stream();
      bf_status =
          table_data->setValue(data_field_id,
                               reinterpret_cast<const uint8_t *>(val.data()),
                               val.size());
      break;
    }
    case bfrt_proto::DataField::kBoolVal: {
      const auto &val = field.bool_val();
      bf_status = table_data->setValue(data_field_id, val);
      break;
    }
    case bfrt_proto::DataField::kFloatVal: {
      const auto &val = field.float_val();
      bf_status = table_data->setValue(data_field_id, val);
      break;
    }
    case bfrt_proto::DataField::kStrVal: {
      const auto &val = field.str_val();
      bf_status = table_data->setValue(data_field_id, val);
      break;
    }
    case bfrt_proto::DataField::kIntArrVal: {
      std::vector<bf_rt_id_t> val;
      std::copy(field.int_arr_val().val().begin(),
                field.int_arr_val().val().end(),
                std::back_inserter(val));
      bf_status = table_data->setValue(data_field_id, val);
      break;
    }
    case bfrt_proto::DataField::kStrArrVal: {
      std::vector<std::string> val;
      std::copy(field.str_arr_val().val().begin(),
                field.str_arr_val().val().end(),
                std::back_inserter(val));
      bf_status = table_data->setValue(data_field_id, val);
      break;
    }
    case bfrt_proto::DataField::kBoolArrVal: {
      std::vector<bool> val;
      std::copy(field.bool_arr_val().val().begin(),
                field.bool_arr_val().val().end(),
                std::back_inserter(val));
      bf_status = table_data->setValue(data_field_id, val);
      break;
    }
    case bfrt_proto::DataField::kContainerArrVal: {
      // Create a vector of data unique_ptrs for inner data
      std::vector<std::unique_ptr<BfRtTableData>> inner_data_vec;
      for (const auto &container : field.container_arr_val().container()) {
        std::unique_ptr<BfRtTableData> inner_data;
        bf_status = table.dataAllocateContainer(data_field_id, &inner_data);
        check_and_return(bf_status,
                         "DataAllocateContainer failed for field:%d table:%s",
                         data_field_id,
                         table_name_get(&table).c_str());
        // Now recursively process all inner data fields
        for (const auto &inner_field : container.val()) {
          auto grpc_status =
              set_data_field(table, inner_field, inner_data.get());
          grpc_check_and_return(grpc_status,
                                "Table data field set failed for table : %s",
                                table_name_get(&table).c_str());
        }
        inner_data_vec.push_back(std::move(inner_data));
      }
      bf_status =
          table_data->setValue(data_field_id, std::move(inner_data_vec));
      break;
    }
    default:
      break;
  }
  check_and_return(bf_status,
                   "SetValue failed for field:%d table:%s",
                   data_field_id,
                   table_name_get(&table).c_str());
  return Status();
}

Status make_data(const BfRtTable &table,
                 const bfrt_proto::TableEntry &table_entry,
                 std::unique_ptr<BfRtTableData> *table_data) {
  // Construct bfrt::BfRtTableData from bfrt_proto::TableData
  BfRtTable::TableType type;
  bf_status_t bf_status = table.tableTypeGet(&type);
  check_and_return(bf_status,
                   "ERROR Unable to get type of the table : %s",
                   table_name_get(&table).c_str());
  bf_rt_id_t action_id = table_entry.data().action_id();
  switch (type) {
    case BfRtTable::TableType::PORT_CFG:
    case BfRtTable::TableType::PORT_STAT:
    case BfRtTable::TableType::PORT_HDL_INFO:
    case BfRtTable::TableType::PORT_FRONT_PANEL_IDX_INFO:
    case BfRtTable::TableType::PORT_STR_INFO:
    case BfRtTable::TableType::PKTGEN_PORT_CFG:
    case BfRtTable::TableType::PKTGEN_APP_CFG:
    case BfRtTable::TableType::PKTGEN_PKT_BUFF_CFG:
    case BfRtTable::TableType::PKTGEN_PORT_MASK_CFG:
    case BfRtTable::TableType::PKTGEN_PORT_DOWN_REPLAY_CFG:
    case BfRtTable::TableType::MATCH_DIRECT:
    case BfRtTable::TableType::ACTION_PROFILE:
    case BfRtTable::TableType::MATCH_INDIRECT:
    case BfRtTable::TableType::MATCH_INDIRECT_SELECTOR:
    case BfRtTable::TableType::SELECTOR:
    case BfRtTable::TableType::TM_QUEUE_CFG:
    case BfRtTable::TableType::TM_QUEUE_MAP:
    case BfRtTable::TableType::TM_QUEUE_COLOR:
    case BfRtTable::TableType::TM_QUEUE_BUFFER:
    case BfRtTable::TableType::TM_QUEUE_SCHED_CFG:
    case BfRtTable::TableType::TM_QUEUE_SCHED_SHAPING:
    case BfRtTable::TableType::TM_L1_NODE_SCHED_CFG:
    case BfRtTable::TableType::TM_L1_NODE_SCHED_SHAPING:
    case BfRtTable::TableType::TM_PIPE_CFG:
    case BfRtTable::TableType::TM_PIPE_SCHED_CFG:
    case BfRtTable::TableType::TM_PORT_SCHED_CFG:
    case BfRtTable::TableType::TM_PORT_SCHED_SHAPING:
    case BfRtTable::TableType::TM_MIRROR_DPG:
    case BfRtTable::TableType::TM_PORT_DPG:
    case BfRtTable::TableType::TM_PPG_CFG:
    case BfRtTable::TableType::TM_PORT_CFG:
    case BfRtTable::TableType::TM_PORT_BUFFER:
    case BfRtTable::TableType::TM_PORT_FLOWCONTROL:
    case BfRtTable::TableType::TM_PORT_GROUP_CFG:
    case BfRtTable::TableType::TM_PORT_GROUP:
    case BfRtTable::TableType::DYN_HASH_ALGO:
    case BfRtTable::TableType::MIRROR_CFG:
    case BfRtTable::TableType::DEV_CFG:
    case BfRtTable::TableType::DEV_WARM_INIT:
    case BfRtTable::TableType::TM_POOL_CFG:
    case BfRtTable::TableType::TM_POOL_SKID:
    case BfRtTable::TableType::TM_POOL_APP:
    case BfRtTable::TableType::TM_POOL_COLOR:
    case BfRtTable::TableType::TM_POOL_APP_PFC:
    case BfRtTable::TableType::TM_COUNTER_IG_PORT:
    case BfRtTable::TableType::TM_COUNTER_EG_PORT:
    case BfRtTable::TableType::TM_COUNTER_QUEUE:
    case BfRtTable::TableType::TM_COUNTER_POOL:
    case BfRtTable::TableType::TM_COUNTER_PIPE:
    case BfRtTable::TableType::TM_COUNTER_PORT_DPG:
    case BfRtTable::TableType::TM_COUNTER_MIRROR_PORT_DPG:
    case BfRtTable::TableType::TM_COUNTER_PPG:
    case BfRtTable::TableType::TM_CFG:
    case BfRtTable::TableType::TM_PIPE_MULTICAST_FIFO: {
      if (action_id == 0) {
        std::vector<bf_rt_id_t> all_fields;
        bf_status = table.dataFieldIdListGet(&all_fields);
        check_and_return(bf_status,
                         "ERROR Unable to get data field list for table : %s",
                         table_name_get(&table).c_str());
        if (static_cast<uint32_t>(table_entry.data().fields().size()) ==
            (all_fields.size())) {
          // configure all data fields
          bf_status = table.dataAllocate(table_data);
        } else {
          // configure part of the data fields
          std::vector<bf_rt_id_t> fields(table_entry.data().fields().size(), 0);
          std::transform(table_entry.data().fields().begin(),
                         table_entry.data().fields().end(),
                         fields.begin(),
                         [](const ::bfrt_proto::DataField &df) -> bf_rt_id_t {
                           return df.field_id();
                         });
          bf_status = table.dataAllocate(fields, table_data);
        }
      } else {
        // action id != 0
        std::vector<bf_rt_id_t> all_fields;
        bf_status = table.dataFieldIdListGet(action_id, &all_fields);
        check_and_return(bf_status,
                         "ERROR Unable to get data field list for table : %s",
                         table_name_get(&table).c_str());
        if (static_cast<uint32_t>(table_entry.data().fields().size()) ==
            all_fields.size()) {
          // configure all data fields
          bf_status = table.dataAllocate(action_id, table_data);
        } else {
          // configure part of the data fields
          std::vector<bf_rt_id_t> fields(table_entry.data().fields().size(), 0);
          std::transform(table_entry.data().fields().begin(),
                         table_entry.data().fields().end(),
                         fields.begin(),
                         [](const ::bfrt_proto::DataField &df) -> bf_rt_id_t {
                           return df.field_id();
                         });
          bf_status = table.dataAllocate(fields, action_id, table_data);
        }
      }
      break;
    }
    default: {
      // The other tables do not support data allocate with a list of fields
      if (action_id == 0) {
        bf_status = table.dataAllocate(table_data);
      } else {
        bf_status = table.dataAllocate(action_id, table_data);
      }
    }
  }
  check_and_return(bf_status,
                   "Table data allocation failed for table : %s",
                   table_name_get(&table).c_str());
  for (const auto &field : table_entry.data().fields()) {
    auto grpc_status = set_data_field(table, field, table_data->get());
    grpc_check_and_return(grpc_status,
                          "Table data field set failed for table : %s",
                          table_name_get(&table).c_str());
  }
  return Status();
}

Status write_entry(const BfRtInfo &info,
                   const BfRtSession &session,
                   const bf_rt_target_t &target,
                   const bfrt_proto::Update_Type &update,
                   const bfrt_proto::TableEntry &table_entry,
                   const bool ignore_not_found,
                   bfrt_proto::WriteResponse *response) {
  const BfRtTable *table;
  auto bf_status = info.bfrtTableFromIdGet(table_entry.table_id(), &table);

  check_and_return(
      bf_status, "Table not found for id %u", table_entry.table_id());

  LOG_DBG("%s:%d Table:%s Update_type:%d Write request:\n%s",
          __func__,
          __LINE__,
          table_name_get(table).c_str(),
          int(update),
          table_entry.DebugString().c_str());

  if (table_entry.is_default_entry()) {
    if (!table_entry.key().fields().empty()) {
      check_and_return(BF_INVALID_ARG, "Non-empty key for default entry");
    }
  }
  // Construct bfrt::BfRtTableKey from bfrt_proto::TableKey
  std::unique_ptr<BfRtTableKey> table_key = nullptr;
  if (table_entry.has_key()) {
    auto grpc_status = make_key(table_entry.key(), *table, &table_key);
    if (grpc_status.error_code() != grpc::OK) {
      return grpc_status;
    }
  }

  auto grpc_status = Status();
  std::unique_ptr<BfRtTableData> table_data = nullptr;
  if (table_entry.has_data()) {
    // Construct bfrt::BfRtTableData from bfrt_proto::TableData
    grpc_status = make_data(*table, table_entry, &table_data);
    if (grpc_status.error_code() != grpc::OK) {
      return grpc_status;
    }
  } else {
    // If table entry has no data, then try and make an empty
    // data object. This can only work for tables which support
    // creating data objects with no action ID.
    grpc_status = make_empty_data(*table, &table_data);
    if (grpc_status.error_code() != grpc::OK) {
      return grpc_status;
    }
  }

  std::string error_msg;
  uint64_t flags = 0;
  switch (update) {
    case bfrt_proto::Update_Type_UNSPECIFIED:
      check_and_return(BF_INVALID_ARG, "Unspecified update type");
      break;
    case bfrt_proto::Update_Type_INSERT:
      if (table_entry.is_default_entry()) {
        error_msg =
            "INSERT of default entry is not supported. Please use MODIFY "
            "instead";
        bf_status = BF_NOT_SUPPORTED;
      } else {
        if (!table_key) {
          error_msg = "Table key cannot be empty";
          bf_status = BF_INVALID_ARG;
          break;
        }
        if (!table_data) {
          error_msg = "Table data cannot be empty";
          bf_status = BF_INVALID_ARG;
          break;
        }
        bf_status = table->tableEntryAdd(
            session, target, 0, *(table_key.get()), *(table_data.get()));
        error_msg = "Table Add failed";
      }
      break;
    case bfrt_proto::Update_Type_MODIFY:
      // Set up reset flags
      if (!table_entry.table_flags().reset_ttl()) {
        BF_RT_FLAG_SET(flags, BF_RT_SKIP_TTL_RESET);
      }
      if (!table_entry.table_flags().reset_stats()) {
        BF_RT_FLAG_SET(flags, BF_RT_SKIP_STAT_RESET);
      }
      if (table_entry.is_default_entry()) {
        if (!table_data) {
          error_msg = "Table data cannot be empty";
          bf_status = BF_INVALID_ARG;
          break;
        }
        bf_status =
            table->tableDefaultEntrySet(session, target, *(table_data.get()));
        error_msg = "Table Default entry set failed";
      } else {
        if (!table_key) {
          error_msg = "Table key cannot be empty";
          bf_status = BF_INVALID_ARG;
          break;
        }
        if (!table_data) {
          error_msg = "Table data cannot be empty";
          bf_status = BF_INVALID_ARG;
          break;
        }
        bf_status = table->tableEntryMod(
            session, target, flags, *(table_key.get()), *(table_data.get()));
        error_msg = "Table Entry modify failed";
      }
      break;
    case bfrt_proto::Update_Type_MODIFY_INC: {
      // Set up modify incremental flag, keep MOD_INC_DELETE
      // support.
      if (table_entry.table_mod_inc_flag().type() ==
              bfrt_proto::TableModIncFlag_Type_MOD_INC_DELETE ||
          table_entry.table_flags().mod_del()) {
        BF_RT_FLAG_SET(flags, BF_RT_INC_DEL);
      }
      if (!table_key) {
        error_msg = "Table key cannot be empty";
        bf_status = BF_INVALID_ARG;
        break;
      }
      if (!table_data) {
        error_msg = "Table data cannot be empty";
        bf_status = BF_INVALID_ARG;
        break;
      }
      bf_status = table->tableEntryModInc(
          session, target, flags, *table_key, *table_data);
      error_msg = "Table Entry modify incremental failed";
    } break;
    case bfrt_proto::Update_Type_DELETE:
      if (table_entry.is_default_entry()) {
        bf_status = table->tableDefaultEntryReset(session, target);
        error_msg = "Table Default Entry reset failed";
      } else {
        if (table_key) {
          if (ignore_not_found) {
            BF_RT_FLAG_SET(flags, BF_RT_IGNORE_NOT_FOUND);
          }
          bf_status = table->tableEntryDel(session, target, flags, *table_key);
          error_msg = "Table Entry delete failed";
        } else {
          bf_status = table->tableClear(session, target);
          error_msg = "Table Entry delete all failed";
        }
      }
      break;
    case bfrt_proto::Update_Type_RESET:
      bf_status = table->tableEntryReset(session, target, 0, *table_key);
      error_msg = "Table Default Entry reset failed";
      break;
    case bfrt_proto::Update_Type_INSERT_OR_MODIFY: {
      if (!table_key) {
        error_msg = "Table key cannot be empty";
        bf_status = BF_INVALID_ARG;
        break;
      }
      bool has_added = false;
      bf_status = table->tableEntryAddOrMod(session,
                                            target,
                                            0,
                                            *(table_key.get()),
                                            *(table_data.get()),
                                            &has_added);
      error_msg = "Table AddOrMod failed";
      check_and_return(bf_status,
                       "%s table:%s",
                       error_msg.c_str(),
                       table_name_get(table).c_str());
      if (!has_added) {
        auto status = response->add_status();
        status->set_space("");
        status->set_message("Entry aleady exists, perform MODIFY");
        status->set_code(grpc::ALREADY_EXISTS);
        status->set_canonical_code(grpc::OK);
      }
    } break;
    default:
      check_and_return(BF_INVALID_ARG, "Invalid update type");
  }
  grpc_check_and_return(grpc_status,
                        "%s table: %s",
                        error_msg.c_str(),
                        table_name_get(table).c_str());
  check_and_return(bf_status,
                   "%s table:%s",
                   error_msg.c_str(),
                   table_name_get(table).c_str());
  return Status();
}

Status write_operation(const BfRtInfo &info,
                       const uint32_t &client_id,
                       const BfRtSession &session,
                       const bf_rt_target_t &target,
                       const bfrt_proto::Update_Type & /*update*/,
                       const bfrt_proto::TableOperation &request,
                       bfrt_proto::WriteResponse * /*response*/) {
  const auto table_id = request.table_id();
  const BfRtTable *table;
  auto bf_status = info.bfrtTableFromIdGet(table_id, &table);
  check_and_return(bf_status, "Table not found");

  auto op_type = getOperationType(request.table_operations_type(), table);

  // 1. Allocate
  // 2. Set
  // 3. Execute
  std::unique_ptr<BfRtTableOperations> table_ops;
  bf_status = table->operationsAllocate(op_type, &table_ops);
  check_and_return(bf_status, "Operation not supported");

  // Save the table_id and the client_id in the cookie
  // Thus we can support the same client doing sync operations
  // on counter and register (and any other async operation) simultaneously.
  // This cookie needs to be freed in callbacks::server_op_cb
  //
  // Only one active request per table, per operation type is allowed.

  callbacks::OperationCookie *cookie =
      new callbacks::OperationCookie(client_id, table_id);
  if (nullptr == cookie) {
    bf_status = BF_NO_SPACE;
  } else if (!(cookie->isValid())) {
    LOG_ERROR("INVALID cookie: client_id=%u table_id=%u (0x%0lx+%0lx)",
              cookie->client_id_,
              cookie->table_id_,
              cookie->time_created_.tv_sec,
              cookie->time_created_.tv_nsec);
    bf_status = BF_UNEXPECTED;
  }
  check_and_return(bf_status, "Cookie init failed");

  switch (op_type) {
    case TableOperationsType::COUNTER_SYNC:
      bf_status = table_ops->counterSyncSet(session,
                                            target,
                                            callbacks::server_op_cb,
                                            reinterpret_cast<void *>(cookie));
      break;
    case TableOperationsType::REGISTER_SYNC:
      bf_status = table_ops->registerSyncSet(session,
                                             target,
                                             callbacks::server_op_cb,
                                             reinterpret_cast<void *>(cookie));
      break;
    case TableOperationsType::HIT_STATUS_UPDATE:
      bf_status =
          table_ops->hitStateUpdateSet(session,
                                       target,
                                       callbacks::server_op_cb,
                                       reinterpret_cast<void *>(cookie));
      break;
    default:
      bf_status = BF_INVALID_ARG;
  }
  if (bf_status != BF_SUCCESS) {
    delete cookie;
  }
  check_and_return(bf_status, "Operation set failed");
  bf_status = table->tableOperationsExecute(*table_ops);
  // We are sure that if BfRtTableObj::tableOperationsExecute
  // fails with an error, there is no chance that server_op_cb
  // will get called for this, so it is safe to delete the cookie
  if (bf_status != BF_SUCCESS) {
    if (cookie->isValid()) {
      delete cookie;
    } else {
      // Shouldn't happen. Better to leave the corrupted cookie in memory
      // than try to delete some other object or already freed cookie.
      LOG_ERROR("corrupted cookie: client_id=%u table_id=%u op_type=%u rc=%d",
                client_id,
                table_id,
                static_cast<unsigned int>(op_type),
                bf_status);
    }
  }
  check_and_return(bf_status, "Operation execute failed");

  return Status();
}

Status write_attribute(const BfRtInfo &info,
                       const uint32_t &client_id,
                       const BfRtSession &session,
                       const bf_rt_target_t &target,
                       const bfrt_proto::Update_Type & /*update*/,
                       const bfrt_proto::TableAttribute &request,
                       bfrt_proto::WriteResponse * /*response*/) {
  const auto table_id = request.table_id();
  const BfRtTable *table;
  auto bf_status = info.bfrtTableFromIdGet(table_id, &table);
  check_and_return(bf_status, "Table not found.");

  Status grpc_status;
  switch (request.attribute_case()) {
    case (bfrt_proto::TableAttribute::kIdleTable): {
      auto &idle_table = request.idle_table();
      grpc_status =
          setAttributeIdleTable(client_id, session, target, *table, idle_table);
      break;
    }
    case (bfrt_proto::TableAttribute::kEntryScope): {
      auto &entry_scope = request.entry_scope();
      grpc_status =
          setAttributeEntryScope(session, target, *table, entry_scope);
      break;
    }
    case (bfrt_proto::TableAttribute::kDynKeyMask): {
      auto &dyn_key_mask = request.dyn_key_mask();
      grpc_status =
          setAttributeDynKeyMask(session, target, *table, dyn_key_mask);
      break;
    }
    case (bfrt_proto::TableAttribute::kDynHashing): {
      auto &dyn_hashing = request.dyn_hashing();
      grpc_status =
          setAttributeDynHashing(session, target, *table, dyn_hashing);
      break;
    }
    case (bfrt_proto::TableAttribute::kByteCountAdj): {
      auto &byte_count_adj = request.byte_count_adj();
      grpc_status = setAttributeMeterByteCountAdj(
          session, target, *table, byte_count_adj);
      break;
    }
    case (bfrt_proto::TableAttribute::kPortStatusNotify): {
      auto &ps_notify = request.port_status_notify();
      grpc_status = setAttributePortStatusChgNotify(
          client_id, session, target, *table, nullptr, ps_notify.enable());
      break;
    }
    case (bfrt_proto::TableAttribute::kIntvlMs): {
      auto &intvl_info = request.intvl_ms();
      grpc_status =
          setAttributePortStatIntvl(session, target, *table, intvl_info);
      break;
    }
    case (bfrt_proto::TableAttribute::kPreDeviceConfig): {
      auto &pre_port_protection = request.pre_device_config();
      grpc_status = setAttributePreDeviceConfig(
          session, target, *table, pre_port_protection);
      break;
    }
    case (bfrt_proto::TableAttribute::kSelectorUpdateNotify): {
      auto &ps_notify = request.selector_update_notify();
      grpc_status = setAttributeSelectorUpdateChgNotify(
          client_id, session, target, *table, nullptr, ps_notify.enable());
      break;
    }
    default:
      grpc_status = to_grpc_status(BF_INVALID_ARG, "Invalid Attribute Type.");
  }
  return grpc_status;
}

void tableOperationWaitForCompletion(
    const bfrt_proto::WriteRequest *request,
    const std::vector<int> &operation_request_index,
    const timespec &deadline_tspec) {
  auto client_id = request->client_id();
  std::shared_ptr<ConnectionData> mutable_connection_sp;
  auto grpc_status = BfRtServer::getInstance().getConnection(
      client_id, &mutable_connection_sp);
  if (!grpc_status.ok()) {
    LOG_ERROR("%s:%d Unable to retrieve connection data for client with id %d",
              __func__,
              __LINE__,
              client_id);
    return;
  }
  timespec oper_wait = {5, 0};
  if (deadline_tspec.tv_sec != INT_MIN) {
    oper_wait.tv_sec = deadline_tspec.tv_sec;
  }
  for (const auto &req_index : operation_request_index) {
    const auto &update = request->updates(req_index);
    const auto &entity = update.entity();
    switch (entity.entity_case()) {
      case bfrt_proto::Entity::kTableOperation: {
        const auto table_id = entity.table_operation().table_id();
        const bool ws = mutable_connection_sp->removeIfPresentTableIdFromList(
            table_id, oper_wait.tv_sec);
        if (!ws) {
          LOG_ERROR(
              "%s:%d Timed out waiting for completion of operation for table "
              "%d",
              __func__,
              __LINE__,
              table_id);
        }
        break;
      }
      case bfrt_proto::Entity::kTableEntry:
      case bfrt_proto::Entity::kTableUsage:
      case bfrt_proto::Entity::kTableAttribute:
      default: {
        LOG_ERROR("%s:%d Trying to process invalid update type %d",
                  __func__,
                  __LINE__,
                  static_cast<int>(entity.entity_case()));
        break;
      }
    }  // end switch
  }    // end for
}

bool isDeadlineExceeded(const timespec &deadline_tspec) {
  if (deadline_tspec.tv_sec == INT_MIN || deadline_tspec.tv_nsec == INT_MIN) {
    return false;
  }
  timespec curr_tspec;
  timespec_get(&curr_tspec, TIME_UTC);

  return curr_tspec.tv_sec > deadline_tspec.tv_sec ||
         (curr_tspec.tv_sec == deadline_tspec.tv_sec &&
          curr_tspec.tv_nsec >= deadline_tspec.tv_nsec);
}

/*
 * process client Metadata
 * deadline_tspec = current time + deadline
 * deadline_tspec_relative = deadline
 * Check and update error_in_response
 * Check and update ignore_not_found
 */
void processClientMetadata(const ServerContext &context,
                           timespec *deadline_tspec,
                           timespec *deadline_tspec_relative,
                           bool *error_in_resp,
                           bool *ignore_not_found) {
  const auto &c_metadata = context.client_metadata();
  int deadline_sec = INT_MIN;
  int deadline_nsec = INT_MIN;
  for (auto iter = c_metadata.begin(); iter != c_metadata.end(); iter++) {
    if (iter->first == "deadline_sec") {
      int sec =
          std::stoi(std::string(iter->second.data(), iter->second.length()));
      if (sec < 0) {
        LOG_ERROR(
            "%s:%d Trying to set deadline (secs) to an invalid value of %d. "
            "Hence not setting deadline at all",
            __func__,
            __LINE__,
            sec);
        return;
      }
      deadline_sec = sec;
    } else if (iter->first == "deadline_nsec") {
      int nsec =
          std::stoi(std::string(iter->second.data(), iter->second.length()));
      if (nsec < 0 || nsec > 999999999) {
        LOG_ERROR(
            "%s:%d Trying to set deadline (nsecs) to an invalid value of %d. "
            "Hence not setting deadline at all",
            __func__,
            __LINE__,
            nsec);
        return;
      }
      deadline_nsec = nsec;
    } else if ((iter->first == "error_in_resp") &&
               (std::stoi(std::string(iter->second.data(),
                                      iter->second.length())) == 1)) {
      // error_in_resp in metadata indicates that errors are to be reported
      // back to the client via response->errors
      if (!error_in_resp) {
        LOG_ERROR(
            "%s:%d Invalid arguemnt of error_in_resp. ", __func__, __LINE__);
        return;
      } else {
        *error_in_resp = true;
      }
    } else if ((iter->first == "ignore_not_found") &&
               (std::stoi(std::string(iter->second.data(),
                                      iter->second.length())) == 1)) {
      if (ignore_not_found) {
        *ignore_not_found = true;
      }
    }
  }
  if (deadline_sec != INT_MIN || deadline_nsec != INT_MIN) {
    timespec_get(deadline_tspec, TIME_UTC);
    deadline_tspec->tv_sec += (deadline_sec != INT_MIN ? deadline_sec : 0);
    deadline_tspec->tv_nsec += (deadline_nsec != INT_MIN ? deadline_nsec : 0);
    if (deadline_tspec->tv_nsec >= 1000000000) {
      ++deadline_tspec->tv_sec;
      deadline_tspec->tv_nsec -= 1000000000;
    }
    deadline_tspec_relative->tv_sec =
        (deadline_sec != INT_MIN ? deadline_sec : 0);
    deadline_tspec_relative->tv_nsec =
        (deadline_nsec != INT_MIN ? deadline_nsec : 0);
  }
}
}  // anonymous namespace

Status setAttributePortStatusChgNotify(
    const uint32_t &client_id,
    const BfRtSession &session,
    const bf_rt_target_t &target,
    const BfRtTable &table,
    std::shared_ptr<const ConnectionData> connection_sp,
    const bool &enable) {
  std::unique_ptr<BfRtTableAttributes> attr;
  bf_status_t bf_status = BF_SUCCESS;
  bf_status =
      table.attributeAllocate(TableAttributesType::PORT_STATUS_NOTIF, &attr);
  check_and_return(
      bf_status, "Failed to allocate Attribute for port status change notify");
  if (connection_sp == nullptr) {
    auto grpc_status =
        BfRtServer::getInstance().getConnection(client_id, &connection_sp);
    grpc_check_and_return(grpc_status, "Falied to get connection");
  }

  if (!connection_sp->getNotifications().enable_port_status_change_) {
    check_and_return(BF_INVALID_ARG,
                     "Port status change notifications are not enabled"
                     "for client %d",
                     client_id);
  }
  bf_status = attr->portStatusChangeNotifSet(
      enable,
      callbacks::portstatuschg_callback,
      nullptr,
      reinterpret_cast<const void *>(
          formCookieFromDeviceAndClient(target.dev_id, client_id)));
  check_and_return(bf_status, "Failed to set port status change notify");
  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status, "Failed to set Port Status Change Notify Params");
  return Status();
}

Status setAttributeSelectorUpdateChgNotify(
    const uint32_t &client_id,
    const BfRtSession &session,
    const bf_rt_target_t &target,
    const BfRtTable &table,
    std::shared_ptr<const ConnectionData> connection_sp,
    const bool &enable) {
  (void)session;
  std::unique_ptr<BfRtTableAttributes> attr;
  bf_status_t bf_status = BF_SUCCESS;
  bf_status = table.attributeAllocate(
      TableAttributesType::SELECTOR_UPDATE_CALLBACK, &attr);
  check_and_return(
      bf_status,
      "Failed to allocate Attribute for selector update callback notify");
  if (connection_sp == nullptr) {
    auto grpc_status =
        BfRtServer::getInstance().getConnection(client_id, &connection_sp);
    grpc_check_and_return(grpc_status, "Falied to get connection");
  }

  if (!connection_sp->getNotifications().enable_selector_update_change_) {
    check_and_return(BF_INVALID_ARG,
                     "Selector Update CB notifications are not enabled"
                     "for client %d",
                     client_id);
  }
  bf_status = attr->selectorUpdateCbSet(
      enable,
      connection_sp->sessionGet(),
      callbacks::selectorupdatechg_callback,
      reinterpret_cast<const void *>(
          formCookieFromDeviceAndClient(target.dev_id, client_id)));

  check_and_return(bf_status, "Failed to set selector update CB notify");
  bf_status = table.tableAttributesSet(session, target, 0, *attr.get());
  check_and_return(bf_status, "Failed to set selector update CB Notify Params");
  return Status();
}

Status learn_callback_register(const ConnectionData &connection_data,
                               const std::shared_ptr<BfRtSession> session,
                               const bf_rt_target_t &target) {
  std::vector<const BfRtLearn *> learns;

  const BoundProgram *bound_program = nullptr;
  auto grpc_status = connection_data.boundProgramGet(&bound_program);
  grpc_check_and_return(grpc_status, "Unable to get bound_program");
  if (bound_program == nullptr) {
    LOG_ERROR("BF_RT_SERVER:%s:%s %d",
              "Unable to get bound_program",
              __func__,
              __LINE__);
    return grpc_status;
  }
  const auto info = bound_program->bfRtInfoGet();
  auto bf_status = info->bfrtInfoGetLearns(&learns);
  check_and_return(bf_status, "Unable to find learn objects from BfRtInfo");

  const auto client_id = connection_data.clientIdGet();
  for (const auto &learn : learns) {
    std::string learn_name;
    bf_status = learn->learnNameGet(&learn_name);
    bf_status = learn->bfRtLearnCallbackRegister(
        session,
        target,
        callbacks::learn_callback,
        reinterpret_cast<const void *>(
            formCookieFromDeviceAndClient(target.dev_id, client_id)));
    check_and_return(bf_status,
                     "Callback register fails for learn filter %s",
                     learn_name.c_str());
  }

  LOG_DBG("%s:%d Learn callback registered for client %d P4 %s",
          __func__,
          __LINE__,
          client_id,
          bound_program->p4NameGet().c_str());
  return Status();
}

Status learn_callback_deregister(const ConnectionData &connection_data,
                                 const std::shared_ptr<BfRtSession> session,
                                 const bf_rt_target_t &target) {
  std::vector<const BfRtLearn *> learns;

  const BoundProgram *bound_program = nullptr;
  auto grpc_status = connection_data.boundProgramGet(&bound_program);
  grpc_check_and_return(grpc_status, "Unable to get bound_program");
  if (bound_program == nullptr) {
    LOG_ERROR("BF_RT_SERVER:%s:%s %d",
              "Unable to get bound_program",
              __func__,
              __LINE__);
    return grpc_status;
  }

  const auto info = bound_program->bfRtInfoGet();
  auto bf_status = info->bfrtInfoGetLearns(&learns);
  check_and_return(bf_status, "Callback deregister fails.");

  for (const auto &learn : learns) {
    bf_status = learn->bfRtLearnCallbackDeregister(session, target);
    check_and_return(bf_status, "Callback deregister fails.");
  }

  LOG_DBG("%s:%d Learn callback deregistered for client %d P4 %s",
          __func__,
          __LINE__,
          connection_data.clientIdGet(),
          bound_program->p4NameGet().c_str());
  return Status();
}

Status BfRuntimeServiceImpl::GetForwardingPipelineConfig(
    ServerContext * /*context */,
    const bfrt_proto::GetForwardingPipelineConfigRequest *request,
    bfrt_proto::GetForwardingPipelineConfigResponse *response) {
  // get the dev_id and p4_name from the request msg
  auto device_id = request->device_id();
  auto client_id = request->client_id();

  SetFwdConfigLockGuard read_lock(BfRtServer::getInstance().setFwdRwLockget(),
                                  false);
  if (!read_lock) {
    check_and_return(
        BF_IN_USE, "Failed to acquire read lock for client %d", client_id);
  }

  // If connection data doesn't exist, it is an independent client performing
  // its first rpc. In this case, we will create a new connection data object
  // for client at this point, and then proceed as though the client already
  // exists
  auto add_status = BfRtServer::getInstance().addConnection(client_id);

  // Get the connection data for this client
  std::shared_ptr<const ConnectionData> connection_sp;

  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  grpc_check_and_return(grpc_status,
                        "Unable to retrieve connection data"
                        " for client with id %d",
                        client_id);
  grpc_status = connection_sp->config_manager.get(device_id, response);
  grpc_check_and_return(grpc_status,
                        "Unable to get config response"
                        " for client with id %d",
                        client_id);
  // Remove ConnectionData object from map for independent clients
  // only if it was inserted as an independent client in this RPC
  if (add_status.ok() && connection_sp->isIndependent()) {
    BfRtServer::getInstance().cleanupConnection(client_id);
  }
  return grpc_status;
}

Status BfRuntimeServiceImpl::SetForwardingPipelineConfig(
    ServerContext * /*context*/,
    const bfrt_proto::SetForwardingPipelineConfigRequest *request,
    bfrt_proto::SetForwardingPipelineConfigResponse * /*response*/) {
  const auto client_id = request->client_id();
  Status grpc_status = Status();
  // Making separate code branches for BIND and non-BIND since
  // non-BINDS require write lock.
  if (request->action() ==
      bfrt_proto::SetForwardingPipelineConfigRequest_Action_BIND) {
    SetFwdConfigLockGuard read_lock(BfRtServer::getInstance().setFwdRwLockget(),
                                    false);
    if (!read_lock) {
      check_and_return(
          BF_IN_USE, "Failed to acquire read lock for client %d", client_id);
    }
    grpc_status =
        BfRtServer::getInstance().forwardingConfigBind(client_id, request);
    grpc_check_and_return(
        grpc_status, "Failed to set Fwd_config for client_id %d", client_id);
  } else {
    // Write lock is blocking as against to the read lock which is non-blocking
    // TODO: Ensure that the write_locker doesn't starve
    SetFwdConfigLockGuard write_lock(
        BfRtServer::getInstance().setFwdRwLockget(), true);
    if (!write_lock) {
      check_and_return(
          BF_IN_USE, "Failed to acquire write lock for client %d", client_id);
    }

    // If it is an independent client performing an RPC, it may not exist
    // in the map
    auto add_status = BfRtServer::getInstance().addConnection(client_id);

    grpc_status =
        BfRtServer::getInstance().forwardingConfigSet(client_id, request);
    grpc_check_and_return(
        grpc_status, "Failed to set Fwd_config for client_id %d", client_id);

    // Remove ConnectionData object from map for independent clients if it was
    // one
    if (add_status.ok()) {
      std::shared_ptr<const ConnectionData> connection_sp;
      auto get_status =
          BfRtServer::getInstance().getConnection(client_id, &connection_sp);
      // There is a possibility that client_id itself was kicked out during
      // forwardingConfigSet() and hence it doesn't exists anymore. Don't
      // error out if getConnection failed in that case
      if (get_status.ok() && connection_sp->isIndependent())
        BfRtServer::getInstance().cleanupConnection(client_id);
    }
  }
  return grpc_status;
}

Status BfRuntimeServiceImpl::Write(ServerContext *context,
                                   const bfrt_proto::WriteRequest *request,
                                   bfrt_proto::WriteResponse *response) {
  timespec deadline_tspec = {INT_MIN};
  timespec deadline_tspec_relative = {INT_MIN};
  bool error_in_resp = false;
  bool ignore_not_found = false;
  processClientMetadata(*context,
                        &deadline_tspec,
                        &deadline_tspec_relative,
                        &error_in_resp,
                        &ignore_not_found);

#ifdef BFRT_PERF_TEST
  /*ProfilerStart("./perf_data.txt");*/
  struct timespec start = {0};
  clock_gettime(CLOCK_MONOTONIC, &start);
#endif
  auto device_id = static_cast<device_id_t>(request->target().device_id());
  BfRtServerErrorReporter error_reporter("Write Error Status");
  auto client_id = request->client_id();

  SetFwdConfigLockGuard read_lock(BfRtServer::getInstance().setFwdRwLockget(),
                                  false);
  if (!read_lock) {
    check_and_return(
        BF_IN_USE, "Failed to acquire read lock for client %d", client_id);
  }

  // If it is an independent client performing an RPC, it may not exist
  // in the map
  auto add_status = BfRtServer::getInstance().addConnection(client_id);

  const BfRtInfo *info;
  // Get the connection data for this client
  std::shared_ptr<const ConnectionData> connection_sp;
  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  grpc_check_and_return(grpc_status,
                        "Unable to retrieve connection data"
                        " for client with id %d",
                        client_id);
  std::string p4_name = request->p4_name();
  // Checking if client is independent, or if subscribed client with
  // side-process/thread started Write RPC with p4 name specified
  if (connection_sp->isIndependent() ||
      (!connection_sp->isIndependent() && p4_name != "")) {
    grpc_status = getBfRtInfoIndependent(device_id, p4_name, &info);
  } else {
    grpc_status = getBfRtInfo(client_id, device_id, connection_sp, &info);
  }
  grpc_check_and_return(grpc_status, "Error getting BfRtInfo");

  auto session = connection_sp->sessionGet();
  if (session == nullptr) {
    check_and_return(
        BF_INVALID_ARG, "Unable to get session for client %d", client_id);
  }
  bf_dev_direction_t direction;
  switch (request->target().direction()) {
    case 0:
      direction = BF_DEV_DIR_INGRESS;
      break;
    case 1:
      direction = BF_DEV_DIR_EGRESS;
      break;
    default:
      direction = BF_DEV_DIR_ALL;
      break;
  }
  bf_rt_target_t target = {device_id,
                           request->target().pipe_id(),
                           direction,
                           static_cast<uint8_t>(request->target().prsr_id())};
  // Find atomicity
  auto atomicity_type = getAtomicity(request);
  if (atomicity_type == BfRtProtoAtomicityType::ROLLBACK_ON_ERROR) {
    // isAtomic is set to true since we want all or none
    // semantics to be applied at packet level too
    auto bf_status = session->beginTransaction(false);
    check_and_return(
        bf_status, "Failed to begin Transaction for client %d", client_id);
  } else if (atomicity_type == BfRtProtoAtomicityType::DATAPLANE_ATOMIC) {
    check_and_return(BF_NOT_IMPLEMENTED, "DATAPLANE_ATOMIC isn't implemented");
  } else {
    // Begin the batch only if the Atomicity is not ROLLBACK_ON_ERROR
    session->beginBatch();
  }

  std::vector<int> operation_request_index;
  for (int updates_served = 0; updates_served < request->updates_size();
       updates_served++) {
    if (isDeadlineExceeded(deadline_tspec)) {
      LOG_ERROR(
          "%s:%d Write RPC request timed out before the Server could service "
          "the %d th update",
          __func__,
          __LINE__,
          updates_served);
      grpc_status =
          Status(StatusCode::DEADLINE_EXCEEDED, "Write RPC request timed out");
      // Push the status to the error reporter
      if (atomicity_type == BfRtProtoAtomicityType::ROLLBACK_ON_ERROR) {
        session->abortTransaction();
      } else {
        session->flushBatch();
        session->endBatch(true);
      }
      session->sessionCompleteOperations();

      // Once we have issued all the writes, wait for all the operations (if
      // any)
      // to complete before we return to the client
      if (operation_request_index.size()) {
        tableOperationWaitForCompletion(
            request, operation_request_index, deadline_tspec_relative);
      }

      return grpc_status;
    }
    const auto &update = request->updates(updates_served);
    const auto &entity = update.entity();
    switch (entity.entity_case()) {
      case bfrt_proto::Entity::kTableEntry: {
        const bfrt_proto::TableEntry &table_entry = entity.table_entry();
        bf_rt_target_t entry_tgt = target;

        // using target from table_entry if it is present
        if (table_entry.has_entry_tgt()) {
          entry_tgt.dev_id = table_entry.entry_tgt().device_id();
          entry_tgt.pipe_id = table_entry.entry_tgt().pipe_id();
          entry_tgt.prsr_id =
              static_cast<uint8_t>(table_entry.entry_tgt().prsr_id());

          switch (table_entry.entry_tgt().direction()) {
            case 0:
              direction = BF_DEV_DIR_INGRESS;
              break;
            case 1:
              direction = BF_DEV_DIR_EGRESS;
              break;
            default:
              direction = BF_DEV_DIR_ALL;
              break;
          }
          entry_tgt.direction = direction;
        }

        grpc_status = write_entry(*info,
                                  *session,
                                  entry_tgt,
                                  update.type(),
                                  entity.table_entry(),
                                  ignore_not_found,
                                  response);
        break;
      }
      case bfrt_proto::Entity::kTableUsage: {
        // Write on TableUsage is not supported.
        grpc_status = to_grpc_status(
            BF_NOT_SUPPORTED, "Write operation not available on TableUsage");
        break;
      }
      case bfrt_proto::Entity::kTableAttribute: {
        grpc_status = write_attribute(*info,
                                      client_id,
                                      *session,
                                      target,
                                      update.type(),
                                      entity.table_attribute(),
                                      response);
        break;
      }
      case bfrt_proto::Entity::kTableOperation: {
        // A "write" on TableOperations is basically an execute. Maybe need to
        // come up with other semantics for it later on.
        grpc_status = write_operation(*info,
                                      client_id,
                                      *session,
                                      target,
                                      update.type(),
                                      entity.table_operation(),
                                      response);
        if (grpc_status.ok()) {
          operation_request_index.push_back(updates_served);
        }
        break;
      }
      default: {
        // If no Entity is present, then it can be a case of
        // table clear. Need to check
        if (update.type() == bfrt_proto::Update_Type_DELETE) {
          grpc_status = write_entry(*info,
                                    *session,
                                    target,
                                    update.type(),
                                    entity.table_entry(),
                                    ignore_not_found,
                                    response);
        } else {
          grpc_status = to_grpc_status(BF_NOT_SUPPORTED, "Invalid update.");
        }
        break;
      }
    }  // end switch

    if (error_in_resp) {
      bfrt_proto::Error *status = NULL;
      // Add a status to response in case it was not added before
      if (response->status_size() == updates_served) {
        status = response->add_status();
        status->set_space("");
        status->set_message(grpc_status.error_message());
        auto error_code = grpc_status.error_code();
        // ignore_not_found flag is designed to return OK during attempt to
        // remove the entry again. code field is used to inform that entry was
        // not present
        if (update.type() == bfrt_proto::Update_Type_DELETE &&
            ignore_not_found) {
          if (error_code == grpc::NOT_FOUND) {
            status->set_code(error_code);
            error_code = grpc::OK;
          }
        }
        status->set_canonical_code(error_code);
      } else {
        status =
            const_cast<bfrt_proto::Error *>(&(*response->status().rbegin()));
      }
      error_reporter.push_back(*status);
    } else {
      // Push the status to the error reporter
      error_reporter.push_back(grpc_status);
    }

    // Check grpc_status on every update request and abort if
    // not OK and if ROLLBACK_ON_ERROR is set
    if (atomicity_type == BfRtProtoAtomicityType::ROLLBACK_ON_ERROR &&
        !grpc_status.ok()) {
      if (!(grpc_status.error_code() == grpc::NOT_FOUND && ignore_not_found)) {
        session->abortTransaction();
        break;  // break for
      }
    }
  }  // end for

  if (atomicity_type == BfRtProtoAtomicityType::ROLLBACK_ON_ERROR) {
    // isHwSynchronous set to true because we want Complete operations
    // functionality too
    session->commitTransaction(true);
  } else {
    session->flushBatch();
    session->endBatch(true);
  }
  session->sessionCompleteOperations();

  // Once we have issued all the writes, wait for all the operations (if any)
  // to complete before we return to the client
  if (operation_request_index.size()) {
    tableOperationWaitForCompletion(
        request, operation_request_index, deadline_tspec_relative);
  }

  // Remove ConnectionData object from map for independent clients
  // only if it was inserted as an independent client in this RPC
  if (add_status.ok() && connection_sp->isIndependent()) {
    BfRtServer::getInstance().cleanupConnection(client_id);
  }

#ifdef BFRT_PERF_TEST
  return calculateRate(start, request->updates_size(), &error_reporter);
#endif

  return error_reporter.get_status();
}

Status BfRuntimeServiceImpl::Read(
    ServerContext *context,
    const bfrt_proto::ReadRequest *request,
    ServerWriter<bfrt_proto::ReadResponse> *writer) {
  timespec deadline_tspec = {INT_MIN};
  timespec deadline_tspec_relative = {INT_MIN};
  bool error_in_resp = false;
  processClientMetadata(*context,
                        &deadline_tspec,
                        &deadline_tspec_relative,
                        &error_in_resp,
                        nullptr);

  const auto client_id = request->client_id();
  // If it is an independent client performing an RPC, it may not exist
  // in the map
  auto add_status = BfRtServer::getInstance().addConnection(client_id);

  // Get the connection data for this client
  std::shared_ptr<const ConnectionData> connection_sp;
  auto grpc_status =
      BfRtServer::getInstance().getConnection(client_id, &connection_sp);
  grpc_check_and_return(grpc_status,
                        "Unable to retrieve connection data"
                        " for client with id %d",
                        client_id);
  auto device_id = static_cast<bf_dev_id_t>(request->target().device_id());
  bfrt_proto::ReadResponse response;
  BfRtServerErrorReporter error_reporter("Read Error Status");
  SetFwdConfigLockGuard read_lock(BfRtServer::getInstance().setFwdRwLockget(),
                                  false);
  if (!read_lock) {
    check_and_return(
        BF_IN_USE, "Failed to acquire read lock for client %d", client_id);
  }

  const BfRtInfo *info;
  std::string p4_name = request->p4_name();
  // Checking if client is independent, or if subscribed client with
  // side-process/thread started Write RPC with p4 name specified
  if (connection_sp->isIndependent() ||
      (!connection_sp->isIndependent() && p4_name != "")) {
    grpc_status = getBfRtInfoIndependent(device_id, p4_name, &info);
  } else {
    grpc_status = getBfRtInfo(client_id, device_id, connection_sp, &info);
  }
  grpc_check_and_return(grpc_status, "Error getting BfRtInfo");

  auto session = connection_sp->sessionGet();

  if (session == nullptr) {
    check_and_return(BF_INVALID_ARG, "Unable to get session for client");
  }
  bf_dev_direction_t direction;
  switch (request->target().direction()) {
    case 0:
      direction = BF_DEV_DIR_INGRESS;
      break;
    case 1:
      direction = BF_DEV_DIR_EGRESS;
      break;
    default:
      direction = BF_DEV_DIR_ALL;
      break;
  }
  bf_rt_target_t target = {device_id,
                           request->target().pipe_id(),
                           direction,
                           static_cast<uint8_t>(request->target().prsr_id())};

  for (int entities_served = 0; entities_served < request->entities_size();
       entities_served++) {
    if (isDeadlineExceeded(deadline_tspec)) {
      LOG_ERROR(
          "%s:%d Read RPC request timed out before the Server could service "
          "the %d th update",
          __func__,
          __LINE__,
          entities_served);
      grpc_status =
          Status(StatusCode::DEADLINE_EXCEEDED, "Read RPC request timed out");
      return grpc_status;
    }
    const auto &entity = request->entities(entities_served);
    switch (entity.entity_case()) {
      case bfrt_proto::Entity::kTableEntry: {
        const bfrt_proto::TableEntry &table_entry = entity.table_entry();
        bf_rt_target_t entry_tgt = target;

        // using target from table_entry if it is present
        if (table_entry.has_entry_tgt()) {
          entry_tgt.dev_id = table_entry.entry_tgt().device_id();
          entry_tgt.pipe_id = table_entry.entry_tgt().pipe_id();
          entry_tgt.prsr_id =
              static_cast<uint8_t>(table_entry.entry_tgt().prsr_id());

          switch (table_entry.entry_tgt().direction()) {
            case 0:
              direction = BF_DEV_DIR_INGRESS;
              break;
            case 1:
              direction = BF_DEV_DIR_EGRESS;
              break;
            default:
              direction = BF_DEV_DIR_ALL;
              break;
          }

          entry_tgt.direction = direction;
        }

        grpc_status = read_entry(
            *info, *session, entry_tgt, entity.table_entry(), &response);
        break;
      }
      case bfrt_proto::Entity::kTableUsage: {
        grpc_status = read_usage(
            *info, *session, target, entity.table_usage(), &response);
        break;
      }
      case bfrt_proto::Entity::kTableAttribute: {
        grpc_status = read_attribute(
            *info, *session, target, entity.table_attribute(), &response);
        break;
      }
      case bfrt_proto::Entity::kTableOperation: {
        check_and_return(BF_NOT_SUPPORTED,
                         "Read operation not available on TableOperation");
        break;
      }
      case bfrt_proto::Entity::kObjectId: {
        grpc_status = read_id(*info, entity.object_id(), &response);
        break;
      }
      case bfrt_proto::Entity::kHandle: {
        grpc_status =
            read_handle(*info, *session, target, entity.handle(), &response);
        break;
      }
      default: {
        grpc_status =
            to_grpc_status(BF_INVALID_ARG, "Invalid Read entity received");
        break;
      }
    }
    // if context is to set error in response
    // pack the error messages in response
    // grpc_status is added in errors for all the entities(success or fail)
    if (error_in_resp) {
      auto status = response.add_status();
      status->set_space("");
      status->set_message(grpc_status.error_message());
      status->set_canonical_code(grpc_status.error_code());
    } else {
      // Push the status to the error reporter
      error_reporter.push_back(grpc_status);
    }
  }

  session->sessionCompleteOperations();

  writer->Write(response);

  // Remove ConnectionData object from map for independent clients
  // only if it was inserted as an independent client in this RPC
  if (add_status.ok() && connection_sp->isIndependent()) {
    BfRtServer::getInstance().cleanupConnection(client_id);
  }
  return error_reporter.get_status();
}

Status BfRuntimeServiceImpl::StreamChannel(ServerContext *context,
                                           StreamChannelReaderWriter *stream) {
  bfrt_proto::StreamMessageRequest request;
  uint32_t client_id = 0;
  uint32_t last_client_id = 0;
  bool already_connected = false;
  Status grpc_status;
  while (stream->Read(&request)) {
    client_id = request.client_id();
    switch (request.update_case()) {
      case bfrt_proto::StreamMessageRequest::kSubscribe: {
        if (already_connected) {
          LOG_ERROR(
              "%s:%d Client %d is already subscribed with this channel."
              " Please stop sending subscribe msgs",
              __func__,
              __LINE__,
              last_client_id);
          continue;
        }
        LOG_DBG(
            "%s:%d Client %d trying to connect", __func__, __LINE__, client_id);
        grpc_status = BfRtServer::getInstance().addConnection(
            client_id, request.subscribe(), context, stream);
        grpc_check_and_return(grpc_status,
                              "Error during Adding connection for client_id %d",
                              client_id);

        // send a msg to the client with the status_code
        bfrt_proto::StreamMessageResponse response;
        auto status_msg = response.mutable_subscribe();
        auto rpc_status = status_msg->mutable_status();
        rpc_status->set_code(static_cast<::google::rpc::Code>(grpc::OK));
        std::shared_ptr<const ConnectionData> connection_sp;
        grpc_status =
            BfRtServer::getInstance().getConnection(client_id, &connection_sp);
        grpc_check_and_return(grpc_status, "Failed to get grpc connection");
        connection_sp->sendStreamMessage(response);
        LOG_DBG("Response sent from stream");
        already_connected = true;
        last_client_id = client_id;
        break;
      }
      case bfrt_proto::StreamMessageRequest::kDigestAck:
        check_and_return(BF_NOT_SUPPORTED, "Not yet implemented");
        break;
      default:
        break;
    }
  }
  // Here the bidriectional stream also serves as the hearbeat of the client
  // to the server. So we use it to figure out when the client has actually
  // left the connection. The Read() on the stream will return false when
  // the client has called WritesDone() on the stream or the stream has
  // failed (or been cancelled). The only caveat is that each client MUST
  // open a bidirectional stream with the server even if it does not want
  // to use it explicitly as it also serves as a heartbeat.
  BfRtServer::getInstance().cleanupConnection(client_id);
  LOG_DBG(
      "%s:%d Client_id %d closed connection", __func__, __LINE__, client_id);
  return grpc_status;
}

}  // namespace server
}  // namespace bfrt
