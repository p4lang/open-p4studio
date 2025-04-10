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

#ifndef SKNOBS_INC
#define SKNOBS_INC

#include <regex.h>



#ifdef __cplusplus
extern "C" {
#endif

#define SKNOBS_MAX_LENGTH 8192

extern int sknobs_init(int argc, char **argv);
extern void sknobs_close(void);

extern int sknobs_add(char *pattern, char *value, char *comment);
extern int sknobs_load(int argc, char *argv[], char *comment);
extern int sknobs_load_string(char *name, char *buffer, char *comment);
extern int sknobs_load_file(char *filename);
extern int sknobs_load_file_if_exists(char *filename);

extern int sknobs_exists(char *name);

typedef void *sknobs_iterator_p;
extern sknobs_iterator_p sknobs_iterate(const char *name);
extern int sknobs_iterator_next(sknobs_iterator_p iterator);

extern char *sknobs_find_file(char *filename);
extern char *sknobs_get_filename(char *name, char *defaultValue);

extern void sknobs_set_string(char *name, char *value);
extern void sknobs_set_value(char *name, unsigned long long value);
extern void sknobs_set_seed(unsigned value);

extern unsigned long long sknobs_eval(char *expr);

extern void sknobs_dump(void);
extern void sknobs_save(char *filename);

extern int sknobs_get_listsize(char *name) ;
extern char *sknobs_get_string_te(const char *name, char *defaultValue, int type, const char* src_file, int src_line);
extern unsigned long long sknobs_get_value_te(const char *name, unsigned long long defaultValue, int type, const char* src_file, int src_line);
extern unsigned long long sknobs_get_dynamic_value_te(const char *name, unsigned long long defaultValue, int type, const char* src_file, int src_line);
extern char *sknobs_iterator_get_string_te(sknobs_iterator_p iterator, int type, const char* src_file, int src_line);

#ifdef __cplusplus
}
#endif

#define sknobs_get_value(name, defaultValue) sknobs_get_value_te(name, defaultValue, 0, __FILE__, __LINE__)
#define sknobs_get_dynamic_value(name, defaultValue) sknobs_get_dynamic_value_te(name, defaultValue, 0, __FILE__, __LINE__)
#define sknobs_get_string(name, defaultValue) sknobs_get_string_te(name, defaultValue, 0, __FILE__, __LINE__)
#define sknobs_iterator_get_string(iterator) sknobs_iterator_get_string_te(iterator, 0, __FILE__, __LINE__)

#endif
