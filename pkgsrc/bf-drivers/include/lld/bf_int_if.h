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


#ifndef BF_INT_IF_H_INCLUDED
#define BF_INT_IF_H_INCLUDED

/* Allow the use in C++ code.  */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file bf_int_if.h
 * \brief Details Device-level APIs.
 *
 */

/**
 * @addtogroup lld-int-api
 * @{
 * This is a description of some APIs.
 */

/** \brief Value corresponding to the MSIx interrupt number for an interrupt
 *         This value is used to identofy a specific interrupt in the
 *         shadow_int[16] registers (even if not using MSI-x interrupts).
 *         0-511 for tofino.
 */
typedef uint32_t bf_int_nbr_t;

/** \brief Tofino (logical) interrupt numbers
 */
typedef enum {
  bf_tof_int_lo = 0,
  bf_tof_int_host_lo = 0,
  bf_tof_int_host_hi = 31,
  bf_tof_int_tbus_lo = 32,
  bf_tof_int_tbus_hi = 63,
  bf_tof_int_cbus_lo = 64,
  bf_tof_int_cbus_hi = 95,  // 2nd word, shadow_int[3], unused
  bf_tof_int_pbus_lo = 128,
  bf_tof_int_pbus_hi = 255,
  bf_tof_int_mbus_lo = 256,
  bf_tof_int_mbus_hi = 511,  // only 140 used?
  bf_tof_int_hi = 511,
} bf_tof_int_nbr_t;

/** \brief Tofino2 (logical) interrupt numbers
 */
typedef enum {
  bf_tof2_int_lo = 0,
  bf_tof2_int_host_lo = 0,
  bf_tof2_int_host_hi = 31,
  bf_tof2_int_tbus_lo = 32,
  bf_tof2_int_tbus_hi = 63,
  bf_tof2_int_cbus_lo = 64,
  bf_tof2_int_cbus_hi = 127,
  bf_tof2_int_mbus_lo = 128,
  bf_tof2_int_mbus_hi = 255,
  bf_tof2_int_pbus_lo = 256,
  bf_tof2_int_pbus_hi = 511,
  bf_tof2_int_hi = 511,
} bf_tof2_int_nbr_t;

/** \brief Tofino3 (logical) interrupt numbers
 */
typedef enum {
  bf_tof3_int_lo = 0,
  bf_tof3_int_host_lo = 0,
  bf_tof3_int_host_hi = 31,
  bf_tof3_int_tbus_lo = 32,
  bf_tof3_int_tbus_hi = 63,
  bf_tof3_int_cbus_lo = 64,
  bf_tof3_int_cbus_hi = 127,
  bf_tof3_int_mbus_lo = 128,
  bf_tof3_int_mbus_hi = 255,
  bf_tof3_int_pbus_lo = 256,
  bf_tof3_int_pbus_hi = 511,
  bf_tof3_int_hi = 511,
} bf_tof3_int_nbr_t;

/** \brief Tofino MSI-x interrupt numbers
 */
typedef enum {
  bf_tof_msi_x_int_host_lo = bf_tof_int_host_lo,
  bf_tof_msi_x_int_host_hi = bf_tof_int_host_hi,
  bf_tof_msi_x_int_tbus_lo = bf_tof_int_tbus_lo,
  bf_tof_msi_x_int_tbus_hi = bf_tof_int_tbus_hi,
  bf_tof_msi_x_int_cbus_lo = bf_tof_int_cbus_lo,
  bf_tof_msi_x_int_cbus_hi =
      bf_tof_int_cbus_hi,  // 2nd word, shadow_int[3], unused
  bf_tof_msi_x_int_pbus_lo = bf_tof_int_pbus_lo,
  bf_tof_msi_x_int_pbus_hi = bf_tof_int_pbus_hi,
  bf_tof_msi_x_int_mbus_lo = bf_tof_int_mbus_lo,
  bf_tof_msi_x_int_mbus_hi = bf_tof_int_mbus_hi,  // only 140 used?
} bf_msi_x_int_nbr_t;

/** \brief Tofino MSI interrupt numbers
 */
typedef enum {
  bf_tof_msi_int_0_switch = 0,
  bf_tof_msi_int_1_tbus = 1,
} bf_msi_int_nbr_t;

// interrupt ownership claim fns
bf_status_t bf_int_claim(bf_dev_id_t dev_id,
                         bf_subdev_id_t subdev_id,
                         bf_int_nbr_t int_nbr);
bf_status_t bf_int_claim_range(bf_dev_id_t dev_id,
                               bf_subdev_id_t subdev_id,
                               bf_int_nbr_t int_nbr_hi,
                               bf_int_nbr_t int_nbr_lo);
bf_status_t bf_int_claim_all(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_claim_host(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_claim_tbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_claim_cbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_claim_pbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_claim_mbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);

// interrupt service fns
bf_status_t bf_int_svc_range(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             bf_int_nbr_t int_nbr_hi,
                             bf_int_nbr_t int_nbr_lo);
bf_status_t bf_int_svc_all(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_svc_host(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_svc_tbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_svc_cbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_svc_pbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_svc_mbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);

// interrupt enable fns
bf_status_t bf_int_ena(bf_dev_id_t dev_id,
                       bf_subdev_id_t subdev_id,
                       bf_int_nbr_t int_nbr);
bf_status_t bf_int_ena_range(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             bf_int_nbr_t int_nbr_hi,
                             bf_int_nbr_t int_nbr_lo);
bf_status_t bf_int_ena_all(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_ena_host(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_ena_tbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_ena_cbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_ena_pbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_ena_mbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);

// interrupt disable fns
bf_status_t bf_int_msk(bf_dev_id_t dev_id,
                       bf_subdev_id_t subdev_id,
                       bf_int_nbr_t int_nbr);
bf_status_t bf_int_msk_range(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             bf_int_nbr_t int_nbr_hi,
                             bf_int_nbr_t int_nbr_lo);
bf_status_t bf_int_msk_all(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_msk_host(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_msk_tbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_msk_cbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_msk_pbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t bf_int_msk_mbus(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);

// MSI-x
bf_status_t bf_int_msi_x_svc(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             bf_msi_x_int_nbr_t msi_x_int,
                             bool msk);
bf_status_t bf_int_msix_map_set(bf_dev_id_t dev_id,
                                bf_subdev_id_t subdev_id,
                                bf_int_nbr_t int_nbr,
                                int msix_num);
bf_status_t bf_msix_map_view_init(bf_dev_id_t dev_id, int irq, int msix);
bf_status_t bf_int_svc_common(bf_dev_id_t dev_id,
                              bf_subdev_id_t subdev_id,
                              bool msk);
// MSI
bf_status_t bf_int_msi_svc(bf_dev_id_t dev_id,
                           bf_subdev_id_t subdev_id,
                           bf_msi_int_nbr_t msi_int,
                           bool msk);
// INTx (legacy)
bf_status_t bf_int_int_x_svc(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             bool msk);

/** \brief User-defined function (with the given signature) to be called
 *         whenever the an interrupt is detected in "status_reg" and the
 *         user registered a handler for it (with this signature).
 *
 *         Note: One or both "enable" registers may be -1 (0xffffffff).
 *               Check before using.
 */
typedef uint32_t (*bf_int_cb)(bf_dev_id_t dev_id,
                              bf_subdev_id_t subdev_id,
                              uint32_t status_reg,
                              uint32_t status_value,
                              uint32_t enable_hi_reg,
                              uint32_t enable_lo_reg,
                              void *userdata);

bf_status_t bf_interrupt_service(bf_dev_id_t dev_id);
bf_status_t bf_bind_interrupt_cb(bf_dev_id_t dev_id,
                                 bf_subdev_id_t subdev_id,
                                 uint32_t offset,
                                 bf_int_cb fn,
                                 void *userdata);

/**
 * @addtogroup bf-fault-api
 * @{
 * This is a description of some APIs.
 */

/** \brief bf_fault_e
 *         Enumeration of BF fault codes. The are passed to the fault
 *         handler (default or user-defined) on occurrence of a fault.
 *
 *  BF_FAULT_UNCORRECTABLE_ECC: A multi-bit ECC (or parity) error was
 *                               detected. The data could not be fixed
 *                               and should not be trusted. Depending on
 *                               what memory location is reporting this
 *                               fault switch functionality may be compromised
 *                               and corrective action taken (such as rebooting
 *                               or replacing the the failing part).
 *
 *  BF_FAULT_CORRECTABLE_ECC  : A single-bit ECC error was detected and
 *                               the value read was corrected. The failing
 *                               memory location may need to be manually
 *                               corrected to prevent subsequent failures.
 *
 *  BF_FAULT_DMA_ERROR        : A data integrity error was detected during
 *                               a DMA operation. In most cases the DMA
 *operation
 *                               should be considered a failure and retried
 *(there
 *                               is one case where a correctable ECC error was
 *                               detected. This can be considered a successful
 *                               operation).
 *
 *  BF_FAULT_SW_ERROR         : A software error was encountered making
 *continued
 *                               execution untrustworthy. The software component
 *                               must be restarted.
 */
typedef enum {
  BF_FAULT_UNCORRECTABLE_ECC = 1,
  BF_FAULT_CORRECTABLE_ECC,
  BF_FAULT_DMA_ERROR,
  BF_FAULT_SW_ERROR,
} bf_fault_e;

/** \brief bf_sw_fault_e
 *         Enumeration of software faults.
 *         Currently there are no software faults defined.
 */
typedef enum {
  BF_SW_FAULT_UNSPEC = 1,
} bf_sw_fault_e;

/** \brief bf_fault_handler_t
 *         User-defined fault handler function.
 *         Notifies the user of failures detected in either hardware
 *         or software. "fault" identifies the specific failure
 *         detected. The user can take any actions deemed necessary
 *         to rectify the condition.
 *         Failures may be loosely classified as either "fatal" or
 *         "non-fatal". For example, a correctable ECC error would likely
 *         be considered a "non-fatal" error as the it has already been
 *         corrected. However, the user may wish to track such failures
 *         over time to identify trends. On the other hand an uncorrectable
 *         ECC error may be considered "fatal", depending on exactly what
 *         memory/register is reporting the error. An error in a statistics
 *         counter may not necessarily reduce functionality to the point
 *         repair or replacement is necessary. But, an error in an internal
 *         queue or match/action RAM may require immediate shutdown to prevent
 *         undefined behavior.
 *         The user controls the determination of "fatal" or "non-fatal" by
 *         whether or not the function returns. If control returns to
 *         the statement following the call then the error is considered
 *         non-fatal and operation resumes.
 *
 * \param fault: int      : system-assigned identifier (0..BF_MAX_DEV_COUNT-1)
 * \param p1   : uint64_t : fault-specific value
 * \param p2   : uint64_t : fault-specific value
 * \param p3   : uint64_t : fault-specific value
 *
 * \return: if this call returns the error is considered non-fatal.
 *
 */
typedef int (*bf_fault_handler_t)(bf_fault_e fault,
                                  uint64_t p1,
                                  uint64_t p2,
                                  uint64_t p3);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* C++ */

#endif  // BF_INT_IF_H_INCLUDED
