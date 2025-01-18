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

#ifndef _RMT_ASSERT_H_
#define _RMT_ASSERT_H_


#ifdef __cplusplus
#include <cstdio>
#include <cassert>
#include <exception>
extern "C" {
#endif /* __cplusplus */

#include "rmt-features.h"


extern int GLOBAL_RMT_ERROR;
extern int GLOBAL_ABORT_ON_ERROR;
extern int GLOBAL_THROW_ON_ERROR;
extern int GLOBAL_THROW_ON_ASSERT;
extern int GLOBAL_SHUTDOWN;
extern bool GLOBAL_FALSE;
extern bool GLOBAL_TRUE;
extern int GLOBAL_ZERO;
extern void* GLOBAL_NULLPTR;



#define SET_GLOBAL_ERROR(err) do { \
  GLOBAL_RMT_ERROR = (err);        \
} while (0);
#define GET_GLOBAL_ERROR()  (GLOBAL_RMT_ERROR)



#define ABORT_IF_GLOBAL_ERROR do {                                      \
  if ((GLOBAL_ABORT_ON_ERROR == 1) && (GLOBAL_RMT_ERROR < 0)) abort();  \
} while (0);
#define ABORT_IF_ERROR(err)   do {                                      \
  if ((GLOBAL_ABORT_ON_ERROR == 1) && ((err) != 0)) abort();            \
} while (0);


extern void rmt_throw_error_fn(int err, const char *file, int line, const char *func);

#define THROW_ERROR(err) do {                                           \
  if ((err) != 0) {                                                     \
    rmt_throw_error_fn((err),__FILE__,__LINE__,__FUNCTION__);           \
  }                                                                     \
} while (0);


extern void rmt_throw_assert_fn(bool expr, const char *file, int line, const char *func,
                                const char *exprstr);

/* NB Have to leave unconditional assert() in macro below else */
/* we get 'control reaches end of non-void function' warnings. */
#define RMT_ASSERT(expr) do {                                           \
  if ((GLOBAL_THROW_ON_ASSERT == 1) && (!(expr) || GLOBAL_FALSE)) {     \
    rmt_throw_assert_fn((expr),__FILE__,__LINE__,__FUNCTION__,#expr);   \
  }                                                                     \
  assert((expr));                                                       \
} while(0);

extern void rmt_throw_nullptr_error_fn(const char *file,
                                       int line,
                                       const char *func,
                                       const char *param);

#define RMT_ASSERT_NOT_NULL(VAL) do {                                 \
  if ((nullptr == VAL) || GLOBAL_FALSE) {                                               \
    rmt_throw_nullptr_error_fn(__FILE__,__LINE__,__FUNCTION__, #VAL); \
  }                                                                   \
} while (0);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // _RMT_ASSERT_H_
