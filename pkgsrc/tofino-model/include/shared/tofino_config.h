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

/**************************************************************************//**
 *
 * @file
 * @brief tofino Configuration Header
 *
 * @addtogroup tofino-config
 * @{
 *
 *****************************************************************************/
#ifndef __TOFINO_CONFIG_H__
#define __TOFINO_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef TOFINO_INCLUDE_CUSTOM_CONFIG
#include <tofino_custom_config.h>
#endif

/* <auto.start.cdefs(TOFINO_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * TOFINO_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef TOFINO_CONFIG_INCLUDE_LOGGING
#define TOFINO_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * TOFINO_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef TOFINO_CONFIG_LOG_OPTIONS_DEFAULT
#define TOFINO_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * TOFINO_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef TOFINO_CONFIG_LOG_BITS_DEFAULT
#define TOFINO_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * TOFINO_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef TOFINO_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define TOFINO_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * TOFINO_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef TOFINO_CONFIG_PORTING_STDLIB
#define TOFINO_CONFIG_PORTING_STDLIB 1
#endif

/**
 * TOFINO_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef TOFINO_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define TOFINO_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS TOFINO_CONFIG_PORTING_STDLIB
#endif

/**
 * TOFINO_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef TOFINO_CONFIG_INCLUDE_UCLI
#define TOFINO_CONFIG_INCLUDE_UCLI 0
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct tofino_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} tofino_config_settings_t;

/** Configuration settings table. */
/** tofino_config_settings table. */
extern tofino_config_settings_t tofino_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* tofino_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int tofino_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(TOFINO_CONFIG_HEADER).header> */

#include "tofino_porting.h"

#endif /* __TOFINO_CONFIG_H__ */
/* @} */
