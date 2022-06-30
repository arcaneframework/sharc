#
# Find the ALIEN includes and library
#
# This module uses
# ALIEN_ROOT
#
# This module defines
# ALIEN_FOUND
# ALIEN_INCLUDE_DIRS
# ALIEN_LIBRARIES
#
# Target alien

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.1")
  cmake_policy(SET CMP0074 OLD)
endif()

if(NOT ALIEN_ROOT)
  set(ALIEN_ROOT $ENV{ALIEN_ROOT})
endif()

message(STATUS "ALIEN_ROOT: ${ALIEN_ROOT}")
message(STATUS "ALIEN_FOUND: ${ALIEN_FOUND}")

if(NOT ALIEN_FOUND)

  set(ALIEN_DIR ${ALIEN_ROOT}/lib/cmake)

  find_package(ALIEN)

endif()

message(STATUS "ALIEN_FOUND: ${ALIEN_FOUND}")

if(TARGET Alien::alien_core)
  message(STATUS "alien_core found")
else()
  message(STATUS "alien_core not found")
endif()

if(ALIEN_FOUND)

  set(type ${CMAKE_BUILD_TYPE})


  message(STATUS "type: ${type}")
  get_target_property(ALIEN_CORE_LIBRARY Alien::alien_core IMPORTED_LOCATION_${type})
  get_target_property(ALIEN_REFSEMANTIC_LIBRARY Alien::alien_semantic_ref IMPORTED_LOCATION_${type})
  get_target_property(ALIEN_EXTERNALPACKAGES_LIBRARY alien_external_packages IMPORTED_LOCATION_${type})
  get_target_property(ALIEN_IFPEN_LIBRARY alien_ifpen_solvers IMPORTED_LOCATION_${type})
  if(TARGET alien_trilinos)
    get_target_property(ALIEN_TRILINOS_LIBRARY alien_trilinos IMPORTED_LOCATION_${type})
  endif()
  if(TARGET alien_hpddm)
    get_target_property(ALIEN_HPDDM_LIBRARY alien_hpddm IMPORTED_LOCATION_${type})
  endif()
  get_target_property(ALIEN_ARCANETOOLS_LIBRARY alien_arcane_tools IMPORTED_LOCATION_${type})
  message(STATUS "ALIEN_CORE_LIBRARY: ${ALIEN_CORE_LIBRARY}")
  message(STATUS "ALIEN_REFSEMANTIC_LIBRARY: ${ALIEN_REFSEMANTIC_LIBRARY}")

  set(ALIEN_LIBRARIES ${ALIEN_CORE_LIBRARY}
    ${ALIEN_REFSEMANTIC_LIBRARY}
    ${ALIEN_ARCANETOOLS_LIBRARY}
    ${ALIEN_IFPEN_LIBRARY}
    ${ALIEN_TRILINOS_LIBRARY}
    ${ALIEN_HPDDM_LIBRARY}
    ${ALIEN_EXTERNALPACKAGES_LIBRARY})

  add_library(alien INTERFACE IMPORTED)
  
  set_property(TARGET alien APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "Alien::alien_core")
  
  set_property(TARGET alien APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "Alien::alien_semantic_ref")
  
  set_property(TARGET alien APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "alien_external_packages")

  set_property(TARGET alien APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "alien_ifpen_solvers")

  if(TARGET alien_trilinos)
    set_property(TARGET alien APPEND PROPERTY 
      INTERFACE_LINK_LIBRARIES "alien_trilinos")
  endif()

  if(TARGET alien_hpddm)
    set_property(TARGET alien APPEND PROPERTY 
      INTERFACE_LINK_LIBRARIES "alien_hpddm")
  endif()
  set_property(TARGET alien APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "alien_arcane_tools")

  # Only for vs15 2017
  if(MSVC_VERSION GREATER 1900)
      if(TARGET hypre)
        get_target_property(HYPRE_LIBRARY hypre IMPORTED_LOCATION)
        get_filename_component(HYPRE_DIR ${HYPRE_LIBRARY} DIRECTORY)
        set(HYPRE_DLL_DIR "${HYPRE_DIR}")
        set(EXTRA_DLLS_TO_COPY 
           ${EXTRA_DLLS_TO_COPY}
           ${HYPRE_DLL_DIR}/hypre.dll)
      endif()
      if(TARGET petsc)
        get_target_property(PETSC_LIBRARY petsc_main IMPORTED_LOCATION)
        get_filename_component(PETSC_DIR ${PETSC_LIBRARY} DIRECTORY)
        set(PETSC_DLL_DIR "${PETSC_DIR}")
        # We need to add superlu, superlu_dist and its dependency
         set(EXTRA_DLLS_TO_COPY 
           ${EXTRA_DLLS_TO_COPY}
           //irfich3/R11-Group/Rt40_50_prj/D/D1653_Arcane/Public/Windows64/Softs/superlu/5.2.1-vs15/${CMAKE_BUILD_TYPE}/lib/libsuperlu_5.2.dll
           //irfich3/R11-Group/Rt40_50_prj/D/D1653_Arcane/Public/Windows64/Softs/superlu_dist/5.4.0-vs15/${CMAKE_BUILD_TYPE}/lib/superlu_dist.dll
           //irfich3/R11-Group/Rt40_50_prj/D/D1653_Arcane/Public/Windows64/Softs/superlu_dist/5.4.0-vs15/${CMAKE_BUILD_TYPE}/lib/blas.dll
           ${PETSC_DLL_DIR}/libpetsc.dll)
      endif()
      if(TARGET ifpsolver_main)
        get_target_property(IFPSOLVER_LIBRARY ifpsolver_main IMPORTED_LOCATION)
        get_target_property(IFPSOLVER_DOMAINDECOMP_LIBRARY ifpsolver_domaindecomp IMPORTED_LOCATION)
        # We need to add ifpsolver_decomp
         set(EXTRA_DLLS_TO_COPY 
           ${EXTRA_DLLS_TO_COPY}
           ${IFPSOLVER_LIBRARY}
           ${IFPSOLVER_DOMAINDECOMP_LIBRARY})
      endif()
  endif()
  add_definitions(-DUSE_ALIEN_V2)
  add_definitions(-DUSE_ALIEN_V20)
  set(Alien_PREFIX ${ALIEN_ROOT} CACHE INTERNAL "Alien prefix")
endif()
