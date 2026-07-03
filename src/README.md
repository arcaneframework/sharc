# ShArc Library (`src/`)

> 🧠 Documentation enrichie par IA — basée sur l'analyse du code source et la structure CMake

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

Le répertoire `src/` regroupe la **bibliothèque principale ShArc** et l’ensemble des modules métier associés. Il ne contient pas directement de logique physique, mais sert de **point d’assemblage** :

- il définit la bibliothèque `ShArc` au niveau CMake,
- il agrège les modules d’orchestration, de simulation diphasique, de solvers FEM et de benchmarks géométriques,
- il décrit les dépendances vis-à-vis du framework **Arcane/ArcGeoSim** et des bibliothèques numériques (ArcRes, Alien, Hypre, PETSc, etc.).

Les sous-répertoires principaux sont :

- `ShArc/` — module d’orchestration global (services de temps, géométrie, post-traitement, validation),
- `TwoPhaseFlowSimulation/` — solveur d’écoulements diphasiques et module de convection thermo-chimique,
- `CAWF/` — bibliothèque de workflows FEM (Poisson, élasticité, compaction, dynamic mesh),
- `ExaDiBench/` — benchmarks géométriques (déformation de maillage, mesures volume/surface/centre),
- `Main/` — point d’entrée exécutable `ShArc.exe` (wrapper ArcGeoSim).

Côté build, `src/CMakeLists.txt` crée la bibliothèque `ShArc`, génère les CMakeLists spécifiques et le modèle Gump (ArcRes), puis lie la bibliothèque aux composants externes :

```cmake
createLibrary(ShArc)

generateCMakeLists(ShArc)

generateGumpModel(TwoPhaseFlowSimulation/DataModel/ArcRes.gump)

linkLibraries(ShArc arcres)

# pour windows
linkLibraries(ShArc
              ArcGeoSim::ArcGeoSim_mesh
              ArcGeoSim::ArcGeoSim_physics
              ArcGeoSim::ArcGeoSim_tests
             )

linkLibraries(ShArc CAWF)

commit(ShArc)
```

Le fichier `src/config.xml` décrit les dépendances logiques de la bibliothèque :

```xml
<makefile>
  <needed-packages>
    <package name="arcane" />
    <package name="boost" />
  </needed-packages>
  <directories>
    <directory>ShArc</directory>
    <directory>TwoPhaseFlowSimulation</directory>
    <directory>ExaDiBench</directory>
  </directories>
</makefile>
```

## Fonctionnalités

Au niveau de la bibliothèque `ShArc`, les fonctionnalités exposées sont principalement :

- ✅ **Orchestration de simulation** (`src/ShArc/`)
  - Gestion de la timeline, du pas de temps, de la géométrie et du post-traitement.
  - Validation de fin de simulation et conditions d’arrêt.

- ✅ **Simulation diphasique et thermo-chimique** (`src/TwoPhaseFlowSimulation/`)
  - Résolution d’écoulements diphasiques pression/saturations.
  - Résolution d’un système P–C–T en convection thermo-chimique.
  - Intégration avec le modèle de données ArcRes.

- ✅ **Solvers FEM génériques (CAWF)** (`src/CAWF/`)
  - Solveurs Poisson, élasticité linéaire, compaction.
  - Gestion de systèmes linéaires FEM multi-backends (Alien, Hypre, PETSc, Trilinos, …).
  - Utilitaires FEM (FEMUtils : DoF, formats de matrices, conditions aux limites, GPU/SYCL).

- ✅ **Benchmarks géométriques (ExaDiBench)** (`src/ExaDiBench/`)
  - Déformation contrôlée de maillages.
  - Calcul et vérification de volumes, surfaces et centres.
  - Export de maillage (VTK, MSH, IXM).

- ✅ **Exécutable ShArc** (`src/Main/`)
  - Point d’entrée minimal `main()` intégrant l’infrastructure ArcGeoSim.
  - Lancement de simulations à partir de fichiers `.arc` (cas de tests, cas utilisateurs).

## Prérequis

- **Langage** :
  - C++ (C++11+) pour tous les modules ShArc.
  - XML/AXL pour la description des modules Arcane.
- **Frameworks / bibliothèques** :
  - Arcane / ArcGeoSim (chargés via les CMake et scripts de la racine).
  - ArcRes (modèle de données physiques).
  - Alien + solveurs linéaires (Hypre, PETSc, Trilinos, etc.) selon configuration.
  - Boost (listé dans `src/config.xml`).
- **Contexte build** :
  - Submodules `common/ArcGeoSim` et `common/ArcaneInfra` initialisés (`git submodule update --init`).
  - Variables CMake/frameworks configurées comme indiqué dans le README racine (par ex. `ARCANEFRAMEWORK_ROOT`).

## Installation

La bibliothèque `ShArc` est construite automatiquement dans le cadre du build global du projet.

Exemple de configuration (extrait du README racine, à adapter à votre environnement) :

```bash
# Depuis la racine du dépôt ShArc
cmake -S sharc -B build-sharc \
  -DCMAKE_BUILD_TYPE=Release \
  -DDOTNET_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DARCANEFRAMEWORK_ROOT=${INSTALL_DIR} \
  -DUSE_ARCANE_V3=ON \
  -DCMAKE_C_COMPILER=mpicc -DCMAKE_CXX_COMPILER=mpic++

cmake --build build-sharc
```

CMake se charge de :

- construire la bibliothèque `ShArc` (ce dossier `src/`),
- construire les modules et bibliothèques associés (`CAWF`, `ExaDiBench`, etc.),
- produire l’exécutable `ShArc.exe` dans `src/Main`.

## Utilisation

Au niveau `src/`, l’utilisation se fait principalement via :

- l’exécutable `ShArc.exe` (voir `src/Main/README.md`),
- les cas de test `.arc` en `test/` (TwoPhaseFlowSimulation, ExaDiBench, CAWF, ThermoChemicalConvection),
- l’intégration dans d’autres projets Arcane/ArcGeoSim qui lient la bibliothèque `ShArc`.

Exemples :

```bash
# Lancer un cas deux phases SPE10
./ShArc.exe test/TwoPhaseFlowSimulation/use-case-two-phase-flow-spe10-petsc-bcgs-ilu0.arc

# Lancer un benchmark géométrique
./ShArc.exe test/ExaDiBench/bench-cube3d.arc
```

## API / Interface

### Modules principaux contenus dans la bibliothèque

| Symbole / module                  | Type             | Description |
|----------------------------------|------------------|-------------|
| `ShArcModule`                    | module C++       | Orchestration globale de la simulation (temps, géométrie, post-traitement, validation). |
| `TwoPhaseFlowSimulationModule`   | module C++       | Simulation d’écoulements diphasiques pression/saturations sur maillages 3D. |
| `ThermoChemicalConvectionModule` | module C++       | Convection thermo-chimique couplée (P–C–T). |
| `CAWF` (library)                 | bibliothèque C++ | Ensemble de solveurs FEM (Poisson, Elasticity, Compaction, DynamicMeshMng) et d’utilitaires FEM. |
| `ExaDiBenchModule`               | module C++       | Benchmarks géométriques (déformations, mesures globales, exports). |
| `ShArc.exe`                      | exécutable       | Point d’entrée de l’application ShArc, basé sur ArcGeoSim. |

### Points d’entrée haut niveau

Les points d’entrée détaillés (méthodes `build`, `init`, `compute`, etc.) sont décrits dans les READMEs des sous-modules :

- `src/ShArc/README.md`
- `src/TwoPhaseFlowSimulation/README.md`
- `src/CAWF/README.md`
- `src/ExaDiBench/README.md`
- `src/Main/README.md`

## Architecture

```text
┌─────────────────────────────────────────────────────┐
│                    src/ (ShArc)                    │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌────────────┐  ┌───────────────────────────────┐  │
│  │  ShArc/    │  │ TwoPhaseFlowSimulation/      │  │
│  │ (orchestr.)│  │ (2-phase & thermo-chem)      │  │
│  └────────────┘  └───────────────────────────────┘  │
│         │                    │                       │
│         ▼                    ▼                       │
│  ┌────────────┐      ┌───────────────┐               │
│  │  CAWF/     │      │ ExaDiBench/   │               │
│  │ (FEM libs) │      │ (geo. bench)  │               │
│  └────────────┘      └───────────────┘               │
│         │                    │                       │
│         ▼                    ▼                       │
│      ┌───────────────┐   ┌───────────────┐           │
│      │   Main/       │   │  DataModel/   │           │
│      │ (ShArc.exe)   │   │ ArcRes Gump   │           │
│      └───────────────┘   └───────────────┘           │
│                                                     │
└─────────────────────────────────────────────────────┘
          │                │                 │
          ▼                ▼                 ▼
   Arcane / ArcGeoSim   ArcRes        Solveurs linéaires
   (framework)          (physique)    (Alien, Hypre, PETSc…)
```

- `src/CMakeLists.txt` gère l’assemblage de la bibliothèque `ShArc` (lien avec `arcres`, `CAWF`, ArcGeoSim).
- `src/config.xml` liste les sous-répertoires gérés par l’infrastructure ArcaneInfra (`ShArc`, `TwoPhaseFlowSimulation`, `ExaDiBench`).

## Dépendances

| Dépendance                   | Usage dans `src/` |
|------------------------------|-------------------|
| `arcane`                     | Framework de base (modules, time loop, maillages, I/O). |
| `boost`                      | Utilitaires C++ (listé dans `config.xml`). |
| `ArcGeoSim::ArcGeoSim_mesh`  | Outils maillage ArcGeoSim (principalement pour Windows). |
| `ArcGeoSim::ArcGeoSim_physics` | Outils physiques ArcGeoSim (Windows). |
| `ArcGeoSim::ArcGeoSim_tests` | Utilitaires de tests ArcGeoSim (Windows). |
| `arcres`                     | Modèle de données pour les systèmes physiques (TwoPhaseFlowSimulation). |
| `CAWF`                       | Bibliothèque FEM liée à `ShArc`. |
| `Alien`, Hypre, PETSc, etc.  | Solveurs et préconditionneurs linéaires, utilisés via CAWF et les modules numériques. |

## Configuration

La configuration détaillée des modules se fait dans :

- les fichiers `config.xml` des sous-répertoires (`src/`, `src/CAWF/`, `src/ExaDiBench/`),
- les descripteurs `.axl` (`ShArc.axl`, `TwoPhaseFlowSimulation.axl`, `ThermoChemicalConvection.axl`, `ExaDiBench.axl`),
- les fichiers de cas `.arc` situés dans `test/`.

Exemples de paramètres globaux à configurer côté build (voir README racine) :

| Variable / option          | Description |
|----------------------------|-------------|
| `ARCANEFRAMEWORK_ROOT`     | Répertoire d’installation du framework Arcane/ArcGeoSim. |
| `ARCGEOSIM_FRAMEWORK_ROOT` | Emplacement alternatif du framework ArcGeoSim (si externalisé). |
| `USE_ARCANE_V3`            | Active l’utilisation d’Arcane v3 et des metas associées (`strong_options`, `alien20`, `arcane3`). |

## Ressources

- `README.md` (racine du dépôt) — installation complète et configuration du projet.
- `src/ShArc/README.md` — documentation du module d’orchestration.
- `src/TwoPhaseFlowSimulation/README.md` — documentation du solveur diphasique et du module thermo-chimique.
- `src/CAWF/README.md` — documentation de la bibliothèque FEM (Poisson, Elasticity, Compaction, FEMUtils).
- `src/ExaDiBench/README.md` — documentation du module de benchmark géométrique.
- `src/Main/README.md` — documentation de l’exécutable `ShArc.exe`.
- `docs/content/src/` — documentation Hugo associée au code source.
- Documentation Arcane / ArcGeoSim / ArcRes — pour les concepts de modules, services, systèmes physiques et solveurs.
