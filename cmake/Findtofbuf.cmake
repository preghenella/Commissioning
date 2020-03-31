include(FindPackageHandleStandardArgs)

find_path(TOFBUF_INCLUDE_DIR NAMES "tofbuf.h" PATHS "/SOFT/SC/include")
message(STATUS "tofbuf.h => ${TOFBUF_INCLUDE_DIR}")

find_file(TOFBUF_SOURCE "libtofbuf.c" PATHS "/SOFT/SC/buffers")
message(STATUS "libtofbuf.c => ${TOFBUF_SOURCE}")

find_package_handle_standard_args(TOFBUF
  DEFAULT_MSG
  TOFBUF_INCLUDE_DIR
  TOFBUF_SOURCE
)

mark_as_advanced(TOFBUF_SOURCE TOFBUF_INCLUDE_DIR)

if(TOFBUF_FOUND)
  set(TOFBUF_SOURCES      ${TOFBUF_SOURCE})
  set(TOFBUF_INCLUDE_DIRS ${TOFBUF_INCLUDE_DIR})
endif()
