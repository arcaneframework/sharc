#
# Find NVAMG libraries
#
# This module defines
# NVAMG_LIBRARIES, the libraries to link against to use blas.
# NVAMG_FOUND If false, do not try to use blas.
# Compilation avec gcc 4.6.2 et openmpi

#foreach(_lib amgxsh)
find_library(NVAMG_LIBRARY amgxsh 
  PATHS ${NVAMG_LIBRARY_PATH} NO_DEFAULT_PATH)
#  if(LIB_SUB_${_lib})
#    set(NVAMG_LIBRARY $NVAMG_LIBRARY} ${LIB_SUB_${_lib}})
#  else(LIB_SUB_${_lib})
#    set(NVAMG_LIBRARY_FAILED "YES")
#  endif(LIB_SUB_${_lib})
#endforeach(_lib)

find_path(NVAMG_INCLUDE_DIR amgx_capi.h
  PATHS ${NVAMG_INCLUDE_PATH} NO_DEFAULT_PATH)

set(NVAMG_FOUND "NO")
if(NVAMG_LIBRARY)
  set(NVAMG_FOUND "YES")
  set(NVAMG_LIBRARIES ${NVAMG_LIBRARY})
  set(NVAMG_INCLUDE_DIRS ${NVAMG_INCLUDE_DIR})
  set(NVAMG_FLAGS ${NVAMG_FLAGS} USE_NVAMG )
endif(NVAMG_LIBRARY)
message(status "--> NvAMG : ${NVAMG_INCLUDE_PATH} ${NVAMG_LIBRARY_PATH} FOUND : ${NVAMG_FOUND}") 
list(APPEND REQUIRED_DEPENDENCIES NVAMG)
