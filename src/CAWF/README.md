# CAWF (ArcaneFEM core)

> 🧠 Documentation enrichie par IA — basée sur l'analyse du code source existant

## 📋 Table des matières

- [Description](#description)
- [Fonctionnalités](#fonctionnalités)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [API / Interface](#api--interface)
- [Architecture](#architecture)
- [Dépendances](#dépendances)
- [Configuration](#configuration)
- [Ressources](#ressources)

## Description

Le répertoire `src/CAWF/` regroupe le cœur des solveurs éléments finis du projet ShArc, issus d'ArcaneFEM / CAWF. Il fournit :

- des solveurs complets pour des PDEs classiques (élasticité linéaire, Poisson),
- un module de compaction géomécanique avec maillage dynamique,
- un ensemble d'utilitaires FEM (`FEMUtils/`) pour la gestion des degrés de liberté (DoF), des formats de matrices, des conditions aux limites, des outils de vérification,
- un gestionnaire de maillage dynamique (`DynamicMeshMng/`) pour les cas évolutifs (ajout de couches, compaction).





  - Formulation de Navier pour déformations élastiques isotropes.
  - Support 2D/3D sur maillages non structurés.
  - Gestion des conditions de Dirichlet, tractions, forces de volume.
  - Implémentations CPU et SYCL (`FemModule.cc`, `FemModuleSYCL.cc`).

  - Solveur Galerkin FEM pour l'équation de Poisson.
  - Maillages 2D/3D généralistes (Gmsh 4.1).
  - Conditions de Dirichlet et potentiellement Neumann.
  - Documentation mathématique détaillée dans `Poisson/Readme.md`.

  - Classe `CompactionModule` avec interface orientée simulation :
    - `init()` — initialisation du cas.
    - `test()` — exécution du scénario de compaction.
    - `updateTopBoundary()`, `computeDensity()`, `computePressure()`, `updateVariablesTN()`.
  - Gestion d'événements de couche (activation de nouvelles couches de cellules à intervalles de temps).
  - Paramètres physiques (gravité, top/bottom boundary, périodes d'événements).

  - Interface `IDynamicMeshMng` et service `DynamicMeshMngService.cc`.
  - Gestion des mises à jour géométriques et de topologie pour les cas de compaction / maillages évolutifs.

  - Formats de matrices et systèmes linéaires :
    - `CsrFormatMatrix.{h,cc}`
    - `BSRFormat.{h,cc}`
    - `CooFormatMatrix.h`
  - Implémentations de systèmes linéaires DoF (`DoFLinearSystem*`, `DoFLinearSystemImplBase`, `IDoFLinearSystemImpl`) avec backends :
    - Alien (`AlienDoFLinearSystem`, `AlienDoFLinearSystemFactory.axl`).
    - PETSc (`PETScDoFLinearSystem`, `PETScDoFLinearSystemFactory.axl`).
    - Hypre (`HypreDoFLinearSystem`, `HypreDoFLinearSystemFactory.axl`).
    - Aleph (`AlephDoFLinearSystem`, `AlephDoFLinearSystemFactory.axl`).
    - variantes GPU/SYCL (`AlienBSRFormatSYCL.cc`, `ArcaneFemFunctionsGpu.{h,cc}`).
  - Conditions aux limites FEM (`FemBoundaryConditions.{axl,cc}`, `IArcaneFemBC.h`).
  - Quadrature de Gauss, DoFs sur cellules et noeuds (`GaussQuadrature.*`, `GaussDoFsOnCells.*`, `FemDoFsOnNodes.*`).
  - Outils de vérification des résultats :
    - `checkNodeResultFile(...)` pour comparer les résultats nodaux à des fichiers de référence (Real, Real2, Real3).
    - `readFileAsCaseTable(...)` pour transformer des fichiers en `CaseTable`.
  - Utilitaires tensoriels/matrices (`FemUtils.h`) avec types `RealMatrix`, `RealVector`, `Tensor2`, opérations d'outer product, trace, norme, etc.


  - Arcane (maillage, variables, modules, MatVec::Matrix, I/O, CaseTable).
  - ArcGeoSim (services applicatifs, apprentis solveurs, outils de géométrie, runners accélérateurs).
  - Outils FEM spécifiques ArcaneFEM (sous le namespace `Arcane::FemUtils`).
  - Alien (matrix/vector distributions, solveurs linéaires).
  - PETSc, Hypre, Aleph (gestion de systèmes linéaires distribués).
  - Support potentiel GPU/SYCL via `Arcane::accelerator::Runner` et implémentations dédiées.











  - comparer les résultats nodaux (scalar, vectoriels) à des fichiers ASCII,
  - avec gestion d'un epsilon relatif et d'un seuil minimal.











