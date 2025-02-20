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

#ifndef _json_ref_h_
#define _json_ref_h_

#include "json.h"

namespace json {

class obj_ref {
    obj         *o;
public:
    obj_ref() : o(nullptr) {}
    obj_ref(obj *o_) : o(o_) {}
    obj_ref(obj &o_) : o(&o_) {}
    obj_ref(const std::unique_ptr<obj> &o_) : o(o_.get()) {}

    explicit operator bool() const { return o != 0; }
    operator string() const { return *o->as_string(); }

    obj *operator->() const { return o; }
    obj_ref operator[](const std::unique_ptr<obj> &i) const {
        return o && o->is<map>() ? o->to<map>()[i] : obj_ref(); }
    obj_ref operator[](const char *str) const {
        return o && o->is<map>() ? o->to<map>()[str].get() : obj_ref(); }
    obj_ref operator[](long n) const {
        if (o && o->is<map>()) return o->to<map>()[n].get();
        if (o && o->is<vector>()) {
            auto &v = o->to<vector>();
            if (n >= 0 && size_t(n) < v.size())
                return v[n].get(); }
        return obj_ref(); }
    obj_ref operator[](int n) const { return operator[](long(n)); }
    bool operator==(obj_ref v) { return o ? v ? *o == *v.o : false : !v; }
    bool operator!=(obj_ref v) { return o ? v ? *o != *v.o : true : !!v; }
    template<class T> bool operator==(T v) { return o && *o == v; }
    template<class T> bool operator!=(T v) { return !o || *o != v; }
    template<class T> bool is() const { return o && o->is<T>(); }
    template<class T> T &to() const {
        if (!o) throw std::bad_cast();
        return o->to<T>(); }
    operator int() const { return to<number>().val; }
    const char *c_str() const { return is<string>() ? to<string>().c_str() : nullptr; }
};

}  // end namespace json

#endif /* _json_ref_h_ */
