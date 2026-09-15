/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_info.h
 *
 *  @brief Contains TDI Info APIs. Mostly to get Table and Learn Object
 *  metadata
 */
#ifndef _TDI_INFO_H
#define _TDI_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get size of the list of TdiTable Objs
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[out] num_tables Size of array of Learn Obj pointers
 *
 * @return Status of the API call
 */
tdi_status_t tdi_num_tables_get(const tdi_info_hdl *tdi, int *num_tables);

/**
 * @brief Get Array of Table Objs from tdi_info
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[out] tdi_table_hdl_ret Array of Table Obj pointers. Memory needs
 * to be allocated by user
 *
 * @return Status of the API call
 */
tdi_status_t tdi_tables_get(const tdi_info_hdl *tdi,
                            const tdi_table_hdl **tdi_table_hdl_ret);

/**
 * @brief Get a Table Object from its fully qualified name
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[in] table_name Fully qualified Table Obj name
 * @param[out] tdi_table_hdl_ret Table Obj Pointer
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_from_name_get(
    const tdi_info_hdl *tdi,
    const char *table_name,
    const tdi_table_hdl **tdi_table_hdl_ret);

/**
 * @brief Get a Table Obj from its ID
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[in] id ID of the Table Obj
 * @param[out] tdi_table_hdl_ret Table Obj pointer
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_from_id_get(
    const tdi_info_hdl *tdi,
    tdi_id_t id,
    const tdi_table_hdl **tdi_table_hdl_ret);

/**
 * @brief Convert Table obj name to ID
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[in] table_name Fully qualified name of the Table Obj
 * @param[out] id ID of the Table Obj
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_name_to_id(const tdi_info_hdl *tdi,
                                   const char *table_name,
                                   tdi_id_t *id_ret);

/**
 * @brief Get size of the list of Learn Objs
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[out] num_learns Size of array of Learn Obj pointers
 *
 * @return Status of the API call
 */
tdi_status_t tdi_num_learns_get(const tdi_info_hdl *tdi_info,
                                 int *num_learns);

/**
 * @brief Get Array of Learn Objs from tdi_info
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[out] tdi_learn_hdl_ret Array of Learn Obj pointers. Memory needs
 * to be allocated by user
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learns_get(const tdi_info_hdl *tdi_info,
                             const tdi_learn_hdl **tdi_learn_hdl_ret);

/**
 * @brief Get a Learn Object from its fully qualified name
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[in] learn_name Fully qualified Learn Obj name
 * @param[out] tdi_learn_hdl_ret Learn Obj Pointer
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_from_name_get(
    const tdi_info_hdl *tdi_info,
    const char *learn_name,
    const tdi_learn_hdl **tdi_learn_hdl_ret);

/**
 * @brief Get a Learn Obj from its ID
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[in] id ID of the Learn Obj
 * @param[out] tdi_learn_hdl_ret Learn Obj pointer
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_from_id_get(
    const tdi_info_hdl *tdi_info,
    tdi_id_t id,
    const tdi_learn_hdl **tdi_learn_hdl_ret);

/**
 * @brief Convert Learn obj name to ID
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[in] learn_name Fully qualified name of the Learn Obj
 * @param[out] id ID of the Learn Obj
 *
 * @return Status of the API call
 */
tdi_status_t tdi_learn_name_to_id(const tdi_info_hdl *tdi_info,
                                   const char *learn_name,
                                   tdi_id_t *id);

/**
 * @brief Get size of list of tables that are dependent on a given table
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[in] tbl_id ID of the Table obj
 * @param[out] num_tables Size of list of tables that depend on the given table
 *
 * @return Status of the API call
 */
tdi_status_t tdi_num_tables_dependent_on_this_table_get(
    const tdi_table_hdl *table_hdl, int *num_tables);

/**
 * @brief Get a list of tables that are dependent on a given table. When
 *  we say that table2 is dependent on table1, we imply that a entry
 *  needs to be added in table1 before we can add a corresponding entry
 *  in table2
 *
 * @param[in] tdi_table_hdl Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[out] table_list Array of tables that depend on the given table. API
 * assumes
 * that the correct memory has been allocated by user
 *
 * @return Status of the API call
 */
tdi_status_t tdi_tables_dependent_on_this_table_get(
    const tdi_table_hdl *table_hdl,
    tdi_id_t *table_list);

/**
 * @brief Get pipeline info of a Program.
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[out] num Size of array of profile names and pipes
 *
 * @return Status of the API call
 */
tdi_status_t tdi_info_num_pipeline_info_get(const tdi_info_hdl *tdi_info,
                                             int *num);

/**
 * @brief Get pipeline info of a Program.
 *
 * @param[in] tdi_info Handle of Info object. Retrieved using
 * tdi_info_get()
 * @param[out] prof_names Array of char ptrs of pipeline profile names. API
 * assumes
 * that the correct memory has been allocated by user
 * @param[out] pipes Array of tdi_dev_pipe_t ptrs of pipe IDs. API assumes
 * that the correct memory has been allocated by user for the ptr array. The
 * pipe
 * array would already be allocated by the API
 *
 * @return Status of the API call
 */
tdi_status_t tdi_info_pipeline_info_get(const tdi_info_hdl *tdi_info,
                                         const char **prof_names,
                                         const tdi_dev_pipe_t **pipes);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_INFO_H
