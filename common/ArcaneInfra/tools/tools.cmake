#------------------------------------------------------------------------------#
# AxlCompiler arguments
#------------------------------------------------------------------------------#

if(NOT WIN32)
  if(${CMAKE_HOST_UNIX} STREQUAL "1")
    list(APPEND AXL_ARGS "--verbose=$(VERBOSE)")
  endif(${CMAKE_HOST_UNIX} STREQUAL "1")
endif(NOT WIN32)

#------------------------------------------------------------------------------#
# MACRO __exec_clr
# macro permettant l'execution d'un binaire C# via mono  
#------------------------------------------------------------------------------#

macro(__exec_clr exe_name)

if(WIN32)
  exec_program(${exe_name}
               ARGS ${ARGN}
               # OUTPUT_VARIABLE log
               RETURN_VALUE ret)
else(WIN32)
  exec_program(${MONO_EXECUTABLE} 
               ARGS ${exe_name} ${ARGN}
               # OUTPUT_VARIABLE log
               RETURN_VALUE ret)
endif(WIN32)
  
  if(NOT ret STREQUAL "0")
    MESSAGE(FATAL_ERROR "Command failure: check error message above")
  endif(NOT ret STREQUAL "0")
endmacro(__exec_clr)

#------------------------------------------------------------------------------#
# MACRO __include_executable
#------------------------------------------------------------------------------#
#
#------------------------------------------------------------------------------#

macro(__include_executable)

  include_directories(${ARCANE_INCLUDE_DIRS})

  add_executable(${APPLICATION} ${CMAKE_SOURCE_DIR}/src/Main/main)

  set(EXTRA_LIBRARIES)
  list(APPEND EXTRA_LIBRARIES ${IFPSOLVER_TARGET_LIBRARIES})
  list(APPEND EXTRA_LIBRARIES ${EXTRA_TARGET_LIBS})
  
  if(${BUILD_SHARED_LIBS})
    set(MYLIBS ${APPLICATION_LIBRARIES})
  else(${BUILD_SHARED_LIBS})
    if(WIN32)
      set(MYLIBS ${APPLICATION_LIBRARIES})
    else(WIN32)
      set(MYLIBS -Wl,-whole-archive ${APPLICATION_LIBRARIES} -Wl,-no-whole-archive)
    endif(WIN32)
  endif(${BUILD_SHARED_LIBS})
  
#  SET_TARGET_PROPERTIES(${APPLICATION}
#    PROPERTIES LINK_DIRECTORIES

#  LINK_DIRECTORIES(
#    ${ARCANE_LIBRARY_DIRS}
#  )

  target_link_libraries(${APPLICATION}
    ${MYLIBS}
    ${EXTRA_LIBRARIES}
    ${ARCANE_LIBRARIES} # patch temporaire pour ATM201
   )

  install(TARGETS ${APPLICATION} DESTINATION bin)
  
  if(WIN32)
  	message(STATUS "PATCH WIN32 : GENERATION DU SCRIPT DE PARSING DU FICHIER VCPROJ")  
	  set(PATCH_STR "")
  	set(PATCH_STR "${PATCH_STR}@echo off\n")
  	set(PATCH_STR "${PATCH_STR}setlocal enabledelayedexpansion\n")
  	set(PATCH_STR "${PATCH_STR}call ${WHOLEARCHIVE_VCPROJ_TOOL} --visual=\"${VISUAL_STUDIO_NAME}\" --path=${CMAKE_BINARY_DIR} --project=${APPLICATION}\n")
  	set(PATCH_STR "${PATCH_STR}exit /b 0\n")
  	file(WRITE "${CMAKE_BINARY_DIR}/${WIN32_PATCH_FILE}" ${PATCH_STR})
  endif(WIN32)
  
endmacro(__include_executable)

#------------------------------------------------------------------------------#
# MACRO __include_subdirectories
#------------------------------------------------------------------------------#
#
#------------------------------------------------------------------------------#

#-- Générateurs C#
set(XML_TO_CMAKE_PARSER ${XML2CMAKE_COMPILER})

if(WIN32)
  set(WHOLEARCHIVE_VCPROJ_TOOL
    ${ARCANE_TOOLS_DIR}/ArcaneInfra.Tools/WholeArchiveVCProj/bin/Release/WholeArchiveVCProj.exe)
endif(WIN32)

if(WIN32)
  set(WINDOWS_PATH_RESOLVER_TOOL
    ${ARCANE_TOOLS_DIR}/ArcaneInfra.Tools/WindowsPathResolver/bin/Debug/WindowsPathResolver.exe)
endif(WIN32)

macro(__include_subdirectories libraries)
  set(XML_TO_CMAKE_PARSER_OPTIONS "--axldb --eclipse-config")
  if (VERBOSE STREQUAL "TRUE")
    set(XML_TO_CMAKE_PARSER_OPTIONS "${XML_TO_CMAKE_PARSER_OPTIONS} --verbose")
  endif (VERBOSE STREQUAL "TRUE")

  # Mode dependance à faire...
  __exec_clr(${XML_TO_CMAKE_PARSER} ${XML_TO_CMAKE_PARSER_OPTIONS}
    ${CMAKE_SOURCE_DIR}/${libraries}
    \"${XML_PACKAGE_LIST}\"
    \"${ARCANE_XML_PACKAGE_LIST}\"
    ${CMAKE_BINARY_DIR}/${libraries}.cmake
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${ARGN})
  
  include(${CMAKE_BINARY_DIR}/${libraries}.cmake)

endmacro(__include_subdirectories)

#------------------------------------------------------------------------------#
# FUNCTION __generate_package_list
#------------------------------------------------------------------------------#
#
#------------------------------------------------------------------------------#

#-- Fichier XML généré de la liste des paquetages dispos
set(XML_PACKAGE_LIST ${CMAKE_BINARY_DIR}/pkglist.xml)

function(__generate_package_info package)
  set(buffer "")
  string(TOLOWER ${package} lower_package)
  set(_Available "false")
  if(${package}_FOUND AND NOT ${package}_DISABLED)
    set(_Available "true")
  endif()
  set(buffer "${buffer} <package name='${lower_package}' available='${_Available}'>\n")
  if (_Available)
    foreach(lib ${${package}_LIBRARIES})
      set(buffer "${buffer}   <lib-name>${lib}</lib-name>\n")
    endforeach(lib)
    foreach(inc ${${package}_INCLUDE_DIRS})
      set(buffer "${buffer}   <include>${inc}</include>\n")
    endforeach(inc)
    foreach(exec ${${package}_EXEC_PATH})
      set(buffer "${buffer}   <bin-path>${exec}</bin-path>\n")
    endforeach(exec)
    foreach(flags ${${package}_FLAGS})
      set(buffer "${buffer}   <flags>${flags}</flags>\n")
    endforeach(flags)
  endif()
  set(buffer "${buffer}  </package>\n\n")
  set(PKG_STR "${PKG_STR} ${buffer}" PARENT_SCOPE)
endfunction(__generate_package_info package)

function(__generate_package_list)
  set(PKG_STR "${PKG_STR}<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n\n")
  set(PKG_STR "${PKG_STR}<!-- Generated file DO NOT EDIT -->\n\n")
  set(PKG_STR "${PKG_STR}<packages>\n\n")
  
  foreach(package ${PACKAGE_LIST})
    __generate_package_info(${package})
  endforeach(package ${PACKAGE_LIST})
  set(PKG_STR "${PKG_STR}</packages>\n")

  file(WRITE ${XML_PACKAGE_LIST} ${PKG_STR})
  
endfunction(__generate_package_list)

#------------------------------------------------------------------------------#
# MACRO __load_arcane_package_list
#------------------------------------------------------------------------------#
#
#------------------------------------------------------------------------------#

set(ARCANE_PACKAGES_LOADER ${PKGLIST_COMPILER})

macro(__load_arcane_package_list)
  if (ARCANE_FOUND) # sinon l'erreur sera gérée plus loin dans CMake.cmake

    MESSAGE(STATUS "---------------------------------------------------")
    message(STATUS "Loading Arcane delegated libraries")
    MESSAGE(STATUS "---------------------------------------------------")

    set(ARCANE_PACKAGES_LOADER_OPTIONS)
    if (VERBOSE STREQUAL "TRUE")
      set(ARCANE_PACKAGES_LOADER_OPTIONS "${ARCANE_PACKAGES_LOADER_OPTIONS} --verbose")
    endif (VERBOSE STREQUAL "TRUE")

    __exec_clr(${ARCANE_PACKAGES_LOADER} ${ARCANE_PACKAGES_LOADER_OPTIONS}
      \"${ARCANE_XML_PACKAGE_LIST}\"
      ${CMAKE_BINARY_DIR}/delegated_arcane_dependencies.cmake
      # OUTPUT_VARIABLE ARCANE_PACKAGES_LOADER_OUTPUT
      )
  
    include(${CMAKE_BINARY_DIR}/delegated_arcane_dependencies.cmake)

  endif(ARCANE_FOUND)

endmacro(__load_arcane_package_list)
