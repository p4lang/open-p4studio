project(liblld VERSION 0.1 LANGUAGES C)
include_directories(../../include/lld)
add_library(lld_o OBJECT
bf_lld_if.c
bf_int_if.c
bf_dma_if.c
lld_i2c_if.c
lld_gpio_if.c
lld.c
lld_csr2jtag.c
lld_csr2jtag.h
lld_debug.c
lld_dev.c
lld_dev_lock.c
lld_dev_tof.c
lld_dev_tof2.c
lld_dev_tof3.c

lld_dr.c
lld_dr_tof2.c
lld_dr_tof3.c

lld_dr_tof.c
lld_dr_if.c
lld_dr_regs.c
lld_dr_regs_tof.c
lld_dr_regs_tof2.c
lld_dr_regs_tof3.c

lld_efuse.c
lld_efuse_tof.c
lld_efuse_tof2.c
lld_efuse_tof3.c

lld_fault_hdlr.c
lld_ind_reg_if.c
lld_ind_reg_if_tof.c
lld_ind_reg_if_tof2.c
lld_ind_reg_if_tof3.c



lld_tof2_interrupt_struct.h
lld_tof2_interrupt.h
lld_tof3_interrupt_struct.h
lld_tof3_interrupt.h
lld_tof_interrupt.h
lld_tof2_interrupt.c
lld_tof3_interrupt.c
lld_tof_interrupt.c
lld_interrupt.c
lldlib_int.h
lldlib_log.h
lld_map.c
lld_mdio_if.c
lld_mdio_tof3.c
lld_reg_if.c
lld_subdev_reg_if.c
lld_reg_parse.c
lld_sku.c
lld_spi_if.c
#lld_tof2_addr_conversion.h
#lld_tof_addr_conversion.h
#local_tofino.h
comira_dbg_access_autogen.h
comira_dbg_info_autogen.h
lldlib_ucli.c
lldgpio_ucli.c
lld_diag_ext.c
lld_tof2_eos.c
lld_tof3_eos.c
lld_tof2_eos_tm.c
lld_tof3_eos_tm.c
lld_tof2_hole_test.c
reg_list_autogen.h
bf_ts_if.c
lld_tof_ts.c
lld_tof2_ts.c
lld_tof3_ts.c
)
add_library(lld SHARED EXCLUDE_FROM_ALL $<TARGET_OBJECTS:lld_o>)

if(BF-PYTHON)
  add_library(bfshell_plugin_debug_o OBJECT cli/debug_cli.c)
  target_include_directories(bfshell_plugin_debug_o PUBLIC ${Python3_INCLUDE_DIRS})
  add_library(bfshell_plugin_debug SHARED $<TARGET_OBJECTS:bfshell_plugin_debug_o>)
  target_link_libraries(bfshell_plugin_debug ${Python3_LIBRARIES})
  SET_TARGET_PROPERTIES(bfshell_plugin_debug PROPERTIES PREFIX "")
  install(FILES cli/xml/debug.xml DESTINATION share/cli/xml)
endif()

# Building LLD doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(srcdir ${CMAKE_CURRENT_SOURCE_DIR}/docs)
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)
  add_custom_target(doc-driver-lld
                    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating LLD Doxygen"
                    VERBATIM)
  set_target_properties(doc-driver-lld PROPERTIES EXCLUDE_FROM_ALL TRUE)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION
    share/doc/bf-drivers/lld OPTIONAL)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/latex DESTINATION
    share/doc/bf-drivers/lld OPTIONAL)
else()
  message(WARNING "Doxygen is needed to build the documentation. Please install doxygen.")
endif()
