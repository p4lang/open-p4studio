// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file psa_table_key.hpp
 *
 *  @brief Contains TDI PSA Table Key APIs
 */
#ifndef _PSA_TABLE_KEY_HPP
#define _PSA_TABLE_KEY_HPP
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_table_key.hpp>

enum tdi_psa_match_type_e {
  TDI_PSA_MATCH_TYPE_OPTIONAL = TDI_MATCH_TYPE_ARCH,
};

namespace tdi {

template<class T = uint64_t>
class KeyFieldValueOptional: public KeyFieldValue {
 public:
  template <
      typename U = T,
      typename = typename std::enable_if<!std::is_pointer<U>::value>::type>
  KeyFieldValueOptional(U value, bool is_valid)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_PSA_MATCH_TYPE_OPTIONAL),
                      sizeof(value),
                      std::is_pointer<U>::value),
        value_(value),
        is_valid_(is_valid) {}
  template <typename U = T,
            typename = typename std::enable_if<std::is_pointer<U>::value>::type>
  KeyFieldValueOptional(U value, bool is_valid, size_t size)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_PSA_MATCH_TYPE_OPTIONAL),
                      size,
                      std::is_pointer<U>::value),
        value_(value),
        is_valid_(is_valid){}
  T value_ = 0;
  bool is_valid_ = 0;
};

} // tdi
#endif  // _PSA_TABLE_KEY_HPP
