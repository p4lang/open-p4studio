#
# what follows is cmake equivalent of p4-build
# outputs for stful as example for tofino target:
#   -- *.bin and context.json <- make make stful-tofino-gen
#   -- libpd.so               <- make stful-tofino-pd
#   -- libpdcli.so            <- make stful-tofino-pdcli
#   -- libpdthrift.so         <- make stful-tofino-pdthrift
# Dummy target for above intermediate targets is "make stful-tofino"
# 
# Similar build targets are created for each device
# Finally, "make stful" builds all of the above for stful
#
# Users of this module need to set the following flags before using it.
# P4C - Path to the BF-P4C compiler
# P4FLAGS - P4 compiler flags
# P4PPFLAGS - P4 pre processing flags
# PDFLAGS - Program dependent flags for PD API generation
# P4_tofino_ARCHITECTURE - tna/t2na/psa/v1model
#

include(PythonDependencies)

execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "if True:
    from distutils import sysconfig as sc
    print(sc.get_python_lib(prefix='', standard_lib=True, plat_specific=True))"
  OUTPUT_VARIABLE PYTHON_SITE
  OUTPUT_STRIP_TRAILING_WHITESPACE)
set(PYTHON_SITE "${PYTHON_SITE}/site-packages")

###############################################################################
# P4 Build with BFRT
###############################################################################

#
# Compile P4 example programs with BFRT per target
#
# p4_build_target (<program_name> <arch> <target> <location>)
# t - name of the p4 example program to build
# arch - P4 architecture (tna, t2na, etc.)
# target - Target device (tofino, tofino2, tofino2m, etc.)
# location - directory of the P4 program
# input_rt_list (OPTIONAL) - 5th param ARGV4. Array of runtime lists.
#           Can be either or both "bfrt;p4rt"
#
function(P4_BUILD_TARGET t arch target p4program)
  set(input_rt_list ${ARGV4})
  # set output files/ commands  based upon whether bfrt and/or p4rt is needed
  set(output_files "")
  set(rt_commands "")
  set(depends_target "")
  set(chiptype ${target})
  if (${target} STREQUAL "tofino2m")
    set(chiptype "tofino2")
  else()
    set(chiptype ${target})
  endif()

  if ("${input_rt_list}" STREQUAL "")
    set(input_rt_list "bfrt")
  endif()
  foreach(rt in lists input_rt_list)
    if (${rt} STREQUAL "bfrt")
      set(output_files "${output_files}" "${t}/${target}/bf-rt.json")
      set(rt_commands "${rt_commands}" "--bf-rt-schema" "${t}/${target}/bf-rt.json")
      set(depends_target "${depends_target}" "${t}/${target}/bf-rt.json")
    elseif (${rt} STREQUAL "p4rt")
      set(output_files "${output_files}" "${t}/${target}/p4info.pb.txt")
      set(rt_commands "${rt_commands}" "--p4runtime-files" "${t}/${target}/p4info.pb.txt")
      set(depends_target "${depends_target}" "${t}/${target}/p4info.pb.txt")
    endif()
  endforeach()
  if (WITH-P4INFO)
      set(output_files "${output_files}" "${t}/${target}/${t}.p4info.pb.txt")
      set(rt_commands "${rt_commands}" "--p4runtime-files" "${t}/${target}/${t}.p4info.pb.txt" "--p4runtime-force-std-externs")
      set(depends_target "${depends_target}" "${t}/${target}/${t}.p4info.pb.txt")
  endif()

  separate_arguments(COMPUTED_P4FLAGS UNIX_COMMAND ${P4FLAGS})
  separate_arguments(COMPUTED_P4PPFLAGS UNIX_COMMAND ${P4PPFLAGS})
  # compile the p4 program
  add_custom_command(OUTPUT ${output_files}
    COMMAND ${P4C} --std ${P4_LANG} --target ${target} --arch ${arch} ${rt_commands} -o ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target} ${COMPUTED_P4PPFLAGS} ${COMPUTED_P4FLAGS} ${P4FLAGS_INTERNAL} -g ${p4program}
    COMMAND ${P4C-GEN-BFRT-CONF} --name ${t} --device ${chiptype} --testdir ./${t}/${target}
         --installdir share/${target}pd/${t} --pipe `${P4C-MANIFEST-CONFIG} --pipe ./${t}/${target}/manifest.json`
    DEPENDS ${p4program} bf-p4c
  )
   add_custom_target(${t}-${target} DEPENDS ${depends_target} driver)
  # install generated conf file
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/ DESTINATION share/p4/targets/${target}
    FILES_MATCHING
    PATTERN "*.conf"
    PATTERN "pipe" EXCLUDE
    PATTERN "logs" EXCLUDE
    PATTERN "graphs" EXCLUDE
  )
  # install bf-rt.json, context.json and tofino.bin
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/ DESTINATION share/${target}pd/${t}
    FILES_MATCHING
    PATTERN "*.json"
    PATTERN "*.bin"
    PATTERN "*.pb.txt"
    PATTERN "*manifest*" EXCLUDE
    PATTERN "logs" EXCLUDE
    PATTERN "graphs" EXCLUDE
    PATTERN "*dynhash*" EXCLUDE
    PATTERN "*prim*" EXCLUDE
    PATTERN "source*" EXCLUDE
    PATTERN "event*" EXCLUDE
    PATTERN "frontend*" EXCLUDE
  )
endfunction()

#
# Wrapper function for P4_BUILD_TARGET above
# t - name of the p4 example program to build
# input_rt_list (OPTIONAL) - 3rd param ARGV2. Array of runtime lists.
#           Can be either or both"bfrt;p4rt"
#
function(P4_BUILD t location)
  set(input_rt_list ${ARGV2})

  if (TOFINO)
    p4_build_target(${t} ${P4_tofino_ARCHITECTURE} "tofino" ${CMAKE_CURRENT_SOURCE_DIR}/${location}/${t}/${t}.p4 ${input_rt_list})
  endif()
  if (TOFINO2)
    p4_build_target(${t} ${P4_tofino2_ARCHITECTURE} "tofino2" ${CMAKE_CURRENT_SOURCE_DIR}/${location}/${t}/${t}.p4 ${input_rt_list})
  endif()
  if (TOFINO2M)
    p4_build_target(${t} ${P4_tofino2_ARCHITECTURE} "tofino2m" ${CMAKE_CURRENT_SOURCE_DIR}/${location}/${t}/${t}.p4 ${input_rt_list})
  endif()
  if (TOFINO3)
    p4_build_target(${t} ${P4_tofino3_ARCHITECTURE} "tofino3" ${CMAKE_CURRENT_SOURCE_DIR}/${location}/${t}/${t}.p4 ${input_rt_list})
  endif()
  add_custom_target(${t} DEPENDS
    $<$<BOOL:${TOFINO}>:${t}-tofino>
    $<$<BOOL:${TOFINO2}>:${t}-tofino2>
    $<$<BOOL:${TOFINO2M}>:${t}-tofino2m>
    $<$<BOOL:${TOFINO3}>:${t}-tofino3>
  )
endfunction()

###############################################################################
# P4 Build with PD
###############################################################################

#
# Compile P4 example programs and generate PD artifacts per target
#
# p4_build_pd_target (<program_name> <arch> <target> <location>)
# t - name of the p4-14 example program to build
# arch - P4 architecture
# target - Target device
# location - directory of the P4 program
#
function(P4_BUILD_PD_TARGET t arch target p4program)
  separate_arguments(COMPUTED_P4FLAGS UNIX_COMMAND ${P4FLAGS})
  separate_arguments(COMPUTED_P4PPFLAGS UNIX_COMMAND ${P4PPFLAGS})
  separate_arguments(COMPUTED_PDFLAGS UNIX_COMMAND ${PDFLAGS})
  # compile the p4 program
  add_custom_command(OUTPUT ${t}/${target}/manifest.json
    COMMAND ${P4C} --std ${P4_LANG} --target ${target} --arch ${arch} --no-bf-rt-schema -o ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target} ${COMPUTED_P4PPFLAGS} ${COMPUTED_P4FLAGS} ${P4FLAGS_INTERNAL} -g ${p4program}
    DEPENDS ${p4program} bf-p4c
  )

  # generate pd.c, pd.h and pd thrift files
  set(PDDOTC ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/src/pd.c)
  set(PDCLIDOTC ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/src/pdcli.c)
  set(PDRPCDOTTHRIFT ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/thrift/p4_pd_rpc.thrift)
  set(PDTHRIFT
    ${t}/${target}/gen-cpp/p4_prefix.h
    ${t}/${target}/gen-cpp/p4_prefix0.cpp
    ${t}/${target}/gen-cpp/p4_prefix1.cpp
    ${t}/${target}/gen-cpp/p4_prefix2.cpp
    ${t}/${target}/gen-cpp/p4_prefix3.cpp
    ${t}/${target}/gen-cpp/p4_prefix4.cpp
    ${t}/${target}/gen-cpp/p4_prefix5.cpp
    ${t}/${target}/gen-cpp/p4_prefix6.cpp
    ${t}/${target}/gen-cpp/p4_prefix7.cpp
    ${t}/${target}/gen-cpp/p4_pd_rpc_types.h
    ${t}/${target}/gen-cpp/res_types.h
    ${t}/${target}/gen-cpp/p4_pd_rpc_types.cpp
    ${t}/${target}/gen-cpp/res_types.cpp
    ${t}/${target}/thrift-src/bfn_pd_rpc_server.cpp
    ${t}/${target}/thrift-src/bfn_pd_rpc_server.h
    ${t}/${target}/thrift-src/p4_pd_rpc_server.ipp
  )
  if ( THRIFT_VERSION_STRING VERSION_LESS 0.14.0 )
    list(APPEND PDTHRIFT
      ${t}/${target}/gen-cpp/p4_pd_rpc_constants.h
      ${t}/${target}/gen-cpp/res_constants.h
      ${t}/${target}/gen-cpp/p4_pd_rpc_constants.cpp
      ${t}/${target}/gen-cpp/res_constants.cpp
    )
  endif()

  # generate pd.c, pdcli.c, p4_pd_rpc.thrift
  add_custom_command(OUTPUT ${PDDOTC} ${PDCLIDOTC} ${PDRPCDOTTHRIFT}
    COMMAND ${PYTHON_COMMAND} ${PDGEN} --path ${t}/${target} --manifest ${t}/${target}/manifest.json ${COMPUTED_PDFLAGS} ${PDFLAGS_INTERNAL} -o ${t}/${target}
    COMMAND ${PYTHON_COMMAND} ${PDGENCLI} ${t}/${target}/cli/pd.json -po ${t}/${target}/src -xo ${t}/${target}/cli/xml -xd ${CMAKE_INSTALL_PREFIX}/share/cli/xml -ll ${CMAKE_INSTALL_PREFIX}/lib/${target}pd/${t}
    DEPENDS ${t}/${target}/manifest.json
  )
  add_custom_target(${t}-${target}-gen DEPENDS ${PDDOTC} ${PDCLIDOTC} ${PDRPCDOTTHRIFT} driver)

  if (THRIFT-DRIVER)
    # generate pd thrift cpp and h files
    add_custom_command(OUTPUT ${PDTHRIFT}
      COMMAND ${THRIFT_COMPILER} --gen cpp -o ${t}/${target} -r ${t}/${target}/thrift/p4_pd_rpc.thrift
      COMMAND ${THRIFT_COMPILER} --gen py  -o ${t}/${target} -r ${t}/${target}/thrift/p4_pd_rpc.thrift
      COMMAND mv -f ${t}/${target}/gen-cpp/${t}.h ${t}/${target}/gen-cpp/p4_prefix.h
      COMMAND sed --in-place 's/include \"${t}.h\"/include \"p4_prefix.h\"/' ${t}/${target}/gen-cpp/${t}.cpp
      COMMAND ${PYTHON_COMMAND} ${PDSPLIT} ${t}/${target}/gen-cpp/${t}.cpp ${t}/${target}/gen-cpp 8
      DEPENDS ${t}/${target}/manifest.json ${THRIFT_COMPILER}
    )
    add_custom_target(${t}-${target}-gen-thrift DEPENDS ${t}-${target}-gen ${PDTHRIFT})
  endif()

  # compile libpd.so
  add_library(${t}-${target}-pd SHARED EXCLUDE_FROM_ALL ${PDDOTC})
  target_compile_options(${t}-${target}-pd PRIVATE -w)
  target_include_directories(${t}-${target}-pd PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
  target_include_directories(${t}-${target}-pd PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target})
  set_target_properties(${t}-${target}-pd PROPERTIES
    LIBRARY_OUTPUT_NAME "pd"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib/${target}pd/${t}
  )
  add_dependencies(${t}-${target}-pd ${t}-${target}-gen)

  # compile libpdcli.so
  add_library(${t}-${target}-pdcli SHARED EXCLUDE_FROM_ALL ${PDCLIDOTC})
  target_compile_options(${t}-${target}-pdcli PRIVATE -w)
  target_include_directories(${t}-${target}-pdcli PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
  target_include_directories(${t}-${target}-pdcli PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target})
  set_target_properties(${t}-${target}-pdcli PROPERTIES
    LIBRARY_OUTPUT_NAME "pdcli"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib/${target}pd/${t}
  )
  add_dependencies(${t}-${target}-pdcli ${t}-${target}-gen)

  if (THRIFT-DRIVER)
    # compile libpdthrift.so
    add_library(${t}-${target}-pdthrift SHARED EXCLUDE_FROM_ALL ${PDTHRIFT})
    target_compile_options(${t}-${target}-pdthrift PRIVATE -w)
    target_include_directories(${t}-${target}-pdthrift PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
    target_include_directories(${t}-${target}-pdthrift PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target})
    target_include_directories(${t}-${target}-pdthrift PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/gen-cpp)
    set_target_properties(${t}-${target}-pdthrift PROPERTIES
      LIBRARY_OUTPUT_NAME "pdthrift"
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib/${target}pd/${t}
    )
    add_dependencies(${t}-${target}-pdthrift ${t}-${target}-gen-thrift)
  endif()

  add_custom_target(${t}-${target} DEPENDS ${t}-${target}-pd ${t}-${target}-pdcli)
  if (THRIFT-DRIVER)
    add_dependencies(${t}-${target} ${t}-${target}-pdthrift)
  endif()

  # install bf-rt.json, context.json and tofino.bin
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/ DESTINATION share/${target}pd/${t}
    FILES_MATCHING
    PATTERN "*.json"
    PATTERN "*.bin"
    PATTERN "*manifest*" EXCLUDE
    PATTERN "logs" EXCLUDE
    PATTERN "graphs" EXCLUDE
    PATTERN "*dynhash*" EXCLUDE
    PATTERN "*prim*" EXCLUDE
    PATTERN "*src*" EXCLUDE
    PATTERN "*gen*" EXCLUDE
    PATTERN "*pd*" EXCLUDE
    PATTERN "*cli*" EXCLUDE
    PATTERN "*thrift*" EXCLUDE
  )

  # the pattern matching is required to not error out for programs not built
  # install CLI files
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/cli/xml/ DESTINATION share/cli/xml
    FILES_MATCHING
    PATTERN "*.xml"
  )
  # install python files
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${t}/${target}/gen-py/ DESTINATION ${PYTHON_SITE}/${target}pd/${t}
    FILES_MATCHING
    PATTERN "*.py"
    PATTERN "*remote"
  )
endfunction()

#
# Wrapper function for P4_BUILD_PD_TARGET
#
function(P4_BUILD_PD t location)
  if (TOFINO)
    p4_build_pd_target(${t} ${P4_tofino_ARCHITECTURE} "tofino" ${CMAKE_CURRENT_SOURCE_DIR}/${location}/${t}/${t}.p4)
  endif()
  if (TOFINO2)
    p4_build_pd_target(${t} ${P4_tofino2_ARCHITECTURE} "tofino2" ${CMAKE_CURRENT_SOURCE_DIR}/${location}/${t}/${t}.p4)
  endif()
  if (TOFINO2M)
    p4_build_pd_target(${t} ${P4_tofino2_ARCHITECTURE} "tofino2m" ${CMAKE_CURRENT_SOURCE_DIR}/${location}/${t}/${t}.p4)
  endif()
  if (TOFINO3)
    p4_build_pd_target(${t} ${P4_tofino3_ARCHITECTURE} "tofino3" ${CMAKE_CURRENT_SOURCE_DIR}/${location}/${t}/${t}.p4)
  endif()
  add_custom_target(${t} DEPENDS
    $<$<BOOL:${TOFINO}>:${t}-tofino>
    $<$<BOOL:${TOFINO2}>:${t}-tofino2>
    $<$<BOOL:${TOFINO2M}>:${t}-tofino2m>
    $<$<BOOL:${TOFINO3}>:${t}-tofino3>
  )
endfunction()
