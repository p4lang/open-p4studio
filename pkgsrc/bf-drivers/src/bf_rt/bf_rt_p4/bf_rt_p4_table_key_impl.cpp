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


#include <arpa/inet.h>
#include <inttypes.h>
#include <bitset>

#include <bf_rt_common/bf_rt_table_field_utils.hpp>
#include <bf_rt_common/bf_rt_utils.hpp>
#include "bf_rt_p4_table_impl.hpp"
#include "bf_rt_p4_table_key_impl.hpp"

namespace bfrt {

// BfRtMatchActionKey *****************
bf_status_t BfRtMatchActionKey::setValue(const bf_rt_id_t &field_id,
                                         const uint64_t &value) {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (key_field->isMatchPriority()) {
    priority = value;
  } else {
    if (key_field->isPartitionIndex()) {
      partition_index = value;
    }

    status = utils::BfRtTableFieldUtils::boundsCheck(
        *table_, *key_field, value, nullptr, 0);
    if (status != BF_SUCCESS) {
      return status;
    }
    // ceil the size and convert bits to bytes
    auto size_bytes = (key_field->getSize() + 7) / 8;
    // endianness conversion
    auto new_value = htobe64(value);
    // Here we did a 64 bit host to big-endian conversion. When copying to
    // the byte array, need to advance the pointer to the value array based
    // on the field size
    // The value array is 8 bytes long. For instance for a 4 byte field, we
    // need to advance the "value" pointer by (8 - 4) bytes, so that it
    // points to the beginning of the field
    // For instance, field value in little endian - 0xCDAB. When invoked
    // with htobe64 becomes
    // 0x00 0x00 0x00 0x00 0x00 0x00 0xAB 0xCD. We need to copy 0x0000ABCD
    // into the array
    // since its a 4 byte field, thus we advance the value pointer by 4
    // bytes
    uint32_t src_offset = (8 - size_bytes);

    uint8_t *val_ptr = ((uint8_t *)&new_value) + src_offset;
    this->setValueInternal(*key_field, val_ptr, size_bytes);
  }
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValue(const bf_rt_id_t &field_id,
                                         const uint8_t *value,
                                         const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, 0, value, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  size_t size_bytes = (key_field->getSize() + 7) / 8;
  if (key_field->isMatchPriority()) {
    uint64_t pri = 0;
    utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &pri);
    this->priority = static_cast<uint32_t>(pri);
  } else if (key_field->isPartitionIndex()) {
    uint64_t p_idx = 0;
    utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &p_idx);
    this->partition_index = static_cast<uint32_t>(p_idx);
    // TODO we need not set this but unless we do so, a duplicate entry problem
    // comes in pipe_mgr since pipe_mgr only checks the match spec buffer for
    // duplicates. Needs fix in pipe_mgr
    this->setValueInternal(*key_field, value, size_bytes);
  } else {
    this->setValueInternal(*key_field, value, size_bytes);
  }
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValueandMask(const bf_rt_id_t &field_id,
                                                const uint64_t &value,
                                                const uint64_t &mask) {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::TERNARY, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, value, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, mask, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  auto field_size = key_field->getSize();
  // ceil the size and convert bits to bytes
  auto size_bytes = (field_size + 7) / 8;

  // endianness conversion
  auto new_value = htobe64(value);
  auto new_mask = htobe64(mask);
  uint32_t src_offset = (8 - size_bytes);
  uint8_t *val_ptr = ((uint8_t *)&new_value) + src_offset;
  uint8_t *msk_ptr = ((uint8_t *)&new_mask) + src_offset;
  setValueAndMaskInternal(*key_field, val_ptr, msk_ptr, size_bytes, true);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValueandMask(const bf_rt_id_t &field_id,
                                                const uint8_t *value,
                                                const uint8_t *mask,
                                                const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::TERNARY, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  size_t size_bytes = (key_field->getSize() + 7) / 8;
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, 0, value, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, 0, mask, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  setValueAndMaskInternal(*key_field, value, mask, size_bytes, true);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValueRange(const bf_rt_id_t &field_id,
                                              const uint64_t &start,
                                              const uint64_t &end) {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::RANGE, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  auto field_size = key_field->getSize();
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, start, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, end, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  // ceil the size and convert bits to bytes
  auto size_bytes = (field_size + 7) / 8;
  // endianness conversion
  auto new_value = htobe64(start);
  auto new_mask = htobe64(end);
  uint32_t src_offset = (8 - size_bytes);
  uint8_t *val_ptr = ((uint8_t *)&new_value) + src_offset;
  uint8_t *msk_ptr = ((uint8_t *)&new_mask) + src_offset;
  // For Range keyfields, tell setValueAndMaskInternal not to mask the key with
  // the mask
  setValueAndMaskInternal(*key_field, val_ptr, msk_ptr, size_bytes, false);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValueRange(const bf_rt_id_t &field_id,
                                              const uint8_t *start,
                                              const uint8_t *end,
                                              const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::RANGE, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  size_t size_bytes = (key_field->getSize() + 7) / 8;
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, 0, start, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, 0, end, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  setValueAndMaskInternal(*key_field, start, end, size_bytes, false);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValueLpm(const bf_rt_id_t &field_id,
                                            const uint64_t &value,
                                            const uint16_t &p_length) {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::LPM, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (p_length > key_field->getSize()) {
    LOG_ERROR(
        "%s:%d %s ERROR : Prefix length (%d) cannot be greater than field "
        "size(%zu "
        "bits) for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        p_length,
        key_field->getSize(),
        field_id);
    return BF_INVALID_ARG;
  }

  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, value, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  // The mask initially is kept all FFs. Let prefix_length = 44, field_size = 48
  // Then we right shift 64-44 = 20 bits and then left shift 48-44 = 4 bits
  // so finally (16 x 0s)(44 x 1s)(4 x 0s), which is what we want.
  uint64_t mask = -1;
  auto field_size = key_field->getSize();
  if (p_length) {
    mask = (mask >> (64 - p_length)) << (field_size - p_length);
  } else {
    mask = 0;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, mask, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }

  priority = field_size - p_length;

  // ceil the size and convert bits to bytes
  auto size_bytes = (field_size + 7) / 8;
  // endianness conversion
  // Here we did a 64 bit host to big-endian conversion. When copying to
  // the byte array, need to advance the pointer to the value array based
  // on the field size
  // The value array is 8 bytes long. For instance for a 2 byte field, we
  // need to advance the "value" pointer by (8 - 2) bytes, so that it
  // points to the beginning of the field
  // For instance, field value in little endian - 0xCDAB. When invoked
  // with htobe64 becomes
  // 0x00 0x00 0x00 0x00 0x00 0x00 0xAB 0xCD. We need to copy 0x0000ABCD
  // into the array
  // since its a 2 byte field, thus we advance the value pointer by 6
  // bytes
  auto new_value = htobe64(value);
  auto new_mask = htobe64(mask);
  uint32_t src_offset = (8 - size_bytes);
  uint8_t *val_ptr = ((uint8_t *)&new_value) + src_offset;
  uint8_t *msk_ptr = ((uint8_t *)&new_mask) + src_offset;
  setValueAndMaskInternal(*key_field, val_ptr, msk_ptr, size_bytes, true);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValueLpm(const bf_rt_id_t &field_id,
                                            const uint8_t *value1,
                                            const uint16_t &p_length,
                                            const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::LPM, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (p_length > key_field->getSize()) {
    LOG_ERROR(
        "%s:%d %s ERROR : Prefix length (%u) cannot be greater than length of "
        "field "
        "(%zu) for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        p_length,
        key_field->getSize(),
        field_id);
    return BF_INVALID_ARG;
  }
  size_t size_bytes = (key_field->getSize() + 7) / 8;
  std::vector<uint8_t> mask(size_bytes, 0xff);

  // If prefix length is 115 and the size is 128 bits, then we need to
  // change 13 bits from the end of the vector to 0. So 1 byte and 5 bits

  uint32_t total_bits_flip = key_field->getSize() - p_length;  // 128 - 115
  uint32_t bytes_flip = total_bits_flip / 8;                   // 13 / 8
  uint32_t bits_flip = total_bits_flip % 8;                    // 13 % 8

  for (uint32_t i = 0; i < bytes_flip; i++) {
    mask[mask.size() - 1 - i] = 0;
  }
  if (bits_flip != 0) {
    mask[mask.size() - 1 - bytes_flip] <<= bits_flip;
  }
  priority = key_field->getSize() - p_length;
  // Check size compatibility for TERNARY
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, 0, value1, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  setValueAndMaskInternal(*key_field, value1, &mask[0], size_bytes, true);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValueOptional(const bf_rt_id_t &field_id,
                                                 const uint64_t &value,
                                                 const bool &is_valid) {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::OPTIONAL, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, value, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  auto field_size = key_field->getSize();
  // ceil the size and convert bits to bytes
  auto size_bytes = (field_size + 7) / 8;
  uint64_t mask = 0;
  if (is_valid) {
    mask = (field_size < 64) ? ((1ULL << field_size) - 1) : UINT64_MAX;
  }

  // endianness conversion
  auto new_value = htobe64(value);
  auto new_mask = htobe64(mask);
  uint32_t src_offset = (8 - size_bytes);
  uint8_t *val_ptr = ((uint8_t *)&new_value) + src_offset;
  uint8_t *msk_ptr = ((uint8_t *)&new_mask) + src_offset;
  setValueAndMaskInternal(*key_field, val_ptr, msk_ptr, size_bytes, false);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::setValueOptional(const bf_rt_id_t &field_id,
                                                 const uint8_t *value,
                                                 const bool &is_valid,
                                                 const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::OPTIONAL, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  size_t size_bytes = (key_field->getSize() + 7) / 8;
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *table_, *key_field, 0, value, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (is_valid) {
    std::vector<uint8_t> mask(size_bytes, 0xff);
    setValueAndMaskInternal(*key_field, value, &mask[0], size_bytes, false);
  } else {
    std::vector<uint8_t> mask(size_bytes, 0);
    setValueAndMaskInternal(*key_field, value, &mask[0], size_bytes, false);
  }
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValue(const bf_rt_id_t &field_id,
                                         uint64_t *value) const {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, value, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (key_field->isMatchPriority()) {
    *value = priority;
  } else {
    // Zero out the out param
    *value = 0;
    // ceil the size and convert bits to bytes
    auto size_bytes = (key_field->getSize() + 7) / 8;
    // advance pointer so that the data aligns with the end
    uint8_t *temp_ptr = reinterpret_cast<uint8_t *>(value) + (8 - size_bytes);
    this->getValueInternal(*key_field, temp_ptr, size_bytes);
    *value = be64toh(*value);
  }
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValue(const bf_rt_id_t &field_id,
                                         const size_t &size,
                                         uint8_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, nullptr, value, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (key_field->isMatchPriority()) {
    auto temp_priority = htobe32(priority);
    std::memcpy(value, &temp_priority, size_bytes);
  } else {
    this->getValueInternal(*key_field, value, size_bytes);
  }
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValueandMask(const bf_rt_id_t &field_id,
                                                uint64_t *value1,
                                                uint64_t *value2) const {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::TERNARY, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, value1, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, value2, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  // Zero out the out-params for sanity
  *value1 = 0;
  *value2 = 0;
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  // advance pointer so that the data aligns with the end
  uint8_t *to_get_val = reinterpret_cast<uint8_t *>(value1) + (8 - size_bytes);
  uint8_t *to_get_msk = reinterpret_cast<uint8_t *>(value2) + (8 - size_bytes);
  getValueAndMaskInternal(*key_field, to_get_val, to_get_msk, size_bytes);
  *value1 = be64toh(*value1);
  *value2 = be64toh(*value2);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValueandMask(const bf_rt_id_t &field_id,
                                                const size_t &size,
                                                uint8_t *value1,
                                                uint8_t *value2) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::TERNARY, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, nullptr, value1, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, nullptr, value2, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  getValueAndMaskInternal(*key_field, value1, value2, size_bytes);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValueRange(const bf_rt_id_t &field_id,
                                              uint64_t *start,
                                              uint64_t *end) const {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::RANGE, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, start, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, end, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  // Zero out the out-params for sanity
  *start = 0;
  *end = 0;
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  // advance pointer so that the data aligns with the end
  uint8_t *to_get_start = reinterpret_cast<uint8_t *>(start) + (8 - size_bytes);
  uint8_t *to_get_end = reinterpret_cast<uint8_t *>(end) + (8 - size_bytes);
  getValueAndMaskInternal(*key_field, to_get_start, to_get_end, size_bytes);
  *start = be64toh(*start);
  *end = be64toh(*end);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValueRange(const bf_rt_id_t &field_id,
                                              const size_t &size,
                                              uint8_t *start,
                                              uint8_t *end) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::RANGE, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, nullptr, start, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, nullptr, end, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  getValueAndMaskInternal(*key_field, start, end, size_bytes);
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValueLpm(const bf_rt_id_t &field_id,
                                            uint64_t *start,
                                            uint16_t *p_length) const {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::LPM, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, start, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  // Zero out the out-params for sanity
  *start = 0;
  *p_length = key_field->getSize() - priority;
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  uint8_t *to_get_val = reinterpret_cast<uint8_t *>(start) + (8 - size_bytes);

  // get the mask in network order in a local first,
  // To do that, first we have to advance the start pointer
  // by 8 - size_bytes. Pipe_mgr will store
  // 0x00 0x00 0x00 0x00 0x00 0x00 0xAB 0xCD as 0xABCD
  // so we need to ensure that this goes towards the end of the local
  // After that, it will be in proper network order which we
  // change to host again
  uint64_t mask = 0;
  uint8_t *to_get_msk = reinterpret_cast<uint8_t *>(&mask);
  to_get_msk += (8 - size_bytes);

  getValueAndMaskInternal(*key_field, to_get_val, to_get_msk, size_bytes);
  *start = be64toh(*start);

  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValueLpm(const bf_rt_id_t &field_id,
                                            const size_t &size,
                                            uint8_t *start,
                                            uint16_t *p_length) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::LPM, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, nullptr, start, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  std::vector<uint8_t> mask(size_bytes, 0);
  getValueAndMaskInternal(*key_field, start, &mask[0], size_bytes);

  *p_length = key_field->getSize() - priority;
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValueOptional(const bf_rt_id_t &field_id,
                                                 uint64_t *value1,
                                                 bool *is_valid) const {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::OPTIONAL, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, value1, nullptr, 0);
  if (status != BF_SUCCESS) {
    return status;
  }
  uint64_t mask = 0;
  // Zero out the out-params for sanity
  *value1 = 0;
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  // advance pointer so that the data aligns with the end
  uint8_t *to_get_val = reinterpret_cast<uint8_t *>(value1) + (8 - size_bytes);
  uint8_t *to_get_msk = reinterpret_cast<uint8_t *>(&mask) + (8 - size_bytes);
  getValueAndMaskInternal(*key_field, to_get_val, to_get_msk, size_bytes);
  *value1 = be64toh(*value1);
  mask = be64toh(mask);
  *is_valid = mask == 0 ? false : true;
  return BF_SUCCESS;
}

bf_status_t BfRtMatchActionKey::getValueOptional(const bf_rt_id_t &field_id,
                                                 const size_t &size,
                                                 uint8_t *value1,
                                                 bool *is_valid) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::OPTIONAL, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *table_, *key_field, nullptr, value1, size);
  if (status != BF_SUCCESS) {
    return status;
  }
  uint64_t mask = 0;
  uint8_t *to_get_msk = reinterpret_cast<uint8_t *>(&mask) + (8 - size_bytes);
  getValueAndMaskInternal(*key_field, value1, to_get_msk, size_bytes);
  *is_valid = mask == 0 ? false : true;
  return BF_SUCCESS;
}

void BfRtMatchActionKey::populate_match_spec(
    pipe_tbl_match_spec_t *pipe_match_spec) const {
  pipe_match_spec->num_valid_match_bits = num_valid_match_bits;
  pipe_match_spec->num_match_bytes = num_valid_match_bytes;
  pipe_match_spec->partition_index = partition_index;
  pipe_match_spec->match_value_bits = key_array;
  pipe_match_spec->match_mask_bits = mask_array;
  pipe_match_spec->priority = priority;
  return;
}

void BfRtMatchActionKey::set_key_from_match_spec_by_deepcopy(
    const pipe_tbl_match_spec_t *p) {
  this->num_valid_match_bits = p->num_valid_match_bits;
  this->num_valid_match_bytes = p->num_match_bytes;
  this->partition_index = p->partition_index;
  this->priority = p->priority;

  if (key_array) delete[] key_array;
  if (mask_array) delete[] mask_array;
  this->key_array = new uint8_t[this->num_valid_match_bytes];
  this->mask_array = new uint8_t[this->num_valid_match_bytes];

  std::memcpy(this->key_array, p->match_value_bits, p->num_match_bytes);
  std::memcpy(this->mask_array, p->match_mask_bits, p->num_match_bytes);

  return;
}

// Be careful while using this function. This assumes that that the
// pipe_match_spec
// will be persistent in memory, since this is shallow copy
// Also, num_valid_match bits and bytes are supposed to be read-only
// which are set at the time of creation of Key Object, so not touching them
void BfRtMatchActionKey::populate_key_from_match_spec(
    const pipe_tbl_match_spec_t &pipe_match_spec) {
  partition_index = pipe_match_spec.partition_index;
  key_array = pipe_match_spec.match_value_bits;
  mask_array = pipe_match_spec.match_mask_bits;
  priority = pipe_match_spec.priority;
  return;
}

// This function extracts the value programmed by the user from the match
// spec byte buf, into the field buf, bit by bit. It extracts each bit
// from the correct location (between the start and end bit) from the
// match spec byte buf one by one and then puts it in the field buf.
// All the byte buffers involved are network ordered
void BfRtMatchActionKey::unpackFieldFromMatchSpecByteBuffer(
    const BfRtTableKeyField &key_field,
    const size_t &size,
    const uint8_t *match_spec_buf,
    uint8_t *field_buf) const {
  // Get the start bit and the end bit of the concerned field
  size_t start_bit = key_field.getStartBit();
  size_t end_bit = start_bit + key_field.getSize() - 1;
  // empty field_buf first
  std::memset(field_buf, 0, size);

  // Extract all the bits between start and end bit from the match spec byte
  // buf into the field buf
  size_t j = 0;
  for (size_t i = start_bit; i <= end_bit; i++, j++) {
    uint8_t temp =
        (match_spec_buf[key_field.getParentFieldFullByteSize() - (i / 8) - 1] >>
         (i % 8)) &
        0x01;

    // Left shift the extracted bit into its correct location
    field_buf[size - (j / 8) - 1] |= (temp << (j % 8));
  }
}

// This function slides in the field buf passed in by the user, in the match
// spec byte buffer at the correct location, bit by bit. It extracts each
// bit of the field buf one by one and then puts it in the correct location
// in the match spec byte buffer. One Important thing to remember is that
// all the byte buffers involved are network ordered
void BfRtMatchActionKey::packFieldIntoMatchSpecByteBuffer(
    const BfRtTableKeyField &key_field,
    const size_t &size,
    const bool &do_masking,
    const uint8_t *field_buf,
    const uint8_t *field_mask_buf,
    uint8_t *match_spec_buf,
    uint8_t *match_mask_spec_buf,
    const BfRtTableObj *table) {
  // Get the start bit and the end bit of the concerned field
  size_t start_bit = key_field.getStartBit();
  size_t end_bit = start_bit + key_field.getSize() - 1;

  // Extract all the bits from field buf into the match spec byte buf between
  // the start bit and the end bit
  size_t j = 0;
  for (size_t i = start_bit; i <= end_bit; i++, j++) {
    uint8_t temp = (field_buf[size - (j / 8) - 1] >> (j % 8)) & 0x01;
    uint8_t temp_mask = 0x01;
    // This function is called even for EXM key fields for which the field mask
    // is defaulted to all zeros. So don't touch the temp_mask
    if (field_mask_buf != nullptr) {
      temp_mask = (field_mask_buf[size - (j / 8) - 1] >> (j % 8)) & 0x01;
      if (do_masking) {
        temp &= temp_mask;
      }
    }

    // Left shift the extracted bit into its correct location
    match_spec_buf[key_field.getParentFieldFullByteSize() - (i / 8) - 1] |=
        (temp << (i % 8));
    match_mask_spec_buf[key_field.getParentFieldFullByteSize() - (i / 8) - 1] |=
        (temp_mask << (i % 8));
  }
  // For exact match fields do_masking is false. Exact match fields need all
  // mask to be ffs. Also, only do this if backend table is exm.
  // Note: Even range do_masking is false, but since field_slices aren't even
  // supported for range right now, we need not think about it
  if (!do_masking && !table->getIsTernaryTable()) {
    for (size_t index = 0; index < key_field.getParentFieldFullByteSize();
         index++) {
      match_mask_spec_buf[index] = 0xff;
    }
  }

  return;
}

void BfRtMatchActionKey::setValueInternal(const BfRtTableKeyField &key_field,
                                          const uint8_t *value,
                                          const size_t &num_bytes) {
  const size_t field_offset = key_field.getOffset();
  if (key_field.isFieldSlice()) {
    this->packFieldIntoMatchSpecByteBuffer(key_field,
                                           num_bytes,
                                           false,
                                           value,
                                           nullptr,
                                           key_array + field_offset,
                                           mask_array + field_offset,
                                           this->table_);
  } else {
    std::memcpy(key_array + field_offset, value, num_bytes);
    // Set the mask to all 1's since this is an exact key field
    std::memset(mask_array + field_offset, 0xff, num_bytes);
    // For ternary match tables, if we have mix of exact match and
    // ternary match key fields, then mask needs to be set correctly
    // based on the key field size in bits even for exact match key fields.
    if (this->table_->getIsTernaryTable()) {
      uint32_t field_size = key_field.getSize();
      if (field_size % 8) {
        uint8_t mask = ((1 << (field_size % 8)) - 1);
        mask_array[field_offset] = mask;
      }
    }
  }
}

void BfRtMatchActionKey::getValueInternal(const BfRtTableKeyField &key_field,
                                          uint8_t *value,
                                          const size_t &num_bytes) const {
  const size_t field_offset = key_field.getOffset();
  if (key_field.isFieldSlice()) {
    this->unpackFieldFromMatchSpecByteBuffer(
        key_field, num_bytes, key_array + field_offset, value);
  } else {
    std::memcpy(value, key_array + field_offset, num_bytes);
  }
}

void BfRtMatchActionKey::setValueAndMaskInternal(
    const BfRtTableKeyField &key_field,
    const uint8_t *value,
    const uint8_t *mask,
    const size_t &num_bytes,
    bool do_masking) {
  const size_t field_offset = key_field.getOffset();
  if (key_field.isFieldSlice()) {
    this->packFieldIntoMatchSpecByteBuffer(key_field,
                                           num_bytes,
                                           do_masking,
                                           value,
                                           mask,
                                           key_array + field_offset,
                                           mask_array + field_offset,
                                           this->table_);
  } else {
    uint8_t *value_ptr = key_array + field_offset;
    uint8_t *mask_ptr = mask_array + field_offset;
    std::memcpy(key_array + field_offset, value, num_bytes);
    std::memcpy(mask_array + field_offset, mask, num_bytes);
    if (do_masking) {
      for (uint32_t i = 0; i < num_bytes; i++) {
        value_ptr[i] = value_ptr[i] & mask_ptr[i];
      }
    }
  }
}

void BfRtMatchActionKey::getValueAndMaskInternal(
    const BfRtTableKeyField &key_field,
    uint8_t *value,
    uint8_t *mask,
    const size_t &num_bytes) const {
  const size_t field_offset = key_field.getOffset();
  if (key_field.isFieldSlice()) {
    this->unpackFieldFromMatchSpecByteBuffer(
        key_field, num_bytes, key_array + field_offset, value);
    this->unpackFieldFromMatchSpecByteBuffer(
        key_field, num_bytes, mask_array + field_offset, mask);
  } else {
    std::memcpy(value, key_array + field_offset, num_bytes);
    std::memcpy(mask, mask_array + field_offset, num_bytes);
  }
}

void BfRtMatchActionKey::setKeySz() {
  size_t bytes = table_->getKeySize().bytes;
  key_array = new uint8_t[bytes];
  std::memset(key_array, 0, bytes);
  mask_array = new uint8_t[bytes];
  std::memset(mask_array, 0, bytes);
  num_valid_match_bytes = bytes;
  num_valid_match_bits = table_->getKeySize().bits;
}

bf_status_t BfRtMatchActionKey::reset() {
  size_t key_size = table_->getKeySize().bytes;
  if (!key_size) {
    // If the key size is ZERO, nothing to do. Think KEYLESS tables and the
    // likes
    return BF_SUCCESS;
  }

  if (key_array) {
    std::memset(key_array, 0, key_size);
  }
  if (mask_array) {
    std::memset(mask_array, 0, key_size);
  };
  partition_index = 0;
  priority = 0;
  return BF_SUCCESS;
}

/* Snapshot */
bf_status_t BfRtSnapshotConfigTableKey::setValue(const bf_rt_id_t &field_id,
                                                 const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  const BfRtSnapshotConfigTable *sc_table =
      static_cast<const BfRtSnapshotConfigTable *>(this->table_);
  // Check if value is valid. Table size represent number of stages, so
  // maximum key value is 1 less than that.
  if (value >= sc_table->getSize()) {
    LOG_ERROR(
        "%s:%d %s Maximum value supported for field %d is %d, provided "
        "%" PRIu64,
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        field_id,
        sc_table->getSize() - 1,
        value);
    return BF_INVALID_ARG;
  }
  this->uint32_fields[field_id] = static_cast<uint32_t>(value);
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotConfigTableKey::setValue(const bf_rt_id_t &field_id,
                                                 const uint8_t *value,
                                                 const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &val);

  const BfRtSnapshotConfigTable *sc_table =
      static_cast<const BfRtSnapshotConfigTable *>(this->table_);
  // Check if value is valid. Table size represent number of stages, so
  // maximum key value is 1 less than that.
  if (val >= sc_table->getSize()) {
    LOG_ERROR(
        "%s:%d %s Maximum value supported for field %d is %d, provided "
        "%" PRIu64,
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        field_id,
        sc_table->getSize() - 1,
        val);
    return BF_INVALID_ARG;
  }
  this->uint32_fields[field_id] = static_cast<uint32_t>(val);

  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotConfigTableKey::getValue(const bf_rt_id_t &field_id,
                                                 uint64_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  uint64_t val = 0;
  auto elem = this->uint32_fields.find(field_id);
  if (elem != this->uint32_fields.end()) {
    val = this->uint32_fields.at(field_id);
  }
  *value = val;
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotConfigTableKey::getValue(const bf_rt_id_t &field_id,
                                                 const size_t &size,
                                                 uint8_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  auto elem = this->uint32_fields.find(field_id);
  if (elem != this->uint32_fields.end()) {
    val = this->uint32_fields.at(field_id);
  }

  utils::BfRtTableFieldUtils::toNetworkOrderData(*key_field, val, value);
  return BF_SUCCESS;
}

void BfRtSnapshotConfigTableKey::get_stage_id(uint32_t *start_stage_p,
                                              uint32_t *end_stage_p) const {
  bool start_found = false, end_found = false;

  std::vector<bf_rt_id_t> ids;
  auto status = this->table_->keyFieldIdListGet(&ids);
  if (status) {
    LOG_ERROR("Unable to get key fields list for SnapshotConfig table");
    return;
  }
  uint64_t field_val;
  for (auto it = ids.begin(); it != ids.end(); ++it) {
    std::string name;
    this->table_->keyFieldNameGet(*it, &name);
    /* Set the start and end stage */
    if (name.compare("start_stage") == 0) {
      this->getValue(*it, &field_val);
      *start_stage_p = field_val;
      start_found = true;
    }
    if (name.compare("end_stage") == 0) {
      this->getValue(*it, &field_val);
      *end_stage_p = field_val;
      end_found = true;
    }
  }
  if (!start_found || !end_found) {
    LOG_ERROR(
        "%s:%d ERROR : Start or end (Trigger) stage missing in the table keys",
        __func__,
        __LINE__);
  }
}

void BfRtSnapshotConfigTableKey::set_stage_id(const uint32_t start_stage,
                                              const uint32_t end_stage) {
  bool start_found = false, end_found = false;
  std::vector<bf_rt_id_t> ids;
  auto status = this->table_->keyFieldIdListGet(&ids);
  if (status) {
    LOG_ERROR("Unable to get key fields list for SnapshotConfig table");
    return;
  }
  for (auto it = ids.begin(); it != ids.end(); ++it) {
    std::string name;
    this->table_->keyFieldNameGet(*it, &name);
    /* Set the start and end stage */
    if (name.compare("start_stage") == 0) {
      this->setValue(*it, start_stage);
      start_found = true;
    }
    if (name.compare("end_stage") == 0) {
      this->setValue(*it, end_stage);
      end_found = true;
    }
  }
  if (!start_found || !end_found) {
    LOG_ERROR(
        "%s:%d ERROR : Start or end (Trigger) stage missing in the table keys",
        __func__,
        __LINE__);
  }
}

// Snapshot trigger
bf_status_t BfRtSnapshotTriggerTableKey::setValue(const bf_rt_id_t &field_id,
                                                  const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  const BfRtSnapshotTriggerTable *sc_table =
      static_cast<const BfRtSnapshotTriggerTable *>(this->table_);
  if (value >= sc_table->getSize()) {
    return BF_INVALID_ARG;
  }
  this->uint32_fields[field_id] = static_cast<uint32_t>(value);
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotTriggerTableKey::setValue(const bf_rt_id_t &field_id,
                                                  const uint8_t *value,
                                                  const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &val);

  const BfRtSnapshotTriggerTable *sc_table =
      static_cast<const BfRtSnapshotTriggerTable *>(this->table_);
  if (val >= sc_table->getSize()) {
    return BF_INVALID_ARG;
  }
  this->uint32_fields[field_id] = val;

  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotTriggerTableKey::getValue(const bf_rt_id_t &field_id,
                                                  uint64_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  uint64_t val = 0;
  auto elem = this->uint32_fields.find(field_id);
  if (elem != this->uint32_fields.end()) {
    val = this->uint32_fields.at(field_id);
  }
  *value = val;
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotTriggerTableKey::getValue(const bf_rt_id_t &field_id,
                                                  const size_t &size,
                                                  uint8_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  auto elem = this->uint32_fields.find(field_id);
  if (elem != this->uint32_fields.end()) {
    val = this->uint32_fields.at(field_id);
  }

  utils::BfRtTableFieldUtils::toNetworkOrderData(*key_field, val, value);
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotTriggerTableKey::set_stage_id(const uint32_t stage) {
  bf_rt_id_t field_id;
  auto status = this->table_->keyFieldIdGet("stage", &field_id);
  if (status) {
    LOG_ERROR("Unable to get key field for SnapshotTrigger table");
    return status;
  }
  this->uint32_fields[field_id] = stage;
  return status;
}

bf_status_t BfRtSnapshotTriggerTableKey::get_stage_id(uint32_t *stage) const {
  bf_rt_id_t field_id;
  auto status = this->table_->keyFieldIdGet("stage", &field_id);
  if (status) {
    LOG_ERROR("Unable to get key field for SnapshotTrigger table");
    return status;
  }
  if (this->uint32_fields.find(field_id) != this->uint32_fields.end()) {
    *stage = this->uint32_fields.at(field_id);
    return BF_SUCCESS;
  }
  LOG_ERROR("Value for stage key field doesn't exist");
  return BF_OBJECT_NOT_FOUND;
}

// Snapshot data
bf_status_t BfRtSnapshotDataTableKey::setValue(const bf_rt_id_t &field_id,
                                               const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  const BfRtSnapshotDataTable *sc_table =
      static_cast<const BfRtSnapshotDataTable *>(this->table_);
  if (value >= sc_table->getSize()) {
    return BF_INVALID_ARG;
  }
  this->uint32_fields[field_id] = static_cast<uint32_t>(value);
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotDataTableKey::setValue(const bf_rt_id_t &field_id,
                                               const uint8_t *value,
                                               const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &val);

  const BfRtSnapshotDataTable *sc_table =
      static_cast<const BfRtSnapshotDataTable *>(this->table_);
  if (val >= sc_table->getSize()) {
    return BF_INVALID_ARG;
  }
  this->uint32_fields[field_id] = static_cast<uint32_t>(val);

  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotDataTableKey::getValue(const bf_rt_id_t &field_id,
                                               uint64_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  uint64_t val = 0;
  auto elem = this->uint32_fields.find(field_id);
  if (elem != this->uint32_fields.end()) {
    val = this->uint32_fields.at(field_id);
  }
  *value = val;
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotDataTableKey::getValue(const bf_rt_id_t &field_id,
                                               const size_t &size,
                                               uint8_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  auto elem = this->uint32_fields.find(field_id);
  if (elem != this->uint32_fields.end()) {
    val = this->uint32_fields.at(field_id);
  }

  utils::BfRtTableFieldUtils::toNetworkOrderData(*key_field, val, value);
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotDataTableKey::set_stage_id(const uint32_t stage) {
  bf_rt_id_t field_id;
  auto status = this->table_->keyFieldIdGet("stage", &field_id);
  if (status) {
    LOG_ERROR("Unable to get key field for SnapshotData table");
    return status;
  }
  this->uint32_fields[field_id] = stage;
  return status;
}

bf_status_t BfRtSnapshotDataTableKey::get_stage_id(uint32_t *stage) const {
  bf_rt_id_t field_id;
  auto status = this->table_->keyFieldIdGet("stage", &field_id);
  if (status) {
    LOG_ERROR("Unable to get key field for SnapshotData table");
    return status;
  }
  if (this->uint32_fields.find(field_id) != this->uint32_fields.end()) {
    *stage = this->uint32_fields.at(field_id);
    return BF_SUCCESS;
  }
  LOG_ERROR("Value for stage key field doesn't exist");
  return BF_OBJECT_NOT_FOUND;
}

/* Snapshot Liveness*/
bf_status_t BfRtSnapshotLivenessTableKey::getFieldName(
    std::string *value) const {
  *value = field_name;
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotLivenessTableKey::setValue(
    const bf_rt_id_t &field_id, const std::string &str_value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  field_name = str_value;

  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotLivenessTableKey::getValue(const bf_rt_id_t &field_id,
                                                   std::string *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  *value = field_name;
  return BF_SUCCESS;
}

// Snapshot Phv Key
bf_status_t BfRtSnapshotPhvTableKey::setValue(const bf_rt_id_t &field_id,
                                              const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  // This table size represent number of stages and max supported value
  const BfRtSnapshotPhvTable *phvtable =
      static_cast<const BfRtSnapshotPhvTable *>(this->table_);
  if (value >= phvtable->getSize()) {
    return BF_INVALID_ARG;
  }
  this->stage_id = static_cast<uint32_t>(value);
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotPhvTableKey::setValue(const bf_rt_id_t &field_id,
                                              const uint8_t *value,
                                              const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &val);
  this->stage_id = static_cast<uint32_t>(val);

  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotPhvTableKey::getValue(const bf_rt_id_t &field_id,
                                              uint64_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  *value = static_cast<uint64_t>(this->stage_id);
  return BF_SUCCESS;
}

bf_status_t BfRtSnapshotPhvTableKey::getValue(const bf_rt_id_t &field_id,
                                              const size_t &size,
                                              uint8_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint64_t val = this->stage_id;
  utils::BfRtTableFieldUtils::toNetworkOrderData(*key_field, val, value);
  return BF_SUCCESS;
}

void BfRtPVSTableKey::setValueAndMaskInternal(const size_t &field_offset,
                                              const uint8_t *pvs_value,
                                              const uint8_t *pvs_mask,
                                              const size_t &num_bytes) {
  // For PVS, the individual fields are stored in host order in the 32 bit
  // field. For eg. if the PVS had 2 fields, field 1: f16(16 bits),
  // field 2: f8(8 bits), if the user set f16 to 0x1234 and f8 to 0x56, the
  // combined key_ field for this case would be key_ = 0x00561234
  uint8_t *value_ptr = reinterpret_cast<uint8_t *>(&key_);
  uint8_t *mask_ptr = reinterpret_cast<uint8_t *>(&mask_);
  std::memcpy(value_ptr + field_offset, pvs_value, num_bytes);
  std::memcpy(mask_ptr + field_offset, pvs_mask, num_bytes);
}

void BfRtPVSTableKey::getValueAndMaskInternal(const size_t &field_offset,
                                              uint8_t *pvs_value,
                                              uint8_t *pvs_mask,
                                              const size_t &num_bytes) const {
  uint32_t key_temp = key_;
  uint32_t mask_temp = mask_;
  uint8_t *value_ptr = reinterpret_cast<uint8_t *>(&key_temp);
  uint8_t *mask_ptr = reinterpret_cast<uint8_t *>(&mask_temp);
  std::memcpy(pvs_value, value_ptr + field_offset, num_bytes);
  std::memcpy(pvs_mask, mask_ptr + field_offset, num_bytes);
}

// BfRtActionTableKey *****************

bf_status_t BfRtActionTableKey::setValue(const bf_rt_id_t &field_id,
                                         const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  member_id = value;
  return BF_SUCCESS;
}

bf_status_t BfRtActionTableKey::setValue(const bf_rt_id_t &field_id,
                                         const uint8_t *value,
                                         const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(bf_rt_id_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(bf_rt_id_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &val);
  this->member_id = static_cast<uint32_t>(val);

  return BF_SUCCESS;
}

bf_status_t BfRtActionTableKey::getValue(const bf_rt_id_t &field_id,
                                         uint64_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  *value = member_id;
  return BF_SUCCESS;
}

bf_status_t BfRtActionTableKey::getValue(const bf_rt_id_t &field_id,
                                         const size_t &size,
                                         uint8_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(bf_rt_id_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(bf_rt_id_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint64_t val = this->member_id;
  utils::BfRtTableFieldUtils::toNetworkOrderData(*key_field, val, value);
  return BF_SUCCESS;
}

// BfRtSelectorTableKey *****************

bf_status_t BfRtSelectorTableKey::setValue(const bf_rt_id_t &field_id,
                                           const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  group_id = value;
  return BF_SUCCESS;
}

bf_status_t BfRtSelectorTableKey::setValue(const bf_rt_id_t &field_id,
                                           const uint8_t *value,
                                           const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(bf_rt_id_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(bf_rt_id_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &val);
  this->group_id = val;

  return BF_SUCCESS;
}

bf_status_t BfRtSelectorTableKey::getValue(const bf_rt_id_t &field_id,
                                           uint64_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  *value = group_id;
  return BF_SUCCESS;
}

bf_status_t BfRtSelectorTableKey::getValue(const bf_rt_id_t &field_id,
                                           const size_t &size,
                                           uint8_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s ERROR Array size of %zd is not equal to the field size %zd "
        "for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint32_t local_val = htobe32(group_id);
  std::memcpy(value, &local_val, sizeof(uint32_t));
  return BF_SUCCESS;
}

bf_status_t BfRtCounterTableKey::setValue(const bf_rt_id_t &field_id,
                                          const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  counter_id = value;
  return BF_SUCCESS;
}

bf_status_t BfRtCounterTableKey::setValue(const bf_rt_id_t &field_id,
                                          const uint8_t *value,
                                          const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s ERROR Array size of %zd is not equal to the field size %zd "
        "for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint32_t val = *(reinterpret_cast<const uint32_t *>(value));
  counter_id = be32toh(val);
  return BF_SUCCESS;
}

template <class T>
bf_status_t getKeyIdxValue(const T &key,
                           const BfRtTableObj &table,
                           const bf_rt_id_t &field_id,
                           uint64_t *value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, &table);
  if (status != BF_SUCCESS) {
    return status;
  }
  *value = key.getIdxKey();
  return BF_SUCCESS;
}

template <class T>
bf_status_t getKeyIdxValue(const T &key,
                           const BfRtTableObj &table,
                           const bf_rt_id_t &field_id,
                           uint8_t *value,
                           const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, &table);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s ERROR Array size of %zd is not equal to the field size %zd "
        "for field id %d",
        __func__,
        __LINE__,
        table.table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint32_t local_val = htobe32(key.getIdxKey());
  std::memcpy(value, &local_val, sizeof(uint32_t));
  return BF_SUCCESS;
}

bf_status_t BfRtCounterTableKey::getValue(const bf_rt_id_t &field_id,
                                          uint64_t *value) const {
  return getKeyIdxValue<BfRtCounterTableKey>(*this, *table_, field_id, value);
}

bf_status_t BfRtCounterTableKey::getValue(const bf_rt_id_t &field_id,
                                          const size_t &size,
                                          uint8_t *value) const {
  return getKeyIdxValue<BfRtCounterTableKey>(
      *this, *table_, field_id, value, size);
}

bf_status_t BfRtMeterTableKey::getValue(const bf_rt_id_t &field_id,
                                        uint64_t *value) const {
  return getKeyIdxValue<BfRtMeterTableKey>(*this, *table_, field_id, value);
}

bf_status_t BfRtMeterTableKey::getValue(const bf_rt_id_t &field_id,
                                        const size_t &size,
                                        uint8_t *value) const {
  return getKeyIdxValue<BfRtMeterTableKey>(
      *this, *table_, field_id, value, size);
}

bf_status_t BfRtMeterTableKey::setValue(const bf_rt_id_t &field_id,
                                        const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  meter_id = value;
  return BF_SUCCESS;
}

bf_status_t BfRtMeterTableKey::setValue(const bf_rt_id_t &field_id,
                                        const uint8_t *value,
                                        const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s ERROR Array size of %zd is not equal to the field size %zd "
        "for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint32_t val = *(reinterpret_cast<const uint32_t *>(value));
  val = be32toh(val);
  meter_id = val;
  return BF_SUCCESS;
}

bf_status_t BfRtLPFTableKey::getValue(const bf_rt_id_t &field_id,
                                      uint64_t *value) const {
  return getKeyIdxValue<BfRtLPFTableKey>(*this, *table_, field_id, value);
}

bf_status_t BfRtLPFTableKey::getValue(const bf_rt_id_t &field_id,
                                      const size_t &size,
                                      uint8_t *value) const {
  return getKeyIdxValue<BfRtLPFTableKey>(*this, *table_, field_id, value, size);
}

bf_status_t BfRtLPFTableKey::setValue(const bf_rt_id_t &field_id,
                                      const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  lpf_id = value;
  return BF_SUCCESS;
}

bf_status_t BfRtLPFTableKey::setValue(const bf_rt_id_t &field_id,
                                      const uint8_t *value,
                                      const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s ERROR Array size of %zd is not equal to the field size %zd "
        "for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint32_t val = *(reinterpret_cast<const uint32_t *>(value));
  val = be32toh(val);
  lpf_id = val;
  return BF_SUCCESS;
}

bf_status_t BfRtWREDTableKey::getValue(const bf_rt_id_t &field_id,
                                       uint64_t *value) const {
  return getKeyIdxValue<BfRtWREDTableKey>(*this, *table_, field_id, value);
}

bf_status_t BfRtWREDTableKey::getValue(const bf_rt_id_t &field_id,
                                       const size_t &size,
                                       uint8_t *value) const {
  return getKeyIdxValue<BfRtWREDTableKey>(
      *this, *table_, field_id, value, size);
}

bf_status_t BfRtWREDTableKey::setValue(const bf_rt_id_t &field_id,
                                       const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  wred_id = value;
  return BF_SUCCESS;
}

bf_status_t BfRtWREDTableKey::setValue(const bf_rt_id_t &field_id,
                                       const uint8_t *value,
                                       const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s ERROR Array size of %zd is not equal to the field size %zd "
        "for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint32_t val = *(reinterpret_cast<const uint32_t *>(value));
  val = be32toh(val);
  wred_id = val;
  return BF_SUCCESS;
}

bf_status_t BfRtRegisterTableKey::getValue(const bf_rt_id_t &field_id,
                                           uint64_t *value) const {
  return getKeyIdxValue<BfRtRegisterTableKey>(*this, *table_, field_id, value);
}

bf_status_t BfRtRegisterTableKey::getValue(const bf_rt_id_t &field_id,
                                           const size_t &size,
                                           uint8_t *value) const {
  return getKeyIdxValue<BfRtRegisterTableKey>(
      *this, *table_, field_id, value, size);
}

bf_status_t BfRtRegisterTableKey::setValue(const bf_rt_id_t &field_id,
                                           const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  register_id = value;
  return BF_SUCCESS;
}

bf_status_t BfRtRegisterTableKey::setValue(const bf_rt_id_t &field_id,
                                           const uint8_t *value,
                                           const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s ERROR Array size of %zd is not equal to the field size %zd "
        "for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  uint32_t val = *(reinterpret_cast<const uint32_t *>(value));
  val = be32toh(val);
  register_id = val;
  return BF_SUCCESS;
}

bf_status_t BfRtPVSTableKey::setValueandMask(const bf_rt_id_t &field_id,
                                             const uint8_t *value1,
                                             const uint8_t *value2,
                                             const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::TERNARY, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  size_t size_bytes = (key_field->getSize() + 7) / 8;
  // Check size compatibility for TERNARY
  if (((key_field->getOffset() + size_bytes) > 4) || (size > size_bytes)) {
    LOG_ERROR(
        "%s:%d Error. Length given is %zu bytes. Field length "
        "is %zu bits. Expecting <= %zu bytes",
        __func__,
        __LINE__,
        size,
        key_field->getSize(),
        size_bytes);
    return BF_INVALID_ARG;
  }
  /* value1 and value2 are byte arrays holding the key and mask.  The pipe_mgr
   * API that will eventually be called to program a PVS takes two uint32_t
   * values in host order (most other APIs take key fields in network order).
   * So convert the byte arrays to two uint32_t values in host order. */
  uint32_t val1 = 0, val2 = 0;
  for (size_t i = 0; i < size; ++i) {
    uint32_t t1 = value1[i];
    val1 |= t1 << (24 - 8 * i);
    uint32_t t2 = value2[i];
    val2 |= t2 << (24 - 8 * i);
  }
  uint8_t *val1_ptr = reinterpret_cast<uint8_t *>(&val1);
  uint8_t *val2_ptr = reinterpret_cast<uint8_t *>(&val2);
  uint32_t offset = 4 - size_bytes;
  setValueAndMaskInternal(
      key_field->getOffset(), val1_ptr + offset, val2_ptr + offset, size_bytes);
  return BF_SUCCESS;
}

bf_status_t BfRtPVSTableKey::getValueandMask(const bf_rt_id_t &field_id,
                                             const size_t &size,
                                             uint8_t *value1,
                                             uint8_t *value2) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::TERNARY, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  // ceil the size and convert bits to bytes
  auto size_bytes = (key_field->getSize() + 7) / 8;
  if (size != size_bytes) {
    LOG_ERROR("%s:%d Size requested %zu is different than field size %zu bytes",
              __func__,
              __LINE__,
              size,
              size_bytes);
    return BF_INVALID_ARG;
  }
  uint32_t val1 = 0;
  uint32_t val2 = 0;
  uint8_t *val1_ptr = reinterpret_cast<uint8_t *>(&val1);
  uint8_t *val2_ptr = reinterpret_cast<uint8_t *>(&val2);
  getValueAndMaskInternal(
      key_field->getOffset(), val1_ptr, val2_ptr, size_bytes);
  // As the individual fields are stored in host order in the combined key for
  // PVS, convert the read values to network order
  val1 = htobe32(val1);
  val2 = htobe32(val2);
  uint32_t offset = 4 - size_bytes;
  // Since the size of the field can be less than 4 bytes, move the pointer to
  // point at the correct byte in the network ordered value before we try to
  // copy it
  std::memcpy(value1, val1_ptr + offset, size_bytes);
  std::memcpy(value2, val2_ptr + offset, size_bytes);
  return BF_SUCCESS;
}

bf_status_t BfRtPVSTableKey::setValueandMask(const bf_rt_id_t &field_id,
                                             const uint64_t &value,
                                             const uint64_t &mask) {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::TERNARY, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  auto field_size = key_field->getSize();

  if (field_size > 64) {
    // Error. This setter cannot be used to set this field
    LOG_ERROR(
        "%s:%d %s ERROR : Not supported since fieldsize = %zu bits > 64 for "
        "field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        field_size,
        field_id);
    return BF_NOT_SUPPORTED;
  }

  if (field_size < 64) {
    auto limit = (1ULL << field_size) - 1;
    if (value > limit || mask > limit) {
      // Error. The value specified exceeds the size of the field
      LOG_ERROR(
          "%s:%d %s ERROR Value %" PRIu64
          " is greater than what field size %zu bits allows for field id %d",
          __func__,
          __LINE__,
          table_->table_name_get().c_str(),
          value,
          field_size,
          field_id);
      return BF_INVALID_ARG;
    }
  }

  auto size_bytes = (field_size + 7) / 8;
  const uint8_t *val_ptr = (reinterpret_cast<const uint8_t *>(&value));
  const uint8_t *msk_ptr = (reinterpret_cast<const uint8_t *>(&mask));
  setValueAndMaskInternal(key_field->getOffset(), val_ptr, msk_ptr, size_bytes);
  return BF_SUCCESS;
}

bf_status_t BfRtPVSTableKey::getValueandMask(const bf_rt_id_t &field_id,
                                             uint64_t *value,
                                             uint64_t *mask) const {
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::TERNARY, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (key_field->getSize() > 64) {
    // Error. This getter cannot be used to get this field
    LOG_ERROR(
        "%s:%d %s ERROR : Not supported since fieldsize = %zu bits > 64 for "
        "field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        key_field->getSize(),
        field_id);
    return BF_NOT_SUPPORTED;
  }

  auto size_bytes = (key_field->getSize() + 7) / 8;
  uint8_t *val1_ptr = (reinterpret_cast<uint8_t *>(value));
  uint8_t *val2_ptr = (reinterpret_cast<uint8_t *>(mask));
  memset(val1_ptr, 0x00, sizeof(uint64_t));
  memset(val2_ptr, 0x00, sizeof(uint64_t));
  getValueAndMaskInternal(
      key_field->getOffset(), val1_ptr, val2_ptr, size_bytes);
  // Since for PVS, the individual fields are stored in host order in the
  // combined key field, we can simply return here
  return BF_SUCCESS;
}

/* Debug counter table */
bf_status_t BfRtTblDbgCntTableKey::setValue(const bf_rt_id_t &field_id,
                                            const std::string &str_value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, this->table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  this->tbl_name = str_value;

  return BF_SUCCESS;
}

bf_status_t BfRtTblDbgCntTableKey::getValue(const bf_rt_id_t &field_id,
                                            std::string *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, this->table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (value == nullptr) {
    return BF_INVALID_ARG;
  }

  *value = this->tbl_name;
  return BF_SUCCESS;
}

// Log table debug counter
bf_status_t BfRtLogDbgCntTableKey::setValue(const bf_rt_id_t &field_id,
                                            const uint64_t &value) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  const BfRtLogDbgCntTable *sc_table =
      static_cast<const BfRtLogDbgCntTable *>(this->table_);
  if (value >= sc_table->getSize()) {
    return BF_INVALID_ARG;
  }
  this->fields_[field_id] = static_cast<uint32_t>(value);
  return BF_SUCCESS;
}

bf_status_t BfRtLogDbgCntTableKey::setValue(const bf_rt_id_t &field_id,
                                            const uint8_t *value,
                                            const size_t &size) {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  utils::BfRtTableFieldUtils::toHostOrderData(*key_field, value, &val);

  const BfRtLogDbgCntTable *sc_table =
      static_cast<const BfRtLogDbgCntTable *>(this->table_);
  if (val >= sc_table->getSize()) {
    return BF_INVALID_ARG;
  }
  this->fields_[field_id] = val;

  return BF_SUCCESS;
}

bf_status_t BfRtLogDbgCntTableKey::getValue(const bf_rt_id_t &field_id,
                                            uint64_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }
  if (value == nullptr) {
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  auto elem = this->fields_.find(field_id);
  if (elem != this->fields_.end()) {
    val = this->fields_.at(field_id);
  }
  *value = val;
  return BF_SUCCESS;
}

bf_status_t BfRtLogDbgCntTableKey::getValue(const bf_rt_id_t &field_id,
                                            const size_t &size,
                                            uint8_t *value) const {
  // Get the key_field from the table
  const BfRtTableKeyField *key_field;
  auto status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &key_field, KeyFieldType::EXACT, table_);
  if (status != BF_SUCCESS) {
    return status;
  }

  if (size != sizeof(uint32_t)) {
    LOG_ERROR(
        "%s:%d %s Array size %zd bytes passed is not equal to the size of the "
        "field %zd bytes, for field id %d",
        __func__,
        __LINE__,
        table_->table_name_get().c_str(),
        size,
        sizeof(uint32_t),
        field_id);
    return BF_INVALID_ARG;
  }
  if (value == nullptr) {
    return BF_INVALID_ARG;
  }

  uint64_t val = 0;
  auto elem = this->fields_.find(field_id);
  if (elem != this->fields_.end()) {
    val = this->fields_.at(field_id);
  }

  utils::BfRtTableFieldUtils::toNetworkOrderData(*key_field, val, value);
  return BF_SUCCESS;
}

bf_status_t BfRtDynHashComputeTableKey::setValue(const bf_rt_id_t &field_id,
                                                 const uint64_t &value) {
  return this->setValueInternal(field_id, value, nullptr, 0);
}

bf_status_t BfRtDynHashComputeTableKey::setValue(const bf_rt_id_t &field_id,
                                                 const uint8_t *value,
                                                 const size_t &size) {
  return this->setValueInternal(field_id, 0, value, size);
}

bf_status_t BfRtDynHashComputeTableKey::setValueInternal(
    const bf_rt_id_t &field_id,
    const uint64_t &value,
    const uint8_t *value_ptr,
    const size_t &size) {
  bf_status_t status = BF_SUCCESS;

  const BfRtTableKeyField *tableKeyField = nullptr;
  status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &tableKeyField, KeyFieldType::EXACT, this->table_);
  if (status != BF_SUCCESS) {
    LOG_ERROR("ERROR: %s:%d Invalid field id %d for table %s",
              __func__,
              __LINE__,
              field_id,
              this->table_->table_name_get().c_str());
    return BF_OBJECT_NOT_FOUND;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *this->table_, *tableKeyField, &value, value_ptr, size);
  if (status != BF_SUCCESS) {
    LOG_ERROR(
        "ERROR: %s:%d %s : Input param compatibility check failed for field id "
        "%d",
        __func__,
        __LINE__,
        this->table_->table_name_get().c_str(),
        tableKeyField->getId());
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *this->table_, *tableKeyField, value, value_ptr, size);
  if (status != BF_SUCCESS) {
    LOG_ERROR("%s:%d %s : Input Param bounds check failed for field id %d",
              __func__,
              __LINE__,
              this->table_->table_name_get().c_str(),
              tableKeyField->getId());
    return status;
  }

  uint64_t val = 0;
  if (value_ptr) {
    utils::BfRtTableFieldUtils::toHostOrderData(
        *tableKeyField, value_ptr, &val);
  } else {
    val = value;
  }

  this->uint64_fields[field_id] = val;
  return BF_SUCCESS;
}

bf_status_t BfRtDynHashComputeTableKey::getValue(const bf_rt_id_t &field_id,
                                                 uint64_t *value) const {
  return this->getValueInternal(field_id, value, nullptr, 0);
}

bf_status_t BfRtDynHashComputeTableKey::getValue(const bf_rt_id_t &field_id,
                                                 const size_t &size,
                                                 uint8_t *value) const {
  return this->getValueInternal(field_id, 0, value, size);
}

bf_status_t BfRtDynHashComputeTableKey::getValueInternal(
    const bf_rt_id_t &field_id,
    uint64_t *value,
    uint8_t *value_ptr,
    const size_t &size) const {
  bf_status_t status = BF_SUCCESS;
  const BfRtTableKeyField *tableKeyField = nullptr;

  status = this->table_->getKeyField(field_id, &tableKeyField);
  if (status != BF_SUCCESS) {
    LOG_ERROR("ERROR: %s:%d Invalid field id %d for table %s",
              __func__,
              __LINE__,
              field_id,
              this->table_->table_name_get().c_str());
    return BF_INVALID_ARG;
  }

  // Do some bounds checking using the utility functions
  auto sts = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *this->table_, *tableKeyField, value, value_ptr, size);
  if (sts != BF_SUCCESS) {
    LOG_ERROR(
        "ERROR: %s:%d %s : Input param compatibility check failed for field id "
        "%d",
        __func__,
        __LINE__,
        this->table_->table_name_get().c_str(),
        tableKeyField->getId());
    return sts;
  }
  uint64_t val;
  auto elem = this->uint64_fields.find(field_id);
  if (elem != this->uint64_fields.end()) {
    val = this->uint64_fields.at(field_id);
  } else {
    val = 0;
  }

  if (value_ptr) {
    utils::BfRtTableFieldUtils::toNetworkOrderData(
        *tableKeyField, val, value_ptr);
  } else {
    *value = val;
  }
  return BF_SUCCESS;
}

bool BfRtDynHashComputeTableKey::isConstant(const bf_rt_id_t &field_id,
                                            const BfRtTable *cfg_tbl) const {
  AnnotationSet annotations;
  cfg_tbl->dataFieldAnnotationsGet(field_id, &annotations);

  auto def_an = Annotation("bfrt_p4_constant", "");
  if (annotations.find(def_an) != annotations.end()) {
    return true;
  }
  return false;
}

bf_status_t BfRtDynHashComputeTableKey::attrListGet(
    const BfRtTable *cfg_tbl,
    std::vector<pipe_hash_calc_input_field_attribute_t> *attr_list) const {
  bf_status_t status = BF_SUCCESS;
  for (const auto &field_id_pair : uint64_fields) {
    const auto &field_id = field_id_pair.first;
    const auto &value = field_id_pair.second;

    // Check if it is a constant field. If constant, skip
    // and continue
    if (this->isConstant(field_id, cfg_tbl)) {
      continue;
    }

    const BfRtTableKeyField *tableKeyField = nullptr;
    status |= this->table_->getKeyField(field_id, &tableKeyField);

    uint8_t *stream = new uint8_t[(tableKeyField->getSize() + 7) / 8];

    pipe_hash_calc_input_field_attribute_t attr{0};
    attr.value.stream = stream;
    std::memcpy(stream, &value, (tableKeyField->getSize() + 7) / 8);

    attr.input_field = field_id;
    attr.type = INPUT_FIELD_ATTR_TYPE_STREAM;
    attr_list->push_back(attr);
    // pipe_mgr Hash compute doesn't care about
    // slice start bit, length or even order. So doesn't
    // matter what is put in those struct fields in attr
  }
  return status;
}

bf_status_t BfRtSelectorGetMemberTableKey::setValue(const bf_rt_id_t &field_id,
                                                    const uint64_t &value) {
  return this->setValueInternal(field_id, value, nullptr, 0);
}

bf_status_t BfRtSelectorGetMemberTableKey::setValue(const bf_rt_id_t &field_id,
                                                    const uint8_t *value,
                                                    const size_t &size) {
  return this->setValueInternal(field_id, 0, value, size);
}

bf_status_t BfRtSelectorGetMemberTableKey::setValueInternal(
    const bf_rt_id_t &field_id,
    const uint64_t &value,
    const uint8_t *value_ptr,
    const size_t &size) {
  bf_status_t status = BF_SUCCESS;

  const BfRtTableKeyField *tableKeyField = nullptr;
  status = utils::BfRtTableFieldUtils::getKeyFieldSafe(
      field_id, &tableKeyField, KeyFieldType::EXACT, this->table_);
  if (status != BF_SUCCESS) {
    LOG_ERROR("ERROR: %s:%d Invalid field id %d for table %s",
              __func__,
              __LINE__,
              field_id,
              this->table_->table_name_get().c_str());
    return BF_OBJECT_NOT_FOUND;
  }
  status = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *this->table_, *tableKeyField, &value, value_ptr, size);
  if (status != BF_SUCCESS) {
    LOG_ERROR(
        "ERROR: %s:%d %s : Input param compatibility check failed for field id "
        "%d",
        __func__,
        __LINE__,
        this->table_->table_name_get().c_str(),
        tableKeyField->getId());
    return status;
  }
  status = utils::BfRtTableFieldUtils::boundsCheck(
      *this->table_, *tableKeyField, value, value_ptr, size);
  if (status != BF_SUCCESS) {
    LOG_ERROR("%s:%d %s : Input Param bounds check failed for field id %d",
              __func__,
              __LINE__,
              this->table_->table_name_get().c_str(),
              tableKeyField->getId());
    return status;
  }

  uint64_t val = 0;
  if (value_ptr) {
    utils::BfRtTableFieldUtils::toHostOrderData(
        *tableKeyField, value_ptr, &val);
  } else {
    val = value;
  }

  this->uint64_fields[field_id] = val;
  return BF_SUCCESS;
}

bf_status_t BfRtSelectorGetMemberTableKey::getValue(const bf_rt_id_t &field_id,
                                                    uint64_t *value) const {
  return this->getValueInternal(field_id, value, nullptr, 0);
}

bf_status_t BfRtSelectorGetMemberTableKey::getValue(const bf_rt_id_t &field_id,
                                                    const size_t &size,
                                                    uint8_t *value) const {
  return this->getValueInternal(field_id, 0, value, size);
}

bf_status_t BfRtSelectorGetMemberTableKey::getValueInternal(
    const bf_rt_id_t &field_id,
    uint64_t *value,
    uint8_t *value_ptr,
    const size_t &size) const {
  bf_status_t status = BF_SUCCESS;
  const BfRtTableKeyField *tableKeyField = nullptr;

  status = this->table_->getKeyField(field_id, &tableKeyField);
  if (status != BF_SUCCESS) {
    LOG_ERROR("ERROR: %s:%d Invalid field id %d for table %s",
              __func__,
              __LINE__,
              field_id,
              this->table_->table_name_get().c_str());
    return BF_INVALID_ARG;
  }

  // Do some bounds checking using the utility functions
  auto sts = utils::BfRtTableFieldUtils::fieldTypeCompatibilityCheck(
      *this->table_, *tableKeyField, value, value_ptr, size);
  if (sts != BF_SUCCESS) {
    LOG_ERROR(
        "ERROR: %s:%d %s : Input param compatibility check failed for field id "
        "%d",
        __func__,
        __LINE__,
        this->table_->table_name_get().c_str(),
        tableKeyField->getId());
    return sts;
  }
  uint64_t val;
  auto elem = this->uint64_fields.find(field_id);
  if (elem != this->uint64_fields.end()) {
    val = this->uint64_fields.at(field_id);
  } else {
    val = 0;
  }

  if (value_ptr) {
    utils::BfRtTableFieldUtils::toNetworkOrderData(
        *tableKeyField, val, value_ptr);
  } else {
    *value = val;
  }
  return BF_SUCCESS;
}

}  // namespace bfrt
