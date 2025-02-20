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

#include <assert.h>
#include "log.h"
#include "script.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "table_config.h"
#include "util.h"

using namespace MODEL_CHIP_NAMESPACE;

std::map<std::string, Script *> *Script::all;
Script::Context *Script::current;

struct Script::Context {
    Context     *prev;
    const char  *fname;
    int         lineno;
    int         errcnt;
};

Script::Script(const char *name) {
    static std::map<std::string, Script *> all_;
    all = &all_;
    assert(all->count(name) == 0);
    all_[name] = this;
}

void error(const char *fmt, ...) {
    va_list     args;
    if (Script::current)
        fprintf(stderr, "%s:%d: ", Script::current->fname, Script::current->lineno);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
    if (Script::current)
        Script::current->errcnt++;
}

void warning(const char *fmt, ...) {
    va_list     args;
    if (Script::current)
        fprintf(stderr, "%s:%d: ", Script::current->fname, Script::current->lineno);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
}

std::string pipe_set_str(unsigned pipes) {
    std::string ret{};
    unsigned counter = 0;

    while (pipes != 0) {
        if (pipes & 0x1) {
            if (ret.size() != 0)
                ret += ", ";
            ret += std::to_string(counter);
        }
        counter++;
        pipes >>=1;
    }
    if (ret.size() == 0)
        return std::string{"no pipes"};
    else
        return std::string{"pipes: "} + ret;
}

unsigned Script::pipe_prefix(char **str) {
    while (isspace(**str)) ++*str;
    unsigned rv = ALL_PIPES;
    // TODO:
    // This can work for up to 10 pipes (0-9).
    // When adding more pipes, we need to change this.
    if (**str >= '0' && **str <= ('0' + (MAX_PIPE_COUNT - 1)) && (*str)[1] == ':') {
        rv = 1 << (**str - '0');
        *str += 2; }
    return rv;
}

char *Script::token(char **str, const char *delim, char *next) {
    char *p = *str;
    char *end;
    p += strspn(p, delim);
    if (!*p) return 0;
    if (*p == '"') {
        ++p;
        end = p + strcspn(p, "\"");
    } else
        end = p + strcspn(p, delim);
    if (next) *next = *end;
    if (*end) *end++ = 0;
    *str = end;
    return p;
}

char Script::skip(char **str, const char *delim) {
    *str += strspn(*str, delim);
    return **str;
}

void Script::skip_to(char **str, const char *delim) {
    *str += strcspn(*str, delim);
}

static const char *find_closer(const char *p, const char *parens, const char *quotes, bool err) {
    int depth = 0, quote = 0;
    while (*p) {
        if (*p == quote) {
            quote = 0;
        } else if (!quote) {
            if (*p == parens[0]) {
                depth++;
            } else if (*p == parens[1]) {
                if (--depth < 0) break;
            } else if (strchr(quotes, *p)) {
                quote = *p; } }
        p++; }
    if (!*p && err)
        error("Can't find matching `%c` for `$%c`", parens[1], parens[0]);
    return p;
}

static bool unmatched_expand(const std::string &line) {
    std::string::size_type      p = 0;
    while ((p = line.find('$', p)) != std::string::npos) {
        ++p;
        if (line[p] == '{') {
            p = find_closer(&line[p+1], "{}", "'\"", false) - &line[0];
            if (!line[p]) return true;
        } else if (line[p] == '(') {
            p = find_closer(&line[p+1], "()", "'\"", false) - &line[0];
            if (!line[p]) return true; } }
    return false;
}

const char *IDchars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789";

static void expand(std::string &line) {
    std::string::size_type      p = 0;
    while ((p = line.find('$', p)) != std::string::npos) {
        auto e = p+1;
        if (line[e] == '{') {
            e = line.find_first_not_of(IDchars, e+1);
            if (e == std::string::npos) e = line.size();
            if (e > p+2 && strchr(":}", line[e])) {
                auto name = line.substr(p+2, e-p-2);
                const char *value = getenv(name.c_str());
                if (line[e] == ':') {
                    auto w = ++e;
                    auto op = line[w++];
                    if (op != '-' && op != '=' && op != '+' && op != '?')
                        w--;
                    e = find_closer(&line[e], "{}", "'\"", true) - &line[0];
                    auto word = line.substr(w, e-w);
                    expand(word);
                    if (line[e]) e++;
                    switch (op) {
                    default: case '-':
                        if (!value || !*value)
                            value = word.c_str();
                        break;
                    case '=':
                        if (!value || !*value) {
                            setenv(name.c_str(), word.c_str(), 1);
                            value = word.c_str(); }
                        break;
                    case '+':
                        if (value && *value)
                            value = word.c_str();
                        break;
                    case '?':
                        if (!value || !*value) {
                            if (word.empty())
                                error("$%s is unset", name.c_str());
                            else
                                error("%s", word.c_str());
                            /* FIXME -- should immediately exit? */ }
                        break; }
                } else if (line[e] == '}') {
                    e++; }
                if (value) {
                    line.replace(p, e-p, value, strlen(value));
                    e = p + strlen(value);
                } else {
                    line.erase(p, e-p);
                    e = p; }
            } else {
                e = find_closer(&line[e], "{}", "'\"", true) - &line[0];
                if (line[e]) e++;
                error("invalid environment variable %.*s", (int)(e-p), &line[p]); }
        } else if (line[e] == '(') {
            e = find_closer(&line[e+1], "()", "'\"", true) - &line[0];
            auto cmd = line.substr(p+2, e-p-2);
            if (line[e]) e++;
            line.erase(p, e-p);
            e = p;  // rescan the shell output for further expansions
            LOG3("running sh script:\n" << cmd);
            if (auto *fp = popen(cmd.c_str(), "r")) {
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), fp)) {
                    auto len = strlen(buffer);
                    if (len == strspn(buffer, " \t\r\n"))
                        continue;  // discard blank lines
                    line.insert(p, buffer, len);
                    p += len; }
                pclose(fp);
                if (p > 0 && line[p-1] == '\n')
                    line.erase(--p, 1);  // erase trailing newline
            } else {
                error("failure running external shell command"); }
        } else {
            e = line.find_first_not_of(IDchars, e);
            if (e == std::string::npos) e = line.size();
            if (e > p+1) {
                auto name = line.substr(p+1, e-p-1);
                auto value = getenv(name.c_str());
                if (value) {
                    line.replace(p, e-p, value, strlen(value));
                    e = p + strlen(value);
                } else {
                    /* leave it alone as table/field names sometimes contain $ */ } } }
        p = e; }
}

int Script::run(const char *fname, std::istream &in) {
    Context local = { current, fname, 0, 0 };
    current = &local;
    std::string line;
    while (getline(in, line)) {
        local.lineno++;
        while (line.back() == '\\' || unmatched_expand(line)) {
            if (line.back() == '\\')
                line.pop_back();
            else
                line += '\n';
            std::string cont;
            if (!getline(in, cont)) break;
            line += cont;
            local.lineno++; }
        expand(line);
        char *l = &line[0], next;
        while (char *cmd = token(&l, " \t\r\n;", &next)) {
            if (!cmd) break;
            if (char *t = strchr(cmd, '#')) {
                if (next != '\n')
                    l += strcspn(l, "\n");
                if (t == cmd) continue;
                next = *t;
                *t = 0; }
            char *end = l;
            if (next == ';' || next == '#' || next == '\n') {
                --l;
            } else {
                end += strcspn(end, ";#\n\"");
                while (*end == '\"') {
                    end += strcspn(end + 1, "\"\n") + 1;
                    if (*end == '\"')
                        end += strcspn(end + 1, ";#\n\"") + 1; }
                if (*end == '#') {
                    *end++ = 0;
                    end += strcspn(end, "\n"); }
                if (*end) *end++ = 0;
            }
            while (isspace(*l)) ++l;
            for (char *p = l+strlen(l)-1; p >= l && isspace(*p); --p) *p = 0;
            LOG1(cmd << ' ' << l);
            if (all->count(cmd))
                (*all)[cmd]->exec(l);
            else
                error("Unknown command %s", cmd);
            l = end; } }
    current = local.prev;
    return local.errcnt;
}

ScriptCommand(help, "") {
    for (auto &cmd : *all)
        printf("  %s %s\n", cmd.first.c_str(), cmd.second->usage());
}

ScriptCommand(setenv, "<name> <value>") {
    skip(&args);
    auto name = token(&args);
    skip(&args);
    auto end = args + strlen(args);
    while (end > args && isspace(end[-1])) *--end = 0;
    if (!name || end == args)
        error("invalid setenv");
    else
        setenv(name, args, 1);
}

ScriptCommand(source, "<file>") {
    skip(&args);
    if (auto file = std::ifstream(args)) {
        Script::run(args, file);
    } else {
        error("can't open %s", args);
    }
}
