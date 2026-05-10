/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file tna_defs.h
 *
 *  @brief Contains Common data types needed for TNA specificcally
 */
#ifndef _TNA_DEFS_H
#define _TNA_DEFS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 32-bit ID for pipe_id
 */
typedef uint32_t tna_pipe_id_t;

/** Identifies an invalid value for a pipeline on an ASIC. */
#define TNA_INVALID_PIPE 0xFFFFFFFF

/** Identifies a pipeline on an ASIC.  Note dev_pipe_id can be set to
 *  TNA_DEV_PIPE_ALL as a special value to indicate "all pipelines". */
#define TNA_DEV_PIPE_ALL 0xFFFF

typedef enum {
  TNA_DIRECTION_INGRESS = 0,
  TNA_DIRECTION_EGRESS = 1,
  TNA_DIRECTION_ALL = 0xff,
} tna_direction_e;

/**
 * @brief Key Field Match Type. A key can have multiple fields,
 * each with a different match type
 */
enum tdi_tna_match_type_e {
  TDI_TNA_MATCH_TYPE_ATCAM = TDI_MATCH_TYPE_ARCH,
};

enum tdi_tna_target_e {
  TDI_TNA_TARGET_PIPE_ID = TDI_TARGET_ARCH,
  TDI_TNA_TARGET_DIRECTION,
};

#ifdef __cplusplus
}
#endif

#endif
