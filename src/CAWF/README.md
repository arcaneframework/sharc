# CAWF – ArcaneFEM Workflows Library

CAWF is a collection of finite-element solvers and utilities built on top of
ArcaneFEM and the ArcGeoSim/Arcane framework. It provides ready-to-use
applications for scalar Poisson problems, linear elasticity, compaction
simulations, and dynamic mesh management, plus shared FEM infrastructure
(`FEMUtils`).

CAWF is built as a library and linked into the main **ShArc** executable.

---

## Contents

- [Overview](#overview)
- [Directory Layout](#directory-layout)
- [Building CAWF](#building-cawf)
- [Solvers and Services](#solvers-and-services)
  - [Poisson Solver](#poisson-solver)
  - [Linear Elasticity Solver](#linear-elasticity-solver)
  - [Compaction Solver](#compaction-solver)
  - [Dynamic Mesh Manager](#dynamic-mesh-manager)
- [Configuration Patterns](#configuration-patterns)
  - [Mesh section](#mesh-section)
  - [FEM section](#fem-section)
  - [Boundary conditions](#boundary-conditions)
  - [Post-processing](#post-processing)
- [FEM Utilities (FEMUtils)](#fem-utilities-femutils)
  - [DoF linear systems](#dof-linear-systems)
  - [Matrix formats](#matrix-formats)
  - [Boundary conditions helpers](#boundary-conditions-helpers)
  - [GPU / SYCL backends](#gpu--sycl-backends)
- [Extending CAWF](#extending-cawf)

---

## Overview

CAWF groups several ArcaneFEM-based modules:

- **Poisson** – solves scalar Poisson equations on 2D/3D unstructured meshes
  using a Galerkin finite element method. Suitable for diffusion, heat
  conduction, and similar elliptic problems.

- **Elasticity** – solves steady linear elasticity (Navier's equations) for
  small deformations in solids, computing the displacement vector field.

- **Compaction** – solves compaction / basin-like problems (e.g. synthetic
  basin tests, cube evolutions).

- **DynamicMeshMng** – service to manage evolving meshes (dynamic mesh
  management) within Arcane/ArcGeoSim.

All solvers share **FEMUtils**, which implements generic finite-element
infrastructure: degree-of-freedom (DoF) linear systems, sparse matrix formats,
boundary conditions, and support for various linear algebra backends.

---

## Directory Layout

The CAWF module is configured in `src/CAWF/config.xml`:

```xml
<makefile>
  <needed-packages>
    <package name="arcane" />
    <package name="precice" optional="true" />
  </needed-packages>

  <directories>
    <directory>Compaction</directory>
    <directory>Elasticity</directory>
    <directory>Poisson</directory>
    <directory>DynamicMeshMng</directory>
  </directories>
</makefile>
```

Key directories:

- `src/CAWF/CMakeLists.txt` – builds the `CAWF` library and links FEMUtils.
- `src/CAWF/FEMUtils/` – shared FEM infrastructure.
- `src/CAWF/Poisson/` – Poisson solver, driver, config, and documentation.
- `src/CAWF/Elasticity/` – linear elasticity solver and documentation.
- `src/CAWF/Compaction/` – compaction solver.
- `src/CAWF/DynamicMeshMng/` – dynamic mesh manager service.

---

## Building CAWF

CAWF is built as part of the main ShArc CMake project.

From `src/CAWF/CMakeLists.txt`:

- The library is created and registered via ArcGeoSim helpers:

```cmake
add_subdirectory(FEMUtils)

createLibrary(CAWF)
generateCMakeLists(CAWF)

target_include_directories(CAWF PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
```

- Optional GPU/SYCL support is enabled via Alien / SYCL options:

```cmake
if(ALIEN_USE_SYCL)
  # hipSYCL, Intel SYCL/DPCPP, AdaptiveCpp backends
  addSources(CAWF Elasticity/FemModuleSYCL.cc)
  target_compile_definitions(CAWF PUBLIC ALIEN_USE_SYCL USE_SYCL2020)
  add_sycl_to_target(TARGET CAWF
                     SOURCES Elasticity/FemModuleSYCL.cc)
  # additional flags depending on backend (HIP, CUDA, etc.)
endif()
```

- On Windows, CAWF links ArcGeoSim physics and tests:

```cmake
linkLibraries(CAWF ArcGeoSim::ArcGeoSim_physics
                  ArcGeoSim::ArcGeoSim_tests)
```

- CAWF always links FEMUtils when available:

```cmake
if(TARGET FEMUtils)
  linkLibraries(CAWF FEMUtils)
endif()
```

To build CAWF:

1. Configure the main ShArc project as documented in the top-level README
   (ensure `Arcane` / `ArcGeoSim` and Alien are available).
2. CMake will automatically build `FEMUtils` and `CAWF` as part of the root
   `CMakeLists.txt` (via `add_subdirectory(src/CAWF)`).

---

## Solvers and Services

### Poisson Solver

Directory: `src/CAWF/Poisson/`  
Documentation: `src/CAWF/Poisson/Readme.md`

The Poisson solver solves

\[ \nabla^2 u = \mathcal{f} \quad \text{in } \Omega^h \]

with Dirichlet and optional Neumann boundary conditions, on 2D/3D unstructured
meshes.

The dedicated Poisson README covers:

- Mathematical formulation of the Poisson problem.
- Example input file (`inputs/circle.arc`).
- Configuration snippets for:
  - Mesh (`meshes/circle_cut.msh`).
  - Source term `<f>5.5</f>`.
  - Dirichlet / Neumann boundary conditions.
  - Post-processing (`VtkHdfV2PostProcessor`, variable `U`).

> For details, see **`src/CAWF/Poisson/Readme.md`**.

---

### Linear Elasticity Solver

Directory: `src/CAWF/Elasticity/`  
Documentation: `src/CAWF/Elasticity/Readme.md`

The elasticity solver solves steady linear elasticity (Navier's equation):

\[ -\nabla\cdot\sigma(\mathbf{x}) = \mathbf{f}(\mathbf{x}) \]

with isotropic stress:

\[ \sigma(\mathbf{x}) = \lambda (\nabla\cdot\mathbf{u})\mathbb{I} +
\mu(\nabla\mathbf{u} + (\nabla\mathbf{u})^\mathrm{T}) \]

The solver is demonstrated on a 2D bar bending under its own weight, via
`bar.2D.Dirichlet.bodyForce.arc`.

The Elasticity README covers:

- Mathematical background (stress, strain, variational formulation).
- Example mesh (`meshes/bar.msh`).
- FEM configuration:
  - Material properties: `<E>21.0e5</E>`, `<nu>0.28</nu>`.
  - Body force: `<f>NULL -1.0</f>`.
  - Dirichlet BC: clamped end `<surface>left</surface>`.
- Post-processing configuration for `U` (displacement vector).

> For details, see **`src/CAWF/Elasticity/Readme.md`**.

---

### Compaction Solver

Directory: `src/CAWF/Compaction/`

The compaction module provides examples such as:

- Cube evolution (`Compaction-cube-cawf-evol.arc`).
- Synthetic basin (`Compaction-synth-basin.arc`).
- Ring compaction (`Compaction-ring.arc`).

Configuration follows the same pattern as Poisson/Elasticity:

- `<meshes>` section for geometry.
- `<fem>` section for material and driving forces.
- `<arcane-post-processing>` for outputs.

> A dedicated `Compaction/Readme.md` can be added mirroring the Poisson and
> Elasticity documentation style.

---

### Dynamic Mesh Manager

Directory: `src/CAWF/DynamicMeshMng/`

Provides a service `DynamicMeshMng` (`DynamicMeshMng.axl`,
`DynamicMeshMngService.cc`) to manage evolving meshes (refinement, motion,
etc.) in Arcane/ArcGeoSim.

Configuration is service-based and can be included in case files to enable
dynamic mesh management for CAWF solvers.

---

## Configuration Patterns

Although each solver has its own `.arc` case file, they all follow a common
structure.

### Mesh section

```xml
<meshes>
  <mesh>
    <filename>meshes/&lt;mesh-file&gt;.msh</filename>
    <subdivider>
      <nb-subdivision>0</nb-subdivision> <!-- optional for mesh refinement -->
    </subdivider>
  </mesh>
</meshes>
```

Use Gmsh 4.1 `.msh` format. Physical groups (e.g. `left`, `horizontal`) must
match the boundary names used in the `<boundary-conditions>` section.

### FEM section

Examples:

- Poisson:

```xml
<fem>
  <f>5.5</f> <!-- scalar source term -->
</fem>
```

- Elasticity:

```xml
<fem>
  <E>21.0e5</E>    <!-- Young's modulus -->
  <nu>0.28</nu>    <!-- Poisson's ratio -->
  <f>NULL -1.0</f> <!-- body force vector (x, y or x, y, z) -->
</fem>
```

### Boundary conditions

Dirichlet (Poisson):

```xml
<boundary-conditions>
  <dirichlet>
    <surface>horizontal</surface>
    <value>0.5</value>
  </dirichlet>
</boundary-conditions>
```

Dirichlet (Elasticity, vector displacement):

```xml
<boundary-conditions>
  <dirichlet>
    <surface>left</surface>
    <value>0.0 0.0</value> <!-- u1, u2 -->
  </dirichlet>
</boundary-conditions>
```

Neumann-like conditions can be added similarly, depending on the solver.

### Post-processing

Common pattern:

```xml
<arcane-post-processing>
  <output-period>1</output-period>
  <format name="VtkHdfV2PostProcessor" />
  <output>
    <variable>U</variable> <!-- or other field -->
  </output>
</arcane-post-processing>
```

Output files are typically `Mesh0.hdf` in
`output/depouillement/vtkhdfv2`, and can be visualized with PARAVIS. Results
are usually of \(\mathbb{P}_1\) finite element order (node-based).

---

## FEM Utilities (FEMUtils)

Directory: `src/CAWF/FEMUtils/`

FEMUtils contains shared infrastructure used by all CAWF solvers.

### DoF linear systems

Core types and interfaces:

- `DoFLinearSystem.h`, `DoFLinearSystem.cc`
- `IDoFLinearSystemFactory.h`

Backend-specific implementations:

- `AlienDoFLinearSystem.cc`, `AlienDoFLinearSystemFactory.axl`
- `HypreDoFLinearSystem.cc`, `HypreDoFLinearSystemFactory.axl`
- `PETScDoFLinearSystem.cc`, `PETScDoFLinearSystemFactory.axl`
- `SequentialBasicDoFLinearSystemFactory.axl`
- `AlephDoFLinearSystem.cc`, `AlephDoFLinearSystemFactory.axl`

These provide a unified API for assembling and solving FEM linear systems with
different linear algebra backends.

### Matrix formats

Sparse matrix infrastructure:

- `CsrFormatMatrix.h`, `CsrFormatMatrix.cc`
- `CsrFormatMatrixView.h`, `CsrFormatMatrixView.cc`
- `BSRFormat.h`, `BSRFormat.cc`
- `AlienBSRFormat.h`, `AlienBSRFormat.cc`
- `CooFormatMatrix.h`

Different formats are used depending on the backend and solver requirements.

### Boundary conditions helpers

- `FemBoundaryConditions.cc`, `FemBoundaryConditions.axl`
- `IArcaneFemBC.h`

These handle application of Dirichlet/Neumann boundary conditions on FEM
systems, mapping configuration `<boundary-conditions>` to algebraic
constraints.

### GPU / SYCL backends

When `ALIEN_USE_SYCL` is enabled, FEMUtils provides SYCL-based implementations:

- `AlienDoFLinearSystemSYCL.cc`
- `AlienBSRFormatSYCL.cc`
- `ArcaneFemFunctionsGpu.h`, `ArcaneFemFunctionsGpu.cc`

The Elasticity module additionally uses `FemModuleSYCL.cc`. CMake configuration
in `src/CAWF/CMakeLists.txt` controls which SYCL implementation (hipSYCL, Intel
SYCL/DPCPP, AdaptiveCpp) is used and adds appropriate compile and link flags.

---

## Extending CAWF

To add a new FEM-based solver to CAWF:

1. **Create a module directory** under `src/CAWF/` (e.g. `NewPhysics/`).
2. **Define the module AXL and C++ implementation**:
   - `NewPhysics.axl` (Arcane module/service declaration).
   - `NewPhysicsModule.h/.cc` (solver implementation using FEMUtils).
3. **Register the directory in `src/CAWF/config.xml`**:

   ```xml
   <directory>NewPhysics</directory>
   ```

4. **Update `src/CAWF/CMakeLists.txt`** if special sources or backends are
   required (e.g. SYCL-specific code).
5. **Add tests and example cases**:
   - `.arc` files under `test/CAWF/NewPhysics/`.
   - Meshes under `test/CAWF/NewPhysics/meshes/`.

Use the existing Poisson and Elasticity modules as templates for:

- Module lifecycle (`init`, `compute`, etc.).
- Configuration structure in `.arc` files.
- Post-processing setup.
