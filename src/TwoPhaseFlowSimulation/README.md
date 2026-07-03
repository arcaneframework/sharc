# TwoPhaseFlowSimulation

> 🧠 Documentation enrichie par IA — basée sur l'analyse du code source et la structure des cas de test

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

Le module `TwoPhaseFlowSimulation` implémente le solveur principal de ShArc pour la **simulation d’écoulements diphasiques en milieu poreux** (pression + saturations), ainsi qu’un module optionnel de **convection thermo‑chimique couplée** (pression, concentration, température).

Il s’appuie sur le modèle de données **ArcRes** pour décrire les systèmes physiques (fluides et solide), et sur le framework **ArcGeoSim/Arcane** pour la discrétisation numérique (schéma deux‑points, transmissivités, opérateurs de type `DivKGrad`) et la résolution non linéaire (`INewtonSolver`). La structure du module est entièrement pilotée par les fichiers `.axl` (descripteurs Arcane) qui enregistrent les points d’entrée (`init`, `compute`) et la liste des services (conditions initiales, lois physiques, conditions aux limites, puits, etc.).

Le module organise la simulation autour d’un système non linéaire `ArcNum::INonLinearSystem` :
- construction de la matrice et du résidu à chaque itération Newton (accumulation, flux internes, flux de bord, flux de puits, équation de fermeture),
- gestion des variables d’état dans des "folders" de cellules/faces (`Law::VariableCellFolder`, `PartialVariableFaceFolder`),
- gestion de la **restauration de l’état** en cas d’échec du solveur (snapshots temporels, `ReplayException`).

Le répertoire encapsule également la logique de **conditions aux limites**, **conditions initiales**, **puits** et **lois physiques** (pression capillaire, densité fluide, perméabilité relative, loi constante), ainsi que des interfaces génériques pour la construction du système physique (`IPhysicalSystem`, `ISystemBuilder`, `IVariableManager`).

## Fonctionnalités

Basé sur les symboles et services détectés :

- ✅ Simulation diphasique principale via `TwoPhaseFlowSimulationModule` :
  - Construction d’un système non linéaire sur pression + saturations.
  - Calcul de l’accumulation, des flux internes, des flux de bord et des flux de puits.
  - Équation de fermeture (somme des saturations = 1).
  - Application de contraintes physiques sur la solution (projection des saturations dans [0,1], renormalisation).

- ✅ Simulation thermo‑chimique couplée via `ThermoChemicalConvectionModule` :
  - Équations pour pression, concentration et température.
  - Accumulation couplée fluide/solide (chaleur volumique, capacité thermique).
  - Flux d’advection (Darcy) et de diffusion moléculaire pour la concentration.
  - Flux de chaleur conductif (fluide/solide) en plus de l’advection.

- ✅ Gestion des **conditions initiales** :
  - Services `IInitialCondition` appliqués sur les variables de domaine (pression, saturations, concentration, température, etc.).

- ✅ Gestion des **conditions aux limites** :
  - Interface `IBoundaryManager` et service `DirichletManagerService`.
  - Dossiers `BoundaryCondition/*` pour les différents types de bord (Dirichlet, etc.).
  - Évaluation des lois physiques sur les faces de bord (densité, viscosité, perméabilité relative, pression capillaire).

- ✅ Gestion des **puits** (`WellCondition`) :
  - Interface `IWellManager`, services de gestion des puits (débits, index de puits via `WellIndexComputer`).
  - Flux puits (terme source/sink) intégrés dans le résidu.

- ✅ Modèle de **système physique ArcRes** :
  - Interfaces `IPhysicalSystem`, `ISystemBuilder`, `IVariableManager`.
  - Dossier `PhysicalSystem/` pour le couplage ArcRes ↔ simulation (domaines, phases, variables).
  - `UserSystemService` pour brancher un système utilisateur configuré.

- ✅ Lois physiques :
  - `CapillaryPressureLaw` (pression capillaire vs saturation).
  - `FluidDensityLaw` (densité fluide vs température et concentration).
  - `RelativePermeabilityPowerLaw` (perméabilité relative en fonction de la saturation).
  - `ConstantLaw` (loi paramétrable constante).
  - Configurateurs `ILawConfigurator` et services `*ConfigService`.

- ✅ Gestion géométrique :
  - Enregistrement des propriétés géométriques (volume, centre, normale) via `IGeometryMng`.
  - Mise à jour explicite de la géométrie (`ManualUpdateGeometryPolicy`), notamment après la création de groupes.

- ✅ Profilage du système linéaire :
  - Profils matrice deux‑points (`_twoPointsProfiler`) et multi‑points (`_multiPointsProfiler`) sur groupes de mailles/faces.

## Prérequis

- Langage :
  - C++ (Arcane/ArcGeoSim, ArcRes) — C++11+.
  - Descripteurs XML `.axl` pour les modules Arcane.

- Frameworks / bibliothèques :
  - Arcane 3 / ArcGeoSim (Mesh, Geometry, Numerics, Law2, AppService).
  - ArcRes (Entities, Properties).
  - ArcNumTools (vecteurs/matrices, solveur Newton `INewtonSolver`).
  - Alien (profilage de matrice `Alien::MatrixProfiler`).

- Contexte projet :
  - ShArc est déjà configuré selon le `CMakeLists.txt` racine (chargement ArcGeoSim via `LoadArcGeoSim.cmake`).
  - Les sous‑modules `common/ArcGeoSim` et `common/ArcaneInfra` sont initialisés (`git submodule update --init`).

## Installation

Le module n’a pas de CMakeLists dédié (il est intégré dans `src/CMakeLists.txt` via les macros ArcGeoSim). Typiquement :

```bash
# À partir de la racine du dépôt ShArc
cmake -S sharc -B build-sharc \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DARCANEFRAMEWORK_ROOT=${INSTALL_DIR} \
  -DUSE_ARCANE_V3=ON \
  # ... compilos MPI, options Alien, etc.

cmake --build build-sharc
```

Aucune étape spécifique n’est nécessaire pour `TwoPhaseFlowSimulation` : le module est compilé dans la bibliothèque ShArc et enregistré via `ARCANE_REGISTER_MODULE_TWOPHASEFLOWSIMULATION` et `ARCANE_REGISTER_MODULE_THERMOCHEMICALCONVECTION`.

## Utilisation

### Points d’entrée module deux phases

Dans `TwoPhaseFlowSimulation.axl` :

```xml
<entry-points>
  <entry-point method-name="init"    name="Init"    where="init" />
  <entry-point method-name="compute" name="Compute" where="compute-loop" />
</entry-points>
```

Ce qui correspond au C++ :

```cpp
void TwoPhaseFlowSimulationModule::init()
{
  info() << "create physical solver";
  _initPhysicalSystem();

  info() << "create function manager";
  ArcRes::FluidSubSystem fluid = system().fluidSubSystem();

  if (fluid.phases().size() != 2)
    fatal() << "fluid-system should have exactly two phases";

  if (system().solidSubSystem().phases().size() != 0)
    fatal() << "solid-system should not be filled";

  _initGeometry();
  _initGroup();
  _initDomainVariableMng();
  if (options()->hasBoundaryCondition())
    _initBoundaryVariableMng();
  _initTransmissivity();
  if (options()->hasWellCondition())
    _initWellVariableMng();
  _initTimeVariableMng();
  _initLaws();
  _initRestore();
  _initEquationSystem();
  _initNewton();
}

void TwoPhaseFlowSimulationModule::compute()
{
  info() << "solve non linear problem at iteration " << m_global_iteration();
  bool r = options()->numerics().newton()->solve();
  if (!r) {
    throw ArcGeoSim::TimeStep::ReplayException("Error in computation - go backward!");
  }
}
```

Usage typique (côté cas `.arc`) :
- on configure le système ArcRes (fluide à deux phases, solide vide),
- on fournit les services `initial-condition`, `law`, `boundary-condition`, `well-condition` dans le bloc `<options>` du module,
- on lance `ShArc.exe` avec le fichier `.arc` correspondant (voir `test/TwoPhaseFlowSimulation`).

### Points d’entrée module thermo‑chimique

Même logique, avec un système fluide + solide (1 phase chacun) et 3 inconnues :
- pression,
- concentration,
- température.

Le `build()` assemble l’accumulation et les flux en tenant compte des propriétés thermiques (capacité, conductivité) et de la diffusion moléculaire.

## API / Interface publique

### Classes principales

| Symbole | Type | Description |
|---------|------|-------------|
| `TwoPhaseFlowSimulationModule` | classe | Module Arcane pour la simulation diphasique pression/saturation. Implémente `ArcNum::INonLinearSystem`. |
| `ThermoChemicalConvectionModule` | classe | Module Arcane pour la convection thermo‑chimique (P, C, T) couplée fluide/solide. Implémente `ArcNum::INonLinearSystem`. |
| `IPhysicalSystem` | interface | Interface pour fournir un `ArcRes::System` complet (fluide/solide, phases, propriétés). |
| `ISystemBuilder` | interface | Interface pour construire un système ArcRes (`buildSystem()`). |
| `IVariableManager` | interface | Interface pour gérer les `Law::VariableCellFolder` de domaine (`domain`, `domainTn`, `domainT0`). |

### Méthodes clés

| Symbole | Type | Description |
|---------|------|-------------|
| `void init()` | méthode de module | Initialise le système physique, la géométrie, les groupes, les variables de domaine/bord/puits, les lois, la restauration et le solveur Newton. |
| `void compute()` | méthode de module | Appelle le solveur Newton pour résoudre le système non linéaire sur le pas de temps courant, avec gestion de rollback (`ReplayException`) en cas d’échec. |
| `Integer nbEquations() const` | méthode | Retourne le nombre d’équations du système (nombre de phases ou variables couplées). |
| `Domain const& systemDomain() const` | méthode | Accès au domaine support de la discrétisation (mailles/faces) pour le système non linéaire. |
| `void initLinearSystemProfile(CellGroup, MatrixProfiler&, ConstArray2View<Integer>, Integer)` | méthode | Construit le profil de la matrice (topologie non nulle) via schéma deux‑points ou multi‑points. |
| `void setSolutionVariables(SharedArray<VariableCellReal*>&)` | méthode | Enregistre les variables de solution (pression, saturations, etc.) dans un tableau de variables Arcane, à partir des propriétés ArcRes. |
| `void build(Vector& residual, Matrix& jacobian)` | méthode | Construit le vecteur résidu et la matrice jacobienne (accumulation, flux internes, flux de bord, flux de puits, fermeture). |
| `void applyConstraintOnSolution(bool &NonPhysicalSolution)` | méthode (TwoPhase) | Projette les saturations dans [0,1] et renormalise pour que la somme soit 1, cellule par cellule. |
| `_initGeometry()` | méthode privée | Enregistre les propriétés géométriques (volume, centre, normale) dans `IGeometryMng` et force leur mise à jour. |
| `_initTransmissivity()` | méthode privée | Calcule les transmissivités (`TwoPointsTransmissivity`) à partir des perméabilités scalaire ou tensorielle. |
| `_buildAccumulation(...)` | méthode privée | Construit le terme d’accumulation en temps (rho·S, rho, concentration, température) avec `Δt`. |
| `_buildFluxInternal(...)` | méthode privée | Construit les flux internes (Darcy, gravité, advection, conduction, diffusion) entre mailles internes. |
| `_buildFluxBoundary(...)` | méthode privée | Construit les flux de bord à partir des données de `IBoundaryManager`. |
| `_buildFluxWell(...)` | méthode privée (TwoPhase) | Construit les flux puits à partir des indices de puits et des variables de puits. |
| `_buildClosure(...)` | méthode privée (TwoPhase) | Implémente l’équation de fermeture (somme des saturations − 1 = 0). |

### Lois physiques (helpers)

| Symbole | Type | Description |
|---------|------|-------------|
| `FluidDensityLaw` | classe | Relation densité fluide `rho(T,C)` avec dérivées `drho/dT`, `drho/dC`. |
| `RelativePermeabilityPowerLaw` | classe | Loi de perméabilité relative `kr(Sw)` avec paramètres `Swi`, `Sgc`, `α`. |
| `ConstantLaw` | classe | Loi constante paramétrable (valeur fixe et dérivées). |

## Architecture

```text
┌─────────────────────────────────────────────────────────┐
│             src/TwoPhaseFlowSimulation                  │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌──────────────────────┐   ┌────────────────────────┐  │
│  │ TwoPhaseFlowSimulation│   │ ThermoChemicalConv.   │  │
│  │      Module.cc        │   │      Module.cc        │  │
│  └──────────────────────┘   └────────────────────────┘  │
│           │                        │                    │
│           │                        │                    │
│           ▼                        ▼                    │
│  ┌──────────────────────┐   ┌────────────────────────┐  │
│  │   PhysicalSystem/    │   │   PhysicalLaws/        │  │
│  │ (IPhysicalSystem,    │   │ (CapillaryPressure,    │  │
│  │  ISystemBuilder,     │   │  FluidDensity, RelPerm │  │
│  │  IVariableManager,   │   │  ConstantLaw, Config   │  │
│  │  PhysicalModelModule)│   └────────────────────────┘  │
│  └──────────────────────┘              │                │
│           │                            │                │
│           ▼                            ▼                │
│  ┌──────────────────────┐   ┌────────────────────────┐  │
│  │ BoundaryCondition/   │   │ InitialCondition/      │  │
│  │ (DirichletManager,   │   │ (Constant, Expression  │  │
│  │  folders faces)      │   │  services)             │  │
│  └──────────────────────┘   └────────────────────────┘  │
│           │                            │                │
│           ▼                            ▼                │
│  ┌──────────────────────┐   ┌────────────────────────┐  │
│  │   WellCondition/     │   │   DataModel/           │  │
│  │ (WellManager,        │   │ (config.xml, gump      │  │
│  │  WellIndexComputer,  │   │  model ArcRes)         │  │
│  │  folders cellules)   │   └────────────────────────┘  │
│  └──────────────────────┘                               │
│                                                         │
└─────────────────────────────────────────────────────────┘
          │                     │                 │
          ▼                     ▼                 ▼
    ArcRes::System        ArcGeoSim Numerics   ArcGeoSim Law2
    (fluids/solid,        (TwoPointsScheme,    (FunctionManager,
     properties)           transmissivity,      evaluator, contributions)
                           geometry, groups)
```

- Les deux modules C++ encapsulent la logique de construction du système non linéaire et s’appuient sur :
  - `PhysicalSystem/` pour le modèle ArcRes.
  - `PhysicalLaws/` pour la configuration et l’évaluation des lois physiques.
  - `BoundaryCondition/`, `InitialCondition/`, `WellCondition/` pour les conditions du problème.
  - `DataModel/` pour le modèle Gump/ArcRes généré.

## Dépendances

| Dépendance | Version (indicative) | Usage |
|------------|----------------------|-------|
| Arcane 3 / ArcGeoSim | Arcane3 + ArcGeoSim (via `LoadArcGeoSim.cmake`) | Gestion maillage, géométrie, services, time loop, macros de module. |
| ArcRes | interne au framework Arcane/ArcGeoSim | Description des systèmes physiques (phases, propriétés, sous‑systèmes). |
| ArcNumTools | interne ArcGeoSim | Vecteurs, matrices, solveur Newton, profilage de système non linéaire. |
| Alien | bibliothèque externe (profilage matrice) | `Alien::MatrixProfiler` pour construire le profil de la matrice linéaire. |

## Configuration

La configuration se fait principalement via les fichiers `.axl` du module et les fichiers `.arc` de test (voir `test/TwoPhaseFlowSimulation`). Principales options (dans `TwoPhaseFlowSimulation.axl` et `ThermoChemicalConvection.axl`) :

| Variable / option | Défaut | Description |
|-------------------|--------|-------------|
| `permx-var-name`  | `PERMX` | Nom de la variable Arcane contenant la perméabilité X en entrée. |
| `permy-var-name`  | `PERMY` | Nom de la variable pour la perméabilité Y. |
| `permz-var-name`  | `PERMZ` | Nom de la variable pour la perméabilité Z. |
| `scalar-perm`     | `true` | Si vrai, utilise une perméabilité scalaire `Permeability`. Sinon, reconstruit un tenseur à partir de `PERMX`, `PERMY`, `PERMZ`. |
| `group-creator`   | `StandardGroupCreator` | Service `IGroupCreator` qui construit les groupes de mailles/faces utilisés pour la discrétisation. |
| `numerics.two-points-scheme` | `DivKGradTwoPoints` | Schéma deux‑points utilisé pour calculer les transmissivités. |
| `numerics.newton` | (service à fournir) | Implémentation du solveur non linéaire `ArcNum::INewtonSolver`. |
| `numerics.nltpfa` | `false` | Active/désactive le schéma NLTPFA (non‑linear two‑points flux approximation). |
| `initial-condition` | minOccurs=1 | Services de condition initiale (pression, saturations, concentration, température). |
| `law`             | minOccurs=0 | Services `ILawConfigurator` pour les lois physiques (capillary pressure, density, relative permeability, etc.). |
| `boundary-condition` | optionnel | Service `IBoundaryManager` décrivant les conditions de bord (Dirichlet, etc.). |
| `well-condition`  | optionnel | Service `IWellManager` décrivant les puits (débits, indices, variables de puits). |

Les fichiers `.arc` de test (sous `test/TwoPhaseFlowSimulation/`) instancient ces services et précisent le maillage (SPE10, cases de puits, etc.) ainsi que les paramètres physiques.

## Ressources

*(liens indicatifs, à adapter selon la documentation interne)*

- Documentation Arcane / ArcGeoSim (framework de simulation et infrastructure) — pour comprendre :
  - la notion de module Arcane (`*.axl`, `ARCANE_REGISTER_MODULE_*`),
  - la gestion des services (`AppService`),
  - les outils numériques (`TwoPointsScheme`, `INewtonSolver`).
- Documentation ArcRes — description des entités `System`, `FluidSubSystem`, `SolidSubSystem` et des propriétés physiques (`Pressure`, `Saturation`, `Density`, etc.).
- Cas de test ShArc :
  - `test/TwoPhaseFlowSimulation/` — configuration des cas SPE10, puits, lois physiques.
  - `test/LargeScaleTwoPhaseFlowSimulation/README.md` — exemple de cas à grande échelle (dimensions de grille, pattern `__NPX__x__NPY__x__NPZ__-__NP__p`).
