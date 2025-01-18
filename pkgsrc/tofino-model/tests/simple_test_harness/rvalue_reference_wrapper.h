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

#ifndef _rvalue_reference_wrapper_h_
#define _rvalue_reference_wrapper_h_

template<class T>
class rvalue_reference_wrapper {
    T   *ref;
public:
    typedef T type;
    rvalue_reference_wrapper(T &&r) : ref(&r) {}
    template<class U>
    rvalue_reference_wrapper(U &&r) : ref(&r) {}
    T &&get() { return std::move(*ref); }
};

#endif /* _rvalue_reference_wrapper_h_ */
