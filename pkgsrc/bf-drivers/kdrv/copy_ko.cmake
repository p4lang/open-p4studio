if(EXISTS "${SRC_BUILD}")
  set(SRC "${SRC_BUILD}")
elseif(EXISTS "${SRC_SOURCE}")
  set(SRC "${SRC_SOURCE}")
else()
  message(FATAL_ERROR "Kernel module not found. Checked '${SRC_BUILD}' and '${SRC_SOURCE}'")
endif()

file(COPY_FILE "${SRC}" "${DST}" ONLY_IF_DIFFERENT)
