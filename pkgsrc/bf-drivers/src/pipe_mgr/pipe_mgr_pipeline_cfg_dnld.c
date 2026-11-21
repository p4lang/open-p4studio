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
//  pipe_mgr_pipeline_cfg_dnld.c
//
//

#include <stdint.h>
#include <unistd.h>
#include <sched.h>
#include <sys/time.h>

#include "pipe_mgr_int.h"
#include "pipe_mgr_phy_mem_map.h"
#include "pipe_mgr_db.h"
#include "pipe_mgr_select_tbl.h"
#include "pipe_mgr_drv.h"

#define WR_BLK_LEN 20
#define WR_REG_LEN 12
#define SCANSET_MIN_LEN 36
// Should be set to the longest defined
#define MAX_RECORD_LEN SCANSET_MIN_LEN

int n_4 = 0;
int n_8 = 0;
int n_16 = 0;
int n_other = 0;

unsigned long fpos[PIPE_MGR_NUM_DEVICES];
int write_blks[PIPE_MGR_NUM_DEVICES];
int write_regs[PIPE_MGR_NUM_DEVICES];
int write_scanset[PIPE_MGR_NUM_DEVICES];

int n_things = 0;
int n_things_between_updates = 100;
int next_char = 0;
char clock_thingy[4] = {'|', '/', '-', '\\'};
void pipeline_cfg_progress_bar(void) {
  if ((n_things++ % n_things_between_updates) == 0 && isatty(STDOUT_FILENO)) {
    printf("\r%c", clock_thingy[++next_char % 4]);
    fflush(stdout);
  }
}

/* Parse a header block encoded in BSON format, see
 * http://bsonspec.org/spec.html for details of the format.
 * Note we only parse a subset of the encoding types since we only use a few
 * types. */
static inline pipe_status_t blob_hdr_elm(bf_dev_id_t dev_id,
                                         profile_id_t prof_id,
                                         uint32_t log_pipe_mask,
                                         FILE *fd,
                                         int *num_stages) {
  const int string_type = 2;
  const int int32_type = 16;
  const uint32_t data_sz_max = 1000000; /* 1MB */
  /* The next four bytes is the size of the element. */
  uint32_t len, data_sz;
  int x = fread((uint8_t *)&len, 4, 1, fd);
  len = le32toh(len);
  /* The data size is four bytes less since the total size includes the four
   * byte size field. */
  data_sz = len - 4;

  /* Make sure the read was good. */
  if (feof(fd) || ferror(fd) || x != 1) return PIPE_UNEXPECTED;
  if (data_sz <= 0 || data_sz > data_sz_max) return PIPE_UNEXPECTED;

  /* Get a buffer to hold the data and read it out. */
  char *buf = PIPE_MGR_CALLOC(data_sz + 5, 1);
  if (!buf) return PIPE_NO_SYS_RESOURCES;
  x = fread(buf, data_sz, 1, fd);
  if (feof(fd) || ferror(fd) || x != 1) {
    PIPE_MGR_FREE(buf);
    return PIPE_UNEXPECTED;
  }

  uint32_t i = 0;
  while (i < data_sz - 1) {
    if (buf[i] == string_type) {
      /* String name followed by a four byte integer followed by a second string
       * value.  The second string has a length equal to the integer value.  */
      char *field_name = NULL, *field_value = NULL;
      uint32_t val_len = 0;
      ++i;
      field_name = buf + i;
      uint32_t field_name_len = strnlen(field_name, data_sz - i);
      if (field_name_len <= 0 || field_name_len == (data_sz - i))
        return PIPE_UNEXPECTED;
      i += field_name_len + 1;

      /* Get the length of the field's value string. */
      if (i + 4 < data_sz) {
        val_len = 0;
        PIPE_MGR_MEMCPY(&val_len, buf + i, 4);
        val_len = le32toh(val_len);
        i += 4;
      } else {
        return PIPE_UNEXPECTED;
      }
      if (val_len < 1) {
        LOG_ERROR("%s:%d Unexpected value size %d", __FILE__, __LINE__, len);
        return PIPE_UNEXPECTED;
      }

      /* Get the string representing the value of the field. */
      if (i + val_len < data_sz) {
        field_value = buf + i;
        /* It should be NULL terminated. */
        if (field_value[val_len - 1] != '\0') {
          return PIPE_UNEXPECTED;
        }
        i += val_len;
      } else {
        return PIPE_UNEXPECTED;
      }

      LOG_TRACE("Dev %d Profile %d to PipeMask 0x%x Property %s = %s",
                dev_id,
                prof_id,
                log_pipe_mask,
                field_name,
                field_value);

    } else if (buf[i] == int32_type) {
      /* String name followed by 4 byte integer value. */
      char *field_name = NULL;
      int32_t field_val = 0;
      ++i;
      field_name = buf + i;
      uint32_t field_name_len = strnlen(field_name, data_sz - i);
      if (field_name_len <= 0 || field_name_len == (data_sz - i))
        return PIPE_UNEXPECTED;
      i += field_name_len + 1;

      /* Get the integer which hold's the fields value. */
      if (i + 4 < data_sz) {
        field_val = 0;
        PIPE_MGR_MEMCPY(&field_val, buf + i, 4);
        field_val = le32toh(field_val);
        i += 4;
      } else {
        return PIPE_UNEXPECTED;
      }

      if (field_name_len == 6 && !strncmp("stages", field_name, 6)) {
        *num_stages = field_val;
      }
      LOG_TRACE("Dev %d Profile %d to PipeMask 0x%x Property %s = %d",
                dev_id,
                prof_id,
                log_pipe_mask,
                field_name,
                field_val);
    } else {
      return PIPE_UNEXPECTED;
    }
  }
  PIPE_MGR_FREE(buf);
  return PIPE_SUCCESS;
}

static int blob_check_len(bf_dev_id_t dev_id, FILE *fd, size_t record_len) {
  uint8_t fbuf[MAX_RECORD_LEN];
  if (record_len > MAX_RECORD_LEN) {
    LOG_ERROR("Unexpected record size %zu", record_len);
    PIPE_MGR_DBGCHK(0);
    return -2;
  }

  /* Back up to read the data out into variables. */
  if (fseek(fd, -4, SEEK_CUR)) return -2;

  /* Read the entire record out to be sure it is complete. */
  size_t x = fread(fbuf, 1, record_len, fd);
  if (feof(fd) || ferror(fd)) {
    LOG_ERROR("Unexpected EOF while reading pipeline config on dev %d", dev_id);
    return -2;
  }
  if (x != record_len) {
    LOG_ERROR("Short read while reading pipeline config on dev %d", dev_id);
    PIPE_MGR_DBGCHK(x == record_len);
    return -2;
  }

  /* Now that we know that the full record is in the file back up again to the
   * initial position, after the four byte header identifying the record type.*/
  ;
  if (fseek(fd, (0 - (record_len - 4)), SEEK_CUR)) return -2;

  return 0;
}

/* Write block of registers handler */
static int blob_wr_blk(pipe_sess_hdl_t sess_hdl,
                       rmt_dev_info_t *dev_info,
                       profile_id_t prof_id,
                       uint32_t log_pipe_mask,
                       pipe_prsr_instance_hdl_t prsr_hdl,
                       FILE *fd,
                       bool is_reg) {
  bf_dev_id_t dev_id = dev_info->dev_id;

  uint32_t ent;
  uint32_t width;
  bool check_stage = true;

  /* Get stage info. */
  uint8_t num_mau_stages = dev_info->num_active_mau;
  uint32_t prsr_stage = 0, dprsr_stage = 0;
  if (LLD_OK != lld_sku_get_prsr_stage(dev_id, &prsr_stage)) return -2;
  if (LLD_OK != lld_sku_get_dprsr_stage(dev_id, &dprsr_stage)) return -2;

  if (blob_check_len(dev_id, fd, WR_BLK_LEN)) return -2;

  uint64_t addr, full_addr;
  bool shadowed = false;
  // uint64_t tmp_addr;
  size_t x = fread((void *)&addr, 8, 1, fd);  // 64b address
  addr = le64toh(addr);
  if (x != 1) {
    PIPE_MGR_DBGCHK(x == 1);
    return -2;
  }
  x = fread((void *)&width, 4, 1, fd);  // 32b data width
  width = le32toh(width);
  if (x != 1) {
    PIPE_MGR_DBGCHK(x == 1);
    return -2;
  }
  x = fread((void *)&ent, 4, 1, fd);  // 32b number of entries
  ent = le32toh(ent);
  if (x != 1) {
    PIPE_MGR_DBGCHK(x == 1);
    return -2;
  }

  if (feof(fd) || ferror(fd)) return -2;
  if (addr == 0ull) {
    LOG_ERROR("Unexpected addr in pipeline config on dev %d", dev_id);
    return -2;
  }

  if (is_reg) {
    /* We expect the following registers to be published as block writes:
     *  - pipes[].mau[].dp.imem.imem_subword16
     *  - pipes[].mau[].dp.imem.imem_subword32
     *  - pipes[].mau[].dp.imem.imem_subword8
     *  - pipes[].mau[].rams.map_alu.row.adrmux.mapram_config
     */
    /* Convert the direct PCIe address to the internal address. */
    full_addr = dev_info->dev_cfg.pcie_pipe_addr_to_full_addr(addr);

  } else {
    /* Clean up the address provided.  It does not yet have the high bit set
     * indicating a pipe access.  Also, set the pipe-id to zero for good
     * measure.  The multicast block write DMA operation should take care of
     * sending the data to the correct pipes but we'll zero the pipe id in the
     * address anyways. */
    full_addr = dev_info->dev_cfg.set_pipe_id_in_addr(addr, 0);
  }

  if (width == 128)
    n_16++;
  else if (width == 64)
    n_8++;
  else if (width == 32)
    n_4++;
  else
    n_other++;

  if (width > 128) {
    LOG_ERROR("%s : Unsupported block write width of %d", __func__, width);
    PIPE_MGR_DBGCHK(0);
    return -2;
  }

  pipe_mgr_drv_ses_state_t *st =
      pipe_mgr_drv_get_ses_state(&sess_hdl, __func__, __LINE__);
  if (!st) {
    PIPE_MGR_DBGCHK(0);
    return -2;
  }

  size_t num_bytes = width / 8 * ent;
  uint32_t bwr_size = pipe_mgr_drv_buf_size(dev_id, PIPE_MGR_DRV_BUF_BWR);
  if (bwr_size == 0) {
    PIPE_MGR_DBGCHK(0);
    return -2;
  }

  uint32_t num_ent_per_write = bwr_size / (width / 8);  // Max possible
  size_t num_writes = num_bytes / bwr_size + ((num_bytes % bwr_size) ? 1 : 0);
  /* Get the type of address this is (register, physical memory,
   * instruction, virtual memory) since block writes of different memory
   * types require different address increments. */
  pipe_ring_addr_type_e addr_type =
      dev_info->dev_cfg.addr_type_from_addr(full_addr);
  uint8_t addr_step = (addr_type == addr_type_register) ? 4 : 1;
  // Following logic assumes that blk write is done to single memory type
  for (size_t wr_nb = 0; wr_nb < num_writes; wr_nb++) {
    /* Allocate a buffer and read the configuration data into it*/
    pipe_mgr_drv_buf_t *b = pipe_mgr_drv_buf_alloc(
        st->sid, dev_id, bwr_size, PIPE_MGR_DRV_BUF_BWR, true);
    if (!b) {
      PIPE_MGR_DBGCHK(0);
      return -2;
    }

    // If remaining entries are less than max, update related variables
    if (num_ent_per_write > ent) num_ent_per_write = ent;
    size_t wr_width = num_ent_per_write * width / 8;

    x = fread((void *)b->addr, 1, wr_width, fd);
    if (x != wr_width) {
      PIPE_MGR_DBGCHK(x == wr_width);
      return -2;
    }

    /* Pull the stage id from the full address. */
    uint32_t stage_id = dev_info->dev_cfg.stage_id_from_addr(full_addr);

    if (addr_type == addr_type_register) {
      /* lookup for imem register contents - ecc correction */
      pipe_mgr_lookup_cache_imem_register_val(
          dev_info,
          log_pipe_mask,
          stage_id,
          /* Pass in the pcie address. */
          dev_info->dev_cfg.dir_addr_set_pipe_type(addr),
          b->addr,
          wr_width,
          &shadowed);
      pipe_mgr_lookup_cache_gfm(dev_info,
                                log_pipe_mask,
                                stage_id,
                                /* Pass in the pcie address. */
                                dev_info->dev_cfg.dir_addr_set_pipe_type(addr),
                                b->addr,
                                wr_width);
    } else {
      /* lookup for parser memory contents
         -- ecc correction
         -- prsr programs */
      pipe_mgr_lookup_cache_parser_bin_cfg(
          dev_info, prsr_hdl, prof_id, full_addr, b->addr, wr_width, &shadowed);
    }

    check_stage = !(stage_id >= num_mau_stages);

    if (!check_stage && stage_id < prsr_stage && stage_id < dprsr_stage) {
      /* Debug build with less than the full complement of stages, drop this
       * write as it is to a stage which isn't present. */
      pipe_mgr_drv_buf_free(b);
    } else {
      // If data is prsr configuration, it will be configured to hardware in
      // reconfig(), only not shadowed writes are handled here.
      if (!shadowed) {
        pipe_status_t status = pipe_mgr_drv_blk_wr(&sess_hdl,
                                                   width / 8,
                                                   num_ent_per_write,
                                                   addr_step,
                                                   full_addr,
                                                   log_pipe_mask,
                                                   b);
        if (status != PIPE_SUCCESS) {
          LOG_ERROR("%s : Write block push in blob download error %s",
                    __func__,
                    pipe_str_err(status));
          PIPE_MGR_DBGCHK(PIPE_SUCCESS == status);
          return -2;
        }
      } else {
        pipe_mgr_drv_buf_free(b);
      }
      write_blks[dev_id]++;
    }
    full_addr += addr_step * num_ent_per_write;
    ent -= num_ent_per_write;
  }
  PIPE_MGR_DBGCHK(ent == 0);
  fpos[dev_id] += WR_BLK_LEN + num_bytes;

  return 0;
}

/* Write register handler */
static int blob_wr_reg(pipe_sess_hdl_t sess_hdl,
                       rmt_dev_info_t *dev_info,
                       profile_id_t prof_id,
                       uint32_t log_pipe_mask,
                       pipe_prsr_instance_hdl_t prsr_hdl,
                       FILE *fd) {
  bf_dev_id_t dev_id = dev_info->dev_id;
  uint32_t reg_addr = 0, reg_data = 0;
  pipe_instr_write_reg_t instr;
  uint32_t stage_id = 0;
  bool shadowed = false;
  bool check_stage = true;

  /* Get stage info. */
  uint8_t num_mau_stages = dev_info->num_active_mau;
  uint32_t prsr_stage = 0, dprsr_stage = 0;
  if (LLD_OK != lld_sku_get_prsr_stage(dev_id, &prsr_stage)) return -2;
  if (LLD_OK != lld_sku_get_dprsr_stage(dev_id, &dprsr_stage)) return -2;

  if (blob_check_len(dev_id, fd, WR_REG_LEN)) return -2;

  size_t x = fread((void *)&reg_addr, 4, 1, fd);  // 32b address
  reg_addr = le32toh(reg_addr);
  if (x != 1) {
    PIPE_MGR_DBGCHK(x == 1);
    return -2;
  }
  x = fread((void *)&reg_data, 4, 1, fd);  // 32b data
  reg_data = le32toh(reg_data);
  if (x != 1) {
    PIPE_MGR_DBGCHK(x == 1);
    return -2;
  }

  if (feof(fd) || ferror(fd)) return -1;

  /* Extract stage_id */
  stage_id = dev_info->dev_cfg.stage_id_from_addr(
      dev_info->dev_cfg.pcie_pipe_addr_to_full_addr(reg_addr));
  check_stage = !(stage_id >= num_mau_stages);

  if (!check_stage && stage_id < prsr_stage && stage_id < dprsr_stage) {
    // just skip nonexistent stage writes
    fpos[dev_id] += WR_REG_LEN;
    write_regs[dev_id]++;
    return 0;
  }
  switch (dev_info->dev_family) {
    case BF_DEV_FAMILY_TOFINO2:
    case BF_DEV_FAMILY_TOFINO3:
      pipe_mgr_lookup_cache_mirrtbl_register_content(
          dev_info, log_pipe_mask, reg_addr, (uint8_t *)&reg_data, 4);
      break;
    default:
      break;
  }
  // check reg and get prsr map
  if (stage_id == prsr_stage) {
    pipe_mgr_lookup_cache_parser_bin_reg_cfg(
        dev_info, prsr_hdl, prof_id, reg_addr, reg_data, &shadowed);
  }
  if (!shadowed) {
    construct_instr_reg_write(dev_id, &instr, reg_addr, reg_data);
    pipe_status_t status =
        pipe_mgr_drv_ilist_add(&sess_hdl,
                               dev_info,
                               &dev_info->profile_info[prof_id]->pipe_bmp,
                               stage_id,
                               (uint8_t *)&instr,
                               sizeof(pipe_instr_write_reg_t));
    if (status != PIPE_SUCCESS) {
      LOG_ERROR("%s : Instruction list add failed for blob download, error %s",
                __func__,
                pipe_str_err(status));
      PIPE_MGR_DBGCHK(PIPE_SUCCESS == status);
      return -2;
    }
    write_regs[dev_id]++;
  }
  fpos[dev_id] += WR_REG_LEN;
  return 0;
}


































































































































// returns 0 on success, -1 on eof, -2 on failure
static int write_next_bloblet(pipe_sess_hdl_t sess_hdl,
                              rmt_dev_info_t *dev_info,
                              profile_id_t prof_id,
                              uint32_t log_pipe_mask,
                              FILE *fd,
                              int *stages_configured) {
  uint32_t atom_val = 0;

  bf_dev_id_t dev_id = dev_info->dev_id;

  /* Get parser program handler */
  static pipe_prsr_instance_hdl_t prsr_hdl = DEFAULT_PRSR_INSTANCE_HDL;
  static profile_id_t prof_id_rst = 0;
  if (prof_id_rst != prof_id) {
    prof_id_rst = prof_id;
    prsr_hdl = DEFAULT_PRSR_INSTANCE_HDL;
  }

read_next_bloblet:

  pipeline_cfg_progress_bar();

  atom_val = 0;

  /* Read four elements from FD, each element is one byte.  These bytes will
   * determine what the next operation in the file is. */
  size_t x = fread((void *)&atom_val, 4, 1, fd);
  if (feof(fd) || ferror(fd) || x != 1) return -1;

  atom_val = le32toh(atom_val);
  char atom_type = atom_val >> 24;
  pipe_status_t sts = PIPE_UNEXPECTED;

  switch (atom_type) {
    case 'H':
      /* Found a header block. */
      sts = blob_hdr_elm(dev_id, prof_id, log_pipe_mask, fd, stages_configured);
      if (PIPE_SUCCESS != sts) return sts;
      goto read_next_bloblet;
    case 'R':
      sts =
          blob_wr_reg(sess_hdl, dev_info, prof_id, log_pipe_mask, prsr_hdl, fd);
      if (PIPE_SUCCESS != sts) return sts;
      goto read_next_bloblet;
    case 'D':
    case 'B':
      sts = blob_wr_blk(sess_hdl,
                        dev_info,
                        prof_id,
                        log_pipe_mask,
                        prsr_hdl,
                        fd,
                        atom_type == 'B');
      if (PIPE_SUCCESS != sts) return sts;
      return 0;
    case 'P':
      /* Update prsr_instance_hdl */
      x = fread((void *)&prsr_hdl, 4, 1, fd);
      PIPE_MGR_DBGCHK(x == 1);
      prsr_hdl = le32toh(prsr_hdl);
      goto read_next_bloblet;






    default: {
      LOG_ERROR(
          "Parse error: atom_val=%x atom_type=(%c)\n", atom_val, atom_type);
      LOG_ERROR("fpos=%lu <%lxh>\n", ftell(fd), ftell(fd));
#define BUF_SIZE 1025
      uint8_t fbuf[BUF_SIZE];
      char eb[BUF_SIZE * 4] = {0};
      size_t ebp = 0;
      if (fseek(fd, -4, SEEK_CUR)) return -2;
      x = fread(fbuf, 1, BUF_SIZE - 1, fd);
      for (size_t i = 0; i < x; i++) {
        if ((i % 16) == 0)
          ebp += snprintf(
              &eb[ebp], sizeof(eb) - ebp, "\n0x%lx : ", fpos[dev_id] + i);
        ebp += snprintf(&eb[ebp], sizeof(eb) - ebp, "%02x ", fbuf[i]);
      }
      LOG_ERROR("%s", eb);

      PIPE_MGR_DBGCHK(0);
      return -2;
    }
  }
  // Should not get here
  return 0;
}

static pipe_status_t pipe_mgr_load_pipeline_cfg(
    pipe_sess_hdl_t sess_hdl,
    rmt_dev_info_t *dev_info,
    pipe_mgr_blob_dnld_params_t *param,
    uint32_t log_pipe_mask,
    int *stages_configured) {
  char *cfg_file = param->cfg_file;
  int ret = 0;

  FILE *blob_fd = fopen(cfg_file, "r");
  if (!blob_fd) {
    LOG_ERROR("Could not open pipeline config file %s", cfg_file);
    PIPE_MGR_DBGCHK(blob_fd);
    return PIPE_INVALID_ARG;
  }
  if (fseek(blob_fd, 0, SEEK_SET)) {
    fclose(blob_fd);
    return -2;
  }

  do {
    ret = write_next_bloblet(sess_hdl,
                             dev_info,
                             param->prof_id,
                             log_pipe_mask,
                             blob_fd,
                             stages_configured);
  } while (ret == 0);

  fclose(blob_fd);
  if (-2 == ret) {
    return PIPE_INVALID_ARG;
  }
  return PIPE_SUCCESS;
}

static void pipe_mgr_init_all_data_structs(bf_dev_id_t dev_id) {
  PIPE_MGR_MEMSET(&fpos[dev_id], 0, sizeof(fpos[dev_id]));
  PIPE_MGR_MEMSET(&write_blks[dev_id], 0, sizeof(write_blks[dev_id]));
  PIPE_MGR_MEMSET(&write_regs[dev_id], 0, sizeof(write_regs[dev_id]));
  PIPE_MGR_MEMSET(&write_scanset[dev_id], 0, sizeof(write_scanset[dev_id]));
}

static pipe_status_t bypass_stage_cfg_write_one_stage(
    pipe_sess_hdl_t shdl,
    rmt_dev_info_t *dev_info,
    pipe_bitmap_t *pipe_bmp,
    dev_stage_t stage_id,
    int mask_idx,
    struct pipe_config_cache_bypass_stage_t *bypass_cfg) {
  for (int i = 0; i < bypass_cfg->reg_cnt; ++i) {
    struct pipe_config_cache_bypass_stage_reg_t *reg = bypass_cfg->regs + i;
    /* Mask value of zero means we don't write the register. */
    if (!reg->mask[mask_idx]) continue;

    /* Get the address of the register, the offset published is the offset
     * within an MAU so we need to add the "pipe" type and stage id to it. */
    for (int a = 0; a < reg->num_offsets; ++a) {
      uint32_t addr = reg->offset[a];
      addr = dev_info->dev_cfg.pcie_pipe_addr_set_stage(addr, stage_id);
      for (int j = 0; j < reg->num_vals; ++j) {
        pipe_instr_write_reg_t instr;
        construct_instr_reg_write(
            dev_info->dev_id, &instr, addr, reg->vals[j] & reg->mask[mask_idx]);
        pipe_status_t rc = pipe_mgr_drv_ilist_add(&shdl,
                                                  dev_info,
                                                  pipe_bmp,
                                                  stage_id,
                                                  (uint8_t *)&instr,
                                                  sizeof instr);
        if (PIPE_SUCCESS != rc) {
          return rc;
        }
        /* If there are multiple values they will be at consecutive offsets. */
        addr += 4;
      }
    }
  }
  return PIPE_SUCCESS;
}

/*
 * Program the bypass stage configuration.
 */
static pipe_status_t pipe_mgr_program_bypass_cfg(
    pipe_sess_hdl_t sess_hdl,
    rmt_dev_info_t *dev_info,
    pipe_mgr_blob_dnld_params_t *param,
    uint32_t log_pipe_mask,
    int stages_configured) {
  pipe_status_t rc = PIPE_SUCCESS;

  /* Tofino-1 doesn't use stage extensions. */
  if (dev_info->dev_family == BF_DEV_FAMILY_TOFINO) {
    return PIPE_SUCCESS;
  }

  /* Check if the number of stages programmed matches the number of active
   * stages in the chip. */
  bool all_stages_programmed = true;
  for (unsigned i = 0; i < dev_info->num_active_pipes; ++i) {
    /* Skip pipes which are not in this profile. */
    if ((1u << i) & ~log_pipe_mask) continue;
    if (stages_configured != (int)dev_info->num_active_mau) {
      all_stages_programmed = false;
      break;
    }
  }

  /* No stage bypass configuration needs to be loaded if all stages were
   * programmed by the tofino.bin download. */
  if (all_stages_programmed) return PIPE_SUCCESS;

  /* Lookup the stage bypass config from the config cache. */
  struct pipe_config_cache_bypass_stage_t *bypass_cfg = NULL;
  bf_map_sts_t sts =
      bf_map_get(&dev_info->profile_info[param->prof_id]->config_cache,
                 pipe_cck_mau_stage_ext,
                 (void **)&bypass_cfg);

  /* If this function was called bypass configuration is required. */
  if (BF_MAP_OK != sts || !bypass_cfg) {
    LOG_ERROR(
        "Dev %d profile-id %d pipe-mask 0x%x: Error loading bin file; bin file "
        "programmed %d stages but chip has fewer stages.  File %s",
        dev_info->dev_id,
        param->prof_id,
        log_pipe_mask,
        stages_configured,
        param->cfg_file);


    return PIPE_INVALID_ARG;
  }

  /* The number of stages specified in the context.json should match what was
   * programmed by the bin file. */
  if (dev_info->profile_info[param->prof_id]->num_stages != stages_configured) {
    LOG_ERROR(
        "Dev %d profile-id %d pipe-mask 0x%x: Error loading bin file; bin file "
        "programmed %d stages but context.json specified %d stages.  File %s",
        dev_info->dev_id,
        param->prof_id,
        log_pipe_mask,
        stages_configured,
        bypass_cfg->last_stage_programmed + 1,
        param->cfg_file);
  }

  /* The bin file's "stages programmed" should match up with the bypass stage
   * configuration from the context.json.  */
  if (stages_configured != bypass_cfg->last_stage_programmed + 1) {
    LOG_ERROR(
        "Dev %d profile-id %d pipe-mask 0x%x: Error loading bin file; bin file "
        "programmed %d stages but context.json specified %d stages.  File %s",
        dev_info->dev_id,
        param->prof_id,
        log_pipe_mask,
        stages_configured,
        bypass_cfg->last_stage_programmed + 1,
        param->cfg_file);
    return PIPE_INVALID_ARG;
  }

  /* First reprogram the "old last stage" (the last stage programmed by the bin
   * file). */
  pipe_bitmap_t *pipe_bmp = &dev_info->profile_info[param->prof_id]->pipe_bmp;
  int stage_id = bypass_cfg->last_stage_programmed;
  rc = bypass_stage_cfg_write_one_stage(
      sess_hdl, dev_info, pipe_bmp, stage_id, 0, bypass_cfg);
  if (PIPE_SUCCESS != rc) {
    LOG_ERROR(
        "Failed to post instruction for bypass stage config, dev %d prof-id %d "
        "pipe-mask 0x%x stage %d, status %s",
        dev_info->dev_id,
        param->prof_id,
        log_pipe_mask,
        stage_id,
        pipe_str_err(rc));
    return rc;
  }

  /* Now program any intermediate bypass stages, that is, stages after the "old
   * last" but before the new final stage. */
  int last_stage = dev_info->num_active_mau - 1;
  for (stage_id = bypass_cfg->last_stage_programmed + 1; stage_id < last_stage;
       ++stage_id) {
    rc = bypass_stage_cfg_write_one_stage(
        sess_hdl, dev_info, pipe_bmp, stage_id, 1, bypass_cfg);
    if (PIPE_SUCCESS != rc) {
      LOG_ERROR(
          "Failed to post instruction for bypass stage config, dev %d prof-id "
          "%d pipe-mask 0x%x stage %d, status %s",
          dev_info->dev_id,
          param->prof_id,
          log_pipe_mask,
          stage_id,
          pipe_str_err(rc));
      return rc;
    }
  }

  /* Finally, program the new last stage. */
  stage_id = last_stage;
  rc = bypass_stage_cfg_write_one_stage(
      sess_hdl, dev_info, pipe_bmp, stage_id, 2, bypass_cfg);
  if (PIPE_SUCCESS != rc) {
    LOG_ERROR(
        "Failed to post instruction for bypass stage config, dev %d prof-id %d "
        "pipe-mask 0x%x stage %d, status %s",
        dev_info->dev_id,
        param->prof_id,
        log_pipe_mask,
        stage_id,
        pipe_str_err(rc));
    return rc;
  }

  /* Also, program the stage ids for all unprogrammed stages. */
  uint32_t addr = 0;
  switch (dev_info->dev_family) {
    /* Tofino-1 doesn't support stage extensions, we shouldn't be here. */
    case BF_DEV_FAMILY_TOFINO2:
      addr = offsetof(tof2_reg, pipes[0].mau[0].rams.match.merge.pred_stage_id);
      break;
    case BF_DEV_FAMILY_TOFINO3:
      addr = offsetof(tof3_reg, pipes[0].mau[0].rams.match.merge.pred_stage_id);
      break;
    default:
      PIPE_MGR_DBGCHK(0);
      return PIPE_UNEXPECTED;
  }
  for (stage_id = bypass_cfg->last_stage_programmed + 1; stage_id <= last_stage;
       ++stage_id) {
    pipe_instr_write_reg_t instr;
    construct_instr_reg_write(dev_info->dev_id, &instr, addr, stage_id);
    rc = pipe_mgr_drv_ilist_add(&sess_hdl,
                                dev_info,
                                pipe_bmp,
                                stage_id,
                                (uint8_t *)&instr,
                                sizeof instr);
    if (PIPE_SUCCESS != rc) {
      LOG_ERROR(
          "Failed to post instruction for bypass stage id, dev %d prof-id %d "
          "pipe-mask 0x%x stage %d, status %s",
          dev_info->dev_id,
          param->prof_id,
          log_pipe_mask,
          stage_id,
          pipe_str_err(rc));
      return rc;
    }
  }

  return rc;
}

pipe_status_t pipe_mgr_download_blob_to_asic(
    pipe_sess_hdl_t sess_hdl,
    rmt_dev_info_t *dev_info,
    pipe_mgr_blob_dnld_params_t *param) {
  int stages_configured = -1;
  pipe_status_t rc = PIPE_SUCCESS;
  uint32_t log_pipe_mask = 0;

  /* Get a bitmap of pipes this profile is assigned to. */
  uint8_t pipe_id = 0;
  PIPE_BITMAP_ITER(&dev_info->profile_info[param->prof_id]->pipe_bmp, pipe_id) {
    log_pipe_mask |= 1 << pipe_id;
  }

  LOG_TRACE(
      "Starting pipeline cfg load device %u, logical pipe mask %X, file %s",
      param->dev_id,
      log_pipe_mask,
      param->cfg_file);

  pipe_mgr_init_all_data_structs(param->dev_id);
  rc = pipe_mgr_load_pipeline_cfg(
      sess_hdl, dev_info, param, log_pipe_mask, &stages_configured);
  if (PIPE_SUCCESS != rc) {
    LOG_ERROR(
        "Failed to load pipeline config on dev %d, logical pipe mask 0x%X, "
        "file %s",
        param->dev_id,
        log_pipe_mask,
        param->cfg_file);
    return rc;
  }

  LOG_TRACE(
      "Completed pipeline cfg load for dev %d, logical pipe mask %X with %d "
      "blocks, %d registers, %d scansets.",
      param->dev_id,
      log_pipe_mask,
      write_blks[param->dev_id],
      write_regs[param->dev_id],
      write_scanset[param->dev_id]);

  rc = pipe_mgr_program_bypass_cfg(
      sess_hdl, dev_info, param, log_pipe_mask, stages_configured);
  if (PIPE_SUCCESS != rc) {
    LOG_ERROR("Failed to program stage extensions for dev %d.", param->dev_id);
    return rc;
  }

  return PIPE_SUCCESS;
}
