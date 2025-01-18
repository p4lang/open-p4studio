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


/*!
 * @file pipe_mgr_rmt_cfg.c
 * @date
 *
 * Implementation of Reconfigurable Match Table (RMT) configuration database.
 */

/* Standard header includes */
#include "p4studio_config.h"
#include <math.h>
#include <unistd.h>
#include <dlfcn.h>

/* Module header files */
#include <target-utils/bit_utils/bit_utils.h>
#include <pipe_mgr/pipe_mgr_err.h>
#include <pipe_mgr/pipe_mgr_config.h>
#include <pipe_mgr/pipe_mgr_porting.h>
#include <pipe_mgr/pipe_mgr_intf.h>
#include <pipe_mgr/pipe_mgr_mirror_intf.h>
#include <dvm/bf_drv_intf.h>
#include <lld/bf_dma_if.h>
#include <lld/lld_inst_list_fmt.h>
#include <lld/lld_err.h>
#include <lld/lld_sku.h>
#include <tofino_regs/tofino.h>
#include <traffic_mgr/traffic_mgr_read_apis.h>

/* Local header files */
#include "pipe_mgr_int.h"
#include "pipe_mgr_pktgen.h"
#include "pipe_mgr_mau_snapshot.h"
#include "pipe_mgr_stful_tbl_mgr.h"
#include "pipe_mgr_drv.h"
#include "pipe_mgr_phy_mem_map.h"
#include "pipe_mgr_mau_tbl_dbg_counters.h"
#include "pipe_mgr_db.h"
#include "pipe_mgr_mirror_buffer.h"
#include "pipe_mgr_hash_compute_json.h"
#include "pipe_mgr_tbl.h"
#include "pipe_mgr_idle.h"
#include "pipe_mgr_select_tbl.h"
#include "pipe_mgr_entry_format_json.h"
#include "pipe_mgr_table_packing.h"
#include "pipe_mgr_exm_tbl_init.h"
#include "pipe_mgr_p4parser.h"
#include "pipe_mgr_hitless_ha.h"
#include "pipe_mgr_dkm_json.h"
#include "pipe_mgr_hitless_ha.h"
#include "pipe_mgr_meter_tbl_init.h"
#include "pipe_mgr_interrupt.h"
#include "pipe_mgr_ctx_json.h"

/* Pointer to global pipe_mgr context */
extern pipe_mgr_ctx_t *pipe_mgr_ctx;
extern pipe_mgr_mutex_t g_pvs_mutex[PIPE_MGR_NUM_DEVICES];

bf_sys_timer_t port_stuck_detect_timers[PIPE_MGR_NUM_DEVICES];

#ifdef UTEST
#define PIPE_MGR_USE_FAKE_TABLE_CFG
#endif

static pipe_status_t get_num_active_pipes(const rmt_dev_info_t *dev_info,
                                          uint32_t *num_pipes);

/* Returns pointer to tbl_info structure. */
pipe_mat_tbl_info_t *pipe_mgr_get_tbl_info(bf_dev_id_t dev_id,
                                           pipe_mat_tbl_hdl_t h,
                                           const char *where,
                                           const int line) {
  const rmt_dev_info_t *dev_info = NULL;
  pipe_mat_tbl_info_t *tbl_info = NULL;
  unsigned i;
  uint32_t p = 0;

  if (PIPE_HDL_TYPE_MAT_TBL != PIPE_GET_HDL_TYPE(h)) {
    LOG_ERROR("Invalid MAT handle %#x at %s:%d", h, where, line);
    return NULL;
  }
  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    for (tbl_info = dev_info->profile_info[p]->tbl_info_list.mat_tbl_list,
        i = 0;
         i < dev_info->profile_info[p]->tbl_info_list.num_mat_tbls;
         ++tbl_info, ++i) {
      if (tbl_info->handle == h) {
        return tbl_info;
      }
    }
  }
  return NULL;
}

pipe_adt_tbl_info_t *pipe_mgr_get_adt_tbl_info(bf_dev_id_t dev_id,
                                               pipe_adt_tbl_hdl_t adt_tbl_hdl,
                                               const char *where,
                                               const int line) {
  const rmt_dev_info_t *dev_info = NULL;
  pipe_adt_tbl_info_t *tbl_info = NULL;
  unsigned i;
  uint32_t p = 0;

  if (PIPE_HDL_TYPE_ADT_TBL != PIPE_GET_HDL_TYPE(adt_tbl_hdl)) {
    LOG_ERROR("Invalid MAT handle %#x at %s:%d", adt_tbl_hdl, where, line);
    return NULL;
  }

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    for (tbl_info = dev_info->profile_info[p]->tbl_info_list.adt_tbl_list,
        i = 0;
         i < dev_info->profile_info[p]->tbl_info_list.num_adt_tbls;
         ++tbl_info, ++i) {
      if (tbl_info->handle == adt_tbl_hdl) {
        return tbl_info;
      }
    }
  }
  return NULL;
}

pipe_select_tbl_info_t *pipe_mgr_get_select_tbl_info(bf_dev_id_t dev_id,
                                                     pipe_sel_tbl_hdl_t h) {
  const rmt_dev_info_t *dev_info = NULL;
  pipe_select_tbl_info_t *tbl_info = NULL;
  unsigned i;
  uint32_t p = 0;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    for (tbl_info = dev_info->profile_info[p]->tbl_info_list.select_tbl_list,
        i = 0;
         i < dev_info->profile_info[p]->tbl_info_list.num_select_tbls;
         ++tbl_info, ++i) {
      if (tbl_info->handle == h) {
        return tbl_info;
      }
    }
  }
  return NULL;
}

pipe_stat_tbl_info_t *pipe_mgr_get_stat_tbl_info(
    bf_dev_id_t dev_id,
    pipe_stat_tbl_hdl_t stat_tbl_hdl,
    const char *where,
    const int line) {
  const rmt_dev_info_t *dev_info = NULL;
  pipe_stat_tbl_info_t *tbl_info = NULL;
  unsigned i;
  uint32_t p = 0;

  if (PIPE_HDL_TYPE_STAT_TBL != PIPE_GET_HDL_TYPE(stat_tbl_hdl)) {
    LOG_ERROR(
        "Invalid Stat table handle %#x at %s:%d", stat_tbl_hdl, where, line);
    return NULL;
  }

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    for (tbl_info = dev_info->profile_info[p]->tbl_info_list.stat_tbl_list,
        i = 0;
         i < dev_info->profile_info[p]->tbl_info_list.num_stat_tbls;
         ++tbl_info, ++i) {
      if (tbl_info->handle == stat_tbl_hdl) {
        return tbl_info;
      }
    }
  }
  return NULL;
}

pipe_meter_tbl_info_t *pipe_mgr_get_meter_tbl_info(
    bf_dev_id_t dev_id,
    pipe_meter_tbl_hdl_t meter_tbl_hdl,
    const char *where,
    const int line) {
  const rmt_dev_info_t *dev_info = NULL;
  pipe_meter_tbl_info_t *tbl_info = NULL;
  unsigned i;
  uint32_t p = 0;

  if (PIPE_HDL_TYPE_METER_TBL != PIPE_GET_HDL_TYPE(meter_tbl_hdl)) {
    LOG_ERROR(
        "Invalid Meter table handle %#x at %s:%d", meter_tbl_hdl, where, line);
    return NULL;
  }

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    for (tbl_info = dev_info->profile_info[p]->tbl_info_list.meter_tbl_list,
        i = 0;
         i < dev_info->profile_info[p]->tbl_info_list.num_meter_tbls;
         ++tbl_info, ++i) {
      if (tbl_info->handle == meter_tbl_hdl) {
        return tbl_info;
      }
    }
  }
  return NULL;
}

bool pipe_mgr_get_meter_impl_type(pipe_res_hdl_t tbl_hdl,
                                  bf_dev_id_t device_id,
                                  pipe_meter_impl_type_e *meter_type) {
  pipe_meter_tbl_info_t *meter_tbl_info = NULL;

  if (PIPE_GET_HDL_TYPE(tbl_hdl) != PIPE_HDL_TYPE_METER_TBL) {
    return false;
  }

  meter_tbl_info =
      pipe_mgr_get_meter_tbl_info(device_id, tbl_hdl, __func__, __LINE__);

  if (meter_tbl_info == NULL) {
    return false;
  }

  if (meter_type) {
    *meter_type = meter_tbl_info->meter_type;
  } else {
    return false;
  }

  return true;
}

pipe_stful_tbl_info_t *pipe_mgr_get_stful_tbl_info(
    bf_dev_id_t dev_id,
    pipe_stful_tbl_hdl_t stful_tbl_hdl,
    const char *where,
    const int line) {
  const rmt_dev_info_t *dev_info = NULL;
  pipe_stful_tbl_info_t *tbl_info = NULL;
  unsigned i;
  uint32_t p = 0;

  if (PIPE_HDL_TYPE_STFUL_TBL != PIPE_GET_HDL_TYPE(stful_tbl_hdl)) {
    LOG_ERROR("Invalid stateful table handle %#x at %s:%d",
              stful_tbl_hdl,
              where,
              line);
    return NULL;
  }

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    for (tbl_info = dev_info->profile_info[p]->tbl_info_list.stful_tbl_list,
        i = 0;
         i < dev_info->profile_info[p]->tbl_info_list.num_sful_tbls;
         ++tbl_info, ++i) {
      if (tbl_info->handle == stful_tbl_hdl) {
        return tbl_info;
      }
    }
  }
  return NULL;
}

/* Returns a pointer to the table name, or NULL if the handle is invalid. */
const char *pipe_mgr_get_tbl_name(bf_dev_id_t dev_id,
                                  pipe_tbl_hdl_t handle,
                                  const char *where,
                                  const int line) {
  union invar_tbl_info_ptr {
    const pipe_mat_tbl_info_t *mat;
    const pipe_adt_tbl_info_t *adt;
    const pipe_select_tbl_info_t *sel;
    const pipe_stat_tbl_info_t *stat;
    const pipe_meter_tbl_info_t *meter;
    const pipe_stful_tbl_info_t *stful;
  };
  union invar_tbl_info_ptr p = {NULL};

  switch (PIPE_GET_HDL_TYPE(handle)) {
    case PIPE_HDL_TYPE_MAT_TBL:
      p.mat = pipe_mgr_get_tbl_info(dev_id, handle, where, line);
      return (p.mat) ? p.mat->name : NULL;

    case PIPE_HDL_TYPE_ADT_TBL:
      p.adt = pipe_mgr_get_adt_tbl_info(dev_id, handle, where, line);
      return (p.adt) ? p.adt->name : NULL;

    case PIPE_HDL_TYPE_SEL_TBL:
      p.sel = pipe_mgr_get_select_tbl_info(dev_id, handle);
      return (p.sel) ? p.sel->name : NULL;

    case PIPE_HDL_TYPE_STAT_TBL:
      p.stat = pipe_mgr_get_stat_tbl_info(dev_id, handle, where, line);
      return (p.stat) ? p.stat->name : NULL;

    case PIPE_HDL_TYPE_METER_TBL:
      p.meter = pipe_mgr_get_meter_tbl_info(dev_id, handle, where, line);
      return (p.meter) ? p.meter->name : NULL;

    case PIPE_HDL_TYPE_STFUL_TBL:
      p.stful = pipe_mgr_get_stful_tbl_info(dev_id, handle, where, line);
      return (p.stful) ? p.stful->name : NULL;

    default:
      LOG_ERROR("Invalid table handle %#x at %s:%d", handle, where, line);
      return NULL;
  }
}

/* Dynamic hash calculation info get */
pipe_dhash_info_t *pipe_mgr_get_hash_calc_info(bf_dev_id_t dev_id,
                                               pipe_hash_calc_hdl_t h) {
  const rmt_dev_info_t *dev_info = NULL;
  pipe_dhash_info_t *dhash_node = NULL;
  uint32_t p = 0;
  bf_map_sts_t msts = BF_MAP_OK;
  unsigned long key = 0;

  /* Temporary disable - need compiler update to change type value
  if (PIPE_HDL_TYPE_CALC_ALGO != PIPE_GET_HDL_TYPE(h)) {
    LOG_ERROR("Invalid CALC handle %#x", h);
    return NULL;
  }
  */
  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    msts = bf_map_get_first(
        &dev_info->profile_info[p]->dhash_info, &key, (void **)&dhash_node);
    while (msts == BF_MAP_OK) {
      if (dhash_node->handle == h) {
        return dhash_node;
      }

      msts = bf_map_get_next(
          &dev_info->profile_info[p]->dhash_info, &key, (void **)&dhash_node);
    }
  }
  return NULL;
}

bool pipe_mgr_tbl_ref_is_direct(bf_dev_id_t dev_id,
                                pipe_mat_tbl_hdl_t h,
                                pipe_mat_tbl_hdl_t res,
                                const char *where,
                                const int line) {
  pipe_mat_tbl_info_t *info = pipe_mgr_get_tbl_info(dev_id, h, where, line);
  if (!info) {
    LOG_ERROR("Cannot find table handle 0x%x on dev %d from %s:%d",
              h,
              dev_id,
              where,
              line);
    PIPE_MGR_DBGCHK(info);
    return false;
  }
  unsigned int i;
  switch (PIPE_GET_HDL_TYPE(res)) {
    case PIPE_HDL_TYPE_ADT_TBL:
      for (i = 0; i < info->num_adt_tbl_refs; ++i)
        if (info->adt_tbl_ref[i].tbl_hdl == res)
          return PIPE_TBL_REF_TYPE_DIRECT == info->adt_tbl_ref[i].ref_type;
      LOG_ERROR(
          "Table handle 0x%x is not associated with table handle 0x%x on dev "
          "%d from %s:%d",
          res,
          h,
          dev_id,
          where,
          line);
      PIPE_MGR_DBGCHK(0);
      return false;
    case PIPE_HDL_TYPE_SEL_TBL:
      for (i = 0; i < info->num_sel_tbl_refs; ++i)
        if (info->sel_tbl_ref[i].tbl_hdl == res)
          return PIPE_TBL_REF_TYPE_DIRECT == info->sel_tbl_ref[i].ref_type;
      LOG_ERROR(
          "Table handle 0x%x is not associated with table handle 0x%x on dev "
          "%d from %s:%d",
          res,
          h,
          dev_id,
          where,
          line);
      PIPE_MGR_DBGCHK(0);
      return false;
    case PIPE_HDL_TYPE_STAT_TBL:
      for (i = 0; i < info->num_stat_tbl_refs; ++i)
        if (info->stat_tbl_ref[i].tbl_hdl == res)
          return PIPE_TBL_REF_TYPE_DIRECT == info->stat_tbl_ref[i].ref_type;
      LOG_ERROR(
          "Table handle 0x%x is not associated with table handle 0x%x on dev "
          "%d from %s:%d",
          res,
          h,
          dev_id,
          where,
          line);
      PIPE_MGR_DBGCHK(0);
      return false;
    case PIPE_HDL_TYPE_METER_TBL:
      for (i = 0; i < info->num_meter_tbl_refs; ++i)
        if (info->meter_tbl_ref[i].tbl_hdl == res)
          return PIPE_TBL_REF_TYPE_DIRECT == info->meter_tbl_ref[i].ref_type;
      LOG_ERROR(
          "Table handle 0x%x is not associated with table handle 0x%x on dev "
          "%d from %s:%d",
          res,
          h,
          dev_id,
          where,
          line);
      PIPE_MGR_DBGCHK(0);
      return false;
    case PIPE_HDL_TYPE_STFUL_TBL:
      for (i = 0; i < info->num_stful_tbl_refs; ++i)
        if (info->stful_tbl_ref[i].tbl_hdl == res)
          return PIPE_TBL_REF_TYPE_DIRECT == info->stful_tbl_ref[i].ref_type;
      LOG_ERROR(
          "Table handle 0x%x is not associated with table handle 0x%x on dev "
          "%d from %s:%d",
          res,
          h,
          dev_id,
          where,
          line);
      PIPE_MGR_DBGCHK(0);
      return false;
  }
  LOG_ERROR("Unknown table handle 0x%x, from %s:%d", res, where, line);
  PIPE_MGR_DBGCHK(0);
  return false;
}

bool pipe_mgr_mat_tbl_uses_only_tcam(bf_dev_id_t dev_id, pipe_mat_tbl_hdl_t h) {
  pipe_mat_tbl_info_t *t = pipe_mgr_get_tbl_info(dev_id, h, __func__, __LINE__);
  if (!t) {
    return false;
  }
  if (TERNARY_MATCH == t->match_type || LONGEST_PREFIX_MATCH == t->match_type ||
      ATCAM_MATCH == t->match_type) {
    /* Ternary, ATCAM and LPM match types are always implemented in TCAM alone.
     */
    return true;
  } else if (EXACT_MATCH == t->match_type) {
    /* Exact match may be implemented in TCAM, SRAM, or both.  Check the
     * RMT info to see which type of table implements this match type. */
    /* Only consider the match tables for this, associated tables such as
     * stats, action data, etc. are not considered. */
    uint32_t i;
    bool ret = true;
    for (i = 0; i < t->num_rmt_info && ret; ++i) {
      ret = RMT_TBL_TYPE_DIRECT_MATCH != t->rmt_info[i].type &&
            RMT_TBL_TYPE_HASH_MATCH != t->rmt_info[i].type &&
            RMT_TBL_TYPE_HASH_ACTION != t->rmt_info[i].type &&
            RMT_TBL_TYPE_PROXY_HASH != t->rmt_info[i].type;
    }
    return ret;
  } else {
    PIPE_MGR_DBGCHK(0);
  }
  return false;
}

bool pipe_mgr_mat_tbl_is_no_key(bf_dev_id_t dev_id, pipe_mat_tbl_hdl_t h) {
  pipe_mat_tbl_info_t *t = pipe_mgr_get_tbl_info(dev_id, h, __func__, __LINE__);
  if (!t) {
    return false;
  }

  if (EXACT_MATCH == t->match_type) {
    uint32_t i;
    for (i = 0; i < t->num_rmt_info; ++i) {
      if (RMT_TBL_TYPE_NO_KEY == t->rmt_info[i].type) {
        return true;
      }
    }
  }
  return false;
}

bool pipe_mgr_mat_tbl_is_phase0(bf_dev_id_t dev_id, pipe_mat_tbl_hdl_t h) {
  pipe_mat_tbl_info_t *t = pipe_mgr_get_tbl_info(dev_id, h, __func__, __LINE__);
  if (!t) {
    return false;
  }

  if (EXACT_MATCH == t->match_type) {
    uint32_t i;
    for (i = 0; i < t->num_rmt_info; ++i) {
      if (RMT_TBL_TYPE_PHASE0_MATCH == t->rmt_info[i].type) {
        return true;
      }
    }
  }
  return false;
}

bool pipe_mgr_mat_tbl_uses_tcam(bf_dev_id_t dev_id, pipe_mat_tbl_hdl_t h) {
  pipe_mat_tbl_info_t *t = pipe_mgr_get_tbl_info(dev_id, h, __func__, __LINE__);
  if (!t) {
    return false;
  }
  if (TERNARY_MATCH == t->match_type || LONGEST_PREFIX_MATCH == t->match_type ||
      ATCAM_MATCH == t->match_type) {
    /* Ternary, ATCAM and LPM match types are always implemented in TCAM alone.
     */
    return true;
  } else if (EXACT_MATCH == t->match_type) {
    /* Exact match may be implemented in TCAM, SRAM, or both.  Check the
     * RMT info to see which type of table implements this match type. */
    /* Only consider the match tables for this, associated tables such as
     * stats, action data, etc. are not considered. */
    uint32_t i;
    for (i = 0; i < t->num_rmt_info; ++i) {
      if (RMT_TBL_TYPE_TERN_MATCH == t->rmt_info[i].type) {
        return true;
      }
    }
    return false;
  }
  return false;
}

bool pipe_mgr_mat_tbl_uses_alpm(bf_dev_id_t dev_id, pipe_mat_tbl_hdl_t h) {
  pipe_mat_tbl_info_t *t = pipe_mgr_get_tbl_info(dev_id, h, __func__, __LINE__);
  if (!t) {
    return false;
  }
  return (ALPM_MATCH == t->match_type);
}

/*
 * bf-driver DB is flat and keyed off profile-id.
 * Profile DB has program and p4_pipeline hierarchy.
 * This API gets the p4-pipeline DB based on the profile index.
 */
static bf_p4_pipeline_t *pipe_mgr_get_p4_pipeline_profile_db(
    bf_device_profile_t *inp_profile, uint32_t index) {
  uint32_t count = 0, p = 0, q = 0;
  bf_p4_pipeline_t *pipeline = NULL;

  for (p = 0; p < inp_profile->num_p4_programs; p++) {
    bf_p4_program_t *prog = &(inp_profile->p4_programs[p]);
    for (q = 0; q < prog->num_p4_pipelines; q++) {
      pipeline = &(prog->p4_pipelines[q]);
      if (count == index) {
        return pipeline;
      }
      count++;
    }
  }

  return NULL;
}

/*
 * bf-driver DB is flat and keyed off profile-id.
 * Profile DB has program and p4_pipeline hierarchy.
 * This API gets the p4-program DB based on the profile index.
 */
static bf_p4_program_t *pipe_mgr_get_p4_program_profile_db(
    bf_device_profile_t *inp_profile, uint32_t index) {
  uint32_t count = 0, p = 0, q = 0;

  for (p = 0; p < inp_profile->num_p4_programs; p++) {
    bf_p4_program_t *prog = &(inp_profile->p4_programs[p]);
    for (q = 0; q < prog->num_p4_pipelines; q++) {
      if (count == index) {
        return prog;
      }
      count++;
    }
  }

  return NULL;
}

/* Blob download */
static pipe_status_t pipe_mgr_blob_download(pipe_sess_hdl_t sess_hdl,
                                            rmt_dev_info_t *dev_info) {
  pipe_status_t status = PIPE_SUCCESS;

  if (!dev_info) {
    LOG_ERROR("Cannot download pipeline config, no device info");
    return PIPE_INVALID_ARG;
  }
  bf_dev_id_t dev_id = dev_info->dev_id;

  /* For each profile in the config, download it to the requested set of pipes.
   */
  profile_id_t prof_id = -1;
  unsigned int i;
  for (i = 0; i < dev_info->num_pipeline_profiles; ++i) {
    prof_id = dev_info->profile_info[i]->profile_id;
    pipe_mgr_blob_dnld_params_t params = {0};
    params.dev_id = dev_id;
    params.prof_id = prof_id;

    snprintf(params.cfg_file,
             sizeof(params.cfg_file),
             "%s",
             dev_info->profile_info[i]->cfg_file);

    /* Only do the blob download if there is a blob */
    if (strnlen(params.cfg_file, PIPE_MGR_CFG_FILE_LEN) > 0) {
      status = pipe_mgr_download_blob_to_asic(sess_hdl, dev_info, &params);
      if (status != PIPE_SUCCESS) {
        return status;
      }
    }
  }

  return status;
}

static int vpn_compare_ascending(const void *a, const void *b) {
  const rmt_tbl_word_blk_t *blk1 = a;
  const rmt_tbl_word_blk_t *blk2 = b;

  if (blk1->vpn_id[0] > blk2->vpn_id[0]) {
    return 1;
  } else if (blk1->vpn_id[0] < blk2->vpn_id[0]) {
    return -1;
  }
  return 0;
}

static int vpn_compare_descending(const void *a, const void *b) {
  const rmt_tbl_word_blk_t *blk1 = a;
  const rmt_tbl_word_blk_t *blk2 = b;

  if (blk1->vpn_id[0] > blk2->vpn_id[0]) {
    return -1;
  } else if (blk1->vpn_id[0] < blk2->vpn_id[0]) {
    return 1;
  }
  return 0;
}

static int stage_id_compare_ascending(const void *a, const void *b) {
  const rmt_tbl_info_t *rmt1 = a;
  const rmt_tbl_info_t *rmt2 = b;

  if (rmt1->stage_id > rmt2->stage_id) {
    return 1;
  } else if (rmt1->stage_id < rmt2->stage_id) {
    return -1;
  } else {
    if (rmt1->handle > rmt2->handle) {
      return 1;
    } else if (rmt1->handle < rmt2->handle) {
      return -1;
    }
  }
  return 0;
}

static void pipe_mgr_sanitize_stage_ids(rmt_tbl_info_t *rmt_info_p,
                                        uint32_t num_rmt_info) {
  /* The pipe-mgr tables expect the stage id in certain order.
   * The context json may not be giving it in the same order. So fix it up
   */
  qsort(rmt_info_p,
        num_rmt_info,
        sizeof(rmt_tbl_info_t),
        stage_id_compare_ascending);
}

static void pipe_mgr_sanitize_mem_ids(rmt_tbl_info_t *rmt_info_p,
                                      uint32_t num_rmt_info) {
  /* The pipe-mgr tables expect the Mem-ids in certain order.
   * The context json may not be giving it in the same order. So fix it up
   */

  bool ascending;
  uint32_t bank, rmt_tbl, word_blk, v;
  rmt_tbl_info_t *rmt_info;

  if (num_rmt_info > 1) {
    pipe_mgr_sanitize_stage_ids(rmt_info_p, num_rmt_info);
  }

  for (rmt_tbl = 0; rmt_tbl < num_rmt_info; rmt_tbl++) {
    rmt_info = &rmt_info_p[rmt_tbl];
    uint32_t MAX_VPNS = (1 << TOF_EXM_SUBWORD_VPN_BITS);
    bool vpns_seen[MAX_VPNS];
    uint32_t num_vpns = 1;

    if (((rmt_info->type == RMT_TBL_TYPE_PROXY_HASH) ||
         (rmt_info->type == RMT_TBL_TYPE_HASH_MATCH)) &&
        (rmt_info->mem_type == RMT_MEM_SRAM)) {
      num_vpns = rmt_info->pack_format.entries_per_tbl_word;
    }









    PIPE_MGR_MEMSET(vpns_seen, 0, sizeof(vpns_seen));

    for (bank = 0; bank < rmt_info->num_tbl_banks; bank++) {
      rmt_tbl_bank_map_t *bank_map = &rmt_info->bank_map[bank];
      for (word_blk = 0; word_blk < bank_map->num_tbl_word_blks; word_blk++) {
        rmt_tbl_word_blk_t *tbl_word_blk = &bank_map->tbl_word_blk[word_blk];
        for (v = 0; v < num_vpns; v++) {
          vpn_id_t vpn = tbl_word_blk->vpn_id[v];
          PIPE_MGR_DBGCHK(vpn < MAX_VPNS);
          if (vpn < MAX_VPNS) {
            PIPE_MGR_DBGCHK(vpns_seen[vpn] == false);
            vpns_seen[vpn] = true;
          }
        }
      }
    }
  }

  for (rmt_tbl = 0; rmt_tbl < num_rmt_info; rmt_tbl++) {
    rmt_info = &rmt_info_p[rmt_tbl];
    ascending = true;

    /* Only doing this for TCAM and TINDs for now, since rearranging
     * might impact other tables (exm) ?
     */
    if ((rmt_info->type != RMT_TBL_TYPE_TERN_MATCH) &&
        (rmt_info->type != RMT_TBL_TYPE_TERN_INDIR)) {
      continue;
    }

    if (rmt_info->type == RMT_TBL_TYPE_TERN_MATCH) {
      ascending = false;
    }

    for (bank = 0; bank < rmt_info->num_tbl_banks; bank++) {
      rmt_tbl_bank_map_t *bank_map = &rmt_info->bank_map[bank];
      if (ascending) {
        qsort(bank_map->tbl_word_blk,
              bank_map->num_tbl_word_blks,
              sizeof(rmt_tbl_word_blk_t),
              vpn_compare_ascending);
      } else {
        qsort(bank_map->tbl_word_blk,
              bank_map->num_tbl_word_blks,
              sizeof(rmt_tbl_word_blk_t),
              vpn_compare_descending);
      }
    }
  }
}

static void pipe_mgr_sanitize_mem_ids_for_all_tbls(
    rmt_dev_tbl_info_t *parsed_tbl_info) {
  /* Fixup the mem-ids based on vpns */
  uint32_t tbl;
  for (tbl = 0; tbl < parsed_tbl_info->num_mat_tbls; tbl++) {
    pipe_mat_tbl_info_t *mat_tbl = &parsed_tbl_info->mat_tbl_list[tbl];
    pipe_mgr_sanitize_mem_ids(mat_tbl->rmt_info, mat_tbl->num_rmt_info);
  }

  for (tbl = 0; tbl < parsed_tbl_info->num_adt_tbls; tbl++) {
    pipe_adt_tbl_info_t *adt_tbl = &parsed_tbl_info->adt_tbl_list[tbl];
    pipe_mgr_sanitize_mem_ids(adt_tbl->rmt_info, adt_tbl->num_rmt_info);
  }

  for (tbl = 0; tbl < parsed_tbl_info->num_stat_tbls; tbl++) {
    pipe_stat_tbl_info_t *stat_tbl = &parsed_tbl_info->stat_tbl_list[tbl];
    pipe_mgr_sanitize_mem_ids(stat_tbl->rmt_info, stat_tbl->num_rmt_info);
  }

  for (tbl = 0; tbl < parsed_tbl_info->num_meter_tbls; tbl++) {
    pipe_meter_tbl_info_t *meter_tbl = &parsed_tbl_info->meter_tbl_list[tbl];
    pipe_mgr_sanitize_mem_ids(meter_tbl->rmt_info, meter_tbl->num_rmt_info);
  }

  for (tbl = 0; tbl < parsed_tbl_info->num_sful_tbls; tbl++) {
    pipe_stful_tbl_info_t *stful_tbl = &parsed_tbl_info->stful_tbl_list[tbl];
    pipe_mgr_sanitize_mem_ids(stful_tbl->rmt_info, stful_tbl->num_rmt_info);
  }

  for (tbl = 0; tbl < parsed_tbl_info->num_select_tbls; tbl++) {
    pipe_select_tbl_info_t *select_tbl = &parsed_tbl_info->select_tbl_list[tbl];
    pipe_mgr_sanitize_mem_ids(select_tbl->rmt_info, select_tbl->num_rmt_info);
  }
}

static pipe_status_t get_num_active_pipes(const rmt_dev_info_t *dev_info,
                                          uint32_t *num_pipes) {
  if (dev_info->virtual_device) {
    switch (dev_info->dev_type) {




      case BF_DEV_BFNT32_25Q:
      case BF_DEV_BFNT32_112_25Q:
      case BF_DEV_BFNT32_25QH:
        *num_pipes = 8;
        return PIPE_SUCCESS;
      case BF_DEV_BFNT10064Q:
      case BF_DEV_BFNT10032Q:
      case BF_DEV_BFNT20128Q:
      case BF_DEV_BFNT20128QM:
      case BF_DEV_BFNT20064Q:
      case BF_DEV_BFNT31_12Q:
      case BF_DEV_BFNT31_112_12Q:
      case BF_DEV_BFNT31_12QH:
        *num_pipes = 4;
        return PIPE_SUCCESS;
      case BF_DEV_BFNT20080T:
      case BF_DEV_BFNT20080TM:
        *num_pipes = 3;
        return PIPE_SUCCESS;
      case BF_DEV_BFNT10032D:
      case BF_DEV_BFNT10032D018:
      case BF_DEV_BFNT10032D020:
      case BF_DEV_BFNT20064D:
        *num_pipes = 2;
        return PIPE_SUCCESS;
      default:
        *num_pipes = 0;
        return PIPE_INVALID_ARG;
    }
  } else {
    if (LLD_OK != lld_sku_get_num_active_pipes(dev_info->dev_id, num_pipes))
      return PIPE_INVALID_ARG;
  }
  return PIPE_SUCCESS;
}

static pipe_status_t get_num_active_subdevices(rmt_dev_info_t *dev_info,
                                               uint32_t *num_subdevices) {
  if (dev_info->virtual_device) {
    switch (dev_info->dev_type) {
      /* Tofino 1 & 2 */
      case BF_DEV_BFNT10064Q:
      case BF_DEV_BFNT10032Q:
      case BF_DEV_BFNT20128Q:
      case BF_DEV_BFNT20128QM:
      case BF_DEV_BFNT20064Q:
      case BF_DEV_BFNT20080T:
      case BF_DEV_BFNT20080TM:
      case BF_DEV_BFNT10032D:
      case BF_DEV_BFNT10032D020:
      case BF_DEV_BFNT10032D018:
      case BF_DEV_BFNT20064D:
      /* Tofino 3 */
      case BF_DEV_BFNT31_12Q:
      case BF_DEV_BFNT31_112_12Q:
      case BF_DEV_BFNT31_12QH:
        *num_subdevices = 1;
        return PIPE_SUCCESS;
      case BF_DEV_BFNT32_25Q:
      case BF_DEV_BFNT32_112_25Q:
      case BF_DEV_BFNT32_25QH:
        *num_subdevices = 2;
        return PIPE_SUCCESS;








      default:
        *num_subdevices = 1;
        return PIPE_INVALID_ARG;
    }
  } else {
    uint32_t subdev_mask = 0;
    if (LLD_OK !=
        lld_sku_get_num_subdev(dev_info->dev_id, num_subdevices, &subdev_mask))
      return PIPE_INVALID_ARG;
  }
  return PIPE_SUCCESS;
}

static pipe_status_t get_num_active_mau(const rmt_dev_info_t *dev_info,
                                        uint8_t *num_active_mau) {
  bf_dev_pipe_t phy_pipe = 0;
  uint32_t mau_tmp;
  if (dev_info->virtual_device) {
    *num_active_mau = dev_info->dev_cfg.num_stages;
  } else {
    lld_sku_map_pipe_id_to_phy_pipe_id(dev_info->dev_id, 0, &phy_pipe);
    lld_sku_get_num_active_mau_stages(dev_info->dev_id, &mau_tmp, phy_pipe);
    *num_active_mau = mau_tmp;
  }
  return PIPE_SUCCESS;
}

static pipe_status_t pipe_id_to_phy_pipe_id(const rmt_dev_info_t *dev_info,
                                            bf_dev_pipe_t lid,
                                            bf_dev_pipe_t *pid) {
  if (dev_info->virtual_device) {
    *pid = lid;
  } else {
    bf_dev_pipe_t x = lid;
    bf_dev_pipe_t y = 0;
    if (LLD_OK == lld_sku_map_pipe_id_to_phy_pipe_id(dev_info->dev_id, x, &y)) {
      *pid = y;
    } else {
      return PIPE_INVALID_ARG;
    }
  }
  return PIPE_SUCCESS;
}

/* Get the lowest pipe of a pipeline */
static bf_dev_pipe_t pipe_mgr_get_lowest_pipe_of_pipeline(
    bf_p4_pipeline_t *p4_pipeline) {
  int i = 0;
  bf_dev_pipe_t lowest_pipe = 0xffff;
  if (p4_pipeline && (p4_pipeline->num_pipes_in_scope != 0)) {
    for (i = 0; i < p4_pipeline->num_pipes_in_scope; i++) {
      if (lowest_pipe > (uint32_t)p4_pipeline->pipe_scope[i]) {
        lowest_pipe = p4_pipeline->pipe_scope[i];
      }
    }
  }
  /* Make sure we found at least one pipe for this P4 and pipeline */
  PIPE_MGR_DBGCHK(lowest_pipe != 0xffff);
  return lowest_pipe;
}

/* This function parses context JSON describing the tables in a
 * given device; When PIPE_MGR_USE_FAKE_TABLE_CFG is defined it loads a few
 * hardcoded tables for the purposes of unit-testing
 */
static pipe_status_t pipe_mgr_rmt_import(bf_dev_id_t dev_id,
                                         rmt_dev_info_t *dev_info,
                                         bf_device_profile_t *inp_profile,
                                         bool virtual_device) {
  uint32_t p = 0;
  profile_id_t profile_id = 0;

  LOG_TRACE("%s: Starting pipe_mgr_rmt_import", __func__);

  if (dev_info == NULL || dev_info->dev_id != dev_id) {
    PIPE_MGR_DBGCHK(0);
    return PIPE_INVALID_ARG;
  }

  /* Process the context for all profiles */
  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    uint32_t ii = 0;
    bf_p4_program_t *p4_program = NULL;
    bf_p4_pipeline_t *p4_pipeline = NULL;
    profile_id = dev_info->profile_info[p]->profile_id;

    if (inp_profile) {
      p4_program = pipe_mgr_get_p4_program_profile_db(inp_profile, p);
      if (!p4_program) {
        LOG_ERROR("P4-program does not exist for profile index %d, dev %d",
                  p,
                  dev_id);
        return PIPE_INVALID_ARG;
      }
      p4_pipeline = pipe_mgr_get_p4_pipeline_profile_db(inp_profile, p);
      if (!p4_pipeline) {
        LOG_ERROR("P4-pipeline does not exist for profile index %d, dev %d",
                  p,
                  dev_id);
        return PIPE_INVALID_ARG;
      }
    } else {
      p4_program = NULL;
      p4_pipeline = NULL;
    }

    if (p4_program) {
      /* Check if profile uses bfrt */
      dev_info->profile_info[profile_id]->uses_bfrt =
          (p4_program->bfrt_json_file) ? true : false;
      strncpy(dev_info->profile_info[profile_id]->prog_name,
              p4_program->prog_name,
              PIPE_MGR_PROG_NAME_LEN - 1);
    }

    if (p4_pipeline) {
      /* Copy the pipeline program name */
      snprintf(dev_info->profile_info[profile_id]->pipeline_name,
               sizeof(dev_info->profile_info[profile_id]->pipeline_name),
               "%s",
               p4_pipeline->p4_pipeline_name);

      /* Copy the cfg file location */
      snprintf(dev_info->profile_info[profile_id]->cfg_file,
               sizeof(dev_info->profile_info[profile_id]->cfg_file),
               "%s",
               p4_pipeline->cfg_file);
    }

    /* Set all pipes for now. The json file should give us this info. */
    PIPE_BITMAP_INIT(&(dev_info->profile_info[profile_id]->pipe_bmp),
                     PIPE_BMP_SIZE);
    if (p4_pipeline && (p4_pipeline->num_pipes_in_scope != 0)) {
      for (ii = 0; (int)ii < p4_pipeline->num_pipes_in_scope; ii++) {
        PIPE_BITMAP_SET(&dev_info->profile_info[profile_id]->pipe_bmp,
                        p4_pipeline->pipe_scope[ii]);
      }
      dev_info->profile_info[profile_id]->lowest_pipe =
          pipe_mgr_get_lowest_pipe_of_pipeline(p4_pipeline);
    } else {
      for (ii = 0; ii < dev_info->num_active_pipes; ii++) {
        PIPE_BITMAP_SET(&dev_info->profile_info[profile_id]->pipe_bmp, ii);
      }
      dev_info->profile_info[profile_id]->lowest_pipe = 0;
    }

#ifndef PIPE_MGR_USE_FAKE_TABLE_CFG
    if (p4_pipeline) {
      const char *pathToJsonDataFile = p4_pipeline->runtime_context_file;
      LOG_TRACE("%s: json cfg file: device %u, file %s, profile id %d ",
                __func__,
                dev_id,
                pathToJsonDataFile,
                profile_id);

      rmt_dev_tbl_info_t *parsed_tbl_info = NULL;
      parsed_tbl_info = pipe_mgr_ctx_json_parse(
          dev_info, profile_id, pathToJsonDataFile, virtual_device);

      if (parsed_tbl_info == NULL) {
        LOG_ERROR("%s : Error in parsing the context jSON file", __func__);
        return PIPE_NO_SYS_RESOURCES;
      }

      pipe_mgr_sanitize_mem_ids_for_all_tbls(parsed_tbl_info);

      dev_info->profile_info[profile_id]->tbl_info_list = *parsed_tbl_info;
      PIPE_MGR_FREE(parsed_tbl_info);
    }
#else
    extern void pipe_mgr_setup_fake_profile_func_ptrs(
        profile_id_t profile_id, rmt_dev_info_t * dev_info);

    pipe_mgr_setup_fake_profile_func_ptrs(profile_id, dev_info);
#ifdef PIPE_MGR_ADT_MGR_UTEST
#define __MAX_ADT_ENTRIES_IN_STAGE 8192
#define __MIN_ADT_ENTRIES_IN_STAGE 1024
    rmt_dev_tbl_info_t *tbl_info =
        &dev_info->profile_info[profile_id]->tbl_info_list;
    tbl_info->num_adt_tbls = 64;
    tbl_info->adt_tbl_list =
        PIPE_MGR_CALLOC(tbl_info->num_adt_tbls, sizeof(pipe_adt_tbl_info_t));
    unsigned i;
    for (i = 0; i < tbl_info->num_adt_tbls; ++i) {
      unsigned num_entries = 0;
      pipe_adt_tbl_info_t *adt = &tbl_info->adt_tbl_list[i];
      char tblName[16] = {0};
      sprintf(tblName, "UtestActionTbl_%d", i);
      adt->name = bf_sys_strdup(tblName);
      adt->handle = PIPE_SET_HDL_TYPE(i, PIPE_HDL_TYPE_ADT_TBL);
      /* Action data width can vary from 8-bits to 1024 bits */
      uint32_t act_data_width = pow(2, i % 9);
      num_entries = __MIN_ADT_ENTRIES_IN_STAGE * (i + 1);
      if (num_entries < __MAX_ADT_ENTRIES_IN_STAGE) {
        adt->num_stages = 1;
      } else {
        adt->num_stages = num_entries / __MAX_ADT_ENTRIES_IN_STAGE;
        if (num_entries % __MAX_ADT_ENTRIES_IN_STAGE) {
          adt->num_stages++;
        }
      }
      adt->size = num_entries;
      adt->rmt_info = PIPE_MGR_CALLOC(adt->num_stages, sizeof(rmt_tbl_info_t));
      unsigned j;
      unsigned remaining_entries = adt->size;
      for (j = 0; j < adt->num_stages; ++j) {
        adt->rmt_info[j].stage_id = j;
        adt->rmt_info[j].num_entries =
            min(__MIN_ADT_ENTRIES_IN_STAGE, remaining_entries);
        remaining_entries -= adt->rmt_info[j].num_entries;
        if (act_data_width <= TOF_SRAM_UNIT_WIDTH / 8) {
          adt->rmt_info[j].pack_format.mem_units_per_tbl_word = 1;
        } else {
          adt->rmt_info[j].pack_format.mem_units_per_tbl_word =
              act_data_width / (TOF_SRAM_UNIT_WIDTH / 8);
        }

        /* With action data, the packing format will be anything apart from 1
         * when the width of a single entry < SRAM unit width.
         */
        if (act_data_width >= TOF_SRAM_UNIT_WIDTH / 8) {
          adt->rmt_info[j].pack_format.entries_per_tbl_word = 1;
        } else {
          adt->rmt_info[j].pack_format.entries_per_tbl_word =
              (TOF_SRAM_UNIT_WIDTH / 8) / (act_data_width);
        }
        /* For action data table, number of tbl bank is always 1 */
        adt->rmt_info[j].num_tbl_banks = 1;
        adt->rmt_info[j].bank_map =
            PIPE_MGR_CALLOC(1, sizeof(rmt_tbl_bank_map_t));
        rmt_tbl_bank_map_t *bm = (adt->rmt_info[j].bank_map);
        unsigned k;
        unsigned curr_ram_id = 0;

        bm->tbl_word_blk =
            PIPE_MGR_CALLOC(bm->num_tbl_word_blks, sizeof(rmt_tbl_word_blk_t));
        for (k = 0; k < bm->num_tbl_word_blks; k++) {
          unsigned l;
          for (l = 0; l < adt->rmt_info[j].pack_format.mem_units_per_tbl_word;
               ++l) {
            bm->tbl_word_blk[k].mem_id[l] = curr_ram_id++;
            bm->tbl_word_blk[k].vpn_id[l] = (i % 2 == 0) ? 0xAB : 0xCD;
          }
        }
      }
    }

#else

    char *pathToJsonDataFile;
    pathToJsonDataFile = PIPE_MGR_UTEST_MAU_CTX_JSON;

    rmt_dev_tbl_info_t *parsed_tbl_info =
        pipe_mgr_ctx_json_parse(dev_id, 0, pathToJsonDataFile, virtual_device);
    if (parsed_tbl_info == NULL) {
      pathToJsonDataFile = "./pipe_mgr_utest_mau_ctx.json";
      parsed_tbl_info = pipe_mgr_ctx_json_parse(
          dev_id, 0, pathToJsonDataFile, virtual_device);

      if (parsed_tbl_info == NULL) {
        LOG_ERROR("%s : Error in parsing the context jSON file", __func__);
        return PIPE_NO_SYS_RESOURCES;
      }
    }

    pipe_mgr_sanitize_mem_ids_for_all_tbls(parsed_tbl_info);

    dev_info->profile_info[profile_id]->tbl_info_list = *parsed_tbl_info;
    PIPE_MGR_FREE(parsed_tbl_info);

#endif  // PIPE_MGR_ADT_MGR_UTEST
#endif  // PIPE_MGR_USE_FAKE_TABLE_CFG

  }  // for num_pipeline_profiles

  if (inp_profile) {
    // coalescing mirror configuration
    dev_info->coal_mirror_enable = inp_profile->coal_mirror_enable;
    dev_info->coal_sessions_num = inp_profile->coal_sessions_num;
    dev_info->coal_min = inp_profile->coal_min;
  }

  PIPE_MGR_LOCK_INIT(g_pvs_mutex[dev_id]);
  return PIPE_SUCCESS;
}

/* Free static entry info */
static void pipe_mgr_rmt_static_entries_free(pipe_mat_tbl_info_t *mat_tbl) {
  unsigned j = 0, k = 0;
  pipe_mgr_static_entry_info_t *ent = NULL;

  if (!mat_tbl) {
    return;
  }

  if (!mat_tbl->static_entries) {
    return;
  }

  for (j = 0; j < mat_tbl->num_static_entries; j++) {
    ent = &mat_tbl->static_entries[j];
    /* Free up match spec */
    if (ent->key) {
      PIPE_MGR_FREE(ent->key);
      ent->key = NULL;
    }
    if (ent->msk) {
      PIPE_MGR_FREE(ent->msk);
      ent->msk = NULL;
    }

    /* Free up action spec */
    if (ent->action_entry.name) {
      PIPE_MGR_FREE(ent->action_entry.name);
    }
    if (ent->action_entry.act_data) {
      for (k = 0; k < ent->action_entry.num_act_data; k++) {
        if (ent->action_entry.act_data[k].name) {
          PIPE_MGR_FREE(ent->action_entry.act_data[k].name);
        }
        if (ent->action_entry.act_data[k].value) {
          PIPE_MGR_FREE(ent->action_entry.act_data[k].value);
        }
      }
      PIPE_MGR_FREE(ent->action_entry.act_data);
      ent->action_entry.act_data = NULL;
    }
  }
  PIPE_MGR_FREE(mat_tbl->static_entries);
  mat_tbl->static_entries = NULL;
}

/* Free hash-action entry info */
static void pipe_mgr_rmt_hash_action_entries_free(
    pipe_mat_tbl_info_t *mat_tbl) {
  unsigned k = 0;
  hash_action_tbl_info_t *ent = NULL;

  if (!mat_tbl) {
    return;
  }

  if (!mat_tbl->hash_action_info) {
    return;
  }
  ent = mat_tbl->hash_action_info;

  /* Free up match spec */
  if (ent->match_param_list) {
    for (k = 0; k < ent->num_match_params; k++) {
      if (ent->match_param_list[k].name) {
        PIPE_MGR_FREE(ent->match_param_list[k].name);
      }
    }
    PIPE_MGR_FREE(ent->match_param_list);
    ent->match_param_list = NULL;
  }

  PIPE_MGR_FREE(mat_tbl->hash_action_info);
  mat_tbl->hash_action_info = NULL;
}

static void pipe_mgr_rmt_default_free(pipe_mat_tbl_info_t *mat_tbl) {
  if (!mat_tbl || !mat_tbl->default_info) {
    return;
  }
  pipe_mgr_action_entry_t *ent = &mat_tbl->default_info->action_entry;

  if (ent->name) {
    PIPE_MGR_FREE(ent->name);
    ent->name = NULL;
  }
  if (ent->act_data) {
    for (unsigned k = 0; k < ent->num_act_data; k++) {
      if (ent->act_data[k].name) {
        PIPE_MGR_FREE(ent->act_data[k].name);
        ent->act_data[k].name = NULL;
      }
      if (ent->act_data[k].value) {
        PIPE_MGR_FREE(ent->act_data[k].value);
        ent->act_data[k].value = NULL;
      }
    }
    PIPE_MGR_FREE(ent->act_data);
    ent->act_data = NULL;
  }
  if (ent->ind_res) {
    PIPE_MGR_FREE(ent->ind_res);
    ent->ind_res = NULL;
  }
  if (ent->dir_res) {
    PIPE_MGR_FREE(ent->dir_res);
    ent->dir_res = NULL;
  }

  PIPE_MGR_FREE(mat_tbl->default_info);
  mat_tbl->default_info = NULL;
}

static void pipe_mgr_rmt_dyn_sel_field_lists_free(
    pipe_dhash_info_t *dhash_node) {
  uint32_t i = 0, j = 0, k = 0;
  pipe_dhash_field_list_t *field_list;

  if (!dhash_node->field_lists) {
    return;
  }

  for (i = 0; i < dhash_node->num_field_lists; i++) {
    field_list = &dhash_node->field_lists[i];
    if (field_list->fields) {
      for (j = 0; j < field_list->num_fields; j++) {
        if (field_list->fields[j].name) {
          PIPE_MGR_FREE(field_list->fields[j].name);
        }
      }
      PIPE_MGR_FREE(field_list->fields);
    }

    if (field_list->crossbar_configs) {
      for (j = 0; j < field_list->num_crossbar_configs; j++) {
        if (field_list->crossbar_configs[j].crossbars) {
          for (k = 0; k < field_list->crossbar_configs[i].num_crossbars; k++) {
            if (field_list->crossbar_configs[j].crossbars[k].name) {
              PIPE_MGR_FREE(field_list->crossbar_configs[j].crossbars[k].name);
            }
          }
          PIPE_MGR_FREE(field_list->crossbar_configs[j].crossbars);
        }
        if (field_list->crossbar_configs[j].crossbar_mods) {
          for (k = 0; k < field_list->crossbar_configs[i].num_crossbar_mods;
               k++) {
            if (field_list->crossbar_configs[j].crossbar_mods[k].name) {
              PIPE_MGR_FREE(
                  field_list->crossbar_configs[j].crossbar_mods[k].name);
            }
          }
          PIPE_MGR_FREE(field_list->crossbar_configs[j].crossbar_mods);
        }
        if (field_list->crossbar_configs[j].ixbar_init.ixbar_inputs) {
          PIPE_MGR_FREE(
              field_list->crossbar_configs[j].ixbar_init.ixbar_inputs);
        }
        if (field_list->crossbar_configs[j].ixbar_mod_init.ixbar_inputs) {
          PIPE_MGR_FREE(
              field_list->crossbar_configs[j].ixbar_mod_init.ixbar_inputs);
        }
      }
      PIPE_MGR_FREE(field_list->crossbar_configs);
    }

    if (field_list->name) {
      PIPE_MGR_FREE(field_list->name);
    }
  }

  PIPE_MGR_FREE(dhash_node->field_lists);
  dhash_node->field_lists = NULL;
  return;
}

static void pipe_mgr_rmt_dyn_sel_alg_free(pipe_dhash_info_t *dhash_node) {
  uint32_t i = 0, j = 0;
  if (!dhash_node->algorithms) {
    return;
  }

  for (i = 0; i < dhash_node->num_algorithms; i++) {
    if (dhash_node->algorithms[i].name) {
      PIPE_MGR_FREE(dhash_node->algorithms[i].name);
    }
    if (dhash_node->algorithms[i].hash_alg.crc_matrix) {
      for (j = 0; j < 256; j++) {
        if (dhash_node->algorithms[i].hash_alg.crc_matrix[j]) {
          PIPE_MGR_FREE(dhash_node->algorithms[i].hash_alg.crc_matrix[j]);
        }
      }
      PIPE_MGR_FREE(dhash_node->algorithms[i].hash_alg.crc_matrix);
    }
  }

  PIPE_MGR_FREE(dhash_node->algorithms);
  dhash_node->algorithms = NULL;
  return;
}

static void pipe_mgr_rmt_dyn_sel_hash_configs_free(
    pipe_dhash_info_t *dhash_node) {
  uint32_t i = 0;
  if (!dhash_node->hash_configs) {
    return;
  }

  for (i = 0; i < dhash_node->num_hash_configs; i++) {
    if (dhash_node->hash_configs[i].hash.hash_bits) {
      PIPE_MGR_FREE(dhash_node->hash_configs[i].hash.hash_bits);
    }
    if (dhash_node->hash_configs[i].hash_mod.hash_bits) {
      PIPE_MGR_FREE(dhash_node->hash_configs[i].hash_mod.hash_bits);
    }
  }

  PIPE_MGR_FREE(dhash_node->hash_configs);
  dhash_node->hash_configs = NULL;
  return;
}

void pipe_mgr_rmt_dynamic_selector_free(bf_map_t *dhash_info) {
  bf_map_sts_t msts = BF_MAP_OK;
  pipe_dhash_info_t *dhash_node = NULL;
  unsigned long key = 0;

  if (!dhash_info) {
    return;
  }
  while (msts == BF_MAP_OK) {
    msts = bf_map_get_first_rmv(dhash_info, &key, (void **)&dhash_node);
    if ((msts == BF_MAP_OK) && dhash_node) {
      /* Cleanup field lists */
      if (dhash_node->field_lists) {
        pipe_mgr_rmt_dyn_sel_field_lists_free(dhash_node);
      }
      if (dhash_node->algorithms) {
        pipe_mgr_rmt_dyn_sel_alg_free(dhash_node);
      }
      if (dhash_node->hash_configs) {
        pipe_mgr_rmt_dyn_sel_hash_configs_free(dhash_node);
      }
      if (dhash_node->name) {
        PIPE_MGR_FREE(dhash_node->name);
      }
      if (dhash_node->curr_field_attrs) {
        PIPE_MGR_FREE(dhash_node->curr_field_attrs);
      }
      PIPE_MGR_FREE(dhash_node);
    }
  }
  bf_map_destroy(dhash_info);
}

/* Free all the allocations made in pipe_mgr_rmt_import. */
static void pipe_mgr_rmt_import_free(bf_dev_id_t dev_id,
                                     rmt_dev_info_t *dev_info) {
  uint32_t p = 0;
  rmt_dev_tbl_info_t *tbl_info = NULL;

  if (dev_info == NULL || dev_info->dev_id != dev_id) {
    PIPE_MGR_DBGCHK(0);
    return;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    tbl_info = &dev_info->profile_info[p]->tbl_info_list;

    unsigned i = 0, j = 0, k = 0;
    for (i = 0; i < tbl_info->num_mat_tbls; ++i) {
      pipe_mat_tbl_info_t *mat_tbl = &tbl_info->mat_tbl_list[i];

      /* Free shared resources once between alpm and its lower-level atcam */
      if (mat_tbl->match_type != ALPM_MATCH) {
        for (j = 0; j < mat_tbl->num_rmt_info; ++j) {
          for (k = 0; k < mat_tbl->rmt_info[j].num_tbl_banks; ++k) {
            PIPE_MGR_FREE(mat_tbl->rmt_info[j].bank_map[k].tbl_word_blk);
          }
          PIPE_MGR_FREE(mat_tbl->rmt_info[j].bank_map);

          if (mat_tbl->rmt_info[j].stash) {
            for (k = 0; k < mat_tbl->rmt_info[j].stash->num_stash_entries;
                 ++k) {
              PIPE_MGR_FREE(mat_tbl->rmt_info[j].stash->stash_entries[k]);
            }
            if (mat_tbl->rmt_info[j].stash->stash_entries) {
              PIPE_MGR_FREE(mat_tbl->rmt_info[j].stash->stash_entries);
            }
            PIPE_MGR_FREE(mat_tbl->rmt_info[j].stash);
          }
        }
        PIPE_MGR_FREE(mat_tbl->rmt_info);
        PIPE_MGR_FREE(mat_tbl->adt_tbl_ref);
        PIPE_MGR_FREE(mat_tbl->sel_tbl_ref);
        PIPE_MGR_FREE(mat_tbl->stat_tbl_ref);
        PIPE_MGR_FREE(mat_tbl->meter_tbl_ref);
        PIPE_MGR_FREE(mat_tbl->stful_tbl_ref);
      }
      /* Free up memory for static entries */
      if (mat_tbl->static_entries) {
        pipe_mgr_rmt_static_entries_free(mat_tbl);
      }
      /* Free up Partition_idx_info */
      if (mat_tbl->partition_idx_info) {
        PIPE_MGR_FREE(mat_tbl->partition_idx_info->partition_field_name);
        PIPE_MGR_FREE(mat_tbl->partition_idx_info);
      }
      /* Free up hash action memory */
      if (mat_tbl->hash_action_info) {
        pipe_mgr_rmt_hash_action_entries_free(mat_tbl);
      }
      /* Free up DKM mask */
      if (mat_tbl->match_key_mask) {
        PIPE_MGR_FREE(mat_tbl->match_key_mask);
        mat_tbl->match_key_mask = NULL;
      }
      /* Free up keyless table memory */
      if (mat_tbl->keyless_info) {
        PIPE_MGR_FREE(mat_tbl->keyless_info);
        mat_tbl->keyless_info = NULL;
      }
      /* Free up init default action memory */
      if (mat_tbl->default_info) {
        pipe_mgr_rmt_default_free(mat_tbl);
      }
      /* Free up default action blacklist. */
      if (mat_tbl->def_act_blacklist) {
        PIPE_MGR_FREE(mat_tbl->def_act_blacklist);
      }

      if (mat_tbl->alpm_info && mat_tbl->match_type != ATCAM_MATCH) {
        if (mat_tbl->alpm_info->field_info) {
          for (j = 0; j < mat_tbl->alpm_info->num_fields; j++) {
            if (mat_tbl->alpm_info->field_info[j].slice_offset) {
              PIPE_MGR_FREE(mat_tbl->alpm_info->field_info[j].slice_offset);
            }
            if (mat_tbl->alpm_info->field_info[j].slice_width) {
              PIPE_MGR_FREE(mat_tbl->alpm_info->field_info[j].slice_width);
            }
          }
          PIPE_MGR_FREE(mat_tbl->alpm_info->field_info);
        }
        if (mat_tbl->alpm_info->act_fn_hdls) {
          PIPE_MGR_FREE(mat_tbl->alpm_info->act_fn_hdls);
        }
        PIPE_MGR_FREE(mat_tbl->alpm_info);
      }

      if (mat_tbl->tbl_global_key_mask_bits) {
        mat_tbl->tbl_global_key_mask_valid = false;
        PIPE_MGR_FREE(mat_tbl->tbl_global_key_mask_bits);
        mat_tbl->tbl_global_key_mask_bits = NULL;
      }
      PIPE_MGR_FREE((char *)mat_tbl->name);
      /* Clean up the hash tables maintained keyed by match-spec in order
       * for match-spec based access.
       */
      pipe_mgr_match_spec_htbl_cleanup(dev_id, mat_tbl);
      if (mat_tbl->act_fn_hdl_info) {
        PIPE_MGR_FREE(mat_tbl->act_fn_hdl_info);
        mat_tbl->act_fn_hdl_info = NULL;
      }
    }
    for (i = 0; i < tbl_info->num_adt_tbls; ++i) {
      pipe_adt_tbl_info_t *tbl = &tbl_info->adt_tbl_list[i];
      for (j = 0; j < tbl->num_rmt_info; ++j) {
        for (k = 0; k < tbl->rmt_info[j].num_tbl_banks; ++k) {
          PIPE_MGR_FREE(tbl->rmt_info[j].bank_map[k].tbl_word_blk);
        }
        PIPE_MGR_FREE(tbl->rmt_info[j].bank_map);
      }
      PIPE_MGR_FREE(tbl->rmt_info);
      PIPE_MGR_FREE((char *)tbl->name);
      if (tbl->act_fn_hdl_info) {
        PIPE_MGR_FREE(tbl->act_fn_hdl_info);
        tbl->act_fn_hdl_info = NULL;
      }
    }
    for (i = 0; i < tbl_info->num_stat_tbls; ++i) {
      pipe_stat_tbl_info_t *tbl = &tbl_info->stat_tbl_list[i];
      for (j = 0; j < tbl->num_rmt_info; ++j) {
        for (k = 0; k < tbl->rmt_info[j].num_tbl_banks; ++k) {
          PIPE_MGR_FREE(tbl->rmt_info[j].bank_map[k].tbl_word_blk);
        }
        PIPE_MGR_FREE(tbl->rmt_info[j].bank_map);
      }
      PIPE_MGR_FREE(tbl->rmt_info);
      PIPE_MGR_FREE((char *)tbl->name);
    }
    for (i = 0; i < tbl_info->num_meter_tbls; ++i) {
      pipe_meter_tbl_info_t *tbl = &tbl_info->meter_tbl_list[i];
      for (j = 0; j < tbl->num_rmt_info; ++j) {
        for (k = 0; k < tbl->rmt_info[j].num_tbl_banks; ++k) {
          PIPE_MGR_FREE(tbl->rmt_info[j].bank_map[k].tbl_word_blk);
          if (tbl->rmt_info[j].color_bank_map) {
            if (tbl->rmt_info[j].color_bank_map[k].tbl_word_blk) {
              PIPE_MGR_FREE(tbl->rmt_info[j].color_bank_map[k].tbl_word_blk);
            }
          }
        }
        PIPE_MGR_FREE(tbl->rmt_info[j].bank_map);
        if (tbl->rmt_info[j].color_bank_map)
          PIPE_MGR_FREE(tbl->rmt_info[j].color_bank_map);
      }
      PIPE_MGR_FREE(tbl->rmt_info);
      PIPE_MGR_FREE((char *)tbl->name);
    }
    for (i = 0; i < tbl_info->num_sful_tbls; ++i) {
      pipe_stful_tbl_info_t *tbl = &tbl_info->stful_tbl_list[i];
      for (j = 0; j < tbl->num_rmt_info; ++j) {
        for (k = 0; k < tbl->rmt_info[j].num_tbl_banks; ++k) {
          PIPE_MGR_FREE(tbl->rmt_info[j].bank_map[k].tbl_word_blk);
        }
        if (tbl->rmt_info[j].color_bank_map) {
          /* Only color based meters have color bank map and there is only one
           * bank
           */
          if (tbl->rmt_info[j].color_bank_map[0].tbl_word_blk) {
            PIPE_MGR_FREE(tbl->rmt_info[j].color_bank_map[0].tbl_word_blk);
          }
          PIPE_MGR_FREE(tbl->rmt_info[j].color_bank_map);
        }
        PIPE_MGR_FREE(tbl->rmt_info[j].bank_map);
      }
      PIPE_MGR_FREE(tbl->actions);
      if (tbl->reg_params) PIPE_MGR_FREE(tbl->reg_params);
      PIPE_MGR_FREE(tbl->rmt_info);
      PIPE_MGR_FREE((char *)tbl->name);
    }
    for (i = 0; i < tbl_info->num_select_tbls; ++i) {
      pipe_select_tbl_info_t *tbl = &tbl_info->select_tbl_list[i];
      for (j = 0; j < tbl->num_rmt_info; ++j) {
        for (k = 0; k < tbl->rmt_info[j].num_tbl_banks; ++k) {
          PIPE_MGR_FREE(tbl->rmt_info[j].bank_map[k].tbl_word_blk);
        }
        PIPE_MGR_FREE(tbl->rmt_info[j].bank_map);
      }
      PIPE_MGR_FREE(tbl->rmt_info);
      PIPE_MGR_FREE((char *)tbl->name);
    }
    if (tbl_info->num_mat_tbls) PIPE_MGR_FREE(tbl_info->mat_tbl_list);
    if (tbl_info->num_adt_tbls) PIPE_MGR_FREE(tbl_info->adt_tbl_list);
    if (tbl_info->num_stat_tbls) PIPE_MGR_FREE(tbl_info->stat_tbl_list);
    if (tbl_info->num_meter_tbls) PIPE_MGR_FREE(tbl_info->meter_tbl_list);
    if (tbl_info->num_sful_tbls) PIPE_MGR_FREE(tbl_info->stful_tbl_list);
    if (tbl_info->num_select_tbls) PIPE_MGR_FREE(tbl_info->select_tbl_list);

    /* Free the MAU stage characteristics information. */
    unsigned long key;
    void *data;
    while (BF_MAP_OK == bf_map_get_first_rmv(
                            &dev_info->profile_info[p]->stage_characteristics,
                            &key,
                            &data)) {
      if (data) PIPE_MGR_FREE(data);
    }
    bf_map_destroy(&dev_info->profile_info[p]->stage_characteristics);

    /* Free the config cache. */
    while (BF_MAP_OK ==
           bf_map_get_first_rmv(
               &dev_info->profile_info[p]->config_cache, &key, &data)) {
      switch (key) {
        case pipe_cck_meta_opt_ctrl:
          PIPE_MGR_FREE(data);
          break;
        case pipe_cck_parser_multi_threading:
          PIPE_MGR_FREE(data);
          break;
        case pipe_cck_meter_sweep_ctrl: {
          struct pipe_config_cache_meter_sweep_t *meter_sweep = data;
          for (i = 0; i < dev_info->num_active_mau; ++i) {
            PIPE_MGR_FREE(meter_sweep->val[i]);
          }
          PIPE_MGR_FREE(meter_sweep->val);
          PIPE_MGR_FREE(meter_sweep);
          break;
        }
        case pipe_cck_meter_ctrl: {
          struct pipe_config_cache_meter_ctl_t *meter_ctl = data;
          for (i = 0; i < dev_info->num_active_mau; ++i) {
            PIPE_MGR_FREE(meter_ctl->val[i]);
          }
          PIPE_MGR_FREE(meter_ctl->val);
          PIPE_MGR_FREE(meter_ctl);
          break;
        }
        case pipe_cck_hash_seed:
        case pipe_cck_hash_parity_group_mask: /* Fall through */
        case pipe_cck_xbar_din_power_ctrl: {  /* Fall through */
          struct pipe_config_cache_2d_byte_array_t *element = data;
          for (i = 0; i < dev_info->num_active_mau; ++i) {
            PIPE_MGR_FREE(element->val[i]);
          }
          PIPE_MGR_FREE(element->val);
          PIPE_MGR_FREE(element);
          break;
        }
        case pipe_cck_mau_stage_ext: {
          struct pipe_config_cache_bypass_stage_t *cfg = data;
          if (cfg->regs) {
            for (int ii = 0; ii < cfg->reg_cnt; ++ii) {
              if (cfg->regs[ii].offset) PIPE_MGR_FREE(cfg->regs[ii].offset);
              if (cfg->regs[ii].vals) PIPE_MGR_FREE(cfg->regs[ii].vals);
            }
            PIPE_MGR_FREE(cfg->regs);
          }
          break;
        }
      }
    }
    bf_map_destroy(&dev_info->profile_info[p]->config_cache);
    /* Free the dynamic hash calculations info */
    pipe_mgr_rmt_dynamic_selector_free(&dev_info->profile_info[p]->dhash_info);
  }
}

pipe_mat_tbl_hdl_t pipe_mgr_tbl_name_to_hdl(bf_dev_id_t dev, const char *name) {
  const rmt_dev_info_t *x = pipe_mgr_get_dev_info(dev);
  if (!x) return 0;
  rmt_dev_tbl_info_t *info = &x->profile_info[0]->tbl_info_list;
  if (!info) return 0;
  unsigned i;
  for (i = 0; i < info->num_mat_tbls; ++i)
    if (!strcmp(info->mat_tbl_list[i].name, name))
      return info->mat_tbl_list[i].handle;
  for (i = 0; i < info->num_adt_tbls; ++i)
    if (!strcmp(info->adt_tbl_list[i].name, name))
      return info->adt_tbl_list[i].handle;
  for (i = 0; i < info->num_stat_tbls; ++i)
    if (!strcmp(info->stat_tbl_list[i].name, name))
      return info->stat_tbl_list[i].handle;
  for (i = 0; i < info->num_meter_tbls; ++i)
    if (!strcmp(info->meter_tbl_list[i].name, name))
      return info->meter_tbl_list[i].handle;
  for (i = 0; i < info->num_sful_tbls; ++i)
    if (!strcmp(info->stful_tbl_list[i].name, name))
      return info->stful_tbl_list[i].handle;
  for (i = 0; i < info->num_select_tbls; ++i)
    if (!strcmp(info->select_tbl_list[i].name, name))
      return info->select_tbl_list[i].handle;
  return 0;
}

uint64_t pipe_mgr_get_clock_speed(bf_dev_id_t dev_id) {
  rmt_dev_info_t *dev_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    return 0;
  }
  return dev_info->clock_speed;
}

uint64_t pipe_mgr_get_bps_clock_speed(bf_dev_id_t dev_id) {
  const rmt_dev_info_t *dev_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    return 0;
  }
  return dev_info->bps_clock_speed;
}

uint64_t pipe_mgr_get_sp_clock_speed(bf_dev_id_t dev_id) {
  // special only for meter and idletime
  const rmt_dev_info_t *dev_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    return 0;
  }

  /* This function is used to get clock frequency for idletime and meters
   * If we are on the sw model, just return 1.27 ghz as the frequency.
   */
  if (dev_info->dev_family == BF_DEV_FAMILY_TOFINO) {
    bf_status_t sts;
    bool is_sw_model = false;
    /* Virtual device are neither model or HW so skip this check for virtual
     * devices. */
    if (!dev_info->virtual_device) {
      sts = bf_drv_device_type_get(dev_id, &is_sw_model);
      if (sts != BF_SUCCESS) {
        LOG_ERROR("%s:%d ERROR in getting device type for device id %d, err %d",
                  __func__,
                  __LINE__,
                  dev_id,
                  sts);
        PIPE_MGR_DBGCHK(0);
      }
      if (is_sw_model) {
        return 1271000000ull;
      }
    }
  }
  return dev_info->sp_clock_speed;
}

uint32_t pipe_mgr_nsec_to_clock(bf_dev_id_t dev, uint32_t ns) {
  uint64_t nanoseconds = ns;
  uint64_t clocks_per_sec = pipe_mgr_get_bps_clock_speed(dev);
  if (!clocks_per_sec) {
    LOG_ERROR("Cannot get clock speed at %s:%d", __func__, __LINE__);
    return ~UINT32_C(0);
  }
  uint64_t nanoseconds_per_second = 1000000000;
  uint64_t clocks = (nanoseconds * clocks_per_sec) / nanoseconds_per_second;
  return clocks & UINT64_C(0xFFFFFFFF00000000) ? 0xFFFFFFFF : clocks;
}

uint32_t pipe_mgr_clock_to_nsec(bf_dev_id_t dev, uint32_t clock) {
  uint64_t nanoseconds;
  uint64_t clocks_per_sec = pipe_mgr_get_bps_clock_speed(dev);
  if (!clocks_per_sec) {
    LOG_ERROR("Cannot get clock speed at %s:%d", __func__, __LINE__);
    return ~UINT32_C(0);
  }
  uint64_t nanoseconds_per_second = 1000000000;
  nanoseconds = clock * nanoseconds_per_second / clocks_per_sec;
  nanoseconds +=
      ((clock * nanoseconds_per_second) % clocks_per_sec == 0 ? 0 : 1);
  return nanoseconds & UINT64_C(0xFFFFFFFF00000000) ? 0xFFFFFFFF : nanoseconds;
}

/* Round to nearest value by adding half division part. */
uint64_t pipe_mgr_usec_to_clock(bf_dev_id_t dev, uint32_t us) {
  return ((uint64_t)us * pipe_mgr_get_clock_speed(dev) + 500000) / 1000000;
}

uint32_t pipe_mgr_clock_to_usec(bf_dev_id_t dev, uint64_t clock) {
  uint64_t clk_speed = pipe_mgr_get_clock_speed(dev);
  if (clk_speed == 0) return 0;
  return (uint32_t)(clock * 1000000 / clk_speed);
}

/* Round to nearest value by adding half division part. */
uint64_t pipe_mgr_usec_to_bps_clock(bf_dev_id_t dev, uint32_t us) {
  return ((uint64_t)us * pipe_mgr_get_bps_clock_speed(dev) + 500000) / 1000000;
}

rmt_port_info_t *pipe_mgr_get_port_info(bf_dev_id_t dev_id, uint16_t port_id) {
  rmt_dev_info_t *dev_info;
  rmt_port_info_t *port_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  for (port_info = dev_info->port_list; port_info;
       port_info = port_info->next) {
    if (port_info && port_info->port_id == port_id) {
      return port_info;
    }
  }

  return NULL;
}

static void free_dev_info(rmt_dev_info_t *dev_info) {
  if (dev_info->pipe_log_phy_map) PIPE_MGR_FREE(dev_info->pipe_log_phy_map);
  if (dev_info->pipe_phy_log_map) PIPE_MGR_FREE(dev_info->pipe_phy_log_map);
  if (dev_info->profile_info) {
    unsigned int i;
    for (i = 0; i < dev_info->num_pipeline_profiles; ++i) {
      PIPE_MGR_FREE(dev_info->profile_info[i]);
    }
    PIPE_MGR_FREE(dev_info->profile_info);
  }
  if (dev_info->mau_scratch_val) {
    unsigned int _p;
    for (_p = 0; _p < dev_info->num_active_pipes; ++_p) {
      if (dev_info->mau_scratch_val[_p]) {
        PIPE_MGR_FREE(dev_info->mau_scratch_val[_p]);
      }
    }
    PIPE_MGR_FREE(dev_info->mau_scratch_val);
    PIPE_MGR_LOCK_DESTROY(&dev_info->mau_scratch_mtx);
  }
  while (dev_info->port_list) {
    rmt_port_info_t *port_info = dev_info->port_list;
    BF_LIST_DLL_REM(dev_info->port_list, port_info, next, prev);
    PIPE_MGR_FREE(port_info);
  }


  PIPE_MGR_FREE(dev_info);
}

static pipe_status_t pipe_mgr_set_dev_info(rmt_dev_info_t **dev_info_p,
                                           bf_dev_id_t dev_id,
                                           bf_dev_type_t dev_type,
                                           bool is_virtual,
                                           bool is_virtual_dev_slave,
                                           bf_device_profile_t *profile) {
  pipe_status_t sts;
  unsigned int i;
  rmt_dev_info_t *dev_info = PIPE_MGR_MALLOC(sizeof(rmt_dev_info_t));
  if (dev_info == NULL) {
    return PIPE_NO_SYS_RESOURCES;
  }
  PIPE_MGR_MEMSET(dev_info, 0, sizeof(rmt_dev_info_t));

  /* Initialize device ID and type */
  dev_info->dev_id = dev_id;
  dev_info->dev_type = dev_type;
  dev_info->dev_family = bf_dev_type_to_family(dev_type);
  dev_info->virtual_device = is_virtual;
  dev_info->virtual_dev_slave = is_virtual_dev_slave;

  /* Copy configuration */
  switch (dev_info->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      PIPE_MGR_MEMCPY(&dev_info->dev_cfg, &tofino_cfg, sizeof(rmt_dev_cfg_t));
      break;
    case BF_DEV_FAMILY_TOFINO2:
      PIPE_MGR_MEMCPY(&dev_info->dev_cfg, &tof2_cfg, sizeof(rmt_dev_cfg_t));
      break;
    case BF_DEV_FAMILY_TOFINO3:
      PIPE_MGR_MEMCPY(&dev_info->dev_cfg, &tof3_cfg, sizeof(rmt_dev_cfg_t));
      break;





    default:
      LOG_ERROR(
          "%s: Unknown device family. Please check if running correct "
          "version of the drivers for the device.",
          __func__);
      PIPE_MGR_DBGCHK(0);
      return PIPE_INVALID_ARG;
  }

  /* Get the number of subdevices this device has. */
  sts = get_num_active_subdevices(dev_info, &dev_info->num_active_subdevices);
  if (PIPE_SUCCESS != sts) {
    LOG_ERROR("%s: Failed to get subdevice count", __func__);
    free_dev_info(dev_info);
    return sts;
  }
  PIPE_MGR_ASSERT(dev_info->num_active_subdevices != 0);
  /* Only Tofino3 has 2 sub-devices */
  if (dev_info->num_active_subdevices > 1) {
    if (dev_info->dev_family != BF_DEV_FAMILY_TOFINO3) {

        PIPE_MGR_ASSERT(0);

    }
    PIPE_MGR_ASSERT(dev_info->num_active_subdevices == 2);
  }

  /* Get the number of pipes this device has. */
  sts = get_num_active_pipes(dev_info, &dev_info->num_active_pipes);
  if (PIPE_SUCCESS != sts) {
    LOG_ERROR("%s: Failed to get pipe count", __func__);
    free_dev_info(dev_info);
    return sts;
  }
  sts = get_num_active_mau(dev_info, &dev_info->num_active_mau);
  if (PIPE_SUCCESS != sts) {
    LOG_ERROR("%s: Failed to get mau stage count", __func__);
    free_dev_info(dev_info);
    return sts;
  }
  /* Get the revision number. */
  lld_err_t rc =
      lld_sku_get_chip_part_revision_number(dev_id, &dev_info->part_rev);
  if (rc != LLD_OK) {
    LOG_ERROR(
        "Could not get chip part revision number, dev %d sts %d", dev_id, rc);
    free_dev_info(dev_info);
    return PIPE_INIT_ERROR;
  }

  /* Adjust num of pipes for two-die (25T) */
  if (dev_info->num_active_subdevices == 2) {
    dev_info->dev_cfg.num_pipelines = dev_info->dev_cfg.num_pipelines * 2;
  }

  /* Get the mappings between logical and physical pipe ids. */
  dev_info->pipe_log_phy_map = PIPE_MGR_CALLOC(
      dev_info->dev_cfg.num_pipelines, sizeof(*dev_info->pipe_log_phy_map));
  if (!dev_info->pipe_log_phy_map) {
    free_dev_info(dev_info);
    return PIPE_NO_SYS_RESOURCES;
  }
  dev_info->pipe_phy_log_map = PIPE_MGR_CALLOC(
      dev_info->dev_cfg.num_pipelines, sizeof(*dev_info->pipe_phy_log_map));
  if (!dev_info->pipe_phy_log_map) {
    free_dev_info(dev_info);
    return PIPE_NO_SYS_RESOURCES;
  }
  for (i = 0; i < dev_info->dev_cfg.num_pipelines; i++) {
    dev_info->pipe_log_phy_map[i] = 0xFF;
    dev_info->pipe_phy_log_map[i] = 0xFF;
  }
  for (i = 0; i < dev_info->dev_cfg.num_pipelines; i++) {
    if (i < dev_info->num_active_pipes) {
      bf_dev_pipe_t l = i, p;
      sts = pipe_id_to_phy_pipe_id(dev_info, l, &p);
      if (PIPE_SUCCESS != sts) {
        LOG_ERROR(
            "Failed to map logical pipe %d to physical id, dev %d error %s",
            l,
            dev_info->dev_id,
            pipe_str_err(sts));
        PIPE_MGR_DBGCHK(PIPE_SUCCESS != sts);
        free_dev_info(dev_info);
        return sts;
      }
      dev_info->pipe_log_phy_map[l] = p;
      dev_info->pipe_phy_log_map[p] = l;
    }
  }

  /* Get the device's clock speed. */
  uint64_t bps_clock_speed, pps_clock_speed;
  bf_status_t status;

  /* Get core clock frequency */
  if (dev_info->virtual_device) {
    /* Virtual devices will use the default clock speed for the chip family. */
    switch (dev_info->dev_family) {
      case BF_DEV_FAMILY_TOFINO:
        bps_clock_speed = pps_clock_speed = 1220000000ull;
        break;
      case BF_DEV_FAMILY_TOFINO2:
        bps_clock_speed = 1312500000ull;
        pps_clock_speed = 1500000000ull;
        break;
      case BF_DEV_FAMILY_TOFINO3:
        bps_clock_speed = 1300000000ull;
        pps_clock_speed = 1262500000ull;
        break;

        PIPE_MGR_DBGCHK(0);
        free_dev_info(dev_info);
        return PIPE_UNEXPECTED;
      default:
        PIPE_MGR_DBGCHK(0);
        free_dev_info(dev_info);
        return PIPE_UNEXPECTED;
    }
  } else {
    status = bf_drv_get_clock_speed(
        dev_info->dev_id, &bps_clock_speed, &pps_clock_speed);
    if (status != BF_SUCCESS) {
      LOG_ERROR("Not able to determine core clock speed, dev %d, sts %s (%d)",
                dev_info->dev_id,
                bf_err_str(status),
                status);
      return status;
    }
  }

  dev_info->clock_speed = pps_clock_speed;
  dev_info->bps_clock_speed = bps_clock_speed;

#ifdef DEVICE_IS_EMULATOR
  if (dev_info->clock_speed == 1600000000ull) {
    dev_info->clock_speed = 200000ull;
  }
#endif
  LOG_TRACE("set clock speed to %" PRIu64 " , bps clock speed to %" PRIu64
            " for dev %d",
            dev_info->clock_speed,
            dev_info->bps_clock_speed,
            dev_info->dev_id);

  switch (dev_info->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      dev_info->sp_clock_speed = dev_info->clock_speed;
      break;
    case BF_DEV_FAMILY_TOFINO2:
    case BF_DEV_FAMILY_TOFINO3:

      dev_info->sp_clock_speed = 1000000000ull;  // 1GHZ
      break;
    default:
      PIPE_MGR_DBGCHK(0);
      free_dev_info(dev_info);
      return PIPE_LLD_FAILED;
  }

  /* Save the number of pipeline profiles and allocate them. */
  if (!profile) {
    dev_info->num_pipeline_profiles = 1;
  } else {
    int p = 0;
    dev_info->num_pipeline_profiles = 0;
    for (p = 0; p < profile->num_p4_programs; p++) {
      dev_info->num_pipeline_profiles +=
          profile->p4_programs[p].num_p4_pipelines;
    }
  }
  dev_info->profile_info = PIPE_MGR_CALLOC(dev_info->num_pipeline_profiles,
                                           sizeof(rmt_dev_profile_info_t *));
  if (!dev_info->profile_info) {
    free_dev_info(dev_info);
    return PIPE_NO_SYS_RESOURCES;
  }
  for (i = 0; i < dev_info->num_pipeline_profiles; ++i) {
    dev_info->profile_info[i] =
        PIPE_MGR_CALLOC(1, sizeof(rmt_dev_profile_info_t));
    if (!dev_info->profile_info[i]) {
      free_dev_info(dev_info);
      return PIPE_NO_SYS_RESOURCES;
    }
    dev_info->profile_info[i]->profile_id = i;
  }

  PIPE_MGR_LOCK_INIT(dev_info->mau_scratch_mtx);
  dev_info->mau_scratch_val = PIPE_MGR_CALLOC(
      dev_info->num_active_pipes, sizeof(*dev_info->mau_scratch_val));
  if (!dev_info->mau_scratch_val) {
    free_dev_info(dev_info);
    return PIPE_NO_SYS_RESOURCES;
  }
  unsigned int _p;
  for (_p = 0; _p < dev_info->num_active_pipes; ++_p) {
    dev_info->mau_scratch_val[_p] = PIPE_MGR_CALLOC(
        dev_info->num_active_mau, sizeof(*dev_info->mau_scratch_val[_p]));
    if (!dev_info->mau_scratch_val[_p]) {
      free_dev_info(dev_info);
      return PIPE_NO_SYS_RESOURCES;
    }
  }

  bf_map_sts_t s = bf_map_add(&pipe_mgr_ctx->dev_info_map, dev_id, dev_info);
  if (BF_MAP_OK != s) {
    free_dev_info(dev_info);
    LOG_ERROR("Failed to save device info, dev %d sts %d", dev_id, s);
    return PIPE_UNEXPECTED;
  }
  *dev_info_p = dev_info;
  return PIPE_SUCCESS;
}

bool pipe_mgr_is_device_present(bf_dev_id_t dev_id) {
  if (dev_id >= PIPE_MGR_NUM_DEVICES) return false;
  return !!pipe_mgr_get_dev_info(dev_id);
}
bool pipe_mgr_is_device_virtual(bf_dev_id_t dev_id) {
  rmt_dev_info_t *x = pipe_mgr_get_dev_info(dev_id);
  return x ? x->virtual_device : false;
}

bool pipe_mgr_is_device_virtual_dev_slave(bf_dev_id_t dev_id) {
  rmt_dev_info_t *x = pipe_mgr_get_dev_info(dev_id);
  return x ? x->virtual_dev_slave : false;
}

/* Init timestamp block to generate global timestamp.
 * This global timestamp is used by parde and TM blocks
 * for various time related operations.
 */
static void tstamp_init_tof(const rmt_dev_info_t *dev_info) {
  bf_dev_id_t dev_id = dev_info->dev_id;
  uint32_t val;

  val = 1000;  // # of clock cycles (~1us).
  lld_write_register(
      dev_id, offsetof(Tofino, device_select.mbc.mbc_mbus.ts_timer), val);

  /* This is a wide register but we will leave the high bits at zero so we only
   * need to write the lowest address. */
  val = 0x3;  // MBC block delay offset.
  lld_write_register(
      dev_id,
      offsetof(Tofino, device_select.mbc.mbc_mbus.global_ts_offset_value),
      val);

  double ts_inc_val = (double)pipe_mgr_get_clock_speed(dev_id) / 1000000000.0;
  ts_inc_val = 1.0 / ts_inc_val;
  ts_inc_val *= (1 << 28);  // fractional nano sec in bottom 28 bits.
  val = ts_inc_val + 1;     // cealing value.

  // For 1.1Ghz, val should be 0xE8BA2E9
  lld_write_register(
      dev_id,
      offsetof(Tofino, device_select.mbc.mbc_mbus.global_ts_inc_value),
      val);

  val = 0;  // Start with timestamp value 0.
  lld_write_register(
      dev_id, offsetof(Tofino, device_select.mbc.mbc_mbus.global_ts_set), val);
  val = 2;  // Enable timestamp
  lld_write_register(
      dev_id, offsetof(Tofino, device_select.mbc.mbc_mbus.ctrl), val);
}

static void tstamp_init_tof2(const rmt_dev_info_t *dev_info) {
  bf_dev_id_t dev_id = dev_info->dev_id;
  uint32_t val;

  val = 10000;
  lld_write_register(
      dev_id, offsetof(tof2_reg, device_select.mbc.mbc_mbus.ts_timer), val);

  /* The global_ts_offset_value is a wide register and the write to the highest
   * address commits the write (which is different than Tofino) so write both
   * sections of it. */
  val = 0x3;
  lld_write_register(dev_id,
                     offsetof(tof2_reg,
                              device_select.mbc.mbc_mbus.global_ts_offset_value
                                  .global_ts_offset_value_0_2),
                     val);
  lld_write_register(dev_id,
                     offsetof(tof2_reg,
                              device_select.mbc.mbc_mbus.global_ts_offset_value
                                  .global_ts_offset_value_1_2),
                     0);

  lld_write_register(
      dev_id,
      offsetof(tof2_reg,
               device_select.mbc.mbc_mbus.global_ts_set.global_ts_set_0_2),
      0);
  lld_write_register(
      dev_id,
      offsetof(tof2_reg,
               device_select.mbc.mbc_mbus.global_ts_set.global_ts_set_1_2),
      0);

  val = 2;  // Enable timestamp
  lld_write_register(
      dev_id, offsetof(tof2_reg, device_select.mbc.mbc_mbus.ctrl), val);
}

static void tstamp_init_tof3(const rmt_dev_info_t *dev_info) {
  bf_dev_id_t dev_id = dev_info->dev_id;
  uint32_t val;
  uint32_t subdev = 0;

  for (subdev = 0; subdev < dev_info->num_active_subdevices; subdev++) {
    val = 10000;
    lld_subdev_write_register(
        dev_id,
        subdev,
        offsetof(tof3_reg, device_select.mbc.mbc_mbus.ts_timer),
        val);

    /* The global_ts_offset_value is a wide register and the write to the
     * highest address commits the write (which is different than Tofino) so
     * write both sections of it. */
    val = 0x3;
    lld_subdev_write_register(
        dev_id,
        subdev,
        offsetof(tof3_reg,
                 device_select.mbc.mbc_mbus.global_ts_offset_value
                     .global_ts_offset_value_0_2),
        val);
    lld_subdev_write_register(
        dev_id,
        subdev,
        offsetof(tof3_reg,
                 device_select.mbc.mbc_mbus.global_ts_offset_value
                     .global_ts_offset_value_1_2),
        0);

    lld_subdev_write_register(
        dev_id,
        subdev,
        offsetof(tof3_reg,
                 device_select.mbc.mbc_mbus.global_ts_set.global_ts_set_0_2),
        0);
    lld_subdev_write_register(
        dev_id,
        subdev,
        offsetof(tof3_reg,
                 device_select.mbc.mbc_mbus.global_ts_set.global_ts_set_1_2),
        0);

    val = 2;  // Enable timestamp
    lld_subdev_write_register(
        dev_id,
        subdev,
        offsetof(tof3_reg, device_select.mbc.mbc_mbus.ctrl),
        val);
  }
}

















































void pipe_mgr_tstamp_init(const rmt_dev_info_t *dev_info) {
  switch (dev_info->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      tstamp_init_tof(dev_info);
      break;
    case BF_DEV_FAMILY_TOFINO2:
      tstamp_init_tof2(dev_info);
      break;
    case BF_DEV_FAMILY_TOFINO3:
      tstamp_init_tof3(dev_info);
      break;





    default:
      PIPE_MGR_DBGCHK(0);
      break;
  }
}

/* Initialize the device for a quick warm reconfig without device add */
bf_status_t pipe_mgr_warm_init_quick(bf_dev_id_t dev_id) {
  rmt_dev_info_t *dev_info;
  pipe_status_t sts = PIPE_SUCCESS;
  pipe_sess_hdl_t sess_hdl = pipe_mgr_ctx->int_ses_hndl;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    return PIPE_OBJ_NOT_FOUND;
  }

  pipe_mgr_init_mode_set(dev_id, BF_DEV_WARM_INIT_FAST_RECFG_QUICK);

  if (!dev_info->virtual_device) {
    /* Device config */
    LOG_TRACE("%s: Starting pipe_mgr_drv_prepare_device", __func__);
    sts = pipe_mgr_drv_prepare_device(sess_hdl, dev_info);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to prepare device", __func__);
      goto err_cleanup;
    }
    LOG_TRACE("%s: Starting config download", __func__);
    sts = pipe_mgr_blob_download(sess_hdl, dev_info);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to download config", __func__);
      goto err_cleanup;
    }

    LOG_TRACE("%s: Starting pipe_mgr_hw_notify_cfg", __func__);
    sts = pipe_mgr_hw_notify_cfg(dev_info);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to setup hw notify cfg", __func__);
      goto err_cleanup;
    }

    LOG_TRACE("%s: Starting pipe_mgr_drv_lrt_buf_warm_init_quick", __func__);
    sts = pipe_mgr_drv_lrt_buf_warm_init_quick(dev_id);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to init LR(t) buffers for a quick warm reconfig",
                __func__);
      goto err_cleanup;
    }
  }

  if (!dev_info->virtual_device) {
    /* Add static and default entries to tables if specified */
    sts = pipe_mgr_static_and_default_entry_init(sess_hdl, dev_id);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to add static entries. Device %u, status %u",
                __func__,
                dev_id,
                sts);
      goto err_cleanup;
    }

    LOG_TRACE("%s: Starting pipe_mgr_parde_device_add", __func__);
    sts = pipe_mgr_parde_device_add(sess_hdl, dev_info);
    if (PIPE_SUCCESS != sts) goto err_cleanup;

#ifndef EMU_SKIP_BLOCKS_OPT
    LOG_TRACE("%s: Starting pipe_mgr_mirror_buf_init", __func__);
    if (PIPE_SUCCESS != pipe_mgr_mirror_buf_init(sess_hdl, dev_id))
      goto err_cleanup;

    LOG_TRACE("%s: Starting pipe_mgr_pktgen_warm_init_quick", __func__);
    if (BF_SUCCESS != pipe_mgr_pktgen_warm_init_quick(sess_hdl, dev_id)) {
      goto err_cleanup;
    }
#endif
  }

  /* Port config */
  LOG_TRACE("%s: Starting port config", __func__);
  rmt_port_info_t *port_info;
  for (port_info = dev_info->port_list; port_info;
       port_info = port_info->next) {
    bf_status_t x = pipe_mgr_pktgen_port_add(
        dev_info, port_info->port_id, port_info->speed);
    if (x != BF_SUCCESS) {
      LOG_ERROR("%s: failed to initialize PGR port controls %d (%s)",
                __func__,
                x,
                bf_err_str(x));
      sts = PIPE_COMM_FAIL;
      goto err_cleanup;
    }

    sts = pipe_mgr_parde_port_add(
        sess_hdl, dev_info, BF_PORT_CB_DIRECTION_EGRESS, port_info->port_id);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR(
          "%s: Parde port config egress failed for port %d on dev %d, sts %s",
          __func__,
          port_info->port_id,
          dev_id,
          pipe_str_err(sts));
      goto err_cleanup;
    }

    sts = pipe_mgr_parde_port_add(
        sess_hdl, dev_info, BF_PORT_CB_DIRECTION_INGRESS, port_info->port_id);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR(
          "%s: Parde port config ingress failed for port %d on dev %d, sts %s",
          __func__,
          port_info->port_id,
          dev_id,
          pipe_str_err(sts));
      goto err_cleanup;
    }
  }

  return PIPE_SUCCESS;

err_cleanup:

  return sts;
}

/* API to instantiate a new device */
pipe_status_t pipe_mgr_add_rmt_device(pipe_sess_hdl_t sess_hdl,
                                      bf_dev_id_t dev_id,
                                      bool virtual_device,
                                      bool virtual_device_slave,
                                      bf_dev_type_t dev_type,
                                      bf_device_profile_t *profile,
                                      bf_dev_init_mode_t dev_init_mode) {
  pipe_status_t sts = PIPE_SUCCESS;
  rmt_dev_info_t *dev_info = pipe_mgr_get_dev_info(dev_id);
  if (dev_info) {
    return PIPE_ALREADY_EXISTS;
  }

  /* Allocate and register device context block. */
  struct pipe_mgr_dev_ctx *dev_ctx = PIPE_MGR_CALLOC(1, sizeof *dev_ctx);
  if (!dev_ctx) return PIPE_NO_SYS_RESOURCES;
  /* Initialize a few maps and related mutexes in the device context. */
  bf_map_init(&dev_ctx->stat_tbl_hdls);
  bf_map_init(&dev_ctx->stat_tbl_map);
  bf_map_init(&dev_ctx->stful_tbls);
  bf_map_init(&dev_ctx->p0_tbls);
  bf_map_init(&dev_ctx->meter_tbl_map);
  bf_map_init(&dev_ctx->adt_tbl_map);
  bf_map_init(&dev_ctx->sel_ctx.tbl_hdl_to_tbl_map);
  bf_map_init(&dev_ctx->sel_ctx.tbl_hdl_to_btbl_map);
  bf_map_init(&dev_ctx->tcam_ctx.tbl_hdl_to_tbl_map);
  bf_map_init(&dev_ctx->tcam_ctx.tbl_hdl_to_btbl_map);
  bf_map_init(&dev_ctx->exm_tbl_map);
  bf_map_init(&dev_ctx->dkm_tbl_map);
  bf_map_init(&dev_ctx->overspeed_25g_map);
  PIPE_MGR_LOCK_INIT(dev_ctx->stat_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->stful_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->p0_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->meter_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->adt_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->sel_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->tcam_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->idle_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->exm_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->dkm_tbl_mtx);
  PIPE_MGR_LOCK_INIT(dev_ctx->overspeed_25g_mtx);
  pipe_mgr_dev_ctx_set(dev_id, dev_ctx);

  pipe_mgr_init_mode_set(dev_id, dev_init_mode);

  /* A flag to indicate if we are in cold boot or warm boot.  This will be used
   * at the end of the function to decide if DMA should be pushed immediately or
   * postponed until a later warm-init-end API is called.  Since DMA operations
   * don't apply to virtual devices, allow that flag to override this one. */
  bool push_dma = !pipe_mgr_is_device_locked(dev_id) && !virtual_device;
  if (push_dma) {
    pipe_mgr_lock_device(dev_id);  // TBD FIX subdev_id
  }

  /* Init and register device info structure. */
  sts = pipe_mgr_set_dev_info(&dev_info,
                              dev_id,
                              dev_type,
                              virtual_device,
                              virtual_device_slave,
                              profile);
  if (sts != PIPE_SUCCESS) {
    LOG_ERROR("%s: failed to initialize device info", __func__);
    goto err_cleanup;
  }

  if (!virtual_device) {
    /* Init shadow memory structs. */
    sts = pipe_mgr_phy_mem_map_init(dev_id);
    if (sts != PIPE_SUCCESS) goto err_cleanup;
  }

  /* Init memory needed for Hash compute.  */
  if (bf_hash_compute_init(dev_id, dev_info->num_pipeline_profiles) !=
      PIPE_SUCCESS) {
    LOG_ERROR("%s: failed to initialize hash compute module", __func__);
    goto err_cleanup;
  }

  /* Init table packing structs. */
  if (pipemgr_tbl_pkging_init(dev_id, dev_info->num_pipeline_profiles) !=
      PIPE_SUCCESS) {
    LOG_ERROR("%s: failed to initialize table packing module", __func__);
    goto err_cleanup;
  }

  /* Init parser value set database. */
  if (pipemgr_pvs_db_init(dev_id, dev_info->num_pipeline_profiles) !=
      PIPE_SUCCESS) {
    LOG_ERROR("%s: failed to initialize pvs module", __func__);
    goto err_cleanup;
  }

  /* Init parser database. */
  sts = pipe_mgr_prsr_db_init(dev_info);
  if (sts != PIPE_SUCCESS) {
    LOG_ERROR("%s: failed to setup prsr db", __func__);
    goto err_cleanup;
  }

  /* Parse the context json and load the table configuration and the
   * shared register configuration cache.
   *
   * This must be done before any init functions that need table info
   * or the configuration cache.
   */
  LOG_TRACE("%s: Starting pipe_mgr_rmt_import ", __func__);
  sts = pipe_mgr_rmt_import(dev_id, dev_info, profile, virtual_device);
  if (sts != PIPE_SUCCESS) {
    LOG_ERROR("%s: failed to import rmt config", __func__);
    goto err_cleanup;
  }

  /* Add device to idletime manager. */
  sts = pipe_mgr_idle_add_device(dev_id);
  if (sts != PIPE_SUCCESS) {
    LOG_ERROR("%s: dev %d, failed to setup idle tables (%s)",
              __func__,
              dev_id,
              pipe_str_err(sts));
    goto err_cleanup;
  }

  if (!virtual_device) {
    /* Init pipe manager DB entry. */
    sts = pipe_mgr_db_init(dev_info);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to init db", __func__);
      goto err_cleanup;
    }

    /* Init the interrupt event DB entry. */
    sts = pipe_mgr_intr_init(dev_info);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to init interrupt evt db", __func__);
      goto err_cleanup;
    }

    /* Init the select table shadow DB. */
    sts = pipe_mgr_sel_shadow_db_init(dev_id);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to init selector shadow db", __func__);
      goto err_cleanup;
    }

    /* Init DR (Descriptor Ring) sizes. */
    sts = pipe_mgr_drv_get_dr_size(dev_id);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to get dr size", __func__);
      goto err_cleanup;
    }

    /* Prepare device for use. */
    LOG_TRACE("%s: Starting pipe_mgr_drv_prepare_device", __func__);
    sts = pipe_mgr_drv_prepare_device(sess_hdl, dev_info);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to prepare device", __func__);
      goto err_cleanup;
    }

    /* Unless this is a hitless warm reinit, download the configuration
     * to the associated pipes. */
    if (!pipe_mgr_init_mode_hitless(dev_id)) {
      LOG_TRACE("%s: Starting config download", __func__);
      sts = pipe_mgr_blob_download(sess_hdl, dev_info);
      if (sts != PIPE_SUCCESS) {
        LOG_ERROR("%s: failed to download config", __func__);
        goto err_cleanup;
      }
    }
  }

  /* Initialize Session Management for this device. */
  LOG_TRACE("%s: Starting pipe_mgr_sm_init ", __func__);
  sts = pipe_mgr_sm_init(sess_hdl, dev_id);
  if (sts != PIPE_SUCCESS) {
    LOG_ERROR("%s: failed to initialize SM for device %u, status %u",
              __func__,
              dev_id,
              sts);
    goto err_cleanup;
  }
  LOG_TRACE("%s: Finished pipe_mgr_sm_init ", __func__);

  if (pipe_mgr_init_mode_hitless(dev_id)) {
    /* Handle the HA stuff */
    sts = pipe_mgr_hitless_ha_init(sess_hdl, dev_id);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to initialize hitless HA for device %u, status %u",
                __func__,
                dev_id,
                sts);
      goto err_cleanup;
    }
    LOG_TRACE("%s: Finished pipe_mgr_hitless_ha_init", __func__);
  }

  if (!virtual_device) {
    LOG_TRACE("%s: Starting pipe_mgr_hw_notify_cfg", __func__);
    sts = pipe_mgr_hw_notify_cfg(dev_info);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to setup hw notify cfg", __func__);
      goto err_cleanup;
    }

    LOG_TRACE("%s: Starting pipe_mgr_drv_setup_fm_buffers", __func__);
    sts = pipe_mgr_drv_setup_fm_buffers(sess_hdl, dev_id);
    if (sts != PIPE_SUCCESS) {
      LOG_ERROR("%s: failed to setup free-mem buffers", __func__);
      goto err_cleanup;
    }
  }

  if (!virtual_device) {
    /* Add static and default entries to tables if specified */
    if (!pipe_mgr_init_mode_hitless(dev_id)) {
      sts = pipe_mgr_static_and_default_entry_init(sess_hdl, dev_id);
      if (sts != PIPE_SUCCESS) {
        LOG_ERROR("%s: failed to add static entries. Device %u, status %u",
                  __func__,
                  dev_id,
                  sts);
        goto err_cleanup;
      }
    }

    sts = pipe_mgr_parde_device_add(sess_hdl, dev_info);
    if (PIPE_SUCCESS != sts) goto err_cleanup;

#ifndef EMU_SKIP_BLOCKS_OPT
    if (PIPE_SUCCESS != pipe_mgr_mirror_buf_init(sess_hdl, dev_id))
      goto err_cleanup;
    if (BF_SUCCESS != pipe_mgr_pktgen_add_dev(sess_hdl, dev_id)) {
      goto err_cleanup;
    }
#endif
    if (PIPE_SUCCESS != pipe_mgr_snapshot_add_device(dev_id)) goto err_cleanup;
    if (PIPE_SUCCESS != pipe_mgr_tbl_dbg_counter_init(dev_id)) goto err_cleanup;
  }

  if (push_dma) {
    pipe_mgr_reconfig_create_dma(dev_id);
    pipe_mgr_unlock_device_internal(dev_id, BF_DEV_INIT_COLD);
    pipe_mgr_config_complete(dev_id);
    pipe_mgr_enable_traffic(dev_id);
  }














  return PIPE_SUCCESS;
err_cleanup:
  if (push_dma) pipe_mgr_unlock_device_cleanup(dev_id);
  if (dev_info) {
    bf_map_rmv(&pipe_mgr_ctx->dev_info_map, dev_id);
    free_dev_info(dev_info);
  }
  pipe_mgr_db_cleanup(dev_id);
  pipe_mgr_phy_mem_map_cleanup(dev_id);

  if ((dev_ctx = pipe_mgr_dev_ctx(dev_id))) {
    bf_map_destroy(&dev_ctx->stat_tbl_hdls);
    bf_map_destroy(&dev_ctx->stat_tbl_map);
    bf_map_destroy(&dev_ctx->stful_tbls);
    bf_map_destroy(&dev_ctx->p0_tbls);
    bf_map_destroy(&dev_ctx->meter_tbl_map);
    bf_map_destroy(&dev_ctx->adt_tbl_map);
    bf_map_destroy(&dev_ctx->sel_ctx.tbl_hdl_to_tbl_map);
    bf_map_destroy(&dev_ctx->sel_ctx.tbl_hdl_to_btbl_map);
    bf_map_destroy(&dev_ctx->tcam_ctx.tbl_hdl_to_tbl_map);
    bf_map_destroy(&dev_ctx->tcam_ctx.tbl_hdl_to_btbl_map);
    bf_map_destroy(&dev_ctx->exm_tbl_map);
    bf_map_destroy(&dev_ctx->overspeed_25g_map);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->stat_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->stful_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->p0_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->meter_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->adt_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->sel_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->tcam_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->idle_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->exm_tbl_mtx);
    PIPE_MGR_LOCK_DESTROY(&dev_ctx->overspeed_25g_mtx);
    PIPE_MGR_MEMSET(dev_ctx, 0, sizeof(struct pipe_mgr_dev_ctx));
  }
  return sts;
}

/* API to de-instantiate a device */
pipe_status_t pipe_mgr_remove_rmt_device(bf_dev_id_t dev_id) {
  rmt_dev_info_t *dev_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    return PIPE_OBJ_NOT_FOUND;
  }

  bool virtual_device = dev_info->virtual_device;
  pipe_mgr_db_cleanup(dev_id);
  if (!virtual_device) {
    /* Free the shadow memory map */
    pipe_mgr_phy_mem_map_cleanup(dev_id);

    /* Clean up interrupt */
    pipe_mgr_intr_cleanup(dev_id);
    /* Clean up selector shadow memory state */
    pipe_mgr_sel_shadow_db_cleanup(dev_id);

    /* Inform snapshot-mgr */
    pipe_mgr_snapshot_remove_device(dev_id);

    /* Cleanup dbg counter db */
    pipe_mgr_tbl_dbg_counter_cleanup(dev_id);

    /* Cleanup driver state */
    pipe_mgr_drv_cleanup_fm_buffers(dev_id);
    pipe_mgr_drv_device_cleanup(dev_id);

#ifndef EMU_SKIP_BLOCKS_OPT
    /* Clean up the packet generator context. */
    pipe_mgr_pktgen_rmv_dev(dev_id);
#endif
    /* Clean up the parde context. */
    pipe_mgr_parde_device_rmv(dev_info);

#ifndef EMU_SKIP_BLOCKS_OPT
    /* Clean up mirroring context. */
    pipe_mgr_mirror_buf_cleanup(dev_id);
#endif
  }

  /* Clean up session management state. */
  pipe_mgr_sm_cleanup(dev_id);

  pipe_mgr_idle_remove_device(dev_id);

  /* Free up memory for Hash compute */
  bf_hash_compute_destroy(dev_id);

  /* Free up memory if allocated for PVS */
  pipe_mgr_free_pvs(dev_id);

  /* Free up memory if allocated for dynamic match key mask */
  pipemgr_dkm_cleanup(dev_id);

  /* Free the memory allocated when the RMT tables were imported. */
  pipe_mgr_rmt_import_free(dev_id, dev_info);

  /* Free device object */
  free_dev_info(dev_info);

  /* Free up memory for the entry encoder. */
  pipemgr_tbl_pkging_cleanup(dev_id);

  /* Free up device context. */
  pipe_mgr_dev_ctx_rmv(dev_id);

  /* Remove device from the list of known devices */
  bf_map_rmv(&pipe_mgr_ctx->dev_info_map, dev_id);

  return PIPE_SUCCESS;
}

/* API to add a port to the device */
pipe_status_t pipe_mgr_add_rmt_port(bf_dev_id_t dev_id,
                                    bf_dev_port_t port_id,
                                    bf_port_speeds_t speed) {
  rmt_port_info_t *port_info;
  rmt_dev_info_t *dev_info;

  /* Check if a port already exists with the port_id */
  if ((port_info = pipe_mgr_get_port_info(dev_id, port_id))) {
    LOG_ERROR("%s: port with port_id(%d) already exists", __func__, port_id);
    return PIPE_ALREADY_EXISTS;
  }

  /* Validate input args */
  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return PIPE_OBJ_NOT_FOUND;
  }

  port_info = PIPE_MGR_MALLOC(sizeof(rmt_port_info_t));
  if (port_info == NULL) {
    return PIPE_NO_SYS_RESOURCES;
  }
  PIPE_MGR_MEMSET(port_info, 0, sizeof(rmt_port_info_t));

  /* Get the physical pipe id of the port. */
  bf_dev_pipe_t phy_pipe;
  pipe_mgr_map_pipe_id_log_to_phy(
      dev_info, dev_info->dev_cfg.dev_port_to_pipe(port_id), &phy_pipe);

  /* Initialize port ID and speed */
  port_info->port_id = port_id;
  port_info->speed = speed;
  port_info->phy_pipe = phy_pipe;

  /* Add to the list of ports on the device */
  BF_LIST_DLL_AP(dev_info->port_list, port_info, next, prev);

  return PIPE_SUCCESS;
}

/* API to remove a port from the device */
pipe_status_t pipe_mgr_remove_rmt_port(bf_dev_id_t dev_id, uint16_t port_id) {
  rmt_port_info_t *port_info;
  rmt_dev_info_t *dev_info;

  /* Find the port */
  if ((port_info = pipe_mgr_get_port_info(dev_id, port_id)) == NULL) {
    LOG_ERROR("%s: port with port_id(%d) not found", __func__, port_id);
    return PIPE_OBJ_NOT_FOUND;
  }

  /* Remove port from the device's list of ports */
  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return PIPE_OBJ_NOT_FOUND;
  }
  BF_LIST_DLL_REM(dev_info->port_list, port_info, next, prev);

  /* Free port object */
  PIPE_MGR_FREE(port_info);

  return PIPE_SUCCESS;
}

uint8_t pipe_mgr_get_num_ram_select_bits(rmt_tbl_info_t *rmt_tbl_info,
                                         uint32_t hash_way_idx) {
  return (
      rmt_tbl_info->bank_map[hash_way_idx].hash_bits.ram_unit_select_bit_hi -
      rmt_tbl_info->bank_map[hash_way_idx].hash_bits.ram_unit_select_bit_lo);
}

uint8_t pipe_mgr_get_ram_select_offset(rmt_tbl_info_t *rmt_tbl_info,
                                       uint32_t hash_way_idx) {
  return (
      rmt_tbl_info->bank_map[hash_way_idx].hash_bits.ram_unit_select_bit_lo);
}

uint16_t pipe_mgr_get_tcam_unit_depth(bf_dev_id_t dev_id) {
  rmt_dev_info_t *dev_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return 0;
  }
  if (!dev_info->num_active_pipes) {
    LOG_ERROR("%s: no pipelines on device", __func__);
    return 0;
  }

  return dev_info->dev_cfg.stage_cfg.tcam_unit_depth;
}

















uint16_t pipe_mgr_get_sram_unit_depth(bf_dev_id_t dev_id) {
  rmt_dev_info_t *dev_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return 0;
  }
  if (!dev_info->num_active_pipes) {
    LOG_ERROR("%s: no pipelines on device", __func__);
    return 0;
  }

  return dev_info->dev_cfg.stage_cfg.sram_unit_depth;
}















uint16_t pipe_mgr_get_mapram_unit_depth(bf_dev_id_t dev_id) {
  rmt_dev_info_t *dev_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return 0;
  }
  if (!dev_info->num_active_pipes) {
    LOG_ERROR("%s: no pipelines on device", __func__);
    return 0;
  }

  return dev_info->dev_cfg.stage_cfg.map_ram_unit_depth;
}

uint16_t pipe_mgr_get_mem_unit_depth(bf_dev_id_t dev_id,
                                     rmt_mem_type_t mem_type) {
  switch (mem_type) {
    case RMT_MEM_SRAM:

      return pipe_mgr_get_sram_unit_depth(dev_id);
    case RMT_MEM_TCAM:

      return pipe_mgr_get_tcam_unit_depth(dev_id);




    case RMT_MEM_MAP_RAM:
      return pipe_mgr_get_mapram_unit_depth(dev_id);


    default:
      break;
  }
  return ~0;
}

/* Is a P4 skipped on the asic */
bool pipe_mgr_is_p4_skipped(const rmt_dev_info_t *dev_info) {
  /* bf-drivers could be started with skip-p4 option. P4's are
     not loaded in this case as bf-switchd passes in zero profiles.
  */
  if (dev_info->num_pipeline_profiles == 0) {
    return true;
  }
  return false;
}

rmt_dev_profile_info_t *pipe_mgr_get_profile_info(bf_dev_id_t dev_id,
                                                  profile_id_t profile_id,
                                                  const char *where,
                                                  const int line) {
  rmt_dev_info_t *dev_info = NULL;
  rmt_dev_profile_info_t *profile = NULL;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return NULL;
  }

  if (profile_id >= (int)dev_info->num_pipeline_profiles) {
    LOG_ERROR("Invalid profile(%d) requested from %s:%d, dev %d",
              profile_id,
              where,
              line,
              dev_info->dev_id);
    PIPE_MGR_DBGCHK(0);
    return NULL;
  }

  profile = dev_info->profile_info[profile_id];
  if (!profile) {
    LOG_ERROR(
        "Unknown profile(%d) requested from %s:%d", profile_id, where, line);
  }
  return profile;
}

rmt_dev_profile_info_t *pipe_mgr_get_profile(const rmt_dev_info_t *dev_info,
                                             profile_id_t profile_id,
                                             const char *where,
                                             const int line) {
  rmt_dev_profile_info_t *profile = NULL;

  if ((0 > profile_id) ||
      (profile_id >= (int)dev_info->num_pipeline_profiles)) {
    LOG_ERROR("Invalid profile(%d) requested on dev %d from %s:%d",
              profile_id,
              dev_info->dev_id,
              where,
              line);
    PIPE_MGR_DBGCHK(0);
    return NULL;
  }

  profile = dev_info->profile_info[profile_id];
  if (!profile) {
    LOG_ERROR(
        "Unknown profile(%d) requested from %s:%d", profile_id, where, line);
  }
  return profile;
}

pipe_status_t pipe_mgr_get_pipe_bmp_for_profile(const rmt_dev_info_t *dev_info,
                                                profile_id_t profile_id,
                                                pipe_bitmap_t *pipe_bmp_p,
                                                const char *where,
                                                const int line) {
  rmt_dev_profile_info_t *profile = NULL;

  if (profile_id >= (int)dev_info->num_pipeline_profiles) {
    LOG_ERROR("Invalid profile(%d) requested on dev %d from %s:%d",
              profile_id,
              dev_info->dev_id,
              where,
              line);
    return PIPE_INVALID_ARG;
  }

  profile = dev_info->profile_info[profile_id];
  if (!profile) {
    LOG_ERROR(
        "Unknown profile(%d) requested from %s:%d", profile_id, where, line);
    return PIPE_OBJ_NOT_FOUND;
  }
  PIPE_BITMAP_INIT(pipe_bmp_p, PIPE_BMP_SIZE);
  PIPE_BITMAP_ASSIGN(pipe_bmp_p, &profile->pipe_bmp);

  return PIPE_SUCCESS;
}

pipe_status_t pipe_mgr_pipe_to_profile(const rmt_dev_info_t *dev_info,
                                       bf_dev_pipe_t pipe_id,
                                       profile_id_t *profile_id,
                                       const char *where,
                                       const int line) {
  rmt_dev_profile_info_t *profile = NULL;
  uint32_t idx = 0;

  if (!dev_info) {
    return PIPE_INVALID_ARG;
  }

  if (pipe_id == BF_DEV_PIPE_ALL) {
    pipe_id = 0; /* Use lowest pipe */
  }

  if (dev_info->num_pipeline_profiles == 0) {
    return PIPE_OBJ_NOT_FOUND;
  }

  for (idx = 0; idx < dev_info->num_pipeline_profiles; idx++) {
    profile = dev_info->profile_info[idx];
    if (!profile) {
      continue;
    }
    if (PIPE_BITMAP_GET(&profile->pipe_bmp, pipe_id)) {
      *profile_id = profile->profile_id;
      return PIPE_SUCCESS;
    }
  }
  LOG_WARN(
      "Unknown pipe (%d) profile requested from %s:%d", pipe_id, where, line);
  return PIPE_OBJ_NOT_FOUND;
}

profile_id_t pipe_mgr_get_tbl_profile(bf_dev_id_t dev_id,
                                      pipe_mat_tbl_hdl_t h,
                                      const char *where,
                                      const int line) {
  rmt_dev_info_t *dev_info = NULL;
  pipe_mat_tbl_info_t *tbl_info = NULL;
  unsigned i;
  uint32_t p = 0;

  if (PIPE_HDL_TYPE_MAT_TBL != PIPE_GET_HDL_TYPE(h)) {
    LOG_ERROR("Invalid MAT handle %#x at %s:%d", h, where, line);
    return -1;
  }
  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return -1;
  }

  for (p = 0; p < dev_info->num_pipeline_profiles; p++) {
    for (tbl_info = dev_info->profile_info[p]->tbl_info_list.mat_tbl_list,
        i = 0;
         i < dev_info->profile_info[p]->tbl_info_list.num_mat_tbls;
         ++tbl_info, ++i) {
      if (tbl_info->handle == h) {
        return dev_info->profile_info[p]->profile_id;
      }
    }
  }

  LOG_ERROR("Unknown table 0x%x (dev %d) profile requested from %s:%d",
            h,
            dev_id,
            where,
            line);

  return -1;
}

int pipe_mgr_tbl_hdl_set_pipe(bf_dev_id_t dev_id,
                              profile_id_t profile_id,
                              pipe_tbl_hdl_t handle,
                              pipe_tbl_hdl_t *ret_handle) {
  rmt_dev_info_t *dev_info = NULL;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return -1;
  }

  if (profile_id >= (int)dev_info->num_pipeline_profiles) {
    LOG_ERROR("Invalid profile(%d) requested on dev %d",
              profile_id,
              dev_info->dev_id);
    PIPE_MGR_DBGCHK(profile_id < (int)dev_info->num_pipeline_profiles);
    return -1;
  }

  /* Change handle only for programs that use bf-rt */
  if (dev_info->profile_info[profile_id]->uses_bfrt) {
    *ret_handle = PIPE_SET_HDL_PIPE(
        handle, dev_info->profile_info[profile_id]->lowest_pipe);
  } else {
    *ret_handle = handle;
  }

  return 0;
}

/* Get the pipe-mask that needs to be Or'ed to the table handle */
bf_status_t pipe_mgr_tbl_hdl_pipe_mask_get(bf_dev_id_t dev_id,
                                           const char *prog_name,
                                           const char *pipeline_name,
                                           uint32_t *pipe_mask) {
  rmt_dev_info_t *dev_info = NULL;
  profile_id_t profile_id = 0;

  *pipe_mask = 0;
  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return BF_INVALID_ARG;
  }

  for (profile_id = 0; profile_id < (int)dev_info->num_pipeline_profiles;
       profile_id++) {
    if ((strncmp(dev_info->profile_info[profile_id]->prog_name,
                 prog_name,
                 strlen(prog_name)) == 0) &&
        (strncmp(dev_info->profile_info[profile_id]->pipeline_name,
                 pipeline_name,
                 strlen(pipeline_name)) == 0)) {
      if (dev_info->profile_info[profile_id]->uses_bfrt) {
        *pipe_mask = PIPE_SET_HDL_PIPE(
            *pipe_mask, dev_info->profile_info[profile_id]->lowest_pipe);
      }
      return BF_SUCCESS;
    }
  }

  /* Invalid Program name specified */
  return BF_INVALID_ARG;
}

/** \brief pipe_mgr_get_phy_addr_for_vpn_addr
  *         Convert a virtual address to an array of physical addresses.
  *
  *
  * NOTE: The virt_addr passed should not have any huffman bits.
  *
  * \param in dev_tgt The device target
  * \param in tbl_hdl The table handle (match, action, selection etc)
  * \param in stage_id The stage id
  * \param in check_stage_table_handle Match the stage table handle
  * \param in stage_table_handle The stage table handle
  * \param in rmt_tbl_type The table type
  * \param in virt_addr The virtual address (without any huffman bits)
  * \param out phy_addr Array to hold the physical addresses.
                         The array should be of size
  RMT_MAX_MEM_UNITS_PER_TBL_WORD_BLK
  * \param out addr_count The count of valid addresses in the phy_addr array
  * \param out subword_pos The position of the subword
  *
  * \return The status of the operation.
  *         PIPE_OBJ_NOT_FOUND if the virtual address is not valid
  *         PIPE_SUCCESS in case of success
  */
/* NOTE: The virt_addr passed should not have any huffman bits.
 * If a version with support for huffman bits is needed, then
 * the huffman bits needs to be stripped off while assigning
 * it to vpn_addr variable below based on the table type
 */
pipe_status_t pipe_mgr_get_phy_addr_for_vpn_addr(dev_target_t dev_tgt,
                                                 pipe_tbl_hdl_t tbl_hdl,
                                                 int stage_id,
                                                 bool check_stage_table_handle,
                                                 uint8_t stage_table_handle,
                                                 rmt_tbl_type_t rmt_tbl_type,
                                                 rmt_virt_addr_t virt_addr,
                                                 uint64_t *phy_addr,
                                                 int *addr_count,
                                                 int *subword_pos) {
  bf_dev_id_t dev_id = dev_tgt.device_id;
  bf_dev_pipe_t log_pipe = dev_tgt.dev_pipe_id;
  pipe_hdl_type_t tbl_type = PIPE_GET_HDL_TYPE(tbl_hdl);
  int num_rmt_info = 0;
  rmt_tbl_info_t *rmt_infos = NULL;
  uint32_t vpn_addr = virt_addr;
  bool exact_match = false;
  rmt_dev_info_t *dev_info = pipe_mgr_get_dev_info(dev_tgt.device_id);

  if (!dev_info) {
    LOG_ERROR("%s:%d Error, failed to get device info for device id %d",
              __func__,
              __LINE__,
              dev_id);
    return PIPE_OBJ_NOT_FOUND;
  }

  switch (tbl_type) {
    case PIPE_HDL_TYPE_MAT_TBL: {
      pipe_mat_tbl_info_t *mat_tbl_info = NULL;
      mat_tbl_info = pipe_mgr_get_tbl_info(dev_id, tbl_hdl, __func__, __LINE__);
      if (mat_tbl_info == NULL) {
        LOG_ERROR("%s:%d Table %d not found in RMT database for device %d",
                  __func__,
                  __LINE__,
                  tbl_hdl,
                  dev_id);
        return PIPE_OBJ_NOT_FOUND;
      }
      num_rmt_info = mat_tbl_info->num_rmt_info;
      rmt_infos = mat_tbl_info->rmt_info;
    } break;
    case PIPE_HDL_TYPE_ADT_TBL: {
      pipe_adt_tbl_info_t *adt_tbl_info = NULL;
      adt_tbl_info =
          pipe_mgr_get_adt_tbl_info(dev_id, tbl_hdl, __func__, __LINE__);
      if (adt_tbl_info == NULL) {
        LOG_ERROR("%s:%d Table %d not found in RMT database for device %d",
                  __func__,
                  __LINE__,
                  tbl_hdl,
                  dev_id);
        return PIPE_OBJ_NOT_FOUND;
      }
      num_rmt_info = adt_tbl_info->num_rmt_info;
      rmt_infos = adt_tbl_info->rmt_info;
    } break;
    default:
      /* Physical addressing scheme cannot be used for these tables */
      PIPE_MGR_DBGCHK(0);
      return PIPE_INVALID_ARG;
  }

  int i;
  int j;
  int k;

  for (i = 0; i < num_rmt_info; i++) {
    rmt_tbl_info_t *rmt_info = &rmt_infos[i];
    if ((rmt_info->type == rmt_tbl_type) && (rmt_info->stage_id == stage_id)) {
      if ((check_stage_table_handle == false) ||
          (rmt_info->handle == stage_table_handle)) {
        break;
      }
    }
  }

  if (i == num_rmt_info) {
    /* The table not found in the given stage */
    return PIPE_OBJ_NOT_FOUND;
  }
  rmt_tbl_info_t *rmt_info = &rmt_infos[i];
  uint8_t mem_units = rmt_info->pack_format.mem_units_per_tbl_word;
  uint8_t entries_per_word = rmt_info->pack_format.entries_per_tbl_word;

  int line_no_bits;
  pipe_mem_type_t pipe_mem_type;
  switch (rmt_info->mem_type) {
    case RMT_MEM_SRAM:

      line_no_bits = log2_uint32_ceil(pipe_mgr_get_sram_unit_depth(dev_id));
      pipe_mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_MEM_TCAM:

      line_no_bits = log2_uint32_ceil(pipe_mgr_get_tcam_unit_depth(dev_id));
      pipe_mem_type = pipe_mem_type_tcam;
      break;
    case RMT_MEM_MAP_RAM:
      /* Not supported now */
      PIPE_MGR_DBGCHK(0);
      return PIPE_NOT_SUPPORTED;
    default:
      PIPE_MGR_DBGCHK(0);
      return PIPE_INVALID_ARG;
  }
  int subword = vpn_addr & ((1 << log2_uint32_ceil(entries_per_word)) - 1);
  vpn_addr >>= log2_uint32_ceil(entries_per_word);
  int vpn = vpn_addr >> line_no_bits;
  int line_no = vpn_addr & ((1 << line_no_bits) - 1);
  int num_vpns = 0;

  if ((tbl_type == PIPE_HDL_TYPE_MAT_TBL) &&
      ((rmt_tbl_type == RMT_TBL_TYPE_HASH_MATCH) ||
       (rmt_tbl_type == RMT_TBL_TYPE_ATCAM_MATCH)) &&
      (rmt_info->mem_type == RMT_MEM_SRAM)) {
    /* For match tables housed in SRAM, multiple entries can be packed
     * and each sub-entry has a unique VPN. In all other cases, there
     * is only VPN for a wide-word block of memories.
     */
    num_vpns = entries_per_word;
    vpn = virt_addr >> line_no_bits;
    line_no = virt_addr & ((1 << line_no_bits) - 1);
    exact_match = true;










  } else {
    num_vpns = 1;
  }

  for (j = 0; j < rmt_info->num_tbl_banks; j++) {
    rmt_tbl_bank_map_t *bank_map = &rmt_info->bank_map[j];
    for (i = 0; i < bank_map->num_tbl_word_blks; i++) {
      rmt_tbl_word_blk_t *tbl_word_blk = &bank_map->tbl_word_blk[i];
      for (k = 0; k < num_vpns; k++) {
        if (tbl_word_blk->vpn_id[k] == vpn) {
          if (exact_match) {
            subword = k;
          }
          break;
        }
      }
      if (k != num_vpns) {
        break;
      }
    }
    if (i != bank_map->num_tbl_word_blks) {
      break;
    }
  }

  if (j == rmt_info->num_tbl_banks) {
    /* This vpn does not exist on this stage */
    return PIPE_OBJ_NOT_FOUND;
  }

  bf_dev_pipe_t phy_pipe_id = log_pipe;
  pipe_mgr_map_pipe_id_log_to_phy(dev_info, log_pipe, &phy_pipe_id);
  rmt_tbl_word_blk_t *tbl_word_blk = &rmt_info->bank_map[j].tbl_word_blk[i];
  for (i = 0; i < mem_units; i++) {
    /* Create a phy addr for each of these */
    phy_addr[i] = dev_info->dev_cfg.get_full_phy_addr(rmt_info->direction,
                                                      phy_pipe_id,
                                                      stage_id,
                                                      tbl_word_blk->mem_id[i],
                                                      line_no,
                                                      pipe_mem_type);
  }
  *addr_count = mem_units;
  *subword_pos = subword;
  return PIPE_SUCCESS;
}

/* Map logical pipe id to phy pipe-id */
inline pipe_status_t pipe_mgr_map_pipe_id_log_to_phy(
    const rmt_dev_info_t *dev_info,
    bf_dev_pipe_t pipe_id,
    bf_dev_pipe_t *phy_pipe_id) {
  /* Init to invalid value */
  *phy_pipe_id = 0xff;

  if (!dev_info) {
    LOG_ERROR("%s: Invalid input parameter dev_info", __func__);
    return PIPE_INVALID_ARG;
  }

  if (pipe_id >= dev_info->dev_cfg.num_pipelines) {
    PIPE_MGR_DBGCHK(pipe_id < dev_info->dev_cfg.num_pipelines);
    return PIPE_INVALID_ARG;
  }
  if (pipe_id >= dev_info->num_active_pipes) {
    LOG_ERROR("%s: Invalid pipe id requested on dev %d (%d, %d)",
              __func__,
              dev_info->dev_id,
              pipe_id,
              dev_info->num_active_pipes);
    PIPE_MGR_DBGCHK(pipe_id < dev_info->num_active_pipes);
    return PIPE_INVALID_ARG;
  }
  if (0xff == dev_info->pipe_log_phy_map[pipe_id]) {
    LOG_ERROR("%s: Unknown pipe id requested on dev %d (%d, %d)",
              __func__,
              dev_info->dev_id,
              pipe_id,
              dev_info->num_active_pipes);
    PIPE_MGR_DBGCHK(dev_info->pipe_log_phy_map[pipe_id] != 0xff);
    return PIPE_INVALID_ARG;
  }
  *phy_pipe_id = dev_info->pipe_log_phy_map[pipe_id];

  return PIPE_SUCCESS;
}

/* Map phy pipe id to log pipe-id */
inline pipe_status_t pipe_mgr_map_phy_pipe_id_to_log_pipe_id(
    bf_dev_id_t dev_id, bf_dev_pipe_t pipe_id, bf_dev_pipe_t *log_pipe_id) {
  rmt_dev_info_t *dev_info = NULL;
  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(0);
    return PIPE_INVALID_ARG;
  }
  return pipe_mgr_map_phy_pipe_id_to_log_pipe_id_optimized(
      dev_info, pipe_id, log_pipe_id);
}

inline pipe_status_t pipe_mgr_map_phy_pipe_id_to_log_pipe_id_optimized(
    const rmt_dev_info_t *dev_info,
    bf_dev_pipe_t pipe_id,
    bf_dev_pipe_t *log_pipe_id) {
  /* Init to invalid value */
  *log_pipe_id = 0xff;

  if (pipe_id >= dev_info->dev_cfg.num_pipelines) {
    return PIPE_INVALID_ARG;
  }
  if (!(dev_info->pipe_phy_log_map[pipe_id] < dev_info->num_active_pipes)) {
    // PIPE_MGR_DBGCHK(dev_info->pipe_phy_log_map[pipe_id] <
    // dev_info->num_active_pipes);
    return PIPE_INVALID_ARG;
  }
  *log_pipe_id = dev_info->pipe_phy_log_map[pipe_id];

  return PIPE_SUCCESS;
}

/* Get number of active pipes on a device */
inline uint32_t pipe_mgr_get_num_active_pipes(bf_dev_id_t dev_id) {
  const rmt_dev_info_t *dev_info = NULL;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return 0;
  }
  return dev_info->num_active_pipes;
}

/* Get number of active subdevices on a device */
inline uint32_t pipe_mgr_get_num_active_subdevices(bf_dev_id_t dev_id) {
  rmt_dev_info_t *dev_info = NULL;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return 1;
  }
  return dev_info->num_active_subdevices;
}

/* Get the pipe memory type from RMT table type */
pipe_status_t pipe_mgr_rmt_tbl_type_to_mem_type(rmt_tbl_type_t type,
                                                pipe_mem_type_t *mem_type) {
  switch (type) {
    case RMT_TBL_TYPE_DIRECT_MATCH:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_HASH_MATCH:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_TERN_MATCH:
      *mem_type = pipe_mem_type_tcam;
      break;
    case RMT_TBL_TYPE_TERN_INDIR:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_ACTION_DATA:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_STATS:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_METER:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_STATEFUL_MEM:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_IDLE_TMO:
      *mem_type = pipe_mem_type_map_ram;
      break;
    case RMT_TBL_TYPE_SEL_PORT_VEC:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_PHASE0_MATCH:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_ATCAM_MATCH:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    case RMT_TBL_TYPE_PROXY_HASH:
      *mem_type = pipe_mem_type_unit_ram;
      break;
    default:
      return PIPE_INVALID_ARG;
  }
  return PIPE_SUCCESS;
}

bool pipe_mgr_mat_tbl_has_idle(bf_dev_id_t dev_id, pipe_mat_tbl_hdl_t tbl_hdl) {
  const pipe_mat_tbl_info_t *mat_tbl_info = NULL;
  const rmt_tbl_info_t *rmt_info = NULL;

  mat_tbl_info = pipe_mgr_get_tbl_info(dev_id, tbl_hdl, __func__, __LINE__);
  if (mat_tbl_info == NULL) {
    LOG_ERROR("%s:%d Table %d not found in RMT database for device %d",
              __func__,
              __LINE__,
              tbl_hdl,
              dev_id);
    return false;
  }

  /* Don't build an idle table for the underlying atcam table of alpm */
  if (mat_tbl_info->alpm_info &&
      mat_tbl_info->handle == mat_tbl_info->alpm_info->atcam_handle) {
    return false;
  }

  uint32_t i = 0;
  for (i = 0; i < mat_tbl_info->num_rmt_info; i++) {
    rmt_info = &mat_tbl_info->rmt_info[i];
    if (rmt_info->type == RMT_TBL_TYPE_IDLE_TMO) {
      return true;
    }
  }

  return false;
}

bf_dev_id_t pipe_mgr_transform_dev_id_for_hitless_ha(bf_dev_id_t device_id) {
  unsigned long key = 0;
  struct pipe_mgr_dev_ctx *dev_ctx = NULL;
  bf_map_sts_t map_sts = BF_MAP_OK;
  map_sts =
      bf_map_get_first(&get_pipe_mgr_ctx()->dev_ctx, &key, (void **)&dev_ctx);
  while (map_sts == BF_MAP_OK) {
    if (pipe_mgr_is_device_virtual(key)) {
      return key;
    }
    map_sts =
        bf_map_get_next(&get_pipe_mgr_ctx()->dev_ctx, &key, (void **)&dev_ctx);
  }
  return device_id;
}

bool pipe_mgr_mat_tbl_is_hash_action(bf_dev_id_t dev_id,
                                     pipe_mat_tbl_hdl_t tbl_hdl) {
  const pipe_mat_tbl_info_t *mat_tbl_info = NULL;
  uint32_t tbl_idx = 0;

  mat_tbl_info = pipe_mgr_get_tbl_info(dev_id, tbl_hdl, __func__, __LINE__);
  if (mat_tbl_info == NULL) {
    LOG_ERROR("%s:%d Table %d not found in RMT database for device %d",
              __func__,
              __LINE__,
              tbl_hdl,
              dev_id);
    return false;
  }

  if (pipe_mgr_sm_tbl_owner(dev_id, tbl_hdl) != PIPE_MGR_TBL_OWNER_EXM) {
    return false;
  }

  for (tbl_idx = 0; tbl_idx < mat_tbl_info->num_rmt_info; tbl_idx++) {
    if (mat_tbl_info->rmt_info[tbl_idx].type == RMT_TBL_TYPE_HASH_ACTION) {
      return true;
    }
  }

  return false;
}

void pipe_mgr_port_stuck_detect_timer_cb(bf_sys_timer_t *timer, void *data) {
  bf_dev_id_t dev_id = (bf_dev_id_t)(intptr_t)data;
  if (dev_id >= PIPE_MGR_NUM_DEVICES) {
    return;
  }

  (void)timer;

  rmt_dev_info_t *dev_info;
  rmt_port_info_t *port_info;

  if (!(dev_info = pipe_mgr_get_dev_info(dev_id))) {
    PIPE_MGR_DBGCHK(dev_info);
    return;
  }

  pipe_sess_hdl_t sess_hdl = pipe_mgr_ctx->int_ses_hndl;
  pipe_status_t sts = pipe_mgr_api_enter(sess_hdl);
  if (PIPE_SUCCESS != sts) {
    LOG_ERROR("%s: Unable to take internal session lock on dev %d, sts %s",
              __func__,
              dev_id,
              pipe_str_err(sts));
    return;
  }
  for (port_info = dev_info->port_list; port_info;
       port_info = port_info->next) {
    // For 100G ports, detect the port stuck condition
    if (port_info->speed == BF_SPEED_100G) {
      // Read the port counter
      uint64_t port_ctr = 0;
      sts = pipe_mgr_parde_port_ebuf_counter_get(
          sess_hdl, dev_info, port_info->port_id, &port_ctr);
      if (sts != PIPE_SUCCESS) {
        LOG_ERROR(
            "%s: Parde failed to read the packet counter for port %d on dev "
            "%d, sts %s",
            __func__,
            port_info->port_id,
            dev_id,
            pipe_str_err(sts));
        pipe_mgr_api_exit(sess_hdl);
        return;
      }

      // Read the port bypass counter
      uint64_t port_bypass_ctr = 0;
      sts = pipe_mgr_parde_port_ebuf_bypass_counter_get(
          sess_hdl, dev_info, port_info->port_id, &port_bypass_ctr);
      if (sts != PIPE_SUCCESS) {
        LOG_ERROR(
            "%s: Parde failed to read the packet bypass counter for port %d on "
            "dev %d, sts %s",
            __func__,
            port_info->port_id,
            dev_id,
            pipe_str_err(sts));
        pipe_mgr_api_exit(sess_hdl);
        return;
      }

      // Read the credits value
      uint32_t credits = 0;
      bf_status_t bf_sts;
      bf_sts =
          bf_tm_port_credits_get_safe(dev_id, port_info->port_id, &credits);
      if (bf_sts != BF_SUCCESS) {
        LOG_ERROR(
            "%s: Parde failed to read the credits value for port %d on dev %d",
            __func__,
            port_info->port_id,
            dev_id);
        PIPE_MGR_DBGCHK(0);
        pipe_mgr_api_exit(sess_hdl);
        return;
      }

      // Detect if the port is stuck. If neither the packet counter nor
      // the egress bypass counter is being incremented and the number of
      // credits left for a port is less than 3 means that the traffic on
      // that particular port is stopped (the port is stuck).
      if (port_info->count == port_ctr &&
          port_info->bypass_count == port_bypass_ctr && credits < 3) {
        LOG_ERROR("Error: Port %d is stuck", port_info->port_id);

        // Call the callback function if a callback has been registered
        bf_notify_port_stuck_events(dev_id, port_info->port_id);
      }

      // Update the state
      port_info->count = port_ctr;
      port_info->bypass_count = port_bypass_ctr;
    }
  }
  pipe_mgr_api_exit(sess_hdl);
  return;
}

pipe_status_t pipe_mgr_port_stuck_detect_timer_init(bf_dev_id_t dev,
                                                    uint32_t timer_msec) {
  pipe_status_t status = PIPE_SUCCESS;
  bf_sys_timer_status_t ret;
  bool is_sw_model = true;
  bf_status_t sts = bf_drv_device_type_get(dev, &is_sw_model);
  rmt_dev_info_t *dev_info = pipe_mgr_get_dev_info(dev);

  if (!dev_info) {
    LOG_ERROR("Unable to get dev_info in port stuck detect timer init dev %d",
              dev);
    PIPE_MGR_DBGCHK(dev_info);
    return PIPE_INVALID_ARG;
  }

  if (dev_info->dev_family != BF_DEV_FAMILY_TOFINO) {
    /* This port stuck detect thread is only valid for TOF1.
     * Return success for all other platforms. */
    return PIPE_SUCCESS;
  }

  if (BF_SUCCESS == sts && !is_sw_model && dev_info &&
      !pipe_mgr_is_p4_skipped(dev_info)) {
    /* If there is already a timer running, stop it. */
    pipe_mgr_port_stuck_detect_timer_cleanup(dev);

    /* Configure a new timer */
    if (!port_stuck_detect_timers[dev].timer) {
      ret = bf_sys_timer_create(&port_stuck_detect_timers[dev],
                                timer_msec,
                                timer_msec,
                                pipe_mgr_port_stuck_detect_timer_cb,
                                (void *)(intptr_t)dev);
      if (BF_SYS_TIMER_OK != ret) {
        LOG_ERROR("Unable to create port stuck detection timer for dev %d",
                  dev);
        PIPE_MGR_DBGCHK(ret == BF_SYS_TIMER_OK);
        status = PIPE_NO_SYS_RESOURCES;
      }
    }

    ret = bf_sys_timer_start(&port_stuck_detect_timers[dev]);
    if (BF_SYS_TIMER_OK != ret) {
      LOG_ERROR("Unable to start port stuck detection timer for dev %d", dev);
      PIPE_MGR_DBGCHK(ret == BF_SYS_TIMER_OK);
      status = PIPE_NO_SYS_RESOURCES;
    }
  }
  return status;
}

pipe_status_t pipe_mgr_port_stuck_detect_timer_cleanup(bf_dev_id_t dev) {
  pipe_status_t status = PIPE_SUCCESS;

  rmt_dev_info_t *dev_info = pipe_mgr_get_dev_info(dev);

  if (!dev_info) {
    LOG_ERROR(
        "Unable to get dev_info in port stuck detect timer cleanup dev %d",
        dev);
    PIPE_MGR_DBGCHK(dev_info);
    return PIPE_INVALID_ARG;
  }

  if (dev_info->dev_family != BF_DEV_FAMILY_TOFINO) {
    /* This port stuck detect thread is only valid for TOF1.
     * Return success for all other platforms. */
    return PIPE_SUCCESS;
  }

  if (port_stuck_detect_timers[dev].timer) {
    bf_sys_timer_status_t ret;
    /* Stop the timer. */
    ret = bf_sys_timer_stop(&(port_stuck_detect_timers[dev]));
    if (BF_SYS_TIMER_OK != ret) {
      PIPE_MGR_DBGCHK(ret == BF_SYS_TIMER_OK);
      status = PIPE_UNEXPECTED;
    }

    /* Delete the timer. */
    ret = bf_sys_timer_del(&(port_stuck_detect_timers[dev]));
    if (BF_SYS_TIMER_OK != ret) {
      PIPE_MGR_DBGCHK(ret == BF_SYS_TIMER_OK);
      status = PIPE_UNEXPECTED;
    }
    port_stuck_detect_timers[dev].timer = NULL;
  }
  return status;
}
