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

#include <boost/filesystem/operations.hpp>

std::string get_executable_dir_fallback()
{
  return "tests/utests";
}


//#if (BOOST_OS_LINUX) // use this if we ever upgrade to boost 1.55 or later
#ifdef __linux__
#  include <unistd.h>

std::string get_executable_dir()
{
    char buf[1024] = {0};
    ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
    if (size == 0 || size == sizeof(buf))
    {
        return get_executable_dir_fallback();
    }
    std::string path(buf, size);
    boost::system::error_code ec;
    boost::filesystem::path p(
        boost::filesystem::canonical(
            path, boost::filesystem::current_path(), ec));
    p.remove_filename();
    return p.make_preferred().string();
}

#else
  // add OS specific code here...
  #warning "Falling back to assuming test is run from top of tree"
std::string get_executable_dir()
{
  return get_executable_dir_fallback();
}

#endif

std::string get_resource_file_path(std::string filename) {
  boost::filesystem::path pth(get_executable_dir());
  pth /= "resources";
  pth /= filename;
  return pth.string();
}
