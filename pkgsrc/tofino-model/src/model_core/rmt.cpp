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

//
// Implementation of common RMT interfaces
//

#include <cstdio>
#include <cstdarg>
#include <exception>
#include <iostream>
#include <csignal>
#include <pthread.h>
#include <unistd.h>

#include <common/rmt-assert.h>
#include <common/rmt.h>
#include <model_core/model.h>
#include <model_core/rmt-version.h>
#include <model_core/timer-manager.h>
#include <model_core/tmodel-sw-version.h>
#include <model_core/rmt-phv-modification.h>

using namespace std;

std::unique_ptr<model_core::Model> GLOBAL_MODEL(nullptr);

extern "C" {

static void exception_handle_print(const char *func, const exception e) {
#ifdef BFN_INTERNAL
  const char *extra = (GLOBAL_SHUTDOWN == 0) ?" - exiting" :" during shutdown";
  std::cerr << "tofino-model: exception " << e.what() << " in function " << func << extra << std::endl;
#else
  if (GLOBAL_SHUTDOWN == 0) {
    std::cerr << "tofino-model: internal error - please submit a bug report" << std::endl;
  }
#endif
}
static void exception_handle_raisesig(const char *func, const exception e) {
  // this handler called for exceptions occurring prior to shutdown
  exception_handle_print(func, e);
  // kill SIGINT should interrupt main thread's pause() causing it to shutdown
  kill(getpid(), SIGINT);
  int dummy = 0;
  pthread_exit((void*)&dummy);
}
static void exception_handle_exit(const char *func, const exception e) {
  // this handler called for exceptions during shutdown
  exception_handle_print(func, e);
  exit(1);
}

int no_rmt_chips = 1;

#ifdef GENERATE_ACCESS_LOG
FILE *access_log_fp;
#endif

bool g_use_pcie_veth = false;
bool g_disable_meter_sweep = false;
/* Num of bytes of packet to log */
uint32_t g_pkt_log_len = 64;


/*
* Initialize the Tofino model
*/
void
rmt_init(int chip_cnt, const char* json_log_file_name)
{
  try {
    RMT_ASSERT((chip_cnt >= 0) && (chip_cnt <= MAX_RMT_CHIPS));
    no_rmt_chips = chip_cnt;
    model_core::Model *model = new model_core::Model( no_rmt_chips, json_log_file_name );

    GLOBAL_SHUTDOWN = 0;
    GLOBAL_MODEL.reset(model);

    // TODO: callers should explicitly call rmt_create_chip and
    // TODO: rmt_destroy_chip to pick exactly what chip they want
    // TODO: for now ALL chip_cnt chips are created and reset

    model->Reset(); // Resets ALL chips

#ifdef GENERATE_ACCESS_LOG
    access_log_fp = fopen("./access.log", "w+");
#endif
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}
/*
* Teardown the Tofino model
*/
void
rmt_uninit() {
  try {
    GLOBAL_SHUTDOWN = 1;
    // Note, we destroy all *Chips* here.
    // But we do NOT destroy the GLOBAL_MODEL object itself
    //  as there could be other threads making calls on Chips.
    // They get given back an error/false if the Chip is gone.
    GLOBAL_MODEL->DestroyAllChips();
  }
  catch (exception &e) {
    exception_handle_exit(__FUNCTION__, e);
  }
}


/*
* Query chip type
*/
bool
rmt_get_type(uint8_t asic_id, uint8_t *chip_type)
{
  try {
    *chip_type = GLOBAL_MODEL->GetType(asic_id);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
  return true;
}
/*
* Query n_pipes etc for chip
*/
bool
rmt_query(uint8_t asic_id, uint32_t *pipes_enabled, uint8_t *num_stages, int *sku, uint32_t *flags)
{
  bool ret = false;
  try {
    ret = GLOBAL_MODEL->QueryChip(asic_id, pipes_enabled, num_stages, sku, flags);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
  return ret;
}

/*
* Setup n_pipes etc for chip - takes effect next time chip is reset
*/
bool
rmt_config(uint8_t asic_id, uint32_t pipes_enabled, uint8_t num_stages, int sku, uint32_t flags)
{
  bool ret = false;
  try {
    ret = GLOBAL_MODEL->ConfigChip(asic_id, pipes_enabled, num_stages, sku, flags);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
  return ret;
}

/*
 * Create/destroy chip - after create, chip will be in reset state
 * Chip types are defined in include/model_core/model.h
 */
void
rmt_create_chip(uint8_t asic_id, uint8_t type)
{
  try {
    GLOBAL_MODEL->CreateChip(asic_id, type);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}
bool
rmt_create_package(uint8_t asic_id, uint8_t type, int num)
{
  bool ok = true;
  try {
    int c[4] = { -1, -1, -1, -1 };

    // Create all chips - break on fail
    for (int i = 0; i < std::min(num,4); i++) {
      if (GLOBAL_MODEL->CreateChip(asic_id + i, type)) {
        c[i] = asic_id + i;
      } else {
        ok = false;
        break;
      }
    }
    // If all creates ok then package
    if (ok) ok = GLOBAL_MODEL->SetPackage(c[0], c[1], c[2], c[3]);

    // If create/package failed destroy any chips created
    if (!ok) {
      for (int i = 0; i < std::min(num,4); i++) {
        if (c[i] >= 0) GLOBAL_MODEL->DestroyChip(c[i]);
      }
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
  return ok;
}
void
rmt_destroy_chip(uint8_t asic_id)
{
  try {
    GLOBAL_MODEL->DestroyChip(asic_id);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}


void rmt_pcie_veth_init(bool use_pcie_veth)
{
  g_use_pcie_veth = use_pcie_veth;
}

void
rmt_meter_sweep_init(bool disable_meter_sweep)
{
  g_disable_meter_sweep = disable_meter_sweep;
}

void
rmt_set_pkt_log_len(uint32_t num_bytes)
{
  g_pkt_log_len = num_bytes;
}






void
rmt_port_down(uint8_t asic_id, uint16_t asic_port)
{
  try {
    GLOBAL_MODEL->PortDown(asic_id, asic_port);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_port_up(uint8_t asic_id, uint16_t asic_port)
{
  try {
    GLOBAL_MODEL->PortUp(asic_id, asic_port);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_packet_receive(uint8_t asic_id, uint16_t asic_port, uint8_t *buf, int len)
{
  try {
    GLOBAL_MODEL->PacketReceivePostBuf(asic_id, asic_port, buf, len);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_start_packet_processing(void)
{
  try {
    for (int chip = 0; chip < no_rmt_chips; chip++) {
      GLOBAL_MODEL->StartPacketProcessing(chip);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_stop_packet_processing(void)
{
  try {
    for (int chip = 0; chip < no_rmt_chips; chip++) {
      GLOBAL_MODEL->StopPacketProcessing(chip);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_transmit_register(RmtPacketCoordinatorTxFn tx_fn)
{
  try {
    for (int chip = 0; chip < no_rmt_chips; chip++) {
      GLOBAL_MODEL->PacketTransmitRegisterFunc(chip, tx_fn);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}


/*
* rmt_install_dru_callbacks
*
* Install callbacks for DMA simulation events.
* Any parameter may be specified as NULL in which
* case the existing callback (if any) is unmodified
*/
void rmt_install_dru_callbacks( DruLearn      dru_learn,
                                DruDiagEvent  dru_diag_event,
                                DruIdleUpdate dru_idle_update,
                                DruLrtUpdate  dru_lrt_update,
                                DruRxPkt      dru_rx_pkt )
{
  try {
    if (dru_learn != NULL) {
      GLOBAL_MODEL->register_callback_dru_learn( dru_learn );
    }
    if (dru_diag_event != NULL) {
      GLOBAL_MODEL->register_callback_dru_diag_event( dru_diag_event );
    }
    if (dru_idle_update != NULL) {
      GLOBAL_MODEL->register_callback_dru_idle_update( dru_idle_update );
    }
    if (dru_lrt_update != NULL) {
      GLOBAL_MODEL->register_callback_dru_lrt_update( dru_lrt_update );
    }
    if (dru_rx_pkt != NULL) {
      GLOBAL_MODEL->register_callback_dru_rx_pkt( dru_rx_pkt );
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}





/***************************************************************************
* model_reg_decode
***************************************************************************/
unsigned int model_reg_decode(int asic, unsigned int addr, int *index)
{
  unsigned int v = 0u;
  try {
    v = static_cast<unsigned int>( GLOBAL_MODEL->DecodeAddress(asic, addr, index) );
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
  return v;
}


/***************************************************************************
* model_reg_read
***************************************************************************/
unsigned int model_reg_read(unsigned int asic, unsigned int addr)
{
  unsigned int v = 0u;
  try {
    v = static_cast<unsigned int>( GLOBAL_MODEL->InWord(asic, addr) );
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
  return v;
}


/***************************************************************************
* model_reg_write
***************************************************************************/
void model_reg_write(unsigned int asic, unsigned int addr, unsigned int value)
{
  try {
#ifdef GENERATE_ACCESS_LOG
    fprintf( access_log_fp, "D 0x%08x 0x%08x\n", addr, value);
#endif
    GLOBAL_MODEL->OutWord(asic, addr, value);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}


/***************************************************************************
* model_ind_read
***************************************************************************/
void model_ind_read(int asic, uint64_t address,
                    uint64_t *data0, uint64_t *data1)
{
  try {
    GLOBAL_MODEL->IndirectRead(asic, address, data0, data1);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}


/***************************************************************************
* model_ind_write
***************************************************************************/
void model_ind_write(int asic, uint64_t address,
                     uint64_t data0, uint64_t data1)
{
  try {
#ifdef GENERATE_ACCESS_LOG
    fprintf( access_log_fp, "I 0x%016lx 0x%016lx 0x%016lx\n", address, data0, data1 );
#endif
    GLOBAL_MODEL->IndirectWrite(asic, address, data0, data1);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}


/***************************************************************************
* model_set_tcam_writereg
***************************************************************************/
void model_set_tcam_writereg(int asic, int pipe, int stage, int mem, uint32_t address,
                             uint64_t data_0, uint64_t data_1, bool write_tcam)
{
  try {
#ifdef GENERATE_ACCESS_LOG
    fprintf(access_log_fp, "T 0x%03x 0x%04x 0x%04x %d 0x%08x 0x%016lx 0x%016lx\n",
            pipe, stage, mem, write_tcam ? 1:0,
            address, data_0, data_1);
#endif
    GLOBAL_MODEL->SetTcamWritereg(asic, pipe, stage, mem, address,
                                  data_0, data_1, write_tcam);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

/***************************************************************************
* model_tcam_copy_word
***************************************************************************/
void model_tcam_copy_word(int asic, int pipe, int stage,
                          int src_table_id, int dst_table_id, int num_tables,
                          int num_words, int adr_incr_dir,
                          uint32_t src_address, uint32_t dst_address)
{
  try {
#ifdef GENERATE_ACCESS_LOG
    fprintf(access_log_fp, "C 0x%03x 0x%04x 0x%08x 0x%08x 0x%04x 0x%08x 0x%08x 0x%08x 0x%08x\n",
            pipe, stage, src_table_id, dst_table_id, num_tables, num_words, adr_incr_dir,
            src_address, dst_address);
#endif
    GLOBAL_MODEL->TcamCopyWord(asic, pipe, stage, src_table_id, dst_table_id, num_tables,
                               num_words, adr_incr_dir, src_address, dst_address);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}


// timer entry for the model - this calls a timer-manager function which in turn will
// call all the callback functions for running timers.
void rmt_timer_increment(uint64_t pico_increment)
{
  try {
    uint64_t clks = model_timer::ModelTimerPicoSecToClocks(pico_increment);
    model_timer::ModelTimerIncrement(clks);
    for (int chip = 0; chip < no_rmt_chips; chip++) {
      GLOBAL_MODEL->TimeIncrement(chip, pico_increment); //Picoseconds
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void rmt_time_set(uint64_t pico_time)
{
  try {
    uint64_t clks = model_timer::ModelTimerPicoSecToClocks(pico_time);
    model_timer::ModelTimerSetTime(clks);
    for (int chip = 0; chip < no_rmt_chips; chip++) {
      GLOBAL_MODEL->TimeSet(chip, pico_time); //Picoseconds
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void rmt_time_get(uint64_t& time)
{
  try {
    model_timer::ModelTimerGetTime(time);
    time = model_timer::ModelTimerClocksToPicoSec(time);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}



void
rmt_update_log_flags(int chip, uint64_t pipes, uint64_t stages,
                     uint64_t types, uint64_t rows_tabs, uint64_t cols,
                     uint64_t or_log_flags, uint64_t and_log_flags)
{
  try {
    if ((chip >= 0) && (chip < no_rmt_chips)) {
      GLOBAL_MODEL->UpdateLogFlags(chip, pipes, stages, types, rows_tabs, cols,
                                   or_log_flags, and_log_flags);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_update_log_type_levels(int asic_id, uint64_t pipes, uint64_t stages,
                           int log_type, uint64_t remove_levels, uint64_t add_levels)
{
  try {
    if ((asic_id >= 0) && (asic_id < no_rmt_chips)) {
      GLOBAL_MODEL->UpdateLogTypeLevels(asic_id, pipes, stages, log_type, remove_levels, add_levels);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_set_log_pkt_signature(int asic_id, int offset, int len, bool use_pkt_sig)
{
  try {
    if ((asic_id >= 0) && (asic_id < no_rmt_chips)) {
      GLOBAL_MODEL->SetLogPktSignature(asic_id, offset, len, use_pkt_sig);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_set_log_pipe_stage(int asic_id, uint64_t pipes, uint64_t stages)
{
  try {
    if ((asic_id >= 0) && (asic_id < no_rmt_chips)) {
      GLOBAL_MODEL->SetLogPipeStage(asic_id, pipes, stages);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_set_save_trace(SaveTracePairs const& save_trace) {
  try {
    for (auto const& pair : save_trace) {
      GLOBAL_MODEL->SetTrace(pair.first, pair.second, true);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_set_continue_on_config_errors(bool continue_on_config_errors) {
   try {
    // We don't allow dumping to a file from the top application.
    GLOBAL_MODEL->ContinueOnConfigErrors(continue_on_config_errors);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }

}


/* These funcs previously in rmt-log.cpp */
static rmt_logging_f rmt_log_fn = nullptr;

int rmt_log_fn_default(int chip, int pipe, const char *buffer) {
  try {
    GLOBAL_MODEL->Log(chip, pipe, buffer);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
  return 0;
}

/**
 * Sets the logging function used by the model.
 */
void rmt_set_model_log_fn(rmt_logging_f log_fn) {
  if (log_fn == nullptr) log_fn = rmt_log_fn_default;
  try {
    for (int chip = 0; chip < no_rmt_chips; chip++) {
      GLOBAL_MODEL->LoggerRegisterFunc(chip, log_fn);
    }
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

/**
 * Specifies the logging dir used by the model.
 */
void
rmt_set_log_dir(const char *log_dir) {
  try {
    GLOBAL_MODEL->SetLogDir(log_dir);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

/**
 * Sets the logging function for logging directly to rmt interface.
 */
void rmt_set_log_fn(rmt_logging_f log_fn) {
  if (log_fn == nullptr) log_fn = rmt_log_fn_default;
  rmt_log_fn = log_fn;
}

/**
 * Logs directly to the installed logging function. Prints formatted string to
 * a buffer and forwards to logging function.
 */
void rmt_log_fn_va(const uint32_t chip_id, const char* fmt, ...) {
  if (nullptr == rmt_log_fn) return;

  try {
    char buffer[768];
    va_list args;

    va_start (args, fmt);
    vsnprintf(buffer, 768, fmt, args);

    rmt_log_fn(chip_id, -1, buffer);
    va_end(args);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}

void
rmt_0bad_mode_set( int en )
{
  try {
    GLOBAL_MODEL->set_0bad_mode( en );
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}




void rmt_set_p4_name_lookup(int chip, int pipe, const char *p4_name_lookup)
{
  try {
    GLOBAL_MODEL->SetP4NameLookup(chip, pipe, p4_name_lookup);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}


void rmt_get_mau_info(int chip, int pipe, int stage,
                      uint32_t *array, int array_size,
                      const char **name_array, bool reset) {
  try {
    GLOBAL_MODEL->GetMauInfo(chip, pipe, stage, array, array_size, name_array, reset);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
}




char *tmodel_get_sw_version(void)       { return const_cast<char*>(TMODEL_SW_VER); }
char *tmodel_get_internal_version(void) { return const_cast<char*>(TMODEL_SW_INTERNAL_VER); }
char *rmt_version(void)                 { return const_cast<char*>(RMT_VERSION); }
char *rmt_register_version(int chip)    {
  char *ver = nullptr;
  try {
    ver = const_cast<char*>(GLOBAL_MODEL->RegisterVersionChip(chip));
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
  }
  return ver;
}

int rmt_set_phv_modification(int chip, int pipe, int stage,
          model_core::RmtPhvModification::ModifyEnum which,
          model_core::RmtPhvModification::ActionEnum action,
                              int index, uint32_t value) {
  int res;
  try {
    res = GLOBAL_MODEL->SetPhvModification(chip, pipe, stage, which, action, index, value);
  }
  catch (exception &e) {
    exception_handle_raisesig(__FUNCTION__, e);
    res = -1;
  }
  return res;
}

} // extern C
