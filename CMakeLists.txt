 cmake_minimum_required(VERSION 3.13)

project(ShArc VERSION 2026.1.1.0)


if (NOT ARCGEOSIM_FRAMEWORK_ROOT)
  if (DEFINED ENV{ARCGEOSIM_FRAMEWORK_ROOT})
    set(ARCGEOSIM_FRAMEWORK_ROOT $ENV{ARCGEOSIM_FRAMEWORK_ROOT})
    set(ARCGEOSIM_FRAMEWORK_BINARY_DIR ${PROJECT_BINARY_DIR})
    set(ARCGEOSIM_EXTERNAL TRUE)
  else()
    set(ARCGEOSIM_FRAMEWORK_ROOT ${PROJECT_SOURCE_DIR}/common)
    set(ARCGEOSIM_FRAMEWORK_BINARY_DIR ${PROJECT_BINARY_DIR}/common)
    set(ARCGEOSIM_EXTERNAL FALSE)
  endif ()
else()
  set(ARCGEOSIM_FRAMEWORK_BINARY_DIR ${PROJECT_BINARY_DIR})
endif ()

# check submodule is loaded
if (NOT EXISTS ${ARCGEOSIM_FRAMEWORK_ROOT}/ArcaneInfra/build-system/arcgeosim/LoadArcGeoSim.cmake)
    message(FATAL_ERROR "build-system not found, Var ARCGEOSIM_FRAMEWORK_ROOT= ${ARCGEOSIM_FRAMEWORK_ROOT} may be not set"
                         "\n                     build-system submodule need to initialized by running  the following command: "
                         "\n   cd ${CMAKE_CURRENT_SOURCE_DIR}"
                         "\n   git submodule update --init"
           )
endif ()

list(APPEND CMAKE_MODULE_PATH ${ARCGEOSIM_FRAMEWORK_ROOT}/ArcaneInfra/build-system/arcgeosim)

set(USE_LANGUAGE_GUMP ON)
set(USE_LANGUAGE_LAW ON)

# chargement du systeme de compilation
include(LoadArcGeoSim)

# ici on met les packages, langages, metas, options...

# activation des strong options (uniquement avec le nouveau systeme de compilation)
loadMeta(NAME strong_options)
loadMeta(NAME alien20)
loadMeta(NAME arcane3)
add_definitions(-DUSE_ARCANE_V3)

# affichage des informations
include(PrintArcGeoSimInformations)


set(CMAKE_INSTALL_LIBDIR "lib")
include(GNUInstallDirs)
message(STATUS "INSTALL_LIBDIR = ${CMAKE_INSTALL_LIBDIR}")

logStatus("Loading ArcGeoSim components...")

logStatus("Loading ArcGeoSim components...")

if(ARCGEOSIM_EXTERNAL)
loadPackage(NAME ArcGeoSim)
endif()
if(ArcGeoSim_FOUND)
  message(status "TARGET ARCGEOSIM_FULL FOUND")
else()
  message(status "COMPILE ARCGEOSIM TARGET")
  if (NOT ARCGEOSIM_EXPORT_TARGET)
    set(ARCGEOSIM_EXPORT_TARGET ${PROJECT_NAME}Targets)
  endif()
  set(REQUIRE_INSTALL_PROJECTTARGETS TRUE)
  set(BUILDSYSTEM_INSTALL_TARGETS TRUE)
  set(BUILDSYSTEM_INSTALL_EXPORT TRUE)
  set(common/ArcGeoSim/src/ArcGeoSim/Mesh/AccessorToolsMng ON)
  add_subdirectory(common/ArcGeoSim)
endif()

logStatus("Loading ShArc components...")

add_subdirectory(src)

logStatus("Loading ShArc.exe...")

add_subdirectory(src/Main)

logStatus("Loading tests...")

add_subdirectory(test)

# finalisation du système de compilation
include(FinishArcGeoSim)
