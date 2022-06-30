#
# Find the SUPERLU_DIST includes and library
#
# This module uses
# SUPERLU_DIST_ROOT
#
# This module defines
# SUPERLU_DIST_FOUND
# SUPERLU_DIST_INCLUDE_DIRS
# SUPERLU_DIST_LIBRARIES
#
# Target superlu 

if(NOT SUPERLU_DIST_ROOT)
  set(SUPERLU_DIST_ROOT $ENV{SUPERLU_DIST_ROOT})
endif()

if(SUPERLU_DIST_ROOT)
  set(_SUPERLU_DIST_SEARCH_OPTS NO_DEFAULT_PATH)
else()
  set(_SUPERLU_DIST_SEARCH_OPTS)
endif()

if(NOT SUPERLU_DIST_FOUND)

  find_library(SUPERLU_DIST_LIBRARY
    NAMES superlu_dist_2.4 superlu_dist
    HINTS ${SUPERLU_DIST_ROOT} 
    PATH_SUFFIXES lib
    ${_SUPERLU_DIST_SEARCH_OPTS}
    )
  mark_as_advanced(SUPERLU_DIST_LIBRARY)

  find_path(SUPERLU_DIST_INCLUDE_DIR superlu_defs.h
    HINTS ${SUPERLU_DIST_ROOT} 
    PATH_SUFFIXES include
    ${_SUPERLU_DIST_SEARCH_OPTS}
    )
  mark_as_advanced(SUPERLU_DIST_INCLUDE_DIR)
  
endif()

# pour limiter le mode verbose
set(SUPERLU_DIST_FIND_QUIETLY ON)

find_package_handle_standard_args(SUPERLU_DIST
  DEFAULT_MSG 
  SUPERLU_DIST_INCLUDE_DIR
  SUPERLU_DIST_LIBRARY
  )

if(SUPERLU_DIST_FOUND AND NOT TARGET superludist)

  set(SUPERLU_DIST_INCLUDE_DIRS ${SUPERLU_DIST_INCLUDE_DIR})
  
  set(SUPERLU_DIST_LIBRARIES ${SUPERLU_DIST_LIBRARY})

  add_library(superludist UNKNOWN IMPORTED)
  
  set_target_properties(superludist PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${SUPERLU_DIST_INCLUDE_DIRS}")
    
  set_target_properties(superludist PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${SUPERLU_DIST_LIBRARY}")
  
endif()

