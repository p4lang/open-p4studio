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

/**
 * @file
 *
 * @brief Header file for high level interfaces to simulation core
 *
 */

#ifndef _RMT_RMT_H
#define _RMT_RMT_H

#include <cstdint>
#include <vector>
#include <utility>

#include <common/rmt-flags.h>
#include <common/rmt-features.h>
#include <model_core/register_categories.h>
#include <model_core/rmt-dru-callback.h>
#include <model_core/rmt-phv-modification.h>


/* PLACEHOLDER */
typedef int bfm_port_t;

#define MAX_RMT_CHIPS          8
#define BAD_DATA_WORD 0x0BAD0BAD

/**
 * @brief Initialize the forwarding simulation
 */
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Initialize the Tofino model
 */
void rmt_init(int chip_cnt, const char* json_log_file_name = NULL);

/**
 * @brief Teardown the Tofino model
 */
void rmt_uninit();

/**
 * @brief Query chip type
 */
bool rmt_get_type(uint8_t asic_id, uint8_t *chip_type);

/**
 * @brief Query config for chip
 */
bool rmt_query(uint8_t asic_id, uint32_t *pipes_enabled, uint8_t *num_stages, int *sku, uint32_t *flags);

/**
 * @brief Setup config for chip
 */
bool rmt_config(uint8_t asic_id, uint32_t pipes_enabled, uint8_t num_stages, int sku, uint32_t flags);

/**
 * @brief Create/destroy chip
 */
void rmt_create_chip(uint8_t asic_id, uint8_t type);
bool rmt_create_package(uint8_t asic_id, uint8_t type, int num);
void rmt_destroy_chip(uint8_t asic_id);

/**
 * @brief Send pcie packets to veth instead of DRU callback
 */
void rmt_pcie_veth_init(bool use_pcie_veth);


void
rmt_meter_sweep_init(bool disable_meter_sweep);

/**
 * @brief Inform model number of bytes to log for each packet
 */
void
rmt_set_pkt_log_len(uint32_t num_bytes);

/**
 * @brief Inform the model a port went down.
 */
void
rmt_port_down(uint8_t asic_id, uint16_t asic_port);

/**
 * @brief Inform the model a port came up.
 */
void
rmt_port_up(uint8_t asic_id, uint16_t asic_port);

/**
 * @brief Inject a packet into the model.
 *
 * @param asic_id Which asic the packet is sent into
 * @param asic_port Which port on the asic the packet is sent into
 * @param buf Byte array containing the packet contents
 * @param len Length of the packet in bytes
 */
void
rmt_packet_receive(uint8_t asic_id, uint16_t asic_port, uint8_t *buf, int len);

/**
 * @brief Start packet processing in the model.
 */
void
rmt_start_packet_processing(void);

/**
 * @brief Stop packet processing in the model.
 */
void
rmt_stop_packet_processing(void);


typedef void (*RmtPacketCoordinatorTxFn)(int asic_id, int port, uint8_t *buf, int len);
/**
 * @brief Register a callback function which the model will use to "transmit"
 * a packet after the model has processed it and decided it should be sent out
 * a model port.
 *
 * @param tx_fn A pointer to the callback function.
 */
void
rmt_transmit_register(RmtPacketCoordinatorTxFn tx_fn);


/**
 * @brief Install callbacks for DMA simulation events
 */
void
rmt_install_dru_callbacks( DruLearn      dru_learn,
                           DruDiagEvent  dru_diag_event,
                           DruIdleUpdate dru_idle_update,
                           DruLrtUpdate  dru_lrt_update,
                           DruRxPkt      dru_rx_pkt );



unsigned int
model_reg_decode(int asic, unsigned int addr, int *index);

unsigned int
model_reg_read(unsigned int asic, unsigned int addr);

void
model_reg_write(unsigned int asic, unsigned int addr, unsigned int value);


void
model_ind_read(int asic, uint64_t address, uint64_t* data0, uint64_t* data1);

void
model_ind_write(int asic, uint64_t address, uint64_t data0, uint64_t data1);

void
model_set_tcam_writereg(int asic, int pipe, int stage, int mem, uint32_t address,
                        uint64_t data_0, uint64_t data_1, bool write_tcam);

void
model_tcam_copy_word(int asic, int pipe, int stage,
                     int src_table_id, int dst_table_id, int num_tables,
                     int num_words, int adr_incr_dir,
                     uint32_t src_address, uint32_t dst_address);


void
rmt_timer_increment(uint64_t pico_increment);

void
rmt_time_set(uint64_t pico_time);

void
rmt_time_get(uint64_t& time);




/**
 * @brief Update the log flags
 *
 * @param chip          The chip number
 * @param pipes         A mask of which pipes to update the flags in
 * @param stages        A mask of which stages to update the flags in
 * @param types         A mask of which types to update the flags for
 * @param rows_tabs     A mask of which rows/tables to update the flags in
 * @param cols          A mask of which columns to update the flags in
 * @param or_log_flags  This value is or with the flags
 * @param and_log_flags This value is anded with the flags
 */
void
rmt_update_log_flags(int chip, uint64_t pipes, uint64_t stages,
                     uint64_t types, uint64_t rows_tabs, uint64_t cols,
                     uint64_t or_log_flags, uint64_t and_log_flags);


/**
 * @brief Enumeration of logging levels understood by sim
 *
 * Simple linear levels
 */
typedef enum bfm_log_level_e {
    BFM_LOG_LEVEL_NONE,      /** No output */
    BFM_LOG_LEVEL_FATAL,     /** Only fatal errors */
    BFM_LOG_LEVEL_ERROR,     /** Errors */
    BFM_LOG_LEVEL_WARN,      /** Warnings */
    BFM_LOG_LEVEL_INFO,      /** Informational */
    BFM_LOG_LEVEL_VERBOSE,   /** Verbose */
    BFM_LOG_LEVEL_TRACE,     /** Most fn calls */
} bfm_log_level_t;


/*
 * Other APIs we expect to need for the simulation
 *
 * Set up the multicast tables
 * Queue configuration
 */
void
rmt_update_log_type_levels(int asic_id, uint64_t pipes, uint64_t stages,
                           int log_type, uint64_t remove_levels, uint64_t add_levels);

void
rmt_set_log_pkt_signature(int asic_id, int offset, int len, bool use_pkt_sig);

void
rmt_set_log_pipe_stage(int asic_id, uint64_t pipes, uint64_t stages);

using SaveTracePairs = std::vector<std::pair<unsigned, unsigned>>;
void
rmt_set_save_trace(SaveTracePairs const& save_trace);

void
rmt_set_continue_on_config_errors(bool continue_on_config_errors);

typedef int (*rmt_logging_f)(int chip, int pipe, const char *buffer);
typedef void (*rmt_logging_init_f)(int id);


/*
 * rmt_log_fn_default will be used instead if a nullptr
 * passed to rmt_set_model_log_fn or rmt_set_log_fn
 */
int
rmt_log_fn_default(int chip, int pipe, const char *buffer);
void
rmt_set_model_log_fn(rmt_logging_f log_fn);
void
rmt_set_log_dir(const char *log_dir);
void
rmt_set_log_fn(rmt_logging_f log_fn);


void
rmt_0bad_mode_set( int en );


void
rmt_log_fn_va(const uint32_t chip_id, const char* fmt, ...);

void
rmt_set_p4_name_lookup(int chip, int pipe, const char *p4_name_lookup);

void
rmt_get_mau_info(int chip, int pipe, int stage,
                 uint32_t *array, int array_size,
                 const char **name_array, bool reset=false);

char*
tmodel_get_sw_version(void);

char*
tmodel_get_internal_version(void);

char*
rmt_version(void);

char*
rmt_register_version(int chip);

int
rmt_set_phv_modification(int chip, int pipe, int stage,
        model_core::RmtPhvModification::ModifyEnum which,
        model_core::RmtPhvModification::ActionEnum action,
                                int index, uint32_t value);


#ifdef __cplusplus
}
#endif

#endif
