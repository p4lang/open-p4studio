// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#include <stdio.h>
#include <tdi/common/c_frontend/tdi_init.h>
#include <tdi/common/c_frontend/tdi_info.h>

// tdi includes
#include <tdi/common/tdi_init.hpp>
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_table.hpp>
#include <tdi/common/tdi_learn.hpp>

// local includes
#include <tdi/common/tdi_utils.hpp>

tdi_status_t tdi_num_tables_get(const tdi_info_hdl *tdi, int *num_tables) {
  if (!tdi) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  std::vector<const tdi::Table *> tables;
  auto status = tdiInfo->tablesGet(&tables);
  if (status != TDI_SUCCESS) {
    return status;
  }
  *num_tables = static_cast<int>(tables.size());
  return status;
}

tdi_status_t tdi_tables_get(const tdi_info_hdl *tdi,
                             const tdi_table_hdl **tdi_table_hdl_ret) {
  if (!tdi) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  std::vector<const tdi::Table *> tables;
  auto status = tdiInfo->tablesGet(&tables);
  if (status != TDI_SUCCESS) {
    return status;
  }

  std::vector<const tdi::Table *>::iterator it;
  /*
   * Iterate through the tables and fill the handle array.
   * We use iterators rather than auto so we can use iterator
   * arithmetic to increment our array index rather than a variable.
   */
  for (it = tables.begin(); it != tables.end(); ++it) {
    const tdi::Table *table = *it;
    tdi_table_hdl_ret[it - tables.begin()] =
        reinterpret_cast<const tdi_table_hdl *>(table);
  }

  return status;
}

tdi_status_t tdi_table_from_name_get(const tdi_info_hdl *tdi,
                                      const char *table_name,
                                      const tdi_table_hdl **table_hdl_ret) {
  if (table_hdl_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!tdi || !table_name) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  const tdi::Table *table;
  auto status = tdiInfo->tableFromNameGet(table_name, &table);
  *table_hdl_ret = reinterpret_cast<const tdi_table_hdl *>(table);
  return status;
}

tdi_status_t tdi_table_from_id_get(const tdi_info_hdl *tdi,
                                    tdi_id_t id,
                                    const tdi_table_hdl **table_hdl_ret) {
  if (table_hdl_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  const tdi::Table *table;
  auto status = tdiInfo->tableFromIdGet(id, &table);
  *table_hdl_ret = reinterpret_cast<const tdi_table_hdl *>(table);
  return status;
}

tdi_status_t tdi_table_name_to_id(const tdi_info_hdl *tdi,
                                  const char *table_name,
                                  tdi_id_t *id_ret) {
  if (id_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!tdi || !table_name) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  const tdi::Table *table;
  auto status = tdiInfo->tableFromNameGet(table_name, &table);
  if (table == nullptr) {
    LOG_ERROR("%s:%d Table %s not found", __func__, __LINE__, table_name);
    return status;
  }
  auto tableInfo = table->tableInfoGet();
  *id_ret = tableInfo->idGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_num_learns_get(const tdi_info_hdl *tdi, int *num_learns) {
  if (!tdi) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  std::vector<const tdi::Learn *> learns;
  auto status = tdiInfo->learnsGet(&learns);
  if (status != TDI_SUCCESS) {
    return status;
  }
  *num_learns = static_cast<int>(learns.size());
  return status;
}

tdi_status_t tdi_learns_get(const tdi_info_hdl *tdi,
                            const tdi_learn_hdl **tdi_learn_hdl_ret) {
  if (!tdi) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  std::vector<const tdi::Learn *> learns;
  auto status = tdiInfo->learnsGet(&learns);
  if (status != TDI_SUCCESS) {
    return status;
  }

  std::vector<const tdi::Learn *>::iterator it;
  /*
   * Iterate through the learns and fill the handle array.
   * We use iterators rather than auto so we can use iterator
   * arithmetic to increment our array index rather than a variable.
   */
  for (it = learns.begin(); it != learns.end(); ++it) {
    const tdi::Learn *learn = *it;
    tdi_learn_hdl_ret[it - learns.begin()] =
        reinterpret_cast<const tdi_learn_hdl *>(learn);
  }

  return status;
}

tdi_status_t tdi_learn_from_name_get(const tdi_info_hdl *tdi,
                                     const char *learn_name,
                                     const tdi_learn_hdl **learn_hdl_ret) {
  if (learn_hdl_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!tdi || !learn_name) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  const tdi::Learn *learn;
  auto status = tdiInfo->learnFromNameGet(learn_name, &learn);
  *learn_hdl_ret = reinterpret_cast<const tdi_learn_hdl *>(learn);
  return status;
}

tdi_status_t tdi_learn_from_id_get(const tdi_info_hdl *tdi,
                                    tdi_id_t id,
                                    const tdi_learn_hdl **learn_hdl_ret) {
  if (learn_hdl_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  const tdi::Learn *learn;
  auto status = tdiInfo->learnFromIdGet(id, &learn);
  *learn_hdl_ret = reinterpret_cast<const tdi_learn_hdl *>(learn);
  return status;
}
#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_learn_name_to_id(const tdi_info_hdl *tdi,
                                   const char *learn_name,
                                   tdi_id_t *id_ret) {
  if (id_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!tdi || !learn_name) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdiInfo = reinterpret_cast<const tdi::TdiInfo *>(tdi);
  const tdi::Learn *learn;
  auto status = tdiInfo->learnNameGet(learn_name, &learn);
  if (learn == nullptr) {
    LOG_ERROR("%s:%d Learn %s not found", __func__, __LINE__, learn_name);
    return status;
  }
  return learn->learnIdGet(id_ret);
}

tdi_status_t tdi_num_tables_dependent_on_this_table_get(
    const tdi_table_hdl *tdi, int *num_tables) {
  if (!tdi || !num_tables) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdi_table = reinterpret_cast<const tdi::Table *>(tdi);
  auto tableInfo = tdi_table->tableInfoGet();
  std::vector<tdi_id_t> table_ids;
  auto table_ids =
      tableInfo->tdiInfoTablesDependentOnThisTableGet();
  *num_tables = static_cast<int>(table_ids.size());
  return TDI_SUCCESS;
}

tdi_status_t tdi_tables_dependent_on_this_table_get(
    const tdi_table_hdl *tdi,
    tdi_id_t *table_list) {
  if (!tdi || !table_list) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdi_info = reinterpret_cast<const tdi::Table *>(tdi);
  auto tableInfo = tdi_table->tableInfoGet();
  std::vector<tdi_id_t> table_ids;
  auto table_ids =
      tableInfo->tdiInfoTablesDependentOnThisTableGet();
  for (auto it = table_ids.begin(); it != table_ids.end(); it++) {
    table_list[it - table_ids.begin()] = *it;
  }
  return TDI_SUCCESS;
}


tdi_status_t tdi_info_num_pipeline_info_get(const tdi_info_hdl *tdi,
                                             int *num) {
  if (!tdi || !num) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdi_info = reinterpret_cast<const tdi::TdiInfo *>(tdi);

  tdi::PipelineProfInfoVec prof_vec;
  tdi_status_t sts = tdi_info->tdiInfoPipelineInfoGet(&prof_vec);

  *num = static_cast<int>(prof_vec.size());
  return sts;
}

tdi_status_t tdi_info_pipeline_info_get(const tdi_info_hdl *tdi,
                                         const char **prof_names,
                                         const tdi_dev_pipe_t **pipes) {
  if (!tdi || !prof_names || !pipes) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto tdi_info = reinterpret_cast<const tdi::TdiInfo *>(tdi);

  tdi::PipelineProfInfoVec prof_vec;
  tdi_status_t sts = tdi_info->tdiInfoPipelineInfoGet(&prof_vec);

  /*
   * Iterate through the names and fill the name string array.
   * We use iterators rather than auto so we can use iterator
   * arithmetic to increment our array index rather than a variable.
   * We use the reference_wrapper because we iterate through a list
   * of string references, not string values.
   */
  tdi::PipelineProfInfoVec::iterator it;
  for (it = prof_vec.begin(); it != prof_vec.end(); ++it) {
    prof_names[it - prof_vec.begin()] = it->first.get().c_str();
    pipes[it - prof_vec.begin()] = it->second.get().data();
  }
  return sts;
}
#endif
