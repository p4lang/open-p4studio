/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * Copyright (C) 2021 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file pna_defs.h
 *
 *  @brief Contains Common data types needed for PNA specificcally
 */
#ifndef _PNA_DEFS_H
#define _PNA_DEFS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 32-bit ID for pipe_id
 */
typedef uint32_t pna_pipe_id_t;

/** Identifies an invalid value for a pipeline on an ASIC. */
#define PNA_INVALID_PIPE 0xFFFFFFFF

/** Identifies a pipeline on an ASIC.  Note dev_pipe_id can be set to
 *  PNA_DEV_PIPE_ALL as a special value to indicate "all pipelines". */
#define PNA_DEV_PIPE_ALL 0xFFFF

typedef enum {
  PNA_DIRECTION_INGRESS = 0,
  PNA_DIRECTION_EGRESS = 1,
  PNA_DIRECTION_ALL = 0xff,
} pna_direction_e;

#ifdef __cplusplus
}
#endif

#endif
