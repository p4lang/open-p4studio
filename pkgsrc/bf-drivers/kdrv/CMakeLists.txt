set(CMAKE_VERBOSE_MAKEFILE ON)

project(kdrv VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find the kernel release
execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNELRELEASE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT KDIR)
  execute_process(
    COMMAND readlink -f /lib/modules/${KERNELRELEASE}/build
    OUTPUT_VARIABLE KERNELDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(KERNELDIR ${KDIR})
endif()

message(STATUS "Kernel release (uname -r): ${KERNELRELEASE}")
message(STATUS "Kernel headers: ${KERNELDIR}")

# Ubuntu 24.04 kernels (and newer upstream versions) are not fully supported
# by these out-of-tree modules yet.
string(REGEX MATCH "^[0-9]+\\.[0-9]+" KERNEL_VERSION_SHORT "${KERNELRELEASE}")
set(KDRV_MODULES_SUPPORTED ON)
if(KERNEL_VERSION_SHORT VERSION_GREATER_EQUAL "6.8")
  set(KDRV_MODULES_SUPPORTED OFF)
  message(WARNING
    "Skipping bf kernel module build on kernel ${KERNELRELEASE}. "
    "Kernel module support is currently limited to kernels older than 6.8.")
endif()

if(KDRV_MODULES_SUPPORTED)
  add_subdirectory(bf_kdrv)
  add_subdirectory(bf_knet)
  add_subdirectory(bf_kpkt)
  add_dependencies(bf_kpkt bf_kdrv)
endif()

install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/bf_kdrv_mod_load DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/bf_kdrv_mod_unload DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/bf_knet_mod_load DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/bf_knet_mod_unload DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/bf_kpkt_mod_load DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/bf_kpkt_mod_unload DESTINATION bin)
