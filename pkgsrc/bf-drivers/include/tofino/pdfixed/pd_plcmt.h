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


#ifndef _TOFINO_PDFIXED_PD_PLCMT_H
#define _TOFINO_PDFIXED_PD_PLCMT_H

#include <tofino/pdfixed/pd_common.h>

/* Placement Data */
typedef void *(*p4_pd_plcmnt_alloc)(size_t size);
typedef void (*p4_pd_plcmnt_free)(void *data);
p4_pd_status_t p4_pd_register_plcmnt_mem_fns(p4_pd_plcmnt_alloc alloc_fn,
                                             p4_pd_plcmnt_free free_fn);
/* Returns the size of the memory pointed to by data.  This can be used to know
 * how much memory to allocate when copying data. */
p4_pd_status_t p4_pd_plcmt_data_size(const void *data, size_t *size);
/* Copies the data at src to dst.  Dst should be pre-allocated and of adequate
 * size. src and dst must not overlap. */
p4_pd_status_t p4_pd_plcmt_copy(const void *src, void *dst);
/* Allocates memory and then copys the data at src into it.  The memory will be
 * allocated using the registered alloc_fn if set.  The address of the copied
 * data will be returned in *copy. */
p4_pd_status_t p4_pd_plcmt_duplicate(const void *src, void **copy);
/* Given an unpacked entry data, return the number of bytes that data would
 * consume in packed format. */
p4_pd_status_t p4_pd_plcmt_pack_data_size(const void *unpacked_data,
                                          size_t *size);
/* Given a packed entry data, return the number of bytes that data would consume
 * in an unpacked format. */
p4_pd_status_t p4_pd_plcmt_unpack_data_size(const void *packed_data,
                                            size_t *size);
/* Given an unpacked source entry data, copy into a packed format at the address
 * provided by packed_dst.  The packed_dst must point at valid memory capable of
 * holding the entry data in packed format. */
p4_pd_status_t p4_pd_plcmt_copy_pack(const void *unpacked_src,
                                     void *packed_dst);
/* Given a packed source entry data, copy into an unpacked format at the address
 * provided by the unpacked_dst.  The unpacked_dst must point at valid memory
 * capable of holding the entry data in unpacked format. */
p4_pd_status_t p4_pd_plcmt_copy_unpack(const void *packed_src,
                                       void *unpacked_dst);
/* Frees placement data previously allocated by the driver which is no longer
 * needed.  This should be used to free the data returned by a delete callback
 * or the old data returned by a modify callback. */
p4_pd_status_t p4_pd_plcmt_free(void *data);

/******************************************************************************
 *
 * MAT Placement Callbacks
 *
 *****************************************************************************/
enum p4_pd_mat_update_type {
  P4_PD_MAT_UPDATE_ADD,
  P4_PD_MAT_UPDATE_ADD_MULTI, /* An add to multiple logical indices. */
  P4_PD_MAT_UPDATE_SET_DFLT,  /* Set the table's default action. */
  P4_PD_MAT_UPDATE_CLR_DFLT,  /* Clear the table's default action. */
  P4_PD_MAT_UPDATE_DEL,
  P4_PD_MAT_UPDATE_MOD,
  P4_PD_MAT_UPDATE_MOV,
  P4_PD_MAT_UPDATE_MOV_MULTI /* A move involving mulitple logical indices. */
};
struct p4_pd_mat_update_set_dflt_params {
  p4_pd_entry_hdl_t ent_hdl;
  /* When using Action Profiles without selector the handle of the action
   * profile member used by this entry is specified as action_profile_mbr.
   * Valid when action_profile_mbr_exists is true. */
  p4_pd_entry_hdl_t action_profile_mbr;
  /* When using Action Profiles the logical index (in the action table) of the
   * action profile member used by this entry is specified as
   * indirect_action_index.
   * When using selection groups with Action Profiles the indirect_action_index
   * represents the base logical index in the action table that this entry will
   * use.
   * Valid when action_profile_mbr_exists or sel_grp_exists is true. */
  p4_pd_idx_t indirect_action_index;
  /* The action_profile_mbr_exists flag is set to true when the entry is using
   * an Action Profile.  In this case, the P4_PD_ADT_UPDATE_ADD update for the
   * action_profile_mbr must be processed first.  Additionally, the logical
   * index specified by indirect_action_index must either be unoccupied or
   * occupied by an entry with a handle equal to action_profile_mbr.
   * Set to true when the entry is using action profile without selection. */
  bool action_profile_mbr_exists;

  /* When using Match-Select sel_grp_hdl specifies the group handle this entry
   * uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_entry_hdl_t sel_grp_hdl;
  /* When using Match-Select indirect_selection_index specifies the base
   * logical index the selection group uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_idx_t indirect_selection_index;
  /* When using Match-Select num_selector_indices specifies the number logical
   * indexes the selection group uses.
   * Valid when sel_grp_exists is true. */
  uint32_t num_selector_indices;
  /* The sel_grp_exists flag is set to true when the entry is using Match-
   * Select.  In this case, each of the consecutive num_selector_indices
   * indexes starting at indirect_selection_index should have at least one
   * member.
   * Set to true when the entry uses action profile with seleciton. */
  bool sel_grp_exists;
  void *data;
};
struct p4_pd_mat_update_clr_dflt_params {
  p4_pd_entry_hdl_t ent_hdl;
};
struct p4_pd_mat_update_add_params {
  p4_pd_entry_hdl_t ent_hdl;
  uint32_t priority;
  p4_pd_idx_t logical_index;
  /* When using Action Profiles without selector the handle of the action
   * profile member used by this entry is specified as action_profile_mbr.
   * Valid when action_profile_mbr_exists is true. */
  p4_pd_entry_hdl_t action_profile_mbr;
  /* When using Action Profiles the logical index (in the action table) of the
   * action profile member used by this entry is specified as
   * indirect_action_index.
   * When using selection groups with Action Profiles the indirect_action_index
   * represents the base logical index in the action table that this entry will
   * use.
   * Valid when action_profile_mbr_exists or sel_grp_exists is true. */
  p4_pd_idx_t indirect_action_index;
  /* The action_profile_mbr_exists flag is set to true when the entry is using
   * an Action Profile.  In this case, the P4_PD_ADT_UPDATE_ADD update for the
   * action_profile_mbr must be processed first.  Additionally, the logical
   * index specified by indirect_action_index must either be unoccupied or
   * occupied by an entry with a handle equal to action_profile_mbr.
   * Set to true when the entry is using action profile without selection. */
  bool action_profile_mbr_exists;
  /* When using Match-Select sel_grp_hdl specifies the group handle this entry
   * uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_entry_hdl_t sel_grp_hdl;
  /* When using Match-Select indirect_selection_index specifies the base
   * logical index the selection group uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_idx_t indirect_selection_index;
  /* When using Match-Select num_selector_indices specifies the number logical
   * indexes the selection group uses.
   * Valid when sel_grp_exists is true. */
  uint32_t num_selector_indices;
  /* The sel_grp_exists flag is set to true when the entry is using Match-
   * Select.  In this case, each of the consecutive num_selector_indices
   * indexes starting at indirect_selection_index should have at least one
   * member.
   * Set to true when the entry uses action profile with seleciton. */
  bool sel_grp_exists;
  void *data;
};
struct p4_pd_mat_update_del_params {
  p4_pd_entry_hdl_t ent_hdl;
};
struct p4_pd_mat_update_mod_params {
  p4_pd_entry_hdl_t ent_hdl;
  /* When using Action Profiles without selector the handle of the action
   * profile member used by this entry is specified as action_profile_mbr.
   * Valid when action_profile_mbr_exists is true. */
  p4_pd_entry_hdl_t action_profile_mbr;
  /* When using Action Profiles the logical index (in the action table) of the
   * action profile member used by this entry is specified as
   * indirect_action_index.
   * When using selection groups with Action Profiles the indirect_action_index
   * represents the base logical index in the action table that this entry will
   * use.
   * Valid when action_profile_mbr_exists or sel_grp_exists is true. */
  p4_pd_idx_t indirect_action_index;
  /* The action_profile_mbr_exists flag is set to true when the entry is using
   * an Action Profile.  In this case, the P4_PD_ADT_UPDATE_ADD update for the
   * action_profile_mbr must be processed first.  Additionally, the logical
   * index specified by indirect_action_index must either be unoccupied or
   * occupied by an entry with a handle equal to action_profile_mbr.
   * Set to true when the entry is using action profile without selection. */
  bool action_profile_mbr_exists;
  /* When using Match-Select sel_grp_hdl specifies the group handle this entry
   * uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_entry_hdl_t sel_grp_hdl;
  /* When using Match-Select indirect_selection_index specifies the base
   * logical index the selection group uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_idx_t indirect_selection_index;
  /* When using Match-Select num_selector_indices specifies the number logical
   * indexes the selection group uses.
   * Valid when sel_grp_exists is true. */
  uint32_t num_selector_indices;
  /* The sel_grp_exists flag is set to true when the entry is using Match-
   * Select.  In this case, each of the consecutive num_selector_indices
   * indexes starting at indirect_selection_index should have at least one
   * member.
   * Set to true when the entry uses action profile with seleciton. */
  bool sel_grp_exists;
  void *data;
};
struct p4_pd_mat_update_mov_params {
  p4_pd_entry_hdl_t ent_hdl;
  p4_pd_idx_t logical_index;
  /* When using Action Profiles without selector the handle of the action
   * profile member used by this entry is specified as action_profile_mbr.
   * Valid when action_profile_mbr_exists is true. */
  p4_pd_entry_hdl_t action_profile_mbr;
  /* When using Action Profiles the logical index (in the action table) of the
   * action profile member used by this entry is specified as
   * indirect_action_index.
   * When using selection groups with Action Profiles the indirect_action_index
   * represents the base logical index in the action table that this entry will
   * use.
   * Valid when action_profile_mbr_exists or sel_grp_exists is true. */
  p4_pd_idx_t indirect_action_index;
  /* The action_profile_mbr_exists flag is set to true when the entry is using
   * an Action Profile.  In this case, the P4_PD_ADT_UPDATE_ADD update for the
   * action_profile_mbr must be processed first.  Additionally, the logical
   * index specified by indirect_action_index must either be unoccupied or
   * occupied by an entry with a handle equal to action_profile_mbr.
   * Set to true when the entry is using action profile without selection. */
  bool action_profile_mbr_exists;
  /* When using Match-Select sel_grp_hdl specifies the group handle this entry
   * uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_entry_hdl_t sel_grp_hdl;
  /* When using Match-Select indirect_selection_index specifies the base
   * logical index the selection group uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_idx_t indirect_selection_index;
  /* When using Match-Select num_selector_indices specifies the number logical
   * indexes the selection group uses.
   * Valid when sel_grp_exists is true. */
  uint32_t num_selector_indices;
  /* The sel_grp_exists flag is set to true when the entry is using Match-
   * Select.  In this case, each of the consecutive num_selector_indices
   * indexes starting at indirect_selection_index should have at least one
   * member.
   * Set to true when the entry uses action profile with selection. */
  bool sel_grp_exists;
  void *data;
};
/* Represents an add to multiple logical indices.  There are
 * logical_index_array_length number of base indices, specified in
 * logical_index_base_array.  Associated with each base index is a count,
 * specified in logical_index_count_array, saying how many consecutive indices
 * are used.  For example, given:
 *   logical_index_array_length = 3
 *   location_array = [ [250,2], [200,1], [300,6] ]
 * The following nine logical indices would be used: 250-251, 200, and 300-305.
 */
struct p4_pd_mat_update_add_multi_params {
  p4_pd_entry_hdl_t ent_hdl;
  uint32_t priority; /* TCAM priority, only valid for TCAM tables. */
  /* When using Action Profiles without selector the handle of the action
   * profile member used by this entry is specified as action_profile_mbr.
   * Valid when action_profile_mbr_exists is true. */
  p4_pd_entry_hdl_t action_profile_mbr;
  /* When using Action Profiles the logical index (in the action table) of the
   * action profile member used by this entry is specified as
   * indirect_action_index.
   * When using selection groups with Action Profiles the indirect_action_index
   * represents the base logical index in the action table that this entry will
   * use.
   * Valid when action_profile_mbr_exists or sel_grp_exists is true. */
  p4_pd_idx_t indirect_action_index;
  /* The action_profile_mbr_exists flag is set to true when the entry is using
   * an Action Profile.  In this case, the P4_PD_ADT_UPDATE_ADD update for the
   * action_profile_mbr must be processed first.  Additionally, the logical
   * index specified by indirect_action_index must either be unoccupied or
   * occupied by an entry with a handle equal to action_profile_mbr.
   * Set to true when the entry is using action profile without selection. */
  bool action_profile_mbr_exists;
  /* When using Match-Select sel_grp_hdl specifies the group handle this entry
   * uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_entry_hdl_t sel_grp_hdl;
  /* When using Match-Select indirect_selection_index specifies the base
   * logical index the selection group uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_idx_t indirect_selection_index;
  /* When using Match-Select num_selector_indices specifies the number logical
   * indexes the selection group uses.
   * Valid when sel_grp_exists is true. */
  uint32_t num_selector_indices;
  /* The sel_grp_exists flag is set to true when the entry is using Match-
   * Select.  In this case, each of the consecutive num_selector_indices
   * indexes starting at indirect_selection_index should have at least one
   * member.
   * Set to true when the entry uses action profile with seleciton. */
  bool sel_grp_exists;
  int logical_index_array_length;
  struct p4_pd_multi_index *location_array;
  void *data;
};
/* Represents a move of an entry occupying multiple logical indices.  Similar
 * to struct p4_pd_mat_update_add_multi_params, logical_index_array_length
 * specifies how many sets of logical indices are moving.  The location_array
 * provides the new logical indexes of the entry specified as a series of base
 * and count pairs representing count number of logical indexes starting at and
 * includeing the base.  For example, given:
 *   logical_index_array_length = 2
 *   location_array = [[50,3], [100,1]]
 * The entry now occupies logical indices 50-52 and 100. */
struct p4_pd_mat_update_mov_multi_params {
  p4_pd_entry_hdl_t ent_hdl;
  /* When using Action Profiles without selector the handle of the action
   * profile member used by this entry is specified as action_profile_mbr.
   * Valid when action_profile_mbr_exists is true. */
  p4_pd_entry_hdl_t action_profile_mbr;
  /* When using Action Profiles the logical index (in the action table) of the
   * action profile member used by this entry is specified as
   * indirect_action_index.
   * When using selection groups with Action Profiles the indirect_action_index
   * represents the base logical index in the action table that this entry will
   * use.
   * Valid when action_profile_mbr_exists or sel_grp_exists is true. */
  p4_pd_idx_t indirect_action_index;
  /* The action_profile_mbr_exists flag is set to true when the entry is using
   * an Action Profile.  In this case, the P4_PD_ADT_UPDATE_ADD update for the
   * action_profile_mbr must be processed first.  Additionally, the logical
   * index specified by indirect_action_index must either be unoccupied or
   * occupied by an entry with a handle equal to action_profile_mbr.
   * Set to true when the entry is using action profile without selection. */
  bool action_profile_mbr_exists;
  /* When using Match-Select sel_grp_hdl specifies the group handle this entry
   * uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_entry_hdl_t sel_grp_hdl;
  /* When using Match-Select indirect_selection_index specifies the base
   * logical index the selection group uses.
   * Valid when sel_grp_exists is true. */
  p4_pd_idx_t indirect_selection_index;
  /* When using Match-Select num_selector_indices specifies the number logical
   * indexes the selection group uses.
   * Valid when sel_grp_exists is true. */
  uint32_t num_selector_indices;
  /* The sel_grp_exists flag is set to true when the entry is using Match-
   * Select.  In this case, each of the consecutive num_selector_indices
   * indexes starting at indirect_selection_index should have at least one
   * member.
   * Set to true when the entry uses action profile with seleciton. */
  bool sel_grp_exists;
  int logical_index_array_length;
  struct p4_pd_multi_index *location_array;
  void *data;
};
/* A union representing all the possible parameters to a MAT update. */
union p4_pd_mat_update_params {
  struct p4_pd_mat_update_set_dflt_params set_dflt;
  struct p4_pd_mat_update_clr_dflt_params clr_dflt;
  struct p4_pd_mat_update_add_params add;
  struct p4_pd_mat_update_del_params del;
  struct p4_pd_mat_update_mod_params mod;
  struct p4_pd_mat_update_mov_params mov;
  struct p4_pd_mat_update_add_multi_params add_multi;
  struct p4_pd_mat_update_mov_multi_params mov_multi;
};
typedef void (*p4_pd_mat_update_cb)(
    p4_pd_dev_target_t dev_tgt,
    p4_pd_tbl_hdl_t tbl_hdl,
    enum p4_pd_mat_update_type update_type,
    union p4_pd_mat_update_params *update_params,
    void *cookie);

enum p4_pd_adt_update_type {
  P4_PD_ADT_UPDATE_ADD,
  P4_PD_ADT_UPDATE_DEL,
  P4_PD_ADT_UPDATE_MOD
};
struct p4_pd_adt_update_add_params {
  p4_pd_entry_hdl_t ent_hdl;
  void *data;
};
struct p4_pd_adt_update_del_params {
  p4_pd_entry_hdl_t ent_hdl;
};
struct p4_pd_adt_update_mod_params {
  p4_pd_entry_hdl_t ent_hdl;
  void *data;
};
union p4_pd_adt_update_params {
  struct p4_pd_adt_update_add_params add;
  struct p4_pd_adt_update_del_params del;
  struct p4_pd_adt_update_mod_params mod;
};
typedef void (*p4_pd_adt_update_cb)(
    p4_pd_dev_target_t dev_tgt,
    p4_pd_tbl_hdl_t tbl_hdl,
    enum p4_pd_adt_update_type update_type,
    union p4_pd_adt_update_params *update_params,
    void *cookie);

enum p4_pd_sel_update_type {
  P4_PD_SEL_UPDATE_GROUP_CREATE,
  P4_PD_SEL_UPDATE_GROUP_DESTROY,
  P4_PD_SEL_UPDATE_ADD,
  P4_PD_SEL_UPDATE_DEL,
  P4_PD_SEL_UPDATE_ACTIVATE,
  P4_PD_SEL_UPDATE_DEACTIVATE,
  P4_PD_SEL_UPDATE_SET_FALLBACK,
  P4_PD_SEL_UPDATE_CLR_FALLBACK
};
struct p4_pd_sel_update_group_create_params {
  p4_pd_grp_hdl_t grp_hdl;
  uint32_t max_members;
  /* The number of consecutive logical indexes the group occupies. */
  uint32_t num_indexes;
  /* The first logical index the group uses.  The group uses all indexes in the
   * range of logical_base_index through base_logical_index+num_indexes-1. */
  p4_pd_idx_t base_logical_index;

  /* An array providing the locations used by this selection group in the
   * action table.  The array is a list of pairs where each pair gives a base
   * logical index and a count of consecutive indexes at that base used by the
   * selection group. */
  int logical_adt_index_array_length;
  struct p4_pd_multi_index *logical_adt_indexes;
};
struct p4_pd_sel_update_group_destroy_params {
  p4_pd_grp_hdl_t grp_hdl;
};
struct p4_pd_sel_update_add_params {
  p4_pd_grp_hdl_t grp_hdl;
  p4_pd_entry_hdl_t ent_hdl;
  p4_pd_idx_t logical_index;
  p4_pd_idx_t logical_subindex;
  void *data;
};
struct p4_pd_sel_update_del_params {
  p4_pd_grp_hdl_t grp_hdl;
  p4_pd_entry_hdl_t ent_hdl;
  p4_pd_idx_t logical_index;
  p4_pd_idx_t logical_subindex;
};
struct p4_pd_sel_update_activate_params {
  p4_pd_grp_hdl_t grp_hdl;
  p4_pd_entry_hdl_t ent_hdl;
  p4_pd_idx_t logical_index;
  p4_pd_idx_t logical_subindex;
};
struct p4_pd_sel_update_deactivate_params {
  p4_pd_grp_hdl_t grp_hdl;
  p4_pd_entry_hdl_t ent_hdl;
  p4_pd_idx_t logical_index;
  p4_pd_idx_t logical_subindex;
};
struct p4_pd_sel_update_set_fallback_params {
  p4_pd_entry_hdl_t ent_hdl;
  void *data;
};
union p4_pd_sel_update_params {
  struct p4_pd_sel_update_group_create_params grp_create;
  struct p4_pd_sel_update_group_destroy_params grp_destroy;
  struct p4_pd_sel_update_add_params add;
  struct p4_pd_sel_update_del_params del;
  struct p4_pd_sel_update_activate_params activate;
  struct p4_pd_sel_update_deactivate_params deactivate;
  struct p4_pd_sel_update_set_fallback_params set_fallback;
};
typedef void (*p4_pd_sel_update_cb)(
    p4_pd_dev_target_t dev_tgt,
    p4_pd_tbl_hdl_t tbl_hdl,
    enum p4_pd_sel_update_type update_type,
    union p4_pd_sel_update_params *update_params,
    void *cookie);

/* Placement Operations */
struct p4_pd_plcmt_info;
struct p4_pd_multi_index {
  p4_pd_idx_t logical_index_base;
  uint8_t logical_index_count;
};
struct p4_pd_plcmt_info *p4_pd_create_plcmt_info();
void p4_pd_destroy_plcmt_info(struct p4_pd_plcmt_info *info);
p4_pd_status_t p4_pd_set_one_plcmt_op_mat_add(
    struct p4_pd_plcmt_info *info,
    p4_pd_entry_hdl_t ent_hdl,
    bf_dev_pipe_t pipe,
    p4_pd_idx_t ent_idx,
    p4_pd_idx_t indirect_selection_idx,
    p4_pd_idx_t indirect_action_idx,
    void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_mat_mov(
    struct p4_pd_plcmt_info *info,
    p4_pd_entry_hdl_t ent_hdl,
    p4_pd_idx_t ent_idx,
    p4_pd_idx_t indirect_selection_index,
    p4_pd_idx_t indirect_action_index,
    void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_mat_mod(
    struct p4_pd_plcmt_info *info,
    p4_pd_entry_hdl_t ent_hdl,
    p4_pd_idx_t indirect_selection_index,
    p4_pd_idx_t indirect_action_index,
    void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_mat_del(struct p4_pd_plcmt_info *info,
                                              p4_pd_entry_hdl_t ent_hdl);
p4_pd_status_t p4_pd_set_one_plcmt_op_mat_set_dflt(
    struct p4_pd_plcmt_info *info,
    p4_pd_entry_hdl_t ent_hdl,
    bf_dev_pipe_t pipe,
    p4_pd_idx_t indirect_selection_idx,
    p4_pd_idx_t indirect_action_idx,
    void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_mat_clr_dflt(
    struct p4_pd_plcmt_info *info,
    p4_pd_entry_hdl_t ent_hdl,
    bf_dev_pipe_t pipe);
p4_pd_status_t p4_pd_set_one_plcmt_op_mat_add_multi(
    struct p4_pd_plcmt_info *info,
    p4_pd_entry_hdl_t ent_hdl,
    bf_dev_pipe_t pipe,
    p4_pd_idx_t indirect_selection_idx,
    p4_pd_idx_t indirect_action_idx,
    int array_length,
    struct p4_pd_multi_index *location_array,
    void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_mat_mov_multi(
    struct p4_pd_plcmt_info *info,
    p4_pd_entry_hdl_t ent_hdl,
    p4_pd_idx_t indirect_selection_idx,
    p4_pd_idx_t indirect_action_idx,
    int array_length,
    struct p4_pd_multi_index *location_array,
    void *ent_data);

p4_pd_status_t p4_pd_set_one_plcmt_op_adt_add(struct p4_pd_plcmt_info *info,
                                              p4_pd_entry_hdl_t ent_hdl,
                                              bf_dev_pipe_t pipe,
                                              void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_adt_mod(struct p4_pd_plcmt_info *info,
                                              p4_pd_entry_hdl_t ent_hdl,
                                              void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_adt_del(struct p4_pd_plcmt_info *info,
                                              p4_pd_entry_hdl_t ent_hdl);

p4_pd_status_t p4_pd_set_one_plcmt_op_sel_grp_create(
    struct p4_pd_plcmt_info *info,
    p4_pd_grp_hdl_t grp_hdl,
    bf_dev_pipe_t pipe,
    uint32_t num_indexes,
    uint32_t max_members,
    p4_pd_idx_t base_logical_index,
    int array_length,
    struct p4_pd_multi_index *location_array);
p4_pd_status_t p4_pd_set_one_plcmt_op_sel_grp_destroy(
    struct p4_pd_plcmt_info *info, p4_pd_grp_hdl_t grp_hdl, bf_dev_pipe_t pipe);
p4_pd_status_t p4_pd_set_one_plcmt_op_sel_add(struct p4_pd_plcmt_info *info,
                                              p4_pd_grp_hdl_t grp_hdl,
                                              p4_pd_entry_hdl_t ent_hdl,
                                              bf_dev_pipe_t pipe,
                                              p4_pd_idx_t ent_idx,
                                              p4_pd_idx_t ent_subidx,
                                              void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_sel_del(struct p4_pd_plcmt_info *info,
                                              p4_pd_grp_hdl_t grp_hdl,
                                              p4_pd_entry_hdl_t ent_hdl,
                                              bf_dev_pipe_t pipe,
                                              p4_pd_idx_t ent_idx,
                                              p4_pd_idx_t ent_subidx);
p4_pd_status_t p4_pd_set_one_plcmt_op_sel_activate(
    struct p4_pd_plcmt_info *info,
    p4_pd_grp_hdl_t grp_hdl,
    p4_pd_entry_hdl_t ent_hdl,
    bf_dev_pipe_t pipe,
    p4_pd_idx_t ent_idx,
    p4_pd_idx_t ent_subidx);
p4_pd_status_t p4_pd_set_one_plcmt_op_sel_deactivate(
    struct p4_pd_plcmt_info *info,
    p4_pd_grp_hdl_t grp_hdl,
    p4_pd_entry_hdl_t ent_hdl,
    bf_dev_pipe_t pipe,
    p4_pd_idx_t ent_idx,
    p4_pd_idx_t ent_subidx);
p4_pd_status_t p4_pd_set_one_plcmt_op_sel_set_fallback(
    struct p4_pd_plcmt_info *info,
    p4_pd_entry_hdl_t ent_hdl,
    bf_dev_pipe_t pipe,
    void *ent_data);
p4_pd_status_t p4_pd_set_one_plcmt_op_sel_clr_fallback(
    struct p4_pd_plcmt_info *info, bf_dev_pipe_t pipe);

p4_pd_status_t p4_pd_process_plcmt_info(p4_pd_sess_hdl_t sess_hdl,
                                        bf_dev_id_t dev_id,
                                        p4_pd_tbl_hdl_t tbl_hdl,
                                        struct p4_pd_plcmt_info *info,
                                        uint32_t api_flags,
                                        uint32_t *processed);

p4_pd_status_t p4_pd_plcmt_set_adt_ent_hdl(void *data,
                                           p4_pd_entry_hdl_t entry_hdl);

p4_pd_status_t p4_pd_plcmt_set_sel_grp_hdl(void *data, p4_pd_grp_hdl_t grp_hdl);

p4_pd_status_t p4_pd_plcmt_set_ttl(void *data, uint32_t ttl);
#endif
