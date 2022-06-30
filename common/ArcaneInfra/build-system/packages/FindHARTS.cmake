#
# Find the HARTS includes and library
#
# This module uses
# HARTS_ROOT
#
# This module defines
# HARTS_FOUND
# HARTS_INCLUDE_DIRS
# HARTS_LIBRARIES
#
# Target harts

if(NOT HARTS_ROOT)
  set(HARTS_ROOT $ENV{HARTS_ROOT})
endif()

if(HARTS_ROOT)
  set(_HARTS_SEARCH_OPTS NO_DEFAULT_PATH)
else()
  set(_HARTS_SEARCH_OPTS)
endif()

if(NOT HARTS_FOUND) 

  find_library(HARTS_LIBRARY 
		NAMES HARTS 
		HINTS ${HARTS_ROOT}
		PATH_SUFFIXES lib
		${_HARTS_SEARCH_OPTS}
    )
  mark_as_advanced(HARTS_LIBRARY)

  find_library(HARTS_RTS_LIBRARY 
		NAMES HARTS HARTSRuntimeSys
		HINTS ${HARTS_ROOT}
		PATH_SUFFIXES lib
		${_HARTS_SEARCH_OPTS}
    )
  mark_as_advanced(HARTS_RTS_LIBRARY)
  
  find_library(HARTS_UTILS_LIBRARY 
		NAMES HARTSUtils
		HINTS ${HARTS_ROOT}
		PATH_SUFFIXES lib
		${_HARTS_SEARCH_OPTS}
    )
  mark_as_advanced(HARTS_UTILS_LIBRARY)
  
  find_path(HARTS_INCLUDE_DIR HARTS/HARTS.h
    HINTS ${HARTS_ROOT} 
		PATH_SUFFIXES include
    ${_HARTS_SEARCH_OPTS}
    )
  mark_as_advanced(HARTS_INCLUDE_DIR)
  
endif()

# pour limiter le mode verbose
set(HARTS_FIND_QUIETLY ON)

find_package_handle_standard_args(HARTS 
	DEFAULT_MSG 
	HARTS_INCLUDE_DIR 
	HARTS_LIBRARY)
if(HARTS_FOUND)
set(HARTS_GIT_VERSION TRUE)
else(HARTS_FOUND)
find_package_handle_standard_args(HARTS 
        DEFAULT_MSG 
        HARTS_INCLUDE_DIR 
        HARTS_RTS_LIBRARY
        HARTS_UTILS_LIBRARY)
endif()

if(HARTS_FOUND AND NOT TARGET harts)
   
  set(HARTS_INCLUDE_DIRS ${HARTS_INCLUDE_DIR})
  
   
  # harts main
  add_library(harts_main UNKNOWN IMPORTED)
	 
  set_target_properties(harts_main PROPERTIES 
	  INTERFACE_INCLUDE_DIRECTORIES "${HARTS_INCLUDE_DIRS}")
    
    
  if(HARTS_GIT_VERSION)
	set(HARTS_LIBRARIES ${HARTS_LIBRARY})

	set_target_properties(harts_main PROPERTIES
			    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
			    IMPORTED_LOCATION "${HARTS_LIBRARY}")
	# harts
	add_library(harts INTERFACE IMPORTED)

	set_property(TARGET harts APPEND PROPERTY 
 			INTERFACE_LINK_LIBRARIES "harts_main")
  else(HARTS_GIT_VERSION)
	set(HARTS_LIBRARIES 
                      ${HARTS_RTS_LIBRARY}
                      ${HARTS_UTILS_LIBRARY})
	# harts rts
	add_library(harts_rts UNKNOWN IMPORTED)
	set_target_properties(harts_rts PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
				IMPORTED_LOCATION "${HARTS_RTS_LIBRARY}")

	# harts utils
	add_library(harts_utils UNKNOWN IMPORTED)
	set_target_properties(harts_utils PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    				IMPORTED_LOCATION "${HARTS_UTILS_LIBRARY}")
	set_property(TARGET harts APPEND PROPERTY 
			    INTERFACE_LINK_LIBRARIES "harts_rts")

	set_property(TARGET harts APPEND PROPERTY 
			    INTERFACE_LINK_LIBRARIES "harts_utils")
    
  endif()
	 

  
endif()
