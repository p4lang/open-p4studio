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

#ifndef _script_h_
#define _script_h_

#include <iostream>
#include <map>
#include <string>

class Script {
    static std::map<std::string, Script *>       *all;
    struct Context;
    static Context      *current;
    friend void error(const char *fmt, ...);
    friend void warning(const char *fmt, ...);
    virtual void exec(char *args) = 0;
    virtual const char *usage() const = 0;
protected:
    Script(const char *name);
public:
    static char *token(char **str, const char *delim=" \t\r\n", char *next=0);
    static unsigned pipe_prefix(char **str);
    static char skip(char **str, const char *delim=" \t\r\n");
    static void skip_to(char **str, const char *delim=" \t\r\n");
    static int run(const char *fname, std::istream &in);
    friend class ScriptCommand_help;
};

extern void error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
extern void warning(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

std::string pipe_set_str(unsigned pipes);

#define ScriptCommand(NAME, USAGE, ...)                                       \
class ScriptCommand_##NAME : public Script {                                  \
    ScriptCommand_##NAME() : Script(#NAME) {}                                 \
    void exec(char *args);                                                    \
    const char *usage() const { return USAGE; }                               \
    static ScriptCommand_##NAME ScriptCommand_##NAME##_singleton_force_link_; \
    __VA_ARGS__                                                               \
} ScriptCommand_##NAME::ScriptCommand_##NAME##_singleton_force_link_;         \
void ScriptCommand_##NAME::exec(char *args)

#endif /* _script_h_ */
