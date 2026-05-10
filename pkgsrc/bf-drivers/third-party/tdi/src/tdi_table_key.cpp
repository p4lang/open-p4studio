// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0

#include <tdi/common/tdi_table_key.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

tdi_status_t TableKey::setValue(const tdi_id_t &field_id,
                                const tdi::KeyFieldValue &&field_value) {
  return this->setValue(field_id, field_value);
}

tdi_status_t TableKey::setValue(const tdi_id_t & /*field_id*/,
                                const tdi::KeyFieldValue & /*field_value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableKey::getValue(const tdi_id_t & /*field_id*/,
                                tdi::KeyFieldValue * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableKey::tableGet(const Table **table) const {
  *table = table_;
  return TDI_SUCCESS;
}

tdi_status_t TableKey::reset() {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

}  // namespace tdi
