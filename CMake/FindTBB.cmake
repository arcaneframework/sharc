#
# Find the TBB (Intel Thread Building blocks) includes and library
#
# This module defines
# TBB_INCLUDE_DIR, where to find headers,
# TBB_LIBRARIES, the libraries to link against to use Papi.
# TBB_FOUND, If false, do not try to use Papi.

message(status "TBB :${TBB_INCLUDE_PATH} ${TBB_LIBRARY_PATH}")
find_path(TBB_INCLUDE_DIR tbb/tbb_thread.h
  PATHS ${TBB_INCLUDE_PATH} NO_DEFAULT_PATH)

find_library(TBB_LIBRARY tbb
  PATHS ${TBB_LIBRARY_PATH} NO_DEFAULT_PATH)


set(TBBR_FOUND "NO")
if(TBB_INCLUDE_DIR AND TBB_LIBRARY)
  set(TBBR_FOUND "YES")
  set(TBBR_LIBRARIES ${TBB_LIBRARY})
  set(TBBR_INCLUDE_DIRS ${TBB_INCLUDE_DIR})
endif(TBB_INCLUDE_DIR AND TBB_LIBRARY)
message(status "TBB : ${TBB_INCLUDE_PATH} ${TBB_LIBRARY_PATH} FOUND : ${TBBR_FOUND}")


list(APPEND REQUIRED_DEPENDENCIES TBBR)
