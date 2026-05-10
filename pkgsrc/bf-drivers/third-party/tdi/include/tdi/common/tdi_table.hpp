// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
/** @file tdi_table.hpp
 *
 *  @brief Contains TDI Table APIs
 */
#ifndef _TDI_TABLE_HPP
#define _TDI_TABLE_HPP

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_attributes.hpp>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>
#include <tdi/common/tdi_operations.hpp>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_table_data.hpp>
#include <tdi/common/tdi_table_key.hpp>
#include <tdi/common/tdi_target.hpp>

namespace tdi {

// Fwd declaration
class TdiInfo;

/**
 * @brief Class to contain metadata of Table Objs like Data and Key Fields,
 *  and perform functions like EntryAdd, AttributeSet, OperationsExecute etc<br>
 * <B>Creation: </B> Cannot be created. can only be queried from \ref
 * tdi::TdiInfo
 */
class Table {
 public:
  /**
   * @brief Vector of pair of Key and Data. The key and data need to be
   * allocated
   * and pushed in the vector before being passed in to the appropriate Get API.
   */
  using keyDataPairs = std::vector<std::pair<tdi::TableKey *, TableData *>>;

  virtual ~Table() = default;

  /// Table APIs
  ///
  /**
   * @name Table APIs
   * @{
   */

  /**
   * @brief Add an entry to the table
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[in] key Entry Key
   * @param[in] data Entry Data
   *
   * @return Status of the API call
   */
  virtual tdi_status_t entryAdd(const tdi::Session &session,
                                const tdi::Target &dev_tgt,
                                const tdi::Flags &flags,
                                const tdi::TableKey &key,
                                const tdi::TableData &data) const;

  /**
   * @brief Modify an existing entry of the table
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[in] key Entry Key
   * @param[in] data Entry Data
   *
   * @return Status of the API call
   */
  virtual tdi_status_t entryMod(const tdi::Session &session,
                                const tdi::Target &dev_tgt,
                                const tdi::Flags &flags,
                                const tdi::TableKey &key,
                                const tdi::TableData &data) const;

  /**
   * @brief Delete an entry of the table
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[in] key Entry Key
   *
   * @return Status of the API call
   */
  virtual tdi_status_t entryDel(const tdi::Session &session,
                                const tdi::Target &dev_tgt,
                                const tdi::Flags &flags,
                                const tdi::TableKey &key) const;

  /**
   * @brief Clear a table. Delete all entries. This API also resets default
   * entry if present and is not const default. If table has always present
   * entries like Counter table, then this table resets all the entries
   * instead.
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   *
   * @return Status of the API call
   */
  virtual tdi_status_t clear(const tdi::Session &session,
                             const tdi::Target &dev_tgt,
                             const tdi::Flags &flags) const;

  /**
   * @brief Set the default Entry of the table. Data fields which aren't
   * set by the user and are not mandatory, are defaulted to default value
   *
   * @details There can be a P4 defined default entry with parameters. This API
   * modifies any existing default entry to the one passed in here. Note that
   * this API is idempotent and should be called either when modifying an
   * existing default entry or to program one newly. There could be a particular
   * action which is designated as a default-only action. In that case, an error
   * is returned if the action id of the data object passed in here is different
   * from the designated default action.
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[in] data Entry Data
   *
   * @return Status of the API call
   */
  virtual tdi_status_t defaultEntrySet(const tdi::Session &session,
                                       const tdi::Target &dev_tgt,
                                       const tdi::Flags &flags,
                                       const tdi::TableData &data) const;

  /**
   * @brief Modify the default entry of the table. Similar to defaultEntrySet
   * but fields which aren't set by the user, are left with previous values
   *
   * @details If there exists a default action, then only that action can be
   * used in the modify just like in set.
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[in] data Entry Data
   *
   * @return Status of the API call
   */
  virtual tdi_status_t defaultEntryMod(const tdi::Session &session,
                                       const tdi::Target &dev_tgt,
                                       const tdi::Flags &flags,
                                       const tdi::TableData &data) const;

  /**
   * @brief Reset the default Entry of the table
   *
   * @details The default entry of the table is reset to the P4 specified
   * default action with parameters, if it exists, else its reset to a "no-op"
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   *
   * @return Status of the API call
   */
  virtual tdi_status_t defaultEntryReset(const tdi::Session &session,
                                         const tdi::Target &dev_tgt,
                                         const tdi::Flags &flags) const;

  /**
   * @brief Get the default Entry of the table
   *
   * @details The default entry returned will be the one programmed or the
   * P4/json defined one, if it exists. Note that, when the entry is obtained
   * from software, the P4 defined default entry will not be available if the
   * default entry was not programmed ever. However, when the entry is obtained
   * from hardware, the P4 defined default entry will be returned even if the
   * default entry was not programmed explicitly.
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[inout] data Entry Data, if not empty will be used to filter
   *                    returned fields
   *
   * @return Status of the API call
   */
  virtual tdi_status_t defaultEntryGet(const tdi::Session &session,
                                       const tdi::Target &dev_tgt,
                                       const tdi::Flags &flags,
                                       tdi::TableData *data) const;

  /**
   * @brief Get an entry from the table
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[in] key Entry Key
   * @param[inout] data Entry Data, if not empty will be used to filter
   *                    returned fields
   *
   * @return Status of the API call
   */
  virtual tdi_status_t entryGet(const tdi::Session &session,
                                const tdi::Target &dev_tgt,
                                const tdi::Flags &flags,
                                const tdi::TableKey &key,
                                tdi::TableData *data) const;

  /**
   * @brief Get an entry from the table by handle
   *
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
  virtual tdi_status_t entryGet(const tdi::Session &session,
                                const tdi::Target &dev_tgt,
                                const tdi::Flags &flags,
                                const tdi_handle_t &entry_handle,
                                tdi::TableKey *key,
                                tdi::TableData *data) const;

  /**
   * @brief Get an entry key from the table by handle
   *
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
  virtual tdi_status_t entryKeyGet(const tdi::Session &session,
                                   const tdi::Target &dev_tgt,
                                   const tdi::Flags &flags,
                                   const tdi_handle_t &entry_handle,
                                   tdi::Target *entry_tgt,
                                   tdi::TableKey *key) const;

  /**
   * @brief Get an entry handle from the table
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[in] key Entry Key
   * @param[out] entry_handle Handle to the entry
   *
   * @return Status of the API call
   */
  virtual tdi_status_t entryHandleGet(const tdi::Session &session,
                                      const tdi::Target &dev_tgt,
                                      const tdi::Flags &flags,
                                      const tdi::TableKey &key,
                                      tdi_handle_t *entry_handle) const;

  /**
   * @brief Get the first entry of the table
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[out] key Entry Key
   * @param[inout] data Entry Data, if not empty will be used to filter
   *                    returned fields
   *
   * @return Status of the API call
   */
  virtual tdi_status_t entryGetFirst(const tdi::Session &session,
                                     const tdi::Target &dev_tgt,
                                     const tdi::Flags &flags,
                                     tdi::TableKey *key,
                                     tdi::TableData *data) const;

  /**
   * @brief Get next N entries of the table following the entry that is
   * specificed by key.
   * If the N queried for is greater than the actual entries, then
   * all the entries present are returned.
   * N must be greater than zero.
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[in] key Entry Key from which N entries are queried
   * @param[in] n Number of entries queried 'N'
   * @param[out] key_data_pairs Vector of N Pairs(key, data). This vector needs
   * to have N entries before the API call is made, else error is returned
   * @param[out] num_returned Actual number of entries returned
   *
   * @return Status of the API call
   */
  virtual tdi_status_t entryGetNextN(const tdi::Session &session,
                                     const tdi::Target &dev_tgt,
                                     const tdi::Flags &flags,
                                     const tdi::TableKey &key,
                                     const uint32_t &n,
                                     keyDataPairs *key_data_pairs,
                                     uint32_t *num_returned) const;

  /**
   * @brief Current Usage of the table
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[out] count Table usage
   *
   * @return Status of the API call
   */
  virtual tdi_status_t usageGet(const tdi::Session &session,
                                const tdi::Target &dev_tgt,
                                const tdi::Flags &flags,
                                uint32_t *count) const;
  /**
   * @brief The maximum size of the table. Note that this size might
   * be different than present in tdi.json especially for Match Action
   * Tables. This is because sometimes some tables in runtime might have
   * different sizes. For example MATs in some targets  might reserve
   * some space for atomic modfies and hence might be 1 or 2 < json size.
   *
   * To get the json size, use TableInfo::sizeGet()
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device target
   * @param[in] flags Call flags
   * @param[out] size Number of total available entries
   *
   * @return Status of the API call
   */
  virtual tdi_status_t sizeGet(const tdi::Session &session,
                               const tdi::Target &dev_tgt,
                               const tdi::Flags &flags,
                               size_t *size) const;
  /** @} */  // End of group Table
             /**
              * @brief Allocate key for the table
              *
              * @param[out] key_ret Key object returned
              *
              * @return Status of the API call
              */
  virtual tdi_status_t keyAllocate(
      std::unique_ptr<tdi::TableKey> *key_ret) const;
  /**
   * @brief Reset the key object associated with the table
   *
   * @param[inout] Pointer to a key object, previously allocated using
   * keyAllocate on the table.
   *
   * @return Status of the API call. Error is returned if the key object is not
   *associated with the table.
   */
  virtual tdi_status_t keyReset(tdi::TableKey *key) const;

  //// Data APIs
  /**
   * @name Data APIs
   * There are 2 base versions of every Data API. One, which takes in an
   * action_id and another which doesn't. If action_id is specified it will
   * always be set. Action IDs in Data objects can only be specified/set
   * during reset/allocate
   *
   * @{
   */
  /**
   * @brief Allocate Data Object for the table
   *
   * @param[out] data_ret Data Object returned
   *
   * @return Status of the API call
   */
  virtual tdi_status_t dataAllocate(
      std::unique_ptr<tdi::TableData> *data_ret) const;
  /**
   * @brief Allocate Data Object for the table
   *
   * @param[in] action_id Action ID
   * @param[out] data_ret Data Object returned
   *
   * @return Status of the API call
   */
  virtual tdi_status_t dataAllocate(
      const tdi_id_t &action_id,
      std::unique_ptr<tdi::TableData> *data_ret) const;

  /**
   * @brief Data Allocate with a list of field-ids. This allocates the data
   * object for
   * the list of field-ids. The field-ids passed must be valid for this table.
   * The Data Object then entertains APIs to read/write only those set of fields
   *
   * @param[in] fields Vector of field IDs
   * @param[out] data_ret Data Object returned
   *
   * @return Status of the API call
   */
  virtual tdi_status_t dataAllocate(
      const std::vector<tdi_id_t> &fields,
      std::unique_ptr<tdi::TableData> *data_ret) const;
  /**
   * @brief Data Allocate with a list of field-ids. This allocates the data
   * object for
   * the list of field-ids. The field-ids passed must be valid for this table.
   * The Data Object then entertains APIs to read/write only those set of fields
   *
   * @param[in] fields Vector of field IDs
   * @param[in] action_id Action ID
   * @param[out] data_ret Data Object returned
   *
   * @return Status of the API call
   */
  virtual tdi_status_t dataAllocate(
      const std::vector<tdi_id_t> &fields,
      const tdi_id_t &action_id,
      std::unique_ptr<tdi::TableData> *data_ret) const;

  /**
   * @brief Reset the data object previously allocated using dataAllocate on the
   * table
   *
   * @details Calling this API resets the action-id in the object to
   * 0 Typically this needs to be done when doing an entry get,
   * since the caller does not know the action-id associated with the entry.
   * Using the data object for an entry add on a table where action-id is
   * expected to be non-0 will result in an error.
   *
   * @param[in/out] data Pointer to the data object allocated using dataAllocate
   * on the table.
   *
   * @return Status of the API call. An error is returned if the data object is
   * not associated with the table
   */
  virtual tdi_status_t dataReset(tdi::TableData *data) const;

  /**
   * @brief Reset the data object previously allocated using dataAllocate on the
   * table
   *
   * @details Calling this API sets the action-id in the object to the
   * passed in value.
   *
   * @param[in] action_id  new action id of the object
   * the table.
   * @param[in/out] Pointer to the data object allocated using dataAllocate on
   * the table.
   *
   * @return Status of the API call. An error is returned if the data object is
   * not associated with the table or if action-id is not applicable on the
   * table.
   */
  virtual tdi_status_t dataReset(const tdi_id_t &action_id,
                                 tdi::TableData *data) const;

  /**
   * @brief Reset the data object previously allocated using dataAllocate on the
   *table
   *
   * @details Calling this API resets the action-id in the object to
   * 0 Typically this needs to be done when doing an entry get,
   * since the caller does not know the action-id associated with the entry.
   * Using the data object for an entry add on a table where action-id is
   * applicable will result in an error. The data object will contain the passed
   * in vector of field-ids active. This is typically done when reading an
   * entry's fields. Note that, the fields passed in must be common data fields
   * across all action-ids (common data fields, such as direct counter/direct
   * meter etc), for tables on which action-id is applicable.
   *
   * @param[in] fields Vector of field-ids that are to be activated in the data
   * object the table.
   * @param[in/out] Pointer to the data object allocated using dataAllocate on
   * the table.
   *
   * @return Status of the API call. An error is returned if the data object is
   * not associated with the table, or if any field-id is action-specific, for
   * tables on which action-id is applicable..
   */
  virtual tdi_status_t dataReset(const std::vector<tdi_id_t> &fields,
                                 tdi::TableData *data) const;

  /**
   * @brief Reset the data object previously allocated using dataAllocate on the
   * table
   *
   * @details Calling this API sets the action-id in the object to the
   * passed in value and the list of fields passed in will be active in the data
   * object. The list of fields passed in must belong to the passed in action-id
   * or common across all action-ids associated with the table.
   *
   * @param[in] fields Vector of field-ids that are to be activated in the data
   * object the table.
   * @param[in] action_id  new action id of the object
   * the table.
   * @param[in/out] Pointer to the data object allocated using dataAllocate on
   * the table.
   *
   * @return Status of the API call. An error is returned if the data object is
   * not associated with the table or if action-id is not applicable on the
   * table.
   */
  virtual tdi_status_t dataReset(const std::vector<tdi_id_t> &fields,
                                 const tdi_id_t &action_id,
                                 tdi::TableData *data) const;

  /** @} */  // End of group Data

  // table attribute APIs
  /**
   * @name Attribute APIs
   * @{
   */
  /**
   * @brief Allocate Attribute object of the table.
   *
   * @param[in]  type Type of the attribute
   * @param[out] attr Attribute object returned
   *
   * @return Status of the API call
   */
  virtual tdi_status_t attributeAllocate(
      const tdi_attributes_type_e &type,
      std::unique_ptr<tdi::TableAttributes> *attr) const;

  /**
   * @brief Reset Attribute object of the table.
   *
   * @param[in]  type Type of the attribute
   * @param[out] attr Attribute object returned
   *
   * @return Status of the API call
   */
  virtual tdi_status_t attributeReset(
      const tdi_attributes_type_e &type,
      std::unique_ptr<tdi::TableAttributes> *attr) const;

  /**
   * @brief Apply an Attribute from an Attribute Object on the
   * table. Before using this API, the Attribute object needs to
   * be allocated and all the necessary values need to
   * be set on the Attribute object
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device Target
   * @param[in] flags Call flags
   * @param[in] tableAttributes Attribute Object
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tableAttributesSet(
      const tdi::Session &session,
      const tdi::Target &dev_tgt,
      const tdi::Flags &flags,
      const tdi::TableAttributes &tableAttributes) const;

  /**
   * @brief Get the current Attributes set on the table. The attribute
   * object passed in as input param needs to be allocated first with
   * the required attribute type.
   *
   * @param[in] session Session Object
   * @param[in] dev_tgt Device Target
   * @param[in] flags Call flags
   * @param[out] tableAttributes Attribute Object
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tableAttributesGet(
      const tdi::Session &session,
      const tdi::Target &dev_tgt,
      const tdi::Flags &flags,
      tdi::TableAttributes *tableAttributes) const;

  /** @} */  // End of group Attributes

  // table operations APIs
  /**
   * @name Operations APIs
   * @{
   */
  /**
   * @brief Allocate Operations object
   *
   * @param[in] type Operations type
   * @param[out] table_ops Operations Object returned
   *
   * @return Status of the API call
   */
  virtual tdi_status_t operationsAllocate(
      const tdi_operations_type_e &type,
      std::unique_ptr<tdi::TableOperations> *table_ops) const;

  /**
   * @brief Execute Operations on a table. User
   * needs to allocate operation object with correct
   * type and then pass it to this API
   *
   * @param[in] tableOperations Operations Object
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tableOperationsExecute(
      const tdi::TableOperations &tableOperations) const;
  /** @} */  // End of group Operations
  const TableInfo *tableInfoGet() const { return table_info_; }

  /**
   * @brief Are Action IDs applicable for this table? This is
   * not always equivalent to whether actions are present or not.
   * It depends upon table type. For example, for Match Action
   * Tables, this should always return true even if no actions
   * exist for a specific MAT
   *
   * @retval True : If Action ID applicable
   * @retval False : If not
   *
   */
  virtual bool actionIdApplicable() const { return false; }

  const TdiInfo *tdiInfoGet() const { return tdi_info_; };

 protected:
  // Targets can choose to use any ctor to create tables. The 2nd one
  // assists with setting table APIs during runtime rather than
  // relying on json schema. If using 2nd ctor and json schema
  // is also provided then it is overridden
  Table(const TdiInfo *tdi_info, const TableInfo *table_info)
      : tdi_info_(tdi_info), table_info_(table_info){};
  Table(const TdiInfo *tdi_info,
        const tdi::SupportedApis table_apis,
        const TableInfo *table_info)
      : tdi_info_(tdi_info), table_info_(table_info) {
    table_info_->apiSupportedSet(std::move(table_apis));
  };

 private:
  // Backpinter to the TdiInfo
  const TdiInfo *tdi_info_;
  // The TableInfo class containing all the metadata from tdi.json
  const TableInfo *table_info_;
  friend tdi::TdiInfo;
};  // end of tdi::Table

}  // namespace tdi

#endif  // _TDI_TABLE_HPP
