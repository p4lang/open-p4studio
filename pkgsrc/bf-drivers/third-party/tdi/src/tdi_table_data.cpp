// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include <algorithm>

#include <tdi/common/tdi_table_data.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const uint64_t & /*value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const uint8_t * /*value*/,
                                 const size_t & /*size*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const std::vector<tdi_id_t> & /*arr*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const std::vector<bool> & /*arr*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const std::vector<std::string> & /*arr*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const int64_t & /*value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const float & /*value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const bool & /*value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(
    const tdi_id_t & /*field_id*/,
    std::vector<std::unique_ptr<tdi::TableData>> /*ret_vec*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const std::string & /*str*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 uint64_t * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 const size_t & /*size*/,
                                 uint8_t * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}
tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::vector<tdi_id_t> * /*arr*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::vector<bool> * /*arr*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::vector<std::string> * /*arr*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 int64_t * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 float * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 bool * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::vector<uint64_t> * /*arr*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(
    const tdi_id_t & /*field_id*/,
    std::vector<tdi::TableData *> * /*ret_vec*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::string * /*str*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::dataAllocate(
    const tdi_id_t & /*container_id*/,
    std::unique_ptr<tdi::TableData> * /*data_ret*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::dataAllocateContainer(
    const tdi_id_t & /*container_id*/,
    const std::vector<tdi_id_t> & /*fields*/,
    std::unique_ptr<tdi::TableData> * /*data_ret*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::reset() { return this->reset(0, 0, {}); }

tdi_status_t TableData::reset(const tdi_id_t &action_id) {
  return this->reset(action_id, 0, {});
}

tdi_status_t TableData::reset(const tdi_id_t &action_id,
                              const std::vector<tdi_id_t> &fields) {
  return this->reset(action_id, 0, fields);
}

tdi_status_t TableData::reset(const tdi_id_t &action_id,
                              const tdi_id_t &container_id,
                              const std::vector<tdi_id_t> &fields) {
  this->actionIdSet(action_id);
  this->container_id_ = container_id;
  this->activeFieldsSet(fields);
  return this->resetDerived();
}

tdi_status_t TableData::activeFieldsSet(const std::vector<tdi_id_t> &fields) {
  this->removed_one_ofs_ = {};
  this->active_fields_s_ = {};
  if (fields.empty()) {
    this->all_fields_set_ = true;
  } else {
    this->all_fields_set_ = false;
    std::copy(fields.begin(),
              fields.end(),
              std::inserter(active_fields_s_, active_fields_s_.end()));
  }
  return TDI_SUCCESS;
}

void TableData::removeActiveField(const tdi_id_t &field_id) {
  // The reason a separate set of removed_one_ofs_ is maintained
  // is because the active_field_s_ set doesn't always
  // contain the list of all fields. It cannot keep it especially
  // if the action_id is not known beforehand. So in those cases,
  // we need to mark if a field was removed.
  this->removed_one_ofs_.insert(field_id);

  // If the set of active fields is empty,
  // then no need to process it or change
  // all_fields_set.
  if (!active_fields_s_.empty()) {
    this->active_fields_s_.erase(field_id);
    all_fields_set_ = false;
  }
}

tdi_status_t TableData::isActive(const tdi_id_t &field_id,
                                 bool *is_active) const {
  // 1. Check if the input field was part of an explicit
  // field removal which can be done via removeActiveField.
  auto it = this->removed_one_ofs_.find(field_id);
  if (it != this->removed_one_ofs_.end()) {
    *is_active = false;
    return TDI_SUCCESS;
  }
  // 2. Check if all fields were set. Then all fields are
  // active

  if (this->all_fields_set_) {
    *is_active = true;
    return TDI_SUCCESS;
  }

  // Lastly check if they are explicitly set in the active
  // fields set
  auto it2 = this->active_fields_s_.find(field_id);
  if (it2 != this->active_fields_s_.end()) {
    *is_active = true;
    return TDI_SUCCESS;
  }
  *is_active = false;
  return TDI_SUCCESS;
}

tdi_status_t TableData::getParent(const tdi::Table **table) const {
  *table = this->table_;
  return TDI_SUCCESS;
}

// Learn object needs to override this
tdi_status_t TableData::getParent(const tdi::Learn ** /*learn*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

}  // namespace tdi
