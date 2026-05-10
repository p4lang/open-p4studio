/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tdi_table.h
 *
 *  @brief Contains TDI Table APIs
 */
#ifndef _TDI_TABLE_H
#define _TDI_TABLE_H

#include <tdi/common/c_frontend/tdi_attributes.h>
#include <tdi/common/c_frontend/tdi_operations.h>
#include <tdi/common/c_frontend/tdi_table_data.h>
#include <tdi/common/c_frontend/tdi_table_key.h>
#include <tdi/common/tdi_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Table APIs */

/**
 * @brief Add an entry to the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key
 * @param[in] data Entry Data
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_add(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 const tdi_table_data_hdl *data);

/**
 * @brief Modify an existing entry of the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key
 * @param[in] data Entry Data
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_mod(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 const tdi_table_data_hdl *data);

/**
 * @brief Modify only an existing default entry of the table.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] data Entry Data
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_default_entry_mod(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *target,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_data_hdl *data);

/**
 * @brief Delete an entry of the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_del(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *dev_tgt,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key);

/**
 * @brief Clear a table. Delete all entries. This API also resets default
 * entry if present and is not const default. If table has always present
 * entries like Counter table, then this table resets all the entries
 * instead.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_clear(const tdi_table_hdl *table_hdl,
                             const tdi_session_hdl *session,
                             const tdi_target_hdl *dev_tgt,
                             const tdi_flags_hdl *flags);

/**
 * @brief Get an entry from the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key
 * @param[inout] data Entry Data, if not empty will be used to filter
 *                    returned fields
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_get(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *dev_tgt,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 tdi_table_data_hdl *data);

/**
 * @brief Get an entry from the table by entry handle
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] entry_handle Handle to the entry
 * @param[out] key Entry Key
 * @param[inout] data Entry Data, if not empty will be used to filter
 *                    returned fields
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_get_by_handle(const tdi_table_hdl *table_hdl,
                                           const tdi_session_hdl *session,
                                           const tdi_target_hdl *dev_tgt,
                                           const tdi_flags_hdl *flags,
                                           const uint32_t entry_handle,
                                           tdi_table_key_hdl *key,
                                           tdi_table_data_hdl *data);

/**
 * @brief Get the first entry of the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[out] key Entry Key
 * @param[inout] data Entry Data, if not empty will be used to filter
 *                    returned fields
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_get_first(const tdi_table_hdl *table_hdl,
                                       const tdi_session_hdl *session,
                                       const tdi_target_hdl *dev_tgt,
                                       const tdi_flags_hdl *flags,
                                       tdi_table_key_hdl *key,
                                       tdi_table_data_hdl *data);

/**
 * @brief Get an entry key from the table by handle
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] entry_handle Handle to the entry
 * @param[out] entry_tgt Target device for specified entry handle,
 *                       may not match dev_tgt values
 * @param[out] key Entry Key
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_key_get(const tdi_table_hdl *table_hdl,
                                     const tdi_session_hdl *session,
                                     const tdi_target_hdl *dev_tgt_in,
                                     const tdi_flags_hdl *flags,
                                     const uint32_t entry_handle,
                                     tdi_target_hdl *dev_tgt_out,
                                     tdi_table_key_hdl *key);

/**
 * @brief Get an entry handle from the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] entry_handle Handle to the entry
 * @param[out] key Entry Key
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_handle_get(const tdi_table_hdl *table_hdl,
                                        const tdi_session_hdl *session,
                                        const tdi_target_hdl *dev_tgt,
                                        const tdi_flags_hdl *flags,
                                        const tdi_table_key_hdl *key,
                                        uint32_t *entry_handle);

/**
 * @brief Get next N entries of the table following the entry that is
 * specificed by key.
 * If the N queried for is greater than the actual entries, then
 * all the entries present are returned.
 * N must be greater than zero.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key from which N entries are queried
 * @param[out] output_keys Array of allocated Key objects.
 * Size should be equal to n
 * @param[out] output_data Array of allocated Data objects.
 * Size should be equal to n
 * @param[in] n Number of entries queried 'N'
 * @param[out] num_returned Actual number of entries returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_get_next_n(const tdi_table_hdl *table_hdl,
                                        const tdi_session_hdl *session,
                                        const tdi_target_hdl *dev_tgt,
                                        const tdi_flags_hdl *flags,
                                        const tdi_table_key_hdl *key,
                                        tdi_table_key_hdl **output_keys,
                                        tdi_table_data_hdl **output_data,
                                        uint32_t n,
                                        uint32_t *num_returned);

/**
 * @brief Current Usage of the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[out] count Table usage
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_usage_get(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *dev_tgt,
                                 const tdi_flags_hdl *flags,
                                 uint32_t *count);
/**
 * @brief Set the default Entry of the table
 *
 * @details There can be a P4 defined default entry with parameters. This API
 * modifies any existing default entry to the one passed in here. Note that
 * this API is idempotent and should be called either when modifying an
 * existing default entry or to program one newly. There could be a particular
 * action which is designated as a default-only action. In that case, an error
 * is returned if the action id of the data object passed in here is different
 * from the designated default action.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] data Entry Data
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_default_entry_set(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *dev_tgt,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_data_hdl *data);

/**
 * @brief Get the default Entry of the table
 *
 * @details The default entry returned will be the one programmed or the P4
 * defined one, if it exists. Note that, when the entry is obtained from
 * software, the P4 defined default entry will not be available if the default
 * entry was not programmed ever. However, when the entry is obtained from
 * hardware, the P4 defined default entry will be returned even if the default
 * entry was not programmed ever.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[inout] data Entry Data, if not empty will be used to filter
 *                    returned fields
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_default_entry_get(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *dev_tgt,
                                         const tdi_flags_hdl *flags,
                                         tdi_table_data_hdl *data);

/**
 * @brief Reset the default Entry of the table
 *
 * @details The default entry of the table is reset to the P4 specified
 * default action with parameters, if it exists, else its reset to a "no-op"
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_default_entry_reset(const tdi_table_hdl *table_hdl,
                                           const tdi_session_hdl *session,
                                           const tdi_target_hdl *dev_tgt,
                                           const tdi_flags_hdl *flags);

/**
 * @brief The maximum size of the table. Note that this size might
 * be different than present in tdi.json especially for Match Action
 * Tables. This is because sometimes MATs might reserve some space for
 * atomic modfies and hence might be 1 or 2 < json size
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[out] size Number of total available entries
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_size_get(const tdi_table_hdl *table_hdl,
                                const tdi_session_hdl *session,
                                const tdi_target_hdl *dev_tgt,
                                const tdi_flags_hdl *flags,
                                size_t *size);

/******************** Key APIs *******************/

/**
 * @brief Allocate key for the table
 *
 * @param[in] table_hdl Table object
 * @param[out] key_hdl_ret Key object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_key_allocate(const tdi_table_hdl *table_hdl,
                                    tdi_table_key_hdl **key_hdl_ret);

/**
 * @brief Reset the key object associated with the table
 *
 * @param[in] table_hdl Table object
 * @param[inout] key_hdl_ret Pointer to a key object, previously allocated using
 * tdi_table_key_allocate() on the table.
 *
 * @return Status of the API call. Error is returned if the key object is not
 * associated with the table.
 */
tdi_status_t tdi_table_key_reset(const tdi_table_hdl *table_hdl,
                                 tdi_table_key_hdl **key_hdl_ret);

/**
 * @brief Deallocate key for the table
 *
 * @param[in] table_hdl Table object
 * @param[in] key_hdl Key object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_key_deallocate(tdi_table_key_hdl *key_hdl);

/******************** Data APIs *******************/

/**
 * @brief Allocate Data Object for the table
 *
 * @param[in] table_hdl Table object
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_data_allocate(const tdi_table_hdl *table_hdl,
                                     tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Allocate Data Object for the table
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_action_data_allocate(const tdi_table_hdl *table_hdl,
                                            const tdi_id_t action_id,
                                            tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Data Allocate with a list of field-ids. This allocates the data
 * object for
 * the list of field-ids. The field-ids passed must be valid for this table.
 * The Data Object then entertains APIs to read/write only those set of fields
 *
 * @param[in] table_hdl Table object
 * @param[in] fields Array of field IDs
 * @param[in] num_array Size of input array
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_data_allocate_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Data Allocate with a list of field-ids. This allocates the data
 * object for
 * the list of field-ids. The field-ids passed must be valid for this table.
 * The Data Object then entertains APIs to read/write only those set of fields
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[in] fields Array of field IDs
 * @param[in] num_array Size of input array
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_action_data_allocate_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Allocate Data Object for a container field
 *
 * @note: Users do not need to deallocate this object. After sending this object
 * as part of setValue, the parent Data object takes ownership. This will get
 * freed when parent is deallocated. For getValue, a previous allocate isn't
 * required.
 * However, if setValue fails for some reason and user needs to deallocate, then
 * a \ref tdi_table_data_deallocate() can be called
 *
 * @param[in] table_hdl Table object
 * @param[in] c_field_id Container field ID
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_data_allocate_container(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Allocate Data Object for a container field
 *
 * @note: Users do not need to deallocate this object. After sending this object
 * as part of setValue, the parent Data object takes ownership. This will get
 * freed when parent is deallocated. For getValue, a previous allocate isn't
 * required.
 * However, if setValue fails for some reason and user needs to deallocate, then
 * a \ref tdi_table_data_deallocate() can be called
 *
 * @param[in] table_hdl Table object
 * @param[in] c_field_id Container field ID
 * @param[in] action_id Action ID
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_action_data_allocate_container(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t action_id,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Data Allocate for container with a list of field-ids.
 * This allocates the data object for
 * the list of field-ids. The field-ids passed must be valid for this table
 * and container.
 * The Data Object then entertains APIs to read/write only those set of fields
 *
 * @note: Users do not need to deallocate this object. After sending this object
 * as part of setValue, the parent Data object takes ownership. This will get
 * freed when parent is deallocated. For getValue, a previous allocate isn't
 * required.
 * However, if setValue fails for some reason and user needs to deallocate, then
 * a \ref tdi_table_data_deallocate() can be called
 *
 * @param[in] table_hdl Table object
 * @param[in] c_field_id Container field ID
 * @param[in] fields Array of field IDs
 * @param[in] num_array Size of input array
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_data_allocate_container_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Data Allocate for container with a list of field-ids.
 * This allocates the data object for
 * the list of field-ids. The field-ids passed must be valid for this table
 * and container.
 * The Data Object then entertains APIs to read/write only those set of fields
 *
 * @note: Users do not need to deallocate this object. After sending this object
 * as part of setValue, the parent Data object takes ownership. This will get
 * freed when parent is deallocated. For getValue, a previous allocate isn't
 * required.
 * However, if setValue fails for some reason and user needs to deallocate, then
 * a \ref tdi_table_data_deallocate() can be called
 *
 * @param[in] table_hdl Table object
 * @param[in] c_field_id Container field ID
 * @param[in] action_id Action ID
 * @param[in] fields Array of field IDs
 * @param[in] num_array Size of input array
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_action_data_allocate_container_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Reset the data object previously allocated using dataAllocate on the
 * table
 *
 * @details Calling this API resets the action-id in the object to 0.
 * Typically this needs to be done when doing an entry get,
 * since the caller does not know the action-id associated with the entry.
 *
 * @param[in] table_hdl Table object
 * @param[inout] data_hdl_ret Pointer to the data object allocated using
 * dataAllocate on the table.
 *
 * @return Status of the API call. An error is returned if the data object is
 * not associated with the table
 */
tdi_status_t tdi_table_data_reset(const tdi_table_hdl *table_hdl,
                                  tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Reset the data object previously allocated using dataAllocate on the
 *table
 *
 * @details Calling this API sets the action-id in the object to the
 * passed in value.
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id  new action id of the object the table.
 * @param[in/out] data_hdl_ret Pointer to the data object allocated using
 * dataAllocate on
 * the table.
 *
 * @return Status of the API call. An error is returned if the data object is
 * not associated with the table or if action_id doesn't exist
 */
tdi_status_t tdi_table_action_data_reset(const tdi_table_hdl *table_hdl,
                                         const tdi_id_t action_id,
                                         tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Reset the data object previously allocated using dataAllocate on the
 * table
 *
 * @details Calling this API resets the action-id in the object to an
 * invalid value. Typically this needs to be done when doing an entry get,
 * since the caller does not know the action-id associated with the entry.
 * Using the data object for an entry add on a table where action-id is
 * expected will result in an error. The data object will contain the passed
 * in vector of field-ids active. This is typically done when reading an
 * entry's fields. Note that, the fields passed in must be common data fields
 * across all action-ids (common data fields, such as direct counter/direct
 * meter etc)
 *
 * @param[in] table_hdl Table object
 * @param[in] fields Array of field-ids that are to be activated in the data
 *object the table.
 * @param[in] num_array Array size of fields
 * @param[in/out] data_hdl_ret Pointer to the data object allocated using
 * dataAllocate on the table.
 *
 * @return Status of the API call. An error is returned if the data object is
 *not associated with the table
 */
tdi_status_t tdi_table_data_reset_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Reset the data object previously allocated using dataAllocate on the
 *table
 *
 * @details Calling this API sets the action-id in the object to the
 * passed in value and the list of fields passed in will be active in the data
 * object. The list of fields passed in must belong to the passed in action-id
 * or common across all action-ids associated with the table.
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id  new action id of the object the table.
 * @param[in] fields Array of field-ids that are to be activated in the data
 * object of the table.
 * @param[in] num_array Input field array size
 * @param[in/out] data_hdl_ret Pointer to the data object allocated using
 *dataAllocate on the table.
 *
 * @return Status of the API call. An error is returned if the data object is
 * not associated with the table
 */
tdi_status_t tdi_table_action_data_reset_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Deallocate data object. Only one version of data deallocate API exists
 *
 * @param[in] data_hdl Data object
 *
 * @return Status of API call
 */
tdi_status_t tdi_table_data_deallocate(tdi_table_data_hdl *data_hdl);
/**
 * @brief Are Action IDs applicable for this table
 *
 * @param[in] table_hdl Table object
 * @param ret_val If actions are applicable
 *
 * @return Status of API call
 */
tdi_status_t tdi_action_id_applicable(const tdi_table_hdl *table_hdl,
                                      bool *ret_val);

/********************** Attribute APIs ***********************/

/**
 * @brief Allocate attribute object.
 *
 * @param[in] tbl_hdl Table handle
 * @param[in] attribute_type enum value of attribute type
 * @param[out] tbl_attr_hdl
 *
 * @return Status of API call
 */
tdi_status_t tdi_attributes_allocate(const tdi_table_hdl *table_hdl,
                                     const tdi_attributes_type_e type,
                                     tdi_attributes_hdl **tbl_attr_hdl);

/**
 * @brief Deallocate attribute object
 *
 * @param[in] tbl_attr_hdl
 *
 * @return Status of API call
 */
tdi_status_t tdi_attributes_deallocate(tdi_attributes_hdl *tbl_attr_hdl);

/**
 * @brief Apply an Attribute from an Attribute Object on the
 * table. Before using this API, the Attribute object needs to
 * be allocated and all the necessary values need to
 * be set on the Attribute object
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device Target
 * @param[in] flags Call flags
 * @param[in] tableAttributes Attribute Object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_attributes_set(const tdi_table_hdl *table_hdl,
                                      const tdi_session_hdl *session,
                                      const tdi_target_hdl *dev_tgt,
                                      const tdi_flags_hdl *flags,
                                      const tdi_attributes_hdl *tbl_attr);

/**
 * @brief Get the current value of an Attribute set on the table.
 * The attribute object passed in as input param needs to be allocated
 * first with the required attribute type.
 * After this API call, invidual gets need to be called on the attribute
 * object. Refer to tdi_attributes.h for those APIs
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device Target
 * @param[in] flags Call flags
 * @param[out] tbl_attr Attribute Object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_attributes_get(const tdi_table_hdl *table_hdl,
                                      const tdi_session_hdl *session,
                                      const tdi_target_hdl *dev_tgt,
                                      const tdi_flags_hdl *flags,
                                      tdi_attributes_hdl *tbl_attr);

/********************* Operations APIs ***********************/

/**
 * @brief Allocate operations object for a particular
 * operation op_type. If operations is not supported on table,
 * API will fail
 *
 * @param[in] table_hdl Table object
 * @param[in] op_type Operation type
 * @param[out] tbl_ops Attribute object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_operations_allocate(const tdi_table_hdl *table_hdl,
                                     const tdi_operations_type_e op_type,
                                     tdi_operations_hdl **tbl_ops);

/**
 * @brief Deallocate operations object
 *
 * @param[in] tbl_attr_hdl
 *
 * @return Status of API call
 */
tdi_status_t tdi_operations_deallocate(tdi_operations_hdl *tbl_op_hdl);

/**
 * @brief Execute Operations on a table. User
 * needs to allocate operation object with correct
 * type and then pass it to this API
 *
 * @param[in] table_hdl Table object
 * @param[in] tbl_ops Operations Object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_operations_execute(const tdi_table_hdl *table_hdl,
                                          const tdi_operations_hdl *tbl_ops);

#ifdef __cplusplus
}
#endif

#endif  //_TDI_TABLE_H
