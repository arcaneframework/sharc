# TwoPhaseFlowSimulation

> 🧠 Documentation enrichie par IA — basée sur l'analyse du code source et la structure du projet

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

Le module `TwoPhaseFlowSimulation` implémente une simulation diphasique (deux phases fluide) en milieu poreux, en s'appuyant sur le cadre ArcGeoSim/Arcane et sur un modèle Gump (`ArcRes::System`) pour la description du système physique.

Il fournit un système non linéaire complet pour un schéma de type volumes finis à deux points, avec assemblage des termes d'accumulation, des flux internes entre cellules, des flux aux frontières ainsi que des contributions des puits (sources/sinks). Les lois physiques (pression capillaire, perméabilité relative, densité, viscosité) sont modularisées dans `PhysicalLaws/` et configurées par des services indépendants.

Le point d'entrée principal est la classe `TwoPhaseFlowSimulationModule`, qui dérive de `ArcaneTwoPhaseFlowSimulationObject` et de `ArcNum::INonLinearSystem`. Elle coordonne :
- la construction du système physique via `PhysicalModelModule`,
- la gestion des variables de domaine (`IVariableManager`),
- les conditions initiales (`InitialCondition/`),
- les conditions aux limites de type Dirichlet (`BoundaryCondition/Dirichlet/`),
- les puits (`WellCondition/Well/`),
- la résolution Newton non linéaire.

## Fonctionnalités

- ✅ Simulation de flux diphasique avec deux phases fluide (pression + saturations).
- ✅ Construction automatique du système physique `ArcRes::System` et des domaines de variables (`PhysicalSystem/PhysicalModelModule.cc`).
- ✅ Assemblage du système non linéaire via `ArcNum::INonLinearSystem` :
  - `_buildAccumulation(...)` : terme d'accumulation masse fluide.
  - `_buildFluxInternal(...)` : flux interne entre cellules (schéma à deux points, transmissivités).
  - `_buildFluxBoundary(...)` : flux sur les frontières avec conditions de type Dirichlet.
  - `_buildFluxWell(...)` : flux induits par les puits producteurs/injecteurs.
  - `_buildClosure(...)` : fermeture ΣS = 1 sur les saturations.
- ✅ Gestion cohérente des variables et domaines :
  - `domain()`, `domainT0()`, `domainTn()` pour les états courant, initial, précédent.
  - enregistrement et restauration des variables en cas d'échec du solveur Newton (`ReplayException`).
- ✅ Gestion des conditions initiales :
  - `InitialCondition/Constant/ConstantService.cc` pour des champs constants (via expression en fonction de x,y,z).
  - `InitialCondition/Expression/ExpressionService.cc` pour des champs définis par expressions analytiques.
- ✅ Gestion des conditions aux limites :
  - `BoundaryCondition/Dirichlet/DirichletManagerService.cc` pour des conditions :
    - simple (une propriété / une valeur),
    - multi-propriété,
    - space-time (propriété fonction de (x,y,z,t)).
- ✅ Gestion des puits (`WellCondition/Well/WellManagerService.cc`) :
  - calcul des indices de puits (`WellIndexComputer`) pour des puits de type pression,
  - puits de type débit (FlowRate) dépendant du volume perforé.
- ✅ Lois physiques paramétrables :
  - `CapillaryPressureLaw` : Pc = Pe·Se^{-1/λ}, avec calcul de Se(S) et dérivées.
  - `RelativePermeabilityPowerLaw` : krw(Sw) loi puissance, avec seuils Sw_i, S_gc.
  - `FluidDensityLaw`, `ConstantLaw` pour densités et propriétés constantes.
- ✅ Profilage et assemblage du système linéaire :
  - `_twoPointsProfiler(...)` et `_multiPointsProfiler(...)` pour construire le profil de matrice Alien.

## Prérequis

- Langage : C++17 (via Arcane/ArcGeoSim).
- Frameworks :
  - Arcane (module ShArc, services, gestion de maillage, exécution parallèle).
  - ArcGeoSim (numérique : transmissivités, solveurs Newton, expressions).
  - ArcRes (modèle Gump, propriétés physiques, phases).
- Solveur linéaire/non linéaire :
  - Interface `ArcNum::INonLinearSystem` et `INewtonSolver` (fournis par ArcGeoSim).
  - Backends linéaires via Alien (profilage de matrice, vecteurs, etc.).
- Côté configuration :
  - Fichiers `.axl` et `.xml` correctement présents dans `src/TwoPhaseFlowSimulation/` et `test/TwoPhaseFlowSimulation/`.
  - ArcGeoSim/Arcane configurés et compilés (voir README racine du projet).

## Installation

Ce module est intégré au projet ShArc via CMake et les macros ArcGeoSim. Aucun CMakeLists dédié n'existe ici : l'intégration se fait depuis `src/CMakeLists.txt`, qui déclare la librairie `ShArc` et génère les modèles Gump.

Exemple schématique de configuration (voir README racine pour les commandes complètes) :

```bash
# Depuis la racine du projet sharc
cmake -S . -B build-sharc \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DARCANEFRAMEWORK_ROOT=${INSTALL_DIR} \
  -DCMAKE_C_COMPILER=mpicc \
  -DCMAKE_CXX_COMPILER=mpic++

cmake --build build-sharc
```

Une fois la librairie `ShArc` construite, le module `TwoPhaseFlowSimulation` est disponible via les cas de test dans `test/TwoPhaseFlowSimulation/` (fichiers `.arc`).

## Utilisation

### Scénario de haut niveau

1. **Choisir un cas de test** dans `test/TwoPhaseFlowSimulation/` (par ex. SPE10, cas de validation, variantes solveurs PETSc/Hypre/IFPS/MCGS).
2. **Lancer `ShArc.exe`** avec le fichier `.arc` correspondant :
   - le module `PhysicalModel` construit le système Gump (`ArcRes::System`),
   - `TwoPhaseFlowSimulationModule` initialise la géométrie, les groupes, les variables, les lois, les conditions initiales/limites/puits,
   - le solveur Newton est initialisé puis appelé via `compute()` à chaque pas de temps.

### Cycle de vie dans `TwoPhaseFlowSimulationModule`

Extrait simplifié :

```cpp
class TwoPhaseFlowSimulationModule
  : public ArcaneTwoPhaseFlowSimulationObject
  , public ArcNum::INonLinearSystem
{
public:
  void init();
  void compute();

  Integer nbEquations() const;
  Domain const& systemDomain() const;

  void initLinearSystemProfile(CellGroup cells,
                               Alien::MatrixProfiler& blockProfiler,
                               ConstArray2View<Integer> indexes,
                               Integer block_size);

  void setSolutionVariables(SharedArray<VariableCellReal*>& solutions);

  const Law::PropertyVector& equationSystem() const;
  void build(ArcNum::Vector& residual, ArcNum::Matrix& jacobian);
  void applyConstraintOnSolution(bool &NonPhysicalSolution);
};
```

Usage typique (côté framework) :
- `init()` est appelé une fois pour :
  - construire le système physique,
  - initialiser géométrie, groupes, variables, lois, Newton.
- `compute()` est appelé à chaque pas de temps :
  - le solveur Newton résout le système non linéaire,
  - une `ReplayException` est levée en cas d'échec, pour revenir en arrière.

### Initialisation d'un cas

Les conditions initiales et limites sont définies dans les fichiers `.arc` via les services :

- `InitialCondition/Constant` et `InitialCondition/Expression`
- `BoundaryCondition/Dirichlet`
- `WellCondition/Well`

Exemple d'intention (pseudo `.arc`) :

```xml
<two-phase-flow-simulation>
  <physical-model system="MySystemDefinition" />
  <initial-condition type="constant" property="Pressure" value="1e7" />
  <initial-condition type="expression" property="Saturation"
                     value="(x,y,z)->0.2 + 0.1*x" />
  <boundary-condition type="dirichlet" group="BoundaryFaces"
                      property="Pressure" value="(x,y,z;t)->1e7" />
  <well group="InjectionCells" flowRate="(t)->1e-3" radius="0.1" skin="0.0" />
  <numerics>
    <newton .../>
    <two-points-scheme .../>
  </numerics>
</two-phase-flow-simulation>
```

## API / Interface publique

Principaux symboles exportés (niveau module/Services, côté code C++) :

| Symbole | Type | Description |
|---------|------|-------------|
| `TwoPhaseFlowSimulationModule::init()` | méthode de module | Initialise le solveur physique : système Gump, géométrie, groupes, variables de domaine, conditions aux limites/puits, lois physiques, équations et solveur de Newton. |
| `TwoPhaseFlowSimulationModule::compute()` | méthode de module | Résout le système non linéaire pour le pas de temps courant via le solveur de Newton, avec gestion de rollback (`ReplayException`) en cas d'échec. |
| `TwoPhaseFlowSimulationModule::build(residual, jacobian)` | méthode `INonLinearSystem` | Assemble le résidu et la matrice jacobienne : accumulation, flux internes, flux frontières, flux puits, fermeture ΣS=1. |
| `TwoPhaseFlowSimulationModule::applyConstraintOnSolution(...)` | méthode `INonLinearSystem` | Applique des contraintes sur les saturations (bornes [0,1], renormalisation pour ΣS=1). |
| `TwoPhaseFlowSimulationModule::initLinearSystemProfile(...)` | méthode `INonLinearSystem` | Construit le profil de matrice (structure de sparsité) pour Alien, via schémas 2 points ou multi points. |
| `TwoPhaseFlowSimulationModule::setSolutionVariables(...)` | méthode `INonLinearSystem` | Remplit la liste de variables solution (`VariableCellReal`) à partir des propriétés inconnues (`m_unknown_manager`). |
| `PhysicalModelModule::init()` | méthode de module | Construit le système physique `ArcRes::System` à partir des options XML/AXL, et initialise les domaines de variables `Domain`, `DomainTN`, `DomainT0`. |
| `PhysicalModelModule::system()` | méthode de module | Retourne le système physique Gump (`ArcRes::System`), avec vérification d'initialisation. |
| `DirichletManagerService::init(...)` | service | Crée et initialise les conteneurs de variables de frontière, enregistre les conditions Dirichlet (simple, multi, space-time). |
| `DirichletManagerService::update(...)` | service | Réévalue les conditions Dirichlet (notamment dépendantes de t). |
| `WellManagerService::init(...)` | service | Crée les conteneurs de puits, calcule les indices de puits et remplit les conditions limite associées. |
| `WellManagerService::update(...)` | service | Met à jour les conditions de puits et les débits en fonction du temps et du volume perforé. |
| `ConstantService::init(...)` | service | Applique une condition initiale constante (via expression en fonction de la position) sur une propriété donnée. |
| `ExpressionService::init(...)` | service | Applique une condition initiale définie par une expression paramétrée (service `condition`), en fonction de la position. |

## Architecture

```text
┌───────────────────────────────────────────────────────────────┐
│                 TwoPhaseFlowSimulation                       │
├───────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌────────────────────┐    ┌─────────────────────────┐        │
│  │ PhysicalSystem     │    │ PhysicalLaws            │        │
│  │ (PhysicalModel)    │    │ (Capillary, krw, rho…)  │        │
│  └────────────────────┘    └─────────────────────────┘        │
│          │                          │                         │
│          ▼                          ▼                         │
│  ┌────────────────────┐    ┌─────────────────────────┐        │
│  │ InitialCondition   │    │ BoundaryCondition       │        │
│  │ (Constant/Expr)    │    │ (DirichletManager)      │        │
│  └────────────────────┘    └─────────────────────────┘        │
│          │                          │                         │
│          ▼                          ▼                         │
│  ┌────────────────────┐    ┌─────────────────────────┐        │
│  │ WellCondition      │    │ TwoPhaseFlowSimulation  │        │
│  │ (WellManager)      │    │ Module (INonLinearSystem│        │
│  └────────────────────┘    └─────────────────────────┘        │
│             │                       │                         │
│             ▼                       ▼                         │
│     ┌───────────────┐        ┌───────────────┐                │
│     │ GeometryMng   │        │ NewtonSolver  │                │
│     │ (ArcGeoSim)   │        │ (ArcGeoSim)   │                │
│     └───────────────┘        └───────────────┘                │
│                                                               │
└───────────────────────────────────────────────────────────────┘
         │                        │
         ▼                        ▼
   ArcRes::System           Alien / ArcNum
 (phases, propriétés)   (vecteurs, matrices, profil)
```

Flux de données (simplifié) :
- `PhysicalModelModule` construit `ArcRes::System` et les domaines `Domain`, `DomainT0`, `DomainTn`.
- `TwoPhaseFlowSimulationModule::init()` :
  - enregistre la géométrie via `IGeometryMng` (volume, centre, normale),
  - initialise les groupes (GroupCreator),
  - crée les conteneurs de variables (`IVariableManager`),
  - appelle les services d'IC/BC/Wells pour remplir les conditions,
  - configure les lois via `ILawConfigurator` et prépare `FunctionManager`.
- `build()` assemble le système non linéaire à chaque pas, en requêtant les contributions des lois physiques et des conditions BC/Wells.

## Dépendances

| Dépendance | Version (indicative) | Usage |
|------------|----------------------|-------|
| Arcane | ≥ 3.x (voir README racine) | Framework de base (modules, services, maillage, exécution parallèle). |
| ArcGeoSim | dépend de l'installation | Numérique (transmissivités, solveur Newton, expressions, AppService). |
| ArcRes | interne ArcGeoSim/Arcane | Modèle Gump : système physique, propriétés, phases. |
| Alien | backend linéaire | Profilage de matrice, vecteurs, jacobien (via `Matrix`, `Vector`, `MatrixProfiler`). |

Au niveau code, on voit notamment :
- `ArcGeoSim/Numerics/DiscreteOperator/IDivKGradDiscreteOperator.h`
- `ArcGeoSim/Numerics/NumericalScheme/TwoPointsScheme/TwoPointsTransmissivity.h`
- `ArcGeoSim/Numerics/ArcNumTools/Solver/INewtonSolver.h`
- `ArcGeoSim/Mesh/Geometry/IGeometryMng.h`
- `ArcGeoSim/Appli/AppService.h`
- `ArcRes/Entities/System.h`, `ArcRes/Properties.h`.

## Configuration

Les variables de configuration et options sont principalement définies dans les fichiers AXL/XML :

- `TwoPhaseFlowSimulation.axl`
- `ThermoChemicalConvection.axl`
- `PhysicalSystem/PhysicalModel.axl`
- `InitialCondition/*/*.axl`
- `BoundaryCondition/Dirichlet/DirichletManager.axl`
- `WellCondition/Well/WellManager.axl`

Exemples de paramètres (conceptuels) :

| Variable / Option | Défaut / Exemple | Description |
|-------------------|------------------|-------------|
| `fluid.phases().size()` | 2 | Le module vérifie qu'il y a exactement deux phases fluide. |
| `solidSubSystem.phases().size()` | 0 | Doit être nul (pas de phases solides remplies). |
| `scalarPerm` | booléen | Contrôle l'utilisation de perméabilité scalaire ou anisotrope (Kx,Ky,Kz) pour les transmissivités. |
| `permxVarName`, `permyVarName`, `permzVarName` | noms de variables maillage | Noms des variables de perméabilité en entrée (maillage). |
| `initialCondition[...]` | liste d'IC | Chaque IC est un service (Constant, Expression) avec propriété et expression associée. |
| `boundaryCondition.boundary[...]` | liste de BC | Conditions de type Dirichlet (single, multi, space-time) avec propriétés et valeurs (expression). |
| `wellCondition.well[...]` | liste de puits | Puits de type pression ou débit, avec groupe de cellules, rayon, skin, etc. |
| `numerics.newton` | bloc solveur | Paramètres du solveur de Newton (tolérances, itérations max, etc.). |

Les valeurs exactes et leur syntaxe sont à lire dans les AXL/XML et les cas `.arc` de `test/TwoPhaseFlowSimulation/`.

## Ressources

- [Documentation Arcane](https://arcaneframework.github.io) — description du framework, modules, services.
- [ArcGeoSim (GitHub)](https://github.com/arcaneframework/ArcGeoSim) — framework numérique utilisé pour les lois, solveurs et géométrie.
- [ShArc (GitHub)](https://github.com/arcaneframework/sharc) — dépôt principal du projet, avec README d'installation détaillé.
- [Alien (GitHub)](https://github.com/arcaneframework/alien) — backend de structures de données linéaires pour Arcane.
- Cas de test two-phase dans `test/TwoPhaseFlowSimulation/` — exemples concrets de fichiers `.arc` configurant ce module pour différents solveurs et grilles.
