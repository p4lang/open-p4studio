# Copyright (C) 2024 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.  You may obtain
# a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
#
# SPDX-License-Identifier: Apache-2.0

import os
import re
import subprocess
from collections import OrderedDict
from pathlib import Path
from typing import Tuple

from dependencies.source.source_dependency_config import SourceDependencyConfig
from utils.git import git_clone, git_update_submodules
from utils.pkg_config import check_pkg_config
from utils.processes import execute


def download_grpc(config: SourceDependencyConfig) -> None:
    grpc_attrs = config.dependency_manager().source_dependency_attributes("grpc")

    linux_bit_version = get_linux_bit_version()
    grpc_version = grpc_attrs[linux_bit_version]['version']

    grpc_repository_dir = config.download_dir(ensure_exists=False)
    if not grpc_repository_dir.exists():
        git_clone('https://github.com/grpc/grpc', 'v' + grpc_version, grpc_repository_dir, recursive=False)

        if linux_bit_version == '32b':
            re2_patch = get_re2_patch(grpc_repository_dir)
            execute('git -C {} apply {}'.format(grpc_repository_dir, re2_patch))

        execute('git -C {} submodule sync'.format(grpc_repository_dir))

        git_update_submodules(grpc_repository_dir)

        if linux_bit_version == '64b':
            # Patch to install re2.pc
            execute('git cherry-pick 9bacb66 -n', grpc_repository_dir / 'third_party/re2')

            '''
            Patch no longer needed
            absl_patch = get_absl_patch(grpc_repository_dir)
            execute('git -C {} apply {}'.format(grpc_repository_dir / 'third_party/abseil-cpp', absl_patch))

            # Get protobuf vulnerability patch for 3.15.8 version
            protobuf_patch = get_protobuf_patch_3_15(grpc_repository_dir)
            execute('git -C {} apply {}'.format(grpc_repository_dir / 'third_party/protobuf', protobuf_patch))
            '''
        elif linux_bit_version == '32b':
            # Get protobuf vulnerability patch for 3.12.2 version
            protobuf_patch = get_protobuf_patch_3_12(grpc_repository_dir)
            execute('git -C {} apply {}'.format(grpc_repository_dir / 'third_party/protobuf', protobuf_patch))
        execute("rm -rf {}".format((grpc_repository_dir / '.git').as_posix()))


def install_grpc_third_party(config: SourceDependencyConfig) -> None:
    override_env = {}

    # Centos stores libs in lib and lib64 directories
    install_lib = (config.install_dir / "lib").as_posix()
    if config.os_name == 'CentOS':
        install_lib += ":{install_dir}/lib64".format(install_dir=config.install_dir)

    ld_library_path = os.environ.get("LD_LIBRARY_PATH", "")
    ld_run_path = os.environ.get("LD_RUN_PATH", "")

    if install_lib not in ld_library_path:
        override_env['LD_LIBRARY_PATH'] = "{}:{}".format(install_lib, ld_library_path)
    if install_lib not in ld_run_path:
        override_env['LD_RUN_PATH'] = "{}:{}".format(install_lib, ld_run_path)
    grpc_dependencies = OrderedDict()
    grpc_dependencies['abseil-cpp'] = 'cmake -DCMAKE_CXX_FLAGS=\"-std=c++17\" \
                                      -DBUILD_SHARED_LIBS=ON \
                                      -DCMAKE_INSTALL_PREFIX={install_dir} \
                                      -DCMAKE_INSTALL_RPATH={rpath} .. \
                                      -DABSL_PROPAGATE_CXX_STD=ON '\
                                      .format(install_dir=config.install_dir, rpath=install_lib)
    grpc_dependencies['zlib'] = 'cmake -DCMAKE_INSTALL_PREFIX={install_dir} \
                                -DCMAKE_INSTALL_RPATH={rpath} ..' \
                                .format(install_dir=config.install_dir, rpath=install_lib)
    grpc_dependencies['cares'] = 'cmake -DCARES_SHARED=on \
                                 -DCMAKE_INSTALL_PREFIX={install_dir} \
                                 -DCMAKE_INSTALL_RPATH={rpath} \
                                 ../cares' \
                                 .format(install_dir=config.install_dir, rpath=install_lib)
    grpc_dependencies['re2'] = 'cmake -DBUILD_SHARED_LIBS=on \
                               -DCMAKE_BUILD_TYPE=RELEASE \
                               -DCMAKE_INSTALL_PREFIX={install_dir} \
                               -DCMAKE_INSTALL_RPATH={rpath} ..' \
                               .format(install_dir=config.install_dir, rpath=install_lib)

    build_dir = config.build_dir()
    for lib, install_cmd in grpc_dependencies.items():
        third_party_build_dir = build_dir / 'third_party/{}/build'.format(lib)
        execute("mkdir -p {}".format(third_party_build_dir))
        execute(install_cmd, third_party_build_dir, override_env)
        execute("make -j{} install".format(config.jobs), third_party_build_dir, override_env)
        execute("sudo ldconfig", third_party_build_dir)


def get_linux_bit_version() -> str:
    # Check os version: 64b or 32b
    pattern = r"(\d+)"
    os_bit = re.search(pattern, subprocess.getoutput('getconf LONG_BIT')).group()  # type: ignore
    return os_bit + 'b'


def install_grpc(config: SourceDependencyConfig) -> None:
    grpc_attrs = config.dependency_manager().source_dependency_attributes("grpc")

    linux_bit_version = get_linux_bit_version()
    grpc_version = grpc_attrs[linux_bit_version]['version']
    protobuf_version = grpc_attrs[linux_bit_version]['protobuf']['version']

    # GRPC++ version implies version of GRPC (like GRPC++:1.17.0 -> GRPC:7.0.0),
    # so we do not check it explicitly
    if not config.force and check_pkg_config(config.install_dir, "grpc++", grpc_version) and \
            check_pkg_config(config.install_dir, "protobuf", protobuf_version):
        return

    override_env = {}
    if config.os_name == 'CentOS':
        override_env[
            'PKG_CONFIG_PATH'] = config.install_dir.as_posix() + \
                                 '{install_dir}/lib/pkgconfig:{install_dir}/lib:{install_dir}/lib64:$PKG_CONFIG_PATH' \
                                 .format(install_dir=config.install_dir)
    else:
        override_env['PKG_CONFIG_PATH'] = config.install_dir.as_posix() + '/lib/pkgconfig:$PKG_CONFIG_PATH'

    ld_library_path = os.environ.get("LD_LIBRARY_PATH", "")
    ld_run_path = os.environ.get("LD_RUN_PATH", "")
    install_lib = (config.install_dir / "lib").as_posix()
    if install_lib not in ld_library_path:
        override_env['LD_LIBRARY_PATH'] = "{}:{}".format(install_lib, ld_library_path)
    if install_lib not in ld_run_path:
        override_env['LD_RUN_PATH'] = "{}:{}".format(install_lib, ld_run_path)

    download_grpc(config)
    build_dir = config.build_dir(copy_download_dir=True)

    # Install submodules from sources
    install_grpc_third_party(config)
    submodule_packages = '-DgRPC_ABSL_PROVIDER=package \
                          -DgRPC_ZLIB_PROVIDER=package \
                          -DgRPC_CARES_PROVIDER=package \
                          -DgRPC_RE2_PROVIDER=package \
                          -DgRPC_SSL_PROVIDER=package'

    # Set libs rpath.
    # Centos stores libs in lib and lib64 directories
    if config.os_name == 'CentOS':
        rpath = "{install_dir}/lib:{install_dir}/lib64".format(install_dir=config.install_dir)
    else:
        rpath = "{install_dir}/lib".format(install_dir=config.install_dir)

    # P4C requires static and dynamic libraries. Install protobuf from source.
    protobuf_build_dir = (build_dir / 'third_party/protobuf')
    if protobuf_version == '25.0.0':
        execute("""cmake . -DCMAKE_INSTALL_PREFIX={install_path} -Dprotobuf_WITH_ZLIB=ON """
                """ -DZLIB_INCLUDE_DIR={install_path}/include -DZLIB_LIB={install_path}/lib """
                """ -Dprotobuf_ABSL_PROVIDER=package -DCMAKE_PREFIX_PATH={install_path} """
                """ -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_INSTALL_RPATH={rpath} """
                """ -DBUILD_SHARED_LIBS=ON """
                """ -DGRPC_BUILD_ENABLE_CCACHE=ON """
                """ -Dprotobuf_ALLOW_CCACHE=ON """
                """ -DCMAKE_POSITION_INDEPENDENT_CODE=ON"""
                .format(install_path=config.install_dir, rpath=rpath), protobuf_build_dir)
        execute("cmake --build . --parallel {jobs}".format(jobs=config.jobs), protobuf_build_dir)
        execute("make install -j{jobs}".format(jobs=config.jobs), protobuf_build_dir)
        execute("sudo ldconfig", protobuf_build_dir)
    else:
        execute("./autogen.sh", protobuf_build_dir)
        execute("""./configure --prefix={install_path} --with-zlib """
                """--with-zlib-include={install_path}/include --with-zlib-lib={install_path}/lib"""
                .format(install_path=config.install_dir), protobuf_build_dir)
        execute("make install -j{}".format(config.jobs), protobuf_build_dir)
        execute("sudo ldconfig", protobuf_build_dir)


    grpc_build_dir = build_dir / 'build'
    execute("mkdir -p {}".format(grpc_build_dir))
    cmake_command = 'cmake -DgRPC_INSTALL=ON \
                -DBUILD_SHARED_LIBS=ON \
                -DgRPC_BUILD_TESTS=OFF \
                -DCMAKE_PREFIX_PATH={install_dir} \
                -DCMAKE_INSTALL_PREFIX={install_dir} \
                -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
                -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
                -DgRPC_PROTOBUF_PROVIDER=package \
                -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=off \
                -DgRPC_BUILD_CSHARP_EXT=off \
                -DgRPC_BUILD_GRPC_NODE_PLUGIN=off \
                -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=off \
                -DgRPC_BUILD_GRPC_PHP_PLUGIN=off \
                -DgRPC_BUILD_GRPC_RUBY_PLUGIN=off \
                -DCMAKE_BUILD_TYPE=Release \
                {sp} \
                -DCMAKE_INSTALL_RPATH={rpath} \
                {src_dir}'.format(install_dir=config.install_dir, sp=submodule_packages, rpath=rpath, src_dir=build_dir)
    execute(cmake_command, grpc_build_dir, override_env)

    execute('make -j{} install'.format(config.jobs), grpc_build_dir, override_env)
    execute('sudo ldconfig')


def get_absl_patch(output_dir: Path) -> Path:
    ########################################################################
    # Issue in absl: https://github.com/abseil/abseil-cpp/issues/952
    # Resolved in PR https://github.com/abseil/abseil-cpp/pull/967/files
    ########################################################################
    patch = """diff --git a/absl/debugging/failure_signal_handler.cc b/absl/debugging/failure_signal_handler.cc
index a9ed6ef9..186a0392 100644
--- a/absl/debugging/failure_signal_handler.cc
+++ b/absl/debugging/failure_signal_handler.cc
@@ -136,7 +136,8 @@ static bool SetupAlternateStackOnce() {
 #else
   const size_t page_mask = sysconf(_SC_PAGESIZE) - 1;
 #endif
-  size_t stack_size = (std::max(SIGSTKSZ, 65536) + page_mask) & ~page_mask;
+  size_t stack_size =
+         (std::max<size_t>(SIGSTKSZ, 65536) + page_mask) & ~page_mask;
 #if defined(ABSL_HAVE_ADDRESS_SANITIZER) || \\
     defined(ABSL_HAVE_MEMORY_SANITIZER) || defined(ABSL_HAVE_THREAD_SANITIZER)
   // Account for sanitizer instrumentation requiring additional stack space.
"""

    filename = output_dir / 'absl.patch'

    with open(filename.as_posix(), 'w') as f:
        f.write(patch)

    return filename


def get_re2_patch(output_dir: Path) -> Path:
    ########################################################################
    # Correct re2 link in git config file
    ########################################################################
    patch = """diff --git a/.gitmodules b/.gitmodules
index dbcc0ae579..781cfac313 100644
--- a/.gitmodules
+++ b/.gitmodules
@@ -22,7 +22,7 @@
 	url = https://github.com/google/boringssl.git
 [submodule "third_party/re2"]
 	path = third_party/re2
-	url = git://github.com/google/re2.git
+	url = https://github.com/google/re2.git
 [submodule "third_party/cares/cares"]
 	path = third_party/cares/cares
 	url = https://github.com/c-ares/c-ares.git
"""  # noqa

    filename = output_dir / 're2.patch'

    with open(filename.as_posix(), 'w') as f:
        f.write(patch)

    return filename


def get_protobuf_patch_3_15(output_dir: Path) -> Path:
    ########################################################################
    # Protobuf 3.15.8 vulnerability.
    # https://github.com/protocolbuffers/protobuf/commit/d1635e1496f51e0d5653d856211e8821bc47adc4#diff-595eef663de11f5567c3e853fe7af8a49fb6e7afe45d4859d55ddfce67a2bb62
    # The patch was adjusted to 3.15 version
    ########################################################################
    patch = """diff --git a/src/google/protobuf/extension_set_inl.h b/src/google/protobuf/extension_set_inl.h
index 074784b96..93db9d2a9 100644
--- a/src/google/protobuf/extension_set_inl.h
+++ b/src/google/protobuf/extension_set_inl.h
@@ -206,16 +206,21 @@ const char* ExtensionSet::ParseMessageSetItemTmpl(
     const char* ptr, const Msg* containing_type,
     internal::InternalMetadata* metadata, internal::ParseContext* ctx) {
   std::string payload;
-  uint32 type_id = 0;
-  bool payload_read = false;
+  uint32_t type_id;
+  enum class State { kNoTag, kHasType, kHasPayload, kDone };
+  State state = State::kNoTag;
+
   while (!ctx->Done(&ptr)) {
     uint32 tag = static_cast<uint8>(*ptr++);
     if (tag == WireFormatLite::kMessageSetTypeIdTag) {
       uint64 tmp;
       ptr = ParseBigVarint(ptr, &tmp);
       GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-      type_id = tmp;
-      if (payload_read) {
+      if (state == State::kNoTag) {
+        type_id = tmp;
+        state = State::kHasType;
+      } else if (state == State::kHasPayload) {
+        type_id = tmp;
         ExtensionInfo extension;
         bool was_packed_on_wire;
         if (!FindExtension(2, type_id, containing_type, ctx, &extension,
@@ -241,20 +246,24 @@ const char* ExtensionSet::ParseMessageSetItemTmpl(
           GOOGLE_PROTOBUF_PARSER_ASSERT(value->_InternalParse(p, &tmp_ctx) &&
                                          tmp_ctx.EndedAtLimit());
         }
-        type_id = 0;
+        state = State::kDone;
       }
     } else if (tag == WireFormatLite::kMessageSetMessageTag) {
-      if (type_id != 0) {
+      if (state == State::kHasType) {
         ptr = ParseFieldMaybeLazily(static_cast<uint64>(type_id) * 8 + 2, ptr,
                                     containing_type, metadata, ctx);
         GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);
-        type_id = 0;
+        state = State::kDone;
       } else {
+        std::string tmp;
         int32 size = ReadSize(&ptr);
         GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-        ptr = ctx->ReadString(ptr, size, &payload);
+        ptr = ctx->ReadString(ptr, size, &tmp);
         GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-        payload_read = true;
+        if (state == State::kNoTag) {
+          payload = std::move(tmp);
+          state = State::kHasPayload;
+        }
       }
     } else {
       ptr = ReadTag(ptr - 1, &tag);
diff --git a/src/google/protobuf/wire_format.cc b/src/google/protobuf/wire_format.cc
index c30b7abff..5c2370963 100644
--- a/src/google/protobuf/wire_format.cc
+++ b/src/google/protobuf/wire_format.cc
@@ -657,9 +657,11 @@ struct WireFormat::MessageSetParser {
   const char* _InternalParse(const char* ptr, internal::ParseContext* ctx) {
     // Parse a MessageSetItem
     auto metadata = reflection->MutableInternalMetadata(msg);
+    enum class State { kNoTag, kHasType, kHasPayload, kDone };
+    State state = State::kNoTag;
+
     std::string payload;
     uint32 type_id = 0;
-    bool payload_read = false;
     while (!ctx->Done(&ptr)) {
       // We use 64 bit tags in order to allow typeid's that span the whole
       // range of 32 bit numbers.
@@ -668,8 +670,11 @@ struct WireFormat::MessageSetParser {
         uint64 tmp;
         ptr = ParseBigVarint(ptr, &tmp);
         GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-        type_id = tmp;
-        if (payload_read) {
+        if (state == State::kNoTag) {
+          type_id = tmp;
+          state = State::kHasType;
+        } else if (state == State::kHasPayload) {
+          type_id = tmp;
           const FieldDescriptor* field;
           if (ctx->data().pool == nullptr) {
             field = reflection->FindKnownExtensionByNumber(type_id);
@@ -696,17 +701,17 @@ struct WireFormat::MessageSetParser {
             GOOGLE_PROTOBUF_PARSER_ASSERT(value->_InternalParse(p, &tmp_ctx) &&
                                            tmp_ctx.EndedAtLimit());
           }
-          type_id = 0;
+          state = State::kDone;
         }
         continue;
       } else if (tag == WireFormatLite::kMessageSetMessageTag) {
-        if (type_id == 0) {
+        if (state == State::kNoTag) {
           int32 size = ReadSize(&ptr);
           GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
           ptr = ctx->ReadString(ptr, size, &payload);
           GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-          payload_read = true;
-        } else {
+          state = State::kHasPayload;
+        } else if (state == State::kHasType) {
           // We're now parsing the payload
           const FieldDescriptor* field = nullptr;
           if (descriptor->IsExtensionNumber(type_id)) {
@@ -720,7 +725,12 @@ struct WireFormat::MessageSetParser {
           ptr = WireFormat::_InternalParseAndMergeField(
               msg, ptr, ctx, static_cast<uint64>(type_id) * 8 + 2, reflection,
               field);
-          type_id = 0;
+          state = State::kDone;
+        } else {
+          int32_t size = ReadSize(&ptr);
+          GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
+          ptr = ctx->Skip(ptr, size);
+          GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
         }
       } else {
         // An unknown field in MessageSetItem.
diff --git a/src/google/protobuf/wire_format_lite.h b/src/google/protobuf/wire_format_lite.h
index 8ffae812c..220779375 100644
--- a/src/google/protobuf/wire_format_lite.h
+++ b/src/google/protobuf/wire_format_lite.h
@@ -1798,6 +1798,9 @@ bool ParseMessageSetItemImpl(io::CodedInputStream* input, MS ms) {
   // we can parse it later.
   std::string message_data;

+  enum class State { kNoTag, kHasType, kHasPayload, kDone };
+  State state = State::kNoTag;
+
   while (true) {
     const uint32 tag = input->ReadTagNoLastTag();
     if (tag == 0) return false;
@@ -1806,26 +1809,34 @@ bool ParseMessageSetItemImpl(io::CodedInputStream* input, MS ms) {
       case WireFormatLite::kMessageSetTypeIdTag: {
         uint32 type_id;
         if (!input->ReadVarint32(&type_id)) return false;
-        last_type_id = type_id;
-
-        if (!message_data.empty()) {
+        if (state == State::kNoTag) {
+          last_type_id = type_id;
+          state = State::kHasType;
+        } else if (state == State::kHasPayload) {
           // We saw some message data before the type_id.  Have to parse it
           // now.
           io::CodedInputStream sub_input(
               reinterpret_cast<const uint8*>(message_data.data()),
               static_cast<int>(message_data.size()));
           sub_input.SetRecursionLimit(input->RecursionBudget());
-          if (!ms.ParseField(last_type_id, &sub_input)) {
+          if (!ms.ParseField(type_id, &sub_input)) {
             return false;
           }
           message_data.clear();
+          state = State::kDone;
         }

         break;
       }

       case WireFormatLite::kMessageSetMessageTag: {
-        if (last_type_id == 0) {
+        if (state == State::kHasType) {
+          // Already saw type_id, so we can parse this directly.
+          if (!ms.ParseField(last_type_id, input)) {
+            return false;
+          }
+          state = State::kDone;
+        } else if (state == State::kNoTag) {
           // We haven't seen a type_id yet.  Append this data to message_data.
           uint32 length;
           if (!input->ReadVarint32(&length)) return false;
@@ -1836,11 +1847,9 @@ bool ParseMessageSetItemImpl(io::CodedInputStream* input, MS ms) {
           auto ptr = reinterpret_cast<uint8*>(&message_data[0]);
           ptr = io::CodedOutputStream::WriteVarint32ToArray(length, ptr);
           if (!input->ReadRaw(ptr, length)) return false;
+          state = State::kHasPayload;
         } else {
-          // Already saw type_id, so we can parse this directly.
-          if (!ms.ParseField(last_type_id, input)) {
-            return false;
-          }
+          if (!ms.SkipField(tag, input)) return false;
         }

         break;
diff --git a/src/google/protobuf/wire_format_unittest.cc b/src/google/protobuf/wire_format_unittest.cc
index e75fc316f..c1439f184 100644
--- a/src/google/protobuf/wire_format_unittest.cc
+++ b/src/google/protobuf/wire_format_unittest.cc
@@ -585,28 +585,54 @@ TEST(WireFormatTest, ParseMessageSet) {
   EXPECT_EQ(message_set.DebugString(), dynamic_message_set.DebugString());
 }

-TEST(WireFormatTest, ParseMessageSetWithReverseTagOrder) {
+namespace {
+std::string BuildMessageSetItemStart() {
   std::string data;
   {
-    unittest::TestMessageSetExtension1 message;
-    message.set_i(123);
-    // Build a MessageSet manually with its message content put before its
-    // type_id.
     io::StringOutputStream output_stream(&data);
     io::CodedOutputStream coded_output(&output_stream);
     coded_output.WriteTag(WireFormatLite::kMessageSetItemStartTag);
+  }
+  return data;
+}
+std::string BuildMessageSetItemEnd() {
+  std::string data;
+  {
+    io::StringOutputStream output_stream(&data);
+    io::CodedOutputStream coded_output(&output_stream);
+    coded_output.WriteTag(WireFormatLite::kMessageSetItemEndTag);
+  }
+  return data;
+}
+std::string BuildMessageSetTestExtension1(int value = 123) {
+  std::string data;
+  {
+    UNITTEST::TestMessageSetExtension1 message;
+    message.set_i(value);
+    io::StringOutputStream output_stream(&data);
+    io::CodedOutputStream coded_output(&output_stream);
     // Write the message content first.
     WireFormatLite::WriteTag(WireFormatLite::kMessageSetMessageNumber,
                              WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
                              &coded_output);
     coded_output.WriteVarint32(message.ByteSizeLong());
     message.SerializeWithCachedSizes(&coded_output);
-    // Write the type id.
-    uint32 type_id = message.GetDescriptor()->extension(0)->number();
+  }
+  return data;
+}
+std::string BuildMessageSetItemTypeId(int extension_number) {
+  std::string data;
+  {
+    io::StringOutputStream output_stream(&data);
+    io::CodedOutputStream coded_output(&output_stream);
     WireFormatLite::WriteUInt32(WireFormatLite::kMessageSetTypeIdNumber,
-                                type_id, &coded_output);
-    coded_output.WriteTag(WireFormatLite::kMessageSetItemEndTag);
+                                extension_number, &coded_output);
   }
+  return data;
+}
+void ValidateTestMessageSet(const std::string& test_case,
+                            const std::string& data) {
+  SCOPED_TRACE(test_case);
   {
     proto2_wireformat_unittest::TestMessageSet message_set;
     ASSERT_TRUE(message_set.ParseFromString(data));
@@ -616,6 +642,11 @@ TEST(WireFormatTest, ParseMessageSetWithReverseTagOrder) {
                   .GetExtension(
                       unittest::TestMessageSetExtension1::message_set_extension)
                   .i());
+
+    // Make sure it does not contain anything else.
+    message_set.ClearExtension(
+        UNITTEST::TestMessageSetExtension1::message_set_extension);
+    EXPECT_EQ(message_set.SerializeAsString(), "");
   }
   {
     // Test parse the message via Reflection.
@@ -631,6 +662,61 @@ TEST(WireFormatTest, ParseMessageSetWithReverseTagOrder) {
                       unittest::TestMessageSetExtension1::message_set_extension)
                   .i());
   }
+  {
+    // Test parse the message via DynamicMessage.
+    DynamicMessageFactory factory;
+    std::unique_ptr<Message> msg(
+        factory
+            .GetPrototype(
+                PROTO2_WIREFORMAT_UNITTEST::TestMessageSet::descriptor())
+            ->New());
+    msg->ParseFromString(data);
+    auto* reflection = msg->GetReflection();
+    std::vector<const FieldDescriptor*> fields;
+    reflection->ListFields(*msg, &fields);
+    ASSERT_EQ(fields.size(), 1);
+    const auto& sub = reflection->GetMessage(*msg, fields[0]);
+    reflection = sub.GetReflection();
+    EXPECT_EQ(123, reflection->GetInt32(
+                       sub, sub.GetDescriptor()->FindFieldByName("i")));
+  }
+}
+}  // namespace
+
+TEST(WireFormatTest, ParseMessageSetWithAnyTagOrder) {
+  std::string start = BuildMessageSetItemStart();
+  std::string end = BuildMessageSetItemEnd();
+  std::string id = BuildMessageSetItemTypeId(
+      UNITTEST::TestMessageSetExtension1::descriptor()->extension(0)->number());
+  std::string message = BuildMessageSetTestExtension1();
+
+  ValidateTestMessageSet("id + message", start + id + message + end);
+  ValidateTestMessageSet("message + id", start + message + id + end);
+}
+
+TEST(WireFormatTest, ParseMessageSetWithDuplicateTags) {
+  std::string start = BuildMessageSetItemStart();
+  std::string end = BuildMessageSetItemEnd();
+  std::string id = BuildMessageSetItemTypeId(
+      UNITTEST::TestMessageSetExtension1::descriptor()->extension(0)->number());
+  std::string other_id = BuildMessageSetItemTypeId(123456);
+  std::string message = BuildMessageSetTestExtension1();
+  std::string other_message = BuildMessageSetTestExtension1(321);
+
+  // Double id
+  ValidateTestMessageSet("id + other_id + message",
+                         start + id + other_id + message + end);
+  ValidateTestMessageSet("id + message + other_id",
+                         start + id + message + other_id + end);
+  ValidateTestMessageSet("message + id + other_id",
+                         start + message + id + other_id + end);
+  // Double message
+  ValidateTestMessageSet("id + message + other_message",
+                         start + id + message + other_message + end);
+  ValidateTestMessageSet("message + id + other_message",
+                         start + message + id + other_message + end);
+  ValidateTestMessageSet("message + other_message + id",
+                         start + message + other_message + id + end);
 }

 void SerializeReverseOrder(
"""   # noqa

    filename = output_dir / 'protobuf.patch_3_15'

    with open(filename.as_posix(), 'w') as f:
        f.write(patch)

    return filename


def get_protobuf_patch_3_12(output_dir: Path) -> Path:
    ########################################################################
    # Protobuf 3.12.2 vulnerability.
    # https://github.com/protocolbuffers/protobuf/commit/d1635e1496f51e0d5653d856211e8821bc47adc4#diff-595eef663de11f5567c3e853fe7af8a49fb6e7afe45d4859d55ddfce67a2bb62
    # The patch was adjusted to 3.12 version
    ########################################################################

    patch = """diff --git a/src/google/protobuf/extension_set_inl.h b/src/google/protobuf/extension_set_inl.h
index 9957f8b..93db9d2 100644
--- a/src/google/protobuf/extension_set_inl.h
+++ b/src/google/protobuf/extension_set_inl.h
@@ -206,15 +206,21 @@ const char* ExtensionSet::ParseMessageSetItemTmpl(
     const char* ptr, const Msg* containing_type,
     internal::InternalMetadata* metadata, internal::ParseContext* ctx) {
   std::string payload;
-  uint32 type_id = 0;
+  uint32_t type_id;
+  enum class State { kNoTag, kHasType, kHasPayload, kDone };
+  State state = State::kNoTag;
+
   while (!ctx->Done(&ptr)) {
     uint32 tag = static_cast<uint8>(*ptr++);
     if (tag == WireFormatLite::kMessageSetTypeIdTag) {
       uint64 tmp;
       ptr = ParseBigVarint(ptr, &tmp);
       GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-      type_id = tmp;
-      if (!payload.empty()) {
+      if (state == State::kNoTag) {
+        type_id = tmp;
+        state = State::kHasType;
+      } else if (state == State::kHasPayload) {
+        type_id = tmp;
         ExtensionInfo extension;
         bool was_packed_on_wire;
         if (!FindExtension(2, type_id, containing_type, ctx, &extension,
@@ -240,19 +246,24 @@ const char* ExtensionSet::ParseMessageSetItemTmpl(
           GOOGLE_PROTOBUF_PARSER_ASSERT(value->_InternalParse(p, &tmp_ctx) &&
                                          tmp_ctx.EndedAtLimit());
         }
-        type_id = 0;
+        state = State::kDone;
       }
     } else if (tag == WireFormatLite::kMessageSetMessageTag) {
-      if (type_id != 0) {
+      if (state == State::kHasType) {
         ptr = ParseFieldMaybeLazily(static_cast<uint64>(type_id) * 8 + 2, ptr,
                                     containing_type, metadata, ctx);
         GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);
-        type_id = 0;
+        state = State::kDone;
       } else {
+        std::string tmp;
         int32 size = ReadSize(&ptr);
         GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-        ptr = ctx->ReadString(ptr, size, &payload);
+        ptr = ctx->ReadString(ptr, size, &tmp);
         GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
+        if (state == State::kNoTag) {
+          payload = std::move(tmp);
+          state = State::kHasPayload;
+        }
       }
     } else {
       ptr = ReadTag(ptr - 1, &tag);
diff --git a/src/google/protobuf/wire_format.cc b/src/google/protobuf/wire_format.cc
index 444510c..8d18df2 100644
--- a/src/google/protobuf/wire_format.cc
+++ b/src/google/protobuf/wire_format.cc
@@ -659,6 +659,9 @@ struct WireFormat::MessageSetParser {
   const char* _InternalParse(const char* ptr, internal::ParseContext* ctx) {
     // Parse a MessageSetItem
     auto metadata = reflection->MutableInternalMetadata(msg);
+    enum class State { kNoTag, kHasType, kHasPayload, kDone };
+    State state = State::kNoTag;
+
     std::string payload;
     uint32 type_id = 0;
     while (!ctx->Done(&ptr)) {
@@ -669,8 +672,11 @@ struct WireFormat::MessageSetParser {
         uint64 tmp;
         ptr = ParseBigVarint(ptr, &tmp);
         GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-        type_id = tmp;
-        if (!payload.empty()) {
+        if (state == State::kNoTag) {
+          type_id = tmp;
+          state = State::kHasType;
+        } else if (state == State::kHasPayload) {
+          type_id = tmp;
           const FieldDescriptor* field;
           if (ctx->data().pool == nullptr) {
             field = reflection->FindKnownExtensionByNumber(type_id);
@@ -697,16 +703,17 @@ struct WireFormat::MessageSetParser {
             GOOGLE_PROTOBUF_PARSER_ASSERT(value->_InternalParse(p, &tmp_ctx) &&
                                            tmp_ctx.EndedAtLimit());
           }
-          type_id = 0;
+          state = State::kDone;
         }
         continue;
       } else if (tag == WireFormatLite::kMessageSetMessageTag) {
-        if (type_id == 0) {
+        if (state == State::kNoTag) {
           int32 size = ReadSize(&ptr);
           GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
           ptr = ctx->ReadString(ptr, size, &payload);
           GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
-        } else {
+          state = State::kHasPayload;
+        } else if (state == State::kHasType) {
           // We're now parsing the payload
           const FieldDescriptor* field = nullptr;
           if (descriptor->IsExtensionNumber(type_id)) {
@@ -720,7 +727,12 @@ struct WireFormat::MessageSetParser {
           ptr = WireFormat::_InternalParseAndMergeField(
               msg, ptr, ctx, static_cast<uint64>(type_id) * 8 + 2, reflection,
               field);
-          type_id = 0;
+          state = State::kDone;
+        } else {
+          int32_t size = ReadSize(&ptr);
+          GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
+          ptr = ctx->Skip(ptr, size);
+          GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
         }
       } else {
         // An unknown field in MessageSetItem.
diff --git a/src/google/protobuf/wire_format_lite.h b/src/google/protobuf/wire_format_lite.h
index c742fe8..4130bc5 100644
--- a/src/google/protobuf/wire_format_lite.h
+++ b/src/google/protobuf/wire_format_lite.h
@@ -1798,6 +1798,9 @@ bool ParseMessageSetItemImpl(io::CodedInputStream* input, MS ms) {
   // we can parse it later.
   std::string message_data;

+  enum class State { kNoTag, kHasType, kHasPayload, kDone };
+  State state = State::kNoTag;
+
   while (true) {
     const uint32 tag = input->ReadTagNoLastTag();
     if (tag == 0) return false;
@@ -1806,26 +1809,34 @@ bool ParseMessageSetItemImpl(io::CodedInputStream* input, MS ms) {
       case WireFormatLite::kMessageSetTypeIdTag: {
         uint32 type_id;
         if (!input->ReadVarint32(&type_id)) return false;
-        last_type_id = type_id;
-
-        if (!message_data.empty()) {
+        if (state == State::kNoTag) {
+          last_type_id = type_id;
+          state = State::kHasType;
+        } else if (state == State::kHasPayload) {
           // We saw some message data before the type_id.  Have to parse it
           // now.
           io::CodedInputStream sub_input(
               reinterpret_cast<const uint8*>(message_data.data()),
               static_cast<int>(message_data.size()));
           sub_input.SetRecursionLimit(input->RecursionBudget());
-          if (!ms.ParseField(last_type_id, &sub_input)) {
+          if (!ms.ParseField(type_id, &sub_input)) {
             return false;
           }
           message_data.clear();
+          state = State::kDone;
         }

         break;
       }

       case WireFormatLite::kMessageSetMessageTag: {
-        if (last_type_id == 0) {
+        if (state == State::kHasType) {
+          // Already saw type_id, so we can parse this directly.
+          if (!ms.ParseField(last_type_id, input)) {
+            return false;
+          }
+          state = State::kDone;
+        } else if (state == State::kNoTag) {
           // We haven't seen a type_id yet.  Append this data to message_data.
           uint32 length;
           if (!input->ReadVarint32(&length)) return false;
@@ -1836,11 +1847,9 @@ bool ParseMessageSetItemImpl(io::CodedInputStream* input, MS ms) {
           auto ptr = reinterpret_cast<uint8*>(&message_data[0]);
           ptr = io::CodedOutputStream::WriteVarint32ToArray(length, ptr);
           if (!input->ReadRaw(ptr, length)) return false;
+          state = State::kHasPayload;
         } else {
-          // Already saw type_id, so we can parse this directly.
-          if (!ms.ParseField(last_type_id, input)) {
-            return false;
-          }
+          if (!ms.SkipField(tag, input)) return false;
         }

         break;
diff --git a/src/google/protobuf/wire_format_unittest.cc b/src/google/protobuf/wire_format_unittest.cc
index 24cb58f..99941bc 100644
--- a/src/google/protobuf/wire_format_unittest.cc
+++ b/src/google/protobuf/wire_format_unittest.cc
@@ -581,28 +581,54 @@ TEST(WireFormatTest, ParseMessageSet) {
   EXPECT_EQ(message_set.DebugString(), dynamic_message_set.DebugString());
 }

-TEST(WireFormatTest, ParseMessageSetWithReverseTagOrder) {
+namespace {
+std::string BuildMessageSetItemStart() {
   std::string data;
   {
-    unittest::TestMessageSetExtension1 message;
-    message.set_i(123);
-    // Build a MessageSet manually with its message content put before its
-    // type_id.
     io::StringOutputStream output_stream(&data);
     io::CodedOutputStream coded_output(&output_stream);
     coded_output.WriteTag(WireFormatLite::kMessageSetItemStartTag);
+  }
+  return data;
+}
+std::string BuildMessageSetItemEnd() {
+  std::string data;
+  {
+    io::StringOutputStream output_stream(&data);
+    io::CodedOutputStream coded_output(&output_stream);
+    coded_output.WriteTag(WireFormatLite::kMessageSetItemEndTag);
+  }
+  return data;
+}
+std::string BuildMessageSetTestExtension1(int value = 123) {
+  std::string data;
+  {
+    UNITTEST::TestMessageSetExtension1 message;
+    message.set_i(value);
+    io::StringOutputStream output_stream(&data);
+    io::CodedOutputStream coded_output(&output_stream);
     // Write the message content first.
     WireFormatLite::WriteTag(WireFormatLite::kMessageSetMessageNumber,
                              WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
                              &coded_output);
     coded_output.WriteVarint32(message.ByteSize());
     message.SerializeWithCachedSizes(&coded_output);
-    // Write the type id.
-    uint32 type_id = message.GetDescriptor()->extension(0)->number();
+  }
+  return data;
+}
+std::string BuildMessageSetItemTypeId(int extension_number) {
+  std::string data;
+  {
+    io::StringOutputStream output_stream(&data);
+    io::CodedOutputStream coded_output(&output_stream);
     WireFormatLite::WriteUInt32(WireFormatLite::kMessageSetTypeIdNumber,
-                                type_id, &coded_output);
-    coded_output.WriteTag(WireFormatLite::kMessageSetItemEndTag);
+                                extension_number, &coded_output);
   }
+  return data;
+}
+void ValidateTestMessageSet(const std::string& test_case,
+                            const std::string& data) {
+  SCOPED_TRACE(test_case);
   {
     proto2_wireformat_unittest::TestMessageSet message_set;
     ASSERT_TRUE(message_set.ParseFromString(data));
@@ -612,6 +638,11 @@ TEST(WireFormatTest, ParseMessageSetWithReverseTagOrder) {
                   .GetExtension(
                       unittest::TestMessageSetExtension1::message_set_extension)
                   .i());
+
+    // Make sure it does not contain anything else.
+    message_set.ClearExtension(
+        UNITTEST::TestMessageSetExtension1::message_set_extension);
+    EXPECT_EQ(message_set.SerializeAsString(), "");
   }
   {
     // Test parse the message via Reflection.
@@ -627,6 +658,61 @@ TEST(WireFormatTest, ParseMessageSetWithReverseTagOrder) {
                       unittest::TestMessageSetExtension1::message_set_extension)
                   .i());
   }
+  {
+    // Test parse the message via DynamicMessage.
+    DynamicMessageFactory factory;
+    std::unique_ptr<Message> msg(
+        factory
+            .GetPrototype(
+                PROTO2_WIREFORMAT_UNITTEST::TestMessageSet::descriptor())
+            ->New());
+    msg->ParseFromString(data);
+    auto* reflection = msg->GetReflection();
+    std::vector<const FieldDescriptor*> fields;
+    reflection->ListFields(*msg, &fields);
+    ASSERT_EQ(fields.size(), 1);
+    const auto& sub = reflection->GetMessage(*msg, fields[0]);
+    reflection = sub.GetReflection();
+    EXPECT_EQ(123, reflection->GetInt32(
+                       sub, sub.GetDescriptor()->FindFieldByName("i")));
+  }
+}
+}  // namespace
+
+TEST(WireFormatTest, ParseMessageSetWithAnyTagOrder) {
+  std::string start = BuildMessageSetItemStart();
+  std::string end = BuildMessageSetItemEnd();
+  std::string id = BuildMessageSetItemTypeId(
+      UNITTEST::TestMessageSetExtension1::descriptor()->extension(0)->number());
+  std::string message = BuildMessageSetTestExtension1();
+
+  ValidateTestMessageSet("id + message", start + id + message + end);
+  ValidateTestMessageSet("message + id", start + message + id + end);
+}
+
+TEST(WireFormatTest, ParseMessageSetWithDuplicateTags) {
+  std::string start = BuildMessageSetItemStart();
+  std::string end = BuildMessageSetItemEnd();
+  std::string id = BuildMessageSetItemTypeId(
+      UNITTEST::TestMessageSetExtension1::descriptor()->extension(0)->number());
+  std::string other_id = BuildMessageSetItemTypeId(123456);
+  std::string message = BuildMessageSetTestExtension1();
+  std::string other_message = BuildMessageSetTestExtension1(321);
+
+  // Double id
+  ValidateTestMessageSet("id + other_id + message",
+                         start + id + other_id + message + end);
+  ValidateTestMessageSet("id + message + other_id",
+                         start + id + message + other_id + end);
+  ValidateTestMessageSet("message + id + other_id",
+                         start + message + id + other_id + end);
+  // Double message
+  ValidateTestMessageSet("id + message + other_message",
+                         start + id + message + other_message + end);
+  ValidateTestMessageSet("message + id + other_message",
+                         start + message + id + other_message + end);
+  ValidateTestMessageSet("message + other_message + id",
+                         start + message + other_message + id + end);
 }

 void SerializeReverseOrder(
"""   # noqa

    filename = output_dir / 'protobuf_3_12.patch'

    with open(filename.as_posix(), 'w') as f:
        f.write(patch)

    return filename
