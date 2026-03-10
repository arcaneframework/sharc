option(ENABLE_DEBUG_MATRIX "Enable Debug matrix instead of a sparse one" OFF)


createLibrary(FEMUtils)

set(ACCELERATOR_SOURCES
  ArcaneFemFunctionsGpu.cc
  CsrDoFLinearSystemImpl.cc
  CsrFormatMatrix.cc
  BSRFormat.cc
)

if (TARGET Arcane::arcane_aleph_hypre)
  list(APPEND ACCELERATOR_SOURCES HypreDoFLinearSystem.cc)
endif()

if (TARGET Arcane::arcane_aleph_petsc)
  list(APPEND ACCELERATOR_SOURCES PETScDoFLinearSystem.cc)
endif()

find_package(Alien)
if (Alien_FOUND)
  list(APPEND ACCELERATOR_SOURCES AlienDoFLinearSystem.cc)
endif()

addSources(FEMUtils
  FemUtils.h
  FemUtils.cc
  FemUtilsGlobal.h
  DoFLinearSystem.h
  DoFLinearSystem.cc
  CooFormatMatrix.h
  CsrFormatMatrix.h
  CsrFormatMatrixView.h
  CsrFormatMatrixView.cc
  BSRFormat.h
  DoFLinearSystemImplBase.cc
  FemDoFsOnNodes.h
  FemDoFsOnNodes.cc
  FemBoundaryConditions.cc
  ArcaneFemFunctionsGpu.h
  ArcaneFemFunctions.h
  ArcaneFemFunctions.cc
  AlephDoFLinearSystem.cc
  GaussQuadrature.h
  GaussDoFsOnCells.h
  GaussDoFsOnCells.cc
  Integer3std.h
  IDoFLinearSystemFactory.h
  IArcaneFemBC.h
  MeshTensorVariable.h
  MeshTensorVariable.H
  internal/IDoFLinearSystemImpl.h
  internal/OrderedRowColumnMap.h
  internal/CsrDoFLinearSystemImpl.h
  internal/DoFLinearSystemImplBase.h
  ${ACCELERATOR_SOURCES}
)

arcane_accelerator_add_source_files(${ACCELERATOR_SOURCES})
arcane_accelerator_add_to_target(FEMUtils)

if(USE_AXLSTAR)
generateAxl(FEMUtils
        AlephDoFLinearSystemFactory.axl
        SequentialBasicDoFLinearSystemFactory.axl
        HypreDoFLinearSystemFactory.axl
        PETScDoFLinearSystemFactory.axl
        AlienDoFLinearSystemFactory.axl
        FemBoundaryConditions.axl
        )
endif()


target_compile_definitions(FEMUtils PRIVATE $<$<BOOL:${ENABLE_DEBUG_MATRIX}>:ENABLE_DEBUG_MATRIX>)


if(TARGET Arcane::arcane_aleph)
  linkLibraries(FEMUtils Arcane::arcane_aleph)
endif()

set(FEMUTILS_HAS_PARALLEL_SOLVER FALSE)
set(FEMUTILS_HAS_PARALLEL_SOLVER_TRILINOS FALSE)
set(FEMUTILS_HAS_PARALLEL_SOLVER_HYPRE FALSE)
set(FEMUTILS_HAS_PARALLEL_SOLVER_PETSC FALSE)
set(FEMUTILS_HAS_PARALLEL_SOLVER_ALIEN FALSE)

if (Alien_FOUND)
  find_package(Boost REQUIRED)
  find_package(AlienPlugins REQUIRED)
  linkLibraries(FEMUtils
                Alien::alien_core
                Alien::alien_semantic_ref
                alien_arcane_tools
                arcane_full
               )
  message(STATUS "Alien backend is available")
  set(FEMUTILS_HAS_SOLVER_BACKEND_ALIEN TRUE)
endif()

if (TARGET Arcane::arcane_aleph_trilinos)
  set(FEMUTILS_HAS_PARALLEL_SOLVER TRUE)
  linkLibraries(FEMUtils Arcane::arcane_aleph_trilinos)
  message(STATUS "Trilinos backend is available")
  set(FEMUTILS_HAS_SOLVER_BACKEND_TRILINOS TRUE)
endif()

if (TARGET Arcane::arcane_aleph_petsc)
  set(FEMUTILS_HAS_PARALLEL_SOLVER TRUE)
  linkLibraries(FEMUtils Arcane::arcane_aleph_petsc)
  message(STATUS "PETSc backend is available")
  set(FEMUTILS_HAS_SOLVER_BACKEND_PETSC TRUE)
  find_package(PETSc)

  if (TARGET arcconpkg_PETSc)
    linkLibraries(FEMUtils arcconpkg_PETSc)
  endif()
endif()


if (TARGET Arcane::arcane_aleph_hypre)
  set(FEMUTILS_HAS_PARALLEL_SOLVER TRUE)
  linkLibraries(FEMUtils Arcane::arcane_aleph_hypre)
  message(STATUS "HYPRE backend is available")
  set(FEMUTILS_HAS_SOLVER_BACKEND_HYPRE TRUE)
  find_package(Hypre)
  # There is several target for Hypre depending on how Hypre is compiled
  # We try 'HYPRE::HYPRE' which is available with recents versions and
  # if it is not available, we try with arccon
  if (TARGET HYPRE::HYPRE)
    linkLibraries(FEMUtils HYPRE::HYPRE)
  elseif (TARGET arccon::Hypre)
    linkLibraries(FEMUtils arccon::Hypre)
  else()
    message(WARNING "Do not find a valid target for Hypre. This may produce link errors")
  endif()
  #target_sources(FemUtils PRIVATE HypreDoFLinearSystem.cc)
  find_package(Threads)
  if (TARGET Threads::Threads)
    linkLibraries(FEMUtils Threads::Threads)
  endif()
endif()


if(NOT FEMUTILS_HAS_PARALLEL_SOLVER)
  message(STATUS "WARNING: No Arcane parallel solver is available. Parallel linear solver system is disabled")
endif()

# Add a variable in cache to check which parallel solvers are availables
set(FEMUTILS_HAS_PARALLEL_SOLVER ${FEMUTILS_HAS_PARALLEL_SOLVER} CACHE BOOL "Is Parallel solver available" FORCE)
set(FEMUTILS_HAS_SOLVER_BACKEND_TRILINOS ${FEMUTILS_HAS_SOLVER_BACKEND_TRILINOS} CACHE BOOL "Is Trilinos solver available" FORCE)
set(FEMUTILS_HAS_SOLVER_BACKEND_PETSC ${FEMUTILS_HAS_SOLVER_BACKEND_PETSC} CACHE BOOL "Is PETSc solver available" FORCE)
set(FEMUTILS_HAS_SOLVER_BACKEND_HYPRE ${FEMUTILS_HAS_SOLVER_BACKEND_HYPRE} CACHE BOOL "Is Hypre solver available" FORCE)
set(FEMUTILS_HAS_SOLVER_BACKEND_ALIEN ${FEMUTILS_HAS_SOLVER_BACKEND_ALIEN} CACHE BOOL "Is Alien solver available" FORCE)

commit(FEMUtils)
