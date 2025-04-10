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
 * @brief tofino Porting Macros.
 *
 * @addtogroup tofino-porting
 * @{
 *
 *****************************************************************************/
#ifndef _SHARED_TOFINO_PORTING_H__
#define _SHARED_TOFINO_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if TOFINO_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef TOFINO_MALLOC
    #if defined(GLOBAL_MALLOC)
        #define TOFINO_MALLOC GLOBAL_MALLOC
    #elif TOFINO_CONFIG_PORTING_STDLIB == 1
        #define TOFINO_MALLOC malloc
    #else
        #error The macro TOFINO_MALLOC is required but cannot be defined.
    #endif
#endif

#ifndef TOFINO_FREE
    #if defined(GLOBAL_FREE)
        #define TOFINO_FREE GLOBAL_FREE
    #elif TOFINO_CONFIG_PORTING_STDLIB == 1
        #define TOFINO_FREE free
    #else
        #error The macro TOFINO_FREE is required but cannot be defined.
    #endif
#endif

#ifndef TOFINO_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define TOFINO_MEMSET GLOBAL_MEMSET
    #elif TOFINO_CONFIG_PORTING_STDLIB == 1
        #define TOFINO_MEMSET memset
    #else
        #error The macro TOFINO_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef TOFINO_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define TOFINO_MEMCPY GLOBAL_MEMCPY
    #elif TOFINO_CONFIG_PORTING_STDLIB == 1
        #define TOFINO_MEMCPY memcpy
    #else
        #error The macro TOFINO_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef TOFINO_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define TOFINO_STRNCPY GLOBAL_STRNCPY
    #elif TOFINO_CONFIG_PORTING_STDLIB == 1
        #define TOFINO_STRNCPY strncpy
    #else
        #error The macro TOFINO_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef TOFINO_VSNPRINTF
    #if defined(GLOBAL_VSNPRINTF)
        #define TOFINO_VSNPRINTF GLOBAL_VSNPRINTF
    #elif TOFINO_CONFIG_PORTING_STDLIB == 1
        #define TOFINO_VSNPRINTF vsnprintf
    #else
        #error The macro TOFINO_VSNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef TOFINO_SNPRINTF
    #if defined(GLOBAL_SNPRINTF)
        #define TOFINO_SNPRINTF GLOBAL_SNPRINTF
    #elif TOFINO_CONFIG_PORTING_STDLIB == 1
        #define TOFINO_SNPRINTF snprintf
    #else
        #error The macro TOFINO_SNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef TOFINO_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define TOFINO_STRLEN GLOBAL_STRLEN
    #elif TOFINO_CONFIG_PORTING_STDLIB == 1
        #define TOFINO_STRLEN strlen
    #else
        #error The macro TOFINO_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* _SHARED_TOFINO_PORTING_H__ */
/* @} */
