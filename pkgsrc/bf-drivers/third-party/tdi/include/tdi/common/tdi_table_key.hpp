// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_table_key.hpp
 *
 *  @brief Contains TDI Table Key APIs
 */
#ifndef _TDI_TABLE_KEY_HPP
#define _TDI_TABLE_KEY_HPP

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <tdi/common/tdi_defs.h>

namespace tdi {

// Forward declarations
class Table;

/**
 * @brief Thin wrapper over Key Field values. Users are free to access the
 * public data members and manipulate if necessary.
 * Each derived class has 2 constructors, One is a templatized version of
 * primitive data types. The other one is byte array version. Both
 * TableKey::setValue and TableKey::getValue() expect memory to be allocated by
 * user if byte array versions are being used.
 *
 * Note for Target driver developers: match type is being saved so as to save
 * from dynamic casting and figure out what match type was used.
 *
 */
class KeyFieldValue {
 public:
  virtual ~KeyFieldValue() = default;
  KeyFieldValue() = delete;
  const tdi_match_type_e &matchTypeGet() const { return match_type_; };
  const bool &is_pointer() const { return is_pointer_; };

 protected:
  KeyFieldValue(tdi_match_type_e match_type, size_t size, bool is_pointer)
      : match_type_(match_type), is_pointer_(is_pointer), size_(size){};

 private:
  const tdi_match_type_e match_type_;
  const bool is_pointer_;

 public:
  // Size in bytes
  // Accessible publicly
  size_t size_{0};
};

template <class T = uint64_t>
class KeyFieldValueExact : public KeyFieldValue {
 public:
  template <
      typename U = T,
      typename = typename std::enable_if<!std::is_pointer<U>::value>::type>
  KeyFieldValueExact(U value)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_EXACT),
                      sizeof(value),
                      std::is_pointer<U>::value),
        value_(value) {}

  template <typename U = T,
            typename = typename std::enable_if<std::is_pointer<U>::value>::type>
  KeyFieldValueExact(U value, size_t size)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_EXACT),
                      size,
                      std::is_pointer<U>::value),
        value_(value) {}
  T value_ = 0;
};

template <class T = uint64_t>
class KeyFieldValueTernary : public KeyFieldValue {
 public:
  template <
      typename U = T,
      typename = typename std::enable_if<!std::is_pointer<U>::value>::type>
  KeyFieldValueTernary(U &value, U &mask)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_TERNARY),
                      sizeof(value),
                      std::is_pointer<U>::value),
        value_(value),
        mask_(mask) {}
  template <typename U = T,
            typename = typename std::enable_if<std::is_pointer<U>::value>::type>
  KeyFieldValueTernary(U value, U mask, size_t size)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_TERNARY),
                      size,
                      std::is_pointer<U>::value),
        value_(value),
        mask_(mask) {}
  T value_ = 0;
  T mask_ = 0;
};

template <class T = uint64_t>
class KeyFieldValueLPM : public KeyFieldValue {
 public:
  template <
      typename U = T,
      typename = typename std::enable_if<!std::is_pointer<U>::value>::type>
  KeyFieldValueLPM(U value, uint16_t prefix_len)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_LPM),
                      sizeof(value),
                      std::is_pointer<U>::value),
        value_(value),
        prefix_len_(prefix_len) {}
  template <typename U = T,
            typename = typename std::enable_if<std::is_pointer<U>::value>::type>
  KeyFieldValueLPM(U value, uint16_t prefix_len, size_t size)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_LPM),
                      size,
                      std::is_pointer<U>::value),
        value_(value),
        prefix_len_(prefix_len) {}
  T value_ = 0;
  uint16_t prefix_len_ = 0;
};

template <class T = uint64_t>
class KeyFieldValueRange : public KeyFieldValue {
 public:
  template <
      typename U = T,
      typename = typename std::enable_if<!std::is_pointer<U>::value>::type>
  KeyFieldValueRange(U low, U high)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_RANGE),
                      sizeof(low),
                      std::is_pointer<U>::value),
        low_(low),
        high_(high) {}
  template <typename U = T,
            typename = typename std::enable_if<std::is_pointer<U>::value>::type>
  KeyFieldValueRange(U low, U high, size_t size)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_RANGE),
                      size,
                      std::is_pointer<U>::value),
        low_(low),
        high_(high) {}
  T low_ = 0;
  T high_ = 0;
};

/**
 * @brief Class to construct key for a table.<br>
 * <B>Creation: </B> Can only be created using \ref
 * tdi::Table::keyAllocate()
 */
class TableKey {
 public:
  TableKey(const Table *table) : table_(table){};
  virtual ~TableKey() = default;

  /**
   * @brief Set value. Only valid on fields of \ref tdi::KeyFieldType "EXACT"
   *
   * @param[in] field_id Field ID
   * @param[in] value   Value. Cannot be greater in value than what the field
   * size allows. For example, if the field size is 3 bits, passing in a value
   * of 14 will throw an error even though uint64_t can easily hold 14.
   *
   * @return Status of the API call
   */
  tdi_status_t setValue(const tdi_id_t &field_id,
                        const tdi::KeyFieldValue &&field_value);

  virtual tdi_status_t setValue(const tdi_id_t &field_id,
                                const tdi::KeyFieldValue &field_value);

  /**
   * @brief Get value. Only valid on fields of \ref tdi::KeyFieldType "EXACT"
   *
   * @param[in] field_id Field ID
   * @param[out] value   Value.
   *
   * @return Status of the API call
   */
  virtual tdi_status_t getValue(const tdi_id_t &field_id,
                                tdi::KeyFieldValue *value) const;

  /**
   * @brief Get the Table Object associated with this Key Object
   *
   * @param[out] table Table Object
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tableGet(const tdi::Table **table) const;

  /**
   * @brief Reset the TableKey object
   *
   * @return
   */
  virtual tdi_status_t reset();

 protected:
  const Table *table_ = nullptr;
};

}  // namespace tdi

#endif  // _TDI_TABLE_KEY_HPP
