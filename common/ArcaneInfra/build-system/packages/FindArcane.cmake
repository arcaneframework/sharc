#
# Find the ARCANE includes and library
#
# This module uses
# ARCANE_ROOT
#
# This module defines
# ARCANE_FOUND
# ARCANE_INCLUDE_DIRS
# ARCANE_LIBRARIES
#
# Target arcane

# warning after pkg_check_modules ARCANE_VERSION is re-defined to what is written in pc file (not consistent info)

if(NOT ARCANE_ROOT)
  set(ARCANE_ROOT $ENV{ARCANE_ROOT})
endif()

if(ARCANE_ROOT)
  set(_ARCANE_SEARCH_OPTS NO_DEFAULT_PATH)
else()
  set(_ARCANE_SEARCH_OPTS)
endif()

if(NOT ARCANE_FOUND) 

  # on supprime pour un reconfigure
  unset(ARCANE_STD_LIBRARY CACHE)
  unset(ARCANE_MPI_LIBRARY CACHE)
  unset(ARCANE_MESH_LIBRARY CACHE)
  unset(ARCANE_CORE_LIBRARY CACHE)
  unset(ARCANE_IMPL_LIBRARY CACHE)
  unset(ARCANE_UTILS_LIBRARY CACHE)
  unset(ARCANE_INCLUDE_DIR CACHE)

  find_library(ARCANE_STD_LIBRARY
    NAMES arcane_std
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES lib
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_STD_LIBRARY)

  find_library(ARCANE_MPI_LIBRARY
    NAMES arcane_mpi
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES lib
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_MPI_LIBRARY)
  
  find_library(ARCANE_MESH_LIBRARY
    NAMES arcane_mesh
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES lib
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_MESH_LIBRARY)

  find_library(ARCANE_CORE_LIBRARY
    NAMES arcane_core
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES lib
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_CORE_LIBRARY)

  find_library(ARCANE_IMPL_LIBRARY
    NAMES arcane_impl
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES lib
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_IMPL_LIBRARY)

  find_library(ARCANE_UTILS_LIBRARY
    NAMES arcane_utils
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES lib
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_UTILS_LIBRARY)

  # librairies optionnelles 
  
  find_library(ARCANE_THREAD_LIBRARY
    NAMES arcane_thread
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES lib
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_THREAD_LIBRARY)

  find_library(ARCANE_TOTALVIEW_LIBRARY
    NAMES arcane_totalview
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES lib
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_TOTALVIEW_LIBRARY)

  find_path(ARCANE_INCLUDE_DIR arcane_core_config.h
    HINTS ${ARCANE_ROOT} 
		PATH_SUFFIXES include
    ${_ARCANE_SEARCH_OPTS}
    )
  mark_as_advanced(ARCANE_INCLUDE_DIR)
endif()

# pour limiter le mode verbose
set(Arcane_FIND_QUIETLY ON)

find_package_handle_standard_args(Arcane 
	DEFAULT_MSG 
	ARCANE_INCLUDE_DIR 
	ARCANE_UTILS_LIBRARY 
	ARCANE_IMPL_LIBRARY 
	ARCANE_CORE_LIBRARY 
	ARCANE_MESH_LIBRARY
	ARCANE_MPI_LIBRARY
	ARCANE_STD_LIBRARY)

if(ARCANE_FOUND AND NOT TARGET arcane)
   
  set(ARCANE_INCLUDE_DIRS ${ARCANE_INCLUDE_DIR})
  
  set(ARCANE_LIBRARIES ${ARCANE_UTILS_LIBRARY}
                       ${ARCANE_IMPL_LIBRARY}
                       ${ARCANE_CORE_LIBRARY} 
                       ${ARCANE_MESH_LIBRARY}
                       ${ARCANE_MPI_LIBRARY}
                       ${ARCANE_STD_LIBRARY}
  )
   
  find_package(PkgConfig QUIET)

  # il vaudrait mieux utiliser pkg_check_modules aussi pour les libs arcane
  # doc http://www.cmake.org/cmake/help/cmake2.6docs.html#module:FindPkgConfig
  # nouvelle version contenant le bon num�ro de version
  set(ENV{PKG_CONFIG_PATH} ${ARCANE_ROOT}/lib/pkgconfig)
  pkg_check_modules(Arcane arcane QUIET)
  
  # hack pour flexlm car mal detect� et propag� par arcane
  link_directories(${Arcane_LIBRARY_DIRS})
  
  foreach(flag ${Arcane_CFLAGS}) 
    if(${flag} MATCHES "^-D")
      string(REGEX REPLACE "-D" "" f ${flag})
	    list(APPEND ARCANE_FLAGS ${f})
    elseif(${flag} MATCHES "^/D")
      string(REGEX REPLACE "/D" "" f ${flag})
	    list(APPEND ARCANE_FLAGS ${f})
   endif()
  endforeach()

  # arcane core
  
  add_library(arcane_core UNKNOWN IMPORTED)
  
  set_target_properties(arcane_core PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${ARCANE_INCLUDE_DIRS}")
  
  set_target_properties(arcane_core PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ARCANE_CORE_LIBRARY}")
  
  set_target_properties(arcane_core PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "${ARCANE_FLAGS}")
  
  # arcane utils
  
  add_library(arcane_utils UNKNOWN IMPORTED)
  
  set_target_properties(arcane_utils PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${ARCANE_INCLUDE_DIRS}")
  
  set_target_properties(arcane_utils PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ARCANE_UTILS_LIBRARY}")
  
  set_target_properties(arcane_utils PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "${ARCANE_FLAGS}")
  
  # arcane mpi
  
  add_library(arcane_mpi UNKNOWN IMPORTED)
  
  set_target_properties(arcane_mpi PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${ARCANE_INCLUDE_DIRS}")
  
  set_target_properties(arcane_mpi PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ARCANE_MPI_LIBRARY}")
  
  set_target_properties(arcane_mpi PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "${ARCANE_FLAGS}")
  
  # arcane std
  
  add_library(arcane_std UNKNOWN IMPORTED)
  
  set_target_properties(arcane_std PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${ARCANE_INCLUDE_DIRS}")
  
  set_target_properties(arcane_std PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ARCANE_STD_LIBRARY}")
  
  set_target_properties(arcane_std PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "${ARCANE_FLAGS}")
  
  # arcane mesh
  
  add_library(arcane_mesh UNKNOWN IMPORTED)
  
  set_target_properties(arcane_mesh PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${ARCANE_INCLUDE_DIRS}")
  
  set_target_properties(arcane_mesh PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ARCANE_MESH_LIBRARY}")
  
  set_target_properties(arcane_mesh PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "${ARCANE_FLAGS}")
  
  # arcane impl
  
  add_library(arcane_impl UNKNOWN IMPORTED)
  
  set_target_properties(arcane_impl PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${ARCANE_INCLUDE_DIRS}")
  
  set_target_properties(arcane_impl PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ARCANE_IMPL_LIBRARY}")
  
  set_target_properties(arcane_impl PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "${ARCANE_FLAGS}")
  
  # arcane
  
  add_library(arcane INTERFACE IMPORTED)
  
  set_property(TARGET arcane APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "arcane_utils")

  set_property(TARGET arcane APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "arcane_core")

  set_property(TARGET arcane APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "arcane_mesh")

  set_property(TARGET arcane APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "arcane_mpi")

  set_property(TARGET arcane APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "arcane_std")

  set_property(TARGET arcane APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "arcane_impl")
 
  if(ARCANE_THREAD_LIBRARY)
    add_library(arcane_thread UNKNOWN IMPORTED)
    
    set_target_properties(arcane_thread PROPERTIES 
      INTERFACE_INCLUDE_DIRECTORIES "${ARCANE_INCLUDE_DIRS}")
    
    set_target_properties(arcane_thread PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${ARCANE_THREAD_LIBRARY}")
    
    set_target_properties(arcane_thread PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS "${ARCANE_FLAGS}")

    set_property(TARGET arcane APPEND PROPERTY 
      INTERFACE_LINK_LIBRARIES "arcane_thread")

    list(APPEND ARCANE_LIBRARIES ${ARCANE_THREAD_LIBRARY})
  
  endif()
  
  if(ARCANE_TOTALVIEW_LIBRARY)
    
    add_library(arcane_totalview UNKNOWN IMPORTED)
    
    set_target_properties(arcane_totalview PROPERTIES 
      INTERFACE_INCLUDE_DIRECTORIES "${ARCANE_INCLUDE_DIRS}")
    
    set_target_properties(arcane_totalview PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${ARCANE_TOTALVIEW_LIBRARY}")
    
    set_target_properties(arcane_totalview PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS "${ARCANE_FLAGS}")

    set_property(TARGET arcane APPEND PROPERTY 
      INTERFACE_LINK_LIBRARIES "arcane_totalview")

    list(APPEND ARCANE_LIBRARIES ${ARCANE_TOTALVIEW_LIBRARY})
  
  endif()
   
  get_property(LIBRARIES TARGET arcane PROPERTY 
      INTERFACE_LINK_LIBRARIES)

  importPackageXmlFile(TARGET arcane XML ${ARCANE_ROOT}/lib/pkglist.xml)
  
  set(EXTRA_DLLS_TO_COPY 
    ${EXTRA_DLLS_TO_COPY}
	${ARCANE_ROOT}/lib/Arcane.Core.dll
	${ARCANE_ROOT}/lib/Arcane.Compiler.Directives.dll
	${ARCANE_ROOT}/lib/Arcane.Utils.dll
	${ARCANE_ROOT}/lib/arcane_dotnet_wrapper.dll
	CACHE INTERNAL "Extra dlls")
endif()
