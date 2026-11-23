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

#include <pd/pd.h>
#include <pipe_mgr/pipe_mgr_intf.h>
#include <pipe_mgr/pipe_mgr_mirror_intf.h>
#include <dvm/bf_drv_intf.h>

#include <tofino/pdfixed/pd_ms.h>

#include <time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
//:: if gen_perf_test_pd == 1:
#ifdef PERF_TEST_PROFILING
#include "gperftools/profiler.h"
#endif
//:: #endif
#include <target-sys/bf_sal/bf_sys_assert.h>
#include <target-sys/bf_sal/bf_sys_sem.h>
#include <target-sys/bf_sal/bf_sys_mem.h>

#define PD_DEBUG 1

#define PIPE_MGR_SUCCESS 0
#define PIPE_MATCH_ACTION_MAX_SIZE 128

#define BYTE_ROUND_UP(x) ((x + 7) >> 3)

bf_drv_client_handle_t bf_drv_hdl;

static void build_pd_res_spec(const pipe_action_spec_t *action_spec,
                              pd_res_spec_t **res_spec,
                              int *resource_cnt) {
  if (action_spec->resource_count) {
    *res_spec = bf_sys_calloc(action_spec->resource_count,
                              sizeof(pd_res_spec_t));
    unsigned i = 0;
    int idx = 0;
    for (i = 0; i < action_spec->resource_count; i++) {
      if (action_spec->resources[i].tag == PIPE_RES_ACTION_TAG_ATTACHED) {
        (*res_spec)[idx].tbl_hdl = action_spec->resources[i].tbl_hdl;
        (*res_spec)[idx].tbl_idx = action_spec->resources[i].tbl_idx;
        idx++;
      }
    }
    *resource_cnt = idx;
  } else {
    *resource_cnt = 0;
  }
  return;
}

//:: if gen_perf_test_pd == 1:
typedef enum bulk_operation_type_ {
    BULK_OPERATION_INVALID=0,
    BULK_OPERATION_ADD,
    BULK_OPERATION_MODIFY,
    BULK_OPERATION_DELETE,
} bulk_operation_type_e;


//:: def build_direct_parameter_spec_dictionary(direct_resources):
//::   specs = {}
//::   for res_name, res_type, _ in direct_resources:
//::     if res_type == "bytes_meter":
//::       specs["bytes_meter"] = [res_name + "_spec"]
//::     elif res_type == "packets_meter":
//::       specs["packets_meter"] = [res_name + "_spec"]
//::     elif res_type == "register":
//::       specs["register"] = [res_name + "_spec"]
//::     elif res_type == "lpf":
//::       specs["lpf"] = [res_name + "_spec"]
//::     elif res_type == "wred":
//::       specs["wred"] = [res_name + "_spec"]
//::     #endif
//::   #endfor
//::   return specs
//:: #enddef

//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     if action_table_hdl: continue
//::     match_type = t_info["match_type"]
//::     specs = build_direct_parameter_spec_dictionary(table_direct_resources[table])
//::     has_match_spec = len(t_info["match_fields"]) > 0
//::     if has_match_spec == 0: continue
typedef p4_pd_status_t (*${p4_pd_prefix}${table}_add_fn_ptr) (
                                               p4_pd_sess_hdl_t sess_hdl,
                                               p4_pd_dev_target_t dev_tgt,
                                               const ${p4_pd_prefix}${table}_match_spec_t* const match_spec,
                                               const void* const action_spec,
//::     if match_type == "ternary" or match_type == "range":
                                               int priority,
//::     #endif
//::     if "bytes_meter" in specs:
                                               ${specs["bytes_meter"]},
//::     elif "packet_meter" in specs:
                                               ${specs["packets_meter"]},
//::     #endif
//::     if "stful_spec" in specs:
                                               ${specs["register"]},
//::     #endif
//::     if "lpf_spec" in specs:
                                               ${specs["lpf"]},
//::     elif "wred_spec" in specs:
                                               ${specs["wred"]},
//::     #endif
//::     if t_info["timeout"]:
                                               uint8_t ttl,
//::     #endif
                                               p4_pd_entry_hdl_t* entry_hdl);

//::     str = ["p4_pd_sess_hdl_t sess_hdl"]
//::     str += ["uint8_t  dev_id"]
//::     str += ["p4_pd_entry_hdl_t entry_hdl"]
//::     str += ["const void * const action_spec"]
//::     if "bytes_meter" in specs:
//::       str += [specs["bytes_meter" + " bytes_meter_spec"]]
//::     elif "packet_meter" in specs:
//::       str += [specs["packets_meter"] + " packet_meter_spec"]
//::     #endif
//::     if "stful_spec" in specs:
//::       str += [specs["register"] + " stful_spec"]
//::     #endif
//::     if "lpf_spec" in specs:
//::       str += [specs["lpf"] + " lpf_spec"]
//::     elif "wred_spec" in specs:
//::       str += [specs["wred"] + " wred_spec"]
//::     #endif
//::     if t_info["timeout"]:
//::       str += ["uint8_t ttl"]
//::     #endif
//::     param_str = ",\n".join(str)

typedef p4_pd_status_t (*${p4_pd_prefix}${table}_modify_fn_ptr) (${param_str});

typedef p4_pd_status_t (*${p4_pd_prefix}${table}_delete_fn_ptr) (p4_pd_sess_hdl_t sess_hdl,
                                                                bf_dev_id_t dev_id,
                                                                p4_pd_entry_hdl_t entry_hdl);

typedef struct ${p4_pd_prefix}${table}_bulk_op_ {
    bulk_operation_type_e operation;
    void* match_spec;
    void* action_spec;
    ${p4_pd_prefix}${table}_add_fn_ptr add_fn_ptr;
    ${p4_pd_prefix}${table}_modify_fn_ptr modify_fn_ptr;
    ${p4_pd_prefix}${table}_delete_fn_ptr delete_fn_ptr;
    int priority;
    uint32_t ttl;
} ${p4_pd_prefix}${table}_bulk_op_t;

typedef struct ${p4_pd_prefix}${table}_bulk_ {
    p4_pd_sess_hdl_t  sess_hdl;
    uint8_t           device_id;
    bf_dev_pipe_t     dev_pipe_id;
    uint32_t num_ops;
    bool action_spec_present;
    bool byte_meter_spec_present;
    bool pkt_meter_spec_present;
    bool lpf_spec_present;
    bool stful_spec_present;
    bool idle_present;
    bool exm;
    ${p4_pd_prefix}${table}_bulk_op_t* operations;
} ${p4_pd_prefix}${table}_bulk_t;

${p4_pd_prefix}${table}_bulk_t *${p4_pd_prefix}${table}_bulk_p;

//::   #endfor
//:: #endif

//:: def get_num_match_bits(t_info, field_info):
//::   num_bits = 0
//::   match_fields = t_info["match_fields"]
//::   for field, type in match_fields:
//::     f_info = field_info[field]
//::     num_bits += f_info["bit_width"]
//::   #endfor
//::   return num_bits
//:: #enddef
//::
//:: def get_num_match_bytes(t_info, field_info):
//::   num_bytes = 0
//::   match_fields = t_info["match_fields"]
//::   for field, type in match_fields:
//::     f_info = field_info[field]
//::     num_bytes += (f_info["bit_width"] + 7) // 8
//::   #endfor
//::   return num_bytes
//:: #enddef
//::
//:: def get_num_action_bits(param_bit_widths):
//::   return sum(param_bit_widths)
//:: #enddef
//::
//:: def get_num_action_bytes(param_bit_widths):
//::   return sum([(w + 7) // 8 for w in param_bit_widths])
//:: #enddef
//::
//:: def get_spec_width(byte_widths):
//::   result = 0
//::   for byte_width in byte_widths:
//::     if byte_width == 3:
//::       result += 4
//::     else:
//::       result += byte_width
//::     #endif
//::   #endfor
//::   return result
//:: #enddef
//::
//:: # match_fields is list of tuples (name, type)
//:: def gen_match_params(match_fields, field_info):
//::   params = []
//::   for field, type in match_fields:
//::     if type[:6] == "valid_":
//::       params += [(field + "_valid", 1)]
//::       if type[6:] == "ternary":
//::         params += [(field + "_valid_mask", 1)]
//::       #endif
//::       continue
//::     #endif
//::     f_info = field_info[field]
//::     bytes_needed = (f_info["bit_width"] + 7 ) // 8
//::     if type != "range":
//::        params += [(field, bytes_needed)]
//::     else:
//::        params += [(field + "_start", bytes_needed)]
//::     #endif
//::     if type == "lpm": params += [(field + "_prefix_length", 2)]
//::     if type == "ternary": params += [(field + "_mask", bytes_needed)]
//::     if type == "range" : params += [(field + "_end", bytes_needed)]
//::   #endfor
//::   return params
//:: #enddef
//::
//:: def gen_action_params(names, bit_widths, _get_type = get_type):
//::   params = []
//::   for name, width in zip(names, bit_widths):
//::     name = "action_" + name
//::     width = (width + 7) // 8
//::     params += [(name, width)]
//::   #endfor
//::   return params
//:: #enddef
//::
//::

//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   act_prof = t_info["action_profile"]
//::   if act_prof is None: continue
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)
static p4_pd_ms_table_state_t *ms_${act_prof}_state;
//:: #endfor

static bf_status_t ${p4_pd_prefix}dev_remove(bf_dev_id_t dev_id) {

//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   act_prof = t_info["action_profile"]
//::   if act_prof is None: continue
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)
  p4_pd_ms_destroy_state_for_dev(ms_${act_prof}_state, dev_id); 
//:: #endfor
  p4_pd_ms_destroy_txn_state_for_dev(dev_id);
  return BF_SUCCESS;
}

static bf_status_t ${p4_pd_prefix}dev_log(bf_dev_id_t dev_id, cJSON *pd_node) {
  cJSON *profs, *prof;
  cJSON_AddStringToObject(pd_node, "name", "${p4_name}");
  cJSON_AddItemToObject(pd_node, "act_profs", profs = cJSON_CreateArray());
//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   act_prof = t_info["action_profile"]
//::   if act_prof is None: continue
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)

  cJSON_AddItemToArray(profs, prof = cJSON_CreateObject());
  cJSON_AddStringToObject(prof, "name", "${act_prof}");
  p4_pd_ms_log_state(dev_id, ms_${act_prof}_state, prof);
//:: #endfor

  return BF_SUCCESS;
}

static bf_status_t ${p4_pd_prefix}dev_restore(bf_dev_id_t dev_id, cJSON *pd_node) {
  cJSON *profs, *prof;

  profs = cJSON_GetObjectItem(pd_node, "act_profs");
  for (prof = profs->child; prof; prof = prof->next) {
//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   act_prof = t_info["action_profile"]
//::   if act_prof is None: continue
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)

    if (strcmp(cJSON_GetObjectItem(prof, "name")->valuestring, "${act_prof}") == 0) {
      p4_pd_ms_restore_state(dev_id, ms_${act_prof}_state, prof);
    }
//:: #endfor
  }

  return BF_SUCCESS;
}

void ${p4_pd_prefix}init(void) {
  p4_pd_ms_init();

//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   act_prof = t_info["action_profile"]
//::   if act_prof is None: continue
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)
  ms_${act_prof}_state = p4_pd_ms_init_state();
//:: #endfor
  bf_status_t r = bf_drv_register("${p4_pd_prefix}pd", &bf_drv_hdl);
  assert(r == BF_SUCCESS);
  bf_drv_client_callbacks_t callbacks = {0};
  callbacks.device_del = ${p4_pd_prefix}dev_remove;
  callbacks.device_log = ${p4_pd_prefix}dev_log;
  callbacks.device_restore = ${p4_pd_prefix}dev_restore;
  bf_drv_client_register_callbacks(bf_drv_hdl, &callbacks, BF_CLIENT_PRIO_2);
}

void ${p4_pd_prefix}device_state_cleanup(uint8_t device_id) {
//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   act_prof = t_info["action_profile"]
//::   if act_prof is None: continue
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)
  p4_pd_ms_destroy_state_for_dev(ms_${act_prof}_state, device_id);
//:: #endfor
}

void ${p4_pd_prefix}cleanup(void) {

  p4_pd_ms_cleanup();

  bf_drv_deregister(bf_drv_hdl);
}

//:: for table, t_info in table_info.items():
//::   if not t_info["match_fields"]: continue
static inline void build_key_${table}
(
 pipe_tbl_match_spec_t *pipe_match_spec,
 const ${p4_pd_prefix}${table}_match_spec_t * const match_spec
)
{
  uint8_t *match_bits = pipe_match_spec->match_value_bits;
  (void) match_bits;
#ifdef LITTLE_ENDIAN_CALLER
  uint16_t tmp16;
  uint32_t tmp32;
  (void) tmp16;
  (void) tmp32;
#endif

//::   for field_name, field_match_type in t_info["match_fields"]:
//::     f_info = field_info[field_name]
//::     width = (f_info["bit_width"] + 7 ) // 8
//::     bit_width = f_info["bit_width"]
//::     if "atcam_partition_index" in t_info:
//::       if field_name == t_info["atcam_partition_index"]:
  match_bits += ${width};
//::         continue
//::       #endif
//::     #endif 

//::     byte_offset = 0
//::     # only case where a byte is wasted
//::     if width == 3: byte_offset += 1
//::     if field_match_type == "range":
//::       field_suffix = "_start"
//::     elif field_match_type[:6] == "valid_":
//::       field_suffix = "_valid"
//::     else:
//::       field_suffix = ""
//::     #endif
#ifdef LITTLE_ENDIAN_CALLER
//::     if width == 2:
  tmp16 = htons(match_spec->${field_name}${field_suffix});
  memcpy(match_bits, &tmp16, ${width});
//::     elif width == 3 or width == 4:
  tmp32 = htonl(match_spec->${field_name}${field_suffix});
  memcpy(match_bits, ((char *) &tmp32) + ${byte_offset}, ${width});
//::     else:
  memcpy(match_bits, &match_spec->${field_name}${field_suffix}, ${width});
//::     #endif
#else
  memcpy(match_bits, ((char *) &match_spec->${field_name}${field_suffix}) + ${byte_offset}, ${width});
#endif
  match_bits += ${width};

//::   #endfor
}

//:: #endfor
//::
//::
//:: for table, t_info in table_info.items():
//::   if not t_info["match_fields"]: continue
//::   match_type = t_info["match_type"]
//::   if match_type != "ternary" and match_type != "lpm" and match_type != "range": continue
static inline void build_mask_${table}
(
 pipe_tbl_match_spec_t *pipe_match_spec,
 const ${p4_pd_prefix}${table}_match_spec_t * const match_spec
)
{
  uint8_t *value_bits = pipe_match_spec->match_value_bits;
  uint8_t *mask_bits = pipe_match_spec->match_mask_bits;
#ifdef LITTLE_ENDIAN_CALLER
  uint16_t tmp16;
  uint32_t tmp32;
  (void) tmp16;
  (void) tmp32;
#endif
  uint32_t i; /* for value masking */
  int bits_to_reset; /* for lpm mask */
  (void) bits_to_reset; /* compiler */

//::   for field_name, field_match_type in t_info["match_fields"]:
//::
//::     f_info = field_info[field_name]
//::     width = (f_info["bit_width"] + 7 ) // 8
//::     bit_width = f_info["bit_width"]
//::     bit_offset = width * 8 - bit_width
//::     if "atcam_partition_index" in t_info:
//::       if field_name == t_info["atcam_partition_index"]:
  memset(mask_bits, 0, ${width});
  mask_bits += ${width};
  value_bits += ${width};
//::         continue
//::       #endif
//::     #endif

//::     first_byte_mask = 0xFF >> bit_offset
//::     byte_offset = 0
//::     if width == 3: byte_offset += 1
//::
//::     if field_match_type == "ternary":
//::       field_suffix = "_mask"
//::     elif field_match_type == "range":
//::       field_suffix = "_end"
//::     elif field_match_type == "valid_ternary":
//::       field_suffix = "_valid_mask"
//::     #endif
//::
//::     if field_match_type == "exact" or field_match_type == "valid_exact":
//::       pass
//::     elif field_match_type == "ternary" or field_match_type == "range" or field_match_type == "valid_ternary":
#ifdef LITTLE_ENDIAN_CALLER
//::       if width == 2:
  tmp16 = htons(match_spec->${field_name}${field_suffix});
  memcpy(mask_bits, &tmp16, ${width});
//::       elif width == 3 or width == 4:
  tmp32 = htonl(match_spec->${field_name}${field_suffix});
  memcpy(mask_bits, ((char *) &tmp32) + ${byte_offset}, ${width});
//::       else:
  memcpy(mask_bits, &match_spec->${field_name}${field_suffix}, ${width});
//::       #endif
#else
  memcpy(mask_bits, ((char *) &match_spec->${field_name}${field_suffix}) + ${byte_offset}, ${width});
#endif
//::     elif field_match_type == "lpm":
  bits_to_reset = ${bit_width} - match_spec->${field_name}_prefix_length;
  if (bits_to_reset >= 8) {
    memset(mask_bits + ${width} - bits_to_reset / 8, 0, bits_to_reset / 8);
  }
  if (bits_to_reset % 8 != 0) {
    mask_bits[${width} - 1 - bits_to_reset / 8] = (unsigned char) 0xFF << (bits_to_reset % 8);
  }
//::     else:
//::       assert(False)
//::     #endif
//::     if field_match_type == "ternary" or field_match_type == "lpm" or field_match_type == "valid_ternary":
  for (i = 0; i < ${width}; i++) {
    value_bits[i] &= mask_bits[i];
  }
//::     #endif
//::     if bit_offset > 0:
#ifdef PD_DEBUG
  mask_bits[0] &= ${first_byte_mask};
#endif
//::     #endif
  mask_bits += ${width};
  value_bits += ${width};

//::   #endfor
}

//:: #endfor


/* Dynamic Exm Table Key Mask */

//:: for table, t_info in table_info.items():
//::   if not t_info["match_fields"]: continue
//::   if not t_info["dynamic_match_key_masks"]: continue
static inline void build_match_key_mask_spec_${table}
(
 pipe_tbl_match_spec_t *pipe_match_spec,
 ${p4_pd_prefix}${table}_match_key_mask_spec_t *match_key_mask_spec
)
{
  uint8_t *match_bits = pipe_match_spec->match_mask_bits;
  (void) match_bits;
#ifdef LITTLE_ENDIAN_CALLER
  uint16_t tmp16;
  uint32_t tmp32;
  (void) tmp16;
  (void) tmp32;
#endif

//::   for field_name, field_match_type in t_info["match_fields"]:
//::     f_info = field_info[field_name]
//::     width = (f_info["bit_width"] + 7 ) // 8
//::     bit_width = f_info["bit_width"]

//::     byte_offset = 0
//::     # only case where a byte is wasted
//::     if width == 3: byte_offset += 1
//::     if field_match_type[:6] == "valid_":
//::       name = field_name + '_valid_mask'
//::     else:
//::       name = field_name + '_mask'
//::     #endif
#ifdef LITTLE_ENDIAN_CALLER
//::     if width == 2:
  tmp16 = htons(match_key_mask_spec->${name});
  memcpy(match_bits, &tmp16, ${width});
//::     elif width == 3 or width == 4:
  tmp32 = htonl(match_key_mask_spec->${name});
  memcpy(match_bits, ((char *) &tmp32) + ${byte_offset}, ${width});
//::     else:
  memcpy(match_bits, &match_key_mask_spec->${name}, ${width});
//::     #endif
#else
  memcpy(match_bits, ((char *) &match_key_mask_spec->${name}) + ${byte_offset}, ${width});
#endif
  match_bits += ${width};

//::   #endfor
}

static inline void unbuild_match_key_mask_spec_${table}
(
 pipe_tbl_match_spec_t *pipe_match_spec,
 ${p4_pd_prefix}${table}_match_key_mask_spec_t *match_key_mask_spec
)
{
  uint8_t *match_bits = pipe_match_spec->match_mask_bits;
  (void) match_bits;
#ifdef LITTLE_ENDIAN_CALLER
  uint16_t tmp16;
  uint32_t tmp32;
  (void) tmp16;
  (void) tmp32;
#endif

//::   for field_name, field_match_type in t_info["match_fields"]:
//::     f_info = field_info[field_name]
//::     width = (f_info["bit_width"] + 7 ) // 8
//::     bit_width = f_info["bit_width"]

//::     byte_offset = 0
//::     # only case where a byte is wasted
//::     if width == 3: byte_offset += 1
//::     if field_match_type[:6] == "valid_":
//::       name = field_name + '_valid_mask'
//::     else:
//::       name = field_name + '_mask'
//::     #endif
#ifdef LITTLE_ENDIAN_CALLER
//::     if width == 2:
  memcpy(&tmp16, match_bits, ${width});
  match_key_mask_spec->${name} = ntohs(tmp16);
//::     elif width == 3 or width == 4:
  tmp32 = 0;
  memcpy(((char *) &tmp32) + ${byte_offset}, match_bits, ${width});
  match_key_mask_spec->${name} = ntohl(tmp32);
//::     else:
  memcpy(&match_key_mask_spec->${name}, match_bits, ${width});
//::     #endif
#else
  memcpy(((char *) &match_key_mask_spec->${name}) + ${byte_offset}, match_bits, ${width});
#endif
  match_bits += ${width};

//::   #endfor
}

//:: #endfor
//::


//:: for table, t_info in table_info.items():
//::   if not t_info["match_fields"]: continue
//::   match_type = t_info["match_type"]
static inline void build_match_spec_${table}
(
 pipe_tbl_match_spec_t *pipe_match_spec,
 const ${p4_pd_prefix}${table}_match_spec_t * const match_spec
)
{
//::   if "atcam_partition_index" in t_info:
  pipe_match_spec->partition_index = match_spec->${t_info["atcam_partition_index"]};
//::   else:
  pipe_match_spec->partition_index = 0;
//::   #endif

  build_key_${table}(pipe_match_spec, match_spec);

  if (pipe_match_spec->num_match_bytes > 0) {
  /* we start by setting the mask entirely to 1s */
    memset(pipe_match_spec->match_mask_bits, 0xFF, pipe_match_spec->num_match_bytes);
  }

//::   if match_type == "exact":
  pipe_match_spec->priority = 0;
  /* exact match: nothing to do with the mask */
//::   elif match_type == "ternary" or match_type == "range":
  build_mask_${table}(pipe_match_spec, match_spec);
//::   elif match_type == "lpm":
//::     for name, m_type in t_info["match_fields"]:
//::       # there can only be one lpm field
//::       if m_type == "lpm":
  /* 0 is highest priority, max possible length is lowest */
//::         f_info = field_info[name]
//::         bit_width = f_info["bit_width"]
  if (match_spec->${name}_prefix_length > ${bit_width}) {
    /* Given prefix length for this field is out of bounds */
    pipe_match_spec->priority = pipe_match_spec->num_valid_match_bits + 1;
    return;
  } else {
    pipe_match_spec->priority = ${bit_width};
    pipe_match_spec->priority -= match_spec->${name}_prefix_length;
  }
//::       #endif
//::     #endfor
  build_mask_${table}(pipe_match_spec, match_spec);
//::   else:
//::     assert(False)
//::   #endif
}

//:: #endfor
//::

//:: def build_indirect_res_dict(a_info):
//::    dict = {}
//::    for elem in a_info["indirect_resources"]:
//::        _, _, name, _ = elem
//::       # const params will not have name, skip them
//::       # if an ind res is also a normal action param, skip it
//::       if name and name not in a_info["ind_res_and_action_param"]:
//::           name = "action_" + name
//::           dict[name] = True
//::       #endif
//::    #endfor
//::    return dict
//:: #enddef

//:: def get_action_indirect_res(a_info, resource_handles, res_hdl):
//::   for t in a_info["indirect_resources"]:
//::     if resource_handles[t[3]] == res_hdl:
//::       return t
//::     #endif
//::   #endfor
//:: #enddef

//:: for action, a_info in action_info.items():
//::   indirect_res_dict = build_indirect_res_dict(a_info)
//::   if not a_info["param_names"]: continue
//::   action_params = gen_action_params(a_info["param_names"],
//::                                     a_info["param_bit_widths"])
static inline void build_action_spec_${action}
(
 pipe_action_data_spec_t *pipe_action_spec,
 const ${p4_pd_prefix}${action}_action_spec_t * const action_spec
)
{
  uint8_t *data_bits = pipe_action_spec->action_data_bits;
  (void) data_bits;

#ifdef LITTLE_ENDIAN_CALLER
  uint16_t tmp16;
  uint32_t tmp32;
  (void) tmp16;
  (void) tmp32;
#endif
//::
//::   for idx, param in enumerate(action_params):
//::
//::     name, width = param
//::     bit_width = a_info["param_bit_widths"][idx]
//::     byte_offset = 0
//::     if name not in indirect_res_dict:
//::       # only case where a byte is wasted
//::       if width == 3: byte_offset += 1
#ifdef LITTLE_ENDIAN_CALLER
//::       if width == 2:
  tmp16 = htons(action_spec->${name});
  memcpy(data_bits, &tmp16, ${width});
//::       elif width == 3 or width == 4:
  tmp32 = htonl(action_spec->${name});
  memcpy(data_bits, ((char *) &tmp32) + ${byte_offset}, ${width});
//::       else:
  memcpy(data_bits, &action_spec->${name}, ${width});
//::       #endif
#else
  memcpy(data_bits, ((char *) &action_spec->${name}) + ${byte_offset}, ${width});
#endif
//::     #endif

  data_bits += ${width};
//::   #endfor

}

static inline void build_indirect_resources_${action}
(
 pipe_action_spec_t *pipe_action_spec,
 const ${p4_pd_prefix}${action}_action_spec_t * const action_spec
)
{
//::   for a_res in a_info["indirect_resources"]:
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec->resources[pipe_action_spec->resource_count++];
    res_spec->tbl_hdl = ${resource_handles[a_res[3]]};
//::     if a_res[0] == "constant":
    res_spec->tbl_idx = ${a_res[1]};
//::     else:
    res_spec->tbl_idx = action_spec->action_${a_res[2]};
//::     #endif
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
  }
//::   #endfor

}

//:: #endfor


//:: for table, t_info in table_info.items():
//::   if not t_info["match_fields"]: continue
static inline void unbuild_key_${table}
(
 pipe_tbl_match_spec_t *pipe_match_spec,
 ${p4_pd_prefix}${table}_match_spec_t * match_spec
)
{
  uint8_t *match_bits = pipe_match_spec->match_value_bits;
  (void) match_bits;
#ifdef LITTLE_ENDIAN_CALLER
  uint16_t tmp16;
  uint32_t tmp32;
  (void) tmp16;
  (void) tmp32;
#endif

//::   for field_name, field_match_type in t_info["match_fields"]:
//::     f_info = field_info[field_name]
//::     width = (f_info["bit_width"] + 7 ) // 8
//::     bit_width = f_info["bit_width"]
//::     if "atcam_partition_index" in t_info:
//::       if field_name == t_info["atcam_partition_index"]:
  match_bits += ${width};
  match_spec->${field_name} = pipe_match_spec->partition_index;
//::         continue
//::       #endif
//::     #endif 

//::     byte_offset = 0
//::     # only case where a byte is wasted
//::     if width == 3: byte_offset += 1
//::     if field_match_type == "range":
//::       field_suffix = "_start"
//::     elif field_match_type[:6] == "valid_":
//::       field_suffix = "_valid"
//::     else:
//::       field_suffix = ""
//::     #endif
#ifdef LITTLE_ENDIAN_CALLER
//::     if width == 2:
  memcpy(&tmp16, match_bits, ${width});
  match_spec->${field_name}${field_suffix} = ntohs(tmp16);
//::     elif width == 3 or width == 4:
  tmp32 = 0;
  memcpy(((char *) &tmp32) + ${byte_offset}, match_bits, ${width});
  match_spec->${field_name}${field_suffix} = ntohl(tmp32);
//::     else:
  memcpy(&match_spec->${field_name}${field_suffix}, match_bits, ${width});
//::     #endif
#else
  memcpy(((char *) &match_spec->${field_name}${field_suffix}) + ${byte_offset}, match_bits, ${width});
#endif
  match_bits += ${width};

//::   #endfor
}

//:: #endfor
//::
//::
//:: for table, t_info in table_info.items():
//::   if not t_info["match_fields"]: continue
//::   match_type = t_info["match_type"]
//::   if match_type != "ternary" and match_type != "lpm" and match_type != "range": continue
static inline void unbuild_mask_${table}
(
 pipe_tbl_match_spec_t *pipe_match_spec,
 ${p4_pd_prefix}${table}_match_spec_t * match_spec
)
{
  uint8_t *mask_bits = pipe_match_spec->match_mask_bits;
#ifdef LITTLE_ENDIAN_CALLER
  uint16_t tmp16;
  uint32_t tmp32;
  (void) tmp16;
  (void) tmp32;
#endif
  int pref_len;
  int i;
  (void) pref_len;
  (void) i;

//::   for field_name, field_match_type in t_info["match_fields"]:
//::
//::     f_info = field_info[field_name]
//::     width = (f_info["bit_width"] + 7 ) // 8
//::     bit_width = f_info["bit_width"]
//::     bit_offset = width * 8 - bit_width

//::     first_byte_mask = 0xFF >> bit_offset
//::     byte_offset = 0
//::     if width == 3: byte_offset += 1
//::
//::     if field_match_type == "ternary":
//::       field_suffix = "_mask"
//::     elif field_match_type == "range":
//::       field_suffix = "_end"
//::     elif field_match_type == "valid_ternary":
//::       field_suffix = "_valid_mask"
//::     #endif
//::
//::     if field_match_type == "exact" or field_match_type == "valid_exact":
//::       pass
//::     elif field_match_type == "ternary" or field_match_type == "range" or field_match_type == "valid_ternary":
#ifdef LITTLE_ENDIAN_CALLER
//::       if width == 2:
  memcpy(&tmp16, mask_bits, ${width});
  match_spec->${field_name}${field_suffix} = ntohs(tmp16);
//::       elif width == 3 or width == 4:
  tmp32 = 0;
  memcpy(((char *) &tmp32) + ${byte_offset}, mask_bits, ${width});
  match_spec->${field_name}${field_suffix} = ntohl(tmp32);
//::       else:
  memcpy(&match_spec->${field_name}${field_suffix}, mask_bits, ${width});
//::       #endif
#else
  memcpy(((char *) &match_spec->${field_name}${field_suffix}) + ${byte_offset}, mask_bits, ${width});
#endif
//::     elif field_match_type == "lpm":
  pref_len = ${bit_width};
  for (i = 0; i < ${bit_width}; i++) {
    if (mask_bits[${width} - 1 - (i / 8)] & (1 << (i % 8))) {
      break;
    }
    pref_len--;
  }
  match_spec->${field_name}_prefix_length = pref_len;
//::     else:
//::       assert(False)
//::     #endif
  mask_bits += ${width};

//::   #endfor
}

//:: #endfor

//:: for action, a_info in action_info.items():
//::   indirect_res_dict = build_indirect_res_dict(a_info)
//::   if not a_info["param_names"]: continue
//::   action_params = gen_action_params(a_info["param_names"],
//::                                     a_info["param_bit_widths"])
static inline void unbuild_action_spec_${action}
(
 pipe_action_data_spec_t *pipe_action_spec,
 pd_res_spec_t *res_spec,
 int num_res,
 ${p4_pd_prefix}${action}_action_spec_t * action_spec
)
{
  uint8_t *data_bits = pipe_action_spec->action_data_bits;
  unsigned i = 0;
  (void)i;
#ifdef LITTLE_ENDIAN_CALLER
  uint16_t tmp16;
  uint32_t tmp32;
  (void) tmp16;
  (void) tmp32;
#endif
//::
//::   for idx, param in enumerate(action_params):

//::     name, width = param
//::     bit_width = a_info["param_bit_widths"][idx]
//::     byte_offset = 0
//::     if name not in indirect_res_dict:
//::       # only case where a byte is wasted
//::       if width == 3: byte_offset += 1
#ifdef LITTLE_ENDIAN_CALLER
//::       if width == 2:
  memcpy(&tmp16, data_bits, ${width});
  action_spec->${name} = ntohs(tmp16);

//::       elif width == 3 or width == 4:
  tmp32 = 0;
  memcpy(((char *) &tmp32) + ${byte_offset}, data_bits, ${width});
  action_spec->${name} = ntohl(tmp32);

//::       else:
  memcpy(&action_spec->${name}, data_bits, ${width});
//::       #endif
#else
  memcpy(((char *) &action_spec->${name}) + ${byte_offset}, data_bits, ${width});
#endif
//::     else:
//::       for elem in a_info["indirect_resources"]:
//::         res_hdl = resource_handles[elem[3]]
//::         res_name = elem[2]
//::         if elem[0] == "constant": continue
//::         if name != "action_" + res_name: continue
  if (num_res) {
    for (i = 0; i < num_res; i++) {
        if (res_spec[i].tbl_hdl == ${res_hdl}) {
          action_spec->${name} = res_spec[i].tbl_idx;
        }
    }
  } else {
    /* If the Resource spec is not passed, then reosurce indices cannot be
     * populated. Set it to an INVALID index to indicate this.
     */
    action_spec->${name} = (uint16_t) PD_INVALID_INDIRECT_RESOURCE_IDX;
  }
//::       #endfor
//::     #endif
  data_bits += ${width};
//::   #endfor
}

//:: #endfor


/* IDLE TIME CONFIG */

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if not t_info["timeout"]: continue
//::   name = p4_pd_prefix + table + "_idle_tmo_enable"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_idle_time_params_t params
)
{
  // don't know if I could just copy the struct...
  pipe_idle_time_params_t pipe_params = {0};
  pipe_params.mode = params.mode;
  pipe_params.u.notify.ttl_query_interval =
    params.params.notify.ttl_query_interval;
  pipe_params.u.notify.max_ttl = params.params.notify.max_ttl;
  pipe_params.u.notify.min_ttl = params.params.notify.min_ttl;
  pipe_params.u.notify.default_callback_choice = 0;
  pipe_params.u.notify.callback_fn = params.params.notify.callback_fn;
  pipe_params.u.notify.client_data = params.params.notify.cookie;
  p4_pd_status_t sts = pipe_mgr_idle_set_params(sess_hdl, dev_id, ${table_hdl},
      pipe_params);
  if (sts) return sts;
  return pipe_mgr_idle_tmo_set_enable(sess_hdl, dev_id, ${table_hdl}, true);
}

//::   name = p4_pd_prefix + table + "_idle_register_tmo_cb"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_idle_tmo_expiry_cb cb,
 void *cookie
)
{
  return pipe_mgr_idle_register_tmo_cb(sess_hdl, dev_id, ${table_hdl}, cb, cookie);
}

//::   name = p4_pd_prefix + table + "_idle_tmo_disable"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id
)
{
  return pipe_mgr_idle_tmo_set_enable(sess_hdl, dev_id, ${table_hdl}, false);
}

//::   name = p4_pd_prefix + table + "_idle_params_get"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_idle_time_params_t *params
)
{
  pipe_idle_time_params_t pipe_params = {0};
  p4_pd_status_t sts = pipe_mgr_idle_get_params(sess_hdl, dev_id, ${table_hdl}, &pipe_params);
  if (sts == PIPE_SUCCESS) {
    params->mode = pipe_params.mode;
    if (params->mode == PD_NOTIFY_MODE) {
      params->params.notify.ttl_query_interval =
          pipe_params.u.notify.ttl_query_interval;
      params->params.notify.max_ttl = pipe_params.u.notify.max_ttl;
      params->params.notify.min_ttl = pipe_params.u.notify.min_ttl;
      params->params.notify.default_callback_choice = 0;
      params->params.notify.callback_fn = pipe_params.u.notify.callback_fn;
      params->params.notify.cookie = pipe_params.u.notify.client_data;
    }
  }
  return sts;
}

//::   name = p4_pd_prefix + table + "_set_ttl"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 uint32_t ttl
)
{
  return pipe_mgr_mat_ent_set_idle_ttl(sess_hdl, dev_id,
				       ${table_hdl}, entry_hdl, ttl, 0, false);
}

//::   name = p4_pd_prefix + table + "_get_ttl"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 uint32_t *ttl
)
{
  return pipe_mgr_mat_ent_get_idle_ttl(sess_hdl, dev_id,
  				       ${table_hdl}, entry_hdl, ttl);
}

//::   name = p4_pd_prefix + table + "_reset_ttl"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_entry_hdl_t entry_hdl
)
{
  return pipe_mgr_mat_ent_reset_idle_ttl(sess_hdl, dev_id,
				       ${table_hdl}, entry_hdl);
}

//::   name = p4_pd_prefix + table + "_update_hit_state"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_idle_time_update_complete_cb callback_fn,
 void *cookie
)
{
  return pipe_mgr_idle_time_update_hit_state(sess_hdl, dev_id,
					     ${table_hdl}, callback_fn, cookie);
}

//::   name = p4_pd_prefix + table + "_get_hit_state"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_idle_time_hit_state_e *hit_state
)
{
  pipe_idle_time_hit_state_e pipe_hit_state;
  p4_pd_status_t status = pipe_mgr_idle_time_get_hit_state(sess_hdl, dev_id,
							   ${table_hdl}, entry_hdl,
							   &pipe_hit_state);
  *hit_state = pipe_hit_state;
  return status;
}

//:: #endfor


/* ADD ENTRIES */


//:: def p4_meter_has_pre_color(meter_name, meter_info):
//::   if meter_info[meter_name]["pre_color"]:
//::     return "true"
//::   else:
//::     return "false"
//::   #endif
//:: #enddef

//:: byte_meter_present = False
//:: packet_meter_present = False

//:: for each_meter, each_m_info in meter_info.items():
//::   if each_m_info["type_"] == "bytes":
//::     byte_meter_present = True
//::   #endif
//::   if each_m_info["type_"] == "packets":
//::     packet_meter_present = True
//::   #endif
//:: #endfor

//:: if len(meter_info) > 0:
//::   for unit in ["bytes", "packets"]:
//::     if unit == "bytes" and not byte_meter_present: continue
//::     if unit == "packets" and not packet_meter_present: continue

static pipe_meter_type_e get_${unit}_meter_type(p4_pd_${unit}_meter_spec_t *pd_spec, bool has_pre_color) {
  p4_pd_meter_type_t requested_meter_type = pd_spec->meter_type;
  if (requested_meter_type != PD_METER_TYPE_COLOR_AWARE &&
          requested_meter_type != PD_METER_TYPE_COLOR_UNAWARE) {
    bf_sys_dbgchk(0);
  }

  if (requested_meter_type == PD_METER_TYPE_COLOR_AWARE) {
	 bf_sys_dbgchk(has_pre_color == true);
  }

  if(!has_pre_color)
    return METER_TYPE_COLOR_UNAWARE;
  else if(requested_meter_type == PD_METER_TYPE_COLOR_UNAWARE)
    return METER_TYPE_COLOR_UNAWARE;
  else
    return METER_TYPE_COLOR_AWARE;
}

//::   #endfor

//::   if byte_meter_present:
static void bytes_meter_spec_pd_to_pipe(p4_pd_bytes_meter_spec_t *pd_spec,
					pipe_meter_spec_t *pipe_spec,
					bool has_pre_color) {
  pipe_spec->meter_type = get_bytes_meter_type(pd_spec, has_pre_color);
  pipe_meter_rate_t *cir = &pipe_spec->cir;
  pipe_meter_rate_t *pir = &pipe_spec->pir;
  cir->type = METER_RATE_TYPE_KBPS;
  cir->value.kbps = pd_spec->cir_kbps;
  pipe_spec->cburst = pd_spec->cburst_kbits; // conversion needed ?
  pir->type = METER_RATE_TYPE_KBPS;
  pir->value.kbps = pd_spec->pir_kbps;
  pipe_spec->pburst = pd_spec->pburst_kbits; // conversion needed ?
}
//::   #endif

//::   if packet_meter_present:
static void packets_meter_spec_pd_to_pipe(p4_pd_packets_meter_spec_t *pd_spec,
					  pipe_meter_spec_t *pipe_spec,
					  bool has_pre_color) {
  pipe_spec->meter_type = get_packets_meter_type(pd_spec, has_pre_color);
  pipe_meter_rate_t *cir = &pipe_spec->cir;
  pipe_meter_rate_t *pir = &pipe_spec->pir;
  cir->type = METER_RATE_TYPE_PPS;
  cir->value.pps = pd_spec->cir_pps;
  pipe_spec->cburst = pd_spec->cburst_pkts; // conversion needed ?
  pir->type = METER_RATE_TYPE_PPS;
  pir->value.pps = pd_spec->pir_pps;
  pipe_spec->pburst = pd_spec->pburst_pkts; // conversion needed ?
}
//::   #endif
//:: #endif

//:: if len(meter_info) > 0:
static void bytes_meter_spec_pipe_to_pd(p4_pd_bytes_meter_spec_t *pd_spec,
					pipe_meter_spec_t *pipe_spec)
{
  pd_spec->cir_kbps = pipe_spec->cir.value.kbps;
  pd_spec->pir_kbps = pipe_spec->pir.value.kbps;
  pd_spec->cburst_kbits = pipe_spec->cburst;
  pd_spec->pburst_kbits = pipe_spec->pburst;
  pd_spec->meter_type = pipe_spec->meter_type;
}

static void packets_meter_spec_pipe_to_pd(p4_pd_packets_meter_spec_t *pd_spec,
					  pipe_meter_spec_t *pipe_spec)
{
  pd_spec->cir_pps = pipe_spec->cir.value.pps;
  pd_spec->cburst_pkts = pipe_spec->cburst;
  pd_spec->pir_pps = pipe_spec->pir.value.pps;
  pd_spec->pburst_pkts = pipe_spec->pburst;
  pd_spec->meter_type = pipe_spec->meter_type;
}
//:: #endif
//::
//:: for register, r_info in register_info.items():
//::   if r_info['table_type'] == "fifo" and r_info['direction'] == "in":
static void register_${register}_value_pipe_to_pd(
    pipe_stful_mem_spec_t *stful_spec,
    ${r_info["v_type"]} *register_value) {
//::     access_map = {1: "byte", 2: "half", 3: "word", 4: "word", 8:"dbl"}
//::     if not r_info["layout"]:
//::       if r_info["width"] == 1:
//::         access = "bit"
//::       else:
//::         access = access_map[(r_info["width"] + 7) // 8]
//::       #endif
  *register_value = stful_spec->${access};
//::     else:
//::       name_0, width = r_info["layout"][0]
//::       name_1, _ = r_info["layout"][1]
//::       access = "dbl_" + access_map[width]
  register_value->${name_0} = stful_spec->${access}.hi;
  register_value->${name_1} = stful_spec->${access}.lo;
//::     #endif
}
//::   elif r_info['table_type'] != "fifo":
static void register_${register}_value_pipe_to_pd(
    pipe_stful_mem_query_t *stful_query,
    ${r_info["v_type"]} *register_values,
    int *value_count) {
  *value_count += stful_query->pipe_count;
  int p;
  for(p = 0; p < stful_query->pipe_count; p++) {
//::     access_map = {1: "byte", 2: "half", 3: "word", 4: "word", 8: "dbl"}
//::     if not r_info["layout"]:
//::       if r_info["width"] == 1:
//::         access = "bit"
//::       else:
//::         access = access_map[(r_info["width"] + 7) // 8]
//::       #endif
    register_values[p] = stful_query->data[p].${access};
//::     else:
//::       name_0, width = r_info["layout"][0]
//::       name_1, _ = r_info["layout"][1]
//::       access = "dbl_" + access_map[width]
    register_values[p].${name_0} = stful_query->data[p].${access}.hi;
    register_values[p].${name_1} = stful_query->data[p].${access}.lo;
//::     #endif
    }
}
//::   #endif

static void register_${register}_value_pd_to_pipe(
    ${r_info["v_type"]} *register_value,
    pipe_stful_mem_spec_t *stful_spec) {
//::   access_map = {1: "byte", 2: "half", 3: "word", 4: "word", 8: "dbl"}
//::   if not r_info["layout"]:
//::     if r_info["width"] == 1:
//::       access = "bit"
//::     else:
//::       access = access_map[(r_info["width"] + 7) // 8]
//::     #endif
  stful_spec->${access} = *register_value;
//::   else:
//::     name_0, width = r_info["layout"][0]
//::     name_1, _ = r_info["layout"][1]
//::     access = "dbl_" + access_map[width]
  stful_spec->${access}.hi = register_value->${name_0};
  stful_spec->${access}.lo = register_value->${name_1};
//::   #endif
}

//:: #endfor
//::
//:: if len(lpf_info) > 0:
static void lpf_spec_pd_to_pipe(p4_pd_lpf_spec_t *pd_spec,
                                pipe_lpf_spec_t *pipe_spec) {
  pipe_spec->lpf_type = pd_spec->lpf_type;
  pipe_spec->gain_decay_separate_time_constant =
    pd_spec->gain_decay_separate_time_constant;
  pipe_spec->gain_time_constant = pd_spec->gain_time_constant;
  pipe_spec->decay_time_constant = pd_spec->decay_time_constant;
  pipe_spec->time_constant = pd_spec->time_constant;
  pipe_spec->output_scale_down_factor = pd_spec->output_scale_down_factor;
}

static void lpf_spec_pipe_to_pd(p4_pd_lpf_spec_t *pd_spec,
                                pipe_lpf_spec_t *pipe_spec) {
  pd_spec->lpf_type = pipe_spec->lpf_type;
  pd_spec->gain_decay_separate_time_constant =
    pipe_spec->gain_decay_separate_time_constant;
  pd_spec->gain_time_constant = pipe_spec->gain_time_constant;
  pd_spec->decay_time_constant = pipe_spec->decay_time_constant;
  pd_spec->time_constant = pipe_spec->time_constant;
  pd_spec->output_scale_down_factor = pipe_spec->output_scale_down_factor;
}

//:: #endif
//:: if len(wred_info) > 0:
static void wred_spec_pd_to_pipe(p4_pd_wred_spec_t *pd_spec,
                                 pipe_wred_spec_t *pipe_spec) {
  pipe_spec->time_constant = pd_spec->time_constant;
  pipe_spec->red_min_threshold = pd_spec->red_min_threshold;
  pipe_spec->red_max_threshold = pd_spec->red_max_threshold;
  pipe_spec->max_probability = pd_spec->max_probability;
}

static void wred_spec_pipe_to_pd(p4_pd_wred_spec_t *pd_spec,
                                 pipe_wred_spec_t *pipe_spec) {
  pd_spec->time_constant = pipe_spec->time_constant;
  pd_spec->red_min_threshold = pipe_spec->red_min_threshold;
  pd_spec->red_max_threshold = pipe_spec->red_max_threshold;
  pd_spec->max_probability = pipe_spec->max_probability;
}
//:: #endif

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if len(t_info["match_fields"]) == 0:
//::     continue
//::   #endif
//::   match_type = t_info["match_type"]
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt",
//::              "const " + p4_pd_prefix + table + "_match_spec_t * const match_spec"]
//::   if match_type == "ternary" or match_type == "range":
//::     params += ["int priority"]
//::   #endif
//::   params += ["p4_pd_entry_hdl_t *entry_hdl"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + table + "_match_spec_to_entry_hdl"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::   num_match_bytes = get_num_match_bytes(t_info, field_info)
//::   num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::   if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::   #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  return pipe_mgr_match_spec_to_ent_hdl(sess_hdl,
            pipe_mgr_dev_tgt,
            ${table_hdl},
            &pipe_match_spec,
            entry_hdl,
            false /* light_pipe_validation */);
}

//:: #endfor


/* Dynamic Exm Table Key Mask */

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if len(t_info["match_fields"]) == 0:
//::     continue
//::   #endif
//::   if not t_info["dynamic_match_key_masks"]:
//::     continue
//::   #endif
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::              p4_pd_prefix + table + "_match_key_mask_spec_t *match_key_mask_spec"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + table + "_match_key_mask_spec_set"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::   num_match_bytes = get_num_match_bytes(t_info, field_info)
//::   num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};

  build_match_key_mask_spec_${table}(&pipe_match_spec, match_key_mask_spec);

  return pipe_mgr_match_key_mask_spec_set(sess_hdl,
            dev_id,
            ${table_hdl},
            &pipe_match_spec);
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + table + "_match_key_mask_spec_reset"
p4_pd_status_t
${name}
(
 ${param_str}
)
{

  return pipe_mgr_match_key_mask_spec_reset(sess_hdl,
            dev_id,
            ${table_hdl});
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::              p4_pd_prefix + table + "_match_key_mask_spec_t *match_key_mask_spec"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + table + "_match_key_mask_spec_get"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::   num_match_bytes = get_num_match_bytes(t_info, field_info)
//::   num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};


 p4_pd_status_t status = pipe_mgr_match_key_mask_spec_get(sess_hdl,
                                 dev_id,
                                 ${table_hdl},
                                 &pipe_match_spec);

  if (status) return status;

  unbuild_match_key_mask_spec_${table}(&pipe_match_spec, match_key_mask_spec);

  return (status);
}

//:: #endfor

//::
//::
//:: def get_direct_parameter_specs(direct_resources, register_info):
//::   specs = []
//::   for res_name, res_type, _ in direct_resources:
//::     if res_type == "bytes_meter":
//::       specs += ["p4_pd_bytes_meter_spec_t *" + res_name + "_spec"]
//::     elif res_type == "packets_meter":
//::       specs += ["p4_pd_packets_meter_spec_t *" + res_name + "_spec"]
//::     elif res_type == "register":
//::       specs += [register_info[res_name]["v_type"] + " *" + res_name + "_spec"]
//::     elif res_type == "lpf":
//::       specs += ["p4_pd_lpf_spec_t *" + res_name + "_spec"]
//::     elif res_type == "wred":
//::       specs += ["p4_pd_wred_spec_t *" + res_name + "_spec"]
//::     #endif
//::   #endfor
//::   return specs
//:: #enddef

//::
//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if action_table_hdl: continue
//::   match_type = t_info["match_type"]
//::   if len(t_info["match_fields"]) == 0: continue
//::   for action in t_info["actions"]:
//::     a_info = action_info[action]
//::     has_action_spec = len(a_info["param_names"]) > 0
//::     params = ["p4_pd_sess_hdl_t sess_hdl",
//::               "p4_pd_dev_target_t dev_tgt",
//::               "const " + p4_pd_prefix + table + "_match_spec_t * const match_spec"]
//::     if match_type == "ternary" or match_type == "range":
//::       params += ["int priority"]
//::     #endif
//::     if has_action_spec:
//::       params += ["const " + p4_pd_prefix + action + "_action_spec_t * const action_spec"]
//::     #endif
//::     if t_info["timeout"]:
//::       params += ["uint32_t ttl"]
//::     #endif
//::     params += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::     params += ["p4_pd_entry_hdl_t *entry_hdl"]
//::     param_str = ",\n ".join(params)
//::     name = p4_pd_prefix + table + "_table_add_with_" + action
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::     num_match_bytes = get_num_match_bytes(t_info, field_info)
//::     num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::     if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::     #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

//::     num_action_bytes = get_num_action_bytes(a_info["param_bit_widths"])
//::     num_action_bits = get_num_action_bits(a_info["param_bit_widths"])
  uint8_t pipe_action_data_bits[${num_action_bytes}];
  memset(pipe_action_data_bits, 0, ${num_action_bytes});
  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_ACTION_DATA_TYPE;
  pipe_action_data_spec_t *pipe_action_data_spec = &pipe_action_spec.act_data;
  pipe_action_data_spec->action_data_bits = pipe_action_data_bits;
  pipe_action_data_spec->num_valid_action_data_bits = ${num_action_bits};
  pipe_action_data_spec->num_action_data_bytes = ${num_action_bytes};
  pipe_action_spec.resource_count = 0;

//::     if has_action_spec:
  build_action_spec_${action}(pipe_action_data_spec, action_spec);
//::     #endif

//::     action_hdl = action_handles[(table, action)]
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

//::     if not t_info["timeout"]:
  uint32_t ttl = 0;
//::     #endif

//::
//::     for res_hdl in table_indirect_resources[table]:
//::       a_res = get_action_indirect_res(a_info, resource_handles, res_hdl)
//::       if a_res is None:
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
  }
//::       else:
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
//::         if a_res[0] == "constant":
    res_spec->tbl_idx = ${a_res[1]};
//::         else:
    res_spec->tbl_idx = action_spec->action_${a_res[2]};
//::         #endif
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
  }
//::       #endif
//::     #endfor

//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tbl_idx = 0;
//::       if "meter" in res_type:
    assert(${res_name}_spec && "direct resource has no spec");
//::         has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::       #endif
//::       if res_type == "bytes_meter":
    bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::       elif res_type == "packets_meter":
    packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::       #endif
//::       if res_type == "register":
//::         r_width = register_info[res_name]["width"]
    if(!${res_name}_spec) { // initialize to default 0 value
      memset(&res_spec->data.stful, 0, sizeof(res_spec->data.stful));
    }
    else {
      register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
    }
//::       #endif
//::       if res_type == "lpf":
    assert(${res_name}_spec && "direct resource has no spec");
    lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
//::       #endif
//::       if res_type == "wred":
    assert(${res_name}_spec && "direct resource has no spec");
    wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
//::       #endif
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
  }
//::     #endfor

  p4_pd_status_t status = pipe_mgr_mat_ent_add(sess_hdl, pipe_mgr_dev_tgt,
					       ${table_hdl}, &pipe_match_spec,
					       ${action_hdl}, &pipe_action_spec,
					       ttl,
					       0 /* TODO */,
					       entry_hdl);
  return status;
}

//::   #endfor
//:: #endfor

/* DELETE ENTRIES */

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if action_table_hdl: continue
//::   match_type = t_info["match_type"]
//::   if len(t_info["match_fields"]) == 0: continue
//::   for idx in range(2):
//::     name = p4_pd_prefix + table + "_table_delete"
//::     pipe_mgr_fn_name = "pipe_mgr_mat_ent_del"
//::     params = ["p4_pd_sess_hdl_t sess_hdl"]
//::     if idx:
//::       name += "_by_match_spec"
//::       pipe_mgr_fn_name += "_by_match_spec"
//::       params += ["p4_pd_dev_target_t dev_tgt",
//::                  "const " + p4_pd_prefix + table + "_match_spec_t * const match_spec"]
//::       if match_type == "ternary" or match_type == "range":
//::         params += ["int priority"]
//::       #endif
//::     else:
//::       params += ["bf_dev_id_t dev_id",
//::                  "p4_pd_entry_hdl_t entry_hdl"]
//::     #endif
//::     param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     if idx:
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::       num_match_bytes = get_num_match_bytes(t_info, field_info)
//::       num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::       if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::       #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  return ${pipe_mgr_fn_name}(sess_hdl,
            pipe_mgr_dev_tgt,
            ${table_hdl},
            &pipe_match_spec,
            0);
//::     else:
  return ${pipe_mgr_fn_name}(sess_hdl,
            dev_id,
            ${table_hdl},
            entry_hdl,
            0 /* TODO */);
//::     #endif
}

//::   #endfor
//:: #endfor

/* Get default entry handle */

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   name = p4_pd_prefix + table + "_table_get_default_entry_handle"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t pd_dev_tgt,
 p4_pd_entry_hdl_t* entry_hdl
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = pd_dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = pd_dev_tgt.dev_pipe_id;
  return pipe_mgr_table_get_default_entry_handle(sess_hdl, pipe_mgr_dev_tgt,
			                              ${table_hdl}, entry_hdl);
}

//::   name = p4_pd_prefix + table + "_table_get_default_entry"
//::   common_params = ["p4_pd_sess_hdl_t sess_hdl",
//::                    "p4_pd_dev_target_t pd_dev_tgt",
//::                    "bool read_from_hw"]
//::   if not action_table_hdl:
//::     params = common_params + [p4_pd_prefix + "action_specs_t * const action_spec"]
//::   else:
//::     # with indirect action data
//::     act_prof = t_info["action_profile"]
//::     assert(act_prof is not None)
//::
//::     if select_hdl:
//::       params = common_params + ["bool *has_mbr_hdl, bool *has_grp_hdl, p4_pd_grp_hdl_t *grp_hdl, p4_pd_mbr_hdl_t *mbr_hdl"]
//::     else:
//::       params = common_params + ["p4_pd_mbr_hdl_t *mbr_hdl"]
//::     #endif
//::     bound_indirect_res = []
//::     for indirect_res_name in t_info["ap_bind_indirect_res_to_match"]:
//::       bound_indirect_res += [indirect_res_name]
//::     #endfor
//::     params += ["int *" + r + "_index" for r in bound_indirect_res]
//::   #endif
//::   param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  p4_pd_status_t status = BF_SUCCESS;
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = pd_dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = pd_dev_tgt.dev_pipe_id;
  pipe_action_spec_t pipe_action_spec = {0};
  p4_pd_act_hdl_t act_fn_hdl;

//::   if not action_table_hdl:
  (void)action_spec;

  uint8_t pipe_action_data_bits[sizeof(${p4_pd_prefix}action_specs_t)];
  if (sizeof(${p4_pd_prefix}action_specs_t)) {
      memset(pipe_action_data_bits, 0, sizeof(${p4_pd_prefix}action_specs_t));
  }
  pipe_action_data_spec_t *pipe_action_data_spec = &pipe_action_spec.act_data;
  pipe_action_data_spec->num_action_data_bytes = sizeof(${p4_pd_prefix}action_specs_t);
  pipe_action_data_spec->action_data_bits = pipe_action_data_bits;

  status = pipe_mgr_table_get_default_entry(sess_hdl, pipe_mgr_dev_tgt, ${table_hdl},
      &pipe_action_spec, &act_fn_hdl, read_from_hw, PIPE_RES_GET_FLAG_ENTRY, NULL);
  if (status) return status;

  if (pipe_action_spec.pipe_action_datatype_bmap != PIPE_ACTION_DATA_TYPE) {
    return BF_UNEXPECTED;
  }
  pd_res_spec_t *res_spec = NULL;
  int resource_cnt = 0;
  build_pd_res_spec(&pipe_action_spec, &res_spec, &resource_cnt);

  switch(act_fn_hdl) {
//::     for action in t_info["actions"]:
//::       a_info = action_info[action]
//::       has_action_spec = len(a_info["param_names"]) > 0
//::       action_hdl = action_handles[(table, action)]
    case ${action_hdl}:
      action_spec->name = ${p4_pd_prefix}${action};
//::       if has_action_spec:
      unbuild_action_spec_${action}(pipe_action_data_spec,
                                    res_spec,
                                    resource_cnt,
                                    &action_spec->u.${p4_pd_prefix}${action});
//::       #endif
      break;
//::     #endfor
  default:
    return BF_UNEXPECTED;
  }
  if (res_spec) {
    bf_sys_free(res_spec);
  }

//::   else:
  status = pipe_mgr_table_get_default_entry(
      sess_hdl, pipe_mgr_dev_tgt, ${table_hdl}, &pipe_action_spec, &act_fn_hdl,
      read_from_hw, PIPE_RES_GET_FLAG_ENTRY, NULL);
  if (status) return status;

  if(pipe_action_spec.pipe_action_datatype_bmap == PIPE_ACTION_DATA_HDL_TYPE) {
//::     if select_hdl:
    (*has_grp_hdl) = false;
    (*has_mbr_hdl) = true;
//::     #endif
    (*mbr_hdl) = pipe_action_spec.adt_ent_hdl;
  }
//::     if select_hdl:
  else if(pipe_action_spec.pipe_action_datatype_bmap == PIPE_SEL_GRP_HDL_TYPE) {
    (*has_grp_hdl) = true;
    (*has_mbr_hdl) = false;
    (*grp_hdl) = pipe_action_spec.sel_grp_hdl;
  }
//::     #endif
  else { return BF_UNEXPECTED; }

//::     for res_hdl in table_indirect_resources[table]:
//::       res_name = resource_handles_rev[res_hdl]
  {
    int i;
    for(i = 0; i < pipe_action_spec.resource_count; i++) {
      if(pipe_action_spec.resources[i].tbl_hdl == ${res_hdl}) {
//::       if res_name in t_info["ap_bind_indirect_res_to_match"]:
        if (pipe_action_spec.resources[i].tag == PIPE_RES_ACTION_TAG_ATTACHED) {
            *(${res_name}_index) = pipe_action_spec.resources[i].tbl_idx;
        }
//::       #endif
        break;
      }
    }
  }
//::     #endfor

//::   #endif

  return status;
}


//:: #endfor


/* Clear default entry */

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   name = p4_pd_prefix + table + "_table_reset_default_entry"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t pd_dev_tgt
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = pd_dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = pd_dev_tgt.dev_pipe_id;
  return pipe_mgr_mat_tbl_default_entry_reset(sess_hdl, pipe_mgr_dev_tgt,
			                                  ${table_hdl}, 0);
}


//:: #endfor

/* MODIFY TABLE PROPERTIES */

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   name = p4_pd_prefix + table + "_set_property"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_tbl_prop_type_t property,
 p4_pd_tbl_prop_value_t value,
 p4_pd_tbl_prop_args_t args
)
{
  pipe_mgr_tbl_prop_type_t prop_type = property;
  pipe_mgr_tbl_prop_value_t prop_val;
  pipe_mgr_tbl_prop_args_t args_val;
  prop_val.value = value.value;
  args_val.value = args.value;
  return pipe_mgr_tbl_set_property(
      sess_hdl, dev_id, ${table_hdl}, prop_type, prop_val, args_val);
}

//::   name = p4_pd_prefix + table + "_get_property"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_tbl_prop_type_t property,
 p4_pd_tbl_prop_value_t *value,
 p4_pd_tbl_prop_args_t *args
)
{
  pipe_mgr_tbl_prop_type_t prop_type = property;
  pipe_mgr_tbl_prop_value_t prop_val;
  pipe_mgr_tbl_prop_args_t args_val;

  p4_pd_status_t status = pipe_mgr_tbl_get_property(
      sess_hdl, dev_id, ${table_hdl}, prop_type, &prop_val, &args_val);

  if (status != PIPE_MGR_SUCCESS) {
    value->value = -1;
    args->value = -1;
  } else {
    value->value = prop_val.value;
    args->value = args_val.value;
  }

  return status;
}

//:: #endfor

/* MODIFY ENTRIES */

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if action_table_hdl: continue
//::   match_type = t_info["match_type"]
//::   if len(t_info["match_fields"]) == 0: continue
//::   for action in t_info["actions"]:
//::     a_info = action_info[action]
//::     has_action_spec = len(a_info["param_names"]) > 0
//::     for idx in range(2):
//::       name = p4_pd_prefix + table + "_table_modify_with_" + action
//::       pipe_mgr_fn_name = "pipe_mgr_mat_ent_set_action"
//::       params = ["p4_pd_sess_hdl_t sess_hdl"]
//::       if idx:
//::         name += "_by_match_spec"
//::         pipe_mgr_fn_name += "_by_match_spec"
//::         params += ["p4_pd_dev_target_t dev_tgt",
//::                    "const " + p4_pd_prefix + table + "_match_spec_t * const match_spec"]
//::         if match_type == "ternary" or match_type == "range":
//::           params += ["int priority"]
//::         #endif
//::       else:
//::         params += ["bf_dev_id_t dev_id",
//::                    "p4_pd_entry_hdl_t entry_hdl"]
//::       #endif
//::       if has_action_spec:
//::         params += ["const " + p4_pd_prefix + action + "_action_spec_t * const action_spec"]
//::       #endif
//::       params += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::       param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::       if idx:
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::         num_match_bytes = get_num_match_bytes(t_info, field_info)
//::         num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::         if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::         #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
//::       #endif

//::       num_action_bytes = get_num_action_bytes(a_info["param_bit_widths"])
//::       num_action_bits = get_num_action_bits(a_info["param_bit_widths"])
  uint8_t pipe_action_data_bits[${num_action_bytes}];
  memset(pipe_action_data_bits, 0, ${num_action_bytes});
  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_ACTION_DATA_TYPE;
  pipe_action_data_spec_t *pipe_action_data_spec = &pipe_action_spec.act_data;
  pipe_action_data_spec->action_data_bits = pipe_action_data_bits;
  pipe_action_data_spec->num_valid_action_data_bits = ${num_action_bits};
  pipe_action_data_spec->num_action_data_bytes = ${num_action_bytes};
  pipe_action_spec.resource_count = 0;
//::       if has_action_spec:
  build_action_spec_${action}(pipe_action_data_spec, action_spec);
//::       #endif

//::       for res_hdl in table_indirect_resources[table]:
//::         a_res = get_action_indirect_res(a_info, resource_handles, res_hdl)
//::         if a_res is None:
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
  }
//::         else:
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
//::           if a_res[0] == "constant":
    res_spec->tbl_idx = ${a_res[1]};
//::           else:
    res_spec->tbl_idx = action_spec->action_${a_res[2]};
//::           #endif
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
  }
//::         #endif

//::       #endfor
//::       for res_name, res_type, res_hdl in table_direct_resources[table]:
//::         if res_type == "counter":
//::           continue
//::         #endif
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tbl_idx = 0;
    res_spec->tag = PIPE_RES_ACTION_TAG_NO_CHANGE;
    if(${res_name}_spec != NULL) {
//::         if "meter" in res_type:
//::           has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::           if res_type == "bytes_meter":
      bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::           elif res_type == "packets_meter":
      packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::           #endif
//::         elif res_type == "register":
//::           r_width = register_info[res_name]["width"]
      register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
//::         elif res_type == "lpf":
      lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
//::         elif res_type == "wred":
      wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
//::         #endif
      res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
    }
  }

//::       #endfor
//::       action_hdl = action_handles[(table, action)]
  return ${pipe_mgr_fn_name}(sess_hdl,
//::       if idx:
             pipe_mgr_dev_tgt,
//::       else:
             dev_id,
//::       #endif
             ${table_hdl},
//::       if idx:
             &pipe_match_spec,
//::       else:
             entry_hdl,
//::       #endif
             ${action_hdl},
             &pipe_action_spec,
             0 /* flags, TODO */);
}

//::     #endfor
//::   #endfor
//:: #endfor


/* SET DEFAULT_ACTION */

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if action_table_hdl: continue
//::   for action in t_info["actions"]:
//::     name = p4_pd_prefix + table + "_set_default_action_" + action
//::     a_info = action_info[action]
//::     if not a_info['allowed_to_be_default_action'][table]: continue
//::     has_action_spec = len(a_info["param_names"]) > 0
//::     params = ["p4_pd_sess_hdl_t sess_hdl",
//::               "p4_pd_dev_target_t dev_tgt"]
//::     if has_action_spec:
//::       params += ["const " + p4_pd_prefix + action + "_action_spec_t * const action_spec"]
//::     #endif
//::     params += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::     params += ["p4_pd_entry_hdl_t *entry_hdl"]
//::     param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     num_action_bytes = get_num_action_bytes(a_info["param_bit_widths"])
//::     num_action_bits = get_num_action_bits(a_info["param_bit_widths"])
  uint8_t pipe_action_data_bits[${num_action_bytes}];
  memset(pipe_action_data_bits, 0, ${num_action_bytes});
  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_ACTION_DATA_TYPE;
  pipe_action_data_spec_t *pipe_action_data_spec = &pipe_action_spec.act_data;
  pipe_action_data_spec->action_data_bits = pipe_action_data_bits;
  pipe_action_data_spec->num_valid_action_data_bits = ${num_action_bits};
  pipe_action_data_spec->num_action_data_bytes = ${num_action_bytes};
  pipe_action_spec.resource_count = 0;
//::     if has_action_spec:
  build_action_spec_${action}(pipe_action_data_spec, action_spec);
//::     #endif

//::
//::     for res_hdl in table_indirect_resources[table]:
//::       a_res = get_action_indirect_res(a_info, resource_handles, res_hdl)
//::       if a_res is None:
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
  }
//::       else:
  {
    pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
//::         if a_res[0] == "constant":
    res_spec->tbl_idx = ${a_res[1]};
//::         else:
    res_spec->tbl_idx = action_spec->action_${a_res[2]};
//::         #endif
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
  }
//::       #endif
//::     #endfor

//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
  {
      /* By default all direct resources have to be attached. For meters, there
       * should be an option not to run the meter ALU, hence passing a NULL
       * meter spec here to add the default entry is allowed, to indicate that
       * meter ALU to be disabled. Same applies to stful.
       */
      pipe_res_spec_t *res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
      res_spec->tbl_hdl = ${res_hdl};
      res_spec->tbl_idx = 0;
      res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
//::       if "meter" in res_type:
    if (${res_name}_spec) {
//::        has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::        if res_type == "bytes_meter":
        bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::        elif res_type == "packets_meter":
        packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::        #endif
     } else {
         res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
     }
//::       elif res_type == "lpf":
        if (${res_name}_spec) {
            lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
        } else {
            res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
        }
//::       elif res_type == "wred":
        if (${res_name}_spec) {
            wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
        } else {
            res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
        }
//::       elif res_type == "register":
//::         r_width = register_info[res_name]["width"]
    if(!${res_name}_spec) {
        res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    }
    else {
        register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
    }
//::       #endif
  }
//::     #endfor

//::     action_hdl = action_handles[(table, action)]
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  return pipe_mgr_mat_default_entry_set(sess_hdl,
				      pipe_mgr_dev_tgt,
				      ${table_hdl},
				      ${action_hdl},
				      &pipe_action_spec,
				      0 /* flags TODO */,
				      entry_hdl);
}

//::   #endfor
//:: #endfor



/* INDIRECT ACTION DATA AND MATCH SELECT */

//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if not action_table_hdl: continue
//::   act_prof = t_info["action_profile"]
//::   assert(act_prof is not None)
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)
//::   for action in t_info["actions"]:
//::     a_info = action_info[action]
//::     has_action_spec = len(a_info["param_names"]) > 0
//::     params = ["p4_pd_sess_hdl_t sess_hdl",
//::               "p4_pd_dev_target_t dev_tgt"]
//::     if has_action_spec:
//::       params += ["const " + p4_pd_prefix + action + "_action_spec_t * const action_spec"]
//::     #endif
//::     params += ["p4_pd_mbr_hdl_t *mbr_hdl"]
//::     param_str = ",\n ".join(params)
//::     name = p4_pd_prefix + act_prof + "_add_member_with_" + action
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     action_hdl = action_handles[(table, action)]
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

//::     num_action_bytes = get_num_action_bytes(a_info["param_bit_widths"])
//::     num_action_bits = get_num_action_bits(a_info["param_bit_widths"])
  uint8_t pipe_action_data_bits[${num_action_bytes}];
  memset(pipe_action_data_bits, 0, ${num_action_bytes});
  pipe_action_data_spec_t pipe_action_data_spec;
  pipe_action_data_spec.action_data_bits = pipe_action_data_bits;
  pipe_action_data_spec.num_valid_action_data_bits = ${num_action_bits};
  pipe_action_data_spec.num_action_data_bytes = ${num_action_bytes};

//::     if has_action_spec:
  build_action_spec_${action}(&pipe_action_data_spec, action_spec);
//::     #endif

  pipe_action_spec_t pipe_action_spec = {0};
//::     if has_action_spec:
  build_indirect_resources_${action}(&pipe_action_spec, action_spec);
//::     #endif
  pipe_action_spec.act_data = pipe_action_data_spec;
  p4_pd_status_t status = pipe_mgr_adt_ent_add(sess_hdl, pipe_mgr_dev_tgt,
					    ${action_table_hdl},
					    ${action_hdl},
                                            0,
					    &pipe_action_spec,
					    mbr_hdl, 0 /* TODO */);

  if(status != PIPE_MGR_SUCCESS) return status;

  p4_pd_ms_new_mbr(sess_hdl, ms_${act_prof}_state, dev_tgt.device_id, *mbr_hdl);
  p4_pd_ms_set_mbr_act(ms_${act_prof}_state, dev_tgt.device_id,
		       *mbr_hdl, ${action_hdl});

//::     for res_hdl in table_indirect_resources[table]:
//::       a_res = get_action_indirect_res(a_info, resource_handles, res_hdl)
//::       if a_res is not None:
  {
    pd_res_spec_t res_spec;
    res_spec.tbl_hdl = ${res_hdl};
//::         if a_res[0] == "constant":
    res_spec.tbl_idx = ${a_res[1]};
//::         else:
    res_spec.tbl_idx = action_spec->action_${a_res[2]};
//::         #endif
    p4_pd_ms_mbr_add_res(ms_${act_prof}_state, dev_tgt.device_id, *mbr_hdl, &res_spec);
  }
//::       #endif

//::     #endfor
//::
  return status;
}

//::     params = ["p4_pd_sess_hdl_t sess_hdl",
//::               "bf_dev_id_t dev_id",
//::               "p4_pd_mbr_hdl_t mbr_hdl"]
//::     if has_action_spec:
//::       params += ["const " + p4_pd_prefix + action + "_action_spec_t * const action_spec"]
//::     #endif
//::     param_str = ",\n ".join(params)
//::     name = p4_pd_prefix + act_prof + "_modify_member_with_" + action
p4_pd_status_t
${name}
(
 ${param_str}
 )
{
  p4_pd_act_hdl_t act_hdl;
  p4_pd_status_t status = p4_pd_ms_get_mbr_act(ms_${act_prof}_state, dev_id,
                                               mbr_hdl, &act_hdl);
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }
  // this target only supports modify with same action
  // TODO: this check is only necessary if member is in a group
  if(act_hdl != ${action_hdl})
    return -1;

//::     num_action_bytes = get_num_action_bytes(a_info["param_bit_widths"])
//::     num_action_bits = get_num_action_bits(a_info["param_bit_widths"])
  uint8_t pipe_action_data_bits[${num_action_bytes}];
  memset(pipe_action_data_bits, 0, ${num_action_bytes});
  pipe_action_data_spec_t pipe_action_data_spec;
  pipe_action_data_spec.action_data_bits = pipe_action_data_bits;
  pipe_action_data_spec.num_valid_action_data_bits = ${num_action_bits};
  pipe_action_data_spec.num_action_data_bytes = ${num_action_bytes};

//::     if has_action_spec:
  build_action_spec_${action}(&pipe_action_data_spec, action_spec);
//::     #endif
  pipe_action_spec_t pipe_action_spec = {0};
//::     if has_action_spec:
  build_indirect_resources_${action}(&pipe_action_spec, action_spec);
//::     #endif
  pipe_action_spec.act_data = pipe_action_data_spec;

  status = pipe_mgr_adt_ent_set(sess_hdl, dev_id,
					       ${action_table_hdl},
					       mbr_hdl,
					       ${action_hdl},
					       &pipe_action_spec,
					       0 /* TODO */);

  if(status != PIPE_MGR_SUCCESS) return status;
  
  return 0;
}

//::   #endfor
//::
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_mbr_hdl_t mbr_hdl"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_del_member"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  p4_pd_status_t status = pipe_mgr_adt_ent_del(sess_hdl, dev_id,
					       ${action_table_hdl},
					       mbr_hdl, 0 /* TODO */);

  if(status != PIPE_MGR_SUCCESS) return status;

  p4_pd_ms_del_mbr(sess_hdl, ms_${act_prof}_state, dev_id, mbr_hdl);

  return status;
}

//::   if not select_hdl: continue
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_sel_tbl_update_cb cb",
//::             "void *cb_ctx"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_register_callback"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  bf_dev_id_t bf_dev_id = dev_id;

  return pipe_mgr_sel_tbl_register_cb(sess_hdl, bf_dev_id,
					       ${select_hdl},
					       (pipe_mgr_sel_tbl_update_callback)cb,
					       cb_ctx);
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt",
//::             "uint32_t max_grp_size",
//::             "p4_pd_grp_hdl_t *grp_hdl"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_create_group"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  p4_pd_status_t status = pipe_mgr_sel_grp_add(sess_hdl, pipe_mgr_dev_tgt,
					       ${select_hdl},
                                               0,
					       max_grp_size,
                                               0xdeadbeef,
					       grp_hdl,
					       0 /* TODO */);

  if(status != PIPE_MGR_SUCCESS) return status;

  p4_pd_ms_new_grp(sess_hdl, ms_${act_prof}_state, dev_tgt.device_id, *grp_hdl);

  return status;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_grp_hdl_t grp_hdl"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_del_group"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  uint32_t count = 0;
  uint32_t mbrs_populated = 0;
  p4_pd_mbr_hdl_t *mbr_hdls = NULL;
  bool *enable = NULL;
  p4_pd_status_t status = PIPE_MGR_SUCCESS;

  status  = pipe_mgr_get_sel_grp_mbr_count(sess_hdl,
                                           dev_id,
                                           ${select_hdl},
                                           grp_hdl,
                                           &count);

  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }

  if (count) {
    mbr_hdls = bf_sys_calloc(count, sizeof(p4_pd_mbr_hdl_t));
    enable = bf_sys_calloc(count, sizeof(bool));

    if (mbr_hdls == NULL || enable == NULL) {
      status = PIPE_NO_SYS_RESOURCES;
      goto free_memory;
    }

    status = pipe_mgr_sel_grp_mbrs_get(sess_hdl,
                                       dev_id,
                                       ${select_hdl},
                                       grp_hdl,
                                       count,
                                       mbr_hdls,
                                       enable,
                                       &mbrs_populated,
                                       false);

    if (status != PIPE_MGR_SUCCESS) {
      goto free_memory;
    }
  }

  status = pipe_mgr_sel_grp_del(sess_hdl, dev_id,
                                ${select_hdl},
                                grp_hdl,
                                0 /* TODO */);

  if (status != PIPE_MGR_SUCCESS) {
    goto free_memory;
  }

  p4_pd_ms_del_grp(sess_hdl,
                   ms_${act_prof}_state,
                   dev_id,
                   grp_hdl,
                   mbrs_populated,
                   mbr_hdls);

free_memory:
  if (mbr_hdls) {
    bf_sys_free(mbr_hdls);
  }

  if (enable) {
    bf_sys_free(enable);
  }

  return status;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_grp_hdl_t grp_hdl",
//::             "p4_pd_mbr_hdl_t mbr_hdl"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_add_member_to_group"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  p4_pd_act_hdl_t act_hdl;
  p4_pd_status_t status = p4_pd_ms_get_mbr_act(ms_${act_prof}_state, dev_id, mbr_hdl,
                                               &act_hdl);
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }
  status = pipe_mgr_sel_grp_mbr_add(sess_hdl, dev_id,
						   ${select_hdl},
						   grp_hdl,
						   act_hdl,
						   mbr_hdl,
						   0 /* TODO */);

  if(status != PIPE_MGR_SUCCESS) return status;

  p4_pd_ms_add_mbr_to_grp(sess_hdl, ms_${act_prof}_state, dev_id, mbr_hdl, grp_hdl);
  p4_pd_ms_set_grp_act(ms_${act_prof}_state, dev_id, grp_hdl, act_hdl);

  return status;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_grp_hdl_t grp_hdl",
//::             "p4_pd_mbr_hdl_t mbr_hdl"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_del_member_from_group"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  p4_pd_status_t status = pipe_mgr_sel_grp_mbr_del(sess_hdl, dev_id,
						   ${select_hdl},
						   grp_hdl,
						   mbr_hdl,
						   0 /* TODO */);

  if(status != PIPE_MGR_SUCCESS) return status;

  p4_pd_ms_del_mbr_from_grp(sess_hdl, ms_${act_prof}_state, dev_id, mbr_hdl, grp_hdl);

  return status;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_grp_hdl_t grp_hdl",
//::             "p4_pd_mbr_hdl_t mbr_hdl",
//::             "enum p4_pd_grp_mbr_state_e mbr_state"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_group_member_state_set"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  if (mbr_state == P4_PD_GRP_MBR_STATE_ACTIVE) {
  return pipe_mgr_sel_grp_mbr_enable(sess_hdl, dev_id,
				     ${select_hdl},
				     grp_hdl,
				     mbr_hdl,
				     0 /* TODO */);
  } else if (mbr_state == P4_PD_GRP_MBR_STATE_INACTIVE) {
  return pipe_mgr_sel_grp_mbr_disable(sess_hdl, dev_id,
				      ${select_hdl},
				      grp_hdl,
				      mbr_hdl,
				      0 /* TODO */);
  }
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_grp_hdl_t grp_hdl",
//::             "p4_pd_mbr_hdl_t mbr_hdl",
//::             "enum p4_pd_grp_mbr_state_e *mbr_state_p"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_group_member_state_get"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  enum pipe_mgr_grp_mbr_state_e pipe_grp_mbr_state;
  p4_pd_status_t status;
  status = pipe_mgr_sel_grp_mbr_state_get(sess_hdl, dev_id,
				     ${select_hdl},
				     grp_hdl,
				     mbr_hdl,
             &pipe_grp_mbr_state);
  if (status != PIPE_SUCCESS) {
    return status;
  }
  switch(pipe_grp_mbr_state) {
    case PIPE_MGR_GRP_MBR_STATE_ACTIVE:
      *mbr_state_p = P4_PD_GRP_MBR_STATE_ACTIVE;
      break;
    case PIPE_MGR_GRP_MBR_STATE_INACTIVE:
      *mbr_state_p = P4_PD_GRP_MBR_STATE_INACTIVE;
      break;
  }
  return status;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_grp_hdl_t grp_hdl",
//::             "uint8_t *hash",
//::             "uint32_t hash_len",
//::             "p4_pd_mbr_hdl_t *mbr_hdl_p"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_group_member_get_from_hash"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  return pipe_mgr_sel_grp_mbr_get_from_hash(
      sess_hdl, dev_id, ${select_hdl}, grp_hdl, hash, hash_len, mbr_hdl_p);
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt",
//::             "p4_pd_mbr_hdl_t mbr_hdl"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_set_dynamic_action_selection_fallback_member"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  p4_pd_status_t status = pipe_mgr_sel_fallback_mbr_set(sess_hdl, pipe_mgr_dev_tgt,
					       ${select_hdl},
                 mbr_hdl,
					       0 /* TODO */);

  if(status != PIPE_MGR_SUCCESS) return status;

  return status;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_reset_dynamic_action_selection_fallback_member"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  p4_pd_status_t status = pipe_mgr_sel_fallback_mbr_reset(sess_hdl, pipe_mgr_dev_tgt,
					       ${select_hdl},
					       0 /* TODO */);

  if(status != PIPE_MGR_SUCCESS) return status;

  return status;
}

//::   params = ["bf_dev_id_t dev_id",
//::             "int (*cb_func)(p4_pd_sess_hdl_t, p4_pd_dev_target_t, void*, unsigned int, unsigned int, int, bool)",
//::             "void *cookie"]
//::   param_str = ",\n ".join(params)
//::   name = p4_pd_prefix + act_prof + "_sel_track_updates"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  return pipe_mgr_sel_tbl_register_cb(0, dev_id,
				     ${select_hdl},
				     (pipe_mgr_sel_tbl_update_callback)cb_func,
				     cookie);
}

//:: #endfor

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if not action_table_hdl: continue
//::   act_prof = t_info["action_profile"]
//::   assert(act_prof is not None)
//::   match_type = t_info["match_type"]
//::   if len(t_info["match_fields"]) == 0: continue
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt",
//::             "const " + p4_pd_prefix + table + "_match_spec_t * const match_spec"]
//::   if match_type == "ternary" or match_type == "range":
//::     params += ["int priority"]
//::   #endif
//::   params_wo = params + ["p4_pd_mbr_hdl_t mbr_hdl"]
//::   if t_info["timeout"]:
//::     params_wo += ["uint32_t ttl"]
//::   #endif
//::   params_wo += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::   bound_indirect_res = []
//::   for indirect_res_name in t_info["ap_bind_indirect_res_to_match"]:
//::     bound_indirect_res += [indirect_res_name]
//::   #endfor
//::   params_wo += ["int " + r + "_index" for r in bound_indirect_res]
//::   params_wo += ["p4_pd_entry_hdl_t *entry_hdl"]
//::   param_str = ",\n ".join(params_wo)
//::   name = p4_pd_prefix + table + "_add_entry"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::   num_match_bytes = get_num_match_bytes(t_info, field_info)
//::   num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::   if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::   #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_ACTION_DATA_HDL_TYPE;
  memset(&pipe_action_spec.act_data, 0, sizeof(pipe_action_spec.act_data));
  pipe_action_spec.adt_ent_hdl = mbr_hdl;
  pipe_action_spec.resource_count = 0;

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  p4_pd_act_hdl_t act_hdl;
  p4_pd_status_t status = p4_pd_ms_get_mbr_act(ms_${act_prof}_state,
                                                 dev_tgt.device_id, mbr_hdl,
                                                 &act_hdl);
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }

//::   if not t_info["timeout"]:
  uint32_t ttl = 0;
//::   #endif

  pd_res_spec_t *pd_res_spec;
  int res_count;
  p4_pd_ms_mbr_get_res(ms_${act_prof}_state, dev_tgt.device_id, mbr_hdl,
		       &res_count, &pd_res_spec);

//::     # using pd_res_spec for all resources guarantees that we are only
//::     # considering resources bound to the action handle
//::     for res_hdl in table_indirect_resources[table]:
//::       res_name = resource_handles_rev[res_hdl]
  {
    pipe_res_spec_t *res_spec;
    int i;
    for(i = 0; i < res_count; i++) {
      if(pd_res_spec[i].tbl_hdl == ${res_hdl}) {
	res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
	res_spec->tbl_hdl = ${res_hdl};
//::       if res_name in t_info["ap_bind_indirect_res_to_match"]:
        res_spec->tbl_idx = ${res_name}_index;
//::       else:
	res_spec->tbl_idx = pd_res_spec[i].tbl_idx;
//::       #endif
	res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
	break;
      }
    }
  }

//::     #endfor
//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
  {
    pipe_res_spec_t *res_spec;
    res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tbl_idx = 0;
//::       if "meter" in res_type:
    assert(${res_name}_spec && "direct resource has no spec");
//::         has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::       #endif
//::       if res_type == "bytes_meter":
    bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::       elif res_type == "packets_meter":
    packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::       #endif
//::       if res_type == "register":
//::         r_width = register_info[res_name]["width"]
    if(!${res_name}_spec) { // initialize to default 0 value
      memset(&res_spec->data.stful, 0, sizeof(res_spec->data.stful));
    }
    else {
      register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
    }
//::       #endif
//::       if res_type == "lpf":
    assert(${res_name}_spec && "direct resource has no spec");
    lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
//::       #endif
//::       if res_type == "wred":
    assert(${res_name}_spec && "direct resource has no spec");
    wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
//::       #endif
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
  }

//::     #endfor
  status = pipe_mgr_mat_ent_add(sess_hdl, pipe_mgr_dev_tgt,
					       ${table_hdl},
					       &pipe_match_spec,
					       act_hdl,
					       &pipe_action_spec,
					       ttl,
					       0 /* TODO */,
					       entry_hdl);

  return status;
}

//::   if not select_hdl: continue
//::   params_w = params + ["p4_pd_grp_hdl_t grp_hdl"]
//::   if t_info["timeout"]:
//::     params_w += ["uint32_t ttl"]
//::   #endif
//::   params_w += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::   params_w += ["int " + r + "_index" for r in bound_indirect_res]
//::   params_w += ["p4_pd_entry_hdl_t *entry_hdl"]
//::   param_str = ",\n ".join(params_w)
//::   name = p4_pd_prefix + table + "_add_entry_with_selector"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     # TODO: UNIFY WITH ABOVE FUNCTION !!!
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::   num_match_bytes = get_num_match_bytes(t_info, field_info)
//::   num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::   if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::   #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_SEL_GRP_HDL_TYPE;
  memset(&pipe_action_spec.act_data, 0, sizeof(pipe_action_spec.act_data));
  pipe_action_spec.sel_grp_hdl = grp_hdl;
  pipe_action_spec.resource_count = 0;

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  p4_pd_act_hdl_t act_hdl;
  p4_pd_status_t status =  p4_pd_ms_get_grp_act(ms_${act_prof}_state,
                                                 dev_tgt.device_id, grp_hdl,
                                                 &act_hdl);
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }

//::   if not t_info["timeout"]:
  uint32_t ttl = 0;
//::   #endif

  pd_res_spec_t *pd_res_spec;
  int res_count;
  p4_pd_ms_grp_get_res(ms_${act_prof}_state, dev_tgt.device_id, grp_hdl,
		       &res_count, &pd_res_spec);

//::     # using pd_res_spec for all resources guarantees that we are only
//::     # considering resources bound to the action handle
//::     for res_hdl in table_indirect_resources[table]:
//::       res_name = resource_handles_rev[res_hdl]
  {
    pipe_res_spec_t *res_spec;
    int i;
    for(i = 0; i < res_count; i++) {
      if(pd_res_spec[i].tbl_hdl == ${res_hdl}) {
	res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
	res_spec->tbl_hdl = ${res_hdl};
//::       if res_name in t_info["ap_bind_indirect_res_to_match"]:
        res_spec->tbl_idx = ${res_name}_index;
//::       else:
	res_spec->tbl_idx = pd_res_spec[i].tbl_idx;
//::       #endif
	res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
	break;
      }
    }
  }

//::     #endfor
//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
  {
    pipe_res_spec_t *res_spec;
    res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tbl_idx = 0;
//::       if "meter" in res_type:
    assert(${res_name}_spec && "direct resource has no spec");
//::         has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::       #endif
//::       if res_type == "bytes_meter":
    bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::       elif res_type == "packets_meter":
    packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::       #endif
//::       if res_type == "register":
//::         r_width = register_info[res_name]["width"]
    if(!${res_name}_spec) { // initialize to default 0 value
      memset(&res_spec->data.stful, 0, sizeof(res_spec->data.stful));
    }
    else {
      register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
    }
//::       #endif
//::       if res_type == "lpf":
    assert(${res_name}_spec && "direct resource has no spec");
    lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
//::       #endif
//::       if res_type == "wred":
    assert(${res_name}_spec && "direct resource has no spec");
    wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
//::       #endif
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
  }
//::     #endfor

  status = pipe_mgr_mat_ent_add(sess_hdl, pipe_mgr_dev_tgt,
					       ${table_hdl},
					       &pipe_match_spec,
					       act_hdl,
					       &pipe_action_spec,
					       ttl,
					       0 /* TODO */,
					       entry_hdl);

  return status;
}

//:: #endfor

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if not action_table_hdl: continue
//::   act_prof = t_info["action_profile"]
//::   assert(act_prof is not None)
//::   match_type = t_info["match_type"]
//::   if len(t_info["match_fields"]) == 0: continue
//::   for idx in range(2):
//::     name = p4_pd_prefix + table + "_modify_entry"
//::     pipe_mgr_fn_name = "pipe_mgr_mat_ent_set_action"
//::     params = ["p4_pd_sess_hdl_t sess_hdl"]
//::     if idx:
//::       name += "_by_match_spec"
//::       pipe_mgr_fn_name += "_by_match_spec"
//::       params += ["p4_pd_dev_target_t dev_tgt",
//::                  "const " + p4_pd_prefix + table + "_match_spec_t * const match_spec"]
//::       if match_type == "ternary" or match_type == "range":
//::         params += ["int priority"]
//::       #endif
//::     else:
//::       params += ["bf_dev_id_t dev_id",
//::                  "p4_pd_entry_hdl_t entry_hdl"]
//::     #endif
//::     params_wo = params + ["p4_pd_mbr_hdl_t mbr_hdl"]
//::     params_wo += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::     bound_indirect_res = []
//::     for indirect_res_name in t_info["ap_bind_indirect_res_to_match"]:
//::       bound_indirect_res += [indirect_res_name]
//::     #endfor
//::     params_wo += ["int " + r + "_index" for r in bound_indirect_res]
//::     param_str = ",\n ".join(params_wo)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     if idx:
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::       num_match_bytes = get_num_match_bytes(t_info, field_info)
//::       num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::       if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::       #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  bf_dev_id_t dev_id = dev_tgt.device_id;

//::     #endif
  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_ACTION_DATA_HDL_TYPE;
  memset(&pipe_action_spec.act_data, 0, sizeof(pipe_action_spec.act_data));
  pipe_action_spec.adt_ent_hdl = mbr_hdl;
  pipe_action_spec.resource_count = 0;

  p4_pd_act_hdl_t act_hdl;
  p4_pd_status_t status =  p4_pd_ms_get_mbr_act(
    ms_${act_prof}_state, dev_id, mbr_hdl, &act_hdl
  );
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }

  pd_res_spec_t *pd_res_spec;
  int res_count;
  p4_pd_ms_mbr_get_res(
    ms_${act_prof}_state, dev_id, mbr_hdl, &res_count, &pd_res_spec
  );

//::     for res_hdl in table_indirect_resources[table]:
//::       res_name = resource_handles_rev[res_hdl]
  {
    // in case the member has no such resource
    pipe_res_spec_t *res_spec;
    res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;

    int i;
    for(i = 0; i < res_count; i++) {
      if(pd_res_spec[i].tbl_hdl == ${res_hdl}) {
//::       if res_name in t_info["ap_bind_indirect_res_to_match"]:
        res_spec->tbl_idx = ${res_name}_index;
//::       else:
	res_spec->tbl_idx = pd_res_spec[i].tbl_idx;
//::       #endif
	res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
	break;
      }
    }
  }

//::     #endfor
//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
//::       if res_type == "counter":
//::         continue
//::       #endif
  {
    pipe_res_spec_t *res_spec;
    res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tbl_idx = 0;
    res_spec->tag = PIPE_RES_ACTION_TAG_NO_CHANGE;
    if(${res_name}_spec != NULL) {
//::       if "meter" in res_type:
//::         has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::         if res_type == "bytes_meter":
      bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::         elif res_type == "packets_meter":
      packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::         #endif
//::       elif res_type == "register":
//::         r_width = register_info[res_name]["width"]
      register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
//::       elif res_type == "lpf":
      lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
//::       elif res_type == "wred":
      wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
//::       #endif
      res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
    }
  }

//::     #endfor
  return ${pipe_mgr_fn_name}(sess_hdl,
//::     if idx:
             pipe_mgr_dev_tgt,
//::     else:
             dev_id,
//::     #endif
             ${table_hdl},
//::     if idx:
             &pipe_match_spec,
//::     else:
             entry_hdl,
//::     #endif
             act_hdl, &pipe_action_spec,
             0 /* flags, TODO */);
}

//::     if not select_hdl: continue
//::     params_w = params + ["p4_pd_grp_hdl_t grp_hdl"]
//::     params_w += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::     params_w += ["int " + r + "_index" for r in bound_indirect_res]
//::     param_str = ",\n ".join(params_w)
//::     name = p4_pd_prefix + table + "_modify_entry_with_selector"
//::     if idx:
//::       name += "_by_match_spec"
//::     #endif
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     if idx:
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::       num_match_bytes = get_num_match_bytes(t_info, field_info)
//::       num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::       if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::       #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  bf_dev_id_t dev_id = dev_tgt.device_id;

//::     #endif
  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_SEL_GRP_HDL_TYPE;
  memset(&pipe_action_spec.act_data, 0, sizeof(pipe_action_spec.act_data));
  pipe_action_spec.sel_grp_hdl = grp_hdl;
  pipe_action_spec.resource_count = 0;

  p4_pd_act_hdl_t act_hdl;
  p4_pd_status_t status =  p4_pd_ms_get_grp_act(
    ms_${act_prof}_state, dev_id, grp_hdl, &act_hdl
  );
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }

  pd_res_spec_t *pd_res_spec;
  int res_count;
  p4_pd_ms_grp_get_res(
    ms_${act_prof}_state, dev_id, grp_hdl, &res_count, &pd_res_spec
  );

//::     for res_hdl in table_indirect_resources[table]:
//::       res_name = resource_handles_rev[res_hdl]
  {
    // in case the member has no such resource
    pipe_res_spec_t *res_spec;
    res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;

    int i;
    for(i = 0; i < res_count; i++) {
      if(pd_res_spec[i].tbl_hdl == ${res_hdl}) {
//::       if res_name in t_info["ap_bind_indirect_res_to_match"]:
        res_spec->tbl_idx = ${res_name}_index;
//::       else:
	res_spec->tbl_idx = pd_res_spec[i].tbl_idx;
//::       #endif
	res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
	break;
      }
    }
  }

//::     #endfor
//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
//::       if res_type == "counter":
//::         continue
//::       #endif
  {
    pipe_res_spec_t *res_spec;
    res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tbl_idx = 0;
    res_spec->tag = PIPE_RES_ACTION_TAG_NO_CHANGE;
    if(${res_name}_spec != NULL) {
//::       if "meter" in res_type:
//::         has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::         if res_type == "bytes_meter":
      bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::         elif res_type == "packets_meter":
      packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::         #endif
//::       elif res_type == "register":
//::         r_width = register_info[res_name]["width"]
      register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
//::       elif res_type == "lpf":
      lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
//::       elif res_type == "wred":
      wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
//::       #endif
      res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
    }
  }

//::     #endfor
  return ${pipe_mgr_fn_name}(sess_hdl,
//::     if idx:
             pipe_mgr_dev_tgt,
//::     else:
             dev_id,
//::     #endif
             ${table_hdl},
//::     if idx:
             &pipe_match_spec,
//::     else:
             entry_hdl,
//::     #endif
             act_hdl, &pipe_action_spec,
             0 /* flags, TODO */);
}

//::   #endfor
//:: #endfor


//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   name = p4_pd_prefix + table + "_get_entry_count"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 uint32_t *count
)
{
  dev_target_t pipe_mgr_dev_tgt;
  bool read_from_hw = false;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_mgr_get_entry_count(sess_hdl, pipe_mgr_dev_tgt, ${table_hdl}, read_from_hw, count);
}

//:: #endfor

//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if not action_table_hdl: continue
//::   act_prof = t_info["action_profile"]
//::   assert(act_prof is not None)
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)
//::   name = p4_pd_prefix + act_prof + "_get_act_prof_entry_count"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 uint32_t *count
)
{
  dev_target_t pipe_mgr_dev_tgt;
  bool read_from_hw = false;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_mgr_get_entry_count(sess_hdl, pipe_mgr_dev_tgt, ${action_table_hdl}, read_from_hw, count);
}

//::   if not select_hdl: continue
//::   name = p4_pd_prefix + act_prof + "_get_selector_group_count"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 uint32_t *count
)
{
  dev_target_t pipe_mgr_dev_tgt;
  bool read_from_hw = false;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_mgr_get_entry_count(sess_hdl, pipe_mgr_dev_tgt, ${select_hdl}, read_from_hw, count);
}

//::   name = p4_pd_prefix + act_prof + "_get_selector_group_member_count"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_grp_hdl_t grp_hdl,
 uint32_t *count
)
{
  return pipe_mgr_get_sel_grp_mbr_count(sess_hdl, dev_id, ${select_hdl}, grp_hdl, count);
}

//:: #endfor

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   match_type = t_info["match_type"]
//::   has_match_spec = len(t_info["match_fields"]) > 0
//::   name = p4_pd_prefix + table + "_get_first_entry_handle"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt, p4_pd_entry_hdl_t *entry_handle
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_mgr_get_first_entry_handle(sess_hdl, ${table_hdl}, pipe_mgr_dev_tgt,
					 entry_handle);  
}

//::   name = p4_pd_prefix + table + "_get_next_entry_handles"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt, p4_pd_entry_hdl_t entry_handle,
  int n, p4_pd_entry_hdl_t *next_entry_handles
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_mgr_get_next_entry_handles(sess_hdl, ${table_hdl}, pipe_mgr_dev_tgt, entry_handle,
					 n, next_entry_handles);
}

//::   name = p4_pd_prefix + table + "_get_entry"
//::   common_params = ["p4_pd_sess_hdl_t sess_hdl",
//::                    "bf_dev_id_t dev_id",
//::                    "p4_pd_entry_hdl_t entry_hdl",
//::                    "bool read_from_hw"]
//::   if has_match_spec:
//::     common_params += [p4_pd_prefix + table + "_match_spec_t * match_spec"]
//::   #endif
//::   if match_type == "ternary" or match_type == "range":
//::     common_params += ["int *priority"]
//::   #endif
//::
//::   num_match_bytes = get_num_match_bytes(t_info, field_info)
//::   num_match_bits = get_num_match_bits(t_info, field_info)
//::
//::   if not action_table_hdl:
//::     params = common_params + [p4_pd_prefix + "action_specs_t * const action_spec"]
//::     param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  (void)sess_hdl;

  p4_pd_act_hdl_t act_fn_hdl;

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_id;
  pipe_mgr_dev_tgt.dev_pipe_id = BF_DEV_PIPE_ALL;

  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::  if num_match_bytes:
  uint8_t pipe_match_value_bits[${num_match_bytes}] = {0};
  uint8_t pipe_match_mask_bits[${num_match_bytes}] = {0};
//::  else:
  uint8_t *pipe_match_value_bits = NULL;
  uint8_t *pipe_match_mask_bits = NULL;
//::  #endif
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;

  pipe_action_spec_t pipe_action_spec = {0};
  uint8_t pipe_action_data_bits[sizeof(${p4_pd_prefix}action_specs_t)];
  if (sizeof(${p4_pd_prefix}action_specs_t)) {
      memset(pipe_action_data_bits, 0, sizeof(${p4_pd_prefix}action_specs_t));
  }
  pipe_action_data_spec_t *pipe_action_data_spec = &pipe_action_spec.act_data;
  pipe_action_data_spec->num_action_data_bytes = sizeof(${p4_pd_prefix}action_specs_t);
  pipe_action_data_spec->action_data_bits = pipe_action_data_bits;

  p4_pd_status_t status = pipe_mgr_get_entry(sess_hdl, ${table_hdl}, pipe_mgr_dev_tgt,
                                             entry_hdl, &pipe_match_spec,
                                             &pipe_action_spec, &act_fn_hdl,
                                             read_from_hw, PIPE_RES_GET_FLAG_ENTRY, NULL);
  if (status) return status;

  // some sanity checking on the returned spec
  // assert(pipe_match_spec.num_valid_match_bits == ${num_match_bits});
  // assert(pipe_match_spec.num_match_bytes == ${num_match_bytes});

  if (pipe_action_spec.pipe_action_datatype_bmap != PIPE_ACTION_DATA_TYPE) {
    bf_sys_dbgchk(0);
    return BF_UNEXPECTED;
  }

  pd_res_spec_t *res_spec = NULL;
  int resource_cnt = 0;
  build_pd_res_spec(&pipe_action_spec, &res_spec, &resource_cnt);

  switch(act_fn_hdl) {
//::     for action in t_info["actions"]:
//::       a_info = action_info[action]
//::       has_action_spec = len(a_info["param_names"]) > 0
//::       action_hdl = action_handles[(table, action)]
  case ${action_hdl}:
    action_spec->name = ${p4_pd_prefix}${action};
//::       if has_action_spec:
    unbuild_action_spec_${action}(pipe_action_data_spec,
                                  res_spec,
                                  resource_cnt,
                                  &action_spec->u.${p4_pd_prefix}${action});
//::       #endif
    break;
//::     #endfor
  default:
    bf_sys_dbgchk(NULL);
    status = BF_UNEXPECTED;
    break;
  }

//::       if has_match_spec:
  bool is_default_entry = (pipe_match_spec.match_value_bits == NULL && ${num_match_bytes} > 0);
  if(!is_default_entry)
    unbuild_key_${table}(&pipe_match_spec, match_spec);
//::       #endif

//::       if match_type == "ternary" or match_type == "range":
  *priority = pipe_match_spec.priority;
//::       #endif
//::       if match_type == "ternary" or match_type == "range" or match_type == "lpm":
//::         if has_match_spec:
  if(!is_default_entry)
    unbuild_mask_${table}(&pipe_match_spec, match_spec);
//::         #endif
//::       #endif

//::   if not action_table_hdl:
  (void)action_spec;
//::   #endif
  if (res_spec) {
    bf_sys_free(res_spec);
  }
  return status;
}

//::   else:
//::     # with indirect action data
//::     act_prof = t_info["action_profile"]
//::     assert(act_prof is not None)
//::
//::     if select_hdl:
//::       params = common_params + ["bool *has_mbr_hdl, bool *has_grp_hdl, p4_pd_grp_hdl_t *grp_hdl, p4_pd_mbr_hdl_t *mbr_hdl"]
//::     else:
//::       params = common_params + ["p4_pd_mbr_hdl_t *mbr_hdl"]
//::     #endif
//::     bound_indirect_res = []
//::     for indirect_res_name in t_info["ap_bind_indirect_res_to_match"]:
//::       bound_indirect_res += [indirect_res_name]
//::     #endfor
//::     params += ["int *" + r + "_index" for r in bound_indirect_res]
//::     param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  (void)sess_hdl;

  p4_pd_act_hdl_t act_fn_hdl;

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_id;
  pipe_mgr_dev_tgt.dev_pipe_id = BF_DEV_PIPE_ALL;

  pipe_tbl_match_spec_t pipe_match_spec = {0};
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }

  pipe_action_spec_t pipe_action_spec = {0};

  p4_pd_status_t status = pipe_mgr_get_entry(sess_hdl, ${table_hdl}, pipe_mgr_dev_tgt,
                                             entry_hdl, &pipe_match_spec,
                                             &pipe_action_spec, &act_fn_hdl,
                                             read_from_hw, PIPE_RES_GET_FLAG_ENTRY, NULL);
  if (status) return status;

  if(pipe_action_spec.pipe_action_datatype_bmap == PIPE_ACTION_DATA_HDL_TYPE) {
//:: if select_hdl:
    (*has_grp_hdl) = false;
    (*has_mbr_hdl) = true;
//:: #endif
    (*mbr_hdl) = pipe_action_spec.adt_ent_hdl;
  }
//::     if select_hdl:
  else if(pipe_action_spec.pipe_action_datatype_bmap == PIPE_SEL_GRP_HDL_TYPE) {
    (*has_grp_hdl) = true;
    (*has_mbr_hdl) = false;
    (*grp_hdl) = pipe_action_spec.sel_grp_hdl;
  }
//::     #endif
  else {
    bf_sys_dbgchk(NULL);
    return BF_UNEXPECTED;
  }

//::       if has_match_spec:
  bool is_default_entry = (pipe_match_spec.match_value_bits == NULL);
  if(!is_default_entry)
    unbuild_key_${table}(&pipe_match_spec, match_spec);
//::       #endif

//::       if match_type == "ternary" or match_type == "range":
  *priority = pipe_match_spec.priority;
//::         if has_match_spec:
  if(!is_default_entry)
    unbuild_mask_${table}(&pipe_match_spec, match_spec);
//::         #endif
//::       #endif

//::     for res_hdl in table_indirect_resources[table]:
//::       res_name = resource_handles_rev[res_hdl]
  {
    int i;
    for(i = 0; i < pipe_action_spec.resource_count; i++) {
      if(pipe_action_spec.resources[i].tbl_hdl == ${res_hdl}) {
//::       if res_name in t_info["ap_bind_indirect_res_to_match"]:
	if (pipe_action_spec.resources[i].tag == PIPE_RES_ACTION_TAG_ATTACHED) {
          *(${res_name}_index) = pipe_action_spec.resources[i].tbl_idx;
	}
//::       #endif
	break;
      }
    }
  }
//::     #endfor

  return status;
}
//::   #endif

//::   if gen_md_pd:
//::     name = p4_pd_prefix + table + "_get_entry_from_plcmt_data"
//::     common_params = ["void *mat_plcmt_data"]
//::     if has_match_spec:
//::       num_match_bytes = get_num_match_bytes(t_info, field_info)
//::       common_params += [p4_pd_prefix + table + "_match_spec_t *match_spec"]
//::     else:
//::       num_match_bytes = 1
//::     #endif
//::     if match_type == "ternary" or match_type == "range":
//::       common_params += ["int *priority"]
//::     #endif
//::     if not action_table_hdl:
//::       params = common_params + [p4_pd_prefix + "action_specs_t * const action_spec"]
//::     else:
//::       act_prof = t_info["action_profile"]
//::       assert(act_prof is not None)
//::       params = common_params + ["p4_pd_mbr_hdl_t *mbr_hdl",
//::                                 "p4_pd_grp_hdl_t *grp_hdl",
//::                                 "bool *has_mbr_hdl",
//::                                 "bool *has_grp_hdl"]
//::     #endif
//::     param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_tbl_match_spec_t pipe_match_spec = {0};
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  p4_pd_act_hdl_t act_fn_hdl;
  pipe_action_spec_t pipe_action_spec = {0};
  uint8_t pipe_action_data_bits[sizeof(${p4_pd_prefix}action_specs_t)];
  memset(pipe_action_data_bits, 0, sizeof(${p4_pd_prefix}action_specs_t));
  pipe_action_data_spec_t *pipe_action_data_spec = &pipe_action_spec.act_data;
  pipe_action_data_spec->action_data_bits = pipe_action_data_bits;

  p4_pd_status_t status = pipe_mgr_plcmt_mat_data_get_entry(mat_plcmt_data,
                                                            &pipe_match_spec,
                                                            &pipe_action_spec,
                                                            &act_fn_hdl);
  if (status) return status;
  pd_res_spec_t *res_spec = NULL;
  int resource_cnt = 0;
  (void) res_spec;
  (void) resource_cnt;
//::     if has_match_spec:
  bool is_default_entry = (pipe_match_spec.match_value_bits == NULL && ${num_match_bytes} > 0);
  if(!is_default_entry)
    unbuild_key_${table}(&pipe_match_spec, match_spec);

//::         if match_type == "ternary" or match_type == "range":
  *priority = pipe_match_spec.priority;
//::         #endif
//::         if match_type == "ternary" or match_type == "range" or match_type == "lpm":
//::             if has_match_spec:
  if(!is_default_entry)
    unbuild_mask_${table}(&pipe_match_spec, match_spec);
//::             #endif
//::         #endif
//::     #endif
//::     if not action_table_hdl:
  switch(act_fn_hdl) {
//::       for action in t_info["actions"]:
//::         a_info = action_info[action]
//::         has_action_spec = len(a_info["param_names"]) > 0
//::         action_hdl = action_handles[(table, action)]
  case ${action_hdl}:
    action_spec->name = ${p4_pd_prefix}${action};
//::         if has_action_spec:
    if (pipe_action_spec.resource_count) {
      build_pd_res_spec(&pipe_action_spec, &res_spec, &resource_cnt);
    }
    unbuild_action_spec_${action}(pipe_action_data_spec,
                                  res_spec,
                                  resource_cnt,
                                  &action_spec->u.${p4_pd_prefix}${action});
    if (res_spec) {
      bf_sys_free(res_spec);
    }
//::         #endif
    break;
//::       #endfor
  default:
    bf_sys_dbgchk(NULL);
    return BF_INVALID_ARG;
  }
//::     else:
  if(pipe_action_spec.pipe_action_datatype_bmap == PIPE_ACTION_DATA_HDL_TYPE) {
    *mbr_hdl = pipe_action_spec.adt_ent_hdl;
    *has_mbr_hdl = true;
    *has_grp_hdl = false;
  } else if(pipe_action_spec.pipe_action_datatype_bmap == PIPE_SEL_GRP_HDL_TYPE) {
    *grp_hdl = pipe_action_spec.sel_grp_hdl;
    *has_mbr_hdl = false;
    *has_grp_hdl = true;
  }
//::     #endif
  return status;
}

//::     name = p4_pd_prefix + table + "_get_plcmt_data"
//::     params = ["p4_pd_sess_hdl_t sess_hdl", "bf_dev_id_t dev_id"]
//::     param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
) {
  return pipe_mgr_plcmt_mat_data_get(sess_hdl, dev_id, ${table_hdl});
}

//::   #endif
//:: #endfor

/* Action name strings; can be indexed by the action name enum. */
const char * ${p4_pd_prefix}action_name_strings[] = {
//:: for action, a_info in action_info.items():
  "${p4_pd_prefix}${action}",
//:: #endfor
};
const char* ${p4_pd_prefix}action_enum_to_string(${p4_pd_prefix}action_names_t e) {
  if (e >= ${p4_pd_prefix}action_names_t_invalid) return NULL;
  return ${p4_pd_prefix}action_name_strings[e];
}
${p4_pd_prefix}action_names_t ${p4_pd_prefix}action_string_to_enum(const char* s) {
  if (!s) return ${p4_pd_prefix}action_names_t_invalid;
//:: for action, a_info in action_info.items():
  if (0 == strcmp(s, "${p4_pd_prefix}${action}")) return ${p4_pd_prefix}${action};
//:: #endfor
  return ${p4_pd_prefix}action_names_t_invalid;
}

//:: action_profiles = set()
//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if not action_table_hdl: continue
//::   act_prof = t_info["action_profile"]
//::   assert(act_prof is not None)
//::   if act_prof in action_profiles: continue
//::   action_profiles.add(act_prof)
//::
//::   name = p4_pd_prefix + act_prof + "_get_member"
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "p4_pd_mbr_hdl_t mbr_hdl",
//::             "bool read_from_hw",
//::             p4_pd_prefix + "action_specs_t *action_spec"]
//::   param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  (void)sess_hdl;

  p4_pd_act_hdl_t act_fn_hdl;

  pipe_action_data_spec_t pipe_action_data_spec;
  uint8_t pipe_action_data_bits[sizeof(${p4_pd_prefix}action_specs_t)];
  memset(pipe_action_data_bits, 0, sizeof(${p4_pd_prefix}action_specs_t));
  pipe_action_data_spec.action_data_bits = pipe_action_data_bits;
  pipe_action_data_spec.num_action_data_bytes = sizeof(${p4_pd_prefix}action_specs_t);

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_id;
  pipe_mgr_dev_tgt.dev_pipe_id = BF_DEV_PIPE_ALL;

  p4_pd_status_t status = pipe_mgr_get_action_data_entry(${action_table_hdl},
                              pipe_mgr_dev_tgt,
                              mbr_hdl,
                              &pipe_action_data_spec,
                              &act_fn_hdl,
                              read_from_hw);
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }

  pd_res_spec_t *res_spec = NULL;
  int resource_cnt = 0;
  (void) res_spec;
  (void) resource_cnt;

  switch(act_fn_hdl) {
//::   for action in t_info["actions"]:
//::     a_info = action_info[action]
//::     has_action_spec = len(a_info["param_names"]) > 0
//::     action_hdl = action_handles[(table, action)]
  case ${action_hdl}:
    action_spec->name = ${p4_pd_prefix}${action};
//::     if has_action_spec:
    p4_pd_ms_mbr_get_res(ms_${act_prof}_state,
                         dev_id,
                         mbr_hdl,
                         &resource_cnt,
                         &res_spec);

    unbuild_action_spec_${action}(&pipe_action_data_spec,
                          res_spec,
                          resource_cnt,
                          &action_spec->u.${p4_pd_prefix}${action});
//::     #endif
    break;
//::   #endfor
  default:
    bf_sys_dbgchk(NULL);
    return BF_UNEXPECTED;
  }
  return status;
}
//::   if gen_md_pd:

//::   name = p4_pd_prefix + act_prof + "_get_member_from_plcmt_data"
//::   params = ["void *adt_plcmt_data",
//::             p4_pd_prefix + "action_specs_t * const action_spec"]
//::   param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  p4_pd_act_hdl_t act_fn_hdl;

  pipe_action_data_spec_t pipe_action_data_spec;
  uint8_t pipe_action_data_bits[sizeof(${p4_pd_prefix}action_specs_t)];
  memset(pipe_action_data_bits, 0, sizeof(${p4_pd_prefix}action_specs_t));
  pipe_action_data_spec.action_data_bits = pipe_action_data_bits;

  p4_pd_status_t status = pipe_mgr_plcmt_adt_data_get_entry(
                              adt_plcmt_data,
                              &pipe_action_data_spec,
                              &act_fn_hdl);
 pd_res_spec_t *res_spec = NULL;
 int resource_cnt = 0;
 (void) res_spec;
 (void) resource_cnt;

  switch(act_fn_hdl) {
//::   for action in t_info["actions"]:
//::     a_info = action_info[action]
//::     has_action_spec = len(a_info["param_names"]) > 0
//::     action_hdl = action_handles[(table, action)]
  case ${action_hdl}:
    action_spec->name = ${p4_pd_prefix}${action};
//::     if has_action_spec:
    unbuild_action_spec_${action}(&pipe_action_data_spec,
                                  res_spec,
                                  resource_cnt,
                                  &action_spec->u.${p4_pd_prefix}${action});
//::     #endif
    break;
//::   #endfor
  default:
    bf_sys_dbgchk(NULL);
    return BF_UNEXPECTED;
  }
  return status;
}

//::   name = p4_pd_prefix + act_prof + "_get_full_member_info_from_plcmt_data"
//::   params = ["uint8_t device_id",
//::             "p4_pd_mbr_hdl_t mbr_hdl",
//::             "void *adt_plcmt_data",
//::             p4_pd_prefix + "action_specs_t * const action_spec"]
//::   param_str = ",\n ".join(params)
/* This API is similar to the api above, with the exception that
 * it will return the complete member info, complete with indirect
 * resource indices as well. This API takes in the device id and the
 * mbr hdl as arguments for that purpose.
 */
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  p4_pd_act_hdl_t act_fn_hdl;

  pipe_action_data_spec_t pipe_action_data_spec;
  uint8_t pipe_action_data_bits[sizeof(${p4_pd_prefix}action_specs_t)];
  memset(pipe_action_data_bits, 0, sizeof(${p4_pd_prefix}action_specs_t));
  pipe_action_data_spec.action_data_bits = pipe_action_data_bits;

  p4_pd_status_t status = pipe_mgr_plcmt_adt_data_get_entry(
                              adt_plcmt_data,
                              &pipe_action_data_spec,
                              &act_fn_hdl);
 pd_res_spec_t *res_spec = NULL;
 int resource_cnt = 0;
 (void) res_spec;
 (void) resource_cnt;

  switch(act_fn_hdl) {
//::   for action in t_info["actions"]:
//::     a_info = action_info[action]
//::     has_action_spec = len(a_info["param_names"]) > 0
//::     action_hdl = action_handles[(table, action)]
  case ${action_hdl}:
    action_spec->name = ${p4_pd_prefix}${action};
//::     if has_action_spec:
    if (ms_${act_prof}_state) {
      p4_pd_ms_mbr_get_res(ms_${act_prof}_state,
                           device_id,
                           mbr_hdl,
                           &resource_cnt, &res_spec);
    }

    unbuild_action_spec_${action}(&pipe_action_data_spec,
                                  res_spec,
                                  resource_cnt,
                                  &action_spec->u.${p4_pd_prefix}${action});
//::     #endif
    break;
//::   #endfor
  default:
    bf_sys_dbgchk(NULL);
    return BF_UNEXPECTED;
  }
  return status;
}

//::   name = p4_pd_prefix + act_prof + "_get_plcmt_data"
//::   params = ["p4_pd_sess_hdl_t sess_hdl", "bf_dev_id_t dev_id"]
//::   param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  return pipe_mgr_plcmt_adt_data_get(sess_hdl, dev_id, ${action_table_hdl});
}
//::   #endif

//::     name = p4_pd_prefix + act_prof + "_get_first_member"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt, int *entry_handle
)
{
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
    return pipe_mgr_get_first_entry_handle(sess_hdl, ${action_table_hdl}, pipe_mgr_dev_tgt, entry_handle);
}
//::     name = p4_pd_prefix + act_prof + "_get_next_members"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt, p4_pd_entry_hdl_t entry_handle,
  int n, int *next_entry_handles
)
{
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
    return pipe_mgr_get_next_entry_handles(sess_hdl, ${action_table_hdl}, pipe_mgr_dev_tgt, entry_handle,
                       n, next_entry_handles);
}
//::   if not select_hdl: continue
//::   name = p4_pd_prefix + act_prof + "_get_first_group"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt, int *grp_hdl
)
{
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
    return pipe_mgr_get_first_entry_handle(sess_hdl, ${select_hdl}, pipe_mgr_dev_tgt, grp_hdl);
}
//::   name = p4_pd_prefix + act_prof + "_get_next_groups"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt, int grp_hdl,
  int n, int *grp_hdls
)
{
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
    // Grp_hdls array has to be big enough to hold at least n group handles
    return pipe_mgr_get_next_entry_handles(sess_hdl, ${select_hdl}, pipe_mgr_dev_tgt,
            grp_hdl, n, grp_hdls);
}
//::   name = p4_pd_prefix + act_prof + "_get_first_group_member"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, bf_dev_id_t dev_id, int grp_hdl,
  int *mbr_hdl
)
{
    return pipe_mgr_get_first_group_member(sess_hdl, ${select_hdl}, dev_id,
            grp_hdl, mbr_hdl);
}
//::   name = p4_pd_prefix + act_prof + "_get_next_group_members"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, bf_dev_id_t dev_id, int grp_hdl,
  int mbr_hdl, int n, int *mbr_hdls
)
{
    // mbr_hdls array should be big enough to hold atleast n member handles
    return pipe_mgr_get_next_group_members(sess_hdl, ${select_hdl}, dev_id,
            grp_hdl, mbr_hdl, n, mbr_hdls);
}
//::   if gen_md_pd:
//::     name = p4_pd_prefix + act_prof + "_get_word_llp_active_member_count"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt, int word_index,
  int *count
)
{
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
    return pipe_mgr_get_word_llp_active_member_count(
        sess_hdl, pipe_mgr_dev_tgt, ${select_hdl}, word_index, count);
}
//::     name = p4_pd_prefix + act_prof + "_get_word_llp_active_members"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, p4_pd_dev_target_t dev_tgt, int word_index,
  int count, int *mbr_hdls
)
{
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

    // mbr_hdls array should be big enough to hold atleast count member handles
    return pipe_mgr_get_word_llp_active_members(
        sess_hdl, pipe_mgr_dev_tgt, ${select_hdl}, word_index, count, mbr_hdls);
}
//::     name = p4_pd_prefix + act_prof + "_sel_get_plcmt_data"
p4_pd_status_t
${name}
(
  p4_pd_sess_hdl_t sess_hdl, bf_dev_id_t dev_id
)
{
    return pipe_mgr_plcmt_sel_data_get(sess_hdl, dev_id, ${select_hdl});
}
//::   #endif
//:: #endfor

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if not action_table_hdl: continue
//::   match_type = t_info["match_type"]
//::   if len(t_info["match_fields"]) == 0: continue
//::   for idx in range(2):
//::     name = p4_pd_prefix + table + "_table_delete"
//::     pipe_mgr_fn_name = "pipe_mgr_mat_ent_del"
//::     params = ["p4_pd_sess_hdl_t sess_hdl"]
//::     if idx:
//::       name += "_by_match_spec"
//::       pipe_mgr_fn_name += "_by_match_spec"
//::       params += ["p4_pd_dev_target_t dev_tgt",
//::                  "const " + p4_pd_prefix + table + "_match_spec_t * const match_spec"]
//::       if match_type == "ternary" or match_type == "range":
//::         params += ["int priority"]
//::       #endif
//::     else:
//::       params += ["bf_dev_id_t dev_id",
//::                  "p4_pd_entry_hdl_t entry_hdl"]
//::     #endif
//::     param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     if idx:
  pipe_tbl_match_spec_t pipe_match_spec = {0};
//::       num_match_bytes = get_num_match_bytes(t_info, field_info)
//::       num_match_bits = get_num_match_bits(t_info, field_info)
  uint8_t pipe_match_value_bits[${num_match_bytes}];
  pipe_match_spec.match_value_bits = pipe_match_value_bits;
  uint8_t pipe_match_mask_bits[${num_match_bytes}];
  if (${num_match_bytes}) {
    memset(pipe_match_value_bits, 0, ${num_match_bytes});
    memset(pipe_match_mask_bits, 0, ${num_match_bytes});
  }
  pipe_match_spec.match_mask_bits = pipe_match_mask_bits;
  pipe_match_spec.num_valid_match_bits = ${num_match_bits};
  pipe_match_spec.num_match_bytes = ${num_match_bytes};
//::       if match_type == "ternary" or match_type == "range":
  pipe_match_spec.priority = priority;
//::       #endif
  build_match_spec_${table}(&pipe_match_spec, match_spec);

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  return ${pipe_mgr_fn_name}(sess_hdl,
            pipe_mgr_dev_tgt,
            ${table_hdl},
            &pipe_match_spec,
            0 /* TODO */);
//::     else:
  return ${pipe_mgr_fn_name}(sess_hdl,
            dev_id,
            ${table_hdl},
            entry_hdl,
            0 /* TODO */);
//::     #endif
}
//::   #endfor
//:: #endfor

//:: for table, t_info in table_info.items():
//::   table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::   if not action_table_hdl: continue
//::   act_prof = t_info["action_profile"]
//::   assert(act_prof is not None)
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt"]
//::   params_wo = params + ["p4_pd_mbr_hdl_t mbr_hdl"]
//::   params_wo += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::   bound_indirect_res = []
//::   for indirect_res_name in t_info["ap_bind_indirect_res_to_match"]:
//::     bound_indirect_res += [indirect_res_name]
//::   #endfor
//::   params_wo += ["int " + r + "_index" for r in bound_indirect_res]
//::   params_wo += ["p4_pd_entry_hdl_t *entry_hdl"]
//::   param_str = ",\n ".join(params_wo)
//::   name = p4_pd_prefix + table + "_set_default_entry"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_ACTION_DATA_HDL_TYPE;
  memset(&pipe_action_spec.act_data, 0, sizeof(pipe_action_spec.act_data));
  pipe_action_spec.adt_ent_hdl = mbr_hdl;
  pipe_action_spec.resource_count = 0;

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  p4_pd_act_hdl_t act_hdl;
  p4_pd_status_t status = p4_pd_ms_get_mbr_act(ms_${act_prof}_state,
                                                 dev_tgt.device_id, mbr_hdl,
                                                 &act_hdl);
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }

  pd_res_spec_t *pd_res_spec;
  int res_count;
  p4_pd_ms_mbr_get_res(ms_${act_prof}_state, dev_tgt.device_id, mbr_hdl,
		       &res_count, &pd_res_spec);

//::     # using pd_res_spec for all resources guarantees that we are only
//::     # considering resources bound to the action handle
//::     for res_hdl in table_indirect_resources[table]:
//::       res_name = resource_handles_rev[res_hdl]
  {
    pipe_res_spec_t *res_spec;
    int i;
    for(i = 0; i < res_count; i++) {
      if(pd_res_spec[i].tbl_hdl == ${res_hdl}) {
	res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
	res_spec->tbl_hdl = ${res_hdl};
//::       if res_name in t_info["ap_bind_indirect_res_to_match"]:
        res_spec->tbl_idx = ${res_name}_index;
//::       else:
	res_spec->tbl_idx = pd_res_spec[i].tbl_idx;
//::       #endif
	res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
	break;
      }
    }
  }

//::     #endfor
//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
  {
    pipe_res_spec_t *res_spec;
    res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tbl_idx = 0;
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
//::       if "meter" in res_type:
    if(${res_name}_spec) {
//::        has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::        if res_type == "bytes_meter":
    bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::        elif res_type == "packets_meter":
    packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::        #endif
    } else {
        res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    }
//::       elif res_type == "lpf":
    if(${res_name}_spec) {
        lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
    } else {
        res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    }
//::       elif res_type == "wred":
    if(${res_name}_spec) {
        wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
    } else {
        res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    }
//::       elif res_type == "register":
//::         r_width = register_info[res_name]["width"]
    if (!${res_name}_spec) {
        res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    } else {
      register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
    }
//::       #endif
  }

//::     #endfor
  return pipe_mgr_mat_default_entry_set(sess_hdl,
				      pipe_mgr_dev_tgt,
				      ${table_hdl},
				      act_hdl,
				      &pipe_action_spec,
				      0 /* flags TODO */,
				      entry_hdl);
}

//::   if not select_hdl: continue
//::   params_w = params + ["p4_pd_grp_hdl_t grp_hdl"]
//::   params_w += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::   params_w += ["int " + r + "_index" for r in bound_indirect_res]
//::   params_w += ["p4_pd_entry_hdl_t *entry_hdl"]
//::   param_str = ",\n ".join(params_w)
//::   name = p4_pd_prefix + table + "_set_default_entry_with_selector"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_action_spec_t pipe_action_spec = {0};
  pipe_action_spec.pipe_action_datatype_bmap = PIPE_SEL_GRP_HDL_TYPE;
  memset(&pipe_action_spec.act_data, 0, sizeof(pipe_action_spec.act_data));
  pipe_action_spec.sel_grp_hdl = grp_hdl;
  pipe_action_spec.resource_count = 0;

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  p4_pd_act_hdl_t act_hdl;
  p4_pd_status_t status =  p4_pd_ms_get_grp_act(ms_${act_prof}_state,
                                                 dev_tgt.device_id, grp_hdl,
                                                 &act_hdl);
  if (status != PIPE_MGR_SUCCESS) {
    return status;
  }

  pd_res_spec_t *pd_res_spec;
  int res_count;
  p4_pd_ms_grp_get_res(ms_${act_prof}_state, dev_tgt.device_id, grp_hdl,
		       &res_count, &pd_res_spec);

//::     # using pd_res_spec for all resources guarantees that we are only
//::     # considering resources bound to the action handle
//::     for res_hdl in table_indirect_resources[table]:
//::       res_name = resource_handles_rev[res_hdl]
  {
    pipe_res_spec_t *res_spec;
    int i;
    for(i = 0; i < res_count; i++) {
      if(pd_res_spec[i].tbl_hdl == ${res_hdl}) {
	res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
	res_spec->tbl_hdl = ${res_hdl};
//::       if res_name in t_info["ap_bind_indirect_res_to_match"]:
        res_spec->tbl_idx = ${res_name}_index;
//::       else:
	res_spec->tbl_idx = pd_res_spec[i].tbl_idx;
//::       #endif
	res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
	break;
      }
    }
  }

//::     #endfor
//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
  {
    pipe_res_spec_t *res_spec;
    res_spec = &pipe_action_spec.resources[pipe_action_spec.resource_count++];
    res_spec->tbl_hdl = ${res_hdl};
    res_spec->tbl_idx = 0;
    res_spec->tag = PIPE_RES_ACTION_TAG_ATTACHED;
//::       if "meter" in res_type:
    if (${res_name}_spec) {
//::         has_pre_color = p4_meter_has_pre_color(res_name, meter_info)
//::        if res_type == "bytes_meter":
    bytes_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::        elif res_type == "packets_meter":
    packets_meter_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.meter, ${has_pre_color});
//::        #endif
    } else {
        res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    }
//::       elif res_type == "lpf":
    if (${res_name}_spec) {
        lpf_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.lpf);
    } else {
        res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    }
//::       elif res_type == "wred":
    if (${res_name}_spec) {
        wred_spec_pd_to_pipe(${res_name}_spec, &res_spec->data.red);
    } else {
        res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    }
//::       elif res_type == "register":
//::         r_width = register_info[res_name]["width"]
    if(!${res_name}_spec) { // initialize to default 0 value
      res_spec->tag = PIPE_RES_ACTION_TAG_DETACHED;
    } else {
      register_${res_name}_value_pd_to_pipe(${res_name}_spec, &res_spec->data.stful);
    }
//::       #endif
  }
//::     #endfor

  return pipe_mgr_mat_default_entry_set(sess_hdl,
				      pipe_mgr_dev_tgt,
				      ${table_hdl},
				      act_hdl,
				      &pipe_action_spec,
				      0 /* flags TODO */,
				      entry_hdl);  
}

//:: #endfor
//:: for lq in learn_quanta:
p4_pd_status_t
${lq["register_fn"]}
(
 p4_pd_sess_hdl_t         sess_hdl,
 uint8_t                  device_id,
 ${lq["cb_fn_type"]}      cb_fn,
 void                    *cb_fn_cookie
)
{
  return pipe_mgr_lrn_digest_notification_register(sess_hdl, device_id, ${lq["handle"]}, (pipe_flow_lrn_notify_cb)cb_fn, cb_fn_cookie);
}

p4_pd_status_t
${lq["deregister_fn"]}
(
 p4_pd_sess_hdl_t         sess_hdl,
 uint8_t                  device_id
)
{
  return pipe_mgr_lrn_digest_notification_deregister(sess_hdl, device_id, ${lq["handle"]});
}

p4_pd_status_t
${lq["notify_ack_fn"]}
(
 p4_pd_sess_hdl_t         sess_hdl,
 ${lq["msg_type"]}       *msg
)
{
  return pipe_mgr_flow_lrn_notify_ack(sess_hdl, ${lq["handle"]}, (pipe_flow_lrn_msg_t*)msg);
}
//:: #endfor

p4_pd_status_t
${p4_pd_prefix}set_learning_timeout(p4_pd_sess_hdl_t shdl,
                                    uint8_t          device_id,
                                    uint32_t         usecs) 
{
  return pipe_mgr_flow_lrn_set_timeout(shdl, device_id, usecs);
}

/* COUNTERS */

//:: for counter, c_info in counter_info.items():
//::   binding = c_info["binding"]
//::   type_ = c_info["type_"]
//::   if binding[0] == "direct":
//::     table_hdl, _, _ = table_handles[binding[1]]
//::     name = "p4_pd_" + p4_prefix + "_counter_read_" + counter
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t entry_hdl,
 int flags,
 p4_pd_counter_value_t *counter_value
)
{
  p4_pd_status_t status;

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  if(flags & COUNTER_READ_HW_SYNC) {
    status = pipe_mgr_direct_stat_ent_database_sync(sess_hdl,
                                                   pipe_mgr_dev_tgt,
                                                   ${table_hdl},
                                                   entry_hdl);
    if (status != PIPE_SUCCESS) {
      return status;
    }
  }

  pipe_stat_data_t stat_data;
  status = pipe_mgr_mat_ent_direct_stat_query(sess_hdl,
					      dev_tgt.device_id,
					      ${table_hdl},
					      entry_hdl,
					      &stat_data);
  if (status != PIPE_SUCCESS) {
    return status;
  }
  counter_value->bytes = stat_data.bytes;
  counter_value->packets = stat_data.packets;
  return PIPE_SUCCESS;
}

//::     name = "p4_pd_" + p4_prefix + "_counter_write_" + counter
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t entry_hdl,
 p4_pd_counter_value_t counter_value
)
{
  pipe_stat_data_t stat_data;
  stat_data.bytes = counter_value.bytes;
  stat_data.packets = counter_value.packets;
  p4_pd_status_t status = pipe_mgr_mat_ent_direct_stat_set(sess_hdl,
							   dev_tgt.device_id,
							   ${table_hdl},
							   entry_hdl,
							   &stat_data);
  return status;
}

//::   else:
//::     counter_hdl = counter_handles[counter]
//::     name = "p4_pd_" + p4_prefix + "_counter_read_" + counter
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int index,
 int flags,
 p4_pd_counter_value_t *counter_value
)
{
  p4_pd_status_t status;
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  if(flags & COUNTER_READ_HW_SYNC) {
    status = pipe_mgr_stat_ent_database_sync(sess_hdl,
                                             pipe_mgr_dev_tgt,
                                             ${counter_hdl},
                                             index);
    if (status != PIPE_SUCCESS) {
      return status;
    }
  }

  pipe_stat_data_t stat_data = {0};
  pipe_stat_data_t *ptr = &stat_data;
  status = pipe_mgr_stat_ent_query(sess_hdl,
				   pipe_mgr_dev_tgt,
				   ${counter_hdl},
				   &index,
                                   1,
				   &ptr);
  if (status != PIPE_SUCCESS) {
    return status;
  }
  counter_value->bytes = stat_data.bytes;
  counter_value->packets = stat_data.packets;
  return PIPE_SUCCESS;
}

//::     name = "p4_pd_" + p4_prefix + "_counter_write_" + counter
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int index,
 p4_pd_counter_value_t counter_value
)
{
  pipe_stat_data_t stat_data;
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  stat_data.bytes = counter_value.bytes;
  stat_data.packets = counter_value.packets;
  p4_pd_status_t status = pipe_mgr_stat_ent_set(sess_hdl,
						pipe_mgr_dev_tgt,
						${counter_hdl},
						index,
						&stat_data);
  return status;
}

//::   #endif
//:: #endfor

//:: for counter, c_info in counter_info.items():
//::   name = "p4_pd_" + p4_prefix + "_counter_hw_sync_" + counter
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_stat_sync_cb cb_fn,
 void *cb_cookie
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
//::   binding = c_info["binding"]
//::   type_ = c_info["type_"]
//::   if binding[0] == "direct":
//::     table_hdl, _, _ = table_handles[binding[1]]
  return pipe_mgr_direct_stat_database_sync(sess_hdl,
					    pipe_mgr_dev_tgt,
					    ${table_hdl},
					    cb_fn, // legit cast
					    cb_cookie);
//::   else:
//::     counter_hdl = counter_handles[counter]
  return pipe_mgr_stat_database_sync(sess_hdl,
				     pipe_mgr_dev_tgt,
				     ${counter_hdl},
				     cb_fn, // legit cast
				     cb_cookie);
//::   #endif
}

//:: #endfor

/* METERS */

//:: for meter, m_info in meter_info.items():
//::   binding = m_info["binding"]
//::   type_ = m_info["type_"]
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt"]
//::   if binding[0] == "direct":
//::     params += ["p4_pd_entry_hdl_t entry_hdl"]
//::   else:
//::     params += ["int index"]
//::   #endif
//::   if type_ == "packets":
//::     params += ["p4_pd_packets_meter_spec_t *meter_spec"]
//::   else:
//::     params += ["p4_pd_bytes_meter_spec_t *meter_spec"]
//::   #endif
//::   param_str = ",\n ".join(params)
//::   name = "p4_pd_" + p4_prefix + "_meter_set_" + meter
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_meter_spec_t *pipe_meter_spec;
//::   meter_hdl = meter_handles[meter]
//::   if binding[0] == "direct":
  pipe_res_spec_t res_spec;
  pipe_meter_spec = &res_spec.data.meter;
  res_spec.tbl_hdl = ${meter_hdl};
  res_spec.tag = PIPE_RES_ACTION_TAG_ATTACHED;
//::   else:
  pipe_meter_spec_t pipe_meter_spec_;
  pipe_meter_spec = &pipe_meter_spec_;
//::   #endif
//::   has_pre_color = p4_meter_has_pre_color(meter, meter_info)
//::   if type_ == "packets":
  packets_meter_spec_pd_to_pipe(meter_spec, pipe_meter_spec, ${has_pre_color});
//::   else:
  bytes_meter_spec_pd_to_pipe(meter_spec, pipe_meter_spec, ${has_pre_color});
//::   #endif
//::   if binding[0] == "direct":
//::     table_hdl, _, _ = table_handles[binding[1]]
  return pipe_mgr_mat_ent_set_resource(sess_hdl, dev_tgt.device_id,
				       ${table_hdl}, entry_hdl,
				       &res_spec, 1 /* resource count */,
				       0);
//::   else:
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_mgr_meter_ent_set(sess_hdl,
				pipe_mgr_dev_tgt,
				${meter_hdl},
				index,
				pipe_meter_spec,
				0);
//::   #endif			    
}

//::   name = "p4_pd_" + p4_prefix + "_meter_bytecount_adjust_set_" + meter
p4_pd_status_t
${name}
(
p4_pd_sess_hdl_t sess_hdl,
p4_pd_dev_target_t dev_tgt,
int bytecount
)
{ 
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
    return pipe_mgr_meter_set_bytecount_adjust(sess_hdl,
                                pipe_mgr_dev_tgt,
                                ${meter_hdl}, bytecount);
}

//::   name = "p4_pd_" + p4_prefix + "_meter_bytecount_adjust_get_" + meter
p4_pd_status_t
${name}
(
p4_pd_sess_hdl_t sess_hdl,
p4_pd_dev_target_t dev_tgt,
int *bytecount
)
{
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
    return pipe_mgr_meter_get_bytecount_adjust(sess_hdl,
                                pipe_mgr_dev_tgt,
                                ${meter_hdl}, bytecount);
}

//::   name = "p4_pd_" + p4_prefix + "_meter_read_" + meter
p4_pd_status_t
${name}
(
 ${param_str}
)
{
    pipe_meter_spec_t pipe_meter_spec = {0};
    pipe_status_t ret = PIPE_SUCCESS;
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
//::   if binding[0] == "direct":
//::     table_hdl, _, _ = table_handles[binding[1]]
    ret = pipe_mgr_meter_read_entry(sess_hdl, pipe_mgr_dev_tgt,
                                    ${table_hdl}, entry_hdl,
                                    &pipe_meter_spec);
//::   else:
    ret = pipe_mgr_meter_read_entry_idx(sess_hdl, pipe_mgr_dev_tgt,
                                        ${meter_hdl}, index,
                                        &pipe_meter_spec,
                                        false);
//::   #endif
    if (ret != PIPE_SUCCESS) {
        return ret;
    }
//::   if type_ == "packets":
    packets_meter_spec_pipe_to_pd(meter_spec, &pipe_meter_spec);
//::   else:
    bytes_meter_spec_pipe_to_pd(meter_spec, &pipe_meter_spec);
//::   #endif

    return PIPE_SUCCESS;
}
//:: #endfor

/* LPF */
//:: for lpf, m_info in lpf_info.items():
//::   binding = m_info["binding"]
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt"]
//::   if binding[0] == "direct":
//::     params += ["p4_pd_entry_hdl_t entry_hdl"]
//::   else:
//::     params += ["int index"]
//::   #endif
//::   params += ["p4_pd_lpf_spec_t *lpf_spec"]
//::   param_str = ",\n ".join(params)
//::   name = "p4_pd_" + p4_prefix + "_lpf_set_" + lpf
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_lpf_spec_t *pipe_lpf_spec;
//::   # this is not a typo, lpf handles are still stored with meter handles
//::   lpf_hdl = meter_handles[lpf]
//::   if binding[0] == "direct":
  pipe_res_spec_t res_spec;
  pipe_lpf_spec = &res_spec.data.lpf;
  res_spec.tbl_hdl = ${lpf_hdl};
  res_spec.tag = PIPE_RES_ACTION_TAG_ATTACHED;
//::   else:
  pipe_lpf_spec_t pipe_lpf_spec_;
  pipe_lpf_spec = &pipe_lpf_spec_;
//::   #endif
  lpf_spec_pd_to_pipe(lpf_spec, pipe_lpf_spec);
//::   if binding[0] == "direct":
//::     table_hdl, _, _ = table_handles[binding[1]]
  return pipe_mgr_mat_ent_set_resource(sess_hdl, dev_tgt.device_id,
				       ${table_hdl}, entry_hdl,
				       &res_spec, 1 /* resource count */,
				       0);
//::   else:
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_mgr_lpf_ent_set(sess_hdl,
                              pipe_mgr_dev_tgt,
                              ${lpf_hdl},
                              index,
                              pipe_lpf_spec,
                              0);
//::   #endif
}

//::   name = "p4_pd_" + p4_prefix + "_lpf_read_" + lpf
p4_pd_status_t
${name}
(
 ${param_str}
)
{
    pipe_lpf_spec_t pipe_lpf_spec = {0};
    pipe_status_t ret = PIPE_SUCCESS;
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
//::   if binding[0] == "direct":
//::     table_hdl, _, _ = table_handles[binding[1]]
    ret = pipe_mgr_lpf_read_entry(sess_hdl, pipe_mgr_dev_tgt,
                                  ${table_hdl}, entry_hdl,
                                  &pipe_lpf_spec);
//::   else:
    ret = pipe_mgr_lpf_read_entry_idx(sess_hdl, pipe_mgr_dev_tgt,
                                      ${lpf_hdl}, index,
                                      &pipe_lpf_spec,
                                      false);
//::   #endif
    if (ret != PIPE_SUCCESS) {
        return ret;
    }
    lpf_spec_pipe_to_pd(lpf_spec, &pipe_lpf_spec);

    return PIPE_SUCCESS;
}
//:: #endfor
//
/* WRED */
//:: for wred, w_info in wred_info.items():
//::   binding = w_info["binding"]
//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "p4_pd_dev_target_t dev_tgt"]
//::   if binding[0] == "direct":
//::     params += ["p4_pd_entry_hdl_t entry_hdl"]
//::   else:
//::     params += ["int index"]
//::   #endif
//::   params += ["p4_pd_wred_spec_t *wred_spec"]
//::   param_str = ",\n ".join(params)
//::   name = "p4_pd_" + p4_prefix + "_wred_set_" + wred
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_wred_spec_t *pipe_red_spec;
//::   # this is not a typo, wred handles are still stored with meter handles
//::   red_hdl = meter_handles[wred]
//::   if binding[0] == "direct":
  pipe_res_spec_t res_spec;
  pipe_red_spec = &res_spec.data.red;
  res_spec.tbl_hdl = ${red_hdl};
  res_spec.tag = PIPE_RES_ACTION_TAG_ATTACHED;
//::   else:
  pipe_wred_spec_t pipe_red_spec_;
  pipe_red_spec = &pipe_red_spec_;
//::   #endif
  wred_spec_pd_to_pipe(wred_spec, pipe_red_spec);
//::   if binding[0] == "direct":
//::     table_hdl, _, _ = table_handles[binding[1]]
  return pipe_mgr_mat_ent_set_resource(sess_hdl, dev_tgt.device_id,
				       ${table_hdl}, entry_hdl,
				       &res_spec, 1 /* resource count */,
				       0);
//::   else:
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_mgr_wred_ent_set(sess_hdl,
                               pipe_mgr_dev_tgt,
                               ${red_hdl},
                               index,
                               pipe_red_spec,
                               0);
//::   #endif
}

//::   name = "p4_pd_" + p4_prefix + "_wred_read_" + wred
p4_pd_status_t
${name}
(
 ${param_str}
)
{
    pipe_wred_spec_t pipe_wred_spec = {0};
    pipe_status_t ret = PIPE_SUCCESS;
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
//::   if binding[0] == "direct":
//::     table_hdl, _, _ = table_handles[binding[1]]
    ret = pipe_mgr_wred_read_entry(sess_hdl, pipe_mgr_dev_tgt,
                                   ${table_hdl}, entry_hdl,
                                   &pipe_wred_spec);
//::   else:
    ret = pipe_mgr_wred_read_entry_idx(sess_hdl, pipe_mgr_dev_tgt,
                                       ${red_hdl}, index,
                                       &pipe_wred_spec,
                                       false);
//::   #endif
    if (ret != PIPE_SUCCESS) {
        return ret;
    }
    wred_spec_pipe_to_pd(wred_spec, &pipe_wred_spec);

    return PIPE_SUCCESS;
}
//:: #endfor

// REGISTERS

//:: for register, r_info in register_info.items():
//::   register_hdl = register_handles[register]
//::   binding = r_info["binding"]
//::   table_type = r_info["table_type"]
//::   if table_type == "fifo":
//::     name = "p4_pd_" + p4_prefix + "_register_occupancy_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int *occupancy
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_stful_fifo_occupancy(sess_hdl, pipe_mgr_dev_tgt, ${register_hdl}, occupancy);
}

//::     name = "p4_pd_" + p4_prefix + "_register_reset_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  return pipe_stful_fifo_reset(sess_hdl, pipe_mgr_dev_tgt, ${register_hdl});
}

//::     if r_info['direction'] == "in":
//::       name = "p4_pd_" + p4_prefix + "_register_dequeue_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int num_to_dequeue,
 ${r_info["v_type"]} *register_values,
 int *num_dequeued
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  pipe_stful_mem_spec_t *pipe_reg_vals;
  pipe_reg_vals = bf_sys_malloc(num_to_dequeue * sizeof *pipe_reg_vals);
  if (!pipe_reg_vals) return 1;

  p4_pd_status_t status = pipe_stful_fifo_dequeue(sess_hdl, pipe_mgr_dev_tgt, ${register_hdl}, num_to_dequeue, pipe_reg_vals, num_dequeued);
  if (0 == status) {
    for (int i=0; i<*num_dequeued; ++i) {
      register_${register}_value_pipe_to_pd(pipe_reg_vals + i, register_values + i);
    }
  }
  bf_sys_free(pipe_reg_vals);
  return status;
}

//::     #endif
//::     if r_info['direction'] == "out":
//::       name = "p4_pd_" + p4_prefix + "_register_enqueue_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int num_to_enqueue,
 ${r_info["v_type"]} *register_values
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  pipe_stful_mem_spec_t *pipe_reg_vals;
  pipe_reg_vals = bf_sys_malloc(num_to_enqueue * sizeof *pipe_reg_vals);
  if (!pipe_reg_vals) return 1;

  for (int i=0; i<num_to_enqueue; ++i) {
    register_${register}_value_pd_to_pipe(register_values + i, pipe_reg_vals + i);
  }

  p4_pd_status_t status = pipe_stful_fifo_enqueue(sess_hdl, pipe_mgr_dev_tgt, ${register_hdl}, num_to_enqueue, pipe_reg_vals);
  bf_sys_free(pipe_reg_vals);
  return status;
}

//::     #endif
//::   else:
//::     name = "p4_pd_" + p4_prefix + "_register_hw_sync_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_register_sync_cb cb_fn,
 void *cb_cookie
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
//::     binding = r_info["binding"]
//::     if binding[0] == "direct":
//::       table_hdl, _, _ = table_handles[binding[1]]
  return pipe_stful_direct_database_sync(sess_hdl, pipe_mgr_dev_tgt,
					 ${table_hdl},
					 cb_fn, // legit cast
					 cb_cookie);
//::     else:
//::       register_hdl = register_handles[register]
  return pipe_stful_database_sync(sess_hdl, pipe_mgr_dev_tgt,
				  ${register_hdl},
				  cb_fn, // legit cast
				  cb_cookie);
//::     #endif
}
//::     if binding[0] == "direct":
//::       table_hdl, _, _ = table_handles[binding[1]]
//::       name = "p4_pd_" + p4_prefix + "_register_read_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 p4_pd_entry_hdl_t entry_hdl,
 int flags,
 ${r_info["v_type"]} *register_values,
 int *value_count
)
{
  p4_pd_status_t status;

  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  uint32_t pipe_api_flags = flags & REGISTER_READ_HW_SYNC ?
                            PIPE_FLAG_SYNC_REQ : 0;
  /* Get the maximum number of elements the query can return. */
  int pipe_count;
  status = pipe_stful_direct_query_get_sizes(sess_hdl,
                                             dev_tgt.device_id,
                                             ${table_hdl},
                                             &pipe_count);
  if(status != PIPE_MGR_SUCCESS) return status;
  /* Allocate space for the query results. */
  pipe_stful_mem_query_t stful_query;
  stful_query.pipe_count = pipe_count;
  stful_query.data = bf_sys_calloc(pipe_count, sizeof *stful_query.data);
  if (!stful_query.data) return PIPE_NO_SYS_RESOURCES;
  /* Perform the query. */
  status = pipe_stful_direct_ent_query(sess_hdl, dev_tgt.device_id,
                                       ${table_hdl}, entry_hdl,
                                       &stful_query, pipe_api_flags);
  if(status != PIPE_MGR_SUCCESS) goto free_query_data;
  /* Convert the query data to PD format. */
  *value_count = 0;
  register_${register}_value_pipe_to_pd(&stful_query, register_values, value_count);
  /* Free the space allocated for the pipe-mgr query data. */
  bf_sys_free(stful_query.data);
  return 0;
free_query_data:
  bf_sys_free(stful_query.data);
  return status;
}

//::       name = "p4_pd_" + p4_prefix + "_register_write_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 bf_dev_id_t dev_id,
 p4_pd_entry_hdl_t entry_hdl,
 ${r_info["v_type"]} *register_value
)
{
  pipe_res_spec_t res_spec = {0};
  res_spec.tbl_hdl = ${register_hdl};
  res_spec.tag = PIPE_RES_ACTION_TAG_ATTACHED;
  register_${register}_value_pd_to_pipe(register_value, &res_spec.data.stful);
  p4_pd_status_t status = pipe_mgr_mat_ent_set_resource(sess_hdl, dev_id,
      ${table_hdl}, entry_hdl, &res_spec, 1, 0);

  return status;
}

//::     else:
//::       name = "p4_pd_" + p4_prefix + "_register_read_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int index,
 int flags,
 ${r_info["v_type"]} *register_values,
 int *value_count
)
{
  p4_pd_status_t status;
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  uint32_t pipe_api_flags = flags & REGISTER_READ_HW_SYNC ?
                            PIPE_FLAG_SYNC_REQ : 0;
  /* Get the maximum number of elements the query can return. */
  int pipe_count;
  status = pipe_stful_query_get_sizes(sess_hdl,
                                      dev_tgt.device_id,
                                      ${register_hdl},
                                      &pipe_count);
  if(status != PIPE_MGR_SUCCESS) return status;
  /* Allocate space for the query results. */
  pipe_stful_mem_query_t stful_query;
  stful_query.pipe_count = pipe_count;
  stful_query.data = bf_sys_calloc(pipe_count, sizeof *stful_query.data);
  if (!stful_query.data) return PIPE_NO_SYS_RESOURCES;
  /* Perform the query. */
  status = pipe_stful_ent_query(sess_hdl, pipe_mgr_dev_tgt,
                                ${register_hdl}, index,
                                &stful_query, pipe_api_flags);
  if(status != PIPE_MGR_SUCCESS) goto free_query_data;
  /* Convert the query data to PD format. */
  *value_count = 0;
  register_${register}_value_pipe_to_pd(&stful_query, register_values, value_count);
  /* Free the space allocated for the pipe-mgr query data. */
  bf_sys_free(stful_query.data);
  return 0;
free_query_data:
  bf_sys_free(stful_query.data);
  return status;
}

//::       name = "p4_pd_" + p4_prefix + "_register_write_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int index,
 ${r_info["v_type"]} *register_value
)
{
  pipe_stful_mem_spec_t stful_spec;
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;
  register_${register}_value_pd_to_pipe(register_value, &stful_spec);

  p4_pd_status_t status = pipe_stful_ent_set(sess_hdl, pipe_mgr_dev_tgt,
					     ${register_hdl}, index,
					     &stful_spec, 0);
  return status;
}

//::       name = "p4_pd_" + p4_prefix + "_register_reset_all_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

//::       register_hdl = register_handles[register]
  return pipe_stful_table_reset(sess_hdl, pipe_mgr_dev_tgt, ${register_hdl},
                                NULL);
}

//::       name = "p4_pd_" + p4_prefix + "_register_write_all_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 ${r_info["v_type"]} *register_value
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  pipe_stful_mem_spec_t stful_spec;
  register_${register}_value_pd_to_pipe(register_value, &stful_spec);
//::       register_hdl = register_handles[register]
  return pipe_stful_table_reset(sess_hdl, pipe_mgr_dev_tgt, ${register_hdl},
                                &stful_spec);
}

//::       name = "p4_pd_" + p4_prefix + "_register_range_reset_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int index,
 int count
)
{
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

//::       register_hdl = register_handles[register]
  return pipe_stful_table_reset_range(sess_hdl, pipe_mgr_dev_tgt, ${register_hdl},
                                      index, count, NULL);
}

//::       name = "p4_pd_" + p4_prefix + "_register_range_read_" + register
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t sess_hdl,
 p4_pd_dev_target_t dev_tgt,
 int index,
 int count,
 int flags,
 int *num_actually_read,
 ${r_info["v_type"]} *register_values,
 int *value_count
)
{
  p4_pd_status_t status;
  dev_target_t pipe_mgr_dev_tgt;
  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  uint32_t pipe_api_flags = flags & REGISTER_READ_HW_SYNC ?
                            PIPE_FLAG_SYNC_REQ : 0;
  /* Get the maximum number of elements the query can return. */
  int pipe_count;
  status = pipe_stful_query_get_sizes(sess_hdl,
                                      dev_tgt.device_id,
                                      ${register_hdl},
                                      &pipe_count);
  if(status != PIPE_MGR_SUCCESS) return status;
  /* Allocate space for the query results. */
  pipe_stful_mem_query_t *stful_query = bf_sys_calloc(count, sizeof *stful_query);
  pipe_stful_mem_spec_t *pipe_data = bf_sys_calloc(pipe_count * count, sizeof *pipe_data);
  if (!stful_query || !pipe_data) {
    status = PIPE_NO_SYS_RESOURCES;
    goto free_query_data;
  }

  for (int j=0; j<count; ++j) {
    stful_query[j].pipe_count = pipe_count;
    stful_query[j].data = pipe_data + (pipe_count * j);
  }

  /* Perform the query. */
  status = pipe_stful_ent_query_range(sess_hdl, pipe_mgr_dev_tgt,
                                      ${register_hdl}, index, count,
                                      stful_query, num_actually_read,
                                      pipe_api_flags);
  if(status != PIPE_MGR_SUCCESS) goto free_query_data;

  /* Convert the query data to PD format. */
  *value_count = 0;
  for (int i=0; i<*num_actually_read; ++i) {
    register_${register}_value_pipe_to_pd(stful_query+i, register_values, value_count);
    register_values += stful_query[i].pipe_count;
  }

free_query_data:
  if (stful_query) bf_sys_free(stful_query);
  if (pipe_data) bf_sys_free(pipe_data);
  return status;
}
//::     #endif
//::   #endif
//:: #endfor


//:: if gen_exm_test_pd == 1:
/* Activate exact match entry */

//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     match_type = t_info["match_type"]
//::     name = p4_pd_prefix + table + "_activate_entry"
//::     params = ["p4_pd_sess_hdl_t sess_hdl",
//::               "uint8_t device_id",
//::               "p4_pd_entry_hdl_t  entry_hdl"]
//::     param_str = ",\n ".join(params)
/**
 * @brief ${name}
 * @param sess_hdl
 * @param device_id
 * @param entry_hdl
*/
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     if match_type != "exact":
  /* This is only supported for exact match tables */
  (void)sess_hdl;
  (void)device_id;
  (void)entry_hdl;
  return PIPE_SUCCESS;
}
//::       continue
//::     #endif
  return pipe_mgr_exm_entry_activate(sess_hdl, device_id,
                                     ${table_hdl}, entry_hdl);
}
//::   #endfor

/* De-Activate exact match entry */

//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     match_type = t_info["match_type"]
//::     name = p4_pd_prefix + table + "_deactivate_entry"
//::     params = ["p4_pd_sess_hdl_t sess_hdl",
//::               "uint8_t device_id",
//::               "p4_pd_entry_hdl_t  entry_hdl"]
//::     param_str = ",\n ".join(params)
/**
 * @brief ${name}
 * @param sess_hdl
 * @param dev_tgt
 * @param entry_hdl
*/
p4_pd_status_t
${name}
(
 ${param_str}
)
{
//::     if match_type != "exact":
  /* This is only supported for exact match tables */
  (void)sess_hdl;
  (void)device_id;
  (void)entry_hdl;
  return PIPE_SUCCESS;
}
//::       continue
//::     #endif
  return pipe_mgr_exm_entry_deactivate(sess_hdl, device_id,
                                       ${table_hdl}, entry_hdl);
}
//::   #endfor
//:: #endif


p4_pd_status_t
${p4_pd_prefix}snapshot_capture_trigger_set(
              pipe_snapshot_hdl_t hdl,
              ${p4_pd_prefix}snapshot_trig_spec_t *trig_spec,
              ${p4_pd_prefix}snapshot_trig_spec_t *trig_mask)
{
  int                                 dir = 0;
  ${p4_pd_prefix}snapshot_trig_spec_t pd_trig_spec;
  ${p4_pd_prefix}snapshot_trig_spec_t pd_trig_mask;

  memcpy(&pd_trig_spec, trig_spec, sizeof(pd_trig_spec));
  memcpy(&pd_trig_mask, trig_mask, sizeof(pd_trig_mask));

  dir = (hdl >> 1) & 0x1;  // dir is bit 1 of hdl
   
  /* Take care of endianness */ 
//:: dir_str = ["ig", "eg"]
    switch (dir) 
    {
//:: for dir in range(0,2):
       case ${dir}:
       {
//::   for name in PHV_Container_Fields[dir]:
//::     width = PHV_Container_Fields[dir][name]
//::     if (width == 1) or (width > 4):
//::       continue
//::     #endif
#ifdef LITTLE_ENDIAN_CALLER
//::     if width == 2:
          pd_trig_spec.u.${dir_str[dir]}.${name} = htons(pd_trig_spec.u.${dir_str[dir]}.${name});
          pd_trig_mask.u.${dir_str[dir]}.${name} = htons(pd_trig_mask.u.${dir_str[dir]}.${name});
//::     elif width == 3 or width == 4:
          pd_trig_spec.u.${dir_str[dir]}.${name} = htonl(pd_trig_spec.u.${dir_str[dir]}.${name});
          pd_trig_mask.u.${dir_str[dir]}.${name} = htonl(pd_trig_mask.u.${dir_str[dir]}.${name});
//::     #endif
#endif
//::   #endfor
         break;
       
       }
//:: #endfor
       default:  
       {
         break;
       }
  }  

  return bf_snapshot_capture_trigger_set(hdl, (void*)&pd_trig_spec, 
                                           (void*)&pd_trig_mask);
}

p4_pd_status_t
${p4_pd_prefix}snapshot_capture_data_get(
              pipe_snapshot_hdl_t hdl,
              bf_dev_pipe_t dev_pipe_id,
              ${p4_pd_prefix}snapshot_capture_arr_t *capture_arr,
              bf_snapshot_capture_ctrl_info_arr_t *capture_ctrl_arr,
              int *num_captures)
{
  p4_pd_status_t status = 0;
  uint32_t data_size = 0, stage_size = 0;
  int i = 0;
  
  *num_captures = 0;
 
  /* Allocate memory for snapshot capture data */
  bf_snapshot_capture_phv_fields_dict_size(hdl, &data_size, &stage_size);
  uint8_t *capture = bf_sys_calloc(1, data_size);
  if (!capture) {
    return PIPE_NO_SYS_RESOURCES;
  }

  /* Call the Snapshot get API */
  status = bf_snapshot_capture_get(hdl, dev_pipe_id, capture, capture_ctrl_arr, num_captures);

  /* Copy the snapshot data into the user memory. User memory is a PD structure */
  for (i=0; (i < BF_MAX_SNAPSHOT_CAPTURES) && (i < data_size/stage_size) ; i++) { 
    memcpy(&capture_arr->captures[i], capture + i*stage_size, sizeof(${p4_pd_prefix}snapshot_capture_t));
  }

  bf_sys_free(capture);

  return status;
}

p4_pd_status_t
${p4_pd_prefix}snapshot_create(
            p4_pd_dev_target_t dev_tgt,
            uint8_t start_stage_id, uint8_t end_stage_id,
            bf_snapshot_dir_t direction,
            pipe_snapshot_hdl_t *hdl)
{
  bf_dev_id_t device_id = dev_tgt.device_id;
  bf_dev_pipe_t dev_pipe_id = (dev_tgt.dev_pipe_id == PD_DEV_PIPE_ALL) ?
                                   DEV_PIPE_ALL : dev_tgt.dev_pipe_id;
  return bf_snapshot_create(device_id,
            dev_pipe_id, start_stage_id, end_stage_id, direction, hdl);
}

p4_pd_status_t
${p4_pd_prefix}snapshot_delete(
            pipe_snapshot_hdl_t hdl)
{
  return bf_snapshot_delete(hdl); 
}

//::  if gen_perf_test_pd == 1:
typedef struct stats_ent_dump_info_ {
    p4_pd_sess_hdl_t sess_hdl;
    p4_pd_dev_target_t dev_tgt;
    p4_pd_entry_hdl_t *entry_hdls;
    int num_entry_hdls;
} stats_ent_dump_info_t;

int num_threads;
#define MAX_THREADS 8
bf_sys_thread_t thread_ids[MAX_THREADS];
bf_sys_mutex_t global_thread_start_mtx;
bf_sys_mutex_t global_thread_stop_mtx;
bf_sys_cond_t global_thread_start_condvar;
bool global_thread_start;
bool global_thread_stop;

void
${p4_pd_prefix}_threads_init()
{
    bf_sys_mutex_init(&global_thread_start_mtx);
    bf_sys_mutex_init(&global_thread_stop_mtx);
    bf_sys_cond_init(&global_thread_start_condvar);
}

void
${p4_pd_prefix}_threads_start()
{
    bf_sys_mutex_lock(&global_thread_start_mtx);
    global_thread_start = true;
    bf_sys_cond_broadcast(&global_thread_start_condvar);
    bf_sys_mutex_unlock(&global_thread_start_mtx);
    return;
}

void
${p4_pd_prefix}_threads_stop()
{
    bf_sys_mutex_lock(&global_thread_stop_mtx);
    global_thread_stop = true;
    bf_sys_mutex_unlock(&global_thread_stop_mtx);
    unsigned i = 0;
    for (i = 0; i < num_threads; i++) {
        bf_sys_thread_join(&thread_ids[i], NULL);
    }
    num_threads = 0;
    return;
}

//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     match_type = t_info["match_type"]
//::     has_match_spec = len(t_info["match_fields"]) > 0
//::     match_type = t_info["match_type"]
//::     if not has_match_spec:
//::       continue
//::     #endif
//::     params = ["p4_pd_sess_hdl_t sess_hdl",
//::               "p4_pd_dev_target_t dev_tgt"]
//::     params += ["int num_entries"]
//::     param_str = ",\n ".join(params)
//::     name = p4_pd_prefix + table + "_table_bulk_init"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
    if (${p4_pd_prefix}${table}_bulk_p) {
        return PIPE_NOT_SUPPORTED;
    }

    ${p4_pd_prefix}${table}_bulk_t* perf_test = NULL;

    perf_test = bf_sys_calloc(1, sizeof(${p4_pd_prefix}${table}_bulk_t));
    perf_test->operations = bf_sys_calloc(num_entries,
                                   sizeof(${p4_pd_prefix}${table}_bulk_op_t));

    perf_test->sess_hdl = sess_hdl;
    perf_test->device_id = dev_tgt.device_id;
    perf_test->dev_pipe_id = dev_tgt.dev_pipe_id;
//::     meter_spec_present = False
//::     stful_spec_present = False
//::     lpf_spec_present = False
//::     wred_spec_present = False
//::
//::     for res_name, res_type, _ in table_direct_resources[table]:
//::       if res_type == "bytes_meter":
//::         meter_spec_present = True;
//::       elif res_type == "packets_meter":
//::         meter_spec_present = True;
//::       elif res_type == "register":
//::         stful_spec_present = True
//::       elif res_type == "lpf":
//::         lpf_spec_present = True
//::       elif res_type == "wred":
//::         wred_spec_present = True
//::       #endif
//::     #endfor
//::     if action_table_hdl:
     perf_test->action_spec_present = false;
//::     else:
     perf_test->action_spec_present = true;
//::     #endif

//::     if meter_spec_present:
     perf_test->meter_spec_present = true;
//::      if res_type == "bytes_meter":
     perf_test->byte_meter = true;
//::      elif rest_type == "packets_meter":
     perf_test->packet_meter = true;
//::      #endif
//::     #endif

//::     if stful_spec_present:
     perf_test->stful_spec_present = true;
//::     #endif

//::     if lpf_spec_present:
     perf_test->lpf_spec_present = true;
//::     #endif

//::     if wred_spec_present:
     perf_test->wred_spec_present = true;
//::     #endif

//::     if t_info["timeout"]:
     perf_test->idle_present = true;
//::     #endif

//::     if match_type == "ternary":
     perf_test->exm = false;
//::     else:
     perf_test->exm = true;
//::     #endif

     ${p4_pd_prefix}${table}_bulk_p = perf_test;

    return PIPE_SUCCESS;
}
//::   #endfor

//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     if action_table_hdl: continue
//::     match_type = t_info["match_type"]
//::     has_match_spec = len(t_info["match_fields"]) > 0
//::     if has_match_spec == 0: continue
//::     for action in t_info["actions"]:
//::       a_info = action_info[action]
//::       has_action_spec = len(a_info["param_names"]) > 0
//::       if not has_action_spec: continue
//::       params = ["p4_pd_sess_hdl_t sess_hdl",
//::                 "p4_pd_dev_target_t dev_tgt"]
//::       if has_match_spec:
//::         params += ["const " + p4_pd_prefix + table + "_match_spec_t * const match_spec"]
//::         match_spec_type = p4_pd_prefix + table + "_match_spec_t"
//::       #endif
//::       if match_type == "ternary" or match_type == "range":
//::         params += ["int priority"]
//::       #endif
//::       if has_action_spec:
//::         params += ["const " + p4_pd_prefix + action + "_action_spec_t * const action_spec"]
//::         action_spec_type = p4_pd_prefix + action + "_action_spec_t"
//::       #endif
//::       if t_info["timeout"]:
//::         params += ["uint32_t ttl"]
//::       #endif
//::       params += get_direct_parameter_specs(table_direct_resources[table], register_info)
//::       specs = build_direct_parameter_spec_dictionary(table_direct_resources[table])
//::       param_str = ",\n ".join(params)
//::       name = p4_pd_prefix + table + "_table_add_with_" + action + "_bulk_setup"
//::       add_fn_name = p4_pd_prefix + table + "_table_add_with_" + action
//::       del_fn_name = p4_pd_prefix + table + "_table_delete"
p4_pd_status_t
${name}
(
 ${param_str}
)
{
    ${p4_pd_prefix}${table}_bulk_t* perf_test =
        (${p4_pd_prefix}${table}_bulk_t *) ${p4_pd_prefix}${table}_bulk_p;

    if (sess_hdl != perf_test->sess_hdl) {
        bf_sys_dbgchk(0);
        return BF_INVALID_ARG;
    }
    if (dev_tgt.device_id != perf_test->device_id) {
        bf_sys_dbgchk(0);
        return BF_INVALID_ARG;
    }
    if (dev_tgt.dev_pipe_id != perf_test->dev_pipe_id) {
        bf_sys_dbgchk(0);
        return BF_INVALID_ARG;
    }
    uint32_t num_entries = perf_test->num_ops;
    perf_test->operations[num_entries].operation = BULK_OPERATION_ADD;
    perf_test->operations[num_entries].add_fn_ptr = ${add_fn_name};
    perf_test->operations[num_entries].delete_fn_ptr = ${del_fn_name};
    perf_test->operations[num_entries].match_spec = bf_sys_malloc(sizeof(${match_spec_type}));
    memcpy(perf_test->operations[num_entries].match_spec, 
           match_spec, sizeof(${match_spec_type}));
//::   if has_action_spec:
    perf_test->operations[num_entries].action_spec =
        bf_sys_malloc(sizeof(${action_spec_type}));

    memcpy(perf_test->operations[num_entries].action_spec,
           action_spec, sizeof(${action_spec_type}));
//::   #endif

//::       if "bytes_meter" in specs:
    perf_test->operations[num_entries].meter_spec =
        bf_sys_malloc(sizeof(${specs["bytes_meter"]}));
    memcpy(perf_test->operations[num_entries].meter_spec, ${specs["bytes_meter"]}
           sizeof(${specs["bytes_meter"]}));
//::       elif "packet_meter" in specs:
    perf_test->operations[num_entries].meter_spec =
        bf_sys_malloc(sizeof(${specs["packets_meter"]}));
    memcpy(perf_test->operations[num_entries].meter_spec, ${specs["packets_meter"]},
           sizeof(${specs["packets_meter"]}));
//::       #endif

//::       if "stful_spec" in specs:
    perf_test->operations[num_entries].stful_spec =
        bf_sys_malloc(sizeof(${specs["register"]}));
    memcpy(perf_test->operations[num_entries].stful_spec,  ${specs["register"]},
           sizeof(${specs["register"]}));
//::       #endif

//::       if "lpf_spec" in specs:
    perf_test->operations[num_entries].lpf_spec =
        bf_sys_malloc(sizeof(${specs["lpf"]}));
    memcpy(perf_test->operations[num_entries].lpf_spec, ${specs["lpf"]},
           sizeof(${specs["lpf"]}));
//::       #endif

//::       if "wred_spec" in specs:
    perf_test->operations[num_entries].wred_spec =
        bf_sys_malloc(sizeof(${specs["wred"]}));
    memcpy(perf_test->operations[num_entries].wred_spec, ${specs["wred"]},
              sizeof(${specs["wred"]}));
//::       #endif

//::       if t_info["timeout"]:
    perf_test->operations[num_entries].ttl = ttl;
//::       #endif

//::       if match_type == "ternary" or match_type == "range":
    perf_test->operations[num_entries].priority = priority;
//::       #endif
//::     fn_name = p4_pd_prefix + table + "_table_add_with_" + action

    perf_test->num_ops++;

   return PIPE_SUCCESS;
}
//::     #endfor

//::     name = p4_pd_prefix + table + "_table_perf_test_execute"
int
${name} ()
{
    ${p4_pd_prefix}${table}_bulk_t* perf_test =
        (${p4_pd_prefix}${table}_bulk_t *) ${p4_pd_prefix}${table}_bulk_p;
    unsigned i = 0;
    int rate = 0;
    p4_pd_status_t pd_status = PIPE_SUCCESS;
    p4_pd_entry_hdl_t *entry_hdls = bf_sys_malloc(sizeof(p4_pd_entry_hdl_t)*perf_test->num_ops);
    if (entry_hdls == NULL) {
      bf_sys_dbgchk(0);
      return -1;
    }
    p4_pd_dev_target_t dev_tgt;
    dev_tgt.device_id = perf_test->device_id;
    dev_tgt.dev_pipe_id = perf_test->dev_pipe_id;
    struct timespec start={0}, end={0}, diff={0};
    clock_gettime(CLOCK_MONOTONIC, &start);
#ifdef PERF_TEST_PROFILING
    ProfilerStart("./perf_data.txt");
#endif
    pipe_mgr_begin_batch(perf_test->sess_hdl);
    for (i = 0; i < perf_test->num_ops; i++) {
        pd_status = perf_test->operations[i].add_fn_ptr(
                perf_test->sess_hdl,
                dev_tgt,
                perf_test->operations[i].match_spec,
                perf_test->operations[i].action_spec,
//::     if match_type == "ternary" or match_type == "range":
                perf_test->operations[i].priority,
//::     #endif
//::     if t_info["timeout"]:
                perf_test->operations[i].ttl,
//::     #endif
                &entry_hdls[i]);
        bf_sys_dbgchk(pd_status == PIPE_SUCCESS);
    }
    pipe_mgr_flush_batch(perf_test->sess_hdl);
#ifdef PERF_TEST_PROFILING
    ProfilerFlush();
    ProfilerStop();
#endif
    clock_gettime(CLOCK_MONOTONIC, &end);
    for (i = 0; i < perf_test->num_ops; i++) {
      pipe_mgr_mat_ent_del(perf_test->sess_hdl, dev_tgt.device_id, ${table_hdl}, entry_hdls[i], 0);
    }
    pipe_mgr_end_batch(perf_test->sess_hdl, false);
    diff.tv_sec = end.tv_sec - start.tv_sec;
    diff.tv_nsec = end.tv_nsec - start.tv_nsec;
    if (diff.tv_nsec < 0) {
      diff.tv_sec -= 1;
      diff.tv_nsec += 1000000000;
    }
    uint64_t microseconds = diff.tv_sec * 1000000 + diff.tv_nsec/1000;
    double ops_per_microsecond = (double)perf_test->num_ops / microseconds;
    rate = ops_per_microsecond * 1000000;
    for (i=0; i < perf_test->num_ops; ++i) {
      if (perf_test->operations[i].match_spec)
        bf_sys_free(perf_test->operations[i].match_spec);
      if (perf_test->operations[i].action_spec)
        bf_sys_free(perf_test->operations[i].action_spec);
//::     if "bytes_meter" in specs or "packets_meter" in specs:
      if (perf_test->operations[i].meter_spec)
        bf_sys_free(perf_test->operations[i].meter_spec);
//::     #endif
//::     if "stful_spec" in specs:
      if (perf_test->operations[i].stful_spec)
        bf_sys_free(perf_test->operations[i].stful_spec);
//::     #endif
//::     if "lpf_spec" in specs:
      if (perf_test->operations[i].lfp_spec)
        bf_sys_free(perf_test->operations[i].lpf_spec);
//::     #endif
//::     if "wred_spec" in specs:
      if (perf_test->operations[i].wred_spec)
        bf_sys_free(perf_test->operations[i].wred_spec);
//::     #endif
    }
    bf_sys_free(perf_test->operations);
    bf_sys_free(perf_test);
    bf_sys_free(entry_hdls);
    ${p4_pd_prefix}${table}_bulk_p = NULL;
    return rate;
}
//::   #endfor
//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     name = table + "_churn"
//::     match_type = t_info["match_type"]
void *
${name} (void *ops)
{
    ${p4_pd_prefix}${table}_bulk_t *operations = ops;
    bf_sys_mutex_lock(&global_thread_start_mtx);
    while (!global_thread_start) {
        bf_sys_cond_wait(&global_thread_start_condvar, &global_thread_start_mtx);
    }
    bf_sys_mutex_unlock(&global_thread_start_mtx);

    unsigned i = 0;
    p4_pd_status_t pd_status = PIPE_MGR_SUCCESS;
    p4_pd_entry_hdl_t *entry_hdls = bf_sys_malloc(sizeof(p4_pd_entry_hdl_t)*operations->num_ops);
    if (entry_hdls == NULL) {
      bf_sys_dbgchk(0);
      return NULL;
    }
    p4_pd_dev_target_t dev_tgt;
    dev_tgt.device_id = operations->device_id;
    dev_tgt.dev_pipe_id = operations->dev_pipe_id;
    bool stop = false;
    while (!stop) {
        for (i = 0; i < operations->num_ops; i++) {
            pd_status = operations->operations[i].add_fn_ptr(
                    operations->sess_hdl,
                    dev_tgt,
                    operations->operations[i].match_spec,
//::         if match_type == "ternary" or match_type == "range":
                    operations->operations[i].priority,
//::         #endif
                    operations->operations[i].action_spec,
//::         if t_info["timeout"]:
                    operations->operations[i].ttl,
//::         #endif
                    &entry_hdls[i]);
            bf_sys_dbgchk(pd_status == PIPE_MGR_SUCCESS);
        }

        for (i = 0; i < operations->num_ops; i++) {
            pd_status = operations->operations[i].delete_fn_ptr(operations->sess_hdl,
                    operations->device_id,
                    entry_hdls[i]);
            bf_sys_dbgchk(pd_status == PIPE_MGR_SUCCESS);
        }

        bf_sys_mutex_lock(&global_thread_stop_mtx);
        if (global_thread_stop) {
            stop = true;
        }
        bf_sys_mutex_unlock(&global_thread_stop_mtx);
    }
    if (entry_hdls) {
        bf_sys_free(entry_hdls);
    }
    return NULL;
}
//::     name = p4_pd_prefix + table + "_churn_thread_spawn"
void
${name} ()
{
    if (!${p4_pd_prefix}${table}_bulk_p) {
        bf_sys_dbgchk(0);
        return;
    }
    int rv = bf_sys_thread_create(&thread_ids[num_threads], ${table}_churn, ${p4_pd_prefix}${table}_bulk_p, 0);
    if (rv < 0) {
        bf_sys_dbgchk(0);
        return;
    }
    num_threads++;
}
//::     for res_name, res_type, res_hdl in table_direct_resources[table]:
//::       if res_type == "counter":
//::         name = table + "_stat_ent_dump"
void *
${name} (void *dump_info)
{
    stats_ent_dump_info_t *info = dump_info;
    bf_sys_mutex_lock(&global_thread_start_mtx);
    while (!global_thread_start) {
        bf_sys_cond_wait(&global_thread_start_condvar, &global_thread_start_mtx);
    }
    bf_sys_mutex_unlock(&global_thread_start_mtx);
    unsigned i = 0;
    bool stop = false;
    p4_pd_status_t status = PIPE_MGR_SUCCESS;
    dev_target_t pipe_mgr_dev_tgt;
    pipe_mgr_dev_tgt.device_id = (info->dev_tgt).device_id;
    pipe_mgr_dev_tgt.dev_pipe_id = (info->dev_tgt).dev_pipe_id;
    while (!stop) {
        for (i = 0; i < info->num_entry_hdls; i++) {
            status = pipe_mgr_direct_stat_ent_database_sync(info->sess_hdl,
                                                            pipe_mgr_dev_tgt,
                                                            ${table_hdl},
                                                            info->entry_hdls[i]);
            bf_sys_dbgchk(status == PIPE_MGR_SUCCESS);
        }
        bf_sys_mutex_lock(&global_thread_stop_mtx);
        if (global_thread_stop) {
            stop = true;
        }
        bf_sys_mutex_unlock(&global_thread_stop_mtx);
    }
    if (info) {
        if (info->entry_hdls) {
            bf_sys_free(info->entry_hdls);
        }
        bf_sys_free(info);
    }
    return 0;
}
//::         name = p4_pd_prefix + table + "_stats_ent_dump_thread_spawn"
void
${name} (p4_pd_sess_hdl_t sess_hdl,
        p4_pd_dev_target_t dev_tgt,
        p4_pd_entry_hdl_t *entry_hdls,
        int num_entry_hdls)
{
    stats_ent_dump_info_t *info = bf_sys_calloc(1, sizeof(stats_ent_dump_info_t));
    if (info == NULL) {
      bf_sys_dbgchk(0);
      return;
    }
    info->entry_hdls = bf_sys_calloc(num_entry_hdls, sizeof(p4_pd_entry_hdl_t));
    if (info->entry_hdls == NULL) {
      bf_sys_dbgchk(0);
      return;
    }
    info->num_entry_hdls = num_entry_hdls;
    info->sess_hdl = sess_hdl;
    info->dev_tgt = dev_tgt;
    memcpy(info->entry_hdls, entry_hdls, sizeof(p4_pd_entry_hdl_t) * num_entry_hdls);
    int rv = bf_sys_thread_create(&thread_ids[num_threads], ${table}_stat_ent_dump, info, 0);
    if (rv < 0) {
        bf_sys_dbgchk(0);
        return;
    }
    num_threads++;
}
//::       #endif
//::     #endfor
//::   #endfor
//:: #endif

//:: if gen_md_pd:
//::   if gen_hitless_ha_test_pd:
p4_pd_status_t ${p4_pd_prefix + "enable_callbacks_for_hitless_ha"} (p4_pd_sess_hdl_t sess_hdl, uint8_t device_id) {
	return pipe_mgr_enable_callbacks_for_hitless_ha(sess_hdl, device_id);
}
//::   #endif
//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     name = p4_pd_prefix + table + "_register_mat_update_cb"
p4_pd_status_t ${name} (p4_pd_sess_hdl_t sess_hdl,
                        bf_dev_id_t dev_id,
                        p4_pd_mat_update_cb cb,
                        void *cb_cookie) {
  return pipe_register_mat_update_cb(sess_hdl, dev_id, ${table_hdl}, (pipe_mat_update_cb)cb, cb_cookie);
}

p4_pd_status_t ${p4_pd_prefix + table + "_get_mat_table_handle"} (p4_pd_tbl_hdl_t *hdl) {
  if (hdl) *hdl = ${table_hdl};
  return 0;
}
//::   #endfor

//::   action_profiles = set()
//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     if not action_table_hdl: continue
//::     act_prof = t_info["action_profile"]
//::     assert(act_prof is not None)
//::     if act_prof in action_profiles: continue
//::     action_profiles.add(act_prof)
//::     name = p4_pd_prefix + act_prof + "_register_adt_update_cb"
p4_pd_status_t ${name} (p4_pd_sess_hdl_t sess_hdl,
                        bf_dev_id_t dev_id,
                        p4_pd_adt_update_cb cb,
                        void *cb_cookie) {
  return pipe_register_adt_update_cb(sess_hdl, dev_id, ${action_table_hdl}, (pipe_adt_update_cb)cb, cb_cookie);
}

p4_pd_status_t ${p4_pd_prefix + act_prof + "get_adt_table_handle"} (p4_pd_tbl_hdl_t *hdl) {
  if (hdl) *hdl = ${action_table_hdl};
  return 0;
}

//::     if select_hdl:
//::       name = p4_pd_prefix + act_prof + "_register_sel_update_cb"
p4_pd_status_t ${name} (p4_pd_sess_hdl_t sess_hdl,
                        bf_dev_id_t dev_id,
                        p4_pd_sel_update_cb cb,
                        void *cb_cookie) {
  return pipe_register_sel_update_cb(sess_hdl, dev_id, ${select_hdl}, (pipe_sel_update_cb)cb, cb_cookie);
}

p4_pd_status_t ${p4_pd_prefix + act_prof + "get_sel_table_handle"} (p4_pd_tbl_hdl_t *hdl) {
  if (hdl) *hdl = ${select_hdl};
  return 0;
}

//::     #endif
//::   #endfor

/************ Hitless HA State restore APIs **************/

//::   cb_name_1 = p4_pd_prefix + "ha_restore_state_from_adt_sel"
static void ${cb_name_1} (p4_pd_sess_hdl_t sess_hdl,
                        bf_dev_id_t dev_id,
                        pipe_adt_tbl_hdl_t adt_tbl_hdl,
                        p4_pd_act_hdl_t act_hdl,
                        p4_pd_grp_hdl_t grp_hdl,
                        p4_pd_mbr_hdl_t mbr_hdl,
                        int resource_count,
                        adt_data_resources_t *resources) {
  pd_res_spec_t res_spec;
  unsigned i = 0;

  (void) res_spec;
  (void) i;
//::   action_profiles = set()
//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     if not action_table_hdl: continue
//::     act_prof = t_info["action_profile"]
//::     assert(act_prof is not None)
//::     if act_prof in action_profiles: continue
//::     action_profiles.add(act_prof)
  if (adt_tbl_hdl == ${action_table_hdl}) {
    // Member add
    if (mbr_hdl && !grp_hdl) {
      p4_pd_ms_new_mbr(sess_hdl, ms_${act_prof}_state, dev_id, mbr_hdl);
      p4_pd_ms_set_mbr_act(ms_${act_prof}_state, dev_id, mbr_hdl, act_hdl);
    }
    // Group add
    if (grp_hdl && !mbr_hdl) {
      p4_pd_ms_new_grp(sess_hdl, ms_${act_prof}_state, dev_id, grp_hdl);
    }

    // Member add to group
    if (grp_hdl && mbr_hdl) {
      p4_pd_ms_add_mbr_to_grp(sess_hdl, ms_${act_prof}_state, dev_id, mbr_hdl, grp_hdl);
      p4_pd_act_hdl_t mbr_act_hdl;
      p4_pd_status_t status = p4_pd_ms_get_mbr_act(ms_${act_prof}_state, dev_id, mbr_hdl, &mbr_act_hdl);
      if (status != PIPE_MGR_SUCCESS) {
        return;
      }
      p4_pd_ms_set_grp_act(ms_${act_prof}_state, dev_id, grp_hdl, mbr_act_hdl);
    }
//::     for action in t_info["actions"]:
//::       a_info = action_info[action]
//::       action_hdl = action_handles[(table, action)]
    if (resource_count) {
      if (act_hdl == ${action_hdl}) {
//::       for res_hdl in table_indirect_resources[table]:
//::         a_res = get_action_indirect_res(a_info, resource_handles, res_hdl)
//::         if a_res is not None:
        res_spec.tbl_hdl = ${res_hdl};
//::           if a_res[0] == "constant":
        res_spec.tbl_idx = ${a_res[1]};
//::           else:
        for (i = 0; i < resource_count; i++) {
          adt_data_resources_t *pipe_res_spec = &resources[i];
          if (pipe_res_spec->tbl_hdl == res_spec.tbl_hdl) {
            res_spec.tbl_idx = pipe_res_spec->tbl_idx;
          }
        }
//::           #endif
        p4_pd_ms_mbr_add_res(ms_${act_prof}_state, dev_id, mbr_hdl, &res_spec);
//::         #endif
//::       #endfor
        p4_pd_ms_mbr_apply_to_grps(ms_${act_prof}_state, dev_id, mbr_hdl, p4_pd_ms_grp_update_res, (void *)ms_${act_prof}_state);
      }
    }
//::     #endfor
  }
//::   #endfor
}

//::   name = p4_pd_prefix + "restore_virtual_dev_state"
p4_pd_status_t ${name}(p4_pd_sess_hdl_t sess_hdl,
                       bf_dev_id_t dev_id,
                       p4_pd_tbl_hdl_t tbl_hdl,
                       struct p4_pd_plcmt_info *info,
                       uint32_t *processed) {
  return pipe_mgr_hitless_ha_restore_virtual_dev_state(
      sess_hdl,
      dev_id,
      tbl_hdl,
      (struct pipe_plcmt_info *)info,
      processed,
      ${cb_name_1});
}

//:: #endif

//:: pvs_names = []
//:: pvs_list = []
//:: for pvs in parser_value_set["ingress"]:
//::   pvs_names.append(pvs["pvs_name"]) 
//::   pvs_list.append(pvs) 
//:: #endfor
//:: for pvs in parser_value_set["egress"]:
//::   if pvs["pvs_name"] not in pvs_names:
//::     pvs_names.append(pvs["pvs_name"]) 
//::     pvs_list.append(pvs) 
//::   #endif
//:: #endfor
//:: for pvs in pvs_list:

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_set_property"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 int                        dev_id,
 p4_pd_pvs_prop_type_t      property,
 p4_pd_pvs_prop_value_t     value,
 p4_pd_pvs_prop_args_t      value_args
)
{
  pipe_mgr_pvs_prop_type_t prop_type = property;
  pipe_mgr_pvs_prop_value_t prop_val;
  pipe_mgr_pvs_prop_args_t args_val;
  prop_val.value = value.value;
  args_val.value = value_args.value;
  return pipe_mgr_pvs_set_property(sess_hdl, dev_id, ${pvs["pvs_handle"]}, prop_type, prop_val, args_val);
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_get_property"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 int                        dev_id,
 p4_pd_pvs_prop_type_t      property,
 p4_pd_pvs_prop_value_t     *value,
 p4_pd_pvs_prop_args_t      value_args
)
{
  pipe_mgr_pvs_prop_type_t prop_type = property;
  pipe_mgr_pvs_prop_value_t prop_val;
  pipe_mgr_pvs_prop_args_t args_val;
  args_val.value = value_args.value;
  p4_pd_status_t status = pipe_mgr_pvs_get_property(sess_hdl, dev_id, ${pvs["pvs_handle"]}, prop_type, &prop_val, args_val);
  if (status != PIPE_MGR_SUCCESS) {
    value->value = -1;
  } else {
    value->value = prop_val.value;
  }
  return status;
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_entry_add"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 p4_pd_dev_parser_target_t  prsr_tgt,
 uint32_t                   parser_value,
 uint32_t                   parser_value_mask,
 p4_pd_pvs_hdl_t            *pvs_entry_handle
)
{
  bf_dev_id_t device_id = prsr_tgt.device_id;
  bf_dev_pipe_t dev_pipe_id = (prsr_tgt.dev_pipe_id == PD_DEV_PIPE_ALL) ?
                                   DEV_PIPE_ALL : prsr_tgt.dev_pipe_id;
  bf_dev_direction_t gress = prsr_tgt.gress_id == PD_PVS_GRESS_INGRESS ? BF_DEV_DIR_INGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_EGRESS ? BF_DEV_DIR_EGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_ALL ? BF_DEV_DIR_ALL :
                                prsr_tgt.gress_id;
  return pipe_mgr_pvs_entry_add(sess_hdl, device_id, ${pvs["pvs_handle"]}, gress, dev_pipe_id, prsr_tgt.parser_id, parser_value, parser_value_mask, pvs_entry_handle);
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_entry_modify"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 int                        dev_id,
 p4_pd_pvs_hdl_t            pvs_entry_handle,
 uint32_t                   parser_value,
 uint32_t                   parser_value_mask
)
{
  bf_dev_id_t device_id = dev_id;
  return pipe_mgr_pvs_entry_modify(sess_hdl, device_id, ${pvs["pvs_handle"]}, pvs_entry_handle, parser_value, parser_value_mask);
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_entry_delete"

p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 int                        dev_id,
 p4_pd_pvs_hdl_t            pvs_entry_handle
)
{
  bf_dev_id_t device_id = dev_id;
  return pipe_mgr_pvs_entry_delete(sess_hdl, device_id, ${pvs["pvs_handle"]}, pvs_entry_handle);
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_entry_get"

p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t     sess_hdl,
 int                  dev_id,
 p4_pd_pvs_hdl_t      pvs_entry_handle,
 uint32_t             *parser_value,
 uint32_t             *parser_value_mask
) {
 bf_dev_id_t device_id = dev_id;
 return pipe_mgr_pvs_entry_get(sess_hdl, device_id, ${pvs["pvs_handle"]}, pvs_entry_handle, parser_value, parser_value_mask, NULL, NULL, NULL);
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_entry_handle_get"

p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 p4_pd_dev_parser_target_t  prsr_tgt,
 uint32_t                   parser_value,
 uint32_t                   parser_value_mask,
 p4_pd_pvs_hdl_t            *pvs_entry_handle
) {
  bf_dev_id_t device_id = prsr_tgt.device_id;
  bf_dev_pipe_t dev_pipe_id = (prsr_tgt.dev_pipe_id == PD_DEV_PIPE_ALL) ?
                                   DEV_PIPE_ALL : prsr_tgt.dev_pipe_id;
  bf_dev_direction_t gress = prsr_tgt.gress_id == PD_PVS_GRESS_INGRESS ? BF_DEV_DIR_INGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_EGRESS ? BF_DEV_DIR_EGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_ALL ? BF_DEV_DIR_ALL :
                                prsr_tgt.gress_id;
  return pipe_mgr_pvs_entry_handle_get(sess_hdl, device_id, ${pvs["pvs_handle"]}, gress, dev_pipe_id, prsr_tgt.parser_id, parser_value, parser_value_mask, pvs_entry_handle);
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_entry_get_first_entry_handle"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 p4_pd_dev_parser_target_t  prsr_tgt,
 p4_pd_pvs_hdl_t            *pvs_entry_handle
) {
  bf_dev_id_t device_id = prsr_tgt.device_id;
  bf_dev_pipe_t dev_pipe_id = (prsr_tgt.dev_pipe_id == PD_DEV_PIPE_ALL) ?
                                   DEV_PIPE_ALL : prsr_tgt.dev_pipe_id;
  bf_dev_direction_t gress = prsr_tgt.gress_id == PD_PVS_GRESS_INGRESS ? BF_DEV_DIR_INGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_EGRESS ? BF_DEV_DIR_EGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_ALL ? BF_DEV_DIR_ALL :
                                prsr_tgt.gress_id;
  return pipe_mgr_pvs_entry_get_first(sess_hdl, device_id, ${pvs["pvs_handle"]}, gress, dev_pipe_id, prsr_tgt.parser_id, pvs_entry_handle);
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_entry_get_next_entry_handles"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 p4_pd_dev_parser_target_t  prsr_tgt,
 p4_pd_pvs_hdl_t            pvs_entry_handle,
 int                        n,
 p4_pd_pvs_hdl_t            *next_entry_handles
) {
  bf_dev_id_t device_id = prsr_tgt.device_id;
  bf_dev_pipe_t dev_pipe_id = (prsr_tgt.dev_pipe_id == PD_DEV_PIPE_ALL) ?
                                   DEV_PIPE_ALL : prsr_tgt.dev_pipe_id;
  bf_dev_direction_t gress = prsr_tgt.gress_id == PD_PVS_GRESS_INGRESS ? BF_DEV_DIR_INGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_EGRESS ? BF_DEV_DIR_EGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_ALL ? BF_DEV_DIR_ALL :
                                prsr_tgt.gress_id;
  return pipe_mgr_pvs_entry_get_next(sess_hdl, device_id, ${pvs["pvs_handle"]}, gress, dev_pipe_id, prsr_tgt.parser_id, pvs_entry_handle, n, next_entry_handles);
}

//::   name = p4_pd_prefix + pvs["pvs_name"] + "_entry_get_count"
p4_pd_status_t
${name}
(
 p4_pd_sess_hdl_t           sess_hdl,
 p4_pd_dev_parser_target_t  prsr_tgt,
 bool                       read_from_hw,
 uint32_t                   *count
) {
  bf_dev_id_t device_id = prsr_tgt.device_id;
  bf_dev_pipe_t dev_pipe_id = (prsr_tgt.dev_pipe_id == PD_DEV_PIPE_ALL) ?
                                   DEV_PIPE_ALL : prsr_tgt.dev_pipe_id;
  bf_dev_direction_t gress = prsr_tgt.gress_id == PD_PVS_GRESS_INGRESS ? BF_DEV_DIR_INGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_EGRESS ? BF_DEV_DIR_EGRESS :
                                prsr_tgt.gress_id == PD_PVS_GRESS_ALL ? BF_DEV_DIR_ALL :
                                prsr_tgt.gress_id;
  return pipe_mgr_pvs_entry_get_count(sess_hdl, device_id, ${pvs["pvs_handle"]}, gress, dev_pipe_id, prsr_tgt.parser_id, read_from_hw, count);
}

//:: #endfor

static void prepend_to_bytestream(uint8_t **p, uint32_t *bit_pos, uint8_t *val, uint32_t bit_width) {
  uint8_t tmp;
  uint8_t *val_p = (val + (bit_width + 7) / 8 - 1);
  uint32_t val_bit_pos = 0;
  while (bit_width) {
    if (*bit_pos != 0) {
      tmp = *val_p << *bit_pos;
      **p |= tmp;
      val_bit_pos = 8 - *bit_pos;
      if (bit_width >= val_bit_pos) {
        (*p)--;
        *bit_pos = 0;
        bit_width -= val_bit_pos;
      } else {
        *bit_pos += bit_width;
        bit_width = 0;
      }
    } else if (val_bit_pos != 0) {
      tmp = *val_p >> val_bit_pos;
      **p |= tmp;
      if (bit_width >= (8 - val_bit_pos)) {
        val_p--;
        *bit_pos = 8 - val_bit_pos;
        bit_width -= (8 - val_bit_pos);
      } else {
        *bit_pos = bit_width;
        bit_width = 0;
      }
      val_bit_pos = 0;
    } else {
      **p = *val_p;
      if (bit_width >= 8) {
        (*p)--;
        val_p--;
        bit_width -= 8;
      } else {
        *bit_pos = bit_width;
        bit_width = 0;
      }
    }
  }
  return;
}

//:: for hash_calc_name, dyn_hash_calc_info in hash_calc.items():

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   params += [p4_pd_prefix + hash_calc_name + "_input_t input"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_input_set"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  pipe_sel_tbl_hdl_t field_list_hdl = 0;
//::   for fl_name, fl_info in dyn_hash_calc_info['field_lists'].items():
//::     enum_name_upper = p4_pd_prefix.upper() + hash_calc_name.upper() + "_INPUT_" + fl_name.upper()
  if (${enum_name_upper} == input) {
    field_list_hdl = ${fl_info["handle"]};
  }
//::   #endfor
  return pipe_mgr_hash_calc_input_set(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, field_list_hdl);
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   params += [p4_pd_prefix + hash_calc_name + "_input_t *input"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_input_get"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  pipe_sel_tbl_hdl_t field_list_hdl = 0;
  bf_status_t status = BF_SUCCESS;

  status = pipe_mgr_hash_calc_input_get(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, &field_list_hdl);
//::   for fl_name, fl_info in dyn_hash_calc_info['field_lists'].items():
//::     enum_name_upper = p4_pd_prefix.upper() + hash_calc_name.upper() + "_INPUT_" + fl_name.upper()
  if (${fl_info["handle"]} == field_list_hdl) {
    *input = ${enum_name_upper};
    return status;
  }
//::   #endfor

  return BF_INVALID_ARG;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   params += [p4_pd_prefix + hash_calc_name + "_algo_t algo"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_algorithm_set"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  pipe_sel_tbl_hdl_t algorithm_hdl = 0;
//::   for alg_name, handle in dyn_hash_calc_info['algs']:
//::     enum_name_upper = p4_pd_prefix.upper() + hash_calc_name.upper() + "_ALGORITHM_" + alg_name.upper()
  if (${enum_name_upper} == algo) {
    algorithm_hdl = ${handle};
  }
//::   #endfor
  return pipe_mgr_hash_calc_algorithm_set(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]},
          algorithm_hdl, NULL, 0);
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   params += [p4_pd_prefix + hash_calc_name + "_algo_t *algo"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_algorithm_get"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  bf_status_t status = BF_SUCCESS;
  pipe_sel_tbl_hdl_t algorithm_hdl = 0;

  status = pipe_mgr_hash_calc_algorithm_get(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]},
          &algorithm_hdl, NULL, NULL);
//::   for alg_name, handle in dyn_hash_calc_info['algs']:
//::     enum_name_upper = p4_pd_prefix.upper() + hash_calc_name.upper() + "_ALGORITHM_" + alg_name.upper()
  if (${handle} == algorithm_hdl) {
    *algo = ${enum_name_upper};
    return status;
  }
//::   #endfor

  return BF_INVALID_ARG;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "uint64_t seed"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_seed_set"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  return pipe_mgr_hash_calc_seed_set(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, seed);
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id",
//::             "uint64_t *seed"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_seed_get"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  return pipe_mgr_hash_calc_seed_get(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, seed);
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   params += [p4_pd_prefix + hash_calc_name + "_input_t input"]
//::   params += ["uint32_t attr_count"]
//::   params += [p4_pd_prefix + hash_calc_name + "_input_field_attribute_t *array_of_attrs"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_input_field_attribute_set"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  bf_status_t status = BF_SUCCESS;
  pipe_sel_tbl_hdl_t field_list_hdl = 0;
//::   for fl_name, fl_info in dyn_hash_calc_info['field_lists'].items():
//::     enum_name_upper = p4_pd_prefix.upper() + hash_calc_name.upper() + "_INPUT_" + fl_name.upper()
  if (${enum_name_upper} == input) {
    field_list_hdl = ${fl_info["handle"]};
  }
//::   #endfor

  pipe_hash_calc_input_field_attribute_t *pipe_attrs = NULL;
  if (attr_count) {
    pipe_attrs = bf_sys_calloc(attr_count, sizeof(pipe_hash_calc_input_field_attribute_t));
    for (uint32_t i = 0; i < attr_count; i++) {
      pipe_attrs[i].slice_start_bit = 0;
      pipe_attrs[i].slice_length = 0;
      pipe_attrs[i].order = array_of_attrs[i].input_field.id + 1;
      pipe_attrs[i].input_field = array_of_attrs[i].input_field.id;
      pipe_attrs[i].type = array_of_attrs[i].type;
      pipe_attrs[i].value.val = array_of_attrs[i].value.val;
    }
  }

  status = pipe_mgr_hash_calc_input_field_attribute_set(
      sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, field_list_hdl, attr_count, pipe_attrs);

  if (pipe_attrs) {
    bf_sys_free(pipe_attrs);
  }
  return status;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   params += [p4_pd_prefix + hash_calc_name + "_input_t input"]
//::   params += ["uint32_t *attr_count"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_input_field_attribute_count_get"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  pipe_sel_tbl_hdl_t field_list_hdl = 0;
  bf_status_t status = BF_SUCCESS;

  status = pipe_mgr_hash_calc_input_get(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, &field_list_hdl);

//::   for fl_name, fl_info in dyn_hash_calc_info['field_lists'].items():
//::     enum_name_upper = p4_pd_prefix.upper() + hash_calc_name.upper() + "_INPUT_" + fl_name.upper()
  if (${fl_info["handle"]} == field_list_hdl && ${enum_name_upper} != input) {
    return BF_INVALID_ARG;
  }
//::   #endfor

  *attr_count = 0;
  return pipe_mgr_hash_calc_input_field_attribute_count_get(
      sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, field_list_hdl, attr_count);
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   params += [p4_pd_prefix + hash_calc_name + "_input_t input"]
//::   params += ["uint32_t max_attr_count"]
//::   params += [p4_pd_prefix + hash_calc_name + "_input_field_attribute_t *array_of_attrs"]
//::   params += ["uint32_t *num_attr_filled"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_input_field_attribute_get"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  pipe_sel_tbl_hdl_t field_list_hdl = 0;
  bf_status_t status = BF_SUCCESS;
  int i = 0, len = 0;
  *num_attr_filled = 0;

  status = pipe_mgr_hash_calc_input_get(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, &field_list_hdl);

//::   for fl_name, fl_info in dyn_hash_calc_info['field_lists'].items():
//::     enum_name_upper = p4_pd_prefix.upper() + hash_calc_name.upper() + "_INPUT_" + fl_name.upper()
  if (${fl_info["handle"]} == field_list_hdl && ${enum_name_upper} != input) {
    return BF_INVALID_ARG;
  }
//::   #endfor

  if (max_attr_count) {
    pipe_hash_calc_input_field_attribute_t *pipe_attrs =
        bf_sys_calloc(max_attr_count, sizeof(pipe_hash_calc_input_field_attribute_t));
    status = pipe_mgr_hash_calc_input_field_attribute_get(
        sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, field_list_hdl, max_attr_count, pipe_attrs, num_attr_filled);
    if (status != PIPE_SUCCESS) {
      bf_sys_free(pipe_attrs);
      return status;
    }

    for (uint32_t i = 0; i < *num_attr_filled; i++) {
      array_of_attrs[i].input_field.id = pipe_attrs[i].input_field;
      array_of_attrs[i].type = pipe_attrs[i].type;
      array_of_attrs[i].value.val = pipe_attrs[i].value.val;
    }
    bf_sys_free(pipe_attrs);
  }

  return BF_SUCCESS;
}

//::   params = ["p4_pd_sess_hdl_t sess_hdl",
//::             "bf_dev_id_t dev_id"]
//::   params += ["uint32_t attr_count"]
//::   params += [p4_pd_prefix + hash_calc_name + "_input_field_attribute_t *array_of_attrs"]
//::   params += ["uint32_t *attr_sizes"]
//::   params += ["uint8_t *hash"]
//::   params += ["uint32_t hash_len"]
//::   param_str = ",\n ".join(params)
//::   fn_name = p4_pd_prefix + "hash_calc_" + hash_calc_name + "_calculate_hash_value"
p4_pd_status_t
${fn_name}
(
 ${param_str}
)
{
  bf_status_t status = BF_SUCCESS;
  uint8_t *stream = NULL, *p = NULL, *attr = NULL;
  uint32_t stream_len = 0, bit_pos = 0, attr_be = 0;
  int i = 0;

  for (i = 0; i < attr_count; i++) {
    stream_len += attr_sizes[i];
  }
  stream_len = (stream_len + 7) / 8;
  stream = bf_sys_calloc(stream_len, sizeof(uint8_t));
  p = stream + stream_len - 1;

  for (i = attr_count - 1; i >= 0; i--) {
    if (attr_sizes[i] <= 32) {
      /* Convert val to big endian bytestream */
      attr_be = htonl(array_of_attrs[i].value.val);
      attr = (uint8_t *)(&attr_be);
      attr += (4 - (attr_sizes[i] + 7) / 8);
    } else {
      attr = array_of_attrs[i].value.stream;
    }
    prepend_to_bytestream(&p, &bit_pos, attr, attr_sizes[i]);
  }

  status = pipe_mgr_hash_calc_calculate_hash_value(sess_hdl, dev_id, ${dyn_hash_calc_info["handle"]}, stream, stream_len, hash, hash_len);
  bf_sys_free(stream);
  return status;
}

//:: #endfor

//:: if gen_hitless_ha_test_pd:
static inline void unbuild_ha_reconc_report
(
 pipe_tbl_ha_reconc_report_t *pipe_ha_report,
 ${p4_pd_prefix}ha_reconc_report_t *ha_report 
)
{
  ha_report->num_entries_added = pipe_ha_report->num_entries_added;
  ha_report->num_entries_deleted = pipe_ha_report->num_entries_deleted;
  ha_report->num_entries_modified = pipe_ha_report->num_entries_modified;
}

//::   for table, t_info in table_info.items():
//::     table_hdl, action_table_hdl, select_hdl = table_handles[table]
//::     name = p4_pd_prefix + table + "_get_ha_reconciliation_report"
//::     params = ["p4_pd_sess_hdl_t sess_hdl", "p4_pd_dev_target_t dev_tgt"]
//::     params += [p4_pd_prefix + "ha_reconc_report_t *ha_report"]
//::     param_str = ",\n ".join(params)
p4_pd_status_t
${name}
(
 ${param_str}
)
{
  pipe_tbl_ha_reconc_report_t pipe_ha_report = {0}; 
  dev_target_t pipe_mgr_dev_tgt;

  pipe_mgr_dev_tgt.device_id = dev_tgt.device_id;
  pipe_mgr_dev_tgt.dev_pipe_id = dev_tgt.dev_pipe_id;

  p4_pd_status_t status = pipe_mgr_mat_ha_reconciliation_report_get(sess_hdl, pipe_mgr_dev_tgt, ${table_hdl}, &pipe_ha_report);
  if (status) return status;

  unbuild_ha_reconc_report(&pipe_ha_report, ha_report);

  return status;
}

//::   #endfor
//:: #endif
