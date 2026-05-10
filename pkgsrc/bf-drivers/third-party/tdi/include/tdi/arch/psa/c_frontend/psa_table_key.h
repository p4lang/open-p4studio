/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_table_key.hpp
 *
 *  @brief Contains TDI Table Key APIs
 */
#ifndef _PSA_TABLE_KEY_H
#define _PSA_TABLE_KEY_H

#include <tdi/common/tdi_defs.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Key Field Type. A key can have multiple fields, each with a different
 * type
 */
enum tdi_match_type_psa_e {
  TDI_MATCH_TYPE_PSA_OPTIONAL = TDI_MATCH_TYPE_ARCH,
};

/** Set */
/**
 * @brief Set value. Only valid on fields of \ref tdi_key_field_type_t
 * "OPTIONAL"
 *
 * @param[in] key_hdl     Key object handle
 * @param[in] field_id    Field ID
 * @param[in] value1      Value
 * @param[in] is_valid    Whether field is valid or not. This in-param is
 *                        not to be confused with the P4 $is_valid construct.
 *                        This param is purely to either mask out or use the
 *                        entire field with optional match type.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_set_value_optional(tdi_table_key_hdl *key_hdl,
                                              const tdi_id_t field_id,
                                              const uint64_t value1,
                                              const bool is_valid);

/**
 * @brief Set value. Only valid on fields of \ref tdi_key_field_type_t
 * "OPTIONAL"
 *
 * @param[in] key_hdl     Key object handle
 * @param[in] field_id    Field ID
 * @param[in] value1      Byte-array for value in network order.
 * @param[in] is_valid    Whether field is valid or not. This in-param is
 *                        not to be confused with the P4 $is_valid construct.
 *                        This param is purely to either mask out or use the
 *                        entire field with optional match type.
 *
 * @param[in] size        Number of bytes of the byte-array. The input number
 *                        of bytes should ceil the size of the field which is in
 *                        bits. For example, if field size is 28 bits, then size
 *                        should be 4.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_set_value_optional_ptr(tdi_table_key_hdl *key_hdl,
                                                  const tdi_id_t field_id,
                                                  const uint8_t *value1,
                                                  const bool is_valid,
                                                  const size_t size);

/** Get */
/**
 * @brief Get value. Only valid on fields of \ref tdi_key_field_type_t
 * "OPTIONAL"
 *
 * @param[in] key_hdl     Key object handle
 * @param[in] field_id    Field ID
 * @param[out] value      Value.
 * @param[out] is_valid   Whether the field is valid for matching
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_get_value_optional(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    uint64_t *value,
    bool *is_valid);

/**
 * @brief Get value. Only valid on fields of \ref tdi_key_field_type_t
 * "OPTIONAL"
 *
 * @param[in] key_hdl     Key object handle
 * @param[in] field_id    Field ID
 * @param[in] size        Size of the value byte-array
 * @param[out] value      Value byte-array. The output is in network order with
 *                        byte padded. If a 28-bit field with value 0xdedbeef is
 *                        being queried, then input size needs to be 4 and the
 *                        output array will be set to 0x0dedbeef.
 * @param[out] is_valid Whether the field is valid for matching
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_get_value_optional_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *value,
    bool *is_valid);

#ifdef __cplusplus
}
#endif

#endif  // _PSA_TABLE_KEY_H
