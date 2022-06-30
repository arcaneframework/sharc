#
# Find MKL libraries
#
# This module defines
# BLAS_LIBRARIES, the libraries to link against to use blas.
# BLAS_FOUND If false, do not try to use blas.
 

 
foreach(_lib hwloc)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${HWLOC_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(LIB_SUB_${_lib})
    set(HWLOC_LIBRARY ${HWLOC_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(HWLOC_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
endforeach(_lib)

find_path(HWLOC_INCLUDE_DIR hwloc.h
 PATHS 
 ${HWLOC_INCLUDE_PATH}
 NO_DEFAULT_PATH)

set(HWLOC_FOUND "NO")
if(HWLOC_LIBRARY)
  set(HWLOC_FOUND "YES")
  set(HWLOC_LIBRARIES ${HWLOC_LIBRARY})
  set(HWLOC_INCLUDE_DIRS ${HWLOC_INCLUDE_DIR})
endif(HWLOC_LIBRARY)

list(APPEND REQUIRED_DEPENDENCIES HWLOC)


