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

#ifndef _MODEL_CORE_CHIP_INTERFACE__
#define _MODEL_CORE_CHIP_INTERFACE__

#include <cstdint>
#include <cinttypes>
#include <string>
#include <common/rmt.h>
#include <model_core/rmt-phv-modification.h>


namespace model_core {

class RegisterBlockBase;
class RegisterBlockIndirectBase;

class ChipInterface {

public:
  virtual ~ChipInterface();

  virtual uint32_t InWord(uint32_t address) = 0;
  virtual void OutWord(uint32_t address, uint32_t data) = 0;
  virtual int  DecodeAddress(uint32_t address, int *index) = 0;

  virtual void IndirectRead(uint64_t address, uint64_t *data0, uint64_t *data1, uint64_t T) = 0;
  virtual void IndirectWrite(uint64_t address, uint64_t data0, uint64_t data1, uint64_t T) = 0;

  virtual bool IsChipInitialized() = 0;
  virtual void InitChip() = 0;
  virtual void ResetChip() = 0;
  virtual bool ConfigChip(uint32_t pipes_enabled, uint8_t num_stages, int sku, uint32_t flags) = 0;
  virtual bool QueryChip(uint32_t *pipes_enabled, uint8_t *num_stages, int *sku, uint32_t *flags) = 0;
  virtual void SetPackage(ChipInterface *chip0, ChipInterface *chip1, ChipInterface *chip2, ChipInterface *chip3) = 0;
  virtual void GetPackage(ChipInterface **chip0, ChipInterface **chip1, ChipInterface **chip2, ChipInterface **chip3) = 0;
  virtual uint32_t EpochChip() = 0;
  virtual uint8_t  GetType() = 0;
  virtual uint32_t GetPipesEnabled() = 0;
  virtual uint8_t  GetNumStages() = 0;
  virtual uint32_t GetFlags() = 0;

  virtual void Subscribe(RegisterBlockBase *block, uint32_t chip,
                         uint32_t offset, uint32_t size,
                         bool isMutable, std::string name) = 0;
  virtual void IndirectSubscribe(RegisterBlockIndirectBase *block, uint32_t chip,
                                 uint64_t offset, uint32_t size,
                                 bool isMutable, std::string name) = 0;

  virtual bool CheckNoOverlaps() = 0;
  virtual void set_0bad_mode( int en ) = 0;


  virtual void PortDown(uint16_t asic_port) = 0;
  virtual void PortUp(uint16_t asic_port) = 0;
  virtual void PacketReceivePostBuf(uint16_t asic_port, uint8_t *buf, int len) = 0;
  virtual void StartPacketProcessing(void) = 0;
  virtual void StopPacketProcessing(void) = 0;
  virtual void PacketTransmitRegisterFunc(RmtPacketCoordinatorTxFn tx_fn) = 0;
  virtual void LoggerRegisterFunc(rmt_logging_f log_fn) = 0;
  virtual void SetLogPktSignature(int offset, int len, bool use_pkt_sig) = 0;
  virtual void SetLogPipeStage(uint64_t pipes, uint64_t stages) = 0;
  virtual void UpdateLogTypeLevels(uint64_t pipes, uint64_t stages,
                                   int log_type, uint64_t remove_levels, uint64_t add_levels) = 0;
  virtual void UpdateLogFlags(uint64_t pipes, uint64_t stages,
                              uint64_t types, uint64_t rows_tabs, uint64_t cols,
                              uint64_t or_log_flags, uint64_t and_log_flags) = 0;
  virtual void SetP4NameLookup(int pipe, const char *p4_name_lookup) = 0;

  virtual void SetTcamWriteReg(int pipe, int stage, int mem, uint32_t address,
                               uint64_t data_0, uint64_t data_1, bool write_tcam) = 0;
  virtual void TcamCopyWord(int pipe, int stage,
                            int src_table_id, int dst_table_id, int num_tables,
                            int num_words, int adr_incr_dir,
                            uint32_t src_address, uint32_t dst_address) = 0;

  virtual void TimeReset() = 0;
  virtual void TimeIncrement(uint64_t pico_increment) = 0;
  virtual void TimeSet(uint64_t pico_time) = 0;
  virtual void TimeGet(uint64_t& time) = 0;

  virtual void GetMauInfo(int pipe, int stage,
                          uint32_t *array, int array_size,
                          const char **name_array, bool reset) = 0;

  virtual const char *NameChip() = 0;
  virtual const char *RegisterVersionChip() = 0;
  virtual void SetPrintWrites(bool v) = 0;

  virtual int SetPhvModification(int pipe, int stage,
      RmtPhvModification::ModifyEnum which,
      RmtPhvModification::ActionEnum action,
                                  int index, uint32_t value) = 0;
};

}

#endif
