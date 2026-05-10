/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_defs.h
 *
 *  @brief Contains Common data types used in TDI
 */
#ifndef _TDI_DEFS_H
#define _TDI_DEFS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 32-bit ID for all TDI info IDs
 */
typedef uint32_t tdi_id_t;

/**
 * @brief 32-bit ID for table entry handles
 */
typedef uint32_t tdi_handle_t;

/**
 * @brief 32-bit ID for Device ID
 */
typedef uint32_t tdi_dev_id_t;

/**
 * @brief enum of table APIs available
 */
enum tdi_table_api_type_e {
  /** Entry Add. Most common API. Applicable to most tables. Adds an entry
     (key,data)*/
  TDI_TABLE_API_TYPE_ADD = 0,
  /** Entry Modify. Like Add but users can change selective fields. Entry should
     be present*/
  TDI_TABLE_API_TYPE_MODIFY = 1,
  /** Entry Modify incremental. Useful in cases where the data is an
  array and only one element needs to be changed.*/
  TDI_TABLE_API_TYPE_MODIFY_INC = 2,

  /** Entry Delete. Deletes an entry. If mod-only table, then this will reset an
     entry to defaults*/
  TDI_TABLE_API_TYPE_DELETE = 3,
  /** Clear. Removes all entries. If mod-only table, then defaults all entries*/
  TDI_TABLE_API_TYPE_CLEAR = 4,
  /** Default Entry Set. Set default entry. If table has
                              const default action, then this would fail*/
  TDI_TABLE_API_TYPE_DEFAULT_ENTRY_SET = 5,
  /** Default Entry Mod. Differs from set in a way that only specified data
   * fields would be affected*/
  TDI_TABLE_API_TYPE_DEFAULT_ENTRY_MODIFY = 6,
  /** Default Entry Reset. */
  TDI_TABLE_API_TYPE_DEFAULT_ENTRY_RESET = 7,
  /** Default Entry Get. */
  TDI_TABLE_API_TYPE_DEFAULT_ENTRY_GET = 8,
  /** Entry Get. */
  TDI_TABLE_API_TYPE_GET = 9,
  /** Entry Get First. */
  TDI_TABLE_API_TYPE_GET_FIRST = 10,
  /** Entry Get Next n entries. */
  TDI_TABLE_API_TYPE_GET_NEXT_N = 11,
  /** Get Usage. Get the current usage of the tables.*/
  TDI_TABLE_API_TYPE_USAGE_GET = 12,
  /** Get Size. Get the runtime size. Only needed for tables which can have
     different runtime size than the size present in json info.*/
  TDI_TABLE_API_TYPE_SIZE_GET = 13,
  /** Get entry by handle instead of key. */
  TDI_TABLE_API_TYPE_GET_BY_HANDLE = 14,
  /** Get entry key by handle. */
  TDI_TABLE_API_TYPE_KEY_GET = 15,
  /** Get entry handle from key. */
  TDI_TABLE_API_TYPE_HANDLE_GET = 16,
  /** Invalid not supported API. */
  TDI_TABLE_API_TYPE_INVALID_API = 17
};
// The same name can be used with or without enum keyword. Advantage of being
// compatible with C++
typedef enum tdi_table_api_type_e tdi_table_api_type_e;

/**
 * @brief Enum of Data Types each Data field can take. This is NOT
 *    Data field Type
 */
enum tdi_field_data_type_e {
  TDI_FIELD_DATA_TYPE_INT_ARR = 0,
  TDI_FIELD_DATA_TYPE_BOOL_ARR = 1,
  TDI_FIELD_DATA_TYPE_UINT64 = 2,
  TDI_FIELD_DATA_TYPE_BYTE_STREAM = 3,
  TDI_FIELD_DATA_TYPE_FLOAT = 4,
  TDI_FIELD_DATA_TYPE_CONTAINER = 5,
  TDI_FIELD_DATA_TYPE_STRING = 6,
  TDI_FIELD_DATA_TYPE_BOOL = 7,
  TDI_FIELD_DATA_TYPE_STRING_ARR = 8,
  TDI_FIELD_DATA_TYPE_INT64 = 9,
  TDI_FIELD_DATA_TYPE_UNKNOWN = 10,
};
typedef enum tdi_field_data_type_e tdi_field_data_type_e;

/**
 * @brief P4 Arch types. If any new p4 arch needs to be
 * used not currently known by TDI, then use TDI_ARCH_TYPE_UNKNOWN
 */
enum tdi_arch_type_e {
  TDI_ARCH_TYPE_BEGIN = 0,
  TDI_ARCH_TYPE_PSA,
  TDI_ARCH_TYPE_PNA,
  TDI_ARCH_TYPE_TNA,
  TDI_ARCH_TYPE_UNKNOWN,
};
typedef enum tdi_arch_type_e tdi_arch_type_e;

/**
 * @brief Mgr type. Devices need to
 * define all of the types themselves
 */
typedef enum {
  TDI_MGR_TYPE_BEGIN = 0,
} tdi_mgr_type_e;

/**
 * @brief Target top level enum and reservation
 */
enum tdi_target_e {
  TDI_TARGET_CORE = 0,
  TDI_TARGET_ARCH = 0x08,
  TDI_TARGET_DEVICE = 0x80,
};
typedef enum tdi_target_e tdi_target_e;

/**
* @brief Target core top level enum and reservation
*/
enum tdi_target_core_enum_e {
  TDI_TARGET_DEV_ID = TDI_TARGET_CORE,
};
typedef enum tdi_target_core_enum_e tdi_target_core_enum_e;

/**
 * @brief Flags top level enum and reservation
 * This enum denotes the index in a 64 bit flag.
 * So the first 8(0-7) bits are reserved for core.
 * 8-15 are for arch. 16-63 are for device.
 */
enum tdi_flags_e {
  TDI_FLAGS_CORE = 0,
  TDI_FLAGS_ARCH = 0x08,
  TDI_FLAGS_DEVICE = 0x10,
  TDI_FLAGS_END = 0x40,
};
typedef enum tdi_flags_e tdi_flags_e;

/**
 * @brief Table type top level enum and reservation.
 * Taking larger reservation space than other enums due
 * to possibility of high number of table types
 */
enum tdi_table_type_e {
  TDI_TABLE_TYPE_CORE = 0x0000,
  TDI_TABLE_TYPE_ARCH = 0x0080,
  TDI_TABLE_TYPE_DEVICE = 0x0800,
};
typedef enum tdi_table_type_e tdi_table_type_e;

/**
 * @brief Match type top level enum and reservation
 */
enum tdi_match_type_e {
  TDI_MATCH_TYPE_CORE = 0,
  TDI_MATCH_TYPE_ARCH = 0x08,
  TDI_MATCH_TYPE_DEVICE = 0x80,
};
typedef enum tdi_match_type_e tdi_match_type_e;

/**
 * @brief Key Field Match Type. A key can have multiple fields,
 * each with a different match type. The below are the supported
 * Core match types
 */
enum tdi_match_type_core_e {
  TDI_MATCH_TYPE_EXACT = TDI_MATCH_TYPE_CORE,
  TDI_MATCH_TYPE_TERNARY,
  TDI_MATCH_TYPE_LPM,
  TDI_MATCH_TYPE_RANGE,
};
typedef enum tdi_match_type_core_e tdi_match_type_core_e;

/**
 * @brief Attributes top level enum and reservation
 */
enum tdi_attributes_type_e {
  TDI_ATTRIBUTES_TYPE_CORE = 0,
  TDI_ATTRIBUTES_TYPE_ARCH = 0x08,
  TDI_ATTRIBUTES_TYPE_DEVICE = 0x80,
};
typedef enum tdi_attributes_type_e tdi_attributes_type_e;

/**
 * @brief Attribute field enums. Either of
 * core, arch or target will define entire list and
 * hence doesn't need to be split.
 *
 */
enum tdi_attributes_field_type_e {
  TDI_ATTRIBUTES_FIELD_BEGIN = 0,
};
typedef enum tdi_attributes_field_type_e tdi_attributes_field_type_e;

/**
 * @brief Operations top level enum and reservation
 */
enum tdi_operations_type_e {
  TDI_OPERATIONS_TYPE_CORE = 0,
  TDI_OPERATIONS_TYPE_ARCH = 0x08,
  TDI_OPERATIONS_TYPE_DEVICE = 0x80,
};
typedef enum tdi_operations_type_e tdi_operations_type_e;

/**
 * @brief Operations field enums. Either of
 * core, arch or target will define entire list and
 * hence doesn't need to be split.
 *
 */
enum tdi_operations_field_type_e {
  TDI_OPERATIONS_FIELD_BEGIN = 0,
};
typedef enum tdi_operations_field_type_e tdi_operations_field_type_e;

/** Identifies an error code. */
typedef int tdi_status_t;

#define TDI_STATUS_VALUES                                                    \
  TDI_STATUS_(TDI_SUCCESS, "Success"),                                       \
      TDI_STATUS_(TDI_NOT_READY, "Not ready"),                               \
      TDI_STATUS_(TDI_NO_SYS_RESOURCES, "No system resources"),              \
      TDI_STATUS_(TDI_INVALID_ARG, "Invalid arguments"),                     \
      TDI_STATUS_(TDI_ALREADY_EXISTS, "Already exists"),                     \
      TDI_STATUS_(TDI_HW_COMM_FAIL, "HW access fails"),                      \
      TDI_STATUS_(TDI_OBJECT_NOT_FOUND, "Object not found"),                 \
      TDI_STATUS_(TDI_MAX_SESSIONS_EXCEEDED, "Max sessions exceeded"),       \
      TDI_STATUS_(TDI_SESSION_NOT_FOUND, "Session not found"),               \
      TDI_STATUS_(TDI_NO_SPACE, "Not enough space"),                         \
      TDI_STATUS_(TDI_EAGAIN,                                                \
                  "Resource temporarily not available, try again later"),    \
      TDI_STATUS_(TDI_INIT_ERROR, "Initialization error"),                   \
      TDI_STATUS_(TDI_TXN_NOT_SUPPORTED, "Not supported in transaction"),    \
      TDI_STATUS_(TDI_TABLE_LOCKED, "Resource held by another session"),     \
      TDI_STATUS_(TDI_IO, "IO error"),                                       \
      TDI_STATUS_(TDI_UNEXPECTED, "Unexpected error"),                       \
      TDI_STATUS_(TDI_ENTRY_REFERENCES_EXIST,                                \
                  "Action data entry is being referenced by match entries"), \
      TDI_STATUS_(TDI_NOT_SUPPORTED, "Operation not supported"),             \
      TDI_STATUS_(TDI_HW_UPDATE_FAILED, "Updating hardware failed"),         \
      TDI_STATUS_(TDI_NO_LEARN_CLIENTS, "No learning clients registered"),   \
      TDI_STATUS_(TDI_IDLE_UPDATE_IN_PROGRESS,                               \
                  "Idle time update state already in progress"),             \
      TDI_STATUS_(TDI_DEVICE_LOCKED, "Device locked"),                       \
      TDI_STATUS_(TDI_INTERNAL_ERROR, "Internal error"),                     \
      TDI_STATUS_(TDI_TABLE_NOT_FOUND, "Table not found"),                   \
      TDI_STATUS_(TDI_IN_USE, "In use"),                                     \
      TDI_STATUS_(TDI_NOT_IMPLEMENTED, "Object not implemented")
enum tdi_status_enum {
#define TDI_STATUS_(x, y) x
  TDI_STATUS_VALUES,
  TDI_STS_MAX
#undef TDI_STATUS_
};
typedef enum tdi_status_enum tdi_status_enum;

static const char *tdi_err_strings[TDI_STS_MAX + 1] = {
#define TDI_STATUS_(x, y) y
    TDI_STATUS_VALUES, "Unknown error"
#undef TDI_STATUS_
};
static inline const char *tdi_err_str(tdi_status_t sts) {
  if (TDI_STS_MAX <= sts || 0 > sts) {
    return tdi_err_strings[TDI_STS_MAX];
  } else {
    return tdi_err_strings[sts];
  }
}

/**
 * @brief Small helper macro and struct for typedef.
 * For sake of strict typechecking, instead of typedef-ing
 * \n void to our handles, we make a small struct with one element.
 */
#define DECLARE_HANDLE(name) \
  struct name##__ {          \
    int unused;              \
  };                         \
  typedef struct name##__ name

// C frontend typedefs
DECLARE_HANDLE(tdi_info_hdl);
DECLARE_HANDLE(tdi_session_hdl);
DECLARE_HANDLE(tdi_table_hdl);
DECLARE_HANDLE(tdi_table_info_hdl);
DECLARE_HANDLE(tdi_device_hdl);
DECLARE_HANDLE(tdi_target_hdl);
DECLARE_HANDLE(tdi_flags_hdl);
DECLARE_HANDLE(tdi_table_key_hdl);
DECLARE_HANDLE(tdi_table_data_hdl);
DECLARE_HANDLE(tdi_attributes_hdl);
DECLARE_HANDLE(tdi_learn_hdl);
DECLARE_HANDLE(tdi_learn_info_hdl);
DECLARE_HANDLE(tdi_learn_field_info_hdl);
DECLARE_HANDLE(tdi_operations_hdl);
DECLARE_HANDLE(tdi_dev_config_hdl);

/**
 * @brief learn_data_hdl and table_data_hdl are the same,
 *  \n hence simple typedef like TdiLearnData in
 *  \n include/tdi_learn.hpp
 */
typedef tdi_table_data_hdl tdi_learn_data_hdl;

/* C and C++ void typedefs */
DECLARE_HANDLE(tdi_learn_msg_hdl);
/** Identifies a pipe on an ASIC.  This is a 2-bit field where the bits identify
 * pipeline.
 */
typedef uint32_t tdi_dev_pipe_t;

/**
 * @brief Get error details string from an error status
 *
 * @param[in] sts Status of type @c tdi_status_t
 * @param[out] err_str Pointer to error string. Doesn't require user to allocate
 *space
 */
// void tdi_err_str(tdi_status_t sts, const char **err_str);

#ifdef __cplusplus
}
#endif

#endif
