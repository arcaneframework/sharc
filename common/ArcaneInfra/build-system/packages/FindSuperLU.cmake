#
# Find the SUPERLU includes and library
#
# This module uses
# SUPERLU_ROOT
#
# This module defines
# SUPERLU_FOUND
# SUPERLU_INCLUDE_DIRS
# SUPERLU_LIBRARIES
#
# Target superlu 

if(NOT SUPERLU_ROOT)
  set(SUPERLU_ROOT $ENV{SUPERLU_ROOT})
endif()

if(SUPERLU_ROOT)
  set(_SUPERLU_SEARCH_OPTS NO_DEFAULT_PATH)
else()
  set(_SUPERLU_SEARCH_OPTS)
endif()

if(NOT SUPERLU_FOUND)

  find_library(SUPERLU_LIBRARY
    NAMES superlu_3.1 superlu_4.0 superlu
    HINTS ${SUPERLU_ROOT} 
    PATH_SUFFIXES lib
    ${_SUPERLU_SEARCH_OPTS}
    )
  mark_as_advanced(SUPERLU_LIBRARY)

  find_path(SUPERLU_INCLUDE_DIR slu_cdefs.h
    HINTS ${SUPERLU_ROOT} 
    PATH_SUFFIXES include
    ${_SUPERLU_SEARCH_OPTS}
    )
  mark_as_advanced(SUPERLU_INCLUDE_DIR)
  
endif()

# pour limiter le mode verbose
set(SUPERLU_FIND_QUIETLY ON)

find_package_handle_standard_args(SUPERLU
  DEFAULT_MSG 
  SUPERLU_INCLUDE_DIR
  SUPERLU_LIBRARY
  )

if(SUPERLU_FOUND AND NOT TARGET superlu)

  set(SUPERLU_INCLUDE_DIRS ${SUPERLU_INCLUDE_DIR})
  
  set(SUPERLU_LIBRARIES ${SUPERLU_LIBRARY})

  add_library(superlu SHARED IMPORTED)
  
  set_target_properties(superlu PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${SUPERLU_INCLUDE_DIRS}")
    
  set_target_properties(superlu PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${SUPERLU_LIBRARY}"
    IMPORTED_NO_SONAME ON)
  
endif()

