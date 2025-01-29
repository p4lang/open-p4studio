project(libbfrt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

include(PythonDependencies)

include_directories(.)
set(BFRT_SRCS
bf_rt_common/bf_rt_info_impl.cpp
bf_rt_common/bf_rt_cjson.cpp
bf_rt_common/bf_rt_table_data_impl.cpp
bf_rt_common/bf_rt_table_key_impl.cpp
bf_rt_common/bf_rt_init.cpp
bf_rt_common/bf_rt_init_impl.cpp
bf_rt_common/bf_rt_table_attributes_impl.cpp
bf_rt_common/bf_rt_table_operations_impl.cpp
bf_rt_common/bf_rt_utils.cpp
bf_rt_common/bf_rt_table_impl.cpp
bf_rt_common/bf_rt_session_impl.cpp
bf_rt_common/bf_rt_pipe_mgr_intf.cpp
bf_rt_mirror/bf_rt_mirror_table_data_impl.cpp
bf_rt_mirror/bf_rt_mirror_table_impl.cpp
bf_rt_mirror/bf_rt_mirror_table_key_impl.cpp
bf_rt_p4/bf_rt_learn_impl.cpp
bf_rt_p4/bf_rt_learn_state.cpp
bf_rt_p4/bf_rt_p4_table_data_impl.cpp
bf_rt_p4/bf_rt_p4_table_impl.cpp
bf_rt_p4/bf_rt_p4_table_key_impl.cpp
bf_rt_p4/bf_rt_table_attributes_state.cpp
bf_rt_p4/bf_rt_table_state.cpp
bf_rt_port/bf_rt_port_mgr_intf.cpp
bf_rt_port/bf_rt_port_table_data_impl.cpp
bf_rt_port/bf_rt_port_table_key_impl.cpp
bf_rt_port/bf_rt_port_table_impl.cpp
bf_rt_port/bf_rt_port_table_attributes_state.cpp
bf_rt_dev/bf_rt_dev_table_data_impl.cpp
bf_rt_dev/bf_rt_dev_table_impl.cpp
bf_rt_pktgen/bf_rt_pktgen_table_data_impl.cpp
bf_rt_pktgen/bf_rt_pktgen_table_key_impl.cpp
bf_rt_pktgen/bf_rt_pktgen_table_impl.cpp
bf_rt_pre/bf_rt_mc_mgr_intf.cpp
bf_rt_pre/bf_rt_pre_state.cpp
bf_rt_pre/bf_rt_pre_table_data_impl.cpp
bf_rt_pre/bf_rt_pre_table_impl.cpp
bf_rt_pre/bf_rt_pre_table_key_impl.cpp
bf_rt_dev/bf_rt_dev_table_data_impl.cpp
bf_rt_dev/bf_rt_dev_table_impl.cpp
bf_rt_tm/bf_rt_tm_intf.cpp
bf_rt_tm/bf_rt_tm_state.cpp
bf_rt_tm/bf_rt_tm_table_data_impl.cpp
bf_rt_tm/bf_rt_tm_table_key_impl.cpp
bf_rt_tm/bf_rt_tm_table_impl.cpp
bf_rt_tm/bf_rt_tm_table_impl_port.cpp
bf_rt_tm/bf_rt_tm_table_impl_portgroup.cpp
bf_rt_tm/bf_rt_tm_table_helper_ppg.cpp
bf_rt_tm/bf_rt_tm_table_helper_sched.cpp
bf_rt_tm/bf_rt_tm_table_helper_pipe.cpp
bf_rt_tm/bf_rt_tm_table_impl_ppg.cpp
bf_rt_tm/bf_rt_tm_table_impl_mirror.cpp
bf_rt_tm/bf_rt_tm_table_impl_queue.cpp
bf_rt_tm/bf_rt_tm_table_impl_l1_node.cpp
bf_rt_tm/bf_rt_tm_table_impl_pool.cpp
bf_rt_tm/bf_rt_tm_table_impl_counters.cpp
bf_rt_tm/bf_rt_tm_table_impl_cfg.cpp
bf_rt_tm/bf_rt_tm_table_impl_pipe.cpp
c_frontend/bf_rt_session_c.cpp
c_frontend/bf_rt_table_c.cpp
c_frontend/bf_rt_table_data_c.cpp
c_frontend/bf_rt_table_key_c.cpp
c_frontend/bf_rt_table_attributes_c.cpp
c_frontend/bf_rt_table_operations_c.cpp
c_frontend/bf_rt_learn_c.cpp
c_frontend/bf_rt_state_c.cpp
c_frontend/bf_rt_init_c.cpp
c_frontend/bf_rt_info_c.cpp
)

if(BF-PYTHON)
  add_library(bfshell_plugin_bf_rt_o OBJECT cli/bf_rt_cli.c)
  target_include_directories(bfshell_plugin_bf_rt_o PUBLIC ${Python3_INCLUDE_DIRS})
  add_library(bfshell_plugin_bf_rt SHARED $<TARGET_OBJECTS:bfshell_plugin_bf_rt_o>)
  target_link_libraries(bfshell_plugin_bf_rt ${Python3_LIBRARIES})
  SET_TARGET_PROPERTIES(bfshell_plugin_bf_rt PROPERTIES PREFIX "")
  install(FILES xml/bf_rt.xml DESTINATION share/cli/xml)
endif()

install(FILES
  bf_rt_mirror/bf_rt_mirror_tf1.json
  bf_rt_mirror/bf_rt_mirror_tf2.json
  bf_rt_mirror/bf_rt_mirror_tf3.json
  bf_rt_dev/bf_rt_dev_tf1.json
  bf_rt_dev/bf_rt_dev_tf2.json
  bf_rt_dev/bf_rt_dev_tf3.json
  bf_rt_port/bf_rt_port_tf1.json
  bf_rt_port/bf_rt_port_tf2.json
  bf_rt_port/bf_rt_port_tf3.json
  bf_rt_pktgen/bf_rt_pktgen_tf1.json
  bf_rt_pktgen/bf_rt_pktgen_tf2.json
  bf_rt_pktgen/bf_rt_pktgen_tf3.json
  bf_rt_pre/bf_rt_pre_tf1.json
  bf_rt_pre/bf_rt_pre_tf2.json
  bf_rt_pre/bf_rt_pre_tf3.json
  bf_rt_tm/bf_rt_tm_tf1.json
  bf_rt_tm/bf_rt_tm_tf2.json
  bf_rt_tm/bf_rt_tm_tf3.json

  bf_rt_p4/bf_rt_p4_tf1.json
  bf_rt_p4/bf_rt_p4_tf2.json
  bf_rt_p4/bf_rt_p4_tf3.json

  DESTINATION share/bf_rt_shared
)

set(PROTO_SRC "")
set(BFRT_GRPC_SRCS "")
set(PROTO_CXX_SRC "")
set(GRPC_CXX_SRC "")

if(GRPC)
  find_package(Protobuf REQUIRED)
  find_package(GRPC REQUIRED)
  include_directories(SYSTEM ${PROTOBUF_INCLUDE_DIR})
  include_directories(SYSTEM ${GRPC_INCLUDE_DIR})

  set(BFRT_GRPC_SRCS
    proto/bf_rt_server_impl.cpp
    proto/bf_rt_server_mgr.cpp
  )
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cpp_out")
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/python_out")

  set(PROTO_CXX_DST_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpp_out")
  set(PROTO_PYTHON_DST_DIR "${CMAKE_CURRENT_BINARY_DIR}/python_out")
  set(PROTO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/proto/bfruntime.proto")

  if(BFRT-GO)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/go_out")
    set(PROTO_GO_DST_DIR "${CMAKE_CURRENT_BINARY_DIR}/go_out")
    file(WRITE "${PROTO_GO_DST_DIR}/go.mod" "package bfrt_proto")
    set(PROTO_GO_SRC "${PROTO_GO_DST_DIR}/bfruntime.pb.go"
               "${PROTO_GO_DST_DIR}/bfruntime_grpc.pb.go")
    add_custom_command(
        OUTPUT "${PROTO_GO_DST_DIR}/bfruntime.pb.go"
               "${PROTO_GO_DST_DIR}/bfruntime_grpc.pb.go"
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
        ARGS --go_out ${PROTO_GO_DST_DIR} --go-grpc_out ${PROTO_GO_DST_DIR}
             -I "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/"
             -I "${CMAKE_CURRENT_SOURCE_DIR}/proto" ${PROTO_SRC}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/"
        DEPENDS "${PROTO_SRC}" ${PROTOBUF_PROTOC_EXECUTABLE}
        COMMENT "Generating Golang gRPC and protobuf files from ${PROTO_SRC}"
        VERBATIM
    )
    set_source_files_properties(${PROTO_GO_SRC} PROPERTIES GENERATED TRUE)
    add_custom_target(bfruntime_pb_go_target ALL DEPENDS "${PROTO_GO_DST_DIR}/bfruntime.pb.go")
    add_custom_target(bfruntime_grpc_pb_go_target ALL DEPENDS "${PROTO_GO_DST_DIR}/bfruntime_grpc.pb.go")
  endif()

  set(PROTO_CXX_SRC
        "${PROTO_CXX_DST_DIR}/bfruntime.pb.cc"
        "${PROTO_CXX_DST_DIR}/bfruntime.pb.h"
     )
  set(GRPC_CXX_SRC
        "${PROTO_CXX_DST_DIR}/bfruntime.grpc.pb.cc"
        "${PROTO_CXX_DST_DIR}/bfruntime.grpc.pb.h"
     )

  add_custom_command(
      OUTPUT "${PROTO_CXX_DST_DIR}/bfruntime.pb.cc"
             "${PROTO_CXX_DST_DIR}/bfruntime.pb.h"
             "${PROTO_CXX_DST_DIR}/bfruntime.grpc.pb.cc"
             "${PROTO_CXX_DST_DIR}/bfruntime.grpc.pb.h"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out ${PROTO_CXX_DST_DIR} --grpc_out ${PROTO_CXX_DST_DIR}
           -I "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/"
           -I "${CMAKE_CURRENT_SOURCE_DIR}/proto"
           --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${PROTO_SRC}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/"
      DEPENDS "${PROTO_SRC}" ${PROTOBUF_PROTOC_EXECUTABLE} ${GRPC_CPP_PLUGIN}
      COMMENT "Generating cpp gRPC and protobuf files from ${PROTO_SRC}"
      VERBATIM
  )

  set_source_files_properties(${PROTO_CXX_SRC} ${GRPC_CXX_SRC} PROPERTIES GENERATED TRUE)
  include_directories("${CMAKE_CURRENT_BINARY_DIR}/cpp_out")
  include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../third-party/cpp_out")
  add_library(bfrt_grpc_proto SHARED EXCLUDE_FROM_ALL ${PROTO_CXX_SRC} ${GRPC_CXX_SRC})

  add_custom_command(
      OUTPUT "${PROTO_PYTHON_DST_DIR}/bfruntime_pb2.py"
             "${PROTO_PYTHON_DST_DIR}/bfruntime_pb2_grpc.py"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --python_out ${PROTO_PYTHON_DST_DIR} --grpc_out ${PROTO_PYTHON_DST_DIR}
           -I "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/"
           -I "${CMAKE_CURRENT_SOURCE_DIR}/proto"
           --plugin=protoc-gen-grpc=${GRPC_PYTHON_PLUGIN} ${PROTO_SRC}
      COMMAND sed
      ARGS -i
      "s/import bfruntime_pb2 as bfruntime__pb2/from . import bfruntime_pb2 as bfruntime__pb2/g"
      "${PROTO_PYTHON_DST_DIR}/bfruntime_pb2_grpc.py"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/"
      DEPENDS "${PROTO_SRC}" ${PROTOBUF_PROTOC_EXECUTABLE} ${GRPC_PYTHON_PLUGIN}
      COMMENT "Generating python gRPC and protobuf files from ${PROTO_SRC}"
      VERBATIM
  )
  add_custom_target(
      bfruntime_pb2_grpc_py_target ALL
      DEPENDS "${PROTO_PYTHON_DST_DIR}/bfruntime_pb2.py" "${PROTO_PYTHON_DST_DIR}/bfruntime_pb2_grpc.py"
  )
  install(FILES ${PROTO_PYTHON_DST_DIR}/bfruntime_pb2.py DESTINATION
    ${SDE_PYTHON_DEPENDENCIES_DIR}/tofino/bfrt_grpc)
  install(FILES ${PROTO_PYTHON_DST_DIR}/bfruntime_pb2_grpc.py DESTINATION
    ${SDE_PYTHON_DEPENDENCIES_DIR}/tofino/bfrt_grpc)

  install(FILES ${PROTO_SRC} DESTINATION share/bf_rt_shared/proto)

  set_source_files_properties(${PROTO_CXX_SRC} ${BFRT_GRPC_SRCS} ${GRPC_CXX_SRC} PROPERTIES
    COMPILE_FLAGS -Wno-error=deprecated-declarations)
  add_library(bfrt_o OBJECT
    ${BFRT_SRCS}
    ${BFRT_GRPC_SRCS}
    ${PROTO_CXX_SRC}
    ${GRPC_CXX_SRC}
  )
  add_dependencies(bfrt_o bf_google_grpc_o bf_google_protobuf_o)
  add_library(bfrt SHARED EXCLUDE_FROM_ALL
    $<TARGET_OBJECTS:bfrt_o>
    $<TARGET_OBJECTS:bf_google_grpc_o>
    $<TARGET_OBJECTS:bf_google_protobuf_o>
  )
  target_link_libraries(bfrt PUBLIC ${PROTOBUF_LIBRARY} ${GRPC_LIBRARY} ${GRPC_GRPC++_LIBRARY} ${GRPC_GRPC++_REFLECTION_LIBRARY})
else()
  add_library(bfrt_o OBJECT ${BFRT_SRCS})
  add_library(bfrt SHARED EXCLUDE_FROM_ALL $<TARGET_OBJECTS:bfrt_o>)
endif()

# Install bfrt_python files to python install directory
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bf_rt_python/ DESTINATION lib/python${BF_PYTHON_VER}/site-packages)

#Install bfruntime python client files
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/bfruntime_grpc_client/python/client.py
  DESTINATION ${SDE_PYTHON_DEPENDENCIES_DIR}/tofino/bfrt_grpc)
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/bfruntime_grpc_client/python/info_parse.py
  DESTINATION ${SDE_PYTHON_DEPENDENCIES_DIR}/tofino/bfrt_grpc)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/python_out/__init__.py "")
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/python_out/__init__.py
  DESTINATION ${SDE_PYTHON_DEPENDENCIES_DIR}/tofino/bfrt_grpc/)

add_subdirectory(tests EXCLUDE_FROM_ALL)

# Building bf_rt doxygen
find_package(Doxygen)
find_package(Doxypy)
if(DOXYGEN_FOUND)
  set(srcdir ${CMAKE_CURRENT_SOURCE_DIR}/docs)
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)
  add_custom_target(doc-driver-bfrt
                    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating BFRT Doxygen"
                    VERBATIM)
  set_target_properties(doc-driver-bfrt PROPERTIES EXCLUDE_FROM_ALL TRUE)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION
    share/doc/bf-drivers/bf_rt OPTIONAL)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/latex DESTINATION
    share/doc/bf-drivers/bf_rt OPTIONAL)

  # BF-RT fixed function tables
  set(srcdir ${CMAKE_CURRENT_SOURCE_DIR}/bf_rt_fixed/docs)
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/bf_rt_fixed/docs/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/bf_rt_fixed/Doxyfile)
  # Tag file for external links to this manual
  set(tagfile ${CMAKE_CURRENT_BINARY_DIR}/bf_rt_fixed/Tagfile_bf_rt_fixed)
  # Tag files for external links from this manual. Doxygen requires unique file names.
  set(tagfile_tm ${CMAKE_CURRENT_BINARY_DIR}/../traffic_mgr/Tagfile_traffic_mgr=../../traffic_mgr/html)
  # To access doxygen custom filters
  set(toolsdir ${CMAKE_CURRENT_SOURCE_DIR}/../../tools)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)
  add_custom_target(doc-driver-bfrt-fixed
                    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bf_rt_fixed
                    COMMENT "Generating BFRT Doxygen for fixed function tables"
                    VERBATIM)
  set_target_properties(doc-driver-bfrt-fixed PROPERTIES EXCLUDE_FROM_ALL TRUE)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bf_rt_fixed/html DESTINATION
    share/doc/bf-drivers/bf_rt_fixed OPTIONAL)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bf_rt_fixed/latex DESTINATION
    share/doc/bf-drivers/bf_rt_fixed OPTIONAL)

  # Building bfruntime_grpc_client doxygen
  if(DOXYPY_FOUND)
    set(srcdir ${CMAKE_CURRENT_SOURCE_DIR}/bfruntime_grpc_client/python/docs)
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/bfruntime_grpc_client/python/docs/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/bfruntime_grpc_client/python/Doxyfile)
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)
    add_custom_target(doc-driver-bfrt-python-grpc
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bfruntime_grpc_client/python
                      COMMENT "Generating bfruntime gRPC python client Doxygen"
                      VERBATIM)
    set_target_properties(doc-driver-bfrt-python-grpc PROPERTIES EXCLUDE_FROM_ALL TRUE)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bfruntime_grpc_client/python/html DESTINATION
      share/doc/bf-drivers/bf_rt/bfruntime_grpc_client/python/ OPTIONAL)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bfruntime_grpc_client/python/latex DESTINATION
      share/doc/bf-drivers/bf_rt/bfruntime_grpc_client/python/ OPTIONAL)
    else()
      message(WARNING "Doxypy is needed to build BFRT gRPC python client documentation. Please install doxypy.")
    endif()
else()
  message(WARNING "Doxygen is needed to build the documentation. Please install doxygen.")
endif()

